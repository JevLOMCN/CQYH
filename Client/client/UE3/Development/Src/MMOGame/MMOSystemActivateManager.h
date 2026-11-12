#ifndef _MMOSYSTEMACTIVATEMANAGER_H_
#define _MMOSYSTEMACTIVATEMANAGER_H_

#include "MMOSingleton.h"
#include <map>
#include <vector>
#include <queue>

// enable system
enum EActivateSystemType
{
	EAST_Guild,					// guild
	EAST_Schedule,				// activity
	EAST_GoldStore,				// gold trading house
	EAST_LingXing,				// spirituality
	EAST_LongWei,				// Dragon Guard heart method
	EAST_NormalQuest,			// normal quest
	EAST_QiXi,					// aura
	EAST_Mall,					// mall
	EAST_Repute,				// reputation
	EAST_MasterStudent,			// master/apprentice
	EAST_Enhance,				// physique enhancement
	EAST_Riding,				// mount
	EAST_Handbook,				// Baixiaosheng (handbook)
	EAST_Tatol
};

class MMOSystemActivateManager : public SingletonMeyers<MMOSystemActivateManager>, public INativeGameEventHandler
{
public:
	MMOSystemActivateManager();
	virtual ~MMOSystemActivateManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	// new feature enabled
	INT GetNextSystemType();
	INT GetChildMenuType(INT systemType);
	UBOOL GetSystemInfo(INT systemType, FString& icon, FString& name);

	void NotifyPlayerLevelChange(INT curLevel);

protected:
	void _PushNewSystem(INT systemType);
	void _ActivateSystem(UINT32 level);

	typedef std::map<INT,INT> ActivateSystemLevelMap;
	ActivateSystemLevelMap _ActivateSystemLevelMap;

	typedef std::set<INT> ActivateSystemSet;
	ActivateSystemSet _ActivateSystemSet;

	typedef std::queue<INT> ActivateSystemQueue;
	ActivateSystemQueue _ActivateSystemQueue;
};

#endif