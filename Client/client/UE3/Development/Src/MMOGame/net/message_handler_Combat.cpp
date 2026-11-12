#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "AbilityProcessor.h"
#include "MMOBuffManager.h"
#include "CommonGameEvents.h"
#include "FSMIO.h"
#include "MMOAchievementManager.h"
//#include "MMOScreenMsgMgr.h"
#include "error_code_name.h"
#include "MMOChatManager.h"
#include "MMOEquipManager.h"
#include "MMOSkillManager.h"
#include "MMOHurtManager.h"
#include "UITextParser.h"
#include "MMODeathManager.h"
#include "MMOQuestManager.h"
#include "MMOPowerManager.h"
#include "MMOGuideManager.h"
#include "MMOFacade.h"
#include "Mir2Attack.h"
#include "MMOClientVariableManager.h"

using namespace ExternalMessage;
extern INT g_moveModGridSize;
extern INT g_netLagToleranceDisatnce;

UBOOL GetSkillPanel( UMMOUISkillPanel **InPtr )
{
	static UMMOUISkillPanel* pPanel = 0;
	if( !pPanel )
		pPanel = ( UMMOUISkillPanel* )MMOUI::FindUIPanel ( "Skill" );
	*InPtr = pPanel;
	if( *InPtr )
		return TRUE;
	else
		return FALSE;
}

//void CMessageHandler::SendRequestUseAbility(INT SlotId)
//{
//	////MMOTargetManager::getSingleton().ResetSafeZoneDelayTime();	
//	//FSMIO::getSingleton().CDTime.ResetCD_SafeZoneDelayTime();
//	//C2G_ROLE_PRECAST_SPELL msg;
//	//msg.wMsg = c2g_role_precast_spell;
//	//msg.bySpellSlot = SlotId;
//	//_Send(&msg, sizeof(msg));
//}

//void CMessageHandler::Send_RequestUseAbility(UINT32 TargetId, UINT16 AbilityId, UINT8 Level, FVector& Point, UINT8 byRotateToTarget)
//{
//	warnf(NAME_Warning, TEXT("====== RequestUseAbility AbilityId[%d] -> TargetId[%d]"),
//		AbilityId, TargetId);
//
//	//debugf(L"OOOOOOOOOOO %d,%d,%d dir=%d", AimX, AimY, AimZ, AimDir);
//
//	MMOFacade::getSingleton().GetMainController()->ResetMoving();//TryingMoveDest = FVector(0,0,0);
//	MMOFacade::getSingleton().UpdateTimeStamp();
//	//MMOTargetManager::getSingleton().ResetSafeZoneDelayTime();	
//	FSMIO::getSingleton().CDTime.ResetCD_SafeZoneDelayTime();
//
//	C2G_ROLE_CAST_SPELL msg;
//	ZeroMemory(&msg, sizeof(msg));
//	msg.wMsg = c2g_role_cast_spell;
//	//msg.dwTimeStamp = MMOFacade::getSingleton().GetCurrentTimeStamp();
//	msg.bySpellSlot = AbilityId;
//	msg.dwTargetID = TargetId;
//	//msg.wSpellLevel = 1;    // todo
//	msg.byRotateToTarget = byRotateToTarget;
//	FVector P = Point;
//	ConvertCoordC2N(P);
//	msg.wTargetX = P.X;
//	msg.wTargetY = P.Y;
//	msg.wTargetZ = P.Z;
//	_Send(&msg, sizeof(msg));
//
//	//debugIDf( TEXT("Network"), L"Cast Spell TimeStamp=%d", msg.dwTimeStamp);
//}

void CMessageHandler::Send_RequestSwitchSpell(UINT16 SpellId)
{
	//warnf(NAME_Warning, TEXT("====== Send_RequestSwitchSpell SpellId[%d]"),SpellId);

	C2G_CAST_SWITCH_SPELL msg;
	ZeroMemory(&msg, sizeof(msg));
	msg.wMsg = c2g_cast_switch_spell;
	msg.wSpellID = SpellId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_RequestUseAbility_Ex(UINT32 TargetId, UINT16 SpellId, UINT8 Level, FVector& Point, UINT8 byRotateToTarget, UINT SerialID)
{
	warnf(TEXT("====== Send_RequestUseAbility_Ex SpellId[%d] -> TargetId[%d] ->SerialID[%d],,,CurrentAbilityState[%d]"),
		SpellId, TargetId, SerialID, MMOFacade::getSingleton().GetMainPlayer()->CurrentAbilityState);

	MMOFacade::getSingleton().GetMainController()->ResetMoving();//TryingMoveDest = FVector(0,0,0);
	MMOFacade::getSingleton().UpdateTimeStamp();
	FSMIO::getSingleton().CDTime.ResetCD_SafeZoneDelayTime();

	C2G_CAST_SPELL_EX msg;
	ZeroMemory(&msg, sizeof(msg));
	msg.wMsg = c2g_cast_spell_ex;
	msg.wSpellID = SpellId;
	msg.dwTargetID = TargetId;
	msg.byRotateToTarget = byRotateToTarget;
	msg.bySerial = SerialID;
	FVector P = Point;
	ConvertCoordC2N(P);
	msg.wTargetX = P.X;
	msg.wTargetY = P.Y;
	msg.wTargetZ = P.Z;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::SendRequestCancelAbility(INT SpellId)
{
	warnf(TEXT("====== SendRequestCancelAbility"));

	C2G_ROLE_CANCEL_CAST msg;
	msg.wMsg = c2g_role_cancel_cast;
//	msg.dwTimeStamp = MMOFacade::getSingleton().GetCurrentTimeStamp();
	msg.wSpellID = SpellId;// .bySlotID = SlotId;
	_Send(&msg, sizeof(msg));
}

//add by JimZhang@snda.com send revive message------------------------------------------------------------
void CMessageHandler::SendRequestReviveAbility( UINT16 AbilityId ,UINT32 dwTargetID,FVector& Point)
{
	AMMOPlayerController *pCtrl = MMOFacade::getSingleton().GetMainController();
	if(pCtrl && pCtrl->bNeedCommitPosition ){
		//MMOTargetManager::getSingleton().ResetSafeZoneDelayTime();	
		FSMIO::getSingleton().CDTime.ResetCD_SafeZoneDelayTime();

		C2G_DIRECTLY_CAST_SPELL revive;
		ZeroMemory(&revive, sizeof(revive));
		revive.wMsg = c2g_directly_cast_spell; 
	//	revive.dwTimeStamp	=MMOFacade::getSingleton().GetCurrentTimeStamp();
		revive.dwSpellID	= AbilityId;
		revive.bySpellLevel	= 0;
		revive.dwTargetID	= dwTargetID;
		revive.wTargetX		= Point.X;
		revive.wTargetY		= Point.Y;
		revive.wTargetZ		= Point.Z;
		_Send(&revive, sizeof(revive));
	}
}

//----------------------------------------------------------------------------
void CMessageHandler::Send_SelectFocus( INT ServeId)
{
	C2G_SELECT_FOCUS msg;
	msg.wMsg = c2g_select_focus;
	msg.dwFocusID = ServeId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_EquipSpell( INT SlotId,INT SpellId )
{
	/*
	struct C2G_EQUIP_SPELL: MESSAGE_HEADER
	{
	UINT8			bySlot;
	UINT16			wSpellID;
	};
	*/
	C2G_EQUIP_SPELL msg;
	msg.wMsg = c2g_equip_spell;
	//qssSlotCount reserved for QTE
	msg.bySlot = SlotId ;
	msg.wSpellID = SpellId;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Parse_g2c_char_prepare_spell(const char* buf, UINT32 size)
{
	G2C_CHAR_PREPARE_SPELL* msg = (G2C_CHAR_PREPARE_SPELL*)buf;
	FVector Pos = FVector(msg->wTargetX, msg->wTargetY, msg->wTargetZ);
	ConvertCoordN2C(Pos);

	MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvPrepareSpell(msg->dwCasterID, msg->wSpellID, msg->bySpellLevel, msg->byRuneSlot, msg->dwTargetID, Pos, msg->wWeaponSpeedFactor, msg->wAtkSpeedFactor, msg->bySerial);
}


void CMessageHandler::Parse_g2c_char_precast_spell(const char* buf, UINT32 size)
{
	//warnf(TEXT("-------Parse_g2c_char_precast_spell "));
	G2C_CHAR_PRECAST_SPELL* msg = (G2C_CHAR_PRECAST_SPELL*)buf;
	MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvSelectTarget(msg->dwCasterID, msg->wSpellID, msg->bySpellLevel, msg->byRuneSlot);
}

void CMessageHandler::Parse_g2c_char_cast_spell(const char* buf, UINT32 size)
{
	//warnf(TEXT("-------Parse_g2c_char_cast_spell "));

	G2C_CHAR_CAST_SPELL* msg = (G2C_CHAR_CAST_SPELL*)buf;
	MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvCastSpell(msg);
}

void CMessageHandler::Parse_g2c_char_break_cast(const char* buf, UINT32 size)
{
	//warnf(TEXT("-------Parse_g2c_char_break_cast "));
	G2C_CHAR_BREAK_CAST* msg = (G2C_CHAR_BREAK_CAST*)buf;
	MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvBreakCast(msg);
}

void CMessageHandler::Parse_g2c_role_cast_over(const char* buf, UINT32 size)
{
	G2C_ROLE_CAST_OVER* msg = (G2C_ROLE_CAST_OVER*)buf;
	warnf(TEXT("-------Parse_g2c_role_cast_over %d spellID= %d"),msg->bySerial,msg->wSpellID);
	Mir2Attack::getSingleton().SendUseAbility(msg->wSpellID);
	MMOFacade::getSingleton().GetAbilityProcessor()->OnMainRoleEndAbility(msg->wSpellID, msg->bySerial);
}

void CMessageHandler::Parse_g2c_char_ex_cast_spell(const char* buf, UINT32 size)
{
	//warnf(TEXT("-------Parse_g2c_char_ex_cast_spell "));
	G2C_CHAR_EX_CAST_SPELL* msg = (G2C_CHAR_EX_CAST_SPELL*)buf;
	MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvExCast(msg);
}

void CMessageHandler::Parse_g2c_char_spell_effect(const char* buf, UINT32 size)
{
	G2C_CHAR_SPELL_EFFECT* msg = (G2C_CHAR_SPELL_EFFECT*)buf;
	MMOFacade::getSingleton().GetAbilityProcessor()->OnRecvEffect(msg->SpellCastInfo.dwCasterID, msg->SpellCastInfo.wSpellID, msg->SpellCastInfo.bySpellLevel, msg->SpellCastInfo.byRuneSlot, msg->dwTargetID, msg->nParam1, msg->shParam2, msg->wFlag, msg->byIsMissileEffect, msg->SpellCastInfo.bySerial, msg->SpellCastInfo.byCastID, msg->byEffectSerial);
}

void CMessageHandler::Parse_g2c_combat_pose_switch( const char* buf, UINT32 size )
{
	G2C_COMBAT_POSE_SWITCH* msg = (G2C_COMBAT_POSE_SWITCH*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter( msg->dwCharID );
	AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter( msg->dwCharID ));
	if(player && player->ActorAttributes)
	{
		player->ActorAttributes->byCombatPoseOn = msg->byCombatPoseOn;	
		//Force immediate switch
		if(msg->byClientFlag==0 || player->GetWeaponType()>= EWT_Riding_1)
		{
			player->RefreshCombatPose();
			return;
		}
	}

	if(player && player->IsMainPlayer())
	{
		if(msg->byClientFlag!=0)
		{
			FSM::Ctrl.IsWantToSendRequest4000Or4001Ability=TRUE;
		}

		//if(msg->byClientFlag!=0 && player->GetWeaponType()< EWT_Riding_1){
		//	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
		//	if(ctrl && player->ServerID==msg->dwCharID){
		//		ctrl->WaitingForTheCombatPose = -1.0f;
		//		if(msg->byCombatPoseOn){
		//			CMessageHandler::GetInstance().SendRequestReviveAbility(4000,player->ServerID,   player->Location);
		//			ctrl->WaitingForTheAbilityRespond = 4000;
		//			FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(0.2f);
		//		}
		//		else
		//		{
		//			CMessageHandler::GetInstance().SendRequestReviveAbility(4001,player->ServerID,   player->Location);
		//			ctrl->WaitingForTheAbilityRespond = 4001;
		//			FSMIO::getSingleton().CDTime.ResetCD_AbilityRespondTime(0.2f);
		//		}
		//	}		
		//}

	}
}

void CMessageHandler::Parse_g2c_directly_hp_change_effect( const char* buf, UINT32 size )
{
	G2C_DIRECTLY_HP_CHANGE_EFFECT* msg = (G2C_DIRECTLY_HP_CHANGE_EFFECT*)buf;
	
	AMMOHud* hud = MMOFacade::getSingleton().GetMyHud();
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter( msg->dwCharID );
	AMMOActor* player = MMOFacade::getSingleton().GetMainPlayer();
	if (!hud || !actor || !player)
	{
		return;
	}

	FDamageResult dmg(EC_EventParm);
	dmg.HPChange = msg->nHpChange;
	if (msg->nHpChange > 0)
	{
		dmg.Type = DRT_Result0;
	}
	else
	{
		dmg.Type = DRT_Result6;
	}
	dmg.MainRole = (player->ServerID == actor->ServerID);
	if (dmg.MainRole)
	{
		hud->AddHitStats(dmg, actor->Location);
	}

	//if( msg->nHpChange > 0 )
	//{				
	//	FDamageResult dmg(EC_EventParm);
	//	dmg.HPChange = msg->nHpChange;
	//	dmg.Type = DRT_Result0;
	//	dmg.MainRole = ( player->ServerID == actor->ServerID );
	//	hud->AddHitStats( dmg , actor->Location );
	//}
	//else
	//{
	//	FDamageResult dmg(EC_EventParm);
	//	dmg.HPChange = msg->nHpChange;
	//	dmg.Type = DRT_Result6;
	//	dmg.MainRole = ( player->ServerID == actor->ServerID );
	//	hud->AddHitStats( dmg , actor->Location );
	//}
}

void CMessageHandler::Parse_g2c_sync_aura_list(const char* buf, UINT32 size)
{
	G2C_SYNC_AURA_LIST* msg = (G2C_SYNC_AURA_LIST*)buf;
	INT byCount = (size - sizeof(G2C_SYNC_AURA_LIST)) / sizeof( AURA_DATA );

	AMMOActor* player = MMOFacade::getSingleton().GetMainPlayer();
	if(player==NULL)return;

	MMOFacade::getSingleton().GetMMOBuffManager()->ClearAllBuff();
	for ( int i=0; i< byCount ; ++i )
	{
		AURA_DATA* pData = &msg->astAuraData[i];
		MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(NULL, player->ServerID, pData->dwInstanceID, pData->wID, 0, pData->dwRemainDuration, pData->dwMaxDuration, pData->byStackCount);
	}	

	MMOGameEvent_OneInt evt( GameEvent_Refresh_SkillSlotLoop, 0 );
	MMOUI::PostGameEvent(&evt);
}

void CMessageHandler::Parse_g2c_sync_aura_brief(const char* buf, UINT32 size)
{
	G2C_SYNC_AURA_BRIEF* msg = (G2C_SYNC_AURA_BRIEF*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if (!actor)
	{
		return;
	}

	for (INT i = 0; i < adgTotal; ++i)
	{
		if (msg->BriefData[i].wTemplateID == 0)
		{
			continue;
		}

		MMOFacade::getSingleton().GetMMOBuffManager()->AddNewBuff(0, msg->dwCharID, msg->BriefData[i].dwInstanceID, msg->BriefData[i].wTemplateID, 1, -1, -1, 1);
	}

}

void CMessageHandler::Parse_g2c_sync_pet_level(const char* buf, UINT32 size)
{
	G2C_SYNC_PET_LEVEL* msg = (G2C_SYNC_PET_LEVEL*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwNpcID);
	if (!actor)
	{
		return;
	}

	actor->ActorAttributes->PetLevel=msg->byLevel;
	G2C_SYNC_NPC_EX_DATA* CacheData = m_NPCDataCache.GetCacheData(msg->dwNpcID);
	//G2C_SYNC_NPC_DATA* CacheData = m_NPCDataCache.GetCacheData(msg->dwNpcID);
	if (CacheData != NULL)
	{
		CacheData->byPetLevel = msg->byLevel;
	} 

	MMOFacade::getSingleton().UpdatePetLevelColor(actor);
	MMOUI::GetUIManager()->delegateUpdateHintText(actor,ESHU_ALL);
}

void CMessageHandler::Parse_g2c_aura_effect(const char* buf, UINT32 size)
{
	G2C_AURA_EFFECT* msg = (G2C_AURA_EFFECT*)buf;
	AMMOHud* hud = MMOFacade::getSingleton().GetMyHud();
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter( msg->dwTargetID );
	AMMOActor* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !hud || !actor )
	{
		return;
	}
	if( msg->nEffectValue > 0 )
	{				
		FDamageResult dmg(EC_EventParm);
		dmg.HPChange = msg->nEffectValue;
		dmg.Type = DRT_Result0;
		dmg.MainRole = ( player->ServerID == actor->ServerID );
		hud->AddHitStats( dmg , actor->Location );
	}
	else
	{
		FDamageResult dmg(EC_EventParm);
		dmg.HPChange = msg->nEffectValue;
		dmg.Type = DRT_Result6;
		dmg.MainRole = ( player->ServerID == actor->ServerID );
		UBOOL MainRoleCaster = ( msg->dwCasterID == player->ServerID );
		if(dmg.MainRole || MainRoleCaster){
			hud->AddHitStats( dmg , actor->Location );
		}
		//Update damage display
		MMOHurtManager::getSingleton().UpdateAllHurt(msg->dwCasterID,msg->nEffectValue);
	}

	//msg->dwInstanceID
}

void CMessageHandler::Parse_g2c_equip_spell(const char* buf, UINT32 size)
{
	//Confirm ability Slot switch succeeded
	G2C_EQUIP_SPELL* msg = (G2C_EQUIP_SPELL*)buf;
	//if( msg->nErrCode != errSuccess )
	//{
	//	//static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);

	//	//FString enumVal = InEnum->GetEnum(msg->nErrCode).ToString();

	//	FString enumVal(g_szErrorCodeNameList[msg->nErrCode]);

	//	debugf( TEXT("[CMessageHandler::Parse_g2c_equip_spell] : %d,%d"),
	//		msg->bySlot, *enumVal );
	//	return;
	//}

	AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		warnf(L"can not find the main player when to init ability list");
		return;
	}

	AMMOPlayerController *pc = MMOFacade::getSingleton().GetMainController();
	if( pc )
	{
		INT skillMainID = msg->wSpellID;
		INT skillRunSlotID = msg->byRuneSlot;
		INT skillLevel = msg->bySpellLevel;
//		if( skillRunSlotID > 0 && msg->byRuneLevel > 0 ){
			//This is the inscription ability
//			skillLevel = ( msg->byRuneLevel < msg->bySpellLevel ? msg->byRuneLevel : msg->bySpellLevel );
//		}

		//UMMOAbility* learnedSkill = mainPlayer->GetAbilityById( skillMainID, skillRunSlotID );
		//if( learnedSkill != NULL ){
		//	skillLevel = learnedSkill->SpellCommonAttribute.Level;
		//}
		warnf(NAME_Warning, TEXT("======!!!!======= [Parse_g2c_equip_spell Slot:%d SkillID:%d SkillLevel:%d SkillRuneID:%d]") , msg->bySlot, skillMainID, skillLevel, skillRunSlotID );
		pc->EquipNewSpellAndAutoUnequipOld( msg->bySlot, skillMainID, skillLevel, skillRunSlotID, false );
	}

	MMOGameEvent_EquipSkill evt( GameEvent_EquipSkill, msg->bySlot, msg->wSpellID );
	MMOUI::PostGameEvent(&evt);

	MMOGameEvent_OneInt evt2( GameEvent_ChangeSkillSlot, msg->bySlot );
	MMOUI::PostGameEvent(&evt2);

	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_EquipSkill);
	MMOUI::PostGameEvent(&evtGuide);
}

//Ability experience
void CMessageHandler::Parse_g2c_sync_spell_pts(const char* buf, UINT32 size)
{
	G2C_SYNC_SPELL_PTS* msg = (G2C_SYNC_SPELL_PTS*)buf;
	if( msg == NULL ){
		return;
	}

	MMOSkillManager::getSingleton().SetMySkillExp( msg->wSpellID, msg->wPts );

	MMOGameEvent_OneInt evt( GameEvent_LearnedSkillExpChanged, msg->wSpellID );
	MMOUI::PostGameEvent(&evt);
}

//Ability slot unlock, responds after level up
void CMessageHandler::Parse_g2c_unlock_spell_slot(const char* buf, UINT32 size)
{
	G2C_UNLOCK_SPELL_SLOT* msg = (G2C_UNLOCK_SPELL_SLOT*)buf;
    //Notify to play slot unlock notification
    int slotId = msg->bySlot;
   
	//Notify ability panel to prepare
	MMOGameEvent_OneInt evt( GameEvent_UnlockSkillSlot, slotId );
	MMOUI::PostGameEvent(&evt);

    AMMOPlayerController *pc = MMOFacade::getSingleton().GetMainController();
    if( pc )
    {
        if( slotId >= qssSlotBegin && slotId < qssSlotCount )
        {
            pc->EquipNewSpellAndAutoUnequipOld( slotId, 0, 0, 0, false );
        }
    }

	MMOGameEvent_OneInt evt2( GameEvent_ChangeSkillSlot, slotId );
	MMOUI::PostGameEvent(&evt2);
}

void CMessageHandler::Parse_g2c_sync_spell_slot(const char* buf, UINT32 size)
{
	//Sync current player ability Slots
	G2C_SYNC_SPELL_SLOT* msg = (G2C_SYNC_SPELL_SLOT*)buf;
	debugf( TEXT("[CMessageHandler::Parse_g2c_sync_spell_slot] ") );

	AMMOActor* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		warnf(L"can not find the main player when to init ability list");
		return;
	}
	
	debugf( TEXT("[Parse_g2c_sync_spell_book] ") );

    //
	MMOEquipManager &mmoEquipMgr = MMOEquipManager::getSingleton();
	FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
	TMap<INT, class UMMOAbility*> usableRuneAblities;
	for( INT i = 0; i < EQUIP_RUNE_SKILL_MAX; ++i ){
		UBOOL isSkillSlotActive = FALSE;
		UMMOAbility* pWeaponSkill = mmoEquipMgr.GetWeaponRuneSkillBySlotIdx( i, pItemInfo, isSkillSlotActive );
		if( isSkillSlotActive == FALSE || pWeaponSkill == NULL ){
			continue;
		}
		INT skillID = pWeaponSkill->eventGetAbilityId();
		usableRuneAblities.FindOrAdd( skillID ) = pWeaponSkill;
	}

	//Function key initialization, relogin case: initialize once for each character at SyncSlot
    AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	if( ctrl ){
		ctrl->eventIniSlotBind( );

		for( int i=qssSlotBegin; i< qssSlotCount ;++i){
			INT skillRunSlotID = 0;
			INT skillLevel = 0;
			INT runeSkillLevel = 0;
			INT skillMainID = msg->astSlot[i].wSpellID;
			if( i >= qssActiveBegin && i <= qssActiveEnd ){
				//Active ability
				UMMOAbility **ppRuneSkill = usableRuneAblities.Find( skillMainID );
				if( ppRuneSkill != NULL && *ppRuneSkill != NULL ){
					skillRunSlotID = (*ppRuneSkill)->GetRuneSlot();
					runeSkillLevel = (*ppRuneSkill)->SpellCommonAttribute.Level;
				}
			}
			//

			//UBOOL isSkillActive = FALSE;
			////UMMOAbility* weaponSkill = MMOEquipManager::getSingleton().GetWeaponRuneSkillByCmdKey( i - qssActiveBegin, E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND, isSkillActive );
			//UMMOAbility* weaponSkill = mmoEquipMgr.GetCurWeaponRuneSkillOfMainSkillID( skillMainID, isSkillActive );
			//if( isSkillActive == TRUE && weaponSkill != NULL ){
			//	skillRunSlotID = weaponSkill->GetRuneSlot();
			//}

			UMMOAbility* learnedSkill = mainPlayer->GetAbilityById( skillMainID, 0 );
			if( learnedSkill != NULL ){
				skillLevel = learnedSkill->SpellCommonAttribute.Level;
				//if( skillRunSlotID != 0 ){
				//	//Has inscription
				//	if( skillLevel > runeSkillLevel ){
				//		skillLevel = runeSkillLevel;
				//	}
				//}
			}
			// TEMP-DIAG(1.05 backport): spell slot sync
			if( skillMainID != 0 ) debugf( NAME_Log, TEXT("[SlotDiag] i=%d slotID=%d spell=%d learned=%d level=%d"), i, (INT)msg->astSlot[i].bySlotID, skillMainID, learnedSkill != NULL ? 1 : 0, skillLevel );
			ctrl->EquipNewSpellAndAutoUnequipOld( msg->astSlot[i].bySlotID, skillMainID, skillLevel, skillRunSlotID, msg->astSlot[i].byLocked );
		}
	}

	//Notify all panels
	MMOGameEvent_Any evt(GameEvent_Sync_SkillEquippedSkills);
	MMOUI::PostGameEvent(&evt);
}
	
void CMessageHandler::Parse_g2c_add_cooldown(const char* buf, UINT32 size)
{
	G2C_ADD_COOLDOWN* msg = (G2C_ADD_COOLDOWN*)buf;
	INT InType = 0;
	INT InId = 0;
	ParseCoolDownID(msg->dwCooldownID, InType, InId);
	MMOFacade::getSingleton().GetAbilityProcessor()->AddCoolDown( InType , InId, msg->dwCooldownValue);

	if (InType == COOLDOWN_QUEST)
	{
		MMOQuestManager::getSingleton().AddQuestCoolDown(InId, msg->dwCooldownValue);
	}
}

void CMessageHandler::Parse_g2c_sync_spell_book(const char* buf, UINT32 size)
{
	//This only syncs all abilities the player has learned, no need to involve UI
	G2C_SYNC_SPELL_BOOK* msg = (G2C_SYNC_SPELL_BOOK*)buf;
	INT byCount = (size - sizeof(G2C_SYNC_SPELL_BOOK)) / sizeof(msg->astSpellData[0]);

	//
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	for (INT i = 0; i < byCount; ++i)
	{
		UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID(msg->astSpellData[i].wID, msg->astSpellData[i].bySpellLevel, 0);

		mmoSkillMgr.AddMyLearnedMainSkillByBook( uniqueSkillID );
		mmoSkillMgr.SetMySkillExp( msg->astSpellData[i].wID, msg->astSpellData[i].wCurPts );
	}

	MMOGameEvent_Any evt( GameEvent_SyncMyLearnedSkillsByBook );
	MMOUI::PostGameEvent(&evt);

	UINT32 abilityCount = 0;
	UINT32 activeAbilityCount = 0;
	TMap<INT, class UMMOAbility*> allLearnedSkills = MMOSkillManager::getSingleton().GetMyLearnedMainSkills();
	for (TMap<INT, class UMMOAbility*>::TIterator iter(allLearnedSkills); iter; ++ iter)
	{
		UMMOAbility* mainAbility = iter.Value();
		if (!mainAbility)
		{
			continue;
		}

		if (mainAbility->bHasSpellProgressData && 
			mainAbility->ClientCommonAttribute.ShowOnUI)
		{
			abilityCount ++;

			if (mainAbility->SpellCommonAttribute.CastType == E_spct_Active)
			{
				activeAbilityCount ++;
			}
		}
	}
	MMOGameEvent_Achievement evtAch1(adSpellCount, abilityCount, 0, 0);
	MMOUI::PostGameEvent(&evtAch1);

	MMOGameEvent_Achievement evtAch2(adActiveSpellCount, activeAbilityCount, 0, 0);
	MMOUI::PostGameEvent(&evtAch2);

	//Update combat power
	MMOPowerManager::getSingleton().UpdatePartPower(PowerPart_SkillPow);
}

//====================================================================================================================
void CMessageHandler::Parse_g2c_role_learn_spell(const char* buf, UINT32 size)
{
	G2C_ROLE_LEARN_SPELL* msg = (G2C_ROLE_LEARN_SPELL*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
	if (actor && actor->ServerID == msg->dwRoleID)
	{
		UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID(msg->wSpellID, msg->bySpellLevel);
		MMOGameEvent_ProcessDynSkill evt( GameEvent_LearnedNewSkill, uniqueSkillID );
		MMOUI::PostGameEvent(&evt);
	}
}
//------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_role_learn_item_spell(const char* buf, UINT32 size)
{
	G2C_ROLE_LEARN_ITEM_SPELL* msg = (G2C_ROLE_LEARN_ITEM_SPELL*)buf;

	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( msg->wSpellID, msg->bySpellLevel );

	MMOGameEvent_ProcessDynSkill learnSkillEvt( GameEvent_LearnedNewSkill, uniqueSkillID );
	learnSkillEvt.EquipItemID = msg->byItemSlot;
	MMOUI::PostGameEvent( &learnSkillEvt );
}
//====================================================================================================================
void CMessageHandler::Parse_g2c_role_remove_spell(const char* buf, UINT32 size)
{
	G2C_ROLE_REMOVE_SPELL* msg = (G2C_ROLE_REMOVE_SPELL*)buf;
	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( msg->wSpellID, 0 );
	
	MMOGameEvent_ProcessDynSkill learnSkillEvt( GameEvent_ForgetSkill, uniqueSkillID );
	MMOUI::PostGameEvent( &learnSkillEvt );
}
//---------------------------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_role_remove_item_spell(const char* buf, UINT32 size)
{
	G2C_ROLE_REMOVE_ITEM_SPELL* msg = (G2C_ROLE_REMOVE_ITEM_SPELL*)buf;
	UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( msg->wSpellID, 0 );

	MMOGameEvent_ProcessDynSkill learnSkillEvt( GameEvent_ForgetSkill, uniqueSkillID );
	learnSkillEvt.EquipItemID = msg->byItemSlot;
	MMOUI::PostGameEvent( &learnSkillEvt );
}
//====================================================================================================================
void CMessageHandler::Parse_g2c_char_select_focus(const char* buf, UINT32 size)
{
	//G2C_CHAR_SELECT_FOCUS* msg = (G2C_CHAR_SELECT_FOCUS*)buf;
	//msg->dwFocusID
}

//---------------------------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_focus_info(const char* buf, UINT32 size)
{
	G2C_FOCUS_INFO* msg = ( G2C_FOCUS_INFO*)buf;
	AMMOActor* actor =Cast<AMMOActor>( MMOFacade::getSingleton().GetActor(msg->dwFocusID) ) ;
	if( !actor )
		return;

	if( actor->IsA(ANPC::StaticClass() ) )
	{
		ANPC* npc = Cast<ANPC>(actor);
		npc->ActorAttributes->SetMaxHP(msg->nMaxHp);
		MMOFacade::getSingleton().SetNPCHP( npc , msg->nCurrentHp);
	}
	else if(actor->IsA(AMMOPlayer::StaticClass()))
	{
		AMMOPlayer* player = Cast<AMMOPlayer>( actor );
		if( !player->IsMainPlayer() )
		{
			player->ActorAttributes->SetMaxHP(msg->nMaxHp);
			player->ActorAttributes->SetMaxMP(msg->nMaxMp);
			MMOFacade::getSingleton().SetPlayerHP(player, msg->nCurrentHp);
			MMOFacade::getSingleton().SetPlayerMP(player, msg->nCurrentMp); 
		}
	}

	//Add and refresh buff
	for(INT i=0;i<msg->byAuraCount;i++)
	{
		AURA_DATA* pData = &msg->astAuraData[i];
		MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(NULL, msg->dwFocusID , pData->dwInstanceID, pData->wID, 0, pData->dwRemainDuration, pData->dwMaxDuration, pData->byStackCount);
	}

	//MMO::TargetManager::getSingleton().SetCurrentTarget(actor);
}
//---------------------------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Parse_g2c_sync_hp(const char* buf, UINT32 size)
{
	G2C_SYNC_HP* msg = (G2C_SYNC_HP*)buf;

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID) ;
	if(actor==NULL)return;
	if(actor->ActorAttributes==NULL)return;

	INT curHp = actor->ActorAttributes->HP;
	INT maxHp = actor->ActorAttributes->MaxHP;
	if(curHp==msg->nCurrentHp&&maxHp==msg->nMaxHp)
	{
		return;
	}

	if(actor->IsMainPlayer())
	{
		MMOFacade::getSingleton().SetMyCurrentMaxHP(msg->nMaxHp);
		MMOFacade::getSingleton().SetMyCurrentHP(msg->nCurrentHp);
	}
	else if(actor->IsA(ANPC::StaticClass()))
	{
		ANPC* tnpc = Cast<ANPC>(actor);
		if(tnpc)
		{
			if(msg->nCurrentHp<curHp && curHp<=maxHp && curHp<=msg->nMaxHp)
			{
				tnpc->ShowHpNumber = TRUE;
			}

			if(maxHp!=msg->nMaxHp)
			{
				G2C_SYNC_NPC_EX_DATA* CacheData = m_NPCDataCache.GetCacheData(msg->dwCharID);
				if (CacheData != NULL)
				{
					CacheData->nMaxHp = msg->nMaxHp;
				} 
			}
		}

		//debugf(TEXT("Parse_g2c_sync_hp==Cur:%d  Max:%d================================="),msg->nCurrentHp, msg->nMaxHp );

		MMOFacade::getSingleton().SetNPCHP( tnpc , msg->nCurrentHp, msg->nMaxHp);
		
	}
	else if(actor->IsA(AMMOPlayer::StaticClass()))
	{
		AMMOPlayer* tplayer = Cast<AMMOPlayer>(actor);
		if(tplayer)
		{
			if(msg->nCurrentHp<curHp && curHp<=maxHp && curHp<=msg->nMaxHp)
			{
				tplayer->ShowHpNumber = TRUE;
			}
		}
		MMOFacade::getSingleton().SetPlayerHP( tplayer,msg->nCurrentHp, msg->nMaxHp);
	}
	else
	{
		actor->ActorAttributes->MaxHP = msg->nMaxHp;
		actor->ActorAttributes->SetHP(msg->nCurrentHp);// = msg->nCurrentHp;
	}
}

void CMessageHandler::Parse_g2c_sync_mp(const char* buf, UINT32 size)
{
	G2C_SYNC_MP* msg = (G2C_SYNC_MP*)buf;

	MMOFacade::getSingleton().SetMyCurrentMP(msg->nCurrentMp);

}

//void CMessageHandler::Parse_g2c_focus_hifreq_sync(const char* buf, UINT32 size)
//{
//	G2C_FOCUS_HIFREQ_SYNC* msg = (G2C_FOCUS_HIFREQ_SYNC*)buf;
//
//	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwFocusID) ;
//	if(actor==NULL)return;
//
//	if( actor->IsA(ANPC::StaticClass() ) )
//	{
//		ANPC* tnpc = Cast<ANPC>(actor);
//		MMOFacade::getSingleton().SetNPCHP( tnpc , msg->nCurrentHp, msg->nMaxHp);
//	}
//	else if( actor->IsA(AMMOPlayer::StaticClass() ))
//	{
//		AMMOPlayer* tplayer = Cast<AMMOPlayer>(actor);
//		MMOFacade::getSingleton().SetPlayerHP( tplayer,msg->nCurrentHp, msg->nMaxHp);
//		//MMOFacade::getSingleton().SetPlayerMP( tplayer,msg->nCurrentMp);
//	}
//
//
//	////msg
//	//AMMOPlayerController* pl = MMOFacade::getSingleton().GetMainController();
//
//
//
//	//if( pl && pl->myHUD )
//	//{
//	//	AMMOActor* actor = MMO::TargetManager::getSingleton().GetCurrentTarget();
//	//	if (!actor)
//	//	{
//	//		return;
//	//	}
//
//	//	if( actor->IsA(ANPC::StaticClass() ) )
//	//	{
//	//		ANPC* tnpc = Cast<ANPC>(actor);
//	//		MMOFacade::getSingleton().SetNPCHP( tnpc , msg->nCurrentHp);
//	//	}
//	//	else if( actor->IsA(AMMOPlayer::StaticClass() ))
//	//	{
//	//		AMMOPlayer* tplayer = Cast<AMMOPlayer>(actor);
//	//		MMOFacade::getSingleton().SetPlayerHP( tplayer,msg->nCurrentHp);
//	//		MMOFacade::getSingleton().SetPlayerMP( tplayer,msg->nCurrentMp);
//	//	}
//	//}
//}

void CMessageHandler::Parse_g2c_new_aura(const char* buf, UINT32 size)
{
	G2C_NEW_AURA* msg = (G2C_NEW_AURA*)buf;
	MMOFacade::getSingleton().GetMMOBuffManager()->AddNewBuff(msg->dwCasterID, msg->dwCharID, msg->dwInstanceID, msg->wAuraID, 0, msg->dwDuration,msg->dwDuration, msg->byStackCount);
	//MMOFacade::getSingleton().GetAbilityProcessor()->AddNewBuff(msg->dwCasterID, msg->dwCharID, msg->dwInstanceID, msg->wAuraID, 0, msg->dwDuration, msg->byStackCount);
}

void CMessageHandler::Parse_g2c_aura_change(const char* buf, UINT32 size)
{
	G2C_AURA_CHANGE* msg = (G2C_AURA_CHANGE*)buf;
	MMOFacade::getSingleton().GetMMOBuffManager()->ChangeBuff(NULL,  msg->dwCharID, msg->AuraData.dwInstanceID,  msg->AuraData.wID, 0, msg->AuraData.dwRemainDuration, msg->AuraData.dwMaxDuration, msg->AuraData.byStackCount);
	//MMOFacade::getSingleton().GetAbilityProcessor()->ChangeBuff(msg->dwCharID, msg->dwInstanceID, msg->dwDuration, msg->byStackCount);
}

void CMessageHandler::Parse_g2c_del_aura(const char* buf, UINT32 size)
{
	G2C_DEL_AURA* msg = (G2C_DEL_AURA*)buf;
	MMOFacade::getSingleton().GetMMOBuffManager()->DeleteBuff(msg->dwCharID, msg->dwInstanceID);
	//MMOFacade::getSingleton().GetAbilityProcessor()->DeleteBuff(msg->dwInstanceID);
}

void CMessageHandler::Parse_g2c_char_freeze(const char* buf, UINT32 size)
{
	G2C_CHAR_FREEZE* msg = (G2C_CHAR_FREEZE*)buf;

	//debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_char_freeze] %d"), msg->dwCharID);

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if (actor)
	{
		FVector FreezeLocation(msg->wCurrentX, msg->wCurrentY, msg->wCurrentZ);
		ConvertCoordN2C(FreezeLocation);

		//INT deltaDist = g_moveModGridSize * g_netLagToleranceDisatnce;
		INT deltaDist = 128.0f * g_netLagToleranceDisatnce;
		FVector DeltaDistance = actor->Location - FreezeLocation;

		if((DeltaDistance.X > -deltaDist) && (DeltaDistance.X < deltaDist) &&
			(DeltaDistance.Y > -deltaDist) && (DeltaDistance.Y < deltaDist)	)
		{
			actor->SetFreeze(msg->byFreezed, actor->Location);
		}
		else
		{
			actor->SetFreeze(msg->byFreezed, FreezeLocation);
		}
	}
}

void CMessageHandler::Parse_g2c_char_client_show_time(const char* buf, UINT32 size)
{
	G2C_CHAR_CLIENT_SHOW_TIME* msg = (G2C_CHAR_CLIENT_SHOW_TIME*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if (actor)
	{
		FVector LastLocation(msg->wCurrentX, msg->wCurrentY, msg->wCurrentZ);
		ConvertCoordN2C(LastLocation);
		LastLocation.Z += actor->GetCollisionHeight();
		actor->SetShowTime(msg->byShowTime, LastLocation);
		//
	}
}

void CMessageHandler::Parse_g2c_char_hard(const char* buf, UINT32 size)
{
	G2C_CHAR_HARD* msg = (G2C_CHAR_HARD*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);

}

void CMessageHandler::Send_Revive(  UINT8 byType )
{
	C2G_REVIVE msg;
	msg.wMsg = c2g_revive;
	msg.byType  = byType;
	_Send( &msg, sizeof(msg));
}

void CMessageHandler::Parse_g2c_char_die(const char* buf, UINT32 size)
{
	G2C_CHAR_DIE * msg = (G2C_CHAR_DIE*)buf;
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if (actor)
	{
		//warnf(NAME_Warning, TEXT("Parse_g2c_char_die========================================================") );
		actor->ProcessDefaultDie(msg->byType);

		if(actor->ActorAttributes)
		{
			if(actor->ActorAttributes->IsGreyName)
			{
				actor->ActorAttributes->IsGreyName = FALSE;
				MMOUI::GetUIManager()->delegateUpdateHintText(actor);
			}
		}

		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		if(player&&player->ServerID==actor->ServerID)
		{
			player->ActorAttributes->SetHP(0);
			debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_char_die]  You died!!!") );
			MMOFacade::getSingleton().NotifyMyHPChange();
//			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Death,TRUE, "");
			//MMOUI::GetUIManager()->delegateShowDeath();

			//AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
			//AMMOHud* hud = Cast<AMMOHud>(ctrl->myHUD);
			//if( ctrl != NULL )
			//{
			//	if ( hud && hud->HudMovie)
			//	{
			//		hud->HudMovie->eventShowDeath();
			//	}
			//}
			//MMOFacade::getSingleton().GetMainController()->eventAddCommonMsgBox(	TEXT("Are you sure you want to revive? ") , 1,  0);
		}
		else
		{
			ANPC* npc = Cast<ANPC>(actor);
			if(npc)
			{
				if(npc->Template)
				{
					debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_char_die] NPC Die:[%d][%d] %s"), npc->GetTemplateID(), npc->Template->TemplateID, *npc->Template->GetDisplayName() );
				}
				else
				{
					debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_char_die] NPC Die:[%d]"), npc->GetTemplateID());
				}

				if( npc->ActorAttributes->GetCurHp() != 0)
				{
					MMOFacade::getSingleton().SetNPCHP( npc , 0 );
				}
			}
			AMMOPlayer* pl = Cast<AMMOPlayer>(actor);
			if(pl && pl!= MMOFacade::getSingleton().GetMainPlayer() )
			{
				debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_char_die] Other player Die:[%s]"), *pl->ActorName);

				if( pl->ActorAttributes->GetCurHp() != 0)
				{
					MMOFacade::getSingleton().SetPlayerHP(pl, 0 );
				}
			}
		}

		MMOGameEvent_CharacterDie evt( actor );
		MMOUI::PostGameEvent(&evt);

		actor->ReCalGridBlockRadius();
	}
}
//void CMessageHandler::Parse_g2c_revive_response(const char* buf, UINT32 size)
//{
//	G2C_REVIVE_RESPONSE * msg = (G2C_REVIVE_RESPONSE*)buf;
//	debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_revive_response] revive response") );
//	if(msg && msg->nDelayTime>0 ){
//		MMOUI::GetUIManager()->delegateCallShowWaitingRevive(msg->nDelayTime/1000);
//	}
//}

void CMessageHandler::Parse_g2c_revive_info(const char* buf, UINT32 size)
{
	G2C_REVIVE_INFO * msg = (G2C_REVIVE_INFO*)buf;
	debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_revive_info] Revive info") );
	
	MMODeathManager::getSingleton().SetReviveInfo(msg);
	//MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Death,TRUE, "");

	MMODeathManager::getSingleton().NoityfyShowDeathPanel();
}


void CMessageHandler::Parse_g2c_char_revive(const char* buf, UINT32 size)
{
	G2C_CHAR_REVIVE * msg = (G2C_CHAR_REVIVE*)buf;
	debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_char_revive] Revived!!!") );
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if (actor)
	{
		actor->eventSetRevive();

		AMMOPlayer* p = Cast<AMMOPlayer>(actor);
		if(p)
		{
			p->PlayReviveEffect(p->GetFootLocation(), p->Rotation);

			//If it is self
			if(p == MMOFacade::getSingleton().GetMainPlayer())
			{
				p->eventEndDeathEffect();

				AMMOPlayerController* conl = MMOFacade::getSingleton().GetMainController();
				// reset action queue
				if (conl && conl->PlayerActionQueue)
				{
					conl->PlayerActionQueue->bLocked = FALSE;
				}
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Death,FALSE, "");
				//MMOUI::GetUIManager()->delegateHideDeath();
			}
		}

	actor->ReCalGridBlockRadius();	// zhaojun@snda, recalculate blocking after revive
	}


	//AMMOPlayerController* conl = MMOFacade::getSingleton().GetMainController();
	//AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	//if(player && actor)
	//{
	//	player->PlayReviveEffect(actor->Location, actor->Rotation);
	//}

	//if(player&&player->ServerID==actor->ServerID)
	//{
	//	// reset action queue
	//	if (conl && conl->PlayerActionQueue)
	//	{
	//		conl->PlayerActionQueue->bLocked = FALSE;
	//	}


	//	player->eventEndDeathEffect();

	//	MMOUI::GetUIManager()->delegateHideDeath();
	//	////Cancel death panel
	//	//AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	//	//AMMOHud* hud = Cast<AMMOHud>(ctrl->myHUD);
	//	//if( ctrl != NULL )
	//	//{
	//	//	if ( hud && hud->HudMovie)
	//	//	{
	//	//		hud->HudMovie->eventHideDeath();
	//	//	}
	//	//}
	//}
}

void CMessageHandler::Parse_g2c_missile_appear(const char* buf, UINT32 size)
{
	G2C_MISSILE_APPEAR* msg = (G2C_MISSILE_APPEAR*)buf;
	OnMissileAppear(msg->stAppearData);
}

void CMessageHandler::Parse_g2c_sync_missile_list(const char*buf, UINT32 size)
{
	G2C_SYNC_MISSILE_LIST* msg = (G2C_SYNC_MISSILE_LIST*)buf;

	INT count = (msg->wSize - sizeof(FLEX_MESSAGE_HEADER) ) / sizeof(MISSILE_APPEAR_DATA);
	for (INT i = 0; i < count; ++i)
	{
		OnMissileAppear(msg->astAppearData[i]);
	}

}

void CMessageHandler::Parse_g2c_missile_move(const char* buf, UINT32 size)
{
	/*
	UINT32          dwMissileID;
	UINT16          wRemainFrm;
	INT32			nFaceDirection;
	INT32           nDestX;
	INT32           nDestY;
	INT32           nDestZ;
	*/
	G2C_MISSILE_MOVE* msg = (G2C_MISSILE_MOVE*)buf;
	FVector Dst(msg->wDestX, msg->wDestY, msg->wDestZ);
	ConvertCoordN2C(Dst);
	FRotator Rot(0, 0, 0);
	Rot.Yaw = ConvertToClientDegree(msg->nFaceDirection);
	FLOAT CostTime = FLOAT(msg->wRemainFrm) / FLOAT(GAME_FPS);
	MMOFacade::getSingleton().GetAbilityProcessor()->OnMissileChangeMovement(msg->dwMissileID, Dst, Rot, CostTime);
}

void CMessageHandler::Parse_g2c_sync_pk_mode(const char* buf, UINT32 size)
{
	G2C_SYNC_PK_MODE* msg = (G2C_SYNC_PK_MODE*)buf;
	
	AMMOActor* actor =Cast<AMMOActor>( MMOFacade::getSingleton().GetActor(msg->dwRoleID));
	if(actor)
	{	
		if(actor->IsMainPlayer())
		{
			MMOFacade::getSingleton().SetMyPkMode(msg->byPkMode);

			FString ModeString = MMOUI::ParseLocalizedPropertyPath(FString::Printf(TEXT("MMOGame.PkMode.PkMode%d"),msg->byPkMode));
			FString ModeChange = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage6")),*ModeString);
			MMOUI::GetUIManager()->delegateAddScreenMessage(ModeChange  ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage6"));
			//MMOFacade::getSingleton().NotifyPlayerPkModeChange();
			MMOFacade::getSingleton().RefleshAllHints();
		}
		else
		{
			actor->ActorAttributes->PkMode = msg->byPkMode;
			MMOFacade::getSingleton().NotifyOtherPlayerPkModeChange(Cast<AMMOPlayer>(actor),actor->ActorAttributes->PkMode);
		}
	}	
}

void CMessageHandler::Parse_g2c_sync_faction(const char* buf, UINT32 size)
{
    G2C_SYNC_FACTION* msg = (G2C_SYNC_FACTION*)buf;

    AMMOActor* actor =Cast<AMMOActor>( MMOFacade::getSingleton().GetActor(msg->dwRoleID));
    if(actor){
        actor->ActorAttributes->Faction = msg->byFaction;
    }	
}

void CMessageHandler::Parse_g2c_enter_combat(const char* buf, UINT32 size)
{
	G2C_ENTER_COMBAT* msg = (G2C_ENTER_COMBAT*)buf;

	MMOFacade::getSingleton().NotifyEnterCompact( msg->dwCharID );
}

void CMessageHandler::Parse_g2c_leave_combat(const char* buf, UINT32 size)
{
	G2C_LEAVE_COMBAT* msg = (G2C_LEAVE_COMBAT*)buf;
	
	MMOFacade::getSingleton().NotifyLeaveCompact( msg->dwCharID );
}

void CMessageHandler::Parse_g2c_sync_combo_kill(const char* buf, UINT32 size)
{
	//Designer ck requested not to display this kind of info anymore
	/*G2C_SYNC_COMBO_KILL* msg = (G2C_SYNC_COMBO_KILL*)buf;
	AMMOActor* actor =Cast<AMMOActor>( MMOFacade::getSingleton().GetActor(msg->dwRoleID));
	FString strId;
	if( msg->nComboKill < 10 ){
		strId = TEXT("MMOGame.Combat.CombatMsg10");
		strId += appItoa( msg->nComboKill );
	}
	else{
		strId = TEXT("MMOGame.Combat.CombatMsg110");
	}
	FString playerName(UTF8_TO_TCHAR(msg->szName));
	FString hint = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( strId ), *playerName );
	//MMOUI::GetUIManager()->delegateShowCombatMsg( hint );
	MMOUI::GetUIManager()->delegateAddScreenMessage( hint, MsgInfoType_Table , strId, msg->byFaction == EBTLTMS_Team1 ? 0 : 1 );*/
}

////Equip rune
//void CMessageHandler::Send_EquipRune( INT inSlot, INT inRuneSlot)
//{
//	C2G_EQUIP_RUNE msg;
//	msg.wMsg = c2g_equip_rune;
//	msg.bySlot = inSlot;
//	msg.byRuneSlot = inRuneSlot;
//	_Send(&msg, sizeof(msg));
//}

////Activate rune
//void CMessageHandler::Send_ActivateRune( INT inSpellId, INT inRuneSlot)
//{
//	C2G_ACTIVATE_RUNE msg;
//	msg.wMsg = c2g_activate_rune;
//	msg.wSpellID = inSpellId;
//	msg.byRuneSlot = inRuneSlot;
//	_Send(&msg, sizeof(msg));
//}

//Rune activation notification
//void CMessageHandler::Parse_g2c_rune_acvitate(const char* buf, UINT32 size)
//{
//	G2C_RUNE_ACVITATE* msg = (G2C_RUNE_ACVITATE*)buf;
//	//if( msg->nErrCode != errSuccess )
//	//{
//	//	//static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);
//	//	//FString enumVal = InEnum->GetEnum(msg->nErrCode).ToString();
//
//	//	FString enumVal(g_szErrorCodeNameList[msg->nErrCode]);
//
//	//	debugf( TEXT("[CMessageHandler::Parse_g2c_rune_acvitate] SpellID: %d,%s"),
//	//		msg->wSpellID, *enumVal );
//	//	return;
//	//}
//
//	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
//	if (!actor)
//	{
//		warnf(L"can not find the main player when to init ability list");
//		return;
//	}
//	RUNE_DATA tRune;
//	tRune.wSpellID =  msg->wSpellID ; 
//	tRune.bySlot =  msg->byRuneSlot; 
//	tRune.byDurabilityType = msg->byDurabilityType; 
//	tRune.nDurability = msg->nDurability;
//
//	MMOSkillManager::getSingleton().UpdateRuneDur( &tRune);
//	/*//Refresh panel z
//	AMMOHud* hud =  MMOFacade::getSingleton().GetMyHud();
//	if( hud && hud->HudMovie )
//	{
//		if( hud->HudMovie->_Skill ) 
//		{
//			//-qssSlotCount
//			//hud->HudMovie->_Skill->RuneActivate( msg->wSpellID , msg->byRuneSlot,msg->byDurabilityType,msg->nDurability );
//			if( hud->HudMovie->_Skill->IsVisible )
//				hud->HudMovie->_Skill->RefreshAll();
//		}
//	}*/
//	//Notify all panels
//	MMOFacade::getSingleton().NotifySkillSlotChange();
//
//}

//Equip rune, equip rune response
void CMessageHandler::Parse_g2c_equip_rune(const char* buf, UINT32 size)
{
	G2C_EQUIP_RUNE* msg = (G2C_EQUIP_RUNE*)buf;

	//if( msg->nErrCode != errSuccess )
	//{
	//	//static UEnum* InEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EERROR_CODE_DEF"), TRUE);
	//	//FString enumVal = InEnum->GetEnum(msg->nErrCode).ToString();

	//	FString enumVal(g_szErrorCodeNameList[msg->nErrCode]);

	//	debugf( TEXT("[CMessageHandler::Parse_g2c_equip_rune] : %d,%s"),
	//		msg->bySlot, *enumVal );
	//	return;
	//}
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		warnf(L"can not find the main player when to init ability list");
		return;
	}

	AMMOPlayerController *pc = MMOFacade::getSingleton().GetMainController();
	if( pc )
	{
		INT mainAbilityID = msg->wSpellID;
		UMMOAbility* curEquipedSkill = MMOSkillManager::getSingleton().GetEquippedSkillBySkillID( mainAbilityID );
		if( curEquipedSkill != NULL ){
			INT curEuippedSlot = curEquipedSkill->SlotHotKeyID;
			//Only if the main ability of this rune ability is already equipped can this rune ability be equipped
			INT curEquipSkillLv = 1;
//			if( msg->byRuneLevel > 0 ){
//				//No level 0 ability
//				curEquipSkillLv = msg->bySpellLevel;//( msg->byRuneLevel < msg->bySpellLevel ? msg->byRuneLevel : msg->bySpellLevel );//take the lower of main ability and inscription ability levels
//			}
//			else{
				//Means there is no rune ability
				curEquipSkillLv = msg->bySpellLevel;
				//if( msg->byRuneSlot != 0 ){
				//	//This must equal 0, otherwise the message data is wrong
				//	warnf(NAME_Error, TEXT("======!!!!======= [Parse_g2c_equip_rune Slot:%d SkillID:%d RuneLevel is 0 but byRuneSlot is NOT 0]") , curEuippedSlot, msg->wSpellID );
				//}
//			}
			pc->EquipNewSpellAndAutoUnequipOld( curEuippedSlot, msg->wSpellID, curEquipSkillLv, msg->byRuneSlot, false );
			warnf(NAME_Warning, TEXT("======!!!!======= [Parse_g2c_equip_rune Slot:%d SkillID:%d SkillRuneID:%d]") , curEuippedSlot, msg->wSpellID, msg->byRuneSlot );

			//
			MMOGameEvent_EquipRuneSkill evt( GameEvent_EquipRuneSkill, curEuippedSlot, msg->wSpellID, msg->byRuneSlot );
			MMOUI::PostGameEvent(&evt);
		}
		else{
			warnf(NAME_Warning, TEXT("Parse_g2c_equip_rune to EquipRuneSKill FAILED!!!! main SKill is not equipped") );
		}
	}
}

//Rune durability change notification
//void CMessageHandler::Parse_g2c_rune_durability_change(const char* buf, UINT32 size)
//{
//	G2C_RUNE_DURABILITY_CHANGE* msg = (G2C_RUNE_DURABILITY_CHANGE*)buf;
//
//	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
//	if (!actor)
//	{
//		warnf(L"can not find the main player when to init ability list");
//		return;
//	}
//	MMOSkillManager::getSingleton().UpdateRuneDur( &msg->RuneData );
//	/*//Refresh panel
//	AMMOHud* hud =  MMOFacade::getSingleton().GetMyHud();
//	if( hud && hud->HudMovie )
//	{
//		if( hud->HudMovie->_Skill ) 
//		{
//			//-qssSlotCount
//			//hud->HudMovie->_Skill->RuneActivate( msg->RuneData.wSpellID , msg->RuneData.bySlot,msg->RuneData.byDurabilityType,msg->RuneData.nDurability );
//			if( hud->HudMovie->_Skill->IsVisible )
//				hud->HudMovie->_Skill->RefreshAll();
//		}
//	}*/
//	//Notify all panels
//	MMOFacade::getSingleton().NotifySkillSlotChange();
//}
//
////Sync all rune durability
//void CMessageHandler::Parse_g2c_sync_rune_list(const char* buf, UINT32 size)
//{
//	/*enum RUNE_DURABILITY_TYPE
//	{
//		rdtInvalid = -1,
//		rdtNever,
//		rdtByUseTimes,
//		rdtByDuration,
//		rdtCount,
//	};*/
//	G2C_SYNC_RUNE_LIST* msg = (G2C_SYNC_RUNE_LIST*)buf;
//
//	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
//	if (!actor)
//	{
//		warnf(L"can not find the main player when to init ability list");
//		return;
//	}
//	INT byCount = (size - sizeof(G2C_SYNC_RUNE_LIST)) / sizeof( RUNE_DATA );
//	// Previous player data may remain
// 	MMOSkillManager::getSingleton().IniRuneData();
//
//	for ( int i=0; i< byCount ; ++i )
//	{
//		MMOSkillManager::getSingleton().UpdateRuneDur( &msg->astRuneData[i] );
//	}	
//	/*//Refresh panel
//	AMMOHud* hud =  MMOFacade::getSingleton().GetMyHud();
//	if( hud && hud->HudMovie )
//	{
//		if( hud->HudMovie->_Skill ) 
//		{
//			//-qssSlotCount
//			if( hud->HudMovie->_Skill->IsVisible )
//				hud->HudMovie->_Skill->RefreshAll();
//		}
//	}*/
//	//Notify all panels
//	MMOFacade::getSingleton().NotifySkillSlotChange();
//}

void CMessageHandler::Parse_g2c_leech_soul(const char* buf, UINT32 size)
{
	G2C_LEECH_SOUL* msg = (G2C_LEECH_SOUL*)buf;
	AMMOActor* target = MMOFacade::getSingleton().GetCharacter(msg->dwToObjID);
	if (target && target->GpDataManager && target->GpDataManager->GameGhostSpawner)
	{
		target->GpDataManager->GameGhostSpawner->OnAction(msg->dwFromObjID, msg->dwToObjID, msg->nSoulPts);
	}
}

//Start heart training
void CMessageHandler::Send_StartNormalPractice( INT byType, INT wSpellID )
{
//	C2G_START_NORMAL_PRACTICE msg;
//	msg.wMsg = c2g_start_normal_practice;
//	msg.byType = byType;
//	msg.wSpellID = wSpellID;
//	_Send(&msg, sizeof(msg));
}

//Start soul training
void CMessageHandler::Send_StartFastPractice( INT byType, INT wSpellID )
{
//	C2G_START_FAST_PRACTICE msg;
//	msg.wMsg = c2g_start_fast_practice;
//	msg.byType = byType;
//	msg.wSpellID = wSpellID;
//	_Send(&msg, sizeof(msg));
}

//Soul training use medicine
void CMessageHandler::Send_SpeedUpFastPractice( INT byType, INT wSpellID )
{
//	C2G_SPEEDUP_FAST_PRACTICE msg;
//	msg.wMsg = c2g_speedup_fast_practice;
//	msg.wSpellID = wSpellID;
//	msg.bySpeedupType = byType;
//	_Send(&msg, sizeof(msg));
}

//Interrupt ability practice
void CMessageHandler::Send_StopPractice( INT wSpellID )
{
//	C2G_STOP_PRACTICE msg;
//	msg.wMsg = c2g_stop_practice;
//	msg.wSpellID = wSpellID;
//	_Send(&msg, sizeof(msg));
}
 
//Ability practice notification
//void CMessageHandler::Parse_g2c_start_practice(const char* buf, UINT32 size)
//{
//	G2C_START_PRACTICE* msg = (G2C_START_PRACTICE*)buf;
//	//MMOFacade::getSingleton().NotifySkillSlotChange();
//	UMMOUISkillPanel* skillPanel = NULL;
//	if( GetSkillPanel( &skillPanel ) )
//	{
//		//Reply did not record CostType
//		skillPanel->NetStartSkillPractice( msg->wSpellID,msg->byPracticeType,msg->nInterval,msg->nEndTime);
//	}
//}
//
////Interrupt ability practice notification
//void CMessageHandler::Parse_g2c_stop_practice(const char* buf, UINT32 size)
//{
//	G2C_STOP_PRACTICE* msg = (G2C_STOP_PRACTICE*)buf;
//
//	//MMOFacade::getSingleton().NotifySkillSlotChange();
//
//	UMMOUISkillPanel* skillPanel = NULL;
//	if( GetSkillPanel( &skillPanel ) )
//	{
//		skillPanel->NetStopSkillPractice( msg->wSpellID );
//	}
//}

//Proficiency increase
//void CMessageHandler::Parse_g2c_spell_progress_change(const char* buf, UINT32 size)
//{
//	G2C_SPELL_PROGRESS_CHANGE* msg = (G2C_SPELL_PROGRESS_CHANGE*)buf;
//	//MMOFacade::getSingleton().NotifySkillSlotChange();
//	UMMOUISkillPanel* skillPanel = NULL;
//	if( GetSkillPanel( &skillPanel ) )
//	{
//		skillPanel->NetSkillProgressChange( msg->wSpellID,msg->nProgress );
//	}
//}

//========================================================================================================================
void CMessageHandler::Send_SpellLevelUp( UINT16 skillID )
{
	//C2G_SPELL_LEVEL_UP msg;
	//msg.wMsg = c2g_spell_level_up;
	//msg.wSpellID = skillID;
	//_Send(&msg, sizeof(msg));
}

//Ability upgrade hint
void CMessageHandler::Parse_g2c_spell_level_up(const char* buf, UINT32 size)
{
	G2C_SPELL_LEVEL_UP* msg = (G2C_SPELL_LEVEL_UP*)buf;

    AMMOPlayer *player = MMOFacade::getSingleton().GetMainPlayer();
    if( player ){
        player->PlaySkillLevelUpEffect( player->GetFootLocation(), player->Rotation );
    }

	MMOGameEvent_SkillLevelUp evt( msg->wSpellID, msg->byLevel, 0 );
	MMOUI::PostGameEvent(&evt);

	INT lvCount = 0;
	TMap< INT, class UMMOAbility* > learnedMainSkills = MMOSkillManager::getSingleton().GetMyLearnedMainSkills();
	for (TMap< INT, class UMMOAbility* >::TIterator iter(learnedMainSkills); iter; ++ iter)
	{
		UMMOAbility *ability = iter.Value();
		if (ability != NULL && ability->SpellCommonAttribute.Level == msg->byLevel)
		{
			lvCount ++;
		}
	}

	// Achievement
	MMOGameEvent_Achievement evtAch(adSpellLevel, msg->byLevel, lvCount, 0);
	MMOUI::PostGameEvent(&evtAch);
}

//Ability points increase
//void CMessageHandler::Parse_g2c_sync_ability_pts(const char* buf, UINT32 size)
//{
//	G2C_SYNC_ABILITY_PTS* msg = (G2C_SYNC_ABILITY_PTS*)buf;
//	//MMOFacade::getSingleton().NotifySkillSlotChange();
//	AMMOPlayerController *PC = MMOFacade::getSingleton().GetMainController();
//	if( PC )
//	{
//		 PC->SkillPoint = msg->nAbilityPts;
//	}
//	UMMOUISkillPanel* skillPanel = NULL;
//	if( GetSkillPanel( &skillPanel ) )
//	{
//		skillPanel->NetSkillPointChangeNotify( );
//	}
//}

