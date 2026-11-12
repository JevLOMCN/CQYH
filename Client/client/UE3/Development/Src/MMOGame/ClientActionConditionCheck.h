#ifndef _CLIENTACTIONCONDITIONCHECK_H_
#define _CLIENTACTIONCONDITIONCHECK_H_

#include "MMOPrerequisites.h"
#include "MMOUtil.h"

struct FClientActionConditionContex
{
	EClientActionConditionCheckType ChceckType;
	FString ResultMessage;
    FString ResultName;

	// Temporary context variables to be used go below
	struct FClientItemInfo* Item;
	class UItem* ItemTemplate;
	class ANPC* NPC;
	class AMMOActor* PlayerActor;
	INT UsefulInt;
	FString SelfPlayerName;

	FString MailReceiver;
	FString MailSubject;
	FString MailContent;

	FClientActionConditionContex(EClientActionConditionCheckType checkType = ECACCT_None);
};


void InitClientActionInformationNotify();
void UninitClientActionInformationNotify();

bool CheckClientActionCondition(FClientActionConditionContex& contex);


typedef bool (*CheckActionConditionFun)(FClientActionConditionContex& contex);

class ClientActionConditionSequence
{
	typedef std::list<EClientActionConditionCheckType, M_UE3_STL_ALLOC(EClientActionConditionCheckType) > FClientActionConditionContexList;
	FClientActionConditionContexList _ConditionCheckList;

public:

	void AddConditionCheckType(EClientActionConditionCheckType checkType);
	bool CheckClientActionCondition(FClientActionConditionContex& contex);
};


// Condition check for using an item
class ClientActionCondition_UseItem : public ClientActionConditionSequence
{
public:

	ClientActionCondition_UseItem()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_UseItem_CheckClass);
		AddConditionCheckType(ECACCT_UseItem_CheckLv);
		AddConditionCheckType(ECACCT_Item_CheckMySex);
		AddConditionCheckType(ECACCT_UseItem_CheckPhyAtk);
		AddConditionCheckType(ECACCT_UseItem_CheckMagAtk);
		AddConditionCheckType(ECACCT_UseItem_CheckTaoAtk);
		//AddConditionCheckType(ECACCT_UseItem_CheckBind); this condition check is not needed
		//AddConditionCheckType(ECACCT_Item_CheckInEnhance);
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
	}
};

// Any operation on an item cannot happen in these states
class ClientActionCondition_ItemAction : public ClientActionConditionSequence
{
public:

	ClientActionCondition_ItemAction()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
	}
};

// Item position move
class ClientActionCondition_ItemMove : public ClientActionConditionSequence
{
public:

	ClientActionCondition_ItemMove()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
	}
};

class ClientActionCondition_ItemsSort : public ClientActionConditionSequence
{
public:

	ClientActionCondition_ItemsSort()
	{
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
		AddConditionCheckType(ECACCT_Item_CheckAllInContainerIfInPanelUsing);
	}
};

//
bool Check_ECACCT_Item_CheckAllInContainerIfInPanelUsing( FClientActionConditionContex& contex );
class ClientActionCondition_ItemForbiddenInPanelOpen : public ClientActionConditionSequence
{
public:

	ClientActionCondition_ItemForbiddenInPanelOpen()
	{
		//AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		//AddConditionCheckType(ECACCT_Item_CheckInEnhanceOpened);
		//AddConditionCheckType(ECACCT_Item_CheckInTradeOpend);
		AddConditionCheckType(ECACCT_Item_CheckAllInContainerIfInPanelUsing);
	}
};

class ClientActionCondition_EquipItem : public ClientActionConditionSequence
{
public:

	ClientActionCondition_EquipItem()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_EquipItem_CheckClass);
		AddConditionCheckType(ECACCT_EquipItem_CheckLv);
		AddConditionCheckType(ECACCT_Item_CheckMySex);
		AddConditionCheckType(ECACCT_UseItem_CheckPhyAtk);
		AddConditionCheckType(ECACCT_UseItem_CheckMagAtk);
		AddConditionCheckType(ECACCT_UseItem_CheckTaoAtk);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
		AddConditionCheckType(ECACCT_Item_CheckInWeaponExp);
	}
};

class ClientActionCondition_UnEquipItem : public ClientActionConditionSequence
{
public:

	ClientActionCondition_UnEquipItem()
	{
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
	}
};

class ClientActionCondition_Hero_EquipItem : public ClientActionConditionSequence
{
public:

	ClientActionCondition_Hero_EquipItem()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Hero_EquipItem_CheckClass);
		AddConditionCheckType(ECACCT_Hero_EquipItem_CheckLv);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
		AddConditionCheckType(ECACCT_Item_CheckInWeaponExp);
	}
};

class ClientActionCondition_SwampItem : public ClientActionConditionSequence
{
public:

	ClientActionCondition_SwampItem()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
		AddConditionCheckType(ECACCT_Item_CheckInWeaponExp);
	}
};

class ClientActionCondition_DelItem : public ClientActionConditionSequence
{
public:

	ClientActionCondition_DelItem()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
		AddConditionCheckType(ECACCT_Item_CheckInWeaponExp);
		//AddConditionCheckType(ECACCT_ALL_ActorLevel);
	}
};

class ClientActionCondition_SplitItem : public ClientActionConditionSequence
{
public:
	ClientActionCondition_SplitItem()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
	}
};

//Soul operations are similar to item move judgment logic
class ClientActionCondition_SoulEmbedOrUnEmbed : public ClientActionConditionSequence
{
public:
	ClientActionCondition_SoulEmbedOrUnEmbed()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInEnhance);
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
		AddConditionCheckType(ECACCT_Item_CheckInSoulEmbed);
		AddConditionCheckType(ECACCT_Item_CheckInSoulUnEmbed);
	}
};

class ClientActionCondition_SoulLevelUp : public ClientActionConditionSequence
{
public:
	ClientActionCondition_SoulLevelUp()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInEnhance);
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
		AddConditionCheckType(ECACCT_Item_CheckInSoulEmbed);
		AddConditionCheckType(ECACCT_Item_CheckInSoulUnEmbed);
	}
};

//Inscription upgrade replacement
class ClientActionCondition_WeaponSkill : public ClientActionConditionSequence
{
public:
	ClientActionCondition_WeaponSkill()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
	}
};

class ClientActionCondition_SkillSetting : public ClientActionConditionSequence
{
public:
	ClientActionCondition_SkillSetting()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
	}
};

class ClientActionCondition_TradeItem : public ClientActionConditionSequence
{
public:

	ClientActionCondition_TradeItem()
	{
		//AddConditionCheckType(ECACCT_Item_CheckCanTrade);
		AddConditionCheckType(ECACCT_UseItem_CheckBind);
		AddConditionCheckType(ECACCT_Item_CheckInEnhance);
		//AddConditionCheckType(ECACCT_TradeItem_CheckDurablility);
		//AddConditionCheckType(ECACCT_TradeItem_CheckTaskItem);
		AddConditionCheckType(ECACCT_TradeItem_CheckItemSign);
	}
};

class ClientActionCondition_EnhanceItem : public ClientActionConditionSequence
{
public:

	ClientActionCondition_EnhanceItem()
	{
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
	}
};

class ClientActionCondition_EquipLevelUp : public ClientActionConditionSequence
{
public:

	ClientActionCondition_EquipLevelUp()
	{
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
		AddConditionCheckType(ECACCT_EquipLevelUp_CheckDurability);
	}
};
bool Check_ECACCT_EquipLevelUp_CheckDurability(FClientActionConditionContex& contex);

class ClientActionCondition_BackDecoratedMakeUp : public ClientActionConditionSequence
{
public:

	ClientActionCondition_BackDecoratedMakeUp()
	{
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
	}
};

class ClientActionCondition_BackDecoratedUpgrade : public ClientActionConditionSequence
{
public:

	ClientActionCondition_BackDecoratedUpgrade()
	{
		//AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
	}
};

class ClientActionCondition_ItemCollection : public ClientActionConditionSequence
{
public:

	ClientActionCondition_ItemCollection()
	{
		AddConditionCheckType(ECACCT_UseItem_CheckBind);
		AddConditionCheckType(ECACCT_Item_CheckInEnhance);
	}
};

class ClientActionCondition_StallItem : public ClientActionConditionSequence
{
public:
	ClientActionCondition_StallItem()
	{
		AddConditionCheckType(ECACCT_Item_CheckCanStall);
		AddConditionCheckType(ECACCT_Item_CheckInEnhance);
		AddConditionCheckType(ECACCT_Item_CheckInTrade);
		AddConditionCheckType(ECACCT_Item_CheckInSoulEmbed);
		AddConditionCheckType(ECACCT_Item_CheckInSoulUnEmbed);
		AddConditionCheckType(ECACCT_Item_CheckInPanelUsing);
	}
};

class ClientActionCondition_OperationMaterial : public ClientActionConditionSequence
{
public:
	ClientActionCondition_OperationMaterial()
	{
		AddConditionCheckType(ECACCT_Material_CheckEquipTool);
		AddConditionCheckType(ECACCT_Material_WeaponDuration);
		AddConditionCheckType(ECACCT_Material_BagFull);
	}
};


bool Check_ECACCT_UseItem_CheckStallState(FClientActionConditionContex& contex);
bool Check_ECACCT_UseItem_CheckClass(FClientActionConditionContex& contex);
bool Check_ECACCT_UseItem_CheckLv(FClientActionConditionContex& contex);
bool Check_ECACCT_UseItem_CheckPhyAtk(FClientActionConditionContex& contex);
bool Check_ECACCT_UseItem_CheckMagAtk(FClientActionConditionContex& contex);
bool Check_ECACCT_UseItem_CheckTaoAtk(FClientActionConditionContex& contex);
bool Check_ECACCT_Hero_UseItem_CheckClass(FClientActionConditionContex& contex);
bool Check_ECACCT_Hero_UseItem_CheckLv(FClientActionConditionContex& contex);
bool Check_ECACCT_UseItem_CheckBind(FClientActionConditionContex& contex);
bool Check_ECACCT_TradeItem_CheckDurablility(FClientActionConditionContex& contex);
bool Check_ECACCT_TradeItem_CheckTaskItem(FClientActionConditionContex& contex);
bool Check_ECACCT_TradeItem_CheckItemSign(FClientActionConditionContex& contex);
bool Check_ECACCT_EquipItem_CheckClass(FClientActionConditionContex& contex);
bool Check_ECACCT_EquipItem_CheckLv(FClientActionConditionContex& contex);
bool Check_ECACCT_ALL_ActorLevel(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckMySex(FClientActionConditionContex& contex);
bool Check_ECACCT_Hero_EquipItem_CheckClass(FClientActionConditionContex& contex);
bool Check_ECACCT_Hero_EquipItem_CheckLv(FClientActionConditionContex& contex);

bool Check_ECACCT_Item_CheckInEnhanceOpened(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckInTradeOpend(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckInWeaponExp(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckCanStall(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckCanTrade(FClientActionConditionContex& contex);

bool Check_ECACCT_Material_BagFull(FClientActionConditionContex& contex);
bool Check_ECACCT_Material_CheckEquipTool(FClientActionConditionContex& contex);
bool Check_ECACCT_Material_WeaponDuration(FClientActionConditionContex& contex);

bool Check_ECACCT_MailSend_CheckReceiver(FClientActionConditionContex& contex);
bool Check_ECACCT_MailSend_CheckReceiverSelf(FClientActionConditionContex& contex);
bool Check_ECACCT_MailSend_CheckSubject(FClientActionConditionContex& contex);
bool Check_ECACCT_MailSend_CheckContent(FClientActionConditionContex& contex);
bool Check_ECACCT_MailSend_CheckContentMaxLen(FClientActionConditionContex& contex);


class ClientActionCondition_SendMail : public ClientActionConditionSequence
{
public:

	ClientActionCondition_SendMail()
	{
		AddConditionCheckType(ECACCT_MailSend_CheckReceiverSelf);
		AddConditionCheckType(ECACCT_MailSend_CheckReceiver);
		AddConditionCheckType(ECACCT_MailSend_CheckSubject);
		AddConditionCheckType(ECACCT_MailSend_CheckContent);
		AddConditionCheckType(ECACCT_MailSend_CheckContentMaxLen);
	}
};

bool Check_ECACCT_Item_CheckInEnhance(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckInTrade(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckInSoulEmbed(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckInSoulUnEmbed(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckInSoulLevelUp(FClientActionConditionContex& contex);
bool Check_ECACCT_Item_CheckInPanelUsing(FClientActionConditionContex& contex);
bool Check_ECACCT_MainPlayer_Is_Trading(FClientActionConditionContex& contex);
//Check whether line switching is allowed in the scene
class ClientActionCondition_ChangeSceneLine : public ClientActionConditionSequence
{
public:

	ClientActionCondition_ChangeSceneLine()
	{
		AddConditionCheckType(ECACCT_CurrentScene_Has_Lines);
		AddConditionCheckType(ECACCT_UseItem_CheckStallState);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Trading);
		AddConditionCheckType(ECACCT_MainPlayer_Is_Combating);
	}
};

bool Check_ECACCT_CurrentScene_Has_Lines(FClientActionConditionContex& contex);
bool Check_ECACCT_MainPlayer_Is_Combating(FClientActionConditionContex& contex);
#endif
