#include "stdafx.h"
#include "Exporter_TagNode.h"
#include "MMOGameClasses.h"

#if !SHIPPING_PC_GAME


//Export NPCs placed individually in the scene
Exporter_TagNode::Exporter_TagNode(void)
{
}
//------------------------------------------------------------------------
Exporter_TagNode::~Exporter_TagNode(void)
{
}
//------------------------------------------------------------------------
void Exporter_TagNode::PrePorcess(INT& GlobalAutoID)
{
	SERVER_POINT_HEADER  *spntHdr = new SERVER_POINT_HEADER;
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
				if(actor->IsA(AMMOServerPoint::StaticClass()))
				{
					ServerTagNodeList.push_back( Cast<AMMOServerPoint>(actor) );
				}
			}
		}
	}

	spntHdr->nPointCount = ServerTagNodeList.size();
	memPointAr.ByteOrderSerialize(spntHdr, sizeof(SERVER_POINT_HEADER));
	for (INT i = 0; i < spntHdr->nPointCount ; ++i)
	{
		SERVER_POINT snpt = {0};

		if(!ServerTagNodeList[i])
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, 0, 
				*FString::Printf(TEXT("ServerTagNodeList[%d] is empty!"), i),
				NULL, MCGROUP_DEFAULT);
			continue;
		}

		memcpy(&snpt.qwNameID, TCHAR_TO_UTF8(*ServerTagNodeList[i]->DisplayName), 
            strnlen(TCHAR_TO_UTF8(*ServerTagNodeList[i]->DisplayName), sizeof(snpt.qwNameID)));
		snpt.nID = ServerTagNodeList[i]->ServerUniqueID;
		FVector p = ServerTagNodeList[i]->Location;
		ConvertCoordC2S(p);
		p.X = (INT32)p.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
		p.Y = (INT32)p.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
		snpt.nX = p.X;
		snpt.nY = p.Y;
		snpt.nZ = p.Z;
		snpt.nRange = ServerTagNodeList[i]->Range * POINT_PER_CM;
		snpt.nDirection = ConvertToServerDegree(ServerTagNodeList[i]->Rotation.Yaw);
		memPointAr.ByteOrderSerialize(&snpt, sizeof(SERVER_POINT));
	}
	SAFEDELETE( spntHdr );
}
//------------------------------------------------------------------------
void Exporter_TagNode::WriteHeader( FBufferArchive& ar)
{
	//Serialize
	SCENE_FILE_BLOCK_HEADER blkHdrs;
	blkHdrs.eBlockType= sbtServerPoint;
	blkHdrs.nBlockSize = 0;
	blkHdrs.nBlockOffset = sizeof(SCENE_FILE_BLOCK_HEADER);

	ar.ByteOrderSerialize(&blkHdrs, sizeof(SCENE_FILE_BLOCK_HEADER) );
	// Store the position first, modify parameters later
	arBlkHdrs = (SCENE_FILE_BLOCK_HEADER*)( ar.GetData() + ar.Tell() - sizeof(SCENE_FILE_BLOCK_HEADER) );

}
//------------------------------------------------------------------------
void Exporter_TagNode::WriteData( FBufferArchive& ar )
{
	// =====================Write NPCSpawnPointList to memory=====================
	arBlkHdrs->nBlockSize = memPointAr.TotalSize();
	// nBlockOffset 
	arBlkHdrs->nBlockOffset = ar.Tell();
	ar.ByteOrderSerialize(memPointAr.GetData(), memPointAr.TotalSize());
}

#endif