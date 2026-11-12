
class MMOUIMasterStudentPanel extends MMOUIBase
	dependson(UIKeyMapManager)
	native(UI)
	config(UIVisibleMask);

enum MasterPanelType
{
	EMSTPT_Outline,
	EMSTPT_MasterPoster,
	EMSTPT_MasterApply,
	EMSTPT_StudentPoster,
	EMSTPT_StudentApply,
};

var GFxObject _ConfirmBox;
var int _IncompleteInfoCnt;
var MasterPanelType _CurPanelType;

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
	pl.AddPostAdvanceDelegate(PostAdvance);
}

native function PostAdvance(float DeltaTime);
native function PostOnShow(bool v);

final native function PopUpMessageBoxToConfirmForceGraduate();
final native function OnConfirmForceGraduate( GFxObject mc );
final native function OnCancelForceGraduate( GFxObject mc );

native function bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

/*
	Data refresh functions
*/
event CallUpdateStateChange(int nMasterState,int nSelfState)
{
	ActionScriptVoid("UCUpdateStateChange");
}
event CallUpdateMasterExp(int state)
{
	ActionScriptVoid("UCUpdateMasterExp");
}
event CallUpdateStudentNum(int nNum,int nMax)
{
	ActionScriptVoid("UCUpdateStudentNum");
}
event CallUpdateMasterApplyListData(GFxObject Provider)
{
	ActionScriptVoid("UCUpdateMasterApplyListData");
}
event CallUpdateMasterPosterListData(GFxObject Provider)
{
	ActionScriptVoid("UCUpdateMasterPosterListData");
}
event CallUpdateStudentPosterListData(GFxObject Provider)
{
	ActionScriptVoid("UCUpdateStudentPosterListData");
}
event CallUpdateStudentApplyListData(GFxObject Provider)
{
	ActionScriptVoid("UCUpdateStudentApplyListData");
}
event CallUpdateMasterInfoListData(GFxObject Provider)
{
	ActionScriptVoid("UCUpdateMasterInfoListData");
}
event CallUCUpdateRecruitState(int nState)
{
	ActionScriptVoid("UCUpdateRecruitState");
}
event CallSetWindowTitle(string title)
{
	ActionScriptVoid("SetWindowTitle");
}
event CallUCClickMasterTap(int nTab)
{
	ActionScriptVoid("UCClickMasterTap");
}
event CallUCUpdateMasterApplyListDataOnSelected(int nIdex,int id)
{
	ActionScriptVoid("UCUpdateMasterApplyListDataOnSelected");
}
event CallUCUpdateStudentApplyListDataOnSelected(int nIdex,int id)
{
	ActionScriptVoid("UCUpdateStudentApplyListDataOnSelected");
}
event CallUCsetMasterInfoCheckBox(bool check,bool show)
{
	ActionScriptVoid("UCsetMasterInfoCheckBox");
}


//---------------moved-----------------------
native function ASClearMasterApplyList();
native function ASAcceptMasterApplyList(int nID);
native function ASDenyMasterApplyList(int nID);
native function ASMasterApplyListNextPage();
native function ASMasterApplyListPrevPage();
native function ASSortMasterApplyList(int nType);
native function ASSortMasterPosterList(int nType);
native function ASSortStudentPosterList(int nType);
native function ASSortStudentApplyList(int nType);
native function ASMasterPosterListRefresh();
native function ASApplyMasterPosterList(int nID);
native function ASCancelMasterPosterList(int nID);
native function ASMasterPosterListNextPage();
native function ASMasterPosterListPrevPage();
native function ASClickMasterPosterList(int nID);
native function ASLeaveMaster();
native function ASFireStudent(int nID);
native function ASClickMasterInfoList(int nID);
native function ASRightMenuMasterApplyList(int nID);
native function ASRightMenuStudentPosterList(int nID);
native function ASApplyStudentPosterList(int nID);
native function ASCancelStudentPosterList(int nID);
native function ASRefreshStudentPosterList();
native function ASClickStudentPosterList(int nID);
native function ASStudentPostListPage(int nPage);
native function ASAcceptStudentApplyList(int nID);
native function ASDenyStudentApplyList(int nID);
native function ASClearStudentApplyList();
native function ASClickStudentApplyList(int nID);
native function ASRightMenuStudentApplyList(int nID);
native function ASClickStartRecruit();
native function ASClickStopRecruit();
native function ASClickAcceptLeave(int nID);
native function ASClickCongratulation(int nID);
native function ASClickMasterTap(int nID);

native function ASApplyGrad();
native function ASForceGrad();

native function OnLeaveMaster();
native function ASRightMenuStudentDetailList(int nID);
native function ASClickCongraluate(int nID);
native function ASRightMenuMasterList(int nID);
native function OnFireStudent(GFxObject mc);
native function OnMasterCancel();

native function ASShowTips_MasterGift(GFxObject mc,int nType);
native function ASHideTips_MasterGift();

native function ASMasterInfoClick(bool selected);
native function ASClickMasterExp(int id,bool bSelfReward);
DefaultProperties
{
	_CurPanelType=EMSTPT_Outline
	IsDynamicLoad=true
}
