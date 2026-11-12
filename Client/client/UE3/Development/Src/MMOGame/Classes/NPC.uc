//
// main NPC base class, handles character animation management
//

class NPC extends MMOActor implements(IMMOActorHUD, IExcelable,IReferenceID, IInteractableActor)
	config(MMOActor)
	placeable	
	native
	hidecategories(MMO_Client_AI,MMO_ClientClimb,MMOActor,MMODebug,AI,ActiveEffect,PassiveEffect,Camera,Debug,Display,Attachment,Physics,Advanced,Object,Movement)
	dependson(TypeDefine, NPCTemplate);

var config float SoundCDTime;
var float CurrentOpenSoundCDTime;
var float CurrentCloseSoundCDTime;
var float CurrentAnimSoundCDTime;

var NPCTemplate Template;

var MMOActor Master;


//determined by dungeon difficulty type
// enum EGameDifficulty
// {
// 	EGD_Easy              <DisplayName="EEGD_Easy">,
// 	EGD_Normal         <DisplayName="EEGD_Normal">,
// 	EGD_Nightmare    <DisplayName="EEGD_Nightmare">,
// 	EGD_Hell               <DisplayName="EEGD_Hell">,
// 	EGD_Inferno          <DisplayName="EEGD_Inferno">,
// };
//difficulty level po
//var(MMOServer) EGameDifficulty Difficulty;

var(MMO_Debug) bool bDebugRanges      <DisplayName="bDebugRanges">;
//var(MMO_UI) int DefaultDialogID <DisplayName="DefaultDialogID">;
var(MMO_UI) string DefaultDialogText <DisplayName="DefaultDialogText">;



//// controller template
// var(MMO_Client_AI) AIController NPCAIControllerTemplate;

var(MMO_Client_AI) float m_fSightRange    <DisplayName="m_fSightRange">;
var(MMO_Client_AI) float m_fPatrolRange   <DisplayName="m_fPatrolRange">;
var(MMO_Client_AI) float m_fTraceRange    <DisplayName="m_fTraceRange">;    // give up tracing players out of this range

var(MMO_Client_AI) bool  m_bIsPossitive   <DisplayName="m_bIsPossitive">;
var(MMO_UI) bool    m_bIsWarehouse  <DisplayName="m_bIsWarehouse">;
var(MMO_UI) string  IdentityName <DisplayName="IdentityName">;
var(MMO_UI) Texture2D   IdentityIcon <DisplayName="IdentityIcon">;

var transient array<Quest> AcceptableQuestList; // list of acceptable quests
var transient array<Quest> CompassableQuestList; // list of quests that can be turned in

var     array<int>	    AffixList;

//var() array<int>    NormalAttackAbilityIds  <DisplayName="normal attack ability ID list">;
//var() float AbilityUseRate      <DisplayName="non-normal ability use rate">;


var DrawCylinderComponent   TraceRangeCylinder;

//// remove, replaced by ability animation
////var(MMOAnim) name    m_NameAttackAnim          <DisplayName="attack animation">;
//var(MMOAnim) name    m_NameAttackIdleAnim      <DisplayName="attack idle">;
//var(MMOAnim) name    m_NameHurtAnim            <DisplayName="hurt animation">;
//var(MMOAnim) name    m_NameBeatBackAnim        <DisplayName="knockback animation">;
var name    m_NameClimbAnim           <DisplayName="m_NameClimbAnim">;
var name    m_NameClimbJumpAnim       <DisplayName="m_NameClimbJumpAnim">;
var name    m_NameClimbOverAnim       <DisplayName="m_NameClimbOverAnim">;
var name    m_NameJumpDown            <DisplayName="m_NameJumpDown">;
var name    m_NameLand                <DisplayName="m_NameLand">;


// idle action variables
enum EWanderType
{
	EWanderType_Stand   <DisplayName="EEWanderType_Stand">,          // stand at the same location
	EWanderType_Roam    <DisplayName="EEWanderType_Roam">,           // roaming around a certain area
	EWanderType_Patrol  <DisplayName="EEWanderType_Patrol">,
	EWanderType_Follow  <DisplayName="EEWanderType_Follow">
};

var(MMO_Client_AI) EWanderType   WanderType  <DisplayName="WanderType">;
var Vector          m_vecOrigLoc;   // origin of the NPC's roaming
var(MMO_Client_AI) float         AttackInterval  <DisplayName="AttackInterval">;  // interval between 2 attack animations
////var() int           NPCHealth;       // expose the health property for editor

//var() float         m_fEscapeHealthRate <DisplayName="flee health rate">;
//var() float         m_fEscapeDistance   <DisplayName="flee distance">;

//var () float        MinDistanceToTarget;
//var () float        MaxDistanceToTarget;

var(MMO_Client_AI) array< PathNode >     PatrolPath    <DisplayName="PatrolPath">;
//var transient int           CurrentNodeIdx;

//var() float         RandomWanderMaxDistance;
var transient Vector        SpawnLocation;
var transient Rotator       SpawnRotation;

//var() float         AIUpdateDelta       <DisplayName="AI update interval">;

var	NPC                     NPCToFollow;
var Vector                  TransToLead;     // translation to the npc to follow
var(MMO_Client_AI) array< NPC >          Follower      <DisplayName="Follower">;

// Low HP range reference value
var float LowHealthParticlePercentThreshold;

// Health bar color when healthy
var Vector HealthyColor;

// Health bar color when hurt
var Vector UnhealthyColor;

//// particle component
////var(MMO) ParticleSystemComponent LowHealthParticleEffect;

// Health bar material template
var MaterialInstanceConstant TheHealthHUDMatTemplate;

// Health bar material
var transient MaterialInstanceConstant MyHealthHUDMatInstance;

// Whether to draw the health bar
var bool EnableDrawHealthBar;

// Health bar scale
var float HealthBarScale;

// Health bar offset
var Vector HealthBarPositionOffset;

var String QuestIconName;

//// time of last attack received
var float LastTakeDamageTime;

var(MMO_Gameplay)      int     DropMoneyCountMin  <DisplayName="DropMoneyCountMin">;
var(MMO_Gameplay)      int     DropMoneyCountMax  <DisplayName="DropMoneyCountMax">;
var(MMO_Gameplay)      int     DropMoneyMin  <DisplayName="DropMoneyMin">;
var(MMO_Gameplay)      int     DropMoneyMax  <DisplayName="DropMoneyMax">;

// climb
var(MMO_ClientClimb) float       JumpUpDistance   <DisplayName="JumpUpDistance">;
var(MMO_ClientClimb) float       ClimbUpDistance  <DisplayName="ClimbUpDistance">;

// temporary parameter: in the Demo the NPC face renders incorrectly due to size issues, so a static mesh is used temporarily
//var( MMOTest )  bool    UseStaticMesh   <DisplayName="use static mesh">;
//var( MMOTest )  StaticMeshComponent TempStaticMesh ;
//var( MMOTest)   StaticMeshComponent    HeadUpComponent1;
//var( MMOTest)   StaticMeshComponent    HeadUpComponent2;

var(MMO_Client_AI)   bool    CouldJumpDown           <DisplayName="CouldJumpDown">;
var(MMO_Client_AI)   float   HealthPercentToJumpDown <DisplayName="HealthPercentToJumpDown">;

var(MMO_Client_AI)   bool    CryForHelp              <DisplayName="CryForHelp">;
var(MMO_Client_AI)   float   HealthPercentToCFH      <DisplayName="HealthPercentToCFH">;
var(MMO_Client_AI)   float   RangeOfHelp             <DisplayName="RangeOfHelp">;

//var()   float   CriticalBeatBackDistance        <DisplayName="critical knockback distance">;
//var()   float   NormalBeatBackDistance          <DisplayName="normal knockback distance">;

//var()   bool    NeedTriggerDeath                <DisplayName="trigger death event">;

var bool    bPendingClimb;
var Volume  PendingClimbVolume;


//---------------------------------------------------------------
//---------------------------------------------------------------
// code for effective functionality is written below
//---------------------------------------------------------------
//---------------------------------------------------------------

enum EQuestHUDStatus
{
	EQHUDS_Acceptable,
	EQHUDS_WillAcceptable,
	EQHUDS_CanSubmit,
	EQHUDS_NotValidForSubmit,
	EQHUDS_None,
};

var int MinimapIconID; //used by minimap UI
var int SceneHintID; //name in the scene
var int WorldmapIconID; //used by minimap UI

var transient EQuestHUDStatus HeadUpType;

var(MMOAnim) bool   HideWeaponWhenRunning   <DisplayName="HideWeaponWhenRunning">;

var   StaticMeshComponent    HeadUpComponent;

var Vector  ServerMoveDest; // debug only

//---------------------------------------------------------------
// destructible parts
//---------------------------------------------------------------
//// currently bound destructible parts, deleted by jimzhang 2012.7.11
//var array<DestructablePartAttachInfo> AttachedDestructableParts;

// initial list of destructible parts
var(MMOAttach) editinline array<DestructablePartAttachInfo> InitAttachDestructableParts <DisplayName="InitAttachDestructableParts">;


//---------------------------------------------------------------
// state presentation
//---------------------------------------------------------------


//var(MMOStatusProc) editinline DeathProcDataSet     DeathBeginProcSet;      

//// death presentation
//var(MMOStatusProc) editinline array<StatusProcData> DeathBeginProcList;
//var(MMOStatusProc) editinline array<StatusProcData> DeathEndProcList;

var(MMOStatusProc) editinline array<StatusProcData> SpawnProcList;
var(MMOStatusProc) editinline array<StatusProcData> TeleportProcList;

var(MMOStatusProc) editinline array<StatusProcData> RebirthProcList;

var(MMOStatusProc) editinline array<ActionProcData> ActionProcList;

var(MMOStatusProc) editinline array<StatusProcData> TickProcList;



var(MMOServer)  int   NPCID <DisplayName="TemplateID"|ExportExcel="0">;
var(MMOServer) name   NameID <DisplayName="TemplateNameID"|ExportExcel="1">;
var(MMOServer) string   DisplayName <ExportExcel="2">;
var(MMOServer) int Level <ExportExcel="3">;
var(MMOServer) int MaxHp <ExportExcel="4">;
var(MMOServer) int MaxMp <ExportExcel="5">;
var(MMOServer) int Speed <ExportExcel="6">;
var(MMOServer) int Radius <ExportExcel="7">;
var(MMOServer) int ServerSpawnTime <DisplayName="SpawnTime"|ExportExcel="8">;
var(MMOServer) int ReviveTime <ExportExcel="9">;
var(MMOServer) int CorpseTime <ExportExcel="10">;
var(MMOServer) int RespawnTimeMin <ExportExcel="11">;
var(MMOServer) int RespawnTimeMax <ExportExcel="12">;
var(MMOServer) int Exp <ExportExcel="13">;
// range is 0-16
var(MMOServer) int ForceID <ExportExcel="14">;
var(MMOServer) string NPCScriptName <DisplayName="NPCScriptName"|ExportExcel="15">;

var(MMOServer) editfixedsize array<DropGroupSet> DropItemSetList <DisplayName="DropSet"|ExportExcel="16">;
//var(MMOServer) int DropItemSetList <DisplayName="DropSet"|ExportExcel="16">;

var(MMOServer) int WeaponPhyAtkHi <DisplayName="WeaponPhyAtkHi"|ExportExcel="17">;
var(MMOServer) int WeaponPhyAtkLo <DisplayName="WeaponPhyAtkLo"|ExportExcel="18">;
var(MMOServer) int WeaponMagAtkHi <DisplayName="WeaponMagAtkHi"|ExportExcel="19">;
var(MMOServer) int WeaponMagAtkLo <DisplayName="WeaponMagAtkLo"|ExportExcel="20">;
var(MMOServer) int ShopID <DisplayName="ShopID"|ExportExcel="21">;

var(MMOServer) bool ImmunePassiveMove <DisplayName="ImmunePassiveMove"|ExportExcel="22">;
var(MMOServer) bool Invincible <DisplayName="Invincible"|ExportExcel="23">;
var(MMOServer) bool AllwaysRun <ExportExcel="24">;
var(MMOServer) int Pattern <ExportExcel="25">;
var(MMOServer) ENPC_QUALITY Quality <ExportExcel="26">;
var(MMOServer) ECHAR_SPECIES_TYPE SpeciesType <ExportExcel="27">;
var(MMOServer) bool Affix <ExportExcel="28">;

//Start used for SingleNPCSpawnPoint
var                        int   AutoServerID;
var(MMOServer)  bool  IsAutoSpawn;
var(MMOServer)	 bool  IsAutoReSpawn;
//End used for SingleNPCSpawnPoint

var(anim) transient float LeftTimeToCheckAboutRestAnim; 
var int NextSoundTime;          // note: this variable is unsigned when in use

var EActorClassType OverrideActorType;

native function bool IsMonster() const;
native function EMMOCursorType GetMouseCursor();

native function vector GetInteractActorLocation();
native function float GetMaxInteractRadius();
native function DoInteract();
native function bool CanInteract() const;
native function SetSilhouette( bool show, Vector sc = vect( 1.f, 1.f, 0.f ) );
native function RefreshCollision();		//Bruce@snda
//---------------------------------------------------------------
// IMMOActorHUD
//---------------------------------------------------------------

function bool OnlyDrawHUDForOwner(){return false;}

native function DrawHUDPostGFx( HUD H );

native function bool DrawMyHUD( MMOHud H );

native final function DebugNativePostBeginPlay();

native function PlayRandomAnim();

native function ResetIdleTimer();

native function SwitchFightIdle(bool bFight);

native function SetDead();

native function string GetDebugInfo();

native function InitBodySize();

native function InitNpcByTemplate(NPCTemplate InTemplate);
// Initialization function
simulated event PostBeginPlay()
{
	DebugNativePostBeginPlay();

	super.PostBeginPlay();

	if(Mesh==none )
	{
		Mesh =  new() class'SkeletalMeshComponent';
	}

	bPushedByEncroachers = false;

	bDead = false;


	//lcc.Dynamic = true;
	//lcc.CompositeDynamic = true;
	//lcc.Gameplay_4 = true;

	//Mesh.SetLightingChannels(lcc);
	Mesh.SetLightEnvironment(LightEnvironment);

	//ActorAttributes = new(self) class'ActorAttribute'(ActorAttributes);
	
	//add by jimzhang make NPC sect = faction, to be consistent with characters
	ActorAttributes.Faction = ForceID;

	if(Controller == none)
	{
		SpawnDefaultController();
	}


	SpawnLocation = Location;
	SpawnRotation = Rotation;

	m_vecOrigLoc = Location;

	//ActorAttribute.MaxHP = 1000;
	//ActorAttributes.HP = 1000;
	//create a new MIC of the healthbar for this object, so that it can uniquely set the MIC's parameters to its own values.
	HealthMax = ActorAttributes.GetMaxHp();
	Health = ActorAttributes.GetCurHp();
	MyHealthHUDMatInstance = new(self) class'MaterialInstanceConstant';
	MyHealthHUDMatInstance.SetParent(TheHealthHUDMatTemplate);

	SetPhysics(PHYS_Walking);

	if( Template != none && Template.WindStrength > 0.1f )
	{
		`Log( "Add interactive wind for npc"@self );
		AddInteractiveWind( Template.WindStrength );
	}

	//class'MMOHUD'.static.AddDrawHUD(self);
}

simulated event Destroyed()
{
	super.Destroyed();
	//class'MMOHUD'.static.RemoveDrawHUD(self);
}

//// there will be a lot of initialization work, add as needed by actual features, useless properties are not copied
//event InitFromNpcTemplate(NPCTemplate InTemplate)
//{
//	local DestructablePartAttachInfo asm;	
//	local ParticleSystemAttachInfo psai;
//	local int i;

//	Template = InTemplate;

//	// when spawn or review, play actions
//	if(SceneObjectAppearType == 3 || SceneObjectAppearType == 4)
//	{
//		PlayActions(Template.PreSpawnActions);
//	}

//	 //PickPriority = InTemplate.PickPriority;
//	 UseingAvatar = InTemplate.AvatarTemplate;
//	 //RotationSpeed = InTemplate.RotationSpeed;

//	 NPCIcon = InTemplate.NPCIcon;
//	 HUDDisplayIcon = InTemplate.HUDDisplayIcon;
//	IdentityIcon = InTemplate.IdentityIcon;
//	IdentityName = InTemplate.IdentityName;

//	 ActorName = InTemplate.ActorName;

//	 //ActorAttributes = new class'ActorAttribute';// InTemplate.ActorAttributes.Class (InTemplate.ActorAttributes);

//	 PawnAnimInfo = InTemplate.PawnAnimInfo;

//	 //EmitterScaleValue = InTemplate.EmitterScaleValue;
//	 //Level = InTemplate.Level;
//	 //ActorAttributes.LevelAttributes.Level =InTemplate.Level ;

//	 // npc attributes
//	 //Difficulty = InTemplate.Difficulty;
//	 //DefaultDialogID = InTemplate.DefaultDialogID;
//	 DefaultDialogText = InTemplate.DefaultDialogText;
//	 if(OverrideActorType != EACT_None)
//	 {
//		ActorType = OverrideActorType;
//		OverrideActorType = EACT_None;
//	 }
//	 else
//	 {
//		ActorType = InTemplate.ActorType;
//	 }
//	 //m_bIsWarehouse = InTemplate.m_bIsWarehouse;

//	 DisplayName = InTemplate.GetDisplayName();
//	 //HideWeaponWhenRunning = InTemplate.HideWeaponWhenRunning;


//	 //DeathBeginProcSet = InTemplate.DeathBeginProcSet;
//	 //DeathBeginProcList = InTemplate.DeathEndProcList;
//	 //DeathEndProcList = InTemplate.DeathEndProcList;
//	 SpawnProcList = InTemplate.SpawnProcList;
//	 TeleportProcList = InTemplate.TeleportProcList;
//	 RebirthProcList = InTemplate.RebirthProcList;
//	 ActionProcList = InTemplate.ActionProcList;
//	 TickProcList = InTemplate.TickProcList;

//	 SpecialDyingActions = InTemplate.DeathComponents;

//	 LeftTimeToCheckAboutRestAnim = Rand(InTemplate.RandomRestAnimCycleTime);

//	 //InitAttachDestructableParts = InTemplate.InitAttachDestructableParts;
//	 //InitAttachParSys = InTemplate.InitAttachParSys;


//	 //DropItemSetList = InTemplate.DropItemSetList;
//	 NPCID = InTemplate.GetTemplateID();
//	 ShopID = InTemplate.ShopID;
//	 ForceID = InTemplate.ForceID;
//	m_bIsWarehouse = InTemplate.IsWarehouse;


//	if(ActorAttributes != none)
//	{
//		Health = ActorAttributes.GetMaxHp();	
//	}

//	InitItems();
//	UseingAvatar = InTemplate.AvatarTemplate;
//	//`log("Init npc from template avatar:"$InTemplate.AvatarTemplate.AvatarName);
//	RefreshAvatar();

//	RebuildMaterialControllerList();

//	RefreshCollision();

//	RefreshCollisonRate();

//	InitAnimNodes();
//	RefreshAnimSet();
//	RefreshMovementSpeed();



//	//`log("________ InitAttachDestructableParts:" @ InitAttachDestructableParts.Length);
//	for(i = 0; i < InitAttachDestructableParts.Length; ++i)
//	{
//		asm = InitAttachDestructableParts[i];
//		asm = new asm.Class ( asm );
//		AttachDestructablePart(asm);

//		//add by jimzhang 2012.7.18, restore to idle state,
//		if(asm.SkelMeshComponent != none &&   asm.SkelMeshComponent.AnimSets.Length>0)
//		{
//			asm.SkelMeshComponent.PlayAnim(  'Idle' ,0.0 , true );
//		}
//	}
//	//`log("________ InitAttachDestructableParts End!" );
	

//	//`log("________ InitAttachParSys:" @ InitAttachParSys.Length );
//	// add particle effect from template
//	for(i = 0; i < InitAttachParSys.Length; ++i)
//	{
//		psai = new InitAttachParSys[i].Class ( InitAttachParSys[i] );
//		AttachParticleSystem(psai);
//	}

//	//`log("________ InitAttachParSys End!" );


//	if(SceneObjectAppearType == ESOAT_Spawn)
//	{
//		InitProcList();
//	}

//	if(self.Template != none && self.Template.Sound_Spawn.Length > 0)
//	{
//		PlayRandomSound(self.Template.Sound_Spawn);
//	}

//	// play spawn actions
//	if(SceneObjectAppearType == 3 || SceneObjectAppearType == 4)
//	{
//		PlayActions(InTemplate.SpawnActions);
//	}

//	//PlayActions(InTemplate.PostSpawnActions);
	

//	// add interactive wind
//	if( Template.WindStrength > 0.1f )
//	{
//		//`Log( "Add interactive wind for npc"@self );
//		AddInteractiveWind( Template.WindStrength );
//	}
//}

//function InitProcList()
//{
//	//`log("________ ProcessStatusProc(SpawnProcList):" @ SpawnProcList.Length);
//	ProcessStatusProc(SpawnProcList);
//	//`log("________ ProcessStatusProc(SpawnProcList) End!" );
//}

event StatusProc_Teleport()
{
	ProcessStatusProc(TeleportProcList);
}

native function ProcessStatusProc(array<StatusProcData> procs);

	
//function ProcessStatusProc(array<StatusProcData> procs )
//{
//	local int i, j;
//	local float randNumb;

//	//`log("********************* ProcessStatusProc" @self);
//	//random///////////////////////////////////////////////////////////////////////////////////
//	randNumb = RandRange(0, 1);
//	//get the randomly selected index
//	for(i = 0; i < procs.Length; ++i)
//	{
//		if(randNumb <= procs[i].Rate)
//		{ 
//			for(j = 0; j < procs[i].Proc.Length; ++j)
//			{
//				`log("*********** process status" @ procs[i].Proc[j]);
//				procs[i].Proc[j].OnExecute(self);
//			}
//			break;
//		}
//		randNumb -= procs[i].Rate;
//	}

//	if(randNumb <= 0.0)
//	{
//		`log("********************* ProcessStatusProc error: randNumb<=0.0 " @self);
//	}
//}

event DoAction(int nActionID)
{
	local int i, j;
	for(i = 0; i < ActionProcList.Length;++i)
	{
		if(nActionID == ActionProcList[i].ActionID)
		{
			for(j = 0; j < ActionProcList[i].Proc.Length; ++j)
			{
				ActionProcList[i].Proc[j].OnExecute(self);
			}
			break;
		}
	}
}

function DoRebirth()
{
	ProcessStatusProc(RebirthProcList);
}


function SpawnReward()
{

}

// get controller
function PossessedBy(Controller C, bool bVehicleTransition)
{
	super.PossessedBy( C, bVehicleTransition );

	//Controller.PushState('Idle');     // the pawn shouldn't depend on the controller. @ming
}

//// drop weapon after death
//function ThrowWeaponOnDeath()
//{
//}


// create the physics object dropped on the ground
function CreateDropKActor(StaticMesh sm, vector position, rotator rot)
{
	local KActor KA;

	KA = Spawn(class'MMOKActor', , , position, rot, , true);
	KA.SetStaticMesh(sm);
	KA.StaticMeshComponent.SetRBChannel(RBCC_Untitled3);
	KA.StaticMeshComponent.SetRBCollidesWithChannel(RBCC_Default,TRUE);
	KA.StaticMeshComponent.SetRBCollidesWithChannel(RBCC_Pawn,TRUE);
	KA.StaticMeshComponent.SetRBCollidesWithChannel(RBCC_Vehicle,TRUE);
	KA.StaticMeshComponent.SetRBCollidesWithChannel(RBCC_Untitled1,TRUE);
	KA.StaticMeshComponent.SetRBCollidesWithChannel(RBCC_Untitled2,TRUE);
	KA.StaticMeshComponent.SetRBCollidesWithChannel(RBCC_Untitled3,FALSE);
	KA.StaticMeshComponent.SetRBCollidesWithChannel(RBCC_BlockingVolume,TRUE);
	KA.StaticMeshComponent.WakeRigidBody();
}

// remove invalid destructible part info
function RemoveInvalidDestructableParts()
{
	local int i;

	i = 0;
	while(i < AttachedDestructableParts.Length)
	{
		if(!AttachedDestructableParts[i].DropByNotify && 
			(GetHealthPercent() <= AttachedDestructableParts[i].ExplodeHPPercent))
		{			
			DetachDestructablePart(AttachedDestructableParts[i]);
		}
		else 
		{
			++i;
		}
	}
}

// Get animation control interrupt type, e.g. whether hurt animation interrupts other ongoing animations, add by jimzhang 2012.8.10
event int GetAnimTypeByActor()
{
	if(ActorType == EACT_Boss)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


event OnCheckDestructableEquipments()
{
	local DestructablePartAttachInfo dpai;

	//`log("[MMOSFX] 1 ---------------------------------------------------");
	//for(i = 0; i < AttachedDestructableParts.Length; ++i)
	foreach AttachedDestructableParts(dpai)
	{
		if((GetHealthPercent() <= dpai.ExplodeHPPercent) && !dpai.DropByNotify)
		{
			// create explosion effect
			CreateDestructablePartExplodeEmitterBySocket(dpai);

			// create drop
			if(dpai.DestructType == EDRT_Drop)
			{
				CreateDropKActor(dpai.MeshAsset,
						GetSocketWorldPosition(dpai.AttachSocket),
						GetSocketWorldRotation(dpai.AttachSocket));
			}

			//DetachDestructablePart(dpai.AttachName);
		}
	}

	RemoveInvalidDestructableParts();
	//`log("[MMOSFX] 222 [DetachDestructablePart] :" @AttachedDestructableParts.Length);

	//`log("[MMOSFX] 3 ---------------------------------------------------");
}

// create destructible part explosion effects according to different binding types
function CreateDestructablePartExplodeEmitterBySocket(DestructablePartAttachInfo dpai)
{
	// create explosion effect
	if(dpai.ExplodeEmitter != none)
	{
		CreateDestructablePartExplodeEmitter(
				dpai.ExplodeEmitter,
				GetSocketWorldPosition(dpai.AttachSocket),
				GetSocketWorldRotation(dpai.AttachSocket));
	} // emitter
}

// create destructible part explosion effect
function CreateDestructablePartExplodeEmitter(Emitter temp, vector pos, rotator rot)
{
	local Emitter sfxEmitter;
	sfxEmitter = Spawn(temp.Class,,, pos, rot, temp, true);
	sfxEmitter.SetBase(self);
	sfxEmitter.bDestroyOnSystemFinish = true;
	sfxEmitter.ParticleSystemComponent.ActivateSystem();
}

// Animation notifies to drop weapon on death
event OnDeathTrowWeapons()
{
	local DestructablePartAttachInfo dpai;

	`log("****************** Drop Weapon");
	foreach AttachedDestructableParts(dpai)
	{
		// create drop
		if((dpai.DestructType == EDRT_Drop) && dpai.DropByNotify)
		{
			// create explosion effect
			CreateDestructablePartExplodeEmitterBySocket(dpai);

			CreateDropKActor(dpai.MeshAsset,
					GetSocketWorldPosition(dpai.AttachSocket),
					GetSocketWorldRotation(dpai.AttachSocket));
		}
	}

	// all bindings must be cleared after death
	while(AttachedDestructableParts.Length > 0)
	{
		DetachDestructablePart(AttachedDestructableParts[0]);
	}
}

function HideNPCWeapon( bool hide )
{
	local DestructablePartAttachInfo dpai;
	foreach AttachedDestructableParts( dpai )
	{
		if( dpai.HideWhenRun )
		{
			if( dpai.IsStaticMeshComponent )
				dpai.MeshComponent.SetHidden( hide );
			else
				dpai.SkelMeshComponent.SetHidden( hide );
		}
	}
}

function Vector GetAbilityTargetDirection( )
{
	local Vector v;
	if( NPCController( Controller ).m_TargetActor != none )
		v = Normal( NPCController( Controller ).m_TargetActor.Location - Location );
	return v;
}

function Vector GetAbilityTargetLocation( )
{
	local Vector p;
	if( NPCController( Controller ).m_TargetActor != none )
		p = NPCController( Controller ).m_TargetActor.Location;
	return p;
}

event AttachHeadUp( EQuestHUDStatus Type )
{
	
	HeadUpComponent  = new( self ) class'StaticMeshComponent';

	HeadUpComponent.SetTranslation( vect(0.0,0.0,150) );

	HeadUpType = type;

	switch (type)
	{
	case EQHUDS_Acceptable:
		HeadUpComponent.SetStaticMesh( StaticMesh(DynamicLoadObject("gantanhao.Mesh.gantanhao", class'StaticMesh')) );
		break;
	case EQHUDS_WillAcceptable:
		HeadUpComponent.SetStaticMesh(  StaticMesh(DynamicLoadObject("gantanhao.Mesh.gantanhao_h", class'StaticMesh')) );
		break;
	case EQHUDS_CanSubmit:
		HeadUpComponent.SetStaticMesh(  StaticMesh(DynamicLoadObject("gantanhao.Mesh.wenhao", class'StaticMesh')) );
		break;
	case EQHUDS_NotValidForSubmit:
		HeadUpComponent.SetStaticMesh(  StaticMesh(DynamicLoadObject("gantanhao.Mesh.wenhao_h", class'StaticMesh')) );
		break;
	}
	//StaticMesh'gantanhao.Mesh.wenhao'

	//HeadUpComponent2.SetHidden(true);
	//HeadUpComponent1.SetHidden(true);
	//HeadUpComponent2.SetHidden(true);

	AttachComponent( HeadUpComponent );

	HeadUpComponent.SetAbsolute(false, true, false);
	HeadUpComponent.SetRotation(MakeRotator(0, 0, 0));
	//self.ForceUpdateComponents();

	//`log("_________ AttachHeadUp" @ Template.Name @ type @ HeadUpComponent.StaticMesh);
}


native function DetachHeadUp();
//{
//	HeadUpType = EQHUDS_None;
//	DetachComponent( HeadUpComponent );
	
//}



//event ServerMoveTo( Vector dest, EActorMoveType type, optional float speed = -1.f )
//{
//	local MMONPCProxyController ctrl;
//	SetMoveType( type, speed );
//	`Log( "[MMO Info] NPC"@ name@ "Move Type"@ type@ "Speed"@ speed );
//	ctrl = MMONPCProxyController( Controller );
//	if( ctrl != none )
//	{
//		ctrl.SimulatedMoveTo( dest );
//	}
//	else
//	{
//		`Log( "[MMO Error] Can't find npc proxy controller"@ name );
//	}

//	ServerMoveDest = dest;
//}

event ServerStartQTE()
{
}

native function bool IsNeedArchetype();

native function ProcessDefaultDie(byte DieCause);

native function PlayActionById(int ActionId);

// export table header, call the script function first, then the native function
event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues)
{	
	// Classes inheriting from ACTOR can only export via native functions
}


// export each row of data, call the script function first, then the native function
event ScriptDataToExcel(out array<string> Row)
{
	// Classes inheriting from ACTOR can only export via native functions
}

native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function ImportFromExcel(array<string> values);
event ScriptImportFromExcel(array<string> Values)
{	
	// Classes inheriting from ACTOR can only export via native functions
}
native function InitDataToExcel();
native function int GetTemplateID() const;

native function string GetIdentityFullPath();

event string GetDetailedDescription( int InIndex )
{
	return "";
}

native function TickStatusProc(array<StatusProcData> procs );

simulated event Tick( float deltaTime )
{
	super.Tick(deltaTime);
	TickStatusProc(TickProcList);
}

native function EPickPriority GetPickPriority();

native function bool CanGuildBuildingInteract() const;

DefaultProperties
{
	//ActorAttributes=ActorAttribute'Monster_Human.Monster_Human_Attribute'

	Begin Object Class=SkeletalMeshComponent Name=InitialSkeletalMesh
		Animations=None
		//SkeletalMesh=SkeletalMesh'Demo_Characters.Mesh.Man'
		//AnimSets.Add(AnimSet'Player_Male_Anim.AS_Male_Base')
		//PhysicsAsset=PhysicsAsset'Player_Male_Anim.Physics.BasePhysics'
		AnimTreeTemplate=AnimTree'Player_AnimTree.AT_Player'
		LightingChannels=(Gameplay_2=TRUE)
		bHasPhysicsAssetInstance=false
		//bUpdateKinematicBonesFromAnimation=true
		//bUpdateJointsFromAnimation=true
		//PhysicsWeight=1.0
		//BlockRigidBody=true
		//CollideActors=true
		//BlockActors=false
		BlockZeroExtent=true
		//BlockNonZeroExtent=false
		//Scale3D=(X=2,Y=2,Z=2)
		//bAllowAmbientOcclusion=FALSE
		RootMotionMode=RMM_Ignore
		RBChannel=RBCC_Untitled3
		bUpdateSkelWhenNotRendered=false
	End Object
	//Mesh comes from the GamePawn base class member (seems to set the spawn character)
	Mesh=InitialSkeletalMesh;
	Components.Add(InitialSkeletalMesh);
	//Begin Object Class=StaticMeshComponent Name=TempStaticMesh1
	//End Object
	//Components.Add(TempStaticMesh1)
	//TempStaticMesh=TempStaticMesh1



	//Begin Object Class=StaticMeshComponent Name=HeadUpComponent11
	//End Object
	//Components.Add(HeadUpComponent11)
	//HeadUpComponent1=HeadUpComponent11

	//Begin Object Class=StaticMeshComponent Name=HeadUpComponent21
	//End Object
	//Components.Add(HeadUpComponent21)
	//HeadUpComponent2=HeadUpComponent21

	m_fPatrolRange=300.f
	m_fSightRange=500.f
	m_fTraceRange=1000.f
	m_bIsPossitive=false
	//m_bIsSnaky=false
	WanderType=EWanderType_Stand
	AttackInterval=0.5f
	//DefaultDialogID=-1

	//m_NameAttackAnim='man_fight01';
	//m_NameHurtAnim='man_wound';

	// TODO: use the right material
//	TheHealthHUDMatTemplate=MaterialInstanceConstant'DunDefMisc.Menus.TowerHealthBarMat_INST'
	
	//NPCAIControllerTemplate=ActiveMonsterAI'MonsterAI.NormalAI'

	EnableDrawHealthBar=true
	HealthBarScale=0.65
	HealthBarPositionOffset=(Z=-10)
	//Physics=PHYS_None
	bBlockActors=false
	bCollideActors=true
	bCollideWorld=false
	BlockRigidBody=false
	LowHealthParticlePercentThreshold=0.375
	UnhealthyColor=(X=1.5)
	HealthyColor=(Y=1.0)
	LastTakeDamageTime=-10000
    NextSoundTime=0

	//RotationRate=(Pitch=200000,Yaw=200000,Roll=200000)

	NPCID=-1
	//bDebugRanges=false;

	HeadUpType=EQHUDS_None

	bPendingClimb=false

	HideWeaponWhenRunning=false

//	UseStaticMesh=false

	CouldJumpDown=false
	CryForHelp=false
	NPCIcon=-1
	MinimapIconID=-1
	WorldmapIconID=-1
	ShopID=-1
	SceneHintID=-1
	m_bIsWarehouse=false

	//CriticalBeatBackDistance=250
	//NormalBeatBackDistance=100
	//RandomWanderMaxDistance=500
	//AbilityUseRate=0.5f
	//MinDistanceToTarget=50
	//MaxDistanceToTarget=800

	DropMoneyCountMin=1 
	DropMoneyCountMax=2
	DropMoneyMin=10
	DropMoneyMax=100
	//AIUpdateDelta=0.2
	//DeathBeginProcList(0)=((Rate=0),(Proc(0)=MMOStatusProc_Ragdoll'DeathProcess.DefaultDeathProc'))
	//ControllerClass=class'MMONPCProxyController'

	PickPriority=EPP_Normal

	DropItemSetList(0)=()
	DropItemSetList(1)=()
	DropItemSetList(2)=()
	DropItemSetList(3)=()
	DropItemSetList(4)=()
	DropItemSetList(5)=()
	DropItemSetList(6)=()
	DropItemSetList(7)=()
	DropItemSetList(8)=()
	DropItemSetList(9)=()
	DropItemSetList(10)=()
	DropItemSetList(11)=()
	DropItemSetList(12)=()
	DropItemSetList(13)=()
	DropItemSetList(14)=()
	DropItemSetList(15)=()

	SpeciesType=E_spc_Human
	ActorType=EACT_NPC
	OverrideActorType=EACT_None

	QuestIconName=""
}
