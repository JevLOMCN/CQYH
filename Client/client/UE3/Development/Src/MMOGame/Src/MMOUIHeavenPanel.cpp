#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include <time.h>
#include "MMOMsgButtonManager.h"
#include "MMOMsgBoxManager.h"
#include "MMOFacade.h"
#include "MMOHeavenManager.h"
#include "MMOGuideManager.h"
#include "UITextParser.h"


IMPLEMENT_CLASS(UMMOUIHeavenPanel)

void UMMOUIHeavenPanel::ConverterConfirmShow()
{
	// Tutorial
	MMOGameEvent_Guide evtGuide(EGUET_SoulToPoint);
	MMOUI::PostGameEvent(&evtGuide);

	FScriptDelegate funYes(EC_EventParm);
	funYes.Object = this;
	funYes.FunctionName = TEXT("ConfirmConverter");

	FScriptDelegate funNo(EC_EventParm);
	funNo.Object = this;
	funNo.FunctionName = TEXT("CancelConverter");	

	INT  nNeedMoney = MMOHeavenManager::getSingleton().GetNeedMoneyConvert();
	if(nNeedMoney == 0)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.ConvertFirstFree"));
		ConverterConfirmBox = MMOMsgBoxManager::getSingleton().AddModalMessageBox(text, 0, funYes, funNo);
	}
	else if(nNeedMoney > 0)
	{
		FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasConverterConfirm")),*UITextParser::HTMLMoneyIconString(MONEYTYPE_02, nNeedMoney));
		ConverterConfirmBox = MMOMsgBoxManager::getSingleton().AddModalMessageBox(text, 0, funYes, funNo);
	}
}
void UMMOUIHeavenPanel::ConfirmConverter(class UGFxObject* mc)
{
	if(mc == NULL)
	{
		return;
	}
	CMessageHandler::GetInstance().Send_HeavenConverter();
	ConverterConfirmBox = NULL;
}
void UMMOUIHeavenPanel::CancelConverter(class UGFxObject* mc)
{
	if(mc == NULL)
	{
		return;
	}
	ConverterConfirmBox = NULL;
}
void UMMOUIHeavenPanel::UpdateHeavenMain()
{
	UpdateSoulValue();
	UpdateXiuLianValue();
	UpdateHeavenExpBar();
	UpdateHeavenLevel();
	UpdateHeavenStar();
	InitTipsContent();
}
void UMMOUIHeavenPanel::UpdateSoulValue()
{
	INT levelUpSoulPts = MMOFacade::getSingleton().GetPlayerMoney( E_MONEYTYPE_SOUL_PTS );
	FString nValueName =  FString::Printf(TEXT("%d"),levelUpSoulPts);
	eventCallUpdateSoulValue(nValueName);
}
void UMMOUIHeavenPanel::UpdateXiuLianValue()
{
	INT levelUpPts = MMOFacade::getSingleton().GetPlayerMoney( E_MONEYTYPE_SPELL_PTS );
	FString nValueName =  FString::Printf(TEXT("%d"),levelUpPts);
	eventCallUpdateXiulianValue(nValueName);

	INT nGasMoney = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_AIR);
}
void UMMOUIHeavenPanel::UpdateNextConverter(UGFxObject* nextData)
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL || mainPlayer->ActorAttributes == NULL)
		return;
	INT nLevel = mainPlayer->ActorAttributes->heavenAttributes.heavenLevel;
	const HeavenLevelConfig*pkData = MMOHeavenManager::getSingleton().GetHeavenLevelData(nLevel);
	if(pkData == NULL)
		return;
	FString nValueName0 =  FString::Printf(TEXT("%d"),pkData->nSoulCost);
	FString nValueName1 =  FString::Printf(TEXT("%d"),MMOHeavenManager::getSingleton().GetNextLevelPts());
	FString nValueName2 =  FString::Printf(TEXT("%d"),MMOHeavenManager::getSingleton().GetNextLevelExp());
	if(nextData)
	{
		INT nLastTime = MMOHeavenManager::getSingleton().GetNeedTime();
		nextData->SetInt("nNeedTime",nLastTime);
		nextData->SetString("nCostSoulValue",nValueName0);
		nextData->SetString("nGetXiuLian",nValueName1);
		nextData->SetString("nGetHeavenExp",nValueName2);

		BOOL bVip = (mainPlayer->GetVipType()>0);
		nextData->SetBool("bVIPSpeedup",bVip);
		nextData->SetBool("bOutLineConvert",bVip);

		INT levelUpSoulPts = MMOFacade::getSingleton().GetPlayerMoney( E_MONEYTYPE_SOUL_PTS );
		BOOL bNotEnough = levelUpSoulPts < pkData->nSoulCost? TRUE:FALSE;
		nextData->SetBool("bSoulNotEnough",bNotEnough);
		eventCallUpdateNextConvertInfo(nextData);
	}

	FString strTimes = FString::Printf(TEXT("%d/%d"),MMOHeavenManager::getSingleton().GetTodayConvertTimes(),MMOHeavenManager::getSingleton().GetTodayConvertMaxTimes());
	eventCallUCSetConvertTimes(strTimes);
}
void UMMOUIHeavenPanel::UpdateHeavenExpBar()
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL || mainPlayer->ActorAttributes == NULL)
		return;
	const HeavenLevelConfig* pkData = MMOHeavenManager::getSingleton().GetHeavenLevelData(mainPlayer->ActorAttributes->heavenAttributes.heavenLevel);
	if(pkData)
	{
		if(mainPlayer->ActorAttributes->heavenAttributes.heavenLevel < MAX_SPIRIT_LEVEL)
			eventCallUpdateExpProBar(mainPlayer->ActorAttributes->heavenAttributes.heavenExp,pkData->nMaxExp);
		else
			eventCallUpdateExpProBar(pkData->nMaxExp,pkData->nMaxExp);
		eventCallUCShowMaxLevelWarnText(mainPlayer->ActorAttributes->heavenAttributes.heavenLevel >= MAX_SPIRIT_LEVEL);
	}	
}

void UMMOUIHeavenPanel::UpdateHeavenLevel()
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL || mainPlayer->ActorAttributes == NULL)
		return;
	INT nLevel = mainPlayer->ActorAttributes->heavenAttributes.heavenLevel;
	INT nStarTitleIndex = MMOHeavenManager::getSingleton().GetStarTitleIndex(nLevel);
	eventCallUpdateStateLevel(nStarTitleIndex,MMOHeavenManager::getSingleton().GetLevelString(nLevel));
	FString strLevel = FString::Printf(TEXT("Lv %d"),nLevel);

	const HeavenLevelConfig* pkLevel =  MMOHeavenManager::getSingleton().GetHeavenLevelData(nLevel);
	if(pkLevel)
	{
		FString strNode1,strNode2;
		MMOHeavenManager::getSingleton().GetGasSkillArr(strNode1,pkLevel->nAtriNodeID1);
		MMOHeavenManager::getSingleton().GetGasSkillArr(strNode2,pkLevel->nAtriNodeID2);
		eventCallUCUpdateStateAll(strLevel,strNode1,strNode2);
	}
}
void UMMOUIHeavenPanel::UpdateHeavenStar()
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL || mainPlayer->ActorAttributes == NULL)
		return;
	INT nLevel =  mainPlayer->ActorAttributes->heavenAttributes.heavenLevel;
	//INT nStar = MMOHeavenManager::getSingleton().GetStarNum(nLevel);
	eventCallUpdateStateLevelStarNum(0/*nStar*/,""/*MMOHeavenManager::getSingleton().GetStarTitle(nLevel)*/);
}
void UMMOUIHeavenPanel::UpdateHeavenLogInfo(UMMOGFXPlayer* gfxPlayer)
{
	if(gfxPlayer == NULL)
		return;
	UGFxObject* dataArray = gfxPlayer->CreateArray();
	if (dataArray == 0)
		return;
	INT  nIndex = 0;

	const TArray<FString>& tArrlog = MMOHeavenManager::getSingleton().GetConverterLog();

	for (INT n = 0; n < tArrlog.Num(); ++n)
	{
		dataArray->SetElementString(n ,tArrlog(n));
	}
	eventCallUpdateHeavenLog(dataArray);
}

void UMMOUIHeavenPanel::InitTipsContent()
{

}
UBOOL UMMOUIHeavenPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}
UBOOL UMMOUIHeavenPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUIHeavenPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	pkSoulSlot = GetObject(TEXT("soulBottle_mc"));
	if(pkSoulSlot&&pkSoulSlot == mc)
	{
		AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
		UItem* tItem = Player->ItemManager->GetItemBySlot( E_CONSLOT_DEPOSIT , 0 );
		if(tItem)
		{
			INT nDesSlot = Player->ItemManager->GetEmptyItemSlot(E_CONSLOT_DEFAULT);
			if(nDesSlot >= 0)
				CMessageHandler::GetInstance().Send_MoveItem(E_CONSLOT_DEFAULT,nDesSlot, E_CONSLOT_DEPOSIT,0 );
			else
			{
			}
		}
	}
	//If equipping here, unequip the soul bottle
	return FALSE;
}
UBOOL UMMOUIHeavenPanel::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC,BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	//If dragging a soul bottle, load the soul bottle
	if(pkSoulSlot == NULL || pkSoulSlot != DestSlotMC)
		return FALSE;
	if(SrcContainerID == E_CONSLOT_DEFAULT&&DestContainerID == E_CONSLOT_DEPOSIT)
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEFAULT, SrcSlotID);
		if(info)
		{
			UItem* itemTemplate = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
			if(itemTemplate&&itemTemplate->ItemType == TEXT("ITEMTYPE_DEPOSIT") )
			{
				CMessageHandler::GetInstance().Send_MoveItem( E_CONSLOT_DEPOSIT,0,E_CONSLOT_DEFAULT,SrcSlotID );
			}
		}
	}
	return FALSE;
}
void UMMOUIHeavenPanel::UpdateSlotState(UMMOGFXPlayer* gfxPlayer)
{
	pkSoulSlot = GetObject(TEXT("soulBottle_mc"));
	if(pkSoulSlot == NULL || gfxPlayer == NULL)
		return;

	FClientItemInfo* info = MMOFacade::getSingleton().FindItem(E_CONSLOT_DEPOSIT, 0);
	if(info)
	{
		UItem* itemTemplate = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
		gfxPlayer->SetItemSlotData(pkSoulSlot,itemTemplate,ESPType_HeavenSoul,E_CONSLOT_DEPOSIT,0);
		eventCallUCSetSoulBottleProgress(info->m_nCurDur,info->m_nMaxDur);
	}
	else
	{
		eventCallUCSetSoulBottleProgress(0,0);
		gfxPlayer->SetItemSlotData(pkSoulSlot,NULL,ESPType_HeavenSoul,E_CONSLOT_DEPOSIT,0);
	}
}