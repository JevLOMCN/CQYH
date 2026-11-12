#ifndef _EXPROTER_CHESSNODE_H_
#define _EXPROTER_CHESSNODE_H_

#if !SHIPPING_PC_GAME

#include "ExportServerData.h"

//illegal zero-sized array needed by the server data structure
#pragma warning(disable:4200)

class Exporter_ChessNode : public IServerSceneExporter
{
public:
	Exporter_ChessNode(void);
	virtual ~Exporter_ChessNode(void);

	virtual void PrePorcess(INT& GlobalAutoID);
	virtual void WriteHeader(FBufferArchive& ar  );
	virtual void WriteData(FBufferArchive& ar);
	
	virtual	int GetBlockNumber() const { return 1; }

private:
	SCENE_FILE_BLOCK_HEADER* arBlkHdrs;
	FBufferArchive			memPointAr;

};

#endif

#endif