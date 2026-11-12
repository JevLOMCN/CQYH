class MMOUIWeaponSkillRplc extends MMOUIBaseSlotContainer
	native(UI)
	dependson(TypeDefine)
	config(UIVisibleMask);

enum EWpnSklRplcPnlSltTp
{
	EWpSklRplcSltTp_Invalid,
	EWpSklRplcSltTp_WpSkillCusm1,
	EWpSklRplcSltTp_WpSkillCusm2,
	EWpSklRplcSltTp_WeaponOp
};

enum EWpnSklRplcCfmState
{
	EWpSklRplcCfmSta_None,
	EWpSklRplcCfmSta_Info,
	EWpSklRplcCfmSta_SideEfct,
	EWpSklRplcCfmSta_TryCnfmRare
};

var array<int> _CurWeaponCurSkill;
var array<int> _CurRplcWeaponSkill;

//
var OperationItemBagPos _ToOpWeapon;
//
var int WEAPON_SKILL_CONSUME_ITEM_ID;
var int WEAPON_SKILL_LIMIT_CONSUME_ITEM_ID;
var int _CurConsumeItemTemplateID;

var bool _IsGuildMode;

//
var GFxObject _WeaponSlotMc;
var GFxObject _ConsumeItemSlotMc;

//
var GFxObject _ConfirmBox;

//
native function NativeInitLoad();
//
native function BagSlotChange( int bagId, int slotId );
native function PlayerMoneyChange( EMONEY_TYPE MoneyType, int DeltaMoney);
//
native function bool OnLeftMousePress(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnLeftMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnRightMouseClick(GFxObject mc, ESlotPanelType SlotType, ECONTAINER_SLOT ContainerID, int SlotID,bool bDefault);
native function bool OnDragDrop(GFxObject srcSlotMC, GFxObject destSlotMC, ESlotPanelType srcSlotType, ECONTAINER_SLOT srcContainerID, int SrcSlotID, ESlotPanelType destSlotType, ECONTAINER_SLOT destContainerID, int destSlotID);
//event OnSubscribeItemSlotAction()
//{
//}

//event OnUnSubscribeItemSlotAction()
//{
//}
native function OnFocusGained();
native function OnFocusLost();
//
final native function ResetUI();
//
native function ConfirmReplaceWeaponSkill( GFxObject pMc );
native function CancelReplaceWeaponSkill( GFxObject pMc );
//
native function ConfirmTryReplaceWeaponSkill( GFxObject pMc );
native function CancelTryReplaceWeaponSkill( GFxObject pMc );
//Existing weapon ability
event CallASSetWSkillRplcCurSkillTotalCount( int cnt )
{
	ActionScriptVoid( "UCSetWSkillRplcCurSkillTotalCount" );
}

event CallASUpdateWSkillRplcCurSkillInfo( GFxObject dataArray )
{
	ActionScriptVoid( "UCUpdateWSkillRplcCurSkillInfo" );
}

//Weapon ability about to be replaced
event CallAsSetWSkillRplcSkillTotalCount( int cnt )
{
	ActionScriptVoid( "UCSetWSkillRplcSkillTotalCount" );
}

event CallASUpdatetWSkillRplcSkillInfo( GFxObject dataArray )
{
	ActionScriptVoid( "UCUpdatetWSkillRplcSkillInfo" );
}

//Cost
event CallASSetWSkillRplcCnsmItemData( int itemTemplateID, string iconName, int itemColor, int slotType )
{
	ActionScriptVoid( "UCSetWSkillRplcCnsmItemData" );
}

event CallASUpdateWSkillRplcItemNum( int needCnt, int curCnt )
{
	ActionScriptVoid( "UCUpdateWSkillRplcItemNum" );
}

//Effect for gaining ability
event CallASUpdateWSkillRplcEffect( int isTryReplace, int idx )
{
	ActionScriptVoid( "UCUpdateWSkillRplcEffect" );
}

event CallASSetWSkillRplcMoneyNeed( string moneyNeed )
{
	ActionScriptVoid( "UCSetWSkillRplcMoneyNeed" );
}

//Button animation
event CallASEnableWSkillRplcTryRplc( bool isEnable )
{
	ActionScriptVoid( "UCEnableWSkillRplcTryRplc" );
}
event CallASEnableWSkillRplcCnfmRplc( bool isEnable )
{
	ActionScriptVoid( "UCEnableWSkillRplcCnfmRplc" );
}

//Guild needs
event CallASSetWSkillRplcIsGuildEdition( bool isOrNot )
{
	ActionScriptVoid( "UCSetWSkillRplcIsGuildEdition" );
}

event CallASSetWSkillRplcGuildNeeds( string moneyNeed )
{
	ActionScriptVoid( "UCSetWSkillRplcGuildNeeds" );
}
//end
//===============================================================================================================
//===============================================================================================================
//
final native function ASClickToTryReplaceWeaponSkill();
final native function ASClickToReplaceWeaponSkill();

final native function ASShowTips_WeaponSkill( GFxObject mc );
final native function ASHideTips_WeaponSkill();

final native function ASShowTips_ConsumeItem( GFxObject mc );
final native function ASHideTips_ConsumeItem();
//===============================================================================================================
//===============================================================================================================
	
DefaultProperties
{
	//FriendlyName=WeaponSkill
	_CurConsumeItemTemplateID=-1
	WEAPON_SKILL_CONSUME_ITEM_ID=21001
	WEAPON_SKILL_LIMIT_CONSUME_ITEM_ID=21011
}
