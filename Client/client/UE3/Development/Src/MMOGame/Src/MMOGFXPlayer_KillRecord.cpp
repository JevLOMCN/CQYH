#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOKillRecordManager.h"

IMPLEMENT_CLASS(UMMOGFXPlayer_KillRecord)

void UMMOGFXPlayer_KillRecord::Init()
{
	Super::Init();

	check(FALSE && "UMMOGFXPlayer_KillRecord::Init()");
	//MMOUI::RegisterMoviePlayer(this, MMOUI::GetUIManager()->MoviePlayer_KillRecord);

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_KillRecord);
	EventAdapter = new TGameEventAdapter<UMMOGFXPlayer_KillRecord>(this, evts);
}

void UMMOGFXPlayer_KillRecord::UnInit()
{
	Super::UnInit();
}

void UMMOGFXPlayer_KillRecord::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL)
		return;
	if(evt->GetEventType() == GameEvent_KillRecord)
	{
		MMOGameEvent_KillRecord*pkEvent = (MMOGameEvent_KillRecord*)evt;
		switch(pkEvent->EventType)
		{
		case MMOGameEvent_KillRecord::EKillRecordEvent_KillMonsterCount:
			{
				if(_killCountPanel)
				{
					_killCountPanel->Show(TRUE);
					_killCountPanel->eventCallUCSetNewKillCount(0,pkEvent->nCount);
				}
			}
			break;
		case MMOGameEvent_KillRecord::EKillRecordEvent_KillMonsterReward:
			{
				if(_killCountPanel)
					_killCountPanel->eventCallUCCloseCountPanel();
				if(_killRecordPanel)
				{
					_killRecordPanel->Show(TRUE);
					_killRecordPanel->eventCallUCSetNewRecord(pkEvent->bNew,0,pkEvent->nCount,pkEvent->nExp);
				}
			}
			break;
		case MMOGameEvent_KillRecord::EKillRecordEvent_KillMonsterRewardMemotary:
			{
				if(_killRecordPanel)
				{
					_killRecordPanel->Show(TRUE);
					_killRecordPanel->eventCallUCSetNewRecord(pkEvent->bNew,1,pkEvent->nCount,pkEvent->nExp);

					AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
					if(actor != NULL)
					{
						FString soundPackage = TEXT("VoiceTips");
						FString soundTemp = TEXT("InstantKillingNotice");

						if(actor->ActorAttributes->GenderType == EGT_Male)
							soundTemp += "_Male";
						else
							soundTemp += "_Female";

						if(actor->GetClassId() == EPCT_PlayerClassA || actor->GetClassId() == EPCT_PlayerClassG) // Lancer reuses Warrior voice
						{
							soundPackage +=  "_Warrior";
							soundTemp += "_Warrior";
						}
						else if(actor->GetClassId() == EPCT_PlayerClassB || actor->GetClassId() == EPCT_PlayerClassH) // Sorcerer reuses Mage voice
						{
							soundPackage +=  "_Mage";
							soundTemp += "_Mage";
						}
						else if(actor->GetClassId() == EPCT_PlayerClassC)
						{
							soundPackage +=  "_Rogue";
							soundTemp += "_Rogue";
						}
						else if(actor->GetClassId() == EPCT_PlayerClassD)
						{
							soundPackage +=  "_Archer";
							soundTemp += "_Archer";
						}
						else if(actor->GetClassId() == EPCT_PlayerClassE)
						{
							soundPackage +=  "_Taoist";
							soundTemp += "_Taoist";
						}
						FString strSound = FString::Printf(TEXT("SoundCue'%s.%s'"),*soundPackage,*soundTemp);
						USoundCue* hintSound = Cast<USoundCue>(StaticLoadObject( USoundCue::StaticClass(),
							NULL, *strSound, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));
						if(hintSound)
							actor->PlaySound(hintSound);	 
					}
				}
			}
			break;
		case MMOGameEvent_KillRecord::EKillRecordEvent_KillPLayerCount:
			{
				if(_killRecordPanel)
				{
					_killRecordPanel->Show(TRUE);
					_killRecordPanel->eventCallUCSetNewRecord(pkEvent->bNew,2,pkEvent->nCount,pkEvent->nExp);
				}
			}
			break;
		}
	}
}
void UMMOGFXPlayer_KillRecord::ASClosedKillRecordPanel()
{
	if(_killCountPanel)
		_killCountPanel->Show(FALSE);
}
void UMMOGFXPlayer_KillRecord::ASClosedKillCountPanel()
{
	if(_killRecordPanel)
		_killRecordPanel->Show(FALSE);
}