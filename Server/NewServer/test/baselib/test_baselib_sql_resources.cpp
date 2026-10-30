#include <array>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <thread>
#include "std_type.h"
#include "process_error.h"
#include "sql_mgr.h"
#include "fake_mysql_client.h"
#include "db_init.h"

// Internal bootstrap helpers are linked from the production baselib object.
INT32 GetDBVersion(MYSQL* mysql);
BOOL ExecuteSqlFile(MYSQL* mysql, const char* prefix, INT32 version);

namespace
{
int checks = 0;
int failures = 0;
int liveData = 0;
int callbacks = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}
struct Data
{
    UINT32 parameter;
    BOOL result;
    Data(UINT32 parameter, BOOL result) : parameter(parameter), result(result) { ++liveData; }
    ~Data() { --liveData; }
};
BOOL on_result(std::list<void*>& rows, void* context)
{
    ++callbacks;
    const auto& data = *static_cast<Data*>(context);
    INT32 value = 0;
    if (rows.size() == 1)
        std::memcpy(&value, rows.front(), sizeof(value));
    check(rows.size() == 1 && value == static_cast<INT32>(data.parameter + 100),
          "callback borrows the decoded result for its own parameter");
    check(liveData > 0, "captured data remains alive throughout callback execution");
    return data.result;
}
SQL_STMT_CALLBACK callback(UINT32 parameter, BOOL result = TRUE)
{
    return MakeOwnedSqlCallback(on_result, std::make_unique<Data>(parameter, result));
}
BOOL borrowed_result(std::list<void*>& rows, void* output)
{
    if (rows.size() != 1)
        return FALSE;
    std::memcpy(output, rows.front(), sizeof(INT32));
    return TRUE;
}
bool init(CSqlMgr& manager)
{
    SQL_STMT_DATA definition{};
    definition.nThreadIndex = -1;
    definition.nSqlStmtID = 0;
    definition.pcszSql = "select {d} where id=[ud]";
    return manager.Init("fake", "", "", "fake", "utf8mb4", 0, &definition, 1, 1) != FALSE;
}
bool finish(CSqlMgr& manager)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{5};
    while (!manager.IsCompleted() && std::chrono::steady_clock::now() < deadline)
    {
        manager.MainLoop();
        std::this_thread::yield();
    }
    return manager.IsCompleted() != FALSE;
}
void check_rejections()
{
    CSqlMgr manager;
    check(!manager.AsyncExecSqlStmtOwned(0, callback(1), "ud", UINT32{1}), "uninitialized manager rejects owned work");
    check(liveData == 0, "rejected work releases captured data immediately");
    check(!manager.SyncExecSqlStmtOwned(0, callback(1), nullptr), "missing format rejects synchronous owned work");
    check(liveData == 0, "invalid format releases the synchronous capture");
    manager.UnInit();
    check(!fake_mysql::ready, "destroying an uninitialized manager does not initialize the client");
    SQL_STMT_DATA invalid{};
    invalid.nSqlStmtID = 1;
    invalid.pcszSql = "select 1";
    check(!manager.Init("fake", "", "", "fake", "", 0, &invalid, 1, 1),
          "invalid statement metadata rejects initialization after allocating its pool");
    check(!fake_mysql::ready && fake_mysql::connections == 0,
          "partial initialization releases metadata, pools and the client lease");
}
void check_results()
{
    cqyh::SqlResults results;
    std::array<std::uint8_t*, 257> addresses{};
    for (std::size_t index = 0; index < addresses.size(); ++index)
    {
        addresses[index] = results.Append(4);
        const auto value = static_cast<std::uint32_t>(index);
        std::memcpy(addresses[index], &value, sizeof(value));
    }
    bool stable = results.Rows().size() == addresses.size();
    std::size_t index = 0;
    for (const auto* row : results.Rows())
    {
        std::uint32_t value{};
        std::memcpy(&value, row, sizeof(value));
        stable = stable && row == addresses[index] && value == index;
        ++index;
    }
    check(stable, "row addresses and packed bytes survive result-set growth");
    results.Rows().clear();
    results.Clear();
    check(results.Rows().empty(), "clearing the borrowed view still permits complete owned cleanup");
    results.Append(0);
    results.Clear();
    check(results.Rows().empty(), "result storage supports reuse and zero-byte rows");
}
void check_bootstrap()
{
    namespace fs = std::filesystem;
    struct WorkingDirectory
    {
        fs::path previous = fs::current_path();
        fs::path path = fs::temp_directory_path() / ("cqyh_sql_" + std::to_string(GetCurrentProcessId()) +
            "_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        WorkingDirectory()
        {
            if (!fs::create_directory(path))
                throw std::runtime_error("cannot reserve SQL fixture directory");
            fs::create_directories(path / "commonconfig/sql");
            fs::current_path(path);
        }
        ~WorkingDirectory()
        {
            std::error_code error;
            fs::current_path(previous, error);
            fs::remove_all(path, error);
        }
    } directory;
    cqyh::MysqlLibraryLease library;
    check(library.Acquire(), "acquire client library for bootstrap fixtures");
    cqyh::MysqlPtr connection(mysql_init(nullptr));
    check(GetDBVersion(connection.get()) == 42 && fake_mysql::results == 0,
          "version lookup releases its successful result set");
    fake_mysql::emptyRows = true;
    check(GetDBVersion(connection.get()) == -1 && fake_mysql::results == 0,
          "missing version row releases its result set on guarded failure");
    check(ExistDB(connection.get(), "fake") && fake_mysql::results == 0,
          "database existence lookup releases its result set");
    fake_mysql::emptyRows = true;
    check(!ExistDB(connection.get(), "missing") && fake_mysql::results == 0,
          "empty database lookup releases its result set");
    const auto script = directory.path / "commonconfig/sql/ownership1.sql";
    { std::ofstream output(script); output << "select 1;\n"; }
    check(ExecuteSqlFile(connection.get(), "ownership", 1) && fake_mysql::results == 0,
          "SQL bootstrap consumes and releases successful query results");
    fake_mysql::failQuery = true;
    check(!ExecuteSqlFile(connection.get(), "ownership", 1), "SQL bootstrap reports a query failure");
    const auto handle = CreateFileA(script.string().c_str(), GENERIC_READ | GENERIC_WRITE,
        0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    check(handle != INVALID_HANDLE_VALUE, "bootstrap query failure releases its input file");
    if (handle != INVALID_HANDLE_VALUE)
        CloseHandle(handle);
    check(!ExecuteSqlFile(connection.get(), "missing", 1), "missing bootstrap file fails without an open resource");
}
void check_completion()
{
    CSqlMgr manager;
    check(init(manager), "initialize the production manager against the fake client");
    check(!init(manager), "repeated initialization preserves the active manager instead of replacing its resources");
    check(!manager.AsyncExecSqlStmtOwned(99, callback(2), "ud", UINT32{2}), "invalid statement rejects owned data");
    check(liveData == 0, "invalid statement ID releases its capture");
    check(!manager.AsyncExecSqlStmtOwned(0, callback(2), "x"), "parameter-format failure recycles an allocated statement");
    check(liveData == 0 && manager.IsCompleted(), "failed parameter packing releases data and balances the pool");
    for (UINT32 parameter = 1; parameter <= 32; ++parameter)
    {
        check(manager.SyncExecSqlStmtOwned(0, callback(parameter), "ud", parameter), "synchronous owned query completes");
        check(liveData == 0 && manager.IsCompleted(), "pooled completion releases captured data before returning");
    }
    check(manager.AsyncExecSqlStmtOwned(0, callback(40, FALSE), "ud", UINT32{40}), "submit a failing asynchronous callback");
    const auto before = callbacks;
    check(!manager.SyncExecSqlStmtOwned(0, callback(41), "ud", UINT32{41}), "sync reports callback failure after processing its own statement");
    check(callbacks == before + 2 && liveData == 0 && manager.IsCompleted(), "callback failure recycles both statements without leaving pending borrowed data");

    bool caught = false;
    auto throwing = [data = std::make_unique<Data>(42, TRUE)](std::list<void*>&, void*) -> BOOL
    { throw std::runtime_error("callback failure"); };
    check(manager.AsyncExecSqlStmtOwned(0, std::move(throwing), "ud", UINT32{42}), "submit a throwing callback");
    try { check(finish(manager), "throwing callback reached completion"); }
    catch (const std::runtime_error&) { caught = true; }
    check(caught && liveData == 0 && manager.IsCompleted(), "callback exception recycles the statement and its captured resources");
    INT32 borrowed = 0;
    caught = false;
    check(manager.AsyncExecSqlStmtOwned(0,
        [](std::list<void*>&, void*) -> BOOL { throw std::runtime_error("unrelated callback failure"); },
        "ud", UINT32{43}), "queue a throwing callback ahead of synchronous borrowed work");
    try { manager.SyncExecSqlStmt(0, borrowed_result, &borrowed, "ud", UINT32{44}); }
    catch (const std::runtime_error&) { caught = true; }
    check(caught && borrowed == 144 && manager.IsCompleted(),
          "sync finishes borrowed work before propagating an unrelated callback exception");
    check(finish(manager), "exception fixture leaves no pending statement");
    manager.UnInit();
    check(fake_mysql::connections == 0 && fake_mysql::statements == 0, "normal shutdown closes statements and connections");
}
void check_retry()
{
    CSqlMgr manager;
    check(init(manager), "initialize retry fixture");
    fake_mysql::failFreeResults = 1;
    check(manager.SyncExecSqlStmtOwned(0, callback(50), "ud", UINT32{50}), "retry after a partial fetched result succeeds");
    check(liveData == 0 && manager.IsCompleted(), "retry keeps exactly the final rows and releases its callback data");
    fake_mysql::failFetchAfterRow = true;
    const auto executedBefore = fake_mysql::executions.load();
    check(manager.SyncExecSqlStmtOwned(0, callback(52), "ud", UINT32{52}) && fake_mysql::executions == executedBefore + 2,
          "fetch failure after a row retries instead of publishing a partial success");
    fake_mysql::failExecutions = 2;
    const auto before = callbacks;
    check(!manager.SyncExecSqlStmtOwned(0, callback(51), "ud", UINT32{51}), "retry exhaustion reports failure");
    check(callbacks == before && liveData == 0 && manager.IsCompleted(), "exhausted query drops its capture without invoking a success callback");
    manager.UnInit();
    check(fake_mysql::connections == 0 && fake_mysql::statements == 0, "reconnect and shutdown release every client handle");
}
void check_shutdown()
{
    CSqlMgr first, second;
    check(init(first) && init(second), "two managers share client initialization");
    first.UnInit();
    check(fake_mysql::ready, "stopping one manager keeps the other manager's client initialized");
    check(second.SyncExecSqlStmtOwned(0, callback(60), "ud", UINT32{60}), "remaining manager can still execute queries");
    const auto before = callbacks;
    for (UINT32 parameter = 0; parameter < 64; ++parameter)
        check(second.AsyncExecSqlStmtOwned(0, callback(parameter), "ud", parameter), "queue owned work before shutdown");
    second.UnInit();
    check(callbacks == before && liveData == 0 && second.IsCompleted(), "shutdown discards queued captures without invoking callbacks");
    check(fake_mysql::connections == 0 && fake_mysql::statements == 0 && fake_mysql::threads == 0 && !fake_mysql::ready,
          "final manager closes handles before ending the client library");
    check(fake_mysql::orderErrors == 0, "statement buffers and connections outlive client close operations");
}

void check_worker_start_failure()
{
    CSqlMgr manager;
    fake_mysql::failThreadInit = true;
    check(init(manager), "launch manager whose worker fails client TLS initialization");
    const auto before = callbacks;
    const auto started = std::chrono::steady_clock::now();
    check(!manager.SyncExecSqlStmtOwned(0, callback(70), "ud", UINT32{70}),
          "synchronous query fails when its worker exits during startup");
    check(std::chrono::steady_clock::now() - started < std::chrono::seconds(2),
          "worker startup failure cannot leave a synchronous caller spinning");
    check(finish(manager) && callbacks == before && liveData == 0,
          "dead worker rejects or drains pending captures without invoking success callbacks");
    manager.UnInit();
    check(fake_mysql::threads == 0 && !fake_mysql::ready, "failed worker shutdown releases library initialization");
}

void check_interruptible_retry()
{
    CSqlMgr manager;
    fake_mysql::failConnect = true;
    check(init(manager), "launch manager with a connection failure");
    // Let the worker enter its reconnect backoff before requesting shutdown.
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (fake_mysql::failConnect && std::chrono::steady_clock::now() < deadline)
        std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    const auto started = std::chrono::steady_clock::now();
    manager.UnInit();
    check(std::chrono::steady_clock::now() - started < std::chrono::seconds(2),
          "shutdown interrupts the eight-second reconnect backoff");
    fake_mysql::failConnect = false;
    check(fake_mysql::threads == 0 && fake_mysql::connections == 0 && !fake_mysql::ready,
          "interrupted retry closes resources before releasing the library");
}

void check_completion_backpressure()
{
    constexpr UINT32 capacity = 65535; // Public legacy queue capacity: 65536 slots, one reserved.
    for (const bool stopBlockedWorker : {false, true})
    {
        CSqlMgr manager;
        check(init(manager), "initialize completion backpressure fixture");
        const auto executedBefore = fake_mysql::executions.load();
        UINT32 completed = 0;
        bool valid = true;
        const auto submit = [&](UINT32 parameter)
        {
            auto owned = std::make_unique<Data>(parameter, TRUE);
            return manager.AsyncExecSqlStmtOwned(0,
                [&, owned = std::move(owned)](std::list<void*>& rows, void*) -> BOOL
                {
                    INT32 value = 0;
                    if (rows.size() == 1)
                        std::memcpy(&value, rows.front(), sizeof(value));
                    valid = valid && rows.size() == 1 && value == static_cast<INT32>(owned->parameter + 100);
                    ++completed;
                    return TRUE;
                }, "ud", parameter) != FALSE;
        };
        bool accepted = true;
        for (UINT32 index = 0; index < capacity; ++index)
            accepted = submit(index) && accepted;
        check(accepted, "fill the actual production completion queue without pumping callbacks");
        auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
        while (fake_mysql::executions < executedBefore + static_cast<int>(capacity) &&
               std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
        check(fake_mysql::executions >= executedBefore + static_cast<int>(capacity), "worker fills its output queue");
        for (UINT32 index = 0; index < 32; ++index)
            accepted = submit(capacity + index) && accepted;
        deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (fake_mysql::executions < executedBefore + static_cast<int>(capacity) + 1 &&
               std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
        check(accepted && fake_mysql::executions >= executedBefore + static_cast<int>(capacity) + 1,
              "additional accepted work reaches completion backpressure");
        if (stopBlockedWorker)
        {
            const auto started = std::chrono::steady_clock::now();
            manager.UnInit();
            check(std::chrono::steady_clock::now() - started < std::chrono::seconds(2),
                  "shutdown interrupts a worker blocked on a full completion queue");
            check(completed == 0 && liveData == 0 && manager.IsCompleted(),
                  "blocked shutdown releases captures from both queues without success callbacks");
        }
        else
        {
            check(finish(manager), "pumping callbacks releases output backpressure");
            check(valid && completed == capacity + 32 && liveData == 0,
                  "full output queue loses no accepted statement or owned callback data");
            manager.UnInit();
        }
    }
}
}

int main()
{
    CLog::GetInstance().SetOutputMask(lotNone);
    check_rejections();
    check_results();
    check_bootstrap();
    check_completion();
    check_retry();
    check_shutdown();
    check_worker_start_failure();
    check_interruptible_retry();
    check_completion_backpressure();
    std::printf("%d SQL resource checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
