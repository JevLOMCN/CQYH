
// ReplayViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ReplayView.h"
#include "ReplayViewDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



enum EReplayDataType
{
    ERDT_Network,
    ERDT_Input,
    ERDT_ScreenRes
};

enum ENetworkDataType
{
    ENDT_Recv,
    ENDT_Send
};

enum EInputDataType
{
    EIDT_Key,
    EIDT_Axis,
    EIDT_Char,
    EIDT_MousePosition
};

static TCHAR* g_inputDataTypeStr[] =
{
    TEXT("Key"),
    TEXT("Axis"),
    TEXT("Char"),
    TEXT("MousePosition")
};

struct FReplayData
{
    FReplayData() : Index(0), GameTime(0) {}
    UINT64 Index;
    DOUBLE GameTime;

    virtual bool Read(FMemFile& pf)
    {
        if (!pf.Read(&Index))
            return false;
        return pf.Read(&GameTime) != 0;
    }
    virtual void ToString(CString& out) = 0;
};

struct FReplayDataScreenRes : public FReplayData
{
    FReplayDataScreenRes() : ResX(0), ResY(0), IsFullScreen(FALSE) {}
    UINT ResX;
    UINT ResY;
    UBOOL IsFullScreen;

    virtual bool Read(FMemFile& pf)
    {
        //if (!FReplayData::Read(pf))
        //    return false;
        return pf.Read(&ResX) && pf.Read(&ResY) && pf.Read(&IsFullScreen);
    }
    virtual void ToString(CString& out)
    {
        out.AppendFormat(_T("0(0.0000)，屏謩信息：%d x %d， %s"), ResX, ResY, IsFullScreen ? _T("全屏") : _T("窗口"));
    }
};

struct FReplayDataNetwork : public FReplayData
{
    FReplayDataNetwork() : DataType(ENDT_Recv), DataSize(0) {}
    ENetworkDataType DataType;
    INT DataSize;
    std::vector< BYTE > buf;

    virtual bool Read(FMemFile& pf)
    {
        buf.clear();
        DataSize = 0;
        if (!FReplayData::Read(pf) || !pf.Read(&DataType) || !pf.Read(&DataSize) ||
            (DataType != ENDT_Recv && DataType != ENDT_Send) ||
            DataSize < (INT)sizeof(WORD) || DataSize > 0xFFFF || (size_t)DataSize > pf.Remaining())
            return false;
        buf.resize(DataSize);
        return pf.ReadByte(&buf[0], DataSize, 1) != 0;
    }
    virtual void ToString(CString& out)
    {
        if (buf.size() < sizeof(WORD))
            return;
        WORD id = 0;
        memcpy(&id, &buf[0], sizeof(id));
        net_message_object& nmo = ((CReplayViewDlg*)theApp.m_pMainWnd)->m_NetMessage;
        out.AppendFormat(_T("%lld(%.04lf)，%s：%d，%s"), Index, GameTime, ENDT_Recv == DataType ? _T("接收") : _T("发送"), id, nmo.GetMsgName(id));
    }
};
struct FIntPoint
{
    INT X, Y;
};
enum EInputEvent
{
    IE_Pressed              =0,
    IE_Released             =1,
    IE_Repeat               =2,
    IE_DoubleClick          =3,
    IE_Axis                 =4,
    IE_MAX                  =5,
};
struct FReplayDataInput : public FReplayData
{
    FReplayDataInput()
        : DataType(EIDT_Key), ControllerId(0), EventType(IE_Pressed), AmountDepressed(0),
          GamePad(FALSE), Delta(0), DeltaTime(0), Character(0), name_idx(0), name_number(0)
    {
        MousePoint.X = MousePoint.Y = 0;
    }
    EInputDataType DataType;
    FIntPoint MousePoint;
    INT ControllerId;
    CString Key;
    EInputEvent EventType;
    FLOAT AmountDepressed;
    UBOOL GamePad;
    FLOAT Delta;
    FLOAT DeltaTime;
    WCHAR Character;

    NAME_INDEX name_idx;
    INT name_number;

    virtual bool Read(FMemFile& pf)
    {
        if (!FReplayData::Read(pf) || !pf.Read(&DataType) ||
            DataType < EIDT_Key || DataType > EIDT_MousePosition ||
            !pf.Read(&MousePoint) || !pf.Read(&ControllerId))
            return false;
        name_idx = name_number = 0;

        if(DataType == EIDT_Key)
        {
            if (!pf.Read(&name_idx) || !pf.Read(&name_number))
                return false;
        }

        return pf.Read(&EventType) && pf.Read(&AmountDepressed) && pf.Read(&GamePad) &&
            pf.Read(&Delta) && pf.Read(&DeltaTime) && pf.Read(&Character);
    }
    virtual void ToString(CString& out)
    {
        static TCHAR* evtTypeStr[] =
        {
            TEXT("IE_Pressed"),
            TEXT("IE_Released"),
            TEXT("IE_Repeat"),
            TEXT("IE_DoubleClick"),
            TEXT("IE_Axis"),
            TEXT("IE_MAX")
        };
        switch (DataType)
        {
        case EIDT_Key:
            out.AppendFormat(_T("%lld(%.04lf)，按键盘 (%d，%d)："), Index, GameTime, MousePoint.X, MousePoint.Y);
            if (EventType >= 0 && EventType < IE_MAX)
                out += evtTypeStr[EventType];
        	break;
        case EIDT_Axis:
            if (((CReplayViewDlg*)theApp.m_pMainWnd)->IsDlgButtonChecked(IDC_CHECK_AXIS) == BST_CHECKED)
                out.AppendFormat(_T("%lld(%.04lf)，滚轮 (%d，%d)"), Index, GameTime, MousePoint.X, MousePoint.Y);
            break;
        case EIDT_Char:
            out.AppendFormat(_T("%lld(%.04lf)，输入字符 (%d，%d)"), Index, GameTime, MousePoint.X, MousePoint.Y);
            break;
        case EIDT_MousePosition:
            if (((CReplayViewDlg*)theApp.m_pMainWnd)->IsDlgButtonChecked(IDC_CHECK_MMOVE) == BST_CHECKED)
                out.AppendFormat(_T("%lld(%.04lf)，鼠标移动 (%d，%d)"), Index, GameTime, MousePoint.X, MousePoint.Y);
            break;
        default:
            out += _T("？？？未知事件？？？");
        }
    }
};


void CReplayViewDlg::ReplayFile(const TCHAR* file)
{
    CString dst_file = file;

    m_LstReplayFile.Empty();
    m_RichBox.Clear();

    FMemFile pf(dst_file, _T("rb"));
    if (pf.FileSize() <= 0)
        return;

    EReplayDataType dataType;
    CString buf;
    bool read_ok = false;
    while (!read_ok)
    {
        if (!pf.Read(&dataType))
            break;
        CString line;
        switch (dataType)
        {
        case ERDT_Network:
            {
                FReplayDataNetwork network;
                if (!network.Read(pf))
                {
                    read_ok = true;
                    break;
                }
                network.ToString(line);
            }
        	break;
        case ERDT_Input:
            {
                FReplayDataInput input;
                if (!input.Read(pf))
                {
                    read_ok = true;
                    break;
                }
                input.ToString(line);
            }
            break;
        case ERDT_ScreenRes:
            {
                FReplayDataScreenRes screen;
                if (!screen.Read(pf))
                {
                    read_ok = true;
                    break;
                }
                screen.ToString(line);
            }
            break;
        default:
            read_ok = true;
        }
        if (!line.IsEmpty())
        {
            buf += line;
            buf += _T("\n");
        }
    }
    m_RichBox.SetWindowText(buf);
    m_LstReplayFile = dst_file;
}

void CReplayViewDlg::ReBoxSize()
{
    if (!m_RichBox.GetSafeHwnd())
        return;

    CRect rc;
    GetClientRect(&rc);
    rc.InflateRect(-5,-5);
    rc.top += 20;
    m_RichBox.MoveWindow(rc);
}

CReplayViewDlg::CReplayViewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReplayViewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReplayViewDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RICHEDIT, m_RichBox);
}

BEGIN_MESSAGE_MAP(CReplayViewDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_DROPFILES()
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_CHECK_MMOVE, &CReplayViewDlg::OnBnClickedCheckMmove)
    ON_BN_CLICKED(IDC_CHECK_AXIS, &CReplayViewDlg::OnBnClickedCheckAxis)
END_MESSAGE_MAP()


BOOL CReplayViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    DragAcceptFiles(TRUE);

    ::SendMessage(m_RichBox.GetSafeHwnd(), EM_EXLIMITTEXT, 0, 0x3FFFFFF);   // 64M

    ReBoxSize();

    CheckDlgButton(IDC_CHECK_AXIS, BST_CHECKED);
    CheckDlgButton(IDC_CHECK_MMOVE, BST_CHECKED);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CReplayViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CReplayViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CReplayViewDlg::OnDropFiles(HDROP hDropInfo)
{
    CDialogEx::OnDropFiles(hDropInfo);

    TCHAR path[MAX_PATH] = {0};
    int length = DragQueryFile(hDropInfo, 0, path, MAX_PATH);
    if (length > 0)
        ReplayFile(path);
    DragFinish(hDropInfo);
}

void CReplayViewDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    ReBoxSize();
}

void CReplayViewDlg::OnBnClickedCheckMmove()
{
    if (!m_LstReplayFile.IsEmpty())
        ReplayFile(m_LstReplayFile);
}

void CReplayViewDlg::OnBnClickedCheckAxis()
{
    OnBnClickedCheckMmove();
}
