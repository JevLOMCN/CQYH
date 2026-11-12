
// Game character controller
//
class MMOPlayerController extends GamePlayerController
	native
	dependson(MMOPlayer)
	dependson(TypeDefine)
	dependson( MMONavigationHelper )
	dependson(UIKeyMapManager)
	config(MMOPlayerController);

`define NavMeshImp  true

var AudioComponent LowHPWarningSound;
var SoundCue LowHPWarningSoundCue;

// Game states
var array< class<MMOGameState> > GameStateClasses;

// Test camera parameters
var int ToggleCameraFov;
struct native TestCameraFov
{
	var float NearFov;
	var float FarFov;
	var float NearPitchOffset;
	var float FarPitchOffset;
	var float NearCameraDistance;
	var float FarCameraDistance;
};

var config array<TestCameraFov> TestCameraFovList;

var Vector PlayerStartPosition;
var PlayerActionQueue PlayerActionQueue;
var UIKeyMapManager     KeyMapManager;

var MMOUIBase InputPanel;

 struct native RoleInfo 	
{
 	var string RoleName;
	//var int level;
	var int dwRoleID;
	//var int classId;
	var int nSceneID;
	var int nSceneTempID;
	var int bySex;
	var int byCareer;
	var int byLevel;
	var int byLayer;
	var int dwPosX;
	var int dwPosY;
	var int dwPosZ;
	var int wDir;
	var int WeaponEffectIndex;
	var int WeaponUpgradePoint;
    var int nOverTime;
    var int nBeginTime;
	var int nSaveTime;
	var int nFaceType;
	var int nHairColor;
	var int nSkinColor;
	var int nHairType;
	//Avatar info is handled separately later
};
//
struct native AvatarInfo
{
	var int RoleId;
	var int MainHand;
	var int OffHand;
	var int EquipList[11/*NES_VISIBLE_TOTAL*/];
};
//Equipment info list of multiple characters
var array<AvatarInfo>  AvatarInfoList;

var array<RoleInfo> RoleList; 
var array<RoleInfo> DeletingRoleList; 
//Currently loaded map
var string MapName;
var int CurMapID;
// Use right mouse button to rotate the camera
var bool		EnableMouseRotCam;

//Determine whether the left mouse button is pressed
var bool        IsLeftMousePressed;
var transient bool bLeftMousePressed;
var transient bool bRightMousePressed;
var transient float MousePressedTime;
var transient bool bInAutoFollowState;

////Determine whether the right mouse button is pressed
//var bool        IsRightMousePressed;
//Chat box active state
var bool	    ChatBoxActived;

//var bool        Repairing;
////Accumulated mouse press time
//var float       DeltaTimeAccumulated;

//var NPC      LockNPC;
//var NPC      CurrentInteractNPC;

var bool        bUIMask;
var string        MaskWidgetName;
var array<int> SkillMap;//skill mapping table 0-9,10:'-',11:'+',12:'mouseright'

// Marker for mouse move target point
var MeshMarker TargetLocationMarker;
var MMOActor   TargetLocationActor;
var Vector     TargetLocationPos; 
var bool       TargetLocationMarkerCanShow;
var MeshMarker PickLocationMarker;

// Current mouse/keyboard control mode, 0 is keyboard, 1 is mouse, 2 is mouse+keyboard
var config int MoveMode;
//Saved login name
var config string SaveUserName;
//Whether to use the mouse in Flash
var config bool bShowSoftMouse;
//Keyboard control route
var int MoveModeByKey;

var transient float LastUpdateFPSTime;


/** Move target from last scripted action */
var Actor ScriptedMoveTarget;

/** Route from last scripted action; if valid, sets ScriptedMoveTarget with the points along the route */
var Route ScriptedRoute;

/** if ScriptedRoute is valid, the index of the current point we're moving to */
var int ScriptedRouteIndex;

/** view focus from last scripted action */
var Actor ScriptedFocus;

/** if true, we're following the scripted route in reverse */
var bool bReverseScriptedRoute;

var Vector2D    PlayerMouse;
var Vector2D    LastScreenMousePos;
var bool        IsMouseMoving;
var Vector      MouseHitWorldLocation;
var Vector      MouseHitWorldNormal;
var Vector      MouseHitNavMeshLocation;
var Vector      MouseProjectionOrigin;
var Vector      MouseProjectionDirection;
var Vector		DestLocation;
var Vector		TempDest;
var Vector      StartTrace;
var Vector      EndTrace;
// Whether auto pathfinding toward the mouse direction
var Vector      TryingMoveDest;
var Vector      LastMoveDest;
var bool        bTryingMoveToMouse;
var bool        bLinearMovement;    //whether to walk straight to the target point
var bool        bOptimizeTargetPos; //whether the target point has been optimized
var bool        bMoveRound;

var Vector      TryingMoveDirection;
var float       StepDistance;

var Vector      PawnEyeLocation;
var const private{private} Actor       TraceActor;

var float       m_fPathFindingFrequency;    // decide how long should we do the next path finding while the l-button is down
var float       m_fDeltaPathingTime;        // the delta time between now and last path finding action

//var bool        DebugShowNavPath;
var bool        bDebugShowCameraDistance;

// debug switch
var bool        bDrawAllNpcInfo;
var bool        bShowMissileCollision;

var bool        bShowNpc;
var bool        bShowMainPlayer;

var bool        bShowTheOtherPlayersByKismet;
var bool        bShowTheOtherPlayersByHotKey;

var bool        bShowDropsByKismet;

var bool        bShowGrid;

//`if( NavMeshImp )
	var MMONavigationHelper     m_NavHelper;
//`else
//	var NavigationPoint         m_DestNavPoint;
//`endif

// Target ladder, auto move is set when entering the state
//var Ladder TargetLadder;

//var array<MMOStone>    StoneValid;
//var MMOStone    StoneToHold;
//var MMOStone    StoneInHand;

// Temporary variables added to test the outfit change system
var bool tempFlag1;
var bool tempFlag2;
var bool tempFlag3;

var int DebugLocationCount;
//var() Vector TempDest;
var vector nextlocation;

var vector WalkingExMoveDir;

// 0:F 1:B 2:L, 3:R
var int MoveKeyInfo[4];  

// Auto attack when mouse is pressed
var bool EnableMouseAutoAttack;

// Toggle keyboard movement
var public bool EnableKeyboardMove;

// Whether waiting for the server response to ability cast
var public int WaitingForTheAbilityRespond;
// Whether waiting for the server response to movement
var public float WaitingForTheMoveToRespond;
// Whether in the ability buffer wait time
var public float WaitingForTheAbilityBuf;
// Whether in the safe zone weapon switch message wait time
var public float WaitingForTheCombatPose;
// Whether in the riding pose switch message wait time
var public float WaitingForEquipMount;

/*
// Knockback direction
var Vector BeatBackDir;
var float BeatBackTime;
*/
//Cursor is in the input box, operations need to be restricted
var bool InTextInput;

/// Hit temporary attributes
var transient AbilityHitEffectComponent CurrentHitEffect;

//Bruce@snda 
var transient array<string> AllAnimArr;
var transient array<float>   AllDurTimeES;
var transient array<MMOActor>  AllNPCActors;
var transient int 	GlobalSeqLoop;
var transient int   nPlayType;
//Bruce@snda 

//// Align the slide start position when jumping
//var bool EnableSlideJumpUpdate;

//// Whether leaving the slide box
//var bool IsJumpOffSlideDown;

// Save old RootMotionMode
var ERootMotionMode OldRootMotionMode;

//// Infinite jump enabled
//var config bool EnableSuperJump;

//// Whether can jump
//var bool EnableJump;

//// Slide speed
//var config float SlideDownSpeed;

//// Wall walking speed
//var config float WalkWallSpeed;

//// Horizontal jump speed multiplier()
//var config float JumpVelocityScale;

//// Jump Z speed
// var config float JumpZSpeed;

// // Jump budget distance
// var config float JumpDist;

// // Jump budget height
// var config float JumpHeight;

// // Player control force while wall walking, must keep pressing space to accumulate
// var float MoveOnWallStrength;

// // Decay speed of player control force while wall walking, seconds to reach 0
// var config float MoveOnWallStrengthFallOffSpeed;

// // Increase speed of player control force while wall walking, amount added per space press
// var config float MoveOnWallStrengthAddSpeed;

// // Player control force drop while wall walking
// var float MoveOnWallStrengthFalling;

 // Player max interaction distance
 var config float MaxInteractiveDistance;

 // Auto pickup item scan distance 4X4 grid cells
 var config float MaxPickDistance;

 // Width of one screen of sync range
 var config float SyncScreenWidth;
 // Half width of one screen of sync range
 var config float SyncScreenHalfWidth;

//------------------------------------------------------------
// Scene interaction control
//------------------------------------------------------------
//var MMOVolume_TriggerLinkageObject CurrentInteractVolume;

//// Whether interaction update is allowed
//var bool EnableInteractUpdate;


//// Whether pushing clockwise
//var bool InteractMoveCW;

//// Which SOCKET is nearest when preparing to push
//var name NearestSocketName;
//var bool IsStandCWPosition;

//// Enable push/pull response after grab lever animation completes
//var bool EnablePushPull;

//// Whether the grab lever animation is playing
//var bool IsPlayingHoldAnim;

//// Grab lever animation time
//var float HoldAnimTime;

//// Grab lever animation time completed since playback started
//var float HoldAnimTimeElapsed;

//// Player character position at grab lever start
//var Vector InitPosBeforeHold;

//// Player character facing at grab lever start
//var rotator InitRotBeforeHold;

//// Lever rotation speed (Percent/Second)
//var config float MoveHandSpikeSpeed;

//// Lever standard rotation speed (degrees/second)
//var config float MoveSpikeStandardSpeed;

//Only show equipment with color quality greater than this
var config EItemColor ColorAboveAsTreasure;

var bool         bIsInQTE;
var IQTEActor    QTEOwner;
var bool         bIsInBossQTE;
var bool         bIsOperatingDoodad;

var bool        RecreatePlayer; // whether to recreate pawn when switching scenes
//var string  RoleName;
var int RoleLv;
//Whether the current player is in soul training; if so, ID is greater than 0
//var int FastPracticeId;
//Soul egg points, ability points
var int SkillPoint;

enum QTEType
{
	EQTET_Frequency,
	EQTET_Accurate,
	EQTET_Final,
};

enum ReviveType
{
	EPRT_NONE,
	EPRT_ITEM,
	EPRT_COIN,
	EPRT_FREE,
};

var QTEType     CurrQTEType;

// Weather system test

// Resource template manager
//var ResourceTemplateManager ResourceTemplates;

//------------------------------------------------------------
// Network protocol related variables
//------------------------------------------------------------
// Login IP and port


// var config array<string> LoginServerIP;//ServeNum
// var config array<string> LoginServerName;
// var config int LoginServerPort;
//var config bool ShowServerSelect;

var transient int PawnServerId;

var config string PlayerPawnArchetypeName;     // records the pawn archetype type, preserved when switching scenes
var config array<string> PlayerPawnLoginSelectArchetypeName;

// Whether the client can use abilities
var bool ClientCanUseAbility;
var bool IsInTransform;

var config int ClassADftSkill[4];
var config int ClassADftSkillLR[2];
var config int ClassBDftSkill[4];
var config int ClassBDftSkillLR[2];

// In certain cases the player must periodically send movement messages to the server
//var bool HackSendMoveToServer;

// Switch and timer for reporting the player's own coordinates
var bool bNeedCommitPosition;
var int PawnLastCommitPositionTickCount;

var transient bool bRequestedAbility;

var bool                bDesiredActor;
var Actor               DesiredActor;
var transient MMOActor	SelectedActor;	// currently selected target character
var int TradeTargetID;                  //current trade target ID
var transient MMODropActor PickItemActor;  // item currently being picked up
//var transient MMODecalManager	    MyDecalManager;

//Equipment synced from server, applied on the next update after syncing

var bool CurrentShowAllUI;

var KeyEventProcessor GameKeyEventProcessor;

var const float PathFindCollisionRadius;

var bool OnRevive;	// refuse position setting during this time

// perf test
var transient bool	bShowPlayerCount;	// display player count number on HUD

native function SetGameKeyEventProcessState(string InNewStateName, optional Object InContextObject);

cpptext
{
	AMMOPlayer* GetPlayer();
	// fangruyi@snda.com
	virtual void SyncPosition( );
	// fangruyi@snda.com

	// LifeSpan QTE total duration, in seconds
	void OnQTEStart(class UBuff*  BuffTemplate,INT objId ,  FLOAT LifeSpan);

	void OnQTEEnd(class UBuff*  BuffTemplate, class AMMOActor* Target);

	void OnAbilityTimerBarStart(class UMMOAbility* Skill, FLOAT Rate , INT IntervalTime = 0);
		
	void OnAbilityTimerBarEnd(class UMMOAbility* Skill,UBOOL bBreak);

	void OnOperateDoodadBarStart(FString strTitle, INT IntervalTime = 0);
		
	void OnOperateDoodadBarBarEnd(UBOOL bBreak);

	//void RequestUseAbility(INT InSlotId);

	// zhaojun@snda, pathfinding parameters that need to be overridden
	virtual void SetupPathfindingParams( FNavMeshPathParams& out_ParamCache );

}

native function string ConsoleCommand(string Command, optional bool bWriteToLog = true);
// Set the current mouse file name, e.g. default.ini, configured in [Cursor] of DefaultGame.ini and stored in MMOGame\Content\CursorAni
final native function SetCurrentMouseCursor(EMMOCursorType cursorType);

exec function TestMouseCursor(EMMOCursorType cursorType)
{
	SetCurrentMouseCursor(cursorType);
}

// Register object with the C++ manager
final native function RegisterToNativeManager();

// if ResX < 0 && ResY < 0, set viewport to fullscreen
final native function SetSystemSettings(int WorldLevel, int TextureLevel, int ShadowLevel, int ResX, int ResY,bool bFullScreen);
final native function GetSystemSettings(out int WorldLevel, out int TextureLevel, out int ShadowLevel, out int ResX, out int ResY, out int bFullScreen);

// open time factor
final native function SetTimeFactor(int state, float value, float time);
//------------------------------------------------------------
// Network protocol related functions
//------------------------------------------------------------
final native function SendRequest_SelectPlayer( int nPlayerID );
final native function SendRequestSetFocus(int InTargetId);
//final native function TeleportToScene(string mapName, vector position);
final native function SendRequest_MoveTo(Vector position,bool bRunOrWalk);
final native function SendRequestStop();
final native function SendRequestAbility( int nAbilityID );

final native function UpdateServerPosition();
final native function CreateRole( string newRolename,int classID,int sex );
//final native function ReturnLogin();

//final native function RequestUseAbility(int TargetId, int AbilityId, int AbilityLevel, Vector Point, optional int byRotateToTarget=1);
final native function int RequestUseAbilityEx(int TargetId,int SpellId,int level, Vector Point, int byRotateToTarget=1);
final native function RequestSwitchSpell(int SpellId);

final native function NativeTestAcceptQuest(int Questid);
final native function NativeTestAbandonQuest(int Questid);
final native function NativeTestCompleteQuest(int Questid, int selectaward);
final native function OnQTEFinish( int id );
native function SetCharAvatart(int i);

final native function CallBack_PreLoad_ChangeScene(); 
// 2D map data generation
//final native function   Gen2DMapData( int w, int h );

// Export mechanism and area info
//final native function ExportDataForServer();

//Only one AMMOPlayerController exists for the whole program lifetime; after a character logs in again, all data that needs resetting is put here
final native function ResetData( );
//Login
final native function LoginGame(string IP, int Port, string username, string password);

//Bruce@snda 
final native function PlayMPlayerAnim();
final native function PlayMPlayerAnimTick(FLOAT delTime);
final native function PlayAllAnimByNPCID(int npcID);
final native function PlayAllAnimByNPCIDTick(FLOAT delTime);
final native function PlayPlayerAnimLoop(int Num);
final native function PlayNPCAnimLoop(int npcID, int SeqLoop);
final native function PlayNPCAnimLoopTick(FLOAT delTime);
final native function PlayPlayerAnimLoopTick(FLOAT delTime);
final native function StopPlayPNAnim();
final native function AddAllAnimAndTime(MMOActor PNActor);
//Bruce@snda 



event  IniSlotBind()
{
	local UIKeyMapManager KeyMapMgr;
	KeyMapMgr = MMOGame(WorldInfo.Game).KeyMapManager;
	KeyMapMgr.IniServerSlotBind();
}

event  SaveSlotBind()
{
	local int j;
	local MMOPlayer pl;
//	local MMOAbility Skill;
	local int KeySkillLimit;
	local UIKeyMapManager KeyMapMgr;
	
	KeyMapMgr = MMOGame(WorldInfo.Game).KeyMapManager;
	pl = MMOPlayer( Pawn );
	`log("MMOPlayerController::SaveSlotBind()");

	KeySkillLimit = 4;

	if( pl.GetClassId() == EPCT_PlayerClassA )
	{
		for( j = 0; j< KeySkillLimit ;j++)
		{
			 ClassADftSkill[j] = KeyMapMgr.GetCmdIDBySlotID( j );
		}	
		//ClassADftSkillLR[0] = KeyMapMgr.GetCmdIDBySlotID( ECK_MouseL );
		//ClassADftSkillLR[1] = KeyMapMgr.GetCmdIDBySlotID( ECK_MouseR );
		ClassADftSkillLR[0] = KeyMapMgr.GetCmdIDBySlotID( ECK_Slot1 );
		ClassADftSkillLR[1] = KeyMapMgr.GetCmdIDBySlotID( ECK_Slot2 );
	}
	else	if( pl.GetClassId() == EPCT_PlayerClassB )
	{
		for( j = 0; j< KeySkillLimit ;j++)
		{
			 ClassBDftSkill[j] = KeyMapMgr.GetCmdIDBySlotID( j );
		}	
		//ClassBDftSkillLR[0] = KeyMapMgr.GetCmdIDBySlotID( ECK_MouseL );
		//ClassBDftSkillLR[1] = KeyMapMgr.GetCmdIDBySlotID( ECK_MouseR );
		ClassBDftSkillLR[0] = KeyMapMgr.GetCmdIDBySlotID( ECK_Slot1 );
		ClassBDftSkillLR[1] = KeyMapMgr.GetCmdIDBySlotID( ECK_Slot2 );
	}
	//Release version has a problem, temporarily use config
	SaveConfig();
}

event Pawn SpawnCharacter(Vector Loc, Rotator Rot)
{
	local Pawn ResultPawn;
	local Pawn PawnArchetypeToUse;

	PawnArchetypeToUse = Pawn(DynamicLoadObject(PlayerPawnArchetypeName,class'MMOPlayer'));
	ResultPawn = Spawn(PawnArchetypeToUse.Class,,,Loc,Rot,PawnArchetypeToUse,true);
	return ResultPawn;
}

//Various tests---------------------------------------------------------------------------------------------------------------------
exec function TestNpcTalk( string strWords )
{
	local MMOUISceneHint mPanel;
	mPanel = MMOUISceneHint( class'MMOUIManager'.static.FindUIPanel(class'MMOUIManager'.default.Panel_SceneHint) );
	mPanel.TestNpcTalk( strWords );
}

native function NativeDeleteBagItem(int slot1, int slot2);
exec function DeleteBagItem(int slot1, int slot2)
{
	NativeDeleteBagItem( slot1,  slot2);
}

native function NativeKismetRemoteEvent(string eventName);
exec function KismetRemoteEvent(string eventName)
{
	NativeKismetRemoteEvent(eventName);
}

native function NativeAres(int nIndex);
exec function Ares(int nIndex)
{
	NativeAres(nIndex);
}

exec function TestDamage( int num )
{
	local int i;
	local DamageResult dmg;
	local vector loc;
	loc = Pawn.Location;
	dmg.HPChange = -100;
	dmg.MainRole = true;
	dmg.Type = DRT_Result6;

	for( i = 0 ; i < num ; i++ )
	{
		MMOHud(myHUD).AddHitStats( dmg , loc );
		//class'MMOUIManager'.static.ShowAttackNumber(  600 , 400 , 1234 );
	}
}

native function NativeTestCtrlType(int CTID);
exec function TestCtrlType(int CTID)
{
	NativeTestCtrlType(CTID);
}

native function NativeTestReputation();
exec function TestReputation()
{
	NativeTestReputation();
}

native function NativeTestMount(int MountID);
exec function TestMount(int MountID)
{
	NativeTestMount(MountID);
}

native function NativeTestSomeThing();
exec function TestSomeThing()
{
	NativeTestSomeThing();
}

native function NativeTestShowPanelEx(string strPanelName, bool bShow);
exec function TestShowPanelEx(string strPanelName, bool bShow)
{
	NativeTestShowPanelEx(strPanelName, bShow);
}

native function NativeTestDropIcon(int TypeID);
exec function TestDropIcon(int TypeID)
{
	NativeTestDropIcon(TypeID);
}

native function NativeTestEfficiency(int nEfficiency);
exec function TestEfficiency(int nEfficiency)
{
	NativeTestEfficiency(nEfficiency);
}


native function NativeTestVersion();
exec function TestVersion()
{
	NativeTestVersion();
}

native function NativeTestNeiGua(string flagName, bool bFlag);
exec function TestNeiGua(string flagName, bool bFlag)
{
	NativeTestNeiGua(flagName, bFlag);
}

native function NativeTestAttackAI(string flagName, float fValue);
exec function TestAttackAI(string flagName, float fValue)
{
	NativeTestAttackAI(flagName, fValue);
}

native function NativeTestUniformSpeed();
exec function TestUniformSpeed()
{
	NativeTestUniformSpeed();
}


native function NativeTestBlock(int BlockType, float BlockRadius);
exec function TestBlock(int BlockType, float BlockRadius)
{
	NativeTestBlock(BlockType, BlockRadius);
}

exec function TestBattleScore()
{
	class'MMOUIManager'.static.ShowPanelEx(class'MMOUIManager'.default.Panel_BattleSummary_BattleScoreBoard, true, "");
}
exec function TestPVPBattle()
{
	class'MMOUIManager'.static.ShowPanelEx( class'MMOUIManager'.default.Panel_PVP_PVPBattle, true, "");
}
exec function TestFBEnd()
{
	//local MMOGFXPlayer_FBEndAll pMv;
	//pMv = MMOGFXPlayer_FBEndAll( class'MMOUIManager'.static.FindMoviePlayer(class'MMOUIManager'.default.MoviePlayer_FBEndAll) );
	//pMv.WaitFBtoSummary();
}
exec function TestSoulEmbed()
{
	class'MMOUIManager'.static.ShowPanelEx( class'MMOUIManager'.default.Panel_SoulEmbed, true, "");
}
exec function TestSoulLevelUp()
{
	class'MMOUIManager'.static.ShowPanelEx( class'MMOUIManager'.default.Panel_SoulLevelUp, true, "");
}
exec function TestWeaponSkill( bool isSealMode )
{
	if( isSealMode ){
		class'MMOUIManager'.static.ShowPanelEx( class'MMOUIManager'.default.Panel_WeaponSkill, true, "ChangeToTab1");
	}
	else{
		class'MMOUIManager'.static.ShowPanelEx( class'MMOUIManager'.default.Panel_WeaponSkill, true, "ChangeToTab0");
	}
	
}
exec function TestFBTask( int mapID, int sceneVariableIdx, int variableValue )
{
	NativeTestFBTask( mapID, sceneVariableIdx, variableValue );
}
exec function TestRank()
{
	class'MMOUIManager'.static.ShowPanelEx( class'MMOUIManager'.default.Panel_RankList, true, "");
}
native function NativeTestFBTask( int mapID, int id, int value );

// exec function ShowServerSelect( bool show)
// {
// }

exec function TestPopupQuest(int id, bool accept, bool share, int shareSrcId)
{
//	class'MMOUIManager'.static.OpenQuestPopupUp(accept, id, share, shareSrcId);
}

exec function TestAcceptQuest( int id)
{
	NativeTestAcceptQuest(id);
}

exec function TestAbandonQuest( int id)
{
	NativeTestAbandonQuest(id);
}

exec function TestCompleteQuest( int id)
{
	NativeTestCompleteQuest(id,0);
}

exec function TeleportInMap(float x, float y, float z)
{
	local Vector pos;
	pos.X = x;
	pos.Y = y;
	pos.Z = z;
	`log("=========== CurentPos" @Pawn.Location.X $ "," @Pawn.Location.Y $ ","  @Pawn.Location.Z);
	Pawn.SetLocation(pos);
 }

exec native function TestGuideArrow(int id, int offsetX, int offsetY);
exec native function TestGuideBox(int id, int offsetX, int offsetY, int width, int height);

exec function DebugShowPos()
{
	`log("=========== CurentPos" @Pawn.Location.X $ "," @Pawn.Location.Y $ ","  @Pawn.Location.Z);
}

exec native function TestMyNewLogic();

// EventSound test
exec native function TestEventSound();

exec function DrawNpcInfo()
{
	bDrawAllNpcInfo = !bDrawAllNpcInfo;
}

exec function ShowMissileCollision()
{
	bShowMissileCollision = !bShowMissileCollision;
}

//--------------------------------------------------------------------------------------------------------------------------

exec native function SetAnimReplayFPS(int InFramesPerSecond);

native function bool IsPlayingMatinee();
native function bool StopActiveMatinee();

native function SetShowNpc(bool bShow);
native function SetShowMainPlayer(bool bShow);
native function SetShowTheOtherPlayers(bool bShow);
native function SetShowDrops(bool bShow);

native function SetShowTheOtherPlayersByHotKey(bool bShow);

native function ClearOldSceneFlags();

// Initialization function
native final function NativePostBeginPlay();

simulated event PostBeginPlay()
{	
	//local int resx, resy, wl, tl, sl, bfull;
	super.PostBeginPlay();

	NativePostBeginPlay();

	// test
	SetCurrentMouseCursor(EMC_Arrow);
	// test

	LastUpdateFPSTime = WorldInfo.TimeSeconds;

	//BagItemIdList.Add( curBagCapacity );
	//EquippedItemIdList.Add(EET_MaxValue);
	PlayerActionQueue = new class'PlayerActionQueue';
	// system settings
	//self.GetSystemSettings(wl, tl, sl, resx, resy, bfull);
	//self.SetSystemSettings(5, 5, 5, resx, resy, (bfull==1 ? true : false) );
	ConsoleCommand("SCALE HIGHEND");


	`Log("__________[MMO]: MMOPlayerController::PostBeginPlay");

	TargetLocationMarker = Spawn(class'MeshMarker', self, 'marker');
	PickLocationMarker = Spawn(class'MeshMarker', self, 'marker_pick');
	TargetLocationMarker.SetHidden(true);
	PickLocationMarker.SetHidden(true);

	`Log("[MMO]: create emitter"@TargetLocationMarker);

//`if( NavMeshImp )
	m_NavHelper = new( self ) class'MMONavigationHelper';
	m_NavHelper.Setup( self );
//`endif
	
	// Create a temporary intermediate object
	RegisterToNativeManager();

	//ResourceTemplates.RegisterToFacade();

	//`Log("__________[MMO]: MMOPlayerController::PostBeginPlay MyDecalManager" @ WorldInfo.MyDecalManager);
// 	MyDecalManager = MMODecalManager(WorldInfo.MyDecalManager);
// 	if( MyDecalManager == none )
// 	{
// 		`Log( "[MMO] No mmo decal manager." );
// 	}
	KeyMapManager = MMOGame( WorldInfo.Game).KeyMapManager;
	KeyMapManager.DefaultKeyMap( self );

	bDrawAllNpcInfo = false;
	bShowMissileCollision = false;

	// all key event processor
	GameKeyEventProcessor = new(self) class'KeyEventProcessor';
	GameKeyEventProcessor.Init(self);
}

event GetSeamlessTravelActorList(bool bToEntry, out array<Actor> ActorList)
{
	local int i,j;
	super.GetSeamlessTravelActorList( bToEntry, ActorList );

	ActorList.AddItem(self);
	if( myHUD !=none)
	{
		`log("MYHUD!!!!"@myHUD);
		ActorList.AddItem( myHUD);
	}
	//if( PlayerCamera != none )
	//{
	//	MMOPlayerCamera(PlayerCamera).CurrentCameraActor = none;
	//	ActorList[ActorList.Length] = PlayerCamera;
	//	//ActorList[ActorList.Length] = PlayerCamera.AnimCameraActor;
	//}
	if( PickLocationMarker != none )
	{
		ActorList[ActorList.Length] = PickLocationMarker;
	}
	if( TargetLocationMarker != none )
	{
		ActorList[ActorList.Length] = TargetLocationMarker;
	}
	if( Pawn != none )
	{
		ActorList[ActorList.Length] = Pawn;
	}

	if(MMOActor(Pawn)!=none)
	{
		for(i = 0; i < MMOActor(Pawn).CanNotBeGCEmitters.Length; ++i)
		{
			if(MMOActor(Pawn).CanNotBeGCEmitters[i] != none)
			{
				ActorList.AddItem(MMOActor(Pawn).CanNotBeGCEmitters[i]);
			}
		}

		for(i = 0; i < MMOActor(Pawn).EquipList.Length; ++i)
		{
			if(ArmorActor(MMOActor(Pawn).EquipList[i].ComponentOrActor) != none)
			{
				ActorList.AddItem(ArmorActor(MMOActor(Pawn).EquipList[i].ComponentOrActor));

				for(j = 0; j < ArmorActor(MMOActor(Pawn).EquipList[i].ComponentOrActor).Attached.Length; ++j)
				{
					ActorList.AddItem(ArmorActor(MMOActor(Pawn).EquipList[i].ComponentOrActor).Attached[j]);
				}
			}
		}
		
	}

}

native function SwitchUIPlayerByMapName(String inMapName);
native function OnHUDLoaded( );


//exec function ToggleHud( bool show)
//{
//	MMOHud(myHUD).HudMovie.SetUIPlayerVisible( show);
//}

// Enable RootMotion
function SetRootMotionMode( ERootMotionMode mode )
{
	OldRootMotionMode = Pawn.Mesh.RootMotionMode;
	Pawn.Mesh.RootMotionMode = mode;
}

// Restore to the previous RootMotionMode
function RestoreRootMotionMode()
{
	Pawn.Mesh.RootMotionMode = OldRootMotionMode;
}

function ShowWeapon(bool show)
{
	MMOActor(Pawn).HideWeapon(!show);
}



exec function MouseAutoAttack(bool on)
{
	EnableMouseAutoAttack = on;
}

event NotifyPhysicsVolumeChange(PhysicsVolume NewVolume)
{
	//`log("OSCAR:XXXXXXXXXXXXXXXX NotifyPhysicsVolumeChange" @NewVolume);
}

event bool NotifyHeadVolumeChange(PhysicsVolume NewVolume)
{
	//`log("OSCAR:XXXXXXXXXXXXXXXX NotifyHeadVolumeChange" @NewVolume);
	return super.NotifyHeadVolumeChange(NewVolume);
}

event bool NotifyLanded(vector HitNormal, Actor FloorActor)
{
	//`log("OSCAR:XXXXXXXXXXXXXXXX NotifyLanded" @HitNormal @FloorActor);
	return super.NotifyLanded(HitNormal, FloorActor);
}

event bool NotifyHitWall(vector HitNormal, actor Wall)
{
	`log("OSCAR:XXXXXXXXXXXXXXXX NotifyHitWall" @HitNormal @Wall);
	//StopMove(true);
	return super.NotifyHitWall(HitNormal, Wall);
}

event NotifyFallingHitWall(vector HitNormal, actor Wall)
{
	//`log("OSCAR:XXXXXXXXXXXXXXXX NotifyFallingHitWall" @HitNormal @Wall);
	//PlayAnimation('HitWallEx', EAnimType_Active, false, 0.1);
	//StopMove(true);
}

event bool NotifyBump(Actor Other, Vector HitNormal)
{
	`log("OSCAR:XXXXXXXXXXXXXXXX NotifyBump" @Other @HitNormal);
	//if(nPlayerColisionType==1)
	//{
	//	MoveToPoint(Pawn.Location);
	//}
	//else if(nPlayerColisionType==2)
	//{   
	//	StopMove(true);
	//}
	return super.NotifyBump(Other, HitNormal);
}

event MayFall(bool bFloor, vector FloorNormal)
{
	`log("OSCAR:XXXXXXXXXXXXXXXX MayFall:" @bFloor @FloorNormal);
	StopMove(true);
}

event LongFall()
{
	//`log("OSCAR:XXXXXXXXXXXXXXXX LongFall");
	StopMove(true);
}

event NotifyJumpApex()
{
	//`log("OSCAR:XXXXXXXXXXXXXXXX NotifyJumpApex");
}

event NotifyMissedJump()
{
	//`log("OSCAR:XXXXXXXXXXXXXXXX NotifyMissedJump");
}

exec function SetPlayerCollisionType(int nType )
{
	nPlayerColisionType = nType;
}

exec function SetPlayerCollisionIncludeAngle(float IncludeAngle)
{
	fPlayerColisionRatio =  Cos(IncludeAngle/2.0);
}

exec function BlockPlayer()
{
	if(nPlayerColisionType == 1)
		SetPlayerCollisionType(0);
	else
		SetPlayerCollisionType(1);
}


exec function ShowState()
{
	`log("Current State:" @ GetStateName() @ " Phys:" @ Pawn.Physics);
}
//exec function startHint( )
//{
//	ShowQTETip(true, EHT_WarningTrap, , 200, 400, 20);
//}
//---------------------------------------------------------------
// Camera operation functions
//---------------------------------------------------------------

// Mouse wheel zoom - zoom in
native final function ZoomCameraIn();

// Mouse wheel zoom - zoom out
native final function ZoomCameraOut();

//---------------------------------------------------------------
// Camera console parameter modification functions
//---------------------------------------------------------------

// Set max distance between camera and character
exec function CamNearCameraDistance(float minDist, float maxDist)
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.NearCameraDistance = minDist;
		cam.FarCameraDistance = maxDist;
		ShowCamDistance();
	}
}

exec function CamNearCameraDofEnable( bool enable )
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.EnableNearDof = enable;
	}
}

exec function CamNearCameraDofBlurKernelSize( float pram )
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.NearDofFarBlurKernelSize = pram;
	}
}

exec function CamNearCameraDofFarBlurAmount( float pram )
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.NearDofMaxFarBlurAmount = pram;
	}
}

exec function CamNearCameraDofPram( float DofFocusDistance, float DofFocusRadius, float DofFalloffExponent )
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.NearDofFocusDistance = DofFocusDistance;
		cam.NearDofFocusRadius = DofFocusRadius;
		cam.NearDofFalloffExponent = DofFalloffExponent;
	}
}

// Show max distance between camera and character in the console window
exec function ShowCamDistance()
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		`log("Distance Near:" @cam.NearCameraDistance $ " Far:" @cam.FarCameraDistance $" Current:" @cam.GetCurrentDistance());
	}
	else
	{
		`log("ShowCamDistance() camera is none!");
	}
}

//// Show current distance between camera and character in the console window
//exec function ShowCamCurrentOffsetDistance()
//{
//	local MMOPlayerCamera cam;
//	cam = MMOPlayerCamera(PlayerCamera);
//	if(cam != none)
//	{
//		`log(cam.GetCurrentDistance());
//	}
//}

// Set camera Pitch offset
exec function CamPitchOffset(float nearDegree, float farDegree)
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.FarCameraPitch = farDegree;
		cam.NearCameraPitch = nearDegree;
		ShowCamPitchOffset();
	}
}

// Show current camera Pitch value
exec function ShowCamPitchOffset()
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		`log("PitchOffset Near:" @cam.NearCameraPitch $ " Far:" @cam.FarCameraPitch $" Current:" @cam.GetTargetPitch());
	}
	else
	{
		`log("ShowCamPitchOffset() camera is none!");
	}
}

// Set camera Yaw offset
exec function CamYawOffset(float offsetDegree)
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.CameraYawOffset = offsetDegree;
		ShowCamYawOffset();
	}
}

// Show current camera Yaw value
exec function ShowCamYawOffset()
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		`log("Yaw Offset:" @cam.CameraYawOffset);
	}
}

// Set camera near/far FOV
exec function CamFov(float nearFovDegree, float farFovDegree)
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.NearFov = nearFovDegree;
		cam.FarFov = farFovDegree;
		ShowCamFOV();
	}
}

// Show current camera FOV
exec function ShowCamFOV()
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		`log("FOV Near:" @cam.NearFov $ " Far:" @cam.FarFov $" Current:" @cam.GetCurrentFOV());
	}
}

// Set target point offset
exec function CamLookAtOffset(float nearDist, float farDist)
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.FarCameraLookAtOffset = farDist;
		cam.NearCameraLookAtOffset = nearDist;
		ShowCamLookAtOffset();
	}
}

// Show current camera LookAt offset value
exec function ShowCamLookAtOffset()
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		`log("Lookat Offset Near:" @cam.NearCameraLookAtOffset $ " Far:" @cam.FarCameraLookAtOffset $" Current:" @cam.GetCurrentLookAtOffset());
	}
}
// Set camera zoom-in speed
exec function CamSetSpeed(float  fSpeed)
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.DefaultMoveSpeed = fSpeed;
	}
}
// Set camera zoom-in percentage
exec function CamSetScale(float  fScale)
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.DefaultMoveScale = fScale;
	}
}
// Enable camera debug mode
exec function CamSetDebug(bool  bEnableDebug)
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		cam.EnableDebugMode = bEnableDebug;
		cam.DefaultSmoothMove = bEnableDebug;
	}
}

exec function CamSetSmooth(bool  sms)
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	`log("CamSetSmooth:" @ sms);
	if(cam != none)
	{
		cam.EnableSmoothCameraPos = sms;
		`log("cam.EnableSmoothCameraPos:" @ cam.EnableSmoothCameraPos);
	}
}
//---------------------------------------------------------------
// Debug console parameter modification functions for auxiliary object display
//---------------------------------------------------------------

// Set ranged range radius
exec function DebugRangeFar(float val)
{
	local MMOPlayer mmoPlayer;
	mmoPlayer = MMOPlayer(Pawn);
	mmoPlayer.SetRangeAttackRange(val);
	`log("Current Far Range:" @ val);
}


// Set melee range radius
exec function DebugRangeNear(float val)
{
	local MMOPlayer mmoPlayer;
	mmoPlayer = MMOPlayer(Pawn);
	mmoPlayer.SetMeleeAttackRange(val);
	`log("Current Near Range:" @ val);
}

// Show ability animation trajectory line
exec function DebugAbilityLine()
{
	MMOGame(WorldInfo.Game).DrawAbilityDebugLine = !MMOGame(WorldInfo.Game).DrawAbilityDebugLine;
}

exec function DebugSelfCenterBox()
{
	MMOGame(WorldInfo.Game).DrawSelfCenterBox = !MMOGame(WorldInfo.Game).DrawSelfCenterBox;
}

// Whether to show debug character state
exec function DebugActorStates(bool on)
{
	MMOGame(WorldInfo.Game).DrawDebugActorState = on;
}

exec function ClearDebugLine()
{
	FlushPersistentDebugLines();
}

// Test screen shake
exec function TestShakeCam()
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	if(cam != none)
	{
		ClientPlayCameraAnim(CameraAnim'Shake.HeavyHitShake', 1.0f);
		//cam.PlayerAnimation("FX_HitEffects.DamageViewShake");
		//CameraAnim'FX_HitEffects.DamageViewShake'
	}
}
//---------------------------------------------------------------
// Mouse move character control
//---------------------------------------------------------------

event int CanProcessMouseKeyEvent(bool bLeftMouse, bool bRelease)
{
	if(bUIMask && !bRelease)
	{
		`log("CanProcessMouseKeyEvent bUIMask:"@MaskWidgetName );
		return -1;
	}

	if(class'MMOUIManager'.static.GetCurrentGameState() != EGameState_InGame)
	{
		`log("CanProcessMouseKeyEvent != EST_Game");
		return -2;
	}

	if(MMOActor(Pawn).bDead && !bRelease)
	{
		`log("Player Dead, can not reply input");
		return -4;
	}

	return 1;
}

native final function UpdateGridFlag();

native final function LogPickedActor(Actor actor);

native final function SetTraceActor(Actor inTraceActor);
native final function Actor GetTraceActor() const;
//native final function SetDesiredTraceActor(Actor tActor);
native final function NativeOnLeftMouseKey(bool bRelease);
native final function EquipNewSpellAndAutoUnequipOld( int slotId, int abilityId, int abilityLv, int runeSlot,bool isLocked );
native final function UnEquipSpell( int slotId, bool isLocked );
native final function GuildAbilityInScene( MMOAbility toGuildAbility, bool isToShow );
native final function SetTransformState( bool isTransformed );
native final function bool IsTransformed();
event ExecOnLeftMouseKey(bool bRelease)
{
	// Bruce@snda
	if(class'Engine'.static.IsEditor())
	{
		bLeftMousePressed = !bRelease;		
	}
	else // Bruce@snda
	{
		NativeOnLeftMouseKey( bRelease);
	}
} 

native function SetInTextInput( bool In );
native function bool GetInTextInput(  );


native function NativeOnRightMouseKey(bool bRelease);
exec function OnRightMouseKey(bool bRelease)
{

	NativeOnRightMouseKey(bRelease);	
}

exec function SetTimeRate( int dilation )
{
	MMOGame( WorldInfo.Game ).MyTimeManager.DemoDilation = dilation;
}

//exec function SpawnMonster( string strName, int number )
//{
//	local NPC temp;
//	local int itr;
//	local Vector loc;
//	local NPC rst;

//	if( strName == "Spider" )
//	{
//		temp = NPC( DynamicLoadObject( "Monster_Spider.NPC_Spider_Rare", class'NPC' ) );
//	}
//	else if( strName == "BigSpider" )
//	{
//		temp = NPC( DynamicLoadObject( "Monster_Spider.NPC_Spider_BIg", class'NPC' ) );
//	}
//	else if( strName == "Wolf" )
//	{
//		temp = NPC( DynamicLoadObject( "Monster_Wolf.NPC_Wolf_Normal", class'NPC' ) );
//	}
//	else if( strName == "BigWolf" )
//	{
//		temp = NPC( DynamicLoadObject( "Monster_Wolf.NPC_Wolf_Advance", class'NPC' ) );
//	}
//// 	else if( strName == "Robber" )
//// 	{
//// 		temp = NPC( DynamicLoadObject( "Monster_Human.Monster_Human", class'NPC' ) );
//// 	}
//// 	else if( strName == "BigRobber" )
//// 	{
//// 		temp = NPC( DynamicLoadObject( "Monster_Human.Monster_Human_Adv", class'NPC' ) );
//// 	}
//	else if( strName == "Boss" )
//	{
//		temp = NPC( DynamicLoadObject( "Monster_Boss.NPCBoss_Boss", class'NPC' ) );
//		number = 1;
//	}
//	else if( strName == "HellDog" )
//	{
//		temp = NPC( DynamicLoadObject( "Monster_Cerberus_01.NpcCore_Cerberus_01", class'NPC' ) );
//	}
//	if( temp != none && number > 0 )
//	{
//		for( itr = 0; itr < number; ++itr )
//		{
//			loc = Pawn.Location;
//			loc.X += FRand() * MaxInteractiveDistance - 300.f;
//			loc.Y += FRand() * MaxInteractiveDistance - 300.f;
//			if( m_NavHelper.FindPathToDest( loc ) )
//			{
//				rst = Spawn( temp.Class, , , loc, , temp, true );
//				if( rst != none )
//					rst.StartMeshFade( true, 2.5f );
//			}
//		}
//	}
//}

//exec function GenData( int width, int height )
//{
//	Gen2DMapData( width, height );
//}

//function MMOStone SelectStone()
//{
//	local float dist, temp;
//	local MMOStone stone, rst;

//	if( StoneValid.Length == 0 )
//		return none;

//	rst = none;
//	dist = 10000000;

//	foreach StoneValid( stone )
//	{
//		if( stone.ParentPlayer != none )
//			continue;
//		temp = VSizeSq( stone.Location - Pawn.Location );
//		if( temp < dist )
//		{
//			dist = temp;
//			rst = stone;
//		}
//	}

//	return rst;
//}

//exec function DemoHoldStone()
//{
//	local float time;
//	local Vector dir;
//	StoneToHold = SelectStone();
//	if( StoneToHold == none )
//		return;

//	if( !IsInState( 'Idle' ) )
//		GotoState( 'Idle' );
	
//	//`log("___________DemoHoldStone()");
//	StoneToHold.PrepareInteraction(MMOPlayer(Pawn));
//	StoneValid.RemoveItem( StoneToHold );

//	dir = Pawn.Location - StoneToHold.Location;
//	dir.Z = 0.f;
//	dir = Normal( dir );
//	// loc = StoneToHold.Location + ( StoneToHold.Radius + 80 ) * dir;
//	MMOActor( Pawn ).ItemManager.RequestHoldWeapon( false );
//	MMOActor( Pawn ).SetDirection(-dir);
//	//MMOActor( Pawn ).SetDirection( StoneToHold.Location );
//	//ClientSetLocation( loc, TargetRotation );
//	//time = MMOActor( Pawn ).PlayAnimation( 'HoldUp', EAnimType_Active, 
//	//	, , , RBA_Translate, RBA_Translate, RBA_Translate );
//	//SetRootMotionMode( RMM_Translate );

//	time = MMOActor( Pawn ).PlayAnimation( 'HoldUp');

//	//Pawn.GroundSpeed = 100;
//	DisableInput();
//	SetTimer( time, false, 'EnableInput' );
//	SetTimer( time, false, 'DisableRootMotion' );

//	ShowWeapon(false);

//	ShowQTETip(true, EHT_HoldStone,, 100, 400);

//	//GotoState('HoldUpObject');
//}

//exec function DemoDropStone()
//{
//	local float time;
//	if( StoneInHand == none )
//		return;

//	StoneInHand.StaticMeshComponent.WakeRigidBody();
//	time = MMOActor( Pawn ).PlayAnimation( 'DropDown' );
//	//Pawn.GroundSpeed = MMOActor(Pawn).RunSpeed;
//	DisableInput();
//	SetTimer( time, false, 'EnableInput' );
//	SetTimer( time, false, 'ReShowWeapon' );
//	ShowQTETip(false);
//}

function ReShowWeapon()
{
	ShowWeapon(true);
}

//exec function DemoThrowStone()
//{
//	local float time;
//	if( StoneInHand == none )
//		return;

//	StoneInHand.StaticMeshComponent.WakeRigidBody();
//	time = MMOActor( Pawn ).PlayAnimation( 'Throw' );
//	//Pawn.GroundSpeed = MMOActor(Pawn).RunSpeed;
//	DisableInput();
//	SetTimer( time, false, 'EnableInput' );
//	SetTimer( time, false, 'ReShowWeapon' );
//}

function EnableInput()
{
	EnableKeyboardMove = true;
}

function DisableInput()
{
	EnableKeyboardMove = false;
}

function DisableRootMotion()
{
	Pawn.Mesh.RootMotionMode = RMM_Ignore;
}


event bool FindPath(Vector Des)
{
	DestLocation = Des;

	if( HasNavPath(Des) )
	{
		DestLocation.Z +=80;//-Pawn.Mesh.Translation.Z;
	//	SetDestinationPosition( DestLocation );
		StopLatentExecution( );
//`if( NavMeshImp )
		m_NavHelper.BeginPathing( DestLocation );
//`else
//`endif
		//`log("findpath0");
		//Initialized to false so that we can run the state frame and recompute the distance at least once
		//`log("++++++++++++ Find Path OK!");
		return true;
	}
	//`log("findpath1");
	//`log("++++++++++++ Find Path False!"@Pawn.Location@"->"@Des);
	return false;
}

// Determine whether there is a reachable pathfinding point
function bool HasNavPath(Vector Des)
{
//`if( NavMeshImp )
	//`log("++++++++++++ Find Nav Path:" @ Des );
	return m_NavHelper.FindPathToDest( Des );

}

/*
event StartMoveToTargetPosition()
{
	if(!EnableKeyboardMove)
	{
		return;
	}

	//`log( "startmoveto0");

	if( FindPath() )
	{
	//	`log( "startmoveto1");
		// If currently moving, do not switch state, to avoid animation reset causing foot sliding
		if(!IsInState('MouseClickMove'))
		{
			 GotoState( 'MouseClickMove' );
		}
		m_fDeltaPathingTime = 0.f;
	}
	else // fly over if unreachable; reachability is checked afterwards
	{
		`log( "++++++++++++ FindPath failed");
		ScriptTrace();
	}
}
*/
event bool IsLeftShiftDown()
{
	return PlayerInput.PressedKeys.find( 'LeftShift' ) != -1;
}

event bool IsRightShiftDown()
{
	return PlayerInput.PressedKeys.find( 'RightShift' ) != -1;
}

event bool IsLeftAltDown()
{
	return PlayerInput.PressedKeys.find( 'LeftAlt' ) != -1;
}

event bool IsRightAltDown()
{
	return PlayerInput.PressedKeys.find( 'RightAlt' ) != -1;
}

event bool IsLeftCtrlDown()
{
	return PlayerInput.PressedKeys.find( 'LeftControl' ) != -1;
}

event bool IsRightCtrlDown()
{
	return PlayerInput.PressedKeys.find( 'RightControl' ) != -1;
}

native function FlushPressedKeys();


exec function DebugShowCameraDistance()
{
	bDebugShowCameraDistance = !bDebugShowCameraDistance;
}

// function CheckPlayerSelect( MMOPlayer pl)
// {
// 
// }

exec function ChangeCameraMode( bool screenshot , float h = -1 )
{
	local  MMOCameraActor CA;
	if( screenshot )
	{
		foreach AllActors(class'MMOCameraActor', CA)
		{
			if(CA.CameraType == ECSC_Base )
			{
				MMOPlayerCamera(PlayerCamera).ChangeShotCameraMode( ECSC_Base, h );
				//MMOHud(myHUD).HudMovie.SetUIPlayerVisible(false);
			}
		}
	}
}

native function int GetCmdNum(byte Type);
native function bool CanUseAnyAbility(MMOAbility InAbility);

static native function Actor PickActor(vector StartWorldPos, vector EndWorldPos);
static native function Actor PickPoint(vector StartWorldPos, vector EndWorldPos);
static native function Actor PickPointActorPreferred(vector StartWorldPos, vector EndWorldPos);
static native function BOOL PickNavMesh(vector StartWorldPos, vector EndWorldPos);


// to clear state properties
final native function OnLeaveScene();
final native function OnEnterScene();


//native function AddCmdHuntToUseAbility(MMOAbility InAbility, optional Actor Target, optional Vector Point);

/*
function AddCmdHuntToUseAbility(MMOAbility InAbility, optional Actor target, optional Vector point)
{
	local IAbilityDamageable enemyActor;
	local float /*MinDis, */MaxDis, CurrentDis;

	if(target == none)
	{
		return;
	}

	if(MMOActor(Pawn).CanUseAnyAbility() == false || MMOActor(Pawn).CanMove() == false)
	{
		`log("can not use ability or can not move ,return!");
		return;
	}

	enemyActor = IAbilityDamageable(target);		
	if((enemyActor != none))
	{
		if(!enemyActor.IsDead())
		{
			/*MinDis = */InAbility.GetMinAttackDistance();
			MaxDis = InAbility.GetMaxAttackDistance();
			CurrentDis = 0.f;
			if(MMOActor(target) != none)
			{
				CurrentDis = MMOActor(target).GetCollisionRadius();
			}
			CurrentDis = VSize(target.Location - Pawn.Location) - CurrentDis - MMOActor(Pawn).GetCollisionRadius();
			if(CurrentDis > MaxDis * 0.9)
			{
				if(MMOActor(Pawn).bFreeze)
				{
					return;
				}
				AddCmdMoveToActor(target, MaxDis * 0.75);
				AddCmdUseAbility(InAbility, target, point, false);
			}
			else
			{
				AddCmdUseAbility(InAbility, target, point);
			}
		}
	}
	else
	{
		AddCmdMoveToPoint(point);
	}
}

*/


event AddCmdMoveNearToPoint(Vector Des)
{
	local PlayerActionEx_MoveNearToPoint MoveToAction;
	local bool bWaitingForClearCache;

	if(class'Engine'.static.IsEditor())
	{
		MMOActor(Pawn).ScriptMoveTo(Des, 580.f);
	}
	else
	{

		bWaitingForClearCache = IsCmdWaitingForClearCache();
		if(bWaitingForClearCache)
		{
			AddCmdWaitingForClearCache();
		}

		MoveToAction = new class'PlayerActionEx_MoveNearToPoint';
		MoveToAction.Destination = Des;
		MoveToAction.Destination.Z += 10;
		MoveToAction.ActionOwner = self;
		if(bWaitingForClearCache)
		{
			MoveToAction.bNeedReplaceTheFrontAction = false;
		}
		PlayerActionQueue.Add(MoveToAction);
	}
}

event AddCmdMoveToPoint(Vector Des,bool bRunOrWalk,bool bCrossJudge = true)
{
	local PlayerActionEx_MoveTo MoveToAction;
	local bool bWaitingForClearCache;
	//if(MMOActor(Pawn).bFreeze)
	//{
	//	return;
	//}

	//if(MMOActor(Pawn).CanMove() == false)
	//{
	//	return;
	//}

	if(class'Engine'.static.IsEditor())
	{
		MMOActor(Pawn).ScriptMoveTo(Des, 580.f);
	}
	else
	{
		bWaitingForClearCache = IsCmdWaitingForClearCache();

		if(bWaitingForClearCache)
		{
			AddCmdWaitingForClearCache();
		}

		MoveToAction = new class'PlayerActionEx_MoveTo';
		MoveToAction.Destination = Des;
		MoveToAction.Destination.Z += 10;
		MoveToAction.ActionOwner = self;
		MoveToAction.bRunOrWalk = bRunOrWalk;
		MoveToAction.bCrossJudge = bCrossJudge;
		if(bWaitingForClearCache)
		{
			MoveToAction.bNeedReplaceTheFrontAction = false;
		}
		PlayerActionQueue.Add(MoveToAction);
	}
}

native function bool IsCmdWaitingForClearCache();

event AddCmdWaitingForClearCache()
{
	local PlayerActionEx_WaitingForClearCache WaitingForClearCache;
	WaitingForClearCache = new class'PlayerActionEx_WaitingForClearCache';
	WaitingForClearCache.ActionOwner = self;
	PlayerActionQueue.Add(WaitingForClearCache);	
}

event AddCmdWaiting(optional bool bPopFrontCmd = true)
{
	local PlayerActionEx_Waiting WaitingAction;
	WaitingAction= new class'PlayerActionEx_Waiting';
	WaitingAction.ActionOwner = self;
	WaitingAction.LiftSpan = 5.f;
	WaitingAction.bNeedReplaceTheFrontAction=bPopFrontCmd;
	PlayerActionQueue.Add(WaitingAction);
}

event AddCmdMoveToActor(Actor Target, Vector DefaultPos,optional float Offset = 0, optional int TemplateID = 0 , optional bool bNeedReplaceTheFrontAction = true, optional bool bFollowFlag = false, optional bool bDoodad = false, optional int targetActorType = 0,optional bool bPrimAttackDynamicRange = false)
{
	local Vector Temp;
	local float  MinDistance;
	local PlayerActionEx_MoveNearTo MoveNearToAction;
	local bool bWaitingForClearCache;


	if(MMOActor(Pawn).CanMove() == false)
	{
		`log("can not move ,return!");
		return;
	}

	if(Target!=none && Target.IsA('MMOActor'))
	{
		MinDistance = MMOActor(Target).GetCollisionRadius() + 64 + 64;
		`log("offset " @ Offset @ " radius" @ MMOActor(Target).GetCollisionRadius() @ " min "@ MinDistance);
	}
	else
	{
		MinDistance = Offset;//Pawn.GetCollisionRadius() + 10;
	}

	//if(Offset < MinDistance)
	//{
	//	Offset = MinDistance;
	//}
	//else
	//{
	//	if (MMOActor(Target).GetCollisionRadius() > 64.0f)
	//		Offset += MMOActor(Target).GetCollisionRadius() - 64.0f;
	//}
	//`log(Offset);

	if(Target!=none){
		Temp = Normal(Pawn.Location - Target.Location) * Offset + Target.Location;
	}
	else
	{
		Temp = DefaultPos;
	}

	if(class'Engine'.static.IsEditor())
	{
		MMOActor(Pawn).ScriptMoveTo(Temp, 580.f);
	}
	else
	{

		bWaitingForClearCache = IsCmdWaitingForClearCache();
		if(bWaitingForClearCache)
		{
			AddCmdWaitingForClearCache();
		}

		//add by jimzhang@snda.com use nearto instead of the original moveto; nearto can check the target position every 1/8 second,
		MoveNearToAction = new class'PlayerActionEx_MoveNearTo';
		MoveNearToAction.Destination    = Temp;
		MoveNearToAction.Destination.Z  += 10;
		if(Target!=none){
			MoveNearToAction.targetActorID  = MMOActor(Target).ServerID;
		}
		else{
			MoveNearToAction.targetActorID  = 0;
		}
		MoveNearToAction.TemplateID     = TemplateID;
		MoveNearToAction.MinDistance    = MinDistance;
		MoveNearToAction.MaxDistance    = Offset;
		MoveNearToAction.ActionOwner    = self;
		MoveNearToAction.bNeedReplaceTheFrontAction = bNeedReplaceTheFrontAction;
		if(bWaitingForClearCache)
		{
			MoveNearToAction.bNeedReplaceTheFrontAction = false;
		}
		MoveNearToAction.FollowFlag     = bFollowFlag;
		MoveNearToAction.IsDoodad     = bDoodad;
		MoveNearToAction.targetActorType = targetActorType;
		MoveNearToAction.bPrimAttackDynamicRange = bPrimAttackDynamicRange;
		PlayerActionQueue.Add(MoveNearToAction);
	}
}

// add by zhangrui05
event AddCmdTeleport(int linkId)
{
	local PlayerActionEx_Teleport TeleportAction;

	TeleportAction = new class'PlayerActionEx_Teleport';
	TeleportAction.LinkId = linkId;
	TeleportAction.ActionOwner = self;
	TeleportAction.bNeedReplaceTheFrontAction = false;
	PlayerActionQueue.Add(TeleportAction);
}

event AddCmdInteract(IInteractableActor Target, optional float interactRadius = 0, optional int TemplateID = 0, optional bool isDoodad = false)
{
	local PlayerActionEx_Interact InteractAction;
	//if(target == none)
	//{
	//	`log("can not find target");
	//	return;
	//}

	InteractAction = new class'PlayerActionEx_Interact';
	InteractAction.target = target;//MMOActor(target);
	InteractAction.TemplateID = TemplateID;
	InteractAction.InteractRange = interactRadius;
	InteractAction.IsDoodad = isDoodad;
	InteractAction.ActionOwner = self;
	InteractAction.bNeedReplaceTheFrontAction = false;
	InteractAction.LiftSpan = 0.250;

	PlayerActionQueue.Add(InteractAction);
}

event AddCmdCannotInterrupt()
{
	local PlayerActionEx_CannotInterrupt CannotInterrupt;
	CannotInterrupt = new class'PlayerActionEx_CannotInterrupt';
	CannotInterrupt.InterruptLevel=1;
	CannotInterrupt.InterruptLevelTime=3.0f;
	CannotInterrupt.ActionOwner = self;
	CannotInterrupt.bNeedReplaceTheFrontAction = false;
	CannotInterrupt.LiftSpan = 0.250;
	PlayerActionQueue.Add(CannotInterrupt);
}

// add by zhaorui05
event AddCmdOpenStall()
{
	local PlayerActionEx_OpenStall OpenStallAction;
	OpenStallAction = new class'PlayerActionEx_OpenStall';
	OpenStallAction.ActionOwner = self;
	OpenStallAction.bNeedReplaceTheFrontAction = false;
	PlayerActionQueue.Add(OpenStallAction);
}

native function MMOActor GetMMOActorByTempleteID(int TemplateID);
native function MMOActor GetMMODoodadByTempleteID(int TemplateID);
native function MMOActor GetClosedMMOActorByTempleteID(int TemplateID);
native function MMOActor GetClosedMMODoodadByTempleteID(int TemplateID);
native function Vector GetFixedMouseHitWorldLocation();

native function UseAbilityById( int InAbilityId, optional Actor target, optional bool bRelease,optional bool bMouseLeftBtn = false);
native function PressAbilityKey(MMOAbility InAbility, optional Actor target, optional bool bRelease,optional bool bMouseLeftBtn = false);
native function PressMovingKey(MMOAbility InAbility, optional Actor target, optional bool bRelease,optional bool bMouseLeftBtn = false);



event ClearAllCmd()
{
	PlayerActionQueue.CancelAll();
}

event AddCmdAutoAttack(int TemplateID , MMOAbility InAbility )
{
	local PlayerActionEx_AutoAttack AutoAttack;
	AutoAttack = new class'PlayerActionEx_AutoAttack';
	AutoAttack.AbilityToUse = InAbility;
	AutoAttack.TemplateID   = TemplateID;
	AutoAttack.ActionOwner = self;
	AutoAttack.bNeedReplaceTheFrontAction = false;
	PlayerActionQueue.Add(AutoAttack);
}

native function Send_CombatPoseOn(bool bOn, optional byte byType = 0);
native function bool GetWaitingAction();
native function NativeAddCmdUseAbility(MMOAbility InAbility, optional Actor target, optional Vector Point, optional bool bPopFrontCmd = true, optional int TemplateID =-1);
event AddCmdUseAbility(MMOAbility InAbility, optional Actor target, optional Vector Point, optional bool bPopFrontCmd = true, optional int TemplateID =-1, optional int targetActorType = 0,optional bool bPrimAttackDynamicAbility = false)
{
	
	local PlayerActionEx_UseAbility UseAbilityAction;
	local PlayerActionEx_Waiting    WaitingAction;
	local bool bWaitingAction;
	bWaitingAction = false;

	if(InAbility == none /*|| InAbility.bCoolDown*/)
	{
		`log("can not find ability, or the ability is cooldown");
		return;
	}

	if(MMOActor(Pawn).CanUseAnyAbility() == false)
	{
		`log("can not use ability ,return!");
		return;
	}

	if(WaitingForTheCombatPose>0.0f)
	{
		`log("Waiting For The CombatPose ,return!");
		return;
	}

	if(WaitingForEquipMount>0.0f)
	{
		`log("Waiting For The EquipMount ,return!");
		return;
	}

	bWaitingAction = GetWaitingAction();
	
	if(bWaitingAction)
	{
		WaitingAction = new class'PlayerActionEx_Waiting';
		WaitingAction.ActionOwner = self;
		WaitingAction.LiftSpan = 5.f;
		WaitingAction.bNeedReplaceTheFrontAction=bPopFrontCmd;
		PlayerActionQueue.Add(WaitingAction);
	}

	//if(MMOPlayer(Pawn).bHoldWeapon==false)
	//{
	//	if(MMOPlayer(Pawn).ActorAttributes.byCombatPoseOn == true && WaitingForTheCombatPose<=0.0f ){
	//		Send_CombatPoseOn(false,0);

	//		WaitingAction = new class'PlayerActionEx_Waiting';
	//		WaitingAction.ActionOwner = self;
	//		WaitingAction.LiftSpan = 5.f;
	//		WaitingAction.bNeedReplaceTheFrontAction=bPopFrontCmd;
	//		PlayerActionQueue.Add(WaitingAction);
	//		bWaitingAction = true;
	//		//return;
	//	}
	//	else{
	//		return;
	//	}

	//	//else{
	//	//	SendRequestAbility( 4001 );//draw weapon ability
	//	//	WaitingForTheAbilityRespond = 4001;
	//	//	return;
	//	//}

	//	//`log("bHoldWeapon = false!");
	//	//return;
	//}


	bRequestedAbility = true;

	UseAbilityAction = new class'PlayerActionEx_UseAbility';
	UseAbilityAction.bCancelAbility = false;
	UseAbilityAction.ActionOwner = self;
	UseAbilityAction.TemplateID = TemplateID;
	UseAbilityAction.AbilityToUse = InAbility;
	UseAbilityAction.TargetLocation = Point;//MouseHitWorldLocation;
	UseAbilityAction.FixedLocation = Point;//GetFixedMouseHitWorldLocation();
	UseAbilityAction.Target = MMOActor(target);
	UseAbilityAction.targetActorType = targetActorType;
	UseAbilityAction.bPrimAttackDynamicAbility = bPrimAttackDynamicAbility;

	//If there is a target, use the target's coordinates as the target coordinates, because the target moves while the delayed command executes; for point-lock abilities the target is null jimzhang@snda.com
	if(UseAbilityAction.Target!=none)
	{
		UseAbilityAction.targetActorID = UseAbilityAction.Target.ServerID;
		UseAbilityAction.TargetLocation = target.Location;
		UseAbilityAction.FixedLocation  = target.Location;
	}

	if(bWaitingAction){
		UseAbilityAction.bNeedReplaceTheFrontAction = false;
	}
	else{
		UseAbilityAction.bNeedReplaceTheFrontAction = bPopFrontCmd;
	}

	if(!bPopFrontCmd)
	{
		UseAbilityAction.LiftSpan = 5.f;
	}
	PlayerActionQueue.Add(UseAbilityAction);

}

event AddCmdCancelAbility(MMOAbility InAbility)
{
	local PlayerActionEx_UseAbility UseAbilityAction ;

	bRequestedAbility = false;

	if(InAbility == none)
	{
		`log("can not find ability");
		return;
	}

	if(InAbility.SpellCommonAttribute.PrepareType != ES_PT_Type1 && InAbility.SpellCommonAttribute.PrepareType != ES_PT_Type5)
	{
		return;
	}

	UseAbilityAction = new class'PlayerActionEx_UseAbility';
	UseAbilityAction.ActionOwner = self;
	UseAbilityAction.AbilityToUse = InAbility;
	UseAbilityAction.bCancelAbility = true;
	PlayerActionQueue.Add(UseAbilityAction);

}

native function  SetSelectTarget(Actor Target);
native function  Actor GetSelectTarget();

native final function DebugIdleBegin();

// Idle state
//state Idle
//{
//	event PushedState()
//	{
//		super.PushedState();
//		`log("      @@@@@@@@@@ Idle	PushedState");
//		MMOPlayer(Pawn).ItemManager.RequestHoldWeapon(true);
//	}

//	event PoppedState()
//	{
//		super.PoppedState();
//		//`log("      @@@@@@@@@@ Idle	PoppedState");
//	}

//	event BeginState(Name PreviousStateName)
//	{
//		DebugIdleBegin();
//		`log("      @@@@@@@@@@ Idle	BeginState from " @ PreviousStateName);
//		Pawn.ZeroMovementVariables();
//		MMOPlayer(Pawn).ItemManager.RequestHoldWeapon(true);
//		//`log( "Idle speed: " @ Pawn.Velocity );
//		ScriptTrace();
//	}

//	event EndState(Name NextStateName)
//	{
//		`log("      @@@@@@@@@@ Idle	EndState goto " @ NextStateName);
//		ScriptTrace();
//	}

//	event HoldUpStone()
//	{
//		//`log("      @@@@@@@@@@ Idle	HoldUpStone ");
//		if( StoneToHold != none )
//		{
//			StoneToHold.HoldUP( MMOPlayer(self.Pawn), 'HoldStone' );
//			StoneInHand = StoneToHold;
//			StoneToHold = none;
//			MMOPlayer(Pawn).isHoldingStone = true;
//			//GotoState( 'HoldingStone' );
//		}
//	}
//}

function ContinueMoveToward()
{
	local Vector Des;
	if(bTryingMoveToMouse)
	{
		Des = Pawn.Location + TryingMoveDirection * StepDistance;
		MoveToPoint(Des);
	}
}

native function ResetMoving();
native function UpdateMoving();
native function AutoRotation();
//native function vector GetOptimizedDes(vector Des);
//native function Vector ValidateMovingDest4AODServer(Vector Dest);
native function bool EnableMoving(Vector Des);

native function SetTargetLocationMarker(Vector Pos, MMOActor TargetActor );
native function ShowTargetLocationMarker();

native function WantToMove(Vector Des,bool bRunOrWalk,float minRadius=-0.1f);
native function NativeMoveToPoint(Vector Des,bool bRunOrWalk,optional bool bCrossJudge=true,optional float minRadius=-0.1f);
event MoveToPoint(Vector Des)
{
	NativeMoveToPoint(Des,true);
	return;








////	local Vector TempDes;
////	local Vector ServerDes;
//	local bool bEditor;
//	local MMOActor tActor;
////	local Pylon TestPylonS, TestPylonD;
////	local bool bFoundPath;
////	local int i;

//	tActor = MMOActor(Pawn);
//	bEditor = class'Engine'.static.IsEditor();
//	if(!bNeedCommitPosition && !bEditor)
//	{
//		return;
//	}
//	//add by hemu
//	if( tActor.MoveType == EMT_PrepareStall || tActor.MoveType == EMT_InStall )
//	{
//		`log("tActor.MoveType == EMT_PrepareStall || tActor.MoveType == EMT_InStall");
//		return;
//	}
//	//add by jim, used to determine actor contact with blocking
//	if(EnableMoving(Des)==false)
//	{
//		return;
//	}

//	if(MMOActor(Pawn).bDead)
//	{
//		bTryingMoveToMouse = false;
//		bHaveToGetNextWayPoint = false;
//		return;
//	}


//	TryingMoveDest = Des;
//	if(ValidateMoveDest())     // validate the move dest, can't be too close to the nav-mesh edge
//	{
//		bHaveToGetNextWayPoint = false;
//		UpdateMoving();
//	}
//	else//if unable to step forward, then just turn, as a gesture
//	{
//		//AutoRotation();
//	}
}


event RequestAndDoMove(Vector Des, float Speed ,bool rotationToDes)
{
	//MMOActor(Pawn).MoveTo( Des, Speed ,rotationToDes);
	//Des.Z -= Pawn.GetCollisionHeight();
	//SendRequest_MoveTo(Des);
}

function MoveToTarget(Actor Target, float Offset)
{

}


native function int GetQuadrant(Vector Des);
event SetMoveModeByKey(int Mode )
{
	local Vector vec;
	local float scale;
	//local int quadrant;
	scale = 3.0;
	MoveModeByKey = mode;

	if(MoveModeByKey>0)
	{
		switch (MoveModeByKey)
		{
		case 1://left
			vec.X = 64.0;
			vec.Y = -64.0;
			vec.Z = 0.0;
			break;
		case 2://right
			vec.X = -64.0;
			vec.Y = 64.0;
			vec.Z = 0.0;
			break;
		case 3://front
			vec.X = 64.0;
			vec.Y = 64.0;
			vec.Z = 0.0;
			break;
		case 4://back
			vec.X = -64.0;
			vec.Y = -64.0;
			vec.Z = 0.0;
			break;
		default:
			vec.X = 0.0;
			vec.Y = 0.0;
			vec.Z = 0.0;
		}

		//MMOActor(pawn).SetExceptionMove(1,GetQuadrant(vec));

		MoveToPoint( Pawn.Location + vec*scale);
		TargetLocationMarker.SetHidden(true);
	}
}

event NotifyWalkEnd()
{
	//local Vector vec;
	//local float scale;
	//scale = 3.0;
	//if(MoveModeByKey>0)
	//{
	//	switch (MoveModeByKey)
	//	{
	//	case 1:
	//		vec.X = 64.0;
	//		vec.Y = -64.0;
	//		vec.Z = 0.0;
	//		break;
	//	case 2:
	//		vec.X = -64.0;
	//		vec.Y = 64.0;
	//		vec.Z = 0.0;
	//		break;
	//	case 3:
	//		vec.X = 64.0;
	//		vec.Y = 64.0;
	//		vec.Z = 0.0;
	//		break;
	//	case 4:
	//		vec.X = -64.0;
	//		vec.Y = -64.0;
	//		vec.Z = 0.0;
	//		break;
	//	default:
	//		vec.X = 0.0;
	//		vec.Y = 0.0;
	//		vec.Z = 0.0;
	//	}
	//	MoveToPoint( Pawn.Location + vec*scale);
	//	TargetLocationMarker.Hide();
	//}
	//else 
	//{
	//	//TargetLocationMarker.NativeShow();
		UpdateMoving();
	//}

	//local Vector NextPoint;
	////`log("NotifyWalkEnd");
	//if(bHaveToGetNextWayPoint)
	//{
	//	if( m_NavHelper.GetNextMoveLocation( NextPoint, Pawn.GetCollisionRadius( ) ))
	//	{
	//		if(VSizeSq2D(NextPoint - Pawn.Location) < 1)
	//		{
	//			//`log("NextPoint is the same as current location");
	//			bHaveToGetNextWayPoint = false;
	//			TargetLocationMarker.Hide();
	//			if(bTryingMoveToMouse)
	//			{
	//				ContinueMoveToward();
	//			}
	//			return;
	//		}
	//		bHaveToGetNextWayPoint = true;
	//		SendRequest_MoveTo(NextPoint);
	//		MMOActor(Pawn).MoveTo(NextPoint, Pawn.GroundSpeed);
	//	}
	//	else
	//	{
	//		bHaveToGetNextWayPoint = false;
	//		TargetLocationMarker.Hide();
	//		if(bTryingMoveToMouse)
	//		{
	//			ContinueMoveToward();
	//		}
	//	}
	//}
	//else
	//{
	//	TargetLocationMarker.Hide();
	//	if(bTryingMoveToMouse)
	//	{
	//		ContinueMoveToward();
	//	}
	//}
}

native final function bool ValidateMoveDest();
native final function Vector BisectionSearchDest(Vector FootPos,Vector StartPos, Vector EndPos, int BisectionTimes);
native final function Vector StepSearchDest(Vector FootPos,Vector StartPos, Vector StepDir, float StepLength, int StepNum);

native final function Vector OptimizeDest(Vector FootPos,Vector DesPos);
native final function Vector GetSafeDest(Vector DesPos,float SafeRadius);

final native function StopMove(optional bool NeedToNotifyServer = false);


//event StopMove(optional bool NeedToRequest = false)
//{
//	if(NeedToRequest)
//	{
//		if(bNeedCommitPosition)
//		{
//			SendRequest_MoveTo(Pawn.Location);
//		}
//	}
//	MMOActor(Pawn).StopMovement();
//	bHaveToGetNextWayPoint = false;
//	IsLeftMousePressed = false;
//	TargetLocationMarker.Hide();
//}



//Get mouse
native function Vector2D GetMousePosition();

// Time update function
native function NativeGeneratePickRay();
native function NativeGetPickPoint();
native function NativeGetPickPointActorPreferred();
native function BOOL NativeGetPickPointNavMesh();
native function NativePlayerTick( float DeltaTime );
//native function NativePickTick(float DeltaTime);

native function UpdateCursor();

event PlayerTick( float DeltaTime )
{
	Super.PlayerTick(DeltaTime);

	NativePlayerTick( DeltaTime );

	if(WaitingForTheAbilityBuf>0.0)
	{
		WaitingForTheAbilityBuf -= DeltaTime;
	}
	
	if(WaitingForTheCombatPose>0.0)
	{
		WaitingForTheCombatPose -= DeltaTime;
	}

	if(WaitingForEquipMount>0.0)
	{
		WaitingForEquipMount -= DeltaTime;
	}

	PlayerActionQueue.Update(DeltaTime) ;
}

exec function ChangeMoveMode(int mode)
{
	MoveMode = mode;
	//if(MoveMode == 1)
	//{
	//	MoveMode = 0;
	//	//EnableKeyboardMove = true;

	//}
	//else
	//{
	//	MoveMode = 1;
	//	//EnableKeyboardMove = false;
	//}
}

//This function is actually initialization
function UpdateSkillPanel()
{

	//MMOHUD( myHUD).HudMovie.InitSkillBar();
}


function EnterWalkingState()
{
	if(!EnableKeyboardMove)
	{
		return;
	}

	if( MoveMode != 0 && MoveMode != 2)
	{
		return;
	}

	if(!IsInState('PlayerWalkingEx'))
	{
		GotoState('PlayerWalkingEx');
	}
}

function LeaveWalkingState()
{
	if(IsInState('PlayerWalkingEx') || IsInState('MouseClickMove'))
	{
		MoveKeyInfo[0] = 0;
		MoveKeyInfo[1] = 0;
		MoveKeyInfo[2] = 0;
		MoveKeyInfo[3] = 0;
		GotoState('Idle');
	}
}

function ForceLeaveWalkingState()
{
	if(IsInState('PlayerWalkingEx') || IsInState('MouseClickMove'))
	{
		MoveKeyInfo[0] = 0;
		MoveKeyInfo[1] = 0;
		MoveKeyInfo[2] = 0;
		MoveKeyInfo[3] = 0;
		GotoState('Idle');
	}
}

native function NativeOnUp(bool bReleased);
native function NativeOnDown(bool bReleased);

exec function OnUp(bool bReleased)
{
	NativeOnUp(bReleased);
}
exec function OnDown(bool bReleased)
{
	NativeOnDown(bReleased);
}

exec function MoveForwardDown()
{
	////`log("~~~~~~~~~~~~~~~~~ MoveForwardDown"@MMOActor(Pawn).bCanMove);
	//MoveKeyInfo[0] = 1;
	//EnterWalkingState();
	
	////MMOActor(Pawn).TargetRotation.Yaw = 0;
}

exec function MoveForwardUp()
{
	////`log("~~~~~~~~~~~~~~~~~ MoveForwardUp");
	//MoveKeyInfo[0] = 0;
	//if(MoveKeyInfo[0]==0 && 0==MoveKeyInfo[1] && 0==MoveKeyInfo[2] && 0==MoveKeyInfo[3])
	//{
	//	LeaveWalkingState();
	//}

}

event ReturnSelectHotKey()
{
    class'MMOUIManager'.static.SendReturnSelRole();
}

//exec function AddContactMessage( string PlayerName,string Content)
//{
//	MMOHud(myHUD).HudMovie.ContactLevel.AddContactMessage(PlayerName,Content);

//}

native function NativeTest(int testData);

exec function Test(int testData)
{
	NativeTest(testData);
}

exec function MoveBackwardDown()
{
	//`log("~~~~~~~~~~~~~~~~~ MoveBackwardDown");
	MoveKeyInfo[1] = 1;
	EnterWalkingState();

	//MMOActor(Pawn).TargetRotation.Yaw = 180 * DegToUnrRot;
}

exec function MoveBackwardUp()
{
	//`log("~~~~~~~~~~~~~~~~~ MoveBackwardUp");
	MoveKeyInfo[1] = 0;
	if(0==MoveKeyInfo[0] && 0==MoveKeyInfo[1] && 0==MoveKeyInfo[2] && 0==MoveKeyInfo[3])
	{
		LeaveWalkingState();
	}
}

exec function StrafeLeftDown()
{
	//`log("~~~~~~~~~~~~~~~~~ StrafeLeftDown");
	MoveKeyInfo[2] = 1;
	EnterWalkingState();

	//MMOActor(Pawn).TargetRotation.Yaw = 270 * DegToUnrRot;
}

exec function StrafeLeftUp()
{
	//`log("~~~~~~~~~~~~~~~~~ StrafeLeftUp");
	MoveKeyInfo[2] = 0;
	if(0==MoveKeyInfo[0] && 0==MoveKeyInfo[1] && 0==MoveKeyInfo[2] && 0==MoveKeyInfo[3])
	{
		LeaveWalkingState();
	}
}

exec function StrafeRightDown()
{
	//`log("~~~~~~~~~~~~~~~~~ StrafeRightDown");
	MoveKeyInfo[3] = 1;
	EnterWalkingState();

	//MMOActor(Pawn).TargetRotation.Yaw = 90 * DegToUnrRot;
}

exec function StrafeRightUp()
{
	//`log("~~~~~~~~~~~~~~~~~ StrafeRightUp");
	MoveKeyInfo[3] = 0;
	if(0==MoveKeyInfo[0] && 0==MoveKeyInfo[1] && 0==MoveKeyInfo[2] && 0==MoveKeyInfo[3])
	{
		LeaveWalkingState();
	}
}

// Player walking using WASD
state PlayerWalkingEx
{
	ignores SeePlayer, HearNoise, Bump;

	event BeginState(Name PreviousStateName)
	{
		DoubleClickDir = DCLICK_None;
		bPressedJump = false;
		GroundPitch = 0;
		if ( Pawn != None )
		{
			Pawn.ShouldCrouch(false);
			//Pawn.SetMovementPhysics();
			Pawn.SetPhysics( PHYS_Walking );
			//if (Pawn.Physics != PHYS_Falling && Pawn.Physics != PHYS_RigidBody) // FIXME HACK!!!
			//	Pawn.SetPhysics(Pawn.WalkingPhysics);

			//if( StoneInHand == none )
			//	Pawn.GroundSpeed = MMOActor(Pawn).GetMoveSpeed();
			//else
			//	Pawn.GroundSpeed = 100;
		}

		//`log("++++++++++xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	event EndState(Name NextStateName)
	{
		GroundPitch = 0;
		if ( Pawn != None )
		{
			Pawn.SetRemoteViewPitch( 0 );
			if ( bDuck == 0 )
			{
				Pawn.ShouldCrouch(false);
			}

			PlayerInput.aBaseY = 0;
			PlayerInput.aStrafe = 0;
			Pawn.Acceleration = vect(0, 0, 0);
			Pawn.Velocity = vect(0, 0, 0);
			//Pawn.SetPhysics(PHYS_None);
		}

		ShowWeapon(true);

		//`log("---------xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	}

	event NotifyPhysicsVolumeChange( PhysicsVolume NewVolume )
	{
		//`log("++++++++++xxxxxxxxxxxx NotifyPhysicsVolumeChange");

		if ( NewVolume.bWaterVolume && Pawn.bCollideWorld )
		{
			GotoState(Pawn.WaterMovementState);
		}
	}

	event PlayerTick( float DeltaTime )
	{
		//local float OldaBaseY;

		//`log("++++++++++xxxxxxxxxxxx PlayerTick");

		if ( !bShortConnectTimeOut )
		{
			bShortConnectTimeOut = true;
			ServerShortTimeout();
		}

		if ( Pawn != AcknowledgedPawn )
		{
			if ( Role < ROLE_Authority )
			{
				// make sure old pawn controller is right
				if ( (AcknowledgedPawn != None) && (AcknowledgedPawn.Controller == self) )
					AcknowledgedPawn.Controller = None;
			}
			AcknowledgePossession(Pawn);
		}

		//WalkingExMoveDir = vector(MMOActor(Pawn).TargetRotation);
		//WalkingExMoveDir = normal(WalkingExMoveDir);
		//Pawn.SetLocation(Pawn.Location + WalkingExMoveDir * Pawn.GroundSpeed * DeltaTime);
		
		//PlayerInput.aBaseY = MoveForwardBackward;
		//PlayerInput.aStrafe = MoveLeftRight;

		//if ( MoveLeftRight != 0 )
		//{
		//	// aBaseY should be magnitude of deflection, not just in up/down direction
		//	OldaBaseY = PlayerInput.aBaseY;
		//	PlayerInput.aBaseY = FMin(Sqrt(Square(MoveLeftRight) + Square(PlayerInput.aBaseY)), 1.f);

		//	if ( OldaBaseY < 0 )
		//	{
		//		PlayerInput.aBaseY *= -1;
		//	}
		//	//RawJoyUp = aBaseY;
		//}


		PlayerInput.PlayerInput(DeltaTime);

		if ( bUpdatePosition )
		{
			ClientUpdatePosition();
		}
		PlayerMove(DeltaTime);

		AdjustFOV(DeltaTime);
	}

	function PlayerMove( float DeltaTime )
	{
		local vector			X,Y,Z, NewAccel;
		local eDoubleClickDir	DoubleClickMove;
		local rotator			OldRotation;
		local bool				bSaveJump;
		local float             camYaw;

		local int tempX, tempY;
		local bool bHasVec;

		//`log("++++++++++xxxxxxxxxxxxPlayerMove");

		if( Pawn == None )
		{
			GotoState('Dead');
		}
		else
		{
			GetAxes(Pawn.Rotation,X,Y,Z);

			// Update acceleration.
			NewAccel = PlayerInput.aForward*X + PlayerInput.aStrafe*Y;
			NewAccel.Z	= 0;
			NewAccel = Pawn.AccelRate * Normal(NewAccel)*100;

			DoubleClickMove = PlayerInput.CheckForDoubleClickMove( DeltaTime/WorldInfo.TimeDilation );

			// Update rotation.
			OldRotation = Rotation;

			camYaw = 0;

			tempY = MoveKeyInfo[0] - MoveKeyInfo[1];
			tempX = MoveKeyInfo[3] - MoveKeyInfo[2];
			bHasVec = true;
			if(tempY > 0)
			{
				if(tempX > 0)
				{
					camYaw = 45;
				}
				else if(tempX == 0)
				{
					camYaw = 0;
				}
				else
				{
					camYaw = 315;
				}
			}
			else if(tempY == 0)
			{
				if(tempX > 0)
				{
					camYaw = 90;
				}
				else if(tempX == 0)
				{
					//camYaw = -MMOPlayerCamera(PlayerCamera).GetTargetYaw();
					bHasVec = false;
				}
				else
				{
					camYaw = 270;
				}
			}
			else
			{
				if(tempX > 0)
				{
					camYaw = 135;
				}
				else if(tempX == 0)
				{
					camYaw = 180;
				}
				else
				{
					camYaw = 225;
				}
			}

			//`log("-----------" @MoveForwardBackward @MoveLeftRight @camYaw);

			if(bHasVec)
			{
				camYaw += MMOPlayerCamera(PlayerCamera).GetTargetYaw() * UnrRotToDeg;

				MMOActor(Pawn).TargetRotation = MakeRotator(0, camYaw * DegToUnrRot, 0);
			}
			else
			{
				MMOActor(Pawn).TargetRotation = MakeRotator(0,0,0);
			}

			ClientSetRotation(RInterpTo(Pawn.Rotation, MMOActor(Pawn).TargetRotation, DeltaTime, MMOActor(Pawn).RotationSpeed));
			//UpdateRotation( DeltaTime );
			bDoubleJump = false;

			if( bPressedJump && Pawn.CannotJumpNow() )
			{
				bSaveJump = true;
				bPressedJump = false;
			}
			else
			{
				bSaveJump = false;
			}

			if( Role < ROLE_Authority ) // then save this move and replicate it
			{
				ReplicateMove(DeltaTime, NewAccel, DoubleClickMove, OldRotation - Rotation);
			}
			else
			{
				ProcessMove(DeltaTime, NewAccel, DoubleClickMove, OldRotation - Rotation);
			}
			bPressedJump = bSaveJump;
		}
	}

	function ProcessMove(float DeltaTime, vector NewAccel, eDoubleClickDir DoubleClickMove, rotator DeltaRot)
	{
		//`log("++++++++++xxxxxxxxxxxx ProcessMove");

		if( Pawn == None )
		{
			return;
		}

		if (Role == ROLE_Authority)
		{
			// Update ViewPitch for remote clients
			Pawn.SetRemoteViewPitch( Rotation.Pitch );
		}

		Pawn.Acceleration = NewAccel;

		CheckJumpOrDuck();
	}
Begin:
	
	StopLatentExecution();

	while(Pawn != none)
	{
		if(MMOActor(Pawn).TargetRotation.Yaw == 0 && MMOActor(Pawn).TargetRotation.Pitch == 0 && MMOActor(Pawn).TargetRotation.Roll == 0)
		{
			WalkingExMoveDir.X = 0;
			WalkingExMoveDir.Y = 0;
			WalkingExMoveDir.Z = 0;
		}
		else
		{
		WalkingExMoveDir = vector(MMOActor(Pawn).TargetRotation);
		WalkingExMoveDir = normal(WalkingExMoveDir);
		}
		//MoveToDirectNonPathPos(Pawn.Location + WalkingExMoveDir);
		MoveToDirectNonPathPos(Pawn.Location + WalkingExMoveDir * 50);
		//`log("++++++++++xxxxxxxxxxxx MoveToDirectNonPathPos"@WalkingExMoveDir);
		//Pawn.SetLocation(Pawn.Location + WalkingExMoveDir * Pawn.GroundSpeed * DeltaTime);
	}

	//PlayerInput.aBaseY = 0;
	//PlayerInput.aStrafe = 0;
	Pawn.ZeroMovementVariables();

	// hack
	//MMOPlayer(Pawn).AttackSlotMgr.FreeAll();

	//GotoState('Idle');
}

// Injury event
event NotifyTakeHitEx( Controller InstigatedBy, Vector HitLocation, int Damage, class<DamageType> DamageType, Vector Momentum, DamageResult dmgRst )
{
	/*
	local MMOPlayer playerA;

	BeatBackDir = Normal(Pawn.Location - InstigatedBy.Pawn.Location);

	playerA = MMOPlayer(Pawn);

	//`log("_______Dmg Type" @dmgRst.DamageType);

	if((dmgRst.DamageType == class'MMODamageType_Boss')
		|| (dmgRst.Type == eDM_Critical))
	{
		PushState('HeavyHurt', 'Begin');
	}
	else 
	{
		playerA.PlayAnimation( 'wound', EAnimType_Passive );
	}
*/
}

exec function DebugDumpStateStack()
{
	DumpStateStack();
}


function bool IsUsingAbility()
{
	return MMOActor(Pawn).IsUsingAbility();
}

//---------------------------------------------------------------
// Utility functions
//---------------------------------------------------------------

// Load level name
exec function LoadLevel(string levelName)
{
	WorldInfo.SeamlessTravel(LevelName);
}

function AbilityEnd(int id)
{
	//`log("ability used up");
	// Ability end response function
}

native function NativeTestResetDrop();
exec function TestResetDrop()
{
	NativeTestResetDrop();
}

native function TestStartRain();
native function TestStartSnow();
native function TestStartStorm();
native function TestStartLightning();
native function TestStopRain();
native function TestStopSnow();
native function TestStopStorm();
native function TestStopLightning();
native function TestEnableWeather( bool bEnable );

exec function StartRain()
{
	TestStartRain();
}

exec function StartSnow()
{
	TestStartSnow();
}

exec function StartStorm()
{
	TestStartStorm();
}

exec function StartLightning()
{
	TestStartLightning();
}

exec function StopRain()
{
	TestStopRain();
}

exec function StopSnow()
{
	TestStopSnow();
}

exec function StopStorm()
{
	TestStopStorm();
}

exec function StopLightning()
{
	TestStopLightning();
}

exec function EnableWeather( bool bEnable )
{
	TestEnableWeather( bEnable );
}

exec native function SetFire();

exec function SetSkinColorByIdx( int idx )
{
	MMOActor(Pawn).ItemManager.ApplySkinColor( idx );
}

exec function SetHairColorByIdx( int idx )
{
	MMOActor(Pawn).ItemManager.ApplyHairColor( idx );
}

exec function SetFaceModelByIdx( int profession ,int sex, int id)
{
	MMOActor(Pawn).ItemManager.ApplyFaceModel(profession, sex, id );
}

exec function SetHairModelByIdx( int profession ,int sex, int id)
{
	MMOActor(Pawn).ItemManager.ApplyHairModel( profession, sex, id );
}

exec native function RestrictDynamicShadow( bool bRestrict, optional float distance = 5000 );

exec function ShotKey_PrintScreen()
{
	ConsoleCommand( "shot" );
}

native final function NativeOnEnterPressed();

exec function ShotKey_Enter()
{
	NativeOnEnterPressed();
}

native function FocusOnInput( MMOUIBase Panel );
native function FocusOutInput( );

exec native function DumpMemoryUseStatus( );

final native function ReturnSelect();
final native function ReturnLogin();

event TeleportToScene(string targetMapName, vector TargetPos ,EGameState nextState = EGameState_InGame )
{
	//local Color fadeColor;

	PlayerStartPosition = targetPos;
	MMOGame( WorldInfo.Game ).PlayerStartPosition = targetPos;
	MMOGame( WorldInfo.Game ).NextGameState = nextState;

	MapName = targetMapName;

	//MMOGame( WorldInfo.Game ).NextMap = targetMapName;
	//MMOGame(WorldInfo.Game).RestartGame();
	//return;
	MMOHud( myHUD).ShowLoadMovie(true);

	//Must ensure all maps in the List exist
	//Only set the Loading state here
	//MMOGame( WorldInfo.Game ).SceneType = EST_Loading;
	//class'MMOUIManager'.static.ActiveGameState(EGameState_Loading);
	//`log("Now Game State:"@MMOGame( WorldInfo.Game).SceneType );

	WorldInfo.SeamlessTravel( MapName );
	StopMove(true);
	//Pawn.SetPhysics(PHYS_None);


	//fadeColor.A = 0.f;
	//fadeColor.R = 1.f;
	//fadeColor.G = 1.f;
	//fadeColor.B = 1.f;
	//ClientSetCameraFade( true, fadeColor, vect2d( 0, 1 ), 2.f );
}

function TeleportToMap( bool restart, optional string TargetMap, optional string StartPoint )
{
	//local string url;

	return;

	if( !restart )
	{
		MMOGame( WorldInfo.Game ).NextMap = TargetMap;
	}
	
	if( StartPoint != "" )
	{
		MMOGame( WorldInfo.Game ).PlayerStartPoint = StartPoint;
	}
	
	`log("________ TeleportToMap" @StartPoint);

	MMOGame(WorldInfo.Game).SaveConfig( );
	//MMOGame(WorldInfo.Game).RestartGame( );

	//url = restart ? TargetMap @ "Restart" : TargetMap;
	//WorldInfo.Game.bRestartLevel = false;
	//WorldInfo.SeamlessTravel( url );
	WorldInfo.SeamlessTravel( TargetMap );
}

exec function LoadMap(string LevelName)
{
	MMOGame(WorldInfo.Game).LoadMap(LevelName);
}

function bool InputKey( int ControllerId, name Key, EInputEvent Event, float AmountDepressed = 1.f, bool bGamepad = FALSE )
{
	`log("~~~~~~~~~~~~~~~~~" @Key @Event);

	//local PlayerController		PC;
 //   local DebugCameraController	DCC;

	//foreach WorldInfo.AllControllers(class'PlayerController', PC)
	//{
	//	if ( PC.bIsPlayer && PC.IsLocalPlayerController() )
	//	{
	//		DCC = DebugCameraController(PC);
	//		if( DCC!=none && DCC.OryginalControllerRef==none )
	//		{
	//			//dcc are disabled, so we are looking for normal player controller
	//			continue;
	//		}

	//		return DCC.NativeInputKey( ControllerId, Key, Event, AmountDepressed, bGamepad );
	//	}
	//}

	return false;
}


//---------------------------------------------------------------
// HUD management
//---------------------------------------------------------------
/** Does any manual HUD drawing for the Player -- called by this PC's HUD during its PostRender */
 native function DrawHUD( HUD H );
 native function DrawHUDPostGFx( HUD H );
 
event InitInputSystem()
{
	local LocalPlayer LP;

	LP = LocalPlayer(Player);

	Super.InitInputSystem();
	// reset the post processing when we get a new PC
	LP.RemoveAllPostProcessingChains();
	LP.InsertPostProcessingChain(LP.Outer.GetWorldPostProcessChain(), INDEX_NONE, true);
	`log("[MMO] Post Process ===========================" @LP.Outer.GetWorldPostProcessChain());
}

//---------------------------s--------------------------
// Scene interaction action functions
//-----------------------------------------------------

// Jump in a direction
//function JumpToDir(Vector dir)
//{
//	local Vector targetJumpPos;
	
//	if(!EnableJump)
//	{
//		return;
//	}

//	// Switch state first, to ensure the speed works properly afterwards
//	GotoState('FallingEx');

//	targetJumpPos.X = 0;
//	targetJumpPos.Y = 0;
//	targetJumpPos.Z = JumpHeight;

//	targetJumpPos += Pawn.Location + dir * JumpDist;

//	Pawn.SuggestJumpVelocity(Pawn.Velocity, targetJumpPos,Pawn.Location, false);

//	Pawn.Velocity.X *= JumpVelocityScale;
//	Pawn.Velocity.Y *= JumpVelocityScale;
//	Pawn.JumpZ = JumpZSpeed;
//	Pawn.bWantsToCrouch = false;
//	Pawn.bJumpCapable = true;
//	Pawn.SetPhysics(PHYS_Falling);
//	//Pawn.DrawDebugLine(Pawn.Location, targetJumpPos, 255, 0, 0, true);
//	Pawn.SetRotation(Rotator(dir));
//	MMOActor(Pawn).PlayAnimation('JumpUp');
//}

// Enable chain jump
//exec function JumpEnableSuper(bool on)
//{
//	EnableSuperJump = on;
//}

//exec function JumpEnable(bool on)
//{
//	EnableJump = on;
//}

// Switch state by different flags when space is pressed
//exec function JumpEx()
//{
//	local bool lastJumpEnable;

//	//`log("OSCAR:____________Jump");
//	if( !EnableKeyboardMove )
//		return;

//	if( StoneInHand != none )
//	{
//		DemoThrowStone();		
//		return;
//	}
//	else if( StoneValid.Length != 0 )
//	{
//		DemoHoldStone();
//		return;
//	}

//	else if(CurrentMoveOnWallVolume != none)
//	{
//		GotoState('MoveOnWall');
//		return;
//	}
//	else if(CurrentClimbJumpBetweenWallVolume != none)
//	{
//		lastJumpEnable = EnableJump;
//		EnableJump = true;
//		JumpToDir(- Vector(CurrentClimbJumpBetweenWallVolume.WallDir));
//		EnableJump = lastJumpEnable;
//		return;
//	}
//	else if(CurrentSlideDownVolume != none)
//	{
//		GotoState('SlideDown');
//		return;
//	}
//	else if(CurrentInteractVolume != none)
//	{
//		if(CurrentInteractVolume.IsInteractable && CurrentInteractVolume.Target.CanInteractionNow())
//		{
//			GotoState('MoveHandSpike');
//			return;
//		}
//	}
	
//	JumpToDir(vector(Pawn.Rotation));
//}

// Falling state
//state FallingEx
//{
//	event bool NotifyLanded(vector HitNormal, Actor FloorActor)
//	{
//		GotoState('Landing');
//		return super.NotifyLanded(HitNormal, FloorActor);
//	}

//	event NotifyFallingHitWall(vector HitNormal, actor Wall)
//	{
//		//`log("OSCAR:XXXXXXXXXXXXXXXX FallingEx::NotifyFallingHitWall" @HitNormal @Wall);
//		// CHECK:
//		//GotoState('HitWallEx');
//	}

//	exec function JumpEx()
//	{
//		if(EnableSuperJump)
//		{
//			global.JumpEx();
//		}
//	}

//	event BeginState(Name PreviousStateName)
//	{
//		//Pawn.ZeroMovementVariables();
//		Pawn.SetPhysics(PHYS_Falling);
//		CurrentMoveOnWallVolume = none;
//		CurrentClimbJumpBetweenWallVolume = none;
//		CurrentSlideDownVolume = none;

//		//`log("OSCAR:_________ FallingEx::BeginState" @PreviousStateName);
//	}
//	event EndState(Name NextStateName)
//	{
//		//`log("OSCAR:_________ FallingEx::EndState" @NextStateName);
//	}

//	event PlayerTick( float deltaTime )
//	{
//		global.PlayerTick(deltaTime);

//		//////`log("OSCAR:############### FallingEx::PlayerTick" @Pawn.Velocity.Z);
//		//if(Pawn.Velocity.Z > 0)
//		//{
//		//	if(!MMOPlayer(Pawn).IsPlayingAnimationByName('JumpUp_Loop', EAnimType_Active))
//		//	{
//		//		//`log("OSCAR:############### JumpUp_Loop");
//		//		PlayAnimation('JumpUp_Loop', EAnimType_Active, true, 0.2);
//		//	}
//		//}
//		//else
//		//{
//		//	if(!MMOPlayer(Pawn).IsPlayingAnimationByName('Falling', EAnimType_Active))
//		//	{
//		//		//`log("OSCAR:############### Falling");
//		//		StopAnimation('JumpUp_Loop', EAnimType_Active);
//		//		PlayAnimation('Falling_Loop', EAnimType_Active, true, 0.2);
//		//	}
//		//}
//	}

//	exec function MoveForwardDown()
//	{
//		if(EnableSuperJump)
//		{
//			global.MoveForwardDown();
//		}
//	}

//	exec function MoveForwardUp()
//	{
//		if(EnableSuperJump)
//		{
//			global.MoveForwardUp();
//		}
//	}

//	exec function MoveBackwardDown()
//	{
//		if(EnableSuperJump)
//		{
//			global.MoveBackwardDown();
//		}
//	}

//	exec function MoveBackwardUp()
//	{
//		if(EnableSuperJump)
//		{
//			global.MoveBackwardUp();
//		}
//	}

//	exec function StrafeLeftDown()
//	{
//		if(EnableSuperJump)
//		{
//			global.StrafeLeftDown();
//		}
//	}

//	exec function StrafeLeftUp()
//	{
//		if(EnableSuperJump)
//		{
//			global.StrafeLeftUp();
//		}
//	}

//	exec function StrafeRightDown()
//	{
//		if(EnableSuperJump)
//		{
//			global.StrafeRightDown();
//		}
//	}

//	exec function StrafeRightUp()
//	{
//		if(EnableSuperJump)
//		{
//			global.StrafeRightUp();
//		}
//	}
//}

//state Landing
//{
//	event BeginState(Name PreviousStateName)
//	{
//		local float aniTime;
//		//Pawn.SetPhysics(PHYS_None);
//		Pawn.ZeroMovementVariables();
//		aniTime = MMOActor(Pawn).PlayAnimation('Landing');
//		SetTimer(aniTime, false, 'TimedPostLanded');
//		bNotifyFallingHitWall = false;
//		//`log("OSCAR:_________ Landing::BeginState" @PreviousStateName);
//	}

//	function TimedPostLanded()
//	{
//		//`log("OSCAR:_________ Landing TimedPostLanded");
//		Pawn.SetPhysics(PHYS_Falling);
//		GotoState('');
//	}

//	event EndState(Name NextStateName)
//	{
//		//bNotifyFallingHitWall = true;
//		//`log("OSCAR:_________ Landing::EndState" @NextStateName);
//	}
//}

//state HitWallEx
//{
//	event BeginState(Name PreviousStateName)
//	{
//		local float aniTime;
//		//Pawn.SetPhysics(PHYS_None);
//		Pawn.ZeroMovementVariables();
//		aniTime = MMOActor(Pawn).PlayAnimation('HitWall');
//		SetTimer(aniTime, false, 'TimedPostHitWall');
//		bNotifyFallingHitWall = false;
//		//`log("OSCAR:_________ HitWall::BeginState" @PreviousStateName);
//	}

//	function TimedPostHitWall()
//	{
//		//`log("OSCAR:_________ HitWall TimedPostHitWall");
//		Pawn.SetPhysics(PHYS_Falling);
//		GotoState('');
//	}

//	event EndState(Name NextStateName)
//	{
//		// TODO: check this comment
//		//bNotifyFallingHitWall = true;
//		//`log("OSCAR:_________ HitWall::EndState" @NextStateName);
//	}
//}
// Lifting object process
//state HoldUpObject
//{
//	event BeginState(Name PreviousStateName)
//	{
		
//	}

//	event EndState(Name NextStateName)
//	{
//	}

//	event PlayerTick( float deltaTime )
//	{
//		local Vector pos;
//		local Rotator rot;

//		local float radius;
//		radius = 52.713181;

//		Pawn.Mesh.GetSocketWorldLocationAndRotation('HoldStone', pos, rot, 0);
		
//		//if(StoneValid != none)
//		//{
//			//`log("OSCAR:__________ Holding");
//			//StoneValid.StaticMeshComponent.PutRigidBodyToSleep();
//			//StoneValid.StaticMeshComponent.SetRBPosition(pos);
//			//StoneValid.StaticMeshComponent.SetRBRotation(rot);
//			//DrawDebugSphere(pos, radius, 32, 255, 0, 0, false);
//		//}
//	}

//	exec function JumpEx()
//	{
//	}
//}
// function SetPawnValue( )
// {
// 	MMOPlayer(Pawn).ActorName = Rolename;
// 	//MMOPlayer(Pawn).m_lv = Rolename;
// }

function StartQTE( IQTEActor towner )
{
	QTEOwner = towner;

	Pawn.ZeroMovementVariables();
	StopLatentExecution();
	GotoState( 'QTE' );
}

function FinishQTE()
{
	if( IsInState( 'QTE' ) )
	{
		GotoState( 'Idle' );
	}
}

event OnAltKeyPressed(bool bPressed)
{
	if( !bPressed )
	{
		QTE_SpaceBar();
	}
}

exec function QTE_SpaceBar()
{
	if( bIsInQTE && QTEOwner != none )
	{
		QTEOwner.NotifyKeyDown();
	}
}

event NotifyQTEStart( QTEType Type, optional bool hideWeapon = false )
{
}

event NotifyQTEEnd( bool Success )
{
}


exec function TraceUnderFoot()
{
	//MMOActor(Pawn).GetMaterialBelowFeet();
}

exec function ShowWater(bool on)
{
	local StaticMeshActor sma;
	foreach AllActors(class'StaticMeshActor', sma)
	{
		if(string(sma.Tag) == "Water")
		{
			sma.SetHidden(!on);
		}
	}
}

// perf test
exec function ShowPlayerCount( bool on )
{
	bShowPlayerCount = on;
}

exec function ShowPlayerSelf()
{
	Pawn.SetHidden(!Pawn.bHidden);
}


exec function ShowNpcAnim(string NpcAction)
{
	local NPC actor;
	`log( "############################################ShowNpcAnim:"@NpcAction);
	if(NpcAction == "DoRebirth")
	{
		foreach AllActors(class'NPC', actor)
		{
			//npc.PlayAnimation('bossskill3');
			actor.DoRebirth();
		}
	}
	else
	{

	}
}

exec function NetTeleportToScene()
{
	TeleportToScene("", vect(0, 0, 0));
}

state HackMove
{
	event PushedState()
	{
		super.PushedState();
	}

	event PoppedState()
	{
		super.PoppedState();
	}

	function PlayerMove(float DeltaTime)
	{
	}
}

event ServerUseTrigger(IServerTrigger Trigger, ETriggerInitState triggerState)
{
	local Sequence GameSeq;
	local array<SequenceObject> AllSeqEvents;
	local array<int> ActivateIndices;
	local int i;
	local SeqEvent_Server_UseTrigger eventObj;

	GameSeq = WorldInfo.GetGameSequence();
	if (GameSeq != None)
	{
		// find any Level Loaded events that exist
		GameSeq.FindSeqObjectsByClass(class'SeqEvent_Server_UseTrigger', true, AllSeqEvents);

		// activate them
		ActivateIndices[0] = triggerState;
		for (i = 0; i < AllSeqEvents.Length; i++)
		{
			eventObj = SeqEvent_Server_UseTrigger(AllSeqEvents[i]);
			`log("OSCAR:!!!!_______ MMOPLayerController ServerUseTrigger:" @eventObj @AllSeqEvents[i]);
			if(eventObj.IsDesiredInstigator(trigger))
			{
				`log("OSCAR:!!!!_______ MMOPLayerController ServerUseTrigger:" @triggerState);
				eventObj.CheckActivate(WorldInfo, Actor(trigger), false, ActivateIndices);
			}
		}
	}
}

event ServerDynamicBlockChange(DynamicBlockingVolume Vol, int bLock)
{
	local Sequence GameSeq;
	local array<SequenceObject> AllSeqEvents;
	//local array<int> ActivateIndices;
	local int i;
	local SeqEvent_Server_DynamicBlockChange eventObj;

	GameSeq = WorldInfo.GetGameSequence();
	if (GameSeq != None)
	{
		// find any Level Loaded events that exist
		GameSeq.FindSeqObjectsByClass(class'SeqEvent_Server_DynamicBlockChange', true, AllSeqEvents);

		// activate them
		//ActivateIndices[0] = block;
		for (i = 0; i < AllSeqEvents.Length; i++)
		{
			eventObj = SeqEvent_Server_DynamicBlockChange(AllSeqEvents[i]);
			//`log("OSCAR:!!!!_______ MMOPLayerController SeqEvent_Server_DynamicBlockChange:" @eventObj @AllSeqEvents[i]);
			if(eventObj.IsDesiredInstigator(vol))
			{
				//`log("OSCAR:!!!!_______ MMOPLayerController SeqEvent_Server_DynamicBlockChange:" @block);
				//eventObj.CheckActivate(WorldInfo, vol, false, ActivateIndices);
				// CHECK:
				eventObj.ForceActivateOutput(block);
			}
		}
	}
}

event ServerTeleportChange(int TeleportID, byte CurrentState)
{
	local Sequence GameSeq;
	local array<SequenceObject> AllSeqEvents;

	local int i;
	local SeqEvent_Server_TeleportOpen eventObj;

	GameSeq = WorldInfo.GetGameSequence();
	if (GameSeq != None)
	{
		// find any Level Loaded events that exist
		GameSeq.FindSeqObjectsByClass(class'SeqEvent_Server_TeleportOpen', true, AllSeqEvents);

		// activate them
		for (i = 0; i < AllSeqEvents.Length; i++)
		{
			eventObj = SeqEvent_Server_TeleportOpen(AllSeqEvents[i]);
			`log("OSCAR:!!!!_______ MMOPLayerController ServerTeleportChange:" @eventObj @AllSeqEvents[i]);
			if(eventObj.IsDesiredInstigator(TeleportID))
			{
				`log("OSCAR:!!!!_______ MMOPLayerController SeqEvent_Server_TeleportOpen:" @TeleportID @CurrentState);
				eventObj.ForceActivateOutput(CurrentState);
			}
		}
	}

}

event ShowEscMenu()
{
	class'MMOUIManager'.static.ShowPanelEx(class'MMOUIManager'.default.Panel_OptionMenu,true, "");
}

exec function SetEngineSpeed(optional int TimeState = 2, float t=0.1 , optional float duration = 20 )
{
	SetTimeFactor( TimeState , t, duration);
}

exec function ShowHitHUDText(bool on)
{
	MMOHud(myHUD).ShowHitHUDText = on;
}

//------------------------------------------------------------------------
// Performance test functions
//------------------------------------------------------------------------
// benchmark function prepare
exec function BFP(optional bool showSkel = true)
{
	if(!showSkel)
	{
		ConsoleCommand("show skelemeshes");
	}

	ConsoleCommand("stat none");
	ConsoleCommand("stat fps");
	ConsoleCommand("stat d3d9rhi");
	ConsoleCommand("stat unit");
	ConsoleCommand("stat memory");
	ConsoleCommand("stat unit");

	ConsoleCommand("toggledrawevents");
	ConsoleCommand("pause");
	ConsoleCommand("freezerendering");
}

exec function wd()
{
	PlayMPlayerAnim();
}

exec function wdx(int Num)
{
	PlayPlayerAnimLoop(Num);
}

exec function nd(int npcID)
{
	PlayAllAnimByNPCID(npcID);
}

exec function ndx(int npcID, int SeqLoop)
{
	PlayNPCAnimLoop(npcID, SeqLoop);
}

exec function td()
{
	StopPlayPNAnim();
}
//Bruce@snda

// benchmark function highend
exec function BFH()
{
	ConsoleCommand("scale highend");
	ConsoleCommand("viewmode lit");
}

// benchmark function highend LightingOnly
exec function BFHLO()
{
	ConsoleCommand("scale highend");
	ConsoleCommand("viewmode LightingOnly");
}

// benchmark function highend Unlit
exec function BFHUL()
{
	ConsoleCommand("scale highend");
	ConsoleCommand("viewmode Unlit");
}

// benchmark function lowend
exec function BFL()
{
	ConsoleCommand("scale lowend");
	ConsoleCommand("viewmode lit");
}

// benchmark function lowend LightingOnly
exec function BFLLO()
{
	ConsoleCommand("scale lowend");
	ConsoleCommand("viewmode LightingOnly");
}

// benchmark function lowend Unlit
exec function BFLUL()
{
	ConsoleCommand("scale lowend");
	ConsoleCommand("viewmode Unlit");
}

// benchmark disable tessellation for water
exec function ToggleWaterTess()
{
	local WaterActor w;
	foreach AllActors(class'WaterActor', w)
	{
		w.DoTessellation = !w.DoTessellation;
	}
}

exec function DebugFrame(bool   bFreeze = true )
{
	ConsoleCommand("toggledrawevents");
	ConsoleCommand("ShowMaterialDrawEvents");
	if( bFreeze )
	{
		ConsoleCommand("FreezeRendering");
		ConsoleCommand("pause");
	}
	ConsoleCommand("set engine bEnableColorClear true");
	
}
//Whether turning is allowed during a combo
exec function SetCanRotWhenCombo( bool bCan )
{
	MMOPlayer(Pawn).CanRotWhenCombo = bCan;
}

//exec function OpenMessageBox()
//{
//	class'MMOUIManager'.static.AddMessageBox("Test Message Box!",111,none,none);
//}

exec function DrawDebugCircle(float radius)
{
	local int x, y;
	local Vector offset, ext;
	DrawDebugCylinder(MMOActor(Pawn).GetFootLocation(), MMOActor(Pawn).GetFootLocation(), radius, 32, 0, 255, 0, true);

	ext.X = radius * 2;
	ext.Y = ext.X;
	ext.Z = 5;

	for(x = 0; x < 3; ++x)	
	{
		for(y = 0; y < 3; ++y)
		{
			offset.X = Pawn.Location.X + (x - 1) * radius * 2 + radius;
			offset.Y = Pawn.Location.Y + (y - 1) * radius * 2 + radius;
			offset.Z = MMOActor(Pawn).GetFootLocation().Z;
			DrawDebugBox(offset, ext, 255, 255, 0, true);
			`log("DrawDebugBox" @ X @ Y @ offset.X @ offset.Y @ ext.X);
		}
	}
}

exec function ClearDrawDebug()
{
	FlushPersistentDebugLines();
}

exec function DebugGotoInGameState()
{
	//class'MMOUIManager'.static.ActiveGameState(EGameState_InGame);
	//MMOGame(WorldInfo.Game).SceneType = EST_Game;
}

exec function ShowUI(name UIName)
{	
	//local MMOUIBase ui;
	//ui = class'MMOUIManager'.static.FindUIPanel(UIName);
	//ui.ToogleVisible();

	class'MMOUIManager'.static.ToggleShowPanelEx(UIName, "");
}

exec function ShowAllUI()
{		
	class'MMOUIManager'.static.ShowAllPanels(CurrentShowAllUI);
	CurrentShowAllUI = !CurrentShowAllUI;
}

// 0~9
exec function OnNumberKey(EKeyCode KeyValue, bool bRelease)
{
	GameKeyEventProcessor.OnNumberKey(KeyValue, bRelease);
}

// a~z
exec function OnCharacterKey(EKeyCode KeyValue, bool bRelease)
{
	GameKeyEventProcessor.OnCharacterKey(KeyValue, bRelease);
}

// blank ,  . / \ = - Enter
exec function OnSymbolKey(EKeyCode KeyValue, bool bRelease)
{
	GameKeyEventProcessor.OnSymbolKey(KeyValue, bRelease);
}

// F1~ F12
exec function OnFunctionKey(EKeyCode KeyValue, bool bRelease)
{
	GameKeyEventProcessor.OnFunctionKey(KeyValue, bRelease);
}

// CTRL, ALT, SHIFT
exec function OnModifierKey(EKeyCode KeyValue, bool bRelease)
{
	GameKeyEventProcessor.OnModifierKey(KeyValue, bRelease);
}

// HOME, END, PGUP, PGDN, Caps Lock, Scroll Lock, Num Lock, Left, Right, Up, Down
exec native final function OnNavigationKey(EKeyCode KeyValue, bool bRelease);

event ForceTriggerPlayerMatineeStopEvent(int outputIndex)
{
    //TriggerGlobalEventClass(class'SeqEvent_PlayerMatineeStop', self);

	local Sequence GameSeq;
	local array<SequenceObject> AllSeqEvents;
	local int i;
	local SeqEvent_PlayerMatineeStop eventObj;

	GameSeq = WorldInfo.GetGameSequence();
	if (GameSeq != None)
	{
		GameSeq.FindSeqObjectsByClass(class'SeqEvent_PlayerMatineeStop', true, AllSeqEvents);
		for (i = 0; i < AllSeqEvents.Length; i++)
		{
			eventObj = SeqEvent_PlayerMatineeStop(AllSeqEvents[i]);
			eventObj.ForceActivateOutput(outputIndex);
		}
	}
}


// ESC, PRSCRN, PAUSE/BREAK, WINDOWS
exec function OnSystemCommandKey(EKeyCode KeyValue, bool bRelease)
{
	GameKeyEventProcessor.OnSystemCommandKey(KeyValue, bRelease);
}

// Mouse Left Key, Mouse Right Key, MouseScrollUp, MouseScrollDown
exec function OnMouseKey(EKeyCode KeyValue, bool bRelease)
{
	GameKeyEventProcessor.OnMouseKey(KeyValue, bRelease);
}

//add by JimZhang@snda.com-------------------------------------------------
native function RequestRevive(int nType,int itemID);

////------------------------------------------------------------------------

native function DoMsgBoxOk( int MsgBoxId, bool agree);
native function DoMsgBoxCannel( int MsgBoxId, bool agree);

function OnMsgBoxOk(GFxObject mc)
{
	local int MsgBoxId;
	MsgBoxId = mc.GetInt("iteminfoid");
	DoMsgBoxOk(MsgBoxId,true);
}

function OnMsgBoxCannel(GFxObject mc)
{
		local int MsgBoxId;
	MsgBoxId = mc.GetInt("iteminfoid");
	DoMsgBoxCannel(MsgBoxId,false);
}


exec function SetMoveSpeed(int spd)
{
	Pawn.GroundSpeed = spd;
}

//Override base class function; originally it would destroy hud
reliable client function ClientSetHUD(class<HUD> newHUDType)
{	
	if( myHUD == none)
	{
		myHUD = (newHUDType != None) ? Spawn(newHUDType, self) : None;
		OnHUDLoaded();
	}
	//SwitchUIPlayer( MMOGame(WorldInfo.Game).SceneType );
	SwitchUIPlayerByMapName( WorldInfo.GetMapName() );
}

// exec function ShowDebugPath()
// {
// 	m_NavHelper._NavigationHandle.bVisualPathDebugging = !m_NavHelper._NavigationHandle.bVisualPathDebugging;
// 	m_NavHelper._NavigationHandle.bUltraVerbosePathDebugging = !m_NavHelper._NavigationHandle.bUltraVerbosePathDebugging;
// }

exec final native function ShowDebugPathTrace();
exec final native function ShowDebugPath();
exec native final function SetPathRadius(int lagMS);
exec native final function SetPathFindZOffset(int offset);

exec function ShowNavMeshDistance()
{
	MMOActor(Pawn).ShowNavmeshDistance = !MMOActor(Pawn).ShowNavmeshDistance;
}

exec function DebugSetCollisionHeight(float halfHeight)
{
	Pawn.SetCollisionSize(Pawn.GetCollisionRadius(), halfHeight);
}


exec function LoadUI()
{
	//MMOHUD(self.myHUD).CreateHUDMovie();
	//MMOHUD(self.myHUD).CreateSplashMovie();
}

exec function ShowPostPPName()
{
//	local int i;
`if (`notdefined(FINAL_RELEASE))
	local LocalPlayer lp;
	lp = LocalPlayer(self.Player);
	`log("Current PostPP:" @ lp.PlayerPostProcess @ WorldInfo.WorldPostProcessChain);
`endif
}

exec function SetDropForceMag( int ForceMag )
{
	MMOGame(WorldInfo.Game).DropForceMag = ForceMag;
}

exec function TestFov()
{
	local MMOPlayerCamera cam;
	cam = MMOPlayerCamera(PlayerCamera);
	
	if(cam != none)
	{
		//`log("TestFov" @ TestCameraFovList.Length);

		if(ToggleCameraFov < TestCameraFovList.Length)
		{
			cam.NearFov = TestCameraFovList[ToggleCameraFov].NearFov;
			cam.FarFov = TestCameraFovList[ToggleCameraFov].FarFov;
			cam.NearCameraPitch = TestCameraFovList[ToggleCameraFov].NearPitchOffset;
			cam.FarCameraPitch = TestCameraFovList[ToggleCameraFov].FarPitchOffset;
			cam.NearCameraDistance = TestCameraFovList[ToggleCameraFov].NearCameraDistance;
			cam.FarCameraDistance = TestCameraFovList[ToggleCameraFov].FarCameraDistance;
			
			cam.DistanceRatio = 1.0;

			ShowCamFOV();
			ShowCamPitchOffset();
			ShowCamDistance();
		}

		++ToggleCameraFov;
		if(ToggleCameraFov >= TestCameraFovList.Length)
		{
			ToggleCameraFov = 0;
		}
	}
}

native final function NativeLogin(string ip, int port, string account, string pwd);
exec native final function LoginSelectRole(int roleid);

exec function Login(string account)//, optional string ip="127.0.0.1", optional int port=8701, optional string pwd="1")
{
	NativeLogin("127.0.0.1", 8701, account, "1");
}

exec function LoginCreateRole(string CharName, int ClassID, int Sex)
{
	CreateRole(CharName, ClassID, Sex);
}

exec function ShowShop(int ShopTemplateID)
{
	class'MMOUIManager'.static.OpenShop(ShopTemplateID);
}


function InitGameStates()
{
	local int i;
	local MMOGameState st;

	`log("__________InitGameStates");

	for(i = 0; i < GameStateClasses.Length; ++i)
	{		
		st = new GameStateClasses[i];
		st.Init();
		class'MMOUIManager'.static.AddGameState(st);
	}

	//Bruce@snda
	if( class'Engine'.static.IsEditor() )
	{
		//MMOGame(WorldInfo.Game).SceneType = EST_Game;
		class'MMOUIManager'.static.ActiveGameState(EGameState_InGame);
	}
	else
	{
		class'MMOUIManager'.static.ActiveGameState(EGameState_Login);
	}
	//Bruce@snda
}

state LoginState
{
	event BeginState(Name PreviousStateName)
	{
		`log("=======LoginState::BeginState");
		Pawn.SetPhysics(PHYS_None);

		// turn off DLE
		if( MMOActor(Pawn).LightEnvironment != none )
		{
			MMOActor(Pawn).LightEnvironment.SetEnabled( false );
		}

		TargetLocationMarker.SetHidden(true);
		PickLocationMarker.SetHidden(true);

		MMOActor(Pawn).DisablePlaceActorToGroundTimer();
	}

	event EndState(Name NextStateName)
	{
		`log("=======LoginState::EndState");
		Pawn.SetPhysics(PHYS_Walking);

		// turn on DLE
		if( MMOActor(Pawn).LightEnvironment != none )
		{
			MMOActor(Pawn).LightEnvironment.SetEnabled( true );
		}

		TargetLocationMarker.SetHidden(false);
		PickLocationMarker.SetHidden(false);
	}

	event PlayerTick( float DeltaTime )
	{
	}

	event ExecOnLeftMouseKey(bool bRelease)
	{		
	} 
	
	exec function OnRightMouseKey(bool bRelease)
	{
	}

	event MoveToPoint(Vector Des)
	{
		`log("=======LoginState::MoveToPoint");
	}
}

exec native final function ActiveRemoteEvent(name eventName);

exec native final function TestDrop(int itemTemplateID);
exec native final function TestDropX(int itemTemplateID, int count, optional int range = 1000);
exec native final function TestDropAll(int dist = 100);
exec native final function TestDropAnim(float g, float iniSpeed, float rps, float rpsAcc, float spawnDelay);
exec native final function TestDropSetParm(string parName, string parVal);
// add by zhangrui05 quest #13020
exec native final function TestDropMoney(int moneyId, int count);

exec native final function TestDoodad(int doodadTemplateID, optional int remainCnt=3);

exec function ShowGridFlag()
{
	bShowGrid = !bShowGrid;
}

// Follow Matinee character
state PlayerMatinee
{
	event BeginState(Name PreviousStateName)
	{
		//`log("---------- Begin Matinee" @ Pawn.Location);
		MMOActor(Pawn).DisablePlaceActorToGroundTimer();
	}

	event EndState(Name NextStateName)
	{
		//`log("---------- End Matinee" @ Pawn.Location);
		MMOActor(Pawn).ResetPlaceActorToGroundTimer();
	}

	event PlayerTick( float deltaTime )
	{
		//`log("---------- PlayerMatinee::PlayerTick" @ Pawn.Location);
	}
}


exec native function TestMatineeSubtitle(bool show, string subtitle);

//For easy testing
exec native function ReloadTween();

exec native function ReloadFreeTips();

// Artists want to turn off LE; this command shows the effect when it is off.
exec function ToggleLightEnvironment()
{
	local MMOPlayer pl;
	pl = MMOPlayer( Pawn );
	pl.LightEnvironment.SetEnabled( !pl.LightEnvironment.IsEnabled() );
}

exec function HideHelmet( bool hide )
{
	local MMOPlayer pl;
	pl = MMOPlayer( Pawn );
	pl.ItemManager.HideHelmet( hide );
}

exec function CamMode(name mode)
{
	SetCameraMode(mode);
}

exec native final function SetHUDNameMoveSpeed(int pix);


native final function NativePreRender(Canvas canv);

event PreRender(Canvas canv)
{
	NativePreRender(canv);
}

exec native final function SetTickOrder(byte order);

exec native final function SetSmoothFrameRate(bool on, optional float minFPS, optional float maxFPS);

exec native final function CreatePlayer(byte playerClass, byte gender, int count, int rangeOffset);

exec native final function DrawNewGlobeFull();
exec native final function DrawNewGlobe();

exec native final function SetMoveModGridSize(int sz);
exec native final function SetNetLagToleranceGridCount(int sz);

exec native final function JumpProg();

native final function OnReturnLoginConfirm();
native final function ExitGameWindow();

function OnDefaultOK()
{
}

event PlayLowHPWarningSound(bool play)
{
	if(play && LowHPWarningSound != none)	
	{
		LowHPWarningSound.Stop();		
		LowHPWarningSound = none;
	}

	if(play && LowHPWarningSound == none)	
	{
		LowHPWarningSound = GetPooledAudioComponent(LowHPWarningSoundCue, Pawn, true, false);		
	}

	if(play && self.LowHPWarningSound != none)	
	{
		LowHPWarningSound.Play();
		return;
	}	
	else if(!play)
	{
		LowHPWarningSound.Stop();
	}
}

exec native final function ShowTopmostPick();
exec native final function PreJump(float x, float y);
exec native final function PreJumpNext();
exec native final function PreJumpEnable();
exec native final function TestDropIconPos(int x, int y);
exec native final function DisWarn();

exec native final function Reload();

defaultproperties
{
	CameraClass=class'MMOPlayerCamera'
	EnableMouseRotCam=true	
	m_fPathFindingFrequency=0.2f
	m_fDeltaPathingTime=0.f
	DebugLocationCount=0
	tempFlag1=false
	tempFlag2=true
	tempFlag3=true
	bUIMask=false

	//NextPlayerAction=none
	
	EnableMouseAutoAttack=false
	WaitingForTheAbilityRespond=0
	WaitingForTheMoveToRespond=0
	WaitingForTheAbilityBuf=0
	WaitingForTheCombatPose=0
	WaitingForEquipMount=0

	//MoveMode=0
	EnableKeyboardMove=true
	//bCurNotDirectAttack=true
	//SkillMap.Add(15)
	//OnReceivedNativeInputKey=InputKey
	bNotifyFallingHitWall=true
	bNotifyPostLanded=true
	bNotifyApex=true

	//EnableInteractUpdate=false
	//EnableSuperJump=true

	//SlideDownSpeed=800
	//WalkWallSpeed=900

	//JumpVelocityScale=2
	//JumpZSpeed=1500
	//JumpDist=400
	//JumpHeight=300
	//MoveOnWallStrengthFallOffSpeed=5
	//PlayerMoveCtrl=none
	
	bIsInQTE=false 
	bIsInBossQTE = false
	bLinearMovement=false
	bOptimizeTargetPos=false

	RecreatePlayer=false
	//DataMap=MMODataMap'MMO_UI.DataMap.DataMap'

	//Repairing=false
	ClientCanUseAbility=true

	bHaveToGetNextWayPoint=false
	ChatBoxActived=false
	StepDistance=250

	MoveModeByKey=0
	fPlayerColisionRatio=0.5
	DesiredActor=none
	bDesiredActor=false

	GameStateClasses[0]=class'MMOGameState_Login'
	GameStateClasses[1]=class'MMOGameState_Loading'
	GameStateClasses[2]=class'MMOGameState_InGame'
	GameStateClasses[3]=class'MMOGameState_Selecting'
	
	PathFindCollisionRadius=96.0f//32.f

	TargetLocationActor=none
	TargetLocationMarkerCanShow=false
	
	bShowPlayerCount=FALSE
	IsInTransform=false

	bShowNpc=true
	bShowMainPlayer=true
	bShowTheOtherPlayersByKismet=true
	bShowTheOtherPlayersByHotKey=true

	bShowDropsByKismet=true

	OnRevive=false
	bIsOperatingDoodad=false
	LowHPWarningSoundCue=SoundCue'Menu.Severe_Danger_Cue'
}
