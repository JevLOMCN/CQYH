/*====================================================================
	Created Date  :		2005-8-24
	
	Specification :  
====================================================================*/
#include "stdafx.h"
#include "../ReadFileBuffer.h"
#include "../PatchPath.h"
#include "../DownloadBuffer.h"
#include "FilePatch.h"
#include "Filepatchdlg.h"
#include "Controls.h"
#include "MD5.h"
#include "App.h"
#include "Configer.h"
#include "File.h"
#include "Task.h"
#include "ThreadPool.h"
#include <assert.h>
#include <sstream>

#ifdef _DEBUG
#	pragma comment(lib, "zlibwapid.lib")
#else
#	pragma comment(lib, "zlibwapi.lib")
#endif

#define M_USE_MT_UPDATE 1

using namespace std;
using namespace ShowGame_Core;

#define M_BEGIN_PATCH	WM_USER + 1
#define M_BEGIN_PATCH_GAME	WM_USER + 2
#define M_BEGIN_PATCH_EDITOR	WM_USER + 3
#define M_BEGIN_PATCH_QAGAME	WM_USER + 4
#define M_BEGIN_PATCH_QAEDITOR	WM_USER + 5

const UINT C_TimerID = 88888;

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



void CFilePatchDlg::WaitForAllWorks( class CThreadPool* thrdPool, const CString& msg /*= _T("工作线程数量:%d")*/ )
{
	bool allComplete = false;
	while(!allComplete)
	{
		allComplete = thrdPool->IsAllTaskCompleted();

		m_strStatusDetail.Format((LPCTSTR)msg, thrdPool->GetWorkingThreadCount());
		M_Update_Msg(true);

		::Sleep(100);
	}
}

CFilePatchDlg::CFilePatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilePatchDlg::IDD, pParent)
	, m_strStatusDetail(_T("N/A"))
    , m_strTotal(_T("N/A"))
    , m_strCurrent(_T("N/A"))
	, _bPatchSuccess(FALSE)
	, _bShouldPatchSelf(FALSE)
	, _countDownTime(0)
	, _strInfoAutoStart(_T(""))
	, _useEngLang(0)
	, _serverPort(INTERNET_INVALID_PORT_NUMBER)
	, _pHttpConnection(0)
	, _InternetSession(0)
	, _forceFix(TRUE)
	,_verCheckThread(0)
	,_checkThreadExit(TRUE)
	,_nextVerCheckTime(0)
	, _use64Bit(FALSE)
	, _iTotalUpdate(0)
	, _iCurUpdate(0)
	, _autoStartTime(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	InitializeCriticalSection(&_csCheckThread);
}

void CFilePatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATUS_DETAIL, m_strStatusDetail);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressAll);
	DDX_Control(pDX, IDC_PROGRESS_FILE, m_ProgressCurrent);
	DDX_Control(pDX, IDC_MIN, _btnMin);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, _btnClose);
	//DDX_Control(pDX, IDC_DHTMLEDIT, _htmlADBar);
	DDX_Text(pDX, IDC_TOTAL, m_strTotal);
	DDX_Text(pDX, IDC_CURRENT, m_strCurrent);
	DDX_Control(pDX, IDC_CURRENT, _infoCurrent);
	DDX_Control(pDX, IDC_TOTAL, _infoTotal);
	DDX_Control(pDX, IDC_STATUS_DETAIL, _infoDetail);
	DDX_Control(pDX, IDBT_STARTGAME, _btStart);
	DDX_Control(pDX, IDBT_STARTEDITOR, _btStartEditor);

	DDX_Control(pDX, IDBT_UPDATE_GAME, _btUpdateGame);
	DDX_Control(pDX, IDBT_UPDATE_EDITOR, _btUpdateEditor);
	DDX_Control(pDX, IDBT_UPDATE_QAGAME, _btUpdateQAGame);
	DDX_Control(pDX, IDBT_UPDATE_QAED, _btUpdateQAEditor);

	DDX_Control(pDX, IDBT_CANCEL, _btCancel);
	DDX_Control(pDX, IDC_AUTOSTART, _infoAutoStart);
	DDX_Text(pDX, IDC_AUTOSTART, _strInfoAutoStart);

	DDX_Check(pDX, IDC_CHECK_Eng, _useEngLang);
	DDX_Check(pDX, IDC_CHECK_FIX, _forceFix);
	DDX_Control(pDX, IDBT_UPDATE_RELEASE_GAME, _btnUpdateReleaseGame);
	DDX_Control(pDX, IDBT_START_RELEASE_GAME, _btnStartReleaseGame);
	
	DDX_Check(pDX, IDC_CHECK_64BIT, _use64Bit);
}

BEGIN_MESSAGE_MAP(CFilePatchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnCancel)
	ON_BN_CLICKED(IDC_MIN, OnBnClickedMin)
	ON_MESSAGE(M_BEGIN_PATCH, OnBeginPatch)
	ON_MESSAGE(M_BEGIN_PATCH_GAME, OnBeginPatchGame)
	ON_MESSAGE(M_BEGIN_PATCH_EDITOR, OnBeginPatchEditor)
	ON_MESSAGE(M_BEGIN_PATCH_QAGAME, OnBeginPatchQAGame)
	ON_MESSAGE(M_BEGIN_PATCH_QAEDITOR, OnBeginPatchQAEditor)
	//ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDBT_STARTGAME, &CFilePatchDlg::OnBnClickedStartgame)
	ON_BN_CLICKED(IDBT_STARTEDITOR, &CFilePatchDlg::OnBnClickedStartEditor)
	ON_BN_CLICKED(IDBT_CANCEL, &CFilePatchDlg::OnBnClickedCancel)

	ON_BN_CLICKED(IDBT_UPDATE_GAME, &CFilePatchDlg::OnBnClickedUpdateGame)
	ON_BN_CLICKED(IDBT_UPDATE_EDITOR, &CFilePatchDlg::OnBnClickedUpdateEditor)

	ON_BN_CLICKED(IDBT_UPDATE_QAGAME, &CFilePatchDlg::OnBnClickedUpdateQAGame)
	ON_BN_CLICKED(IDBT_UPDATE_QAED, &CFilePatchDlg::OnBnClickedUpdateQAEditor)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDBT_UPDATE_RELEASE_GAME, &CFilePatchDlg::OnBnClickedUpdateReleaseGame)
	ON_BN_CLICKED(IDBT_START_RELEASE_GAME, &CFilePatchDlg::OnBnClickedStartReleaseGame)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDBT_REF_VERSION, &CFilePatchDlg::OnBnClickedRefVersion)
	ON_BN_CLICKED(IDBT_UPDATE_SHADER_CACHE, &CFilePatchDlg::OnBnClickedUpdateShaderCache)
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

BOOL CFilePatchDlg::OnInitDialog(){
	CDialog::OnInitDialog();

	//std::wcout.imbue(std::locale("chs"));
	setlocale(LC_ALL, "");

	ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	static CString iniFile = App::GetAppPath() + C_IniFile;
	
	static Configer cfg;
	cfg.LoadFile(iniFile);
	_autoStartTime = cfg.GetIntValue(_T("Time"), _T("AutoStart"), 20);	

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

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetWindowText(_T("自动更新"));

	_btnMin.Set4In1Bitmap(IDB_MIN);
	_btnMin.SetDrawTrans(TRUE);

	_btnClose.Set4In1Bitmap(IDC_BUTTON_CLOSE);
	_btnClose.SetDrawTrans(TRUE);

	
	//_btStart.Set4In1Bitmap(IDB_START);
	//_btStart.SetDrawTrans(TRUE);

	//_btCancel.Set4In1Bitmap(IDB_CANCEL);
	//_btCancel.SetDrawTrans(TRUE);

	_BmpBk.LoadBitmap(IDB_BK);
	CRgn rgn;
	CClientDC dc(this);
	SIZE sz = CU_GetBitmapSize(_BmpBk);
	MoveWindow(0, 0, sz.cx, sz.cy, FALSE);

	// 通过配置文件读取布局
	MoveControlToRect(m_ProgressCurrent, _T("CurrentProg"));
	MoveControlToRect(m_ProgressAll, _T("AllProg"));
	//MoveControlToRect(_htmlADBar, _T("HtmlAD"));

	TCHAR strBuffer[MAX_PATH];
	if (!ClientPatch::ReadModulePath(strBuffer))
		return FALSE;
	CString strTmp = strBuffer;
	_strPatchConfigFile = strTmp.Left( strTmp.ReverseFind('\\') + 1 );
	_strVersionLocalConfigFile = _strPatchConfigFile;
	_strPatchConfigFile += C_IniFile;
	_strVersionLocalConfigFile += C_VerLocalIniFile;

	_UpdateADImage(_strPatchConfigFile);

	//_htmlADBar.SetAdURL(_strAdURL, _strAdImg);

	MoveControlToRect(_btStart, _T("BtStart"));
	MoveControlToRect(_btCancel, _T("BtCancel"));

	MoveControlToRect(_btnMin, _T("BtClose"));
	MoveControlToRect(_btnClose, _T("BtMin"));

	MoveControlToRect(_infoDetail, _T("InfoDetail"));
	MoveControlToRect(_infoCurrent, _T("InfoCurrent"));
	MoveControlToRect(_infoTotal, _T("InfoTotal"));
	MoveControlToRect(_infoAutoStart, _T("InfoAutoStart"));

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

	rc = GetControlRect(_T("InfoAutoStart"));
	//_infoAutoStart.SetBgImage(&_BmpBk, rc);
	_infoAutoStart.SetTextColor(RGB(255, 255, 255));
	_infoAutoStart.SetTextAlignFormat(DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	CU_SetupRegion(rgn, &dc, _BmpBk, RGB(255, 0, 255));
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

	m_ProgressAll.SetStep(1);
	m_ProgressCurrent.SetStep(1);

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

	UINT id;
	_checkThreadExit = FALSE;
	_verCheckThread = (HANDLE)_beginthreadex(0,0,VerCheckThread,this,0,&id);

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CFilePatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFilePatchDlg::OnPaint() 
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
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CFilePatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFilePatchDlg::OnBnClickedMin()
{
	ShowWindow(SW_MINIMIZE);
}

void CFilePatchDlg::OnCancel()
{
	EnterCriticalSection(&_csCheckThread);
	_patchVersion.Empty();
	LeaveCriticalSection(&_csCheckThread);

	if(_bPatchSuccess)
	{
		CDialog::OnCancel();
		return;
	}

// 	if( MessageBox(_T("取消更新将导致游戏不能运行！确定取消吗？")
// 		, _T("警告！")
// 		, MB_YESNO | MB_ICONQUESTION) == IDYES
// 		)
	{
		CDialog::OnCancel();
	}
}
//查找本地所有.vi2文件，读取信息
void CFilePatchDlg::CheckLocalInfo()
{
	_strLog += _T("//-----------------------------------------------------------------\n");
	_strLog += _T("CheckLocalInfo...\n");

	SFilePatch	filePatch;

	CFileFind finder;
	_vectorFilePatches.clear();

#if 1
	TCHAR strBuffer[MAX_PATH];
	::GetPrivateProfileString(_serverSection
		, _T("ViFile"), _T("")
		, strBuffer, MAX_PATH
		, _strPatchConfigFile
		);

	CString viFile = strBuffer;
// 	filePatch._strLocalViFilePathName	= _strInstallDir + viFile;//finder.GetFilePath();
// 	filePatch._strViFileName			= viFile;
// 	filePatch._strServerSubDir			= "";
// 	filePatch._strServerFileSuffix		= viFile;
// 	_vectorFilePatches.push_back(filePatch);

	viFile += "2";
	filePatch._strLocalViFilePathName	= _strInstallDir + viFile;//finder.GetFilePath();
	filePatch._strViFileName			= viFile;
	filePatch._strServerSubDir			= "";
	filePatch._strServerFileSuffix		= viFile;
	_vectorFilePatches.push_back(filePatch);

#else
	// Find .vi2 first
	CString strViFile = _strInstallDir + _T("*.vi2");
	BOOL bWorking = finder.FindFile(strViFile);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		filePatch._strLocalViFilePathName	= finder.GetFilePath();
		filePatch._strViFileName			= finder.GetFileName();
		filePatch._strServerSubDir			= finder.GetFileTitle() + _T("/");
		filePatch._strServerFileSuffix		= /*filePatch._strServerSubDir + */finder.GetFileName();

		_vectorFilePatches.push_back(filePatch);

		_strLog += _T("Find Installed Component : ");
		_strLog += filePatch._strViFileName;
		_strLog += _T("\n");
	}
	
	// Find .vi second
	strViFile = _strInstallDir + _T("*.vi");
	bWorking = finder.FindFile(strViFile);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		BOOL bV2Exist = FALSE;
		for(size_t t = 0; t < _vectorFilePatches.size(); ++t)
		{
			CString strCompare = finder.GetFileTitle() + _T("/");
			if(strCompare == _vectorFilePatches[t]._strServerSubDir)
			{
				bV2Exist = TRUE;
				break;
			}
		}

		if(!bV2Exist)
		{
			filePatch._strLocalViFilePathName	= finder.GetFilePath();
			filePatch._strViFileName			= finder.GetFileTitle() + _T(".vi2");
			filePatch._strServerSubDir			= finder.GetFileTitle() + _T("/");
			filePatch._strServerFileSuffix		= /*filePatch._strServerSubDir + */filePatch._strViFileName;

			_vectorFilePatches.push_back(filePatch);

			_strLog += _T("Find Installed Component : ");
			_strLog += filePatch._strViFileName;
			_strLog += _T("\n");
		}
	}
#endif

	for(size_t t = 0; t < _vectorFilePatches.size(); ++t)
	{
		LoadLocalViInfo(_vectorFilePatches[t]);
	}

	if(_vectorFilePatches.size() == 0)
		_strLog += _T("None package is Found! Make sure you have installed Game.\n");
}


void CFilePatchDlg::LoadLocalViInfo(SFilePatch& filePatch)
{
	_strLog += _T("//-----------------------------------------------------------------\n");
	_strLog += _T("LoadLocalViInfo : ") + filePatch._strViFileName;
	_strLog += _T("\n");

	CString viFile = filePatch._strLocalViFilePathName;
	
	if(!_LoadLocalViInfo(viFile, filePatch))
	{
		CString targFile = _strTmpDir + filePatch._strViFileName;
		if(DownLoadServerViFile(filePatch, targFile))
		{
			_LoadLocalViInfo(targFile, filePatch, true);
		}
	}


	g_genMD5TaskTotalCount = 0;(LONG)filePatch._vectorLocalViInfoes.size();
	g_genMD5TaskCompletedCount = 0;

	// Repair function, check local file exist
	for(size_t t = 0; t < filePatch._vectorLocalViInfoes.size(); ++t)
	{
		if( _T("Dir") == filePatch._vectorLocalViInfoes[t]._contentType)
		{
			filePatch._vectorLocalViInfoes[t]._codeMD5 = _T("None");
			continue;
		}

		CFileStatus rStatus;
		if( CFile::GetStatus(filePatch._vectorLocalViInfoes[t]._relatedPathName, rStatus ) )
		{

			// Recalculate local file md5 string(for local file maybe modified by user)
			if(_forceFix)
			{
				m_strStatusDetail.Format(_T("正在检查%s..."), (LPCTSTR)filePatch._vectorLocalViInfoes[t]._relatedPathName);
				M_Update_Msg(true);

#if M_USE_MT_UPDATE
				InterlockedIncrement(&g_genMD5TaskTotalCount);
				GenerateMD5Task* task = new GenerateMD5Task(t, filePatch._vectorLocalViInfoes[t]._relatedPathName,
					filePatch._vectorLocalViInfoes[t]._codeMD5);
				g_genMD5ThreadPool->Run(task);
#else
				FILE* pFile = 0;
				_tfopen_s(&pFile, (LPCTSTR)filePatch._vectorLocalViInfoes[t]._relatedPathName, _T("rb"));
				ASSERT(pFile != NULL);
				MD5 md5(pFile);
				filePatch._vectorLocalViInfoes[t]._codeMD5 = md5.hex_digest();
#endif
			}
		}
		else
		{
			// local File or Dir missing
			filePatch._vectorLocalViInfoes[t]._codeMD5 = _T("Missing");

			_strLog += _T("Missing Local File : ");
			_strLog += filePatch._vectorLocalViInfoes[t]._relatedPathName;
			_strLog += _T("\n");
		}
	}

#if M_USE_MT_UPDATE
	//WaitForAllWorks(g_genMD5ThreadPool, _T("MD5生成工作线程数量:%d"));
	m_ProgressCurrent.SetRange(0, (short)g_genMD5TaskTotalCount);
	
	bool allComplete = false;
	while(!allComplete)
	{
		allComplete = g_genMD5TaskCompletedCount == g_genMD5TaskTotalCount;//g_genMD5ThreadPool->IsAllTaskCompleted();

		m_strStatusDetail.Format(_T("生成MD5:%d/%d 线程数:%d/%d"),
			g_genMD5TaskCompletedCount, g_genMD5TaskTotalCount,
			g_genMD5ThreadPool->GetWorkingThreadCount(),
			g_genMD5ThreadPool->GetPoolSize());

		m_ProgressCurrent.SetPos(g_genMD5TaskCompletedCount);

		M_Update_Msg(true);

		::Sleep(100);
	}
#endif
}

LRESULT CFilePatchDlg::OnBeginPatch(WPARAM wParam, LPARAM lParam)
{
	EnterCriticalSection(&_csCheckThread);
	_patchVersion.Empty();
	if (!DownFileToString(_serverSection, _patchVersion))
		_patchVersion.Empty();
	LeaveCriticalSection(&_csCheckThread);

	try
	{
		_btUpdateGame.EnableWindow(FALSE);
		_btUpdateEditor.EnableWindow(FALSE);

		_btUpdateQAGame.EnableWindow(FALSE);
		_btUpdateQAEditor.EnableWindow(FALSE);

        m_ProgressCurrent.SetPos(0);
        m_ProgressAll.SetPos(0);
        m_strStatusDetail = _T("检查本地版本中...");
        m_strTotal = _T("");
        m_strCurrent = _T("正在检查文件，等待更新...");

		//_vectorPatchInstalls.clear();

		//连接Patch服务器
		//_InternetSession.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5);
		//_pHttpConnection = _InternetSession.GetHttpConnection(_strServer, _serverPort);

		_startGenMD5Time = CTime::GetCurrentTime();
		CheckLocalInfo();
		_endGenMD5Time = CTime::GetCurrentTime();

		_strLog += _T("//-----------------------------------------------------------------\n");
		_strLog += _T("连接FilePatch服务器：") + _strInitRoot + _T(" on ") + _strServer;
		_strLog += _T("\n");

		m_strStatusDetail = _T("正在获取更新文件列表...");
		M_Update_Msg(TRUE);

		_iTotalUpdate = 0;
		for(size_t t = 0; t < _vectorFilePatches.size(); ++t)
		{
			_strLog += _T("//-----------------------------------------------------------------\n正在获取更新文件列表...\n");
			_strLog += _vectorFilePatches[t]._strViFileName + _T("...\n");

			if( DownLoadServerViFile(_vectorFilePatches[t]) 
				&&
				LoadServerViInfo(_vectorFilePatches[t])
				)
			{
				_startCompareMD5Time = CTime::GetCurrentTime();
				Compare(_vectorFilePatches[t]);
				_endCompareMD5Time = CTime::GetCurrentTime();

				_iTotalUpdate += _vectorFilePatches[t]._iPatchCount;
			}
			else
			{
				_vectorFilePatches[t]._bCheckServerViSuccess = FALSE;

				_strLog += _T("DownLoadServerViFile : Failed for ");
				_strLog += _vectorFilePatches[t]._strViFileName;
				_strLog += _T("\n");
			}
		}
		m_ProgressAll.SetRange(0, (short)_iTotalUpdate);
		m_ProgressAll.SetPos(0);
		_iCurUpdate = 0;
		m_strTotal.Format(_T("%d"), _iTotalUpdate);

		_strLog += _T("//-----------------------------------------------------------------\n需要更新数目：");
		_strLog += m_strTotal;
		_strLog += _T("\n");

		_startDownloadTime = CTime::GetCurrentTime();

		
		for(size_t t = 0; _iTotalUpdate > 0 && t < _vectorFilePatches.size(); ++t)
		{
			m_strStatusDetail = _T("正在准备更新任务：");
			m_strStatusDetail += _vectorFilePatches[t]._strViFileName + _T("...");
			M_Update_Msg(TRUE);

			_strLog += _T("//-----------------------------------------------------------------\n");
			_strLog += m_strStatusDetail;
			_strLog += _T("\n");

			DoPatch(_vectorFilePatches[t]);
		}

        //assert(_iCurUpdate == _iTotalUpdate);

		_strLog += _T("//-----------------------------------------------------------------\n");
		m_strStatusDetail = _T("正在更新更新记录...");
		M_Update_Msg(TRUE);
		DoPatchVi();

		_strLog += _T("//-----------------------------------------------------------------\n");
		if(_vectorPatchInstalls.size() > 0)
		{
			m_strStatusDetail = _T("正在更新其它程序...");
			M_Update_Msg(TRUE);

			for(size_t pt = 0; pt < _vectorPatchInstalls.size(); ++pt)
			{
				_strLog +=  _T("Installing : ") + _vectorPatchInstalls[pt] + _T("\n");
				StartProcess(_vectorPatchInstalls[pt]);
			}
		}
		_strLog += _T("//-----------------------------------------------------------------\n");
	}
	catch(...)
	{
	}

	
	m_strStatusDetail = _T("更新完成，准备启动游戏...");
    m_strCurrent = _T("OK");
    m_strTotal = _T("OK");
    m_ProgressAll.SetRange(0,100);
    m_ProgressAll.SetPos(100);
    m_ProgressCurrent.SetPos(0);
	M_Update_Msg(TRUE);

	_bPatchSuccess = TRUE;
	
	if (!_patchVersion.IsEmpty())
	{
		if (_serverSection.Find(_T("Editor")) >= 0)
			WritePrivateProfileString(_T("EDIT"), _T("VERSION"), _patchVersion, _strVersionLocalConfigFile);
		else if (_serverSection.Find(_T("Shipping")) >= 0)
			WritePrivateProfileString(_T("RELEASE"), _T("VERSION"), _patchVersion, _strVersionLocalConfigFile);
		else
			WritePrivateProfileString(_T("GAME"), _T("VERSION"), _patchVersion, _strVersionLocalConfigFile);
		OnBnClickedRefVersion();
	}

	PatchOK();

	_endDownloadTime = CTime::GetCurrentTime();

	//CDialog::OnOK();

	CTimeSpan chkTm = _endGenMD5Time - _startGenMD5Time;
	CTimeSpan cmpTm = _endCompareMD5Time - _startCompareMD5Time;
	CTimeSpan dowloadTm = _endDownloadTime - _startDownloadTime;

	m_strStatusDetail.Format(_T("生成MD5:%d 比较MD5:%d 下载:%d"), chkTm.GetTotalSeconds(), cmpTm.GetTotalSeconds(), dowloadTm.GetTotalSeconds());
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

	return 0;
}

LRESULT CFilePatchDlg::OnBeginPatchGame(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedUpdateGame();
	return  0;
}

LRESULT CFilePatchDlg::OnBeginPatchEditor(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedUpdateEditor();
	return  0;
}

LRESULT CFilePatchDlg::OnBeginPatchQAGame(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedUpdateQAGame();
	return 0;
}

LRESULT CFilePatchDlg::OnBeginPatchQAEditor(WPARAM wParam, LPARAM lParam)
{
	OnBnClickedUpdateQAEditor();
	return 0;
}

BOOL CFilePatchDlg::DownLoadServerViFile(SFilePatch& filePatch, const CString& targetFileName)
{
	CString strFilePathName = _strInitRoot;	//_T("/PatchRoot/");
	strFilePathName += filePatch._strServerFileSuffix;

	CHttpFile *pFile = _pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET
		, strFilePathName 
		, NULL
		, 1
		, NULL
		, NULL
		, HSR_DOWNLOAD | INTERNET_FLAG_RELOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT
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
	}

	CStdioFile csf;
	CString targFile = targetFileName;
	if(targFile.IsEmpty())
	{
		targFile = _strTmpDir + filePatch._strViFileName;
	}

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
}

BOOL CFilePatchDlg::LoadServerViInfo(SFilePatch& filePatch)
{
	CString strFileName = _strTmpDir + filePatch._strViFileName;
	CStdioFile stdFile;
	if( stdFile.Open(strFileName, CFile::modeRead | CFile::typeText) )
	{
		SVersionInfo ViInfo;

		CString strTmp;
		while(stdFile.ReadString(strTmp))
		{
			int iPos1, iPos2;
			iPos1 = strTmp.Find(_T(','));
			ViInfo._relatedPathName = strTmp.Left(iPos1);

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
				filePatch._vectorServerViInfoes.push_back(ViInfo);
		}

		stdFile.Close();

		return (filePatch._vectorServerViInfoes.size() != 0);
	}
	else
	{
		return FALSE;
	}
}

void CFilePatchDlg::Compare(SFilePatch& filePatch)
{
#if M_USE_MT_UPDATE
	
	g_compareMD5TaskTotalCount = (LONG)filePatch._vectorLocalViInfoes.size();
	g_compareMD5TaskCompletedCount = 0;

	for(size_t t = 0; t < filePatch._vectorLocalViInfoes.size(); ++t)
	{
		CompareMD5Task* task = new CompareMD5Task(t, filePatch._vectorLocalViInfoes[t], filePatch);
		g_compareMD5ThreadPool->Run(task);
	}

	m_ProgressCurrent.SetRange(0, (short)g_compareMD5TaskTotalCount);

	//WaitForAllWorks(g_compareMD5ThreadPool, _T("比较MD5工作线程数量:%d"));	
	bool allComplete = false;
	while(!allComplete)
	{
		allComplete = g_compareMD5TaskCompletedCount == g_compareMD5TaskTotalCount;//g_compareMD5ThreadPool->IsAllTaskCompleted();

		m_strStatusDetail.Format(_T("比较MD5:%d/%d 线程数:%d/%d"),
			g_compareMD5TaskCompletedCount, g_compareMD5TaskTotalCount, 
			g_compareMD5ThreadPool->GetWorkingThreadCount(),
			g_compareMD5ThreadPool->GetPoolSize());

		m_ProgressCurrent.SetPos(g_compareMD5TaskCompletedCount);

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
				pInfoServer->_isMatched = TRUE;
				pInfoLocal->_isMatched = TRUE;

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
			info._codeMD5				= pInfoServer->_codeMD5;
			info._originLen			= pInfoServer->_originLen;
			info._contentType				= pInfoServer->_contentType;
			if(filePatch._strViFileName.CompareNoCase(_T("PatchInstall.vi2")) == 0)
			{
				info._iOperation = EPO_Install;	// Patch Install
			}
			else
			{
				info._iOperation = EPO_Patch;	// Patch
			}

			filePatch._iPatchCount++;
			filePatch._bShouldUpdateViFile = TRUE;

			filePatch._vectorLocalViInfoes.push_back(info);
		}
	}
}

void CFilePatchDlg::DoPatch(SFilePatch& filePatch)
{
// 	_vectorFilePatches.clear();
// 	_vectorPatchInstalls.clear();

	// Manage Dir first
	map<CString, int>	strDirMap;
	for(size_t t = 0; t < filePatch._vectorLocalViInfoes.size(); ++t)
	{
		SLocalVersionInfo* pInfoLocal = &(filePatch._vectorLocalViInfoes[t]);
		if(pInfoLocal->_contentType == _T("Dir"))
		{
			CString strTmp = _strInstallDir + pInfoLocal->_relatedPathName;
			strDirMap.insert(make_pair(strTmp, 0));

			pInfoLocal->_iOperation = EPO_None;
		}
	}
	for(map<CString, int>::iterator it = strDirMap.begin(); it != strDirMap.end(); ++it)
	{
		if( ::CreateDirectory(it->first, 0) )
		{
			_strLog += _T("CreateDirectory : ") + it->first;
			_strLog += _T("\n");
		}
	}


	g_downloadTaskTotalCount = 0;//(LONG)filePatch._vectorLocalViInfoes.size();
	g_downloadTaskCompletedCount = 0;
	g_writeFileTaskCompletedCount = 0;
		
	m_ProgressCurrent.SetRange(0, (short)_iTotalUpdate);

	for(size_t t = 0; t < filePatch._vectorLocalViInfoes.size(); ++t)
	{
		SLocalVersionInfo* pInfoLocal = &(filePatch._vectorLocalViInfoes[t]);

		BOOL bUpdateItem = FALSE;

		if(pInfoLocal->_iOperation == EPO_Patch		// Patch
			||
			pInfoLocal->_iOperation == EPO_Install	// Patch Install
			)
		{
			if(pInfoLocal->_contentType == _T("Dir"))
			{
				// 'Dir' has already managed above

				//bUpdateItem = TRUE;
			}
			else
			{
				if(pInfoLocal->_relatedPathName.Find(_T("VersionPatch.exe")) != -1)
				{
					_bShouldPatchSelf = TRUE;

					_strLog += _T("//-----------------------------------------------------------------\n");
					_strLog += _T("Should PatchSelf : ") + pInfoLocal->_relatedPathName;
					_strLog += _T("\n");
				}

				m_strCurrent = pInfoLocal->_relatedPathName;
				//m_ProgressCurrent.SetPos(0);
				M_Update_Msg(TRUE);

#if M_USE_MT_UPDATE
				InterlockedIncrement(&g_downloadTaskTotalCount);
				DownloadTask* task = new DownloadTask(t, _strServer, _serverPort, _strInitRoot,
					*pInfoLocal, _strInstallDir, _strTmpDir);
				g_downloadThreadPool->Run(task);
#else
				DownLoadFile(pInfoLocal, filePatch);
#endif
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
				CString strTmp = _strInstallDir + pInfoLocal->_relatedPathName;
				::DeleteFile(strTmp);
				bUpdateItem = TRUE;

				_strLog += _T("DeleteFile : ") + strTmp;
				_strLog += _T("\n");
			}
		}

		if(bUpdateItem)
		{
			_iCurUpdate ++;
            //m_ProgressAll.SetPos(_iCurUpdate);
            assert(_iCurUpdate <= _iTotalUpdate);
			m_strTotal.Format(_T("%d/%d"), _iCurUpdate, _iTotalUpdate);

			m_ProgressCurrent.SetPos(_iCurUpdate);

			M_Update_Msg(TRUE);
		}
	}

	//m_ProgressAll.StepIt();
}

BOOL CFilePatchDlg::DownLoadFile(SLocalVersionInfo* pInfo, SFilePatch& filePatch)
{
	CString strFilePathName = _strInitRoot;	//_T("/PatchRoot/");
	strFilePathName += /*filePatch._strServerSubDir + */pInfo->_relatedPathName;

	BOOL bCompressed = FALSE;
	if(pInfo->_originLen != 0)
	{
		bCompressed = TRUE;
		strFilePathName += _T(".zfp");
	}

	//strFilePathName.Replace(" ", "%20");

	CHttpFile *pFile = _pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET
		, strFilePathName 
		, NULL
		, 1
		, NULL
		, NULL
		, HSR_DOWNLOAD | INTERNET_FLAG_RELOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT
		); 

	if(!pFile)
	{
		_strLog += _T("Missing Server File: ") + strFilePathName;
		_strLog += _T("\n");
		
		return FALSE;
	}

	if(!pFile->SendRequest())
	{
		pFile->Close();

		_strLog += _T("Failed to get Server File: ") + strFilePathName;
		_strLog += _T("\n");

		return FALSE;
	}

	DWORD dwStatusCode;
	if(pFile->QueryInfoStatusCode(dwStatusCode))
	{
		//200-299 Success 
		if( !(dwStatusCode >= 200 && dwStatusCode < 300) )
		{
			pFile->Close();

			_strLog += _T("Failed to get Server File: ") + strFilePathName;
			_strLog += _T("\n");

			return FALSE;
		}
	}

	CString strPutLocalFileName;
	CString strRealLocalFileName = _strInstallDir + pInfo->_relatedPathName;
	if(bCompressed)
		strPutLocalFileName = _strTmpDir + _T("~~CasualPath.tmp");
	else
		strPutLocalFileName = strRealLocalFileName;

	if(pInfo->_relatedPathName.Find(_T("VersionPatch.exe")) != -1)
	{
		strRealLocalFileName.SetAt(strRealLocalFileName.GetLength() - 1, _T('t'));
	}

	UINT dwFileLength = 0;
	BOOL hasContentLength = FALSE;
	BOOL validContentLength = TRUE;
	CStdioFile csf;
	if( !csf.Open(strPutLocalFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary) )
	{
		_strLog += _T("Failed to Create Local File: ") + strPutLocalFileName;
		_strLog += _T("\n");

		pFile->Close();

		return FALSE;
	}
	else
	{
		CString strLength;
		hasContentLength = pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, strLength);
		TCHAR* lengthEnd = NULL;
		ULONGLONG declaredLength = _tcstoui64((LPCTSTR)strLength, &lengthEnd, 10);
		validContentLength = !hasContentLength ||
			(lengthEnd != (LPCTSTR)strLength && *lengthEnd == 0 && declaredLength <= (UINT)-1);
		dwFileLength = validContentLength && hasContentLength ? (UINT)declaredLength : 0;
		int iTotalPercent	= dwFileLength / 4096;

		m_ProgressCurrent.SetRange(0, (short)iTotalPercent);
	}

	std::vector<BYTE> downloaded;
	if (!validContentLength || !ClientPatch::ReadDownload(*pFile, downloaded, hasContentLength ? dwFileLength : (UINT)-1) ||
		(hasContentLength && downloaded.size() != dwFileLength))
	{
		pFile->Close();
		delete pFile;
		csf.Close();
		_strLog += _T("Invalid download length.\n");
		return FALSE;
	}
	dwFileLength = (UINT)downloaded.size();
	BYTE* fileBuf = new BYTE[dwFileLength];
	if (dwFileLength > 0)
		memcpy(fileBuf, &downloaded[0], dwFileLength);
	std::vector<BYTE>().swap(downloaded);
	csf.Close();

	pFile->Close();

	if(bCompressed)
	{
		CFile cf;
		//if( cf.Open(strPutLocalFileName, CFile::modeRead | CFile::typeBinary, 0) )
		{
			UINT iSrcLen = dwFileLength;//(UINT)cf.GetLength();
			BYTE* pbyteBuffer = fileBuf;//new BYTE[iSrcLen];
			//cf.Read(pbyteBuffer, iSrcLen);
			//cf.Close();

			uLong uncomprLen = pInfo->_originLen;
			Byte *uncompr = new Byte[uncomprLen];
			int err = uncompress(uncompr, &uncomprLen, pbyteBuffer, iSrcLen);
			if (err != Z_OK)
			{
				_strLog += _T("Failed to Uncompress file: ") + strRealLocalFileName;
				_strLog += _T("\n");

				delete[] fileBuf;//delete [] pbyteBuffer;
				delete [] uncompr;

				return FALSE;
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

	if(pInfo->_iOperation == EPO_Install)		// Patch Install
	{
		_vectorPatchInstalls.push_back(strRealLocalFileName);
	}

	return TRUE;
}

void CFilePatchDlg::DoPatchVi()
{
	for(size_t t = 0; t < _vectorFilePatches.size(); ++t)
	{
		if(!_vectorFilePatches[t]._bShouldUpdateViFile)
			continue;

		CString strPutLocalFileName = _strTmpDir + _vectorFilePatches[t]._strViFileName;
		
		CFile cf;
		if( cf.Open(strPutLocalFileName, CFile::modeRead | CFile::typeBinary, 0) )
		{
			std::vector<BYTE> bytes;
			const bool complete = ClientPatch::ReadFileBuffer(cf, bytes);
			cf.Close();
			if (!complete)
			{
				_bPatchSuccess = FALSE;
				continue;
			}

			CFile out;
			if (out.Open(_vectorFilePatches[t]._strViFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary, 0))
			{
				if (!bytes.empty())
					out.Write(&bytes[0], (UINT)bytes.size());
				out.Close();
			}

			_strLog += _T("DoPatchVi : ") + _vectorFilePatches[t]._strViFileName;
			_strLog += _T("\n");
		}
	}
}


HBRUSH CFilePatchDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	SAFE;
	
	HBRUSH br = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	UNREFERENCED_PARAMETER(br);

	pDC->SetBkMode(TRANSPARENT);
	return NULL;


	static CBrush br1(RGB(0, 153, 204));
	static CBrush br2(RGB(255, 255, 204));

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

LRESULT CFilePatchDlg::OnNcHitTest(CPoint point)
{
	SAFE;

	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);

	UNSAFE;
}

BOOL CFilePatchDlg::OnEraseBkgnd(CDC* pDC)
{
	SAFE;

	CU_TransparentBlt(pDC, _BmpBk, CPoint(0, 0), CSize(0, 0), TRUE, RGB(255, 0, 255));

	return TRUE;

	UNSAFE;
}

int CFilePatchDlg::StartProcess(const CString& strExeFilePatchName)
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

UINT __stdcall CFilePatchDlg::VerCheckThread(LPVOID lpThreadParameter)
{
	CFilePatchDlg* patch_dialog = (CFilePatchDlg*)lpThreadParameter;

	while (!patch_dialog->VerCheckNeedExit())
	{
		DWORD nt = GetTickCount();
		if (nt > patch_dialog->_nextVerCheckTime)
		{
			CString ver_name;
			if (!patch_dialog->DownFileToString(_T("QAGameServer"), ver_name))
				ver_name = _T("未知");
			patch_dialog->SetDlgItemText(IDC_QA_VERSION_GAME, CString(_T("版本: ")) + ver_name);

			if (!patch_dialog->DownFileToString(_T("QAEditorServer"), ver_name))
				ver_name = _T("未知");
			patch_dialog->SetDlgItemText(IDC_QA_VERSION_EDIT, CString(_T("版本: ")) + ver_name);

			if (!patch_dialog->DownFileToString(_T("Server"), ver_name))
				ver_name = _T("未知");
			patch_dialog->SetDlgItemText(IDC_END_VERSION_GAME, CString(_T("版本: ")) + ver_name);

			if (!patch_dialog->DownFileToString(_T("EditorServer"), ver_name))
				ver_name = _T("未知");
			patch_dialog->SetDlgItemText(IDC_END_VERSION_EDIT, CString(_T("版本: ")) + ver_name);

            if (!patch_dialog->DownFileToString(_T("ShippingGameServer"), ver_name))
                ver_name = _T("未知");
            patch_dialog->SetDlgItemText(IDC_RELEASE_VERSION_GAME, CString(_T("版本: ")) + ver_name);

			TCHAR vcs[64] = {0};

			GetPrivateProfileString(_T("GAME"), _T("VERSION"), _T(""), vcs, 63, patch_dialog->_strVersionLocalConfigFile);
			if (!*vcs)
				_tcscpy_s(vcs, _T("未知"));
			patch_dialog->SetDlgItemText(IDC_LOCAL_VERSION_GAME, CString(_T("版本: ")) + vcs);

			GetPrivateProfileString(_T("EDIT"), _T("VERSION"), _T(""), vcs, 63, patch_dialog->_strVersionLocalConfigFile);
			if (!*vcs)
				_tcscpy_s(vcs, _T("未知"));
			patch_dialog->SetDlgItemText(IDC_LOCAL_VERSION_EDIT, CString(_T("版本: ")) + vcs);

            GetPrivateProfileString(_T("RELEASE"), _T("VERSION"), _T(""), vcs, 63, patch_dialog->_strVersionLocalConfigFile);
            if (!*vcs)
                _tcscpy_s(vcs, _T("未知"));
            patch_dialog->SetDlgItemText(IDC_RELEASE_VERSION_EDIT, CString(_T("版本: ")) + vcs);

			_tstrtime_s(vcs);
			patch_dialog->SetDlgItemText(IDC_REF_VERSION_TIME, CString(_T("最近一次刷新版本时间:")) + vcs);

			EnterCriticalSection(&patch_dialog->_csCheckThread);
			patch_dialog->_nextVerCheckTime = nt + 60*1000;
			LeaveCriticalSection(&patch_dialog->_csCheckThread);
		}
		Sleep(1);
	}
	return 0;
}

BOOL CFilePatchDlg::DownFileToString(const CString& section_name, CString& out_string)
{
	int server_count = GetPrivateProfileInt(section_name, _T("ServerCount"), -1, _strPatchConfigFile);
	if (server_count == -1)
		return FALSE;

	TCHAR ver_file[MAX_PATH] = {0};
	GetPrivateProfileString(section_name, _T("VerFile"), _T(""), ver_file, MAX_PATH-1, _strPatchConfigFile);
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
				, HSR_DOWNLOAD | INTERNET_FLAG_RELOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT
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
			catch (CException* /*exp*/)
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

void CFilePatchDlg::OnBnClickedStartgame()
{
	UpdateData();

	CString app = App::GetAppPath() + _T("BinariesDev\\Win32\\");

	CString para = _T("-log -DEV_RELEASE");

	if(_useEngLang)
	{
		para += _T(" -LANGUAGE=ENG");
	}
	else
	{
		para += _T(" -LANGUAGE=CHN");
	}
	
	::ShellExecute(0, _T("open"), _T("MMOGame.exe"), (LPCTSTR)para, app, SW_NORMAL);

	if(_autoStartTime > 0)
	{
		KillTimer(C_TimerID);
	}
}


void CFilePatchDlg::OnBnClickedStartEditor()
{
	UpdateData();

	CString platformDir = _T("Win32");
	if(_use64Bit)
	{
		platformDir = _T("Win64");
	}

	CString app = App::GetAppPath() + _T("BinariesEditor\\") + platformDir + _T("\\");

	CString para = _T("editor -log -EDITOR_RELEASE -NoGADWarning");

	if(_useEngLang)
	{
		para += _T(" -LANGUAGE=ENG");
	}
	else
	{
		para += _T(" -LANGUAGE=CHN");
	}

	::ShellExecute(0, _T("open"), _T("MMOGame.exe"), (LPCTSTR)para, app, SW_NORMAL);
}

void CFilePatchDlg::OnBnClickedCancel()
{
	CFilePatchDlg::OnCancel();
}

void CFilePatchDlg::OnBnClickedUpdateGame()
{
	_btStart.EnableWindow(FALSE);
	_serverSection = _T("Server");
	StartPatch();
}

void CFilePatchDlg::OnBnClickedUpdateEditor()
{
	_btStartEditor.EnableWindow(FALSE);
	_serverSection = _T("EditorServer");
	StartPatch();
}

void CFilePatchDlg::OnBnClickedUpdateQAGame()
{
	_btStart.EnableWindow(FALSE);
	_serverSection = _T("QAGameServer");
	StartPatch();
}

void CFilePatchDlg::OnBnClickedUpdateQAEditor()
{
	_btStartEditor.EnableWindow(FALSE);
	_serverSection = _T("QAEditorServer");
	StartPatch();
}

void CFilePatchDlg::PatchOK()
{
#if M_USE_MT_UPDATE
	//WaitForAllWorks(g_downloadThreadPool, _T("下载工作线程数量:%d"));
	//WaitForAllWorks(g_writeFileThreadPool, _T("写文件工作线程数量:%d"));
	m_ProgressCurrent.SetRange(0, (short)g_downloadTaskTotalCount);
	m_ProgressAll.SetRange(0, (short)g_downloadTaskTotalCount);

	
	bool allComplete = false;
	while(!allComplete)
	{
		//allComplete = g_downloadThreadPool->IsAllTaskCompleted() && g_writeFileThreadPool->GetWorkingThreadCount();
		allComplete = (g_downloadTaskCompletedCount == g_downloadTaskTotalCount)
			&& (g_writeFileTaskCompletedCount == g_downloadTaskTotalCount);

		m_strStatusDetail.Format(_T("下载:%d/%d (%d/%d) 写文件:%d/%d (%d/%d)"),
			g_downloadTaskCompletedCount, g_downloadTaskTotalCount, g_downloadThreadPool->GetWorkingThreadCount(), g_downloadThreadPool->GetPoolSize(),
			g_writeFileTaskCompletedCount, g_downloadTaskTotalCount, g_writeFileThreadPool->GetWorkingThreadCount(), g_writeFileThreadPool->GetPoolSize());

		m_ProgressCurrent.SetPos(g_downloadTaskCompletedCount);
		m_ProgressAll.SetPos(g_writeFileTaskCompletedCount);

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

	if(_autoStartTime > 0)
	{
		this->SetTimer(C_TimerID, 1000, NULL);
	}

	_btUpdateGame.EnableWindow(TRUE);
	_btUpdateEditor.EnableWindow(TRUE);

	_btUpdateQAGame.EnableWindow(TRUE);
	_btUpdateQAEditor.EnableWindow(TRUE);

	_btStart.EnableWindow(TRUE);
	_btStartEditor.EnableWindow(TRUE);

	_btnUpdateReleaseGame.EnableWindow(TRUE);
	_btnStartReleaseGame.EnableWindow(TRUE);
	//_btStart.ShowWindow(SW_SHOW);

	if(_pHttpConnection)
	{
		_pHttpConnection->Close();
	}

	if(_InternetSession)
	{
		delete _InternetSession;
	}
}


void CFilePatchDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	_strInfoAutoStart.Format(_T("游戏将在%d秒后自动启动"), _autoStartTime - _countDownTime);
	UpdateData(FALSE);
	_infoAutoStart.Invalidate(FALSE);
	_infoAutoStart.UpdateWindow();
	_countDownTime++;

	if(_countDownTime == _autoStartTime)
	{
		OnBnClickedStartgame();
	}

	CDialog::OnTimer(nIDEvent);
}

void CFilePatchDlg::StartPatch()
{

	UpdateData();

	DWORD id = FindAppProcessID(_T("MMOGame.exe"));
	if(id != -1)
	{
		if(AfxMessageBox(_T("游戏运行，请先关闭游戏才能进行更新！\n\n如果选择确定将强行非正常关闭程序！")
			, MB_OKCANCEL | MB_ICONHAND) == IDOK
			)
		{
			HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
			if(ProcessHandle)
				TerminateProcess(ProcessHandle, 0);
		}
		else
		{
			return;
		}
	}

	_vectorFilePatches.clear();
	_vectorPatchInstalls.clear();

	vector<SServer> vectorServers;

	// Install Dir
 	TCHAR strBuffer[MAX_PATH];
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
			, strBuffer, MAX_PATH
			, _strPatchConfigFile
			);

		if( _tcslen(strBuffer) > 6 )
		{
			SServer server;

			DWORD dwType;
			if( ::AfxParseURL(strBuffer, dwType, server._strServer, server._strServerInitPath, server._dwPort)
				&&
				AFX_INET_SERVICE_HTTP == dwType
				)
			{
				vectorServers.push_back(server);
			}
		}
	}

	int iValidServer = -1;
	if(vectorServers.size() == 0)
	{
		AfxMessageBox( _T("没有发现更新服务器配置文件：PatchConfig.ini！\n\n也可能更新服务器配置有误！"), MB_OK | MB_ICONINFORMATION );
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
		_InternetSession = new CInternetSession();
		//_InternetSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5);

		
		strTmp = "ViFile";
		::GetPrivateProfileString(_serverSection
			, strTmp, _T("")
			, strBuffer, MAX_PATH
			, _strPatchConfigFile
			);

		// 轮询Patch服务器
		vector<SServer>::iterator it;
		for(it = vectorServers.begin(); it != vectorServers.end(); )
		{
			CString strTestFile = it->_strServerInitPath + strBuffer;//_T("index.html");//
			_pHttpConnection = _InternetSession->GetHttpConnection(it->_strServer, it->_dwPort);
			CHttpFile *pFile = _pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET
				, strTestFile
				, NULL
				, 1
				, NULL
				, NULL
				, HSR_DOWNLOAD | INTERNET_FLAG_RELOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT
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
			catch (CException* /*exp*/)
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
			++it;
		}

		//Hide InfoUI
		//		checkServerDlg.ShowWindow(SW_HIDE);
	}

	int iValidServerCount = int(vectorServers.size());
	if(iValidServerCount == 0)
	{
		AfxMessageBox(_T("未找到可用的更新服务器!"), MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	if(iValidServerCount == 1)
		iValidServer = 0;
	else
	{
		iValidServer = rand() % iValidServerCount;
	}

	_strServer		= vectorServers[iValidServer]._strServer;
	_serverPort = vectorServers[iValidServer]._dwPort;
	_strInitRoot	= vectorServers[iValidServer]._strServerInitPath;

	_UpdateADImage(_strPatchConfigFile);

	PostMessage(M_BEGIN_PATCH);
}

void CFilePatchDlg::_UpdateADImage( const CString& strPatchConfigFile )
{
	TCHAR strTmpBuf[MAX_PATH];
	::GetPrivateProfileString(_T("AD"), _T("GotoURL")
		, _T("http://aod.sdo.com")
		, strTmpBuf, MAX_PATH, strPatchConfigFile
		);

	UINT adCnt = ::GetPrivateProfileInt(_T("AD"), _T("AdSourceCnt"), 1, strPatchConfigFile );
	adCnt = rand() % adCnt + 1;

	CString adSrc;
	adSrc.Format(_T("AdSource%d"), adCnt);

	_strAdURL = strTmpBuf;
	::GetPrivateProfileString(_T("AD"), (LPCTSTR)adSrc
		, _T("http://img.dfs.sdg-china.com/m/hu5K92bV7B52XKwl10.JPG")
		, strTmpBuf, MAX_PATH, strPatchConfigFile
		);

	_strAdImg = strTmpBuf;

	//_htmlADBar.SetAdURL(_strAdURL, _strAdImg);
}

bool CFilePatchDlg::_LoadLocalViInfo( const CString& viFile, SFilePatch &filePatch, bool newInstall )
{
	CStdioFile stdFile;
	if( stdFile.Open(viFile, CFile::modeRead | CFile::typeText) )
	{
		SLocalVersionInfo localViInfo;

		CString strTmp;
		while(stdFile.ReadString(strTmp))
		{
			int iPos1, iPos2;
			iPos1 = strTmp.Find(_T(','));
			localViInfo._relatedPathName = strTmp.Left(iPos1);

			iPos2 = strTmp.Find(_T(','), iPos1 + 1);
			localViInfo._contentType = strTmp.Mid(iPos1 + 1, iPos2 - iPos1 - 1);

			iPos1 = iPos2;
			iPos2 = strTmp.Find(_T(','), iPos1 + 1);
			localViInfo._codeMD5 = strTmp.Mid(iPos1 + 1, iPos2 - iPos1 - 1);

			if(newInstall)
			{
				localViInfo._codeMD5 = "Missing";
			}

			CString str = strTmp.Right(strTmp.GetLength() - iPos2 - 1);
			localViInfo._originLen = _ttoi((LPCTSTR)str);

			if(localViInfo._relatedPathName.GetLength() &&
				( localViInfo._contentType == _T("Dir") || localViInfo._contentType == _T("File") )
				)
				filePatch._vectorLocalViInfoes.push_back(localViInfo);
		}

		stdFile.Close();
		return true;
	}

	return false;
}


void CFilePatchDlg::OnBnClickedUpdateReleaseGame()
{
	_btnUpdateReleaseGame.EnableWindow(FALSE);
	_btnStartReleaseGame.EnableWindow(FALSE);
	_serverSection = _T("ShippingGameServer");
	StartPatch();
}


void CFilePatchDlg::OnBnClickedStartReleaseGame()
{
	UpdateData();

	CString app = App::GetAppPath() + _T("AOD\\Binaries\\Win32\\");

	CString para = _T("-log –seekfreeloading -FINAL_RELEASE -nohomedir");

	if(_useEngLang)
	{
		para += _T(" -LANGUAGE=ENG");
	}
	else
	{
		para += _T(" -LANGUAGE=CHN");
	}

	::ShellExecute(0, _T("open"), _T("MMOGame.exe"), (LPCTSTR)para, app, SW_NORMAL);
}

void CFilePatchDlg::OnDestroy()
{
	if (_verCheckThread)
	{
		_checkThreadExit = TRUE;
		WaitForSingleObject(_verCheckThread, INFINITE);
		_verCheckThread = 0;
	}
	DeleteCriticalSection(&_csCheckThread);

	CDialog::OnDestroy();
}


void CFilePatchDlg::OnBnClickedRefVersion()
{
	EnterCriticalSection(&_csCheckThread);
	_nextVerCheckTime = 0;
	LeaveCriticalSection(&_csCheckThread);
}


void CFilePatchDlg::OnBnClickedUpdateShaderCache()
{
	_serverSection = _T("ShaderCache");
	StartPatch();
}
