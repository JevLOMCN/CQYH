#ifndef _MMOGUIDEDATA_H_
#define _MMOGUIDEDATA_H_

#include "MMOSingleton.h"
#include <map>
#include <vector>
#include <queue>
#include "UnString.h"
#include "guild_def.h"
#include "MMOFacade.h"
#include "MMODataManager.h"
#include "MMOJourneyData.h"

enum
{
	MAX_GUIDE_COUNT = 128,
	MAX_TARGET_COMPONENT_COUNT = 5,
	MAX_PLAYER_CLASS_COUNT = 5
};

// guide data
struct GuideData
{
	UINT32 Id;												// guide id
	UBOOL Enable;											// enabled
	UINT32 Type;											// display type
	FString Desc;											// guide description
	EGuideEventType ActivateType;							// activation event
	EGuideEventType DeactiveType;							// termination event

	UINT32 LevelReq;										// level requirement
	UINT32 GuideReq;										// guide requirement
	UINT32 QuestReq;										// quest requirement
	UINT32 ItemReq[5];										// item requirement
	UINT32 SkillReq[5];										// skill requirement
	UINT32 ItemQuaReq;										// item quality requirement
	UINT32 NpcReq;											// NPC to talk to
	EDOODAD_TYPE DoodadTypeReq;								// Doodad type
	EPlayerClass ClassReq;									// class requirement

	FString Picture[MAX_PLAYER_CLASS_COUNT];				// picture caption

	FString Sound;											// voice name
	FString SoundIcon;										// voice icon

	FString TargetWindowName;								// associated window name
	FString TargetComponentName[MAX_TARGET_COMPONENT_COUNT];// associated component name
	FVector2D Offset;										// offset relative to the associated component
	UBOOL IsBoxed;											// whether it has a border effect
	FVector2D BoxOffset;									// offset of the border effect
	FVector2D BoxSize;										// size of the border effect
	EGuideBoxType BoxType;									// type of the border effect
	EGuideArrowTipsType ArrowTipsType;						// arrow tip text type
	UBOOL TraceBagSlot;										// whether to track bag slot
	UBOOL TraceMenuBar;										// whether to track MenuBar
	ETraceMenuBarType TraceMenuBarType;						// MenuBar tracking type
	UINT32 TraceMenuType;									// track MenuType
	UINT32 TraceMenuChildType;								// track MenuChildType
	UBOOL TraceJourney;
	UINT32 TraceJourneyType;

	GuideData()
	{
		Id = 0;
		Type = EGDT_Null;
		Desc = TEXT("");
		Enable = FALSE;
		ActivateType = EGUET_Null;
		DeactiveType = EGUET_Null;

		LevelReq = 0;	
		GuideReq = 0;		
		QuestReq = 0;
		for (INT i = 0; i < MAX_PLAYER_CLASS_COUNT; ++ i)
		{
			ItemReq[i] = 0;
			SkillReq[i] = 0;
		}
		ItemQuaReq = 0;					
		NpcReq = 0;			
		DoodadTypeReq = E_dt_Invalid;
		ClassReq = EPCT_None;

		for (INT i = 0; i < MAX_PLAYER_CLASS_COUNT; ++ i)
		{
			Picture[i] = TEXT("");
		}						
		Sound = TEXT("");
		SoundIcon = TEXT("");

		TargetWindowName = TEXT("0");
		for (INT i = 0; i < MAX_TARGET_COMPONENT_COUNT; ++ i)
		{
			TargetComponentName[i] = TEXT("");
		}

		Offset = FVector2D(0, 0);
		IsBoxed = FALSE;								
		BoxOffset = FVector2D(0, 0);							
		BoxSize = FVector2D(0, 0);
		BoxType = EGBT_Null;
		ArrowTipsType = EGATT_Null;

		TraceBagSlot = FALSE;
		TraceMenuBar = FALSE;
		TraceMenuBarType = ETMBT_Right;
		TraceMenuType = EMenuType_Bag;
		TraceMenuChildType = EMenuType_Child_Bag;

		TraceJourney = FALSE;
		TraceJourneyType = JT_TOTAL;
	}
};

struct GuideDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("guide.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static GuideData* CreateDataObject(INT templateID, bool& needAddToMap);
	static void DestroyDataObject(GuideData* obj);
	static void ParseData(GuideData* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("GuideId");
	}

	static FString FormatAsString(GuideData* obj)
	{
		return "";
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(GuideData* obj, TSet<FString>& outResList);
#endif
};

typedef TDataManger<GuideData, GuideDataProcessor> GuideDataManager;


enum GuideState
{
	GS_Unactivated = 0,
	GS_Activated,
	GS_Complete
};

typedef std::map<UINT32, GuideData, M_UE3_STL_ALLOC_MAP(UINT32, GuideData)> GuideDataMap;
typedef std::map<UINT32, GuideState, M_UE3_STL_ALLOC_MAP(UINT32, GuideState)> GuideStateMap;
typedef std::list<UINT32, M_UE3_STL_ALLOC(UINT32)> GuideList;
typedef std::map<UINT32, GuideList, M_UE3_STL_ALLOC_MAP(UINT32, GuideList)> GuideEventMap;

class MMOGuideData : public SingletonMeyers<MMOGuideData>
{
public:
	MMOGuideData();
	virtual ~MMOGuideData();

	void Init();
	void Shutdown();
	void Clear();
	void CollectReferencedResourceFromUpk(TSet<FString>& outResList);

	UBOOL LoadGuideData(const TCHAR* filePath);
	UBOOL LoadGuideDataFromUpk();
	void UnloadGuideData();

	void AddGuideData(const GuideData& data);
	UBOOL GetGuideData(UINT32 id, GuideData& data);
	GuideDataMap& GetGuideDataMap();

	UBOOL IsGuideEnable(UINT32 id);
	void ClearGuideState();
	GuideStateMap& GetGuideStateMap();
	GuideState GetGuideState(UINT32 id);
	void UpdateGuideState(UINT32 id, GuideState state);

	GuideEventMap& GetActivateGuideEventMap();
	GuideEventMap& GetDeactiveGuideEventMap();

protected:

	GuideDataMap _GuideDataMap;
	GuideStateMap _GuideStateMap;
	GuideEventMap _ActivateGuideEventMap;
	GuideEventMap _DeactiveGuideEventMap;
};

#endif