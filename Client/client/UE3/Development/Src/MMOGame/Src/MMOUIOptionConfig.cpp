#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOUtil.h"
#include "EngineAudioDeviceClasses.h"
#include "MMODropManager.h"
#include "MMOGuideManager.h"
#include "MMOWeatherManager.h"
#include "ReplayManager.h"
#include "DebugMMO.h"

#include "MMOMsgBoxManager.h"
#include "D3D9Drv.h"
#include "../../Engine/Src/SceneRenderTargets.h"

IMPLEMENT_CLASS(UMMOUIOptionConfig)


struct FDisplayResolution
{
	INT Width;
	INT Height;

    bool operator == (const FDisplayResolution& r) const
    {
        return r.Width == Width && r.Height == Height;
    }
};

typedef std::vector< FDisplayResolution > VEC_DISPLAY;
static VEC_DISPLAY g_vecDisplay;

FDisplayResolution g_lastSelectedRes = {0, 0};

class VLevelData
{
public:
	virtual UBOOL GetBoolData(INT nLevel){return FALSE;};
	virtual INT   GetIntData(INT nLevel){return 0;};
	virtual FLOAT GetFloatData(INT nLevel){return 0.0f;};

	virtual FLOAT GetMinValue() const { return 0.0f; }
	virtual FLOAT GetMaxValue() const { return 1.0f; }
};

class INTLevelData : public VLevelData
{
public:
	INTLevelData(INT nV0,INT nV1,INT nV2,INT nV3,INT nV4)
	{
		arrLevel[0] = nV0;arrLevel[1] = nV1;arrLevel[2] = nV2;arrLevel[3] = nV3;arrLevel[4] = nV4;
	}
	virtual INT   GetIntData(INT nLevel){return arrLevel[nLevel];};

	INT arrLevel[5];

	virtual FLOAT GetMinValue() const { return arrLevel[0]; }
	virtual FLOAT GetMaxValue() const { return arrLevel[4]; }
};

class BOOLLevelData : public VLevelData
{
public:
	BOOLLevelData(BOOL nV0,BOOL nV1,BOOL nV2,BOOL nV3,BOOL nV4)
	{
		arrLevel[0] = nV0;arrLevel[1] = nV1;arrLevel[2] = nV2;arrLevel[3] = nV3;arrLevel[4] = nV4;
	}
	virtual UBOOL GetBoolData(INT nLevel){return arrLevel[nLevel];};

	BOOL arrLevel[5];
};

class FLOATLevelData : public VLevelData
{
public:
	FLOATLevelData(FLOAT nV0,FLOAT nV1,FLOAT nV2,FLOAT nV3,FLOAT nV4)
	{
		arrLevel[0] = nV0;arrLevel[1] = nV1;arrLevel[2] = nV2;arrLevel[3] = nV3;arrLevel[4] = nV4;
	}

	virtual FLOAT GetFloatData(INT nLevel){return arrLevel[nLevel];};
	FLOAT arrLevel[5];
};


struct FDisplayDetailOption
{
	FString SettingName;
	ESystemSettingType SysSettingType;
	VLevelData* levelData;
	BOOL bInAdvanceList;
	FSystemSetting* SystemSetting;

	FDisplayDetailOption(const FString& inSettingName, ESystemSettingType inSysSettingType,
		VLevelData* inLevelData, BOOL inbInAdvanceList, FSystemSetting* inSystemSetting)
		: SettingName(inSettingName)
		, SysSettingType(inSysSettingType)
		, levelData(inLevelData)
		, bInAdvanceList(inbInAdvanceList)
		, SystemSetting(inSystemSetting)
	{

	}
};

TArray<FDisplayDetailOption> g_displayDetailOptions;

INTLevelData	VShadowQuality(1, 1, 2, 2, 3);
FLOATLevelData	VFRestrictWholeSceneDynamicShadowDistance(3800,3800,3800,4000,5000);
BOOLLevelData	 VFRenderHDR(FALSE,FALSE,TRUE,TRUE,TRUE);
BOOLLevelData	 VFDynamicShadows(FALSE,FALSE,TRUE,TRUE,TRUE);
BOOLLevelData	 VFMotionBlur(FALSE,FALSE,TRUE,TRUE,TRUE);
BOOLLevelData	 VFDepthOfField(FALSE,FALSE,FALSE,FALSE,TRUE);
BOOLLevelData	 VFAmbientOcclusion(FALSE,FALSE,TRUE,TRUE,TRUE);
BOOLLevelData	 VFBloom(FALSE,FALSE,TRUE,TRUE,TRUE);
BOOLLevelData	 VFbAllowLightShafts(FALSE,FALSE,TRUE,TRUE,TRUE);
//BOOLLevelData	 VFAllowPostProcessAA(FALSE,FALSE,TRUE,TRUE,TRUE);	// always allow AA
// #if M_MMO_HD
//INTLevelData	VFTextureLODDrop(2,3,3,3,3);
// #else
// INTLevelData	VFTextureLODDrop(0,1,2,3,3);
// #endif

//BOOLLevelData	 VFbUseShadowmaps(FALSE,FALSE,TRUE,TRUE,TRUE);
BOOLLevelData	 VFAllowInteractiveFoliage(FALSE,FALSE,TRUE,TRUE,TRUE);
//
BOOLLevelData	 VFAutoAdjustPerformance(TRUE,TRUE,TRUE,TRUE,TRUE);
BOOLLevelData	 VFHintBeforeAutoAdjust(TRUE,TRUE,TRUE,TRUE,TRUE);
//INTLevelData	VFSleepTimeMS(15,10,2,1,0);
INTLevelData	VFAutoPerformanceCheckTime(10,20,30,40,50);
INTLevelData	VFLowerCheckFPS(20,25,30,40,50);
INTLevelData	VFBackGroundFPS(20,25,30,35,45);
INTLevelData	VFForeGroundFPS(60,60,60,60,60);

INTLevelData	VFSkeletalMeshLODBiasHD(1,1,2,2,2);
INTLevelData    VFParticleLODBiasHD(0,1,2,2,2);
INTLevelData    VFStaticMeshLODBiasHD(0,1,1,2,2);
BOOLLevelData   VFShowFoliageInstanceHD(FALSE,TRUE,TRUE,TRUE,TRUE);

INTLevelData	VFSkeletalMeshLODBias(0,0,1,1,1);
INTLevelData    VFParticleLODBias(0,0,0,1,2);
INTLevelData    VFStaticMeshLODBias(0,0,0,1,2);
BOOLLevelData   VFShowFoliageInstance(FALSE,FALSE,FALSE,FALSE,TRUE);
// can't adjust this setting at run-time!
//BOOLLevelData	VAllowDirectionalLightMaps(FALSE,FALSE,TRUE,TRUE,TRUE);



VLevelData* GetLevelDataBySetName(FString strName)
{
	for (INT i = 0,j = 0; i < g_displayDetailOptions.Num(); ++i)
	{
		const FDisplayDetailOption& opt = g_displayDetailOptions(i);
		if(opt.SettingName == strName)
			return opt.levelData;
	}
	return NULL;
}


/*
	settingName  setting keyword
	settingType  value type of the setting
	setLevel     level data 1-5
	inType       whether to show in the advanced settings list on the right
*/
#define M_FindSysSetting(settingName, settingType,setLevel,inType) displayDetailOptions.AddItem(FDisplayDetailOption(TEXT(settingName), settingType, setLevel,inType,NULL))

void InitDisplayDetailOptionList()
{	
	// Pick here from FSystemSetting FSystemSettings::SystemSettings[] in SystemSettings.cpp
	TArray<FDisplayDetailOption> displayDetailOptions;
	{
		//M_FindSysSetting("DrawSilhouettes", SST_BOOL),
		M_FindSysSetting("ForeGroundFPS", SST_INT,&VFForeGroundFPS,TRUE);
		M_FindSysSetting("BackGroundFPS", SST_INT,&VFBackGroundFPS,TRUE);

		M_FindSysSetting("ShadowQuality", SST_INT, &VShadowQuality, TRUE);
		M_FindSysSetting("RestrictWholeSceneDynamicShadowDistance", SST_FLOAT,&VFRestrictWholeSceneDynamicShadowDistance,TRUE);
		//M_FindSysSetting("BackGroundFPS", SST_INT,&VFBackGroundFPS,TRUE);
		if(M_MMO_HD)
		{
			INTLevelData* VFMaxShowPlayerCount = new INTLevelData(100,120,160,200,200);
			M_FindSysSetting("MaxShowPlayerCount", SST_INT,VFMaxShowPlayerCount,TRUE);
			
			INTLevelData* VFTextureLODDrop = new INTLevelData(0,1,1,1,1);
			M_FindSysSetting("NumMipsToDrop", SST_INT,VFTextureLODDrop,FALSE);

			M_FindSysSetting("SkeletalMeshLODBias", SST_INT, &VFSkeletalMeshLODBiasHD,TRUE);
			M_FindSysSetting("ParticleLODBias", SST_INT, &VFParticleLODBiasHD,TRUE);
			M_FindSysSetting("StaticMeshLODBias", SST_INT, &VFStaticMeshLODBiasHD,TRUE);
			M_FindSysSetting("ShowFoliageInstance", SST_BOOL, &VFShowFoliageInstanceHD,TRUE);

			BOOLLevelData* VFbAllowHighQualityMaterials = new BOOLLevelData(TRUE,TRUE,TRUE,TRUE,TRUE);
			M_FindSysSetting("bAllowHighQualityMaterials", SST_BOOL, VFbAllowHighQualityMaterials,TRUE);
		}
		else
		{
			INTLevelData* VFMaxShowPlayerCount = new INTLevelData(10,30,50,80,100);
			M_FindSysSetting("MaxShowPlayerCount", SST_INT,VFMaxShowPlayerCount,TRUE);


			INTLevelData* VFTextureLODDrop = new INTLevelData(0,0,0,1,1);
			M_FindSysSetting("NumMipsToDrop", SST_INT,VFTextureLODDrop,TRUE);

			M_FindSysSetting("SkeletalMeshLODBias", SST_INT, &VFSkeletalMeshLODBias,TRUE);
			M_FindSysSetting("ParticleLODBias", SST_INT, &VFParticleLODBias,TRUE);
			M_FindSysSetting("StaticMeshLODBias", SST_INT, &VFStaticMeshLODBias,TRUE);
			M_FindSysSetting("ShowFoliageInstance", SST_BOOL, &VFShowFoliageInstance,TRUE);

			BOOLLevelData* VFbAllowHighQualityMaterials = new BOOLLevelData(FALSE,FALSE,FALSE,FALSE,TRUE);
			M_FindSysSetting("bAllowHighQualityMaterials", SST_BOOL, VFbAllowHighQualityMaterials, FALSE);			
		}

		//M_FindSysSetting("SleepTimeMS", SST_INT);
		// Grouped by light, shadow, effects, hardware features

		M_FindSysSetting("RenderHDR", SST_BOOL,&VFRenderHDR,TRUE);

		M_FindSysSetting("DynamicShadows", SST_BOOL,&VFDynamicShadows,TRUE);
		//M_FindSysSetting("bUseShadowmaps", SST_BOOL,&VFbUseShadowmaps,TRUE);
		M_FindSysSetting("AmbientOcclusion", SST_BOOL,&VFAmbientOcclusion,TRUE);

#if !SHIPPING_PC_GAME
		// This is only used for debugging
		//M_FindSysSetting("RestrictWholeSceneDynamicShadowParameters", SST_BOOL);
#endif

		M_FindSysSetting("Bloom", SST_BOOL,&VFBloom,TRUE);
		M_FindSysSetting("bAllowLightShafts", SST_BOOL,&VFbAllowLightShafts,TRUE);

		if(M_MMO_HD)
		{
			BOOLLevelData* VFDistortion = new BOOLLevelData(FALSE,FALSE,TRUE,TRUE,TRUE);
			M_FindSysSetting("Distortion", SST_BOOL, VFDistortion,TRUE);
		}
		else
		{
			BOOLLevelData* VFDistortion = new BOOLLevelData(FALSE,FALSE,FALSE,FALSE,TRUE);
			M_FindSysSetting("Distortion", SST_BOOL, VFDistortion,FALSE);
		}

		M_FindSysSetting("MotionBlur", SST_BOOL,&VFMotionBlur,TRUE);
		M_FindSysSetting("DepthOfField", SST_BOOL,&VFDepthOfField,TRUE);
		//M_FindSysSetting("AllowPostProcessAA", SST_BOOL,&VFAllowPostProcessAA,TRUE);
		M_FindSysSetting("AllowInteractiveFoliage", SST_BOOL,&VFAllowInteractiveFoliage,TRUE);

		//
		M_FindSysSetting("AutoAdjustPerformance", SST_BOOL,&VFAutoAdjustPerformance,FALSE);
		M_FindSysSetting("HintBeforeAutoAdjust", SST_BOOL,&VFHintBeforeAutoAdjust,FALSE);
		//M_FindSysSetting("SleepTimeMS", SST_INT,&VFSleepTimeMS,FALSE);
		M_FindSysSetting("AutoPerformanceCheckTimeNew", SST_INT,&VFAutoPerformanceCheckTime,FALSE);
		M_FindSysSetting("LowerCheckFPS", SST_INT,&VFLowerCheckFPS,FALSE);
		//M_FindSysSetting("DirectionalLightmaps", SST_BOOL,&VAllowDirectionalLightMaps,FALSE),
	}

	g_displayDetailOptions.Empty(0);

	for (INT i = 0; i < displayDetailOptions.Num(); ++i)
	{
		displayDetailOptions(i).SystemSetting = GSystemSettings.FindSystemSetting(displayDetailOptions(i).SettingName, displayDetailOptions(i).SysSettingType);
		g_displayDetailOptions.AddItem(displayDetailOptions(i));
	}
}

UGFxObject* UMMOUIOptionConfig::CreateDisplayDetailOptionList()
{
	// Need to implement the corresponding FLASH data structure and create it here as a data list to pass in; for those needing a display name, use DisplayDetailOption.SettingsName as the format to localize in stringmap
	 UGFxObject* datList = GFXPlayer->CreateArray();
	 for (INT i = 0,j = 0; i < g_displayDetailOptions.Num(); ++i)
	 {
		 // fill option data
		 const FDisplayDetailOption& opt = g_displayDetailOptions(i);
		 if(opt.SystemSetting == NULL)
			 continue;

		 if(!(opt.bInAdvanceList))
			 continue;

		 UGFxObject* dat = GFXPlayer->CreateObject(TEXT("ActionScript3.Option.AdvancedVideo.OptionItemData"));
		 if(dat)
		 {
			 dat->SetString(TEXT("optionKey"),opt.SettingName);
			 dat->SetInt(TEXT("type"),opt.SysSettingType);

			 FString strDisplay =  FString::Printf(TEXT("MMOGame.Option.SettingsName.%s"),*(opt.SettingName));
			 strDisplay = MMOUI::ParseLocalizedPropertyPath(strDisplay);
			 dat->SetString(TEXT("title"),strDisplay);

			 InitSystemSettingToUI(dat,opt.SystemSetting,-1);
			
			 datList->SetElementObject(j,dat);
			 j++;
		 }
	 }
	 return datList;
}

void UMMOUIOptionConfig::ApplyAdvanceSettingByUI(UGFxObject* datList)
{
	if(datList == NULL)
		return;
	for (INT n = 0;datList->GetElementObject(n)!= NULL; ++n)
	{
		UGFxObject* dat = datList->GetElementObject(n);
		if(dat)
		{
			FSystemSetting* pkSystem = NULL;
			FString strDis = dat->GetString(TEXT("optionKey"));
			for (INT i = 0; i < g_displayDetailOptions.Num(); ++i)
			{
				if(strDis == g_displayDetailOptions(i).SettingName)	
				{
					pkSystem = g_displayDetailOptions(i).SystemSetting;
					break;
				}
			}
			if(pkSystem == NULL)
				continue;
			switch(dat->GetInt(TEXT("type")))
			{
			case SST_INT:
				{
					INT nValue = (INT)dat->GetFloat(TEXT("sliderValue"));
					FVSSGenericInt* pkFVS = (FVSSGenericInt*)(pkSystem->SettingUpdate);
					if(pkSystem->SettingUpdate&&pkFVS)
					{
						nValue = Clamp(nValue,pkFVS->Min,pkFVS->Max);
					}
					INT nCurValue = 0;
					GSystemSettings.GetInt(pkSystem->SettingName,nCurValue);
					if(nValue != nCurValue)
					{
						if(FString(pkSystem->SettingName) == TEXT("NumMipsToDrop"))
						{
							FScriptDelegate funYes(EC_EventParm);
							AMMOPlayerController *pc = MMOFacade::getSingleton().GetMainController();
							funYes.Object = pc;
							funYes.FunctionName = TEXT("OnDefaultOK");	

							UGFxObject* MsgboxWindow = MMOMsgBoxManager::getSingleton().AddModalMessageBox(
								TEXT("Changing texture quality requires a game restart to take effect!"), 0, funYes, funYes, FALSE);
						}

						GSystemSettings.SetInt(pkSystem->SettingName,nValue);
					}
					break;
				}
			case SST_BOOL:
				{
					UBOOL bValue = (UBOOL)dat->GetBool(TEXT("checkButtonSelected"));
					UBOOL bCurValue = FALSE;
					GSystemSettings.GetBool(pkSystem->SettingName,bCurValue);
					if (bCurValue != bValue)
						GSystemSettings.SetBool(pkSystem->SettingName,bValue);
					break;
				}
			case SST_FLOAT:
				{
					FLOAT fValue = (FLOAT)dat->GetFloat(TEXT("sliderValue"));
					FVSSGenericFloat* pkFVS = (FVSSGenericFloat*)(pkSystem->SettingUpdate);
					if(pkSystem->SettingUpdate&&pkFVS)
					{
						fValue = Clamp(fValue,pkFVS->Min,pkFVS->Max);
					}
					FLOAT fCurValue = 0.0f;
					GSystemSettings.GetFloat(pkSystem->SettingName,fCurValue);
					if(abs(fValue - fCurValue) > 0.00001f)
						GSystemSettings.SetFloat(pkSystem->SettingName,fValue);
					break;
				}
			case SST_ENUM:
				{
					break;
				}
			case SST_STRING:
				{
					break;
				}
			}
		}
	}
}
void UMMOUIOptionConfig::InitSystemSettingToUI(UGFxObject* dat,FSystemSetting* pkSystem,INT nLevel)
{
	if(dat == NULL || pkSystem == NULL)
		return;

	switch(pkSystem->SettingType)
	{
	case SST_INT:
		{
			FVSSGenericInt* pkFVS = (FVSSGenericInt*)(pkSystem->SettingUpdate);
			if(pkFVS)
			{
				dat->SetFloat(TEXT("MinValue"),pkFVS->Min);
				dat->SetFloat(TEXT("MaxValue"),pkFVS->Max);
				dat->SetFloat(TEXT("byDetalStep"),pkFVS->Step);
			}
			if(nLevel >= 0)
			{
				INTLevelData *pkData = (INTLevelData*)GetLevelDataBySetName(pkSystem->SettingName);
				if(pkData)
				{
					INT nVa = pkData->GetIntData(nLevel);
					dat->SetFloat(TEXT("sliderValue"),nVa);
					return;
				}
			}
			INT nValue = 0;
			GSystemSettings.GetInt(pkSystem->SettingName,nValue);
			dat->SetFloat(TEXT("sliderValue"),nValue);
			break;
		}
	case SST_FLOAT:
		{
			FVSSGenericFloat* pkFVS = (FVSSGenericFloat*)(pkSystem->SettingUpdate);
			if(pkFVS)
			{
				dat->SetFloat(TEXT("MinValue"),pkFVS->Min);
				dat->SetFloat(TEXT("MaxValue"),pkFVS->Max);
				dat->SetFloat(TEXT("byDetalStep"),pkFVS->Step);
			}
			if(nLevel >= 0)
			{
				FLOATLevelData*pkData = (FLOATLevelData*)GetLevelDataBySetName(pkSystem->SettingName);
				if(pkData)
				{
					FLOAT nVa = pkData->GetFloatData(nLevel);
					dat->SetFloat(TEXT("sliderValue"),nVa);
					return;
				}
			}
			FLOAT nValue = 0.0f;
			GSystemSettings.GetFloat(pkSystem->SettingName,nValue);
			dat->SetFloat(TEXT("sliderValue"),nValue);
			break;
		}
	case SST_BOOL:
		{
			if(nLevel >= 0)
			{
				BOOLLevelData*pkData = (BOOLLevelData*)GetLevelDataBySetName(pkSystem->SettingName);
				if(pkData)
				{
					BOOL nVa = pkData->GetBoolData(nLevel);
					dat->SetBool(TEXT("checkButtonSelected"),nVa);
					return;
				}
			}
			UBOOL bRes = FALSE;
			GSystemSettings.GetBool(pkSystem->SettingName,bRes);
			dat->SetBool(TEXT("checkButtonSelected"),bRes);
			break;
		}
	case SST_ENUM:
		{
			break;
		}
	case SST_STRING:
		{

			break;
		}
	}
}

bool SortDisplayResByWidth(const FDisplayResolution& lhs, const FDisplayResolution& rhs)
{
	if(lhs.Width > rhs.Width)
	{
		return true;
	}

	return false;
}

bool SortDisplayResByHeight(const FDisplayResolution& lhs, const FDisplayResolution& rhs)
{
	if(lhs.Height > rhs.Height)
	{
		if(lhs.Width >= rhs.Width)
		{
			return true;
		}
	}

	return false;
}

extern INT UpdateDisplayResolution(INT curResW, INT curResH)
{
	g_vecDisplay.clear();

	DEVMODE DisplayMode;
	UINT ModeIndex = 0;
	INT selIdx = 0;
	while (EnumDisplaySettings(NULL, ModeIndex++, &DisplayMode))
	{
		if(DisplayMode.dmPelsWidth >= 1280)
		{
            FDisplayResolution dr = {DisplayMode.dmPelsWidth, DisplayMode.dmPelsHeight};
			if(std::find(g_vecDisplay.begin(), g_vecDisplay.end(), dr) == g_vecDisplay.end())
			{
				g_vecDisplay.push_back(dr);
			}
		}
	}

// 	warnf(NAME_Log, TEXT("Before sort by width======================================="));
// 	for(size_t i = 0; i < g_vecDisplay.size(); ++i)
// 	{
// 		warnf(NAME_Log, TEXT("ResSize: %d x %d"), g_vecDisplay[i].Width, g_vecDisplay[i].Height);
// 	}

	std::sort(g_vecDisplay.begin(), g_vecDisplay.end(), SortDisplayResByWidth);

// 	warnf(NAME_Log, TEXT("After sort by width======================================="));
// 	for(size_t i = 0; i < g_vecDisplay.size(); ++i)
// 	{
// 		warnf(NAME_Log, TEXT("ResSize: %d x %d"), g_vecDisplay[i].Width, g_vecDisplay[i].Height);
// 	}

	std::sort(g_vecDisplay.begin(), g_vecDisplay.end(), SortDisplayResByHeight);

// 	warnf(NAME_Log, TEXT("After sort by height======================================="));
// 	for(size_t i = 0; i < g_vecDisplay.size(); ++i)
// 	{
// 		warnf(NAME_Log, TEXT("ResSize: %d x %d"), g_vecDisplay[i].Width, g_vecDisplay[i].Height);
// 	}

	for(size_t i = 0; i < g_vecDisplay.size(); ++i)
	{
		if(curResW == g_vecDisplay[i].Width && curResH == g_vecDisplay[i].Height)
		{
			selIdx = i;

			SET_WARN_COLOR(COLOR_CYAN);
			warnf(NAME_Log, TEXT("======Idx:%d %d x %d"), selIdx, curResW, curResH);
			CLEAR_WARN_COLOR();
		}
	}

	if(selIdx == 0 && curResW > 1280)
	{
		SET_WARN_COLOR(COLOR_CYAN);
		for (size_t i = g_vecDisplay.size() - 1; i > 0; --i)
		{
			if(g_vecDisplay[i].Width > curResW && g_vecDisplay[i].Height > curResH)
			{
				warnf(NAME_Log, TEXT("Find neareast res:%d %d %d"), i, g_vecDisplay[i].Width, g_vecDisplay[i].Height);
				selIdx = i;
				break;
			}
		}
		CLEAR_WARN_COLOR();
	}

	return selIdx;
}

extern void UCSetObjectText(UMMOUIBase* wnd, const TCHAR* ctrl, const FString& txt);

void UMMOUIOptionConfig::NativeInitOnce()
{
    //UGFxObject* obj = GetObject(_T("tab_input"));
    //if (obj)
        //obj->SetVisible(false);
}

void UMMOUIOptionConfig::FillToUI()
{
	UINT resX, resY;
	if(g_lastSelectedRes.Width == 0)
	{
		g_lastSelectedRes.Width = GSystemSettings.ResX;
		g_lastSelectedRes.Height = GSystemSettings.ResY;
	}

	resX = g_lastSelectedRes.Width;
	resY = g_lastSelectedRes.Height;

	if(GSystemSettings.bFullscreen)
	{
		RHIGetSupportedResolution(resX, resY);
	}


    INT selIdx = UpdateDisplayResolution(resX, resY);

	SET_WARN_COLOR(COLOR_CYAN);
	warnf(NAME_Log, TEXT("-----FillToUI Current Idx:%d SysRes: %d x %d -> : %d x %d "), selIdx,
		GSystemSettings.ResX, GSystemSettings.ResY, resX, resY);
	CLEAR_WARN_COLOR();


    UGFxObject* dt = GFXPlayer->CreateObject(TEXT("ActionScript3.Option.OptionData"));
    //check(dt);
	if(dt == NULL)return;
    UGFxObject* size_lst = GFXPlayer->CreateArray();
    //check(size_lst);
	if(size_lst == NULL)return;
    dt->SetString(_T("wnd_title"), MMOUI::ParseLocalizedPropertyPath(_T("MMOGame.Option.Menu.Button.Name.SystemConfig")));

    dt->SetBool(_T("video_enable"), TRUE);
    {
        dt->SetInt(_T("WndFullscreen"), GSystemSettings.bFullscreen ? 1 : 0);

        for (size_t i = 0; i < g_vecDisplay.size(); ++i)
        {
			//warnf(NAME_Log, TEXT("ResSize: %d x %d"), g_vecDisplay[i].Width, g_vecDisplay[i].Height);

			size_lst->SetElementString(i, FString::Printf(TEXT("%dx%d"),
				g_vecDisplay[i].Width, g_vecDisplay[i].Height));
		}

		// get adapter list
		UGFxObject* AdapterList = GFXPlayer->CreateArray();
		if( AdapterList )
		{
			FD3D9DynamicRHI* pRHI = (FD3D9DynamicRHI*)GDynamicRHI;
			if( pRHI )
			{
				const TArray<FString>& desc = pRHI->GetValidVideoCardDescriptions();
				for( INT adapterIdx = 0; adapterIdx < desc.Num(); ++adapterIdx )
					AdapterList->SetElementString( adapterIdx, desc(adapterIdx) );
			}
		}

		// set to UI
		dt->SetObject( _T("GPU_List"), AdapterList );

		dt->SetBool( _T("WndSizeFixed"), M_MMO_HD ? FALSE : TRUE );

		// set selected GPU id
		INT SelectedAdapterId = 0;
		if( GConfig->GetInt( TEXT("Engine.Engine"), TEXT("D3DAdapterId"), SelectedAdapterId, GEngineIni ) )
		{
			dt->SetInt( _T("GPU_List_Selected_Idx"), SelectedAdapterId );
		}

		//Overall effect level 1 - 5  0 is custom
		dt->SetInt(_T("DrawEffectLevel"), GSystemSettings.CompatLevelComposite);
		if(GSystemSettings.CompatLevelComposite != 0)
		{
			ASConfigVideoReq(GSystemSettings.CompatLevelComposite);
		}
		else
		{
			UGFxObject* pkAdvanceList = CreateDisplayDetailOptionList();
			dt->SetObject(TEXT("advancedOptionData"),pkAdvanceList);
		}

		const FCompatibilityLevelInfo MachineCompatLevel = appGetCompatibilityLevelV2();
		dt->SetInt(TEXT("RecommandLevel"),MachineCompatLevel.CompositeLevel);

        dt->SetObject(_T("WndSize"), size_lst);
        dt->SetInt(_T("WndSizeIdx"), selIdx);
		
		//Performance tuning settings
		dt->SetBool(TEXT("auto_adjust_perf"), GSystemSettings.bAutoAdjustPerformance);
		dt->SetBool(TEXT("notice_auto_adjust"), GSystemSettings.bHintBeforeAutoAdjust);

		dt->SetFloat(TEXT("adjust_interval_check"),GSystemSettings.AutoPerformanceCheckTimeNew);
		dt->SetFloat(TEXT("adjust_interval_check_Min"),VFAutoPerformanceCheckTime.GetMinValue());
		dt->SetFloat(TEXT("adjust_interval_check_Max"),VFAutoPerformanceCheckTime.GetMaxValue());
		dt->SetFloat(TEXT("adjust_interval_check_Step"),10);

		dt->SetFloat(TEXT("adjust_FPS_check"),GSystemSettings.LowerCheckFPS);
		dt->SetFloat(TEXT("adjust_FPS_check_Min"),VFLowerCheckFPS.GetMinValue());
		dt->SetFloat(TEXT("adjust_FPS_check_Max"),VFLowerCheckFPS.GetMaxValue());
		dt->SetFloat(TEXT("adjust_FPS_check_Step"),5);
    }

    dt->SetBool(_T("sound_enable"), TRUE);
    {
		UMMOUIManager* uimgr = MMOUI::GetUIManager();
		if(uimgr){
			dt->SetBool(_T("SoundAll"),   uimgr->bSoundAll		);
			dt->SetBool(_T("SoundBack"),  uimgr->bSoundBack		);
			//dt->SetBool(_T("SoundEnv"),   uimgr->bSoundEnv		);
			dt->SetBool(_T("SoundVoice"), uimgr->bSoundVoice	);
			dt->SetBool(_T("SoundSfx"), uimgr->bSoundSfx	);
			dt->SetBool(_T("SoundFocusOut"), uimgr->bSoundFocusOut	);
			dt->SetInt(_T("SoundAllVL"),  uimgr->nSoundAllVL	);
			dt->SetInt(_T("SoundBackVL"), uimgr->nSoundBackVL	);
			//dt->SetInt(_T("SoundEnvVL"),  uimgr->nSoundEnvVL	);
			dt->SetInt(_T("SoundVoiceVL"),uimgr->nSoundVoiceVL	);
			dt->SetInt(_T("SoundSfxVL"),uimgr->nSoundSfxVL	);
		}

  //      UAudioDevice* ad = GEngine->GetAudioDevice();
		//if(ad) // oscar@snda TODO: find out why it was not created
		//{
		//	dt->SetBool(_T("SoundAll"),   ad->VolumeTotal_op    );
		//	dt->SetBool(_T("SoundBack"),  ad->VolumeBackMusic_op);
		//	dt->SetBool(_T("SoundEnv"),   ad->VolumeEnv_op      );
		//	dt->SetBool(_T("SoundInput"), ad->VolumeVoice_op    );
		//	dt->SetBool(_T("SoundOther"), ad->VolumeSfx_op    );
		//	dt->SetInt(_T("SoundAllVL"),   (INT)Clamp(ad->VolumeTotal     * 100.0f, 0.0f, 100.0f));
		//	dt->SetInt(_T("SoundBackVL"),  (INT)Clamp(ad->VolumeBackMusic * 100.0f, 0.0f, 100.0f));
		//	dt->SetInt(_T("SoundEnvVL"),   (INT)Clamp(ad->VolumeEnv       * 100.0f, 0.0f, 100.0f));
		//	dt->SetInt(_T("SoundInputVL"), (INT)Clamp(ad->VolumeVoice     * 100.0f, 0.0f, 100.0f));
		//	dt->SetInt(_T("SoundOtherVL"), (INT)Clamp(ad->VolumeSfx     * 100.0f, 0.0f, 100.0f));
		//}
    }

//  FillInputConfig(dt, FALSE, MMOFacade::getSingleton().GCTType);

    eventUCFillToUI(dt);
}

void RefreshWnd(HWND hw)
{
	if(!hw)
	{
		return;
	}

	//RECT rc;
	//::GetWindowRect(hw, &rc);
	//::GetClientRect(hw, &rc);
	::InvalidateRect(hw, NULL, TRUE);
	//::UpdateWindow(hw);
	::PostMessage(hw, WM_PAINT, 0, 0);
}

BOOL CALLBACK EnumChildWnd(HWND hwnd, LPARAM lpar)
{
	RefreshWnd(hwnd);
	return TRUE;
}

void ClearBloomRenderTarget()
{
	ENQUEUE_UNIQUE_RENDER_COMMAND(
		ClearRenderTarget,
	{
		if ( GSceneRenderTargets.GetFilterColorSurface(SRTI_FilterColor1) && GSceneRenderTargets.GetFilterColorTexture(SRTI_FilterColor1) )
		{
			GSceneRenderTargets.BeginRenderingFilter(SRTI_FilterColor1);
			RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
			GSceneRenderTargets.FinishRenderingFilter(SRTI_FilterColor1);
		}

		if ( GSceneRenderTargets.GetFilterColorSurface(SRTI_FilterColor2) && GSceneRenderTargets.GetFilterColorTexture(SRTI_FilterColor2) )
		{
			GSceneRenderTargets.BeginRenderingFilter(SRTI_FilterColor2);
			RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
			GSceneRenderTargets.FinishRenderingFilter(SRTI_FilterColor2);
		}

		if ( GSceneRenderTargets.GetFilterColorSurface(SRTI_FilterColor0) && GSceneRenderTargets.GetFilterColorTexture(SRTI_FilterColor0) )
		{
			GSceneRenderTargets.BeginRenderingFilter(SRTI_FilterColor0);
			RHIClear(TRUE,FLinearColor(0,0,0,0),FALSE,0,FALSE,0);
			GSceneRenderTargets.FinishRenderingFilter(SRTI_FilterColor0);
		}
	} );	
}

#include <ShlObj.h>


extern void ResetRenderTarget();

void UMMOUIOptionConfig::ApplyByUI(class UGFxObject* dt)
{	
	if (!dt)
        return;

	g_updateMainPlayerHudName = true;
	UBOOL bAllowBloom = GSystemSettings.GetAllowBloom();

    AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();

    if (dt->GetBool(_T("video_enable")))
    {
        INT full = dt->GetInt(_T("WndFullscreen"));
        INT idx = dt->GetInt(_T("WndSizeIdx"));
        FDisplayResolution dr = {1280,720};
		if (idx >= 0 && idx < (INT)g_vecDisplay.size())
		{
			dr = g_vecDisplay[idx];
		}
//        ctrl->ConsoleCommand(FString::Printf(TEXT("setres %dx%d %s"), dr.Width, dr.Height, full ? TEXT("F") : TEXT("W")));

		g_lastSelectedRes.Width = dr.Width;
		g_lastSelectedRes.Height = dr.Height;

#if M_ENABLE_RPMGR
		FReplayDataScreenRes* scrDat = new FReplayDataScreenRes;
		scrDat->ResX = dr.Width;
		scrDat->ResY = dr.Height;
		scrDat->IsFullScreen = (UBOOL)full;
		ReplayManager::getSingleton().Recode(scrDat);
#endif

        FSystemSettings old;
        GSystemSettings.ModifyBegin(old);

		GSystemSettings.SetBool( TEXT("AutoAdjustPerformance"),	dt->GetBool( _T("auto_adjust_perf") ) );
				
		GSystemSettings.SetBool( TEXT("HintBeforeAutoAdjust"),	dt->GetBool( _T("notice_auto_adjust") ) );
		GSystemSettings.SetInt( TEXT("AutoPerformanceCheckTimeNew"),	dt->GetFloat( _T("adjust_interval_check") ) );
		GSystemSettings.SetInt( TEXT("LowerCheckFPS"),	dt->GetFloat( _T("adjust_FPS_check") ) );
		
		GEngine->MaxSmoothedFrameRate =(FLOAT)GSystemSettings.ForeGroundFPS + 2; // oscar@snda
		warnf(NAME_Log, TEXT("==== ForeGroundFPS:%f"), GEngine->MaxSmoothedFrameRate);

		//GSystemSettings.SetBool( TEXT("DrawSilhouettes"),		dt->GetBool(_T("Stroke")) );
		/*INT NewPlayerCountLevel = dt->GetInt( _T("PlayersLevel") );
		if (NewPlayerCountLevel != GSystemSettings.MaxShowPlayerCount)
		{
			GSystemSettings.SetInt( TEXT("MaxShowPlayerCount"), NewPlayerCountLevel);
			MMOFacade::getSingleton().UpdatePlayerShowCount();
		}
		*/

		//GSystemSettings.SetInt( TEXT("MaxParticleCollisionCalcDistance"), dt->GetInt( _T("ParticleLevel") ) );

		INT nMaxPlayerCount = GSystemSettings.MaxShowPlayerCount;
		UGFxObject* pkAdvance = dt->GetObject(TEXT("advancedOptionData"));
		ApplyAdvanceSettingByUI(pkAdvance);

		GSystemSettings.CompatLevelComposite = dt->GetInt( _T("DrawEffectLevel") );
		GConfig->SetInt(TEXT("AppCompat"), TEXT("CompatLevelComposite"), GSystemSettings.CompatLevelComposite, GEngineIni);
		// TODO: GSystemSettings.SetInt( TEXT("CompatLevelComposite"),		dt->GetInt( _T("DrawEffectLevel") ) );

		UBOOL vsync = GSystemSettings.bUseVSync;
		if( GSystemSettings.CompatLevelComposite != 0 )
		{	// not user defined
			FString section = FString::Printf( TEXT("SystemSettingsBucket%d"), GSystemSettings.CompatLevelComposite );
			GConfig->GetBool(*section, TEXT("UseVsync"), vsync, GSystemSettingsIni);
			GConfig->GetInt(*section, TEXT("MaxParticleSpriteCount"), GSystemSettings.MaxParticleSpriteCount, GSystemSettingsIni);
			GConfig->GetInt(*section, TEXT("MaxParticleSubUVCount"), GSystemSettings.MaxParticleSubUVCount, GSystemSettingsIni);
		}
		else
		{
			FString section = FString::Printf( TEXT("SystemSettingsBucket%d"), 2 );
			//GConfig->GetBool(*section, TEXT("UseVsync"), vsync, GSystemSettingsIni);
			GConfig->GetInt(*section, TEXT("MaxParticleSpriteCount"), GSystemSettings.MaxParticleSpriteCount, GSystemSettingsIni);
			GConfig->GetInt(*section, TEXT("MaxParticleSubUVCount"), GSystemSettings.MaxParticleSubUVCount, GSystemSettingsIni);
		}

		ctrl->ConsoleCommand(FString::Printf(TEXT("setresvsync %dx%d %s %s"), dr.Width, dr.Height, full ? TEXT("F") : TEXT("W"), vsync ? TEXT("Y") : TEXT("N")));

		// Physics particles can only stay on at particle effect level 5
		/*
		// not used
		if (GSystemSettings.ParticleLevel >= 5)
		{
			GSystemSettings.bAllowParticleDropDetail = FALSE;
		}
		else
		{			
			GSystemSettings.bAllowParticleDropDetail = TRUE;
		}
		*/
		
        GSystemSettings.ModifyEnd(old);

		bool needResetRT = false;
		if(!GSystemSettings.bAutoAdjustPerformance && (GSystemSettings.AutoFPSLevel != 5))
		{
			needResetRT = true;
			FlushRenderingCommands();
			GSystemSettings.AutoFPSLevel = 5;
			ResetRenderTarget();
		}

		//GConfig->Flush(FALSE, GEngineIni);
		if ( (bAllowBloom != GSystemSettings.GetAllowBloom() && !GSystemSettings.GetAllowBloom())
			&& !needResetRT)
		{
			ClearBloomRenderTarget();
		}

		// see if the adapter is changed..
		INT SelectedAdapterId = 0;
		if( GConfig->GetInt( TEXT("Engine.Engine"), TEXT("D3DAdapterId"), SelectedAdapterId, GEngineIni ) )
		{
			INT NewGPUId = dt->GetInt( _T("GPU_List_Selected_Idx") );
			if( SelectedAdapterId != NewGPUId )
			{
				GConfig->SetInt( TEXT("Engine.Engine"), TEXT("D3DAdapterId"), NewGPUId, GEngineIni );
				//GConfig->Flush( FALSE, GEngineIni );

				FScriptDelegate funcOK(EC_EventParm);
				MMOMsgBoxManager::getSingleton().AddMessageBox(
					MMOUI::ParseLocalizedPropertyPath(
					TEXT("MMOGame.Option.WarnRestart")),0,funcOK,funcOK,FALSE);
			}
		}

		// save engine.ini
		GConfig->Flush(FALSE, GEngineIni);

		SET_WARN_COLOR(COLOR_CYAN);
		warnf(NAME_Log, TEXT("+++++ApplyByUI Select SysRes: %d x %d"), GSystemSettings.ResX, GSystemSettings.ResY);
		CLEAR_WARN_COLOR();

		// oscar@snda [#4499] Exiting fullscreen on XP corrupts the desktop, have to clean up after M$
		//RefreshWnd(::GetDesktopWindow());
		//RefreshWnd(::FindWindowEx(GetDesktopWindow(), NULL, _T("Shell_TrayWnd"), _T("")));
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
		//EnumChildWindows(::FindWindowEx(GetDesktopWindow(), NULL, _T("Shell_TrayWnd"), _T("")), EnumChildWnd, 0);
    }

	UMMOUIManager* uimgr = MMOUI::GetUIManager();
    if (dt->GetBool(_T("sound_enable")))
    {
		uimgr->bSoundAll	= dt->GetBool(_T("SoundAll"));
		uimgr->bSoundBack	= dt->GetBool(_T("SoundBack"));
		//uimgr->bSoundEnv	= dt->GetBool(_T("SoundEnv"));
		uimgr->bSoundVoice	= dt->GetBool(_T("SoundVoice"));
		uimgr->bSoundSfx	= dt->GetBool(_T("SoundSfx"));
		uimgr->bSoundFocusOut= dt->GetBool(_T("SoundFocusOut"));

		uimgr->nSoundAllVL		=dt->GetInt(_T("SoundAllVL"));
		uimgr->nSoundBackVL		=dt->GetInt(_T("SoundBackVL")); 
		//uimgr->nSoundEnvVL		=dt->GetInt(_T("SoundEnvVL"));  
		uimgr->nSoundVoiceVL	=dt->GetInt(_T("SoundVoiceVL"));
		uimgr->nSoundSfxVL		=dt->GetInt(_T("SoundSfxVL"));


        MMOFacade::getSingleton().SetCurVolumeAndSaveAudioConfig();
    }

	//UMMOUIManager* uimgr = MMOUI::GetUIManager();
	uimgr->SaveConfigToServer();

	//uimgr->SaveSoundConfigToServer();
	MMOFacade::getSingleton().NotifyOptionChange();
	if ( AInstancedShadowPlaneActor::GetInstancedShadowPlaneActor( true ) )
		AInstancedShadowPlaneActor::NotifyOptionChange();

}

void UMMOUIOptionConfig::ASTempApplyUI(UGFxObject* dt)
{
	if (dt->GetBool(_T("sound_enable")))
	{
		UAudioDevice* adioDv = GEngine->GetAudioDevice();
		if( adioDv == NULL ){
			return;
		}

		adioDv->VolumeTotal_op		= dt->GetBool(_T("SoundAll"));
		adioDv->VolumeBackMusic_op	= dt->GetBool(_T("SoundBack"));
		//adioDv->VolumeEnv_op		= uimgr->bSoundEnv;	
		adioDv->VolumeVoice_op		= dt->GetBool(_T("SoundVoice"));
		adioDv->VolumeSfx_op		= dt->GetBool(_T("SoundSfx"));
		GIsVolumeInBackground		= dt->GetBool(_T("SoundFocusOut"));
		adioDv->VolumeTotal			= (float)dt->GetInt(_T("SoundAllVL")) / 100.0f;
		adioDv->VolumeBackMusic		= (float)dt->GetInt(_T("SoundBackVL")) / 100.0f;
		//adioDv->VolumeEnv			= (float)dt->GetInt(_T("SoundEnvVL"))/ 100.0f;
		adioDv->VolumeVoice			= (float)dt->GetInt(_T("SoundVoiceVL"))	/ 100.0f;
		adioDv->VolumeSfx			= (float)dt->GetInt(_T("SoundSfxVL")) / 100.0f;
	}
}


void UMMOUIOptionConfig::ASConfigVideoReq(INT lv)
{
    if (lv >= 1 && lv <= 5)
    {
		UGFxObject* datList = GFXPlayer->CreateArray();
		for (INT i = 0,j = 0; i < g_displayDetailOptions.Num(); ++i)
		{
			const FDisplayDetailOption& opt = g_displayDetailOptions(i);
			if(opt.SystemSetting == NULL)
				continue;
			if(opt.bInAdvanceList)
			{
				UGFxObject* dat = GFXPlayer->CreateObject(TEXT("ActionScript3.Option.AdvancedVideo.OptionItemData"));
				if(dat)
				{
					dat->SetString(TEXT("optionKey"),opt.SettingName);
					dat->SetInt(TEXT("type"),opt.SysSettingType);

					FString strDisplay =  FString::Printf(TEXT("MMOGame.Option.SettingsName.%s"),*(opt.SettingName));
					strDisplay = MMOUI::ParseLocalizedPropertyPath(strDisplay);
					dat->SetString(TEXT("title"),strDisplay);

					InitSystemSettingToUI(dat,opt.SystemSetting,lv-1);

					datList->SetElementObject(j,dat);
					j++;

				}
			}
		}
		eventCallUCSetOptionListData(datList);
	}
}
void UMMOUIOptionConfig::ApplyEffectLevelVideoData()
{
	INT nMaxPlayerCount = GSystemSettings.MaxShowPlayerCount;

	/*
	Because some layered data is on the UI side and not written into systemsetting.h, after automatically adjusting the effect level, the data for that level must be adjusted
	*/
	/*const FCompatibilityLevelInfo MachineCompatLevel = appGetCompatibilityLevelV2();*/
	INT nLevelVodeo = GSystemSettings.CompatLevelComposite;

	if( nLevelVodeo > 0 && nLevelVodeo <= 5)
	{
		for (INT i = 0,j = 0; i < g_displayDetailOptions.Num(); ++i)
		{
			const FDisplayDetailOption& opt = g_displayDetailOptions(i);
			if(opt.SystemSetting == NULL)
				continue;

			if(!(opt.bInAdvanceList))
				continue;

			switch(opt.SystemSetting->SettingType)
			{
			case SST_INT:
				{
					INT nValue = opt.levelData->GetIntData(nLevelVodeo);
					INT nCurValue = 0;
					GSystemSettings.GetInt(opt.SystemSetting->SettingName,nCurValue);
					if(nValue != nCurValue)
						GSystemSettings.SetInt(opt.SystemSetting->SettingName,nValue);
					break;
				}
			case SST_BOOL:
				{
					BOOL bValue = opt.levelData->GetBoolData(nLevelVodeo);
					UBOOL bCurValue = FALSE;
					GSystemSettings.GetBool(opt.SystemSetting->SettingName,bCurValue);
					if (bCurValue != bValue)
						GSystemSettings.SetBool(opt.SystemSetting->SettingName,bValue);
					break;
				}
			case SST_FLOAT:
				{
					FLOAT fValue = opt.levelData->GetFloatData(nLevelVodeo);
					FLOAT fCurValue = 0.0f;
					GSystemSettings.GetFloat(opt.SystemSetting->SettingName,fCurValue);
					if(abs(fValue - fCurValue) > 0.00001f)
						GSystemSettings.SetFloat(opt.SystemSetting->SettingName,fValue);
					break;
				}
			}
		}
	}
	GEngine->MaxSmoothedFrameRate =(FLOAT)GSystemSettings.ForeGroundFPS + 2; // oscar@snda
}
//void UMMOUIOptionConfig::FillInputConfig(class UGFxObject* dt, UBOOL mdf_config,INT nModfGCTType)
//{
//	UMMOKeyManager* keymgr = UMMOKeyManager::getSingleton();
//	FString cstr;
//
//	dt->SetBool(_T("input1_enable"), TRUE);
//	{
//		UGFxObject* kst = GFXPlayer->CreateArray();
//		for(INT i=0;i<12;i++)
//		{
//			UGFxObject *KeyInfoObj = GFXPlayer->CreateObject("Object");
//			if(KeyInfoObj){
//				KeyInfoObj->SetString(TEXT("KeyName"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::EKeyFunction(UMMOKeyManager::KF_BarSkill1+i), mdf_config), cstr, 1));
//				KeyInfoObj->SetString(TEXT("ComKeyName"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::EKeyFunction(UMMOKeyManager::KF_BarSkill1+i), mdf_config), cstr, 2));
//				KeyInfoObj->SetInt(TEXT("KeyEnum"), UMMOKeyManager::KF_BarSkill1+i);
//			}
//			kst->SetElementObject( i, KeyInfoObj);
//		}
//		dt->SetObject(_T("SlotKeyList"), kst);
//		//CQ_SKILL_HJJ;Begin
//		//if(nModfGCTType == GC_MIRFOREVER){
//		//	dt->SetString(_T("BarSkill1"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx1, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill2"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx2, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill3"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx3, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill4"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx4, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill5"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx5, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill6"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx6, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkillA1"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx1, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA2"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx2, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA3"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx3, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA4"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx4, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA5"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx5, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA6"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkillEx6, mdf_config), cstr, 2));
//		//	dt->SetInt(_T("BarSkillT1"), UMMOKeyManager::KF_BarSkillEx1);
//		//	dt->SetInt(_T("BarSkillT2"), UMMOKeyManager::KF_BarSkillEx2);
//		//	dt->SetInt(_T("BarSkillT3"), UMMOKeyManager::KF_BarSkillEx3);
//		//	dt->SetInt(_T("BarSkillT4"), UMMOKeyManager::KF_BarSkillEx4);
//		//	dt->SetInt(_T("BarSkillT5"), UMMOKeyManager::KF_BarSkillEx5);
//		//	dt->SetInt(_T("BarSkillT6"), UMMOKeyManager::KF_BarSkillEx6);
//
//		//	dt->SetString(_T("BarItem1"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx1, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem2"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx2, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem3"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx3, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem4"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx4, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem5"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx5, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem6"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx6, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItemA1"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx1, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA2"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx2, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA3"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx3, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA4"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx4, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA5"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx5, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA6"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItemEx6, mdf_config), cstr, 2));
//		//	dt->SetInt(_T("BarItemT1"), UMMOKeyManager::KF_BarItemEx1);
//		//	dt->SetInt(_T("BarItemT2"), UMMOKeyManager::KF_BarItemEx2);
//		//	dt->SetInt(_T("BarItemT3"), UMMOKeyManager::KF_BarItemEx3);
//		//	dt->SetInt(_T("BarItemT4"), UMMOKeyManager::KF_BarItemEx4);
//		//	dt->SetInt(_T("BarItemT5"), UMMOKeyManager::KF_BarItemEx5);
//		//	dt->SetInt(_T("BarItemT6"), UMMOKeyManager::KF_BarItemEx6);
//
//		//}
//		//else{
//		//	dt->SetString(_T("BarSkill1"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill1, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill2"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill2, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill3"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill3, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill4"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill4, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill5"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill5, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkill6"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill6, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarSkillA1"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill1, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA2"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill2, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA3"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill3, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA4"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill4, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA5"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill5, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarSkillA6"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarSkill6, mdf_config), cstr, 2));
//		//	dt->SetInt(_T("BarSkillT1"), UMMOKeyManager::KF_BarSkill1);
//		//	dt->SetInt(_T("BarSkillT2"), UMMOKeyManager::KF_BarSkill2);
//		//	dt->SetInt(_T("BarSkillT3"), UMMOKeyManager::KF_BarSkill3);
//		//	dt->SetInt(_T("BarSkillT4"), UMMOKeyManager::KF_BarSkill4);
//		//	dt->SetInt(_T("BarSkillT5"), UMMOKeyManager::KF_BarSkill5);
//		//	dt->SetInt(_T("BarSkillT6"), UMMOKeyManager::KF_BarSkill6);
//
//		//	dt->SetString(_T("BarItem1"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem1, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem2"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem2, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem3"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem3, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem4"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem4, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem5"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem5, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItem6"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem6, mdf_config), cstr, 1));
//		//	dt->SetString(_T("BarItemA1"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem1, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA2"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem2, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA3"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem3, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA4"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem4, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA5"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem5, mdf_config), cstr, 2));
//		//	dt->SetString(_T("BarItemA6"), keymgr->KeyToString(keymgr->FindKeyworld(UMMOKeyManager::KF_BarItem6, mdf_config), cstr, 2));
//		//	dt->SetInt(_T("BarItemT1"), UMMOKeyManager::KF_BarItem1);
//		//	dt->SetInt(_T("BarItemT2"), UMMOKeyManager::KF_BarItem2);
//		//	dt->SetInt(_T("BarItemT3"), UMMOKeyManager::KF_BarItem3);
//		//	dt->SetInt(_T("BarItemT4"), UMMOKeyManager::KF_BarItem4);
//		//	dt->SetInt(_T("BarItemT5"), UMMOKeyManager::KF_BarItem5);
//		//	dt->SetInt(_T("BarItemT6"), UMMOKeyManager::KF_BarItem6);
//		//}
//		//CQ_SKILL_HJJ;Begin
//	}
//
//	dt->SetBool(_T("input2_enable"), TRUE);
//	//if (0)
//	{
//		UGFxObject* klst = GFXPlayer->CreateArray();
//		//check(klst);
//		if(klst == NULL)return;
//		BYTE kIndex = UMMOKeyManager::KF_Func_Begin;
//		for (BYTE k = UMMOKeyManager::KF_Func_Begin; k < UMMOKeyManager::KF_Func_End; ++k)
//		{
//			UMMOKeyManager::KKeyworkd* pk = keymgr->FindKeyworld((UMMOKeyManager::EKeyFunction)k, mdf_config);
//			if(keymgr->CanDoFunction((UMMOKeyManager::EKeyFunction)k))
//			{
//				UGFxObject* ko = GFXPlayer->CreateObject(TEXT("ActionScript3.Option.InputKeyItemData"));
//				//check(ko);
//				if(ko == NULL)return;
//				ko->SetInt(_T("ktype"), k);
//				if (pk)
//					ko->SetString(_T("kkey"), keymgr->KeyToString(pk, cstr));
//				else
//					ko->SetString(_T("kkey"), _T(""));
//				ko->SetString(_T("kdesc"), keymgr->FuncToString((UMMOKeyManager::EKeyFunction)k, cstr));
//				klst->SetElementObject(kIndex - UMMOKeyManager::KF_Func_Begin, ko);
//				++kIndex;
//			}
//		}
//		dt->SetObject(_T("KeyList"), klst);
//	}
//}

//void UMMOUIOptionConfig::FillInputToUI(UBOOL mdf_config, INT nModfGCTType)
//{
//	UGFxObject* dt = GFXPlayer->CreateObject(TEXT("ActionScript3.Option.OptionData"));
//	//check(dt);
//	if(dt == NULL)return;
//	dt->SetString(_T("wnd_title"), MMOUI::ParseLocalizedPropertyPath(_T("MMOGame.Option.Menu.Button.Name.SystemConfig")));
//
//	FillInputConfig(dt, mdf_config, nModfGCTType);
//
//	eventUCFillToUI(dt);
//}

void UMMOUIOptionConfig::PostOnShow(UBOOL show)
{
    UGFxObject* page_video = GetObject("page_video");
    if (page_video)
    {
        TArray<struct FASValue> Args;
        page_video->GetObject("window_mode")->Invoke("close", Args);
        page_video->GetObject("window_size")->Invoke("close", Args);
    }

	Super::PostOnShow(show);

	if(show)
	{
		UGFxObject* page_game_ = GetObject("tab_game");
		UGFxObject* page_input = GetObject("tab_input");
		if (page_game_)
			page_game_->SetVisible(MMO::GetCurrentGameState() == EGameState_InGame);
		if (page_input)
			page_input->SetVisible(MMO::GetCurrentGameState() == EGameState_InGame);

		UMMOKeyManager::getSingleton()->OnOpenUI();
		eventUCSetCtrlType(MMOFacade::getSingleton().GCTType);
		FillToUI();

		//eventUCShowWarning( TEXT("")  );

		MMOGameEvent_Any evt(GameEvent_ShowOptionPanel);
		MMOUI::PostGameEvent(&evt);
		//GFXPlayer->CaptureAllKeys();
	}
	else
	{
		//GFXPlayer->ClearCaptureKeys();

		MMOFacade::getSingleton().SetCurVolumeAndSaveAudioConfig();

		UMMOKeyManager::getSingleton()->MdfKeyEnd();

		MMOGameEvent_Any evt(GameEvent_HideOptionPanel);
		MMOUI::PostGameEvent(&evt);
	}
}

//void UMMOUIOptionConfig::ShowKeyWarning(UBOOL bShow)
//{
//	if(bShow){
//		eventUCShowWarning( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Options.ShowWarning"))  );
//	}
//	else
//	{
//		eventUCShowWarning( TEXT("") );
//	}
//}


void UMMOUIOptionConfig::Init()
{
	Super::Init();

//	MMOUI::GetUIManager()->SetShowKeyWarningDelegate(FScriptDelegate(this, TEXT("ShowKeyWarning")));
}

void UMMOUIOptionConfig::UnInit()
{
	Super::UnInit();

//	FScriptDelegate nullDelgate(EC_EventParm);
//	MMOUI::GetUIManager()->SetShowKeyWarningDelegate(nullDelgate);
}