#include "stdafx.h"
#include "Exporter_DoodadSpawn.h"
#include "MMOGameClasses.h"
#include "net/message_handler.h"

#if !SHIPPING_PC_GAME


Exporter_DoodadSpawn::Exporter_DoodadSpawn(void)
{
}
//------------------------------------------------------------------------
Exporter_DoodadSpawn::~Exporter_DoodadSpawn(void)
{
}
//------------------------------------------------------------------------
void Exporter_DoodadSpawn::PrePorcess(INT& GlobalAutoID)
{
	DOODAD_SPAWN_DATA_HEADER  Header;
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
				if(actor->IsA(ADoodadSpawnGroup::StaticClass()))
				{
					DoodadSpawnGroupList.push_back( Cast<ADoodadSpawnGroup >(actor) );
				}
				//else if(actor->IsA(ADoodadServerInstance::StaticClass()))
				//{
				//	ADoodadSpawnGroup*
				//	DoodadSpawnGroupList.push_back();
				//}
			}
		}
	}

	INT autoID = 0;
	// Auto-numbering
	for( DoodadSpawnGroupVector::iterator it = DoodadSpawnGroupList.begin(); it != DoodadSpawnGroupList.end() ; ++it)
	{
		ADoodadSpawnGroup* tDoodad = *it;
		tDoodad->AutoServerID = autoID;
		++autoID;
	}

	Header.nGroupCount = DoodadSpawnGroupList.size();
	memPointAr.ByteOrderSerialize(&Header, sizeof(Header));
	for (INT i = 0; i < Header.nGroupCount ; ++i)
	{
		if(!DoodadSpawnGroupList[i])
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, 0, 
				*FString::Printf(TEXT("DoodadServerInstance[%d] is empty!"), i),
				NULL, MCGROUP_DEFAULT);
			continue;
		}
        INT32 nDataSize = sizeof(DOODAD_SPAWN_GROUP_DATA) + 
            sizeof(DOODAD_SPAWN_POINT_DATA) * DoodadSpawnGroupList[i]->DoodadSpawnPoints.Num();
        DOODAD_SPAWN_GROUP_DATA* pGroupData = (DOODAD_SPAWN_GROUP_DATA*)malloc(nDataSize);
        memset(pGroupData, 0, nDataSize);

		pGroupData->nGroupID			= DoodadSpawnGroupList[i]->GroupID;
		pGroupData->nMaxConcurrentPoint	= DoodadSpawnGroupList[i]->MaxConcurrentPoint;
		pGroupData->IsAutoSpawn			= DoodadSpawnGroupList[i]->IsAutoSpawn;
		pGroupData->IsAutoReSpawn		= DoodadSpawnGroupList[i]->IsAutoReSpawn;
		pGroupData->nRespawnInterval	= DoodadSpawnGroupList[i]->RespawnInterval;
		pGroupData->nPointCount			= DoodadSpawnGroupList[i]->DoodadSpawnPoints.Num();
		
		for(INT j = 0; j < DoodadSpawnGroupList[i]->DoodadList.Num(); ++j)
		{
			if(DoodadSpawnGroupList[i]->DoodadList(j).Doodad)
			{
				pGroupData->DoodadList[j].nTemplateID = DoodadSpawnGroupList[i]->DoodadList(j).Doodad->GetTemplateID();
				pGroupData->DoodadList[j].nWeight = DoodadSpawnGroupList[i]->DoodadList(j).nWeight;
			}
		}

		for(INT j = 0; j < pGroupData->nPointCount; ++j)
		{
			if(!DoodadSpawnGroupList[i]->DoodadSpawnPoints(j))
			{
				GWarn->MapCheck_Add(MCTYPE_ERROR, DoodadSpawnGroupList[i]->DoodadSpawnPoints(j), 
					*FString::Printf(TEXT("DoodadServerInstance[%d] Doodad Template is empty!"), j),
					NULL, MCGROUP_DEFAULT);
				continue;
			}
			FVector p = DoodadSpawnGroupList[i]->DoodadSpawnPoints(j)->Location;
			ConvertCoordC2S(p);
			p.X = (INT32)p.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
			p.Y = (INT32)p.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
			pGroupData->PointList[j].nX = p.X;
			pGroupData->PointList[j].nY = p.Y;
			pGroupData->PointList[j].nZ = p.Z;
			////song@snda.com
			//INT MAXRot = 65536;
			INT yaw = DoodadSpawnGroupList[i]->DoodadSpawnPoints(j)->Rotation.Yaw;
			//while (yaw < 0)
			//{
			//	yaw += MAXRot;
			//}
			//yaw %= MAXRot;
			pGroupData->PointList[j].shDirection = ConvertToServerDegree(yaw);


		}

		memPointAr.ByteOrderSerialize(pGroupData, nDataSize);
	}
}
//------------------------------------------------------------------------
void Exporter_DoodadSpawn::WriteHeader( FBufferArchive& ar)
{
	//Serialize
	SCENE_FILE_BLOCK_HEADER blkHdrs;
	blkHdrs.eBlockType= sbtDoodadSpawnData;
	blkHdrs.nBlockSize = 0;
	blkHdrs.nBlockOffset = sizeof(SCENE_FILE_BLOCK_HEADER);

	ar.ByteOrderSerialize(&blkHdrs, sizeof(SCENE_FILE_BLOCK_HEADER) );
	// Store the position first, modify parameters later
	arBlkHdrs = (SCENE_FILE_BLOCK_HEADER*)( ar.GetData() + ar.Tell() - sizeof(SCENE_FILE_BLOCK_HEADER) );

}
//------------------------------------------------------------------------
void Exporter_DoodadSpawn::WriteData( FBufferArchive& ar )
{
	// =====================Write NPCSpawnPointList to memory=====================
	arBlkHdrs->nBlockSize = memPointAr.TotalSize();
	// nBlockOffset 
	arBlkHdrs->nBlockOffset = ar.Tell();
	ar.ByteOrderSerialize(memPointAr.GetData(), memPointAr.TotalSize());
}

#endif