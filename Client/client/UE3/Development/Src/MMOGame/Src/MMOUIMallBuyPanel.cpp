#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MallManager.h"
#include "UILayout.h"
#include "UITextParser.h"
#include "MallController.h"
#include "MMOMsgBoxManager.h"

IMPLEMENT_CLASS(UMMOUIMallBuyPanel)


void UMMOUIMallBuyPanel::Init()
{
	Super::Init();
	eventInitBuyGoodsIcon();
}

void UMMOUIMallBuyPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if(v)
	{
		FVector2D scrSz = MMOFacade::GetScreenSize();
		SetPosition((scrSz.X - eventGetWidth())/2,(scrSz.Y - eventGetHeight())/2);
	}
	else
	{
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MallFriend,FALSE, "");
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MallEquipRole,FALSE, "");
	}
}

void UMMOUIMallBuyPanel::NativeInitLoad()
{	
	Super::NativeInitLoad();
}

void UMMOUIMallBuyPanel::OnShowMallBuyPanel(INT nGoodsID)
{
	//Home page gift pack, specific, change later
	const REAL_MONEY_ITEM_INFO* pkItem = MallManager::getSingleton().GetGoodsByID(nGoodsID);
	if(pkItem != NULL)	
	{
		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate( pkItem->m_nItemTemplateID );
		if(itemTemp != NULL)
		{
			UGFxObject* object = GFXPlayer->CreateObject(TEXT("ActionScript3.Mall.MallGoodsData"));
			if(object != NULL)
			{
				object->SetInt("ItemID",pkItem->m_nItemTemplateID);
				FString nameItem = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)itemTemp->ItemColor));
				nameItem += itemTemp->GetDisplayName();
				nameItem += UITextParser::HTMLEndFont();

				object->SetString("ItemName",nameItem);
				if(pkItem->m_nOriginalPrice == 0)
					object->SetString("ItemCost0",TEXT(""));
				else
					object->SetString("ItemCost0",UITextParser::HTMLMoneyIconString(MONEYTYPE_04, pkItem->m_nOriginalPrice));
				object->SetString("ItemCost1",UITextParser::HTMLMoneyIconString(MONEYTYPE_04, pkItem->m_nCurrentPrice));
				object->SetInt("ItemCostValue",pkItem->m_nCurrentPrice);
				object->SetString("ItemDescrible",itemTemp->Description);
				object->SetInt("ItemTag",pkItem->m_byFlag);
				object->SetInt("ItemType",pkItem->m_nRenQi);
				object->SetInt("ItemNum",pkItem->m_shItemCount);
				object->SetInt("ItemIndex",1);

				UGFxObject* pkItemIcon = GetObject(TEXT("buyGoodsIcon_mc"));
				if(pkItemIcon != NULL)
				{
					GFXPlayer->SetItemSlotData(pkItemIcon,itemTemp,ESPType_MallStore,-1,-1);
					UGFxObject* pAmount = pkItemIcon->GetObject(TEXT("valueAmount"));
					ASSERT_CHECK_ERROR_RETURN(pAmount);
					pAmount->SetText(appItoa(pkItem->m_shItemCount));
				}

				eventCallUpdateGoodsData(object);
				eventCallUpdateGoldMoney(MallManager::getSingleton().GetRealMoney());
				return;
			}
		}
	}
	Show(FALSE);
}
UBOOL UMMOUIMallBuyPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return TRUE;
}
UBOOL UMMOUIMallBuyPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return TRUE;
}
UBOOL UMMOUIMallBuyPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return TRUE;
}
UBOOL UMMOUIMallBuyPanel::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIMallBuyPanel::ASClickGiveFriend(INT nGoodsID,INT nNum)
{
	MallManager::getSingleton().TestMallInfo();
	if(!friendName.IsEmpty())
	{
		friendName = TEXT("");
		
		eventCallSetBuyAimPlayerName(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.GiveFriendTextHint")));
		if(giveBtn)
			giveBtn->SetString( TEXT("label"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.GiveFriendBtnText1")));
		if(buyBtn)
			buyBtn->SetString( TEXT("label"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.BuyBtnText1")));

		UMMOUIMallFriendPanel* panel = (UMMOUIMallFriendPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_MallFriend);
		if (panel)
		{
			panel->Show(FALSE);
		}
	}
	else
	{
		MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_MallFriend,TRUE, "");
		UMMOUIMallFriendPanel* panel = (UMMOUIMallFriendPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_MallFriend);
		if (panel)
		{
			panel->OnShowFriendPanel(nGoodsID,nNum);
		}
	}
}

void UMMOUIMallBuyPanel::ASClickBuyGoods(INT nGoodsID,INT nNum)
{
	//MallController::getSingleton().NotifyBuyResult(1010001);
	ConfirmMallBuyHint();
}

void UMMOUIMallBuyPanel::ConfirmMallBuyHint()
{
	UMMOUIMallPanel* mallPanel = (UMMOUIMallPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Mall);
	if (!mallPanel)
	{
		return;
	}

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("ConfirmMallBuy");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("CancelMallBuy");	

	const REAL_MONEY_ITEM_INFO*	pkGoodsInfo	= MallManager::getSingleton().GetGoodsByID(mallPanel->GoodsID);
	if(pkGoodsInfo == NULL)
		return;
	UItem* pkItem = MMOFacade::getSingleton().FindItemTemplate( pkGoodsInfo->m_nItemTemplateID );
	if(pkItem == NULL)
		return;

	FString strInfo(TEXT(""));
	INT nNum = GetGoodsNum();
	if(friendName.IsEmpty())
	{
		strInfo = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsBuyself1"));
		strInfo += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkItem->ItemColor));
		strInfo += pkItem->GetDisplayName();
		strInfo += UITextParser::HTMLEndFont();
		strInfo += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsBuyNum")),nNum);
		strInfo += UITextParser::HTMLNewLine();
		strInfo += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsBuyCost"));
		strInfo += UITextParser::HTMLMoneyIconString(MONEYTYPE_04, pkGoodsInfo->m_nCurrentPrice*nNum);
		strInfo += "\n";
		strInfo += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsConfirmBuy1"));
	}
	else
	{
		strInfo = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsGive"));
		strInfo += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkItem->ItemColor));
		strInfo += pkItem->GetDisplayName();
		strInfo += UITextParser::HTMLEndFont();
		strInfo += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsBuyNum")),nNum);
		strInfo += UITextParser::HTMLNewLine();
		strInfo += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsBuyCost"));
		strInfo += UITextParser::HTMLMoneyIconString(MONEYTYPE_04, pkGoodsInfo->m_nCurrentPrice*nNum);
		strInfo += "\n";
		strInfo += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.MallStore.GoodsConfirmGive"));
		strInfo += friendName;
		strInfo += TEXT("?");
	}
	MallBuyConfirmBox = MMOMsgBoxManager::getSingleton().AddModalMessageBox(strInfo/*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Mall.BuyConfirmHint"))*/, 0, funYes, funNo);
}

void UMMOUIMallBuyPanel::ConfirmMallBuy(class UGFxObject* mc)
{
	if(mc == NULL)
		return;

	UMMOUIMallPanel* panel = (UMMOUIMallPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Mall);
	if (panel)
	{
		if(panel->GoodsID > 0)
		{
			if(friendName.IsEmpty())
				MallController::getSingleton().SendBuyGoods(panel->GoodsID, GetGoodsNum());
			else
				MallController::getSingleton().SendGiveFriendGoods(friendName, panel->GoodsID, GetGoodsNum());
		}
	}

	MallBuyConfirmBox = NULL;
}
void UMMOUIMallBuyPanel::CancelMallBuy(class UGFxObject* mc)
{
	if(mc == NULL)
		return;
	MallBuyConfirmBox = NULL;
}

INT UMMOUIMallBuyPanel::GetGoodsNum()
{
	INT nCount = 1;
	if(textNum)
		nCount = appAtoi(*(textNum->GetText()));
	return nCount;
}