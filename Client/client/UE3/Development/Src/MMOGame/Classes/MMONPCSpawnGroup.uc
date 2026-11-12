// MMONPCSpwanGroup
//NPC spawn point group
//hemu@snda 120424

class MMONPCSpawnGroup extends Actor
	dependson(NPCSpawnPoint)
	placeable
	native
	hidecategories(Object,Debug,Advanced,Physics,CollisionChanged,Attachment,DisplayDebug)
	ClassGroup(__Server_Old__);

var editoronly Texture2D SpawnGroupSpriteTex;
// auto-numbered ID for server use
var(MMOServer_NPCSpawn) editoronly editconst int AutoServerID;

// auto spawn, must be set to false if controlled by Kismet
var(MMOServer_NPCSpawn) editoronly bool IsAutoSpawn;
//auto respawn, likewise, must be set to false if controlled by Kismet
var(MMOServer_NPCSpawn) editoronly bool IsAutoReSpawn;

// manually configured spawn group ID, NPCs will be grouped under this ID
//var(MMOServer_NPCSpawn) int NPCSpawnGroupId;
//var(MMOServer_NPCSpawn) editoronly int   RespawnTime;
var(MMOServer_NPCSpawn) editoronly int RespawnTimeMin;
var(MMOServer_NPCSpawn) editoronly int RespawnTimeMax;
//manually configured monster spawn chance
var(MMOServer_NPCSpawn) editoronly int MaxConcurrentPoint;
//associated spawn point
var(MMOServer_NPCSpawn) editoronly array<NPCSpawnPoint> NPCSpawnPoints;

//display component
var(MMOClient) editoronly CylinderComponent CollisionCylinder;
var(MMOClient) editoronly DrawCylinderComponent EditorCollisionCylinder;
var(MMOClient) editoronly StaticMeshComponent EditorStaticMeshComponent;

var editoronly SpriteComponent EditorSprite;
var editoronly MMONpcSpawnGroupEditorComponent EditorComponent;


// get the associated NPC spawn point
native function array<NPCSpawnPoint> GetNPCSpawnPoints();
// get object position
native function vector GetObjectServerPosition();

cpptext
{
	
	virtual INT GetServerID(){ return AutoServerID; }
	virtual INT GetObjectServerType() const { return (INT)kotSpawnGroup; }

	#if WITH_EDITOR
	virtual void CheckForErrors();
	#endif

	virtual UBOOL NeedsLoadForClient() const { return FALSE; } // oscar@snda never loaded in game

}
native function    IniSpirite();
simulated event PostBeginPlay()
{
	super.PostBeginPlay();
	IniSpirite();
}
DefaultProperties
{
	SpawnGroupSpriteTex=Texture2D'MMOEditor.ServerTrigger'

	Begin Object Class=CylinderComponent Name=CollisionCylinder0
		CollisionRadius=40
		CollisionHeight=200
		BlockNonZeroExtent=true
		BlockZeroExtent=true
		BlockActors=true
		CollideActors=true
		LightingChannels=()
		CastShadow=false
		bCastDynamicShadow=false
		bCastHiddenShadow=false
		bCastShadowAsTwoSided=false
		bCastStaticShadow=false
		bAcceptsDecals=false
		bAcceptsDecalsDuringGameplay=false
		bAcceptsDynamicDecals=false
		bAcceptsDynamicDominantLightShadows=false
		bAcceptsDynamicLights=false
		bAcceptsLights=false
		bAcceptsStaticDecals=false
	End Object
	CollisionComponent=CollisionCylinder0
	Components.Add(CollisionCylinder0)
	CollisionCylinder=CollisionCylinder0

	Begin Object Class=SpriteComponent Name=Sprite
		Sprite=Texture2D'MMOEditor.ServerTrigger'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Server"
		//Scale=0.2
	End Object
	EditorSprite=Sprite
	Components.Add(Sprite);

	Begin Object Class=DrawCylinderComponent  Name=DebugRangeFar0
		CylinderColor=(R=0,G=255,B=0,A=255)
		CylinderRadius=40.0
		CylinderTopRadius=40.0
		CylinderHeight=200.0
		CylinderHeightOffset=0.0
		CylinderSides=32
		bDrawWireCylinder=true
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		HiddenGame=true
		LightingChannels=()
		CastShadow=false
		bCastDynamicShadow=false
		bCastHiddenShadow=false
		bCastShadowAsTwoSided=false
		bCastStaticShadow=false
		bAcceptsDecals=false
		bAcceptsDecalsDuringGameplay=false
		bAcceptsDynamicDecals=false
		bAcceptsDynamicDominantLightShadows=false
		bAcceptsDynamicLights=false
		bAcceptsLights=false
		bAcceptsStaticDecals=false
	End Object
	Components.Add(DebugRangeFar0)
	EditorCollisionCylinder=DebugRangeFar0

	Begin Object Class=StaticMeshComponent  Name=StaticMeshComponent0
		StaticMesh=FracturedStaticMesh'GamePlayNodeBuddies.3D_Icons.NodeBuddy__BASE_TALL_FRACTURED'	
		BlockNonZeroExtent=true
		BlockZeroExtent=true
		BlockActors=true
		CollideActors=true
		LightingChannels=()
		CastShadow=false
		bCastDynamicShadow=false
		bCastHiddenShadow=false
		bCastShadowAsTwoSided=false
		bCastStaticShadow=false
		bAcceptsDecals=false
		bAcceptsDecalsDuringGameplay=false
		bAcceptsDynamicDecals=false
		bAcceptsDynamicDominantLightShadows=false
		bAcceptsDynamicLights=false
		bAcceptsLights=false
		bAcceptsStaticDecals=false
	End Object
	Components.Add(StaticMeshComponent0)
	EditorStaticMeshComponent=StaticMeshComponent0

	Begin Object Class=MMONpcSpawnGroupEditorComponent  Name=EditorComponent0
	End Object
	EditorComponent=EditorComponent0
	Components.Add(EditorComponent0)

	IsAutoSpawn=true
	//NPCSpawnGroupId=1
}
