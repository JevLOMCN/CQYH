/*=============================================================================
	EditorLevelUtils.cpp: Editor-specific level management routines
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include "UnrealEd.h"
#include "EditorLevelUtils.h"

#include "BusyCursor.h"
#include "DlgGenericComboEntry.h"
#include "EngineProcBuildingClasses.h"
#include "FileHelpers.h"
#include "LevelBrowser.h"
#include "LevelUtils.h"
 // oscar@snda
#include <map>
#include "SourceControl.h"
#include "BSPOps.h"
#include "EngineAnimClasses.h"
#include "EngineFoliageClasses.h"
#include "LandscapeEdit.h"
#include "UnObjectTools.h"
// oscar@snda



namespace EditorLevelUtils
{
	/** Global: True if we're currently updating levels for actors */
	static UBOOL GIsUpdatingLevelsForActors = FALSE;


	/**
	 * Returns true if we're current in the middle of updating levels for actors.  This is used to
	 * prevent re-entrancy problems where 'actor moved' callbacks and fire off as we're moving
	 * actors between levels and such.
	 *
	 * @return	True if we're currently updating levels
	 */
	UBOOL IsCurrentlyUpdatingLevelsForActors()
	{
		return GIsUpdatingLevelsForActors;	
	}


	/**
	 * Tries to find the best possible level for a level grid volume actor based on spatial overlap
	 *
	 * @param	InActor		The actor to find the best level for
	 *
	 * @return	The level streaming object that represents the best level we could find for this actor
	 */
	ULevelStreaming* FindBestLevelForActorInLevelGridVolume( AActor* InActor )
	{
		// Is this actor in a level that's associated with a grid volume?
		ULevelStreaming* LevelStreaming = FLevelUtils::FindStreamingLevel( InActor->GetLevel() );
		check( LevelStreaming != NULL );
		check( LevelStreaming->EditorGridVolume->IsActorMemberOfGrid( InActor ) );


		// Find the best grid cell to place the actor in
		ULevelStreaming* BestLevelStreaming = NULL;
		{
			// Grab this actor's world space AABB
			FBox ActorAABB;
			ActorAABB = InActor->GetComponentsBoundingBox(TRUE);	// Out // oscar@snda

			// If the actor returns a null or zero-volume bounds then use the actor's origin by default
			if( ActorAABB.GetVolume() < KINDA_SMALL_NUMBER ||
				( ActorAABB.Min == FVector::ZeroVector && ActorAABB.Max == FVector::ZeroVector ) )
			{
				ActorAABB.Min = ActorAABB.Max = InActor->Location;
			}


			// Is this actor based on another actor?
			if( InActor->Base != NULL )
			{
				ULevel* BaseActorLevel = InActor->Base->GetLevel();
				if( BaseActorLevel != NULL )
				{
					// Always place actor in it's base actor's level if we can
					BestLevelStreaming = FLevelUtils::FindStreamingLevel( BaseActorLevel );
				}
			}


			if( BestLevelStreaming == NULL )
			{
				const UBOOL bMustOverlap = FALSE;
				FLevelGridCellCoordinate BestGridCell;
				if( ensure( LevelStreaming->EditorGridVolume->FindBestGridCellForBox( ActorAABB, bMustOverlap, BestGridCell ) ) )
				{
					// Now find the actual level that goes with this grid cell
					BestLevelStreaming = LevelStreaming->EditorGridVolume->FindLevelForGridCell( BestGridCell );
					if( BestLevelStreaming == NULL )
					{
						// No level has been created for this grid cell yet.  We'll create one now.
						FGridAndCellCoordinatePair GridAndCellPair;
						GridAndCellPair.GridVolume = LevelStreaming->EditorGridVolume;
						GridAndCellPair.CellCoordinate = BestGridCell;
						ULevel* CreatedLevel = CreateLevelForGridCell( GridAndCellPair );
						if( CreatedLevel != NULL )
						{
							BestLevelStreaming = LevelStreaming->EditorGridVolume->FindLevelForGridCell( BestGridCell );
						}
						else
						{
							// Was unable to create a new level
						}
					}
				}
			}
		}


		if( ensure( BestLevelStreaming != NULL && BestLevelStreaming->LoadedLevel != NULL ) )
		{
			return BestLevelStreaming;
		}

		return NULL;
	}


	
	/**
	 * Moves the specified list of actors to the specified level
	 *
	 * @param	ActorsToMove		List of actors to move
	 * @param	DestLevelStreaming	The level streaming object associated with the destination level
	 * @param	OutNumMovedActors	The number of actors that were successfully moved to the new level
	 */
	void MovesActorsToLevel( TLookupMap< AActor* >& ActorsToMove, ULevelStreaming* DestLevelStreaming, INT& OutNumMovedActors )
	{
		OutNumMovedActors = 0;


		// Backup the current contents of the clipboard string as we'll be using cut/paste features to move actors
		// between levels and this will trample over the clipboard data.
		const FString OriginalClipboardContent = appClipboardPaste();

		check( DestLevelStreaming != NULL );
		ULevel* DestLevel = DestLevelStreaming->LoadedLevel;
		check( DestLevel != NULL );


		// Save the current level so we can restore it later
		ULevel* OldCurrentLevel = GWorld->CurrentLevel;

		
		// Deselect all actors
		GEditor->Exec(TEXT("ACTOR SELECT NONE"));


		// Set the current level to the destination level.  Any 'pasted' actors will land in this level.
		GWorld->CurrentLevel = DestLevel;
		const FString& NewLevelName = DestLevelStreaming->PackageName.ToString();


		for( TLookupMap< AActor* >::TIterator CurActorIt( ActorsToMove ); CurActorIt; ++CurActorIt )
		{
			AActor* CurActor = CurActorIt.Key();
			check( CurActor != NULL );

			debugf( NAME_DevGarbage, TEXT("GC -1.MovesActorsToLevel: %x"),
#ifdef _WIN64
				(UINT64)CurActor
#else
				(UINT)CurActor
#endif
				);  // oscar@snda

			if(CurActor->GetLevel() == DestLevel)
			{
				warnf(NAME_DevLevelTools, TEXT("Skip the actor moving to the same level: %s -> %s"), *CurActor->GetPathName(), *NewLevelName);
				continue;
			}

			warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Moving %s to %s" ), *CurActor->GetPathName(), *NewLevelName );

			if( !FLevelUtils::IsLevelLocked( DestLevel ) && !FLevelUtils::IsLevelLocked( CurActor ) )
			{
				ULevelStreaming* ActorPrevLevel = FLevelUtils::FindStreamingLevel( CurActor->GetLevel() );
				const FString& PrevLevelName = ActorPrevLevel != NULL ? ActorPrevLevel->PackageName.ToString() : CurActor->GetLevel()->GetName();
				warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Moving %s from %s to %s" ), *CurActor->GetPathName(), *PrevLevelName, *NewLevelName );

				// Select this actor
				GEditor->SelectActor( CurActor, TRUE, NULL, FALSE, TRUE );
			}
			else 
			{
				// Either the source or destination level was locked!
				// @todo: Display warning
			}
		}


		if( GEditor->GetSelectedActorCount() > 0 )
		{
			// @todo: Perf: Not sure if this is needed here.
			GEditor->NoteSelectionChange();

			// Move the actors!
			const UBOOL bUseCurrentLevelGridVolume = FALSE;
			GEditor->MoveSelectedActorsToCurrentLevel( bUseCurrentLevelGridVolume );

			// The moved (pasted) actors will now be selected
			OutNumMovedActors += GEditor->GetSelectedActorCount();
		}


		// Restore the old current level
		GWorld->CurrentLevel = OldCurrentLevel;


		// Restore the original clipboard contents
		appClipboardCopy( *OriginalClipboardContent );
	}

	void MovesActorsToLevel( TLookupMap< AActor* >& ActorsToMove, ULevel* DestLevel, INT& OutNumMovedActors )
	{
		OutNumMovedActors = 0;


		// Backup the current contents of the clipboard string as we'll be using cut/paste features to move actors
		// between levels and this will trample over the clipboard data.
		const FString OriginalClipboardContent = appClipboardPaste();

		check( DestLevel != NULL );


		// Save the current level so we can restore it later
		ULevel* OldCurrentLevel = GWorld->CurrentLevel;


		// Deselect all actors
		GEditor->Exec(TEXT("ACTOR SELECT NONE"));


		// Set the current level to the destination level.  Any 'pasted' actors will land in this level.
		GWorld->CurrentLevel = DestLevel;
		const FString& NewLevelName = DestLevel->GetOutermost()->GetName();


		for( TLookupMap< AActor* >::TIterator CurActorIt( ActorsToMove ); CurActorIt; ++CurActorIt )
		{
			AActor* CurActor = CurActorIt.Key();
			check( CurActor != NULL );

			debugf( NAME_DevGarbage, TEXT("GC -1.MovesActorsToLevel: %x"),
#ifdef _WIN64
				(UINT64)CurActor
#else
				(UINT)CurActor
#endif
				);  // oscar@snda

			if(CurActor->GetLevel() == DestLevel)
			{
				warnf(NAME_DevLevelTools, TEXT("Skip the actor moving to the same level: %s -> %s"), *CurActor->GetPathName(), *NewLevelName);
				continue;
			}

			warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Moving %s to %s" ), *CurActor->GetPathName(), *NewLevelName );

			if( !FLevelUtils::IsLevelLocked( DestLevel ) && !FLevelUtils::IsLevelLocked( CurActor ) )
			{
				ULevelStreaming* ActorPrevLevel = FLevelUtils::FindStreamingLevel( CurActor->GetLevel() );
				const FString& PrevLevelName = ActorPrevLevel != NULL ? ActorPrevLevel->PackageName.ToString() : CurActor->GetLevel()->GetName();
				warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Moving %s from %s to %s" ), *CurActor->GetPathName(), *PrevLevelName, *NewLevelName );

				// Select this actor
				GEditor->SelectActor( CurActor, TRUE, NULL, FALSE, TRUE );
			}
			else 
			{
				// Either the source or destination level was locked!
				// @todo: Display warning
			}
		}


		if( GEditor->GetSelectedActorCount() > 0 )
		{
			// @todo: Perf: Not sure if this is needed here.
			GEditor->NoteSelectionChange();

			// Move the actors!
			const UBOOL bUseCurrentLevelGridVolume = FALSE;
			GEditor->MoveSelectedActorsToCurrentLevel( bUseCurrentLevelGridVolume );

			// The moved (pasted) actors will now be selected
			OutNumMovedActors += GEditor->GetSelectedActorCount();
		}


		// Restore the old current level
		GWorld->CurrentLevel = OldCurrentLevel;


		// Restore the original clipboard contents
		appClipboardCopy( *OriginalClipboardContent );
	}



	/**
	 * Finds the level grid volume that an actor belongs to and returns it
	 *
	 * @param	InActor		The actor to find the level grid volume for
	 *
	 * @return	The level grid volume this actor belong to, or NULL if the actor doesn't belong to a level grid volume
	 */
	ALevelGridVolume* GetLevelGridVolumeForActor( AActor* InActor )
	{
		check( InActor != NULL );

		// Is this actor in a level that's associated with a grid volume?
		ULevelStreaming* LevelStreaming = FLevelUtils::FindStreamingLevel( InActor->GetLevel() );
		if( LevelStreaming != NULL && LevelStreaming->EditorGridVolume != NULL )
		{
			// Actor is in a level grid volume, so return that
			return LevelStreaming->EditorGridVolume;
		}

		// Actor does not belong to a level grid volume
		return NULL;
	}



	/**
	 * For each actor in the specified list, checks to see if the actor needs to be moved to
	 * a different grid volume level, and if so queues the actor to be moved
	 *
	 * @param	InActorsToProcess	List of actors to update level membership for.  Note that this list may be modified by the function.
	 */
	void UpdateLevelsForActorsInLevelGridVolumes( TArray< AActor* >& InActorsToProcess )
	{
		// Check for re-entrancy!
		check( !IsCurrentlyUpdatingLevelsForActors() );
		GIsUpdatingLevelsForActors = TRUE;


		warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Updating levels for %i actors" ), InActorsToProcess.Num() );

		// Build a list of actors to process, with parent actors appearing before their children
		// Also, make sure that all based actors are also in this list.  If a parent actor is moved
		// to a new level, its children may need to be relocated as well!
		// @todo perf: The following operation is effectively O(N^2)
		TLookupMap< AActor* > OrderedActorsToProcess;
		INT NumActorsInGridVolumes = 0;
		{
			// NOTE: We'll grow this list as we're iterating over it to catch child actors
			const INT OriginalActorCount = InActorsToProcess.Num();
			for( INT CurActorIndex = 0; CurActorIndex < InActorsToProcess.Num(); ++CurActorIndex )
			{
				AActor* CurActor = InActorsToProcess( CurActorIndex );

				if(CurActor->IsA(ALandscapeProxy::StaticClass())
					|| CurActor->IsA(ALandscape::StaticClass()))
				{
					continue;
				}

				// Make sure this actor isn't a streaming volume itself!
				// Also, ignore other actor types (such as ProcBuilding low LOD actors, which are destroyed
				// any re-created automatically by the editor.)
				if( !CurActor->IsA( ALevelStreamingVolume::StaticClass() ) &&
					!CurActor->IsA( ALevelGridVolume::StaticClass() ) &&
					!CurActor->IsA( AProcBuilding_SimpleLODActor::StaticClass() ) &&
					!CurActor->IsA( ADefaultPhysicsVolume::StaticClass() ) &&
					!CurActor->IsA( AWorldInfo::StaticClass() ) &&
					!CurActor->IsABuilderBrush() )
				{
					// Is this actor in a level that's associated with a grid volume?
					if( GetLevelGridVolumeForActor( CurActor ) != NULL )
					{
						++NumActorsInGridVolumes;

						// Keep track of where our children lie within the list, so that we can make sure
						// to insert the parent before its children
						UINT MinChildListIndex = (UINT)INDEX_NONE;

						// Find this actor's children
						TArray<AActor*> ChildActors;
						for( FActorIterator AllActorsIt; AllActorsIt; ++AllActorsIt )
						{
							AActor* TestActor = *AllActorsIt;

							// Make sure the actor we're testing isn't our current actor, and that it's actually
							// parented to another actor
							if( TestActor != CurActor && !TestActor->bDeleteMe && TestActor->Base != NULL )
							{
								// Is this my direct child?
								if( TestActor->Base == CurActor )
								{
									// Add this actor to our list of actors to process, if we don't already have it
									// @todo perf: Use a TLookupMap for this list as well to speed up this test?
									if( !InActorsToProcess.ContainsItem( TestActor ) )
									{
										InActorsToProcess.AddItem( TestActor );
									}

									// Update min child list index
									INT* ExistingIndex = OrderedActorsToProcess.Find( CurActor );
									if( ExistingIndex != NULL )
									{
										// @todo: Not sure if this code is ever hit in practice!
										const UINT ChildIndexInList = (UINT)*ExistingIndex;
										if( ChildIndexInList < MinChildListIndex )
										{
											MinChildListIndex = ChildIndexInList;
										}
									}
								}
							}
						}

						// Now add the current actor to the list, making sure that it appears before children.
						// Also note that it's possible that we've already been added (as a child of another actor.)
						// In that case, we would have already been added to the list after our parents.
						if( !OrderedActorsToProcess.HasKey( CurActor ) )
						{
							if( MinChildListIndex != (UINT)INDEX_NONE )
							{
								OrderedActorsToProcess.InsertItem( CurActor, MinChildListIndex );
							}
							else
							{
								OrderedActorsToProcess.AddItem( CurActor );
							}
						}
					}
				}
			}

			const INT NumBasedActorsAdded = InActorsToProcess.Num() - OriginalActorCount;
			if( NumBasedActorsAdded > 0 )
			{
				warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Found an additional %i actor(s) that were parented to actors in this list" ), NumBasedActorsAdded );
			}
		}


		warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Processing %i actors (includes based actors)" ), OrderedActorsToProcess.Num() );

		typedef TMap< ULevelStreaming*, TLookupMap<AActor*> > FLevelToActorsMap;
		FLevelToActorsMap LevelToActorsMap;

		// Iterate over actors that need to be processed.  Note that this list is ordered such that parents
		// always appear before their children
		INT TotalActorCount = 0;
		INT MovedActorCount = 0;
		for( TLookupMap< AActor* >::TIterator CurActorIt( OrderedActorsToProcess ); CurActorIt; ++CurActorIt )
		{
			AActor* CurActor = CurActorIt.Key();

			ULevelStreaming* BestLevelStreaming = NULL;


			// Is this actor based on another actor?
			if( CurActor->Base != NULL )
			{
				// Check to see if the base actor is in our already-processed list of actors.  Remember, we
				// always process parents before children so if the parent is due to be moved, it will definitely
				// be in this list.  This handles the case where the base actor is due to be moved to a new level
				// but hasn't been moved yet.  We want attached actors to end up in the same level as their parent!
				{
					for( FLevelToActorsMap::TIterator LevelIt( LevelToActorsMap ); LevelIt; ++LevelIt )
					{
						ULevelStreaming* CurLevelStreaming = LevelIt.Key();
						check( CurLevelStreaming != NULL );

						// Note: This array is ordered such that parent actors appear before their children
						TLookupMap< AActor* >& ActorsArray = LevelIt.Value();
						if( ActorsArray.HasKey( CurActor->Base ) )
						{
							// Found the parent actor!  We'll make sure the child is placed into the same level
							BestLevelStreaming = CurLevelStreaming;
							break;
						}
					}
				}
			}


			if( BestLevelStreaming == NULL )
			{
				// Find the best grid cell to place the actor in
				BestLevelStreaming = FindBestLevelForActorInLevelGridVolume( CurActor );
			}


			if( BestLevelStreaming != NULL && BestLevelStreaming->LoadedLevel != NULL )
			{
				// Does the actor need to be moved to a new level?
				ULevel* BestLevel = BestLevelStreaming->LoadedLevel;
				if( CurActor->GetLevel() != BestLevel )
				{
					if( !LevelToActorsMap.HasKey( BestLevelStreaming ) )
					{
						LevelToActorsMap.Set( BestLevelStreaming, TLookupMap< AActor* >() );
					}

					// Note that its important to maintain the order of the actors in the per-level
					// actor array, as parents must appear in the list before children
					TLookupMap< AActor* >& ActorsArray = *LevelToActorsMap.Find( BestLevelStreaming );
					if( !ActorsArray.HasKey( CurActor ) )
					{
						ActorsArray.AddItem( CurActor );
					}
				}
			}

			++TotalActorCount;
		}


		// For each destination level, move actors to that level
		for( FLevelToActorsMap::TIterator LevelIt( LevelToActorsMap ); LevelIt; ++LevelIt )
		{
			ULevelStreaming* DestLevelStreaming = LevelIt.Key();
			check( DestLevelStreaming != NULL );

			// Note: This array is ordered such that parent actors appear before their children
			TLookupMap< AActor* >& ActorsArray = LevelIt.Value();

			// Move actors to the level!
			INT NumActorsMovedToLevel = 0;
			MovesActorsToLevel( ActorsArray, DestLevelStreaming, NumActorsMovedToLevel );

			MovedActorCount += NumActorsMovedToLevel;
		}

		warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Processed %i actors in grid volumes (of %i total actors in world)" ), NumActorsInGridVolumes, TotalActorCount );
		if( MovedActorCount > 0 )
		{
			warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Moved %i actors to a different level" ), MovedActorCount );
		}
		else
		{
			warnf( NAME_DevLevelTools, TEXT( "AutoLevel: No actors were moved" ), MovedActorCount );
		}
		warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Finished updating levels for %i actors" ), InActorsToProcess.Num() );


		if( MovedActorCount > 0 )
		{
			// Queue a level browser update because the actor counts may have changed
			GCallbackEvent->Send( CALLBACK_RefreshEditor_LevelBrowser ); 
		}


		GIsUpdatingLevelsForActors = FALSE;
	}


	/**
	 * Scans all actors in memory and checks to see if each actor needs to be moved to a different
	 * grid volume level, and if so queues the actor to be moved
	 */
	void UpdateLevelsForAllActors()
	{
		// Never move actors around while Matinee is open as their locations may be temporary
		if( !GEditorModeTools().IsModeActive( EM_InterpEdit ) )
		{
			warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Updating levels for all actors" ) );

			//UObject::CollectGarbage( GARBAGE_COLLECTION_KEEPFLAGS ); // oscar@snda

			// Build up a list of actors to process
			// @todo gridvolume: Multiple GCs will happen in this stack frame which may invalidate these ptrs!
			TArray< AActor* > ActorsToProcess;
			for( FActorIterator CurActorIt; CurActorIt; ++CurActorIt )
			{
				AActor* CurActor = *CurActorIt;
				if( CurActor != NULL )
				{
					ActorsToProcess.AddItem( CurActor );
				}
			}

			// No need to gather based actors since we're already passing in every actor!
			UpdateLevelsForActorsInLevelGridVolumes( ActorsToProcess );

			warnf( NAME_DevLevelTools, TEXT( "AutoLevel: Finished updating levels for all actors" ) );
		}
	}


	static TArray<FString> GStreamingMethodStrings;
	static TArray<UClass*> GStreamingMethodClassList;

	/**
	 * Initializes the list of possible level streaming methods. 
	 * Does nothing if the lists are already initialized.
	 */
	void InitializeStreamingMethods()
	{
		check( GStreamingMethodStrings.Num() == GStreamingMethodClassList.Num() );
		if ( GStreamingMethodClassList.Num() == 0 )
		{
			// Assemble a list of possible level streaming methods.
			for ( TObjectIterator<UClass> It ; It ; ++It )
			{
				if ( It->IsChildOf( ULevelStreaming::StaticClass() ) &&
					(It->ClassFlags & CLASS_EditInlineNew) &&
					!(It->ClassFlags & CLASS_Hidden) &&
					!(It->ClassFlags & CLASS_Abstract) &&
					!(It->ClassFlags & CLASS_Deprecated) &&
					!(It->ClassFlags & CLASS_Transient) )
				{
					const FString ClassName( It->GetName() );
					// Strip the leading "LevelStreaming" text from the class name.
					// @todo DB: This assumes the names of all ULevelStreaming-derived types begin with the string "LevelStreaming".
					GStreamingMethodStrings.AddItem( ClassName.Mid( 14 ) );
					GStreamingMethodClassList.AddItem( *It );
				}
			}
		}
	}


	/**
	 * Adds the named level package to the world.  Does nothing if the level already exists in the world.
	 *
	 * @param	LevelPackageBaseFilename	The base filename of the level package to add.
	 * @param	OverrideLevelStreamingClass	Unless NULL, forces a specific level streaming class type instead of prompting the user to select one
	 *
	 * @return								The new level, or NULL if the level couldn't added.
	 */
	ULevel* AddLevelToWorld(const TCHAR* LevelPackageBaseFilename, UClass* OverrideLevelStreamingClass)
	{
		ULevel* NewLevel = NULL;
		UBOOL bIsPersistentLevel = (GWorld->PersistentLevel->GetOutermost()->GetName() == FString(LevelPackageBaseFilename));

		if ( bIsPersistentLevel || FLevelUtils::FindStreamingLevel( LevelPackageBaseFilename ) )
		{
			// Do nothing if the level already exists in the world.
			appMsgf( AMT_OK, *LocalizeUnrealEd("LevelAlreadyExistsInWorld") );
		}
		else
		{
			UClass* SelectedClass = OverrideLevelStreamingClass;
			if( SelectedClass == NULL )
			{
				// Ensure the set of available level streaming methods is initialized.
				InitializeStreamingMethods();

				// Display a dialog prompting the user to choose streaming method for this level.
				const FString TitleText( FString::Printf( TEXT("%s - %s"), *LocalizeUnrealEd("SelectStreamingMethod"), LevelPackageBaseFilename ) );

				WxDlgGenericComboEntry dlg( TRUE );
				dlg.SetTitleAndCaption( *TitleText, *LocalizeUnrealEd("StreamingMethod:"), FALSE );
				dlg.PopulateComboBox( GStreamingMethodStrings );
				dlg.SetSelection( TEXT("Kismet") );

				if ( dlg.ShowModal() == wxID_OK )
				{
					// Use the selection index to look up the selected level streaming type.
					SelectedClass = GStreamingMethodClassList( dlg.GetComboBox().GetSelection() );
				}
				else
				{
					// User pressed cancel, so bail
					return NULL;
				}
			}

			const FScopedBusyCursor BusyCursor;

			ULevelStreaming* StreamingLevel = static_cast<ULevelStreaming*>( UObject::StaticConstructObject( SelectedClass, GWorld, NAME_None, 0, NULL) );

			// Associate a package name.
			StreamingLevel->PackageName = LevelPackageBaseFilename;

			// Seed the level's draw color.
			StreamingLevel->DrawColor = FColor::MakeRandomColor();

			// Add the new level to worldinfo.
			GWorld->GetWorldInfo()->StreamingLevels.AddItem( StreamingLevel );
			GWorld->GetWorldInfo()->PostEditChange();
			GWorld->MarkPackageDirty();

			NewLevel = StreamingLevel->LoadedLevel;
			LevelBrowser::SetLevelVisibility( NewLevel, TRUE );

			// Create a kismet sequence for this level if one does not already exist.
			if ( NewLevel )
			{
				USequence* ExistingSequence = NewLevel->GetGameSequence();
				if ( !ExistingSequence )
				{
					// The newly added level contains no sequence -- create a new one.
					USequence* NewSequence = ConstructObject<USequence>( USequence::StaticClass(), NewLevel, TEXT("Main_Sequence"), RF_Transactional );
					GWorld->SetGameSequence( NewSequence, NewLevel );
					NewSequence->MarkPackageDirty();

					// Fire CALLBACK_RefreshEditor_Kismet when falling out of scope.
					FScopedRefreshEditor_Kismet RefreshEditor_KismetCallback;
					RefreshEditor_KismetCallback.Request();
				}
			} 
		}

		return NewLevel;
	}

	/**
	 * Removes the specified level from the world.  Refreshes.
	 *
	 * @return	TRUE	If a level was removed.
	 */
	UBOOL RemoveLevelFromWorld(ULevel* InLevel)
	{
		// Disallow for cooked packages.
		if( GWorld && GWorld->GetOutermost()->PackageFlags & PKG_Cooked )
		{
			appMsgf( AMT_OK, *LocalizeUnrealEd("Error_OperationDisallowedOnCookedContent") );
			return FALSE;
		}

		if( !FLevelUtils::IsLevelLocked(InLevel) )
		{
			// If the level isn't locked (which means it can be removed) unselect all actors before removing the level
			// This avoids crashing in areas that rely on getting a selected actors level. The level will be invalid after its removed.
			for( INT ActorIdx = 0; ActorIdx < InLevel->Actors.Num(); ++ActorIdx )
			{
				if( InLevel->Actors( ActorIdx ) != NULL )
				{
					GEditor->SelectActor( InLevel->Actors( ActorIdx ), FALSE, NULL, FALSE );
				}
			}
			// Update the property windows in case one was referencing an actor in this level
			GUnrealEd->UpdatePropertyWindows();
		}

		const UBOOL bRemovingCurrentLevel	= InLevel && InLevel == GWorld->CurrentLevel;
		const UBOOL bRemoveSuccessful		= FLevelUtils::RemoveLevelFromWorld( InLevel );
		if ( bRemoveSuccessful )
		{
			// Collect garbage to clear out the destroyed level
			UObject::CollectGarbage( GARBAGE_COLLECTION_KEEPFLAGS );

			// If the current level was removed, make the persistent level current.
			WxLevelBrowser* LevelBrowser = GUnrealEd->GetBrowser<WxLevelBrowser>( TEXT("LevelBrowser") );
			if( bRemovingCurrentLevel )
			{
				if( ensure( LevelBrowser != NULL ) )
				{
					LevelBrowser->MakeLevelCurrent( GWorld->PersistentLevel );
				}
			}

			// Do a full level browser redraw.
			if( LevelBrowser != NULL )
			{
				LevelBrowser->RequestUpdate( TRUE );
			}

			// Redraw the main editor viewports.
			GCallbackEvent->Send( CALLBACK_RedrawAllViewports );

			// refresh editor windows
			GCallbackEvent->Send( CALLBACK_RefreshEditor_AllBrowsers );

			// refresh Kismet windows
			GCallbackEvent->Send( CALLBACK_RefreshEditor_Kismet );
		}
		return bRemoveSuccessful;
	}

	/**
	 * Creates a new streaming level.
	 *
	 * @param	bMoveSelectedActorsIntoNewLevel		If TRUE, move any selected actors into the new level.
	 * @param	DefaultFilename						Optional file name for level.  If empty, the user will be prompted during the save process.
	 * @param	OverrideLevelStreamingClass	Unless NULL, forces a specific level streaming class type instead of prompting the user to select one
	 * 
	 * @return	Returns the newly created level, or NULL on failure
	 */
	ULevel* CreateNewLevel(UBOOL bMoveSelectedActorsIntoNewLevel, const FString& DefaultFilename, UClass* OverrideLevelStreamingClass )
	{
		// Disallow for cooked packages.
		if( GWorld && GWorld->GetOutermost()->PackageFlags & PKG_Cooked )
		{
			appMsgf( AMT_OK, *LocalizeUnrealEd("Error_OperationDisallowedOnCookedContent") );
			return NULL;
		}

		WxLevelBrowser* LevelBrowser = GUnrealEd->GetBrowser<WxLevelBrowser>( TEXT("LevelBrowser") );
		LevelBrowser->DisableUpdate();

		// Editor modes cannot be active when any level saving occurs.
		GEditorModeTools().ActivateMode( EM_Default );

		// Cache the current GWorld and clear it, then allocate a new one, and finally restore it.
		UWorld* OldGWorld = GWorld;
		GWorld = NULL;
		UWorld::CreateNew();
		UWorld* NewGWorld = GWorld;
		check(NewGWorld);

		// Save the new world to disk.
		const UBOOL bNewWorldSaved = FEditorFileUtils::SaveLevel( NewGWorld->PersistentLevel, DefaultFilename );
		FString NewPackageName;
		if ( bNewWorldSaved )
		{
			NewPackageName = GWorld->GetOutermost()->GetName();
		}

		// Restore the old GWorld and GC the new one.
		GWorld = OldGWorld;

		NewGWorld->RemoveFromRoot();
		UObject::CollectGarbage( GARBAGE_COLLECTION_KEEPFLAGS );

		LevelBrowser->EnableUpdate();

		// If the new world was saved successfully, import it as a streaming level.
		if ( bNewWorldSaved )
		{
			ULevel* NewLevel = AddLevelToWorld( *NewPackageName, OverrideLevelStreamingClass );

			if ( bMoveSelectedActorsIntoNewLevel )
			{
				LevelBrowser->MakeLevelCurrent( NewLevel );

				const UBOOL bUseCurrentLevelGridVolume = FALSE;
				GEditor->MoveSelectedActorsToCurrentLevel( bUseCurrentLevelGridVolume );
			}

			return NewLevel;
		}
		// If the new world wasn't saved successfully, force an update of the level browser as it could
		// have become invalidated during the process
		else
		{
			LevelBrowser->Update();
		}
	
		return NULL;
	}



	/**
	 * Creates a new level for the specified grid cell
	 *
	 * @param	GridCell	The grid volume and cell to create a level for
	 *
	 * @return	The newly-created level, or NULL on failure
	 */
	ULevel* CreateLevelForGridCell( FGridAndCellCoordinatePair& GridCell )
	{
		// Disallow for cooked packages.
		if( GWorld && GWorld->GetOutermost()->PackageFlags & PKG_Cooked )
		{
			appMsgf( AMT_OK, *LocalizeUnrealEd("Error_OperationDisallowedOnCookedContent") );
			return NULL;
		}


		// Check to see if the level already exists for this cell
		ULevelStreaming* AlreadyExistingLevel = GridCell.GridVolume->FindLevelForGridCell( GridCell.CellCoordinate );
		if( !ensure( AlreadyExistingLevel == NULL ) )
		{
			// Level already exists.  Shouldn't be calling this function.
			return AlreadyExistingLevel->LoadedLevel;
		}


		const FString& LevelName = GridCell.GridVolume->GetLevelGridVolumeName();


		// If the grid only has a single vertical slice, then don't bother appending the Z coordinate
		// to the level file name
		FString LevelCoordinatesString;
		if( GridCell.GridVolume->Subdivisions[ 2 ] < 2 )
		{
			LevelCoordinatesString = FString::Printf(
				TEXT( "%02ix%02iy"),
				GridCell.CellCoordinate.X,
 				GridCell.CellCoordinate.Y );
		}
		else
		{
			LevelCoordinatesString = FString::Printf(
				TEXT( "%02ix%02iy%02iz"),
				GridCell.CellCoordinate.X,
 				GridCell.CellCoordinate.Y,
				GridCell.CellCoordinate.Z );
		}

		// Set a file name for the new level based on the grid volume name and cell coordinates
		const FString LevelFilename = FString::Printf(
			TEXT( "%s_%s.%s" ),
			*GWorld->GetOutermost()->GetName(), // oscar@snda
			//*LevelName,
			*LevelCoordinatesString,
			*FURL::DefaultMapExt );


		// Grab the path to the persistent level
		FString PersistentLevelPath;
		if( ensure( GPackageFileCache->FindPackageFile( *GWorld->GetOutermost()->GetName(), NULL, PersistentLevelPath ) ) )
		{
			const FString LevelPath = FFilename( PersistentLevelPath ).GetPath() * LevelFilename;

			// Create the level
			const UBOOL bMoveActorsToNewLevel = FALSE;
			ULevel* NewLevel = NULL;
			for (INT i = 0; i < GWorld->Levels.Num(); ++i)
			{
				ULevel* level = GWorld->Levels(i);
				FString lvName = level->GetOutermost()->GetName() + TEXT(".udk");
				if(lvName == LevelFilename)
				{
					NewLevel = level;
					break;
				}
			}

			if(!NewLevel)
			{
				NewLevel = CreateNewLevel( bMoveActorsToNewLevel, LevelPath, ULevelStreamingKismet::StaticClass() );
			}

			if( NewLevel != NULL )
			{
				ULevelStreaming* NewLevelStreaming = FLevelUtils::FindStreamingLevel( NewLevel ); 
				check( NewLevelStreaming != NULL );


				// Point the level streaming object to our grid volume
				NewLevelStreaming->Modify();
				NewLevelStreaming->EditorGridVolume = GridCell.GridVolume;
				NewLevelStreaming->GridPosition[ 0 ] = GridCell.CellCoordinate.X;
				NewLevelStreaming->GridPosition[ 1 ] = GridCell.CellCoordinate.Y;
				NewLevelStreaming->GridPosition[ 2 ] = GridCell.CellCoordinate.Z;

				// Update the level grid volume so the visual cue in the 3D viewport will reflect
				// that we have an actual level for this grid cell now
				{
					GridCell.GridVolume->PreEditChange( NULL );

					const UBOOL bAlwaysMarkDirty = TRUE;
					GridCell.GridVolume->Modify( bAlwaysMarkDirty );

					GridCell.GridVolume->PostEditChange();
				}

				// Success!
				return NewLevel;
			}
			else
			{
				// Level creation failed
			}
		}
		else
		{
			// Couldn't get path for persistent level.   Not saved yet?
		}

		
		// Failed to create new level
		return NULL;	
	}



	/**
	 * Given a location for the new actor, determines which level the actor would best be placed in.
	 * This uses the world's 'current' level and grid volume when selecting a level.
	 *
	 * @param	InActorLocation				Location of the actor that we're interested in finding a level for
	 * @param	bUseCurrentLevelGridVolume	True if we should try to place the actor into a level associated with the world's 'current' level grid volume, if one is set
	 *
	 * @return	The best level to place the new actor into
	 */
	ULevel* GetLevelForPlacingNewActor( const FVector& InActorLocation, const UBOOL bUseCurrentLevelGridVolume )
	{
		ULevel* DesiredLevel = GWorld->CurrentLevel;
		if( GWorld->CurrentLevelGridVolume != NULL && bUseCurrentLevelGridVolume )
		{
			// Find the best grid cell to place the actor in
			FBox LocationBox( InActorLocation, InActorLocation );
			const UBOOL bMustOverlap = FALSE;
			FLevelGridCellCoordinate BestGridCell;
			if( GWorld->CurrentLevelGridVolume->FindBestGridCellForBox( LocationBox, bMustOverlap, BestGridCell ) )
			{
				// Now find the actual level that goes with this grid cell
				ULevelStreaming* BestLevel = GWorld->CurrentLevelGridVolume->FindLevelForGridCell( BestGridCell );
				if( BestLevel == NULL )
				{
					// No level has been created for this grid cell yet.  We'll create one now.
					FGridAndCellCoordinatePair GridAndCellPair;
					GridAndCellPair.GridVolume = GWorld->CurrentLevelGridVolume;
					GridAndCellPair.CellCoordinate = BestGridCell;
					ULevel* CreatedLevel = CreateLevelForGridCell( GridAndCellPair );
					if( CreatedLevel != NULL )
					{
						BestLevel = GWorld->CurrentLevelGridVolume->FindLevelForGridCell( BestGridCell );
					}
					else
					{
						// Failed to create the level
					}
				}

				// We now have the desired level!
				if( BestLevel != NULL && BestLevel->LoadedLevel != NULL )
				{
					DesiredLevel = BestLevel->LoadedLevel;
				}
			}
		}

		return DesiredLevel;
	}

	// oscar@snda

	ULevel* GetLevelForPlacingNewActor( const FBox& LocationBox, const UBOOL bUseCurrentLevelGridVolume = TRUE )
	{
		ULevel* DesiredLevel = GWorld->CurrentLevel;
		if( GWorld->CurrentLevelGridVolume != NULL && bUseCurrentLevelGridVolume )
		{
			// Find the best grid cell to place the actor in
			const UBOOL bMustOverlap = FALSE;
			FLevelGridCellCoordinate BestGridCell;
			if( GWorld->CurrentLevelGridVolume->FindBestGridCellForBox( LocationBox, bMustOverlap, BestGridCell ) )
			//if( GWorld->CurrentLevelGridVolume->FindBestGridCellForBox( LocationBox, BestGridCell ) ) // oscar@snda
			{
				// Now find the actual level that goes with this grid cell
				ULevelStreaming* BestLevel = GWorld->CurrentLevelGridVolume->FindLevelForGridCell( BestGridCell );
				if( BestLevel == NULL )
				{
					// No level has been created for this grid cell yet.  We'll create one now.
					FGridAndCellCoordinatePair GridAndCellPair;
					GridAndCellPair.GridVolume = GWorld->CurrentLevelGridVolume;
					GridAndCellPair.CellCoordinate = BestGridCell;
					ULevel* CreatedLevel = CreateLevelForGridCell( GridAndCellPair );
					if( CreatedLevel != NULL )
					{
						BestLevel = GWorld->CurrentLevelGridVolume->FindLevelForGridCell( BestGridCell );
					}
					else
					{
						// Failed to create the level
					}
				}

				// We now have the desired level!
				if( BestLevel != NULL && BestLevel->LoadedLevel != NULL )
				{
					DesiredLevel = BestLevel->LoadedLevel;
				}
			}
		}

		return DesiredLevel;
	}

	FBox CalcAABB(const TArray< TArray<AActor*> >& actorsArr, const TArray<ALandscape*>& landscape)
	{
		FBox box(0);
		for(INT ii = 0; ii < actorsArr.Num(); ++ii)
		{
			const TArray<AActor*>& actors = actorsArr(ii);

			if(actors.Num() > 1)
			{
				box = actors(0)->GetComponentsBoundingBox(TRUE);
			}

			for (INT i = 1; i < actors.Num(); ++i)
			{
				AActor* actor = actors(i);
				FBox actorBox = actor->GetComponentsBoundingBox(TRUE);

				if(actorBox.Min.Z < -GWorld->GetWorldInfo()->LevelGridSize)
				{
					//appDebugBreak();
					GWarn->MapCheck_Add(MCTYPE_ERROR, actor, TEXT("This collision mesh appears to be in negative Z space and more than one level grid cell away."), NULL);
				}
				else
				{
					GWarn->MapCheck_Add(MCTYPE_WARNING, actor, TEXT("This collision mesh appears to be in negative Z space. If it is not a big problem it can be ignored."), NULL, MCGROUP_KISMET);
				}

				actorBox.Min.Z = Max(0.0f, actorBox.Min.Z);
				actorBox.Min.X = Max(0.0f, actorBox.Min.X);
				actorBox.Min.Y = Max(0.0f, actorBox.Min.Y);

				box += actorBox;
			}
		}

		for (INT j = 0; j < landscape.Num(); ++j)
		{
			ALandscape* land = landscape(j);
			FBox actorBox = land->GetComponentsBoundingBox(TRUE);
			box += actorBox;
		}

		return box;
	}

	FIntPoint ConvertToTileCoord(const FVector& vect, INT tileSize)
	{
		FIntPoint pt;
		pt.X = appCeil(vect.X / tileSize);
		pt.Y = appCeil(vect.Y / tileSize);
		return pt;
	}

	void FindActorsToSplit( TArray<AActor*> &actorsToSplit, TArray<ALandscape*> &landscapes, TArray< TArray<AActor*> >* actorsByLevel )
	{
		FString ExlcudeMoveToGridLayer;
		GConfig->GetString(TEXT("Configuration"), TEXT("ExlcudeMoveToGridLayer"), ExlcudeMoveToGridLayer, GEditorIni);
		ExlcudeMoveToGridLayer = ExlcudeMoveToGridLayer.ToUpper();

		for (INT i = 0; i < GWorld->Levels.Num(); ++i)
		{
			ULevel* level = GWorld->Levels(i);
			ULevelStreaming* lvs = FLevelUtils::FindStreamingLevel( level );
			if ( lvs && lvs->DisableAutoSplit)
			{
				// Skip levels that do not need splitting
				continue;
			}

			TArray<AActor*> levelActors;

			for (INT j = 0; j < level->Actors.Num(); ++j)
			{
				AActor* actor = level->Actors(j);
				if ( actor )
				{
					if(lvs && lvs->EditorGridVolume && lvs->EditorGridVolume->IsActorMemberOfGrid( actor ) )
					{
						continue;
					}

					FString actorLayer = actor->Layer.ToString();
					actorLayer = actorLayer.ToUpper();
					if(actor->IsReferencedByKismet())
					{
						continue;
					}

					if(actorLayer.InStr(ExlcudeMoveToGridLayer) != INDEX_NONE)
					{
						continue;
					}

					if(actor->IsA(AStaticMeshActor::StaticClass())
						|| actor->IsA(APointLight::StaticClass())
						|| actor->IsA(APointLightMovable::StaticClass())
						|| actor->IsA(APointLightToggleable::StaticClass())
						|| actor->IsA(ASkeletalMeshActor::StaticClass()))
					{
						levelActors.AddItem(actor);
					}
// 					else if(actor->IsA(AEmitter::StaticClass()))
// 					{
// 						AEmitter* par = Cast<AEmitter>(actor);
// 						if(par && par->ParticleSystemComponent->Template)
// 						{
// 							TArrayNoInit<UParticleEmitter*>& emts = par->ParticleSystemComponent->Template->Emitters;
// 							for (INT k = 0; k < emts.Num(); ++k )
// 							{
// 								for (INT m = 0; m < emts(k)->LODLevels.Num(); ++m)
// 								{
// 									UParticleModuleRequired* pmr = emts(k)->LODLevels(m)->RequiredModule;
// 									if(pmr && pmr->bIsRain)
// 									{
// 										continue;
// 									}
// 								}
// 							}
// 						}
// 
// 						actorsToSplit.AddItem(actor);
// 					}
					else if(actor->IsA(ALandscape::StaticClass()))
					{
						landscapes.AddItem(Cast<ALandscape>(actor));
					}
				}
			} // for actors

			if(actorsByLevel)
			{
				actorsByLevel->AddItem(levelActors);
			}
			else
			{
				actorsToSplit += levelActors;
			}
		} // for level
	}


#if 0
	void InitTileCoord( ULevelStreamingDistance* distLvs, INT x, INT y, INT tileSize )
	{
		distLvs->Modify();
		distLvs->Origin.X = (x + 1) * tileSize - tileSize / 2;
		distLvs->Origin.Y = (x + 1) * tileSize - tileSize / 2;
		distLvs->Origin.Z = 0.0f;
		distLvs->TileOrig.X = x;
		distLvs->TileOrig.Y = y;
		distLvs->MaxDistance = tileSize + tileSize / 2;
		distLvs->MarkPackageDirty();
		distLvs->LoadedLevel->MarkPackageDirty();
	}



	ULevel* FindOrCreateLevel(INT x, INT y, INT tileSize)
	{
		ULevelStreaming* MatchingLevel = NULL;

		AWorldInfo*	WorldInfo = GWorld->GetWorldInfo();
		for( INT LevelIndex = 0 ; LevelIndex < WorldInfo->StreamingLevels.Num() ; ++LevelIndex )
		{
			ULevelStreaming* CurStreamingLevel = WorldInfo->StreamingLevels( LevelIndex );
			if( CurStreamingLevel && (CurStreamingLevel->TileOrig.X == x) && (CurStreamingLevel->TileOrig.Y == y))
			{
				ULevelStreamingDistance* distLvs = Cast<ULevelStreamingDistance>(CurStreamingLevel);
				if(distLvs)
				{
					InitTileCoord(distLvs, x, y, tileSize);
				}

				return CurStreamingLevel->LoadedLevel;
			}
		}

		FString mapPath;
		GPackageFileCache->FindPackageFile(*GWorld->GetMapName(), NULL, mapPath);
		FFilename mapName(mapPath);
		
		// create a new level		
		FString lvName = FString::Printf(TEXT("%s\\%s_%d_%d.udk"), *mapName.GetPath(), *GWorld->GetMapName(), x, y);
		ULevel* lv = EditorLevelUtils::CreateNewLevel(FALSE, lvName, ULevelStreamingDistance::StaticClass());
		
		ULevelStreaming* lvs = FLevelUtils::FindStreamingLevel(lv);
		ULevelStreamingDistance* distLvs = Cast<ULevelStreamingDistance>(lvs);
		if(distLvs)
		{
			InitTileCoord(distLvs, x, y, tileSize);
		}
		else
		{
			appMsgf(AMT_OK, TEXT("Not a valid distance-based dynamic loading sublevel type: %s"), *lvName);
		}

		return lv;
	}
#endif

	typedef std::map<INT, TLookupMap<AActor*> > TileYDataMap;
	typedef std::map<INT,  TileYDataMap> TileXYDataMap;

	TileXYDataMap g_LevelTiles;

	typedef std::map<ULevelStreaming*, TLookupMap<AActor*> > LevelActorMap;

	LevelActorMap g_levelActors;

	void TryAddToMap(AActor* actor, INT tileSize)
	{
		FIntPoint pt = ConvertToTileCoord(actor->Location, tileSize);
		TileXYDataMap::iterator ix = g_LevelTiles.find(pt.X);
		if(ix == g_LevelTiles.end())
		{
			ix = g_LevelTiles.insert(std::make_pair(pt.X, TileYDataMap())).first;
		}

		TileYDataMap::iterator iy = ix->second.find(pt.Y);
		if(iy == ix->second.end())
		{
			iy = ix->second.insert(std::make_pair(pt.Y, TLookupMap<AActor*>())).first;
		}

		iy->second.AddItem(actor);
	}

	void TryAddToLvMap(AActor* actor)
	{
		ULevel* lv = GetLevelForPlacingNewActor(actor->Location);
		check(lv);

		debugf( NAME_DevGarbage, TEXT("GC -2.TryAddToLvMap: %s %x -> %s"), *actor->GetPathName(),
#ifdef _WIN64
			(UINT64)actor
#else
			(UINT)actor
#endif
			, *lv->GetOutermost()->GetName()
			);  // oscar@snda

		if(lv == actor->GetLevel())
		{
			debugf( NAME_DevGarbage, TEXT("GC -2.TryAddToLvMap: Skip %s"), *actor->GetPathName() );
			return;
		}

		ULevelStreaming* streaming = FLevelUtils::FindStreamingLevel(lv);
		if(streaming)
		{
			LevelActorMap::iterator it = g_levelActors.find(streaming);
			if(it == g_levelActors.end())
			{
				it = g_levelActors.insert( std::make_pair( streaming, TLookupMap<AActor*>() )).first;
			}

			it->second.AddItem(actor);
		}
		else
		{
			warnf(NAME_Error, TEXT("Can't add actor:%s to grid!"), *actor->GetPathName());
		}
	}

	template<class T>
	T* TCreateVolume(const FBox& box)
	{
		UCubeBuilder* CubeBuilder = ConstructObject<UCubeBuilder>( UCubeBuilder::StaticClass() );
		CubeBuilder->X = box.GetExtent().X * 2; 
		CubeBuilder->Y = box.GetExtent().Y * 2;
		CubeBuilder->Z = box.GetExtent().Z * 2;
		CubeBuilder->eventBuild();

		T* 	vol = (T*)GWorld->SpawnActor(T::StaticClass(),NAME_None,GWorld->GetBrush()->Location);
		if( vol )
		{
			vol->PreEditChange(NULL);

			FBSPOps::csgCopyBrush
				(
				vol,
				GWorld->GetBrush(),
				0,
				RF_Transactional,
				1,
				TRUE
				);

			vol->Modify();
			vol->SetLocation( box.GetCenter() );
			vol->PostEditChange();
		}

		UCubeBuilder* CubeBuilderOriginal = ConstructObject<UCubeBuilder>( UCubeBuilder::StaticClass() );
		CubeBuilderOriginal->X = CubeBuilderOriginal->Y = CubeBuilderOriginal->Z = 5;
		CubeBuilderOriginal->eventBuild();

		return vol;
	}

	void RenameLandscapeRes(ALandscape* Landscape)
	{
		FString mapPath;
		GPackageFileCache->FindPackageFile(*GWorld->GetMapName(), NULL, mapPath);
		FFilename mapName(mapPath);
		FString sharePkgPath = mapName.GetPath();
		
		Landscape->Modify();
		if(Landscape->GetLandscapeInfo())
		{
			Landscape->GetLandscapeInfo()->Modify();
		}

		TArray<UObject*> RenameObjects;
		//FString MsgBoxList;

		// Check the Physical Material is same package with Landscape
		if (Landscape->DefaultPhysMaterial && Landscape->DefaultPhysMaterial->GetOutermost() == Landscape->GetOutermost())
		{
			//appMsgf(AMT_OK, *LocalizeUnrealEd("LandscapePhyMaterial_Warning"));
			RenameObjects.AddUniqueItem(Landscape->DefaultPhysMaterial);
			//MsgBoxList += Landscape->DefaultPhysMaterial->GetPathName();
			//MsgBoxList += FString::Printf(TEXT("\n"));
		}

		// Check the LayerInfoObjects are same package with Landscape
		for (int i = 0; i < Landscape->LayerInfoObjs.Num(); ++i)
		{
			ULandscapeLayerInfoObject* LayerInfo = Landscape->LayerInfoObjs(i).LayerInfoObj;
			if (LayerInfo && LayerInfo->GetOutermost() == Landscape->GetOutermost())
			{
				RenameObjects.AddUniqueItem(LayerInfo);
				//MsgBoxList += LayerInfo->GetPathName();
				//MsgBoxList += FString::Printf(TEXT("\n"));
			}
		}

		for (TArray<ULandscapeComponent*>::TIterator It(Landscape->LandscapeComponents); It; ++It)
		{
			UMaterialInterface* LandscapeMaterial = (*It)->GetLandscapeMaterial();
			if ( LandscapeMaterial && LandscapeMaterial->GetOutermost() == (*It)->GetOutermost() )
			{
				ULandscapeComponent* Comp = *It;
				RenameObjects.AddUniqueItem(LandscapeMaterial);
			}
		}
// 		UMaterialInterface* LandscapeMaterial = MoveComponent->GetLandscapeMaterial();
// 		if ( LandscapeMaterial && LandscapeMaterial->GetOutermost() == MoveComponent->GetOutermost() )
// 		{
// 			RenameObjects.AddUniqueItem(LandscapeMaterial);
// 		}

		// sharePkgPath + TEXT("\\") + 
		ObjectTools::g_defaultMoveToPackageName = GWorld->PersistentLevel->GetOutermost()->GetName() + TEXT("_grid");

		UBOOL bSucceed =  ObjectTools::RenameObjectsWithRefs( RenameObjects, FALSE, NULL );

		ObjectTools::g_defaultMoveToPackageName.Empty();
	}

	void MoveLandscapeComponentToLevels(ALandscape* Landscape, ULandscapeComponent* MoveComponent, ULevel* DestLv)
	{
		if( !Landscape )
		{
			return;
		}

		if(MoveComponent->GetLandscapeProxy()->GetLevel() == DestLv)
		{
			return;
		}

		ULandscapeHeightfieldCollisionComponent* ColComp = MoveComponent->GetCollisionComponent();

		TSet<ALandscapeProxy*> SelectProxies;
		TSet<UTexture2D*> OldTextureSet;
		TSet<ULandscapeComponent*> TargetSelectedComponents;
		TArray<ULandscapeHeightfieldCollisionComponent*> TargetSelectedCollisionComponents;
		
		TSet<ULandscapeComponent*> HeightmapUpdateComponents;

		INT ComponentSizeVerts = Landscape->NumSubsections * (Landscape->SubsectionSizeQuads+1);		
		INT NeedHeightmapSize = 1<<appCeilLogTwo( ComponentSizeVerts );

		
		SelectProxies.Add(MoveComponent->GetLandscapeProxy());
		if (MoveComponent->GetLandscapeProxy()->GetOuter() != DestLv)
		{
			TargetSelectedComponents.Add(MoveComponent);
		}

		if (ColComp->GetLandscapeProxy()->GetOuter() != DestLv)
		{
			TargetSelectedCollisionComponents.AddItem(ColComp);
		}

		for(TSet<ULandscapeComponent*>::TConstIterator It(TargetSelectedComponents);It;++It)
		{
			ULandscapeComponent* Comp = *It;
			Comp->Modify();				
			OldTextureSet.Add(Comp->HeightmapTexture);
		}

		INT Progress = 0;
		INT TotalProgress = TargetSelectedComponents.Num() * TargetSelectedCollisionComponents.Num();

		// Need to split all the component which share Heightmap with selected components
		// Search neighbor only
		for(TSet<ULandscapeComponent*>::TConstIterator It(TargetSelectedComponents);It;++It)
		{
			ULandscapeComponent* Comp = *It;
			INT SearchX = Comp->HeightmapTexture->Mips(0).SizeX / NeedHeightmapSize;
			INT SearchY = Comp->HeightmapTexture->Mips(0).SizeY / NeedHeightmapSize;

			for (INT Y = 0; Y < SearchY; ++Y)
			{
				for (INT X = 0; X < SearchX; ++X)
				{
					// Search for four directions...
					for (INT Dir = 0; Dir < 4; ++Dir)
					{
						INT XDir = (Dir>>1) ? 1 : -1;
						INT YDir = (Dir%2) ? 1 : -1;
						ULandscapeComponent* Neighbor =  Comp->GetLandscapeInfo()->XYtoComponentMap.FindRef(ALandscape::MakeKey(Comp->SectionBaseX + XDir*X*Comp->ComponentSizeQuads, Comp->SectionBaseY + YDir*Y*Comp->ComponentSizeQuads));
						if (Neighbor && Neighbor->HeightmapTexture == Comp->HeightmapTexture && !HeightmapUpdateComponents.Contains(Neighbor))
						{
							Neighbor->Modify();
							if (!TargetSelectedComponents.Contains(Neighbor))
							{
								Neighbor->HeightmapScaleBias.X = -1.f; // just mark this component is for original level, not current level
							}
							HeightmapUpdateComponents.Add(Neighbor);
						}
					}
				}
			}
		}

		// Changing Heightmap format for selected components
		for(TSet<ULandscapeComponent*>::TConstIterator It(HeightmapUpdateComponents);It;++It)
		{
			ULandscapeComponent* Comp = *It;
			ALandscape::SplitHeightmap(Comp, (Comp->HeightmapScaleBias.X > 0.f));
		}

		// Delete if it is no referenced textures...
		for(TSet<UTexture2D*>::TIterator It(OldTextureSet);It;++It)
		{
			(*It)->SetFlags(RF_Transactional);
			(*It)->Modify();
			(*It)->MarkPackageDirty();
			(*It)->ClearFlags(RF_Standalone);
		}

		ALandscapeProxy* LandscapeProxy = NULL;
		// Find there is already a LandscapeProxy Actor
		for (FActorIterator It; It; ++It)
		{
			ALandscapeProxy* Proxy = Cast<ALandscapeProxy>(*It);
			if (Proxy && Proxy->GetOuter() == DestLv )
			{
				if ((Proxy->bIsProxy && Proxy->IsValidLandscapeActor(Landscape)) || (Proxy == Landscape))
				{
					LandscapeProxy = Proxy;
					break;
				}
			}
		}

		ULevel* tmpLv = GWorld->CurrentLevel;
		GWorld->CurrentLevel = DestLv;

		if (!LandscapeProxy)
		{
			LandscapeProxy = Cast<ALandscapeProxy>( GWorld->SpawnActor( ALandscapeProxy::StaticClass() ) );
			//, NAME_None, FVector(0,0,0),FRotator(0,0,0), NULL, 0, FALSE, NULL, NULL, FALSE, DestLv) );
		}

		GWorld->CurrentLevel = tmpLv;

		for(TSet<ALandscapeProxy*>::TIterator It(SelectProxies);It;++It)
		{
			(*It)->Modify();
		}

		LandscapeProxy->Modify();
		LandscapeProxy->MarkPackageDirty();
		if (LandscapeProxy->bIsProxy)
		{
			LandscapeProxy->UpdateLandscapeActor(Landscape);
		}

		// Change Weight maps...
		{
			FLandscapeEditDataInterface LandscapeEdit(ColComp->GetLandscapeInfo());
			for(TSet<ULandscapeComponent*>::TConstIterator It(TargetSelectedComponents);It;++It)
			{
				ULandscapeComponent* Comp = *It;
				INT TotalNeededChannels = Comp->WeightmapLayerAllocations.Num();
				INT CurrentLayer = 0;
				TArray<UTexture2D*> NewWeightmapTextures;

				// Code from ULandscapeComponent::ReallocateWeightmaps
				// Move to other channels left
				while( TotalNeededChannels > 0 )
				{
					// debugf(TEXT("Still need %d channels"), TotalNeededChannels);

					UTexture2D* CurrentWeightmapTexture = NULL;
					FLandscapeWeightmapUsage* CurrentWeightmapUsage = NULL;

					// [ming@snda
#define PACK_WEIGHTMAPS 1

#if PACK_WEIGHTMAPS
					if( TotalNeededChannels < 8 )
#else
					if( TotalNeededChannels < 4 )
#endif
					// ming@snda]
					{
						// debugf(TEXT("Looking for nearest"));

						// see if we can find a suitable existing weightmap texture with sufficient channels
						INT BestDistanceSquared = MAXINT;
						for( TMap<UTexture2D*,struct FLandscapeWeightmapUsage>::TIterator It(LandscapeProxy->WeightmapUsageMap); It; ++It )
						{
							FLandscapeWeightmapUsage* TryWeightmapUsage = &It.Value();
							// [ming@snda
#if PACK_WEIGHTMAPS
							if( TryWeightmapUsage->FreeChannelCountTotal() >= TotalNeededChannels )
							{
								for( INT ChanIdx = 0; ChanIdx < 8; ++ChanIdx )
								{
									if( TryWeightmapUsage->GetChannelUsage(ChanIdx) != NULL )
									{
										INT TryDistanceSquared = 
											Square(TryWeightmapUsage->GetChannelUsage(ChanIdx)->SectionBaseX - Comp->SectionBaseX) + Square(TryWeightmapUsage->GetChannelUsage(ChanIdx)->SectionBaseX - Comp->SectionBaseX);
#else
							if( TryWeightmapUsage->FreeChannelCount() >= TotalNeededChannels )
							{
								// See if this candidate is closer than any others we've found
								for( INT ChanIdx=0;ChanIdx<4;ChanIdx++ )
								{
									if( TryWeightmapUsage->ChannelUsage[ChanIdx] != NULL  )
									{
										INT TryDistanceSquared = Square(TryWeightmapUsage->ChannelUsage[ChanIdx]->SectionBaseX - Comp->SectionBaseX) + Square(TryWeightmapUsage->ChannelUsage[ChanIdx]->SectionBaseX - Comp->SectionBaseX);
#endif
							// ming@snda]
										if( TryDistanceSquared < BestDistanceSquared )
										{
											CurrentWeightmapTexture = It.Key();
											CurrentWeightmapUsage = TryWeightmapUsage;
											BestDistanceSquared = TryDistanceSquared;
										}
									}
								}
							}
						}
					}

					UBOOL NeedsUpdateResource=FALSE;
					// No suitable weightmap texture
					if( CurrentWeightmapTexture == NULL )
					{
						Comp->MarkPackageDirty();

						// Weightmap is sized the same as the component
						INT WeightmapSize = (Comp->SubsectionSizeQuads+1) * Comp->NumSubsections;

						// We need a new weightmap texture
						CurrentWeightmapTexture = ConstructObject<UTexture2D>(UTexture2D::StaticClass(), DestLv->GetOutermost(), NAME_None, RF_Public);
						// [ming@snda
#if PACK_WEIGHTMAPS
						CurrentWeightmapTexture->Init(WeightmapSize,WeightmapSize*2,PF_A8R8G8B8);
#else
						CurrentWeightmapTexture->Init(WeightmapSize,WeightmapSize,PF_A8R8G8B8);
#endif
						// ming@snda]
						CurrentWeightmapTexture->SRGB = FALSE;
						CurrentWeightmapTexture->CompressionNone = TRUE;
						CurrentWeightmapTexture->MipGenSettings = TMGS_LeaveExistingMips;
						CurrentWeightmapTexture->AddressX = TA_Clamp;
						CurrentWeightmapTexture->AddressY = TA_Clamp;
						CurrentWeightmapTexture->LODGroup = TEXTUREGROUP_Terrain_Weightmap;
						// Alloc dummy mips
						Comp->CreateEmptyTextureMips(CurrentWeightmapTexture);
						CurrentWeightmapTexture->UpdateResource();

						// Store it in the usage map
						CurrentWeightmapUsage = &LandscapeProxy->WeightmapUsageMap.Set(CurrentWeightmapTexture, FLandscapeWeightmapUsage());

						// debugf(TEXT("Making a new texture %s"), *CurrentWeightmapTexture->GetName());
					}

					NewWeightmapTextures.AddItem(CurrentWeightmapTexture);

					// [ming@snda
#if PACK_WEIGHTMAPS
					for( INT ChanIdx=0; ChanIdx < 8 && TotalNeededChannels > 0; ++ChanIdx )
					{
						if( CurrentWeightmapUsage->GetChannelUsage(ChanIdx) == NULL )
						{
#else
					for( INT ChanIdx=0;ChanIdx<4 && TotalNeededChannels > 0;ChanIdx++ )
					{
						// debugf(TEXT("Finding allocation for layer %d"), CurrentLayer);

						if( CurrentWeightmapUsage->ChannelUsage[ChanIdx] == NULL  )
						{
#endif
					// ming@snda]
							// Use this allocation
							FWeightmapLayerAllocationInfo& AllocInfo = Comp->WeightmapLayerAllocations(CurrentLayer);

							if( AllocInfo.WeightmapTextureIndex == 255 )
							{
								// New layer - zero out the data for this texture channel
								LandscapeEdit.ZeroTextureChannel( CurrentWeightmapTexture, ChanIdx );
							}
							else
							{
								UTexture2D* OldWeightmapTexture = Comp->WeightmapTextures(AllocInfo.WeightmapTextureIndex);

								// Copy the data
								LandscapeEdit.CopyTextureChannel( CurrentWeightmapTexture, ChanIdx, OldWeightmapTexture, AllocInfo.WeightmapTextureChannel );
								LandscapeEdit.ZeroTextureChannel( OldWeightmapTexture, AllocInfo.WeightmapTextureChannel );

								// Remove the old allocation
								FLandscapeWeightmapUsage* OldWeightmapUsage = Comp->GetLandscapeProxy()->WeightmapUsageMap.Find(OldWeightmapTexture);
								// [ming@snda
#if PACK_WEIGHTMAPS
								OldWeightmapUsage->SetChannelUsage( AllocInfo.WeightmapTextureChannel, NULL );
#else
								OldWeightmapUsage->ChannelUsage[AllocInfo.WeightmapTextureChannel] = NULL;
#endif
								// ming@snda]
							}

							// Assign the new allocation
							// [ming@snda
#if PACK_WEIGHTMAPS
							CurrentWeightmapUsage->SetChannelUsage( ChanIdx, Comp );
#else
							CurrentWeightmapUsage->ChannelUsage[ChanIdx] = Comp;
#endif
							// ming@snda]
							AllocInfo.WeightmapTextureIndex = NewWeightmapTextures.Num()-1;
							AllocInfo.WeightmapTextureChannel = ChanIdx;
							CurrentLayer++;
							TotalNeededChannels--;
						}
					}
				}

				// Replace the weightmap textures
				Comp->WeightmapTextures = NewWeightmapTextures;

				// Update the mipmaps for the textures we edited
				for( INT Idx=0;Idx<Comp->WeightmapTextures.Num();Idx++)
				{
					UTexture2D* WeightmapTexture = Comp->WeightmapTextures(Idx);
					FLandscapeTextureDataInfo* WeightmapDataInfo = LandscapeEdit.GetTextureDataInfo(WeightmapTexture);

					INT NumMips = WeightmapTexture->Mips.Num();
					TArray<FColor*> WeightmapTextureMipData(NumMips);
					for( INT MipIdx=0;MipIdx<NumMips;MipIdx++ )
					{
						WeightmapTextureMipData(MipIdx) = (FColor*)WeightmapDataInfo->GetMipData(MipIdx);
					}

					ULandscapeComponent::UpdateWeightmapMips(Comp->NumSubsections, Comp->SubsectionSizeQuads, WeightmapTexture, WeightmapTextureMipData, 0, 0, MAXINT, MAXINT, WeightmapDataInfo);
				}
			}
			// Need to Repacking all the Weight map (to make it packed well...)
			Landscape->RemoveInvalidWeightmaps();
		}


		// Move the components to the Proxy actor
		// This does not use the MoveSelectedActorsToCurrentLevel path as there is no support to only move certain components.
		for(TSet<ULandscapeComponent*>::TConstIterator It(TargetSelectedComponents);It;++It)
		{
			// Need to move or recreate all related data (Height map, Weight map, maybe collision components, allocation info)
			ULandscapeComponent* Comp = *It;
			Comp->GetLandscapeProxy()->LandscapeComponents.RemoveItem(Comp);
			Comp->InvalidateLightingCache();
			Comp->Rename(NULL, LandscapeProxy);
			LandscapeProxy->LandscapeComponents.AddItem( Comp );
			Comp->UpdateMaterialInstances();	
			Comp->ConditionalDetach();

			//GWarn->StatusUpdatef( Progress++, TotalProgress, *FString::Printf( TEXT("Moving Component : %s"), *Comp->GetName() ) );
		}

		for(TArray<ULandscapeHeightfieldCollisionComponent*>::TConstIterator It(TargetSelectedCollisionComponents);It;++It)
		{
			// Need to move or recreate all related data (Height map, Weight map, maybe collision components, allocation info)
			ULandscapeHeightfieldCollisionComponent* Comp = *It;

			// Move any foliage associated
			AInstancedFoliageActor* OldIFA = AInstancedFoliageActor::GetInstancedFoliageActorForLevel( Comp->GetLandscapeProxy()->GetLevel() );
			if (OldIFA)
			{
				OldIFA->MoveInstancesForComponentToCurrentLevel(Comp);
			}

			Comp->GetLandscapeProxy()->CollisionComponents.RemoveItem(*It);
			Comp->Rename(NULL, LandscapeProxy);
			LandscapeProxy->CollisionComponents.AddItem(*It);
			Comp->ConditionalDetach();

			//GWarn->StatusUpdatef( Progress++, TotalProgress, *FString::Printf( TEXT("Moving Component : %s"), *Comp->GetName() ) );
		}

		GEditor->SelectNone(FALSE, TRUE);
		GEditor->SelectActor( LandscapeProxy, TRUE, NULL, FALSE, TRUE );
		const UBOOL bUseCurrentLevelGridVolume = FALSE;

		GEditor->SelectNone(FALSE, TRUE);

		// Update our new components
		LandscapeProxy->ConditionalUpdateComponents();
		// Init RB physics for editor collision
		LandscapeProxy->InitRBPhysEditor();

		for(TSet<ALandscapeProxy*>::TIterator It(SelectProxies);It;++It)
		{
			(*It)->ConditionalUpdateComponents();
			(*It)->InitRBPhysEditor();
		}

		Landscape->bLockLocation = (Landscape->GetLandscapeInfo()->XYtoComponentMap.Num() != Landscape->LandscapeComponents.Num());

		if (Landscape != LandscapeProxy)
		{
			for (int i = 0; i < Landscape->LayerInfoObjs.Num(); ++i )
			{
				LandscapeProxy->LayerInfoObjs.AddItem(FLandscapeLayerStruct(Landscape->LayerInfoObjs(i).LayerInfoObj, LandscapeProxy, NULL));
			}
		}
		// ming@snda
#undef PACK_WEIGHTMAPS
	}

	void MergeLandScapeComponentToLevel( ULandscapeInfo* pInfo, ULevel* lv )
	{
		FLevelUtils::g_enableSelectWarn = false;
		//FSourceControl::DisableSCC = true;
		FSourceControl::BeginBatchAddToCheckList();

		g_LevelTiles.clear();
		g_levelActors.clear();

		ALevelGridVolume* vol =  GWorld->CurrentLevelGridVolume;
		if( GWorld->CurrentLevelGridVolume == NULL )
		{
			for( FActorIterator ActorIt; ActorIt; ++ActorIt )
			{
				vol = Cast< ALevelGridVolume >( *ActorIt );
				if(vol)
				{
					break;
				}
			}
		}
		//GWarn->StatusUpdatef(-1, -1, TEXT("Collecting static mesh and terrain list..."));
		warnf(TEXT("=============Merge LandScape==== Begin Merge ===================="));
		int i = 0; 
		int num = pInfo->XYtoComponentMap.Num();
		for (TMap<QWORD, ULandscapeComponent*>::TIterator It(pInfo->XYtoComponentMap); It; ++It)
		{
			++i;
			ULandscapeComponent* Comp = It.Value();
			ULevel* curLevel  = Comp->GetLandscapeProxy()->GetLevel();
			if ( curLevel == lv )
				continue;
			ALandscape *curLandScapeActor = Comp->GetLandscapeActor();
			warnf(TEXT("Merge Component : %s"), *Comp->GetName());
			//GWarn->EnableUpdateUI = true;
			//GWarn->StatusUpdatef( i, num,
				//*FString::Printf( TEXT("Merge Component : %s"), *Comp->GetName() ) );
			//GWarn->EnableUpdateUI = false;
			MoveLandscapeComponentToLevels(curLandScapeActor, Comp, lv);
		}
		warnf(TEXT("=============Merge LandScape==== complete Merge ===================="));
		AWorldInfo* WorldInfo = GWorld->GetWorldInfo();
		for( INT LevelIndex = 0 ; LevelIndex < WorldInfo->StreamingLevels.Num() ; ++LevelIndex )
		{
			ULevelStreaming* LevelStreamingObject = WorldInfo->StreamingLevels(LevelIndex);
			if( LevelStreamingObject )
			{
				if(LevelStreamingObject->GridPosition[0] > vol->Subdivisions[0]
				|| LevelStreamingObject->GridPosition[1] > vol->Subdivisions[1]
				|| LevelStreamingObject->GridPosition[2] > vol->Subdivisions[2])
				{
					FString lvName = LevelStreamingObject->LoadedLevel ? LevelStreamingObject->LoadedLevel->GetName() : TEXT("");
					GWarn->MapCheck_Add(MCTYPE_ERROR, LevelStreamingObject,
						*FString::Printf(TEXT("Invalid grid level coordinates: %s - %s %d,%d,%d"), *LevelStreamingObject->GetName(), *lvName,
						LevelStreamingObject->GridPosition[0], 
						LevelStreamingObject->GridPosition[1],
						LevelStreamingObject->GridPosition[2]), NULL);
				}
			}
		}


		g_LevelTiles.clear();
		g_levelActors.clear();

		warnf(TEXT("=============Merge LandScape==== Begin UpdateActor===================="));
		FLOAT startLogTime = appSeconds();
		UpdateLevelsForAllActors();
		FLOAT endLogTime = appSeconds();

		warnf(TEXT("=============Merge LandScape==== End UpdateActor====================%.2f"), startLogTime - endLogTime);
		FLevelUtils::g_enableSelectWarn = true;

		GWarn->EnableUpdateUI = true;

		FSourceControl::EndBatchAddToCheckList();
	}


	void AutoSplitLevels( INT tileSize )
	{
		//GWarn->BeginSlowTask(TEXT("Auto split scene"), TRUE);

		// oscar@snda
		FLevelUtils::g_enableSelectWarn = false;
		//FSourceControl::DisableSCC = true;
		FSourceControl::BeginBatchAddToCheckList();

		g_LevelTiles.clear();
		g_levelActors.clear();

		TArray< TArray<AActor*> > actorsToSplitArr;
		TArray<AActor*> actorsToSplitTmp;
		TArray<ALandscape*> landscapes;

		GEditor->SelectNone(TRUE, TRUE, FALSE);
		//GEditor->BeginTransaction(TEXT("Tile"));

		GWarn->StatusUpdatef(-1, -1, TEXT("Collecting static mesh and terrain lists..."));
		FindActorsToSplit(actorsToSplitTmp, landscapes, &actorsToSplitArr);

		//INT startActorsCount = actorsToSplit.Num();

		GWarn->StatusUpdatef(-1, -1, TEXT("Calculating bounds..."));

		FBox worldBox = CalcAABB(actorsToSplitArr, landscapes);

		INT halfTileSz = GWorld->GetWorldInfo()->LevelGridSize / 2;

//   		if( GWorld->CurrentLevelGridVolume != NULL )
//   		{
//   			GWorld->CurrentLevelGridVolume = NULL;
//   			GCallbackEvent->Send( CALLBACK_NewCurrentLevel );
//   		}
 		GWorld->CurrentLevel = GWorld->PersistentLevel;
 		GCallbackEvent->Send( CALLBACK_NewCurrentLevel );
 		GEditor->SelectNone(TRUE, TRUE, FALSE);
 		
  		ALevelGridVolume* vol = NULL;
		
#if defined(_DEBUG) || 1
		if(appMsgf(AMT_YesNo, TEXT("Create grid volume?")))
#endif
		{
			vol = TCreateVolume<ALevelGridVolume>(worldBox);
  			vol->Subdivisions[0] = appCeil(worldBox.GetExtent().X / halfTileSz);
  			vol->Subdivisions[1] = appCeil(worldBox.GetExtent().Y / halfTileSz);
  			vol->Subdivisions[2] = appCeil(worldBox.GetExtent().Z / halfTileSz);
  			vol->KeepLoadedRange = tileSize + tileSize / 2;
  			vol->LoadingDistance = tileSize + tileSize / 2;
		}
		else if( GWorld->CurrentLevelGridVolume == NULL )
		{
			for( FActorIterator ActorIt; ActorIt; ++ActorIt )
			{
				vol = Cast< ALevelGridVolume >( *ActorIt );
				if(vol)
				{
					break;
				}
			}
		}
		
		if(vol)
		{
			GWorld->CurrentLevelGridVolume = vol;
			GCallbackEvent->Send( CALLBACK_NewCurrentLevel );
		}
		else
		{
			vol = GWorld->CurrentLevelGridVolume;
		}

		FVector ext = worldBox.GetExtent();

		INT tileXCnt = (INT)appCeil(ext.X / halfTileSz);
		INT tileYCnt = (INT)appCeil(ext.Y / halfTileSz);

		const UBOOL bPromptUserToSave = FALSE;
		const UBOOL bSaveMapPackages = TRUE;
		const UBOOL bSaveContentPackages = TRUE;
		
		const UBOOL bFastSave = FALSE; // We are saving all files not just writable ones

#if 0
#if defined(_DEBUG) || 1
		if(appMsgf(AMT_YesNo, TEXT("Proc landscape?")))
#endif
		{
			for (INT j = 0; j < landscapes.Num(); ++j)
			{
				ALandscape* landscape = landscapes(j);
				RenameLandscapeRes(landscape);

				TArray<ULandscapeComponent*> ProcComps;
				for(UINT ComponentIndex = 0;ComponentIndex < (UINT)landscape->LandscapeComponents.Num();ComponentIndex++)
				{
					ULandscapeComponent* landComp = landscape->LandscapeComponents(ComponentIndex);
					ProcComps.AddItem(landComp);
				}

				for (INT compIdx = 0; compIdx < ProcComps.Num(); ++compIdx)
				{
					ULandscapeComponent* landComp = ProcComps(compIdx);
					FBox box = landComp->Bounds.GetBox();
					ULevel* lv = GetLevelForPlacingNewActor(box.GetCenter()); // oscar@snda

					if(lv == GWorld->PersistentLevel)
					{
						continue;
					}

					//lv = GetLevelForPlacingNewActor(box.GetCenter());
					if(lv)
					{
						INT dbgXMin = ((box.Min.X - worldBox.Min.X) / tileSize);
						INT dbgYMin = ((box.Min.Y - worldBox.Min.Y) / tileSize);

						INT dbgXMax = ((box.Max.X - worldBox.Min.X) / tileSize);
						INT dbgYMax = ((box.Max.Y - worldBox.Min.Y) / tileSize);

						ULevelStreaming* lvs = FLevelUtils::FindStreamingLevel(lv);

						warnf(NAME_Warning, TEXT("########## (%d,%d) - (%d,%d) %s (%d,%d,%d)"),
							dbgXMin, dbgYMin, dbgXMax, dbgYMax, *lv->GetOutermost()->GetName(),
							lvs->GridPosition[0],
							lvs->GridPosition[1],
							lvs->GridPosition[2]);

						GWarn->EnableUpdateUI = true;
						GWarn->StatusUpdatef( compIdx, ProcComps.Num(),
							*FString::Printf( TEXT("Moving Component : %s (%d/%d)"), *landComp->GetName(), compIdx, ProcComps.Num() ) );
						GWarn->EnableUpdateUI = false;

						GWorld->CurrentLevel = lv;
						MoveLandscapeComponentToLevels(landscape, landComp, lv);
					}
					else
					{
						warnf(NAME_Error, TEXT("No matching level found while searching grid cells for Landscape:%s block #%d!"),
							*landscape->GetPathName(), compIdx);
					}
				}

				FString mapPath;
				GPackageFileCache->FindPackageFile(*GWorld->GetMapName(), NULL, mapPath);
				FFilename mapName(mapPath);
				FString sharePkgPath = mapName.GetPath();

				FString PackageName =  GWorld->PersistentLevel->GetOutermost()->GetName() + TEXT("_grid");
				FString SaveFileName = sharePkgPath + TEXT("\\") + PackageName + TEXT(".upk");
				GUnrealEd->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""), *PackageName, *SaveFileName));
				FCheckoutResult sccResult = FSourceControl::TryCheckOut(SaveFileName);
				sccResult.TryAddToDefaultChangelist();

				FEditorFileUtils::SaveDirtyPackages( bPromptUserToSave, bSaveMapPackages, bSaveContentPackages, bFastSave );
			}
		}

		GWorld->CurrentLevelGridVolume = vol;
		GCallbackEvent->Send( CALLBACK_NewCurrentLevel );
#endif

#if 1
#if defined(_DEBUG) || 1
		if(appMsgf(AMT_YesNo, TEXT("Move actors to grid level?")))
#endif
		{
			warnf(TEXT("=============AutoSplit Actors==== Begin===================="));
			FLOAT startLogTime = appSeconds();
			for(INT ii = 0; ii < actorsToSplitArr.Num(); ++ii)
			{
				TArray<AActor*>& actorsToSplit = actorsToSplitArr(ii);

				g_levelActors.clear();

				for (INT i = 0; i < actorsToSplit.Num(); ++i)
				{
					GWarn->EnableUpdateUI = true;
					GWarn->StatusUpdatef(i, actorsToSplit.Num(), TEXT("[%d/%d] Splitting static mesh blocks (%d/%d)..."), ii, actorsToSplitArr.Num(), i + 1, actorsToSplit.Num());
					GWarn->EnableUpdateUI = false;
					AActor* actor = actorsToSplit(i);
					//actor->AddToRoot();
					TryAddToLvMap(actor);
				}

				INT step = 0;
				for (LevelActorMap::iterator itla = g_levelActors.begin();
					itla != g_levelActors.end(); ++itla)
				{
					GWarn->EnableUpdateUI = true;
					GWarn->StatusUpdatef(step, g_levelActors.size(), TEXT("[%d/%d] Moving static meshes (%d/%d)..."), ii, actorsToSplitArr.Num(), step + 1, g_levelActors.size());
					GWarn->EnableUpdateUI = false;

					INT movedActors = 0;
					MovesActorsToLevel( itla->second, itla->first, movedActors);

					//FEditorFileUtils::SaveDirtyPackages( bPromptUserToSave, bSaveMapPackages, bSaveContentPackages, bFastSave );
					++step;
				}

				// 			for (INT i = 0; i < actorsToSplit.Num(); ++i)
				// 			{				
				// 				AActor* actor = actorsToSplit(i);
				// 				actor->RemoveFromRoot();
				// 			}
			}
			FLOAT endLogTime = appSeconds();
			warnf(TEXT("=============AutoSplit Actors==== End====================%.2f"), startLogTime - endLogTime);
		}
#endif

		GCallbackEvent->Send( CALLBACK_RefreshEditor_LevelBrowser );


		//UObject::CollectGarbage( GARBAGE_COLLECTION_KEEPFLAGS );

		//GUnrealEd->Exec(TEXT("MAP CHECK"));

		AWorldInfo* WorldInfo = GWorld->GetWorldInfo();
		for( INT LevelIndex = 0 ; LevelIndex < WorldInfo->StreamingLevels.Num() ; ++LevelIndex )
		{
			ULevelStreaming* LevelStreamingObject = WorldInfo->StreamingLevels(LevelIndex);
			if( LevelStreamingObject )
			{
				if(LevelStreamingObject->GridPosition[0] > vol->Subdivisions[0]
				|| LevelStreamingObject->GridPosition[1] > vol->Subdivisions[1]
				|| LevelStreamingObject->GridPosition[2] > vol->Subdivisions[2])
				{
					FString lvName = LevelStreamingObject->LoadedLevel ? LevelStreamingObject->LoadedLevel->GetName() : TEXT("");
					GWarn->MapCheck_Add(MCTYPE_ERROR, LevelStreamingObject,
						*FString::Printf(TEXT("Invalid grid level coordinates: %s - %s %d,%d,%d"), *LevelStreamingObject->GetName(), *lvName,
						LevelStreamingObject->GridPosition[0], 
						LevelStreamingObject->GridPosition[1],
						LevelStreamingObject->GridPosition[2]), NULL);
				}
			}
		}


		//GWarn->EndSlowTask();
		//GWarn->MapCheck_ShowConditionally();
				
// 		actorsToSplit.Empty();
// 		FindActorsToSplit(actorsToSplit, landscapes);
// 
// 		INT endActorsCount = actorsToSplit.Num();
// 
// 		warnf(NAME_Log, TEXT("Check actors start:%d end:%d diff:%d"), startActorsCount, endActorsCount, endActorsCount - startActorsCount);

		g_LevelTiles.clear();
		g_levelActors.clear();

		warnf(TEXT("=============AutoSplit Actors==== Begin 2===================="));
		FLOAT startLogTime = appSeconds();
		UpdateLevelsForAllActors();
		FLOAT endLogTime = appSeconds();
		
		warnf(TEXT("=============AutoSplit Actors==== End 2====================%.2f"), startLogTime - endLogTime);
		//FSourceControl::DisableSCC = false;
		FLevelUtils::g_enableSelectWarn = true;
		
		GWarn->EnableUpdateUI = true;

		FSourceControl::EndBatchAddToCheckList();
	}

	void AutoMergeLevels()
	{
		FString ExlcudeMoveToGridLayer;
		GConfig->GetString(TEXT("Configuration"), TEXT("ExlcudeMoveToGridLayer"), ExlcudeMoveToGridLayer, GEditorIni);
		ExlcudeMoveToGridLayer = ExlcudeMoveToGridLayer.ToUpper();

		// oscar@snda
		FLevelUtils::g_enableSelectWarn = false;

		//FSourceControl::DisableSCC = true;
		FSourceControl::BeginBatchAddToCheckList();

		TArray< TArray<AActor*> > actorsToSplitArr;
		TArray<AActor*> actorsToSplitTmp;
		TArray<ALandscape*> landscapes;

		GEditor->SelectNone(TRUE, TRUE, FALSE);

		GWorld->CurrentLevel = GWorld->PersistentLevel;
		GCallbackEvent->Send( CALLBACK_NewCurrentLevel );		
		GEditor->SelectNone(TRUE, TRUE, FALSE);

 		GWarn->StatusUpdatef(-1, -1, TEXT("Collecting static mesh and terrain lists..."));
		for (INT i = 0; i < GWorld->Levels.Num(); ++i)
		{
			ULevel* level = GWorld->Levels(i);
			ULevelStreaming* lvs = FLevelUtils::FindStreamingLevel( level );
			if (lvs)
			{
				// ****_##x##y##z
				FString lvName = lvs->PackageName.ToString();
				INT idxDot = lvName.InStr(TEXT("."), TRUE, TRUE);
				bool zFound = lvName[lvName.Len() - 1] == L'z';
				bool yFound = lvName[lvName.Len() - 4] == L'y';
				bool xFound = lvName[lvName.Len() - 7] == L'x';
				if(!(xFound && yFound && zFound))
				{
					continue;
				}
			}
			else
			{
				continue;
			}

			GWarn->EnableUpdateUI = true;
			GWarn->StatusUpdatef(i + 1, GWorld->Levels.Num(), TEXT("[%d/%d] Moving static mesh: %s..."), i + 1, GWorld->Levels.Num(),  *level->GetPathName());
			GWarn->EnableUpdateUI = false;


			TLookupMap<AActor*> levelActors;

			for (INT j = 0; j < level->Actors.Num(); ++j)
			{
				AActor* actor = level->Actors(j);
				if ( actor )
				{
					if(lvs && lvs->EditorGridVolume && lvs->EditorGridVolume->IsActorMemberOfGrid( actor ) )
					{
						//continue;
					}

					FString actorLayer = actor->Layer.ToString();
					actorLayer = actorLayer.ToUpper();
					if(actor->IsReferencedByKismet())
					{
						continue;
					}

					if(actorLayer.InStr(ExlcudeMoveToGridLayer) != INDEX_NONE)
					{
						continue;
					}

					if(actor->IsA(AStaticMeshActor::StaticClass())
						|| actor->IsA(APointLight::StaticClass())
						|| actor->IsA(APointLightMovable::StaticClass())
						|| actor->IsA(APointLightToggleable::StaticClass())
						|| actor->IsA(ASkeletalMeshActor::StaticClass()))
					{
						levelActors.AddItem(actor);
					}
				}
			}

			INT movedActors = 0;
			MovesActorsToLevel( levelActors, GWorld->PersistentLevel, movedActors);
			SET_WARN_COLOR(COLOR_WHITE);
			warnf(NAME_Warning, TEXT("Found %d actors, moved %d"), levelActors.Num(), movedActors);
			CLEAR_WARN_COLOR();
		}

		GCallbackEvent->Send( CALLBACK_RefreshEditor_LevelBrowser );

		//FSourceControl::DisableSCC = false;
		FLevelUtils::g_enableSelectWarn = true;

		GWarn->EnableUpdateUI = true;

		FSourceControl::EndBatchAddToCheckList();
	}
// oscar@snda
}

