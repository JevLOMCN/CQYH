/*=============================================================================
// LadderVolumes, when touched, cause ladder supporting actors to use Phys_Ladder.
// note that underwater ladders won't be waterzones (no breathing problems)
// Copyright 1998-2010 Epic Games, Inc. All Rights Reserved.
============================================================================= */

class MMOVolume_MapSize extends Volume
	native
	placeable;

enum EMapSize_Type
{
	EMST_TYPE1		<DisplayName="2048*2048">,          // stand at the same location
	EMST_TYPE2		<DisplayName="1024*1024">,           // roaming around a certain area
	EMST_TYPE3		<DisplayName="512*512">,
};

enum EMapSize_Priorrity
{
	EMSP_NotShow	<DisplayName="Not ShowIn UI">,
	EMSP_PLow	    <DisplayName="Low">,
	EMSP_PMid	    <DisplayName="Mid">,
	EMSP_PHigh	    <DisplayName="High">,
};

// Whether it is client-only (if so, no need to export)
var(MMOMapSize) bool IsExport;
var(MMOMapSize) int AerialPhotoPrecent;
//var(MMOMapSize) editconst EMapSize_Type MapSizeType;
var(MMOMapSize) EMapSize_Priorrity Priorrity;
var(MMOMapSize) string ReuseMapVolumeName;
var(MMOMapSize) string DisplayName;
var(MMOMapSize) editconst int MiniMapTileRowNum;
var(MMOMapSize) editconst int MiniMapTileColNum;
var(MMOMapSize) editconst float MiniMapPixelSize;
var(MMOMapSize) editconst float MiniMapScale;
var(MMOMapSize) editconst int MiniMapOffsetX;
var(MMOMapSize) editconst int MiniMapOffsetY;

var(MMOMapSize) editconst int WorldMapTileRowNum;
var(MMOMapSize) editconst int WorldMapTileColNum;
var(MMOMapSize) editconst float WorldMapPixelSize;
var(MMOMapSize) editconst float WorldMapScale;
var(MMOMapSize) editconst int WorldMapOffsetX;
var(MMOMapSize) editconst int WorldMapOffsetY;
var(MMOMapSize) int WorldMapSizeX;

defaultproperties
{
	IsExport = true
	AerialPhotoPrecent=54
	Begin Object Name=BrushComponent0
		CollideActors=false
		BlockActors=false
		BlockZeroExtent=false
		BlockNonZeroExtent=true
		BlockRigidBody=false
		bDisableAllRigidBody=true
	End Object

	bWorldGeometry=true
	bCollideActors=false
	bBlockActors=false
	bMovable=false

	//MapSizeType=EMST_TYPE3
	Priorrity=EMSP_PLow
	//same as AerialPhotoPrecent
	MiniMapPixelSize=70
	MiniMapTileRowNum=2
	MiniMapTileColNum=2
	MiniMapScale=1.0
	MiniMapOffsetX=0
	MiniMapOffsetY=512

	//same as AerialPhotoPrecent
	WorldMapPixelSize=70
	WorldMapTileRowNum=2
	WorldMapTileColNum=2
	WorldMapScale=1.0
	WorldMapOffsetX=0
	WorldMapOffsetY=512
	WorldMapSizeX=512
}
