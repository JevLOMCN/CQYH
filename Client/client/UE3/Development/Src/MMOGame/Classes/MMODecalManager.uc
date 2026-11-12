/**
*	MMODecalManager.uc
*/

class MMODecalManager extends DecalManager
	native
	config(MMOGame);

var DecalComponent	SelectionDecal;		// Character selection marker
var MMOActor		SelectedActor;		// Selected character

enum SelectionDecalType
{
	ESDT_Self,
	ESDT_Friend,
	ESDT_Enemy,
	ESDT_Default
};

var MaterialInstanceConstant		SelfDecal;
var MaterialInstanceConstant		FriendDecal;
var MaterialInstanceConstant		EnemyDecal;
var MaterialInstanceConstant		DefaultDecal;
var int DecalSize;

var transient SelectionDecalType	CurSelectionDecalType;
var transient MaterialInstanceConstant	DecalMat_Self;
var transient MaterialInstanceConstant	DecalMat_Friend;
var transient MaterialInstanceConstant	DecalMat_Enemy;
var transient MaterialInstanceConstant	DecalMat_Default;

var bool	bInitialized;

native function UpdateSelectionDecalRotation( MMOActor actor );

function InitDecalMaterials()
{
	//local MaterialInstanceConstant mic;

	
		DecalMat_Self = new class'MaterialInstanceConstant';
		DecalMat_Self.SetParent( SelfDecal );
	

	
		DecalMat_Friend = new class'MaterialInstanceConstant';
		DecalMat_Friend.SetParent( FriendDecal );
	

	
		DecalMat_Enemy = new class'MaterialInstanceConstant';
		DecalMat_Enemy.SetParent( EnemyDecal );
	

	
		DecalMat_Default = new class'MaterialInstanceConstant';
		DecalMat_Default.SetParent( DefaultDecal );
	

	if( DecalMat_Default != none )
	{
		SelectionDecal.SetDecalMaterial( DecalMat_Default );
		CurSelectionDecalType = ESDT_Default;
	}
	else
	{
		`Log( "[MMO Debug] Failed to creat default selection decal:"@DefaultDecal );
	}
}

event SelectActor( MMOActor selActor, optional SelectionDecalType type = ESDT_Default )
{
	local bool rst;
	local float radius, height;

	return;

	if( SelectedActor == selActor )
		return;

	if( !bInitialized )
	{
		InitDecalMaterials();
		bInitialized = true;
	}

	SelectionDecal.Width = self.DecalSize;
	SelectionDecal.Height = self.DecalSize;

	if( SelectedActor != none )
	{
		SelectedActor.DetachComponent( SelectionDecal );
	}
	SelectedActor = selActor;
	if( SelectedActor != none )
	{
		UpdateSelectionDecalRotation( SelectedActor );
		SelectedActor.AttachComponent( SelectionDecal );

		SelectedActor.GetBoundingCylinder( radius, height );

		SelectionDecal.FarPlane = height + 30.0f;	// make the decal within 30cm under the actor

		`Log( "[MMO Info] Set selection decal"@SelectedActor@"type:"@type@"range:"@SelectionDecal.FarPlane );
	}

	if( type != CurSelectionDecalType )
	{
		rst = false;
		switch( type )
		{
		case ESDT_Self:
			if( DecalMat_Self != none )
			{
				SelectionDecal.SetDecalMaterial( DecalMat_Self );
				rst = true;
			}
			break;
		case ESDT_Friend:
			if( DecalMat_Friend != none )
			{
				SelectionDecal.SetDecalMaterial( DecalMat_Friend );
				rst = true;
			}
			break;
		case ESDT_Enemy:
			if( DecalMat_Enemy != none )
			{
				SelectionDecal.SetDecalMaterial( DecalMat_Enemy );
				rst = true;
			}
			break;
		case ESDT_Default:
			if( DecalMat_Default != none )
			{
				SelectionDecal.SetDecalMaterial( DecalMat_Default );
				rst = true;
			}
			break;
		default:
			break;
		}

		if( rst == false )
		{
			`Log( "[MMO Debug] Failed to set material"@type@"for selection decal" );
		}
		else
		{
			CurSelectionDecalType = type;
		}
	}

	// todo: set decal transform, color, size, etc.
	//SetDecalParameters(
	//	SelectionDecal,
	//	DecalMaterial,
	//	DecalLocation,
	//	DecalOrientation,
	//	Width,
	//	Height,
	//	Thickness,
	//	bNoClip,
	//	DecalRotation,
	//	HitComponent,
	//	bProjectOnTerrain,
	//	bProjectOnSkeletalMeshes,
	//	HitBone,
	//	HitNodeIndex,
	//	HitLevelIndex,
	//	INDEX_NONE,
	//	InDepthBias,
	//	InBlendRange
	//	);
}

cpptext
{
	virtual void TickSpecial(FLOAT DeltaTime);
}

defaultproperties
{
	Begin Object Class=DecalComponent Name=selDecal
		bMovableDecal=true
		bStaticDecal=false
		bProjectOnStaticMeshes=true
		bProjectOnSkeletalMeshes=FALSE
		Width=512
		Height=512
		ParentRelativeOrientation=(Pitch=-16384, Yaw=0, Roll=0)
		DecalTransform=DecalTransform_OwnerRelative
	End Object
	SelectionDecal=selDecal
	//Components.Add(selDecal)

	SelfDecal=MaterialInstanceConstant'FX_Decal_001.MI_DecPrent_ChooseRing_blue'
	FriendDecal=MaterialInstanceConstant'FX_Decal_001.MI_DecPrent_ChooseRing_green'
	EnemyDecal=MaterialInstanceConstant'FX_Decal_001.MI_DecPrent_ChooseRing_red'
	DefaultDecal=MaterialInstanceConstant'FX_Decal_001.MI_DecPrent_ChooseRing_yellow'
	DecalSize=200

	bInitialized=false
}