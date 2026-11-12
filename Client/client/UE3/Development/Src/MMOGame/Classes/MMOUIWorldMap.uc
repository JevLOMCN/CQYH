class MMOUIWorldMap extends MMOUIBase
	native(UI)
	config(UIVisibleMask)
	dependson(GlobalMapInfo, MMOUIUnitedMap);

enum EPanelTypeWorldMap
{
	ePnlTyp_WorldMap,
	ePnlTyp_UnitedMap
};

var EPanelTypeWorldMap _CurPanelType;
var EPanelTypeWorldMap _CurPanelContentType;
//
var MMOUIUnitedMap _UnitedMapPanel;

var GFxObject RootMc;
var GFxObject MapNameTitle_tf;
var GFxObject MyIcon;
var GFxObject MyPostion_Tf;
var GFxObject MousePostion_Tf;
var GFxObject MapPaper;
var GFxObject MapBGLoader;
//var GFxObject MapPlayerView;
var GFxObject MapNPCView;//the layer where NPC icons are stacked
var GFxObject MapPathView;//the path layer
var GFxObject MapExtraInfoView;//the layer that shows supplementary icon info
var bool IsToShowMapMarksExtraInfo;
//
var GFxObject _MapMarkfilter_tf;
var GFxObject _MapListMc;
var GFxObject _MapTaskListMc;
var GFxObject _MapMarksListMc;

//
var GFxObject _ToggleStaticMarkBtn;
var GFxObject _ToggleTaskBtn;

//
var GFxObject _UserMarkPanelMc;
var GFxObject _UserMarkNameTf;
var GFxObject _AddUserMarkBtn;
var GFxObject _DelUserMarkBtn;
var GFxObject _ShareUserMarkBtn;
var GFxObject _ShareUserMarkList;
var GFxObject _ChooseUserMarkTypeBtn[6];

var array<GFxObject> MapTiles;

var Vector2D map3Dto2D;
var Vector2D MapUiCenterPos;
//var Vector2D uiShowedMapOffset;
var Vector2D MaskZonePos;
var Vector2D PaperZoneCenterPos;
var Vector2D PaperZoneSize;
var Vector2D PaperMaskBorder;
var Vector2D ThisPos;
var Vector AddMarkPos;
var float CameraYawDeg;

var int InRegionIdx;
var bool IsMultilayerRegion;

var string CurMapTexName;
var int SelectedUserMarkIdx;

var bool CanFuncNpcShow;
var bool CanTaskNpcShow;

var float MapUIResScale;
var int ZoomLevel;
var int MaxZoomLevel;
var float ZoomDefaultQuot;
var float MapUIZoom;
var float MapUIZoomDefault;

var FLOAT ValidMapPhotoSize;
var FLOAT ScaledValidMapPhotoSize;

var Vector2D MapBGLoaderPos;
var Vector2D MapBGMoveLimit;
var Vector2D MapImgOffset;//offset when pasting the aerial map image onto the final map

var Vector2D LastMoveModify;
var bool IsTipsShowed;

//native function ToogleVisible();

//native function ToggleWorldMap();
//
native function InitWorldMap( int mapID, int regionDataIdx );
//
native function InitWorldMapTiles( string inMapFileName, InMapRegion RegionInMap, bool isResetPos );
//
native function InitMapStaticMarks();
//
native function ClearWorldMapUI();
//
native function RecoverWorldMapUI();
//
native function UpdateWorldMapContents();

native function UpdateMapTilesPosition();
//update NPCs position ( static in map )
native function UpdateMapStaticMarks( Vector centerPos3D, float fYawDeg );
//update monsters position( near player )
native function UpdateMarksNearPlayer( Vector centerPos3D, float fYawDeg );
//
native function UpdateMarksGlobalPlayer( Vector centerPos3D, float fYawDeg  );
//update player marks
native function UpdatePlayerMarksInMap( Vector centerPos3D, float fYawDeg );
//
native function UpdateMapPath( Vector centerPos3D, float fYawDeg );
//
native function UpdateMapList();
//
event FlashWorldMapMapList( GFxObject Provider, int selIdx )
{
	ActionScriptVoid( "UCUpdateWorldMapMapList" );
}
//
native function UpdateMapTaskList();
//
event FlashWorldMapTaskList( GFxObject Provider )
{
	ActionScriptVoid( "UCUpdateWorldMapTaskList" );
}
//Update NPC, dungeon list
native function UpdateMapStaticMarkList();
//
event FlashWorldMapPointsList( GFxObject Provider )
{
	ActionScriptVoid( "UCUpdateWorldMapPointsList" );
}
//
native function ToggleShowStaticMarksOf( int enumEActorInMapFlag, bool isShow );
//
native function ToggleShowNearPlayerMarksAndMyself( bool isShow );
//
//native function bool IfNeedReloadMap( int regionIdx );
//
native function Zoom( bool ZoomIn );
native function ZoomTo(int level);
//
native function ResizeMap();
//
native function MoveMapByDelta( int deltaX, int deltaY );
//
native function HighlightMark( int idxInStaticMap );
//
event ResetWorldMapUI()
{
	ActionScriptVoid( "UCResetMapContent" );
}
//
native function UpdateMapUserMarkShareList();
//
native function PrepareShowAddUserMarkPanel( float uiPosX, float uiPosY );
//
native function DelMapMark();
//
native function ShareCurUserMarkTo( int targetType );
//
native function CloseUserMarkPanel();
//
native function ChangeScene();

native function ShowMapTips( bool isShow );
//----------------------------------
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

//===========================
event InitNewMapMarkIcon( int markType, int markIdx, bool isToShow, string markName )
{
	ActionScriptVoid( "UCInitNewMapMarkIcon" );
}

event UpdateNewMapMarkIcon( int markType, int markIdx, int iconPosX, int iconPosY, float rotation )
{
	ActionScriptVoid( "UCUpdateNewMapMarkIcon" );
}
//===========================
event FlashWorldMapShareUserMarkkList( GFxObject Provider )
{
	ActionScriptVoid( "UCUpdateWorldMapShareUserMarkkList" );
}
//----------------------------------
event ChangeMapMarkShowLayer( GFxObject uiObj, int layerIdx )
{
	ActionScriptVoid( "UCChangeMapMarkLayer" );
}
//----------------------------------
event CallASSetUserMarkPanelIconName( string markName )
{
	ActionScriptVoid( "UCSetUserMarkPanelIconName" );
}
//
event GFxObject CallASGetMapMarkUIObjectContainer( int type )
{
	return ActionScriptObject("GetMapMarkUIObjectContainer");
}
//event CallASSetMapScaleSlider( float curPos )
event CallASSetMapScaleSlider( int maxPos, int curPos )
{
	ActionScriptVoid( "UCUpdateSliderValue" );
}

//----------------------------------
event UCSetXY( int posX, int posY )
{
	ActionScriptVoid( "UCSetXY" );
}

//----------------------------------
event CallASChangeWorldMapContent( EPanelTypeWorldMap panelType )
{
	_CurPanelContentType = panelType;
	ActionScriptVoid( "UCChangeWorldMapContent" );
}

event CallASChangeWorldMapTabBtn( EPanelTypeWorldMap panelType )
{
	_CurPanelType = panelType;
	ActionScriptVoid( "UCChangeWorldMapTabBtn" );
}

event CallASShowBackUnitedMapBtn( bool isToShow )
{
	ActionScriptVoid( "UCShowBack" );
}

event CallShowGuildOccupyName(bool isToShow, string occupyName)
{
	ActionScriptVoid("UCGuildOccupyName");
}

//==============================================================================================================
//==============================================================================================================
native function BeforeViewUpdate( float DeltaTime );
//
native function ASWorldMapZoom( bool ZoomIn );
native function ASWorldMapZoomTo( int level );

native function ASMapDrag( float deltaX, float deltaY );

//Add marker point
native function ASAddOrRemoveMarkMap( int clickPosX, int clickPosY );
//Select marker point share target
native function ASChangeShareUserMarkTarget( int listIdx, int targetType );
//Share marker point
native function ASShareUserMark( int targetType );
//Switch map
native function ASChangeMap( int mapID, int regionInMapIdx );
//Toggle between large map and global map
native function ASShowWorldMapTabPanel( int type );
//Find fixed points on the map
native function ASFindThisMapMark( int idx );
//
native function ASFindMapTask( int npcIdx );
//Toggle showing function NPCs
native function ASToggleFuncNpc( bool isShow );
//Toggle showing quest NPCs
native function ASToggleTaskNpc( bool isShow );
//
native function ASInputCoodinateAndMoveTo( int inputX, int inputY );
//
native function ASConfirmAddUserMark();
//
native function ASConfirmDelUserMark();
//
native function ASCloseUserMarkPanel();
//Move to map marker point
native function ASMoveToThisMapMark( int idxInStaticMap );
//Map point tooltip display
native function ASShowWorldMapMarkHint( int type, int idx );
//
native function ASHideWorldMapMarkHint();
//Map click movement
native function ASMoveToMapPos( int clickPosX, int clickPosY );
//native function ASMoveToRealMapPos( int mapPosX, int mapPosY );
//
native function ASShowTipsWorldMapMarkListItem( GFxObject mc, int idxInStaticMap, string contentText );
native function ASHideTipsWorldMapMarkListItem( GFxObject mc );
//function ToggleWorldMap()
//{
//	ToggleShowUI( class'MMOUIManager'.default.Panel_WorldMap );
	
//}

//World map
native function ASClickToWorldMap( int tileIdx, int mapID, string regionName );
native function ASClickToBack( GfxObject fromMc, GfxObject toBackMc );
native function ASClickToNewWorld( GfxObject fromMc, GfxObject toNewMc );

native function ASShowTipsUnitedMapTile( GfxObject fromMc, int mapID, string regionName, string displayMapName );
native function ASHideTipsUnitedMapTile();
native function ASToggleShowUnitedMapTileLevel( bool isToShow );
//==============================================================================================================
//==============================================================================================================

DefaultProperties
{
	//FriendlyName=WorldMap
	_CurPanelType=ePnlTyp_WorldMap
	_CurPanelContentType=ePnlTyp_WorldMap
	IsDynamicLoad=true
	CanFuncNpcShow=true
	CanTaskNpcShow=true
	CameraYawDeg=45
	ZoomLevel=0
	MapUIZoom=1
	MaxZoomLevel=16
	MapUIZoomDefault=0.63//0.467
	ValidMapPhotoSize=1024//512*2*2;
	ScaledValidMapPhotoSize=597//380*2;
	//ZoomDefaultQuot=0.7421875//ratio of map UI valid area length to actual map length 380*2/(512*2)
	//MapImgOffsetX=366.48421052631578947368421052632//(652*2-380*2)/2/ZoomDefaultQuot
	////183.24210526315789473684210526316//(652-380)/2/ZoomDefaultQuot
	//MapImgOffsetY=1239.5789473684210526315789473684//((540*2-380*2)/2+380*2 )/ZoomDefaultQuot
	////619.78947368421052631578947368421//((540-380)/2+380 )/ZoomDefaultQuot
}
