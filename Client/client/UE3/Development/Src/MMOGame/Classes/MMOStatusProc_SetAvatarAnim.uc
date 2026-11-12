//
//Set animation set
//
class MMOStatusProc_SetAvatarAnim extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	dependson(AnimSet)
	hidecategories(Object);

var() array<AnimSet> ani;

event OnExecute(MMOActor actor)
{
	//`log( "############################################MMOStatusProc_SetAvatarAnim:" @ ani.Length );
	if( ani.Length>0 && ani[0] != none  )
	{
		//`log( "############################################MMOStatusProc_SetAvatarAnim: actor.SetAnimSet(ani) " );
		actor.SetAnimSet(ani);
	}
	else
	{
		//`log( "############################################MMOStatusProc_SetAvatarAnim: actor.RefreshAnimSet(true); " );
		actor.RefreshAnimSet();
	}
}
native function Update(MMOActor actor);


defaultproperties
{

}