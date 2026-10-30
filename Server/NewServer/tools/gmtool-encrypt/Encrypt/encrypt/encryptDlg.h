
// encryptDlg.h : header file
//

#pragma once
#include <memory>
namespace gm_file { enum class Operation; }


// CencryptDlg dialog
class CencryptDlg : public CDialogEx
{
// Construction
public:
	CencryptDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ENCRYPT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
    virtual void ShowOperationResult(bool success,const CString& message);
private:
    struct OperationState { bool running=false; };
    std::shared_ptr<OperationState> m_operation=std::make_shared<OperationState>();
    void TransformSelectedFile(gm_file::Operation operation);
public:
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton1();
};
