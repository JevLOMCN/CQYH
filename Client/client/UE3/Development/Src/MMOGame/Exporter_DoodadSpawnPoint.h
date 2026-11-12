#ifndef _SINGLENPCSPAWNPOINT_H_
#define _SINGLENPCSPAWNPOINT_H_

#if !SHIPPING_PC_GAME

#include "ExportServerData.h"

//illegal zero-sized array needed by the server data structure
#pragma warning(disable:4200)

class Exporter_DoodadSpawnPoint : public IServerSceneExporter
{
public:
	Exporter_DoodadSpawnPoint(void);
	virtual ~Exporter_DoodadSpawnPoint(void);

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