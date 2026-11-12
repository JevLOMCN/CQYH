
#include "stdafx.h"
#include "MallManager.h"
#include "MMOFacade.h"
#include "MMOGameClasses.h"
#include "CommonGameEvents.h"
#include "MMOMsgButtonManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "MallController.h"
#include "tabfile.h"

MallManager::MallManager():nNumPerPage(15),nGoldMoneyID(1)
{
	nRealMoney = 0;
	nCrc       = 0;
	nPageNumType = 0;
	curGoodsType = MallStoreGOODS_All;
	curGoodsUseStateType = MallUSINGGOODS_ALL;
	nCurPage = 0;
	nMaxPage = 0;
	mallGoodsList.clear();
}

void MallManager::Init()
{
	nRealMoney = 0;
	nCrc       = 0;
	nPageNumType = 0;
	curGoodsType = MallStoreGOODS_All;
	curGoodsUseStateType = MallUSINGGOODS_ALL;
	nCurPage = 0;
	nMaxPage = 0;
	mallGoodsList.clear();
}
void MallManager::Shutdown()
{
	nRealMoney = 0;
	nCrc = 0;
	mallGoodsList.clear();
}
void MallManager::NotifyGameStateChange()
{
	static BOOL bInit = FALSE;
	if(MMO::GetCurrentGameState() == EGameState_Selecting || MMO::GetCurrentGameState() == EGameState_Login || MMO::GetCurrentGameState() == EGameState_None)
	{
		bInit = FALSE;
	}
	else if(MMO::GetCurrentGameState() == EGameState_InGame)
	{
		if(!bInit)
		{
			Init();
			LoadGoodsData(TEXT("real_money_shop.txt"));
		}
		bInit = TRUE;
	}
}
void MallManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{

}
INT32	MallManager::GetRealMoney()
{
	return nRealMoney;
}

UINT32	MallManager::GetMallCrc()
{
	return nCrc;
}
void MallManager::ClearData()
{
	nCrc = 0;
	nCurPage = 0;
	nMaxPage = 0;
	mallGoodsList.clear();
	mapMountID.clear();
}
void MallManager::SetPage(INT nNum)
{
	nCurPage = nNum;
}
INT	MallManager::GetPage()
{
	return nCurPage;
}	
INT	MallManager::GetMaxPage()
{
	return nMaxPage;
}
const MallGoodsList& MallManager::GetGoodsList()
{
	return mallGoodsList;
}
const MallGoodsClientDataList& MallManager::GetGoodsClientDataList()
{
	return mallClientDataList;
}
const REAL_MONEY_ITEM_INFO*	MallManager::GetGoodsByID(INT nGoodsID)const
{
	MallGoodsList::const_iterator itr = mallGoodsList.begin();
	for (; itr != mallGoodsList.end(); ++itr)
	{
		if(itr->m_nItemTemplateID == nGoodsID)
			return &(*itr);
	}
	return 0;
}

void MallManager::NotifyBuyResult(INT nGoodsID, INT nMoney)
{
	nRealMoney -= nMoney;
}

void MallManager::NotifyRealMoney(INT32 nMoney)
{
	nRealMoney = nMoney;
}
void MallManager::NotifyGoodsList(const M2C_SYNC_REAL_MONEY_SHOP_DATA* pkData)
{
	if(pkData == NULL)
		return;
	if(nCrc == pkData->dwCrc)
		return;

	ClearData();
	nCrc = pkData->dwCrc;

	for(UINT32 n = 0; n < pkData->dwItemCount; ++n)
	{
		if(pkData->Items[n].m_nItemTemplateID == nGoldMoneyID)
			continue;
		mallGoodsList.push_back(pkData->Items[n]);
	}
}

BOOL MallManager::LoadGoodsData(const TCHAR* filePath)
{
	ClearData();

	return FALSE;
}

void MallManager::SetGoodsType(INT nType)
{
	static UBOOL nAdd = 0;
	nAdd = (curGoodsType == nType)?!nAdd:0;
	curGoodsType = (MallStoreGoodsType)nType;
	SortGoodsList(curGoodsType,(INT)nAdd);
}
void MallManager::SetGoodsToMainPlayerType(INT nType)
{
	curGoodsUseStateType = (MallStoreGoodstoUseType)nType;
	FilterGoodsType(nType);
	INT nSizeGoods = vecMallCurGoods.size();
	nMaxPage = nSizeGoods/nNumPerPage + ((nSizeGoods%nNumPerPage == 0)?0:1);
	if(nMaxPage == 0)
		nMaxPage = 1;
	if(nCurPage >= nMaxPage)
		nCurPage = nMaxPage - 1;
}
INT MallManager::GetGoodsType()
{
	return (INT)curGoodsType;
}
INT	MallManager::GetGoodsUseType()
{
	return (INT)curGoodsUseStateType;
}
BOOL MallManager::CompareRenQi_ADD(const REAL_MONEY_ITEM_INFO&pkLeft,const REAL_MONEY_ITEM_INFO&pkRight)
{
	return (pkLeft.m_nRenQi < pkRight.m_nRenQi);
}
BOOL MallManager::CompareRenQi_DEC(const REAL_MONEY_ITEM_INFO&pkLeft,const REAL_MONEY_ITEM_INFO&pkRight)
{
	return (pkLeft.m_nRenQi > pkRight.m_nRenQi);
}
BOOL MallManager::CompareCost_ADD(const REAL_MONEY_ITEM_INFO&pkLeft,const REAL_MONEY_ITEM_INFO&pkRight)
{
	return (pkLeft.m_nCurrentPrice < pkRight.m_nCurrentPrice);
}
BOOL MallManager::CompareCost_DEC(const REAL_MONEY_ITEM_INFO&pkLeft,const REAL_MONEY_ITEM_INFO&pkRight)
{
	return (pkLeft.m_nCurrentPrice > pkRight.m_nCurrentPrice);
}
void MallManager::SortGoodsList(INT nType, INT nAdd)
{
	if((MallStoreGoodsType)nType == MallStoreGOODS_RENQI)
	{
		nAdd? mallGoodsList.sort(CompareRenQi_ADD):mallGoodsList.sort(CompareRenQi_DEC);
	}
	else if((MallStoreGoodsType)nType == MallStoreGOODS_COST)
	{
		nAdd? mallGoodsList.sort(CompareCost_ADD):mallGoodsList.sort(CompareCost_DEC);
	}
	/*vecMallCurGoods.clear();
	MallGoodsList::const_iterator itr = mallGoodsList.begin();
	for (; itr != mallGoodsList.end(); ++itr)
	{
		if(itr->m_byPage == nPageNumType)
			vecMallCurGoods.push_back(itr->m_nItemTemplateID);
	}*/
	FilterGoodsType(curGoodsUseStateType);
}
void MallManager::NotifyBuyInfo(const M2C_SALE_INFO*pkData)
{
	if(pkData == NULL)
		return;
	if(pkData->nSaleInfoCount != 0)
		mallBuyInfoList.clear();
	for (INT n = 0; n < pkData->nSaleInfoCount; ++n)
	{
		const SALE_INFO& data = pkData->stSaleInfo[n];

		UItem* pkItem = MMOFacade::getSingleton().FindItemTemplate( data.nItemID );
		if(pkItem == NULL)
			continue;
		time_t time = (time_t)data.dwTime;
		tm     *tp = localtime(&time);
		if(tp == NULL)
			continue;
		FString strName = UTF8_TO_TCHAR(data.szRoleName);
		FString strInfo = FString::Printf(TEXT("<font color='#14a800'>[%d:%02d]</font> %s purchased"),tp->tm_hour,tp->tm_min,*strName);

		strInfo += LinkManager::HtmlLink_ColorItem(pkItem);

		mallBuyInfoList.push_back(strInfo);
	}
}
void MallManager::TestMallInfo()
{
	/*MallGoodsClientDataList::iterator itr = mallClientDataList.begin();
	for (INT n = 0;itr != mallClientDataList.end(); ++n,++itr)
	{
		UItem* pkItem = MMOFacade::getSingleton().FindItemTemplate( itr->nItemTemplateID);
		if(pkItem == NULL)
			continue;
		time_t time = (time_t)56566666;
		tm     *tp = localtime(&time);
		if(tp == NULL)
			continue;
		FString strInfo = FString::Printf(TEXT("<font color='#14a800'>[%d:%02d]</font> Player purchased"),tp->tm_hour,tp->tm_min);

		strInfo += LinkManager::HtmlLink_ColorItem(pkItem);

		mallBuyInfoList.push_back(strInfo);
	}
	MMOGameEvent_MallStore evt( MMOGameEvent_MallStore::EMallStoreEvent_BuyInfoChange);
	MMOUI::PostGameEvent(&evt);*/
}
const MallBuyInfoList& MallManager::GetBuyInfo()const
{
	return mallBuyInfoList;
}
void MallManager::TickUpdateInfo()
{
	if(nCrc == 0)
		return;
	static time_t tLastSend = 0;
	time_t ttN = time(NULL);
	if (ttN - tLastSend > 60)
	{
		tLastSend = ttN;
		MallController::getSingleton().NotifyReqBuyInfoChange();
	}
}
void MallManager::GetGoodsByPage(std::vector<INT>& vecGoods)
{
	vecGoods.clear();
	
	for (INT n = nNumPerPage*nCurPage; n < (INT)vecMallCurGoods.size()&&(n < nNumPerPage*(nCurPage+1));++n)
	{
		vecGoods.push_back(vecMallCurGoods.at(n));
	}
}
const std::vector<INT>&	 MallManager::GetHotGoods()
{
	return vecMallHotGoods;
}
const std::vector<INT>&	 MallManager::GetShowGoods()
{
	return vecMallShowGoods;
}
void MallManager::SetGoodsPageType(MallStorePageType pageType)
{
	nPageNumType = pageType;
	FilterGoodsType(curGoodsUseStateType);
	INT nSizeGoods = vecMallCurGoods.size();
	nMaxPage = nSizeGoods/nNumPerPage + ((nSizeGoods%nNumPerPage == 0)?0:1);
	if(nMaxPage == 0)
		nMaxPage = 1;
	nCurPage = 0;
}
INT MallManager::GetMountIDByItemID(INT nGoodsID)
{
	std::map<INT,INT>::iterator itr = mapMountID.find(nGoodsID);
	if(itr != mapMountID.end())
		return itr->second;
	return 0;
}
const MALLGOODSSHOWDATA*MallManager::GetGoodsClientData(INT nGoodsID)const
{
	MallGoodsClientDataList::const_iterator itr = mallClientDataList.begin();
	for (;itr != mallClientDataList.end(); ++itr)
	{
		if(itr->nItemTemplateID == nGoodsID)
			return &(*itr);
	}
	return NULL;
}
void MallManager::FilterGoodsType(INT nShowType)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL)
		return;
	EPlayerClass job = (EPlayerClass)mainPlayer->ActorAttributes->ClassType;
	vecMallCurGoods.clear();
	MallGoodsList::const_iterator itr = mallGoodsList.begin();
	for (; itr != mallGoodsList.end(); ++itr)
	{
		if(itr->m_byPage == nPageNumType)
		{
			if(nShowType == (INT)MallUSINGGOODS_AVAIBLEJOB)
			{
				UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate( itr->m_nItemTemplateID );
				if(itemTemp)
				{
					if(MMO::CanBeUsedByPlayerClass(job, itemTemp->RequireClass))
						vecMallCurGoods.push_back(itr->m_nItemTemplateID);
				}
			}
			else
				vecMallCurGoods.push_back(itr->m_nItemTemplateID);
		}
	}
}

void MallManager::UpdateMoneyLog( SHOP_LOG* log , INT count )
{
	shopLogArray.Empty();

	for ( INT i = 0 ; i < count ; i++ )
	{
		shopLogArray.AddItem( log[i] );
	}

	MMOGameEvent_Any msg(GameEvent_UpdateMoneyExchangeLog);
	MMOUI::PostGameEvent( &msg );
}