#include "std_type.h"
#include "process_error.h"
#include "free_object_pool.h"
#include "object_pool.h"
#include "id_object_pool.h"
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace
{
int checks = 0, failures = 0;
void check(bool condition, const char* description)
{
    ++checks;
    if (!condition)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}
struct Record
{
    static inline int live = 0, attempts = 0, throwAt = 0;
    int value;
    Record() : value(17)
    {
        if (++attempts == throwAt)
            throw std::runtime_error("expected pool construction failure");
        ++live;
    }
    ~Record() { --live; }
};
// An empty constructor lets the former allocator report its misalignment without
// relying on an aligned load/store fault to demonstrate the problem.
struct alignas(64) Aligned { Aligned() noexcept {} };

struct Ordered
{
    static inline int next = 0;
    static inline std::vector<int> destroyed;
    int ordinal = next++;
    ~Ordered() { destroyed.push_back(ordinal); }
};

void destruction_order()
{
    Ordered::next = 0;
    Ordered::destroyed.clear();
    Ordered::destroyed.reserve(16);
    {
        CObjectPool<Ordered> pool; // Ordinal zero is the existing sentinel payload.
        pool.Init(2);
        auto* a = pool.New();
        auto* b = pool.New();
        auto* c = pool.New();
        pool.Delete(a);
        pool.Delete(b);
        pool.Delete(c);
        pool.UnInit();
        check(Ordered::destroyed == std::vector<int>{4, 3, 2, 1},
              "growing pool retains newest-chunk-first and reverse-slot destruction");
    }
    check(Ordered::destroyed.back() == 0, "growing pool destroys the sentinel after its chunks");
    Ordered::next = 0;
    Ordered::destroyed.clear();
    auto pool = std::make_unique<CIDObjectPool<Ordered, UINT32>>();
    pool->Init(2);
    auto* a = pool->New(1);
    auto* b = pool->New(2);
    auto* c = pool->New(3);
    pool->Delete(a);
    pool->Delete(b);
    pool->Delete(c);
    pool->UnInit();
    check(Ordered::destroyed == std::vector<int>{2, 3, 0, 1},
          "ID pool retains newest-chunk-first and ascending-slot destruction");
}

void fixed_pool(bool baseline)
{
    CFreeObjectPool<Record> pool;
    check(pool.Init(0) && !pool.New() && pool.GetSize() == 0 && pool.UnInit(),
          "fixed pool preserves an explicitly empty configuration");
    check(pool.Init(3) && Record::live == 3, "fixed pool constructs every slot once");
    std::array<Record*, 3> records{};
    for (auto& record : records)
        record = pool.New();
    check(records[0] && records[1] && records[2] && !pool.New(), "fixed pool has its configured capacity");
    auto* raw = static_cast<char*>(pool.GetDataPool());
    bool layout = true;
    for (auto* record : records)
    {
        const auto offset = reinterpret_cast<char*>(record) - raw - pool.GetDataOffset();
        layout = offset >= 0 && offset % pool.GetDataSize() == 0 && layout;
    }
    check(layout, "packet allocator retains pool base, stride and payload offset contracts");
    records[0]->value = 91;
    pool.Delete(records[0]);
    auto* reused = pool.New();
    check(reused == records[0] && reused->value == 91 && Record::live == 3,
          "fixed pool reuses the same object without reconstructing it");
    if (!baseline)
        check(!pool.Init(8) && pool.GetSize() == 3, "fixed pool rejects reinitialization without losing live slots");
    for (auto* record : records)
        pool.Delete(record);
    check(pool.UnInit() && Record::live == 0, "fixed pool destroys all slots at shutdown");
    check(pool.UnInit() && pool.Init(2) && pool.UnInit() && Record::live == 0,
          "fixed pool supports repeated shutdown and restart");
}

void growing_pool(bool baseline)
{
    const int before = Record::live;
    {
        CObjectPool<Record> pool;
        const int sentinel = Record::live;
        check(pool.Init(3) && Record::live == sentinel + 3, "growing pool constructs its initial slots");
        std::vector<Record*> records;
        for (int index = 0; index < 200; ++index)
        {
            auto* record = pool.New();
            if (!record)
                break;
            record->value = index;
            records.push_back(record);
        }
        bool stable = records.size() == 200;
        for (int index = 0; index < static_cast<int>(records.size()); ++index)
            stable = records[index]->value == index && stable;
        check(stable && pool.GetUsedSize() == 200, "growing pool retains addresses and values across chunk growth");
        if (!baseline)
            check(!pool.Init(8) && pool.GetUsedSize() == 200, "growing pool rejects reinitialization without losing records");
        auto* first = records.front();
        pool.Delete(first);
        check(pool.New() == first && first->value == 0, "growing pool recycles objects without reconstructing them");
        for (auto* record : records)
            pool.Delete(record);
        check(pool.GetFreeSize() == pool.GetSize(), "growing pool returns all slots to the free list");
        check(pool.UnInit() && Record::live == sentinel, "growing pool destroys only chunk payloads at shutdown");
        check(pool.UnInit() && pool.Init(2) && pool.UnInit(), "growing pool restarts after shutdown");
    }
    check(Record::live == before, "growing pool destruction balances every constructor");
}

void id_pool(bool baseline)
{
    auto pool = std::make_unique<CIDObjectPool<Record, UINT32>>();
    check(pool->Init(4) && Record::live == 4, "ID pool constructs initial slots");
    std::vector<Record*> records;
    bool valid = true;
    for (UINT32 index = 0; index < 200; ++index)
    {
        const UINT32 id = 7 + index * HASH_TABLE_SIZE;
        auto* record = pool->New(id);
        if (!record)
        {
            valid = false;
            break;
        }
        record->value = index;
        records.push_back(record);
    }
    for (UINT32 index = 0; index < records.size(); ++index)
        valid = pool->Find(7 + index * HASH_TABLE_SIZE) == records[index] &&
                records[index]->value == static_cast<int>(index) && valid;
    check(valid && records.size() == 200, "colliding IDs retain lookup and pointer stability across growth");
    const int freeBefore = pool->GetFreeSize();
    check(!pool->New(7) && pool->GetFreeSize() == freeBefore && pool->Find(7) == records.front(),
          "duplicate IDs preserve the existing record and free-list accounting");
    if (!baseline)
        check(!pool->Init(8) && pool->GetUsedSize() == 200, "ID pool rejects reinitialization without losing indexed records");
    Record foreign;
    check(!pool->Delete(&foreign), "ID pool rejects pointers from outside its chunks");
    UINT32 next = 0;
    auto visit = [&](UINT32 id, Record* record)
    {
        valid = id == 7 + next * HASH_TABLE_SIZE && record == records[next] && valid;
        ++next;
        return pool->Delete(record);
    };
    check(pool->Traverse(visit) && valid && next == 200 && pool->GetUsedSize() == 0,
          "traversal retains ID order and permits deleting the current record");
    check(!pool->Find(7) && pool->UnInit() && Record::live == 1, "ID pool releases every slot while preserving foreign objects");
    check(pool->UnInit() && pool->Init(2) && pool->UnInit(), "ID pool supports shutdown and restart");
}

void alignment()
{
    auto pool = std::make_unique<CIDObjectPool<Aligned, UINT32>>();
    pool->Init(3); // Packed metadata previously shifted payloads by 3 * 28 bytes.
    std::vector<Aligned*> values;
    bool aligned = true;
    for (UINT32 id = 1; id <= 30; ++id)
    {
        auto* record = pool->New(id);
        aligned = record && reinterpret_cast<std::uintptr_t>(record) % alignof(Aligned) == 0 && aligned;
        values.push_back(record);
    }
    check(aligned, "ID pool aligns over-aligned payloads in initial and expanded chunks");
    for (auto* record : values)
        if (record)
            pool->Delete(record);
    pool->UnInit();
    auto wide = std::make_unique<CIDObjectPool<Record, UINT64>>();
    wide->Init(3);
    constexpr UINT64 id = 0xFEDCBA9876543210ULL;
    auto* record = wide->New(id);
    check(record && wide->Find(id) == record, "64-bit IDs retain their full lookup value");
    wide->Delete(record);
    wide->UnInit();
}

template<class Pool>
void construction_failure(const char* description)
{
    auto pool = std::make_unique<Pool>();
    const int before = Record::live;
    Record::throwAt = Record::attempts + 2;
    bool returned = false;
    try { returned = !pool->Init(4); }
    catch (...) {}
    Record::throwAt = 0;
    check(returned && Record::live == before && pool->GetSize() == 0, description);
    check(pool->Init(3) && pool->UnInit() && Record::live == before,
          "pool remains reusable after a partially constructed chunk fails");
}

template<class Pool>
void growth_failure()
{
    auto pool = std::make_unique<Pool>();
    pool->Init(2);
    auto allocate = [&](UINT32 id)
    {
        if constexpr (requires { pool->New(id); })
            return pool->New(id);
        else
            return pool->New();
    };
    auto* first = allocate(1);
    auto* second = allocate(2);
    first->value = 41;
    second->value = 42;
    const int before = Record::live;
    Record::throwAt = Record::attempts + 2;
    Record* rejected = nullptr;
    bool threw = false;
    try { rejected = allocate(3); }
    catch (...) { threw = true; }
    Record::throwAt = 0;
    check(!threw && !rejected && Record::live == before && pool->GetSize() == 2 &&
          pool->GetUsedSize() == 2 && first->value == 41 && second->value == 42,
          "failed chunk growth preserves live objects, ownership and accounting");
    auto* third = allocate(3);
    check(third && pool->GetSize() == 4 && first->value == 41,
          "a pool can retry growth after payload construction fails");
    pool->Delete(first);
    pool->Delete(second);
    pool->Delete(third);
    pool->UnInit();
}
}

int main(int argc, char** argv)
{
    CLog::GetInstance().SetOutputMask(lotNone);
    const bool baseline = argc > 1 && std::string_view(argv[1]) == "--baseline";
    fixed_pool(baseline);
    growing_pool(baseline);
    id_pool(baseline);
    alignment();
    destruction_order();
    if (!baseline)
    {
        construction_failure<CFreeObjectPool<Record>>("fixed pool rolls back partially constructed records");
        construction_failure<CObjectPool<Record>>("growing pool rolls back partially constructed records");
        construction_failure<CIDObjectPool<Record, UINT32>>("ID pool rolls back partially constructed records");
        growth_failure<CObjectPool<Record>>();
        growth_failure<CIDObjectPool<Record, UINT32>>();
    }
    check(Record::live == 0, "all pool tests balance object lifetimes");
    std::printf("%d pool checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
