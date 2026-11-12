class Guide extends Object implements(IExcelable,IReferenceID)
	native
	hidecategories(Object);

enum EGuideEventType
{
	EGUET_Null																				<DisplayName="EGUET_Null">,		                    // none
	EGUET_LevelChange																		<DisplayName="EGUET_LevelChange">,		            // level change (new system unlocked)
	EGUET_GetItem																			<DisplayName="EGUET_GetItem">,		                // obtain item
	EGUET_EnterZone																			<DisplayName="EGUET_EnterZone">,		            // enter zone
	EGUET_EnterGame																			<DisplayName="EGUET_EnterGame">,		            // enter game
	EGUET_EnterScene																		<DisplayName="EGUET_EnterScene">,		            // enter map
	EGUET_Move																				<DisplayName="EGUET_Move">,		                    // movement tutorial
	EGUET_CloseGuidePanel																	<DisplayName="EGUET_CloseGuidePanel">,		        // close tutorial panel
	EGUET_ClickNaviLink																		<DisplayName="EGUET_ClickNaviLink">,		        // click pathfinding hyperlink
	EGUET_OpenQuestDialogSinglePage															<DisplayName="EGUET_OpenQuestDialogSinglePage">,    // quest dialog (single page)
	EGUET_OpenQuestDialogMultiPage															<DisplayName="EGUET_OpenQuestDialogMultiPage">,	    // quest dialog (multi page)
	EGUET_QuestPageDown																		<DisplayName="EGUET_QuestPageDown">,		        // quest dialog page turn
	EGUET_QuestPageUp																		<DisplayName="EGUET_QuestPageUp">,		            // quest dialog page turn
	EGUET_CloseDialog																		<DisplayName="EGUET_CloseDialog">,		            // close dialog
	EGUET_DialogCompleteQuest																<DisplayName="EGUET_DialogCompleteQuest">,		    // click complete quest in dialog
	EGUET_DialogAcceptQuest																	<DisplayName="EGUET_DialogAcceptQuest">,		    // click accept quest in dialog
	EGUET_AcceptQuest																		<DisplayName="EGUET_AcceptQuest">,		            // accept quest
	EGUET_CompleteQuest																		<DisplayName="EGUET_CompleteQuest">,		        // complete quest
	EGUET_OpenNpcDialog																		<DisplayName="EGUET_OpenNpcDialog">,		        // NPC dialog
	EGUET_NpcDialogServerOption																<DisplayName="EGUET_NpcDialogServerOption">,	    // quest dialog
	EGUET_ShowBagPanel																		<DisplayName="EGUET_ShowBagPanel">,		            // open bag
	EGUET_UseItem																			<DisplayName="EGUET_UseItem">,		                // use item
	EGUET_DragItemToSkillBar																<DisplayName="EGUET_DragItemToSkillBar">,	        // drag item to hotbar
	EGUET_Emergency																			<DisplayName="EGUET_Emergency">,		            // emergency event
	EGUET_AddDrop																			<DisplayName="EGUET_AddDrop">,		                // first drop
	EGUET_ClickCompItem																		<DisplayName="EGUET_ClickCompItem">,		        // click crafting item
	EGUET_KillMonster																		<DisplayName="EGUET_KillMonster">,		            // first monster kill
	EGUET_LearnSkill																		<DisplayName="EGUET_LearnSkill">,		            // learn first ability
	EGUET_ClickFlyLink																		<DisplayName="EGUET_ClickFlyLink">,		            // click teleport shoes hyperlink
	EGUET_SelectQuestAward																	<DisplayName="EGUET_SelectQuestAward">,		        // select quest reward
	EGUET_ShowQuestPanel																	<DisplayName="EGUET_ShowQuestPanel">,		        // open quest panel
	EGUET_EquipWeapon																		<DisplayName="EGUET_EquipWeapon">,		            // equip item
	EGUET_KismetEnd																			<DisplayName="EGUET_KismetEnd">,		            // cutscene ended
	EGUET_ShowCharacterPanel																<DisplayName="EGUET_ShowCharacterPanel">,	        // open character attributes
	EGUET_GetAchievement																	<DisplayName="EGUET_GetAchievement">,		        // obtain achievement
	EGUET_ShowAchievementPanel																<DisplayName="EGUET_ShowAchievementPanel">,	        // open achievement panel
	EGUET_ShowDungeonEnterPanel																<DisplayName="EGUET_ShowDungeonEnterPanel">,        // open dungeon entry panel
	EGUET_ChooseDungeonLvPanel																<DisplayName="EGUET_ChooseDungeonLvPanel">,	        // select dungeon difficulty
	EGUET_EnterDungeon																		<DisplayName="EGUET_EnterDungeon">,		            // enter dungeon
	EGUET_ShowDungeonAward																	<DisplayName="EGUET_ShowDungeonAward">,		        // dungeon reward
	EGUET_ExitDungeonConfirm																<DisplayName="EGUET_ExitDungeonConfirm">,           // leave dungeon
	EGUET_ExitDungeon																		<DisplayName="EGUET_ExitDungeon">,		            // leave dungeon
	EGUET_Extendbag																			<DisplayName="EGUET_Extendbag">,		            // expand bag
	EGUET_ShowMasterPanel																	<DisplayName="EGUET_ShowMasterPanel">,		        // open master/apprentice panel
	EGUET_ChooseChatChanel																	<DisplayName="EGUET_ChooseChatChanel">,		        // select chat channel
	EGUET_SendChat																			<DisplayName="EGUET_SendChat">,		                // chat
	EGUET_ShowMailPanel																		<DisplayName="EGUET_ShowMailPanel">,		        // open mail panel
	EGUET_StudentApply																		<DisplayName="EGUET_StudentApply">,		            // apply for a master
	EGUET_ShowHeavenPanel																	<DisplayName="EGUET_ShowHeavenPanel">,		        // open realm panel
	EGUET_SoulToPoint																		<DisplayName="EGUET_SoulToPoint">,		            // convert soul to training points
	EGUET_ShowSkillLevelUpPanel																<DisplayName="EGUET_ShowSkillLevelUpPanel">,        // ability level-up panel
	EGUET_SkillLevelUp																		<DisplayName="EGUET_SkillLevelUp">,		            // ability level-up
	EGUET_ShowSkillSettingPanel																<DisplayName="EGUET_ShowSkillSettingPanel">,        // open ability config screen
	EGUET_SetSkillSetting																	<DisplayName="EGUET_SetSkillSetting">,		        // enter ability config screen from ability
	EGUET_EquipSkill																		<DisplayName="EGUET_EquipSkill">,		            // click to select an ability and equip it to the hotbar
	EGUET_ShowWeaponSkillPanel																<DisplayName="EGUET_ShowWeaponSkillPanel">,	        // open inscription screen
	EGUET_DropWeaponToWeaponSkill															<DisplayName="EGUET_DropWeaponToWeaponSkill">,      // place weapon into inscription screen
	EGUET_DropWeaponSkill																	<DisplayName="EGUET_DropWeaponSkill">,		        // place inscription
	EGUET_EmbedWeaponSkill																	<DisplayName="EGUET_EmbedWeaponSkill">,		        // embed inscription
	EGUET_WeaponSkillLvUp																	<DisplayName="EGUET_WeaponSkillLvUp">,		        // upgrade inscription
	EGUET_ShowGuildPanel																	<DisplayName="EGUET_ShowGuildPanel">,		        // open guild panel
	EGUET_ShowGuildViewPanel																<DisplayName="EGUET_ShowGuildViewPanel">,	        // open guild view panel
	EGUET_ShowTeamPanel																		<DisplayName="EGUET_ShowTeamPanel">,		        // open team panel
	EGUET_CreateTeam																		<DisplayName="EGUET_CreateTeam">,		            // create team
	EGUET_ClickDecompEquip																	<DisplayName="EGUET_ClickDecompEquip">,		        // click decompose equipment
	EGUET_DecompEquip																		<DisplayName="EGUET_DecompEquip">,		            // decompose equipment
	EGUET_CompEquip																			<DisplayName="EGUET_CompEquip">,		            // craft equipment
	EGUET_ShowGemPanel																		<DisplayName="EGUET_ShowGemPanel">,		            // open spirit stone socketing panel
	EGUET_GemPanelWithEquip																	<DisplayName="EGUET_GemPanelWithEquip">,	        // drag weapon into spirit stone socketing panel
	EGUET_GemPanelWithGem																	<DisplayName="EGUET_GemPanelWithGem">,		        // drag spirit stone into spirit stone socketing panel
	EGUET_GemEmbed																			<DisplayName="EGUET_GemEmbed">,		                // spirit stone socketing
	EGUET_ShowStrongPanel																	<DisplayName="EGUET_ShowStrongPanel">,		        // open physique enhancement panel
	EGUET_ChooseHeadPart																	<DisplayName="EGUET_ChooseHeadPart">,		        // select head
	EGUET_Strong																			<DisplayName="EGUET_Strong">,		                // enhance
	EGUET_ShowGasPraPanel																	<DisplayName="EGUET_ShowGasPraPanel">,		        // open qi cultivation panel
	EGUET_OpenGas																			<DisplayName="EGUET_OpenGas">,		                // open qi sea
	EGUET_EquipGas																			<DisplayName="EGUET_EquipGas">,		                // equip qi sea
	EGUET_OpenShopPanel																		<DisplayName="EGUET_OpenShopPanel">,		        // open shop panel
	EGUET_ShowShopPanel																		<DisplayName="EGUET_ShowShopPanel">,		        // show shop panel
	EGUET_CloseShopPanel																	<DisplayName="EGUET_CloseShopPanel">,		        // close shop panel
	EGUET_BuyItem																			<DisplayName="EGUET_BuyItem">,		                // buy item
	EGUET_ShowActivityPanel																	<DisplayName="EGUET_ShowActivityPanel">,	        // activity panel
	EGUET_SwitchLine																		<DisplayName="EGUET_SwitchLine">,		            // switch line
	EGUET_ShowWeaponLvUpPanel																<DisplayName="EGUET_ShowWeaponLvUpPanel">,	        // weapon upgrade
	EGUET_ActivateJourneyType																<DisplayName="EGUET_ActivateJourneyType">,	        // activate Mafa journal category
	EGUET_OpenNpcMap																		<DisplayName="EGUET_OpenNpcMap">,		            // open Mafa monster-hunting map
	EGUET_ReputationFull																	<DisplayName="EGUET_ReputationFull">,		        // reputation full
	EGUET_OpenJourneyView																	<DisplayName="EGUET_OpenJourneyView">,	            // open Mafa reward screen
	EGUET_GetJourneyAwards                                                                  <DisplayName="EGUET_GetJourneyAwards">,             // claim Mafa rewards
	EGUET_GetTitle                                                                          <DisplayName="EGUET_GetTitle">,                     // Get title
	EGUET_ChooseTitle                                                                       <DisplayName="EGUET_ChooseTitle">,                  // Choose title
	EGUET_EquipTitle                                                                        <DisplayName="EGUET_EquipTitle">,                   // Equip title
	EGUET_BagFull                                                                           <DisplayName="EGUET_BagFull">,                      // First time bag is full
};

enum EGuideType
{
	EGDT_Null                                                                               <DisplayName="EGDT_Null">,
	EGDT_Illustrate                                                                         <DisplayName="EGDT_Illustrate">,
	EGDT_TipLT                                                                              <DisplayName="EGDT_TipLT">,
	EGDT_TipRT                                                                              <DisplayName="EGDT_TipRT">,
	EGDT_TipLB                                                                              <DisplayName="EGDT_TipLB">,
	EGDT_TipRB                                                                              <DisplayName="EGDT_TipRB">,
	EGDT_ArrowT                                                                             <DisplayName="EGDT_ArrowT">,
	EGDT_ArrowB                                                                             <DisplayName="EGDT_ArrowB">,
	EGDT_ArrowL                                                                             <DisplayName="EGDT_ArrowL">,
	EGDT_ArrowR                                                                             <DisplayName="EGDT_ArrowR">,
	EGDT_Dialog                                                                             <DisplayName="EGDT_Dialog">,
	EGDT_Sound                                                                              <DisplayName="EGDT_Sound">,
};

enum EGuideBoxType
{
	EGBT_Null                                                                               <DisplayName="EGBT_Null">,
	EGBT_Square                                                                             <DisplayName="EGBT_Square">,
	EGBT_Triangle                                                                           <DisplayName="EGBT_Triangle">,
	EGBT_Circle                                                                             <DisplayName="EGBT_Circle">,
	EGBT_SquareSmall                                                                        <DisplayName="EGBT_SquareSmall">,
};

enum EGuideArrowTipsType
{
	EGATT_Null                                                                              <DisplayName="EGATT_Null">,
	EGATT_1                                                                                 <DisplayName="EGATT_1">,
	EGATT_2                                                                                 <DisplayName="EGATT_2">,
	EGATT_3                                                                                 <DisplayName="EGATT_3">,
	EGATT_4                                                                                 <DisplayName="EGATT_4">,
	EGATT_5                                                                                 <DisplayName="EGATT_5">,
	EGATT_6                                                                                 <DisplayName="EGATT_6">,
	EGATT_7                                                                                 <DisplayName="EGATT_7">,
	EGATT_8                                                                                 <DisplayName="EGATT_8">,
	EGATT_9                                                                                 <DisplayName="EGATT_9">,
	EGATT_10                                                                                <DisplayName="EGATT_10">,
};

enum ETraceMenuBarType
{
	ETMBT_Left                                                                              <DisplayName="ETMBT_Left">,
	ETMBT_Right                                                                             <DisplayName="ETMBT_Right">,
};

struct native GuidePicture
{
	var() Texture2D Picture                                                                 <DisplayName="Picture" | ExportExcel="Client">;
};

struct native GuideItem
{
	var() int ItemId                                                                        <DisplayName="ItemId" | ExportExcel="Client">;
};

struct native GuideSkill
{
	var() int SkillId                                                                       <DisplayName="SkillId" | ExportExcel="Client">;
};

struct native GuideOffset
{
	var() int X                                                                             <DisplayName="X" | ExportExcel="Client">;
	var() int Y                                                                             <DisplayName="Y" | ExportExcel="Client">;
};

struct native GuideSize
{
	var() int Width                                                                         <DisplayName="Width" | ExportExcel="Client">;
	var() int Height                                                                        <DisplayName="Height" | ExportExcel="Client">;
};

struct native GuideTargetComponent
{
	var() init string TargetComponentName                                                   <DisplayName="TargetComponentName" | ExportExcel="Client">;
};

var() editconst int GuideId                                                                 <DisplayName="GuideId" | ExportExcel="Client">;
var() EGuideType GuideType                                                                  <DisplayName="GuideType" | ExportExcel="Client">;
var() init string GuideDesc         				                                        <DisplayName="GuideDesc" | ExportExcel="Client">;
var() bool Enable                                                                           <DisplayName="Enable" | ExportExcel="Client">;
var() EGuideEventType ActivateGuideType                                                     <DisplayName="ActivateGuideType" | ExportExcel="Client">;
var() EGuideEventType DeactiveGuideType                                                     <DisplayName="DeactiveGuideType" | ExportExcel="Client">;

var(Condition) int LevelReq				                                                    <DisplayName="LevelReq" | ExportExcel="Client">;
var(Condition) int GuideReq				                                                    <DisplayName="GuideReq" | ExportExcel="Client">;
var(Condition) int QuestReq				                                                    <DisplayName="QuestReq" | ExportExcel="Client">;
var(Condition) editfixedsize array<GuideItem> ItemReqNew      	                            <DisplayName="ItemReqNew" | ExportExcel="Client">;
var(Condition) editfixedsize array<GuideSkill> SkillReq      	                            <DisplayName="SkillReq" | ExportExcel="Client">;
var(Condition) int ItemQuaReq				                                                <DisplayName="ItemQuaReq" | ExportExcel="Client">;
var(Condition) int NpcReq				                                                    <DisplayName="NpcReq" | ExportExcel="Client">;
var(Condition) EDOODAD_TYPE DoodadTypeReq		                                            <DisplayName="DoodadTypeReq" | ExportExcel="Client">;
var(Condition) EPlayerClass ClassReq			                                            <DisplayName="ClassReq" | ExportExcel="Client">;

var(Picture) editfixedsize array<GuidePicture> Picture				                        <DisplayName="Picture" | ExportExcel="Client">;

var(Sound) SoundCue GuideSound                                                              <DisplayName="GuideSound" | ExportExcel="Client">;
var(Sound) Texture2D GuideSoundIcon                                                         <DisplayName="GuideSoundIcon" | ExportExcel="Client">;

var(Display) init string TargetWindowName				                                    <DisplayName="TargetWindowName" | ExportExcel="Client">;
var(Display) editfixedsize array<GuideTargetComponent> TargetComponent	                    <DisplayName="TargetComponent" | ExportExcel="Client">;
var(Display) GuideOffset TargetComponentOffset 				                                <DisplayName="TargetComponentOffset" | ExportExcel="Client">;
var(Display) bool WithBox 				                                                    <DisplayName="WithBox" | ExportExcel="Client">;
var(Display) GuideOffset BoxOffset 				                                            <DisplayName="BoxOffset" | ExportExcel="Client">;
var(Display) GuideSize BoxSize 				                                                <DisplayName="BoxSize" | ExportExcel="Client">;
var(Display) EGuideBoxType BoxType				                                            <DisplayName="BoxType" | ExportExcel="Client">;
var(Display) EGuideArrowTipsType ArrowTipsType				                                <DisplayName="ArrowTipsType" | ExportExcel="Client">;
var(Display) bool TraceBagSlot              				                                <DisplayName="TraceBagSlot" | ExportExcel="Client">;
var(Display) bool TraceMenuBar				                                                <DisplayName="TraceMenuBar" | ExportExcel="Client">;
var(Display) ETraceMenuBarType TraceMenuBarType			                                    <DisplayName="TraceMenuBarType" | ExportExcel="Client">;
var(Display) int TraceMenuType				                                                <DisplayName="TraceMenuType" | ExportExcel="Client">;
var(Display) int TraceMenuChildType				                                            <DisplayName="TraceMenuChildType" | ExportExcel="Client">;
var(Display) bool TraceJourney				                                                <DisplayName="TraceJourney" | ExportExcel="Client">;
var(Display) int TraceJourneyType     			                                            <DisplayName="TraceJourneyType" | ExportExcel="Client">;
var(Display) bool TraceSkillBar                                                             <DisplayName="TraceSkillBar" | ExportExcel="Client">;
var(Display) int TraceSkillIndex                                                            <DisplayName="TraceSkillIndex" | ExportExcel="Client">;


event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues)
{
}

event ScriptDataToExcel(out array<string> Row)
{
}

event ScriptImportFromExcel(array<string> Values)
{
// 	local string val;
// 	val = values[0];
// 	QuestTempID = int(val);
}

native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function bool IsNeedArchetype();
native function ImportFromExcel(array<string> values);
native function InitDataToExcel();
native function int GetTemplateID() const;

DefaultProperties
{
	Picture(0)=()
	Picture(1)=()
	Picture(2)=()
	Picture(3)=()
	Picture(4)=()

	TargetComponent(0)=()
	TargetComponent(1)=()
	TargetComponent(2)=()
	TargetComponent(3)=()
	TargetComponent(4)=()

	ItemReqNew(0)=()
	ItemReqNew(1)=()
	ItemReqNew(2)=()
	ItemReqNew(3)=()
	ItemReqNew(4)=()

	SkillReq(0)=()
	SkillReq(1)=()
	SkillReq(2)=()
	SkillReq(3)=()
	SkillReq(4)=()

	ClassReq=EPCT_None
}
