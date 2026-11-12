#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "DialogOptionAction.h"
#include "UITextParser.h"
#include "FSMIO.h"
#include "MMOQuestManager.h"
#include "LinkManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOGuildManager.h"
#include "MMOGuideManager.h"
#include "EngineAudioDeviceClasses.h"
#include "ClientActionConditionCheck.h"
#include "MMOChatManager.h"
#include "MMOEquipManager.h"

IMPLEMENT_CLASS(UMMOUIDialogNew);

void UMMOUIDialogNew::Init()
{
	Super::Init();

	//check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_TargetChange);
	evts.AddItem(GameEvent_Quest);
	evts.AddItem(GameEvent_ShowActivation);
	EventAdapter = new TGameEventAdapter<UMMOUIDialogNew>(this, evts);
}

void UMMOUIDialogNew::UnInit()
{
	Super::UnInit();
}

void UMMOUIDialogNew::PostOnShow(UBOOL show)
{
	Super::PostOnShow(show);

	if (show)
	{
		INT h = GetInt(TEXT("height"));
		INT w = GetInt(TEXT("width"));
		UGFxObject* bg = this->GetObject(TEXT("_diag_bg_mc"));
		if (bg)
		{
			h = bg->GetInt(TEXT("height"));
			w = bg->GetInt(TEXT("width"));
		}

		FVector2D screenSize = MMOFacade::GetScreenSize();
		FLOAT x = 0.0f, y = 0.0f;

		this->GetPosition(x, y);
		this->SetPosition(INT(screenSize.X - w) / 2 + 70, screenSize.Y - h);
	}
	else
	{
		CloseAllPopUpPanel();

		MMOGameEvent_Guide evtGuide(EGUET_CloseDialog);
		MMOUI::PostGameEvent(&evtGuide);
	}
}

void UMMOUIDialogNew::CloseAllPopUpPanel()
{
	if( ConfirmBox != NULL ){
		MMOMsgBoxManager::getSingleton().DispatchMsgBoxButtonNo( ConfirmBox );
		ConfirmBox = NULL;
	}
}

void UMMOUIDialogNew::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_TargetChange:
		{
			MMOGameEvent_TargetEvent* targetEvt = (MMOGameEvent_TargetEvent*)evt;
			if(targetEvt == NULL)
			{
				return;
			}

			if(targetEvt->EventType == MMOGameEvent_TargetEvent::ETCT_DesiredTarget)
			{
				// If the target changed and is not the same one, close first
				if(DialogNpc && targetEvt->DesiredTarget != DialogNpc)
				{
					CloseDialog();
				}
			}
		}
		break;

	case GameEvent_Quest:
		{
			MMOGameEvent_Quest* questEvt = (MMOGameEvent_Quest*)evt;
			if(questEvt == NULL)
			{
				return;
			}

			const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questEvt->QuestId);
			if (!quest)
			{
				return;
			}

			switch(questEvt->EventType)
			{
			case MMOGameEvent_Quest::EQET_AcceptQuest:
			case MMOGameEvent_Quest::EQET_CompleteQuest:
				{
					TryPopupQuest(questEvt->QuestId);
				}
				break;
			
			case MMOGameEvent_Quest::EQET_ShareQuest:
				{
					OpenQuestPopupUp(TRUE, questEvt->QuestId, TRUE, questEvt->ShareSrcId);	
				}
				break;

			case MMOGameEvent_Quest::EQET_UpdateQuestPage:
				{
					eventCallUCUpdateDiagPage(MMOQuestManager::getSingleton().GetQuestDialogCurrentPage());
					eventCallUCPlay(MMOQuestManager::getSingleton().GetPageTotalTime()*1000);
				}
				break;
			}
		}
		break;
	case GameEvent_ShowActivation:
		{
			FScriptDelegate funOk(EC_EventParm);
			funOk.FunctionName = "ActivationOk";
			funOk.Object = this;

			FScriptDelegate funNull(EC_EventParm);
			funNull.FunctionName = "ActivationCancel";
			funNull.Object = this;

			FString msg = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.Activation") );

			UGFxObject* msgBox = MMOMsgBoxManager::getSingleton().AddModalInputMsgbox(msg  , 0 , 
				msg , funOk,  funNull, eInputFormatAll );

			if (msgBox)
			{
				UGFxObject* input = msgBox->GetObject( TEXT("textInput") );
				if(input)
					input->SetInt( TEXT("maxChars") , 16 );
			}
			
		}
		break;
	}
}

void UMMOUIDialogNew::SetVisible(UBOOL bValue)
{
    __super::SetVisible(bValue);
}

void UMMOUIDialogNew::TryPopupQuest(INT newCompleteQuestID)
{
	AMMOPlayerController* mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if (!mainPlayerCtrl)
	{
		return;
	}

	// If a matinee is playing at this time, do not pop up
	if (mainPlayerCtrl->IsPlayingMatinee())
	{
		CloseDialog();
		return;
	}

	if (!DialogNpc)
	{
		return;
	}

	TArray<const UQuest*> compassableList;
	for (INT i = 0; i < DialogNpc->CompassableQuestList.Num(); ++ i)
	{
		UQuest* quest = DialogNpc->CompassableQuestList(i);
		if (quest && MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID()))
		{
			compassableList.AddItem(quest);				
		}
	}

	// After completing a quest, if there are other available quests, automatically pop up the quest accept UI
	if (compassableList.Num() == 1)
	{
		const UQuest* quest = compassableList(0);
		if (quest)
		{
			OpenQuestPopupUp(FALSE, quest->GetTemplateID(), FALSE, 0);
		}
		return;
	}
	else if (compassableList.Num() > 1)
	{
		ParseContext ctx;
		DialogContext* DialogText = new DialogContext;
		UITextParser::getSingleton().Parse(ctx, *DialogText, TEXT(""));

		FDialogContent DC(EC_EventParm);
		CMessageHandler::GetInstance().ConvertDialog(DialogText, DC);
		OpenDialog(DialogNpc, DC);
		return;
	}

	TArray<const UQuest*> acceptableList;
	for (INT i = 0; i < DialogNpc->AcceptableQuestList.Num(); ++ i)
	{
		const UQuest* quest = DialogNpc->AcceptableQuestList(i);
		if (quest->GetTemplateID() != newCompleteQuestID
			&& MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID())
			&& quest->QuestType == E_qt_Trunk) // #15294 designer changed it so only main quests auto pop up
		{
			acceptableList.AddItem(quest);
		}
	}

	if (acceptableList.Num() == 1)
	{
		const UQuest* quest = acceptableList(0);
		if (quest)
		{
			OpenQuestPopupUp(TRUE, quest->GetTemplateID(), FALSE, 0);
		}
		return;
	}
	else if (acceptableList.Num() > 1)
	{
		ParseContext ctx;
		DialogContext* DialogText = new DialogContext;
		UITextParser::getSingleton().Parse(ctx, *DialogText, TEXT(""));

		FDialogContent DC(EC_EventParm);
		CMessageHandler::GetInstance().ConvertDialog(DialogText, DC);
		OpenDialog(DialogNpc, DC);
		return;
	}

	CloseDialog();
}

void UMMOUIDialogNew::TryPopupQuestWithoutNpc(INT questId)
{
	CloseDialog(FALSE);

	if (questId == INVALID_QUEST_ID)
	{
		return;
	}

	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return;
	}

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	// No NPC, show the main character portrait
	AMMOPlayer* clonePlayer = eventClonePlayer(mainPlayer);
	SetShowActor(clonePlayer);

	OpenQuestPopupUp(TRUE, quest->GetTemplateID(), FALSE, 0);
}

void UMMOUIDialogNew::SetShowActor(AMMOActor* actor)
{
	if( !GFXPlayer )
	{
		return;
	}

	if (!actor)
	{
		return;
	}

	ShowLevel = GFXPlayer->eventGetShowLevel( "HalfShowLevel" );
	if (!RTSet && ShowLevel && ShowLevel->RenderTarget )
	{
		//GFXPlayer->SetExternalTexture( TEXT("NpcDialogAvatar -nopack"), ShowLevel->RenderTarget );
		PendingRTCounter = 1;	// set the texture 1 frame later
	}

	if (ShowLevel)
	{
		ShowLevel->SetEnabled( TRUE );
		FName tag( "C_01", FNAME_Find );
		if( actor->UseingAvatar )
			tag = actor->UseingAvatar->PlacementNodeTag;
		// change to dynamic lighting
		if( actor->LightEnvironment )
			actor->LightEnvironment->SetEnabled( FALSE );
		ShowLevel->SetShowActor(actor, tag);
	}
}

void UMMOUIDialogNew::CheckPendingRT( FLOAT deltaTime )
{
	if( RTSet )
		return;

	if( PendingRTCounter > 0 )
	{
		--PendingRTCounter;
	}
	else if( PendingRTCounter == 0 /*&& ShowLevel && ShowLevel->RenderTarget*/ )
	{
		GFXPlayer->SetExternalTexture( TEXT("NpcDialogAvatar__nopack"), ShowLevel->RenderTarget );
		PendingRTCounter = -1;
		RTSet = true;
	}
}

void UMMOUIDialogNew::OpenDialog(class ANPC* NPC , struct FDialogContent DiaCont)//const FString& DT,INT QI,const FString& A0,INT AI0,const FString& A1,INT AI1,const FString& A2,INT AI2)
{
	ClearContex();

	DialogNpc = NPC;
    DialogNpcRot = DiaCont.NPCOldRot;

	//MMOTargetManager::getSingleton().SetD .SetDesireTarget(NULL,NPC);// .SetCurrentTarget(DialogNpc);

	ANPC* showNPC = eventCloneNPC( NPC );
	if( showNPC )
	{
		showNPC->Mesh->bUseFullLevelMipmaps = TRUE;
		SetShowActor(showNPC);
	}

	CurDialogContent = DiaCont;

	//SetVisible(TRUE);
	//GotoAndPlayI(1);
	UpdateNPCDialog();
}
//------------------------------------------------------------------------
void UMMOUIDialogNew::ClearContex()
{
	// Manually newed earlier, must be manually deleted
	if(CurDialogContent.Contex)
	{
		delete CurDialogContent.Contex;
		CurDialogContent.Contex = 0;
	}
}

void UMMOUIDialogNew::CloseDialog(UBOOL playSound)
{
    if (!DialogNpc)
        return;

    if (DialogNpc && !DialogNpcRot.IsZero())
    {
        DialogNpc->SetDesiredRotation(DialogNpcRot);
        DialogNpcRot = FRotator::ZeroRotator;
    }

	ClearContex();

	if (playSound)
	{
		PlayDialogNpcSound(TRUE);
	}
	else
	{
		MMOFacade::getSingleton().StopAllVoiceSound(FALSE, 2.0, 0.0);
	}
	
	if (ShowLevel)
	{
		ShowLevel->SetEnabled(FALSE);
	}
    DialogNpc = 0;

	//Clear context
	MMOQuestManager::getSingleton().ResetContent();

	Show(FALSE);

	// Prevent the dialog from automatically showing again after being temporarily hidden by Matinee or scene switch
	LastVisible = FALSE;

	//CMessageHandler::GetInstance().Send_StopNPCInteract();
}
//------------------------------------------------------------------------

typedef std::vector<UQuest*, M_UE3_STL_ALLOC(UQuest*) > SortQuestList;

bool SortQuestFunctor(const UQuest* lhs, const UQuest* rhs)
{
	bool r = lhs->QuestType < rhs->QuestType;

	return r;
}

void SortQuest(SortQuestList& src)
{
	std::sort(src.begin(), src.end(), SortQuestFunctor);
}

void SetupQuestDiagOption(UQuest* quest, EQuestHUDStatus status, UGFxMoviePlayer* GFXPlayer, FDialogContent& CurDialogContent, UGFxObject* option_list, INT& optionIndex)
{
	if (!quest)
	{
		return;
	}

	if (!GFXPlayer)
	{
		return;
	}

	if (!(CurDialogContent.Contex))
	{
		return;
	}

	if (!option_list)
	{
		return;
	}

	UGFxObject* optionData = GFXPlayer->CreateObject(TEXT("ActionScript3.NPCDialog.NPCDialogOptionData"));
	if (!optionData)
	{
		return;
	}

	optionData->SetInt( TEXT("option_type"), EOptionType_Quest);

	// First add the client's dialog option ID
	CurDialogContent.Contex->AdvanceClientActionID();
	optionData->SetInt( TEXT("option_id"), CurDialogContent.Contex->ClientActionAutoID );
	DialogOptionAction_UICommand_OpenQuest* questAction = new DialogOptionAction_UICommand_OpenQuest();
	questAction->SetQuestTemplateID(quest->QuestTempID);

	CurDialogContent.Contex->OptionActionMap.insert(
		std::make_pair(CurDialogContent.Contex->ClientActionAutoID, questAction));

	FString questOptCap = UITextParser::HTMLFormatQuestName(quest, false, true);
	optionData->SetString( TEXT("option_label"), questOptCap);

	FString icon = TEXT("");
	MMOQuestManager::getSingleton().GetQuestStateIcon(quest->QuestType, status, TRUE, icon);
	optionData->SetString( TEXT("option_icon"), icon);

	option_list->SetElementObject(optionIndex, optionData);
	++optionIndex;
}

void UMMOUIDialogNew::PlayDialogNpcSound(UBOOL bClose)
{
	if (!DialogNpc)
	{
		return;
	}

	if (!DialogNpc->Template)
	{
		return;
	}

	if (bClose)
	{
		if (DialogNpc->CanPlayCloseSoundNow())
		{
			DialogNpc->StartCloseSoundPlayCD();
			DialogNpc->PlayRandomSound(DialogNpc->Template->Sound_CloseDiag);
		}
	}
	else
	{
		if (DialogNpc->CanPlayOpenSoundNow())
		{
			DialogNpc->StartOpenSoundPlayCD();
			if (DialogNpc->Template->Sound_OpenDiag.Num() > 0)
			{
				DialogNpc->PlayRandomSound(DialogNpc->Template->Sound_OpenDiag);
			}
			else
			{
				DialogNpc->PlayTalkSound();
			}
		}
	}
}

void UMMOUIDialogNew::UpdateNPCDialog()
{
	UBOOL hasDiagContent = FALSE;

	UGFxObject* dataArr = GFXPlayer->CreateArray();	
	//check(dataArr != NULL);
	if(dataArr == NULL) return;

	UGFxObject* diagData = GFXPlayer->CreateObject(TEXT("ActionScript3.NPCDialog.NPCDialogData"));
	//check(diagData != NULL);
	if(diagData == NULL) return;

	dataArr->SetElementObject(0, diagData); // only one

	if (FALSE == CurDialogContent.Contex->TempText.IsEmpty())
	{
		hasDiagContent = TRUE;
	}

	FString dlgText = TEXT("");
	if (CurDialogContent.Contex->TempText != TEXT(""))
	{
		dlgText = CurDialogContent.Contex->TempText;
	} 
	else if (DialogNpc)
	{
		dlgText = DialogNpc->DefaultDialogText;
	}
	diagData->SetString(TEXT("diag_text"), dlgText);

	if (DialogNpc)
	{
		diagData->SetString(TEXT("diag_npc_name"), DialogNpc->DisplayName);
	}
	else
	{
		diagData->SetString(TEXT("diag_npc_name"), TEXT(""));
	}
	diagData->SetInt(TEXT("diag_type"), (INT)EDiagDataType_Dialog);
	
	// If it is a clan dungeon envoy, insert option
	UNPCTemplate* npcTemplateNew = NULL;
	
	if (DialogNpc)
	{
		npcTemplateNew = MMOFacade::getSingleton().FindNPCTemplate(DialogNpc->NPCID);
	}

	if (npcTemplateNew && npcTemplateNew->IsGuildDungeon)
	{
		GuildDungeonDataMap guildDungeonDataMap = MMOGuildData::getSingleton().GetGuildDungeonDataMap();
		GuildDungeonDataMap::iterator iter = guildDungeonDataMap.begin();
		for (; iter != guildDungeonDataMap.end(); ++ iter)
		{
			GuildDungeonData data = iter->second;
			DialogOptionAction_UICommand_GuildDungeon* guildDungeonAction = new DialogOptionAction_UICommand_GuildDungeon();
			guildDungeonAction->SetDungeonId(data.DungeonId);
			guildDungeonAction->Parse(data.DungeonName, NULL);
			
			CurDialogContent.Contex->AdvanceClientActionID();
			CurDialogContent.Contex->OptionActionMap.insert(std::make_pair(CurDialogContent.Contex->ClientActionAutoID, guildDungeonAction));
		}
	}

	DialogContext::DialogOptionActionMap::iterator it =
		CurDialogContent.Contex->OptionActionMap.begin();

	INT optionIndex = 0;

	UGFxObject* option_list = GFXPlayer->CreateArray();	
	//check(option_list != NULL);
	if(option_list == NULL) return;

	// Add dialog options created from text parsing
	for (; it != CurDialogContent.Contex->OptionActionMap.end(); ++it)
	{
		UGFxObject* optionData = GFXPlayer->CreateObject(TEXT("ActionScript3.NPCDialog.NPCDialogOptionData"));
		//check(optionData != NULL);
		if(optionData == NULL) return;

		optionData->SetInt( TEXT("option_type") , EOptionType_Default);
		optionData->SetInt( TEXT("option_id") , it->first );
		optionData->SetString( TEXT("option_label") , it->second->GetDisplayText());
		optionData->SetString( TEXT("option_icon"), TEXT("img://UI_NPCDialog.T_DiagOptionIcon")); // TODO:
		option_list->SetElementObject(optionIndex, optionData);
		++optionIndex;
	}

	if(!CurDialogContent.Contex->OptionActionMap.empty())
	{
		hasDiagContent = TRUE;
	}
	
	diagData->SetObject(TEXT("option_list"), option_list);

	UBOOL hasQuestContent = FALSE;
	// Quest list configured on the NPC template
	if (DialogNpc != NULL)
	{
		SortQuestList canCompleteList, acceptedList;
		for( INT i =0 ; i < DialogNpc->CompassableQuestList.Num() ; i++ )
		{
			UQuest* quest = DialogNpc->CompassableQuestList(i);

			if( quest && MMOQuestData::getSingleton().IsQuestCompassable(quest->GetTemplateID()) )
			{
				canCompleteList.push_back(quest);				
			}
			else if(quest && MMOQuestData::getSingleton().IsQuestAccepted(quest->GetTemplateID()))
			{
				acceptedList.push_back(quest);
			}
		}

		// Completable quest list
		SortQuest(canCompleteList);

		for(size_t j = 0; j < canCompleteList.size(); ++j)
		{
			UQuest* quest = canCompleteList[j];
			SetupQuestDiagOption(quest, EQHUDS_CanSubmit, GFXPlayer, CurDialogContent, option_list, optionIndex);
		}

		// Acceptable quest list
		SortQuestList acceptableList;
		for(INT i = 0; i < DialogNpc->AcceptableQuestList.Num(); i++)
		{
			UQuest* quest = DialogNpc->AcceptableQuestList(i);

			if( quest && MMOQuestData::getSingleton().IsQuestAvailable(quest->GetTemplateID()) )
			{
				acceptableList.push_back(quest);
			}
		}

		SortQuest(acceptableList);

		for(size_t k = 0; k < acceptableList.size(); ++k)
		{
			UQuest* quest = acceptableList[k];
			SetupQuestDiagOption(quest, EQHUDS_Acceptable, GFXPlayer, CurDialogContent, option_list, optionIndex);
		}

		// Already accepted quest list
		SortQuest(acceptedList);

		for(size_t k = 0; k < acceptedList.size(); ++k)
		{
			UQuest* quest = acceptedList[k];
			SetupQuestDiagOption(quest, EQHUDS_NotValidForSubmit, GFXPlayer, CurDialogContent, option_list, optionIndex);
		}

		hasQuestContent = !acceptableList.empty() || !acceptedList.empty() || !canCompleteList.empty();
		hasDiagContent = hasDiagContent || hasQuestContent;

		// Tutorial event
		MMOGameEvent_Guide evtGuide(EGUET_OpenNpcDialog, DialogNpc->NPCID);
		MMOUI::PostGameEvent(&evtGuide);
	}

	if (CurDialogContent.Contex->OptionActionMap.size() == 0)
	{
		if (CurDialogContent.Contex->TempText.IsEmpty())
		{// If there are no dialog options, do not pop up (requirement #8589)
			CloseDialog(FALSE);
			PlayDialogNpcSound(FALSE);
			return;
		}
		else if (DialogNpc && DialogNpc->Template && DialogNpc->Template->DefaultDialogTextWithBubble)
		{
			UMMOUISceneHint* sceneHint = (UMMOUISceneHint*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_SceneHint);
			if (sceneHint)
			{
				sceneHint->AddActorTalkBubble(DialogNpc->ServerID, DialogNpc->Template->DefaultDialogText);
				CloseDialog(FALSE);
				PlayDialogNpcSound(FALSE);
				return;
			}
		}
	}
	else if (CurDialogContent.Contex->OptionActionMap.size() == 1)
	{// If there is only one dialog option, do it directly
		DialogContext::DialogOptionActionMap::iterator iter = CurDialogContent.Contex->OptionActionMap.begin();
		if (iter != CurDialogContent.Contex->OptionActionMap.end())
		{
			DialogContext::IDialogOptionActionPtr action = iter->second;
			if (action)
			{
				if (action->GetTypeName() == TEXT("OpenPanel") || 
					action->GetTypeName() == TEXT("OpenShop")   ||
					action->GetTypeName() == TEXT("BlackMarket"))

				{
					action->Execute();

					CloseDialog(FALSE);
					PlayDialogNpcSound(FALSE);
					return;
				}
				else if (action->GetTypeName() == TEXT("OpenQuest"))
				{
					DialogOptionAction_UICommand_OpenQuest* questAction = (DialogOptionAction_UICommand_OpenQuest*)(action.get());
					if (questAction)
					{
						questAction->Execute();
						// If it is an item-collection quest, do not show the dialog, but keep DialogNpc, so CloseDialog cannot be used, only Show(FALSE)
						if (MMOQuestData::getSingleton().IsQuestRecycle(questAction->GetQuestTemplateID()))
						{
							Show(FALSE);
							return;
						}
						else
						{
							if (hasQuestContent)
							{
								Show(TRUE);
							} 
							else
							{
								CloseDialog(FALSE);
							}
							return;
						}
					}
				}
			}
		}
	}

	eventCallUCUpdateDiagData(dataArr);
	if (hasDiagContent)
	{
		Show(TRUE);
	} 
	else
	{
		CloseDialog(FALSE);
	}
	PlayDialogNpcSound(FALSE);
}

//------------------------------------------------------------------------
void UMMOUIDialogNew::SelectDiagOption(int optionID)
{
	DialogContext::DialogOptionActionMap::iterator i =
		CurDialogContent.Contex->OptionActionMap.find(optionID);

	if(i != CurDialogContent.Contex->OptionActionMap.end())
	{
		i->second->Execute();
	}

	//CloseDialog();
}

//------------------------------------------------------------------------
/*
void UMMOUIDialogNew::TickForTestInteractNPCDistance(FLOAT deltaTime)
{	
	//if(this->IsVisible && !g_forceShowDiag)
	//{
	//	AMMOActor* targetActor = TargetManager::getSingleton().GetCurrentTarget();

	//	if(!targetActor)
	//	{
	//		Show(FALSE);
	//		CloseDialog();
	//	}
	//}//end of if IsVisible
}
*/

void UMMOUIDialogNew::TrySubmitQuest(INT questID)
{
	INT selectAwardItemCount = MMOQuestData::getSingleton().GetSelectableAwardItemCount(questID);
	if (selectAwardItemCount <= 1)
	{
		// If there are no or only one class-matching selectable rewards, get the reward index
		SelectedAwardIndex = MMOQuestData::getSingleton().GetDefaultSelectableAwardItemIndex(questID);
		MMOQuestManager::getSingleton().CompleteQuest(questID, SelectedAwardIndex);
		SelectedAwardIndex = -1;
	}
	else
	{
		if (SelectedAwardIndex == -1)
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.SelectAwardTip"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Quest.SelectAwardTip"));
		}
		else
		{
			MMOQuestManager::getSingleton().CompleteQuest(questID, SelectedAwardIndex);
			SelectedAwardIndex = -1;
		}
	}

	// Tutorial event
	MMOGameEvent_Guide evtGuide(EGUET_DialogCompleteQuest);
	MMOUI::PostGameEvent(&evtGuide);
}

void UMMOUIDialogNew::OpenQuestPopupUp(UBOOL accept, INT questID, UBOOL share, INT shareSrcId)
{
	const UQuest* questData = MMOQuestData::getSingleton().GetQuestData(questID);
	if(!questData)
	{
		return;
	}
	UGFxObject* dataArr = GFXPlayer->CreateArray();	
	//check(dataArr != NULL);
	if(dataArr == NULL) return;

	MMOQuestManager::getSingleton().SetPageQuestId(questID);
	//Reset time and page count
	CurrentDialogPage = 0;
	MMOQuestManager::getSingleton().SetQuestDialogCurrentPage(CurrentDialogPage);
	MMOQuestManager::getSingleton().RestPageTime();
	MMOQuestManager::getSingleton().ClearContext();
	//Parse various dialogs
	MMOQuestManager::getSingleton().ParseQuestDialog(questID,TRUE,*MMOQuestManager::getSingleton().GetQuestDialongContext());
	
	//Determine whether to paginate
	if (MMOQuestManager::getSingleton().GetMaxDialogPage() <= MIN_PAGENUMBER)
	{
		UGFxObject* diagData = CreateDialogContext(0,questID,share,shareSrcId,EDialogContextType_Quest);

		dataArr->SetElementObject(0, diagData);  

		// Requirement 11961
		MMOQuestManager::getSingleton().PlayQuestSound(questID, DialogNpc);

		// Tutorial event
		MMOGameEvent_Guide evtGuide(EGUET_OpenQuestDialogSinglePage, DialogNpc ? DialogNpc->NPCID : 0);
		MMOUI::PostGameEvent(&evtGuide);
	}
	else
	{
		for (int i =0;i<MMOQuestManager::getSingleton().GetMaxDialogPage();i++)
		{
			UGFxObject* diagData;

			if (i == MMOQuestManager::getSingleton().GetMaxDialogPage()-1)
			{
				diagData = CreateDialogContext(i,questID,share,shareSrcId,EDialogContextType_Quest_Page);
			}
			else
			{
				diagData = CreateDialogContext(i,questID,share,shareSrcId,EDialogContextType_Dialog);
			}
			dataArr->SetElementObject(i, diagData); 
		}

		// Tutorial event
		MMOGameEvent_Guide evtGuide(EGUET_OpenQuestDialogMultiPage, DialogNpc ? DialogNpc->NPCID : 0);
		MMOUI::PostGameEvent(&evtGuide);
	}

	eventCallUCUpdateDiagData(dataArr);
	eventCallUCPlay(MMOQuestManager::getSingleton().GetPageTotalTime()*1000);
	Show(TRUE);
}

void UMMOUIDialogNew::TryPopupQuestOver(INT questId)
{
	CloseDialog(FALSE);

	UQuest* quest = (UQuest*)MMOQuestData::getSingleton().GetQuestData(questId);
	if(!quest)
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateArray();	
	//check(object != NULL);
	if(object == NULL) return;

	MMOQuestManager::getSingleton().SetPageQuestId(questId);
	//Reset time and page count
	CurrentDialogPage = 0;
	MMOQuestManager::getSingleton().SetQuestDialogCurrentPage(CurrentDialogPage);
	MMOQuestManager::getSingleton().RestPageTime();
	MMOQuestManager::getSingleton().ClearContext();
	// Parse various dialogs
	MMOQuestManager::getSingleton().ParseQuestDialog(questId,TRUE,*MMOQuestManager::getSingleton().GetQuestDialongContext());

	for (int i =0;i<MMOQuestManager::getSingleton().GetMaxDialogPage();i++)
	{
		UGFxObject* diagData= CreateDialogContext(i,questId,FALSE,0,EDialogContextType_Dialog);
		object->SetElementObject(i, diagData); 

		if (MMOQuestManager::getSingleton().GetMaxDialogPage() <= MIN_PAGENUMBER)
		{
			break;
		}
	}

	if (MMOQuestData::getSingleton().IsQuestAccepted(questId) &&
		!MMOQuestData::getSingleton().IsQuestCompleted(questId))
	{
		UNPCTemplate* autoStartNpc = ((UQuest*)quest)->GetAutoStartDialogHeadIcon();
		UNPCTemplate* startNpc = ((UQuest*)quest)->GetStartNpc();
		if (autoStartNpc)
		{
//			ANPC* showNPC = eventCloneNPC(quest->GetAutoStartDialogHeadIcon());
//			SetShowActor(showNPC);
		} 
		else if (startNpc)
		{
//			ANPC* showNPC = eventCloneNPC(quest->GetStartNpc());
//			SetShowActor(showNPC);
		}
		else
		{
			if (ShowLevel)
			{
				ShowLevel->SetEnabled( FALSE );
			}
		}
	} 
	else if (MMOQuestData::getSingleton().IsQuestCompleted(questId))
	{
		UNPCTemplate* autoFinishNpc = ((UQuest*)quest)->GetAutoFinishDialogHeadIcon();
		UNPCTemplate* finishNpc = ((UQuest*)quest)->GetFinishNpc();
		if (autoFinishNpc)
		{
//			ANPC* showNPC = eventCloneNPC(quest->GetAutoFinishDialogHeadIcon());
//			SetShowActor(showNPC);
		} 
		else if (finishNpc)
		{
//			ANPC* showNPC = eventCloneNPC(quest->GetFinishNpc());
//			SetShowActor(showNPC);
		}
		else
		{
			if (ShowLevel)
			{
				ShowLevel->SetEnabled( FALSE );
			}
		}
	}

	eventCallUCUpdateDiagData(object);
	eventCallUCPlay(MMOQuestManager::getSingleton().GetPageTotalTime()*1000);
	Show(TRUE);
}

UGFxObject* UMMOUIDialogNew::CreateDialogContext(INT i,INT questID,UBOOL share, INT shareSrcId,BYTE type)
{
	UGFxObject* diagData = GFXPlayer->CreateObject(TEXT("ActionScript3.NPCDialog.NPCDialogData"));
	//check(diagData != NULL);
	if(diagData == NULL) return NULL;

	diagData->SetString(TEXT("diag_text"), TEXT(""));
	diagData->SetString(TEXT("diag_npc_name"), DialogNpc ? DialogNpc->DisplayName : FString(TEXT("")));
	// diagData->SetInt(TEXT("diag_type"), (INT)EDiagDataType_Quest);
	diagData->SetBool(TEXT("quest_share"), share);
	diagData->SetInt(TEXT("quest_share_id"), shareSrcId);

	UGFxObject* questDetailData = MMOQuestManager::getSingleton().GetQuestDetailData(questID, GFXPlayer, TRUE);
	if(questDetailData == NULL) return NULL;
	
	switch (type)
	{
	case EDialogContextType_Dialog:
		{
			diagData->SetInt(TEXT("diag_type"), (INT)EDiagDataType_Dialog);
			diagData->SetString(TEXT("diag_text"), MMOQuestManager::getSingleton().GetContextContent(i));
		}
		break;
	case EDialogContextType_Quest_Page:
		{
			questDetailData->SetString(TEXT("quest_desc"),MMOQuestManager::getSingleton().GetContextContent(i));
			diagData->SetInt(TEXT("diag_type"), (INT)EDiagDataType_Quest);
		}
		break;
	case EDialogContextType_Quest:
		{
			diagData->SetInt(TEXT("diag_type"), (INT)EDiagDataType_Quest);
		}
		break;
	}
	diagData->SetObject(TEXT("quest_detail_data"), questDetailData);
	return diagData;
}

 void UMMOUIDialogNew::AcceptAuest(INT questID, UBOOL share, INT shareSrcId)
 {
	 const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questID);
	 if(!quest)
	 {
		 return;
	 }

	 if(quest->IsTimeLimit() == TRUE)
	 {
		 INT timeLimit = quest->DurationLimit;
		 INT limitH = timeLimit / (60 * 60);
		 timeLimit -= limitH * (60 * 60);

		 INT limitM = timeLimit / 60;
		 timeLimit -= limitM * 60;

		 FString timeStr = FString::Printf(TEXT("%02d:%02d:%02d"), limitH, limitM, timeLimit);

		 FScriptDelegate okFun(this, TEXT("OnAcceptTimeLimitQuest"));

		 FScriptDelegate cancelFun(this, TEXT("OnRejectTimeLimitQuest"));

		 FString sMsg = FString::Printf(
			 *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Quest.AcceptTimeLimitMsg" )),
			  *timeStr);
		 MMOMsgBoxManager::getSingleton().AddMessageBox( sMsg, questID , okFun ,cancelFun );
	 }
	 else
	 {
		 // TODO: refactor when there is time
		 MMOQuestManager::getSingleton().AcceptQuest(questID, share, shareSrcId);

		 if( ShowLevel )
		 {
			 ShowLevel->SetEnabled( FALSE );
		 }
		 Show(FALSE);
		 //TryPopupQuest(questID); // retry after EQET_AcceptQuest

		 //Clear context
		 MMOQuestManager::getSingleton().ResetContent();
	 }

	 // Tutorial event
	 MMOGameEvent_Guide evtGuide(EGUET_DialogAcceptQuest);
	 MMOUI::PostGameEvent(&evtGuide);
 }


 UBOOL UMMOUIDialogNew::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
 { 
	 if(bDefault)return FALSE;
	 return FALSE; 
 } 

 UBOOL UMMOUIDialogNew::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
 {
	 if(bDefault)return FALSE;
	 return FALSE;
 }
 //------------------------------------------------------------------------
 UBOOL UMMOUIDialogNew::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
 {
	 if(bDefault)return FALSE;
	 return FALSE;
 }
 //------------------------------------------------------------------------

 UBOOL UMMOUIDialogNew::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
 {
	 return FALSE;
 }

 void UMMOUIDialogNew::OnAcceptTimeLimitQuest(class UGFxObject* mc)
 {
	 if(!mc)
	 {
		 return;
	 }

	 INT questID = mc->GetInt(TEXT("iteminfoid"));
	 MMOQuestManager::getSingleton().AcceptQuest(questID, FALSE, 0);

	 TryPopupQuest(questID);
 }

 void UMMOUIDialogNew::OnRejectTimeLimitQuest(class UGFxObject* mc)
 {

 }

 //============================================================================================================
 void UMMOUIDialogNew::ConfirmTakeBackCurLeveUpWeaponRtNow()
 {
	 FScriptDelegate funYes(EC_EventParm);
	 funYes.Object = this;
	 funYes.FunctionName = TEXT( "TakeBackCurLeveUpWeaponRtNow" );

	 FScriptDelegate funNo(EC_EventParm);
	 funNo.Object = this;
	 funNo.FunctionName = TEXT( "CancelTakeBackCurLeveUpWeaponRtNow" );

	 //FString strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.ConfirmLevelUpTitle") );
	 FString strContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.ConfirmTakeBackCurLeveUpWeaponRtNow") );

	 CloseAllPopUpPanel();
	 ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( strContent, 0, funYes, funNo );
	 //ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBoxWithCheck( strTitle, 0, strContent, funYes, funNo, TRUE );
	 //if( ConfirmBox != NULL ){
		// ConfirmBox->SetBool( TEXT("forbidHint"), FALSE );
	 //}
 }
 //---------------------------------------------------------------------------------------------------------------
 void UMMOUIDialogNew::TakeBackCurLeveUpWeaponRtNow( UGFxObject *pMc )
 {
	 if( ConfirmBox == NULL ){
		 return;
	 }
	 ConfirmBox = NULL;

	 //INT64 curMny = MMOFacade::getSingleton().GetPlayerMoney64( E_MONEYTYPE_02 );
	 //if( curMny < 100000 ){
	 //	FString errorName = TEXT("MMOGame.EquipLevelUp.NoMoneyTakeBack");
	 //	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(errorName), MsgInfoType_Table , errorName);
	 //	MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath(errorName));
	 //	return;
	 //}

	 if( MMOFacade::getSingleton().IsBagFull() ){
		 FString errorName =TEXT("MMOGame.EquipLevelUp.BackIsFull");
		 MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(errorName), MsgInfoType_Table , errorName);
		 MMOChatManager::getSingleton().AddSystemMessage(MMOUI::ParseLocalizedPropertyPath(errorName));
		 return;
	 }
	 //
	 CMessageHandler::GetInstance().Send_EquipLevelUpRightNow_TakeOut();

	 //
	 CloseDialog();
 }
 //---------------------------------------------------------------------------------------------------------------
 void UMMOUIDialogNew::CancelTakeBackCurLeveUpWeaponRtNow( UGFxObject *pMc )
 {
	 ConfirmBox = NULL;
 }
 //============================================================================================================
 void UMMOUIDialogNew::ConfirmLeveUpWeaponCurrentEquipped()
 {
	 FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
	 if( pItemInfo == NULL ){
		 FString erronStrName = TEXT("MMOGame.EquipLevelUp.NoLevelUpEquip");
		 MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( erronStrName ), MsgInfoType_Table, erronStrName );
		 return;
	 }

	 //
	 ClientActionCondition_EquipLevelUp checker;

	 FClientActionConditionContex contex;
	 contex.Item = pItemInfo;

	 if( !checker.CheckClientActionCondition(contex) ){
		 return;
	 }

	 INT64 money = MMOFacade::getSingleton().GetPlayerMoney64( E_MONEYTYPE_02 );
	 if (money < WEAPON_LEVEL_UP_COST){
		 FString erronStrName = TEXT("MMOGame.EnhanceGeneral.EnhcError4");
		 MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(erronStrName), MsgInfoType_Table, erronStrName);
		 return;
	 }

	 //
	 UBOOL isShowConfirmBox = FALSE;
	 GConfig->GetBool( TEXT("PopUpMsgBox"), TEXT("ConfirmEquipLevelUpCurrentWeapon") , isShowConfirmBox , GDefaultGameIni );
	 if( isShowConfirmBox ){
		 FScriptDelegate funYes(EC_EventParm);
		 funYes.Object = this;
		 funYes.FunctionName = TEXT( "ConfirmLevelUpCurWeaponAndGo" );

		 FScriptDelegate funNo(EC_EventParm);
		 funNo.Object = this;
		 funNo.FunctionName = TEXT( "CancelLevelUpCurWeapon" );

		 FString strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.ConfirmLevelUpTitle") );
		 FString strContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.ConfirmLevelUpCurWeaponContent") );

		 CloseAllPopUpPanel();
		 ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBoxWithCheck( strTitle, FALSE, strContent, funYes, funNo, TRUE );
		 if( ConfirmBox != NULL ){
			 ConfirmBox->SetBool( TEXT("forbidHint"), FALSE );
		 }
	 }
	 else{
		 if( MMOEquipManager::getSingleton().IsMaxUpgradeLv( pItemInfo ) ){
			 ConfirmLeveUpWeaponHighLevel( TRUE );
			 return;
		 }
		 CMessageHandler::GetInstance().Send_EquipLevelUpShortcut_PutIn( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );

		 // Tutorial
		 MMOGameEvent_Guide evtGuide(EGUET_ShowWeaponLvUpPanel);
		 MMOUI::PostGameEvent(&evtGuide);
	 }
 }
 //---------------------------------------------------------------------------------------------------------------
 void UMMOUIDialogNew::ConfirmLevelUpCurWeaponAndGo( UGFxObject *pMc )
 {
	 if( ConfirmBox == NULL || pMc == NULL ){
		 return;
	 }

	 INT isCheckConfirmBox = pMc->GetInt(TEXT("iteminfoid"));
	 if( !isCheckConfirmBox ){
		 FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
		 if( pItemInfo == NULL ){
			 FString erronStrName = TEXT("MMOGame.EquipLevelUp.NoLevelUpEquip");
			 MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( erronStrName ), MsgInfoType_Table, erronStrName );
			 return;
		 }
		 if( MMOEquipManager::getSingleton().IsMaxUpgradeLv( pItemInfo ) ){
			 ConfirmLeveUpWeaponHighLevel( TRUE );
			 return;
		 }

		 UBOOL isNotShowAgain = ConfirmBox->GetBool( TEXT("forbidHint") );
		 GConfig->SetBool( TEXT("PopUpMsgBox"), TEXT("ConfirmEquipLevelUpCurrentWeapon"), !isNotShowAgain, GDefaultGameIni );
	 }

	 ConfirmBox = NULL;
	 //
	 CMessageHandler::GetInstance().Send_EquipLevelUpShortcut_PutIn( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );

	 //
	 CloseDialog();
 }

 //---------------------------------------------------------------------------------------------------------------
 void UMMOUIDialogNew::CancelLevelUpCurWeapon( UGFxObject *pMc )
 {
	 ConfirmBox = NULL;
 }
 //============================================================================================================
 void UMMOUIDialogNew::ConfirmLeveUpWeaponHighLevel( UBOOL isMessageBoxCall /*= FALSE*/ )
 {
	 FClientItemInfo *pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND );
	 if( pItemInfo == NULL ){
		 FString erronStrName = TEXT("MMOGame.EquipLevelUp.NoLevelUpEquip");
		 MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( erronStrName ), MsgInfoType_Table, erronStrName );
		 return;
	 }
	 
	 FScriptDelegate funYes(EC_EventParm);
	 funYes.Object = this;
	 funYes.FunctionName = TEXT( "ConfirmLevelUpCurWeaponAndGo" );
	 
	 FScriptDelegate funNo(EC_EventParm);
	 funNo.Object = this;
	 funNo.FunctionName = TEXT( "CancelLevelUpCurWeapon" );
	 
	 FString strTitle = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.ConfirmLevelUpTitle") );
	 FString strContent = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.EquipLevelUp.ConfirmLevelUpHighLevel") );
	 //
	 if( isMessageBoxCall == FALSE ){
		 CloseAllPopUpPanel();
	 }
	 
	 ConfirmBox = MMOMsgBoxManager::getSingleton().AddMessageBox( strContent, TRUE, funYes, funNo );
 }
 //============================================================================================================
 //////////////////////////////////////////////////////////////////////////

 void UMMOUIDialogNew::ASSelectDiagOption(INT optionID)
 {
	 SelectDiagOption(optionID);
 }

 void UMMOUIDialogNew::ASNPCDialogPage(INT pageIndex)
 {
	 CurrentDialogPage = pageIndex;
	 MMOQuestManager::getSingleton().SetQuestDialogCurrentPage(pageIndex);
	 //reset time
	 MMOQuestManager::getSingleton().RestPageTime();

	 //animation needs to replay on page turn
	 eventCallUCPlay(MMOQuestManager::getSingleton().GetPageTotalTime()*1000);
 }

 void UMMOUIDialogNew::ASCloseDialog()
 {
	 CloseDialog(FALSE);
	 //need to clear here
	 MMOQuestManager::getSingleton().ResetContent();
 }

 void UMMOUIDialogNew::ASSelectAward(class UGFxObject* icon,INT idx,INT itemTemplateID)
 {
	 debugf(TEXT("----[ASSelectAward] %d %d"), idx, itemTemplateID);
	 SelectedAwardIndex = idx;

	 if (SelectedAwardIndex >= 0)
	 {
		 // Tutorial event
		 MMOGameEvent_Guide evtGuide(EGUET_SelectQuestAward);
		 MMOUI::PostGameEvent(&evtGuide);
	 }
 }

 void UMMOUIDialogNew::ASAcceptQuest(INT questID, UBOOL share, INT shareSrcId)
 {
	 AcceptAuest(questID, share, shareSrcId);
 }

 void UMMOUIDialogNew::AbortQuest(INT questID)
 {
	 UMMOUIManager* uiManager = MMOUI::GetUIManager();
	 if (uiManager)
	 {
		 UMMOUIDialogNew* dialog = (UMMOUIDialogNew*)MMOUI::FindUIPanelEx(uiManager->Panel_DialogNew);
		 if (dialog)
		 {
			 dialog->CloseDialog();
		 }
	 }
	 CMessageHandler::GetInstance().Send_AbandonQuest(questID);
 }

 void UMMOUIDialogNew::ASAbortQuest(INT questID)
 {
	 if(questID == INVALID_QUEST_ID)
	 {
		 return;
	 }

	 const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questID);
	 if (!quest)
	 {
		 return;
	 }

	 if(!MMOQuestData::getSingleton().IsQuestAccepted(questID))
	 {
		 return;
	 }

	 if (!quest->CanAbandon)
	 {
		 FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.CanNotAbandon"));
		 MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Quest.CanNotAbandon"));
		 return;
	 }

	 FString text =  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Quest.AborbQuestConfirm"));
	 text = FString::Printf(*text, *UITextParser::HTMLFormatQuestName(quest));

	 FScriptDelegate funcOK(EC_EventParm);
	 funcOK.FunctionName = TEXT("AbortQuestOK");
	 funcOK.Object = this;

	 FScriptDelegate funcCanel(EC_EventParm);
	 funcCanel.FunctionName = TEXT("AbortQuestCanel");
	 funcCanel.Object = this;

	 MMOMsgBoxManager::getSingleton().AddMessageBox(text, questID, funcOK, funcCanel);
 }

 void UMMOUIDialogNew::ASSubmitQuest(INT questID)
 {
	 TrySubmitQuest(questID);
 }

 void UMMOUIDialogNew::ActivationOk(class UGFxObject* mc)
 {
	 FString textInput = mc->GetObject( "textInput" )->GetString( TEXT("text") );
	 CMessageHandler::GetInstance().Send_Activation(textInput);

 }

  void UMMOUIDialogNew::ActivationCancel(class UGFxObject* mc)
  {

  }