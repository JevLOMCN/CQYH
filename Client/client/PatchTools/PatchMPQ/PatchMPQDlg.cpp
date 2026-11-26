/*====================================================================
	Created Date  :		2005-8-24
	
	Specification :  
====================================================================*/
#include "stdafx.h"
#include "../ReadFileBuffer.h"
#include "../PatchPath.h"
#include "PatchMPQ.h"
#include "PatchMPQdlg.h"
#include "Controls.h"
#include "App.h"
#include "Configer.h"
#include "File.h"
#include "Task.h"
#include "ThreadPool.h"
#include <assert.h>
#include <sstream>
#include <algorithm>
#include "PatchMPQUtil.h"
#include <DbgHelp.h>

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

CPatchMPQDlg::CPatchMPQDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchMPQDlg::IDD, pParent)
	, _strPath(_T(""))
	, _statusDetail(_T("N/A"))
    , _statusTotal(_T("N/A"))
    , _statusCurrent(_T("N/A"))
	, _isPatchSuccess(TRUE)
	, _countDownTime(0)
	, _progressTotal(0)
	, _progressCount(0)
{
	_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPatchMPQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, _progressCurrent);
	DDX_Control(pDX, IDC_BUTTON_MIN, _btnMin);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, _btnClose);
	DDX_Control(pDX, IDC_CURRENT, _infoCurrent);
	DDX_Control(pDX, IDC_BUTTON_INSTALL, _btInstall);
	DDX_Text(pDX, IDC_EDIT_INSTALL_PATH, _strPath);
	DDX_Text(pDX, IDC_CURRENT, _statusCurrent);
	DDX_Control(pDX, IDC_EDIT_INSTALL_PATH, _editPath);
	DDX_Control(pDX, IDC_BUTTON_INSTALL_PATH, _btInstallPath);
}

BEGIN_MESSAGE_MAP(CPatchMPQDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()

	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_MIN, OnBnClickedMin)
	ON_BN_CLICKED(IDC_BUTTON_INSTALL, &CPatchMPQDlg::OnBnClickedInstall)
	ON_BN_CLICKED(IDC_BUTTON_INSTALL_PATH, &CPatchMPQDlg::OnBnClickedInstallPath)

	ON_MESSAGE(M_BEGIN_PATCH, OnBeginPatch)
END_MESSAGE_MAP()

CRect GetControlRect(const CString& name)
{
	static CString iniFile = App::GetAppPath() + C_IniFile;
	
	static Configer cfg;
	static bool init = false;
	if(!init)
	{
//		cfg.LoadFile(iniFile);
//		init = true;
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

BOOL CPatchMPQDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//std::wcout.imbue(std::locale("chs"));
	setlocale(LC_ALL, "");

	//ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	static CString iniFile = App::GetAppPath() + C_IniFile;
	
	//static Configer cfg;
	//cfg.LoadFile(iniFile);
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

	//SetWindowText(_T("《传奇永恒》全文件修复"));

	_btnMin.Set4In1Bitmap(IDB_MIN);
	_btnMin.SetDrawTrans(TRUE);

	_btnClose.Set4In1Bitmap(IDC_BUTTON_CLOSE);
	_btnClose.SetDrawTrans(TRUE);

	_btInstall.Set4In1Bitmap(IDC_BUTTON_INSTALL);
	_btInstall.SetDrawTrans(TRUE);

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
	MoveControlToRect(_btInstall, _T("BtMin"));

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

void CPatchMPQDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPatchMPQDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData(FALSE);
	_countDownTime++;

	CDialog::OnTimer(nIDEvent);
}

void CPatchMPQDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CPatchMPQDlg::OnPaint() 
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

HCURSOR CPatchMPQDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(_hIcon);
}

CBrush br1(RGB(0, 153, 204));
CBrush br2(RGB(255, 255, 204));

HBRUSH CPatchMPQDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

LRESULT CPatchMPQDlg::OnNcHitTest(CPoint point)
{
	SAFE;

	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);

	UNSAFE;
}

BOOL CPatchMPQDlg::OnEraseBkgnd(CDC* pDC)
{
	SAFE;

	CU_TransparentBlt(pDC, _bmpBk, CPoint(0, 0), CSize(0, 0), TRUE, RGB(255, 0, 255));

	return TRUE;

	UNSAFE;
}

void CPatchMPQDlg::OnBnClickedMin()
{
	ShowWindow(SW_MINIMIZE);
}

void CPatchMPQDlg::OnBnClickedCancel()
{
	if (_isPatchSuccess)
	{
		CDialog::OnCancel();
		return;
	}

	if (MessageBox(_T("取消修复文件将导致游戏不能运行！确定取消吗？"),
		_T("警告！"),
		MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		CDialog::OnCancel();
	}
}

int CPatchMPQDlg::StartProcess(const CString& strExeFilePatchName)
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

void CPatchMPQDlg::WaitForAllWorks(class CThreadPool* thrdPool, const CString& msg /*= _T("工作线程数量:%d")*/)
{
	bool allComplete = false;
	while (!allComplete)
	{
		allComplete = thrdPool->IsAllTaskCompleted();

		_statusDetail.Format((LPCTSTR)msg, thrdPool->GetWorkingThreadCount());
		M_Update_Msg(true);

		::Sleep(100);
	}
}

void CPatchMPQDlg::OnBnClickedInstallPath()
{
	WCHAR pszPath[MAX_PATH];

	BROWSEINFO info;
	info.hwndOwner = m_hWnd;
	info.pidlRoot = NULL;
	info.pszDisplayName = pszPath;
	info.lpszTitle = _T("请选择游戏安装目录");                     
	info.ulFlags = 0;                     
	info.lpfn = NULL;
	info.lParam = 0;
	info.iImage = 0;

	LPITEMIDLIST lp = SHBrowseForFolder(&info);
	if (lp && SHGetPathFromIDList(lp, pszPath))
	{
		_strPath = pszPath;
		SetDlgItemText(IDC_EDIT_INSTALL_PATH, _strPath);
	}
	else
	{
		AfxMessageBox(_T("无效目录，请重新选择"));
	}
}

bool CPatchMPQDlg::PatchPack(const char* pSrcDir, const char* pDeltaDir, const char* pSubDir)
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

	_progressCount = 0;
	_progressCurrent.SetRange(0, (short)(vecPackFiles.size()));

	bool bSucc = true;
	// 2.delete in pak
	for (unsigned int i = 0; i < vecPackFiles.size(); ++i)
	{
		const PathString& strPackPath = vecPackFiles[i];
		WCHAR szPackPath[MAX_PATH];
		if (!ClientPatch::ToWide(strPackPath.c_str(), szPackPath))
			return false;

		FilePathList::iterator iterDelete = _deleteFileList.begin();
		for (; iterDelete != _deleteFileList.end();)
		{
			MSG msg;
			while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			const std::string& filename = iterDelete->first;

			if (pSubDirWithoutFirstSlash && strstr(filename.c_str(), pSubDirWithoutFirstSlash) != NULL)
			{
				const char* pszFileName = filename.c_str() + nSubDirLen;

				WCHAR szFilePath[MAX_PATH];
				if (!ClientPatch::ToWide(pszFileName, szFilePath))
					return false;

				bSucc = (*g_pfDeletePackFile)(szPackPath, szFilePath);
				if (bSucc)
				{
					_statusCurrent = _T("删除文件: ");

					iterDelete = _deleteFileList.erase(iterDelete);
					continue;
				}
				else
				{
					_statusCurrent = _T("删除文件失败: ");
				}
			}

			++ iterDelete;
		}

		_progressCurrent.SetPos(++_progressCount);
		UpdateData(FALSE);
	}

	// 3.extract the src file from mpq
	std::vector<PakFileIndex> vecPakFileIndex(vecDeltaFiles.size());

	unsigned int nDeltaDirLen = strDeltaDir.length();

	std::string strSrcTempDir(strSrcDir);
	strSrcTempDir += "\\Temp";

	CreateDirectoryHierarchy(strSrcTempDir.c_str(), true);

	ClientPatch::ScopedCurrentDirectory<WCHAR> currentDirectory;
	WCHAR szSrcDirTemp[MAX_PATH];
	if (!ClientPatch::ToWide(strSrcTempDir.c_str(), szSrcDirTemp) || !currentDirectory.Change(szSrcDirTemp))
		return false;

	_progressCount = 0;
	_progressCurrent.SetRange(0, (short)(vecDeltaFiles.size()));

	bSucc = true;
	for (unsigned int i = 0; i < vecDeltaFiles.size(); ++i)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		PathString& strDeltaFilePath = vecDeltaFiles[i];
		
		PathString strFilePath;
		int index = strDeltaFilePath.find(".delta");
		if (index != -1)
		{
			strFilePath = std::string(strDeltaFilePath.c_str() + nDeltaDirLen + 1, strDeltaFilePath.length() - nDeltaDirLen - std::string(".delta").length() - 1);
		}
		else
		{
			strFilePath = std::string(strDeltaFilePath.c_str() + nDeltaDirLen + 1, strDeltaFilePath.length() - nDeltaDirLen - 1);
		}
		
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

		PathString strSubDir(pSubDir);
		strSubDir += PathString("\\");
		WCHAR szSubDir[MAX_PATH];
		if (!ClientPatch::ToWide(strSubDir.c_str(), szSubDir))
			return false;

		int iPakIndex = -1;

		if (index != -1)
		{
			bSucc = (*g_pfExtractPackFile)(szSrcPath, szTempPath, szFilePath, iPakIndex);
			if (bSucc)
			{
				PathString strSubFile = strFilePath;
				if (pSubDirWithoutFirstSlash)
				{
					strSubFile = PathString(pSubDirWithoutFirstSlash) + "\\" + strFilePath;
				}

				ResultMD5List;
				std::map<std::string, std::string>::iterator iterRes;
				PathString strMD5("0");
				iterRes = _resFileList.find(strSubFile);
				if (iterRes != _resFileList.end())
				{
					strMD5 = iterRes->second;
				}

				if (CompareFileWithMD5(strTempFilePath, strMD5))
				{
					// 如果已经合并过了
					bSucc = true;
					_statusCurrent = _T("合并文件: ");
				}
				else
				{
					bSucc = (*g_pfMergePackFile)(szTempFilePath, szDeltaFilePath, szTempFilePath);
					if (bSucc)
					{
						_statusCurrent = _T("合并文件: ");
					}
					else
					{
						_statusCurrent = _T("合并文件失败: ");
					}
				}
			}
			else
			{
				_statusCurrent = _T("解压文件失败: ");
			}
		}
		else
		{
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
		}

		_statusCurrent += szDeltaFilePath;
		_statusCurrent += _T(" 到 ");
		_statusCurrent += szTempFilePath;

		_progressCurrent.SetPos(++_progressCount);
		UpdateData(FALSE);

		vecPakFileIndex[i] = PakFileIndex(iPakIndex, i);
	}
	
	// 4.add all file into mpq(include added)
	_progressCount = 0;
	_progressCurrent.SetRange(0, (short)(vecPakFileIndex.size()));

	std::sort(vecPakFileIndex.begin(), vecPakFileIndex.end());
	for (unsigned int i = 0; i < vecPakFileIndex.size(); ++i)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		const PakFileIndex& fileIndex = vecPakFileIndex[i];
		check(fileIndex.iFileIndex < vecDeltaFiles.size());

		PathString& strDeltaFilePath = vecDeltaFiles[fileIndex.iFileIndex];
		
		PathString strFilePath;
		int index = strDeltaFilePath.find(".delta");
		if (index != -1)
		{
			strFilePath = std::string(strDeltaFilePath.c_str() + nDeltaDirLen + 1, strDeltaFilePath.length() - nDeltaDirLen - std::string(".delta").length() - 1);
		}
		else
		{
			strFilePath = std::string(strDeltaFilePath.c_str() + nDeltaDirLen + 1, strDeltaFilePath.length() - nDeltaDirLen - 1);
		}

		PathString strPackPath;
		WCHAR szPackPath[MAX_PATH];
		WCHAR szFilePath[MAX_PATH];
		if (!ClientPatch::ToWide(strFilePath.c_str(), szFilePath))
			return false;

		PathString strPakDir = strSrcDir + PathString("\\");

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
		UpdateData(FALSE);
	}

	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//DeleteFileTree(strSrcDirTemp);
	DeleteFilesInDirectory(strSrcTempDir.c_str(), true);

	currentDirectory.Restore();

	return true;
}

bool CPatchMPQDlg::PatchFile(const char* pSrcFile, const char* pDeltaFile, const char* pSubDir, const char* pFileNakedName)
{
	PathString strSrcFile(pSrcFile);
	PathString strDeltaFile(pDeltaFile);
	PathString strSubDir(pSubDir);

	const char* pSubDirWithoutFirstSlash = NULL;
	unsigned int nSubDirLen = 0;

	if ((*pSubDir) != 0)
	{
		pSubDirWithoutFirstSlash = pSubDir + 1;
		nSubDirLen = strlen(pSubDirWithoutFirstSlash) + 1;
	}

	PathString strSubFile(pFileNakedName);
	if (pSubDirWithoutFirstSlash)
	{
		strSubFile = PathString(pSubDirWithoutFirstSlash) + PathString(pFileNakedName);
		int deltaIndex = strSubFile.find(".delta");
		if (deltaIndex != -1)
		{
			strSubFile = strSubFile.substr(0, deltaIndex);
		}
	}

	PathString strFileDir = strSrcFile.substr(0, strSrcFile.find_last_of('\\') + 1);
	if (!IsDirExist(strFileDir))
	{
		MakeSureDirectoryPathExists(strFileDir);
	}

	WCHAR szSrcDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strSrcFile.c_str(), -1, szSrcDir, (int)(sizeof(szSrcDir) / sizeof(szSrcDir[0]))))
		return false;

	WCHAR szDeltaDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strDeltaFile.c_str(), -1, szDeltaDir, (int)(sizeof(szDeltaDir) / sizeof(szDeltaDir[0]))))
		return false;

	WCHAR szSubDir[1024];
	if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strSubDir.c_str(), -1, szSubDir, (int)(sizeof(szSubDir) / sizeof(szSubDir[0]))))
		return false;
	
	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	bool bSucc = false;
	int index = strSrcFile.find(".delta");
	if (index != -1)
	{// 合并差异文件
		strSrcFile = strSrcFile.substr(0, index);
		if (!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, strSrcFile.c_str(), -1, szSrcDir, (int)(sizeof(szSrcDir) / sizeof(szSrcDir[0]))))
			return false;

		ResultMD5List;
		std::map<std::string, std::string>::iterator iterRes;
		PathString strMD5("0");
		iterRes = _resFileList.find(strSubFile);
		if (iterRes != _resFileList.end())
		{
			strMD5 = iterRes->second;
		}

		if (CompareFileWithMD5(strSrcFile, strMD5))
		{
			// 如果已经合并过了
			bSucc = true;
			_statusCurrent = _T("合并文件: ");
		}
		else
		{
			bSucc = (*g_pfMergePackFile)(szSrcDir, szDeltaDir, szSrcDir);
			if (bSucc)
			{
				_statusCurrent = _T("合并文件: ");
			}
			else
			{
				_statusCurrent = _T("合并文件失败: ");
			}
		}
	}
	else
	{// 拷贝新增文件
		if (IsFileExist(strSrcFile))
		{// 去除文件的只读属性， 否则会拷贝失败
			DWORD attr = ::GetFileAttributes((LPCTSTR)szSrcDir);
			attr &= ~FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes((LPCTSTR)szSrcDir, attr);
		}

		bSucc = ::CopyFile(szDeltaDir, szSrcDir, FALSE) ? true : false;
		if (bSucc)
		{
			_statusCurrent = _T("拷贝文件: ");
		}
		else
		{
			_statusCurrent = _T("拷贝文件失败: ");
		}
	}

	_statusCurrent += szDeltaDir;
	_statusCurrent += _T(" 到 ");
	_statusCurrent += szSrcDir;

	_progressCurrent.SetRange(0, 100);
	_progressCurrent.SetPos(100);
	UpdateData(FALSE);

	return bSucc;
}

void CPatchMPQDlg::InitPatchDir()
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

eSubDirType CPatchMPQDlg::GetSubDirType(const char* pSubDir)
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

bool CPatchMPQDlg::IsDeltaFile(const char* pszFilePath)
{
	FilePathList::iterator iter = _deltaFileList.find(std::string(pszFilePath));
	if (iter != _deltaFileList.end())
	{
		return true;
	}

	return false;
}

bool CPatchMPQDlg::IsNewFile(const char* pszFilePath)
{
	FilePathList::iterator iter = _newFileList.find(std::string(pszFilePath));
	if (iter != _newFileList.end())
	{
		return true;
	}

	return false;
}

bool CPatchMPQDlg::Patch(const char* pszSrcPath, const char* pszDeltaPath, const char* pszShareSubDir)
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
					
					eSubDirType e = GetSubDirType(strSubDir.c_str() + 1);
					switch (e)
					{
					case ESDT_NOT_PACK:
					case ESDT_NOT_SURE:
						{
							PathString strSrcDir(pszSrcPath);
							PathString strDeltaDir(pszDeltaPath);

							strSrcDir += strSubDir;
							strSrcDir += PathString(fd.cFileName);
							strDeltaDir += strSubDir;
							strDeltaDir += PathString(fd.cFileName);

							bSucc = bSucc & PatchFile(strSrcDir.c_str(), strDeltaDir.c_str(), strSubDir.c_str(), PathString(fd.cFileName).c_str());
						}
						break;
					case ESDT_PACK:
						// 如果是包内文件, 交给PatchPack，这里不处理
						break;
					}
				}
			}
		} while (FindNextFile(Handle, &fd) && bSucc);

		FindClose(Handle);
	}

	return bSucc;
}

LRESULT CPatchMPQDlg::OnBeginPatch(WPARAM wParam, LPARAM lParam)
{
	char szGameFilePath[MAX_PATH];
	if (!ClientPatch::ToAnsi(_strPath, szGameFilePath))
	{
		_isPatchSuccess = FALSE;
		return 0;
	}

	PathString strDeleteDir(szGameFilePath);
	FilePathList::iterator iterDelete = _deleteFileList.begin();
	for (; iterDelete != _deleteFileList.end();)
	{
		const PathString& strDeleteFilePath = strDeleteDir + PathString("\\") + iterDelete->first;

		WCHAR szDeleteFilePath[MAX_PATH];
		if (!ClientPatch::ToWide(strDeleteFilePath.c_str(), szDeleteFilePath))
		{
			_isPatchSuccess = FALSE;
			return 0;
		}

		if (IsFileExist(strDeleteFilePath))
		{
			bool bSucc = ::DeleteFile(szDeleteFilePath) ? true : false;
			if (bSucc)
			{
				iterDelete = _deleteFileList.erase(iterDelete);
				continue;
			}
		}

		++iterDelete;
	}

	char szDeltaFilePath[MAX_PATH];
	if (!ClientPatch::ToAnsi(_strDeltaFilePath, szDeltaFilePath))
	{
		_isPatchSuccess = FALSE;
		return 0;
	}

	_progressCount = 0;
	if (Patch(szGameFilePath, szDeltaFilePath, "\0"))
	{
		// 修改新的版本号
		CString strSrcLVPath = App::GetAppPath() + _T("LocalVersion.xml");

		char szLocalVersionPath[MAX_PATH];
		if (!ClientPatch::ToAnsi(strSrcLVPath, szLocalVersionPath))
		{
			_isPatchSuccess = FALSE;
			return 0;
		}

		if (IsFileExist(szLocalVersionPath))
		{// 去除文件的只读属性， 否则会拷贝失败
			DWORD attr = ::GetFileAttributes((LPCTSTR)strSrcLVPath);
			attr &= ~FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes((LPCTSTR)strSrcLVPath, attr);
		}

		CString strDstLVPath = _strPath + _T("\\") + _T("LocalVersion.xml");
		bool bSucc = ::CopyFile(strSrcLVPath, strDstLVPath, FALSE) ? true : false;
		if (bSucc)
		{
			_statusCurrent = _T("拷贝文件: ");
		}
		else
		{
			_statusCurrent = _T("拷贝文件失败: ");

			CString strTips = _T("无法拷贝LocalVersion.xml！\n版本:") + _strVersionEnd + _T("的补丁安装失败！\n请运行全文件修复工具检查.");
			AfxMessageBox(strTips);

			EndDialog(IDOK);
			_btInstallPath.EnableWindow(TRUE);
			_btInstall.EnableWindow(TRUE);

			return 0;
		}

		_statusCurrent = _T("安装成功");
		_progressCurrent.SetRange(0, 100);
		_progressCurrent.SetPos(100);
		UpdateData(FALSE);

		CString strTips = _T("版本:") + _strVersionEnd + _T("的补丁安装成功！\n如有问题请运行全文件修复工具检查.");
		AfxMessageBox(strTips);
	}
	else
	{
		_statusCurrent = _T("安装失败");

		CString strTips = _T("版本:") + _strVersionEnd + _T("的补丁安装失败！\n请运行全文件修复工具检查.");
		AfxMessageBox(strTips);
	}

	EndDialog(IDOK);
	_btInstallPath.EnableWindow(TRUE);
	_btInstall.EnableWindow(TRUE);
	return 0;
}

void CPatchMPQDlg::OnBnClickedInstall()
{
	char szGamePath[1024];
	if (!ClientPatch::ToAnsi(_strPath, szGamePath))
	{
		_isPatchSuccess = FALSE;
		return;
	}
	if (!IsDirExist(szGamePath))
	{
		AfxMessageBox(_T("无效目录，无法找到游戏文件，请重新选择"));
		return;
	}

	// 通过特征文件判断目录是否有效
	CString strGamePath = _strPath;
	strGamePath += _T("\\Launcher.exe");
	if (!File::IsFileExist(strGamePath))
	{
		AfxMessageBox(_T("无效目录，无法找到游戏文件，请重新选择"));
		return;
	}

	_btInstallPath.EnableWindow(FALSE);
	_btInstall.EnableWindow(FALSE);
	
	_curClientVer = _T("");
	_strVersionBegin = _T("");
	_strVersionEnd = _T("");
	_strDeltaFilePath = _T("");

	// 补丁包版本号比较
	CString strVersionPath = _strPath;
	strVersionPath += _T("\\CliV.ini");
	if (!File::IsFileExist(strVersionPath))
	{
		AfxMessageBox(_T("没有检测到本地版本号，请至少启动一次游戏。"), MB_OK);
		_btInstallPath.EnableWindow(TRUE);
		_btInstall.EnableWindow(TRUE);
		return;
	}
	else
	{
		// 先判断文件是unicode 还是 ansi
		CStdioFile verFile;
		if (!verFile.Open(strVersionPath, CFile::modeRead))
		{
			AfxMessageBox(_T("没有检测到本地版本号，请至少启动一次游戏。"), MB_OK);
			_btInstallPath.EnableWindow(TRUE);
			_btInstall.EnableWindow(TRUE);
			return;
		}

		verFile.SeekToBegin();
		const bool bUnicode = ClientPatch::HasUnicodeBom(verFile);

		verFile.Close();

		if (bUnicode)
		{
			if (verFile.Open(strVersionPath, CFile::modeRead | CFile::typeBinary))
			{
				verFile.Seek(2, CFile::begin);
				if (!verFile.ReadString(_curClientVer))
				{
					verFile.Close();
					AfxMessageBox(_T("没有读到游戏版本号，请至少启动一次游戏。"), MB_OK);
					_btInstallPath.EnableWindow(TRUE);
					_btInstall.EnableWindow(TRUE);
					return;
				}
				verFile.Close();
			}
		}
		else
		{
			if (verFile.Open(strVersionPath, CFile::modeRead | CFile::typeText))
			{
				if (!verFile.ReadString(_curClientVer))
				{
					verFile.Close();
					AfxMessageBox(_T("没有读到游戏版本号，请至少启动一次游戏。"), MB_OK);
					_btInstallPath.EnableWindow(TRUE);
					_btInstall.EnableWindow(TRUE);
					return;
				}
				verFile.Close();
			}
		}
	}

	CString strCmdLine = AfxGetApp()->m_lpCmdLine;
	_strVersionBegin = strCmdLine.Left(strCmdLine.Find(_T(" ")));
	_strVersionEnd = strCmdLine.Mid(strCmdLine.Find(_T(" ")), strCmdLine.GetLength() - strCmdLine.Find(_T(" ")));

	char szClientVersion[MAX_PATH];
	if (!ClientPatch::ToAnsi(_curClientVer, szClientVersion))
	{
		_isPatchSuccess = FALSE;
		return;
	}

	char szCurVersion[MAX_PATH];
	if (!ClientPatch::ToAnsi(_strVersionBegin, szCurVersion))
	{
		_isPatchSuccess = FALSE;
		return;
	}

	if (strcmp(szClientVersion, szCurVersion) != 0)
	{
		AfxMessageBox(_T("当前游戏版本号和补丁版本号不一致，安装失败"), MB_OK);
		_btInstallPath.EnableWindow(TRUE);
		_btInstall.EnableWindow(TRUE);
		return;
	}

	// 加载差异文件列表
	CString strDeltaInfoPath = App::GetAppPath() + _T("patch_delta_direct.dat");
	char szDeltaInfoPath[MAX_PATH];
	if (!ClientPatch::ToAnsi(strDeltaInfoPath, szDeltaInfoPath))
	{
		_isPatchSuccess = FALSE;
		return;
	}

	_statusCurrent = _T("加载差异文件列表");
	UpdateData(FALSE);

	if (!IsFileExist(szDeltaInfoPath))
	{
		AfxMessageBox(_T("无法加载差异文件列表!"));
		_btInstallPath.EnableWindow(TRUE);
		_btInstall.EnableWindow(TRUE);
		return;
	}

	_deltaFileList.clear();
	ParseDirectFileSection((LPCTSTR)strDeltaInfoPath.GetBuffer(), (LPCTSTR)CString("DeltaPathInfo"), _deltaFileList);
	_newFileList.clear();
	ParseDirectFileSection((LPCTSTR)strDeltaInfoPath.GetBuffer(), (LPCTSTR)CString("NewPathInfo"), _newFileList);
	_deleteFileList.clear();
	ParseDirectFileSection((LPCTSTR)strDeltaInfoPath.GetBuffer(), (LPCTSTR)CString("DelPathInfo"), _deleteFileList);
	_resFileList.clear();
	ParseDirectFileSection((LPCTSTR)strDeltaInfoPath.GetBuffer(), (LPCTSTR)CString("ResultMD5Info"), _resFileList);
	strDeltaInfoPath.ReleaseBuffer();

	_progressTotal = _deltaFileList.size() + _newFileList.size();
	_progressCurrent.SetRange(0, (short)(_progressTotal));

	// 开始打补丁
	_strDeltaFilePath = App::GetAppPath() + _T("Pkg");
	char szDeltaFilePath[MAX_PATH];
	if (!ClientPatch::ToAnsi(_strDeltaFilePath, szDeltaFilePath))
	{
		_isPatchSuccess = FALSE;
		return;
	}

	if (!IsDirExist(szDeltaFilePath))
	{
		AfxMessageBox(_T("无法找到补丁包!"));
		_btInstallPath.EnableWindow(TRUE);
		_btInstall.EnableWindow(TRUE);
		return;
	}

	_statusCurrent = _T("开始安装补丁");
	UpdateData(FALSE);

	PostMessage(M_BEGIN_PATCH);
}
