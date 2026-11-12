class MMOGFXPlayer_MsgBall extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIMsgButtonBar       _MsgButtonBar;


function RegisterMsgButtonBar( MMOUIMsgButtonBar mc )
{
	_MsgButtonBar = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_MsgBall_MsgButtonBar );
}




//native function OnDragDrop(GFxObject SrcSlotMC, GFxObject DestSlotMC, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID, ESlotPanelType DestSlotType, ECONTAINER_SLOT DestContainerID, int DestSlotID);

native function ASMsgButtonClick( int idx);//moved

native function ASMsgListClick( int idx , int type );//moved



native function ASOnPanelClose( string PanelName );//moved


DefaultProperties
{
	//bAllowFocus=false
	MovieInfoStr="UI_MsgBall.MsgBall"
	bCloseOnLevelChange=FALSE
	Priority=6 // Keep this at the bottom to receive keyboard and mouse events
}
