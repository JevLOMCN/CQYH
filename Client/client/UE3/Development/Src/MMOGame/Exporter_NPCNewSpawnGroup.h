#ifndef _EXPORTER_NPCNEWSPAWNGROUP_H_
#define _EXPORTER_NPCNEWSPAWNGROUP_H_

#if !SHIPPING_PC_GAME

#include "ExportServerData.h"

//illegal zero-sized array needed by the server data structure
#pragma warning(disable:4200)

class Exporter_NPCNewSpawnGroup : public IServerSceneExporter
{
public:
	Exporter_NPCNewSpawnGroup(void);
	virtual ~Exporter_NPCNewSpawnGroup(void);

	virtual void PrePorcess(INT& GlobalAutoID);
	void WriteToClientXML( INT& GlobalAutoID );

	void ExportSpawnPoint( ANPCNewSpawnPoint* sppt );
	void ExportSmallGroup( FSpawnSmallGroup* sGrp );
	void ExportMediumGroup( ANPCNewSpawnGroupMediumActor* mGrp );

	void RemoveNullRef(ANPCNewSpawnGroupActor* spawnGroup);
	void RemoveNullRef(ANPCNewSpawnGroupMediumActor* spawnMedGroup);

	virtual void WriteHeader(FBufferArchive& ar  );
	virtual void WriteData(FBufferArchive& ar);
	
	virtual	int GetBlockNumber() const { return 1; }

private:
	
	SCENE_FILE_BLOCK_HEADER* SpawnGroupHeader;

	FBufferArchive			MemAr;
	ANPCNewSpawnGroupActor* CurrentLargeGroup;
	ANPCNewSpawnGroupMediumActor* CurrentMedGroup;
	INT CurrentMedGrpIdx;
	INT CurrentSmallGrpIdx;
};

#endif

#endif
