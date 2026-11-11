// 1.05 wing panel ("UnrealWing", UI_UnrealWing.upk / UnrealWing.swf), mirrors MMOUIMountPanel.
// The wing list is built from wings.txt, the level / upgrade data from wings_level.txt, the owned wings and the wing level
// come from g2c_wing_info.
class MMOUIWingPanel extends MMOUIBaseSlotContainer
native(UI)
config(UIVisibleMask);


var int SelectWingID;
var int BackShowType;

native function UpdateWing();

native function int GetSlotType();

native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);
native function bool OnDoubleClickSlot(GFxObject SrcSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, bool bDefault);


event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

event InitOnce()
{
	super.InitOnce();
}

// UnrealWingPanel.UCUpdateWingData(data)
event CallUpdateWingData(GFxObject Data)
{
	ActionScriptVoid("UCUpdateWingData");
}

// UnrealWingPanel.UCSetLocalizationData(data)
event CallSetWingLocalization(GFxObject Data)
{
	ActionScriptVoid("UCSetLocalizationData");
}

// UnrealWingPanel.UCOnWingUpgrade()
event CallOnWingUpgrade()
{
	ActionScriptVoid("UCOnWingUpgrade");
}

// The pieces of UCUpdateWingData, fed one by one so that one failing AS function cannot block the others.
// UnrealWingPanel.UpdateWingList(array)
event CallUpdateWingList(GFxObject Data)
{
	ActionScriptVoid("UpdateWingList");
}

// UnrealWingPanel.UpdatePropertyList(array)
event CallUpdateWingProps(GFxObject Data)
{
	ActionScriptVoid("UpdatePropertyList");
}

// UnrealWingPanel.UpdatWingSlots(array)
event CallUpdateWingSlots(GFxObject Data)
{
	ActionScriptVoid("UpdatWingSlots");
}

// UnrealWingPanel.UCUpdateUpgradePanel(data)
event CallUpdateWingUpgrade(GFxObject Data)
{
	ActionScriptVoid("UCUpdateUpgradePanel");
}

// UnrealWingPanel.UpdateSelectWing(id)
event CallSelectWing(int WingID)
{
	ActionScriptVoid("UpdateSelectWing");
}

// preview actor: a copy of the main player that is shown in the "PawnShowLevel_Mall" render target (see MMOUIWingPanel.cpp)
event MMOPlayer ClonePlayer( MMOPlayer Base )
{
	local MMOPlayer P;
	P = Base.Spawn( class'MMOPlayer', GetPC() );
	if( P != none )
	{
		P.InitItems();
		P.CloneFrom( Base );
		P.SetPhysics( PHYS_None );
	}
	return P;
}

// ---- called by UnrealWing.swf (ExternalInterface.call) ----
native function ASClickWingList(int WingID);
native function AsClickWing(int WingID);
native function ASClickWingUpgrade();
native function ASHasNoWingTip();
native function ASRotateWingMesh(int Direction);
native function ASRotateWingMeshRelease();
native function ASSwitchBackShowType(int ShowType);
native function ASShowTipsByType(int TipsType, GFxObject mc);

DefaultProperties
{
	IsDynamicLoad=true
	DefaultVisible=false
	SelectWingID=-1
	BackShowType=2
}
