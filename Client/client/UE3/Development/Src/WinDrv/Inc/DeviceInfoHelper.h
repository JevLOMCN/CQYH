#ifndef _DEVICE_INFO_HELPER_H_
#define _DEVICE_INFO_HELPER_H_

#include <comdef.h>
#include <Wbemidl.h>
#include "../../../../../../Common/utility/fixed_string.h"

# pragma comment(lib, "wbemuuid.lib")

#include "..\..\..\..\..\..\Common\message\c2g_message.h"
#include "..\..\..\..\..\..\Common\define\\version.h"

using namespace ExternalMessage;	//......

class DeviceInfoHelper
{
	template<class T> class ScopedComPtr
	{
		ScopedComPtr(const ScopedComPtr&);
		ScopedComPtr& operator=(const ScopedComPtr&);
	public:
		T* Value;
		ScopedComPtr() : Value(NULL) {}
		~ScopedComPtr() { if (Value) Value->Release(); }
	};
	class ScopedVariant
	{
		ScopedVariant(const ScopedVariant&);
		ScopedVariant& operator=(const ScopedVariant&);
	public:
		VARIANT Value;
		ScopedVariant() { VariantInit(&Value); }
		~ScopedVariant() { VariantClear(&Value); }
	};
	class ScopedComInit
	{
		ScopedComInit(const ScopedComInit&);
		ScopedComInit& operator=(const ScopedComInit&);
	public:
		ScopedComInit() {}
		~ScopedComInit() { CoUninitialize(); }
	};
	static HRESULT ReadProperty(IWbemClassObject* object, const WCHAR* name, VARTYPE type, VARIANT* value)
	{
		VariantClear(value);
		HRESULT result = object->Get(name, 0, value, 0, 0);
		if (FAILED(result) || value->vt == VT_EMPTY || value->vt == VT_NULL) return E_FAIL;
		if (type == VT_BSTR)
			return value->vt == VT_BSTR && value->bstrVal ? S_OK : E_INVALIDARG;
		if (value->vt != type)
			return VariantChangeType(value, value, 0, type);
		return S_OK;
	}

	UBOOL GetCpuInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc, UBOOL bLogToFile)
	{
		if (!InSvc) return FALSE;
		ScopedComPtr<IEnumWbemClassObject> enumerator;
		IEnumWbemClassObject*& pEnumerator = enumerator.Value;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_Processor"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres) || !pEnumerator)
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;
		UBOOL found = FALSE;
		while(pEnumerator)
		{
			ScopedComPtr<IWbemClassObject> row;
			uReturn = 0;
			hres = pEnumerator->Next(WBEM_INFINITE, 1, &row.Value, &uReturn);
			pclsObj = row.Value;

			if (FAILED(hres)) return FALSE;
			if (uReturn == 0) return found;
			if (uReturn != 1 || !pclsObj) return FALSE;
			found = TRUE;

			ScopedVariant property;
			VARIANT& vtProp = property.Value;

			if (bLogToFile) GLog->Logf(TEXT("Cpu Info:"));

			// Get the value of the Name property
			hres = ReadProperty(pclsObj, L"Name", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf(TEXT("    Name:%s"), vtProp.bstrVal);
				ClientFixedString::Assign(Info.stcpuinfo.Name, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"Family", VT_UI2, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Family:%d")), vtProp.uiVal);
				Info.stcpuinfo.Family = vtProp.uiVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"Level", VT_UI2, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Level:%d")), vtProp.uiVal);
				Info.stcpuinfo.Level = vtProp.uiVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"Revision", VT_UI2, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Revision:%d")), vtProp.uiVal);
				Info.stcpuinfo.Revision = vtProp.uiVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"Version", VT_BSTR, &vtProp);
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
			ClientFixedString::Assign(Info.stcpuinfo.Version, TCHAR_TO_UTF8(*osInfo));

			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"Stepping", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Stepping:%s")), vtProp.bstrVal);
				//strcpy_s(Info.stcpuinfo.Stepping, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}

			extern FString GetPlayerRoleName();
			FString roleName = GetPlayerRoleName();
			ClientFixedString::Assign(Info.stcpuinfo.Stepping, TCHAR_TO_UTF8(*roleName));

			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"AddressWidth", VT_UI2, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    AddressWidth:%d")), vtProp.uiVal);
				Info.stcpuinfo.AddressWidth = vtProp.uiVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"NumberOfCores", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    NumberOfCores:%d")), vtProp.uintVal);
				Info.stcpuinfo.NumberOfCores = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"NumberOfLogicalProcessors", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    NumberOfLogicalProcessors:%d")), vtProp.uintVal);
				Info.stcpuinfo.NumberOfLogicalProcessors = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"MaxClockSpeed", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    MaxClockSpeed:%d")), vtProp.uintVal);
				Info.stcpuinfo.MaxClockSpeed = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"L2CacheSize", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    L2CacheSize:%d")), vtProp.uintVal);
				Info.stcpuinfo.L2CacheSize = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"L3CacheSize", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    L3CacheSize:%d")), vtProp.uintVal);
				Info.stcpuinfo.L3CacheSize = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			break;

		}
		return TRUE;
	}

	UBOOL GetMemoryInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc, UBOOL bLogToFile)
	{
		if (!InSvc) return FALSE;
		ScopedComPtr<IEnumWbemClassObject> enumerator;
		IEnumWbemClassObject*& pEnumerator = enumerator.Value;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_PhysicalMemory"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres) || !pEnumerator)
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;
		UBOOL found = FALSE;

		while(pEnumerator)
		{

			ScopedComPtr<IWbemClassObject> row;
			uReturn = 0;
			hres = pEnumerator->Next(WBEM_INFINITE, 1, &row.Value, &uReturn);
			pclsObj = row.Value;

			if (FAILED(hres)) return FALSE;
			if (uReturn == 0) return found;
			if (uReturn != 1 || !pclsObj) return FALSE;
			found = TRUE;

			if (bLogToFile) GLog->Logf((TEXT("Memory Info:")));

			ScopedVariant property;
			VARIANT& vtProp = property.Value;

			hres = ReadProperty(pclsObj, L"Capacity", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Capacity:%s")), vtProp.bstrVal);
				Info.stmemoryinfo.Capacity += _wtoi64(vtProp.bstrVal);
			}

			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"TypeDetail", VT_UI2, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    TypeDetail:%u")), vtProp.uiVal);
				Info.stmemoryinfo.MemoryType = vtProp.uiVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"Speed", VT_UI4, &vtProp);
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
		if (!InSvc) return FALSE;
		ScopedComPtr<IEnumWbemClassObject> enumerator;
		IEnumWbemClassObject*& pEnumerator = enumerator.Value;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_VideoController"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres) || !pEnumerator)
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;
		UBOOL found = FALSE;

		while(pEnumerator)
		{

			ScopedComPtr<IWbemClassObject> row;
			uReturn = 0;
			hres = pEnumerator->Next(WBEM_INFINITE, 1, &row.Value, &uReturn);
			pclsObj = row.Value;

			if (FAILED(hres)) return FALSE;
			if (uReturn == 0) return found;
			if (uReturn != 1 || !pclsObj) return FALSE;
			found = TRUE;

			if (bLogToFile) GLog->Logf((TEXT("Video Info:")));
			ScopedVariant property;
			VARIANT& vtProp = property.Value;



			hres = ReadProperty(pclsObj, L"Name", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Name:%s")), vtProp.bstrVal);
				ClientFixedString::Assign(Info.stvideoinfo.Name, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"AdapterRAM", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    AdapterRAM:%u")), vtProp.uintVal);
				Info.stvideoinfo.AdapterRAM = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"VideoMemoryType", VT_UI2, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    VideoMemoryType:%u")), vtProp.uiVal);
				Info.stvideoinfo.VideoMemoryType = vtProp.uiVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"CurrentBitsPerPixel", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    CurrentBitsPerPixel:%u")), vtProp.uintVal);
				Info.stvideoinfo.CurrentBitsPerPixel = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"CurrentHorizontalResolution", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    CurrentHorizontalResolution:%u")), vtProp.uintVal);
				Info.stvideoinfo.CurrentHorizontalResolution = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"CurrentRefreshRate", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    CurrentRefreshRate:%u")), vtProp.uintVal);
				Info.stvideoinfo.CurrentRefreshRate = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"CurrentVerticalResolution", VT_UI4, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    CurrentVerticalResolution:%u")), vtProp.uintVal);
				Info.stvideoinfo.CurrentVerticalResolution = vtProp.uintVal;
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"VideoProcessor", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    VideoProcessor:%s")), vtProp.bstrVal);
				ClientFixedString::Assign(Info.stvideoinfo.VideoProcessor, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			break;
		}
		return TRUE;

	}

	UBOOL GetDiskInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc, UBOOL bLogToFile)
	{
		if (!InSvc) return FALSE;
		ScopedComPtr<IEnumWbemClassObject> enumerator;
		IEnumWbemClassObject*& pEnumerator = enumerator.Value;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_DiskDrive"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres) || !pEnumerator)
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;
		UBOOL found = FALSE;

		while(pEnumerator)
		{

			ScopedComPtr<IWbemClassObject> row;
			uReturn = 0;
			hres = pEnumerator->Next(WBEM_INFINITE, 1, &row.Value, &uReturn);
			pclsObj = row.Value;

			if (FAILED(hres)) return FALSE;
			if (uReturn == 0) return found;
			if (uReturn != 1 || !pclsObj) return FALSE;
			found = TRUE;

			if (bLogToFile) GLog->Logf((TEXT("Disk Info:")));

			ScopedVariant property;
			VARIANT& vtProp = property.Value;



			hres = ReadProperty(pclsObj, L"MediaType", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    MediaType:%s")), vtProp.bstrVal);
				//strcpy_s(Info.stdiskinfo.MediaType, MAX_DEVICE_STRING_LEN, TCHAR_TO_UTF8(vtProp.bstrVal));
			}

			FString  strBUILD_NUMBER = appItoa(BUILD_NUMBER);
			ClientFixedString::Assign(Info.stdiskinfo.MediaType, TCHAR_TO_UTF8(*strBUILD_NUMBER));

			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"Size", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Size:%s")), vtProp.bstrVal);
				//Info.stdiskinfo.Size = _wtoi64(vtProp.bstrVal);
			}

			MEMORYSTATUSEX M = {0};
			M.dwLength = sizeof(MEMORYSTATUSEX);
			if (GlobalMemoryStatusEx(&M))
				Info.stdiskinfo.Size = M.ullTotalVirtual;

			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"Model", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Model:%s")), vtProp.bstrVal);
				ClientFixedString::Assign(Info.stdiskinfo.Model, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			break;
		}
		return true;

	}


	UBOOL GetNetworkAdapterInfo(C2G_CLIENT_HARDWARE_INFO& Info, IWbemServices *InSvc, UBOOL bLogToFile)
	{
		if (!InSvc) return FALSE;
		ScopedComPtr<IEnumWbemClassObject> enumerator;
		IEnumWbemClassObject*& pEnumerator = enumerator.Value;
		HRESULT hres = InSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_NetworkAdapter"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
			NULL,&pEnumerator);

		if (FAILED(hres) || !pEnumerator)
		{
			return FALSE;
		}

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;
		UBOOL found = FALSE;

		while(pEnumerator)
		{

			ScopedComPtr<IWbemClassObject> row;
			uReturn = 0;
			hres = pEnumerator->Next(WBEM_INFINITE, 1, &row.Value, &uReturn);
			pclsObj = row.Value;

			if (FAILED(hres)) return FALSE;
			if (uReturn == 0) return found;
			if (uReturn != 1 || !pclsObj) return FALSE;
			found = TRUE;

			ScopedVariant property;
			VARIANT& vtProp = property.Value;


			hres = ReadProperty(pclsObj, L"PhysicalAdapter", VT_BOOL, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if(vtProp.boolVal == false)
				{
					continue;
				}
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"NetEnabled", VT_BOOL, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if(vtProp.boolVal == false)
				{
					continue;
				}
			}
			VariantClear(&vtProp);

			if (bLogToFile) GLog->Logf((TEXT("NetworkAdapter Info:")));



			hres = ReadProperty(pclsObj, L"Name", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    Name:%s")), vtProp.bstrVal);
				ClientFixedString::Assign(Info.stnetworkadapterinfo.Name, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"MACAddress", VT_BSTR, &vtProp);
			if (SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    MACAddress:%s")), vtProp.bstrVal);
				ClientFixedString::Assign(Info.stnetworkadapterinfo.NetworkAdapterInfo, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			//   hres = ReadProperty(pclsObj, L"MaxSpeed", VT_BSTR, &vtProp);
			//if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			//{
			//	Info.MaxSpeed = _wtoi64(vtProp.bstrVal);
			//}
			//VariantClear(&vtProp);

			hres = ReadProperty(pclsObj, L"AdapterType", VT_BSTR, &vtProp);
			if(SUCCEEDED(hres) && vtProp.vt != VT_NULL)
			{
				if (bLogToFile) GLog->Logf((TEXT("    AdapterType:%s")), vtProp.bstrVal);
				ClientFixedString::Assign(Info.stnetworkadapterinfo.AdapterType, TCHAR_TO_UTF8(vtProp.bstrVal));
			}
			VariantClear(&vtProp);

			break;
		}
		return TRUE;

	}

	public:

	UBOOL GetHardwareInfo(C2G_CLIENT_HARDWARE_INFO& OutInfo, UBOOL bLogToFile = FALSE)
	{
		memset(&OutInfo, 0, sizeof(OutInfo));
		OutInfo.wMsg = c2g_client_hardware_info;
		HRESULT hres =  CoInitializeEx(0, COINIT_APARTMENTTHREADED); 
		if (FAILED(hres))
		{
			warnf(TEXT("Failed to initialize COM library. Err=%d"), hres);
			return FALSE;              // Program has failed.
		}

		ScopedComInit comInitialization;

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
			return FALSE;          // Program has failed.
		}

		// Obtain the initial locator to Windows Management
		// on a particular host computer.
		ScopedComPtr<IWbemLocator> locator;
		IWbemLocator*& pLoc = locator.Value;

		hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &pLoc);

		if (FAILED(hres) || !pLoc)
		{
			warnf(TEXT("Failed to create IWbemLocator object. Err=%d"), hres);
			//cout << "Failed to create IWbemLocator object. "<< "Error code = 0x"<< hex << hres << endl;
			return FALSE;       // Program has failed.
		}

		ScopedComPtr<IWbemServices> services;
		IWbemServices*& pSvc = services.Value;

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

		if (FAILED(hres) || !pSvc)
		{
			warnf(TEXT("Could not connect. Err=%d"), hres);
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
			return FALSE;               // Program has failed.
		}



		UBOOL Ret = GetCpuInfo(OutInfo, pSvc, bLogToFile);

		Ret = GetMemoryInfo(OutInfo, pSvc, bLogToFile);

		//MonitorInfo monitor;
		//Ret = GetDesktopMonitorInfo(monitor, pSvc);

		Ret = GetVideoInfo(OutInfo, pSvc, bLogToFile);

		Ret = GetDiskInfo(OutInfo, pSvc, bLogToFile);

		Ret = GetNetworkAdapterInfo(OutInfo, pSvc, bLogToFile);



		// Cleanup
		// ========

		return TRUE;
	}

};




#endif
