#ifndef _EXPROTER_TAGNODE_H_
#define _EXPROTER_TAGNODE_H_

#if !SHIPPING_PC_GAME

#include "ExportServerData.h"
#include <vector>

//illegal zero-sized array needed by the server data structure
#pragma warning(disable:4200)

class Exporter_TagNode : public IServerSceneExporter
{
public:
	Exporter_TagNode(void);
	virtual ~Exporter_TagNode(void);

	virtual void PrePorcess(INT& GlobalAutoID);
	virtual void WriteHeader(FBufferArchive& ar  );
	virtual void WriteData(FBufferArchive& ar);
	
	virtual	int GetBlockNumber() const { return 1; }

private:
	SCENE_FILE_BLOCK_HEADER* arBlkHdrs;

	//vector< ANPCServerInstance * >	NPCSpawnPointList; 
	std::vector< AMMOServerPoint* >	ServerTagNodeList; 
	FBufferArchive			memPointAr;

};

#endif

#endif