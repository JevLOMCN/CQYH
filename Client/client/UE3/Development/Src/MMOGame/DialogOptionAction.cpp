#include "stdafx.h"
#include "MMOFacade.h"
#include "DialogOptionAction.h"
#include "UITextParser.h"
#include "MMOQuestManager.h"
#include "MMOGuildManager.h"
#include "MMOGuideManager.h"
#include "WebHtmlManager.h"
#include "MMOMsgBoxManager.h"

DialogOptionActionMap g_dialogOptionActMap;

bool initDialogActionMap();

static bool g_initDiagActMap = initDialogActionMap();


M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenShop, TEXT("Shop"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GuildSalary, TEXT("GuildSalary"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GuildTerri, TEXT("GuildTerri"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GuildDungeon, TEXT("GuildDungeon"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GuildSupport, TEXT("GuildSupport"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_HireHero, TEXT("HireHero"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenWeaponSkill, TEXT("OpenWeaponSkill"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenWeaponSkillSeal, TEXT("OpenWeaponSkillSeal"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_BeginSpecialRepair, TEXT("BeginSpecialRepair"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenEquipLevelUp, TEXT("OpenEquipLevelUp"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_LevelUpEquipCurEquipped, TEXT("LevelUpEquipCurEquipped"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GetbackCurLevelUpEquip, TEXT("GetBackCurLevelUpEquip"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GetbackCurLevelUpEquipRightNow, TEXT("GetBackCurLevelUpEquipRtNow"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_InquirePrisonTerm, TEXT("InquirePrisonTerm"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenActivation, TEXT("OpenActivation"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_ApplySpecialExp, TEXT("ApplySpecialExp"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_QuerySpecialExp, TEXT("QuerySpecialExp"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GetSpecialExp, TEXT("GetSpecialExp"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenBlackMarket, TEXT("BlackMarket"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenHeroRank, TEXT("OpenHeroRank"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_QueryVSRewardInfo, TEXT("QueryVSRewardInfo"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_PrepareRemakeBook, TEXT("PrepareRemakeBook"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_PrepareRemakeMdsn, TEXT("PrepareRemakeMdsn"));
M_DECL_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_QueryVSRewards, TEXT("QueryVSRewards"));

bool initDialogActionMap()
{
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenShop, TEXT("Shop"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GuildSalary, TEXT("GuildSalary"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GuildTerri, TEXT("GuildTerri"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GuildDungeon, TEXT("GuildDungeon"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GuildSupport, TEXT("GuildSupport"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_HireHero, TEXT("HireHero"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenWeaponSkill, TEXT("OpenWeaponSkill"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenWeaponSkillSeal, TEXT("OpenWeaponSkillSeal"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_BeginSpecialRepair, TEXT("BeginSpecialRepair"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenEquipLevelUp, TEXT("OpenEquipLevelUp"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_LevelUpEquipCurEquipped, TEXT("LevelUpEquipCurEquipped"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GetbackCurLevelUpEquip, TEXT("GetBackCurLevelUpEquip"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GetbackCurLevelUpEquipRightNow, TEXT("GetBackCurLevelUpEquipRtNow"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_InquirePrisonTerm, TEXT("InquirePrisonTerm"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenActivation, TEXT("OpenActivation"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_ApplySpecialExp, TEXT("ApplySpecialExp"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_QuerySpecialExp, TEXT("QuerySpecailExp"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_GetSpecialExp, TEXT("GetSpecialExp"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenBlackMarket, TEXT("BlackMarket"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_OpenHeroRank, TEXT("OpenHeroRank"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_QueryVSRewardInfo, TEXT("QueryVSRewardInfo"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_PrepareRemakeBook, TEXT("PrepareRemakeBook"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_PrepareRemakeMdsn, TEXT("PrepareRemakeMdsn"));
	M_REG_DIAG_OPT_ACT_FUN(DialogOptionAction_UICommand_QueryVSRewards, TEXT("QueryVSRewards"));

	return true;
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenPanel
//------------------------------------------------------------------------
DialogOptionAction_UICommand_OpenPanel::DialogOptionAction_UICommand_OpenPanel(const FName& PanelName)
	: UIPanelName(PanelName)
{
}


void DialogOptionAction_UICommand_OpenPanel::Execute()
{
	if(UIPanelName != NAME_None)
	{
		MMOUI::GetUIManager()->ShowPanelEx(UIPanelName, TRUE, "");
		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
			if (dialog)
			{
				dialog->CloseDialog();
			}
		}
	}
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenWeaponSkill
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_OpenWeaponSkill::Execute()
{
	INT curPlayerLv = MMOFacade::getSingleton().GetMyLevel();
	if( curPlayerLv < 25 ){
		//Level insufficient, do not show
		FString hintName = TEXT("MMOGame.WeaponSkill.LevelLimitedHint");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( hintName ), MsgInfoType_Table, hintName );

		return;
	}

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_WeaponSkill, TRUE, TEXT("ChangeToTab0"));
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenWeaponSkillSeal
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_OpenWeaponSkillSeal::Execute()
{
	//UMMOUIWeaponSkill *pWeaponSkillPanel = (UMMOUIWeaponSkill*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_WeaponSkill );
	//MMOUI::GetUIManager()->FindUIPanelEx(MMOUI::GetUIManager()->Panel_WeaponSkill, TRUE, "");
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_WeaponSkill, TRUE, TEXT("ChangeToTab1"));
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
}
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_PrepareRemake
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_PrepareRemakeBook::Execute()
{
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_RemakeFactory, TRUE, TEXT("ChangeToTab1"));
	UMMOUIRemakeFactory *pRemakeFactory = (UMMOUIRemakeFactory*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_RemakeFactory );
	if( pRemakeFactory != NULL ){
		pRemakeFactory->ChangeMode( EReMkTp_Book, FALSE );

		if( !pRemakeFactory->IsVisible ){
			pRemakeFactory->Show( TRUE );
		}
	}
	//
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
}
void DialogOptionAction_UICommand_PrepareRemakeMdsn::Execute()
{
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_RemakeFactory, TRUE, TEXT("ChangeToTab1"));
	UMMOUIRemakeFactory *pRemakeFactory = (UMMOUIRemakeFactory*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_RemakeFactory );
	if( pRemakeFactory != NULL ){
		pRemakeFactory->ChangeMode( EReMkTp_Medicine, FALSE );

		if( !pRemakeFactory->IsVisible ){
			pRemakeFactory->Show( TRUE );
		}
	}
	//
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
}
////------------------------------------------------------------------------
//// DialogOptionAction_UICommand_ApplyForVS
////------------------------------------------------------------------------
//void DialogOptionAction_UICommand_ApplyForVS::Execute()
//{
//	CMessageHandler::GetInstance().Send_ApplyForVS();
//	//
//	UMMOUIManager* uiManager = MMOUI::GetUIManager();
//	if (uiManager)
//	{
//		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanel(uiManager->Panel_DialogNew);
//		if (dialog)
//		{
//			dialog->CloseDialog();
//		}
//	}
//}
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_QueryVSRewards
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_QueryVSRewards::Execute()
{
	CMessageHandler::GetInstance().Send_QueryVSRewardInfo();
	//
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanel(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
}
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_BeginSpecialRepair
//------------------------------------------------------------------------
//DialogOptionAction_UICommand_BeginSpecialRepair::DialogOptionAction_UICommand_BeginSpecialRepair()
//{	
//}

void DialogOptionAction_UICommand_BeginSpecialRepair::Execute()
{
	UMMOUIBag *pBagPanel = (UMMOUIBag*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_Bag );
	if( pBagPanel != NULL ){
		if( !pBagPanel->IsVisible ){
			pBagPanel->Show( TRUE );
		}
		pBagPanel->eventClickSpecRepair();
	}
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenEquipLevelUp
//------------------------------------------------------------------------
//DialogOptionAction_UICommand_OpenEquipLevelUp::DialogOptionAction_UICommand_OpenEquipLevelUp()
//{	
//}

void DialogOptionAction_UICommand_OpenEquipLevelUp::Execute()
{
	INT curPlayerLv = MMOFacade::getSingleton().GetMyLevel();
	if( curPlayerLv < 12 ){
		//Level insufficient, do not show
		FString hintName = TEXT("MMOGame.EquipLevelUp.LevelLimitedHint");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( hintName ), MsgInfoType_Table, hintName );

		return;
	}

	UMMOUIEquipLevelUp *pEquipLvUp = (UMMOUIEquipLevelUp*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_EquipLevelUp );
	if( pEquipLvUp != NULL ){
		if( !pEquipLvUp->IsVisible ){
			pEquipLvUp->Show( TRUE );
		}
	}
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_LevelUpEquipCurEquipped
//------------------------------------------------------------------------
//DialogOptionAction_UICommand_LevelUpEquipCurEquipped::DialogOptionAction_UICommand_LevelUpEquipCurEquipped()
//{	
//}

void DialogOptionAction_UICommand_LevelUpEquipCurEquipped::Execute()
{
	//INT curPlayerLv = MMOFacade::getSingleton().GetMyLevel();
	//if( curPlayerLv < 12 ){
	//	//Level insufficient, do not show
	//	FString hintName = TEXT("MMOGame.EquipLevelUp.LevelLimitedHint");
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( hintName ), MsgInfoType_Table, hintName );

	//	return;
	//}

	UMMOUIDialogNew *pDialogPanel = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DialogNew );
	if( pDialogPanel != NULL ){
		pDialogPanel->ConfirmLeveUpWeaponCurrentEquipped();
	}
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GetbackCurLevelUpEquip
//------------------------------------------------------------------------
//DialogOptionAction_UICommand_GetbackCurLevelUpEquip::DialogOptionAction_UICommand_GetbackCurLevelUpEquip()
//{	
//}

void DialogOptionAction_UICommand_GetbackCurLevelUpEquip::Execute()
{
	CMessageHandler::GetInstance().Send_EquipLevelUpShortcut_TakeOut();
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GetbackCurLevelUpEquip
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_GetbackCurLevelUpEquipRightNow::Execute()
{
	//CMessageHandler::GetInstance().Send_EquipLevelUpRightNow_TakeOut();
	UMMOUIDialogNew *pDialogPanel = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DialogNew );
	if( pDialogPanel != NULL ){
		pDialogPanel->ConfirmTakeBackCurLeveUpWeaponRtNow();
	}
}

//Offline experience
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_ApplySpecialExp
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_ApplySpecialExp::Execute()
{
	UMMOUISpecialExpApply* panel = (UMMOUISpecialExpApply*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_SpecialExpApply );
	if( panel != NULL ){
		panel->Show( TRUE );
	}
}
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_QuerySpecialExp
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_QuerySpecialExp::Execute()
{
	//CMessageHandler::GetInstance().Send_QuerySpecialExp();
}
//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GetSpecialExp
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_GetSpecialExp::Execute()
{
	CMessageHandler::GetInstance().Send_GetCurAllSpecialExpInfo();
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenShop
//------------------------------------------------------------------------
DialogOptionAction_UICommand_OpenShop::DialogOptionAction_UICommand_OpenShop()
	: ShopTemplateID(-1)
{	
}

void DialogOptionAction_UICommand_OpenShop::Execute()
{
	// Tutorial event
	MMOGameEvent_Guide evtGuide(EGUET_OpenShopPanel);
	MMOUI::PostGameEvent(&evtGuide);

	MMOUI::GetUIManager()->ShowNpcShop(ShopTemplateID);

	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Shop, TRUE);
	//MMOUI::GetUIManager()->delegateOpenShop(ShopTemplateID);
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
}

void DialogOptionAction_UICommand_OpenShop::Parse(const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters)
{
	AbstractDialogOptionAction::Parse(DisplayText, ctx,pParameters);

	if(ctx)
	{
		ShopTemplateID = ctx->NPC && ctx->NPC->Template ? ctx->NPC->Template->ShopID : -1;
	}
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenBlackMarket
//------------------------------------------------------------------------
DialogOptionAction_UICommand_OpenBlackMarket::DialogOptionAction_UICommand_OpenBlackMarket()
	: BlackMarketID(-1)
{	
}

void DialogOptionAction_UICommand_OpenBlackMarket::Execute()
{

	MMOUI::GetUIManager()->ShowBlackMarket(BlackMarketID);

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
}

void DialogOptionAction_UICommand_OpenBlackMarket::Parse(const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters)
{
	AbstractDialogOptionAction::Parse(DisplayText, ctx,pParameters);

	if(ctx)
	{
		BlackMarketID = ctx->NPC && ctx->NPC->Template ? ctx->NPC->Template->BlackMarketID : -1;
	}
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenQuest
//------------------------------------------------------------------------
DialogOptionAction_UICommand_OpenQuest::DialogOptionAction_UICommand_OpenQuest()
	: QuestTemplateID(-1)
{	
}

void DialogOptionAction_UICommand_OpenQuest::Execute()
{
	if (MMOQuestData::getSingleton().IsQuestRecycle(QuestTemplateID))
	{
		// Open the recycle panel
		UMMOUIQuestItemCollectionPanel* panel = (UMMOUIQuestItemCollectionPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_QuestItemCollection);
		if (panel)
		{
			panel->ShowItemCollectionPanel(QuestTemplateID);
		}
		return;
	}

	UBOOL HasAccept = MMOQuestData::getSingleton().IsQuestAccepted(QuestTemplateID);

	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->OpenQuestPopupUp(!HasAccept, QuestTemplateID, FALSE, 0);
		}
	}
}

void DialogOptionAction_UICommand_OpenQuest::Parse( const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters )
{
	AbstractDialogOptionAction::Parse(DisplayText, ctx,pParameters);	
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GuildSalary
//------------------------------------------------------------------------
DialogOptionAction_UICommand_GuildSalary::DialogOptionAction_UICommand_GuildSalary()
{	
}

void DialogOptionAction_UICommand_GuildSalary::Execute()
{
	CMessageHandler::GetInstance().Send_DrawSalary();
}

void DialogOptionAction_UICommand_GuildSalary::Parse(const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters)
{
	AbstractDialogOptionAction::Parse(DisplayText, ctx,pParameters);
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GuildTerri
//------------------------------------------------------------------------
DialogOptionAction_UICommand_GuildTerri::DialogOptionAction_UICommand_GuildTerri()
{	
}

void DialogOptionAction_UICommand_GuildTerri::Execute()
{
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	if (baseInfo.Id <= 0)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GuildWarehouse.NoGuild"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.GuildWarehouse.NoGuild"));
		return;
	}

	CMessageHandler::GetInstance().Send_EnterGuildMap(GUILD_ENTER_OWNER, baseInfo.Id);
}

void DialogOptionAction_UICommand_GuildTerri::Parse(const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters)
{
	AbstractDialogOptionAction::Parse(DisplayText, ctx,pParameters);
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GuildDungeon
//------------------------------------------------------------------------
DialogOptionAction_UICommand_GuildDungeon::DialogOptionAction_UICommand_GuildDungeon()
	: DungeonId(-1)
{	
}

void DialogOptionAction_UICommand_GuildDungeon::Execute()
{
	if (DungeonId != -1)
	{
		UMMOUIGuildDungeonEnterPanel* panel = (UMMOUIGuildDungeonEnterPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_DungeonEnter);
		if (panel)
		{
			panel->ShowGuildDungeonEnterPanel(DungeonId);
		}
	}
}

void DialogOptionAction_UICommand_GuildDungeon::Parse(const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters)
{
	AbstractDialogOptionAction::Parse(DisplayText, ctx, pParameters);
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_GuildSupport
//------------------------------------------------------------------------
DialogOptionAction_UICommand_GuildSupport::DialogOptionAction_UICommand_GuildSupport()
{
}

void DialogOptionAction_UICommand_GuildSupport::Execute()
{
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	if (baseInfo.Id <= 0)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GuildWarehouse.NoGuild"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.GuildWarehouse.NoGuild"));
		return;
	}

	UMMOUIGuildSupportPanel* panel = (UMMOUIGuildSupportPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Support);
	if (panel)
	{
		panel->ShowGuildSupportPanel();
	}
}

void DialogOptionAction_UICommand_GuildSupport::Parse(const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters)
{
	AbstractDialogOptionAction::Parse(DisplayText, ctx, pParameters);
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_Intruction
//------------------------------------------------------------------------
DialogOptionAction_UICommand_Instruction::DialogOptionAction_UICommand_Instruction()
	: InstructionKey(TEXT(""))
{	
}

void DialogOptionAction_UICommand_Instruction::Execute()
{
	if (InstructionKey != TEXT(""))
	{
		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			UMMOUIGuildInstructionPanel* instruPanel = (UMMOUIGuildInstructionPanel*)MMOUI::FindUIPanelEx(uiManager->Panel_Guild_Instruction);
			if (instruPanel)
			{
				instruPanel->SetInstruction(InstructionKey);
				uiManager->ShowPanelEx(uiManager->Panel_Guild_Instruction, TRUE, "");
			}
		}
	}
}

void DialogOptionAction_UICommand_Instruction::Parse(const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters)
{
	AbstractDialogOptionAction::Parse(DisplayText, ctx, pParameters);
}

void DialogOptionAction_UICommand_Instruction::SetInstructionKey(const FString& instructionKey)
{
	InstructionKey = instructionKey;
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_HireHero
//------------------------------------------------------------------------
DialogOptionAction_UICommand_HireHero::DialogOptionAction_UICommand_HireHero():ShopTemplateID(-1)
{	
}

void DialogOptionAction_UICommand_HireHero::Execute()
{
	INT Index = appAtoi(*OptionParameters);
	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(Player)
	{
		if(Player->ServerHeroID<=0)
		{
			CMessageHandler::GetInstance().Send_HeroHire((Index-1)/2, (Index-1)%2+1);
		}
		else
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Hero.MessageHireHeroFail1")),MsgInfoType_Table,TEXT("MMOGame.Hero.MessageHireHeroFail1") );
		}
	}
}

void DialogOptionAction_UICommand_HireHero::Parse(const FString& DisplayText, struct ParseContext* ctx, const FString* pParameters)
{
	AbstractDialogOptionAction::Parse(DisplayText, ctx,pParameters);
}

AbstractDialogOptionAction::AbstractDialogOptionAction()
{

}

AbstractDialogOptionAction::~AbstractDialogOptionAction()
{
}

void DialogOptionAction_SendServerOption::Execute()
{
	// Tutorial event
	MMOGameEvent_Guide evtGuide(EGUET_NpcDialogServerOption);
	MMOUI::PostGameEvent(&evtGuide);

	CMessageHandler::GetInstance().Send_NpcDialogOption(OptionID);
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (uiManager)
	{
		UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		if (dialog)
		{
			dialog->CloseDialog();
		}
	}
}

void DialogOptionAction_SendServerOption::Parse( const FString& DisplayText, struct ParseContext* ctx,const FString* pParameters/*=NULL*/ )
{
	//AbstractDialogOptionAction::Parse(DisplayText, ctx,pParameters);
	DialogContext tmpCtx;
	ParseContext parseCtx;
	FGuardSkipAppendNPCDialogOption gardCtx(ctx, true);
	UITextParser::getSingleton().Parse(parseCtx, tmpCtx, DisplayText);
	OptionDisplayText = *tmpCtx.TempText;
}

DialogOptionAction_SendServerOption::DialogOptionAction_SendServerOption( INT optionID )
	: OptionID(optionID)
{
}

DialogOptionAction_OpenURL::DialogOptionAction_OpenURL()
{
}

void DialogOptionAction_OpenURL::Execute()
{
    if (!OptionParameters.IsEmpty())
		WebHtmlManager::getSingleton().OpenUrl(*OptionParameters);
       // appLaunchURL(*OptionParameters);
}

DialogOptionAction_OpenQURL::DialogOptionAction_OpenQURL(INT t)
    :nType(t)
{
}

unsigned char QURL_ToHex(unsigned char x) 
{ 
    return  x > 9 ? x + 55 : x + 48; 
}
std::string QURL_UrlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) || 
            (str[i] == '-') ||
            (str[i] == '_') || 
            (str[i] == '.') || 
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += QURL_ToHex((unsigned char)str[i] >> 4);
            strTemp += QURL_ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}

void DialogOptionAction_OpenQURL::Execute()
{
    switch (nType)
    {
    case 1:
        {
            /*TCHAR ipFile[1024];
            appSprintf(ipFile, TEXT("%sIP.ini"), *appGameConfigDir());
            FString url_cfg;
            if (!GConfig->GetString(TEXT("QURL"), TEXT("qsurvey"), url_cfg, ipFile))
                return;
            FTCHARToANSI name_cov(*FString(MMOFacade::getSingleton().GetMainPlayer()->ActorName));
            std::string name_ansi = (ANSICHAR*)name_cov;
            std::string name_safe = QURL_UrlEncode(name_ansi);
            FANSIToTCHAR name_tchar(name_safe.c_str());

            url_cfg += _T("&ptid=");
            url_cfg += CMessageHandler::GetInstance()._UserName;

            url_cfg += _T("&rolename=");
            url_cfg += name_tchar;

            //appLaunchURL(*url_cfg);

			WebHtmlManager::getSingleton().OpenUrl(url_cfg);*/
			WebHtmlManager::getSingleton().OpenSurveyUrl();
        }
    	break;
	case 2:
		{
			WebHtmlManager::getSingleton().OpenBugSubmitUrl();
			break;
		}
    }
}

//------------------------------------------------------------------------
// DialogOptionAction_UICommand_InquirePrisonTerm
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_InquirePrisonTerm::Execute()
{
	//CMessageHandler::GetInstance().Send_InquirePrisonTerm();
}
//===============================================================================================================


//------------------------------------------------------------------------
// DialogOptionAction_UICommand_OpenActivation
//------------------------------------------------------------------------

void DialogOptionAction_UICommand_OpenActivation::Execute()
{
	
	MMOGameEvent_Any evt( GameEvent_ShowActivation );
	MMOUI::PostGameEvent(&evt);
}

void DialogOptionAction_UICommand_OpenHeroRank::Execute()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_HeroRank, TRUE, "");
}


//------------------------------------------------------------------------
// DialogOptionAction_UICommand_QueryVSRewardInfo
//------------------------------------------------------------------------
void DialogOptionAction_UICommand_QueryVSRewardInfo::Execute()
{
	CMessageHandler::GetInstance().Send_QueryVSRewardInfo();
}
