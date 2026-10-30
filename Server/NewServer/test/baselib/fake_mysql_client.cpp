#include "fake_mysql_client.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include "mysql.h"

namespace
{
struct Connection { bool alive = true; };
struct Result
{
    int remaining;
    char value[3]{'4', '2', '\0'};
    char* row[2]{value, nullptr};
};
MYSQL_RES* make_result()
{
    ++fake_mysql::results;
    return reinterpret_cast<MYSQL_RES*>(new Result{fake_mysql::emptyRows.exchange(false) ? 0 : 1});
}
struct Statement
{
    Connection* connection;
    MYSQL_BIND* parameters = nullptr;
    MYSQL_BIND* results = nullptr;
    unsigned long parameterCount = 0;
    int fetched = 0;
    std::int32_t value = 0;
    std::string query;
    fake_mysql::Rows rows;
    std::size_t nextRow = 0;
    bool scripted = false;
};
Statement& statement(MYSQL_STMT* value) { return *reinterpret_cast<Statement*>(value); }
}

extern "C"
{
int STDCALL mysql_server_init(int, char**, char**)
{
    if (fake_mysql::failLibrary.exchange(false))
        return 1;
    fake_mysql::ready = true;
    return 0;
}
void STDCALL mysql_server_end()
{
    if (fake_mysql::threads != 0)
        ++fake_mysql::orderErrors;
    fake_mysql::ready = false;
}
bool STDCALL mysql_thread_init()
{
    if (fake_mysql::failThreadInit.exchange(false))
        return true;
    ++fake_mysql::threads;
    return false;
}
void STDCALL mysql_thread_end() { --fake_mysql::threads; }
MYSQL_RES* STDCALL mysql_list_dbs(MYSQL*, const char*) { return make_result(); }
MYSQL_RES* STDCALL mysql_store_result(MYSQL*) { return make_result(); }
MYSQL_ROW STDCALL mysql_fetch_row(MYSQL_RES* value)
{
    auto& result = *reinterpret_cast<Result*>(value);
    return result.remaining-- > 0 ? result.row : nullptr;
}
void STDCALL mysql_free_result(MYSQL_RES* value)
{ delete reinterpret_cast<Result*>(value); --fake_mysql::results; }
int STDCALL mysql_query(MYSQL*, const char*) { return fake_mysql::failQuery.exchange(false) ? 1 : 0; }
int STDCALL mysql_next_result(MYSQL*) { return -1; }
unsigned int STDCALL mysql_field_count(MYSQL*) { return 0; }
MYSQL* STDCALL mysql_init(MYSQL*)
{
    if (fake_mysql::failInit.exchange(false))
        return nullptr;
    ++fake_mysql::connections;
    return reinterpret_cast<MYSQL*>(new Connection);
}
MYSQL* STDCALL mysql_real_connect(MYSQL* value, const char*, const char*, const char*,
                                  const char*, unsigned int, const char*, unsigned long)
{ return fake_mysql::failConnect.exchange(false) ? nullptr : value; }
int STDCALL mysql_set_character_set(MYSQL*, const char*) { return 0; }
unsigned int STDCALL mysql_errno(MYSQL*) { return 2000; }
const char* STDCALL mysql_error(MYSQL*) { return "simulated client failure"; }
void STDCALL mysql_close(MYSQL* value)
{
    if (!fake_mysql::ready)
        ++fake_mysql::orderErrors;
    auto* connection = reinterpret_cast<Connection*>(value);
    connection->alive = false;
    delete connection;
    --fake_mysql::connections;
}
MYSQL_STMT* STDCALL mysql_stmt_init(MYSQL* value)
{
    if (fake_mysql::failStmtInit.exchange(false))
        return nullptr;
    ++fake_mysql::statements;
    return reinterpret_cast<MYSQL_STMT*>(new Statement{reinterpret_cast<Connection*>(value)});
}
int STDCALL mysql_stmt_prepare(MYSQL_STMT* value, const char* query, unsigned long length)
{
    if (fake_mysql::failPrepare.exchange(false))
        return 1;
    const std::string_view text(query, length);
    statement(value).query = text;
    statement(value).parameterCount = static_cast<unsigned long>(std::count(text.begin(), text.end(), '?'));
    return 0;
}
unsigned long STDCALL mysql_stmt_param_count(MYSQL_STMT* value)
{ return statement(value).parameterCount + fake_mysql::parameterCountAdjustment.exchange(0); }
bool STDCALL mysql_stmt_bind_param(MYSQL_STMT* value, MYSQL_BIND* parameters)
{
    if (fake_mysql::failBindParam.exchange(false))
        return true;
    statement(value).parameters = parameters;
    return false;
}
bool STDCALL mysql_stmt_bind_result(MYSQL_STMT* value, MYSQL_BIND* results)
{ statement(value).results = results; return false; }
int STDCALL mysql_stmt_execute(MYSQL_STMT* value)
{
    ++fake_mysql::executions;
    if (fake_mysql::failExecutions.load() > 0)
    {
        --fake_mysql::failExecutions;
        return 1;
    }
    if (!fake_mysql::ready)
        return 1;
    auto& stmt = statement(value);
    stmt.fetched = 0;
    stmt.nextRow = 0;
    stmt.rows.clear();
    stmt.scripted = false;
    if (const auto handler = fake_mysql::preparedHandler.load())
    {
        try
        {
            stmt.rows = handler(stmt.query, {stmt.parameters, stmt.parameterCount}, stmt.results);
            stmt.scripted = true;
            return 0;
        }
        catch (...)
        {
            ++fake_mysql::scriptErrors;
            return 1;
        }
    }
    std::int32_t parameter = 0;
    if (stmt.parameters)
        std::memcpy(&parameter, stmt.parameters[0].buffer, sizeof(parameter));
    stmt.value = parameter + 100;
    stmt.fetched = 0;
    return 0;
}
int STDCALL mysql_stmt_fetch(MYSQL_STMT* value)
{
    auto& stmt = statement(value);
    if (stmt.scripted)
    {
        if (stmt.nextRow != 0 && fake_mysql::failFetchAfterRow.exchange(false))
            return 1;
        if (stmt.nextRow == stmt.rows.size())
            return MYSQL_NO_DATA;
        const auto& row = stmt.rows[stmt.nextRow++];
        bool truncated = false;
        for (std::size_t column = 0; column < row.size(); ++column)
        {
            const auto& cell = row[column];
            auto& binding = stmt.results[column];
            std::memset(binding.buffer, 0, binding.buffer_length);
            std::memcpy(binding.buffer, cell.data(), std::min<std::size_t>(cell.size(), binding.buffer_length));
            const bool overflow = cell.size() > binding.buffer_length;
            if (binding.length)
                *binding.length = static_cast<unsigned long>(cell.size());
            if (binding.is_null)
                *binding.is_null = false;
            if (binding.error)
                *binding.error = overflow;
            truncated = truncated || overflow;
        }
        return truncated ? MYSQL_DATA_TRUNCATED : 0;
    }
    if (stmt.fetched++ != 0)
        return fake_mysql::failFetchAfterRow.exchange(false) ? 1 : MYSQL_NO_DATA;
    if (stmt.results)
        std::memcpy(stmt.results[0].buffer, &stmt.value, sizeof(stmt.value));
    return 0;
}
bool STDCALL mysql_stmt_free_result(MYSQL_STMT*) { return fake_mysql::failFreeResults.exchange(0) > 0; }
bool STDCALL mysql_stmt_close(MYSQL_STMT* value)
{
    auto& stmt = statement(value);
    if (!fake_mysql::ready || !stmt.connection->alive)
        ++fake_mysql::orderErrors;
    // A real client may inspect bound buffers while closing a prepared statement.
    if (stmt.parameters)
    {
        volatile auto firstByte = *static_cast<const unsigned char*>(stmt.parameters[0].buffer);
        (void)firstByte;
    }
    delete &stmt;
    --fake_mysql::statements;
    return fake_mysql::failClose.exchange(false);
}
}
