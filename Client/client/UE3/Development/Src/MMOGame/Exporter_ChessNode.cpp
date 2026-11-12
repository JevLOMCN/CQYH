#include "stdafx.h"
#include "Exporter_ChessNode.h"
#include "MMOGameClasses.h"

#if !SHIPPING_PC_GAME


//Export NPCs placed individually in the scene
Exporter_ChessNode::Exporter_ChessNode(void)
{
}
//------------------------------------------------------------------------
Exporter_ChessNode::~Exporter_ChessNode(void)
{
}
//------------------------------------------------------------------------
void Exporter_ChessNode::PrePorcess(INT& GlobalAutoID)
{
	//MMOSERVER_CHESSNODE_HEADER  *spntHdr = new MMOSERVER_CHESSNODE_HEADER;
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
	//			if(actor->IsA(AMMOServerCHESSNODE::StaticClass()))
	//			{
	//				ServerCHESSNODEList.push_back( Cast<AMMOServerCHESSNODE>(actor) );
	//			}
	//		}
	//	}
	//}

	//spntHdr->nCHESSNODECount = ServerCHESSNODEList.size();
	//memPointAr.ByteOrderSerialize(spntHdr, sizeof(MMOSERVER_CHESSNODE_HEADER));
	//MMOSERVER_CHESSNODE snpt = {0};
	//for (UINT i = 0; i < spntHdr->nCHESSNODECount ; ++i)
	//{
	//	if(!ServerCHESSNODEList[i])
	//	{
	//		GWarn->MapCheck_Add(MCTYPE_ERROR, 0, 
	//			*FString::Printf(TEXT("ServerCHESSNODEList[%d] is empty!"), i),
	//			NULL, MCGROUP_DEFAULT);
	//		continue;
	//	}


	//	memcpy_s((void*)&snpt.nNodeName, 8, (void*)(TCHAR_TO_UTF8(*ServerCHESSNODEList[i]->DisplayName)), 8);
	//	snpt.nX = ServerCHESSNODEList[i]->Location.X * POINT_PER_CM;
	//	snpt.nY = ServerCHESSNODEList[i]->Location.Y * POINT_PER_CM;
	//	snpt.nZ = ServerCHESSNODEList[i]->Location.Z * POINT_PER_CM;
	//	//snpt.shDirection = (UINT16)NPCSpawnPointList[i]->Rotation.Yaw;
	//	memPointAr.ByteOrderSerialize(&snpt, sizeof(MMOSERVER_CHESSNODE));
	//}
	//SAFEDELETE( spntHdr );
}
//------------------------------------------------------------------------
void Exporter_ChessNode::WriteHeader( FBufferArchive& ar)
{
	////Serialize
	//SCENE_FILE_BLOCK_HEADER blkHdrs;
	//blkHdrs.eBlockType= sbtServerCHESSNODE;
	//blkHdrs.nBlockSize = 0;
	//blkHdrs.nBlockOffset = sizeof(SCENE_FILE_BLOCK_HEADER);

	//ar.ByteOrderSerialize(&blkHdrs, sizeof(SCENE_FILE_BLOCK_HEADER) );
	//// Store the position first, modify parameters later
	//arBlkHdrs = (SCENE_FILE_BLOCK_HEADER*)( ar.GetData() + ar.Tell() - sizeof(SCENE_FILE_BLOCK_HEADER) );

}
//------------------------------------------------------------------------
void Exporter_ChessNode::WriteData( FBufferArchive& ar )
{
	//// =====================Write NPCSpawnPointList to memory=====================
	//arBlkHdrs->nBlockSize = memPointAr.TotalSize();
	//// nBlockOffset 
	//arBlkHdrs->nBlockOffset = ar.Tell();
	//ar.ByteOrderSerialize(memPointAr.GetData(), memPointAr.TotalSize());
}

#endif