#include "stdafx.h"
#include "MMOFacade.h"
#include "Shop.h"
#include "CommonGameEvents.h"

#define ShopDataFile TEXT("ShopData")
#define Millisecond 1000
ShopManager::ShopManager()
{
	_curShop = NULL;
	_crc = 0;
	_hasLoadShop = FALSE;
	_hasCheckShopCrc = FALSE;
	_curShopType = InvalidShop;
	bShowShop = FALSE;
}

void ShopManager::Init()
{
	//Changed again to not read from local
	_crc = 0;

	//SCOPE_LOG_TIME(___INIT__, TEXT("ShopManager::Init"), 0);

	//TArray< TArray<FString> > strArr2D;
	//if(!MMOFacade::ReadConfigString(strArr2D, TEXT("Shop.txt")))
	//{
	//	warnf(NAME_Error, TEXT("!!!!!!!!!!!!!! Read Shop.txt failed!"));
	//	return;
	//}

	//// Skip the first 2 rows of table header, not needed
	//for (INT i = 2; i < strArr2D.Num(); ++i)
	//{
	//	const INT C_SHOP_ID_IDX = 2;
	//	const INT C_SHOP_NAME_IDX = 3;
	//	const INT C_SHOP_ITEM_IDX_BEGIN = 4;
	//	const INT C_SHOP_ITEM_DAT_CNT = 4;

	//	const TArray<FString>& datArr = strArr2D(i);
	//	FShopData dat;

	//	if(datArr.Num() > 4)
	//	{
	//		dat.Id = appAtoi(*datArr(C_SHOP_ID_IDX));
	//		dat.ShopName = datArr(C_SHOP_NAME_IDX);
	//	}

	//	for (INT j = C_SHOP_ITEM_IDX_BEGIN; j < datArr.Num(); j += C_SHOP_ITEM_DAT_CNT)
	//	{
	//		if(datArr(j).Len() > 0)
	//		{
	//			FShopItemTemplate shopItem;
	//			INT itemID = appAtoi(*datArr(j));
	//			shopItem.ShopItem = MMOFacade::getSingleton().FindItemTemplate(itemID);
	//			shopItem.ItemCount = appAtoi(*datArr(j + 1));
	//			shopItem.CostType = appAtoi(*datArr(j + 2));
	//			shopItem.CostValue = appAtoi(*datArr(j + 3));
	//			dat.ItemList.AddItem(shopItem);
	//		}
	//	}

	//	_AddShopData(dat);
	//}

	BlackMarketData::getSingleton().InitFromFile(TEXT("black_market.txt"), true);
}

ServerShopData* ShopManager::FindShopData(INT shopID) const
{
	return (ServerShopData*)_shopDataMap.Find( shopID );
}

ServerShopData* ShopManager::FindBlackMarketData(INT ID) const
{
	return (ServerShopData*)_blackMarketDataMap.Find(ID);
}

const ServerShopItemData* ShopManager::FindShopItem(INT ShopTemplateID, INT ItemIdx) const
{
	ServerShopData* dat = FindShopData(ShopTemplateID);
	if(dat)
	{
		if(ItemIdx < dat->_ShowItemList.Num())
		{
			return &dat->_ShowItemList(ItemIdx);
		}
	}

	return 0;
}

void ShopManager::CheckShopData()
{
	/*if(_hasCheckShopCrc)
	{
	return;
	}*/

	LoadShopData();

	CMessageHandler::GetInstance().Send_RequestShop( _crc );

	_hasCheckShopCrc = TRUE;
}

const ServerShopItemData* ShopManager::GetCurShopItem( INT ItemIdx )
{
	if(_curShop)
	{

		for ( INT i = 0 ; i < _curShop->_ShowItemList.Num() ; i++ )
		{
			if (_curShop->_ShowItemList(i).ShopSlot == ItemIdx)
			{
				return &_curShop->_ShowItemList(i);
			}
		}
		
	}
	return NULL;
}


ServerShopItemData* ShopManager::GetCurShopItemByItemTempId( INT itemTempId )
{
	//Note, this function finds correctly only if the shop cannot contain two things with the same tempId
	if(_curShop)
	{
		for ( INT i = 0 ; i < _curShop->_ShowItemList.Num() ; i++ )
		{
			if( _curShop->_ShowItemList(i).ShopItem && _curShop->_ShowItemList(i).ShopItem->GetTemplateID() == itemTempId )
			{
				return &_curShop->_ShowItemList(i);
			}
		}
	}
	return NULL;
}

void ShopManager::ClearCurShopItemList()
{
	_equipShopItemList.Empty();
	_materialShopItemList.Empty();
	_normalShopItemList.Empty();
}

void ShopManager::UpdateShopItemList()
{
	if( !_curShop )
	{
		if (_curShopType == NormalShop)
		{
			_curShop = FindShopData( _curShopId );
		}
		else
		{
			_curShop = FindBlackMarketData( _curShopId );
		}
	}

	if( !_curShop )
	{
		return;
	}

	if(_curShop->_ShowItemList.Num() <= 0)
	{
		return;
	}

	for( int i = 0 ; i < _curShop->_ShowItemList.Num() ; i++ )
	{
		ServerShopItemData& SI = _curShop->_ShowItemList(i);
		if( !SI.ShopItem )
			continue;

		if (_curShopType != BlackMarket)
			SI.ShopSlot = i;

		//if( SI.ShopItem->ItemType == TEXT("ITEMTYPE_EQUIP") )
			//_equipShopItemList.AddItem(SI);
		//else if( SI.ShopItem->ItemType == TEXT("ITEMTYPE_MATERIAL") )
			//_materialShopItemList.AddItem(SI);
		//else
			_normalShopItemList.AddItem(SI);
	}
}

void ShopManager::SetCurShop( INT shopId )
{
	if ( _curShopType == NormalShop)
	{
		_curShop = FindShopData(shopId);
	}
	else
	{
		_curShop = FindBlackMarketData(shopId);
	}
	_curShopId = shopId;
}

void ShopManager::ClearCurShop()
{
	if (_curShop)
	{
		_curShop->_ShowItemList.Empty();
		_curShop = NULL;
	}
	
}
//void ShopManager::_AddShopData(const FShopData& dat)
//{
//	if(FindShopData(dat.Id) != NULL)
//	{
//		warnf(NAME_Error, TEXT("!!!!!!!!!!!!!! duplicate shop id:%d"), dat.Id);
//	}
//
//	_shopData[dat.Id] = dat;
//}

INT ShopManager::GetCurShopItemNum()
{
	if(_curShop)
	{
		return _curShop->_ShowItemList.Num();
	}
	return 0;
}

void ShopManager::AddShopItem( INT count , NPC_SHOP_DATA* data )
{
	_shopDataMap.Empty();

	for ( INT i = 0 ; i < count ; i++ )
	{
		ServerShopData& shopData = _shopDataMap.FindOrAdd(data[i].m_nShopID);
		shopData._ShopId = data[i].m_nShopID;
		shopData._ShopName = UTF8_TO_TCHAR(data[i].m_szShopName) ;
		ServerShopItemData shopItem;
		shopItem.ShopItem = MMOFacade::getSingleton().FindItemTemplate( data[i].m_nItemID );
		shopItem.CostType[0] = data[i].m_nCostItemID1;
		shopItem.CostValue[0] = data[i].m_nCostValue1;
		shopItem.CostType[1] = data[i].m_nCostItemID2;
		shopItem.CostValue[1] = data[i].m_nCostValue2;
		shopItem.RequireForceID = data[i].m_nRequireForceID;
		shopItem.Reputation = data[i].m_nReputation;
		shopItem.RequireGuildBuildingID = data[i].m_nRequireGuildBuildingID;
		shopItem.BuildingLevel = data[i].m_nBuildingLevel;
		shopItem.ItemCount = data[i].m_nItemCount;
		shopItem.ShopTypeSell = data[i].m_nShopTypeSell;
		shopData._ShowItemList.AddItem(shopItem);
	}
	
	MMOGameEvent_Any msg(GameEvent_ShopUpdate);
	MMOUI::PostGameEvent(&msg);
}

void ShopManager::SaveShopData( INT count , NPC_SHOP_DATA* data )
{
	FArchive* pArchive = NULL;

	if( MMOFacade::getSingleton().BeginExportAccountData( &pArchive , ShopDataFile , false ) )
	{
		//Store crc first
		pArchive->Serialize( (void*)(&_crc) , sizeof(UINT32) );
		//Store goods count
		pArchive->Serialize( (void*)(&count) , sizeof(INT) );
		pArchive->Serialize( (void*)(data) , sizeof(NPC_SHOP_DATA)*count );

	}
	MMOFacade::getSingleton().EndExportAccountData( pArchive );
}

void ShopManager::LoadShopData()
{
	if(_hasLoadShop)
		return;

	FArchive* pArchive	= NULL;
	if( MMOFacade::getSingleton().BeginLoadAccountBinary( &pArchive , ShopDataFile , false ) )
	{
		INT TotalSize = pArchive->TotalSize();		
		pArchive->Serialize( &_crc, sizeof(UINT32) ); 
		INT count = 0;
		pArchive->Serialize( &count, sizeof(INT) );
		NPC_SHOP_DATA* shopData = new NPC_SHOP_DATA[count];
		pArchive->Serialize( shopData, sizeof(NPC_SHOP_DATA)*count );

		AddShopItem( count , shopData );
		SAFE_DELETE(shopData);
	}

	_hasLoadShop = TRUE;
	MMOFacade::getSingleton().EndLoadAccountBinary( pArchive );
}

void ShopManager::ShowShopType()
{
	_shopType.Empty();

	if (!_curShop)
		return;

	for(int i=0; i<_curShop->_ShowItemList.Num();i++)
	{
		ServerShopItemData SI = _curShop->_ShowItemList(i);
		if( !SI.ShopItem )
			continue;
		_shopType.AddUniqueItem(SI.CostType[0]);
	}

	MMOGameEvent_Any evt( GameEvent_ShopTypeUpdate );
	MMOUI::PostGameEvent( &evt );
}

void BlackMarketDataProcessor::ParseData(ServerShopItemData* obj, class CTabFile* dataRow, int rowIdx)
{
	obj->ShopSlot = TReadDataValue_Int<BlackMarketData>(TEXT("BlackMarketProductID"), dataRow, rowIdx);
	INT ItemID = TReadDataValue_Int<BlackMarketData>(TEXT("ItemID"), dataRow, rowIdx);
	obj->ShopItem = MMOFacade::getSingleton().FindItemTemplate( ItemID);
	obj->CostType[0] = obj->CostType[1]= MONEYTYPE_02;
	obj->CostValue[0] = obj->CostValue[1]= TReadDataValue_Int<BlackMarketData>(TEXT("Price"), dataRow, rowIdx);
	obj->LimitType = TReadDataValue_Int<BlackMarketData>(TEXT("LimitType"), dataRow, rowIdx);
	obj->ItemCount = TReadDataValue_Int<BlackMarketData>(TEXT("Limit"), dataRow, rowIdx);
	obj->MaxCoolDownTime  = TReadDataValue_Int<BlackMarketData>(TEXT("CooldownTime"), dataRow, rowIdx)* Millisecond;
	obj->CurrentCoolDown = 0;
	INT blackMarketId = TReadDataValue_Int<BlackMarketData>(TEXT("BlackMarketID"), dataRow, rowIdx);
	ShopManager::getSingleton().AddItemToBlackMarket(*obj,blackMarketId);
}

ServerShopItemData* BlackMarketDataProcessor::CreateDataObject( INT templateID, bool& needAddToMap )
{
	ServerShopItemData* item = BlackMarketData::getSingleton().FindAndModifyData(templateID);

	if(!item)
	{
		needAddToMap = true;
		item = new ServerShopItemData;
	}
	else
	{
		needAddToMap = false;
	}

	return item;
}

void ShopManager::AddItemToBlackMarket(ServerShopItemData& obj, INT shopId)
{
	ServerShopData& blackMarketData = _blackMarketDataMap.FindOrAdd(shopId);
	blackMarketData._ShowItemList.AddItem(obj);
	blackMarketData._ShopId = shopId;
}

void ShopManager::UpdateBlackMarketItemListCount(BLACK_MARKET_LIMIT_INFO* item)
{
	if (!_curShop)
		return;

	for( int i = 0 ; i < _curShop->_ShowItemList.Num() ; i++ )
	{
		ServerShopItemData& SI = _curShop->_ShowItemList(i);
		if( !SI.ShopItem )
			continue;
		if (SI.ShopSlot == item->nBlackMarketProductID)
		{
			SI.ItemCount = item->nCount;
			break;
		}
	}
}

void ShopManager::UpdateBlackMarketItemListCoolDown(BLACK_MARKET_COOLDOWN_INFO* item)
{
	for( int i = 0 ; i < _curShop->_ShowItemList.Num() ; i++ )
	{
		ServerShopItemData& SI = _curShop->_ShowItemList(i);
		if( !SI.ShopItem )
			continue;
		if (SI.ShopSlot == item->nBlackMarketProductID)
		{
			__time32_t lastTime = item->dwCooldown;
			_localtime32(&lastTime);

			__time32_t curTime;
			_time32(&curTime);
			_localtime32(&curTime);

			INT time = lastTime - curTime ;

			if (time <=0 )
			{
				time = 0;
			}
			SI.CurrentCoolDown = time * Millisecond;
			break;
		}
	}
}

INT ShopManager::GetCurShopSellType()
{
	INT shopSellType = -1;

	if (_curShop && _curShop->_ShowItemList.Num()>0)
	{
		ServerShopItemData& SI = _curShop->_ShowItemList(0);
		shopSellType = SI.ShopTypeSell;
	}
	return shopSellType;
}
