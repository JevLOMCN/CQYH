#ifndef CQYH_BASELIB_MYSQL_OWNER_H
#define CQYH_BASELIB_MYSQL_OWNER_H

#include "mysql.h"
#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <utility>

namespace cqyh
{
// Client initialization is process-wide, not one independent library per manager.
// Leases retain their synchronization state through static-object destruction.
class MysqlLibraryLease
{
    struct State
    {
        std::mutex mutex;
        std::size_t users = 0;
    };
    static std::shared_ptr<State> SharedState()
    {
        static const auto state = std::make_shared<State>();
        return state;
    }
public:
    MysqlLibraryLease() = default;
    MysqlLibraryLease(const MysqlLibraryLease&) = delete;
    MysqlLibraryLease& operator=(const MysqlLibraryLease&) = delete;
    MysqlLibraryLease(MysqlLibraryLease&& other) noexcept : m_state(std::move(other.m_state)) {}
    MysqlLibraryLease& operator=(MysqlLibraryLease&& other) noexcept
    {
        if (this != &other)
        {
            Reset();
            m_state = std::move(other.m_state);
        }
        return *this;
    }
    ~MysqlLibraryLease() { Reset(); }

    bool Acquire()
    {
        if (m_state)
            return true;
        auto state = SharedState();
        const std::scoped_lock lock(state->mutex);
        if (state->users == 0 && mysql_library_init(0, nullptr, nullptr) != 0)
            return false;
        ++state->users;
        m_state = std::move(state);
        return true;
    }
    void Reset() noexcept
    {
        if (auto state = std::move(m_state))
        {
            const std::scoped_lock lock(state->mutex);
            if (--state->users == 0)
                mysql_library_end();
        }
    }
private:
    std::shared_ptr<State> m_state;
};

class MysqlThreadLease
{
public:
    MysqlThreadLease() : m_initialized(mysql_thread_init() == 0) {}
    MysqlThreadLease(const MysqlThreadLease&) = delete;
    MysqlThreadLease& operator=(const MysqlThreadLease&) = delete;
    ~MysqlThreadLease() { if (m_initialized) mysql_thread_end(); }
    explicit operator bool() const noexcept { return m_initialized; }
private:
    bool m_initialized;
};

// Declare this at the entry of threads that call the MySQL client. The thread
// state ends before its library lease, even when no connection is currently open.
class MysqlThreadContext
{
public:
    MysqlThreadContext()
    {
        if (m_library.Acquire())
            m_thread.emplace();
    }
    explicit operator bool() const noexcept
    {
        return m_thread && static_cast<bool>(*m_thread);
    }
private:
    MysqlLibraryLease m_library;
    std::optional<MysqlThreadLease> m_thread;
};

struct MysqlCloser
{
    void operator()(MYSQL* value) const noexcept { mysql_close(value); }
    MysqlLibraryLease library;
};
struct MysqlResultCloser
{
    void operator()(MYSQL_RES* value) const noexcept { mysql_free_result(value); }
};
struct MysqlStmtCloser
{
    void operator()(MYSQL_STMT* value) const noexcept { mysql_stmt_close(value); }
};
using MysqlPtr = std::unique_ptr<MYSQL, MysqlCloser>;
using MysqlResultPtr = std::unique_ptr<MYSQL_RES, MysqlResultCloser>;
using MysqlStmtPtr = std::unique_ptr<MYSQL_STMT, MysqlStmtCloser>;

// Keep client initialization alive for the entire connection lifetime.
[[nodiscard]] inline MysqlPtr MakeMysql()
{
    MysqlLibraryLease library;
    if (!library.Acquire())
        return {};
    auto* connection = mysql_init(nullptr);
    if (!connection)
        return {};
    return MysqlPtr(connection, MysqlCloser{std::move(library)});
}

inline bool CloseMysqlStatement(MysqlStmtPtr& statement) noexcept
{
    auto* handle = statement.release();
    return !handle || mysql_stmt_close(handle) == 0;
}
}

#endif // CQYH_BASELIB_MYSQL_OWNER_H
