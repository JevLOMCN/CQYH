class MMOUIMinimap extends MMOUIBase
	native(UI)
	dependson(MMOUISceneHint, GlobalMapInfo);

var MMOUIMinimap _Minimap;
var bool bShowTips;
//
//var Vector2D mapstone3DSize;
//var Vector2D FullTextureMapSize;//full map texture size
//var Vector2D SubTextureMapSize;//map texture size after splitting
var array<GFxObject> MapTiles;
var GFxObject MapNPCView;//the layer where NPC icons are stacked
var GFxObject MapPathView;//the path layer
var GFxObject MapCoord;
var GFxObject MapName;
var GFxObject MapBlock;
var GFxObject PlayerArrow;
var GFxObject BattleEnterToggleBtn;
var GFxObject BattleLeaveBtn;
var GFxObject ButtonHoverHintMc;

var GFxObject FBAidBtn;
var GFxObject SceneIconMc;
var GFxObject SceneLinesDropDownList;

var GFxObject ToggleShowMapBtn;
var GFxObject MailBtn;
var GFxObject MiniMapZoomInBtn;
var GFxObject MiniMapZoomOutBtn;
var GFxObject WorldMapOpenBtn;

var GFxObject ConfirmMsgBox;

var int InRegionIdx;
var bool IsMultilayerRegion;
var int PrepareSceneLineID;

var int PanelWidth;
var int PanelHeight;

var float MapViewBound;
var int CurMapViewBound;

var string CurMapTexName;
var string CurMapDisplayName;

var int ZoomLevel;
var float ZoomQuot;
var float ZoomDefaultQuot;
var float CameraYawDeg;
var const float MiniMap_SIN_MAP;
var const float MiniMap_COS_MAP;

var bool IsCurrentMapShowing;
var bool IsSmallMinimapMode;

// add by zhangrui@snda
var bool IsReputationTrace;
// Bottom coordinate when the reputation bar is closed
var float PanelBottomWithoutReputation;
// Bottom coordinate when the reputation bar is open
var float PanelBottomWithReputation;

native function ChangeScene();
native function InitMiniMap();
native function ClearUI();

//
native function SwitchMinimapToPathMap();
native function SwitchMinimapMode();
native function ToggleShowMap();
native function DesideToShowMinimap();
native function ToggleMinimapShowType();
event CallASToggleShowMap( bool isShowMap )
{
	ActionScriptVoid("ToggleShowMap");
}

native function InitMiniMapTiles( string inMapFileName, InMapRegion RegionInMap );
//
native function ClearMiniMapUI();
//
native function bool IfNeedReloadMap( int regionIdx );
//
native function InitMapStaticMarks();
//native function INT CheckRegion();
native function UpdateMinimap();
//update NPCs position ( static in map )
native function UpdateMapStaticMarks( Vector2D centerPos, float fYawDegCosine, float fYawDegSine );
//update monsters position( near player )
native function UpdateMarksNearPlayer( Vector2D centerPos, float fYawDegCosine, float fYawDegSine );
//
native function UpdateMarksGlobalPlayer( Vector2D centerPos, float fYawDegCosine, float fYawDegSine );
//update player marks
native function UpdatePlayerMarksInMap( Vector2D centerPos, float fYawDegCosine, float fYawDegSine );
//
final native function UpdateSceneLinesList();

event FlashSceneLinesList( GFxObject Provider, int selIdx )
{
	ActionScriptVoid( "UCUpdateSceneLinesList" );
}
event ResetSceneLinesList()
{
	ActionScriptVoid( "UCResetSceneLinesList" );
}

event CallUpdateTraceRelation(GFxObject data)
{
	ActionScriptVoid( "UpdateTraceRelation" );
}

event CallUCupdateSign(GFxObject data)
{
	ActionScriptVoid( "UCupdateSign" );
}
native function ZoomIn();
native function ZoomOut();

//native function Vector2D Get2DPosFromVector( Vector Pos3D );
//native function Vector2D Get2DPosOffsetByPlayerPos( Vector Pos3D );
//native function  SetMapPiece( intPoint MapPieceNum, bool onLeft , bool onTop, Vector2D PlayerPos2D );


//--------------
event AddMapMarkEventListerner( GFxObject newUiObj )
{
	ActionScriptVoid( "UCAddMapMark" );
}
//
event InitMapMark( GFxObject newUiObj, int objDataID, int type, String frameName )
{
	ActionScriptVoid( "UCInitMapMark" );
}
//
event SetMapMarkIcon( GFxObject Provider )
{
	ActionScriptVoid( "UCSetMapMarkIcon" );
}
//
event GFxObject CallASGetMapMarkUIObjectContainer( int type )
{
	return ActionScriptObject("GetMapMarkUIObjectContainer");
}
//--------------
event CallUpdateDayTime(int time)
{
//	ActionScriptVoid("UCUpdateDayTime");
}

event CallUpdateWeather(int type)
{
//	ActionScriptVoid("UCUpdateWeather");
}

event CallSetPathFindState(int state)
{
	ActionScriptVoid("UCSetPathFindState");
}

native function StopFBTimeCountDownAndHide();
event StartFBTimeCountDown( int second )
{
	ActionScriptVoid("UCStartFBTimeCountDown");
}
event UCCallStopFBTimeCountDown()
{
	ActionScriptVoid("StopFBTimeCountDown");
}
//
event UCCallStartBattleQueueTimeCountDown( int second )
{
	ActionScriptVoid("UCStartBattleQueueTimeCountDown");
}
event UCCallStopBattleQueueTimeCountDown()
{
	ActionScriptVoid("StopBattleQueueTimeCountDown");
}
event UCCallSetHoverHintTitle( string title )
{
	ActionScriptVoid("SetHoverHintTitle");
}
event UCCallShowBattleBtnHoverHint( string contentFormat )
{
	ActionScriptVoid("ShowBattleBtnHoverHint");
}

event CallASSetMapZoneType( int zoneType )
{
	ActionScriptVoid("SetMapZoneType");
}
//
final native function ReqExitDuplicate_Ok( GFxObject pMc );
final native function ReqExitDuplicate_Cancel( GFxObject pMc );

native function NotifyJoinedBattleQueue();
native function NotifyLeaveBattleQueue();

final native function PreparingToChangeSceneLine( int combinatedSceneID );
final native function TryInterruptChangingSceneLine();
final native function NativeFinishPreparingToChangeSceneLine();
event FinishPreparingToChangeSceneLine()
{
	NativeFinishPreparingToChangeSceneLine();
}
native final function UpdateFramePinVisible();

//===============================================================================================================
//===============================================================================================================
native function bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace);

final native function ASShowHintMapType( GFxObject mc );
final native function ASHideHintMapType();

final native function ASChangeSceneLine( int lineID );
final native function ASShowMapLineDropList();

final native function ASShowTips_PinValue(GFxObject mc);
final native function ASHideTips_PinValue(GFxObject mc);
//Minimap
function ASMinimapZoomIn()
{
	if( _Minimap != none)
	{
		_Minimap.ZoomIn();
	}
}

function ASMinimapZoomOut()
{
	if( _Minimap != none)
	{
		_Minimap.ZoomOut();
	}
}

//===============================================================================================================
//===============================================================================================================
native function BeforeViewUpdate(float DeltaTime);
native function ASToggleShowMap();
native function ASToggleMap();
native function ASShowTipsToggleShowMap();
//
native function ASShowMinimapMarkHint( int type, int idx );
native function ASHideMinimapMarkHint();
//
native function ASClickBattleInfo( int x , int y );
native function ASHintBattleInfo();
native function ASHideHintBattleInfo();
//
native function ASToggleArenaEnt();
//
native function ASNotifyFBTimesUp();
//
native function ASClickFbInfo( int x , int y );
//
native function ASClickShoppingMallBtn();
native function ASClickActivityBtn();
//
native function ASShowTips_MapLineInfo( int stateType );
native function ASHideTips_MapLineInfo();
//
native function ASShowTips_MapZone( int zoneType );
native function ASHideTips_MapZone();
//
native function ASMoveToMinimapPos( int clickPosX, int clickPosY );

native function ASStopPathFind();

function OpenWorldMap()
{
	class'MMOUIManager'.static.ToggleShowPanelEx( class'MMOUIManager'.default.Panel_WorldMap, "" );
}

native final function ASClickMailIcon();

native function UpdateTraceReputationData(GFxObject data);

//function UpdateTraceReputationData(GFxObject data)
//{
//	if( _Minimap != none)
//	{
//		_Minimap.CallUpdateTraceRelation(data);
//	}
//}

// add by zhangrui05@snda
native final function ASShowMiniMapTrace();
native final function ASHideMiniMapTrace();

native final function ASUpdateFPSTips(GFxObject mc);
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{
	//FriendlyName=Minimap
	CameraYawDeg=45
	MiniMap_SIN_MAP=-0.70710678118654752440084436210485;//sin45
	MiniMap_COS_MAP=0.70710678118654752440084436210485;//cos45
	IsCurrentMapShowing=true
	IsSmallMinimapMode=true
	CurMapTexName=""
	InRegionIdx=-1
	DefaultVisible=true
	ZoomDefaultQuot=1.37
	ZoomQuot=1.37
	ZoomLevel=0
	CloseOnEsc=false

	IsReputationTrace=false
	PanelBottomWithoutReputation=288
	PanelBottomWithReputation=327
}
