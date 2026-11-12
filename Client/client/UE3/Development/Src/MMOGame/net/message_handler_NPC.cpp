#include "stdafx.h"
#include "message_handler.h"
#include "UITextParser.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "CommonGameEvents.h"
#include "MMOGuildManager.h"

using namespace ExternalMessage;
//---------------------------------------------------------------------------------------------------------
void CMessageHandler::Send_InteractNpc(INT serverID)
{
	C2G_INTERACT_WITH_NPC msg;
	msg.wMsg = c2g_interact_with_npc;
	msg.dwNpcID = serverID;
	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Send_StopNPCInteract()
{
	C2G_STOP_INTERACT_WITH_NPC msg;
	msg.wMsg = c2g_stop_interact_with_npc;
	_Send(&msg, sizeof(C2G_STOP_INTERACT_WITH_NPC));
}
//------------------------------------------------------------------------------------------------------------------------------
void CMessageHandler::Send_NpcDialogOption(INT para)
{
	C2G_SELECT_DIALOG_OPTION msg;
	msg.wMsg = c2g_select_dialog_option;
	//msg.dwNpcID = NpcServeId;
	msg.nParam  = para;
	_Send(&msg, sizeof(msg));
}


void CMessageHandler::SyncNpcData(G2C_SYNC_NPC_EX_DATA* NewData)
{
	if (NewData == NULL)
	{
		return;
	}

	UNPCTemplate* NpcTemplate = MMOFacade::getSingleton().FindNPCTemplate(NewData->nTemplateID);
	if (NpcTemplate == NULL)
	{
		static TArray<INT> s_MissingNpcTemplates;
		if (s_MissingNpcTemplates.FindItemIndex(NewData->nTemplateID) == INDEX_NONE)
		{
			s_MissingNpcTemplates.AddItem(NewData->nTemplateID);
			warnf(NAME_Error, TEXT("!!!!!!!!!!!!can not find npc template by id:%d (logged once)"), NewData->nTemplateID);
		}
		return;
	}

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter( NewData->dwNpcID );
	if (actor == NULL)
	{
		warnf(NAME_Error, TEXT("!!!!!!!!!!!!no such npc whose id is : %d, templateId is : %d"), NewData->dwNpcID, NewData->nTemplateID);
		return;
	}

	ANPC* npc = Cast<ANPC>(actor);

	if (npc == NULL)
	{
		warnf(TEXT("Not a NPC, retur"));
		return;
	}


	if (npc->ActorAttributes)
	{
		if(NewData->byType==E_npct_Pet)
		{
			npc->dwHostID = NewData->dwHostID;
			npc->MasterName =  FString(UTF8_TO_TCHAR(NewData->szHostName));
			npc->ActorAttributes->PetLevel = NewData->byPetLevel;

		}
		else
		{
			npc->dwHostID = 0;
			npc->MasterName =  FString("");
		}
	}


	

	if (NpcTemplate != npc->Template)
	{		
		npc->Quality = NewData->byQuality;
		npc->Level = NewData->byLevel;
			
		npc->SetActorLevel(npc->Level);
		INT   OldHpPercent = 100;
		if (npc->ActorAttributes)
		{
			npc->ActorAttributes->PetLevel=NewData->byPetLevel;

			MMOFacade::getSingleton().UpdatePetLevelColor(npc);
			if(npc->ActorAttributes->PetLevel>0)
			{
				MMOUI::GetUIManager()->delegateUpdateHintText(npc,ESHU_NAME);
			}
			OldHpPercent = npc->ActorAttributes->HpPercent;
		}

		npc->InitNpcByTemplate(NpcTemplate);
		npc->ReCalGridBlockRadius();	//zhaojun@snda

		if (npc->ActorAttributes)
		{			
			npc->ActorAttributes->HpPercent = OldHpPercent;
			npc->ActorAttributes->SetMaxHP(NewData->nMaxHp);
			if(npc->ActorAttributes->HP>0 && npc->ActorAttributes->HP<=NewData->nMaxHp)
			{
				//Means sync_hp has already arrived
			}
			else
			{
				npc->ActorAttributes->SetHP(NewData->nMaxHp*OldHpPercent/100);
			}

			////npc->ActorAttributes->dwHostID = NewData->dwHostID;
			//npc->dwHostID = NewData->dwHostID;
			//npc->Master = MMOFacade::getSingleton().GetCharacter(NewData->dwHostID);
			//if(npc->Master){
			//	npc->Master->PetId = NewData->dwNpcID;
			//	npc->MasterName = npc->Master->ActorName;
			//}
			//else
			//{
			//	npc->MasterName = TEXT("");
			//}
		}

		if(npc->Template && (npc->GetActorType() != EACT_Pet))
		{
			npc->SetActorType( npc->Template->ActorType );
		}
		npc->AffixList.Empty();
	}
	else
	{
		if (npc->ActorAttributes)
		{
			//npc->ActorAttributes->PetLevel=NewData->byPetLevel;
			MMOFacade::getSingleton().UpdatePetLevelColor(npc);
			if(npc->ActorAttributes->PetLevel>0)
			{
				MMOUI::GetUIManager()->delegateUpdateHintText(npc,ESHU_NAME);
			}
		}

		npc->RefreshAnimSet();
	}

	npc->AcceptableQuestList.Empty();
	npc->CompassableQuestList.Empty();

	npc->bResourceInitialized = TRUE;
	npc->bActorDataReset = TRUE;
	//npc->fActorDataResetDelay = 0.3f;

	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();

	MMOFacade::getSingleton().NotifyAddNewNpcData(npc);
	
	// quest NPC
	MMOQuestManager::getSingleton().UpdateQuestNpcStatus(npc);

	// Guild building NPC
	MMOGuildManager::getSingleton().UpdateGuildBuilding(npc);
}


void CMessageHandler::Parse_g2c_play_action(const char* buf, UINT32 size)
{
	G2C_PLAY_ACTION* msg = (G2C_PLAY_ACTION*)buf;

	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(msg->dwCharID);
	if( actor)
	{
		actor->PlayActionById(msg->byActionID);
	}
}


void CMessageHandler::Parse_g2c_sync_npc_data(const char* buf, UINT32 size)
{
	//debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_sync_npc_data]"));
	G2C_SYNC_NPC_DATA* msg = (G2C_SYNC_NPC_DATA*)buf;

#if 0	
	const UNPCTemplate* npcTemplate = MMOFacade::getSingleton().FindNPCTemplate(msg->nTemplateID);
	debugf(TEXT("-----Parse_g2c_sync_npc_data NPCID:%d %d %s"), msg->dwNpcID, msg->nTemplateID, (npcTemplate ? *npcTemplate->GetDisplayName() : TEXT("")));

	//if(msg->nTemplateID == 6085)
	//{
	//	msg->nTemplateID = msg->nTemplateID;
	//}
#endif


	G2C_SYNC_NPC_EX_DATA* CacheData = m_NPCDataCache.GetCacheData(msg->dwNpcID);

	//INT2SyncNPCDataMap::iterator pos = m_NPCDataCache.find(msg->dwNpcID);
	if (CacheData == NULL)
	{
		G2C_SYNC_NPC_EX_DATA* NewNode = new G2C_SYNC_NPC_EX_DATA;
		check(NewNode);
		memcpy(NewNode, msg, sizeof(G2C_SYNC_NPC_DATA));
		NewNode->byType=E_npct_Npc;
		NewNode->byPetLevel=0;
		NewNode->dwHostID=0;

		m_NPCDataCache.AddNewData(msg->dwNpcID, NewNode);
		MMOFacade::getSingleton().AddOjbectToSync(msg->dwNpcID);
	}
	else
	{
		memcpy(CacheData, msg, sizeof(G2C_SYNC_NPC_DATA));
		CacheData->byType=E_npct_Npc;
		CacheData->byPetLevel=0;
		CacheData->dwHostID=0;
		MMOFacade::getSingleton().AddOjbectToSync(msg->dwNpcID);
	}
}


void CMessageHandler::Parse_g2c_sync_npc_ex_data(const char* buf, UINT32 size)
{
	//debugIDf( TEXT("Network"), TEXT("[CMessageHandler::Parse_g2c_sync_npc_data]"));
	G2C_SYNC_NPC_EX_DATA* msg = (G2C_SYNC_NPC_EX_DATA*)buf;


	G2C_SYNC_NPC_EX_DATA* CacheData = m_NPCDataCache.GetCacheData(msg->dwNpcID);

	//INT2SyncNPCDataMap::iterator pos = m_NPCDataCache.find(msg->dwNpcID);
	if (CacheData == NULL)
	{
		G2C_SYNC_NPC_EX_DATA* NewNode = new G2C_SYNC_NPC_EX_DATA;
		check(NewNode);
		memcpy(NewNode, msg, sizeof(G2C_SYNC_NPC_EX_DATA));
		m_NPCDataCache.AddNewData(msg->dwNpcID, NewNode);
		MMOFacade::getSingleton().AddOjbectToSync(msg->dwNpcID);
	}
	else
	{
		memcpy(CacheData, msg, sizeof(G2C_SYNC_NPC_EX_DATA));
		MMOFacade::getSingleton().AddOjbectToSync(msg->dwNpcID);
	}
}

//-----------------------------------------------------------------------
void CMessageHandler::Parse_g2c_interact_result(const char* buf, UINT32 size)
{
	G2C_INTERACT_RESULT* msg = (G2C_INTERACT_RESULT*)buf;
	ParseContext ctx;
	ANPC* npc = Cast<ANPC>( MMOFacade::getSingleton().GetCharacter(INT(msg->dwNpcID)) );
	if( npc)
	{
		ctx.NPC = npc;

        FDialogContent DC(EC_EventParm);

		AMMOActor* player = MMOFacade::getSingleton().GetMainActor();
		if(player && npc->Template && npc->Template->FaceToActorWhenInteract)
		{
            DC.NPCOldRot = npc->Rotation;
			FVector dir = player->Location - npc->Location;
			dir.SafeNormal2D();
			dir.Z = 0;
			npc->SetDesiredRotation(dir.Rotation());
			//npc->SetRotation(dir.Rotation());
		}
        else
            DC.NPCOldRot = FRotator::ZeroRotator;

        if( npc->Template )
        {
			MMOGuildManager::getSingleton().NotifyInteractBuildingNpc(msg->dwNpcID, npc->Template->GetTemplateID());

            MMOGameEvent_NPCInteract evt(msg->dwNpcID, npc->Template->GetTemplateID());
            MMOUI::PostGameEvent(&evt);
        }

		DialogContext* DialogText = new DialogContext;

		FString options = UTF8_TO_TCHAR(msg->szDialogContent);
		
		//options = UTF8_TO_TCHAR(TCHAR_TO_ANSI(*options));

#if defined(_DEBUG) && 0
		options = TEXT("<#D>你好啊，<#SelfName><#D/> \
							  <#O:1>需要我的特殊服务么？\n奖励：<#I:1012035> <O#/> \
							  <#SO:2><#T:MMOErrorInfo.ERROR_CODE_DEF.errDisconnectTimeOut><#SO/>\
							  <#T:MMOErrorInfo.ERROR_CODE_DEF.errDisconnectTimeOut>");		
#endif
		UITextParser::getSingleton().Parse(ctx, *DialogText, *options);
		
		ConvertDialog(DialogText, DC);
		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
			if (dialog)
			{
				dialog->OpenDialog(npc, DC);
			}
		}
	}
}

//-----------------------------------------------------------------------
void CMessageHandler::ConvertDialog(DialogContext* Dialog, FDialogContent& FDialog)
{
	FDialog.Contex = Dialog;
	for ( INT i = 0 ; i < Dialog->DialogText.Num() ; i++)
	{
		FDialog.DialogTextList.AddItem( Dialog->DialogText(i) );
	}
	
	FDialog.DialogAnswerId[0] = Dialog->TextId;
	FDialog.DialogAnswerId[1] = -1;
	FDialog.DialogAnswerId[2] = -1;
	FDialog.DialogAnswer[0] = Dialog->AnswerText;

	//appMemcmp( FDialog.DialogAnswerId , Dialog-> , sizeof(Dialog->DialogAnswerId) );

	//for ( INT i = 0 ; i < 3 ; i++ )
	//{
	//	FDialog.DialogAnswer[i] = Dialog->DialogAnswer[i];
	//}

	FDialog.QuestId = Dialog->QuestId;
}