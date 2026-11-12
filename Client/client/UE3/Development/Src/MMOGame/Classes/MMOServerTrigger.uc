/****
****
* @todo
* This block needs to be removed
*****/
class MMOServerTrigger extends Actor implements(IServerTrigger, IAbilityDamageable)
	placeable
	native
	ClassGroup(__Server_Old__)
	hidecategories(Object,Debug,Advanced,Physics,CollisionChanged,Attachment,DisplayDebug);

var(MMOServer) editconst int TriggerID;
var(MMOServer) ETriggerType TriggerType;
var(MMOServer) ETrapEventType TrapEventType;

var(MMOServer) ETriggerInitState InitTriggerState;
//var(MMOServer) float CollisionRadius;
//var(MMOServer) float CollisionHeight;

var(MMOServer) array<IServerTrigger> LinkedTrigger;

var(MMOServer_Delay) int DelayTime;

// Associated blocking box
var(MMOServer_Blocking) array<DynamicBlockingVolume> AssociatedBlockingVolume;

var(MMOServer_NPCSpawn) array<NPCSpawnPoint> NPCSpawnPoints;

var(MMOServer_NPCDead) NPCDeadEvent NPCDeadEvent;

var(MMOServer_EventArea) array<IServerArea> LinkedEventArea;

var(MMOServer_Skill) array<AbilityEvent> AbilityList;

var(MMOServer_Teleport) TeleportEvent TeleportEvent;
var(MMOServer_LoadingTrigger) int LoadingTime;
var(MMOServer_OpenTeleport) array<TeleportOrig> LinkedTeleportList;

var(MMOClient) CylinderComponent CollisionCylinder;
var(MMOClient) DrawCylinderComponent EditorCollisionCylinder;
var(MMOClient) StaticMeshComponent EditorStaticMeshComponent;

simulated event PostBeginPlay()
{
	super.PostBeginPlay();

	RegisterToFacade();

	//CollisionCylinder.SetCylinderSize(CollisionRadius, CollisionHeight);

	//EditorCollisionCylinder.CylinderRadius = CollisionRadius;
	//EditorCollisionCylinder.CylinderTopRadius = CollisionRadius;
	//EditorCollisionCylinder.CylinderHeight = CollisionHeight;

}

event TakeDamage(int DamageAmount, Controller EventInstigator, vector HitLocation, vector Momentum, class<DamageType> DamageType, optional TraceHitInfo HitInfo, optional Actor DamageCauser)
{
	super.TakeDamage(DamageAmount, EventInstigator, HitLocation, Momentum, DamageType, HitInfo, DamageCauser); 
	`log("_________ MMOServerTriggerObject: TakeDamage" @DamageCauser);
}

// Get faction ID to calculate damage
event int GetFriendlyMask()
{
	return 0;
}

// if processed return true, else false
event ProcessDamage(DamageData Damage)
{
	TakeDamage(Damage.DamageValue, none, vect(0,0,0), vect(0,0,0), none);
}

event bool IsDead()
{
	return false;
}

native function ETriggerType GetTriggerType();
native function ETrapEventType GetTrapEventType();

// Register with Facade for lookup
native function RegisterToFacade();

native function int GetTriggerID();
native function SetAutoID(int id);

// Get the associated blocking box
native function array<DynamicBlockingVolume> GetBlockingVolumes();

// Get object size
native function Box GetObjectServerBounds();

// get object position
native function vector GetObjectServerPosition();

// Whether the initial state is open, closed, or unavailable
native function ETriggerInitState GetServerInitState();

// Return an identifier name for debugging
native function string GetClientName();

// get the associated NPC spawn point
native function array<NPCSpawnPoint> GetNPCSpawnPoints();

// Get associated mechanisms, to activate them after the current event area completes
native function array<IServerTrigger> GetLinkedTrigger();

native function int GetDelayTime();

// Trigger ability list
native function array<AbilityEvent> GetAbilityList();

// Trigger ability list
native function TeleportEvent GetTeleportEvent();

native function array<IServerArea> GetLinkedEventArea();
native function int GetLoadingTime();

native function NPCDeadEvent GetNPCDeadEvent();

native function array<TeleportOrig> GetLinkedTeleportList();

cpptext
{
	virtual void MarkPackageDirty( UBOOL InDirty = 1 ) const;
}

DefaultProperties
{
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
		RBChannel=RBCC_Nothing
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
	Components.Add(Sprite);

	// Add debug range wireframe - ranged attack range
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

	//CollisionHeight=300
	//CollisionRadius=400

	//DrawScale=0.2
	//bCollideActors=TRUE
	//bBlockActors=true
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
		RBChannel=RBCC_Nothing
	End Object
	Components.Add(StaticMeshComponent0)
	EditorStaticMeshComponent=StaticMeshComponent0

	Begin Object Class=ServerTriggerEditorComponent  Name=EditorComponent0
	End Object
	Components.Add(EditorComponent0)
	
}
