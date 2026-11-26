#include "MMOFacade.h"
#include "WebHtmlManager.h"
#include <climits>
#include <string>
#include <vector>
#include "MMOMsgButtonManager.h"

extern HWND GGameWindow;
extern INT   g_loginAreaID;
extern BYTE  g_loginServerGroup;

WebHtmlManager::WebHtmlManager(void)
	: pFunInitWebWindowTitle(NULL), pFunRedrawWindow(NULL),
	  _OpenGmBallDelegate(EC_EventParm), _webHwnd(NULL)
{
	_webDll = NULL;
	pFunInitWebWindowDll = NULL;
	pFunOpenUrl = NULL;
	pFunShutDownDll = NULL;
	_newWebUrl.Empty();

	//
	pFunSDRS_Encrypt = NULL;
	pFunSDRS_Decrypt = NULL;
	pFunSDRS_Release = NULL;
	_GameServiceEncryptDll = NULL;
}
WebHtmlManager::~WebHtmlManager(void)
{
	_webDll = NULL;
	pFunInitWebWindowDll = NULL;
	pFunOpenUrl = NULL;
	pFunShutDownDll = NULL;
}
void WebHtmlManager::SetOpenGmBallDelegate(FScriptDelegate fun)
{
	_OpenGmBallDelegate = fun;
}
void WebHtmlManager::OpenNewSurvey()
{
	if (_newWebUrl.Len() > 0)
	{
		OpenUrl(_newWebUrl);
		_newWebUrl.Empty();
	}
}
bool WebHtmlManager::AddNewUrlMessageBox(FString url)
{
	/*
	unlock level already restricted
	*/
	UMMOUIMenuBar* menuBar = Cast<UMMOUIMenuBar>(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Skillbar_Menubar));
	if (!menuBar)
	{
		return false;
	}
	/*BOOL bGMSHow = menuBar->IsMenuActivated(EMenuType_GMWebPage);
	if(!bGMSHow)
	return true;*/

	INT nPos = url.InStr(TEXT("http://"));
	if(nPos >= 0)
	{
		_newWebUrl = url.Mid(nPos);
		nPos = _newWebUrl.InStr(TEXT("¡¯"));
		if(nPos > 0)
			_newWebUrl = _newWebUrl.Replace(TEXT("¡¯"),TEXT(""));
		_newWebUrl = GetWholePath(_newWebUrl);
		MMOMsgButtonManager_Parms Parms(EC_EventParm);
		MMOMsgButtonManager::getSingleton().AddNewButton(_OpenGmBallDelegate, Parms, E_MsgBtnShow_GMBall );
		return true;
	}
	return false;
}
struct WebHTMLContent
{
	FString strContent;
	const FString strFileName;
	const FString strFileNewName;
	const FString strReplaceWholeTag;
	const FString strRepHrefUrlTag;
	const FString strRepUrlTitleTag;
	//const FString strSection;
	FString GetSeclectionTemplateHTML();
public:
	BOOL RepalceUrl(TArray<FString>& arrUrl);
	void DeleteFile();
	WebHTMLContent();
private:
	BOOL HtmlReadFile(const FString& fileName, FString& content) const;
	BOOL HtmlSaveFile(const FString& fileName, const FString& content) const;
	BOOL LoadWebPage();
	BOOL SaveWebPage();
	BOOL EnCodeUrl(FString url,FString& strReplace);
} g_WebConetent;


WebHTMLContent::WebHTMLContent()
	: strFileName(TEXT("surveylist.htm"))
	, strFileNewName(TEXT("tempsurveylist.htm"))
	, strReplaceWholeTag(TEXT("$webcontentstring$"))
	, strRepHrefUrlTag(TEXT("$HrefUrl$"))
	, strRepUrlTitleTag(TEXT("$UrlTitle$"))
	//, strSection(TEXT("<li><div class= \"form_list\"><div class=\"list_tit\">$UrlTitle$</div><div class=\"clearit\"></div></div><div class=\"form_btn\"><a target=\"_self\" href=\"$HrefUrl$\"><img src=\"./surveylist_files/btn.jpg\"></a></div></li>"))
{
	strContent.Empty();
}
BOOL WebHTMLContent::RepalceUrl(TArray<FString>& arrUrl)
{
	if(arrUrl.Num() == 0)
		return FALSE;
	//load template
	LoadWebPage();

	//replace content
	FString wholeReplace;
	for (INT n = 0;  n< arrUrl.Num(); ++n)
	{
		FString strReplace;
		EnCodeUrl(arrUrl(n),strReplace);
		wholeReplace += strReplace;
	}
	strContent = strContent.Replace(*strReplaceWholeTag,*wholeReplace);
	INT nPos = strContent.InStr(TEXT("<!DOCTYPE"));
	if(nPos > 0)
	{
		strContent = strContent.Mid(nPos);
	}
	//generate new html
	return SaveWebPage();
}
BOOL WebHTMLContent::EnCodeUrl(FString url,FString& strReplace)
{
	strReplace = GetSeclectionTemplateHTML();
	strReplace = strReplace.Replace(*strRepHrefUrlTag,*url);
	strReplace = strReplace.Replace(*strRepUrlTitleTag,TEXT("System Notice"));
	return TRUE;
}
BOOL WebHTMLContent::LoadWebPage()
{
	FString PathName = GFileManager->ConvertToAbsolutePath(TEXT("..\\..\\MMOGame\\Account\\surveyfile\\surveylist.htm"));
	
	strContent = TEXT("");
	UBOOL bResult = HtmlReadFile(PathName, strContent);
	//UBOOL bResult = appLoadFileToString( strContent, *PathName, GFileManager, 0);

	return bResult;
}

BOOL WebHTMLContent::HtmlReadFile(const FString& fileName, FString& content) const
{
	HANDLE hFile = ::CreateFile(*fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	LARGE_INTEGER fileSize;
	fileSize.QuadPart = 0;
	if (!GetFileSizeEx(hFile, &fileSize) || fileSize.QuadPart < 0 || fileSize.QuadPart >= MAXINT)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	const DWORD requestedSize = (DWORD)fileSize.QuadPart;
	char* buffer = new char[(size_t)requestedSize + 1];
	if (!buffer)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	DWORD readSize = 0;
	BOOL bResult = ReadFile(hFile, buffer, requestedSize, &readSize, NULL);
	if (bResult && readSize == requestedSize)
	{
		buffer[readSize] = 0;
		content = UTF8_TO_TCHAR(buffer);
	}
	else
	{
		bResult = FALSE;
	}
	delete[] buffer;
	CloseHandle(hFile);
	return bResult;
}

BOOL WebHTMLContent::HtmlSaveFile(const FString& fileName, const FString& content) const
{
	BOOL bResult = FALSE;
	HANDLE hFile = ::CreateFile( *fileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD dwWritten;

		UNICHAR BOM = UNICODE_BOM;
		bResult = WriteFile(hFile, &BOM, sizeof(BOM), &dwWritten, NULL );
		bResult = WriteFile(hFile, *content, content.Len()*sizeof(TCHAR), &dwWritten, NULL );
							
		CloseHandle( hFile );
	}
	return bResult;
}
BOOL WebHTMLContent::SaveWebPage()
{
	UBOOL bResult = TRUE;

	FString PathName = GFileManager->ConvertToAbsolutePath(TEXT("..\\..\\MMOGame\\Account\\surveyfile\\tempsurveylist.htm"));
	//bResult = strContent.Len() > 0 ? appSaveStringToFile( strContent, *PathName ) : TRUE;
	bResult = strContent.Len() > 0 ? HtmlSaveFile(PathName, strContent) : TRUE;
	return bResult;
}
void WebHTMLContent::DeleteFile()
{
	if(strContent.Len() > 0)
	{
		strContent.Empty();

		FString filePath = GFileManager->ConvertToAbsolutePath(TEXT("..\\..\\MMOGame\\Account\\surveyfile\\tempsurveylist.htm"));
		GFileManager->Delete(*filePath);
	}
}

FString WebHTMLContent::GetSeclectionTemplateHTML()
{
	//strSection(TEXT("<li><div class= \"form_list\"><div class=\"list_tit\">$UrlTitle$</div><div class=\"clearit\"></div></div><div class=\"form_btn\"><a target=\"_self\" href=\"$HrefUrl$\"><img src=\"./surveylist_files/btn.jpg\"></a></div></li>"))

	FString PathName = GFileManager->ConvertToAbsolutePath(TEXT("..\\..\\MMOGame\\Account\\surveyfile\\SelectionTemplate.htm"));

	FString content = TEXT("");
	UBOOL bResult = HtmlReadFile(PathName, content);
	return content;
}

struct CursorDragWebWindowCaption 
{
	CursorDragWebWindowCaption()
		: bBenginDrag(false), bBeginDragSize(false), nFrameSizeState(0)
		, nOldCursorPoint(), nOldWndRect(), nLstFramePoint(), nRelHwnd(NULL)
		, nFrameSize0(0), nFrameSize1(0)
	{
		nLstFramePoint.x = -123;
	}
	~CursorDragWebWindowCaption(){}

	bool bBenginDrag;
	bool bBeginDragSize;
	/*
		 |           |
		4|     3	 |  5
	-----|-----------|-----
		 |           |
		1|      	 |2
	-----|-----------|-----
		7|		6	 |	8
		 |			 |
	*/
	INT  nFrameSizeState;//state value as shown in the diagram above
	POINT	nOldCursorPoint;
	RECT	nOldWndRect;
	POINT	nLstFramePoint;
	HWND	nRelHwnd;
	INT     nFrameSize0;
	INT     nFrameSize1;
	void SetHwnd(HWND hwnd);
	void BeginDrag();
	void BeginDragSize(INT nState);
	void EndDrag();
	void DragProcess();
} g_CursorDragWebWindow;

extern void CursorDragWebWindowProcess()
{
	g_CursorDragWebWindow.DragProcess();
}

//
void WebHtmlManager::Init()
{
}
//
void WebHtmlManager::Shutdown()
{
	if(pFunShutDownDll&&_webDll)
	{
		if(_webHwnd)
		{
			CloseWindow(_webHwnd);
			DestroyWindow(_webHwnd);
		}
		pFunShutDownDll();
		FreeLibrary(_webDll);
	}

	//
	UnloadGameServiceEncrypt();
}
void WebHtmlManager::CloseWebPage()
{
	if(_webHwnd)
	{
		CloseWindow(_webHwnd);
		DestroyWindow(_webHwnd);
		_webHwnd = NULL;
	}
}
void WebHtmlManager::NotifyOpenSurveyPage(TArray<FString>& arrPath)
{
	TArray<FString> strArr;
	for(INT n = 0; n < arrPath.Num(); ++n)
	{
		strArr.AddItem(GetWholePath(arrPath(n)));
	}
	g_WebConetent.RepalceUrl(strArr);
	OpenUrl(TEXT(""));
}
void WebHtmlManager::OpenSurveyUrl()
{
	CMessageHandler::GetInstance().Send_WebSurveyPageAck();
}
//===========================================================================================================================
void WebHtmlManager::OpenGameService( FString roleName, INT roleLv )
{
	const FString ipFileName = appGameConfigDir() + TEXT("IP.ini");
	const TCHAR* ipFile = *ipFileName;

	FString url_cfg;
	if( !GConfig->GetString( TEXT("QURL"), TEXT("caihongService"), url_cfg, ipFile ) )
	{
		return;
	}

	BOOL isOK = LoadGameServiceEncrypt();
	if( !isOK )
	{
		return;
	}

	std::string stdStrTemp;
	int tempInt;
	//FString fStrTemp;

	//player account (encrypted)
	url_cfg += TEXT("&id");

	stdStrTemp = FTCHARToANSI( CMessageHandler::GetInstance()._UserName );
	tempInt = stdStrTemp.length();
	char* szEncryptAccount = (char*)pFunSDRS_Encrypt( stdStrTemp.c_str(), tempInt );
	url_cfg += szEncryptAccount;

	pFunSDRS_Release( szEncryptAccount );
	szEncryptAccount = NULL;
	//player level
	url_cfg += TEXT("&gd=");
	url_cfg += appItoa( roleLv );
	//game
	url_cfg += TEXT("&gm=991002359");
	//server region
	url_cfg += TEXT("&ar=");
	url_cfg += appItoa( g_loginAreaID );
	//game server
	url_cfg += TEXT("&sv=");
	url_cfg += appItoa( g_loginServerGroup );
	//character info (encrypted)
	url_cfg += TEXT("&rl=");

	stdStrTemp = FTCHARToANSI( *roleName );
	tempInt = stdStrTemp.length();
	char* szEncryptName = (char*)pFunSDRS_Encrypt( stdStrTemp.c_str(), tempInt );
	url_cfg += szEncryptName;

	////debug
	//stdStrTemp = szEncryptAccount;
	//char *debugChar = (char*)pFunSDRS_Decrypt( stdStrTemp.c_str(), tempInt );
	//pFunSDRS_Release(debugChar);
	////end debug

	pFunSDRS_Release( szEncryptName );
	szEncryptName = NULL;
	//
	url_cfg += TEXT("&ck=&s=chbzgame");

	//OpenUrl( url_cfg, 455+36, 455+70 );
	FString strStyle = TEXT("center|showmodal");
	OpenUrlByBrowser( url_cfg, strStyle, 455 + 36, 455 + 70 );
}
//----------------------------------------------------------------------------------------------------------------
BOOL WebHtmlManager::LoadGameServiceEncrypt()
{
	BOOL bResult = TRUE;

	if( _GameServiceEncryptDll == NULL )
	{
		_GameServiceEncryptDll = LoadLibrary( TEXT("SDRS_Encrypt.dll") );
		if( _GameServiceEncryptDll != NULL)
		{
#pragma warning(push)
#pragma warning(disable:4191)
			pFunSDRS_Encrypt = (FunSDRS_EncryptDll)GetProcAddress( _GameServiceEncryptDll, "SDRS_Encrypt" );
			pFunSDRS_Decrypt = (FunSDRS_DecryptDll)GetProcAddress( _GameServiceEncryptDll, "SDRS_Decrypt" );
			pFunSDRS_Release = (FunSDRS_ReleaseDll)GetProcAddress( _GameServiceEncryptDll, "SDRS_Release" );
#pragma warning(pop)
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
			warnf( NAME_Error, TEXT("=============== [WebHtmlManager::LoadGameServiceEncrypt: Failed to LoadLibrary]") );
		}
	}

	return bResult;
}
//----------------------------------------------------------------------------------------------------------------
void WebHtmlManager::UnloadGameServiceEncrypt()
{
	if( _GameServiceEncryptDll != NULL )
	{
		FreeLibrary( _GameServiceEncryptDll );
		_GameServiceEncryptDll = NULL;
	}
}
//===========================================================================================================================
void WebHtmlManager::OpenBugSubmitUrl()
{
	const FString ipFileName = appGameConfigDir() + TEXT("IP.ini");
	const TCHAR* ipFile = *ipFileName;
	FString url_cfg;
	if (!GConfig->GetString(TEXT("QURL"), TEXT("bugsubmit"), url_cfg, ipFile))
		return;
	OpenUrl(url_cfg);
}
//===========================================================================================================================
extern RECT GPrimaryMonitorWorkRect;
//BOOL CALLBACK MMOEnumWindowsProcIE(HWND hWnd, LPARAM lParam)
//{
//	TCHAR wndTitle[256];
//	::GetWindowText( hWnd, wndTitle, 256 - 1 );
//	FString strWndTitle = wndTitle;
//	if( strWndTitle.InStr( TEXT("Internet Explorer") ) >= 0 ){
//		INT wndWidth = 1024;
//		INT wndHeight = 768;
//
//		INT ieWndX = 0;
//		ieWndX = ( GPrimaryMonitorWorkRect.right - GPrimaryMonitorWorkRect.left - wndWidth ) / 2;
//		ieWndX = ( ieWndX >= 0 ? ieWndX : 0 );
//		INT ieWndY = 0;
//		ieWndY = ( GPrimaryMonitorWorkRect.bottom - GPrimaryMonitorWorkRect.top - wndHeight ) / 2;
//		ieWndY = ( ieWndY >= 0 ? ieWndY : 0 );
//
//		::SetWindowPos( hWnd, NULL, ieWndX, ieWndY, wndWidth, wndHeight, 0 );
//		//::MoveWindow( hWnd, ieWndX, ieWndY, ieWndY, wndWidth, wndHeight, TRUE );
//		//return FALSE;
//	}
//
//	return TRUE;
//}
void WebHtmlManager::OpenGameQuitWeb()
{
	const FString ipFileName = appGameConfigDir() + TEXT("IP.ini");
	const TCHAR* ipFile = *ipFileName;

	FString url_cfg;
	if( !GConfig->GetString( TEXT("QURL"), TEXT("urlquit"), url_cfg, ipFile ) )
	{
		return;
	}

	HKEY hRegKey = NULL;
	LPCTSTR subKey = _T("Software\\Microsoft\\Internet Explorer\\Main");
	INT ret = ::RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ | KEY_SET_VALUE, &hRegKey);
	if (ret == ERROR_SUCCESS)
	{
		LPCTSTR keyName = _T("Window_Placement");
		WINDOWPLACEMENT placement = {0};
		DWORD keyType = 0;
		DWORD dataSize = sizeof(placement);
		ret = ::RegQueryValueEx(hRegKey, keyName, 0, &keyType, (LPBYTE)&placement, &dataSize);
		if (ret == ERROR_SUCCESS && keyType == REG_BINARY && dataSize == sizeof(placement))
		{
			INT wndWidth = 1024;
			INT wndHeight = 768;
			INT ieWndX = (GPrimaryMonitorWorkRect.right - GPrimaryMonitorWorkRect.left - wndWidth) / 2;
			INT ieWndY = (GPrimaryMonitorWorkRect.bottom - GPrimaryMonitorWorkRect.top - wndHeight) / 2;
			ieWndX = (ieWndX >= 0 ? ieWndX : 0);
			ieWndY = (ieWndY >= 0 ? ieWndY : 0);
			placement.length = sizeof(placement);
			placement.rcNormalPosition.left = ieWndX;
			placement.rcNormalPosition.right = ieWndX + wndWidth;
			placement.rcNormalPosition.top = ieWndY;
			placement.rcNormalPosition.bottom = ieWndY + wndHeight;
			placement.showCmd = SW_SHOWNORMAL;
			::RegSetValueEx(hRegKey, keyName, 0, keyType, (const BYTE*)&placement, sizeof(placement));
		}
		RegCloseKey(hRegKey);
	}
	//
	OpenUrlByIE( url_cfg );
	//::EnumWindows( MMOEnumWindowsProcIE, 0 );
}
//===========================================================================================================================
void WebHtmlManager::OpenRechargeYuanBao()
{
	const FString ipFileName = appGameConfigDir() + TEXT("IP.ini");
	const TCHAR* ipFile = *ipFileName;
	FString url_cfg;
	if (!GConfig->GetString(TEXT("QURL"), TEXT("urlRechargeYuanBao"), url_cfg, ipFile))
		return;

	FString strStyle = TEXT("center|showmodal");
	OpenUrlByBrowser(url_cfg,strStyle,449,460);
	//OpenUrlByBrowser(url_cfg);
	//OpenUrl(url_cfg);
}

void WebHtmlManager::OpenRechargeYuanBaoByIE()
{
	const FString ipFileName = appGameConfigDir() + TEXT("IP.ini");
	const TCHAR* ipFile = *ipFileName;
	FString url_cfg;
	if (!GConfig->GetString(TEXT("QURL"), TEXT("urlRechargeYuanBao2"), url_cfg, ipFile))
		return;

	OpenUrlByIE(url_cfg);
}

void WebHtmlManager::OpenSellYuanBao()
{
	const FString ipFileName = appGameConfigDir() + TEXT("IP.ini");
	const TCHAR* ipFile = *ipFileName;
	FString url_cfg;
	if (!GConfig->GetString(TEXT("QURL"), TEXT("urlSellYuanBao"), url_cfg, ipFile))
		return;

	INT pos = url_cfg.InStr( TEXT("$gaid$") , FALSE , FALSE );
	FString left  = url_cfg.Left(pos);
	FString right = url_cfg.RightChop( pos + appStrlen("$gaid$") );
	FString strID = FString::Printf(TEXT("%d"),g_loginAreaID);
	url_cfg = left + strID + right;
	FString strStyle = TEXT("center|showmodal");
	OpenUrlByBrowser(url_cfg,strStyle,1024,768);
	//OpenUrl(url_cfg);
}

void WebHtmlManager::OpenGManagerByIE()
{
	const FString ipFileName = appGameConfigDir() + TEXT("IP.ini");
	const TCHAR* ipFile = *ipFileName;
	FString url_cfg;
	if (!GConfig->GetString(TEXT("QURL"), TEXT("urlGManager"), url_cfg, ipFile))
		return;

	OpenUrlByIE(url_cfg);
}
void WebHtmlManager::OpenDynamicHelpByIE()
{
	const FString ipFileName = appGameConfigDir() + TEXT("IP.ini");
	const TCHAR* ipFile = *ipFileName;
	FString url_cfg;
	if (!GConfig->GetString(TEXT("QURL"), TEXT("urlDynamicHelp"), url_cfg, ipFile))
		return;

	OpenUrlByIE(url_cfg);
}
extern UBOOL SDOOpenWindow(LPCWSTR pwcsWinType, LPCWSTR pwcsWinName, LPCWSTR pwcsSrc, int nLeft, int nTop,int nWidth, int nHeight, LPCWSTR pwcsMode);

void WebHtmlManager::OpenUrlByIE(FString strUrl)
{
	HINSTANCE insIE = ShellExecute(NULL,TEXT("Open"),TEXT("iexplore.exe"),*strUrl,NULL,SW_SHOWNORMAL);
	HINSTANCE noIE =  (HINSTANCE)ERROR_FILE_NOT_FOUND;

	if(insIE==noIE)
	{
		ShellExecute(NULL,TEXT("Open"),*strUrl,NULL,NULL,SW_SHOWNORMAL);
	}
}

void WebHtmlManager::OpenUrlByBrowser(FString strUrl,FString strStyle,INT w,INT h)
{
	
	UBOOL bRes = SDOOpenWindow(TEXT(""), TEXT(""), *strUrl, 0, 0, w, h, *strStyle);// TEXT("BROWSER"));	// xul=WebBrowser.xml;sf=true;cp=true;ap=center"));

	if(bRes==FALSE)
	{
		OpenUrlByIE(strUrl);
		//OpenUrl(strUrl, w, h);
	}
}

void WebHtmlManager::OpenUrl(FString strUrl,INT w/*=920*/,INT h/*=580*/)
{
	if(0)
		AddNewUrlMessageBox(strUrl);
	if(_webDll == NULL)
	{
		_webDll = LoadLibrary(TEXT("webManager.dll"));
		if(_webDll != NULL)
		{
			#pragma warning(push)
			#pragma warning(disable:4191)
			pFunInitWebWindowDll = (FunInitWebWindowDll)GetProcAddress(_webDll,"dllCreatWebWindow");
			pFunOpenUrl = (FunOpenWebUrl)GetProcAddress(_webDll,("dllOpenUrl"));
			pFunShutDownDll = (FunShutDownWebDll)GetProcAddress(_webDll,("dllReleaseWeb"));
			pFunRedrawWindow = (FunRedrawWebWindow)GetProcAddress(_webDll,("dllReDrawWebWindow"));
			pFunInitWebWindowTitle = (FunInitWebWindowTitle)GetProcAddress(_webDll,("dllCreatWebWindowTitle"));
			#pragma warning(pop)
		}
		else
		{
			  warnf( NAME_Error, TEXT("=============== [WebHtmlManager::OpenUrl: Failed to LoadLibrary]") );
		}
	}

	if(pFunInitWebWindowDll)
	{
		if(_webHwnd)
		{
			CloseWindow(_webHwnd);
			DestroyWindow(_webHwnd);
			_webHwnd = NULL;
		}
		_webHwnd = pFunInitWebWindowDll(GGameWindow, (UINT)w, (UINT)h);

		if (_webHwnd == NULL)
		{
			warnf( NAME_Error, TEXT("=============== [WebHtmlManager::OpenUrl: Failed to CreateWebWnd]") );
		}
// 		else
// 		{
// 			//::SetWindowPos(_webHwnd, HWND_TOPMOST, 0, 0, w, h, SWP_NOMOVE|SWP_FRAMECHANGED);
// 			RECT rcWebWindow,rcGameWindow;
// 			GetWindowRect(GGameWindow, &rcGameWindow);
// 			GetWindowRect(_webHwnd, &rcWebWindow);
// 			INT centerX = (rcGameWindow.right - rcGameWindow.left - w) / 2;
// 			INT centerY = (rcGameWindow.bottom - rcGameWindow.top - h) / 2;
// 			::MoveWindow(_webHwnd, centerX, centerY, w, h, FALSE);
// 		}

		if(pFunInitWebWindowTitle)
		{
			pFunInitWebWindowTitle(NULL,TEXT(""),TEXT(""));
		}

		g_CursorDragWebWindow.SetHwnd(_webHwnd);

		SetWindowLongPtr(_webHwnd, GWLP_WNDPROC, (LONG_PTR)StaticWebWndProc);

		//DWORD dwStyle = ::GetWindowLong(_webHwnd, GWL_STYLE);
		////DWORD dwNewStyle = (dwStyle & ~WS_CHILD) | WS_POPUP;
		//DWORD dwNewStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		//warnf( NAME_Log, TEXT("Web window style : %X"), dwStyle );
		//::SetWindowLong(_webHwnd, GWL_STYLE, dwNewStyle);
		//warnf( NAME_Log, TEXT("Web window new style : %X"), ::GetWindowLong(_webHwnd, GWL_STYLE) );

		LONG dwWinStyle = ::GetWindowLong(GGameWindow,GWL_STYLE);
		dwWinStyle |= WS_CLIPCHILDREN;
		::SetWindowLong(GGameWindow,GWL_STYLE,dwWinStyle);
	}
	else
	{
		warnf( NAME_Error, TEXT("=============== [WebHtmlManager::OpenUrl: pFunInitWebWindowDll == NULL]") );
	}

	if(pFunOpenUrl)
	{
		if (strUrl.Len() == 0)
		{
			FString filePath = GFileManager->ConvertToAbsolutePath(TEXT("..\\..\\MMOGame\\Account\\surveyfile\\tempsurveylist.htm"));
			pFunOpenUrl(*filePath);
		}
		else
		{
			pFunOpenUrl(*strUrl);
		}
	}	
	else
	{
		warnf( NAME_Error, TEXT("=============== [WebHtmlManager::OpenUrl: pFunOpenUrl == NULL]") );
	}

}
void WebHtmlManager::RedrawWebWindow()
{
	if(pFunRedrawWindow)
		pFunRedrawWindow();
}

//extern bool g_isLogSuccess;
//extern bool g_isSDOLogin;
//extern void ShowSDOLoginDialogWnd(bool bForce);

LRESULT APIENTRY WebHtmlManager::StaticWebWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	//UBOOL isShowLogin = (g_isSDOLogin && !g_isLogSuccess);

	switch(Message)
	{
	case WM_SYSCOMMAND:
		{
			switch( wParam & 0xfff0 )
			{
			case SC_MOVE:
				{
					g_CursorDragWebWindow.BeginDrag();
					return TRUE;
				}
				break;
			case SC_SIZE:
				{
					g_CursorDragWebWindow.BeginDragSize(wParam & 0x000f);
					return TRUE;
				}
				break;
				//case SC_CLOSE:
				//	{
				//		if (isShowLogin && !g_isSDOLoginWnd )
				//		{
				//			ShowSDOLoginDialogWnd(true);
				//		}
				//	}
				//	break;
			}
			if(wParam == 1)
			{
				//g_WebConetent.DeleteFile();
				return TRUE;
			}
		}
		break;
	case WM_COMMAND:
		{
			switch( wParam & 0xfff0 )
			{
			case SC_CLOSE:
				{
					SendMessage(hWnd,WM_SYSCOMMAND,SC_CLOSE,lParam);
					return TRUE;
				}
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			SendMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,lParam);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		{
			//if (isShowLogin)
			//{
			//	ShowSDOLoginDialogWnd(true);
			//}
		}
		break;
	}

	return DefWindowProc( hWnd, Message, wParam, lParam );
}

unsigned char WebHtmlManager::QURL_ToHex(unsigned char x) 
{ 
	return  x > 9 ? x + 55 : x + 48; 
}
std::string WebHtmlManager::QURL_UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) || 
			(str[i] == '-') ||
			(str[i] == '_') || 
			(str[i] == '.') || 
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += QURL_ToHex((unsigned char)str[i] >> 4);
			strTemp += QURL_ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}



//percent encoding
//http://zh.wikipedia.org/zh-cn/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81

BOOL WebHtmlManager::UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase)
{
	if (!szSrc || !pBuf || cbBufLen <= 0)
		return FALSE;
	const size_t sourceLength = strlen(szSrc);
	if (sourceLength > INT_MAX)
	{
		pBuf[0] = 0;
		return FALSE;
	}
	const std::string input(szSrc, sourceLength);
	pBuf[0] = 0;
	if (input.empty())
		return TRUE;
	int wideCount = MultiByteToWideChar(CP_ACP, 0, input.c_str(), (int)input.size(), NULL, 0);
	if (wideCount <= 0)
		return FALSE;
	std::vector<WCHAR> wide(wideCount);
	if (MultiByteToWideChar(CP_ACP, 0, input.c_str(), (int)input.size(), &wide[0], wideCount) != wideCount)
		return FALSE;
	int utf8Count = WideCharToMultiByte(CP_UTF8, 0, &wide[0], wideCount, NULL, 0, NULL, NULL);
	if (utf8Count <= 0)
		return FALSE;
	std::string utf8(utf8Count, '\0');
	if (WideCharToMultiByte(CP_UTF8, 0, &wide[0], wideCount, &utf8[0], utf8Count, NULL, NULL) != utf8Count)
		return FALSE;

	const char* hex = bUpperCase ? "0123456789ABCDEF" : "0123456789abcdef";
	std::string result;
	const size_t capacity = (size_t)cbBufLen - 1;
	for (size_t index = 0; index < utf8.size(); ++index)
	{
		const unsigned char c = (unsigned char)utf8[index];
		const bool plain = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
			(c >= '0' && c <= '9') || c == '-' || c == '.' || c == '~';
		const size_t width = (plain || c == ' ') ? 1 : 3;
		if (width > capacity - result.size())
			return FALSE;
		if (plain)
			result += c;
		else if (c == ' ')
			result += '+';
		else
		{
			result += '%';
			result += hex[c >> 4];
			result += hex[c & 15];
		}
	}
	memcpy(pBuf, result.c_str(), result.size() + 1);
	return TRUE;
}

//after decoding it is utf-8 encoded
BOOL WebHtmlManager::UrlDecode(const char* szSrc, char* pBuf, int cbBufLen)
{
	if (!szSrc || !pBuf || cbBufLen <= 0)
		return FALSE;
	const size_t sourceLength = strlen(szSrc);
	if (sourceLength > INT_MAX)
	{
		pBuf[0] = 0;
		return FALSE;
	}
	const std::string input(szSrc, sourceLength);
	pBuf[0] = 0;
	if (input.empty())
		return TRUE;

	std::string utf8;
	for (size_t index = 0; index < input.size(); ++index)
	{
		const unsigned char c = (unsigned char)input[index];
		if (c == '%')
		{
			if (input.size() - index < 3)
				return FALSE;
			const char h = input[index + 1];
			const char l = input[index + 2];
			const int high = h >= '0' && h <= '9' ? h - '0' :
				(h >= 'A' && h <= 'F' ? h - 'A' + 10 : (h >= 'a' && h <= 'f' ? h - 'a' + 10 : -1));
			const int low = l >= '0' && l <= '9' ? l - '0' :
				(l >= 'A' && l <= 'F' ? l - 'A' + 10 : (l >= 'a' && l <= 'f' ? l - 'a' + 10 : -1));
			if (high < 0 || low < 0)
				return FALSE;
			utf8 += (char)((high << 4) | low);
			index += 2;
		}
		else
			utf8 += c == '+' ? ' ' : (char)c;
	}

	int wideCount = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.c_str(), (int)utf8.size(), NULL, 0);
	if (wideCount <= 0)
		return FALSE;
	std::vector<WCHAR> wide(wideCount);
	if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.c_str(), (int)utf8.size(), &wide[0], wideCount) != wideCount)
		return FALSE;
	int ansiCount = WideCharToMultiByte(CP_ACP, 0, &wide[0], wideCount, NULL, 0, NULL, NULL);
	if (ansiCount <= 0 || ansiCount >= cbBufLen)
		return FALSE;
	std::string result(ansiCount, '\0');
	if (WideCharToMultiByte(CP_ACP, 0, &wide[0], wideCount, &result[0], ansiCount, NULL, NULL) != ansiCount)
		return FALSE;
	memcpy(pBuf, result.c_str(), result.size() + 1);
	return TRUE;
}

FString WebHtmlManager::GetWholePath(const FString& path)
{
	FString wholeStr = path;
 	FTCHARToUTF8 name_cov(*FString(MMOFacade::getSingleton().GetMainPlayer()->ActorName));
 	std::string name_ansi = (ANSICHAR*)name_cov;
 	std::string name_safe = QURL_UrlEncode(name_ansi);
 	FANSIToTCHAR name_tchar(name_safe.c_str());

	//game ID
	wholeStr += _T("&appId=991002359");

	//player account name
	wholeStr += _T("&ptId=");
	wholeStr += CMessageHandler::GetInstance()._UserName;

	wholeStr += _T("&sndaId=");
	wholeStr += CMessageHandler::GetInstance()._UserName;

	wholeStr += FString::Printf(_T("&areaId=%d"),g_loginAreaID);

	wholeStr += FString::Printf(_T("&groupId=%d"),g_loginServerGroup);

	wholeStr += _T("&roleName=");
	wholeStr += name_tchar;

	warnf(NAME_Log, TEXT("URL:%s"), *wholeStr);

	return wholeStr;
}

void CursorDragWebWindowCaption::SetHwnd(HWND hwnd)
{
	EndDrag();
	nRelHwnd = hwnd;
	nFrameSize0 = 0;
	nFrameSize1 = 0;
	RECT rcGame = {0}, rcClient = {0};
	if (!hwnd || !GetWindowRect(GGameWindow, &rcGame) || !GetClientRect(GGameWindow, &rcClient))
		return;
	nFrameSize0 = (rcGame.right - rcGame.left - rcClient.right + rcClient.left)/2;
	nFrameSize1 = (rcGame.bottom - rcGame.top - rcClient.bottom + rcClient.top - nFrameSize0);
}
void CursorDragWebWindowCaption::BeginDrag()
{
	EndDrag();
	if (!nRelHwnd || !GetCursorPos(&nOldCursorPoint) ||
		!ScreenToClient(GGameWindow, &nOldCursorPoint) || !GetWindowRect(nRelHwnd, &nOldWndRect))
		return;
	nLstFramePoint = nOldCursorPoint;
	bBenginDrag = true;
}

void CursorDragWebWindowCaption::BeginDragSize(INT nState)
{
	EndDrag();
	if (nState < 1 || nState > 8 || !nRelHwnd || !GetCursorPos(&nOldCursorPoint) ||
		!ScreenToClient(GGameWindow, &nOldCursorPoint) || !GetWindowRect(nRelHwnd, &nOldWndRect))
		return;
	nLstFramePoint = nOldCursorPoint;
	nFrameSizeState = nState;
	bBeginDragSize = true;
}
void CursorDragWebWindowCaption::EndDrag()
{ 
	bBenginDrag = false;
	bBeginDragSize = false;
	nFrameSizeState = 0;
}
void CursorDragWebWindowCaption::DragProcess()
{
	if(nRelHwnd == NULL)
	{
		EndDrag();
		return;
	}
	bool bRedraw = false;
	if(bBenginDrag)
	{
		POINT nNewCursorPoint = {0};
		if (!GetCursorPos(&nNewCursorPoint) || !ScreenToClient(GGameWindow, &nNewCursorPoint))
		{
			EndDrag();
			return;
		}

		if (nLstFramePoint.x != nNewCursorPoint.x || nLstFramePoint.y != nNewCursorPoint.y)
		{
			RECT rcGame = {0}, rcClient = {0};
			if (!GetWindowRect(GGameWindow, &rcGame) || !GetClientRect(GGameWindow, &rcClient))
			{
				EndDrag();
				return;
			}
			if(PtInRect(&rcClient,nNewCursorPoint))
			{
				const INT windowX = nOldWndRect.left + nNewCursorPoint.x - nOldCursorPoint.x - rcGame.left - nFrameSize0;
				const INT windowY = nOldWndRect.top + nNewCursorPoint.y - nOldCursorPoint.y - rcGame.top - nFrameSize1;
				MoveWindow(nRelHwnd, windowX, windowY,
					nOldWndRect.right - nOldWndRect.left,  nOldWndRect.bottom - nOldWndRect.top, TRUE);
			}
			nLstFramePoint = nNewCursorPoint;
			bRedraw = true;
		}
	}
	else if (bBeginDragSize)
	{
		POINT nNewCursorPoint = {0};
		if (!GetCursorPos(&nNewCursorPoint) || !ScreenToClient(GGameWindow, &nNewCursorPoint))
		{
			EndDrag();
			return;
		}
		if (nLstFramePoint.x != nNewCursorPoint.x || nLstFramePoint.y != nNewCursorPoint.y)
		{
			RECT rcWnd = {0};
			if (!GetWindowRect(nRelHwnd, &rcWnd))
			{
				EndDrag();
				return;
			}

			INT nPosX = 0,nPosY = 0,nWidth = 0,nHeight = 0;
			if(nFrameSizeState == 4)
			{
				nWidth = nLstFramePoint.x - nNewCursorPoint.x;
				nHeight = nLstFramePoint.y - nNewCursorPoint.y;
				nPosX = -nWidth;
				nPosY = -nHeight;
			}
			else if(nFrameSizeState == 3)
			{
				nHeight = nLstFramePoint.y - nNewCursorPoint.y;
				nPosY = -nHeight;
			}
			else if(nFrameSizeState == 5)
			{
				nWidth =  - nLstFramePoint.x + nNewCursorPoint.x;
				nHeight = nLstFramePoint.y - nNewCursorPoint.y;
				nPosY = -nHeight;
			}
			else if(nFrameSizeState == 2)
			{
				nWidth =  - nLstFramePoint.x + nNewCursorPoint.x;
			}
			else if(nFrameSizeState == 8)
			{
				nWidth = - nLstFramePoint.x + nNewCursorPoint.x;
				nHeight = - nLstFramePoint.y + nNewCursorPoint.y;
			}
			else if(nFrameSizeState == 6)
			{
				nHeight = - nLstFramePoint.y + nNewCursorPoint.y;
			}
			else if(nFrameSizeState == 7)
			{
				nWidth = nLstFramePoint.x - nNewCursorPoint.x;
				nHeight = - nLstFramePoint.y + nNewCursorPoint.y;
				nPosX = -nWidth;
			}
			else if(nFrameSizeState == 1)
			{
				nWidth = nLstFramePoint.x - nNewCursorPoint.x;
				nPosX = -nWidth;
			}
			RECT rcGame = {0};
			if (!GetWindowRect(GGameWindow, &rcGame))
			{
				EndDrag();
				return;
			}
			MoveWindow(nRelHwnd,rcWnd.left + nPosX - rcGame.left - nFrameSize0, rcWnd.top + nPosY - rcGame.top - nFrameSize1, 
				rcWnd.right - rcWnd.left + nWidth,rcWnd.bottom - rcWnd.top + nHeight, TRUE);
			nLstFramePoint = nNewCursorPoint;
			bRedraw = true;
		}
	}
	if(bRedraw)
	{
		::RedrawWindow(nRelHwnd,NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ALLCHILDREN);
		WebHtmlManager::getSingleton().RedrawWebWindow();
	}
	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		EndDrag();
}