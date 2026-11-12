#include "stdafx.h"
#include "Exporter_SearchPoint.h"
#include <sstream>
#include <fstream>
#include <ios>
#include "SourceControl.h"
#include "MMOFacade.h"

#if !SHIPPING_PC_GAME

Exporter_SearchPoint::Exporter_SearchPoint(void)
{
}
//------------------------------------------------------------------------
Exporter_SearchPoint::~Exporter_SearchPoint(void)
{
}
//------------------------------------------------------------------------
void Exporter_SearchPoint::ProcessData(INT& GlobalAutoID)
{
	INT autoOriID = 1;
	//INT autoTgtID = 1;

	// Auto-number first
// 	for(TObjectIterator<ASearchPoint> it; it; ++it)
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

	FString mapName = World->GetWorldInfo()->GetMapName() + TEXT(".search");
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

	for( int i = 0; i < World->Levels.Num(); ++i )
	{
		ULevel* Level = World->Levels(i);
		for (INT j = 0; j < Level->Actors.Num(); ++j)
		{
			AActor* actor = Level->Actors(j);

			if(actor && (actor->GetClass() == ASearchPoint::StaticClass()))
			{
				ASearchPoint* SearchPoint = Cast<ASearchPoint>(actor);
                //SearchPoint->SearchPointID = autoOriID++;

				memset(tmpBuf, 0, sizeof(wchar_t) * C_TMPBUF_SZ);

				swprintf_s(tmpBuf, C_TMPBUF_SZ - 1,
					TEXT("\t\t<SearchPoint SearchPointID=\"%d\" SearchPointName=\"%s\" />\n"),
					SearchPoint->SearchPointID,
					*SearchPoint->Script);
				ss << tmpBuf;
			}
		} 
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
}
//------------------------------------------------------------------------
#endif