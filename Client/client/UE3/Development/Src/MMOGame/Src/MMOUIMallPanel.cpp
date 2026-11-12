#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "UITextParser.h"
#include "MallController.h"
#include "MailManager.h"
#include "MallManager.h"

IMPLEMENT_CLASS(UMMOUIMallPanel)


void UMMOUIMallPanel::Init()
{
	Super::Init();

	eventInitMallGoodsIcon();

	UGFxObject* data = GFXPlayer->CreateArray();
	if(data)
	{
		INT nIndex = 0;
		data->SetElementString(nIndex++,MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsType1")));
		data->SetElementString(nIndex++,MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsTypeCostSort")));
		eventCallInitDropMenuList(0,data);
	}
	
	UGFxObject* dataSec = GFXPlayer->CreateArray();
	if(dataSec)
	{
		INT nIndex = 0;
		dataSec->SetElementString(nIndex++,MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsType3")));
		dataSec->SetElementString(nIndex++,MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsType2")));
		eventCallInitDropMenuList(1,dataSec);
	}
}

void UMMOUIMallPanel::NativeInitLoad()
{	
	Super::NativeInitLoad();
}

void UMMOUIMallPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if(v)
	{
		UpdateGoldMoney(0);
		MallController::getSingleton().SendMallRealMoney();
		MallController::getSingleton().SendMallGoodsList();

		FVector2D scrSz = MMOFacade::GetScreenSize();
		SetPosition((scrSz.X - eventGetWidth())/2,(scrSz.Y - eventGetHeight())/2);

		eventCallUCSetMallTab(0);
		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.Title")));	
		UpdateMallHomePanel();
	}
}
void UMMOUIMallPanel::UpdateMallGoodsPanel()
{
	UpdateMallHomeHotList();
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(dataProvider == NULL)
		return;
	std::vector<INT> vecGoods;
	MallManager::getSingleton().GetGoodsByPage(vecGoods);
	INT nPage = MallManager::getSingleton().GetPage();
	INT nMaxPage = MallManager::getSingleton().GetMaxPage();
	FString strPage = FString::Printf(TEXT("%d/%d"),nPage + 1,nMaxPage);
	if(prePageBtn)
		prePageBtn->SetBool("enabled",(nPage > 0));
	if(nextPageBtn)
		nextPageBtn->SetBool("enabled",(nPage + 1 < nMaxPage));
	eventCallUpdatePageNum(strPage);

	for (INT nIndex = 0 ,nShow = 0; nIndex < (INT)vecGoods.size(); ++nIndex)
	{
		const REAL_MONEY_ITEM_INFO*	pkGoods = MallManager::getSingleton().GetGoodsByID(vecGoods[nIndex]);
		if(pkGoods == NULL)
			continue;
		UItem* pkItem = MMOFacade::getSingleton().FindItemTemplate( pkGoods->m_nItemTemplateID );
		if(pkItem == 0)
			continue;
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Mall.MallGoodsData"));
		if(object == NULL)
			continue;
		object->SetInt("ItemID",pkGoods->m_nItemTemplateID);

		FString nameItem = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkItem->ItemColor));
		nameItem += pkItem->GetDisplayName();
		nameItem += UITextParser::HTMLEndFont();

		object->SetString("ItemName",nameItem);
		if(pkGoods->m_nOriginalPrice == 0)
			object->SetString("ItemCost0",TEXT(""));
		else
			object->SetString("ItemCost0",UITextParser::HTMLMoneyIconString(MONEYTYPE_04, pkGoods->m_nOriginalPrice)); 
		object->SetString("ItemCost1",UITextParser::HTMLMoneyIconString(MONEYTYPE_04, pkGoods->m_nCurrentPrice));
		object->SetInt("ItemCostValue",pkGoods->m_nCurrentPrice);
		object->SetString("ItemDescrible",pkItem->Description);
		object->SetInt("ItemTag",pkGoods->m_byFlag);
		object->SetInt("ItemType",pkGoods->m_nRenQi);
		object->SetInt("ItemNum",pkGoods->m_shItemCount);
		object->SetInt("ItemIndex",nShow);
		SetGoodsItemIcon(pkItem,nShow,pkGoods->m_shItemCount);

		dataProvider->SetElementObject(nShow++,object);
	}
	eventCallUpdateGoodsList(dataProvider);

}
void UMMOUIMallPanel::UpdateMallHomePanel()
{
	UpdateMallHomeHotList();
	UpdateMallHomeShowList();
}
void UMMOUIMallPanel::UpdateMallHomeHotList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(dataProvider == NULL )
		return;
	const MallGoodsList& goodsList = MallManager::getSingleton().GetGoodsList();
	MallGoodsList::const_iterator iterGoodsInfo = goodsList.begin();
	for (INT nIndex = 0; iterGoodsInfo != goodsList.end(); ++ iterGoodsInfo)
	{
		UItem* pkItem = MMOFacade::getSingleton().FindItemTemplate( iterGoodsInfo->m_nItemTemplateID );
		if(pkItem == NULL)
			continue;

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Mall.MallGoodsData"));
		if(object == NULL)
			continue;
		object->SetInt("ItemID",iterGoodsInfo->m_nItemTemplateID);
		FString nameItem = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkItem->ItemColor));
		nameItem += pkItem->GetDisplayName();
		nameItem += UITextParser::HTMLEndFont();
		object->SetString("ItemName",nameItem);
		if(iterGoodsInfo->m_nOriginalPrice == 0)
			object->SetString("ItemCost0",TEXT(""));
		else
			object->SetString("ItemCost0",UITextParser::HTMLMoneyIconString(MONEYTYPE_04, iterGoodsInfo->m_nOriginalPrice));
		object->SetString("ItemCost1",UITextParser::HTMLMoneyIconString(MONEYTYPE_04, iterGoodsInfo->m_nCurrentPrice));
		object->SetInt("ItemCostValue",iterGoodsInfo->m_nCurrentPrice);
		object->SetString("ItemDescrible",pkItem->Description);
		object->SetInt("ItemTag",iterGoodsInfo->m_byFlag);
		object->SetInt("ItemType",iterGoodsInfo->m_nRenQi);
		object->SetInt("ItemNum",iterGoodsInfo->m_shItemCount);

		object->SetInt("ItemIndex",nIndex + 1);
		SetHotItemIcon(pkItem,nIndex,iterGoodsInfo->m_shItemCount);
		dataProvider->SetElementObject(nIndex++,object);
	}
	eventCallUpdateMallHotList(dataProvider);
}
void UMMOUIMallPanel::UpdateMallHomeShowList()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(dataProvider == NULL)
		return;
	const std::vector<INT>& goodsList = MallManager::getSingleton().GetShowGoods();
	for (INT nIndexShow = 0, nIndex = 0; nIndexShow < (INT)goodsList.size(); ++nIndexShow)
	{
		const REAL_MONEY_ITEM_INFO*	pkGoodsInfo	= MallManager::getSingleton().GetGoodsByID(goodsList[nIndexShow]);
		if(pkGoodsInfo == NULL)
			continue;
		const MALLGOODSSHOWDATA*	pkGoodsClient	= MallManager::getSingleton().GetGoodsClientData(goodsList[nIndexShow]);
		if(pkGoodsClient == NULL)
			continue;
		UItem* pkItem = MMOFacade::getSingleton().FindItemTemplate( pkGoodsInfo->m_nItemTemplateID );
		if(pkItem == NULL)
			continue;

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Mall.MallGoodsData"));
		if(object == NULL)
			continue;
		object->SetInt("ItemID",pkGoodsInfo->m_nItemTemplateID);
		FString nameItem = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkItem->ItemColor));
		nameItem += pkItem->GetDisplayName();
		nameItem += UITextParser::HTMLEndFont();
		object->SetString("ItemName",nameItem);
		object->SetString("ItemIcon",pkGoodsClient->iconShowPath);
		if(pkGoodsInfo->m_nOriginalPrice == 0)
			object->SetString("ItemCost0",TEXT(""));
		else
			object->SetString("ItemCost0",UITextParser::HTMLMoneyIconString(MONEYTYPE_04, pkGoodsInfo->m_nOriginalPrice));
		object->SetString("ItemCost1",UITextParser::HTMLMoneyIconString(MONEYTYPE_04, pkGoodsInfo->m_nCurrentPrice));
		object->SetInt("ItemCostValue",pkGoodsInfo->m_nCurrentPrice);
		object->SetString("ItemDescrible",pkItem->Description);
		object->SetInt("ItemTag",pkGoodsInfo->m_byFlag);
		object->SetInt("ItemType",pkGoodsInfo->m_nRenQi);
		object->SetInt("ItemNum",pkGoodsInfo->m_shItemCount);
		//Display area
		object->SetInt("ItemIndex",nIndex + 1);
		dataProvider->SetElementObject(nIndex++,object);
	}
	eventCallUCUpdateMallHomeShowData(dataProvider);
}
void UMMOUIMallPanel::SetGoodsItemIcon(UItem* pkItem,INT nIndex,INT nNum)
{
	if(pkItem == NULL)
		return;
	if(nIndex < 0 || nIndex >= mallGoodsList.Num())
		return;
	GFXPlayer->eventCallSetItemData( mallGoodsList(nIndex) , 0 , TEXT("") , -1, ESPType_MallStore, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
	GFXPlayer->SetItemSlotData(mallGoodsList(nIndex),pkItem,ESPType_MallStore,-1,-1);

	if(mallGoodsList(nIndex))
	{
		UGFxObject* pAmount = mallGoodsList(nIndex)->GetObject(TEXT("valueAmount"));
		ASSERT_CHECK_ERROR_RETURN(pAmount);
		pAmount->SetText(appItoa(nNum));
	}
}
void UMMOUIMallPanel::SetHotItemIcon(UItem* pkItem,INT nIndex,INT nNum)
{
	if(pkItem == NULL)
		return;
	if(nIndex < 0 || nIndex >= mallHotList.Num())
		return;
	GFXPlayer->eventCallSetItemData( mallHotList(nIndex) , 0 , TEXT("") , -1, ESPType_MallStore, TEXT("") , ITEMCOLOR_INVALID, -1, -1 );
	GFXPlayer->SetItemSlotData(mallHotList(nIndex),pkItem,ESPType_MallStore,-1,-1);
	if(mallHotList(nIndex))
	{
		UGFxObject* pAmount = mallHotList(nIndex)->GetObject(TEXT("valueAmount"));
		ASSERT_CHECK_ERROR_RETURN(pAmount);
		pAmount->SetText(appItoa(nNum));
	}
}
void UMMOUIMallPanel::UpdateGoldMoney(INT nMoney)
{
	eventCallUpdateMoney(nMoney);
}
UBOOL UMMOUIMallPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return TRUE;
}
UBOOL UMMOUIMallPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return TRUE;
}
UBOOL UMMOUIMallPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return TRUE;
}
UBOOL UMMOUIMallPanel::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	return TRUE;
}
void UMMOUIMallPanel::UpdateNoticeBuy()
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if(dataProvider == NULL)
		return;
	const MallBuyInfoList&	dataInfo = MallManager::getSingleton().GetBuyInfo();
	MallBuyInfoList::const_iterator itr = dataInfo.begin();
	for (INT n = 0; itr != dataInfo.end(); ++n,++itr)
	{
		dataProvider->SetElementString(n,*itr);
	}
	eventCallUCUpdateNewNotice(dataProvider);
}
void UMMOUIMallPanel::AddNewNotice(const FString& str)
{
	eventCallAddNewNotice(str);
}

INT UMMOUIMallPanel::GetCurGoodsID()
{
	return GoodsID;
}

//////////////////////////////////////////////////////////////////////////
void UMMOUIMallPanel::ASClickDropMenuHotType(int nType)
{
	MallController::getSingleton().SetSortType(nType);
}

void UMMOUIMallPanel::ASClickDropMenuAvaibleType(int nType)
{
	MallController::getSingleton().SetShowType(nType);
}

void UMMOUIMallPanel::ASClickBtnBuyVip()
{

}

void UMMOUIMallPanel::ASClickBtnCheckIn()
{

}

void UMMOUIMallPanel::ASAddGoldMoney()
{
	//MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.GoldAddHint")),MsgInfoType_Table,TEXT("MMOGame.Mall.GoldAddHint"));
	ShellExecute(NULL, L"open", L"http://pay.sdo.com", NULL, NULL, SW_SHOWNORMAL);
}

void UMMOUIMallPanel::ASClickMallTap(INT nIndex)
{
	MallManager::getSingleton().SetGoodsPageType((MallStorePageType)nIndex);

	if(nIndex == (INT)MallStorePageHome)
		UpdateMallHomePanel();
	else
		UpdateMallGoodsPanel();
}

void UMMOUIMallPanel::ASClickMallGoods(INT nGoodsID)
{
	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MallBuyGoods,TRUE, ""); 
	UMMOUIMallBuyPanel* mallBuyPanel = (UMMOUIMallBuyPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_MallBuyGoods);
	if (mallBuyPanel)
	{
		mallBuyPanel->OnShowMallBuyPanel(nGoodsID);
		UMMOUIMallEquipRolePanel* mallEquipRolePanel = (UMMOUIMallEquipRolePanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_MallEquipRole);
		if (mallEquipRolePanel)
		{
			mallEquipRolePanel->OnShowMallEquipPanel(nGoodsID);
		}
		GoodsID = nGoodsID;
	}
}

void UMMOUIMallPanel::ASClickPrePage()
{
	MallController::getSingleton().DecPage();
}

void UMMOUIMallPanel::ASClickNextPage()
{
	MallController::getSingleton().AddPage();
}

