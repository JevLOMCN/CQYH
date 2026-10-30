#include "stdafx.h"
#include "tabfile.h"

#include <stdio.h>
#include <cstdlib>
#include <memory>
#ifdef __linux__
#include <iconv.h>
#endif
#include "zlib.h"

#include "utf8.h"
#include "file_owner.h"
#include "number_parse.h"
#include <span>
#include <limits>

BOOL CTabFile::Init(const TCHAR* pcszFileName, INT32 nSkipLine)
{
	INT32 nRetCode = 0;
    cqyh::FilePtr pFile;
    UINT32 uBOM = 0;
    INT32 nStringIndex = 0;
    INT32 nOffsetIndex = 0;
    INT32 nIndex = 0;
    BOOL bInQuote = FALSE;
    INT32 fieldsInRow = 0;
    std::size_t offsetCount = 0;
    std::unique_ptr<char[]> convertedData;

    if (m_pData || !m_pOffsetTable.empty())
    {
        LOG_CHECK_ERROR(!m_pData && m_pOffsetTable.empty());
        return FALSE;
    }
	m_nSkipLine = nSkipLine;
	m_nColCount = 1;
	m_nRowCount = 0;

	LOG_PROCESS_ERROR(pcszFileName);

	pFile.reset(g_OpenFile(pcszFileName, "rb"));
	LOG_PROCESS_ERROR(pFile);
	
	m_uSize = pFile->Size();
	LOG_PROCESS_ERROR(m_uSize > 0 && m_uSize < static_cast<std::size_t>((std::numeric_limits<INT32>::max)()));

	nRetCode = pFile->Read(&uBOM, sizeof(uBOM));
	LOG_PROCESS_ERROR(nRetCode == sizeof(uBOM));

	//comment by chy, 2015/9/2
    //LOG_PROCESS_ERROR((uBOM & 0xFFFF) == UTF16_BOM);

	if ((uBOM & 0xFFFF) == UTF16_BOM)
	{
		m_uSize -= 2;
		nRetCode = pFile->Seek(2, SEEK_SET);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else if ((uBOM & 0xFFFFFF) == UTF8_BOM)
	{
		m_uSize -= 3;
		nRetCode = pFile->Seek(3, SEEK_SET);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else	// utf8 default
	{
		nRetCode = pFile->Seek(0, SEEK_SET);
		LOG_PROCESS_ERROR(nRetCode);
	}

	m_pData = std::make_unique<char[]>(m_uSize + 1);
	LOG_PROCESS_ERROR(m_pData);

    nRetCode = pFile->Read(m_pData.get(), (UINT32)m_uSize);
	LOG_PROCESS_ERROR(nRetCode == m_uSize);

    pFile.reset();

    // The class owns m_pData; the temporary conversion buffer owns itself.
    // Keep a sentinel for the parser's one-character lookahead.
    if ((uBOM & 0xFFFF) == UTF16_BOM)
    {
        LOG_PROCESS_ERROR(m_uSize % 2 == 0);
        const auto capacity = m_uSize * 3 / 2;
        convertedData = std::make_unique<char[]>(capacity + 1);
#ifdef __linux__
        const auto handle = iconv_open("UTF-8", "UTF-16LE");
        LOG_PROCESS_ERROR(handle != reinterpret_cast<iconv_t>(-1));
        struct IconvOwner
        {
            iconv_t handle;
            ~IconvOwner() { iconv_close(handle); }
        } converter{handle};
        char* input = m_pData.get();
        char* output = convertedData.get();
        size_t inputRemaining = m_uSize;
        size_t outputRemaining = capacity;
        nRetCode = iconv(handle, &input, &inputRemaining, &output, &outputRemaining);
        LOG_PROCESS_ERROR(nRetCode != -1);
        m_uSize = capacity - outputRemaining;
#else
        const auto convertedSize = utf16_2_utf8(reinterpret_cast<const wchar_t*>(m_pData.get()),
                                               m_uSize / 2, convertedData.get(), capacity);
        LOG_PROCESS_ERROR(convertedSize > 0);
        m_uSize = convertedSize;
#endif
        m_pData = std::move(convertedData);
    }

    LOG_PROCESS_ERROR(m_uSize < static_cast<std::size_t>((std::numeric_limits<INT32>::max)()));
    m_dwCRC32 = crc32(0, reinterpret_cast<const Bytef*>(m_pData.get()), static_cast<uInt>(m_uSize));

	// first scan
    m_nColCount = 1;
    m_nRowCount = 0;
    while (nIndex < m_uSize)
    {
	    switch (m_pData[nIndex])
	    {
	    case '\t':
		    if (m_nRowCount == 0 && !bInQuote)
			    m_nColCount++;
		    break;
	    case '\r':
            if (!bInQuote)
            {
                m_pData[nIndex] = 0;
            }
            break;
	    case '\n':
	        if (!bInQuote)
		    {
	            if (nIndex - 1 >= 0 && m_pData[nIndex - 1] == '\r')
                {
                    nIndex++;
                    continue;
                }
			    m_nRowCount++;
		    }
		    break;
	    case '\"':
		    if (bInQuote)
		    {
			    if (m_pData[nIndex + 1] != '\"')
				    bInQuote =  false;
			    else
				    nIndex++;
		    }
		    else
			    bInQuote = true;
		    break;
	    }

	    nIndex++;
    }

    LOG_PROCESS_ERROR(m_nRowCount > 0 && m_nColCount > 0);
    LOG_PROCESS_ERROR(static_cast<std::size_t>(m_nRowCount) <= m_uSize / static_cast<std::size_t>(m_nColCount));
    offsetCount = static_cast<std::size_t>(m_nColCount) * m_nRowCount;
    // Every recorded field needs a delimiter; reject impossible rectangular shapes
    // before allocating, including sparse inputs with enormous header/row products.
    LOG_PROCESS_ERROR(offsetCount <= m_uSize);
    m_pOffsetTable.assign(offsetCount, -1);

	// second scan
	nIndex = 0;

	bInQuote = false;
	while (nIndex < m_uSize)
	{
		switch (m_pData[nIndex])
		{
		case '\t':
		case '\n':
			if (!bInQuote)
			{
                const bool endOfRow = m_pData[nIndex] == '\n';
				m_pData[nIndex] = 0;

				while ((nStringIndex < nIndex) && 
                    (m_pData[nStringIndex] == 0 ||
                    m_pData[nStringIndex] == '\r' ||
					m_pData[nStringIndex] == '\n'))
					nStringIndex++;

				LOG_PROCESS_ERROR(static_cast<std::size_t>(nOffsetIndex) < m_pOffsetTable.size());
				m_pOffsetTable[nOffsetIndex] = nStringIndex;
				nOffsetIndex++;
                ++fieldsInRow;
                if (endOfRow)
                {
                    LOG_PROCESS_ERROR(fieldsInRow == m_nColCount);
                    fieldsInRow = 0;
                }

				nStringIndex = nIndex + 1;
			}
			break;
		case '\"':
			if (bInQuote)
			{
				if (m_pData[nIndex + 1] == '\"')
				{
					int nTempIndex = nIndex;
					while (m_pData[nTempIndex] != 0 && m_pData[nTempIndex+1] != '\n' && m_pData[nTempIndex+1] != '\t')
					{
						m_pData[nTempIndex] = m_pData[nTempIndex + 1];
						nTempIndex++;
					}
					m_pData[nTempIndex] = 0;
				}
				else
				{
					bInQuote =  false;
					m_pData[nIndex] = 0;
				}
			}
			else
			{
				bInQuote = true;
				m_pData[nIndex] = 0;
			}

			break;
		}

		nIndex++;
	}

    LOG_PROCESS_ERROR(static_cast<std::size_t>(nOffsetIndex) == m_pOffsetTable.size());
	// create name map
	for (int nIndex = 0; nIndex < m_nColCount; nIndex++)
	{
		std::pair<MAP_NAME_2_COL::iterator, bool> InsRet;

		const char* pcszValue = GetData(nIndex, 0);
		LOG_PROCESS_ERROR(pcszValue);

		InsRet = m_mapName2Col.insert(std::make_pair(pcszValue, nIndex));
		LOG_PROCESS_ERROR(InsRet.second);
	}

	return TRUE;
Exit0:
	UnInit();

	return FALSE;
}

BOOL CTabFile::UnInit(void)
{
    m_pData.reset();
    std::vector<INT32>{}.swap(m_pOffsetTable);
    m_uSize = 0;
    m_nSkipLine = 0;
    m_nRowCount = 0;
    m_nColCount = 0;
    m_dwCRC32 = 0;

    m_mapName2Col.clear();

	return true;
}

INT32 CTabFile::GetRowCount(void)
{
    const auto count = static_cast<std::int64_t>(m_nRowCount) - 1 - m_nSkipLine;
    return count > 0 && count <= (std::numeric_limits<INT32>::max)() ? static_cast<INT32>(count) : 0;
}

INT32 CTabFile::GetColCount(void)
{
	return m_nColCount;
}

const char* CTabFile::GetColName(INT32 nCol)
{
    return GetData(nCol, 0);
}


const char* CTabFile::GetData(INT32 nCol, INT32 nRow) const
{
    if (nCol < 0 || nCol >= m_nColCount)
    {
        LOG_CHECK_ERROR(nCol >= 0 && nCol < m_nColCount);
        return nullptr;
    }
    const auto row = nRow;
    if (row < 0 || row >= m_nRowCount || !m_pData || m_pOffsetTable.empty())
    {
        LOG_CHECK_ERROR(row >= 0 && row < m_nRowCount && m_pData && !m_pOffsetTable.empty());
        return nullptr;
    }
    const std::span<const INT32> offsets(m_pOffsetTable);
    const auto fieldAt = [&](std::size_t index) -> const char*
    {
        const auto offset = offsets[index * m_nColCount + nCol];
        if (offset < 0 || static_cast<std::size_t>(offset) > m_uSize)
        {
            LOG_CHECK_ERROR(offset >= 0 && static_cast<std::size_t>(offset) <= m_uSize);
            return nullptr;
        }
        return m_pData.get() + offset;
    };
    return fieldAt(static_cast<std::size_t>(row));
}

const char* CTabFile::GetValueData(INT32 nCol, INT32 nRow) const
{
    // Column names address physical row zero. Only value reads skip the header.
    const auto row = static_cast<std::int64_t>(nRow) + 1 + m_nSkipLine;
    if (row < 0 || row >= m_nRowCount)
    {
        LOG_CHECK_ERROR(row >= 0 && row < m_nRowCount);
        return nullptr;
    }
    const auto* field = GetData(nCol, static_cast<INT32>(row));
    if (field != nullptr && *field == '\0' && m_nRowCount > 2)
        return GetData(nCol, 2);
    return field;
}

std::optional<INT32> CTabFile::FindColumn(std::string_view name) const
{
    const auto column = m_mapName2Col.find(name);
    if (column == m_mapName2Col.end())
    {
        LOG_CHECK_ERROR(column != m_mapName2Col.end());
        return std::nullopt;
    }
    return column->second;
}

BOOL CTabFile::ReadData(INT32 nCol, INT32 nRow, char* pszValue, INT32 nSize)
{
    const auto* data = GetValueData(nCol, nRow);
    if (data == nullptr)
        return FALSE;
    if (pszValue == nullptr || nSize < 0)
    {
        LOG_CHECK_ERROR(pszValue != nullptr && nSize >= 0);
        return FALSE;
    }
    strxcpy(pszValue, data, static_cast<std::size_t>(nSize));
    return TRUE;
}

BOOL CTabFile::ReadData(const char* pcszColName, int nRow, char* pszValue, INT32 nSize)
{
    if (pcszColName == nullptr)
    {
        LOG_CHECK_ERROR(pcszColName != nullptr);
        return FALSE;
    }
    return ReadData(std::string_view{pcszColName}, nRow, pszValue, nSize);
}

BOOL CTabFile::ReadData(std::string_view columnName, INT32 nRow, char* pszValue, INT32 nSize)
{
    const auto column = FindColumn(columnName);
    return column ? ReadData(*column, nRow, pszValue, nSize) : FALSE;
}

BOOL CTabFile::ReadData(INT32 nCol, INT32 nRow, INT32& rnValue)
{
    const auto* data = GetValueData(nCol, nRow);
    if (data == nullptr)
        return FALSE;
    const auto parsed = cqyh::ParseIntegerPrefix(data);
    if (!parsed && parsed.error() == std::errc::result_out_of_range)
    {
        // The shipped MaxExp sentinel exceeds INT32. Retain the legacy CRT limit
        // behavior here; callers needing checked conversion use ParseIntegerPrefix.
        rnValue = static_cast<INT32>(std::strtol(data, nullptr, 10));
        return TRUE;
    }
    // Empty/non-numeric fields retain atoi's zero result; defaults were selected above.
    rnValue = parsed.value_or(0);
    return TRUE;
}

BOOL CTabFile::ReadData(const char* pcszColName, int nRow, INT32& rnValue)
{
    if (pcszColName == nullptr)
    {
        LOG_CHECK_ERROR(pcszColName != nullptr);
        return FALSE;
    }
    return ReadData(std::string_view{pcszColName}, nRow, rnValue);
}

BOOL CTabFile::ReadData(std::string_view columnName, INT32 nRow, INT32& rnValue)
{
    const auto column = FindColumn(columnName);
    return column ? ReadData(*column, nRow, rnValue) : FALSE;
}

UINT32 CTabFile::GetCRC(void)
{
    return m_dwCRC32;
}
