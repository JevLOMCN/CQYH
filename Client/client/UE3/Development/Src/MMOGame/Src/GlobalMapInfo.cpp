#include "stdafx.h"
//#include "MMOGameClasses.h"
#include "MMOUtil.h"
#include "XML.h"
#include "UnLinkedObjDrawUtils.h"
#include "FileHelpers.h"
#include "teleport_def.h"
#include "MMOFacade.h"
#include "MMODataManager.h"

#if WITH_EDITOR
#include "MServerMapCfgExport.h"
#include "SourceControl.h"
#endif

#include "MMONavigationManager.h"

IMPLEMENT_CLASS(UGlobalMapInfo)
//IMPLEMENT_CLASS(UMapTeleports)
//IMPLEMENT_CLASS(UTeleportObject)
//IMPLEMENT_CLASS(UTeleportLink)

//UGlobalMapInfo* GGlobalMapInfo = 0;
UGlobalMapInfo* UGlobalMapInfo::GetSingleton()
{
	//UPackage* Package = GetGlobalMapInfoPackage();
	static UGlobalMapInfo* GlobalMapInfoSingleton = 0;

	if( GlobalMapInfoSingleton != 0 && GlobalMapInfoSingleton->IsValid() ){
		return GlobalMapInfoSingleton;
	}

#if 1
	FString cnfgName;
	if(GConfig->GetString(TEXT("SingletonData"), TEXT("GlobalMapInfo"), cnfgName, GGameIni))
	{
		GlobalMapInfoSingleton = Cast<UGlobalMapInfo>(UObject::StaticLoadObject(
			UGlobalMapInfo::StaticClass(), NULL, *cnfgName, NULL, LOAD_NoWarn | LOAD_Quiet, NULL));

		if(!GlobalMapInfoSingleton)
		{
			appMsgf(AMT_OK, TEXT("Error loading map data. The config file may not exist, please check whether it is present locally:\r\n%s"), *cnfgName);
		}

		GlobalMapInfoSingleton->AddToRoot();
	}
	else
	{
		appErrorf(TEXT("GlobalMapInfo not load! Check in MMOGame/Config/DefaultGame.ini"));
	}
#else
	for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
	{
		GlobalMapInfoSingleton = Cast<UGlobalMapInfo>(Package->GetNetObjectAtIndex(j));
		if(GlobalMapInfoSingleton)
		{
			GlobalMapInfoSingleton->AddToRoot();
			break;
		}
	}
#endif
	return GlobalMapInfoSingleton;
}
//------------------------------------------------------------------------
UPackage* UGlobalMapInfo::GetGlobalMapInfoPackage()
{
	FString PackageName = TEXT("GlobalMapsInfos.upk");
	FString ExistingFile, SaveFileName;

	if( GPackageFileCache->FindPackageFile( *PackageName, NULL, ExistingFile ) )
	{
		// If the package already exists, save to the existing filename.
		SaveFileName = ExistingFile;
	}

	UPackage* Package = LoadPackage( NULL, *PackageName, LOAD_NoWarn );
	if( !Package )
	{
		appErrorf( TEXT("Unable to load %s"), *PackageName );
		return NULL;
	}

	if(!Package->IsFullyLoaded())
	{
		Package->FullyLoad();
	}

	return Package;
}
//------------------------------------------------------------------------
UGlobalMapInfo* UGlobalMapInfo::Singleton()
{
	return GetSingleton();
}
//------------------------------------------------------------------------
void UGlobalMapInfo::SavePackage()
{
#if WITH_EDITOR
	UPackage* PackageToSave = GetGlobalMapInfoPackage();
	FString PackageName = PackageToSave->GetName();
	FString ExistingFile;
	FString SaveFileName;

	if( GPackageFileCache->FindPackageFile( *PackageName, NULL, ExistingFile ) )
	{
		// If the package already exists, save to the existing filename.
		SaveFileName = ExistingFile;
	}


	TArray<UPackage*> Packages;
	Packages.AddItem(PackageToSave);
	FEditorFileUtils::PromptForCheckoutAndSave(Packages, TRUE, TRUE);
#endif
	//if( GFileManager->IsReadOnly( *SaveFileName ) )
	//{
	//	appMsgf( AMT_OK, *FString::Printf( LocalizeSecure(LocalizeUnrealEd("Error_CouldntWriteToFile_F"), *SaveFileName)) );
	//	//bAllPackagesWereSaved = FALSE;
	//}
	//else if( !GEngine->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""),
	//	*PackageName, *SaveFileName) ) )
	//{
	//	// Couldn't save.
	//	appMsgf( AMT_OK, *LocalizeUnrealEd("Error_CouldntSavePackage") );
	//	//bAllPackagesWereSaved = FALSE;
	//}
	////PackageToSave->SavePackage(PackageToSave, 0, 0, SaveFileName);
}
//------------------------------------------------------------------------
void UGlobalMapInfo::LoadTeleportsFromXML()
{
	GWarn->BeginSlowTask(TEXT("Refresh Global Map Data"), TRUE);

	UPackage* PackageToSave = GetGlobalMapInfoPackage();
	UGlobalMapInfo* singleton = Singleton();

	PackageToSave->MarkPackageDirty();
	//singleton->Teleports.Reset();
	
	TArray<FString> teleportFiles;
	FString globalMapInfoDir;
	
	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"),
		globalMapInfoDir, GGameIni);

	globalMapInfoDir = MMO::FormatTempleteDataLoadingDir(globalMapInfoDir);

	FString curDir = GFileManager->GetCurrentDirectory();
	GFileManager->SetCurDirectory(*globalMapInfoDir);
	const FString ext = TEXT(".teleport");
	GFileManager->FindFiles(teleportFiles, *(FString(TEXT("*")) + ext), TRUE, false);


	singleton->ServerMapInfoList.Empty();

	//for (INT i = 0; i < teleportFiles.Num(); ++i)
	//INT teleportNetNodeServerID = 0;
	//teleport net node reload,update flag
	for (INT k = TeleportNetNodeList.Num() - 1; k >= 0; --k){
		TeleportNetNodeList(k).IsUpdated = FALSE;
	}
	//TeleportNetNodeList.Empty();

	for (INT i = 0; i < ExportMaps.Num(); ++i )
	{
		FString xmlFile = ExportMaps(i).MapName + ext;
		if(xmlFile.Len() <= 0)
		{
			GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, 
				*FString::Printf( TEXT("Map with ID[%d] name is empty!"), ExportMaps(i).ServerID), NULL);
			continue;
		}

		if((GFileManager->FileSize(*xmlFile) == INDEX_NONE))
		{
			GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, 
				*FString::Printf( TEXT("Map export file not found:%s"), *ExportMaps(i).MapName), NULL);
			continue;
		}

		GWarn->StatusUpdatef(i + 1, teleportFiles.Num(), TEXT("%s"), *xmlFile);

		FMapTeleports* mapTeleports = 0;
		bool addNew = true;

		//UMapTeleports mapTeleportsNew;

		FString mapName = ExportMaps(i).MapName;


		for (INT j = 0; j < singleton->Teleports.Num(); ++j)
		{
			FMapTeleports* mt = &singleton->Teleports(j);
			if(mt->MapName == mapName)
			{
				mapTeleports = mt;
				addNew = false;
				break;
			}
		}

		FMapTeleports mapTeleportsObj(EC_EventParm);
		if(addNew)
		{
			mapTeleports = &mapTeleportsObj;
			mapTeleports->DrawScale = 1.0f;
			//mapTeleports = CastChecked<UMapTeleports>( UObject::StaticConstructObject(
			//	UMapTeleports::StaticClass(), singleton) );

			mapTeleports->MapName = mapName;
		}
		else if(mapTeleports->DrawScale == 0.0f)
		{
			mapTeleports->DrawScale = 1.0f;
		}


		XMLDoc doc;
		if(doc.loadFromFile(TCHAR_TO_ANSI(*xmlFile)))
		{
			FVector2D mapSize;
			mapSize.InitFromString(
				ANSI_TO_TCHAR(doc.getRoot().getAttribute("CanvasDrawSize").c_str()));
			mapSize *= 0.01; // conver to M
			mapTeleports->CanvasDrawSize = FIntPoint(mapSize.X, mapSize.Y);

			// load server map info
			FServerMapInfos mapInfoNew(EC_EventParm);

			bool addNewToServer = true;
			FServerMapInfos* mapInfo = &mapInfoNew;
			for (INT k = 0; k < ServerMapInfoList.Num(); ++k)
			{
				// if the current export list already has the same server map ID, do not add a new one
				if(ServerMapInfoList(k).ServerMapID == ExportMaps(i).ServerID)
				{
					mapInfo = &ServerMapInfoList(k);
					addNewToServer = false;
					break;
				}
			}

			mapInfo->MapName = ExportMaps(i).MapDisplayName; // said to use the Chinese name
			mapInfo->ConfigName = ExportMaps(i).MapName;
			mapInfo->ServerMapID = ExportMaps(i).ServerID;
			mapInfo->Map2DExportSize = mapSize;

			FExportMap mapExportInfo = FindMapInfo(ExportMaps(i).ServerID);
			mapInfo->ServerMapType = mapExportInfo.ServerMapType;
			//mapInfo->ServerMapType = getIntAttribuate("ServerMapType", doc.getRoot());
			if(addNewToServer)
			{
				if( ServerMapInfoList.Num() == 0 ){
					ServerMapInfoList.InsertItem( *mapInfo, 0 );
				}
				else{
					//sort from low to high of id
					INT cnt = ServerMapInfoList.Num();
					bool isInserted = false;
					for (INT i = 0; i < cnt; ++i){
						FServerMapInfos &exportServerMapInfo = ServerMapInfoList(i);

						if( exportServerMapInfo.ServerMapID >= mapInfo->ServerMapID ){
							ServerMapInfoList.InsertItem( *mapInfo, i );
							isInserted = true;
							break;
						}
					}//end of for
					//
					if( !isInserted ){
						ServerMapInfoList.Push( *mapInfo );
					}
				}
			}

			//original teleport reload,update flag
			for(INT k = mapTeleports->TeleportOrigList.Num() - 1; k >= 0; --k){
				mapTeleports->TeleportOrigList(k).IsUpdated = FALSE;
			}
			//target teleport reload,update flag
			for(INT k = mapTeleports->TeleportTargetList.Num() - 1; k >= 0; --k){
				mapTeleports->TeleportTargetList(k).IsUpdated = FALSE;
			}
			XMLElement e = doc.getRoot().firstChild();
			while(!e.isNull())
			{
				//UTeleportObject* tel =
				//	CastChecked<UTeleportObject>( UObject::StaticConstructObject(
				//	UTeleportObject::StaticClass(), singleton,
				//	FName(ANSI_TO_TCHAR(e.getAttribute("Name").c_str()))));
				
				FTeleportObject telObj(EC_EventParm);
				FTeleportObject* tel = &telObj;

				FString telObjName = ANSI_TO_TCHAR(e.getAttribute("Name").c_str());				

				bool addNew = true;
				if(e.name() == "TeleportOrig")
				{
					FVector loc;
					loc.InitFromString(ANSI_TO_TCHAR(e.getAttribute("Position").c_str()));
					for (INT k = 0; k < mapTeleports->TeleportOrigList.Num(); ++k)
					{
						if( mapTeleports->TeleportOrigList(k).Location == loc && mapTeleports->TeleportOrigList(k).Name == telObjName)
						{
							tel = &mapTeleports->TeleportOrigList(k);
							addNew = false;
							break;
						}
					}

					tel->ServerID = getIntAttribuate("ServerID", e);
					tel->Name = telObjName;
					tel->Location = loc;//.InitFromString(ANSI_TO_TCHAR(e.getAttribute("Position").c_str()));
					tel->DisplayName = UTF8_TO_TCHAR(e.getAttribute("DisplayName").c_str());
					tel->Radius = getIntAttribuate("Radius", e);

					tel->IsUpdated = TRUE;

					if(addNew)
					{
						mapTeleports->TeleportOrigList.AddItem(*tel);
					}
				}
				else if(e.name() == "TeleportTarget")
				{
					FVector loc;
					loc.InitFromString(ANSI_TO_TCHAR(e.getAttribute("Position").c_str()));
					for (INT k = 0; k < mapTeleports->TeleportTargetList.Num(); ++k)
					{
						if( mapTeleports->TeleportTargetList(k).Location == loc && mapTeleports->TeleportTargetList(k).Name == telObjName)
						{
							tel = &mapTeleports->TeleportTargetList(k);
							addNew = false;
							break;
						}
					}
					////junjie@snda
					//tel->ServerID = getIntAttribuate("ServerID", e);
					tel->Name = telObjName;
					tel->Location = loc;//.InitFromString(ANSI_TO_TCHAR(e.getAttribute("Position").c_str()));
					tel->DisplayName = UTF8_TO_TCHAR(e.getAttribute("DisplayName").c_str());
					tel->Radius = 1.0F;
					appSSCANF(ANSI_TO_TCHAR(e.getAttribute("Radius").c_str()), TEXT("%f"), &tel->Radius);
					tel->Yaw = getIntAttribuate("Rotation", e);
					tel->NpcId = getIntAttribuate("NpcId", e);
					tel->IsUpdated = TRUE;

					if(addNew)
					{
						mapTeleports->TeleportTargetList.AddItem(*tel);
					}
				}
				//Teleport station node
				else if(e.name() == "TeleportNetNode")
				{
					//INT teleportNetNodeID = getIntAttribuate("ServerID", e);

					//
					FTeleportNodeObj telNodeObj(EC_EventParm);
					FTeleportNodeObj* telNode = &telNodeObj;
					//for (INT k = 0; k < mapTeleports->TeleportNetNodeList.Num(); ++k)
					//{
					//	if(mapTeleports->TeleportNetNodeList(k).AutoServerID == teleportNetNodeID)
					//	{
					//		tel = &mapTeleports->TeleportNetNodeList(k);
					//		addNew = false;
					//		break;
					//	}
					//}
					INT nodeInSceneID = getIntAttribuate("InSceneID", e);
					for (INT k = 0; k < TeleportNetNodeList.Num(); ++k)
					{
						if( nodeInSceneID == TeleportNetNodeList(k).InSceneID && TeleportNetNodeList(k).Name == telObjName )
						{
							telNode = &TeleportNetNodeList(k);
							addNew = false;
							break;
						}
					}

					telNode->IsUpdated = TRUE;

					telNode->Name = telObjName;
					//telNode->AutoServerID = teleportNetNodeID;
					telNode->DisplayName = UTF8_TO_TCHAR( e.getAttribute("DisplayName").c_str() );
					telNode->PicResName = ANSI_TO_TCHAR( e.getAttribute("PicResName").c_str() );
					telNode->IsOpened = getIntAttribuate("State", e) > 0 ? TRUE : FALSE;
					telNode->InSceneID = nodeInSceneID;//getIntAttribuate("InSceneID", e);
					telNode->Region = getIntAttribuate("RegionID", e);
					//telNode->ShowPriority = getIntAttribuate("ShowPriority", e);
					telNode->Location.InitFromString(ANSI_TO_TCHAR(e.getAttribute("Position").c_str()));
					telNode->Radius = getIntAttribuate("Radius", e);
					telNode->Yaw = getIntAttribuate("Rotation", e);


					if(addNew){
						//mapTeleports->TeleportNetNodeList.AddItem(*telNode);
						TeleportNetNodeList.AddItem(*telNode);
					}
				}

				e = e.nextSibling();
			}

			//delete not updated original teleport
			for( INT k = mapTeleports->TeleportOrigList.Num() - 1; k >= 0; --k ){
				if( mapTeleports->TeleportOrigList(k).IsUpdated == FALSE ){
					mapTeleports->TeleportOrigList.Remove(k);
				}
			}
			///delete not updated target teleport
			for( INT k = mapTeleports->TeleportTargetList.Num() - 1; k >= 0; --k ){
				if( mapTeleports->TeleportTargetList(k).IsUpdated == FALSE ){
					mapTeleports->TeleportTargetList.Remove(k);
				}
			}

			if(addNew)
			{
				singleton->Teleports.AddItem(*mapTeleports);
			}
		}
		else
		{
			warnf( TEXT("[UGlobalMapInfo::LoadTeleportsFromXML] Load xml file failed:%s"),
				*xmlFile);
		}
	}

	//teleport net node reload,update flag
	for( INT k = TeleportNetNodeList.Num() - 1; k >= 0 ; --k ){
		//not updated means deleted
		if( TeleportNetNodeList(k).IsUpdated == FALSE ){
			TeleportNetNodeList.Remove(k);
			////again
			//++k;
		}
	}
	//teleport net node number
	INT finCnt = TeleportNetNodeList.Num();
	for( INT k = 0; k < finCnt ; ++k ){
		TeleportNetNodeList(k).ServerID = k;
		if( k >= 128 ){
			GWarn->MapCheck_Add( MCTYPE_WARNING, this, TEXT("-- Too mang teleport Net nodes, more than 128 nodes, excess nodes will not take effect."), NULL );
		}
	}

	UpdateLinks();

	GFileManager->SetCurDirectory(*curDir);
	
	GWarn->EndSlowTask();
	//SavePackage();
}
//------------------------------------------------------------------------
int UGlobalMapInfo::FindLinkByOrig( TArray<FTeleportLink*> *plinkList, 
	const FString& MapName,const FString& origObjName)
{
	int iCnt = 0;

	if( plinkList != NULL ){
		(*plinkList).Empty();
	}
	
	for (INT i = 0; i < TeleportLinks.Num(); ++i)
	{
		FTeleportLink* lnk = &TeleportLinks(i);
		if( (lnk->FromMapName == MapName)
			&& (lnk->From.Name == origObjName))
		{
			if( plinkList ){
				(*plinkList).Push(lnk);
			}
			++iCnt;
		}
	}

	return iCnt;
}
//------------------------------------------------------------------------

int UGlobalMapInfo::FindLinkByOrigID( TArray<FTeleportLink*> *plinkList, const FString& MapName,int origID)
{
	int iCnt = 0;

	if( plinkList != NULL ){
		(*plinkList).Empty();
	}

	for (INT i = 0; i < TeleportLinks.Num(); ++i)
	{
		FTeleportLink* lnk = &TeleportLinks(i);
		if( (lnk->FromMapName == MapName)
			&& (lnk->From.ServerID == origID))
		{
			if( plinkList ){
				(*plinkList).Push(lnk);
			}
			++iCnt;
		}
	}

	return iCnt;
}

// >add by zhangrui05@snda
int UGlobalMapInfo::GetMapLinks(INT fromMapId, INT toMapId, TArray<FTeleportLink*>& linkList)
{
	int count = 0;
	linkList.Empty();

	FString fromMapName = TEXT("");
	FString toMapName = TEXT("");
	UGlobalMapInfo::GetSingleton()->FindMapName(fromMapId, fromMapName);
	UGlobalMapInfo::GetSingleton()->FindMapName(toMapId, toMapName);

	for (INT i = 0; i < TeleportLinks.Num(); ++i)
	{
		FTeleportLink* link = &TeleportLinks(i);
		if (link && link->FromMapName == fromMapName && link->ToMapName == toMapName)
		{
			linkList.Push(link);
			count ++;
		}
	}

	return count;
}

//
FTeleportLink* UGlobalMapInfo::FindLinkByMapAndID( const FString& mapName, int linkID )
{
	FTeleportLink* pLink = NULL;
	for (INT i = 0; i < TeleportLinks.Num(); ++i){
		pLink = &TeleportLinks(i);
		if( (pLink->FromMapName == mapName) && (pLink->srcLinkID == linkID)){
			return pLink;
		}
	}//end of for
	return NULL;
}
//------------------------------------------------------------------------
void UGlobalMapInfo::UpdateLinks()
{
	typedef std::map<UINT, UINT, M_UE3_STL_ALLOC_MAP(UINT, UINT) > Uint2UintMap;
	Uint2UintMap origLinkMap;

	for (INT i = TeleportLinks.Num() - 1; i >= 0 ; --i)
	{
		FTeleportLink* lnk = &TeleportLinks(i);
		FTeleportObject* src = FindOrig(lnk->FromMapName, lnk->From.Name);
		FTeleportObject* targ = FindTarget(lnk->ToMapName, lnk->To.Name);

		lnk->IsInvalid = false;
		if(src)
		{
			lnk->From = *src;
		}
		else
		{
			lnk->IsInvalid = true;
		}

		if(targ)
		{
			lnk->To = *targ;
		}
		else
		{
			lnk->IsInvalid = true;
		}
		//
		if( lnk->IsInvalid ){
			TeleportLinks.Remove( i );
			continue;
		}
		//
		UINT key = (UINT)FindMapInfoByName( lnk->FromMapName ).ServerID;//*( (int*)&src );
		Uint2UintMap::iterator it = origLinkMap.find( key );
		if( it == origLinkMap.end() ){
			origLinkMap.insert( std::make_pair( key, 0 ) );
		}
		it = origLinkMap.find( key );

		UINT curIdx = ++(it->second);
		//int iCurLnkIdx = FindLinkByOrig( NULL, lnk->FromMapName, lnk->From.Name );
		//lnk->srcLinkID = ( ( lnk->From.ServerID) << 16 ) + ( curIdx & 0x0000ffff );
		lnk->srcLinkID = curIdx;
	}
}
//------------------------------------------------------------------------
FTeleportObject* UGlobalMapInfo::FindOrig(const FString& mapName, const FString& origObjName)
{
	for (INT i = 0; i < Teleports.Num(); ++i)
	{
		FMapTeleports* mapTel = &Teleports(i);
		if(mapTel->MapName == mapName)
		{
			for (INT j = 0; j < mapTel->TeleportOrigList.Num(); ++j)
			{
				if(mapTel->TeleportOrigList(j).Name == origObjName)
				{
					return &mapTel->TeleportOrigList(j);
				}
			}
		}
	}

	return 0;
}

//
const TArray< FTeleportObject > *UGlobalMapInfo::FindAllOrigInMap( const FString& mapName )
{
	const TArray< FTeleportObject > *pTeleports = NULL;
	for (INT i = 0; i < Teleports.Num(); ++i){
		FMapTeleports* mapTel = &Teleports(i);
		if(mapTel->MapName == mapName){
			pTeleports = &mapTel->TeleportOrigList;
			break;
		}
	}

	return pTeleports;
}

//------------------------------------------------------------------------
FTeleportObject* UGlobalMapInfo::FindTarget(const FString& mapName, const FString& origObjName)
{
	for (INT i = 0; i < Teleports.Num(); ++i)
	{
		FMapTeleports* mapTel = &Teleports(i);
		if(mapTel->MapName == mapName)
		{
			for (INT j = 0; j < mapTel->TeleportTargetList.Num(); ++j)
			{
				if(mapTel->TeleportTargetList(j).Name == origObjName)
				{
					return &mapTel->TeleportTargetList(j);
				}
			}
		}
	}

	return 0;
}
//------------------------------------------------------------------------
int UGlobalMapInfo::FindTeleportNetNodesInRegion( TArray<FTeleportNodeObj*> *pNodesList, INT RegionID )
{
	int iCnt = 0;

	if( pNodesList != NULL ){
		(*pNodesList).Empty();
	}

	for (INT j = 0; j < TeleportNetNodeList.Num(); ++j)
	{
		FTeleportNodeObj* pNode = &TeleportNetNodeList(j);
		if(pNode->Region == RegionID)
		{
			++iCnt;

			if( pNodesList ){
				(*pNodesList).Push(pNode);
			}
		}
	}//end of for TeleportNetNodeList in Teleports

	return iCnt;
}
//------------------------------------------------------------------------
//int UGlobalMapInfo::FindAllTeleportNetNodes( TArray<FTeleportNodeObj*> *pNodesList )
//{
//	int iCnt = 0;
//	if( pNodesList == NULL ){
//		return;
//	}
//
//	for (INT i = 0; i < Teleports.Num(); ++i)
//	{
//		FMapTeleports* mapTel = &Teleports(i);
//
//		for (INT j = 0; j < mapTel->TeleportNetNodeList.Num(); ++j)
//		{
//			FTeleportNodeObj* pNode = &mapTel->TeleportNetNodeList(j);
//
//			++iCnt;
//			(*plinkList).Push(pNode);
//		}//end of for TeleportNetNodeList in Teleports
//	}//end of for Teleports
//
//	return iCnt;
//}
//------------------------------------------------------------------------
FTeleportNodeObj* UGlobalMapInfo::FindTeleportNetNodes( INT nodeID )
{
	if( nodeID < 0 ){
		return NULL;
	}

	FTeleportNodeObj* pNode = NULL;
	INT nodesCnt = TeleportNetNodeList.Num();
	INT searchedCnt = 0;

	for( INT i = nodeID; searchedCnt < nodesCnt && i < nodesCnt; ){
		pNode = &TeleportNetNodeList(i);
		if( pNode->ServerID == nodeID ){
			break;
		}

		++searchedCnt;

		++i;
		i = ( i >= nodesCnt ) ? 0 : i;

	}//end of for TeleportNetNodeList in Teleports

	return pNode;
}
//------------------------------------------------------------------------
FTeleportNodeObj* UGlobalMapInfo::FindTeleportNetNodes( INT inSceneID, FString nodeName )
{
	FTeleportNodeObj* pNode = NULL;

	for (INT j = 0; j < TeleportNetNodeList.Num(); ++j)
	{
		pNode = &TeleportNetNodeList(j);
		if( inSceneID == pNode->InSceneID && pNode->Name == nodeName )
		{
			break;
		}
	}//end of for TeleportNetNodeList in Teleports

	return pNode;
}
//------------------------------------------------------------------------
void UGlobalMapInfo::ExportToServer()
{
#if !SHIPPING_PC_GAME

	GWarn->MapCheck_Clear();

	LoadTeleportsFromXML();

	// scene auto-numbering
// 	for (INT i = 0; i < ServerMapInfoList.Num(); ++i)
// 	{
// 		ServerMapInfoList(i).ServerMapID = i + 1;
// 	}

	{ // export scene config
		FString ServerMapCfgFile;
		GConfig->GetString(TEXT("Configuration"), TEXT("ServerMapCfgFile"), ServerMapCfgFile, GEditorIni);

		FString ServerMapCfgExcelFile;
		GConfig->GetString(TEXT("Configuration"), TEXT("ServerMapCfgExcelFile"), ServerMapCfgExcelFile, GEditorIni);

		FCheckoutResult sccResult1 = FSourceControl::TryCheckOut(ServerMapCfgFile);
		FCheckoutResult sccResult2 = FSourceControl::TryCheckOut(ServerMapCfgExcelFile);

		ServerMapCfgFile = GFileManager->ConvertToAbsolutePath(*ServerMapCfgFile);
		ServerMapCfgExcelFile = GFileManager->ConvertToAbsolutePath(*ServerMapCfgExcelFile);

		TArray< TArray<FString> > StringArrList;
		TArray<FString> rowData;
		rowData.AddItem(TEXT("SceneID"));
		rowData.AddItem(TEXT("Level"));
		rowData.AddItem(TEXT("MapDisplayName"));
		rowData.AddItem(TEXT("ExportMapName"));
		rowData.AddItem(TEXT("HideRoleName"));
		rowData.AddItem(TEXT("OpenMapPkMode"));
		rowData.AddItem(TEXT("MapPkMode"));
		rowData.AddItem(TEXT("Type"));
        //rowData.AddItem(TEXT("SceneStyle"));
		rowData.AddItem(TEXT("ClientSceneVariableType"));
		rowData.AddItem(TEXT("MapDescript"));
		rowData.AddItem(TEXT("MinimapName"));
// 		rowData.AddItem(TEXT("Width"));
// 		rowData.AddItem(TEXT("Height"));
		rowData.AddItem(TEXT("ConfigName"));
		rowData.AddItem(TEXT("MapName"));
		rowData.AddItem(TEXT("Map2DExportSize"));

        rowData.AddItem(TEXT("BattleType"));
        rowData.AddItem(TEXT("BattleSideCount"));
        rowData.AddItem(TEXT("MinLimitPerSide"));
        rowData.AddItem(TEXT("MaxLimitPerSide"));

        rowData.AddItem(TEXT("InitLineCount"));
		rowData.AddItem(TEXT("MaxLineCount"));

		rowData.AddItem(TEXT("MinPlayerLevel"));
		rowData.AddItem(TEXT("MaxPlayerLevel"));
		rowData.AddItem(TEXT("MinPortalLevel"));
		rowData.AddItem(TEXT("MaxPortalLevel"));

		rowData.AddItem(TEXT("Script"));
		rowData.AddItem(TEXT("FinishTarget"));

		////rowData.AddItem(TEXT("bPK"));
		//rowData.AddItem(TEXT("bEnablePKPunishment"));
		////rowData.AddItem(TEXT("bReducedDurability"));
		//rowData.AddItem(TEXT("DropItemDurability"));

		rowData.AddItem(TEXT("CannotUseScroll"));
		rowData.AddItem(TEXT("CannotUseTeleportRing"));
		rowData.AddItem(TEXT("CannotBeRecalled"));

		rowData.AddItem(TEXT("CannotGroupChat"));
		rowData.AddItem(TEXT("CannotSceneChat"));
		rowData.AddItem(TEXT("CannotScreenChat"));
		rowData.AddItem(TEXT("CannotWhisperChat"));
		rowData.AddItem(TEXT("CannotTeamChat"));
		rowData.AddItem(TEXT("CannotGuildChat"));
		rowData.AddItem(TEXT("CannotFriendChat"));

		rowData.AddItem(TEXT("CannotTrade"));
		//rowData.AddItem(TEXT("CannotDrop"));
		rowData.AddItem(TEXT("EnableAntiCheat"));
		rowData.AddItem(TEXT("GpkPunish"));
		rowData.AddItem(TEXT("GpkKick"));
		//
		rowData.AddItem(TEXT("EnableIntimacy"));
		//
		rowData.AddItem(TEXT("LoginJumpSceneTempID"));
		rowData.AddItem(TEXT("LoginJumpX"));
		rowData.AddItem(TEXT("LoginJumpY"));
		rowData.AddItem(TEXT("LoginJumpZ"));
		//
		rowData.AddItem(TEXT("MaxRoleCount"));
		rowData.AddItem(TEXT("BlockRole"));

		rowData.AddItem(TEXT("MineItemID1"));
		rowData.AddItem(TEXT("MineItemID2"));
		rowData.AddItem(TEXT("MineItemID3"));
		rowData.AddItem(TEXT("MineItemID4"));

		rowData.AddItem(TEXT("MineDoodadID1"));
		rowData.AddItem(TEXT("MineDoodadChance1"));
		rowData.AddItem(TEXT("MineDoodadID2"));
		rowData.AddItem(TEXT("MineDoodadChance2"));

		for(INT j = 0; j < UCONST_MaxSceneMineDrop; ++j)
		{
			rowData.AddItem(FString::Printf(TEXT("MineDropChance%d%d"), j/8+1, j%8));
			rowData.AddItem(FString::Printf(TEXT("MineDropGroup%d%d"), j/8+1, j%8));
		}

		rowData.AddItem(TEXT("CannotSendMail"));
		rowData.AddItem(TEXT("CannotUseExchange"));
		rowData.AddItem(TEXT("CannotUseAuction"));
		rowData.AddItem(TEXT("CannotJoinTeam"));

		for(INT j = 0; j < 4; ++j)
		{
			rowData.AddItem(FString::Printf(TEXT("Weather%d"), j + 1));
			rowData.AddItem(FString::Printf(TEXT("Weather%dWeight"), j + 1));
			rowData.AddItem(FString::Printf(TEXT("Weather%dMinDuration"), j + 1));
			rowData.AddItem(FString::Printf(TEXT("Weather%dMaxDuration"), j + 1));
		}

		rowData.AddItem(TEXT("MinWeatherRefreshInterval"));
		rowData.AddItem(TEXT("MaxWeatherRefreshInterval"));

		for (INT k = 0; k < UCONST_MaxSceneMajorTarget; ++k)
		{
			rowData.AddItem(FString::Printf(TEXT("Progress%dMainTarget_Desc"), k + 1));
			rowData.AddItem(FString::Printf(TEXT("Progress%dMainTarget_ID"), k + 1));
			rowData.AddItem(FString::Printf(TEXT("Progress%dMainTarget_TargetValue"), k + 1));
			for (INT m = 0; m < UCONST_MaxSceneMinorTarget; ++m)
			{
				rowData.AddItem(FString::Printf(TEXT("Progress%dChildTargets%d_Desc"), k + 1, m + 1));
				rowData.AddItem(FString::Printf(TEXT("Progress%dChildTargets%d_ID"), k + 1, m + 1));
				rowData.AddItem(FString::Printf(TEXT("Progress%dChildTargets%d_TargetValue"), k + 1, m + 1));
			}
		}

		StringArrList.AddItem(rowData);

		// add usage tag
		TArray<FString> rowUsageData;
		for (INT i = 0; i < rowData.Num(); ++i)
		{
			rowUsageData.AddItem(TEXT("Both"));
		}

		StringArrList.AddItem(rowUsageData);
		
		// default value
		rowUsageData.Empty();
		for (INT i = 0; i < rowData.Num(); ++i)
		{
			rowUsageData.AddItem(TEXT("0"));
		}
		StringArrList.AddItem(rowUsageData);

		for (INT i = 0; i < ServerMapInfoList.Num(); ++i)
		{
			rowData.Empty();
			FExportMap mapExportInfo = FindMapInfo(ServerMapInfoList(i).ServerMapID);
			rowData.AddItem(FString::Printf(TEXT("%d"), ServerMapInfoList(i).ServerMapID));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.Level));

			rowData.AddItem(mapExportInfo.MapDisplayName);
			rowData.AddItem(mapExportInfo.MapName);

			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.HideRoleName));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.OpenMapPkMode));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.MapPkMode));

			//rowData.AddItem(TEXT("_IMPORT_"));
			rowData.AddItem(FString::Printf(TEXT("%d"), ServerMapInfoList(i).ServerMapType));
            //rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.ServerMapStyle));

			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.ClientSceneVariableType));
			rowData.AddItem(mapExportInfo.MapDescript);
			rowData.AddItem(mapExportInfo.MinimapName);
// 			rowData.AddItem(FString::Printf(TEXT("%d"), (INT)ServerMapInfoList(i).Map2DExportSize.X));
// 			rowData.AddItem(FString::Printf(TEXT("%d"), (INT)ServerMapInfoList(i).Map2DExportSize.Y));			
			rowData.AddItem(ServerMapInfoList(i).ConfigName);
			rowData.AddItem(ServerMapInfoList(i).MapName);
			rowData.AddItem(ServerMapInfoList(i).Map2DExportSize.ToString());

            rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.BattleType));
            rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.SideCount));
            rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.MinLimitPerSide));
            rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.MaxLimitPerSide));

            rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.InitLineCount));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.MaxLineCount));

			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.MinPlayerLevel));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.MaxPlayerLevel));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.MinPortalLevel));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.MaxPortalLevel));

			rowData.AddItem(mapExportInfo.Script);

			// FinishTarget changed to Id, because NPCTemplate causes the client to preload lots of resources in game mode, modified by zhangrui05
			rowData.AddItem(FString::Printf(TEXT("%s"), mapExportInfo.FinishTarget > 0 ? *FString::Printf(TEXT("%d"), mapExportInfo.FinishTarget) : TEXT("0")));

			////rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.bPK));
			//rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.bEnablePKPunishment));
			////rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.bReducedDurability));
			//rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.DropItemDurability));

			rowData.AddItem(appItoa(mapExportInfo.UseScroolDisabled));
			rowData.AddItem(appItoa(mapExportInfo.UseTeleportRingDisabled));
			rowData.AddItem(appItoa(mapExportInfo.BeRecalledDisabled));

			rowData.AddItem(appItoa(mapExportInfo.ChatGroupDisabled));
			rowData.AddItem(appItoa(mapExportInfo.ChatSceneDisabled));
			rowData.AddItem(appItoa(mapExportInfo.ChatScreenDisabled));
			rowData.AddItem(appItoa(mapExportInfo.ChatWhisperDisabled));
			rowData.AddItem(appItoa(mapExportInfo.ChatTeamDisabled));
			rowData.AddItem(appItoa(mapExportInfo.ChatGuildDisabled));
			rowData.AddItem(appItoa(mapExportInfo.ChatFriendDisabled));

			rowData.AddItem(appItoa(mapExportInfo.TradeDisabled));
			//rowData.AddItem(appItoa(mapExportInfo.ItemDropDisabled));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.EnableAntiCheat));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.GpkPunish));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.GpkKick));
			//
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.EnableIntimacy));
			//
			rowData.AddItem(appItoa(mapExportInfo.LoginJumpSceneTempID));
			rowData.AddItem(appItoa(mapExportInfo.LoginJumpX));
			rowData.AddItem(appItoa(mapExportInfo.LoginJumpY));
			rowData.AddItem(appItoa(mapExportInfo.LoginJumpZ));
			//
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.MaxRoleCount));
			rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.BlockRole));
			
			rowData.AddItem(appItoa(mapExportInfo.MineItemID1));
			rowData.AddItem(appItoa(mapExportInfo.MineItemID2));
			rowData.AddItem(appItoa(mapExportInfo.MineItemID3));
			rowData.AddItem(appItoa(mapExportInfo.MineItemID4));

			rowData.AddItem(appItoa(mapExportInfo.MineDoodadID1));
			rowData.AddItem(appItoa(mapExportInfo.MineDoodadChance1));
			rowData.AddItem(appItoa(mapExportInfo.MineDoodadID2));
			rowData.AddItem(appItoa(mapExportInfo.MineDoodadChance2));

			for(INT j = 0; j < UCONST_MaxSceneMineDrop; ++j)
			{
				rowData.AddItem(appItoa(mapExportInfo.SceneMineDrop(j).Chance));
				rowData.AddItem(appItoa(mapExportInfo.SceneMineDrop(j).DropGroup));
			}

			rowData.AddItem(appItoa(mapExportInfo.CannotSendMail));
			rowData.AddItem(appItoa(mapExportInfo.CannotUseExchange));
			rowData.AddItem(appItoa(mapExportInfo.CannotUseAuction));
			rowData.AddItem(appItoa(mapExportInfo.CannotJoinTeam));

			for(INT j = 0; j < mapExportInfo.WeatherTypes.Num(); ++j)
			{
				static UEnum* weatherEnum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("ESCENE_WEATHER_TYPE"), TRUE);
				FString weatherTypeStr = weatherEnum->GetEnum(mapExportInfo.WeatherTypes(j).WeatherType).ToString();
				weatherTypeStr = weatherTypeStr.Right(weatherTypeStr.Len() - 2);
				weatherTypeStr = weatherTypeStr.Replace(TEXT("_"), TEXT(""));

				rowData.AddItem(FString::Printf(TEXT("%s"), *weatherTypeStr));
				rowData.AddItem(FString::Printf(TEXT("%d"), (INT)mapExportInfo.WeatherTypes(j).WeatherWeight));
				rowData.AddItem(FString::Printf(TEXT("%d"), (INT)mapExportInfo.WeatherTypes(j).MinDuration));
				rowData.AddItem(FString::Printf(TEXT("%d"), (INT)mapExportInfo.WeatherTypes(j).MaxDuration));
			}

			rowData.AddItem(FString::Printf(TEXT("%d"), (INT)mapExportInfo.MinWeatherRefreshInterval));
			rowData.AddItem(FString::Printf(TEXT("%d"), (INT)mapExportInfo.MaxWeatherRefreshInterval));

			for (INT k = 0; k < UCONST_MaxSceneMajorTarget; ++k)
			{
				rowData.AddItem(mapExportInfo.Progress(k).MainTarget.Desc);
				rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.Progress(k).MainTarget.ID));
				rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.Progress(k).MainTarget.TargetValue));

				for (INT m = 0; m < UCONST_MaxSceneMinorTarget; ++m)
				{
					rowData.AddItem(mapExportInfo.Progress(k).ChildTargets[m].Desc);
					rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.Progress(k).ChildTargets[m].ID));
					rowData.AddItem(FString::Printf(TEXT("%d"), mapExportInfo.Progress(k).ChildTargets[m].TargetValue));
				}
			}
			

			StringArrList.AddItem(rowData);
		}

#if WITH_EDITOR
		MServerMapCfgExport::ExportToExcel(StringArrList, ServerMapCfgExcelFile, ServerMapCfgFile);
#endif

		sccResult1.TryAddToDefaultChangelist();
		sccResult2.TryAddToDefaultChangelist();
	}
	//
	{ // export dungeon info
		FString ServerDungeonCfgFile;
		GConfig->GetString(TEXT("Configuration"), TEXT("ServerDungeonCfgFile"), ServerDungeonCfgFile, GEditorIni);

		FString ServerDungeonCfgExcelFile;
		GConfig->GetString(TEXT("Configuration"), TEXT("ServerDungeonCfgExcelFile"), ServerDungeonCfgExcelFile, GEditorIni);

		FCheckoutResult sccDungeonResult1 = FSourceControl::TryCheckOut(ServerDungeonCfgFile);
		FCheckoutResult sccDungeonResult2 = FSourceControl::TryCheckOut(ServerDungeonCfgExcelFile);

		ServerDungeonCfgFile = GFileManager->ConvertToAbsolutePath(*ServerDungeonCfgFile);
		ServerDungeonCfgExcelFile = GFileManager->ConvertToAbsolutePath(*ServerDungeonCfgExcelFile);

		TArray< TArray<FString> > DungeonStringArrList;
		TArray<FString> dungeonRowData;

		dungeonRowData.AddItem(TEXT("ID_Difficulty"));
		dungeonRowData.AddItem(TEXT("InitState"));
		dungeonRowData.AddItem(TEXT("Type"));

		dungeonRowData.AddItem(TEXT("MinLevel"));
		dungeonRowData.AddItem(TEXT("MaxLevel"));
		dungeonRowData.AddItem(TEXT("RoleLimit"));
		dungeonRowData.AddItem(TEXT("MaxEnterCount"));

		dungeonRowData.AddItem(TEXT("StandardTime"));
		dungeonRowData.AddItem(TEXT("StandardNpcValue"));
		dungeonRowData.AddItem(TEXT("StandardDoodadValue"));
		dungeonRowData.AddItem(TEXT("StandardAttackValue"));

		dungeonRowData.AddItem(TEXT("Exp_vip1"));
		dungeonRowData.AddItem(TEXT("Exp_vip2"));
		dungeonRowData.AddItem(TEXT("Exp_vip3"));

		dungeonRowData.AddItem(TEXT("Exp_team"));
		dungeonRowData.AddItem(TEXT("Exp_guild"));
		dungeonRowData.AddItem(TEXT("Exp_first"));
		dungeonRowData.AddItem(TEXT("Exp_activity"));

		dungeonRowData.AddItem(TEXT("Reward1_group"));
		dungeonRowData.AddItem(TEXT("Reward2_group"));
		dungeonRowData.AddItem(TEXT("Reward3_group"));

		dungeonRowData.AddItem(TEXT("Reward1_free"));
		dungeonRowData.AddItem(TEXT("Reward2_free"));
		dungeonRowData.AddItem(TEXT("Reward3_free"));

		dungeonRowData.AddItem(TEXT("Reward1_cost"));
		dungeonRowData.AddItem(TEXT("Reward2_cost"));
		dungeonRowData.AddItem(TEXT("Reward3_cost"));

		dungeonRowData.AddItem(TEXT("ItemIsAll"));
		dungeonRowData.AddItem(TEXT("ItemID"));
		dungeonRowData.AddItem(TEXT("ItemNumber"));
		dungeonRowData.AddItem(TEXT("Fatigue"));

		dungeonRowData.AddItem(TEXT("StandardBossValue"));
		dungeonRowData.AddItem(TEXT("StandardEliteNpcValue"));
		dungeonRowData.AddItem(TEXT("StandardSwiftTime"));
		//dungeonRowData.AddItem(TEXT("UndeadValue"));
		for(INT j = 0; j < UCONST_MaxDungeonCustomTarget; ++j)
		{
			//dungeonRowData.AddItem(FString::Printf(TEXT("TargetName%d"), j + 1));
			//dungeonRowData.AddItem(FString::Printf(TEXT("TargetDesc%d"), j + 1));
			dungeonRowData.AddItem(FString::Printf(TEXT("CustomGoalVarID%d"), j + 1));
			dungeonRowData.AddItem(FString::Printf(TEXT("CustomGoalVarValue%d"), j + 1));
			dungeonRowData.AddItem(FString::Printf(TEXT("CustomGoalScore%d"), j + 1));
		}

		for(INT j = 0; j < UCONST_MaxDungeonSpecialReward; ++j)
		{
			dungeonRowData.AddItem(FString::Printf(TEXT("DungeonSpecialRewardsID%d"), j + 1));
		}

		DungeonStringArrList.AddItem(dungeonRowData);

		// add usage tag
		TArray<FString> rowUsageData;
		for (INT i = 0; i < dungeonRowData.Num(); ++i)
		{
			rowUsageData.AddItem(TEXT("Both"));
		}

		DungeonStringArrList.AddItem(rowUsageData);

		// default value
		rowUsageData.Empty();
		for (INT i = 0; i < dungeonRowData.Num(); ++i)
		{
			rowUsageData.AddItem(TEXT("0"));
		}
		DungeonStringArrList.AddItem(rowUsageData);

		for (INT i = 0; i < ExportDungeonInfo.Num(); ++i)
		{
			dungeonRowData.Empty();

			FDungeonInfo &dungeonExportInfo = ExportDungeonInfo(i);
			//
			//INT dungeonInfoID = dungeonExportInfo.MapID * 10000 + dungeonExportInfo.Difficulty;
			INT dungeonInfoID = dungeonExportInfo.MapID;
			dungeonExportInfo.DungeonInfoID = dungeonInfoID;
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.DungeonInfoID));

			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.InitState));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Type));

			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.MinLevel));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.MaxLevel));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.RoleLimit));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.MaxEnterCount));

			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.StandardTime));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.StandardNpcValue));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.StandardDoodadValue));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.StandardAttackValue));

			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ExpForVip1));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ExpForVip2));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ExpForVip3));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ExpForTeam));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ExpForGuild));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ExpForFirst));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ExpForActivity));

			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Reward1_groupid));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Reward2_groupid));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Reward3_groupid));

			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Reward1_free));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Reward2_free));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Reward3_free));

			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Reward1_cost));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Reward2_cost));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Reward3_cost));

			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ItemIsAll));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ItemTemplateID));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.ItemNumber));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.Fatigue));

			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.StandardBossValue));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.StandardEliteNpcValue));
			dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.StandardSwiftTime));
			//dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.UndeadValue));

			for(INT j = 0; j < UCONST_MaxDungeonCustomTarget; ++j)
			{
				//dungeonRowData.AddItem(dungeonExportInfo.CustomTarget[j].TargetName);
				//dungeonRowData.AddItem(dungeonExportInfo.CustomTarget[j].TargetDesc);
				dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.CustomTarget[j].ID));
				dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.CustomTarget[j].TargetValue));
				dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.CustomTarget[j].TargetLevel));
			}

			for(INT j = 0; j < UCONST_MaxDungeonSpecialReward; ++j)
			{
				dungeonRowData.AddItem(FString::Printf(TEXT("%d"), dungeonExportInfo.DungeonSpecialRewardsID[j]));
			}

			DungeonStringArrList.AddItem(dungeonRowData);
		}
		sccDungeonResult1.TryAddToDefaultChangelist();
		sccDungeonResult2.TryAddToDefaultChangelist();

		MServerMapCfgExport::ExportToExcel(DungeonStringArrList, ServerDungeonCfgExcelFile, ServerDungeonCfgFile);
	}

	{ // export all scene teleport points to server
		//paired teleport points of 22
		FString ServerMapTeleportsFile;
		GConfig->GetString(TEXT("Configuration"), TEXT("ServerMapTeleportsFile"), ServerMapTeleportsFile, GEditorIni);
		FCheckoutResult sccResult = FSourceControl::TryCheckOut(ServerMapTeleportsFile);
		SaveToFile(ServerMapTeleportsFile);
		sccResult.TryAddToDefaultChangelist();
		//area teleport network
		FString ServerMapTeleportNetNodesFile;
		GConfig->GetString(TEXT("Configuration"), TEXT("ServerMapTeleportNetNodesFile"), ServerMapTeleportNetNodesFile, GEditorIni);
		FCheckoutResult stnnResult = FSourceControl::TryCheckOut(ServerMapTeleportNetNodesFile);
		SaveNetNodesToFile(ServerMapTeleportNetNodesFile);
		stnnResult.TryAddToDefaultChangelist();
	}

	LoadNPCInfoFromXMLFiles();
	LoadNpcSpawnGroupInfoFromXMLFiles();
	LoadQuestVolumeInfoFromXMLFiles();
	LoadMapSizeVolumeFromXML();
	LoadSearchPointFromXMLFiles();

	GWarn->MapCheck_ShowConditionally();
	
	this->MarkPackageDirty();
	this->SavePackage();

// 
// 	{ // export all scene teleport points to client
// 		FString ClientMapTeleportsFile;
// 		GConfig->GetString(TEXT("Configuration"), TEXT("ClientMapTeleportsFile"), ClientMapTeleportsFile, GEditorIni);
// 		FCheckoutResult sccResult = FSourceControl::TryCheckOut(ClientMapTeleportsFile);
// 		SaveToFile(ClientMapTeleportsFile);
// 		sccResult.TryAddToDefaultChangelist();
// 	}
	
#endif // !SHIPPING_PC_GAME
}

//------------------------------------------------------------------------
INT UGlobalMapInfo::FindMapID(const FString& mapName)
{
	for (INT i = 0; i < ExportMaps.Num(); ++i)
	{
		if(ExportMaps(i).MapName == mapName)
		{
			return ExportMaps(i).ServerID;
		}
	}

	return 0;
}
//------------------------------------------------------------------------
INT UGlobalMapInfo::FindIdxInMapOfType( INT mapID, BYTE type )
{
	INT idx = -1;
	bool isFound = false;
	for (INT i = 0; i < ExportMaps.Num(); ++i)
	{
		FServerMapInfos &serverMapInfo = ServerMapInfoList(i);

		if( serverMapInfo.ServerMapType == type ){
			++idx;

			if( serverMapInfo.ServerMapID == mapID ){
				isFound = true;
				break;
			}
		}

	}

	idx = isFound ? idx : -1;
	return idx;
}

//
void UGlobalMapInfo::FindAllMapOfType( BYTE type, TArray<UINT> &mapIDContainer )
{
	mapIDContainer.Empty();

	for (INT i = 0; i < ExportMaps.Num(); ++i)
	{
		FExportMap &serverMapInfo = ExportMaps(i);

		if( serverMapInfo.ServerMapType == type ){
			mapIDContainer.Push( serverMapInfo.ServerID );
		}
	}
}
////
//const FServerMapInfos *UGlobalMapInfo::FindServerMapInfo( INT mapID )
//{
//	FServerMapInfos *pFoundInfo = NULL;
//
//	for( INT i = ServerMapInfoList.Num() - 1; i >= 0; --i ){
//		if( ServerMapInfoList(i).ServerMapID == mapID ){
//			pFoundInfo = &ServerMapInfoList(i);
//			break;
//		}
//	}
//
//	return pFoundInfo;
//}
//------------------------------------------------------------------------
TArray<FDungeonInfo> &UGlobalMapInfo::GetDungeonInfoList()
{
	return ExportDungeonInfo;
}
//------------------------------------------------------------------------
const TArray<FExportMap> &UGlobalMapInfo::GetAllScenesInfo()
{
	return ExportMaps;
}

//------------------------------------------------------------------------
UBOOL UGlobalMapInfo::FindMapName(int mapID,FString &MapName )
{
	for (INT i = 0; i < ExportMaps.Num(); ++i)
	{
		if(ExportMaps(i).ServerID == mapID)
		{
			MapName = ExportMaps(i).MapName;
			return TRUE;
		}
	}
	warnf(NAME_Error, TEXT("====== [UGlobalMapInfo::Can't FindMapName ] INVALID_SCENE[%d]") ,mapID);
	return FALSE;
}
//------------------------------------------------------------------------
FExportMap UGlobalMapInfo::FindMapInfo(int mapID)
{
	for (INT i = 0; i < ExportMaps.Num(); ++i)
	{
		if(ExportMaps(i).ServerID == mapID)
		{
			return ExportMaps(i);
		}
	}

	FExportMap dft(EC_EventParm);
	dft.MapDisplayName = TEXT("N/A");
	dft.MapName = TEXT("N/A");
	
	return dft;
}
//------------------------------------------------------------------------
FExportMap* UGlobalMapInfo::FindMapInfoPointer(int mapID)
{
	for (INT i = 0; i < ExportMaps.Num(); ++i)
	{
		if(ExportMaps(i).ServerID == mapID)
		{
			return &ExportMaps(i);
		}
	}

	return NULL;
}

FServerMapInfos* UGlobalMapInfo::FindServerMapInfPointer(INT serverID)
{
	for (INT i = 0; i < ServerMapInfoList.Num(); ++i)
	{
		if(ServerMapInfoList(i).ServerMapID == serverID)
		{
			return &ServerMapInfoList(i);
		}
	}

	return NULL;
}

//---add by JimZhang@snda.com---------------------------------------------------------------------
FExportMap UGlobalMapInfo::FindMapInfoByName(const FString& mapName)
{
	for (INT i = 0; i < ExportMaps.Num(); ++i)
	{
		if(ExportMaps(i).MapName == mapName)
		{
			return ExportMaps(i);
		}
	}

	FExportMap dft(EC_EventParm);
	dft.MapDisplayName = TEXT("N/A");
	dft.MapName = TEXT("N/A");

	return dft;
}

//------------------------------------------------------------------------
INT UGlobalMapInfo::GetNpcInstanceInMap( INT sceneID, TArray<FNPCInstanceInfo*> &rtnList )
{
	rtnList.Empty();

	INT cnt = NPCInfoList.Num();
	FNPCInstanceInfo *pNpcInstance = NULL;
	INT dataBeginIdx = -1;
	for( INT i = 0; i < cnt; ++i ){
		pNpcInstance = &NPCInfoList(i);

		if( pNpcInstance->MapID == sceneID ){
			rtnList.Push( pNpcInstance );

			dataBeginIdx = ( ( dataBeginIdx == -1 ) ? i : dataBeginIdx );
			//continue search
			continue;
		}

		//all this map NPC data is in block, so if not in this block means not exit
		if( dataBeginIdx != -1 ){
			break;
		}
	}

	return dataBeginIdx;
}

//------------------------------------------------------------------------
const TArray<FNPCGroupInfo>* UGlobalMapInfo::GetNpcGroupsInMap( INT sceneID )
{
	const TArray<FNPCGroupInfo>* rtnList = NULL;

	INT cnt = SceneContentList.Num();
	FNPCGroupInfo *pNpcGroups = NULL;
	INT dataBeginIdx = -1;
	for( INT i = 0; i < cnt; ++i ){
		if( SceneContentList(i).MapID == sceneID ){
			dataBeginIdx = i;
			break;
		}
	}

	if( dataBeginIdx >= 0 ){
		rtnList = &SceneContentList(dataBeginIdx).NpcGroupsInfoList;
	}

	return rtnList;
}

//------------------------------------------------------------------------
const TArray<FQuestVolumeInfo>* UGlobalMapInfo::GetQuestVolumesInMap( INT sceneID )
{
	const TArray<FQuestVolumeInfo>* rtnList = NULL;

	INT cnt = QuestVolumeInfoList.Num();
	INT dataBeginIdx = -1;
	for( INT i = 0; i < cnt; ++i ){
		if( QuestVolumeInfoList(i).MapID == sceneID ){
			dataBeginIdx = i;
			break;
		}
	}

	if( dataBeginIdx >= 0 ){
		rtnList = &QuestVolumeInfoList(dataBeginIdx).Infos;
	}

	return rtnList;
}
//------------------------------------------------------------------------
const TArray<FNaviPointInfo>* UGlobalMapInfo::GetAllNavPointsInMap( INT sceneID )
{
	const TArray<FNaviPointInfo>* rtnList = NULL;

	INT cnt = NaviPointInfoList.Num();
	INT dataBeginIdx = -1;
	for( INT i = 0; i < cnt; ++i ){
		if( NaviPointInfoList(i).MapID == sceneID ){
			dataBeginIdx = i;
			break;
		}
	}

	if( dataBeginIdx >= 0 ){
		rtnList = &NaviPointInfoList(dataBeginIdx).Infos;
	}

	return rtnList;
}
//------------------------------------------------------------------------
TArray<FInMapRegion> &UGlobalMapInfo::GetAllMapRegionInfo()
{
	return MapZoneList;
}

//------------------------------------------------------------------------
void UGlobalMapInfo::SaveToFile( const FString& BinFile ) 
{
	FArchive* ar = GFileManager->CreateFileWriter(*BinFile);
	if(ar != NULL)
	{
		INT cnt = this->TeleportLinks.Num();
		ar->ByteOrderSerialize(&cnt, sizeof(INT));

		for (INT i = 0; i < cnt; ++i)
		{
			SCENE_TELEPORTS teleportLink = {0};

			//teleportLink.ID = TeleportLinks(i).From.ServerID;
			//junjie@snda: support 1 original teleport to N target teleports
			//teleportLink.ID = ( (TeleportLinks(i).From.ServerID) << 16 ) + ( TeleportLinks(i).To.ServerID & 0x0000ffff );
			//teleportLink.ID = ( (TeleportLinks(i).From.ServerID) << 16 ) + ( i & 0x0000ffff );
			teleportLink.ID = TeleportLinks(i).srcLinkID;

			teleportLink.SrcSceneID = FindMapID(TeleportLinks(i).FromMapName);
			teleportLink.SrcRadius = TeleportLinks(i).From.Radius * POINT_PER_CM;
			FVector p = TeleportLinks(i).From.Location;
			ConvertCoordC2S(p);
			p.X = (INT32)p.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
			p.Y = (INT32)p.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
			teleportLink.SrcSceneX = p.X;
			teleportLink.SrcSceneY = p.Y;
			teleportLink.SrcSceneZ = p.Z;

			teleportLink.TargetSceneID = FindMapID(TeleportLinks(i).ToMapName);
			p = TeleportLinks(i).To.Location;
			ConvertCoordC2S(p);
			p.X = (INT32)p.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
			p.Y = (INT32)p.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
			teleportLink.TargetSceneX = p.X;
			teleportLink.TargetSceneY = p.Y;
			teleportLink.TargetSceneZ = p.Z;
			teleportLink.TargetRotationYaw = ConvertToServerDegree(TeleportLinks(i).To.Yaw);
			teleportLink.TargetRadius = (INT32)(TeleportLinks(i).To.Radius * POINT_PER_CM);

			ar->ByteOrderSerialize(&teleportLink, sizeof(SCENE_TELEPORTS));
		}

		ar->Close();
	}
}
//------------------------------------------------------------------------
void UGlobalMapInfo::SaveNetNodesToFile( const FString& BinFile ) 
{
	FArchive* ar = GFileManager->CreateFileWriter(*BinFile);
	if(ar != NULL)
	{
		INT cnt = this->TeleportNetNodeList.Num();
		ar->ByteOrderSerialize(&cnt, sizeof(INT));

		for (INT i = 0; i < cnt; ++i)
		{
			TELEPORT_PLATFORM teleportNode = {0};

			teleportNode.ID = TeleportNetNodeList(i).ServerID;
			teleportNode.nInitEnable = ( TeleportNetNodeList(i).IsOpened > 0 ? TRUE : FALSE );

			teleportNode.nSceneID = TeleportNetNodeList(i).InSceneID;
			teleportNode.nRadius = TeleportNetNodeList(i).Radius * POINT_PER_CM;
			FVector p = TeleportNetNodeList(i).Location;
			ConvertCoordC2S(p);
			p.X = (INT32)p.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
			p.Y = (INT32)p.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
			teleportNode.nSceneX = p.X;
			teleportNode.nSceneY = p.Y;
			teleportNode.nSceneZ = p.Z;
			teleportNode.nRotationYaw = ConvertToServerDegree(TeleportNetNodeList(i).Yaw);
			teleportNode.nMoneyType = TeleportNetNodeList(i).NeedMnyType;
			teleportNode.nMoney = TeleportNetNodeList(i).NeedMoney;

			ar->ByteOrderSerialize(&teleportNode, sizeof(TELEPORT_PLATFORM));
		}

		ar->Close();
	}
}
//------------------------------------------------------------------------

/**
 * Called when a property on this object has been modified externally
 *
 * @param PropertyThatChanged the property that was modified
 */
void UGlobalMapInfo::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//LoadTeleportsFromXML();
}
//------------------------------------------------------------------------
void UGlobalMapInfo::LoadNPCInfoFromXMLFiles()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("UGlobalMapInfo::LoadNPCInfoFromXMLFiles"), 0);

	GWarn->BeginSlowTask(TEXT("Refresh NPC Data"), TRUE);

	TArray<FString> npcFiles;
	FString globalMapInfoDir;

	UGlobalMapInfo* singleton = Singleton();

	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"),
		globalMapInfoDir, GGameIni);
	globalMapInfoDir = MMO::FormatTempleteDataLoadingDir(globalMapInfoDir);

	FString curDir = GFileManager->GetCurrentDirectory();
	GFileManager->SetCurDirectory(*globalMapInfoDir);
	const FString ext = TEXT(".npc");
	GFileManager->FindFiles(npcFiles, *(FString(TEXT("*")) + ext), TRUE, false);

	singleton->NPCInfoList.Empty();

	for (INT i = 0; i < npcFiles.Num(); ++i)
	{
		FString xmlFile = npcFiles(i);

		GWarn->StatusUpdatef(i + 1, npcFiles.Num(), TEXT("%s"), *xmlFile);

		INT mapID = FindMapID(xmlFile.Left(xmlFile.Len() - ext.Len()));
		if( mapID <= 0 ){
			//not found
			continue;
		}

		XMLDoc doc;
		if(doc.loadFromFile(TCHAR_TO_ANSI(*xmlFile)))
		{
			XMLElement e = doc.getRoot().firstChild();
			while(!e.isNull())
			{
				if(e.name() == "NPC")
				{
					FNPCInstanceInfo info(EC_EventParm);
					info.MapID = mapID;
					info.Name = ANSI_TO_TCHAR( e.getAttribute("Name").c_str() );
					info.TemplateID = getIntAttribuate("TemplateID", e);

					info.NpcDiplayName = UTF8_TO_TCHAR( e.getAttribute("DisplayName").c_str() );
					info.NpcType = getIntAttribuate("NpcType", e);
					info.NpcFuncType = getIntAttribuate("NpcFuncType", e);
					
					info.Position.InitFromString(ANSI_TO_TCHAR(e.getAttribute("Position").c_str()));
					singleton->NPCInfoList.AddItem(info);
				} // if

				e = e.nextSibling();
			} // while
		} // if
	}

	GFileManager->SetCurDirectory(*curDir);

	GWarn->EndSlowTask();
}

//------------------------------------------------------------------------
void UGlobalMapInfo::LoadNpcSpawnGroupInfoFromXMLFiles()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("UGlobalMapInfo::LoadNpcSpawnGroupInfoFromXMLFiles"), 0);

	GWarn->BeginSlowTask(TEXT("Refresh NpcSpawnGroup Data"), TRUE);

	FString globalMapInfoDir;

	UGlobalMapInfo* singleton = Singleton();

	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"), globalMapInfoDir, GGameIni);
	globalMapInfoDir = MMO::FormatTempleteDataLoadingDir(globalMapInfoDir);

	FString curDir = GFileManager->GetCurrentDirectory();
	GFileManager->SetCurDirectory(*globalMapInfoDir);

	const FString ext = TEXT(".spawngroup");
	TArray<FString> npcGroupFiles;
	GFileManager->FindFiles(npcGroupFiles, *(FString(TEXT("*")) + ext), TRUE, false);

	//new data takes precedence
	singleton->SceneContentList.Empty();

	//
	for (INT i = 0; i < npcGroupFiles.Num(); ++i)
	{
		FString xmlFile = npcGroupFiles(i);

		GWarn->StatusUpdatef(i + 1, npcGroupFiles.Num(), TEXT("%s"), *xmlFile);

		INT mapID = FindMapID(xmlFile.Left(xmlFile.Len() - ext.Len()));
		if( mapID <= 0 ){
			GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("The map for the %s.Spawngroup file is not in ExportMap. Add it to ExportMap or delete the file!"), *xmlFile.Left(xmlFile.Len() - ext.Len()) ), NULL);
			continue;
		}

		INT sceneContentListCnt = singleton->SceneContentList.Num();
		FSceneContent *pCurSceneContent = NULL;
		for( INT j = 0; j < sceneContentListCnt; ++j ){
			if( singleton->SceneContentList(j).MapID == mapID ){
				pCurSceneContent = &singleton->SceneContentList(j);
				//cleanup
				pCurSceneContent->NpcGroupsInfoList.Empty();
				break;
			}
		}

		if( pCurSceneContent == NULL ){
			FSceneContent newSceneContent(EC_EventParm);
			newSceneContent.MapID = mapID;
			singleton->SceneContentList.Push( newSceneContent );
			pCurSceneContent = &singleton->SceneContentList.Last();
		}

		XMLDoc doc;
		if(doc.loadFromFile(TCHAR_TO_ANSI(*xmlFile)))
		{
			XMLElement e = doc.getRoot().firstChild();
			while(!e.isNull())
			{
				if(e.name() == "NPCGroup")
				{
					FNPCGroupInfo info(EC_EventParm);
					info.DisplayName = UTF8_TO_TCHAR( e.getAttribute("DisplayName").c_str() );
					info.NpcTemplateID = getIntAttribuate("GroupNpcID", e);
					info.NpcLevelMin = getIntAttribuate("GroupNpcLv", e);

					info.MapID = mapID;
					info.Position.InitFromString(ANSI_TO_TCHAR(e.getAttribute("Position").c_str()));
					info.GroupType = ( EENUM_NPC_GROUP_TYPE )getIntAttribuate("GroupType", e);

					pCurSceneContent->NpcGroupsInfoList.Push(info);
				} // if

				e = e.nextSibling();
			} // while
		} // if
	}

	GFileManager->SetCurDirectory(*curDir);

	GWarn->EndSlowTask();
}
//------------------------------------------------------------------------
void UGlobalMapInfo::LoadMapSizeVolumeFromXML()
{
	GWarn->BeginSlowTask(TEXT("Refresh MapSizeVolume Data"), TRUE);

	TArray<FString> mapZoneFiles;
	FString globalMapInfoDir;

	UGlobalMapInfo* singleton = Singleton();

	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"),
		globalMapInfoDir, GGameIni);
	globalMapInfoDir = MMO::FormatTempleteDataLoadingDir(globalMapInfoDir);

	FString curDir = GFileManager->GetCurrentDirectory();
	GFileManager->SetCurDirectory(*globalMapInfoDir);
	const FString ext = TEXT(".volume");
	GFileManager->FindFiles(mapZoneFiles, *(FString(TEXT("*")) + ext), TRUE, false);

	singleton->MapZoneList.Empty();

	for (INT i = 0; i < mapZoneFiles.Num(); ++i)
	{
		FString xmlFile = mapZoneFiles(i);

		GWarn->StatusUpdatef(i + 1, mapZoneFiles.Num(), TEXT("%s"), *xmlFile);

		INT mapID = FindMapID(xmlFile.Left(xmlFile.Len() - ext.Len()));
		if( mapID <= 0 ){
			continue;
		}

		XMLDoc doc;
		if(doc.loadFromFile(TCHAR_TO_ANSI(*xmlFile)))
		{
			XMLElement e = doc.getRoot().firstChild();
			while(!e.isNull())
			{
				if(e.name() == "VolumeMapSize")
				{
					FInMapRegion info(EC_EventParm);
					info.MapID = mapID;

					info.VolumeName = UTF8_TO_TCHAR( e.getAttribute("Name").c_str() );
					info.DisplayName = UTF8_TO_TCHAR( e.getAttribute("DisplayName").c_str() );
					info.Priority = getIntAttribuate("Priority", e);
					//info.IsReuseMapResouce = ( info.Priority == EMSP_NotShow );
					info.ReuseMapVolumeName = UTF8_TO_TCHAR( e.getAttribute("ReuseMapVolumeName").c_str() );
					TArray<FVector> boundingPos;
					FVector pos;
					pos.InitFromString(ANSI_TO_TCHAR(e.getAttribute("MinPos").c_str()));
					boundingPos.Push( pos );
					pos.InitFromString(ANSI_TO_TCHAR(e.getAttribute("MaxPos").c_str()));
					boundingPos.Push( pos );
					FBox boundingBox( boundingPos );
					info.BoundingBox = boundingBox;

					info.MiniMapTileRowNum = getIntAttribuate("MiniMapTileRowNum", e);
					info.MiniMapTileColNum = getIntAttribuate("MiniMapTileColNum", e);
					info.MiniMapPixelSize = getFloatAttribuate("MiniMapPixelSize", e);
					info.MiniMapScale = getFloatAttribuate("MiniMapScale", e);
					info.MiniMapOffsetX = getIntAttribuate("MiniMapOffsetX", e);
					info.MiniMapOffsetY = getIntAttribuate("MiniMapOffsetY", e);

					info.WorldMapTileRowNum = getIntAttribuate("WorldMapTileRowNum", e);
					info.WorldMapTileColNum = getIntAttribuate("WorldMapTileColNum", e);
					info.WorldMapPixelSize = getFloatAttribuate("WorldMapPixelSize", e);
					info.WorldMapScale = getFloatAttribuate("WorldMapScale", e);
					info.WorldMapOffsetX = getIntAttribuate("WorldMapOffsetX", e);
					info.WorldMapOffsetY = getIntAttribuate("WorldMapOffsetY", e);

					singleton->MapZoneList.AddItem(info);
				} // if

				e = e.nextSibling();
			} // while
		} // if
	}

	GFileManager->SetCurDirectory(*curDir);

	GWarn->EndSlowTask();
}

void UGlobalMapInfo::LoadSearchPointFromXMLFiles()
{
	GWarn->BeginSlowTask(TEXT("Refresh NPC Data"), TRUE);

	TArray<FString> searchFiles;
	FString globalMapInfoDir;

	UGlobalMapInfo* singleton = Singleton();

	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"),
		globalMapInfoDir, GGameIni);

	FString curDir = GFileManager->GetCurrentDirectory();
	GFileManager->SetCurDirectory(*globalMapInfoDir);
	const FString ext = TEXT(".search");
	GFileManager->FindFiles(searchFiles, *(FString(TEXT("*")) + ext), TRUE, false);

	singleton->SearchPoints.Empty();

	for (INT i = 0; i < searchFiles.Num(); ++i)
	{
		FString xmlFile = searchFiles(i);

		GWarn->StatusUpdatef(i + 1, searchFiles.Num(), TEXT("%s"), *xmlFile);

		INT mapID = FindMapID(xmlFile.Left(xmlFile.Len() - ext.Len()));
		if( mapID <= 0 ){
			continue;
		}

		XMLDoc doc;
		if(doc.loadFromFile(TCHAR_TO_ANSI(*xmlFile)))
		{
			XMLElement e = doc.getRoot().firstChild();
			while(!e.isNull())
			{
				if(e.name() == "SearchPoint")
				{
					FSearchPointObject info;
					info.SearchPointID		= getIntAttribuate("SearchPointID", e);
					info.SearchPointName	= ANSI_TO_TCHAR(e.getAttribute("SearchPointName").c_str());
					singleton->SearchPoints.AddItem(info);
				} // if

				e = e.nextSibling();
			} // while
		} // if
	}

	GFileManager->SetCurDirectory(*curDir);

	GWarn->EndSlowTask();	
}

// add by zhangrui05@snda.com
void UGlobalMapInfo::LoadNaviPointInfoFromXMLFiles()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("UGlobalMapInfo::LoadNaviPointInfoFromXMLFiles"), 0);

	GWarn->BeginSlowTask(TEXT("Refresh NaviPoint Data"), TRUE);

	FString globalMapInfoDir;
	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"),
		globalMapInfoDir, GGameIni);
	globalMapInfoDir = MMO::FormatTempleteDataLoadingDir(globalMapInfoDir);

	FString curDir = GFileManager->GetCurrentDirectory();
	GFileManager->SetCurDirectory(*globalMapInfoDir);

	TArray<FString> naviPointFiles;
	const FString ext = TEXT(".navipoints");
	GFileManager->FindFiles(naviPointFiles, *(FString(TEXT("*")) + ext), TRUE, false);

	NaviPointInfoList.Empty();
	for (INT i = 0; i < naviPointFiles.Num(); ++i)
	{
		FString fileBuffer = naviPointFiles(i);

		GWarn->StatusUpdatef(i + 1, naviPointFiles.Num(), TEXT("%s"), *fileBuffer);
		INT mapID = FindMapID(fileBuffer.Left(fileBuffer.Len() - ext.Len()));
		if( mapID <= 0 ){
			FString mapName = fileBuffer.Left(fileBuffer.Len() - ext.Len());
			GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("%s.navipoints cannot find the matching map (%s)!"), *mapName, *mapName ), NULL);
			continue;
		}

		INT mapNavPointListCnt = NaviPointInfoList.Num();
		FMapNaviPointInfo *pMapNavPoints = NULL;
		for( INT j = 0; j < mapNavPointListCnt; ++j )
		{
			if( NaviPointInfoList(j).MapID == mapID )
			{
				pMapNavPoints = &NaviPointInfoList(j);
				//cleanup
				pMapNavPoints->Infos.Empty();
				break;
			}
		}

		if( pMapNavPoints == NULL )
		{
			FMapNaviPointInfo newMapNavPointsInfo(EC_EventParm);
			newMapNavPointsInfo.MapID = mapID;
			NaviPointInfoList.Push( newMapNavPointsInfo );
			pMapNavPoints = &NaviPointInfoList.Last();
		}

		XMLDoc doc;
		if (doc.loadFromFile(TCHAR_TO_ANSI(*fileBuffer)))
		{
			XMLElement element = doc.getRoot().firstChild();
			while (!element.isNull())
			{
				if (element.name() == "NaviPoint")
				{
					FNaviPointInfo info(EC_EventParm);
					//info.MapId = mapID;
					info.Name = ANSI_TO_TCHAR(element.getAttribute("Name").c_str());
					info.DisplayName = UTF8_TO_TCHAR(element.getAttribute("DisplayName").c_str());
					info.IsShowOnUI = (ENavPointUIShowType)getIntAttribuate("IsShow", element);
					info.Position.InitFromString(ANSI_TO_TCHAR(element.getAttribute("Position").c_str()));
					info.RedirectLoc.InitFromString(ANSI_TO_TCHAR(element.getAttribute("RedirectLoc").c_str()));
					pMapNavPoints->Infos.Push(info);
				}

				element = element.nextSibling();
			}
		}
	}

	GFileManager->SetCurDirectory(*curDir);
	GWarn->EndSlowTask();
}

//------------------------------------------------------------------------
void UGlobalMapInfo::LoadQuestVolumeInfoFromXMLFiles()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("UGlobalMapInfo::LoadQuestVolumeInfoFromXMLFiles"), 0);

	GWarn->BeginSlowTask(TEXT("Refresh QuestVolume Data"), TRUE);

	FString globalMapInfoDir;
	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"), globalMapInfoDir, GGameIni);
	globalMapInfoDir = MMO::FormatTempleteDataLoadingDir(globalMapInfoDir);

	FString curDir = GFileManager->GetCurrentDirectory();
	GFileManager->SetCurDirectory(*globalMapInfoDir);

	TArray<FString> questVolFiles;
	const FString ext = TEXT(".trigvolume");
	GFileManager->FindFiles(questVolFiles, *(FString(TEXT("*")) + ext), TRUE, false);

	QuestVolumeInfoList.Empty();
	for (INT i = 0; i < questVolFiles.Num(); ++i)
	{
		FString fileBuffer = questVolFiles(i);

		GWarn->StatusUpdatef(i + 1, questVolFiles.Num(), TEXT("%s"), *fileBuffer);
		INT mapID = FindMapID(fileBuffer.Left(fileBuffer.Len() - ext.Len()));
		if( mapID <= 0 ){
			GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("Trigvolume file with Name[%s] is not found in ExportMap!"), *fileBuffer.Left(fileBuffer.Len() - ext.Len()) ), NULL);
			continue;
		}

		INT mapQuestVolListCnt = QuestVolumeInfoList.Num();
		FMapQuestVolumeInfo *pCurMapQuestVols = NULL;
		for( INT j = 0; j < mapQuestVolListCnt; ++j ){
			if( QuestVolumeInfoList(j).MapID == mapID ){
				pCurMapQuestVols = &QuestVolumeInfoList(j);
				//cleanup
				pCurMapQuestVols->Infos.Empty();
				break;
			}
		}

		if( pCurMapQuestVols == NULL ){
			FMapQuestVolumeInfo newMapTriggerVolumeInfo(EC_EventParm);
			newMapTriggerVolumeInfo.MapID = mapID;
			QuestVolumeInfoList.Push( newMapTriggerVolumeInfo );
			pCurMapQuestVols = &QuestVolumeInfoList.Last();
		}

		XMLDoc doc;
		if (doc.loadFromFile(TCHAR_TO_ANSI(*fileBuffer)))
		{
			XMLElement element = doc.getRoot().firstChild();
			while (!element.isNull())
			{
				if (element.name() == "TriggerVolume")
				{
					FQuestVolumeInfo info(EC_EventParm);
					info.MapID = mapID;
					info.Name = ANSI_TO_TCHAR(element.getAttribute("Name").c_str());
					info.Position.InitFromString(ANSI_TO_TCHAR(element.getAttribute("Position").c_str()));
					info.Quests.AddItem(appAtoi(ANSI_TO_TCHAR(element.getAttribute("QuestId0").c_str())));
					info.Quests.AddItem(appAtoi(ANSI_TO_TCHAR(element.getAttribute("QuestId1").c_str())));
					info.Quests.AddItem(appAtoi(ANSI_TO_TCHAR(element.getAttribute("QuestId2").c_str())));
					info.Quests.AddItem(appAtoi(ANSI_TO_TCHAR(element.getAttribute("QuestId3").c_str())));
					pCurMapQuestVols->Infos.Push(info);
				}

				element = element.nextSibling();
			}
		}
	}

	GFileManager->SetCurDirectory(*curDir);
	GWarn->EndSlowTask();
}

void UGlobalMapInfo::PostLoad()
{
	Super::PostLoad();

	for (INT i = 0; i < ExportMaps.Num(); ++i)
	{
		INT addNew = 4 - ExportMaps(i).WeatherTypes.Num();
		if(addNew > 0)
		{
			ExportMaps(i).WeatherTypes.AddZeroed(addNew);
		}

		addNew = UCONST_MaxSceneMajorTarget - ExportMaps(i).Progress.Num();
		if(addNew > 0)
		{
			ExportMaps(i).Progress.AddZeroed(addNew);
		}

		addNew = UCONST_MaxSceneMineDrop - ExportMaps(i).SceneMineDrop.Num();
		if(addNew > 0)
		{
			ExportMaps(i).SceneMineDrop.AddZeroed(addNew);
		}
	}
}


//------------------------------------------------------------------------------------------------
// reverse-import scene config data
//------------------------------------------------------------------------------------------------

#if !SHIPPING_PC_GAME

#include "MMOTemplateObjectFinder.h"

struct SceneConfigDataExcelImporter
{
	static const TCHAR* GetName()
	{
		return TEXT("scene.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static FExportMap* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		FExportMap* item = new FExportMap;
		item->ServerID = templateID;
		return item;
	}

	static void DestroyDataObject(FExportMap* obj)
	{
		delete obj;
	}

	static void ParseData(FExportMap* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("SceneID");
	}

	static FString FormatAsString(FExportMap* obj)
	{
		return FString::Printf(TEXT("ExportMap[%d] %s"), obj->ServerID, *obj->MapDisplayName);
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(FExportMap* obj, TSet<FString>& outResList)
	{
		// FinishTarget changed to Id, because NPCTemplate causes the client to preload lots of resources in game mode, modified by zhangrui05
		//_CollectReferencedResource(obj->FinishTarget, outResList);
	}
#endif
};

typedef TDataManger<FExportMap, SceneConfigDataExcelImporter> SceneConfigReader;

void SceneConfigDataExcelImporter::ParseData(FExportMap* obj, class CTabFile* dataRow, int rowIdx)
{
	obj->ServerID = TReadDataValue_Int<SceneConfigReader>(TEXT("SceneID"), dataRow, rowIdx);
	obj->Level = TReadDataValue_Int<SceneConfigReader>(TEXT("Level"), dataRow, rowIdx);
	obj->MapDisplayName = TReadDataValue_FString<SceneConfigReader>(TEXT("MapDisplayName"), dataRow, rowIdx);
	obj->MapName = TReadDataValue_FString<SceneConfigReader>(TEXT("ExportMapName"), dataRow, rowIdx);
	obj->HideRoleName = TReadDataValue_Int<SceneConfigReader>(TEXT("HideRoleName"), dataRow, rowIdx);
	obj->OpenMapPkMode = TReadDataValue_Int<SceneConfigReader>(TEXT("OpenMapPkMode"), dataRow, rowIdx);
	obj->MapPkMode = TReadDataValue_Int<SceneConfigReader>(TEXT("MapPkMode"), dataRow, rowIdx);

	obj->ServerMapType = TReadDataValue_Int<SceneConfigReader>(TEXT("Type"), dataRow, rowIdx);
	//obj->ServerMapStyle = TReadDataValue_Int<SceneConfigReader>(TEXT("SceneStyle"), dataRow, rowIdx);

	obj->ClientSceneVariableType = TReadDataValue_Int<SceneConfigReader>(TEXT("ClientSceneVariableType"), dataRow, rowIdx);
	obj->MapDescript = TReadDataValue_FString<SceneConfigReader>(TEXT("MapDescript"), dataRow, rowIdx);
	obj->MinimapName = TReadDataValue_FString<SceneConfigReader>(TEXT("MinimapName"), dataRow, rowIdx);
// 	obj->ImportConfigName = TReadDataValue_FString<SceneConfigReader>(TEXT("ConfigName"), dataRow, rowIdx);
// 	obj->ImportMapName = TReadDataValue_FString<SceneConfigReader>(TEXT("MapName"), dataRow, rowIdx);
	obj->ImportServerMapInfos.ServerMapID = obj->ServerID;
	obj->ImportServerMapInfos.ServerMapType = obj->ServerMapType;
	obj->ImportServerMapInfos.ConfigName = TReadDataValue_FString<SceneConfigReader>(TEXT("ConfigName"), dataRow, rowIdx);
	obj->ImportServerMapInfos.ConfigName = TReadDataValue_FString<SceneConfigReader>(TEXT("MapName"), dataRow, rowIdx);
	FString tmp = TReadDataValue_FString<SceneConfigReader>(TEXT("Map2DExportSize"), dataRow, rowIdx);
	obj->ImportServerMapInfos.Map2DExportSize.InitFromString(tmp);

	obj->BattleType = TReadDataValue_Int<SceneConfigReader>(TEXT("BattleType"), dataRow, rowIdx);
	obj->SideCount = TReadDataValue_Int<SceneConfigReader>(TEXT("BattleSideCount"), dataRow, rowIdx);
	
	obj->MinLimitPerSide = TReadDataValue_Int<SceneConfigReader>(TEXT("MinLimitPerSide"), dataRow, rowIdx);
	obj->MaxLimitPerSide = TReadDataValue_Int<SceneConfigReader>(TEXT("MaxLimitPerSide"), dataRow, rowIdx);

	obj->InitLineCount = TReadDataValue_Int<SceneConfigReader>(TEXT("InitLineCount"), dataRow, rowIdx);
	obj->MaxLineCount = TReadDataValue_Int<SceneConfigReader>(TEXT("MaxLineCount"), dataRow, rowIdx);

	obj->MinPlayerLevel = TReadDataValue_Int<SceneConfigReader>(TEXT("MinPlayerLevel"), dataRow, rowIdx);
	obj->MaxPlayerLevel = TReadDataValue_Int<SceneConfigReader>(TEXT("MaxPlayerLevel"), dataRow, rowIdx);
	obj->MinPortalLevel = TReadDataValue_Int<SceneConfigReader>(TEXT("MinPortalLevel"), dataRow, rowIdx);
	obj->MaxPortalLevel = TReadDataValue_Int<SceneConfigReader>(TEXT("MaxPortalLevel"), dataRow, rowIdx);

	obj->Script = TReadDataValue_FString<SceneConfigReader>(TEXT("Script"), dataRow, rowIdx);
	// FinishTarget changed to Id, because NPCTemplate causes the client to preload lots of resources in game mode, modified by zhangrui05
	//obj->FinishTarget = TReadDataValue_TemplateIDObject<UNPCTemplate, SceneConfigReader, TemplateObjectFinder_NPCTemplate>(TEXT("FinishTarget"), dataRow, rowIdx);
	obj->FinishTarget = TReadDataValue_Int<SceneConfigReader>(TEXT("FinishTarget"), dataRow, rowIdx);

	//obj->bPK = TReadDataValue_Int<SceneConfigReader>(TEXT("bPK"), dataRow, rowIdx);
	//obj->bEnablePKPunishment = TReadDataValue_Int<SceneConfigReader>(TEXT("bEnablePKPunishment"), dataRow, rowIdx);
	//obj->bReducedDurability = TReadDataValue_Int<SceneConfigReader>(TEXT("bReducedDurability"), dataRow, rowIdx);
	//obj->DropItemDurability = TReadDataValue_Int<SceneConfigReader>(TEXT("DropItemDurability"), dataRow, rowIdx);

	//whether the town portal scroll can be used
	obj->UseScroolDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotUseScroll"), dataRow, rowIdx);
	//whether teleport to specified coordinates can be used
	obj->UseTeleportRingDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotUseTeleportRing"), dataRow, rowIdx);
	//whether players can be (pulled/pulled by) others
	obj->BeRecalledDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotBeRecalled"), dataRow, rowIdx);
	//chat restrictions
	obj->ChatGroupDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotGroupChat"), dataRow, rowIdx);
	obj->ChatSceneDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotSceneChat"), dataRow, rowIdx);
	obj->ChatScreenDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotScreenChat"), dataRow, rowIdx);
	obj->ChatWhisperDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotWhisperChat"), dataRow, rowIdx);
	obj->ChatTeamDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotTeamChat"), dataRow, rowIdx);
	obj->ChatGuildDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotGuildChat"), dataRow, rowIdx);
	obj->ChatFriendDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotFriendChat"), dataRow, rowIdx);
	//whether trading is allowed
	obj->TradeDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotTrade"), dataRow, rowIdx);
	//whether dropping items is allowed
	//obj->ItemDropDisabled = TReadDataValue_Int<SceneConfigReader>(TEXT("CannotDrop"), dataRow, rowIdx);
	obj->EnableAntiCheat = TReadDataValue_Int<SceneConfigReader>(TEXT("EnableAntiCheat"), dataRow, rowIdx);
	obj->GpkPunish = TReadDataValue_Int<SceneConfigReader>(TEXT("GpkPunish"), dataRow, rowIdx);
	obj->GpkKick = TReadDataValue_Int<SceneConfigReader>(TEXT("GpkKick"), dataRow, rowIdx);
	//
	obj->EnableIntimacy = TReadDataValue_Int<SceneConfigReader>(TEXT("EnableIntimacy"), dataRow, rowIdx);
	//
	obj->LoginJumpSceneTempID = TReadDataValue_Int<SceneConfigReader>(TEXT("LoginJumpSceneTempID"), dataRow, rowIdx);
	obj->LoginJumpX = TReadDataValue_Int<SceneConfigReader>(TEXT("LoginJumpX"), dataRow, rowIdx);
	obj->LoginJumpY = TReadDataValue_Int<SceneConfigReader>(TEXT("LoginJumpY"), dataRow, rowIdx);
	obj->LoginJumpZ = TReadDataValue_Int<SceneConfigReader>(TEXT("LoginJumpZ"), dataRow, rowIdx);
	//
	obj->MaxRoleCount = TReadDataValue_Int<SceneConfigReader>(TEXT("MaxRoleCount"), dataRow, rowIdx);
	obj->BlockRole = (UBOOL)TReadDataValue_Int<SceneConfigReader>(TEXT("BlockRole"), dataRow, rowIdx);
	
	obj->MineItemID1 = TReadDataValue_Int<SceneConfigReader>(TEXT("MineItemID1"), dataRow, rowIdx);
	obj->MineItemID2 = TReadDataValue_Int<SceneConfigReader>(TEXT("MineItemID2"), dataRow, rowIdx);
	obj->MineItemID3 = TReadDataValue_Int<SceneConfigReader>(TEXT("MineItemID3"), dataRow, rowIdx);
	obj->MineItemID4 = TReadDataValue_Int<SceneConfigReader>(TEXT("MineItemID4"), dataRow, rowIdx);

	obj->MineDoodadID1 = TReadDataValue_Int<SceneConfigReader>(TEXT("MineDoodadID1"), dataRow, rowIdx);
	obj->MineDoodadChance1 = TReadDataValue_Int<SceneConfigReader>(TEXT("MineDoodadChance1"), dataRow, rowIdx);

	obj->MineDoodadID2 = TReadDataValue_Int<SceneConfigReader>(TEXT("MineDoodadID2"), dataRow, rowIdx);
	obj->MineDoodadChance2 = TReadDataValue_Int<SceneConfigReader>(TEXT("MineDoodadChance2"), dataRow, rowIdx);

	for(INT j = 0; j < UCONST_MaxSceneMineDrop; ++j)
	{
		obj->SceneMineDrop.AddZeroed();

		obj->SceneMineDrop(j).Chance = TReadDataValue_Int<SceneConfigReader>(FString::Printf(TEXT("MineDropChance%d%d"), j/8+1, j%8), dataRow, rowIdx);
		obj->SceneMineDrop(j).DropGroup = TReadDataValue_Int<SceneConfigReader>(FString::Printf(TEXT("MineDropGroup%d%d"), j/8+1, j%8), dataRow, rowIdx);
	}

	obj->CannotSendMail = (UBOOL)TReadDataValue_Int<SceneConfigReader>(TEXT("CannotSendMail"), dataRow, rowIdx);
	obj->CannotUseExchange = (UBOOL)TReadDataValue_Int<SceneConfigReader>(TEXT("CannotUseExchange"), dataRow, rowIdx);
	obj->CannotUseAuction = (UBOOL)TReadDataValue_Int<SceneConfigReader>(TEXT("CannotUseAuction"), dataRow, rowIdx);
	obj->CannotJoinTeam = (UBOOL)TReadDataValue_Int<SceneConfigReader>(TEXT("CannotJoinTeam"), dataRow, rowIdx);

	static UEnum* weatherEnum = FindObject<UEnum>(UServerEnum::StaticClass(), TEXT("ESCENE_WEATHER_TYPE"), TRUE);
	for(INT j = 0; j < 4; ++j)
	{
		obj->WeatherTypes.AddZeroed();

		FString weatherTypeStr = TEXT("E_swt_");
		FString readWeatherTypeStr = TReadDataValue_FString<SceneConfigReader>(FString::Printf(TEXT("Weather%d"), j + 1), dataRow, rowIdx);
		weatherTypeStr += readWeatherTypeStr.Right( readWeatherTypeStr.Len() - 3 );
		obj->WeatherTypes(j).WeatherType = weatherEnum->FindEnumIndex( *weatherTypeStr );

		obj->WeatherTypes(j).WeatherWeight = TReadDataValue_Float<SceneConfigReader>(FString::Printf(TEXT("Weather%dWeight"), j + 1), dataRow, rowIdx);
		obj->WeatherTypes(j).MinDuration = TReadDataValue_Float<SceneConfigReader>(FString::Printf(TEXT("Weather%dMinDuration"), j + 1), dataRow, rowIdx);
		obj->WeatherTypes(j).MaxDuration = TReadDataValue_Float<SceneConfigReader>(FString::Printf(TEXT("Weather%dMaxDuration"), j + 1), dataRow, rowIdx);
	}

	obj->MinWeatherRefreshInterval = TReadDataValue_Float<SceneConfigReader>(TEXT("MinWeatherRefreshInterval"), dataRow, rowIdx);
	obj->MaxWeatherRefreshInterval = TReadDataValue_Float<SceneConfigReader>(TEXT("MaxWeatherRefreshInterval"), dataRow, rowIdx);

	obj->Progress.AddZeroed(UCONST_MaxSceneMajorTarget);
	for (INT k = 0; k < UCONST_MaxSceneMajorTarget; ++k)
	{
		obj->Progress(k).MainTarget.Desc = TReadDataValue_FString<SceneConfigReader>(FString::Printf(TEXT("Progress%dMainTarget_Desc"), k + 1), dataRow, rowIdx);
		obj->Progress(k).MainTarget.ID = TReadDataValue_Int<SceneConfigReader>(FString::Printf(TEXT("Progress%dMainTarget_ID"), k + 1), dataRow, rowIdx);
		obj->Progress(k).MainTarget.TargetValue = TReadDataValue_Int<SceneConfigReader>(FString::Printf(TEXT("Progress%dMainTarget_TargetValue"), k + 1), dataRow, rowIdx);

		for (INT m = 0; m < UCONST_MaxSceneMinorTarget; ++m)
		{
			obj->Progress(k).ChildTargets[m].Desc = TReadDataValue_FString<SceneConfigReader>(FString::Printf(TEXT("Progress%dChildTargets%d_Desc"), k + 1, m + 1), dataRow, rowIdx);
			obj->Progress(k).ChildTargets[m].ID = TReadDataValue_Int<SceneConfigReader>(FString::Printf(TEXT("Progress%dChildTargets%d_ID"), k + 1, m + 1), dataRow, rowIdx);
			obj->Progress(k).ChildTargets[m].TargetValue = TReadDataValue_Int<SceneConfigReader>(FString::Printf(TEXT("Progress%dChildTargets%d_TargetValue"), k + 1, m + 1), dataRow, rowIdx);
		}
	}
}

void SceneConfigDataExcelImporter::PostInitData()
{
	SceneConfigReader& itemMgr = SceneConfigReader::getSingleton();
	SceneConfigReader::DataTypeMap::const_iterator it = itemMgr._dataMap.begin();
	for (; it != itemMgr._dataMap.end(); ++it)
	{
		const FExportMap* srcDat = it->second;
		FExportMap* expMap = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer(srcDat->ServerID);
		warnf(NAME_Error, TEXT("====== [UGlobalMapInfo::LOAD_SCENE[%d %s]") ,srcDat->ServerID, *srcDat->MapName);
		if(expMap)
		{
			*expMap = *srcDat;

			FServerMapInfos* smi = UGlobalMapInfo::GetSingleton()->FindServerMapInfPointer(srcDat->ServerID);
			if(smi)
			{
				*smi = srcDat->ImportServerMapInfos;
				//smi->ServerMapType = srcDat->ServerMapType;
				//smi->ConfigName = srcDat->ImportConfigName;
				//smi->MapName = srcDat->ImportMapName;
			}
		}
		else
		{
			warnf(NAME_Error, TEXT("====== [UGlobalMapInfo::LOAD_SCENE[%d]") ,srcDat->ServerID);
			UGlobalMapInfo::GetSingleton()->ExportMaps.AddItem(*srcDat);
// 			FServerMapInfos smi;
// 			smi.ConfigName = srcDat->ImportConfigName;
// 			smi.MapName = srcDat->ImportMapName;
// 			smi.ServerMapID = srcDat->ServerID;
// 			smi.ServerMapType = srcDat->ServerMapType;
			UGlobalMapInfo::GetSingleton()->ServerMapInfoList.AddItem(srcDat->ImportServerMapInfos);
		}
	}
}

struct DungeonConfigDataExcelImporter
{
	static const TCHAR* GetName()
	{
		return TEXT("dungeon.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static FDungeonInfo* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		FDungeonInfo* item = new FDungeonInfo;
		item->DungeonInfoID = templateID;
		return item;
	}

	static void DestroyDataObject(FDungeonInfo* obj)
	{
		delete obj;
	}

	static void ParseData(FDungeonInfo* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("ID_Difficulty");
	}

	static FString FormatAsString(FDungeonInfo* obj)
	{
		return FString::Printf(TEXT("DungeonInfo[%d]"), obj->DungeonInfoID);
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(FDungeonInfo* obj, TSet<FString>& outResList)
	{
		//_CollectReferencedResource(obj, outResList);
	}
#endif
};

typedef TDataManger<FDungeonInfo, DungeonConfigDataExcelImporter> DungeonConfigReader;

void DungeonConfigDataExcelImporter::ParseData(FDungeonInfo* obj, class CTabFile* dataRow, int rowIdx)
{
	// INT dungeonInfoID = dungeonExportInfo.MapID * 10000 + dungeonExportInfo.Difficulty;
	obj->DungeonInfoID = TReadDataValue_Int<DungeonConfigReader>(TEXT("ID_Difficulty"), dataRow, rowIdx);

	obj->MapID = obj->DungeonInfoID;// / 10000;
	obj->Difficulty = 0;//obj->DungeonInfoID - obj->MapID * 10000;

	obj->InitState = TReadDataValue_Int<DungeonConfigReader>(TEXT("InitState"), dataRow, rowIdx);
	obj->Type = TReadDataValue_Int<DungeonConfigReader>(TEXT("Type"), dataRow, rowIdx);

	obj->MinLevel = TReadDataValue_Int<DungeonConfigReader>(TEXT("MinLevel"), dataRow, rowIdx);
	obj->MaxLevel = TReadDataValue_Int<DungeonConfigReader>(TEXT("MaxLevel"), dataRow, rowIdx);
	obj->RoleLimit = TReadDataValue_Int<DungeonConfigReader>(TEXT("RoleLimit"), dataRow, rowIdx);
	obj->MaxEnterCount = TReadDataValue_Int<DungeonConfigReader>(TEXT("MaxEnterCount"), dataRow, rowIdx);

	obj->StandardTime = TReadDataValue_Int<DungeonConfigReader>(TEXT("StandardTime"), dataRow, rowIdx);
	obj->StandardNpcValue = TReadDataValue_Int<DungeonConfigReader>(TEXT("StandardNpcValue"), dataRow, rowIdx);
	obj->StandardDoodadValue = TReadDataValue_Int<DungeonConfigReader>(TEXT("StandardDoodadValue"), dataRow, rowIdx);
	obj->StandardAttackValue = TReadDataValue_Int<DungeonConfigReader>(TEXT("StandardAttackValue"), dataRow, rowIdx);

	obj->ExpForVip1 = TReadDataValue_Int<DungeonConfigReader>(TEXT("Exp_vip1"), dataRow, rowIdx);
	obj->ExpForVip2 = TReadDataValue_Int<DungeonConfigReader>(TEXT("Exp_vip2"), dataRow, rowIdx);
	obj->ExpForVip3 = TReadDataValue_Int<DungeonConfigReader>(TEXT("Exp_vip3"), dataRow, rowIdx);
	
	obj->ExpForTeam = TReadDataValue_Int<DungeonConfigReader>(TEXT("Exp_team"), dataRow, rowIdx);
	obj->ExpForGuild = TReadDataValue_Int<DungeonConfigReader>(TEXT("Exp_guild"), dataRow, rowIdx);
	obj->ExpForFirst = TReadDataValue_Int<DungeonConfigReader>(TEXT("Exp_first"), dataRow, rowIdx);
	obj->ExpForActivity = TReadDataValue_Int<DungeonConfigReader>(TEXT("Exp_activity"), dataRow, rowIdx);

	obj->Reward1_groupid = TReadDataValue_Int<DungeonConfigReader>(TEXT("Reward1_group"), dataRow, rowIdx);
	obj->Reward2_groupid = TReadDataValue_Int<DungeonConfigReader>(TEXT("Reward2_group"), dataRow, rowIdx);
	obj->Reward3_groupid = TReadDataValue_Int<DungeonConfigReader>(TEXT("Reward3_group"), dataRow, rowIdx);

	obj->Reward1_free = TReadDataValue_Int<DungeonConfigReader>(TEXT("Reward1_free"), dataRow, rowIdx);
	obj->Reward2_free = TReadDataValue_Int<DungeonConfigReader>(TEXT("Reward2_free"), dataRow, rowIdx);
	obj->Reward3_free = TReadDataValue_Int<DungeonConfigReader>(TEXT("Reward3_free"), dataRow, rowIdx);

	obj->Reward1_cost = TReadDataValue_Int<DungeonConfigReader>(TEXT("Reward1_cost"), dataRow, rowIdx);
	obj->Reward2_cost = TReadDataValue_Int<DungeonConfigReader>(TEXT("Reward2_cost"), dataRow, rowIdx);
	obj->Reward3_cost = TReadDataValue_Int<DungeonConfigReader>(TEXT("Reward3_cost"), dataRow, rowIdx);

	obj->ItemIsAll = TReadDataValue_Int<DungeonConfigReader>(TEXT("ItemIsAll"), dataRow, rowIdx);
	obj->ItemTemplateID = TReadDataValue_Int<DungeonConfigReader>(TEXT("ItemID"), dataRow, rowIdx);
	obj->ItemNumber = TReadDataValue_Int<DungeonConfigReader>(TEXT("ItemNumber"), dataRow, rowIdx);
	obj->Fatigue = TReadDataValue_Int<DungeonConfigReader>(TEXT("Fatigue"), dataRow, rowIdx);

	obj->StandardBossValue = TReadDataValue_Int<DungeonConfigReader>(TEXT("StandardBossValue"), dataRow, rowIdx);
	obj->StandardEliteNpcValue = TReadDataValue_Int<DungeonConfigReader>(TEXT("StandardEliteNpcValue"), dataRow, rowIdx);
	obj->StandardSwiftTime = TReadDataValue_Int<DungeonConfigReader>(TEXT("StandardSwiftTime"), dataRow, rowIdx);
	//obj->UndeadValue = TReadDataValue_Int<DungeonConfigReader>(TEXT("UndeadValue"), dataRow, rowIdx);

	for(INT j = 0; j < UCONST_MaxDungeonCustomTarget; ++j)
	{
		//obj->CustomTarget[j].TargetName = TReadDataValue_FString<DungeonConfigReader>(FString::Printf(TEXT("TargetName%d"), j + 1), dataRow, rowIdx);
		//obj->CustomTarget[j].TargetDesc = TReadDataValue_FString<DungeonConfigReader>(FString::Printf(TEXT("TargetDesc%d"), j + 1), dataRow, rowIdx);
		obj->CustomTarget[j].ID = TReadDataValue_Int<DungeonConfigReader>(FString::Printf(TEXT("CustomGoalVarID%d"), j + 1), dataRow, rowIdx);
		obj->CustomTarget[j].TargetValue = TReadDataValue_Int<DungeonConfigReader>(FString::Printf(TEXT("CustomGoalVarValue%d"), j + 1), dataRow, rowIdx);
		obj->CustomTarget[j].TargetLevel = TReadDataValue_Int<DungeonConfigReader>(FString::Printf(TEXT("CustomGoalScore%d"), j + 1), dataRow, rowIdx);
	}

	for(INT j = 0; j < UCONST_MaxDungeonSpecialReward; ++j)
	{
		obj->DungeonSpecialRewardsID[j] = TReadDataValue_Int<DungeonConfigReader>(FString::Printf(TEXT("DungeonSpecialRewardsID%d"), j + 1), dataRow, rowIdx);
	}
}

void DungeonConfigDataExcelImporter::PostInitData()
{
	DungeonConfigReader& itemMgr = DungeonConfigReader::getSingleton();
	DungeonConfigReader::DataTypeMap::const_iterator it = itemMgr._dataMap.begin();
	for (; it != itemMgr._dataMap.end(); ++it)
	{
		const FDungeonInfo* srcDat = it->second;

		// INT dungeonInfoID = dungeonExportInfo.MapID * 10000 + dungeonExportInfo.Difficulty;
		//INT mapID = srcDat->DungeonInfoID / 10000;
		//INT diffLv = srcDat->DungeonInfoID - mapID * 10000;

		TArrayNoInit<struct FDungeonInfo>& exportDungeonInfo = UGlobalMapInfo::GetSingleton()->ExportDungeonInfo;
		bool found = false;
		for (INT i = 0; i < exportDungeonInfo.Num(); ++i)
		{
			FDungeonInfo* destDat = &exportDungeonInfo(i);
			if(destDat && (destDat->DungeonInfoID == srcDat->DungeonInfoID))
			{
				*destDat = *srcDat;
				found = true;
				break;
			}
		}

		if(!found)
		{
			exportDungeonInfo.AddItem(*srcDat);
		}
	}
}

extern void _LoadNPCTemplates();

void UGlobalMapInfo::ImportFromExcel(TArray< TArray<FString> >& sceneDat,
	TArray< TArray<FString> >& dungeonDat)
{	
	_LoadNPCTemplates();
	
#if 0 // TABFILE

	SceneConfigReader::getSingleton().InitFromData(sceneDat);

	DungeonConfigReader::getSingleton().InitFromData(dungeonDat);
#endif
}

void UGlobalMapInfo::ImportFromText()
{
	_LoadNPCTemplates();
	
	SceneConfigReader::getSingleton().InitFromFile(SceneConfigDataExcelImporter::GetName());
	DungeonConfigReader::getSingleton().InitFromFile(DungeonConfigDataExcelImporter::GetName());
}

#endif // SHIPPING_PC_GAME