
// encryptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "encrypt.h"
#include "encryptDlg.h"
#include "afxdialogex.h"
#include "file_transform.h"
#include <filesystem>
#include <format>
#include <string>
#include <utility>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CencryptDlg dialog



CencryptDlg::CencryptDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CencryptDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CencryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CencryptDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON2, &CencryptDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON1, &CencryptDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CencryptDlg message handlers

BOOL CencryptDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CencryptDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CencryptDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CencryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CencryptDlg::TransformSelectedFile(gm_file::Operation operation)
{
    const auto state=m_operation;
    if (std::exchange(state->running,true)) return;
    struct Reset
    {
        std::shared_ptr<OperationState> state;
        ~Reset(){state->running=false;}
    } reset{state};
    CString filename;
    GetDlgItemText(IDC_MFCEDITBROWSE1,filename);
    const std::filesystem::path input(std::wstring(filename.GetString(),static_cast<std::size_t>(filename.GetLength())));
    const auto result=gm_file::transform_file(input,operation);
    if (result)
    {
        const auto message=std::format(L"{}\n{}",operation==gm_file::Operation::encrypt ? L"加密成功" : L"解密成功",result->native());
        ShowOperationResult(true,CString(message.c_str()));
    }
    else
    {
        // The standard file workflow returns ASCII diagnostics; paths remain native Unicode.
        const std::wstring detail(result.error().begin(),result.error().end());
        const auto message=std::format(L"操作失败\n{}",detail);
        ShowOperationResult(false,CString(message.c_str()));
    }
}

void CencryptDlg::ShowOperationResult(bool success,const CString& message)
{
    MessageBox(message,L"文件转换",MB_OK|(success ? MB_ICONINFORMATION : MB_ICONERROR));
}

void CencryptDlg::OnBnClickedButton2()
{
    TransformSelectedFile(gm_file::Operation::decrypt);
}

void CencryptDlg::OnBnClickedButton1()
{
    TransformSelectedFile(gm_file::Operation::encrypt);
}
