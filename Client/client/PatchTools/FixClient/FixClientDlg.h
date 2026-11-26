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

#ifdef _DEBUG
#	define ENABLE_HTTS_DOWNLOAD 0
#endif

#ifndef ENABLE_HTTS_DOWNLOAD
#	define ENABLE_HTTS_DOWNLOAD 1
#endif

bool HttpsDownload(const CString& strURL, const CString& targetFilePath, CInternetSession* sess = NULL);


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

class CFixClientDlg : public CDialog
{
public:
	CFixClientDlg(CWnd* pParent = NULL);	// 标准构造函数

	enum { IDD = IDD_FIXCLIENT_DIALOG };

	BOOL PatchSuccess() const { return _isPatchSuccess; }

protected:
	CString					_curClientVer;

	BOOL					_isPatchSuccess;
	CString					_fullServerURL;
	CString					_firstDownloadFile;
	CString					_serverIP;
	INTERNET_PORT			_serverPort;
	CString					_initURLRoot;

	CInternetSession*		_internetSession;
	CHttpConnection			*_httpConnectionPtr;

	HICON					_hIcon;
	CBitmap					_bmpBk;

	int						_totalUpdate;
	int						_curUpdate;

	CString					_strLog;
	CString					_strTmpDir;
	CString					_strInstallDir;

	CString				_statusTotal;
	CString				_statusCurrent;
	CString				_statusDetail;

	CProgressCtrl		_progressTotal;
	CProgressCtrl		_progressCurrent;

	BmpRgnButton		_btnMin;
	BmpRgnButton		_btnClose;

	TransparentCaption _infoCurrent;
	TransparentCaption _infoTotal;
	TransparentCaption _infoDetail;

	BmpRgnButton _btUpdateGame;
	int _countDownTime;

	CString _serverSection;
	CString _strPatchConfigFile;
	CString _strVersionLocalConfigFile;

	CTime _startGenMD5Time;
	CTime _endGenMD5Time;

	CTime _startCompareMD5Time;
	CTime _endCompareMD5Time;


	CTime _startDownloadTime;
	CTime _endDownloadTime;

	typedef std::vector<std::string> SubDirList;
	SubDirList _nonePackSubDirList;
	SubDirList _packSubDirList;

	int _progressCount;

	int						StartProcess(const CString& strExeFilePatchName);

	void					Compare(SFilePatch& filePatch);

	CString					GetTmpFileList() const;
	BOOL					LoadViInfo(VersionInfoes& filePatch, const CString& fileName);
	BOOL					DownLoadServerFile(const CString& section_name, const CString& key_name, const CString& targetFileName);
	BOOL					DownLoadServerFileToString(const CString& section_name, const CString& key_name, CString& out_string);
	void					GenerateLocalMD5();

	void StartFix();
	void DownloadPatch(SFilePatch& filePatch);
	void DownloadPatchOK();
	void WaitForAllWorks(class CThreadPool* thrdPool, const CString& msg = _T("工作线程数量:%d"));

	// add by zhangrui05
	// >>
	void InitPatchDir();
	void DoPatch();
	bool Patch(const char* pszSrcPath, const char* pszDeltaPath, const char* pszShareSubDir);
	bool PatchPack(const char* pSrcDir, const char* pDeltaDir, const char* pSubDir);
	bool PatchFile(const char* pSrcFile, const char* pDeltaFile);
	eSubDirType GetSubDirType(const char* pSubDir);
	bool IsDeltaFile(const char* pszFilePath);
	bool IsNewFile(const char* pszFilePath);
	// <<

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL OnInitDialog();

	// 生成的消息映射函数
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnBeginPatch(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCancel();
	afx_msg void OnBnClickedMin();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedUpdateGame();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()


};
