class MMOGFXPlayer_Minimap extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIMinimap _Minimap;
var bool bShowTips;

//native function PostAdvance(float DeltaTime);
native function UpdateMinimap(float DeltaTime);

function RegisterMinimap( MMOUIMinimap mc )
{
	_Minimap = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Minimap);
}

event bool SkipCaptureInputKey(const string key, EInputEvent eIE, const string strHoverControllerName, Vector2D ClickPosInMovieSpace)
{
	//`log( "strHoverControllerName : "$strHoverControllerName );
	if( strHoverControllerName == "PlayerArraw_mc" || strHoverControllerName == "MapNpcView_mc" ){
		return false;
	}

	return super.SkipCaptureInputKey(key, eIE, strHoverControllerName, ClickPosInMovieSpace);
}

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

//
native function ASToggleShowMap();
native function ASToggleMap();
native function ASShowTipsToggleShowMap();
//
native function ASMoveToMinimapPos( int clickPosX, int clickPosY );
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
//End
DefaultProperties
{
	MovieInfoStr="UI_Minimap.Minimap"
	bAllowFocus=FALSE
	Priority=3
	bShowTips=FALSE
}