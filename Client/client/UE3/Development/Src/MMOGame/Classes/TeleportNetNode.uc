//
// TeleportNetNode teleport network node
//

class TeleportNetNode extends Actor implements (IPickableActor, IInteractableActor)
	placeable
	native
	hidecategories(Object,Debug,Display,Attachment,Collision,Physics,Advanced)
	ClassGroup(__Server__);

enum EWorldRegion
{
	EWR_EAST		<DisplayName="EAST">,
	EWR_SOUTH       <DisplayName="SOUTH">,
	EWR_WEST        <DisplayName="WEST">,
	EWR_NORTH       <DisplayName="NORTH">
};

var(MMOServer) int TeleportClientRange;
//var(MMOServer) int ClientID;
var(MMOServer) int InSceneID;
var(MMOServer) bool InitState;
var(MMOServer) int ShowPriority;

//var(MMOServer) int NeedMnyType;
//var(MMOServer) int NeedMoney;

var(MMOServer) string DisplayName;
var(MMOServer) EWorldRegion RegionID;
var(MMOServer) string PicResName;
// debug range wireframe - melee range
var(Editor) editoronly DrawCylinderComponent TeleportRange;

var() StaticMeshComponent TeleportNodeMesh;
var() ParticleSystemComponent TeleportStaticEffect              <DisplayName="Actived Effect">;
var() ParticleSystemComponent TeleportActivingEffect            <DisplayName="Activing Effect">;
var() CylinderComponent CollisionCylinderComponent;
var() EPickPriority PickPriority;

native function vector GetServerPosition();

native function EPickPriority GetPickPriority();
native function EMMOCursorType GetMouseCursor();

native final function HandleTeleport();
native final function HandleTeleportDisapart();

simulated event PostBeginPlay()
{
	super.PostBeginPlay();

	SetCollision(true, false, false);
	SetCollisionType(COLLIDE_TouchAll);
}


native function vector GetInteractActorLocation();
native function float GetMaxInteractRadius();
native function DoInteract();
native function bool CanInteract() const;
native function SetSilhouette( bool show, Vector sc = vect( 1.f, 1.f, 0.f ) );

event Touch( Actor Other, PrimitiveComponent OtherComp, vector HitLocation, vector HitNormal )
{
	//if(MMOPlayer(Other).IsMainPlayer())
	//{
	//	HandleTeleport();
	//}
}

event UnTouch( Actor Other )
{
	if(MMOPlayer(Other).IsMainPlayer())
	{
		HandleTeleportDisapart();
	}
}

event ActiveThis( bool isActive )
{
	TeleportStaticEffect.SetActive( isActive, false );
}

event PlayFirstActived()
{
	TeleportStaticEffect.DeactivateSystem();
	TeleportActivingEffect.ActivateSystem( false );
	//TeleportActivingEffect.SetActive( true, false );
	TeleportActivingEffect.OnSystemFinished = TeleportOnFirstActiveEffectFinished;
}

simulated function TeleportOnFirstActiveEffectFinished( ParticleSystemComponent psc )
{
	TeleportActivingEffect.DeactivateSystem();
	//TeleportActivingEffect.SetActive( false, false );
	//active static effect
	//`log(">>>TeleportOnFirstActiveEffectFinished>>>>>TeleportStaticEffect:"$TeleportStaticEffect.bIsActive$" , TeleportActivingEffect:"$TeleportActivingEffect.bIsActive);
	TeleportStaticEffect.ActivateSystem( false );
}

DefaultProperties
{
	Begin Object Class=StaticMeshComponent Name=TeleportNodeStaticMeshComponent0
		StaticMesh=StaticMesh'bb_tpc.STM.S_bb_tpc01'
		CollideActors=false
		Scale3D=(X=1.0,Y=1.0,Z=1.0)
		Translation=(X=0.0,Y=0.0,Z=-47.0)

		bAcceptsLights=TRUE
		bUsePrecomputedShadows=TRUE
		LightingChannels=(BSP=FALSE,Dynamic=FALSE,Static=TRUE,CompositeDynamic=FALSE)
	End Object
	Components.Add(TeleportNodeStaticMeshComponent0)
	TeleportNodeMesh=TeleportNodeStaticMeshComponent0
	
	Begin Object Class=ParticleSystemComponent Name=TeleportNodeEffect0
		Template=ParticleSystem'Fx_Sence_Portal.01Par.Fx_Portal_001'
		bAutoActivate=false
		bIsActive=false
	End Object
	Components.Add(TeleportNodeEffect0)
	TeleportStaticEffect=TeleportNodeEffect0

	Begin Object Class=ParticleSystemComponent Name=TeleportNodeEffect1
		Template=ParticleSystem'Fx_Sence_Portal.01Par.Fx_Portal_002'
		bAutoActivate=false
		bIsActive=false
	End Object
	Components.Add(TeleportNodeEffect1)
	TeleportActivingEffect=TeleportNodeEffect1

	Begin Object Class=SpriteComponent Name=Sprite
		Sprite=Texture2D'MMOEditor.TeleportOrig'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Server"		
	End Object
	Components.Add(Sprite);
	//DrawScale=0.2

	//ToSceneClientRangeRadius=200
	//Region=EWR_R1
	InitState=false

		// add debug range wireframe - melee range
	Begin Object Class=DrawCylinderComponent  Name=DebugRangeNear0
		CylinderColor=(R=255,G=0,B=0,A=255)
		CylinderRadius=400.0
		CylinderTopRadius=400.0
		CylinderHeight=0.0
		CylinderHeightOffset=0.0
		CylinderSides=32
		bDrawWireCylinder=true
		HiddenGame=true
	End Object
	Components.Add(DebugRangeNear0)
	TeleportRange=DebugRangeNear0
	TeleportClientRange=400

	Begin Object Class=CylinderComponent Name=CollisionCylinder0
		CollisionRadius=100
		CollisionHeight=200
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
	IsPickable=true
}
