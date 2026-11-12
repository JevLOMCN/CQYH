//
// Main screen HUD class
//
class MMOHud extends HUD
	dependson(MMOPlayerController)
	config(MMOPlayerController)
	native(UI);

// Version number
//var string ProductVersion;
//Whether to show damage info
var config bool ShowHitHUDText;
var config int DamageRandomRange;

// A font rendering class, can be controlled to change fonts
var FontRenderInfo	TextRenderInfo;

// Controls whether to show mouse trace debug line.
var bool	DrawDebugTraces;

/** The most-recent mouse deprojection World direction, updated every frame in PostRender. */
var Vector MouseProjectionDirection;

/** The most-recent mouse deprojection World origin, updated every frame in PostRender. */
var Vector MouseProjectionOrigin;

var class<MMOUIGFXPlayer> UIGFXPlayerClass;
//var class<MMOGFXLoadPlayer> LoadGFXPlayerClass;
var MMOUIGFXPlayer   HudMovie;

var float fDeltaTime;


var array<MMOGFXPlayer> AllMoviePlayers;

var init array<IMMOActorHUD> ActorHUDList;

// max number of hit info that can be displayed in one frame
var int	MaxHitNumber;
var float DefualtDamageTextLifespan;


struct native HitStats
{
	var DamageResult dmg;
	var vector HitLocation;

};

enum EGainType
{
	EGainType_Money,
	EGainType_Exp,
};

struct native GainInfo
{
	var EGainType Type;
	var int SubType;
	var int Value;
	var vector Location;
};

var array<float> DamageTextLifeSpan;

var array<HitStats> thisHit;
var array<GainInfo> GainInfoList;

var array<delegate<OnPostRender> >    vecOnPostRender;
static final  delegate              OnPostRender(Canvas cvs, float fDelta);
native static final function        AddPostRenderDelegate(delegate<OnPostRender> fun);

native function AddGainInfo( EGainType Type , int Value , vector vLocation , optional int subType = 0 );

// if successed return true
native function bool AddNewDamgeTextNode();

native function UpdateDamgeTextNode(FLOAT DeltaSeconds);

native function AddHitStats(DamageResult dmg,vector HitLocation);


//event AddHitStats(DamageResult dmg,vector HitLocation)
//{

//	local HitStats temp ;
	
//	temp.dmg = dmg;
//	temp.HitLocation = HitLocation;

//	thisHit.AddItem(temp);
	
//}

function AddHitInfo(int Value, bool bCritical, vector HitLocation)
{
	
}

event Tick( float DeltaTime)
{
	fDeltaTime = DeltaTime;

	UpdateDamgeTextNode(DeltaTime);
}

function ShowHitStates()
{
	// commended by fangruyi, as now do not know show what
	
	Local int i;
	local Vector screenPos;
	local Vector2D Screensize;
	local int num;
	num = Min(5,thisHit.Length);
	
	for( i = 0 ; i < num ; i++ )
	{
		screenPos = Canvas.Project(thisHit[i].HitLocation);
		
		MMOGame( WorldInfo.Game ).GetViewportClient().GetViewportSize(Screensize);
		
		screenPos.X = screenPos.X + Rand(DamageRandomRange) -DamageRandomRange/2;
		screenPos.Y = screenPos.Y + Rand(DamageRandomRange) -DamageRandomRange/2;

		if( thisHit[i].dmg.HPChange > 0 )
		{
			class'MMOUIManager'.static.ShowHPRestore( screenPos.X , screenPos.y , thisHit[i].dmg.HPChange ,thisHit[i].HitLocation);
		}
		else
		{
			switch(thisHit[i].dmg.Type)
			{
				case DRT_Result7: //Critical hit
					if( thisHit[i].dmg.MainRole )				
					{
						class'MMOUIManager'.static.ShowCriticaledNumber(  screenPos.X, screenPos.y , Abs( thisHit[i].dmg.HPChange ),thisHit[i].HitLocation );
					}
					else
					{
						class'MMOUIManager'.static.ShowCriticalNumber(  screenPos.X, screenPos.y , Abs( thisHit[i].dmg.HPChange ) ,thisHit[i].HitLocation);
					}
					break;
				case DRT_Result2: //Immune
					class'MMOUIManager'.static.ShowImmunity( screenPos.X, screenPos.y ,thisHit[i].HitLocation);
					break;
				case DRT_Result1: //Miss
					class'MMOUIManager'.static.ShowMiss( screenPos.X, screenPos.y ,thisHit[i].HitLocation);
					break;
				case DRT_Result3: //Block
					class'MMOUIManager'.static.ShowBlockNumber( screenPos.X, screenPos.y , Abs( thisHit[i].dmg.HPChange ) ,thisHit[i].HitLocation);
					break;
				case DRT_Result5: //Dodge
					if( thisHit[i].dmg.MainRole )
					{
						class'MMOUIManager'.static.ShowDodge( screenPos.X, screenPos.y ,0,thisHit[i].HitLocation);
					}
					else
					{
						class'MMOUIManager'.static.ShowDodge( screenPos.X, screenPos.y ,1,thisHit[i].HitLocation);
					}
					break;
				case DRT_Result4: //Parry
					class'MMOUIManager'.static.ShowParry( screenPos.X, screenPos.y ,thisHit[i].HitLocation);
					break;
				case DRT_Result6:
					if( thisHit[i].dmg.MainRole )				
					{
						class'MMOUIManager'.static.ShowAttackedNumber(  screenPos.X, screenPos.y , Abs( thisHit[i].dmg.HPChange ) ,thisHit[i].HitLocation);
					}
					else
					{
						class'MMOUIManager'.static.ShowAttackNumber(  screenPos.X , screenPos.y , Abs( thisHit[i].dmg.HPChange ) ,thisHit[i].HitLocation);
					}
					break;
				case DRT_Result0:
					//if( thisHit[0].dmg.HPChange > 0 )
					//{
					//	class'MMOUIManager'.static.ShowHPRestore( screenPos.X , screenPos.y , thisHit[0].dmg.HPChange );
					//}
					break;
			}
		}
	}
	
	if(num>0)
	{
		thisHit.Remove(0, num);
	}
}

final native function NativePostBeginPlay(LocalPlayer LocPlayer);

event MMOGFXPlayer CreateMoviePlayer(class<MMOGFXPlayer> ClassType)
{
	local MMOGFXPlayer moviePlayer;
	//`Log("[MMO]: new CreateMoviePlayer!");
	moviePlayer = new ClassType;
	
	//`Log("[MMO]: CreateMoviePlayer.SetTimingMode!");
	//player.SetTimingMode(TM_Real);

	//`Log("[MMO]: CreateMoviePlayer.Init!");
	//player.Init(class'Engine'.static.GetEngine().GamePlayers[0]);
	//`Log("[MMO]: AllMoviePlayers.AddItem(player);");
	AllMoviePlayers.AddItem(moviePlayer);
	return moviePlayer;
}

// event DeleteMoviePlayer(MMOGFXPlayer deletePlayer)
// {
// 	local MMOGFXPlayer player;
// 	foreach AllMoviePlayers( player )
// 	{
// 		if(player == deletePlayer)
// 		{
// 			AllMoviePlayers.RemoveItem(player);
// 			break;
// 		}
// 	}
// }

// Initialization function
simulated event PostBeginPlay()
{
	super.PostBeginPlay();

	`Log("[MMO]: HUD Init");

	//if(!class'WorldInfo'.static.IsPlayInEditor())
	//{

		`Log("[MMO]: CreateHUDMovie");
		//CreateHUDMovie();

		NativePostBeginPlay(class'Engine'.static.GetEngine().GamePlayers[HudMovie.LocalPlayerOwnerIndex]);

		//`Log("[MMO]: CreateSplashMovie");
		//CreateSplashMovie();		
	//}

	`Log("[MMO]: HUD Init End!");
	//HudMovie.EnterUIPlayerMode( EGameState_Splash);
	//SplashMovie.EnterUIPlayerMode( EGameState_Splash);
	//MMOPlayerController(PlayerOwner).SwitchUIPlayer(EGameState_Splash);
}

// function CreateSplashMovie()
// {
// 	//return;
// 
// 	if( SplashMovie != none)
// 		return;
// 
// 	`Log("[MMO]: new UISplashPlayerClass!");
// 	SplashMovie = new UISplashPlayerClass;
// 	
// 	`Log("[MMO]: SplashMovie.SetTimingMode!");
// 	SplashMovie.SetTimingMode(TM_Real);
// 
// 	`Log("[MMO]: SplashMovie.Init!");
// 	SplashMovie.Init(class'Engine'.static.GetEngine().GamePlayers[SplashMovie.LocalPlayerOwnerIndex]);
// }

native function PostCreateUI();

function CreateHUDMovie()
{
	//local MMOPlayer A;
	//local bool bMale;
	
	
	if( HudMovie != none)
		return;
	
	`Log("[MMO]: new UIGFXPlayerClass!");

	HudMovie = new UIGFXPlayerClass;

	`Log("[MMO]: SetTimingMode!");
	//HudMovie.SetTimingMode(TM_Real);

	`Log("[MMO]: HudMovie.Init!");
	HudMovie.Init(class'Engine'.static.GetEngine().GamePlayers[HudMovie.LocalPlayerOwnerIndex]);
	
	HudMovie.SetController(PlayerOwner);
	//`log("CreateHUDMovie"@HudMovie);
	PostCreateUI();
	
	//bShowHUD = true;
	
}

// function CreateLoadMovie()
// {
// 	if( LoadMovie != none)
// 		return;
// 	LoadMovie = new LoadGFXPlayerClass;
// 	LoadMovie.SetTimingMode(TM_Real);
// 	LoadMovie.Init(class'Engine'.static.GetEngine().GamePlayers[HudMovie.LocalPlayerOwnerIndex]);
// 
// }

function ShowLoadMovie( bool show)
{
	if( show )
	{
		//SplashMovie.Close(true);
		//SplashMovie.Close(false);
		//SplashMovie.SetUIPlayerVisible(false);
		//close all panel when loading scene
		//class'MMOUIManager'.static.ShowAllPanels( FALSE );

		HudMovie.Close();
		//HudMovie.SetUIPlayerVisible(false);
	}
// 	if( LoadMovie == none)
// 	{
// 		CreateLoadMovie();
// 	}
// 	if( LoadMovie !=none)
// 		LoadMovie.SetVisible(show);
}

// Return the mouse screen coordinates in the current scene
function vector2D GetMouseCoordinates()
{
	local Vector2D mousePos;
	mousePos = MMOGame( WorldInfo.Game ).GetViewportClient().GetMousePosition();
	return mousePos;
}

//function vector2D GetMouseCoordinates()
//{
//      local Vector2D mousePos;
//      local UIInteraction UIController;
//      local GameUISceneClient GameSceneClient;
//      UIController  = PlayerOwner.GetUIController();
//      if ( UIController != None)
//      {
//            GameSceneClient = UIController.SceneClient;
//            if ( GameSceneClient != None )
//            {
//                  mousePos.X = GameSceneClient.MousePosition.X;
//                  mousePos.Y = GameSceneClient.MousePosition.Y; 
//            }
//      }
//      return mousePos;
//}

// Draw HUD

native function DrawHUD();
// {
//    local int i;
//    //Define a string constant
// 
//   // local MMOPlayerController pc;
// 
//   //local vector ScreenPos;
// 
//    super.DrawHUD();
// 
//    NativeDrawHUD();
// 
// /*
//    pc = MMOPlayerController(PlayerOwner);
//    //cam = MMOPlayerCamera(pc.PlayerCamera);
//    
//    //Show what kind of class object the control class's current mouse position points to
//    //(TraceActor(TraceActor is already assigned in the PostRender method)
// 	
// 
// 	//Canvas.SetPos( 150, 300 );
// 	//Canvas.DrawText(PlayerOwner.GetStateName());   
//    
//    //Render the StringMessage content at x=250,y=10
//    //Canvas.Font =  Font'EngineFonts.SmallFont';
//    //Canvas.DrawColor = MakeColor(255,183,11,255);
//    //Canvas.SetPos( 100, 10 );
//    //Canvas.DrawText( StringMessage, false, , , TextRenderInfo );
// 
//    // Draw version number
//    //Canvas.DrawColor = MakeColor(255, 255, 255, 255);
//    //Canvas.Font = class'Engine'.Static.GetSmallFont();
//    //Canvas.SetPos( 10, Canvas.SizeY - 20 );
//    //Canvas.DrawText( ProductVersion, false, , , TextRenderInfo );
// */
// }

native function DrawHUDPostGFx();
// {
// 	local int i;
// 	super.DrawHUDPostGFx();
// 	for(i = 0; i < ActorHUDList.Length; ++i)	
// 	{
// 		ActorHUDList[i].DrawHUDPostGFx(self);
// 	}
// }

function InitPackage()
{

	
}

native function Actor PickActors(Vector StartTrace, Vector EndTrace);
native function ShowSilhouette(Actor actor, bool on, optional vector silhouettecolor=vect(1,0,0));
native function NativePostRender();
native function SetWidgetInCenter( GFxObject Obj);

// Post-render processing, draw the actual data here
event PostRender()
{

	local MMOPlayerController IsoPlayerController;
	//local IAbilityDamageable destructActor;
	//local Actor oldTraceActor;
	local int i;
	local delegate<OnPostRender> delegateOnPostRender;
	//local Actor TracedActor;

	super.PostRender();

	if( !bShowHud )
		return;

 	IsoPlayerController = MMOPlayerController(PlayerOwner);
	if(IsoPlayerController != none)
	{
		//`log("________ TraceActor" @IsoPlayerController.TraceActor);
	
	}

// 	if(SplashMovie!=none &&SplashMovie.MPVisible )
// 	{
// 		SplashMovie.TickHud( fDeltaTime ); // TODO: native
// 	}

	//if( HudMovie != none&& HudMovie.MPVisible)
	if( HudMovie != none )
	{
		HudMovie.TickHud( Canvas , fDeltaTime ); // TODO: native
	}

	for( i=0; i < vecOnPostRender.Length; ++i )
	{
		delegateOnPostRender = vecOnPostRender[i];
		delegateOnPostRender( Canvas , fDeltaTime );
	}
	//Begin rendering state text
	//DrawHUD(); // TODO: native
	if(DrawDebugTraces)
	{
		//Parent class can show the pathfinding route
		if(PlayerOwner.ViewTarget != none)
		{
			super.DrawRoute(Pawn(PlayerOwner.ViewTarget));
		}

		DrawTraceDebugRays();
	}
	//DrawCursor();
/*    directly draw CaptureScene picture on the screen
	if(MMOGame(WorldInfo.Game) != none && MMOGame(WorldInfo.Game).CaptureComponent.TextureTarget != none)
	{
		ShowTexture =	MMOGame(WorldInfo.Game).CaptureComponent.TextureTarget;
		Canvas.SetPos(0, 0);
		col.A = 1;
		col.R = 1;
		col.G = 1;
		col.B = 1;
		Canvas.DrawTile(ShowTexture, ShowTexture.GetSurfaceWidth(), ShowTexture.GetSurfaceHeight(), 0, 0, ShowTexture.GetSurfaceWidth(), ShowTexture.GetSurfaceHeight(),col, false, BLEND_Opaque); 
		//Canvas.DrawTexture(ShowTexture, 1.0f);
	}
*/

	//Start Start: outside the game the following handling should not be needed for now and would produce lots of Warnings hemu@snda
	//@todo @todo split into State handling
	if( !class'Engine'.static.IsEditor() &&
		class'MMOUIManager'.static.GetCurrentGameState() != EGameState_InGame )
	{
		return;	
	}

	//oldTraceActor = IsoPlayerController.TraceActor;
	NativePostRender();
	//@debug
	if(IServerTrigger(IsoPlayerController.GetTraceActor()) != none )
			`Log("Touched, ServerTigger"@IsoPlayerController.GetTraceActor().Name);
	//destructActor = IAbilityDamageable(IsoPlayerController.TraceActor);
	

	
	//}
	ShowHitStates();
	//End End: outside the game the following handling should not be needed for now and would produce lots of Warnings hemu@snda
	ShowGainInfoUI();
}

function ShowGainInfoUI()
{
	local Vector screenPos;
	//local Vector2D Screensize;
	if( GainInfoList.Length >0)
	{
		screenPos = Canvas.Project(GainInfoList[0].Location);

		//MMOGame( WorldInfo.Game ).GetViewportClient().GetViewportSize(Screensize);
		screenPos.X = screenPos.X + Rand(DamageRandomRange) -DamageRandomRange/2;
		screenPos.Y = screenPos.Y + Rand(DamageRandomRange) -DamageRandomRange/2;
		   
		switch(GainInfoList[0].Type)
		{
		case EGainType_Money: 
			class'MMOUIManager'.static.ShowGetMoney (  screenPos.X, screenPos.y , Abs( GainInfoList[0].Value ) , GainInfoList[0].SubType ,GainInfoList[0].Location);
			break;
		case EGainType_Exp: 
			class'MMOUIManager'.static.ShowExpNumber (  screenPos.X, screenPos.y , Abs( GainInfoList[0].Value ) , GainInfoList[0].Location);
			break;
		}

		//thisHit.show = false;
		GainInfoList.Remove(0,1);
		
	}
}

// Toggle Show mouse trace debug line
exec function ToggleIsometricDebug()
{
      DrawDebugTraces = !DrawDebugTraces;
      if(DrawDebugTraces)
      {
            `Log("Show mouse trace debug line");
      }
      else
      {
            `Log("Hide mouse trace debug line");
      }
}

// Draw debug mouse ray
function DrawTraceDebugRays()
{
      local MMOPlayerController IsoPlayerController;
      IsoPlayerController = MMOPlayerController(PlayerOwner);

      //Draw selection range.
      //MakeColorMakeColor parameters are R, G, B and the alpha channel.
      Draw3DLine(IsoPlayerController.StartTrace, IsoPlayerController.EndTrace, MakeColor(255,128,128,255));
      //Draw move route
      Draw3DLine(IsoPlayerController.PawnEyeLocation, IsoPlayerController.MouseHitWorldLocation, MakeColor(0,200,255,255));
}

// Draw mouse
function DrawCursor()
{
	//local UIInteraction UIController;
 //   UIController  = PlayerOwner.GetUIController();

	//Canvas.SetDrawColor(255,255,255,255);
	//Canvas.SetPos(UIController.SceneClient.MousePosition.x, UIController.SceneClient.MousePosition.y);
	//Canvas.DrawTexture(Texture2D'DunDefMisc.Menus.CursorIcon',1);

}

/** Checks whether a box of a given width or height is within the canvas bounds. */
native function bool IsOnCanvas(Vector screenPos, float width, float height);
//{
//	return !(screenPos.X < Canvas.OrgX-width/2 || screenPos.Y < Canvas.OrgY-height/2 || screenPos.X > Canvas.ClipX+width/2 || screenPos.Y > Canvas.ClipY+height/2);
//}


DefaultProperties
{
	DrawDebugTraces=false
	UIGFXPlayerClass=class'MMOUIGFXPlayer'

	//thisHit.show = false
	//LoadMovie=none
	//HudMovie=none
	//SplashMovie=none
	//ProductVersion="V2011.09.30"
	//ShowHitHUDText=true

	MaxHitNumber=20
	DefualtDamageTextLifespan=0.81
}
