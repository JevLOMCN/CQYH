#ifndef _EXPORTER_NPCSPAWNGROUP_H_
#define _EXPORTER_NPCSPAWNGROUP_H_

#if !SHIPPING_PC_GAME

#include "ExportServerData.h"
#include <vector>

//illegal zero-sized array needed by the server data structure
#pragma warning(disable:4200)

class Exporter_NPCSpawnGroup : public IServerSceneExporter
{
public:
	Exporter_NPCSpawnGroup(void);
	virtual ~Exporter_NPCSpawnGroup(void);

	virtual void PrePorcess(INT& GlobalAutoID);
	virtual void WriteHeader(FBufferArchive& ar  );
	virtual void WriteData(FBufferArchive& ar);
	
	virtual	int GetBlockNumber() const { return 2; }

	void		IniData();
	void     CleanUp();
private:
	SCENE_FILE_BLOCK_HEADER* arBlkHdrs[2];

	std::vector< ANPCSpawnPoint * > SpawnPointList; 
	FBufferArchive			memPointAr;

	std::vector< AMMONPCSpawnGroup * > SpawnGroupList;
	FBufferArchive			memGroupAr;
};

#endif

#endif
