#include "stdafx.h"
#include "UStallManager.h"
#include "MMOFacade.h"
#include "MMODataManager.h"

UStallManager::UStallManager()
{
	Init();
	MMOUI::RegisterEventHandler(this,GameEvent_Stall);
}

UStallManager::~UStallManager()
{
	Clear();
	MMOUI::UnRegisterEventHandler(this,GameEvent_Stall);
}

void UStallManager::Init()
{
	appMemset(&_StallItemSellInfo, 0, sizeof(StallItemInfo) * MAX_STALL_SLOT_COUNT);
	appMemset(&_StallItemBuyInfo, 0, sizeof(StallItemInfo) * MAX_STALL_SLOT_COUNT);

	for (int i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
		_StallItemSellInfo[i].SrcBag = -1;
		_StallItemSellInfo[i].SrcSlot = -1;
	}

	for (int i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
		_StallItemBuyInfo[i].SrcBag = -1;
		_StallItemBuyInfo[i].SrcSlot = -1;
	}

	_StallName = TEXT("");

	_StallBuyPlayerId = 0;

	_StallUpgradeLeftTimeProgress = 0.0f;
	_StallUpgradeLeftTime = 0;
	_StallRecordInfoList.clear();
	_StallAttentionSet.clear();
	_StallPlayerActorMap.clear();

	_StallTax = EST_5;

	LoadStallSettingPriceBinary();
}

void UStallManager::Shutdown()
{
	appMemset(&_StallItemSellInfo, 0, sizeof(StallItemInfo) * MAX_STALL_SLOT_COUNT);
	appMemset(&_StallItemBuyInfo, 0, sizeof(StallItemInfo) * MAX_STALL_SLOT_COUNT);
}

void UStallManager::Clear()
{
	appMemset(&_StallItemSellInfo, 0, sizeof(StallItemInfo) * MAX_STALL_SLOT_COUNT);
	appMemset(&_StallItemBuyInfo, 0, sizeof(StallItemInfo) * MAX_STALL_SLOT_COUNT);

	_StallUpgradeLeftTimeProgress = 0.0f;
	_StallUpgradeLeftTime = 0;
	_StallRecordInfoList.clear();
	_StallAttentionSet.clear();
	_StallPlayerActorMap.clear();

	_StallTax = EST_5;

	ExportStallSettingPriceBinary();
}

void UStallManager::Update(FLOAT deltaTime)
{
	CTRL_TICK("MMOStallManager::Update");

	_UpdateStallUpgradeLeftTime(deltaTime);
}

void UStallManager::LoadStallSettingPriceBinary()
{
	//FArchive* pArchive      = NULL;
	//size_t  lDataLength		= 0; 
	//FString DataTypeName	= TEXT("PriceHistory");

	//if( MMOFacade::getSingleton().BeginLoadAccountBinary( &pArchive,DataTypeName ) )
	//{
	//	INT TotalSize = pArchive->TotalSize();
	//	INT Size = TotalSize * sizeof(BYTE) / sizeof(StallItemInfo);
	//	_StallItemInfoMap.Add( Size );
	//	pArchive->Serialize( _StallItemInfoMap.GetData(), pArchive->TotalSize() );
	//}
	//MMOFacade::getSingleton().EndLoadAccountBinary( pArchive );
}

void UStallManager::UpdateStallSettingPrice(INT itemId, INT price)
{
	StallItemPriceMap::iterator iter = _StallItemPriceMap.find(itemId);
	if (iter != _StallItemPriceMap.end())
	{
		iter->second = price;
	}
	else
	{
		_StallItemPriceMap.insert(std::make_pair(itemId, price));
	}
}

void UStallManager::ClearOnSaleFlag()
{
	for (int i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
		if (_StallItemSellInfo[i].Item)
		{
			if (_StallItemSellInfo[i].SrcBag >= 0 && _StallItemSellInfo[i].SrcSlot >= 0)
			{
				FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(_StallItemSellInfo[i].SrcBag, _StallItemSellInfo[i].SrcSlot);
				if (itemInfo)
				{
					itemInfo->ResetSaleDur();
				}
			}

			_StallItemSellInfo[i].Item->DefaultDuration += _StallItemSellInfo[i].CurDur;
		}
	}
}

INT UStallManager::GetCachedStallSettingPrice(INT itemId)
{
	StallItemPriceMap::iterator iter = _StallItemPriceMap.find(itemId);
	if (iter != _StallItemPriceMap.end())
	{
		return iter->second;
	}

	return MIN_STALL_PRICE;
}

UBOOL UStallManager::CheckStartStallSetting()
{
	for (int i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
		if (_StallItemSellInfo[i].CurDur > 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

INT UStallManager::CalculateStallSettingTotalValue()
{
	INT ret = 0;
	for (int i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
		if (_StallItemSellInfo[i].CurDur > 0)
		{
			ret += (_StallItemSellInfo[i].CurDur * _StallItemSellInfo[i].Price);
		}
	}
	return ret;
}

void UStallManager::ExportStallSettingPriceBinary()
{
	//void    *pData			= _StallItemInfoMap.GetData();
	//size_t  lDataLength		= sizeof(StallItemInfo)*_StallItemInfoMap.Num();
	//FString DataTypeName	= TEXT("-PriceHistory-");

	//MMOFacade::getSingleton().ExportSimpleAccountData( pData,lDataLength,DataTypeName );
}

void UStallManager::AddStallItem(int stallSlot, int srcConId, int srcSlotId, int itemCount, int price)
{
	UItem* item = NULL;
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( mainPlayer->ItemManager != NULL)
		item = mainPlayer->ItemManager->GetItemBySlot(ECONTAINER_SLOT(srcConId), srcSlotId);
	if( !item )
		return;

	BYTE durType = item->ItemDuration;
	FClientItemInfo* tItemInfo = MMOFacade::getSingleton().FindItem(srcConId , srcSlotId);
	if (tItemInfo)
	{
		// fix bug #23292 cannot subtract here, it would affect durability and the stored value of the experience bottle
		//tItemInfo->MinusSaleDur(itemCount); 
		tItemInfo->SetSaleDur(tItemInfo->m_nCurDur);
	}

	StallItemInfo stallItemInfo;
	if (!GetStallSellItem(stallSlot, stallItemInfo))
	{
		return;
	}

	stallItemInfo.ItemId = item->ItemId;
	stallItemInfo.Item = item;
	stallItemInfo.DurType = durType;
	stallItemInfo.Price = price;
	stallItemInfo.MaxStack = item->MaxStackNum;
	stallItemInfo.SrcBag = srcConId;
	stallItemInfo.SrcSlot = srcSlotId;

	if (durType == E_DURTYPE_STACK)
	{
		// currently cannot stack on an already sold slot
		stallItemInfo.CurDur = itemCount;
		if (stallItemInfo.Item)
		{
			stallItemInfo.Item->DefaultDuration -= stallItemInfo.CurDur;
		}
	}
	else 
	{
		// equipment or items without durability cannot be stacked
		stallItemInfo.CurDur = 1;
	}

	SetStallSellItem(stallSlot, stallItemInfo);

	MMOFacade::getSingleton().LockItem(srcConId, srcSlotId, ESPType_StallSetting);

	MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_StallSettingItemChange, stallSlot);
	MMOUI::PostGameEvent(&evt);
}

void UStallManager::AddStallBuyItem(int stallSlot, int itemId, int itemCurDur, int itemPrice,int modfiyTime)
{
	const UItem* tItem = MMOFacade::getSingleton().FindItemTemplate( itemId );
	class UItem* item = const_cast<UItem*>( tItem ); 

	StallItemInfo stallItemInfo;
	if (!GetStallBuyItem(stallSlot, stallItemInfo))
	{
		return;
	}

	stallItemInfo.ItemId = itemId ;
	stallItemInfo.DurType = item->ItemDuration;
	stallItemInfo.Price = itemPrice;
	stallItemInfo.ModifyTime = modfiyTime;
	stallItemInfo.SrcBag   = E_CONSLOT_STALL_BUY; //fixed, StallBuy stored locally
	stallItemInfo.SrcSlot  = stallSlot;

	if (stallItemInfo.DurType == E_DURTYPE_STACK)
	{
		stallItemInfo.CurDur = itemCurDur;
	}
	else 
	{
		stallItemInfo.CurDur = 1;
	}

	SetStallBuyItem(stallSlot, stallItemInfo);

	MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_StallBuyItemChange, stallSlot);
	MMOUI::PostGameEvent(&evt);
}

void UStallManager::ResetStallSellItem(int stallSlot)
{
	if (stallSlot < 0 || stallSlot >= MAX_STALL_SLOT_COUNT)
	{
		return;
	}

	// bag unlock
	if (_StallItemSellInfo[stallSlot].SrcBag >= 0 && _StallItemSellInfo[stallSlot].SrcSlot >= 0)
	{
		MMOFacade::getSingleton().LockItem(_StallItemSellInfo[stallSlot].SrcBag, _StallItemSellInfo[stallSlot].SrcSlot, -1);
	}

	_StallItemSellInfo[stallSlot].ItemId = 0;
	_StallItemSellInfo[stallSlot].Item = 0;
	_StallItemSellInfo[stallSlot].DurType = 0;
	_StallItemSellInfo[stallSlot].MaxStack = 0;
	_StallItemSellInfo[stallSlot].CurDur = 0;
	_StallItemSellInfo[stallSlot].Price = 0;
	_StallItemSellInfo[stallSlot].SrcBag = -1;
	_StallItemSellInfo[stallSlot].SrcSlot = -1;
}

void UStallManager::ResetStallBuyItem(int stallSlot)
{
	if (stallSlot < 0 || stallSlot >= MAX_STALL_SLOT_COUNT)
	{
		return;
	}

	MMOFacade::getSingleton().DeleteItem(E_CONSLOT_STALL_BUY, stallSlot);

	_StallItemBuyInfo[stallSlot].ItemId = 0;
	_StallItemBuyInfo[stallSlot].Item = 0;
	_StallItemBuyInfo[stallSlot].DurType = 0;
	_StallItemBuyInfo[stallSlot].MaxStack = 0;
	_StallItemBuyInfo[stallSlot].CurDur = 0;
	_StallItemBuyInfo[stallSlot].Price = 0;
	_StallItemBuyInfo[stallSlot].SrcBag = -1;
	_StallItemBuyInfo[stallSlot].SrcSlot = -1;
}

void UStallManager::BuyStallItem(int stallSlot, int leftCount)
{
	StallItemInfo stallItemInfo;
	if (!GetStallBuyItem(stallSlot, stallItemInfo))
	{
		return;
	}
	//add the split-out Dur amount back; Flag is not handled here, cleared all at once at the end
	stallItemInfo.CurDur = leftCount;
	SetStallBuyItem(stallSlot, stallItemInfo);

	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(stallItemInfo.SrcBag , stallItemInfo.SrcSlot);
	if (itemInfo)
	{
		//recalculate total price of items currently for sale
		itemInfo->OnSaleCount = leftCount;
		itemInfo->m_nCurDur = leftCount;
	}

	if (stallItemInfo.CurDur <= 0)
	{
		ResetStallBuyItem(stallSlot);
	}

	MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_StallBuyItemChange, stallSlot);
	MMOUI::PostGameEvent(&evt);
}

INT UStallManager::GetEmptyStallSettingSlot()
{
	for (int i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
        if (_StallItemSellInfo[i].CurDur <= 0)
		{
            return i;
		}
    }

    return -1;
}

void UStallManager::ResetStallSettingItems()
{
	for (INT i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
		ResetStallSellItem(i);
	}
}

void UStallManager::ResetStallBuyItems()
{
	for (INT i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
		ResetStallBuyItem(i);
	}
}

void UStallManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if (evt->GetEventType() == EGameEvent::GameEvent_Stall)
	{
		MMOGameEvent_StallEvent *e = (MMOGameEvent_StallEvent*)evt;
		if (e->EventType == MMOGameEvent_StallEvent::ESET_CachedPriceChange)
		{

		}
		else if (e->EventType == MMOGameEvent_StallEvent::ESET_StateChange)
		{
			if(UStallManager::getSingleton().GetStallBuyPlayerId() == e->Paramter)
			{
				ResetStallBuyItems();
				MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallBuy, FALSE,"");
			}
			else if (MMOFacade::getSingleton().IsSelf(e->Paramter))
			{
				if (MMOFacade::getSingleton().GetMainPlayer()->MoveType < EMT_PrepareStall)
				{
					ClearOnSaleFlag();
					ResetStallSettingItems();
					MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallSetting, FALSE,"");
				}
			}
		}
	}
}

void UStallManager::SellStallItem(int stallSlot, int sellCount)
{
	StallItemInfo stallItemInfo;
	if (!GetStallSellItem(stallSlot, stallItemInfo))
	{
		return;
	}

	// add to trade record
	StallRecordInfo record;
	__time32_t localTime;
	_time32(&localTime);
	struct tm* time = _localtime32(&localTime); 
	if (time)
	{
		record.Time = FString::Printf(TEXT("%02d:%02d:%02d"), time->tm_hour, time->tm_min, time->tm_sec);
	}
	UItem* item = MMOFacade::getSingleton().FindItemTemplate(stallItemInfo.ItemId);
	if (item)
	{
		record.ItemName = FString::Printf(TEXT("%s x%d"), *item->GetDisplayName(), sellCount);
	}
	record.Operate = TEXT("Sell");
	record.Profit = stallItemInfo.Price * sellCount;
	_StallRecordInfoList.push_back(record);

	stallItemInfo.CurDur -= sellCount;
	SetStallSellItem(stallSlot, stallItemInfo);

	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(stallItemInfo.SrcBag , stallItemInfo.SrcSlot);
	if (itemInfo)
	{
		//recalculate total price of items currently for sale
		itemInfo->OnSaleCount -= sellCount;
		//tItemInfo->m_nCurDur = tItemInfo->OriDur - tItemInfo->OnSaleCount;
		//tItemInfo->OriDur -= sellCount;
	}

	if(stallItemInfo.CurDur <= 0)
	{
		if( itemInfo )
		{
			itemInfo->ResetSaleDur();
		}
		ResetStallSellItem(stallSlot);
	}

	MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_StallSettingItemChange, stallSlot);
	MMOUI::PostGameEvent(&evt);
}

void UStallManager::RemoveStallItem(int stallSlot)
{
	StallItemInfo stallItemInfo;
	if (!GetStallSellItem(stallSlot, stallItemInfo))
	{
		return;
	}

	// add the split-out Dur amount back; Flag is not handled here, cleared all at once at the end
	if (stallItemInfo.Item)
	{
		stallItemInfo.Item->DefaultDuration += stallItemInfo.CurDur;
	}

	stallItemInfo.CurDur = 0;
	SetStallSellItem(stallSlot, stallItemInfo);

	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(stallItemInfo.SrcBag, stallItemInfo.SrcSlot);
	if (itemInfo)
	{
		itemInfo->ResetSaleDur();
	}
	ResetStallSellItem(stallSlot);

	MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_StallSettingItemChange, stallSlot);
	MMOUI::PostGameEvent(&evt);
}

void UStallManager::SetStallBuyItem(int stallSlot, const StallItemInfo& info)
{
	if (stallSlot < 0 || stallSlot >= MAX_STALL_SLOT_COUNT)
	{
		return;
	}

	_StallItemBuyInfo[stallSlot] = info;
}

UBOOL UStallManager::GetStallBuyItem(int stallSlot, StallItemInfo& info)
{
	if (stallSlot < 0 || stallSlot >= MAX_STALL_SLOT_COUNT)
	{
		return FALSE;
	}

	info = _StallItemBuyInfo[stallSlot];
	return TRUE;
}

void UStallManager::SetStallSellItem(int stallSlot, const StallItemInfo& info)
{
	if (stallSlot < 0 || stallSlot >= MAX_STALL_SLOT_COUNT)
	{
		return;
	}

	_StallItemSellInfo[stallSlot] = info;
}

UBOOL UStallManager::GetStallSellItem(int stallSlot, StallItemInfo& info)
{
	if (stallSlot < 0 || stallSlot >= MAX_STALL_SLOT_COUNT)
	{
		return FALSE;
	}

	info = _StallItemSellInfo[stallSlot];
	return TRUE;
}

void UStallManager::SetStallBuyPlayerId(UINT32 playerId)
{
	_StallBuyPlayerId = playerId;
}

INT UStallManager::GetStallBuyPlayerId()
{
	return _StallBuyPlayerId;
}

void UStallManager::ChangeActorStallState(AMMOPlayer* player, UINT8 stallState)
{
	if (!player)
	{
		return;
	}

	switch (stallState)
	{
	case essNotOpen:
		RealCloseStall(player);
		break;

	case essPreparing:
		{
			if (player->MoveType == EMT_InStall)
			{
				CloseStall(player);
			}
			else
			{
				OpenStall(player);
			}
		}
		break;

	case essOpen:
		RealOpenStall(player);
		break;
	}

    //MMOUI::GetUIManager()->delegateUpdateHintText( player );
};

void UStallManager::CreateStallActor(AMMOPlayer* player)
{
	DestroyStallActor(player);

	if (!player)
	{
		return;
	}

	AMMOStallActor* stallActor = Cast<AMMOStallActor>(GWorld->SpawnActor(AMMOStallActor::StaticClass()));
	if (!stallActor)
	{
		return;
	}

	// add to management list
	_StallPlayerActorMap.insert(std::make_pair(player->ServerID, stallActor->GetIndex()));

	if (!stallActor->StallMeshComponent)
	{
		return;
	}

	FString meshName = player->StallMeshName1;
	if (player->StallLevel > 0)
	{
		meshName = player->StallMeshName2;
	}

	UStaticMesh* stallMesh = LoadUEObject<UStaticMesh>(meshName, TEXT("DefaultMMOGame.ini"));
	stallActor->StallMeshComponent->SetStaticMesh(stallMesh);
	stallActor->StallMeshComponent->SetLightEnvironment(player->LightEnvironment);
	if (player->Mesh)
	{
		stallActor->StallMeshComponent->SetLightingChannels(player->Mesh->LightingChannels);
	}

	FVector pos = player->GetFootLocation();
	pos.X += 60.0f;
	pos.Z += 5.0f;
	stallActor->SetLocation(pos);
}

void UStallManager::DestroyStallActor(AMMOPlayer* player)
{
	if (!GWorld)
	{
		return;
	}

	if (!player)
	{
		return;
	}

	StallPlayerActorMap::iterator iter = _StallPlayerActorMap.find(player->ServerID);
	if (iter != _StallPlayerActorMap.end())
	{
		AMMOStallActor* stallActor = Cast<AMMOStallActor>(UObject::GetIndexedObject(iter->second));
		if (stallActor && !stallActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
		{
			GWorld->DestroyActor(stallActor);
		}

		_StallPlayerActorMap.erase(iter);
	}
}

void UStallManager::RequestOpenStall()
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	// check whether stall can be set up on Guid
	ZONE_DATA* zoneData = NULL;
	GRID_DATA* gridData = NULL;
	INT tax = EST_5;
	gServerGridWorld.GetZoneAndGrid(mainPlayer->Location, zoneData, gridData);
	if (zoneData && gridData)
	{
		if (!gridData->uStallLevel)
		{
			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errStallForbid"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errStallForbid"));
			return;
		}
	}

	// check distance to surrounding stalls
	STLInt2IntMap* characterMap = MMOFacade::getSingleton().GetCharacterMap();
	if (characterMap)
	{
		STLInt2IntMap::iterator iterCharacter = characterMap->begin();
		for (; iterCharacter != characterMap->end(); ++ iterCharacter)
		{
			AMMOPlayer* player = Cast<AMMOPlayer>(UObject::GetIndexedObject(iterCharacter->second));
			if (player && (player->MoveType == EMT_PrepareStall || player->MoveType == EMT_InStall))
			{
				FLOAT playerDist = mainPlayer->Location.Distance(player->Location);
				FLOAT gridSize = FLOAT(GRID_WIDTH) / FLOAT(POINT_PER_CM);
				if (playerDist < gridSize * 3.0f)
				{
					FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errStallOthersOpen"));
					MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table ,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errStallOthersOpen"));
					return;
				}
			}
		}
	}

	// find the nearest stall spot
	AMMOStallPoint* stallPoint = 0;
	FLOAT dist = 1e30;
	for (TObjectIterator<AMMOStallPoint> iter; iter; ++ iter)
	{
		if (FVector(mainPlayer->Location - iter->Location).Size() <= dist)
		{
			stallPoint = *iter;
			dist = FVector(mainPlayer->Location - stallPoint->Location).Size();
		}
	}

	if (stallPoint)
	{
		MMOFacade::getSingleton().GetMainController()->eventClearAllCmd();
		MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToActor(NULL, stallPoint->Location, 64.0f);
		MMOFacade::getSingleton().GetMainController()->eventAddCmdOpenStall();
	}
}

void UStallManager::OpenStall(AMMOPlayer* player)
{
	if (!player)
	{
		return;
	}

	player->MoveType = EMT_PrepareStall;
	player->OpenStall();
	player->StallName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.RequireOpenStall"));

	CreateStallActor(player);

	MMOUI::GetUIManager()->delegateResetPlayerHint(player);
	
	if (MMOFacade::getSingleton().IsSelf(player->ServerID)) 
	{
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Bag, TRUE, "");
		MMOUI::GetUIManager()->delegateOpenBag(E_CONSLOT_DEFAULT);
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Stall_StallSetting, TRUE, "");

		// tax rate
		SetTax(EST_5);
		ZONE_DATA* zoneData = NULL;
		GRID_DATA* gridData = NULL;
		gServerGridWorld.GetZoneAndGrid(player->Location, zoneData, gridData);
		if (zoneData && gridData)
		{
			switch (gridData->uStallLevel)
			{
			case 1:
				SetTax(EST_5);
				break;
			case 2:
				SetTax(EST_10);
				break;
			case 3:
				SetTax(EST_20);
				break;
			}
		}
	}
}

void UStallManager::CloseStall(AMMOPlayer* player)
{
	if (!player)
	{
		return;
	}

	player->MoveType = EMT_PrepareStall;
	player->StallName = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.RequireOpenStall"));
	MMOUI::GetUIManager()->delegateResetPlayerHint(player);

	MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_StateChange, player->ServerID);
	MMOUI::PostGameEvent(&evt);
}

void UStallManager::RealOpenStall(AMMOPlayer* player)
{
	if (!player)
	{
		return;
	}

	player->MoveType = EMT_InStall;
	player->OpenStall();

	CreateStallActor(player);

	MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_StateChange, player->ServerID);
	MMOUI::PostGameEvent(&evt);
}

void UStallManager::RealCloseStall(AMMOPlayer* player)
{
	if (!player)
	{
		return;
	}

	DestroyStallActor(player);
	player->CloseStall();

	MMOUI::GetUIManager()->delegateResetPlayerHint(player);

	if (MMOFacade::getSingleton().IsSelf(player->ServerID))
	{
		MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_StateChange, player->ServerID);
		MMOUI::PostGameEvent(&evt);

		UMMOUIBag* bag = (UMMOUIBag*)MMOUI::FindUIPanel(MMOUI::GetUIManager()->Panel_Bag);
		if (bag)
		{
            bag->eventRefreshBag();
        }

		// #19735
		_StallRecordInfoList.clear();
	}
}

void UStallManager::SetStallUpgradeLeftTime(UINT32 leftTime)
{
	__time32_t destTime = leftTime;
	_localtime32(&destTime);

	__time32_t localTime;
	_time32(&localTime);
	_localtime32(&localTime);

	_StallUpgradeLeftTime = destTime - localTime;

	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if (player)
	{
		player->StallLevel = _StallUpgradeLeftTime > 0 ? 1 : 0;
	}

	MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_SyncStall);
	MMOUI::PostGameEvent(&evt);
}

void UStallManager::_UpdateStallUpgradeLeftTime(FLOAT deltaTime)
{
	if (_StallUpgradeLeftTime > 0)
	{
		_StallUpgradeLeftTimeProgress += deltaTime;
		if (_StallUpgradeLeftTimeProgress >= 1.0f)
		{
			_StallUpgradeLeftTime -= appFloor(_StallUpgradeLeftTimeProgress);

			if (_StallUpgradeLeftTime <= 0)
			{
				AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
				if (player)
				{
					player->StallLevel = 0;
				}
			}

			MMOGameEvent_StallEvent evt(MMOGameEvent_StallEvent::ESET_SyncStall);
			MMOUI::PostGameEvent(&evt);

			_StallUpgradeLeftTimeProgress = 0.0f;
		}
	}
}

void UStallManager::ChangeStallLevel(UINT32 roleId, UINT8 level)
{
	AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(roleId));
	if (!player)
	{
		return;
	}

	player->StallLevel = level;
	player->OpenStall();

	CreateStallActor(player);
}

void UStallManager::AddAttention(const FString& stallName)
{
	_StallAttentionSet.insert(stallName);
}

void UStallManager::RemoveAttention(const FString& stallName)
{
	StallAttentionSet::iterator iter = _StallAttentionSet.find(stallName);
	if (iter != _StallAttentionSet.end())
	{
		_StallAttentionSet.erase(iter);
	}
}

UBOOL UStallManager::IsAttentionStall(const FString& stallName)
{
	StallAttentionSet::iterator iter = _StallAttentionSet.find(stallName);
	if (iter != _StallAttentionSet.end())
	{
		return TRUE;
	}

	return FALSE;
}

void UStallManager::SetTax(INT tax)
{
	_StallTax = tax;
}

INT64 UStallManager::GetStallSellInfoPrice()
{
	INT64 price = 0;

	for (int i = 0; i < MAX_STALL_SLOT_COUNT; ++ i)
	{
		price += _StallItemSellInfo[i].Price * _StallItemSellInfo[i].CurDur;
	}

	return price;
}