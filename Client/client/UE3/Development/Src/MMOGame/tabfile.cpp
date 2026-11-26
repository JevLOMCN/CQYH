#include "stdafx.h"
#include "tabfile.h"
#include "../../../../../Common/utility/tab_parser.h"

#include <stdio.h>

#define UTF8_IGNORE_ERROR		0x01
#define UTF8_SKIP_BOM			0x02
#define UTF32_BOM               (0x0000FEFF)
#define UTF16_BOM               (0xFEFF)
#define UTF8_BOM                (0xBFBBEF)

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
    FArchive* file = NULL;
    unsigned short bom = 0;
    INT totalSize = 0;
    size_t characterCount = 0;
    std::vector<size_t> offsets;

    if (!pcszFileName || m_pData || m_pOffsetTable)
        return false;
    file = GFileManager->CreateFileReader(pcszFileName);
    if (!file)
        return false;
    totalSize = file->TotalSize();
    if (totalSize <= (INT)sizeof(bom) ||
        (totalSize - sizeof(bom)) % sizeof(TCHAR) != 0)
        goto Exit0;
    file->Serialize(&bom, sizeof(bom));
    if (file->IsError() || bom != UTF16_BOM)
        goto Exit0;

    m_uSize = totalSize - sizeof(bom);
    characterCount = m_uSize / sizeof(TCHAR);
    m_pData = new TCHAR[characterCount + 1];
    file->Serialize(m_pData, m_uSize);
    m_pData[characterCount] = 0;
    if (file->IsError())
        goto Exit0;
    file->Close();
    delete file;
    file = NULL;

    if (!ClientTable::Parse(m_pData, characterCount, offsets, m_nColCount, m_nRowCount))
        goto Exit0;
    m_pOffsetTable = new int[offsets.size()];
    for (size_t i = 0; i < offsets.size(); ++i)
        m_pOffsetTable[i] = (int)offsets[i];
    for (int column = 0; column < m_nColCount; ++column)
    {
        const TCHAR* value = GetData(column, 0);
        if (!value || !m_mapName2Col.insert(std::make_pair(value, column)).second)
            goto Exit0;
    }
    return true;
Exit0:
    if (file)
    {
        file->Close();
        delete file;
    }
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

const TCHAR* CTabFile::GetData(int nCol, int nRow) const
{
	LOG_PROCESS_ERROR(nCol >= 0 && nCol < m_nColCount);
	LOG_PROCESS_ERROR(nRow >= 0 && nRow < m_nRowCount);

	return m_pData + m_pOffsetTable[nRow * m_nColCount + nCol];
Exit0:
	return NULL;
}

const TCHAR* CTabFile::GetData(const TCHAR* pcszColName, int nRow) const
{
	MAP_NAME_2_COL::const_iterator it;

	it = m_mapName2Col.find(pcszColName);
	LOG_PROCESS_ERROR(it != m_mapName2Col.end());

	return GetData(it->second, nRow);
Exit0:
	return NULL;
}

int CTabFile::GetKeyColIdx( const TCHAR* pcszColName ) const
{
	MAP_NAME_2_COL::const_iterator it;

	it = m_mapName2Col.find(pcszColName);
	LOG_PROCESS_ERROR(it != m_mapName2Col.end());

	return it->second;

Exit0:
	return -1;
}
