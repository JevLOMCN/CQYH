class MMOUIGuildWarehouse extends MMOUIBaseSlotContainer
	native(UI)
	config(UIVisibleMask);

var GFxObject _Tab;
var int _CurPage;
const MaxSlotCount = 117;

// Without this there will be very strange problems
var array<GFxObject> ItemSlotList;

event InitOnce()
{	
	local int i;
	local GFxObject obj;
	
	super.InitOnce();

	class'MMOUIManager'.static.SetFreshWarehouseDelegate(FreshWarehouse);
	class'MMOUIManager'.static.SetUpdateAccessInfoDelegate(UpdateAccessInfo);
	
	_CurPage = E_GUILD_STORAGE_TAG_1;

	for (i = 0; i < MaxSlotCount; i ++)
	{
		obj = GetObject("ItemSlot"$i);
		if (obj != none)
		{
			ItemSlotList.AddItem(obj);
		}
	}
}

event CallSetTabEnable(int num)
{
	ActionScriptVoid("SetTabEnable");
}

native function NativeInitOnce();
native function NativeInitLoad();

event CallSetAccessInfoText(string htmlText)
{
	ActionScriptVoid("SetAccessInfoText");
}

native function MoreInfo();
// Send refresh
native function SendFreshWarehouse();
// Refresh
native function FreshWarehouse();
native function TabChange(int tabId);
native function ClearUI();

native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

native function UpdateAccessInfo();

// Warehouse
native final function ASGuildWarehouseMore();
native final function ASGuildWarehouseUpdate();
native final function ASGuildWarehouseTab(int tabId);
native final function ASViewGuildLog(bool visible);

DefaultProperties
{
	IsDynamicLoad=true
}
