class MMOGame extends GameInfo
	config(MMOGame)
	dependson(MMOGameState)
	native;

// enum ESceneType
// {
// 	EST_Loading,
// 	EST_Login,
// 	EST_Game,
// };


// General game data

var string GamePlayDataManagerTemplateName;   // for editor

var bool bUseClassicHUD;

//var  EGameState PlayerState ;//put here first add by york, who sets config?

//var transient ESceneType SceneType;

var bool LevelLoaded;
var bool ShowNPCHP;

var MMOPlayer   curPlayer;

var int nCurrentUseItemID;

/** What PlayerController Archetype to use for the Player's Controller -- 
 *  note this is NOT Dynamically Loaded, so it must be referenced somewhere in script, or in the gameplay level, in order
 *  for it to be correctly spawned. */
var config string PlayerControllerArchetypeName;

var config string MonsterDefaultTemplate;

var string      NextMap;

var config string PlayerStartPoint;
var Vector PlayerStartPosition;

var MMOGlobalTimeManager    MyTimeManager;

var public MMOSoundManager SoundManager;

var UIKeyMapManager KeyMapManager;

var int SelectPlayer;

var const private{private} MMOPlayerController MainController;

// Whether to draw ability debug lines
var config public bool DrawAbilityDebugLine;

// Whether to show character center BOX
var bool DrawSelfCenterBox;

// Whether to show debug character state
var config public bool DrawDebugActorState;

var transient SceneCapture2DComponent CaptureComponent;

//Character related config parameters in the editor
var config string EditorPreviewPawnRes;
var config string StaticMeshWeaponName;
var config string WeaponAttachSocketName;

// standard collision data
var config float StandardCollisionHeight;
var config float StandardCollisionRadius;

// show level names for ui
var(UI) string	UIFullBodyLevelName;
var(UI) string	UIHalfBodyLevelName;
var(UI) string	UIHeroBodyLevelName;
var(UI) string	UIMallBodyLevelName;
var(UI) string  UIHandbookBodyLevelName;

var transient LevelStreamingUIShow	UIFullBodyLevel;
var transient LevelStreamingUIShow	UIHalfBodyLevel;
var transient LevelStreamingUIShow	UIHeroBodyLevel;
var transient LevelStreamingUIShow	UIMallBodyLevel;
var transient LevelStreamingUIShow	UIHandbookBodyLevel;

var transient bool		IsLongJumping;
var transient bool		NotifyServerAfterLongJump;
var transient Vector	LongJumpTargetLocation;
var transient bool		JumpInSameScene;

var float DefaultSceneDesaturation;
var config int   DropForceMag;

var string DefaultLocalMapName;
var EGameState NextGameState;

var native private pointer SuspendRenderingThread{class FSuspendRenderingThread};

native final function PreBeginPlayNative();
native final function SetMainController(MMOPlayerController ctrl);
native final function MMOPlayerController GetMainController() const;
native final function FinishLoading2Login();
native final function FinishLoading2Select();

//------------------------------------------------------------
// Network protocol related
//------------------------------------------------------------
native final function SendRequest_FinishLoading();
native final function vector GetCurrentTeleportPosition();

function SetGameSpeed( Float T )
{
	//do noting
}

simulated function PreBeginPlay()
{
	MyTimeManager = Spawn( class'MMOGlobalTimeManager' );

	KeyMapManager = new class'UIKeyMapManager';
	KeyMapManager.init( );

	//`log("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	PreBeginPlayNative();

	Super.PreBeginPlay();

	//class'MMOGameFacade'.static.GetGameFacade().Init();
}


event PostBeginPlay()
{
	super.PostBeginPlay();

	class'GamePlayDataManager'.static.Singleton().InitGamePlayData();

	//SceneType = EST_Login;

	//PlayerState = EGameState_Splash;
	//`log("Now Game State:"@SceneType @ class'Engine'.static.GetDefaultPostProcessChain());
}

event Destroyed()
{
	super.Destroyed();
	//if(GpDataManager != none)
	//{
	//	GpDataManager.UnInitGamePlayData();
	//}
	class'GamePlayDataManager'.static.Singleton().UnInitGamePlayData();
}

event UILevelLoaded( String LevelName )
{
	//todo
}

event PostLogin( PlayerController NewPlayer )
{
	super.PostLogin(NewPlayer);

	MMOPlayerController(NewPlayer).InitGameStates();
}

function GameViewportClient GetViewportClient()
{
	return class'UIRoot'.static.GetCurrentUIController().Outer;
}

event GetSeamlessTravelActorList(bool bToEntry, out array<Actor> ActorList)
{
	super.GetSeamlessTravelActorList(bToEntry, ActorList);
	ActorList.AddItem(self);
	if(GameReplicationInfo != none)
	{
		ActorList.AddItem(GameReplicationInfo);
	}
}

function GenericPlayerInitialization(Controller C)
{
	if ( !bUseClassicHUD )
	{
	//	HUDType = class'UTGFxHudWrapper';//The default value is already set here, so not adding it here for now
	}
	super.GenericPlayerInitialization(C);
}

/** modified to spawn from archetype template, rather than just the class */
function Pawn SpawnDefaultPawnFor(Controller NewPlayer, NavigationPoint StartSpot)
{
	local Rotator StartRotation;
	local Pawn ResultPawn;
	local Pawn PawnArchetypeToUse;
	//local StaticMesh WeaponMesh;

	//ScriptTrace();

	// don't allow pawn to be spawned with any pitch or roll
	StartRotation.Yaw = StartSpot.Rotation.Yaw;

	//`log("IsGame="$class'Engine'.static.IsGame());
	//`log("IsEditor="$class'Engine'.static.IsEditor());
	if( class'Engine'.static.IsGame() && !class'Engine'.static.IsEditor())
	{
	`log("_______ Spawn Player From Archetype:" @
		MMOPlayerController(NewPlayer).PlayerPawnArchetypeName@
		"At"@ StartSpot.Location);
		PawnArchetypeToUse = Pawn(DynamicLoadObject(MMOPlayerController(NewPlayer).PlayerPawnArchetypeName, class'MMOPlayer'));
		ResultPawn = Spawn(class'MMOPlayer',,,StartSpot.Location,StartRotation,none,true);		
	}
	else
	{
		`log("Spawn Editor Preview Pawn:"$EditorPreviewPawnRes);
		PawnArchetypeToUse = Pawn(DynamicLoadObject(EditorPreviewPawnRes,class'MMOPlayer'));
		// PIEPIE weapon lighting is wrong, removed for now to avoid artist misunderstanding
		//WeaponMesh = StaticMesh(DynamicLoadObject(StaticMeshWeaponName,class'StaticMesh'));		
		ResultPawn = Spawn(PawnArchetypeToUse.Class,,,StartSpot.Location,StartRotation,PawnArchetypeToUse,true);
	}

	if ( ResultPawn == None )
	{
		`log("Couldn't spawn player of type "$PawnArchetypeToUse.Class$" at "$StartSpot);
	}

	if(MMOActor(ResultPawn) != none && MMOActor(PawnArchetypeToUse) != none)
	{
		MMOActor(ResultPawn).PawnAnimInfo = MMOActor(PawnArchetypeToUse).PawnAnimInfo;
	}

	if(class'Engine'.static.IsEditor())
	{
		MMOActor(ResultPawn).InitAnimNodes();
		MMOActor(ResultPawn).RefreshAnim();
		//MMOActor(ResultPawn).AddPreviewWeapon(WeaponMesh, name(WeaponAttachSocketName));
		//MMOActor(ResultPawn).AddInteractiveWind();
	}

	SetMainController(MMOPlayerController(NewPlayer));
	curPlayer = MMOPlayer(ResultPawn);

	if(ResultPawn != none && ResultPawn.IsA('MMOActor'))
	{
		MMOActor(ResultPawn).ActorType = EACT_PlayerSelf;
		MMOActor(ResultPawn).SetMainController(MMOPlayerController(NewPlayer));
	}
	
	MMOPlayer(ResultPawn).CheckActorLight();


	return ResultPawn;
}

/** modified to spawn from archetype template, rather than just the class */
function PlayerController SpawnPlayerController(vector SpawnLocation, rotator SpawnRotation)
{
	local PlayerController ControllerArchetypeToUse;
	//ScriptTrace();
	`log("Spawn player controller:[" $PlayerControllerArchetypeName$ "]");
	ControllerArchetypeToUse = PlayerController(DynamicLoadObject(PlayerControllerArchetypeName,class'PlayerController'));
	return Spawn(ControllerArchetypeToUse.Class,,, SpawnLocation, SpawnRotation,ControllerArchetypeToUse);
}

function string GetNextMap()
{
	return NextMap;
}



//native native function  NavigationPoint FindPlayerStart( Controller Player, optional byte InTeam, optional string IncomingName );
event  NavigationPoint FindPlayerStart( Controller Player, optional byte InTeam, optional string IncomingName )
{
	local NavigationPoint temp;

	`Log( "[MMO Game Deubg] FindPlayerStart:" @InTeam @IncomingName );

	if( !WorldInfo.IsPlayInEditor() && PlayerStartPoint != "" )
	{
		ForEach AllActors( class 'NavigationPoint', temp )
		{
			if( string( temp.Name ) == PlayerStartPoint )
			{
			//	PlayerStartPoint = "PlayerStart_0";
				return temp;
			}
		}
	}

	//ForEach AllActors( class 'NavigationPoint', temp )
	//{
	//	PlayerStartPoint = "PlayerStart_0";
	//	return temp;
	//}

	`Log( "[MMO Deubg] No player start spot:"@ PlayerStartPoint );
	return super.FindPlayerStart( Player, InTeam, IncomingName );
}

function LoadMap(string LevelName)
{
	WorldInfo.SeamlessTravel(LevelName);
	`log("_______________ Load Level:" @LevelName);

	//PlayerState = EGameState_Loading;
	//`log("Now Game State:"@PlayerState );
}

native final function NativePostSeamlessTravel();

native final function SetCameraParameters();		

event PostSeamlessTravel()
{
	super.PostSeamlessTravel();

	`log("_______________ PostSeamlessTravel:" @ MainController.MapName);

	//StartMatch();

	WorldInfo.GRI = GameReplicationInfo;

	//`log("Now Game State:"@PlayerState );

	`log("Set PlayerStartPosition="$PlayerStartPosition);
	MainController.Pawn.SetLocation(PlayerStartPosition);

	MainController.Pawn.SetPhysics(PHYS_Walking);

	MainController.BlockNextLevelLoading = true;

	//SendRequest_FinishLoading();

	if(MainController.MapName != DefaultLocalMapName )
	{
		// use long jump, which will block when the scene is loading
		StartLongJump( PlayerStartPosition, true, false );
		//SetTimer(1.5f, false, nameof(NotifyServerFinishedLoading), self);
	}
	//This may go back to Select or Login
	else if( NextGameState == EGameState_Login )
	{
		SetTimer(1.5f, false, nameof(FinishLoading2Login), self);
	}
	else if( NextGameState == EGameState_Selecting )
	{
		SetTimer(1.5f, false, nameof(FinishLoading2Select), self);
	}

	// todo: laten the loading action
	//UIFullBodyLevel = WorldInfo.LoadUILevel( UIFullBodyLevelName, vect( 0.f, 0.f, -5000.f ) );
	//UIHalfBodyLevel = WorldInfo.LoadUILevel( UIHalfBodyLevelName, vect( 0.f, 0.f, -10000.f ) );
	if( UIFullBodyLevel != none )
	{
		`log("WorldInfo.AddUILevel UIFullBodyLevel");
		WorldInfo.AddUILevel( UIFullBodyLevel, vect( 0.f, 0.f, 0.f ) );
	}
	//else
	//{
	//	UIFullBodyLevel = WorldInfo.LoadUILevel( UIFullBodyLevelName, vect( 0.f, 0.f, -5000.f ) );
	//}
	if( UIHalfBodyLevel != none )
	{
		`log("WorldInfo.AddUILevel UIHalfBodyLevel");
		WorldInfo.AddUILevel( UIHalfBodyLevel, vect( 0.f, 0.f, 0.f ) );
	}
	//else
	//{
	//	UIHalfBodyLevel = WorldInfo.LoadUILevel( UIHalfBodyLevelName, vect( 0.f, 0.f, -10000.f ) );
	//}
	if( UIHeroBodyLevel != none )
	{
		`log("WorldInfo.AddUILevel UIHeroBodyLevel");
		WorldInfo.AddUILevel( UIHeroBodyLevel, vect( 0.f, 0.f, 0.f ) );
	}
	if( UIMallBodyLevel != none )
	{
		`log("WorldInfo.AddUILevel UIHeroBodyLevel");
		WorldInfo.AddUILevel( UIMallBodyLevel, vect( 0.f, 0.f, 0.f ) );
	}
	if( UIHandbookBodyLevel != none )
	{
		`log("WorldInfo.AddUILevel UIHandbookBodyLevel");
		WorldInfo.AddUILevel( UIHandbookBodyLevel, vect( 0.f, 0.f, 0.f ) );
	}
	NativePostSeamlessTravel();
}

function LevelStreamingUIShow GetUILevel( string lvName )
{
	if( lvName == UIFullBodyLevelName )
	{
		if( UIFullBodyLevel == none )
		{
			UIFullBodyLevel = WorldInfo.LoadUILevel( UIFullBodyLevelName, vect( 0.f, 0.f, -5000.f ) );
		}
		return UIFullBodyLevel;
	}
	if( lvName == UIHalfBodyLevelName )
	{
		if( UIHalfBodyLevel == none )
		{
			UIHalfBodyLevel = WorldInfo.LoadUILevel( UIHalfBodyLevelName, vect( 0.f, 0.f, -10000.f ) );
		}
		return UIHalfBodyLevel;
	}
	if( lvName == UIHeroBodyLevelName )
	{
		if( UIHeroBodyLevel == none )
		{
			UIHeroBodyLevel = WorldInfo.LoadUILevel( UIHeroBodyLevelName, vect( 0.f, 0.f, -15000.f ) );
		}
		return UIHeroBodyLevel;		
	}
	if( lvName == UIMallBodyLevelName )
	{
		if( UIMallBodyLevel == none )
		{
			UIMallBodyLevel = WorldInfo.LoadUILevel( UIMallBodyLevelName, vect( 0.f, 0.f, -20000.f ) );
		}
		return UIMallBodyLevel;		
	}
	if( lvName == UIHandbookBodyLevelName )
	{
		if( UIHandbookBodyLevel == none )
		{
			UIHandbookBodyLevel = WorldInfo.LoadUILevel( UIHandbookBodyLevelName, vect( 0.f, 0.f, -25000.f ) );
		}
		return UIHandbookBodyLevel;		
	}
	return none;
}

function NotifyServerFinishedLoading()
{
	SendRequest_FinishLoading();
}

function SeamlessTravelPlayers( MMOPlayer oldPawn, MMOPlayer newPawn )
{
	newPawn.ServerID = oldPawn.ServerID;

	newPawn.ActorAttributes.SetHP( oldPawn.ActorAttributes.GetCurHp() );
	newPawn.ActorAttributes.SetMP( oldPawn.ActorAttributes.GetCurMp() );
	newPawn.ActorAttributes.SetMaxHP( oldPawn.ActorAttributes.GetMaxHp() );
	newPawn.ActorAttributes.SetMaxMP ( oldPawn.ActorAttributes.GetMaxMp() );

	//`Log( "[MMO Debug] player hp"@oldPawn.ActorAttributes.HP@newPawn.ActorAttributes.HP );

	//clientAttr->SetHP(serverAttr.dwHP);
	//clientAttr->SetMP(serverAttr.dwMP);
	//clientAttr->SetMaxHP(serverAttr.stAttackProp.dwMaxHP);
	//clientAttr->SetMaxMP(serverAttr.stAttackProp.dwMaxMP);
	//clientAttr->BaseAttackAttribute.MeleeAttackMin = serverAttr.stAttackProp.dwMinPhysicalDamage;
	//clientAttr->BaseAttackAttribute.MeleeAttackMax = serverAttr.stAttackProp.dwMaxPhysicalDamage;

	//clientAttr->BaseAttackAttribute.SpellAttackMin = serverAttr.stAttackProp.dwMinMagicDamage;
	//clientAttr->BaseAttackAttribute.SpellAttackMax = serverAttr.stAttackProp.dwMaxMagicDamage;

	//clientAttr->BaseDefenseAttribute.SpellResistance = serverAttr.stAttackProp.dwMagicDefence;
	//clientAttr->BaseDefenseAttribute.Armor = serverAttr.stAttackProp.dwPhysicalDefence;

	`Log( "[MMO Debug] Exchange player attributes after seamless travel." );
}

event HandleSeamlessTravelPlayer(out Controller C)
{
	local Vector StartLocation;
	local rotator StartRotation;
	local NavigationPoint StartSpot;
	local PlayerController PC, NewPC;
	//local PlayerReplicationInfo OldPRI;
	local Pawn oldPawn;
	local LocalPlayer LP;


	ScriptTrace();
	`log(">> MMOGame::HandleSeamlessTravelPlayer:" @ C,,'SeamlessTravel');

	//if( UIFullBodyLevel != none )
	//{
		//WorldInfo.RemoveUILevel( UIFullBodyLevel );
		//UIFullBodyLevel = none;
	//}
	//if( UIHalfBodyLevel != none )
	//{
		//WorldInfo.RemoveUILevel( UIHalfBodyLevel );
		//UIHalfBodyLevel = none;
	//}

	PC = PlayerController(C);
	if (PC != None && PC.Class != PlayerControllerClass)
	{
		if (PC.Player != None)
		{
			ScriptTrace();
			`log("Spawn new player controller!!!!!!");
			// we need to spawn a new PlayerController to replace the old one
			NewPC = SpawnPlayerController(PC.Location, PC.Rotation);
			if (NewPC == None)
			{
				`Warn("Failed to spawn new PlayerController for" @ PC.GetHumanReadableName() @ "(old class" @ PC.Class $ ")");
				PC.Destroy();
				return;
			}
			else
			{
				PC.CleanUpAudioComponents();
				PC.SeamlessTravelTo(NewPC);
				NewPC.SeamlessTravelFrom(PC);
				SwapPlayerControllers(PC, NewPC);
				PC = NewPC;
				C = NewPC;
			}
		}
		else
		{
			PC.Destroy();
		}
	}
	else
	{
		//// clear out data that was only for the previous game
		//C.PlayerReplicationInfo.Reset();
		//// create a new PRI and copy over info; this is necessary because the old gametype may have used a different PRI class
		//OldPRI = C.PlayerReplicationInfo;
		//C.InitPlayerReplicationInfo();
		//OldPRI.SeamlessTravelTo(C.PlayerReplicationInfo);
		//// we don't need the old PRI anymore
		////@fixme: need a way to replace PRIs that doesn't cause incorrect "player left the game"/"player entered the game" messages
		//OldPRI.Destroy();
	}

	//// get rid of team if this is not a team game
	//if (!bTeamGame && C.PlayerReplicationInfo.Team != None)
	//{
	//	C.PlayerReplicationInfo.Team.Destroy();
	//	C.PlayerReplicationInfo.Team = None;
	//}

	// Find a start spot.
	StartSpot = FindPlayerStart(C, C.GetTeamNum());

	//if (StartSpot == None)
	//{
	//	`warn(GameMessageClass.Default.FailedPlaceMessage);
	//	`warn("========== no start spot, use PlayerStartPosition" @PlayerStartPosition @GetCurrentTeleportPosition());
	//	C.ClientSetLocation(GetCurrentTeleportPosition() + vect(0,0,80), StartRotation);		
	//}
	//else
	//{
	//	StartRotation.Yaw = StartSpot.Rotation.Yaw;
	//	//C.ClientSetLocation(StartSpot.Location, StartRotation);		
	//	`warn("========== find start spot, use PlayerStartPosition" @PlayerStartPosition @GetCurrentTeleportPosition());
	//	C.SetLocation(GetCurrentTeleportPosition() + vect(0,0,80));	
	//	C.Pawn.SetLocation(GetCurrentTeleportPosition());
	//	C.SetRotation(StartRotation);
	//}

	StartLocation = GetCurrentTeleportPosition();
	if( StartSpot != none )
		StartRotation.Yaw = StartSpot.Rotation.Yaw;

	C.ClientSetLocation( StartLocation, StartRotation );
	if( MMOPlayerController(PC).RecreatePlayer && C.Pawn != none )
	{
		oldPawn = C.Pawn;
		C.UnPossess();
		C.Pawn = SpawnDefaultPawnFor( C, StartSpot );
		//MMOPlayerController(PC).SetPawnValue( );
		SeamlessTravelPlayers( MMOPlayer(oldPawn), MMOPlayer(C.Pawn) );
		C.Possess( C.Pawn, false );

		oldPawn.Destroy();

		MMOPlayerController(PC).RecreatePlayer = false;
	}

	if( C.Pawn != none )
	{
		if( MMOActor( C.Pawn ).ActorAttributes == none )
		{
			`Log( "[MMO Debug] actor attributes is NULL during scene travel." );
			MMOActor( C.Pawn ).ActorAttributes = new(C.Pawn) class'ActorAttribute';
		}
		C.Pawn.SetLocation( StartLocation );
		C.Pawn.SetRotation( StartRotation );
		C.Pawn.SetPhysics(PHYS_Walking);
	}
	else
	{
		`Log( "[MMO Debug] no pawn during scene travel!" );
		C.SetLocation( StartLocation );
		C.SetRotation( StartRotation );
	}

	C.StartSpot = StartSpot;

	if (PC != None)
	{
		PC.CleanUpAudioComponents();

		// tell the player controller to register its data stores again
		PC.ClientInitializeDataStores();

		SetSeamlessTravelViewTarget(PC);
		if (PC.PlayerReplicationInfo.bOnlySpectator)
		{
			//PC.GotoState('Spectating');
			PC.PlayerReplicationInfo.bIsSpectator = true;
			PC.PlayerReplicationInfo.bOutOfLives = true;
			NumSpectators++;
		}
		else
		{
			NumPlayers++;
			NumTravellingPlayers--;
			//PC.GotoState('Idle');
		}
	}
	else
	{
		NumBots++;
		//C.GotoState('RoundEnded');
	}

	SetMainController(MMOPlayerController(C));
	MMOPlayer(MainController.Pawn).CheckActorLight();
	GenericPlayerInitialization(C);
	C.Pawn.SetPhysics(PHYS_Walking);

	// To fix custom post processing chain when not running in editor or PIE.
	if( PC != none )
	{
		LP = LocalPlayer(PC.Player); 
		if(LP != None) 
		{
			//`Log ( "Local player set chain:"@LP.Outer.GetWorldPostProcessChain() );
			LP.RemoveAllPostProcessingChains(); 
			LP.InsertPostProcessingChain(LP.Outer.GetWorldPostProcessChain(),INDEX_NONE,true); 
		} 
	}

	`log("<< MMOGame::HandleSeamlessTravelPlayer:" @ C,,'SeamlessTravel');
}

function StartHumans()
{
	local PlayerController P;
//	local LocalPlayer lp;

	foreach WorldInfo.AllControllers(class'PlayerController', P)
	{
		if (P.Pawn == None)
		{
			if ( bGameEnded )
			{
				return; // telefrag ended the game with ridiculous frag limit
			}
			else if (P.CanRestartPlayer())
			{
				RestartPlayer(P);
			}
		}
		//if( P.Pawn != None )
		//{
		//	lp = LocalPlayer(P.Player);
		//	if( lp != none )
		//	{
		//		lp.RemoveAllPostProcessingChains();
		//		lp.InsertPostProcessingChain( lp.Outer.GetWorldPostProcessChain(),INDEX_NONE,true);
		//	}
		//}
	}
}

function RestartPlayer(Controller NewPlayer)
{
	local NavigationPoint startSpot;
	local int TeamNum, Idx;
	local array<SequenceObject> Events;
	local SeqEvent_PlayerSpawned SpawnedEvent;
	//local LocalPlayer LP; 
	//local PlayerController PC; 

	`log( "<< MMOGame:RestartPlayer: " @NewPlayer,,'SeamlessTravel' );

	bRestartLevel = false;

	// super.RestartPlayer
	if( bRestartLevel && WorldInfo.NetMode!=NM_DedicatedServer && WorldInfo.NetMode!=NM_ListenServer )
	{
		`warn("bRestartLevel && !server, abort from RestartPlayer"@WorldInfo.NetMode);
		return;
	}
	// figure out the team number and find the start spot
	TeamNum = ((NewPlayer.PlayerReplicationInfo == None) || (NewPlayer.PlayerReplicationInfo.Team == None)) ? 255 : NewPlayer.PlayerReplicationInfo.Team.TeamIndex;
	StartSpot = FindPlayerStart(NewPlayer, TeamNum);

	// if a start spot wasn't found,
	if (startSpot == None)
	{
		// check for a previously assigned spot
		if (NewPlayer.StartSpot != None)
		{
			StartSpot = NewPlayer.StartSpot;
			`warn("Player start not found, using last start spot");
		}
		else
		{
			// otherwise abort
			`warn("Player start not found, failed to restart player");
			return;
		}
	}
	// try to create a pawn to use of the default class for this player
	if (NewPlayer.Pawn == None)
	{
		NewPlayer.Pawn = SpawnDefaultPawnFor(NewPlayer, StartSpot);
	}
	if (NewPlayer.Pawn == None)
	{
		`log("failed to spawn player at "$StartSpot);
		NewPlayer.GotoState('Dead');
		if ( PlayerController(NewPlayer) != None )
		{
			PlayerController(NewPlayer).ClientGotoState('Dead','Begin');
		}
	}
	else
	{
		//`Log( "[MMO Debug] Player location before teleport:"@ NewPlayer.Pawn.Location );

		// initialize and start it up
		NewPlayer.Pawn.SetAnchor(startSpot);
		if ( PlayerController(NewPlayer) != None )
		{
			PlayerController(NewPlayer).TimeMargin = -0.1;
			startSpot.AnchoredPawn = None; // SetAnchor() will set this since IsHumanControlled() won't return true for the Pawn yet
		}
		NewPlayer.Pawn.LastStartSpot = PlayerStart(startSpot);
		NewPlayer.Pawn.LastStartTime = WorldInfo.TimeSeconds;
		NewPlayer.Possess(NewPlayer.Pawn, false);
		NewPlayer.Pawn.PlayTeleportEffect(true, true);
		NewPlayer.ClientSetRotation(NewPlayer.Pawn.Rotation, TRUE);

		if (!WorldInfo.bNoDefaultInventoryForPlayer)
		{
			AddDefaultInventory(NewPlayer.Pawn);
		}
		SetPlayerDefaults(NewPlayer.Pawn);

		// activate spawned events
		if (WorldInfo.GetGameSequence() != None)
		{
			WorldInfo.GetGameSequence().FindSeqObjectsByClass(class'SeqEvent_PlayerSpawned',TRUE,Events);
			for (Idx = 0; Idx < Events.Length; Idx++)
			{
				SpawnedEvent = SeqEvent_PlayerSpawned(Events[Idx]);
				if (SpawnedEvent != None &&
					SpawnedEvent.CheckActivate(NewPlayer,NewPlayer))
				{
					SpawnedEvent.SpawnPoint = startSpot;
					SpawnedEvent.PopulateLinkedVariableValues();
				}
			}
		}
	}

	NewPlayer.Pawn.SetLocation( StartSpot.Location );
	NewPlayer.Pawn.SetRotation( StartSpot.Rotation );
	`Log( "[MMO Debug] Start position:"@ startSpot.Location );
	
	//// To fix custom post processing chain when not running in editor or PIE.
	//PC = PlayerController(NewPlayer);
	//LP = LocalPlayer(PC.Player); 
	//if(LP != None) 
	//{
	//	`Log ( "Local player set chain:"@LP.Outer.GetWorldPostProcessChain() );
	//	LP.RemoveAllPostProcessingChains(); 
	//	LP.InsertPostProcessingChain(LP.Outer.GetWorldPostProcessChain(),INDEX_NONE,true); 
	//} 

	//`Log( "[MMO Debug] Player location after teleport:"@ NewPlayer.Pawn.Location );
}

native function EnableDeathEffects( bool bEnable );

event NPC SpawnDefaultMonster(Vector Loc, Rotator Rot)
{
	local NPC Ret;
	local NPC NPCArchetypeToUse;

	NPCArchetypeToUse = NPC(DynamicLoadObject(MonsterDefaultTemplate,class'NPC'));
	Ret = Spawn(NPCArchetypeToUse.Class,,, Loc, Rot, none, true);
	return Ret;
}

native final function NativeResetLevel();

function ResetLevel()
{
	super.ResetLevel();
	NativeResetLevel();
}

native function StartLongJump( Vector TargetLocation, bool NotifyServerWhenFinish, bool SameScene );
native function EndLongJump();

/**
* Allow do something right after async loading finishes
*/
native function NotifyAsyncLoadingFinished();

DefaultProperties
{
	ShowNPCHP=false

	bUseClassicHUD=false
	curPlayer=none

	nCurrentUseItemID=100

	LevelLoaded=false

	bDelayedStart=false

	DefaultPawnClass=class'MMOGame.MMOPlayer'
	PlayerControllerClass=class'MMOGame.MMOPlayerController'
	GameReplicationInfoClass=class'MMOGame.MMOGameReplicationInfo'


	KeyMapManager=none

	//Settings UI class
    HUDType=class'MMOGame.MMOHud'

	//sSoundManager=MMOSoundManager'MMOSoundManager.SoundManagerConfig'

	GamePlayDataManagerTemplateName="GamePlayData.GamePlayData"

	//DrawAbilityDebugLine=true
	//DrawDebugActorState=true
	SelectPlayer=-1
	//CurPc=none
	//bTravel = true

	UIFullBodyLevelName="PawnShowLevel"
	UIHalfBodyLevelName="HalfShowLevel"
	UIHeroBodyLevelName="PawnShowLevel_Hero"
	UIMallBodyLevelName="PawnShowLevel_Mall"
	UIHandbookBodyLevelName="PawnShowLevel_Handbook"

	IsLongJumping=false
	NotifyServerAfterLongJump=false
	JumpInSameScene=false
}
