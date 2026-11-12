#ifndef _DIALOGOPTIONACTION_H_
#define _DIALOGOPTIONACTION_H_

#include "MMOPrerequisites.h"
#include "IDialogOptionAction.h"
#include "MMOUtil.h"

typedef class IDialogOptionAction* (*CreateDialogOptionActionFunction)();

typedef std::map<std::wstring, CreateDialogOptionActionFunction, M_UE3_STL_ALLOC_MAP(std::wstring, CreateDialogOptionActionFunction*) > DialogOptionActionMap;
extern DialogOptionActionMap g_dialogOptionActMap;

#define M_DECL_DIAG_OPT_ACT_FUN(className, keyWord) IDialogOptionAction* CreateDialogOptionAction##className() { return new className; }
#define M_REG_DIAG_OPT_ACT_FUN(className, keyWord) g_dialogOptionActMap[keyWord] = CreateDialogOptionAction##className;

class AbstractDialogOptionAction : public IDialogOptionAction
{
public:

	AbstractDialogOptionAction();
	virtual ~AbstractDialogOptionAction();

	virtual FString GetDisplayText() const { return OptionDisplayText; }
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters)
	{
		OptionDisplayText = DisplayText;
		if(pParameters)
		{
			OptionParameters = *pParameters;
		}
	}

protected:

	FString OptionDisplayText;
	FString OptionParameters;
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenPanel
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_OpenPanel : public AbstractDialogOptionAction
{
	FName UIPanelName;

public:
	DialogOptionAction_UICommand_OpenPanel(const FName& PanelName);
	virtual FString GetTypeName() const { return FString(TEXT("OpenPanel")); }
	virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenWeaponSkill
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_OpenWeaponSkill : public AbstractDialogOptionAction
{
public:
	//DialogOptionAction_UICommand_OpenWeaponSkill();
	virtual FString GetTypeName() const { return FString(TEXT("OpenWeaponSkill")); }
	virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenWeaponSkill
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_OpenWeaponSkillSeal : public AbstractDialogOptionAction
{
public:
	//DialogOptionAction_UICommand_OpenWeaponSkillSeal();
	virtual FString GetTypeName() const { return FString(TEXT("OpenWeaponSkillSeal")); }
	virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_BeginSpecialRepair
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_BeginSpecialRepair : public AbstractDialogOptionAction
{
public:
	//DialogOptionAction_UICommand_BeginSpecialRepair();
	virtual FString GetTypeName() const { return FString(TEXT("BeginSpecialRepair")); }
	virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_BeginSpecialRepair
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_OpenEquipLevelUp : public AbstractDialogOptionAction
{
public:
	//DialogOptionAction_UICommand_OpenEquipLevelUp();
	virtual FString GetTypeName() const { return FString(TEXT("OpenEquipLevelUp")); }
	virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_BeginSpecialRepair
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_LevelUpEquipCurEquipped : public AbstractDialogOptionAction
{
public:
	//DialogOptionAction_UICommand_LevelUpEquipCurEquipped();
	virtual FString GetTypeName() const { return FString(TEXT("LevelUpEquipCurEquipped")); }
	virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_BeginSpecialRepair
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_GetbackCurLevelUpEquip : public AbstractDialogOptionAction
{
public:
	//DialogOptionAction_UICommand_GetbackCurLevelUpEquip();
	virtual FString GetTypeName() const { return FString(TEXT("GetBackCurLevelUpEquip")); }
	virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GetbackCurLevelUpEquipRightNow
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_GetbackCurLevelUpEquipRightNow : public AbstractDialogOptionAction
{
public:
	//DialogOptionAction_UICommand_GetbackCurLevelUpEquipRightNow();
	virtual FString GetTypeName() const { return FString(TEXT("GetBackCurLevelUpEquipRtNow")); }
	virtual void Execute();
};

//Experience forging
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_ApplySpecialExp
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_ApplySpecialExp : public AbstractDialogOptionAction
{
public:
	virtual FString GetTypeName() const { return FString(TEXT("ApplySpecialExp")); }
	virtual void Execute();
};
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_QuerySpecialExp
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_QuerySpecialExp : public AbstractDialogOptionAction
{
public:
	virtual FString GetTypeName() const { return FString(TEXT("QuerySpecialExp")); }
	virtual void Execute();
};
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GetSpecialExp
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_GetSpecialExp : public AbstractDialogOptionAction
{
public:
	virtual FString GetTypeName() const { return FString(TEXT("GetSpecialExp")); }
	virtual void Execute();
};
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GetSpecialExp
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_QueryVSRewardInfo : public AbstractDialogOptionAction
{
public:
	virtual FString GetTypeName() const { return FString(TEXT("QueryVSRewardInfo")); }
	virtual void Execute();
};
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenShop
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_OpenShop : public AbstractDialogOptionAction
{
	INT ShopTemplateID;

public:
	DialogOptionAction_UICommand_OpenShop();
	virtual FString GetTypeName() const { return FString(TEXT("OpenShop")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters=NULL);
};
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_PrepareRemake
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_PrepareRemakeBook : public AbstractDialogOptionAction
{
public:
	virtual FString GetTypeName() const { return FString(TEXT("PrepareRemakeBook")); }
	virtual void Execute();
};

class DialogOptionAction_UICommand_PrepareRemakeMdsn : public AbstractDialogOptionAction
{
public:
	virtual FString GetTypeName() const { return FString(TEXT("PrepareRemakeMdsn")); }
	virtual void Execute();
};
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_QueryVSRewards
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_QueryVSRewards : public AbstractDialogOptionAction
{
public:
	virtual FString GetTypeName() const { return FString(TEXT("QueryVSRewards")); }
	virtual void Execute();
};
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenBlackMarket
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_OpenBlackMarket : public AbstractDialogOptionAction
{
	INT BlackMarketID;

public:
	DialogOptionAction_UICommand_OpenBlackMarket();
	virtual FString GetTypeName() const { return FString(TEXT("BlackMarket")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters=NULL);
};
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenQuest
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_OpenQuest : public AbstractDialogOptionAction
{
	INT QuestTemplateID;
public:
	DialogOptionAction_UICommand_OpenQuest();
	virtual FString GetTypeName() const { return FString(TEXT("OpenQuest")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters=NULL);

	INT GetQuestTemplateID() const { return QuestTemplateID; }
	void SetQuestTemplateID(INT val) { QuestTemplateID = val; }
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GuildSalary
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_GuildSalary : public AbstractDialogOptionAction
{
public:
	DialogOptionAction_UICommand_GuildSalary();
	virtual FString GetTypeName() const { return FString(TEXT("GuildSalary")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters=NULL);
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GuildTerri
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_GuildTerri : public AbstractDialogOptionAction
{
public:
	DialogOptionAction_UICommand_GuildTerri();
	virtual FString GetTypeName() const { return FString(TEXT("GuildTerri")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters=NULL);
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GuildDungeon
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_GuildDungeon : public AbstractDialogOptionAction
{
	INT DungeonId;
public:
	DialogOptionAction_UICommand_GuildDungeon();
	virtual FString GetTypeName() const { return FString(TEXT("GuildDungeon")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters=NULL);

	INT GetDungeonId() const { return DungeonId; }
	void SetDungeonId(INT val) { DungeonId = val; }
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GuildSupport
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_GuildSupport : public AbstractDialogOptionAction
{
public:
	DialogOptionAction_UICommand_GuildSupport();
	virtual FString GetTypeName() const { return FString(TEXT("GuildSupport")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters = NULL);
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_Intruction
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_Instruction : public AbstractDialogOptionAction
{
	FString InstructionKey;

public:
	DialogOptionAction_UICommand_Instruction();
	virtual FString GetTypeName() const { return FString(TEXT("Instruction")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters=NULL);

	void SetInstructionKey(const FString& instructionKey);
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_HireHero
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_HireHero : public AbstractDialogOptionAction
{
	INT ShopTemplateID;

public:
	DialogOptionAction_UICommand_HireHero();
	virtual FString GetTypeName() const { return FString(TEXT("HireHero")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters=NULL);
};

//------------------------------------------------------------------------
// DialogOptionAction_SendServerOption
//------------------------------------------------------------------------
class DialogOptionAction_SendServerOption : public AbstractDialogOptionAction
{
	INT OptionID;

public:
	DialogOptionAction_SendServerOption(INT optionID);
	virtual FString GetTypeName() const { return FString(TEXT("SendServerOption")); }
	virtual void Execute();
	virtual void Parse(const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters=NULL);
};

//------------------------------------------------------------------------
// DialogOptionAction_OpenURL
//------------------------------------------------------------------------
class DialogOptionAction_OpenURL : public AbstractDialogOptionAction
{
public:
	DialogOptionAction_OpenURL();
	virtual FString GetTypeName() const { return FString(TEXT("OpenURL")); }
    virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_OpenQURL
//------------------------------------------------------------------------
class DialogOptionAction_OpenQURL : public AbstractDialogOptionAction
{
    INT nType;

public:
	DialogOptionAction_OpenQURL(INT t);
	virtual FString GetTypeName() const { return FString(TEXT("OpenQURL")); }
    virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_InquirePrisonTerm
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_InquirePrisonTerm : public AbstractDialogOptionAction
{
public:
	//DialogOptionAction_UICommand_InquirePrisonTerm();
	virtual FString GetTypeName() const { return FString(TEXT("InquirePrisonTerm")); }
	virtual void Execute();
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenActivation
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_OpenActivation : public AbstractDialogOptionAction
{

public:
	virtual FString GetTypeName() const { return FString(TEXT("OpenActivation")); }
	virtual void Execute();

	//void ActivationOk(class UGFxObject* mc);
	
};

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenHeroRank
//------------------------------------------------------------------------
class DialogOptionAction_UICommand_OpenHeroRank : public AbstractDialogOptionAction
{

public:
	virtual FString GetTypeName() const { return FString(TEXT("OpenHeroRank")); }
	virtual void Execute();

};

#endif
