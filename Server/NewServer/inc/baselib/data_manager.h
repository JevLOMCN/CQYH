#ifndef _DATA_MANAGER_H_
#define _DATA_MANAGER_H_

#include <string.h>
#include <time.h>
#include "tabfile.h"
#include "time_ex.h"
#include "ifile.h"
#include "read_data_helper.h"
#include "dir.h"
#include "global_data_manager.h"

template <class T>
class CDataManager
{
public:
	CDataManager();
	virtual ~CDataManager();

	static CDataManager& GetInstance(void);

    typedef BOOL (*POST_PROCESS_DATA_FUNC)(T* pData, BOOL bInit);
	BOOL Init(const char* pcszConfigName, const char* pcszPath, POST_PROCESS_DATA_FUNC pFunc);
	BOOL UnInit(void);
    virtual BOOL Reload(BOOL bThread, BOOL bInit);
	virtual BOOL PostProcessData(void);

	const T* GetData(INT32 nDataID);
	INT32 GetID(const char* pcszName);
	virtual INT32 GetDataCount(void);
	virtual UINT32 GetDataSize(void);
	virtual UINT32 GetCRC(void);

	template<class TFunc>
	inline BOOL Traverse(TFunc& rFunc);

    const T* GetFirstData(void);
	const T* GetNextData();

	T* GetWritableData(INT32 nDataID);

private:
	BOOL _SetData(INT32 nDataID, const char* pcszName, T* pData);
	BOOL _LoadCache(LOADED_DATA_LIST* pLoadedDataList, UINT32 dwCRC);
	virtual BOOL _Load(LOADED_DATA_LIST* pLoadedDataList);
    virtual BOOL _Insert(LOADED_DATA_LIST* pLoadedDataList);

	static CDataManager<T>  ms_Instance;

	typedef std::map<INT32, T*>		    MAP_ID_2_DATA;
	MAP_ID_2_DATA	        m_mapID2Data;

	struct ltstr
	{
		bool operator()(const char* s1, const char* s2) const
		{
			return strcmp(s1, s2) < 0;
		}
	};
	typedef std::map<const char*, INT32, ltstr>	MAP_NAME_2_ID;
	MAP_NAME_2_ID	        m_mapName2ID;

	char                    m_szConfigName[256];
	char                    m_szPath[256];
	POST_PROCESS_DATA_FUNC  m_pFunc;
	BOOL					m_bNeedPostProcessData;
    UINT32                  m_dwCRC;

	typename MAP_ID_2_DATA::iterator m_it;

    friend class CGlobalDataManager;
};

template<class T>
CDataManager<T> CDataManager<T>::ms_Instance;

template<class T>
CDataManager<T>::CDataManager()
{
    m_szConfigName[0] = 0;
    m_szPath[0] = 0;
    m_pFunc = NULL;
    m_dwCRC = 0;
}

template<class T>
CDataManager<T>::~CDataManager()
{
    UnInit();
}

template<class T>
CDataManager<T>& CDataManager<T>::GetInstance(void)
{
	return ms_Instance;
}

template<class T>
BOOL CDataManager<T>::Init(const char* pcszConfigName, const char* pcszPath, POST_PROCESS_DATA_FUNC pFunc)
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pcszConfigName);
    LOG_PROCESS_ERROR(pcszPath);

    strxcpy(m_szConfigName, pcszConfigName, 256);
    strxcpy(m_szPath, pcszPath, 256);
    m_pFunc = pFunc;
	m_bNeedPostProcessData = FALSE;
    m_dwCRC = 0;

    nRetCode = CGlobalDataManager::GetInstance().AddDataManager(pcszConfigName, this);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = Reload(FALSE, TRUE);
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:

    return FALSE;
}

template<class T>
BOOL CDataManager<T>::Reload(BOOL bThread, BOOL bInit)
{
    INT32 nRetCode = 0;
    LOAD_TASK* pLoadTask = NULL;
    LOADED_DATA_LIST* pLoadedDataList = NULL;

    pLoadedDataList = new LOADED_DATA_LIST;
    LOG_PROCESS_ERROR(pLoadedDataList);

    pLoadedDataList->bLoaded = FALSE;
    pLoadedDataList->bSkipped = FALSE;
    pLoadedDataList->bInit = bInit;
    strxcpy(pLoadedDataList->szConfigName, m_szConfigName, 256);
    pLoadedDataList->qwStartTick = GetTickCountEx();

    if (bThread)
    {
        pLoadedDataList->bOnlyLoadCache = TRUE;

        pLoadTask = new LOAD_TASK;
        LOG_PROCESS_ERROR(pLoadTask);

        pLoadTask->nTaskType = lttLoadData;
        pLoadTask->pTaskData = pLoadedDataList;

        nRetCode = CGlobalDataManager::GetInstance().PushTask(pLoadTask);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        pLoadedDataList->bOnlyLoadCache = FALSE;

        nRetCode = _Load(pLoadedDataList);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = _Insert(pLoadedDataList);
        LOG_PROCESS_ERROR(nRetCode);

        SAFE_DELETE(pLoadedDataList);
    }

    return TRUE;
Exit0:
    SAFE_DELETE(pLoadTask);
    SAFE_DELETE(pLoadedDataList);

    return FALSE;
}

template<class T>
BOOL CDataManager<T>::PostProcessData(void)
{
	INT32 nRetCode = 0;

	if (m_pFunc && m_bNeedPostProcessData)
	{
		for (auto it = m_mapID2Data.begin();
			it != m_mapID2Data.end(); ++it)
		{
			nRetCode = m_pFunc(it->second, TRUE);
			if (!nRetCode)
			{
				WRN("Failed to post process data id %d in %s data file", it->first, m_szConfigName);
				continue;
			}
		}
	}

	m_bNeedPostProcessData = FALSE;

	return TRUE;
}

template<class T>
BOOL CDataManager<T>::_LoadCache(LOADED_DATA_LIST* pLoadedDataList, UINT32 dwCRC)
{
	INT32 nRetCode = 0;
	char szFileName[MAX_PATH];
	IFile* pCacheFile = NULL;
	UINT32 dwFileSize = 0;
	char* pCacheFileData = NULL;
	DATA_CACHE_HEADER* pHeader = NULL;
	LOADED_DATA* pLoadedData = NULL;

	LOG_PROCESS_ERROR(pLoadedDataList);

	snprintf(szFileName, MAX_PATH, "cache/%s/%s.txt", m_szPath, m_szConfigName);
	pCacheFile = g_OpenFile(szFileName, "rb");
	PROCESS_ERROR(pCacheFile);

	dwFileSize = pCacheFile->Size();
	pCacheFileData = new char[dwFileSize];
	LOG_PROCESS_ERROR(pCacheFileData);

	nRetCode = pCacheFile->Read(pCacheFileData, dwFileSize);
	LOG_PROCESS_ERROR(nRetCode == dwFileSize);
	PROCESS_ERROR(dwFileSize >= sizeof(DATA_CACHE_HEADER));

	pHeader = (DATA_CACHE_HEADER*)pCacheFileData;

	if (pHeader->dwTag == CACHE_FILE_TAG && pHeader->dwVersion == CACHE_FILE_VERSION &&
		pHeader->dwCRC == dwCRC && pHeader->dwDataSize == sizeof(T) &&
		pHeader->dwFileSize == dwFileSize)
	{
		if (dwCRC == m_dwCRC)
		{
			pLoadedDataList->bSkipped = TRUE;
		}
		else
		{
			INT32 nRowCount = (dwFileSize - sizeof(DATA_CACHE_HEADER)) / (sizeof(LOADED_DATA) + sizeof(T));

			char* pPointer = pCacheFileData;
			pPointer += sizeof(DATA_CACHE_HEADER);

			for (INT32 nRowIndex = 0; nRowIndex < nRowCount; nRowIndex++)
			{
				pLoadedData = new LOADED_DATA;
				LOG_PROCESS_ERROR(pLoadedData);

				memcpy(pLoadedData, pPointer, sizeof(LOADED_DATA));
				pPointer += sizeof(LOADED_DATA);
				pLoadedData->pData = NULL;

				pLoadedData->pData = new T;
				LOG_PROCESS_ERROR(pLoadedData->pData);

				memcpy(pLoadedData->pData, pPointer, sizeof(T));
				pPointer += sizeof(T);

				if (pLoadedData->pcszName == NULL)
					pLoadedData->pcszName = "Unknown Name";
				else
					pLoadedData->pcszName += (INT64)pLoadedData->pData;

				pLoadedDataList->DataList.push_back(pLoadedData);
				pLoadedData = NULL;
			}

			pLoadedDataList->nRowCount = nRowCount;
		}

		pLoadedDataList->bLoaded = TRUE;
	}

	SAFE_DELETE_ARRAY(pCacheFileData);

	nRetCode = pCacheFile->Close();
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	SAFE_DELETE(pLoadedData);
	SAFE_DELETE_ARRAY(pCacheFileData);
	if (pCacheFile)
		pCacheFile->Close();

	return FALSE;
}

template<class T>
BOOL CDataManager<T>::_Load(LOADED_DATA_LIST* pLoadedDataList)
{
	INT32 nRetCode = 0;
    char szFileName[MAX_PATH];
	CTabFile oTabFile;
	INT32 nRowIndex = 0;
    LOADED_DATA* pLoadedData = NULL;
    char* pCacheFileData = NULL;
    FILE* pNewCacheFile = NULL;
    DATA_CACHE_HEADER* pHeader = NULL;

    LOG_PROCESS_ERROR(pLoadedDataList);

    snprintf(szFileName, MAX_PATH, "%s/%s.txt", m_szPath, m_szConfigName);
    nRetCode = oTabFile.Init(szFileName, 1);
    LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _LoadCache(pLoadedDataList, oTabFile.GetCRC());
	LOG_CHECK_ERROR(nRetCode);

    if (!pLoadedDataList->bLoaded && !pLoadedDataList->bOnlyLoadCache)
    {
        char* pPointer = NULL;
        DATA_CACHE_HEADER* pHeader = NULL;

        snprintf(szFileName, MAX_PATH, "cache/%s", m_szPath);
        g_CreateDirectory(szFileName, TRUE);
        snprintf(szFileName, MAX_PATH, "cache/%s/%s.txt", m_szPath, m_szConfigName);

        pNewCacheFile = fopen(szFileName, "wb");
        if (pNewCacheFile)
        {
            UINT32 dwCacheFileSize = sizeof(DATA_CACHE_HEADER) + (sizeof(LOADED_DATA) + sizeof(T)) * oTabFile.GetRowCount();

            pCacheFileData = new char[dwCacheFileSize];
            LOG_PROCESS_ERROR(pCacheFileData);

            pPointer = pCacheFileData;

            pHeader = (DATA_CACHE_HEADER*)pPointer;
            pPointer += sizeof(DATA_CACHE_HEADER);
        }

        for (nRowIndex = 0; nRowIndex < oTabFile.GetRowCount(); nRowIndex++)
        {
            pLoadedData = new LOADED_DATA;
            LOG_PROCESS_ERROR(pLoadedData);

            pLoadedData->nRowIndex = nRowIndex;
            pLoadedData->nID = -1;
            pLoadedData->pcszName = "Unknown Name";
            pLoadedData->pData = new T;
            LOG_PROCESS_ERROR(pLoadedData->pData);
            memset(pLoadedData->pData, 0, sizeof(T));

            nRetCode = T::ReadLine(pLoadedData->nID, pLoadedData->pcszName, (T*)pLoadedData->pData, &oTabFile, nRowIndex);
            if (!nRetCode)
            {
                WRN("Failed to read data at row %d id %d name %s in %s data file", nRowIndex + 2,pLoadedData->nID, pLoadedData->pcszName, m_szConfigName);
                SAFE_DELETE(pLoadedData->pData);
                continue;
            }

            if (pNewCacheFile)
            {
                if (strcmp(pLoadedData->pcszName, "Unknown Name") == 0)
                {
                    pLoadedData->pcszName = NULL;
                    memcpy(pPointer, pLoadedData, sizeof(LOADED_DATA));
                    pPointer += sizeof(LOADED_DATA);
                    pLoadedData->pcszName = "Unknown Name";
                }
                else
                {
                    const char* pcszName = pLoadedData->pcszName;
                    pLoadedData->pcszName = (char*)(pLoadedData->pcszName - (char*)pLoadedData->pData);
                    LOG_PROCESS_ERROR((INT64)(pLoadedData->pcszName) >= 0 && (INT64)(pLoadedData->pcszName) < sizeof(T));

                    memcpy(pPointer, pLoadedData, sizeof(LOADED_DATA));
                    pPointer += sizeof(LOADED_DATA);

                    pLoadedData->pcszName = pcszName;
                }

                memcpy(pPointer, pLoadedData->pData, sizeof(T));
                pPointer += sizeof(T);
            }

            pLoadedDataList->DataList.push_back(pLoadedData);
            pLoadedData = NULL;
        }

#if defined(WIN64)
        if (pNewCacheFile)
        {
            pHeader->dwTag = CACHE_FILE_TAG;
            pHeader->dwVersion = CACHE_FILE_VERSION;
            pHeader->dwCRC = oTabFile.GetCRC();
            pHeader->dwFileSize = (UINT32)(sizeof(DATA_CACHE_HEADER) + 
                (sizeof(LOADED_DATA) + sizeof(T)) * pLoadedDataList->DataList.size());
            pHeader->dwDataSize = sizeof(T);

            nRetCode = (INT32)fwrite(pCacheFileData, 1, pHeader->dwFileSize, pNewCacheFile);
            LOG_PROCESS_ERROR(nRetCode == pHeader->dwFileSize);

            fclose(pNewCacheFile);
            pNewCacheFile = NULL;
        }
#endif  // WIN64

        SAFE_DELETE_ARRAY(pCacheFileData);

        pLoadedDataList->nRowCount = oTabFile.GetRowCount();
        pLoadedDataList->bLoaded = TRUE;
    }

	m_dwCRC = oTabFile.GetCRC();

	nRetCode = oTabFile.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
    CRI("failed to read data in %s", m_szConfigName);

    SAFE_DELETE(pLoadedData);

    SAFE_DELETE_ARRAY(pCacheFileData);

    if (pNewCacheFile)
        fclose(pNewCacheFile);

	nRetCode = oTabFile.UnInit();
	LOG_CHECK_ERROR(nRetCode);

	UnInit();

	return FALSE;
}

template<class T>
BOOL CDataManager<T>::_Insert(LOADED_DATA_LIST* pLoadedDataList)
{
    INT32 nRetCode = 0;
    INT32 nCounter = 0;
    typename MAP_ID_2_DATA::iterator itData;

    LOG_PROCESS_ERROR(pLoadedDataList);
    LOG_PROCESS_ERROR(pLoadedDataList->bLoaded);

    if (!pLoadedDataList->bSkipped)
    {
        UINT64 qwStartTick = GetTickCountEx();

        for (std::list<LOADED_DATA*>::iterator it = pLoadedDataList->DataList.begin();
            it != pLoadedDataList->DataList.end(); ++it)
        {
            LOADED_DATA* pLoadedData = *it;

            if (!pLoadedDataList->bInit)
            {
                itData = m_mapID2Data.find(pLoadedData->nID);
            }

            if (!pLoadedDataList->bInit && itData != m_mapID2Data.end())
            {
                if (m_pFunc)
                {
                    nRetCode = m_pFunc(itData->second, FALSE);
                    if (!nRetCode)
                    {
                        WRN("Failed to post process original data at row %d id %d name %s in %s data file", 
                            pLoadedData->nRowIndex + 2, pLoadedData->nID, pLoadedData->pcszName, m_szConfigName);
                        continue;
                    }
                }
                memcpy(itData->second, pLoadedData->pData, sizeof(T));
            }
            else
            {
                nRetCode = _SetData(pLoadedData->nID, pLoadedData->pcszName, (T*)pLoadedData->pData);
                if (!nRetCode)
                {
                    WRN("duplicated id %d or name %s at row %d in %s data file", 
                        pLoadedData->nID, pLoadedData->pcszName, pLoadedData->nRowIndex + 2, m_szConfigName);
                    continue;
                }

                pLoadedData->pData = NULL;
            }

            nCounter++;
        }
        UINT64 qwCurrentTick = GetTickCountEx();
        INF("read %d / %d data in %s successfully (cost %d ms %d ms %.2f MB)", nCounter, pLoadedDataList->nRowCount, m_szConfigName, 
            (INT32)(qwCurrentTick - pLoadedDataList->qwStartTick), (INT32)(qwCurrentTick - qwStartTick), (sizeof(T) + sizeof(void*) * 4) * nCounter / (1024 * 1024.0f));
#if defined(_DEBUG) & !defined(_FINAL) 
        if (nCounter < pLoadedDataList->nRowCount)
            g_AppendErrorMsg("%s.txt中有---%d---条数据错误\n", m_szConfigName, pLoadedDataList->nRowCount - nCounter);
#endif  // _DEBUG & !_FINAL
		m_bNeedPostProcessData = TRUE;
    }
    else
    {
//        INF("skipped to read data in %s", m_szConfigName); 
    }

    return TRUE;
Exit0:
    CRI("failed to read data in %s", m_szConfigName);

    return FALSE;
}

template<class T>
BOOL CDataManager<T>::UnInit(void)
{
	typename MAP_ID_2_DATA::iterator it;
	for (it = m_mapID2Data.begin();
		it != m_mapID2Data.end(); ++it)
	{
		const T* pData = it->second;
		SAFE_DELETE(pData);
	}

	m_mapID2Data.clear();
	m_mapName2ID.clear();

	return TRUE;
}

template<class T>
const T* CDataManager<T>::GetData(INT32 nDataID)
{
	typename MAP_ID_2_DATA::iterator it;

	it = m_mapID2Data.find(nDataID);
	if (it != m_mapID2Data.end())
		return it->second;

	return NULL;
}

template<class T>
INT32 CDataManager<T>::GetID(const char* pcszName)
{
	typename MAP_NAME_2_ID::iterator it;

	it = m_mapName2ID.find(pcszName);
	if (it != m_mapName2ID.end())
		return it->second;

	return NULL;
}

template<class T>
INT32 CDataManager<T>::GetDataCount(void)
{
	return (INT32)m_mapID2Data.size();
}

template<class T>
UINT32 CDataManager<T>::GetDataSize(void)
{
	return sizeof(T);
}

template<class T>
UINT32 CDataManager<T>::GetCRC(void)
{
	return m_dwCRC;
}

template<class T>
BOOL CDataManager<T>::_SetData(INT32 nDataID, const char* pcszName, T* pData)
{
	typename MAP_ID_2_DATA::iterator itID;
	typename MAP_NAME_2_ID::iterator itName;

	PROCESS_ERROR(pData);

	itID = m_mapID2Data.find(nDataID);
	PROCESS_ERROR(itID == m_mapID2Data.end());

    if (strcmp(pcszName, "Unknown Name") != 0)
    {
        itName = m_mapName2ID.find(pcszName);
        PROCESS_ERROR(itName == m_mapName2ID.end());

        m_mapName2ID[pcszName] = nDataID;
    }

    m_mapID2Data[nDataID] = pData;

	return TRUE;
Exit0:
	return FALSE;
}

template<class T>
template<class TFunc>
inline BOOL CDataManager<T>::Traverse(TFunc& rFunc)
{
	INT32 nRetCode = 0;

	for (typename MAP_ID_2_DATA::iterator it = m_mapID2Data.begin();
		it != m_mapID2Data.end(); ++it)
	{
		nRetCode = rFunc(it->second);
		PROCESS_ERROR(nRetCode);
	}

	return TRUE;
Exit0:
	return FALSE;
}

template<class T>
const T* CDataManager<T>::GetFirstData(void)
{
	m_it = m_mapID2Data.begin();
	if (m_it == m_mapID2Data.end())
		return NULL;
	else
		return m_it->second;
}

template<class T>
const T* CDataManager<T>::GetNextData()
{
	if (m_it == m_mapID2Data.end())
		return NULL;
	else
	{
		++m_it;

		if (m_it == m_mapID2Data.end())
			return NULL;

		return m_it->second;
	}
}

template<class T>
T* CDataManager<T>::GetWritableData(INT32 nDataID)
{
	typename MAP_ID_2_DATA::iterator it;

	it = m_mapID2Data.find(nDataID);
	if (it != m_mapID2Data.end())
		return it->second;

	return NULL;
}

#endif	// _DATA_MANAGER_H_
