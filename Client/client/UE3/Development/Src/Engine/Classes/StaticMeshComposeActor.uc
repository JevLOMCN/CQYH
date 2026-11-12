//=============================================================================
// StaticMeshActor.
// An actor that is drawn using a static mesh(a mesh that never changes, and
// can be cached in video memory, resulting in a speed boost).
// Note that PostBeginPlay() and SetInitialState() are never called for StaticMeshActors
// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
//=============================================================================

class StaticMeshComposeActor extends StaticMeshActor
	native
	placeable;

struct native FStaticMeshActorInfo
{
	var() StaticMesh StaticMesh; 
	var() Vector Location; 
	var() Rotator Rotation; 
	var() Vector DrawScale3D; 
};

var() editconst StaticMesh StaticMesh;
var() editconst array<FStaticMeshActorInfo> StaticMeshs; // Info of the original StaticMeshActor

defaultproperties
{
}