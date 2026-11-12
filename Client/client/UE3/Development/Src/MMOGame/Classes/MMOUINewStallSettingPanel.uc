class MMOUINewStallSettingPanel extends MMOUIBaseSlotContainer
	native(UI)
	config(UIVisibleMask);

native function ClearUI();
native function InitUI();

native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

// Add item to stall
native final function AddItem(int srcConId, int srcSlotId, int dstSlotId);

// Refresh Slot  list, delist, sold
native final function UpdateItemSlot(int stallSlot, int srcConId, int srcSlotId, int count, int price);

event UpdateStallSettingWidget(GFxObject obj)
{
	ActionScriptVoid("UpdateStallSettingWidget");
}

event FocusInput()
{
	ActionScriptVoid("UCSetFocus");
}

event UpdateMoneyUI(GFxObject obj)
{
	ActionScriptVoid("UCUpdateMoney");
}

event UpdateButtonStateUI(GFxObject obj)
{
	ActionScriptVoid("UCUpdateButtonState");
}

event UpdateStallNameAndAdd(GFxObject obj)
{
	ActionScriptVoid("UCUpdateNameAndAdd");
}

event UpdateUpgradeLeftTime(string timeString, bool over)
{
	ActionScriptVoid("UCSetUpdateTimeLeft");
}

event UpdateStallRecord(GFxObject obj)
{
	ActionScriptVoid("UCSetRecordListData");
}

event CheckSettingItem(int slot)
{
	ActionScriptVoid("UCCheckSettingItem");
}

event OnSubscribeItemSlotAction()
{
}

event OnUnSubscribeItemSlotAction()
{
}

native function RefreshButton();
native function RefreshMoney();
native function RefreshNameAndAdd();

native function ASOpenStall(string stallName);
native function ASCloseStall();
native function ASArrangeStall();
native function ASCloseStallPanel();
native function ASUpgradeStall();
native function ASViewStallRecord();

DefaultProperties
{
}