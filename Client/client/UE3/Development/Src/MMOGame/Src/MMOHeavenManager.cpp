
#include "stdafx.h"
#include "MMOHeavenManager.h"
#include "MMOSkillManager.h"
#include "MMOFacade.h"
#include "MMOGameClasses.h"
#include "net/message_handler.h"
#include "CommonGameEvents.h"
#include "MMOMsgButtonManager.h"
#include <time.h>
#include "UITextParser.h"
#include "MMOPowerManager.h"
#include "MMOAchievementManager.h"

MMOHeavenManager::MMOHeavenManager()
{

}
void MMOHeavenManager::Init()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOHeavenManager::Init"), 0);
	LoadHeavenData();
}
void MMOHeavenManager::Shutdown()
{
	Clear();
}
void MMOHeavenManager::Clear()
{
	gasItemTemplateMap.clear();
	heavenConvertList.clear();
	heavenLevelDataMap.clear();
	myGasItemList.clear();
	heavenAttList.clear();
	converterLogLookUp.Empty();
}
void MMOHeavenManager::OnNativeGameEvent( struct MMOGameEvent* evt )
{

}
void MMOHeavenManager::LoadHeavenData()
{
	LoadOpenSlotCondition(TEXT("spirit_slot.txt"));

	MMOHeavenLevelDataManager::getSingleton().InitFromFile(TEXT("spirit.txt"), true);
	MMOHeavenGasItemDataManager::getSingleton().InitFromFile(TEXT("air.txt"), true);
	MMOHeavenBloodDataManager::getSingleton().InitFromFile(TEXT("blood.txt"), true);
	MMOHeavenJustConvertManager::getSingleton().InitFromFile(TEXT("spirit_change.txt"), true);
	curMaxID = 0;
	nHeavenLevel = 0;
	nCurHeavenExp = 0;
	gasPractiseState = 1;
	bLevelLimited = TRUE;
	bSoulLimited  = TRUE;
	bServerWaiting = TRUE;
	pkTargetGasSlot = NULL;
	nCurConvertType = 0;
	nGasOperateType = 0; //0 is invalid  1 obtained by qi cultivation  2 obtained by synthesis 3 obtained by exchange
}

void MMOHeavenManager::Update(FLOAT deltaTime)
{
	CTRL_TICK("MMOHeavenManager");

	UpdateCheckSendNormalConverter();
}
HeavenAttList& MMOHeavenManager::GetHeavenAttributeList()
{
	heavenAttList.clear();
	const HeavenLevelConfig * pkCurLevel = GetHeavenCurLevelData();
	if(pkCurLevel != NULL)
	{
		if(pkCurLevel->nAtriNodeID1 > 0)
			heavenAttList.push_back(pkCurLevel->nAtriNodeID1);
		if(pkCurLevel->nAtriNodeID2 > 0)
			heavenAttList.push_back(pkCurLevel->nAtriNodeID2);
	}
	for (INT n = 0; (n < arrSlotGasEquip.Num())&&(n < UCONST_GasEquipSlotMaxNum); ++n)
	{
		GasItem *pkItem = GetItemBySlot(sasbtEquip,n);
		if(pkItem != NULL)
		{
			HeavenGasItemConfig* pkGas = GetGasTemplateConfig(pkItem->nTempID);
			if(pkGas != NULL)
			{
				if(pkGas->nAtriNodeID1 > 0)
					heavenAttList.push_back(pkGas->nAtriNodeID1);
				if(pkGas->nAtriNodeID2 > 0)
					heavenAttList.push_back(pkGas->nAtriNodeID2);
			}
		}
	}
	return heavenAttList;
}
INT	MMOHeavenManager::GetGasPowerByID(INT nGasID)
{
	INT nPower = 0;
	HeavenGasItemConfig* pkGas = GetGasTemplateConfig(nGasID);
	if(pkGas != NULL)
	{
		if(pkGas->nAtriNodeID1 > 0)
		{
			const ItemRandomAttr* heavenAttr1 = MMOFacade::getSingleton().GetRandomAttributById( pkGas->nAtriNodeID1 );
			if( heavenAttr1 )
				nPower += heavenAttr1->power;	
		}
		if(pkGas->nAtriNodeID2 > 0)
		{
			const ItemRandomAttr* heavenAttr2 = MMOFacade::getSingleton().GetRandomAttributById( pkGas->nAtriNodeID2 );
			if( heavenAttr2 )				
				nPower += heavenAttr2->power;	
		}
	}
	return nPower;
}
INT	MMOHeavenManager::GetEquipGasPower()
{
	INT nPower = 0;
	for (INT n = 0; (n < arrSlotGasEquip.Num())&&(n < UCONST_GasEquipSlotMaxNum); ++n)
	{
		GasItem *pkItem = GetItemBySlot(sasbtEquip,n);
		if(pkItem != NULL)
		{
			nPower += GetGasPowerByID(pkItem->nTempID);
		}
	}
	return nPower;
}
INT	MMOHeavenManager::GetCanEquipSlotID()
{
	//default to the first
	INT nSlotID = 0;
	for (INT n = 0; (n < arrSlotGasEquip.Num())&&(n < UCONST_GasEquipSlotMaxNum); ++n)
	{
		GasItem *pkItem = GetItemBySlot(sasbtEquip,n);
		if(pkItem == NULL)
		{
			nSlotID = n;
			break;
		}
	}
	return nSlotID;
}
BOOL MMOHeavenManager::GetBloodLockState(INT nPage)
{
	if(nPage < MAX_BLOODSEA_PAGE&&nPage >= 0)
	{
		if(nBloodState[nPage] > 0)
			return TRUE;
	}
	return FALSE;
}
const TArray<FString>& MMOHeavenManager::GetConverterLog() 
{ 
	return converterLogLookUp;
};

const HeavenGasTemplateMap& MMOHeavenManager::GetGasItemDataMap()
{
	return gasItemTemplateMap;
}

const GasItem* MMOHeavenManager::GetGasItemBySlot(INT nContainerID, INT nSlotIndex)
{
	return GetItemBySlot(nContainerID,nSlotIndex);
}
const HeavenGasItemConfig*  MMOHeavenManager::GetGasTemplate(INT nTempID)
{
	 HeavenGasTemplateMap::const_iterator itrFind  = gasItemTemplateMap.find(nTempID);
	if(itrFind != gasItemTemplateMap.end())
		return itrFind->second;

	 return NULL;
}
HeavenGasItemConfig* MMOHeavenManager::GetGasTemplateConfig(INT nTempID)
{
	HeavenGasTemplateMap::iterator itrFind  = gasItemTemplateMap.find(nTempID);
	if(itrFind != gasItemTemplateMap.end())
		return itrFind->second;

	return NULL;
}
GasItem* MMOHeavenManager::GetItemByID(INT nID)
{
	GasItemList::iterator itr = myGasItemList.begin();
	for (; itr != myGasItemList.end(); ++itr)
	{
		if(itr->nUniqueID == nID)
			return &(*itr);
	}
	return NULL;
}
const HeavenLevelConfig* MMOHeavenManager::GetHeavenCurLevelData()
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer&&mainPlayer->ActorAttributes)
	{
		return GetHeavenLevelData(mainPlayer->ActorAttributes->heavenAttributes.heavenLevel);
	}
	return NULL;
}
const HeavenLevelConfig* MMOHeavenManager::GetHeavenLevelData(INT nLevel)
{
	HeavenLevelDataMap::const_iterator itrFind = heavenLevelDataMap.find(nLevel);
	if(itrFind != heavenLevelDataMap.end())
		return itrFind->second;

	return NULL;
}
HeavenLevelConfig* MMOHeavenManager::GetHeavenLevelDataIn(INT nLevel)
{
	HeavenLevelDataMap::const_iterator itrFind = heavenLevelDataMap.find(nLevel);
	if(itrFind != heavenLevelDataMap.end())
		return itrFind->second;

	return NULL;
}
const HeavenBloodConfig* MMOHeavenManager::GetHeavenBloodData(INT nPage,INT nSlot,INT nLevel)
{
	HeavenBloodDataList::const_iterator itr = heavenBloodDataList.begin();
	for(; itr != heavenBloodDataList.end(); ++itr)
	{
		if((itr->nPage == nPage)&&(itr->nSlot == nSlot)&&(itr->nLevel == nLevel))
			return &(*itr);
	}
	return NULL;
}
const GasSlot* MMOHeavenManager::GetTargetSlot()const
{
	return pkTargetGasSlot;
}

BloodSlot* MMOHeavenManager::GetBloodSlotByIndex(INT nPage, INT nIndex)
{
	for (INT n = 0; n < arrSlotBlood.Num(); ++n)
	{
		if(arrSlotBlood(n).nPage == nPage && arrSlotBlood(n).nIndex == nIndex)
			return &arrSlotBlood(n);
	}
	return NULL;
}
const BloodSlot* MMOHeavenManager::GetBloodSlot(INT nPage,INT nIndex)const
{
	for (INT n = 0; n < arrSlotBlood.Num(); ++n)
	{
		if(arrSlotBlood(n).nPage == nPage && arrSlotBlood(n).nIndex == nIndex)
			return &arrSlotBlood(n);
	}
	return NULL;
}
INT MMOHeavenManager::GenerateUniqueID()
{
	return ++curMaxID;
}
INT	MMOHeavenManager::GetBtnPractiseState(INT nIndex)
{
	return (gasPractiseState&((0x0001)<<nIndex));
}
GasItem* MMOHeavenManager::GetItemBySlot(INT nContainerID,INT nSlotID)
{
	if(nSlotID < 0 )
		return NULL;
	INT nUniqueID = -1;
	if(nContainerID == sasbtBag)
	{
		if(nSlotID < arrSlotGasBag.Num())
			nUniqueID = arrSlotGasBag(nSlotID).nUniqueID;
	}
	else if (nContainerID == sasbtEquip)
	{
		if(nSlotID < arrSlotGasEquip.Num())
			nUniqueID = arrSlotGasEquip(nSlotID).nUniqueID;
	}
	else if (nContainerID == sasbtStall)
	{
		if(nSlotID < arrSlotGasBank.Num())
			nUniqueID = arrSlotGasBank(nSlotID).nUniqueID;
	}
	return GetItemByID(nUniqueID);
}

INT MMOHeavenManager::GetNeedTime()
{
	if(nNowNeedTime > 10000000 || nNowNeedTime < 0)
		return 0;
	if(bLevelLimited || bSoulLimited)
		return 0;
	return nNowNeedTime;
}
void MMOHeavenManager::NotifyGameStateChange()
{
	if(MMO::GetCurrentGameState() == EGameState_Login || MMO::GetCurrentGameState() == EGameState_Selecting || MMO::GetCurrentGameState() == EGameState_None)
		bServerWaiting = TRUE;

	static BOOL bFirst = FALSE;
	if(MMO::GetCurrentGameState() == EGameState_Login || MMO::GetCurrentGameState() == EGameState_Selecting)
		bFirst = TRUE; 
	if(bFirst&&MMO::GetCurrentGameState() == EGameState_InGame)
	{
		bFirst = FALSE;
		//compute offline time conversion
		CheckSendOutLineConverter(nHeavenLevel);
	}
}
void MMOHeavenManager::NotifyBloodOpen(INT nPage)
{
	if(nPage <= MAX_BLOODSEA_PAGE && nPage >0)
		nBloodState[nPage - 1] = 1;
	MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_BloodStateOpen);
	evt.SlotContainerID = nPage;
	MMOUI::PostGameEvent(&evt);

	// Achievement
	int openPageCount = 0;
	for (int i = 0; i < MAX_BLOODSEA_PAGE; ++ i)
	{
		if (nBloodState[i] == 1)
		{
			openPageCount ++;
		}
	}
	MMOGameEvent_Achievement evtAch(adSpiritBloodPageCount, openPageCount, 0, 0);
	MMOUI::PostGameEvent(&evtAch);
}
void MMOHeavenManager::UpdateCheckSendNormalConverter()
{
	static time_t tLastSend = 0;
	if(bServerWaiting)
	{
		if(!bLevelLimited&&!bSoulLimited&&(tLastSend>0))
		{
			time_t ttN = time(NULL);
			if(ttN - tLastSend > 60)
			{
				bServerWaiting = FALSE;
				CheckSetNeedTime(TRUE); 
			}
		}
	}
	else
		tLastSend = 0;

	//no tick when level is insufficient, soul is insufficient, or waiting for the server response
	if(bLevelLimited || bSoulLimited || bServerWaiting)
		return;

	static INT nLevel = -1;
	static HeavenLevelConfig* pkLevel = NULL;

	if(pkLevel == NULL || nLevel != nHeavenLevel)
	{
		pkLevel = GetHeavenLevelDataIn(nHeavenLevel);
		nLevel = nHeavenLevel;
	}
	if(pkLevel)
	{
		time_t tt = time(NULL);
		UINT32 dwTime = (dwLastExp2PtsTime == 0)? pkLevel->nNeedTime : dwLastExp2PtsTime;
		nNowNeedTime = dwTime - tt + tNowTime;
		if(nNowNeedTime < 0)
		{
			bServerWaiting = TRUE;
			tLastSend = time(NULL);
			CMessageHandler::GetInstance().Send_HeavenNormalConverter();
			dwLastExp2PtsTime = 0;
			nNowNeedTime = 0;
		}
	}
}

UINT32 MMOHeavenManager::GetTodayConvertTimes()
{
	return nDailyExp2PtsTime;
}

UINT32 MMOHeavenManager::GetTodayConvertMaxTimes()
{
	const HeavenLevelConfig* pkLevel =  GetHeavenLevelData(nHeavenLevel);
	if(pkLevel)
		return pkLevel->nDailyExp2PtsMaxCount;
	return 0;
}

INT	MMOHeavenManager::GetNeedMoneyOpenSlot(INT nContainerID, INT nSlotIndex)
{
	INT nMoney = 0;//nSlotIndex is the quantity
	if(nContainerID == sasbtBag)
	{
		for (INT n = arrSlotGasBag.Num(); n < arrSlotGasBag.Num() + nSlotIndex && n < UCONST_GasBagSlotMaxNum;++n)
		{
			nMoney += 50 + 5*(n - 16);
		}
		//nMoney = 50 + 5*(arrSlotGasBag.Num() - 16);
	}
	else if (nContainerID == sasbtStall)
	{
		for (INT n = arrSlotGasBank.Num(); n < arrSlotGasBank.Num() + nSlotIndex && n < UCONST_GasBankSlotMaxNum;++n)
		{
			nMoney += 50 + 5*(n - 8);
		}
		//nMoney = 50 + 5*(arrSlotGasBank.Num() - 8);
	}
	else if (nContainerID == sasbtEquip)
	{
		//if(nSlotIndex < arrSlotTax.Num() && nSlotIndex >=0 && nSlotIndex < UCONST_GasEquipSlotMaxNum)
		//	nMoney = arrSlotTax(nSlotIndex).nMoney;
		if (arrSlotGasEquip.Num() < arrSlotTax.Num())
		{
			nMoney = arrSlotTax(arrSlotGasEquip.Num()).nMoney;
		}
	}
	return nMoney;
}
INT	MMOHeavenManager::GetNeedMoneyConvert()
{
	//first conversion is free
	if(nCurHeavenExp <= 0 && nHeavenLevel == 1)
		return 0;
	HeavenLevelConfig* pkLevel =  GetHeavenLevelDataIn(nHeavenLevel);
	if(pkLevel)
	{
		INT nDiscount = GetJustConvertDiscount();
		INT nNeedTime = (nNowNeedTime/60 + (nNowNeedTime%60 == 0?0:1));
		return (pkLevel->nNeedMoney*nNeedTime*((10000 + nDiscount)/10000));
	}
	return -1;
}

INT MMOHeavenManager::GetStarNum(INT nLevel)
{
	return heavenTitleTool.GetStartNum(nLevel);
}
INT MMOHeavenManager::GetStarTitleIndex(INT nLevel)
{
	return heavenTitleTool.GetStarTitleIndex(nLevel);
}
INT	MMOHeavenManager::GetLevelTitleIndex(INT nLevel)
{
	return heavenTitleTool.GetLevelTitleIndex(nLevel);
}

FString	MMOHeavenManager::GetLevelTitle(INT nLevel)
{
	return heavenTitleTool.GetLevelTitle(nLevel);
}

FString	MMOHeavenManager::GetStarTitle(INT nLevel)
{
	return heavenTitleTool.GetStarTitle(nLevel);
}

FString	MMOHeavenManager::GetLevelString(INT nLevel)
{
	return heavenTitleTool.GetLevel(nLevel);
}

GasSlot* MMOHeavenManager::GetSlotByIndex(INT nContainerID,INT nSlotID)
{
	if(nContainerID == sasbtBag)
	{
		if(nSlotID < arrSlotGasBag.Num())
			return &arrSlotGasBag(nSlotID);
	}
	else if (nContainerID == sasbtEquip)
	{
		if(nSlotID < arrSlotGasEquip.Num())
			return &arrSlotGasEquip(nSlotID);
	}
	else if (nContainerID == sasbtStall)
	{
		if(nSlotID < arrSlotGasBank.Num())
			return &arrSlotGasBank(nSlotID);
	}
	return NULL;
}

void MMOHeavenManager::ChangeTargetGasItem(INT nSrcSlot)
{
	pkTargetGasSlot = GetSlotByIndex(sasbtBag,nSrcSlot);
	GasItem* pkItem = GetItemBySlot(sasbtBag,nSrcSlot);
	if(pkItem == 0)
		pkTargetGasSlot = 0;
	if(pkTargetGasSlot)
		pkTargetGasSlot->nTagID = nSrcSlot;
	MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_TargetSlotState);
	MMOUI::PostGameEvent(&evt);
}

void MMOHeavenManager::RefreshTargetItem()
{
	if(pkTargetGasSlot)
	{
		GasItem* pkItem = GetItemBySlot(sasbtBag,pkTargetGasSlot->nTagID);
		if(pkItem == NULL)
			pkTargetGasSlot = NULL;
		MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_TargetSlotState);
		MMOUI::PostGameEvent(&evt);
	}
}
void MMOHeavenManager::AddNewGas(GasItem& data)
{
	data.nUniqueID = GenerateUniqueID();
	myGasItemList.push_back(data);
}

void MMOHeavenManager::DelGasItem(INT nUniqueID)
{
	GasItemList::iterator itr = myGasItemList.begin();
	for (; itr != myGasItemList.end(); ++itr)
	{
		if(itr->nUniqueID == nUniqueID)
		{
			myGasItemList.erase(itr);
			break;
		}
	}
}

void MMOHeavenManager::SetConvertType(INT nType)
{
	nCurConvertType = nType;
}
INT	MMOHeavenManager::GetConvertType()
{
	return nCurConvertType;
}
FString	MMOHeavenManager::GetConvertStr()
{
	FString hint;
	if(nCurConvertType == (INT)HeavenConvertType_OnLineConvert)
		hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintOnLineConvert"));
	else if (nCurConvertType == (INT)HeavenConvertType_JustConvert)
		hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintJustConvert"));
	else if (nCurConvertType == (INT)HeavenConvertType_OutLineConvert)
		hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintOutLineConverter"));
	return hint;
}
void MMOHeavenManager::SetGasOperateType(INT nType)
{
	nGasOperateType = nType;
	nTempGasOperateType = nType;
	ShowNewGasHint(0);
}
BOOL MMOHeavenManager::GenerateHeavenMergeGasHint(FString& hint)
{
	hint = TEXT("");
	if (pkTargetGasSlot)
	{
		const GasItem* pkItem = GetItemBySlot(sasbtBag,pkTargetGasSlot->nTagID);
		if(pkItem == NULL)
			return FALSE;
		HeavenGasItemConfig* pkCon = GetGasTemplateConfig(pkItem->nTempID);
		if(pkCon == NULL)
			return FALSE;

		INT nExp = 0;
		INT nCount = 0;
		for (INT n = 0; n < arrSlotGasBag.Num(); ++n)
		{
			if(pkTargetGasSlot->nTagID != n)
			{
				const GasItem* pkBagGas = GetItemBySlot(sasbtBag,n);
				if(pkBagGas)
				{
					nCount++;
					nExp += pkBagGas->nCurExp;
				}
			}
		}
		if(nCount == 0)
		{
			hint = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasBagNoGasSorb"));
			return FALSE;
		}
		hint += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkCon->nQuality));
		hint += FString::Printf(TEXT("<p >%s  Lv%d </p>"), *(pkCon->name),pkCon->nLevel);
		hint += UITextParser::HTMLEndFont();

		INT nLevel = GetGasExpLevelUp(pkCon,nExp + pkItem->nCurExp);
		hint += MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HeavenGasMergeHint3"));
		hint += UITextParser::HTMLNewLine();
		hint += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HeavenGasMergeHint1")),nExp);
		if(nLevel > pkCon->nLevel)
		{
			hint += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HeavenGasMergeHint2")),nLevel);
		}
		return TRUE;
	}
	return FALSE;
}
INT	MMOHeavenManager::GetGasExpLevelUp(HeavenGasItemConfig*pkGas,INT nExp)
{
	if(pkGas == NULL)
		return 0;
	HeavenGasItemConfig*pkGasTemp = pkGas;

	INT nCurExp = nExp/* + pkGasTemp->nExp*/;
	INT nLevel = pkGasTemp->nLevel;
	//this loop depends too much on config; a config error could hang in the loop, so protect it
	INT nWhileCircle = 0;
	while(nCurExp >= pkGasTemp->nNextLevelExp)
	{
		pkGasTemp = GetGasTemplateConfig(pkGasTemp->nNextLevelID);
		if(pkGasTemp == NULL)
			break;
		nLevel = pkGasTemp->nLevel;
		if(nLevel >= MAX_AIR_LEVEL)
			break;

		nWhileCircle ++;
		if(nWhileCircle > MAX_AIR_LEVEL)
			break;
	}
	return nLevel;
}
void MMOHeavenManager::CheckSoulValue()
{
	HeavenLevelConfig* pkLevel = GetHeavenLevelDataIn(nHeavenLevel);
	if(pkLevel)
	{
		INT nCurSoul = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_SOUL_PTS);
		bSoulLimited = (nCurSoul < pkLevel->nSoulCost);
	}
}
void MMOHeavenManager::CheckLevelValue()
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer)
		bLevelLimited = (mainPlayer->ActorAttributes->LevelAttributes.Level < SPIRIT_OPEN_LEVEL);
}
void MMOHeavenManager::CheckSetNeedTime(BOOL bForce /*= FALSE*/)
{
	if(bServerWaiting)
		return;
	BOOL bLevel = bLevelLimited;
	BOOL bSoul  = bSoulLimited;

	CheckSoulValue();
	CheckLevelValue();

	if(bLevel != bLevelLimited || bSoul != bSoulLimited || bForce)
	{
		if(!bSoulLimited&&!bLevelLimited)
		{
			HeavenLevelConfig* pkLevel = GetHeavenLevelDataIn(nHeavenLevel);
			if(pkLevel)
			{
				tNowTime = time(NULL);
				nNowNeedTime = (dwLastExp2PtsTime == 0)?pkLevel->nNeedTime:dwLastExp2PtsTime;
				MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_UpdateNeedTime);
				MMOUI::PostGameEvent(&evt);
			}
		}
		else if ((bSoul != bSoulLimited) && bSoulLimited)
		{
			MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_UpdateNeedTime);
			MMOUI::PostGameEvent(&evt);
		}
	}
}
void MMOHeavenManager::SetLastOffLineTime(UINT32 dwTime)
{
	dwLastOfflineTime = dwTime;
}
// check offline conversion
void MMOHeavenManager::CheckSendOutLineConverter(INT nLevel)
{
	// vip right
	
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer&&mainPlayer->GetVipType() <= 0)
		return;

	time_t ttNow = time(NULL);
	if(dwLastOfflineTime == 0) 
		return;
	time_t offLine = ttNow -  dwLastOfflineTime;
	const HeavenLevelConfig* pkDate = GetHeavenLevelData(nLevel);
	if(pkDate)
	{
		if(offLine > pkDate->nNeedTime)
			CMessageHandler::GetInstance().Send_OutLineConverter();
	}
}

void MMOHeavenManager::GetHeavenGasItemTips(FString& tips,INT nContainerID,INT nSlotID)
{
	if(nSlotID == MAX_AIRSEA_BAG_CONTAINER_SIZE && nContainerID == sasbtBag)
	{
		if(pkTargetGasSlot)
			nSlotID = pkTargetGasSlot->nTagID;
	}
	const GasItem* pkItem = GetGasItemBySlot(nContainerID,nSlotID);
	if(pkItem == NULL)
		return;
	const HeavenGasItemConfig* pkTemp = GetGasTemplate(pkItem->nTempID);
	if(pkTemp == NULL)
		return;

	tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkTemp->nQuality));
	tips += FString::Printf(TEXT("<p >%s        Lv%d</p>"), *(pkTemp->name),pkTemp->nLevel);
	tips += UITextParser::HTMLEndFont();
	INT nNextExp = pkItem->nCurExp;
	if(pkTemp->nNextLevelExp > 0)
		nNextExp = pkTemp->nNextLevelExp;
	if(pkTemp->nLevel >= MAX_AIR_LEVEL)
		tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.GasExpMaxHint")),pkItem->nCurExp);
	else
		tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.HeavenGasTipsExp")),pkItem->nCurExp,nNextExp);
	tips += UITextParser::HTMLNewLine();
	
	tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)1));
	tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.GasTipsPower")),GetGasPowerByID(pkItem->nTempID));
	tips += UITextParser::HTMLEndFont();
	tips += UITextParser::HTMLNewLine();

	tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
	GenerateGasTipsByNodeID(tips,pkTemp->nAtriNodeID1,pkTemp->nAtriNodeID2);
	tips += UITextParser::HTMLEndFont();

	if(pkTemp->nJob >= 0)
	{
		tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.HeavenGasTipsJob")),*MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa( pkTemp->nJob))); 
		tips += UITextParser::HTMLNewLine();
	}

	if(pkTemp->nLevel == MAX_AIR_LEVEL)
		return;
	const HeavenGasItemConfig* pkNextTemp = GetGasTemplate(pkTemp->nNextLevelID);
	if(pkNextTemp)
	{
		tips += UITextParser::HTMLNewLine();
		tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.GasTipsNextLevelWork"));
		tips += UITextParser::HTMLNewLine();
		tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
		GenerateGasTipsByNodeID(tips,pkNextTemp->nAtriNodeID1,pkNextTemp->nAtriNodeID2);
		tips += UITextParser::HTMLEndFont();
	}
}
void MMOHeavenManager::GetHeavenGasTipsByGasID(FString& tips,INT nGasID,INT nCurExp)
{
	const HeavenGasItemConfig* pkTemp = GetGasTemplate(nGasID);
	if(pkTemp == NULL)
		return;

	tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkTemp->nQuality));
	tips += FString::Printf(TEXT("<p >%s        Lv%d</p>"), *(pkTemp->name),pkTemp->nLevel);
	tips += UITextParser::HTMLEndFont();
	INT nNextExp = pkTemp->nExp;
	INT nExp = pkTemp->nExp;
	if(nCurExp > 0)
		nExp = nCurExp;
	if(pkTemp->nNextLevelExp > nNextExp)
		nNextExp = pkTemp->nNextLevelExp;
	if(pkTemp->nLevel >= MAX_AIR_LEVEL)
		tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.GasExpMaxHint")),nExp);
	else
		tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.HeavenGasTipsExp")),nExp,nNextExp);
	tips += UITextParser::HTMLNewLine();
	tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)1));
	tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.GasTipsPower")),GetGasPowerByID(nGasID));
	tips += UITextParser::HTMLEndFont();
	tips += UITextParser::HTMLNewLine();

	if(pkTemp->nJob >= 0)
	{
		tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.HeavenGasTipsJob")),*MMOUI::ParseLocalizedPropertyPath( FString( TEXT("MMOGame.School.SchoolName") ) + appItoa( pkTemp->nJob))); 
		tips += UITextParser::HTMLNewLine();
	}
	tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
	GenerateGasTipsByNodeID(tips,pkTemp->nAtriNodeID1,pkTemp->nAtriNodeID2);
	tips += UITextParser::HTMLEndFont();

	if(pkTemp->nLevel == MAX_AIR_LEVEL)
		return;

	const HeavenGasItemConfig* pkNextTemp = GetGasTemplate(pkTemp->nNextLevelID);
	if(pkNextTemp)
	{
		tips += UITextParser::HTMLNewLine();
		tips += MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.GasTipsNextLevelWork"));
		tips += UITextParser::HTMLNewLine();
		tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
		GenerateGasTipsByNodeID(tips,pkNextTemp->nAtriNodeID1,pkNextTemp->nAtriNodeID2);
		tips += UITextParser::HTMLEndFont();
	}
}
void MMOHeavenManager::GenerateGasTipsByNodeID(FString& tips,INT nNondeID1,INT nNodeID2,bool bRow)
{
	BOOL bAdd = GetGasSkillArr(tips,nNondeID1);
	if(bAdd&&bRow)
		tips += TEXT("\n");
	else
		tips += TEXT("  ");
	GetGasSkillArr(tips,nNodeID2);
}
BOOL MMOHeavenManager::GetGasSkillArr(FString& tips,INT nID)
{
	const ItemRandomAttr* pkArr1 = MMOFacade::getSingleton().GetRandomAttributById( nID);
	if(pkArr1 == NULL)
		return FALSE;

	if(pkArr1->formatType == E_cmi_A || pkArr1->formatType == E_cmi_B)
	{
		tips += pkArr1->strText;
		return TRUE;
	}
	else if (pkArr1->formatType == E_cmi_020)
	{
		INT nSkillID = appAtoi((TCHAR*)*(pkArr1->attrType));
		UINT key = MMOSkillManager::CombinateAbilityID(nSkillID,pkArr1->value);
		UMMOAbility* ability = MMOSkillManager::getSingleton().FindAbilityTemplate(key);
		if(ability)
		{
			tips += ability->GetAbilityDetailDescribe();
			//tips += ability->eventGetAbilityDescribe();
			return TRUE;
		}
	}
	return FALSE;
}
void MMOHeavenManager::GetEquipGasPowerDescrible(FString& strPower)
{
	strPower.Empty(0);
	INT nPower = GetEquipGasPower();
	for (INT n = 0; n < arrSlotGasEquip.Num(); ++n)
	{
		const GasItem* pkItem = GetGasItemBySlot(sasbtEquip,n);
		if(pkItem == NULL)
			continue;
		const HeavenGasItemConfig* pkTemp = GetGasTemplate(pkItem->nTempID);
		if(pkTemp == NULL)
			continue;
		if( strPower.IsEmpty())
		{
			strPower += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.GasTipsPowerTitle")),nPower);
			strPower += UITextParser::HTMLNewLine();
		}
		strPower += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)pkTemp->nQuality));
		strPower += FString::Printf(TEXT("<p >%s     Lv%d</p>"), *(pkTemp->name),pkTemp->nLevel);
		strPower += UITextParser::HTMLEndFont();
	}
}
const SlotOpenTaxData* MMOHeavenManager::GetSlotEquipTax(INT nIndex) const
{
	if(nIndex < arrSlotTax.Num())
		return &arrSlotTax(nIndex);
	return NULL;
}
void MMOHeavenManager::GetHeavenBloodTips(FString&tips,INT nPage,INT nSlot)
{
	const BloodSlot* pkSlot = GetBloodSlot(nPage,nSlot);
	if(pkSlot == NULL)
		return;
	if (pkSlot->nValue == 0)
	{
		const BloodSlot* pkLastSlot = GetBloodSlot(nPage,nSlot - 1);
		if(pkLastSlot&&pkLastSlot->nValue == 0)
			tips = TEXT("Unlock the previous slot first");
		else
			tips = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.BloodSlotOPHint2"));
	}
	else if (MAX_BLOODSEA_SLOT_NUMBER == pkSlot->nValue)
	{
		tips += pkSlot->sName;
		tips += " ";
		tips +=  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.BloodSlotOPHint1"));
	}
	else
	{
		tips += pkSlot->sName;
		tips += " ";
		tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.BloodSlotOPHint3")),pkSlot->nValue,MAX_BLOODSEA_SLOT_NUMBER);
	}

	const HeavenBloodConfig* pkConfig = GetHeavenBloodData(nPage,nSlot,pkSlot->nValue);
	if(pkConfig)
	{
		tips += UITextParser::HTMLNewLine();
		tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
		GenerateGasTipsByNodeID(tips,pkConfig->nAttValue1,pkConfig->nAttValue2);
		tips += UITextParser::HTMLEndFont();
	}
	const HeavenBloodConfig* pkNextConfig = GetHeavenBloodData(nPage,nSlot,pkSlot->nValue + 1);
	if(pkNextConfig)
	{
		if(pkNextConfig->nAttValue1 == 0 && pkNextConfig->nAttValue2 == 0)
			return;
		tips += UITextParser::HTMLBeginFont(TEXT("#555555"));
		tips += UITextParser::HTMLNewLine();
		tips +=  FString::Printf(TEXT("Tier %d"),pkSlot->nValue + 1);/*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.BloodSlotOPHint4"));*/
		tips += UITextParser::HTMLNewLine();
		GenerateGasTipsByNodeID(tips,pkNextConfig->nAttValue1,pkNextConfig->nAttValue2);
		tips += UITextParser::HTMLEndFont();
	}
	INT nNeedPts = 0;
	if(pkConfig)
		nNeedPts = pkConfig->nHeavenPtsCost;
	else if(pkNextConfig)
		nNeedPts = pkNextConfig->nHeavenPtsCost;

	if( MAX_BLOODSEA_SLOT_NUMBER != pkSlot->nValue)
	{
		tips += UITextParser::HTMLBeginFont(TEXT("#FF5555"));
		tips += UITextParser::HTMLNewLine();
		tips += FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.BloodTipsNeedXiuLian")),nNeedPts);
		tips += UITextParser::HTMLEndFont();
	}
}
INT MMOHeavenManager::GetJustConvertDiscount()
{
	INT nAddDiscount = 0;
	HeavenConvertList::const_iterator itr = heavenConvertList.begin();
	for (; itr != heavenConvertList.end(); ++itr)
	{
		if((*itr).nTimesCount <= nDailyExp2PtsTime)
		{
			nAddDiscount = (*itr).nCostDiscount;
		}
		else
			break;
	}
	return nAddDiscount;
}
void MMOHeavenManager::ShowNewGasHint(INT nGasID)
{
	if(nGasOperateType == (INT) HeavenGasOperateType_Invalid)
		return;

	static INT nStaticGasID = 0;
	static INT nStaticLevel = 0;
	if(nGasID == 0)
	{
		nStaticGasID = 0;
		nStaticLevel = 0;
		return;
	}
	HeavenGasItemConfig* pkGas = GetGasTemplateConfig(nGasID);
	if(pkGas == NULL)
		return;
	if(nGasOperateType == (INT)HeavenGasOperateType_Merge)
	{
		if(nGasID != nStaticGasID || nStaticLevel != pkGas->nLevel)
		{
			nStaticGasID = nGasID;
			nStaticLevel =  pkGas->nLevel;
			/*
			if(pkTargetGasSlot != NULL)
			{
				pkTargetGasSlot->nTagID    = -1;
				pkTargetGasSlot = NULL;
				MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_TargetSlotState);
				MMOUI::PostGameEvent(&evt);
			}*/
		}
		else
			return;
	}
	else
	{
		nStaticGasID = 0;
		nStaticLevel = 0;
	}
	if(nTempGasOperateType == HeavenGasOperateType_Practise)
	{
		nTempGasOperateType = 0;
		MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_PractiseAnimation);
		MMOUI::PostGameEvent(&evt);
	}
	else if(nTempGasOperateType == HeavenGasOperateType_Merge)
	{
		nTempGasOperateType = 0;
		MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_MergeAnimation);
		MMOUI::PostGameEvent(&evt);
	}
	FString strInfo = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Heaven.HeavenGasHintGet")),*(pkGas->name),pkGas->nLevel);
	MMOUI::GetUIManager()->delegateAddScreenMessage(strInfo,MsgInfoType_Table,TEXT("MMOGame.Heaven.HeavenGasHintGet"));
}
//wrap tips
void MMOHeavenManager::GetHeavenTips(FString& tips,INT nLevel)
{
	tips = TEXT("");
	INT nIndex = nLevel*5 + 1;
	tips += UITextParser::HTMLBeginParagraph();
	tips += FString::Printf(TEXT("<font color='#ffd100'>%s      </font>"),*heavenTitleTool.GetLevelTitle(nIndex));
	tips += UITextParser::HTMLEndParagraph();

	for (INT n = 0; n < 5; ++n)
	{
		const HeavenLevelConfig* pkLevel = GetHeavenLevelData(nIndex + n);
		if(pkLevel == NULL)
			continue;
		if(nIndex + n <= nHeavenLevel)
		{
			tips += UITextParser::HTMLNewLine();

			tips += UITextParser::HTMLBeginFont(TEXT("#14a800"));
			tips += UITextParser::HTMLBeginParagraph();
			tips += FString::Printf(TEXT("%s "),*heavenTitleTool.GetLevel(nIndex + n));
			tips += UITextParser::HTMLEndParagraph();
			tips += UITextParser::HTMLEndFont();

			tips += UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)0));
			//tips += UITextParser::HTMLBeginParagraph(TEXT("right"));
			GenerateGasTipsByNodeID(tips,pkLevel->nAtriNodeID1,pkLevel->nAtriNodeID2);
			//tips += UITextParser::HTMLEndParagraph();
			tips += UITextParser::HTMLEndFont();
		}
		else
		{
			tips += UITextParser::HTMLNewLine();
			tips += UITextParser::HTMLBeginFont(TEXT("#555555"));
			tips += UITextParser::HTMLBeginParagraph();
			tips += FString::Printf(TEXT("%s "),*heavenTitleTool.GetLevel(nIndex + n));
			tips += UITextParser::HTMLEndParagraph();

			//tips += UITextParser::HTMLBeginParagraph(TEXT("right"));
			GenerateGasTipsByNodeID(tips,pkLevel->nAtriNodeID1,pkLevel->nAtriNodeID2);
			//tips += UITextParser::HTMLEndParagraph();
			tips += UITextParser::HTMLEndFont();
		}
	}
}
void MMOHeavenManager::NotifyLevelChange(INT nLevel)
{
	if (bLevelLimited)
		CheckSetNeedTime();
}
void MMOHeavenManager::NotifyMoneyChange(INT nMoneyType,INT nMoney,INT nOldMoney)
{
	if(nMoneyType == (INT)E_MONEYTYPE_SOUL_PTS)
		CheckSetNeedTime();
	else if (nMoneyType == (INT)E_MONEYTYPE_AIR)
	{
		INT nChange = nMoney - nOldMoney;
		if(nChange > 0)
		{
			FString strInfo = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GasMoneyGetHint")),nChange);
			MMOUI::GetUIManager()->delegateAddScreenMessage(strInfo,MsgInfoType_Table,TEXT("MMOGame.Heaven.GasMoneyGetHint"));
		}
	}
	else if(E_MONEYTYPE_SPELL_PTS == nMoneyType)
	{
		GerateLogInfo(0,nMoney - nOldMoney);
	}
}
//server notify
void MMOHeavenManager::NotifyUpdateHeavenData(const ROLE_DB_SPIRIT_DATA* pkData)
{
	if(pkData == NULL)
		return;
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer)
	{
		mainPlayer->ActorAttributes->heavenAttributes.heavenLevel = pkData->byLevel;
		mainPlayer->ActorAttributes->heavenAttributes.heavenExp = pkData->nExp;
		nCurHeavenExp = pkData->nExp;
		nHeavenLevel = pkData->byLevel;
		bLevelLimited = mainPlayer->ActorAttributes->LevelAttributes.Level < SPIRIT_OPEN_LEVEL;
	}

	//dwLastOfflineTime = 0;
	dwLastExp2PtsTime = pkData->dwExp2PtsRemainTime;
	nCostMoneyAll	  = pkData->nMoneyCost;
	nDailyExp2PtsTime = pkData->dwDailyUseMoneyChangeCount;
	INT nTime = dwLastExp2PtsTime;
	if(nTime < 0 )
		dwLastExp2PtsTime = 0;
	arrSlotGasBag.Empty();
	arrSlotGasEquip.Empty();
	arrSlotGasBank.Empty();
	NotifyOpenGasSlot(sasbtEquip,pkData->nEquipContainerSize);
	NotifyOpenGasSlot(sasbtStall,pkData->nStallContainerSize);
	NotifyOpenGasSlot(sasbtBag,pkData->nBagContainerSize);

	gasPractiseState = pkData->nProductAirLevel;
	//gas
	myGasItemList.clear();
	converterLogLookUp.Empty();
	for(INT n = 0; n < MAX_AIRSEA_BAG_CONTAINER_SIZE; ++n)
	{
		if( pkData->nBagContainerTemplate[n] > 0/* && pkData->nBagContainerExp[n] > 0*/)
		{
			GasItem data;
			data.nContainerID = sasbtBag;
			data.nSlotIndex   = n;
			data.nTempID      = pkData->nBagContainerTemplate[n];
			data.nCurExp	  = pkData->nBagContainerExp[n];
			AddNewGas(data);

			GasSlot* pkSlot = GetSlotByIndex(sasbtBag,n);
			if(pkSlot)
			{
				pkSlot->nUniqueID = data.nUniqueID;
			}
		}
	}

	for(INT n = 0; n < MAX_AIRSEA_STALL_CONTAINER_SIZE; ++n)
	{
		if( pkData->nStallContainerTemplate[n] > 0 /*&& pkData->nStallContainerExp[n] > 0*/)
		{
			GasItem data;
			data.nContainerID = sasbtStall;
			data.nSlotIndex   = n;
			data.nTempID      = pkData->nStallContainerTemplate[n];
			data.nCurExp	  = pkData->nStallContainerExp[n];
			AddNewGas(data);

			GasSlot* pkSlot = GetSlotByIndex(sasbtStall,n);
			if(pkSlot)
			{
				pkSlot->nUniqueID = data.nUniqueID;
			}
		}
	}

	for(INT n = 0; n < MAX_AIRSEA_EQUIP_CONTAINER_SIZE; ++n)
	{
		if( pkData->nEquipContainerTemplate[n] > 0/* && pkData->nEquipContainerExp[n] > 0*/)
		{
			GasItem data;
			data.nContainerID = sasbtEquip;
			data.nSlotIndex   = n;
			data.nTempID      = pkData->nEquipContainerTemplate[n];
			data.nCurExp	  = pkData->nEquipContainerExp[n];
			AddNewGas(data);

			GasSlot* pkSlot = GetSlotByIndex(sasbtEquip,n);
			if(pkSlot)
			{
				pkSlot->nUniqueID = data.nUniqueID;
			}
		}
	}
	arrSlotBlood.Empty();
	for (INT n = 0; n < MAX_BLOODSEA_PAGE; ++n)
	{
		for (INT i = 0; i < MAX_BLOODSEA_SLOT_PER_PAGE; ++i)
		{
			BloodSlot data;
			data.nPage = n + 1;
			data.nIndex = i + 1;
			data.nValue = pkData->nBloodSlot[n][i];
			FString strCon = TEXT("MMOGame.Heaven.BloodSlotName");
			strCon += appItoa(n);
			strCon += appItoa(i);
			data.sName = MMOUI::ParseLocalizedPropertyPath(strCon);
			arrSlotBlood.AddItem(data);
		}
	}
	for(INT n = 0; n < MAX_BLOODSEA_PAGE; ++n)
	{
		nBloodState[n] = (pkData->byBloodState&(1<<n));
	}

	bServerWaiting = FALSE;
	CheckSetNeedTime(TRUE);
	//refresh realm combat power
	MMOPowerManager::getSingleton().UpdatePartPower( PowerPart_HeavenPow );
	MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_SynData);
	MMOUI::PostGameEvent(&evt);
}
void MMOHeavenManager::NotifyUpdateBloodState(INT nPage,INT nIndex, INT nNum)
{
	BloodSlot* pkSlot = GetBloodSlotByIndex(nPage,nIndex);
	if(pkSlot)
	{
		pkSlot->nValue = nNum;
		pkSlot->nState = nNum;

		MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_BloodStateUpdate);
		evt.SlotContainerID = nPage;
		evt.SlotID = nIndex;
		evt.nValue = nNum;
		MMOUI::PostGameEvent(&evt);

		// Achievement
		MMOGameEvent_Achievement evtAch1(adSpiritBloodPageCompleteCount, nPage, nNum, 0);
		MMOUI::PostGameEvent(&evtAch1);

		MMOGameEvent_Achievement evtAch2(adSpiritBloodPageLevel, nPage, nIndex, nNum);
		MMOUI::PostGameEvent(&evtAch2);
	}
}
void MMOHeavenManager::NotifyOpenGasSlot(INT nContainerID,INT nSlotIndex)
{
	if(nContainerID == sasbtEquip)
	{
		if(nSlotIndex > UCONST_GasEquipSlotMaxNum || (nSlotIndex - arrSlotGasEquip.Num() < 1))
			return;
		for(INT n = arrSlotGasEquip.Num(); n < nSlotIndex; ++n)
		{
			arrSlotGasEquip.AddItem(GasSlot());
		}

		// Achievement
		MMOGameEvent_Achievement evtAch(adSpiritBuyAirEquipSlotCount, arrSlotGasEquip.Num(), 0, 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (nContainerID == sasbtBag)
	{
		if(nSlotIndex > UCONST_GasBagSlotMaxNum || (nSlotIndex - arrSlotGasBag.Num() < 1))
			return;
		for(INT n = arrSlotGasBag.Num(); n < nSlotIndex; ++n)
		{
			arrSlotGasBag.AddItem(GasSlot());
		}

		// Achievement
		MMOGameEvent_Achievement evtAch(adSpiritBuyAirBagSlotCount, arrSlotGasBag.Num(), 0, 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	else if (nContainerID == sasbtStall)
	{
		if(nSlotIndex > UCONST_GasBankSlotMaxNum || (nSlotIndex - arrSlotGasBank.Num()) < 1)
			return;
		for(INT n = arrSlotGasBank.Num(); n < nSlotIndex; ++n)
		{
			arrSlotGasBank.AddItem(GasSlot());
		}

		// Achievement
		MMOGameEvent_Achievement evtAch(adSpiritBuyAirStallSlotCount, arrSlotGasBank.Num(), 0, 0);
		MMOUI::PostGameEvent(&evtAch);
	}
	MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_UpdateGasSlot);
	evt.SlotContainerID = nContainerID;
	evt.SlotID = nSlotIndex - 1;
	MMOUI::PostGameEvent(&evt);
}
void MMOHeavenManager::GerateLogInfo(INT nExp,INT nSpell)
{
	static INT sta_Exp = 0;
	static INT sta_Spell = 0;
	if(nSpell > 0)
		sta_Spell = nSpell;
	if(nExp > 0)
		sta_Exp = nExp;
	if(nSpell > 0 && sta_Exp > 0)
	{
		FString strInfo = FString::Printf(TEXT("<font color='#14a800'>[%s]</font>"),*GetConvertStr());
		strInfo += FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.GetReward1")),sta_Spell,sta_Exp);

		sta_Exp = 0;
		sta_Spell = 0;
		converterLogLookUp.AddItem(strInfo);
		MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_LogInfo);
		MMOUI::PostGameEvent(&evt);
	}
}
INT	MMOHeavenManager::GetNextLevelExp()
{
	const HeavenLevelConfig*	pkLevel = GetHeavenCurLevelData();
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer&&mainPlayer->ActorAttributes)
	{
		const HeavenLevelConfig*	pkLevel = GetHeavenLevelData(mainPlayer->ActorAttributes->heavenAttributes.heavenLevel);

		if(mainPlayer->GetVipType() > vlLevel0)
		{
			INT nDisCount = 100;
			if(mainPlayer->GetVipType() == vlLevel1)
			{
				nDisCount = pkLevel->VipOnlineChangeDiscount1 + 100;
			}
			else if(mainPlayer->GetVipType() == vlLevel2)
			{
				nDisCount = pkLevel->VipOnlineChangeDiscount2 + 100;
			}
			else if(mainPlayer->GetVipType() == vlLevel3)
			{
				nDisCount = pkLevel->VipOnlineChangeDiscount3 + 100;
			}
			return (pkLevel->nAddExp*nDisCount/100);
		}
		else
		{
			return pkLevel->nAddExp;
		}
	}
	return 0;
}
INT MMOHeavenManager::GetNextLevelPts()
{
	const HeavenLevelConfig*	pkLevel = GetHeavenCurLevelData();
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer&&mainPlayer->ActorAttributes)
	{
		const HeavenLevelConfig*	pkLevel = GetHeavenLevelData(mainPlayer->ActorAttributes->heavenAttributes.heavenLevel);

		if(mainPlayer->GetVipType() > vlLevel0)
		{
			INT nDisCount = 100;
			if(mainPlayer->GetVipType() == vlLevel1)
			{
				nDisCount = pkLevel->VIPOfflineDiscount1 + 100;
			}
			else if(mainPlayer->GetVipType() == vlLevel2)
			{
				nDisCount = pkLevel->VIPOfflineDiscount2 + 100;
			}
			else if(mainPlayer->GetVipType() == vlLevel3)
			{
				nDisCount = pkLevel->VIPOfflineDiscount3 + 100;
			}
			return (pkLevel->nAddHeavenPts*nDisCount/100);
		}
		else
		{
			return pkLevel->nAddHeavenPts;
		}
	}
	return 0;
}
void MMOHeavenManager::NotifyUpdateHeavenExp(INT nExp)
{
	INT nLastExp = 0;
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer)
	{
		nLastExp = mainPlayer->ActorAttributes->heavenAttributes.heavenExp;
		mainPlayer->ActorAttributes->heavenAttributes.heavenExp = nExp;
		nCurHeavenExp = nExp;
	}
	if((nExp - nLastExp) > 0)
	{
		GerateLogInfo(nExp - nLastExp,0);
		if (nCurConvertType == (INT)HeavenConvertType_JustConvert)
		{
			nDailyExp2PtsTime++;
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HeavenGasJustConvertSuc")),MsgInfoType_Table,TEXT("MMOGame.Heaven.HeavenGasJustConvertSuc"));
		}
	}
	bServerWaiting = FALSE;
	CheckSetNeedTime(TRUE);
	MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_UpdateExp);
	MMOUI::PostGameEvent(&evt);
}
void MMOHeavenManager::NotifyUpdateHeavenLevel(INT nLevel)
{
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer)
		mainPlayer->ActorAttributes->heavenAttributes.heavenLevel = nLevel;
	nHeavenLevel = nLevel;

	time_t t = time(NULL);
	tm     *tp = localtime(&t);
	if(tp)
	{
		FString strInfo = FString::Printf(TEXT("<font color='#14a800'>[%s]</font>"),*GetConvertStr()); 
		FString strLevel = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.LevelUp")),nLevel);
		strInfo += strLevel;
		converterLogLookUp.AddItem(strInfo);
		MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_LogInfo);
		MMOUI::PostGameEvent(&evt);
		MMOUI::GetUIManager()->delegateAddScreenMessage(strLevel,MsgInfoType_Table,TEXT("MMOGame.Heaven.LevelUp"));
	}
	//refresh realm combat power
	MMOPowerManager::getSingleton().UpdatePartPower( PowerPart_HeavenPow );
	CheckSetNeedTime();
	MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_UpdateLevel);
	MMOUI::PostGameEvent(&evt);

	//
	MMOGameEvent_OneInt sLvUpEvt( GameEvent_MySpiritLevelUp, nLevel);
	MMOUI::PostGameEvent(&sLvUpEvt);
}
void MMOHeavenManager::NotifyUpdatePractiseBtnIndex(INT nIndex)
{
	gasPractiseState = nIndex;
	MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_PractisePhrase);
	MMOUI::PostGameEvent(&evt);
}
void MMOHeavenManager::NotifyUpdateGasChange(INT nExp,INT nGasID,INT nContainerID,INT nSlotIndex)
{
	GasItem* pkData = GetItemBySlot(nContainerID,nSlotIndex);
	if(pkData)
	{
		pkData->nCurExp = nExp;
		pkData->nTempID = nGasID;

		if(nExp == 0 && nGasID == 0)
		{
			GasSlot* pkSlot = GetSlotByIndex(nContainerID,nSlotIndex);
			if (pkSlot)
			{
				pkSlot->nUniqueID = 0;
				pkSlot->nTagID = -1;
			}
			DelGasItem(pkData->nUniqueID);
		}
		else
			ShowNewGasHint(nGasID);
	}
	else if(nGasID != 0)
	{
		GasSlot* pkSlot = GetSlotByIndex(nContainerID,nSlotIndex);
		if(pkSlot)
		{
			GasItem data;
			data.nContainerID = nContainerID;
			data.nSlotIndex   = nSlotIndex;
			data.nCurExp      = nExp;
			data.nTempID      = nGasID;
			AddNewGas(data);
			pkSlot->nUniqueID = data.nUniqueID;
			ShowNewGasHint(nGasID);
		}
	}
	if(nContainerID == sasbtEquip)
		MMOPowerManager::getSingleton().UpdatePartPower( PowerPart_HeavenPow );
	RefreshTargetItem();
	MMOGameEvent_Heaven evt( MMOGameEvent_Heaven::EHeavenEvent_UpdateGasItem);
	evt.SlotID = nSlotIndex;
	evt.SlotContainerID = nContainerID;
	MMOUI::PostGameEvent(&evt);
}
void MMOHeavenManager::ClearTargetSlot()
{
	pkTargetGasSlot = NULL;
}
//return True for everything that needs to be intercepted
BOOL MMOHeavenManager::CheckGasBagFull()
{
	for (INT n = 0; n < arrSlotGasBag.Num(); ++n)
	{
		GasItem* pkItem = GetItemBySlot(sasbtBag,n);
		if(pkItem == NULL)
			return FALSE;
	}
	MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintGasBagFull")),MsgInfoType_Table,TEXT("MMOGame.Heaven.HintGasBagFull"));
	return TRUE;
}
BOOL MMOHeavenManager::CheckSoultoConvert()
{
	if(bLevelLimited)
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintLevelNoEnoughDo")),MsgInfoType_Table,TEXT("MMOGame.Heaven.HintLevelNoEnoughDo"));
		return TRUE;
	}
	HeavenLevelConfig* pkLevel = GetHeavenLevelDataIn(nHeavenLevel);;
	if(pkLevel)
	{
		INT nCurSoul = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_SOUL_PTS);
		if(nCurSoul < pkLevel->nSoulCost)
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintSoulNoEnoughDo")),MsgInfoType_Table,TEXT("MMOGame.Heaven.HintSoulNoEnoughDo"));
			return TRUE;
		}
	}
	return FALSE;
}
BOOL MMOHeavenManager::CheckMoneytoOpenSlot(INT nContainerID, INT nSlotID)
{
	INT nGold = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_02);

	if(nGold < GetNeedMoneyOpenSlot(nContainerID,nSlotID))
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintGoldNoEnoughDo")),MsgInfoType_Table,TEXT("MMOGame.Heaven.HintGoldNoEnoughDo"));
		return TRUE;
	}
	return FALSE;
}
BOOL MMOHeavenManager::CheckXiuLiantoPractise(INT nIndex)
{
	return FALSE;
}
BOOL MMOHeavenManager::CheckGasMoneytoExchange(INT nGasID)
{
	INT nMoney = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_AIR);
	HeavenGasTemplateMap::const_iterator itr = gasItemTemplateMap.find(nGasID);
	if(itr != gasItemTemplateMap.end())
	{
		if(itr->second->nExchangeCost > nMoney)
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintGasMoneyNoEnoughDo")),MsgInfoType_Table,TEXT("MMOGame.Heaven.HintGasMoneyNoEnoughDo"));
			return TRUE;
		}
	}
	return FALSE;
}
BOOL MMOHeavenManager::CheckLeveltoOpenSlot(INT nContainerID, INT nSlotID)
{
	return FALSE;
}
BOOL MMOHeavenManager::CheckHeavenLeveltoEquipSlot(INT nContainerID, INT nSlotID)
{
	if(nContainerID == sasbtEquip)
	{//open equipment one by one
		INT nCount = arrSlotGasEquip.Num();
		if (nCount < arrSlotTax.Num())
		{
			if(nHeavenLevel < arrSlotTax(nCount).nHeavenLevel)
			{
				FString strInfo = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.HintHeavenLevelNoEnoughDo")),arrSlotTax(nCount).nHeavenLevel);
				MMOUI::GetUIManager()->delegateAddScreenMessage(strInfo,MsgInfoType_Table,TEXT("MMOGame.Heaven.HintHeavenLevelNoEnoughDo"));
				return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL MMOHeavenManager::CheckHavePurpleGasBag()
{
	if(pkTargetGasSlot == NULL)
		return FALSE;
	for (INT n = 0; n < arrSlotGasBag.Num(); ++n)
	{
		if(pkTargetGasSlot->nTagID == n)
			continue;
		GasItem* pkItem = GetItemBySlot(sasbtBag,n);
		if(pkItem != NULL)
		{
			const HeavenGasItemConfig *pkGas = GetGasTemplate(pkItem->nTempID);
			if(pkGas != NULL)
			{
				if(pkGas->nQuality >= EIC_Color4 )
					return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL MMOHeavenManager::LoadOpenSlotCondition(const TCHAR* filePath)
{
	arrSlotTax.Empty(0);

	return FALSE;
}
INT  HeavenTitleTool::GetStartNum(INT nLevel)
{
	if( 0 < nLevel&&nLevel <= MAX_SPIRIT_LEVEL)
	{
		INT nIndex = (nLevel - 1)/5;
		if(nIndex > starTitle.Num() - 1)
			nIndex = starTitle.Num() - 1;
		INT nStar  = nLevel - nIndex*5;
		return nStar;
	}
	return 0;
}
INT  HeavenTitleTool::GetLevelTitleIndex(INT nLevel)
{
	if( 0 < nLevel&&nLevel <= MAX_SPIRIT_LEVEL)
	{
		INT nIndex = (nLevel - 1)/15;
		if(nIndex > levelTitle.Num() - 1)
			nIndex = levelTitle.Num() - 1;
		return nIndex;
	}
	return 0;
}
INT HeavenTitleTool::GetStarTitleIndex(INT nLevel)
{
	if( 0 < nLevel&&nLevel <= MAX_SPIRIT_LEVEL)
	{
		INT nIndex = (nLevel - 1)/5;
		if(nIndex > starTitle.Num() - 1)
			nIndex = starTitle.Num() - 1;
		return nIndex;
	}
	return 0;
}
FString HeavenTitleTool::GetLevelTitle(INT nLevel)
{
	INT nIndex = GetLevelTitleIndex(nLevel);
	if(nIndex >= 0)
		return levelTitle(nIndex);
	return "";
}
FString HeavenTitleTool::GetStarTitle(INT nLevel)
{
	if( 0 < nLevel&&nLevel <= MAX_SPIRIT_LEVEL)
	{
		INT nIndex = (nLevel - 1)/5;
		if(nIndex > starTitle.Num() - 1)
			nIndex = starTitle.Num() - 1;
		INT nStar  = nLevel - nIndex*5;
		return FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarNum") ),nStar)+starTitle(nIndex);
	}
	return "";
}
FString	HeavenTitleTool::_IndexGetLevelTitle(INT nIndex)
{
	if(nIndex >=0 && nIndex < starTitle.Num())
		return starTitle(nIndex);
	return "";
}
FString	HeavenTitleTool::_IndexGetStarTitle(INT nIndex)
{
	return TEXT("");
}
FString	HeavenTitleTool::GetLevel(INT nLevel)
{
	return FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.LevelNum") ),nLevel);
}
HeavenTitleTool::HeavenTitleTool()
{
	levelTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.LevelTitle1")));
	levelTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.LevelTitle2")));
	levelTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.LevelTitle3")));
	levelTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.LevelTitle4")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle1")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle2")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle3")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle4")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle5")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle6")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle7")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle8")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle9")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle10")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle11")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle12")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle13")));
	starTitle.AddItem(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.StarTitle14")));
}

void MMOHeavenManager::AddGasItemTemplateData(INT nKey,HeavenGasItemConfig* data)
{
	if(data == NULL)
		return;
	HeavenGasTemplateMap::const_iterator ItrFind = gasItemTemplateMap.find(nKey);
	if(ItrFind == gasItemTemplateMap.end())
	{
		gasItemTemplateMap.insert(std::make_pair(nKey, data));
	}
}
void MMOHeavenManager::AddHeavenLevelData(INT nKey,HeavenLevelConfig* data)
{
	if(data == NULL)
		return;
	HeavenLevelDataMap::const_iterator ItrFind = heavenLevelDataMap.find(nKey);
	if(ItrFind == heavenLevelDataMap.end())
	{
		heavenLevelDataMap.insert(std::make_pair(nKey, data));
	}
}
void MMOHeavenManager::AddHeavenBloodData(HeavenBloodConfig& data)
{
	heavenBloodDataList.push_back(data);
}
void MMOHeavenManager::AddHeavenConvertData(HeavenJustConvertData& data)
{
	heavenConvertList.push_back(data);
}
void MMOHeavenManager::ParseHeavenError(INT32 errorId, INT32 param1, INT32 param2, FString& errorStr)
{
	if(errorId == errSpiritChangeCDTime)
	{
		if(param1 < 0)
			param1 = 0;
		dwLastExp2PtsTime = param1 + 20;//by convention, push back 20 seconds
		bServerWaiting = FALSE;
		CheckSetNeedTime(TRUE);
	}
}
HeavenLevelConfig* HeavenLevelDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	HeavenLevelConfig *pkData = new HeavenLevelConfig();
	return  pkData;
}

void HeavenLevelDataProcessor::DestroyDataObject(HeavenLevelConfig* obj)
{
	if(obj)
	{
		delete obj;
		obj = NULL;
	}
}


void HeavenLevelDataProcessor::ParseData( HeavenLevelConfig* obj, class CTabFile* dataRow, int rowIdx )
{
	if(obj)
	{
		obj->nLevel				= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("Level"), dataRow, rowIdx);
		obj->nAddExp			= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("AddExp"), dataRow, rowIdx);
		obj->nAddHeavenPts		= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("AddSpiritPts"), dataRow, rowIdx);
		obj->nExp2PtsRate		= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("Power"), dataRow, rowIdx);
		obj->nMaxExp			= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("NextLevelExp"), dataRow, rowIdx);
		obj->nNeedMoney			= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("MoneyCostOneShot"), dataRow, rowIdx);
		obj->nNeedTime			= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("Exp2PtsCDTime"), dataRow, rowIdx);
		obj->nSoulCost			= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("SoulCost"), dataRow, rowIdx);
		obj->VIPOfflineDiscount1	= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("VipOfflineChangeDiscountLevel1"), dataRow, rowIdx);
		obj->VIPOfflineDiscount2	= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("VipOfflineChangeDiscountLevel2"), dataRow, rowIdx);
		obj->VIPOfflineDiscount3	= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("VipOfflineChangeDiscountLevel3"), dataRow, rowIdx);
		obj->VipOnlineChangeDiscount1	= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("VipOnlineChangeDiscountLevel1"), dataRow, rowIdx);
		obj->VipOnlineChangeDiscount2	= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("VipOnlineChangeDiscountLevel2"), dataRow, rowIdx);
		obj->VipOnlineChangeDiscount3	= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("VipOnlineChangeDiscountLevel3"), dataRow, rowIdx);
		obj->nAtriNodeID1		= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("AttrTemplateNodeID1"), dataRow, rowIdx);
		obj->nAtriNodeID2		= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("AttrTemplateNodeID2"), dataRow, rowIdx);
		obj->nDailyExp2PtsMaxCount		= TReadDataValue_Int<MMOHeavenLevelDataManager>(TEXT("DailyExp2PtsMaxCount"), dataRow, rowIdx);
	}
}

void HeavenLevelDataProcessor::PostInitData()
{
	MMOHeavenLevelDataManager& itemMgr = MMOHeavenLevelDataManager::getSingleton();
	MMOHeavenLevelDataManager::DataTypeMap::const_iterator it = itemMgr._dataMap.begin();
	for (; it != itemMgr._dataMap.end(); ++it)
	{
		MMOHeavenManager::getSingleton().AddHeavenLevelData(it->first, it->second);
	}
}


HeavenGasItemConfig* HeavenGasItemDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	HeavenGasItemConfig *pkData = new HeavenGasItemConfig();
	return  pkData;
}

void HeavenGasItemDataProcessor::DestroyDataObject(HeavenGasItemConfig* obj)
{
	if(obj)
	{
		delete obj;
		obj = NULL;
	}
}

void HeavenGasItemDataProcessor::ParseData( HeavenGasItemConfig* obj, class CTabFile* dataRow, int rowIdx )
{
	if(obj)
	{
		obj->nID			= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("ID"), dataRow, rowIdx);
		obj->name			= TReadDataValue_FString<MMOHeavenGasItemDataManager>(TEXT("Description"), dataRow, rowIdx);
		obj->nLevel			= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("Level"), dataRow, rowIdx);
		obj->nQuality		= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("Color"), dataRow, rowIdx);
		obj->nExp			= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("Exp"), dataRow, rowIdx);
		obj->nNextLevelExp	= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("NextLevelExp"), dataRow, rowIdx);
		obj->nNextLevelID	= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("NextLevelID"), dataRow, rowIdx);
		obj->nAtriNodeID1	= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("NodeTemplateID1"), dataRow, rowIdx);
		obj->nAtriNodeID2	= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("NodeTemplateID2"), dataRow, rowIdx);
		obj->nExchangeCost  = TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("ExchangeCount"), dataRow, rowIdx);
		obj->nGroupID		= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("GroupID"), dataRow, rowIdx);	
		obj->nCanExchange	= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("ExChangeAvaible"), dataRow, rowIdx);	
		obj->nJob			= TReadDataValue_Int<MMOHeavenGasItemDataManager>(TEXT("JobLimitedShow"), dataRow, rowIdx);	
		obj->Icon0			= TReadDataValue_FString<MMOHeavenGasItemDataManager>(TEXT("Icon0"), dataRow, rowIdx);
		//obj->Icon1			= TReadDataValue_FString<MMOHeavenGasItemDataManager>(TEXT("Icon1"), dataRow, rowIdx);
	}
}

void HeavenGasItemDataProcessor::PostInitData()
{
	MMOHeavenGasItemDataManager& itemMgr = MMOHeavenGasItemDataManager::getSingleton();
	MMOHeavenGasItemDataManager::DataTypeMap::const_iterator it = itemMgr._dataMap.begin();
	for (; it != itemMgr._dataMap.end(); ++it)
	{
		MMOHeavenManager::getSingleton().AddGasItemTemplateData(it->first, it->second);
	}
}

HeavenBloodConfig* HeavenBloodDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return  NULL;
}

void HeavenBloodDataProcessor::DestroyDataObject(HeavenBloodConfig* obj)
{

}

void HeavenBloodDataProcessor::ParseData( HeavenBloodConfig* obj, class CTabFile* dataRow, int rowIdx )
{
	HeavenBloodConfig bloodData;

	bloodData.nPage				=  TReadDataValue_Int<MMOHeavenBloodDataManager>(TEXT("Page"), dataRow, rowIdx);
	bloodData.nSlot				=  TReadDataValue_Int<MMOHeavenBloodDataManager>(TEXT("Slot"), dataRow, rowIdx);
	bloodData.nLevel			=  TReadDataValue_Int<MMOHeavenBloodDataManager>(TEXT("Number"), dataRow, rowIdx);
	bloodData.nHeavenPtsCost	=  TReadDataValue_Int<MMOHeavenBloodDataManager>(TEXT("SpiritPtsCost"), dataRow, rowIdx);
	bloodData.nRate				=  TReadDataValue_Int<MMOHeavenBloodDataManager>(TEXT("Rate"), dataRow, rowIdx);
	bloodData.nLevelLimit		=  TReadDataValue_Int<MMOHeavenBloodDataManager>(TEXT("LevelLimit"), dataRow, rowIdx);
	bloodData.nAttValue1		=  TReadDataValue_Int<MMOHeavenBloodDataManager>(TEXT("AttrTemplateNodeID1"), dataRow, rowIdx);
	bloodData.nAttValue2		=  TReadDataValue_Int<MMOHeavenBloodDataManager>(TEXT("AttrTemplateNodeID2"), dataRow, rowIdx);
	bloodData.nPageLevelLimit	=  TReadDataValue_Int<MMOHeavenBloodDataManager>(TEXT("PageLevelLimit"), dataRow, rowIdx);
	MMOHeavenManager::getSingleton().AddHeavenBloodData(bloodData);
}

void HeavenBloodDataProcessor::PostInitData()
{
}
HeavenJustConvertData* HeavenJustConvertProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return  NULL;
}

void HeavenJustConvertProcessor::DestroyDataObject(HeavenJustConvertData* obj)
{

}

void HeavenJustConvertProcessor::ParseData( HeavenJustConvertData* obj, class CTabFile* dataRow, int rowIdx )
{
	HeavenJustConvertData nData;

	nData.nID				=  TReadDataValue_Int<MMOHeavenJustConvertManager>(TEXT("Index"), dataRow, rowIdx);
	nData.nTimesCount		=  TReadDataValue_Int<MMOHeavenJustConvertManager>(TEXT("DailyChangeTime"), dataRow, rowIdx);
	nData.nCostDiscount		=  TReadDataValue_Int<MMOHeavenJustConvertManager>(TEXT("MoneyCostDiscount"), dataRow, rowIdx);

	MMOHeavenManager::getSingleton().AddHeavenConvertData(nData);
}

void HeavenJustConvertProcessor::PostInitData()
{
}