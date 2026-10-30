#include "stdafx.h"
#include "gm_log.h"
#include <time.h>
#include "time_ex.h"
#include "server_config.h"
#include "user_manager.h"
#include "dir.h"
#include "ifile.h"

//#define MAX_GMLOG_PARAM_SIZE          (65535)
#define MAX_GM_RECORD_MESSAGE_LEN	(1024 * 16)

CGMLog CGMLog::m_Instance;

CGMLog& CGMLog::GetInstance()
{
    return m_Instance;
}

CGMLog::CGMLog()
{
    memset(m_pFile, 0, sizeof(m_pFile));
    memset(m_dwCreateFileTime, 0, sizeof(m_dwCreateFileTime));
}

CGMLog::~CGMLog()
{
    memset(m_pFile, 0, sizeof(m_pFile));
    memset(m_dwCreateFileTime, 0, sizeof(m_dwCreateFileTime));
}

BOOL CGMLog::Init()
{
    INT32       nRetCode = 0;

    memset(m_pFile, 0, sizeof(m_pFile));
    memset(m_dwCreateFileTime, 0, sizeof(m_dwCreateFileTime));

    m_nTimeZone = get_time_zone();

    for (INT32 i = glfBegin; i < glfNum; ++i)
    {
        CheckFile(i);
    }
    
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMLog::UnInit()
{
    INT32       nRetCode = 0;

    for (INT32 i = glfBegin; i < glfNum; i++)
    {
        if (m_pFile[i])
            fclose(m_pFile[i]);
    }


    return TRUE;
}

BOOL CGMLog::CheckFile(INT32 eLogFileEnum)
{
    INT32       nRetCode = 0;

    char* pMessage = NULL;
    UINT32 dwCurrentTime = time(NULL);

    LOG_PROCESS_ERROR(eLogFileEnum > glfNone && eLogFileEnum < glfNum);

    if ((dwCurrentTime+ m_nTimeZone) / 86400 != (m_dwCreateFileTime[eLogFileEnum] + m_nTimeZone) / 86400)
    {
        if (m_pFile[eLogFileEnum])
        {
            fclose(m_pFile[eLogFileEnum]);
            m_pFile[eLogFileEnum] = NULL;
        }
    }

    if (m_pFile[eLogFileEnum] == NULL)
    {
        char szFileName[MAX_PATH];

        const tm_ex* pNow = localtime_ex();
        snprintf(szFileName, MAX_PATH, "logdata/%04d-%02d-%02d", pNow->tm_year, pNow->tm_mon, pNow->tm_mday);

        nRetCode = g_CreateDirectory("logdata", TRUE);
        LOG_CHECK_ERROR(nRetCode);

        nRetCode = g_CreateDirectory(szFileName, TRUE);
        LOG_CHECK_ERROR(nRetCode);

        switch (eLogFileEnum)
        {
        case glfGM:
        {
            snprintf(szFileName, MAX_PATH, "logdata/%04d-%02d-%02d/GM_%04d_%02d_%02d.txt",
                pNow->tm_year, pNow->tm_mon, pNow->tm_mday,
                pNow->tm_year, pNow->tm_mon, pNow->tm_mday);
        }
            break;
        case glfItemLog:
        {
            snprintf(szFileName, MAX_PATH, "logdata/%04d-%02d-%02d/gmitem_%04d_%02d_%02d.txt",
                pNow->tm_year, pNow->tm_mon, pNow->tm_mday,
                pNow->tm_year, pNow->tm_mon, pNow->tm_mday);
        }
            break;
        case glfMoneyLog:
        {
            snprintf(szFileName, MAX_PATH, "logdata/%04d-%02d-%02d/gmmoney_%04d_%02d_%02d.txt",
                pNow->tm_year, pNow->tm_mon, pNow->tm_mday,
                pNow->tm_year, pNow->tm_mon, pNow->tm_mday);
        }
            break;
        default:
            LOG_PROCESS_ERROR(FALSE);
            break;
        }

        m_pFile[eLogFileEnum] = fopen(szFileName, "ab+");
        LOG_PROCESS_ERROR(m_pFile[eLogFileEnum]);

        nRetCode = fseek(m_pFile[eLogFileEnum], 0, SEEK_END);
        LOG_PROCESS_ERROR(nRetCode == 0);

        if (ftell(m_pFile[eLogFileEnum]) == 0)
        {
            INT32 nSize = 0;
            
            switch (eLogFileEnum)
            {
            case glfGM:
            {
                nSize = strlen("Time\tAreaID\tGMName\tCommandType\tContent\r\n");
                nRetCode = fwrite("Time\tAreaID\tGMName\tCommandType\tContent\r\n", 1, nSize, m_pFile[eLogFileEnum]);
                LOG_PROCESS_ERROR(nRetCode == nSize);
            }
                break;
            case glfItemLog:
            {
                nSize = strlen("Time\tAreaID\tGroupID\tRoleID\tActionType\tGuid\tTemplateID\tItemCount\tReason\tUserDefinedID\r\n");
                nRetCode = fwrite("Time\tAreaID\tGroupID\tRoleID\tActionType\tGuid\tTemplateID\tItemCount\tReason\tUserDefinedID\r\n", 1, nSize, m_pFile[eLogFileEnum]);
                LOG_PROCESS_ERROR(nRetCode == nSize);
            }
                break;
            case glfMoneyLog:
            {
                nSize = strlen("Time\tAreaID\tGroupID\tSrcRoleID\tTargetRoleID\tAction\tMoneyType\tMoneyCount\tReason\tParam\r\n");
                nRetCode = fwrite("Time\tAreaID\tGroupID\tSrcRoleID\tTargetRoleID\tAction\tMoneyType\tMoneyCount\tReason\tParam\r\n", 1, nSize, m_pFile[eLogFileEnum]);
                LOG_PROCESS_ERROR(nRetCode == nSize);
            }
                break;
            default:
                LOG_PROCESS_ERROR(FALSE);
                break;
            }

            nRetCode = fflush(m_pFile[eLogFileEnum]);
            LOG_PROCESS_ERROR(nRetCode == 0);
        }
        m_dwCreateFileTime[eLogFileEnum] = dwCurrentTime;
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMLog::WriteLog(const char *pcszUserName, const char *pcszCmdStr, const char *format, ...)
{
    INT32       nRetCode = 0;
    INT32       nMsgLen = 0;
    va_list     vaList;
    char        szTime[64] = { 0 };
    INT32	    nMessageSize = 0;

    LOG_PROCESS_ERROR(pcszCmdStr);
    LOG_PROCESS_ERROR(pcszUserName);

    CheckFile(glfGM);
    LOG_PROCESS_ERROR(m_pFile[glfGM] != NULL);

    memset(m_LogContent[glfGM], 0, MAX_LOG_LENGTH);
    // time areaid groupid roleid rolename result command
    nMsgLen = snprintf(m_LogContent[glfGM], MAX_LOG_LENGTH, "%s\t%d\t%s\t%s\t", get_time_str(time(NULL)),
        g_ServerConfig.Common.nAreaID, pcszUserName, pcszCmdStr);

    if (format)
    {
        va_start(vaList, format);
        vsnprintf(m_LogContent[glfGM] + nMsgLen, MAX_LOG_LENGTH - nMsgLen, format, vaList);
        va_end(vaList);
    }
    strcat(m_LogContent[glfGM], "\r\n");

    nMessageSize = (INT32)strnlen(m_LogContent[glfGM], MAX_GM_RECORD_MESSAGE_LEN);

    nRetCode = fwrite(m_LogContent[glfGM], 1, nMessageSize, m_pFile[glfGM]);
    LOG_PROCESS_ERROR(nRetCode == nMessageSize);

    nRetCode = fflush(m_pFile[glfGM]);
    LOG_CHECK_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMLog::WriteLogWithTime(UINT32 dwTime, const char *pcszName, const char *pcszCmdStr, const char *format, ...)
{
    INT32       nRetCode = 0;
    INT32       nMsgLen = 0;
    char        szTime[64] = { 0 };
    va_list     vaList;
    INT32	    nMessageSize = 0;

    LOG_PROCESS_ERROR(pcszCmdStr);
    LOG_PROCESS_ERROR(pcszName);

    CheckFile(glfGM);
    LOG_PROCESS_ERROR(m_pFile[glfGM] != NULL);

    memset(m_LogContent[glfGM], 0, MAX_LOG_LENGTH);
    // time areaid groupid roleid rolename result command
    nMsgLen = snprintf(m_LogContent[glfGM], MAX_LOG_LENGTH, "%s\t%d\t%s\t%s\t", get_time_str(dwTime),
        g_ServerConfig.Common.nAreaID, pcszName, pcszCmdStr);

    if (format)
    {
        va_start(vaList, format);
        vsnprintf(m_LogContent[glfGM] + nMsgLen, MAX_LOG_LENGTH - nMsgLen, format, vaList);
        va_end(vaList);
    }

    strcat(m_LogContent[glfGM], "\r\n");

    nMessageSize = (INT32)strnlen(m_LogContent[glfGM], MAX_GM_RECORD_MESSAGE_LEN);

    nRetCode = fwrite(m_LogContent[glfGM], 1, nMessageSize, m_pFile[glfGM]);
    LOG_PROCESS_ERROR(nRetCode == nMessageSize);

    nRetCode = fflush(m_pFile[glfGM]);
    LOG_CHECK_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CGMLog::WriteLogData(INT32 nFileType, const char *pcszBuffer)
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(pcszBuffer);
    LOG_PROCESS_ERROR(nFileType > glfNone && nFileType < glfNum);

    CheckFile(nFileType);
    LOG_PROCESS_ERROR(m_pFile[nFileType] != NULL);

    nRetCode = fwrite(pcszBuffer, 1, strnlen(pcszBuffer, MAX_GM_RECORD_MESSAGE_LEN), m_pFile[nFileType]);

    nRetCode = fflush(m_pFile[nFileType]);
    LOG_CHECK_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
    return FALSE;
}
