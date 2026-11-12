//
// NPCNewSpawnGroupActor
//
class NPCNewSpawnGroupActor extends NPCNewSpawnGroupEditorActor implements(IUniqueID)
	//dependson(TypeDefine)
	placeable
	native
	ClassGroup(__Server__);

 var(MMOServer) editoronly editinline int ServerUniqueID;			//song@snda.com
 var(MMOServer) editoronly int MaxDieCount;
 var(MMOServer) editoronly int EliteMin;
 var(MMOServer) editoronly int EliteMax;
 var(MMOServer) editoronly IntervalTimeData EliteInterval;
 
 var(MMOServer) editoronly int ChampionMin;
 var(MMOServer) editoronly int ChampionMax;
 var(MMOServer) editoronly IntervalTimeData ChampionInterval;
 
 var(MMOServer) editoronly init string DeadTree;
 var(MMOServer) editoronly array<NPCNewSpawnGroupMediumActor> MediumGroupList;

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
 native function InitUniqueID();			//song@snda.com
 native function INT GetUniqueID();
 native function SetUniqueID(int id);

DefaultProperties
{	
	ChampionInterval=(Hours=3)
	EliteInterval=(Hours=1)
}
