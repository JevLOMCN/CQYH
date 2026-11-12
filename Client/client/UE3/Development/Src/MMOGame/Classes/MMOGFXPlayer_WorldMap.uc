class MMOGFXPlayer_WorldMap extends MMOGFXPlayer
	native(UI)
	config(MMOGame);

var MMOUIWorldMap _WorldMap;

//native function PostAdvance(float DeltaTime);

function RegisterWorldMap( MMOUIWorldMap mc )
{
	_WorldMap = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_WorldMap);
}

// event UnRegister()
// {
// 	if( _WorldMap != none ){
// 		_WorldMap.ClearWorldMapUI();
// 	}
// 
// 	super.UnRegister();
// 	RegisterUI( none, class'MMOUIManager'.default.Panel_WorldMap);
// 	MMOUI::RegisterMoviePlayer(class'MMOUIManager'.default.MoviePlayer_WorldMap);
// }

//
native function ASWorldMapZoom( bool ZoomIn );
native function ASWorldMapZoomTo( int level );

native function ASMapDrag( float deltaX, float deltaY );

//Map click movement
native function ASMoveToMapPos( int clickPosX, int clickPosY );
//native function ASMoveToRealMapPos( int mapPosX, int mapPosY );
native function ASInputCoodinateAndMoveTo( int inputX, int inputY );
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
//Move to map marker point
native function ASMoveToThisMapMark( int idxInStaticMap );
//
native function ASFindMapTask( int npcIdx );
//Toggle showing function NPCs
native function ASToggleFuncNpc( bool isShow );
//Toggle showing quest NPCs
native function ASToggleTaskNpc( bool isShow );
//
native function ASConfirmAddUserMark();
//
native function ASConfirmDelUserMark();
//
native function ASCloseUserMarkPanel();
//Map point tooltip display
native function ASShowWorldMapMarkHint( int type, int idx );
//
native function ASHideWorldMapMarkHint();
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
//End
DefaultProperties
{
	MovieInfoStr="UI_WorldMap.WorldMap"
	Priority=5
}
