#include "stdafx.h"
#include "MMOGuideData.h"
#include "MMOFacade.h"
#include "Inc\MMOGameUIClasses.h"
#include "MMOQuestManager.h"

using namespace ExternalMessage;

//////////////////////////////////////////////////////////////////////////
GuideData* GuideDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	GuideData* obj = GuideDataManager::getSingleton().FindAndModifyData(templateID);
	if (obj)
	{
		needAddToMap = false;
	}
	else
	{
		obj = new GuideData();
		if (obj)
		{
			obj->Id = templateID;
		}
		needAddToMap = true;
	}

	return obj;
}

void GuideDataProcessor::DestroyDataObject(GuideData* obj)
{
	if (obj)
	{
		delete obj;
		obj = NULL;
	}
}

void GuideDataProcessor::ParseData(GuideData* obj, class CTabFile* dataRow, int rowIdx)
{
	GuideData data;

	data.Id = TReadDataValue_Int<GuideDataManager>(TEXT("GuideId"), dataRow, rowIdx);
	if (data.Id == 0)
	{
		return;
	}

	data.Enable = TReadDataValue_Int<GuideDataManager>(TEXT("Enable"), dataRow, rowIdx);
	if (!data.Enable)
	{
		return;
	}

	data.Type = TReadDataValue_Int<GuideDataManager>(TEXT("GuideType"), dataRow, rowIdx);
	data.Desc = TReadDataValue_FString<GuideDataManager>(TEXT("GuideDesc"), dataRow, rowIdx);
	data.ActivateType = (EGuideEventType)TReadDataValue_Int<GuideDataManager>(TEXT("ActivateGuideType"), dataRow, rowIdx);
	data.DeactiveType = (EGuideEventType)TReadDataValue_Int<GuideDataManager>(TEXT("DeactiveGuideType"), dataRow, rowIdx);

	data.LevelReq = TReadDataValue_Int<GuideDataManager>(TEXT("LevelReq"), dataRow, rowIdx);
	data.GuideReq = TReadDataValue_Int<GuideDataManager>(TEXT("GuideReq"), dataRow, rowIdx);
	data.QuestReq = TReadDataValue_Int<GuideDataManager>(TEXT("QuestReq"), dataRow, rowIdx);

	for (INT i = 0; i < MAX_PLAYER_CLASS_COUNT; ++ i)
	{
		FString rowName = FString::Printf(TEXT("ItemReqNew.ItemId%d"), i + 1);
		data.ItemReq[i] = TReadDataValue_Int<GuideDataManager>(*rowName, dataRow, rowIdx);

		rowName = FString::Printf(TEXT("SkillReq.SkillId%d"), i + 1);
		data.SkillReq[i] = TReadDataValue_Int<GuideDataManager>(*rowName, dataRow, rowIdx);
	}

	data.ItemQuaReq = TReadDataValue_Int<GuideDataManager>(TEXT("ItemQuaReq"), dataRow, rowIdx);
	data.NpcReq = TReadDataValue_Int<GuideDataManager>(TEXT("NpcReq"), dataRow, rowIdx);
	data.DoodadTypeReq = (EDOODAD_TYPE)TReadDataValue_Int<GuideDataManager>(TEXT("DoodadTypeReq"), dataRow, rowIdx);
	data.ClassReq = (EPlayerClass)TReadDataValue_Int<GuideDataManager>(TEXT("ClassReq"), dataRow, rowIdx);

	for (INT i = 0; i < MAX_PLAYER_CLASS_COUNT; ++ i)
	{
		FString rowName = FString::Printf(TEXT("Picture.Picture%d"), i + 1);
		FString picName = TReadDataValue_FString<GuideDataManager>(*rowName, dataRow, rowIdx);
		if (!picName.IsEmpty())
		{
			INT subIndex = picName.InStr(TEXT("Texture2D ")) + FString(TEXT("Texture2D ")).Len();
			data.Picture[i] = picName.Mid(subIndex);
		}
	}			

	data.Sound = TReadDataValue_FString<GuideDataManager>(TEXT("GuideSound"), dataRow, rowIdx);
	if (data.Sound == TEXT("0"))
	{
		data.Sound = TEXT("");
	}
	// used for the release build resource list
	if (obj)
	{
		obj->Sound = data.Sound;
	}

	FString iconName = TReadDataValue_FString<GuideDataManager>(TEXT("GuideSoundIcon"), dataRow, rowIdx);
	if (!iconName.IsEmpty())
	{
		INT subIndex = iconName.InStr(TEXT("Texture2D ")) + FString(TEXT("Texture2D ")).Len();
		data.SoundIcon = iconName.Mid(subIndex);
	}

	data.TargetWindowName = TReadDataValue_FString<GuideDataManager>(TEXT("TargetWindowName"), dataRow, rowIdx);
	for (INT i = 0; i < MAX_TARGET_COMPONENT_COUNT; ++ i)
	{
		FString rowName = FString::Printf(TEXT("TargetComponent.TargetComponentName%d"), i + 1);
		data.TargetComponentName[i] = TReadDataValue_FString<GuideDataManager>(*rowName, dataRow, rowIdx);
	}

	INT x = TReadDataValue_Int<GuideDataManager>(TEXT("TargetComponentOffset.X"), dataRow, rowIdx);
	INT y = TReadDataValue_Int<GuideDataManager>(TEXT("TargetComponentOffset.Y"), dataRow, rowIdx);
	data.Offset = FVector2D(x, y);

	data.IsBoxed = TReadDataValue_Int<GuideDataManager>(TEXT("WithBox"), dataRow, rowIdx);

	x = TReadDataValue_Int<GuideDataManager>(TEXT("BoxOffset.X"), dataRow, rowIdx);
	y = TReadDataValue_Int<GuideDataManager>(TEXT("BoxOffset.Y"), dataRow, rowIdx);
	data.BoxOffset = FVector2D(x, y);

	x = TReadDataValue_Int<GuideDataManager>(TEXT("BoxSize.Width"), dataRow, rowIdx);
	y = TReadDataValue_Int<GuideDataManager>(TEXT("BoxSize.Height"), dataRow, rowIdx);
	data.BoxSize = FVector2D(x, y);

	data.BoxType = (EGuideBoxType)TReadDataValue_Int<GuideDataManager>(TEXT("BoxType"), dataRow, rowIdx);
	data.ArrowTipsType = (EGuideArrowTipsType)TReadDataValue_Int<GuideDataManager>(TEXT("ArrowTipsType"), dataRow, rowIdx);
	data.TraceBagSlot = TReadDataValue_Int<GuideDataManager>(TEXT("TraceBagSlot"), dataRow, rowIdx);
	data.TraceMenuBar = TReadDataValue_Int<GuideDataManager>(TEXT("TraceMenuBar"), dataRow, rowIdx);
	data.TraceMenuBarType = (ETraceMenuBarType)TReadDataValue_Int<GuideDataManager>(TEXT("TraceMenuBarType"), dataRow, rowIdx);
	data.TraceMenuType = TReadDataValue_Int<GuideDataManager>(TEXT("TraceMenuType"), dataRow, rowIdx);
	data.TraceMenuChildType = TReadDataValue_Int<GuideDataManager>(TEXT("TraceMenuChildType"), dataRow, rowIdx);
	data.TraceJourney = TReadDataValue_Int<GuideDataManager>(TEXT("TraceJourney"), dataRow, rowIdx);
	data.TraceJourneyType = TReadDataValue_Int<GuideDataManager>(TEXT("TraceJourneyType"), dataRow, rowIdx);

	MMOGuideData::getSingleton().AddGuideData(data);
}

void GuideDataProcessor::PostInitData()
{
}

#if WITH_EDITOR

void GuideDataProcessor::CollectReferencedResourceFromDataTable(GuideData* obj, TSet<FString>& outResList)
{
	if (obj)
	{
		//UObject* data = UObject::StaticLoadObject(USoundCue::StaticClass(), NULL, *(obj->Sound), NULL, LOAD_NoWarn | LOAD_Quiet, NULL);
		USoundCue* data = LoadUEObject<USoundCue>(obj->Sound, TEXT("guide.txt"));
		if (data)
		{
			_CollectReferencedResource(data, outResList);
		}
	}
}

#endif

//////////////////////////////////////////////////////////////////////////
MMOGuideData::MMOGuideData()
{
}

MMOGuideData::~MMOGuideData()
{
}

void MMOGuideData::Init()
{
	Clear();
	LoadGuideData(TEXT("guide.txt"));
}

void MMOGuideData::Shutdown()
{
	UnloadGuideData();
	Clear();
}

void MMOGuideData::Clear()
{
	_GuideStateMap.clear();
}

UBOOL MMOGuideData::LoadGuideData(const TCHAR* filePath)
{
	M_BeginLoadData(TEXT("guide.txt"));
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOGuideData::Init"), 0);

	_GuideDataMap.clear();
	_ActivateGuideEventMap.clear();
	_DeactiveGuideEventMap.clear();
	GuideDataManager::getSingleton().InitFromFile(filePath, true);
	
	return TRUE;
}

// use this when loading release build resources
UBOOL MMOGuideData::LoadGuideDataFromUpk()
{
	TArray<FString> DirArray;
	GConfig->GetArray(TEXT("GuideTemplates"), TEXT("TemplateDir"), DirArray, GGameIni);
	MMO::FormatTempleteDataLoadingDir(DirArray);

	_GuideDataMap.clear();
	_ActivateGuideEventMap.clear();
	_DeactiveGuideEventMap.clear();

	for (INT i = 0; i < DirArray.Num(); ++ i)
	{
		const FString& cfgDir = DirArray(i);
		debugIDf(TEXT("TemplateData"), TEXT("===== Loading guide template data form %s"), *cfgDir);

		TArray<FString> packageDirs;
		appFindFilesInDirectory(packageDirs, *cfgDir, TRUE, FALSE);
		for (INT j = 0; j < packageDirs.Num(); ++ j)
		{
			UPackage* package = UObject::LoadPackage( NULL, *packageDirs(j), LOAD_Verify );
			if (!package)
			{
				M_AddDataError(TEXT("guide"), FString::Printf(TEXT("Cannot load quest data package: %s"), *packageDirs(j)));
				continue;
			}

			if (!package->IsFullyLoaded())
			{
				package->FullyLoad();
			}

			for (INT k = 0; k < package->GetNetObjectCount(0); ++ k)
			{
				UGuide* guide = Cast<UGuide>(package->GetNetObjectAtIndex(k));
				if (!guide)
				{
					continue;
				}
				if (guide->GetTemplateID() == 0)
				{
					continue;
				}
				
				if (!guide->Enable)
				{
					continue;
				}

				guide->AddToRoot();

				GuideDataMap::const_iterator iter = _GuideDataMap.find((UINT32)guide->GetTemplateID());
				if (iter != _GuideDataMap.end())
				{
					continue;
				}

				GuideData data;

				data.Id = guide->GetTemplateID();
				data.Type = guide->GuideType;
				data.Desc = guide->GuideDesc;
				data.Enable = guide->Enable;
				data.ActivateType = (EGuideEventType)guide->ActivateGuideType;
				data.DeactiveType = (EGuideEventType)guide->DeactiveGuideType;

				data.LevelReq = guide->LevelReq;
				data.GuideReq = guide->GuideReq;
				data.QuestReq = guide->QuestReq;

				for (INT i = 0; i < guide->ItemReqNew.Num(); ++ i)
				{
					if (i < MAX_PLAYER_CLASS_COUNT)
					{
						data.ItemReq[i] = guide->ItemReqNew(i).ItemId;
					}
				}

				for (INT i = 0; i < guide->SkillReq.Num(); ++ i)
				{
					if (i < MAX_PLAYER_CLASS_COUNT)
					{
						data.SkillReq[i] = guide->SkillReq(i).SkillId;
					}
				}

				data.ItemQuaReq = guide->ItemQuaReq;
				data.NpcReq = guide->NpcReq;
				data.DoodadTypeReq = (EDOODAD_TYPE)(guide->DoodadTypeReq);
				data.ClassReq = (EPlayerClass)(guide->ClassReq);

				for (INT i = 0; i < guide->Picture.Num(); ++ i)
				{
					if (i < MAX_PLAYER_CLASS_COUNT)
					{
						if (guide->Picture(i).Picture)
						{
							data.Picture[i] = guide->Picture(i).Picture->GetPathName();
						}
					}
				}

				if (guide->GuideSound)
				{
					data.Sound = guide->GuideSound->GetPathName();
				}

				if (guide->GuideSoundIcon)
				{
					data.SoundIcon = guide->GuideSoundIcon->GetPathName();
				}

				data.TargetWindowName = guide->TargetWindowName;
				for (INT i = 0; i < MAX_TARGET_COMPONENT_COUNT; ++ i)
				{
					data.TargetComponentName[i] = guide->TargetComponent(i).TargetComponentName;
				}

				INT x = guide->TargetComponentOffset.X;
				INT y = guide->TargetComponentOffset.Y;
				data.Offset = FVector2D(x, y);

				data.IsBoxed = guide->WithBox;

				x = guide->BoxOffset.X;
				y = guide->BoxOffset.Y;
				data.BoxOffset = FVector2D(x, y);

				x = guide->BoxSize.Width;
				y = guide->BoxSize.Height;
				data.BoxSize = FVector2D(x, y);
				data.BoxType = (EGuideBoxType)(guide->BoxType);
				data.ArrowTipsType = (EGuideArrowTipsType)(guide->ArrowTipsType);

				data.TraceBagSlot = guide->TraceBagSlot;
				data.TraceMenuBar = guide->TraceMenuBar;
				data.TraceMenuBarType = (ETraceMenuBarType)guide->TraceMenuBarType;
				data.TraceMenuType = guide->TraceMenuType;
				data.TraceMenuChildType = (EMenuType)guide->TraceMenuChildType;

				data.TraceJourney = guide->TraceJourney;
				data.TraceJourneyType = guide->TraceJourneyType;

				AddGuideData(data);
			}
		}
	}

	return TRUE;
}

void MMOGuideData::UnloadGuideData()
{
	_GuideDataMap.clear();
	_ActivateGuideEventMap.clear();
	_DeactiveGuideEventMap.clear();
}

void MMOGuideData::AddGuideData(const GuideData& data)
{
	_GuideDataMap.insert(std::make_pair(data.Id, data));

	// activation event list
	GuideEventMap::iterator iterActivate = _ActivateGuideEventMap.find(data.ActivateType);
	if (iterActivate != _ActivateGuideEventMap.end())
	{
		GuideList& list = iterActivate->second;
		list.push_back(data.Id);
	}
	else
	{
		GuideList list;
		list.clear();
		list.push_back(data.Id);
		_ActivateGuideEventMap.insert(std::make_pair(data.ActivateType, list));
	}

	// termination event list
	GuideEventMap::iterator iterDeactive = _DeactiveGuideEventMap.find(data.DeactiveType);
	if (iterDeactive != _DeactiveGuideEventMap.end())
	{
		GuideList& list = iterDeactive->second;
		list.push_back(data.Id);
	}
	else
	{
		GuideList list;
		list.clear();
		list.push_back(data.Id);
		_DeactiveGuideEventMap.insert(std::make_pair(data.DeactiveType, list));
	}
}

UBOOL MMOGuideData::GetGuideData(UINT32 id, GuideData& data)
{
	GuideDataMap::iterator iter = _GuideDataMap.find(id);
	if (iter != _GuideDataMap.end())
	{
		data = iter->second;
		return TRUE;
	}
	return FALSE;
}

GuideDataMap& MMOGuideData::GetGuideDataMap()
{
	return _GuideDataMap;
}

void MMOGuideData::ClearGuideState()
{
	_GuideStateMap.clear();
	GuideDataMap::iterator iterData = _GuideDataMap.begin();
	for (; iterData != _GuideDataMap.end(); ++ iterData)
	{
		_GuideStateMap.insert(std::make_pair(iterData->first, GS_Unactivated));
	}
}

UBOOL MMOGuideData::IsGuideEnable(UINT32 id)
{
	GuideData data;
	if (GetGuideData(id, data))
	{
		if (data.Enable)
		{
			return TRUE;
		}
	}

	return FALSE;
}

GuideState MMOGuideData::GetGuideState(UINT32 id)
{
	if (id == 0)
	{
		return GS_Complete;
	}

	GuideStateMap::iterator iter = _GuideStateMap.find(id);
	if (iter != _GuideStateMap.end())
	{
		return iter->second;
	}

	return GS_Unactivated;
}

void MMOGuideData::UpdateGuideState(UINT32 id, GuideState state)
{
	GuideStateMap::iterator iter = _GuideStateMap.find(id);
	if (iter != _GuideStateMap.end())
	{
		iter->second = state;
	}
}

GuideStateMap& MMOGuideData::GetGuideStateMap()
{
	return _GuideStateMap;
}

GuideEventMap& MMOGuideData::GetActivateGuideEventMap()
{
	return _ActivateGuideEventMap;
}

GuideEventMap& MMOGuideData::GetDeactiveGuideEventMap()
{
	return _DeactiveGuideEventMap;
}