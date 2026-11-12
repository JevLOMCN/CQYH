#include "stdafx.h"
#include "ClientActionConditionCheck.h"
#include "MMOFacade.h"
#include "net/message_handler.h"
#include "MMOChatManager.h"
#include "MMOUtil.h"
#include "MMOMapDataManager.h"
#include "MMOAttributeAccountManager.h"

typedef std::map<EClientActionConditionCheckType, CheckActionConditionFun, M_UE3_STL_ALLOC_MAP(EClientActionConditionCheckType, CheckActionConditionFun) > ClientActionInformationNotifyMap;

ClientActionInformationNotifyMap g_ClientActionInformationNotifyMap;

FClientActionConditionContex::FClientActionConditionContex( EClientActionConditionCheckType checkType /*= ECACCT_None*/ ) : ChceckType(checkType)
	, Item(0)
	, ItemTemplate(0)
	, NPC(0)
	, PlayerActor(0)
{
	if(Item && !ItemTemplate)
	{
		ItemTemplate = MMOFacade::getSingleton().FindItemTemplate(Item->m_nTemplateID);
	}
}


void InitClientActionInformationNotify()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("InitClientActionInformationNotify"), 0);

#define REG_ACTION_INFO_NOTIFY(fun) g_ClientActionInformationNotifyMap[fun] = Check_##fun;

	REG_ACTION_INFO_NOTIFY(ECACCT_EquipItem_CheckClass);
	REG_ACTION_INFO_NOTIFY(ECACCT_EquipItem_CheckLv);

	REG_ACTION_INFO_NOTIFY(ECACCT_ALL_ActorLevel);
	
	REG_ACTION_INFO_NOTIFY(ECACCT_Hero_EquipItem_CheckClass);
	REG_ACTION_INFO_NOTIFY(ECACCT_Hero_EquipItem_CheckLv);

	REG_ACTION_INFO_NOTIFY(ECACCT_UseItem_CheckClass);
	REG_ACTION_INFO_NOTIFY(ECACCT_UseItem_CheckLv);
	REG_ACTION_INFO_NOTIFY(ECACCT_UseItem_CheckPhyAtk);
	REG_ACTION_INFO_NOTIFY(ECACCT_UseItem_CheckMagAtk);
	REG_ACTION_INFO_NOTIFY(ECACCT_UseItem_CheckTaoAtk);
	REG_ACTION_INFO_NOTIFY(ECACCT_UseItem_CheckStallState);

	REG_ACTION_INFO_NOTIFY(ECACCT_MailSend_CheckReceiver);
	REG_ACTION_INFO_NOTIFY(ECACCT_MailSend_CheckReceiverSelf);
	REG_ACTION_INFO_NOTIFY(ECACCT_MailSend_CheckSubject);
	REG_ACTION_INFO_NOTIFY(ECACCT_MailSend_CheckContent);
	REG_ACTION_INFO_NOTIFY(ECACCT_MailSend_CheckContentMaxLen);
	REG_ACTION_INFO_NOTIFY(ECACCT_UseItem_CheckBind);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckAllInContainerIfInPanelUsing);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckInEnhance);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckInWeaponExp);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckInTrade);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckInSoulEmbed);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckInSoulUnEmbed);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckInSoulLevelUp);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckInPanelUsing);
	REG_ACTION_INFO_NOTIFY(ECACCT_TradeItem_CheckDurablility);
	REG_ACTION_INFO_NOTIFY(ECACCT_TradeItem_CheckTaskItem);
	REG_ACTION_INFO_NOTIFY(ECACCT_TradeItem_CheckItemSign);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckInEnhanceOpened);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckInTradeOpend);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckCanStall);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckCanTrade);
	REG_ACTION_INFO_NOTIFY(ECACCT_Item_CheckMySex);

	REG_ACTION_INFO_NOTIFY(ECACCT_CurrentScene_Has_Lines);
	REG_ACTION_INFO_NOTIFY(ECACCT_MainPlayer_Is_Combating);
	REG_ACTION_INFO_NOTIFY(ECACCT_MainPlayer_Is_Trading);
	REG_ACTION_INFO_NOTIFY(ECACCT_EquipLevelUp_CheckDurability);

	REG_ACTION_INFO_NOTIFY(ECACCT_Material_CheckEquipTool);
	REG_ACTION_INFO_NOTIFY(ECACCT_Material_WeaponDuration);
	REG_ACTION_INFO_NOTIFY(ECACCT_Material_BagFull);
}

void UninitClientActionInformationNotify()
{
	g_ClientActionInformationNotifyMap.clear();
}

FString GetMsgFormatString(EClientActionConditionCheckType e, FString& keyname)
{
	static UEnum* enumObj = FindObject<UEnum>(UTypeDefine::StaticClass(), TEXT("EClientActionConditionCheckType"), TRUE);
	keyname = FString(TEXT("MMOErrorInfo.EClientActionConditionCheckType.")) + enumObj->GetEnum(e).ToString();
	return MMOUI::ParseLocalizedPropertyPath(keyname);
}

bool CheckClientActionCondition( FClientActionConditionContex& contex )
{
	ClientActionInformationNotifyMap::iterator i = g_ClientActionInformationNotifyMap.find(contex.ChceckType);
	if(i != g_ClientActionInformationNotifyMap.end())
	{
		if((*i->second)(contex))
		{
			return true;
		}
		else
		{
			MMOChatManager::getSingleton().AddSystemMessage(contex.ResultMessage);

			UBOOL bRet = FALSE;
			FString localStr = MMOUI::ParseLocalizedPropertyPath(contex.ResultName, bRet);
#if SHIPPING_PC_GAME
			if(bRet)
			{
				MMOUI::GetUIManager()->delegateAddScreenMessage(localStr,MsgInfoType_Table,contex.ResultName);
			}
#else
			MMOUI::GetUIManager()->delegateAddScreenMessage(localStr,MsgInfoType_Table,contex.ResultName);
#endif		
			return false;
		}
	}
	else
	{
		warnf(NAME_Warning, TEXT("[InterpretClientActionInformationNotify] notify type not found:%d"), (INT)contex.ChceckType);
	}

	return false;
}

bool ClientActionConditionSequence::CheckClientActionCondition( FClientActionConditionContex& contex )
{
	FClientActionConditionContexList::iterator i = _ConditionCheckList.begin();
	for (; i != _ConditionCheckList.end(); ++i)
	{
		// Return as soon as one does not match
		contex.ChceckType = *i;
		if(!::CheckClientActionCondition(contex))
		{
			return false;
		}
	}

	return true;
}

void ClientActionConditionSequence::AddConditionCheckType( EClientActionConditionCheckType checkType )
{
	_ConditionCheckList.push_back(checkType);
}

bool Check_ECACCT_UseItem_CheckClass( FClientActionConditionContex& contex )
{
	if(contex.Item && contex.ItemTemplate)
	{
		//If it is a synthesis scroll, it can definitely be used
		if(contex.ItemTemplate->ItemType == TEXT("ITEMTYPE_FORMULA"))
		{
			return true;
		}

		EPlayerClass playerClass = (EPlayerClass)MMOFacade::getSingleton().GetMainPlayer()->ActorAttributes->ClassType;
		if(MMO::CanBeUsedByPlayerClass(playerClass, contex.ItemTemplate->RequireClass))
		{
			return true;
		}		
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}
//=========================================================================
bool Check_ECACCT_UseItem_CheckLv( FClientActionConditionContex& contex )
{
	if(contex.Item && contex.ItemTemplate)
	{
		INT playerLv = MMOFacade::getSingleton().GetMainPlayer()->GetActorLevel();
		if(playerLv >= contex.ItemTemplate->RequireLevel)
		{
			return true;
		}
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}
bool Check_ECACCT_ALL_ActorLevel( FClientActionConditionContex& contex )
{
	if(contex.Item && contex.ItemTemplate)
	{
		INT playerLv = MMOFacade::getSingleton().GetMainPlayer()->GetActorLevel();
		if(playerLv >= s_ActorLvAllowGameSystem)
		{
			return true;
		}
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_UseItem_CheckPhyAtk(FClientActionConditionContex& contex)
{
	UItem *pItem = contex.ItemTemplate;
	UEquip *pEquip = Cast<UEquip>( pItem );
	if( pEquip != NULL )
	{
		INT curPhyAtk = MMOAttributeAccountManager::getSingleton().GetOwnerAttribute( catMaxPhyAtk, AttrListenerType_Mine );
		if( curPhyAtk >= pEquip->RequireMaxPhyAtk )
		{
			return true;
		}
	}
	else if( pItem != NULL ){
		return true;
	}

	contex.ResultMessage = GetMsgFormatString( contex.ChceckType, contex.ResultName );
	return false;
}

bool Check_ECACCT_UseItem_CheckMagAtk(FClientActionConditionContex& contex)
{
	UItem *pItem = contex.ItemTemplate;
	UEquip *pEquip = Cast<UEquip>( pItem );
	if( pEquip )
	{
		INT curMagAtk = MMOAttributeAccountManager::getSingleton().GetOwnerAttribute( catMaxMagAtk, AttrListenerType_Mine );
		if( curMagAtk >= pEquip->RequireMaxMagAtk )
		{
			return true;
		}
	}
	else if( pItem != NULL ){
		return true;
	}

	contex.ResultMessage = GetMsgFormatString( contex.ChceckType, contex.ResultName );
	return false;
}

bool Check_ECACCT_UseItem_CheckTaoAtk(FClientActionConditionContex& contex)
{
	UItem *pItem = contex.ItemTemplate;
	UEquip *pEquip = Cast<UEquip>( pItem );
	if( pEquip )
	{
		INT curTaoAtk = MMOAttributeAccountManager::getSingleton().GetOwnerAttribute( catMaxTaoAtk, AttrListenerType_Mine );
		if( curTaoAtk >= pEquip->RequireMaxTaoAtk )
		{
			return true;
		}
	}
	else if( pItem != NULL ){
		return true;
	}

	contex.ResultMessage = GetMsgFormatString( contex.ChceckType, contex.ResultName );
	return false;
}
//=========================================================================
bool Check_ECACCT_Item_CheckMySex( FClientActionConditionContex& contex )
{
	UItem *pItem = contex.ItemTemplate;
	
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if( pItem == NULL || mainPlayer == NULL ){
		return false;
	}
	if( pItem->RequireGender == EGT_None ){
		return true;
	}

	UActorAttribute* pActorAttributes = mainPlayer->ActorAttributes;
	if( pActorAttributes == NULL ){
		return false;
	}

	EGenderType gender = (EGenderType)pActorAttributes->GenderType;
	if( gender == pItem->RequireGender ){
		return true;
	}

	contex.ResultMessage = GetMsgFormatString( contex.ChceckType, contex.ResultName );
	return false;
}
//=========================================================================
bool Check_ECACCT_Hero_UseItem_CheckClass( FClientActionConditionContex& contex )
{
	if(contex.Item && contex.ItemTemplate && MMOFacade::getSingleton().GetMainPlayer()->HeroAttributes)
	{
		EPlayerClass playerClass = (EPlayerClass)MMOFacade::getSingleton().GetMainPlayer()->HeroAttributes->ClassType;
		if(MMO::CanBeUsedByPlayerClass(playerClass, contex.ItemTemplate->RequireClass))
		{
			return true;
		}		
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_Hero_UseItem_CheckLv( FClientActionConditionContex& contex )
{
	if(contex.Item && contex.ItemTemplate && MMOFacade::getSingleton().GetMainPlayer()->HeroAttributes)
	{
		INT playerLv = (EPlayerClass)MMOFacade::getSingleton().GetMainPlayer()->HeroAttributes->LevelAttributes.Level;
		if(playerLv >= contex.ItemTemplate->RequireLevel)
		{
			return true;
		}
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}


bool Check_ECACCT_Hero_EquipItem_CheckClass( FClientActionConditionContex& contex )
{
	return Check_ECACCT_Hero_UseItem_CheckClass(contex); // Only the ENUM differs
}

bool Check_ECACCT_Hero_EquipItem_CheckLv( FClientActionConditionContex& contex )
{
	return Check_ECACCT_Hero_UseItem_CheckLv(contex); // Only the ENUM differs
}

bool Check_ECACCT_EquipItem_CheckClass( FClientActionConditionContex& contex )
{
	return Check_ECACCT_UseItem_CheckClass(contex); // Only the ENUM differs
}

bool Check_ECACCT_EquipItem_CheckLv( FClientActionConditionContex& contex )
{
	return Check_ECACCT_UseItem_CheckLv(contex); // Only the ENUM differs
}

bool Check_ECACCT_UseItem_CheckStallState( FClientActionConditionContex& contex )
{
	if(contex.PlayerActor == NULL)
	{
		contex.PlayerActor = MMOFacade::getSingleton().GetMainPlayer();
	}

	if(contex.PlayerActor->MoveType != EMT_PrepareStall
		&& contex.PlayerActor->MoveType != EMT_InStall)
	{
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_Item_CheckCanStall(FClientActionConditionContex& contex)
{
	if(contex.ItemTemplate)
	{
		// #21536 Quest items are allowed to be traded
		if(contex.ItemTemplate->CanSell /*&& contex.ItemTemplate->ItemType != TEXT("ITEMTYPE_QUEST") && contex.ItemTemplate->Trade*/ )
		{
			return true;
		}
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_Item_CheckCanTrade(FClientActionConditionContex& contex)
{
	return true;
	//if(contex.ItemTemplate)
	//{
	//	if(contex.ItemTemplate->Trade)
	//	{
	//		return true;
	//	}
	//}

	//contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	//return false;
}



bool Check_ECACCT_UseItem_CheckBind( FClientActionConditionContex& contex )
{
	if(contex.Item)
	{
		if(contex.Item->m_byBindType == E_BINDTYPE_BINDED)
		{
			contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
			return false;
		}
	}

	return true;
}

bool Check_ECACCT_MailSend_CheckReceiver( FClientActionConditionContex& contex )
{
	if(contex.MailReceiver.Len() > 0)
	{
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_MailSend_CheckReceiverSelf( FClientActionConditionContex& contex )
{
	if(contex.MailReceiver != contex.SelfPlayerName)
	{
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_MailSend_CheckSubject( FClientActionConditionContex& contex )
{
	if(contex.MailSubject.Len() > 0)
	{
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_MailSend_CheckContent( FClientActionConditionContex& contex )
{
	if(contex.MailContent.Len() > 0)
	{
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_MailSend_CheckContentMaxLen( FClientActionConditionContex& contex )
{
	if(contex.MailContent.Len() < MAIL_CONTENT_LEN)
	{
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_Item_CheckInEnhance( FClientActionConditionContex& contex )
{
	UMMOUIEquipEnhance *pEnhanceUi = (UMMOUIEquipEnhance*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_EquipEnhance );
	bool canUse = true;

	if(pEnhanceUi && contex.Item)
	{
		canUse = !pEnhanceUi->CheckInUse(contex.Item->m_byItemSlot);
	}
	//
	if( !canUse ){
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	}

	return canUse;
}

bool Check_ECACCT_Item_CheckInTrade( FClientActionConditionContex& contex )
{
	UMMOUITradeP2P *pTradeP2PUi = (UMMOUITradeP2P*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_TradeP2P );
	bool canUse = true;

	if(pTradeP2PUi && contex.Item)
	{
		canUse = !pTradeP2PUi->CheckInTrade(contex.Item->m_byConSlot, contex.Item->m_byItemSlot);
	}
	//
	if( !canUse ){
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	}

	return canUse;
}

//
bool Check_ECACCT_Item_CheckInSoulEmbed( FClientActionConditionContex& contex )
{
	bool canUse = true;

	if( contex.Item )
	{
		canUse = ( contex.Item->IsDragIntoOtherUI != ESPType_SoulEmbed );
	}
	//
	if( !canUse ){
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	}

	return canUse;
}

//
bool Check_ECACCT_Item_CheckInSoulUnEmbed( FClientActionConditionContex& contex )
{
	bool canUse = true;

	if( contex.Item )
	{
		canUse = ( contex.Item->IsDragIntoOtherUI != ESPType_SoulUnEmbed );
	}
	//
	if( !canUse ){
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	}

	return canUse;
}

//
bool Check_ECACCT_Item_CheckInSoulLevelUp( FClientActionConditionContex& contex )
{
	bool canUse = true;

	if( contex.Item )
	{
		canUse = ( contex.Item->IsDragIntoOtherUI != ESPType_SoulLevelUp );
	}
	//
	if( !canUse ){
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	}

	return canUse;
}

//
bool Check_ECACCT_Item_CheckAllInContainerIfInPanelUsing( FClientActionConditionContex& contex )
{
	MMOFacade &mmoFacade = MMOFacade::getSingleton();
	AMMOPlayer *mainPlayer = mmoFacade.GetMainPlayer();
	if( mainPlayer == NULL ){
		return false;
	}

	//Poll items
	bool nothingIsInUsing = true;
	INT containerEnum = contex.UsefulInt;
	UINT bagCapability = mainPlayer->ItemManager->CurCapacity( containerEnum );
	for( UINT j = 0; j < bagCapability; ++j){
		const FClientItemInfo *pItemInfo = mmoFacade.FindItem( containerEnum, j );
		//if found, add the amount
		if( pItemInfo != NULL ){
			if( pItemInfo->IsDragIntoOtherUI > 0 ){
				nothingIsInUsing = false;
				break;
			}
		}
	}//end of for

	if( !nothingIsInUsing )
	{
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	}

	return nothingIsInUsing;
}

//
bool Check_ECACCT_Item_CheckInPanelUsing( FClientActionConditionContex& contex )
{
	bool canUse = true;

	if( contex.Item )
	{
		canUse = ( contex.Item->IsDragIntoOtherUI < 0 );
	}
	//
	if( !canUse ){
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	}

	return canUse;
}

bool Check_ECACCT_TradeItem_CheckDurablility( FClientActionConditionContex& contex )
{
	if(contex.Item)
	{
		if(contex.Item->m_nMaxDur <= 2 * EQUIP_DUR_MULTIPLIER)
		{
			contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
			return false;
		}
	}

	return true;
}

bool Check_ECACCT_EquipLevelUp_CheckDurability( FClientActionConditionContex& contex )
{
	if(contex.Item)
	{
		if(contex.Item->m_nMaxDur <= 2 * EQUIP_DUR_MULTIPLIER)
		{
			contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
			return false;
		}
	}

	return true;
}

bool Check_ECACCT_TradeItem_CheckTaskItem( FClientActionConditionContex& contex )
{
	if(contex.ItemTemplate)
	{
		if(contex.ItemTemplate->ItemType == "ITEMTYPE_QUEST")
		{
			contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
			return false;
		}
	}

	return true;
}

bool Check_ECACCT_TradeItem_CheckItemSign(FClientActionConditionContex& contex)
{
	if (NULL != contex.Item && contex.Item->m_dwRoleID != 0)
	{
		contex.ResultMessage = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Bag.ErrorSignatureForbidTrade"));
		return false;
	}
	return true;
}

bool Check_ECACCT_Item_CheckInEnhanceOpened(FClientActionConditionContex& contex)
{
	UMMOUIEquipEnhance *pEnhanceUi = (UMMOUIEquipEnhance*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_EquipEnhance );

	if(pEnhanceUi && pEnhanceUi->IsVisible == FALSE)
	{
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_Item_CheckInWeaponExp(FClientActionConditionContex& contex)
{
	//UMMOUIEquipEnhance *pEnhanceUi = (UMMOUIEquipEnhance*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_EquipEnhance );
	bool isUsing = false;


	if( contex.Item && contex.Item->IsOnWeaponExp)
	{
		isUsing = true;
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	}

	return !isUsing;
}

bool Check_ECACCT_Item_CheckInTradeOpend(FClientActionConditionContex& contex)
{
	UMMOUITradeP2P *pTradeP2PUi = (UMMOUITradeP2P*)MMOUI::FindUIPanel( MMOUI::GetUIManager()->Panel_TradeP2P );

	if(pTradeP2PUi && pTradeP2PUi->IsVisible == FALSE)
	{
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

//
bool Check_ECACCT_CurrentScene_Has_Lines(FClientActionConditionContex& contex)
{
	MMOMapDataManager *pMapDataManager = &MMOMapDataManager::getSingleton();

	//
	FExportMap &curMapInfo = pMapDataManager->GetCurMap();
	UBOOL canChangeLines = ( curMapInfo.ServerMapType == E_est_Line );
	if( canChangeLines == TRUE ){
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}


//
bool Check_ECACCT_MainPlayer_Is_Combating(FClientActionConditionContex& contex)
{
	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();

	if( !pMainPlayer->IsInFighting() ){
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

//
bool Check_ECACCT_MainPlayer_Is_Trading(FClientActionConditionContex& contex)
{
	//AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();

	//if( !pMainPlayer->IsInTrading() ){
	//	return true;
	//}
	AMMOPlayer *pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();

	if( !MMOFacade::getSingleton().IsMainPlayerTrading() ){
		return true;
	}

	contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
	return false;
}

bool Check_ECACCT_Material_BagFull(FClientActionConditionContex& contex)
{
	if (MMOFacade::getSingleton().IsBagFull())
	{
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
		return false;
	}
	return true;
}

bool Check_ECACCT_Material_CheckEquipTool(FClientActionConditionContex& contex)
{
	AMMOPlayer *pMainPlayer  = MMOFacade::getSingleton().GetMainPlayer();
	INT			needWeaponID = 99900040;
	INT			needWeaponID1 = 599900040;
	UEquip	   *pWeapon		 = NULL;

	GConfig->GetInt(TEXT("MineMaterial"), TEXT("NeedEquipToolID"), needWeaponID, GDefaultGameIni);
	
	pWeapon = pMainPlayer->GetWeaponEquip();
	if(pWeapon->ItemId == needWeaponID1)
		return true;
	if (pWeapon == NULL || pWeapon->ItemId != needWeaponID)
	{
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
		return false;
	}
	return true;
}

bool Check_ECACCT_Material_WeaponDuration(FClientActionConditionContex& contex)
{
	FClientItemInfo* itemInfo = MMOFacade::getSingleton().FindItem(E_CONSLOT_EQUIPPED, E_NES_MAIN_HAND);
	if (itemInfo == NULL || itemInfo->m_nCurDur <= 0)
	{
		contex.ResultMessage = GetMsgFormatString(contex.ChceckType, contex.ResultName);
		return false;
	}
	return true;
}


