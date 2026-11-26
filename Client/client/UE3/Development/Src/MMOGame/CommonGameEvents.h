#ifndef _COMMONGAMEEVENTS_H_
#define _COMMONGAMEEVENTS_H_

#include "MMOGameEvent.h"

//
// GameEvent_UpdateMoney
//
struct MMOGameEvent_UpdateMoney : public MMOGameEvent
{
public:	

	explicit MMOGameEvent_UpdateMoney(MONEY_TYPE moneyType, INT oldCnt, INT newCnt)
		: MoneyType(moneyType)
		, OldCount(oldCnt)
		, NewCount(newCnt)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_UpdateMoney;
	}

	MONEY_TYPE MoneyType;
	INT OldCount;
	INT NewCount;
};

struct MMOGameEvent_VariableChange : public MMOGameEvent_Any
{
	MMOGameEvent_VariableChange(INT iVarType, INT iVarReferID, INT iVarID, INT iValue, EGameEvent varChangeType)
		: MMOGameEvent_Any(varChangeType)
		, VarType(iVarType)
		, VarReferID(iVarReferID)
		, VarID(iVarID)
		, VarValue(iValue)
	{
	}

	INT VarType;
	INT VarReferID;
	INT VarID;
	INT VarValue;
};

//
// MMOGameEvent_CharacterDie
//
struct MMOGameEvent_CharacterDie : public MMOGameEvent
{
public:	

	MMOGameEvent_CharacterDie(AMMOActor* diedActor)
		: DiedActor(diedActor)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_CharacterDie;
	}

	AMMOActor* DiedActor;
};

struct MMOGameEvent_MarqueeNoticeInfo : public MMOGameEvent
{
public:	

	MMOGameEvent_MarqueeNoticeInfo( FString noticeText , FString textColor , INT repeatTimes , INT showSpeed )
		: TextColor(textColor)
		, RepeatTimes(repeatTimes)
		, ShowSpeed(showSpeed)
		, NoticeText(noticeText)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_MarqueeNoticeInfo;
	}

	FString TextColor;
	INT RepeatTimes;
	FString NoticeText;
	INT ShowSpeed;
};

struct MMOGameEvent_RollingNoticeInfo : public MMOGameEvent
{
public:	

	MMOGameEvent_RollingNoticeInfo( FString noticeText , FString textColor )
		: TextColor(textColor)
		, NoticeText(noticeText)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_RollingNoticeInfo;
	}

	FString TextColor;
	FString NoticeText;
};

//
struct MMOGameEvent_SendMail : public MMOGameEvent
{
public:	

	MMOGameEvent_SendMail(FString playName)
		: PlayName(playName)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_SendMailTo;
	}

	FString PlayName;
};

//struct MMOGameEvent_AttentionBy : public MMOGameEvent
//{
//public:	
//
//	MMOGameEvent_SendMail( INT roleId , FString playName)
//		: PlayName(playName)
//		, RoleId(roleId)
//	{
//	}
//
//	virtual EGameEvent GetEventType() const
//	{
//		return GameEvent_AttentionBy;
//	}
//
//	FString PlayName;
//	INT	RoleId;
//};

struct MMOGameEvent_UpdateNearContact : public MMOGameEvent
{
public:	

	MMOGameEvent_UpdateNearContact()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_UpdateNearContact;
	}

	
};

// MMOGameEvent_ActorAppear
//
struct MMOGameEvent_ActorAppear : public MMOGameEvent
{
public:	

	MMOGameEvent_ActorAppear(AActor* actor)
		: Actor(actor)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_ActorAppear;
	}

	AActor* Actor;
};

//
// MMOGameEvent_ActorDestroyed
//
struct MMOGameEvent_ActorDestroyed : public MMOGameEvent
{
public:	

	MMOGameEvent_ActorDestroyed(AMMOActor* actor)
		: Actor(actor)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_ActorDestroyed;
	}

	AMMOActor* Actor;
};

//
// MMOGameEvent_NPCAppear
//
struct MMOGameEvent_NPCAppear : public MMOGameEvent
{
public:	

	MMOGameEvent_NPCAppear(ANPC* actor)
		: Actor(actor)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_NPCAppear;
	}

	ANPC* Actor;
};

//
// MMOGameEvent_NPCDestroyed
//
struct MMOGameEvent_NPCDestroyed : public MMOGameEvent
{
public:	

	MMOGameEvent_NPCDestroyed(ANPC* actor)
		: Actor(actor)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_NPCDestroyed;
	}

	ANPC* Actor;
};

//
// MMOGameEvent_SceneLoadFinish
//
struct MMOGameEvent_SceneLoadFinish : public MMOGameEvent
{
public:	

	MMOGameEvent_SceneLoadFinish()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEVent_SceneLoadFinish;
	}
};

struct MMOGameEvent_LeaveGame : public MMOGameEvent
{
public:	

	MMOGameEvent_LeaveGame()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_LeaveGame;
	}
};

struct MMOGameEvent_OtherPlayerLevelUp : public MMOGameEvent
{
public:	
	INT roleID;
	INT lv;
	MMOGameEvent_OtherPlayerLevelUp()
	{
		roleID = 0;
		lv = 0;
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_OtherPlayerLevelUp;
	}
};

//
struct MMOGameEvent_PlayerBaseInfo : public MMOGameEvent
{
public:	

	MMOGameEvent_PlayerBaseInfo( UINT32 dwRoldID , struct PlayerBaseInfo* playerBaseInfo )
		: _RoldID(dwRoldID)
	{
		_PlayerBaseInfo = playerBaseInfo;
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_PlayerBaseInfo;
	}

	UINT32 _RoldID;
	struct PlayerBaseInfo* _PlayerBaseInfo;
};

struct MMOGameEvent_ClearBag : public MMOGameEvent
{
public:	

	MMOGameEvent_ClearBag()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_ClearBag;
	}
};

struct MMOGameEvent_BagOpenPack : public MMOGameEvent
{
public:	

	MMOGameEvent_BagOpenPack( INT id  )
		: itemId(id)
	{
		
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_BagOpenPack;
	}

	INT itemId;
};

struct MMOGameEvent_GaryBagItemByNpcShop : public MMOGameEvent
{
public:	

	MMOGameEvent_GaryBagItemByNpcShop( BOOL v  )
		: bShow(v)
	{

	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_GrayBagItemByShop;
	}

	BOOL bShow;
};


struct MMOGameEvent_NPCInteract : public MMOGameEvent
{
public:	

	MMOGameEvent_NPCInteract(INT npcServerID, INT npcTempalteID)
		: NPCServerID(npcServerID)
		, NPCTemplateID(npcTempalteID)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_NPCInteract;
	}

	INT NPCServerID;
	INT NPCTemplateID;
};


struct MMOGameEvent_SortTimer : public MMOGameEvent
{
public:	

	MMOGameEvent_SortTimer(INT timeLeft)
		: TimeSecondsLeft(timeLeft)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_SortTimer;
	}

	INT TimeSecondsLeft;
};

struct MMOGameEvent_MsgboxCallBack : public MMOGameEvent
{
public:	

	MMOGameEvent_MsgboxCallBack(EGameEvent evtType , UGFxObject* box , UBOOL agree )
		: EventType(evtType)
		
	{
		Box = box;
		Agree = agree;
	}

	UGFxObject* Box;
	UBOOL	Agree;

	EGameEvent EventType;
	virtual EGameEvent GetEventType() const{ return EventType; }


};

struct MMOGameEvent_SortTimerBegin : public MMOGameEvent
{
public:	

	MMOGameEvent_SortTimerBegin()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_SortTimerBegin;
	}
};

struct MMOGameEvent_SortTimerEnd : public MMOGameEvent
{
public:	

	MMOGameEvent_SortTimerEnd()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_SortTimerEnd;
	}
};

struct MMOGameEvent_MatineeSubtitle : public MMOGameEvent
{
public:

	enum EMatineeSubtitleEventType
	{
		EMSET_Show,
		EMSET_UpdateSubtitle
	};

	MMOGameEvent_MatineeSubtitle(const FString& subTitle, EMatineeSubtitleEventType evtType)
		: SubtitleEventType(evtType)
		, ShowSubtitle(false)
		, Subtitle(subTitle)
	{
	}

	MMOGameEvent_MatineeSubtitle(bool show, EMatineeSubtitleEventType evtType)
		: SubtitleEventType(evtType)
		, ShowSubtitle(show)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_MatineeSubtitle;
	}

	EMatineeSubtitleEventType SubtitleEventType;
	bool ShowSubtitle;
	FString Subtitle;
};


struct MMOGameEvent_MainPlayerDataChange : public MMOGameEvent
{
public:

	enum EPlayerDataChangeEventType
	{
		EMSET_HP,
		EMSET_MP,
	};

	MMOGameEvent_MainPlayerDataChange(EPlayerDataChangeEventType changeType, class AMMOPlayer* mainPlayer, INT currIntVal, INT maxIntVal = 1)
		: DataChangeEventType(changeType)
		, MainPlayer(mainPlayer)
		, CurrentIntValue(currIntVal)
		, MaxIntValue(maxIntVal)
		, CurrentFloatValue(0.0f)
		, MaxFloatValue(1.0f)
		, BoolValue(FALSE)
	{
	}

	MMOGameEvent_MainPlayerDataChange(EPlayerDataChangeEventType changeType, class AMMOPlayer* mainPlayer, FLOAT currFloatVal, FLOAT maxFloatVal = 1)
		: DataChangeEventType(changeType)
		, MainPlayer(mainPlayer)
		, CurrentIntValue(0)
		, MaxIntValue(1)
		, CurrentFloatValue(currFloatVal)
		, MaxFloatValue(maxFloatVal)
		, BoolValue(FALSE)
	{
	}

	MMOGameEvent_MainPlayerDataChange(EPlayerDataChangeEventType changeType, class AMMOPlayer* mainPlayer, UBOOL boolVal)
		: DataChangeEventType(changeType)
		, MainPlayer(mainPlayer)
		, CurrentIntValue(0)
		, MaxIntValue(1)
		, CurrentFloatValue(0.0f)
		, MaxFloatValue(1.0f)
		, BoolValue(boolVal)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_MainPlayerDataChange;
	}

	EPlayerDataChangeEventType DataChangeEventType;
	class AMMOPlayer* MainPlayer;

	INT CurrentIntValue;
	INT MaxIntValue;

	FLOAT CurrentFloatValue;
	FLOAT MaxFloatValue;

	UBOOL BoolValue;
};

struct MMOGameEvent_ServerError : public MMOGameEvent
{
	MMOGameEvent_ServerError(INT errCode, INT iValue1, INT iValue2)
		: ErrCode(errCode)
		, Param1(iValue1)
		, Param2(iValue2)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_ServerError;
	}

	INT ErrCode;
	INT Param1;
	INT Param2;
};

struct MMOGameEvent_OneInt : public MMOGameEvent_Any
{
	MMOGameEvent_OneInt(EGameEvent evtType, INT iValue)
		: MMOGameEvent_Any(evtType)
		, IntData(iValue)
	{
	}

	INT IntData;
};

struct MMOGameEvent_NewCD : public MMOGameEvent_Any
{
	MMOGameEvent_NewCD(EGameEvent evtType, INT cdType , INT cdId )
		: MMOGameEvent_Any(evtType)
		, CDType(cdType)
		, CDId(cdId)
	{
	}

	INT CDType;
	INT CDId;
};

struct MMOGameEvent_AddMsgBall : public MMOGameEvent_Any
{
	MMOGameEvent_AddMsgBall( EGameEvent evtType, INT msgBallType , INT msgNum )
		: MMOGameEvent_Any(evtType)		
	{
		MsgBallType = msgBallType;
		MsgNum = msgNum;
	}

	INT MsgBallType;
	INT MsgNum;
};

struct MMOGameEvent_EquipSkill : public MMOGameEvent
{
public:	

	MMOGameEvent_EquipSkill(EGameEvent evtType, INT skillSlot, INT skillID )
		: EventType(evtType)
		, SkillSlotEnum(skillSlot)
		, SkillID(skillID)
	{
	}

	INT SkillSlotEnum;
	INT	SkillID;

	EGameEvent EventType;
	virtual EGameEvent GetEventType() const{ return EventType; }
};

struct MMOGameEvent_EquipRuneSkill : public MMOGameEvent_EquipSkill
{
public:	

	MMOGameEvent_EquipRuneSkill(EGameEvent evtType, INT skillSlot, INT skillID, INT runeSlot )
		: MMOGameEvent_EquipSkill( evtType, skillSlot, skillID )
		, RuneIdx(runeSlot)
	{
	}

	INT RuneIdx;
};

struct MMOGameEvent_SkillLevelUp : public MMOGameEvent
{
public:	

	MMOGameEvent_SkillLevelUp(INT skillID, INT newLevel, INT skillRuneID )
		: SkillID(skillID)
		, NewLevel(newLevel)
		, SkillRuneID(skillRuneID)
	{
	}

	INT SkillID;
	INT	SkillRuneID;
	INT NewLevel;

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_SkillLevelUp;
	}
};

struct MMOGameEvent_ProcessDynSkill : public MMOGameEvent_Any
{
public:	

	MMOGameEvent_ProcessDynSkill( EGameEvent evtType, INT uniqueSkillID )
		: MMOGameEvent_Any(evtType),
		  UniqueSkillID(uniqueSkillID)
	{
		EquipItemID = -1;
	}

	INT UniqueSkillID;
	INT	EquipItemID;
};

struct MMOGameEvent_Item : public MMOGameEvent_Any
{
public:	

	MMOGameEvent_Item(EGameEvent evtType, INT iConIdx, INT iSlotIdx)
		: MMOGameEvent_Any(evtType)
		, ContainerIdx(iConIdx)
		, SlotIdx(iSlotIdx)
	{
		ItemTemplateID = 0;
	}

	INT ContainerIdx;
	INT SlotIdx;
	INT ItemTemplateID;
};

struct MMOGameEvent_ItemSwap : public MMOGameEvent
{
	MMOGameEvent_ItemSwap( INT srcConIdx, INT srcSlotIdx, INT dstConIdx, INT dstSlotIdx)
		: SrcConID(srcConIdx)
		, SrcSlotID(srcSlotIdx)
		, DstConID(dstConIdx)
		, DstSlotID(dstSlotIdx)
	{
	}
	virtual EGameEvent GetEventType() const{ return GameEvent_Item_SlotSwapped; }

	INT SrcConID;
	INT SrcSlotID;
	INT DstConID;
	INT DstSlotID;
};

struct MMOGameEvent_ShowGuildView : public MMOGameEvent
{
public:	

	MMOGameEvent_ShowGuildView()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_ShowGuildView;
	}
};

struct MMOGameEvent_UpdatePlayerVipHint : public MMOGameEvent
{
public:	

	MMOGameEvent_UpdatePlayerVipHint()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_UpdatePlayerVipHint;
	}
};

struct MMOGameEvent_ActiveRevivePnt : public MMOGameEvent
{
	MMOGameEvent_ActiveRevivePnt( FVector &pos )
		:_ActivePos(pos)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_ActiveRevivePnt;
	}

	FVector _ActivePos;
};

struct MMOGameEvent_UpdateRolePow : public MMOGameEvent
{
	MMOGameEvent_UpdateRolePow( INT id,INT p)
		:roleId(id),
		pow(p)
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_CharactorPower;
	}

	INT roleId;
	INT pow;
};

struct MMOGameEvent_DailySignIn : public MMOGameEvent
{
public:

	MMOGameEvent_DailySignIn( INT signInDay, UBOOL isSignedIn, UBOOL isToBeValid )
		: LastSignInDay( signInDay )
		, CanCurSigneIn( isSignedIn )
		, IsToBeValid( isToBeValid )
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_SyncSignInDay;
	}

	INT LastSignInDay;
	UBOOL CanCurSigneIn;
	UBOOL IsToBeValid;
};

#endif