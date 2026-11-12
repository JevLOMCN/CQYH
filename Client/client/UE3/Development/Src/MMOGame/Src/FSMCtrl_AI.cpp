

#include "StdAfx.h"
#include "FSMState.h"
#include "FSMCtrl_AI.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "MMONavigationManager.h"
#include "ScaleformEngine.h"
#include "UITextParser.h"
#include "FSMIO.h"
#include "DoodadManager.h"
#include "Mir2Attack.h"
#include "MMODropManager.h"
using namespace FSM;


//FSM control class registration--------------------------------------------------------------------------------------------------
ADD_FSM_CTRL(FSMCtrl_KeepRunning)
ADD_FSM_CTRL(FSMCtrl_SearchDoodad)
ADD_FSM_CTRL(FSMCtrl_AIInSafeZone)
ADD_FSM_CTRL(FSMCtrl_ShowPlayerInfo)
ADD_FSM_CTRL(FSMCtrl_ShowHintBtn)




UBOOL FSMCtrl_KeepRunning::Update(FLOAT DelTime)
{
	if( FSM::Ctrl.ctrlKeepRunning &&
		FSM::Ctrl.state_Attack == FALSE &&
		FSM::Ctrl.state_Moving == FALSE &&
		FSM::Ctrl.state_Interact == FALSE &&
		//FSM::Ctrl.state_Standby &&
		FSM::Char.Controller			&&
		FSM::Char.MainPlayer			&&
		FSM::Char.MainPlayer->isMoving==FALSE &&
		FSM::Char.MoveEnableInAdvance	&&
		FSM::CDTime.MoveDelayTime<=0.0f  &&
		FSM::CDTime.MoveDelayFrame<=0 &&
		FSM::Char.IsDead==FALSE			)
	{
		return TRUE;
	}

	return FALSE;
}
void FSMCtrl_KeepRunning::Action()
{
	UBOOL bWaitingSendMove = FSMIO::getSingleton().Trigger.GetWaitingSendMove();
	if(bWaitingSendMove==FALSE)
	{
		if(FSM::CDTime.RunDelayTime<=0.0f)
		{	
			gServerGridWorld.SetDynamicBLock(TRUE);
			FSM::Char.Controller->eventClearAllCmd();
			//FSM::Char.Controller->eventAddCmdMoveToPoint(FSM::Char.MainPlayer->Location+FSM::Ctrl.vecRotation*256.0,TRUE,FALSE);
			FVector dest = FSM::Target.CurOptimizeTargetPos;
			FVector dis = dest-FSM::Char.MainPlayer->Location;
			dis.Z = 0.0;
			if(dis.X>-64.0f && dis.X<64.0f && dis.Y>-64.0f && dis.Y<64.0f)
			{
				FSM::Char.Controller->eventAddCmdMoveToPoint(dest,TRUE,FALSE);
			}
			else if(dis.X>-256.0f && dis.X<256.0f && dis.Y>-256.0f && dis.Y<256.0f)
			{
				dis.Normalize();
				dest = FSM::Char.MainPlayer->Location + dis*342.0f;
				FSM::Char.Controller->eventAddCmdMoveToPoint(dest,TRUE,FALSE);
			}
			else
			{

				FSM::Char.Controller->eventAddCmdMoveToPoint(dest,TRUE,FALSE);
			}
		}
	}
	return;
}

//Search nearby doodad, and interact------------------------------------------------------------------------------------------------------
UBOOL FSMCtrl_SearchDoodad::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_SearchDoodad");

	if( FSM::Ctrl.state_Switching_Flag &&
		FSM::Ctrl.state_Interact ==FALSE  &&
		FSM::Ctrl.state_Attack   ==FALSE  &&
		FSM::Ability.DesireAbility  &&
		FSM::Target.DesireMMOActor == NULL &&
		FSM::Input.ShiftDown==FALSE	&&
		FSM::Input.CtrlBtnDown==FALSE&&
		FSM::Input.AltBtnDown==TRUE&&
		FSM::Char.MoveEnable		&&
		FSM::Char.Controller		&&
		FSM::Char.MainPlayer		&&
		FSM::Char.IsDead==FALSE		)
	{
		FSM::Ctrl.state_Switching_Flag = FALSE;
		return TRUE;
	}
	return FALSE;
}

void FSMCtrl_SearchDoodad::Action()
{
	INT DoodadId = FSM::Target.OptimizeDoodadID(FSM::Char.MainPlayer->Location,FSM::Ability.DesireAbility->SpellCommonAttribute.SearchDistance);
	if(DoodadId>0)
	{
		AMMOActor* pDoodad =  DoodadManager::getSingleton().FindDoodadActor(DoodadId);
		if(pDoodad)
		{
			FSM::Ctrl.state_Interact= TRUE;
			FSM::Ctrl.state_Moving  = FALSE;
			FSM::Ctrl.state_Switching_Flag = TRUE;
			FSMIO::getSingleton().Target.SetDesireActor(pDoodad);
			FSMIO::getSingleton().Target.SetDesireTarget(FSM::Ability.DesireAbility,pDoodad);
		}
	}
}

extern INT g_SerialID;
extern INT g_LastRequestAbilitySerialId;
extern INT g_LastRequestAbilityId;
//Enter/leave safe zone performance Ai----------------------------------------------------
UBOOL FSMCtrl_AIInSafeZone::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_AIInSafeZone");

	if( FSM::CDTime.AiInSafeZoneTime<=0.0f  &&
		FSM::Target.AttackMMOActor==NULL	&&
		FSM::Input.ShiftDown==FALSE	&&
		FSM::Char.MainPlayer		&&
		FSM::Char.Controller		&&
		FSM::Char.MoveEnable		&&
		FSM::Char.IsDead==FALSE		)
	{
		return TRUE;
	}

	//Check whether to cast ability 4000 or 4001
	if(	FSM::Ctrl.IsWantToSendRequest4000Or4001Ability &&
		FSM::Char.Controller		&&
		FSM::Char.MainPlayer)
	{
		if( FSM::Char.MainPlayer->GetWeaponType()< EWT_Riding_1 &&
			FSM::Char.MainPlayer->CanUseAnyAbility() &&
			FSM::Char.MainPlayer->IsUsingAbility()==FALSE &&
			FSM::Char.Controller->bIsOperatingDoodad==FALSE	)
		{
			FSM::Char.Controller->WaitingForTheCombatPose = -1.0f;
			if(FSM::Char.MainPlayer->ActorAttributes->byCombatPoseOn){
				CMessageHandler::GetInstance().SendRequestReviveAbility(4000,FSM::Char.MainPlayer->ServerID,   FSM::Char.MainPlayer->Location);
				FSM::Char.Controller->WaitingForTheAbilityRespond = 4000;

				FSM::Char.MainPlayer->CurrentAbilityState = EAS_Requested;
				g_LastRequestAbilitySerialId = FSM::Char.MainPlayer->CurrentRequestAbilitySerialId;
				g_LastRequestAbilityId = FSM::Char.MainPlayer->CurrentRequestAbilityId;
				FSM::Char.MainPlayer->CurrentRequestAbilitySerialId = g_SerialID;
				FSM::Char.MainPlayer->CurrentRequestAbilityId = 4000;
				g_SerialID = (++g_SerialID) % 256;

				FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(0.2f);
			}
			else
			{
				CMessageHandler::GetInstance().SendRequestReviveAbility(4001,FSM::Char.MainPlayer->ServerID,   FSM::Char.MainPlayer->Location);
				FSM::Char.Controller->WaitingForTheAbilityRespond = 4001;

				FSM::Char.MainPlayer->CurrentAbilityState = EAS_Requested;
				g_LastRequestAbilitySerialId = FSM::Char.MainPlayer->CurrentRequestAbilitySerialId;
				g_LastRequestAbilityId = FSM::Char.MainPlayer->CurrentRequestAbilityId;
				FSM::Char.MainPlayer->CurrentRequestAbilitySerialId = g_SerialID;
				FSM::Char.MainPlayer->CurrentRequestAbilityId = 4001;
				g_SerialID = (++g_SerialID) % 256;

				FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(0.2f);
			}
			FSM::Ctrl.IsWantToSendRequest4000Or4001Ability=FALSE;
		}
		return FALSE;
	}

	return FALSE;
}

void FSMCtrl_AIInSafeZone::Action()
{
	if(MMO::GetCurrentGameState() == EGameState_InGame){//In game
		FSMIO::getSingleton().CDTime.ResetCD_SafeZoneDelayTime();
		if(	FSM::Char.MainPlayer->IsInSafeZone && FSM::Char.MainPlayer->ActorAttributes->byCombatPoseOn==FALSE )
		{
			CMessageHandler::GetInstance().Send_CombatPoseOn(TRUE,1);
		}
		else if(FSM::Char.MainPlayer->IsInSafeZone==FALSE && FSM::Char.MainPlayer->ActorAttributes->byCombatPoseOn)
		{
			CMessageHandler::GetInstance().Send_CombatPoseOn(FALSE,1);
		}
	}
}
//Quick view player info----------------------------------------------------
UBOOL FSMCtrl_ShowPlayerInfo::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_ShowPlayerInfo");

	//Ctrl+walk key, quick view the specified player
	if( FSM::Ctrl.state_Switching_Flag  &&
		FSM::Ctrl.KeyType == KT_SECONDARY &&
		//FSM::Ability.DesireAbility==FALSE &&
		FSM::Input.ShiftDown==FALSE		&&
		FSM::Input.AltBtnDown==FALSE    &&
		FSM::Input.CtrlBtnDown==TRUE	&&
		//(FSM::Ctrl.state_Moving==TRUE || FSM::Ctrl.state_Interact== TRUE)	&&	
		FSM::Target.CurTargetMMOPlayer  &&
		FSM::Char.Controller			&&
		FSM::Char.MainPlayer			&&
		FSM::Char.IsDead==FALSE			)
	{
		return TRUE;
	}
	return FALSE;
}

void FSMCtrl_ShowPlayerInfo::Action()
{
	FSM::Ctrl.state_Switching_Flag = FALSE;
	FSM::Ctrl.state_Attack = FALSE;
	FSM::Ctrl.state_Moving = FALSE;
	FSM::Ctrl.state_Interact = FALSE;

	UMMOUIPlayerInfo* playerInfo = Cast<UMMOUIPlayerInfo>( MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_PlayerInfo ) );
	if(playerInfo && FSM::Target.CurTargetMMOPlayer)
	{
		FSMIO::getSingleton().Target.SetDesireAbility(NULL);
		playerInfo->InitPlayerRoleId( FSM::Target.CurTargetMMOPlayer->ServerID );
		playerInfo->Show( TRUE );
	}

}

//Player overhead name button----------------------------------------------------
UBOOL FSMCtrl_ShowHintBtn::Update(FLOAT DelTime)
{
	CTRL_TICK_F("FSMCtrl_ShowHintBtn");

	if( FSM::Input.AltBtnDown_Last!=FSM::Input.AltBtnDown ||
		FSM::Input.CtrlBtnDown_Last!=FSM::Input.CtrlBtnDown)
	{
		if(FSM::Input.AltBtnDown)
		{
			if(FSM::Input.CtrlBtnDown)
			{
				_bShowHintBtn=FALSE;
				return TRUE;
			}
			else
			{
				_bShowHintBtn=TRUE;
				return TRUE;
			}
		}
		else if(FSM::Input.AltBtnDown_Last!=FSM::Input.AltBtnDown)
		{
			_bShowHintBtn=FALSE;
			return TRUE;
		}
	}
	return FALSE;
}

void FSMCtrl_ShowHintBtn::Action()
{

	UBOOL bCurDropShow = MMODropManager::getSingleton().GetShowDropHud();
	if( bCurDropShow != _bShowHintBtn ){
		MMODropManager::getSingleton().SetShowDropHud( _bShowHintBtn );
	}

	//

	UMMOUIManager* UM = MMOUI::GetUIManager();
	UMMOUISkillBar* skb = Cast<UMMOUISkillBar>(UM->FindUIPanel(UM->Panel_Skillbar));
	if( skb && skb->IsSkillSettingBtnShow != _bShowHintBtn ){
		skb->eventUCToggleShowSkillShortcutButtons( _bShowHintBtn );

		//
		UM->delegateShowHintBtn( _bShowHintBtn );

		//
		AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		if( pc != NULL && !_bShowHintBtn ){
			pc->eventOnAltKeyPressed( _bShowHintBtn );
		}	
	}

}
