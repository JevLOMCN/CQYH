#include "stdafx.h"
#include "Exporter_NPCSpawnGroup.h"
#include "MMOGameClasses.h"

#if !SHIPPING_PC_GAME

Exporter_NPCSpawnGroup::Exporter_NPCSpawnGroup(void)
{
	//sbtNpcSpawnPoint,
	//sbtNpcSpawnGroup,
}
//------------------------------------------------------------------------
void Exporter_NPCSpawnGroup::CleanUp()
{
	SpawnPointList.clear();
	SpawnGroupList.clear();
}
//------------------------------------------------------------------------
Exporter_NPCSpawnGroup::~Exporter_NPCSpawnGroup(void)
{
	CleanUp();
}
//------------------------------------------------------------------------
void Exporter_NPCSpawnGroup::PrePorcess(INT& GlobalAutoID)
{
	////------------------------------------------------------------------------
	//// Classify objects
	////------------------------------------------------------------------------
	//for (INT i = 0; i < GWorld->Levels.Num(); ++i)
	//{
	//	ULevel* level = GWorld->Levels(i);
	//	for (INT j = 0; j < level->Actors.Num(); ++j)
	//	{
	//		AActor* actor = level->Actors(j);
	//		if(actor)
	//		{				
	//			if(actor->IsA(AMMONPCSpawnGroup::StaticClass()))
	//			{
	//				SpawnGroupList.push_back( Cast<AMMONPCSpawnGroup >(actor) );
	//			}
	//			else if(actor->IsA(ANPCSpawnPoint::StaticClass()))
	//			{
	//				SpawnPointList.push_back( Cast<ANPCSpawnPoint >(actor) );
	//			}
	//		}
	//	}
	//}

	//INT autoID = 0;

	//// Auto-numbering
	//for( vector< AMMONPCSpawnGroup * >::iterator it = SpawnGroupList.begin(); it != SpawnGroupList.end() ; ++it)
	//{
	//	AMMONPCSpawnGroup* SpawnGrp = *it;
	//	SpawnGrp->AutoServerID = autoID;
	//	++autoID;
	//}

	//IniData();

}
//------------------------------------------------------------------------
void Exporter_NPCSpawnGroup::IniData()
{
	//NPC_SPAWN_POINT_HEADER  *spntHdr = new NPC_SPAWN_POINT_HEADER;
	//NPC_SPAWN_GROUP_HEADER *grpHdr  = new NPC_SPAWN_GROUP_HEADER;

	//// =========================Export SpawnGroupList==================
	//grpHdr->nGroupCount = SpawnGroupList.size();
	////Fixed length, allocate uniformly
	//NPC_SPAWN_GROUP_DATA *grpList = new NPC_SPAWN_GROUP_DATA[ grpHdr->nGroupCount ];
	//memGroupAr.ByteOrderSerialize(grpHdr, sizeof(NPC_SPAWN_GROUP_HEADER));
	//for (INT i = 0; i < grpHdr->nGroupCount ; ++i)
	//{
	//	//AutoServerID not used for now
	//	grpList[i].nGroupID = SpawnGroupList[i]->AutoServerID;//NPCSpawnGroupId;
	//	if(SpawnGroupList[i]->MaxConcurrentPoint > SpawnGroupList[i]->NPCSpawnPoints.Num())
	//		grpList[i].nMaxConcurrentPoint = SpawnGroupList[i]->NPCSpawnPoints.Num();
	//	else
	//		grpList[i].nMaxConcurrentPoint = SpawnGroupList[i]->MaxConcurrentPoint;
	//	grpList[i].IsAutoSpawn = SpawnGroupList[i]->IsAutoSpawn;
	//	grpList[i].IsAutoReSpawn = SpawnGroupList[i]->IsAutoReSpawn;
	//	//This is why Grp is exported first: it completes the Group assignment for SpawnPoint
	//	for( INT j = 0; j < SpawnGroupList[i]->NPCSpawnPoints.Num(); ++j )
	//	{
	//		if(!SpawnGroupList[i])
	//		{
	//			GWarn->MapCheck_Add(MCTYPE_ERROR, 0, 
	//				*FString::Printf(TEXT("SpawnGroupList[%d] is empty!"), i),
	//				NULL, MCGROUP_DEFAULT);
	//		}
	//		else if(!SpawnGroupList[i]->NPCSpawnPoints(j))
	//		{
	//			GWarn->MapCheck_Add(MCTYPE_ERROR, SpawnGroupList[i], 
	//				*FString::Printf(TEXT("SpawnPoints is empty:%s"), *SpawnGroupList[i]->GetName()),
	//				NULL, MCGROUP_DEFAULT);
	//		}
	//		else
	//		{
	//			SpawnGroupList[i]->NPCSpawnPoints( j )->GroupID = SpawnGroupList[i]->AutoServerID;
	//		}
	//	}
	//	memGroupAr.ByteOrderSerialize(&grpList[i], sizeof(NPC_SPAWN_GROUP_DATA));
	//}

	//// =========================Export ANPCSpawnPoint==================
	//spntHdr->nPointCount = SpawnPointList.size();
	////This is variable length so memory needs to be allocated in advance
	//int SpawnPointDataSize= spntHdr->nPointCount*sizeof( NPC_SPAWN_POINT_DATA ) ;
	//for (INT i = 0; i < spntHdr->nPointCount ; ++i)
	//{
	//	SpawnPointDataSize+=SpawnPointList[i]->NPCTemplateList.Num() * sizeof( NPC_SPAWN_DATA );
	//}
	//char* pMem=new char[ SpawnPointDataSize ]; 
	//char* pSpawnPointData=pMem;
	//NPC_SPAWN_POINT_DATA *spntData;
	//NPC_SPAWN_DATA		*spntNpcData;
	//memPointAr.ByteOrderSerialize(spntHdr, sizeof(NPC_SPAWN_POINT_HEADER));
	//for (INT i = 0; i < spntHdr->nPointCount ; ++i)
	//{
	//	spntData = (NPC_SPAWN_POINT_DATA*)pSpawnPointData;
	//	pSpawnPointData += sizeof(NPC_SPAWN_POINT_DATA);
	//	spntData->nX = SpawnPointList[i]->Location.X * POINT_PER_CM;
	//	spntData->nY = SpawnPointList[i]->Location.Y * POINT_PER_CM;
	//	spntData->nZ = SpawnPointList[i]->Location.Z * POINT_PER_CM;
	//	spntData->shDirection = (UINT16)(SpawnPointList[i]->Rotation.Yaw);
	//	spntData->nGroupID = SpawnPointList[i]->GroupID; 
	//	spntData->nNpcCount = SpawnPointList[i]->NPCTemplateList.Num();
	//	//The stream << could also be used for direct input here, but this is more intuitive and easier to debug
	//	memPointAr.ByteOrderSerialize(spntData, sizeof(NPC_SPAWN_POINT_DATA));
	//	//SpawnNPCTableOffest starts after all NPCSpawnPoint data
	//	for( INT j = 0; j < spntData -> nNpcCount  ; ++j )
	//	{
	//		spntNpcData = (NPC_SPAWN_DATA*)pSpawnPointData;
	//		pSpawnPointData += sizeof(NPC_SPAWN_DATA);
	//		//Over-configured, may be misconfigured, resulting in empty values
	//		if(!SpawnPointList[i]->NPCTemplateList(j).NPCTemplateArchi )
	//		{
	//			spntNpcData->nTemplateID = 0 ;
	//			spntNpcData->nWeight = 0 ;
	//			GWarn->MapCheck_Add(MCTYPE_ERROR, SpawnPointList[i], 
	//				*FString::Printf(TEXT("SpawnPointList[%d] NPCTemplateList [%d]is empty!"), i,j ),
	//				NULL, MCGROUP_DEFAULT);
	//		}
	//		else
	//		{
	//			spntNpcData->nTemplateID = SpawnPointList[i]->NPCTemplateList(j).NPCTemplateArchi->NPCID;
	//			spntNpcData->nWeight = SpawnPointList[i]->NPCTemplateList(j).Weight;
	//		}
	//		memPointAr.ByteOrderSerialize(spntNpcData, sizeof(NPC_SPAWN_DATA));
	//	}
	//}

	//SAFEDELETE( spntHdr );
	//SAFEDELETE( grpHdr );
	//SAFEDELETEARRAY( grpList );
	//spntData=NULL;
	//spntNpcData=NULL;
	//SAFEDELETE( pMem );
}
//------------------------------------------------------------------------
void Exporter_NPCSpawnGroup::WriteHeader( FBufferArchive& ar)
{
	////Serialize
	////Ugly, make do for now
	//SCENE_FILE_BLOCK_HEADER blkHdrs[2];
	//blkHdrs[0].eBlockType= sbtNpcSpawnPoint;
	//blkHdrs[0].nBlockSize = 0;
	//blkHdrs[0].nBlockOffset = sizeof(SCENE_FILE_BLOCK_HEADER);

	//blkHdrs[1].eBlockType=sbtNpcSpawnGroup;
	//blkHdrs[1].nBlockSize = 0;
	//blkHdrs[1].nBlockOffset = sizeof(SCENE_FILE_BLOCK_HEADER);

	//ar.ByteOrderSerialize(&blkHdrs[0], sizeof(SCENE_FILE_BLOCK_HEADER) );
	//// Store the position first, modify parameters later
	//arBlkHdrs[0] = (SCENE_FILE_BLOCK_HEADER*)( ar.GetData() + ar.Tell() - sizeof(SCENE_FILE_BLOCK_HEADER) );

	//ar.ByteOrderSerialize(&blkHdrs[1], sizeof(SCENE_FILE_BLOCK_HEADER));
	//arBlkHdrs[1] = (SCENE_FILE_BLOCK_HEADER*)( ar.GetData() + ar.Tell() - sizeof(SCENE_FILE_BLOCK_HEADER) );
}
//------------------------------------------------------------------------
void Exporter_NPCSpawnGroup::WriteData( FBufferArchive& ar )
{
	//// =====================Write SpawnPointList to memory=====================
	//arBlkHdrs[0]->nBlockSize = memPointAr.TotalSize();
	////Calculate SpawnPointList nBlockOffset. Note: Offest means the Global Offset relative to SCENE_FILE_HEADER
	//arBlkHdrs[0]->nBlockOffset = ar.Tell();
	////Serialize will likely ReAllocate and the address will change; watching arBlkHdrs[0] will show anomalies
	//ar.ByteOrderSerialize(memPointAr.GetData(), memPointAr.TotalSize());

	////===================== Write SpawnGroup to memory=======================
	//arBlkHdrs[1]->nBlockSize = memGroupAr.TotalSize();
	//arBlkHdrs[1]->nBlockOffset = ar.Tell();
	//ar.ByteOrderSerialize(memGroupAr.GetData(), memGroupAr.TotalSize());
}

#endif