#ifndef __MMOITEMDATAMANAGER_H__
#define __MMOITEMDATAMANAGER_H__

#include "MMODataManager.h"
#include "MMOTemplateObjectFinder.h"
#include "MMOSkillManager.h"
#include "MMOEquipManager.h"

struct ItemTemplateDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("item.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static UItem* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		UItem* item = Cast<UItem>(GEngine->StaticConstructObject(UItem::StaticClass()));
		item->AddToRoot();
		return item;
	}

	static void DestroyDataObject(UItem* obj)
	{
		obj->RemoveFromRoot();
	}

	static void ParseData(UItem* obj, class CTabFile* dataRow, int rowIdx);

	template<typename DataMgr>
	static void TParseData(UItem* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(UItem* obj)
	{
		return obj->GetFullName();
	}

	static void PostInitData();
	static void LoadAllItemTemplate();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(UItem* obj, TSet<FString>& outResList)
	{
		if(obj)
		{
			obj->CollectReferencedResource(outResList);
		}
	}
#endif
};

typedef TDataManger< UItem, ItemTemplateDataProcessor > MMOItemDataManager;

struct EquipTemplateDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("item_armor.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static UEquip* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		UEquip* item = Cast<UEquip>(GEngine->StaticConstructObject(UEquip::StaticClass()));
		item->AddToRoot();
		return item;
	}

	static void DestroyDataObject(UEquip* obj)
	{
		obj->RemoveFromRoot();
	}

	static void ParseData(UEquip* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID");
	}

	static FString FormatAsString(UEquip* obj)
	{
		return obj->GetFullName();
	}

	static void PostInitData();

	static void LoadAllItemTemplate();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(UEquip* obj, TSet<FString>& outResList)
	{
		_CollectReferencedResource(obj, outResList);

		if(obj && obj->GetEquipAppearenceTemplate())
		{
			obj->GetEquipAppearenceTemplate()->CollectReferencedResource(outResList);
		}

		if(obj && obj->GetAdvancedEquipAppearenceTemplate())
		{
			obj->GetAdvancedEquipAppearenceTemplate()->CollectReferencedResource(outResList);
		}
	}
#endif
};

typedef TDataManger< UEquip, EquipTemplateDataProcessor > MMOEquipDataManager;


//------------------------------------------------------------------------------------------------



template<typename DataMgr>
void ItemTemplateDataProcessor::TParseData( UItem* obj, class CTabFile* dataRow, int rowIdx )
{
	obj->ItemId = TReadDataValue_Int<DataMgr>(TEXT("ID"), dataRow, rowIdx);
	obj->ItemName = TReadDataValue_FString<DataMgr>(TEXT("Name"), dataRow, rowIdx);
	obj->ItemType = TReadDataValue_FString<DataMgr>(TEXT("Type"), dataRow, rowIdx);
	obj->ItemSubType = TReadDataValue_FString<DataMgr>(TEXT("SubType"), dataRow, rowIdx);
	obj->RequireLevel = TReadDataValue_Int<DataMgr>(TEXT("RequireLevel"), dataRow, rowIdx);

	//INT tmpVal = TReadDataValue_Int<DataMgr>(TEXT("RequireClass"), dataRow, rowIdx);
	//memcpy(&obj->RequireClass, &tmpVal, sizeof(FSpell_ClassType));

	FString tmpVal = TReadDataValue_FString<DataMgr>(TEXT("RequireClass"), dataRow, rowIdx);
	// NOTE: adding a new class requires modifying the following related code
	obj->RequireClass.bClassType1 = tmpVal.InStr(TEXT("clsWarrior")) != INDEX_NONE;
	obj->RequireClass.bClassType2 = tmpVal.InStr(TEXT("clsMage")) != INDEX_NONE;
	obj->RequireClass.bClassType3 = tmpVal.InStr(TEXT("clsRogue")) != INDEX_NONE;
	obj->RequireClass.bClassType4 = tmpVal.InStr(TEXT("clsArcher")) != INDEX_NONE;
	obj->RequireClass.bClassType5 = tmpVal.InStr(TEXT("clsNpc")) != INDEX_NONE; // this is NPC
	obj->RequireClass.bClassType6 = tmpVal.InStr(TEXT("clsTaoist")) != INDEX_NONE; // the newly added 6 is Taoist
	// 1.05 backport: Dragon Lancer (clsSpear) and Sorcerer (clsSorcerer)
	obj->RequireClass.bClassType7 = tmpVal.InStr(TEXT("clsSpear")) != INDEX_NONE;
	obj->RequireClass.bClassType8 = tmpVal.InStr(TEXT("clsSorcerer")) != INDEX_NONE;

	obj->RequireGender = (EGenderType)TReadDataValue_Int<DataMgr>(TEXT("RequireGender"), dataRow, rowIdx);

	//obj->MaxEquip = TReadDataValue_Int<DataMgr>(TEXT("MaxEquip"), dataRow, rowIdx);
	//obj->MaxOwn = TReadDataValue_Int<DataMgr>(TEXT("MaxOwn"), dataRow, rowIdx);

	obj->ItemColor = (EItemColor)TReadDataValue_Int<DataMgr>(TEXT("Color"), dataRow, rowIdx);
	obj->ItemBindType = (EBIND_TYPE)TReadDataValue_Int<DataMgr>(TEXT("BindType"), dataRow, rowIdx);

	//FString durTypeString = TReadDataValue_Int<DataMgr>(TEXT("DurType"), dataRow, rowIdx);
	//INT refineDataClassId = MMOFacade::getSingleton().FindSerEnumValue( durTypeString, TEXT("EDUR_TYPE"));
	obj->ItemDuration = TReadDataValue_ServerEnum<DataMgr>(TEXT("DurType"), dataRow, rowIdx, TEXT("EDUR_TYPE"));

	obj->DefaultDuration = TReadDataValue_Int<DataMgr>(TEXT("Dur"), dataRow, rowIdx);

	obj->Weight = TReadDataValue_Int<DataMgr>(TEXT("Weight"), dataRow, rowIdx);

	obj->SpellLevel = TReadDataValue_Int<DataMgr>(TEXT("SpellLevel"), dataRow, rowIdx);

	obj->ToUseAbility_SpellID = TReadDataValue_Int<DataMgr>(TEXT("SpellID"), dataRow, rowIdx);
	//if( spellID > 0 ){
	//	UINT Key = 0;
	//	if( obj->ItemType == TEXT("ITEMTYPE_RUNE") ){
	//		//inscription item, special handling here
	//		MMOEquipManager::getSingleton().GetSkillUniqueIDOnWeapon( Key, spellID );
	//	}
	//	else{
	//		Key = MMOSkillManager::CombinateAbilityID( spellID, obj->SpellLevel, 0 );
	//	}
 //		obj->GetToUseAbility() = MMOSkillManager::getSingleton().FindAbilityTemplate( Key );
 //		if(obj->GetToUseAbility()==NULL)
 //		{
 //			M_AddDataError(TEXT("item.txt"), FString::Printf(TEXT("Item %s ability attribute error, cannot find ability with id=%d"), *obj->ItemName,spellID ));
 //		}
	//}
	//else{
	//	obj->GetToUseAbility() = NULL;
	//}

	obj->SelfCoolDown = TReadDataValue_Float<DataMgr>(TEXT("SelfCoolDown"), dataRow, rowIdx);
	obj->GroupCoolDown = TReadDataValue_FString<DataMgr>(TEXT("GroupCoolDown"), dataRow, rowIdx);

	//obj->ItemSubSubType = TReadDataValue_FString<DataMgr>(TEXT("SubSubType"), dataRow, rowIdx);

	obj->SellPrice = TReadDataValue_Int<DataMgr>(TEXT("Price"), dataRow, rowIdx);

	//obj->UsedStatus = (EITEM_COMBAT_TYPE)TReadDataValue_Int<DataMgr>(TEXT("UsedStatus"), dataRow, rowIdx);

	obj->Drop = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("CanDrop"), dataRow, rowIdx);
	//obj->Trade = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("Trade"), dataRow, rowIdx);
	//obj->Storage = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("Storage"), dataRow, rowIdx);
	obj->CanSell = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("CanSell"), dataRow, rowIdx);
	//obj->RecordTradeLog = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("Log"), dataRow, rowIdx);
	obj->Expensive = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("Expensive"), dataRow, rowIdx);
	obj->CanDecompose = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("CanDecompose"), dataRow, rowIdx);

	obj->Script = TReadDataValue_FString<DataMgr>(TEXT("Script"), dataRow, rowIdx);
	obj->DisplayIcon_Str = TReadDataValue_FString<DataMgr>(TEXT("DisplayIcon"), dataRow, rowIdx);
	//obj->DisplayIcon = TReadDataValue_Object<UTexture2D, DataMgr>(TEXT("DisplayIcon"), dataRow, rowIdx);

	obj->Description = TReadDataValue_FString<DataMgr>(TEXT("Description"), dataRow, rowIdx);
	obj->MaxStackNum = TReadDataValue_Int<DataMgr>(TEXT("Dur"), dataRow, rowIdx);

	obj->DropMesh_Str = TReadDataValue_FString<DataMgr>(TEXT("DropMesh"), dataRow, rowIdx);
	//obj->DropMesh = TReadDataValue_Object<UStaticMesh, DataMgr>(TEXT("DropMesh"), dataRow, rowIdx);

	obj->SoundMaterialType = (EItemMaterialType)TReadDataValue_Int<DataMgr>(TEXT("SoundMaterialType"), dataRow, rowIdx);
	obj->SoundActionType = (EItemActionSoundType)TReadDataValue_Int<DataMgr>(TEXT("SoundActionType"), dataRow, rowIdx);
	obj->ClothNoiseType = (EClothNoiseType)TReadDataValue_Int<DataMgr>(TEXT("ClothNoiseType"), dataRow, rowIdx);

	obj->ItemLevel = TReadDataValue_Int<DataMgr>(TEXT("ItemLevel"), dataRow, rowIdx);

	//obj->DisplayLargeIcon = TReadDataValue_Object<UTexture2D, DataMgr>(TEXT("DisplayLargeIcon"), dataRow, rowIdx);
	obj->DisplayLargeIcon_Str = TReadDataValue_FString<DataMgr>(TEXT("DisplayLargeIcon"), dataRow, rowIdx);

	//obj->DisplayLargeIconFemale = TReadDataValue_Object<UTexture2D, DataMgr>(TEXT("DisplayLargeIconFemale"), dataRow, rowIdx);
	obj->DisplayLargeIconFemale_Str = TReadDataValue_FString<DataMgr>(TEXT("DisplayLargeIconFemale"), dataRow, rowIdx);

	//obj->LimitDuration = TReadDataValue_Int<DataMgr>(TEXT("LimitDuration"), dataRow, rowIdx);
	//obj->LimitCount = TReadDataValue_Int<DataMgr>(TEXT("LimitCount"), dataRow, rowIdx);

	obj->Auction = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("Auction"), dataRow, rowIdx);
	obj->AuctionFee1 = TReadDataValue_Int<DataMgr>(TEXT("AuctionFee1"), dataRow, rowIdx);
	obj->AuctionFee2 = TReadDataValue_Int<DataMgr>(TEXT("AuctionFee2"), dataRow, rowIdx);
	obj->AuctionFee3 = TReadDataValue_Int<DataMgr>(TEXT("AuctionFee3"), dataRow, rowIdx);
	obj->AuctionType = TReadDataValue_Int<DataMgr>(TEXT("AuctionType"), dataRow, rowIdx);
	obj->AuctionSubType = TReadDataValue_Int<DataMgr>(TEXT("AuctionSubType"), dataRow, rowIdx);
	obj->AuctionDefaultPrice = TReadDataValue_Int<DataMgr>(TEXT("AuctionDefaultPrice"), dataRow, rowIdx);
	obj->ItemPack = TReadDataValue_Int<DataMgr>(TEXT("ItemPack"), dataRow, rowIdx);
	obj->ItemPack2 = TReadDataValue_Int<DataMgr>(TEXT("ItemPack2"), dataRow, rowIdx);
	obj->ShopTypeSell = TReadDataValue_Int<DataMgr>(TEXT("ShopTypeSell"), dataRow, rowIdx);
	obj->TradeNeedIntimacy = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("TradeNeedIntimacy"), dataRow, rowIdx);
	obj->CanResource = (UBOOL)TReadDataValue_Int<DataMgr>(TEXT("Resource"), dataRow, rowIdx);
}
#endif
