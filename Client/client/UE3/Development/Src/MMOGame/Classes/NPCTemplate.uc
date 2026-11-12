//
// main NPC base class, handles character animation management
//

class NPCTemplate extends Object implements(IExcelable,IReferenceID)
	config(MMONPC)
	native
	hidecategories(Object,Debug,Advanced,Physics,CollisionChanged,Attachment,DisplayDebug,Display,Movement,Collision)
	;

//---------------------------------------------------------------
// state presentation
//---------------------------------------------------------------

struct native StatusProcData
{
	var() float Rate							                                <DisplayName="Rate">;
	var() editinline array<IMMOStatusProc> Proc	                                <DisplayName="Proc">;
};

struct native ActionProcData
{
	var() int ActionID							                                <DisplayName="ActionID">;
	var() editinline array<IMMOStatusProc> Proc	                                <DisplayName="Proc">;
};

struct native DropGroupSet
{
	// percentage
	var() int Weight			                                                <DisplayName="Weight" | ExportExcel="Client">;
	var() INT DropGroup	                                                        <DisplayName="DropGroupID" | ExportExcel="Client">;
};

struct native ActionNode
{
	var() int ActionId;
	var() editinline array<AbilityComponent> Actions;
};

enum ENPCFunctionType
{
	ENPCFY_EquipEnhance,
	ENPCFY_PVP,
	ENPCFY_Merchant,
	ENPCFY_Warehouse,
	ENPCFY_NormalFun,
	ENPCFY_RecruitNpc,
	ENPCFY_GuildNpc,
	ENPCFY_SalaryNpc,
	ENPCFY_ReputationNpc,
	ENPCFY_ArenaNpc
};

enum EGuildBuildingInteractType
{
	EGBIT_NO_LIMIT                                                              <DisplayName = "EGBIT_NO_LIMIT">,
	EGBIT_ONLY_SELF                                                             <DisplayName = "EGBIT_ONLY_SELF">,
	EGBIT_ONLY_ALLY                                                             <DisplayName = "EGBIT_ONLY_ALLY">,
	EGBIT_ONLY_ENEMY                                                            <DisplayName = "EGBIT_ONLY_ENEMY">,
};

struct native DialogOptionConfig
{
	var() bool Enable                                                           <DisplayName="Enable" | ExportExcel="Both">;
	var() init string ClientDiagText                                            <DisplayName="ClientDiagText" | ExportExcel="Client">;
};

//var(MMOServer) EOBJ_BODY_TYPE BodyType                                        <DisplayName="BodyType" | ExportExcel="Both" | UseString="True">;
var(MMOServer) init string   BodyType                                           <ExportExcel="Both">;
var(MMOServer) int TemplateID			                                        <DisplayName="TemplateID" | ExportExcel="Both">;
var(MMOServer) init string NameID		                                        <DisplayName="TemplateName" | ExportExcel="Both">;
var(MMOServer) int Level <ExportExcel="Both">;

var(MMOServer) int GroupID		                                                <DisplayName="GroupID" | ExportExcel="Both">;
var(MMOServer) private{public} init string DisplayName	                        <DisplayName="DisplayName" | ExportExcel="Both">;
var(MMOServer) ENPC_QUALITY Quality	                                            <DisplayName="Quality" | ExportExcel="Both">;
//var(MMOServer) bool bPetCanAttack                                             <DisplayName="PetCanAttack" | ExportExcel="Both">;

var(MMOServer) int ServerSpawnTime			                                    <DisplayName="ServerSpawnTime" | DisplayName="SpawnTime" | ExportExcel="Server">;
var(MMOServer) int ReviveTime				                                    <DisplayName="ReviveTime" | ExportExcel="Server">;
var(MMOServer) int CorpseTime				                                    <DisplayName="CorpseTime" | ExportExcel="Server">;

var(MMOServer) int RespawnTimeMin				                                <DisplayName="RespawnTimeMin" | ExportExcel="Server">;
var(MMOServer) int RespawnTimeMax				                                <DisplayName="RespawnTimeMax" | ExportExcel="Server">;

var(MMOServer) int AIData				                                        <DisplayName="AIData" | ExportExcel="Server">;
var(MMOServer) editconst const int AIDataID 				                    <DisplayName="AIDataID" | ExportExcel="Server">;

// range is 0-16
var(MMOServer) int ForceID					                                    <DisplayName="ForceID" | ExportExcel="Server">;
var(MMOServer) init string NPCScriptName	                                    <DisplayName="NPCScriptName" | ExportExcel="Server">;

var(MMOServer) int ShopID					                                    <DisplayName="ShopID" | ExportExcel="Both">;

var(MMOServer) bool ImmunePassiveMove		                                    <DisplayName="ImmunePassiveMove" | ExportExcel="Server">;
var(MMOServer) bool Invincible				                                    <DisplayName="Invincible" | ExportExcel="Server">;
var(MMOServer) bool AllwaysRun				                                    <DisplayName="AllwaysRun" | ExportExcel="Server">;
var(MMOServer) bool IgnoreInvisible				                                <DisplayName="IgnoreInvisible" | ExportExcel="Server">;
var(MMOServer) bool CounterInvisible				                            <DisplayName="CounterInvisible" | ExportExcel="Server">;
var(MMOServer) int Pattern														<DisplayName="Pattern" | ExportExcel="Server">;
var(MMOServer) ECHAR_SPECIES_TYPE SpeciesType                                   <DisplayName="SpeciesType" | ExportExcel="Server">;

var(MMOServer) int RivalForceID					                                <DisplayName="RivalForceID" | ExportExcel="Server">;
var(MMOServer) bool ReduceReputation			                                <DisplayName="ReduceReputation" | ExportExcel="Server">;
var(MMOServer) bool Affix				                                        <DisplayName="Affix" | ExportExcel="Server">;
var(MMOServer) bool CanbeThrown			                                        <DisplayName="CanbeThrown" | ExportExcel="Server">;
var(MMOServer) float RotateSpeed                                                <DisplayName="RotateSpeed" | ExportExcel="Server">;
var(MMOServer) int AttackRange                                                  <DisplayName="AttackRange" | ExportExcel="Server">;
var(MMOServer) bool Exclusive                                                   <DisplayName="Exclusive" | ExportExcel="Server">;
var(MMOServer) bool Profit                                                      <ExportExcel="Server">;
var(MMOServer) bool NeverBeHit                                                  <ExportExcel="Both">;
var(MMOServer) int ExpRate                                                      <ExportExcel="Server">;
var(MMOServer) int SoulRate                                                     <ExportExcel="Server">;
var(MMOServer) int DailyExp                                                     <ExportExcel="Server">;
var(MMOServer) bool AuraInvalid                                                 <ExportExcel="Server">;
var(MMOServer) bool AttackRedName                                               <ExportExcel="Server">;
var(MMOServer) int Radius					                                    <DisplayName="Radius" | ExportExcel="Server">;
var(MMOServer) float RadiusScale				                                <DisplayName="RadiusScale" | Convert="10000" | ExportExcel="Server">;
var(MMOServer) bool DieIDMonitor                                                <ExportExcel="Server">;
var(MMOServer) int CorpseDoodadID                                               <ExportExcel="Server">;

// see requirement 11627 for details
var(MMOServer) int BeKilledVarIndex                                             <DisplayName="BeKilledVarIndex" | ExportExcel="Server">;
var(MMOServer) int KillPlayerVarIndex                                           <DisplayName="KillPlayerVarIndex" | ExportExcel="Server">;

var(MMOServer) int BigDropInterval                                              <ExportExcel="Server">;

var(MMOServer) int BlackMarketID					                            <DisplayName = "BlackMarketID" | ExportExcel = "Both">;
var(MMOServer) EGuildBuildingInteractType BuildingInteractType					<DisplayName = "BuildingInteractType" | ExportExcel = "Both">;

struct native SpellEffectGroup
{
	var(SpellEffectGroup) bool bSpellEffectGroupType1 <ExportExcel="0"|DisplayName="SpellEffectGroupType1">;
	var(SpellEffectGroup) bool bSpellEffectGroupType2 <ExportExcel="1"|DisplayName="SpellEffectGroupType2">;
	var(SpellEffectGroup) bool bSpellEffectGroupType3 <ExportExcel="2"|DisplayName="SpellEffectGroupType3">;
	var(SpellEffectGroup) bool bSpellEffectGroupType4 <ExportExcel="3"|DisplayName="SpellEffectGroupType4">;
	var(SpellEffectGroup) bool bSpellEffectGroupType5 <ExportExcel="4"|DisplayName="SpellEffectGroupType5">;
	var(SpellEffectGroup) bool bSpellEffectGroupType6 <ExportExcel="5"|DisplayName="SpellEffectGroupType6">;
	var(SpellEffectGroup) bool bSpellEffectGroupType7 <ExportExcel="6"|DisplayName="SpellEffectGroupType7">;
	var(SpellEffectGroup) bool bSpellEffectGroupType8 <ExportExcel="7"|DisplayName="SpellEffectGroupType8">;
	var(SpellEffectGroup) bool bSpellEffectGroupType9 <ExportExcel="8"|DisplayName="SpellEffectGroupType9">;
	var(SpellEffectGroup) bool bSpellEffectGroupType10 <ExportExcel="9"|DisplayName="SpellEffectGroupType10">;
	var(SpellEffectGroup) bool bSpellEffectGroupType11 <ExportExcel="10"|DisplayName="SpellEffectGroupType11">;
	var(SpellEffectGroup) bool bSpellEffectGroupType12 <ExportExcel="11"|DisplayName="SpellEffectGroupType12">;
	var(SpellEffectGroup) bool bSpellEffectGroupType13 <ExportExcel="12"|DisplayName="SpellEffectGroupType13">;
	var(SpellEffectGroup) bool bSpellEffectGroupType14 <ExportExcel="13"|DisplayName="SpellEffectGroupType14">;
	var(SpellEffectGroup) bool bSpellEffectGroupType15 <ExportExcel="14"|DisplayName="SpellEffectGroupType15">;
	var(SpellEffectGroup) bool bSpellEffectGroupType16 <ExportExcel="15"|DisplayName="SpellEffectGroupType16">;
};

var(MMOServer) SpellEffectGroup SpellEffectGroups <ExportExcel="Server"|Merger="0"|DisplayName="SpellEffectGroup">;

// add by zhangrui05@snda
var(MMOServer) bool StatMoney													<DisplayName = "StatMoney" | ExportExcel = "Server">;
var(MMOServer) int RuneGroup                                                    <ExportExcel="Server">;

var(MMOServer) bool Broadcast                                                   <DisplayName = "Broadcast" | ExportExcel="Server">;
//---------------------------------------------------------------
// Client Attributes
//---------------------------------------------------------------

var(MMOClient) AvatarData AvatarTemplate		                                <DisplayName="AvatarTemplate" | ExportExcel="Client">;
var(MMOClient) bool		  UsePlaneShadow		                                <DisplayName="Generation Plane Shadow" | ExportExcel = "Client">;
var(MMOClient) int		  ClientTemplateID										<DisplayName="ClientTemplateID" | ExportExcel = "Both">;

var(Anim) MMOAnimGroup PawnAnimInfo				                                <DisplayName="PawnAnimInfo">;
var(Anim) float RandomRestAnimCycleTime                                         <DisplayName="RandomRestAnimCycleTime">;
var(Anim) float RandomRestAnimPlayRate                                          <DisplayName="RandomRestAnimPlayRate" | ClampMin=0.0 | ClampMax=1.0>;
var(Anim) float FightRandomRestAnimCycleTime;
var(Anim) float FightRandomRestAnimPlayRate                                     <ClampMin=0.0 | ClampMax=1.0>;

var(MMO_UI) init string DefaultDialogText		                                <DisplayName="DefaultDialogText" | ExportExcel="Client">;
var(MMO_UI) bool DefaultDialogTextWithBubble	                                <DisplayName="DefaultDialogTextWithBubble" | ExportExcel="Client">;
var(MMO_UI) init string IdentityName			                                <DisplayName="IdentityName" | ExportExcel="Client">;
var(MMO_UI) Texture2D IdentityIcon			                                    <DisplayName="IdentityIcon" | ExportExcel="Client">;
var(MMO_UI) int NPCIcon						                                    <DisplayName="NPCIcon" | ExportExcel="Client">;
var(MMO_UI) Texture2D HUDDisplayIcon			                                <DisplayName="HUDDisplayIcon" | ExportExcel="Client">;
var(MMO_UI) string ActorName					                                <DisplayName="ActorName" | ExportExcel="Client">;

var(MMO_UI) bool IsReputationFunc	                                            <DisplayName="IsReputationFunc" | ExportExcel="Client">;
var(MMO_UI) bool IsPVP	                                                        <DisplayName="IsPVP" | ExportExcel="Client">;
var(MMO_UI) bool IsMerchant	                                                    <DisplayName="IsMerchant" | ExportExcel="Client">;
var(MMO_UI) bool IsWarehouse	                                                <DisplayName="IsWarehouse" | ExportExcel="Client">;
var(MMO_UI) bool IsNormalFunction                                               <DisplayName="IsNormalFunction">;
var(MMO_UI) bool IsGuildDungeon                                                 <DisplayName="IsGuildDungeon" | ExportExcel="Client">;
// character type
var(MMO_UI) EActorClassType ActorType                                           <DisplayName="ActorType" | ExportExcel="Client">;
var(MMO_UI) bool FaceToActorWhenInteract                                        <DisplayName="FaceToActorWhenInteract" | ExportExcel="Client">;
var(MMO_UI) bool DisablePick                                                    <DisplayName="DisablePick" | ExportExcel="Client">;
var(MMO_UI) bool HideHUDName                                                    <DisplayName="HideHUDName" | ExportExcel="Client">;

//----------------------------------------------------------------------
// after adding or removing the following fields, the patch must be finalized at the end of void UITextParser::Parse( !!!
//----------------------------------------------------------------------
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_PlayerBank                       <DisplayName="PlayerBank" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_GuildBank                        <DisplayName="GuildBank" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_NPCShop                          <DisplayName="NPCShop" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_RewardQuest                      <DisplayName="RewardQuest" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_CreateGuild                      <DisplayName="CreateGuild" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_GuildSalary                      <DisplayName="GuildSalary" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_Auction                          <DisplayName="Auction" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_WeaponRune                       <DisplayName="WeaponRune" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_GemEmbed                         <DisplayName="GemEmbed" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_LevelUpCurWeapon                 <DisplayName="LevelUpCurWeapon" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_GetBackCurLvUpWeapon             <DisplayName="GetBackCurLvUpWeapon" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_SpecialRepair                    <DisplayName="SpecialRepair" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_Gamble                           <DisplayName="Gamble" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_Exercise                         <DisplayName="ExpExercise" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_GambleBook                       <DisplayName="GambleBook" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_ArenaReward                      <DisplayName="ArenaReward" | ExportExcel="Both">;
var(MMO_UI_Diag) DialogOptionConfig EnableDiag_GamblePotion                     <DisplayName="GamblePotion" | ExportExcel="Both">;

var(MMOStatusProc) editinline array<StatusProcData> SpawnProcList		        <DisplayName="SpawnProcList">;
var(MMOStatusProc) editinline array<StatusProcData> TeleportProcList	        <DisplayName="TeleportProcList">;
var(MMOStatusProc) editinline array<StatusProcData> RebirthProcList		        <DisplayName="RebirthProcList">;
var(MMOStatusProc) editinline array<ActionProcData> ActionProcList		        <DisplayName="ActionProcList">;
var(MMOStatusProc) editinline array<StatusProcData> TickProcList		        <DisplayName="TickProcList">;

var(SoundFX) array<SoundCue> Sound_OpenDiag                                     <DisplayName="Sound_OpenDiag">;
var(SoundFX) array<SoundCue> Sound_CloseDiag                                    <DisplayName="Sound_CloseDiag">;
var(SoundFX) array<SoundCue> Sound_Spawn                                        <DisplayName="Sound_Spawn">;

var(SoundFX) editinline array<AbilityComponent> EnterFightEffects;
var(SoundFX) editinline array<AbilityComponent> ExitFightEffects;

var(Actions) editinline array<AbilityComponent> DeathComponents;
// always play
var(Actions) editinline array<AbilityComponent> DefaultDeathComponents;

var(Actions) editinline array<AbilityComponent> PreSpawnActions;
var(Actions) editinline array<AbilityComponent> SpawnActions;

var(Actions) array<ActionNode> Actions;

var() EBloodEffectType NormalHitBloodEffectType;
var() EBloodEffectType DeadHitBloodEffectType;

var(Wind) float WindStrength;

var(GuildBuilding) SkeletalMeshComponent BuildingMesh;

//-------------------------------------------------------
// export to Excel
//-------------------------------------------------------

native function bool IsNeedArchetype();

event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues){}
event ScriptDataToExcel(out array<string> Row){}

native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function ImportFromExcel(array<string> values);
event ScriptImportFromExcel(array<string> Values){}
native function InitDataToExcel();

native function int GetTemplateID() const;

native final function String GetIconName();

native final function string GetDisplayName() const;

cpptext
{
	virtual FString GetDetailedDescription( INT InIndex )
	{
		switch( InIndex )
		{
		case 0:
			return FString::Printf(TEXT("%d"), TemplateID);
			break;
		case 1:
			return NameID;
			break;
		case 2:
			return DisplayName;
			break;
		}

		return TEXT("");
	}

	#if WITH_EDITOR
	void CheckForErrors();
	#endif

}

DefaultProperties
{	
	TemplateID=-1
	SpeciesType=E_spc_Human
	ActorType=EACT_Monster
	FaceToActorWhenInteract=1
	RandomRestAnimCycleTime=30.f
	RandomRestAnimPlayRate=1.0f
	FightRandomRestAnimCycleTime=3.f
	FightRandomRestAnimPlayRate=1.f
	//Quality=E_nq_Random
	ExpRate=10000
	SoulRate=10000
	DailyExp=1
	WindStrength=1.f
	RivalForceID=-1
	RadiusScale=0.5f
	
	BeKilledVarIndex=-1
	KillPlayerVarIndex=-1
	//BodyType=E_body_1x1
	BodyType="E_body_1x1"
	//bPetCanAttack=true
	BigDropInterval=1000
	StatMoney=false

	UsePlaneShadow=true
	BuildingInteractType=EGBIT_NO_LIMIT

	ClientTemplateID=0

	Broadcast=false
}
