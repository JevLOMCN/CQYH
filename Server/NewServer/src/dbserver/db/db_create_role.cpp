#include "stdafx.h"
#include "db_create_role.h"

#include "zlib.h"
#include "server_def.h"

#include "../logic/dbrole.h"
#include "../logic/dbuser.h"
#include "../logic/mem_mgr.h"

#define CREATE_ROLE "INSERT INTO role (GroupID, RoleID, RoleName, UserName, CreateTime, DeleteTime, LoginTime, RoleDeleted, BaseData, RoleData) VALUES (?, ?, ?, ?, now(), NULL, now(), 0, ?, ?)"

CDBCreateRole::CDBCreateRole()
{
    m_stmt = nullptr;
    memset(m_bind_param, 0, sizeof(m_bind_param));
    memset(m_bind_result, 0, sizeof(m_bind_result));
}

CDBCreateRole::~CDBCreateRole()
{

}

BOOL CDBCreateRole::Init(MYSQL* mysql)
{
    cqyh::MysqlStmtPtr prepared;
    INT32 nRet = 0;
    UINT32 param_count = 0;

    LOG_PROCESS_ERROR(mysql);
    LOG_PROCESS_ERROR(!m_stmt);

    prepared.reset(mysql_stmt_init(mysql));
    LOG_PROCESS_ERROR(prepared.get());

    nRet = mysql_stmt_prepare(prepared.get(), CREATE_ROLE, (unsigned long)strlen(CREATE_ROLE));
    LOG_PROCESS_ERROR(nRet == 0);

    param_count = mysql_stmt_param_count(prepared.get());
    LOG_PROCESS_ERROR(param_count == 6);

    m_stmt = std::move(prepared);
    return TRUE;

Exit0:
    return FALSE;
}

BOOL CDBCreateRole::UnInit()
{
    return cqyh::CloseMysqlStatement(m_stmt);
}

BOOL CDBCreateRole::DoCreateRole(UINT8 byGroupID, UINT32 roleid, char* rolename, char* username,
                                 char* base_data_buf, UINT32 base_data_buf_len, char* role_data_buf, UINT32 role_data_buf_len)
{
    unsigned long role_name_len = (unsigned long)strnlen(rolename, ROLE_NAME_LEN);
    unsigned long user_name_len = (unsigned long)strnlen(username, USER_NAME_LEN);

    unsigned char zipped_role_data[MEM_SIZE];
    unsigned long zipped_role_data_len = MEM_SIZE;

    INT32 nRet = 0;

    LOG_PROCESS_ERROR(rolename);
    LOG_PROCESS_ERROR(username);
    LOG_PROCESS_ERROR(base_data_buf);
    LOG_PROCESS_ERROR(base_data_buf_len);
    LOG_PROCESS_ERROR(role_data_buf);
    LOG_PROCESS_ERROR(role_data_buf_len);
    LOG_PROCESS_ERROR(byGroupID < MAX_SERVER_GROUP_COUNT && byGroupID >= 0);

    nRet = compress(zipped_role_data, &zipped_role_data_len, (unsigned char*)role_data_buf, role_data_buf_len);
    LOG_PROCESS_ERROR(nRet == Z_OK);

    memset(m_bind_param, 0, sizeof(m_bind_param));

    m_bind_param[0].buffer_type = MYSQL_TYPE_TINY;
    m_bind_param[0].buffer = &byGroupID;

    m_bind_param[1].buffer_type = MYSQL_TYPE_LONG;
    m_bind_param[1].buffer = &roleid;

    m_bind_param[2].buffer_type = MYSQL_TYPE_STRING;
    m_bind_param[2].buffer = rolename;
    m_bind_param[2].buffer_length = role_name_len;
    m_bind_param[2].length = &role_name_len;

    m_bind_param[3].buffer_type = MYSQL_TYPE_STRING;
    m_bind_param[3].buffer = username;
    m_bind_param[3].buffer_length = user_name_len;
    m_bind_param[3].length = &user_name_len;


    m_bind_param[4].buffer_type = MYSQL_TYPE_BLOB;
    m_bind_param[4].buffer = base_data_buf;
    m_bind_param[4].buffer_length = base_data_buf_len;
    m_bind_param[4].length = (unsigned long*)&base_data_buf_len;

    m_bind_param[5].buffer_type = MYSQL_TYPE_STRING;
    m_bind_param[5].buffer = zipped_role_data;
    m_bind_param[5].buffer_length = zipped_role_data_len;
    m_bind_param[5].length = &zipped_role_data_len;

    nRet = mysql_stmt_bind_param(m_stmt.get(), m_bind_param);
    LOG_PROCESS_ERROR(nRet == 0);

    nRet = mysql_stmt_execute(m_stmt.get());
    if (nRet != 0)
        return FALSE;

    //debug
    DBG("create roleid:%u, original data len:%u, zipped data len:%u, zipped data crc:%X", roleid, role_data_buf_len, zipped_role_data_len,
        crc32(0, (const Bytef*)zipped_role_data, zipped_role_data_len));
    //end debug

    return TRUE;

Exit0:
    DBG("%d, %s", mysql_stmt_errno(m_stmt.get()), mysql_stmt_error(m_stmt.get()));
    return FALSE;
}
