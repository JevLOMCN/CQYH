#include "stdafx.h"
#include "db_save_role.h"
#include "role_db_def.h"
#include "../logic/mem_mgr.h"
#include "zlib.h"

#define SAVE_ROLE "UPDATE role SET LoginTime = now(), BaseData = ?, RoleData = ? WHERE RoleID = ?"

CDBSaveRole::CDBSaveRole()
{
    m_stmt = nullptr;
    m_dwRoleID = 0;
    memset(m_bind_param, 0, sizeof(m_bind_param));
}

CDBSaveRole::~CDBSaveRole()
{

};

BOOL CDBSaveRole::Init(MYSQL* mysql)
{
    cqyh::MysqlStmtPtr prepared;
    INT32 nRet = 0;
    UINT32 param_count = 0;

    LOG_PROCESS_ERROR(mysql);
    LOG_PROCESS_ERROR(!m_stmt);

    prepared.reset(mysql_stmt_init(mysql));
    LOG_PROCESS_ERROR(prepared.get());

    nRet = mysql_stmt_prepare(prepared.get(), SAVE_ROLE, (unsigned long)strlen(SAVE_ROLE));
    LOG_PROCESS_ERROR(nRet == 0);

    param_count = mysql_stmt_param_count(prepared.get());
    LOG_PROCESS_ERROR(param_count == 3);

    m_stmt = std::move(prepared);
    return TRUE;

Exit0:
    return FALSE;
}

BOOL CDBSaveRole::UnInit()
{
    return cqyh::CloseMysqlStatement(m_stmt);
}

BOOL CDBSaveRole::DoSaveRole(char* buf, UINT32 bufsize)
{
    ROLE_DB_DATA* pRoleData = (ROLE_DB_DATA*)buf;
    ROLE_DB_BASE_DATA* pBaseData = (ROLE_DB_BASE_DATA*)(pRoleData->RoleData + pRoleData->BlockTable[rbtBase].dwStartOffset);
    unsigned long base_data_len = sizeof(ROLE_DB_BASE_DATA);
    unsigned long role_data_len = bufsize;

    unsigned char zipped_role_data[MEM_SIZE];
    unsigned long zipped_role_data_len = MEM_SIZE;
    INT32 nRet = compress(zipped_role_data, &zipped_role_data_len, (unsigned char*)pRoleData, role_data_len);
    //debug
    DBG("save roleid:%u, original data len:%u, zipped data len:%u, zipped data crc:%X", m_dwRoleID, role_data_len, zipped_role_data_len,
        crc32(0, (const Bytef*)zipped_role_data, zipped_role_data_len));
    //end debug
    LOG_PROCESS_ERROR(nRet == Z_OK);

    memset(&m_bind_param, 0, sizeof(m_bind_param));

    m_bind_param[0].buffer_type = MYSQL_TYPE_BLOB;
    m_bind_param[0].buffer = pBaseData;
    m_bind_param[0].buffer_length = base_data_len;
    m_bind_param[0].length = &base_data_len;

    m_bind_param[1].buffer_type = MYSQL_TYPE_BLOB;
    m_bind_param[1].buffer = zipped_role_data;
    m_bind_param[1].buffer_length = zipped_role_data_len;
    m_bind_param[1].length = &zipped_role_data_len;

    m_bind_param[2].buffer_type = MYSQL_TYPE_LONG;
    m_bind_param[2].buffer = &m_dwRoleID;

    nRet = mysql_stmt_bind_param(m_stmt.get(), m_bind_param);
    LOG_PROCESS_ERROR(nRet == 0);

    nRet = mysql_stmt_execute(m_stmt.get());
    LOG_PROCESS_ERROR(nRet == 0);

    return TRUE;

Exit0:
    DBG("%d, %s", mysql_stmt_errno(m_stmt.get()), mysql_stmt_error(m_stmt.get()));
    return FALSE;
}