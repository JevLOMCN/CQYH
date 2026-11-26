
#include "stdafx.h"
#include "../PatchPath.h"
#include "Task.h"
#include <sstream>
#include "config.h"

using namespace std;


#ifdef _DEBUG
#	pragma comment(lib, "zlibwapid.lib")
#else
#	pragma comment(lib, "zlibwapi.lib")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection g_logLock;

// typedef std::vector<bool> TaskCompleteList;
// TaskCompleteList g_taskCompleteList;
// CCriticalSection g_taskCompleteListLock;
//const int g_iVersion = 2;
CString g_srcPath;
CString g_srcPathReplace;
CString g_targPath;
CString g_versionName;
CString g_downloadRootURL;
bool g_useVerMD5FileNameFormat = false;

VersionInfoes g_verInfoList;
VersionInfoes g_verInfoCopyFileList;

CThreadPool* g_threadPool;
//CCriticalSection g_verInfoListLock;

int g_threadCnt = 0;
unsigned __int64 g_maxThreadTaskWeight = 0;
unsigned __int64 g_totalFileSize = 0;
long g_completedTaskCnt = 0;
int g_totalTaskCnt = 0;

CStdioFile g_logFile;

void Log(const CString& str)
{
	CSingleLock lock(&g_logLock, TRUE);
	CString logStr = (str + _T("\n"));
#if defined(_DEBUG)
	::OutputDebugString((LPCTSTR)logStr);
#endif

	//g_logFile.WriteString(logStr);
	//g_logFile.Flush();
	std::wcout << (LPCTSTR) str << std::endl;
}

bool SortInfoList(const SVersionInfo& lhs, const SVersionInfo& rhs)
{
	return lhs._originLen > rhs._originLen;
}


class CViGeneratorApp : public CWinApp
{
public:
	BOOL InitInstance();

	BOOL ExitInstance(void);
} theApp;



BOOL CViGeneratorApp::InitInstance()
{
//	AfxOleInit();
	CWinApp::InitInstance();

	//std::wcout.imbue(std::locale("chs"));
	setlocale(LC_ALL, "");

	HANDLE hnd = CreateMutex(NULL, FALSE, _T("AOD_ViGen"));
	if(hnd)
	{
		DWORD dwR = ::WaitForSingleObject(hnd, 2000); // try 2 seconds
		if(dwR == WAIT_TIMEOUT || dwR == WAIT_ABANDONED)
		{
			CloseHandle( hnd );
			return FALSE;
		}
	}

	CString procDir;

	int argc = __argc;
	TCHAR strAppDir[MAX_PATH];
	if (!ClientPatch::ReadCurrentDirectory(strAppDir))
		return FALSE;

	if(argc == 2)
	{
		procDir = __targv[1];
	
		g_targPath = procDir + TEXT("\\..\\") + GetPathFolderName(procDir) + TEXT("_Patch\\");
	}
	else if(argc == 3)
	{
		TCHAR* arg1 = __targv[1];
		TCHAR* arg2 = __targv[2];
		procDir = arg1;
		g_targPath = arg2;
	}
	else if(argc == 5)
	{
		TCHAR* arg1 = __targv[1];
		TCHAR* arg2 = __targv[2];
		procDir = arg1;
		g_targPath = arg2;
		g_versionName = (TCHAR*) __targv[3];
		g_downloadRootURL = (TCHAR*) __targv[4];
		g_useVerMD5FileNameFormat = true;
	}
	else
	{
// 		CString str = _T("Usage:\n");
// 		str += __argv[0];
// 		str += _T("	DirectoryName");
// 		MessageBox(0, str, _T("Usage"), MB_OK | MB_ICONINFORMATION);
// 		return FALSE;
		procDir = strAppDir;

		g_targPath = procDir + TEXT("\\..\\") + GetPathFolderName(procDir) + TEXT("_Patch\\");
	}

	if(procDir.Right(1) == _T("\\"))
	{
		procDir = procDir.Left(procDir.GetLength() - 1);
	}

	if(!isDirExist(g_targPath))
	{
		mkDir(g_targPath, true);
		//_tmkdir((LPCTSTR)g_targPath);
	}

	g_logFile.Open(_T("vi.log"), CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareDenyNone);


	time_t startTime = ::time(NULL);
	
	TCHAR tmpbufs[128], tmpbufe[128];
	_tstrtime_s( tmpbufs, 128 );

	SYSTEM_INFO info;
	GetSystemInfo(&info);	

	g_threadCnt = info.dwNumberOfProcessors * 2;

	g_threadPool = new CThreadPool(g_threadCnt, true, 2);
// 	g_threadPool->SetPoolSize(g_threadCnt);
// 	g_threadPool->Create();

	//MFCThreadPool::getSingleton().Init(thrdCnt);
	//size_t maxMemSize = MFCThreadPool::getSingleton().GetMemoryBufferSize() / 1024 / 1024;
//	size_t maxMemSize = 0;

	CString logStr;
	logStr.Format(_T("=====App dir: %s"), strAppDir);
	Log(logStr);

	logStr.Format(_T("=====Processing dir: %s\n targDir:%s\n Threads: %d"), (LPCTSTR)procDir, (LPCTSTR)g_targPath, g_threadCnt);
	Log(logStr);

	std::wstringstream ss;
	ss <<
#if M_USE_BATCH_TASK
		"#Batch Task  " <<
#endif

#if M_USE_FILE_MAP
		"#File Map  " <<
#endif

#if M_USE_SORT
		"# Sort" <<
#endif
	std::endl;

	Log(ss.str().c_str());

	//std::cout << "=====Processing dir:" << procDir << " threads:" << g_threadCnt << " ..." << std::endl;

	g_srcPath = procDir + TEXT("\\");// + GetPathFolderName(procDir) + _T("\\");
	g_srcPathReplace = procDir + TEXT("\\");

	CString srcDir = g_srcPath.Left(g_srcPath.GetLength() - 1);

	// Reuse the validated working directory captured before creating worker threads.

	CString mkDirCmd = strAppDir;
	mkDirCmd += _T("\\__mkdir.bat");
	CString para = srcDir + _T(" ") + g_targPath;
	HINSTANCE hr = ::ShellExecute(0, _T("open"), (LPCTSTR)mkDirCmd, (LPCTSTR)para, CString(strAppDir) + _T("\\"), SW_NORMAL);
	UNREFERENCED_PARAMETER(hr);
	RecurseFindFiles(srcDir);

	std::wcout << _T("g_maxThreadTaskWeight:") << g_maxThreadTaskWeight << std::endl;

#if M_USE_SORT
	std::sort(g_verInfoList.begin(), g_verInfoList.end(), SortInfoList);
#endif

	CopyTask* copyTask = new CopyTask;
	//copyTask->_verInfoCopyFileList = g_verInfoCopyFileList;
	g_threadPool->Run(copyTask);


	g_totalTaskCnt = (int)g_verInfoList.size();

	std::cout << "g_totalTaskCnt:" << g_totalTaskCnt << std::endl;

#if M_USE_BATCH_TASK
	int idx = 0;
	int desiredWeight = (int)((float)g_maxThreadTaskWeight * 0.8f);
	CompressTaskBatch* batchTask = new CompressTaskBatch(idx++);
	for (size_t i = 0; i < g_verInfoList.size(); ++i)
	{
		if(batchTask->TotalWeight() < desiredWeight)
		{
			batchTask->AddTask(&g_verInfoList[i]);
		}
		else
		{
			g_threadPool->Run(batchTask);
			batchTask = new CompressTaskBatch(idx++);
			batchTask->AddTask(&g_verInfoList[i]);
		}
	}

	g_threadPool->Run(batchTask);

#else
	for (size_t i = 0; i < g_verInfoList.size(); ++i)
	{
		g_threadPool->Run(new CompressTask((int)i, &g_verInfoList[i]));
	}
#endif


	// wait for all task complete
	bool allComplete = false;
	while(!allComplete)
	{
		allComplete = g_threadPool->IsAllTaskCompleted();

		::Sleep(100);
	}

	g_threadPool->Destroy();

	//-----------------------------------------------------------
	// Version 1
	CStdioFile stdFile;
	CString strOutFile;
	strOutFile = g_targPath + /*_T("\\")  +*/ GetPathFolderName(g_srcPath) + _T(".vi");
#if 0

	if( stdFile.Open(strOutFile, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText) )
	{
		CString strTmp;

		int iMaxLen = 0;
		for(size_t t = 0; t < g_verInfoList.size(); ++t)
		{
			//g_verInfoList[t]._strRelatedPathName.Replace(g_outPath, _T(""));

			if(g_verInfoList[t]._relatedPathName.GetLength() > iMaxLen)
				iMaxLen = g_verInfoList[t]._relatedPathName.GetLength();
		}

		CString strFormat = _T("%s,%s,%u,%u\n");
		for(size_t t = 0; t < g_verInfoCopyFileList.size(); ++t)
		{
			strTmp.Format(strFormat
				, (LPCTSTR)g_verInfoCopyFileList[t]._relatedPathName
				, (LPCTSTR)g_verInfoCopyFileList[t]._contentType
				, g_verInfoCopyFileList[t]._timeModify
				, g_verInfoCopyFileList[t]._originLen
				);

			strTmp.Replace(_T("\\"), _T("/"));
			stdFile.WriteString(strTmp);
		}

		for(size_t t = 0; t < g_verInfoList.size(); ++t)
		{
			strTmp.Format(strFormat
				, (LPCTSTR)g_verInfoList[t]._relatedPathName
				, (LPCTSTR)g_verInfoList[t]._contentType
				, g_verInfoList[t]._timeModify
				, g_verInfoList[t]._originLen
				);

			strTmp.Replace(_T("\\"), _T("/"));
			stdFile.WriteString(strTmp);
		}



		stdFile.Close();
	}
	else
	{
		std::cerr << _T("Can't write file:") << strOutFile << std::endl;
		return FALSE;
	}
#endif

	//CStdioFile stdFile;
	CString strOutFile2 = strOutFile;
	if(g_useVerMD5FileNameFormat)
	{
		strOutFile2 += _T("md5");
	}
	else
	{
		strOutFile2 += _T("2");
	}

	if( stdFile.Open(strOutFile2, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText) )
	{
		CString strTmp;

		if(g_useVerMD5FileNameFormat)
		{
			strTmp.Format(_T("%s,%s\n"), g_downloadRootURL, g_versionName);
			stdFile.WriteString(strTmp);
		}

		int iMaxLen = 0;
		for(size_t t = 0; t < g_verInfoList.size(); ++t)
		{
			//g_verInfoList[t]._strRelatedPathName.Replace(g_outPath, _T(""));

			if(g_verInfoList[t]._relatedPathName.GetLength() > iMaxLen)
				iMaxLen = g_verInfoList[t]._relatedPathName.GetLength();
		}

		CString strFormat = _T("%s,%s,%s,%u\n");

		for(size_t t = 0; t < g_verInfoCopyFileList.size(); ++t)
		{
			strTmp.Format(strFormat
				, (LPCTSTR)g_verInfoCopyFileList[t]._relatedPathName
				, (LPCTSTR)g_verInfoCopyFileList[t]._contentType
				, (LPCTSTR)g_verInfoCopyFileList[t]._codeMD5
				, g_verInfoCopyFileList[t]._originLen
				);

			strTmp.Replace(_T("\\"), _T("/"));
			stdFile.WriteString(strTmp);
		}

		for(size_t t = 0; t < g_verInfoList.size(); ++t)
		{
			strTmp.Format(strFormat
				, (LPCTSTR)g_verInfoList[t]._relatedPathName
				, (LPCTSTR)g_verInfoList[t]._contentType
				, (LPCTSTR)g_verInfoList[t]._codeMD5
				, g_verInfoList[t]._originLen
				);

			strTmp.Replace(_T("\\"), _T("/"));
			stdFile.WriteString(strTmp);
		}
		stdFile.Close();
	}
	else
	{
		std::cerr << _T("Can't write file:") << strOutFile2 << std::endl;
		//MessageBox(0, strOutFile2, _T("File is read only£¡"), MB_OK | MB_ICONHAND);
		return FALSE;
	}

	time_t endTime = ::time(NULL);
	_tstrtime_s( tmpbufe, 128 );

	std::wcout << _T("Task completed:") << g_completedTaskCnt << _T("/") << g_totalTaskCnt << std::endl;
	logStr.Format(_T("========Start: %s End:%s Total:%d Seconds"), tmpbufs, tmpbufe, endTime - startTime);
	Log(_T("========================================================"));
	Log(logStr);
	Log(_T("========================================================"));
	
	g_logFile.Close();

	while(g_threadPool->GetState() != Destroyed)
	{
		::Sleep(2);
	}

	delete g_threadPool;

	return FALSE;
}



BOOL CViGeneratorApp::ExitInstance(void)
{

	return CWinApp::ExitInstance();
}
