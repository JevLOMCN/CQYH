#include "LaunchPrivate.h"
#include "SDOLogin.h"
#include "login2Interface.h"

#include "interface.h"
#include "SDOA4Client.h"
#include <strsafe.h>
#include "SplashScreen.h"
#include "..\..\Common\define\version.h"

#pragma warning(push)
#pragma warning(disable : 4191)

extern HWND GGameWindow;
extern FString g_sdoSessionID;

int CALLBACK OnLogin(int nErrorCode, const _SDOLLoginResult* pLoginResult, int nUserData, int nReserved) ;
ISDOLogin2*			_g_SDOLogin = NULL;
HINSTANCE			_g_hInstSDOLDll = 0;

HINSTANCE			g_hInstSDOLDll = 0;
LPSDOLInitialize    SDOLInitialize = NULL;
LPSDOLGetModule     SDOLGetModule = NULL;
LPSDOLTerminal      SDOLTerminal = NULL;

ISDOLLogin*			g_SDOLogin = NULL;
ISDOAApp*			g_SDOApp = NULL;

bool g_isSDOLogin = false;
bool g_isSDOLoginWnd = false;
bool g_isLogSuccess = false;

int g_lastX = 0;
int g_lastY = 0;
int g_loginAreaID = 0;
extern TCHAR g_sdoLoginIP[MAX_PATH];
extern INT g_sdoLoginPort;
extern TCHAR g_sdoLoginIP2[MAX_PATH];
extern INT g_sdoLoginPort2;
extern BYTE g_loginServerGroup;
FString g_loginServerName;

void TryParseLoginIP();

/* release snda login sdk */
void DestroySDOLogin()
{
	if (g_SDOLogin)
	{
		g_SDOLogin->Release();
		g_SDOLogin = NULL;
	}

	if(g_SDOApp)
	{
		g_SDOApp->Release();
		g_SDOApp=NULL;
	}


	if (SDOLTerminal)
	{  // first to call SDOLTerminal
		SDOLTerminal();
		SDOLTerminal = NULL;
	}

	if (g_hInstSDOLDll)
	{
		FreeLibrary(g_hInstSDOLDll);
		g_hInstSDOLDll = 0;
	}
	g_isSDOLoginWnd = FALSE;
	SDOLInitialize = NULL;
}

bool ParseAreaGroup(INT& areaID, BYTE& groupID)
{
	TArray<FString> ipStrArr;
	FString ipString;
	if(Parse(appCmdLine(), TEXT("-ip="), ipString, FALSE))
	{
		INT ipParCnt = ipString.ParseIntoArray(&ipStrArr, TEXT(","), FALSE);
		if(ipParCnt == 7) // areaID,groupID,IP.IP.IP.IP,port,IP.IP.IP.IP,port,serverName
		{
			areaID = appAtoi(*ipStrArr(0));
			groupID = appAtoi(*ipStrArr(1));
			g_loginServerName = ipStrArr(6);
			return true;
		}
	}

	return false;
}


/* load snda login sdk */
void LoadSDOLogin()
{
 	if(g_SDOLogin)
 	{
 		return;
 	}

	appSetSplashText(SplashTextType::StartupProgress, TEXT("Loading SDO login component..."));
// 
// 	g_isSDOLogin = true;
// 
	WCHAR strExePath[MAX_PATH] = {0};
	WCHAR strExeName[MAX_PATH] = {0};
	WCHAR* strLastSlash = NULL;
	GetModuleFileNameW( NULL, strExePath, MAX_PATH );

	strExePath[MAX_PATH-1]=0;
	strLastSlash = wcsrchr( strExePath, TEXT('\\') );
	if( strLastSlash )
	{	// get directory of exe
		StringCchCopyW( strExeName, MAX_PATH, &strLastSlash[1] );
		*strLastSlash = 0;
		strLastSlash = wcsrchr( strExeName, TEXT('.') );
		if( strLastSlash )
			*strLastSlash = 0;
	}

	WCHAR strGameWidgetDll[MAX_PATH] = {0};
	StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\..\\..\\sdo\\sdologinentry.dll", strExePath );
	
	WCHAR strGameWidgetDll2[MAX_PATH] = {0};
	StringCchPrintfW(strGameWidgetDll2, MAX_PATH, L"%s\\..\\..\\sdo\\sdologin2.dll", strExePath );

	DestroySDOLogin();

	//
	_g_hInstSDOLDll = LoadLibraryW(strGameWidgetDll2);
    _LPSDOLInitialize _SDOLInitialize = (_LPSDOLInitialize)GetProcAddress(_g_hInstSDOLDll, "SDOLInitialize");
	_LPSDOLGetModule  _SDOLGetModule  = (_LPSDOLGetModule)GetProcAddress(_g_hInstSDOLDll, "SDOLGetModule");
	_LPSDOLTerminal   _SDOLTerminal   = (_LPSDOLTerminal)GetProcAddress(_g_hInstSDOLDll, "SDOLTerminal");

	if (_SDOLInitialize == NULL) return;
	if (_SDOLInitialize() == 0)
	{
		if(_SDOLGetModule(IID_ISDOLogin2, (void**)&_g_SDOLogin) == 0)
		{
			_g_SDOLogin->ShowLoginDialog(OnLogin, 0, 0);
			Sleep(3000);
			_g_SDOLogin->Release();
		}
		_SDOLTerminal();
	}

	if ( GetFileAttributesW(strGameWidgetDll) != 0xFFFFFFFF )
	{	// load library "sdologinentry.dll"
		g_hInstSDOLDll = LoadLibraryW(strGameWidgetDll);
		if (g_hInstSDOLDll)
		{
			SDOLInitialize = (LPSDOLInitialize)GetProcAddress( g_hInstSDOLDll, "SDOLInitialize" );
			SDOLGetModule = (LPSDOLGetModule)GetProcAddress( g_hInstSDOLDll, "SDOLGetModule" );
			SDOLTerminal = (LPSDOLTerminal)GetProcAddress( g_hInstSDOLDll, "SDOLTerminal" );
			if ( SDOLInitialize && SDOLGetModule && SDOLTerminal)
			{
				if(!ParseAreaGroup(g_loginAreaID, g_loginServerGroup))
				{
					warnf(NAME_Error, TEXT("Parse area id & server group id error:%s"), appCmdLine());
				}
				else
				{
					warnf(NAME_Log, TEXT("AreaID:%d ServerGroupID:%d"), g_loginAreaID, g_loginServerGroup);
				}

				// load successfully
				SDOLAppInfo tmpAppInfo = {
					sizeof(SDOLAppInfo),   // size of struct
					991002359,		       // unique ID of APP, assigned by SDG
					L"991002359_2270",     // APP name
					MMO_GAME_VER,          // version of game client
					g_loginAreaID,         // Area ID, assigned by SDG
					g_loginServerGroup     // Group ID, assigned by SDG
				};

				if (SDOLInitialize(&tmpAppInfo) == SDOL_ERRORCODE_OK)
				{ //  initialize snda login sdk successfully

					SDOLGetModule(__uuidof(ISDOLLogin), (void**)&g_SDOLogin);

					if ((!g_SDOLogin))
					{
						//g_isSDOLogin = false;
						DestroySDOLogin();
					}

					SDOLGetModule(__uuidof(ISDOAApp), (void**)&g_SDOApp);

				}
				else
				{
					//g_isSDOLogin = false;
					DestroySDOLogin();
				}
			}
			else
			{
				//g_isSDOLogin = false;
				DestroySDOLogin();
			}
		}
	}
}

extern bool g_canShowGameWind;
extern bool g_isLogSuccess;
/* callback of login sdk */
int CALLBACK OnLogin(int nErrorCode, const _SDOLLoginResult* pLoginResult, int nUserData, int nReserved) 
{ 
	//SDOLLoginResult loginResult;

	//warnf(NAME_Log, TEXT("==SDO== ErrCode:%d"), nErrorCode);

	//appSleep(2);

	//if (SDOL_ERRORCODE_OK == nErrorCode) 
	//{ 
	//	g_isLogSuccess = true;
	//	loginResult = *pLoginResult; // login result, including sessionid
	//	
	//	g_sdoSessionID = loginResult.SessionId;
	//	g_canShowGameWind = true;
	//	SendMessage(GGameWindow, WM_ACTIVATEAPP, 1,0);
	//	ShowWindow( GGameWindow, SW_SHOW );		
	//	SetForegroundWindow(GGameWindow);

	//	extern void EndSDOLogin();
	//	EndSDOLogin();
	//	//g_isSDOLogin = false;
	//} 
	//else if (SDOL_ERRORCODE_LOGINCANCEL == nErrorCode)`
	//{
	//	g_isLogSuccess = false;
	//	appRequestExit(0);
	//	//ExitProcess(0);
	//}

	MessageBoxW(NULL, pLoginResult->SessionId, L"", MB_OK);

	g_isLogSuccess = true;
	g_sdoSessionID = pLoginResult->SessionId;
	g_canShowGameWind = true;
	SendMessage(GGameWindow, WM_ACTIVATEAPP, 1,0);
	ShowWindow( GGameWindow, SW_SHOW );		
	SetForegroundWindow(GGameWindow);

	extern void EndSDOLogin();
	EndSDOLogin();

	g_isSDOLoginWnd = FALSE;
	return _SDOL_LOGINRESULT_CLOSE; //close login dialog according to this message
} 

/* show login dialog */
void ShowLoginDialog()
{
	TryParseLoginIP();

	if (g_SDOLogin)
	{
		warnf(NAME_Log, TEXT("==SDO== ShowLoginDialog"));

		appSetSplashText(SplashTextType::StartupProgress, TEXT("Showing SDO login window..."));

		g_canShowGameWind = true;
		//SendMessage(GGameWindow, WM_ACTIVATEAPP, 0,0);
		ShowWindow( GGameWindow, SW_SHOW );
		SendMessage(GGameWindow, WM_ACTIVATEAPP, 1,0);
		g_SDOLogin->SetOwnerWindow(GGameWindow);
		//g_SDOLogin->ShowLoginDialog(OnLogin, 0, 0);
		g_isSDOLoginWnd = TRUE;

		_g_SDOLogin->ShowLoginDialog(OnLogin, 0, 0);
	}
}

UBOOL SDOOpenWindow(LPCWSTR pwcsWinType, LPCWSTR pwcsWinName, LPCWSTR pwcsSrc, int nLeft, int nTop,int nWidth, int nHeight, LPCWSTR pwcsMode)
{
	if (g_SDOApp)
	{
		g_SDOApp->OpenWindow(pwcsWinType, pwcsWinName, pwcsSrc, nLeft, nTop, nWidth, nHeight, pwcsMode);
		return TRUE;
	}
	return FALSE;
	//return TRUE;
}

void ShowSDOLoginDialogWnd(bool bForce)
{
	if(g_SDOLogin)
	{
		if(bForce && g_isSDOLoginWnd==FALSE)
		{
			ShowLoginDialog();
		}

		RECT rc;
		::GetWindowRect(GGameWindow, &rc);
		MoveSDOLoginWindow( rc.left, rc.top );
		g_SDOLogin->MoveLoginDialog(g_lastX, g_lastY);
	}
}

void HideSDOLoginDialogWnd()
{
	if(g_SDOLogin)
	{
		g_SDOLogin->MoveLoginDialog(-600, -600);
	}
}

BOOL _SD_AfxModifyStyle(HWND hWnd, int nStyleOffset,
	DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	//ASSERT(hWnd != NULL);
	DWORD dwStyle = ::GetWindowLong(hWnd, nStyleOffset);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if (dwStyle == dwNewStyle)
		return FALSE;

	::SetWindowLong(hWnd, nStyleOffset, dwNewStyle);
	if (nFlags != 0)
	{
		::SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}
	return TRUE;
}

BOOL _SD_ModifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0)
{
	return _SD_AfxModifyStyle(hWnd, GWL_STYLE, dwRemove, dwAdd, nFlags);
}

BOOL _SD_ModifyStyleEx(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0)
{
	return _SD_AfxModifyStyle(hWnd, GWL_EXSTYLE, dwRemove, dwAdd, nFlags);
}

void MoveSDOLoginWindow(INT x, INT y)
{
	//if (g_isSDOLogin && g_SDOLogin)
	//{
	//	RECT rc;
	//	::GetWindowRect(GGameWindow, &rc);
	//	
	//	HWND loginWnd = ::FindWindow(TEXT("igwUserLoginDialog"), TEXT("igwUserLoginDialog"));
	//	if(loginWnd)
	//	{
	//		_SD_ModifyStyle(loginWnd, WS_CAPTION, WS_POPUP);//WS_CHILD);
	//		_SD_ModifyStyleEx(loginWnd, WS_EX_WINDOWEDGE, 0);

	//		RECT rcLogin;
	//		::GetWindowRect(loginWnd, &rcLogin);
	//		x += ((rc.right - rc.left) - (rcLogin.right - rcLogin.left)) / 2;
	//		y += ((rc.bottom - rc.top) - (rcLogin.bottom - rcLogin.top)) / 2;
	//	}

	//	g_lastX = x;
	//	g_lastY = y;
	//	g_SDOLogin->MoveLoginDialog(x, y);
	//	
	//}
}

void TryParseLoginIP()
{
	FString ipString;
	if(Parse(appCmdLine(), TEXT("-ip="), ipString, FALSE))
	{
		TArray<FString> ipStrArr;
		INT ipParCnt = ipString.ParseIntoArray(&ipStrArr, TEXT(","), FALSE);
		if(ipParCnt == 7) // areaID,groupID,IP.IP.IP.IP,port,IP.IP.IP.IP,port,serverName
		{
			g_loginAreaID = appAtoi(*ipStrArr(0));
			g_loginServerGroup = appAtoi(*ipStrArr(1));
			appStrcpy(g_sdoLoginIP, MAX_PATH, *ipStrArr(2));
			g_sdoLoginPort = appAtoi(*ipStrArr(3));

			appStrcpy(g_sdoLoginIP2, MAX_PATH, *ipStrArr(4));
			g_sdoLoginPort2 = appAtoi(*ipStrArr(5));

			g_loginServerName = ipStrArr(6);
		}
		else
		{
			appErrorf(LocalizeSecure(Localize(TEXT("Error"), TEXT("IPParamError"), TEXT("ProgError")), *ipString));
			appExit();
		}
	}
}

#pragma warning(pop)
