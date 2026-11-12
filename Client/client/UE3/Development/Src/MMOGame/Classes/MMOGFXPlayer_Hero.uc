class MMOGFXPlayer_Hero extends MMOGFXPlayer
	native(UI)
	config(MMOGame);


var MMOUIHero            Hero;
var TurnButtonStats TurnButton;

function RegisterHero( MMOUIHero mc )
{        
	Hero = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Hero);
}

native function Test() ;


// event UnRegister()
// {
// 	super.UnRegister();
// 
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_Hero);
// 	MMOUI::RegisterMoviePlayer(class'MMOUIManager'.default.MoviePlayer_Hero);
// 	class'MMOUIManager'.static.SetRefreshHeroAttributeDelegate(none);
// 	class'MMOUIManager'.static.SetRefreshHeroSlotDelegate(none);
// 	class'MMOUIManager'.static.SetRefreshHeroDelegate(none);
// 	class'MMOUIManager'.static.SetRefreshSummonDelegate(none);
// }

native function ShowHero();
native function PostAdvance(float DeltaTime );

native function ASHeroUncall();
native function ASHeroSummon();
native function ASHeroFire();
native function ASHeroBag();
native function ASHeroRename();

native function OnHeroFireOk(GFxObject mc);

function RefreshHero(bool bForceUpdate)
{
	Hero.RefreshHero(bForceUpdate);
}

function RefreshAttribute()
{
	Hero.RefreshAttribute();
	Hero.RefreshInfo();
}

function RefreshSlot()
{
	Hero.RefreshSlot();
	Hero.ModelEquip();
}

function RefreshSummon()
{
	Hero.RefreshSummon();
}

function ButtonDrag( int deltaX )
{
	local rotator newRot;
	newRot = ShowAvatar.Rotation;
	newRot.Yaw += -deltaX*100; //3030 is the coefficient between mouse movement pixels and angle value
	ShowAvatar.SetPhysics(PHYS_None);
	ShowAvatar.SetRotation(newRot);
}

function TurnRelease()
{
	TurnButton = TBS_None;

}

function TurnPress( bool bLeft)
{
	if(ShowAvatar != none)
		ShowAvatar.SetPhysics(PHYS_None);
	if( bLeft)
		TurnButton = TBS_Left;
	else
		TurnButton = TBS_Right;
}


DefaultProperties
{
	MovieInfoStr="UI_HeroWidget.HeroWidget"
	TurnButton=TBS_None
}
