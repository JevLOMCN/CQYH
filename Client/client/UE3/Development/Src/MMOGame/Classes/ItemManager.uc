/*
 *	 Item manager class, also the bag manager class
 *	 Directly supports stacking items of the same type
 * 
 * */
class ItemManager extends Object
	dependson(MMOSkeletalMeshMerge)
	hidecategories(Object)
	editinlinenew
	native;

//`define EquipMaxType 12
//`define AvatarShowMaxType 11

//var() array<Equip>        DefaultEquipListTemplate;	// Default equipment template list

var transient init array<EquipMesh> OldMeshList;		// Equipment models from the last UpdateOwnerData

var transient init array<Texture2D> SkinTextures;		// Current skin color textures
var transient init array<Texture2D>	SkinTexturesBase;	// The skin texture used to find the specular/normal textures

// The above is only needed by the old outfit-change mechanism; the new one does not need to change body parts
var transient init array<EquipMesh> DefaultMeshList;	// Default models

var transient init array<Equip>  EquippedItemList_Normal;	// Current equipment - normal
var transient init array<Equip>	 EquippedItemList_Avatar;	// Current equipment - fashion
var transient init array<Equip>  EquippedItemList_Hero;	// Current equipment - hero

// Both old and new outfit mechanisms use this array; the new mechanism only uses some positions
var transient init array<EquipMesh>	CurrentMeshList;	// Current models

var int			Capacity; //Changed to non-editable, max bag capacity
var array<int>  CurCapacity; //Current max bag capacity

var transient init array<Item>   BagItemList;		//Item bag
//var transient init array<Item>   BagEquipList;		//Equipment bag
//var transient init array<Item>   BagMaterialList;	//Material bag
var transient init array<Item>   BagQuestList;		//Quest bag
var transient init array<Item>   BagBankList; //Warehouse
var transient init array<Item>   BagHeroList;		//Hero bag
var transient init array<Item>   BagDepositList;		//Deposit bag, for soul bottles etc., to be extended
var transient init array<Item>   BagResourceList;		//Resource bag

var transient MMOActor			Owner;      // Owner of this bag
var transient EPlayerClass		ClassType;
var transient EGenderType			GenderType;

var transient MMOSkeletalMeshMerge	MergedMesh;		// use the merged mesh instead of separate meshes
//var transient MMOSkeletalMeshMerge	NewMergeMesh;	// replace the old mesh after the merge is done
var transient SkeletalMeshComponent	MergedComp;
var transient bool	bIsMerging;
var transient bool	bIsPreparing;

var transient float	MergePrepareTime;
var transient float	MergeTime;
var transient float	MaxMergeTime;

var transient MMOActorMaterialController MaterialController;

var transient bool bUseCachedTemplate;

// hair & skin color
var transient int	CurrentHairColorIdx;
var transient int	CurrentSkinColorIdx;

var transient int	CurrentHairModelIdx;
var transient int	CurrentFaceModelIdx;

// hide helmet by logic
var transient bool	bHelmetHidden;

// use shoulder ( Left ) instead of face for merging
var transient bool	ReplaceFaceByShoulder;

// Whether to use the new outfit scheme (whole body as one piece)
var transient bool	UseMergedMesh;

native function SendMoveItem( int srcconslot,int srcitemslot ,int desconslot,int desitemslot);

//native function InitMergedMesh();
native function Reset();
native function SetDefaultMeshes( AvatarData InAvatarData, int InKey);
native function Init(MMOActor OwnerTarget);

event Item AddItemToBag( const Item itemTemplate, int Slot, ECONTAINER_SLOT bag,bool bNewPick = false )
{
	local Item  newItem;
	local Equip newEquip;

	local bool useAdvancedAppearence;
	useAdvancedAppearence = IsAdvancedEquipAppearence(bag, Slot);
	newItem = CreateItemByTemplate(itemTemplate, useAdvancedAppearence);
	if( newItem != none )
	{
		switch( bag )
		{
		case E_CONSLOT_EQUIPPED:
			//`Log( "[MMO Debug] Trying to add equip to equipped list. Do this by TakeOn/Off." );
			break;
		//case E_CONSLOT_EQUIPMENT:
		//	if( BagEquipList[slot] != none )
		//	{
		//		`Log( "[MMO Debug] Equip slot"@slot@"not empty while trying to add item"@itemTemplate );
		//	}
		//	BagEquipList[slot] = newItem;
		//	break;
		case E_CONSLOT_DEFAULT:
			BagItemList[slot] = newItem;
			break;
		//case E_CONSLOT_MATERIAL:
		//	BagMaterialList[slot] = newItem;
		//	break;
		//case E_CONSLOT_QUEST:
		//	BagQuestList[slot] = newItem;
		//	break;
		case E_CONSLOT_BANK:
			BagBankList[slot] = newItem;
			break;
		case E_CONSLOT_HERO_EQUIPMENT:
			BagHeroList[slot] = newItem;
			break;
		case E_CONSLOT_HERO_EQUIPPED:
			newEquip = Equip(newItem);
			EquippedItemList_Hero[slot] = newEquip;
			break;
		case E_CONSLOT_DEPOSIT:
			BagDepositList[slot] = newItem;
			break;
		case E_CONSLOT_RESOURCE:
			BagResourceList[slot] = newItem;
			break;
		default:
			break;
		}
	}
	//Just-picked-up item, play pickup sound
	if( bNewPick )
	{
		//itemSound = class'GamePlayDataManager'.static.Singleton().GetItemActionSounds(  newItem.SoundActionType );
		class'GamePlayDataManager'.static.Singleton().PlayItemActionSounds(  Owner, newItem.SoundActionType, EActionSound_Get);
		/*if( itemSound != none && itemSound.Get.Length >0 )
		{
			Owner.PlaySound( itemSound.Get[ Rand( itemSound.Get.Length) ] );
		}*/
	}
	return newItem;
}

native function Item CreateItemByTemplate(Item itemTemp, bool UseAdvancedAppearence);

native final function Item GetItemById( int ItemId, ECONTAINER_SLOT bagType );
native final function int FetchItem( Item pItem, ECONTAINER_SLOT bagType );	// get item and remove from the bag
native final function InsertItem( Item pItem, ECONTAINER_SLOT bagType, int slot );	// insert item into the bag

native final function int GetEmptyItemSlot( ECONTAINER_SLOT bagType ); // Get empty slot index, returns -1 if none
native final function DeleteItemBySlot(ECONTAINER_SLOT bagType ,int slot);
native final function Equip GetEquipBySlot( int slot, bool normalEquip );
native final function Item GetItemBySlot(ECONTAINER_SLOT bagType ,int slot);

//add by JimZhang@snda.com
native final function int GetItemCountById( int ItemId, ECONTAINER_SLOT bagType );

//add by zhangrui05
native final function bool IsAdvancedEquipAppearence(ECONTAINER_SLOT bag, int slot);

event bool CheckNeedRemerge()
{
	local int i;
	for( i = 0; i < EBMT_NoMerge; ++i )
	{
		if( CurrentMeshList[i] != OldMeshList[i] )
			return true;
	}
	return false;
}

native function UpdateOwnerData( optional bool bForce = false );

event UpdateCurrentAttackRange()
{
	local Equip e;
	e = Owner.GetWeaponEquip();
	if(e!=none)
	{
		Owner.CurrentAttackRange = e.FixAttrbutes.AttackRange;
	}
}

/*
 * Request sheathe/draw weapon
 * Param  : whether it is a sheathe or draw operation
 * Return: >0 means success (1 needs operation, 2 needs no operation)
 */
event int RequestHoldWeapon(bool bHold)
{
	Owner.HoldWeapon(bHold);

	return 1;
}

// Request to equip an item. ItemId: item ID, -1: create new item
native function Item RequestTakeOn(int SlotId, int TargetSlotId, bool normalEquip, optional const Item itemTemplate, optional bool autoChagneBag = false);
// Request to unequip, SlotId
native function bool RequestTakeOff(int SlotId, bool normalEquip);

//Get the slot position where an equipment can be worn
function native GetEquippedSlotByEquipType( int EquipType , out int tConSlot , out int tItemSlot  );

native function int RequestExchangeItem(int BagId,int Item1Pos, int Bag2Id,int Item2Pos);

//function SetOpacity(float percent)
//{
//	local int i;
//	for(i = 0; i < CurrentMeshList.Length; ++i)
//	{
//		CurrentMeshList[i].SetOpacity(percent);
//	}
//}


native final function SetSilhouette( bool show, Vector sc );

function StartMeshFade( bool fadeIn, float time )
{
	//local int i;
	//for( i = 0; i < EquippedItemList_Normal.Length; ++i )
	//{
	//	if( EquippedItemList_Normal[i] != none )
	//	{
	//		EquippedItemList_Normal[i].Fade( fadeIn, time );
	//	}
	//}
	//for( i = 0; i < EquippedItemList_Avatar.Length; ++i )
	//{
	//	if( EquippedItemList_Avatar[i] != none )
	//	{
	//		EquippedItemList_Avatar[i].Fade( fadeIn, time );
	//	}
	//}
}

native function bool PrepareMerge();
native function bool CheckPrepareState();
native function AddToMergeMeshCache();

function DoMerge()
{
	`Log( "[MMO Info] Merge Start:"@Owner );

	// double check to avoid the mesh to be merged get rendered...
	if( MergedComp != none && MergedComp.SkeletalMesh == MergedMesh )
	{
		`Log( "[MMO Error] Trying to merge a mesh that is rendering!" );
		MergedComp.SetSkeletalMesh( none );
		//Owner.SetHidden( true );
		Owner.SetVisibleByMergeMesh(false);
	}

	MergedMesh.DoMerge();

	bIsPreparing = false;
	bIsMerging = true;

	MergeTime = 0.0;
}

native function PostMerge();
//{
//	local SkeletalMeshComponent baseComp;
//	baseComp = Owner.GetMasterComponent();
//	if( baseComp == none )
//		return;
//
//	if( MergedMesh == none /*|| !MergedMesh.Check()*/ )
//		return;
//
//	if( MergedComp == none )
//	{
//		MergedComp = new class'SkeletalMeshComponent';
//	}
//
//	MergedComp.SetSkeletalMesh( MergedMesh );
//	MergedComp.SetParentAnimComponent( baseComp );
//	MergedComp.SetLightingChannels( baseComp.LightingChannels );
//	MergedComp.SetLightEnvironment( Owner.LightEnvironment );
//	Owner.LightEnvironment.ResetEnvironment();
//
//	Owner.AttachComponent( MergedComp );
//
//	//if( MaterialController != none )
//	//	MaterialController.Init( MergedComp );
//
//	Owner.RebuildMaterialControllerList();
//	Owner.RefreshAnim();
//
//	`Log( "[MMO Info] Set merged mesh"@MergedMesh@"for"@Owner@". Merge Time:"@MergeTime );
//
//	bIsMerging = false;
//}

function Tick( float DeltaTime )
{
	local int i;

	for( i = 0; i < EquippedItemList_Normal.Length; ++i )
	{
		if( EquippedItemList_Normal[i] != none )
		{
			EquippedItemList_Normal[i].Tick( DeltaTime );
		}
	}
	for( i = 0; i < EquippedItemList_Avatar.Length; ++i )
	{
		if( EquippedItemList_Avatar[i] != none )
		{
			EquippedItemList_Avatar[i].Tick( DeltaTime );
		}
	}

	if( bUseCachedTemplate )
	{
		`Log( "[MMO Info] Use cached template for"@Owner );
		PostMerge();
		//Owner.SetHidden( false );
		Owner.SetVisibleByMergeMesh(true);
		bUseCachedTemplate = FALSE;
		// for debuging the case bIsPreparing is also true...
		if( bIsPreparing )
		{
			`Log( "[MMO Error] bUseCachedTemplate && bIsPreparing! Wrong case." );
		}
	}
	// shouldn't need this 'else'
	else if( bIsPreparing )
	{
		MergePrepareTime += DeltaTime;
		if( MaxMergeTime > 0.f && MergePrepareTime >= MaxMergeTime )
		{
			// print a warning for long time cost jobs
			`Log( "[MMO Info] Merge prepare takes long time for"@Owner );
		}
		MergedMesh.UpdateState();
		if( CheckPrepareState() )
		{
			DoMerge();
		}
	}
	else if( bIsMerging )
	{
		MergedMesh.UpdateState();
		MergeTime += DeltaTime;

		if( /*MergedMesh != none &&*/ MergedMesh.CheckFinish() )
		{
			`Log( "[MMO Info] Done Merge"@Owner );
			MergedMesh.PostMerge();
			PostMerge();
			//Owner.SetHidden( false );
			Owner.SetVisibleByMergeMesh(true);

			// this is a newly merged mesh
			AddToMergeMeshCache();
		}

		if( MaxMergeTime > 0.f && MergeTime >= MaxMergeTime )
		{
			// don't allow time out for now, but do give a warning for long time jobs
			/*`Log( "[MMO Info] Merge time out." );
			MergedMesh.CancelMerge();
			bIsMerging = false;
			Owner.Sethidden( false );*/
			//`Log( "[MMO Info] Merging takes long time for"@Owner );
		}
	}
}

native final function EquipMesh GetDefaultEquipMeshByType(EBodyMeshType type);

native final function ChangeBySeleteMesh(EBodyMeshType type, string path);
native final function ChangeByEquipMesh(EBodyMeshType type, string path);

// interfaces to apply hair/skin color
native final function ApplySkinColor( int idx );
native final function ApplyHairColor( int idx );
native final function ApplyEmissiveSilhouetteFactor( float factor );

native final function UpdateBeardColor();

native final function ApplyFaceModel(int profession, int sex,int id);
native final function ApplyHairModel(int profession, int sex,int id);

native final function SetHair( int ModelID, int ColorID );
native final function SetFace( int ModelID );

native final function UpdateApperance();

// type: the index of the mesh in the default mesh list;
// colorOnly: only refresh the color texture( not the base texture )
native final function RefreshSkinTexture( int defaultMeshID, bool colorOnly );

// hide a specific part by logic
//native final function HideByType( int type, bool hidden );
native final function HideHelmet( bool bHidden );
native function ChangeEquipMeshMaterial( EBodyMeshType type, string materialPath );
cpptext
{
	//template< typename CActionClass >
	//void PalyAnimOnSkeletalEquipMesh( FName NewAnim )
	//{
	//	for ( int i = 0 ; i < EquippedItemList_Normal.Num() ; ++i )
	//	{
	//		UEquip* WeaponEquip = EquippedItemList_Normal( i );
	//		if ( WeaponEquip )
	//		{
	//			for ( int j = 0 ; j < WeaponEquip->MeshList.Num() ; ++j )
	//			{
	//				UEquipMesh *pEquipMesh =  WeaponEquip->MeshList( j );
	//				if(pEquipMesh)
	//				{
						
	//				}
	//				if ( pEquipMesh && pEquipMesh->ResourceType == ECRT_SkeletalMesh && pEquipMesh->AnimationType == EUAT_SelfAnimation  )
	//				{
	//					if ( pEquipMesh->AlwaysUseIdleAnim && pEquipMesh->MeshType == EBMT_BackOrnamental )
	//					{
	//						NewAnim = pEquipMesh->IdleAnim;
	//					}
	//					UMMOActorAT* pAnimCtr = pEquipMesh->AnimationController;
	//					if ( pAnimCtr )
	//					{
	//						CActionClass::PalyAnim( pAnimCtr, NewAnim );
	//					}
	//				}
	//			}
	//		}	
	//	}
	
	//}
}

DefaultProperties
{
	Capacity=48
	MaxMergeTime=10.0	// negative value means no time limit
	bUseCachedTemplate=false
	
	CurrentHairColorIdx=-1
	CurrentSkinColorIdx=-1
	CurrentHairModelIdx=-1
	CurrentFaceModelIdx=-1

	bHelmetHidden=false

	ReplaceFaceByShoulder=false
}
