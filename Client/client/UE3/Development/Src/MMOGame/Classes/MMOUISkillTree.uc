class MMOUISkillTree extends MMOUIBaseSlotContainer
	dependson(UIKeyMapManager)
	native(UI)
	config(UIVisibleMask);

enum eSkillTreePaneType
{
	ESkillPnl_HeavenMain,
	ESkillPnl_Active,
	ESkillPnl_SpecialSkills,
	ESkillPnl_GasMain,
	ESkillPnl_HeavenBlood
};
var const int SkillLevelUpLimitLv;

var eSkillTreePaneType CurPanel;
var UIKeyMapManager KeyMapManager;

var const int MAX_SKILL_TREE_PANEL;//5
var GFxObject Panel[5];

//active skills
var bool IsShowAllActiveSkills;

var bool IsAutoCheckSkillStyle;

var GFxObject ActiveSkillsListPanel;
var GFxObject ActiveSkillsDetailListPanel;
var int ActiveSkillsDetaiListPanelWidth;
var int ActiveSkillsDetaiListPanelHeight;
var GFxObject ActiveDetailSkillListPanelArrowMc;
var int ListArrowMcWidth;
var int ListArrowMcHeight;
var GFxObject CurActiveDetalSkillMainSkillMc;

var GFxObject ShowSkillSettingBtn;
var GFxObject ShowLearnedActiveSkillsChkBtn;
var GFxObject ShowAllActiveSkillsChkBtn;

var const int ActiveSkillsListItemHalfHeight;
var const int ActiveSkillsListItemWidth;

var const int ListArrowOffsetX;
var const int ListArrowOffsetY;
//-----------------------------------------------------------------------------------------
//Realm related
var MMOUIHeavenPanel  _MainHeaven;
var MMOUIHeavenGasMainPanel _GasPractiseMain;
var MMOUIHeavenBloodPanel _BloodPanel;
//--
native function PostOnShow(bool v);
final native function UpdatePanel( int panelIdx );
final native function HidePanel();

event ShowPanelOf( int tabIdx )
{
	ActionScriptVoid( "UCShowContent" );
}

event SetTabVisible(bool skillPanel,bool heavenMain,bool gasMain,bool bloodPanel)
{
	ActionScriptVoid( "UCSetSkillTabVisibleState" );
}
//----------------------------------------------------------------------------------------
//active skills
final native function UpdateActiveSkillPanel();
final native function UpdateActiveSkillList();

final native function ShowActiveSkillDetailOf( GFxObject mc, int skillUniqueID, int showPosX, int showPosY );
final native function UpdateActiveSkillDetail( int skillUniqueID );
final native function HideActiveSkillDetail();

final native function ShowLearnedActiveSkills();
final native function ShowAllActiveSkills();

//
event SetActiveSkillList( GFxObject Provider )
{
	ActionScriptVoid( "UCSetActiveSkillList" );
}
//
event SetDetailActiveSkillList( GFxObject Provider )
{
	ActionScriptVoid( "UCSetDetailActiveSkillList" );
}

//----------------------------------------------------------------------------------------
//special skills
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject srcSlotMC, GFxObject destSlotMC, ESlotPanelType srcSlotType, ECONTAINER_SLOT srcContainerID, int SrcSlotID, ESlotPanelType destSlotType, ECONTAINER_SLOT destContainerID, int destSlotID);
//
final native function UpdateSpecialSkillPanel();
final native function UpdateSpecialSkillList();
//
native function BagSlotChange( INT BagId,INT SlotId );

event SetSpecialSkillList( GFxObject Provider )
{
	ActionScriptVoid( "UCSetSpecialSkillList" );
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
final native function UpdateHeavenMainPanel();
final native function UpdateHeavenGasMainPanel();
final native function UpdateHeavenBloodPanel();
final native function UpdateHeavenExchangePanel();
final native function UpdateHeavenLog();
native function PlayerMoneyChange(EMONEY_TYPE MoneyType, int DeltaMoney);

event CallSetXiuLianValue(int data)
{
	ActionScriptVoid( "UCSetXiuLianValue");
}
event CallSetWindowTitle(string title)
{
	ActionScriptVoid("SetWindowTitle");
}
event CallSetSkillTabState(int nIndex)
{
	ActionScriptVoid("UCSetSkillTabState");
}

//===============================================================================================================
//===============================================================================================================
native function ASStartDrag(GFxObject SrcItemSlot, ESlotPanelType SrcSlotType, ECONTAINER_SLOT SrcContainerID, int SrcSlotID);

final native function ASLevelUpSkill( int uniqueSkillID );
final native function ASShowTips_SkillLvUpBtn( GFxObject mc, int uniqueSkillID );
final native function ASHideTips_SkillLvUpBtn();

final native function ASShowTips_SkillTreeActiveSkill( GFXObject mc );
final native function ASHideTips_SkillTreeActiveSkill();

final native function ASShowTips_SkillTreeRuneSkill( GFXObject mc );
final native function ASHideTips_SkillTreeRuneSkill();

final native function ASShowSkillDetail( GFxObject mc, int skillUniqueID, int showPosX, int showPosY );
final native function ASOnScroolingActiveSkillList();

final native function ASShowLearnedActiveSkills();
final native function ASShowAllActiveSkills();
final native function ASShowSkillSetting();

final native function ASHideActiveDetailSkillsListPanel();

final native function ASSetSkill( INT skillSetIdx, INT uniqueSkillID );

//Realm related
function RegisterHeavenMain( MMOUIHeavenPanel mc )
{
	_MainHeaven = mc;
}
function RegisterHeavenGasMain(MMOUIHeavenGasMainPanel mc)
{
	_GasPractiseMain = mc;
}
function RegisterHeavenBlood(MMOUIHeavenBloodPanel mc)
{
	_BloodPanel = mc;
}
final native function ASClickConverter();
final native function ASClickOneKeyMergeGas();
final native function ASClickBtnLevelState(int nLevelIndex,int nPanelIndex);
final native function ASActivateBlood(int nType);
final native function ASOneKeyGoBlood(int nType);
final native function ASClickTab(int nIndex);
final native function ASClickOpenGasPractisePanel();
final native function ASClickOpenHeavenLog();
final native function ASShowTips_HeavenLevel(GFxObject mc, int nLevel);
final native function ASHideTips_HeavenLevel();
final native function ASUpdateNeedTime(int nTime);
final native function ASShowTips_HeavenBloodSlot(GFxObject mc,int nSlotIndex, int nPanelIndex);
final native function ASHideTips_HeavenBloodSlot();
final native function ASShowTips_HeavenExpBar(GFxObject mc);
final native function ASHideTips_HeavenExpBar();
final native function ASShowTips_HeavenGasSlot(GFxObject mc);
final native function ASHideTips_HeavenGasSlot();
final native function ASShowTips_HeavenGasExchangeSlot(GFxObject mc,int nGasID);
final native function ASHideTips_HeavenExchangeGasSlot();
final native function ASShowTips_HeavenHintCtrl(GFxObject mc,int nIdex);
final native function ASHideTips_HeavenHintCtrl();
final native function ASShowTips_HeavenGasPower(GFxObject mc);
final native function ASHideTips_HeavenGasPower();
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{
	SkillLevelUpLimitLv=10;
	CurPanel=ESkillPnl_HeavenMain
	MAX_SKILL_TREE_PANEL=5
	ActiveSkillsListItemWidth=184
	ActiveSkillsListItemHalfHeight=31
	ListArrowOffsetX=15
	ListArrowOffsetY=34
}
