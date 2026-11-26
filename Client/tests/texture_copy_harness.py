"""Production texture copy/lock/merge paths against exact-size mock RHI buffers."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r'/\*[\s\S]*?\*/|//[^\n]*', '', source(path))

    parts = [r'''
#include <functional>
namespace TextureCopyTests {
using SIZE_T = size_t;
using DWORD = UINT;
using TCHAR = char;
template<class T> struct TArray : TextureMergeSafetyTests::TArray<T> {
    void RemoveSingleItemSwap(const T& value) {
        auto it = std::find(this->values.begin(), this->values.end(), value);
        assert(it != this->values.end()); this->values.erase(it);
    }
};
template<class T> T Min(T a, T b) { return a < b ? a : b; }
template<class T> T Max(T a, T b) { return a > b ? a : b; }
template<class T> T Clamp(T n, T a, T b) { return Max(a, Min(n, b)); }
template<class... Args> void debugf(Args...) {}
bool appMemIsZero(const void* p, size_t size) {
    auto* b = static_cast<const BYTE*>(p);
    for (size_t i = 0; i < size; ++i) if (b[i]) return false;
    return true;
}
enum { PF_DXT1 };
struct PixelFormat { INT BlockSizeX, BlockSizeY, BlockBytes; };
PixelFormat GPixelFormats[] = {{4, 4, 8}};
struct Settings { INT drop = 0; INT GetNumMipsToDrop() const { return drop; } } GSystemSettings;
struct Surface {
    struct Mip {
        UINT row_bytes, stride;
        INT rows;
        std::vector<BYTE> data;
        bool locked = false;
        Mip(INT w, INT h, UINT padding)
            : row_bytes(((w + 3) / 4) * 8), stride(row_bytes + padding), rows((h + 3) / 4),
              data(size_t(stride) * rows, 0xdd) {}
    };
    std::vector<Mip> mips;
    bool fail = false, short_stride = false, omit_stride = false;
    INT locks = 0, unlocks = 0;
    Surface(INT width, INT height, INT count, UINT padding) {
        for (INT i = 0; i < count; ++i) mips.emplace_back(Max(1, width >> i), Max(1, height >> i), padding);
    }
    void CheckUnlocked() const { for (auto& mip : mips) assert(!mip.locked); }
};
using FTexture2DRHIParamRef = Surface*;
BYTE* RHILockTexture2D(Surface* surface, INT index, UBOOL, UINT& stride, UBOOL) {
    assert(surface && index >= 0 && index < (INT)surface->mips.size());
    ++surface->locks;
    auto& mip = surface->mips[index];
    assert(!mip.locked);
    if (surface->fail) return nullptr;
    mip.locked = true;
    if (!surface->omit_stride) stride = surface->short_stride ? mip.row_bytes - 1 : mip.stride;
    return mip.data.data();
}
void RHIUnlockTexture2D(Surface* surface, INT index, UBOOL) {
    assert(surface && index >= 0 && index < (INT)surface->mips.size());
    assert(surface->mips[index].locked);
    surface->mips[index].locked = false; ++surface->unlocks;
}
struct FTexture2DResource {
    Surface* surface = nullptr;
    Surface* GetTexture2DRHI() { return surface; }
};
struct FTexture2DDynamicResource : FTexture2DResource {};
struct UTexture2D {
    INT SizeX = 0, SizeY = 0, Format = PF_DXT1, ResidentMips = 0;
    TArray<INT> Mips;
    FTexture2DResource* Resource = nullptr;
    bool loaded = true;
    bool IsFullyStreamedIn() const { return loaded; }
};
struct UTexture2DDynamic : UTexture2D { INT NumMips = 0; };
struct FEventWin {
    bool signalled = false;
    void Create() {}
    void Trigger() { signalled = true; }
    bool Wait(INT) { bool result = signalled; signalled = false; return result; }
};
struct FNonAbandonableTask {};
std::vector<std::function<void()>> commands;
#pragma push_macro("ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER")
#undef ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER
#define ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(Name, Type, Var, Value, Code) \
    do { Type Var = Value; commands.emplace_back([Var]() Code); } while (0)
''']
    header = clean(game.parent / "Engine/Inc/UnTextureMerge.h")
    parts.append(function(header, "struct FTextureMergeRegion\n") + ";")
    parts.append("using TextureMergeList = TArray<FTextureMergeRegion>;")
    parts.append(function(header, "struct FTextureLockData\n") + ";")
    parts.append("using TextureLockDataList = TArray<FTextureLockData*>;")
    parts.append(function(header, "class FTextureMergeUtil\n").replace("private:", "public:") + ";")
    parts.append(function(header, "class FTextureMergeTask :") + ";")
    parts.append(r'''
template<class T> struct FAutoDeleteAsyncTask {
    T task;
    explicit FAutoDeleteAsyncTask(FTextureMergeUtil* utility) : task(utility) {}
    void StartBackgroundTask() { task.DoWork(); delete this; }
};
TextureLockDataList FTextureMergeUtil::LockData_D, FTextureMergeUtil::LockData_N, FTextureMergeUtil::LockData_S;
UINT FTextureMergeUtil::CurrentRunningThreadNum = 0;
struct FBlendTextureRegion2D {};
using TextureRHIList = TArray<FTexture2DRHIParamRef>;
''')
    cpp = clean(game.parent / "Engine/Src/UnTextureMerge.cpp")
    parts.append(function(cpp, "struct DXT1Tile\n") + ";")
    parts.append(r'''
// Preserve exact tile reads/writes while avoiding unrelated compression math.
INT blends = 0;
void BlendTile_CPU(const DXT1Tile* sd, DXT1Tile* dd, const DXT1Tile* sn, DXT1Tile* dn,
                   const DXT1Tile* ss, DXT1Tile* ds) {
    ++blends; memcpy(dd, sd, sizeof(*dd)); memcpy(dn, sn, sizeof(*dn)); memcpy(ds, ss, sizeof(*ds));
}
''')
    for signature in [
        "static UBOOL GetTextureMergeBufferLayout(", "static void GetTextureMergeCopyBounds(",
        "FTextureMergeUtil::FTextureMergeUtil()", "FTextureMergeUtil::~FTextureMergeUtil()",
        "UBOOL FTextureMergeUtil::Validate()", "void FTextureMergeUtil::StartNewMerge(",
        "void FTextureMergeUtil::CancelCurrentMerge()", "UBOOL FTextureMergeUtil::DoMerge()",
        "void FTextureMergeUtil::RenderThread_BlendRegions()", "void FTextureMergeUtil::RenderThread_StartMerge()",
        "UBOOL FTextureMergeUtil::LockTargetTextureData()", "UBOOL FTextureMergeUtil::GetScrDataFromTex(",
        "void FTextureMergeUtil::RenderThread_MergeOneTexBegin()", "void FTextureMergeUtil::StartMergingThread()",
        "void FTextureMergeUtil::CheckLockDataRefNum(", "void FTextureMergeUtil::RenderThread_MergeOneTexEnd()",
        "void FTextureMergeUtil::RenderThread_FinishMerge()", "void FTextureMergeUtil::BackgroundThread_MergeTextures()",
        "void FTextureMergeUtil::UpdateState()", "FTextureLockData* FTextureMergeUtil::GetLockData_D(",
        "FTextureLockData* FTextureMergeUtil::GetLockData_N(", "FTextureLockData* FTextureMergeUtil::GetLockData_S(",
    ]:
        parts.append(function(cpp, signature))
    parts.append(r'''
#pragma pop_macro("ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER")
struct Image {
    Surface surface;
    FTexture2DDynamicResource resource;
    UTexture2DDynamic texture;
    Image(INT w, INT h, INT count, UINT padding, INT first = 0)
        : surface(Max(1, w >> first), Max(1, h >> first), count - first, padding) {
        resource.surface = &surface; texture.Resource = &resource;
        texture.SizeX = w; texture.SizeY = h; texture.Mips.values.resize(count);
        texture.ResidentMips = count - first; texture.NumMips = count;
    }
    void Fill(BYTE seed) {
        for (auto& mip : surface.mips)
            for (INT row = 0; row < mip.rows; ++row)
                for (UINT col = 0; col < mip.row_bytes; ++col)
                    mip.data[row * mip.stride + col] = BYTE((seed + row * 7 + col) % 251);
    }
    void CheckPadding() {
        surface.CheckUnlocked();
        for (auto& mip : surface.mips)
            for (INT row = 0; row < mip.rows; ++row)
                for (UINT col = mip.row_bytes; col < mip.stride; ++col)
                    assert(mip.data[row * mip.stride + col] == 0xdd);
    }
};
void Pump(FTextureMergeUtil& merge) {
    for (INT n = 0; n < 1000; ++n) {
        if (!commands.empty()) {
            auto command = std::move(commands.front()); commands.erase(commands.begin()); command();
        }
        merge.UpdateState();
        if (commands.empty() && merge.MergeFinished()) break;
    }
    assert(commands.empty() && merge.MergeFinished() && !merge.IsDoingMerge());
    assert(FTextureMergeUtil::CurrentRunningThreadNum == 0);
    assert(!FTextureMergeUtil::LockData_D.Num() && !FTextureMergeUtil::LockData_N.Num() && !FTextureMergeUtil::LockData_S.Num());
}
void Assign(FTextureMergeUtil& merge, Image& d, Image& n, Image& s, Image& td, Image& tn, Image& ts,
            FTextureMergeRegion region, INT layers) {
    for (INT i = 0; i < layers; ++i) region.Textures.AddItem(&d.texture);
    merge.SourceRegions_D.AddItem(region);
    region.Textures.values.assign(layers, &n.texture); merge.SourceRegions_N.AddItem(region);
    region.Textures.values.assign(layers, &s.texture); merge.SourceRegions_S.AddItem(region);
    merge.TargetTexture_D = &td.texture; merge.TargetTexture_N = &tn.texture; merge.TargetTexture_S = &ts.texture;
}
void Scenario(FTextureMergeRegion region, INT sx, INT sy, INT dx, INT dy, INT width, INT height, bool direct) {
    GSystemSettings.drop = 0;
    Image d(32, 64, 1, 5), n(32, 64, 1, 13), s(32, 64, 1, 21);
    Image td(16, 16, 1, 3), tn(16, 16, 1, 11), ts(16, 16, 1, 19);
    d.Fill(11); n.Fill(41); s.Fill(71);
    FTextureMergeUtil merge;
    Assign(merge, d, n, s, td, tn, ts, region, 2);
    assert(merge.Validate());
    merge.StartNewMerge(!direct); Pump(merge);
    auto check_image = [&](Image& source, Image& target) {
        const auto& input = source.surface.mips[0];
        const auto& output = target.surface.mips[0];
        for (INT row = 0; row < output.rows; ++row)
            for (UINT col = 0; col < output.row_bytes; ++col) {
                BYTE expected = 0;
                if (row >= dy / 4 && row < (dy + height) / 4 && col >= UINT(dx / 4 * 8) && col < UINT((dx + width) / 4 * 8))
                    expected = input.data[(row - dy / 4 + sy / 4) * input.stride + col - dx / 4 * 8 + sx / 4 * 8];
                assert(output.data[row * output.stride + col] == expected);
            }
        source.CheckPadding(); target.CheckPadding();
    };
    check_image(d, td); check_image(n, tn); check_image(s, ts);
}
void Run() {
    for (bool direct : {false, true}) {
        Scenario({4, 28, 8, 8, 4, 4, {}}, 4, 28, 4, 4, 8, 8, direct); // Full-source cache, offset subregion.
        Scenario({28, 60, 32, 64, 12, 12, {}}, 28, 60, 12, 12, 4, 4, direct); // Both edges.
        Scenario({0, 0, -1, -1, 0, 0, {}}, 0, 0, 0, 0, 16, 16, direct); // Full copy to smaller target.
        Scenario({0, 0, 2147483647, 2147483647, 0, 0, {}}, 0, 0, 0, 0, 16, 16, direct);
        Scenario({0, 0, 0, 8, 0, 0, {}}, 0, 0, 0, 0, 0, 8, direct);
        Scenario({28, 60, -1, -1, -1, -1, {}}, 28, 60, 12, 12, 4, 4, direct);
    }
    FTextureMergeRegion region{8, 16, 16, 24, 16, 8, {}};
    INT sx, sy, dx, dy, w, h;
    GetTextureMergeCopyBounds(region, 2, 16, 16, 16, 16, 4, 4, sx, sy, dx, dy, w, h);
    assert(sx == 0 && sy == 4 && dx == 4 && dy == 0 && w == 4 && h == 8);
    for (INT mip : {-1, 31, 2147483647}) {
        GetTextureMergeCopyBounds(region, mip, 16, 16, 16, 16, 4, 4, sx, sy, dx, dy, w, h);
        assert(!sx && !sy && !dx && !dy && !w && !h);
    }
    const INT edge[] = {-2147483647 - 1, -1, 0, 1, 3, 4, 15, 16, 2147483647};
    for (INT offset : edge) for (INT size : edge) for (INT target : edge) {
        FTextureMergeRegion value{offset, offset, size, size, target, target, {}};
        GetTextureMergeCopyBounds(value, 0, 32, 64, 16, 16, 4, 4, sx, sy, dx, dy, w, h);
        assert(sx >= 0 && sy >= 0 && dx >= 0 && dy >= 0 && w >= 0 && h >= 0);
        assert(sx + w <= 32 && sy + h <= 64 && dx + w <= 16 && dy + h <= 16);
        for (INT v : {sx, sy, dx, dy, w, h}) assert(v % 4 == 0);
    }
    UINT pitch;
    SIZE_T rows;
    assert(GetTextureMergeBufferLayout(1, 1, pitch, rows) && pitch == 8 && rows == 1);
    assert(!GetTextureMergeBufferLayout(0, 1, pitch, rows));
    assert(!GetTextureMergeBufferLayout(2147483647, 2147483647, pitch, rows));

    {
        GSystemSettings.drop = 0;
        Image d(16, 16, 3, 0), n(16, 16, 3, 0), s(16, 16, 3, 0);
        Image td(16, 16, 2, 0), tn(16, 16, 2, 0), ts(16, 16, 2, 0);
        FTextureMergeUtil merge;
        Assign(merge, d, n, s, td, tn, ts, {0, 0, 16, 16, 0, 0, {}}, 1);
        assert(merge.Validate());
        for (INT invalid : {-1, 10, 31, 2147483647}) {
            GSystemSettings.drop = invalid; assert(!merge.DoMerge() && commands.empty());
        }
        GSystemSettings.drop = 0;
        for (INT invalid : {0, 10, 2147483647}) {
            td.texture.NumMips = invalid; assert(!merge.DoMerge() && commands.empty());
        }
        td.texture.NumMips = 2;
        tn.texture.SizeX = 8; assert(!merge.Validate()); tn.texture.SizeX = 16;
        auto* saved = d.texture.Resource;
        d.texture.Resource = nullptr; assert(!merge.Validate()); d.texture.Resource = saved;
        n.texture.loaded = false; assert(!merge.Validate()); n.texture.loaded = true;
        s.texture.ResidentMips = 4; assert(!merge.Validate()); s.texture.ResidentMips = 3;
        s.texture.ResidentMips = 1; assert(!merge.Validate()); s.texture.ResidentMips = 3;
        n.texture.SizeY = 8; assert(!merge.Validate()); n.texture.SizeY = 16;
        merge.SourceRegions_N(0).Textures(0) = nullptr; assert(!merge.Validate());
        merge.SourceRegions_N(0).Textures(0) = &n.texture;
        merge.SourceRegions_N(0).Textures.AddItem(&n.texture); assert(!merge.Validate());
        merge.SourceRegions_N(0).Textures.values.pop_back();
        assert(merge.Validate());
        for (auto* regions : {&merge.SourceRegions_D, &merge.SourceRegions_N, &merge.SourceRegions_S})
            regions->values.insert(regions->values.begin(), FTextureMergeRegion{0, 0, 0, 0, 0, 0, {}});
        assert(merge.DoMerge()); Pump(merge); // Optional empty first region must be skipped safely.
    }

    for (bool direct : {false, true}) for (INT drop : {0, 1, 2}) {
        GSystemSettings.drop = drop;
        Image d(32, 32, 6, 3, drop), n(32, 32, 6, 9, drop), s(32, 32, 6, 15, drop);
        Image td(32 >> drop, 32 >> drop, 3, 7), tn(32 >> drop, 32 >> drop, 3, 17), ts(32 >> drop, 32 >> drop, 3, 27);
        d.Fill(11); n.Fill(41); s.Fill(71);
        FTextureMergeUtil merge;
        Assign(merge, d, n, s, td, tn, ts, {0, 0, -1, -1, 0, 0, {}}, 2);
        if (direct) { assert(merge.Validate()); merge.StartNewMerge(FALSE); }
        else assert(merge.DoMerge());
        GSystemSettings.drop = 9; // Settings changes must not alter a queued merge's mip base.
        merge.TargetTexture_D = nullptr; merge.TargetTexture_N = nullptr; merge.TargetTexture_S = nullptr;
        Pump(merge);
        auto equal_mips = [&](Image& source, Image& target) {
            for (INT mip = 0; mip < 3; ++mip) {
                auto& input = source.surface.mips[mip]; auto& output = target.surface.mips[mip];
                for (INT row = 0; row < output.rows; ++row)
                    assert(!memcmp(&input.data[row * input.stride], &output.data[row * output.stride], output.row_bytes));
            }
            source.CheckPadding(); target.CheckPadding();
        };
        equal_mips(d, td); equal_mips(n, tn); equal_mips(s, ts);
    }
    for (bool direct : {false, true}) for (INT channel = 0; channel < 6; ++channel) for (INT failure = 0; failure < 3; ++failure) {
        GSystemSettings.drop = 0;
        Image d(16, 16, 1, 3), n(16, 16, 1, 5), s(16, 16, 1, 7);
        Image td(16, 16, 1, 9), tn(16, 16, 1, 11), ts(16, 16, 1, 13);
        Surface* surfaces[] = {&d.surface, &n.surface, &s.surface, &td.surface, &tn.surface, &ts.surface};
        auto* failed = surfaces[channel];
        failed->fail = failure == 0; failed->short_stride = failure == 1; failed->omit_stride = failure == 2;
        FTextureMergeUtil merge;
        Assign(merge, d, n, s, td, tn, ts, {0, 0, 16, 16, 0, 0, {}}, 1);
        assert(merge.Validate());
        merge.StartNewMerge(!direct); Pump(merge);
        assert(merge.bCancelCurrentMerge);
        for (auto* surface : surfaces) surface->CheckUnlocked();
    }
    GSystemSettings.drop = 0;
    assert(blends > 0);
    puts("Texture copy paths clip offset regions, cache full padded source mips, use channel strides and release failed locks");
}
}
''')
    return "\n".join(parts)
