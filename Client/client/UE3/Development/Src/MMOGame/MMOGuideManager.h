#ifndef _MMOGUIDEMANAGER_H_
#define _MMOGUIDEMANAGER_H_

#include "MMOSingleton.h"
#include <map>
#include <vector>
#include "UnString.h"
#include "guild_def.h"
#include "MMOFacade.h"
#include "MMOGuideData.h"

// guide record
struct GuideRecord
{
	UINT32 Id;
	GuideState State;
};

// Tutorial event
struct MMOGameEvent_Guide : public MMOGameEvent
{
public:
	MMOGameEvent_Guide(EGuideEventType eventType, UINT32 value1 = 0, UINT32 value2 = 0, UINT32 value3 = 0)
		: _EventType(eventType), _Value1(value1), _Value2(value2), _Value3(value3)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_Guide;
	}

	EGuideEventType _EventType;
	UINT32 _Value1;
	UINT32 _Value2;
	UINT32 _Value3;
};

class MMOGuideManager : public SingletonMeyers<MMOGuideManager>, public INativeGameEventHandler
{
public:
	MMOGuideManager();
	virtual ~MMOGuideManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void LoadRecord(BYTE* buf, UINT size);
	void SaveRecord();

	void SetGuideEnable(UBOOL isEnable);
    UBOOL IsGuideEnable(){ return _IsGuideEnable; }

	// for testing position
	void TestGuideArrow(INT id, INT offsetX, INT offsetY);
	void TestGuideBox(INT id, INT offsetX, INT offsetY, INT width, INT height);

protected:
	// condition check
	UBOOL _PreGuideCheck(UINT32 id);
	UBOOL _LevelCheck(UINT32 id);
	UBOOL _QuestCheck(UINT32 id);
	UBOOL _ItemCheck(UINT32 id);
	UBOOL _NpcCheck(UINT32 id);
	UBOOL _DoodadTypeCheck(UINT32 id);
	UBOOL _EnterZoneCheck(UINT32 id);
	UBOOL _LearnSkillCheck(UINT32 id);
	UBOOL _ClassCheck(UINT32 id);
	UBOOL _ConditionCheck(UINT32 id);

	void _ActivateGuide(UINT32 id);
	void _DeactiveGuide(UINT32 id);

	UGFxObject* _GetTargetComponent(UGFxObject* targetPanel, const GuideData& data);
	void _AttachGuidePanel(UINT32 id);
	void _DetachGuidePanel(UINT32 id);
	void _DetachAllGuidePanel();

	void _UpdateAllGuidePanel();
	void _UpdateGuideBox(UGFxObject* guide, const GuideData& data);											// update guide box
	void _UpdateBagSlotGuide(UMMOUIBase* targetPanel, UGFxObject* guide, const GuideData& data);			// update bag slot guide

	void _PlayGuideSound(UINT32 id);
	void _UpdateGuideState(UINT32 id);

protected:
	typedef std::map<UINT32, UGFxObject*, M_UE3_STL_ALLOC_MAP(UINT32, UGFxObject*)> GuideObjectMap;
	GuideObjectMap _GuideObjectMap;

	UINT32 _CurNpcId;
	UINT32 _CurDoodadType;
	UINT32 _CurZoneGuideId;
	UBOOL _IsGuideEnable;
	UBOOL _IsLoadRecord;
};

#endif