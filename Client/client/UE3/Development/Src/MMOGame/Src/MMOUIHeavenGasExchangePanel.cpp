#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOHeavenManager.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUIHeavenGasExchangePanel)


void UMMOUIHeavenGasExchangePanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if(v)
	{
		SetPosition(200,100);
		UpdateExchangeGasList();
		eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Heaven.ExchangeTitle")));
	}
}
void UMMOUIHeavenGasExchangePanel::OnClickExchangeGasItem(INT nGasID)
{
	CMessageHandler::GetInstance().Send_GasExchange(nGasID);
}
void UMMOUIHeavenGasExchangePanel::UpdateExchangeGasList()
{
	if(GFXPlayer == NULL)
		return;
	INT nJob = -1;
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer&&mainPlayer->ActorAttributes)
		nJob = mainPlayer->GetClassId();

	UGFxObject* pkArrData = GFXPlayer->CreateArray();
	if(pkArrData == NULL)
		return;
	INT nIndex = 0;
	const HeavenGasTemplateMap& mapItem = MMOHeavenManager::getSingleton().GetGasItemDataMap();
	for (HeavenGasTemplateMap::const_iterator itr = mapItem.begin();itr != mapItem.end(); ++itr )
	{
		//Qi that cannot be exchanged
		if(itr->second->nCanExchange <= 0)
			continue;
		//Class restriction
		if((nJob == itr->second->nJob) || (itr->second->nJob == -1))
		{
			UGFxObject* pkData = GFXPlayer->CreateObject(TEXT("UI_SkillTree.SkillTreeAS3.HeavenGasExchangeData"));
			if(pkData == NULL)
				continue;
			pkData->SetInt("GasID",itr->second->nID);
			FString  GasName = UITextParser::HTMLBeginFont(UITextParser::HTMLGetItemColor((EItemColor)itr->second->nQuality));
			GasName += FString::Printf(TEXT("<p >%s</p>"), *(itr->second->name));
			GasName += UITextParser::HTMLEndFont();
			pkData->SetString("GasName",GasName);

			FString strDes;
			MMOHeavenManager::getSingleton().GenerateGasTipsByNodeID(strDes,itr->second->nAtriNodeID1,itr->second->nAtriNodeID2);
			pkData->SetString("GasDescrible",strDes);
			FString strSwf = FString::Printf(TEXT("../%s"),*(itr->second->Icon0));
			pkData->SetString("IconName0",strSwf);
			pkData->SetString("IconName1",itr->second->Icon1);
			pkData->SetInt("GasCost",itr->second->nExchangeCost);
			pkData->SetInt("GasExp",itr->second->nExp);
			pkData->SetInt("GasColor",itr->second->nQuality);
			pkData->SetInt("GasLevel",itr->second->nLevel);
			pkArrData->SetElementObject(nIndex++,pkData);
		}
	}
	eventCallUpdateHeavenGasItemList(pkArrData);
}

//===============================================================================================================
//===============================================================================================================
void UMMOUIHeavenGasExchangePanel::ASClickExchangeGasItem(INT nGasID)
{
	OnClickExchangeGasItem(nGasID);
}
//===============================================================================================================
//===============================================================================================================