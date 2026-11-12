//
//Show a larger Hint on screen
//
class MMOStatusProc_ShowSceneHint extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);


/*
 *History museum, use case:
 * //Amen Ugly code
		//Make a UI notification action @todo
		AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
		AMMOHud* hud = Cast<AMMOHud>(ctrl->myHUD);
		if( msg->wSpellID == 7010 )
		{
			hud->HudMovie->eventShowMsgSceneHint( ctrl->StringMap->eventGetGamePlayStrByName( "DoorOfFateOpen" ) );
		}
		else if( msg->wSpellID == 7021  )
		{
			hud->HudMovie->eventShowMsgSceneHint( ctrl->StringMap->eventGetGamePlayStrByName( "TransducerOpen" ) );
		}
 * */
//Defined in tGamePlay in Localization
var() string StrId;

event OnExecute(MMOActor actor)
{
	//No longer used
	//local MMOPlayerController pc;
	//pc = actor.GetMainController();
	
	//`log( "############################################MMOStatusProc_ShowSceneHint:"@StrId);
	//MMOHud(pc.myHUD).HudMovie.ShowMsgSceneHint( ParseLocalizedPropertyPath( "MMOGame.GamePlay." $ StrId ) );
	//NativeOnExecute(actor);
}
//native function NativeOnExecute(MMOActor actor);
native function Update(MMOActor actor);


defaultproperties
{

}