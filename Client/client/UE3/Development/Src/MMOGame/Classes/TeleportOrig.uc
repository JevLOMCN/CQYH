//
// TeleportOrig teleport origin
//

class TeleportOrig extends Actor implements (IPickableActor, IInteractableActor)
	placeable
	native
	hidecategories(Object,Debug,Display,Attachment,Collision,Physics,Advanced)
	ClassGroup(__Server__);

var(MMOServer) string ToSceneName;
//var(MMOServer) vector ToSceneClientPosition;
//var(MMOServer) float ToSceneClientRangeRadius;
var(MMOServer) bool InitEnabled;
var(MMOServer) int TeleportClientRange;
var(MMOServer) editconst int AutoServerID;

var(MMOServer) int MinPortalLevel;
var(MMOServer) int MaxPortalLevel;

var(UI) Vector TipsPosOffset;
// debug range wireframe - melee range
var(Editor) editoronly DrawCylinderComponent TeleportRange;

var() ParticleSystemComponent TeleportStaticEffect             <DisplayName="Static Effect">;
var() ParticleSystemComponent TeleportFoucsEffect            <DisplayName="Foucs Effect">;

var() CylinderComponent CollisionCylinderComponent;
var() EPickPriority PickPriority;
var   bool bInTeleportOrig;

native function vector GetServerPosition();

native function EPickPriority GetPickPriority();
native function EMMOCursorType GetMouseCursor();

native final function Send_TeleportReq();
native final function HandleTeleport();
native final function HandleTeleportDisapart();

native function vector GetInteractActorLocation();
native function float GetMaxInteractRadius();
native function DoInteract();
native function bool CanInteract() const;
native function SetSilhouette( bool show, Vector sc = vect( 1.f, 1.f, 0.f ) );


simulated event PostBeginPlay()
{
	super.PostBeginPlay();

	SetCollision(true, false, false);
	SetCollisionType(COLLIDE_TouchAll);
}

event Touch( Actor Other, PrimitiveComponent OtherComp, vector HitLocation, vector HitNormal )
{
	local MMOPlayer player;
	local MMOPlayerController pc;
	
	player = MMOPlayer(Other);

	if(player!=none && player.IsMainPlayer())
	{
		bInTeleportOrig=true;
		if(player != none)
		{
			pc = MMOPlayerController(player.Controller);
			pc.AddCmdWaiting();
			pc.AddCmdInteract(self, 256.0f);
			pc.AddCmdCannotInterrupt();
		}
		//HandleTeleport();
	}
}

event UnTouch( Actor Other )
{
	local MMOPlayer player;
	local MMOPlayerController pc;
	player = MMOPlayer(Other);
	if(player!=none &&player.IsMainPlayer())
	{
		bInTeleportOrig=false;
		HandleTeleportDisapart();
		pc = MMOPlayerController(player.Controller);
		pc.ClearAllCmd();
	}
}

final native function ActiveStaticEffect( bool isActive );
final native function ActiveFoucsEffect( bool isActive );

DefaultProperties
{
	Begin Object Class=SpriteComponent Name=Sprite
		Sprite=Texture2D'MMOEditor.TeleportOrig'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Server"		
	End Object
	Components.Add(Sprite);
	//DrawScale=0.2

	Begin Object Class=ParticleSystemComponent Name=TeleportOrigEffect0
		Template=ParticleSystem'light_gate.FX.FX_light_gate01'
		bAutoActivate=false
		bIsActive=false
		Translation=(X=0,Y=0,Z=500)
		Rotation=(Pitch=-8192,Yaw=0,Roll=0)
		Scale=1.5
	End Object
	Components.Add(TeleportOrigEffect0)
	TeleportStaticEffect=TeleportOrigEffect0

	Begin Object Class=ParticleSystemComponent Name=TeleportOrigEffect2
		Template=ParticleSystem'light_gate.FX.FX_light_gate01a'
		bAutoActivate=false
		bIsActive=false
		Translation=(X=0,Y=0,Z=500)
		Rotation=(Pitch=-8192,Yaw=0,Roll=0)
		Scale=1.5
	End Object
	Components.Add(TeleportOrigEffect2)
	TeleportFoucsEffect=TeleportOrigEffect2

	//ToSceneClientRangeRadius=200
	InitEnabled=true

		// add debug range wireframe - melee range
	Begin Object Class=DrawCylinderComponent  Name=DebugRangeNear0
		CylinderColor=(R=255,G=0,B=0,A=255)
		CylinderRadius=800.0
		CylinderTopRadius=800.0
		CylinderHeight=0.0
		CylinderHeightOffset=0.0
		CylinderSides=32
		bDrawWireCylinder=true
		HiddenGame=true
	End Object
	Components.Add(DebugRangeNear0)
	TeleportRange=DebugRangeNear0
	TeleportClientRange=800

	Begin Object Class=CylinderComponent Name=CollisionCylinder0
		CollisionRadius=300
		CollisionHeight=500
		BlockNonZeroExtent=false
		BlockZeroExtent=false
		BlockActors=false
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
	CollisionCylinderComponent=CollisionCylinder0
	bCollideActors=true

	TipsPosOffset=(x=0,y=0,z=300)
	PickPriority=EPP_Floor
	IsPickable=true
	bInTeleportOrig=false
	MinPortalLevel=0
	MaxPortalLevel=99999
}
