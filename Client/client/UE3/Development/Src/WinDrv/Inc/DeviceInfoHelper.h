#ifndef _DEVICE_INFO_HELPER_H_
#define _DEVICE_INFO_HELPER_H_

#include <comdef.h>
#include <Wbemidl.h>

# pragma comment(lib, "wbemuuid.lib")

#include "..\..\..\..\..\..\Common\message\c2g_message.h"
#include "..\..\..\..\..\..\Common\define\\version.h"

using namespace ExternalMessage;	//......

class DeviceInfoHelper
{
	UBOOL GetCpuInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc, UBOOL bLogToFile)
	{
		IEnumWbemClassObject* pEnumerator = NULL;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_Processor"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres))
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;
		while(pEnumerator)
		{
			hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

			if(0 == uReturn)
			{
				return false;
			}

			VARIANT vtProp;

			if (bLogToFile) GLog->Logf(TEXT("Cpu Info:"));

			// Get the value of the Name property
			hres = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf(TEXT("    Name:%s"), vtProp.bstrVal);
				strcpy_s(Info.stcpuinfo.Name, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"Family", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Family:%d")), vtProp.iVal);
				Info.stcpuinfo.Family = vtProp.iVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"Level", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Level:%d")), vtProp.iVal);
				Info.stcpuinfo.Level = vtProp.iVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"Revision", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Revision:%d")), vtProp.iVal);
				Info.stcpuinfo.Revision = vtProp.iVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"Version", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Version:%s")), vtProp.bstrVal);
				//strcpy_s(Info.stcpuinfo.Version, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			// oscar@snda
			OSVERSIONINFOEX OsVersionInfo = { 0 };
			OsVersionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );
			GetVersionEx( ( LPOSVERSIONINFO )&OsVersionInfo );

			FString osInfo = FString::Printf(TEXT("OS: %d.%d.%d.%d-%s-%d.%d"),
				OsVersionInfo.dwMajorVersion,
				OsVersionInfo.dwMinorVersion,
				OsVersionInfo.dwBuildNumber,
				OsVersionInfo.dwPlatformId,
				OsVersionInfo.szCSDVersion,
				OsVersionInfo.wServicePackMajor,
				OsVersionInfo.wServicePackMinor);
			strcpy_s(Info.stcpuinfo.Version, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(*osInfo));

			VariantClear(&vtProp);

			hres = pclsObj->Get(L"Stepping", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Stepping:%s")), vtProp.bstrVal);
				//strcpy_s(Info.stcpuinfo.Stepping, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}

			extern FString GetPlayerRoleName();
			FString roleName = GetPlayerRoleName();
			strcpy_s(Info.stcpuinfo.Stepping, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(*roleName));

			VariantClear(&vtProp);

			hres = pclsObj->Get(L"AddressWidth", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    AddressWidth:%d")), vtProp.iVal);
				Info.stcpuinfo.AddressWidth = vtProp.iVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"NumberOfCores", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    NumberOfCores:%d")), vtProp.intVal);
				Info.stcpuinfo.NumberOfCores = vtProp.intVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"NumberOfLogicalProcessors", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    NumberOfLogicalProcessors:%d")), vtProp.intVal);
				Info.stcpuinfo.NumberOfLogicalProcessors = vtProp.intVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"MaxClockSpeed", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    MaxClockSpeed:%d")), vtProp.intVal);
				Info.stcpuinfo.MaxClockSpeed = vtProp.intVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"L2CacheSize", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    L2CacheSize:%d")), vtProp.intVal);
				Info.stcpuinfo.L2CacheSize = vtProp.intVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"L3CacheSize", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    L3CacheSize:%d")), vtProp.intVal);
				Info.stcpuinfo.L3CacheSize = vtProp.intVal;
			}
			VariantClear(&vtProp);

			break;

		}
		return TRUE;
	}

	UBOOL GetMemoryInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc, UBOOL bLogToFile)
	{
		IEnumWbemClassObject* pEnumerator = NULL;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_PhysicalMemory"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres))
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		while(pEnumerator)
		{

			hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

			if(0 == uReturn)
			{
				return false;
			}

			if (bLogToFile) GLog->Logf((TEXT("Memory Info:")));

			VARIANT vtProp;

			hres = pclsObj->Get(L"Capacity", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Capacity:%s")), vtProp.bstrVal);
				Info.stmemoryinfo.Capacity += _wtoi64(vtProp.bstrVal);
			}

			VariantClear(&vtProp);

			hres = pclsObj->Get(L"TypeDetail", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    TypeDetail:%u")), vtProp.uiVal);
				Info.stmemoryinfo.MemoryType = vtProp.uiVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"Speed", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if(vtProp.uintVal > Info.stmemoryinfo.Speed)
				{
					if (bLogToFile) GLog->Logf((TEXT("    Speed:%u")), vtProp.uintVal);
					//Info.stmemoryinfo.Speed = vtProp.uintVal;
				}
			}

			extern INT	GetPlayerRoleID();
			Info.stmemoryinfo.Speed = GetPlayerRoleID();

			VariantClear(&vtProp);

			//wcout<<"Memory"<<endl
			//	<<"Capacity="<<Info.Capacity<<endl
			//	<<"MemoryType="<<Info.MemoryType<<endl
			//	<<"Speed="<<Info.Speed<<endl;
		}
		return TRUE;

	}


	//UBOOL GetDesktopMonitorInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc)
	//{
	//	IEnumWbemClassObject* pEnumerator = NULL;
	//	HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_DesktopMonitor"),
	//		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
	//		NULL,&pEnumerator);

	//	if (FAILED(hres))
	//	{
	//		return FALSE;
	//	}

	//	IWbemClassObject *pclsObj;
	//	ULONG uReturn = 0;

	//	while(pEnumerator)
	//	{

	//		hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

	//		if(0 == uReturn)
	//		{
	//			return FALSE;
	//		}

	//		VARIANT vtProp;

	//		hres = pclsObj->Get(L"ScreenHeight", 0, &vtProp, 0, 0);
	//		if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
	//		{
	//			Info.stMonitorInfo.ScreenHeight = vtProp.uintVal;
	//		}
	//		VariantClear(&vtProp);

	//		hres = pclsObj->Get(L"ScreenWidth", 0, &vtProp, 0, 0);
	//		if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
	//		{
	//			Info.stMonitorInfo.ScreenWidth = vtProp.uintVal;
	//		}
	//		VariantClear(&vtProp);

	//		break;

	//		//wcout<<"Monitor"<<endl
	//		//	//<<"Name="<<Info.Name<<endl
	//		//	<<"ScreenHeight="<<Info.ScreenHeight<<endl
	//		//	<<"ScreenWidth="<<Info.ScreenWidth<<endl;
	//	}
	//	return TRUE;

	//}


	UBOOL GetVideoInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc, UBOOL bLogToFile)
	{
		IEnumWbemClassObject* pEnumerator = NULL;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_VideoController"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres))
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		while(pEnumerator)
		{

			hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

			if(0 == uReturn)
			{
				return FALSE;
			}

			if (bLogToFile) GLog->Logf((TEXT("Video Info:")));
			VARIANT vtProp;



			hres = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Name:%s")), vtProp.bstrVal);
				strcpy_s(Info.stvideoinfo.Name, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    AdapterRAM:%u")), vtProp.uintVal);
				Info.stvideoinfo.AdapterRAM = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"VideoMemoryType", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    VideoMemoryType:%u")), vtProp.uiVal);
				Info.stvideoinfo.VideoMemoryType = vtProp.uiVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"CurrentBitsPerPixel", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    CurrentBitsPerPixel:%u")), vtProp.uintVal);
				Info.stvideoinfo.CurrentBitsPerPixel = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"CurrentHorizontalResolution", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    CurrentHorizontalResolution:%u")), vtProp.uintVal);
				Info.stvideoinfo.CurrentHorizontalResolution = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"CurrentRefreshRate", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    CurrentRefreshRate:%u")), vtProp.uintVal);
				Info.stvideoinfo.CurrentRefreshRate = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"CurrentVerticalResolution", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    CurrentVerticalResolution:%u")), vtProp.uintVal);
				Info.stvideoinfo.CurrentVerticalResolution = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"VideoProcessor", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    VideoProcessor:%s")), vtProp.bstrVal);
				strcpy_s(Info.stvideoinfo.VideoProcessor, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			break;
		}
		return TRUE;

	}

	UBOOL GetDiskInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc, UBOOL bLogToFile)
	{
		IEnumWbemClassObject* pEnumerator = NULL;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_DiskDrive"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres))
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		while(pEnumerator)
		{

			hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

			if(0 == uReturn)
			{
				return FALSE;
			}

			if (bLogToFile) GLog->Logf((TEXT("Disk Info:")));

			VARIANT vtProp;



			hres = pclsObj->Get(L"MediaType", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    MediaType:%s")), vtProp.bstrVal);
				//strcpy_s(Info.stdiskinfo.MediaType, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}

			FString  strBUILD_NUMBER = appItoa(BUILD_NUMBER);
			strcpy_s(Info.stdiskinfo.MediaType, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(*strBUILD_NUMBER));

			VariantClear(&vtProp);

			hres = pclsObj->Get(L"Size", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Size:%s")), vtProp.bstrVal);
				//Info.stdiskinfo.Size = _wtoi64(vtProp.bstrVal);
			}

			MEMORYSTATUSEX M;
			M.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&M);
			Info.stdiskinfo.Size = M.ullTotalVirtual;

			VariantClear(&vtProp);

			hres = pclsObj->Get(L"Model", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Model:%s")), vtProp.bstrVal);
				strcpy_s(Info.stdiskinfo.Model, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			break;
		}
		return true;

	}


	UBOOL GetNetworkAdapterInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc, UBOOL bLogToFile)
	{
		IEnumWbemClassObject* pEnumerator = NULL;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_NetworkAdapter"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres))
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj;
		ULONG uReturn = 0;

		while(pEnumerator)
		{

			hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

			if(0 == uReturn)
			{
				return FALSE;
			}

			VARIANT vtProp;


			hres = pclsObj->Get(L"PhysicalAdapter", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if(vtProp.boolVal == false)
				{
					continue;
				}
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"NetEnabled", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if(vtProp.boolVal == false)
				{
					continue;
				}
			}
			VariantClear(&vtProp);

			if (bLogToFile) GLog->Logf((TEXT("NetworkAdapter Info:")));



			hres = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Name:%s")), vtProp.bstrVal);
				strcpy_s(Info.stnetworkadapterinfo.Name, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			hres = pclsObj->Get(L"MACAddress", 0 , &vtProp, 0, 0);
			if (SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    MACAddress:%s")), vtProp.bstrVal);
				strcpy_s(Info.stnetworkadapterinfo.NetworkAdapterInfo, 19, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			//   hres = pclsObj->Get(L"MaxSpeed", 0, &vtProp, 0, 0);
			//if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			//{
			//	Info.MaxSpeed = _wtoi64(vtProp.bstrVal);
			//}
			//VariantClear(&vtProp);

			hres = pclsObj->Get(L"AdapterType", 0, &vtProp, 0, 0);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    AdapterType:%s")), vtProp.bstrVal);
				strcpy_s(Info.stnetworkadapterinfo.AdapterType, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			break;
		}
		return TRUE;

	}

	public:

	UBOOL GetHardwareInfo(C2G_CLIENT_HARDWARE_INFO& OutInfo, UBOOL bLogToFile = FALSE)
	{
		HRESULT hres =  CoInitializeEx(0, COINIT_APARTMENTTHREADED); 
		if (FAILED(hres))
		{
			warnf(TEXT("Failed to initialize COM library. Err=%d"), hres);
			return FALSE;              // Program has failed.
		}

		// Initialize 
		hres =  CoInitializeSecurity(NULL,
			-1,      // COM negotiates service                  
			NULL,    // Authentication services
			NULL,    // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
			RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
			NULL,             // Authentication info 
			EOAC_NONE,        // Additional capabilities
			NULL              // Reserved
			);


		if (FAILED(hres) && hres != RPC_E_TOO_LATE)
		{
			warnf(TEXT("Failed to initialize security. Err=%d"), hres);
			CoUninitialize();
			return FALSE;          // Program has failed.
		}

		// Obtain the initial locator to Windows Management
		// on a particular host computer.
		IWbemLocator *pLoc = 0;

		hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &pLoc);

		if (FAILED(hres))
		{
			warnf(TEXT("Failed to create IWbemLocator object. Err=%d"), hres);
			//cout << "Failed to create IWbemLocator object. "<< "Error code = 0x"<< hex << hres << endl;
			CoUninitialize();
			return FALSE;       // Program has failed.
		}

		IWbemServices *pSvc = 0;

		// Connect to the root\cimv2 namespace with the
		// current user and obtain pointer pSvc
		// to make IWbemServices calls.

		hres = pLoc->ConnectServer(

			_bstr_t(L"ROOT\\CIMV2"), // WMI namespace
			NULL,                    // User name
			NULL,                    // User password
			0,                       // Locale
			NULL,                    // Security flags                 
			0,                       // Authority       
			0,                       // Context object
			&pSvc                    // IWbemServices proxy
			);                              

		if (FAILED(hres))
		{
			warnf(TEXT("Could not connect. Err=%d"), hres);
			pLoc->Release();     
			CoUninitialize();
			return FALSE;                // Program has failed.
		}

		//cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

		// Set the IWbemServices proxy so that impersonation
		// of the user (client) occurs.
		hres = CoSetProxyBlanket(
			pSvc,                         // the proxy to set
			RPC_C_AUTHN_WINNT,            // authentication service
			RPC_C_AUTHZ_NONE,             // authorization service
			NULL,                         // Server principal name
			RPC_C_AUTHN_LEVEL_CALL,       // authentication level
			RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
			NULL,                         // client identity 
			EOAC_NONE                     // proxy capabilities     
			);

		if (FAILED(hres))
		{
			warnf(TEXT("Could not set proxy blanket. Err=%d"), hres);
			pSvc->Release();
			pLoc->Release();     
			CoUninitialize();
			return FALSE;               // Program has failed.
		}


		memset(&OutInfo, 0 ,sizeof(OutInfo));
		OutInfo.wMsg = c2g_client_hardware_info;
		UBOOL Ret = GetCpuInfo(OutInfo, pSvc, bLogToFile);

		Ret = GetMemoryInfo(OutInfo, pSvc, bLogToFile);

		//MonitorInfo monitor;
		//Ret = GetDesktopMonitorInfo(monitor, pSvc);

		Ret = GetVideoInfo(OutInfo, pSvc, bLogToFile);

		Ret = GetDiskInfo(OutInfo, pSvc, bLogToFile);

		Ret = GetNetworkAdapterInfo(OutInfo, pSvc, bLogToFile);



		// Cleanup
		// ========

		pSvc->Release();
		pLoc->Release();     
		CoUninitialize();

		return TRUE;
	}

};




#endif
