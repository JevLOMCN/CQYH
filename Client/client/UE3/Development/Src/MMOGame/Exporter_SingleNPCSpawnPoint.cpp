#include "stdafx.h"
#include "Exporter_SingleNPCSpawnPoint.h"
#include "MMOGameClasses.h"
#include "net/message_handler.h"

#if !SHIPPING_PC_GAME

using namespace std;
//Export NPCs placed individually in the scene
Exporter_SingleNPCSpawnPoint::Exporter_SingleNPCSpawnPoint(void)
{
}
//------------------------------------------------------------------------
Exporter_SingleNPCSpawnPoint::~Exporter_SingleNPCSpawnPoint(void)
{
}
//------------------------------------------------------------------------

void Exporter_SingleNPCSpawnPoint::PrePorcess(INT& GlobalAutoID)
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

// 	INT autoID = 0;
// 	// Auto-numbering
// 	for( vector< ANPCServerInstance * >::iterator it = NPCSpawnPointList.begin(); it != NPCSpawnPointList.end() ; ++it)
// 	{
// 		ANPCServerInstance* tNPC = *it;
// 		tNPC->AutoServerID = autoID;
// 		++autoID;
// 	}

	spntHdr->nNpcCount = NPCSpawnPointList.size();
	memPointAr.ByteOrderSerialize(spntHdr, sizeof(NPC_SINGLE_SPAWN_HEADER));
	NPC_SINGLE_SPAWN snpt = {0};
	for (INT i = 0; i < spntHdr->nNpcCount ; ++i)
	{
		ANPCServerInstance* npcInst = NPCSpawnPointList[i];
		if(!npcInst)
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, 0, 
				*FString::Printf(TEXT("NPCServerInstance[%d] is empty!"), i),
				NULL, MCGROUP_DEFAULT);
			continue;
		}

		if(npcInst->TemplateNew)
		{
			snpt.nNpcTemplateID = npcInst->TemplateNew->GetTemplateID();
		}
		else
		{
			snpt.nNpcTemplateID = 0;
			GWarn->MapCheck_Add(MCTYPE_ERROR, npcInst, 
				*FString::Printf(TEXT("NPCServerInstance[%d] NPC Template is empty!"), i),
				NULL, MCGROUP_DEFAULT);
		}

		snpt.qwNameID = 0;
		memcpy(&snpt.qwNameID, TCHAR_TO_UTF8(*npcInst->NameID), strnlen(TCHAR_TO_UTF8(*npcInst->NameID), sizeof(snpt.qwNameID)));
		snpt.nID = npcInst->ServerUniqueID;

		snpt.IsAutoSpawn = npcInst->IsAutoSpawn;
		snpt.IsAutoReSpawn = npcInst->IsAutoReSpawn;
		FVector p = npcInst->Location;
		ConvertCoordC2S(p);
		p.X = (INT32)p.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
		p.Y = (INT32)p.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
		snpt.nX = p.X;
		snpt.nY = p.Y;
		snpt.nZ = p.Z;
		snpt.wRange = (UINT16)npcInst->Range;

		//song@snda.com
		INT MAXRot = 65536;
		INT yaw = npcInst->Rotation.Yaw;
		//while (yaw < 0)
		//{
		//	yaw += MAXRot;
		//}
		//yaw %= MAXRot;
		snpt.wDirection = ConvertToServerDegree(yaw);


		snpt.byQuality = npcInst->Quality;
		snpt.nSpawnTime = UTypeDefine::ConvertToMS(npcInst->SpawnTimeServer);
		snpt.nMinRespawnTime = UTypeDefine::ConvertToMS(npcInst->MinRespawnTime);
		snpt.nMaxRespawnTime = UTypeDefine::ConvertToMS(npcInst->MaxRespawnTime);
		snpt.nReviveTime = UTypeDefine::ConvertToMS(npcInst->ReviveTime);

		snpt.nAIID = npcInst->AIData;

// 		if(npcInst->AIData)
// 		{
// 			snpt.nAIID = npcInst->AIData->GetTemplateID();
// 		}
// 		else
// 		{
// 			snpt.nAIID = 0;
// // 			GWarn->MapCheck_Add(MCTYPE_ERROR, npcInst, 
// // 				*FString::Printf(TEXT("NPCServerInstance[%d] AI Data is empty!"), i),
// // 				NULL, MCGROUP_DEFAULT);
// 		}

		snpt.nCurrentHp = npcInst->CurrentHP;
		snpt.nCurrentMp = npcInst->CurrentMP;
		snpt.bCanDropItem = npcInst->CanDropItem;
		snpt.nLevelOffset = npcInst->LevelOffset;

		snpt.nPatrolPathID = 0;
		if(npcInst->PatrolOriginServerPoint)
		{
			FString NameID = npcInst->PatrolOriginServerPoint->DisplayName;
			if ( NameID.StartsWith( PATROL_TAG ) )
			{
				NameID = NameID.Mid(strlen(PATROL_TAG));
				INT nSeperatorIndex = NameID.InStr( PATROL_SEPERATOR );
				if( nSeperatorIndex == -1 )
				{
					//appErrorf
					continue;
				}
				NameID = NameID.Left( nSeperatorIndex );
				if ( NameID.IsNumeric())
				{
					snpt.nPatrolPathID = appAtoi(*NameID);
				}
			}
		}
		snpt.nPatrolStyle = npcInst->PatrolStyle;
		snpt.nFollowHostID = npcInst->FollowHostID;
		snpt.nFollowOffsetX = npcInst->FollowOffsetX;
		snpt.nFollowOffsetY = npcInst->FollowOffsetY;
		
		strxcpy(snpt.szDeadScript, TCHAR_TO_UTF8(*npcInst->DeadScript), MAX_EMBEDDED_SCRIPT_LEN - 1);
		
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
