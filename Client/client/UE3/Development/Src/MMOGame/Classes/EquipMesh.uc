/*=============================================================================
	EquipMesh.uc: base mesh elements to form equips.
	An Equip can content one or more EquipMeshes.
	Eg. a Shirt chould has both Chest and Hands meshes.
=============================================================================*/

class EquipMesh extends Object
	EditInlineNew
	native
	hidecategories(Object);

enum EComponentResourceType
{
	ECRT_StaticMesh     <DisplayName="ECRT_StaticMesh">, 
	ECRT_SkeletalMesh     <DisplayName="ECRT_SkeletalMesh">,
	ECRT_Particle     <DisplayName="ECRT_Particle">,
};

enum EComponentAddingType
{
	ECAT_AddToBaseActor     <DisplayName="ECAT_AddToBaseActor">,
	ECAT_AttachToBaseSocket     <DisplayName="ECAT_AttachToBaseSocket">,
	//ECAT_ChangeTexture,
};

// for SkeletalMesh components, which kind of animation data to use
enum EUsingAnimationType
{
	EUAT_NoAnimation     <DisplayName="EUAT_NoAnimation">,
	EUAT_BaseAnimation     <DisplayName="EUAT_BaseAnimation">,
	EUAT_SelfAnimation     <DisplayName="EUAT_SelfAnimation">,
};

var transient editconst bool ShowResType;
var transient editconst bool ShowStaticMesh;
var transient editconst bool ShowSkeletalMesh;
var transient editconst bool ShowParticle;
var transient editconst bool ShowSocket;
var transient editconst bool ShowCarrySocket;
var transient editconst bool ShowTexture;
var transient editconst bool ShowAnimsets;



// identify which body part this mesh is supposed to be
var() EBodyMeshType	MeshType								<DisplayName="MeshType">;

// identify the layer of this body mesh 
// so that higher P meshes will cover( setHidden ) lower ones
var() int Priority;
var(ToDel) EComponentResourceType    ResourceType				<DisplayName="ResourceType">;
var(ToDel) StaticMesh                StaticMesh					<DisplayName="StaticMesh">;
var() SkeletalMesh	            SkeletalMesh				<DisplayName="SkeletalMesh">;
var(ToDel) ParticleSystem            ParticleSystem				<DisplayName="ParticleSystem">;
// skin texture config removed, to support swapping skin color via texture
//var() Texture2D				BaseTexture					<DisplayName="BaseTexture">;	// texture for skin
// equipment texture
var() Texture2D					RegionTexture				<DisplayName="RegionTexture">;	// texture for body parts
// if it is the default equipment (bare model), this is the swappable skin texture; if other equipment, this is the swappable color texture (uses the same specular and normal maps as RegionTexture)
var() array<Texture2D>			ColorTextures				<DisplayName="ColorTextures">;
// the texture id this equip uses. for those equipmeshes with different color textures who share the same spec/norm texture
var() int						UsedColorTextureID			<DisplayName="ColorID">;
// indicate if the equip needs a skin texture ( ie. the equip has some skin regions )
var() bool						bNeedSkinTexture			<DisplayName="NeedSkinTexture">;

var(ToDel) EComponentAddingType  ComponentAddingType				<DisplayName="ComponentAddingType">;
// if ComponentAddingType == ECAT_AttachToBaseSocket, have to set a socket name in main character
var(ToDel) name                  SocketName						<DisplayName="SocketName"|DynamicList="SocketList">;
// for weapons, the attach socket can be switched
var(ToDel) name                  CarrySocketName					<DisplayName="CarrySocketName"|DynamicList="SocketList">;
var(ToDel) bool                  bSupportSwithAttachSocket		<DisplayName="SupportSwithAttachSocket">;
var(ToDel) EUsingAnimationType   AnimationType					<DisplayName="AnimationType">;
var AnimTree              AnimTree;
var(ToDel) AnimSet               AnimSet							<DisplayName="AnimSet">;
var(ToDel) bool                  AlwaysUseIdleAnim	            <DisplayName="AlwaysUseIdleAnim">;
var(ToDel) name                  IdleAnim			            <DisplayName="SequenceNameToPlayAnim">;

var() bool			bHideFlag;
var() EBodyMeshType ToHideBodyType;

var float Scale;

var() editinline ArmorResourceData ArmorTemplate;



//// primitive component for attaching
//var transient PrimitiveComponent	PrimComponent;
//var transient PrimitiveComponent    SubPrimComponent;
//var transient array<MMOEmitter>     AttachedEmitters;

//var transient MMOActorAT    AnimationController;
//var transient AnimNodeBlend WeaponActionLoopBlendNode;
//var transient AnimNodeSequence ActionNode;
//var transient AnimNodeSequence LoopNode;


//var bool bHold;

//var int CurrentAnimKey;

//---------------------------------------------------------------
// material controller
//---------------------------------------------------------------
var MMOActorMaterialController		MaterialController;

//function SetHidden( bool hide )
//{
//	if( PrimComponent != none )
//	{
//		PrimComponent.SetHidden( hide );
//	}
//}

//function SetOpacity(float percent)
//{
//	if(MeshComponent(PrimComponent) == none)
//	{
//		return;
//	}

//	if(MaterialController == none)
//	{
//		MaterialController = new class'MMOActorMaterialController'();
//		MaterialController.Init(MeshComponent(PrimComponent));
//	}
//	MaterialController.SetOpacity(percent);
//}


native final function SetSilhouette( bool show, Vector sc );


//native function InitAnimation(SkeletalMeshComponent InSkeletalMesh, MMOActor Target);
//native function InitComponent( MMOActor Target );

//native function SetVisible(bool bVisible);
//native function AddAttachedEmitter(MMOEmitter InEmitter);
//native function DestroyAttachedEmitters();

// check if need to hide other part based on the equip mesh's type
native final function DoHideCheck( MMOActor Target, bool TakeOn );

native function bool OnTakeOn(MMOActor Target, bool UseMergedMesh);
native function bool OnTakeOff(MMOActor Target, bool UseMergedMesh);

//event bool SwitchHold( MMOActor BaseActor, bool bHoldInHand )
//{
//	local SkeletalMeshComponent BaseComponent;
//	if(bSupportSwithAttachSocket && bHold != bHoldInHand && BaseActor != none)
//	{
//		bHold = bHoldInHand;
//		BaseComponent = BaseActor.GetMasterComponent();
//		if(BaseComponent == none)
//		{
//			`warn("BaseComponent is none");
//			return false;
//		}
//		if(bHold)
//		{
//			BaseComponent.DetachComponent(PrimComponent);
//			BaseComponent.AttachComponentToSocket(PrimComponent, SocketName);

//			if(SubPrimComponent != none)
//			{
//				BaseComponent.DetachComponent(SubPrimComponent);
//				BaseComponent.AttachComponentToSocket(SubPrimComponent, SocketName);
//			}
//		}
//		else
//		{
//			BaseComponent.DetachComponent(PrimComponent);
//			BaseComponent.AttachComponentToSocket(PrimComponent, CarrySocketName);

//			if(SubPrimComponent != none)
//			{
//				BaseComponent.DetachComponent(SubPrimComponent);
//				BaseComponent.AttachComponentToSocket(SubPrimComponent, CarrySocketName);
//			}
//		}
//		return true;
//	}

//	return false;
//}

//native function float PlayAnimation(name Anim, optional int InKey = 0, optional float Rate = 1.0f, optional bool bLoop = false, optional ERootBoneAxis RootBoneAxisX = RBA_Discard,optional ERootBoneAxis RootBoneAxisY = RBA_Discard,optional ERootBoneAxis RootBoneAxisZ = RBA_Discard, optional EAnimBlendType BlendType = EABT_FullBody, optional float Position = 0.f, optional float InBlendIn=0.1, optional float InBlendOut=0.1, optional EAnimLevel InAnimLevel = EAL_Action);
//native function PlayAnim(name Anim, bool bLoop, float InRate, float InStartTime);
//native function PlayAnimByKey(name Anim, bool bLoop, float InRate, float InStartTime, int InKey);
//native function StopAnimByKey(int InKey);
//native function OnAnimEnd(AnimNodeSequence SeqNode, float PlayedTime, float ExcessTime);
//native function RefreshAnim();
//native function SetMaterialInstanceParamValue(linearcolor NewColor, name Param);

//native function SetSkeletMesh(SkeletalMesh mesh);
cpptext
{	
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent);
	
	#if WITH_EDITOR
	void CheckForError(class UEquipAppearence* parent);
	virtual void PostLoad();
	void _UpdatePropertyEdit();
	virtual void CollectReferencedResource(TSet<FString>& outResList);
	#endif
}

DefaultProperties
{
	MeshType=EBMT_None
	Priority=0

	bSupportSwithAttachSocket=false
	ShowResType=true
	ShowStaticMesh=true
	ShowSkeletalMesh=true
	ShowParticle=true
	ShowSocket=true
	ShowTexture=true
	ShowAnimsets=true
	AnimTree=AnimTree'AT_Common.AT_Common'

	bHideFlag=false
	Scale=1.0

	AlwaysUseIdleAnim=true

	UsedColorTextureID=0
	bNeedSkinTexture=false
	ToHideBodyType=EBMT_None
}