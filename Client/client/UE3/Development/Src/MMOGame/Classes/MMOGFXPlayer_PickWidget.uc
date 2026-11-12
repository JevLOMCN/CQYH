/********************************************************************
	Created:	2013/06
	Filename: 	UMMOGFXPlayer_PickWidget
	file ext:	uc
	Author:		hemu@snda
	Purpose:	Drop pickup GFXPlayer-PickWidget
*********************************************************************/

class MMOGFXPlayer_PickWidget extends MMOGFXPlayer
	dependson(MMOUIBase, TypeDefine)
	native(UI)
	config(MMOGame);


var MMOUIDropItemTips	            _DropItemTips;

function registerDropItemTips( MMOUIDropItemTips mc)
{
	_DropItemTips = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_PickWidget_DorpItemTips);
}

native function OnPostRender( Canvas cvs, float fDelta);
native function PostAdvance(float DeltaTime);
native function OnHUDLoaded();

native final function Pickup( int ItemID );
// {
// 	_DropItemTips.TipsPickItem(ItemID);
// }

DefaultProperties
{
	MovieInfoStr="UI_PickWidget.PickWidget"
	bCloseOnLevelChange=FALSE
	bAllowFocus=FALSE
	Priority=1
}
