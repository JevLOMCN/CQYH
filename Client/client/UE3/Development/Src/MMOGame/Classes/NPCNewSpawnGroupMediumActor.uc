//
// NPCNewSpawnGroupMediumActor
//
class NPCNewSpawnGroupMediumActor extends NPCNewSpawnGroupEditorActor implements(IUniqueID)
	dependson(TypeDefine)
	placeable
	native
	ClassGroup(__Server__);

struct native SpawnSmallGroup
{
	var() int Weight;
	var() bool DynamicRespawnTime;
	var() array<NPCNewSpawnPoint> SpawnPointList;

	structdefaultproperties
	{
		DynamicRespawnTime = false;
		Weight=1.0f
	}
};

//var(MMOServer) init string NameID;
var(MMOServer) editoronly editinline int ServerUniqueID;			//song@snda.com
var(MMOServer) editoronly bool AutoSpawn;
var(MMOServer) editoronly bool AutoRespawn;
var(MMOServer) editoronly IntervalTimeData MinRespawnTime;
var(MMOServer) editoronly IntervalTimeData MaxRespawnTime;
var(MMOServer) editoronly int LevelOffset;
var(MMOServer) editoronly init string DeadTree;
var(MMOServer) editoronly array<SpawnSmallGroup> SmallGroupList;
var(MMOServer) editoronly EENUM_NPC_GROUP_TYPE SpawnNpcGroupType;

cpptext
{
	virtual void DrawEditorDynamicElements(FPrimitiveDrawInterface* PDI,const FSceneView* View,UINT DPGIndex,DWORD Flags = 0);
	virtual void PostDuplicate() { Super::PostDuplicate(); SetUniqueID(-1); InitUniqueID(); }
	virtual UBOOL NeedsLoadForClient() const { return FALSE; } // oscar@snda never loaded in game

#if WITH_EDITOR
	virtual void CheckForErrors();
#endif
}

simulated event PostBeginPlay()
{
	super.PostBeginPlay();
}
native function InitUniqueID();					//song@snda.com
native function INT GetUniqueID();
native function SetUniqueID(int id);

DefaultProperties
{
	MinRespawnTime=(Seconds=0)
	MaxRespawnTime=(Seconds=0)
	SpawnNpcGroupType=ENpcGroupType_Invalid
	Begin Object Name=DebugRangeFar0
		CylinderColor=(R=255,G=128,B=0,A=255)		
	End Object
}
