class MMOUITips extends MMOUIBase
	config(MMOGame)
	native(UI);

enum ItemSlotType
{
	TFT_ItemSlot,
	TFT_ItemBar,
	TFT_Tab,
	TFT_MenuBar,
	TFT_Attribute,
	TFT_AwardIcon,
	TFT_Other,
	TFT_BuffSlot,
	TFT_MountSlot,
	TFT_RepairAll,
	TFT_RuneActive,
	TFT_RuneButton,
	TFT_PlainText,
	TFT_NormalText,
	TFT_FreeStyle,
	TFT_AutoSizeTips,//tips whose width auto-adjusts to the character count
	TFT_AvartaRuneButton,
	TFT_SkillEquipped, //show abilities on the character
	TFT_SkillSlotTemplate,//template ability, the ability attached to the Slot component
	TFT_SkillOfMyWithNext,//template ability, the ability attached to the Slot component, with next level
	TFT_SkillTemplate,//template ability, non-Slot component, ability obtained via the component's UniqueSkillID
	TFT_SkillTemplateWithNext,//template ability, non-Slot component, ability obtained via the component's UniqueSkillID, with next level
	TFT_LockedSlot,
	TFT_HeavenGasSlot,
	TFT_EquipSlotEnhance,//enhance attribute stage hint
	TFT_Link,//hyperlink
	TFT_EquipDurability,
	TFT_GoldStore,
};

enum eCommonTipsStyle
{
	CM_TIPS_Style1,
	CM_TIPS_Style2,
	CM_TIPS_Style3
};

var localized string string1;
var localized string string2;
var localized string string3;
var localized string string4;

var config array<string> AttributeName;
var config array<int> AttributeId;

var GFxObject TipsText;
var GFxObject TipsTextR;
var GFxObject TipsBg;
var GFxObject NextSkillTip;
var GFxObject subTipPanel;

var bool IsTipsAnchored;
var Vector TipAnchoredPos;
var bool IsAnchored3D;

var bool ShowCompareTip;
var bool ShowNextSkillTip;
var bool ShowDetail;
var ItemSlotType CurrentTipsType;
var GFxObject CurrentFromObject;

var Vector2D LastCallSetTipsPosMousePos;
var int TipPosOffsetX;
var int TipPosOffsetY;
var vector2d TipCurrentSize;

event InitOnce()
{
	
}

event string GetSkillDescription(MMOAbility Skill)
{
	return repl(skill.GetAbilityDescribe(),"\\n","<br>");
}

event string GetItemDescription(const Item Item)
{
	return repl(item.Description ,"\\n","<br>");
}

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
	
	//ItemTipCompare = GetObject("itemTipCompare");

	`log("___MMOUIManager'.static.SetShowTips(ShowTips)");
	class'MMOUIManager'.static.SetShowTipsDelegate(ShowTips);
	class'MMOUIManager'.static.SetHideTipsDelegate(HideTips);
	class'MMOUIManager'.static.SetShowTipsHTMLDelegate(ShowTipsHTML);
	class'MMOUIManager'.static.SetToggleTipAidDelegate(ToggleTipAid);
	class'MMOUIManager'.static.SetShowFreeStyleTipsDelegate(ShowFreeStyleTips);
	class'MMOUIManager'.static.SetShowTextTipsDelegate(ShowTextTips);
	class'MMOUIManager'.static.SetShowLinkTipsDelegate(ShowLinkTips);

	class'MMOUIManager'.static.SetIsTipsShowDelegate(IsTipsShow);

	class'MMOUIManager'.static.SetAnchorTipsDelegate(SetAnchored);
}

function HideTipsEx()
{
	SetVisible(false);
}

native function bool NativeIsTipsShow();
function bool IsTipsShow()
{ 
	return NativeIsTipsShow();
}

event CallUpdateItemTipData(GFxObject Provider)
{
	subTipPanel = GetSubTipPanelByType( TFT_ItemSlot );
    if ( none != subTipPanel )
    {
    	subTipPanel.ActionScriptVoid("updateData");
    }
    else
    {
    	`log("CallUpdateItemTipData::can't find ItemTip Panel");
    }
}

event CallUpdateMountTipData(GFxObject Provider)
{
	subTipPanel = GetSubTipPanelByType( TFT_MountSlot );
    if ( none != subTipPanel )
    {
    	subTipPanel.ActionScriptVoid("updateData");
    }
    else
    {
    	`log("CallUpdateMountTipData::can't find MountTip Panel");
    }
}

event CallUpdateItemTipCompareData( int idx, GFxObject Provider )
{
	ShowCompareTip = ( Provider != none );
	ActionScriptVoid("UpdateExtraItemTips");
}

event CallASSetExtraItemTipsPosX( int posX )
{
	ActionScriptVoid("SetExtraItemTipsPosX");
}

event CallASSetExtraItemTipsPosY( int posY )
{
	ActionScriptVoid("SetExtraItemTipsPosY");
}

event int CallASGetExtraItemTipsWidth()
{
	return ActionScriptInt("GetExtraItemTipsWidth");
}

event int CallASGetExtraItemTipsHeight()
{
	return ActionScriptInt("GetExtraItemTipsHeight");
}

event CallUpdateSkillTipData(GFxObject Provider)
{
	subTipPanel = GetSubTipPanelByType( TFT_SkillEquipped );
    if ( none != subTipPanel )
    {
    	subTipPanel.ActionScriptVoid("updateData");
    }
    else
    {
    	`log("CallUpdateSkillTipData::can't find SkillTip Panel");
    }
}

event CallUpdateNextSkillTipData(GFxObject Provider)
{
	if ( none == NextSkillTip )
    {
		NextSkillTip = GetObject( "nextSkillTip" );
    }

    if ( none != NextSkillTip )
    {
    	NextSkillTip.ActionScriptVoid("updateData");
    }
    else
    {
    	`log("CallUpdateNextSkillTipData::can't find SkillTip Panel");
    }
}

event CallCloseAll()
{
	ActionScriptVoid("CloseAll");
}

native function bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

native function NativeInitLoad();
native final function ShowLinkTips( GFxObject FromObject );
//native final function ShowFixPosItemTips(Item pItem, GFxObject FromObject);
native final function ShowFreeStyleTips(string title,string text,int showStyle, int displayPriority);
native final function ShowEquipSlotEnhanceTips( int enhanceClassID, int curEnhanceLv );
native final function ShowTextTips(string text);
native final function ShowTipsHTML(ItemSlotType TipsType, GFxObject FromObject, bool IsRefreshing = false, int displayPriority = 3 );
native final function ToggleTipAid( );
native final function SetAnchored( Vector toAnchorPos, bool isPos3D );
native final function RefreshTip( );

native final function ShowTips(int tipstype ,int  objectId,GfxObject widge );
native final function GfxObject GetSubTipPanelByType( ItemSlotType TipsType );
native final function HideTips();
native final function AutoPosition();
native function CallUpdateItemTipDataNative( GFxObject Provider );

native function PostAdvance(float DeltaTime);
native function SetTipsPostion( Vector2D toPos );

DefaultProperties
{
	IsAnchored3D=false
	IsTipsAnchored=false
	TipPosOffsetX=25
	TipPosOffsetY=25
	DefaultVisible=false
	CloseOnEsc=false
}