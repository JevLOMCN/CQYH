/*
 *	 Battlefield config
 * */
class MMOBattleTemplate extends Object
	hidecategories(Object)
	native;

//Battlefield type
enum EBattle_Mode
{
	EBATL_INVALID		<DisplayName="EBATL_TYPE_INVALID">,
	EBATL_5V5		    <DisplayName="EBATL_TYPE_5V5">,
	EBATL_10V10		    <DisplayName="EBATL_TYPE_10V10">,
	EBATL_15V15		    <DisplayName="EBATL_TYPE_15V15">,
	EBATL_10V10V10		<DisplayName="EBATL_TYPE_10V10V10">,
	EBATL_20V20		    <DisplayName="EBATL_TYPE_20V20">
};

enum EBattle_Style
{
	EBS_GetCoreItem            <DisplayName="Get Flag">,
	//EBS_HuntPlayer             <DisplayName="Kill Player">,
	//EBS_HuntNpc                <DisplayName="Kill NPC">,
	//EBS_Occupy                 <DisplayName="Get territory">,
	//EBS_Guard                  <DisplayName="Guard territory">,
	//EBS_ConvoyNpc              <DisplayName="Help NPC">,
	//EBS_KeepState              <DisplayName="Keep Buff">
};

struct native BattleItemReward
{
	//Item reward
	var() int                       ID                <DisplayName="Item ID">;
	//Item reward count
	var() int                       Num               <DisplayName="Item Number">;

	structdefaultproperties
	{
		ID=0
		Num=0
	}
};

struct native BattleReward
{
	//Item reward
	var() array<BattleItemReward>   Items                <DisplayName="Items Reward(battle score)">;
	////Experience reward
	//var() int                       Exp                  <DisplayName="Exp Reward">;
	////Victory battlefield points reward
	//var() int                       WinPoints            <DisplayName="Points Reward for Win">;
	////Defeat battlefield points reward
	//var() int                       FaildPoints          <DisplayName="Points Reward for Faild">;
	////Reputation reward
	//var() int                       FameID               <DisplayName="Fame of which(it's ID)">;
	////Reputation reward points
	//var() int                       FamePoints           <DisplayName="Fame Rewards">;

	structdefaultproperties
	{
		//Exp=0
		//WinPoints=0
		//FaildPoints=0
		//FameID=0
		//FamePoints=0
	}
};

struct native BattleScore
{
	var() int       ScoreToWin                   <DisplayName="Score To Win">;

	//var() int       GetCoreItem            <DisplayName="Get Flag Score">;
	//var() int       HuntPlayer             <DisplayName="Kill Player Score">;
	//var() int       HuntNpc                <DisplayName="Kill NPC score">;
	//var() int       Occupy                 <DisplayName="Get territory score">;
	//var() int       Guard                  <DisplayName="Guard territory score">;
	//var() int       ConvoyNpc              <DisplayName="Help NPC score">;
	//var() int       KeepState              <DisplayName="Keep Buff score">;

	//var() int       Damage1st                   <DisplayName="First damage score">;
	//var() int       Damage2nd                   <DisplayName="Second damage score">;
	//var() int       Damage3rd                   <DisplayName="Third damage score">;
	//var() int       Heal1st                     <DisplayName="First Heal score">;
	//var() int       Heal2nd                     <DisplayName="Second Heal score">;
	//var() int       Heal3rd                     <DisplayName="Third Heal score">;

	structdefaultproperties
	{
		ScoreToWin=0
		//GetCoreItem=0
		//HuntPlayer=0
		//HuntNpc=0
		//Occupy=0
		//Guard=0
		//ConvoyNpc=0
		//KeepState=0
		//Damage1st=0
		//Damage2nd=0
		//Damage3rd=0
		//Heal1st=0
		//Heal2nd=0
		//Heal3rd=0
	}
};
//Battlefield info
struct native BattleTemplate
{
	//Battlefield map name
	var() int              ID                     <DisplayName="Map ServerID">;
	//Battlefield name (for display)
	var() string           DisplayName            <DisplayName="Battle Name for display">;
	//Battlefield background description
	var() string           Desc	                  <DisplayName="Battle discription">;
	//Resource image ID
	var() int              IconIdx                <DisplayName="Icon Index in Flash">;
	//Battlefield respawn time
	var() int              ReviveTime             <DisplayName="Revive Time">;
	////Number of players to open
	//var() int              PlayerMin              <DisplayName="Minimal player">;
	////Level restriction
	//var() int              LevelLimit             <DisplayName="Level Limit">;
	//Battlefield time
	var() int              ContinueTime           <DisplayName="Continue Time">;
	////Respawn mode
	//var() int              RebownType             <DisplayName="Rebown Type">;
	////Battlefield battle mode
	//var EBattle_Mode     Mode                   <DisplayName="Battle Mode">;

	//Battlefield reward
	var() BattleReward     Reward                 <DisplayName="Battle Reward">;

	////Battlefield map name
	//var() int              MapServerID            <DisplayName="Map ServerID">;

	//Whether checked in random battlefield
	var bool                       IsSelected;
	
	//Battlefield victory condition description
	var() string           WinDesc  	                <DisplayName="Battle Win Desc">;
	//Battlefield scoring description
	var() string           ScoreDesc                    <DisplayName="Battle Score Desc">;
	////Battlefield scoring table
	var() BattleScore      Score                        <DisplayName="Battle Score">;
	//Main quest
	var() EBattle_Style    BattleStyle                  <DisplayName="Battle Style">;

	structdefaultproperties
	{
		ID=1
		DisplayName=""
		Desc=""
		IconIdx=0
		ReviveTime=30
		IsSelected=true
		WinDesc=""
		ScoreDesc=""
	}
};

var int RandomBattleMinLv                                     <DisplayName="Random Min Level">;
var int RandomBattleMinSlt                                    <DisplayName="Random Min Select">;
var int RandomBattleMaxSlt                                    <DisplayName="Random Max Select">;
var(RandomBattle) BattleTemplate  RandomBattle                              <DisplayName="Random Battle">;

var(AllBattles) array<BattleTemplate> BattleTemplateList                  <DisplayName="Battle List">;

cpptext
{
	int GetBattleTemplateCnt();
	void RevertSelected();
	FBattleTemplate *FindBattleTemplateBy( INT battleID );
	FBattleTemplate *GetBattleTemplateIn( INT idx );
	//
	INT GetBattleServerIdx( INT battleID );
}

defaultproperties
{
	RandomBattleMinLv=0
	RandomBattleMinSlt=0
	RandomBattleMaxSlt=200
}