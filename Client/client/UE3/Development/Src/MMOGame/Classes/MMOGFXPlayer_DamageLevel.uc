class MMOGFXPlayer_DamageLevel extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIDamageNumberLevel            DamageLevel;

function RegisterDamageLevel( MMOUIDamageNumberLevel mc )
{
	
	DamageLevel = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_DamageLevel );
}

native function PostAdvance(float DeltaTime );
//native function ASSetPos3DTo2D(GFxObject mc, float PosX, float PosY, float PosZ );
//native function ASSetTweenData(GFxObject DataObj, string TweenName );

DefaultProperties
{
	MovieInfoStr="UI_DamageLevel.DamageLevel"
	IsLockedDynamicLoad=true
	bCloseOnLevelChange=FALSE
	Priority=2 // Keep this at the bottom to receive keyboard and mouse events
	bAllowInput=false
	bAllowFocus=false
}
