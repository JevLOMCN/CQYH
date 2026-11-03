//------------------------------------------------------------------------
// oscar@snda
//------------------------------------------------------------------------

#include "UnrealEdCLR.h"
#include "MServerMapCfgExport.h"

#if UE3_WX32 && WITH_MANAGED_CODE
#include "ManagedCodeSupportCLR.h"
#endif

#if WITH_EDITOR

void MServerMapCfgExport::ExportToExcel(
	const TArray< TArray<FString> >& CfgData,
	const FString& ExcelFile,
	const FString& ExportFile)
{
#if WITH_MANAGED_CODE
	
	array<String^, 2>^ datList = CLRTools::ToStringArray2D(CfgData);
	UnrealEdCSharp::Excel::ExcelUtil^ excelUtil = gcnew UnrealEdCSharp::Excel::ExcelUtil();
	excelUtil->SaveAsExcel(datList, CLRTools::ToString(ExcelFile));

#endif
}


void MServerMapCfgExport::ImportSceneConfigFromExcel(const FString& sceneFile, TArray< TArray<FString> >& sceneDat,
	const FString& dunFile, TArray< TArray<FString> >& dungeonDat)
{
#if WITH_MANAGED_CODE
	{
		UnrealEdCSharp::Excel::ExcelUtil^ excelUtil = gcnew UnrealEdCSharp::Excel::ExcelUtil();
		List< List<String^>^ > ^ excelDat;
		excelUtil->LoadFromExcel(CLRTools::ToString(sceneFile), excelDat);
		CLRTools::ToFStringArray2D(excelDat, sceneDat);
	}

	{
		UnrealEdCSharp::Excel::ExcelUtil^ excelUtil = gcnew UnrealEdCSharp::Excel::ExcelUtil();
		List< List<String^>^ > ^ excelDat;
		excelUtil->LoadFromExcel(CLRTools::ToString(dunFile), excelDat);
		CLRTools::ToFStringArray2D(excelDat, dungeonDat);
	}
#endif
}

#endif