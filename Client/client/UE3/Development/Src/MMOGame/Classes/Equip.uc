/*
 * equipment class, functions include clothing, armor, weapons and other visible objects
 * supports four kinds of visible objects: swapped static mesh, skeletal skinning, particle, texture
 * 
 * for non-texture items, models are combined as Components in two ways: one is added to the Actor's Components array, the other is attached to a socket of the Base skeleton
 *
 * for SkeletalMesh there are two kinds of animation: one uses the base animation, the other uses independent animation, which requires configuring the animation tree, animation set, and playback node info.
 * 
 * 
 * 
 */
class Equip extends Item
	native;

struct native FixAttr
{
	var() float WeaponPhyAtkLo	<DisplayName="WeaponPhyAtkLo"|ExportExcel="0">;
	var() float WeaponPhyAtkHi	<DisplayName="WeaponPhyAtkHi"|ExportExcel="1">;
	var() float WeaponMagAtkLo	<DisplayName="WeaponMagAtkLo"|ExportExcel="2">;
	var() float WeaponMagAtkHi	<DisplayName="WeaponMagAtkHi"|ExportExcel="3">;
	var() float WeaponTaoAtkLo	<DisplayName="WeaponTaoAtkLo"|ExportExcel="4">;
	var() float WeaponTaoAtkHi	<DisplayName="WeaponTaoAtkHi"|ExportExcel="5">;
	var() int AttackRange		<DisplayName="AttackRange"|ExportExcel="6">;
	var() int PhyDefLo			<DisplayName="PhyDefLo"|ExportExcel="7">;
	var() int PhyDefHi			<DisplayName="PhyDefHi"|ExportExcel="8">;
	var() int MagDefLo			<DisplayName="MagDefLo"|ExportExcel="9">;
	var() int MagDefHi			<DisplayName="MagDefHi"|ExportExcel="10">;
    //var() int Strength		<DisplayName="Strength"|ExportExcel="6">;
    //var() int Agility			<DisplayName="Agility"|ExportExcel="7">;
    //var() int Intellect		<DisplayName="Intellect"|ExportExcel="8">;
    //var() int Stamina			<DisplayName="Stamina"|ExportExcel="9">;
    //var() int Spirit			<DisplayName="Spirit"|ExportExcel="10">;
    var() int MaxHp				<DisplayName="MaxHp"|ExportExcel="11">;
    var() int MaxMp				<DisplayName="MaxMp"|ExportExcel="12">;

	var() int Accurate		    <DisplayName="Accurate"|ExportExcel="13">;
	var() int Agility		    <DisplayName="Agility"|ExportExcel="14">;
	var() int AtkSpeed          <DisplayName="AtkSpeed"|ExportExcel="15">;
	var() int MagDodge		    <DisplayName="MagDodge"|ExportExcel="16">;

	//var() float BlockRate    <DisplayName="BlockRate"|ExportExcel="9"|Convert="10000">;
	//var() float ParryRate    <DisplayName="ParryRate "|ExportExcel="10"|Convert="10000">;
	//var() int BlockValue    <DisplayName="BlockValue"|ExportExcel="11"|ClampMin=0|ClampMax=65535>;
	//var() float CritDamageModificationRate    <DisplayName="CritDamageModificationRate "|ExportExcel="12"|Convert="10000"|ClampMin=0|ClampMax=5>;
	var() int WeatherAttrID					    <DisplayName="WeatherID"|ExportExcel="17">;
	//var() int FoundationRandomAttribute     <DisplayName="FoundationRandomAttribute"|ExportExcel="14"|ClampMin=1|ClampMax=65535>;

	var() int BasePow               <DisplayName="BasePower"|ExportExcel="18">;
};

struct native EquipSkill
{
	var() int WeaponRuneID			    <DisplayName="WeaponRuneID"|ExportExcel="0">;
};

var transient MMOActor		BaseActor;

var deprecated editinline array< EquipMesh >	MeshListTemplate_M	<DisplayName="MeshListTemplate_M">;
var deprecated editinline array< EquipMesh > MeshListTemplate_F	<DisplayName="MeshListTemplate_F">;

var() private{private} EquipAppearence EquipAppearenceTemplate <ExportExcel="42"|DisplayName="EquipAppearenceTemplate">;
var string EquipAppearenceTemplate_Str;

// task #21843 add by zhangrui05@snda
var() private{private} EquipAppearence AdvancedEquipAppearenceTemplate <ExportExcel = "49" | DisplayName = "AdvancedEquipAppearenceTemplate">;
var string AdvancedEquipAppearenceTemplate_Str;

var	transient array< EquipMesh>	MeshList;

//export linked to Item
var() int MaxRefineLevel								<ExportExcel="38"|ClampMin=0 | ClampMax=300|DisplayName="MaxRefineLevel">;
//fixed attributes
var() FixAttr FixAttrbutes								<ExportExcel="39"|DisplayName="FixAttrbutes">;
//additional attributes
//var() array<AddAttr> AddAttrList						<ExportExcel="33"|DisplayName="AddAttrList">;
//whether identification is required
var() bool NeedIdentify									<ExportExcel="40"|DisplayName="NeedIdentify">;

//weapon rune
var() int MaxRuneCount				<ExportExcel="41"|DisplayName="MaxRuneCount">;
//use the DropMesh in Item uniformly
//var()	StaticMesh			DropMeshTemp;
var() int SocketCount									<ExportExcel="43"|DisplayName="SocketCount">;

var() int RepairCost; // repair price, bound gold
var() int RepairCostSpecial; // special repair price
var() int IdentifyCost; // identify price, bound gold
var() int RandomizeCost; // reforge price, bound gold

var() int RequireMaxPhyAtk					<ExportExcel="45"|DisplayName="RequireMaxPhyAtk">;
var() int RequireMaxMagAtk					<ExportExcel="46"|DisplayName="RequireMaxMagAtk">;
var() int RequireMaxTaoAtk					<ExportExcel="47"|DisplayName="RequireMaxTaoAtk">;

var() bool CanRepair					<ExportExcel="48"|DisplayName="CanRepair">;

var() bool IsIconSame;

native function Init(EPlayerClass pc, EGenderType gt, bool useAdvancedApperance);
//native function bool OnDrop(DropItemActor Target, bool bNew,Vector DropLocation,Rotator DropRotation);

//native function SetVisible(bool bVisible);
//native function AddAttachedEmitter(MMOEmitter InEmitter, int InIndex);
//native function DestroyAttachedEmitters();


native function bool IsWeapon();
native function bool IsEquip();


//function SetHidden(bool b)
//{
//	local EquipMesh em;
//	foreach MeshList( em )
//	{
//		em.SetHidden( b );
//	}
//}

//native function PlayAnim(name Anim, bool bLoop, float InRate, float InStartTime);
//native function PlayAnimByKey(name Anim, bool bLoop, float InRate, float InStartTime, int InKey);
//native function StopAnimByKey(int InKey);
//native function OnAnimEnd(AnimNodeSequence SeqNode, float PlayedTime, float ExcessTime);
//native function RefreshAnim();
native final function EquipAppearence GetEquipAppearenceTemplate();
native final function EquipAppearence GetAdvancedEquipAppearenceTemplate();

//function SetOpacity(float percent)
//{
//	local EquipMesh em;
//	foreach MeshList( em )
//	{
//		em.SetOpacity( percent );
//	}
//}


function PrimitiveComponent CreateCollisionComponent()
{
	local PrimitiveComponent Comp;
	Comp = new class'CylinderComponent';
	return Comp;
}


native function bool OnTakeOn(MMOActor Target, bool UseAdvancedAppearence);

native function bool OnTakeOff(MMOActor Target);

//event bool OnTakeOff(MMOActor Target)
//{
//	local EquipMesh em;

//	super.OnTakeOff(Target);

//	if(Target == none)
//	{
//		`log("Target is none, return Armor.uc:115");
//		return false;
//	}

//	if( BaseActor != Target )
//	{
//		`Log( "[MMO Debug] Can't take off equip"@self@"from actor"@Target );
//		return false;
//	}

//	BaseActor = none;

//	DestroyAttachedEmitters();

//	foreach MeshList( em )
//	{
//		// todo: check failure
//		//if( em.MeshType == EBMT_None )
//			if( em.OnTakeOff( Target ) )
//			{
//				//if( em.MeshType == EBMT_Weapon )
//				//{
//				//	if( em.SocketName == 'Item_WeaponLeftHand' )
//				//	{
//				//		Target.ItemManager.CurrentMeshList[EBMT_Weapon] = none;
//				//	}
//				//	else if( em.SocketName == 'Item_WeaponRightHand' )
//				//	{
//				//		Target.ItemManager.CurrentMeshList[EBMT_Weapon2] = none;
//				//	}
//				//	else
//				//	{
//				//		`Log( "[MMO Debug] Wrong weapon socket name:"@em.SocketName@em );
//				//		continue;
//				//	}
//				//}
//				//else
//					Target.ItemManager.CurrentMeshList[em.MeshType] = none;
//			}
//	}

//	`Log( "[MMO Debug] OnTakeOff: "@ EquipAppearenceTemplate.EquipType );

//	//Target.ItemManager.EquippedItemList[EquipType] = none;

//	return true;
//}

native final function SetSilhouette( bool show, Vector sc );


//function Fade( bool fadeIn, float fadeTime )
//{
//	local EquipMesh em;
//	foreach MeshList( em )
//	{
//		if(em.MaterialController == none)
//		{
//			em.MaterialController = new class'MMOActorMaterialController'();
//			em.MaterialController.Init(MeshComponent(em.PrimComponent));
//		}

//		if( em.MaterialController != none )
//		{
//			if( fadeIn )
//				em.MaterialController.PlayOpacityFadeIn( 1.f / fadeTime );
//			else
//				em.MaterialController.PlayOpacityFadeOut( 1.f / fadeTime );
//		}
//	}
//}

//// for weapons, shields, etc. that can switch between held and stowed states
//event bool SwitchHold(bool bHoldInHand)
//{
//	local EquipMesh em;

//	if( !IsWeapon() )
//	{
//		`warn( "Trying to switch hold for a non weapon item." );
//		return false;
//	}

//	foreach MeshList( em )
//	{
//		em.SwitchHold( BaseActor, bHoldInHand );
//	}

//	return true;
//}

function Tick( float DeltaTime )
{
	local EquipMesh em;
	foreach MeshList( em )
	{
		if( em.MaterialController != none )
		{
			em.MaterialController.Tick( DeltaTime );
		}
	}
}

native function String GetIconName(int nState = 0);
native function String GetIconNamePostfixBy(int actorClass);
//-------------------------------------------------------
// export to Excel
//-------------------------------------------------------

// export table header, call the script function first, then the native function
event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues)
{
	super.ScriptExportExcelHeaders(Header, DefaultValues);
}

// export each row of data, call the script function first, then the native function
event ScriptDataToExcel(out array<string> Row)
{
	super.ScriptDataToExcel(Row);
}

native final function bool HasAdditiveAttributes() const;

DefaultProperties
{	
	//ItemType=EIT_ARMOR
	//ShowWeaponType=true
	//SocketCount=0
	//EquipedSkills(0)=()
	//EquipedSkills(1)=()
	//EquipedSkills(2)=()
	//EquipedSkills(3)=()
	//EquipedSkills(4)=()
	//EquipedSkills(5)=()
	//AddAttrList(0)=()
	//AddAttrList(1)=()
	//AddAttrList(2)=()
	//AddAttrList(3)=()
	//AddAttrList(4)=()
	//AddAttrList(5)=()
	//AddAttrList(6)=()
	//AddAttrList(7)=()
	//AddAttrList(8)=()
	//AddAttrList(9)=()
	//AddAttrList(10)=()
	//AddAttrList(11)=()
	//AddAttrList(12)=()
	//AddAttrList(13)=()
	//AddAttrList(14)=()
	//AddAttrList(15)=()
}
