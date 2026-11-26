#include "stdafx.h"
#include "Exporter_TriggerVolume.h"
#include "scene_def.h"

#if !SHIPPING_PC_GAME

Exporter_TriggerVolume::Exporter_TriggerVolume(void)
{
}

Exporter_TriggerVolume::~Exporter_TriggerVolume(void)
{
}

void Exporter_TriggerVolume::ProcessData(INT& GlobalAutoID)
{
	UWorld*	World = GWorld;
	FString ExportDir;
	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"), ExportDir, GGameIni);

	ExportDir = MMO::FormatTempleteDataLoadingDir(ExportDir);

	FString mapName = World->GetWorldInfo()->GetMapName() + TEXT(".trigvolume");
	FString mapPath = ExportDir + mapName;

	std::wstringstream ss;
	ss << TEXT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	const INT C_TMPBUF_SZ = 2048;
	wchar_t buffer[C_TMPBUF_SZ];
	INT mapType = E_est_Static;
	swprintf_s(buffer, C_TMPBUF_SZ - 1, TEXT("<root ServerMapName=\"%s\" ServerMapType=\"%d\">\n"), *(World->GetWorldInfo()->GetMapName()), mapType);
	ss << buffer;

	for (TObjectIterator<AMMOVolume_Trigger> iter; iter; ++ iter)
	{
		AMMOVolume_Trigger* volume = *iter;
		if (volume && volume->VolumeType == EVVT_QuestZone)
		{
			if (volume->Quests.Num() > 0 && volume->Quests.Num() <= MAX_QUEST_PER_ZONE)
			{
				memset(buffer, 0, sizeof(wchar_t) * C_TMPBUF_SZ);
				INT questIDs[4] = {0};
				for (INT questIndex = 0; questIndex < ARRAY_COUNT(questIDs) && questIndex < volume->Quests.Num(); ++questIndex)
					questIDs[questIndex] = volume->Quests(questIndex);

				swprintf_s(buffer, C_TMPBUF_SZ - 1,
					TEXT("\t<TriggerVolume Name=\"%s\" Position=\"%s\" QuestId0=\"%d\" QuestId1=\"%d\" QuestId2=\"%d\" QuestId3=\"%d\"/>\n"),
					*(volume->GetName()),
					*(volume->Location.ToString()),
					questIDs[0],
					questIDs[1],
					questIDs[2],
					questIDs[3]);

				ss << buffer;
			}
		}
	}

	ss << TEXT("</root>\n");

	FCheckoutResult result = FSourceControl::TryCheckOut(mapPath);
	std::string mapPathTemp(TCHAR_TO_ANSI(*mapPath));
	std::wfstream fs(mapPathTemp.c_str(), std::ios_base::out);
	if (fs.good())
	{
		fs << TCHAR_TO_UTF8(ss.str().c_str());
		fs.close();
	}
	result.TryAddToDefaultChangelist();
}

#endif