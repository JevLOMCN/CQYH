#ifndef __MMOGameEvent_Splash_H__
#define __MMOGameEvent_Splash_H__

#include "MMOGameEvent.h"

struct MMOGameEvent_Splash : public MMOGameEvent
{
public:

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_Splash;
	}

	enum ESplashEventType
	{
		ESET_ShowSplashUI,
		ESET_LoginGameSuccess,
		ESET_CreateRoleSuccess,
		//ESET_JumpSelectRole,
		ESET_JumpCreateRole,
		ESET_EnterGameSuccess,
		ESET_EnterGameEnd,
		ESET_CreateListRoleCheck,
		ESET_UpdateRoleListUI,
	};

	MMOGameEvent_Splash() : EvtType(ESET_ShowSplashUI), PositionInfo(0.0f, 0.0f, 0.0f), SelectRoleIndex(-1), ShowSplashUI(FALSE) {}

	ESplashEventType EvtType;
	FVector PositionInfo;
	INT SelectRoleIndex;
	UBOOL ShowSplashUI;
};

#endif
