#include "stdafx.h"
#include "MMOAuctionManager.h"
#include "MMOFacade.h"
#include "MMOGameClasses.h"
#include "CommonGameEvents.h"
#include "net/message_handler.h"
#include "UITextParser.h"

#define  PAYTIME 86400

bool MMOAuctionManager::CompareOrderTime(const ClientAuctionItemInfo& leftData, const ClientAuctionItemInfo& rightData)
{
	return leftData.dwFinishTime > rightData.dwFinishTime;
}

void AuctionSearchFilter::Reset()
{
	MainType = EATM_ALL;
	SubType = EATS_WW_ALL;
	SearchName = TEXT("");
	PlayerClass = INVALID_PROFESSION;
	LevelMin = 0;
	LevelMax = MAX_ROLE_LEVEL;
	ItemColor = ITEMCOLOR_INVALID;
	AuctionState = easInvalid;
	//SearchPageNumb = 0;
	//bUnitPrice = FALSE;
	//SortType = esbTotal;
	SortAscendent = 0;
	FindIDList.Empty();
	searchType = EAST_ItemType;
}

AuctionSearchFilter::AuctionSearchFilter()
{
	Reset();
	SortType = esbTotalPrice;
}


UGFxObject* AuctionSearchFilter::ToGfx( UGFxMoviePlayer* gfxPlayer ) const
{
	UGFxObject* obj = gfxPlayer->CreateObject(TEXT("UI_AuctionHouse.AS.SearchFilterData"));
	if(obj)
	{
		obj->SetString(TEXT("item_name"), SearchName);
		obj->SetInt(TEXT("player_class_idx"), PlayerClass);
		obj->SetInt(TEXT("lv_min"), LevelMin);
		obj->SetInt(TEXT("lv_max"), LevelMax);
		obj->SetInt(TEXT("item_color_idx"), ItemColor);
		obj->SetInt(TEXT("auction_status"), AuctionState);
	}

	return obj;
}

void AuctionSearchFilter::FromGfx( UGFxObject* obj )
{
	SearchName = obj->GetString(TEXT("item_name"));
	PlayerClass = obj->GetInt(TEXT("player_class_idx"));
	LevelMin = obj->GetInt(TEXT("lv_min"));
	LevelMax = obj->GetInt(TEXT("lv_max"));
	ItemColor = obj->GetInt(TEXT("item_color_idx"));
	AuctionState = (EAuctionStatus)obj->GetInt(TEXT("auction_status"));
	
	//FindIDList.Empty();

	//if(!SearchName.IsEmpty())
	//{
	//	const ItemTemplateMap& itemMap = MMOFacade::getSingleton().GetItemTemplateMap();
	//	for (ItemTemplateMap::const_iterator i = itemMap.begin(); i != itemMap.end(); ++i)
	//	{
	//		/*if(FindIDList.Num() >= AUCTION_SEARCH_REQUEST_MAX_ITEM_ID_COUNT)
	//		{
	//			break;
	//		}*/

	//		if(INDEX_NONE != i->second->ItemName.InStr(SearchName, FALSE, TRUE))
	//		{
	//			FindIDList.AddItem(i->first);
	//		}
	//	}
	//}
	//else
	//{
	//	MMOAuctionManager::getSingleton().SetFilterCategory( EATM_ALL , EATS_WW_ALL );
	//}
}

MMOAuctionManager::MMOAuctionManager()
	: _currentTabPage(EATID_Invalid)
	, _currentPageNum(0)
	, _maxPageNumb(1)
	, _peakMaxPage(2)
	, _lastActionIsNextPage(true)
	, _searchItemId(0)
{
}

MMOAuctionManager::~MMOAuctionManager()
{
}

void MMOAuctionManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_LeaveSelectingRole:
		{
			Reset();

			//LoadFailedList();
		}
		break;

	case GameEvent_ChangeRole:
		{
			//SaveFailedList();
		}
		break;

	case GameEvent_PlayerBaseInfo:
		{
			MMOGameEvent_PlayerBaseInfo* info = (MMOGameEvent_PlayerBaseInfo*)evt;
// 			PlayerNameMap::iterator it = _playerNameMap.find(info->_RoldID);
// 			if(it == _playerNameMap.end())
// 			{
// 				_playerNameMap.insert(std::make_pair(info->_RoldID, info->_PlayerBaseInfo->_RoleName));
// 			}
// 			else
// 			{
// 				it->second = info->_PlayerBaseInfo->_RoleName;
// 			}

			MMOGameEvent_Auction_UpdatePlayerName evt(info->_RoldID, info->_PlayerBaseInfo->_RoleName);
			MMOUI::PostGameEvent(&evt);
		}
		break;
	}
}

void MMOAuctionManager::Init()
{
	AuctionCategoryList::getSingleton().InitFromFile(TEXT("AuctionCategory.txt"), true);

	MMOUI::RegisterEventHandler(this, GameEvent_ChangeRole);
	MMOUI::RegisterEventHandler(this, GameEvent_LeaveSelectingRole);
	MMOUI::RegisterEventHandler(this, GameEvent_PlayerBaseInfo);

	for(INT i = 0; i < AUCTION_SELL_CONFIG; i++)
	{
		AuctionSellConfig.Push(0);
	}

	GConfig->GetInt(TEXT("Auction"),TEXT("AuctionSellTime1"),AuctionSellConfig(0),GDefaultGameIni);
	GConfig->GetInt(TEXT("Auction"),TEXT("AuctionSellTime2"),AuctionSellConfig(1),GDefaultGameIni);
	GConfig->GetInt(TEXT("Auction"),TEXT("AuctionSellTime3"),AuctionSellConfig(2),GDefaultGameIni);

	GConfig->GetInt(TEXT("Auction"),TEXT("AuctionSellMoney1"),AuctionSellConfig(3),GDefaultGameIni);
	GConfig->GetInt(TEXT("Auction"),TEXT("AuctionSellMoney2"),AuctionSellConfig(4),GDefaultGameIni);
	GConfig->GetInt(TEXT("Auction"),TEXT("AuctionSellMoney3"),AuctionSellConfig(5),GDefaultGameIni);

	_auctionItemVec.clear();
	bShowByLink = FALSE;
	linkItemId = 0;
	linkItemIndex = 0;
	bShowLinkItem = FALSE;
	bUpdateCurrentPage = FALSE;
}


void MMOAuctionManager::RequistInitUIData()
{

}


void MMOAuctionManager::Shutdown()
{
	//SaveFailedList();

	MMOUI::UnRegisterEventHandler(this, GameEvent_ChangeRole);
	MMOUI::UnRegisterEventHandler(this, GameEvent_LeaveSelectingRole);
	MMOUI::UnRegisterEventHandler(this, GameEvent_PlayerBaseInfo);
}

void MMOAuctionManager::Reset()
{
	UpdateSearchFilter(0);

	_auctionViewItemList.clear();
	_auctionMySellingItemList.clear();
	_auctionMyBuyingItemList.clear();
	_auctionMyFailedAuctionItemList.clear();
	_auctionMyBoughtItemList.clear();

	ResetPageNumber();
	//_playerNameMap.clear();

	NotifySearchResult();
	NotifyFailedList();
	NotifyMySellingList();
	NotifyMyBuyingList();

	MMOGameEvent_Any resetAuctionSellEvt(GameEvent_Auction_Reset);
	MMOUI::PostGameEvent(&resetAuctionSellEvt);

	bShowByLink = FALSE;
	linkItemId = 0;
	linkItemIndex = 0;
	bShowLinkItem = FALSE;
	bUpdateCurrentPage = FALSE;
}

void MMOAuctionManager::UpdateSearchFilter(UGFxObject* dt)
{
	if (dt)
	{
		_searchFilter.FromGfx(dt);
	}
	else
	{
		_searchFilter.Reset();
		ResetPageNumber();
	}
}

void MMOAuctionManager::Send_SearchByFilter()
{
	//if (_searchFilter.MainType ==EATM_ALL && _searchFilter.SubType ==EATS_WW_ALL)
		//return;
	_lastActionIsNextPage = true;
	bUpdateCurrentPage = FALSE;

	switch(_searchFilter.searchType)
	{
	case EAST_ItemId:
		{
			C2M_AUCTION_SEARCH* as = (C2M_AUCTION_SEARCH*)CMessageHandler::ms_MsgBuffer;
			as->bySortBy = _searchFilter.SortType;
			as->nItemTemplateID = _searchItemId;
			as->byAscending = (UINT8)_searchFilter.SortAscendent;
			CMessageHandler::GetInstance().Send_AuctionSearch(as);
		}
		break;
	case EAST_ItemType:
		{
			CMessageHandler::GetInstance().Send_AuctionQueryByType(_searchFilter.MainType,_searchFilter.SubType,_searchFilter.SortType,(UINT8)_searchFilter.SortAscendent);
		}
		break;
	}
	
	
	
	//as->bySortBy = _searchFilter.SortType;
	//as->bySortAscendent = _searchFilter.SortAscendent;
	//as->byPage = std::max(0, _currentPageNum);
	//
	//as->byAuctionType = _searchFilter.MainType;
	//as->byAuctionSubType = _searchFilter.SubType;
	//
	//as->byItemColor = _searchFilter.ItemColor == 0 ? 0 : (_searchFilter.ItemColor - 1);
	//
	//as->byMinRequireLevel = _searchFilter.LevelMin;
	//as->byMaxRequireLevel = _searchFilter.LevelMax;
	//
	//as->byRequireClass = _searchFilter.PlayerClass == 0 ? -1 : (1 << (_searchFilter.PlayerClass - 1)); // -1 means all classes
	//as->byAuctionStatus = _searchFilter.AuctionState;

	//as->byItemIDCount = _searchFilter.FindIDList.Num();
	//memcpy(&as->anItemTemplateID[0], _searchFilter.FindIDList.GetData(), sizeof(INT) * as->byItemIDCount);
	
	//as->bySortBy = _searchFilter.SortType;
	//as->nItemTemplateID = _searchItemId;
	//CMessageHandler::GetInstance().Send_AuctionSearch(as);
}

void MMOAuctionManager::UpdateSearchSortBy( INT colIdx, UBOOL decending )
{
	_searchFilter.SortType = ConvertColIndexToServerSortBy(colIdx);
	_searchFilter.SortAscendent = !decending;
}


void MMOAuctionManager::ConvertServerAuctionItemList( AuctionItemList& arr, UINT8 count, AUCTION_ITEM_INFO* serverItemList )
{
	arr.clear();

	UINT32 timeLeft = MMOAuctionManager::getSingleton().GetClientRealTime();
	size_t offset = 0;
	for (INT i = 0; i < count; ++i)
	{
		ClientAuctionItemInfo itemInfo;
		AUCTION_ITEM_INFO* auctionItem = (AUCTION_ITEM_INFO*)(((BYTE*)serverItemList) + offset);

		offset += itemInfo.ConvertFromServer(auctionItem);

		UItem* itemTemp = itemInfo.ItemTemplate;

		if (!itemTemp)
		{
			warnf(NAME_Error, TEXT("Auction item template not found:%d"), itemInfo.m_nTemplateID);
			continue;
		}

		/*__time32_t localTime = auctionItem->dwFinishTime;
		struct tm *date = _localtime32(&localTime); 
		FString s;
		if (date)
		{
			s = FString::Printf(TEXT("%d/%d/%d %d:%d"), date->tm_year + 1900, date->tm_mon + 1, date->tm_mday, date->tm_hour,date->tm_min);
		}*/

		if (MMOAuctionManager::getSingleton().GetCurrentTabPage() == EATID_View )
		{
			if (auctionItem->byAuctionStatus != easEffective)
			{
				continue;
			}
		}

		/*if (timeLeft >= auctionItem->dwFinishTime)
		{
			continue;
		}*/

		arr.push_back(itemInfo);
	}
}


void MMOAuctionManager::OnServer_SearchResult(UINT8 count, AUCTION_ITEM_INFO* serverItemList)
{
	if(_lastActionIsNextPage && ((count == AUCTION_SEARCH_RESULT_ITEMS_PER_PAGE)
				&& (_maxPageNumb < _peakMaxPage)))
	{
		++_maxPageNumb;
	}

	ConvertServerAuctionItemList(_auctionViewItemList, count, serverItemList);

	NotifySearchResult();
	NotifyUpdatePageInfo();

	if (bShowLinkItem)
	{
		NotifyShowLinkItem();
		
	}
}
 
void MMOAuctionManager::OnServer_QueryAuctionItem(M2C_AUCTION_QUERY* msg)
{
	/*if(msg->byFound)
	{
		ClientAuctionItemInfo info;
		info.ConvertFromServer(&msg->stAuction);
		if (!info.ItemTemplate)
		{
			warnf(NAME_Error, TEXT("OnServer_QueryAuctionItemAuction item template not found:%d"), info.m_nTemplateID);
			return;
		}
		
		TryAddToList(_auctionMyFailedAuctionItemList, info);
		NotifyFailedList();
	}
	else
	{
		if(TryRemoveFromList(_auctionMyFailedAuctionItemList, msg->dwAuctionID))
		{
			NotifyFailedList();
		}
	}


	TryRemoveFromList(_auctionMyFailedAuctionItemIDList, msg->dwAuctionID);*/

	if (msg)
	{
		currentQueryAuctionInfo.dwAuctionID = msg->dwAuctionID;
		currentQueryAuctionInfo.szGuildName = UTF8_TO_TCHAR(msg->szGuildName);
		currentQueryAuctionInfo.szSellerName = UTF8_TO_TCHAR(msg->szSellerName);
		currentQueryAuctionInfo.bySellerLevel = msg->bySellerLevel;
		currentQueryAuctionInfo.dwBuyerCount = msg->dwBuyerCount;
		currentQueryAuctionInfo.bySellerClass = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"),msg->bySellerClass));

		AUCTION_ITEM_INFO* info = &msg->stAuction; 
		currentQueryAuctionInfo.info.ConvertFromServer(info);
		NotifyUpdateQueryItem();
	}

	
	
}


void MMOAuctionManager:: NotifyUpdateQueryItem()
{
	MMOGameEvent_Any evt( GameEvent_Auction_UpdateQueryItem );
	MMOUI::PostGameEvent(&evt);
}

void MMOAuctionManager::OnServer_ItemSell(AUCTION_ITEM_INFO* item)
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_ItemSell);

	ClientAuctionItemInfo info;
	info.ConvertFromServer(item);
	if (!info.ItemTemplate)
	{
		warnf(NAME_Error, TEXT("OnServer_ItemSellAuction item template not found:%d"), info.m_nTemplateID);
		return;
	}

	_auctionMySellingItemList.push_back(info);
	evt.AuctionItemListPtr = &_auctionMySellingItemList;
	MMOUI::PostGameEvent(&evt);

	FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("Auction.Sell.OK"));
	MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,error);

}

void MMOAuctionManager::OnServer_ItemCancelSell(UINT32 id)
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_ItemCancelSell);
	
	AuctionItemList::iterator it = _auctionMySellingItemList.begin();
	for (; it != _auctionMySellingItemList.end(); ++it)
	{
		if ((*it).AuctionId == id)
		{
			_auctionMySellingItemList.erase(it);
			break;
		}
	}

	evt.AuctionItemListPtr = &_auctionMySellingItemList;
	MMOUI::PostGameEvent(&evt);

	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( TEXT("Auction.Sell.Cancel") ),MsgInfoType_Table,TEXT("Auction.Sell.Cancel") ); 

}

void MMOAuctionManager::OnServer_ItemBuy(AUCTION_ITEM_INFO* item)
{
	ClientAuctionItemInfo info;
	info.ConvertFromServer(item);
	if (!info.ItemTemplate)
	{
		warnf(NAME_Error, TEXT("OnServer_ItemBuyAuction item template not found:%d"), info.m_nTemplateID);
		return;
	}

	MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("Auction.Buy.Succeeded")) , MsgInfoType_Table, TEXT("Auction.Buy.Succeeded"));

	// After buying, it must be removed from the failed table
	if(TryRemoveFromList(_auctionMyFailedAuctionItemList, item->dwAuctionID))
	{
		NotifyFailedList();
	}

	// Add to BuyingList
	//TryAddToList(_auctionMyBuyingItemList, info);
	

	if(item->byAuctionStatus == easWaitForPayment)
	{
		TryRemoveFromList(_auctionViewItemList, item->dwAuctionID);
		TryRemoveFromList(_auctionMyBuyingItemList, item->dwAuctionID);
	}
	else
	{
		TryAddToList(_auctionViewItemList, info);
		TryAddToList(_auctionMyBuyingItemList, info);
	}

	NotifyMyBuyingList();
	NotifySearchResult(bUpdateCurrentPage);

// 	TryAddToList(_auctionMyFailedAuctionItemList, info);
// 	NotifyFailedList();

// 	TryRemoveFromList(_auctionViewItemList, info);
// 	NotifySearchResult();
}

void MMOAuctionManager::OnServer_AuctionFinish( M2C_AUCTION_FINISHED* msg )
{
	//const ClientAuctionItemInfo* info = TryFindLocal(msg->dwAuctionID);
	
	TryRemoveFromList(_auctionViewItemList, msg->dwAuctionID);
	NotifySearchResult();

	//If it is an item consigned by oneself, skip the operations below
	if (Check_IsMySellingItem(msg->dwAuctionID))
	{
		return;
	}

	// Bid succeeded, remove from Failed/Buying lists
	if(msg->bySuccess)
	{
		if(TryRemoveFromList(_auctionMyFailedAuctionItemList, msg->dwAuctionID))
		{
			NotifyFailedList();
		}

		if(TryRemoveFromList(_auctionMyBuyingItemList, msg->dwAuctionID))
		{
			NotifyMyBuyingList();
		}

// 		MMOUI::GetUIManager()->delegateAddScreenMessage(
// 			MMOUI::ParseLocalizedPropertyPath(TEXT("Auction.Finish.Succeeded")));
	}
	else // Bid failed, add to Failed list and remove from Buying
	{
		//Check whether it is an item consigned by oneself
		if (Check_IsMySellingItem(msg->dwAuctionID) == FALSE)
		{
// 			MMOUI::GetUIManager()->delegateAddScreenMessage(
// 				MMOUI::ParseLocalizedPropertyPath(TEXT("Auction.Finish.Failed")));

			const ClientAuctionItemInfo* info = TryFindLocal(msg->dwAuctionID);
			if(info)
			{
				TryAddToList(_auctionMyFailedAuctionItemList, *info);
				NotifyFailedList();
			}
		}

		if(TryRemoveFromList(_auctionMyBuyingItemList, msg->dwAuctionID))
		{
			NotifyMyBuyingList();
		}

		//Bid failed, refresh the current page
		NotifyUpdateCurrentPageInfo();

		// Query the server again to make sure the state is correct
		//CMessageHandler::GetInstance().Send_AuctionQuery(msg->dwAuctionID);
	}
}

void MMOAuctionManager::NotifyUpdateCurrentPageInfo()
{
	switch(MMOAuctionManager::getSingleton().GetCurrentTabPage())
	{
	case EATID_View:
		//NotifySearchResult();
		MMOAuctionManager::getSingleton().Send_SearchByFilter();
		break;
	case EATID_Buy:
		//NotifyMyBuyingList();
		//NotifyFailedList();
		CMessageHandler::GetInstance().Send_AuctionListMyBuying();
		//QueryFailedItems();
		break;
	}
}

BOOL MMOAuctionManager::Check_IsMySellingItem(INT itemID)
{
	AuctionItemList::iterator itor = _auctionMySellingItemList.begin();
	while(itor != _auctionMySellingItemList.end())
	{
		if ((*itor).dwAuctionID == itemID)
		{
			return TRUE;
		}
		++itor;
	}
	return FALSE;
}
void MMOAuctionManager::OnServer_ListMySelling(UINT8 count, AUCTION_ITEM_INFO* item_lst)
{
	_auctionMySellingItemList.clear();

	UINT32 timeLeft = time(0);
	size_t offset = 0;
	for (INT i = 0; i < count; ++i)
	{
		ClientAuctionItemInfo itemInfo;
		AUCTION_ITEM_INFO* auctionItem = (AUCTION_ITEM_INFO*)(((BYTE*)item_lst) + offset);

		offset += itemInfo.ConvertFromServer(auctionItem);

		UItem* itemTemp = itemInfo.ItemTemplate;

		if (!itemTemp)
		{
			warnf(NAME_Error, TEXT("Auction item template not found:%d"), itemInfo.m_nTemplateID);
			continue;
		}

		/*if (timeLeft >= auctionItem->dwFinishTime)
		{
			continue;
		}*/
		
		if (_currentTabPage == EATID_Order)
		{
			if (itemInfo.byAuctionStatus == easWaitForPayment || itemInfo.byAuctionStatus == easFinished || itemInfo.byAuctionStatus == easPaymentExpired)
			{
				_auctionMySellingItemList.push_back(itemInfo);
			}
		}
		else
		{
			if (itemInfo.byAuctionStatus == easEffective)
			{
				_auctionMySellingItemList.push_back(itemInfo);
			}
			
		}
		
	}

	//ConvertServerAuctionItemList(_auctionMySellingItemList, count, item_lst);

	NotifyMySellingList();

}

void MMOAuctionManager::OnServer_ListMyBuying(UINT8 count, AUCTION_ITEM_INFO* item_lst)
{
	if (_currentTabPage == EATID_Order)
	{
		_auctionMyBoughtItemList.clear();
	}
	else if(_currentTabPage == EATID_Buy)
	{
		_auctionMyBuyingItemList.clear();
		_auctionMyFailedAuctionItemList.clear();
	}
	

	UINT32 timeLeft = time(0);
	size_t offset = 0;
	AMMOPlayer* self = MMOFacade::getSingleton().GetMainPlayer();

	if (self == NULL)
	{
		return;
	}

	for (INT i = 0; i < count; ++i)
	{
		ClientAuctionItemInfo itemInfo;
		AUCTION_ITEM_INFO* auctionItem = (AUCTION_ITEM_INFO*)(((BYTE*)item_lst) + offset);

		offset += itemInfo.ConvertFromServer(auctionItem);

		UItem* itemTemp = itemInfo.ItemTemplate;

		if (!itemTemp)
		{
			warnf(NAME_Error, TEXT("Auction item template not found:%d"), itemInfo.m_nTemplateID);
			continue;
		}

		/*if (timeLeft >= auctionItem->dwFinishTime)
		{
			continue;
		}*/

		if (_currentTabPage == EATID_Order)
		{
			if (itemInfo.byAuctionStatus != easEffective && itemInfo.byAuctionStatus !=easExpired)
			{
				_auctionMyBoughtItemList.push_back(itemInfo);
			}
		}
		else if(_currentTabPage == EATID_Buy)
		{
			if (itemInfo.byAuctionStatus == easEffective)
			{
				if (self->ServerID == auctionItem->dwBuyerRoleID)
				{
					_auctionMyBuyingItemList.push_back(itemInfo);
				}
				else 
				{
					_auctionMyFailedAuctionItemList.push_back(itemInfo);
				}
			}
			
		}
		
	}

	if (_currentTabPage == EATID_Order)
	{
		//Sort
		_auctionMyBoughtItemList.sort(CompareOrderTime);
	}

	NotifyUpdateMyBuyingList();
	//ConvertServerAuctionItemList(_auctionMyBuyingItemList, count, item_lst);

	/*for (INT i = 0; i < _auctionMyBuyingItemList.Num(); ++i)
	{
	TryRemoveFromList(_auctionMyFailedAuctionItemIDList,
	_auctionMyBuyingItemList(i).dwAuctionID);
	}*/

}

void MMOAuctionManager::NotifyFailedList()
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_ListFailed);
	evt.AuctionItemListPtr = &_auctionMyFailedAuctionItemList;
	MMOUI::PostGameEvent(&evt);
}

INT MMOAuctionManager::GetMoneyRatio(INT64 money)
{
	INT ratio = 0;
	if (money > 0)
	{
		ratio = money / MONEY_RATIO;
	}
	return ratio;
}


UGFxObject* MMOAuctionManager::CreateAuctionItemListDataObject( UMMOGFXPlayer* gfxPlayer, const AuctionItemList& itemList )
{
	//TArray<INT> needQueryPlayerList;
	UGFxObject* datList = gfxPlayer->CreateArray();
	//check(datList);
	if(datList == NULL)
		return NULL;

	UINT32 timeLeft = MMOAuctionManager::getSingleton().GetClientRealTime();

	INT idx = 0;
	INT i = 0;
	AuctionItemList::const_iterator itor = itemList.begin();
	for (i; itor != itemList.end(); ++itor,++i)
	{		
		const ClientAuctionItemInfo& itemInfo = *itor;
		
		/*if (timeLeft >= itemInfo.dwFinishTime)
		{
		continue;
		}*/

		UGFxObject*  dat = gfxPlayer->CreateObject(TEXT("UI_AuctionHouse.AS.AuctionItemData"));
		//check(dat);
		if(dat == NULL)
			continue;

		UGFxObject* slotDat = gfxPlayer->CreateObject(TEXT("ActionScript3.Slot.ItemSlotData"));
		//check(slotDat);		
		if(slotDat == NULL)
			continue;

		slotDat->SetString(TEXT("SlotRenderClassName"),  TEXT("NormalItemSlotRender"));
		slotDat->SetInt( TEXT("slotType"), ESPType_AuctionItem );
		slotDat->SetInt( TEXT("slotIndex"), i );
		slotDat->SetBool( TEXT("HaveData"), TRUE );

		slotDat->SetInt(TEXT("quantity"), itemInfo.GetCurStackCount(itemInfo.ItemTemplate)/*(itemInfo.ItemTemplate->ItemDuration == E_DURTYPE_STACK ? itemInfo.m_nCurDur : 1)*/);
		slotDat->SetInt(TEXT("iconIndex"), -1);		
		slotDat->SetInt(TEXT("itemColor"), itemInfo.ItemTemplate->ItemColor);

		FString cs = FString(TEXT("img://")) + MMOFacade::getSingleton().GetItemIconNamePersonalized( itemInfo.ItemTemplate );
		slotDat->SetString(TEXT("iconName"), cs);

		slotDat->SetInt(TEXT("containerID"), -1);
		slotDat->SetInt(TEXT("slotID"), itemInfo.dwAuctionID);
		slotDat->SetInt(TEXT("itemTemplateID"), itemInfo.m_nTemplateID);

		dat->SetObject(TEXT("ItemData"), slotDat);

		dat->SetInt(TEXT("AuctionID"), itemInfo.dwAuctionID);
		dat->SetString(TEXT("ItemName"), UITextParser::HTMLFormatItemColorString(itemInfo.ItemTemplate));
		dat->SetInt(TEXT("ItemLevel"), itemInfo.ItemTemplate->RequireLevel);
		dat->SetInt(TEXT("SellerID"), itemInfo.dwSellerRoleID);
		dat->SetInt(TEXT("BuyerID"), itemInfo.dwBuyerRoleID);		


		if (MMOAuctionManager::getSingleton().GetLinkItemShow() && itemInfo.dwAuctionID == MMOAuctionManager::getSingleton().GetLinkItemId())
		{
			MMOAuctionManager::getSingleton().SetLinkItemIndex(idx);
		}

		PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(itemInfo.dwSellerRoleID);
		if(playerInfo != NULL)
		{
			dat->SetString(TEXT("Seller"), playerInfo->_RoleName);
			dat->SetString(TEXT("BuyerGuildName"),playerInfo->_GuildName);

			FString sProfession = MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + FString::Printf(TEXT("%d"),playerInfo->_RoleClass));
			dat->SetString(TEXT("BuyerProfession"),sProfession);
		}
		else
		{
			//needQueryPlayerList.AddItem(it->first);
			dat->SetString(TEXT("Seller"), TEXT(""));
		}

		PlayerBaseInfo* playerInfoBuyer = MMOFacade::getSingleton().RequestPlayerBaseInfo(itemInfo.dwBuyerRoleID);
		if(playerInfoBuyer != NULL)
		{
			dat->SetString(TEXT("Buyer"), playerInfoBuyer->_RoleName);
		}
		else
		{
			//needQueryPlayerList.AddItem(it->first);
			dat->SetString(TEXT("Buyer"), TEXT(""));
		}

		/*PlayerNameMap::const_iterator it = MMOAuctionManager::getSingleton()._playerNameMap.find(itemInfo.dwSellerRoleID);
		if(it != MMOAuctionManager::getSingleton()._playerNameMap.end())
		{
			dat->SetString(TEXT("Seller"), it->second);
		}
		else
		{
			needQueryPlayerList.AddItem(it->first);
			dat->SetString(TEXT("Seller"), TEXT("..."));
		}*/

		dat->SetInt(TEXT("StackCount"), itemInfo.GetCurStackCount(itemInfo.ItemTemplate)/*(itemInfo.ItemTemplate->ItemDuration == E_DURTYPE_STACK ? itemInfo.m_nCurDur : 1)*/);
		
		//dat->SetBool(TEXT("ShowUnitPrice"), _searchFilter.bUnitPrice); pure UI implementation

		INT price = MMOAuctionManager::getSingleton().GetMoneyRatio(itemInfo.nPrice);
		dat->SetInt(TEXT("CurrentAuctionPrice"), price);
		INT buyPrice = MMOAuctionManager::getSingleton().GetMoneyRatio(itemInfo.nBuyoutPrice);
		dat->SetInt(TEXT("FixedPrice"),buyPrice);

		
		INT32 timeTemp = itemInfo.dwFinishTime - timeLeft <=0 ? 0 :  itemInfo.dwFinishTime - timeLeft ;
		dat->SetInt(TEXT("TimeLeft"), timeTemp);

		/*if (MMOAuctionManager::getSingleton().GetCurrentTabPage() == EATID_View && timeTemp <=0 )
		{
			continue;
		}*/

		dat->SetInt(TEXT("ItemColor"), itemInfo.ItemTemplate->ItemColor);
		dat->SetInt(TEXT("AuctionMainType"), itemInfo.ItemTemplate->AuctionType);
		dat->SetInt(TEXT("AuctionSubType"), itemInfo.ItemTemplate->AuctionSubType);
		dat->SetInt(TEXT("ItemTemplateID"), itemInfo.ItemTemplate->GetTemplateID());

		__time32_t localTime = itemInfo.dwStartTime;
		struct tm *time = _localtime32(&localTime);
		if (time)
		{
			FString startTime = FString::Printf(TEXT("%d-%d-%d"),time->tm_year+1900,time->tm_mon+1,time->tm_mday); 
			dat->SetString(TEXT("AuctionStartTime"),startTime);
			dat->SetString(TEXT("OrderTime"),startTime);
		}
		

		INT depositPrice = MMOAuctionManager::getSingleton().GetMoneyRatio(itemInfo.nDepositPrice);
		dat->SetInt(TEXT("AuctionDepositPrice"),depositPrice);
		dat->SetString(TEXT("PayTimeLimit"),MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Auction.PayTimeLimit")));
		dat->SetInt(TEXT("AuctionPlayerCount"),0);

		//dat->SetInt(TEXT("BuyerLevel"),itemInfo.nSellerLevel);
		

		if (itemInfo.byAuctionStatus == easEffective && itemInfo.dwBuyerRoleID > 0)
		{
			dat->SetBool(TEXT("IsAuction"),TRUE);
			dat->SetBool(TEXT("IsMyAuction"),TRUE);
		}
		else
		{
			dat->SetBool(TEXT("IsAuction"),FALSE);
			dat->SetBool(TEXT("IsMyAuction"),FALSE);
		}
		
		//if (MMOAuctionManager::getSingleton().CheckItemIsAuction(itemInfo.dwAuctionID))
		//{
			//dat->SetBool(TEXT("IsMyAuction"),TRUE);
		//}
		//else
		//{
			//dat->SetBool(TEXT("IsMyAuction"),FALSE);
		//}
		dat->SetInt(TEXT("CurrentTabPage"),MMOAuctionManager::getSingleton().GetCurrentTabPage());
		//itdst->SetBool(TEXT("ShowUnitPrice"), udt->byAuctionStatus);
		dat->SetInt(TEXT("OrderState"),itemInfo.byAuctionStatus);

		if (itemInfo.byAuctionStatus == easWaitForPayment)
		{
			UINT32 payTimeLeft = PAYTIME - (timeLeft - itemInfo.dwFinishTime);
			dat->SetInt(TEXT("PayTimeLeft"),payTimeLeft);

		}

		
		AMMOPlayer *player = MMOFacade::getSingleton().GetMainPlayer();
		if (player)
		{
			//Whether the item is sold by oneself
			if (player->ServerID == itemInfo.dwSellerRoleID)
			{
				dat->SetBool("IsMyOwnItem",TRUE);
			}
			else
			{
				dat->SetBool("IsMyOwnItem",FALSE);
			}

			//Whether the item is one the player is bidding on
			if (player->ServerID == itemInfo.dwBuyerRoleID)
			{
				dat->SetBool("IsMyBiddingItem",TRUE);
			}
			else
			{
				dat->SetBool("IsMyBiddingItem",FALSE);
			}
		}
		datList->SetElementObject(idx, dat);
		++idx;
	}

	return datList;
}

UGFxObject* MMOAuctionManager::CreateAuctionItemCategoryListDataObject( UMMOGFXPlayer* gfxPlayer )
{
	UGFxObject* datList = gfxPlayer->CreateArray();
	//check(datList);
	if(datList == NULL)
		return NULL;

	const INT C_TypeLv1 = 0;
	const INT C_TypeLv2 = 1;
	//const INT C_TypeLv3 = 2;

	const INT C_Type_Leaf = 0;
	const INT C_Type_Branch_Closed = 1;
	const INT C_Type_Branch_Opened = 2;

	INT idx = 0;
	const AuctionCategoryList::DataTypeMap& dataMap = AuctionCategoryList::getSingleton().GetDataMap();
	AuctionCategoryList::DataTypeMap::const_iterator it = dataMap.begin();
	for (; it != dataMap.end(); ++it)
	{
		const FAuctionCategoryItem* item = it->second;
		if(item->MainType.Val == 0)
		{
			//continue; // skip all type
		}

		UGFxObject*  dat = gfxPlayer->CreateObject(TEXT("UI_AuctionHouse.AS.AuctionItemCategoryData"));
		//check(dat);
		if(dat == NULL)
			continue;

		dat->SetInt(TEXT("categoryType"), C_TypeLv1);
		dat->SetBool(TEXT("isSelected"), FALSE);
		dat->SetInt(TEXT("mainTypeValue"), item->MainType.Val);
		dat->SetInt(TEXT("subTypeValue"), 0);

		dat->SetString(TEXT("label"), item->MainType.StringVal);
		dat->SetInt(TEXT("index"), idx);
		dat->SetInt(TEXT("type"), C_Type_Branch_Closed);
		dat->SetInt(TEXT("deepth"), 0);
		dat->SetBool(TEXT("visible"), TRUE);
		
		datList->SetElementObject(idx, dat);
		++idx;

		if(item->SubType.Num() > 1) // If there is only one subtype, do not show it
		{
			for (INT j = 0; j < item->SubType.Num(); ++j)
			{
				const FStringValuePair* subItem = &item->SubType(j);

				dat = gfxPlayer->CreateObject(TEXT("UI_AuctionHouse.AS.AuctionItemCategoryData"));
				//check(dat);
				if(dat == NULL)
					continue;

				dat->SetInt(TEXT("categoryType"), C_TypeLv2);
				dat->SetBool(TEXT("isSelected"), FALSE);
				dat->SetInt(TEXT("mainTypeValue"), item->MainType.Val);
				dat->SetInt(TEXT("subTypeValue"), subItem->Val);

				dat->SetString(TEXT("label"), subItem->StringVal);
				dat->SetInt(TEXT("index"), idx);
				dat->SetInt(TEXT("type"), C_Type_Leaf);
				dat->SetInt(TEXT("deepth"), 1);
				dat->SetBool(TEXT("visible"), TRUE);

				datList->SetElementObject(idx, dat);
				++idx;
			}
		}
	}	

	return datList;
}

UGFxObject* MMOAuctionManager::CreatePlayerNameDataObject( UMMOGFXPlayer* gfxPlayer, INT id, const FString& name )
{
	UGFxObject* dat = gfxPlayer->CreateObject(TEXT("UI_AuctionHouse.AS.PlayerNameData"));
	dat->SetInt(TEXT("PlayerID"), id);
	dat->SetString(TEXT("PlayerName"), name);
	return dat;
}


UINT8 MMOAuctionManager::ConvertColIndexToServerSortBy( INT idx )
{
	switch(idx)
	{
		/*	case 0: return esbColor;
		case 1: return esbRequireLevel;
		case 2: return esbFinishTime;
		case 3: return esbRequireLevel;*/
		case 4: return esbTotalPrice;
		case 5: return esbBuyoutPrice;
		case 2:return esbFinishTime;
		//case 6: return esbSinglePrice;
		//case 7: return esbBuyoutSinglePrice;
	}

	//return esbDefault;
	return esbTotal;
}

void MMOAuctionManager::SetFilterCategory( BYTE mainType, BYTE subType )
{
	_searchFilter.searchType = EAST_ItemType;
	_searchFilter.MainType = mainType;
	_searchFilter.SubType = subType;
}

void MMOAuctionManager::UpdateSearchPageNumb( INT numb )
{
	//_searchFilter.SearchPageNumb = numb;
	_currentPageNum = numb;
}

UGFxObject* MMOAuctionManager::CreateViewListItemDataObject( UMMOGFXPlayer* gfxPlayer ) const
{
	return CreateAuctionItemListDataObject(gfxPlayer, _auctionViewItemList);
}

UGFxObject* MMOAuctionManager::CreateMyBuyingListItemDataObject( UMMOGFXPlayer* gfxPlayer ) const
{
	return CreateAuctionItemListDataObject(gfxPlayer, _auctionMyBuyingItemList);
}

UGFxObject* MMOAuctionManager::CreateMyBuyingFailedListItemDataObject( UMMOGFXPlayer* gfxPlayer ) const
{
	return CreateAuctionItemListDataObject(gfxPlayer, _auctionMyBuyingItemList);
}

UGFxObject* MMOAuctionManager::CreateMySellingListItemDataObject( UMMOGFXPlayer* gfxPlayer ) const
{
	return CreateAuctionItemListDataObject(gfxPlayer, _auctionMySellingItemList);
}


void MMOAuctionManager::SetCurrentTabPage( EAuctionTabId currentTabPage )
{
	/*if(_currentTabPage == currentTabPage)
	{
	return;
	}*/

	bUpdateCurrentPage = FALSE;

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Bag , FALSE, "");

	_currentTabPage = currentTabPage;

	switch(currentTabPage)
	{
	case EATID_View:
		{
			
			MMOAuctionManager::getSingleton().SetFilterCategory(0,0);
			Send_SearchByFilter();
		}
		break;

	case EATID_Buy:
		{
			
			CMessageHandler::GetInstance().Send_AuctionListMyBuying();
			//QueryFailedItems();
		}
		break;

	case EATID_Sell:
		{
			CMessageHandler::GetInstance().Send_AuctionListMySelling();
			MMOGameEvent_Any evt(GameEvent_Auction_ShowBag);
			MMOUI::PostGameEvent(&evt);
		}
		break;
	case EATID_Order:
		{
			CMessageHandler::GetInstance().Send_AuctionListMyBuying();
		}
		break;
	}
}

bool MMOAuctionManager::Contain( const AuctionItemList&lst, INT auctionID )
{
	AuctionItemList::const_iterator it = lst.begin();
	for (; it != lst.end(); ++it)
	{
		if((*it).dwAuctionID == auctionID)
		{			
			return true;
		}
	}

	return false;
}


bool MMOAuctionManager::TryRemoveFromList( AuctionItemList&lst, INT auctionID )
{
	AuctionItemList::iterator it = lst.begin();
	for (; it != lst.end(); ++it)
	{
		if((*it).dwAuctionID == auctionID)
		{
			lst.erase(it);
			return true;
		}
	}

	return false;
}

bool MMOAuctionManager::TryRemoveFromList( TArray<INT>&lst, INT auctionID )
{
	for (INT i = 0; i < lst.Num(); ++i)
	{
		if(lst(i) == auctionID)
		{
			lst.Remove(i);
			return true;
		}
	}

	return false;
}

void MMOAuctionManager::TryAddToList( AuctionItemList&lst, const ClientAuctionItemInfo& info )
{
	AuctionItemList::iterator it = lst.begin();
	for (; it != lst.end(); ++it)
	{
		if((*it).dwAuctionID == info.dwAuctionID)
		{
			(*it) = info;
			return;
		}
	}

	lst.push_back(info);
}


TArray<INT> MMOAuctionManager::GetFailedAuctionList() const
{
	TArray<INT> lst;

	AuctionItemList::const_iterator it = _auctionMyFailedAuctionItemList.begin();
	for (; it != _auctionMyFailedAuctionItemList.end(); ++it)
	{
		lst.AddItem((*it).dwAuctionID);
	}
	return lst;
}

void MMOAuctionManager::SetFailedAuctionList( const TArray<INT>& lst )
{
	for (INT i = 0; i < lst.Num(); ++i)
	{
		CMessageHandler::GetInstance().Send_AuctionQuery(lst(i));
	}
}

void MMOAuctionManager::NotifySearchResult(BOOL update)
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_SearchResult);
	evt.AuctionItemListPtr = &_auctionViewItemList;
	evt.bUpdateCurrentPage = update;
	MMOUI::PostGameEvent(&evt);
}

void MMOAuctionManager::BuyFromViewList( INT auctionID, INT price )
{
	const ClientAuctionItemInfo* info = FindAuctionItem(_auctionViewItemList, auctionID);
	if(info)
	{
		Buy(*info, price);
	}
}

void MMOAuctionManager::BuyFromBuyingList( INT auctionID, INT price )
{
	const ClientAuctionItemInfo* info = FindAuctionItem(_auctionMyBuyingItemList, auctionID);
	if(info)
	{
		Buy(*info, price);
	}
}

void MMOAuctionManager::BuyFromFaildAuctionList( INT auctionID, INT price )
{
	const ClientAuctionItemInfo* info = FindAuctionItem(_auctionMyFailedAuctionItemList, auctionID);
	if(info)
	{
		Buy(*info, price);
	}
}

void MMOAuctionManager::Buy( const ClientAuctionItemInfo& info, INT price )
{
	//// If greater than the buyout price, use the buyout price
	//INT realPrice = info.GetNextPrice();
	//
	//// Can only be greater than the current price
	//if(price > info.nPrice && price < realPrice)
	//{
	//	realPrice = price;
	//}
	INT64 realPrice = GetRealAuctionMoney(price);
	CMessageHandler::GetInstance().Send_AuctionBuy(info.dwAuctionID, realPrice);
}

BOOL MMOAuctionManager::IsShowSmg_DefaultPrice(INT dwAuctionID,INT price,Auction_ScreenMsg_Type type)
{
	const ClientAuctionItemInfo* info;
	switch (type)
	{
	case AS_Type_ViewPage:
		info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionViewItemList, dwAuctionID));
		return Check_DefaultAuctionPrice(*info,price);
		break;
	case AS_Type_BuyPage:
		info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionMyBuyingItemList, dwAuctionID));
		return Check_DefaultAuctionPrice(*info,price);
		break;
	case  AS_Type_BuyFailedPage:
		info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionMyFailedAuctionItemList, dwAuctionID));
		return Check_DefaultAuctionPrice(*info,price);
		break;
	}

	return TRUE;

}
BOOL MMOAuctionManager::Check_DefaultAuctionPrice(const ClientAuctionItemInfo& info,INT price)
{
	if (&info)
	{
		if (info.byAuctionStatus == easEffective && info.dwBuyerRoleID > 0)
		{
			INT64 nPrice = GetRealAuctionMoney(price);

			if (nPrice < info.GetNextPrice())
			{
				//Add a scrolling text prompt
				FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAuctionBuyingPriceLow"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAuctionBuyingPriceLow"));
				return TRUE;
			}
		}
	}
	
	return TRUE;
}

bool MMOAuctionManager::CheckItemIsAuction(UINT32 itemId)
{
	const ClientAuctionItemInfo* info;

	//First look in the outbid items
	info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionMyFailedAuctionItemList, itemId));
	if (info)
	{
		//In the outbid list, which means you are not the highest bidder
		return FALSE;
	}

	//Look in the bidding list
	info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionMyBuyingItemList, itemId));
	if (info)
	{
		//In the bidding list, which means you are the highest bidder
		return TRUE;
	}
	return FALSE;
	
}

const ClientAuctionItemInfo* MMOAuctionManager::FindAuctionItem( const AuctionItemList&lst, INT auctionID )
{
	AuctionItemList::const_iterator it = lst.begin();
	for (; it != lst.end(); ++it)
	{
		if((*it).dwAuctionID == auctionID)
		{
			return &(*it);
		}
	}

	return NULL;
}

const ClientAuctionItemInfo* MMOAuctionManager::TryFindLocal( INT auctionID ) const
{
	{
		const ClientAuctionItemInfo* info = FindAuctionItem(_auctionViewItemList, auctionID);
		if(info)
		{
			return info;
		}
	}

	{
		const ClientAuctionItemInfo* info = FindAuctionItem(_auctionMyBuyingItemList, auctionID);
		if(info)
		{
			return info;
		}
	}

	{
		const ClientAuctionItemInfo* info = FindAuctionItem(_auctionMyFailedAuctionItemList, auctionID);
		if(info)
		{
			return info;
		}
	}

	{
		const ClientAuctionItemInfo* info = FindAuctionItem(_auctionMySellingItemList, auctionID);
		if(info)
		{
			return info;
		}
	}

	{
		const ClientAuctionItemInfo* info = FindAuctionItem(_auctionMyBoughtItemList, auctionID);
		if(info)
		{
			return info;
		}
	}

	return NULL;
}


void MMOAuctionManager::NotifyMySellingList()
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_ListMySelling);
	evt.AuctionItemListPtr = &_auctionMySellingItemList;
	MMOUI::PostGameEvent(&evt);
}

void MMOAuctionManager::NotifyMyBuyingList()
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_ListMyBuying);
	evt.AuctionItemListPtr = &_auctionMyBuyingItemList;
	MMOUI::PostGameEvent(&evt);
}

void MMOAuctionManager::NotifyUpdatePageInfo()
{
	/*MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_UpdatePageNumb);
	evt.AuctionItemListPtr = NULL;
	evt.CurPageNum = _currentPageNum;
	evt.MaxPageNum = _maxPageNumb;
	MMOUI::PostGameEvent(&evt);*/
}

void MMOAuctionManager::SendQueryPlayerInfo( const TArray<INT>& idList )
{
	for (INT i = 0; i < idList.Num(); ++i)
	{
		MMOFacade::getSingleton().RequestPlayerBaseInfo(idList(i), false);
	}
}

void MMOAuctionManager::QueryFailedItems()
{
	if (_auctionMyFailedAuctionItemIDList.Num()<=0 && _auctionMyFailedAuctionItemList.size() >0)
	{
		AuctionItemList::const_iterator it = _auctionMyFailedAuctionItemList.begin();
		for (; it != _auctionMyFailedAuctionItemList.end(); ++it)
		{
			CMessageHandler::GetInstance().Send_AuctionQuery((*it).dwAuctionID);
		}
	}
	else
	{
		for (INT i = 0; i < _auctionMyFailedAuctionItemIDList.Num(); ++i)
		{
			CMessageHandler::GetInstance().Send_AuctionQuery(_auctionMyFailedAuctionItemIDList(i));
		}
	}
	
}

void MMOAuctionManager::ResetPageNumber()
{
	_currentPageNum = 0;
	_maxPageNumb = 1;
	_peakMaxPage = 2;

	NotifyUpdatePageInfo();
}

void MMOAuctionManager::LastActionIsNextPage( bool val )
{
	_lastActionIsNextPage = val;

	_peakMaxPage = std::max(2, std::max(_peakMaxPage, _maxPageNumb));

	if(val)
	{
		//++_peakMaxPage;
	}
}

void MMOAuctionManager::SaveFailedList()
{
	M_LogAuction(TEXT("@@@@@@@@@ Saving auction failed ID:%d"), _auctionMyFailedAuctionItemList.size());

	_auctionMyFailedAuctionItemIDList.Empty(_auctionMyFailedAuctionItemList.size());
	_auctionMyFailedAuctionItemIDList.Add(_auctionMyFailedAuctionItemList.size());

	// First save the current failures
	INT j = 0;
	AuctionItemList::const_iterator it = _auctionMyFailedAuctionItemList.begin();
	for (j; it != _auctionMyFailedAuctionItemList.end(); ++it,++j)
	{
		_auctionMyFailedAuctionItemIDList(j) = (*it).dwAuctionID;
	}

	INT cnt = _auctionMyFailedAuctionItemIDList.Num();
	if(cnt > 0)
	{
		FArchive* ar = 0;
		if(MMOFacade::getSingleton().BeginExportAccountData(&ar, TEXT("Auction")))
		{
			ar->Serialize(&cnt, sizeof(INT));

			for (INT i = 0; i < cnt; ++i)
			{
				ar->Serialize(&_auctionMyFailedAuctionItemIDList(i), sizeof(INT));
			}

			MMOFacade::getSingleton().EndExportAccountData(ar);
		}

		_auctionMyFailedAuctionItemIDList.Empty();
	}
}

void MMOAuctionManager::LoadFailedList()
{
	// Load the previous failure list from disk
	FArchive* ar = 0;
	MMOFacade::getSingleton().BeginLoadAccountBinary(&ar, TEXT("Auction"));
	if(ar)
	{
		INT cnt = 0;
		INT id = 0;
		ar->Serialize(&cnt, sizeof(INT));

		if(cnt > 0)
		{
			_auctionMyFailedAuctionItemIDList.Empty(cnt);
			_auctionMyFailedAuctionItemIDList.Add(cnt);
		}

		for (INT i = 0; i < cnt; ++i)
		{
			ar->Serialize(&id, sizeof(INT));
			_auctionMyFailedAuctionItemIDList(i) = id;
		}
		MMOFacade::getSingleton().EndLoadAccountBinary(ar);

		M_LogAuction(TEXT("@@@@@@@@@ Loading auction failed ID:%d"), _auctionMyFailedAuctionItemIDList.Num());
	}
}

UINT32 ClientAuctionItemInfo::ConvertFromServer( AUCTION_ITEM_INFO* info )
{
	dwAuctionID = info->dwAuctionID;
	dwSellerRoleID = info->dwSellerRoleID;
	dwBuyerRoleID = info->dwBuyerRoleID;
	nPrice = info->llPrice;
	nBuyoutPrice = info->llBuyoutPrice;
	dwFinishTime = info->dwFinishTime;
	byAuctionStatus = info->byAuctionStatus;
	nDepositPrice = info->llDepositPrice;
	dwStartTime = info->dwStartTime;
	//nSellerLevel = info->nSellerLevel;
	//szSellerGuildName =  UTF8_TO_TCHAR(info->szSellerGuildName);
	UINT32 sz = Load(info->acItem);

	ItemTemplate = MMOFacade::getSingleton().FindItemTemplate(m_nTemplateID);

	return sz + sizeof(AUCTION_ITEM_INFO);
}


//------------------------------------------------------------------------------------------------
// Category
//------------------------------------------------------------------------------------------------

FAuctionCategoryItem* FAuctionCategoryListProcessor::CreateDataObject( INT templateID, bool& needAddToMap )
{
	FAuctionCategoryItem* item = AuctionCategoryList::getSingleton().FindAndModifyData(templateID);

	if(!item)
	{
		needAddToMap = true;
		item = new FAuctionCategoryItem;
	}
	else
	{
		needAddToMap = false;
	}

	return item;
}

void FAuctionCategoryListProcessor::ParseData( FAuctionCategoryItem* obj, class CTabFile* dataRow, int rowIdx )
{
	if(!obj->MainType.IsValid())
	{
		obj->MainType.StringVal = TReadDataValue_FString<AuctionCategoryList>(TEXT("MainType"), dataRow, rowIdx);
		obj->MainType.Val = TReadDataValue_Int<AuctionCategoryList>(TEXT("MainTypeValue"), dataRow, rowIdx);		
	}

	FStringValuePair subType;

	subType.StringVal = TReadDataValue_FString<AuctionCategoryList>(TEXT("SubType"), dataRow, rowIdx);		
	subType.Val = TReadDataValue_Int<AuctionCategoryList>(TEXT("SubTypeValue"), dataRow, rowIdx);

	obj->SubType.AddItem(subType);
}

void FAuctionCategoryListProcessor::PostInitData()
{

}

ItemInfo* MMOAuctionManager::GetAuctionItemInfo(INT itemId)
{
	const ItemInfo* itemInfo = static_cast<const ItemInfo*>(TryFindLocal(itemId));
	return const_cast<ItemInfo*>(itemInfo);
}

void MMOAuctionManager::AddAuctionItem(UItem* node)
{
	if (node->Auction)
	{
		AuctionItemMap::iterator itor = _auctionItemMap.find(node->AuctionType);
		if (itor != _auctionItemMap.end())
		{
			AuctionItemMap_Sub::iterator itor_sub = itor->second->find(node->AuctionSubType);
			if (itor_sub != itor->second->end())
			{
				itor_sub->second->push_back(node->GetTemplateID());
			}
			else
			{
				AuctionItemVec* vec  = new AuctionItemVec();
				vec->push_back(node->GetTemplateID());
				itor->second->insert(make_pair(node->AuctionSubType,vec));
			}
		}
		else
		{
			AuctionItemMap_Sub* itemMap = new AuctionItemMap_Sub();
			AuctionItemVec* vec  = new AuctionItemVec();
			vec->push_back(node->GetTemplateID());
			itemMap->insert(make_pair(node->AuctionSubType,vec));
			_auctionItemMap.insert(std::make_pair(node->AuctionType,itemMap));
		}
	}
	
}

void MMOAuctionManager::SearchItem()
{

	_auctionItemVec.clear();

	AuctionItemMap::iterator itor = _auctionItemMap.begin();
	while(itor != _auctionItemMap.end())
	{
		AuctionItemMap_Sub::iterator itor_sub = itor->second->begin();
		while(itor_sub != itor->second->end())
		{
			AuctionItemVec::iterator itor_vec = itor_sub->second->begin();
			while(itor_vec != itor_sub->second->end())
			{
				_auctionItemVec.push_back(*itor_vec);
				itor_vec++;
			}
			itor_sub++;
		}
		itor++;
	}
	
}

UGFxObject* MMOAuctionManager::CreateSearchItem(UMMOGFXPlayer* gfxPlayer)
{
	UGFxObject* datList = gfxPlayer->CreateArray();
	//check(datList);
	if(datList == NULL)
		return NULL;

	CreateItemFromVec(&_auctionItemVec,datList,gfxPlayer);
	return datList;
}

UGFxObject* MMOAuctionManager::CreateQuickSearchItem(UMMOGFXPlayer* gfxPlayer)
{
	UGFxObject* datList = gfxPlayer->CreateArray();
	if(datList == NULL)
		return NULL;

	CreateQuickSearchItemFromVec(&_auctionItemVec,datList,gfxPlayer);
	return datList;
}


UGFxObject* MMOAuctionManager::CreateTreeList(UMMOGFXPlayer* gfxPlayer)
{
	UGFxObject* datList = gfxPlayer->CreateArray();
	//check(datList);
	if(datList == NULL)
		return NULL;
	UINT32 timeLeft = time(0);
	

	AuctionItemMap::iterator itor = _auctionItemMap.find(_searchFilter.MainType);
	if (itor != _auctionItemMap.end())
	{
		AuctionItemMap_Sub::iterator itor_sub = itor->second->find(_searchFilter.SubType);
		if (itor_sub != itor->second->end())
		{
			AuctionItemVec *vec = itor_sub->second;
			UpdateAuctionItemVec(vec);
			CreateItemFromVec(vec,datList,gfxPlayer);
		}
		
	}
	return datList;
	
}

void MMOAuctionManager::UpdateAuctionItemVec(AuctionItemVec* vec)
{
	_auctionItemVec.clear();
	AuctionItemVec::iterator itor = vec->begin();

	while(itor != vec->end())
	{
		_auctionItemVec.push_back(*itor);
		itor++;
	}
}

void MMOAuctionManager::CreateItemFromVec(AuctionItemVec* vec,UGFxObject* datList,UMMOGFXPlayer* gfxPlayer,Auction_FilterType filterType)
{
	INT idx = 0;
	AuctionItemVec::iterator itor_vec = vec->begin();
	while(itor_vec != vec->end())
	{
		UItem* item = MMOFacade::getSingleton().FindItemTemplate(*itor_vec);
		if (item)
		{
			//if (filterType != EAFT_Invalid)
			//{
				if (ChechIsCreateItem(item,filterType) == FALSE)
				{
					itor_vec++;
					continue;
				}
			//}
			
			if (CreateItemSolt(item,gfxPlayer,idx))
			{
				datList->SetElementObject(idx, CreateItemSolt(item,gfxPlayer,idx));
			}
			
		}
		idx++;
		itor_vec++;
	}
}

void MMOAuctionManager::CreateQuickSearchItemFromVec(AuctionItemVec* vec,UGFxObject* datList,UMMOGFXPlayer* gfxPlayer)
{
	INT idx = 0;
	AuctionItemVec::iterator itor_vec = vec->begin();
	while(itor_vec != vec->end())
	{
		UItem* item = MMOFacade::getSingleton().FindItemTemplate(*itor_vec);
		if (item)
		{
			if (item->GetTemplateID() == _searchItemId)
			{
				if (CreateItemSolt(item,gfxPlayer,idx))
				{
					datList->SetElementObject(idx, CreateItemSolt(item,gfxPlayer,idx));
					break;
				}
			}
		}
		itor_vec++;
	}
}

UGFxObject* MMOAuctionManager::CreateItemSolt(UItem* item,UMMOGFXPlayer* gfxPlayer,INT idx)
{
	UGFxObject*  dat = gfxPlayer->CreateObject(TEXT("UI_AuctionHouse.AS.AuctionItemData"));
	UGFxObject* slotDat = gfxPlayer->CreateObject(TEXT("ActionScript3.Slot.ItemSlotData"));

	if(dat != NULL && slotDat != NULL)
	{
		slotDat->SetString(TEXT("SlotRenderClassName"),  TEXT("NormalItemSlotRender"));
		slotDat->SetInt( TEXT("slotType"), ESPType_Invalid );
		slotDat->SetInt( TEXT("slotIndex"), idx );
		slotDat->SetBool( TEXT("HaveData"), TRUE );

		slotDat->SetInt(TEXT("quantity"), 1);
		slotDat->SetInt(TEXT("iconIndex"), -1);		
		slotDat->SetInt(TEXT("itemColor"), item->ItemColor);

		FString cs = FString(TEXT("img://")) + MMOFacade::getSingleton().GetItemIconNamePersonalized( item );
		slotDat->SetString(TEXT("iconName"), cs);

		slotDat->SetInt(TEXT("containerID"), -1);
		slotDat->SetInt(TEXT("slotID"), item->GetTemplateID());
		slotDat->SetInt(TEXT("itemTemplateID"), item->GetTemplateID());

		dat->SetObject(TEXT("ItemData"), slotDat);

		dat->SetInt(TEXT("AuctionID"), item->GetTemplateID());
		dat->SetString(TEXT("ItemName"), UITextParser::HTMLFormatItemColorString(item));
		dat->SetInt(TEXT("ItemTemplateID"), item->GetTemplateID());
		dat->SetInt(TEXT("ItemColor"), item->ItemColor);
		dat->SetInt("ItemLevel",item->ItemLevel);
	}
	
	return dat;
	
}

bool MMOAuctionManager::ChechIsCreateItem(UItem* item,Auction_FilterType filterType)
{
	
	if (ChechIsCreateItem_Class(item) /*&& ChechIsCreateItem_ItemColor(item)*/
		/*&& ChechIsCreateItem_LV(item)*/ &&CheckIsCreateItem_Name(item))
	{
		return TRUE;
	}
	return FALSE;
}

bool MMOAuctionManager::ChechIsCreateItem_Class(UItem* item)
{
	bool isCreate_Class = TRUE;
	if (_searchFilter.PlayerClass != INVALID_PROFESSION)
	{
		if (MMO::CanBeUsedByPlayerClass((EPlayerClass(_searchFilter.PlayerClass - 1)), item->RequireClass) == FALSE)
		{
			isCreate_Class = FALSE;
		}
	}
	return isCreate_Class;
}
bool MMOAuctionManager::ChechIsCreateItem_ItemColor(UItem* item)
{
	bool isCreate_ItemColor = TRUE;
	if (_searchFilter.ItemColor != INVALID_SELECTEDINDEX && item->ItemColor != _searchFilter.ItemColor)
	{
		isCreate_ItemColor = FALSE;
	}
	return isCreate_ItemColor;
}

bool MMOAuctionManager::ChechIsCreateItem_LV(UItem* item)
{
	bool isCreate_LV = TRUE;
	if (item->RequireLevel < _searchFilter.LevelMin || item->RequireLevel > _searchFilter.LevelMax )
	{
		isCreate_LV = FALSE;
	}
	return isCreate_LV;
}

bool MMOAuctionManager::CheckIsCreateItem_Name(UItem* item)
{
	bool isCreate_Name = TRUE;
	if (!_searchFilter.SearchName.IsEmpty())
	{
		if (item->GetDisplayName().InStr(_searchFilter.SearchName,FALSE, TRUE) == INDEX_NONE )
		{
			isCreate_Name = FALSE;
		}
		else if (item->Auction == FALSE)
		{
			isCreate_Name = FALSE;
		}
	}
	
	return isCreate_Name;
}


bool MMOAuctionManager::IsShowTreeList(Auction_FilterType type)
{
	bool v = true;
	/*switch(type)
	{
	case EAFT_Profession:
	case EAFT_ItemColor:
		{
			if (_searchFilter.SearchName.IsEmpty() && _searchFilter.MainType ==0 && _searchFilter.SubType ==0)
			{
				v = false;
			}

		}
		break;
	default:
		break;
	}*/
	return v;
}

void MMOAuctionManager::NotifyCreateTreeItemList()
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_UpdateTreeList);
	MMOUI::PostGameEvent(&evt);
}

void MMOAuctionManager::NotifySearchTreeItemList()
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_SearchTreeList);
	MMOUI::PostGameEvent(&evt);
}

void MMOAuctionManager::NotifyQuickSearch()
{
	SearchItem();
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_QuickSearch);
	MMOUI::PostGameEvent(&evt);
}

bool MMOAuctionManager::CkeckIsShowTreeList(BYTE mainType, BYTE subType)
{
	if (mainType > 0 && subType > 0)
	{
		return true;
	}
	else if(mainType > 0 && subType == 0)
	{
		const AuctionCategoryList::DataTypeMap& dataMap = AuctionCategoryList::getSingleton().GetDataMap();
		AuctionCategoryList::DataTypeMap::const_iterator it = dataMap.begin();
		while (it != dataMap.end())
		{
			const FAuctionCategoryItem* item = it->second;
			if (item->MainType.Val == mainType)
			{
				if (item->SubType.Num() <= 1)
				{
					const FStringValuePair* subItem = &item->SubType(0);
					_searchFilter.SubType =  subItem->Val;
					return true;
				}
			}
			
			it++;
		}
	}
	return false;
}

void MMOAuctionManager::NotifyFilterTreeItem(Auction_FilterType nType)
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_FilterTreeList);
	evt.filterType = nType;
	MMOUI::PostGameEvent(&evt);
	
}

void MMOAuctionManager::NotifyUpdateItemCategoryList()
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_UpdateItemCategoryList);
	MMOUI::PostGameEvent(&evt);
}

UGFxObject* MMOAuctionManager::CreateTreeListByFilter(UMMOGFXPlayer* gfxPlayer,Auction_FilterType filterType)
{
	UGFxObject* datList = gfxPlayer->CreateArray();
	//check(datList);
	if(datList == NULL)
		return NULL;

	UpdateFilterByName();
	if (_searchFilter.SearchName.Len() > 0)
	{
		SearchItem();
	}
	
	CreateItemFromVec(&_auctionItemVec,datList,gfxPlayer,filterType);
	return datList;
}

void MMOAuctionManager::UpdateFilterByName()
{
	if (_searchFilter.SearchName.Len() <= 0)
	{
		_searchFilter.MainType = EATM_ALL;
		_searchFilter.SubType = EATS_WW_ALL;
		_auctionItemVec.clear();
	}
}

AuctionItemVec* MMOAuctionManager:: GetAuctionItemVec()
{
	return &_auctionItemVec;
}
void MMOAuctionManager::ResetAuctionItemVec()
{
	_auctionItemVec.clear();
}

void MMOAuctionManager::AuctionItemVec_AddItem(INT32 id)
{
	_auctionItemVec.push_back(id);
}

void MMOAuctionManager::SetSearchItemId(INT id)
{
	_searchItemId = id;
	_searchFilter.searchType = EAST_ItemId;

	/*UItem* item = MMOFacade::getSingleton().FindItemTemplate(id);
	if (item)
	{
		SetFilterCategory( item->AuctionType , item->AuctionSubType );
	}*/
	
}

void MMOAuctionManager::ResetSearchFilter()
{
	_searchFilter.Reset();
}

void MMOAuctionManager::OnCategoryClick(BYTE mainType, BYTE subType)
{
	//if (MMOAuctionManager::getSingleton().CkeckIsShowTreeList(mainType,subType))
	//{
		//MMOAuctionManager::getSingleton().NotifyCreateTreeItemList();
		//MMOAuctionManager::getSingleton().ResetSearchFilter();
		MMOAuctionManager::getSingleton().SetFilterCategory(mainType, subType);
		Send_SearchByFilter();
		//CMessageHandler::GetInstance().Send_AuctionQueryByType(mainType,subType,_searchFilter.SortType,_searchFilter.SortAscendent);
	//}
}

UItem* MMOAuctionManager::GetSearchItemByID()
{
	UItem* item = MMOFacade::getSingleton().FindItemTemplate(_searchItemId);
	if (item)
	{
		return item;
	}

	return NULL;
}

void MMOAuctionManager::NotifyQuickSearchByBag(BYTE SlotType,BYTE ContainerID,INT SlotID)
{
	MMOGameEvent_Auction_QuickSearchByBag evt(SlotType, ContainerID, SlotID);
	MMOUI::PostGameEvent(&evt);
}

INT MMOAuctionManager::GetitemIdByAuctionId(INT auctionId,Auction_ScreenMsg_Type type)
{
	INT itemId = 0;
	ClientAuctionItemInfo* info = NULL;
	switch (type)
	{
	case AS_Type_ViewPage:
		info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionViewItemList, auctionId));
		break;
	case AS_Type_BuyPage:
		info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionMyBuyingItemList, auctionId));
		break;
	case  AS_Type_BuyFailedPage:
		info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionMyFailedAuctionItemList, auctionId));
		break;
	}
	
	if (info && info->ItemTemplate)
	{
		itemId = info->ItemTemplate->ItemId;
	}
	
	return itemId;
}

INT64 MMOAuctionManager::GetRealAuctionMoney(INT64 price)
{
	INT64 realPrice = price * MONEY_RATIO;
	return realPrice;
}


ClientAuctionItemInfo* MMOAuctionManager::GetItemFromBuyListAndBuyOutList(INT auctionId)
{
	INT itemId = 0;
	ClientAuctionItemInfo* info = NULL;

	info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionMyBuyingItemList, auctionId));

	if (info == NULL)
	{
		info = const_cast<ClientAuctionItemInfo*>(FindAuctionItem(_auctionMyFailedAuctionItemList, auctionId));
	}
	
	return info;
}

void MMOAuctionManager::NotifyUpdateMyBuyingList()
{
	switch(_currentTabPage)
	{
	case EATID_Buy:
		{
			NotifyMyBuyingList();
			NotifyFailedList();
		}
		break;
	case EATID_Order:
		{
			NotifyUpdateBoughtList();
		}
		break;
	}

}
void MMOAuctionManager::NotifyUpdateBoughtList()
{
	MMOGameEvent_Auction_DataUpdate evt(GameEvent_Auction_ListMyBought);
	evt.AuctionItemListPtr = &_auctionMyBoughtItemList;
	MMOUI::PostGameEvent(&evt);
}

void MMOAuctionManager::NotifyUpdatePayLeftItem()
{
	if (_currentTabPage == EATID_Order)
	{
		SetCurrentTabPage(EATID_Order);
	}
}

const ClientAuctionItemInfo* MMOAuctionManager::GetBoughtItemById(INT auctionId)
{
	const ClientAuctionItemInfo* info = FindAuctionItem(_auctionMyBoughtItemList, auctionId);
	if (info)
	{
		return info;
	}
	else
	{
		return NULL;
	}
}

void MMOAuctionManager::NotifyShowLinkItem()
{
	MMOGameEvent_Any evt( GameEvent_Auction_ShowLinkItem );
	MMOUI::PostGameEvent( &evt );
}

void MMOAuctionManager::ResetLinkItemInfo()
{
	linkItemIndex = 0;
	linkItemId = 0;
	bShowLinkItem = FALSE;
}

UINT32 MMOAuctionManager::GetClientRealTime()
{
	UINT32 newTime = 0;

	UINT32 timeLeft = time(0);
	INT32 timeDiff =  MMOFacade::getSingleton().GetClientToSceneSecondDiff();

	if (timeDiff > 0)
	{
		newTime = timeLeft - timeDiff;
	}
	else
	{
		newTime = timeLeft + ( -timeDiff);
	}
	
	return newTime;
}

void MMOAuctionManager::UpdateAuctionView()
{
	switch(_currentTabPage)
	{
	case EATID_View:
		{
			NotifySearchResult(TRUE);
		}
		break;
		/*case EATID_Buy:
		case EATID_Order:
		{
		NotifyUpdateMyBuyingList();
		}
		break;*/
	}
}

void MMOAuctionManager::ResetSellInfo()
{
	sellInfo.byAuctionTimeType = 0;
	sellInfo.byConSlot = 0;
	sellInfo.byItemSlot = 0;
	sellInfo.byOpen = 0;
	sellInfo.llBuyoutPrice = 0;
	sellInfo.llDepositePrice = 0;
	sellInfo.llPrice = 0;
}

void MMOAuctionManager::SetAuctionSellInfo(AuctionSellInfo info)
{
	sellInfo.byAuctionTimeType = info.byAuctionTimeType;
	sellInfo.byConSlot = info.byConSlot;
	sellInfo.byItemSlot = info.byItemSlot;
	sellInfo.byOpen = info.byOpen;
	sellInfo.llBuyoutPrice = info.llBuyoutPrice;
	sellInfo.llDepositePrice = info.llDepositePrice;
	sellInfo.llPrice = info.llPrice;
}