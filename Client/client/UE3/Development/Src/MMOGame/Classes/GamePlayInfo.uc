/*
 * common config data in the game
 * 
 * 
 */
class GamePlayInfo extends Object
	editinlinenew;

// animation set data
struct AnimSetInfo
{
	var() EPlayerClass Type;
	var() AnimSet Set;
};

var() array<AnimSetInfo> SetInfos;


// weapon stance suffix name
struct EWeaponPoseString
{
	//var() EWeaponPoseType PoseType;
	var() string str;
};

var() array<EWeaponPoseString> WeaponPoseStringInfos;







function AnimSet GetAnimSet(EPlayerClass e)
{
	local int i;
	for(i = 0; i < SetInfos.Length; ++i)
	{
		if(e == SetInfos[i].Type && SetInfos[i].Set != none)
		{
			return SetInfos[i].Set;
		}
	}
	return none;
}
