#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"
#include "MMOJourneyData.h"

IMPLEMENT_CLASS(UMMOUIReputationPanel);

void UMMOUIReputationPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	//TArray<EGameEvent> evts;
	//evts.AddItem(GameEvent_MsgBoxArenaWaitEnter);
	//EventAdapter = new TGameEventAdapter<UMMOUIArenaQueue>(this, evts);

	MMOUI::GetUIManager()->SetUpdateReputationDataDelegate(FScriptDelegate(this, TEXT("UpdateReputationData")));
	MMOUI::GetUIManager()->SetRefreshReputationSelectedDelegate(FScriptDelegate(this, TEXT("RefreshReputationSelected")));

}

void UMMOUIReputationPanel::UnInit()
{
	UMMOUIBase::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);
	MMOUI::GetUIManager()->SetUpdateReputationDataDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetRefreshReputationSelectedDelegate(nullDelgate);

}

void UMMOUIReputationPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (NULL != evt)
	{
	}
}

UBOOL UMMOUIReputationPanel::SkipCaptureInputKey(const FString& key,BYTE eIE,const FString& strHoverControllerName,FVector2D ClickPosInMovieSpace)
{
	return Super::SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace );
}

void UMMOUIReputationPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		ShowPanelAdvancedData* pPanelAdvancedData = MMOFacade::getSingleton().GetShowPanelAdvancedData(FriendlyName.GetNameString());
		if(pPanelAdvancedData) 
		{
			if(pPanelAdvancedData->Prarm1>0)
			{
				SelectListID = pPanelAdvancedData->Prarm1;
			}
			MMOFacade::getSingleton().DelShowPanelAdvancedData(FriendlyName.GetNameString());
		}

		MMOUI::GetUIManager()->delegateUpdateReputationData();
		// Temporarily disabled for CQ version
		//MMOUI::GetUIManager()->delegateUpdateMenuEffect(2,EMenuType_Repute,TRUE,FALSE);
		
		//INT TraceForceID = MMOUI::GetUIManager()->GetTraceRelationForceID();

		if(SelectListID>=0)
		{
			eventCallSetSelectedItem(SelectListID);
			//eventCallRefreshReputationSelected();
			SelectListID=-1;

		}
		else
		{
			INT TraceForceID = MMOUI::GetUIManager()->GetTraceRelationForceID();
			eventCallSetSelectedItem(TraceForceID);
			//eventCallInitList(TraceListIndex);
			//eventCallRefreshReputationSelected();
		}




	}
}

void UMMOUIReputationPanel::ChangeReputationType(INT ID,INT ReputationType)
{
	if(ReputationType==0)return;
	for(int i=0;i<ReputationList.Num();i++)
	{
		if(ReputationList(i).ForceID == ID)
		{
			if(ReputationList(i).type != ReputationType)
			{
				ReputationList(i).type = ReputationType;
				//ResetReputationTypeConfig();
			}
			break;
		}
	}
}

INT UMMOUIReputationPanel::KeyToNumber(const FString&Key, INT index)
{
	FString strBuf = MMOUI::ParseLocalizedPropertyPath(Key + FString::Printf(TEXT("%d"),index)	);
	return appAtoi(*strBuf);
}

void UMMOUIReputationPanel::InitReputationData()
{
	FString strBuf ;
	FString strIndex ;
	FString strForceID ;
	INT		i=0,j=0;
	BOOL	bBreak=false;


	UGFxObject *mc = NULL;
	ItemSlot.Empty();
	for( INT i = 0 ; i < 10 ; i++ )
	{
		mc = GetObject( FString::Printf( TEXT("mc_slot%d") , i  ));
		if( mc )
		{
			mc->SetInt("SlotType", ESPType_Invalid );	
			mc->SetInt("slotType", ESPType_Invalid );	
			mc->SetInt("slotIndex",i);		
			mc->SetInt("itemid",i);
			ItemSlot.AddItem(mc);	
		}
	}




	//ReputationName.Empty();
	//ReputationValue.Empty();
	//ReputationValue.Push(KeyToNumber(FString("MMOGame.Reputation.ReputationValue") , 0) );
	//ReputationValue.Push(KeyToNumber(FString("MMOGame.Reputation.ReputationValue") , 1) );
	//ReputationName.Push(MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.Reputation.ReputationName0")));
	//ReputationName.Push(MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.Reputation.ReputationName1")));


	ReputationList.Empty();
	for(i=0;i<MAX_REPUTATION_COUNT;i++)
	{
		ReputationTemp*  pReputationTemp = MMOFacade::getSingleton().GetReputationTemp(i);
		if(pReputationTemp==NULL)
		{
			continue;
		}

		FReputationData2 data ;
		data.TempIndex		= i;
		if(pReputationTemp->ListLevel==0)
		{
			data.type			= 2;
		}
		else{
			data.type			= 0;
		}
		data.ForceName		= pReputationTemp->ForceName;
		data.ForceLevel		= pReputationTemp->ListLevel;
		data.ForceID		= pReputationTemp->ForceID;
		data.Visible		= pReputationTemp->ForceVisible;
		data.ForceTips		= pReputationTemp->ForceTips;
		data.ForceInfo		= pReputationTemp->ForceInfo;
		data.InitValue		= pReputationTemp->InitValue;
		data.MaxValue		= pReputationTemp->MaxValue;


		ReputationList.Push(data);
	}

	for(i=0;i<ReputationList.Num()-1;i++)
	{
		if(ReputationList(i).type==0 && ReputationList(i).ForceLevel<ReputationList(i+1).ForceLevel)
		{
			ReputationList(i).type = 2;
		}
	}
}

void UMMOUIReputationPanel::UpdateRewordSlot(UGFxObject * SlotObj)
{

	if(SlotObj)
	{
		INT ID[10];
		INT Count[10];
		for(INT i=0;i<10;i++)
		{
			ID[i]=0;
			Count[i]=0;
		}

		INT nClassId = MMOFacade::getSingleton().GetMainPlayer()->GetClassId();
		if(nClassId==EPCT_PlayerClassG) nClassId = 0;		// 1.05 Lancer reuses Warrior rewards
		else if(nClassId==EPCT_PlayerClassH) nClassId = 1;	// 1.05 Sorcerer reuses Mage rewards
		if(nClassId==0)
		{
			ID[0]		= SlotObj->GetInt("_WarriorAwardItemID1");
			Count[0]	= SlotObj->GetInt("_WarriorAwardItemCount1");
			ID[1]		= SlotObj->GetInt("_WarriorAwardItemID2");
			Count[1]	= SlotObj->GetInt("_WarriorAwardItemCount2");
			ID[2]		= SlotObj->GetInt("_WarriorAwardItemID3");
			Count[2]	= SlotObj->GetInt("_WarriorAwardItemCount3");
			ID[3]		= SlotObj->GetInt("_WarriorAwardItemID4");
			Count[3]	= SlotObj->GetInt("_WarriorAwardItemCount4");

		}
		else if(nClassId==1)
		{
			ID[0]		= SlotObj->GetInt("_MageAwardItemID1");
			Count[0]	= SlotObj->GetInt("_MageAwardItemCount1");
			ID[1]		= SlotObj->GetInt("_MageAwardItemID2");
			Count[1]	= SlotObj->GetInt("_MageAwardItemCount2");
			ID[2]		= SlotObj->GetInt("_MageAwardItemID3");
			Count[2]	= SlotObj->GetInt("_MageAwardItemCount3");
			ID[3]		= SlotObj->GetInt("_MageAwardItemID4");
			Count[3]	= SlotObj->GetInt("_MageAwardItemCount4");
		}
		else if(nClassId==2)
		{
			ID[0]		= SlotObj->GetInt("_RogueAwardItemID1");
			Count[0]	= SlotObj->GetInt("_RogueAwardItemCount1");
			ID[1]		= SlotObj->GetInt("_RogueAwardItemID2");
			Count[1]	= SlotObj->GetInt("_RogueAwardItemCount2");
			ID[2]		= SlotObj->GetInt("_RogueAwardItemID3");
			Count[2]	= SlotObj->GetInt("_RogueAwardItemCount3");
			ID[3]		= SlotObj->GetInt("_RogueAwardItemID4");
			Count[3]	= SlotObj->GetInt("_RogueAwardItemCount4");
		}
		else if(nClassId==3)
		{
			ID[0]		= SlotObj->GetInt("_ArcherAwardItemID1");
			Count[0]	= SlotObj->GetInt("_ArcherAwardItemCount1");
			ID[1]		= SlotObj->GetInt("_ArcherAwardItemID2");
			Count[1]	= SlotObj->GetInt("_ArcherAwardItemCount2");
			ID[2]		= SlotObj->GetInt("_ArcherAwardItemID3");
			Count[2]	= SlotObj->GetInt("_ArcherAwardItemCount3");
			ID[3]		= SlotObj->GetInt("_ArcherAwardItemID4");
			Count[3]	= SlotObj->GetInt("_ArcherAwardItemCount4");
		}


		for(INT i=0;i<10;i++)
		{
			if(ID[i]>0||Count[i]>0){
				UItem* item	= MMOFacade::getSingleton().FindItemTemplate( ID[i] );
				SetItemBarData(ItemSlot(i),item,i,Count[i]);
			}
			else
			{
				SetItemBarData(ItemSlot(i),NULL,i,0);
			}
		}


		//FString Reword = SlotObj->GetString("reword");
		//TArray<FString> RewardString = MMO::SplitString( *Reword, TEXT(";"), 0, FALSE );

		//num = RewardString.Num();
		//for(INT j=0;j<num;j++)
		//{
		//	UItem* item	= MMOFacade::getSingleton().FindItemTemplate( appAtoi((TCHAR*)*RewardString(j)) );
		//	SetItemBarData(ItemSlot(j),item,j);
		//}

	}
	
	//for(INT i=num;i<10;i++)
	//{
	//	SetItemBarData(ItemSlot(i),NULL,i);
	//}
}

void UMMOUIReputationPanel::UpdateReputationData()
{
	if( !GFXPlayer )
		return;

	UGFxObject *DataProvider = GFXPlayer->CreateArray();
	INT ForceID = MMOUI::GetUIManager()->GetTraceRelationForceID();

	for(int i = 0; i < ReputationList.Num(); i++)
	{
		UGFxObject * ItemObj = GFXPlayer->CreateObject("Object");
		ItemObj->SetString("label",ReputationList(i).ForceName);
		ItemObj->SetInt("index",i);
		ItemObj->SetInt("type",ReputationList(i).type);
		ItemObj->SetInt("deepth",ReputationList(i).ForceLevel);

		ItemObj->SetString("tips",ReputationList(i).ForceTips);

		ItemObj->SetString("info",ReputationList(i).ForceInfo);//+pReputationTemp->ForceAward[nClassId]);

		ReputationTemp*  pReputationTemp = MMOFacade::getSingleton().GetReputationTemp(ReputationList(i).TempIndex);
		if(pReputationTemp){
			INT nClassId = MMOFacade::getSingleton().GetMainPlayer()->GetClassId();
			//ItemObj->SetString("reword",pReputationTemp->ForceAward[nClassId]);

			ItemObj->SetInt("_WarriorAwardItemID1",		pReputationTemp->_WarriorAwardItemID[0]);
			ItemObj->SetInt("_WarriorAwardItemCount1",	pReputationTemp->_WarriorAwardItemCount[0]);
			ItemObj->SetInt("_WarriorAwardItemID2",		pReputationTemp->_WarriorAwardItemID[1]);
			ItemObj->SetInt("_WarriorAwardItemCount2",	pReputationTemp->_WarriorAwardItemCount[1]);
			ItemObj->SetInt("_WarriorAwardItemID3",		pReputationTemp->_WarriorAwardItemID[2]);
			ItemObj->SetInt("_WarriorAwardItemCount3",	pReputationTemp->_WarriorAwardItemCount[2]);
			ItemObj->SetInt("_WarriorAwardItemID4",		pReputationTemp->_WarriorAwardItemID[3]);
			ItemObj->SetInt("_WarriorAwardItemCount4",	pReputationTemp->_WarriorAwardItemCount[3]);

			ItemObj->SetInt("_MageAwardItemID1",		pReputationTemp->_MageAwardItemID[0]);
			ItemObj->SetInt("_MageAwardItemCount1",		pReputationTemp->_MageAwardItemCount[0]);
			ItemObj->SetInt("_MageAwardItemID2",		pReputationTemp->_MageAwardItemID[1]);
			ItemObj->SetInt("_MageAwardItemCount2",		pReputationTemp->_MageAwardItemCount[1]);
			ItemObj->SetInt("_MageAwardItemID3",		pReputationTemp->_MageAwardItemID[2]);
			ItemObj->SetInt("_MageAwardItemCount3",		pReputationTemp->_MageAwardItemCount[2]);
			ItemObj->SetInt("_MageAwardItemID4",		pReputationTemp->_MageAwardItemID[3]);
			ItemObj->SetInt("_MageAwardItemCount4",		pReputationTemp->_MageAwardItemCount[3]);

			ItemObj->SetInt("_RogueAwardItemID1",		pReputationTemp->_RogueAwardItemID[0]);
			ItemObj->SetInt("_RogueAwardItemCount1",	pReputationTemp->_RogueAwardItemCount[0]);
			ItemObj->SetInt("_RogueAwardItemID2",		pReputationTemp->_RogueAwardItemID[1]);
			ItemObj->SetInt("_RogueAwardItemCount2",	pReputationTemp->_RogueAwardItemCount[1]);
			ItemObj->SetInt("_RogueAwardItemID3",		pReputationTemp->_RogueAwardItemID[2]);
			ItemObj->SetInt("_RogueAwardItemCount3",	pReputationTemp->_RogueAwardItemCount[2]);
			ItemObj->SetInt("_RogueAwardItemID4",		pReputationTemp->_RogueAwardItemID[3]);
			ItemObj->SetInt("_RogueAwardItemCount4",	pReputationTemp->_RogueAwardItemCount[3]);

			ItemObj->SetInt("_ArcherAwardItemID1",		pReputationTemp->_ArcherAwardItemID[0]);
			ItemObj->SetInt("_ArcherAwardItemCount1",	pReputationTemp->_ArcherAwardItemCount[0]);
			ItemObj->SetInt("_ArcherAwardItemID2",		pReputationTemp->_ArcherAwardItemID[1]);
			ItemObj->SetInt("_ArcherAwardItemCount2",	pReputationTemp->_ArcherAwardItemCount[1]);
			ItemObj->SetInt("_ArcherAwardItemID3",		pReputationTemp->_ArcherAwardItemID[2]);
			ItemObj->SetInt("_ArcherAwardItemCount3",	pReputationTemp->_ArcherAwardItemCount[2]);
			ItemObj->SetInt("_ArcherAwardItemID4",		pReputationTemp->_ArcherAwardItemID[3]);
			ItemObj->SetInt("_ArcherAwardItemCount4",	pReputationTemp->_ArcherAwardItemCount[3]);

			ItemObj->SetInt("_TaoistAwardItemID1",		pReputationTemp->_TaoistAwardItemID[0]);
			ItemObj->SetInt("_TaoistAwardItemCount1",	pReputationTemp->_TaoistAwardItemCount[0]);
			ItemObj->SetInt("_TaoistAwardItemID2",		pReputationTemp->_TaoistAwardItemID[1]);
			ItemObj->SetInt("_TaoistAwardItemCount2",	pReputationTemp->_TaoistAwardItemCount[1]);
			ItemObj->SetInt("_TaoistAwardItemID3",		pReputationTemp->_TaoistAwardItemID[2]);
			ItemObj->SetInt("_TaoistAwardItemCount3",	pReputationTemp->_TaoistAwardItemCount[2]);
			ItemObj->SetInt("_TaoistAwardItemID4",		pReputationTemp->_TaoistAwardItemID[3]);
			ItemObj->SetInt("_TaoistAwardItemCount4",	pReputationTemp->_TaoistAwardItemCount[3]);
		}
		else
		{
			ItemObj->SetString("reword","");
		}

		ItemObj->SetBool("canReceive",FALSE);


		if(ReputationList(i).ForceLevel > 0)
		{
			INT		reputation	= MMOFacade::getSingleton().GetPlayerReputation(MMOFacade::getSingleton().GetMainPlayer(), ReputationList(i).ForceID);
			BOOL	activeState = MMOFacade::getSingleton().GetPlayerActiveState(MMOFacade::getSingleton().GetMainPlayer(), ReputationList(i).ForceID);
			BOOL    awardState  = MMOFacade::getSingleton().GetPlayerAwardState(MMOFacade::getSingleton().GetMainPlayer(), ReputationList(i).ForceID);

			//if configured as invisible, do not display
			activeState = activeState && ReputationList(i).Visible;

			if(awardState==false && pReputationTemp && reputation>=pReputationTemp->MaxValue ){
				awardState  = false;
			}
			else
			{
				awardState = true;
			}

			ItemObj->SetInt("trackForceID",ForceID);
			ItemObj->SetInt("forceID",ReputationList(i).ForceID);
			ItemObj->SetInt("colorReputation",1);
			ItemObj->SetInt("curReputation",reputation);
			ItemObj->SetBool("visible",activeState);
			ItemObj->SetBool("awardState",awardState);
			if(pReputationTemp){
				ItemObj->SetInt("minReputation",pReputationTemp->InitValue);
				ItemObj->SetInt("maxReputation",pReputationTemp->MaxValue);
			}
		}
		else
		{
			ItemObj->SetBool("visible",ReputationList(i).Visible);
		}

		DataProvider->SetElementObject(i,ItemObj);
	}

	eventCallUpdateItemsData(DataProvider);	
}



void UMMOUIReputationPanel::SetItemBarData( UGFxObject* mc , UItem* item , INT slotId , INT num )
{
	if(mc==NULL)return;

	UGFxObject *pSlotData = mc->GetObject( TEXT("SlotData") );
	if(pSlotData && pSlotData->GetString(TEXT("SlotRenderClassName")) != TEXT("NormalItemSlotRender") )
	{
		pSlotData = NULL;
	}

	if( pSlotData == NULL )
	{
		pSlotData = GFXPlayer->CreateObject(TEXT("ActionScript3.Slot.ItemSlotData"));
		if(pSlotData){
			pSlotData->SetString(TEXT("SlotRenderClassName"),  TEXT("NormalItemSlotRender"));
		}
	}

	if( pSlotData )
	{
		if(item){
			pSlotData->SetInt( TEXT("slotType"), ESPType_Invalid );
			pSlotData->SetInt( TEXT("slotIndex"), slotId );

			pSlotData->SetBool( TEXT("HaveData"), TRUE );
			pSlotData->SetInt( TEXT("EquipID"), ESBarSlotTp_Item );

			//INT count = MMOFacade::getSingleton().GetItemCountInBag( item->GetTemplateID() );
			pSlotData->SetInt( TEXT("quantity"), num );
			FString cs = FString(TEXT("img://"))+MMOFacade::getSingleton().GetItemIconNamePersonalized( item );
			pSlotData->SetString( TEXT("iconName"), cs );
			pSlotData->SetInt( TEXT("itemTemplateID"), item->GetTemplateID() );
			pSlotData->SetInt( TEXT("itemColor"), item->ItemColor );


		}
		else
		{
			pSlotData->SetInt( TEXT("slotType"), ESPType_Invalid );
			pSlotData->SetInt( TEXT("slotIndex"), slotId );


			pSlotData->SetBool( TEXT("HaveData"), FALSE );
			pSlotData->SetInt( TEXT("EquipID"), ESBarSlotTp_Invalid );
			pSlotData->SetInt( TEXT("quantity"), 0 );
			pSlotData->SetString( TEXT("iconName"), TEXT("") );
			pSlotData->SetInt( TEXT("itemTemplateID"), 0 );
			pSlotData->SetInt( TEXT("itemColor"), 0 );

		}
		mc->SetObject(TEXT("SlotData"),pSlotData);
		TArray<struct FASValue> asArgs;
		mc->Invoke(TEXT("UpdateData"), asArgs);
	}

}

void UMMOUIReputationPanel::AwardClick(INT ForceID)
{
	if (MMOFacade::getSingleton().GetBagEmptySlotCount() < MMOJourneyData::getSingleton().GetReputationAwardsCount(ForceID))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(TEXT("Not enough bag space to claim the reward"),
			MsgInfoType_Table,
			TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAchievementRewardBagFull"));
	}
	else
	{
		CMessageHandler::GetInstance().Send_ReceiveReputationAward(ForceID);
	}
}