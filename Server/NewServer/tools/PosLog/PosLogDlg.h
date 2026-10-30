#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "common_log.h"
#include <functional>
#include <memory>

class CPositionCanvas:public CButton
{
public:
    std::function<void(HDC,const CRect&)> renderer;
    void DrawItem(LPDRAWITEMSTRUCT item) override;
};
class CPosLogDlg:public CDialogEx
{
public:
    explicit CPosLogDlg(CWnd* parent=nullptr);
    enum { IDD=IDD_POSLOG_DIALOG };
    afx_msg void OnBnClickedButton1();
    afx_msg void OnNMClickList2(NMHDR*,LRESULT* result);
    afx_msg void OnLvnItemchangedList2(NMHDR*,LRESULT* result);
    afx_msg void OnCbnSelchangeCombo1();
    afx_msg void OnBnClickedSaveBmp();
protected:
    void DoDataExchange(CDataExchange* exchange) override;
    BOOL OnInitDialog() override;
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual std::optional<std::filesystem::path> ChooseLogFile();
    virtual std::optional<std::filesystem::path> ChooseBitmapFile();
    virtual void ReportError(const std::string& message);
    DECLARE_MESSAGE_MAP()
private:
    struct State
    {
        poslog::LogData data;
        std::vector<poslog::GroupKey> rows;
        std::optional<poslog::GroupKey> selected;
        std::uint32_t bitmap_side{};
        bool loaded=false,busy=false,publishing=false;
    };
    std::shared_ptr<State> m_state=std::make_shared<State>();
    HICON m_hIcon{};
    CListCtrl m_SceneList;
    CPositionCanvas m_DrawCanvas;
    CComboBox m_PicSize;
    CButton m_SaveAsBmp;
    void UpdateSaveState();
};
