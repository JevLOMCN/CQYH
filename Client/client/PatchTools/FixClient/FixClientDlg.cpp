/*====================================================================
	Created Date  :		2005-8-24
	
	Specification :  
====================================================================*/
#include "stdafx.h"
#include "../ReadFileBuffer.h"
#include "../PatchPath.h"
#include "FixClient.h"
#include "FixClientdlg.h"
#include "Controls.h"
#include "MD5.h"
#include "App.h"
#include "Configer.h"
#include "File.h"
#include "Task.h"
#include "ThreadPool.h"
#include <assert.h>
#include <sstream>
#include <algorithm>
#include "FixClientUtil.h"
#include <DbgHelp.h>

#ifdef _DEBUG
#	pragma comment(lib, "zlibwapid.lib")
#else
#	pragma comment(lib, "zlibwapi.lib")
#endif
#pragma comment(lib, "dbghelp")
#define M_USE_MT_UPDATE 1

using namespace std;
using namespace ShowGame_Core;

#define M_BEGIN_PATCH	WM_USER + 1
#define M_BEGIN_PATCH_GAME	WM_USER + 2
#define M_BEGIN_PATCH_EDITOR	WM_USER + 3
#define M_BEGIN_PATCH_QAGAME	WM_USER + 4
#define M_BEGIN_PATCH_QAEDITOR	WM_USER + 5

const UINT C_TimerID = 88888;

#define TMP_FILE_LIST_FILE _T("TmpFixClientList.vi2");

// 方便DEBUG时看清UI进度条更新
#ifdef _DEBUG
#	define M_Debug_Delay ::Sleep(1000);
#else
#	define M_Debug_Delay 
#endif

typedef std::vector<CString> CStringVector;

CStringVector _clientMPQFiles;
CStringVector _clientRawFiles;

SFilePatch _patchList;

CString g_versionName;
CString g_downloadURLRoot;
DWORD g_downloadServiceType = 0;
INTERNET_PORT g_downloadServerPort = 0;
CString g_downloadServerIP;
CString g_downloadObjectURI;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


DWORD FindAppProcessID(LPCTSTR strPathName)
{
	HANDLE handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPALL,0);
	PROCESSENTRY32 Info;
	Info.dwSize = sizeof(PROCESSENTRY32);
	if(::Process32First(handle, &Info))
	{
		do{
			CString ss = Info.szExeFile;
			if(!ss.CompareNoCase(strPathName))
			{
				::CloseHandle(handle);
				return Info.th32ProcessID;
			}
		}

		while(::Process32Next(handle,&Info));

		::CloseHandle(handle);
	}

	return (DWORD)-1;
}

struct SServer
{
	CString			_fullServerURL;
	CString			_strServer;
	CString			_strServerInitPath;
	INTERNET_PORT	_dwPort;
};


//-----------------------------------------------------------------------------
#define M_Update_Msg(b)		{\
	if(b)\
{\
	UpdateData(FALSE);\
	_infoCurrent.Invalidate(FALSE);\
	_infoTotal.Invalidate(FALSE);\
	_infoDetail.Invalidate(FALSE);\
}\
	MSG msg;\
	while(::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))\
	{\
		TranslateMessage(&msg);\
		DispatchMessage(&msg);\
	}\
}



void CFixClientDlg::WaitForAllWorks( class CThreadPool* thrdPool, const CString& msg /*= _T("工作线程数量:%d")*/ )
{
	bool allComplete = false;
	while(!allComplete)
	{
		allComplete = thrdPool->IsAllTaskCompleted();

		_statusDetail.Format((LPCTSTR)msg, thrdPool->GetWorkingThreadCount());
		M_Update_Msg(true);

		::Sleep(100);
	}
}

CFixClientDlg::CFixClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFixClientDlg::IDD, pParent)
	, _statusDetail(_T("N/A"))
    , _statusTotal(_T("N/A"))
    , _statusCurrent(_T("N/A"))
	, _isPatchSuccess(TRUE)
	, _countDownTime(0)
	, _serverPort(INTERNET_INVALID_PORT_NUMBER)
	, _httpConnectionPtr(0)
	, _internetSession(0)
	, _curUpdate(0)
	, _totalUpdate(0)
	, _progressCount(0)
{
	_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFixClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATUS_DETAIL, _statusDetail);
	DDX_Control(pDX, IDC_PROGRESS, _progressTotal);
	DDX_Control(pDX, IDC_PROGRESS_FILE, _progressCurrent);
	DDX_Control(pDX, IDC_MIN, _btnMin);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, _btnClose);
	DDX_Text(pDX, IDC_TOTAL, _statusTotal);
	DDX_Text(pDX, IDC_CURRENT, _statusCurrent);
	DDX_Control(pDX, IDC_CURRENT, _infoCurrent);
	DDX_Control(pDX, IDC_TOTAL, _infoTotal);
	DDX_Control(pDX, IDC_STATUS_DETAIL, _infoDetail);

 	DDX_Control(pDX, IDBT_UPDATE_GAME, _btUpdateGame);
}

BEGIN_MESSAGE_MAP(CFixClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnCancel)
	ON_BN_CLICKED(IDC_MIN, OnBnClickedMin)
	ON_MESSAGE(M_BEGIN_PATCH, OnBeginPatch)
	//ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()

	ON_BN_CLICKED(IDBT_UPDATE_GAME, &CFixClientDlg::OnBnClickedUpdateGame)
	ON_WM_TIMER()
	ON_WM_DESTROY()
 END_MESSAGE_MAP()

CRect GetControlRect(const CString& name)
{
	static CString iniFile = App::GetAppPath() + C_IniFile;
	
	static Configer cfg;
	static bool init = false;
	if(!init)
	{
		cfg.LoadFile(iniFile);
		init = true;
	}
	
	// 	CurrentProg=l,t,w,h
	
	CString str = cfg.GetStringValue(_T("UI"), name, _T("0,0,50,50"));
	CRect rc(0, 0, 50, 50);
	TCHAR comma1 = 0, comma2 = 0, comma3 = 0;
	std::basic_istringstream<TCHAR> input((LPCTSTR)str);
	if (!(input >> rc.left >> comma1 >> rc.top >> comma2 >> rc.right >> comma3 >> rc.bottom) ||
		comma1 != _T(',') || comma2 != _T(',') || comma3 != _T(','))
	{
		return CRect(0, 0, 50, 50);
	}

	const LONGLONG right = (LONGLONG)rc.left + rc.right;
	const LONGLONG bottom = (LONGLONG)rc.top + rc.bottom;
	if (right < -MAXLONG - 1 || right > MAXLONG || bottom < -MAXLONG - 1 || bottom > MAXLONG)
	{
		return CRect(0, 0, 50, 50);
	}
	rc.right = (LONG)right;
	rc.bottom = (LONG)bottom;

	return rc;
}

void MoveControlToRect(CWnd& wnd, const CString& name, BOOL repaint = FALSE)
{
	//CRect rc = GetControlRect(name);
	//wnd.MoveWindow(rc.left, rc.top, rc.right, rc.bottom, repaint);
}

BOOL CFixClientDlg::OnInitDialog(){
	CDialog::OnInitDialog();

	//std::wcout.imbue(std::locale("chs"));
	setlocale(LC_ALL, "");

	//ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	static CString iniFile = App::GetAppPath() + C_IniFile;
	
	static Configer cfg;
	cfg.LoadFile(iniFile);
	//_autoStartTime = cfg.GetIntValue(_T("Time"), _T("AutoStart"), 20);	

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(_hIcon, TRUE);			// 设置大图标
	SetIcon(_hIcon, FALSE);		// 设置小图标

	SetWindowText(_T("《传奇永恒》全文件修复"));

	_btnMin.Set4In1Bitmap(IDB_MIN);
	_btnMin.SetDrawTrans(TRUE);

	_btnClose.Set4In1Bitmap(IDC_BUTTON_CLOSE);
	_btnClose.SetDrawTrans(TRUE);

	_btUpdateGame.Set4In1Bitmap(IDB_FIX);
	_btUpdateGame.SetDrawTrans(TRUE);

	//_btStart.Set4In1Bitmap(IDB_START);
	//_btStart.SetDrawTrans(TRUE);

	//_btCancel.Set4In1Bitmap(IDB_CANCEL);
	//_btCancel.SetDrawTrans(TRUE);

	_bmpBk.LoadBitmap(IDB_BK);
	CRgn rgn;
	CClientDC dc(this);
	SIZE sz = CU_GetBitmapSize(_bmpBk);
	MoveWindow(0, 0, sz.cx, sz.cy, FALSE);

	// 通过配置文件读取布局
	MoveControlToRect(_progressCurrent, _T("CurrentProg"));
	MoveControlToRect(_progressTotal, _T("AllProg"));
	//MoveControlToRect(_htmlADBar, _T("HtmlAD"));

	TCHAR strBuffer[MAX_PATH];
	if (!ClientPatch::ReadModulePath(strBuffer))
		return FALSE;
	CString strTmp = strBuffer;
	_strPatchConfigFile = strTmp.Left( strTmp.ReverseFind('\\') + 1 );
	_strVersionLocalConfigFile = _strPatchConfigFile;
	_strPatchConfigFile += C_IniFile;
	//_strVersionLocalConfigFile += C_VerLocalIniFile;

// 	MoveControlToRect(_btStart, _T("BtStart"));
// 	MoveControlToRect(_btCancel, _T("BtCancel"));

	MoveControlToRect(_btnMin, _T("BtClose"));
	MoveControlToRect(_btnClose, _T("BtMin"));
	MoveControlToRect(_btUpdateGame, _T("BtMin"));

	MoveControlToRect(_infoDetail, _T("InfoDetail"));
	MoveControlToRect(_infoCurrent, _T("InfoCurrent"));
	MoveControlToRect(_infoTotal, _T("InfoTotal"));

	CRect rc = GetControlRect(_T("InfoDetail"));
	//_infoDetail.SetBgImage(&_BmpBk, rc);
	_infoDetail.SetTextColor(RGB(255, 255, 255));
	_infoDetail.SetTextAlignFormat(DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	rc = GetControlRect(_T("InfoCurrent"));
	//_infoCurrent.SetBgImage(&_BmpBk, rc);
	_infoCurrent.SetTextColor(RGB(255, 255, 255));
	_infoCurrent.SetTextAlignFormat(DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	_infoCurrent.ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	rc = GetControlRect(_T("InfoTotal"));
	//_infoTotal.SetBgImage(&_BmpBk, rc);
	_infoTotal.SetTextColor(RGB(255, 255, 255));
	_infoTotal.SetTextAlignFormat(DT_SINGLELINE | DT_RIGHT | DT_VCENTER);


	CU_SetupRegion(rgn, &dc, _bmpBk, RGB(255, 0, 255));
	SetWindowRgn(rgn, TRUE);
	CenterWindow();

	// Temp Dir
	//TCHAR strBuffer[MAX_PATH];
	::GetTempPath(MAX_PATH, strBuffer);
	_strTmpDir = strBuffer;

	// Install Dir
	if (!ClientPatch::ReadModulePath(strBuffer))
		return FALSE;
	strTmp = strBuffer;
	_strInstallDir = strTmp.Left( strTmp.ReverseFind('\\') + 1 );

	_progressTotal.SetStep(1);
	_progressCurrent.SetStep(1);

	if(__argc == 2)
	{
		CString arg = __targv[1];
		arg.MakeLower();
		if(arg == _T("-qae"))
		{
			PostMessage(M_BEGIN_PATCH_QAEDITOR);
		}
		else if(arg == _T("-qag"))
		{
			PostMessage(M_BEGIN_PATCH_QAGAME);
		}
		else if(arg == _T("-g"))
		{
			PostMessage(M_BEGIN_PATCH_GAME);
		}
		else if(arg == _T("-e"))
		{
			PostMessage(M_BEGIN_PATCH_EDITOR);
		}
	}

	InitPatchDir();

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CFixClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CFixClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, _hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CFixClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(_hIcon);
}

void CFixClientDlg::OnBnClickedMin()
{
	ShowWindow(SW_MINIMIZE);
}

void CFixClientDlg::OnCancel()
{
	if(_isPatchSuccess)
	{
		CDialog::OnCancel();
		return;
	}

	if( MessageBox(_T("取消修复文件将导致游戏不能运行！确定取消吗？")
		, _T("警告！")
		, MB_YESNO | MB_ICONQUESTION) == IDYES
		)
	{
		g_genMD5ThreadPool->Destroy();
		g_compareMD5ThreadPool->Destroy();
		g_downloadThreadPool->Destroy();
		g_writeFileThreadPool->Destroy();

		g_genMD5ThreadPool->Create();
		g_compareMD5ThreadPool->Create();
		g_downloadThreadPool->Create();
		g_writeFileThreadPool->Create();

		CDialog::OnCancel();
	}
}


void FindClientMPQFiles(const CString& path)
{
	CFileFind findClientMPQ;
		
	BOOL working = findClientMPQ.FindFile(path);
	while (working)
	{
		working = findClientMPQ.FindNextFile();

		if(findClientMPQ.IsDots())
		{
			continue;
		}

		if(findClientMPQ.IsDirectory())
		{
			CString subPath = findClientMPQ.GetFilePath() + _T("\\*.*");
			FindClientMPQFiles(subPath);
		}
		else
		{
			CString fileName = findClientMPQ.GetFilePath();
			if(File::GetFileExtFromFullPath(fileName).MakeLower() == PAK_EXT_WITH_POINT)
			{
				_clientMPQFiles.push_back(fileName);
			}
			else
			{
				_clientRawFiles.push_back(fileName);
			}
		}
	}

	findClientMPQ.Close();
}


CString CFixClientDlg::GetTmpFileList() const
{
	return _strTmpDir + TMP_FILE_LIST_FILE;
}

LRESULT CFixClientDlg::OnBeginPatch(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	try
#endif
	{
		TCHAR strBuffer[MAX_PATH] = {0};


		_progressTotal.SetRange(0, 5);
		_progressTotal.SetStep(1);
        _progressCurrent.SetPos(0);
        _progressTotal.SetPos(0);
        _statusDetail = _T("检查本地版本中...");
        _statusTotal = _T("");
        _statusCurrent = _T("正在检查文件，等待更新...");

		//连接Patch服务器
		//_InternetSession.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5);
		//_pHttpConnection = _InternetSession.GetHttpConnection(_strServer, _serverPort);

		//---------------------------------------------------
		// 1.下载服务器文件列表
		//---------------------------------------------------
		_statusTotal = _T("正在下载服务器文件列表...");
		_statusCurrent = _T("下载文件列表...");
		_progressTotal.StepIt();
		M_Update_Msg(TRUE);
		M_Debug_Delay;

// 		CString targFile = GetTmpFileList();
// 		if(!DownLoadServerFile(_serverSection, _T("ViFile"), targFile))
// 		{
// 			_isPatchSuccess = TRUE;
// 			AfxMessageBox(_T("服务器文件列表下载失败，可能是网络不稳定，请稍后重试!"), MB_OK|MB_ICONEXCLAMATION);
// 			return 0;
// 		}
// 		else
		{
			// _firstDownloadFile用前面连接服务器时下载的文件
			LoadViInfo(_patchList._vectorServerViInfoes, _firstDownloadFile);
		}
		
		char szClientVersion[MAX_PATH];
		if (!ClientPatch::ToAnsi(_curClientVer, szClientVersion))
		{
			_isPatchSuccess = FALSE;
			return 0;
		}

		char szCurVersion[MAX_PATH];
		if (!ClientPatch::ToAnsi(g_versionName, szCurVersion))
		{
			_isPatchSuccess = FALSE;
			return 0;
		}

		if (strcmp(szClientVersion, szCurVersion) != 0)
		{
			AfxMessageBox(_T("当前游戏版本号和服务器修复文件版本号不一致，修复失败"), MB_OK);
			_btUpdateGame.EnableWindow(TRUE);
			return 0;
		}

		//---------------------------------------------------
		// 2.查找本地配置目录中的所有文件
		//---------------------------------------------------
		_statusTotal = _T("正在查找本地文件...");
		_statusCurrent = _T("查找本地文件...");
		_progressTotal.StepIt();
		M_Update_Msg(TRUE);
		M_Debug_Delay;

		CString clientDir = _T("");
		::GetPrivateProfileString(_serverSection
			, TEXT("GameDir"), _T("")
			, strBuffer, MAX_PATH - 1
			, _strPatchConfigFile
			);

		clientDir = strBuffer;

		clientDir = App::GetAppPath() + clientDir + _T("*.*");

		_clientMPQFiles.clear();
		_clientRawFiles.clear();
		FindClientMPQFiles(clientDir);

		//---------------------------------------------------
		// 3.生成本地文件的MD5码，MPQ中的文件要从MPQ中读取并生成
		//---------------------------------------------------
		_statusTotal = _T("正在生成本地文件MD5码...");
		_statusCurrent = _T("生成本地文件MD5码...");
		_progressTotal.StepIt();
		M_Update_Msg(TRUE);
		M_Debug_Delay;

		_startGenMD5Time = CTime::GetCurrentTime();
		GenerateLocalMD5();
		_endGenMD5Time = CTime::GetCurrentTime();

		//---------------------------------------------------
		// 4.比较并决定哪些文件要下载
		//---------------------------------------------------
		_statusTotal = _T("正在比较文件MD5码...");
		_statusCurrent = _T("比较文件MD5码...");
		_progressTotal.StepIt();
		M_Update_Msg(TRUE);
		M_Debug_Delay;
		Compare(_patchList);

		//---------------------------------------------------
		// 5.下载并修复文件
		//---------------------------------------------------
		_statusTotal = _T("正在修复损坏的文件...");
		_statusCurrent = _T("修复损坏的文件...");
		_progressTotal.StepIt();
		M_Update_Msg(TRUE);
		M_Debug_Delay;

		_startDownloadTime = CTime::GetCurrentTime();

		DownloadPatch(_patchList);

        //assert(_iCurUpdate == _iTotalUpdate);

		_strLog += _T("//-----------------------------------------------------------------\n");
	}
#ifndef _DEBUG
	catch(...)
	{
		_tprintf(_T("Catch unknown error!"));
	}
#endif

	DownloadPatchOK();
	
	// 合并文件
	DoPatch();

	//_statusDetail = _T("更新完成...");
    _statusCurrent = _T("完成");
    _statusTotal = _T("完成");
    _progressTotal.SetRange(0,100);
    _progressTotal.SetPos(100);
	_progressCurrent.SetRange(0, 100);
    _progressCurrent.SetPos(100);
	//M_Update_Msg(TRUE);



	_isPatchSuccess = TRUE;

	_endDownloadTime = CTime::GetCurrentTime();

	//CDialog::OnOK();

	CTimeSpan chkTm = _endGenMD5Time - _startGenMD5Time;
	CTimeSpan cmpTm = _endCompareMD5Time - _startCompareMD5Time;
	CTimeSpan dowloadTm = _endDownloadTime - _startDownloadTime;

	_statusDetail.Format(_T("生成MD5:%d 比较MD5:%d 下载:%d"), chkTm.GetTotalSeconds(), cmpTm.GetTotalSeconds(), dowloadTm.GetTotalSeconds());
	M_Update_Msg(TRUE);

	std::wcout << _T("Generate MD5 Time:") << chkTm.GetTotalSeconds() << std::endl;
	std::wcout << _T("Compare MD5 Time:") << cmpTm.GetTotalSeconds() << std::endl;
	std::wcout << _T("Download Time:") << dowloadTm.GetTotalSeconds() << std::endl;

	CStdioFile stdFile;
	if( stdFile.Open(_T("Patch.log"), CFile::modeCreate | CFile::modeWrite | CFile::typeText) )
	{
		stdFile.WriteString(_strLog);

		stdFile.Close();
	}

	_btUpdateGame.EnableWindow(TRUE);
	EndDialog(IDOK);
	return 0;
}


void CFixClientDlg::Compare(SFilePatch& filePatch)
{
#if M_USE_MT_UPDATE
	
	g_compareMD5TaskTotalCount = (LONG)filePatch._vectorLocalViInfoes.size();
	g_compareMD5TaskCompletedCount = 0;

	for(size_t t = 0; t < filePatch._vectorLocalViInfoes.size(); ++t)
	{
		CompareMD5Task* task = new CompareMD5Task((int)t, filePatch._vectorLocalViInfoes[t], filePatch);
		g_compareMD5ThreadPool->Run(task);
	}

	_progressCurrent.SetRange(0, (short)g_compareMD5TaskTotalCount);

	//WaitForAllWorks(g_compareMD5ThreadPool, _T("比较MD5工作线程数量:%d"));	
	bool allComplete = false;
	while(!allComplete)
	{
		allComplete = g_compareMD5TaskCompletedCount == g_compareMD5TaskTotalCount;//g_compareMD5ThreadPool->IsAllTaskCompleted();

		_statusDetail.Format(_T("比较MD5:%d/%d 线程数:%d/%d"),
			g_compareMD5TaskCompletedCount, g_compareMD5TaskTotalCount, 
			g_compareMD5ThreadPool->GetWorkingThreadCount(),
			g_compareMD5ThreadPool->GetPoolSize());

		_progressCurrent.SetPos(g_compareMD5TaskCompletedCount);

		M_Update_Msg(true);

		::Sleep(100);
	}

#else
	for(size_t t = 0; t < filePatch._vectorLocalViInfoes.size(); ++t)
	{
		SLocalVersionInfo* pInfoLocal = &(filePatch._vectorLocalViInfoes[t]);

		BOOL bFound = FALSE;
		for(size_t k = 0; k < filePatch._vectorServerViInfoes.size(); ++k)
		{
			SVersionInfo* pInfoServer = &(filePatch._vectorServerViInfoes[k]);

			if( pInfoLocal->_relatedPathName.CompareNoCase(pInfoServer->_relatedPathName) == 0 )
			{
				pInfoServer->isMatched = TRUE;
				pInfoLocal->isMatched = TRUE;

				if(pInfoLocal->_codeMD5 != pInfoServer->_codeMD5)
				{
					filePatch._iPatchCount++;
					filePatch._bShouldUpdateViFile = TRUE;
					if(filePatch._strViFileName.CompareNoCase(_T("PatchInstall.vi2")) == 0)
					{
						pInfoLocal->_iOperation		= 3;	// Patch Install
					}
					else
					{
						pInfoLocal->_iOperation		= 1;	// Patch
					}
					pInfoLocal->_originLen	= pInfoServer->_originLen;
				}
				bFound = TRUE;
				break;
			}
		}

		if(!bFound && pInfoLocal->_contentType != _T("Dir"))
		{
			filePatch._iPatchCount++;
			filePatch._bShouldUpdateViFile = TRUE;
			pInfoLocal->_iOperation = EPO_Remove;	// Remove
		}
	}
#endif

	for(size_t k = 0; k < filePatch._vectorServerViInfoes.size(); ++k)
	{
		SVersionInfo* pInfoServer = &(filePatch._vectorServerViInfoes[k]);

		if(!pInfoServer->_isMatched)
		{
			SLocalVersionInfo info;
			info._relatedPathName	= pInfoServer->_relatedPathName;
			info._codeMD5 = pInfoServer->_codeMD5;
			info._originLen	= pInfoServer->_originLen;
			info._contentType = pInfoServer->_contentType;
			info._codeMD5OnServerFile = pInfoServer->_codeMD5;

			if (filePatch._strViFileName.CompareNoCase(_T("PatchInstall.vi2")) == 0)
			{
				info._iOperation = EPO_Install;	// Patch Install
			}
			else
			{
				info._iOperation = EPO_Patch;	// Patch
			}

			filePatch._iPatchCount++;
// 			filePatch._bShouldUpdateViFile = TRUE;

			filePatch._vectorLocalViInfoes.push_back(info);
		}
	}
}

void CFixClientDlg::DownloadPatch(SFilePatch& filePatch)
{
	// Manage Dir first
	CString strDeltaDir = _T("");
	strDeltaDir = _T("fix_") + g_versionName + _T("\\");
	if (::CreateDirectory(strDeltaDir, 0))
	{
		_strLog += _T("CreateDirectory : ") + strDeltaDir;
		_strLog += _T("\n");
	}

	map<CString, int>	strDirMap;
	for(size_t t = 0; t < filePatch._vectorLocalViInfoes.size(); ++t)
	{
		SLocalVersionInfo* pInfoLocal = &(filePatch._vectorLocalViInfoes[t]);
		if(pInfoLocal->_contentType == _T("Dir"))
		{
			CString strTmp = strDeltaDir + pInfoLocal->_relatedPathName;
			strDirMap.insert(make_pair(strTmp, 0));

			pInfoLocal->_iOperation = EPO_None;
		}
	}
	for(map<CString, int>::iterator it = strDirMap.begin(); it != strDirMap.end(); ++it)
	{
		if (::CreateDirectory(it->first, 0))
		{
			_strLog += _T("CreateDirectory : ") + it->first;
			_strLog += _T("\n");
		}
	}


	g_downloadTaskTotalCount = 0;//(LONG)filePatch._vectorLocalViInfoes.size();
	g_downloadTaskCompletedCount = 0;
	g_writeFileTaskCompletedCount = 0;
		
	_totalUpdate = (int)filePatch._vectorLocalViInfoes.size();
	_progressCurrent.SetRange(0, (short)_totalUpdate);

	for(size_t t = 0; t < filePatch._vectorLocalViInfoes.size(); ++t)
	{
		SLocalVersionInfo* pInfoLocal = &(filePatch._vectorLocalViInfoes[t]);

		BOOL bUpdateItem = FALSE;

		if (pInfoLocal->_iOperation == EPO_Patch	||	// Patch
			pInfoLocal->_iOperation == EPO_Install)		// Patch Install
		{
			if(pInfoLocal->_contentType == _T("Dir"))
			{
				// 'Dir' has already managed above

				//bUpdateItem = TRUE;
			}
			else if (pInfoLocal->_contentType == _T("File"))
			{
				if(pInfoLocal->_relatedPathName.Find(_T("VersionPatch.exe")) != -1)
				{
					_strLog += _T("//-----------------------------------------------------------------\n");
					_strLog += _T("Should PatchSelf : ") + pInfoLocal->_relatedPathName;
					_strLog += _T("\n");
				}

				InterlockedIncrement(&g_downloadTaskTotalCount);
				DownloadTask* task = new DownloadTask((int)t, 
					_internetSession,
					g_downloadServerIP,
					g_downloadServerPort,
					g_downloadObjectURI,
					*pInfoLocal, 
					strDeltaDir,
					_strTmpDir);
				g_downloadThreadPool->Run(task);
				
				bUpdateItem = TRUE;

				_strLog += _T("DownLoadFile : ") + pInfoLocal->_relatedPathName;
				if(pInfoLocal->_originLen > 0)
				{
					_strLog += _T(".zfp");
				}
				_strLog += _T("\n");
			}
		}
		else if(pInfoLocal->_iOperation == EPO_Remove)	// Remove
		{
			if(pInfoLocal->_contentType == _T("Dir"))
			{
			}
			else
			{
				CString strTmp = pInfoLocal->_relatedPathName;
				//::DeleteFile(strTmp);
				bUpdateItem = TRUE;

				_strLog += _T("DeleteFile : ") + strTmp;
				_strLog += _T("\n");
			}
		}

		if(bUpdateItem)
		{
			_curUpdate ++;
            //m_ProgressAll.SetPos(_iCurUpdate);
            assert(_curUpdate <= _totalUpdate);
			_statusTotal.Format(_T("%d/%d"), _curUpdate, _totalUpdate);

			_progressCurrent.SetPos(_curUpdate);

			M_Update_Msg(TRUE);
		}
	}

	//m_ProgressAll.StepIt();
}

CBrush br1(RGB(0, 153, 204));
CBrush br2(RGB(255, 255, 204));

HBRUSH CFixClientDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	SAFE;
	
	HBRUSH br = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	UNREFERENCED_PARAMETER(br);

	pDC->SetBkMode(TRANSPARENT);
	return NULL;



	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 255, 255));

	if(pWnd->GetDlgCtrlID() == IDC_STATUS_DETAIL)
	{
		pDC->SetTextColor(RGB(204, 102, 0));
		return br2;
	}

	return br1;

	UNSAFE;
}

LRESULT CFixClientDlg::OnNcHitTest(CPoint point)
{
	SAFE;

	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);

	UNSAFE;
}

BOOL CFixClientDlg::OnEraseBkgnd(CDC* pDC)
{
	SAFE;

	CU_TransparentBlt(pDC, _bmpBk, CPoint(0, 0), CSize(0, 0), TRUE, RGB(255, 0, 255));

	return TRUE;

	UNSAFE;
}

int CFixClientDlg::StartProcess(const CString& strExeFilePatchName)
{
	CString strCmd = strExeFilePatchName;

	//CTime	ctime;
	PROCESS_INFORMATION stProcessInfo;
	ZeroMemory( &stProcessInfo, sizeof(stProcessInfo) );

	STARTUPINFO stStartup;
	ZeroMemory( &stStartup, sizeof(stStartup) );
	stStartup.cb = sizeof(stStartup);
	stStartup.dwFlags =  STARTF_USESHOWWINDOW;
	stStartup.wShowWindow = SW_SHOW;

	// Start the child process. 
	if( CreateProcess(NULL, // No module name (use command line). 
		(LPWSTR)(LPCTSTR)strCmd,		// Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		CREATE_SUSPENDED,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&stStartup,              // Pointer to STARTUPINFO structure.
		&stProcessInfo )             // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		::ResumeThread(stProcessInfo.hThread);
		//ctime = CTime::GetCurrentTime();
	}
	else
	{
		return 1;
	}

	// Wait until child process exits.
	WaitForSingleObject( stProcessInfo.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( stProcessInfo.hProcess );
	CloseHandle( stProcessInfo.hThread );

	return 0;
}

void CFixClientDlg::OnBnClickedUpdateGame()
{
	_serverSection = _T("Mir3D");
	_btUpdateGame.EnableWindow(FALSE);
	StartFix();
}


void CFixClientDlg::DownloadPatchOK()
{
#if M_USE_MT_UPDATE

	//_progressCurrent.SetRange(0, (short)g_downloadTaskTotalCount);
	//_progressTotal.SetRange(0, (short)g_downloadTaskTotalCount);

	
	bool allComplete = false;
	while(!allComplete)
	{
		allComplete = (g_downloadTaskCompletedCount == g_downloadTaskTotalCount)
			&& (g_writeFileTaskCompletedCount == g_downloadTaskTotalCount);

		_statusCurrent = CString(_T("下载:")) + g_downloadTaskDetail;
		//m_ProgressCurrent.SetPos(0);
		M_Update_Msg(TRUE);

		_statusDetail.Format(_T("下载:%d/%d (%d/%d) 写文件:%d/%d (%d/%d)"),
			g_downloadTaskCompletedCount, g_downloadTaskTotalCount, g_downloadThreadPool->GetWorkingThreadCount(), g_downloadThreadPool->GetPoolSize(),
			g_writeFileTaskCompletedCount, g_downloadTaskTotalCount, g_writeFileThreadPool->GetWorkingThreadCount(), g_writeFileThreadPool->GetPoolSize());

		//_progressCurrent.SetPos(g_downloadTaskCompletedCount);
		//_progressTotal.SetPos(g_writeFileTaskCompletedCount);

		M_Update_Msg(true);

		::Sleep(100);
	}

	g_genMD5ThreadPool->Destroy();
	g_compareMD5ThreadPool->Destroy();
	g_downloadThreadPool->Destroy();
	g_writeFileThreadPool->Destroy();

	g_genMD5ThreadPool->Create();
	g_compareMD5ThreadPool->Create();
	g_downloadThreadPool->Create();
	g_writeFileThreadPool->Create();

#endif

	_btUpdateGame.EnableWindow(TRUE);

	if(_httpConnectionPtr)
	{
		_httpConnectionPtr->Close();
	}

	if(_internetSession)
	{
		delete _internetSession;
		_internetSession = NULL;
	}
}

// add by zhangrui05
// >>
// fix all files
void CFixClientDlg::DoPatch()
{
	_statusDetail = _T("文件修复中，请稍后...");
	M_Update_Msg(TRUE);

	TCHAR strBuffer[MAX_PATH] = { 0 };
	CString clientDir = _T("");
	::GetPrivateProfileString(_serverSection
		, TEXT("GameDir"), _T("")
		, strBuffer, MAX_PATH - 1
		, _strPatchConfigFile);

	clientDir = strBuffer;
	clientDir = App::GetAppPath() + clientDir;
	// 去掉最后一个"\\"
	clientDir = clientDir.Left(clientDir.ReverseFind(_T('\\')));
	clientDir.Replace('/', '\\');

	CString deltaDir = App::GetAppPath() + _T("fix_") + g_versionName;
	deltaDir.Replace('/', '\\');

	char szClientPath[MAX_PATH];
	if (!ClientPatch::ToAnsi(clientDir, szClientPath))
	{
		_isPatchSuccess = FALSE;
		return;
	}

	char szDeltaPath[MAX_PATH];
	if (!ClientPatch::ToAnsi(deltaDir, szDeltaPath))
	{
		_isPatchSuccess = FALSE;
		return;
	}

	_progressCurrent.SetRange(0, 100);
	_progressCurrent.SetPos(100);
	M_Update_Msg(true);

	if (Patch(szClientPath, szDeltaPath, "\0"))
	{
		CString strTips = _T("版本:") + g_versionName + _T("修复成功, 请重新运行客户端.");
		AfxMessageBox(strTips);
	}
	else
	{
		CString strTips = _T("版本:") + g_versionName + _T("修复失败, 请重新下载客户端.");
		AfxMessageBox(strTips);
	}

	// 删除缓存目录
	PathString strDeltaDir = _T("fix_") + g_versionName;
	if (IsDirExist(strDeltaDir))
	{
		DeleteFilesInDirectory(strDeltaDir.c_str(), true);
	}
}
// <<

void CFixClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData(FALSE);
	_countDownTime++;

	CDialog::OnTimer(nIDEvent);
}

void CFixClientDlg::StartFix()
{
	UpdateData();

	_firstDownloadFile.Empty();

	DWORD id = FindAppProcessID(_T("MMOGame-Win32-Shipping.exe"));
	if(id != -1)
	{
		if(AfxMessageBox(_T("检测到游戏正在运行，请先关闭游戏才能进行更新！\n\n是否强行关闭游戏？")
			, MB_OKCANCEL | MB_ICONHAND) == IDOK
			)
		{
			HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
			if(ProcessHandle)
				TerminateProcess(ProcessHandle, 0);
		}
		else
		{
			_btUpdateGame.EnableWindow(TRUE);
			return;
		}
	}
	
	_isPatchSuccess = FALSE;

	_patchList.Clear();
	_clientMPQFiles.clear();
	_clientRawFiles.clear();

	vector<SServer> vectorServers;
	vector<SServer> vectorServersValid;

	// Install Dir
	TCHAR strBuffer[2048] = {0};
// 	::GetModuleFileName(NULL, strBuffer, MAX_PATH);
// 	CString strTmp = strBuffer;
// 	_strPatchConfigFile = strTmp.Left( strTmp.ReverseFind('\\') + 1 );
// 	_strPatchConfigFile += C_IniFile;
// 
	int iServer = ::GetPrivateProfileInt(_serverSection, _T("ServerCount"), 0, _strPatchConfigFile);
	if(iServer < 0)
		iServer = 0;

	CString strTmp;
	for(int i = 0; i < iServer; ++i)
	{
		strTmp.Format(_T("Server%d"), i+1);
		::GetPrivateProfileString(_serverSection
			, strTmp, _T("")
			, strBuffer, 2048 - 1
			, _strPatchConfigFile
			);

		if( _tcslen(strBuffer) > 6 )
		{
			SServer server;
			server._fullServerURL = strBuffer;
			DWORD dwType;
			if( ::AfxParseURL(strBuffer, dwType, server._strServer, server._strServerInitPath, server._dwPort)
				&&
				(AFX_INET_SERVICE_HTTP == dwType ||
				AFX_INET_SERVICE_HTTPS == dwType)
				)
			{
				vectorServers.push_back(server);
			}
		}
	}

	int iValidServer = -1;
	if(vectorServers.size() == 0)
	{
		AfxMessageBox(_T("没有发现更新服务器配置文件：FixClient.ini！\n\n也可能更新服务器配置有误！"), MB_OK | MB_ICONINFORMATION);
		_btUpdateGame.EnableWindow(TRUE);
		return;
	}
	else
	{
		//Show InfoUI
		//		CheckServerDlg checkServerDlg;
		//		checkServerDlg.Create(CheckServerDlg::IDD);
		//		checkServerDlg.ShowWindow(SW_SHOW);
		//		checkServerDlg.UpdateWindow();

		//CInternetSession		_InternetSession;
		//CHttpConnection			*_pHttpConnection;
		_internetSession = new CInternetSession();
		//_InternetSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5);

		// 下载 CliV.ini
		::GetPrivateProfileString(_serverSection
			, _T("VersionServer")
			, _T("")
			, strBuffer
			, 2048 - 1
			, _strPatchConfigFile
			);

		bool downloadSucc = false;
		if (_tcslen(strBuffer) > 6)
		{
			SServer server;
			server._fullServerURL = strBuffer;
			DWORD dwType;
			if (::AfxParseURL(strBuffer, dwType, server._strServer, server._strServerInitPath, server._dwPort) &&
				(AFX_INET_SERVICE_HTTP == dwType || AFX_INET_SERVICE_HTTPS == dwType))
			{
				CString testURL = strBuffer;
				testURL += _T("CliV.ini");
				CString tmpTargFile = _T("CliV.ini");
				if (HttpsDownload(testURL, tmpTargFile))
				{
					downloadSucc = true;
				}
			}
		}

		if (!downloadSucc)
		{
			::GetPrivateProfileString(_serverSection
				, _T("BackupVersionServer")
				, _T("")
				, strBuffer
				, 2048 - 1
				, _strPatchConfigFile
				);

			if (_tcslen(strBuffer) > 6)
			{
				SServer server;
				server._fullServerURL = strBuffer;
				DWORD dwType;
				if (::AfxParseURL(strBuffer, dwType, server._strServer, server._strServerInitPath, server._dwPort) &&
					(AFX_INET_SERVICE_HTTP == dwType || AFX_INET_SERVICE_HTTPS == dwType))
				{
					CString testURL = strBuffer;
					testURL += _T("CliV.ini");
					CString tmpTargFile = _T("CliV.ini");
					if (HttpsDownload(testURL, tmpTargFile))
					{
						downloadSucc = true;
					}
				}
			}
		}

		if (!downloadSucc)
		{
			AfxMessageBox(_T("未找到可用的更新服务器，无法下载CliV.ini！"), MB_OK | MB_ICONEXCLAMATION);
			_isPatchSuccess = TRUE;
			_btUpdateGame.EnableWindow(TRUE);
			return;
		}

		if (!File::IsFileExist(_T("CliV.ini")))
		{
			AfxMessageBox(_T("无法读取CliV.ini，修复失败!"), MB_OK);
			_btUpdateGame.EnableWindow(TRUE);
			return;
		}

		// 先判断文件是unicode 还是 ansi
		CStdioFile verFile;
		if (!verFile.Open(_T("CliV.ini"), CFile::modeRead))
		{
			AfxMessageBox(_T("无法读取CliV.ini，修复失败!"), MB_OK);
			_btUpdateGame.EnableWindow(TRUE);
			return;
		}

		verFile.SeekToBegin();
		const bool bUnicode = ClientPatch::HasUnicodeBom(verFile);

		verFile.Close();

		if (bUnicode)
		{
			if (verFile.Open(_T("CliV.ini"), CFile::modeRead | CFile::typeBinary))
			{
				verFile.Seek(2, CFile::begin);
				if (!verFile.ReadString(_curClientVer))
				{
					verFile.Close();
					AfxMessageBox(_T("无法读取CliV.ini，修复失败!"), MB_OK);
					_btUpdateGame.EnableWindow(TRUE);
					return;
				}
				verFile.Close();
			}
		}
		else
		{
			if (verFile.Open(_T("CliV.ini"), CFile::modeRead | CFile::typeText))
			{
				if (!verFile.ReadString(_curClientVer))
				{
					verFile.Close();
					AfxMessageBox(_T("无法读取CliV.ini，修复失败!"), MB_OK);
					_btUpdateGame.EnableWindow(TRUE);
					return;
				}
				verFile.Close();
			}
		}
		//_curClientVer = _T("0.0.1.2");

		strTmp = "ViFile";
		::GetPrivateProfileString(_serverSection
			, strTmp, _T("")
			, strBuffer,  2048 - 1
			, _strPatchConfigFile
			);
#if 1
		vectorServersValid.clear();
		for(size_t idx = 0; idx < vectorServers.size(); ++idx)
		{
			SServer* it = &vectorServers[idx];
			CString testURL = it->_fullServerURL + _curClientVer + _T("/") + strBuffer;
			CString tmpTargFile;
			tmpTargFile.Format(_T("%d_%s"), idx, (LPCTSTR)strBuffer);
			if(HttpsDownload(testURL, tmpTargFile))
			{
				_firstDownloadFile = tmpTargFile;
				vectorServersValid.push_back(*it);
				break;
			}
		}
#else

		// 轮询Patch服务器
		vector<SServer>::iterator it;
		for(it = vectorServers.begin(); it != vectorServers.end(); ++it)
		{
			CString strTestFile = it->_strServerInitPath + strBuffer;//_T("index.html");//
			_httpConnectionPtr = _internetSession->GetHttpConnection(it->_strServer, INTERNET_FLAG_SECURE, it->_dwPort);
			CHttpFile *pFile = _httpConnectionPtr->OpenRequest(CHttpConnection::HTTP_VERB_GET
				, strTestFile
				, NULL
				, 1
				, NULL
				, NULL
				, HSR_DOWNLOAD |
				INTERNET_FLAG_SECURE |
				INTERNET_FLAG_EXISTING_CONNECT |
				INTERNET_FLAG_RELOAD |
				INTERNET_FLAG_NO_CACHE_WRITE |
				INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
				INTERNET_FLAG_IGNORE_CERT_CN_INVALID
				);

			if(!pFile)
			{
				it = vectorServers.erase(it);
				continue;
			}

			BOOL bWebOK = FALSE;

			try
			{
				if(!pFile->SendRequest())
				{
					pFile->Close();
					it = vectorServers.erase(it);
					continue;
				}

				DWORD dwStatusCode;
				if(pFile->QueryInfoStatusCode(dwStatusCode))
				{
					//200-299 Success 
					if(dwStatusCode >= 200 && dwStatusCode < 300)
					{
						bWebOK = TRUE;
					}
				}
			}
			catch (CException*)
			{
				//exp->ReportError();
			}

			if(!bWebOK)
			{
				pFile->Close();
				it = vectorServers.erase(it);
				continue;
			}

			pFile->Close();
		}

#endif
		//Hide InfoUI
		//		checkServerDlg.ShowWindow(SW_HIDE);
	}

	int iValidServerCount = int(vectorServersValid.size());
	if(iValidServerCount == 0)
	{
		AfxMessageBox(_T("未找到可用的更新服务器!"), MB_OK|MB_ICONEXCLAMATION);
		_isPatchSuccess = TRUE;
		_btUpdateGame.EnableWindow(TRUE);
		return;
	}

	if(iValidServerCount == 1)
		iValidServer = 0;
	else
	{
		iValidServer = rand() % iValidServerCount;
	}

	_fullServerURL = vectorServersValid[iValidServer]._fullServerURL;
	_serverIP		= vectorServersValid[iValidServer]._strServer;
	_serverPort = vectorServersValid[iValidServer]._dwPort;
	_initURLRoot	= vectorServersValid[iValidServer]._strServerInitPath;

	PostMessage(M_BEGIN_PATCH);
}
// 
// bool CFixClientDlg::_LoadLocalViInfo( const CString& viFile, SFilePatch &filePatch, bool newInstall )
// {
// 	CStdioFile stdFile;
// 	if( stdFile.Open(viFile, CFile::modeRead | CFile::typeText) )
// 	{
// 		SLocalVersionInfo localViInfo;
// 
// 		CString strTmp;
// 		while(stdFile.ReadString(strTmp))
// 		{
// 			int iPos1, iPos2;
// 			iPos1 = strTmp.Find(_T(','));
// 			localViInfo._strRelatedPathName = strTmp.Left(iPos1);
// 
// 			iPos2 = strTmp.Find(_T(','), iPos1 + 1);
// 			localViInfo._strType = strTmp.Mid(iPos1 + 1, iPos2 - iPos1 - 1);
// 
// 			iPos1 = iPos2;
// 			iPos2 = strTmp.Find(_T(','), iPos1 + 1);
// 			localViInfo._strMD5 = strTmp.Mid(iPos1 + 1, iPos2 - iPos1 - 1);
// 
// 			if(newInstall)
// 			{
// 				localViInfo._strMD5 = "Missing";
// 			}
// 
// 			CString str = strTmp.Right(strTmp.GetLength() - iPos2 - 1);
// 			localViInfo._uOriginLen = _ttoi((LPCTSTR)str);
// 
// 			if(localViInfo._strRelatedPathName.GetLength() &&
// 				( localViInfo._strType == _T("Dir") || localViInfo._strType == _T("File") )
// 				)
// 				filePatch._vectorLocalViInfoes.push_back(localViInfo);
// 		}
// 
// 		stdFile.Close();
// 		return true;
// 	}
// 
// 	return false;
// }


void CFixClientDlg::OnDestroy()
{
	CDialog::OnDestroy();
}


BOOL CFixClientDlg::DownLoadServerFileToString(const CString& section_name, const CString& inkey_name, CString& out_string)
{
	int server_count = GetPrivateProfileInt(section_name, _T("ServerCount"), -1, _strPatchConfigFile);
	if (server_count == -1)
		return FALSE;

	TCHAR ver_file[MAX_PATH] = {0};
	GetPrivateProfileString(section_name, inkey_name, _T(""), ver_file, MAX_PATH-1, _strPatchConfigFile);
	if (!*ver_file)
		return FALSE;

	for (int i = 0; i < server_count; ++i)
	{
		CString key_name;
		key_name.Format(_T("Server%d"), server_count);
		TCHAR str_buf[MAX_PATH] = {0};
		GetPrivateProfileString(section_name, key_name, _T(""), str_buf, MAX_PATH-1, _strPatchConfigFile);
		if (!*str_buf)
			continue;

		DWORD svr_type;
		INTERNET_PORT svr_port;
		CString svr_ip, svr_path;
		_tcscat_s(str_buf, ver_file);
		if (AfxParseURL(str_buf, svr_type, svr_ip, svr_path, svr_port) && AFX_INET_SERVICE_HTTP == svr_type)
		{
			CInternetSession inetSession;
			//inetSession.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10);
			CHttpConnection* httpConnect = inetSession.GetHttpConnection(svr_ip, svr_port);
			if (!httpConnect)
				continue;
			CHttpFile* http_file = httpConnect->OpenRequest(CHttpConnection::HTTP_VERB_GET
				, svr_path
				, NULL
				, 1
				, NULL
				, NULL
				, HSR_DOWNLOAD | INTERNET_FLAG_RELOAD | INTERNET_FLAG_EXISTING_CONNECT
				);
			if (!http_file)
				continue;

			BOOL bWebOK = FALSE;
			try
			{
				if(!http_file->SendRequest())
				{
					http_file->Close();
					continue;
				}
				DWORD dwStatusCode;
				if(http_file->QueryInfoStatusCode(dwStatusCode))
				{
					if(dwStatusCode >= 200 && dwStatusCode < 300)
						bWebOK = TRUE;
				}
			}
			catch (CException*)
			{
				//exp->ReportError();
			}
			if (bWebOK)
			{
				char file_buf[2048] = {0};
				int n;
				CStringA file_text;
				while ((n = http_file->Read(file_buf,2048) ) > 0)
					file_text.Append(file_buf, n);
				out_string = file_text;
				http_file->Close();
				return TRUE;
			}
			http_file->Close();
		}
	}

	return FALSE;
}



BOOL CFixClientDlg::DownLoadServerFile(const CString& section_name, const CString& inkey_name, const CString& targetFileName)
{
	CString strFilePathName = _initURLRoot;	//_T("/PatchRoot/");

	TCHAR ver_file[MAX_PATH] = {0};
	GetPrivateProfileString(section_name, inkey_name, _T(""), ver_file, MAX_PATH-1, _strPatchConfigFile);
	if (!*ver_file)
	{
		return FALSE;
	}

	strFilePathName += ver_file;//filePatch._strServerFileSuffix;

#if 1
	CString downLoadFile = _fullServerURL + ver_file;
	bool downOK = HttpsDownload(downLoadFile, targetFileName);
	return downOK ? TRUE : FALSE;
#else
	CHttpFile *pFile = _httpConnectionPtr->OpenRequest(CHttpConnection::HTTP_VERB_GET
		, strFilePathName 
		, NULL
		, 1
		, NULL
		, NULL
		, HSR_DOWNLOAD | INTERNET_FLAG_RELOAD | INTERNET_FLAG_EXISTING_CONNECT
		);
	if(!pFile)
		return FALSE;

	if(!pFile->SendRequest())
	{
		pFile->Close();
		return FALSE;
	}

	DWORD dwStatusCode;
	if(pFile->QueryInfoStatusCode(dwStatusCode))
	{
		//200-299 Success 
		if( !(dwStatusCode >= 200 && dwStatusCode < 300) )
		{
			pFile->Close();
			return FALSE;
		}
		else
		{
			// 301是重定向
			if(dwStatusCode == 301)
			{
				int iii = 0;
				++iii;
			}
		}
	}

	CStdioFile csf;
	CString targFile = targetFileName;
	// 	if(targFile.IsEmpty())
	// 	{
	// 		targFile = _strTmpDir + filePatch._strViFileName;
	// 	}

	::DeleteFile(targFile);

	csf.Open(targFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

	TCHAR buf[4096];
	int n;
	while( (n = pFile->Read(buf,4096) ) > 0 )
	{
		csf.Write(buf,n);
	}

	csf.Close();

	pFile->Close();
	return TRUE;
#endif
}


BOOL CFixClientDlg::LoadViInfo(VersionInfoes& filePatch, const CString& fileName)
{
	g_versionName = _T("");
	CString strFileName = fileName;//_strTmpDir + filePatch._strViFileName;
	CStdioFile stdFile;
	if( stdFile.Open(strFileName, CFile::modeRead | CFile::typeText) )
	{
		SVersionInfo ViInfo;
				
		CString strTmp;
		// read version & url
		if(stdFile.ReadString(strTmp))
		{
			int iPos1;
			iPos1 = strTmp.Find(_T(','));
			g_downloadURLRoot = strTmp.Left(iPos1);

			g_versionName = strTmp.Right(strTmp.GetLength() - iPos1 - 1);

			AfxParseURL(g_downloadURLRoot, g_downloadServiceType, g_downloadServerIP,
				g_downloadObjectURI, g_downloadServerPort);
			g_downloadObjectURI.Replace('/', '\\');
		}

		while(stdFile.ReadString(strTmp))
		{
			int iPos1, iPos2;
			iPos1 = strTmp.Find(_T(','));
			ViInfo._relatedPathName = strTmp.Left(iPos1);
			ViInfo._relatedPathName.Replace('/', '\\');

			iPos2 = strTmp.Find(_T(','), iPos1 + 1);
			ViInfo._contentType = strTmp.Mid(iPos1 + 1, iPos2 - iPos1 - 1);

			iPos1 = iPos2;
			iPos2 = strTmp.Find(_T(','), iPos1 + 1);
			ViInfo._codeMD5 = strTmp.Mid(iPos1 + 1, iPos2 - iPos1 - 1);

			CString str = strTmp.Right(strTmp.GetLength() - iPos2 - 1);
			ViInfo._originLen = _ttoi((LPCTSTR)str);

 			if(ViInfo._relatedPathName.GetLength() &&
 				( ViInfo._contentType == _T("Dir") || ViInfo._contentType == _T("File") )
 				)
			{
				filePatch.push_back(ViInfo);
			}
		}

		stdFile.Close();

		return (filePatch.size() != 0);
	}
	else
	{
		return FALSE;
	}
}

void CFixClientDlg::GenerateLocalMD5()
{
	InitializeCriticalSection(&g_csGenMD5);

	//---------------------------------------------------
	// 1.先计算总计要处理的文件，包括MPQ中的文件数量
	//---------------------------------------------------
	//CStringVector _clientRawFiles;

	size_t totalFileCnt = _clientRawFiles.size();
	for (size_t t = 0; t < _clientMPQFiles.size(); ++t)
	{
		// ZRTODO: totalFileCnt += _clientMPQFiles[t].fileCount
		// add by zhangrui
		// >>
		CString filePath = _clientMPQFiles[t];
		int fileCount = ((*g_pfGetPackFileCount)(filePath.GetBuffer(filePath.GetLength())));
		filePath.ReleaseBuffer();
		totalFileCnt += fileCount;
		// <<
	}

	_patchList._vectorLocalViInfoes.resize(totalFileCnt);

	//---------------------------------------------------
	// 2.处理Raw Files
	//---------------------------------------------------
	//CStringVector _clientRawFiles;

	TCHAR strBuffer[MAX_PATH] = { 0 };
	CString clientDir = _T("");
	::GetPrivateProfileString(_serverSection
		, TEXT("GameDir"), _T("")
		, strBuffer, MAX_PATH - 1
		, _strPatchConfigFile);

	clientDir = strBuffer;
	clientDir = App::GetAppPath() + clientDir;
	clientDir.Replace('/', '\\');

	int viInfoIndex = 0;
	for (size_t t = 0; t < _clientRawFiles.size(); ++t)
	{
		CString filePath = _clientRawFiles[t]; 

		if (viInfoIndex >= (int)(_patchList._vectorLocalViInfoes.size()))
		{
			break;
		}

		SLocalVersionInfo* ViInfo = &_patchList._vectorLocalViInfoes[viInfoIndex];
		ViInfo->_relatedPathName = filePath.Right(filePath.GetLength() - clientDir.GetLength());

		CFileStatus rStatus;
		if( CFile::GetStatus(filePath, rStatus) )
		{
			CString msgFileName = filePath;
			msgFileName = filePath.Right(filePath.GetLength() - clientDir.GetLength());
			_statusDetail.Format(_T("正在准备计算文件MD5码任务：%s..."), (LPCTSTR)msgFileName);
			M_Update_Msg(true);

			// 增加一个多线程任务计数，以便后面比较工作是否完成
			InterlockedIncrement(&g_genMD5TaskTotalCount);
			GenerateMD5Task* task = new GenerateMD5Task(viInfoIndex, filePath, L"", FALSE, ViInfo->_codeMD5);
			g_genMD5ThreadPool->Run(task);
		}
		else
		{
			// local File or Dir missing
			ViInfo->_codeMD5 = _T("Missing");

			_strLog += _T("Missing Local File : ");
			_strLog += filePath;
			_strLog += _T("\n");
		}

		viInfoIndex ++;
	}
	
	//---------------------------------------------------
	// 3.处理MPQ中的文件的MD5
	//---------------------------------------------------

	//CStringVector _clientMPQFiles;
	for (size_t t = 0; t < _clientMPQFiles.size(); ++t)
	{
		// ZRTODO:
		// add by zhangrui
		// >>
		CString filePath = _clientMPQFiles[t];
		unsigned int fileCount = ((*g_pfGetPackFileCount)(filePath.GetBuffer(filePath.GetLength())));

		char(*fileList)[MAX_PATH] = new char[fileCount][MAX_PATH];
		bool success = ((*g_pfGetPackFileList)(filePath.GetBuffer(filePath.GetLength()), fileList, fileCount, fileCount));
		if (success)
		{
			CString fileRelatedPath = filePath.Left(filePath.ReverseFind('\\') + 1);
			fileRelatedPath = fileRelatedPath.Right(fileRelatedPath.GetLength() - clientDir.GetLength());

			for (size_t i = 0; i < fileCount; ++i)
			{
				CString fileName = CString(fileList[i]);

				if (viInfoIndex >= (int)(_patchList._vectorLocalViInfoes.size()))
				{
					break;
				}

				SLocalVersionInfo* ViInfo = &_patchList._vectorLocalViInfoes[viInfoIndex];
				ViInfo->_relatedPathName = fileRelatedPath + fileName;

				CString msgFileName = fileName;
				_statusDetail.Format(_T("正在准备计算文件MD5码任务：MPQ:%s..."), (LPCTSTR)fileName);
				M_Update_Msg(true);

				// 增加一个多线程任务计数，以便后面比较工作是否完成
				InterlockedIncrement(&g_genMD5TaskTotalCount);
				GenerateMD5Task* task = new GenerateMD5Task(viInfoIndex, filePath, fileName, TRUE, ViInfo->_codeMD5);
				g_genMD5ThreadPool->Run(task);

				viInfoIndex ++;
			}
		}
		else
		{
			_strLog += _T("Missing Local File : ");
			_strLog += filePath;
			_strLog += _T("\n");
		}
		delete[]fileList;
		// <<
	}


#if M_USE_MT_UPDATE
	//WaitForAllWorks(g_genMD5ThreadPool, _T("MD5生成工作线程数量:%d"));
	_progressCurrent.SetRange(0, (short)g_genMD5TaskTotalCount);
	_statusDetail = _T("请耐心等待...");

	bool allComplete = false;
	while(!allComplete)
	{
		allComplete = g_genMD5TaskCompletedCount == g_genMD5TaskTotalCount;//g_genMD5ThreadPool->IsAllTaskCompleted();

		_statusCurrent.Format(_T("生成文件MD5码进度:%d/%d 工作中线程数:%d/%d"),
			g_genMD5TaskCompletedCount, g_genMD5TaskTotalCount,
			g_genMD5ThreadPool->GetWorkingThreadCount(),
			g_genMD5ThreadPool->GetPoolSize());

		_progressCurrent.SetPos(g_genMD5TaskCompletedCount);

		M_Update_Msg(true);

		::Sleep(100);
	}
#endif

	DeleteCriticalSection(&g_csGenMD5);
}


bool HttpsDownload(const CString& strURL, const CString& targetFilePath, CInternetSession* sess)
{
	DWORD dwStatus = 0;

	DWORD dwServiceType = 0;
	INTERNET_PORT serverPort = INTERNET_INVALID_PORT_NUMBER;
	CString serverIP, objectURI;
	if (!AfxParseURL(strURL, dwServiceType, serverIP, objectURI, serverPort))
		return false;

	CInternetSession* internetSession = sess ? sess : new CInternetSession();
	CHttpConnection* httpConn = NULL;
	CHttpFile* httpFile = NULL;

	bool downloadOK = false;

	try
	{
		if(serverPort == INTERNET_DEFAULT_HTTPS_PORT)
		{
			// 必须传入INTERNET_FLAG_SECURE
			httpConn = internetSession->GetHttpConnection(
				serverIP, INTERNET_FLAG_SECURE, serverPort, NULL, NULL);
		}
		else
		{
			httpConn = internetSession->GetHttpConnection(serverIP, serverPort);
		}

		if (httpConn)
		{
			DWORD dwOpenRequestFlags =
				//INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
				INTERNET_FLAG_KEEP_CONNECTION |
				INTERNET_FLAG_NO_AUTH |
				INTERNET_FLAG_NO_COOKIES |
				INTERNET_FLAG_NO_UI |
				INTERNET_FLAG_DONT_CACHE |
				INTERNET_FLAG_RELOAD |
				HSR_DOWNLOAD |
				INTERNET_OPTION_URL;

			if(serverPort == INTERNET_DEFAULT_HTTPS_PORT)
			{
				dwOpenRequestFlags |=
					INTERNET_FLAG_NO_AUTO_REDIRECT	// 必须自己处理重定向								
					| INTERNET_FLAG_SECURE				// 设置启用HTTPS
					//SECURITY_FLAG_IGNORE_UNKNOWN_CA |
					| INTERNET_FLAG_IGNORE_CERT_CN_INVALID
					| INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
			}

			httpFile = httpConn->OpenRequest(CHttpConnection::HTTP_VERB_GET, objectURI, NULL, 1,
				NULL, NULL, dwOpenRequestFlags);


			if(serverPort == INTERNET_DEFAULT_HTTPS_PORT)
			{
				DWORD dwFlags = 0;

				DWORD dwFlagsSecurity = SECURITY_FLAG_IGNORE_UNKNOWN_CA
					| SECURITY_FLAG_IGNORE_CERT_CN_INVALID
					| SECURITY_FLAG_IGNORE_CERT_DATE_INVALID
					;
				dwFlags |= dwFlagsSecurity;

				BOOL r = httpFile->SetOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
				UNREFERENCED_PARAMETER(r);
			}

			if (httpFile->SendRequest())
			{
				httpFile->QueryInfoStatusCode(dwStatus);

				if ((dwStatus == 301) || (dwStatus == 302))
				{
					CStringA htmlContent;
					char buf[4096] = { 0 };
					int n = 0;
					while ((n = httpFile->Read(buf, 4096)) > 0)
					{
						htmlContent.Append(buf, n);
					}

					httpFile->Close();
					delete httpFile;
					httpFile = NULL;

					CStringA tag = ("href=\"");
					int idx = htmlContent.Find(tag, 0);
					int idx2 = htmlContent.Find(("\""), idx + tag.GetLength());
					CStringA redirURL = htmlContent.Mid(idx + tag.GetLength(), idx2 - idx - tag.GetLength());
					CString newURL = CA2T(redirURL.GetString());
					downloadOK = HttpsDownload(newURL, targetFilePath, internetSession);
				}
				else if (dwStatus >= 200 && dwStatus < 300)
				{
					CStdioFile csf;
					csf.Open(targetFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

					char buf[4096] = { 0 };
					int n = 0;
					while ((n = httpFile->Read(buf, 4096)) > 0)
					{
						csf.Write(buf, n);
					}
					csf.Close();

					httpFile->Close();
					delete httpFile;
					httpFile = NULL;
					downloadOK = true;
				}
			}
		} // if httpCon
	}
	catch (CInternetException*
#ifdef _DEBUG
		e
#endif
		)
	{
#ifdef _DEBUG
		e->ReportError();
#endif
		downloadOK = false;
	}

	if (httpConn)
	{
		httpConn->Close();
		delete httpConn;
		httpConn = NULL;
	}

	if (!sess && internetSession)
	{
		internetSession->Close();
		delete internetSession;
		internetSession = NULL;
	}

	return downloadOK;
}


bool CFixClientDlg::PatchPack(const char* pSrcDir, const char* pDeltaDir, const char* pSubDir)
{
	std::string strSrcDir(pSrcDir);
	strSrcDir += pSubDir;

	std::string strDeltaDir(pDeltaDir);
	strDeltaDir += pSubDir;

	const char* pSubDirWithoutFirstSlash = NULL;
	unsigned int nSubDirLen = 0;

	if ((*pSubDir) != 0)
	{
		pSubDirWithoutFirstSlash = pSubDir + 1;
		nSubDirLen = strlen(pSubDirWithoutFirstSlash) + 1;
	}

	// 1.listing delta file
	std::vector<PathString> vecDeltaFiles;
	EnumFilesInDirectory(vecDeltaFiles, strDeltaDir.c_str());

	std::vector<PathString> vecPackFiles;
	EnumFilesInDirectory(vecPackFiles, strSrcDir.c_str(), PAK_EXT_WITH_POINT);

	// 2.extract the src file from mpq
	_progressCount = 0;
	std::vector<PakFileIndex> vecPakFileIndex(vecDeltaFiles.size());

	unsigned int nDeltaDirLen = strDeltaDir.length();

	std::string strSrcTempDir(strSrcDir);
	strSrcTempDir += "\\Temp";

	CreateDirectoryHierarchy(strSrcTempDir.c_str(), true);

	ClientPatch::ScopedCurrentDirectory<WCHAR> currentDirectory;
	WCHAR szSrcDirTemp[MAX_PATH];
	if (!ClientPatch::ToWide(strSrcTempDir.c_str(), szSrcDirTemp) || !currentDirectory.Change(szSrcDirTemp))
		return false;

	for (unsigned int i = 0; i < vecDeltaFiles.size(); ++i)
	{
		PathString& strDeltaFilePath = vecDeltaFiles[i];
		PathString strFilePath = std::string(strDeltaFilePath.c_str() + nDeltaDirLen + 1, strDeltaFilePath.length() - nDeltaDirLen - 1);
		
		PathString strTempFilePath = strSrcTempDir + "\\" + strFilePath;
		PathString strTempFileDir = strTempFilePath.substr(0, strTempFilePath.find_last_of('\\') + 1);
		if (!IsDirExist(strTempFileDir))
		{
			MakeSureDirectoryPathExists(strTempFileDir);
		}

		WCHAR szSrcPath[MAX_PATH];
		if (!ClientPatch::ToWide(strSrcDir.c_str(), szSrcPath))
			return false;

		WCHAR szTempPath[MAX_PATH];
		if (!ClientPatch::ToWide(strSrcTempDir.c_str(), szTempPath))
			return false;

		WCHAR szDeltaFilePath[MAX_PATH];
		if (!ClientPatch::ToWide(strDeltaFilePath.c_str(), szDeltaFilePath))
			return false;

		WCHAR szFilePath[MAX_PATH];
		if (!ClientPatch::ToWide(strFilePath.c_str(), szFilePath))
			return false;

		WCHAR szTempFilePath[MAX_PATH];
		if (!ClientPatch::ToWide(strTempFilePath.c_str(), szTempFilePath))
			return false;

		int iPakIndex = -1;
		bool bSucc = (*g_pfExtractPackFile)(szSrcPath, szTempPath, szFilePath, iPakIndex);
		if (bSucc)
		{
			_statusCurrent = _T("解压文件:");
		}
		else
		{
			_statusCurrent = _T("解压文件失败: ");
		}
		_statusCurrent += szFilePath;
		_statusCurrent += _T(" 到 ");
		_statusCurrent += szTempFilePath;
		M_Update_Msg(TRUE);

		if (IsFileExist(strTempFilePath))
		{// 去除文件的只读属性， 否则会拷贝失败
			DWORD attr = ::GetFileAttributes((LPCTSTR)szTempFilePath);
			attr &= ~FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes((LPCTSTR)szTempFilePath, attr);
		}

		bSucc = ::CopyFile(szDeltaFilePath, szTempFilePath, FALSE) ? true : false;
		if (bSucc)
		{
			_statusCurrent = _T("拷贝文件: ");
		}
		else
		{
			_statusCurrent = _T("拷贝文件失败: ");
		}

		_statusCurrent += szDeltaFilePath;
		_statusCurrent += _T(" 到 ");
		_statusCurrent += szTempFilePath;
		M_Update_Msg(TRUE);

		_progressCurrent.SetPos(++_progressCount);
		UpdateData(FALSE);

		vecPakFileIndex[i] = PakFileIndex(iPakIndex, i);
	}

	// 3.add all file into mpq(include added)
	_progressCount = 0;
	std::sort(vecPakFileIndex.begin(), vecPakFileIndex.end());
	for (unsigned int i = 0; i < vecPakFileIndex.size(); ++i)
	{
		const PakFileIndex& fileIndex = vecPakFileIndex[i];
		check(fileIndex.iFileIndex < vecDeltaFiles.size());

		PathString& strDeltaFile = vecDeltaFiles[fileIndex.iFileIndex];
		PathString strFilePath = std::string(strDeltaFile.c_str() + nDeltaDirLen + 1, strDeltaFile.length() - nDeltaDirLen - 1);
		
		std::string strPackPath;
		WCHAR szPackPath[MAX_PATH];
		WCHAR szFilePath[MAX_PATH];
		if (!ClientPatch::ToWide(strFilePath.c_str(), szFilePath))
			return false;

		PathString strPakDir = strSrcDir + PathString("\\");

		bool bSucc = false;
		int iPakIndex = fileIndex.iPakIndex;
		// open archive
		if (iPakIndex == -1)
		{
			DWORD dwNewFileCount = (DWORD)(vecPakFileIndex.size() - i);
			if (vecPackFiles.size() > 0)
			{
				if (GetFileSize(vecPackFiles.back()) > PAK_SIZE_THRESHOLD)
				{
					char szPakName[MAX_PATH];
					sprintf_s(szPakName, sizeof(szPakName), PAK_FULLNAME_FORMAT, vecPackFiles.size());

					strPakDir += szPakName;
					vecPackFiles.push_back(strPakDir);

					strPackPath = strPakDir;
					if (!ClientPatch::ToWide(strPackPath.c_str(), szPackPath))
						return false;

					bSucc = (*g_pfAddPackFile)(szPackPath, szFilePath, true, dwNewFileCount);

				}
				else
				{
					strPackPath = vecPackFiles.back();
					if (!ClientPatch::ToWide(strPackPath.c_str(), szPackPath))
						return false;

					bSucc = (*g_pfAddPackFile)(szPackPath, szFilePath, false, dwNewFileCount);
				}
			}
			else
			{
				char szPakName[MAX_PATH];
				sprintf_s(szPakName, sizeof(szPakName), PAK_FULLNAME_FORMAT, vecPackFiles.size());

				strPakDir += szPakName;
				vecPackFiles.push_back(strPakDir);

				strPackPath = strPakDir;
				if (!ClientPatch::ToWide(strPackPath.c_str(), szPackPath))
					return false;

				bSucc = (*g_pfAddPackFile)(szPackPath, szFilePath, true, dwNewFileCount);
			}
		}
		else
		{
			strPackPath = vecPackFiles[iPakIndex];
			if (!ClientPatch::ToWide(strPackPath.c_str(), szPackPath))
				return false;

			bSucc = (*g_pfAddPackFile)(szPackPath, szFilePath, false, 0);
		}

		if (bSucc)
		{
			_statusCurrent = _T("打包文件: ");
		}
		else
		{
			_statusCurrent = _T("打包文件失败: ");
		}

		_statusCurrent += szFilePath;
		_statusCurrent += _T(" 到 ");
		_statusCurrent += szPackPath;

		_progressCurrent.SetPos(++_progressCount);
		M_Update_Msg(TRUE);
	}

	//DeleteFileTree(strSrcDirTemp);
	DeleteFilesInDirectory(strSrcTempDir.c_str(), true);

	currentDirectory.Restore();

	return true;
}

bool CFixClientDlg::PatchFile(const char* pSrcFile, const char* pDeltaFile)
{
	PathString strSrcFile(pSrcFile);
	PathString strDeltaFile(pDeltaFile);

	PathString strFileDir = strSrcFile.substr(0, strSrcFile.find_last_of('\\') + 1);
	if (!IsDirExist(strFileDir))
	{
		MakeSureDirectoryPathExists(strFileDir);
	}

	// merge common file
	WCHAR szDeltaDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strDeltaFile.c_str(), -1, szDeltaDir, (int)(sizeof(szDeltaDir) / sizeof(szDeltaDir[0]))))
		return false;

	WCHAR szSrcDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strSrcFile.c_str(), -1, szSrcDir, (int)(sizeof(szSrcDir) / sizeof(szSrcDir[0]))))
		return false;

	if (IsFileExist(strSrcFile))
	{// 去除文件的只读属性， 否则会拷贝失败
		DWORD attr = ::GetFileAttributes((LPCTSTR)szSrcDir);
		attr &= ~FILE_ATTRIBUTE_READONLY;
		::SetFileAttributes((LPCTSTR)szSrcDir, attr);
	}

	// 不能自己修复自己，否则会报错
	PathString strFileName = strDeltaFile.substr(strDeltaFile.find_last_of('\\') + 1, strDeltaFile.length() - strDeltaFile.find_last_of('\\'));
	if (strFileName == PathString("FixClient.exe") ||
		strFileName == PathString("FixClient.ini") ||
		strFileName == PathString("XDelta3WrapFactory.dll") || 
		strFileName == PathString("XDelta3WrapFactory2.dll"))
	{
		M_Update_Msg(TRUE);
		return true;
	}

	bool bSucc = CopyFile(szDeltaDir, szSrcDir, FALSE) ? true : false;

	_statusCurrent = _T("拷贝文件: ");
	_statusCurrent += szDeltaDir;
	_statusCurrent += _T(" 到 ");
	_statusCurrent += szSrcDir;

	M_Update_Msg(TRUE);
	return bSucc;
}

void CFixClientDlg::InitPatchDir()
{
	_nonePackSubDirList.clear();
	_nonePackSubDirList.push_back("Binaries");
	_nonePackSubDirList.push_back("Engine");
	_nonePackSubDirList.push_back("history");
	_nonePackSubDirList.push_back("SDO");
	_nonePackSubDirList.push_back("MMOGame\\Account");
	_nonePackSubDirList.push_back("MMOGame\\Content");
	_nonePackSubDirList.push_back("MMOGame\\Logs");
	_nonePackSubDirList.push_back("MMOGame\\Splash");
	_nonePackSubDirList.push_back("MMOGame\\Movies");

	_packSubDirList.clear();
	_packSubDirList.push_back("MMOGame\\Config");
	_packSubDirList.push_back("MMOGame\\Localization");
	_packSubDirList.push_back("MMOGame\\CookedPC");
}

eSubDirType CFixClientDlg::GetSubDirType(const char* pSubDir)
{
	for (unsigned int i = 0; i < _packSubDirList.size(); ++i)
	{
		const PathString& PakSubDir = _packSubDirList[i];

		if (strcmp(PakSubDir, pSubDir) == 0)
		{
			return ESDT_PACK;
		}

		if (strstr(PakSubDir, pSubDir) != NULL)
		{
			return ESDT_NOT_SURE;
		}
	}

	return ESDT_NOT_PACK;
}

bool CFixClientDlg::Patch(const char* pszSrcPath, const char* pszDeltaPath, const char* pszShareSubDir)
{
	WIN32_FIND_DATA fd;

	CString strFindDir(pszDeltaPath);
	strFindDir += pszShareSubDir;
	strFindDir += _T("\\*.*");

	bool bSucc = true;
	HANDLE Handle = FindFirstFile(strFindDir, &fd);
	if (Handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd.cFileName[0] != '.')
			{
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					PathString strSubDir(pszShareSubDir);
					strSubDir += "\\";
					strSubDir += PathString(fd.cFileName);

					eSubDirType e = GetSubDirType(strSubDir.c_str() + 1);
					switch (e)
					{
					case ESDT_NOT_PACK:
					case ESDT_NOT_SURE:
						bSucc = bSucc && Patch(pszSrcPath, pszDeltaPath, strSubDir.c_str());
						break;
					case ESDT_PACK:
						bSucc = bSucc && PatchPack(pszSrcPath, pszDeltaPath, strSubDir.c_str());
						break;
					}
				}
				else
				{
					PathString strSubDir(pszShareSubDir);
					strSubDir += "\\";
					strSubDir += PathString(fd.cFileName);

					eSubDirType e = GetSubDirType(strSubDir.c_str() + 1);
					switch (e)
					{
					case ESDT_NOT_PACK:
					case ESDT_NOT_SURE:
						{
							PathString strSrcDir(pszSrcPath);
							PathString strDeltaDir(pszDeltaPath);

							strSrcDir += strSubDir;
							strDeltaDir += strSubDir;

							bSucc = bSucc & PatchFile(strSrcDir.c_str(), strDeltaDir.c_str());
						}
						break;
					case ESDT_PACK:
						// 如果是包内文件, 交给PatchDir，这里不处理
						break;
					}
				}
			}
		} while (FindNextFile(Handle, &fd) && bSucc);

		FindClose(Handle);
	}

	return bSucc;
}
