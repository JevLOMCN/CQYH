#ifndef _MMOASSISTMANAGER_H_
#define _MMOASSISTMANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMOUtil.h"
#include "MMOAssistData.h"
#include <queue>
#include "MMOClientVariableManager.h"

#define SAVE_TO_SERVER
#define PROTECT_DATA_COUNT (ECVT_Assist_Protect_End - ECVT_Assist_Protect_Begin)
#define ASSIST_ITEM_CHECK_TIME 0.5f

class MMOAssistManager : public SingletonMeyers<MMOAssistManager>, public INativeGameEventHandler
{
public:
	MMOAssistManager();
	virtual ~MMOAssistManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void LoadData(UINT32 skillData, const UINT32 (&protectData)[PROTECT_DATA_COUNT]);
	void SaveData();

	// Drop
	UBOOL IsDropValuable(UINT32 id);
	UBOOL IsDropAutoPick(UINT32 id);
	UBOOL IsDropVisible(UINT32 id);
	void UpdateDropDisplay(UINT32 id);

	// Game mode
	UBOOL IsQuestMode();

	void NotifyEnterGame();

protected:
	void _LoadAssistDrop();
	void _SaveAssistDrop();

	void _LoadAssistSkill(UINT32 skillData);
	void _SaveAssistSkill();

	void _LoadAssistProtect(const UINT32 (&protectData)[PROTECT_DATA_COUNT]);
	void _SaveAssistProtect();

	void _UpdateAssistItem(FLOAT deltaTime);
	void _UseSlowHpItem(FLOAT deltaTime);
	void _UseSlowMpItem(FLOAT deltaTime);
	void _UseFastHpItem1(FLOAT deltaTime);
	void _UseFastMpItem1(FLOAT deltaTime);
	void _UseFastHpItem2(FLOAT deltaTime);
	void _UseFastMpItem2(FLOAT deltaTime);
	void _UseProtectHpItem(FLOAT deltaTime);
	void _UseProtectMpItem(FLOAT deltaTime);
	UBOOL _UseItem(UINT32 itemId);

private:
	FLOAT _AssistItemCheckTimeProcess;
	FLOAT _AssistItemTimeProcess[EPT_TOTAL];
	UBOOL _FirstEnterGame;
};

#endif