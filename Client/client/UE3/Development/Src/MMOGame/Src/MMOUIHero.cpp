#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "attribute_def.h"
#include "ClientActionConditionCheck.h"


IMPLEMENT_CLASS(UMMOUIHero)

void UMMOUIHero::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if(v)
	{
		eventRefreshHero(TRUE);
	}
}

void UMMOUIHero::ClearUI()
{

}
//------------------------------------------------------------------------
void UMMOUIHero::EnterScene()
{
	if(GFXPlayer)
	{
		GFXPlayer->NeedInitSceneUI = true;
	}
	//HeroClassId = EPCT_None;
	//HeroSex     = EGT_None; 
}

//------------------------------------------------------------------------
void UMMOUIHero::ChangeScene()
{
	UMMOUIBase::ChangeScene();
	//CreateShowLevel();
}

INT UMMOUIHero::GetSlotType()
{
	return E_CONSLOT_HERO_EQUIPPED;
}

//------------------------------------------------------------------------
void UMMOUIHero::UpdateEquip()
{
	if(!(GFXPlayer&&GFXPlayer->ShowAvatar)) return;

	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();
	if(!(Player&&Player->ItemManager))return;


	for( int i = 0; i < NES_VISIBLE_TOTAL; ++i )
	{
		const UEquip* equip = Player->ItemManager->EquippedItemList_Hero(i);
		const UItem* item = MMOFacade::getSingleton().FindItemTemplate(equip->ItemId);
		if(!item)
		{
			continue;
		}
		GFXPlayer->ShowAvatar->ItemManager->RequestTakeOn( -1, i, true, item );
	}
	GFXPlayer->ShowAvatar->ItemManager->UpdateOwnerData();

}
//------------------------------------------------------------------------
void UMMOUIHero::CreateShowLevel()
{
	if( ! ShowLevel  && GFXPlayer )
	{

		ShowLevel = GFXPlayer->eventGetShowLevel( ShowLevelName );

	}
	if( !ShowLevel || !ShowLevel->RenderTarget )
		return;
	ShowLevel->SetEnabled( true );
	GFXPlayer->SetExternalTexture( TEXT("img_HeroPic -nopack"),ShowLevel->RenderTarget);
}
//------------------------------------------------------------------------
void UMMOUIHero::EnableCapture()
{
	UBOOL bEnable = (Player&&Player->ServerHeroID>0);

	CreateShowLevel();

	if( !ShowLevel  )
		return;

	//if( GFXPlayer->ShowAvatar==NULL)
	//	return;

	eventCallShowCapture(bEnable);
	if( bEnable )
	{
		
		//UMMOGFXPlayer_Hero* player = Cast<UMMOGFXPlayer_Hero>(GFXPlayer);
		//if(GFXPlayer->NeedInitSceneUI)
		{
			SysModelEquip();
			GFXPlayer->NeedInitSceneUI = FALSE;
		}
		//SysModelEquip();
		ShowLevel->SetEnabled( bEnable );

		FName tag( "C_01", FNAME_Find );
		if( GFXPlayer->ShowAvatar ){
			if( GFXPlayer->ShowAvatar->UseingAvatar )
				tag = GFXPlayer->ShowAvatar->UseingAvatar->PlacementNodeTag;
			// change to dynamic lighting
			if( GFXPlayer->ShowAvatar->LightEnvironment )
				GFXPlayer->ShowAvatar->LightEnvironment->SetEnabled( FALSE );
			ShowLevel->SetShowActor( GFXPlayer->ShowAvatar, tag );
			//ShowLevel->SetShowActor( GFXPlayer->ShowAvatar, FName( "C_01", FNAME_Find ) );
		}
	}
	else
	{
		ShowLevel->SetEnabled( bEnable );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void UMMOUIHero::SysModelEquip()
{

	if(GIsPlayInEditorWorld)
	{
		return;
	}

	AMMOPlayer* Player = MMOFacade::getSingleton().GetMainPlayer();

	if( GFXPlayer->ShowAvatar == NULL )
	{
		for( TObjectIterator<AMMOPlayer>It; It; ++It )
		{
			AMMOPlayer* Avatar = (*It);
			if( Avatar && Avatar->ActorName == TEXT("hero_model") )
			{
				GFXPlayer->ShowAvatar = Avatar;
				GFXPlayer->ShowAvatar->WorldInfo = Player->WorldInfo;
				break;
			}
		}

	}
	if( GFXPlayer->ShowAvatar != NULL )
	{
		if(	HeroClassId!=Player->GetHeroClassId() || HeroSex!= Player->GetHeroSex() )
		{
			//GFXPlayer->ShowAvatar->eventSetMesh( Player->GetHeroClassId() , Player->GetHeroSex() );
			//GFXPlayer->ShowAvatar->ItemManager->clea
			GFXPlayer->ShowAvatar->NativeSetMesh( Player->GetHeroClassId() , Player->GetHeroSex() );
			HeroClassId	= Player->GetHeroClassId();
			HeroSex		= Player->GetHeroSex();
		}
		ModelEquip();
	}				
	return;
}

//------------------------------------------------------------------------------------------------------------------------
void UMMOUIHero::ModelEquip()
{
	AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	if(GFXPlayer->ShowAvatar==NULL)return;

	for(int i = 0; i < pl->ItemManager->EquippedItemList_Hero.Num(); i++)
	{
		UEquip* tEquip = pl->ItemManager->EquippedItemList_Hero(i);
		if( tEquip != NULL ) //&& tEquip->MeshList.Num() > 0 )
		{			
			//const UItem* tEquipTemplate = ( MMOFacade::getSingleton().FindItemTemplate( tEquip->ItemId ) );
			//if(tEquipTemplate)
			{
				GFXPlayer->ShowAvatar->ItemManager->RequestTakeOn( -1, i , true , tEquip ,false );
			}
		}
		else
		{
			if(GFXPlayer->ShowAvatar->ItemManager->GetEquipBySlot(i,true) )
			{
				GFXPlayer->ShowAvatar->ItemManager->RequestTakeOff(i, true);
			}
		}
	}

	GFXPlayer->ShowAvatar->ItemManager->UpdateOwnerData();
}


INT         UMMOUIHero::GetTextColor(INT Val, INT OriVal)
{
	if(Val==OriVal)
	{
		return MMOUI::GetUIManager()->OriginAttrColor;
	}
	else if(Val>OriVal)
	{
		return MMOUI::GetUIManager()->IncreaseAttrColor;
	}
	else
	{
		return MMOUI::GetUIManager()->DecreaseAttrColor;
	}
}

UGFxObject* UMMOUIHero::CreateAttributeTab(FString NodeName)
{
	UGFxObject * AttributeObj = GFXPlayer->CreateObject("Object");
	if(AttributeObj){
		AttributeObj->SetInt("type",2);
		AttributeObj->SetBool("visible",true);
		AttributeObj->SetInt("deepth",0);
		AttributeObj->SetString("label",   MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Hero."))+NodeName) );
	}
	return AttributeObj;
}

void UMMOUIHero::CreateAttributeNull(INT enum1, INT& index, UGFxObject *DataProvider, UEnum* Enum)
{
	FString ClientEnumStr1 = FString(Enum->GetEnum(enum1).ToString());
	UGFxObject * AttributeObj = GFXPlayer->CreateObject("Object");
	if(AttributeObj){
		AttributeObj->SetInt("type",0);
		AttributeObj->SetBool("visible",true);
		AttributeObj->SetInt("deepth",1);
		AttributeObj->SetString("label",   MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Hero."))+ClientEnumStr1) );
		AttributeObj->SetString("textStr", TEXT(""));
		AttributeObj->SetInt("textColor",  0 );
	}

	DataProvider->SetElementObject( index++, AttributeObj);
}

UGFxObject* UMMOUIHero::CreateAttributeValue(FString NodeName, INT Val, INT OriVal,BOOL PercentageFlag)
{
	UGFxObject * AttributeObj = GFXPlayer->CreateObject("Object");
	if(AttributeObj){
		AttributeObj->SetInt("type",0);
		AttributeObj->SetBool("visible",true);
		AttributeObj->SetInt("deepth",1);
		AttributeObj->SetString("label",   MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Hero."))+NodeName) );
		if(PercentageFlag)AttributeObj->SetString("textStr", FString::Printf( TEXT("%.2f%%"), (FLOAT)Val/100 ));
		else			  AttributeObj->SetString("textStr", FString::Printf( TEXT("%d"), Val ));	
		AttributeObj->SetInt("textColor",  GetTextColor( Val, OriVal) );
	}
	return AttributeObj;
}

UGFxObject* UMMOUIHero::CreateAttributeRange(FString NodeName, INT Val, INT OriVal,INT Val2, INT OriVal2,BOOL PercentageFlag)
{
	UGFxObject * AttributeObj = GFXPlayer->CreateObject("Object");
	if(AttributeObj){
		AttributeObj->SetInt("type",0);
		AttributeObj->SetBool("visible",true);
		AttributeObj->SetInt("deepth",1);
		AttributeObj->SetString("label",   MMOUI::ParseLocalizedPropertyPath( FString(TEXT("MMOGame.Hero."))+NodeName) );
		if(PercentageFlag)AttributeObj->SetString("textStr", FString::Printf( TEXT("%.2f%%-%.2f%%"), (FLOAT)Val/100, (FLOAT)Val2/100 ));
		else			  AttributeObj->SetString("textStr", FString::Printf( TEXT("%d-%d"), Val,Val2 ));	
		AttributeObj->SetInt("textColor",  GetTextColor( Val, OriVal) );
	}
	return AttributeObj;
}

void UMMOUIHero::AddAttributeList(INT enum1,INT enum2, INT& index, UGFxObject *DataProvider,UActorAttribute* Attributes,UEnum* Enum ,INT level,INT classId,BOOL floatFormat )
{
	FString ClientEnumStr1 = FString(Enum->GetEnum(enum1).ToString());
	INT OriginValue1 = MMOFacade::getSingleton().GetAttributeConfigData( level, classId , enum1 );
	INT PlusValue1	 = MMOFacade::getSingleton().GetAttribute2ConfigData( level, classId , enum1 );

	if(enum2<=0)
	{
		DataProvider->SetElementObject( index++, CreateAttributeValue(ClientEnumStr1,Attributes->GetAttribute(enum1),OriginValue1+PlusValue1,floatFormat)	);

	}
	else
	{
		//FString ClientEnumStr2 = FString(Enum->GetEnum(enum2).ToString());
		INT OriginValue2 = MMOFacade::getSingleton().GetAttributeConfigData( level, classId , enum2 );
		INT PlusValue2	 = MMOFacade::getSingleton().GetAttribute2ConfigData( level, classId , enum2 );
		DataProvider->SetElementObject( index++, CreateAttributeRange(ClientEnumStr1,Attributes->GetAttribute(enum1),OriginValue1+PlusValue1,Attributes->GetAttribute(enum2),OriginValue2+PlusValue2,floatFormat)	);

	}
}

//---------------------------------------------------------------------
void UMMOUIHero::RefreshAttribute()
{
	INT index = 0;
	UEnum* Enum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("ECHAR_ATTR_TYPE"), TRUE);
	UGFxObject *DataProvider = GFXPlayer->CreateArray();
	if(DataProvider==NULL)return;
	if(Player==NULL)return;


	if(Player->ServerHeroID<=0){

		// JO*(WJOSIJERg
		DataProvider->SetElementObject( index++,CreateAttributeTab(TEXT("Tab1")) );
		//CreateAttributeNull(catStrength		,index,DataProvider,Enum);
		//CreateAttributeNull(catAgility		,index,DataProvider,Enum);
		//CreateAttributeNull(catIntellect	,index,DataProvider,Enum);
		//CreateAttributeNull(catStamina		,index,DataProvider,Enum);
		//CreateAttributeNull(catSpirit		,index,DataProvider,Enum);

		DataProvider->SetElementObject( index++,CreateAttributeTab(TEXT("Tab2")) );
		////CreateAttributeNull(catMeleeAtkLo	,index,DataProvider,Enum);
		////CreateAttributeNull(catMagicAtkLo	,index,DataProvider,Enum);
		//CreateAttributeNull(catPhyCritRate	,index,DataProvider,Enum);
		//CreateAttributeNull(catMagCritRate	,index,DataProvider,Enum);

		DataProvider->SetElementObject( index++,CreateAttributeTab(TEXT("Tab3")) );
		CreateAttributeNull(catPhyDef		,index,DataProvider,Enum);
		CreateAttributeNull(catMagDef		,index,DataProvider,Enum);
		CreateAttributeNull(catMaxHp		,index,DataProvider,Enum);
		CreateAttributeNull(catHpRecovery	,index,DataProvider,Enum);
		CreateAttributeNull(catMaxMp		,index,DataProvider,Enum);
		CreateAttributeNull(catMpRecovery	,index,DataProvider,Enum);
		//CreateAttributeNull(catDodgeRate	,index,DataProvider,Enum);
		//CreateAttributeNull(catParryRate	,index,DataProvider,Enum);
		//CreateAttributeNull(catBlockRate	,index,DataProvider,Enum);
	}
	else{

		UActorAttribute* Attributes = Player->HeroAttributes;
		if(Attributes==NULL)return;

		INT level	= Player->HeroAttributes->LevelAttributes.Level;
		INT classId	= Player->HeroAttributes->ClassType;

		DataProvider->SetElementObject( index++,CreateAttributeTab(TEXT("Tab1")) );
//		AddAttributeList(catStrength	,0,index,DataProvider,Attributes,Enum,level,classId);
//		AddAttributeList(catAgility		,0,index,DataProvider,Attributes,Enum,level,classId);
//		AddAttributeList(catIntellect	,0,index,DataProvider,Attributes,Enum,level,classId);
//		AddAttributeList(catStamina		,0,index,DataProvider,Attributes,Enum,level,classId);
//		AddAttributeList(catSpirit		,0,index,DataProvider,Attributes,Enum,level,classId);

		DataProvider->SetElementObject( index++,CreateAttributeTab(TEXT("Tab2")) );
////	AddAttributeList(catMeleeAtkLo		,catMeleeAtkHi,index,DataProvider,Attributes,Enum,level,classId);
////	AddAttributeList(catMagicAtkLo		,catMagicAtkHi,index,DataProvider,Attributes,Enum,level,classId);
//		AddAttributeList(catPhyCritRate		,0,index,DataProvider,Attributes,Enum,level,classId,TRUE);
//		AddAttributeList(catMagCritRate		,0,index,DataProvider,Attributes,Enum,level,classId,TRUE);

		DataProvider->SetElementObject( index++,CreateAttributeTab(TEXT("Tab3")) );
		AddAttributeList(catPhyDef		,0,index,DataProvider,Attributes,Enum,level,classId);
		AddAttributeList(catMagDef		,0,index,DataProvider,Attributes,Enum,level,classId);
		AddAttributeList(catMaxHp		,0,index,DataProvider,Attributes,Enum,level,classId);
		AddAttributeList(catHpRecovery	,0,index,DataProvider,Attributes,Enum,level,classId);
		AddAttributeList(catMaxMp		,0,index,DataProvider,Attributes,Enum,level,classId);
		AddAttributeList(catMpRecovery	,0,index,DataProvider,Attributes,Enum,level,classId);
		//AddAttributeList(catDodgeRate	,0,index,DataProvider,Attributes,Enum,level,classId,TRUE);
		//AddAttributeList(catParryRate	,0,index,DataProvider,Attributes,Enum,level,classId,TRUE);
		//AddAttributeList(catBlockRate	,0,index,DataProvider,Attributes,Enum,level,classId,TRUE);

		// JO*(WJOSIJERg end


	}

	eventCallUpdateAttribute(DataProvider);
}

//---------------------------------------------------------------------
void UMMOUIHero::RefreshInfo()
{
	if(Player==NULL)return;

	if(Player->ServerHeroID<=0)
	{
		UGFxObject * AttributeObj = GFXPlayer->CreateObject("Object");
		AttributeObj->SetString("Growth",   TEXT("") );
		AttributeObj->SetString("Understanding",   TEXT("") );
		AttributeObj->SetString("Exp",		TEXT("") );
		AttributeObj->SetString("RoleName", TEXT("") );
		eventCallUpdateInfo(AttributeObj);
	}
	else{

		UActorAttribute* Attributes = Player->HeroAttributes;
		if(Attributes==NULL)return;

		UGFxObject * AttributeObj = GFXPlayer->CreateObject("Object");
		AttributeObj->SetString("Growth",   FString::Printf( TEXT("%d"), Attributes->Growth) );
		AttributeObj->SetString("Understanding",   TEXT("0") );
		AttributeObj->SetString("Exp",   FString::Printf( TEXT("%d/%d"), Attributes->LevelAttributes.CurrentExp, Attributes->LevelAttributes.MaxExp ) );

		AttributeObj->SetString("RoleName", FString::Printf( TEXT( "%d%s(%s)  %s") , Attributes->LevelAttributes.Level ,
			*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Character.Lv") ),
			*MMOUI::ParseLocalizedPropertyPath( FString::Printf(TEXT("MMOGame.School.SchoolName%d"), Attributes->ClassType)),
			*Player->HeroName  ) );

		eventCallUpdateInfo(AttributeObj);
	}
}

//---------------------------------------------------------------------
void UMMOUIHero::InitUIText()
{
	UGFxObject * UiTextObj = GFXPlayer->CreateObject("Object");

	UiTextObj->SetString("Summon",		MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Hero.Summon") ));
	UiTextObj->SetString("Fire",		MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Hero.Fire") ));
	UiTextObj->SetString("Bag",			MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Hero.Bag") ));
	UiTextObj->SetString("Rename",		MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Hero.Rename") ));
	UiTextObj->SetString("Uncall",		MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Hero.Uncall") ));

	eventCallInitUIText(UiTextObj);
}

void UMMOUIHero::RefreshSummon()
{
	eventCallShowSummon(Player->HeroSummon);
}

//---------------------------------------------------------------------
void UMMOUIHero::RefreshSlot()
{
	////AMMOPlayer* pl = MMOFacade::getSingleton().GetMainPlayer();
	//if(Player==NULL)return;

	//if(Player->ServerHeroID<=0)return;

	//UActorAttribute* Attributes = Player->HeroAttributes;
	//if(Attributes==NULL)return;

	//UGFxObject *DataProvider = GFXPlayer->CreateArray();
	//if(DataProvider==NULL)return;

	//INT list[] = {E_NES_HEAD,E_NES_SHOULDER,E_NES_BODY,E_NES_LEG,E_NES_ARM,E_NES_WRIST_L,E_NES_FINGER_L,E_NES_AVATAR,E_NES_NECK,E_NES_WAIST,E_NES_FOOT,-1,E_NES_WRIST_R,E_NES_FINGER_R,E_NES_MAIN_HAND};
	//for(int i=0;i<15;i++)
	//{
	//	UGFxObject * SlotObj = GFXPlayer->CreateObject(TEXT("ActionScript3.ItemSlotData"));

	//	SlotObj->SetString(TEXT("slotName") ,FString::Printf(TEXT("slot_%d"),i)	);
	//	SlotObj->SetInt(TEXT("EquipID")	,E_CONSLOT_HERO_EQUIPPED );
	//	//SlotObj->SetInt(TEXT("itemid")	,list[i] );

	//	SlotObj->SetInt(TEXT("slotID")	,list[i] );
	//	SlotObj->SetInt(TEXT("slotType")	,ESPType_Hero );
	//	SlotObj->SetInt(TEXT("containerID"),E_CONSLOT_HERO_EQUIPPED );

	//	UEquip* equip = NULL;
	//	if( list[i]>=0 )
	//	{
	//		equip = Player->ItemManager->EquippedItemList_Hero(list[i]);
	//	}
	//	if( equip )
	//	{
	//		SlotObj->SetString(TEXT("iconName")	,MMOFacade::getSingleton().GetItemIconNamePersonalized( equip ) );
	//		SlotObj->SetInt(TEXT("itemColor")	,equip->ItemColor );
	//		SlotObj->SetInt(TEXT("itemIndex")	,equip->ItemId );
	//		SlotObj->SetString(TEXT("itemtype")	,TEXT("") );
	//		SlotObj->SetInt(TEXT("quantity")	,0 );
	//	}
	//	else
	//	{
	//		SlotObj->SetString(TEXT("iconName")	,TEXT("") );
	//		SlotObj->SetInt(TEXT("itemColor")	,0 );
	//		SlotObj->SetInt(TEXT("itemIndex")	,-1 );
	//		SlotObj->SetString(TEXT("itemtype")	,TEXT("") );
	//		SlotObj->SetInt(TEXT("quantity")	,0 );
	//	}
	//	DataProvider->SetElementObject( i, SlotObj );
	//}

	//eventCallUpdateSlot(DataProvider);

}

//------------------------------------------------------------------------------------------------------------

UBOOL UMMOUIHero::OnLeftMousePress(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault) 
{ 
	if(bDefault)return FALSE;
	return FALSE; 
} 

UBOOL UMMOUIHero::OnLeftMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;
	return FALSE;
}

UBOOL UMMOUIHero::OnRightMouseClick(class UGFxObject* mc,BYTE SlotType,BYTE ContainerID,INT SlotID,UBOOL bDefault)
{
	if(bDefault)return FALSE;

	if(SlotType == ESPType_Hero)
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(ContainerID, SlotID);
		if(info)
		{
			UItem* item = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);

			ClientActionCondition_Hero_EquipItem checker;

			FClientActionConditionContex contex;
			contex.Item = info;
			contex.ItemTemplate = item;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}

			UEquip* equip = Cast<UEquip>(item);
			if(equip)
			{
				INT DestSlot = MMOFacade::getSingleton().GetEmptyItemSlot(E_CONSLOT_DEFAULT);
				if(DestSlot >= 0)
				{
					CMessageHandler::GetInstance().Send_MoveItem(E_CONSLOT_DEFAULT, DestSlot, ContainerID, SlotID);
				}
				else
				{
					FString msg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
					MMOUI::GetUIManager()->delegateAddScreenMessage(msg,MsgInfoType_Table,TEXT("MMOErrorInfo.EClientActionConditionCheckType.ECACCT_Bag_Full"));
				}
				return TRUE;
			}
			else
			{
				warnf( NAME_Error, TEXT("[UMMOUIHero::OnRightMouseClick] move equip faild: %s"), *item->GetDisplayName() );
			}
		}
	}

	return FALSE;
}

void AskEquipBind(ItemInfo* info);
UBOOL UMMOUIHero::OnDragDrop(class UGFxObject* SrcSlotMC,class UGFxObject* DestSlotMC, BYTE SrcSlotType,BYTE SrcContainerID,INT SrcSlotID,BYTE DestSlotType,BYTE DestContainerID,INT DestSlotID)
{
	// zhaojun@snda
	if (SrcContainerID == DestContainerID && SrcSlotID == DestSlotID)
		return FALSE;

	if(SrcSlotType == ESPType_Hero && DestSlotType == ESPType_Invalid) // discard
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
		if(info)
		{
			ClientActionCondition_EquipItem checker;

			FClientActionConditionContex contex;
			contex.Item = info;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}

			MMOUI::GetUIManager()->delegateDeleteItem(SrcContainerID, SrcSlotID);
		}

		return TRUE;
	}
	else if(SrcSlotType == ESPType_Hero && DestSlotType == ESPType_Hero)  // swap
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(SrcContainerID, SrcSlotID);
		if(info)
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);
			//UItem* destItemTemp = MMOFacade::getSingleton().FindItemTemplate(DestContainerID, DestSlotID);

			ClientActionCondition_EquipItem checker;

			FClientActionConditionContex contex;
			contex.Item = info;
			contex.ItemTemplate = itemTemp;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}

			INT srcSlotEquipType = SrcSlotMC->GetInt(TEXT("itemid"));
			INT destSlotEquipType = DestSlotMC->GetInt(TEXT("itemid"));

			// slots of different types cannot be swapped
			if(srcSlotEquipType != destSlotEquipType)
			{
				return TRUE;
			}

			CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);

			return TRUE;
		}
	}
	else if(SrcSlotType == ESPType_Hero && DestSlotType == ESPType_Bag) // unequip
	{
		FClientItemInfo* info = MMOFacade::getSingleton().FindItem(DestContainerID, DestSlotID);
		if(info) // if the target slot has equipment that is not bound
		{
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate(info->m_nTemplateID);

			ClientActionCondition_Hero_EquipItem checker;

			FClientActionConditionContex contex;
			contex.Item = info;
			contex.ItemTemplate = itemTemp;

			if(!checker.CheckClientActionCondition(contex))
			{
				return TRUE;
			}

			UEquip* equip = Cast<UEquip>(itemTemp);

			if(itemTemp && equip && info->m_byBindType != E_BINDTYPE_BINDED
				&& equip->ItemBindType == E_BINDTYPE_BOE)
			{
				AskEquipBind(info);
			}
			else
			{
				CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
			}
		}
		else
		{
			CMessageHandler::GetInstance().Send_MoveItem(DestContainerID, DestSlotID, SrcContainerID, SrcSlotID);
		}

		return TRUE;
	}

	return FALSE;
}

