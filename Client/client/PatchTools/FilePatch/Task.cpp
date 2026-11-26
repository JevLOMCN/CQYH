#include "stdafx.h"
#include "../DownloadBuffer.h"
#include "Task.h"
#include "MD5.h"
#include "ThreadPool.h"

#define M_LOG_TO_CONSOLE 0

CThreadPool* g_genMD5ThreadPool;
CThreadPool* g_compareMD5ThreadPool;
CThreadPool* g_downloadThreadPool;
CThreadPool* g_writeFileThreadPool;

CCriticalSection g_logLock;
CCriticalSection _patchCountLock;

volatile LONG g_genMD5TaskTotalCount = 0;
volatile LONG g_genMD5TaskCompletedCount = 0;

volatile LONG g_compareMD5TaskTotalCount = 0;
volatile LONG g_compareMD5TaskCompletedCount = 0;

volatile LONG g_downloadTaskTotalCount = 0;
volatile LONG g_downloadTaskCompletedCount = 0;

volatile LONG g_writeFileTaskCompletedCount = 0;

void LogMsgToUI(const CString& msg)
{
//#if M_LOG_TO_CONSOLE
	CSingleLock lock(&g_logLock, TRUE);
	//_tprintf(_T("%s\n"), (LPCTSTR)msg);
	std::wcout << (LPCTSTR)(msg) << std::endl;
//#endif
}

void GenerateMD5Task::Run()
{
#if M_LOG_TO_CONSOLE
	CString str;
	str.Format(_T("[%d] Gen MD5:%s"), _taskID, (LPCTSTR)_srcFile);		
	LogMsgToUI(str);
#endif

	FILE* pFile = 0;
	_tfopen_s(&pFile, (LPCTSTR)_srcFile, _T("rb"));
	if(pFile != NULL)
	{
		MD5 md5(pFile);
		_targetMD5 = md5.hex_digest();
	}

	InterlockedIncrement(&g_genMD5TaskCompletedCount);
}

void CompareMD5Task::Run()
{
#if M_LOG_TO_CONSOLE
	CString str;
	str.Format(_T("[%d] Compare MD5: %d/%d"), _taskID, g_compareMD5TaskCompletedCount, g_compareMD5TaskTotalCount);		
	LogMsgToUI(str);
#endif

	SLocalVersionInfo* pInfoLocal = &_infoLocal;

	BOOL bFound = FALSE;
	for(size_t k = 0; k < _filePatch._vectorServerViInfoes.size(); ++k)
	{
		SVersionInfo* pInfoServer = &(_filePatch._vectorServerViInfoes[k]);

		if( pInfoLocal->_relatedPathName.CompareNoCase(pInfoServer->_relatedPathName) == 0 )
		{
			pInfoServer->_isMatched = TRUE;
			pInfoLocal->_isMatched = TRUE;

			if(pInfoLocal->_codeMD5 != pInfoServer->_codeMD5)
			{
				{
					CSingleLock lock(&_patchCountLock);
					_filePatch._iPatchCount++;
					_filePatch._bShouldUpdateViFile = TRUE;
				}
				if(_filePatch._strViFileName.CompareNoCase(_T("PatchInstall.vi2")) == 0)
				{
					pInfoLocal->_iOperation = EPO_Install;	// Patch Install
				}
				else
				{
					pInfoLocal->_iOperation = EPO_Patch;	// Patch
				}
				pInfoLocal->_originLen	= pInfoServer->_originLen;
			}
			bFound = TRUE;
			break;
		}
	}

	if(!bFound && pInfoLocal->_contentType != _T("Dir"))
	{
		CSingleLock lock(&_patchCountLock);
		_filePatch._iPatchCount++;
		_filePatch._bShouldUpdateViFile = TRUE;
		pInfoLocal->_iOperation = EPO_Remove;	// Remove
	}

	InterlockedIncrement(&g_compareMD5TaskCompletedCount);
}


void DownloadTask::Run()
{
	_internetSession = new CInternetSession();
	
	_httpConnection = _internetSession->GetHttpConnection(_strServerIP, _serverPort);

	CString strFilePathName = _strInitRoot;	//_T("/PatchRoot/");
	strFilePathName += /*filePatch._strServerSubDir + */_localInfo._relatedPathName;

	BOOL bCompressed = FALSE;
	if(_localInfo._originLen != 0)
	{
		bCompressed = TRUE;
		strFilePathName += _T(".zfp");
	}

#if M_LOG_TO_CONSOLE
	CString str;
	str.Format(_T("[%d] Download:%s"), _taskID, (LPCTSTR)strFilePathName);		
	LogMsgToUI(str);
#endif

	CHttpFile *pFile = _httpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET
		, strFilePathName 
		, NULL
		, 1
		, NULL
		, NULL
		, HSR_DOWNLOAD | INTERNET_FLAG_RELOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT
		); 

	if(!pFile)
	{
		// _strLog += _T("Missing Server File: ") + strFilePathName;
		// _strLog += _T("\n");
		CString str;
		str.Format(_T("Missing Server File:%s"), (LPCTSTR)strFilePathName);		
		LogMsgToUI(str);
		InterlockedIncrement(&g_downloadTaskCompletedCount);
		InterlockedIncrement(&g_writeFileTaskCompletedCount);
		return;
	}

	try
	{
		pFile->SendRequest();
	}
	catch (CException* /*e*/)
	{
		pFile->Close();

		//e->ReportError();
		// _strLog += _T("Failed to get Server File: ") + strFilePathName;
		// _strLog += _T("\n");

		CString str;
		str.Format(_T("Download Failed:%s"), (LPCTSTR)strFilePathName);		
		LogMsgToUI(str);
		InterlockedIncrement(&g_downloadTaskCompletedCount);
		InterlockedIncrement(&g_writeFileTaskCompletedCount);
		return;
	}

	DWORD dwStatusCode;
	if(pFile->QueryInfoStatusCode(dwStatusCode))
	{
		//200-299 Success 
		if( !(dwStatusCode >= 200 && dwStatusCode < 300) )
		{
			pFile->Close();

			// _strLog += _T("Failed to get Server File: ") + strFilePathName;
			// _strLog += _T("\n");
			CString str;
			str.Format(_T("Query Failed %d :%s"), dwStatusCode, (LPCTSTR)strFilePathName);
			LogMsgToUI(str);
			InterlockedIncrement(&g_downloadTaskCompletedCount);
			InterlockedIncrement(&g_writeFileTaskCompletedCount);
			return;
		}
	}

// 	CString strPutLocalFileName;
	CString strRealLocalFileName = _strInstallDir + _localInfo._relatedPathName;
// 	if(bCompressed)
// 		strPutLocalFileName = _strTmpDir + _T("~~CasualPath.tmp");
// 	else
// 		strPutLocalFileName = strRealLocalFileName;

// 	if(_localInfo._strRelatedPathName.Find(_T("VersionPatch.exe")) != -1)
// 	{
// 		strRealLocalFileName.SetAt(strRealLocalFileName.GetLength() - 1, _T('t'));
// 	}

	UINT dwFileLength = 0;
	BOOL hasContentLength = FALSE;
	CString strLength;
	hasContentLength = pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, strLength);
	TCHAR* lengthEnd = NULL;
	ULONGLONG declaredLength = _tcstoui64((LPCTSTR)strLength, &lengthEnd, 10);
		BOOL validContentLength = !hasContentLength ||
			(lengthEnd != (LPCTSTR)strLength && *lengthEnd == 0 && declaredLength <= (UINT)-1);
		dwFileLength = validContentLength && hasContentLength ? (UINT)declaredLength : 0;
	//int iTotalPercent	= dwFileLength / 4096;

	//CStdioFile csf;
	//if( !csf.Open(strPutLocalFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary) )
	//{
	//	// _strLog += _T("Failed to Create Local File: ") + strPutLocalFileName;
	//	// _strLog += _T("\n");

	//	pFile->Close();

	//	return;
	//}
	//else
	//{
	//	CString strLength;
	//	pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, strLength);
	//	dwFileLength = (UINT)_ttoi((LPCTSTR)strLength);
	//	int iTotalPercent	= dwFileLength / 4096;

	//	// m_ProgressCurrent.SetRange(0, (short)iTotalPercent);
	//}

	std::vector<BYTE> downloaded;
	if (!validContentLength || !ClientPatch::ReadDownload(*pFile, downloaded, hasContentLength ? dwFileLength : (UINT)-1) ||
		(hasContentLength && downloaded.size() != dwFileLength))
	{
		pFile->Close();
		delete pFile;
		LogMsgToUI(_T("Invalid download length."));
		InterlockedIncrement(&g_downloadTaskCompletedCount);
		InterlockedIncrement(&g_writeFileTaskCompletedCount);
		return;
	}
	dwFileLength = (UINT)downloaded.size();
	BYTE* fileBuf = new BYTE[dwFileLength];
	if (dwFileLength > 0)
		memcpy(fileBuf, &downloaded[0], dwFileLength);
	std::vector<BYTE>().swap(downloaded);

	pFile->Close();

	InterlockedIncrement(&g_downloadTaskCompletedCount);

#if 1
	WriteToFileTask* task = new WriteToFileTask(_taskID, fileBuf, dwFileLength,
		strRealLocalFileName, bCompressed == TRUE, _localInfo._originLen);
	g_writeFileThreadPool->Run(task);
#else
	if(bCompressed)
	{
		CFile cf;
		//if( cf.Open(strPutLocalFileName, CFile::modeRead | CFile::typeBinary, 0) )
		{
			UINT iSrcLen = dwFileLength;//(UINT)cf.GetLength();
			BYTE* pbyteBuffer = fileBuf;//new BYTE[iSrcLen];
			//cf.Read(pbyteBuffer, iSrcLen);
			//cf.Close();

			uLong uncomprLen = _localInfo._originLen;
			Byte *uncompr = new Byte[uncomprLen];
			int err = uncompress(uncompr, &uncomprLen, pbyteBuffer, iSrcLen);
			if (err != Z_OK)
			{
				// _strLog += _T("Failed to Uncompress file: ") + strRealLocalFileName;
				// _strLog += _T("\n");

				delete[] fileBuf;//delete [] pbyteBuffer;
				delete[] uncompr;

				return;
			}
			else
			{
				CFile out;
				if( out.Open(strRealLocalFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary, 0) )
				{
					out.Write(uncompr, uncomprLen);
					out.Close();
				}
			}

			//delete [] pbyteBuffer;
			delete [] uncompr;
		}
	}
	else
	{
		CFile out;
		if( out.Open(strRealLocalFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary, 0) )
		{
			out.Write(fileBuf, dwFileLength);
			out.Close();
		}
	}

	delete[] fileBuf;
#endif

// 	if(_localInfo._iOperation == EPO_Install)		// Patch Install
// 	{
// 		_vectorPatchInstalls.push_back(strRealLocalFileName);
// 	}

	_httpConnection->Close();
	//delete _pHttpConnection;
	delete _internetSession;
}

void WriteToFileTask::Run()
{
#if M_LOG_TO_CONSOLE
	CString str;
	str.Format(_T("[%d] Write to file:%s"), _taskID, (LPCTSTR)_filePath);		
	LogMsgToUI(str);
#endif

	if(_compressed)
	{
		CFile cf;
		//if( cf.Open(strPutLocalFileName, CFile::modeRead | CFile::typeBinary, 0) )
		{
			UINT iSrcLen = _bufLen;//(UINT)cf.GetLength();
			BYTE* pbyteBuffer = _buffer;//new BYTE[iSrcLen];
			//cf.Read(pbyteBuffer, iSrcLen);
			//cf.Close();

			uLong uncomprLen = _uncompressLen;
			Byte *uncompr = new Byte[uncomprLen];
			int err = uncompress(uncompr, &uncomprLen, pbyteBuffer, iSrcLen);
			if (err != Z_OK)
			{
				// _strLog += _T("Failed to Uncompress file: ") + strRealLocalFileName;
				// _strLog += _T("\n");

				CString str;
				str.Format(_T("=== uncompress error:%s"), (LPCTSTR)_filePath);		
				LogMsgToUI(str);

				delete[] _buffer;//delete [] pbyteBuffer;
				delete[] uncompr;

				InterlockedIncrement(&g_writeFileTaskCompletedCount);
				return;
			}
			else
			{
				CFile out;
				if( out.Open(_filePath, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary, 0) )
				{
					out.Write(uncompr, uncomprLen);
					out.Close();
				}
			}

			//delete [] pbyteBuffer;
			delete [] uncompr;
		}
	}
	else
	{
		CFile out;
		if( out.Open(_filePath, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary, 0) )
		{
			out.Write(_buffer, _bufLen);
			out.Close();
		}
	}

	delete[] _buffer;

	InterlockedIncrement(&g_writeFileTaskCompletedCount);
}
