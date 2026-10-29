#include <cstdio>
#include <cstring>
#include <type_traits>
#include <initializer_list>
#include "std_type.h"
#include "process_error.h"
#include "mysql_owner.h"
#include "fake_mysql_client.h"
#include "../../src/dbserver/db/db_create_role.h"
#include "../../src/dbserver/db/db_delete_role.h"
#include "../../src/dbserver/db/db_update_role_delete_time.h"
#include "../../src/dbserver/db/db_list_role.h"
#include "../../src/dbserver/db/db_load_role.h"
#include "../../src/dbserver/db/db_save_role.h"
#include "../../src/gmserver/gm_db_operation.h"
#include "../../src/gmserver/gm_data.h"
#include "../../src/gmserver/prep_stmt.h"

namespace
{
int checks = 0;
int failures = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}
template<class Wrapper>
void check_statement()
{
    static_assert(!std::is_copy_constructible_v<Wrapper>);
    auto connection = cqyh::MakeMysql();
    const auto before = fake_mysql::statements.load();
    {
        Wrapper wrapper;
        fake_mysql::failStmtInit = true;
        check(!wrapper.Init(connection.get()) && fake_mysql::statements == before,
              "statement allocation failure leaves no owned handle");
        fake_mysql::failPrepare = true;
        check(!wrapper.Init(connection.get()) && fake_mysql::statements == before,
              "failed preparation releases its temporary statement");
        fake_mysql::parameterCountAdjustment = 1;
        check(!wrapper.Init(connection.get()) && fake_mysql::statements == before,
              "parameter metadata mismatch releases its temporary statement");
        check(wrapper.Init(connection.get()) && fake_mysql::statements == before + 1,
              "successful initialization transfers exactly one statement");
        auto* handle = wrapper.m_stmt.get();
        check(!wrapper.Init(connection.get()) && wrapper.m_stmt.get() == handle && fake_mysql::statements == before + 1,
              "reinitialization preserves the live prepared statement");
        check(wrapper.UnInit() && !wrapper.m_stmt && fake_mysql::statements == before,
              "explicit statement cleanup releases its handle");
        check(wrapper.UnInit(), "statement cleanup is idempotent");
        check(wrapper.Init(connection.get()), "prepared wrapper can be reused after cleanup");
    }
    check(fake_mysql::statements == before, "wrapper destruction releases a live prepared statement");
}
void check_binding_failure()
{
    auto connection = cqyh::MakeMysql();
    CDBLoadRole wrapper;
    fake_mysql::failBindParam = true;
    check(!wrapper.Init(connection.get()) && !wrapper.m_stmt && fake_mysql::statements == 0,
          "parameter binding failure releases the temporary statement");
    check(wrapper.Init(connection.get()), "binding failure permits successful retry");
    fake_mysql::failClose = true;
    check(!wrapper.UnInit() && !wrapper.m_stmt && fake_mysql::statements == 0,
          "native close failure is reported without retaining a closed handle");
}
void check_connections()
{
    fake_mysql::failLibrary = true;
    check(!cqyh::MakeMysql() && !fake_mysql::ready && fake_mysql::connections == 0,
          "client initialization failure acquires no connection or lease");
    fake_mysql::failInit = true;
    check(!cqyh::MakeMysql() && !fake_mysql::ready && fake_mysql::connections == 0,
          "connection allocation failure releases its library lease");
    auto first = cqyh::MakeMysql();
    auto second = cqyh::MakeMysql();
    check(first && second && first.get() != second.get() && fake_mysql::connections == 2,
          "connection factories return independent owned handles");
    auto moved = std::move(second);
    first = cqyh::MysqlPtr{};
    check(!second && moved && fake_mysql::ready && fake_mysql::connections == 1,
          "moving a connection preserves its lease after another owner closes");
    moved = cqyh::MysqlPtr{};
    check(!fake_mysql::ready && fake_mysql::connections == 0,
          "the final connection ends its retained client-library lease");
}
void check_thread_context()
{
    {
        cqyh::MysqlThreadContext context;
        check(context && fake_mysql::ready && fake_mysql::threads == 1,
              "client thread context retains initialization without an open connection");
    }
    check(!fake_mysql::ready && fake_mysql::threads == 0,
          "client thread state ends before its final library lease");
    fake_mysql::failThreadInit = true;
    {
        cqyh::MysqlThreadContext context;
        check(!context && fake_mysql::threads == 0, "failed client thread initialization acquires no thread state");
    }
    check(!fake_mysql::ready, "failed client thread initialization releases its library lease");
}
void check_prepared_connection()
{
    DBSetting setting{};
    std::strcpy(setting.szServerAddr, "fake");
    std::strcpy(setting.szDBName, "fake");
    CPrepStmt wrapper;
    for (auto* failure : {&fake_mysql::failConnect, &fake_mysql::failStmtInit,
                          &fake_mysql::failPrepare})
    {
        *failure = true;
        check(!wrapper.Init(&setting, "select 1") && fake_mysql::connections == 0 && fake_mysql::statements == 0,
              "prepared connection initialization failure releases both native handles");
        check(!fake_mysql::ready, "failed prepared connection releases its client lease");
    }
    fake_mysql::failExecutions = 1;
    check(!wrapper.Init(&setting, "select 1") && fake_mysql::connections == 0 && fake_mysql::statements == 0,
          "failed initial execution releases statement and connection");
    check(wrapper.Init(&setting, "select 1"), "prepared connection succeeds after failure");
    auto* statement = wrapper.GetStmt();
    check(!wrapper.Init(&setting, "select 2") && wrapper.GetStmt() == statement,
          "prepared connection rejects reinitialization without replacing resources");
    check(wrapper.UnInit() && fake_mysql::statements == 0 && fake_mysql::connections == 0 && !fake_mysql::ready,
          "prepared connection closes its statement before connection and library");
    check(wrapper.UnInit(), "prepared connection cleanup is idempotent");
    {
        CPrepStmt scoped;
        check(scoped.Init(&setting, "select 1"), "initialize destructor cleanup fixture");
    }
    check(fake_mysql::connections == 0 && fake_mysql::statements == 0 && !fake_mysql::ready,
          "prepared connection destructor releases all resources");
}
}
int main()
{
    CLog::GetInstance().SetOutputMask(lotNone);
    check_connections();
    check_thread_context();
    check_statement<CDBCreateRole>();
    check_statement<CDBDeleteRole>();
    check_statement<CDBUpdateRoleDeleteTime>();
    check_statement<CDBListRole>();
    check_statement<CDBLoadRole>();
    check_statement<CDBSaveRole>();
    check_statement<CDBItemLogWithGuid>();
    check_statement<CDBItemLogWithTempID>();
    check_statement<CDBItemLogWithRoleGuid>();
    check_statement<CDBItemLogWithRoleTempID>();
    check_statement<CDBItemLogWithRole>();
    check_binding_failure();
    check_prepared_connection();
    check(fake_mysql::orderErrors == 0, "native resources close in a valid library/connection/buffer order");
    std::printf("%d MySQL handle checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
