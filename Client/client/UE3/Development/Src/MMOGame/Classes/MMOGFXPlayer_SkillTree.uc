class MMOGFXPlayer_SkillTree extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUISkillTree _SkillTree;
var MMOUIHeavenGasExchangePanel _HeavenGasExchange;
var MMOUIHeavenGasPractisePanel _GasPractisePanel;

var const int SkillLevelUpLimitLv;

function RegisterSkillTree( MMOUISkillTree mc )
{
	_SkillTree = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_SkillTree);
}

function RegisterHeavenGasExchange( MMOUIHeavenGasExchangePanel mc )
{
	_HeavenGasExchange = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_HeavenGasExchang);
}

function RegisterHeavenMain( MMOUIHeavenPanel mc )
{
	_SkillTree._MainHeaven = mc;
}
function RegisterHeavenGasMain(MMOUIHeavenGasMainPanel mc)
{
	_SkillTree._GasPractiseMain = mc;
}
function RegisterHeavenBlood(MMOUIHeavenBloodPanel mc)
{
	_SkillTree._BloodPanel = mc;
}
function RegisterHeavenGasPractise(MMOUIHeavenGasPractisePanel mc)
{
	_GasPractisePanel = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_HeavenPractise);
}

native function PostAdvance(float DeltaTime);
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
final native function ASClickConverter();
final native function ASClickBtnPractiseGas(int nBtnIndex);
final native function ASClickOneKeyPractise();
final native function ASClickOneKeyMergeGas();
final native function ASClickBtnLevelState(int nLevelIndex,int nPanelIndex);
final native function ASActivateBlood(int nType);
final native function ASOneKeyGoBlood(int nType);
final native function ASClickTab(int nIndex);
final native function ASClickExchangeGasItem(int nGasID);
final native function ASClickOpenExchangePanel();
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
//
DefaultProperties
{
	MovieInfoStr="UI_Heaven.Heaven"
	Priority=5
	SkillLevelUpLimitLv=10;
}
