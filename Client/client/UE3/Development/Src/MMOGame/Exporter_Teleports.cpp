#include "stdafx.h"
#include "Exporter_Teleports.h"
#include <sstream>
#include <fstream>
#include <ios>
#include "SourceControl.h"
#include "MMOFacade.h"


#if !SHIPPING_PC_GAME

Exporter_Teleports::Exporter_Teleports(void)
{
}
//------------------------------------------------------------------------
Exporter_Teleports::~Exporter_Teleports(void)
{
}
//------------------------------------------------------------------------
void Exporter_Teleports::ProcessData(INT& GlobalAutoID)
{
	INT autoOriID = 1;
	INT autoTgtID = 1;
	//INT autoNodeID = 0;

	// Auto-number first
// 	for(TObjectIterator<ATeleportOrig> it; it; ++it)
// 	{		
// 		(*it)->AutoServerID = autoID;
// 		++autoID;
// 	}


//#if HAVE_SCC

	UWorld*	World = GWorld;
	//static_cast<UWorld*>( UObject::StaticFindObject(UWorld::StaticClass(), PackageToSave, TEXT("TheWorld")) );
	FString ExportDir;
	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"), ExportDir, GGameIni);
	
	ExportDir = MMO::FormatTempleteDataLoadingDir(ExportDir);

	FString mapName = World->GetWorldInfo()->GetMapName() + TEXT(".teleport");
	FString mapPath = ExportDir + mapName;

	const INT C_TMPBUF_SZ = 2048;
	wchar_t tmpBuf[C_TMPBUF_SZ];

	std::wstringstream ss;

	ss << TEXT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	FVector2D MapSize(100, 50);
	INT ServerMapType = E_est_Static;
	UMMOMapInfo* mapInfo = Cast<UMMOMapInfo>(World->GetWorldInfo()->GetMapInfo());
	if(mapInfo)
	{
		MapSize = mapInfo->Map2DExportSize;
		//ServerMapType = mapInfo->ServerMapType;
	}

	swprintf_s(tmpBuf, C_TMPBUF_SZ - 1,
		TEXT("<root CanvasDrawSize=\"%s\" ServerMapType=\"%d\">\n"),
		*MapSize.ToString(), ServerMapType);

	ss << tmpBuf;

	TArray<ATeleportNetNode*> teleportNetNodesList;
	for( int i = 0; i < World->Levels.Num(); ++i )
	{
		ULevel* Level = World->Levels(i);
		for (INT j = 0; j < Level->Actors.Num(); ++j)
		{
			AActor* actor = Level->Actors(j);

			if(actor && (actor->GetClass() == ATeleportOrig::StaticClass()))
			{
				ATeleportOrig* teleport = Cast<ATeleportOrig>(actor);
                teleport->AutoServerID = autoOriID++;

				memset(tmpBuf, 0, sizeof(wchar_t) * C_TMPBUF_SZ);

				swprintf_s(tmpBuf, C_TMPBUF_SZ - 1,
					TEXT("\t\t<TeleportOrig ServerID=\"%d\" Name=\"%s\" Position=\"%s\" Radius=\"%d\" DisplayName=\"%s\"/>\n"),
					teleport->AutoServerID, *teleport->GetName(),
					*teleport->Location.ToString(), teleport->TeleportClientRange,
					*teleport->ToSceneName);

				ss << tmpBuf;
			}
			else if(actor && (actor->GetClass() == ATeleportTarget::StaticClass()))
			{
				ATeleportTarget* teleport = Cast<ATeleportTarget>(actor);
				////junjie@snda
				//teleport->AutoClientID = autoTgtID++;
				//FVector pos = teleport->GetServerPosition();
				memset(tmpBuf, 0, sizeof(wchar_t) * C_TMPBUF_SZ);

				const INT C_MAXClientRot = 65536;
				INT yaw = teleport->Rotation.Yaw;
				while (yaw < 0)
				{
					yaw += C_MAXClientRot;
				}

				yaw %= C_MAXClientRot;

				swprintf_s(tmpBuf, C_TMPBUF_SZ - 1,
					TEXT("\t\t<TeleportTarget Name=\"%s\" Position=\"%s\" DisplayName=\"%s\" Radius=\"%f\" Rotation=\"%d\" NpcId=\"%d\"/>\n"),
					*teleport->GetName(), *teleport->Location.ToString(),
					*teleport->DisplayName, teleport->ServerRangeRadius, yaw,
					teleport->NpcId);

				////junjie@snda
				//swprintf_s(tmpBuf, C_TMPBUF_SZ - 1,
				//	TEXT("\t\t<TeleportTarget ServerID=\"%d\" Name=\"%s\" Position=\"%s\" DisplayName=\"%s\" Radius=\"%f\" Rotation=\"%d\"/>\n"),
				//	teleport->AutoClientID, *teleport->GetName(), *teleport->Location.ToString(),
				//	*teleport->DisplayName, teleport->ServerRangeRadius, yaw);

				ss << tmpBuf;
			}
			//Teleport station node
			else if(actor && (actor->GetClass() == ATeleportNetNode::StaticClass()))
			{
				ATeleportNetNode* teleport = Cast<ATeleportNetNode>(actor);
				//sort from low to high
				INT curCnt = teleportNetNodesList.Num();
				INT idx = 0;
				for( ; idx < curCnt; ++idx ){
					if( teleport->ShowPriority <= teleportNetNodesList(idx)->ShowPriority ){
						teleportNetNodesList.InsertItem( teleport, idx );
						break;
					}
				}

				if( idx >= curCnt ){
					//highest now
					teleportNetNodesList.Push( teleport );
				}
			}
			// if
		} // for
	}
	//wirte teleport net nodes to file from high to low
	for( INT i = teleportNetNodesList.Num() - 1; i >= 0; --i ){
		ATeleportNetNode* teleport = teleportNetNodesList(i);
		//teleport->AutoClientID = autoNodeID++;

		memset(tmpBuf, 0, sizeof(wchar_t) * C_TMPBUF_SZ);

		const INT C_MAXClientRot = 65536;
		INT yaw = teleport->Rotation.Yaw;
		while (yaw < 0){
			yaw += C_MAXClientRot;
		}
		yaw %= C_MAXClientRot;

		swprintf_s( tmpBuf, C_TMPBUF_SZ - 1,
			//TEXT("\t\t<TeleportNetNode Name=\"%s\" DisplayName=\"%s\" State=\"%d\" InSceneID=\"%d\" RegionID=\"%d\" ShowPriority=\"%d\" Position=\"%s\" Radius=\"%d\" Rotation=\"%d\"/>\n"),
			TEXT("\t\t<TeleportNetNode Name=\"%s\" PicResName=\"%s\" DisplayName=\"%s\" State=\"%d\" InSceneID=\"%d\" RegionID=\"%d\" Position=\"%s\" Radius=\"%d\" Rotation=\"%d\"/>\n"),
			*teleport->GetName(), *teleport->PicResName, *teleport->DisplayName, teleport->InitState, teleport->InSceneID, teleport->RegionID, /*teleport->ShowPriority,*/
			*teleport->Location.ToString(), teleport->TeleportClientRange, yaw );

		ss << tmpBuf;
	}

	ss << TEXT("</root>\n");


	SET_WARN_COLOR(COLOR_GREEN);
	warnf(TEXT("=====[Export]===== Exported server teleport file:%s"), *mapPath);
	CLEAR_WARN_COLOR();


	//FScopedSourceControl sccObj;
#if HAVE_SCC
	FCheckoutResult sccResult = FSourceControl::TryCheckOut(mapPath);
#endif

	std::string str(TCHAR_TO_ANSI(*mapPath));
	std::wfstream fs(str.c_str(), std::ios_base::out);
	if(fs.good())
	{
		fs << TCHAR_TO_UTF8(ss.str().c_str());
		fs.close();
	}

#if HAVE_SCC
	sccResult.TryAddToDefaultChangelist();
#endif
//#endif
}
//------------------------------------------------------------------------
#endif