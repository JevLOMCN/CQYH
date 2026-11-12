/*
 * supports animation sequences with position
 * only supports a single displacement in an animation sequence
 * 
 * 
 * */
class AbilityTimeFactorComponent extends AbilityComponent
	editinlinenew
	hidecategories(Object)
	native;

enum ETimeFactorType
{
	ETFT_None,
	ETFT_SpeedUp,
	ETFT_SpeedDown,
	ETFT_PauseAnim,
};

var(TimeFactor) ETimeFactorType Type;
var(TimeFactor) float           FactorValue;

cpptext
	{
		virtual struct BaseComponent* CreateCplusplusNode();
	}

DefaultProperties
{
	Type=ETFT_SpeedDown
	FactorValue=0.3
	bHaveTrail=true
	LineColor=(R=0.13333,G=0.6941,B=0.2980,A=1.0)
	TitleString="TimeFactor"
	Duration=0.15
}
