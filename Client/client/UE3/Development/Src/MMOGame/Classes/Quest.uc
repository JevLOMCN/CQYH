class Quest extends Object implements(IExcelable,IReferenceID)
	native
	hidecategories(Object);

enum EQuestResetType
{
	EQRT_None                                                                               <DisplayName="EQRT_None">,
	EQRT_Day	                                                                            <DisplayName="EQRT_Day">,
	EQRT_Week	                                                                            <DisplayName="EQRT_Week">,
	EQRT_Month	                                                                            <DisplayName="EQRT_Month">,
	EQRT_Year	                                                                            <DisplayName="EQRT_Year">
};

//
// basic fields
//
struct native QuestResetTime
{
	var() int Year                                                                          <DisplayName="Year" | ClampMin=1900 | ExportExcel="Both">;
	var() int Month                                                                         <DisplayName="Month" | ClampMin=0 | ClampMax=12 | ExportExcel="Both">;
	var() int Day		                                                                    <DisplayName="Day" | ClampMin=0 | ClampMax=31 | ExportExcel="Both">;
	var() int Hour		                                                                    <DisplayName="Hour" | ClampMin=0 | ClampMax=23 | ExportExcel="Both">;
	var() int Minute	                                                                    <DisplayName="Minute" | ClampMin=0 | ClampMax=59 | ExportExcel="Both">;
	var() int Second	                                                                    <DisplayName="Second" | ClampMin=0 | ClampMax=59 | ExportExcel="Both">;
};

struct native QuestCheckKillNpc
{
	var() NPCTemplate CheckKillNpcNew		                                                <DisplayName="CheckKillNpc" | ExportExcel="Both">;
	var int CheckKillNpcNewId;
	var() int CheckKillNpcGroupId                                                           <DisplayName="CheckKillNpcGroupId" | ExportExcel="Both">;
	var() int CheckKillNpcCnt	                                                            <DisplayName="CheckKillNpcCnt" | ExportExcel="Both">;
	var() init string Description                                                           <DisplayName="CheckKillNpcDesc" | ExportExcel="Client">;
	var() int CheckKillNpcMap                                                               <DisplayName="CheckKillNpcMap" | ExportExcel="Client">;
	var() init string CheckKillNpcNav                                                       <DynamicList="CheckKillNpcNav" | ExportExcel="Client">;
	var() bool CheckKillNpcFly                                                              <DisplayName="CheckKillNpcFly" | ExportExcel="Client">;
};

struct native QuestItem
{
	var() editconst Item CheckItem	                                                        <DisplayName="CheckItem">;
	var() int CheckItemID                                                                   <DisplayName="CheckItemID" | ExportExcel="Both">;
	var() int CheckItemCnt	                                                                <DisplayName="CheckItemCnt" | ExportExcel="Both">;
	var() init string Description                                                           <DisplayName="CheckItemDesc" | ExportExcel="Client">;
};

enum EItemDropSource
{
	EIDS_None                                                                               <DisplayName="None">,
	EIDS_Doodad                                                                             <DisplayName="Doodad">,
	EIDS_NPC                                                                                <DisplayName="NPC">,
};

struct native QuestCheckItem
{
	var() editconst Item CheckItem	                                                        <DisplayName="CheckItem">;
	var() int CheckItemID                                                                   <DisplayName="CheckItemID" | ExportExcel="Both">;
	var() int CheckItemCnt	                                                                <DisplayName="CheckItemCnt" | ExportExcel="Both">;
	var() init string Description                                                           <DisplayName="CheckItemDesc" | ExportExcel="Client">;
	var() int CheckItemMap                                                                  <DisplayName="CheckItemMap" | ExportExcel="Client">;
	var() init string CheckItemNav                                                          <DynamicList="CheckItemNav" | ExportExcel="Client">;
	var() bool CheckItemFly                                                                 <DisplayName="CheckItemFly" | ExportExcel="Client">;
	var() EItemDropSource ItemDropSource                                                    <DisplayName="ItemDropSource" | ExportExcel="Client">;
};

struct native QuestCheckKillDoodad
{
	var() int CheckKillDoodadID                                                             <DisplayName="CheckKillDoodadID" | ExportExcel="Both">;
	var() int CheckKillDoodadCnt	                                                        <DisplayName="CheckKillDoodadCnt" | ExportExcel="Both">;
	var() init string CheckKillDoodadDesc	                                                <DisplayName="CheckKillDoodadDesc" | ExportExcel="Client">;
	var() int CheckKillDoodadMap                                                            <DisplayName="CheckKillDoodadMap" | ExportExcel="Client">;
	var() init string CheckKillDoodadNav                                                    <DynamicList="CheckKillDoodadNav" | ExportExcel="Client">;
	var() bool CheckKillDoodadFly                                                           <DisplayName="CheckKillDoodadFly" | ExportExcel="Client">;
};

enum ECheckKillPlayerLevelStart
{
	ECKPLS_30                                                                               <DisplayName="30">,
	ECKPLS_50                                                                               <DisplayName="50">,
	ECKPLS_70                                                                               <DisplayName="70">,
};

enum ECheckKillPlayerLevelEnd
{
	ECKPLE_50                                                                               <DisplayName="50">,
	ECKPLE_70                                                                               <DisplayName="70">,
	ECKPLE_90                                                                               <DisplayName="90">,
};

struct native QuestCheckKillPlayer
{
	var() ECheckKillPlayerLevelStart eCheckKillPlayerLevelStart                             <DisplayName="CheckKillPlayerLevelStart">;
	var int CheckKillPlayerLevelStart					                                    <ExportExcel="Both">;
	var() ECheckKillPlayerLevelEnd eCheckKillPlayerLevelEnd                                 <DisplayName="CheckKillPlayerLevelEnd">;
	var int CheckKillPlayerLevelEnd					                                        <ExportExcel="Both">;
	var() int CheckKillPlayerBuffId                                                         <DisplayName="CheckKillPlayerBuffId" | ExportExcel="Both">;
	var() int CheckKillPlayerCnt                                                            <DisplayName="CheckKillPlayerCnt" | ExportExcel="Both">;
	var() init string CheckKillPlayerDesc	                                                <DisplayName="CheckKillPlayerDesc" | ExportExcel="Client">;
};

struct native QuestDestination
{
	var() int DestMapId                                                                     <DisplayName="DestMapId" | ExportExcel="Server">;
	var() float DestX                                                                       <DisplayName="DestX" | ExportExcel="Server">;
	var() float DestY                                                                       <DisplayName="DestY" | ExportExcel="Server">;
	var() float DestZ                                                                       <DisplayName="DestZ" | ExportExcel="Server">;
};

struct native QuestCheckVar
{
	var() init string CheckVarType                                                          <DynamicList="CheckVarType" | ExportExcel="Both">;
	var() int CheckVarID	                                                                <DisplayName="CheckVarID" | ExportExcel="Both">;
	var() int CheckVarCnt	                                                                <DisplayName="CheckVarCnt" | ExportExcel="Both">;
	var() init string Description                                                           <DisplayName="CheckVarDesc" | ExportExcel="Client">;

	structdefaultproperties
	{
		CheckVarType = "vtInvalid";
		CheckVarID = -1;
	}
};

struct native QuestCheckClassVar
{
	var() int CheckClassVarCnt	                                                            <DisplayName="CheckClassVarCnt" | ExportExcel="Client">;
	var() init string CheckClassVarDesc                                                     <DisplayName="CheckClassVarDesc" | ExportExcel="Client">;
};

struct native QuestAwardItem
{
	var() editconst Item AwardItem	                                                        <DisplayName="AwardItem">;	
	var() int AwardItemID                                                                   <DisplayName="AwardItemID" | ExportExcel="Both">;
	var() int AwardItemCnt	                                                                <DisplayName="AwardItemCnt" | ExportExcel="Both">;
};

struct native QuestSelectableAwardItem
{
	var() editconst Item SelectableAwardItem		                                        <DisplayName="SelectableAwardItem">;
	var() int SelectableAwardItemID                                                         <DisplayName="SelectableAwardItemID" | ExportExcel="Both">;
	var() int SelectableAwardItemCnt	                                                    <DisplayName="SelectableAwardItemCnt" | ExportExcel="Both">;
};

struct native QuestPageSound
{
	var() SoundCue PageSound                                                                <DisplayName="PageSound" | ExportExcel="Client">;
	var transient init string PageSoundPath;
};

// quest acquisition hint
enum EEmergencyTipsType
{
	EEMTT_NewEvent                                                                          <DisplayName="EEMTT_NewEvent">,
	EEMTT_Help                                                                              <DisplayName="EEMTT_Help">,
	EEMTT_Danger                                                                            <DisplayName="EEMTT_Danger">,
};

// quest accept method
enum EAcceptQuestType
{
	EAQT_Npc                                                                                <DisplayName="EAQT_Npc">,
	EAQT_Item                                                                               <DisplayName="EAQT_Item">,
	EAQT_Zone                                                                               <DisplayName="EAQT_Zone">,
	EAQT_Level                                                                              <DisplayName="EAQT_Level">,
};

// quest submit method
enum ESubmitQuestType
{
	ESQT_Npc                                                                                <DisplayName="ESQT_Npc">,
	ESQT_Item                                                                               <DisplayName="ESQT_Item">,
	ESQT_Zone                                                                               <DisplayName="ESQT_Zone">,
	ESQT_Auto                                                                               <DisplayName="ESQT_Auto">,
	ESQT_Manual                                                                             <DisplayName="ESQT_Manual">,
};

//
// general quest settings
//
var() int QuestTempID				                                                        <DisplayName="QuestTempID" | ExportExcel="Both">;
var() init string QuestName				                                                    <DisplayName="QuestName" | ExportExcel="Client">;
var() EQUEST_TYPE QuestType                                                                 <DisplayName="QuestType" | ExportExcel="Both">;
var() int QuestChapter                                                                      <DisplayName="QuestChapter" | ExportExcel="Both">;
var() int QuestStage                                                                        <DisplayName="QuestStage" | ExportExcel="Both">;
var() int QuestLevel                                                                        <DisplayName="QuestLevel" | ExportExcel="Both">;
var() int CircleID                                                                          <DisplayName="CircleID" | ExportExcel="Both">;
var() int StarLevel                                                                         <DisplayName="StarLevel" | ExportExcel="Both">;
var() EQuestResetType ResetType		                                                        <DisplayName="ResetType" | ExportExcel="Both">;
var() QuestResetTime ResetTime		                                                        <DisplayName="ResetTime">;
var int nResetTime					                                                        <ExportExcel="Both">;
var() bool CanAbandon				                                                        <DisplayName="CanAbandon" | ExportExcel="Both">;
var() bool CanShare					                                                        <DisplayName="CanShare" | ExportExcel="Both">;
var() bool CanPublish				                                                        <DisplayName="CanPublish" | ExportExcel="Both">;
var() int DurationLimit				                                                        <DisplayName="DurationLimit" | ExportExcel="Both">;

var() editconst editfixedsize array<QuestDestination> Destinations                          <DisplayName="Destinations" | ExportExcel="Server">;

//
// quest description
//
var(QuestDescription) init string AcceptQuestDesc                                           <DisplayName="AcceptQuestDesc" | ExportExcel="Client">;
var(QuestDescription) init string QuestDescript			                                    <DisplayName="QuestDescript" | ExportExcel="Client" | Edit="TextEdit_Nouse">;
var(QuestDescription) init string QuestCompletedDescript			                        <DisplayName="QuestCompletedDescript" | ExportExcel="Client" | Edit="TextEdit_Nouse">;
var(QuestDescription) init string AcceptQuestDialog		                                    <DisplayName="AcceptQuestDialog" | ExportExcel="Client" | Edit="TextEdit_Nouse">;
var(QuestDescription) init string PreSubmitQuestDialog	                                    <DisplayName="PreSubmitQuestDialog" | ExportExcel="Client" | Edit="TextEdit_Nouse">;
var(QuestDescription) init string CompleteQuestDialog	                                    <DisplayName="CompleteQuestDialog" | ExportExcel="Client" | Edit="TextEdit_Nouse">;

//
// quest sound effects
//
var(QuestSound) SoundCue QuestSoundAccept                                                   <DisplayName="QuestSoundAccept" | ExportExcel="Client">;
var transient init string QuestSoundAcceptPath;
var(QuestSound) editfixedsize array<QuestPageSound> QuestPageSoundAccept                    <DisplayName="QuestPageSoundAccept" | ExportExcel="Client">;
var(QuestSound) Texture2D QuestSoundAcceptedIcon                                            <DisplayName="QuestSoundAcceptedIcon" | ExportExcel="Client">;
var transient init string QuestSoundAcceptedIconPath;
var(QuestSound) SoundCue QuestSoundAccepted                                                 <DisplayName="QuestSoundAccepted" | ExportExcel="Client">;
var transient init string QuestSoundAcceptedPath;

var(QuestSound) SoundCue QuestSoundUnderway                                                 <DisplayName="QuestSoundUnderway" | ExportExcel="Client">;
var transient init string QuestSoundUnderwayPath;

var(QuestSound) SoundCue QuestSoundSubmit                                                   <DisplayName="QuestSoundSubmit" | ExportExcel="Client">;
var transient init string QuestSoundSubmitPath;
var(QuestSound) editfixedsize array<QuestPageSound> QuestPageSoundSubmit                    <DisplayName="QuestPageSoundSubmit" | ExportExcel="Client">;
var(QuestSound) Texture2D QuestSoundCompleteIcon                                            <DisplayName="QuestSoundCompleteIcon" | ExportExcel="Client">;
var transient init string QuestSoundCompleteIconPath;
var(QuestSound) SoundCue QuestSoundComplete                                                 <DisplayName="QuestSoundComplete" | ExportExcel="Client">;
var transient init string QuestSoundCompletePath;

//
// emergency quest
//
var(Emergency) int EmergencyRate                                                            <DisplayName="EmergencyRate" | ExportExcel="Both" | ClampMin=0 | ClampMax=100>;
var(Emergency) int EmergencyCD                                                              <DisplayName="EmergencyCD" | ExportExcel="Both">;
var(Emergency) EEmergencyTipsType EmergencyTipsType		                                    <DisplayName="EmergencyTipsType" | ExportExcel="Client">;

//
// accept quest settings
//
var(AcceptSetting) EAcceptQuestType StartType                                               <DisplayName="StartType" | ExportExcel="Client">;
var(AcceptSetting) NPCTemplate StartNPCNew		                                            <DisplayName="StartNPC" | ExportExcel="Both">;
var int StartNPCNewId;
var(AcceptSetting) int StartNPCMap                                                          <DisplayName="StartNPCMap" | ExportExcel="Client">;
var(AcceptSetting) bool StartNpcFly				                                            <DisplayName="StartNpcFly" | ExportExcel="Client">;
var bool StartZone				                                                            <ExportExcel="Server">;
var(AcceptSetting) editconst Item StartItem		                                            <DisplayName="StartItem" | ExportExcel="Both">;
var(AcceptSetting) int StartItemID                                                          <DisplayName="StartItemID">;
var(AcceptSetting) bool StartWithDialog                                                     <DisplayName="StartWithDialog" | ExportExcel="Client">;
var(AcceptSetting) bool StartOverWithDialog                                                 <DisplayName="StartOverWithDialog" | ExportExcel="Client">;
var(AcceptSetting) NPCTemplate AutoStartDialogHeadIcon		                                <DisplayName="AutoStartDialogHeadIcon" | ExportExcel="Client">;
var int AutoStartDialogHeadIconId;

//
// submit quest settings
//
var(SubmitSetting) ESubmitQuestType FinishType                                              <DisplayName="FinishType" | ExportExcel="Client">;
var(SubmitSetting) NPCTemplate FinishNpcNew		                                            <DisplayName="FinishNpc" | ExportExcel="Both">;
var int FinishNpcNewId;
var(SubmitSetting) editconst Item FinishItem		                                        <DisplayName="FinishItem" | ExportExcel="Both">;
var(SubmitSetting) int FinishItemID				                                            <DisplayName="FinishItemID">;
var(SubmitSetting) bool FinishWithDialog                                                    <DisplayName="FinishWithDialog" | ExportExcel="Client">;
var(SubmitSetting) bool FinishOverWithDialog                                                <DisplayName="FinishOverWithDialog" | ExportExcel="Client">;
var(SubmitSetting) NPCTemplate AutoFinishDialogHeadIcon                                     <DisplayName="AutoFinishDialogHeadIcon" | ExportExcel="Client">;
var int AutoFinishDialogHeadIconId;
var(SubmitSetting) int AutoStartNextID                                                      <DisplayName="AutoStartNextID" | ExportExcel="Client">;

//
// accept/turn-in quest check conditions
//
var(AcceptCondition) int MaxCompleteCount				                                    <DisplayName="MaxCompleteCount" | ExportExcel="Both">;
var(AcceptCondition) EPlayerClass ClassLimit			                                    <DisplayName="ClassLimit" | ExportExcel="Both">;
var(AcceptCondition) EGenderType SexLimit				                                    <DisplayName="SexLimit" | ExportExcel="Both">;
var(AcceptCondition) EQUEST_RELATION_TYPE RelationLimit                                     <DisplayName="RelationLimit" | ExportExcel="Both">;
var(AcceptCondition) Quest PreQuestTempID1		                                            <DisplayName="PreQuestTempID1" | ExportExcel="Both">;
var int PreQuestId1;
var(AcceptCondition) Quest PreQuestTempID2		                                            <DisplayName="PreQuestTempID2" | ExportExcel="Both">;
var int PreQuestId2;
var(AcceptCondition) Buff BuffLimit				                                            <DisplayName="BuffLimit" | ExportExcel="Both">;
var int BuffLimitId;
var(AcceptCondition) int BuffLevelLimit			                                            <DisplayName="BuffLevelLimit" | ExportExcel="Both">;
var(AcceptCondition) int MaxLevelLimit			                                            <DisplayName="MaxLevelLimit" | ExportExcel="Both">;
var(AcceptCondition) int MinLevelLimit			                                            <DisplayName="MinLevelLimit" | ExportExcel="Both">;
var(AcceptCondition) int ReputationForceID		                                            <DisplayName="ReputationForceID" | ExportExcel="Both">;
var(AcceptCondition) int ReputationMaxLimit		                                            <DisplayName="ReputationMaxLimit" | ExportExcel="Both">;
var(AcceptCondition) int ReputationMinLimit		                                            <DisplayName="ReputationMinLimit" | ExportExcel="Both">;
var(AcceptCondition) EMONEY_TYPE EMoneyTypeLimit	                                        <DisplayName="MoneyTypeLimit">;
var int MoneyTypeLimit                                                                      <ExportExcel="Both">;
var(AcceptCondition) int MoneyAmountLimit		                                            <DisplayName="MoneyAmountLimit" | ExportExcel="Both">;
var(AcceptCondition) int YearLimit				                                            <DisplayName="YearLimit" | ExportExcel="Both">;
var(AcceptCondition) int MonthLimit				                                            <DisplayName="MonthLimit" | ExportExcel="Both">;
//var(AcceptCondition) int WeekdayLimit                                                     <DisplayName="WeekdayLimit">;
//var(AcceptCondition) int DateLimit                                                        <DisplayName="DateLimit">;
//var(AcceptCondition) int Duration                                                         <DisplayName="Duration">;
var(AcceptCondition) int StartWeekday			                                            <DisplayName="StartWeekday" | ExportExcel="Both">;
var(AcceptCondition) int WeekdayDuration		                                            <DisplayName="WeekdayDuration" | ExportExcel="Both">;
var(AcceptCondition) int StartMonthday			                                            <DisplayName="StartMonthday" | ExportExcel="Both">;
var(AcceptCondition) int MonthDayDuration		                                            <DisplayName="MonthDayDuration" | ExportExcel="Both">;
var(AcceptCondition) QuestResetTime AcceptTimeStart                                         <DisplayName="AcceptTimeStart">;
var(AcceptCondition) QuestResetTime AcceptTimeEnd	                                        <DisplayName="AcceptTimeEnd">;
var int nAcceptTimeStart                                                                    <ExportExcel="Both">;
var int nAcceptTimeEnd                                                                      <ExportExcel="Both">;
var(AcceptCondition) int AcceptStartHour			                                        <DisplayName="AcceptStartHour" | ExportExcel="Both">;
var(AcceptCondition) int AcceptStartMinute			                                        <DisplayName="AcceptStartMinute" | ExportExcel="Both">;
var(AcceptCondition) int AcceptEndHour				                                        <DisplayName="AcceptEndHour" | ExportExcel="Both">;
var(AcceptCondition) int AcceptEndMinute			                                        <DisplayName="AcceptEndMinute" | ExportExcel="Both">;
var(AcceptCondition) editfixedsize array<QuestItem> ItemLimit	                            <DisplayName="ItemLimit" | ExportExcel="Both">;
var(AcceptCondition) editfixedsize array<QuestCheckVar> VarLimit	                        <DisplayName="VarLimit" | ExportExcel="Both">;
var(AcceptCondition) init string BeforeAcceptScriptName		                                <DisplayName="BeforeAcceptScriptName" | ExportExcel="Server">;

//
// accept quest rewards
//
var(AcceptReward) EMONEY_TYPE EOfferCurrencyType						                    <DisplayName="OfferCurrencyType">;
var int OfferCurrencyType                                                                   <ExportExcel="Both">;
var(AcceptReward) int OfferCurrencyAmount							                        <DisplayName="OfferCurrencyAmount" | ExportExcel="Both">;
var(AcceptReward) int OfferBuff									                            <DisplayName="OfferBuff" | ExportExcel="Both">;
var(AcceptReward) int OfferBuffLevel								                        <DisplayName="OfferBuffLevel" | ExportExcel="Both">;
var(AcceptReward) editfixedsize array<QuestItem> OfferItem	                                <DisplayName="OfferItem" | ExportExcel="Both">;
var(AcceptReward) init string AfterAcceptScriptName		                                    <DisplayName="AfterAcceptScriptName" | ExportExcel="Server">;

//
// quest completion conditions
//
var(CompleteCondition) int CheckBuff										                <DisplayName="CheckBuff" | ExportExcel="Both">;
var(CompleteCondition) int CheckBuffLevel									                <DisplayName="CheckBuffLevel" | ExportExcel="Both">;
var(CompleteCondition) editfixedsize array<QuestCheckKillNpc> CheckKillNpc	                <DisplayName="CheckKillNpc" | ExportExcel="Both">;
var(CompleteCondition) editfixedsize array<QuestCheckItem> CheckItem		                <DisplayName="CheckItem" | ExportExcel="Both">;
var(CompleteCondition) int TalkNpcID										                <DisplayName="TalkNpcID" | ExportExcel="Both">;
var(CompleteCondition) int TalkNpcMap										                <DisplayName="TalkNpcMap" | ExportExcel="Client">;
var(CompleteCondition) bool TalkNpcFly				                                        <DisplayName="TalkNpcFly" | ExportExcel="Client">;
var(CompleteCondition) int TalkVarID										                <DisplayName="TalkVarID" | ExportExcel="Both" | ClampMin=-1 | ClampMax=3>;
var(CompleteCondition) int ExplorVarID										                <DisplayName="ExplorVarID" | ExportExcel="Both" | ClampMin=-1 | ClampMax=3>;
var(CompleteCondition) int CheckLevel										                <DisplayName="CheckLevel" | ExportExcel="Both">;
var(CompleteCondition) editfixedsize array<QuestCheckVar> CheckVar			                <DisplayName="CheckVar" | ExportExcel="Both">;
//var(CompleteCondition) int CheckVarType1									                <DisplayName="CheckVarType1" | ExportExcel="79">;
//var(CompleteCondition) int CheckVarType2									                <DisplayName="CheckVarType2" | ExportExcel="80">;
var(CompleteCondition) init string BeforeCompleteScriptName		                            <DisplayName="BeforeCompleteScriptName" | ExportExcel="Server">;
var(CompleteCondition) int CheckReputationForceID		                                    <DisplayName="CheckReputationForceID" | ExportExcel="Both">;
var(CompleteCondition) int CheckReputation		                                            <DisplayName="CheckReputation" | ExportExcel="Both">;
var(CompleteCondition) editfixedsize array<QuestCheckKillDoodad> CheckKillDoodad            <DisplayName="CheckKillDoodad" | ExportExcel="Both">;
var(CompleteCondition) editfixedsize array<QuestCheckKillPlayer> CheckKillPlayer            <DisplayName="CheckKillPlayer" | ExportExcel="Both">;
var(CompleteCondition) editfixedsize array<QuestCheckClassVar> CheckClassVar                <DisplayName="CheckClassVar" | ExportExcel="Client">;

//
// quest completion rewards
//
var(CompleteReward) EMONEY_TYPE EAwardCurrencyType										    <DisplayName="AwardCurrencyType">;
var int AwardCurrencyType                                                                   <ExportExcel="Both">;
var(CompleteReward) int AwardCurrencyAmount												    <DisplayName="AwardCurrencyAmount" | ExportExcel="Both">;
var(CompleteReward) EMONEY_TYPE EAwardCurrencyTypeEx										<DisplayName="AwardCurrencyTypeEx">;
var int AwardCurrencyTypeEx                                                                 <ExportExcel="Both">;
var(CompleteReward) int AwardCurrencyAmountEx											    <DisplayName="AwardCurrencyAmountEx" | ExportExcel="Both">;
var(CompleteReward) int AwardExp														    <DisplayName="AwardExp" | ExportExcel="Both">;
var(CompleteReward) int AwardBuff														    <DisplayName="AwardBuff" | ExportExcel="Both">;
var(CompleteReward) int AwardBuffLevel													    <DisplayName="AwardBuffLevel" | ExportExcel="Both">;
var(CompleteReward) int AwardReputationType												    <DisplayName="AwardReputationType" | ExportExcel="Both">;
var(CompleteReward) int AwardReputation													    <DisplayName="AwardReputation" | ExportExcel="Both">;
var(CompleteReward) int AwardTitle	    												    <DisplayName="AwardTitle" | ExportExcel="Both">;
var(CompleteReward) int AwardActivity                                                       <DisplayName="AwardActivity" | ExportExcel="Both">;
var(CompleteReward) editfixedsize array<QuestAwardItem> AwardItem						    <DisplayName="AwardItem" | ExportExcel="Both">;
var(CompleteReward) editfixedsize array<QuestSelectableAwardItem> SelectableAwardItem	    <DisplayName="SelectableAwardItem" | ExportExcel="Both">;
var(CompleteReward) init string AfterCompleteScriptName		                                <DisplayName="AfterCompleteScriptName" | ExportExcel="Server">;

//
//actions executed on quest abandon
//
var(Abandon) init string AbandonScriptName		                                            <DisplayName="AbandonScriptName" | ExportExcel="Server">;

event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues)
{
}

event ScriptDataToExcel(out array<string> Row)
{
}

native function ImportFromExcel(array<string> values);
event ScriptImportFromExcel(array<string> Values)
{
// 	local string val;
// 	val = values[0];
// 	QuestTempID = int(val);
}

native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function bool IsNeedArchetype();
native function InitDataToExcel();
native function int GetTemplateID() const;

native final function bool IsTimeLimit() const;

DefaultProperties
{
//	CanAccept=true
//	bQuestFollow=false
// 	QuestState=EQuestState_None

	ClassLimit=EPCT_None

	ItemLimit(0)=()
	ItemLimit(1)=()
	ItemLimit(2)=()
	ItemLimit(3)=()

	VarLimit(0)=()
	VarLimit(1)=()

	OfferItem(0)=()
	//OfferItem(1)=()
	//OfferItem(2)=()
	//OfferItem(3)=()

	CheckKillNpc(0)=()
	CheckKillNpc(1)=()
	CheckKillNpc(2)=()
	CheckKillNpc(3)=()
	CheckItem(0)=()
	CheckItem(1)=()
	CheckItem(2)=()
	CheckItem(3)=()
	CheckVar(0)=()
	CheckVar(1)=()
	CheckVar(2)=()
	CheckVar(3)=()

	CheckKillDoodad(0)=()
	CheckKillDoodad(1)=()
	CheckKillDoodad(2)=()
	CheckKillDoodad(3)=()
	
	CheckKillPlayer(0)=()
	CheckKillPlayer(1)=()

	CheckClassVar(0)=()
	CheckClassVar(1)=()
	CheckClassVar(2)=()
	CheckClassVar(3)=()
	CheckClassVar(4)=()

	AwardItem(0)=()
	AwardItem(1)=()
	AwardItem(2)=()
	AwardItem(3)=()
	AwardItem(4)=()
	SelectableAwardItem(0)=()
	SelectableAwardItem(1)=()
	SelectableAwardItem(2)=()
	SelectableAwardItem(3)=()
	SelectableAwardItem(4)=()
	SelectableAwardItem(5)=()
	SelectableAwardItem(6)=()
	SelectableAwardItem(7)=()
	SelectableAwardItem(8)=()
	SelectableAwardItem(9)=()
	SelectableAwardItem(10)=()
	SelectableAwardItem(11)=()
	SelectableAwardItem(12)=()
	SelectableAwardItem(13)=()
	SelectableAwardItem(14)=()
	SelectableAwardItem(15)=()
	SelectableAwardItem(16)=()
	SelectableAwardItem(17)=()
	SelectableAwardItem(18)=()
	SelectableAwardItem(19)=()

	//MaleAwardList(0)=()
	//MaleAwardList(1)=()
	//MaleAwardList(2)=()
	//MaleAwardList(3)=()
	//MaleAwardList(4)=()
	//MaleAwardList(5)=()

	//FemaleAwardList(0)=()
	//FemaleAwardList(1)=()
	//FemaleAwardList(2)=()
	//FemaleAwardList(3)=()
	//FemaleAwardList(4)=()
	//FemaleAwardList(5)=()

	//AwardList(0)=()
	//AwardList(1)=()
	//AwardList(2)=()
	//AwardList(3)=()
	//AwardList(4)=()
	//AwardList(5)=()
	nResetTime=0;
	nAcceptTimeStart=0;
	nAcceptTimeEnd=0;


	//KillMonsterList(0)=()
	//KillMonsterList(1)=()
	//KillMonsterList(2)=()
	//KillMonsterList(3)=()	

// 	AcceptStartHour=-1
// 	AcceptEndHour=-1
// 	AcceptStartMinute=-1
// 	AcceptEndMinute=-1

	MonthLimit=-1
	//WeekdayLimit=-1

	StartWeekday=-1
	TalkVarID=-1	
	ExplorVarID=-1
	
	Destinations(0)=()
	Destinations(1)=()
	Destinations(2)=()
	Destinations(3)=()
	Destinations(4)=()
	Destinations(5)=()
	Destinations(6)=()
	Destinations(7)=()

	QuestPageSoundAccept(0)=()
	QuestPageSoundAccept(1)=()
	QuestPageSoundAccept(2)=()
	QuestPageSoundAccept(3)=()
	QuestPageSoundAccept(4)=()
	QuestPageSoundAccept(5)=()
	QuestPageSoundAccept(6)=()
	QuestPageSoundAccept(7)=()

	QuestPageSoundSubmit(0)=()
	QuestPageSoundSubmit(1)=()
	QuestPageSoundSubmit(2)=()
	QuestPageSoundSubmit(3)=()
	QuestPageSoundSubmit(4)=()
	QuestPageSoundSubmit(5)=()
	QuestPageSoundSubmit(6)=()
	QuestPageSoundSubmit(7)=()

	ReputationForceID=-1		                                            
	ReputationMaxLimit=0	                                            
	ReputationMinLimit=0

	CheckReputationForceID=-1	                                    
	CheckReputation=0

	AwardReputationType	=-1											    
	AwardReputation=0	
	
	StartZone=false
}
