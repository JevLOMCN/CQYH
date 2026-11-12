//
// Used to implement destructible objects in the scene
//
class MMODescructibleActor extends FracturedStaticMeshActor implements(IAbilityDamageable, IMMOActorHUD, IServerTrigger)
	placeable
	dependson(WorldInfo, ActorAttribute)
	config(Game)
	native;

// Low HP range reference value
var(MMO) float LowHealthParticlePercentThreshold <DisplayName="LowHealthParticlePercentThreshold">;

// Health bar color when healthy
var(MMO) Vector HealthyColor <DisplayName="HealthyColor">;

// Health bar color when hurt
var(MMO) Vector UnhealthyColor <DisplayName="UnhealthyColor">;

// Effect template when hit
var(MMO) Emitter HurtEffectTemplate <DisplayName="HurtEffectTemplate">;

// Effect template on death
var(MMO) Emitter DiedEffectTemplate <DisplayName="DiedEffectTemplate">;

// Particle component
var(MMO) ParticleSystemComponent LowHealthParticleEffect <DisplayName="LowHealthParticleEffect">;

// Health bar material template
var(MMO) MaterialInstanceConstant TheHealthHUDMatTemplate <DisplayName="TheHealthHUDMatTemplate">;

// Health bar material
var transient MaterialInstanceConstant MyHealthHUDMatInstance;

// Whether to draw the health bar
var(MMO) bool EnableDrawHealthBar <DisplayName="EnableDrawHealthBar">;

// Health bar scale
var(MMO) float HealthBarScale <DisplayName="HealthBarScale">;

// Health bar offset
var(MMO) Vector HealthBarPositionOffset <DisplayName="HealthBarPositionOffset">;

// Time of the last attack
var float LastTakeDamageTime;


// Faction ID, AND two faction IDs; non-zero means friendly
var int FriendlyMask;

// Character attributes
var(MMO) ActorAttribute ActorAttributes <DisplayName="ActorAttributes">;

// Disappear time
var(MMO) float FadeOutTime <DisplayName="FadeOutTime">;
var float FadeLife;

var(MMOServer) int TriggerID;
var(MMOServer) ETriggerType TriggerType;
// Associated blocking box
var(MMOServer) array<DynamicBlockingVolume> AssociatedBlockingVolume;
var(MMOServer) ETriggerInitState InitTriggerState;
var(MMOServer) array<NPCSpawnPoint> NPCSpawnPoints;
var(MMOServer_NPCDead) NPCDeadEvent NPCDeadEvent;
var(MMOServer) array<IServerTrigger> LinkedTrigger;
var(MMOServer) int DelayTime;
var(MMOServer) array<AbilityEvent> AbilityList;
var(MMOServer) TeleportEvent TeleportEvent;

var(MMOServer) ETrapEventType TrapEventType;

var(MMOServer_EventArea) array<IServerArea> LinkedEventArea;
var(MMOServer_LoadingTrigger) int LoadingTime;

var(MMOServer_OpenTeleport) array<TeleportOrig> LinkedTeleportList;
// Get attack/defense attributes
function ActorAttribute GetActorAttribute()
{
	return ActorAttributes;
}

// Initialization function
simulated event PostBeginPlay()
{
	super.PostBeginPlay();

	RegisterToFacade();

	ActorAttributes = new(self) class'ActorAttribute'(ActorAttributes);

	//create a new MIC of the healthbar for this object, so that it can uniquely set the MIC's parameters to its own values.
	MyHealthHUDMatInstance = new(self) class'MaterialInstanceConstant';
	MyHealthHUDMatInstance.SetParent(TheHealthHUDMatTemplate);
}

// Calculate HP percentage
native function float GetHealthPercent();
//{
//	return float(ActorAttributes.HP) / float(ActorAttributes.MaxHP);
//}

//---------------------------------------------------------------
// IMMOActorHUD
//---------------------------------------------------------------

function bool OnlyDrawHUDForOwner(){return false;}

native function DrawHUDPostGFx( HUD H);


native function bool DrawMyHUD( MMOHud H );
//{
//	local Vector screenPos;
//	local float healthPercent;
//	local LinearColor healthColor;
//	local Vector healthColorVec;
//	local Vector objectIconWorldPos;
//	local float cameraDistFromPos;
//	local float iconWidth, iconHeight;
//	local float IconDistanceScale;

//	if(!EnableDrawHealthBar)
//		return false;

//	objectIconWorldPos = Location+(HealthBarPositionOffset>>Rotation);
//	screenPos = H.Canvas.Project(objectIconWorldPos);
//	cameraDistFromPos = VSize(objectIconWorldPos - H.PlayerOwner.PlayerCamera.ViewTarget.POV.Location);
//	IconDistanceScale = FClamp(1.0 - cameraDistFromPos/4000,0.25,1)*(H.Canvas.ClipY / 768.0f)*HealthBarScale;

//	iconWidth = 166*IconDistanceScale;
//	iconHeight = 18*IconDistanceScale;

//	if(!H.IsOnCanvas(screenPos,iconWidth,iconHeight))
//		return false;

//	healthPercent = GetHealthPercent();
//	healthColorVec = VLerp(UnhealthyColor,HealthyColor,healthPercent);
	
//	if(`TimeSince(LastTakeDamageTime) < 2)
//		healthColorVec *= 1 + 13.0*Abs(sin(WorldInfo.TimeSeconds*7));

//	healthColor.R=healthColorVec.X;
//	healthColor.G=healthColorVec.Y;
//	healthColor.B=healthColorVec.Z;

//	MyHealthHUDMatInstance.SetScalarParameterValue('Percent',healthPercent);
//	MyHealthHUDMatInstance.SetVectorParameterValue('BarColor',healthColor);

//	H.Canvas.SetPos(screenPos.x-iconWidth/2,screenPos.y-iconHeight/2);
//	H.Canvas.DrawMaterialTile(MyHealthHUDMatInstance,iconWidth,iconHeight);

//	return true;
//}

event int GetFriendlyMask()
{
	return FriendlyMask;
}

// if processed return true, else false
event ProcessDamage(DamageData Damage)
{
	ActorAttributes.HP -= Damage.DamageValue;

	if(ActorAttributes.HP <= 0)
	{
		ActorDied(none, vect(0,0,0), none, Damage.BaseActor);
	}
	else if(HurtEffectTemplate != none)
	{
		Spawn(HurtEffectTemplate.Class,,,Location,Rotator(Damage.BaseActor.Location-Location),HurtEffectTemplate,true);
		if(GetHealthPercent() < LowHealthParticlePercentThreshold && !LowHealthParticleEffect.bIsActive)
		{
			LowHealthParticleEffect.ActivateSystem();
		}
	}

	//super.TakeDamage( Damage.DamageValue, (Damage.Attacker!=none)?Damage.Attacker.Controller:none, Damage.HitLocation, Damage.HitMomention, none);
}

event bool IsDead()
{
	if(LifeSpan < 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


// Destroy event
event Destroyed()
{
	super.Destroyed();
	//OnDestroy_RemoveFromTargetableList();
}

// Death
function ActorDied(Controller EventInstigator, vector HitLocation, class<DamageType> DamageType, Actor DamageCauser)
{
//	local MMOEmitterSpawnable sfx;

	LifeSpan = -1;
	EnableDrawHealthBar = false;

	if(DiedEffectTemplate != none)
	{
		Spawn(DiedEffectTemplate.Class,,,Location,Rotation,DiedEffectTemplate,true);
	}

	Explode();

	PushState('FadeOut');
	//if(LowHealthSound.IsPlaying())
	//	LowHealthSound.Stop();
}

//---------------------------------------------------------------
// Ability collision event
//---------------------------------------------------------------
event Touch( Actor Other, PrimitiveComponent OtherComp, vector HitLocation, vector HitNormal )
{
	//super.Touching(Other, OtherComp, HitLocation, HitNormal);
	
	//`log("$$$$$--------$$$$$$$$$ Touch" @Other @HitLocation);

	//if(MMOEmitter(Other) != none)
	//{
	//	self.Move(-HitNormal * 100);
	//}
}

// called for PendingTouch actor after physics completes
event PostTouch( Actor Other )
{
	//`log("$$$$$$--------$$$$$$$$ PostTouch" @Other);
}

event UnTouch( Actor Other )
{
	//`log("$$$$$--------$$$$$$$$$ UnTouch" @Other);
}

event Bump( Actor Other, PrimitiveComponent OtherComp, Vector HitNormal )
{
	//"ActorCollision", 
	//`log("$$$$$--------$$$$$$$$$ Bump" @Other);
}

// Disappearing fragments
state FadeOut
{
	event PushedState( )
	{
		super.PushedState( );

		FadeLife = 0.0f;
		//`log("$$$$$--------$$$$$$$$$ PushedState");
	}

	event BeginState( Name PreviousStateName )
	{
		super.BeginState(PreviousStateName);

		FadeLife = 0.0f;

		//`log("$$$$$--------$$$$$$$$$ BeginState");
	}

	simulated event Tick( float deltaTime )
	{
		FadeLife += deltaTime;
		if(FadeLife >= FadeOutTime)
		{
			//`log("$$$$$--------$$$$$$$$$ Destroy();");
			
			Destroy();
			PopState();
		}
	}
}

// Register with Facade for lookup
native function RegisterToFacade();

native function ETriggerType GetTriggerType();
native function ETrapEventType GetTrapEventType();

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

defaultproperties
{
	FriendlyMask = 4

	// Add hurt particle component
	Begin Object Class=ParticleSystemComponent Name=ParticleSystemComponent2
	bAutoActivate=false
	End Object
	Components.Add(ParticleSystemComponent2)
	LowHealthParticleEffect=ParticleSystemComponent2

	//HurtEffectTemplate=MMOEmitterSpawnable'MMOArchetTypes.Emitter_HitStone'
	//DiedEffectTemplate=MMOEmitterSpawnable'MMOArchetTypes.Emitter_StoneExplosion'
	//ActorAttributes = ActorAttribute'MMO_ActorAttribute.DestructibleActor_Stone'


//	TheHealthHUDMatTemplate=MaterialInstanceConstant'DunDefMisc.Menus.TowerHealthBarMat_INST'

	EnableDrawHealthBar=true
	HealthBarScale=0.65
	HealthBarPositionOffset=(Z=10)
	//Physics = PHYS_None
	//bBlockActors=true
	//bCollideActors=true
	//bCollideWorld=true
	//BlockRigidBody=true
	LowHealthParticlePercentThreshold=0.375
	UnhealthyColor=(X=1.5)
	HealthyColor=(Y=1.0)
	LastTakeDamageTime=-10000
	FadeOutTime = 3
}