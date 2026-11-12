class MMOUIActivityViewPanel extends MMOUIBase
	native(UI)
	config(UIVisibleMask);


var int  nCurSortType;
var int  nSortAdd;
var int  nFilterType;
var int  nFilterReward;

var array<GFxObject> rewardList;

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}
event InitActivityViewReward()
{
	local int i;
	local GFxObject ItemSlot;
	local GFxObject FBView_mc;
	FBView_mc = GetObject("activity_info_movie");
	if(FBView_mc != none)
	{
		for( i =0 ;i< 8 ;++i )
		{
			ItemSlot = FBView_mc.GetObject("activityRewardItem"$i$"");
			if(ItemSlot != none)
			{
				ItemSlot.SetFloat("itemid",i); 
				rewardList.AddItem( ItemSlot );
			}
		}
	}
}
// UC->AS
event CallSetWindowTitle(string title)
{
	ActionScriptVoid("SetWindowTitle");
}

event CallUpdateActivity(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateActivityListData");
}

event CallUpdateActivityTeamList(GFxObject data)
{
	ActionScriptVoid("UCUpdateActivityTeamListData");
}

event CallUpdateActivityTeamButtonState(bool bTeam)
{
	ActionScriptVoid("UCUpdateButtonState");
}

event CallUpdateActivityTeamPageInfo(int totalPage, int curPage)
{
	ActionScriptVoid("UCUpdateActivityTeamPageText");
}

event CallUpdateActivityDetailInfo(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateActivityDetailInfo");
}

event CallUpdateActivityRewardBarInfo(GFxObject dataProvider)
{
	ActionScriptVoid("UCUpdateRewardBarInfo");
}

event CallUCUpdateHuoyueValue(int nCurValue, int nMaxValue)
{
	ActionScriptVoid("UCUpdateHuoyueValue");
}

event CallShowActivityTeamTips(bool bShow)
{
	ActionScriptVoid("UCShowActivityTeamTips");
}

event CallUpdateActivityTeamTips(GFxObject data)
{
	ActionScriptVoid("UCUpdateActivityTeamTips");
}

event CallShowRewardBar(bool bShow)
{
	ActionScriptVoid("UCShowRewardBar");
}

native  function ASSortActivity(int type);
native  function ASShowActivityFilterType(int nType);
native  function ASShowActivityFilterRewardType(int nType);

native function ASClickGetReward();
native function ASClickEnterTeamHome();
native function ASClickActivityList(int nActivityID);
native function ASClickActivityFBList(int nFBID);
native function ASClickFastJoinTeam(int nID);
native function ASClickRandJoinTeam(int nID);

native function  ASShowTips_ActivityRewardItem(GFxObject mc);
native function  ASHideTips_ActivityRewardItem();

native function  ASShowTips_ActivityRewardItemBar(GFxObject mc,int nIndex);
native function  ASHideTips_ActivityRewardItemBar();

//Dungeon lobby related
native function ASShowActivityFBView();
native function ASShowActivityTeamView();
native function ASClickActivityTeamList(int nTeamId);
native function ASRollOverActivityTeamList(int nTeamId);
native function ASRollOutActivityTeamList(int nTeamId);

native function ASClickTeamManager();
native function ASClickCreateTeam();
native function ASClickEnterTeam();
native function ASClickQuickEnterLimitted();
native function ASClickQuickEnter();
native function ASClickInvite();
native function ASClickChangeTarget();
native function ASClickLeaveTeam();
native function ASPrevTeamPageClick();
native function ASNextTeamPageClick();

DefaultProperties
{	
	IsDynamicLoad=true
	DefaultVisible = false
	nCurSortType = -1;
	nSortAdd = 0;
	nFilterType = 0;
	nFilterReward = 0;
}
