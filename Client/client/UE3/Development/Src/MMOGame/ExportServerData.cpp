#include "stdafx.h"
#include "ExportServerData.h"
#include "Exporter_Kismet.h"
#include "DoodadManager.h"
#include "MMOEngineUtil.h"

#include <sstream>
#include <ios>
#include <fstream>
#include "MMOUtil.h"
//#include <logicmacros.h>
#include "..\Engine\Src\UnNovodexSupport.h"
#include "Stream.h"
#include "MMOChatManager.h"

#if WITH_EDITOR
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "SourceControl.h"
#include "DoodadManager.h"
#include "MMOWeatherManager.h"
#include "MMOItemDataManager.h"
#include "MMOEquipManager.h"
#include "ActivityManager.h"
#include "MMOBuffManager.h"
#include "MMONpcFactory.h"
#include "MMODropManager.h"
#include "MMOSkillManager.h"
#include "MMOQuestData.h"
#include "MMOLoginManager.h"
#include "MMOGuideData.h"
#include "LoadMapTipsManager.h"
#include "MMOHandbookData.h"
#include "MMOJourneyData.h"
#include "Shop.h"

void FixMMOVolume_Blocking();
void FixNPCAIData();
extern void MoveSceneMaterial();

#endif

extern bool g_disableBloodyContent; // oscar@snda

#include <vector>

//#include "NxuStream2\NXU_helper.h"
//#include "NxuStream2\NXU_schema.h"
//#include "NxuStream2\NXU_Streaming.h"

using namespace std;

extern INT g_forceAnimIdx;

#if !SHIPPING_PC_GAME

const INT C_TMPBUF_SZ = 2048;

#define M_USE_MESH_BUILDER 1	

extern bool g_bShowGrid;
extern int g_nCenterZoneX;
extern int g_nCenterZoneY;
extern int g_nRange;
extern BOOL RefreshGrid(void);
extern BOOL LoadGrid(void);
extern bool g_bRecastEx;

extern bool g_EanbleSlopeCheck;
extern bool g_EnableMoveStepHeightCheck;
extern bool g_DebugMove;



GlobalDataExporterList GGlobalDataExporterList;
ServerSceneExporterList GServerSceneExporterList;

#endif // #if !SHIPPING_PC_GAME


IMPLEMENT_CLASS(UMMOMapInfo);

#if !SHIPPING_PC_GAME
UClass* skipClasses[] =
{
	ADoodadSpawnPoint::StaticClass(),
	ADoodadSpawnGroup::StaticClass(),

	AMMONPCSpawnGroup::StaticClass(),
	ANPCServerInstance::StaticClass(),
	ANPCSpawnPoint::StaticClass(),

	ANPCNewSpawnPoint::StaticClass(),
	ANPCNewSpawnGroupMediumActor::StaticClass(),
	ANPCNewSpawnGroupEditorActor::StaticClass(),
	ANPCNewSpawnGroupActor::StaticClass(),

	ATeleportTarget::StaticClass(),
	USeqAct_Modifier::StaticClass(),
	ANote::StaticClass(),
	AMMOServerPoint::StaticClass(),
};

bool SkipCookClass(UObject* Object)
{
	for (INT i = 0; i < ARRAY_COUNT(skipClasses); ++i)
	{
		if(Object->IsA(skipClasses[i]))
		{
			return true;
		}
	}
	return false;
}

UClass* skipRefListClasses[] =
{
	USeqAct_Modifier::StaticClass()
};

bool SkipCookClassWhenMakeRefList(UObject* Object)
{
	for (INT i = 0; i < ARRAY_COUNT(skipRefListClasses); ++i)
	{
		if(Object->IsA(skipRefListClasses[i]))
		{
			return true;
		}
	}
	return false;
}

void CollectReferencedResourceFromDataTable(TSet<FString>& outResList)
{
// 	GuildBuildingDataManager::getSingleton().InitFromFile(filePath, true);
// 	GuildWelfareDataManager::getSingleton().InitFromFile(filePath, true);
// 	GuildDungeonDataManager::getSingleton().InitFromFile(filePath, true);
// 	QuestDoodadDataManager::getSingleton().InitFromFile(filePath, true);
// 	QuestRecycleDataManager::getSingleton().InitFromFile(filePath, true);
// 	RewardQuestCostDataManager::getSingleton().InitFromFile(filePath, true);
	MMOItemDataManager::getSingleton().InitFromFile(TEXT("item.txt"), true);
	MMOEquipDataManager::getSingleton().InitFromFile(TEXT("item_armor.txt"), true);
// 	AchievementDataManager::getSingleton().InitFromFile(filePath, true);
// 	AchievementStatDataManager::getSingleton().InitFromFile(filePath, true);
// 	MMOActivityDataManager::getSingleton().InitFromFile(TEXT("Activity.txt"), true);
// 	MMOActivityRewardBarDataManager::getSingleton().InitFromFile(TEXT("activity_reward.txt"), true);
// 	AuctionCategoryList::getSingleton().InitFromFile(TEXT("AuctionCategory.txt"), true);
// 	MMOHeavenLevelDataManager::getSingleton().InitFromFile(TEXT("spirit.txt"), true);
// 	MMOHeavenGasItemDataManager::getSingleton().InitFromFile(TEXT("air.txt"), true);
// 	MMOHeavenBloodDataManager::getSingleton().InitFromFile(TEXT("blood.txt"), true);
// 	MMOHeavenJustConvertManager::getSingleton().InitFromFile(TEXT("spirit_change.txt"), true);
// 	HelperDataManager::getSingleton().InitFromFile(filePath, true);
 	LoadMapTipsDataManager::getSingleton().InitFromFile(TEXT("loadmaptips.txt"), true);
// 	CongratulationGiftDataManager::getSingleton().InitFromFile(TEXT("master_gift.txt"), true);
// 	SpellVarManager::getSingleton().InitFromFile(TEXT("spell_var.txt"), TRUE, TRUE);
// 	SpellProgessDataManager::getSingleton().InitFromFile(TEXT("spell_progress.txt"));
 	AbilityTemplateDataIndexManager::getSingleton().InitFromFile(TEXT("spell.txt"));	
 	NPCTemplateDataIndexManager::getSingleton().InitFromFile(TEXT("npc.txt"), true);
 	MissileTemplateDataIndexManager::getSingleton().InitFromFile(TEXT("missile.txt"));
 	BuffTemplateDataIndexManager::getSingleton().InitFromFile(TEXT("aura.txt"));

	DoodadManager::getSingleton().LoadDoodadData();	
	MMODropGroupDataManager::getSingleton().InitFromFile(TEXT("drop_group.txt"), true);
 	MMODoodadDropDataManager::getSingleton().InitFromFile(TEXT("doodad_drop_data.txt"), true);

	ChangeMeshItemList::getSingleton().InitFromFile(TEXT("Apperance.txt"), true);

	// add by zhangrui05
	MMOQuestData::getSingleton().LoadQuestDataFromUpk();
	MMOGuideData::getSingleton().LoadGuideData(TEXT("guide.txt"));
	MMOHandbookData::getSingleton().LoadHandbookData(TEXT("handbook.txt"));
	MMOJourneyData::getSingleton().LoadJourneyData(TEXT("journey.txt"));

	BlackMarketData::getSingleton().InitFromFile(TEXT("black_market.txt"), true);

#define M_ADD_CONFIG_ASSET(sec, k, ini, cls) { FString cfgObjPath; \
	GConfig->GetString(sec, k, cfgObjPath, ini);\
	UObject* objDat = UObject::StaticLoadObject(cls, NULL, *cfgObjPath, NULL, LOAD_NoWarn | LOAD_Quiet, NULL);\
	_CollectReferencedResource(objDat, outResList); }


	M_ADD_CONFIG_ASSET(TEXT("Quest"), TEXT("DoodadEffect"), GGameIni, UParticleSystem::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("ItemSound"), TEXT("DropCurrencySound"), GGameIni, USoundCue::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("ItemSound"), TEXT("DropCurrencySwitshSound"), GGameIni, USoundCue::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("ItemSound"), TEXT("DropSwishSound"), GGameIni, USoundCue::StaticClass());

	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("PlayerLevelUpEffect"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UParticleSystem::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("PlayerSkillLearnedEffect"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UParticleSystem::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("PlayerSkillLevelUpEffect"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UParticleSystem::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("PlayerReviveEffect"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UParticleSystem::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("PlayerTeleportLeaveEffect"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UParticleSystem::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("PlayerTeleportEnterEffect"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UParticleSystem::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("PlayerAchievementEffect"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UParticleSystem::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("SabacStatueEffect"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UParticleSystem::StaticClass());

	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODecalManager"), TEXT("SelfDecal"), TEXT("..\\..\\MMOGame\\Config\\DefaultMMOGame.ini"), UMaterialInstanceConstant::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODecalManager"), TEXT("FriendDecal"), TEXT("..\\..\\MMOGame\\Config\\DefaultMMOGame.ini"), UMaterialInstanceConstant::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODecalManager"), TEXT("EnemyDecal"), TEXT("..\\..\\MMOGame\\Config\\DefaultMMOGame.ini"), UMaterialInstanceConstant::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODecalManager"), TEXT("DefaultDecal"), TEXT("..\\..\\MMOGame\\Config\\DefaultMMOGame.ini"), UMaterialInstanceConstant::StaticClass());
	

	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("BindDropMesh1"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("BindDropMesh2"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("BindDropMesh3"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("BindDropMesh4"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("BindDropMesh5"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());

	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("GoldDropMesh1"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("GoldDropMesh2"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("GoldDropMesh3"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("GoldDropMesh4"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMODropActor"), TEXT("GoldDropMesh5"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());

	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("StallMeshName1"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());
	M_ADD_CONFIG_ASSET(TEXT("MMOGame.MMOPlayer"), TEXT("StallMeshName2"), TEXT("..\\..\\MMOGame\\Config\\MMOMMOGame.ini"), UStaticMesh::StaticClass());

	M_ADD_CONFIG_ASSET(TEXT("ChatColor"), TEXT("LabaIconPath"), TEXT("..\\..\\MMOGame\\Config\\DefaultUIColor.ini"), UTexture2D::StaticClass());

	{
		INT imgNum = 1;
		GConfig->GetInt(TEXT("LoadingBg"), TEXT("Count"), imgNum, GUIIni);

		for (INT i = 0; i < imgNum; ++i)
		{
			FString imgPath = TEXT("MMOUIImage.Loading.bg");
			imgPath += appItoa(i);
			UObject* objDat = UObject::StaticLoadObject( UTexture2D::StaticClass() , NULL , *imgPath , NULL, LOAD_NoWarn , NULL);
			_CollectReferencedResource(objDat, outResList);
		}
	}


	MMOItemDataManager::getSingleton().CollectReferencedResource(outResList);
	MMOEquipDataManager::getSingleton().CollectReferencedResource(outResList);

	AbilityTemplateDataIndexManager::getSingleton().CollectReferencedResource(outResList);
	NPCTemplateDataIndexManager::getSingleton().CollectReferencedResource(outResList);
	MissileTemplateDataIndexManager::getSingleton().CollectReferencedResource(outResList);
	BuffTemplateDataIndexManager::getSingleton().CollectReferencedResource(outResList);

	MMODropGroupDataManager::getSingleton().CollectReferencedResource(outResList);
	MMODoodadDropDataManager::getSingleton().CollectReferencedResource(outResList);
	MMODoodadTemplateDataManager::getSingleton().CollectReferencedResource(outResList);

	ChangeMeshItemList::getSingleton().CollectReferencedResource(outResList);

	GuideDataManager::getSingleton().CollectReferencedResource(outResList);
	HandbookDataManager::getSingleton().CollectReferencedResource(outResList);
	JourneyDataManager::getSingleton().CollectReferencedResource(outResList);
}

bool SkipPrepareForSaving(UPackage* Package, UObject* Object, UBOOL bIsSavedInSeekFreePackage)
{
	if(Object->GetClass()->GetDefaultObject() == Object)
	{
		return false;
	}

#if 1
	
// 	if(Object->IsA(ADoodadSpawnPoint::StaticClass()))
// 	{
// 		UClass* Class = Object->GetClass();
// 		//if (Class->ClassFlags & CLASS_HasCrossLevelRefs)
// 		{
// 			for (TFieldIterator<UProperty> PropIt(Class); PropIt; ++PropIt)
// 			{
// 				UProperty* prop = *PropIt;
// 				warnf(TEXT("@@@@@@@@ %s edOnly:%d"), *prop->GetNameCPP(), prop->HasAnyPropertyFlags(CPF_EditorOnly));
// 			}
// 		}
// 	}

	if(SkipCookClass(Object))
	{
		return true;
	}
// 	for (INT i = 0; i < ARRAY_COUNT(skipClasses); ++i)
// 	{
// 		if(Object->IsA(skipClasses[i]))
// 		{
// 			return true;
// 		}
// 	}
#endif

	return false;
}

bool SkipCookMMOObject(UPackage* Package, UObject* Object, UBOOL bIsSavedInSeekFreePackage)
{
#if 0

#if 0
	static FString skipPkgPath = FString(TEXT("MMOGame\\Content\\MMOContent\\01_Scene\\"));
	
	FFilename PackageFile;
	if (GPackageFileCache->FindPackageFile(*Package->GetName(), NULL, PackageFile, NULL))
	{
		if(PackageFile.InStr(skipPkgPath) != INDEX_NONE)
		{
			//warnf(NAME_Warning, TEXT("==== %s %s"), *Object->GetFullName(), *PackageFile);
			if(Object->IsA(UStaticMesh::StaticClass()))
			{
				Object->ClearFlags(RF_ForceTagExp); 
				Object->ClearFlags(RF_TagExp); 
				Object->SetFlags(RF_Transient);	
				return true;
			}
		}
	}
#endif

	if(SkipCookClass(Object))
	{
		Object->ClearFlags(RF_ForceTagExp); 
		Object->ClearFlags(RF_TagExp); 
		Object->SetFlags(RF_Transient);	
		return true;

	}
// 	for (INT i = 0; i < ARRAY_COUNT(skipClasses); ++i)
// 	{
// 		if(Object->IsA(skipClasses[i]))
// 		{
// 			Object->ClearFlags(RF_ForceTagExp); 
// 			Object->ClearFlags(RF_TagExp); 
// 			Object->SetFlags(RF_Transient);	
// 			return true;
// 		}
// 	}
#endif

	return false;
}

#endif

//------------------------------------------------------------------------
// GameExec editor command line parsing
//------------------------------------------------------------------------

extern bool g_moveImm;
extern bool g_useMidPt;
extern bool bUseGMCmd;
#if M_ENABLE_NET_MSG_BUFF
extern FLOAT g_maxProcTime;
#endif
extern bool g_enableGridLoad;

#if STATS
extern TMap<FString, UBOOL> g_TickCtrlMap;
#endif

void PreloadNPCMesh()
{
	TSet<INT> npcList;
	FString filePath = GetCurrentNPCPreLoadingListFile();
	FArchive* refAr = GFileManager->CreateFileReader(*filePath);
	if(refAr)
	{
		(*refAr) << npcList;
		delete refAr;

		SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_BLUE);
		warnf(NAME_Log, TEXT("PreloadNPCMesh:%d"), npcList.Num());
		CLEAR_WARN_COLOR();

		TSet<INT>::TIterator it(npcList);
		for (; it; ++it)
		{
			MMOFacade::getSingleton().FindNPCTemplate(*it);
		}
	}
	else
	{
		warnf(NAME_Warning, TEXT("PreloadNPCMesh: file not found %s"), *filePath);
	}
}

extern FString g_PetNameColor;


UBOOL GameExec(const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (ParseCommand(&Cmd, TEXT("testchat")))
	{
		FString strFilePath = TEXT("..\\..\\MMOGame\\Config\\test_chat.txt");
		CTabFile tabfile;
		if (tabfile.Init(*strFilePath))
		{
			static int lastIdx = 1;
			for (int i = lastIdx; i < tabfile.GetRowCount() - 1; ++i)
			{
				const TCHAR* tarID = tabfile.GetData(L"TargetID", i);
				int IndexId = appAtoi(tarID);
				int ChannelId = GetTypeFromID(IndexId);
				const TCHAR* msg = tabfile.GetData(L"Message", i);
				const TCHAR* srcID = tabfile.GetData(L"SourceID", i);

				if (FString(msg) == TEXT(""))
				{
					//lastIdx = i;
					warnf(NAME_Warning, TEXT("Msg is null."));
					//GLog->Flush();
					//appExit();
					//exit(0);
					//return TRUE;
					continue;
				}

				MMOChatManager::getSingleton().ReceiveMessage(cltNormal, ChannelId, IndexId,
					msg, srcID, appAtoi(srcID));

				TArray<ChatMessage>& ChatBuffList = MMOChatManager::getSingleton().GetChatBuffList();
				if (ChatBuffList.Num() > 2048)
				{
					ChatBuffList.Empty();
				}
			}
		}
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("PreloadNPCMesh")))
	{
		PreloadNPCMesh();
		return TRUE;
	}

	if( ParseCommand(&Cmd, TEXT("petname"))  )
	{
		g_PetNameColor = Cmd;
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("ANIMFPS")))
	{
		g_forceAnimIdx = Min(4, Max(0, appAtoi(Cmd)));
		SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_BLUE);
		warnf(NAME_Log, TEXT("Force Anim Lv:%d"), g_forceAnimIdx);
		CLEAR_WARN_COLOR();
		return TRUE;
	}


	if( ParseCommand(&Cmd, TEXT("leakmem"))  )
	{
		static std::map<UClass*, TArray<UINT> > sizeMap;
		static INT statCount = 0;
		UBOOL saveToFile = (ParseToken(Cmd, 0) == TEXT("SAVE"));
		if(saveToFile)
		{
			if(statCount > 1)
			{
				const FString Filename = FString::Printf(TEXT("%sLeakMem-%s.csv"), *appGameLogDir(), *appSystemTimeString());
				FArchive* CSVFile = GFileManager->CreateFileWriter(*Filename);
				if(CSVFile)
				{
					FString HeaderRow = TEXT("Class,");
					for (INT i = 1; i <= statCount; ++i)
					{
						if(i < statCount)
						{
							HeaderRow += FString::Printf(TEXT("Mark%d,"), i);
						}
						else
						{
							HeaderRow += FString::Printf(TEXT("Mark%d"), i);
						}
					}

					HeaderRow += LINE_TERMINATOR;
					CSVFile->Serialize( TCHAR_TO_ANSI( *HeaderRow ), HeaderRow.Len() );

					for (std::map<UClass*, TArray<UINT> >::iterator it = sizeMap.begin(); it != sizeMap.end(); ++it)
					{
						FString Row =  it->first->GetName() + TEXT(",");
						for (INT k = 0; k < it->second.Num(); ++k)
						{
							if(k < statCount - 1)
							{
								Row += FString::Printf(TEXT("%d,"), it->second(k));
							}
							else
							{
								Row += FString::Printf(TEXT("%d"), it->second(k));
							}
						}

						Row += LINE_TERMINATOR;
						CSVFile->Serialize( TCHAR_TO_ANSI( *Row ), Row.Len() );
					}

					CSVFile->Close();
					delete CSVFile;

					SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_BLUE);
					warnf(NAME_Log, TEXT("Mark memory saved to:%s"), *Filename);
					CLEAR_WARN_COLOR();
				}
			}
		}
		else
		{
			FObjectIterator It;
			for (; It; ++It)
			{
				UObject* obj = (*It);
				UClass* cls = obj->GetClass();
				std::map<UClass*, TArray<UINT> >::iterator findIt = sizeMap.find(cls);
				FBufferArchive memAr;
				obj->Serialize(memAr);
				if(findIt != sizeMap.end())
				{
					if(findIt->second.Num() < statCount + 1)
					{
						findIt->second.AddItem(memAr.TotalSize());
					}
					else
					{
						int idx = findIt->second.Num() - 1;
						UINT val = findIt->second(idx) + memAr.TotalSize();
						findIt->second(idx) = val;
					}
				}
				else
				{
					TArray<UINT> tmpArr;
					tmpArr.InsertZeroed(0, statCount + 1);
					tmpArr(statCount) = memAr.TotalSize();
					sizeMap[cls] = tmpArr;
				}
			}

			++statCount;
			SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_BLUE);
			warnf(NAME_Log, TEXT("Mark memory leak:%d"), statCount);
			CLEAR_WARN_COLOR();
		}

		return TRUE;
	}

#if STATS

	if( ParseCommand(&Cmd, TEXT("tick"))  )
	{
		FString Parameter(ParseToken(Cmd, 0));
		//Parameter = Parameter.ToUpper();

		if(g_TickCtrlMap.HasKey(Parameter))
		{
			UBOOL* val = g_TickCtrlMap.Find(Parameter);
			if(val)
			{
				g_TickCtrlMap.Set(Parameter, !*val);
				FString dbgMsg = FString::Printf(TEXT("Set %s Tick: %s"), *Parameter, (*val) ? TEXT("On") : TEXT("Off"));
				debugf(TEXT("%s"), *dbgMsg);
				MMOChatManager::getSingleton().AddChatMessage(ECC_System, dbgMsg, TEXT("Debug"));
			}
		}
		else
		{
			FString dbgMsg = FString::Printf(TEXT("Tick switch not found: %s"), *Parameter);
			debugf(TEXT("%s"), *dbgMsg);
			MMOChatManager::getSingleton().AddChatMessage(ECC_System, dbgMsg, TEXT("Debug"));
		}
		return TRUE;
	}

	if( ParseCommand(&Cmd, TEXT("ticks"))  )
	{
		TLookupMap<FString> keys;
		INT cnt = g_TickCtrlMap.GetKeys(keys);
		debugf(TEXT("============Ctrl Ticks:"));
		for (INT i = 0; i < keys.Num(); ++i)
		{
			debugf(TEXT("%s"), *(*keys.GetItem(i)));
		}
		return TRUE;
	}

	if( ParseCommand(&Cmd, TEXT("tickso"))  )
	{
		TLookupMap<FString> keys;
		INT cnt = g_TickCtrlMap.GetKeys(keys);
		for (INT i = 0; i < keys.Num(); ++i)
		{
			g_TickCtrlMap.Set(*keys.GetItem(i), TRUE);
		}

		return TRUE;
	}
#endif

	if( ParseCommand(&Cmd, TEXT("grid"))  )
	{
		g_enableGridLoad = !g_enableGridLoad;
		FString dbgMsg = FString::Printf(TEXT("Grid loading: %s"), g_enableGridLoad ? TEXT("On") : TEXT("Off"));
		debugf(TEXT("%s"), *dbgMsg);
		MMOChatManager::getSingleton().AddChatMessage(ECC_System, dbgMsg, TEXT("Debug"));
		return TRUE;
	}

	if( ParseCommand(&Cmd, TEXT("statmem"))  )
	{
		FObjectIterator It;
		std::map<UClass*, UINT> sizeMap;
		FLOAT total = 0;
		for (; It; ++It)
		{
			UObject* obj = (*It);
			UClass* cls = obj->GetClass();
			std::map<UClass*, UINT>::iterator findIt = sizeMap.find(cls);
			FBufferArchive memAr;
			obj->Serialize(memAr);
			if(findIt != sizeMap.end())
			{
				UINT val = findIt->second + memAr.TotalSize();
				findIt->second = val;
			}
			else
			{
				sizeMap[cls] = memAr.TotalSize();
			}

			total += memAr.TotalSize();
		}

		debugf(TEXT("===================================="));
		for (std::map<UClass*, UINT>::iterator i = sizeMap.begin(); i != sizeMap.end(); ++i)
		{
			FLOAT sz = ((FLOAT)i->second / 1024.0f / 1024.0f);
			if(sz > 0.01f)
			{
				debugf(TEXT("%s\t\t\t%.2f MB"), *i->first->GetName(), sz);
			}
		}
		debugf(TEXT("=== Total:%.2f MB"), total / 1024.0f /1024.0f);
		GMalloc->DumpAllocations(*GLog);

		return TRUE;
	}
	
#if STATS

	if(ParseCommand(&Cmd, TEXT("TEXMEM")))
	{
		FString csvFile = appGameLogDir() + TEXT("texture.csv");
		FArchive* CSVFile = GFileManager->CreateFileWriter(*csvFile);
		if( !CSVFile )
		{
			return TRUE;
		}

		FString CSVLine;
		CSVLine = TEXT("TextureName,Size in Bytes, MB\n");
		CSVFile->Serialize(TCHAR_TO_ANSI(*CSVLine), CSVLine.Len());
		TObjectIterator<UObject> It;
		for (; It; ++It)		
		{
			UObject* obj = *It;
			if(obj->IsA(UTexture2D::StaticClass()))
			{
				UTexture2D* tex = Cast<UTexture2D>(obj);

				if(!tex)
				{
					continue;
				}

				FTexture2DResource* res2d = (FTexture2DResource*)tex->Resource;
				if(res2d)
				{
					FString CSVLine = FString::Printf(TEXT("%s,%d,%.2f\n"), *obj->GetPathName(), res2d->GetTextureSize(), (FLOAT)res2d->GetTextureSize() / 1024.0f / 1024.0f);
					CSVFile->Serialize(TCHAR_TO_ANSI(*CSVLine), CSVLine.Len());
				}
			}
		}

		CSVFile->Close();
		delete CSVFile;
		return TRUE;
	}
#endif

	if(ParseCommand(&Cmd, TEXT("dumpanim")))
	{
		FString csvFile = appGameLogDir() + TEXT("animation.csv");
		FArchive* CSVFile = GFileManager->CreateFileWriter(*csvFile);
		if( !CSVFile )
		{
			return TRUE;
		}

		FString CSVLine;
		CSVLine = TEXT("AnimSetName,Size in Bytes, MB\n");
		CSVFile->Serialize(TCHAR_TO_ANSI(*CSVLine), CSVLine.Len());
		TObjectIterator<UObject> It;
		for (; It; ++It)		
		{
			UObject* obj = *It;
			if(obj->IsA(UAnimSet::StaticClass()))
			{
				UAnimSet* anim = Cast<UAnimSet>(obj);

				if(!anim)
				{
					continue;
				}

				FString CSVLine = FString::Printf(TEXT("%s,%d,%.2f\n"), *obj->GetPathName(), anim->GetResourceSize(), (FLOAT)anim->GetResourceSize() / 1024.0f / 1024.0f);
				CSVFile->Serialize(TCHAR_TO_ANSI(*CSVLine), CSVLine.Len());
			}
		}

		CSVFile->Close();
		delete CSVFile;
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("dumpanimseq")))
	{
		FString csvFile = appGameLogDir() + TEXT("animationseq.csv");
		FArchive* CSVFile = GFileManager->CreateFileWriter(*csvFile);
		if( !CSVFile )
		{
			return TRUE;
		}

		FString CSVLine;
		CSVLine = TEXT("AnimSeqName,Size in Bytes, MB\n");
		CSVFile->Serialize(TCHAR_TO_ANSI(*CSVLine), CSVLine.Len());
		TObjectIterator<UObject> It;
		for (; It; ++It)		
		{
			UObject* obj = *It;
			if(obj->IsA(UAnimSet::StaticClass()))
			{
				UAnimSet* anim = Cast<UAnimSet>(obj);

				if(!anim)
				{
					continue;
				}

				for( INT i=0; i < anim->Sequences.Num(); i++ )
				{
					UAnimSequence* AnimSeq = anim->Sequences(i);
					if( AnimSeq )
					{
						FString CSVLine = FString::Printf(TEXT("%s.%s,%d,%.2f\n"), *anim->GetPathName(), *AnimSeq->SequenceName.ToString(), AnimSeq->GetResourceSize(), (FLOAT)AnimSeq->GetResourceSize() / 1024.0f / 1024.0f);
						CSVFile->Serialize(TCHAR_TO_ANSI(*CSVLine), CSVLine.Len());
					}
				}
			}
		}

		CSVFile->Close();
		delete CSVFile;
		return TRUE;
	}

	if( ParseCommand(&Cmd, TEXT("UI"))  )
	{
		FString Parameter(ParseToken(Cmd, 0));
		if( Parameter.Len() > 0 )
		{
			GDrawGFx = appAtoi(*Parameter);
		}
		else
		{
			GDrawGFx = !GDrawGFx;
		}		

		return TRUE;
	}

	if( ParseCommand(&Cmd, TEXT("ShowSelf"))  )
	{
		MMOFacade::getSingleton().GetMainController()->SetShowMainPlayer(
			!MMOFacade::getSingleton().GetMainController()->bShowMainPlayer);
		return TRUE;
	}

#if !SHIPPING_PC_GAME
	if(ParseCommand(&Cmd, TEXT("gm")))
	{
		bUseGMCmd = !bUseGMCmd;
		FString dbgMsg = FString::Printf(TEXT("GM command input mode: %s"), bUseGMCmd ? TEXT("On") : TEXT("Off"));
		debugf(TEXT("%s"), *dbgMsg);
		MMOChatManager::getSingleton().AddChatMessage(ECC_System, dbgMsg, TEXT("Debug"));
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("ReloadCensoredWordList")))
	{
		MMOFacade::getSingleton().ReadCensoredWordList();
		FString dbgMsg = FString::Printf(TEXT("ReadCensoredWordList()"));
		debugf(TEXT("%s"), *dbgMsg);
		MMOChatManager::getSingleton().AddChatMessage(ECC_System, dbgMsg, TEXT("Debug"));
		return TRUE;
	}
	
#endif


	#if M_ENABLE_NET_MSG_BUFF
	if(ParseCommand(&Cmd, TEXT("fullnetfps")))
	{
		FLOAT procFPS = appAtof(Cmd);
		if(procFPS <= 0)
		{
			procFPS = 1;
		}
		g_maxProcTime = 1.0f / procFPS;
		FString dbgMsg = FString::Printf(TEXT("Set network message processing rate: %f FPS"), procFPS);
		debugf(TEXT("%s"), *dbgMsg);
		MMOChatManager::getSingleton().AddChatMessage(ECC_System, dbgMsg, TEXT("Debug"));
		return TRUE;
	}
#endif

	if(ParseCommand(&Cmd, TEXT("CamSmooth")))
	{
		AMMOPlayerCamera* cam = Cast<AMMOPlayerCamera>(MMOFacade::getSingleton().GetMainController()->PlayerCamera);
		if(cam)
		{
			cam->DefaultSmoothMove = !cam->DefaultSmoothMove;
			cam->EnableSmoothCameraPos = cam->DefaultSmoothMove;
			FString dbgMsg = FString::Printf(TEXT("Camera smooth follow: %s"), cam->EnableSmoothCameraPos ? TEXT("On") : TEXT("Off"));
			debugf(TEXT("%s"), *dbgMsg);
			MMOChatManager::getSingleton().AddChatMessage(ECC_System, dbgMsg, TEXT("Debug"));
		}
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("CamMoveSpeed")))
	{
		AMMOPlayerCamera* cam = Cast<AMMOPlayerCamera>(MMOFacade::getSingleton().GetMainController()->PlayerCamera);
		if(cam)
		{
			cam->DefaultMoveSpeed = appAtof(Cmd);
			FString dbgMsg = FString::Printf(TEXT("Camera smooth follow speed set to: %f"), cam->DefaultMoveSpeed);
			debugf(TEXT("%s"), *dbgMsg);
			MMOChatManager::getSingleton().AddChatMessage(ECC_System, dbgMsg, TEXT("Debug"));
		}
		return TRUE;
	}

#if M_ENABLE_NET_MSG_BUFF
	if(ParseCommand(&Cmd, TEXT("fullnet")))
	{
		CMessageHandler::GetInstance()._fullSpeedProcNetMsg = !CMessageHandler::GetInstance()._fullSpeedProcNetMsg;
		FString dbgMsg = FString::Printf(TEXT("Process network messages at full speed: %s"), CMessageHandler::GetInstance()._fullSpeedProcNetMsg ? TEXT("On") : TEXT("Off"));
		debugf(TEXT("%s"), *dbgMsg);
		MMOChatManager::getSingleton().AddChatMessage(ECC_System, dbgMsg, TEXT("Debug"));
		return TRUE;
	}
#endif

#if !MMO_FINAL_SHIPPING

#ifdef _DEBUG
	if(ParseCommand(&Cmd, TEXT("StrMem")))
	{
		warnf(NAME_Warning, TEXT("String Alloc Count:%I64u Size:%I64u MB SizeInMem:%I64u KB"),
			FDefaultStringAllocator::_AllocCount,
			FDefaultStringAllocator::_AllocSize/1024/1024,
			FDefaultStringAllocator::_AllocSizeInMem/1024);
		return TRUE;
	}
#endif

	if(ParseCommand(&Cmd, TEXT("dumpvol")))
	{
		for(TObjectIterator<AMMOVolume_AmbientMusic> It; It; ++It)
		{
			debugf(TEXT("MusicVolumn:%s"), *(*It)->GetName());
		}

		for(TObjectIterator<AMMOVolume_AmbientSound> It; It; ++It)
		{
			debugf(TEXT("SoundVolumn:%s"), *(*It)->GetName());
		}
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("UseMidPt")))
	{
		g_useMidPt = appAtoi(Cmd) != 0;
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("UseMidPt")))
	{
		g_useMidPt = appAtoi(Cmd) != 0;
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("bloody")))
	{
		g_disableBloodyContent = !g_disableBloodyContent;
		debugf(TEXT("bloody:%d"), (INT)g_disableBloodyContent);
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("BackGroundFPS")))
	{
		GSystemSettings.BackGroundFPS = appAtoi(Cmd);
		return TRUE;
	}


	if(ParseCommand(&Cmd, TEXT("TestFCM")))
	{
		UINT8 isAdult = CMessageHandler::GetInstance()._isAdult;

		CMessageHandler::GetInstance()._isAdult = 0;

		M2C_FCM_STATE msg;
		msg.wMsg = m2c_fcm_state;
		msg.nOnlineMinutes = appAtoi(Cmd);
		CMessageHandler::GetInstance().DebugParseMsg((const char*)&msg, sizeof(msg));

		// 		msg.nOnlineMinutes = 4 * 60;
		// 		CMessageHandler::GetInstance().DebugParseMsg((const char*)&msg, sizeof(msg));
		// 
		// 		msg.nOnlineMinutes = 5 * 60 + 1;
		// 		CMessageHandler::GetInstance().DebugParseMsg((const char*)&msg, sizeof(msg));

		CMessageHandler::GetInstance()._isAdult = isAdult;

		return TRUE;
	}
#endif

#if !SHIPPING_PC_GAME

	if(ParseCommand(&Cmd, TEXT("MoveImm")))
	{
		g_moveImm = !g_moveImm;
		return TRUE;
	}
	

	DWORD dwValue = 0;

#if M_HACK_AVATAR_COUNT
	if(ParseCommand(&Cmd, TEXT("HACKAVATAR")))
	{
		extern INT g_hackAvatarCount;
		g_hackAvatarCount = Max(1, appAtoi(Cmd));
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("HACKAVATAR")))
	{
		extern INT g_hackAvatarCount;
		g_hackAvatarCount = Max(1, appAtoi(Cmd));
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("SETHACKAVATAR")))
	{
		extern bool g_enableHackAvatarCount;
		g_enableHackAvatarCount = appAtoi(Cmd) != 0;
		return TRUE;
	}
#endif

#if !MMO_FINAL_SHIPPING

	if(ParseCommand(&Cmd, TEXT("DynamicShadow")))
	{
		GSystemSettings.RestrictWholeSceneDynamicShadowParameters = (appAtoi(Cmd) != 0);
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("DynamicShadowR")))
	{
		GSystemSettings.RestrictWholeSceneDynamicShadowDistance = appAtof(Cmd);
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("RESCALLSTACK")))
	{
		SaveResourceLoadingCallStack();
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("RESETRESCALLSTACK")))
	{
		ResetLoadingCallStackRecord();
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("RENDCALLSTACK")))
	{
#if M_DEBUG_RENDER_CALLSTACK
		g_enableRenderingCallStackTrace = true;

		g_capRenderCallStackCount = appAtoi(Cmd);
#endif
		return TRUE;
	}

	if(ParseCommand(&Cmd, TEXT("DRAWCALLSTACK")))
	{
#if M_DEBUG_RENDER_CALLSTACK
		g_enableRenderingDrawCallStackTrace = true;

		g_capRenderDrawCallStackCount = appAtoi(Cmd);
#endif
		return TRUE;
	}

#endif // !MMO_FINAL_SHIPPING

	if( ParseCommand(&Cmd, TEXT("ESSCENE"))
		|| ParseCommand(&Cmd, TEXT("ESMAP")))
	{
		GWarn->MapCheck_Clear();

		INT curStep = 1;
		const INT steps = 3;
		//------------------------------------------------------------------------
		// Binary scene file
		//------------------------------------------------------------------------
		GWarn->StatusUpdatef(curStep++, steps, TEXT("Exporting Server Scene..."));
		ExportBinaryServerScene();

		GWarn->MapCheck_ShowConditionally();
		return TRUE;
	}
	else if(Cmd[0] == TEXT('.'))
	{
		// spawn npctype npccnt
		CMessageHandler::GetInstance().Send_GMCommand(Cmd + 1);

		return TRUE;
	}
	else if( ParseCommand(&Cmd, TEXT("LOGID")) )
	{
		FOutputDevice::EnableLogID( FName(Cmd), 
			!FOutputDevice::IsLogIDEnabled(FName(Cmd)) );
		return TRUE;
	}
	else if( ParseCommand(&Cmd, TEXT("DUMPRT")) )
	{
#if WANTS_DRAW_MESH_EVENTS
		GEnableDumpRenderTargets = TRUE;
		GRenderTargetIndex = 0;
		GProfilingGPU = TRUE;
#endif
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("Recast")))
	{
		g_bRecastEx = false;
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("RecastEx")))
	{
		g_bRecastEx = true;
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("ShowGrid")))
	{
		g_bShowGrid = !g_bShowGrid; 
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("LoadGrid")))
	{
		LoadGrid();
		return TRUE;
	}
	else if (Parse(Cmd, TEXT("range "), dwValue))
	{
		g_nRange = dwValue;
		RefreshGrid();
		return TRUE;
	}
	else if (Parse(Cmd, TEXT("down "), dwValue))
	{
		g_nCenterZoneX -= dwValue;
		RefreshGrid();
		return TRUE;
	}
	else if (Parse(Cmd, TEXT("up "), dwValue))
	{
		g_nCenterZoneX += dwValue;
		RefreshGrid();
		return TRUE;
	}
	else if (Parse(Cmd, TEXT("right "), dwValue))
	{
		g_nCenterZoneY += dwValue;
		RefreshGrid();
		return TRUE;
	}
	else if (Parse(Cmd, TEXT("left "), dwValue))
	{
		g_nCenterZoneY -= dwValue;
		RefreshGrid();
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("down")))
	{
		g_nCenterZoneX--; 
		RefreshGrid();
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("up")))
	{
		g_nCenterZoneX++; 
		RefreshGrid();
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("right")))
	{
		g_nCenterZoneY++;
		RefreshGrid();
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("left")))
	{
		g_nCenterZoneY--;
		RefreshGrid();
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("BuildGrid")))
	{
#if WITH_EDITOR
		GWarn->MapCheck_Clear();
		FPathBuilder::BuildGrid();
		GWarn->MapCheck_ShowConditionally();
#endif
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("DebugPath")))
	{
		GEngine->Exec(TEXT("SHOW PATHS"));
		GEngine->Exec(TEXT("ShowDebugPath"));
		GEngine->Exec(TEXT("DebugShowNavPathResult 1"));
		extern bool debug_draw_navmesh_path;
		debug_draw_navmesh_path = true;
		return TRUE;
	}
	else if (ParseCommand(&Cmd, TEXT("BlockCheck")))
	{
		gClientBlockCheck = !gClientBlockCheck;
	}
#if WITH_EDITOR
	else if( ParseCommand(&Cmd, TEXT("Night"))  )
	{
		MMOWeatherManager::getSingleton().TestNight();
		return true;
	}
	else if( ParseCommand(&Cmd, TEXT("Day"))  )
	{
		MMOWeatherManager::getSingleton().TestDay();
		return true;
	}
	else if( ParseCommand(&Cmd, TEXT("ConvertToNewNPCTemplate"))  )
	{
		ConvertToNewNPCTemplate();
		return true;
	}
	else if( ParseCommand(&Cmd, TEXT("FixNPCTemplateRef"))  )
	{
		//FixNPCTemplateRef(); // No longer used
		return true;
	}
	else if( ParseCommand(&Cmd, TEXT("ExEquipApp"))  )
	{
		ExtractEquipAppearence();
		return true;
	}
	else if( ParseCommand(&Cmd, TEXT("findaddmat"))  )
	{
		GWarn->BeginSlowTask(TEXT("Finding additive lit material..."), TRUE);
		GWarn->MapCheck_Clear();

		TObjectIterator<UMaterial> matIt;
		for (; matIt; ++matIt)
		{
			UMaterial* Material = *matIt;
			if( Material && Material->LightingModel != MLM_Unlit && Material->BlendMode == BLEND_Additive)
			{
				GWarn->MapCheck_Add(MCTYPE_ERROR, Material, TEXT("Material with Additive blend mode is not Unlit!"), NULL);
			}
		}
		GWarn->EndSlowTask();
		GWarn->MapCheck_ShowConditionally();
		return true;
	}
	else if( ParseCommand(&Cmd, TEXT("fixmmovol"))  )
	{
		FixMMOVolume_Blocking();
		return true;
	}
	
#endif
	else if( ParseCommand(&Cmd, TEXT("MoveDebug"))  )
	{
		g_DebugMove = !g_DebugMove;
		debugf(TEXT("MoveDebug:%d SlopCheck:%d HeightCheck:%d"),
			(INT)g_DebugMove, (INT)g_EanbleSlopeCheck, (INT)g_EnableMoveStepHeightCheck);
		return true;
	}
	else if( ParseCommand(&Cmd, TEXT("MoveSlopeCheck"))  )
	{
		g_EanbleSlopeCheck = !g_EanbleSlopeCheck;
		debugf(TEXT("MoveDebug:%d SlopCheck:%d HeightCheck:%d"),
			(INT)g_DebugMove, (INT)g_EanbleSlopeCheck, (INT)g_EnableMoveStepHeightCheck);
		return true;
	}
	else if( ParseCommand(&Cmd, TEXT("MoveHeightCheck"))  )
	{
		g_EnableMoveStepHeightCheck = !g_EnableMoveStepHeightCheck;
		debugf(TEXT("MoveDebug:%d SlopCheck:%d HeightCheck:%d"),
			(INT)g_DebugMove, (INT)g_EanbleSlopeCheck, (INT)g_EnableMoveStepHeightCheck);
		return true;
	}
	else if( ParseCommand(&Cmd, TEXT("FixSUID"))  )
	{
		for(TObjectIterator<AActor> it; it; ++it)
		{
			AActor* serverActor = *it;

			if(serverActor->GetOutermost())
			{
				serverActor->GetOutermost()->MarkPackageDirty();
			}

			typedef TScriptInterface<IIUniqueID> IUniqueID;
			IUniqueID itf = ScriptInterfaceCast<IIUniqueID>(serverActor);
			if(itf)
			{
				itf->SetUniqueID(-1);
				itf->InitUniqueID();
			}
		}
		
		appMsgf(AMT_OK, TEXT("Server Unique ID fixed OK!"));

		return true;
	}
	else if(ParseCommand(&Cmd, TEXT("AddSoul")))
	{
		//add by zhengli
		//Used for debugging the spirit fusion system
				// spawn npctype npccnt
		/*
		if( ParseCommand(&Cmd, TEXT("?")) )
		{
			debugf( TEXT("GM Command: @spawn npctype npccnt") );
		}
		else
		{
			CMessageHandler::GetInstance().Send_GMCommand(Cmd);
		}
		*/
		int dc,ds,sc,ss,SocketIndex;
		Parse(Cmd,TEXT("DC="),dc);
		Parse(Cmd,TEXT("DS="),ds);
		Parse(Cmd,TEXT("SC="),sc);
		Parse(Cmd,TEXT("SS="),ss);
		Parse(Cmd,TEXT("SocketIndex="),SocketIndex);

		CMessageHandler::GetInstance().Send_AddSoul(dc,ds,SocketIndex,sc,ss);

		return TRUE;
	}
#if !SHIPPING_PC_GAME
	else if(ParseCommand(&Cmd, TEXT("TestDoodadEff")))
	{
		DoodadManager::getSingleton().TestDoodadEffect();
		return TRUE;
	}
	else if(ParseCommand(&Cmd, TEXT("FixModifier")))
	{
		extern void FixModifier();
		FixModifier();
		return TRUE;
	}
	else if(ParseCommand(&Cmd, TEXT("FixQuestItem")))
	{
		extern void FixQuestItem();
		FixQuestItem();
		return TRUE;
	}
	else if(ParseCommand(&Cmd, TEXT("FixNPCAIData")))
	{
		FixNPCAIData();
		return TRUE;
	}
	else if(ParseCommand(&Cmd, TEXT("MoveSceneMat")))
	{
		MoveSceneMaterial();
		return TRUE;
	}
#endif


#endif // #if !SHIPPING_PC_GAME

	return FALSE;
}

#if !SHIPPING_PC_GAME

//------------------------------------------------------------------------
// Export Recast dtMesh Data from callback
//------------------------------------------------------------------------
#include "RecastNavMesh.h"
APylon* GCurrentNavMeshPylon = 0;
bool GEnableExportServerNavMeshExport = false;
#include "editor_scene.h"
extern CEditorScene g_Scene;
extern BOOL RefreshGrid(void);

void GenerateNavMeshCallBack(unsigned char* data, const int dataSize)
{
	int nRetCode = 0;

	if (!g_Scene.IsModified())
		return;

	if(!GEnableExportServerNavMeshExport)
	{
		return;
	}

	nRetCode = g_Scene.ProcessNavData(data, dataSize);
	if (!nRetCode)
	{
		GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Failed to process nav data")), NULL);
	}

	FString ExportDir;
	GConfig->GetString(TEXT("Configuration"), TEXT("ServerSceneDataExport"), ExportDir, GEditorIni);
	//ExportDir += GWorld->GetWorldInfo()->GetMapName() + FString(TEXT("\\"));
	::_wmkdir(*ExportDir);
	
    //For now assume a map has only one dtNavMesh;
    FString fileName = ExportDir + GWorld->GetWorldInfo()->GetMapName() + TEXT("\\") + GWorld->GetWorldInfo()->GetMapName() + TEXT(".nav");

	//FString fileName = ExportDir + GWorld->GetWorldInfo()->GetMapName() + TEXT("_")
	//	+ GCurrentNavMeshPylon->GetName() + TEXT(".dtNavMesh");

	FCheckoutResult sccResult = FSourceControl::TryCheckOut(fileName);

	FArchive* ar = GFileManager->CreateFileWriter(*fileName);
	if(ar)
	{
		ar->ByteOrderSerialize((void*)data, dataSize);
		ar->Flush();
		ar->Close();
	}

	RefreshGrid();

	nRetCode = g_Scene.Save();
	if (!nRetCode)
	{
		GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Failed to save grid data of scene")), NULL);
	}

	sccResult.TryAddToDefaultChangelist();
}

bool BindCallbackForNavMeshGenerate()
{
	GGenerateNavMeshCallBack = GenerateNavMeshCallBack;
	return true;
}

bool gBindCallbackForNavMeshGenerate = BindCallbackForNavMeshGenerate();

#else
void GenerateNavMeshCallBack(unsigned char* data, const int dataSize)
{}
#endif // #if !SHIPPING_PC_GAME


#if WITH_EDITOR
void PostEditorCreateActor(AActor* actor)
{
	if(!actor)
	{
		return;
	}

	typedef TScriptInterface<IIUniqueID> IUniqueID;
	IUniqueID itf = ScriptInterfaceCast<IIUniqueID>(actor);
	if(itf)
	{
		itf->SetUniqueID(-1);
		itf->InitUniqueID();
	}
}

#include "BSPOps.h"

void FixMMOVolume_Blocking()
{
	TArray<AActor*> oldVolums;
	TObjectIterator<AMMOVolume_Blocking> blkIt;
	for (; blkIt; ++blkIt)
	{
		AMMOVolume_Blocking* blkVol = *blkIt;
		oldVolums.AddItem(blkVol);
		if(blkVol->Brush)
		{
// 			blkVol->PreEditChange(NULL);
// 			blkVol->Modify();
// 
// 			FBSPOps::csgPrepMovingBrush(blkVol);
// 
// 			blkVol->PostEditChange();
		}
	}

// 	for (INT i = 0; i < oldVolums.Num(); ++i)
// 	{
// 		AMMOVolume_Blocking* blkVol = (AMMOVolume_Blocking*)oldVolums(i);
// 
// 		FBox GetActor1 = blkVol->GetComponentsBoundingBox( true );
// 
// 		float GetMaxValue_X = GetActor1.Max.X;
// 		float GetMinValue_X = GetActor1.Min.X;
// 		float GetMaxValue_Y = GetActor1.Max.Y;
// 		float GetMinValue_Y = GetActor1.Min.Y;
// 		float GetMaxValue_Z = GetActor1.Max.Z;
// 		float GetMinValue_Z = GetActor1.Min.Z;
// 
// 		FBox GetActor = blkVol->GetComponentsBoundingBox( true );
// 
// 		float ActorOneMaxX = GetActor.Max.X;
// 		if ( ActorOneMaxX > GetMaxValue_X )
// 			GetMaxValue_X = ActorOneMaxX;
// 
// 		float ActorOneMinX = GetActor.Min.X;
// 		if ( ActorOneMinX < GetMinValue_X )
// 			GetMinValue_X = ActorOneMinX;
// 
// 		float ActorOneMaxY = GetActor.Max.Y;
// 		if ( ActorOneMaxY > GetMaxValue_Y )
// 			GetMaxValue_Y = ActorOneMaxY;
// 
// 		float ActorOneMinY = GetActor.Min.Y;
// 		if ( ActorOneMinY < GetMinValue_Y )
// 			GetMinValue_Y = ActorOneMinY;
// 
// 		float ActorOneMaxZ = GetActor.Max.Z;
// 		if ( ActorOneMaxZ > GetMaxValue_Z )
// 			GetMaxValue_Z = ActorOneMaxZ;
// 
// 		float ActorOneMinZ = GetActor.Min.Z;
// 		if ( ActorOneMinZ < GetMinValue_Z )
// 			GetMinValue_Z = ActorOneMinZ;	
// 
// 		FBox GetAllCenter = FBox( FVector(GetMinValue_X, GetMinValue_Y, GetMinValue_Z), FVector(GetMaxValue_X, GetMaxValue_Y, GetMaxValue_Z) );
// 
// 		UCubeBuilder* CubeBuilder = ConstructObject<UCubeBuilder>( UCubeBuilder::StaticClass() );
// 		CubeBuilder->X = GetMaxValue_X - GetMinValue_X; 
// 		CubeBuilder->Y = GetMaxValue_Y - GetMinValue_Y;
// 		CubeBuilder->Z = GetMaxValue_Z - GetMinValue_Z;
// 		CubeBuilder->eventBuild();
// 
// 		GWorld->CurrentLevel = blkVol->GetLevel();
// 
// 		AMMOVolume_Blocking* 	LastCreatedVolume = (AMMOVolume_Blocking*)GWorld->SpawnActor(AMMOVolume_Blocking::StaticClass(),NAME_None,GWorld->GetBrush()->Location);
// 		if( LastCreatedVolume )
// 		{
// 			LastCreatedVolume->PreEditChange(NULL);
// 
// 			FBSPOps::csgCopyBrush
// 				(
// 				LastCreatedVolume,
// 				GWorld->GetBrush(),
// 				0,
// 				RF_Transactional,
// 				1,
// 				TRUE
// 				);
// 
// 			LastCreatedVolume->Modify();
// 			// 				LastCreatedVolume->VolumeType = EVVT_Surface;
// 			// 				LastCreatedVolume->ServerUniqueID = -1;
// 			// 				LastCreatedVolume->TerrainSurfaceType = ETST_Normal;
// 			// 				LastCreatedVolume->CollisionType = COLLIDE_BlockAllButWeapons;
// 			// 				LastCreatedVolume->bBlockActors = TRUE;
// 			// 				LastCreatedVolume->BlockRigidBody = TRUE;
// 			LastCreatedVolume->SetLocation( GetAllCenter.GetCenter() );
// 			LastCreatedVolume->PostEditChange();
// 		}
// 
// 		UCubeBuilder* CubeBuilderOriginal = ConstructObject<UCubeBuilder>( UCubeBuilder::StaticClass() );
// 		CubeBuilderOriginal->X = CubeBuilderOriginal->Y = CubeBuilderOriginal->Z = 5;
// 		CubeBuilderOriginal->eventBuild();
// 
// // 		AMMOVolume_Blocking* 	LastCreatedVolume = (AMMOVolume_Blocking*)GWorld->SpawnActor(
// // 			AMMOVolume_Blocking::StaticClass(),NAME_None, blkVol->Location, blkVol->Rotation, blkVol);
// 	}

	extern void DeleteOldVolums(TArray<AActor*>& actors);
	DeleteOldVolums(oldVolums);
}

void FixNPCAIData()
{
	TObjectIterator<UNPCTemplate> blkIt;
	for (; blkIt; ++blkIt)
	{
		UNPCTemplate* npc = *blkIt;
		npc->AIData = npc->AIDataID;
		npc->MarkPackageDirty();
	}
}

bool WriteResListFile(const FString& fileName, const TSet<INT>& resList)
{
	FArchive* ar = GFileManager->CreateFileWriter(*fileName);
	if(ar)
	{
		(*ar) << (TSet<INT>&)resList;
		ar->Flush();
		ar->Close();
		delete ar;
		return true;
	}
	else
	{
		warnf(NAME_Error, TEXT("WriteResListFile failed to open file:%s"), *fileName);
		return false;
	}
}
#endif


FString GetCurrentNPCPreLoadingListFile()
{
	FString globalMapInfoDir;
	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"), globalMapInfoDir, GGameIni);
	globalMapInfoDir = MMO::FormatTempleteDataLoadingDir(globalMapInfoDir);
	FString fileName = globalMapInfoDir + GWorld->GetWorldInfo()->GetMapName() + TEXT(".preloading");
	return fileName;
}

void GenNpcPreloadResList()
{
	TSet<INT> npcList;
	TSet<INT> otherList;
	for(TObjectIterator<UObject> It; It; ++It)
	{
		ANPCServerInstance* npcInstance = Cast<ANPCServerInstance>(*It);
		ANPCNewSpawnPoint* npcNewSpawnPoint = Cast<ANPCNewSpawnPoint>(*It);
		if(npcInstance)
		{
			if(npcInstance->TemplateNew)
			{
				npcList.Add(npcInstance->TemplateNew->TemplateID);
			}
		}

		if(npcNewSpawnPoint)
		{
			for(INT i = 0; i < npcNewSpawnPoint->NPCTemplateList.Num(); ++i)
			{
				if(npcNewSpawnPoint->NPCTemplateList(i).NPCTemplateNew)
				{

					//FString resName = appItoa(npcNewSpawnPoint->NPCTemplateList(i).TemplateID);
					npcList.Add(npcNewSpawnPoint->NPCTemplateList(i).NPCTemplateNew->TemplateID);
				}
			}
		}
	}

	FString fileName = GetCurrentNPCPreLoadingListFile();
#if HAVE_SCC
	FCheckoutResult sccResult = FSourceControl::TryCheckOut(fileName);
#endif

	FArchive* ar = GFileManager->CreateFileWriter(*fileName);
	if(ar)
	{
		(*ar) << (TSet<INT>&)npcList;
		ar->Flush();
		ar->Close();
		delete ar;
	}
	else
	{
		warnf(NAME_Error, TEXT("GenNpcPreloadResList failed to open file:%s"), *fileName);
	}
#if HAVE_SCC
	sccResult.TryAddToDefaultChangelist();
#endif
}