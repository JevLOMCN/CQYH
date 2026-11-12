/*=============================================================================
	Launch.cpp: Game launcher.
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include "LaunchPrivate.h"
#include "NvApexManager.h"
#include "../SDOLogin.h" // oscar@snda

//shan@snda[
#include "VersionHelpers.h"
//shan@snda]

extern int g_loginAreaID;
extern BYTE g_loginServerGroup;


//[fangruyi
#if USING_GPK
#include "../../External/GPK3.0.13/Include/GPKitClt.h"
#pragma comment(lib,"GPKitClt.lib")

SGPK::IGPKCltDynCode *GCltDynCode = NULL;
#endif



//fangruyi]

#if PLATFORM_DESKTOP && !CONSOLE

#include "SplashScreen.h"
#if WITH_OPEN_AUTOMATE
#include "OpenAutomate.h"
#endif

#if HAVE_WXWIDGETS
// use wxWidgets as a DLL
#include <wx/evtloop.h>  // has our base callback class
#endif

#if _WINDOWS && defined(_DEBUG)
#include <crtdbg.h>
#endif

FEngineLoop	GEngineLoop;
/** Whether to use wxWindows when running the game */
UBOOL		GUsewxWindows = FALSE;

/** Whether we should pause before exiting. used by UCC */
UBOOL		GShouldPauseBeforeExit;

/** Whether we should generate crash reports even if the debugger is attached. */
UBOOL		GAlwaysReportCrash = FALSE;

extern "C" int test_main(int argc, char ** argp)
{
	return 0;
}

/*-----------------------------------------------------------------------------
	WinMain.
-----------------------------------------------------------------------------*/

#if _WINDOWS
extern TCHAR MiniDumpFilenameW[1024];
extern INT CreateMiniDump( LPEXCEPTION_POINTERS ExceptionInfo );

// use wxWidgets as a DLL
extern bool IsUnrealWindowHandle( HWND hWnd );
#endif



/**
 * Performs any required cleanup in the case of a fatal error.
 */
static void StaticShutdownAfterError()
{
	// Make sure Novodex is correctly torn down.
	DestroyGameRBPhys();

#if WITH_FACEFX
	// Make sure FaceFX is shutdown.
	UnShutdownFaceFX();
#endif // WITH_FACEFX

#if HAVE_WXWIDGETS
	// Unbind DLLs (e.g. SCC integration)
	WxLaunchApp* LaunchApp = (WxLaunchApp*) wxTheApp;
	if( LaunchApp )
	{
		LaunchApp->ShutdownAfterError();
	}
#endif
}



#if HAVE_WXWIDGETS
class WxUnrealCallbacks : public wxUnrealCallbacks
{
public:

	virtual bool IsUnrealWindowHandle(HWND hwnd) const
	{
		return ::IsUnrealWindowHandle(hwnd);
	}

	virtual bool IsRequestingExit() const 
	{ 
		return GIsRequestingExit ? true : false; 
	}

	virtual void SetRequestingExit( bool bRequestingExit ) 
	{ 
		GIsRequestingExit = bRequestingExit ? true : false; 
	}


	/** Called by WxWidgets if it catches an SEH exception within a WndProc */
	virtual INT WndProcExceptionFilter( LPEXCEPTION_POINTERS ExceptionInfo )
	{
		if( IsDebuggerPresent() )
		{
			// Break on exception thrown from WxWidgets as WndProc will sometimes swallow the
			// exception and we otherwise wouldn't get a chance to break.  Note that this is
			// likely a fatal error and if you opt to continue execution, state may be corrupt!
			appDebugBreak();
			return EXCEPTION_CONTINUE_SEARCH;
		}

		return CreateMiniDump( ExceptionInfo );
	}



	/** Called for unhandled exceptions from WxWidgets WndProc functions */
	virtual void WndProcUnhandledExceptionCallback()
	{
		GError->HandleError();
		StaticShutdownAfterError();
	}

};

static WxUnrealCallbacks s_UnrealCallbacks;
#endif //HAVE_WXWIDGETS



/**
 * Sets global to TRUE if the app should pause infinitely before exit.
 * Currently used by UCC.
 */
void SetShouldPauseBeforeExit(INT ErrorLevel)
{
	// If we are UCC, determine 
	if( GIsUCC )
	{
		// UCC.
		UBOOL bInheritConsole = FALSE;

#if !CONSOLE
		if(NULL != GLogConsole)
		{
			// if we're running from a console we inherited, do not sleep indefinitely
			bInheritConsole = GLogConsole->IsInherited();
		}
#endif

		// Either close log window manually or press CTRL-C to exit if not in "silent" or "nopause" mode.
		GShouldPauseBeforeExit = !bInheritConsole && !GIsSilent && !ParseParam(appCmdLine(),TEXT("NOPAUSE"));
		// if it was specified to not pause if successful, then check that here
		if (ParseParam(appCmdLine(),TEXT("NOPAUSEONSUCCESS")) && ErrorLevel == 0)
		{
			// we succeeded, so don't pause 
			GShouldPauseBeforeExit = FALSE;
		}
	}
}

/** 
 * PreInits the engine loop 
 */
INT EnginePreInit( const TCHAR* CmdLine )
{
	INT ErrorLevel = GEngineLoop.PreInit( CmdLine );

	SetShouldPauseBeforeExit( ErrorLevel );

	return( ErrorLevel );
}

/** 
 * Inits the engine loop 
 */
INT EngineInit( const TCHAR* SplashName )
{
	appShowSplash( SplashName );

	INT ErrorLevel = GEngineLoop.Init();

	if ( !GIsGame )
	{
		appHideSplash();
	}

	return( ErrorLevel );
}

#if 1 // add by showlong, add support for dragging the window without blocking the game
extern void MMOCursorDragWindowFrameProcess();
extern void CursorDragWebWindowProcess();
#endif

/** 
 * Ticks the engine loop 
 */
void EngineTick( void )
{
	M_FORCE_TRY_BEGIN; // oscar@snda
	ResetRenderingCallStack(); // oscar@snda

	GEngineLoop.Tick();
#if 1 // add by showlong, add support for dragging the window without blocking the game
    MMOCursorDragWindowFrameProcess();
	CursorDragWebWindowProcess();
#endif

	SaveRenderingCallStack(); // oscar@snda

	M_FORCE_TRY_END("Protected for EngineTick"); // oscar@snda
}

/**
 * Shuts down the engine
 */
void EngineExit( void )
{
	// Make sure this is set
	GIsRequestingExit = TRUE;

	DestroySDOLogin(); // oscar@snda

	M_FORCE_TRY_BEGIN; // oscar@snda
	GEngineLoop.Exit();
	M_FORCE_TRY_END("Protected for GEngineLoop.Exit"); // oscar@snda
}

#include "..\..\Common\define\version.h" // oscar@snda
extern void TryParseLoginIP();
/**
 * Static guarded main function. Rolled into own function so we can have error handling for debug/ release builds depending
 * on whether a debugger is attached or not.
 */
INT GuardedMain( const TCHAR* CmdLine, HINSTANCE hInInstance, HINSTANCE hPrevInstance, INT nCmdShow )
{
	// For unix based OS's, it is >essential< that this is called as early on in the process as possible;
	//	it determines the base directory by caching the current working directory (and the working directory is changed regularly later).
	appBaseDir();

	TryParseLoginIP();

	// make sure GEngineLoop::Exit() is always called.
	struct EngineLoopCleanupGuard 
	{ 
		~EngineLoopCleanupGuard()
		{
			EngineExit();
		}
	} CleanupGuard;

	// oscar@snda
#if defined(_DEBUG)
	if(ParseParam(CmdLine, TEXT("DBGMSG")))
	{
		appMsgf(AMT_OK, TEXT("Attach & debug now!"));
	}
#endif

	MEMORYSTATUSEX M;
	M.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&M);

	DWORDLONG mem2G = 2;
	mem2G *= 1024 * 1024 * 1024;
	mem2G += 1024 * 1024 * 2; // < 2.2G

	if(M.ullTotalVirtual < mem2G)
	{
		M_MMO_HD = false;
		M_LOW_MEM = true;
		::MessageBox(NULL, 
			TEXT("Only about 2 GB of virtual memory was detected. The game will be very unstable (often a white screen).\n\nPlease find the \"Enable 3G memory extension mode\" shortcut in the game's install menu, right-click it and choose \"Run as administrator\", then restart Windows and start the game again.\n\nUpgrading to a 64-bit operating system is recommended for a smoother experience."),
			TEXT("Notice"),
			MB_OK|MB_ICONWARNING);
	}
	else
	{
		if(ParseParam(CmdLine, TEXT("Lite")))
		{
			M_MMO_HD = false;
		}
		else
		{	
			DWORDLONG mem3G = 3;
			mem3G *= 1024 * 1024 * 1024;		
			if(M.ullTotalVirtual <= mem3G)
			{
				HANDLE Handle = CreateFileW( TEXT("NOTIP32"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

				if( Handle == INVALID_HANDLE_VALUE )
				{
					if(IDNO == ::MessageBox(NULL, 
						TEXT("Less than 3 GB of virtual memory detected. Automatically switching to 32-bit mode for a stable experience!\n\nUpgrading to a 64-bit operating system gives a smoother experience.\n\nShow this prompt next time the game starts?"),
						TEXT("Notice"),
						MB_YESNO|MB_ICONQUESTION))
					{
						Handle = ::CreateFileW(TEXT("NOTIP32"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
						CloseHandle( Handle );
					}
				}
				else
				{
					CloseHandle( Handle );
				}

				M_MMO_HD = false;
			}
		}
	}
	// oscar@snda

	// Set up minidump filename. We cannot do this directly inside main as we use an FString that requires 
	// destruction and main uses SEH.
	// These names will be updated as soon as the Filemanager is set up so we can write to the log file.
	// That will also use the user folder for installed builds so we don't write into program files or whatever.
#if _WINDOWS
	appStrcpy( MiniDumpFilenameW, *FString::Printf( TEXT("MMO-v%d-%d-%d-%s.dmp"), MAJOR_VERSION, MINOR_VERSION, BUILD_NUMBER, *appSystemTimeString() ) ); // oscar@snda

	CmdLine = RemoveExeName(CmdLine);
#endif

#if DEDICATED_SERVER
	//Made to match size of GCmdLine in UnMisc.cpp
	TCHAR ServerCmdLine[16384];
	ServerCmdLine[0] = '\0';
	//Inject server commandlet onto commandline
	appStrncat( ServerCmdLine, TEXT("SERVER "), ARRAY_COUNT(ServerCmdLine) );
	appStrncat( ServerCmdLine, CmdLine, ARRAY_COUNT(ServerCmdLine) );
	CmdLine = ServerCmdLine;
#endif

#if WITH_STEAMWORKS
	extern void appSteamHandleCmdLine(const TCHAR** CmdLine);
	appSteamHandleCmdLine(&CmdLine);
#endif

#if WITH_OPEN_AUTOMATE
	if( ParseParam( CmdLine, TEXT( "OPENAUTOMATE"), TRUE ) )
	{
		GOpenAutomate = new FOpenAutomate();

// NVCHANGE_BEGIN_JC - OpenAutomate Support ( Bug fix )
		FString CleanCmdLine = FString( CmdLine ).Replace( TEXT( "\"" ), TEXT( "" ) );
		extern TCHAR GCmdLine[16384];
		appStrcpy( GCmdLine , *CleanCmdLine );
		CmdLine = GCmdLine;
// NVCHANGE_END_JC - OpenAutomate Support ( Bug fix )
	}
#endif // WITH_OPEN_AUTOMATE

	// oscar@snda
	GIsCooking = (appStristr(CmdLine, TEXT("CookPackages")) != NULL);
	GUseSeekFreeLoading = (appStristr(CmdLine, TEXT("SEEKFREELOADING")) != NULL);
	// oscar@snda

	INT ErrorLevel = EnginePreInit( CmdLine );

	GUsewxWindows = 0;
#if HAVE_WXWIDGETS
	GUsewxWindows	= GIsEditor || ParseParam(appCmdLine(),TEXT("WXWINDOWS")) || ParseParam(appCmdLine(),TEXT("REMOTECONTROL")) || ParseParam(appCmdLine(),TEXT("GM"));
#endif

	// exit if PreInit failed.
	if ( ErrorLevel != 0 || GIsRequestingExit )
	{
		return ErrorLevel;
	}

	if( GUsewxWindows )
	{
#if HAVE_WXWIDGETS
		// use wxWidgets as a DLL
		// set the call back class here
		SetUnrealCallbacks( &s_UnrealCallbacks );

		// UnrealEd of game with wxWindows.
		ErrorLevel = wxEntry( hInInstance, hPrevInstance, "", nCmdShow);
#endif
	}
#if WITH_OPEN_AUTOMATE
	else if( ( GOpenAutomate != NULL ) && !GIsEditor )
	{
		ErrorLevel = EngineInit( TEXT( "PC\\Splash.bmp" ) );

		if( GOpenAutomate->Init( CmdLine ) )
		{
			GIsRequestingExit = GOpenAutomate->ProcessLoop();

			// NVCHANGE_BEGIN_JC - OpenAutomate Support
			//while( !GIsRequestingExit )
			//{
			//	EngineTick();
			//}
			// NVCHANGE_END_JC - OpenAutomate Support
		}

		delete GOpenAutomate;
		GOpenAutomate = NULL;
	}
#endif // WITH_OPEN_AUTOMATE
	else
	{
#if PLATFORM_MACOSX
		ErrorLevel = EngineInit( TEXT("Mac\\Splash.bmp") );
#else
		// Game without wxWindows.
		ErrorLevel = EngineInit( GIsEditor ? TEXT("PC\\EdSplash.bmp") : TEXT("PC\\Splash.png") );
#endif

		while( !GIsRequestingExit )
		{
			EngineTick();
		}
	}
	return ErrorLevel;
}

#if _WINDOWS
/**
 * Maintain a named mutex to detect whether we are the first instance of this game
 */
HANDLE GNamedMutex = NULL;

void ReleaseNamedMutex( void )
{
	if( GNamedMutex )
	{
		ReleaseMutex( GNamedMutex );
		GNamedMutex = NULL;
	}
}

UBOOL MakeNamedMutex( const TCHAR* CmdLine )
{
	UBOOL bIsFirstInstance = FALSE;

	TCHAR MutexName[MAX_SPRINTF] = TEXT( "" );
	appSprintf( MutexName, TEXT( "UnrealEngine3_%d" ), GAMENAME );

	GNamedMutex = CreateMutex( NULL, TRUE, MutexName );

	if( GNamedMutex	&& GetLastError() != ERROR_ALREADY_EXISTS && !ParseParam( CmdLine, TEXT( "NEVERFIRST" ) ) )
	{
		// We're the first instance!
		bIsFirstInstance = TRUE;
	}
	else
	{
		// Still need to release it in this case, because it gave us a valid copy
		ReleaseNamedMutex();
		// There is already another instance of the game running.
		bIsFirstInstance = FALSE;
	}

	return( bIsFirstInstance );
}

/**
 * Handler for CRT parameter validation. Triggers error
 *
 * @param Expression - the expression that failed crt validation
 * @param Function - function which failed crt validation
 * @param File - file where failure occured
 * @param Line - line number of failure
 * @param Reserved - not used
 */
void InvalidParameterHandler(const TCHAR* Expression,
							 const TCHAR* Function, 
							 const TCHAR* File, 
							 UINT Line, 
							 uintptr_t Reserved)
{
	appErrorf(TEXT("SECURE CRT: Invalid parameter detected.\nExpression: %s Function: %s. File: %s Line: %d\n"), 
		Expression ? Expression : TEXT("Unknown"), 
		Function ? Function : TEXT("Unknown"), 
		File ? File : TEXT("Unknown"), 
		Line );
}

/**
 * Setup the common debug settings 
 */
void SetupWindowsEnvironment( void )
{
	// all crt validation should trigger the callback
	_set_invalid_parameter_handler(InvalidParameterHandler);

#ifdef _DEBUG
	// Disable the message box for assertions and just write to debugout instead
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_DEBUG );
	// don't fill buffers with 0xfd as we make assumptions for FNames st we only use a fraction of the entire buffer
	_CrtSetDebugFillThreshold( 0 );
#endif
}

#if WITH_MANAGED_CODE
// Implemented in ManagedCodeSupportCLR.cpp
extern INT ManagedGuardedMain( const TCHAR* CmdLine, HINSTANCE hInInstance, HINSTANCE hPrevInstance, INT nCmdShow );
#endif

/**
 * The inner exception handler catches crashes/asserts in native C++ code and is the only way to get the correct callstack
 * when running a 64-bit executable. However, XAudio2 doesn't always like this and it may result in no sound.
 */
#if _WIN64
	UBOOL GEnableInnerException = TRUE;
#else
	UBOOL GEnableInnerException = FALSE;
#endif

/**
 * Called from Managed code.
 * The inner exception handler catches crashes/asserts in native C++ code and is the only way to get the correct callstack
 * when running a 64-bit executable. However, XAudio2 doesn't like this and it may result in no sound.
 */
INT GuardedMainWrapper( const TCHAR* CmdLine, HINSTANCE hInInstance, HINSTANCE hPrevInstance, INT nCmdShow )
{
	INT ErrorLevel = 0;
	if ( GEnableInnerException )
	{
	 	__try
		{
			// Run the guarded code.
			ErrorLevel = GuardedMain( CmdLine, hInInstance, hPrevInstance, nCmdShow );
		}
		__except( CreateMiniDump( GetExceptionInformation() ), EXCEPTION_CONTINUE_SEARCH )
		{
		}
	}
	else
	{
		// Run the guarded code.
		ErrorLevel = GuardedMain( CmdLine, hInInstance, hPrevInstance, nCmdShow );
	}
	return ErrorLevel;
}

#pragma warning(push)
#pragma warning(disable:4191)
UBOOL CheckSystemAndMemory()
{
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;

	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			// handle error
		}
	}

	if (bIsWow64 != TRUE)
	{
		::MessageBox(NULL, TEXT("A 32-bit system was detected. This test only supports 64-bit systems (4 GB RAM). Please upgrade to a 64-bit system with 4 GB of RAM before installing the game. We apologise for the inconvenience."),TEXT("Warning"), MB_OK | MB_ICONWARNING | MB_TOPMOST | MB_SYSTEMMODAL);
		return FALSE;
	}


	//MEMORYSTATUSEX statex;

	//statex.dwLength = sizeof (statex);

	//GlobalMemoryStatusEx (&statex);

	//if (statex.ullTotalPhys < 4000000000)
	//{
	//	::MessageBox(NULL, TEXT("Detected that your memory is less than 4G. This test only supports 64-bit (4G memory) systems; we recommend upgrading to a 64-bit system and making sure 4G of memory is installed before installing the game. We apologize for the inconvenience."),TEXT("Warning"), MB_OK | MB_ICONWARNING | MB_TOPMOST | MB_SYSTEMMODAL);
	//	return FALSE;
	//}

	return TRUE;
}

//shan@snda[
void DisableDPI()
{
	if ( IsWindows8OrGreater() == 1 )
	{
		typedef HRESULT ( WINAPI* LPSetProcessDPIAwareness )( _In_ int );
	
		HINSTANCE nShcore = LoadLibrary( TEXT("Shcore.dll"));
	 
		if ( nShcore )
		{
			LPSetProcessDPIAwareness fnSetProcessDPIAwareness = (LPSetProcessDPIAwareness) GetProcAddress( nShcore, "SetProcessDpiAwareness" );
			if ( fnSetProcessDPIAwareness )
			fnSetProcessDPIAwareness( 2 );
	 
			FreeLibrary( nShcore );
		}
	}
	HINSTANCE nUser32 = LoadLibrary( TEXT("user32.dll"));
	if ( nUser32 )
	{
		typedef BOOL ( WINAPI* LPSetProcessDPIAware ) ( void );
		LPSetProcessDPIAware pSetProcessDPIAware = (LPSetProcessDPIAware)GetProcAddress( nUser32, "SetProcessDPIAware" );
		if ( pSetProcessDPIAware )
			pSetProcessDPIAware();
		FreeLibrary( nUser32 );
	}
}
//shan@snda]
#pragma warning(pop)

extern bool ParseAreaGroup(INT& areaID, BYTE& groupID);

bool ShouldSkipGPK()
{
	FString strCmdLine(GetCommandLine());
	bool shouldSkipGPK = (strCmdLine.InStr(TEXT("make"), FALSE, TRUE) != INDEX_NONE)
		|| (strCmdLine.InStr(TEXT("cookpackages"), FALSE, TRUE) != INDEX_NONE)
		|| (strCmdLine.InStr(TEXT("editor"), FALSE, TRUE) != INDEX_NONE)
		|| (strCmdLine.InStr(TEXT("makereflist"), FALSE, TRUE) != INDEX_NONE)
		|| (strCmdLine.InStr(TEXT("skipgpk"), FALSE, TRUE) != INDEX_NONE);
	return shouldSkipGPK;
}

INT WINAPI WinMain( HINSTANCE hInInstance, HINSTANCE hPrevInstance, char*, INT nCmdShow )
{
//shan@snda[
	DisableDPI();
//shan@snda]
#if SHIPPING_PC_GAME// oscar@snda

	HANDLE hnd = CreateMutex(NULL, FALSE, _T("AOD_Game"));
	if(hnd)
	{
		DWORD dwR = ::WaitForSingleObject(hnd, 2000); // try 2 seconds
		if(dwR == WAIT_TIMEOUT || dwR == WAIT_ABANDONED)
		{
#if 0
			::MessageBox(NULL, TEXT("Legend Eternal is already running. Please wait for the login screen to appear, or force-close the process and start again."),
				TEXT("Warning"), MB_OK | MB_ICONWARNING | MB_TOPMOST | MB_SYSTEMMODAL);
			CloseHandle( hnd );
			return FALSE;
#else
			if(IDNO == ::MessageBox(NULL, TEXT("Legend Eternal is already running. Run the game in low quality mode?\r\nNote: running multiple clients may use up memory or video memory and stop the game from running properly!"),
				TEXT("Warning"), MB_YESNO | MB_ICONWARNING | MB_TOPMOST | MB_SYSTEMMODAL))
			{
				CloseHandle( hnd );
				//return FALSE;
			}
			else
			{
				M_MMO_HD = false;
			}
#endif
		}
	}

	//fangruyi
// 	if (!CheckSystemAndMemory())
// 	{
// 		return FALSE;
// 	}


#endif	// oscar@snda

	//[fangruyi
#if USING_GPK

#if SHIPPING_PC_GAME
	LPCSTR gpkUpdate = "https://cq.dorado.sdo.com/cq/gpk/";
	{
#else
	LPCSTR gpkUpdate = "https://172.30.66.31/gpk/";

	if(!ShouldSkipGPK())
	{
#endif
		GCltDynCode = SGPK::GPKStart(gpkUpdate, "CQYH");		  //todo : using a right url
		if(GCltDynCode == NULL)
		{
			return 0;
		}
	}
#endif
// fangruyi]



	// Setup common Windows settings
	SetupWindowsEnvironment();

	// default to no game
	appStrcpy(GGameName, TEXT("None"));

	INT ErrorLevel			= 0;
	GIsStarted				= 1;
	hInstance				= hInInstance;
	const TCHAR* CmdLine	= GetCommandLine();

	// oscar@snda
	if(ParseParam(CmdLine, TEXT("sdo")))
	{
        extern bool g_isSDOLogin;
		g_isSDOLogin = true;
	}

#if SHIPPING_PC_GAME
	UBOOL isSDOTest = ParseParam(CmdLine, TEXT("sdot"));
	if(isSDOTest)
#endif
	{
		if(!ParseAreaGroup(g_loginAreaID, g_loginServerGroup))
		{
			//warnf(NAME_Error, TEXT("Parse area id & server group id error:%s"), appCmdLine());
		}
		else
		{
			//warnf(NAME_Log, TEXT("AreaID:%d ServerGroupID:%d"), g_loginAreaID, g_loginServerGroup);
		}
	}

#if SHIPPING_PC_GAME
	if(!(g_isSDOLogin || isSDOTest))
	{
		::MessageBox(NULL, TEXT("Please start the game with the shortcut created during installation.\nOr run Launcher.exe in the install directory."),
			TEXT("Notice"), MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SYSTEMMODAL);
		return FALSE;
	}
#endif
	// oscar@snda
	
#if !SHIPPING_PC_GAME && !CONSOLE
	// Named mutex we use to figure out whether we are the first instance of the game running. This is needed to e.g.
	// make sure there is no contention when trying to save the shader cache.
	GIsFirstInstance = MakeNamedMutex( CmdLine );

	if ( ParseParam( CmdLine,TEXT("crashreports") ) )
	{
		GAlwaysReportCrash = TRUE;
	}
#endif

	// Using the -noinnerexception parameter will disable the exception handler within native C++, which is call from managed code,
	// which is called from this function.
	// The default case is to have three wrapped exception handlers (note that wxWidgets also use an exception handler, which would be a 4th one):
	// Native: WinMain() -> Managed: ManagedGuardedMain() -> Native: GuardedMainWrapper().
	// The inner exception handler in GuardedMainWrapper() catches crashes/asserts in native C++ code and is the only way to get the
	// correct callstack when running a 64-bit executable. However, XAudio2 sometimes (?) don't like this and it may result in no sound.
#if _WIN64
	if ( ParseParam(CmdLine,TEXT("noinnerexception")) || GIsBenchmarking )
	{
		GEnableInnerException = FALSE;
	}
#endif

#if defined( _DEBUG )
	if( TRUE && !GAlwaysReportCrash )
#else
	if( appIsDebuggerPresent() && !GAlwaysReportCrash )
#endif
	{
		// Don't use exception handling when a debugger is attached to exactly trap the crash. This does NOT check
		// whether we are the first instance or not!
		ErrorLevel = GuardedMain( CmdLine, hInInstance, hPrevInstance, nCmdShow );
	}
	else
	{
		// Use structured exception handling to trap any crashes, walk the the stack and display a crash dialog box.
 		__try
 		{
			GIsGuarded = 1;
			// Run the guarded code.
#if WITH_MANAGED_CODE
			ErrorLevel = ManagedGuardedMain( CmdLine, hInInstance, hPrevInstance, nCmdShow );
#else
			ErrorLevel = GuardedMainWrapper( CmdLine, hInInstance, hPrevInstance, nCmdShow );
#endif
			GIsGuarded = 0;
		}
		__except( GEnableInnerException ? EXCEPTION_EXECUTE_HANDLER : CreateMiniDump( GetExceptionInformation() ) )
		{
#if !SHIPPING_PC_GAME && !CONSOLE
			// Release the mutex in the error case to ensure subsequent runs don't find it.
			ReleaseNamedMutex();
#endif
			// Crashed.
			ErrorLevel = 1;
			GError->HandleError();
			StaticShutdownAfterError();
			appRequestExit( TRUE );
		}
	}

	// Final shut down.
	appExit();

#if USING_GPK
	if (GCltDynCode != NULL)
	{
		GCltDynCode->Release();
	}

#endif

#if !SHIPPING_PC_GAME && !CONSOLE
	// Release the named mutex again now that we are done.
	ReleaseNamedMutex();
#endif

	// pause if we should
	if (GShouldPauseBeforeExit)
	{
		// oscar@snda
		//Sleep(INFINITE);
		while (!::_kbhit())
		{
			Sleep(2);
		}
		// oscar@snda
	}
	
	GIsStarted = 0;

#if SHIPPING_PC_GAME// oscar@snda
	if(hnd)
	{
		CloseHandle(hnd);
	}
#endif	// oscar@snda

	return ErrorLevel;
}

#ifdef _WINDLL

INT PIBGuardedMainInit( const TCHAR* CmdLine )
{
	INT ErrorLevel = EnginePreInit( CmdLine );

	// exit if PreInit failed.
	if ( ErrorLevel != 0 || GIsRequestingExit )
	{
		return( 1 );
	}

	ErrorLevel = EngineInit( TEXT("PCConsole\\Splash.bmp") );

	return( ErrorLevel );
}

/** 
 * Inits the loaded DLL version of UE3
 */
INT PIBInit( HINSTANCE hInInstance, const TCHAR* CmdLine )
{
	SetupWindowsEnvironment();

	// default to no game
	appStrcpy(GGameName, TEXT("None"));

	INT ErrorLevel = 0;
	GIsStarted = 1;
	hInstance = hInInstance;

#ifdef _DEBUG
	if( TRUE )
#else
	if( IsDebuggerPresent() )
#endif
	{
		// Don't use exception handling when a debugger is attached to exactly trap the crash. This does NOT check
		// whether we are the first instance or not!
		ErrorLevel = PIBGuardedMainInit( CmdLine );
	}
	else
	{
		// Use structured exception handling to trap any crashes, walk the the stack and display a crash dialog box.
		__try
		{
			GIsGuarded = 1;
			ErrorLevel = PIBGuardedMainInit( CmdLine );
			GIsGuarded = 0;
		}
		__except( CreateMiniDump( GetExceptionInformation() ) )
		{
			// Crashed.
			ReleaseNamedMutex();

			ErrorLevel = 1;
			GError->HandleError();
			StaticShutdownAfterError();
			appRequestExit( TRUE );
		}
	}

	return( ErrorLevel );
}

/**
 * Shuts the loaded DLL version of UE3 down
 */
void PIBShutdown( void )
{
	EngineExit();

	appExit();

	GIsStarted = 0;
}

#endif // _WINDLL

#endif

#include <exception>

extern UBOOL GAlreadyCreatedMinidump;

// oscar@snda
void CreateCustomDump(const TCHAR* fileName)
{
	__try
	{
		appStrcpy( MiniDumpFilenameW, fileName );
		RaiseException( -1, 0, 0, NULL );
	}
	__except( CreateMiniDump( GetExceptionInformation() ), EXCEPTION_CONTINUE_EXECUTION )
	{
	}

	GAlreadyCreatedMinidump = FALSE;
}
// oscar@snda

#endif
