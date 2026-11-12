/*
 * base class for equip
 * */

class Item extends Object implements(IExcelable,IReferenceID)
	hidecategories(Object)
	dependson(TypeDefine)
	native
	hidecategories(Object,Debug,Advanced,Physics,CollisionChanged,Attachment,DisplayDebug,Display,Movement,Collision);

var(Item)   int             ItemId				<ExportExcel="0"|DisplayName="ItemId">;
var(Item)   private{private} Texture2D	    DisplayIcon			<ExportExcel="24"|DisplayName="DisplayIcon">;
var(Item)   private{private} Texture2D	    DisplayLargeIcon			<ExportExcel="33"|DisplayName="DisplayLargeIcon">;
var(Item)   private{private} Texture2D	    DisplayLargeIconFemale		<ExportExcel="34"|DisplayName="DisplayLargeIconFemale">;

var string DisplayIcon_Str;
var string DisplayLargeIcon_Str;
var string DisplayLargeIconFemale_Str;

//var         int             InstanceID			<DisplayName="InstanceID">;
var(Item)   string     ItemName			<ExportExcel="1"|DisplayName="ItemName">;
var(Item)   int             ItemLevel           <ExportExcel="31"|DisplayName="ItemLevel">;
var(Item)   string          Description			<ExportExcel="25"|DisplayName="Description">;
var(Item)   int             MaxStackNum			<ExportExcel="26"|DisplayName="MaxStackNum">;  // min 1: can't overlap; numbers below 1 has no meaning

var(Item)  private{private} StaticMesh  DropMesh					<ExportExcel="27"|DisplayName="DropMesh">;
var string DropMesh_Str;

var transient ParticleSystemComponent   EmitterComponent;
var transient MMOActor Owner;


// Item type
var(Item) string ItemType					<ExportExcel="2"|DynamicList="ItemType">;

// Item subtype (just specify on export)
//var (Item)EItemSubType ItemSubType				<ExportExcel="3"|DisplayName="ItemSubType">;
var(Item) string ItemSubType					<ExportExcel="3"|DynamicList="ItemSubType">;

var(Item) int RequireLevel						<ExportExcel="4"|DisplayName="RequireLevel">;

var(Item) Spell_ClassType RequireClass			<ExportExcel="5" |Merger="0"|DisplayName="RequireClass">;//EPlayerClass 
var(Item) EGenderType RequireGender				<ExportExcel="6"|DisplayName="RequireGender">;
var(Item) int MaxEquip							<ExportExcel="7"|DisplayName="MaxEquip">;
var(Item) int MaxOwn							<ExportExcel="8"|DisplayName="MaxOwn">;

// Item color
var(Item) EItemColor ItemColor					<ExportExcel="9"|DisplayName="ItemColor">;
// Item bind type
var(Item) EBIND_TYPE ItemBindType			<ExportExcel="10"|DisplayName="ItemBindType">;
// Item durability
var(Item) EDUR_TYPE ItemDuration		<ExportExcel="11"|DisplayName="ItemDuration">;
// Current durability
var(Item) int DefaultDuration					<ExportExcel="12"|DisplayName="DefaultDuration">;
//After being placed in a stall it is split; the server only keeps a reference, while the client needs to fake the actual move; this records the original durability
//var int OriDuration;


//Ability referenced by the item
var(Item) protected{protected} MMOAbility ToUseAbility				<ExportExcel="13"|DisplayName="ToUseAbility">;
var transient int ToUseAbility_SpellID;

var(Item) int SpellLevel						<ExportExcel="14"|DisplayName="SpellLevel">;

//Item use cooldown
var(Item) float SelfCoolDown					<ExportExcel="15"|Convert="1000"|DisplayName="SelfCoolDown">;
var(Item) string GroupCoolDown					<ExportExcel="16"|DisplayName="GroupCoolDown">;

//Equipment sub-subtype
var(Item) string ItemSubSubType					<ExportExcel="17"|DynamicList="ItemSubSubType">;

var init array<INT> AttributeList; //by york, all attributes are here, no other attributes are used
//Sell price: the price when selling the item to the system. Sellable: whether the item can be sold to a shop
var(Item) int SellPrice							<ExportExcel="18"|DisplayName="SellPrice">;
//Use state: in which state the item can be used, distinguished only by the highest-level state
var(Item) EITEM_COMBAT_TYPE UsedStatus			<ExportExcel="19"|DisplayName="UsedStatus">;
//Discard: whether the item can be discarded
var(Item) bool Drop								<ExportExcel="20"|DisplayName="Drop">;
////Sellable: whether the item can be sold to a shop
//var(Item) int	Shell <ExportExcel="21">;
//Tradable: whether players can trade the item; only unbound items can be traded
//var(Item) bool Trade							<ExportExcel="21"|DisplayName="Trade">;
//Storable: whether the item can be put into storage such as a warehouse
var(Item) bool Storage							<ExportExcel="22"|DisplayName="Storage">;

var(Item) bool CanSell							<ExportExcel="32"|DisplayName="CanSell">;
var(Item) bool Expensive						<ExportExcel="38"|DisplayName="Expensive">;
var(Item) bool CanDecompose                     <ExportExcel="39"|DisplayName="CanDecompose">;

var(Item) string Script					<ExportExcel="23"|DisplayName="Script">;

var int byPrefixLevel;

var(Item) int LimitDuration <ExportExcel="35"|DisplayName="LimitDuration">;
var(Item) int LimitCount <ExportExcel="36"|DisplayName="LimitCount">;
var(Item) bool RecordTradeLog <ExportExcel="37"|DisplayName="RecordTradeLog">;

var(Item) bool Auction;
var(Item) int AuctionFee1<ClampMin=0 | ClampMax=1000000000>;
var(Item) int AuctionFee2<ClampMin=0 | ClampMax=1000000000>;
var(Item) int AuctionFee3<ClampMin=0 | ClampMax=1000000000>;
var(Item) byte AuctionType;
var(Item) byte AuctionSubType;
var(Item) int AuctionDefaultPrice;
var(Item) int Weight;
var(Item) int ItemPack;
var(Item) int ItemPack2;
var(Item) int ShopTypeSell;
var(Item) bool TradeNeedIntimacy;

var(Item) bool CanResource;
//---------------------------------------------------------------
// Sound effect config
//---------------------------------------------------------------
// only for sound effect
var(SoundFX) EItemMaterialType SoundMaterialType		<ExportExcel="28"|DisplayName="SoundMaterialType">;
var(SoundFX) EItemActionSoundType SoundActionType		<ExportExcel="29"|DisplayName="SoundActionType">;

//var(SoundFX) array<SoundCue> Sound_Use;
//var(SoundFX) array<SoundCue> Sound_Get;
//var(SoundFX) array<SoundCue> Sound_Fall;
//var(SoundFX) array<SoundCue> Sound_Land;

//var(SoundFX) array<SoundCue> Sound_Attack				<DisplayName="Sound_Attack">;
//var(SoundFX) array<SoundCue> Sound_CriticalAttack		<DisplayName="Sound_CriticalAttack">;
var(SoundFX) EClothNoiseType ClothNoiseType             <ExportExcel="30"|DisplayName="ClothNoiseType">;

//native function PlayRandomSound(array<SoundCue> soundList);

native function Init( EPlayerClass pc, EGenderType gt, bool useAdvancedApperance );
// Item discarded (may need to be placed on the ground)
//native function bool OnDrop(DropItemActor Target, bool bNew,Vector DorpLocation,Rotator DropRotation);
native function bool IsUseblForActor( MMOActor actor);
//native function String GetLargeIconName();

native function bool IsWeapon();
native function bool IsEquip();

// When this item is used/equipped, OnTakeOn is called, Target is the target; usually handles equipment-related content here, e.g. how many attributes this item adds to the target
//event bool OnTakeOn(MMOActor Target)
//{
//	return true;
//}

native function bool OnTakeOn(MMOActor Target, bool UseAdvancedAppearence);
native function bool OnTakeOff(MMOActor Target);

//event bool OnTakeOff(MMOActor Target)
//{
//	return true;
//}

event OnPickUp(MMOActor Target);

function AttachDropEmitter()
{
	//EmitterComponent = new class'ParticleSystemComponent';
	//EmitterComponent.SetTemplate( ParticleSystem (DynamicLoadObject( "FX_Scene.Interaction.WuPingDiaoLuo",class'ParticleSystem')  )  );
	//EmitterComponent.SetTranslation(RelativeLocation);
	//EmitterComponent.SetRotation(RelativeRotation);
	//AttachComponent( EmitterComponent );
}
function DetachDropEmitter()
{
	//DetachComponent(EmitterComponent);
	
}



function SetEmissiveColor(LinearColor c);

function SetOpacity(float percent);

native function bool IsNeedArchetype();

function Tick( float DeltaTime )
{
}

//nStatenState usage convention
native function String GetIconName(int nState = 0);

//-------------------------------------------------------
// export to Excel
//-------------------------------------------------------

// export table header, call the script function first, then the native function
event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues)
{


}

// export each row of data, call the script function first, then the native function
event ScriptDataToExcel(out array<string> Row)
{	
}

native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function ImportFromExcel(array<string> values);
event ScriptImportFromExcel(array<string> Values)
{
	//local string val;

	//val = values[0];
	//ItemId = int(val);
}
native function InitDataToExcel();

event string GetDetailedDescription( int InIndex )
{
	local string locDescription;
	switch( InIndex )
	{
	case 0:
		locDescription = string(ItemId);
		break;
	case 1:
		locDescription = ItemName;
		break;
	case 2:
		locDescription = Description;
		break;

	default:
		break;
	}

	return locDescription;
}

native function int GetTemplateID() const;
native final function string GetDisplayName() const;

DefaultProperties
{
}
