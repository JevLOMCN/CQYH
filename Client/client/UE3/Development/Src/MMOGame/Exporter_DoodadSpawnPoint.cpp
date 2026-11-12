#include "stdafx.h"
#include "Exporter_SingleNPCSpawnPoint.h"
#include "MMOGameClasses.h"

#if !SHIPPING_PC_GAME


//Export NPCs placed individually in the scene
Exporter_DoodadSpawnPoint::Exporter_DoodadSpawnPoint(void)
{
}
//------------------------------------------------------------------------
Exporter_DoodadSpawnPoint::~Exporter_DoodadSpawnPoint(void)
{
}
//------------------------------------------------------------------------
void Exporter_DoodadSpawnPoint::PrePorcess(INT& GlobalAutoID)
{
	NPC_SINGLE_SPAWN_HEADER  *spntHdr = new NPC_SINGLE_SPAWN_HEADER;
	//------------------------------------------------------------------------
	// Classify objects
	//------------------------------------------------------------------------
	for (INT i = 0; i < GWorld->Levels.Num(); ++i)
	{
		ULevel* level = GWorld->Levels(i);
		for (INT j = 0; j < level->Actors.Num(); ++j)
		{
			AActor* actor = level->Actors(j);
			if(actor)
			{				
				if(actor->IsA(ANPCServerInstance::StaticClass()))
				{
					NPCSpawnPointList.push_back( Cast<ANPCServerInstance >(actor) );
				}
			}
		}
	}

	INT autoID = 0;
	// Auto-numbering
	for( vector< ANPCServerInstance * >::iterator it = NPCSpawnPointList.begin(); it != NPCSpawnPointList.end() ; ++it)
	{
		ANPCServerInstance* tNPC = *it;
		tNPC->AutoServerID = autoID;
		++autoID;
	}

	spntHdr->nNpcCount = NPCSpawnPointList.size();
	memPointAr.ByteOrderSerialize(spntHdr, sizeof(NPC_SINGLE_SPAWN_HEADER));
	NPC_SINGLE_SPAWN snpt = {0};
	for (INT i = 0; i < spntHdr->nNpcCount ; ++i)
	{
		if(!NPCSpawnPointList[i])
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, 0, 
				*FString::Printf(TEXT("NPCServerInstance[%d] is empty!"), i),
				NULL, MCGROUP_DEFAULT);
			continue;
		}

		if(!NPCSpawnPointList[i]->Template)
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, NPCSpawnPointList[i], 
				*FString::Printf(TEXT("NPCServerInstance[%d] NPC Template is empty!"), i),
				NULL, MCGROUP_DEFAULT);
			continue;
		}

		snpt.nNpcTemplateID = NPCSpawnPointList[i]->Template->NPCID;
		snpt.AutoServerID= NPCSpawnPointList[i]->AutoServerID;
		snpt.IsAutoSpawn = NPCSpawnPointList[i]->IsAutoSpawn;
		snpt.IsAutoReSpawn = NPCSpawnPointList[i]->IsAutoReSpawn;
		snpt.nX = NPCSpawnPointList[i]->Location.X * POINT_PER_CM;
		snpt.nY = NPCSpawnPointList[i]->Location.Y * POINT_PER_CM;
		snpt.nZ = NPCSpawnPointList[i]->Location.Z * POINT_PER_CM;
		snpt.shDirection = (UINT16)NPCSpawnPointList[i]->Rotation.Yaw;
		memPointAr.ByteOrderSerialize(&snpt, sizeof(NPC_SINGLE_SPAWN));
	}
	SAFEDELETE( spntHdr );
}
//------------------------------------------------------------------------
void Exporter_SingleNPCSpawnPoint::WriteHeader( FBufferArchive& ar)
{
	//Serialize
	SCENE_FILE_BLOCK_HEADER blkHdrs;
	blkHdrs.eBlockType= sbtNpcSingleSpawn;
	blkHdrs.nBlockSize = 0;
	blkHdrs.nBlockOffset = sizeof(SCENE_FILE_BLOCK_HEADER);

	ar.ByteOrderSerialize(&blkHdrs, sizeof(SCENE_FILE_BLOCK_HEADER) );
	// Store the position first, modify parameters later
	arBlkHdrs = (SCENE_FILE_BLOCK_HEADER*)( ar.GetData() + ar.Tell() - sizeof(SCENE_FILE_BLOCK_HEADER) );

}
//------------------------------------------------------------------------
void Exporter_SingleNPCSpawnPoint::WriteData( FBufferArchive& ar )
{
	// =====================Write NPCSpawnPointList to memory=====================
	arBlkHdrs->nBlockSize = memPointAr.TotalSize();
	// nBlockOffset 
	arBlkHdrs->nBlockOffset = ar.Tell();
	ar.ByteOrderSerialize(memPointAr.GetData(), memPointAr.TotalSize());
}

#endif