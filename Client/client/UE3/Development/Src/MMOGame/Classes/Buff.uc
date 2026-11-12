class Buff extends Object implements(IExcelable,IReferenceID)
	hidecategories(Object)
	native;

struct native BuffProcData
{
	var() editinline array<IMMOStatusProc> Proc;
};

// client property
var(client) string BuffDescription									<DisplayName="BuffDescription">;
var(client) Texture2D     DisplayIcon								<DisplayName="DisplayIcon">;
var(client) bool      bShowIcon										<DisplayName="ShowIcon">;
var(client) EBuffType bBuffType										<DisplayName="BuffType">;
var(client) int       StartShowCount								<DisplayName="StartShowCount">;
var(client) bool      bShowTimeUpTips								<DisplayName="ShowTimeUpTips">;
var(client) bool      bShowStackCount								<DisplayName="ShowStackCount">;
var(client) float     LeftTimeToShow								<DisplayName="LeftTimeToShow">; 
// effect list on buffed target, non-stackable effect list
var(client) editinline array<AbilityComponent> SingleEffectList		<DisplayName="SingleEffectList">;
// effect list that must match the BUFF stack count
var(client) editinline array<AbilityComponent> OverlapEffectList	<DisplayName="OverlapEffectList">;
// effect list on buffed target, non-stackable effect list
var(client) array<ComponentNode> SingleComponentList				<DisplayName="SingleComponentList">;
// effect list that must match the BUFF stack count
var(client) array<ComponentNode> OverlapComponentList				<DisplayName="OverlapComponentList">;

var(client) editinline array<AbilityComponent> BuffEndComponentList;
var(client) EElementaryType   OverriddenElementaryType;

//QTE
var(QTE) bool   bQTEBuff						<DisplayName="QTEBuff">;
var(QTE) int    QTESpellId						<DisplayName="QTESpellId">;

var() int TemplateID							<ExportExcel="Both"|DisplayName="TemplateID">;
var() name BuffName								<ExportExcel="Both"|DisplayName="BuffName">;
var() EAURA_CATEGORY Category					<ExportExcel="Both"|DisplayName="Category">;
var() EAURA_EFFECT_TYPE Type					<ExportExcel="Both"|DisplayName="Type">;
var() int Group								    <ExportExcel="Both"|DisplayName="Group">;
var() float Duration							<ExportExcel="Both"|Convert="1000"|DisplayName="Duration">;
var() float Delay								<ExportExcel="Both"|Convert="1000"|DisplayName="Delay">;
var() float Interval							<ExportExcel="Both"|Convert="1000"|DisplayName="Interval">;
var() EAURA_STACK_TYPE StackSameIdSameCaster	<ExportExcel="Server"|DisplayName="StackSameIdSameCaster">;
var() EAURA_STACK_TYPE StackSameTypeSameCaster	<ExportExcel="Server"|DisplayName="StackSameTypeSameCaster">;
var() EAURA_STACK_TYPE StackSameGroupSameCaster	<ExportExcel="Server"|DisplayName="StackSameGroupSameCaster">;
var() EAURA_STACK_TYPE StackSameIdDiffCaster	<ExportExcel="Server"|DisplayName="StackSameIdDiffCaster">;
var() EAURA_STACK_TYPE StackSameTypeDiffCaster	<ExportExcel="Server"|DisplayName="StackSameTypeDiffCaster">;
var() EAURA_STACK_TYPE StackSameGroupDiffCaster	<ExportExcel="Server"|DisplayName="StackSameGroupDiffCaster">;
var() bool TimeCover							<ExportExcel="Server"|DisplayName="TimeCover">;
var() bool StackResetTime               		<ExportExcel="Server"|DisplayName="StackResetTime">;
var() int InitStackCount						<ExportExcel="Both"|DisplayName="InitStackCount">;
var() int MaxStackCount							<ExportExcel="Both"|DisplayName="MaxStackCount">;
var() int StrengthLevel							<ExportExcel="Server"|DisplayName="StrengthLevel">;
var() bool CanDispel							<ExportExcel="Server"|DisplayName="CanDispel">;
var() int DispelCount							<ExportExcel="Server"|DisplayName="DispelCount">;
var() bool TrigOnRemoveWhenDispel				<ExportExcel="Server"|DisplayName="TrigOnRemoveWhenDispel">;
var() bool TrigOnRemoveWhenTimeup				<ExportExcel="Server"|DisplayName="TrigOnRemoveWhenTimeup">;
var() ESPELL_ATTACK_TYPE AttackType				<ExportExcel="Both"|DisplayName="AttackType">;
var() ESPELL_JUDGE_TYPE JudgeType				<ExportExcel="Both"|DisplayName="JudgeType">;
var() ESPELL_DAMAGE_TYPE ElementaryType			<ExportExcel="Both"|DisplayName="ElementaryType">;
var() EMODIFIER_X_TYPE EffectPctX	    		<ExportExcel="Server"|DisplayName="EffectPctX">;
var() int EffectPct								<ExportExcel="Both"|DisplayName="EffectPct">;
var() EMODIFIER_X_TYPE EffectValueX		    	<ExportExcel="Server"|DisplayName="EffectValueX">;
var() int EffectValue							<ExportExcel="Both"|DisplayName="EffectValue">;
var() int ThreatPct								<ExportExcel="Server"|DisplayName="ThreatPct">;
var() int ThreatValue							<ExportExcel="Server"|DisplayName="ThreatValue">;
var() int ThreatMax								<ExportExcel="Server"|DisplayName="ThreatMax">;
var() bool RemoveWhenDead						<ExportExcel="Server"|DisplayName="RemoveWhenDead">;
var() bool SaveToDB								<ExportExcel="Server"|DisplayName="SaveToDB">;
var() bool OfflineTimeValid						<ExportExcel="Server"|DisplayName="OfflineTimeValid">;
var() bool HiddenToClient						<ExportExcel="Server"|DisplayName="HiddenToClient">;
var() bool HiddenToSelf							<ExportExcel="Server"|DisplayName="HiddenToSelf">;
var() bool ControlDecay 						<ExportExcel="Server"|DisplayName="ControlDecay">;
var() bool GenerateObstacle						<ExportExcel="Server"|DisplayName="GenerateObstacle">;
var() float EffectTimeMin						<ExportExcel="Server"|Convert="1000"|DisplayName="EffectTimeMin">;
var() bool Eternal						        <ExportExcel="Both"|DisplayName="Eternal">;
var() bool IgnoreDefense				        <ExportExcel="Server"|DisplayName="IgnoreDefense">;
var() EAURA_RESIST_TYPE ResistType		        <ExportExcel="Both"|DisplayName="ResistType">;
var() bool CanCancel    				        <ExportExcel="Both"|DisplayName="CanCancel">;
var() EAURA_DISPLAY_GROUP DisplayGroup	        <ExportExcel="Both"|DisplayName="DisplayGroup">;
var() int DisplayValue              	        <ExportExcel="Both"|DisplayName="DisplayValue">;
var() bool EffectDecay              	        <ExportExcel="Server"|DisplayName="EffectDecay">;

var() SeqAct_Modifier OnAppend					<DisplayName="OnAppend">;
var() SeqAct_Modifier BeforeStack				<DisplayName="BeforeStack">;
var() SeqAct_Modifier AfterStack				<DisplayName="AfterStack">;
var() SeqAct_Modifier OnTimeup					<DisplayName="OnTimeup">;
var() SeqAct_Modifier OnDispel					<DisplayName="OnDispel">;
var() SeqAct_Modifier OnRemove					<DisplayName="OnRemove">;
var() SeqAct_Modifier OnEx1						<DisplayName="OnEx1">;
var() SeqAct_Modifier OnEx2						<DisplayName="OnEx2">;

var	int OnAppendID								<ExportExcel="Server">;
var	int BeforeStackID							<ExportExcel="Server">;
var	int AfterStackID							<ExportExcel="Server">;
var	int OnTimeupID								<ExportExcel="Server">;
var	int OnDispelID								<ExportExcel="Server">;
var	int OnRemoveID								<ExportExcel="Server">;
var() float ExDelay1							<ExportExcel="Server"|Convert="1000"|DisplayName="ExDelay1">;
var	int OnEx1ID									<ExportExcel="Server">;
var() float ExDelay2							<ExportExcel="Server"|Convert="1000"|DisplayName="ExDelay2">;
var	int OnEx2ID									<ExportExcel="Server">;

//MMOStatusProc
var(MMOStatusProc) editinline array<BuffProcData> StartProcList		<DisplayName="StartProcList">;
var(MMOStatusProc) editinline array<BuffProcData> EndProcList		<DisplayName="EndProcList">;

event DoStart(MMOActor Actor)
{
	local int i, j;
	
	for(i = StartProcList.Length - 1; i >= 0; --i)
	{
		for(j = 0; j < StartProcList[i].Proc.Length; ++j)
		{
			`log("------------------------------------------------------------------DoStart index="$i@"proc="$j);
			StartProcList[i].Proc[j].OnExecute(actor);
		}
	}
}
event DoEnd(MMOActor Actor)
{
	local int i, j;
	for(i = EndProcList.Length - 1; i >= 0; --i)
	{
		for(j = 0; j < EndProcList[i].Proc.Length; ++j)
		{
			EndProcList[i].Proc[j].OnExecute(actor);
		}
	}
}

event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues)
{

}

event ScriptDataToExcel(out array<string> Row)
{
}

event ScriptImportFromExcel(array<string> Values)
{
}

native function ImportFromExcel(array<string> values);
native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function bool IsNeedArchetype();
native function InitDataToExcel();
native function int GetTemplateID() const;
native function string GetBuffIconName();

cpptext
{	
	virtual FString GetDetailedDescription( INT InIndex )
	{
		switch( InIndex )
		{
		case 0:
			return FString::Printf(TEXT("%d"), GetTemplateID());
			break;
		}

		return TEXT("");
	}
}

DefaultProperties
{
	bShowTimeUpTips = true;
	bShowStackCount = true;
	bBuffType       = EBuff_Normal;
	OverriddenElementaryType=EETT_Elementary0;
	EffectDecay		= true;
}
