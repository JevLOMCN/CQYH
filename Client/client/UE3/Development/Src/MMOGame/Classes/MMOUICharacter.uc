class MMOUICharacter extends MMOUIBaseSlotContainer
	dependson(MMOUIEquipSlotEnhancePanel)
	native(UI)
	config(UIVisibleMask);

var TurnButtonStats TurnButton;

enum CharaPanelType
{
	EChaPT_Equip,
	EChaPT_Reputation,
	EChaPT_Title,
	EChaPT_Money,
	EChaPT_Enhance
};
var CharaPanelType _CurType;

//add by jimzhang@sdna.com add parallel tab pages, increase the movie depth of the original character panel
var GFxObject showPanel;
var GFxObject relationPanel;
var GFxObject famePanel;

//Physique enhancement
var MMOUIEquipSlotEnhancePanel _EnhcPanel;

var GFxObject _AttributePanel;


//var GFxObject     EquipWeapon;
//var GFxObject     EquipCloth;
//var GFxObject     EquipShoe;
//var GFxObject       EquipHelmet;
//var GFxObject       EquipTrouser;
//var GFxObject       EquipShoulder;
//var GFxObject       EquipWrist;
//var GFxObject       EquipSuit;

var array<GFxObject> EquipSlotList;

var GFxObject     AttributePanel;

var String	ShowLevelName;
var LevelStreamingUIShow	ShowLevel;
var GFxObject       MagicUpTxt;
var GFxObject       MagicTxt;
var GFxObject       ManaValue;

var bool ResetCharactorDataFlag;
var float QueryPowerCD;

struct native ReputationData
{
	var int     TempIndex;
	var int     type;
	var int     ForceID;
	var int	    ForceLevel;
	var int     InitValue;
	var int     MaxValue;
	var init String  ForceName;
	var init String  ForceTips;
	var init String  ForceInfo;
	var bool    Visible;
	var bool    awardState;
};
var array<ReputationData> ReputationList;
//var array<string> ReputationName;
//var array<int> ReputationValue;

native function ToogleVisible();

// todo: no close event, cannot actively close SceneCapture
native function EnableCapture(bool b);


native function CreateShowLevel();

native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);
//
native function bool OnEquipPanelRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnEquipPanelDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

native function ClearUI();

//native function NativeOnShow(bool v);

event OnSubscribeItemSlotAction()
{
}

event OnUnSubscribeItemSlotAction()
{
}


native function NativeShowInfoBox(GFxObject data);
native function NativeRefreshInfoBox(GFxObject data);

event CallShowReputationInfo( String TitleName ,String MessageInfo, bool bShow )
{
	ActionScriptVoid("ShowReputationInfo");
}
event CallRefreshReputationInfo( String TitleName ,String MessageInfo )
{
	ActionScriptVoid("RefreshReputationInfo");
}

native function EnterScene();

native function ChangeScene();

event CallUpdateBattlePoint( GFxObject data )
{
	ActionScriptVoid("UpdateBattlePoint");
}

event CallUpdateBaseInfo( GFxObject data )
{
	ActionScriptVoid("UpdateBaseInfo");
}

event CallSetAttributeData( GFxObject data )
{
	ActionScriptVoid("SetAttributeData");
}

event CallSetAttributeItem( GFxObject data )
{
	ActionScriptVoid("SetAttributeItem");
}



event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);

	InitReputationData();
	ShowLevel = none;

	pl.AddPostAdvanceDelegate(PostAdvance);
}


native function PlayerTitleChange( int TitleID ); 
native function int KeyToNumber(String Key, int index);
native function InitReputationData();
native function UpdateReputationData();
//native function DisplayReputationData(int ForceID, int ReputationNow, int ReputationLast );
native function ChangeReputationType(int ID,int ReputationType);
native function ResetReputationTypeConfig();

native function ShowTraceReputationInfoPanel();
native function UpdateTraceReputationDisplay();
native function UpdateTitleData(bool bReset);
native function UpdateMoneyData();
native function UpdatePlayerReputation( MMOPlayer Pl , int ForceID, int newReputation, int lastReputation );

native function int GetReputationLevel(int ForceID,int Reputation);
native function string GetReputationLevelName(int ForceID,int Reputation);
native function string GetReputationForceName(int ForceID);
native function bool GetReputationVisible(int ForceID);


//function SetItemsData()
//{
//	local int i;
//	local GFxObject DataProvider;
//	local GFxObject ItemObj;
//	//local GFxObject Obj;
//	//local array<ASValue> args;

//	DataProvider = CreateArray();

//	for(i=0;i<28;i++)
//	{
//		ItemObj = CreateObject("Object");
//		ItemObj.SetString("label","item"@i);
//		ItemObj.SetInt("index",i);
//		ItemObj.SetInt("type",0);

//		ItemObj.SetInt("deepth",1);
//		ItemObj.SetInt("curReputation",3*i);
//		ItemObj.SetInt("maxReputation",100);
//		ItemObj.SetString("txtReputation","Relationship");

//		if(i==0||i==3||i==6)ItemObj.SetInt("type",1);
//		if(i==0||i==6)ItemObj.SetInt("deepth",0);
//		if(i==4||i==5)ItemObj.SetInt("deepth",2);

//		DataProvider.SetElementObject(i,ItemObj);
//	}

//	CallUpdateItemsData(DataProvider);
//}


event CallUpdateItemsData(GFxObject Provider)
{
	ActionScriptVoid("updateItemsData");
}

event CallUpdateFameData(GFxObject Provider, bool bReset)
{
	ActionScriptVoid("updateFameData");
}

event CallResetFameData()
{
	ActionScriptVoid("resetFameData");
}

event CallUpdateMoneyData(GFxObject Provider)
{
	ActionScriptVoid("updateMoneyData");
}

event CallUCUpdateTabClicked()
{
	ActionScriptVoid("UCUpdateTabClicked");
}

event CallUpdatePlayerHPMP(int type, int v,int max)
{
	ActionScriptVoid("updatePlayerHPMP");
}

native function NativeInitOnce(); 

native function PlayerLevelChange( int lastLv , int newLv );
native function PlayerBaseInfoChange();

native function BagSlotChange(int BagId,int SlotID);
native function ItemChange(int ContainerID, int SlotID);
native function ItemDelete(int ContainerID, int SlotID);

native function UpdateArmorUI( );
native function UpdateShowAvatar( int SlotID );
native function UpdateRecover();
native function InitRecover();

native function ModelEquip();

native function SysModelEquip();

native function ResetCharactorData();
//native function UpdateAttributePanel();


final native function ClickTab( int tabID );

//function ToggleCharactor()
//{
//	ChangeVisible();
//	//`Log( "Character UI Open"@IsVisible );

//	if( !IsVisible)
//	{
//		return;
//	}
	
//	UpdateArmorUI();
//	UpdateAttributePanel();
//	EnableCapture( true );
//	SysModelEquip();
//	GotoAndStopI(0); // oscar@snda
//}

function TurnLeft()
{

}

function TurnRight()
{
}


native final function PlayerHPChange(int NewHP, int MaxHP);
native final function PlayerMPChange(int NewMP, int MaxMP);
native function PlayerAttributeChange( int AtrrId, int inValue );
native function PlayerPkValueChange( int PkValue );
native function PlayerExpChange(int currentExp,int MaxExp,INT ChangeExp);
native function PlayerMoneyChange(EMONEY_TYPE MoneyType, int DeltaMoney);


function ASCharactorTab( int TabId )
{
	ClickTab( TabId );
}


function ASShowInfoBox( GFxObject data)
{
	//NativeShowInfoBox( data );
}

function ASRefreshInfoBox( GFxObject data )
{
	NativeRefreshInfoBox( data );
}

function ASChangeReputationType( int ID,int ReputationType )
{
	ChangeReputationType( ID, ReputationType );
}

function ASShowTraceReputationInfoPanel()
{
	//ShowTraceReputationInfoPanel();
}

native function ASTitleTabClick(int ShowType);
native function ASUseCurrentTitle( int TitleId );
native function ASClickTitle( int TitleId );

native function ASShowTotalEnhancProp();
native function ASHideTotalEnhancProp();

native function ASShowTips_EnhanceGuide( GFxObject mc );
native function ASHideTips_EnhanceGuide( GFxObject mc );

function TurnRelease()
{
	TurnButton = TBS_None;

}

function ButtonDrag( int deltaX )
{
	local rotator newRot;
	newRot = GFXPlayer.ShowAvatar.Rotation;
	newRot.Yaw += -deltaX*100; //3030 is the coefficient between mouse movement pixels and angle value
	GFXPlayer.ShowAvatar.SetPhysics(PHYS_None);
	GFXPlayer.ShowAvatar.SetRotation(newRot);
}

function TurnPress( bool bLeft)
{
	if(GFXPlayer.ShowAvatar != none)
		GFXPlayer.ShowAvatar.SetPhysics(PHYS_None);

	if( bLeft)
		TurnButton = TBS_Left;
	else
		TurnButton = TBS_Right;
}

native function PostAdvance(float DeltaTime );

//===============================================================================================================
//===============================================================================================================
//Physique enhancement
native function ASSelectEquipSlot(int slotId);
native function ASShowTips_EquipSlot(int id);
native function ASHideTips_EquipSlot(int id);
native function ASShowTips_ActiveIcon(GFxObject mc);
native function ASEnhanceEquipSlot();

native function EnhanceEquipSlotOK();
native function EnhanceEquipSlotCancel();
//===============================================================================================================
//===============================================================================================================


DefaultProperties
{
	ShowLevelName="PawnShowLevel"
	ResetCharactorDataFlag=false
	QueryPowerCD=0.0
	TurnButton=TBS_None

	FriendlyName=Charactor
}
