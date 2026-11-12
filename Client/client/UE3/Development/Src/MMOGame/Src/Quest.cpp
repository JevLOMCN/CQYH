#include "stdafx.h"
#include "MMOFacade.h"
#include "MMOQuestManager.h"
#include "MMOItemDataManager.h"

IMPLEMENT_CLASS(UQuest)

void UQuest::GetDynamicListValues(const DynamicListInfo& ListInfo, TArray<FString>& Values)
{
	if (ListInfo.ListName == TEXT("CheckKillNpcNav"))
	{
		if (ListInfo.ArrayIndex >= 0 && ListInfo.ArrayIndex < CheckKillNpc.Num())
		{
			const FQuestCheckKillNpc* checkMonster = &CheckKillNpc(ListInfo.ArrayIndex);			
			UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
			const TArray<FNaviPointInfo>* mapNavPoints = NULL;
			if( globalMapInfo != NULL ){
				mapNavPoints = globalMapInfo->GetAllNavPointsInMap( checkMonster->CheckKillNpcMap );
			}
			if( mapNavPoints == NULL ){
				return;
			}
			const TArray<FNaviPointInfo> &mapNavPointsList = *mapNavPoints;

			for (INT i = 0; i < mapNavPointsList.Num(); ++ i)
			{
				const FNaviPointInfo &navInfo = mapNavPointsList(i);
				Values.AddItem(navInfo.Name);
			}

			if (Values.Num() == 0)
			{
				Values.AddItem("None");
			}
		}
	}
	else if (ListInfo.ListName == TEXT("CheckKillDoodadNav"))
	{
		if (ListInfo.ArrayIndex >= 0 && ListInfo.ArrayIndex < CheckKillDoodad.Num())
		{
			const FQuestCheckKillDoodad* checkKillDoodad = &CheckKillDoodad(ListInfo.ArrayIndex);			
			UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
			const TArray<FNaviPointInfo>* mapNavPoints = NULL;
			if( globalMapInfo != NULL ){
				mapNavPoints = globalMapInfo->GetAllNavPointsInMap( checkKillDoodad->CheckKillDoodadMap );
			}
			if( mapNavPoints == NULL ){
				return;
			}
			const TArray<FNaviPointInfo> &mapNavPointsList = *mapNavPoints;

			for (INT i = 0; i < mapNavPointsList.Num(); ++ i)
			{
				const FNaviPointInfo &navInfo = mapNavPointsList(i);
				Values.AddItem(navInfo.Name);
			}

			if (Values.Num() == 0)
			{
				Values.AddItem("None");
			}
		}
	}
	else if (ListInfo.ListName == TEXT("CheckItemNav"))
	{
		if (ListInfo.ArrayIndex >= 0 && ListInfo.ArrayIndex < CheckItem.Num())
		{
			FQuestCheckItem* checkItem = &CheckItem(ListInfo.ArrayIndex);			
			UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
			const TArray<FNaviPointInfo>* mapNavPoints = NULL;
			if( globalMapInfo != NULL ){
				mapNavPoints = globalMapInfo->GetAllNavPointsInMap( checkItem->CheckItemMap );
			}
			if( mapNavPoints == NULL ){
				return;
			}
			const TArray<FNaviPointInfo> &mapNavPointsList = *mapNavPoints;

			for (INT i = 0; i < mapNavPointsList.Num(); ++ i)
			{
				const FNaviPointInfo &navInfo = mapNavPointsList(i);
				Values.AddItem(navInfo.Name);
			}

			if (Values.Num() == 0)
			{
				Values.AddItem("None");
			}
		}
	}
	else if (ListInfo.ListName == TEXT("CheckVarType"))
	{		
		for (INT i = 0; i < vtTotal; ++ i)
		{
			switch (i)
			{
			case vtInvalid:
				Values.AddItem("vtInvalid");
				break;
			case vtRole:
				Values.AddItem("vtRole");
				break;
			case vtScene:
				Values.AddItem("vtScene");
				break;
			case vtGuild:
				Values.AddItem("vtGuild");
				break;
			case vtQuest:
				Values.AddItem("vtQuest");
				break;
			case vtKillNpcCount:
				Values.AddItem("vtKillNpcCount");
				break;
			case vtClient:
				Values.AddItem("vtClient");
				break;
			case vtAI:
				Values.AddItem("vtAI");
				break;
			case vtAISpell:
				Values.AddItem("vtAISpell");
				break;
			case vtItemRecycle:
				Values.AddItem("vtItemRecycle");
				break;
			}
		}

		if (Values.Num() == 0)
		{
			Values.AddItem("vtInvalid");
		}
	}
}

void UQuest::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.Property->GetName() == TEXT("CheckKillNpcMap"))
	{
		INT index = PropertyChangedEvent.GetArrayIndex(TEXT("CheckKillNpc"));
		if (index >= 0 && index < CheckKillNpc.Num())
		{
			FQuestCheckKillNpc* checkMonster = &CheckKillNpc(index);			
			if (checkMonster)
			{
				checkMonster->CheckKillNpcNav = "None";
			}
		}
	}
	else if (PropertyChangedEvent.Property->GetName() == TEXT("CheckKillDoodadMap"))
	{
		INT index = PropertyChangedEvent.GetArrayIndex(TEXT("CheckKillDoodad"));
		if (index >= 0 && index < CheckKillDoodad.Num())
		{
			FQuestCheckKillDoodad* checkKillDoodad = &CheckKillDoodad(index);			
			if (checkKillDoodad)
			{
				checkKillDoodad->CheckKillDoodadNav = "None";
			}
		}
	}
	else if (PropertyChangedEvent.Property->GetName() == TEXT("CheckItemMap"))
	{
		INT index = PropertyChangedEvent.GetArrayIndex(TEXT("CheckItem"));
		if (index >= 0 && index < CheckItem.Num())
		{
			FQuestCheckItem* checkItem = &CheckItem(index);			
			if (checkItem)
			{
				checkItem->CheckItemNav = "None";
			}
		}
	}
	else if( PropertyChangedEvent.Property->GetName() == "CheckVarType" )
	{
		INT index = PropertyChangedEvent.GetArrayIndex(TEXT("VarLimit"));
		if (index >= 0 && index < VarLimit.Num())
		{
			FQuestCheckVar* checkVar = &VarLimit(index);			
			if (checkVar)
			{
				checkVar->CheckVarID = -1;
				checkVar->CheckVarCnt = 0;
				checkVar->Description = TEXT("");
			}
		}

		index = PropertyChangedEvent.GetArrayIndex(TEXT("CheckVar"));
		if (index >= 0 && index < CheckVar.Num())
		{
			FQuestCheckVar* checkVar = &CheckVar(index);			
			if (checkVar)
			{
				checkVar->CheckVarID = -1;
				checkVar->CheckVarCnt = 0;
				checkVar->Description = TEXT("");
			}
		}
	}
}

UBOOL UQuest::IsNeedArchetype()
{
	return false;
}

void UQuest::DataToExcel(TArray<FString>& Row)
{
}

void UQuest::ExportExcelHeaders(TArray<FString>& Header,TArray<FString>& DefaultValues)
{
}

void UQuest::ImportFromExcel(const TArray<FString>& Values)
{	
	EMoneyTypeLimit = MoneyTypeLimit == -1 ? E_MONEYTYPE_COUNT : (EMONEY_TYPE)MoneyTypeLimit;
	EOfferCurrencyType = OfferCurrencyType == -1 ? E_MONEYTYPE_COUNT : (EMONEY_TYPE)OfferCurrencyType;
	EAwardCurrencyType = AwardCurrencyType == -1 ? E_MONEYTYPE_COUNT : (EMONEY_TYPE)AwardCurrencyType;
	EAwardCurrencyTypeEx = AwardCurrencyTypeEx == -1 ? E_MONEYTYPE_COUNT : (EMONEY_TYPE)AwardCurrencyTypeEx;
}

void UQuest::InitDataToExcel()
{
	if(ResetType == 0)
		nResetTime = ResetTime.Hour*3600 + ResetTime.Minute*60 + ResetTime.Second;
	else
		nResetTime = ResetTime.Day*24*3600 + ResetTime.Hour*3600 + ResetTime.Minute*60 + ResetTime.Second;

	struct tm tStart, tEnd;
	tStart.tm_year = AcceptTimeStart.Year-1900;
	tStart.tm_mon = AcceptTimeStart.Month;
	tStart.tm_mday = AcceptTimeStart.Day;
	tStart.tm_hour = AcceptTimeStart.Hour;
	tStart.tm_min = AcceptTimeStart.Minute;
	tStart.tm_sec = AcceptTimeStart.Second;
	tStart.tm_wday = 0;
	tStart.tm_yday = 0;
	tStart.tm_isdst = 0;
	
	tEnd.tm_year = AcceptTimeEnd.Year-1900;
	tEnd.tm_mon = AcceptTimeEnd.Month;
	tEnd.tm_mday = AcceptTimeEnd.Day;
	tEnd.tm_hour = AcceptTimeEnd.Hour;
	tEnd.tm_min = AcceptTimeEnd.Minute;
	tEnd.tm_sec = AcceptTimeEnd.Second;
	tEnd.tm_wday = 0;
	tEnd.tm_yday = 0;
	tEnd.tm_isdst = 0;

	time_t timeStart = mktime(&tStart);
	time_t timeEnd = mktime(&tEnd);
	
	nAcceptTimeStart = timeStart;
	nAcceptTimeEnd = timeEnd;
	
	// Get sub-quest destination
	for (INT i = 0; i < 8; ++ i)
	{
		Destinations(i).DestMapId = 0;
		Destinations(i).DestX = 0.0;
		Destinations(i).DestY = 0.0;
		Destinations(i).DestZ = 0.0;
	}

	int destIndex = 0;

	// start npc
	if (StartNPCNewId > 0 && StartNpcFly)
	{
		INT destMapId = 0;
		FVector destPos(0.0, 0.0, 0.0);
		UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
		for (INT i = 0; i < globalMapInfo->NPCInfoList.Num(); ++ i)
		{
			FNPCInstanceInfo* npcInfo = &globalMapInfo->NPCInfoList(i);
			if (npcInfo && StartNPCNewId == npcInfo->TemplateID && StartNPCMap == npcInfo->MapID)
			{
				destMapId = npcInfo->MapID;
				destPos = npcInfo->Position;
				break;
			}
		}

		if (destIndex < 8)
		{ 
			Destinations(destIndex).DestMapId = destMapId;
			Destinations(destIndex).DestX = destPos.X;
			Destinations(destIndex).DestY = destPos.Y;
			Destinations(destIndex).DestZ = destPos.Z;
			destIndex ++;
		}
	}

	// talk to npc
	if (TalkNpcID > 0 && TalkNpcFly)
	{
		INT destMapId = 0;
		FVector destPos(0.0, 0.0, 0.0);
		UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
		for (INT i = 0; i < globalMapInfo->NPCInfoList.Num(); ++ i)
		{
			FNPCInstanceInfo* npcInfo = &globalMapInfo->NPCInfoList(i);
			if (npcInfo && TalkNpcID == npcInfo->TemplateID && TalkNpcMap == npcInfo->MapID)
			{
				destMapId = npcInfo->MapID;
				destPos = npcInfo->Position;
				break;
			}
		}

		if (destIndex < 8)
		{ 
			Destinations(destIndex).DestMapId = destMapId;
			Destinations(destIndex).DestX = destPos.X;
			Destinations(destIndex).DestY = destPos.Y;
			Destinations(destIndex).DestZ = destPos.Z;
			destIndex ++;
		}
	}

	// check item
	for (INT i = 0; i < CheckItem.Num(); ++ i)
	{
		const FQuestCheckItem* checkItem = &CheckItem(i);			
		if (checkItem && checkItem->CheckItemFly)
		{
			INT destMapId = 0;
			FVector destPos(0.0, 0.0, 0.0);
			UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
			const TArray<FNaviPointInfo>* mapNavPoints = NULL;
			if( globalMapInfo != NULL ){
				mapNavPoints = globalMapInfo->GetAllNavPointsInMap( checkItem->CheckItemMap );
			}
			if( mapNavPoints == NULL ){
				return;
			}
			const TArray<FNaviPointInfo> &mapNavPointsList = *mapNavPoints;

			for (INT j = 0; j < mapNavPointsList.Num(); ++ j)
			{
				const FNaviPointInfo &navInfo = mapNavPointsList(i);
				if (checkItem->CheckItemNav == navInfo.Name)
				{
					destMapId = checkItem->CheckItemMap;
					destPos = navInfo.Position;
					break;
				}
			}
			
			if (destIndex < 8)
			{
				Destinations(destIndex).DestMapId = destMapId;
				Destinations(destIndex).DestX = destPos.X;
				Destinations(destIndex).DestY = destPos.Y;
				Destinations(destIndex).DestZ = destPos.Z;
				destIndex ++;
			}
		}
	}

	// check kill npc
	for (INT i = 0; i < CheckKillNpc.Num(); ++ i)
	{
		const FQuestCheckKillNpc* checkMonster = &CheckKillNpc(i);			
		if (checkMonster && checkMonster->CheckKillNpcFly)
		{
			INT destMapId = 0;
			FVector destPos(0.0, 0.0, 0.0);
			UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
			const TArray<FNaviPointInfo>* mapNavPoints = NULL;
			if( globalMapInfo != NULL ){
				mapNavPoints = globalMapInfo->GetAllNavPointsInMap( checkMonster->CheckKillNpcMap );
			}
			if( mapNavPoints == NULL ){
				return;
			}
			const TArray<FNaviPointInfo> &mapNavPointsList = *mapNavPoints;

			for (INT j = 0; j < mapNavPointsList.Num(); ++ j)
			{
				const FNaviPointInfo &navInfo = mapNavPointsList(j);
				if (checkMonster->CheckKillNpcNav == navInfo.Name)
				{
					destMapId = checkMonster->CheckKillNpcMap;
					destPos = navInfo.Position;
					break;
				}
			}

			if (destIndex < 8)
			{ 
				Destinations(destIndex).DestMapId = destMapId;
				Destinations(destIndex).DestX = destPos.X;
				Destinations(destIndex).DestY = destPos.Y;
				Destinations(destIndex).DestZ = destPos.Z;
				destIndex ++;
			}
		}
	}

	// check doodad
	for (INT i = 0; i < CheckKillDoodad.Num(); ++ i)
	{
		const FQuestCheckKillDoodad* checkKillDoodad = &CheckKillDoodad(i);			
		if (checkKillDoodad && checkKillDoodad->CheckKillDoodadFly)
		{
			INT destMapId = 0;
			FVector destPos(0.0, 0.0, 0.0);
			UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
			const TArray<FNaviPointInfo>* mapNavPoints = NULL;
			if( globalMapInfo != NULL ){
				mapNavPoints = globalMapInfo->GetAllNavPointsInMap( checkKillDoodad->CheckKillDoodadMap );
			}
			if( mapNavPoints == NULL ){
				return;
			}
			const TArray<FNaviPointInfo> &mapNavPointsList = *mapNavPoints;

			for (INT j = 0; j < globalMapInfo->NaviPointInfoList.Num(); ++ j)
			{
				const FNaviPointInfo &navInfo = mapNavPointsList(i);
				if (checkKillDoodad->CheckKillDoodadNav == navInfo.Name)
				{
					destMapId = checkKillDoodad->CheckKillDoodadMap;
					destPos = navInfo.Position;
					break;
				}
			}

			if (destIndex < 8)
			{ 
				Destinations(destIndex).DestMapId = destMapId;
				Destinations(destIndex).DestX = destPos.X;
				Destinations(destIndex).DestY = destPos.Y;
				Destinations(destIndex).DestZ = destPos.Z;
				destIndex ++;
			}
		}
	}

	// Update the old data
	for (INT i = 0; i < VarLimit.Num(); ++ i)
	{
		FQuestCheckVar* checkVar = &VarLimit(i);			
		if (checkVar)
		{
			if (checkVar->CheckVarType == TEXT("0") || checkVar->CheckVarType == TEXT("None") || checkVar->CheckVarType == TEXT(""))
			{
				checkVar->CheckVarType = TEXT("vtInvalid");
			}
		}
	}

	for (INT i = 0; i < CheckVar.Num(); ++ i)
	{
		FQuestCheckVar* checkVar = &CheckVar(i);			
		if (checkVar)
		{
			if (checkVar->CheckVarType == TEXT("0") || checkVar->CheckVarType == TEXT("None") || checkVar->CheckVarType == TEXT(""))
			{
				checkVar->CheckVarType = TEXT("vtInvalid");
			}
		}
	}

	// 

	MoneyTypeLimit = EMoneyTypeLimit == E_MONEYTYPE_COUNT ? -1 : (INT)EMoneyTypeLimit;
	OfferCurrencyType = EOfferCurrencyType == E_MONEYTYPE_COUNT ? -1 : (INT)EOfferCurrencyType;
	AwardCurrencyType = EAwardCurrencyType == E_MONEYTYPE_COUNT ? -1 : (INT)EAwardCurrencyType;
	AwardCurrencyTypeEx = EAwardCurrencyTypeEx == E_MONEYTYPE_COUNT ? -1 : (INT)EAwardCurrencyTypeEx;

	for (INT i = 0; i < CheckKillPlayer.Num(); ++ i)
	{
		FQuestCheckKillPlayer* checkKillPlayer = &CheckKillPlayer(i);			
		if (checkKillPlayer)
		{
			switch (checkKillPlayer->eCheckKillPlayerLevelStart)
			{
			case ECKPLS_30:
				checkKillPlayer->CheckKillPlayerLevelStart = 30;
				break;
			case ECKPLS_50:
				checkKillPlayer->CheckKillPlayerLevelStart = 50;
				break;
			case ECKPLS_70:
				checkKillPlayer->CheckKillPlayerLevelStart = 70;
				break;
			}

			switch (checkKillPlayer->eCheckKillPlayerLevelEnd)
			{
			case ECKPLE_50:
				checkKillPlayer->CheckKillPlayerLevelEnd = 50;
				break;
			case ECKPLE_70:
				checkKillPlayer->CheckKillPlayerLevelEnd = 70;
				break;
			case ECKPLE_90:
				checkKillPlayer->CheckKillPlayerLevelEnd = 90;
				break;
			}
		}
	}

	StartZone = StartType == EAQT_Zone;
}

INT UQuest::GetTemplateID() const
{
	return QuestTempID;
}

UBOOL UQuest::IsTimeLimit() const
{
	return DurationLimit > 0;
}

template<typename T>
void FindQuestItem(TArrayNoInit<T>& arr, const FString& errFmt, const FString& questName, INT questTemplateID)
{
	for (INT i = 0; i < arr.Num(); ++i)
	{
		T& item = arr(i);
		if(item.CheckItemID > 0)
		{
			item.CheckItem = (UItem*)MMOFacade::getSingleton().FindItemTemplate(item.CheckItemID);

			if(!item.CheckItem)
			{
				M_AddDataError(TEXT("quest.txt"), FString::Printf(
					LocalizeSecure(errFmt, questTemplateID, *questName, item.CheckItemID)));
			}
		}
	}
}

void UQuest::PostLoad()
{
	Super::PostLoad();

	if (GIsGame)
	{
		if(StartItemID > 0)
		{
			StartItem = (UItem*)MMOFacade::getSingleton().FindItemTemplate(StartItemID);
			if(!StartItem)
			{
				M_AddDataError(TEXT("quest.txt"), FString::Printf(LocalizeSecure(Localize(TEXT("Error"), TEXT("StartQuestItemNotFound"), TEXT("ProgError")),
					GetTemplateID(), *QuestName, StartItemID)));
			}
		}

		if(FinishItemID > 0)
		{
			FinishItem = (UItem*)MMOFacade::getSingleton().FindItemTemplate(FinishItemID);
			
			if(!FinishItem)
			{
				M_AddDataError(TEXT("quest.txt"), FString::Printf(LocalizeSecure(Localize(TEXT("Error"), TEXT("FinishQuestItemNotFound"), TEXT("ProgError")),
					GetTemplateID(), *QuestName, FinishItemID)));
			}
		}

		FindQuestItem<FQuestItem>(ItemLimit, Localize(TEXT("Error"), TEXT("ItemLimitQuestItemNotFound"), TEXT("ProgError")), QuestName, GetTemplateID());
		FindQuestItem<FQuestItem>(OfferItem, Localize(TEXT("Error"), TEXT("OfferItemQuestItemNotFound"), TEXT("ProgError")), QuestName, GetTemplateID());

		FindQuestItem<FQuestCheckItem>(CheckItem, Localize(TEXT("Error"), TEXT("CheckItemQuestItemNotFound"), TEXT("ProgError")), QuestName, GetTemplateID());

		for (INT i = 0; i < AwardItem.Num(); ++i)
		{
			FQuestAwardItem& item = AwardItem(i);
			if(item.AwardItemID > 0)
			{
				item.AwardItem = (UItem*)MMOFacade::getSingleton().FindItemTemplate(item.AwardItemID);
				if(!item.AwardItem)
				{
					M_AddDataError(TEXT("quest.txt"), FString::Printf(LocalizeSecure(Localize(TEXT("Error"), TEXT("AwardItemQuestItemNotFound"), TEXT("ProgError")),
						GetTemplateID(), *QuestName, item.AwardItemID)));
				}
			}
		}

		for (INT i = 0; i < SelectableAwardItem.Num(); ++i)
		{
			FQuestSelectableAwardItem& item = SelectableAwardItem(i);
			if(item.SelectableAwardItemID > 0)
			{
				item.SelectableAwardItem = (UItem*)MMOFacade::getSingleton().FindItemTemplate(item.SelectableAwardItemID);
				if(!item.SelectableAwardItem)
				{
					M_AddDataError(TEXT("quest.txt"), FString::Printf(LocalizeSecure(Localize(TEXT("Error"), TEXT("SelectableAwardItemNotFound"), TEXT("ProgError")),
						GetTemplateID(), *QuestName, item.SelectableAwardItemID)));
				}
			}
		}
	}

	if (CheckVar.Num() > MAX_CHECK_VAR_COUNT)
	{
		INT curCheckVarCount = CheckVar.Num();
		for (INT i = MAX_CHECK_VAR_COUNT; i < curCheckVarCount; ++ i)
		{
			CheckVar.Remove(i);
		}
	}

	if (CheckClassVar.Num() < 5)
	{
		INT curCheckVarCount = CheckClassVar.Num();
		for (INT i = 0; i < 5 - curCheckVarCount; ++ i) 
		{
			FQuestCheckClassVar checkVar;
			checkVar.CheckClassVarCnt = 0;
			checkVar.CheckClassVarDesc = TEXT("");
			CheckClassVar.AddItem(checkVar);
		}
	}
}

#if !SHIPPING_PC_GAME

template<typename T>
bool ReplaceQuestItem(TArrayNoInit<T>& arr)
{
	bool modified = false;
	for (INT i = 0; i < arr.Num(); ++i)
	{
		T& item = arr(i);
		if(item.CheckItem)
		{
			modified = true;
			item.CheckItemID = item.CheckItem->GetTemplateID();
		}
	}

	return modified;
}

void FixQuestItem()
{
	for( TObjectIterator<UQuest> It ; It ; ++It )
	{
		UQuest* quest = *It;
		if(quest)
		{
			bool modified = false;
			if(quest->StartItem)
			{
				modified = true;
				quest->StartItemID = quest->StartItem->GetTemplateID();
			}

			if(quest->FinishItem)
			{
				modified = true;
				quest->FinishItemID = quest->FinishItem->GetTemplateID();
			}

			bool tmpRVal = ReplaceQuestItem<FQuestItem>(quest->ItemLimit);		
			modified = modified || tmpRVal;

			tmpRVal = ReplaceQuestItem<FQuestItem>(quest->OfferItem);
			modified = modified || tmpRVal;

			tmpRVal = ReplaceQuestItem<FQuestCheckItem>(quest->CheckItem);
			modified = modified || tmpRVal;

			for (INT i = 0; i < quest->AwardItem.Num(); ++i)
			{
				FQuestAwardItem& item = quest->AwardItem(i);
				if(item.AwardItem)
				{
					modified = true;
					item.AwardItemID = item.AwardItem->GetTemplateID();
				}
			}

			for (INT i = 0; i < quest->SelectableAwardItem.Num(); ++i)
			{
				FQuestSelectableAwardItem& item = quest->SelectableAwardItem(i);
				if(item.SelectableAwardItem)
				{
					modified = true;
					item.SelectableAwardItemID = item.SelectableAwardItem->GetTemplateID();
				}
			}

			if(modified)
			{
				quest->MarkPackageDirty();
			}
		}
	}
}

#endif

UNPCTemplate* UQuest::GetStartNpc()
{
	if (StartNPCNew)
	{
		return StartNPCNew;
	}

	StartNPCNew = MMOFacade::getSingleton().FindNPCTemplate(StartNPCNewId);
	return StartNPCNew;
}

UNPCTemplate* UQuest::GetFinishNpc()
{
	if (FinishNpcNew)
	{
		return FinishNpcNew;
	}

	FinishNpcNew = MMOFacade::getSingleton().FindNPCTemplate(FinishNpcNewId);
	return FinishNpcNew;
}

UNPCTemplate* UQuest::GetCheckKillNpc(int index)
{
	if (index >= 0 && index < CheckKillNpc.Num())
	{
		if (CheckKillNpc(index).CheckKillNpcNew)
		{	
			return CheckKillNpc(index).CheckKillNpcNew;
		}

		CheckKillNpc(index).CheckKillNpcNew = MMOFacade::getSingleton().FindNPCTemplate(CheckKillNpc(index).CheckKillNpcNewId);
		return CheckKillNpc(index).CheckKillNpcNew;
	}

	return NULL;
}

UNPCTemplate* UQuest::GetAutoStartDialogHeadIcon()
{
	if (AutoStartDialogHeadIcon)
	{
		return AutoStartDialogHeadIcon;
	}

	AutoStartDialogHeadIcon = MMOFacade::getSingleton().FindNPCTemplate(AutoStartDialogHeadIconId);
	return AutoStartDialogHeadIcon;
}

UNPCTemplate* UQuest::GetAutoFinishDialogHeadIcon()
{
	if (AutoFinishDialogHeadIcon)
	{
		return AutoFinishDialogHeadIcon;
	}

	AutoFinishDialogHeadIcon = MMOFacade::getSingleton().FindNPCTemplate(AutoFinishDialogHeadIconId);
	return AutoFinishDialogHeadIcon;
}

UItem* UQuest::GetLimitItem(int index)
{
	if (index >= 0 && index < ItemLimit.Num())
	{
		if (ItemLimit(index).CheckItem)
		{	
			return ItemLimit(index).CheckItem;
		}

		if (ItemLimit(index).CheckItemID > 1)
		{
			ItemLimit(index).CheckItem = MMOFacade::getSingleton().FindItemTemplate(ItemLimit(index).CheckItemID);
			return ItemLimit(index).CheckItem;
		}
	}

	return NULL;
}

UItem* UQuest::GetCheckItem(int index)
{
	if (index >= 0 && index < CheckItem.Num())
	{
		if (CheckItem(index).CheckItem)
		{	
			return CheckItem(index).CheckItem;
		}

		if (CheckItem(index).CheckItemID > 1)
		{
			CheckItem(index).CheckItem = MMOFacade::getSingleton().FindItemTemplate(CheckItem(index).CheckItemID);
			return CheckItem(index).CheckItem;
		}
	}

	return NULL;
}

UItem* UQuest::GetAwardItem(int index)
{
	if (index >= 0 && index < AwardItem.Num())
	{
		if (AwardItem(index).AwardItem)
		{	
			return AwardItem(index).AwardItem;
		}

		if (AwardItem(index).AwardItemID > 1)
		{
			AwardItem(index).AwardItem = MMOFacade::getSingleton().FindItemTemplate(AwardItem(index).AwardItemID);
			return AwardItem(index).AwardItem;
		}
	}

	return NULL;
}

UItem* UQuest::GetSelectableAwardItem(int index)
{
	if (index >= 0 && index < SelectableAwardItem.Num())
	{
		if (SelectableAwardItem(index).SelectableAwardItem)
		{	
			return SelectableAwardItem(index).SelectableAwardItem;
		}

		if (SelectableAwardItem(index).SelectableAwardItemID > 1)
		{
			SelectableAwardItem(index).SelectableAwardItem = MMOFacade::getSingleton().FindItemTemplate(SelectableAwardItem(index).SelectableAwardItemID);
			return SelectableAwardItem(index).SelectableAwardItem;
		}
	}

	return NULL;
}

UItem* UQuest::GetStartItem()
{
	if (StartItem)
	{
		return StartItem;
	}

	if (StartItemID > 1)
	{
		StartItem = MMOFacade::getSingleton().FindItemTemplate(StartItemID);
		return StartItem;
	}

	return NULL;
}

UItem* UQuest::GetFinishItem()
{
	if (FinishItem)
	{
		return FinishItem;
	}

	if (FinishItemID > 1)
	{
		FinishItem = MMOFacade::getSingleton().FindItemTemplate(FinishItemID);
		return FinishItem;
	}

	return NULL;
}

USoundCue* UQuest::GetQuestPageSoundAccept(int index)
{
	if (index >= 0 && index < QuestPageSoundAccept.Num())
	{
		if (QuestPageSoundAccept(index).PageSound)
		{
			return QuestPageSoundAccept(index).PageSound;
		}

		QuestPageSoundAccept(index).PageSound = LoadUEObject<USoundCue>(QuestPageSoundAccept(index).PageSoundPath, TEXT("quest.txt"));
		return QuestPageSoundAccept(index).PageSound;
	}

	return NULL;
}

USoundCue* UQuest::GetQuestPageSoundSubmit(int index)
{
	if (index >= 0 && index < QuestPageSoundSubmit.Num())
	{
		if (QuestPageSoundSubmit(index).PageSound)
		{
			return QuestPageSoundSubmit(index).PageSound;
		}

		QuestPageSoundSubmit(index).PageSound = LoadUEObject<USoundCue>(QuestPageSoundSubmit(index).PageSoundPath, TEXT("quest.txt"));
		return QuestPageSoundSubmit(index).PageSound;
	}

	return NULL;
}

USoundCue* UQuest::GetQuestSoundAccept()
{
	if (QuestSoundAccept)
	{
		return QuestSoundAccept;
	}

	QuestSoundAccept = LoadUEObject<USoundCue>(QuestSoundAcceptPath, TEXT("quest.txt"));
	return QuestSoundAccept;
}

USoundCue* UQuest::GetQuestSoundAccepted()
{
	if (QuestSoundAccepted)
	{
		return QuestSoundAccepted;
	}

	QuestSoundAccepted = LoadUEObject<USoundCue>(QuestSoundAcceptedPath, TEXT("quest.txt"));
	return QuestSoundAccepted;
}

USoundCue* UQuest::GetQuestSoundUnderway()
{
	if (QuestSoundUnderway)
	{
		return QuestSoundUnderway;
	}

	QuestSoundUnderway = LoadUEObject<USoundCue>(QuestSoundUnderwayPath, TEXT("quest.txt"));
	return QuestSoundUnderway;
}

USoundCue* UQuest::GetQuestSoundSubmit()
{
	if (QuestSoundSubmit)
	{
		return QuestSoundSubmit;
	}

	QuestSoundSubmit = LoadUEObject<USoundCue>(QuestSoundSubmitPath, TEXT("quest.txt"));
	return QuestSoundSubmit;
}

USoundCue* UQuest::GetQuestSoundComplete()
{
	if (QuestSoundComplete)
	{
		return QuestSoundComplete;
	}

	QuestSoundComplete = LoadUEObject<USoundCue>(QuestSoundCompletePath, TEXT("quest.txt"));
	return QuestSoundComplete;
}

UTexture2D* UQuest::GetQuestSoundAcceptedIcon()
{
	if (QuestSoundAcceptedIcon)
	{
		return QuestSoundAcceptedIcon;
	}

	QuestSoundAcceptedIcon = LoadUEObject<UTexture2D>(QuestSoundAcceptedIconPath, TEXT("quest.txt"));
	return QuestSoundAcceptedIcon;
}

UTexture2D* UQuest::GetQuestSoundCompleteIcon()
{
	if (QuestSoundCompleteIcon)
	{
		return QuestSoundCompleteIcon;
	}

	QuestSoundCompleteIcon = LoadUEObject<UTexture2D>(QuestSoundCompleteIconPath, TEXT("quest.txt"));
	return QuestSoundCompleteIcon;
}