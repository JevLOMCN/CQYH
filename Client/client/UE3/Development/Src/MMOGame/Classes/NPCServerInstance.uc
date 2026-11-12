//
// NPCServerInstance
//
class NPCServerInstance extends MMOActor implements(IUniqueID)
	placeable
	dependson(NPC)
	native
	hidecategories(AI,Object,Debug,Display,Attachment,Collision,Physics,Advanced,Mobile,AvatarData,SoundFX,Speed,MMOUI,MMOActor,MMO,Anim,MMOAttach,Pawn,AICmdHistoryItem,Camera)
	ClassGroup(__Server__);

var(MMOServer) editoronly init string NameID;
//var deprecated NPC Template;
var() editoronly NPCTemplate TemplateNew;
//var	SkeletalMeshComponent SkeletalMeshComponent;

var editoronly editconst int   AutoServerID;
var() editoronly bool  IsAutoSpawn;
var() editoronly bool  IsAutoReSpawn;
var() editoronly ENPC_QUALITY Quality;
var() editoronly int Range;
var() editoronly IntervalTimeData SpawnTimeServer <DisplayName="SpawnTime">;
var() editoronly IntervalTimeData MinRespawnTime;
var() editoronly IntervalTimeData MaxRespawnTime;
var() editoronly IntervalTimeData ReviveTime;
//var() NPCAIData AIData;
var(MMOServer) editoronly INT AIData;
var() editoronly int CurrentHP;
var() editoronly int CurrentMP;
var() editoronly bool CanDropItem;
var() editoronly int LevelOffset;
var() editoronly init string DeadScript;

var(MMOServer) editoronly editconst int ServerUniqueID;			//song@snda.com
var(MMOServer) editoronly MMOServerPoint PatrolOriginServerPoint;
var(MMOServer) editoronly EAI_PATROL_STYLE PatrolStyle;
var(MMOServer) editoronly int FollowHostID;
var(MMOServer) editoronly int FollowOffsetX;
var(MMOServer) editoronly int FollowOffsetY;

var editoronly SpriteComponent EditorSprite;

simulated event PostBeginPlay()
{
	super.PostBeginPlay();

	if(class'Engine'.static.IsEditor())
	{
		RefresAvatarPIE();
		Mesh.SetHidden(false);
	}
	else
	{
		//CollisionComponent = none;
	}

	SetCollisionSize(1, 1);
	SetCollisionType(COLLIDE_NoCollision);
	SetCollision(false, false, true);
}

native function InitItems();
native final function RefresAvatarPIE();

event InitAnimNodes()
{
}

event RefreshCollision()
{
}

native function AvatarData GetAvatarData();
native function InitUniqueID();					//song@snda.com
native function INT GetUniqueID();
native function SetUniqueID(int id);

native function SetServerLocation(Vector NewLoc);	// zhaojun@snda

cpptext
{
	virtual INT GetServerID(){ return AutoServerID; }
	virtual INT GetObjectServerType() const { return (INT)kotSingleNpc; }

	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent);

	void RefresAvatarInEditor();

	virtual void PostDuplicate() { Super::PostDuplicate(); SetUniqueID(-1); InitUniqueID(); }

	virtual UBOOL NeedsLoadForClient() const; // oscar@snda never loaded in game
}

DefaultProperties
{
// 	Begin Object Class=SkeletalMeshComponent Name=SkeletalMeshComponent0
// 		Animations=None
// 		//SkeletalMesh=SkeletalMesh'Demo_Characters.Mesh.Man'
// 		//AnimSets.Add(AnimSet'Player_Male_Anim.AS_Male_Base')
// 		//PhysicsAsset=PhysicsAsset'Player_Male_Anim.Physics.BasePhysics'
// 		AnimTreeTemplate=AnimTree'Player_AnimTree.AT_Player'
// 		LightingChannels=(Gameplay_2=TRUE)
// 		bHasPhysicsAssetInstance=false
// 		//bUpdateKinematicBonesFromAnimation=true
// 		//bUpdateJointsFromAnimation=true
// 		//PhysicsWeight=1.0
// 		//BlockRigidBody=true
// 		//CollideActors=true
// 		//BlockActors=false
// 		BlockZeroExtent=true
// 		//BlockNonZeroExtent=false
// 		//Scale3D=(X=2,Y=2,Z=2)
// 		//bAllowAmbientOcclusion=FALSE
// 		RootMotionMode=RMM_Ignore
// 		RBChannel=RBCC_Untitled3
// 	End Object
// 	CollisionComponent=SkeletalMeshComponent0
// 	//SkeletalMeshComponent=SkeletalMeshComponent0
// 	Components.Add(SkeletalMeshComponent0)
// 	Mesh=SkeletalMeshComponent0

	Begin Object Class=SpriteComponent Name=SpriteHUD
		Sprite=Texture2D'MMOEditor.MonsterSpawnPoint'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Server"
		Translation=(X=0,Y=0,Z=200);
	End Object
	EditorSprite=SpriteHUD
	Components.Add(SpriteHUD);

	bHidden=true
	PickPriority=EPP_Max

	CanDropItem=true
	//Quality=E_nq_Random
}
