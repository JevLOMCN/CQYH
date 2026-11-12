#include "stdafx.h"
#include "MMOCommandlet.h"
#include "MMOFacade.h"
#include "MMOItemDataManager.h"
#include "DoodadManager.h"
#include "MMODropManager.h"
#include "MMOQuestData.h"
#include "SourceControl.h"
#include "EngineSoundClasses.h"

IMPLEMENT_CLASS(UMakeCookMapListCommandlet)
IMPLEMENT_CLASS(UMakeDataTemplateListCommandlet)
IMPLEMENT_CLASS(UMakeParseStringCommandlet)
IMPLEMENT_CLASS(UExportAllObjectCommandlet)
IMPLEMENT_CLASS(UFindObjectsReferencersCommandlet)
// add by zhangrui05@snda
IMPLEMENT_CLASS(UMakeQuestDoodadDropDataCommandlet)

void AddAutoMapList(TArray<FString>& cookPackages)
{
	for(INT i = 0; i < UGlobalMapInfo::GetSingleton()->ExportMaps.Num(); ++i)
	{
		FString mapName = UGlobalMapInfo::GetSingleton()->ExportMaps(i).MapName + TEXT(".udk");
		warnf(NAME_Log, TEXT("Adding auto map list:%s"), *mapName);
		cookPackages.AddUniqueItem(mapName);
	}
}

INT GetSession( const FString text , const FString preSign , const FString postSign , FString& outKey );
FString NewLine( FString text , INT startPos );
FString GetLine( FString& text );
void GetKeyValue( const FString text , FString& outKey , FString& outValue );



std::map< std::wstring , LanguageType > g_LangMap;

struct LangText
{
	FString lang[LT_Num];
};
typedef std::map< std::wstring , LangText > LangStringMap;
LangStringMap g_LangString;

void ConvertConfig( const FString text , FString fileName , LanguageType langType )
{

	FString keyString , valueString , Session;	
	FString textCopy = text;
	FString outText;
	
	while( textCopy.Len() > 0 )
	{
		FString line = GetLine( textCopy );
		//skip empty lines
		if( line.Len() == 0 )
			continue;
		//skip comment lines
		if( line.Left(1) == TEXT(";") )
			continue;
		//determine whether it is session or key
		INT nextSession = line.InStr( TEXT("[") );
		INT nextValue = line.InStr( TEXT("=") );
		if( nextSession < 0 && nextValue < 0 )
		{
			continue;
		}
		else if( nextSession >= 0 && nextValue < 0 )
		{
			GetSession( line , TEXT("[") , TEXT("]") , Session );
			continue;
		}
		else if( nextSession < 0 && nextValue >= 0 )
		{
			GetKeyValue( line , keyString , valueString );
		}
		else if( nextSession < nextValue )
		{
			GetSession( line , TEXT("[") , TEXT("]") , Session );
			continue;
		}
		else
		{
			GetKeyValue( line , keyString , valueString );
		}

		if( !Session.IsEmpty() )
		{
			FString fullName = fileName + TEXT(".") + Session + TEXT(".") + keyString;
			LangStringMap::iterator itor = g_LangString.find( *fullName );
			if( itor != g_LangString.end() )
			{
				itor->second.lang[langType] = valueString;
			}
			else
			{
				LangText langText;
				langText.lang[langType] = valueString;
				g_LangString.insert( std::make_pair( *fullName , langText ) );
			}
		}
	}
	
}

FString GetLine( FString& text )
{
	INT pos = text.InStr( TEXT("\r\n") , FALSE , FALSE );
	FString outLine;
	if( pos >= 0 )
	{
		outLine = text.Left(pos);
		text = text.RightChop( pos + appStrlen(TEXT("\r\n")) );
	}
	else
	{
		outLine = text;
		text = TEXT("");
	}
	return outLine;
	
}
//rightChop cuts off the left and takes the right... what a pain

FString NewLine( FString text , INT startPos )
{
	INT pos = text.InStr( TEXT("\r\n") , FALSE , FALSE , startPos ) + appStrlen( TEXT("\r\n") );
	FString newLine = text.RightChop(pos);
	return newLine;
}

void GetKeyValue( const FString text , FString& outKey , FString& outValue )
{
	INT pos = text.InStr( TEXT("=") , FALSE , FALSE );
	outKey = text.Left(pos);
	outValue = text.RightChop( pos + appStrlen("=") );
}

INT GetSession( const FString text , const FString preSign , const FString postSign , FString& outSession )
{
	INT startPos = text.InStr( preSign ) + preSign.Len() ;
	INT endPos = text.InStr( postSign );
	if( startPos < 0 || endPos < 0 )
	{
		outSession = TEXT("");
	}
	else
	{
		outSession = text.Mid( startPos , endPos-startPos );
	}
	
	return endPos;
}

INT UMakeParseStringCommandlet::Main(const FString& Params)
{
#if !SHIPPING_PC_GAME
	
	//langMap.insert( std::make_pair( TEXT("CHN") , LT_CHN ) );
	//langMap.insert( std::make_pair( TEXT("INT") , LT_INT ) );

	//for( INT PathIndex = GSys->LocalizationPaths.Num()-1 ; PathIndex >= 0 ; PathIndex-- )
	{
		
		FFilename FilenameLang	= *GSys->LocalizationPaths(1);  // "..\..\MMOGame\Localization"

		TArray<FString> filesNames;
		appFindFilesInDirectory( filesNames , *FilenameLang , TRUE , TRUE );
		
		FString text;
		for ( INT fileIndex = 0 ; fileIndex < filesNames.Num() ; fileIndex++ )
		{
			FString fullName = filesNames(fileIndex);
			INT slashPos = fullName.InStr( TEXT("\\") , TRUE ) + 1;
			FString pureFileName = fullName.RightChop(slashPos);
			FString lang = pureFileName.Right(3);
			pureFileName = pureFileName.LeftChop(4);

			lang = lang.ToUpper();
			//std::map< std::wstring , LanguageType >::iterator itor = langMap.find( *langString );
			
			INT langType = MMOUI::GetStringType( lang );
				
			if( langType < 0 )
			{
				continue;
			}

			appLoadFileToString( text , *fullName );
			
			FString keyWord;
			keyWord.Empty();

			ConvertConfig( text , pureFileName , (LanguageType)langType );
		}
		//appLoadFileToString( text , *FilenameLang );
	}

	FString printText;

	for( LangStringMap::iterator it = g_LangString.begin() ; it != g_LangString.end() ; it++ )
	{
		printText += FString(it->first.c_str()) + TEXT("\t") + it->second.lang[LT_INT] + TEXT("\t") + it->second.lang[LT_CHN] + TEXT("\r\n");
	}
	FString writeFileName = appGameConfigDir() + TEXT("StringMap.txt");
	FArchive* ar = GFileManager->CreateFileWriter( *writeFileName );
	std::string str( TCHAR_TO_ANSI(*printText) );
	
	
	ar->Serialize( (void*) str.c_str() , str.length()*sizeof(char) );
	ar->Flush();
	ar->Close();
	delete ar;

#endif
	return 0;
}


extern UBOOL GIsExportingResources;
INT UExportAllObjectCommandlet::Main(const FString& Params)
{
	//UObject* ToTest = LoadObject<UObject>( NULL, TEXT("UI_Texture.Img_TipsBG"), NULL, LOAD_None, NULL );
	
	GIsExportingResources = TRUE;


	TArray<FString> Tokens, Switches;

	ParseCommandLine(*Params, Tokens, Switches);

	for (INT i = 0; i < Tokens.Num(); ++i)
	{
		ConvertFileRootFolders.AddUniqueItem(Tokens(i));
	}

	FString ExportString(TEXT("_SBExp"));
	this->SetFlags(RF_Marked);

	TArray<FString> NativePackageName;
	NativePackageName.AddItem(TEXT("Core"));
	NativePackageName.AddItem(TEXT("Engine"));
	NativePackageName.AddItem(TEXT("GameFramework"));
	NativePackageName.AddItem(TEXT("GFxUI"));
	NativePackageName.AddItem(TEXT("GFxUIEditor"));
	NativePackageName.AddItem(TEXT("IpDrv"));
	NativePackageName.AddItem(TEXT("MMOGame"));
	NativePackageName.AddItem(TEXT("OnlineSubsystemGameSpy"));
	NativePackageName.AddItem(TEXT("OnlineSubsystemLive"));
	NativePackageName.AddItem(TEXT("OnlineSubsystemPC"));
	NativePackageName.AddItem(TEXT("OnlineSubsystemSteamworks"));
	NativePackageName.AddItem(TEXT("UnrealEd"));
	NativePackageName.AddItem(TEXT("WinDrv"));

	TArray<FString> CanNotExportPackageName = NativePackageName;
	for (INT i = 0; i < NativePackageName.Num(); ++i)
	{
		UPackage* ToSearchPackage = UObject::FindPackage(NULL, *NativePackageName(i));
		if (!ToSearchPackage)
		{
			continue;
		}

		ULinkerLoad* linker = ULinkerLoad::FindExistingLinkerForPackage(ToSearchPackage);
		check(linker);
		TArray<FObjectImport>& ImportMap = linker->GetImportMap();
		for (INT j = 0; j < ImportMap.Num(); ++j)
		{
			if (ImportMap(j).OuterIndex == 0 && ImportMap(j).ClassName == FName(TEXT("Package")))
			{
				CanNotExportPackageName.AddUniqueItem(ImportMap(j).ObjectName.ToString());
			}
		}
	}


	TArray<UClass*> CanExportType;
	CanExportType.AddItem(UTexture::StaticClass());
	CanExportType.AddItem(UStaticMesh::StaticClass());
	CanExportType.AddItem(USkeletalMesh::StaticClass());
	CanExportType.AddItem(UParticleSystem::StaticClass());
	CanExportType.AddItem(UAnimSet::StaticClass());
	CanExportType.AddItem(USoundNodeWave::StaticClass());
	//CanExportType.AddItem(UPhysicsAsset::StaticClass());
	CanExportType.AddItem(UFont::StaticClass());


	for (INT i = 0; i < ConvertFileRootFolders.Num(); ++i)
	{
		TArray<FString> Results;
		appFindFilesInDirectory(Results, *ConvertFileRootFolders(i), TRUE, FALSE);
		//Results.Empty();
		//Results.AddItem(FString(TEXT("F:\\WorkCode\\08.Prog_MPQ\\Client\\UE3\\MMOGame\\Content\\MMOContent\\08.UI\\UI_Texture.upk")));
		for (INT j = 0; j < Results.Num(); ++j)
		{
			//if (Results(j).InStr(ExportString, TRUE, TRUE) != INDEX_NONE)
			//{
			//	continue;
			//}

			GPackageFileCache->CachePackage(*Results(j), TRUE, FALSE);

			FString PackageName = FPackageFileCache::PackageFromPath(*Results(j));
			INT Index = 0;
			if (CanNotExportPackageName.FindItem(PackageName, Index))
			{
				continue;
			}

			UPackage* ToProcessPackage = UObject::LoadPackage(NULL, *PackageName, LOAD_None);
			if (ToProcessPackage == NULL)
			{
				continue;
			}
			ToProcessPackage->FullyLoad();

			if (ToProcessPackage->HasAnyFlags(PKG_ExportedResourcePackage))
			{
				continue;
			}

			SET_WARN_COLOR(COLOR_GREEN);
			warnf(NAME_Log, TEXT("Processing package: [%d/%d] %s"), j, Results.Num(), *PackageName);
			CLEAR_WARN_COLOR();

			INT ProcessedCount = 0;

			for( FObjectIterator It; It; ++It )
			{
				UObject* ToProcessObject = *It;
				if (ToProcessObject == NULL)
				{
					continue;
				}

				if (ToProcessObject->IsIn(ToProcessPackage) == FALSE)
				{
					continue;
				}

				if (ToProcessObject->HasAnyFlags(RF_Public) == FALSE)
				{
					continue;
				}

				if (ToProcessObject->HasAnyFlags(RF_Transient))
				{
					continue;
				}

				if (ToProcessObject->IsA(UPackage::StaticClass()))
				{
					continue;
				}

				if (ToProcessObject->IsA(UObjectRedirector::StaticClass()))
				{
					continue;
				}

				UBOOL CanExport = FALSE;
				for (INT l = 0; l < CanExportType.Num(); ++l)
				{
					if (ToProcessObject->IsA(CanExportType(l)))
					{
						CanExport = TRUE;
						break;
					}
				}
				if (!CanExport)
				{
					continue;
				}

				FString NewPackageName = FString::Printf(TEXT("%s_%s"), *ToProcessObject->GetPathName(), *ToProcessObject->GetClass()->GetName());
				NewPackageName = NewPackageName.Replace(TEXT("."), TEXT("__"));

				UPackage* NewPackage = UObject::CreatePackage( NULL, *NewPackageName );

				if (NewPackage == NULL)
				{
					warnf(TEXT("Create New Package(%s) Failed"), *NewPackageName);
					continue;
				}

				debugf(TEXT("Object:%s To Export"), *ToProcessObject->GetFullName());
				if (ToProcessObject->Rename(*(ToProcessObject->GetName()), NewPackage, REN_Test))
				{
					ToProcessObject->Rename(*(ToProcessObject->GetName()), NewPackage, REN_None);
				}
				else
				{
					warnf(TEXT("Rename error:%s -> %s"), *ToProcessPackage->GetPathName(), *NewPackage->GetName());
					continue;
				}
				

				ToProcessObject->SetFlags(RF_Public | RF_Standalone);
				ToProcessObject->MarkPackageDirty();

				NewPackage->FullyLoad();

				FString OutPath, OutFileName, OutExtension;
				FPackageFileCache::SplitPath(*Results(j), OutPath, OutFileName, OutExtension);

				FString ToSaveFileName = FString::Printf(TEXT("%s\\%s.%s"), *OutPath, *NewPackageName, *OutExtension);

				UObject::SavePackage( NewPackage, NULL, RF_Standalone, *ToSaveFileName, GWarn );

				ProcessedCount++;
			}

			if (ProcessedCount > 0)
			{
#if HAVE_SCC
				FCheckoutResult sccResult = FSourceControl::TryCheckOut(Results(j));
				if (sccResult.AddToSCC == false)
				{
					continue;
				}
#endif
				ToProcessPackage->MarkPackageDirty(TRUE);
				//ToProcessPackage->SetFlags(PKG_Cooked);
				UObject::SavePackage(ToProcessPackage, NULL, RF_Standalone, *Results(j), GWarn);

				debugf(TEXT("Package:%s Exported OK"), *Results(j));
			}


			CollectGarbage(RF_Native | RF_Marked, TRUE);
		}

	}

	this->ClearFlags(RF_Marked);


	GIsExportingResources = FALSE;

	return 0;
}

INT UFindObjectsReferencersCommandlet::Main(const FString& Params)
{
	TArray<FString> ToFindObjectList;

	TArray<FString> Tokens, Switches;

	ParseCommandLine(*Params, Tokens, Switches);

	for (INT i = 0; i < Tokens.Num(); ++i)
	{
		ToFindObjectList.AddUniqueItem(Tokens(i));
	}

	struct Node
	{
		FString ObjectName;
		TArray<FString> ReferencedByThisList;
		TArray<FString> ThisReferenceList;   
	};

	std::map<FString, Node> AllObjects;

	class FArchiveAnalyzeReferences : public FArchive
	{
		UObject* BaseObject;
		std::map<FString, Node>* OutAllObjects;

		TArray<UObject*> SearchedObjectList;

		FArchive& operator<<( UObject*& Obj )
		{
			//if (!Obj)
			//{
			//	return *this;
			//}

			//if (SearchedObjectList.FindItemIndex(Obj) != INDEX_NONE)
			//{
			//	return *this;
			//}

			//SearchedObjectList.AddItem(Obj);

			//UObject* OuterMost = BaseObject->GetOutermost();
			//if (TypeList.FindItemIndex(*Obj->GetClass()->GetName()) == INDEX_NONE)
			//{
			//	if (Obj->IsIn(OuterMost))
			//	{
			//		debugf(L"To Serialize:%s", *Obj->GetFullName());
			//		Obj->Serialize(*this);
			//	}
			//}
			//else
			//{
			//	debugf(L"Got a res : %s", *Obj->GetFullName());
			//	OutObjects.AddItem(Obj);
			//}
			return *this;
		}
	public:

		void GenerateReference(UObject* InObject, std::map<FString, Node>* InMap)
		{
			check(InObject);
			BaseObject = InObject;
			BaseObject->Serialize(*this);
			OutAllObjects = InMap;
		}
	};


	FString FilePaths(TEXT("..\\..\\MMOGame\\Content\\"));
		TArray<FString> Results;
		appFindFilesInDirectory(Results, *FilePaths, TRUE, FALSE);
		//Results.Empty();
		//Results.AddItem(FString(TEXT("F:\\WorkCode\\08.Prog_Stable\\Client\\UE3\\MMOGame\\Content\\MMOContent\\08.UI\\UI_Texture.upk")));
		for (INT j = 0; j < Results.Num(); ++j)
		{
			FString PackageName = FPackageFileCache::PackageFromPath(*Results(j));
			INT Index = 0;

			UPackage* ToProcessPackage = UObject::LoadPackage(NULL, *PackageName, LOAD_None);
			if (ToProcessPackage == NULL)
			{
				continue;
			}
			ToProcessPackage->FullyLoad();

			FArchiveAnalyzeReferences Finder;
			Finder.GenerateReference(ToProcessPackage, &AllObjects);
		}







	return 0;
}

INT UMakeCookMapListCommandlet::Main( const FString& Params )
{
#if !SHIPPING_PC_GAME

	FArchive* ar = GFileManager->CreateFileWriter(TEXT("mapcooklist.txt"));
	if(!ar)
	{
		return -1;
	}

	typedef TSet<FString> FStringSet;
	FStringSet mapList;

	for(INT i = 0; i < UGlobalMapInfo::GetSingleton()->ExportMaps.Num(); ++i)
	{
		FString mapName = UGlobalMapInfo::GetSingleton()->ExportMaps(i).MapName + TEXT(".udk\r\n");
		mapList.Add(mapName);
	}

	FString cfgMap;
	GConfig->GetString(TEXT("URL"), TEXT("Map"), cfgMap, GEngineIni);
	cfgMap += TEXT("\r\n");
	mapList.Add(cfgMap);

	GConfig->GetString(TEXT("URL"), TEXT("LocalMap"), cfgMap, GEngineIni);
	cfgMap += TEXT("\r\n");
	mapList.Add(cfgMap);

	GConfig->GetString(TEXT("URL"), TEXT("TransitionMap"), cfgMap, GEngineIni);
	cfgMap += TEXT("\r\n");
	mapList.Add(cfgMap);
	
	FStringSet::TIterator it(mapList);
	for (; it; ++it)
	{
		FString mapName = *it;
		debugf(TEXT("@@@@@@@ CookMapList: %s"), *mapName);
		ar->Serialize((void*)*mapName, mapName.Len() * sizeof(TCHAR));
	}
	ar->Flush();
	ar->Close();
	delete ar;

#endif	

	return 0;
}

#if !SHIPPING_PC_GAME
void MakeDataTemplateList(UClass* requiredType, const TArray<FString>& dirs, const FString& saveToFileName)
{
	struct FDataTemplate
	{
		INT TemplateID;
		FString ObjectPath;
	};

	TArray<FDataTemplate> dataTemplateList;

	for (INT k = 0; k < dirs.Num(); ++k)
	{
		const FString& cfgDir = dirs(k);

		warnf(NAME_Log, TEXT("~~~~~~ Loading template data form %s"), *cfgDir);

		TArray<FString> pkgs;
		appFindFilesInDirectory(pkgs, *cfgDir, TRUE, FALSE);
		for (INT i = 0; i < pkgs.Num(); ++i)
		{
			UPackage* Package = UObject::LoadPackage( NULL, *pkgs(i), LOAD_Verify );
			if( !Package )
			{
				warnf( NAME_Error, TEXT("~~~~~~ [MakeDataTemplateList] Unable to load %s"), *pkgs(i) );
				continue;;
			}

			if(!Package->IsFullyLoaded())
			{
				Package->FullyLoad();
			}

			for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
			{
				UObject* obj = Package->GetNetObjectAtIndex(j);
				if(obj->IsA(requiredType))
				{
					TScriptInterface<IIReferenceID> refIDObj =
						ScriptInterfaceCast<IIReferenceID>(obj);

					if(refIDObj)
					{
						FDataTemplate dat;
						dat.TemplateID = refIDObj->GetTemplateID();
						dat.ObjectPath = obj->GetPathName();
						dataTemplateList.AddItem(dat);
					}
					else
					{
						warnf(NAME_Error, TEXT("~~~~~~ [%s] can't convert to IReferenceID"),
							*obj->GetFullName());
					}
				}
				else
				{
					//warnf(NAME_Warning, TEXT("~~~~~~ [%s] is not required type:%s"),
					//	*obj->GetFullName(), *requiredType->GetName());
				}
			}
		}
	}

	FString outFile = "..\\..\\MMOGame\\Config\\";
	outFile += saveToFileName;
	outFile = GFileManager->ConvertToAbsolutePath(*outFile);
	warnf(NAME_Log, TEXT("~~~~~~ Saving %d obj to file:%s"), dataTemplateList.Num(), *outFile);
	
	FArchive* ar = GFileManager->CreateFileWriter(*outFile);
	
	UNICHAR BOM = UNICODE_BOM;
	ar->Serialize( &BOM, sizeof(BOM) );

	//FString section(TEXT("[data]"));
	//ar << section;
	//ar->Serialize((void*)*section, section.Len() * sizeof(TCHAR)))

	for (int i = 0; i < dataTemplateList.Num(); ++i)
	{
		const FDataTemplate& dat = dataTemplateList(i);
		FString tmp = appItoa(dat.TemplateID) + TEXT("=") + dat.ObjectPath + TEXT("\r\n");
		ar->Serialize(const_cast<TCHAR*>(*tmp), sizeof(TCHAR) * tmp.Len());
		//debugf(TEXT("~~~~~~ %s"), *tmp);
	}

	ar->Flush();
	ar->Close();
	delete ar;
}
#endif

INT UMakeDataTemplateListCommandlet::Main( const FString& Params )
{
#if !SHIPPING_PC_GAME

	// NPC Template
	{
		TArray<FString> dirArray;
		GConfig->GetArray( TEXT("NPCTemplates"), TEXT("TemplateDir"), dirArray, GGameIni );
		MMO::FormatTempleteDataLoadingDir(dirArray);
		MakeDataTemplateList(UNPCTemplate::StaticClass(), dirArray, TEXT("Templates_NPC.txt"));
	}

	// Item Template
	{
		TArray<FString> dirArray;
		GConfig->GetArray( TEXT("ItemsTemplates"), TEXT("TemplateDir"), dirArray, GGameIni );
		MMO::FormatTempleteDataLoadingDir(dirArray);
		MakeDataTemplateList(UItem::StaticClass(), dirArray, TEXT("Templates_Items.txt"));
	}

	// Doodad Template
	{
		TArray<FString> dirArray;
		GConfig->GetArray( TEXT("DoodadTemplates"), TEXT("TemplateDir"), dirArray, GGameIni );
		MMO::FormatTempleteDataLoadingDir(dirArray);
		MakeDataTemplateList(UDoodad::StaticClass(), dirArray, TEXT("Templates_Doodad.txt"));
	}

	// DoodadDrop Template
// 	{
// 		TArray<FString> dirArray;
// 		GConfig->GetArray( TEXT("DoodadDropTemplates"), TEXT("TemplateDir"), dirArray, GGameIni );
// 		MMO::FormatTempleteDataLoadingDir(dirArray);
// 		MakeDataTemplateList(UDoodad_Drop::StaticClass(), dirArray, TEXT("Templates_DoodadDrop.txt"));
// 	}

	// Quest Template
	{
		TArray<FString> dirArray;
		GConfig->GetArray( TEXT("QuestTemplates"), TEXT("TemplateDir"), dirArray, GGameIni );
		MMO::FormatTempleteDataLoadingDir(dirArray);
		MakeDataTemplateList(UQuest::StaticClass(), dirArray, TEXT("Templates_Quest.txt"));
	}

	// Ability Template
	{
		TArray<FString> dirArray;
		GConfig->GetArray( TEXT("AbilityTemplates"), TEXT("TemplateDir"), dirArray, GGameIni );
		MMO::FormatTempleteDataLoadingDir(dirArray);
		MakeDataTemplateList(UMMOAbility::StaticClass(), dirArray, TEXT("Templates_Ability.txt"));
	}

#endif	

	return 0;
}

// add by zhangrui05@snda
INT UMakeQuestDoodadDropDataCommandlet::Main(const FString& Params)
{
#if !SHIPPING_PC_GAME
	// dependency data
	MMOItemDataManager::getSingleton().InitFromFile(TEXT("item.txt"), true);
	MMOEquipDataManager::getSingleton().InitFromFile(TEXT("item_armor.txt"), true);

	DoodadManager::getSingleton().LoadDoodadData();	
	MMODropGroupDataManager::getSingleton().InitFromFile(TEXT("drop_group.txt"), true);
	MMODoodadDropDataManager::getSingleton().InitFromFile(TEXT("doodad_drop_data.txt"), true);
	MMOQuestData::getSingleton().LoadQuestDataFromUpk();

	// generated data
	MMOQuestData::getSingleton().GenerateQuestDoodadDropData(TEXT("quest_doodad.txt"));
#endif
	return 0;
}