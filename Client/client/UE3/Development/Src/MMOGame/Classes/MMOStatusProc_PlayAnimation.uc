//
// Open action
//
class MMOStatusProc_PlayAnimation extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() name Anim;
var() float Rate;
var() bool bLoop;

event OnExecute(MMOActor actor)
{
	//actor.PlayTestAnim(0);
	`log( "############################################PlayAnimation:"@Anim);
	actor.PlayAnimation(  Anim,  , Rate ,  bLoop );

}
native function Update(MMOActor actor);


defaultproperties
{
	Rate = 1.0f; 
	bLoop = false;
}