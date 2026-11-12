//=============================
// MMODoodad 
//simple interactive objects in the scene, similar to DropItem
//=============================

class Doodad extends Object implements(IExcelable, IReferenceID, IMMOActor)
	dependson(TypeDefine)
	dependson(AvatarData)
	dependson(ServerEnum)
	dependson(NPC)
	hidecategories(Object,Debug,Advanced,Physics,CollisionChanged,Attachment,DisplayDebug,Display,Movement,Collision)
	native;

//InstanceID 
//reserved, actually uses the Id of DropItemActor
var int DoodadID; 
var SkeletalMeshComponent SkelComponent;
//TemplateID
//var(Item) int DoodadTemplateID; 
var(DoodadCollision) bool EnableCollision <DisplayName="Enable Collision" | ExportExcel="Client">;
var() Texture2D DisplayIcon				                        <DisplayName="DisplayIcon" | ExportExcel="Client">;
var() name DoodadName				                            <DisplayName="DoodadName" | ExportExcel="Client">;
var() name AnimIdle				                                <DisplayName="AnimIdle" | ExportExcel="Client">;
var() name AnimOpen				                                <DisplayName="AnimOpen" | ExportExcel="Client">;
var() name AnimClose				                            <DisplayName="AnimClose" | ExportExcel="Client">;
var() name AnimDie				                                <DisplayName="AnimDie" | ExportExcel="Client">;

var() init string Description				                    <DisplayName="Description" | ExportExcel="Client">;
var() ParticleSystem ActiveEffectTemplate                       <ExportExcel="Client">;
var() float ActiveEffectScale                                   <ExportExcel="Client">;
//var() SkeletalMesh DoodadMesh				                    <DisplayName="DoodadMesh">;
var() bool IsDelay					                            <DisplayName="IsDelay" | ExportExcel="Client">;
var() float Delay					                            <DisplayName="Delay" | ExportExcel="Client">;
var bool IsDestroy;
var(AvatarData) AvatarData DoodadAvatar			                <DisplayName="DoodadAvatar" | ExportExcel="Client">;

var() float DurationBeforeDestroy                               <ExportExcel="Client">;
var() float DestructForceScale                                  <ExportExcel="Client">;
//Avatar Data

var(Actions) editinline array<AbilityComponent> DeathEffectList;
var(Actions) editinline array<AbilityComponent> HitEffectList;
var(Actions) editinline array<AbilityComponent> OnSpawnEffectList;

var(MMOServer) int ID				                            <DisplayName="ID" | ExportExcel="Both">;
var(MMOServer) int Level <ExportExcel="Both">;
var(MMOServer) string DisplayName		                        <DisplayName="Name" | ExportExcel="Client">;
var(MMOServer) EDOODAD_TYPE DoodadType		                    <DisplayName="DoodadType" | ExportExcel="Both">;
var(MMOServer) EDOODAD_SUB_TYPE	DoodadSubType	                <DisplayName="DoodadSubType" | ExportExcel="Both">;
var(MMOServer) int OperateDuration                              <DisplayName="OperateDuration" | ExportExcel="Both">;
var(MMOServer) int MinOperateCount                              <DisplayName="MinOperateCount" | ExportExcel="Both">;
var(MMOServer) int MaxOperateCount                              <DisplayName="MaxOperateCount" | ExportExcel="Both">;
var(MMOServer) string ScriptName		                        <DisplayName="ScriptName" | ExportExcel="Server">;

var(MMOServer) int RequireItemID                                <ExportExcel="Server">;
var(MMOServer) bool CostItem                                    <ExportExcel="Server">;
//var(MMOServer) Quest RequireQuestID                           <ExportExcel="23">;
var(MMOServer) init string RequireVarID                         <ExportExcel="Both">;
var(MMOServer) init string RequireVarValue                      <ExportExcel="Both">;
var(MMOServer) NPCTemplate SpawnNpcID                           <ExportExcel="Server">;
var(MMOServer) int SpawnNpcChance                               <ExportExcel="Server" | ClampMin=0 | ClampMax=10000>;
var(MMOServer) int AuraID                                       <ExportExcel="Server">;
var(MMOServer) float LifeTime                                   <ExportExcel="Server"|Convert="1000">;
var(MMOServer) int Radius                                       <ExportExcel="Both">;
var(MMOServer) init string   BodyType                           <ExportExcel="Both">;


// animation group the character plays when this object is opened, references the animation group Doodad_RoleAnimGroup.upk, which must be loaded first
//var() init string	RoleAnimGroup                               <DisplayName="Doodad RoleAnimGroup" | DynamicList="Doodad_RoleAnimGroup" | ExportExcel="Client">;

var() name BeginAnimOpener;
var() name LoopAnimOpener;
var() name EndAnimOpener;

var() bool bNeedPlayDoodadLoopSound;

// whether to show the name on mouse hover
var() bool DoododNameDisplay                                    <ExportExcel="Client">;

// cursor type on mouse hover and the effect released after opening
var() EMMOCursorType DoododCursorType                           <ExportExcel="Client">;

// effect played after this object is gathered
var() ParticleSystem DoodadEndEffect                            <ExportExcel="Client">;

var(UI) edittextbox init string TipsText                        <ExportExcel="Client">;
var(UI) Vector TipsPosOffset;
// sound type when this object is destroyed (DoodadSndList in gameplaydata must be set to match)
//var(Sound) EItemMaterialType DoodadSoundType;

native function EActorClassType GetActorType() const;
event ScriptExportExcelHeaders(out array<string> Header, out array<string> DefaultValues){}
event ScriptDataToExcel(out array<string> Row){}

native function ExportExcelHeaders(out array<string> Header, out array<string> DefaultValues);
native function DataToExcel(out array<string> Row);
native function ImportFromExcel(array<string> values);
event ScriptImportFromExcel(array<string> Values){}
native function InitDataToExcel();

native function int GetTemplateID() const;

cpptext
{	
	virtual FString GetDetailedDescription( INT InIndex )
	{
		switch( InIndex )
		{
		case 0:
			return FString::Printf(TEXT("%d"), ID);
			break;

		case 1:
			return FString::Printf(TEXT("%s"), *DoodadName.ToString());
			break;

		case 2:
			return FString::Printf(TEXT("%s"), *Description);
			break;
		}

		return TEXT("");
	}
	//virtual void GetDynamicListValues(const DynamicListInfo& ListInfo, TArray<FString>& Values);
}


defaultproperties
{
	AnimIdle="Idle"
	AnimOpen="Open"
	AnimClose="Close"
	AnimDie="Death"
	IsDelay=false
	Delay=1.0
	IsDestroy=false
	ActiveEffectScale=1.0
	BeginAnimOpener=OpenDoodadStart
	LoopAnimOpener=OpenDoodadLoop
	EndAnimOpener=OpenDoodadEnd
	bNeedPlayDoodadLoopSound=false
	EnableCollision=true
	/*
	Begin Object Class=StaticMeshComponent Name=StaticMeshComponent0
		StaticMesh=StaticMesh'Pickups.jump_pad.S_Pickups_Jump_Pad'
		CollideActors=false
		CastShadow=FALSE
		bCastDynamicShadow=FALSE
		bAcceptsLights=TRUE
		bForceDirectLightMap=TRUE
		LightingChannels=(BSP=TRUE,Dynamic=FALSE,Static=TRUE,CompositeDynamic=TRUE)
		//LightEnvironment=JumpPadLightEnvironment
	End Object
 	Components.Add(StaticMeshComponent0)
	*/
	/*
	Begin Object Class=UTParticleSystemComponent Name=ParticleSystemComponent1
		Translation=(X=0.0,Y=0.0,Z=-35.0)
		Template=particleSystem'Pickups.jump_pad.P_Pickups_Jump_Pad_FX'
		bAutoActivate=true
		SecondsBeforeInactive=1.0f
	End Object
	Components.Add(ParticleSystemComponent1)
	*/
    DoododNameDisplay = true;
    DoododCursorType=EMC_Pick;

	DurationBeforeDestroy=5.0f
	DestructForceScale=1.0f

	TipsPosOffset=(x=0,y=0,z=100)
	Begin Object Class=AbilityComp_MaterialEffect Name=DefaultHitMatEffect
		EffectType=EMET_ChangeMaterialProperty
		EffectTemplateId=2
	End Object
	HitEffectList(0)=DefaultHitMatEffect

	BodyType="E_body_0x0"

}