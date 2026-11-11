class MMOUIMessageBoxZone extends MMOUIBase
	native(UI)
	config(UIVisibleMask);

var GFxObject _ConfirmBox;

native function ShowUI(name inFriendlyName, optional bool inShow=true);
//{
//	GFXPlayer.ShowUI(inFriendlyName, inShow);
//}
native function bool OnEsc();

event GfxObject CallGetFirstChild()
{
	return ActionScriptObject("GetFirstChild");
}

event GfxObject CallClearAllMsgbox()
{
	return ActionScriptObject("ClearAllMsgbox");
}

// 1.05 MessageBox.swf: AddModalMsgbox(name, yesLabel, noLabel, twoButton)
function GfxObject ASAddModalMsgbox( string msgName , string yesLabel , string noLabel , bool twoButton )
{
	return ActionScriptObject("AddModalMsgbox");
}

event GfxObject CallAddModalMsgbox( string msgName , optional bool twoButton = true )
{
	return ASAddModalMsgbox( msgName , "OK" , "Cancel" , twoButton );
}

event GfxObject CallAddInputMsgbox( string msgName , optional bool twoButton = true)
{
	return ActionScriptObject("AddInputMsgbox");
}

event GfxObject CallAddModelessMsgbox( string msgName , optional bool twoButton = true )
{
	return ActionScriptObject("AddModelessMsgbox");
}

event GfxObject CallAddModalInputMsgbox( string msgName , optional bool twoButton = true )
{
	return ActionScriptObject("AddModalInputMsgbox");
}

event GfxObject CallAddMsgboxWithCheckBox( string msgName , optional bool twoButton = true )
{
	return ActionScriptObject("AddMsgboxWithCheckBox");
}

event GfxObject CallAddModelessExtrMsgbox( string msgName , optional bool twoButton = true )
{
	return ActionScriptObject("AddModelessExtrMsgbox");
}

event GfxObject CallAddBigMsgbox( string msgName , optional bool twoButton = true )
{
	return ActionScriptObject("AddBigMsgbox");
}


native function AutoAdjustSystemSettingOk(GFxObject mc);
native function AutoAdjustSystemSettingCancel(GFxObject mc);
native function ASUnLockMsgAck( bool Ok,GFXObject mc);
native function ASInputMsgAck( bool Ok , GFXObject mc );
native function ASCountDownMsgBoxAck( bool Ok, GFXObject mc );
native function ASCountDownMsgBoxTimeUp( GFXObject mc );
native function ASMessageBoxCheck(GFXObject mc,bool bCheck);

DefaultProperties
{
	//MAX_SacrificeSoul=5
	DefaultVisible=true
	IsVisible=true
}
