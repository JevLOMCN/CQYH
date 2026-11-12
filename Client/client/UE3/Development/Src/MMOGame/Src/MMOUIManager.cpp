#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "UILayout.h"
#include "MMOMsgBoxManager.h"
#include "MMOShopManager.h"
#include "FSMIO.h"
#include "MMOClientVariableManager.h"
#include "MMOMapDataManager.h"
#include "EngineAudioDeviceClasses.h"
#include "FSMState.h"

IMPLEMENT_CLASS(UMMOUIManager)

	typedef std::list<FString> PanelList;
extern PanelList g_PanelList;

UMMOGFXPlayer_RootBase* g_CommonMoviePlayer = 0;
extern UMMOGFXPlayer_RootBase* g_rootGFXPlayer;


void UMMOUIManager::RegisterMMOActorHUD(const TScriptInterface<class IIMMOActorHUD>& hud)
{
	UILevel.AddItem(hud);
}

void UMMOUIManager::UnRegisterMMOActorHUD(const TScriptInterface<class IIMMOActorHUD>& hud)
{
	UILevel.RemoveItem(hud);
}

void UMMOUIManager::RefreshSkillBarByServerData(INT32* addr, INT  size)
{
	UMMOUISkillBar* UI = (UMMOUISkillBar*)FindUIPanel(Panel_Skillbar);
	if(UI)
	{
		UI->RefreshSkillBarByServerData(addr, size);
		////Newbie default item bar
		//AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		//if( mainPlayer != NULL && mainPlayer->ActorAttributes->LevelAttributes.Level < 10 && mainPlayer->ActorAttributes->LevelAttributes.CurrentExp < 10 ){//leave some margin for 10
		//	UI->InitItemBarItemForNewComing();
		//}
	}
}

//void UMMOUIManager::RefreshSkillBarSkillData(BYTE* addr, INT size)
//{
//	UMMOUISkillBar* UI = (UMMOUISkillBar*)FindUIPanel(Panel_Skillbar);
//	if(UI)
//	{
//		UI->ReflashSkillSlotRedirection(addr, size);
//	}
//}

void UMMOUIManager::SetTraceRelationForceID(INT ForceID)
{
	TraceRelationForceID = ForceID;
	//Refresh display
	MMOGameEvent_Any evt( GameEvent_UpdateTraceRelation );
	MMOUI::PostGameEvent(&evt);
}

INT UMMOUIManager::GetTraceRelationForceID()
{
	return TraceRelationForceID;
}

void UMMOUIManager::SetLastPlayerName(FName LastPlayerName)
{
	LastDelayLoadPlayerName = LastPlayerName;
}

void UMMOUIManager::SetLastPlayerNameByPanelName(FName PanelName)
{
	FName PlayerName = MMOUI::FindMoviePlayerName(PanelName);
	UMMOGFXPlayer* Player = MMOUI::FindMoviePlayer(PlayerName);
	UMMOUIBase* UI = FindUIPanel(PanelName);

	if(UI && Player && UI->IsDynamicLoad && Player->IsAllHide() && Player->IsLockedDynamicLoad==FALSE )
	{
		SetLastPlayerName(PlayerName);
	}
}

void UMMOUIManager::CloseLastPlayer(FName CurPlayerName)
{
	//Delete the temporarily cached player
	if(LastDelayLoadPlayerName.GetNameString().Len()>0)
	{
		if(LastDelayLoadPlayerName!=CurPlayerName)
		{
			MMOUI::CloseMoviePlayersByName(LastDelayLoadPlayerName);	
		}
		LastDelayLoadPlayerName = TEXT("");
	}
}

void UMMOUIManager::CloseLastPlayerByPanelName(FName PanelName)
{
	FName PlayerName = MMOUI::FindMoviePlayerName(PanelName);
	CloseLastPlayer(PlayerName);
}
//------------------------------------------------------------------------
void UMMOUIManager::AddGameState(class UMMOGameState* gs)
{
	MMO::AddGameState(gs);
}

BYTE UMMOUIManager::GetCurrentGameState()
{
	return MMO::GetCurrentGameState();	
}

void UMMOUIManager::ActiveGameState(BYTE gs)
{
	MMO::ActiveGameState((EGameState)gs);
}

class UMMOGameState* UMMOUIManager::FindGameStateObject(BYTE gs)
{
	return MMO::FindGameStateObject((EGameState)gs);
}
//------------------------------------------------------------------------
class UItem* UMMOUIManager::FindItemTemplate(BYTE ContainerID,INT SlotID)
{
	return MMOFacade::getSingleton().FindItemTemplate(ContainerID, SlotID);
}
//------------------------------------------------------------------------
class UItem* UMMOUIManager::FindItemTemplateByID(INT ItemTemplateID)
{
	return MMOFacade::getSingleton().FindItemTemplate(ItemTemplateID);
}
//-----------------------canvas
void UMMOUIManager::SetSceneView(class UCanvas* cvs)
{
	if(cvs && !GIsEditor)
	{
		MMOFacade::getSingleton().SetCanvas(cvs);
	}	
}

//------------------------------------------------------------------------
void UMMOUIManager::ShowMouseCursor(UBOOL ShowMouse)
{
	MMOFacade::ShowMouseCursor(ShowMouse);
}

void UMMOUIManager::SetMouseCursor(BYTE cursorType)
{
	MMOFacade::SetMouseCursor((EMMOCursorType)cursorType);
}

//------------------------------------------------------------------------
void UMMOUIManager::SetCommonUIComponentMoviePlayer(class UMMOGFXPlayer_RootBase* inMoviePlayer)
{
	g_CommonMoviePlayer = inMoviePlayer;
}

class UMMOGFXPlayer_RootBase* UMMOUIManager::GetCommonUIComponentMoviePlayer()
{
	return g_CommonMoviePlayer;
}
//------------------------------------------------------------------------
class UMMOGFXPlayer* UMMOUIManager::FindMoviePlayer(FName PanelName)
{
	return MMOUI::FindMoviePlayer(PanelName);
}

class UMMOUIBase* UMMOUIManager::FindUIPanel(FName PanelName)
{
	return MMOUI::FindUIPanel(PanelName);
}

class UMMOGFXPlayer* UMMOUIManager::LoadMoviePlayers(FName PlayerName)
{
	UMMOGFXPlayer* Player = MMOUI::FindMoviePlayer(PlayerName);
	if(Player==NULL){
		AMMOHud* HUD = MMOFacade::getSingleton().GetMyHud();
		if(HUD){
			//CloseLastPlayer(PlayerName);
			FName PlayerClassName = FName(*FString::Printf(TEXT("%s%s"),TEXT("MMOGame.MMOGFXPlayer_"), *(PlayerName.GetNameString()) ));
			MMOUI::LoadMoviePlayers(HUD,PlayerClassName);
			Player = MMOUI::FindMoviePlayer(PlayerName);
		}
	}
	return Player;
}

UMMOUIBase* UMMOUIManager::ShowPanel(FName PanelName,UBOOL Show,const FString& TabName)
{
	UMMOUIBase* UI = FindUIPanel(PanelName);
	if(UI)
	{
		UI->Show(Show);
		if (Show && UI->CanChangeTabOnPostShow && !TabName.IsEmpty())
		{
			TArray<struct FASValue> vArgs;
			UI->Invoke(TabName, vArgs);
		}
	}
	return UI;
}

extern UBOOL IsViewLoaded(FName panelName);
extern UMMOGFXPlayer_RootBase* FindViewLoader(FName panelName);

UMMOUIBase* UMMOUIManager::ShowPanelEx(FName PanelName,UBOOL Show,const FString& TabName, INT displayPriority)
{
	FName PlayerName = MMOUI::FindMoviePlayerName(PanelName);
	UMMOGFXPlayer* Player = MMOUI::FindMoviePlayer(PlayerName);

	if(Show){
		if( Player )
		{
			UMMOUIBase* UI = FindUIPanel(PanelName);
			if(UI)
			{
				UI->Show(TRUE, displayPriority);
				//CloseLastPlayer(PlayerName);
				if (UI->CanChangeTabOnPostShow && !TabName.IsEmpty())
				{
					TArray<struct FASValue> vArgs;
					UI->Invoke(TabName, vArgs);
				}
			}
			return UI;
		}
		else
		{
			AMMOHud* HUD = MMOFacade::getSingleton().GetMyHud();
			if(HUD)
			{
				//CloseLastPlayer(PlayerName);
				UMMOGFXPlayer_RootBase* loadedPlayer = FindViewLoader(PanelName);
				if(!IsViewLoaded(PanelName) && !(loadedPlayer && loadedPlayer->LoadViewByName(PanelName)))
				{
					FName PlayerClassName = FName(*FString::Printf(TEXT("%s%s"),TEXT("MMOGame.MMOGFXPlayer_"), *(PlayerName.GetNameString()) ));
					MMOUI::LoadMoviePlayers(HUD,PlayerClassName);
				}

				UMMOUIBase* UI = FindUIPanel(PanelName);
				if(UI)
				{
					UI->Show(TRUE, displayPriority);
					if (UI->CanChangeTabOnPostShow && !TabName.IsEmpty())
					{
						TArray<struct FASValue> vArgs;
						UI->Invoke(TabName, vArgs);
					}
				}

				return UI;
			}
		}
	}
	else
	{
		if( Player )
		{
			UMMOUIBase* UI = FindUIPanel(PanelName);
			if(UI)
			{
				Player->ResetDragState();
				UI->Show(FALSE, displayPriority);
				UI->LastVisible = FALSE;
				//if(UI->IsDynamicLoad && Player->IsAllHide() && Player->IsLockedDynamicLoad==FALSE)
				//{
				//	SetLastPlayerName(PlayerName);
				//}

				//Set VIP portable shop flag
				UI->SetShowWithOutTarget(FALSE);
			}

			return UI;
		}
	}
	return 0;
}

UMMOUIBase* UMMOUIManager::ShowPanelByDefaultVisible( FName panelName,const FString& TabName )
{
	UMMOUIBase* UI = FindUIPanel( panelName );
	if( NULL != UI )
	{
		UI->Show( UI->DefaultVisible );
		if (UI->DefaultVisible && !TabName.IsEmpty())
		{
			TArray<struct FASValue> vArgs;
			UI->Invoke(TabName, vArgs);
		}
	}
	return UI;
}


void UMMOUIManager::OnLeftShift(UBOOL bRelease)
{
	delegateOnShift(bRelease);
}

UBOOL UMMOUIManager::OnEsc()
{
	AMMOPlayerController* Controller =MMOFacade::getSingleton().GetMainController();
	if(Controller==NULL)
	{
		return FALSE;
	}
	if(Controller->Pawn==NULL)
	{
		return FALSE;
	}


	if(MMO::GetCurrentGameState() != EGameState_InGame)
	{
		UBOOL bSkip = Controller->StopActiveMatinee();
		if (bSkip)
		{
			return TRUE;
		}

		return FALSE;
	}
	else
	{
		PanelList::reverse_iterator iter = g_PanelList.rbegin();
		for (; iter != g_PanelList.rend(); ++iter)
		{
			UMMOUIBase* UI = MMOUI::GetUIManager()->FindUIPanel(FName(**iter));
			{
				if(UI && UI->OnEsc())
				{
					if(g_rootGFXPlayer)
					{
						g_rootGFXPlayer->eventUCClearFocus();
					}
					return TRUE;
				}
			}
		}

		UBOOL bSkip = Controller->StopActiveMatinee();
		if (bSkip)
		{
			return TRUE;
		}


		//Controller->eventShowEscMenu();
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionMenu, TRUE, TEXT(""));

		return TRUE;
	}



	return FALSE;
}

UMMOUIBase* UMMOUIManager::ToggleShowPanel(FName PanelName,const FString& TabName)
{
	UMMOUIBase* UI = FindUIPanel(PanelName);
	if(UI)
	{		
		UI->ToogleVisible();
		if (UI->GetVisible() && UI->CanChangeTabOnPostShow && !TabName.IsEmpty())
		{
			TArray<struct FASValue> vArgs;
			UI->Invoke(TabName, vArgs);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("UMMOUIManager::ToggleShowPanel cannot find this UI: %s"), *PanelName.ToString());
	}

	return UI;
}

UMMOUIBase* UMMOUIManager::ToggleShowPanelEx(FName PanelName,const FString& TabName)
{
#if 1
	UMMOUIBase* UI = FindUIPanel(PanelName);
	if(!UI)
	{
		if(!g_rootGFXPlayer->LoadViewByName(PanelName))
		{
			FName PlayerName = MMOUI::FindMoviePlayerName(PanelName);

			AMMOHud* HUD = MMOFacade::getSingleton().GetMyHud();
			if(HUD)
			{
				//CloseLastPlayer(PlayerName);
				FName PlayerClassName = FName(*FString::Printf(TEXT("%s%s"),TEXT("MMOGame.MMOGFXPlayer_"), *(PlayerName.GetNameString()) ));
				MMOUI::LoadMoviePlayers(HUD,PlayerClassName);
			}
		}
	}

	UI = FindUIPanel(PanelName);
	if(UI)
	{
		if(UI->GetVisible())
		{
			UI->Show(FALSE);
			//Player->ResetDragState();
		}
		else
		{
			UI->Show(TRUE);

			if (UI->CanChangeTabOnPostShow && !TabName.IsEmpty())
			{
				TArray<struct FASValue> vArgs;
				UI->Invoke(TabName, vArgs);
			}
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("UMMOUIManager::ToggleShowPanelEx cannot find this UI: %s"), *PanelName.ToString());
	}

	return UI;

#else
	FName PlayerName = MMOUI::FindMoviePlayerName(PanelName);
	UMMOGFXPlayer* Player = MMOUI::FindMoviePlayer(PlayerName);
	if( Player)
	{
		UMMOUIBase* UI = FindUIPanel(PanelName);
		if(UI)
		{
			if(UI->GetVisible()){
				UI->Show(FALSE);
				Player->ResetDragState();
				//if(UI->IsDynamicLoad && Player->IsAllHide() && Player->IsLockedDynamicLoad==FALSE )
				//{
				//	SetLastPlayerName(PlayerName);
				//}
			}
			else
			{
				UI->Show(TRUE);
				//CloseLastPlayer(PlayerName);
				if (UI->CanChangeTabOnPostShow && !TabName.IsEmpty())
				{
					TArray<struct FASValue> vArgs;
					UI->Invoke(TabName, vArgs);
				}
			}
		}
		return UI;
	}
	else
	{
		AMMOHud* HUD = MMOFacade::getSingleton().GetMyHud();
		if(HUD)
		{
			//CloseLastPlayer(PlayerName);
			FName PlayerClassName = FName(*FString::Printf(TEXT("%s%s"),TEXT("MMOGame.MMOGFXPlayer_"), *(PlayerName.GetNameString()) ));
			MMOUI::LoadMoviePlayers(HUD,PlayerClassName);

			UMMOUIBase* UI = FindUIPanel(PanelName);
			if(UI)
			{
				UI->Show(TRUE);
				if (UI->CanChangeTabOnPostShow && !TabName.IsEmpty())
				{
					TArray<struct FASValue> vArgs;
					UI->Invoke(TabName, vArgs);
				}
			}	
			return UI;
		}
	}
	return 0;
#endif
}

void UMMOUIManager::ShowPatternPanel(class UMMOUIBase* RefPanel, class UMMOUIBase* PatternPanel)
{
	MMOUI::ShowPatternPanel(RefPanel, PatternPanel);
}

void UMMOUIManager::ShowAllPanels(UBOOL Show, UBOOL bRecover)
{
	MMOUI::ShowAllPanels(Show,bRecover);
}

//battle state
void UMMOUIManager::SetShowEnterBattleDelegate(FScriptDelegate fun)
{
	__ShowEnterBattle__Delegate = fun;
}

//
void UMMOUIManager::SetShowCombatMsgDelegate(FScriptDelegate fun)
{
	__ShowCombatMsg__Delegate = fun;
}

void UMMOUIManager::SetShowTipsHTMLDelegate(FScriptDelegate fun)
{
	__ShowTipsHTML__Delegate = fun;
}

void UMMOUIManager::SetToggleTipAidDelegate(FScriptDelegate fun)
{
	__ToggleTipAid__Delegate = fun;
}

void UMMOUIManager::SetIsTipsShowDelegate(FScriptDelegate fun)
{
	__IsTipsShow__Delegate = fun;
}


void UMMOUIManager::SetAnchorTipsDelegate(FScriptDelegate fun)
{
	__AnchorTips__Delegate = fun;
}

void UMMOUIManager::SetShowFreeStyleTipsDelegate(FScriptDelegate fun)
{
	__ShowFreeStyleTips__Delegate = fun;
}

void UMMOUIManager::SetShowTextTipsDelegate(FScriptDelegate fun)
{
	__ShowTextTips__Delegate = fun;
}

void UMMOUIManager::SetShowTipsDelegate(FScriptDelegate fun)
{
	__ShowTips__Delegate = fun;
}

void UMMOUIManager::SetUpdateBagCapSlotDelegate(FScriptDelegate fun)
{
	__UpdateBagCapSlot__Delegate = fun;
}

void UMMOUIManager::SetHintNewItemLabelDelegate(FScriptDelegate fun)
{
	__HintNewItemLabel__Delegate = fun;
}

void UMMOUIManager::SetHideTipsDelegate(FScriptDelegate fun)
{
	__HideTips__Delegate = fun;
}

void UMMOUIManager::SetShowLinkTipsDelegate(FScriptDelegate fun)
{
	__ShowLinkTips__Delegate = fun;
}

void UMMOUIManager::ShowItemLinkTips( UGFxObject *fromObject )
{
	UMMOUITips *tipsPanel = (UMMOUITips*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_Tips );
	if( tipsPanel != NULL )
	{

		if (tipsPanel->IsVisible==FALSE ||
			tipsPanel->CurrentTipsType == TFT_FreeStyle ||
			tipsPanel->CurrentTipsType == TFT_Link
			){
			//if( !tipsPanel->IsVisible/* && !tipsPanel->IsTipsAnchored */){
			//delegateShowItemTips( pItem, fromObject );
			tipsPanel->ShowLinkTips( fromObject );
			tipsPanel->SetTextLinkTipsPos();
		}
	}
}

void UMMOUIManager::ShowItemLinkTipsWithoutAnchored( UGFxObject *fromObject )
{
	UMMOUITips *tipsPanel = (UMMOUITips*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_DynamicUI_Tips );
	if( tipsPanel != NULL ){
		if( !tipsPanel->IsVisible){
			tipsPanel->ShowLinkTips( fromObject );
		}
	}
}

void UMMOUIManager::SetEnterPrivateChatDelegate(FScriptDelegate fun)
{
	__EnterPrivateChat__Delegate = fun;
}

void UMMOUIManager::SetSetFilterChannelNumDelegate(FScriptDelegate fun)
{
	__SetFilterChannelNum__Delegate = fun;
}
//
//void UMMOUIManager::SetSystemMessageDelegate(FScriptDelegate fun)
//{
//	__AddSystemMessage__Delegate = fun;
//}

void UMMOUIManager::SetSetFilterChannelDelegate(FScriptDelegate fun)
{
	__SetFilterChannel__Delegate = fun;
}

void UMMOUIManager::SetScreenMessageDelegate(FScriptDelegate fun)
{
	__AddScreenMessage__Delegate = fun;
}

void UMMOUIManager::SetOpenShopDelegate(FScriptDelegate fun)
{
	__OpenShop__Delegate = fun;
}

void UMMOUIManager::SetOpenBlackMarketDelegate(FScriptDelegate fun)
{
	__OpenBlackMarket__Delegate = fun;
}

// dialog delegate
//void UMMOUIManager::SetOpenDialogDelegate(FScriptDelegate fun)
//{
//	__OpenDialog__Delegate = fun;
//}

//void UMMOUIManager::SetCloseDialogDelegate(FScriptDelegate fun)
//{
//	__CloseDialog__Delegate = fun;
//}

//void UMMOUIManager::SetAddInputMessageBoxDelegate(FScriptDelegate fun)
//{
//	__AddInputMessageBox__Delegate = fun;
//}

//void UMMOUIManager::SetAddMessageBoxDelegate(FScriptDelegate fun)
//{
//	__AddMessageBox__Delegate = fun;
//}

void UMMOUIManager::SetReplaceHotSkillDelegate(FScriptDelegate fun)
{
	__ReplaceHotSkill__Delegate = fun;
}

//void UMMOUIManager::SetAddModalMessageBoxDelegate(FScriptDelegate fun)
//{
//	__AddModalMessageBox__Delegate = fun;
//}

//void UMMOUIManager::SetDispatchMsgOkDelegate(FScriptDelegate fun)
//{
//	__DispatchMsgBoxButtonOk__Delegate = fun;
//}
//
//void UMMOUIManager::SetDispatchMsgNoDelegate(FScriptDelegate fun)
//{
//	__DispatchMsgBoxButtonNo__Delegate = fun;
//}

// void UMMOUIManager::SetToggleDropItemTipsDelegate(FScriptDelegate fun)
// {
// 	__ToggleDropItemTips__Delegate = fun;
// }

//void UMMOUIManager::SetOpenWarehouseDelegate(FScriptDelegate fun)
//{
//	__OpenWarehouse__Delegate = fun;
//}

void UMMOUIManager::SetUpdateWarehouseCapSlotDelegate(FScriptDelegate fun)
{
	__UpdateWarehouseCapSlot__Delegate = fun;
}


//void UMMOUIManager::SetToggleTeamDelegate(FScriptDelegate fun)
//{
//	__ToggleTeam__Delegate = fun;
//}

//void UMMOUIManager::SetInviteAddTeamDelegate(FScriptDelegate fun)
//{
//	__InviteAddTeam__Delegate = fun;
//}

//void UMMOUIManager::SetLeaveTeamDelegate(FScriptDelegate fun)
//{
//	__LeaveTeam__Delegate = fun;
//}
//
//void UMMOUIManager::SetMoveLeaderDelegate(FScriptDelegate fun)
//{
//	__MoveLeader__Delegate = fun;
//}

//void UMMOUIManager::SetInviteTeamRequestDelegate(FScriptDelegate fun)
//{
//	__InviteTeamRequest__Delegate = fun;
//}

//void UMMOUIManager::SetAddTeamRequestDelegate(FScriptDelegate fun)
//{
//	__AddTeamRequest__Delegate = fun;
//}

//void UMMOUIManager::SetToggleCharactorDelegate(FScriptDelegate fun)
//{
//	__ToggleCharactor__Delegate = fun;
//}
//P2P Trade
void UMMOUIManager::SetApplyDoTradeDelegate(FScriptDelegate fun)
{
	__ApplyDoTrade__Delegate = fun;
}

//void UMMOUIManager::SetToggleWorldMapDelegate(FScriptDelegate fun)
//{
//	__ToggleWorldMap__Delegate = fun;
//}

//void UMMOUIManager::SetToggleTeleportNetDelegate(FScriptDelegate fun)
//{
//	__ToggleTeleportNet__Delegate = fun;
//}

//void UMMOUIManager::SetToggleFBEnterDelegate(FScriptDelegate fun)
//{
//	__ToggleFBEnter__Delegate = fun;
//}

//void UMMOUIManager::SetTogglePVPDelegate(FScriptDelegate fun)
//{
//	__TogglePVP__Delegate = fun;
//}

void UMMOUIManager::SetToggleBattleSummaryDelegate(FScriptDelegate fun)
{
	__ToggleBattleSummary__Delegate = fun;
}

void UMMOUIManager::SetToggleSkillDelegate(FScriptDelegate fun)
{
	__ToggleSkill__Delegate = fun;
}

//void UMMOUIManager::SetOpenQuestPopupUpDelegate(FScriptDelegate fun)
//{
//	__OpenQuestPopupUp__Delegate = fun;
//}

void UMMOUIManager::SetAddTraceQuestDelegate(FScriptDelegate fun)
{
	__AddTraceQuest__Delegate = fun;
}

//--------------------------------------------
void UMMOUIManager::SetBuffEnableAbilityDelegate(FScriptDelegate fun)
{
	__BuffEnableAbility__Delegate = fun;
}


void UMMOUIManager::SetUpdateBossQTEDelegate(FScriptDelegate fun)
{
	__UpdateBossQTE__Delegate = fun;
}




//DamageLevel
void UMMOUIManager::SetShowAttackNumberDelegate( FScriptDelegate fun )
{
	__ShowAttackNumber__Delegate = fun;
}

void UMMOUIManager::SetShowAttackedNumberDelegate(FScriptDelegate fun)
{
	__ShowAttackedNumber__Delegate = fun;
}

void UMMOUIManager::SetShowDodgeDelegate(FScriptDelegate fun)
{
	__ShowDodge__Delegate = fun;
}

void UMMOUIManager::SetShowMissDelegate(FScriptDelegate fun)
{
	__ShowMiss__Delegate = fun;
}

void UMMOUIManager::SetShowParryDelegate(FScriptDelegate fun)
{
	__ShowParry__Delegate = fun;
}

void UMMOUIManager::SetShowImmunityDelegate(FScriptDelegate fun)
{
	__ShowImmunity__Delegate = fun;
}

void UMMOUIManager::SetShowCriticalNumberDelegate(FScriptDelegate fun)
{
	__ShowCriticalNumber__Delegate = fun;
}

void UMMOUIManager::SetShowCriticaledNumberDelegate(FScriptDelegate fun)
{
	__ShowCriticaledNumber__Delegate = fun;
}

void UMMOUIManager::SetShowBlockNumberDelegate(FScriptDelegate fun)
{
	__ShowBlockNumber__Delegate = fun;
}

void UMMOUIManager::SetShowExpNumberDelegate(FScriptDelegate fun)
{
	__ShowExpNumber__Delegate = fun;
}

void UMMOUIManager::SetShowHPRestoreDelegate(FScriptDelegate fun)
{
	__ShowHPRestore__Delegate = fun;
}

void UMMOUIManager::SetShowGetMoneyDelegate(FScriptDelegate fun)
{
	__ShowGetMoney__Delegate = fun;
}

//Friend
void UMMOUIManager::SetMoveUnReadMsgToReadDelegate(FScriptDelegate fun)
{
	__MoveUnReadMsgToRead__Delegate = fun;
}

//void UMMOUIManager::SetToggleFriendDelegate(FScriptDelegate fun)
//{
//	__ToggleFriend__Delegate = fun;
//}

//void UMMOUIManager::SetChangeFriendGroupDelegate(FScriptDelegate fun)
//{
//	__ChangeFriendGroup__Delegate = fun;
//}

//void UMMOUIManager::SetRequestAddAttentionDelegate(FScriptDelegate fun)
//{
//	__RequestAddAttention__Delegate = fun;
//}

//void UMMOUIManager::SetRequestDelAttentionDelegate(FScriptDelegate fun)
//{
//	__RequestDelAttention__Delegate = fun;
//}

//void UMMOUIManager::SetUpdateBlackListDelegate(FScriptDelegate fun)
//{
//	__UpdateBlackList__Delegate = fun;
//}

//void UMMOUIManager::SetUpdateNearContactDelegate(FScriptDelegate fun)
//{
//	__UpdateNearContactList__Delegate = fun;
//}
//
//void UMMOUIManager::SetUpdateEnemyListDelegate(FScriptDelegate fun)
//{
//	__UpdateEnemyList__Delegate = fun;
//}

void UMMOUIManager::SetChangeGroupNameDelegate(FScriptDelegate fun)
{
	__ChangeGroupName__Delegate = fun;
}

//void UMMOUIManager::SetDeleteGroupDelegate(FScriptDelegate fun)
//{
//	__DeleteGroup__Delegate = fun;
//}

void UMMOUIManager::SetSelectFriendChatDelegate(FScriptDelegate fun)
{
	__SelectFriendChat__Delegate = fun;
}

//Keyboard simulated press--------------------------------------------------------------------------------------------------
void UMMOUIManager::SetUpdateSkillBarKeyDownDelegate(FScriptDelegate fun)
{
	__UpdateSkillBarKeyDown__Delegate = fun;
}

void UMMOUIManager::SetUpdateMainMenuKeyDownDelegate(FScriptDelegate fun)
{
	__UpdateMainMenuKeyDown__Delegate = fun;
}

//Hero--------------------------------------------------------------------------------------------------
void UMMOUIManager::SetRefreshHeroAttributeDelegate(FScriptDelegate fun)
{
	__RefreshHeroAttribute__Delegate = fun;
}

void UMMOUIManager::SetRefreshHeroSlotDelegate(FScriptDelegate fun)
{
	__RefreshHeroSlot__Delegate = fun;
}

void UMMOUIManager::SetRefreshHeroDelegate(FScriptDelegate fun)
{
	__RefreshHero__Delegate = fun;
}

void UMMOUIManager::SetRefreshSummonDelegate(FScriptDelegate fun)
{
	__RefreshSummon__Delegate = fun;
}

//Mount----------------------------------------------------------------------------------------------------
void UMMOUIManager::SetRefreshMountDelegate(FScriptDelegate fun)
{
	__RefreshMount__Delegate = fun;
}

void UMMOUIManager::SetRefreshEquipedMountDelegate(FScriptDelegate fun)
{
	__RefreshEquipedMount__Delegate = fun;
}

//void UMMOUIManager::SetShowKeyWarningDelegate(FScriptDelegate fun)
//{
//	__ShowKeyWarning__Delegate = fun;
//}

//Death-------------------------------------------------------------------------------------------------
void UMMOUIManager::SetShowDeathDelegate(FScriptDelegate fun)
{
	__ShowDeath__Delegate = fun;
}

void UMMOUIManager::SetHideDeathDelegate(FScriptDelegate fun)
{
	__HideDeath__Delegate = fun;
}

void UMMOUIManager::SetCallShowWaitingReviveDelegate(FScriptDelegate fun)
{
	__CallShowWaitingRevive__Delegate = fun;
}


void UMMOUIManager::SetUpdateHintTextDelegate(FScriptDelegate fun)
{
	__UpdateHintText__Delegate = fun;
}

void UMMOUIManager::SetUpdateHintTextByIDDelegate(FScriptDelegate fun)
{
	__UpdateHintTextByID__Delegate = fun;
}

void UMMOUIManager::SetCallBack_PreLoadDelegate(FScriptDelegate fun)
{
	__CallBack_PreLoad__Delegate = fun;
}

void UMMOUIManager::SetShowHintBtnDelegate(FScriptDelegate fun)
{
	__ShowHintBtn__Delegate = fun;
}

void UMMOUIManager::SetUpdateMinimapDelegate(FScriptDelegate fun)
{
	__UpdateMinimap__Delegate = fun;
}

void UMMOUIManager::SetAddBuffPanelDelegate(FScriptDelegate fun)
{
	__AddBuffPanel__Delegate = fun;
}

void UMMOUIManager::SetDeleteBuffPanelDelegate(FScriptDelegate fun)
{
	__DeleteBuffPanel__Delegate = fun;
}

void UMMOUIManager::SetFlashHPBarDelegate(FScriptDelegate fun)
{
	__FlashHPBar__Delegate = fun;
}

void UMMOUIManager::SetUpdatePetStateDelegate(FScriptDelegate fun)
{
	__UpdatePetState__Delegate = fun;
}

void UMMOUIManager::SetUpdateMenuEffectDelegate(FScriptDelegate fun)
{
	__UpdateMenuEffect__Delegate = fun;
}



void UMMOUIManager::SetAddFunctionBuffPanelDelegate(FScriptDelegate fun)
{
	__AddFunctionBuffPanel__Delegate = fun;
}

void UMMOUIManager::SetDeleteFunctionBuffPanelDelegate(FScriptDelegate fun)
{
	__DeleteFunctionBuffPanel__Delegate = fun;
}

void UMMOUIManager::SetAddTargetBuffPanelDelegate(FScriptDelegate fun)
{
	__AddTargetBuffPanel__Delegate = fun;
}

void UMMOUIManager::SetHeroInfoUpdateDataDelegate(FScriptDelegate fun)
{
	__HeroInfoUpdateData__Delegate = fun;
}

void UMMOUIManager::SetDeleteTargetBuffPanelDelegate(FScriptDelegate fun)
{
	__DeleteTargetBuffPanel__Delegate = fun;
}

//void UMMOUIManager::SetShowProgressBarDelegate(FScriptDelegate fun)
//{
//	__ShowProgressBar__Delegate = fun;
//}
//
//void UMMOUIManager::SetCloseProgressBarDelegate(FScriptDelegate fun)
//{
//	__CloseProgressBar__Delegate = fun;
//}
//
//void UMMOUIManager::SetShowProgressBarStrengthDelegate(FScriptDelegate fun)
//{
//	__ShowProgressBarStrength__Delegate = fun;
//}
//
//void UMMOUIManager::SetCloseProgressBarStrengthDelegate(FScriptDelegate fun)
//{
//	__CloseProgressBarStrength__Delegate = fun;
//}
//
//void UMMOUIManager::SetShowProgressBarQTEDelegate(FScriptDelegate fun)
//{
//	__ShowProgressBarQTE__Delegate = fun;
//}
//
//void UMMOUIManager::SetCloseProgressBarQTEDelegate(FScriptDelegate fun)
//{
//	__CloseProgressBarQTE__Delegate = fun;
//}

//void UMMOUIManager::SetShowHintSceneWidgetDelegate(FScriptDelegate fun)
//{
//	__ShowHintSceneWidget__Delegate = fun;
//}
//
//void UMMOUIManager::SetCloseHintLevelUpWidgetDelegate(FScriptDelegate fun)
//{
//	__CloseHintLevelUpWidget__Delegate = fun;
//}

//void UMMOUIManager::SetAddSkillLearnedMsgDelegate(FScriptDelegate fun)
//{
//    __AddSkillLearnedMsg__Delegate = fun;
//}

//void UMMOUIManager::SetShowHintLevelUpWidgetDelegate(FScriptDelegate fun)
//{
//	__ShowHintLevelUpWidget__Delegate = fun;
//}

//void UMMOUIManager::SetCloseHintSceneWidgetDelegate(FScriptDelegate fun)
//{
//	__CloseHintSceneWidget__Delegate = fun;
//}

void UMMOUIManager::SetOnShiftDelegate(FScriptDelegate fun)
{
	__OnShift__Delegate = fun;
}

void UMMOUIManager::SetOpenBagDelegate(FScriptDelegate fun)
{
	__OpenBag__Delegate = fun;
}

void UMMOUIManager::SetGetOpenBagDelegate(FScriptDelegate fun)
{
	__GetOpenBag__Delegate = fun;
}

void UMMOUIManager::SetDeleteItemDelegate(FScriptDelegate fun)
{
	__DeleteItem__Delegate = fun;
}

void UMMOUIManager::SetTurntoPageDelegate(FScriptDelegate fun)
{
	__TurntoPage__Delegate = fun;
}

//void UMMOUIManager::SetToggleBagDelegate(FScriptDelegate fun)
//{
//	__ToggleBag__Delegate = fun;
//}

void UMMOUIManager::SetLockItemDelegate(FScriptDelegate fun)
{
	__LockItem__Delegate = fun;
}


void UMMOUIManager::SetOpenRClickMenuDelegate(FScriptDelegate fun)
{
	__OpenRClickMenu__Delegate = fun;
}

void UMMOUIManager::SetHideRClickChildMenuDelegate(FScriptDelegate fun)
{
	__HideRClickChildMenu__Delegate = fun;
}

void UMMOUIManager::SetHideRClickMenuDelegate(FScriptDelegate fun)
{
	__HideRClickMenu__Delegate = fun;
}

void UMMOUIManager::SetOpenRClickChildMenu_FriendDelegate(FScriptDelegate fun)
{
	__OpenRClickChildMenu_Friend__Delegate = fun;
}

void UMMOUIManager::SetOpenRClickChildMenu_TeamDelegate(FScriptDelegate fun)
{
	__OpenRClickChildMenu_Team__Delegate = fun;
}

void UMMOUIManager::SetCloseRClickMenuDelegate(FScriptDelegate fun)
{
	__CloseRClickMenu__Delegate = fun;
}


//void UMMOUIManager::SetRequireOpenStallDelegate( FScriptDelegate fun )
//{
//	__RequireOpenStall__Delegate = fun;
//}

//void UMMOUIManager::SetOpenStallSettingDelegate( FScriptDelegate fun )
//{
//	__OpenStallSetting__Delegate = fun;
//}

//void UMMOUIManager::SetOpenSkillDelegate( FScriptDelegate fun )
//{
//	__OpenSkill__Delegate = fun;
//}

//void UMMOUIManager::SetOpenMasterStudentDelegate( FScriptDelegate fun )
//{
//	__OpenMasterStudent__Delegate = fun;
//}

//hintbuttonbar
void UMMOUIManager::SetAddNewHintButtonDelegate(FScriptDelegate fun)
{
	__AddNewHintButton__Delegate = fun;
}

void UMMOUIManager::SetRemoveHintButtonDelegate(FScriptDelegate fun)
{
	__RemoveHintButton__Delegate = fun;
}

//msgbutton
void UMMOUIManager::SetSetMsgButtonNumDelegate(FScriptDelegate fun)
{
	__SetMsgButtonNum__Delegate = fun;
}

void UMMOUIManager::SetSetMsgButtonIconDelegate(FScriptDelegate fun)
{
	__SetMsgButtonIcon__Delegate = fun;
}

//
void UMMOUIManager::OpenGameAnnouncement( int updateVer, int reserve )
{
	UMMOUIGameAnnouncement* panel = (UMMOUIGameAnnouncement*)MMOUI::FindUIPanelEx( Panel_GameAnnouncement );
	if( panel != NULL ){
		panel->UpdateContent( updateVer );
		panel->Show( TRUE );
	}
}

//arena
void UMMOUIManager::SetOpenArenaQueueDelegate(FScriptDelegate fun)
{
	__OpenArenaQueue__Delegate = fun;
}

void UMMOUIManager::SetLeaveArenaQueueDelegate(FScriptDelegate fun)
{
	__LeaveArenaQueue__Delegate = fun;
}

void UMMOUIManager::SetAskEnterArenaDelegate(FScriptDelegate fun)
{
	__AskEnterArena__Delegate = fun;
}

void UMMOUIManager::SetOpenArenaScoreSingleDelegate(FScriptDelegate fun)
{
	__OpenArenaScoreSingle__Delegate = fun;
}

void UMMOUIManager::SetCloseArenaScoreDelegate(FScriptDelegate fun)
{
	__CloseArenaScore__Delegate = fun;
}
//
void UMMOUIManager::SetASClickQTEDelegate(FScriptDelegate fun)
{
	__ASClickQTE__Delegate = fun;
}

void UMMOUIManager::SetASPkModeChangeDelegate(FScriptDelegate fun)
{
	__ASPkModeChange__Delegate = fun;
}


//void UMMOUIManager::SetShowUnlockAOGStringDelegate(FScriptDelegate fun)
//{
//	__ShowUnlockAOGString__Delegate = fun;
//}
//void UMMOUIManager::SetHideUnlockAOGStringDelegate(FScriptDelegate fun)
//{
//	__HideUnlockAOGString__Delegate = fun;
//}

//void UMMOUIManager::SetShowInGameMenuDelegate(FScriptDelegate fun)
//{
//	__ShowInGameMenu__Delegate = fun;
//}
//
//void UMMOUIManager::SetOpenSystemSettingDelegate(FScriptDelegate fun)
//{
//    __OpenSystemSetting__Delegate = fun;
//}

void UMMOUIManager::SendReturnSelRole()
{
	//FScriptDelegate funYes(EC_EventParm);
	//funYes.Object = this;
	//funYes.FunctionName = TEXT("ImmReturnSelRole");	

	//FScriptDelegate funNo(EC_EventParm);
	//funNo.Object = this;

	AMMOPlayer* actor = MMOFacade::getSingleton().GetMainPlayer();
	if(actor)
	{
		FScriptDelegate funYes(EC_EventParm);
		funYes.Object = this;
		funYes.FunctionName = TEXT("ImmReturnSelRole");	

		FScriptDelegate funNo(EC_EventParm);
		funNo.Object = this;

		MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Return.Select.Safe")), 0, funYes, funNo);
	}

	//   if (actor && !actor->IsInSafeZone){
	//	FScriptDelegate funYes(EC_EventParm);
	//	funYes.Object = this;
	//	funYes.FunctionName = TEXT("ImmReturnLogin");	

	//	FScriptDelegate funNo(EC_EventParm);
	//	funNo.Object = this;

	//       MMOMsgBoxManager::getSingleton().AddMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Return.Select.NoSafe")), 0, funYes, funNo);
	//}
	//   else{
	//	FScriptDelegate funYes(EC_EventParm);
	//	funYes.Object = this;
	//	funYes.FunctionName = TEXT("ImmReturnSelRole");	

	//	FScriptDelegate funNo(EC_EventParm);
	//	funNo.Object = this;

	//       MMOMsgBoxManager::getSingleton().AddMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Return.Select.Safe")), 0, funYes, funNo);
	//}
}

void UMMOUIManager::PromptFreePK()
{
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	//funYes.FunctionName = TEXT("ImmReturnLogin");	

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("ImmReturnLogin");	

	UGFxObject* MsgboxWindow = MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.FreePK.PromptFreePK")), 0, funYes, funNo);
	if(MsgboxWindow)
	{
		UGFxObject* butt0 = MsgboxWindow->GetObject(TEXT("yesButton_mc"));
		if(butt0)
		{
			butt0->SetString( TEXT("label") , TEXT("Agree")  );
		}

		UGFxObject* butt1 = MsgboxWindow->GetObject(TEXT("noButton_mc"));
		if(butt1)
		{
			butt1->SetString( TEXT("label") , TEXT("Quit")  );
		}
	}

}


void UMMOUIManager::SendReturnLogin()
{
	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("ImmReturnLogin");	

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;

	MMOMsgBoxManager::getSingleton().AddModalMessageBox(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Return.Login")), 0, funYes, funNo);
}

void UMMOUIManager::SendQuitGame()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_OptionQuit,TRUE, "");
	UMMOUIOptionQuit* QUI =  Cast<UMMOUIOptionQuit>(FindUIPanel(MMOUI::GetUIManager()->Panel_OptionQuit));
	//QUI->eventUCInitQuitTimer("Will ", 20, " seconds then exit game automatically...");
	if(QUI)
	{
		QUI->_ReturnType = 0;
		UGFxObject* immedObj	=QUI->GetObject(TEXT("quit_immed"));
		if(immedObj)
		{
			immedObj->SetString(TEXT("label"),TEXT("Quit Now"));
		}
		UGFxObject* cancelObj	=QUI->GetObject(TEXT("quit_cancel"));
		if(cancelObj)
		{
			cancelObj->SetString(TEXT("label"),TEXT("Continue Playing"));
		}
		QUI->eventUCInitQuitTimer(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Return.Quit.CD.Head")), 10, MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Return.Quit.CD.Tail")));
	}
}

void UMMOUIManager::ImmReturnSelRole()
{
	MMOGameEvent_Any evt(GameEvent_ChangeRole);
	MMOUI::PostGameEvent(&evt);

	CMessageHandler::GetInstance().Send_ReturnSelRole();
}

void UMMOUIManager::ImmReturnLogin()
{
	MMOGameEvent_Any evt(GameEvent_ChangeRole);
	MMOUI::PostGameEvent(&evt);

	if(CMessageHandler::GetInstance().IsConnection())
	{
		CMessageHandler::GetInstance().Send_ReturnLogin();
		CMessageHandler::GetInstance().CloseConnection(true);
	}
	else
	{
		AMMOPlayerController* pc	= MMOFacade::getSingleton().GetMainController();
		if(pc){
			pc->ReturnLogin();
		}
	}
}

void UMMOUIManager::ImmQuitGame()
{
	MMOFacade::getSingleton().GetMainController()->ConsoleCommand(TEXT("exit"));
}

void UMMOUIManager::SetUpdateReputationDataDelegate(FScriptDelegate fun)
{
	__UpdateReputationData__Delegate = fun;
}

void UMMOUIManager::SetRefreshReputationSelectedDelegate(FScriptDelegate fun)
{
	__RefreshReputationSelected__Delegate = fun;
}

void UMMOUIManager::SetUpdateTraceReputationDataDelegate(FScriptDelegate fun)
{
	__UpdateTraceReputationData__Delegate = fun;
}

//void UMMOUIManager::SetDisplayReputationDataDelegate(FScriptDelegate fun)
//{
//	__DisplayReputationData__Delegate = fun;
//}

void UMMOUIManager::SetUpdateTitleDataDelegate(FScriptDelegate fun)
{
	__UpdateTitleData__Delegate = fun;
}

void UMMOUIManager::SetResetPlayerHintDelegate(FScriptDelegate fun)
{
	__ResetPlayerHint__Delegate = fun;
}

void UMMOUIManager::SetOpenStallBuyDelegate(FScriptDelegate fun)
{
	__OpenStallBuy__Delegate = fun;
}

// Achievement
//void UMMOUIManager::SetShowAchievementPanelDelegate(FScriptDelegate fun)
//{
//	__ShowAchievementPanel__Delegate = fun;
//}

// Guild Create
void UMMOUIManager::SetShowGuildCreatePanelDelegate(FScriptDelegate fun)
{
	__ShowGuildCreatePanel__Delegate = fun;
}

// Guild View
void UMMOUIManager::SetShowGuildViewPanelDelegate(FScriptDelegate fun)
{
	__ShowGuildViewPanel__Delegate = fun;
}

// Guild Territory
void UMMOUIManager::SetShowGuildTerritoryPanelDelegate(FScriptDelegate fun)
{
	__ShowGuildTerritoryPanel__Delegate = fun;
}

void UMMOUIManager::SetUpdateAccessInfoDelegate(FScriptDelegate fun)
{
	__UpdateAccessInfo__Delegate = fun;
}

void UMMOUIManager::SetFreshWarehouseDelegate(FScriptDelegate fun)
{
	__FreshWarehouse__Delegate = fun;
}

//// Guild Main
//void UMMOUIManager::SetShowGuildMainPanelDelegate(FScriptDelegate fun)
//{
//	__ShowGuildMainPanel__Delegate = fun;
//}

void UMMOUIManager::SetForbidChatDelegate(FScriptDelegate fun)
{
	__ForbidChat__Delegate = fun;
}

void UMMOUIManager::SetCancelAllianceDelegate(FScriptDelegate fun)
{
	__CancelAlliance__Delegate = fun;
}
//TScriptInterface<class IIItemSlotActionListener> g_DefaultListener;
TScriptInterface<class IIItemSlotActionListener> g_CurrentListener;
//TScriptInterface<class IIItemSlotActionListener> g_LastListener;
//void UMMOUIManager::SetDefaultItemSlotActionListener(const TScriptInterface<class IIItemSlotActionListener>& listener)
//{
//	if(g_DefaultListener.GetInterface() != NULL)
//	{
//		g_DefaultListener->eventOnUnSubscribeItemSlotAction();
//	}
//
//	if(listener.GetInterface() != NULL)
//	{
//		listener->eventOnSubscribeItemSlotAction();
//	}
//
//	g_DefaultListener = listener;
//
//}

//TScriptInterface<class IIItemSlotActionListener> UMMOUIManager::GetDefaultItemSlotActionListener()
//{
//	return g_DefaultListener;
//}

void UMMOUIManager::SetCurrentItemSlotActionListener(const TScriptInterface<class IIItemSlotActionListener>& listener)
{
	if(g_CurrentListener==listener)
	{
		return;
	}
	debugf(NAME_Error, TEXT("[SetCurrentItemSlotActionListener]"));

	if(g_CurrentListener.GetInterface() != NULL)
	{
		g_CurrentListener->eventOnUnSubscribeItemSlotAction();
	}

	if(listener.GetInterface() != NULL)
	{
		listener->eventOnSubscribeItemSlotAction();
	}

	g_CurrentListener = listener;
	ClearCurrentItemSlotActionListenerCountDown = -1;

}

void UMMOUIManager::ClearCurrentItemSlotActionListener()
{
	SetCurrentItemSlotActionListener(DefaultListener);
	ClearCurrentItemSlotActionListenerCountDown = -1;
}

void UMMOUIManager::ClearCurrentItemSlotActionListenerDelay(INT nFrame)
{
	ClearCurrentItemSlotActionListenerCountDown = nFrame;
}
void UMMOUIManager::ClearCurrentItemSlotActionListenerUpdate()
{
	if(ClearCurrentItemSlotActionListenerCountDown>=0)
	{
		ClearCurrentItemSlotActionListenerCountDown--;
		if(ClearCurrentItemSlotActionListenerCountDown<0)
		{
			ClearCurrentItemSlotActionListener();
		}
	}
}

UBOOL UMMOUIManager::IsItemSlotActionListener()
{
	return g_CurrentListener!=DefaultListener;
}


TScriptInterface<class IIItemSlotActionListener> UMMOUIManager::GetCurrentItemSlotActionListener()
{
	return g_CurrentListener;
}

//TScriptInterface<class IIItemSlotActionListener> UMMOUIManager::GetLastItemSlotActionListener()
//{
//	return g_LastListener;
//}



//------------------------------------------------------------------------------
void UMMOUIManager::MoveByKey( BYTE MoveType,UBOOL MoveFlag)
{
	int nIndex=3;
	for(int i=0;i<4;i++)
	{
		if(WSADKey[i] == MoveType)
		{
			nIndex = i;
			break;
		}
	}

	if(MoveFlag)//add press
	{
		for(int i=nIndex;i>0;i--)
		{
			WSADKey[i]=WSADKey[i-1];
		}
		WSADKey[0] = MoveType;
	}
	else//remove press
	{
		for(int i=nIndex;i<4;i++)
		{
			WSADKey[i]=WSADKey[i+1];
		}
		WSADKey[3] = EMCK_Empty;
	}
}

void UMMOUIManager::CloseAllHintForDragging()
{
	UMMOUISkillBar* pSkillBar = (UMMOUISkillBar*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Skillbar);
	if(pSkillBar)
	{
		pSkillBar->CloseHintForAllDragging();
	}
}

void UMMOUIManager::OnUIShow(class UMMOUIBase* ui, UBOOL show)
{
	if(show)
	{
		for(UIMaps::TIterator pairIt(AllUIMaps); pairIt; ++pairIt)
		{
			UMMOUIBase* cmpUI = pairIt.Value();
			if(cmpUI && cmpUI != ui)
			{
				if(cmpUI->ExclusiveVisibilityMask & ui->ExclusiveWithVisibilityMask)
				{
					if(cmpUI->IsVisible)
					{
						cmpUI->Show(FALSE);
					}
				}
				else if(cmpUI->CoexistVisibilityMask & ui->CoexistWithVisibilityMask)
				{
					if(!cmpUI->IsVisible)
					{
						cmpUI->Show(TRUE);
					}
				}
			}
		}

		for(UILayoutList::iterator i = g_layoutList.begin(); i != g_layoutList.end(); ++i)
		{
			IUILayout* layout = *i;
			if(layout->Layout(ui))
			{
				break;
			}
		}
	}
}

void UMMOUIManager::AutoEquipMount()
{
	if(MMOFacade::getSingleton().GetMainController())
	{
		if(MMOFacade::getSingleton().GetMainActor()->ActorAttributes->MountID>0)
		{
			MMOFacade::getSingleton().GetMainController()->NativeTestMount(0);
		}
		else{
			MMOFacade::getSingleton().GetMainController()->NativeTestMount(1);
		}
	}
}

extern UBOOL __BitOp(INT32& v, size_t pos, UBOOL st, UBOOL save_mode)
{
	if (save_mode)
	{
		if (st)
			v |= (1<<pos);
		else
			v &= ~(1<<pos);
	}
	else
	{
		st = ((v & (1<<pos)) ? TRUE : FALSE);
	}
	return st;
}
void UMMOUIManager::SaveConfigToServer()
{
	UBOOL save_mode = TRUE;

	UMMOUIManager* uimgr = MMOUI::GetUIManager();
	INT32 val = 0, ips = 0;
	__BitOp(val, ips++, TRUE                             , save_mode); // always fill 1, meaning data is valid

	// Do not change the fill order below arbitrarily
	__BitOp(val, ips++, uimgr->HintHideNameSelf          , save_mode);
	__BitOp(val, ips++, uimgr->HintHideNameSelfTitle     , save_mode);
	__BitOp(val, ips++, uimgr->HintHideNameOther         , save_mode);
	__BitOp(val, ips++, uimgr->HintHideNameOtherTitle    , save_mode);
	__BitOp(val, ips++, uimgr->HintHideNameFamilySelf    , save_mode);
	__BitOp(val, ips++, uimgr->HintHideNameFamilyOther   , save_mode);
	__BitOp(val, ips++, uimgr->HintHideNameMonster       , save_mode);
	__BitOp(val, ips++, uimgr->HintHideNameDrop          , save_mode);
	__BitOp(val, ips++, uimgr->BloodDrawSelf			 , save_mode);
	__BitOp(val, ips++, uimgr->BloodDrawPlayer           , save_mode);
	__BitOp(val, ips++, uimgr->BloodDrawMonster          , save_mode);
	__BitOp(val, ips++, uimgr->HurtHideNameSelf          , save_mode);
	//__BitOp(val, ips++, uimgr->HurtHideNameOther       , save_mode);
	__BitOp(val, ips++, uimgr->SkillEffectHideSelf       , save_mode);
	__BitOp(val, ips++, uimgr->SkillEffectHideOther      , save_mode);
	__BitOp(val, ips++, uimgr->SkillEffectHideCameraShake , save_mode);
	__BitOp(val, ips++, uimgr->DrawGuide                 , save_mode);
	__BitOp(val, ips++, uimgr->DrawChatBubble            , save_mode);
	//__BitOp(val, ips++, uimgr->DrawAITip               , save_mode);
	//__BitOp(val, ips++, uimgr->DrawWeather             , save_mode);
	__BitOp(val, ips++, uimgr->DeclineStrangeChat        , save_mode);
	__BitOp(val, ips++, uimgr->DeclineInviteTeam         , save_mode);
	__BitOp(val, ips++, uimgr->DeclineInviteFamily       , save_mode);
	__BitOp(val, ips++, uimgr->DeclineInviteDisciple     , save_mode);
	__BitOp(val, ips++, uimgr->DeclineInviteMaster       , save_mode);
	__BitOp(val, ips++, uimgr->DeclineShowEnemy			 , save_mode);
	__BitOp(val, ips++, uimgr->DrawBloodNumber           , save_mode);
	__BitOp(val, ips++, uimgr->DrawHurt					 , save_mode);
	__BitOp(val, ips++, uimgr->DrawGameFrame             , save_mode);
	__BitOp(val, ips++, uimgr->HintFriendOnline          , save_mode);
	__BitOp(val, ips++, uimgr->HintHideNearPlayer        , save_mode);
	__BitOp(val, ips++, uimgr->HintMarkSelf				 , save_mode);
	__BitOp(val, ips++, FSM::Ctrl.ctrlFlag_IgnoreShiftBtn, save_mode);
	__BitOp(val, ips++, uimgr->QuestMode				 , save_mode);
	// The 32 bits are used up here; to add more, add an enum in MMOClientVariableManager
	MMOClientVariableManager::getSingleton().SetClientVariable(ECVT_Game_Config, val);

	//Add a 32-bit extension
	INT32 val2 = 0, ips2 = 0;
	__BitOp(val2, ips2++, uimgr->DeclineShowTrade, save_mode);
	MMOClientVariableManager::getSingleton().SetClientVariable(ECVT_Game_Config2, val2);

}

void UMMOUIManager::SaveSoundConfigToServer()
{


	UMMOUIManager* uimgr = MMOUI::GetUIManager();
	if(uimgr==NULL)return;

	UBOOL save_mode = TRUE;
	INT32 data[2] ={0,0};
	BYTE* pByteData = (BYTE*)data;
	INT32 val = 0, ips = 0;
	__BitOp(val, ips++, TRUE                             , save_mode); // always fill 1, meaning data is valid

	// Do not change the fill order below arbitrarily
	__BitOp(val, ips++, uimgr->bSoundAll		, save_mode);
	__BitOp(val, ips++, uimgr->bSoundBack		, save_mode);
	__BitOp(val, ips++, uimgr->bSoundEnv		, save_mode);
	__BitOp(val, ips++, uimgr->bSoundVoice		, save_mode);
	__BitOp(val, ips++, uimgr->bSoundSfx		, save_mode);
	__BitOp(val, ips++, uimgr->bSoundFocusOut?FALSE:TRUE	, save_mode);

	*(pByteData+0) = val;

	*(pByteData+1) =  uimgr->nSoundAllVL;
	*(pByteData+2) =  uimgr->nSoundBackVL;
	*(pByteData+3) =  uimgr->nSoundEnvVL;
	*(pByteData+4) =  uimgr->nSoundVoiceVL;
	*(pByteData+5) =  uimgr->nSoundSfxVL;

	MMOClientVariableManager::getSingleton().SetClientVariable(ECVT_Sound_Config1, data[0]);
	MMOClientVariableManager::getSingleton().SetClientVariable(ECVT_Sound_Config2, data[1]);
}



void UMMOUIManager::SetItemSlotWaitingState(UBOOL bWaiting)
{
	if(bWaiting){
		if(bWaitingFlag==FALSE){
			UItemSlotActionListener_Waiting* pWaitingListener =  eventGetWaitingListener();
			SetCurrentItemSlotActionListener(pWaitingListener);
			bWaitingFlag = TRUE;
		}
	}
	else
	{
		if(bWaitingFlag){
			ClearCurrentItemSlotActionListener();
			bWaitingFlag = FALSE;
		}
	}
}

void UMMOUIManager::ShowNpcShop( INT tempId )
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Shop, TRUE, "");
	MMOUI::GetUIManager()->delegateOpenShop( tempId );
	MMOShopManager::getSingleton().setShopId(tempId);
}

void UMMOUIManager::ShowBlackMarket(int id)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Shop, TRUE, "");
	MMOUI::GetUIManager()->delegateOpenBlackMarket( id );
	
}

void UMMOUIManager::ShowTargetPlayerInfo()
{
	AMMOActor* targetActor = FSMIO::getSingleton().Target.GetDesireMMOActor();
	if( targetActor )
	{
		UMMOUIPlayerInfo* playerInfo = Cast<UMMOUIPlayerInfo>( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_PlayerInfo ) );
		if(playerInfo)
		{
			playerInfo->InitPlayerRoleId( targetActor->ServerID );
			playerInfo->Show( TRUE );
		}
	}
}

void UMMOUIManager::ApplySoundConfigByServer(INT n1,INT n2)
{
	UBOOL save_mode = FALSE;
	UBOOL save_buf = FALSE;

	UMMOUIManager* uimgr = MMOUI::GetUIManager();
	if(uimgr==NULL)return;

	INT32 data[2] ={n1,n2};
	BYTE* pByteData = (BYTE*)data;

	INT32 val = *(pByteData+0), ips = 0;
	UBOOL is_val = __BitOp(val, ips++, save_buf, save_mode);


	if (is_val)
	{
		// Do not change the access order below arbitrarily
		uimgr->bSoundAll        = __BitOp(val, ips++, save_buf, save_mode);
		uimgr->bSoundBack       = __BitOp(val, ips++, save_buf, save_mode);
		uimgr->bSoundEnv        = __BitOp(val, ips++, save_buf, save_mode);
		uimgr->bSoundVoice      = __BitOp(val, ips++, save_buf, save_mode);
		uimgr->bSoundSfx        = __BitOp(val, ips++, save_buf, save_mode);
		uimgr->bSoundFocusOut   = __BitOp(val, ips++, save_buf, save_mode)?FALSE:TRUE;


		uimgr->nSoundAllVL		= *(pByteData+1);
		uimgr->nSoundBackVL		= *(pByteData+2);
		uimgr->nSoundEnvVL		= *(pByteData+3);
		uimgr->nSoundVoiceVL	= *(pByteData+4);
		uimgr->nSoundSfxVL	= *(pByteData+5);
	}
	else
	{
		uimgr->bSoundAll        = TRUE;
		uimgr->bSoundBack       = TRUE;
		uimgr->bSoundEnv        = TRUE;
		uimgr->bSoundVoice      = TRUE;
		uimgr->bSoundSfx		= TRUE;
		uimgr->bSoundFocusOut	= TRUE;


		uimgr->nSoundAllVL		= 50;
		uimgr->nSoundBackVL		= 50;
		uimgr->nSoundEnvVL		= 50;
		uimgr->nSoundVoiceVL	= 50;
		uimgr->nSoundSfxVL	= 50;

		SaveSoundConfigToServer();
	}


	UAudioDevice* ad = GEngine->GetAudioDevice();
	if(ad) // oscar@snda TODO: find out why it was not created
	{
		ad->VolumeTotal_op     = uimgr->bSoundAll;
		ad->VolumeBackMusic_op = uimgr->bSoundBack;
		ad->VolumeEnv_op       = uimgr->bSoundEnv;	
		ad->VolumeVoice_op     = uimgr->bSoundVoice;
		ad->VolumeSfx_op     = uimgr->bSoundSfx;
		GIsVolumeInBackground     = uimgr->bSoundFocusOut;
		ad->VolumeTotal     = (float)uimgr->nSoundAllVL		/ 100.0f;
		ad->VolumeBackMusic = (float)uimgr->nSoundBackVL	/ 100.0f;
		ad->VolumeEnv       = (float)uimgr->nSoundEnvVL		/ 100.0f;
		ad->VolumeVoice     = (float)uimgr->nSoundVoiceVL	/ 100.0f;
		ad->VolumeSfx     = (float)uimgr->nSoundSfxVL	/ 100.0f;
	}
}

void UMMOUIManager::ApplyConfigByServer(INT n,INT n2)
{
	UBOOL save_mode = FALSE;
	UBOOL save_buf = FALSE;

	UMMOUIManager* uimgr = MMOUI::GetUIManager();
	INT32 val = n, ips = 0;
	INT32 val2 = n2, ips2 = 0;
	UBOOL is_val = __BitOp(val, ips++, save_buf, save_mode);
	if (is_val)
	{
		// Do not change the access order below arbitrarily
		uimgr->HintHideNameSelf				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintHideNameSelfTitle		= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintHideNameOther			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintHideNameOtherTitle		= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintHideNameFamilySelf		= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintHideNameFamilyOther		= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintHideNameMonster			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintHideNameDrop				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->BloodDrawSelf				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->BloodDrawPlayer				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->BloodDrawMonster				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HurtHideNameSelf				= __BitOp(val, ips++, save_buf, save_mode);
		//uimgr->HurtHideNameOther			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->SkillEffectHideSelf			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->SkillEffectHideOther			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->SkillEffectHideCameraShake	= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DrawGuide					= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DrawChatBubble				= __BitOp(val, ips++, save_buf, save_mode);
		//uimgr->DrawAITip					= __BitOp(val, ips++, save_buf, save_mode);
		//uimgr->DrawWeather				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DeclineStrangeChat			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DeclineInviteTeam			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DeclineInviteFamily			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DeclineInviteDisciple		= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DeclineInviteMaster			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DeclineShowEnemy				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DrawBloodNumber				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DrawHurt						= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->DrawGameFrame				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintFriendOnline				= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintHideNearPlayer			= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->HintMarkSelf					= __BitOp(val, ips++, save_buf, save_mode);
		FSM::Ctrl.ctrlFlag_IgnoreShiftBtn	= __BitOp(val, ips++, save_buf, save_mode);
		uimgr->QuestMode					= __BitOp(val, ips++, save_buf, save_mode);
		//Extension
		uimgr->DeclineShowTrade				= __BitOp(val2, ips2++, save_buf, save_mode);

	}
	else
	{
		uimgr->HintHideNameSelf				= FALSE;
		uimgr->HintHideNameSelfTitle		= FALSE;
		uimgr->HintHideNameOther			= FALSE;
		uimgr->HintHideNameOtherTitle		= FALSE;
		uimgr->HintHideNameFamilySelf		= FALSE;
		uimgr->HintHideNameFamilyOther		= FALSE;
		uimgr->HintHideNameMonster			= FALSE;
		uimgr->HintHideNameDrop				= FALSE;
		uimgr->BloodDrawSelf				= TRUE;
		uimgr->BloodDrawPlayer				= TRUE;
		uimgr->BloodDrawMonster				= TRUE;
		uimgr->HurtHideNameSelf				= FALSE;
		uimgr->HurtHideNameOther			= FALSE;
		uimgr->SkillEffectHideSelf			= FALSE;
		uimgr->SkillEffectHideOther			= FALSE;
		uimgr->SkillEffectHideCameraShake	= FALSE;
		uimgr->DrawGuide					= TRUE;
		uimgr->DrawChatBubble				= TRUE;
		uimgr->DrawAITip					= TRUE;
		uimgr->DrawWeather					= TRUE;
		uimgr->DrawBloodNumber				= TRUE;
		uimgr->DrawHurt						= FALSE;
		uimgr->DeclineStrangeChat			= FALSE;
		uimgr->DeclineInviteTeam			= FALSE;
		uimgr->DeclineInviteFamily			= FALSE;
		uimgr->DeclineInviteDisciple		= FALSE;
		uimgr->DeclineInviteMaster			= FALSE;
		uimgr->DeclineShowEnemy				= FALSE;
		uimgr->DrawGameFrame				= TRUE;
		uimgr->HintFriendOnline				= TRUE;
		uimgr->HintHideNearPlayer			= TRUE;
		uimgr->HintMarkSelf					= TRUE;
		uimgr->QuestMode					= TRUE;

		uimgr->DeclineShowTrade				= FALSE;
		SaveConfigToServer();
	}
}

void UMMOUIManager::InitUIManager()
{
	UAudioDevice* adioDv = GEngine->GetAudioDevice();
	if( adioDv != NULL ){

		GConfig->GetBool( TEXT("XAudio2.XAudio2Device"), TEXT("VolumeFocus_op"), GIsVolumeInBackground, GEngineIni );


		bSoundAll			= adioDv->VolumeTotal_op;
		bSoundBack			= adioDv->VolumeBackMusic_op;
		bSoundEnv			= adioDv->VolumeEnv_op;
		bSoundVoice			= adioDv->VolumeVoice_op;
		bSoundSfx			= adioDv->VolumeSfx_op;
		bSoundFocusOut		= GIsVolumeInBackground;

		nSoundAllVL			= adioDv->VolumeTotal * 100;
		nSoundBackVL		= adioDv->VolumeBackMusic * 100;
		nSoundEnvVL			= adioDv->VolumeEnv * 100;
		nSoundVoiceVL		= adioDv->VolumeVoice * 100;
		nSoundSfxVL			= adioDv->VolumeSfx * 100;
	}

	//Describes which MoverPlayer the panel belongs to, used to determine the owning moviePlayer before loading
// 	MMOUI::SetPanelToMoviePlayer(Panel_Login_User,		MoviePlayer_NewLogin);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Login_Select,	MoviePlayer_NewLogin);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Login_Create,	MoviePlayer_NewLogin);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Login_Revert,	MoviePlayer_NewLogin);

	//MMOUI::SetPanelToMoviePlayer(Panel_Bag,				MoviePlayer_Bag);
// 	MMOUI::SetPanelToMoviePlayer(Panel_MoneyExchange,	MoviePlayer_Bag);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Quest,			MoviePlayer_Quest);
// 	MMOUI::SetPanelToMoviePlayer(Panel_QuestStory,		MoviePlayer_Quest);
// 	MMOUI::SetPanelToMoviePlayer(Panel_QuestReward,		MoviePlayer_Quest);
// 	MMOUI::SetPanelToMoviePlayer(Panel_QuestItemCollection,		MoviePlayer_Quest);
// 	MMOUI::SetPanelToMoviePlayer(Panel_QuestRewardBuyTimes,		MoviePlayer_Quest);
// 	MMOUI::SetPanelToMoviePlayer(Panel_SoulLevelUp,			MoviePlayer_SoulLevelUp);
// 	MMOUI::SetPanelToMoviePlayer(Panel_SoulEmbed,			MoviePlayer_Soul);
// 	MMOUI::SetPanelToMoviePlayer(Panel_QuestTrace,		MoviePlayer_QuestTrace);
//// 	MMOUI::SetPanelToMoviePlayer(Panel_Death,			MoviePlayer_Death);
	//MMOUI::SetPanelToMoviePlayer(Panel_Chat,			MoviePlayer_Chat);
	//MMOUI::SetPanelToMoviePlayer(Panel_Chat_ChatOption,	MoviePlayer_Chat);

	//	MMOUI::SetPanelToMoviePlayer(Panel_QuestTrace,		MoviePlayer_QuestTrace);

	//MMOUI::SetPanelToMoviePlayer(Panel_Friend,					MoviePlayer_Friend);
	//MMOUI::SetPanelToMoviePlayer(Panel_Friend_FriendChatHint,	MoviePlayer_Friend);
	//MMOUI::SetPanelToMoviePlayer(Panel_Friend_FriendChatLevel,	MoviePlayer_Friend);

// 	MMOUI::SetPanelToMoviePlayer(Panel_HintButtonBar,			MoviePlayer_HintButtonBar);
// 	MMOUI::SetPanelToMoviePlayer(Panel_WorldMap,				MoviePlayer_WorldMap);
// 	MMOUI::SetPanelToMoviePlayer(Panel_UnitedMap,				MoviePlayer_UnitedMap);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Minimap,					MoviePlayer_Minimap);
// 	MMOUI::SetPanelToMoviePlayer(Panel_PathMap,					MoviePlayer_PathMap);
// 	MMOUI::SetPanelToMoviePlayer(Panel_TeleportNet,				MoviePlayer_TeleportNet);

// 	MMOUI::SetPanelToMoviePlayer(Panel_FBEnter_FBEnter,			MoviePlayer_FBEnter);
// 	MMOUI::SetPanelToMoviePlayer(Panel_FBEnter_FBEnterMsg,		MoviePlayer_FBEnter);
// 	MMOUI::SetPanelToMoviePlayer(Panel_FBTask,					MoviePlayer_FBTask);
// 	MMOUI::SetPanelToMoviePlayer(Panel_FBEndAll_FBSummary,		MoviePlayer_FBEndAll);
// 	MMOUI::SetPanelToMoviePlayer(Panel_FBEndAll_FBAward,		MoviePlayer_FBEndAll);
// 	MMOUI::SetPanelToMoviePlayer(Panel_FBEndAll_FBBattleComfirm,MoviePlayer_FBEndAll);

// 	MMOUI::SetPanelToMoviePlayer(Panel_Arena_ArenaEntrance,		MoviePlayer_Arena);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Arena_ArenaState,		MoviePlayer_Arena);
// 
// 	MMOUI::SetPanelToMoviePlayer(Panel_PVP_Arena,					MoviePlayer_PVP);
// 	MMOUI::SetPanelToMoviePlayer(Panel_PVP_PVPBattle,			MoviePlayer_PVP);
// 	MMOUI::SetPanelToMoviePlayer(Panel_PVP_ArenaScore,			MoviePlayer_PVP);
// 
// 
// 	MMOUI::SetPanelToMoviePlayer(Panel_BattleSummary_BattleSummary,		MoviePlayer_BattleSummary);
// 	MMOUI::SetPanelToMoviePlayer(Panel_BattleSummary_BattleScoreBoard,	MoviePlayer_BattleSummary);
// 
// 	MMOUI::SetPanelToMoviePlayer(Panel_FightingScreenHint,		MoviePlayer_FightingScreenHint);
// 	MMOUI::SetPanelToMoviePlayer(Panel_EquipGainHintBox,		MoviePlayer_PopUpHintBox);
// 
// 	MMOUI::SetPanelToMoviePlayer(Panel_EquipEnhance,			MoviePlayer_EquipEnhance);
// 	MMOUI::SetPanelToMoviePlayer(Panel_EquipLevelUp,			MoviePlayer_EquipLevelUp);
// 	MMOUI::SetPanelToMoviePlayer(Panel_MutiLineMsgBox,			MoviePlayer_EquipLevelUp);

// 	MMOUI::SetPanelToMoviePlayer(Panel_Skill,					MoviePlayer_Skill);
// 	MMOUI::SetPanelToMoviePlayer(Panel_SkillSetting,			MoviePlayer_SkillSetting);
// 	MMOUI::SetPanelToMoviePlayer(Panel_SkillTree,				MoviePlayer_SkillTree);
// 	MMOUI::SetPanelToMoviePlayer(Panel_HeavenGasExchang,		MoviePlayer_SkillTree);
// 	MMOUI::SetPanelToMoviePlayer(Panel_HeavenPractise,			MoviePlayer_SkillTree);
// 	MMOUI::SetPanelToMoviePlayer(Panel_WeaponSkill,				MoviePlayer_WeaponSkill);
// 	MMOUI::SetPanelToMoviePlayer(Panel_WeaponSkillSetting,		MoviePlayer_WeaponSkillSetting);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Stall_StallSetting,		MoviePlayer_Stall);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Stall_StallBuy,			MoviePlayer_Stall);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Stall_StallMsg,			MoviePlayer_Stall);


	//MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_Buff,			MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_ProgressBar,	MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_StrengthBar,	MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_FBSummaryProgressBar,	MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_QTEBar,		MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_LevelUp,		MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_FightingScreenHint_ScreenPopUpBoxMsg,	MoviePlayer_FightingScreenHint);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_Scene,			MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_Tips,			MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_Transducer,	MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_BattleState,	MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_WeatherChange,	MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_BossBlood,		MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_NewEvent,		MoviePlayer_DynamicUI);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DynamicUI_UnlockAOGString,		MoviePlayer_DynamicUI);
// 

// 	MMOUI::SetPanelToMoviePlayer(Panel_HudInfo_TargetInfo,		MoviePlayer_HudInfo);
// 	MMOUI::SetPanelToMoviePlayer(Panel_HudInfo_MyInfo,		MoviePlayer_HudInfo);
// 	MMOUI::SetPanelToMoviePlayer(Panel_HudInfo_HeroInfo,		MoviePlayer_HudInfo);
// 	MMOUI::SetPanelToMoviePlayer(Panel_HudInfo_TeamMate,		MoviePlayer_HudInfo);
// 	MMOUI::SetPanelToMoviePlayer(Panel_HudInfo_TeamHint,		MoviePlayer_HudInfo);

	//MMOUI::SetPanelToMoviePlayer(Panel_SimplePlayerInfo,		MoviePlayer_PlayerInfo);
	//MMOUI::SetPanelToMoviePlayer(Panel_PlayerInfo,		MoviePlayer_PlayerInfo);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DurabilityShowing,		MoviePlayer_DurabilityShowing);

	//MMOUI::SetPanelToMoviePlayer(Panel_RClickMenu,				MoviePlayer_RClickMenu);
	//MMOUI::SetPanelToMoviePlayer(Panel_Team,					MoviePlayer_Team);
	//MMOUI::SetPanelToMoviePlayer(Panel_TeamInvite,				MoviePlayer_Team);
	//MMOUI::SetPanelToMoviePlayer(Panel_TeamTarget,				MoviePlayer_Team);
	//MMOUI::SetPanelToMoviePlayer(Panel_TeamAuctionList,			MoviePlayer_TeamAuction);
	//MMOUI::SetPanelToMoviePlayer(Panel_TeamAuction,				MoviePlayer_TeamAuction);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Warehouse,				MoviePlayer_Warehouse);
//// 	MMOUI::SetPanelToMoviePlayer(Panel_DamageLevel,				MoviePlayer_DamageLevel);
// // 	MMOUI::SetPanelToMoviePlayer(Panel_Character,				MoviePlayer_Charactor);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Hero,					MoviePlayer_Hero);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Mount,					MoviePlayer_Mount);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Reputation,				MoviePlayer_Reputation);
// 	MMOUI::SetPanelToMoviePlayer(Panel_TaskPopup,				MoviePlayer_TaskPopup);
// 	MMOUI::SetPanelToMoviePlayer(Panel_OptionMenu,		        MoviePlayer_Option);
// 	MMOUI::SetPanelToMoviePlayer(Panel_OptionConfig,	        MoviePlayer_Option);
// 	MMOUI::SetPanelToMoviePlayer(Panel_OptionQuit,	            MoviePlayer_Option);
// 	//MMOUI::SetPanelToMoviePlayer(Panel_Shop,					MoviePlayer_Shop);
// 	MMOUI::SetPanelToMoviePlayer(Panel_TradeP2P,				MoviePlayer_TradeP2P);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Dialog,					MoviePlayer_Dialog);
// 	MMOUI::SetPanelToMoviePlayer(Panel_DialogNew,				MoviePlayer_DialogNew);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Achievement,				MoviePlayer_Achievement);
// 	MMOUI::SetPanelToMoviePlayer(Panel_AchievementHint,			MoviePlayer_Achievement);
// 	MMOUI::SetPanelToMoviePlayer(Panel_MsgInfo,					MoviePlayer_MsgInfo);
// 	MMOUI::SetPanelToMoviePlayer(Panel_NoticeInfo,					MoviePlayer_NoticeInfo);
//// 	MMOUI::SetPanelToMoviePlayer(Panel_SceneHint,				MoviePlayer_SceneHint);
// 	MMOUI::SetPanelToMoviePlayer(Panel_WeaponExp,				MoviePlayer_WeaponExp);
// 	MMOUI::SetPanelToMoviePlayer(Panel_PickWidget_DorpItemTips, MoviePlayer_PickWidget);
	//MMOUI::SetPanelToMoviePlayer(Panel_MasterStudent,			MoviePlayer_MasterStudent);
	//MMOUI::SetPanelToMoviePlayer(Panel_MasterCongMain,			MoviePlayer_MasterStudent);

	//MMOUI::SetPanelToMoviePlayer(Panel_Mail,						MoviePlayer_Mail);
	//MMOUI::SetPanelToMoviePlayer(Panel_Mail_MailRead,				MoviePlayer_Mail);
// 	MMOUI::SetPanelToMoviePlayer(Panel_BattleTeam_BattleTeamCreate, MoviePlayer_BattleTeam);
// 	MMOUI::SetPanelToMoviePlayer(Panel_BattleTeam,					MoviePlayer_BattleTeam);
// 	MMOUI::SetPanelToMoviePlayer(Panel_PVP_ArenaQueue,				MoviePlayer_PVP);
// 	MMOUI::SetPanelToMoviePlayer(Panel_PVP_ArenaHint,				MoviePlayer_PVP);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Skillbar,					MoviePlayer_Skillbar);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Skillbar_Menubar,			MoviePlayer_Skillbar);
// 	MMOUI::SetPanelToMoviePlayer(Panel_MsgBall_MsgButtonBar,		MoviePlayer_MsgBall);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Skillbar_Buff,				MoviePlayer_Skillbar);
// 
// 	MMOUI::SetPanelToMoviePlayer(Panel_Benchmark,					MoviePlayer_Benchmark);
// 
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_Warehouse,				MoviePlayer_GuildBase);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_Create,				MoviePlayer_GuildBase);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_View,					MoviePlayer_GuildBase);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_Main,					MoviePlayer_GuildBase);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_WarView,				MoviePlayer_GuildBase);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_SabacEnroll,			MoviePlayer_GuildBase);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_Territory,			    MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_Welfare,			    MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_DungeonBegin,			MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_DungeonEnd,			MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_DungeonEnter,			MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_DungeonExit,			MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_WelfareUpgrade,				MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_BuildingCreate,				MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_BuildingUpgradeMulti,			MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_BuildingUpgradeSingle,			MoviePlayer_GuildTerritory);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_WarEnd,				MoviePlayer_GuildWar);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_WarHint,				MoviePlayer_GuildWar);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_WarStart,				MoviePlayer_GuildWar);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Guild_Instruction,			MoviePlayer_GuildBase);
	//MMOUI::SetPanelToMoviePlayer(Panel_UndergroundTrade, 				MoviePlayer_UndergroundTrade);
	//MMOUI::SetPanelToMoviePlayer(Panel_UTSell,									MoviePlayer_UndergroundTrade);
	//MMOUI::SetPanelToMoviePlayer(Panel_UTBid,									MoviePlayer_UndergroundTrade);
	//MMOUI::SetPanelToMoviePlayer(Panel_EquipSlotEnhance,			MoviePlayer_EquipSlotEnhance);
 	//MMOUI::SetPanelToMoviePlayer(Panel_MessageboxZone,				MoviePlayer_MessageBox );
// 	MMOUI::SetPanelToMoviePlayer(Panel_ActivityQuit,				MoviePlayer_Activity);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Activity,					MoviePlayer_Activity);
// 	MMOUI::SetPanelToMoviePlayer(Panel_EquipDecompose,				MoviePlayer_EquipDecompose);
// 	MMOUI::SetPanelToMoviePlayer(Panel_EquipCompose,				MoviePlayer_EquipDecompose);
// 	MMOUI::SetPanelToMoviePlayer(Panel_EquipDecomposeHint,			MoviePlayer_EquipDecompose);
// 
// 	MMOUI::SetPanelToMoviePlayer(Panel_HintCombat_BossQTE,			MoviePlayer_HintCombat);
	//MMOUI::SetPanelToMoviePlayer(Panel_RankList,					MoviePlayer_RankList);
	//MMOUI::SetPanelToMoviePlayer(Panel_RankConfig,				MoviePlayer_RankList);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Mall,						MoviePlayer_Mall);
// 	MMOUI::SetPanelToMoviePlayer(Panel_MallFriend,					MoviePlayer_Mall);
// 	MMOUI::SetPanelToMoviePlayer(Panel_MallBuyGoods,				MoviePlayer_Mall);
// 	MMOUI::SetPanelToMoviePlayer(Panel_MallEquipRole,				MoviePlayer_Mall);

	//MMOUI::SetPanelToMoviePlayer(Panel_AuctionView,				MoviePlayer_Auction);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Vip,						MoviePlayer_Vip);
	//MMOUI::SetPanelToMoviePlayer(Panel_Hurt,				MoviePlayer_Hurt);
	//MMOUI::SetPanelToMoviePlayer(Panel_GoldStore,           MoviePlayer_GoldStore);
// 	MMOUI::SetPanelToMoviePlayer(Panel_KillRecordPanel,				MoviePlayer_KillRecord);
// 	MMOUI::SetPanelToMoviePlayer(Panel_KillCountPanel,				MoviePlayer_KillRecord);
	//MMOUI::SetPanelToMoviePlayer(Panel_ItemStore,            MoviePlayer_ItemStore);
	//MMOUI::SetPanelToMoviePlayer(Panel_GuideOption,			 MoviePlayer_GuideOption);

// 	MMOUI::SetPanelToMoviePlayer(Panel_Helper,				MoviePlayer_Helper);
// 	MMOUI::SetPanelToMoviePlayer(Panel_HelperPlayer,		MoviePlayer_Helper);
// 	MMOUI::SetPanelToMoviePlayer(Panel_ShopMessage,         MoviePlayer_Shop  );
// 	MMOUI::SetPanelToMoviePlayer(Panel_Handbook,			MoviePlayer_Handbook);
// 	MMOUI::SetPanelToMoviePlayer(Panel_Journey,				MoviePlayer_JourneyTrace);
// 	MMOUI::SetPanelToMoviePlayer(Panel_JourneyView,			MoviePlayer_JourneyView);
// 	MMOUI::SetPanelToMoviePlayer(Panel_JourneyComplete,		MoviePlayer_JourneyView);
}

void UMMOUIManager::ShowHurtPanel(UBOOL isShow)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Hurt,isShow, "");
}
void UMMOUIManager::SetPanelShowWithOutTarget(FName PanelName)
{
	UMMOUIBase* UI = FindUIPanel(PanelName);
	if(UI)
	{
		UI->SetShowWithOutTarget();
	}
}


void UMMOUIManager::ShowProgressBar( INT countDownTime,const FString& strTitle, INT type )
{
	MMOUI::GetUIManager()-> ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_ProgressBar,TRUE,"");
	UMMOUIProgressBarWidget* UI = Cast<UMMOUIProgressBarWidget>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_ProgressBar));
	if(UI)
	{
		UI->ShowProgressBar(countDownTime,strTitle,type);
	}
}
void UMMOUIManager::CloseProgressBar(INT closeType)
{	
	UMMOUIProgressBarWidget* UI = Cast<UMMOUIProgressBarWidget>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_ProgressBar));
	if(UI)
	{
		UI->CloseProgressBar(closeType);
		UI->Show(FALSE);
	}
}

void UMMOUIManager::ShowProgressBarStrength(INT time, INT max)
{
	MMOUI::GetUIManager()-> ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_StrengthBar,TRUE,"");
	UMMOUIProgressBarStrengthWidget* UI = Cast<UMMOUIProgressBarStrengthWidget>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_StrengthBar));
	if(UI)
	{
		UI->ShowProgressBarStrength(time,max);
	}
}

void UMMOUIManager::CloseProgressBarStrength(INT closeType)
{
	UMMOUIProgressBarStrengthWidget* UI = Cast<UMMOUIProgressBarStrengthWidget>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_StrengthBar));
	if(UI)
	{
		UI->CloseProgressBarStrength(closeType);
		UI->Show(FALSE);
	}
}

void UMMOUIManager::ShowUnlockAOGString(const FString& content)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_UnlockAOGString,TRUE,"");
	UMMOUIUnlockAOGString* UI = Cast<UMMOUIUnlockAOGString>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_UnlockAOGString));
	if(UI)
	{
		UI->eventCallUCSetRenownUnlockAOGString(content);
	}
}

void UMMOUIManager::HideUnlockAOGString()
{
	UMMOUIUnlockAOGString* UI = Cast<UMMOUIUnlockAOGString>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_UnlockAOGString));
	if(UI)
	{
		UI->eventCallUCPlayHideAnimation();
		UI->Show(FALSE);
	}
}

void UMMOUIManager::ShowProgressBarQTE(INT time,const FString& keyName,const FString& imgName,INT nActorID,INT nAbilityID)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_QTEBar,TRUE,"");
	UMMOUIProgressBarQTEWidget* UI = Cast<UMMOUIProgressBarQTEWidget>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_QTEBar));
	if(UI)
	{
		UI->ShowProgressBarQTE(time, keyName,imgName, nActorID, nAbilityID);
	}
}
void UMMOUIManager::CloseProgressBarQTE(INT closeType)
{
	UMMOUIProgressBarQTEWidget* UI = Cast<UMMOUIProgressBarQTEWidget>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_QTEBar));
	if(UI)
	{
		UI->CloseProgressBarQTE(closeType);
	}
}

void UMMOUIManager::ShowHintLevelUpWidget(const FString& levelUp,INT strengthAdd,INT agilityAdd,	INT intellectAdd,	INT staminaAdd, 	INT spiritAdd,	 INT maxHpAdd )
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_LevelUp,TRUE,"");
	UMMOUIHintLevelUpWidget* UI = Cast<UMMOUIHintLevelUpWidget>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_LevelUp));
	if(UI)
	{
		UI->eventOpen(levelUp, strengthAdd, agilityAdd,	intellectAdd,	staminaAdd, spiritAdd,	 maxHpAdd);
	}
}
void UMMOUIManager::CloseHintLevelUpWidget()
{
	UMMOUIHintLevelUpWidget* UI = Cast<UMMOUIHintLevelUpWidget>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_LevelUp));
	if(UI)
	{
		UI->eventClose();
	}
}

void UMMOUIManager::ShowHintSceneWidget()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, TRUE, "");
	UMMOUIHintSceneWidget* UI = Cast<UMMOUIHintSceneWidget>(FindUIPanel(MMOUI::GetUIManager()->Panel_DynamicUI_Scene));
	if(UI)
	{
		MMOMapDataManager &mapDataManager = MMOMapDataManager::getSingleton();

		//
		AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		if( mainPlayer == NULL ){
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, FALSE, "");
			return;
		}
		INT regionIdx = mapDataManager.GetCurPlayerRegionIn( mainPlayer->Location );
		FInMapRegion *pCurReginInCurMap = mapDataManager.GetReginInCurMap( regionIdx );

		FString hintContent;
		if( pCurReginInCurMap == NULL ){
			warnf(NAME_Error, TEXT("======!!!!======= [UMMOGFXPlayer_DynamicUI::ShowHintSceneWidget] NoMapRegion in CurSceneMap") );
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, FALSE, "");
			return;
		}
		else{
			//update map name
			hintContent = pCurReginInCurMap->DisplayName;
			if( hintContent.IsEmpty() ){
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, FALSE, "");
				return;
			}
		}

		FExportMap &curMapInfo = mapDataManager.GetCurMap();
		UBOOL canChangeLines = ( curMapInfo.ServerMapType == E_est_Line );
		if( canChangeLines == TRUE ){
			FString stringFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Minimap.SceneLineListString") );
			UINT curSceneLineID = 0;
			UINT curSceneServerID = 0;
			UINT curCombinatedSceneID = mapDataManager.GetCurSceneLineIn();
			MMOMapDataManager::ParseServerMapLine( curCombinatedSceneID, curSceneServerID, curSceneLineID );
			hintContent += FString::Printf( *stringFormat, curSceneLineID );
		}

		UI->ShowHintSceneMessage( hintContent, 48 );
	}

}
void UMMOUIManager::CloseHintSceneWidget()
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_Scene, FALSE, "");
}