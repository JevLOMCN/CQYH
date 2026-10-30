#pragma once
// Included only by the runtime fixture; production uses the Windows APIs.
#include <winsock2.h>
#include <windows.h>
BOOL WINAPI FixtureSetServiceStatus(SERVICE_STATUS_HANDLE, LPSERVICE_STATUS);
SERVICE_STATUS_HANDLE WINAPI FixtureRegisterHandler(LPCSTR, LPHANDLER_FUNCTION);
BOOL WINAPI FixtureDispatcher(const SERVICE_TABLE_ENTRYA*);
BOOL WINAPI FixtureConsoleHandler(PHANDLER_ROUTINE, BOOL);
BOOL WINAPI FixtureConsoleTitle(LPCSTR);
DWORD WINAPI FixtureModuleName(HMODULE, LPWSTR, DWORD);
SC_HANDLE WINAPI FixtureOpenSCM(LPCWSTR, LPCWSTR, DWORD);
SC_HANDLE WINAPI FixtureCreateService(SC_HANDLE, LPCWSTR, LPCWSTR, DWORD, DWORD, DWORD, DWORD,
    LPCWSTR, LPCWSTR, LPDWORD, LPCWSTR, LPCWSTR, LPCWSTR);
SC_HANDLE WINAPI FixtureOpenService(SC_HANDLE, LPCWSTR, DWORD);
BOOL WINAPI FixtureControlService(SC_HANDLE, DWORD, LPSERVICE_STATUS);
BOOL WINAPI FixtureDeleteService(SC_HANDLE);
BOOL WINAPI FixtureCloseService(SC_HANDLE);
#define SetServiceStatus FixtureSetServiceStatus
#define RegisterServiceCtrlHandlerA FixtureRegisterHandler
#define StartServiceCtrlDispatcherA FixtureDispatcher
#define SetConsoleCtrlHandler FixtureConsoleHandler
#define SetConsoleTitleA FixtureConsoleTitle
#define GetModuleFileNameW FixtureModuleName
#define OpenSCManagerW FixtureOpenSCM
#define CreateServiceW FixtureCreateService
#define OpenServiceW FixtureOpenService
#define ControlService FixtureControlService
#define DeleteService FixtureDeleteService
#define CloseServiceHandle FixtureCloseService
#define StartDumpThread FixtureStartDump
#define StopDumpThread FixtureStopDump
#define g_InitStatisInfo FixtureInitStats
#define g_InitReportSetting FixtureInitReport
#define ProcessOption FixtureProcessOption
