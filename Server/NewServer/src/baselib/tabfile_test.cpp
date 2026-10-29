#include "stdafx.h"
#include "tabfile.h"

#define COLUMN_COUNT    (6)

const char* g_pszTestCase[][COLUMN_COUNT + 1] =
{
    {"Field1",  "Field2",   "Field3",   "Field4",   "Field5",   "Field6",   "\r\n"},
    {"01",      "02",       "03",       "04",       "05",       "06",       "\n"},
    {"11",      "12",       "13",       "14",       "15",       "16",       "\n"},
    {"\"21\"",  "\"\"\"\"", "\"\"\"\"", "\"\"\"\"", "\"\"\"\"", "\"\"\"\"", "\n"},
    {"",        "",         "",         "",         "",         "",         "\n"},
    {"  ",      "  ",       "  ",       "  ",       "  ",       "  ",       "\n"},
};

const char* g_pszTestResult[][COLUMN_COUNT + 1] =
{
    {"Field1",  "Field2",   "Field3",   "Field4",   "Field5",   "Field6",   "\r\n"},
    {"01",      "02",       "03",       "04",       "05",       "06",       "\n"},
    {"11",      "12",       "13",       "14",       "15",       "16",       "\r"},
    {"21",      "\"",       "\"",       "\"",       "\"",       "\"",       "\n"},
    {"",        "",         "",         "",         "",         "",         "\n"},
    {"  ",      "  ",       "  ",       "  ",       "  ",       "  ",       "\n"},
};

BOOL TestTabFile(void)
{
    INT32 nRetCode = 0;
    size_t uWriteSize = 0;
    FILE* pFile = NULL;
    char  szTabFile[1024 * 16];
    size_t uTabFileSize = 0;
    CTabFile oTabFile;

    /************************************************************************/

    pFile = fopen("c:\\test.txt", "wb");
    LOG_PROCESS_ERROR(pFile);

    uTabFileSize = 0;
    for (INT32 nRowIndex = 0; nRowIndex < 1; nRowIndex++)
    {
        for (INT32 nColIndex = 0; nColIndex < 1; nColIndex++)
        {
            size_t uFieldSize = strlen(g_pszTestCase[nRowIndex][nColIndex]);
            memcpy(szTabFile + uTabFileSize, g_pszTestCase[nRowIndex][nColIndex], uFieldSize);
            uTabFileSize += uFieldSize;

            szTabFile[uTabFileSize++] = '\t';
        }

        uTabFileSize--;
        size_t uFieldSize = strlen(g_pszTestCase[nRowIndex][COLUMN_COUNT]);
        memcpy(szTabFile + uTabFileSize, g_pszTestCase[nRowIndex][COLUMN_COUNT], uFieldSize);
        uTabFileSize += uFieldSize;
    }
    
    uWriteSize = fwrite(szTabFile, 1, uTabFileSize, pFile);
    LOG_PROCESS_ERROR(uWriteSize == uTabFileSize);

    fclose(pFile);
    pFile = NULL;

    nRetCode = oTabFile.Init("c:\\test.txt", 1);
    LOG_PROCESS_ERROR(nRetCode);

    for (INT32 nRowIndex = 0; nRowIndex < oTabFile.GetRowCount(); nRowIndex++)
    {
        for (INT32 nColIndex = 0; nColIndex < oTabFile.GetColCount(); nColIndex++)
        {
            char szValue[256];
            nRetCode = oTabFile.ReadData(nColIndex, nRowIndex, szValue, 256);
            if (!nRetCode)
            {
                CRI("Row %d Col %d : NULL string");
                continue;
            }
            if (strcmp(szValue, g_pszTestResult[nRowIndex][nColIndex]) != 0)
            {
                CRI("Row %d Col %d : %s but expect %s", nRowIndex, nColIndex, 
                    szValue, g_pszTestResult[nRowIndex][nColIndex]);
            }
        }
    }

    oTabFile.UnInit();

    /************************************************************************/

    pFile = fopen("c:\\test.txt", "wb");
    LOG_PROCESS_ERROR(pFile);

    uTabFileSize = 0;
    for (INT32 nRowIndex = 0; nRowIndex < sizeof(g_pszTestCase) / sizeof(g_pszTestCase[0]); nRowIndex++)
    {
        for (INT32 nColIndex = 0; nColIndex < COLUMN_COUNT; nColIndex++)
        {
            size_t uFieldSize = strlen(g_pszTestCase[nRowIndex][nColIndex]);
            memcpy(szTabFile + uTabFileSize, g_pszTestCase[nRowIndex][nColIndex], uFieldSize);
            uTabFileSize += uFieldSize;

            szTabFile[uTabFileSize++] = '\t';
        }

        uTabFileSize--;
        size_t uFieldSize = strlen(g_pszTestCase[nRowIndex][COLUMN_COUNT]);
        memcpy(szTabFile + uTabFileSize, g_pszTestCase[nRowIndex][COLUMN_COUNT], uFieldSize);
        uTabFileSize += uFieldSize;
    }

    uWriteSize = fwrite(szTabFile, 1, uTabFileSize, pFile);
    LOG_PROCESS_ERROR(uWriteSize == uTabFileSize);

    fclose(pFile);
    pFile = NULL;

    nRetCode = oTabFile.Init("c:\\test.txt", 1);
    LOG_PROCESS_ERROR(nRetCode);

    for (INT32 nRowIndex = 0; nRowIndex < oTabFile.GetRowCount(); nRowIndex++)
    {
        for (INT32 nColIndex = 0; nColIndex < oTabFile.GetColCount(); nColIndex++)
        {
            char szValue[256];
            nRetCode = oTabFile.ReadData(nColIndex, nRowIndex, szValue, 256);
            if (!nRetCode)
            {
                CRI("Row %d Col %d : NULL string");
                continue;
            }
            if (strcmp(szValue, g_pszTestResult[nRowIndex][nColIndex]) != 0)
            {
                CRI("Row %d Col %d : %s but expect %s", nRowIndex, nColIndex, 
                    szValue, g_pszTestResult[nRowIndex][nColIndex]);
            }
        }
    }

    oTabFile.UnInit();

    return TRUE;
Exit0:
    if (pFile)
        fclose(pFile);

    oTabFile.UnInit();

    return FALSE;
}
