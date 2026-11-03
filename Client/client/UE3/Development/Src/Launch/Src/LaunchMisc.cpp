/**
 * Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
 */

#include "LaunchPrivate.h"

#if UE3_WX32 && HAVE_WXWIDGETS
#include <wx/evtloop.h>
#endif

#if _WINDOWS

#include "SplashScreen.h"

#if WITH_EDITOR
#include "PropertyWindowManager.h"
#endif
#include <ErrorRep.h>
#include <Werapi.h>

#pragma comment(lib, "Faultrep.lib")
#pragma comment(lib, "wer.lib")


//
// Minidump support/ exception handling.
//

#pragma pack(push,8)
#include <DbgHelp.h>
#pragma pack(pop)

TCHAR MiniDumpFilenameW[1024] = TEXT("");
/*static */UBOOL GAlreadyCreatedMinidump = FALSE; // oscar@snda

INT CreateMiniDump( LPEXCEPTION_POINTERS ExceptionInfo )
{
	// Only create a minidump the first time this function is called.
	// (Can be called the first time from the RenderThread, then a second time from the MainThread.)
	if ( GAlreadyCreatedMinidump == FALSE )
	{
		GAlreadyCreatedMinidump = TRUE;

		//// Try to create file for minidump.
		//HANDLE FileHandle	= CreateFileW( MiniDumpFilenameW, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		//	
		//// Write a minidump.
		//if( FileHandle != INVALID_HANDLE_VALUE )
		//{
		//	MINIDUMP_EXCEPTION_INFORMATION DumpExceptionInfo;

  //          DumpExceptionInfo.ThreadId			= ::GetCurrentThreadId();
		//	DumpExceptionInfo.ExceptionPointers	= ExceptionInfo;
		//	DumpExceptionInfo.ClientPointers	= true;

		//	MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), FileHandle, MiniDumpNormal, &DumpExceptionInfo, NULL, NULL );
		//	CloseHandle( FileHandle );
		//}

		GLog->Flush();

		TCHAR MiniDumpExe[] = TEXT("..\\MiniDump.exe");
		//2019-6-9
		warnf( NAME_Log, TEXT("============================%s\n "),MiniDumpFilenameW);
		FString CallingCommandLine = FString::Printf(TEXT("%u %u %p \"%s\""), GetCurrentProcessId(),  ::GetCurrentThreadId() , ExceptionInfo , MiniDumpFilenameW);

		void * ProcHandle = appCreateProc(MiniDumpExe, *CallingCommandLine);

		if( ProcHandle != NULL )
		{
			appWaitForProc(ProcHandle);
		}
		else
		{
			warnf(TEXT("Couldn't start up the MiniDump process!"));
		}

		const SIZE_T StackTraceSize = 65535;
		ANSICHAR* StackTrace = (ANSICHAR*) appSystemMalloc( StackTraceSize );
		StackTrace[0] = 0;
		// Walk the stack and dump it to the allocated memory.
		appStackWalkAndDump( StackTrace, StackTraceSize, 0, ExceptionInfo->ContextRecord );
#if !CONSOLE
		appStrncat( GErrorHist, ANSI_TO_TCHAR(StackTrace), ARRAY_COUNT(GErrorHist) - 1 );
#endif
		appSystemFree( StackTrace );

#if SHIPPING_PC_GAME 
		DWORD dwOpt = 0;
		EFaultRepRetVal repret = ReportFault( ExceptionInfo, dwOpt);
#endif
	}

	return EXCEPTION_EXECUTE_HANDLER;
}


#if HAVE_WXWIDGETS
//
//	WxLaunchApp implementation.
//

#if UE3_WX32
// wxWidgets 3's default GUI logger opens a modal dialog. Keep its diagnostics
// visible in the engine log, including during unattended editor startup.
class WxUnrealLog : public wxLog
{
protected:
	virtual void DoLogTextAtLevel(wxLogLevel Level, const wxString& Message)
	{
		debugf(NAME_Warning, TEXT("wxWidgets [%d]: %s"), (INT)Level, Message.wc_str());
	}
};
#endif

/**
 * Gets called on initialization from within wxEntry.	
 */
bool WxLaunchApp::OnInit()
{
#if UE3_WX32
	delete wxLog::SetActiveTarget(new WxUnrealLog());
#endif
#if !UE3_WX32
	wxApp::OnInit();
#endif // wxWidgets 3's default OnInit parses arguments already owned by UE3.

#if WITH_MANAGED_CODE
	// If running the editor, the exe's config file must be found to proceed
	if ( GIsEditor )
	{
		// Ensure that the required *.exe.config file is found and warn the user if it is not before exiting
		// (It would cause a crash later if it were missing)
		FFilename ConfigFileName =  FString::Printf( TEXT("%s.exe.config"), appExecutableName() );
		if ( GFileManager->FileSize( *ConfigFileName ) == INDEX_NONE )
		{
			// Warn the user
			FString ErrorString = FString::Printf( LocalizeSecure( LocalizeError( TEXT("MissingExeConfigFileError"), TEXT("CORE") ), *ConfigFileName ) );
			appMsgf( AMT_OK, *ErrorString );

			return 0;
		}
	}
#endif // WITH_MANAGED_CODE

	appShowSplash( GIsEditor ? TEXT("PC\\EdSplash.bmp") : TEXT("PC\\Splash.png") );

	// Initialize XML resources
	wxXmlResource::Get()->InitAllHandlers();
	verify( wxXmlResource::Get()->Load( *FString( GetEditorResourcesDir() * FString( TEXT("wxRC/UnrealEd*.xrc") ) ) ) );

	INT ErrorLevel = GEngineLoop.Init();
	if ( ErrorLevel )
	{
		appHideSplash();
		return 0;
	}

	// Init subsystems
#if WITH_EDITOR
	InitPropertySubSystem();
#endif

	if ( !GIsEditor )
	{
		appHideSplash();
	}

	return 1;
}

/** 
 * Gets called after leaving main loop before wxWindows is shut down.
 */
int WxLaunchApp::OnExit()
{
	return wxApp::OnExit();
}

/**
 * Performs any required cleanup in the case of a fatal error.
 */
void WxLaunchApp::ShutdownAfterError()
{
}

/**
 * Callback from wxWindows main loop to signal that we should tick the engine.
 */
void WxLaunchApp::TickUnreal()
{
	if( !GIsRequestingExit )
	{
		GEngineLoop.Tick();
	}
}

#if UE3_WX32
int WxLaunchApp::OnRun()
{
	bUnrealMainLoopStarted = true;
	return wxApp::OnRun();
}

bool WxLaunchApp::ProcessIdle()
{
	// wxWidgets 3 also calls ProcessIdle from Yield(), unlike the old
	// engine-specific loop. Never tick from a nested UI message pump or
	// while a worker owns wx's GUI lock: the engine touches wx windows.
	if (!wxThread::IsMain() || !wxGuiOwnedByMainThread())
	{
		return false;
	}
	wxEventLoopBase* const Loop = wxEventLoopBase::GetActive();
	if (!Loop || Loop->IsYielding() || !bUnrealMainLoopStarted || bUnrealTickInProgress || GIsSlowTask)
	{
		return false;
	}
	// Modal dialogs own a separate event loop. Keep their idle UI work,
	// but do not advance the engine while the caller's UI operation is open.
	if (Loop != GetMainLoop())
	{
		return wxApp::ProcessIdle();
	}
	bUnrealTickInProgress = true;
	if (!GIsRequestingExit)
	{
		TickUnreal();
	}
	if (GIsRequestingExit)
	{
		wxEventLoopBase* ActiveLoop = wxEventLoopBase::GetActive();
		if (ActiveLoop) ActiveLoop->ScheduleExit(0);
		bUnrealTickInProgress = false;
		return false;
	}
	wxApp::ProcessIdle();
	bUnrealTickInProgress = false;
	return true;
}
#endif

/**
 * The below is a manual expansion of wxWindows's IMPLEMENT_APP to allow multiple wxApps.
 *
 * @warning: when upgrading wxWindows, make sure that the below is how IMPLEMENT_APP looks like
 */
wxAppConsole * wxCreateApp()
{
	wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "UnrealEngine3");
	return GIsEditor ? new WxUnrealEdApp : new WxLaunchApp;
}
wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);
WxLaunchApp& wxGetApp() { return *(WxLaunchApp *)wxTheApp; }

#endif //HAVE_WXWIDGETS

#endif   //_WINDOWS
