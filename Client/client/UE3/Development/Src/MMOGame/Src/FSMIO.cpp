#include "StdAfx.h"
#include "FSMState.h"
#include "FSMIO.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "MMONavigationManager.h"
#include "ScaleformEngine.h"
#include "UITextParser.h"
#include "MMOSkillManager.h"
#include "MMOChatManager.h"
#include "Mir2Attack.h"

using namespace FSM;
extern UBOOL g_PreMovetoPoint;
extern FLOAT g_Run_AniRate[EPCT_MAX];
extern FLOAT g_Walk_AniRate[EPCT_MAX];
extern FLOAT g_Run_Oblique_AniRate[EPCT_MAX];
extern FLOAT g_Walk_Oblique_AniRate[EPCT_MAX];
extern FLOAT g_Run_Phase[EPCT_MAX];
extern FLOAT g_Walk_Phase[EPCT_MAX];
extern FLOAT g_Run_Oblique_Phase[EPCT_MAX];
extern FLOAT g_Walk_Oblique_Phase[EPCT_MAX];
extern UBOOL g_ResetPhaseFlag;

void FSMIO_CDTime::ResetCD_SafeZoneDelayTime()
{
	FSM::CDTime.AiInSafeZoneTime = 2.0f;
}

void FSMIO_CDTime::ResetCD_CheckWeaponBindTime(FLOAT CheckTime)
{
	FSM::CDTime.CheckWeaponBindTime = CheckTime;
}

void FSMIO_CDTime::ResetCD_AbilityRespondTime(FLOAT bRespondTime)
{
	FSM::CDTime.AbilityRespondTime  = bRespondTime;
}

void FSMIO_CDTime::ResetCD_SwitchRespondTime(FLOAT bRespondTime)
{
	FSM::CDTime.SwitchRespondTime  = bRespondTime;
}


void FSMIO_CDTime::ResetCD_WalkAndRunDelay(FLOAT fWalkDelayTime,FLOAT fRunDelayTime)
{
	FSM::CDTime.WalkDelayTime  = fWalkDelayTime+0.1f;
	FSM::CDTime.RunDelayTime   = fRunDelayTime+0.1f;
}


void FSMIO_CDTime::ResetCD_MoveDelay(FLOAT fDelayTime)
{
	FSM::CDTime.MoveDelayTime = fDelayTime;
}

void FSMIO_CDTime::ResetCD_MoveDelay_Frame(INT nDelayFrame)
{
	FSM::CDTime.MoveDelayFrame = nDelayFrame;
}

//-------------------------------------------------------------------
UMMOAbility* FSMIO_Ability::GetDefaultAbility()
{
	return  Mir2Attack::getSingleton().GetPrimaryAbility();//  FSM::Ability.GetDefaultAbility();
}

UBOOL FSMIO_Ability::CanUseAnyAbility(class UMMOAbility* InAbility)
{
	if(	InAbility &&
		FSM::Ability.CurrentAbility && 
		FSM::Char.MainPlayer &&		
		FSM::Char.MainPlayer->IsUsingAbility() )
	{
		if(FSM::Ability.CurrentAbility->SpellCommonAttribute.CancelType == E_spclt_None)
		{
			return FALSE;
		}
		else if(FSM::Ability.CurrentAbility->SpellCommonAttribute.CancelType == E_spclt_DependOnValue &&
				InAbility->SpellCommonAttribute.CancelLevel<FSM::Ability.CurrentAbility->SpellCommonAttribute.CancelLevel)
		{
			return FALSE;
		}
		else if(FSM::Ability.CurrentAbility->SpellCommonAttribute.CancelType == E_spclt_DependOnValue &&
				InAbility->SpellCommonAttribute.CancelLevel == FSM::Ability.CurrentAbility->SpellCommonAttribute.CancelLevel )
		{
			if( FSM::Ability.CurrentAbility->SpellCommonAttribute.CancelHardTime &&
				FSM::Ability.CurrentAbility->SpellCommonAttribute.AttackType!=E_spat_Phy  &&
				InAbility->SpellCommonAttribute.AttackType==E_spat_Phy &&
				FSM::Char.MainPlayer->CurrentAbilityState == EAS_Cast )
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}		
	return TRUE;
}

void FSMIO_Ability::SetCurrentAbility(class UMMOAbility* InAbility)
{
	FSM::Ability.CurrentAbility = InAbility;
}

//-------------------------------------------------------------------
void FSMIO_State::ClearAllState()
{
	FSMStateManager::getSingleton().Init();
}

//-------------------------------------------------------------------
AMMOActor*	FSMIO_Target::GetTargetMMOActor()
{
	return FSM::Target.CurTargetMMOActor;
}
AMMOActor*	FSMIO_Target::GetDesireMMOActor()
{
	return FSM::Target.DesireMMOActor;
}
AMMOActor*	FSMIO_Target::GetAttackMMOActor()
{
	return FSM::Target.AttackMMOActor;
}
AMMOActor*	FSMIO_Target::GetSelectMMOActor()
{
	return FSM::Target.SelectMMOActor;
}
AActor*		FSMIO_Target::GetDesireActor()
{
	return FSM::Target.DesireActor;
}

void FSMIO_Target::SetSelectMMOActor(AActor* Actor)
{
	FSM::Target.SelectMMOActor = Cast<AMMOActor>(Actor);
}

void FSMIO_Target::SetDesirePos(FVector Pos, INT NPCTempleteID)
{
	FSM::Trigger.IsDesirePosTrigger = TRUE;
	FSM::Target.DesirePos = Pos;
	FSM::Target.DesireNPCTempleteID = NPCTempleteID;

	SetDesireActor(NULL);
}

void FSMIO_Target::SetDesireActor_FromAbility()
{
	if(FSM::Target.CurTargetMMOActor && FSM::Target.CurTargetMMOActor->GetActorType()!=EACT_NPC)
	{
		FSM::Target.AbilityTargetMMOActor = FSM::Target.CurTargetMMOActor;
	}
}

void FSMIO_Target::SetDesireActor(AActor* Target)
{
	FSM::Target.LastDesireMMOActor = FSM::Target.DesireMMOActor;
	FSM::Target.DesireActor		= Target;
	FSM::Target.DesireMMOActor	= Cast<AMMOActor>(Target);
	FSM::Target.DesireMMOPlayer	= Cast<AMMOPlayer>(Target);
	FSM::Target.DesireNPC		= Cast<ANPC>(Target);
	FSM::Target.DesireInteractableActor = ScriptInterfaceCast<IIInteractableActor>(FSM::Target.DesireActor);
	if(FSM::Target.DesireInteractableActor && FSM::Target.DesireInteractableActor->CanInteract()==FALSE)
	{
		FSM::Target.DesireInteractableActor = ScriptInterfaceCast<IIInteractableActor>(NULL);
		//SetDesireActor(NULL);
	}

	if(FSM::Target.DesireNPC && FSM::Target.DesireNPC->Template==NULL)
	{
		FSM::Target.DesireActor		= NULL;
		FSM::Target.DesireMMOActor	= NULL;
		FSM::Target.DesireMMOPlayer	= NULL;
		FSM::Target.DesireNPC		= NULL;
		FSM::Target.DesireInteractableActor = ScriptInterfaceCast<IIInteractableActor>(NULL);
	}

	FSM::CDTime.FocusTime = 0.001;

	//if(FSM::Target.DesireMMOActor && FSM::Target.LastDesireMMOActor && FSM::Target.DesireActor!=FSM::Target.LastDesireMMOActor)
	//{
	//	//FSM::Target.DesireMMOActorChangeFlag = TRUE;
	//}
	//else
	//{
	//	//FSM::Target.DesireMMOActorChangeFlag = FALSE;
	//}
	if(FSM::Target.DesireMMOActor==NULL&&FSM::Target.LastDesireMMOActor&&FSM::Char.Controller)
	{
		FSM::Char.Controller->eventClearAllCmd();
	}

	AMMOPlayer* MainPlayer  =	MMOFacade::getSingleton().GetMainPlayer();
	AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
	if(MainPlayer==NULL || Controller==NULL)return;

	FSM::Target.DesireAttackble= MMOFacade::getSingleton().IsAttackable(MainPlayer,FSM::Target.DesireMMOActor);
	if(FSM::Target.DesireAttackble)
	{
		FSM::Target.AttackMMOActor = FSM::Target.DesireMMOActor;
	}
	else
	{
		FSM::Target.AttackMMOActor = NULL;
	}


	FSM::Target.SelectMMOActor =  FSM::Target.DesireMMOActor;
	Controller->SetSelectTarget(FSM::Target.SelectMMOActor);

	if(FSM::Target.DesireMMOActor==NULL)
	{
		if(Controller->TargetLocationMarker){
			Controller->TargetLocationMarker->Hide();
		}
	}

	if(FSM::Target.LastDesireMMOActor != FSM::Target.DesireMMOActor){
		if(FSM::Target.DesireMMOActor){
			FSM::CDTime.FocusTime = -1.0;
			Controller->SendRequestSetFocus(FSM::Target.DesireMMOActor->ServerID);
		}

		MMOGameEvent_TargetEvent evt(MMOGameEvent_TargetEvent::ETCT_DesiredTarget, FSM::Target.CurTargetMMOActor, FSM::Target.SelectMMOActor);
		MMOUI::PostGameEvent(&evt);
	}
}

void FSMIO_Target::SetUITargetActor(INT nUITargetMMOActorID)
{
	FSM::Target.UITargetMMOActorID = nUITargetMMOActorID;
}

void FSMIO_Target::ClearDesireActor()
{
	FSM::Target.LastDesireMMOActor = FSM::Target.DesireMMOActor;
	FSM::Target.AttackMMOActor = FSM::Target.DesireMMOActor = NULL;
	//FSM::Ctrl.IsBattleState = FALSE;	//关闭战斗状态
	if(FSM::Target.LastDesireMMOActor != FSM::Target.DesireMMOActor){
		MMOGameEvent_TargetEvent evt(MMOGameEvent_TargetEvent::ETCT_DesiredTarget, FSM::Target.CurTargetMMOActor, FSM::Target.SelectMMOActor);
		MMOUI::PostGameEvent(&evt);
	}
}

void FSMIO_Target::UpdateDesireAbility(int slotId, int abilityId)
{
	if(FSM::Ability.DesireAbility && FSM::Ability.DesireAbility->GetTemplateID()==abilityId )
	{
		SetDesireAbility( MMOSkillManager::getSingleton().GetEquippedSkill( slotId ) );
	}
}

UMMOAbility* FSMIO_Target::GetDesireAbility()
{
	return FSM::Ability.DesireAbility;
}

void FSMIO_Target::SetDesireFClassAbility(UMMOAbility* Ability,UBOOL bAddOrRemove)
{
	if(bAddOrRemove){
		FSM::Ability.DesireFClassAbility = Ability;
	}
	else if(FSM::Ability.DesireFClassAbility==Ability)
	{
		FSM::Ability.DesireFClassAbility = NULL;
	}
	//防止技能升级后，id相同而level不同的情况
	else if (FSM::Ability.DesireFClassAbility && Ability && FSM::Ability.DesireFClassAbility->SpellCommonAttribute.Id == Ability->SpellCommonAttribute.Id)
	{
		FSM::Ability.DesireFClassAbility = NULL;
	}
}

void FSMIO_Target::SetCacheAbility(UMMOAbility* Ability)
{
	if(Ability)
	{
		if(Ability->SpellCommonAttribute.CastType==E_spct_Switch)
		{
			return;
		}

		if(Ability->SpellCommonAttribute.NeedCache)
		{
			FSM::Ability.CacheAbility = Ability;
		}
		else if(Ability->SpellCommonAttribute.AttackType==E_spat_Mag || Ability->SpellCommonAttribute.AttackType==E_spat_Tao )
		{
			if(FSM::Ability.CurrentAbility && FSM::Char.MainPlayer->IsUsingAbility() && FSM::Ability.CurrentAbility->SpellCommonAttribute.AttackType==E_spat_Phy)
			{
				FSM::Ability.CacheAbility = Ability;
			}
			else
			{
				FSM::Ability.CacheAbility = NULL;
			}
		}
		else
		{
			FSM::Ability.CacheAbility = NULL;
		}

		//if(Ability->SpellCommonAttribute.NeedCache)
		//{
		//	FSM::Ability.CacheAbility = Ability;
		//}
		//else
		//{
		//	FSM::Ability.CacheAbility = NULL;
		//}
	}
	else{
		FSM::Ability.CacheAbility = NULL;
	}
}

void FSMIO_Target::SetDesireAbility(UMMOAbility* Ability)
{
	if(Ability==NULL)
	{
		FSM::Ability.CurrentAbility = NULL;
		return;
	}

	if(FSM::Char.Controller==NULL)
	{
		return;
	}

	if(FSM::Ability.DesireFClassAbility && Mir2Attack::getSingleton().InPrimaryAbility(Ability->SpellCommonAttribute.Id) )
	{
		return;
	}

	if(FSM::Ability.DesireAbility && Ability )
	{
		if(Ability->SpellCommonAttribute.CancelType==E_spclt_DependOnValue && Ability->SpellCommonAttribute.CancelLevel>FSM::Ability.DesireAbility->SpellCommonAttribute.CancelLevel)
		{
			FSM::Char.Controller->WaitingForTheAbilityBuf = -1.0f;
			FSM::Char.Controller->WaitingForTheAbilityRespond = 0;
			FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(-1.0f);
		}
		else if(Ability->SpellCommonAttribute.CancelType==E_spclt_Instant)
		{
			FSM::Char.Controller->WaitingForTheAbilityBuf = -1.0f;
			FSM::Char.Controller->WaitingForTheAbilityRespond = 0;
			FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(-1.0f);
		}
	}
	FSM::Ability.DesireAbility = Ability;
	if(FSM::Ability.DesireAbility && FSM::Ability.DesireAbility->bCoolDown)
	{
		UBOOL   Res=TRUE;
		FString cs=FString::Printf(TEXT("MMOGame.Ability.InCD_%d_%d"),FSM::Ability.DesireAbility->SpellCommonAttribute.Id,FSM::Ability.DesireAbility->SpellCommonAttribute.RuneSlotID );
		FString screenMsg = MMOUI::ParseLocalizedPropertyPath(cs, Res);
		if(Res){
			MMOUI::GetUIManager()->delegateAddScreenMessage( screenMsg , MsgInfoType_Warning , TEXT("MMOGame.Ability.InCD") );
			MMOChatManager::getSingleton().AddSystemMessage(screenMsg);
		}
		else
		{
			//FString screenMsg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Ability.InCD_Default"));
			//MMOUI::GetUIManager()->delegateAddScreenMessage( screenMsg , MsgInfoType_Warning , TEXT("MMOGame.Ability.InCD") );
			//MMOChatManager::getSingleton().AddSystemMessage(screenMsg);
		}
	}

}

void FSMIO_Target::ClearDesireAbility(UMMOAbility* Ability)
{
	//结束需要结束的技能
	AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
	if( Ability && Controller)
	{
		if(Ability->ClientCommonAttribute.bRequestCancelOnRelease)
		{
			Controller->eventAddCmdCancelAbility(Ability);
		}
	}
	FSM::Ability.DesireAbility =	FSMIO::getSingleton().Ctrl.GetLeftBtnBindAbility(FALSE); // NULL; 
	FSM::Ability.UsableAbility =	FSMIO::getSingleton().Ctrl.GetLeftBtnBindAbility(FALSE); // NULL;
	FSM::Ctrl.KeyType = KT_PRIMARY;
	//FSM::Ctrl.IsBattleState = FALSE;	//关闭战斗状态
}

void	FSMIO_Target::SetTargetActor(AActor* TargetActor)
{
	FSM::Target.SetTargetActor(TargetActor);
	if(FSM::Target.CurTargetMMOActor){
		FSM::CDTime.FocusTime = 0.001;
	}
}

void	FSMIO_Target::SetDropActor(AActor* TargetActor)
{
	SetDesireActor(TargetActor);
	FSM::Ctrl.state_Interact= TRUE;
	FSM::Ctrl.state_Switching_Flag = TRUE;
}

void	FSMIO_Target::ClearActor(AActor* Actor)
{
	FSM::Target.ClearActor(Actor);
}

void FSMIO_Target::SetDesireTarget(UMMOAbility* Ability,AActor* Target)
{
	if(FSM::Ctrl.state_Attack){
		warnf(TEXT("-------SetDesireTarget "));
		g_PreMovetoPoint = FALSE;
		//FSMIO::getSingleton().Trigger.SetWaitingSendMove(TRUE);
		//CMessageHandler::GetInstance().Send_Clear_Spell_Cache();
	}

	//AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
	//if(Controller && Controller->TargetLocationMarker)
	//{
	//	Controller->TargetLocationMarker->Hide();
	//}
	g_ResetPhaseFlag=TRUE;

	//if(Ability && Ability->ClientCommonAttribute.GenerateLockStatus){
	//	if(FSM::Target.CurTargetMMOActor){
	//		SetDesireActor(FSM::Target.CurTargetMMOActor);
	//	}
	//}


	if(FSM::Ctrl.KeyType == KT_PRIMARY)
	{
		if(FSM::Target.CurTargetMMOActor){
			SetDesireActor(FSM::Target.CurTargetMMOActor);
			//FSM::Char.Controller->eventClearAllCmd();
		}
	}

	if(Ability && Ability->ClientCommonAttribute.GenerateLockStatus){
		//if(FSM::Target.CurTargetMMOActor && FSM::Ctrl.KeyType == KT_PRIMARY){
		//	SetDesireActor(FSM::Target.CurTargetMMOActor);
		//}

		if(Ability && Ability->ClientCommonAttribute.TargetSelectActor && FSM::Ctrl.KeyType != KT_PRIMARY)
		{
			SetDesireActor_FromAbility();
		}
	}


	
	SetDesireAbility(Ability);

	//MMONavigationManager::getSingleton().Stop();
}

void FSMIO_Target::ClearDesireTarget(UMMOAbility* Ability) 
{
	//if(Ability&&Ability->ClientCommonAttribute.bRequestCancelOnRelease)
	//{

	//}
	//else{
	//	return;
	//}



	if(Ability&&Ability->ClientCommonAttribute.bRequestCancelOnRelease)
	{

	}
	//else if(FSM::Ctrl.state_Attack && GetAttackMMOActor())
	else if(GetAttackMMOActor())
	{
		return;
	}
	//ClearDesireActor();
	ClearDesireAbility(Ability);
}


void FSMIO_Target::SetInputText()
{
	UMMOAbility* pDesireAbility = FSMIO::getSingleton().Target.GetDesireAbility();
	if(pDesireAbility )
	{
		FSMIO::getSingleton().Target.ClearDesireTarget(pDesireAbility);
	}
	FSMIO::getSingleton().Target.SetDesireFClassAbility(FSM::Ability.DesireFClassAbility,FALSE);
}

void FSMIO_Target::SetKeepRunning(UBOOL bRunning)
{
	FSM::Ctrl.ctrlKeepRunning = bRunning;
	if(FSM::Ctrl.ctrlKeepRunning && FSM::Char.MainPlayer)
	{
		FSM::Ctrl.vecRotation = FSM::Char.MainPlayer->Rotation.Vector().SafeNormal();
		FSM::Ctrl.vecRotation.Z = 0.0;
	}
}

void FSMIO_Target::ToggleKeepRunning()
{
	FSM::Ctrl.ctrlKeepRunning = !FSM::Ctrl.ctrlKeepRunning ;
	if(FSM::Ctrl.ctrlKeepRunning && FSM::Char.MainPlayer)
	{
		FSM::Ctrl.vecRotation = FSM::Char.MainPlayer->Rotation.Vector().SafeNormal();
		FSM::Ctrl.vecRotation.Z = 0.0;
	}
}

//-------------------------------------------------------------------
//UMMOAbility* FSMIO_Input::OnLeftMouseKey(UBOOL bRelease )
//{
//	UIUICommand* cmd;
//	UMMOAbility* ToUseAbility=NULL;
//	AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
//	if(Controller==NULL)return ToUseAbility;
//	
//
//	if( MMOFacade::getSingleton().GCTType == GC_DIABLO3 )
//	{
//		cmd = Controller->KeyMapManager->UICmdList(ECK_Slot1).Cmd;
//		if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
//		{
//			ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
//		}
//		
//		if(bRelease && FSM::Target.CurTargetMMOActor==NULL )
//		{
//			Controller->ShowTargetLocationMarker();
//		}
//	}
//	else if( MMOFacade::getSingleton().GCTType == GC_LOL )
//	{
//		if(!bRelease)FSMIO::getSingleton().Ctrl.SetSelectCtrl(TRUE);
//	}
//	else
//	{
//		cmd = Controller->KeyMapManager->UICmdList(ECK_Slot1).Cmd;
//		if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
//		{
//			ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
//		}
//		FSMIO::getSingleton().Ctrl.SetActionCtrl(!bRelease);
//	}
//	return ToUseAbility;
//}

//UMMOAbility* FSMIO_Input::OnRightMouseKey(UBOOL bRelease)
//{
//	UIUICommand* cmd;
//	UMMOAbility* ToUseAbility=NULL;
//	AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
//	if(Controller==NULL)return ToUseAbility;
//
//	if( MMOFacade::getSingleton().GCTType == GC_DIABLO3 )
//	{
//		cmd = Controller->KeyMapManager->UICmdList(ECK_Slot2).Cmd;
//		if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
//		{
//			ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
//		}
//	}
//	else if( MMOFacade::getSingleton().GCTType == GC_LOL )
//	{
//		cmd = Controller->KeyMapManager->UICmdList(ECK_Slot1).Cmd;
//		if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
//		{
//			ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
//		}
//		if(!bRelease){
//			FSMIO::getSingleton().Ctrl.SetActionCtrl(TRUE);
//			FSMIO::getSingleton().Ctrl.SetSelectCtrl(FALSE);
//		}
//		if(bRelease && FSM::Target.CurTargetMMOActor==NULL )
//		{
//			Controller->ShowTargetLocationMarker();
//		}
//
//	}
//	else
//	{
//		FSMIO::getSingleton().Ctrl.SetMoveCtrl(!bRelease);
//	}
//	return ToUseAbility;
//}



//-------------------------------------------------------------------
void FSMIO_Ctrl::StartNavigation()
{
	FSM::Ctrl.state_Attack = FALSE;
	FSM::Ctrl.state_Interact = FALSE;
	FSM::Ctrl.state_Moving   = FALSE;
	FSM::Ctrl.state_AiDesire = FALSE;
}


void  FSMIO_Ctrl::SetAlwaysWalk(UBOOL bFlag)
{
	FSM::Ctrl.state_AlwaysWalk = bFlag;
}
UBOOL FSMIO_Ctrl::GetAlwaysWalk()
{
	return FSM::Ctrl.state_AlwaysWalk;
}

void  FSMIO_Ctrl::SetAlwaysWalkBase(UBOOL bFlag)
{
	FSM::Ctrl.state_AlwaysWalkBase = bFlag;
}
UBOOL FSMIO_Ctrl::GetAlwaysWalkBase()
{
	return FSM::Ctrl.state_AlwaysWalkBase;
}

void  FSMIO_Ctrl::SetGuildBattle(UBOOL bActive)
{
	FSM::Ctrl.IsBattleState = bActive;
}

UBOOL FSMIO_Ctrl::GetGuildBattle()
{
	return FSM::Ctrl.IsBattleState;
}

void  FSMIO_Ctrl::SetKeyType(KEY_TYPE KeyType)
{
	FSM::Ctrl.KeyType = KeyType;
	if(FSM::Ctrl.KeyType!=KT_NONE)
	{
		FSM::Ctrl.KeyType_Keeping = FSM::Ctrl.KeyType;
	}
}

UMMOAbility* FSMIO_Ctrl::GetLeftBtnBindAbility(UBOOL bRelease )
{
	return Mir2Attack::getSingleton().GetPrimaryAbility();


	//UIUICommand* cmd;
	//UMMOAbility* ToUseAbility=NULL;
	//AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
	//if(Controller==NULL)return ToUseAbility;

	//cmd = Controller->KeyMapManager->UICmdList(ECK_NormalAtk).Cmd;
	//if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
	//{
	//	ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
	//}

	////if( MMOFacade::getSingleton().GCTType == GC_DIABLO3 )
	////{
	////	//FSM::Ctrl.KeyType = KT_PRIMARY;
	////	cmd = Controller->KeyMapManager->UICmdList(ECK_Slot1).Cmd;
	////	if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
	////	{
	////		ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
	////	}

	////	if(bRelease && FSM::Target.CurTargetMMOActor==NULL )
	////	{
	////		Controller->ShowTargetLocationMarker();
	////	}
	////}
	////else if( MMOFacade::getSingleton().GCTType == GC_LOL )
	////{
	////	//FSM::Ctrl.KeyType = KT_SECONDARY;
	////}
	////else if( MMOFacade::getSingleton().GCTType == GC_MIR2 )
	////{
	////	//FSM::Ctrl.KeyType = KT_PRIMARY;
	////	cmd = Controller->KeyMapManager->UICmdList(ECK_NormalAtk).Cmd;
	////	//cmd = Controller->KeyMapManager->UICmdList(ECK_Slot1).Cmd;
	////	
	////	if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
	////	{
	////		ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
	////	}
	////}
	////else if( MMOFacade::getSingleton().GCTType == GC_MIRFOREVER )
	////{
	////	//FSM::Ctrl.KeyType = KT_SECONDARY;

	////}

	////FSM::Ctrl.KeyType_Keeping = FSM::Ctrl.KeyType;
	//return ToUseAbility;
}


UMMOAbility* FSMIO_Ctrl::GetRightBtnBindAbility(UBOOL bRelease)
{
	//UIUICommand* cmd;
	UMMOAbility* ToUseAbility=NULL;
	AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
	if(Controller==NULL)return ToUseAbility;


	//if( MMOFacade::getSingleton().GCTType == GC_DIABLO3 )
	//{
	//	FSM::Ctrl.KeyType = KT_SECONDARY;
	//	cmd = Controller->KeyMapManager->UICmdList(ECK_Slot2).Cmd;
	//	if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
	//	{
	//		ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
	//	}
	//}
	//else if( MMOFacade::getSingleton().GCTType == GC_LOL )
	//{
	//	FSM::Ctrl.KeyType = KT_PRIMARY;
	//	cmd = Controller->KeyMapManager->UICmdList(ECK_Slot1).Cmd;
	//	if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
	//	{
	//		ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
	//	}		
	//}
	//else if( MMOFacade::getSingleton().GCTType == GC_MIR2 )
	//{
	//	FSM::Ctrl.KeyType = KT_SECONDARY;
	//}
	//else if( MMOFacade::getSingleton().GCTType == GC_MIRFOREVER )
	//{
	//	FSM::Ctrl.KeyType = KT_PRIMARY;
	//	//cmd = Controller->KeyMapManager->UICmdList(ECK_Slot1).Cmd;
	//	cmd = Controller->KeyMapManager->UICmdList(ECK_NormalAtk).Cmd;
	//	
	//	if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
	//	{
	//		ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
	//	}
	//}

	//FSM::Ctrl.KeyType_Keeping = FSM::Ctrl.KeyType;

	return ToUseAbility;
}

void FSMIO_Ctrl::AbilityKeyDown()
{
	UBOOL bNeedUpdateKeyInfo = FALSE;
	if(FSM::Ctrl.KeyType == KT_PRIMARY)
	{
		PrimaryKeyDown();
		bNeedUpdateKeyInfo = TRUE;
	}
	else if(FSM::Ctrl.KeyType == KT_SECONDARY)
	{
		SecondaryKeyDown();
	}
	else
	{
		bNeedUpdateKeyInfo = TRUE;
	}
	FSM::Ctrl.state_Switching_Flag = TRUE;

	//// for test
	//if (bNeedUpdateKeyInfo)
	//{
	//	UMMOAbility* ToUseAbility = FSM::Ability.DesireAbility;
	//	if (ToUseAbility)
	//	{
	//		UBOOL bInCD = FALSE;
	//		UBOOL bNotEnoughMp = FALSE;
	//		if (ToUseAbility->bCoolDown/* || ToUseAbility->ClientCommonAttribute.ModifyCD >= 0.0001*/)
	//		{
	//			bInCD = TRUE;
	//		}

	//		if (ToUseAbility->CanUseManaCheck(MMOFacade::getSingleton().GetMainActor()) == FALSE)
	//		{
	//			bNotEnoughMp = TRUE;
	//		}
	//		FVector HitLoc = MMOFacade::getSingleton().GetMainController()->MouseHitWorldLocation;
	//		ConvertCoordC2N(HitLoc);

	//		warnf(TEXT("------------------------Send_SpellRequestInfo"));
	//		CMessageHandler::GetInstance().Send_SpellRequestInfo(
	//			ToUseAbility->SpellCommonAttribute.Id, 
	//			MMOFacade::getSingleton().GetMainActor()->GetMp(),
	//			Max(ToUseAbility->CoolDownInfo.GroupTimeLeft * 1000, ToUseAbility->CoolDownInfo.SpellTimeLeft),
	//			(FSM::Target.AttackMMOActor ? FSM::Target.AttackMMOActor->ServerID : 0),
	//			(INT32)HitLoc.X, (INT32)HitLoc.Y, (INT32)HitLoc.Z,
	//			bNotEnoughMp, 
	//			bInCD, 
	//			1000 * appSeconds(), 
	//			FSM::Input.ShiftDown
	//			);
	//	}
	//}
}

void FSMIO_Ctrl::AbilityKeyUp()
{
	if(FSM::Ctrl.KeyType == KT_PRIMARY)
	{
		PrimaryKeyUp();
	}
	else if(FSM::Ctrl.KeyType == KT_SECONDARY)
	{
		SecondaryKeyUp();
	}
	else
	{
		OtherKeyUp();
	}

	

	if(FSM::Char.Controller && FSM::Char.Controller->bRightMousePressed)
	{
		SetKeyType(KT_SECONDARY);
	}
	else if(FSM::Char.Controller && FSM::Char.Controller->bLeftMousePressed)
	{
		SetKeyType(KT_PRIMARY);
	}
	else{
		SetKeyType(KT_NONE);
	}
	FSM::Ctrl.state_Switching_Flag = FALSE;
}

void FSMIO_Ctrl::SetInputText()
{
	if(FSM::Ctrl.KeyType == KT_ELSE)
	{
		OtherKeyUp();
		SetKeyType(KT_PRIMARY);
		FSM::Ctrl.state_Switching_Flag = FALSE;
		FSM::Ability.UsableAbility = NULL;
	}
}

void FSMIO_Ctrl::OtherKeyUp()
{
	if(MMOFacade::getSingleton().GCTType == GC_DIABLO3)
	{
		FSM::Ctrl.state_Moving	= FALSE;
		FSM::Ctrl.state_Attack	= FALSE;
		FSM::Ctrl.state_Interact= FALSE;
		FSM::Ctrl.state_AiDesire= FALSE;
	}
	else if(MMOFacade::getSingleton().GCTType == GC_LOL)
	{
		FSM::Ctrl.state_Moving	= FALSE;
	}
	//else if(MMOFacade::getSingleton().GCTType == GC_MIR2)
	//{
	//	FSM::Ctrl.state_Moving	= FALSE;
	//	FSM::Ctrl.state_Attack	= FALSE;
	//}
	else if(MMOFacade::getSingleton().GCTType == GC_MIRFOREVER || MMOFacade::getSingleton().GCTType == GC_MIR2)
	{
		if(FSM::Target.DesireActor && FSM::Target.AttackMMOActor)
		{
			AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
			UIUICommand* cmd = Controller->KeyMapManager->UICmdList(ECK_NormalAtk).Cmd;
			debugf(NAME_Log, TEXT("[AtkDiag] with-target attack: cmd=%s isAbility=%d"), cmd ? *cmd->GetName() : TEXT("NULL"), (cmd && cmd->IsA(UUICmd_Ability::StaticClass())) ? 1 : 0);
			if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
			{
				UMMOAbility* ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
				debugf(NAME_Log, TEXT("[AtkDiag]   bound ability=%s id=%d"), ToUseAbility ? *ToUseAbility->GetName() : TEXT("NULL"), ToUseAbility ? ToUseAbility->eventGetAbilityId() : -1);
				FSMIO::getSingleton().Target.SetDesireAbility(ToUseAbility);
			}
			else
			{
				FSM::Ctrl.state_Moving	= FALSE;
				FSM::Ctrl.state_Attack	= FALSE;			
			}
		}
		else
		{
			AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
			if(Controller && Controller->bLeftMousePressed)
			{
				UIUICommand* cmd = Controller->KeyMapManager->UICmdList(ECK_NormalAtk).Cmd;
				// TEMP-DIAG(1.05 backport): normal attack binding
				debugf(NAME_Log, TEXT("[AtkDiag] left-click attack: cmd=%s isAbility=%d"), cmd ? *cmd->GetName() : TEXT("NULL"), (cmd && cmd->IsA(UUICmd_Ability::StaticClass())) ? 1 : 0);
				if( cmd != NULL && cmd->IsA( UUICmd_Ability::StaticClass()) )
				{
					UMMOAbility* ToUseAbility = ( Cast<UUICmd_Ability>(cmd) )->eventGetBindAbilityData();
					debugf(NAME_Log, TEXT("[AtkDiag]   bound ability=%s id=%d"), ToUseAbility ? *ToUseAbility->GetName() : TEXT("NULL"), ToUseAbility ? ToUseAbility->eventGetAbilityId() : -1);
					FSMIO::getSingleton().Target.SetDesireAbility(ToUseAbility);
				}
			}
			else if(Controller && Controller->bRightMousePressed)
			{
				FSM::Ctrl.state_Moving	= TRUE;
				FSM::Ctrl.state_Attack	= FALSE;
			}
			else
			{
				FSM::Ctrl.state_Moving	= FALSE;
				FSM::Ctrl.state_Attack	= FALSE;
			}
		}
	}
}

void FSMIO_Ctrl::PrimaryKeyDown()
{
	FSM::Ctrl.state_Moving	= FALSE;
	FSM::Ctrl.state_Attack	= FALSE;
	FSM::Ctrl.state_Interact= FALSE;
	FSM::Ctrl.state_AiDesire= FALSE;


	AMMODoodadActor* doodad = Cast<AMMODoodadActor>(FSM::Target.CurTargetActor);
	if(doodad && doodad->GetMouseCursor()==0)
	{
		//隐形doodod,无法选中
	}
	else
	{
		FSMIO::getSingleton().Target.SetDesireActor(FSM::Target.CurTargetActor);
	}

	//FSMIO::getSingleton().Target.SetDesireActor(FSM::Target.CurTargetActor);

	if(FSM::Target.DesireAttackble)
	{
		if(FSM::Target.DesireMMOPlayer)
		{
			FSM::Ctrl.state_Attack	= FALSE;
		}
		else{
			FSM::Ctrl.state_Attack	= TRUE;
		}
	}
	else if(FSM::Target.DesireInteractableActor || FSM::Target.DesireMMOActor)
	{
		FSM::Ctrl.state_Interact= TRUE;
	}
	else
	{
		if(MMOFacade::getSingleton().GCTType == GC_MIR2)
		{
			FSM::Ctrl.state_Moving	= TRUE;
		}
		//FSM::Ctrl.state_Moving	= TRUE;
	}


	if(MMOFacade::getSingleton().GCTType == GC_DIABLO3)
	{
		FSM::Ctrl.state_AiDesire = FSM::Ctrl.state_Attack;
	}
}


void FSMIO_Ctrl::PrimaryKeyUp()
{
	if(MMOFacade::getSingleton().GCTType == GC_DIABLO3)
	{
		AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
		if(FSM::Ctrl.state_Moving && Controller)
		{
			Controller->ShowTargetLocationMarker();
		}

		FSM::Ctrl.state_Moving	= FALSE;
		FSM::Ctrl.state_Attack	= FALSE;
		FSM::Ctrl.state_Interact= FALSE;
		FSM::Ctrl.state_AiDesire= FALSE;
	}
	else if(MMOFacade::getSingleton().GCTType == GC_LOL)
	{
		AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
		if(FSM::Ctrl.state_Moving && Controller)
		{
			Controller->ShowTargetLocationMarker();
		}
		FSM::Ctrl.state_Moving	= FALSE;
	}
	else if(MMOFacade::getSingleton().GCTType == GC_MIR2 )
	{
		AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
		if(FSM::Ctrl.state_Moving && Controller)
		{
			Controller->ShowTargetLocationMarker();
		}
		FSM::Ctrl.state_Moving	= FALSE;
	}
	else if(MMOFacade::getSingleton().GCTType == GC_MIRFOREVER )
	{
		AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
		if(FSM::Ctrl.state_Moving && Controller)
		{
			Controller->ShowTargetLocationMarker();
		}
		FSM::Ctrl.state_Moving	= FALSE;
	}
}

void FSMIO_Ctrl::SecondaryKeyDown()
{
	FSM::Ctrl.state_Moving	= FALSE;
	FSM::Ctrl.state_Attack	= FALSE;
	FSM::Ctrl.state_Interact= FALSE;
	FSM::Ctrl.state_AiDesire= FALSE;
	if(MMOFacade::getSingleton().GCTType == GC_DIABLO3)
	{
		//D3里面的右键，用来施放技能
		FSM::Ctrl.state_Attack	= TRUE;
	}
	else if(MMOFacade::getSingleton().GCTType == GC_LOL)
	{
		//LOL里的左键，用来选择
	}
	else if(MMOFacade::getSingleton().GCTType == GC_MIR2)
	{
		//传奇里的右键，用来走路
		FSM::Ctrl.state_Moving	= TRUE;
	}
	else if(MMOFacade::getSingleton().GCTType == GC_MIRFOREVER)
	{
		//传奇永恒里的左键，用来走路
		//FSM::Ctrl.state_Moving	= TRUE;

		//舒工再次口谕后的取消
		////舒工口谕后的改动，
		//if(	(FSM::Target.DesireInteractableActor || FSM::Target.DesireMMOActor)&&
		//	FSM::Target.DesireMMOActor==FSM::Target.CurTargetMMOActor &&
		//	FSM::Target.DesireAttackble==FALSE)
		//{
		//	FSM::Ctrl.state_Interact= TRUE;
		//}
		//else
		{
			FSM::Ctrl.state_Moving	= TRUE;
		}
	}

}
void FSMIO_Ctrl::SecondaryKeyUp()
{

	if(MMOFacade::getSingleton().GCTType == GC_MIR2)
	{
		AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
		if(FSM::Ctrl.state_Moving && Controller)
		{
			Controller->ShowTargetLocationMarker();
		}
	}
	if(MMOFacade::getSingleton().GCTType == GC_MIRFOREVER)
	{
		AMMOPlayerController* Controller	=	MMOFacade::getSingleton().GetMainController();
		if(FSM::Ctrl.state_Moving && Controller)
		{
			Controller->ShowTargetLocationMarker();
		}
	}

	FSM::Ctrl.state_Moving	= FALSE;
	FSM::Ctrl.state_Attack	= FALSE;
	FSM::Ctrl.state_Interact= FALSE;
	FSM::Ctrl.state_AiDesire= FALSE;	
}

void FSMIO_Ctrl::UpdateMoving()
{
	//在移动状态下走路
	if( FSM::Ctrl.state_Moving			&&
		FSM::Input.ShiftDown==FALSE		&&
		FSM::Input.CtrlBtnDown==FALSE	&&
		FSM::Char.Controller			&&
		FSM::Char.MainPlayer			&&
		FSM::Char.MoveEnableInAdvance	&&
		FSM::Char.IsDead==FALSE	)
	{
		warnf(TEXT("------------------------UpdateMoving"));
		FSM::Char.Controller->SendRequest_MoveTo(	FSM::Target.CurTargetPos ,TRUE);
		FSMIO::getSingleton().Trigger.SetWaitingSendMove(TRUE);
	}
	else
	{
		FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
		if (FSM::Char.Controller->TargetLocationMarker)
		{
			FSM::Char.Controller->TargetLocationMarker->Hide();
		}
	}
}

void FSMIO_Trigger::SetWaitingSendMove(UBOOL bFlag)
{
	FSM::Trigger.IsWaitingSendMove = bFlag;
	FSM::Trigger.DelayTimeWaitingSendMove = 1.0;
}

UBOOL FSMIO_Trigger::GetWaitingSendMove()
{
	return FSM::Trigger.IsWaitingSendMove;
}

void  FSMIO_Trigger::SetShowTips(UBOOL bShow, UBOOL bForce)
{
	if(bForce)
	{
		FSM::Target.ShowUITipsFlag = bShow;
	}
	else if(FSM::Target.CaptureInput && bShow)
	{
		FSM::Target.ShowUITipsFlag = bShow;
	}
	else
	{
		FSM::Target.ShowUITipsFlag = FALSE;
	}
}


