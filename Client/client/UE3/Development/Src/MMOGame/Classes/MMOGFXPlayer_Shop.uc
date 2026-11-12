class MMOGFXPlayer_Shop extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUINPCShop            _NPCShop;
var MMOUIShopMessage _shopMessagePanel;

function RegisterShop( MMOUINPCShop mc )
{
	_NPCShop = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Shop);
	//self.OnPostAdvance = _NPCShop.TickForTestInteractNPCDistance;
}

//moved
event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)
{
	// Mouse wheel scroll, and not inside the UI
	if ((key == "MouseScrollUp" || key == "MouseScrollDown") && (strHoverControllerName == ""))
		return false;
	else
		return true;
}

function ASRegisterShopMessage(MMOUIShopMessage mc)
{
	_shopMessagePanel = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_ShopMessage);
}
// event UnRegister()
// {
// 	super.UnRegister();
// 	RegisterUI(none, class'MMOUIManager'.default.Panel_Shop);
// 	MMOUI::RegisterMoviePlayer(class'MMOUIManager'.default.MoviePlayer_Shop);
// 
// 	class'MMOUIManager'.static.SetOpenShopDelegate(none);	
// 
// }

function ASShopMsgNumChange(int num)//moved
{
	_NPCShop.NumChange(num);
}

native function ASNPCShopRepair();//moved

native function ASNPCShopRepairAll();//moved
native function ASNPCShopClose();//moved

function NPCShopUsebleFilterToggle(bool bToggle )//moved
{
	_NPCShop.UsebleFilterToggle( bToggle );
}

native function ASShowTips_RepairAll( GFxObject butt );//moved


native function ASHideTips_RepairAll( GFxObject butt );//moved


function ASNPCShopPagePrev()//moved
{
	_NPCShop.PagePrev();
}

function ASNPCShopPageNext()//moved
{
	_NPCShop.PageNext();
}

// Purchase handling
//function ASShopMsgAck( bool agree , GFXObject mc )
//{
	//_NPCShop.ShopMsgAck( agree , mc );
//}

function ASShopMsgAck(bool agree,int count)//moved
{
	_NPCShop.ShopMsgAck( agree , count );
}

function ASNpcShopTab(int itemid)//moved
{
	_NPCShop.NpcShopTab(itemid);
}

DefaultProperties
{
	MovieInfoStr="UI_ShopWidget.ShopWidget"
	Priority=5
}
