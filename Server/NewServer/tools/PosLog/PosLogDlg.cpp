#include "stdafx.h"
#include "PosLog.h"
#include "PosLogDlg.h"
#include "afxdialogex.h"
#include "bmp_file.h"
#include <format>
#include <limits>
#include <utility>

namespace
{
struct NativeView
{
    HWND handle;CWnd* owner;
    bool Current() const {return ::IsWindow(handle) && CWnd::FromHandlePermanent(handle)==owner;}
};
template<class T> struct Busy
{
    std::shared_ptr<T> state;
    ~Busy(){state->busy=false;state->publishing=false;}
};
}
void CPositionCanvas::DrawItem(LPDRAWITEMSTRUCT item)
{
    if(!item)return;
    const CRect rectangle(item->rcItem);
    ::FillRect(item->hDC,&item->rcItem,::GetSysColorBrush(COLOR_BTNFACE));
    const auto draw=renderer;if(draw)draw(item->hDC,rectangle);
}
CPosLogDlg::CPosLogDlg(CWnd* parent):CDialogEx(IDD,parent) {}
void CPosLogDlg::DoDataExchange(CDataExchange* exchange)
{
    CDialogEx::DoDataExchange(exchange);DDX_Control(exchange,IDC_LIST2,m_SceneList);
    DDX_Control(exchange,IDC_BUTTON3,m_DrawCanvas);DDX_Control(exchange,IDC_COMBO1,m_PicSize);DDX_Control(exchange,IDC_SAVE_BMP,m_SaveAsBmp);
}
BEGIN_MESSAGE_MAP(CPosLogDlg,CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1,&CPosLogDlg::OnBnClickedButton1)
    ON_NOTIFY(NM_CLICK,IDC_LIST2,&CPosLogDlg::OnNMClickList2)
    ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST2,&CPosLogDlg::OnLvnItemchangedList2)
    ON_CBN_SELCHANGE(IDC_COMBO1,&CPosLogDlg::OnCbnSelchangeCombo1)
    ON_BN_CLICKED(IDC_SAVE_BMP,&CPosLogDlg::OnBnClickedSaveBmp)
END_MESSAGE_MAP()
BOOL CPosLogDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();m_state=std::make_shared<State>();
    m_hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);SetIcon(m_hIcon,TRUE);SetIcon(m_hIcon,FALSE);
    m_SceneList.SetExtendedStyle(m_SceneList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
    m_SceneList.InsertColumn(0,L"Time",LVCFMT_LEFT,50);m_SceneList.InsertColumn(1,L"SceneTempID",LVCFMT_LEFT,100);m_SceneList.InsertColumn(2,L"Count",LVCFMT_LEFT,80);
    m_PicSize.ResetContent();
    for(const auto side:{512,1024})
    {
        const auto label=std::format(L"{}*{}",side,side);const auto index=m_PicSize.InsertString(m_PicSize.GetCount(),label.c_str());
        if(index>=0)m_PicSize.SetItemData(index,side);
    }
    m_DrawCanvas.ModifyStyle(BS_TYPEMASK,BS_OWNERDRAW);
    const std::weak_ptr<State> weak=m_state;
    m_DrawCanvas.renderer=[weak](HDC dc,const CRect& rectangle)
    {
        const auto state=weak.lock();if(!state || !state->selected || rectangle.Width()<=0 || rectangle.Height()<=0)return;
        const auto found=state->data.groups.find(*state->selected);if(found==state->data.groups.end())return;
        for(const auto point:found->second)
            if(const auto pixel=poslog::pixel(point,static_cast<std::uint32_t>(rectangle.Width()),static_cast<std::uint32_t>(rectangle.Height())))
                ::SetPixelV(dc,rectangle.left+static_cast<int>(pixel->column),rectangle.bottom-1-static_cast<int>(pixel->row),RGB(255,0,0));
    };
    UpdateSaveState();return TRUE;
}
void CPosLogDlg::OnPaint()
{
    if(!IsIconic()){CDialogEx::OnPaint();return;}
    CPaintDC dc(this);SendMessage(WM_ICONERASEBKGND,reinterpret_cast<WPARAM>(dc.GetSafeHdc()),0);
    CRect rectangle;GetClientRect(&rectangle);
    dc.DrawIcon((rectangle.Width()-GetSystemMetrics(SM_CXICON))/2,(rectangle.Height()-GetSystemMetrics(SM_CYICON))/2,m_hIcon);
}
HCURSOR CPosLogDlg::OnQueryDragIcon(){return static_cast<HCURSOR>(m_hIcon);}
std::optional<std::filesystem::path> CPosLogDlg::ChooseLogFile()
{
    CFileDialog dialog(TRUE,L"log",nullptr,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,L"Position logs (*.log)|*.log|All files (*.*)|*.*||",this);
    if(dialog.DoModal()!=IDOK)return {};
    const auto path=dialog.GetPathName();return std::filesystem::path(std::wstring(path.GetString(),static_cast<std::size_t>(path.GetLength())));
}
std::optional<std::filesystem::path> CPosLogDlg::ChooseBitmapFile()
{
    CFileDialog dialog(FALSE,L"bmp",nullptr,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,L"Bitmap files (*.bmp)|*.bmp||",this);
    if(dialog.DoModal()!=IDOK)return {};
    const auto path=dialog.GetPathName();return std::filesystem::path(std::wstring(path.GetString(),static_cast<std::size_t>(path.GetLength())));
}
void CPosLogDlg::ReportError(const std::string& message)
{
    const std::wstring text(message.begin(),message.end());MessageBox(text.c_str(),L"PosLog",MB_OK|MB_ICONERROR);
}
void CPosLogDlg::UpdateSaveState()
{
    m_SaveAsBmp.EnableWindow(m_state->loaded && m_state->selected.has_value() && (m_state->bitmap_side==512 || m_state->bitmap_side==1024));
}
void CPosLogDlg::OnBnClickedButton1()
{
    const auto state=m_state;if(std::exchange(state->busy,true))return;Busy guard{state};
    const NativeView view{GetSafeHwnd(),this};
    const auto current=[&]{return view.Current() && m_state==state;};
    try
    {
        const auto filename=ChooseLogFile();if(!current() || !filename)return;
        auto parsed=poslog::load_log(*filename);if(!parsed){ReportError(parsed.error());return;}
        if(parsed->groups.size()>static_cast<std::size_t>((std::numeric_limits<int>::max)())){ReportError("Position log contains too many scene groups.");return;}
        struct Row{poslog::GroupKey key;std::wstring hour,scene,count;};std::vector<Row> rows;std::vector<poslog::GroupKey> keys;
        for(auto entry=parsed->groups.rbegin();entry!=parsed->groups.rend();++entry)
        {
            rows.push_back({entry->first,std::format(L"{}:00",entry->first.hour),std::to_wstring(entry->first.scene),std::to_wstring(entry->second.size())});keys.push_back(entry->first);
        }
        state->publishing=true;state->data=std::move(*parsed);state->rows=std::move(keys);state->selected.reset();state->loaded=true;
        m_SceneList.DeleteAllItems();if(!current())return;
        for(std::size_t index=0;index<rows.size();++index)
        {
            const auto& row=rows[index];const auto item=m_SceneList.InsertItem(static_cast<int>(index),row.hour.c_str());if(!current())return;
            if(item<0){state->loaded=false;ReportError("Cannot publish the scene list.");if(current())UpdateSaveState();return;}
            m_SceneList.SetItemText(item,1,row.scene.c_str());if(!current())return;
            m_SceneList.SetItemText(item,2,row.count.c_str());if(!current())return;
            m_SceneList.SetItemData(item,index);if(!current())return;
        }
        SetDlgItemText(IDC_STATIC_FILE,filename->c_str());if(!current())return;
        state->publishing=false;LRESULT result{};OnLvnItemchangedList2(nullptr,&result);
    }
    catch(const std::exception&){if(current())ReportError("Cannot load the position log.");}
}
void CPosLogDlg::OnNMClickList2(NMHDR*,LRESULT* result){if(result)*result=0;}
void CPosLogDlg::OnLvnItemchangedList2(NMHDR*,LRESULT* result)
{
    if(result)*result=0;const auto state=m_state;if(state->publishing || !state->loaded)return;
    const auto selected=m_SceneList.GetNextItem(-1,LVNI_SELECTED);state->selected.reset();
    if(selected>=0)
    {
        const auto index=m_SceneList.GetItemData(selected);
        if(index<state->rows.size() && state->data.groups.contains(state->rows[index]))state->selected=state->rows[index];
    }
    m_DrawCanvas.Invalidate(FALSE);UpdateSaveState();
}
void CPosLogDlg::OnCbnSelchangeCombo1()
{
    const auto selected=m_PicSize.GetCurSel();const auto value=selected<0 ? 0 : m_PicSize.GetItemData(selected);
    m_state->bitmap_side=value==512 || value==1024 ? static_cast<std::uint32_t>(value) : 0;UpdateSaveState();
}
void CPosLogDlg::OnBnClickedSaveBmp()
{
    const auto state=m_state;if(std::exchange(state->busy,true))return;Busy guard{state};
    const NativeView view{GetSafeHwnd(),this};const auto current=[&]{return view.Current() && m_state==state;};
    try
    {
        if(!state->loaded || !state->selected || (state->bitmap_side!=512 && state->bitmap_side!=1024)){ReportError("Load a log, select a scene and choose a bitmap size first.");return;}
        const auto found=state->data.groups.find(*state->selected);if(found==state->data.groups.end()){ReportError("Selected scene is no longer available.");return;}
        const auto points=found->second;const auto side=state->bitmap_side;
        auto filename=ChooseBitmapFile();if(!current() || !filename)return;
        if(filename->extension().empty())*filename+=".bmp";
        const auto result=poslog::write_bitmap(*filename,points,side);if(!result)ReportError(result.error());
    }
    catch(const std::exception&){if(current())ReportError("Cannot save the bitmap.");}
}
