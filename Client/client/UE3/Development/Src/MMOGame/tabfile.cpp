#include "stdafx.h"
#include "tabfile.h"

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
	int nRetCode = 0;
    FILE* pFile = NULL;
    unsigned int uBOM = 0;
    size_t nStringIndex = 0;
    int nOffsetIndex = 0;
    size_t nIndex = 0;
    bool bInQuote = false;
    int nTabCount = 0;
	wchar_t* pszData = NULL;
    TCHAR* pszTmp = NULL;

	m_nColCount = 1;
	m_nRowCount = 0;

	LOG_PROCESS_ERROR(pcszFileName);

	////////////////////////////////////////////////////////////////////////////
	//pFile = _tfopen(pcszFileName, _T("rb"));
	//LOG_PROCESS_ERROR(pFile);

	//fseek(pFile, 0, SEEK_END);
	//m_uSize = ftell(pFile);
	//fseek(pFile, 0, SEEK_SET);

	////m_uSize = pFile->Size();
	//LOG_PROCESS_ERROR(m_uSize > 0);

	//nRetCode = fread(&uBOM, sizeof(unsigned int), 1, pFile);
	//LOG_PROCESS_ERROR(nRetCode == 1);

	//FArchiveEncode::Decode((BYTE*)&uBOM, sizeof(UINT));

	//LOG_PROCESS_ERROR((uBOM & 0xFFFF) == UTF16_BOM);

	//if ((uBOM & 0xFFFF) == UTF16_BOM)
	//{
	//	m_uSize -= 2;
	//	nRetCode = fseek(pFile, 2, SEEK_SET);
	//	LOG_PROCESS_ERROR(nRetCode == 0);
	//}
	//else if ((uBOM & 0xFFFFFF) == UTF8_BOM)
	//{
	//	m_uSize -= 3;
	//	//nRetCode = pFile->Seek(3, SEEK_SET);
	//	nRetCode = fseek(pFile, 3, SEEK_SET);
	//	LOG_PROCESS_ERROR(nRetCode == 0);
	//}
	//else	// utf8 default
	//{
	//	//nRetCode = pFile->Seek(0, SEEK_SET);
	//	nRetCode = fseek(pFile, 0, SEEK_SET);
	//	LOG_PROCESS_ERROR(nRetCode == 0);
	//}

	//LOG_PROCESS_ERROR(m_pData == NULL);
	//m_pData = new TCHAR[m_uSize/2];
	//memset(m_pData, 0, m_uSize);
	//LOG_PROCESS_ERROR(m_pData);

	//nRetCode = fread(m_pData, m_uSize, 1, pFile);
	////nRetCode = pFile->Read(m_pData, (Uint)m_uSize);
	//LOG_PROCESS_ERROR(nRetCode == 1);

	//FArchiveEncode::Decode((BYTE*)m_pData, m_uSize);

	////pFile->Close();
	//fclose(pFile);
	//pFile = NULL;

	//////////////////zhaojun@snda change to FileManager/////////////////////
	FArchive* file = GFileManager->CreateFileReader(pcszFileName);//_T("rb")

	LOG_PROCESS_ERROR(file);

	m_uSize = file->TotalSize();// ftell(pFile);

	LOG_PROCESS_ERROR(m_uSize > 0);

	file->Serialize(&uBOM, sizeof(unsigned int));//fread(&uBOM, sizeof(unsigned int), 1, pFile);
	
	//LOG_PROCESS_ERROR(nRetCode == 1);

	//FArchiveEncode::Decode((BYTE*)&uBOM, sizeof(UINT));//Decode in file->Serialize

	LOG_PROCESS_ERROR((uBOM & 0xFFFF) == UTF16_BOM);

	if ((uBOM & 0xFFFF) == UTF16_BOM)
	{
		m_uSize -= 2;
		/*nRetCode = */file->Seek(2);//fseek(pFile, 2, SEEK_SET);
		//LOG_PROCESS_ERROR(nRetCode == 0);
	}
	else if ((uBOM & 0xFFFFFF) == UTF8_BOM)
	{
		m_uSize -= 3;
		/*nRetCode = */file->Seek(3);
		//nRetCode = fseek(pFile, 3, SEEK_SET);
		//LOG_PROCESS_ERROR(nRetCode == 0);
	}
	else	// utf8 default
	{
		/*nRetCode = */file->Seek(0);
		//nRetCode = fseek(pFile, 0, SEEK_SET);
		//LOG_PROCESS_ERROR(nRetCode == 0);
	}

	LOG_PROCESS_ERROR(m_pData == NULL);
	m_pData = new TCHAR[m_uSize/2];
	memset(m_pData, 0, m_uSize);
	LOG_PROCESS_ERROR(m_pData);

	/*nRetCode = */file->Serialize(m_pData, m_uSize);
	//LOG_PROCESS_ERROR(nRetCode == 1);
	//FArchiveEncode::Decode((BYTE*)m_pData, m_uSize);//Decode in file->Serialize

	//fclose(pFile);
	file->Close();
	delete file;
	//////////////////zhaojun@snda change to FileManager end////////////////////

	// convert
	if ((uBOM & 0xFFFF) == UTF16_BOM)
	{
//         pszTmp = new TCHAR[m_uSize * 3 / 2];
//         memset(pszTmp, 0, m_uSize * 3 / 2);
// 
//         pszData = (wchar_t*)m_pData;
//         m_pData = new TCHAR[m_uSize * 3 / 2];
//         LOG_PROCESS_ERROR(m_pData);
// 
// 		
// 		//m_uSize = utf16_2_utf8(pszData, m_uSize / 2, pszTmp, m_uSize * 3 / 2);
// 		//LOG_PROCESS_ERROR(m_uSize > 0);
// 
// 		memcpy(m_pData, pszTmp, m_uSize);
//         //SAFE_DELETE_ARRAY(pszTmp);
// 		delete[] pszTmp;
	}

	// first scan
	m_nColCount = 1;
	m_nRowCount = 0;
	const size_t maxSz = m_uSize / 2;
	while (nIndex < maxSz)
	{
		switch (m_pData[nIndex])
		{
		case TEXT('\t'):
			if (m_nRowCount == 0 && !bInQuote)
				m_nColCount++;
			nTabCount++;
			break;

		case TEXT('\r'):
            if (!bInQuote)
            {
                m_pData[nIndex] = 0;
            }
            break;

		case TEXT('\n'):
		    if (!bInQuote)
			{
		        if (nIndex - 1 >= 0 && m_pData[nIndex - 1] == TEXT('\r'))
                {
                    nIndex++;
                    continue;
                }
				m_nRowCount++;
			}
			break;

		case TEXT('\"'):
			if (bInQuote)
			{
				if (m_pData[nIndex + 1] != TEXT('\"'))
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

	LOG_PROCESS_ERROR(m_pOffsetTable == NULL);
	m_pOffsetTable = new int[m_nColCount * m_nRowCount];
	LOG_PROCESS_ERROR(m_pOffsetTable);

	// second scan
	nIndex = 0;

	bInQuote = false;
	while (nIndex < maxSz)
	{
		switch (m_pData[nIndex])
		{
		case TEXT('\t'):
		case TEXT('\n'):
			if (!bInQuote)
			{
				m_pData[nIndex] = 0;

				while ((nStringIndex < nIndex) && 
                    (m_pData[nStringIndex] == 0 ||
                    m_pData[nStringIndex] == TEXT('\r') ||
					m_pData[nStringIndex] == TEXT('\n')))
					nStringIndex++;

				LOG_PROCESS_ERROR(nOffsetIndex < m_nColCount * m_nRowCount);
				m_pOffsetTable[nOffsetIndex] = nStringIndex;
				nOffsetIndex++;

				nStringIndex = nIndex + 1;
			}
			break;

		case TEXT('\"'):
			if (bInQuote)
			{
				if (m_pData[nIndex + 1] == TEXT('\"'))
				{
					int nTempIndex = nIndex;
					while (m_pData[nTempIndex] != 0 && m_pData[nTempIndex+1] != TEXT('\n') && m_pData[nTempIndex+1] != TEXT('\t'))
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

	TCHAR* pcszValue = 0;
	// create name map
	for (int nIndex = 0; nIndex < m_nColCount; nIndex++)
	{
		std::pair<MAP_NAME_2_COL::iterator, bool> InsRet;

		pcszValue = (TCHAR*)GetData(nIndex, 0);
		LOG_PROCESS_ERROR(pcszValue);

		InsRet = m_mapName2Col.insert(std::make_pair(pcszValue, nIndex));
#if !MMO_FINAL_SHIPPING
		if(!InsRet.second)
		{
			if(pcszValue && appStrlen(pcszValue) > 0)
			{
				warnf(NAME_Error, TEXT("Duplicate header field: '%s' column %d, file: %s"), pcszValue, nIndex, pcszFileName);
				appMsgf(AMT_OK, TEXT("Duplicate header field: '%s' column %d, file: %s"), pcszValue, nIndex, pcszFileName);
				goto Exit0;
			}
			else
			{
				warnf(NAME_Error, TEXT("Empty header field! column %d, file: %s"), nIndex, pcszFileName);
				appMsgf(AMT_OK, TEXT("Empty header field! column %d, file: %s"), nIndex, pcszFileName);
				goto Exit0;
			}
		}
#endif
		LOG_PROCESS_ERROR(InsRet.second);
	}

	SAFE_DELETE_ARRAY(pszData)

	return true;

Exit0:
	
	if (pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

	SAFE_DELETE_ARRAY(pszData)
	SAFE_DELETE_ARRAY(pszTmp)

	UnInit();

	return false;
}

bool CTabFile::UnInit(void)
{
	SAFE_DELETE_ARRAY(m_pData)
	SAFE_DELETE_ARRAY(m_pOffsetTable)

    m_mapName2Col.clear();

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
