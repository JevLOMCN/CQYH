class AbilityHitEffectComponent extends AbilityComponent
	editinlinenew
	native;


enum HitPositionType
{
	HPT_Center,
	HPT_Up,
	HPT_Down,
};

enum HitDirectionType
{
	HDT_VERTICAL,
	HDT_HIGH,
	HDT_MIDDLE,
	HDT_LOW,
	HDT_HORIZONTAL,
};

enum HitPowerLevel
{
	HPL_STRONG,
	HPL_MEDIUM,
	HPL_WEAK,
};

// direction is Creator -> Target
//var() float XYMomentum;
//var() float ZMomentum;
//var() float HitValue;
var() HitDirectionType DirectionType;
var() HitPowerLevel PowerLevel;
var() HitPositionType PositionType;

cpptext
{
	virtual struct BaseComponent* CreateCplusplusNode();
}

event Vector GetMomentum(Actor BaseActor, Actor Target)
{
	local Vector Dir, Momentum;
	local float power;
	local float h;
	local float v;

	Dir = BaseActor.Location - Target.Location;
	Dir.Z = 0;

	if (Dir.X * Dir.X + Dir.Y * Dir.Y > 0.00001f)
	{
		power = GetPower();
		h = GetHComponent();
		v = GetVComponent();

		Dir = Normal(Dir);

		Dir.X *= h;
		Dir.Y *= h;
		Dir.Z = v;

		Momentum = Dir * power;
	}
	else
	{
		Momentum.X = 0;
		Momentum.Y = 0;
		Momentum.Z = 0;
	}

	return Momentum;
}

function float GetPower()
{
	switch (PowerLevel)
	{
	case HPL_STRONG:
		return 1500;
	case HPL_MEDIUM:
		return 1000;
	case HPL_WEAK:
		return 600;
	default:
		return 1000;
	}
}

function float GetVComponent()
{
	switch (DirectionType)
	{
	case HDT_VERTICAL:
		return 1;
	case HDT_HIGH:
		return 0.866f;
	case HDT_MIDDLE:
		return 0.707f;
	case HDT_LOW:
		return 0.5f;
	case HDT_HORIZONTAL:
		return 0;
	}
}

function float GetHComponent()
{
	switch (DirectionType)
	{
	case HDT_VERTICAL:
		return 0;
	case HDT_HIGH:
		return 0.5f;
	case HDT_MIDDLE:
		return 0.707f;
	case HDT_LOW:
		return 0.866f;
	case HDT_HORIZONTAL:
		return 1;
	}
}


DefaultProperties
{
	DirectionType = HDT_MIDDLE;
	PowerLevel = HPL_MEDIUM;
}
