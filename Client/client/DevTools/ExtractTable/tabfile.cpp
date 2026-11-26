#include "stdafx.h"
#include "tabfile.h"
#include "../../../Common/utility/tab_parser.h"
#include <climits>

#include <stdio.h>

#include "utf8.h"
//#include "ifile.h"

#define LOG_PROCESS_ERROR(__condition__)							\
	do																\
	{																\
	if (!(__condition__))										\
		{															\
		printf("LOG_PROCESS_ERROR("#__condition__") at line %d in function %s() file \"%s\"", \
		__LINE__, __FUNCTION__, __FILE__);                  \
		goto Exit0;												\
		}															\
	} while (0)

#define SAFE_DELETE(__pointer__)				\
	do											\
	{											\
	if (__pointer__)						\
		{										\
		delete (__pointer__);				\
		(__pointer__) = NULL;				\
		}										\
	} while (0);

#define SAFE_DELETE_ARRAY(__pointer__)			\
	do											\
	{											\
	if (__pointer__)						\
		{										\
		delete [] (__pointer__);			\
		(__pointer__) = NULL;				\
		}										\
	} while (0);

CTabFile::CTabFile()
{
	m_pData = NULL;
	m_uSize = 0;
	m_nRowCount = 0;
	m_nColCount = 0;
	m_pOffsetTable = NULL;
}

CTabFile::~CTabFile()
{
	UnInit();
}

bool CTabFile::Init(const TCHAR* pcszFileName)
{
    FILE* file = NULL;
    unsigned short bom = 0;
    long totalSize = 0;
    wchar_t* wideData = NULL;
    size_t units = 0;
    size_t capacity = 0;
    int converted = 0;
    std::vector<size_t> offsets;

    if (!pcszFileName || m_pData || m_pOffsetTable)
        return false;
    file = _tfopen(pcszFileName, _T("rb"));
    if (!file)
        return false;
    if (fseek(file, 0, SEEK_END) != 0)
        goto Exit0;
    totalSize = ftell(file);
    if (totalSize <= (long)sizeof(bom) || fseek(file, 0, SEEK_SET) != 0 ||
        (totalSize - sizeof(bom)) % sizeof(wchar_t) != 0)
        goto Exit0;
    if (fread(&bom, sizeof(bom), 1, file) != 1 || bom != UTF16_BOM)
        goto Exit0;
    units = (totalSize - sizeof(bom)) / sizeof(wchar_t);
    if (units > (INT_MAX - 1) / 3)
        goto Exit0;
    wideData = new wchar_t[units];
    if (fread(wideData, sizeof(wchar_t), units, file) != units)
        goto Exit0;
    fclose(file);
    file = NULL;
    capacity = units * 3;
    m_pData = new char[capacity + 1];
    converted = utf16_2_utf8(wideData, units, m_pData, capacity);
    if (converted <= 0 || (size_t)converted > capacity)
        goto Exit0;
    m_uSize = converted;
    m_pData[m_uSize] = 0;
    delete[] wideData;
    wideData = NULL;
    if (!ClientTable::Parse(m_pData, m_uSize, offsets, m_nColCount, m_nRowCount))
        goto Exit0;
    m_pOffsetTable = new int[offsets.size()];
    for (size_t i = 0; i < offsets.size(); ++i)
        m_pOffsetTable[i] = (int)offsets[i];
    for (int column = 0; column < m_nColCount; ++column)
    {
        const char* value = GetData(column, 0);
        if (!value || !m_mapName2Col.insert(std::make_pair(value, column)).second)
            goto Exit0;
    }
    return true;
Exit0:
    if (file)
        fclose(file);
    delete[] wideData;
    UnInit();
    return false;
}

bool CTabFile::UnInit(void)
{
	SAFE_DELETE_ARRAY(m_pData)
	SAFE_DELETE_ARRAY(m_pOffsetTable)

    m_mapName2Col.clear();
    m_uSize = 0;
    m_nRowCount = 0;
    m_nColCount = 0;

	return true;
}

int CTabFile::GetRowCount(void)
{
	return m_nRowCount;
}

int CTabFile::GetColCount(void)
{
	return m_nColCount;
}

const char* CTabFile::GetData(int nCol, int nRow)
{
	LOG_PROCESS_ERROR(nCol >= 0 && nCol < m_nColCount);
	LOG_PROCESS_ERROR(nRow >= 0 && nRow < m_nRowCount);

	return m_pData + m_pOffsetTable[nRow * m_nColCount + nCol];
Exit0:
	return NULL;
}

const char* CTabFile::GetData(const char* pcszColName, int nRow)
{
	MAP_NAME_2_COL::iterator it;

	it = m_mapName2Col.find(pcszColName);
	LOG_PROCESS_ERROR(it != m_mapName2Col.end());

	return GetData(it->second, nRow);
Exit0:
	return NULL;
}
