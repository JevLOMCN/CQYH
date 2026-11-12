#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOVipManager.h"
#include "MallManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIVip)

//================================================================================================
void UMMOUIVip::PreOnShow(UBOOL show)
{
	Super::PreOnShow( show );

	if( IsInitialized == FALSE ){
		return;
	}

	//
	if( show ){
		MMOVipManager::getSingleton().ReadVipPrivilege();
		//
		UpdatePanel();

	}
}
//========================================================================================
void UMMOUIVip::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	if(evt == NULL){
		return;
	}

	switch(evt->GetEventType())
	{
	case GameEvent_VipStateChanged:
		{
			MMOGameEvent_OneInt *vipStateEvt = (MMOGameEvent_OneInt*)evt;

			AMMOActor* actor = MMOFacade::getSingleton().GetCharacter( vipStateEvt->IntData );
			if (actor == NULL){
				break;
			}

			//
			AMMOPlayer *pPlayer = Cast<AMMOPlayer>(actor);
			if( pPlayer != NULL && pPlayer->IsMainPlayer() ){
				UpdateCurVipInfo();
			}
		}
		break;
	}
}
//
void UMMOUIVip::UpdatePanel()
{
	UpdateCurVipInfo();
	UpdateList();
	InitBuyVipInfo();
	//
	UpdateMyYuanBao();
}
//========================================================================================================================================
void UMMOUIVip::PlayerMoneyChange( BYTE MoneyType, INT DeltaMoney )
{
	if( IsVisible && MoneyType == E_MONEYTYPE_04 ){
		UpdateMyYuanBao();
	}
}
//========================================================================================================================================
void UMMOUIVip::UpdateMyYuanBao()
{
	INT curYuanBao = Player->GetMoney( E_MONEYTYPE_04 );
	eventCallAsSetCurYuanBao( UITextParser::HTMLMoneyIconString( E_MONEYTYPE_04, curYuanBao ) );
}
//===================================================================================================================================
void UMMOUIVip::UpdateList()
{
	MMOVipManager &mmoVipMgr = MMOVipManager::getSingleton();

	TArray<FString> privilegeTitle = mmoVipMgr.GetVipPrivilege( eVIPTyp_None );
	TArray<FString> lv3Privilege = mmoVipMgr.GetVipPrivilege( eVIPTyp_LV3 );
	TArray<FString> lv2Privilege = mmoVipMgr.GetVipPrivilege( eVIPTyp_LV2 );
	TArray<FString> lv1Privilege = mmoVipMgr.GetVipPrivilege( eVIPTyp_LV1 );

	//
	INT vipPrivilegeCnt = privilegeTitle.Num();
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	for( INT i = 0; i < vipPrivilegeCnt; ++i){
		UGFxObject* itemObj = GFXPlayer->CreateObject( TEXT("UI_VIP.AS3.VipItemData") );
		//check(itemObj != NULL);	
		if(itemObj == NULL)
			continue;

		itemObj->SetString( TEXT("Vipname"), privilegeTitle(i) );
		itemObj->SetString( TEXT("diamond"), lv3Privilege(i) );
		itemObj->SetString( TEXT("gold"), lv2Privilege(i) );
		itemObj->SetString( TEXT("silver"), lv1Privilege(i) );

		dataProvider->SetElementObject( i, itemObj );
	}

	eventFlashVipPrivilegeList( dataProvider );
}

//===================================================================================================================================
void UMMOUIVip::UpdateCurVipInfo()
{
	MMOVipManager &mmoVipMgr = MMOVipManager::getSingleton();

	eVipType curMyVip = mmoVipMgr.GetMyCurrentVipType();
	
	//
	FString vipStr;
	if( curMyVip == eVIPTyp_None ){
		vipStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.CurNoVip") );
	}
	else{
		FString vipName;
		UINT leftDay;
		UINT leftHour;
		mmoVipMgr.GetVipDescriptionString( curMyVip, vipName, leftDay, leftHour );

		vipStr = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.CurVipDesc") ), *vipName );

		if( leftDay > 0 ){
			vipStr += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.VipLeftDay") ), leftDay );
		}

		if( leftHour > 0 ){
			vipStr += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.VipLeftHour") ), leftHour );
		}
	}

	eventCallAsSetVipTitle( vipStr );

	eventCallAsUpdateVIPShop(curMyVip);
	//eventCallAsSetVipsHint( vipHintStr );
}
//===================================================================================================================================
void UMMOUIVip::InitBuyVipInfo()
{
	MMOVipManager &mmoVipMgr = MMOVipManager::getSingleton();
	MallManager &mmoMallMgr = MallManager::getSingleton();
	MMOFacade &mmoFacade = MMOFacade::getSingleton();

	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	INT itemIdx = 0;
	for( INT i = eVIPTyp_LV1; i < eVIPTyp_LVMax; ++i ){
		INT itemID = mmoVipMgr.GetVipItemID( i );

		//
		UGFxObject* itemObj = GFXPlayer->CreateObject( TEXT("UI_VIP.AS3.VipGoods") );
		//check(itemObj != NULL);	
		if(itemObj == NULL)
			continue;

		itemObj->SetInt( TEXT("ItemID"), i );
		UItem *pItemTemplate = mmoFacade.FindItemTemplate( itemID );
		if( pItemTemplate != NULL ){
			itemObj->SetString( TEXT("ItemName"), pItemTemplate->GetDisplayName() );
			itemObj->SetString( TEXT("ItemIcon"), mmoFacade.GetItemIconNamePersonalized( pItemTemplate ) );
		}

		const REAL_MONEY_ITEM_INFO* pkItem = mmoMallMgr.GetGoodsByID( itemID );
		if( pkItem != NULL ){
			itemObj->SetString( TEXT("ItemCost"), UITextParser::HTMLMoneyIconString( MONEYTYPE_04, pkItem->m_nCurrentPrice ) );
		}
		else{
			itemObj->SetString( TEXT("ItemCost"), TEXT("Expensive") );
		}

		dataProvider->SetElementObject( itemIdx++, itemObj );
	}
	
	eventCallAsSetVipItem( dataProvider );
}
//===================================================================================================================================
void UMMOUIVip::Init()
{
	Super::Init();

	InitMc();

	check(NULL == EventAdapter);
	if (NULL == EventAdapter){
		TArray<EGameEvent> evts;
		evts.AddItem(GameEvent_VipStateChanged);
		EventAdapter = new TGameEventAdapter<UMMOUIVip>(this, evts);
	}
}

//
void UMMOUIVip::InitMc()
{
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIVip::ASBuyVipLevel( INT level )
{
	INT itemID = MMOVipManager::getSingleton().GetVipItemID( level );
	if( itemID <= 0 ){
		return;
	}

	//
	CMessageHandler::GetInstance().Send_MallBuyGoods( itemID, 1 );

}

//===================================================================================
void UMMOUIVip::ASBugYuanBao()
{

}

//===================================================================================
void UMMOUIVip::ASVIPTips( UGFxObject *hintMc )
{
	if( hintMc == NULL ){
		return;
	}

	MMOVipManager &mmoVipMgr = MMOVipManager::getSingleton();

	eVipType curMyVip = mmoVipMgr.GetMyCurrentVipType();
	//
	FString vipHintStr;
	for( INT i = eVIPTyp_LV1; i < eVIPTyp_LVMax; ++i ){
		if( curMyVip == i ){
			continue;
		}
		if( mmoVipMgr.GetMyVipLeftSecond( i ) <= 0 ){
			continue;
		}

		//
		FString thisVipHintStr;
		FString vipName;
		UINT leftDay;
		UINT leftHour;
		mmoVipMgr.GetVipDescriptionString( i, vipName, leftDay, leftHour );

		thisVipHintStr = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.VipTipsFormat") ), *vipName );

		if( leftDay > 0 ){
			thisVipHintStr += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.VipLeftDay") ), leftDay );
		}

		if( leftHour > 0 ){
			thisVipHintStr += FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.VipLeftHour") ), leftHour );
		}

		vipHintStr += thisVipHintStr;
		vipHintStr += TEXT("<br>");
	}

	//
	if( vipHintStr.Len() <= 0 ){
		vipHintStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Vip.HasNoVipTips") );
	}

	//
	hintMc->SetString( TEXT("tipsHtml"), vipHintStr );
	MMOUI::GetUIManager()->delegateShowTipsHTML( TFT_AutoSizeTips, hintMc );
}
//===================================================================================
void UMMOUIVip::ASHideVIPTips()
{
	MMOUI::GetUIManager()->delegateHideTips();
}

void UMMOUIVip::ASVipShopClick(int type)
{
	MMOVipManager::getSingleton().ShowVipShopByType((eVipShopType)type);
}
//===============================================================================================================
//===============================================================================================================