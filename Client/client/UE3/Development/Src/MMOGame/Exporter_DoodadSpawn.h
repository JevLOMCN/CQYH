#ifndef _DOODADINSTANCE_H_
#define _DOODADINSTANCE_H_

#if !SHIPPING_PC_GAME

#include "ExportServerData.h"
#include "MMOUtil.h"

//illegal zero-sized array needed by the server data structure
#pragma warning(disable:4200)

class Exporter_DoodadSpawn : public IServerSceneExporter
{
public:
	Exporter_DoodadSpawn(void);
	virtual ~Exporter_DoodadSpawn(void);

	virtual void PrePorcess(INT& GlobalAutoID);
	virtual void WriteHeader(FBufferArchive& ar  );
	virtual void WriteData(FBufferArchive& ar);
	
	virtual	int GetBlockNumber() const { return 1; }

private:
	SCENE_FILE_BLOCK_HEADER* arBlkHdrs;

	typedef std::vector< ADoodadSpawnGroup *, M_UE3_STL_ALLOC(ADoodadSpawnGroup *) > DoodadSpawnGroupVector;
	DoodadSpawnGroupVector	DoodadSpawnGroupList; 
	FBufferArchive			memPointAr;

};

#endif

#endif