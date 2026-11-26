"""Production recipe fallback, DXT1 upload and diagnostic packet boundaries."""
import re


def harness(client, game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    parts = ["namespace RecipeTests {\nenum { NAME_Error = 1 };"]
    header = clean(source(game / "EquipDecomposeManager.h"))
    parts.append(re.search(r"#define COMPOSE_CONSUME_COUNT\s+[^\n]+", header).group())
    for name in ["EquipComposeTemplate", "DecomposeResult"]:
        parts.append(function(header, "struct " + name + "\n") + ";")
    parts.append(r'''
using EquipComposeMap = std::map<INT, EquipComposeTemplate>;
struct EquipDecomposeManager {
    EquipComposeMap m_EquipComposeMap;
    EquipComposeTemplate& GetComposeTemplateById(INT itemId);
};
''')
    parts.append(function(source(game / "EquipDecomposeManager.cpp"),
                          "EquipComposeTemplate& EquipDecomposeManager::GetComposeTemplateById("))
    parts.append(r'''
void Run() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(EquipComposeTemplate) unsigned char bytes[sizeof(EquipComposeTemplate)];
        memset(bytes, pattern, sizeof(bytes));
        auto* recipe = new(bytes) EquipComposeTemplate;
        assert(!recipe->itemId && !recipe->equipId && !recipe->cost && !recipe->bBroadCast);
        for (INT value : recipe->matId) assert(!value);
        for (INT value : recipe->matCnt) assert(!value);
        alignas(DecomposeResult) unsigned char result_bytes[sizeof(DecomposeResult)];
        memset(result_bytes, pattern, sizeof(result_bytes));
        auto* result = new(result_bytes) DecomposeResult;
        assert(!result->itemId && !result->cnt);
    }
    EquipDecomposeManager manager;
    auto& first = manager.GetComposeTemplateById(123);
    assert(first.itemId == 123 && !first.equipId && !first.cost && !first.bBroadCast);
    for (INT value : first.matId) assert(!value);
    for (INT value : first.matCnt) assert(!value);
    first.matId[0] = 42;
    assert(manager.GetComposeTemplateById(123).matId[0] == 42);
    for (INT value : manager.GetComposeTemplateById(456).matId) assert(!value);
    puts("Equipment recipe default and missing-ID fallback regressions passed");
}
}
#undef COMPOSE_CONSUME_COUNT
namespace JigsawUploadTests {
''')
    defines = re.findall(r"(?m)^#define JIGSAW_\w+[^\n]*", clean(source(client / "Common/define/dds_def.h")))
    parts.extend(defines)
    parts.append(r'''
struct Surface {
    UINT stride;
    bool lock_ok = true, writes_stride = true;
    int locks = 0, unlocks = 0;
    std::vector<BYTE> bytes;
    Surface(UINT row_bytes, UINT rows, UINT padding = 0)
        : stride(row_bytes + padding), bytes((row_bytes + padding) * rows, 0xcd) {}
};
struct FTexture2DResource {
    Surface* surface;
    Surface* GetTexture2DRHI() { return surface; }
};
struct UTexture2D { INT SizeX, SizeY; FTexture2DResource* Resource; };
BYTE* RHILockTexture2D(Surface* surface, UINT, UBOOL, UINT& stride, UBOOL) {
    assert(surface); ++surface->locks;
    if (!surface->lock_ok) return nullptr;
    if (surface->writes_stride) stride = surface->stride;
    return surface->bytes.data();
}
void RHIUnlockTexture2D(Surface* surface, UINT, UBOOL) { ++surface->unlocks; }
struct MMOJigsawManager {
    UTexture2D* pTargetTexture = nullptr;
    UTexture2D* pAnsTexture[JIGSAW_ANSWER_COUNT] = {};
    char _TargetTextureData[JIGSAW_DATA_SIZE] = {};
    char _AnswerTextureData[JIGSAW_ANSWER_COUNT][JIGSAW_ANSWER_DATA_SIZE] = {};
    BOOL isBuildingTexture = FALSE, isBuildDone = FALSE;
    void BuilJigsawTexture();
};
''')
    jigsaw = source(game / "MMOJigsawManager.cpp")
    parts.append(function(jigsaw, "static BOOL UploadJigsawTexture("))
    parts.append(function(jigsaw, "void MMOJigsawManager::BuilJigsawTexture()"))
    parts.append(r'''
void Run() {
    for (UINT width : {UINT(JIGSAW_PIC_WIDTH), UINT(JIGSAW_ANSWER_WIDTH)}) {
        const UINT row_bytes = width / 4 * 8, rows = width / 4;
        std::vector<char> source(row_bytes * rows);
        for (size_t i = 0; i < source.size(); ++i) source[i] = (char)(i % 127);
        for (UINT padding : {0u, 1u, 16u, 256u}) {
            Surface surface(row_bytes, rows, padding);
            FTexture2DResource resource{&surface}; UTexture2D texture{(INT)width, (INT)width, &resource};
            assert(UploadJigsawTexture(&texture, source.data(), width, source.size()));
            assert(surface.locks == 1 && surface.unlocks == 1);
            for (UINT row = 0; row < rows; ++row) {
                assert(!memcmp(surface.bytes.data() + row * surface.stride, source.data() + row * row_bytes, row_bytes));
                for (UINT i = row_bytes; i < surface.stride; ++i) assert(surface.bytes[row * surface.stride + i] == 0xcd);
            }
        }
        for (int invalid = 0; invalid < 8; ++invalid) {
            Surface surface(row_bytes, rows);
            FTexture2DResource resource{&surface}; UTexture2D texture{(INT)width, (INT)width, &resource};
            if (invalid == 0) { surface.stride = row_bytes - 1; surface.bytes.resize(surface.stride * rows); }
            if (invalid == 1) surface.stride = 0;
            if (invalid == 2) surface.lock_ok = false;
            if (invalid == 3) surface.writes_stride = false;
            if (invalid == 4) texture.Resource = nullptr;
            if (invalid == 5) texture.SizeX = 1;
            if (invalid == 6) texture.SizeY = 1;
            const size_t length = invalid == 7 ? source.size() - 1 : source.size();
            assert(!UploadJigsawTexture(&texture, source.data(), width, length));
            for (BYTE value : surface.bytes) assert(value == 0xcd);
            assert(surface.unlocks == ((invalid == 0 || invalid == 1 || invalid == 3) ? 1 : 0));
        }
        if (sizeof(size_t) == 4) {
            Surface surface(row_bytes, rows); surface.stride = 0xffffffff;
            FTexture2DResource resource{&surface}; UTexture2D texture{(INT)width, (INT)width, &resource};
            assert(!UploadJigsawTexture(&texture, source.data(), width, source.size()));
            assert(surface.unlocks == 1);
            for (BYTE value : surface.bytes) assert(value == 0xcd);
        }
    }
    Surface target(256, 32, 16), answer(64, 8, 16);
    FTexture2DResource target_resource{&target}, answer_resource{&answer};
    UTexture2D target_texture{JIGSAW_PIC_WIDTH, JIGSAW_PIC_WIDTH, &target_resource};
    UTexture2D answer_texture{JIGSAW_ANSWER_WIDTH, JIGSAW_ANSWER_WIDTH, &answer_resource};
    MMOJigsawManager manager; manager.pTargetTexture = &target_texture;
    for (auto& texture : manager.pAnsTexture) texture = &answer_texture;
    memset(manager._TargetTextureData, 0x12, sizeof(manager._TargetTextureData));
    memset(manager._AnswerTextureData, 0x34, sizeof(manager._AnswerTextureData));
    manager.BuilJigsawTexture();
    assert(manager.isBuildDone && !manager.isBuildingTexture && target.unlocks == 1 && answer.unlocks == JIGSAW_ANSWER_COUNT);
    manager.pAnsTexture[0] = nullptr; manager.BuilJigsawTexture();
    assert(!manager.isBuildDone && !manager.isBuildingTexture);
    manager.pTargetTexture = nullptr; manager.isBuildDone = TRUE; manager.BuilJigsawTexture();
    assert(!manager.isBuildDone && !manager.isBuildingTexture);
    assert(!UploadJigsawTexture(&target_texture, nullptr, JIGSAW_PIC_WIDTH, JIGSAW_DATA_SIZE));
    puts("DXT1 texture stride, lock failure and upload completion regressions passed");
}
}
''')
    parts.extend("#undef " + re.search(r"#define (\w+)", line).group(1) for line in defines)
    parts.append(r'''
#define SHIPPING_PC_GAME 0
#define M_ENABLE_RPMGR 1
#define ANSI_TO_TCHAR(s) (s)
namespace ErrorPacketTests {
bool g_replayMode = false;
int boxes = 0;
std::string message;
int MessageBox(void*, const char* text, int, int) { ++boxes; message = text; return 0; }
''')
    parts.append(function(clean(source(client / "Common/message/g2c_message.h")),
                          "struct G2C_SYNC_DATA_ERROR") + ";")
    parts.append("struct CMessageHandler { void Parse_g2c_sync_data_error(const char*, UINT32); };")
    parts.append(function(source(game / "net/message_handler.cpp"),
                          "void CMessageHandler::Parse_g2c_sync_data_error("))
    parts.append(r'''
void Run() {
    CMessageHandler parser;
    parser.Parse_g2c_sync_data_error(nullptr, 100);
    for (UINT size = 0; size <= sizeof(G2C_SYNC_DATA_ERROR) + 20; ++size) {
        std::vector<char> bytes(size, 'X');
        parser.Parse_g2c_sync_data_error(bytes.data(), size);
        assert(!boxes);
    }
    for (UINT length : {1u, 2u, 19u, 65500u}) {
        std::vector<char> bytes(sizeof(G2C_SYNC_DATA_ERROR) + length, 'X');
        bytes.back() = 0;
        parser.Parse_g2c_sync_data_error(bytes.data(), (UINT)bytes.size());
        assert(message == std::string(length - 1, 'X'));
    }
    int before = boxes; g_replayMode = true;
    std::vector<char> bytes(sizeof(G2C_SYNC_DATA_ERROR) + 1, 0);
    parser.Parse_g2c_sync_data_error(bytes.data(), (UINT)bytes.size()); assert(boxes == before);
    puts("Diagnostic packet terminator and short-header regressions passed");
}
}
#undef SHIPPING_PC_GAME
#undef M_ENABLE_RPMGR
#undef ANSI_TO_TCHAR
''')
    return "\n".join(parts)
