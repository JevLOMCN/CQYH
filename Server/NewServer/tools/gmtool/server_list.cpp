#include "stdafx.h"
#include "string_ex.h"
#include "server_list.h"
#include "read_data_helper.h"
#include "zlib.h"
#include "../gmtool-encrypt/encrypt/encrypt/base.h"


SERVER_LIST  g_ServerList;

BOOL SERVER_LIST_DATA::ReadLine(INT32& rnID, const char*& rpcszName, SERVER_LIST_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
    READ_START();

    READ_INT("ServerID", nServerID);
    READ_STR("ServerName", szName);
    READ_STR("IPAddr", szAddr);
    READ_INT("Port", dwPort);

    READ_END(nServerID);
}

//BOOL g_InitServerList()
//{
//    INT32       nRetCode = 0;
//    const SERVER_LIST_DATA    *pData = NULL;
//    BOOL        bRet = FALSE;
//    char        szFinalFile[128] = { 0 };
//    char        szFile[128] = { 0 };
//    char        *pBuffer = NULL;
//    INT32       nLength = 0;
//    char        *pOBuffer = NULL;
//    INT32       nOLength = 1024*1024;
//    char        szOutName[256] = { 0 };
//
//    memset(&g_ServerList, 0, sizeof(g_ServerList));
//
//    UINT32  dwcrc = crc32(0, (Bytef*)"ServerList", strlen("ServerList"));
//    sprintf_s(szFinalFile, 128, "%u", dwcrc);
//
//    sprintf(szFile, "./config/%s", szFinalFile);
//
//    FILE *pf = fopen(szFile, "rb");
//    LOG_PROCESS_ERROR(pf);
//
//    pBuffer = new char[1024 * 1024];
//    LOG_PROCESS_ERROR(pBuffer);
//
//    fseek(pf, 0, SEEK_END);
//    nLength = ftell(pf);
//    fseek(pf, 0, SEEK_SET);
//    fread(pBuffer, 1, nLength, pf);
//    fclose(pf);
//    pf = NULL;
//
//    pOBuffer = new char[1024 * 1024];
//    LOG_PROCESS_ERROR(pOBuffer);
//    memset(pOBuffer, 0, 1024 * 1024);
//
//    bRet = Decrypt(nLength, pBuffer, nOLength, pOBuffer);
//    LOG_PROCESS_ERROR(bRet);
//   
//    sprintf(szOutName, "%s%s.txt", "./config/", szFinalFile);
//
//    pf = fopen(szOutName, "wb");
//    LOG_PROCESS_ERROR(pf);
//
//    fwrite(pOBuffer, nOLength, 1, pf);
//    fclose(pf);
//    pf = NULL;
//
//    delete[] pBuffer;
//    delete[] pOBuffer;
//
//    nRetCode = CDataManager<SERVER_LIST_DATA>::GetInstance().Init(szFinalFile, "./config", NULL);
//    LOG_PROCESS_ERROR(nRetCode);
//
//    bRet = DeleteFile(szOutName);
//    LOG_PROCESS_ERROR(bRet);
//
//    pData = CDataManager<SERVER_LIST_DATA>::GetInstance().GetFirstData();
//    LOG_PROCESS_ERROR(pData);
//    while (pData)
//    {
//        g_ServerList.svrList[g_ServerList.nSvrNum].nServerID = pData->nServerID;
//        g_ServerList.svrList[g_ServerList.nSvrNum].dwPort = pData->dwPort;
//        strxcpy(g_ServerList.svrList[g_ServerList.nSvrNum].szName, pData->szName, sizeof(g_ServerList.svrList[g_ServerList.nSvrNum].szName));
//        strxcpy(g_ServerList.svrList[g_ServerList.nSvrNum].szAddr, pData->szAddr, sizeof(g_ServerList.svrList[g_ServerList.nSvrNum].szAddr));
//        ++g_ServerList.nSvrNum;
//
//        pData = CDataManager<SERVER_LIST_DATA>::GetInstance().GetNextData();
//    }
//
//    return TRUE;
//Exit0:
//    return FALSE;
//}
