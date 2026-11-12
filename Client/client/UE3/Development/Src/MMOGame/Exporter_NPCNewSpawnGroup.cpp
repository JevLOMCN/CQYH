#include "stdafx.h"
#include "Exporter_NPCNewSpawnGroup.h"
#include "MMOGameClasses.h"
#include "net\message_handler.h"

#if !SHIPPING_PC_GAME

Exporter_NPCNewSpawnGroup::Exporter_NPCNewSpawnGroup(void)
	: SpawnGroupHeader(0)
	, CurrentMedGroup(0)
	, CurrentMedGrpIdx(0)
	, CurrentSmallGrpIdx(0)
	, CurrentLargeGroup(0)
{

}
//------------------------------------------------------------------------
Exporter_NPCNewSpawnGroup::~Exporter_NPCNewSpawnGroup(void)
{
}
//------------------------------------------------------------------------
void Exporter_NPCNewSpawnGroup::PrePorcess(INT& GlobalAutoID)
{
	TArray<ANPCNewSpawnGroupActor*> allSpawnGroups;

	for(TObjectIterator<ANPCNewSpawnGroupActor> It; It; ++It)
	{
		ANPCNewSpawnGroupActor* spawnGrp = *It;
		allSpawnGroups.AddItem(spawnGrp);
		RemoveNullRef(spawnGrp);
	}

	for(INT i = 0; i < allSpawnGroups.Num();)
	{
		ANPCNewSpawnGroupActor* spawnGrp = Cast<ANPCNewSpawnGroupActor>(allSpawnGroups(i));		
		if(spawnGrp && spawnGrp->MediumGroupList.Num() == 0)
		{
			allSpawnGroups.Remove(i);
		}
		else
		{
			 ++i;
		}
	}

	NPC_SPAWN_DATA_HEADER spawnDataHdr;
	spawnDataHdr.nLargeGroupCount = allSpawnGroups.Num();
	MemAr.ByteOrderSerialize(&spawnDataHdr, sizeof(NPC_SPAWN_DATA_HEADER));

	// large group loop
	for(INT i = 0; i < allSpawnGroups.Num(); ++i)
	{
		ANPCNewSpawnGroupActor* spawnGrp = Cast<ANPCNewSpawnGroupActor>(allSpawnGroups(i));
		if(!spawnGrp)
		{
			continue;
		}

		CurrentLargeGroup = spawnGrp;
		NPC_SPAWN_LARGE_GROUP largeGrp;
		
		largeGrp.nChampionInterval = UTypeDefine::ConvertToMS(spawnGrp->ChampionInterval);
		largeGrp.nChampionMax = spawnGrp->ChampionMax;
		largeGrp.nChampionMin = spawnGrp->ChampionMin;

		largeGrp.nEliteInterval = UTypeDefine::ConvertToMS(spawnGrp->EliteInterval);
		largeGrp.nEliteMax = spawnGrp->EliteMax;
		largeGrp.nEliteMin = spawnGrp->EliteMin;
		largeGrp.nMaxDieCount = spawnGrp->MaxDieCount;

		strxcpy(largeGrp.szDeadScript, *spawnGrp->DeadTree, 31);
		largeGrp.nMediumGroupCount = spawnGrp->MediumGroupList.Num();
		largeGrp.nID = spawnGrp->ServerUniqueID;

		MemAr.ByteOrderSerialize(&largeGrp, sizeof(NPC_SPAWN_LARGE_GROUP));

		// medium group loop
		for (INT j = 0; j < spawnGrp->MediumGroupList.Num(); ++j)
		{
			ANPCNewSpawnGroupMediumActor* mGrp = Cast<ANPCNewSpawnGroupMediumActor>(spawnGrp->MediumGroupList(j));
			if(mGrp)
			{
				CurrentMedGroup = mGrp;
				CurrentMedGrpIdx = j;
				ExportMediumGroup(mGrp);
			}
		} // for j
	} // for i


	//output to client
	WriteToClientXML( GlobalAutoID );
}
//------------------------------------------------------------------------
void Exporter_NPCNewSpawnGroup::WriteToClientXML( INT& GlobalAutoID )
{
	UWorld*	World = GWorld;
	//static_cast<UWorld*>( UObject::StaticFindObject(UWorld::StaticClass(), PackageToSave, TEXT("TheWorld")) );
	FString ExportDir;
	GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"), ExportDir, GGameIni);

	ExportDir = MMO::FormatTempleteDataLoadingDir(ExportDir);

	FString mapName = World->GetWorldInfo()->GetMapName() + TEXT(".spawngroup");
	FString mapPath = ExportDir + mapName;

	TArray<ANPCNewSpawnGroupActor*> allSpawnGroups;
	for(TObjectIterator<ANPCNewSpawnGroupActor> It; It; ++It)
	{
		ANPCNewSpawnGroupActor* spawnGrp = *It;
		allSpawnGroups.AddItem(spawnGrp);
		RemoveNullRef(spawnGrp);
	}

	for(INT i = 0; i < allSpawnGroups.Num();)
	{
		ANPCNewSpawnGroupActor* spawnGrp = Cast<ANPCNewSpawnGroupActor>(allSpawnGroups(i));		
		if(spawnGrp && spawnGrp->MediumGroupList.Num() == 0)
		{
			allSpawnGroups.Remove(i);
		}
		else
		{
			++i;
		}
	}
	
	const INT C_TMPBUF_SZ = 2048;
	wchar_t tmpBuf[C_TMPBUF_SZ];

	std::wstringstream ss;

	ss << TEXT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	ss << TEXT("<root>\n");
	// large group loop
	INT nLargeGroupCount = allSpawnGroups.Num();
	for(INT i = 0; i < nLargeGroupCount; ++i){
		ANPCNewSpawnGroupActor* spawnGrp = Cast<ANPCNewSpawnGroupActor>(allSpawnGroups(i));
		if(!spawnGrp){
			continue;
		}

		// medium group loop
		for (INT j = 0; j < spawnGrp->MediumGroupList.Num(); ++j)
		{
			ANPCNewSpawnGroupMediumActor* mGrp = Cast<ANPCNewSpawnGroupMediumActor>(spawnGrp->MediumGroupList(j));
			if( mGrp == NULL ){
				continue;
			}
			if( mGrp->SpawnNpcGroupType == ENpcGroupType_Invalid ){
				//No output needed (not shown in the big map, so assumed not needed for now)
				continue;
			}

			if( mGrp->SmallGroupList.Num() == 0 )
			{
				continue;
			}
			FSpawnSmallGroup* sGrp = &mGrp->SmallGroupList(0);

			if( sGrp->SpawnPointList.Num() == 0 )
			{
				continue;
			}
			ANPCNewSpawnPoint* sppt = Cast<ANPCNewSpawnPoint>(sGrp->SpawnPointList(0));

			for (INT n = 0; n < MAX_NPC_TEMPLATE_COUNT; ++n)
			{
				if( sppt->NPCTemplateList(n).NPCTemplateNew )
				{
					UNPCTemplate *npc = sppt->NPCTemplateList(n).NPCTemplateNew;
					memset(tmpBuf, 0, sizeof(wchar_t) * C_TMPBUF_SZ);

					//NPC function attributes
					INT npcFuncType = 0;
					//
					swprintf_s(tmpBuf, C_TMPBUF_SZ - 1,
						TEXT("\t<NPCGroup DisplayName=\"%s\" GroupNpcID=\"%d\" GroupNpcLv=\"%d\" GroupType=\"%d\" Position=\"%s\"/>\n"),
						*npc->GetDisplayName(), npc->TemplateID, mGrp->LevelOffset, mGrp->SpawnNpcGroupType, *mGrp->Location.ToString());

					ss << tmpBuf;

					break;
				}
			}

		} // for medium
	} // for large
	
	ss << TEXT("</root>\n");


	SET_WARN_COLOR(COLOR_GREEN);
	warnf(TEXT("=====[Export]===== Exported npc info file:%s"), *mapPath);
	CLEAR_WARN_COLOR();

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
void Exporter_NPCNewSpawnGroup::WriteHeader( FBufferArchive& ar)
{
	SCENE_FILE_BLOCK_HEADER blkHdrs;
	blkHdrs.eBlockType= sbtNpcSpawnData;
	blkHdrs.nBlockSize = 0;
	blkHdrs.nBlockOffset = sizeof(SCENE_FILE_BLOCK_HEADER);
	
	ar.ByteOrderSerialize(&blkHdrs, sizeof(SCENE_FILE_BLOCK_HEADER) );
	// Store the position first, modify parameters later
	SpawnGroupHeader = (SCENE_FILE_BLOCK_HEADER*)( ar.GetData() + ar.Tell() - sizeof(SCENE_FILE_BLOCK_HEADER) );
}
//------------------------------------------------------------------------
void Exporter_NPCNewSpawnGroup::WriteData( FBufferArchive& ar )
{
	SpawnGroupHeader->nBlockSize = MemAr.TotalSize();
	SpawnGroupHeader->nBlockOffset = ar.Tell();
	
	ar.ByteOrderSerialize(MemAr.GetData(), MemAr.TotalSize());
}
//------------------------------------------------------------------------
void Exporter_NPCNewSpawnGroup::ExportMediumGroup( ANPCNewSpawnGroupMediumActor* mGrp )
{
	NPC_SPAWN_MEDIUM_GROUP medGrp;
	appMemset(&medGrp, 0, sizeof(NPC_SPAWN_MEDIUM_GROUP));

	if(!mGrp)
	{
		MemAr.ByteOrderSerialize(&medGrp, sizeof(NPC_SPAWN_MEDIUM_GROUP));

		GWarn->MapCheck_Add(MCTYPE_ERROR, CurrentLargeGroup, 
			*FString::Printf(TEXT("[%s]MediumGroup[%d] is empty!"),
			*CurrentLargeGroup->GetName(), CurrentMedGrpIdx),
			NULL, MCGROUP_DEFAULT);
		return;	
	}

	//memcpy(&medGrp.qwNameID, TCHAR_TO_UTF8(*mGrp->NameID), strnlen(TCHAR_TO_UTF8(*mGrp->NameID), sizeof(medGrp.qwNameID)));
	medGrp.bAutoRespawn = mGrp->AutoRespawn;
	medGrp.bAutoSpawn = mGrp->AutoSpawn;
	medGrp.nLevelOffset = mGrp->LevelOffset;
	medGrp.nMinRespawnTime = UTypeDefine::ConvertToMS(mGrp->MinRespawnTime);
	medGrp.nMaxRespawnTime = UTypeDefine::ConvertToMS(mGrp->MaxRespawnTime);
	strxcpy(medGrp.szDeadScript, *mGrp->DeadTree, 31);
	medGrp.nSmallGroupCount = mGrp->SmallGroupList.Num();
	medGrp.nID = mGrp->ServerUniqueID;
    medGrp.nTotalWeight = 0;
    for (INT k = 0; k < mGrp->SmallGroupList.Num(); ++k)
    {
        medGrp.nTotalWeight += mGrp->SmallGroupList(k).Weight;
    } // for k

	MemAr.ByteOrderSerialize(&medGrp, sizeof(NPC_SPAWN_MEDIUM_GROUP));

	// small group loop
	for (INT k = 0; k < mGrp->SmallGroupList.Num(); ++k)
	{
		CurrentSmallGrpIdx = k;
		FSpawnSmallGroup* sGrp = &mGrp->SmallGroupList(k);
		ExportSmallGroup(sGrp);
	} // for k
}
//------------------------------------------------------------------------
void Exporter_NPCNewSpawnGroup::ExportSmallGroup( FSpawnSmallGroup* sGrp )
{
	NPC_SPAWN_SMALL_GROUP smGrp;

	smGrp.nWeight = sGrp->Weight;
	smGrp.nPointCount = sGrp->SpawnPointList.Num();
	smGrp.bDynamicRespawnTime = sGrp->DynamicRespawnTime;

	MemAr.ByteOrderSerialize(&smGrp, sizeof(NPC_SPAWN_SMALL_GROUP));

	// spawn point loop
	for (INT m = 0; m < sGrp->SpawnPointList.Num(); ++m)
	{
		ANPCNewSpawnPoint* sppt = Cast<ANPCNewSpawnPoint>(sGrp->SpawnPointList(m));
		if(sppt)
		{
			ExportSpawnPoint(sppt);
		}
		else
		{
			NPC_SPAWN_POINT_DATA spt;
			appMemset(&spt, 0, sizeof(NPC_SPAWN_POINT_DATA));
			MemAr.ByteOrderSerialize(&spt, sizeof(NPC_SPAWN_POINT_DATA));

			GWarn->MapCheck_Add(MCTYPE_ERROR, CurrentMedGroup, 
				*FString::Printf(TEXT("MediumGroup[%d][%d] SmallGroup[%d] NPCNewSpawnPoint[%d] is empty!"),
				CurrentMedGrpIdx, CurrentMedGroup->ServerUniqueID, CurrentSmallGrpIdx, m),
				NULL, MCGROUP_DEFAULT);
		}
	} // for m
}
//------------------------------------------------------------------------
void Exporter_NPCNewSpawnGroup::ExportSpawnPoint( ANPCNewSpawnPoint* sppt )
{
	NPC_SPAWN_POINT_DATA spt;
	FVector pos = sppt->GetServerPosition();
	spt.nX = pos.X;
	spt.nY = pos.Y;
	spt.nZ = pos.Z;
	//song@snda.com
	INT MAXRot = 65536;
	INT yaw = sppt->Rotation.Yaw;
	//while (yaw < 0)
	//{
	//	yaw += MAXRot;
	//}
	//yaw %= MAXRot;
	spt.wDirection = ConvertToServerDegree(yaw);

 	spt.byQuality = sppt->Quality;
	spt.wRange = sppt->Range;
// 	spt.byUseRandomQuality = sppt->UseRandomQuality;
// 	spt.byUseTemplateQuality = sppt->UseTemplateQuality;
	spt.nSpawnTime = UTypeDefine::ConvertToMS(sppt->SpawnTime);
	spt.nReviveTime = UTypeDefine::ConvertToMS(sppt->ReviveTime);
	spt.nAIID = sppt->AIData;
	/*if(sppt->AIData)
	{
		spt.nAIID = sppt->AIData->GetTemplateID();
	}
	else
	{
		spt.nAIID = 0;
	}*/

	spt.nCurrentHp = sppt->CurrentHP;
	spt.nCurrentMp = sppt->CurrentMP;
	spt.bCanDropItem = sppt->CanDropItem;

	spt.nPatrolPathID = 0;
	if(sppt->PatrolOriginServerPoint)
	{
		FString NameID = sppt->PatrolOriginServerPoint->DisplayName;
		if ( NameID.StartsWith( PATROL_TAG ) )
		{
			NameID = NameID.Mid(strlen(PATROL_TAG));
			INT nSeperatorIndex = NameID.InStr( PATROL_SEPERATOR );
			if( nSeperatorIndex == -1 )
			{
				//appErrorf
				return;
			}
			NameID = NameID.Left( nSeperatorIndex );
			if ( NameID.IsNumeric())
			{
				spt.nPatrolPathID = appAtoi(*NameID);
			}
		}
	}
	spt.nPatrolStyle = sppt->PatrolStyle;
	spt.nFollowHostID = sppt->FollowHostID;
	spt.nFollowOffsetX = sppt->FollowOffsetX;
	spt.nFollowOffsetY = sppt->FollowOffsetY;

	appMemset(&spt.NpcList, 0, sizeof(NPC_SPAWN_POINT_DATA::NPC_TEMPLATE_LIST) * MAX_NPC_TEMPLATE_COUNT);

	// npc template list loop
	bool isEmptyNPCTemplateList = true;
	for (INT n = 0; n < MAX_NPC_TEMPLATE_COUNT; ++n)
	{
		if(sppt->NPCTemplateList(n).NPCTemplateNew)
		{
			spt.NpcList[n].nTemplateID = sppt->NPCTemplateList(n).NPCTemplateNew->GetTemplateID();
			isEmptyNPCTemplateList = false;
		}
		else
		{
			spt.NpcList[n].nTemplateID = 0;

// 			GWarn->MapCheck_Add(MCTYPE_ERROR, sppt, 
// 				*FString::Printf(TEXT("MediumGroup[%d][%s] SmallGroup[%d] NPCNewSpawnPoint[%s][%d] NPCTemplate is empty!"),
// 				CurrentMedGrpIdx, *CurrentMedGroup->NameID, CurrentSmallGrpIdx, *sppt->GetName(), n),
// 				NULL, MCGROUP_DEFAULT);
		}

		spt.NpcList[n].nWeight = sppt->NPCTemplateList(n).Weight;
	}

	if(isEmptyNPCTemplateList)
	{
		GWarn->MapCheck_Add(MCTYPE_ERROR, sppt, 
			*FString::Printf(TEXT("MediumGroup[%d][%d] SmallGroup[%d] NPCNewSpawnPoint[%s] NPCTemplate list is empty!"),
			CurrentMedGrpIdx, CurrentMedGroup->ServerUniqueID, CurrentSmallGrpIdx, *sppt->GetName()),
			NULL, MCGROUP_DEFAULT);
	}

	MemAr.ByteOrderSerialize(&spt, sizeof(NPC_SPAWN_POINT_DATA));
}
//------------------------------------------------------------------------
void Exporter_NPCNewSpawnGroup::RemoveNullRef( ANPCNewSpawnGroupActor* spawnGroup )
{
	for (INT i = 0; i < spawnGroup->MediumGroupList.Num(); )
	{
		ANPCNewSpawnGroupMediumActor* medActor = Cast<ANPCNewSpawnGroupMediumActor>(spawnGroup->MediumGroupList(i));
		if(medActor)
		{
			RemoveNullRef(spawnGroup->MediumGroupList(i));
			++i;
		}
		else
		{
			spawnGroup->MediumGroupList.Remove(i);
			//--i;
		}
	}
	
	for (INT i = 0; i < spawnGroup->MediumGroupList.Num(); )
	{
		if((spawnGroup->MediumGroupList(i) == NULL)
			|| !Cast<ANPCNewSpawnGroupMediumActor>(spawnGroup->MediumGroupList(i))
			|| (spawnGroup->MediumGroupList(i)->SmallGroupList.Num() == 0))
		{
			spawnGroup->MediumGroupList.Remove(i);
		}
		else
		{
			++i;
		}
	}
}
//------------------------------------------------------------------------
void Exporter_NPCNewSpawnGroup::RemoveNullRef( ANPCNewSpawnGroupMediumActor* spawnMedGroup )
{
	for (INT i = 0; i < spawnMedGroup->SmallGroupList.Num(); )
	{
		TArrayNoInit< class ANPCNewSpawnPoint* > & spawnPtLst = spawnMedGroup->SmallGroupList(i).SpawnPointList;
		
		for (INT j = 0; j < spawnPtLst.Num(); )
		{
			if(spawnPtLst(j) == NULL || !Cast<ANPCNewSpawnPoint>(spawnPtLst(j)))
			{
				spawnPtLst.Remove(j);
			}
			else
			{
				++j;
			}
		}

		if(spawnPtLst.Num() == 0)
		{
			spawnMedGroup->SmallGroupList.Remove(i);
		}
		else
		{
			++i;
		}
	}
}

#endif