"""Run the actual queue, object pool, and ReplayView record readers."""
import re


def harness(client, game, source, function):
    def clean(text):
        text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
        return re.sub(r"(?m)^#include[^\n]*", "", text)

    base = game / "net/base"
    parts = [r'''
#include <intrin.h>
#include <mutex>
namespace ContainerTests {
#define PROCESS_ERROR(value) LOG_PROCESS_ERROR(value)
#define LOG_CHECK_ERROR(value) assert(value)
#undef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(value) do { delete[] value; value = nullptr; } while (0)
struct FCriticalSection { std::mutex mutex; };
struct FScopeLock {
    std::lock_guard<std::mutex> guard;
    FScopeLock(FCriticalSection* section) : guard(section->mutex) {}
};
INT64 _CAS(volatile INT64* target, INT64 exchange, INT64 comparand) {
    return _InterlockedCompareExchange64(target, exchange, comparand);
}
''']
    parts.extend(clean(source(base / name)) for name in [
        "base_list.h", "circle_list.h", "circle_list.cpp", "free_stack.h", "free_stack.cpp", "free_object_pool.h"])
    parts.append(r'''
struct alignas(64) Aligned { unsigned char bytes[64]; };
void Run() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(CBaseList) unsigned char bytes[sizeof(CBaseList)];
        memset(bytes, pattern, sizeof(bytes));
        CBaseList* list = new(bytes) CBaseList;
        assert(list->IsEmpty() && !list->PopHead() && !list->PopRear());
        assert(!list->PushHead(nullptr) && !list->PushRear(nullptr));
        BASE_LIST_NODE first, second, third;
        ClearNode(&first); ClearNode(&second); ClearNode(&third);
        assert(list->PushHead(&first) && list->PushRear(&second));
        assert(!list->IsEmpty() && !list->PushHead(&first));
        assert(CBaseList::Insert(&third, &first));
        assert(list->PopHead() == &first && list->PopRear() == &second);
        assert(CBaseList::Remove(&third) && !CBaseList::Remove(&third));
        assert(list->IsEmpty() && !list->PopHead() && !list->PopRear());
        assert(list->PushRear(&first) && list->PopHead() == &first);
        list->~CBaseList();
    }
    CCircleList queue;
    assert(queue.IsEmpty() && !queue.Pop() && !queue.Check());
    assert(!queue.Init(0) && queue.Init(3));
    INT first = 1, second = 2;
    assert(queue.Push(&first) && queue.Push(&second) && !queue.Push(&first));
    assert(!queue.Init(5));
    assert(queue.Check() == &first && queue.Pop() == &first);
    queue.UnInit();
    assert(queue.IsEmpty() && !queue.Pop() && !queue.Check() && !queue.Push(&first));
    assert(queue.Init(3) && queue.Push(&second) && queue.Pop() == &second);
    assert(!queue.Pop() && queue.IsEmpty());
    queue.UnInit();
    CFreeObjectPool<Aligned> pool;
    assert(!pool.Init(-1) && pool.GetSize() == 0);
    assert(pool.Init(0) && !pool.New());
    assert(pool.Init(3) && pool.GetSize() == 3 && !pool.Init(2));
    Aligned* values[3];
    for (auto& value : values) {
        value = pool.New();
        assert(value && (uintptr_t)value % alignof(Aligned) == 0);
        for (unsigned char byte : value->bytes) assert(!byte);
    }
    assert(!pool.New());
    Aligned foreign;
    assert(!pool.Delete(&foreign) && !pool.Delete(nullptr));
    assert(!pool.Delete(reinterpret_cast<Aligned*>(reinterpret_cast<char*>(values[0]) + 1)));
    for (auto* value : values) {
        assert(pool.Delete(value));
        for (INT repeat = 0; repeat < 5; ++repeat) {
            Aligned* again = pool.New();
            assert(again == value);
            assert(pool.Delete(again));
        }
    }
    assert(pool.UnInit() && pool.GetSize() == 0 && !pool.New());
    assert(!pool.Delete(values[0]));
    CFreeObjectPool<UINT32> scalars;
    assert(scalars.Init(1));
    UINT32* scalar = scalars.New();
    assert(scalar && !*scalar && scalars.Delete(scalar));
    puts("Queue cleanup and aligned object-pool regressions passed");
}
#undef PROCESS_ERROR
#undef LOG_CHECK_ERROR
#undef SAFE_DELETE_ARRAY
}
namespace ReplayViewTests {
using DOUBLE = double;
using NAME_INDEX = INT;
using TCHAR = char;
struct CString {
    std::string value;
    void AppendFormat(const char*, ...) {}
    CString& operator+=(const char* text) { value += text; return *this; }
};
struct net_message_object { const char* GetMsgName(WORD) { return "message"; } };
enum { IDC_CHECK_AXIS, IDC_CHECK_MMOVE, BST_CHECKED = 1 };
struct CReplayViewDlg {
    net_message_object m_NetMessage;
    INT IsDlgButtonChecked(INT) { return BST_CHECKED; }
};
struct App { CReplayViewDlg* m_pMainWnd; } theApp;
#define _T(s) s
''')
    replay = source(client / "client/DevTools/ReplayView/ReplayViewDlg.cpp")
    begin = replay.index("enum EReplayDataType")
    end = replay.index("void CReplayViewDlg::ReplayFile(")
    parts.append(clean(replay[begin:end]))
    parts.append(r'''
template<class T> void Append(std::vector<BYTE>& bytes, const T& value) {
    const BYTE* start = reinterpret_cast<const BYTE*>(&value);
    bytes.insert(bytes.end(), start, start + sizeof(value));
}
void Base(std::vector<BYTE>& bytes) { Append<UINT64>(bytes, 123); Append<DOUBLE>(bytes, 2.5); }
template<class Record> void Truncations(const std::vector<BYTE>& bytes) {
    for (size_t count = 0; count < bytes.size(); ++count) {
        std::vector<BYTE> truncated(bytes.begin(), bytes.begin() + count);
        FMemFile input(truncated.data(), truncated.size(), false);
        Record record;
        assert(!record.Read(input));
    }
}
void Run() {
    CReplayViewDlg dialog; theApp.m_pMainWnd = &dialog;
    {
        FILE* output = fopen("replay-remaining-test.bin", "wb");
        assert(output && fwrite("abc", 1, 3, output) == 3);
        fclose(output);
        FMemFile loaded("replay-remaining-test.bin", "rb");
        assert(loaded.Remaining() == 3);
        loaded.ReAllocate(64);
        assert(loaded.Remaining() == 3);
        loaded.Seek(64);
        assert(!loaded.Remaining());
        loaded.Seek(1);
        assert(loaded.Remaining() == 2);
        remove("replay-remaining-test.bin");
    }
    check_constructed<FReplayDataNetwork>([](FReplayDataNetwork& record) {
        assert(!record.Index && !record.GameTime && record.DataType == ENDT_Recv && !record.DataSize && record.buf.empty());
        CString output;
        record.ToString(output);
    });
    check_constructed<FReplayDataScreenRes>([](FReplayDataScreenRes& record) {
        assert(!record.Index && !record.GameTime && !record.ResX && !record.ResY && !record.IsFullScreen);
    });
    check_constructed<FReplayDataInput>([](FReplayDataInput& record) {
        assert(!record.Index && !record.GameTime && record.DataType == EIDT_Key && !record.ControllerId);
        assert(!record.MousePoint.X && !record.MousePoint.Y && record.EventType == IE_Pressed);
        assert(!record.AmountDepressed && !record.GamePad && !record.Delta && !record.DeltaTime && !record.Character);
        assert(!record.name_idx && !record.name_number);
    });
    for (INT size : {-1, 0, 1, 65536, 0x7fffffff}) {
        std::vector<BYTE> bytes; Base(bytes); Append<INT>(bytes, ENDT_Recv); Append(bytes, size);
        FMemFile input(bytes.data(), bytes.size(), false);
        FReplayDataNetwork record;
        record.buf.assign(3, 0xA5);
        assert(!record.Read(input) && record.buf.empty());
    }
    for (INT size : {2, 3, 1024, 65535}) {
        std::vector<BYTE> bytes; Base(bytes); Append<INT>(bytes, ENDT_Recv); Append(bytes, size);
        bytes.resize(bytes.size() + size, 0);
        FMemFile input(bytes.data(), bytes.size(), false);
        FReplayDataNetwork record;
        assert(record.Read(input) && record.Index == 123 && record.GameTime == 2.5 && record.buf.size() == size);
        assert(!input.Remaining());
        if (size < 1024) Truncations<FReplayDataNetwork>(bytes);
    }
    for (INT type : {-1, 2, 0x7fffffff}) {
        std::vector<BYTE> bytes; Base(bytes); Append(bytes, type); Append<INT>(bytes, 2); Append<WORD>(bytes, 0);
        FMemFile input(bytes.data(), bytes.size(), false);
        FReplayDataNetwork record;
        assert(!record.Read(input));
    }
    std::vector<BYTE> screen;
    Append<UINT>(screen, 1920); Append<UINT>(screen, 1080); Append<UBOOL>(screen, TRUE);
    FMemFile inputScreen(screen.data(), screen.size(), false);
    FReplayDataScreenRes resolution;
    assert(resolution.Read(inputScreen) && resolution.ResX == 1920 && resolution.ResY == 1080);
    Truncations<FReplayDataScreenRes>(screen);
    for (INT type = EIDT_Key; type <= EIDT_MousePosition; ++type) {
        std::vector<BYTE> bytes; Base(bytes); Append(bytes, type);
        Append<INT>(bytes, 11); Append<INT>(bytes, 22); Append<INT>(bytes, 1);
        if (type == EIDT_Key) { Append<NAME_INDEX>(bytes, 4); Append<INT>(bytes, 5); }
        Append<EInputEvent>(bytes, IE_Pressed); Append<FLOAT>(bytes, 1); Append<UBOOL>(bytes, FALSE);
        Append<FLOAT>(bytes, 2); Append<FLOAT>(bytes, 3); Append<WCHAR>(bytes, L'A');
        FMemFile input(bytes.data(), bytes.size(), false);
        FReplayDataInput record;
        assert(record.Read(input) && record.Index == 123 && record.DataType == type);
        assert(record.MousePoint.X == 11 && record.MousePoint.Y == 22 && record.Character == L'A');
        assert(record.name_idx == (type == EIDT_Key ? 4 : 0));
        Truncations<FReplayDataInput>(bytes);
    }
    for (INT type : {-1, 4, 0x7fffffff}) {
        std::vector<BYTE> bytes; Base(bytes); Append(bytes, type);
        bytes.resize(bytes.size() + 64, 0);
        FMemFile input(bytes.data(), bytes.size(), false);
        FReplayDataInput record;
        assert(!record.Read(input));
    }
    puts("Replay record initialization and boundary regressions passed");
}
#undef _T
}
''')
    return "\n".join(parts)
