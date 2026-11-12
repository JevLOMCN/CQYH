/**
 *
 * Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
 */

class GlobalMapInfo
	extends Object
	hidecategories(Object)
	native
	config(GlobalMapInfo)
	dependson(MMOMapInfo)
	dependson(TeleportNetNode)
	dependson(NPCNewSpawnGroupEditorActor)
	dependson(MMOVolume_Trigger)
	dependson(ServerEnum);

//enum EServerMapType
//{
//	esmt_None <DisplayName="None">,
//	esmt_Static <DisplayName="Static">,
//    esmt_Dungeon <DisplayName="Dungeon">,
//    esmt_BattleField <DisplayName="BattleField">,
//    esmt_Arena <DisplayName="Arena">,
//    esmt_Line <DisplayName="Line">
//};

struct native WeatherConfig
{
	var() ESCENE_WEATHER_TYPE WeatherType;
	var() float WeatherWeight;
	var() float MinDuration;
	var() float MaxDuration;
	structdefaultproperties
	{
		WeatherType=E_swt_Invalid
	}
};

struct native SceneProgress
{
	var() init String Desc;
	var() int ID;
	var() int TargetValue;
	//var int CurValue;
	structdefaultproperties
	{
		ID=-1
		TargetValue=-1
		//CurValue=0
	} 
};

const MaxSceneMinorTarget = 3;
struct native SceneTarget
{
	var() SceneProgress MainTarget;
	//var() SceneProgress ChildTargets[MaxSceneMinorTarget];
	var() SceneProgress ChildTargets[MaxSceneMinorTarget];
};

enum EENUM_SCENE_VTYPE
{
	ESceneVType_Invalid,
	ESceneVType_Dungeon,
	ESceneVType_FlagBattle,
	ESceneVType_Arena,
};

// server map infos
struct native ServerMapInfos
{
	var() init string ConfigName;
	var() init string MapName;
	var() INT ServerMapID;
	var() Vector2D Map2DExportSize;
	var() EENUM_SCENE_VA ServerMapType;
};

//mining drop data
struct native MineDropInfos
{
	var() int Chance	    <DisplayName="MineDropChance">;
	var() int DropGroup	    <DisplayName="MineDropGroup">;
};

const MaxSceneMineDrop = 16;
const MaxSceneMajorTarget = 8;
struct native ExportMap
{
	var() int ServerID						<DisplayName="ServerID">;
	// scene level
	var() int Level							<DisplayName="Level">;
	
	// map config name used for display, Chinese names are required, so the former config name was moved here
	var() init	string	    MapDisplayName;//	<DisplayName="MapName">;

	// map config name used by the client
	var() init string MapName;//				<DisplayName="ClientMapName">;
	//whether this map shows player names
	var() bool HideRoleName           <DisplayName="HideRoleName" | ExportExcel="Client">;

	// pk mode specified by the map
	var() bool OpenMapPkMode		<DisplayName="OpenMapPkMode" | ExportExcel="Client" >;
	var() EERolePkMode MapPkMode		<DisplayName="MapPkMode" | ExportExcel="Client" >;

	// name used for fast lookup
	var() init string ShortName		<DisplayName="ShortName">;

	var() EENUM_SCENE_VA ServerMapType		<DisplayName="ServerMapType">;
	//var() EENUM_ROOM_STYLE	ServerMapStyle	<DisplayName="ServerMapStyle">;
	var() EENUM_SCENE_VTYPE ClientSceneVariableType	<DisplayName="ClientSceneVariableType">;

	var() init	string	    MapDescript		<DisplayName="MapDescript">;
	var() init	string	    MinimapName		<DisplayName="MinimapName">;

// 	var init	string	    ImportConfigName		<DisplayName="ImportConfigName">;
// 	var init	string	    ImportMapName		<DisplayName="ImportMapName">;
	
	var() EBATTLE_TEMPLATE_TYPE BattleType	<DisplayName="BattleType">;
	var() int SideCount						<DisplayName="BattleSideCount">;
	var() int MinLimitPerSide				<DisplayName="MinLimitPerSide">;
	var() int MaxLimitPerSide				<DisplayName="MaxLimitPerSide">;

	var() int MinPlayerLevel				<DisplayName="MinPlayerLevel">;
	var() int MaxPlayerLevel				<DisplayName="MaxPlayerLevel">;
	var() int MinPortalLevel				<DisplayName="MinPortalLevel">;
	var() int MaxPortalLevel				<DisplayName="MaxPortalLevel">;

	var() int InitLineCount					<DisplayName="InitLineCount">;
	var() int MaxLineCount                  <DisplayName="MaxLineCount">;
	var() init string Script						<DisplayName="Script">;
	var() int FinishTarget			        <DisplayName="FinishTarget">;
	var() init editfixedsize array<SceneTarget> Progress		<DisplayName="Progress">;
	
	//var() int StandardTime					<DisplayName="StandardTime">;
	//var() int StandardNpcValue				<DisplayName="StandardNpcValue">;
	//var() int StandardDoodadValue			<DisplayName="StandardDoodadValue">;
	//var() int StandardAttackValue			<DisplayName="StandardAttackValue">;

	//var() DropGroup Reward1_group			<DisplayName="Reward1_group">;
	//var() int Reward1_free					<DisplayName="Reward1_free">;
	//var() int Reward1_cost					<DisplayName="Reward1_cost">;
	//var() DropGroup Reward2_group			<DisplayName="Reward2_group">;
	//var() int Reward2_free					<DisplayName="Reward2_free">;
	//var() int Reward2_cost					<DisplayName="Reward2_cost">;
	//var() DropGroup Reward3_group			<DisplayName="Reward3_group">;
	//var() int Reward3_free					<DisplayName="Reward3_free">;
	//var() int Reward3_cost					<DisplayName="Reward3_cost">;

	//var() int ItemIsAll						<DisplayName="ItemIsAll">;
	//var() Item ItemID						<DisplayName="ItemID">;
	//var() int ItemNumber					<DisplayName="ItemNumber">;
	//var() int Fatigue						<DisplayName="Fatigue">;

	//var() bool bPK							<DisplayName="PK">;
	//var() bool bEnablePKPunishment			<DisplayName="Enable PKPunishment">;
	//var() bool bReducedDurability			<DisplayName="ReducedDurability">;
	//var() bool DropItemDurability			<DisplayName="DropItemDurability">;

	//whether the town portal scroll can be used
	var() bool UseScroolDisabled				<DisplayName="CannotUseScroll">;
	//whether teleport to specified coordinates can be used
	var() bool UseTeleportRingDisabled			<DisplayName="CannotUseTeleportRing">;
	//whether players can be (pulled/pulled by) others
	var() bool BeRecalledDisabled				<DisplayName="CannotBeRecalled">;
	//chat restrictions
	var() bool ChatGroupDisabled				<DisplayName="CannotGroupChat">;
	var() bool ChatSceneDisabled				<DisplayName="CannotSceneChat">;
	var() bool ChatScreenDisabled				<DisplayName="CannotScreenChat">;
	var() bool ChatWhisperDisabled				<DisplayName="CannotWhisperChat">;
	var() bool ChatTeamDisabled					<DisplayName="CannotTeamChat">;
	var() bool ChatGuildDisabled			    <DisplayName="CannotGuildChat">;
	var() bool ChatFriendDisabled				<DisplayName="CannotFriendChat">;
	//whether trading is allowed
	var() bool TradeDisabled					<DisplayName="CannotTrade">;
	//whether dropping items is allowed
	//var() bool ItemDropDisabled					<DisplayName="CannotDrop">;
	//whether anti-cheat is enabled
	var() bool EnableAntiCheat					<DisplayName="EnableAntiCheat">;
	//whether cheaters are sent to jail when found
	var() bool GpkPunish				        <DisplayName="GpkPunish">;
	//whether to kick offline when an anomaly is found
	var() bool GpkKick					        <DisplayName="GpkKick">;

	//whether friendliness is enabled
	var() bool EnableIntimacy					<DisplayName="EnableIntimacy">;
	//
	var() int LoginJumpSceneTempID				<DisplayName="LoginJumpSceneTempID">;
	var() int LoginJumpX				        <DisplayName="LoginJumpX">;
	var() int LoginJumpY				        <DisplayName="LoginJumpY">;
	var() int LoginJumpZ				        <DisplayName="LoginJumpZ">;

	var() int MaxRoleCount					    <DisplayName="MaxRoleCount">;
	var() bool BlockRole                        <DisplayName="BlockRole">;

	//mining
	var() int MineItemID1					<DisplayName="MineItemID1">;
	var() int MineItemID2					<DisplayName="MineItemID2">;
	var() int MineItemID3					<DisplayName="MineItemID3">;
	var() int MineItemID4					<DisplayName="MineItemID4">;

	var() int MineDoodadID1					<DisplayName="MineDoodadID1">;
	var() int MineDoodadChance1				<DisplayName="MineDoodadChance1">;
	var() int MineDoodadID2					<DisplayName="MineDoodadID2">;
	var() int MineDoodadChance2				<DisplayName="MineDoodadChance2">;

	var() init editfixedsize array<MineDropInfos> SceneMineDrop		<DisplayName="SceneMineDrop">;

	//mail sending/receiving forbidden
	var() bool CannotSendMail       <DisplayName="CannotSendMail">;
	//gold trading house forbidden
	var() bool CannotUseExchange    <DisplayName="CannotUseExchange">;
	//supreme auction house forbidden
	var() bool CannotUseAuction     <DisplayName="CannotUseAuction">;
	//teaming forbidden
	var() bool CannotJoinTeam       <DisplayName="CannotJoinTeam">;

	var(Weather) editfixedsize init array<WeatherConfig> WeatherTypes <DisplayName="WeatherTypes">;
	var(Weather) float MinWeatherRefreshInterval <DisplayName="MinWeatherRefreshInterval">;
	var(Weather) float MaxWeatherRefreshInterval <DisplayName="MaxWeatherRefreshInterval">;


	// this is only for convenience when importing from EXCEL
	var ServerMapInfos ImportServerMapInfos;

	structdefaultproperties
	{
		InitLineCount=1
		HideRoleName=false
		OpenMapPkMode=false
		EnableIntimacy=true
		CannotSendMail=false
		CannotUseExchange=false
		CannotUseAuction=false
		CannotJoinTeam=false
		ClientSceneVariableType=ESceneVType_Invalid;
		MapPkMode=E_rpm_Total;
		SceneMineDrop(0)=()
		SceneMineDrop(1)=()
		SceneMineDrop(2)=()
		SceneMineDrop(3)=()
		SceneMineDrop(4)=()
		SceneMineDrop(5)=()
		SceneMineDrop(6)=()
		SceneMineDrop(7)=()
		SceneMineDrop(8)=()
		SceneMineDrop(9)=()
		SceneMineDrop(10)=()
		SceneMineDrop(11)=()
		SceneMineDrop(12)=()
		SceneMineDrop(13)=()
		SceneMineDrop(14)=()
		SceneMineDrop(15)=()
		//ServerMapType=esmt_Static
	}
};

var() array<ExportMap> ExportMaps;

enum EDungeonTargetLevel
{
	EFBAchivType_Invalid,
	EFBAchivType_Bronze,
	EFBAchivType_Silver,
	EFBAchivType_Gold
};
struct native DungeonTarget
{
	var() init String TargetName			            <DisplayName="DungeonTargetName">;
	var() init String TargetDesc			            <DisplayName="DungeonTargetDesc">;
	var() EDungeonTargetLevel TargetLevel	            <DisplayName="DungeonTargetLevel">;
	var() int TargetValue	                            <DisplayName="DungeonTarget">;
	var() bool IsHighTargetValue	                            <DisplayName="DungeonTargetHighValueIsGood">;
	var() int ID	                                    <DisplayName="DungeonTargetID">;
	//var int CurValue;
	structdefaultproperties
	{
		TargetLevel=EFBAchivType_Invalid
		TargetValue=-1
		IsHighTargetValue=true
		ID=-1
	}
};

const MaxDungeonCustomTarget = 4;
const MaxDungeonSpecialReward = 10;
struct native DungeonInfo
{
	var int DungeonInfoID;
	var() int MapID			                    <DisplayName="MapID">;
	var int Difficulty		<DisplayName="Difficulty">;
	var() int InitState			<DisplayName="InitState">;
	var() int Type			    <DisplayName="Type">;

	var() int MinLevel			<DisplayName="MinLevel">;
	var() int MaxLevel			<DisplayName="MaxLevel">;
	var() int RoleLimit         <DisplayName="RoleLimit">;
	var() int MaxEnterCount     <DisplayName="MaxEnterCount">;

	var() int StandardTime			        <DisplayName="StandardTime">;
	var() int StandardNpcValue			    <DisplayName="StandardNpcValue">;
	var() int StandardDoodadValue			<DisplayName="StandardDoodadValue">;
	var() int StandardAttackValue			<DisplayName="StandardAttackValue">;

	var() int StandardBossValue			    <DisplayName="StandardBossValue">;
	var() int StandardEliteNpcValue			<DisplayName="StandardEliteNpcValue">;
	var() int StandardSwiftTime			    <DisplayName="StandardSwiftTime">;
	var() DungeonTarget CustomTarget[MaxDungeonCustomTarget]	    <DisplayName="CustomTarget">;

	var() int ExpForVip1			    <DisplayName="Exp_Vip1">;
	var() int ExpForVip2			    <DisplayName="Exp_Vip2">;
	var() int ExpForVip3			    <DisplayName="Exp_Vip3">;
	var() int ExpForTeam			<DisplayName="ExpForTeam">;
	var() int ExpForGuild			<DisplayName="ExpForGuild">;
	var() int ExpForFirst			<DisplayName="ExpForFirst">;
	var() int ExpForActivity		<DisplayName="ExpForActivity">;

	var() int DungeonSpecialRewardsID[MaxDungeonSpecialReward]	    <DisplayName="DungeonSpecialRewardsID">;
// 	var() editconst DropGroup Reward1_group			<DisplayName="Reward1_group">;
// 	var() editconst DropGroup Reward2_group			<DisplayName="Reward2_group">;
// 	var() editconst DropGroup Reward3_group			<DisplayName="Reward3_group">;

	var() int Reward1_groupid			<DisplayName="Reward1_group">;
	var() int Reward2_groupid			<DisplayName="Reward2_group">;
	var() int Reward3_groupid			<DisplayName="Reward3_group">;

	var() int Reward1_free			<DisplayName="Reward1_free">;
	var() int Reward1_cost			<DisplayName="Reward1_cost">;

	var() int Reward2_free			<DisplayName="Reward2_free">;
	var() int Reward2_cost			<DisplayName="Reward2_cost">;
	
	var() int Reward3_free			<DisplayName="Reward3_free">;
	var() int Reward3_cost			<DisplayName="Reward3_cost">;

	var() int ItemIsAll			<DisplayName="ItemIsAll">;
	//var() editconst Item ItemID			<DisplayName="ItemID">;
	var() int ItemTemplateID			<DisplayName="ItemID">;
	var() int ItemNumber		<DisplayName="ItemNumber">;
	var() int Fatigue			<DisplayName="Fatigue">;
};

var() array<DungeonInfo> ExportDungeonInfo;

struct native TeleportObject
{
	var() editconst int ServerID;
	var() editconst string Name;
	var() editconst string DisplayName;
	var() editconst Vector Location;
	var() editconst float Radius;
	var() editconst int Yaw;
	var() editconst int NpcId;          // add by zhangrui05, used for indoor npc pathfinding

	var editoronly bool IsHighlighted;
	var editoronly bool IsSelected;

	var editoronly bool IsUpdated;
};

struct native TeleportNodeObj extends TeleportObject
{
	var() EMONEY_TYPE NeedMnyType       <DisplayName="Money Type">;
	var() int NeedMoney                 <DisplayName="Money Need">;

	//var() editconst int AutoServerID;
	//var() editconst string Name;
	var() editconst int InSceneID;
	var() editconst string PicResName;
	//var() editconst string DisplayName;
	var() editconst EWorldRegion Region;
	var() editconst bool IsOpened;
	//var() editconst int ShowPriority;
	//var() editconst Vector Location;
	//var() editconst float Radius;
	//var() editconst int Yaw;

	//var editoronly bool IsHighlighted;
	//var editoronly bool IsSelected;

	//var editoronly bool IsUpdated;

	structdefaultproperties
	{
		NeedMnyType=E_MONEYTYPE_02
	}
};

struct native MapTeleports
{
	var() string MapName;
	var() Texture2D MapMiniMapTexture;
	var() array<TeleportObject> TeleportOrigList;
	var() array<TeleportObject> TeleportTargetList;
	//var() array<TeleportNodeObj> TeleportNetNodeList;
	var() IntPoint CanvasLocation;
	var() IntPoint CanvasDrawSize;
	var() float DrawScale;

	var editoronly bool IsHighlighted;
	var editoronly bool IsSelected;
// 	structcpptext   
// 	{
// 		void Draw(FViewport* Viewport, FCanvas* Canvas);
// 	}

	structdefaultproperties
	{
		DrawScale=1.0f
	}

	
};

struct native TeleportLink
{
	var editconst int srcLinkID;
	var() string FromMapName;
	var() string ToMapName;
	var() TeleportObject From;
	var() TeleportObject To;
	var editoronly bool IsHighlighted;
	var editoronly bool IsSelected;
	var editoronly bool IsInvalid;

// 	structcpptext
// 	{
// 		void Draw(FViewport* Viewport, FCanvas* Canvas);
// 	}
};

struct native SearchPointObject
{
	var() editconst int SearchPointID;
	var() editconst string SearchPointName;
};

var() array<TeleportLink> TeleportLinks;
var() array<TeleportNodeObj> TeleportNetNodeList;
// runtime load only
var() editconst array<MapTeleports> Teleports;

var() editconst array<SearchPointObject> SearchPoints; 

//var config float MapDrawGlobalScale;
var() config float MapCanvasScale;

var() editconst array<ServerMapInfos> ServerMapInfoList;

struct native NPCInstanceInfo
{
	var() string Name;
	var() int MapID;
	var() int TemplateID;
	var() string NpcDiplayName;
	var() EActorClassType NpcType;
	var() int NpcFuncType;
	var() vector Position;
};

struct native NPCGroupInfo
{
	var() editconst string DisplayName;
	var() editconst int NpcTemplateID;
	var() editconst int NpcLevelMin;

	var() editconst int MapID;
	var() editconst vector Position;
	var() editconst EENUM_NPC_GROUP_TYPE GroupType;
};

struct native SceneContent
{
	var() editconst int MapID;
	var() editconst array<NPCGroupInfo> NpcGroupsInfoList;
};

struct native TeleportLinkInfo
{
	var() int FromMapID;
	var() int FromTeleportOrigID;

	var() int ToMapID;
	var() int ToTeleportTargetID;
};

var() editconst array<NPCInstanceInfo> NPCInfoList;
var() editconst array< SceneContent > SceneContentList;

struct native InMapRegion
{
	var() editconst int MapID;
	var() editconst string VolumeName;
	var() editconst string DisplayName;
	var() editconst string ReuseMapVolumeName;

	var() editconst int Priority;
	var() editconst Box BoundingBox;

	var() editconst int MiniMapTileRowNum;
	var() editconst int MiniMapTileColNum;
	var() editconst float MiniMapPixelSize;
	var() editconst float MiniMapScale;
	var() editconst int MiniMapOffsetX;
	var() editconst int MiniMapOffsetY;

	var() editconst int WorldMapTileRowNum;
	var() editconst int WorldMapTileColNum;
	var() editconst float WorldMapPixelSize;
	var() editconst float WorldMapScale;
	var() editconst int WorldMapOffsetX;
	var() editconst int WorldMapOffsetY;
};

var() array<InMapRegion> MapZoneList;

// add by zhangrui05@snda
// global navigation point info for quests
struct native NaviPointInfo 
{
	//var() int MapId;
	var() string Name;
	var() string DisplayName;
	var() byte IsShowOnUI;
	var() vector Position;
	var() Vector RedirectLoc;
};

struct native MapNaviPointInfo
{
	var() int MapID;
	var() editconst array<NaviPointInfo> Infos;
};

var() editconst array<MapNaviPointInfo> NaviPointInfoList;

// global TriggerVolume info

struct native QuestVolumeInfo
{
	var() int MapID;
	var() string Name;
	var() vector Position;
	var() array<int> Quests;   
};

struct native MapQuestVolumeInfo
{
	var() int MapID;
	var() editconst array<QuestVolumeInfo> Infos;
};

var() editconst array<MapQuestVolumeInfo> QuestVolumeInfoList;

static native function GlobalMapInfo Singleton();

native function PostLoad();
native final function SavePackage();
native final function LoadTeleportsFromXML();
native final function ExportToServer();
native final function int FindMapID(string mapName);
native final function int FindIdxInMapOfType( int mapID, EENUM_SCENE_VA type );
native final function bool FindMapName(int mapID,out string MapName );
native final function ExportMap FindMapInfo(int mapID);
//add by JimZhang@snda.com
native final function ExportMap FindMapInfoByName(string mapName);

defaultproperties
{
}
