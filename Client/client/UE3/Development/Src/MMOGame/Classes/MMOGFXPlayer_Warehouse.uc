class MMOGFXPlayer_Warehouse extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIWarehouse            Warehouse;

function RegisterWarehouse( MMOUIWarehouse mc )
{
	Warehouse = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Warehouse);

	//self.OnPostAdvance = Warehouse.TickForTestInteractNPCDistance;
}

// event UnRegister()
// {
// 	super.UnRegister();
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_Warehouse);
// 	MMOUI::RegisterMoviePlayer(class'MMOUIManager'.default.MoviePlayer_Warehouse);
// 
// 	//class'MMOUIManager'.static.SetOpenWarehouseDelegate(none);
// 	class'MMOUIManager'.static.SetUpdateWarehouseCapSlotDelegate(none);
// 
// }

native function ASSortWarehouse();//moved

event ClearItemSlot(GFxObject ItemSlot)//moved
{
	local int slotId;
	slotId = ItemSlot.GetInt("slotID");
	//Override this function; clearing slot attributes in bag uses ESPType_Bag
	SetItemSlotData( ItemSlot, none, ESPType_Warehouse , E_CONSLOT_BANK , slotId );
}

DefaultProperties
{
	MovieInfoStr="UI_WarehouseWidget.WarehouseWidget"
	Priority=5
}
