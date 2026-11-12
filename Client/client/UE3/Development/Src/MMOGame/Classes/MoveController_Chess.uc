/*
 * 
 * special controller for board-game NPCs,
 * 
 */
class MoveController_Chess extends MoveController
	native
	hidecategories(Object,Physics,Collision,MMO,Mobile,Advanced,Attachment,Display,Movement,Debug);

	
	//interface
	native function OnLeftMouseKey(Actor srcActor,bool bRelease,Vector StartTrace,Vector EndTrace);


DefaultProperties
{

}
