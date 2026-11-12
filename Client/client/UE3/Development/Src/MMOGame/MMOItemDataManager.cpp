#include "stdafx.h"
#include "MMOItemDataManager.h"
#include "MMOFacade.h"

#if !SHIPPING_PC_GAME
TMap< FString, TArray<FString> > g_clientTabFiles;

void StatTabHeader(const FString& tabFileName, const FString& headerName, const FString& headerUsage)
{
	bool isClientUsage = headerUsage == TEXT("Client") || headerUsage == TEXT("Both");
	// N/A means confidential || headerUsage == TEXT("N/A");
	if(isClientUsage)
	{
		return;
	}

	TArray<FString>* tabFile = (TArray<FString>*)g_clientTabFiles.Find(tabFileName);
	if(!tabFile)
	{
		tabFile = &g_clientTabFiles.Set(tabFileName, TArray<FString>());
	}

	INT idx = 0;
	if(!tabFile->FindItem(headerName, idx))
	{
		tabFile->AddItem(headerName);
	}
}

void _DumpUsedTabHeaders()
{
	FString buf;

	for (TMap<FString, TArray<FString> >::TIterator itor(g_clientTabFiles); itor; ++itor)
	{
		TArray<FString>& val = itor.Value();
		buf += FString(TEXT("\n\n====TabFile:")) + itor.Key() + TEXT("\n");
		for (INT i = 0; i < val.Num(); ++i)
		{
			buf += val(i) + TEXT("\n");
		}
	}

	appSaveStringToFile(buf, TEXT("UsedTabHeader.txt"));
}
#endif


void ItemTemplateDataProcessor::ParseData( UItem* obj, class CTabFile* dataRow, int rowIdx )
{
	TParseData<MMOItemDataManager>(obj, dataRow, rowIdx);
}

void ItemTemplateDataProcessor::PostInitData()
{
	MMOItemDataManager& itemMgr = MMOItemDataManager::getSingleton();
	MMOItemDataManager::DataTypeMap::iterator it = itemMgr._dataMap.begin();
	for (; it != itemMgr._dataMap.end(); ++it)
	{
		MMOFacade::getSingleton().AddItemTemplate(it->first, it->second);
	}

	LoadAllItemTemplate();
}

void ItemTemplateDataProcessor::LoadAllItemTemplate()
{
	if(M_MMO_HD)
	{
		SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_BLUE);
		warnf(NAME_Log, TEXT("MMOItemDataManager::LoadAllItemTemplate %d"), MMOItemDataManager::getSingleton()._dataMap.size());
		CLEAR_WARN_COLOR();
		MMOItemDataManager::DataTypeMap::iterator it = MMOItemDataManager::getSingleton()._dataMap.begin();
		for (; it != MMOItemDataManager::getSingleton()._dataMap.end(); ++it)
		{
			UItem* item = it->second;
			//item->GetDisplayIcon();
			//item->GetDisplayLargeIcon();
			//item->GetDisplayLargeIconFemale();
			item->GetDropMesh();
		}
	}
}

void EquipTemplateDataProcessor::ParseData( UEquip* obj, class CTabFile* dataRow, int rowIdx )
{
	ItemTemplateDataProcessor::TParseData<MMOEquipDataManager>(obj, dataRow, rowIdx);

	obj->ItemSubSubType = TReadDataValue_FString<MMOEquipDataManager>(TEXT("SubSubType"), dataRow, rowIdx);
	obj->MaxRefineLevel = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MaxRefineLevel"), dataRow, rowIdx);
	obj->FixAttrbutes.WeaponPhyAtkLo = TReadDataValue_Int<MMOEquipDataManager>(TEXT("PhyAtk"), dataRow, rowIdx);
	obj->FixAttrbutes.WeaponPhyAtkHi = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MaxPhyAtk"), dataRow, rowIdx);
	obj->FixAttrbutes.WeaponMagAtkLo = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MagAtk"), dataRow, rowIdx);
	obj->FixAttrbutes.WeaponMagAtkHi = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MaxMagAtk"), dataRow, rowIdx);
	obj->FixAttrbutes.WeaponTaoAtkLo = TReadDataValue_Int<MMOEquipDataManager>(TEXT("TaoAtk"), dataRow, rowIdx);
	obj->FixAttrbutes.WeaponTaoAtkHi = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MaxTaoAtk"), dataRow, rowIdx);

	obj->FixAttrbutes.PhyDefLo = TReadDataValue_Int<MMOEquipDataManager>(TEXT("PhyDef"), dataRow, rowIdx);
	obj->FixAttrbutes.PhyDefHi = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MaxPhyDef"), dataRow, rowIdx);
	obj->FixAttrbutes.MagDefLo = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MagDef"), dataRow, rowIdx);
	obj->FixAttrbutes.MagDefHi = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MaxMagDef"), dataRow, rowIdx);

	obj->FixAttrbutes.AttackRange = TReadDataValue_Int<MMOEquipDataManager>(TEXT("AtkRange"), dataRow, rowIdx);

	obj->FixAttrbutes.MaxHp = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MaxHp"), dataRow, rowIdx);
	obj->FixAttrbutes.MaxMp = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MaxMp"), dataRow, rowIdx);

	obj->FixAttrbutes.Accurate = TReadDataValue_Int<MMOEquipDataManager>(TEXT("Accurate"), dataRow, rowIdx);
	obj->FixAttrbutes.Agility = TReadDataValue_Int<MMOEquipDataManager>(TEXT("Agility"), dataRow, rowIdx);
	obj->FixAttrbutes.AtkSpeed = TReadDataValue_Int<MMOEquipDataManager>(TEXT("AtkSpeed"), dataRow, rowIdx);

	obj->FixAttrbutes.MagDodge = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MagDodge"), dataRow, rowIdx);

	obj->FixAttrbutes.WeatherAttrID = TReadDataValue_Int<MMOEquipDataManager>(TEXT("WeatherAttrID"), dataRow, rowIdx);
	//obj->FixAttrbutes.FoundationRandomAttribute = TReadDataValue_Int<MMOEquipDataManager>(TEXT("FoundationRandomAttribute"), dataRow, rowIdx);
	
	//obj->FixAttrbutes.FixedRandomAttribute1 = TReadDataValue_Int<MMOEquipDataManager>(TEXT("FixedRandomAttribute1"), dataRow, rowIdx);
	//obj->FixAttrbutes.FixedRandomAttribute2 = TReadDataValue_Int<MMOEquipDataManager>(TEXT("FixedRandomAttribute2"), dataRow, rowIdx);
	//obj->FixAttrbutes.FixedRandomAttribute3 = TReadDataValue_Int<MMOEquipDataManager>(TEXT("FixedRandomAttribute3"), dataRow, rowIdx);
	//obj->FixAttrbutes.FixedRandomAttribute4 = TReadDataValue_Int<MMOEquipDataManager>(TEXT("FixedRandomAttribute4"), dataRow, rowIdx);

	obj->FixAttrbutes.BasePow = TReadDataValue_Int<MMOEquipDataManager>(TEXT("BasePow"), dataRow, rowIdx);

	//obj->EquipedSkills(0).WeaponRuneID = TReadDataValue_Int<MMOEquipDataManager>(TEXT("WeaponRuneID1"), dataRow, rowIdx);
	//obj->EquipedSkills(1).WeaponRuneID = TReadDataValue_Int<MMOEquipDataManager>(TEXT("WeaponRuneID2"), dataRow, rowIdx);
	//obj->EquipedSkills(2).WeaponRuneID = TReadDataValue_Int<MMOEquipDataManager>(TEXT("WeaponRuneID3"), dataRow, rowIdx);
	//obj->EquipedSkills(3).WeaponRuneID = TReadDataValue_Int<MMOEquipDataManager>(TEXT("WeaponRuneID4"), dataRow, rowIdx);
	//obj->EquipedSkills(4).WeaponRuneID = TReadDataValue_Int<MMOEquipDataManager>(TEXT("WeaponRuneID5"), dataRow, rowIdx);
	//obj->EquipedSkills(5).WeaponRuneID = TReadDataValue_Int<MMOEquipDataManager>(TEXT("WeaponRuneID6"), dataRow, rowIdx);
	//obj->MaxRuneCount = TReadDataValue_Int<MMOEquipDataManager>(TEXT("MaxRuneCount"), dataRow, rowIdx);
	UBOOL isHasSecondRune = ( TReadDataValue_Int<MMOEquipDataManager>(TEXT("SecondRuneCostMax"), dataRow, rowIdx) > 0 );
	obj->MaxRuneCount = ( isHasSecondRune ? 2 : 1 );

	obj->IsIconSame = TReadDataValue_Int<MMOEquipDataManager>(TEXT("IsIconSame"), dataRow, rowIdx);

	obj->NeedIdentify = (UBOOL)TReadDataValue_Int<MMOEquipDataManager>(TEXT("Identified"), dataRow, rowIdx);
	obj->SocketCount = TReadDataValue_Int<MMOEquipDataManager>(TEXT("SocketCountMax"), dataRow, rowIdx);
	//obj->CanDecompose = (UBOOL)TReadDataValue_Int<MMOEquipDataManager>(TEXT("CanDecompose"), dataRow, rowIdx);

	//obj->GetEquipAppearenceTemplate() = TReadDataValue_Object<UEquipAppearence, MMOEquipDataManager>(TEXT("EquipAppearenceTemplate"), dataRow, rowIdx);
	obj->EquipAppearenceTemplate_Str = TReadDataValue_FString<MMOEquipDataManager>(TEXT("EquipAppearenceTemplate"), dataRow, rowIdx);
	obj->AdvancedEquipAppearenceTemplate_Str = TReadDataValue_FString<MMOEquipDataManager>(TEXT("AdvancedEquipAppearenceTemplate"), dataRow, rowIdx);

	obj->CanRepair = TReadDataValue_Int<MMOEquipDataManager>(TEXT("CanRepair"), dataRow, rowIdx);
	obj->RepairCost = TReadDataValue_Int<MMOEquipDataManager>(TEXT("RepairCost"), dataRow, rowIdx);
	obj->RepairCostSpecial = TReadDataValue_Int<MMOEquipDataManager>(TEXT("SpecialRepairCost"), dataRow, rowIdx);
	obj->IdentifyCost = TReadDataValue_Int<MMOEquipDataManager>(TEXT("IdentifyCost"), dataRow, rowIdx);
	obj->RandomizeCost = TReadDataValue_Int<MMOEquipDataManager>(TEXT("RandomizeCost"), dataRow, rowIdx);

	obj->RequireMaxPhyAtk = TReadDataValue_Int<MMOEquipDataManager>(TEXT("RequireMaxPhyAtk"), dataRow, rowIdx);
	obj->RequireMaxMagAtk = TReadDataValue_Int<MMOEquipDataManager>(TEXT("RequireMaxMagAtk"), dataRow, rowIdx);
	obj->RequireMaxTaoAtk = TReadDataValue_Int<MMOEquipDataManager>(TEXT("RequireMaxTaoAtk"), dataRow, rowIdx);
}

void EquipTemplateDataProcessor::PostInitData()
{
	MMOEquipDataManager::DataTypeMap::const_iterator it = MMOEquipDataManager::getSingleton()._dataMap.begin();
	for (; it != MMOEquipDataManager::getSingleton()._dataMap.end(); ++it)
	{
		MMOFacade::getSingleton().AddItemTemplate(it->first, it->second);
	}

	LoadAllItemTemplate();
}

void EquipTemplateDataProcessor::LoadAllItemTemplate()
{
	if(M_MMO_HD)
	{
		SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_BLUE);
		warnf(NAME_Log, TEXT("MMOEquipDataManager::LoadAllItemTemplate %d"), MMOEquipDataManager::getSingleton()._dataMap.size());
		CLEAR_WARN_COLOR();
		MMOEquipDataManager::DataTypeMap::iterator it = MMOEquipDataManager::getSingleton()._dataMap.begin();
		for (; it != MMOEquipDataManager::getSingleton()._dataMap.end(); ++it)
		{
			UItem* item = it->second;
			//item->GetDisplayIcon();
			//item->GetDisplayLargeIcon();
			//item->GetDisplayLargeIconFemale();
			item->GetDropMesh();
		}
	}
}
