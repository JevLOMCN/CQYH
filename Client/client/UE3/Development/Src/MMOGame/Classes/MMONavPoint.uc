
class MMONavPoint extends PathNode
	placeable
	native
	hidecategories(NavigationPoint,VehicleUsage,Display,Attachment,Collision,Physics,Advanced,Debug,MMO,Object,Mobile)
	ClassGroup(__Nav__);


var() init string ConfigName;

// add by zhangrui05@snda.com
enum ENavPointType
{
	ENPT_None   <DisplayName="ENPT_None">,
	ENPT_Fight	<DisplayName="ENPT_Fight">,
	ENPT_Script	<DisplayName="ENPT_Script">
};

enum ENavPointUIShowType
{
	ENPUIST_Hide   <DisplayName="ENPUIST_Hide">,
	ENPUIST_Type1	<DisplayName="ENPUIST_Type1">,
	ENPUIST_Type2	<DisplayName="ENPUIST_Type2">,
	ENPUIST_Type3	<DisplayName="ENPUIST_Type3">
};

var() ENavPointType NavPointType		<DisplayName="ENavPointType">;
var() NPCTemplate TargetNpc             <DisplayName="TargetNpc">;
var() int TargetNpcMapId                <DisplayName="TargetNpcMapId">;
var() init string ScriptName            <DisplayName="ScriptName">;
var() ENavPointUIShowType IsShowOnUI    <DisplayName="IsShowOnUI">;
var() string DisplayName                <DisplayName="DisplayName">;
var() Vector RedirectLoc                <DisplayName="RedirectLocation">;

cpptext
{
	#if WITH_EDITOR
	virtual void CheckForErrors()
	{
		// No need to check
	}
	#endif
}

DefaultProperties
{
}
