//
// NPCNewSpawnGroupEditorActor
//
class NPCNewSpawnGroupEditorActor extends Actor implements(IEditorSelectedDrawLine)
	//dependson(NPCSpawnPoint, TypeDefine)
	//placeable
	//abstract
	native
	hidecategories(Object,Debug,Advanced,Physics,CollisionChanged,Attachment,DisplayDebug, MMO, Mobile, Collision, Display)
	//ClassGroup(__Server__)
	config(NPCSpawnGroup);

enum EENUM_NPC_GROUP_TYPE
{
	ENpcGroupType_Invalid,
	ENpcGroupType_Boss,
	ENpcGroupType_Gold,
	ENpcGroupType_Silver,
};

var editoronly globalconfig array<color> DrawLineColors;

//display component
var editoronly CylinderComponent CollisionCylinder;
var editoronly DrawCylinderComponent EditorCollisionCylinder;
var editoronly StaticMeshComponent EditorStaticMeshComponent;
var editoronly SpriteComponent EditorSprite;

// get object position
native function vector GetObjectServerPosition();

cpptext
{
	virtual void DrawEditorDynamicElements(FPrimitiveDrawInterface* PDI,const FSceneView* View,UINT DPGIndex,DWORD Flags = 0){}
}

simulated event PostBeginPlay()
{
	super.PostBeginPlay();
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

	Begin Object Class=EditorSelectedDrawLineComponent  Name=EditorComponent0
	End Object
	Components.Add(EditorComponent0)
}
