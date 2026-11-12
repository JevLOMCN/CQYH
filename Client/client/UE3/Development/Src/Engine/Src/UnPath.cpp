/*=============================================================================
	UnPath.cpp: Unreal pathnode placement
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
 =============================================================================*/

#include "EnginePrivate.h"
#include "UnPath.h"
#include "UnTerrain.h"
#include "EngineAIClasses.h"
#include "EngineMaterialClasses.h"
#include "EngineSequenceClasses.h"
#include "EngineInterpolationClasses.h"
#include "DebugRenderSceneProxy.h"

// oscar@snda
#if !SHIPPING_PC_GAME
bool g_drawPathDebug = false;
#endif

INT g_pickZOffset = 64;
int g_pathRadiusLagMS = 0;
// oscar@snda

//JimZhang@snda
AActor** g_ppActorList=NULL;
INT g_nActorNum = 0;
GRID_DATA* g_pBufGrid=NULL;
FVector g_BlockPosition;


IMPLEMENT_CLASS(APathBlockingVolume);
IMPLEMENT_CLASS(ARoute);

const FOctreeNodeBounds FNavigationOctree::RootNodeBounds(FVector(0,0,0),HALF_WORLD_MAX);
/** maximum objects we can have in one node before we split it */
#define MAX_OBJECTS_PER_NODE 10


void ANavigationPoint::TogglePathRendering(UBOOL bShouldDrawPaths)
{
	UBOOL bHasComponent = FALSE;
	for (INT Idx = 0; Idx < Components.Num(); Idx++)
	{
		UPathRenderingComponent *PathRenderer = Cast<UPathRenderingComponent>(Components(Idx));
		if (PathRenderer != NULL)
		{
			bHasComponent = TRUE;
			PathRenderer->SetHiddenGame(!bShouldDrawPaths);
			break;
		}
	}
	if (!bHasComponent)
	{
		UPathRenderingComponent *PathRenderer = ConstructObject<UPathRenderingComponent>(UPathRenderingComponent::StaticClass(),this);
		PathRenderer->SetHiddenGame(!bShouldDrawPaths);
		AttachComponent(PathRenderer);
	}
}

/**
 * Removes paths from all navigation points in the world, and removes all
 * path markers from actors in the world.
 */
#if WITH_EDITOR
void AScout::UndefinePaths()
{
	debugfSuppressed(NAME_DevPath,TEXT("Remove old reachspecs"));
	GWarn->BeginSlowTask( TEXT("Undefining Paths"), FALSE);
	//clear NavigationPoints
	const INT ProgressDenominator = FActorIteratorBase::GetProgressDenominator();
	const INT UpdateGranularity = ProgressDenominator / 5;
	INT LastUpdate =0;
	GWarn->StatusUpdatef( 0, ProgressDenominator, *LocalizeUnrealEd(TEXT("Undefining")) );
	for( FActorIterator It; It; ++It )
	{
		if(It.GetProgressNumerator() >= LastUpdate + UpdateGranularity)
		{
			GWarn->UpdateProgress( It.GetProgressNumerator(), ProgressDenominator );
			LastUpdate=It.GetProgressNumerator();
		}

		AActor*	Actor = *It;
		ANavigationPoint *Nav = Cast<ANavigationPoint>(Actor);
		if (Nav != NULL)
		{
			APylon* Pylon = Cast<APylon>(Actor);
			if (!(Nav->GetClass()->ClassFlags & CLASS_Placeable))
			{
				/* delete any nodes which aren't placeable, because they were automatically generated,
				  and will be automatically generated again. */
				GWorld->DestroyActor(Nav);
			}
			else
			{
				// reset the nav network id for this nav
				Nav->NetworkID = -1;
				// and clear any previous paths
				Nav->ClearPaths();
			}
		}
		else
		{
			Actor->ClearMarker();
		}
	}
	if ( GWorld->GetWorldInfo()->bPathsRebuilt )
	{
		debugf(TEXT("undefinepaths Clear paths rebuilt"));
	}

	GWorld->NavigationOctree->RemoveAllObjects();
	GWorld->GetWorldInfo()->bPathsRebuilt = FALSE;
	GWarn->EndSlowTask();
}

void AScout::ReviewPaths()
{
	GWarn->BeginSlowTask( *LocalizeUnrealEd(TEXT("ReviewingPathsE")), FALSE );

	if ( !GWorld->GetFirstNavigationPoint() )
	{
		GWarn->MapCheck_Add( MCTYPE_ERROR, NULL, *FString( LocalizeUnrealEd( "MapCheck_Message_NoNavigationPoints" ) ), TEXT( "NoNavigationPoints" ) );
	}
	else
	{
		INT NumDone = 0;
		INT NumPaths = 0;
		INT NumStarts = 0;
		for ( ANavigationPoint* N=GWorld->GetFirstNavigationPoint(); N!=NULL; N=N->nextNavigationPoint )
		{
			if ( Cast<APlayerStart>(N) )
			{
				NumStarts++;
			}
			NumPaths++;
		}
		
		if ( NumStarts < MinNumPlayerStarts )
		{
			GWarn->MapCheck_Add( MCTYPE_ERROR, GWorld->GetFirstNavigationPoint(), *FString::Printf( LocalizeSecure( LocalizeUnrealEd( "MapCheck_Message_NeedMorePlayerstarts" ), NumStarts, MinNumPlayerStarts ) ), TEXT( "NeedMorePlayerstarts" ) );
		}

		SetPathCollision(TRUE);
		GWarn->StatusUpdatef( 0, NumPaths, *LocalizeUnrealEd(TEXT("ReviewingPaths")) );
		for ( ANavigationPoint* N=GWorld->GetFirstNavigationPoint(); N!=NULL; N=N->nextNavigationPoint )
		{
			N->ReviewPath(this);
			GWarn->UpdateProgress( NumDone++, NumPaths );
		}
		SetPathCollision(FALSE);
	}

	GWarn->EndSlowTask();
}

void AActor::SetCollisionForPathBuilding(UBOOL bNowPathBuilding)
{
	if (bNowPathBuilding)
	{
		// turn off collision - for non-static actors with bPathColliding false and blocking volumes with no player collision
		if (!bDeleteMe)
		{
			if ( bBlockActors && bCollideActors &&
				(!bStatic && !bPathColliding))
			{
				bPathTemp = TRUE;
				SetCollision(FALSE, bBlockActors, bIgnoreEncroachers);
			}
			else
			{
				bPathTemp = FALSE;
			}
		}
	}
	else if( bPathTemp )
	{
		bPathTemp = FALSE;
		SetCollision(TRUE, bBlockActors, bIgnoreEncroachers);
	}
}

void AStaticMeshActor::SetCollisionForPathBuilding(UBOOL bNowPathBuilding)
{
	if( StaticMeshComponent && StaticMeshComponent->StaticMesh && StaticMeshComponent->StaticMesh->bCanBecomeDynamic )
	{
		if ( bNowPathBuilding )
		{
			bPathTemp = TRUE;
			SetCollision(FALSE, bBlockActors, bIgnoreEncroachers);
		}
		else if(bPathTemp)
		{
			bPathTemp = FALSE;
			SetCollision(TRUE, bBlockActors, bIgnoreEncroachers);
		}
	}
	else
	{
		Super::SetCollisionForPathBuilding(bNowPathBuilding);
	}
}
void APathBlockingVolume::SetCollisionForPathBuilding(UBOOL bNowPathBuilding)
{
	if ( bNowPathBuilding )
	{
		bPathTemp = TRUE;
		// turn ON collision when path building
		SetCollision(TRUE, bBlockActors, bIgnoreEncroachers);
	}
	else if ( bPathTemp )
	{
		bPathTemp = FALSE;
		// turn OFF collision when not path building
		SetCollision(FALSE, bBlockActors, bIgnoreEncroachers);		
	}
}

void AScout::SetPathCollision(UBOOL bEnabled)
{
	for( FActorIterator It; It; ++It )
	{
		if( !FPathBuilder::IsBuildingPaths() || FPathBuilder::IsPtWithinPathBuildBounds(It->Location) )
		{
			It->SetCollisionForPathBuilding(bEnabled);
		}
	}
}

INT AScout::PrunePathsForNav(ANavigationPoint* Nav)
{
	// default is just a pass through
	return Nav->PrunePaths();
}

void AScout::PrunePaths(INT NumPaths)
{
	// first, check for unnecessary PathNodes
	INT NumDone = 0;
	GWarn->StatusUpdatef( NumDone, NumPaths, *LocalizeUnrealEd(TEXT("CheckingForUnnecessaryPathNodes")) );
	for (ANavigationPoint* Nav = GWorld->GetFirstNavigationPoint(); Nav != NULL; Nav = Nav->nextNavigationPoint)
	{
		GWarn->UpdateProgress( NumDone, NumPaths );

		if (Nav->GetClass() == APathNode::StaticClass() && Nav->PathList.Num() > 0)
		{
			// if the PathNode is connected only to other nodes that have paths that encompass it, then it is unnecessary
			UBOOL bNecessary = TRUE;
			for (INT i = 0; i < Nav->PathList.Num(); i++)
			{
				// nodes with forced paths are always necessary
				if (Nav->PathList(i)->IsA(UForcedReachSpec::StaticClass()))
				{
					bNecessary = TRUE;
					break;
				}
				// if Nav is not designed for flying Pawns, ignore end nodes that are
				if ( *Nav->PathList(i)->End != NULL && (Nav->bFlyingPreferred || !Nav->PathList(i)->End.Nav()->bFlyingPreferred) &&
						(Nav->PhysicsVolume == NULL || Nav->PathList(i)->End->PhysicsVolume == NULL || Nav->PhysicsVolume->bWaterVolume == Nav->PathList(i)->End->PhysicsVolume->bWaterVolume) )
				{
					bNecessary = FALSE;
					UBOOL bOverlapsNav = FALSE;
					for (INT j = 0; j < Nav->PathList(i)->End.Nav()->PathList.Num(); j++)
					{
						UReachSpec* Spec = Nav->PathList(i)->End.Nav()->PathList(j);
						if (Spec != NULL && *Spec->End != NULL)
						{
							if (Spec->bAddToNavigationOctree && Spec->End != Nav && Spec->IsOnPath(Nav->Location, Nav->CylinderComponent->CollisionRadius))
							{
								bOverlapsNav = TRUE;
								break;
							}
						}
					}

					if (!bOverlapsNav)
					{
						bNecessary = TRUE;
						break;
					}
				}
			}
			if (!bNecessary)
			{
				// second pass: check that there are no special one-way connections that require this node
				for (ANavigationPoint* SecondNav = GWorld->GetFirstNavigationPoint(); SecondNav != NULL; SecondNav = SecondNav->nextNavigationPoint)
				{
					if (Nav->bFlyingPreferred || !SecondNav->bFlyingPreferred)
					{
						UReachSpec* Spec = SecondNav->GetReachSpecTo(Nav);
						if (Spec != NULL && !Spec->bAddToNavigationOctree && Nav->GetReachSpecTo(SecondNav) == NULL)
						{
							bNecessary = TRUE;
							break;
						}
					}
				}
				if (!bNecessary)
				{
					// consider as necessary if it's part of any scripted paths
					for (FActorIterator It; It; ++It)
					{
						ARoute* Route = Cast<ARoute>(*It);
						if (Route != NULL)
						{
							for (INT i = 0; i < Route->RouteList.Num(); i++)
							{
								if (Route->RouteList(i).Nav() == Nav)
								{
									bNecessary = TRUE;
									break;
								}
							}
						}
					}
					if (!bNecessary)
					{
						GWarn->MapCheck_Add( MCTYPE_WARNING, Nav, *FString::Printf( LocalizeSecure( LocalizeUnrealEd( "MapCheck_Message_UselessPathNode" ), *Nav->GetName() ) ), TEXT( "UselessPathNode" ) );
					}
				}
			}
		}
		NumDone++;
	}

	// prune excess reachspecs
	debugfSuppressed(NAME_DevPath,TEXT("Prune reachspecs"));
	INT NumPruned = 0;
	NumDone = 0;
	GWarn->StatusUpdatef( NumDone, NumPaths, *LocalizeUnrealEd(TEXT("PruningReachspecs")) );
	for (ANavigationPoint* Nav = GWorld->GetFirstNavigationPoint(); Nav != NULL; Nav = Nav->nextNavigationPoint)
	{
		GWarn->UpdateProgress( NumDone, NumPaths );
		NumPruned += PrunePathsForNav(Nav);
		NumDone++;
	}

	// second prune pass
	NumDone =0;
	for (ANavigationPoint* Nav = GWorld->GetFirstNavigationPoint(); Nav != NULL; Nav = Nav->nextNavigationPoint)
	{
		GWarn->UpdateProgress( NumDone, NumPaths );
		NumPruned += SecondPassPrunePathsForNav(Nav);
		NumDone++;
	}
	debugfSuppressed(NAME_DevPath,TEXT("Pruned %d reachspecs"), NumPruned);
}

void AScout::UpdateInterpActors(UBOOL &bProblemsMoving, TArray<USeqAct_Interp*> &InterpActs)
{
#if WITH_EDITORONLY_DATA
	bProblemsMoving = FALSE;
	// Keep track of which actors we are updating - so we can check for multiple actions updating the same actor.
	TArray<AActor*> MovedActors;
	for (TObjectIterator<USeqAct_Interp> InterpIt; InterpIt; ++InterpIt)
	{
		// For each SeqAct_Interp we flagged as having to be updated for pathing...
		USeqAct_Interp* Interp = *InterpIt;
		if(Interp->bInterpForPathBuilding)
		{
			InterpActs.AddItem(Interp);
			// Initialise it..
			Interp->InitInterp();
			for(INT j=0; j<Interp->GroupInst.Num(); j++)
			{
				// Check if any of the actors its working on have already been affected..
				AActor* InterpActor = Interp->GroupInst(j)->GroupActor;
				if(InterpActor)
				{
					if( MovedActors.ContainsItem(InterpActor) )
					{
						// This Actor is being manipulated by another SeqAct_Interp - so bail out.
						appMsgf(AMT_OK, LocalizeSecure(LocalizeUnrealEd("Error_MultipleInterpsReferToSameActor"), *InterpActor->GetName()));
						appMsgf(AMT_OK,TEXT("The full name of the interp actor which can't be interp'd is: %s.%s"),*InterpActor->GetOutermost()->GetName(),*InterpActor->GetName());
						bProblemsMoving = true;
					}
					else
					{
						// Things are ok - just add it to the list.
						MovedActors.AddItem(InterpActor);
					}
				}
			}

			if(!bProblemsMoving)
			{
				// If there were no problem initialising, first backup the current state of the actors..
				for(INT i=0; i<Interp->GroupInst.Num(); i++)
				{
					Interp->GroupInst(i)->SaveGroupActorState();
					AActor* GroupActor = Interp->GroupInst(i)->GetGroupActor();
					if( GroupActor )
					{
						const UBOOL bOnlyCaptureChildren = FALSE;
						Interp->SaveActorTransforms( GroupActor, bOnlyCaptureChildren );
					}
				}

				// ..then interpolate them to the 'PathBuildTime'.
				Interp->UpdateInterp( Interp->InterpData->PathBuildTime, true );
			}
			else
			{
				// If there were problems, term this interp now (so we won't try and restore its state later on).
				Interp->TermInterp();
			}
		}
	}
#endif // WITH_EDITORONLY_DATA
}

void AScout::RestoreInterpActors(TArray<USeqAct_Interp*> &InterpActs)
{
	// Reset any interpolated actors.
	const FString LocalizeBuildPathsResettingActors( LocalizeUnrealEd(TEXT("BuildPathsResettingActors")) );
	for(INT i=0; i<InterpActs.Num(); i++)
	{
		GWarn->StatusUpdatef( i, InterpActs.Num(), *LocalizeBuildPathsResettingActors );

		USeqAct_Interp* Interp = CastChecked<USeqAct_Interp>( InterpActs(i) );

		if(Interp->bInterpForPathBuilding)
		{
			for(INT j=0; j<Interp->GroupInst.Num(); j++)
			{
				Interp->GroupInst(j)->RestoreGroupActorState();
			}

			Interp->RestoreActorVisibilities();
			Interp->RestoreActorTransforms();
			Interp->TermInterp();
			Interp->Position = 0.f;
		}
	}
}

void AScout::BuildNavLists()
{
	// build the per-level nav lists
	UBOOL bBuildCancelled = GEngine->GetMapBuildCancelled();
	if (!bBuildCancelled)
	{
		TArray<FGuid> NavGuids;
		// reset the world's nav list
		GWorld->ResetNavList();
		// for each level,
		GWarn->StatusUpdatef( 0, GWorld->Levels.Num(), *LocalizeUnrealEd(TEXT("BuildPathsNavigationPointsOnBases")) );
		for ( INT LevelIdx = 0; LevelIdx < GWorld->Levels.Num(); LevelIdx++ )
		{
			ULevel *Level = GWorld->Levels(LevelIdx);
			if (Level != NULL)
			{
				Level->ResetNavList();
				UBOOL bHasPathNodes = FALSE;
				// look for any path nodes in this level
				for (INT ActorIdx = 0; ActorIdx < Level->Actors.Num() && !bBuildCancelled; ActorIdx++)
				{
					ANavigationPoint *Nav = Cast<ANavigationPoint>(Level->Actors(ActorIdx));
					if (Nav != NULL && !Nav->IsPendingKill())
					{
						// note that the level does have pathnodes
						bHasPathNodes = TRUE;
						// check for invalid cylinders
						if (Nav->CylinderComponent == NULL)
						{
							GWarn->MapCheck_Add( MCTYPE_WARNING, Nav, *FString::Printf( LocalizeSecure( LocalizeUnrealEd( "MapCheck_Message_CylinderComponentNull" ), *Nav->GetName() ) ), TEXT( "CylinderComponentNull" ) );
						}
						else
						{
							// add to the level path list
							Level->AddToNavList(Nav);
							// base the nav points as well
							Nav->FindBase();
							// init the nav for pathfinding
							Nav->InitForPathFinding();
							Nav->bHasCrossLevelPaths = FALSE;
							// and generate a guid for it as well
							if (!Nav->GetGuid()->IsValid() || NavGuids.ContainsItem(*Nav->GetGuid()))
							{
								Nav->NavGuid = appCreateGuid();
							}
							else
							{
								// save the existing guid to check for duplicates
								NavGuids.AddItem(*Nav->GetGuid());
							}
						}
					}

					// Init guids for all Path Object Interface actors
					IInterface_NavMeshPathObject* PO = InterfaceCast<IInterface_NavMeshPathObject>(Level->Actors(ActorIdx));
					if( PO != NULL )
					{
						PO->InitGuid( NavGuids );
					}

					bBuildCancelled = GEngine->GetMapBuildCancelled();
				}
				GWarn->UpdateProgress( LevelIdx, GWorld->Levels.Num() );
				// if there are any nodes, 
				if (bHasPathNodes)
				{
					// mark the level as dirty so that it will be saved
					Level->MarkPackageDirty();
					// and add to the world nav list
					GWorld->AddLevelNavList(Level);
				}
			}
		}
	}
}

/**
 * Clears all the paths and rebuilds them.
 *
 * @param	bReviewPaths	If TRUE, review paths if any were created.
 * @param	bShowMapCheck	If TRUE, conditionally show the Map Check dialog.
 * @param	bUndefinePaths	IF TRUE, paths will be undefined first
 */
void AScout::DefinePaths(UBOOL bReviewPaths, UBOOL bShowMapCheck, UBOOL bUndefinePaths)
{
	GWarn->BeginSlowTask( *LocalizeUnrealEd(TEXT("DefiningPaths")), FALSE );

	// Rest map has pathing errors warning.
	GWorld->GetWorldInfo()->bMapHasPathingErrors = FALSE;

	// Build Terrain Collision Data
	for (TObjectIterator<UTerrainComponent> TerrainIt; TerrainIt; ++TerrainIt)
	{
		TerrainIt->BuildCollisionData();
	}

	// remove old paths
	if (bUndefinePaths)
	{
		UndefinePaths();
	}

	// Position interpolated actors in desired locations for path-building.
	TArray<USeqAct_Interp*> InterpActs;
	UBOOL bProblemsMoving = FALSE;
	UpdateInterpActors(bProblemsMoving,InterpActs);
	UBOOL bBuildCancelled = GEngine->GetMapBuildCancelled();
	if (!bProblemsMoving && !bBuildCancelled)
	{
		// Setup all actor collision for path building
		SetPathCollision(TRUE);
		// number of paths, used for progress bar
		INT NumPaths = 0; 
		// Add NavigationPoint markers to any actors which want to be marked
		INT ProgressDenominator = FActorIteratorBase::GetProgressDenominator();
		SetCollision( FALSE, FALSE, bIgnoreEncroachers );
		GWarn->StatusUpdatef( 0, ProgressDenominator, *LocalizeUnrealEd(TEXT("BuildPathsDefining")) );
		for( FActorIterator It; It && !bBuildCancelled; ++It )
		{
			GWarn->UpdateProgress(It.GetProgressNumerator(), ProgressDenominator);
			AActor *Actor = *It;
			NumPaths += Actor->AddMyMarker(this);

			bBuildCancelled = GEngine->GetMapBuildCancelled();
		}
		// build the navigation lists
		BuildNavLists();
		// setup the scout
		SetCollision(TRUE, TRUE, bIgnoreEncroachers);

		// Adjust cover
		if( !bBuildCancelled )
		{
			//@note - needs to happen after addmymarker in case newly created cover was added
			Exec( TEXT("ADJUSTCOVER FROMDEFINEPATHS=1") );
		}

		// calculate and add reachspecs to pathnodes
		debugfSuppressed(NAME_DevPath,TEXT("Add reachspecs"));
		INT NumDone = 0;
		GWarn->StatusUpdatef( 0, NumPaths, *LocalizeUnrealEd(TEXT("BuildPathsAddingReachspecs")) );
		for( ANavigationPoint *Nav=GWorld->GetFirstNavigationPoint(); Nav && !bBuildCancelled; Nav=Nav->nextNavigationPoint )
		{
			Nav->addReachSpecs(this,FALSE);
			GWarn->UpdateProgress(NumDone++, NumPaths);
			debugfSuppressed( NAME_DevPath, TEXT("Added reachspecs to %s"),*Nav->GetName() );
			bBuildCancelled = GEngine->GetMapBuildCancelled();
		}

		// Called to eliminate paths so computing long paths 
		// doesn't bother iterating over specs that will be deleted anyway
		if( !bBuildCancelled )
		{
			PrunePaths(NumPaths);
		}

		Exec( *FString::Printf(TEXT("ADDLONGREACHSPECS NUMPATHS=%d"), NumPaths) );

		// allow scout to add any game specific special reachspecs
		if (!bBuildCancelled)
		{
			debugf(TEXT("%s add special paths"),*GetName());
			AddSpecialPaths(NumPaths, FALSE);
		}
		// turn off collision and reset temporarily changed actors
		SetPathCollision( FALSE );

		// Add forced specs if needed
		GWarn->BeginSlowTask( *LocalizeUnrealEd(TEXT("AddingForcedSpecs")), FALSE );
		{
			INT NavigationPointCount = 0;
			for( ANavigationPoint* Nav = GWorld->GetFirstNavigationPoint(); Nav && !bBuildCancelled; Nav = Nav->nextNavigationPoint )
			{
				Nav->AddForcedSpecs( this );

				GWarn->StatusUpdatef(NavigationPointCount, NumPaths, LocalizeSecure(LocalizeUnrealEd(TEXT("BuildPathsAddingForcedSpecsFE")), NavigationPointCount));

				bBuildCancelled = GEngine->GetMapBuildCancelled();
				NavigationPointCount++;
			}

			for( ANavigationPoint* Nav = GWorld->GetFirstNavigationPoint(); Nav && !bBuildCancelled; Nav = Nav->nextNavigationPoint )
			{
				// Call prune paths so MaxPathSize can be updated
				PrunePathsForNav(Nav);
				bBuildCancelled = GEngine->GetMapBuildCancelled();
			}
		}
		GWarn->EndSlowTask();

		if( !bBuildCancelled )
		{
			INT NavigationPointCount = 0;

			// sort PathLists
			// clear pathschanged flags and remove bases if in other level
			// handle specs crossing levels
			for( ANavigationPoint *Nav=GWorld->GetFirstNavigationPoint(); Nav && !bBuildCancelled; Nav=Nav->nextNavigationPoint )
			{
				Nav->SortPathList();
				Nav->bPathsChanged = FALSE;
				GWarn->StatusUpdatef( NavigationPointCount, NumPaths, LocalizeSecure(LocalizeUnrealEd(TEXT("BuildPathsCheckingCrossLevelReachspecsFE")), NavigationPointCount) );
				bBuildCancelled = GEngine->GetMapBuildCancelled();
				NavigationPointCount++;
			}

			if (!bBuildCancelled)
			{
				GWorld->GetWorldInfo()->bPathsRebuilt = TRUE;
				debugf(TEXT("SET paths rebuilt"));
			}
		}
	}
	// reset the interp actors moved for path building
	RestoreInterpActors(InterpActs);

	GWarn->EndSlowTask();

	// if the build was cancelled then invalidate paths
	if( bBuildCancelled )
	{
		UndefinePaths();
	}
	else
	{
		debugfSuppressed(NAME_DevPath,TEXT("All done"));
		if ( bReviewPaths && GWorld->GetFirstNavigationPoint() )
		{
			ReviewPaths();
		}
		if ( bShowMapCheck )
		{
			GWarn->MapCheck_ShowConditionally();
		}
	}
}

/** 
 * inserts an appropriate slot at the given index
 * extra code here to copy over manually assigned properties from adjacent slots to this one
 */
void InsertHomogeneousSlotToLink(const FVector& SlotLoc, ACoverLink* Link, INT InsertIdx, AScout* Scout)
{
	INT PrevIdx = InsertIdx-1;
	INT NextIdx = InsertIdx+1;
	

	
	INT NewIdx = Link->AddCoverSlot(SlotLoc, Link->GetSlotRotation(PrevIdx),InsertIdx,FALSE,Scout);					
	check(NewIdx == InsertIdx);
	check( PrevIdx >= 0 && NextIdx < Link->Slots.Num() );

	FCoverSlot& PrevSlot = Link->Slots(PrevIdx);
	FCoverSlot& NextSlot = Link->Slots(NextIdx);
	FCoverSlot& NewSlot = Link->Slots(NewIdx);

	BYTE ForcedCoverType = CT_None;
	if( PrevSlot.ForceCoverType != CT_None )
	{
		ForcedCoverType = PrevSlot.ForceCoverType;
	}

	if( NextSlot.ForceCoverType != ForcedCoverType )
	{
		if( NextSlot.ForceCoverType == CT_MidLevel || ForcedCoverType == CT_None )
		{
			ForcedCoverType = NextSlot.ForceCoverType;
		}
	}
	NewSlot.bForceCanPopUp = PrevSlot.bForceCanPopUp || PrevSlot.bForceCanPopUp;
	NewSlot.bEnabled = PrevSlot.bEnabled && PrevSlot.bEnabled;
	NewSlot.bAllowPopup = PrevSlot.bAllowPopup && PrevSlot.bAllowPopup;
	NewSlot.bAllowMantle = PrevSlot.bAllowMantle && PrevSlot.bAllowMantle;
	NewSlot.bAllowCoverSlip = PrevSlot.bAllowCoverSlip && PrevSlot.bAllowCoverSlip;
	NewSlot.bAllowClimbUp = PrevSlot.bAllowClimbUp && PrevSlot.bAllowClimbUp;
	NewSlot.bAllowSwatTurn = PrevSlot.bAllowSwatTurn && PrevSlot.bAllowSwatTurn;
	NewSlot.bForceNoGroundAdjust = PrevSlot.bForceNoGroundAdjust || PrevSlot.bForceNoGroundAdjust;
	NewSlot.bPlayerOnly = PrevSlot.bPlayerOnly || PrevSlot.bPlayerOnly;

	if( PrevSlot.LocationDescription != CoverDesc_None ) 
	{
		NewSlot.LocationDescription = PrevSlot.LocationDescription;
	}
	else 
	if( NextSlot.LocationDescription != CoverDesc_None )
	{
		NewSlot.LocationDescription = NextSlot.LocationDescription;
	}
}

/**
 * enforces a max distance between cover slots to ensure proper density
 * @param Link - link we're fixing up density for
 * @param Scout - scout ref
 * @return - number of slots we've added
 */
INT FixupSlotDensity(ACoverLink* Link, AScout* Scout)
{
	static UBOOL bSkipAll = FALSE;
	// if this has been disabled by the LD just bail now
	if( bSkipAll || ! Link->bDoAutoSlotDensityFixup )
	{
		return 0;
	}

	INT AddedSlotsThisLink = 0;

	// see if the link has any slots that failed to align to a surface
	UBOOL bLinkSlotFailedToFindSurface = FALSE;
	for ( INT SlotIndex = 0; SlotIndex < Link->Slots.Num(); ++SlotIndex )
	{
		if ( Link->Slots(SlotIndex).bFailedToFindSurface )
		{
			bLinkSlotFailedToFindSurface = TRUE;
			break;
		}
	}
	if( Link->Slots.Num() > 50 || bLinkSlotFailedToFindSurface )
	{
		warnf(NAME_Warning, TEXT("%s.%s has too many slots, or did not align properly.. interval enforcement is being skipped!"), *Link->GetOutermost()->GetName(), *Link->GetName());
	}
	else
	{

		// verify that there are slots at the proper interval 
		FVector PrevLoc = Link->GetSlotLocation(0);
		for (INT SlotIdx = 1; SlotIdx < Link->Slots.Num(); ++SlotIdx)
		{
			FVector Curr_Loc = Link->GetSlotLocation(SlotIdx);

			FVector Delta = (Curr_Loc-PrevLoc);
			FLOAT Dist = Delta.Size();

			// big vertical gaps probably indicate a slot is floating in space
			if( Abs<FLOAT>(Delta.Z) > 5.0f * Scout->NavMeshGen_EntityHalfHeight )
			{
				continue;
			}

			// too much of a gap, add a new slot!
			if ( Dist > Link->AutoCoverSlotInterval )
			{
				FVector Dir = Delta/Dist;

				// split space up into an even # of segments 				
				INT NumSegmentsToAdd = appFloor(Dist / Link->AutoCoverSlotInterval);
				FLOAT Offset = Dist / (NumSegmentsToAdd+1);
				FVector NewLoc(0.f);
				for( INT SegAddNum=1;SegAddNum <= NumSegmentsToAdd; ++SegAddNum )
				{
					NewLoc = PrevLoc + Dir * Offset * SegAddNum;
					InsertHomogeneousSlotToLink(NewLoc,Link,SlotIdx+SegAddNum-1,Scout);
					if( ++AddedSlotsThisLink > 50 )
					{
						warnf(NAME_Warning,TEXT("%s.%s - Slot spacing enforcement has added way too many slots, something has gone wrong!"), *Link->GetOutermost()->GetName(), *Link->GetName());
						break;
					}
				}
				SlotIdx = SlotIdx+NumSegmentsToAdd-1;
				PrevLoc = NewLoc;
			}
			else
			{
				PrevLoc = Curr_Loc;
			}

			if( AddedSlotsThisLink > 50 )
			{
				break;
			}
		}

		// after this has been run once on the slot, disable it so there is no chance of it affecting things later on
		Link->bDoAutoSlotDensityFixup=FALSE;
	}

	return AddedSlotsThisLink;
}
/** 
*	Called before addReachSpecs so that slot locations can be updated for proper placement 
*	of the slot markers, since slot location will be nudged around
*/
void AScout::AdjustCover( UBOOL bFromDefinePaths )
{
	GWarn->BeginSlowTask( *LocalizeUnrealEd(TEXT("AdjustCover")), FALSE);

	INT NumCover = 0;
	INT NumDone  = 0;
	TArray<ACoverLink*> Links;
	UBOOL bBuildCancelled = GEngine->GetMapBuildCancelled();

	INT TotalSlotsBeforeAdd = 0;
	AWorldInfo *Info = GWorld->GetWorldInfo();
	for( ACoverLink *Link = Info->CoverList; Link != NULL && !bBuildCancelled; Link = Link->NextCoverLink )
	{
		if( !FPathBuilder::IsBuildingPaths() || FPathBuilder::IsPtWithinPathBuildBounds(Link->Location) )
		{
			// perform initial order independent setup
			Link->SortSlots();
			NumCover++;
			TotalSlotsBeforeAdd+=Link->Slots.Num();
		}
	}

	// Initial alignment/orientation pass
	
	INT AddedSlots = 0;
	
	const FString LocalizeBuildCoverAdjustingCoverSlots( LocalizeUnrealEd(TEXT("BuildCoverAdjustingCoverSlots")) );
	for (ACoverLink *Link = Info->CoverList; Link != NULL && !bBuildCancelled; Link = Link->NextCoverLink)
	{
			
		if( !FPathBuilder::IsBuildingPaths() || FPathBuilder::IsPtWithinPathBuildBounds(Link->Location) )
		{

			// fixup slot density
			AddedSlots += FixupSlotDensity(Link,this);

			//GWarn->UpdateProgress(NumDone++, NumCover);
			for (INT SlotIdx = 0; SlotIdx < Link->Slots.Num(); SlotIdx++)
			{
				Link->AutoAdjustSlot( SlotIdx, FALSE );
			}
		}
		bBuildCancelled = GEngine->GetMapBuildCancelled();
	}

	debugf(TEXT("Added %i slots due to spacing! (had %i before adds) (%i added bytes)"), AddedSlots, TotalSlotsBeforeAdd, sizeof(FCoverSlot) * AddedSlots);

	GWarn->EndSlowTask();
}

/** 
*	Called, after reach specs have been added, to generate fire links and special move flags
*/
void AScout::BuildCover( UBOOL bFromDefinePaths )
{
	GWarn->BeginSlowTask( *LocalizeUnrealEd(TEXT("BuildingCover")), FALSE);

	DOUBLE BeginTime = appSeconds();
	INT NumCover = 0;
	INT NumDone  = 0;
	TArray<ACoverLink*> Links;
	UBOOL bBuildCancelled = GEngine->GetMapBuildCancelled();

	// disable collision for the scout so we don't accidentally interfere with linechecks
	SetCollision(FALSE,FALSE,bIgnoreEncroachers);

	for( INT LevelIdx = 0; LevelIdx < GWorld->Levels.Num(); LevelIdx++ )
	{
		ULevel* Level = GWorld->Levels(LevelIdx);
		Level->PurgeCrossLevelCoverArrays();
	}

	if( !bFromDefinePaths )
	{
		// setup the nav lists before building since they are most likely outdated
		BuildNavLists();
		SetPathCollision(TRUE);
	}

	AWorldInfo *Info = GWorld->GetWorldInfo();
	for (ACoverLink *Link = Info->CoverList; Link != NULL && !bBuildCancelled; Link = Link->NextCoverLink)
	{
		Link->ClearExposedFireLinks();
		NumCover++;
	}

	INT LastUpdate = 0;
	INT UpdateGranularity = NumCover / 5;

	if( !bFromDefinePaths )
	{
		// Adjust all slots if it wasn't already done in DefinePaths
		AdjustCover( FALSE );


		// Second alignment/orientation pass
		NumDone = 0;
		GWarn->StatusUpdatef( NumDone++, NumCover, *LocalizeUnrealEd(TEXT("BuildCoverAdjustingCoverSlots")) );
		for (ACoverLink *Link = Info->CoverList; Link != NULL && !bBuildCancelled; Link = Link->NextCoverLink)
		{
			if( !FPathBuilder::IsBuildingPaths() || FPathBuilder::IsPtWithinPathBuildBounds(Link->Location) )
			{

				if(NumDone >= (LastUpdate + UpdateGranularity))
				{
					GWarn->UpdateProgress( NumDone++, NumCover );
					LastUpdate=NumDone;
				}

				for (INT SlotIdx = 0; SlotIdx < Link->Slots.Num(); SlotIdx++)
				{
					Link->AutoAdjustSlot( SlotIdx, TRUE );
				}
			}
			bBuildCancelled = GEngine->GetMapBuildCancelled();
		}

		const FString LocalizeBuildCoverAdjustingCoverSlots( LocalizeUnrealEd(TEXT("BuildCoverAdjustingCoverSlots")) );
		LastUpdate =0;
		for( ACoverLink *Link = Info->CoverList; Link != NULL && !bBuildCancelled; Link = Link->NextCoverLink )
		{
			if(NumDone >= (LastUpdate + UpdateGranularity))
			{
				GWarn->StatusUpdatef( NumDone++, NumCover, *LocalizeBuildCoverAdjustingCoverSlots );
				LastUpdate=NumDone;
			}

			if( !FPathBuilder::IsBuildingPaths() || FPathBuilder::IsPtWithinPathBuildBounds(Link->Location) )
			{

				for( INT SlotIdx = 0; SlotIdx < Link->Slots.Num(); SlotIdx++ )
				{
					Link->BuildSlotInfo( SlotIdx );
				}
			}
			bBuildCancelled = GEngine->GetMapBuildCancelled();
		}
	}

	// Pass for fire links
	NumDone = 0;
	LastUpdate=0;
	for( ACoverLink *Link = Info->CoverList; Link != NULL && !bBuildCancelled; Link = Link->NextCoverLink, NumDone++ )
	{
		if(NumDone >= (LastUpdate + UpdateGranularity))
		{
			GWarn->StatusUpdatef( NumDone, NumCover, LocalizeSecure(LocalizeUnrealEd(TEXT("BuildCoverBuildingFireLinksF")), NumDone, NumCover) );
			LastUpdate = NumDone;
		}

		if( !FPathBuilder::IsBuildingPaths() || FPathBuilder::IsPtWithinPathBuildBounds(Link->Location) )
		{
			Link->BuildFireLinks( this );
		}

		bBuildCancelled = GEngine->GetMapBuildCancelled();
	}

	// Pass for other links (ie exposed links)
	NumDone = 0;
	LastUpdate = 0;
	for( ACoverLink *Link = Info->CoverList; Link != NULL && !bBuildCancelled; Link = Link->NextCoverLink, NumDone++ )
	{
		if(NumDone >= (LastUpdate + UpdateGranularity))
		{
			GWarn->StatusUpdatef( NumDone, NumCover, LocalizeSecure(LocalizeUnrealEd(TEXT("BuildCoverBuildingOtherLinksF")), NumDone, NumCover) );
			LastUpdate = NumDone;
		}

		if( !FPathBuilder::IsBuildingPaths() || FPathBuilder::IsPtWithinPathBuildBounds(Link->Location) )
		{
			Link->BuildOtherLinks( this );
		}

		bBuildCancelled = GEngine->GetMapBuildCancelled();
	}

	if( !bFromDefinePaths )
	{
		SetPathCollision(FALSE);
	}

	GWorld->DumpCoverStats();

	debugf(TEXT("Time: %.2f"),appSeconds()-BeginTime);

	GWarn->EndSlowTask();
}

void AScout::FinishPathBuild()
{
	UBOOL bBuildCancelled = GEngine->GetMapBuildCancelled();
	if( !bBuildCancelled )
	{
		AWorldInfo *Info = GWorld->GetWorldInfo();
		// check for links across levels
		for (ACoverLink *Link = Info->CoverList; Link != NULL; Link = Link->NextCoverLink)
		{
			for (INT SlotIdx = 0; SlotIdx < Link->Slots.Num(); SlotIdx++)
			{
				for (INT Idx = 0; Idx < Link->Slots(SlotIdx).FireLinks.Num(); Idx++)
				{
					FFireLink &FireLink = Link->Slots(SlotIdx).FireLinks(Idx);
					// Get info destination of the firelink
					FCoverInfo DestInfo;
					if( !Link->GetFireLinkTargetCoverInfo( SlotIdx, Idx, DestInfo ) )
					{
						continue;
					}

					FireLink.SetCoverRefIdx( Link->FindCoverReference( DestInfo.Link, DestInfo.SlotIdx ) );
				}
			}
		}

		// remove bases in other levels
		for( ANavigationPoint *Nav = GWorld->GetFirstNavigationPoint(); Nav && !bBuildCancelled; Nav=Nav->nextNavigationPoint )
		{
			if (Nav->Base != NULL && (Nav->Base->GetOutermost() != Nav->GetOutermost()))
			{
				Nav->SetBase(NULL);
				Nav->bPathsChanged = FALSE;
				Nav->PostEditChange();
				// check for cross level paths,
				Nav->bHasCrossLevelPaths = FALSE;
				for (INT PathIdx = 0; PathIdx < Nav->PathList.Num(); PathIdx++)
				{
					UReachSpec *Spec = Nav->PathList(PathIdx);
					if (Spec->Start != NULL && *Spec->End != NULL &&
						Spec->Start == Nav && !Spec->bPruned &&
						Spec->Start->GetOutermost() != Spec->End->GetOutermost())
					{
						ANavigationPoint *Start = Spec->Start;
						ANavigationPoint *End = Spec->End.Nav();
						debugf(TEXT("Path crosses levels %s vs %s"),*Start->GetFullName(),*End->GetFullName());
						Spec->End.Guid = *End->GetGuid();
						Nav->bHasCrossLevelPaths = TRUE;
					}
				}
			}
		}
	}
}


//------------------------------------------------------------------------------------------------
// certain actors add paths to mark their position
INT ANavigationPoint::AddMyMarker(AActor *S)
{
	return 1;
}

INT APathNode::AddMyMarker(AActor *S)
{
	GWorld->GetWorldInfo()->bHasPathNodes = true;
	return 1;
}

FVector ALadderVolume::FindCenter()
{
	FVector Center(0.f,0.f,0.f);
	check(BrushComponent);
	check(Brush);
	for(INT PolygonIndex = 0;PolygonIndex < Brush->Polys->Element.Num();PolygonIndex++)
	{
		FPoly&	Poly = Brush->Polys->Element(PolygonIndex);
		FVector NewPart(0.f,0.f,0.f);
		for(INT VertexIndex = 0;VertexIndex < Poly.Vertices.Num();VertexIndex++)
			NewPart += Poly.Vertices(VertexIndex);
		NewPart /= Poly.Vertices.Num();
		Center += NewPart;
	}
	Center /= Brush->Polys->Element.Num();
	return Center;
}

INT ALadderVolume::AddMyMarker(AActor *S)
{
	check(BrushComponent);

	if ( !bAutoPath || !Brush )
		return 0;

	FVector Center = FindCenter();
	Center = LocalToWorld().TransformFVector(Center);

	AScout* Scout = Cast<AScout>(S);
	if ( !Scout )
		return 0 ;

	UClass *pathClass = AAutoLadder::StaticClass();
	AAutoLadder* DefaultLadder = (AAutoLadder*)( pathClass->GetDefaultActor() );

	// find ladder bottom
	FCheckResult Hit(1.f);
	GWorld->SingleLineCheck(Hit, this, Center - 10000.f * ClimbDir, Center, TRACE_World);
	if ( Hit.Time == 1.f )
		return 0;
	FVector Position = Hit.Location + DefaultLadder->CylinderComponent->CollisionHeight * ClimbDir;

	// place Ladder at bottom of volume
	GWorld->SpawnActor(pathClass, NAME_None, Position, FRotator(0,0,0), NULL, FALSE, FALSE, this);

	// place Ladder at top of volume + 0.5 * CollisionHeight of Ladder
	Position = FindTop(Center + 500.f * ClimbDir);
	GWorld->SpawnActor(pathClass, NAME_None, Position - 5.f * ClimbDir, FRotator(0,0,0), NULL, FALSE, FALSE, this);
	return 2;
}

// find the edge of the brush in the ClimbDir direction
FVector ALadderVolume::FindTop(FVector V)
{
	if ( Encompasses(V) )
		return FindTop(V + 500.f * ClimbDir);

	// trace back to brush edge from this outside point
	FCheckResult Hit(1.f);
	check(BrushComponent);
	BrushComponent->LineCheck( Hit, V - 10000.f * ClimbDir, V, FVector(0.f,0.f,0.f), 0 );
	return Hit.Location;

}
#endif

/**
 * Called when a property value from a member struct or array has been changed in the editor.
 */
void ALadderVolume::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	// Check to see if a member of the WallDir property was modified
	FEditPropertyChain::TDoubleLinkedListNode* MemberNode = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
	UProperty* OutermostProperty = MemberNode->GetValue();
	if ( OutermostProperty != NULL )
	{
		FName OuterPropertyName = OutermostProperty->GetFName();
		if ( OuterPropertyName == TEXT("WallDir") )
		{
			WallDirArrow->Rotation = WallDir;
		}
	}

	// Now call the parent implementation which will update the volume brush
	FPropertyChangedEvent PropertyEvent(PropertyChangedEvent.PropertyChain.GetActiveNode()->GetValue());
	Super::PostEditChangeProperty( PropertyEvent );
}

//------------------------------------------------------------------------------------------------
//Private methods

#if WITH_EDITOR
UBOOL FPathBuilder::NavMeshWorld()
{
	for( FActorIterator It; It; ++It )
	{
		APylon* Pylon = Cast<APylon>(*It);
		if( Pylon != NULL )
		{
			return TRUE;
		}
	}
	return FALSE;
}

#define PATHBUILDOP(x) \
	if ( MapCancelled == NULL || !(*MapCancelled)() ) \
{ \
	FPathBuilder::Exec( TEXT(x) ); \
} \

void FPathBuilder::BuildPaths(UBOOL bBuildReachSpecs, UBOOL bOnlyBuildSelected, MapCancelledFunc MapCancelled)
{
	bBuildingPaths = TRUE;
	PathBuildBounds.Init();
	PathBuildBounds.IsValid = FALSE;
	PATHBUILDOP("PREDEFINEPATHS");

	UBOOL bNavMesh = NavMeshWorld();
	if ( bNavMesh )
	{
		if( bOnlyBuildSelected )
		{		
			PATHBUILDOP("GENERATENAVMESH SHOWMAPCHECK=0 ONLYBUILDSELECTED=1");
		}
		else
		{
			PATHBUILDOP("GENERATENAVMESH SHOWMAPCHECK=0 ONLYBUILDSELECTED=0");
		}

		if(bBuildReachSpecs)
		{
			PATHBUILDOP("DEFINEPATHS REVIEWPATHS=1 SHOWMAPCHECK=0 UNDEFINEPATHS=0");
		}
	}
	else
	{
		PATHBUILDOP("DEFINEPATHS REVIEWPATHS=1 SHOWMAPCHECK=0 UNDEFINEPATHS=1");
	}

	if( !bOnlyBuildSelected || PathBuildBounds.IsValid )
	{
		PATHBUILDOP("BUILDCOVER FROMDEFINEPATHS=0");
	}
	
	PATHBUILDOP("POSTDEFINEPATHS");
	if(bBuildReachSpecs||!bNavMesh)
	{
		PATHBUILDOP("BUILDNETWORKIDS");
	}
	PATHBUILDOP("FINISHPATHBUILD");
	bBuildingPaths = FALSE;
}

// james@snda
UBOOL FPathBuilder::BuildGrid()
{
	UBOOL buildResult = FALSE;
	Scout = GetScout();
	if( Scout )
	{
		buildResult = Scout->GenerateGridData();
	}
	DestroyScout();

	return buildResult;
}
// james@snda

void FPathBuilder::AddBoxToPathBuildBounds( const FBox& Box )
{
	if( !PathBuildBounds.IsValid )
	{
		PathBuildBounds.Init();
	}
	PathBuildBounds+=Box;
}
UBOOL FPathBuilder::IsPtWithinPathBuildBounds(const FVector& Point )
{
	if( PathBuildBounds.IsValid )
	{
		return PathBuildBounds.IsInside(Point);
	}
	return TRUE;	
}
#endif

AScout* FPathBuilder::Scout = NULL;
UBOOL FPathBuilder::bBuildingPaths = FALSE;
DWORD FPathBuilder::LoadedPathVersionNum = 0;
FBox FPathBuilder::PathBuildBounds = FBox();

void FPathBuilder::DestroyScout()
{
	for (FActorIterator It; It; ++It)
	{
		AScout* TheScout = Cast<AScout>(*It);
		if (TheScout != NULL)
		{
			if (TheScout->Controller != NULL)
			{
				GWorld->DestroyActor(TheScout->Controller);
			}
			GWorld->DestroyActor(TheScout);
		}
	}

	FPathBuilder::Scout = NULL;
}

/*getScout()
Find the scout actor in the level. If none exists, add one.
*/
AScout* FPathBuilder::GetScout()
{
	AScout* NewScout = FPathBuilder::Scout;
	if (NewScout == NULL || NewScout->IsPendingKill())
	{
		NewScout = NULL;
		FString ScoutClassName = GEngine->ScoutClassName;
		UClass *ScoutClass = FindObject<UClass>(ANY_PACKAGE, *ScoutClassName);
		if (ScoutClass == NULL)
		{
			appErrorf(TEXT("Failed to find scout class for path building!"));
		}
		// search for an existing scout
		for( FActorIterator It; It && NewScout == NULL; ++It )
		{
			if (It->IsA(ScoutClass))
			{
				NewScout = Cast<AScout>(*It);
			}
		}
		// if one wasn't found create a new one
		if (NewScout == NULL)
		{
			NewScout = (AScout*)GWorld->SpawnActor(ScoutClass);
			check(NewScout != NULL);
			// set the scout as transient so it can't get accidentally saved in the map
			NewScout->SetFlags(RF_Transient);
			// give it a default controller
			
			// MT->Make sure scout always spawns in the persistent level! so it doesn't get orphaned in sublevels
			ULevel* OldLevel = GWorld->CurrentLevel;
			GWorld->CurrentLevel = GWorld->PersistentLevel;
			NewScout->Controller = (AController*)GWorld->SpawnActor(FindObjectChecked<UClass>(ANY_PACKAGE, TEXT("AIController")));
			GWorld->CurrentLevel = OldLevel;

			check(NewScout->Controller != NULL);
			NewScout->Controller->SetFlags(RF_Transient);
		}
		if (NewScout != NULL)
		{
			// initialize scout collision properties
			NewScout->SetCollision( TRUE, TRUE, NewScout->bIgnoreEncroachers );
			NewScout->bCollideWorld = 1;
			NewScout->SetZone( 1,1 );
			NewScout->PhysicsVolume = GWorld->GetWorldInfo()->GetDefaultPhysicsVolume();
			NewScout->SetVolumes();
			NewScout->bHiddenEdTemporary = TRUE;
			NewScout->SetPrototype();

		}
	}
	return NewScout;
}

#if WITH_EDITOR
void FPathBuilder::Exec( const TCHAR* Str )
{
	Scout = GetScout();
	if( Scout )
	{
		Scout->Exec( Str );
	}
	DestroyScout();
}

void AScout::Exec( const TCHAR* Str )
{
	if( ParseCommand( &Str, TEXT("PREDEFINEPATHS") ) )
	{
		// Give kismet objects a chance to influence path building
		for( TObjectIterator<USequenceObject> SeqObj; SeqObj; ++SeqObj )
		{
			SeqObj->PrePathBuild( this );
		}
	}
	else if( ParseCommand( &Str, TEXT("POSTDEFINEPATHS") ) )
	{
		GWarn->BeginSlowTask( TEXT("PostPathBuild..."), FALSE );
		SetPathCollision(TRUE);

		INT Count = 0;
		for( TObjectIterator<USequenceObject> SeqObj; SeqObj; ++SeqObj)
		{
			Count++;
		}
		INT Idx=0;
		for( TObjectIterator<USequenceObject> SeqObj; SeqObj; ++SeqObj,++Idx)
		{
			SeqObj->PostPathBuild( this );
		}

		SetPathCollision(FALSE);

		GWarn->EndSlowTask();
	}
	else if( ParseCommand( &Str, TEXT("DEFINEPATHS") ) )
	{
		UBOOL bReviewPaths	= FALSE;
		UBOOL bShowMapCheck = FALSE;
		UBOOL bUndefinePaths = FALSE;

		ParseUBOOL( Str, TEXT("REVIEWPATHS="),  bReviewPaths  );
		ParseUBOOL( Str, TEXT("SHOWMAPCHECK="), bShowMapCheck );
		ParseUBOOL( Str, TEXT("UNDEFINEPATHS="), bUndefinePaths );

		DefinePaths( bReviewPaths, bShowMapCheck,bUndefinePaths );
	}
	else if( ParseCommand( &Str, TEXT("BUILDCOVER") ) )
	{
		UBOOL bFromDefinePaths = FALSE;

		ParseUBOOL( Str, TEXT("FROMDEFINEPATHS="), bFromDefinePaths );

		BuildCover( bFromDefinePaths );
		FPathBuilder::Exec( TEXT("BUILDCOMBATZONES FROMDEFINEPATHS=0") );
	}
	else if( ParseCommand( &Str, TEXT("SETPATHCOLLISION") ) )
	{
		UBOOL bEnabled = FALSE;

		ParseUBOOL( Str, TEXT("ENABLED="), bEnabled );

		SetPathCollision( bEnabled );
	}
	else if( ParseCommand( &Str, TEXT("ADDLONGREACHSPECS") ) )
	{
		INT NumPaths = 0;
		Parse( Str, TEXT("NUMPATHS="), NumPaths );
		AddLongReachSpecs( NumPaths );
	}	
	else if( ParseCommand( &Str, TEXT("BUILDNETWORKIDS") ) )
	{
		ANavigationPoint::BuildNetworkIDs();
	}
	else if( ParseCommand( &Str, TEXT("FINISHPATHBUILD") ) )
	{
		FinishPathBuild();
	}

	// NAV MESH
	if( ParseCommand( &Str, TEXT("GENERATENAVMESH") ) )
	{
		UBOOL bShowMapCheck = FALSE;
		UBOOL bOnlyBuildSelected = FALSE;
		ParseUBOOL( Str, TEXT("SHOWMAPCHECK="), bShowMapCheck );
		ParseUBOOL( Str, TEXT("ONLYBUILDSELECTED="), bOnlyBuildSelected );

		//------------------------------------------------------------------------
		// oscar@snda
// 		extern bool GEnableExportServerNavMeshExport;
// 		GEnableExportServerNavMeshExport = true;
// 		GenerateNavMesh( bShowMapCheck, bOnlyBuildSelected );
// 		GEnableExportServerNavMeshExport = false;
		// oscar@snda
		//------------------------------------------------------------------------

		GenerateNavMesh( bShowMapCheck, bOnlyBuildSelected );
	}
}

/** Do a second pass to add long range reachspecs */
void AScout::AddLongReachSpecs( INT NumPaths )
{
	debugfSuppressed(NAME_DevPath, TEXT("Add long range reachspecs"));
	INT NumDone = 0;
	UBOOL bBuildCancelled = GEngine->GetMapBuildCancelled();
	const FString LocalizeBuildPathsAddingLongRangeReachspecs( LocalizeUnrealEd(TEXT("BuildPathsAddingLongRangeReachspecs")) );
	for( ANavigationPoint *Nav=GWorld->GetFirstNavigationPoint(); Nav && !bBuildCancelled; Nav=Nav->nextNavigationPoint )
	{
		GWarn->StatusUpdatef( NumDone, NumPaths, *LocalizeBuildPathsAddingLongRangeReachspecs );
		bBuildCancelled = GEngine->GetMapBuildCancelled();
		Nav->AddLongPaths(this, FALSE);
		NumDone++;
	}

	// Called again to potentially prune new paths
	if( !bBuildCancelled )
	{
		PrunePaths(NumPaths);
	}
}

FVector AScout::GetMaxSize()
{
	FLOAT MaxRadius = 0.f;
	FLOAT MaxHeight = 0.f;
	for (INT i = 0; i < PathSizes.Num(); i++)
	{
		MaxRadius = Max<FLOAT>(MaxRadius, PathSizes(i).Radius);
		MaxHeight = Max<FLOAT>(MaxHeight, PathSizes(i).Height);
	}
	return FVector(MaxRadius, MaxHeight, 0.f);
}
#endif


void AScout::PostBeginPlay()
{
	Super::PostBeginPlay();
	SetPrototype();
}

void AScout::SetPrototype()
{
}

AScout* AScout::GetGameSpecificDefaultScoutObject()
{
	static AScout* CachedScoutDefaultObject=NULL;


	if(CachedScoutDefaultObject == NULL && !(GIsGarbageCollecting))
	{
		UClass* ScoutClass = UObject::StaticLoadClass(AScout::StaticClass(), NULL, *GEngine->ScoutClassName, NULL, LOAD_None, NULL);

		if(ScoutClass != NULL)
		{
			CachedScoutDefaultObject = ScoutClass->GetDefaultObject<AScout>();
		}
	}

	return CachedScoutDefaultObject;

}
//=============================================================================
// UReachSpec

static void DrawLineArrow(FPrimitiveDrawInterface* PDI,const FVector &start,const FVector &end,const FColor &color,FLOAT mag)
{
	// draw a pretty arrow
	FVector dir = end - start;
	FLOAT dirMag = dir.Size();
	dir /= dirMag;
	FVector YAxis, ZAxis;
	dir.FindBestAxisVectors(YAxis,ZAxis);
	FMatrix arrowTM(dir,YAxis,ZAxis,start);
	DrawDirectionalArrow(PDI,arrowTM,color,dirMag,mag,SDPG_World);
}


/**
* Adds geometery for a UReachSpec object.
* @param ReachSpec		ReachSpec to add geometry for.
*/
void UReachSpec::AddToDebugRenderProxy(FDebugRenderSceneProxy* DRSP)
{
	if ( Start && *End && !End->IsPendingKill() )
	{
		FPlane PathColorValue = PathColor();
		FVector Dir = (End->Location - Start->Location);
		FLOAT Size = Dir.Size();
		Dir /= Size;
		UBOOL bOneWay = (End.Nav()->GetReachSpecTo(Start) == NULL);
		
		FLOAT StartDist = Size * 0.5f;
		FLOAT EndDist = (GIsGame) ? (Max<FLOAT>(Size-10.f,5.f)) : (Max<FLOAT>(Size-20.f,5.f));
		if(bOneWay && AScout::GetGameSpecificDefaultScoutObject()->bHightlightOneWayReachSpecs)
		{
			// if there's no edge going back, draw a red dashed line from middle back to non connected point
			new(DRSP->DashedLines) FDebugRenderSceneProxy::FDashedLine(Start->Location + Dir * (Size - EndDist) ,Start->Location + (Dir * StartDist),FLinearColor(255,0,0),6.f);
		}

		FLinearColor Color = FLinearColor(PathColorValue.X,PathColorValue.Y,PathColorValue.Z,PathColorValue.W);
		//new(DRSP->ArrowLines) FDebugRenderSceneProxy::FArrowLine(Start->Location + (Dir * StartDist), Start->Location + Dir * EndDist,Color);
		new(DRSP->Lines) FDebugRenderSceneProxy::FDebugLine(Start->Location, End->Location,Color);
		if((reachFlags & R_JUMP) == R_JUMP)
		{
			new(DRSP->Stars) FDebugRenderSceneProxy::FWireStar((Start->Location + End->Location)*0.5f,Color,25.f);
		}
	}
	else
	{
		// check for connections across levels
		if ( GIsEditor && Start && End.Guid.IsValid() )
		{
			for (FActorIterator It; It; ++It)
			{
				ANavigationPoint *Nav = Cast<ANavigationPoint>(*It);
				if (Nav != NULL && *Nav->GetGuid() == End.Guid)
				{

					FVector Dir = (Nav->Location - Start->Location);
					FLOAT Size = Dir.Size();
					Dir /= Size;
					//new(DRSP->ArrowLines) FDebugRenderSceneProxy::FArrowLine(Start->Location + (Dir * Min<FLOAT>(10.f,Size * 0.2f)), Nav->Location + Dir * (Max<FLOAT>(Size-10.f,5.f)), FColor(128,128,128,255));

					if( GIsEditor &&
						Start->IsSelected() && 
						Nav->IsSelected() )
					{
						const INT Idx = DRSP->Cylinders.Add();
						FDebugRenderSceneProxy::FWireCylinder &Cylinder = DRSP->Cylinders(Idx);

						Cylinder.Base = Start->Location + FVector(0,0,(CollisionHeight - Start->CylinderComponent->CollisionHeight));
						Cylinder.Color = FColor(255,0,0);
						Cylinder.Radius = CollisionRadius;
						Cylinder.HalfHeight = CollisionHeight;
					}

					// no need to continue searching
					break;
				}
			}
		}
	}
}

void FNavigationOctreeObject::SetOwner(UObject* InOwner)
{
	Owner = InOwner;
	OwnerType = 0;
	if (Cast<ANavigationPoint>(Owner))
	{
		OwnerType |= NAV_NavigationPoint;
	}
	else if (Cast<UReachSpec>(Owner))
	{
		OwnerType |= NAV_ReachSpec;
	}
}

void FNavigationOctreeObject::SetBox(const FBox& InBoundingBox)
{
	UBOOL bIsInOctree = (OctreeNode != NULL);
	if (bIsInOctree)
	{
		GWorld->NavigationOctree->RemoveObject(this);
	}
	BoundingBox = InBoundingBox;
	BoxCenter = BoundingBox.GetCenter();
	if (bIsInOctree)
	{
		GWorld->NavigationOctree->AddObject(this);
	}
}

UBOOL FNavigationOctreeObject::OverlapCheck(const FBox& TestBox)
{
	if (Owner != NULL && OwnerType & NAV_ReachSpec)
	{
		return ((UReachSpec*)Owner)->NavigationOverlapCheck(TestBox);
	}
	else
	{
		// the default is to only use the AABB check
		return false;
	}
}

/** destructor, removes us from the octree if we're still there */
FNavigationOctreeObject::~FNavigationOctreeObject()
{
	if (OctreeNode != NULL && GWorld != NULL && GWorld->NavigationOctree != NULL)
	{
		GWorld->NavigationOctree->RemoveObject(this);
	}
}

/** filters an object with the given bounding box through this node, either adding it or passing it to its children
 * if the object is added to this node, the node may also be split if it exceeds the maximum number of objects allowed for a node without children
 * assumes the bounding box fits in this node and always succeeds
 * @param Object the object to filter
 * @param NodeBounds the bounding box for this node
 */
void FNavigationOctreeNode::FilterObject(FNavigationOctreeObject* Object, const FOctreeNodeBounds& NodeBounds)
{
	INT ChildIndex = -1;
	if (Children != NULL)
	{
		ChildIndex = FindChild(NodeBounds, Object->BoundingBox);
	}
	if (ChildIndex != -1)
	{
		Children[ChildIndex].FilterObject(Object, FOctreeNodeBounds(NodeBounds, ChildIndex));
	}
	else
	{
		if (Children == NULL && Objects.Num() >= MAX_OBJECTS_PER_NODE)
		{
			Children = new FNavigationOctreeNode[8];

			// remove and reinsert primitives at this node, to see if they belong at a lower level
			TArray<FNavigationOctreeObject*> ReinsertObjects(Objects);
			Objects.Empty();
			ReinsertObjects.AddItem(Object);
			
			for (INT i = 0; i < ReinsertObjects.Num(); i++)
			{
				FilterObject(ReinsertObjects(i), NodeBounds);
			}
		}
		else
		{
			// store the object here
			Objects.AddItem(Object);
			Object->OctreeNode = this;
		}
	}
}

/** returns all objects in this node and all children whose bounding box intersects with the given sphere
 * @param Point the center of the sphere
 * @param RadiusSquared squared radius of the sphere
 * @param Extent bounding box for the sphere
 * @param OutObjects (out) all objects found in the radius
 * @param NodeBounds the bounding box for this node
 */
void FNavigationOctreeNode::RadiusCheck(const FVector& Point, FLOAT RadiusSquared, const FBox& Extent, TArray<FNavigationOctreeObject*>& OutObjects, const FOctreeNodeBounds& NodeBounds)
{
	// iterate through all the objects and add all the ones whose origin is within the radius
	for (INT i = 0; i < Objects.Num(); i++)
	{
		if (SphereAABBIntersectionTest(Point, RadiusSquared, Objects(i)->BoundingBox))
		{
			OutObjects.AddItem(Objects(i));
		}
	}

	// check children
	if (Children != NULL)
	{
		INT ChildIdx[8];
		INT NumChildren = FindChildren(NodeBounds, Extent, ChildIdx);
		for (INT i = 0; i < NumChildren; i++)
		{
			Children[ChildIdx[i]].RadiusCheck(Point, RadiusSquared, Extent, OutObjects, FOctreeNodeBounds(NodeBounds, ChildIdx[i]));
		}
	}
}

/** checks the given box against the objects in this node and returns the first object found that intersects with it
 * recurses down to children that intersect the box
 * @param Box the box to check
 * @param NodeBounds the bounding box for this node
 * @return the first object found that intersects, or NULL if none do
 */
void FNavigationOctreeNode::OverlapCheck(const FBox& Box, TArray<FNavigationOctreeObject*>& OutObjects, const FOctreeNodeBounds& NodeBounds)
{
	const INT ObjectsNum = Objects.Num();
	// iterate through all the objects; if we find one that intersects the given box, return it
	for (INT i = 0; i < ObjectsNum; i++)
	{
		if (i+2 < ObjectsNum)
		{
			CONSOLE_PREFETCH(&Objects(i+2)->BoundingBox);		
		}
		if (Objects(i)->BoundingBox.Intersect(Box) && !Objects(i)->OverlapCheck(Box))
		{
			OutObjects.AddItem(Objects(i));
		}
	}

	// check children
	if (Children != NULL)
	{
		INT ChildIdx[8];
		INT NumChildren = FindChildren(NodeBounds, Box, ChildIdx);
		for (INT i = 0; i < NumChildren; i++)
		{
			Children[ChildIdx[i]].OverlapCheck(Box, OutObjects, FOctreeNodeBounds(NodeBounds, ChildIdx[i]));
		}
	}
}

/** adds an object with the given bounding box
 * @param Object the object to add
 * @note this method assumes the object is not already in the octree
 */
void FNavigationOctree::AddObject(FNavigationOctreeObject* Object)
{
	//debugf(TEXT("Adding object %s to octree"),*Object->GetOwner<UObject>()->GetPathName());
	const FBox& BoundingBox = Object->BoundingBox;
	if (	BoundingBox.Max.X < -HALF_WORLD_MAX || BoundingBox.Min.X > HALF_WORLD_MAX ||
			BoundingBox.Max.Y < -HALF_WORLD_MAX || BoundingBox.Min.Y > HALF_WORLD_MAX ||
			BoundingBox.Max.Z < -HALF_WORLD_MAX || BoundingBox.Min.Z > HALF_WORLD_MAX )
	{
		debugf(NAME_Warning, TEXT("%s is outside the world"), *Object->GetOwner<UObject>()->GetName());
	}
	else
	{
		// verify that the object wasn't already in the octree
		checkSlow(Object->OctreeNode == NULL);

		RootNode->FilterObject(Object, RootNodeBounds);
	}
}

/** removes the given object from the octree
 * @param Object the object to remove
 * @return true if the object was in the octree and was removed, false if it wasn't in the octree
 */
UBOOL FNavigationOctree::RemoveObject(FNavigationOctreeObject* Object)
{
	if (Object->OctreeNode == NULL)
	{
		return false;
	}
	else
	{
		//debugf(TEXT("Removing object %s from octree"),*Object->GetOwner<UObject>()->GetPathName());
		UBOOL bResult = Object->OctreeNode->RemoveObject(Object);
		if (!bResult)
		{
			debugf(NAME_Warning, TEXT("Attempt to remove %s from navigation octree but it isn't there"), *Object->GetOwner<UObject>()->GetName());
		}		
		Object->OctreeNode = NULL;

		return bResult;
	}
}

/** 
 * Removes all objects from octree.
 */
void FNavigationOctree::RemoveAllObjects()
{
	delete RootNode;
	RootNode = new FNavigationOctreeNode;
}

/** counts the number of nodes and objects there are in the octree
 * @param NumNodes (out) incremented by the number of nodes (this one plus all children)
 * @param NumObjects (out) incremented by the total number of objects in this node and its child nodes
 */
void FNavigationOctreeNode::CollectStats(INT& NumNodes, INT& NumObjects)
{
	NumNodes++;
	NumObjects += Objects.Num();
	if (Children != NULL)
	{
		for (INT i = 0; i < 8; i++)
		{
			Children[i].CollectStats(NumNodes, NumObjects);
		}
	}
}

/** 
* Find the given object in the octree node
* @param bRecurseChildren - recurse on this node's child nodes
* @return TRUE if object exists in the octree node 
*/
UBOOL FNavigationOctreeNode::FindObject( UObject* Owner, UBOOL bRecurseChildren )
{
	if( Owner )
	{
		for( INT ObjIdx=0; ObjIdx < Objects.Num(); ObjIdx++ )
		{
			FNavigationOctreeObject* CurObj = Objects(ObjIdx);
			if( CurObj->GetOwner<UObject>() == Owner )
			{
				return TRUE;
			}
		}
		if( Children != NULL &&
			bRecurseChildren )
		{
			for( INT i=0; i<8; i++ )
			{
				if( Children[i].FindObject(Owner,TRUE) )
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

/** console command handler for implementing debugging commands */
UBOOL FNavigationOctree::Exec(const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (ParseCommand(&Cmd, TEXT("NAVOCTREE")))
	{
		if (ParseCommand(&Cmd, TEXT("STATS")))
		{
			INT NumNodes = 0, NumObjects = 0;
			RootNode->CollectStats(NumNodes, NumObjects);
			Ar.Logf(TEXT("Number of objects: %i"), NumObjects);
			Ar.Logf(TEXT("Number of nodes: %i"), NumNodes);
			Ar.Logf(TEXT("Memory used by octree structures: %i bytes"), sizeof(FNavigationOctree) + NumNodes * sizeof(FNavigationOctreeNode) + NumObjects * sizeof(FNavigationOctreeObject*));
			Ar.Logf(TEXT("Memory used by objects in the octree: %i bytes"), NumObjects * sizeof(FNavigationOctreeObject));
		}
		else if( ParseCommand(&Cmd,TEXT("FIND")) )
		{
			UObject* Object;
			if(	ParseObject(Cmd,TEXT("NAME="), UObject::StaticClass(), Object, ANY_PACKAGE) )
			{
				debugf(TEXT("NAVOCTREE: FIND: %s = %s"), *Object->GetFullName(), RootNode->FindObject(Object,TRUE) ? TEXT("TRUE") : TEXT("FALSE") );
			}
			else
			{
				debugf(TEXT("NAVOCTREE: FIND: invalid object"));
			}
		}

		return 1;
	}	
	else
	{
		return 0;
	}
}

INT ARoute::ResolveRouteIndex( INT Idx, BYTE RouteDirection, BYTE& out_bComplete, BYTE& out_bReverse)
{
	if( RouteDirection == ERD_Forward )
	{
		// If reached the end of the route list
		if( Idx >= RouteList.Num() )
		{
			if( RouteType == ERT_Linear )
			{
				// Notify completed route
				out_bComplete = 1;
				Idx = -1;
			}
			else
			if( RouteType == ERT_Loop )
			{
				// Reverse direction
				out_bReverse = 1;
				Idx	= RouteList.Num() - 2;
			}
			else
			if( RouteType == ERT_Circle )
			{
				// Circle back to start of the list
				Idx = 0;
			}
		}
	}
	else /* if( RouteDirection == ERD_Reverse ) */
	{
		// If reached the start of the route list
		if( Idx < 0 )
		{
			if( RouteType == ERT_Linear )
			{
				// Notify completed route
				out_bComplete = 1;
				Idx = -1;
			}
			else
			if( RouteType == ERT_Loop )
			{
				// Reverse direction
				out_bReverse = 1;
				Idx = 1;
			}
			else
			if( RouteType == ERT_Circle )
			{
				// Circle back to end of the list
				Idx = RouteList.Num() - 1;
			}
		}
	}

	return Idx;
}

/**
 *	Find the closest navigation point in the route
 *	(that is also within tether distance)
 */
INT ARoute::MoveOntoRoutePath( APawn* P, BYTE RouteDirection, FLOAT DistFudgeFactor )
{
	FLOAT BestDistSq	= 0.f;
	FLOAT DistSq		= 0.f;
	FLOAT DistToClosest = 0.f;
	FLOAT DistToPawn	= 0.f;

	BYTE bReverse		= 0;
	BYTE bComplete		= 0;

	// Find closest route index
	INT BestIdx = -1;
	INT Idx = 0;
	for( Idx = 0; Idx < RouteList.Num(); Idx++ )
	{
		ANavigationPoint* Nav = RouteList(Idx).Nav();
		if( Nav != NULL )
		{
			DistSq = (P->Location-Nav->Location).SizeSquared();
			if( BestIdx < 0 ||
				DistSq < BestDistSq )
			{
				BestIdx		= Idx;
				BestDistSq	= DistSq;
			}
		}
	}
	Idx = BestIdx;

	//debug
	//`AILog( "- closest Route Idx"@Idx@((Idx>=0)?RouteList[Idx].Nav:None) );

	if( RouteDirection == ERD_Forward )
	{
		BestIdx = ResolveRouteIndex( Idx+1+RouteIndexOffset, RouteDirection, bComplete, bReverse );
	}
	else
	{
		BestIdx = ResolveRouteIndex( Idx-1+RouteIndexOffset, RouteDirection, bComplete, bReverse );
	}


	// If route is ending
	if( BestIdx < 0 )
	{
		// Move to last node in route
		if( RouteDirection == ERD_Forward )
		{
			BestIdx = RouteList.Num() - 1;
		}
		else
		{
			BestIdx = 0;
		}
	}
	else
	{
		//debug
		//`AILog( "- resolve index"@BestIdx@RouteList[BestIdx].Nav@Idx );

		if( Idx >= 0 && RouteList(BestIdx).Actor != NULL && RouteList(Idx).Actor != NULL)
		{
			
			DistToClosest = (RouteList(BestIdx)->Location-RouteList(Idx)->Location).Size();
			DistToPawn    = (RouteList(BestIdx)->Location-P->Location).Size() * DistFudgeFactor;

			// If Pawn is between our closest index and the next index
			if( DistToClosest <= DistToPawn )
			{
				// Move toward next one
				BestIdx = Idx;
			}
		}
	}

	//debug
	//`AILog( "- move onto target"@BestIdx@((BestIdx>=0)?RouteList[BestIdx].Nav:None) );

	return BestIdx;
}


//////////////////////////////////////////////////////////////////////////
// zhaojun@snda


#ifndef LOG_PROCESS_ERROR
#define LOG_PROCESS_ERROR(__condition__)	if (!(__condition__))	{goto Exit0;}					
#endif

bool FServerGridWorld::loadFile4Client(const TCHAR* pMapName)
{
	if (pMapName && ((*pMapName) != 0) && appStricmp(pMapName, m_szSceneName) != 0)
	{
		UnInit();

		Init(pMapName);

		FString phxFilePath, navFilePath, scnFilePath;
		FString ExportDir;

#if MMO_ENCRYPT_FILE_CONTENT || SHIPPING_PC_GAME
		GConfig->GetString(TEXT("Core.System"), TEXT("SeekFreePCPaths"), ExportDir, GEngineIni);
		ExportDir += TEXT("\\scene\\");

		phxFilePath = FString::Printf(TEXT("%s%s\\%s.%s"), 
			*ExportDir, m_szSceneName, m_szSceneName, TEXT("phx"));

		navFilePath = FString::Printf(TEXT("%s%s\\%s.%s"), 
			*ExportDir, m_szSceneName, m_szSceneName, TEXT("nav"));

		scnFilePath = FString::Printf(TEXT("%s%s\\%s.%s"), 
			*ExportDir, m_szSceneName, m_szSceneName, TEXT("scn"));

		//warnf(NAME_Error, TEXT("Loading nav file:%s\n%s\n%s"), *phxFilePath, *navFilePath, *scnFilePath);
#else
		GConfig->GetString(TEXT("Configuration"), TEXT("ServerSceneDataExport"), ExportDir, GEditorIni);

		phxFilePath = FString::Printf(TEXT("%s%s\\%s.%s"), 
			*ExportDir, m_szSceneName, m_szSceneName, TEXT("phx"));

		navFilePath = FString::Printf(TEXT("%s%s\\%s.%s"), 
			*ExportDir, m_szSceneName, m_szSceneName, TEXT("nav"));

		scnFilePath = FString::Printf(TEXT("%s%s\\%s.%s"), 
			*ExportDir, m_szSceneName, m_szSceneName, TEXT("scn"));
#endif

		m_bLoadResult = LoadPhxFromFile(*phxFilePath);
		m_bLoadResult = (m_bLoadResult && LoadNavFromFile(*navFilePath));
		m_bLoadResult = (m_bLoadResult && LoadScnFromFile(*scnFilePath));
	}

	// for test
	//ZONE_DATA* pZone;
	//GRID_DATA* pGrid;
	//GetZoneAndGrid(FVector(60354,44634,144), pZone, pGrid);

	//INT32 arrayPath[4096];
	//INT32 arraySize = sizeof(arrayPath) / sizeof(INT32);
	//GeneratePath(arraySize, arrayPath, FVector(60354,44634,144), FVector(60544,43638,144));

	return m_bLoadResult;// &&
}

bool FServerGridWorld::LoadNavFromFile( const TCHAR* pNavFilePath )
{
	if (pNavFilePath == NULL)
	{
		return false;
	}

	bool bResult = false;
	int nRetCode = 0;

	// check the file ext name == .phx;  let it go...let it go

	//fopen_s(&pFile, pNavFilePath, "rb");

	warnf(NAME_Warning, TEXT("Loading Nav file:%s"), pNavFilePath);
	
	FArchive* file = GFileManager->CreateFileReader(pNavFilePath);
	
	if (file)
	{
		//fseek(pFile, 0, SEEK_END);
		int nDataSize = file->TotalSize();// ftell(pFile);
		//fseek(pFile, 0, SEEK_SET);

		char* pData = new char[nDataSize];
		file->Serialize(pData, nDataSize);//fread(pData, 1, nDataSize, pFile);
		SCENE_NAV_HEADER* pHeader = (SCENE_NAV_HEADER*)pData;

		// Copy it instead of pointing to it directly; feels more comfortable
		m_pNavHeader = new SCENE_NAV_HEADER;
		memcpy(m_pNavHeader, pHeader, sizeof(SCENE_NAV_HEADER));

		m_NavVertexData = new NAV_VERTEX_DATA[pHeader->wVertexCount];
		memcpy(m_NavVertexData, pHeader->VertexData, sizeof(NAV_VERTEX_DATA) * m_pNavHeader->wVertexCount);

		m_NavEdgeData = new NAV_EDGE_DATA[pHeader->wEdgeCount];
		memcpy(m_NavEdgeData, pHeader->VertexData + pHeader->wVertexCount, sizeof(NAV_EDGE_DATA) * pHeader->wEdgeCount);

		m_NavFixedVertexData = new NAV_VERTEX_DATA[pHeader->wVertexCount];
		memcpy(m_NavFixedVertexData, (NAV_EDGE_DATA*)(pHeader->VertexData + pHeader->wVertexCount) + pHeader->wEdgeCount, sizeof(NAV_VERTEX_DATA) * pHeader->wVertexCount);
		// ignore the fixed nav-vertex data

		//////////////////////////////////////////////////////////////////////////

		//fclose(pFile);
		if(file)
		{
			file->Close();
			delete file;
		}

		delete [] (pData);

		return true;
	}
	else
	{
		warnf(NAME_Error, TEXT("Faild to load Nav file:%s"), pNavFilePath);
		return false;
	}
}

bool FServerGridWorld::LoadScnFromFile( const TCHAR* pNavFilePath )
{
	// now we don't need scn file
	return true;
}

void FServerGridWorld::ConvertPosClient2Server( const FVector& pos, INT64& llX, INT64& llY, INT64& llZ )
{
	FVector p = pos;
	ConvertCoordC2S(p);
	llX = p.X;
	llY = p.Y;
	llZ = p.Z;
}

void FServerGridWorld::ConvertPosServer2Client(INT64 llX, INT64 llY, INT64 llZ, FVector& pos)
{
	pos.X = (float)llX;
	pos.Y = (float)llY;
	pos.Z = (float)llZ;
	ConvertCoordS2C(pos);
}

void FServerGridWorld::GetGridCenter(FVector& center, const FVector& pos)
{
	INT64 llX, llY, llZ;

	FServerGridWorld::ConvertPosClient2Server(pos, llX, llY, llZ);

	//INT32 nZoneX = (INT32)(llX / ZONE_WIDTH);
	//INT32 nZoneY = (INT32)(llY / ZONE_WIDTH);
	INT64 nGridX = (INT32)((llX / GRID_WIDTH));
	INT64 nGridY = (INT32)((llY / GRID_WIDTH));

	llX = (nGridX + nGridX + 1) * GRID_WIDTH / 2;
	llY = (nGridY + nGridY + 1) * GRID_WIDTH / 2;

	FServerGridWorld::ConvertPosServer2Client(llX, llY, llZ, center);
}

void FServerGridWorld::GetGridCorner(FVector corner[4], const FVector& pos)	// if succeed, return none-null, corner then valid, z is always invalid
{
	INT64 llX, llY, llZ;

	FServerGridWorld::ConvertPosClient2Server(pos, llX, llY, llZ);

	//INT32 nZoneX = (INT32)(llX / ZONE_WIDTH);
	//INT32 nZoneY = (INT32)(llY / ZONE_WIDTH);
	INT64 nGridX = (INT32)((llX / GRID_WIDTH));
	INT64 nGridY = (INT32)((llY / GRID_WIDTH));

	llX = (nGridX) * GRID_WIDTH;
	llY = (nGridY) * GRID_WIDTH;

	INT64 llX2 = llX + GRID_WIDTH;
	INT64 llY2 = llY + GRID_WIDTH;

	FServerGridWorld::ConvertPosServer2Client(llX, llY, llZ, corner[0]);
	FServerGridWorld::ConvertPosServer2Client(llX, llY2, llZ, corner[1]);
	FServerGridWorld::ConvertPosServer2Client(llX2, llY2, llZ, corner[2]);
	FServerGridWorld::ConvertPosServer2Client(llX2, llY, llZ, corner[3]);
}

UBOOL FServerGridWorld::GetZoneAndGrid(INT64 llX,INT64 llY,INT64 llZ,ZONE_DATA*& rpZone, GRID_DATA*& rpGrid)
{
	if (llX < 0 || llY < 0)
	{
		rpZone = NULL;
		rpGrid = NULL;
		return FALSE;
	}

	INT32 nZoneX = (INT32)(llX / ZONE_WIDTH);
	INT32 nZoneY = (INT32)(llY / ZONE_WIDTH);
	INT32 nGridXInZone = (INT32)((llX / GRID_WIDTH) % GRID_PER_ZONE);
	INT32 nGridYInZone = (INT32)((llY / GRID_WIDTH) % GRID_PER_ZONE);
	UINT16 usLayer = (UINT16)(llZ / GRID_LAYER_HEIGHT);
	ZONE_DATA* pZoneData = NULL;
	UINT16 usGridIndex = NULL;
	UINT16 usNextIndex = NULL;

	LOG_PROCESS_ERROR(llX >= 0 && llY >= 0 && llZ >= 0);
	LOG_PROCESS_ERROR(nZoneX >= 0 && nZoneX < MAX_ZONE_PER_SCENE);
	LOG_PROCESS_ERROR(nZoneY >= 0 && nZoneY < MAX_ZONE_PER_SCENE);

	pZoneData = m_ZoneData[nZoneX][nZoneY];
	rpZone = pZoneData;
	
	if (!pZoneData)
	{
		return FALSE;
	}

	usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];
	
	if (!usGridIndex)
	{
		rpGrid = NULL;
		return FALSE;
	}

	usNextIndex = pZoneData->GridList[usGridIndex].uNextGrid;

	while (usNextIndex != NULL && usLayer > pZoneData->GridList[usNextIndex].uLowLayer)
	{
		usGridIndex = usNextIndex;
		usNextIndex = pZoneData->GridList[usNextIndex].uNextGrid;
	}

	rpGrid = pZoneData->GridList + usGridIndex;
	return TRUE;

Exit0:

	return FALSE;
}

UBOOL FServerGridWorld::GetZoneAndGridXY( const FVector& pos, INT32& X, INT32& Y )
{
	INT64 llX = 0;
	INT64 llY = 0;
	INT64 llZ = 0;

	ConvertPosClient2Server(pos, llX, llY, llZ);

	X = (INT32)(llX / GRID_WIDTH) ;
	Y = (INT32)(llY / GRID_WIDTH) ;

	return TRUE;
}

UBOOL FServerGridWorld::GetZoneAndGrid( const FVector& pos, ZONE_DATA*& rpZone, GRID_DATA*& rpGrid )
{
	INT64 llX = 0;
	INT64 llY = 0;
	INT64 llZ = 0;

	ConvertPosClient2Server(pos, llX, llY, llZ);

	return GetZoneAndGrid(llX, llY, llZ, rpZone, rpGrid);
}

//////////////////////////////////////////////////////////////////////////
struct BASE_LIST_NODE
{
	BASE_LIST_NODE*	pPrev;
	BASE_LIST_NODE* pNext;
};

inline void ClearNode(BASE_LIST_NODE* pNode)
{
	pNode->pPrev = NULL;
	pNode->pNext = NULL;
}

struct OPEN_LIST_DATA : BASE_LIST_NODE
{
	UINT16					wEdge;
	UINT16					wFromEdge;
	UINT16					wCost;
};

class CBaseList4Path
{
public:
	CBaseList4Path()
	{
		Clear();
	}
	~CBaseList4Path()
	{
		//LOG_CHECK_ERROR(m_Head.pPrev == NULL);
		//LOG_CHECK_ERROR(m_Head.pNext == &m_Rear);
		//LOG_CHECK_ERROR(m_Rear.pPrev == &m_Head);
		//LOG_CHECK_ERROR(m_Rear.pNext == NULL);
	}

	inline void Clear(void);
	inline BOOL IsEmpty(void);

	inline BASE_LIST_NODE& Head(void);
	inline BASE_LIST_NODE& Rear(void);

	static inline BOOL InsertAfter(BASE_LIST_NODE* pNewNode, BASE_LIST_NODE* pInsertAfter);
	static inline BOOL InsertBefore(BASE_LIST_NODE* pNewNode, BASE_LIST_NODE* pInsertAfter);

	static inline BOOL Remove(BASE_LIST_NODE* pNode);

	inline BOOL PushHead(BASE_LIST_NODE* pNode);
	inline BASE_LIST_NODE* PopHead(void);

	inline BOOL PushRear(BASE_LIST_NODE* pNode);
	inline BASE_LIST_NODE* PopRear(void);

private:
	BASE_LIST_NODE  m_Head;
	BASE_LIST_NODE  m_Rear;
};

#define node_cast(__obj_class_ptr__, __member__, __node_ptr__)  \
	((__obj_class_ptr__)((char*)(__node_ptr__) - (char*)&(((__obj_class_ptr__)0)->__member__)))

void CBaseList4Path::Clear()
{
	m_Head.pPrev = NULL;
	m_Head.pNext = &m_Rear;
	m_Rear.pPrev = &m_Head;
	m_Rear.pNext = NULL;
}

BOOL CBaseList4Path::IsEmpty()
{
	LOG_PROCESS_ERROR(m_Head.pPrev == NULL);
	LOG_PROCESS_ERROR(m_Head.pNext == &m_Rear);
	LOG_PROCESS_ERROR(m_Rear.pPrev == &m_Head);
	LOG_PROCESS_ERROR(m_Rear.pNext == NULL);

	return TRUE;
Exit0:
	return FALSE;
}

inline BASE_LIST_NODE& CBaseList4Path::Head(void)
{
	return m_Head;
}

inline BASE_LIST_NODE& CBaseList4Path::Rear(void)
{
	return m_Rear;
}

inline BOOL CBaseList4Path::InsertAfter(BASE_LIST_NODE* pNewNode, BASE_LIST_NODE* pTargetNode)
{
	LOG_PROCESS_ERROR(pNewNode && pTargetNode);
	LOG_PROCESS_ERROR(pNewNode->pPrev == NULL && pNewNode->pNext == NULL);
	LOG_PROCESS_ERROR(pTargetNode->pNext);

	pNewNode->pPrev = pTargetNode;

	//	if(pTargetNode->pNext)
	{
		pNewNode->pNext = pTargetNode->pNext;
		pTargetNode->pNext->pPrev = pNewNode;
	}

	pTargetNode->pNext = pNewNode;

	return TRUE;
Exit0:

	return FALSE;
}

inline BOOL CBaseList4Path::InsertBefore(BASE_LIST_NODE* pNewNode, BASE_LIST_NODE* pTargetNode)
{
	LOG_PROCESS_ERROR(pNewNode && pTargetNode);
	LOG_PROCESS_ERROR(pNewNode->pPrev == NULL && pNewNode->pNext == NULL);
	LOG_PROCESS_ERROR(pTargetNode->pPrev);

	pNewNode->pNext = pTargetNode;
	//	if(pTargetNode->pPrev)
	{
		pNewNode->pPrev = pTargetNode->pPrev;
		pTargetNode->pPrev->pNext = pNewNode;

	}
	pTargetNode->pPrev = pNewNode;

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL CBaseList4Path::Remove(BASE_LIST_NODE* pNode)
{
	LOG_PROCESS_ERROR(pNode);
	LOG_PROCESS_ERROR(pNode->pPrev && pNode->pNext);

	pNode->pPrev->pNext = pNode->pNext;
	pNode->pNext->pPrev = pNode->pPrev;

	pNode->pNext = pNode->pPrev = NULL;

	return TRUE;
Exit0:

	return FALSE;
}

inline BOOL CBaseList4Path::PushHead(BASE_LIST_NODE* pNode)
{
	LOG_PROCESS_ERROR(pNode->pNext == NULL && pNode->pPrev == NULL);

	pNode->pPrev = &m_Head;
	pNode->pNext = m_Head.pNext;

	pNode->pNext->pPrev = pNode;
	m_Head.pNext = pNode;

	return TRUE;
Exit0:
	return FALSE;
}

inline BASE_LIST_NODE* CBaseList4Path::PopHead(void)
{
	BASE_LIST_NODE* pResult = NULL;

	LOG_PROCESS_ERROR(m_Head.pNext != &m_Rear);

	pResult = m_Head.pNext;

	LOG_PROCESS_ERROR(pResult);

	m_Head.pNext = pResult->pNext;

	LOG_PROCESS_ERROR(pResult->pNext);

	pResult->pNext->pPrev = &m_Head;

	pResult->pPrev = pResult->pNext = NULL;

	return pResult;
Exit0:
	return NULL;
}

inline BOOL CBaseList4Path::PushRear(BASE_LIST_NODE* pNode)
{
	LOG_PROCESS_ERROR(pNode->pNext == NULL && pNode->pPrev == NULL);

	pNode->pPrev = m_Rear.pPrev;
	pNode->pNext = &m_Rear;

	m_Rear.pPrev = pNode;
	pNode->pPrev->pNext = pNode;

	return TRUE;
Exit0:
	return FALSE;
}

inline BASE_LIST_NODE* CBaseList4Path::PopRear(void)
{
	BASE_LIST_NODE* pResult = NULL;

	LOG_PROCESS_ERROR(m_Rear.pPrev != &m_Head);

	pResult = m_Rear.pPrev;
	m_Rear.pPrev = pResult->pPrev;
	pResult->pPrev->pNext = &m_Rear;

	pResult->pPrev = pResult->pNext = NULL;

	return pResult;
Exit0:
	return NULL;
}

// left-right construct a line, positive means point is at one side, negative means other side
// 0 means the 3 pt in the same line, otherwise not, zhaojun@snda, copy from server
INT64 _CollineationCheck(NAV_VERTEX_DATA* pPoint, NAV_VERTEX_DATA* pLeftPoint, NAV_VERTEX_DATA* pRightPoint)
{
	INT64 xp = pPoint->nX;
	INT64 yp = pPoint->nY;
	INT64 xl = pLeftPoint->nX;
	INT64 yl = pLeftPoint->nY;
	INT64 xr = pRightPoint->nX;
	INT64 yr = pRightPoint->nY;

	return ((yr - yl) * xp + (xl - xr) * yp + (xr*yl - xl*yr));
}

INT16 _PathFindingDistance(INT64 sx, INT64 sy, INT64 sz, INT64 dx, INT64 dy, INT64 dz)
{
	INT64 d = (dx - sx) * (dx - sx) + (dy - sy) * (dy - sy) + (dz - sz) * (dz - sz);

	d = d / GRID_WIDTH / GRID_WIDTH / 4;

	return appSqrt(d);
}
//////////////////////////////////////////////////////////////////////////

#define MAX_OPEN_LIST_DATA_COUNT	(65536)
#define MAX_SEARCH_EDGE_COUNT (128)		// modified by zhangrui05 for fixing #10687

// search
#if 1
	OPEN_LIST_DATA s_OpenListDataPool[MAX_OPEN_LIST_DATA_COUNT];
	UINT8 s_byVisitTable[MAX_OPEN_LIST_DATA_COUNT/8];
	UINT16 s_wVertexList[MAX_SEARCH_EDGE_COUNT];
	INT32 g_resultVertexCount = 0;
#else
	OPEN_LIST_DATA* s_OpenListDataPool = new OPEN_LIST_DATA[MAX_OPEN_LIST_DATA_COUNT];
	UINT8* s_byVisitTable = new UINT8[MAX_OPEN_LIST_DATA_COUNT/8];
	UINT16* s_wVertexList = new UINT16[MAX_SEARCH_EDGE_COUNT];
#endif

class GlobalVarInitHelper
{
public:
	GlobalVarInitHelper()
	{
		for (INT32 i = 0; i < MAX_OPEN_LIST_DATA_COUNT; i++)
		{
			s_OpenListDataPool[i].wEdge = i;
		}
	}

	~GlobalVarInitHelper()
	{

	}
};

static GlobalVarInitHelper g_globalVarInitHelper;

#if !SHIPPING_PC_GAME
# define LogPathFindFail debugf
#else
# define LogPathFindFail(...)
#endif

UBOOL FServerGridWorld::FixPath(const FVector& wayPoint, INT wayPointIndex)
{
	if (wayPointIndex < m_arraySize)
	{
		INT64 x,y,z;
		ConvertPosClient2Server(wayPoint,x,y,z);

		m_arrayPath[3 * wayPointIndex] = (INT32)x;
		m_arrayPath[3 * wayPointIndex + 1] = (INT32)y;
		m_arrayPath[3 * wayPointIndex + 2] = (INT32)z;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

FVector FServerGridWorld::GetSafeDest(const FVector& DesPos,FLOAT SafeRadius)
{
	ZONE_DATA* pSrcZone = NULL;
	GRID_DATA* pSrcGrid = NULL;	
	GetZoneAndGrid(DesPos, pSrcZone, pSrcGrid);
	if(pSrcGrid==NULL || pSrcGrid->bOutOfPoly )//|| pSrcGrid->bBorder)
	{
		return DesPos;
	}

	FVector DeltaStep[8] = {FVector(1.0f,0.0,0.0),FVector(0.0f,1.0,0.0),FVector(-1.0f,0.0,0.0),FVector(0.0f,-1.0f,0.0),
		FVector(0.7071f,0.7071f,0.0),FVector(-0.7071f,0.7071,0.0),FVector(0.7071f,-0.7071f,0.0),FVector(-0.7071f,-0.7071f,0.0)};

	FVector DeltaStepOut = FVector(0.f,0.f,0.f);
	for(INT i=0;i<8;i++){
		GetZoneAndGrid(DesPos+DeltaStep[i]*SafeRadius, pSrcZone, pSrcGrid);

		if(pSrcGrid==NULL || pSrcGrid->bOutOfPoly || pSrcGrid->bBorder)
		{
			DeltaStepOut += -DeltaStep[i];
		}
	}

	if(DeltaStepOut.X<-0.1)DeltaStepOut.X=-1.0f;
	else if(DeltaStepOut.X> 0.1)DeltaStepOut.X= 1.0f;

	if(DeltaStepOut.Y<-0.1)DeltaStepOut.Y=-1.0f;
	else if(DeltaStepOut.Y> 0.1)DeltaStepOut.Y= 1.0f;

	return DesPos+DeltaStepOut*SafeRadius;
}


//Add by JimZhang@snda
FVector FServerGridWorld::OptimizeDes_Line( const FVector Step1, const FVector Step2, UBOOL& bRes, UBOOL& bRun )
{
	ZONE_DATA* pBufZone = NULL;
	GRID_DATA* pBufGrid = NULL;
	bRes= GetZoneAndGrid(Step1, pBufZone, pBufGrid);

	////Add log to show the blocking situation------------------------------------------------------------------------------------------------------
	//if(bRun && bRes && pBufGrid && !pBufGrid->bOutOfPoly && IsBlockedActorExist(pBufGrid) && IsInSafeZone(pBufGrid)==FALSE )
	//{

	//	g_ppActorList = gServerGridWorld.GetActorList(pBufGrid,g_nActorNum);
	//	if(g_ppActorList)
	//	{
	//	}
	//	else
	//	{
	//		warnf( NAME_Log, TEXT("*****************************Dynamic blocking, has blocking flag, but there is no actor left in the grid cell!!!!!!") );
	//	}
	//}
	////--------------------------------------------------------------------------------------------------------------------------



	if(bRes && pBufGrid && !pBufGrid->bOutOfPoly && (IsBlockedActorExist(pBufGrid)==FALSE||IsInSafeZone(pBufGrid)))
	{
		if(bRun){
			bRes= GetZoneAndGrid(Step2, pBufZone, pBufGrid);
			if(bRes && pBufGrid && !pBufGrid->bOutOfPoly && (IsBlockedActorExist(pBufGrid)==FALSE||IsInSafeZone(pBufGrid)) )
			{
				bRes = TRUE;
				return Step2;
			}
			bRes = FALSE;
			return Step2;
		}
		else
		{
			bRes = TRUE;
			return Step1;
		}
	}
	bRes = FALSE;
	return Step1;
}

FVector FServerGridWorld::GetHitPoint( const FVector& StartPoint,  const FVector& EndPoint)
{
	ZONE_DATA* pBufZone = NULL;
	GRID_DATA* pBufGrid = NULL;

	FLOAT dis  = (EndPoint-StartPoint).Size2D();
	FLOAT dis2 = (EndPoint-StartPoint).SizeSquared2D();
	FVector Step = EndPoint-StartPoint;
	Step.Normalize();
	Step = Step*128.0f;

	FVector CurPoint = StartPoint+Step;
	UBOOL bProgress = (dis2>(CurPoint-StartPoint).SizeSquared2D());
	while(bProgress)
	{
		UBOOL bRes= GetZoneAndGrid(CurPoint, pBufZone, pBufGrid);
		if(bRes && pBufGrid)
		{
			if(pBufGrid->bBlockBullet)
			{
				return CurPoint;
			}
			else
			{
				if(pBufGrid->bBorder==FALSE)//Not an edge, judge one extra grid cell
				{
					CurPoint += Step;
				}
			}
		}

		CurPoint += Step;
		bProgress = (dis2>(CurPoint-StartPoint).SizeSquared2D());
	}

	return EndPoint;
}


INT FServerGridWorld::GetBlockType( const FVector Step1 )
{
	ZONE_DATA* pBufZone = NULL;
	GRID_DATA* pBufGrid = NULL;
	UBOOL bRes= GetZoneAndGrid(Step1, pBufZone, pBufGrid);
	if(bRes && pBufGrid && pBufGrid->bOutOfPoly)
	{
		return 2;
	}
	else if(bRes && pBufGrid && pBufGrid->bOutOfPoly==FALSE)
	{
		if(IsInSafeZone(pBufGrid)==FALSE && IsBlockedActorExist(pBufGrid)==TRUE)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 3;
	}
}

FVector FServerGridWorld::GetNearbyGridPos(const FVector& SelfPos, INT Index, FLOAT fStep)
{
	static FVector DirArray[8] = {	FVector(1.0,0.0,0.0),FVector(1.0,1.0,0.0),FVector(0.0,1.0,0.0),FVector(-1.0,1.0,0.0),
		FVector(-1.0,0.0,0.0),FVector(-1.0,-1.0,0.0),FVector(0.0,-1.0,0.0),FVector(1.0,-1.0,0.0) };

	FVector step1 = SelfPos;
	ConvertCoordC2S(step1);
	step1 += DirArray[Index]*128.0*fStep*POINT_PER_CM;
	ConvertCoordS2C(step1);
	return step1;
}

INT FServerGridWorld::OptimizeDir_Mir2( const FVector& inFoot, const FVector& inDest)
{
	//New feel/////////////////////////////////////////////////////////////
	FVector V1 = inFoot;
	FVector V2 = inDest;
	ConvertCoordC2S(V1);
	ConvertCoordC2S(V2);
	FVector NewDirVec =  (V2-V1)/POINT_PER_CM;
	INT IndexNew=-1;
	INT X1=0;
	INT Y1=0;

	INT OffsetIndex = 0;
	if(NewDirVec.X<=64.0f && NewDirVec.X>=-64.0f && NewDirVec.Y<=64.0f && NewDirVec.Y>=-64.0f)
	{
		OffsetIndex = -8;
		NewDirVec.SafeNormal();
		NewDirVec = NewDirVec*128.0;
	}

	if(NewDirVec.X<=64.0f && NewDirVec.X>=-64.0f)X1=0;
	else if(NewDirVec.X<196.0f && NewDirVec.X>-196.0f && (NewDirVec.Y>320.0f||NewDirVec.Y<-320.0f))X1=0;
	else if(NewDirVec.X>0.0f) X1=1;
	else X1=-1;

	if(NewDirVec.Y<=64.0f && NewDirVec.Y>=-64.0f)Y1=0;
	else if(NewDirVec.Y<=64.0f && NewDirVec.Y>-196.0f && (NewDirVec.X>320.0f||NewDirVec.X<-320.0f))Y1=0;
	else if(NewDirVec.Y>0.0f) Y1=1;
	else Y1=-1;

	if(X1==1 &&Y1==0 )IndexNew=0;
	else if(X1==1 &&Y1==1 )IndexNew=1;
	else if(X1==0 &&Y1==1 )IndexNew=2;
	else if(X1==-1&&Y1==1 )IndexNew=3;
	else if(X1==-1&&Y1==0 )IndexNew=4;
	else if(X1==-1&&Y1==-1)IndexNew=5;
	else if(X1==0 &&Y1==-1)IndexNew=6;
	else if(X1==1 &&Y1==-1)IndexNew=7;

	return IndexNew + OffsetIndex;
}

INT FServerGridWorld::GetYawFromDirIndex( INT DirIndex )
{
	if(DirIndex<0)return DirIndex;
	return DirIndex*1024*8;
}

FVector FServerGridWorld::OptimizeDes_Mir2( const FVector& inFoot, const FVector& inDest, UBOOL& bRes, UBOOL& bRun, UBOOL& bOblique,UBOOL bCrossJudge,FLOAT minRadius,UBOOL bEx )
{
	INT i=0;
	ZONE_DATA* pBufZone = NULL;
	GRID_DATA* pBufGrid = NULL;

	FVector VecDest = inDest;

	INT nBlockType = GetBlockType(inDest);
	if(nBlockType>=2)
	{
		bEx = FALSE;
	}

//New feel/////////////////////////////////////////////////////////////
	INT Index = OptimizeDir_Mir2(inFoot, inDest);
	if(Index<0)
	{
		bRes = FALSE;
		return inFoot;
	}

	FVector step1 = GetNearbyGridPos(inFoot,  Index, 1.0f);
	FVector step2 = GetNearbyGridPos(inFoot,  Index, 2.0f);
	FLOAT delta1 = (step1-VecDest).Size2D();
	FLOAT delta2 = (step2-VecDest).Size2D();

	if(bRun&&delta1<=minRadius)//Already within range on the first step of running, switch away directly and advance one step
	{
		//Do not process, jump to the next step and walk
	}
	else
	{
		UBOOL bCross = delta1 > delta2+16.0;
		if(bCrossJudge==FALSE)
		{
			bCross = TRUE;
		}

		FVector ResVector = OptimizeDes_Line( step1, step2, bRes,bRun );
		if(bRes&&(bCross||bRun==FALSE))
		{
			if(bRun&&bCross)//Normal running handling, ensure not to overshoot, then turn
			{
				bOblique=(Index%2==1);
				return ResVector;
			}
			if(bRun==FALSE)//Normal walking handling
			{
				bOblique=(Index%2==1);
				return ResVector;
			}
		}
	}

	if(bRun)//When running is not satisfied, take one step of walking
	{
		UBOOL bRunOrWalk = FALSE;
		//FVector ResVector2 = OptimizeDes_Line( inFoot+DirArray[Index]*128.0, inFoot+DirArray[Index]*128.0*2.0, bRes,bRunOrWalk );
		FVector ResVector2 = OptimizeDes_Line( step1, step2, bRes,bRunOrWalk );
		if(bRes)
		{
			bOblique=(Index%2==1);
			bRun = bRunOrWalk;
			return ResVector2;
		}
	}

	if(bRes==FALSE)//When neither running nor walking works, determine the blocking type; dynamic blocking does not disturb the path
	{
		UBOOL bRes1= GetZoneAndGrid(step1, pBufZone, pBufGrid);
		UBOOL bDynamicBLock1 = (bRes1 && pBufGrid && !pBufGrid->bOutOfPoly && IsBlockedActorExist(pBufGrid));
		if(bDynamicBLock1)
		{
			g_BlockPosition = step1;
			g_pBufGrid = pBufGrid;
		}
		else
		{
			g_pBufGrid = NULL;
		}

		UBOOL bRes2= GetZoneAndGrid(step2, pBufZone, pBufGrid);
		UBOOL bDynamicBLock2 = (bRes2 && pBufGrid && !pBufGrid->bOutOfPoly && IsBlockedActorExist(pBufGrid));

		if(m_DynamicBLock)
		{
			if(bDynamicBLock1||bDynamicBLock2)
			{
				bRes = FALSE;
				return inFoot;
			}
		}
	}

//If there is static blocking, perform detour judgment-----------------------------------------------------------------------
	bRun = FALSE; //Detours are always walked*****************
	INT Index2 = Index+1;
	if(Index2>=8)Index2=0;
	UBOOL bRes2;
	step1 = GetNearbyGridPos(inFoot,  Index2, 1.0f);
	step2 = GetNearbyGridPos(inFoot,  Index2, 2.0f);
	FVector ResVector2 = OptimizeDes_Line( step1, step2, bRes2,bRun );

	INT Index3 = Index-1;
	if(Index3<0)Index3=7;
	UBOOL bRes3;
	step1 = GetNearbyGridPos(inFoot,  Index3, 1.0f);
	step2 = GetNearbyGridPos(inFoot,  Index3, 2.0f);
	FVector ResVector3 = OptimizeDes_Line( step1, step2, bRes3,bRun );

	if(bRes2&&bRes3)
	{
		FLOAT Dis2 = (VecDest-ResVector2).SizeSquared2D();
		FLOAT Dis3 = (VecDest-ResVector3).SizeSquared2D();
		if(Dis2<Dis3)
		{
			bOblique=(Index2%2==1);
			bRes = bRes2;
			return ResVector2;
		}
		else
		{
			bOblique=(Index3%2==1);
			bRes = bRes3;
			return ResVector3;
		}
	}
	else if(bRes2)
	{
		bOblique=(Index2%2==1);
		bRes = bRes2;
		return ResVector2;
	}
	else if(bRes3)
	{
		bOblique=(Index3%2==1);
		bRes = bRes3;
		return ResVector3;
	}
	


//If there is no way to go, perform the advanced pillar-circumvention judgment; this part is an advanced extra that Legend doesn't have, added so Legend Eternal feels better and doesn't get stuck on pillars-----------------------------------------------------------------------
	if(bEx)
	{
		bRun = FALSE; //Detours are always walked*****************
		INT Index4 = Index+2;
		if(Index4>=8)Index4=Index4-8;
		FVector step4 = GetNearbyGridPos(inFoot,  Index4, 1.0f);

		UBOOL bRes4 = GetZoneAndGrid(step4, pBufZone, pBufGrid);
		UBOOL bBLock4 = (bRes4==FALSE || pBufGrid==FALSE || pBufGrid->bOutOfPoly==TRUE || IsBlockedActorExist(pBufGrid));

		if(bBLock4==FALSE)
		{
			UBOOL bOblique4=FALSE;
			OptimizeDes_Mir2(step4, inDest, bRes4, bRun, bOblique4,bCrossJudge,minRadius);
			if(bRes4)
			{
				bOblique=(Index4%2==1);
				bRes = bRes4;
				return step4;
			}
		}

		INT Index5 = Index-2;
		if(Index5<0)Index5=Index5+8;
		FVector step5 = GetNearbyGridPos(inFoot,  Index5, 1.0f);

		UBOOL bRes5 = GetZoneAndGrid(step5, pBufZone, pBufGrid);
		UBOOL bBLock5 = (bRes5==FALSE || pBufGrid==FALSE || pBufGrid->bOutOfPoly==TRUE || IsBlockedActorExist(pBufGrid));

		if(bBLock5==FALSE)
		{
			UBOOL bOblique5=FALSE;
			OptimizeDes_Mir2(step5, inDest, bRes5, bRun, bOblique5,bCrossJudge,minRadius);
			if(bRes5)
			{
				bOblique=(Index5%2==1);
				bRes = bRes5;
				return step5;
			}
		}
	}
//---------------------------------------------------------------------------------------------
	bRes = FALSE;
	return inFoot;

}

//Add by JimZhang@snda Take the nearest reachable pathfinding point to the target; when the straight-line distance toward the target is less than 64*3, walk to the grid cell nearest to the target
FVector FServerGridWorld::OptimizeDes( const FVector& inFoot, const FVector& inDest, UBOOL& bOptimized )
{
	INT i=0;
	ZONE_DATA* pBufZone = NULL;
	GRID_DATA* pBufGrid = NULL;

	//Target point is directly reachable
	UBOOL bRes= GetZoneAndGrid(inDest, pBufZone, pBufGrid);
	if(pBufGrid && !pBufGrid->bOutOfPoly && !pBufGrid->bBorder)
	{
		bOptimized = FALSE;
		return inDest;
	}

	//Determine how far one can walk directly toward the target point
	FVector Out	= inFoot;
	FVector direct = inDest-inFoot;
	FLOAT dis = direct.Size();
	INT num = INT(dis/64.0f);
	direct.Normalize();


	//for(i=1;i<=num;i++)
	//{
	//	UBOOL bR3 = GetZoneAndGrid(inFoot+direct*64.0f*FLOAT(i), pBufZone, pBufGrid);
	//	if(bR3==FALSE)break;
	//	else if(pBufGrid==NULL)break;
	//	else if(pBufGrid->bOutOfPoly||pBufGrid->bBorder)break;
	//	Out = inFoot+direct*32.0f*FLOAT(i);
	//}
	//if(i>=3)
	//{
	//	return Out;
	//}

	//Determine how close to the target point one can get
	FVector DeltaStep[8] = {FVector(1.0f,0.0,0.0),FVector(0.0f,1.0,0.0),FVector(-1.0f,0.0,0.0),FVector(0.0f,-1.0f,0.0),
		FVector(0.7071f,0.7071f,0.0),FVector(-0.7071f,0.7071,0.0),FVector(0.7071f,-0.7071f,0.0),FVector(-0.7071f,-0.7071f,0.0)};

	FLOAT DeltaStepWeight[8];//Directional weights
	for(INT k=0;k<8;k++)
	{
		DeltaStepWeight[k] = (DeltaStep[k]-direct*3.0f).Size2D();
	}

	for(i=1;i<=16;i++)
	{
		for(INT j=0;j<8;j++)
		{
			FVector TestPos = inDest + DeltaStep[j]*DeltaStepWeight[j]*32.0f*FLOAT(i);
			UBOOL bRes = GetZoneAndGrid(TestPos, pBufZone, pBufGrid);
			if(bRes && pBufGrid && !pBufGrid->bOutOfPoly && !pBufGrid->bBorder)
			{
				bOptimized = TRUE;
				return TestPos;
			}
		}
	}

	bOptimized = TRUE;
	return inFoot;
}

void FServerGridWorld::GeneratePathAnz()
{
	FVector BuffPos;
	for(INT i=1;i<m_arraySize;i++){
		FetchPath(BuffPos, i);
		BuffPos = GetSafeDest(BuffPos,64.0f+32.0f);
		FixPath(BuffPos, i);
	}

	FVector StartPos;
	FVector CurrentPos;
	INT AnzIndex=1;
	FetchPath(StartPos, 0);
	for(INT i=2;i<m_arraySize;i++)
	{
		if(FetchPath(CurrentPos, i))
		{
			if(IsStraight( StartPos, CurrentPos, TRUE) )
			{
				AnzIndex = i;
			}
			else{
				break;
			}
		}
		else
		{
			break;
		}
	}

	if(AnzIndex>1)
	{
		for(INT i=AnzIndex,j=1;i<m_arraySize;i++,j++)
		{
			m_arrayPath[j*3+0] = m_arrayPath[i*3+0];
			m_arrayPath[j*3+1] = m_arrayPath[i*3+1];
			m_arrayPath[j*3+2] = m_arrayPath[i*3+2];
		}
		m_arraySize = 	m_arraySize+1-AnzIndex;
	}
}

UINT FServerGridWorld::IsStraight( const FVector& inSour, const FVector& inDest, UBOOL IgnoreBorder )
{
	ZONE_DATA* pSrcZone = NULL;
	GRID_DATA* pSrcGrid = NULL;

	ZONE_DATA* pDestZone = NULL;
	GRID_DATA* pDestGrid = NULL;

	UBOOL bR1= GetZoneAndGrid(inSour, pSrcZone, pSrcGrid);
	UBOOL bR2= GetZoneAndGrid(inDest, pDestZone, pDestGrid);

	if(bR1 ==FALSE || bR2==FALSE)
	{
		return FALSE;
	}

	if(pSrcGrid ==NULL || pDestGrid==NULL)
	{
		return FALSE;
	}

	if(pSrcGrid->bOutOfPoly )//|| pSrcGrid->bBorder)
	{
		return FALSE;
	}

	if(pDestGrid->bOutOfPoly )//|| pDestGrid->bBorder)
	{
		return FALSE;
	}

	if(IgnoreBorder==FALSE)
	{
		if( pSrcGrid->bBorder)
		{
			return FALSE;
		}

		if(pDestGrid->bBorder)
		{
			return FALSE;
		}
	}

	FVector direct = inDest-inSour;
	FLOAT dis = direct.Size();
	INT num = INT(dis/32.0f);
	direct.Normalize();

	for(INT i=1;i<=num;i++)
	{
		UBOOL bR3 = GetZoneAndGrid(inSour+direct*32.0f*FLOAT(i), pDestZone, pDestGrid);
		if(bR3==FALSE)return FALSE;
		else if(pDestGrid==NULL)return FALSE;
		else if(pDestGrid->bOutOfPoly)return FALSE;
		else if(IgnoreBorder==FALSE&&pDestGrid->bBorder)return FALSE;
	}

	return TRUE;
}

UINT FServerGridWorld::GeneratePathLine( const FVector& inSour, const FVector& inDest )
{
	m_arraySize = sizeof(m_arrayPath) / sizeof(m_arrayPath[0]) / 3;
	INT32& rnResultSize = m_arraySize;
	INT32* pnResult = m_arrayPath;
	m_arrayPointer = 1;

	UBOOL bRes =  IsStraight( inSour, inDest, FALSE );
	if(bRes){

		INT64 llDestX;
		INT64 llDestY;
		INT64 llDestZ;

		INT32 nResultSize = 0;
		ConvertPosClient2Server(inSour, llDestX, llDestY, llDestZ);
		pnResult[0] = (INT32)llDestX;
		pnResult[1] = (INT32)llDestY;
		pnResult[2] = (INT32)llDestZ;
		nResultSize++;
		ConvertPosClient2Server(inDest, llDestX, llDestY, llDestZ);
		pnResult[3] = (INT32)llDestX;
		pnResult[4] = (INT32)llDestY;
		pnResult[5] = (INT32)llDestZ;
		nResultSize++;
		rnResultSize = nResultSize;
	}

	return bRes;
}

UINT FServerGridWorld::GeneratePath( const FVector& inSour, const FVector& inDest )
{
	//DWORD moveCycles = appCycles();


	//// oscar@snda deal with the case where grid cells generated on slopes such as stairs are too thin
	FVector dest = inDest;
	//dest.Z += g_pickZOffset;

	FVector sour = inSour;
	//sour.Z += g_pickZOffset;
	//// oscar@snda

	m_arraySize = sizeof(m_arrayPath) / sizeof(m_arrayPath[0]) / 3;
	INT32& rnResultSize = m_arraySize;
	INT32* pnResult = m_arrayPath;
	m_arrayPointer = 1;

	INT64 llSrcX;
	INT64 llSrcY;
	INT64 llSrcZ;
	ZONE_DATA* pSrcZone = NULL;
	GRID_DATA* pSrcGrid = NULL;

	INT64 llDestX;
	INT64 llDestY;
	INT64 llDestZ;
	ZONE_DATA* pDestZone = NULL;
	GRID_DATA* pDestGrid = NULL;

	ConvertPosClient2Server(sour, llSrcX, llSrcY, llSrcZ);
	GetZoneAndGrid(llSrcX, llSrcY, llSrcZ, pSrcZone, pSrcGrid);

	ConvertPosClient2Server(dest, llDestX, llDestY, llDestZ);
	GetZoneAndGrid(llDestX, llDestY, llDestZ, pDestZone, pDestGrid);

	INT32 nSrcX = (INT32)llSrcX;
	INT32 nSrcY = (INT32)llSrcY;
	INT32 nSrcZ = (INT32)llSrcZ;

	INT32 nDestX = (INT32)llDestX;
	INT32 nDestY = (INT32)llDestY;
	INT32 nDestZ = (INT32)llDestZ;

	OPEN_LIST_DATA* pOpenData = NULL;
	OPEN_LIST_DATA* pInsertPos = NULL;

	CBaseList4Path OpenList;
	UINT16 wCost;
	
	INT32 nResultSize;
	INT32 PushCount = 0;
	INT32 PopCount = 0;

	//PROCESS_ERROR(rnResultSize >= 2);
	//PROCESS_ERROR(wSrcEdge < m_nNavEdgeCount);
	//PROCESS_ERROR(wDestEdge < m_nNavEdgeCount);

	nResultSize = 0;
	pnResult[0] = nSrcX;
	pnResult[1] = nSrcY;
	pnResult[2] = nSrcZ;
	nResultSize++;
	
	if(!pSrcZone)
	{
		warnf(NAME_Error, TEXT("+++ Pathfinding: start point %s has no Zone grid data!"), *inSour.ToString());
		goto Exit0;
	}

	if(!pDestZone)
	{
		warnf(NAME_Warning, TEXT("+++ Pathfinding: target point %s has no Zone grid data. It may be empty, a map border, or out of sync with the server."), *inDest.ToString());
		goto Exit0;
	}

	if(!pSrcGrid)
	{
		warnf(NAME_Error, TEXT("+++ Pathfinding: start point %s has no grid data!"), *inSour.ToString());
		goto Exit0;
	}

	if(!pDestGrid)
	{
		warnf(NAME_Warning, TEXT("+++ Pathfinding: target point %s has no grid data. It may be empty, a map border, or out of sync with the server."), *inDest.ToString());
		goto Exit0;
	}

	UINT16 wSrcEdge = pSrcZone->wEdge[pSrcGrid->uEdge];
	UINT16 wDestEdge = pDestZone->wEdge[pDestGrid->uEdge]; 


	if(pSrcGrid->bOutOfPoly)
	{
		warnf(NAME_Error, TEXT("+++ Pathfinding: start point %s is outside the Poly!"), *inSour.ToString());
	}

	if(pDestGrid->bOutOfPoly)
	{
		warnf(NAME_Error, TEXT("@@@ Pathfinding: target point is outside the Poly!"));
	}


	if(pSrcGrid->bOutOfPoly || pDestGrid->bOutOfPoly)
	{
		goto Exit0;
	}

	if (!pSrcZone || !pSrcGrid || !pDestZone || !pDestGrid)
	{
		rnResultSize = 1;
		LogPathFindFail(TEXT("FServerGridWorld::GeneratePath faild @ 1"));
		goto Exit0;
	}


	if (m_pNavHeader == NULL || m_NavVertexData == NULL || m_NavEdgeData == NULL || nResultSize >= rnResultSize
		|| wSrcEdge == 0xFFFF || wSrcEdge == 0xFFFE || wDestEdge == 0xFFFF || wDestEdge == 0xFFFE)
	{
		rnResultSize = nResultSize;
		LogPathFindFail(TEXT("FServerGridWorld::GeneratePath faild @ 2"));
		goto Exit0;
	}

	if (pDestGrid->bBorder)
	{
		goto Exit0;
	}

	// check in same poly
	if (wSrcEdge == wDestEdge)
	{
		if (pDestGrid->bOutOfPoly/* ||
								 pSrcGrid->uLowLayer > pDestGrid->uHighLayer ||
								 pSrcGrid->uHighLayer < pDestGrid->uLowLayer*/)
		{
			rnResultSize = nResultSize;
			LogPathFindFail(TEXT("FServerGridWorld::GeneratePath faild @ 3"));
			goto Exit0;
		}
		else
		{
			pnResult[nResultSize * 3 + 0] = nDestX;
			pnResult[nResultSize * 3 + 1] = nDestY;
			pnResult[nResultSize * 3 + 2] = nDestZ;
			nResultSize++;
			rnResultSize = nResultSize;

			return TRUE;
		}
	}

	INT nNavEdgeCount = m_pNavHeader->wEdgeCount;
	const INT nNavVertexCount = m_pNavHeader->wVertexCount;

	memset(s_byVisitTable, 0, nNavEdgeCount / 8 + 1);
	OpenList.Clear();

	// prepare src edge
	pOpenData = s_OpenListDataPool + wDestEdge;

	s_byVisitTable[wDestEdge / 8] |= 1 << (wDestEdge % 8);
	pOpenData->pPrev = pOpenData->pNext = NULL;
	pOpenData->wFromEdge = 0xFFFF;

	pOpenData->wCost = _PathFindingDistance(nDestX, nDestY, nDestZ, m_NavVertexData[pOpenData->wEdge].nX, m_NavVertexData[pOpenData->wEdge].nY, m_NavVertexData[pOpenData->wEdge].nZ);

	OpenList.PushHead(pOpenData);

	for (INT32 nIndex = 0; nIndex < MAX_EDGE_PER_POLY - 1 && m_NavEdgeData[wDestEdge].wLinkEdge[nIndex] != 0xFFFF; nIndex++)
	{
		pOpenData = s_OpenListDataPool + m_NavEdgeData[wDestEdge].wLinkEdge[nIndex];
		s_byVisitTable[pOpenData->wEdge / 8] |= 1 << (pOpenData->wEdge % 8);

		pOpenData->pPrev = pOpenData->pNext = NULL;
		pOpenData->wFromEdge = 0xFFFF;

		pOpenData->wCost =  _PathFindingDistance(nDestX, nDestY, nDestZ, m_NavVertexData[pOpenData->wEdge].nX, m_NavVertexData[pOpenData->wEdge].nY, m_NavVertexData[pOpenData->wEdge].nZ);

		pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
		
		LOG_PROCESS_ERROR(pInsertPos);

		UINT32 nLoop = 0;

		while (pInsertPos->pNext && pInsertPos != &OpenList.Rear() && pInsertPos->wCost < pOpenData->wCost)
		{
			++nLoop;

			LOG_PROCESS_ERROR(nLoop < 1000);

			pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;
		}

		OpenList.InsertBefore(pOpenData, pInsertPos);
	}

	while (!OpenList.IsEmpty())
	{
		pOpenData = (OPEN_LIST_DATA*)(OpenList.PopHead());
		LOG_PROCESS_ERROR(pOpenData);

		PopCount++;
		UINT16 wPairEdge = m_NavEdgeData[pOpenData->wEdge].wPairEdge;

		if (m_NavEdgeData[pOpenData->wEdge].wStartEdge == wSrcEdge)
			break;

		if (wPairEdge == 0xFFFF)
			continue;

		if (m_NavEdgeData[wPairEdge].wStartEdge == wSrcEdge)
		{
			wCost = pOpenData->wCost + _PathFindingDistance(nSrcX, nSrcY, nSrcZ, m_NavVertexData[wPairEdge].nX, m_NavVertexData[wPairEdge].nY, m_NavVertexData[wPairEdge].nZ);

			if ((s_byVisitTable[wPairEdge / 8] & (1 << (wPairEdge % 8))) == 0)
			{
				s_byVisitTable[wPairEdge / 8] |= 1 << (wPairEdge % 8);

				s_OpenListDataPool[wPairEdge].pPrev = s_OpenListDataPool[wPairEdge].pNext = NULL;
				s_OpenListDataPool[wPairEdge].wCost = wCost;
				s_OpenListDataPool[wPairEdge].wFromEdge = pOpenData->wFromEdge;

				pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
				while (pInsertPos != &OpenList.Rear() && pInsertPos && pInsertPos->wCost < s_OpenListDataPool[wPairEdge].wCost)
					pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

				OpenList.InsertBefore(s_OpenListDataPool + wPairEdge, pInsertPos);
				PushCount++;
			}
			else if (wCost < s_OpenListDataPool[wPairEdge].wCost)
			{
				s_OpenListDataPool[wPairEdge].wFromEdge = pOpenData->wFromEdge;
				s_OpenListDataPool[wPairEdge].wCost = wCost;

				CBaseList4Path::Remove(s_OpenListDataPool + wPairEdge);

				pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
				while (pInsertPos != &OpenList.Rear() && pInsertPos && pInsertPos->wCost <= s_OpenListDataPool[wPairEdge].wCost)
					pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

				OpenList.InsertBefore(s_OpenListDataPool + wPairEdge, pInsertPos);
				PushCount++;
			}
		}
		else
		{
			if ((s_byVisitTable[wPairEdge / 8] & (1 << (wPairEdge % 8))) == 0)
			{
				s_byVisitTable[wPairEdge / 8] |= 1 << (wPairEdge % 8);
				s_OpenListDataPool[wPairEdge].pPrev = s_OpenListDataPool[wPairEdge].pNext = NULL;
				s_OpenListDataPool[wPairEdge].wCost = pOpenData->wCost;
				s_OpenListDataPool[wPairEdge].wFromEdge = pOpenData->wFromEdge;
			}
			else if (pOpenData->wCost < s_OpenListDataPool[wPairEdge].wCost)
			{
				s_OpenListDataPool[wPairEdge].wFromEdge = pOpenData->wFromEdge;
				s_OpenListDataPool[wPairEdge].wCost = pOpenData->wCost;

				CBaseList4Path::Remove(s_OpenListDataPool + wPairEdge);

				pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
				while (pInsertPos != &OpenList.Rear() && pInsertPos && pInsertPos->wCost <= s_OpenListDataPool[wPairEdge].wCost)
					pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

				OpenList.InsertBefore(s_OpenListDataPool + wPairEdge, pInsertPos);
				PushCount++;
			}
		}

		NAV_EDGE_DATA* pPairEdge = m_NavEdgeData + m_NavEdgeData[pOpenData->wEdge].wPairEdge;
		for (INT32 nIndex = 0; nIndex < MAX_EDGE_PER_POLY - 1 && pPairEdge->wLinkEdge[nIndex] != 0xFFFF; nIndex++)
		{
			UINT16 wEdge = pPairEdge->wLinkEdge[nIndex];
			OPEN_LIST_DATA* pLinkData = (OPEN_LIST_DATA*)s_OpenListDataPool + wEdge;

			wCost = pOpenData->wCost + pPairEdge->wLinkCost[nIndex];
			if (m_NavEdgeData[wEdge].wStartEdge == wSrcEdge)
			{
				wCost += _PathFindingDistance(nSrcX, nSrcY, nSrcZ, m_NavVertexData[wEdge].nX, m_NavVertexData[wEdge].nY, m_NavVertexData[wEdge].nZ);
			}

			if ((s_byVisitTable[wEdge / 8] & (1 << (wEdge % 8))) == 0)
			{
				s_byVisitTable[wEdge / 8] |= 1 << (wEdge % 8);
				pLinkData->pPrev = pLinkData->pNext = NULL;
				pLinkData->wFromEdge = pOpenData->wEdge;
				pLinkData->wCost = wCost;

				if (pLinkData->wCost < ((OPEN_LIST_DATA*)OpenList.Rear().pPrev)->wCost / 2)
				{
					pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
					while (pInsertPos != &OpenList.Rear() && pInsertPos && pInsertPos->wCost <= pLinkData->wCost)
						pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

					OpenList.InsertBefore(pLinkData, pInsertPos);
				}
				else
				{
					pInsertPos = (OPEN_LIST_DATA*)OpenList.Rear().pPrev;
					while (pInsertPos != &OpenList.Head() && pInsertPos && pInsertPos->wCost > pLinkData->wCost)
						pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pPrev;

					OpenList.InsertAfter(pLinkData, pInsertPos);
				}
				PushCount++;
			}
			else if (wCost < pLinkData->wCost)
			{
				pLinkData->wFromEdge = pOpenData->wEdge;
				pLinkData->wCost = wCost;

				CBaseList4Path::Remove(pLinkData);

				if (pLinkData->wCost < ((OPEN_LIST_DATA*)OpenList.Rear().pPrev)->wCost / 2)
				{
					pInsertPos = (OPEN_LIST_DATA*)OpenList.Head().pNext;
					while (pInsertPos != &OpenList.Rear() && pInsertPos && pInsertPos->wCost <= pLinkData->wCost)
						pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pNext;

					OpenList.InsertBefore(pLinkData, pInsertPos);
				}
				else
				{
					pInsertPos = (OPEN_LIST_DATA*)OpenList.Rear().pPrev;
					while (pInsertPos != &OpenList.Head() && pInsertPos && pInsertPos->wCost > pLinkData->wCost)
						pInsertPos = (OPEN_LIST_DATA*)pInsertPos->pPrev;

					OpenList.InsertAfter(pLinkData, pInsertPos);
				}

				PushCount++;
			}
		}
	}

	LOG_PROCESS_ERROR(!OpenList.IsEmpty());

	g_resultVertexCount = 0;
	s_wVertexList[g_resultVertexCount++] = nNavVertexCount;		// the src point

	if (m_NavEdgeData[pOpenData->wEdge].wPairEdge != 0xFFFF)
		s_wVertexList[g_resultVertexCount++] = m_NavEdgeData[pOpenData->wEdge].wPairEdge;
	while (pOpenData->wFromEdge != 0xFFFF && g_resultVertexCount < MAX_SEARCH_EDGE_COUNT - 1)
	{
		s_wVertexList[g_resultVertexCount++] = pOpenData->wFromEdge;
		pOpenData = s_OpenListDataPool + pOpenData->wFromEdge;
	}

	LOG_PROCESS_ERROR(g_resultVertexCount < MAX_SEARCH_EDGE_COUNT - 1);
	s_wVertexList[g_resultVertexCount++] = nNavVertexCount + 1;	// the dest point

	INT64 nRadius = 300 * Max(0, ((int)m_NetLatency + g_pathRadiusLagMS));
	if (nRadius < GRID_WIDTH)
		nRadius = GRID_WIDTH;
	else if (nRadius > GRID_WIDTH * 4)
		nRadius = GRID_WIDTH * 4;

	GenerateMedialPathPoint(nSrcX, nSrcY, nSrcZ, nDestX, nDestY, nDestZ, pnResult, nResultSize, rnResultSize, s_wVertexList, g_resultVertexCount, nRadius);

	LOG_PROCESS_ERROR(nResultSize < rnResultSize);
	pnResult[nResultSize * 3 + 0] = nDestX;
	pnResult[nResultSize * 3 + 1] = nDestY;
	pnResult[nResultSize * 3 + 2] = nDestZ;
	nResultSize++;

	rnResultSize = nResultSize;

	OpenList.Clear();

	//delete [] s_OpenListDataPool;
	//delete [] s_byVisitTable;
	//delete [] s_wVertexList;

	//g_moveCycles = Max(g_moveCycles, appCycles() - moveCycles);

	return TRUE;

Exit0:

	rnResultSize = nResultSize;

	OpenList.Clear();

	LogPathFindFail(TEXT("FServerGridWorld::GeneratePath faild @ 4"));
	return FALSE;
}

void ClampPortalAlongEdge(NAV_VERTEX_DATA* portalNewL, NAV_VERTEX_DATA* portalNewR, const NAV_VERTEX_DATA* portalL, const NAV_VERTEX_DATA* portalR, INT64 radius)
{
	INT64 dx = portalR->nX - portalL->nX;
	INT64 dy = portalR->nY - portalL->nY;
	INT64 dz = portalR->nZ - portalL->nZ;

	INT64 l = dx * dx + dy * dy + dz * dz;

	if (l >= (radius * radius * 4))
	{
		float f = (float)radius / appSqrt(l);

		portalNewL->nX = (portalL->nX + f * dx);
		portalNewL->nY = (portalL->nY + f * dy);
		portalNewL->nZ = (portalL->nZ + f * dz);

		portalNewR->nX = (portalR->nX - f * dx);
		portalNewR->nY = (portalR->nY - f * dy);
		portalNewR->nZ = (portalR->nZ - f * dz);
	}
	else
	{
		// not long enough, use the middle point
		portalNewR->nX = portalNewL->nX = (portalL->nX + portalR->nX) / 2;
		portalNewR->nY = portalNewL->nY = (portalL->nY + portalR->nY) / 2;
		portalNewR->nZ = portalNewL->nZ = (portalL->nZ + portalR->nZ) / 2;
	}
}

bool IsNavVertexDataEqual(const NAV_VERTEX_DATA* L, const NAV_VERTEX_DATA* R)
{
	return (L->nX == R->nX && L->nY == R->nY && L->nZ == R->nZ);
}


bool g_useMidPt = false;

void FServerGridWorld::GenerateMedialPathPoint(INT32 sx, INT32 sy, INT32 sz, INT32 dx, INT32 dy, INT32 dz,
	INT32 pnResult[], INT32& nResultSize, INT ResultBuffSize, UINT16* edgeList, INT32 edgeCount, INT64 nRadius)
{
	struct Portal
	{
		NAV_VERTEX_DATA ptLeft;
		NAV_VERTEX_DATA ptRight;

		void SetLeftPoint(INT32 x, INT32 y, INT32 z)
		{
			ptLeft.nX = x;
			ptLeft.nY = y;
			ptLeft.nZ = z;
		}

		void SetRightPoint(INT32 x, INT32 y, INT32 z)
		{
			ptRight.nX = x;
			ptRight.nY = y;
			ptRight.nZ = z;
		}
	};

	Portal* pPortals = NULL;
	Portal* pPortalsModified = NULL;

	Portal LocalPortalBuffer[1024];
	Portal LocalPortalBufferModified[1024];

	if (edgeCount < sizeof(LocalPortalBuffer) /  sizeof(LocalPortalBuffer[0]))
	{
		pPortals = LocalPortalBuffer;
		pPortalsModified = LocalPortalBufferModified;
	}
	else
	{
		pPortals = new Portal[edgeCount];
		pPortalsModified = new Portal[edgeCount];
	}

	pPortals[0].SetLeftPoint(sx, sy, sz);
	pPortals[0].SetRightPoint(sx, sy, sz);

	pPortalsModified[0].SetLeftPoint(sx, sy, sz);
	pPortalsModified[0].SetRightPoint(sx, sy, sz);
	
	
	for (INT32 nPortalEdgeIndex = 1; nPortalEdgeIndex < edgeCount - 1; nPortalEdgeIndex++)
	{
		NAV_EDGE_DATA* pEdgeData = m_NavEdgeData + edgeList[nPortalEdgeIndex];
		const NAV_VERTEX_DATA* portalL = m_NavVertexData + pEdgeData->wRightPoint;
		const NAV_VERTEX_DATA* portalR = m_NavVertexData + pEdgeData->wLeftPoint;

		pPortalsModified[nPortalEdgeIndex].ptLeft = m_NavFixedVertexData[pEdgeData->wRightPoint];
		pPortalsModified[nPortalEdgeIndex].ptRight = m_NavFixedVertexData[pEdgeData->wLeftPoint];

		pPortals[nPortalEdgeIndex].ptLeft = m_NavFixedVertexData[pEdgeData->wRightPoint];
		pPortals[nPortalEdgeIndex].ptRight = m_NavFixedVertexData[pEdgeData->wLeftPoint];

//		ClampPortalAlongEdge(&(pPortalsModified[nPortalEdgeIndex].ptLeft), &(pPortalsModified[nPortalEdgeIndex].ptRight), portalL, portalR, nRadius);
		//if(g_useMidPt)
		//{
		//	ClampPortalAlongEdge(&(pPortals[nPortalEdgeIndex].ptLeft), &(pPortals[nPortalEdgeIndex].ptRight), portalL, portalR, nRadius);
		//}
		//else
		//{
		//	ClampPortalAlongEdge(&(pPortals[nPortalEdgeIndex].ptLeft), &(pPortals[nPortalEdgeIndex].ptRight), portalL, portalR, 0);//GRID_WIDTH);
		//}
	}

	pPortals[edgeCount-1].SetLeftPoint(dx, dy, dz);
	pPortals[edgeCount-1].SetRightPoint(dx, dy, dz);

	pPortalsModified[edgeCount-1].SetLeftPoint(dx, dy, dz);
	pPortalsModified[edgeCount-1].SetRightPoint(dx, dy, dz);

	NAV_VERTEX_DATA *ptPortalPoint, *ptPortalPointLeft, *ptPortalPointRight;
	NAV_VERTEX_DATA *ptPortalPointModified, *ptPortalPointLeftModified, *ptPortalPointRightModified;
	INT32 nPortalEdge, nPortalLeftEdge, nPortalRightEdge;

	// here point on the edge corresponding
	ptPortalPoint = &(pPortals[0].ptLeft);
	ptPortalPointLeft = &(pPortals[0].ptLeft);
	ptPortalPointRight = &(pPortals[0].ptRight);

	ptPortalPointModified = &(pPortalsModified[0].ptLeft);
	ptPortalPointLeftModified = &(pPortalsModified[0].ptLeft);
	ptPortalPointRightModified = &(pPortalsModified[0].ptRight);

	nPortalEdge = nPortalLeftEdge = nPortalRightEdge = 0;

	for (INT32 nPortalEdgeIndex = 1; nPortalEdgeIndex < edgeCount; nPortalEdgeIndex++)
	{
		NAV_VERTEX_DATA *ptCurPortalPointLeft = &(pPortals[nPortalEdgeIndex].ptLeft);
		NAV_VERTEX_DATA *ptCurPortalPointRight = &(pPortals[nPortalEdgeIndex].ptRight);

		NAV_VERTEX_DATA *ptCurPortalPointLeftModified = &(pPortalsModified[nPortalEdgeIndex].ptLeft);
		NAV_VERTEX_DATA *ptCurPortalPointRightModified = &(pPortalsModified[nPortalEdgeIndex].ptRight);

		// Right vertex.
		if (_CollineationCheck(ptPortalPoint, ptPortalPointRight, ptCurPortalPointRight) >= 0)
		{
			if (IsNavVertexDataEqual(ptPortalPoint, ptPortalPointRight) || _CollineationCheck(ptPortalPoint, ptPortalPointLeft, ptCurPortalPointRight) < 0)
			{
				// clamp the portal right

				ptPortalPointRight = ptCurPortalPointRight;
				nPortalRightEdge = nPortalEdgeIndex;

				ptPortalPointRightModified = ptCurPortalPointRightModified;
			}
			else
			{
				// can't clamp the portal
				
				// walk to the left point
				if (nPortalLeftEdge == edgeCount - 1)
				{
					// now we can arrive at the destination directly, no medial path point, then loop end
					
				}
				else
				{
					// generate the path point, 
					LOG_PROCESS_ERROR(nResultSize < ResultBuffSize);
					pnResult[nResultSize * 3 + 0] = ptPortalPointLeftModified->nX;
					pnResult[nResultSize * 3 + 1] = ptPortalPointLeftModified->nY;
					pnResult[nResultSize * 3 + 2] = ptPortalPointLeftModified->nZ;

					if(g_useMidPt)
					{
						pnResult[nResultSize * 3 + 0] = ptPortalPointLeft->nX;
						pnResult[nResultSize * 3 + 1] = ptPortalPointLeft->nY;
						pnResult[nResultSize * 3 + 2] = ptPortalPointLeft->nZ;
					}

// 					FVector pt1, pt1m;
// 					ConvertPosServer2Client(ptPortalPointLeft->nX, ptPortalPointLeft->nY, ptPortalPointLeft->nZ, pt1);
// 					ConvertPosServer2Client(ptPortalPointLeftModified->nX, ptPortalPointLeftModified->nY, ptPortalPointLeftModified->nZ, pt1m);
// 
// 					GWorld->GetWorldInfo()->GetAPlayerController()->DrawDebugSphere(pt1, 5, 16, 0, 255, 255, TRUE);
// 					GWorld->GetWorldInfo()->GetAPlayerController()->DrawDebugSphere(pt1m, 5, 16, 255, 0, 255, TRUE);

					nResultSize++;
				}

				// and then re-cal the portal
				ptPortalPoint = ptPortalPointRight = ptPortalPointLeft;
				nPortalEdge = nPortalRightEdge = nPortalLeftEdge;

				nPortalEdgeIndex = nPortalEdge;


				ptPortalPointModified = ptPortalPointRightModified = ptPortalPointLeftModified;
				continue;
			}
		}

		// Left vertex.
		if (_CollineationCheck(ptPortalPoint, ptPortalPointLeft, ptCurPortalPointLeft) <= 0)
		{
			if (IsNavVertexDataEqual(ptPortalPoint, ptPortalPointLeft) || _CollineationCheck(ptPortalPoint, ptPortalPointRight, ptCurPortalPointLeft) > 0)
			{
				// clamp the portal left
				ptPortalPointLeft = ptCurPortalPointLeft;
				nPortalLeftEdge = nPortalEdgeIndex;

				ptPortalPointLeftModified = ptCurPortalPointLeftModified;
			}
			else
			{
				// can't clamp the portal

				// walk to the left point
				if (nPortalRightEdge == edgeCount - 1)
				{
					// now we can arrive at the destination directly, no medial path point, then loop end

				}
				else
				{
					// generate the path point, 
					LOG_PROCESS_ERROR(nResultSize < ResultBuffSize);
					pnResult[nResultSize * 3 + 0] = ptPortalPointRightModified->nX;
					pnResult[nResultSize * 3 + 1] = ptPortalPointRightModified->nY;
					pnResult[nResultSize * 3 + 2] = ptPortalPointRightModified->nZ;

					if(g_useMidPt)
					{
						pnResult[nResultSize * 3 + 0] = ptPortalPointRight->nX;
						pnResult[nResultSize * 3 + 1] = ptPortalPointRight->nY;
						pnResult[nResultSize * 3 + 2] = ptPortalPointRight->nZ;
					}

					nResultSize++;

// 					FVector pt1, pt1m;
// 					ConvertPosServer2Client(ptPortalPointLeft->nX, ptPortalPointLeft->nY, ptPortalPointLeft->nZ, pt1);
// 					ConvertPosServer2Client(ptPortalPointRightModified->nX, ptPortalPointRightModified->nY, ptPortalPointRightModified->nZ, pt1m);
// 
// 					GWorld->GetWorldInfo()->GetAPlayerController()->DrawDebugSphere(pt1, 5, 16, 0, 255, 255, TRUE);
// 					GWorld->GetWorldInfo()->GetAPlayerController()->DrawDebugSphere(pt1m, 5, 16, 255, 0, 255, TRUE);
				}

				// and then re-cal the portal
				ptPortalPoint = ptPortalPointLeft = ptPortalPointRight;
				nPortalEdge = nPortalLeftEdge = nPortalRightEdge;

				nPortalEdgeIndex = nPortalEdge;

				ptPortalPointModified = ptPortalPointLeftModified = ptPortalPointRightModified;

				continue;
			}
		}
	}

Exit0:

	if (pPortals != LocalPortalBuffer)
	{
		delete [] pPortals;
	}

	if (pPortalsModified != LocalPortalBufferModified)
	{
		delete [] pPortalsModified;
	}
}

void FServerGridWorld::SyncActor4ServerWorld()
{

}

FServerGridWorld::FServerGridWorld()
{
	Init(NULL);
}

FServerGridWorld::~FServerGridWorld()
{
	if (*m_szSceneName != 0)
	{
		//appDebugBreak();
	}
	UnInit();
}

void DrawPickedGrid(const FVector v[4], float z0, float z1, FLinearColor color = FLinearColor(0.5f, 0, 0))
{
	FCanvas* pCanvas = NULL;

	ULineBatchComponent* LineBatcher = /*bPersistentLines ? GWorld->PersistentLineBatcher : */GWorld->LineBatcher;
	//LineBatcher->DrawLine(LineStart, LineEnd, FColor(R, G, B), SDPG_World);


		FVector ltu(v[0]);
		FVector rtu(v[1]);
		FVector rbu(v[2]);
		FVector lbu(v[3]);

		FVector ltb(v[0]);
		FVector rtb(v[1]);
		FVector rbb(v[2]);
		FVector lbb(v[3]);
		//FVector rbb(x1,y1,z1);

		ltu.Z = z0;
		lbu.Z = z0;
		rtu.Z = z0;
		rbu.Z = z0;

		ltb.Z = z1;
		lbb.Z = z1;
		rtb.Z = z1;
		rbb.Z = z1;

		//FLinearColor color(0.5f, 0, 0);

		LineBatcher->DrawLine(ltu, rtu, color, SDPG_World);
		LineBatcher->DrawLine(rtu, rbu, color, SDPG_World);
		LineBatcher->DrawLine(rbu, lbu, color, SDPG_World);
		LineBatcher->DrawLine(lbu, ltu, color, SDPG_World);

		LineBatcher->DrawLine(ltb, rtb, color, SDPG_World);
		LineBatcher->DrawLine(rtb, rbb, color, SDPG_World);
		LineBatcher->DrawLine(rbb, lbb, color, SDPG_World);
		LineBatcher->DrawLine(lbb, ltb, color, SDPG_World);

		LineBatcher->DrawLine(ltu, ltb, color, SDPG_World);
		LineBatcher->DrawLine(rtu, rtb, color, SDPG_World);
		LineBatcher->DrawLine(rbu, rbb, color, SDPG_World);
		LineBatcher->DrawLine(lbu, lbb, color, SDPG_World);
}

void DrawPickedGrid(INT64 x0, INT64 y0, INT64 z0, INT64 x1, INT64 y1, INT64 z1, FLinearColor color = FLinearColor(0.5f, 0, 0))
{
	FVector ltu, rbb;
	FServerGridWorld::ConvertPosServer2Client(x0, y0, z0, ltu);
	FServerGridWorld::ConvertPosServer2Client(x1, y1, z1, rbb);

	FCanvas* pCanvas = NULL;

	ULineBatchComponent* LineBatcher = /*bPersistentLines ? GWorld->PersistentLineBatcher : */GWorld->LineBatcher;
	//LineBatcher->DrawLine(LineStart, LineEnd, FColor(R, G, B), SDPG_World);


	//if (pCanvas != NULL)
	{
		float x0 = ltu.X;
		float y0 = ltu.Y;
		float z0 = ltu.Z;

		float x1 = rbb.X;
		float y1 = rbb.Y;
		float z1 = rbb.Z;

		//FVector ltu(x0,y0,z0);
		FVector lbu(x0,y1,z0);
		FVector rtu(x1,y0,z0);
		FVector rbu(x1,y1,z0);

		FVector ltb(x0,y0,z1);
		FVector lbb(x0,y1,z1);
		FVector rtb(x1,y0,z1);
		//FVector rbb(x1,y1,z1);

		//FLinearColor color(0.5f, 0, 0);

		LineBatcher->DrawLine(ltu, rtu, color, SDPG_World);
		LineBatcher->DrawLine(rtu, rbu, color, SDPG_World);
		LineBatcher->DrawLine(rbu, lbu, color, SDPG_World);
		LineBatcher->DrawLine(lbu, ltu, color, SDPG_World);

		LineBatcher->DrawLine(ltb, rtb, color, SDPG_World);
		LineBatcher->DrawLine(rtb, rbb, color, SDPG_World);
		LineBatcher->DrawLine(rbb, lbb, color, SDPG_World);
		LineBatcher->DrawLine(lbb, ltb, color, SDPG_World);

		LineBatcher->DrawLine(ltu, ltb, color, SDPG_World);
		LineBatcher->DrawLine(rtu, rtb, color, SDPG_World);
		LineBatcher->DrawLine(rbu, rbb, color, SDPG_World);
		LineBatcher->DrawLine(lbu, lbb, color, SDPG_World);
	}
}

UBOOL ZoneLineIntersect(FVector& ptIntersect,
	ZONE_DATA* pZoneData, INT64 sx, INT64 sy, INT64 sz, INT64 ex, INT64 ey, INT64 ez)
{ 
	if (pZoneData == NULL)
		return FALSE;

	float tx[GRID_PER_ZONE * 2] = {-1};
	float ty[GRID_PER_ZONE * 2] = {-1};


	INT64 dx = ex - sx;
	INT64 dy = ey - sy;
	INT64 dz = ez - sz;

	tx[0] = 0;

	INT32 xCount = 1;

	if (dx != 0)
	{
		INT64 x = (sx / GRID_WIDTH + (dx > 0 ? 1 : 0))* GRID_WIDTH;

		float t = 0;

		while (t >= 0 &&  t <= 1 && xCount < ARRAY_COUNT(tx))
		{
			t = (float)(x - sx) / dx;

			if (t > 0 && t < 1)
			{
				tx[xCount] = t;
				++xCount;
			}

			x += (dx > 0 ? GRID_WIDTH : -GRID_WIDTH);
		}
	}

	tx[xCount] = 1;
	++xCount;

	//////////////////////////////////////////////////////////////////////////

	INT32 yCount = 0;

	if (dy != 0)
	{
		INT64 y = (sy / GRID_WIDTH + (dy > 0 ? 1 : 0)) * GRID_WIDTH;

		float t = 0;

		while (t >= 0 &&  t <= 1 && yCount < ARRAY_COUNT(ty))
		{
			t = (float)(y - sy) / dy;

			if (t > 0 && t < 1)
			{
				ty[yCount] = t;
				++yCount;
			}

			y += (dy > 0 ? GRID_WIDTH : -GRID_WIDTH);
		}
	}

	INT32 xi = 0;
	INT32 yi = 0;

	float t0 = -1;
	float t1 = -1;

	while (xi < xCount || yi < yCount)
	{

		if (tx[xi] <= ty[yi] || yi >= yCount)
		{
			t1 = tx[xi];
			++xi;
		}
		else
		{
			t1 = ty[yi];
			++yi;
		}

		if (t0 != -1)
		{
			// to do here
			float t = 0.5f * (t0 + t1);

			INT64 llX = sx + t * dx;
			INT64 llY = sy + t * dy;
			INT64 llZ = sz + t * dz;

			INT64 Z0 = sz + t0 * dz;
			INT64 Z1 = sz + t1 * dz;

			INT32 nGridXInZone = (INT32)((llX / GRID_WIDTH) % GRID_PER_ZONE);
			INT32 nGridYInZone = (INT32)((llY / GRID_WIDTH) % GRID_PER_ZONE);
			
			UINT16 usLayer = (UINT16)(llZ / GRID_LAYER_HEIGHT);
			UINT16 usGridIndex = NULL;
			UINT16 usNextIndex = NULL;

			usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];

			const unsigned nSwing = 3;

			while (usGridIndex && usGridIndex < pZoneData->nGridCount)
			{
				GRID_DATA* pGrid = pZoneData->GridList + usGridIndex;

				if (pGrid &&
					usLayer >= (pGrid->uLowLayer > nSwing ? (pGrid->uLowLayer - nSwing) : 0)
					&& usLayer <= pGrid->uHighLayer + nSwing
					&& pGrid->bOutOfPoly == 0)
				{
					// intersect
					FServerGridWorld::ConvertPosServer2Client(llX, llY, llZ, ptIntersect);

					//DrawPickedGrid(sx+t0*dx,sy+t0*dy,Z0,sx+t1*dx,sy+t1*dy,Z1);
					return TRUE;
				}

				usGridIndex = pZoneData->GridList[usGridIndex].uNextGrid;
			}
		}

		t0 = t1;
	}

	return FALSE;
}



UBOOL ZoneLineIntersectEx(FVector& ptIntersect, FLOAT& deltaZ,
	ZONE_DATA* pZoneData, INT64 sx, INT64 sy, INT64 sz, INT64 ex, INT64 ey, INT64 ez)
{ 
	if (pZoneData == NULL)
		return FALSE;

	float tx[GRID_PER_ZONE * 2] = {-1};
	float ty[GRID_PER_ZONE * 2] = {-1};


	INT64 dx = ex - sx;
	INT64 dy = ey - sy;
	INT64 dz = ez - sz;

	tx[0] = 0;

	INT32 xCount = 1;

	if (dx != 0)
	{
		INT64 x = (sx / GRID_WIDTH + (dx > 0 ? 1 : 0))* GRID_WIDTH;

		float t = 0;

		while (t >= 0 &&  t <= 1 && xCount < ARRAY_COUNT(tx))
		{
			t = (float)(x - sx) / dx;

			if (t > 0 && t < 1)
			{
				tx[xCount] = t;
				++xCount;
			}

			x += (dx > 0 ? GRID_WIDTH : -GRID_WIDTH);
		}
	}

	tx[xCount] = 1;
	++xCount;

	//////////////////////////////////////////////////////////////////////////

	INT32 yCount = 0;

	if (dy != 0)
	{
		INT64 y = (sy / GRID_WIDTH + (dy > 0 ? 1 : 0)) * GRID_WIDTH;

		float t = 0;

		while (t >= 0 &&  t <= 1 && yCount < ARRAY_COUNT(ty))
		{
			t = (float)(y - sy) / dy;

			if (t > 0 && t < 1)
			{
				ty[yCount] = t;
				++yCount;
			}

			y += (dy > 0 ? GRID_WIDTH : -GRID_WIDTH);
		}
	}

	INT32 xi = 0;
	INT32 yi = 0;

	float t0 = -1;
	float t1 = -1;

	while (xi < xCount || yi < yCount)
	{

		if (tx[xi] <= ty[yi] || yi >= yCount)
		{
			t1 = tx[xi];
			++xi;
		}
		else
		{
			t1 = ty[yi];
			++yi;
		}

		if (t0 != -1)
		{
			// to do here
			float t = 0.5f * (t0 + t1);

			INT64 llX = sx + t * dx;
			INT64 llY = sy + t * dy;
			INT64 llZ = sz + t * dz;

			INT64 Z0 = sz + t0 * dz;
			INT64 Z1 = sz + t1 * dz;

			INT32 nGridXInZone = (INT32)((llX / GRID_WIDTH) % GRID_PER_ZONE);
			INT32 nGridYInZone = (INT32)((llY / GRID_WIDTH) % GRID_PER_ZONE);

			UINT16 usLayer = (UINT16)(llZ / GRID_LAYER_HEIGHT);
			UINT16 usGridIndex = NULL;
			UINT16 usNextIndex = NULL;

			usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];

			const unsigned nSwing = 3;

			while (usGridIndex && usGridIndex < pZoneData->nGridCount)
			{
				GRID_DATA* pGrid = pZoneData->GridList + usGridIndex;

				if (pGrid &&
					usLayer >= (pGrid->uLowLayer > nSwing ? (pGrid->uLowLayer - nSwing) : 0)
					&& usLayer <= pGrid->uHighLayer + nSwing
					&& pGrid->bOutOfPoly == 0)
				{
					// intersect
					FServerGridWorld::ConvertPosServer2Client(llX, llY, llZ, ptIntersect);

#ifdef _DEBUG
					//debug -draw//
					//FVector corner[4];
					//FServerGridWorld::GetGridCorner(corner, ptIntersect);


					////DrawPickedGrid(sx+t0*dx,sy+t0*dy,Z0,sx+t1*dx,sy+t1*dy,Z1);
					//DrawPickedGrid(corner, Z0 / POINT_PER_CM, Z0 / POINT_PER_CM,
					//	gServerGridWorld.IsBlockedActorExist(pGrid) ? FLinearColor(0.5f, 0, 0) : FLinearColor(0, 0.5f, 0));
					//~debug-draw//
#endif

					deltaZ = pGrid->uHighLayer*GRID_LAYER_HEIGHT/POINT_PER_CM - ptIntersect.Z;
					return TRUE;
				}

				usGridIndex = pZoneData->GridList[usGridIndex].uNextGrid;
			}
		}

		t0 = t1;
	}

	return FALSE;
}


UBOOL FServerGridWorld::Pick(FVector& ptFirstPicked, 
	const FVector& ptStartWorldClient, const FVector& ptEndWorldClient)
{
	FLOAT fDeltaHight = 0.0f;
	INT64 llStartX;
	INT64 llStartY;
	INT64 llStartZ;
	ZONE_DATA* pStartZone = NULL;
	GRID_DATA* pStartGrid = NULL;

	INT64 llEndX;
	INT64 llEndY;
	INT64 llEndZ;
	ZONE_DATA* pEndZone = NULL;
	GRID_DATA* pEndGrid = NULL;

	ConvertPosClient2Server(ptStartWorldClient, llStartX, llStartY, llStartZ);
	ConvertPosClient2Server(ptEndWorldClient, llEndX, llEndY, llEndZ);

	if(llStartZ == llEndZ)
	{
		appDebugBreak();
		return FALSE;
	}

	INT64 llDeltaX = llEndX - llStartX;
	INT64 llDeltaY = llEndY - llStartY;
	INT64 llDeltaZ = llEndZ - llStartZ;

	ZONE_DATA* pCurZone = NULL;
	GRID_DATA* pCurGrid = NULL;

	GetZoneAndGrid(llStartX, llStartY, llStartZ, pCurZone, pCurGrid);

	if (llDeltaX == 0 && llDeltaY == 0)
	{
		if (pCurZone && pCurGrid)
		{
			ConvertPosServer2Client(llStartX, llStartY, GRID_LAYER_HEIGHT * pCurGrid->uHighLayer, ptFirstPicked);
			return TRUE;
		}
		else
		{
			appDebugBreak();
			return FALSE;
		}
	}

	UBOOL XMajor = (Abs(llDeltaX) > Abs(llDeltaY) ? 1 : 0);
	UBOOL MajorSign = XMajor ? (llDeltaX > 0) : (llDeltaY > 0);
	UBOOL FollowSign = XMajor ? (llDeltaY > 0) : (llDeltaX > 0);

	INT64 EpsilonMajor = MajorSign ? -1 : 1;
	INT64 EpsilonFollow = FollowSign ? -1 : 1;

	INT64 DeltaMajor = XMajor ? llDeltaX : llDeltaY;
	INT64 StartMajor = XMajor ? llStartX : llStartY;

	INT64 DeltaFollow = XMajor ? llDeltaY : llDeltaX;
	INT64 StartFollow = XMajor ? llStartY : llStartX;

	INT64 Index = (MajorSign ? (StartMajor / ZONE_WIDTH + 1) : (StartMajor / ZONE_WIDTH));

	INT64 ZoneCount = ((MajorSign ? DeltaMajor : -DeltaMajor) / ZONE_WIDTH) + 1;
	INT64 ZoneIndex = 0;

	INT64 CurX = llStartX;
	INT64 CurY = llStartY;
	DOUBLE CurZ = llStartZ;

	DOUBLE CurMajor = StartMajor;
	DOUBLE CurFollow = StartFollow;

	while(ZoneIndex < ZoneCount)
	{
		ZONE_DATA* pNewZone;
		GRID_DATA* pNewGrid;

		DOUBLE NewMajor = Index * ZONE_WIDTH;
		DOUBLE NewFollow = (NewMajor - StartMajor ) / DeltaMajor * DeltaFollow + StartFollow;
		DOUBLE NewZ = (NewMajor - StartMajor ) / DeltaMajor * llDeltaZ + llStartZ;

		INT64 NewX = (XMajor ? NewMajor + EpsilonMajor : NewFollow + EpsilonFollow);
		INT64 NewY = (XMajor ? NewFollow + EpsilonFollow : NewMajor + EpsilonMajor);

		pNewZone = NULL;
		pNewGrid = NULL;
		GetZoneAndGrid(NewX, NewY, NewZ, pNewZone, pNewGrid);

		//FVector ptIntersect;

		if (pCurZone == pNewZone || pNewZone == NULL || pCurZone == NULL)
		{
			//pCurZone
			//ZoneLineIntersect(pCurZone, );
			if (pCurZone != NULL)
			{
				if (ZoneLineIntersectEx(ptFirstPicked, fDeltaHight,pCurZone, CurX, CurY, CurZ, NewX, NewY, NewZ) == TRUE)
				{
					if(fDeltaHight<-128.0 || fDeltaHight>128.0)
					{
						return FALSE;
					}
					return TRUE;
				}
			}
			else if (pNewZone != NULL)
			{
				if (ZoneLineIntersectEx(ptFirstPicked, fDeltaHight,pNewZone, CurX, CurY, CurZ, NewX, NewY, NewZ) == TRUE)
				{
					if(fDeltaHight<-128.0 || fDeltaHight>128.0)
					{
						return FALSE;
					}
					return TRUE;
				}
			}
		}
		else
		{
			//pCurZone, pNewZone
			check(DeltaFollow != 0);

			INT FF1 = (INT64)(CurFollow / ZONE_WIDTH);
			INT FF2 = (INT64)(NewFollow / ZONE_WIDTH);

			check(FF1 != FF2 || CurFollow * NewFollow < 0);

			// cal the intersect point

			DOUBLE TempFollow = (INT64)((FollowSign ? NewFollow : CurFollow) / ZONE_WIDTH) * ZONE_WIDTH;
			DOUBLE TempMajor = (TempFollow - StartFollow ) * DeltaMajor / DeltaFollow + StartMajor;
			INT64 TempZ = (TempMajor - StartMajor ) * llDeltaZ / DeltaMajor + llStartZ;

			INT64 TempX = 0;
			INT64 TempY = 0;

			if (pCurZone != NULL)
			{
				TempX = (XMajor ? TempMajor + EpsilonMajor: TempFollow + EpsilonFollow);
				TempY = (XMajor ? TempFollow + EpsilonFollow : TempMajor + EpsilonMajor);

#ifdef _DEBUG
				ZONE_DATA* pTempZone = NULL;
				GRID_DATA* pTempGrid = NULL;

				//GetZoneAndGrid(CurX, CurY, CurZ, pTempZone2, pTempGrid2);
				GetZoneAndGrid(TempX, TempY, TempZ, pTempZone, pTempGrid);
				if (pTempZone != NULL && pTempZone != pCurZone)
					appDebugBreak();
#endif

				if (ZoneLineIntersectEx(ptFirstPicked, fDeltaHight, pCurZone, CurX, CurY, CurZ, TempX, TempY, TempZ))
				{
					if(fDeltaHight<-128.0 || fDeltaHight>128.0)
					{
						return FALSE;
					}
					return TRUE;
				}
			}

			//////////////////////////////////////////////////////////////////////////

			if (pNewZone != NULL)
			{
				TempX = (XMajor ? TempMajor - EpsilonMajor: TempFollow - EpsilonFollow);
				TempY = (XMajor ? TempFollow - EpsilonFollow : TempMajor - EpsilonMajor);

#ifdef _DEBUG
				ZONE_DATA* pTempZone = NULL;
				GRID_DATA* pTempGrid = NULL;

				GetZoneAndGrid(TempX, TempY, TempZ, pTempZone, pTempGrid);
				//GetZoneAndGrid(NewX, NewY, NewZ, pTempZone2, pTempGrid2);
				if (pTempZone != NULL && pTempZone != pNewZone)
					appDebugBreak();
#endif

				if (ZoneLineIntersectEx(ptFirstPicked, fDeltaHight, pNewZone, TempX, TempY, TempZ, NewX, NewY, NewZ))
				{
					if(fDeltaHight<-128.0 || fDeltaHight>128.0)
					{
						return FALSE;
					}
					return TRUE;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		CurMajor = Index * ZONE_WIDTH;
		CurFollow = (CurMajor - StartMajor ) / DeltaMajor * DeltaFollow + StartFollow;
		CurZ = (CurMajor - StartMajor ) / DeltaMajor * llDeltaZ + llStartZ;

	
		CurX = (XMajor ? CurMajor - EpsilonMajor: CurFollow - EpsilonFollow); + 0.5f;
		CurY = (XMajor ? CurFollow - EpsilonFollow: CurMajor - EpsilonMajor);

		pCurZone = NULL;
		pCurGrid = NULL;
		GetZoneAndGrid(CurX, CurY, CurZ, pCurZone, pCurGrid);

		// loop
		MajorSign ? (++Index) : (--Index);
		++ZoneIndex;
	}

	return FALSE;
}

FVector FServerGridWorld::ValidatePoint( 
	const FVector& ptSour, const FVector& ptReference )
{
	return ptSour;
}

#if !SHIPPING_PC_GAME
void FServerGridWorld::DebugDrawPathResult( AActor* drawer )
{
	FVector drawDebugPathOffset(0, 0, 32.0f);
 	for (INT32 i = 0; i < g_resultVertexCount; ++i)
 	{
		FColor clr;
		if(i == 0)
		{
			clr = FColor(255, 255, 0);
		}
		else if(i == g_resultVertexCount - 1)
		{
			clr = FColor(0, 255, 255);
		}
		else
		{
			clr = FColor(0, 255, 0);
		}

		INT32 edgeIdx = s_wVertexList[i];
		if(edgeIdx < m_pNavHeader->wEdgeCount)
		{
			NAV_EDGE_DATA* edgeData = &m_NavEdgeData[edgeIdx];
			// draw linked edge
			for (INT j = 0; j < MAX_EDGE_PER_POLY - 1; ++j)
			{
				if(edgeData->wLinkEdge[j] == 0xFFFF)
				{
					break;
				}

				NAV_EDGE_DATA* linkEdgeData = &m_NavEdgeData[edgeData->wLinkEdge[j]];

				const NAV_VERTEX_DATA* portalL = m_NavVertexData + linkEdgeData->wRightPoint;
				const NAV_VERTEX_DATA* portalR = m_NavVertexData + linkEdgeData->wLeftPoint;

				FVector ptL;//(portalL->nX, portalL->nY, portalL->nZ);
				FVector ptR;//(portalR->nX, portalR->nY, portalR->nZ);

				ConvertPosServer2Client(portalL->nX, portalL->nY, portalL->nZ, ptL);
				ConvertPosServer2Client(portalR->nX, portalR->nY, portalR->nZ, ptR);

				drawer->DrawDebugLine(ptL + drawDebugPathOffset, ptR + drawDebugPathOffset, clr.R, clr.G, clr.B, TRUE);

			}

			// draw start edge
// 			{
// 				NAV_EDGE_DATA* startEdgeData = &m_NavEdgeData[edgeData->wStartEdge];
// 				const NAV_VERTEX_DATA* portalL = m_NavVertexData + startEdgeData->wRightPoint;
// 				const NAV_VERTEX_DATA* portalR = m_NavVertexData + startEdgeData->wLeftPoint;
// 
// 				FVector ptL;//(portalL->nX, portalL->nY, portalL->nZ);
// 				FVector ptR;//(portalR->nX, portalR->nY, portalR->nZ);
// 
// 				ConvertPosServer2Client(portalL->nX, portalL->nY, portalL->nZ, ptL);
// 				ConvertPosServer2Client(portalR->nX, portalR->nY, portalR->nZ, ptR);
// 
// 				drawer->DrawDebugLine(ptL + drawDebugPathOffset + FVector(0, 0, 10), ptR + drawDebugPathOffset + FVector(0, 0, 10), 128, 0, 0, TRUE);
// 			}


			// draw self
			{
				const NAV_VERTEX_DATA* portalL = m_NavVertexData + edgeData->wRightPoint;
				const NAV_VERTEX_DATA* portalR = m_NavVertexData + edgeData->wLeftPoint;

				FVector ptL;//(portalL->nX, portalL->nY, portalL->nZ);
				FVector ptR;//(portalR->nX, portalR->nY, portalR->nZ);

				ConvertPosServer2Client(portalL->nX, portalL->nY, portalL->nZ, ptL);
				ConvertPosServer2Client(portalR->nX, portalR->nY, portalR->nZ, ptR);

				//drawer->DrawDebugLine(ptL + drawDebugPathOffset + FVector(0, 0, 5), ptR + drawDebugPathOffset + FVector(0, 0, 5), clr.R, clr.G, clr.B, TRUE);
				drawer->DrawDebugLine(ptL + drawDebugPathOffset + FVector(0, 0, 10), ptR + drawDebugPathOffset + FVector(0, 0, 10), 255, 0, 0, TRUE);
			}
		}
 	}

	//return m_arrayPath;
}

#endif

void FServerGridWorld::Init( const TCHAR* pMapName )
{
	__super::Init(pMapName);

	m_bLoadResult = false;

	m_pNavHeader = NULL;
	m_NavVertexData = NULL;
	m_NavEdgeData = NULL;

	m_NetLatency = 100;

	m_DynamicBLock = FALSE;

#if defined(CLIENT_GRID_EXT)
	m_mapGridExtData.Empty();
#endif

	if( ParseParam(appCmdLine(),TEXT("NOBLOCKCHECK") ) )
	{
		gClientBlockCheck = FALSE;
	}
}

void FServerGridWorld::UnInit()
{
	__super::UnInit();

	m_bLoadResult = false;

	delete m_pNavHeader;
	m_pNavHeader = NULL;

	delete [] m_NavVertexData;
	m_NavVertexData = NULL;

	delete [] m_NavEdgeData;
	m_NavEdgeData = NULL;

#if defined(CLIENT_GRID_EXT)
	m_mapGridExtData.Empty();
#endif
}


#if defined(CLIENT_GRID_EXT)

UBOOL FServerGridWorld::IsBlockedActorExist( GRID_DATA* pGrid ) const
{
	if (gClientBlockCheck)
	{
		if (pGrid == NULL)
			return FALSE;

		const GRID_DATA_EXT* pDataExt = m_mapGridExtData.Find(pGrid);

		if (pDataExt == NULL)
			return FALSE;

		return (pDataExt->numBlockedActors > 0);
	}
	else
	{
		return FALSE;
	}
}

void FServerGridWorld::OnActorPosChanged( AActor* pActor, const FVector& oldLoc, const FVector& newLoc, INT32 BlockRadius )
{
	if (pActor && oldLoc != newLoc)
	{
		ZONE_DATA *pOldZone = NULL, *pNewZone = NULL;	// zhaojun@snda
		GRID_DATA *pOldGrid = NULL, *pNewGrid = NULL;	// zhaojun@snda

		INT32 nRadius = BlockRadius;

		//check(pActor->Location == oldLoc);

		for (INT32 x = -nRadius; x <= nRadius; ++x)
		{
			for (INT32 y = -nRadius; y <= nRadius; ++y)
			{
				FVector pos(x*(float)(GRID_WIDTH/POINT_PER_CM), y*(float)(GRID_WIDTH/POINT_PER_CM), (float)(GRID_WIDTH/POINT_PER_CM));
				FVector pos2(pos);

				pos += oldLoc;
				pos2 += newLoc;

				GetZoneAndGrid(pos, pOldZone, pOldGrid);	// zhaojun@snda
				GetZoneAndGrid(pos2, pNewZone, pNewGrid);

				if (pOldGrid != pNewGrid)
				{
					RemoveActor(pOldGrid, pActor);
					AddActor(pNewGrid, pActor);

					if (x ==0 && y == 0)
						pActor->OnZoneOrGridChanged(pOldZone, pOldGrid, pNewZone, pNewGrid);
				}
			}
		}
	}
}

void FServerGridWorld::OnActorBlockRadiusChanged( AActor* pActor, const FVector& loc,INT32 OldBlockRadius, INT32 NewBlockRadius )
{
	if (pActor && OldBlockRadius != NewBlockRadius)
	{
		ZONE_DATA *pOldZone = NULL, *pNewZone = NULL;	// zhaojun@snda
		GRID_DATA *pOldGrid = NULL, *pNewGrid = NULL;	// zhaojun@snda

		for (INT32 x = -OldBlockRadius; x <= OldBlockRadius; ++x)
		{
			for (INT32 y = -OldBlockRadius; y <= OldBlockRadius; ++y)
			{
				FVector pos(x*(float)(GRID_WIDTH/POINT_PER_CM), y*(float)(GRID_WIDTH/POINT_PER_CM), (float)(GRID_WIDTH/POINT_PER_CM));
				pos += loc;

				GetZoneAndGrid(pos, pOldZone, pOldGrid);	// zhaojun@snda

				RemoveActor(pOldGrid, pActor);
			}
		}

		for (INT32 x = -NewBlockRadius; x <= NewBlockRadius; ++x)
		{
			for (INT32 y = -NewBlockRadius; y <= NewBlockRadius; ++y)
			{
				FVector pos(x*(float)(GRID_WIDTH/POINT_PER_CM), y*(float)(GRID_WIDTH/POINT_PER_CM), (float)(GRID_WIDTH/POINT_PER_CM));
				pos += loc;

				GetZoneAndGrid(pos, pNewZone, pNewGrid);	// zhaojun@snda

				AddActor(pNewGrid, pActor);
			}
		}
	}
}

#endif

FServerGridWorld gServerGridWorld;
UBOOL gClientBlockCheck = TRUE;



// zhaojun@snda
