class MMOUIPathMap extends MMOUIBase
	native(UI)
	config(UIVisibleMask);

var array<GFxObject> MapTiles;
var GFxObject MapNPCView;//the layer where NPC icons are stacked
var GFxObject MapPathView;//the path layer
var GFxObject MapPaper;
var GFxObject MyIcon;

var GFxObject MapZoneMaskMc;
var Vector2D MapVisibleZoneSize;

var Vector2D PaperZoneSizeHalf;
var Vector2D MapUiCenterPos;

var int InRegionIdx;
var bool IsMultilayerRegion;

var string CurMapTexName;

var float CameraYawDeg;
var float ZoomDefaultQuot;

native function ChangeScene();
native function InitPathMap();
native function ClearUI();
native function ResizeMap( float scaleVaule );

native function InitPathMapTilesByCurSceneData();
native function InitPathMapTiles( string inMapFileName, InMapRegion RegionInMap );
//
native function ClearPathMapUI();
//
//native function bool IfNeedReloadMap( int regionIdx );
//
native function InitMapStaticMarks();
//native function INT CheckRegion();
native function UpdatePathMap();
//update NPCs position ( static in map )
native function UpdateMapStaticMarks( Vector centerPos3D, float fYawDeg );
//update monsters position( near player )
native function UpdateMarksNearPlayer( Vector centerPos3D, float fYawDeg );
//
native function UpdateMarksGlobalPlayer( Vector centerPos3D, float fYawDeg );
//update player marks
native function UpdatePlayerMarksInMap( Vector centerPos3D, float fYawDeg );

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
event GFxObject CallASGetMapMarkUIObjectContainer( int type )
{
	return ActionScriptObject("GetMapMarkUIObjectContainer");
}
//===============================================================================================================
//===============================================================================================================
native function BeforeViewUpdate( float DeltaTime );

native function bool SkipCaptureInputKey( string key, EInputEvent eIE, string strHoverControllerName, Vector2D ClickPosInMovieSpace);
//
native function ASPathMapToMinimap();

//
native function ASShowPathMapMarkHint( int type, int idx );
native function ASHidePathMapMarkHint();
//===============================================================================================================
//===============================================================================================================

DefaultProperties
{
	//FriendlyName=PathMap
	//CameraYawDeg=45
	ZoomDefaultQuot=0.621
	CurMapTexName=""
	InRegionIdx=-1
	IsDynamicLoad=true
	CloseOnEsc=false
}
