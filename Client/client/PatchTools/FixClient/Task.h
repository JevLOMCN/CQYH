#ifndef __TASK_H__
#define __TASK_H__

#include "RunObject.h"
#include "../ViGenerator/PatchHelper.h"

extern CCriticalSection g_logLock;

// typedef std::vector<bool> TaskCompleteList;
// TaskCompleteList g_taskCompleteList;
// CCriticalSection g_taskCompleteListLock;
//extern const int g_iVersion = 2;
extern CString g_srcPath;
extern CString g_srcPathReplace;
extern CString g_targPath;

// extern VersionInfoes g_verInfoList;
// extern VersionInfoes g_verInfoCopyFileList;

extern CThreadPool* g_genMD5ThreadPool;
extern CThreadPool* g_compareMD5ThreadPool;
extern CThreadPool* g_downloadThreadPool;
extern CThreadPool* g_writeFileThreadPool;
//CCriticalSection g_verInfoListLock;

//extern int g_threadCnt;
extern unsigned __int64 g_maxThreadTaskWeight;
extern unsigned __int64 g_totalFileSize;

extern long g_completedTaskCnt;
extern int g_totalTaskCnt;


extern volatile LONG g_genMD5TaskTotalCount;
extern volatile LONG g_genMD5TaskCompletedCount;

extern volatile LONG g_compareMD5TaskTotalCount;
extern volatile LONG g_compareMD5TaskCompletedCount;

extern volatile LONG g_downloadTaskTotalCount;
extern volatile LONG g_downloadTaskCompletedCount;
extern CString g_downloadTaskDetail;

extern volatile LONG g_writeFileTaskCompletedCount;

// add by zhangrui
// >>
typedef int(*pfGetPackFileCount)(LPCWSTR lpszSrcFilePath);
typedef bool(*pfGetPackFileList)(LPCWSTR lpszArchivePath, char pFilePath[][MAX_PATH], unsigned int nMaxCount, unsigned int nCount);
typedef bool(*pfGetPackFileBufferSized)(LPCWSTR lpszArchivePath, LPCWSTR lpszFilePath, unsigned char* pBuffer, DWORD bufferSize);
typedef DWORD(*pfGetPackFileSize)(LPCWSTR lpszArchivePath, LPCWSTR lpszFilePath);
typedef bool(*pfDeletePackFile)(LPCWSTR lpszArchivePath, LPCWSTR lpszFilePath);
typedef bool(*pfExtractPackFile)(LPCWSTR lpszSrcPath, LPCWSTR lpszTempPath, LPCWSTR lpszFilePath, int& iPackIndex);
typedef bool(*pfAddPackFile)(LPCWSTR lpszArchivePath, LPCWSTR lpszFilePath, bool bCreateArchive, DWORD dwNewFileCount);

extern HMODULE g_hXDelta3;
extern pfGetPackFileCount g_pfGetPackFileCount;
extern pfGetPackFileList g_pfGetPackFileList;
extern pfGetPackFileBufferSized g_pfGetPackFileBufferSized;
extern pfGetPackFileSize g_pfGetPackFileSize;
extern pfDeletePackFile g_pfDeletePackFile;
extern pfExtractPackFile g_pfExtractPackFile;
extern pfAddPackFile g_pfAddPackFile;

extern CRITICAL_SECTION g_csGenMD5;
// <<

void CompleteCompress(class CompressTask* task);
CString GetPathFolderName(const CString& srcPath);
bool isDirExist(const CString& dir);
bool mkDir(CString filePath, bool bRecurse);
void RecurseFindFiles(LPCTSTR pstr/*, VersionInfoes& VIs*/);
bool ShouldSkipFile(const CString& strExt);

class AbstractTask : public IRunObject
{
protected:
	int _taskID;

public:
	AbstractTask(int taskID)
		: _taskID(taskID)
	{

	}

	virtual ~AbstractTask(){}
};

class GenerateMD5Task : public AbstractTask
{
public:

	CString _srcFile;
	CString _srcSubFile;
	CString& _targetMD5;
	BOOL _isMPQ;

	GenerateMD5Task(int taskID, const CString& srcFile, const CString& srcSubFile, BOOL isMPQ, CString& targetMD5)
		:  AbstractTask(taskID)
		, _srcFile(srcFile)
		, _srcSubFile(srcSubFile)
		, _targetMD5(targetMD5)
		, _isMPQ(isMPQ)
	{
	}


	GenerateMD5Task::~GenerateMD5Task()
	{
	}

	virtual bool AutoDelete()
	{
		return true;
	}

	virtual void Run();
};


class CompareMD5Task : public AbstractTask
{
public:

	SLocalVersionInfo& _infoLocal;
	SFilePatch& _filePatch;

	CompareMD5Task(int taskID, SLocalVersionInfo& localInfo, SFilePatch& filePatch)
		: AbstractTask(taskID)
		, _infoLocal(localInfo)
		, _filePatch(filePatch)
	{
	}

	virtual bool AutoDelete()
	{
		return true;
	}

	virtual void Run();
};

class DownloadTask : public AbstractTask
{
public:

	class CInternetSession* _internetSession;
	CCriticalSection _sessionLock;
	class CHttpConnection* _httpConnection;

	CString			_strServerIP;
	INTERNET_PORT	_serverPort;
	CString			_strInitRoot;


	SLocalVersionInfo _localInfo;
	CString			_strInstallDir;
	CString			_strTmpDir;

	DownloadTask(int taskID, CInternetSession* session, const CString& serverIP, INTERNET_PORT serverPort,
		const CString& initRoot, const SLocalVersionInfo& localInfo,
		const CString& strInstallDir, const CString& strTmpDir)
		:  AbstractTask(taskID)
		, _internetSession(session)
		, _httpConnection(0)
		, _strServerIP(serverIP)
		, _serverPort(serverPort)
		, _localInfo(localInfo)
		, _strInitRoot(initRoot)
		, _strInstallDir(strInstallDir)
		, _strTmpDir(strTmpDir)
	{
	}

	virtual bool AutoDelete()
	{
		return true;
	}

	virtual void Run();
};


class WriteToFileTask : public AbstractTask
{
	BYTE* _buffer;
	UINT _bufLen;
	CString _filePath;
	bool _compressed;
	UINT _uncompressLen;
public:

	WriteToFileTask(int taskID, BYTE* buffer, UINT bufLen, const CString& filePath, bool compressed, uLong uncompressLen)
		:  AbstractTask(taskID)
		, _buffer(buffer)
		, _bufLen(bufLen)
		, _filePath(filePath)
		, _compressed(compressed)
		, _uncompressLen(uncompressLen)
	{
	}

	virtual bool AutoDelete()
	{
		return true;
	}

	virtual void Run();
};

#endif