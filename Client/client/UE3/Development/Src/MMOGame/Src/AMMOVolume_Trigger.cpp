#include "stdafx.h"
#include "MMOFacade.h"
#include "MMOUtil.h"
#include "MMOQuestManager.h"
#include "MMOBuffManager.h"
#include "MMOGuideManager.h"
IMPLEMENT_CLASS(AMMOVolume_Trigger)
IMPLEMENT_CLASS(AMMOVolume_Blocking)

void AMMOVolume_Trigger::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	EditTriggerProperty = (VolumeType == EVTT_Trigger);
	EditPKType = (VolumeType == EVVT_PKType);
	EditStall = (VolumeType == EVVT_StallZone);
	EditQuestTriggerType = (VolumeType == EVVT_QuestZone);
	EditDoor = (VolumeType == EVVT_Door);
	EditGuideTriggerType = (VolumeType == EVVT_Guide);
	EditMine = (VolumeType == EVVT_Mine);
	EditTeleport = (VolumeType == EVVT_CannotTeleport);
	EditLoseItem = (VolumeType == EVVT_CannotLoseItem);
	EditDropItem = (VolumeType == EVVT_CannotDropItem);
	EditNpcDropItem = (VolumeType == EVVT_NpcCannotDropItem);
}

// void AMMOVolume_Trigger::RegisterToFacade()
// {
// 	if( MMO::Facade::getSingletonPtr() )
// 		MMOFacade::getSingleton().RegisterTriggerVolume(this);
// }
// //------------------------------------------------------------------------
// INT AMMOVolume_Trigger::GetAreaID()
// {
// 	return AreaID;
// }
// //------------------------------------------------------------------------
// void AMMOVolume_Trigger::SetAutoID(INT Id)
// {
// 	AreaID = Id;
// }
// //------------------------------------------------------------------------
// FBox AMMOVolume_Trigger::GetObjectServerBounds()
// {
// 	FBox box = GetComponentsBoundingBox();
// 	return Util::ConvertToServerAABB(box);
// }
// //------------------------------------------------------------------------
// FVector AMMOVolume_Trigger::GetObjectServerPosition()
// {
// 	FBox box = GetObjectServerBounds();
// 	return Util::GetServerCoord(box);
// }
//------------------------------------------------------------------------
// TArray<class ADynamicBlockingVolume*> AMMOVolume_Trigger::GetBlockingVolumes()
// {
// 	return AssociatedBlockingVolume;
// }
//------------------------------------------------------------------------
// TArray<class ANPCSpawnPoint*> AMMOVolume_Trigger::GetNPCSpawnPoints()
// {
// 	return NPCSpawnPoints;
// }
//------------------------------------------------------------------------
// FString AMMOVolume_Trigger::GetClientName()
// {
// 	return GetName();
// }
////------------------------------------------------------------------------
//TArray<TScriptInterface<class IIServerTrigger> > AMMOVolume_Trigger::GetLinkedTrigger()
//{
//	return LinkedTrigger;
//}
////------------------------------------------------------------------------
//BYTE AMMOVolume_Trigger::GetTrapEventType()
//{
//	return TrapEventType;
//}
void AMMOVolume_Trigger::SetUniqueID(INT id)
{
	ServerUniqueID = id;
}

INT AMMOVolume_Trigger::GetUniqueID()
{
	return ServerUniqueID;
}

//song@snda.com
void AMMOVolume_Trigger::InitUniqueID()
{
	const int MaxSize = MAX_VOLUME_COUNT;
	bool UniqueIDs[MaxSize];
	memset(UniqueIDs, false, sizeof(UniqueIDs));
	for(TObjectIterator<AMMOVolume_Trigger> it; it; ++it)
	{
		if((*it)->ServerUniqueID > 0 && (*it)->ServerUniqueID < MaxSize/* && !(*it)->bDeleteMe && !(*it)->bPendingDelete*/)
		{
			if( (*it)->VolumeType == EVTT_Trigger  )// only count for trigger volume
			{
				if ( UniqueIDs[(*it)->ServerUniqueID] )
					(*it)->ServerUniqueID = -1;// to re-arrange
				else
					UniqueIDs[(*it)->ServerUniqueID] = true;
			}
		}
	}

	int nLastIndex = 1;
	for(TObjectIterator<AMMOVolume_Trigger> it; it; ++it)
	{
		if((*it)->ServerUniqueID <= 0 && (*it)->VolumeType == EVTT_Trigger )// only count trigger volume
		{
			int i = nLastIndex;
			for(; i < MaxSize; ++i)
			{
				if(!UniqueIDs[i])
				{
					(*it)->ServerUniqueID = i;
					UniqueIDs[i] = true;

					nLastIndex = i+1;
					break;
				}
			}
			if ( i >= MaxSize )
			{
				appErrorf( TEXT("Too Much AMMOVolume_Trigger with EVTT_Trigger! name: %s"), *((*it)->GetName()) );
			}
		}
	}
}
//song@snda.com

// add by zhangrui05@snda.com >
void AMMOVolume_Trigger::OnEnterQuestZone(class AMMOPlayer* Other)
{
	if (MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}

	if (Other && Other->IsMainPlayer())
	{
		if (Quests(0) > 0)
		{
			MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_EnterQuestZone, Quests(0));
			evt.QuestGridType = QuestTriggerType;
			MMOUI::PostGameEvent(&evt);
		}
	}
}

void AMMOVolume_Trigger::OnLeaveQuestZone(class AMMOPlayer* Other)
{
	if (MMO::GetCurrentGameState() != EGameState_InGame)
	{
		return;
	}

	if (Other && Other->IsMainPlayer())
	{
		if (Quests(0) > 0)
		{
			MMOGameEvent_Quest evt(MMOGameEvent_Quest::EQET_LeaveQuestZone, Quests(0));
			evt.QuestGridType = QuestTriggerType;
			MMOUI::PostGameEvent(&evt);
		}
	}
}

// add by zhangrui05@snda.com
void AMMOVolume_Trigger::OnTriggerGuide(class AMMOPlayer* Other)
{
	if (Other && Other->IsMainPlayer())
	{
		// Guide
		MMOGameEvent_Guide evtGuide(EGUET_EnterZone, GuideId);
		MMOUI::PostGameEvent(&evtGuide);
	}
}

void AMMOVolume_Trigger::OnEnterSafe(class AMMOPlayer* Other)
{
#if 0
	//if(Other)
	//{
	//	Other->IsInSafeZone = TRUE;
	//	if( Other->IsMainPlayer() ){

	//		//if (MMO::GetCurrentGameState() == EGameState_InGame && Other->CanUseAnyAbility() ){
	//		//	CMessageHandler::GetInstance().Send_CombatPoseOn(TRUE,0);//1);
	//		//}
	//		//else
	//		//{
	//		//	warnf(NAME_Error, TEXT("OnEnterSafe!"));
	//		//}
	//		MMOFacade::getSingleton().GetMMOBuffManager()->AddOrChangeBuff(NULL, Other->ServerID, SERVERID_SAFEZONE, BUFFID_SAFEZONE, 0, 0.0, 10000.0, 1,TRUE);
	//	}
	//}

	//if( Other && Other->IsMainPlayer() )
	//{
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage2")),MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage2") );
	//}
#endif
}

void AMMOVolume_Trigger::OnLeaveSafe(class AMMOPlayer* Other)
{
#if 0
	//if(Other)
	//{
	//	Other->IsInSafeZone = FALSE;
	//	

	//	if( Other->IsMainPlayer() ){

	//		//if (MMO::GetCurrentGameState() == EGameState_InGame  && Other->CanUseAnyAbility()){
	//		//	CMessageHandler::GetInstance().Send_CombatPoseOn(FALSE,0);//1);
	//		//}
	//		//else
	//		//{
	//		//	warnf(NAME_Error, TEXT("OnLeaveSafe!"));
	//		//}
	//		MMOFacade::getSingleton().GetMMOBuffManager()->DeleteBuff(Other->ServerID, SERVERID_SAFEZONE);// >AddOrChangeBuff(NULL, Other->ServerID, 2502999, 2502, 0, 1.0, 1.0, 1,TRUE);
	//	}

	//}

	//if( Other && Other->IsMainPlayer() )
	//{
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage4")),MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage4") );
	//}
#endif
}

void AMMOVolume_Trigger::OnEnterFree(class AMMOPlayer* Other)
{
#if 0
	//if( Other && Other->IsMainPlayer() )
	//{
	//	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.PkMode.PkMessage1")) ,MsgInfoType_Table,TEXT("MMOGame.PkMode.PkMessage1"));
	//}
#endif
}

