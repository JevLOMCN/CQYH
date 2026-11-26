#include "stdafx.h"
#include "../ReadFileBuffer.h"
#include "Task.h"
#include "config.h"
#include "File.h"


void CopyTask::Run()
{
	time_t startTime = time(0);

	{
		CSingleLock lock(&g_logLock, TRUE);
		std::wcout << _T("Starting copy tasks, count:") << g_verInfoCopyFileList.size() << std::endl;
	}

	for (size_t i = 0; i < g_verInfoCopyFileList.size(); ++i)
	{
		SVersionInfo* _vi = &g_verInfoCopyFileList[i];
		//_vi->_uOriginLen = 0;
		CString targFile = g_targPath + /*_T("\\") +*/ _vi->_relatedPathName;
		int idx = targFile.ReverseFind('\\');
		CString targPath = targFile.Left(idx);
		// 			if(!isDirExist(targPath))
		// 			{
		// 				mkDir(targPath, true);
		// 			}

		if(_vi->_contentType == _T("File"))
		{
			FILE* pFile = 0;
			_tfopen_s(&pFile, (LPCTSTR)_vi->_fullPathName, _T("rb"));
			ASSERT(pFile != NULL);

			MD5 md5(pFile);

			char* md5Buf = md5.hex_digest();
			_vi->_codeMD5 = CString(md5Buf);
			delete[] md5Buf;

			if(g_useVerMD5FileNameFormat)
			{
				targFile = File::GetVerMD5FileName(targFile, _vi->_codeMD5, g_versionName);
			}

			BOOL copyResult = ::CopyFile(_vi->_fullPathName, targFile, FALSE);
#ifdef _DEBUG
			{
				CSingleLock lock(&g_logLock, TRUE);
				std::wcout << _T("Copy result:") << copyResult << std::endl;
			}
#else
			UNREFERENCED_PARAMETER(copyResult);
#endif
		}
	}

	time_t endTime = time(0);

	{
		CSingleLock lock(&g_logLock, TRUE);
		std::wcout << _T("Copy tasks completed:") << endTime - startTime << _T("s") << std::endl;
	}
}

bool CopyTask::AutoDelete()
{
	return true;
}

void AbstractCompressTask::Compress( SVersionInfo* vi )
{
	BOOL bCompressSuccess = FALSE;

	CFile cf;			
	if( cf.Open(vi->_fullPathName, CFile::modeRead | CFile::typeBinary, 0) )
	{
		const unsigned long capacity = compressBound(vi->_originLen);
		if (cf.GetLength() != vi->_originLen || capacity < vi->_originLen || capacity == 0)
		{
			cf.Close();
			vi->_originLen = 0;
			return;
		}
		std::vector<BYTE> compressed(capacity);
		BYTE empty = 0;
#if M_USE_FILE_MAP
		HANDLE hmap = NULL;
		BYTE* pbyteBuffer = &empty;
		if (vi->_originLen > 0)
		{
			hmap = ::CreateFileMapping((HANDLE)cf, NULL, PAGE_READONLY, 0, 0, NULL);
			if (!hmap)
			{
				cf.Close();
				vi->_originLen = 0;
				return;
			}
			pbyteBuffer = (BYTE*)::MapViewOfFile(hmap, FILE_MAP_READ, 0, 0, vi->_originLen);
			if (!pbyteBuffer)
			{
				::CloseHandle(hmap);
				cf.Close();
				vi->_originLen = 0;
				return;
			}
		}
#else
		std::vector<BYTE> bytes;
		if (!ClientPatch::ReadFileBuffer(cf, bytes, vi->_originLen) || bytes.size() != vi->_originLen)
		{
			cf.Close();
			vi->_originLen = 0;
			return;
		}
		cf.Close();
		BYTE* pbyteBuffer = bytes.empty() ? &empty : &bytes[0];
#endif

		MD5 md5;
		md5.update(pbyteBuffer, vi->_originLen);
		md5.finalize();
		char* md5Buf = md5.hex_digest();				
		vi->_codeMD5 = CString(md5Buf);
		delete[] md5Buf;

		unsigned long comprLen = capacity;
		BYTE* compr = &compressed[0];

		int err = compress(compr, &comprLen, (const BYTE*)pbyteBuffer, vi->_originLen);
		if(err != Z_OK)
		{
			CSingleLock lock(&g_logLock, TRUE);
			std::wcout << _T("------- Compress error: ") << err << std::endl;
		}
		else
		{
#if defined(_DEBUG)
			{
				CSingleLock lock(&g_logLock, TRUE);
				int ratio = vi->_originLen ? (int)((FLOAT)comprLen / (FLOAT)vi->_originLen * 100) : 0;
				std::wcout << _idx << _T(" ---- Compressed:") << (LPCTSTR)vi->_relatedPathName
					<< _T(" ") << ratio << _T("%") << std::endl;
			}
#endif

			CString targFile = g_targPath + /*_T("\\") +*/ vi->_relatedPathName;
			int idx = targFile.ReverseFind('\\');
			CString targPath = targFile.Left(idx);
			//if(!isDirExist(targPath))
			{
				//mkDir(targPath, true);
			}

			CString stOutName = targFile + _T(".zfp");

#ifdef _DEBUG
			std::wcout << _T("_______ out file:") << (LPCTSTR)stOutName << std::endl;
#endif
			CFile out;

			if(g_useVerMD5FileNameFormat)
			{
				stOutName = File::GetVerMD5FileName(stOutName, vi->_codeMD5, g_versionName);
			}

			if( out.Open(stOutName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary, 0) )
			{
				out.Write(compr, comprLen);
				out.Close();

				bCompressSuccess = TRUE;
			}
		}

#if M_USE_FILE_MAP
		if (hmap)
		{
			::UnmapViewOfFile(pbyteBuffer);
			::CloseHandle(hmap);
		}
		cf.Close();
#endif

		if(!bCompressSuccess)
		{
			vi->_originLen = 0;
		}
	}
}

void CompressTask::Run()
{
#if defined(_DEBUG)
	{
		CSingleLock lock(&g_logLock, TRUE);
		std::wcout << _idx << _T(" ---- Processing:") << (LPCTSTR)_vi->_relatedPathName
			<< _T(" ") << _vi->_originLen / 1024 / 1024 << _T("MB") << std::endl;
	}
#endif

	Compress(_vi);

	InterlockedIncrement(&g_completedTaskCnt);
}

void CompressTaskBatch::Run()
{
	{
		CSingleLock lock(&g_logLock, TRUE);
		std::wcout << _idx << _T(" Starting batch task, count:") << _tasks.size() << _T(" total weight:") << _totalWeight << std::endl;
	}

	for (size_t i = 0; i < _tasks.size(); ++i)
	{
		SVersionInfo* vi = _tasks[i];
		Compress(vi);
	}
	InterlockedExchangeAdd(&g_completedTaskCnt, (long)_tasks.size());
	{
		CSingleLock lock(&g_logLock, TRUE);
		std::wcout << _idx << _T(" BatchTask Completed!") << _tasks.size() << _T(" g_completedTaskCnt:") << g_completedTaskCnt << std::endl;
	}
}





void RecurseFindFiles(LPCTSTR pstr/*, VersionInfoes& VIs*/)
{
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(pstr);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;

		SVersionInfo vi;
		CString str = finder.GetFilePath();
		vi._fullPathName = str;
		CString strExt = str.Right(str.GetLength() - str.ReverseFind(_T('.')));

		// if it's a directory, recursively search it
		if(finder.IsDirectory())
		{
			vi._relatedPathName	= str;
			vi._relatedPathName.Replace(g_srcPathReplace, _T(""));
			vi._contentType				= _T("Dir");
			vi._timeModify			= 0;
			vi._originLen			= 0;
			vi._codeMD5				= _T("None");
			g_verInfoCopyFileList.push_back(vi);

			RecurseFindFiles(str);
		}

		// normal file
		if(!finder.IsDirectory()
			&&
			!ShouldSkipFile(strExt)
			)
		{
			vi._relatedPathName = str;
			vi._relatedPathName.Replace(g_srcPathReplace, _T(""));
			vi._contentType = _T("File");
			CTime fileTime;
			finder.GetLastWriteTime(fileTime);
			vi._timeModify = fileTime.GetTime();


			vi._originLen = static_cast<unsigned int>(finder.GetLength());
			const int minCompressSize = 10 * 1024;
			if(vi._originLen <= minCompressSize)
			{
				vi._originLen = 0;
				g_verInfoCopyFileList.push_back(vi);
			}
			else
			{
				vi._costWeight = vi._originLen;
				g_totalFileSize += vi._originLen;
				g_maxThreadTaskWeight += vi._costWeight / g_threadCnt;
				g_verInfoList.push_back(vi);
			}
		}
	}

	finder.Close();
}


bool ShouldSkipFile(const CString& strExt)
{
	return
		(
		strExt.CompareNoCase(_T(".ilk")) == 0
		||
		strExt.CompareNoCase(_T(".zfp")) == 0
		||
		strExt.CompareNoCase(_T(".vi")) == 0
		||
		strExt.CompareNoCase(_T(".vi2")) == 0
		||
		strExt.CompareNoCase(_T(".exp")) == 0		
		);

}

CString GetPathFolderName(const CString& srcPath)
{
	CString targDir;
	if(srcPath.Right(1) == _T("\\"))
	{
		targDir = srcPath.Left(srcPath.GetLength() - 1);
	}
	else
	{
		targDir = srcPath;
	}
	int idx = targDir.ReverseFind(_T('\\'));
	CString targName = targDir.Mid(idx + 1, targDir.GetLength() - idx);
	return targName;
}


bool isDirExistCRT(const CString& dirPath)
{
	return _taccess((LPCTSTR)dirPath, 0) != -1;
}

bool isDirExist(const CString& dir)
{
	DWORD attr;
	attr = GetFileAttributes(dir);

	return ((attr != (DWORD)-1) && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

bool mkDir(CString filePath, bool bRecurse)
{
	//统一路径分隔符
	filePath.Replace('/', '\\');

	int len = filePath.GetLength();
	if(len < 2)
		return false;

	//去掉最后的斜杠
	while(len != 0 && filePath[len - 1] == '\\')
	{
		filePath = filePath.Left(len - 1);
		len = filePath.GetLength();
	}

	if(len <= 0 )    return false;

	//已经存在直接返回成功
	if(isDirExist(filePath))
	{
		return true;
	}

	//类似G:这样的目录不允许创建
	if(len == 2)    return false;

	//不递归创建，直接返回创建结果
	if(!bRecurse)
	{
		return CreateDirectory(filePath, NULL) == TRUE;
	}

	//递归创建目录
	CString topDir ;
	int index = filePath.Find('\\', 0);

	while(-1 != index)
	{
		topDir = filePath.Left(index);
		index = filePath.Find('\\', index + 1);

		//存在
		if(isDirExist(topDir))
		{
			continue;
		}

		//不存在
		CreateDirectory(topDir, NULL);  
	}

	if(!isDirExist(filePath))
		return CreateDirectory(filePath, NULL) == TRUE; 

	return 0;
}


//--------------------------------------------------------------------------------------
//
//bool File::makeDir(const CString& dirNameSrc)
//{
//	String dirName = StringUtil::standardisePath(dirNameSrc);
//	TCHAR strDir[MAX_PATH];
//	TCHAR strPath[MAX_PATH];
//	TCHAR strName[MAX_PATH];
//	TCHAR strExt[MAX_PATH];
//	_tsplitpath((LPCTSTR)dirName, strDir, strPath, strName, strExt);
//
//	StringVector dirList = StringUtil::split(strPath, ("/"));
//
//	StringVector::iterator it = dirList.begin();
//	while (it != dirList.end())
//	{
//		String str = *it;
//		if(str.empty())
//		{
//			it = dirList.erase(it);
//		}
//		else
//		{
//			++it;
//		}
//	}
//
//	size_t cnt = dirList.size();
//
//	for (size_t i = 0; i < cnt; ++i)
//	{
//		String tmpDir;
//		String dirPath(strDir);
//		if(!dirPath.empty())
//		{
//			tmpDir = dirPath + ("/");
//		}
//
//		for(size_t j = 0; j <= i; ++j)
//		{
//			tmpDir += dirList[j] + ("/");
//		}
//
//		if(!isDirExist(tmpDir.c_str()))
//		{
//			//tmpDir = ("\"") + tmpDir + ("\"");
//			_tmkdir(tmpDir.c_str());
//		}
//	}
//
//	return isDirExist(dirName);
//}
