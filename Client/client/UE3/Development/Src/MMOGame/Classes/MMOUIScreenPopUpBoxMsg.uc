class MMOUIScreenPopUpBoxMsg extends MMOUIBase
	dependson(TypeDefine)
	native(UI);

//Play gained ability hint
native function PlayScreenPopUpBoxMsg( int msgId , EScreenMsgType type );
native function PlayScreenPopUpBoxMsgByString( EScreenMsgType type, String title, String content, String iconPath, String extraString );

//Gained ability hint playback finished notification
native function MsgPlayEnd();

//===============================================================================================================
//===============================================================================================================
native function ASScreenPopUpBoxMsgPlayEnd();
//===============================================================================================================
//===============================================================================================================
native function bool SkipCaptureInputKey(String key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);


DefaultProperties
{
	//FriendlyName=ScreenPopUpBoxMsg
}
