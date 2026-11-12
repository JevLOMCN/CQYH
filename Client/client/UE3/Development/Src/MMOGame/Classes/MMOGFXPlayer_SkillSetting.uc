class MMOGFXPlayer_SkillSetting extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUISkillSettingPanel _SkillSetting;
var int SkillLevelUpLimitLv;
function RegisterSkillSetting( MMOUISkillSettingPanel mc )
{
	_SkillSetting = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_SkillSetting);
}

//native function PostAdvance(float DeltaTime);
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

final native function ASClickTab(int nIndex);


//
DefaultProperties
{
	SkillLevelUpLimitLv=10
	//MovieInfoStr is right, it is called SkillTree for historical reasons
	MovieInfoStr="UI_SkillTree.SkillTree"
	Priority=5
}
