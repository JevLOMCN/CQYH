class MMOUIVehicleBook extends MMOUIBaseSlotContainer
	native(UI)
	config(UIVisibleMask);

// Mount book (1.05): list of all mounts, the mounted one, and the "beast power" collected from owned mounts.

native function int  GetSlotType();
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

native function RefreshVehicleBook();

// called from the swf (ExternalInterface)
native function ASClickMountSlot(int MountID);
native function ASSetInfoShowMode(int Mode);
native function ASMountVehicle(int MountID);
native function ASUnMountVehicle();
native function ASModelRotate(int Direction);
native function ASReleaseModelRotate();
native function ASShowMountGuide();
native function AsUseRideSkill();
native function ASRideSkillGetPath();

// calls into the swf
event CallSetLocalizationData(GFxObject data)
{
	ActionScriptVoid("UCSetLocalizationData");
}

event CallUpdateVehicleList(GFxObject data)
{
	ActionScriptVoid("UCUpdateVehicleList");
}

event CallSetVehicleInfoShowMode(int Mode)
{
	ActionScriptVoid("UCSetVehicleInfoShowMode");
}

event CallSetCurSelectedVehicle(int MountID)
{
	ActionScriptVoid("UCSetCurSelectedVehicle");
}

event CallSetRideSkillLearned(bool Learned)
{
	ActionScriptVoid("UCSetRideSkillLearned");
}

// copy of the main player for the 3D preview (same as MMOUIWingPanel)
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

event CallSetMountedVehicle(int MountID)
{
	ActionScriptVoid("UCSetMountedVehicle");
}

event CallUpdateMountPowerData(GFxObject data)
{
	ActionScriptVoid("UCUpdateMountPowerData");
}

event CallUpdateMountPowerAttrTotal(GFxObject data)
{
	ActionScriptVoid("UCUpdateMountPowerAttrTotal");
}

event CallUpdateMountSoul(GFxObject data)
{
	ActionScriptVoid("UCUpdateMountSoul");
}

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

event InitOnce()
{
	super.InitOnce();
}

DefaultProperties
{
	IsDynamicLoad=true
	DefaultVisible=false
}