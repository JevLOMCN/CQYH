/*====================================================================
	Created Date  :		2005-8-24
	
	Specification :  
====================================================================*/
#pragma once

#include <afxinet.h>
#include "BmpRgnButton.h"
#include "DhtmlEdit.h"

#include "../ViGenerator/PatchHelper.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "TransparentCaption.h"
#include <set>

#ifdef _DEBUG
#	define ENABLE_HTTS_DOWNLOAD 0
#endif

#ifndef ENABLE_HTTS_DOWNLOAD
#	define ENABLE_HTTS_DOWNLOAD 1
#endif

struct PakFileIndex
{
	int iPakIndex;
	unsigned int iFileIndex;

	PakFileIndex()
		: iPakIndex(-1), iFileIndex(0)
	{
	}

	PakFileIndex(int p, unsigned int f)
	{
		iPakIndex = p;
		iFileIndex = f;
	}

	bool operator < (const PakFileIndex& rhs) const
	{
		return iPakIndex < rhs.iPakIndex;
	}
};

enum eSubDirType
{
	ESDT_PACK,
	ESDT_NOT_PACK,
	ESDT_NOT_SURE,
};

class CPatchMPQDlg : public CDialog
{
public:
	CPatchMPQDlg(CWnd* pParent = NULL);	// 标准构造函数

	enum { IDD = IDD_PATCH_MPQ_DIALOG };

	BOOL PatchSuccess() const { return _isPatchSuccess; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL OnInitDialog();

	int	StartProcess(const CString& strExeFilePatchName);
	void WaitForAllWorks(class CThreadPool* thrdPool, const CString& msg = _T("工作线程数量:%d"));

	void InitPatchDir();
	bool Patch(const char* pszSrcPath, const char* pszDeltaPath, const char* pszShareSubDir);
	bool PatchPack(const char* pSrcDir, const char* pDeltaDir, const char* pSubDir);
	bool PatchFile(const char* pSrcFile, const char* pDeltaFile, const char* pSubDir, const char* pFileNakedName);
	eSubDirType GetSubDirType(const char* pSubDir);
	bool IsDeltaFile(const char* pszFilePath);
	bool IsNewFile(const char* pszFilePath);

	// 生成的消息映射函数
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedMin();
	afx_msg void OnBnClickedInstallPath();
	afx_msg void OnBnClickedInstall();
	afx_msg LRESULT OnBeginPatch(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	BOOL _isPatchSuccess;

	HICON _hIcon;
	CBitmap _bmpBk;

	CString _strTmpDir;
	CString _strInstallDir;

	CString _strPath;
	CString _statusCurrent;
	CString	_statusTotal;
	CString	_statusDetail;

	CEdit _editPath;
	CProgressCtrl _progressCurrent;

	TransparentCaption _infoCurrent;
	TransparentCaption _infoTotal;
	TransparentCaption _infoDetail;

	BmpRgnButton _btnMin;
	BmpRgnButton _btnClose;

	CButton _btInstallPath;
	BmpRgnButton _btInstall;
	int _countDownTime;

	CString _strPatchConfigFile;
	CString _strVersionLocalConfigFile;

	CTime _startDownloadTime;
	CTime _endDownloadTime;

	typedef std::vector<std::string> SubDirList;
	SubDirList _nonePackSubDirList;
	SubDirList _packSubDirList;

	typedef std::map<std::string, std::string> FilePathList;
	FilePathList _deltaFileList;
	FilePathList _newFileList;
	FilePathList _deleteFileList;
	typedef std::map<std::string, std::string> ResultMD5List;
	ResultMD5List _resFileList;

	int _progressTotal;
	int _progressCount;

	CString _curClientVer;
	CString _strVersionBegin;
	CString _strVersionEnd;
	CString _strDeltaFilePath;
};
