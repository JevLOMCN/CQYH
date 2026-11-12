#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOBossBloodManager.h"
#include "MMOFacade.h"

//=======================================================================================
void MMOBossBloodManager::Init()
{
	MMOUI::RegisterEventHandler(this, GameEvent_NPCAppear);
	MMOUI::RegisterEventHandler(this, GameEvent_NPCDestroyed);
	nBossID = 0;
}
//=======================================================================================
void MMOBossBloodManager::Shutdown()
{
	nBossID = 0;
}
void MMOBossBloodManager::SetBossID(INT nBoss)
{
	nBossID = nBoss;
}
INT MMOBossBloodManager::GetBossID()
{
	return nBossID;
}
void MMOBossBloodManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{
	switch(evt->GetEventType())
	{
	case GameEvent_NPCAppear:
		{
			MMOGameEvent_NPCAppear* actorEvt = (MMOGameEvent_NPCAppear*)evt;
			if(actorEvt->Actor && actorEvt->Actor->IsA(ANPC::StaticClass()))
			{
				ANPC* npc = Cast<ANPC>( actorEvt->Actor );
				if(npc)
				{
					EActorClassType actorType = (EActorClassType)npc->GetActorType();
					if(actorType == EACT_Boss)
					{
						MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_BossBlood, TRUE, "");
						nBossID = npc->ServerID;
						MMOGameEvent_BossBlood  eBossEvent(MMOGameEvent_BossBlood::EBossBlood_Event_Appear);
						eBossEvent.title = npc->DisplayName;
						eBossEvent.byteQuality = npc->Quality;
						if(npc->ActorAttributes)
						{
							eBossEvent.nMaxHp = npc->ActorAttributes->MaxHP;
							eBossEvent.nCurHp = npc->ActorAttributes->HP;
							eBossEvent.nLevel = npc->ActorAttributes->LevelAttributes.Level;
							AMMOActor* aActor = Cast<AMMOActor>( actorEvt->Actor );
							//Since the server did not sync npc attributes at the start, the initial value is 0,
							if(aActor&&eBossEvent.nCurHp == 0 && aActor->eventIsDead()==FALSE)
								eBossEvent.nCurHp = npc->ActorAttributes->MaxHP;
						}
						MMOUI::PostGameEvent(&eBossEvent);
					}
				}
			}break;
		}
	case GameEvent_NPCDestroyed:
		{
			MMOGameEvent_NPCDestroyed* actorEvt = (MMOGameEvent_NPCDestroyed*)evt;
			if(actorEvt->Actor && actorEvt->Actor->IsA(ANPC::StaticClass()))
			{
				ANPC* npc = Cast<ANPC>( actorEvt->Actor );
				if(npc)
				{
					EActorClassType actorType = (EActorClassType)npc->GetActorType();
					if(actorType == EACT_Boss&&nBossID == npc->ServerID)
					{
						MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_DynamicUI_BossBlood, FALSE, "");
						MMOGameEvent_BossBlood  eBossEvent(MMOGameEvent_BossBlood::EBossBlood_Event_Destroy);
						MMOUI::PostGameEvent(&eBossEvent);
					}
				}
			}
		}
		break;
	}
}