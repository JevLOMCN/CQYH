
#include "stdafx.h"
#include "ScreenMsgQueue.h"
#include "UGlobalMapInfo.h"
#include "MMOMapDataManager.h"
#include "MMOSkillManager.h"

#if 0

int _tmain(int argc, _TCHAR* argv[])
{
	::setlocale(LC_ALL, "en-us");

	TestUI1 testUI1;
	TestUI2 testUI2;

	ScreenMessageQueue::getSingleton().RegisterMessageUI(&testUI1);
	ScreenMessageQueue::getSingleton().RegisterMessageUI(&testUI2);

	MessageData* msg = new MessageData;
	msg->MessageType = ESMT_Ability;
	msg->MessageText = " ESMT_Ability";
	ScreenMessageQueue::getSingleton().AddMessage(ESMC_TopCenter, msg);

	msg = new MessageData;
	msg->MessageType = ESMT_Achivement;
	msg->MessageText = " ESMT_Ability";
	ScreenMessageQueue::getSingleton().AddMessage(ESMC_TopCenter, msg);

	msg = new MessageData;
	msg->MessageType = ESMT_Achivement;
	msg->MessageText = " ESMT_Ability2";
	ScreenMessageQueue::getSingleton().AddMessage(ESMC_TopCenter, msg);

	ScreenMessageQueue::getSingleton().Enable(true);
	for(int i = 0; i < 10; ++i)
	{
		ScreenMessageQueue::getSingleton().Update(1.0);
		::Sleep(1000);
	}

	return 0;
}
#endif

//=================================================================================================
//skill learned
float MessageUISkillLearned::DisplayMessage(MessageData* msgData)
{
	UMMOUIScreenPopUpBoxMsg *pScreenPopUpBoxMsg = (UMMOUIScreenPopUpBoxMsg*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint_ScreenPopUpBoxMsg );
	if( pScreenPopUpBoxMsg == NULL || msgData == NULL ){
		return 0;
	}

	UINT uniqueSkillID = msgData->MagicValue;
	UMMOAbility* ability = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( ability != NULL && ability->MainSkill != NULL ){
		ability = ability->MainSkill;
	}

	if( ability == NULL ){
		return 0;
	}

	FString strTitle;
	if( E_spct_Active == ability->SpellCommonAttribute.CastType ){
		if( E_spqte_None == ability->SpellCommonAttribute.QTEType ){
			strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ActiveSkillLearned" ));
		}
		else{
			strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.AvartaSkillLearned" ));
		}
	}
	else if( E_spct_Passive == ability->SpellCommonAttribute.CastType  ){
		strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.PassiveSkillLearned" ));
	}
	else{
		return 0;
	}

	FString strContent;
	strContent = ability->eventGetAbilityName();

	FString strIconPath;
	strIconPath = ability->GetAbilityIconName();

	pScreenPopUpBoxMsg->PlayScreenPopUpBoxMsgByString( msgData->MessageType, strTitle, strContent, strIconPath, TEXT("") );

	return 2;
}
//=================================================================================================
float MessageUIWeaponSkillGained::DisplayMessage(MessageData* msgData)
{
	UMMOUIScreenPopUpBoxMsg *pScreenPopUpBoxMsg = (UMMOUIScreenPopUpBoxMsg*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint_ScreenPopUpBoxMsg );
	if( pScreenPopUpBoxMsg == NULL || msgData == NULL ){
		return 0;
	}

	UINT uniqueSkillID = msgData->MagicValue;
	UMMOAbility* pWeaponSkill = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );

	if( pWeaponSkill == NULL ){
		return 0;
	}

	FString strTitle;
	strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.WeaponSkill.ResultSuccessHintScreenTitle" ));

	FString strContent;
	UMMOAbility *pMainSkill = MMOSkillManager::getSingleton().GetMainAbilityFor( pWeaponSkill );
	if( pMainSkill != NULL ){
		strContent += pMainSkill->eventGetAbilityName();
	}
	strContent += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Tips.WeaponSkillName") ), *( pWeaponSkill->eventGetAbilityName() ) );

	FString strIconPath;
	strIconPath = pWeaponSkill->GetAbilityIconName();

	pScreenPopUpBoxMsg->PlayScreenPopUpBoxMsgByString( msgData->MessageType, strTitle, strContent, strIconPath, TEXT("") );

	return 2;
}
//=================================================================================================
//skill slot unlock
float MessageUISkillSlotUnlock::DisplayMessage(MessageData* msgData)
{
	UMMOUIScreenPopUpBoxMsg *pScreenPopUpBoxMsg = (UMMOUIScreenPopUpBoxMsg*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint_ScreenPopUpBoxMsg );
	if( pScreenPopUpBoxMsg == NULL || msgData == NULL ){
		return 0;
	}

	INT slotIdx = msgData->MagicValue;
	FString strContent;
	FString strExtraString;
	//if( slotIdx >= ECK_QTE1 && slotIdx <= ECK_QTE3 ){
	if( slotIdx == ECK_QTE1 ){
		strContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.AvartaSlotUnlock" ));
	}
	else if( slotIdx >= ECK_Passive1 && slotIdx <= ECK_Passive3 ){
		strContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.PassiveSlotUnlock" ));
	}
	else if( slotIdx >= ECK_Slot1 && slotIdx <= ECK_Slot6 ){
		strContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.ActiveSlotUnlock" ));
		FString strLookUp = TEXT("MMOGame.GamePlay.SkillSlot" );
		strLookUp += appItoa( slotIdx );
		strExtraString = MMOUI::ParseLocalizedPropertyPath( strLookUp );
	}
	else{
		return 0;
	}

	FString strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.SlotUnlock" ));

	pScreenPopUpBoxMsg->PlayScreenPopUpBoxMsgByString( msgData->MessageType, strTitle, strContent, TEXT(""), strExtraString );

	return 2;
}
//=================================================================================================
//skill learned
float MessageUIMainSkillLevelUp::DisplayMessage(MessageData* msgData)
{
	UMMOUIScreenPopUpBoxMsg *pScreenPopUpBoxMsg = (UMMOUIScreenPopUpBoxMsg*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint_ScreenPopUpBoxMsg );
	if( pScreenPopUpBoxMsg == NULL || msgData == NULL ){
		return 0;
	}
	
	UINT uniqueSkillID = msgData->MagicValue;
	UMMOAbility* ability = MMOSkillManager::getSingleton().FindAbilityTemplate( uniqueSkillID );
	if( ability == NULL ){
		return 0;
	}

	FString strTitle;
	FString abilityName = ability->eventGetAbilityName();
	strTitle = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.SkillLvUpNotifyScreenTitle") ), *abilityName );

	FString strContent;
	INT skllNewLevel = ability->SpellCommonAttribute.Level;
	strContent = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Skill.SkillLvUpNotifyScreenText") ), skllNewLevel );

	FString strIconPath;
	strIconPath= ability->GetAbilityIconName();

	pScreenPopUpBoxMsg->PlayScreenPopUpBoxMsgByString( msgData->MessageType, strTitle, strContent, strIconPath, TEXT("") );

	return 1.666;
}
//=================================================================================================
//Achievement
float MessageUIAchievement::DisplayMessage(MessageData* msgData)
{
	UMMOUIAchievementHintPanel *pAchievementHint = (UMMOUIAchievementHintPanel*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_AchievementHint );
	if( pAchievementHint == NULL || msgData == NULL ){
		return 0;
	}
	pAchievementHint->ShowAchievementHintPanel( msgData->MagicValue );

	return 3.834;
}
//=================================================================================================
//Achievement
float MessageUIFBAchievement::DisplayMessage(MessageData* msgData)
{
	UMMOUIScreenPopUpBoxMsg *pScreenPopUpBoxMsg = (UMMOUIScreenPopUpBoxMsg*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint_ScreenPopUpBoxMsg );
	if( pScreenPopUpBoxMsg == NULL || msgData == NULL ){
		return 0;
	}
	
	INT fbAchievementID = msgData->MagicValue;
	FBAchievementsState *curFBAchievement = MMOMapDataManager::getSingleton().FindFBAchievementCurrentDungeon( fbAchievementID );
	if( curFBAchievement == NULL ){
		return 0;
	}

	//
	const FDungeonTarget *thisTemplate = curFBAchievement->_achievementTemplate;
	if( thisTemplate == NULL ){
		return 0;
	}

	FString strTitle = thisTemplate->TargetName;

	pScreenPopUpBoxMsg->PlayScreenPopUpBoxMsgByString( msgData->MessageType, strTitle, TEXT(""), TEXT(""), TEXT("") );

	return 2;
}
//=================================================================================================
//FBUnlock
float MessageUIFBUnlock::DisplayMessage(MessageData* msgData)
{
	return 0;
	//UMMOUIScreenPopUpBoxMsg *pScreenPopUpBoxMsg = (UMMOUIScreenPopUpBoxMsg*)MMOUI::FindUIPanelEx( MMOUI::GetUIManager()->Panel_FightingScreenHint_ScreenPopUpBoxMsg );
	//if( pScreenPopUpBoxMsg == NULL || msgData == NULL ){
	//	return 0;
	//}
	//
	//INT curFbMapID = msgData->MagicValue;
	//FExportMap mapExportInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo( curFbMapID );
	//if( mapExportInfo.ServerMapType != E_est_Dungeon ){
	//	return 0;
	//}

	//INT curFBDiff = MMOMapDataManager::getSingleton().GetCurFBDiff();

	//INT curMaxDiff = MMOMapDataManager::getSingleton().GetCurFBMaxDiff();
	//INT fbOpened1PersonDiffMax = MMOMapDataManager::getSingleton().GetMaxDiffOfType( curFbMapID, EFBDTyp_OnePerson, curMaxDiff );
	//INT fbOpened5PersonDiffMax = MMOMapDataManager::getSingleton().GetMaxDiffOfType( curFbMapID, EFBDTyp_FivePerson, curMaxDiff );

	//INT nextDifficulty = esdInvalid;
	//if( curFBDiff >= esdNormal1 && curFBDiff <= esdHeroic1 && fbOpened1PersonDiffMax < esdHeroic1 ){
	//	//1 person FB
	//	nextDifficulty = fbOpened1PersonDiffMax + 1;
	//}
	//else if( curFBDiff >= esdNormal5 && curFBDiff <= esdHeroic5 && fbOpened5PersonDiffMax < esdHeroic5 ){
	//	//5 person FB
	//	nextDifficulty = fbOpened5PersonDiffMax + 1;
	//}

	//INT iMaxDiffMask = MMOMapDataManager::getSingleton().GetDungeonInfoDifftyMask( curFbMapID );
	//INT intMask = nextDifficulty - esdNormal1;
	//if( nextDifficulty == esdInvalid || ( ( iMaxDiffMask & ( 1 << intMask ) ) == 0 ) ){
	//	return FALSE;
	//}

	////FString hintFormat = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBDiff.FBUnlockHint1") );

	//FString difficultyNameKey = TEXT("MMOGame.FBDiff.FBDiff");
	//difficultyNameKey += appItoa( nextDifficulty - ENUM_SCENE_DIFFICULTY::esdNormal1 );
	//FString strContent = MMOUI::ParseLocalizedPropertyPath( *difficultyNameKey );

	//FString strTitle = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBDiff.FBUnlockHintScreenTitle") ), *mapExportInfo.MapDisplayName );

	//pScreenPopUpBoxMsg->PlayScreenPopUpBoxMsgByString( msgData->MessageType, strTitle, strContent, TEXT(""), TEXT("") );

	//return 2;
}

//=================================================================================================
void ScreenMessageQueue::Init()
{
	static MessageUISkillLearned g_MsgUISkillLearned;
	ScreenMessageQueue::getSingleton().RegisterMessageUI(&g_MsgUISkillLearned);

	//
	static MessageUIWeaponSkillGained g_MsgUIWeaponSkillGained;
	ScreenMessageQueue::getSingleton().RegisterMessageUI(&g_MsgUIWeaponSkillGained);

	//skill slot unlock
	static MessageUISkillSlotUnlock g_MsgUISkillSlotUnlock;
	ScreenMessageQueue::getSingleton().RegisterMessageUI(&g_MsgUISkillSlotUnlock);

	//skill learned
	static MessageUIMainSkillLevelUp g_MsgUIMainSkillLevelUp;
	ScreenMessageQueue::getSingleton().RegisterMessageUI(&g_MsgUIMainSkillLevelUp);

	//Achievement
	static MessageUIAchievement g_MsgUIAchievement;
	ScreenMessageQueue::getSingleton().RegisterMessageUI(&g_MsgUIAchievement);

	//FB Achievement
	static MessageUIFBAchievement g_MsgUIFBAchievement;
	ScreenMessageQueue::getSingleton().RegisterMessageUI(&g_MsgUIFBAchievement);

	//
	static MessageUIFBUnlock g_MsgUIFBUnlock;
	ScreenMessageQueue::getSingleton().RegisterMessageUI(&g_MsgUIFBUnlock);

	//
	//MMOUI::RegisterEventHandler( this, GameEvent_SceneFinished);
	MMOUI::RegisterEventHandler( this, GameEvent_FBAchieve);
	MMOUI::RegisterEventHandler( this, GameEvent_UnlockSkillSlot);
	MMOUI::RegisterEventHandler( this, GameEvent_SkillUiUpdate);
	MMOUI::RegisterEventHandler( this, GameEvent_SkillLevelUp);
	MMOUI::RegisterEventHandler( this, GameEvent_FinishLoading);
	MMOUI::RegisterEventHandler( this, GameEvent_EnterGameSuccess);
}
//=======================================================================================
void ScreenMessageQueue::Shutdown()
{
	MMOUI::UnRegisterEventHandler( this, GameEvent_EnterGameSuccess);
	MMOUI::UnRegisterEventHandler( this, GameEvent_FinishLoading);
	MMOUI::UnRegisterEventHandler( this, GameEvent_SkillLevelUp);
	MMOUI::UnRegisterEventHandler( this, GameEvent_SkillUiUpdate);
	MMOUI::UnRegisterEventHandler( this, GameEvent_UnlockSkillSlot);
	MMOUI::UnRegisterEventHandler( this, GameEvent_FBAchieve);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_SceneFinished);
}
//=======================================================================================
void ScreenMessageQueue::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_UnlockSkillSlot:
		{
			MMOGameEvent_OneInt* newUnlockSlotEvt = (MMOGameEvent_OneInt*)evt;

			MessageData *pNewMsgData = new MessageData();
			pNewMsgData->MessageType = ESMT_SkillSlotUnlock;
			pNewMsgData->MagicValue = newUnlockSlotEvt->IntData;
			
			ScreenMessageQueue::getSingleton().AddMessage( ESMC_TopCenter, pNewMsgData );
		}
		break;
	case GameEvent_SkillUiUpdate:
		{
			MMOGameEvent_SkillUiUpdate* skillEvent = (MMOGameEvent_SkillUiUpdate*)evt;
			switch(skillEvent->EventType)
			{
			case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillLearned:
				{
					MessageData *pNewMsgData = new MessageData();
					pNewMsgData->MessageType = ESMT_SkillLearned;
					pNewMsgData->MagicValue = skillEvent->MagicInt;

					ScreenMessageQueue::getSingleton().AddMessage( ESMC_TopCenter, pNewMsgData );
				}
				break;
			}
		}
		break;
	case MMOGameEvent_SkillUiUpdate::ESkillUiEvent_NewSkillLearned:
		{
			MMOGameEvent_ProcessDynSkill* newSkillEvt = (MMOGameEvent_ProcessDynSkill*)evt;


		}
		break;
	case GameEvent_SkillLevelUp:
		{
			MMOGameEvent_SkillLevelUp *lvUpSkillEvt = (MMOGameEvent_SkillLevelUp*)evt;

			UINT skillUniqueID = MMOSkillManager::CombinateAbilityID( lvUpSkillEvt->SkillID, lvUpSkillEvt->NewLevel, 0 );
			MessageData *pNewMsgData = new MessageData();
			pNewMsgData->MessageType = ESMT_MainSkillLevelUp;
			pNewMsgData->MagicValue = skillUniqueID;

			ScreenMessageQueue::getSingleton().AddMessage( ESMC_TopCenter, pNewMsgData );
		}
		break;
	case GameEvent_SceneFinished:
		{
			MMOGameEvent_OneInt *sceneFinishedEvt = (MMOGameEvent_OneInt*)evt;

			FExportMap *pCurMapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer( sceneFinishedEvt->IntData );
			if( pCurMapInfo != NULL && pCurMapInfo->ServerMapType == E_est_Dungeon ){
				MessageData *pNewMsgData = new MessageData();
				pNewMsgData->MessageType = ESMT_FBUnlock;
				pNewMsgData->MagicValue = sceneFinishedEvt->IntData;
				//a bit laggy, add a delay here
				pNewMsgData->DelayTime = 2;

				ScreenMessageQueue::getSingleton().AddMessage( ESMC_TopCenter, pNewMsgData );
			}
		}
		break;
	case GameEvent_FBAchieve:
		{
			MMOGameEvent_OneInt *fbAchieveEvt = (MMOGameEvent_OneInt*)evt;

			MessageData *pNewMsgData = new MessageData();
			pNewMsgData->MessageType = ESMT_FBAchievement;
			pNewMsgData->MagicValue = fbAchieveEvt->IntData;

			ScreenMessageQueue::getSingleton().AddMessage( ESMC_BottomCenter, pNewMsgData );
		}
		break;
	case GameEvent_FinishLoading:
		{
			ScreenMessageQueue::getSingleton().Enable( true );
		}
		break;
	case GameEvent_EnterGameSuccess:
		{
			ScreenMessageQueue::getSingleton().Clear();
		}
		break;
	}
}

ScreenMessageQueue& ScreenMessageQueue::getSingleton()
{
	static ScreenMessageQueue inst;
	return inst;
}

void ScreenMessageQueue::AddMessage( EScreenMsgChannel channel, MessageData* msgData )
{
	MessageChannelMap::iterator i = this->_messages.find(channel);
	if(i == _messages.end())
	{
		i = _messages.insert(std::make_pair(channel, MessageDataList())).first;
	}

	i->second.push(msgData);
}

void ScreenMessageQueue::Update( float deltaTime )
{
	CTRL_TICK("ScreenMessageQueue");

	if(!_enable)
	{
		return;
	}

	//
	if( MMO::GetCurrentGameState() != EGameState_InGame )
	{
		return;
	}

	MessageChannelMap::iterator i =	_messages.begin();
	for(; i != _messages.end(); ++i)
	{
		MessageDataList& lst = i->second;
		if(!lst.empty())
		{
			MessageData* dat = lst.front();
			dat->DelayTime -= deltaTime;
			float nextDelayTime = 0.0f;

			if(dat->DelayTime <= 0.0f)
			{
				IMessageUI* ui = GetMessageUI(dat->MessageType);
				if(ui)
				{
					nextDelayTime = ui->DisplayMessage(dat);
				}
				else
				{
					// err hand
				}

				lst.pop();
				dat->Destroy();

				if(!lst.empty())
				{
					dat = lst.front();
					dat->DelayTime = nextDelayTime;
				}
			} // if
		} // lst.empty
	}
}

void ScreenMessageQueue::NotifyEndPlayMsg()
{
	return;
}

IMessageUI* ScreenMessageQueue::GetMessageUI( EScreenMessageType msgType )
{
	MessageUIMap::iterator i = _messageUIMap.find(msgType);
	if(i != _messageUIMap.end())
	{
		return i->second;
	}

	return 0;
}

void ScreenMessageQueue::RegisterMessageUI( IMessageUI* ui )
{
	_messageUIMap.insert(std::make_pair(ui->GetType(), ui));
}

void ScreenMessageQueue::Enable( bool enab )
{
	_enable = enab;
}

void ScreenMessageQueue::Clear()
{
	_messages.clear();
}

void ScreenMessageQueue::ClearCurrent( EScreenMsgChannel channel )
{
	MessageChannelMap::iterator i = this->_messages.find(channel);
	if(i == _messages.end())
	{
		return;
	}

	MessageDataList& lst = i->second;
	if(!lst.empty())
	{
		MessageData* dat = lst.front();
		lst.pop();
		dat->Destroy();
	} // lst.empty
}