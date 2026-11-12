#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"


IMPLEMENT_CLASS(UMMOUIMountPanel);


void UMMOUIMountPanel::Init()
{
	Super::Init();
	UGFxObject *equip_btn	= GetObject( TEXT("equip_btn") );
	if(equip_btn)
	{
		equip_btn->SetString(TEXT("label"),  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mount.Equip"))  );
	}
	UGFxObject *unequip_btn = GetObject( TEXT("unequip_btn") );
	if(unequip_btn)
	{
		unequip_btn->SetString(TEXT("label"),  MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Mount.Unequip"))  );
	}

	MMOUI::GetUIManager()->SetRefreshMountDelegate(FScriptDelegate(this, TEXT("RefreshMount")));
	MMOUI::GetUIManager()->SetRefreshEquipedMountDelegate(FScriptDelegate(this, TEXT("RefreshEquipedMount")));


}

void UMMOUIMountPanel::UnInit()
{
	Super::UnInit();

	FScriptDelegate nullDelgate(EC_EventParm);
	MMOUI::GetUIManager()->SetRefreshMountDelegate(nullDelgate);
	MMOUI::GetUIManager()->SetRefreshEquipedMountDelegate(nullDelgate);

}

void UMMOUIMountPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		UpdateMount();
		UpdateEquipedMount();
		//eventRefreshMount(TRUE);
	}
}

INT UMMOUIMountPanel::GetSlotType()
{
	return 0;//E_CONSLOT_Mount_EQUIPPED;
}


//------------------------------------------------------------------------------------------------------------

UBOOL UMMOUIMountPanel::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 

UBOOL UMMOUIMountPanel::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	if(mc && SlotType==ESPType_Mount)
	{
		
		//SelectMountID = SlotID;
		eventCallSelectMount(SlotID);
	}
	return FALSE;
}

UBOOL UMMOUIMountPanel::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	return FALSE;
}

UBOOL UMMOUIMountPanel::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	// zhaojun@snda
	if (SrcContainerID == DestContainerID && SrcSlotID == DestSlotID)
		return FALSE;

	//if(SrcSlotType == ESPType_Mount && DestSlotType == ESPType_Invalid) // discard
	//{
	//	FClientItemInfo* info = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
	//	if(info)
	//	{
	//		ClientActionCondition_EquipItem checker;

	//		FClientActionConditionContex contex;
	//		contex.Item = info;

	//		if(!checker.CheckClientActionCondition(contex))
	//		{
	//			return TRUE;
	//		}

	//		MMOUI::GetUIManager()->delegateDeleteItem(SrcContainerID, SrcSlotID);
	//	}

	//	return TRUE;
	//}
	//else if(SrcSlotType == ESPType_Mount && DestSlotType == ESPType_Mount)  // swap
	//{
	//	FClientItemInfo* info = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
	//	if(info)
	//	{
	//		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
	//		UItem* destItemTemp = MMOFacade::getSingleton().FindItemTemplate(DestContainerID, DestSlotID);

	//		ClientActionCondition_EquipItem checker;

	//		FClientActionConditionContex contex;
	//		contex.Item = info;
	//		contex.ItemTemplate = itemTemp;

	//		if(!checker.CheckClientActionCondition(contex))
	//		{
	//			return TRUE;
	//		}

	//		INT srcSlotEquipType = SrcSlotMC->GetInt(TEXT("itemid"));
	//		INT destSlotEquipType = DestSlotMC->GetInt(TEXT("itemid"));

	//		// Grids of different types cannot be swapped
	//		if(srcSlotEquipType != destSlotEquipType)
	//		{
	//			return TRUE;
	//		}

	//		CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);

	//		return TRUE;
	//	}
	//}
	//else if(SrcSlotType == ESPType_Mount && DestSlotType == ESPType_Bag) // unequip
	//{
	//	FClientItemInfo* info = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
	//	if(info) // if the target grid has equipment that is not bound
	//	{
	//		UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);

	//		ClientActionCondition_Hero_EquipItem checker;

	//		FClientActionConditionContex contex;
	//		contex.Item = info;
	//		contex.ItemTemplate = itemTemp;

	//		if(!checker.CheckClientActionCondition(contex))
	//		{
	//			return TRUE;
	//		}

	//		UEquip* equip = Cast<UEquip>(itemTemp);

	//		if(itemTemp && equip && info->m_byBindType != E_BINDTYPE_BINDED
	//			&& equip->ItemBindType != E_BINDTYPE_NONE)
	//		{
	//			AskEquipBind(info);
	//		}
	//		else
	//		{
	//			CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
	//		}
	//	}
	//	else
	//	{
	//		CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
	//	}

	//	return TRUE;
	//}

	return FALSE;
}


//---------------------------------------------------------------------
void UMMOUIMountPanel::UpdateEquipedMount()
{
	AMMOPlayer* pMainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(pMainPlayer && pMainPlayer->ActorAttributes){
		eventCallSetEquipedMount(pMainPlayer->ActorAttributes->MountID);
	}	
}

void UMMOUIMountPanel::UpdateMount()
{
	UGFxObject *DataProvider = GFXPlayer->CreateArray();
	if(DataProvider==NULL)return;


	INT i=0;
	MountDataMap* pMountMap = MMOFacade::getSingleton().GetMountDataMap();
	MountDataMap::iterator MountMapItor ;
	for ( MountMapItor= pMountMap->begin();  MountMapItor !=pMountMap->end(); MountMapItor++ )
	{
		MountData* pMount = &(MountMapItor->second);
		if(pMount){


			UGFxObject * SlotObj = GFXPlayer->CreateObject(TEXT("ActionScript3.Mount.MountSlotData"));

			if(SlotObj){
				SlotObj->SetBool( TEXT("HaveData"), TRUE );
				SlotObj->SetInt( TEXT("EquipID"), ESBarSlotTp_Mount );
				SlotObj->SetInt("SlotType", ESPType_Mount );
				SlotObj->SetInt("slotType", ESPType_Mount );
				SlotObj->SetInt("slotIndex",i);

				SlotObj->SetInt( TEXT("itemTemplateID"), pMount->ID );
				SlotObj->SetInt(TEXT("slotID")	,   pMount->ID );
				SlotObj->SetInt(TEXT("containerID"),E_CONSLOT_MAX );



				SlotObj->SetInt(TEXT("Have")		,pMount->Have );
				SlotObj->SetBool(TEXT("isLocked")	,pMount->Have<=0 );

				SlotObj->SetInt(TEXT("IsEnable")	, 1 );
				SlotObj->SetInt(TEXT("classesLevel")	,pMount->LevelLimit);
				SlotObj->SetInt(TEXT("classesQuality")	,pMount->Quality );
				SlotObj->SetInt(TEXT("itemColor")	,pMount->Quality );

				SlotObj->SetString(TEXT("slotName") , pMount->Name	);
				//SlotObj->SetInt(TEXT("EquipID")	,E_CONSLOT_HERO_EQUIPPED );
				//SlotObj->SetInt(TEXT("itemid")	,list[i] );



				SlotObj->SetString(TEXT("iconName")	,FString::Printf(TEXT("img://%s"), *pMount->IconName ) );
			
				//SlotObj->SetInt(TEXT("itemIndex")	, i );
				//SlotObj->SetString(TEXT("itemtype")	,TEXT("") );
				//SlotObj->SetInt(TEXT("quantity")	,pMount->Quality );
				SlotObj->SetString(TEXT("tipsHtml")	,pMount->Script );

				//SlotObj->SetString(TEXT("commonTip")	, pMount->Name )

				DataProvider->SetElementObject( i++, SlotObj );
			}
		}
	}

	//SelectMountID = -1;
	eventCallSelectMount(-1);

	eventCallUpdateMount(DataProvider);

}


void UMMOUIMountPanel::ASEquipMount(INT MountID)
{
	CMessageHandler::GetInstance().Send_EquipMount(MountID);
}

void UMMOUIMountPanel::ASUnEquipMount()
{
	CMessageHandler::GetInstance().Send_EquipMount(0);
}
