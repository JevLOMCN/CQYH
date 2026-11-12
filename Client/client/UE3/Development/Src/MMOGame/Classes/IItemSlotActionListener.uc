//
// IItemSlotActionListener interface
//
interface IItemSlotActionListener
	dependson(TypeDefine)
	native(UI);

native function int  GetSlotType();
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);
native function bool OnDoubleClickSlot(GFxObject SrcSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, bool bDefault);

event OnSubscribeItemSlotAction();
event OnUnSubscribeItemSlotAction();