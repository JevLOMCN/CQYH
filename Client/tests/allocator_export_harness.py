"""Production allocator size limits and editor-export buffer regressions."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    util = clean(source(game / "Inc/MMOUtil.h"))
    parts = [r'''
#include <limits>
#include <new>
#include <cstdarg>
namespace AllocatorExportTests {
using DWORD = uint32_t;
struct Heap {
    int calls = 0;
    DWORD bytes = 0;
    bool fail = false;
    void* Malloc(DWORD count) {
        ++calls; bytes = count;
        if (fail || count > 1024 * 1024) return nullptr;
        return malloc(count ? count : 1);
    }
    void Free(void* value) { free(value); }
} heap;
Heap* GMalloc = &heap;
''']
    for signature in ["template<typename T>\nstruct STLAllocatorBase\n",
                      "template<typename T>\nstruct STLAllocatorBase<const T>",
                      "template\n\t<\n\ttypename T,\n\ttypename AllocPolicy\n\t>",
                      "class UE3STLAllocatorPolicy\n"]:
        parts.append(function(util, signature) + ";")
    parts.append(r'''
struct SmallPolicy {
    static size_t limit, last;
    static int calls;
    static void* allocateBytes(size_t count) { ++calls; last = count; return malloc(count); }
    static void deallocateBytes(void* value) { free(value); }
    static size_t getMaxAllocationSize() { return limit; }
};
size_t SmallPolicy::limit = 64, SmallPolicy::last = 0;
int SmallPolicy::calls = 0;
template<class Action> void AllocationFails(Action action) {
    bool failed = false;
    try { action(); } catch (const std::bad_alloc&) { failed = true; }
    assert(failed);
}
void AllocatorTests() {
    STLAllocator<uint64_t, SmallPolicy> small;
    assert(small.max_size() == 8);
    auto* exact = small.allocate(8); assert(exact && SmallPolicy::last == 64);
    small.deallocate(exact, 8);
    int calls = SmallPolicy::calls;
    AllocationFails([&] { small.allocate(9); }); assert(SmallPolicy::calls == calls);
    SmallPolicy::limit = (std::numeric_limits<size_t>::max)();
    assert(small.max_size() == (std::numeric_limits<size_t>::max)() / sizeof(uint64_t));
    AllocationFails([&] { small.allocate(small.max_size() + 1); }); assert(SmallPolicy::calls == calls);
    AllocationFails([&] { small.allocate((std::numeric_limits<size_t>::max)()); }); assert(SmallPolicy::calls == calls);
    SmallPolicy::limit = 3; assert(!small.max_size());
    AllocationFails([&] { small.allocate(1); }); assert(SmallPolicy::calls == calls);
    STLAllocator<uint32_t, UE3STLAllocatorPolicy> integers;
    assert(UE3STLAllocatorPolicy::getMaxAllocationSize() == size_t(0xffffffffu));
    assert(integers.max_size() == size_t(0xffffffffu) / sizeof(uint32_t));
    calls = heap.calls;
    AllocationFails([&] { integers.allocate(integers.max_size() + 1); }); assert(heap.calls == calls);
    if (sizeof(size_t) > sizeof(DWORD)) {
        size_t excessive_bytes = size_t(0xffffffffu) + 1;
        AllocationFails([&] { UE3STLAllocatorPolicy::allocateBytes(excessive_bytes); });
        assert(heap.calls == calls);
    }
    auto* value = integers.allocate(1); integers.construct(value); assert(!*value && heap.bytes == sizeof(*value));
    integers.destroy(value); integers.deallocate(value, 1);
    auto* zero = integers.allocate(0); integers.deallocate(zero, 0);
    heap.fail = true; AllocationFails([&] { integers.allocate(1); }); heap.fail = false;
    puts("STL allocation element/byte limits, overflow/truncation and failed allocations passed");
}
using TCHAR = char;
struct FString {
    std::string value;
    void Empty() { value.clear(); }
    void Reserve(UINT count) { value.reserve(count); }
    FString& operator+=(const char* text) { value += text; return *this; }
    const char* operator*() const { return value.c_str(); }
};
''')
    bt = clean(source(game / "Src/USeqAct_BTNode.cpp"))
    if "char szTabs[" in bt or bt.count("Buffer += tabs;") != 2:
        raise ValueError("Behavior-tree export must use checked indentation at both output sites")
    parts.append(function(bt, "static BOOL BuildBTIndentation("))
    exporter = source(game / "ExportServerDataBinary.cpp")
    status_lines = [line.strip() for line in exporter.splitlines()
                    if 'GWarn->StatusUpdatef(0, 0, TEXT("%s : %d' in line]
    if len(status_lines) != 1 or "wsprintf(szMessage" in exporter:
        raise ValueError("Export status must format actor names directly with a literal format")
    parts.append(r'''
struct Warn {
    std::string output;
    void StatusUpdatef(int, int, const char* format, ...) {
        va_list args; va_start(args, format);
        va_list copy; va_copy(copy, args);
        int length = vsnprintf(nullptr, 0, format, copy); va_end(copy);
        std::vector<char> text(length + 1);
        vsnprintf(text.data(), text.size(), format, args); va_end(args);
        output.assign(text.data(), length);
    }
} warning;
Warn* GWarn = &warning;
void Report(const FString& ActorName, INT nInterval) {
''')
    parts.extend([status_lines[0], "}"])
    parts.append(r'''
void Run() {
    AllocatorTests();
    FString tabs;
    for (int depth : {0, 1, 63, 64, 65, 255, 256, 1024, 65536}) {
        tabs.value = "old"; assert(BuildBTIndentation(depth, tabs));
        assert(tabs.value == std::string(size_t(depth) * 4, ' '));
    }
    for (int depth : {-2147483647 - 1, -1, 2147483647}) {
        tabs.value = "old"; assert(!BuildBTIndentation(depth, tabs) && tabs.value.empty());
    }
    for (int length : {0, 1, 1023, 1024, 65536}) {
        FString actor; actor.value.assign(length, 'A'); actor.value += "%n%s%I64d";
        Report(actor, 123);
        assert(warning.output == actor.value + " : 123\n");
    }
    puts("Dynamic behavior-tree indentation and long/literal actor-name export messages passed");
}
}
''')
    return "\n".join(parts)
