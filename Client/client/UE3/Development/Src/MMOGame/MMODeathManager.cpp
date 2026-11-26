#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMODeathManager.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"

//=======================================================================================
MMODeathManager::MMODeathManager()
	: WaitTime(0), KillerID(0), PlayerKilled(FALSE), ReviveTime(0)
{
	appMemzero(ReviveInfo, sizeof(ReviveInfo));
}

void MMODeathManager::Init()
{
	//MMOUI::RegisterEventHandler( this, GameEvent_UnlockSkillSlot);
	//MMOUI::RegisterEventHandler( this, GameEvent_LearnedNewSkill);
	//MMOUI::RegisterEventHandler( this, GameEvent_SkillLevelUp);
	//MMOUI::RegisterEventHandler( this, GameEvent_FinishLoading);
	//MMOUI::RegisterEventHandler( this, GameEvent_EnterGameSuccess);
	GConfig->GetInt( TEXT("ReviveTime"), TEXT("ReviveTimeCount") , ReviveTime , GDefaultGameIni);
	PlayerKilled = FALSE;
	WaitTime = 0;
}
//=======================================================================================
void MMODeathManager::Shutdown()
{
	//MMOUI::UnRegisterEventHandler( this, GameEvent_EnterGameSuccess);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_FinishLoading);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_SkillLevelUp);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_LearnedNewSkill);
	//MMOUI::UnRegisterEventHandler( this, GameEvent_UnlockSkillSlot);
}


void MMODeathManager::SetReviveInfo(G2C_REVIVE_INFO* pInfo)
{
	if(pInfo)
	{
		for(INT i=crtInvalid ;i<crtTotal; i++){
			memcpy(&ReviveInfo[i] , &(pInfo->ReviveInfo[i]) , sizeof(REVIVE_INFO) );
		}
		WaitTime = pInfo->nWaitTime;
		if(WaitTime<0)WaitTime=0;
		KillerID = pInfo->dwKillerID;
	}
}

REVIVE_INFO* MMODeathManager::GetReviveInfo(CHAR_REVIVE_TYPE tp)
{
	if(tp<crtTotal && tp>crtInvalid ){
		return &ReviveInfo[tp];
	}
	return NULL;
}

void MMODeathManager::NoityfyShowDeathPanel()
{
	if(MMO::GetCurrentGameState() != EGameState_InGame)
		return;

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(KillerID);
	if (actor)
	{
		FString strHint;
		if (actor->IsA(ANPC::StaticClass() ))
		{
			PlayerKilled = FALSE;
			ANPC* npc = Cast<ANPC>(actor);
			if(npc&&npc->Template)
				strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.KillInfo.ToSysChat")),*(npc->Template->GetDisplayName()));
		}
		else if(actor->IsA(AMMOPlayer::StaticClass()) && actor->ServerID != MMOFacade::getSingleton().GetMainPlayer()->ServerID)
		{
			PlayerKilled = TRUE;
			strHint = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.KillInfo.ToSysChat")),*(actor->ActorName));
		}
		
		MMOChatManager::getSingleton().AddSystemMessage(strHint); 
	}
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Death,TRUE, "");
}
