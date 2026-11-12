class MMOUISkillBar extends MMOUIBaseSlotContainer implements(IMMOActorHUD)
	native(UI)
	dependson(UIKeyMapManager)
	dependson(TypeDefine)
	;

enum SkillBarSlotType
{
	ESBarSlotTp_Invalid,
	ESBarSlotTp_Item,
	ESBarSlotTp_Skill,
	ESBarSlotTp_Mount
};

enum ESkillBarSlotUpdateType
{
	ESBarSlotUpdTp_Mana,
	ESBarSlotUpdTp_WeaponType,
	ESBarSlotUpdTp_Buff,
	ESBarSlotUpdTp_NoData
};

struct native SlotContainer
{
	var int ID;
	var int AidID;
	var int State;
	var GFxObject SlotObject;
	var SkillBarSlotType Type;
};

var GFxObject ExpBar;
var GFxObject MpBar;
var GFxObject HpBar;
var GFxObject ActionBarFx;

var GFxObject PlayerLevelTf;
var GFxObject PlayerPKBtn;
var GFxObject PlayerNameTf;
var GFxObject PlayerProMc;
const SkillBarSlotMax = 12;
var SlotContainer SkillBarSlot[SkillBarSlotMax];
var int IsBarSlotDataNeedUpload;

var UIKeyMapManager KeyMap;
var int SKILL_BAR_ONLY_ITEM_CONTAINER;
var bool IsBarTypeUpdated;

var int LastHP;
var int LastMP;
//Double experience
var int Exp_Extra;

var int SkillSlotUpdState;
var bool NeedRefreshSkillSlotDisable;

var GFxObject NewSkillHintBtn;

var bool IsSkillSettingBtnShow;
//
var int CurShowedSkillListSlotIdx;

var bool IsCurSkillDragHint;
var bool IsCurItemDragHint;

var MaterialInstanceConstant TheHealthHUDMatTemplateBlood;
var MaterialInstanceConstant TheHealthHUDMatTemplateWarrior;
var MaterialInstanceConstant TheHealthHUDMatTemplateMage;
var MaterialInstanceConstant TheHealthHUDMatTemplateRogue;
var MaterialInstanceConstant TheHealthHUDMatTemplateArcher;


var MaterialInstanceConstant TheHealthHUDMatTemplateBlood_Half;

var MaterialInstanceConstant TheHealthHUDMatTemplateWarrior_Half;
var MaterialInstanceConstant TheHealthHUDMatTemplateMage_Half;
var MaterialInstanceConstant TheHealthHUDMatTemplateRogue_Half;
var MaterialInstanceConstant TheHealthHUDMatTemplateArcher_Half;

var Texture2D GlobeGlassTexture;
var Texture2D GlobeFrameTexture;
var Texture2D GlobeFrameTextureFull;

var transient MaterialInstanceConstant MaterialGlobe[5];

var transient MaterialInstanceConstant MaterialGlobeHalf_Blood;
var transient MaterialInstanceConstant MaterialGlobeHalf_Warrior;
var transient MaterialInstanceConstant MaterialGlobeHalf_Mage;
var transient MaterialInstanceConstant MaterialGlobeHalf_Rogue;
var transient MaterialInstanceConstant MaterialGlobeHalf_Archer;

native function ClearUI();

event InitSlotMc()
{
	local int i;

	for( i = 0 ; i < SkillBarSlotMax ; i++ ){
		SkillBarSlot[i].SlotObject = GetObject( "slot_"$i );
	}
}

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);

	KeyMap = MMOGame(GetPC().WorldInfo.Game).KeyMapManager;

	class'MMOUIManager'.static.SetBuffEnableAbilityDelegate(BuffEnableAbility);

	MaterialGlobe[0] = new(self) class'MaterialInstanceConstant';
	MaterialGlobe[0].SetParent(TheHealthHUDMatTemplateBlood);

	MaterialGlobe[1] = new(self) class'MaterialInstanceConstant';
	MaterialGlobe[1].SetParent(TheHealthHUDMatTemplateWarrior);

	MaterialGlobe[2] = new(self) class'MaterialInstanceConstant';
	MaterialGlobe[2].SetParent(TheHealthHUDMatTemplateMage);

	MaterialGlobe[3] = new(self) class'MaterialInstanceConstant';
	MaterialGlobe[3].SetParent(TheHealthHUDMatTemplateRogue);

	MaterialGlobe[4] = new(self) class'MaterialInstanceConstant';
	MaterialGlobe[4].SetParent(TheHealthHUDMatTemplateArcher);



	MaterialGlobeHalf_Blood = new(self) class'MaterialInstanceConstant';
	MaterialGlobeHalf_Blood.SetParent(TheHealthHUDMatTemplateBlood_Half);

	MaterialGlobeHalf_Warrior = new(self) class'MaterialInstanceConstant';
	MaterialGlobeHalf_Warrior.SetParent(TheHealthHUDMatTemplateWarrior_Half);

	MaterialGlobeHalf_Mage = new(self) class'MaterialInstanceConstant';
	MaterialGlobeHalf_Mage.SetParent(TheHealthHUDMatTemplateMage_Half);

	MaterialGlobeHalf_Rogue = new(self) class'MaterialInstanceConstant';
	MaterialGlobeHalf_Rogue.SetParent(TheHealthHUDMatTemplateRogue_Half);

	MaterialGlobeHalf_Archer = new(self) class'MaterialInstanceConstant';
	MaterialGlobeHalf_Archer.SetParent(TheHealthHUDMatTemplateArcher_Half);
}

native function UpdateSkillBarKeyDown(bool bRelease, int inCmdID, int slotCmdKey);

//native function BagSlotChange(int BagId,int SlotID);
native function OnItemSlotChanged( INT containerID, INT slotID );

native function PlayerExpChange( INT CurrentExp, INT MaxExp,INT ChangeExp );
native function PlayerLevelChange( int lastLv , int newLv );
native function PlayerPkModeChange( int PkMode );
native function PlayerBaseInfoChange();
native final function PlayerHPChange(int NewHP, int MaxHP);
native final function PlayerMPChange(int NewMP, int MaxMP);
native function PlayerAttributeChange( int AtrrId, int inValue );

native final function FlashHPBar();
native function BuffEnableAbility();

native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType slotType, ECONTAINER_SLOT containerID, int slotID, bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType slotType, ECONTAINER_SLOT containerID, int slotID, bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType slotType, ECONTAINER_SLOT containerID, int slotID, bool bDefault);
native function bool OnDragDrop(GFxObject srcSlotMC, GFxObject destSlotMC, ESlotPanelType srcSlotType, ECONTAINER_SLOT srcContainerID, int SrcSlotID, ESlotPanelType destSlotType, ECONTAINER_SLOT destContainerID, int destSlotID);

native function bool SkipCaptureInputKey(String key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

event CallUCSetExpText( GFxObject expData )
{
	ActionScriptVoid("UCSetExpText");
}

event CallUCSetCareer( GFxObject carrerData )
{
	IsBarTypeUpdated = true;
	ActionScriptVoid("UCSetCareer");
}

//event CallUCSetCtrlType( int CtrlType )
//{
//	ActionScriptVoid("UCSetCtrlType");
//}


native function bool DrawMyHUD( MMOHud H );
native function DrawHUDPostGFx( HUD H);

/** whether to only draw the HUD if the Pawn whose viewport we're drawing is the Owner of this Actor */
function bool OnlyDrawHUDForOwner()
{
	return false;
}

event float CallGetBloodGlobeDrawX()
{
	return ActionScriptFloat("UCGetBloodGlobeDrawX");
}

event float CallGetBloodGlobeDrawY()
{
	return ActionScriptFloat("UCGetBloodGlobeDrawY");
}

event float CallGetEnergyGlobeDrawX()
{
	return ActionScriptFloat("UCGetEnergyGlobeDrawX");
}

event float CallGetEnergyGlobeDrawY()
{
	return ActionScriptFloat("UCGetEnergyGlobeDrawY");
}

event CallShowGlobe(bool show)
{
	ActionScriptVoid("UCShowGlobe");	
}

event CallSetExpExtra(int exp)
{
	ActionScriptVoid("UCSetExpExtra");	
}

event CallSkillActivate(int index,int type, bool bLoop)
{
	ActionScriptVoid("UCSkillActivate");
}

//native final function OnClickNewSkillLearnedBtn();

//event UCShowNewSkillHintBtn( bool isShow )
//{
//	ActionScriptVoid("ShowNewSkillHintBtn");
//}
//
event UCToggleShowSkillShortcutButtons( bool isToShow )
{
	if( IsSkillSettingBtnShow == true && isVisible == false )
	{
		UCToggleShowSkillShortcutPanel( false );
	}
	IsSkillSettingBtnShow = isToShow;

	ActionScriptVoid( "UCSetSkillShortcutButtonsVisible" );
}
//
event SetSkillList( int skillListType, String listTitle, GFxObject Provider )
{
	ActionScriptVoid( "UCSetSkillShortcutPanelData" );
}
event UCToggleShowSkillShortcutPanel( bool isToShow )
{
	ActionScriptVoid( "UCSetSkillShortcutPanelVisible" );
}

event CallASHintForSkillSlot( int slotIdx, bool isToShow )
{
	ActionScriptVoid("UCHintForSkillSlot");
}

event CallASHintForItemSlot( bool isToSHow )
{
	ActionScriptVoid("UCHintForItemSlot");
	//ActionScriptVoid("AISearchTarget");
}

event CallAsToGraySlotIcon( GFxObject mc, bool isToGray )
{
	ActionScriptVoid("GraySlotIcon");
}

event CallShowExpExtraBuff(bool v)
{
	ActionScriptVoid("UCShowExpBuff");
}

event CallUCupdatePetSkill(int stateID)
{
	ActionScriptVoid("UCupdatePetSkill");
}
//===============================================================================================================
//===============================================================================================================
native function ASStartDrag(GFxObject srcItemSlot, ESlotPanelType srcSlotType, ECONTAINER_SLOT srcContainerID, int srcSlotID);
native function ASSlot_StartDrag( GFxObject mcSlot);

native function ASOnClickNewSkillLearnedBtn();

native function ASChangePetState(int stateID);

//Ability settings
native function ASShowTips_SkillSettingActive( GFxObject mc );
native function ASHideTips_SkillSettingActive( GFxObject mc );
native function ASSetSkill( int skillKeyType, int skillID );
native function ASCloseSkillList();

native function ASShowPanel( int idx );
native function ASHideShortcutButtons();

native function UpdatePetState(int stateID);
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{
	//FriendlyName=Skillbar
	DefaultVisible=true
	CloseOnEsc=false

	//ExpBarMaxWidth=724

	SkillSlotUpdState=0
	NeedRefreshSkillSlotDisable=false
	IsBarTypeUpdated=false

	TheHealthHUDMatTemplateBlood=MaterialInstanceConstant'GlobeMaterial.M_GlobeBLood'
	TheHealthHUDMatTemplateWarrior=MaterialInstanceConstant'GlobeMaterial.M_GlobeWarrior'
	TheHealthHUDMatTemplateMage=MaterialInstanceConstant'GlobeMaterial.M_GlobeMage'
	TheHealthHUDMatTemplateRogue=MaterialInstanceConstant'GlobeMaterial.M_GlobeRogue'
	TheHealthHUDMatTemplateArcher=MaterialInstanceConstant'GlobeMaterial.M_GlobeArcher'
	

	TheHealthHUDMatTemplateArcher_Half=MaterialInstanceConstant'GlobeMaterial.MT_Globe02_Allmp001'
	TheHealthHUDMatTemplateRogue_Half=MaterialInstanceConstant'GlobeMaterial.MT_Globe02_Allmp001'
	TheHealthHUDMatTemplateMage_Half=MaterialInstanceConstant'GlobeMaterial.MT_Globe02_Allmp001'
	TheHealthHUDMatTemplateWarrior_Half=MaterialInstanceConstant'GlobeMaterial.MT_Globe02_Allmp001'

	TheHealthHUDMatTemplateBlood_Half=MaterialInstanceConstant'GlobeMaterial.M_GlobeBLood_Half'
// 	TheHealthHUDMatTemplateWarrior_Half=MaterialInstanceConstant'GlobeMaterial.M_GlobeWarrior_Half'
// 	TheHealthHUDMatTemplateMage_Half=MaterialInstanceConstant'GlobeMaterial.M_GlobeMage_Half'
// 	TheHealthHUDMatTemplateRogue_Half=MaterialInstanceConstant'GlobeMaterial.M_GlobeRogue_Half'
// 	TheHealthHUDMatTemplateArcher_Half=MaterialInstanceConstant'GlobeMaterial.M_GlobeArcher_Half'

	GlobeFrameTextureFull=Texture2D'GlobeMaterial.T_GlobeFrameFullGlobe'
	GlobeFrameTexture=Texture2D'GlobeMaterial.T_GlobeFrame'
	GlobeGlassTexture=Texture2D'GlobeMaterial.T_GlobeGlass'
}
