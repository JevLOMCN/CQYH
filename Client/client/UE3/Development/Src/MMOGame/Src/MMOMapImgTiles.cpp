#include "stdafx.h"
//#include "MMOGameUIClasses.h"
#include "MMOMapImgTiles.h"
#include "MMOFacade.h"

MMOMapImgTiles::MapImgTilesMap MMOMapImgTiles::_MapImgTile;
//
bool InitMapImgTiles()
{
	//static MMOMapImgTiles g_miniMapTiles( 3, 3 );
	static MMOMapImgTiles g_miniMapTiles( 2, 2 );
	MMOMapImgTiles::RegtMapImgTiles( E_MINIMAP_TILES, &g_miniMapTiles );
	//
	static MMOMapImgTiles g_pathMapTiles( 2, 2 );
	MMOMapImgTiles::RegtMapImgTiles( E_PATHMAP_TILES, &g_pathMapTiles );
	//
	static MMOMapImgTiles g_worldMapTiles( 2, 2 );
	MMOMapImgTiles::RegtMapImgTiles( E_WORLDMAP_TILES, &g_worldMapTiles );

	return true;
}
static bool g_InitMapImgTiles = InitMapImgTiles();
//
void MMOMapImgTiles::RegtMapImgTiles( MapTilesType mType, MMOMapImgTiles* tiles )
{
	_MapImgTile[mType] = tiles;
}
//
MMOMapImgTiles* MMOMapImgTiles::GetMapImgTiles( MapTilesType mType )
{
	MapImgTilesMap::iterator i = _MapImgTile.find(mType);
	if(i != _MapImgTile.end())
	{
		return i->second;
	}

	return 0;
}
//
MMOMapImgTiles::MMOMapImgTiles( UINT16 rowMax, UINT16 colMax )
{
	_TileUiRowMax = colMax;
	_TileUiColMax = rowMax;
	_TilesGap = 0.5;
	_ZoomFactor = 1;

	_GfxPlayer = NULL;
	_TileImgRowNum = 0;
	_TileImgColNum = 0;
	_IsInitialized = FALSE;
	_IsLastOnLeft = TRUE;
	_IsLastOnTop = TRUE;
	_RowIdx = 0;
	_ColIdx = 0;
	//
	_ShowTile = new UITile *[_TileUiRowMax];
	for( int i = 0; i < _TileUiRowMax; ++i ){
		_ShowTile[i] = new UITile[_TileUiColMax];
	}
	//
	if( _InterTempTile != NULL ){
		delete[] _InterTempTile;
	}
	_InterTempTile = new UITile [ _TileUiRowMax > _TileUiColMax ? _TileUiRowMax : _TileUiColMax ];
}
MMOMapImgTiles::~MMOMapImgTiles()
{
	for( int i = 0; i < _TileUiRowMax; ++i ){
		delete[] _ShowTile[i];
	}
	delete[] _ShowTile;
	_ShowTile = NULL;
	//
	delete[] _InterTempTile;
	_InterTempTile = NULL;
}
//===============================================================================================================================
bool MMOMapImgTiles::LoadUiTile( TArray<UGFxObject*>titles, FString texLoaderName )
{
	INT iIdx = 0;
	for( UINT i = 0; i < _TileUiRowMax; ++i ){
		for( UINT j = 0; j < _TileUiColMax; ++j ){
			//_ShowTile[i][j]._UiObj = parentMC->GetObject( *FString::Printf( TEXT("%s_%d_%d"), *tileName, i, j ) );
			iIdx = i * _TileUiColMax + j;
			_ShowTile[i][j]._UiObj = titles( iIdx );
			_ShowTile[i][j]._ImgLoderIdx = iIdx + 1;
			_ShowTile[i][j]._IsChanged = TRUE;
		}
	}
	_texLoaderName = texLoaderName;
	return titles.Num() == iIdx + 1;
}
//===============================================================================================================================
//@Desc: initialize block data
void MMOMapImgTiles::Initialize( UMMOGFXPlayer *OwnerPlayer, FString mapName, FVector pos3D,
							  UINT uTileW, UINT uTileH, INT tileImgRowNum, INT tileImgColNum,
							 FVector2D region3Dto2D, FVector2D region2dOffsetInMap, FVector2D afterTurnedOffsetInMap, float fScale )
{
	_TexName = mapName;
	_GfxPlayer = OwnerPlayer;
	//initialize the big map size before cutting
	_MapTileSize.X = uTileW;
	_MapTileSize.Y = uTileH;
	//
	_TileImgRowNum = tileImgRowNum;
	_TileImgColNum = tileImgColNum;
	//_Map3Dto2D is the conversion ratio from 3d coordinates to 2d coordinates; this local map's Y axis direction is opposite to the world 3d Y axis
	_Map3Dto2D = region3Dto2D;
	if( _Map3Dto2D.X == 0 ){
		_Map3Dto2D.X = 1;
	}
	if( _Map3Dto2D.Y == 0 ){
		_Map3Dto2D.Y = 1;
	}
	//_MapOffset coordinate origin is the bottom-left (0,0), up is the positive Y axis, right is the positive X axis
	_MapOffset = region2dOffsetInMap;
	//offset after rotating the original map left
	_MapOffset2 = afterTurnedOffsetInMap;
	//
	_ZoomFactor = fScale;
	//reset to load images and update tiles position
	ResetAllTiles();
	//initilize internal parameters which aid to computation
	FVector2D pos2DInMap = Map2DPosByWorld3DPos( pos3D );
	_MapTileImgIdx.Y = pos2DInMap.X / _MapTileSize.X; 
	_MapTileImgIdx.X = pos2DInMap.Y / _MapTileSize.Y;
	_LastTileIdx = _MapTileImgIdx;
	//
	_IsLastOnLeft = pos2DInMap.X < _MapTileImgIdx.Y * _MapTileSize.X + _MapTileSize.X / 2;
	_IsLastOnTop = pos2DInMap.Y < _MapTileImgIdx.X * _MapTileSize.Y + _MapTileSize.Y / 2;
	////place as centered as possible, with blocks on all sides, keep adjusting in update, rough setup first
	//_RowIdx = _IsLastOnTop ? _TileUiRowMax / 2 : ( _TileUiRowMax - 1 ) / 2;
	//_ColIdx = _IsLastOnLeft ? _TileUiColMax / 2 : ( _TileUiColMax - 1 ) / 2;

	//maps directly to the resource index, range issues are adjusted in InitTilesFitImgTilesNums
	_RowIdx = _MapTileImgIdx.Y;
	_ColIdx = _MapTileImgIdx.X;
	//
	InitTilesFitImgTilesNums();
	//
	_IsInitialized = TRUE;
}
//-----------------------------------------------------------------------------------------------
//@Desc: keep all tile blocks from accessing nonexistent textures
void MMOMapImgTiles::InitTilesFitImgTilesNums()
{
	INT tileUiRowIdxMax = _TileUiRowMax - 1;
	INT tileUiColIdxMax = _TileUiColMax - 1;
	INT tileImgRowIdxMax = _TileImgRowNum - 1;
	INT tileImgColIdxMax = _TileImgColNum - 1;

	//up
	if( _RowIdx > (UINT)_MapTileImgIdx.X ){
		_RowIdx = (UINT)_MapTileImgIdx.X;
	}
	//down
	INT leftRow = tileUiRowIdxMax - _RowIdx;
	INT leftRowImg = tileImgRowIdxMax - _MapTileImgIdx.X;
	if( leftRow > leftRowImg ){
		_RowIdx = tileUiRowIdxMax - leftRowImg;
	}
	//left
	if( _ColIdx > (UINT)_MapTileImgIdx.Y ){
		_ColIdx = (UINT)_MapTileImgIdx.Y;
	}
	//right
	INT leftCol = tileUiColIdxMax - _ColIdx;
	INT leftColImg = tileImgColIdxMax - _MapTileImgIdx.Y;
	if( leftCol > leftColImg ){
		_ColIdx = tileImgColIdxMax - leftColImg;
	}

	//
	if( _RowIdx < 0 ){
		_RowIdx = 0;
	}
	if( _ColIdx < 0 ){
		_ColIdx = 0;
	}

	if( _RowIdx >= _TileUiRowMax ){
		_RowIdx = tileUiRowIdxMax;
		
	}
	if( _ColIdx >= _TileUiColMax ){
		_ColIdx = tileUiColIdxMax;
	}
	//
	_ShowTile[_RowIdx][_ColIdx]._ImgIdxX = _MapTileImgIdx.X;
	_ShowTile[_RowIdx][_ColIdx]._ImgIdxY = _MapTileImgIdx.Y;

	for( UINT i = 0; i < _TileUiRowMax; ++i ){
		for( UINT j = 0; j < _TileUiColMax; ++j ){
			_ShowTile[i][j]._IsChanged = TRUE;
		}
	}
}
//===============================================================================================================================
//@Desc: when each tile is not smaller than the visible area, automatically adjust the fill count to 3X3 or more grid blocks, so blocks likely needed in the next adjustment are preloaded
void MMOMapImgTiles::TilesGoAroundByPosInTile( FVector2D pos2DInMap, bool hasBound )
{
	FIntPoint newTileImgIdx;

	newTileImgIdx.Y = pos2DInMap.X / _MapTileSize.X; 
	newTileImgIdx.X = pos2DInMap.Y / _MapTileSize.Y;
	//
	FVector2D tileCenterInMap2dPos;
	tileCenterInMap2dPos.X = ( _MapTileImgIdx.Y * _MapTileSize.X + _MapTileSize.X / 2 );
	tileCenterInMap2dPos.Y = ( _MapTileImgIdx.X * _MapTileSize.Y + _MapTileSize.Y / 2 );
	UBOOL onLeft = pos2DInMap.X < tileCenterInMap2dPos.X;
	UBOOL onTop = pos2DInMap.Y < tileCenterInMap2dPos.Y;

	bool isNotLeft = true;
	bool isNotRight = true;
	bool isNotTop = true;
	bool isNotBottom = true;
	if( hasBound ){
		isNotLeft = _ShowTile[_RowIdx][_ColIdx]._ImgIdxY > 0;
		isNotRight = _ShowTile[_RowIdx][_ColIdx]._ImgIdxY < _TileImgColNum - 1;
		isNotTop = _ShowTile[_RowIdx][_ColIdx]._ImgIdxX > 0;
		isNotBottom = _ShowTile[_RowIdx][_ColIdx]._ImgIdxX < _TileImgRowNum - 1;
	}
	//
	//left right
	//if( _IsLastOnLeft != onLeft || _ShowTile[_RowIdx][_ColIdx]._ImgIdxY != newTileImgIdx.Y ){
	if( _IsLastOnLeft != onLeft && _ShowTile[_RowIdx][_ColIdx]._ImgIdxY == newTileImgIdx.Y ){
		if( onLeft && _ColIdx == 0 && isNotLeft ){
			//now on most left
			Right2Left();
			++_ColIdx;
		}
		else if( !onLeft && _ColIdx == (_TileUiColMax - 1) && isNotRight ){
			//now on most right
			Left2Right();
			--_ColIdx;
		}
	}
	else if( _MapTileImgIdx.Y != newTileImgIdx.Y ){
		INT diff = 0;
		if( !hasBound ){
			diff = ( ( _MapTileImgIdx.Y > newTileImgIdx.Y ) ? -1 : 1 );
		}
		else{
			diff = ( ( _MapTileImgIdx.Y > newTileImgIdx.Y ) ? (isNotLeft?-1:0) : (isNotRight?1:0) );
		}

		if( diff ){
			_LastTileIdx.Y = _MapTileImgIdx.Y;
			_MapTileImgIdx.Y = newTileImgIdx.Y;
		}

		INT temp = diff + _ColIdx;

		_ColIdx =  ( temp < _TileUiColMax ? ( temp >= 0 ? temp : 0 ) : _ColIdx );
	}
	//top bottom
	//if( _IsLastOnTop != onTop || _LastTileIdx.Y != _MapTileImgIdx.Y ){
	//if( _IsLastOnTop != onTop || _ShowTile[_RowIdx][_ColIdx]._ImgIdxX != newTileImgIdx.X ){
	if( _IsLastOnTop != onTop && _ShowTile[_RowIdx][_ColIdx]._ImgIdxX == newTileImgIdx.X ){
		if( onTop && _RowIdx == 0 && isNotTop ){
			//now on most top
			Bottom2Top();
			++_RowIdx;
		}
		else if( !onTop && _RowIdx == (_TileUiRowMax - 1) && isNotBottom ){
			//now on most bottom
			Top2Bottom();
			--_RowIdx;
		}
	}
	else if( _MapTileImgIdx.X != newTileImgIdx.X ){
		INT diff = 0;
		if( !hasBound ){
			diff = ( ( _MapTileImgIdx.X > newTileImgIdx.X ) ? -1 : 1 );
		}
		else{
			diff = ( ( _MapTileImgIdx.X > newTileImgIdx.X ) ? (isNotTop?-1:0) : (isNotBottom?1:0) );
		}

		if( diff ){
			_LastTileIdx.X = _MapTileImgIdx.X;
			_MapTileImgIdx.X = newTileImgIdx.X;
		}
		INT temp = diff + _RowIdx;

		_RowIdx = ( temp < _TileUiRowMax ? ( temp >= 0 ? temp : 0 ) : _RowIdx );
	}

	//if accross the hole map , mark all changed
	if( Abs<INT>( _LastTileIdx.X - newTileImgIdx.X ) > 2 || Abs<INT>( _LastTileIdx.Y - newTileImgIdx.Y ) > 2 ){
		_RowIdx = ( _TileUiRowMax - 1 ) >> 1;
		_ColIdx = ( _TileUiColMax - 1 ) >> 1;
		
		ResetAllTiles();
		InitTilesFitImgTilesNums();

		_LastTileIdx.X = newTileImgIdx.X;
		_LastTileIdx.Y = newTileImgIdx.Y;
	}
	////make sure nothing is drawn out of bounds
	//if( isNotLeft && isNotLeft && isNotTop && isNotBottom ){
	//	_LastTileIdx = _MapTileImgIdx;
	//	_MapTileImgIdx = newTileImgIdx;
	//}
	//else{
	//	onLeft = pos2DInMap.X < _MapTileImgIdx.Y * _MapTileSize.X + _MapTileSize.X / 2;
	//	onTop = pos2DInMap.Y < _MapTileImgIdx.X * _MapTileSize.Y + _MapTileSize.Y / 2;
	//}
	//
	//_MainTilePos.X = (INT)( -pos2DInMap.X + _MapTileImgIdx.Y * _MapTileSize.X );
	//_MainTilePos.Y = (INT)( -pos2DInMap.Y + _MapTileImgIdx.X * _MapTileSize.Y );
	_MainTilePos.X = -pos2DInMap.X + _MapTileImgIdx.Y * _MapTileSize.X;
	_MainTilePos.Y = -pos2DInMap.Y + _MapTileImgIdx.X * _MapTileSize.Y;
	//
	_IsLastOnLeft = onLeft;
	_IsLastOnTop = onTop;
}

//---------------------------------------------------------------------------------------------------------------
//@Desc: when tile blocks are about to stop covering the visible range, automatically adjust tile blocks to fill the gap, so the content blocks in the visible range display
void MMOMapImgTiles::UpdateTilesFitsView( FVector2D viewCenterPosInMap, UINT viewWidth, UINT viewHeight )
{
	UBOOL hasBound = FALSE;
	FLOAT lateralBound = viewWidth >> 1;
	FLOAT verticalBound = viewHeight >> 1;

	FIntPoint newTileImgIdx;
	newTileImgIdx.Y = viewCenterPosInMap.X / _MapTileSize.X; 
	newTileImgIdx.X = viewCenterPosInMap.Y / _MapTileSize.Y;

	//horizontal
	bool needResetY = false;
	INT tileImgDiffY = ( _ShowTile[_RowIdx][_ColIdx]._ImgIdxY - newTileImgIdx.Y );
	if( tileImgDiffY > 0 ){
		needResetY = ( tileImgDiffY > _ColIdx );
	}
	else if( tileImgDiffY < 0 ){
		needResetY = ( -tileImgDiffY > ( _TileUiColMax - 1 - _ColIdx ) );
	}

	INT newColIdx = _ColIdx;
	FLOAT leftDis = ( viewCenterPosInMap.X - newTileImgIdx.Y * _MapTileSize.X );
	FLOAT rightDis = ( _MapTileSize.X - leftDis );
	if( !needResetY ){
		newColIdx = _ColIdx - tileImgDiffY;
		leftDis += newColIdx * _MapTileSize.X;
		rightDis += ( _TileUiColMax - newColIdx - 1 ) * _MapTileSize.X;
	}

	//vertical
	bool needResetX = false;
	INT tileImgDiffX = ( _ShowTile[_RowIdx][_ColIdx]._ImgIdxX - newTileImgIdx.X );
	if( tileImgDiffX > 0 ){
		needResetX = ( tileImgDiffX > _RowIdx );
	}
	else if( tileImgDiffX < 0 ){
		needResetX = ( -tileImgDiffX > ( _TileUiRowMax - 1 - _RowIdx ) );
	}

	INT newRowIdx = _RowIdx;
	FLOAT TopDis = ( viewCenterPosInMap.Y - newTileImgIdx.X * _MapTileSize.Y );
	FLOAT BottomDis = ( _MapTileSize.Y - TopDis );

	if( !needResetX ){
		newRowIdx = _RowIdx - tileImgDiffX;
		TopDis += newRowIdx * _MapTileSize.Y;
		BottomDis += ( _TileUiRowMax - newRowIdx - 1 ) * _MapTileSize.Y;
	}

	//if accross the hole map , mark all changed
	if( needResetX || needResetY ){
		_RowIdx = ( _TileUiRowMax - 1 ) >> 1;
		_ColIdx = ( _TileUiColMax - 1 ) >> 1;

		ResetAllTiles();
		//InitTilesFitImgTilesNums();

		_LastTileIdx.X = newTileImgIdx.X;
		_LastTileIdx.Y = newTileImgIdx.Y;

		_MapTileImgIdx = newTileImgIdx;
		return;
	}

	//
	//lateral
	for( INT i = 0; leftDis < lateralBound && i < _TileUiColMax; ++i){
		Right2Left();
		++newColIdx;
		leftDis += _MapTileSize.X;
	}
	for( INT i = 0; rightDis < lateralBound && i < _TileUiColMax; ++i){
		Left2Right();
		--newColIdx;
		rightDis += _MapTileSize.X;
	}

	//vertical
	for( INT j = 0; TopDis < verticalBound && j < _TileUiRowMax; ++j){
		Bottom2Top();
		++newRowIdx;
		TopDis += _MapTileSize.X;
	}
	for( INT j = 0; BottomDis < verticalBound && j < _TileUiRowMax; ++j){
		Top2Bottom();
		--newRowIdx;
		BottomDis += _MapTileSize.X;
	}
	
	//
	_RowIdx = newRowIdx;
	_ColIdx = newColIdx;
	_MapTileImgIdx = newTileImgIdx;
	//
	_MainTilePos.X = -viewCenterPosInMap.X + _MapTileImgIdx.Y * _MapTileSize.X;
	_MainTilePos.Y = -viewCenterPosInMap.Y + _MapTileImgIdx.X * _MapTileSize.Y;

	//
	_LastTileIdx.X = newTileImgIdx.X;
	_LastTileIdx.Y = newTileImgIdx.Y;
}

//---------------------------------------------------------------------------------------------------------------
//@Desc: only update tile block positions
void MMOMapImgTiles::OnlyUpdateTilesPosition( FVector2D viewCenterPosInMap )
{
	_MainTilePos.X = -viewCenterPosInMap.X + _MapTileImgIdx.Y * _MapTileSize.X;
	_MainTilePos.Y = -viewCenterPosInMap.Y + _MapTileImgIdx.X * _MapTileSize.Y;
}
//===============================================================================================================================
FVector2D MMOMapImgTiles::TilesKeepNotExceedBoundAround( FVector2D pos2DInMap, UINT halfW, UINT halfH )
{
	_MapTileImgIdx.Y = pos2DInMap.X / _MapTileSize.X; 
	_MapTileImgIdx.X = pos2DInMap.Y / _MapTileSize.Y;

	_MainTilePos.X = (INT)( -pos2DInMap.X + _MapTileImgIdx.Y * _MapTileSize.X );
	_MainTilePos.Y = (INT)( -pos2DInMap.Y + _MapTileImgIdx.X * _MapTileSize.Y );

	InitTilesFitImgTilesNums();
	return KeepTilesNotExceed( halfW, halfH );
}
//===============================================================================================================================
/*
@desc: only sets data, TileTheMap must be called afterwards to take effect
@Rtn: the adjusted offset
*/
FVector2D MMOMapImgTiles::KeepTilesNotExceed( UINT halfW, UINT halfH )
{
	INT xBound = (INT)halfW;
	INT yBound = (INT)halfH;

	FVector2D offset2D(0, 0);
	//-1 because blocks cannot join tightly, so they overlap; deduct it here
	//too right
	INT leftTopPosX = _MainTilePos.X - _ColIdx * ( _MapTileSize.X - _TilesGap );
	leftTopPosX *= _ZoomFactor;
	if(  leftTopPosX + xBound > 0  ){
		offset2D.X = -1 * ( leftTopPosX + xBound ) / _ZoomFactor;
		_MainTilePos.X += offset2D.X;
	}
	//too buttom
	INT leftTopPosY = _MainTilePos.Y - _RowIdx * ( _MapTileSize.Y - _TilesGap );
	leftTopPosY *= _ZoomFactor;
	if( leftTopPosY + yBound > 0 ){
		offset2D.Y = -1 * ( leftTopPosY + yBound ) / _ZoomFactor;
		_MainTilePos.Y += offset2D.Y;
	}
	//too left
	INT rightButtomPosX = _MainTilePos.X + (INT)(_TileUiColMax - _ColIdx) * ( _MapTileSize.X - _TilesGap );
	rightButtomPosX *= _ZoomFactor;
	if( xBound - rightButtomPosX > 0  ){
		offset2D.X =  ( xBound - rightButtomPosX ) / _ZoomFactor;
		_MainTilePos.X += offset2D.X;
	}
	//too top
	INT rightButtomPosY = _MainTilePos.Y + (INT)(_TileUiRowMax - _RowIdx) * ( _MapTileSize.Y - _TilesGap );
	rightButtomPosY *= _ZoomFactor;
	if( yBound - rightButtomPosY > 0 ){
		offset2D.Y = ( yBound - rightButtomPosY ) / _ZoomFactor;
		_MainTilePos.Y += offset2D.Y;
	}
	return offset2D;
}

//===============================================================================================================================
//@Desc: tiling
void MMOMapImgTiles::TileTheMap()
{
	UTexture2D* tex = NULL;

	INT offsetInRow = 0;
	INT offsetInCol = 0;
	FLOAT mainTileX = _MainTilePos.X;
	FLOAT mainTileY = _MainTilePos.Y;
	for( INT i = 0; i < _TileUiRowMax; ++i ){
		for( INT j = 0; j < _TileUiColMax; ++j ){
			offsetInRow = (INT)( j - _ColIdx );
			offsetInCol = (INT)( i - _RowIdx );
			//Postion
			FLOAT posX = mainTileX + offsetInRow * ( _MapTileSize.X - _TilesGap );
			FLOAT posY = mainTileY + offsetInCol * ( _MapTileSize.Y - _TilesGap );
			//FLOAT posX = _MainTilePos.X + offsetInRow * ( _MapTileSize.X - _TilesGap );
			//FLOAT posY = _MainTilePos.Y + offsetInCol * ( _MapTileSize.Y - _TilesGap );
			//if( offsetInCol ){
			//	posX += s_TuneAidX;
			//}
			//if( offsetInRow ){
			//	posY += s_TuneAidY;
			//}
			if( _ShowTile[i][j]._UiObj == NULL ){
				continue;
			}
			_ShowTile[i][j]._UiObj->SetPosition( posX, posY );
			//if not change, no need update texture
			//if( !_ShowTile[i][j]._IsChanged ){
			//	continue;
			//}
			//texture
			INT TileY = _MapTileImgIdx.Y + j - _ColIdx;
			INT TileX = _MapTileImgIdx.X + i - _RowIdx;
			////texture number, up/down is the world X axis, left/right is the world Y axis
			////the top-left block is numbered 0_0, increasing to the right and down
			//INT TileX = _MapTileImgIdx.X + j - _ColIdx;
			//INT TileY = _MapTileImgIdx.Y + i - _RowIdx;
			//if has wrong texture index, nothing can do for now
			//if( TileX < 0 || TileY < 0 ){
			//	continue;
			//}
			if( !_ShowTile[i][j]._IsChanged
				&& _ShowTile[i][j]._ImgIdxX == TileX
				&& _ShowTile[i][j]._ImgIdxY == TileY ){
					continue;
			}
			tex = Cast<UTexture2D>( UObject::StaticLoadObject(	UTexture2D::StaticClass(), NULL,
															//*FString::Printf( TEXT("MMO_MapSnapshot.%s.%s256x256_%d_%d"), *_TexName, *_TexName, TileX, TileY ),
															*FString::Printf( TEXT("%s_%d_%d"), *_TexName, TileX, TileY ),
															NULL, LOAD_NoWarn, NULL) );
			if( tex == NULL ){
				tex = Cast<UTexture2D>( UObject::StaticLoadObject(	UTexture2D::StaticClass(), NULL,
															//*FString::Printf( TEXT("MMO_MapSnapshot.%s.%s256x256_%d_%d"), *_TexName, *_TexName, TileX, TileY ),
															*FString::Printf( TEXT("%s_%02d_%02d"), *_TexName, TileX, TileY ),
															NULL, LOAD_NoWarn, NULL) );
			}
			_ShowTile[i][j]._IsChanged = FALSE;
			_ShowTile[i][j]._ImgIdxX = TileX;
			_ShowTile[i][j]._ImgIdxY = TileY;
			if( tex != NULL){
				_GfxPlayer->SetExternalTexture( FString::Printf( *_texLoaderName, _ShowTile[i][j]._ImgLoderIdx ), tex );
			}
			else{
				warnf(NAME_Error, TEXT("======!!!!======= [TileTheMap map not found %s_%02d_%02d]"), *_TexName, TileX, TileY );
				//if this map doesn't exist, load a pure black image by default
				tex = Cast<UTexture2D>( UObject::StaticLoadObject(	UTexture2D::StaticClass(), NULL,
																TEXT("SeaDg03_MMOVolume_MapSize_2_mMap.SeaDg03_MMOVolume_MapSize_2_mMap_5_1"),
																NULL, LOAD_NoWarn, NULL) );
				if( tex != NULL){
					_GfxPlayer->SetExternalTexture( FString::Printf( *_texLoaderName, _ShowTile[i][j]._ImgLoderIdx ), tex );
				}
			}//end of else
		}//end of for j
	}//end of for i
}

//===============================================================================================================================
//@Desc: coordinate conversion
//===============================================================================================================================
FVector2D MMOMapImgTiles::Map2DPosByWorld3DPos( FVector pos3D )
{
	FVector2D pos2D;
	pos2D.X = pos3D.Y * _Map3Dto2D.X + _MapOffset.X;
	pos2D.Y = pos3D.X * _Map3Dto2D.Y + _MapOffset.Y;

	//
	FVector2D afterTurnPos2D;
	afterTurnPos2D.X =  pos2D.X * COS_MAP + pos2D.Y * SIN_MAP + _MapOffset2.X;
	afterTurnPos2D.Y = -pos2D.X * SIN_MAP + pos2D.Y * COS_MAP + _MapOffset2.Y;

	return afterTurnPos2D;
}
//@desc: convert a 2d coordinate within this local map to a world coordinate; this local map's coordinate system has its origin at the top-left (0,0)
FVector MMOMapImgTiles::World3DPosByScaledMap2DPos( FVector2D pos2D )
{
	FVector2D beforeTurnPos2D;
	beforeTurnPos2D.X = pos2D.X - _MapOffset2.X;
	beforeTurnPos2D.Y = pos2D.Y - _MapOffset2.Y;
	beforeTurnPos2D.X = -beforeTurnPos2D.Y * SIN_MAP + beforeTurnPos2D.X * COS_MAP;
	beforeTurnPos2D.Y =  beforeTurnPos2D.Y * COS_MAP + beforeTurnPos2D.X * SIN_MAP;

	FVector pos3D;
	pos3D.Y = ( beforeTurnPos2D.X / _ZoomFactor - _MapOffset.X ) / _Map3Dto2D.X;
	pos3D.X = ( beforeTurnPos2D.Y / _ZoomFactor - _MapOffset.Y ) / _Map3Dto2D.Y;

	return pos3D;
}
//
FVector MMOMapImgTiles::World3DPosByMap2DPos( FVector2D pos2D )
{
	FVector2D beforeTurnPos2D;
	beforeTurnPos2D.X = pos2D.X - _MapOffset2.X;
	beforeTurnPos2D.Y = pos2D.Y - _MapOffset2.Y;
	beforeTurnPos2D.X = -beforeTurnPos2D.Y * SIN_MAP + beforeTurnPos2D.X * COS_MAP;
	beforeTurnPos2D.Y =  beforeTurnPos2D.Y * COS_MAP + beforeTurnPos2D.X * SIN_MAP;

	FVector pos3D;
	pos3D.Y = ( beforeTurnPos2D.X - _MapOffset.X ) / _Map3Dto2D.X;
	pos3D.X = ( beforeTurnPos2D.Y - _MapOffset.Y ) / _Map3Dto2D.Y;

	return pos3D;
}
//----------------------------------------------------------------------------------------------------------------
FVector2D MMOMapImgTiles::Map2DOffsetFromWorld3DPos( FVector pos3D, FVector pos3DFrom )
{
	FVector2D pos2D;
	pos2D.X = ( pos3D.Y - pos3DFrom.Y ) * _Map3Dto2D.X;
	pos2D.Y = ( pos3D.X - pos3DFrom.X ) * _Map3Dto2D.Y;

	//
	FVector2D afterTurnPos2D;
	afterTurnPos2D.X =  pos2D.X * COS_MAP + pos2D.Y * SIN_MAP;
	afterTurnPos2D.Y = -pos2D.X * SIN_MAP + pos2D.Y * COS_MAP;

	return afterTurnPos2D;
}
////
//FVector2D MMOMapImgTiles::PosInUiByScaledUiCenter( FVector pos3DNeedCovert, FVector2D pos2DCenter )
//{
//	FVector centerPos3D = World3DPosByScaledMap2DPos( pos2DCenter );
//	FVector2D rst = ScaledMap2DOffsetFromWorld3DPos( pos3DNeedCovert, centerPos3D ) + _MainTilePos;
//	return rst;
//}

//===============================================================================================================================
void MMOMapImgTiles::ResetAllTiles()
{
	for( UINT i = 0; i < _TileUiRowMax; ++i ){
		for( UINT j = 0; j < _TileUiColMax; ++j ){
			_ShowTile[i][j]._IsChanged = TRUE;
			_ShowTile[i][j]._ImgIdxX = 0;//0x80000001;
			_ShowTile[i][j]._ImgIdxY = 0;//0x80000001;
		}
	}
}
//
/*
*/
void MMOMapImgTiles::Right2Left()
{
	//UITile _InterTempTile[_TileUiColMax];
	INT maxColIdx = _TileUiColMax - 1;

	for( int i = 0; i < _TileUiRowMax; ++i ){
		_InterTempTile[i] = _ShowTile[i][maxColIdx];
	}
	for( int j = maxColIdx; j > 0; --j ){
		for( int i = 0; i < _TileUiRowMax; ++i ){
			_ShowTile[i][j] = _ShowTile[i][j-1];
		}
	}
	for( int i = 0; i < _TileUiRowMax; ++i ){
		_ShowTile[i][0] = _InterTempTile[i];
		_ShowTile[i][0]._IsChanged = TRUE;
	}
}
void MMOMapImgTiles::Left2Right()
{
	//UITile _InterTempTile[_TileUiRowMax];
	INT maxColIdx = _TileUiColMax - 1;

	for( int i = 0; i < _TileUiRowMax; ++i ){
		_InterTempTile[i] = _ShowTile[i][0];
	}
	for( int j = 0; j < maxColIdx; ++j ){
		for( int i = 0; i < _TileUiRowMax; ++i ){
			_ShowTile[i][j] = _ShowTile[i][j+1];
		}
	}
	for( int i = 0; i < _TileUiRowMax; ++i ){
		_ShowTile[i][maxColIdx] = _InterTempTile[i];
		_ShowTile[i][maxColIdx]._IsChanged = TRUE;
	}

}
//
void MMOMapImgTiles::Bottom2Top()
{
	//UITile _InterTempTile[_TileUiColMax];
	INT maxRowIdx = _TileUiRowMax - 1;

	for( int j = 0; j < _TileUiColMax; ++j ){
		_InterTempTile[j] = _ShowTile[maxRowIdx][j];
	}
	for( int i = maxRowIdx; i > 0; --i ){
		for( int j = 0; j < _TileUiColMax; ++j ){
			_ShowTile[i][j] = _ShowTile[i-1][j];
		}
	}
	for( int j = 0; j < _TileUiColMax; ++j ){
		_ShowTile[0][j] = _InterTempTile[j];
		_ShowTile[0][j]._IsChanged = TRUE;
	}
}
void MMOMapImgTiles::Top2Bottom()
{
	//UITile _InterTempTile[_TileUiColMax];
	INT maxRowIdx = _TileUiRowMax - 1;

	for( int j = 0; j < _TileUiColMax; ++j ){
		_InterTempTile[j] = _ShowTile[0][j];
	}
	for( int i = 0; i < maxRowIdx; ++i ){
		for( int j = 0; j < _TileUiColMax; ++j ){
			_ShowTile[i][j] = _ShowTile[i+1][j];
		}
	}
	for( int j = 0; j < _TileUiColMax; ++j ){
		_ShowTile[maxRowIdx][j] = _InterTempTile[j];
		_ShowTile[maxRowIdx][j]._IsChanged = TRUE;
	}
}
/*
void MMOMapImgTiles::Right2Left()
{
	UITile temp[3];
	INT maxColIdx = _TileUiColMax-1;
	temp[0] = _ShowTile[0][maxColIdx];
	temp[1] = _ShowTile[1][maxColIdx];
	temp[2] = _ShowTile[2][maxColIdx];
	for( int j = maxColIdx; j > 0; --j ){
		_ShowTile[0][j] = _ShowTile[0][j-1];
		_ShowTile[1][j] = _ShowTile[1][j-1];
		_ShowTile[2][j] = _ShowTile[2][j-1];
	}
	_ShowTile[0][0] = temp[0];
	_ShowTile[1][0] = temp[1];
	_ShowTile[2][0] = temp[2];
	//
	_ShowTile[0][0]._IsChanged = TRUE;
	_ShowTile[1][0]._IsChanged = TRUE;
	_ShowTile[2][0]._IsChanged = TRUE;
}
void MMOMapImgTiles::Left2Right()
{
	UITile temp[3];
	INT maxColIdx = _TileUiColMax-1;
	temp[0] = _ShowTile[0][0];
	temp[1] = _ShowTile[1][0];
	temp[2] = _ShowTile[2][0];
	for( int j = 0; j < maxColIdx; ++j ){
		_ShowTile[0][j] = _ShowTile[0][j+1];
		_ShowTile[1][j] = _ShowTile[1][j+1];
		_ShowTile[2][j] = _ShowTile[2][j+1];
	}
	_ShowTile[0][maxColIdx] = temp[0];
	_ShowTile[1][maxColIdx] = temp[1];
	_ShowTile[2][maxColIdx] = temp[2];
	//
	_ShowTile[0][maxColIdx]._IsChanged = TRUE;
	_ShowTile[1][maxColIdx]._IsChanged = TRUE;
	_ShowTile[2][maxColIdx]._IsChanged = TRUE;
}
//
void MMOMapImgTiles::Bottom2Top()
{
	UITile temp[3];
	INT maxRowIdx = _TileUiRowMax-1;
	temp[0] = _ShowTile[maxRowIdx][0];
	temp[1] = _ShowTile[maxRowIdx][1];
	temp[2] = _ShowTile[maxRowIdx][2];
	for( int i = maxRowIdx; i > 0; --i ){
		_ShowTile[i][0] = _ShowTile[i-1][0];
		_ShowTile[i][1] = _ShowTile[i-1][1];
		_ShowTile[i][2] = _ShowTile[i-1][2];
	}
	_ShowTile[0][0] = temp[0];
	_ShowTile[0][1] = temp[1];
	_ShowTile[0][2] = temp[2];
	//
	_ShowTile[0][0]._IsChanged = TRUE;
	_ShowTile[0][1]._IsChanged = TRUE;
	_ShowTile[0][2]._IsChanged = TRUE;
}
void MMOMapImgTiles::Top2Bottom()
{
	UITile temp[3];
	INT maxRowIdx = _TileUiRowMax-1;
	temp[0] = _ShowTile[0][0];
	temp[1] = _ShowTile[0][1];
	temp[2] = _ShowTile[0][2];
	for( int i = 0; i < maxRowIdx; ++i ){
		_ShowTile[i][0] = _ShowTile[i+1][0];
		_ShowTile[i][1] = _ShowTile[i+1][1];
		_ShowTile[i][2] = _ShowTile[i+1][2];
	}
	_ShowTile[maxRowIdx][0] = temp[0];
	_ShowTile[maxRowIdx][1] = temp[1];
	_ShowTile[maxRowIdx][2] = temp[2];
	//
	_ShowTile[maxRowIdx][0]._IsChanged = TRUE;
	_ShowTile[maxRowIdx][1]._IsChanged = TRUE;
	_ShowTile[maxRowIdx][2]._IsChanged = TRUE;
}
*/
///*
//@ put all tiles around the origin of the coordinate
//*/
//void MMOMapImgTiles::JustifyMapTilesToOrigin( FVector2D pos2DInMap )
//{
//	//FVector2D pos2DInMap = Map2DPosByWorld3DPos( pos3D  );
//	_MapTileImgIdx.Y = pos2DInMap.X / _MapTileSize.X; 
//	_MapTileImgIdx.X = pos2DInMap.Y / _MapTileSize.Y;
//	//
//	_RowIdx = ( _TileUiRowMax - 1 ) >> 1;
//	_ColIdx = ( _TileUiColMax - 1 ) >> 1;
//	//
//	UBOOL onLeft = pos2DInMap.X < _MapTileImgIdx.Y * _MapTileSize.X + _MapTileSize.X / 2;
//	UBOOL onTop = pos2DInMap.Y < _MapTileImgIdx.X * _MapTileSize.Y + _MapTileSize.Y / 2;
//	//position
//	FLOAT colOffset;
//	colOffset = (FLOAT)_ColIdx - (FLOAT)_TileUiColMax / 2;
//	_MainTilePos.X = (INT)( _MapTileSize.X * colOffset );
//
//	FLOAT rowOffset;
//	rowOffset = (FLOAT)_RowIdx - (FLOAT)_TileUiRowMax / 2;
//	_MainTilePos.Y = (INT)( _MapTileSize.Y * rowOffset );
//	//
//	_IsLastOnLeft = onLeft;
//	_IsLastOnTop = onTop;
//}
