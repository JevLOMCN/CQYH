class MMOUIWeaponExpPanel extends MMOUIBaseSlotContainer
	native(UI)
	config(MMOGame)
	;

var GFxObject WeaponSlot;
var GFxObject WeaponExpText;
var array<GFxObject> Bottle;
var array<GFxObject> BottleNeedText;
var array<GFxObject> BottleHaveText;
var array<GFxObject> BottleIcon;

var const int BottleNumber;

var config array<int> BottleItemTempId;
var config array<int> BottleExp;
var config array<int> BottleNeedNum;

event InitOnce()
{
	local int i;
	
	Bottle.Add(BottleNumber);
	BottleNeedText.Add(BottleNumber);
	BottleHaveText.Add(BottleNumber);
	BottleIcon.Add(BottleNumber);
	
	for( i = 0 ; i < BottleNumber ; i++ )
	{
		Bottle[i] = GetObject("CostType"$i);
		if( Bottle[i] != none )
		{
			Bottle[i].SetString( "label" , string( BottleExp[i] ) );
			BottleIcon[i] = Bottle[i].GetObject("icon_mc");
			if( BottleIcon[i] != none )
			{
				BottleNeedText[i] = BottleIcon[i].GetObject("tf_cnt");
				BottleHaveText[i] = BottleIcon[i].GetObject("tf_curHave");
			}			
		}
	}

	WeaponExpText = GetObject("tF_exp");

	WeaponSlot = GetObject("weaponSlot");

	if( WeaponSlot != none )
	{
		WeaponSlot.SetInt( "SlotType" , ESPType_WeaponExp );
	}


	super.InitOnce();
	
}

native function NativeInitOnce();

native function GetWeaponExp();

native function UpdateBottleCount();

native function RemoveWeaponItem( INT ContainerID , INT SlotID );
native function AddWeaponItem( INT ContainerID , INT SlotID );

event OnShowWeaponExp(bool show)
{
	local int LastContainerID;
	local INT LastSlotID;

	//super.OnShow(show);

	if(show)
	{
		GFXPlayer.SetItemSlotData( WeaponSlot , none , ESPType_WeaponExp );
		WeaponExpText.SetText("0");
		UpdateBottleCount();
	}
	else
	{
		LastContainerID = WeaponSlot.GetInt("containerID");
		LastSlotID = WeaponSlot.GetInt("slotID");

		if( LastContainerID == E_CONSLOT_DEFAULT /*E_CONSLOT_EQUIPMENT*/ )
		{
			//First remove the previously drawn weapon
			RemoveWeaponItem(LastContainerID,LastSlotID);
		}
	}

}

//native function ToggleWeaponExpPanel();


native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

DefaultProperties
{
	IsDynamicLoad=true
	BottleNumber=3
}
