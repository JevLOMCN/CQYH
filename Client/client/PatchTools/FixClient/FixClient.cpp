/*====================================================================
	Created Date  :		2005-8-25
	
	Specification :  
====================================================================*/
#include "stdafx.h"
#include "FixClient.h"
#include "FixClientDlg.h"
#include "CheckServerDlg.h"
#include "Controls.h"
#include "Task.h"
#include "ThreadPool.h"
#include <iostream>

#include <vector>
using namespace std;

//int g_threadCnt = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFixClientApp theApp;

BEGIN_MESSAGE_MAP(CFixClientApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CFixClientApp::CFixClientApp()
{
}

BOOL CFixClientApp::InitInstance()
{
//	_tsetlocale(LC_ALL, _T("zh-CN"));
 	_tsetlocale(LC_ALL, NULL);
// 	std::wcout.imbue(locale(""));

	HANDLE hnd = CreateMutex(NULL, FALSE, _T("AOD_Patch"));
	if(hnd)
	{
		DWORD dwR = ::WaitForSingleObject(hnd, 2000); // try 2 seconds
		if(dwR == WAIT_TIMEOUT || dwR == WAIT_ABANDONED)
		{
			CloseHandle( hnd );
			return FALSE;
		}
	}
	
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	

//	UINT winVer = 0;
	OSVERSIONINFO osinfo;
	GetVersionEx(&osinfo);

	if(osinfo.dwMajorVersion > 4)
	{
		HMODULE hMSIModule = 0;
		hMSIModule = LoadLibrary(_T("Msimg32.dll")); 
		if(hMSIModule) 
		{ 
			g_pTransparentBlt = (LPTRANSPARENTBLT) GetProcAddress(hMSIModule, "TransparentBlt"); 
		} 
	}

	srand( (unsigned)time( NULL ) );

	SYSTEM_INFO info;
	GetSystemInfo(&info);	

	//g_threadCnt = info.dwNumberOfProcessors * 2;

	g_genMD5ThreadPool = new CThreadPool(max(1, info.dwNumberOfProcessors * 2 - 2), true, 50);
	g_compareMD5ThreadPool = new CThreadPool(max(1, info.dwNumberOfProcessors * 4), true, 50);
	g_downloadThreadPool = new CThreadPool(max(1, 0 * info.dwNumberOfProcessors * 2), true, 50);
	g_writeFileThreadPool = new CThreadPool(max(1, 0 * info.dwNumberOfProcessors * 2), true, 100);

	// add by zhangrui
	// >>
	g_hXDelta3 = ::LoadLibraryEx(L"LauncherModules\\XDelta3WrapFactory.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (g_hXDelta3)
	{
		g_pfGetPackFileCount = (pfGetPackFileCount)(::GetProcAddress(g_hXDelta3, "GetPackFileCount"));
		g_pfGetPackFileList = (pfGetPackFileList)(::GetProcAddress(g_hXDelta3, "GetPackFileList"));
		g_pfGetPackFileBufferSized = (pfGetPackFileBufferSized)(::GetProcAddress(g_hXDelta3, "GetPackFileBufferSized"));
		g_pfGetPackFileSize = (pfGetPackFileSize)(::GetProcAddress(g_hXDelta3, "GetPackFileSize"));
		if (!g_pfGetPackFileBufferSized || !g_pfGetPackFileSize)
		{
			AfxMessageBox(_T("The patch library must be updated before checking archive files."), MB_OK | MB_ICONHAND);
			FreeLibrary(g_hXDelta3);
			g_hXDelta3 = NULL;
			return FALSE;
		}
		g_pfDeletePackFile = (pfDeletePackFile)(::GetProcAddress(g_hXDelta3, "DeletePackFile"));
		g_pfExtractPackFile = (pfExtractPackFile)(::GetProcAddress(g_hXDelta3, "ExtractPackFile"));
		g_pfAddPackFile = (pfAddPackFile)(::GetProcAddress(g_hXDelta3, "AddPackFile"));
	}
	else
	{
		AfxMessageBox(_T("无法找到LauncherModules/XDelta3WrapFactory.dll！"), MB_OK | MB_ICONHAND);
		return FALSE;
	}
	// <<

	CFixClientDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	if(dlg.PatchSuccess() == TRUE)
	{
	}
	else
	{
		AfxMessageBox( _T("修复过程被中断，有可能不能正常进入游戏，请抽空重新运行全文件修复！"), MB_OK | MB_ICONHAND );
		return FALSE;
	}

	return FALSE;
}

void DestroyThreadPool(CThreadPool* pool)
{
	pool->Destroy();
	while(pool->GetState() != Destroyed)
	{
		::Sleep(2);
	}

	delete pool;
}


int CFixClientApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	// add zhangrui 
	// >>
	FreeLibrary(g_hXDelta3);
	// <<

	DestroyThreadPool(g_genMD5ThreadPool);
	DestroyThreadPool(g_compareMD5ThreadPool);
	DestroyThreadPool(g_downloadThreadPool);
	DestroyThreadPool(g_writeFileThreadPool);

	return CWinApp::ExitInstance();
}
