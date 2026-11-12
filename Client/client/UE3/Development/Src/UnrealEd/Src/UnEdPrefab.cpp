/*=============================================================================
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#include "UnrealEd.h"
#include "EngineSequenceClasses.h"
#include "EnginePrefabClasses.h"
#include "UnLinkedObjEditor.h"
#include "Kismet.h"
#include "ScopedTransaction.h"
#include "BSPOps.h"

// oscar@snda
#include "EngineSequenceClasses.h"
#include "EnginePhysicsClasses.h"
#include "EnginePrefabClasses.h"
#include "EngineAnimClasses.h"
#include "EngineSplineClasses.h"
#include "EngineSoundClasses.h"
#include "UnClient.h"
#include "EngineMeshClasses.h"
#include "GameFrameworkClasses.h"
#include "EngineFogVolumeClasses.h"

#include "..\..\Engine\Src\ScenePrivate.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"
#include "FileHelpers.h"
#include "SourceControl.h"
#include "UnTemplate.h"
#include "ImageUtils.h"

// oscar@snda

/**
 * Creates an archetype based on the parameters specified.  If PackageName or
 * ArchetypeName are not specified, displays a prompt to the user.
 *
 * @param	ArchetypeBase	the object to create the archetype from
 * @param	ArchetypeName	name for the archetype. 
 * @param	PackageName		package for the archetype
 * @param	GroupName		group for the archetype
 *
 * @return	a pointer to the archetype that was created
 */
UObject* UUnrealEdEngine::Archetype_CreateFromObject( UObject* ArchetypeBase, FString& ArchetypeName, FString& PackageName, FString& GroupName )
{
	if ( ArchetypeBase == NULL )
	{
		return NULL;
	}

	// if no package or name were specified, prompt the user for this info
	if ( ArchetypeName.Len() == 0 || PackageName.Len() == 0 )
	{
		WxDlgNewArchetype dlg;
		int Result = dlg.ShowModal(PackageName,GroupName,ArchetypeName);
		if ( Result == wxID_CANCEL )
			return NULL;

		ArchetypeName = dlg.GetObjectName();
		GroupName = dlg.GetGroup();
		PackageName = dlg.GetPackage();
	}

	UObject* Result = NULL;
	if ( ArchetypeName.Len() > 0 && PackageName.Len() > 0 )
	{
		// create or load the package specified
		UPackage* Package = GEngine->CreatePackage(NULL,*PackageName);
		UPackage* Group = NULL;
		if( GroupName.Len() )
		{
			// first try loading the group, in case it already exists
			Group = LoadObject<UPackage>(Package, *GroupName, NULL, LOAD_NoWarn|LOAD_FindIfFail,NULL);
			if ( Group == NULL )
			{
				Group = GEngine->CreatePackage(Package,*GroupName);
			}	
		}

		Package->SetDirtyFlag(TRUE);

		// Group will be used as the archetype's outer, so if the group is invalid, set it to the package itself
		if ( Group == NULL )
		{
			Group = Package;
		}

		// create the archetype class
		Result = ArchetypeBase->CreateArchetype(*ArchetypeName, Group);

		// Inform the content browser of the newly created archetype
		GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, Result));
	}

	return Result;
}

/** Function to ensure we didn't manage to set an Object's Archetype to be itself. */
static void CheckNoSelfArchetypes(INT Blah)
{
	for( FObjectIterator It; It; ++It )
	{
		UObject* Obj = *It;
		if(Obj->GetArchetype() == Obj)
		{
			appMsgf(AMT_OK, TEXT("Object '%s' Is It's Own Archetype! [%d]. Please Tell James Golding."), *Obj->GetName(), Blah);
		}
	}
}

/** 
*	Utility for taking a set of actors and finding a sensible 'origin' transform for the set.
*	Takes the origin of the Actor with the biggest bounding-box volume.
*/
static FMatrix FindSensibleTransform( TArray<UObject*>& Objects )
{
	// Init to world origin.
	FMatrix OutTransform;
	OutTransform.SetIdentity();

	FLOAT BiggestVol = -BIG_NUMBER;

	for(INT i=0; i<Objects.Num(); i++)
	{
		AActor* Actor = Cast<AActor>(Objects(i));
		if(Actor)
		{
			FBox Box = Actor->GetComponentsBoundingBox();

			FLOAT Vol = 0.f;
			if(Box.IsValid)
			{
				FVector Extent = Box.GetExtent();
				Vol = 8 * Extent.X * Extent.Y * Extent.Z;
			}

			if(Vol > BiggestVol)
			{
				OutTransform = FTranslationMatrix( Actor->Location );
				BiggestVol = Vol;
			}
		}
	}

	return OutTransform;
}

/**
 *	If there are references to non-RF_Public objects, other than those in the InObjects array, 
 *	this function will return false and pop up a warning.
 */
static UBOOL SequenceReferencesOnlyObjs(USequence* InSeq, TArray<UObject*>& InObjects)
{
	TArray<UObject*> BadObjs;

	// Check Events
	TArray<USequenceObject*> EventObjs;
	InSeq->FindSeqObjectsByClass(USequenceEvent::StaticClass(), EventObjs, TRUE);

	for(INT i=0; i<EventObjs.Num(); i++)
	{
		USequenceEvent* Evt = CastChecked<USequenceEvent>( EventObjs(i) );
		if( Evt->Originator && 
			!Evt->Originator->HasAnyFlags(RF_Public) &&
			!InObjects.ContainsItem(Evt->Originator) )
		{
			BadObjs.AddUniqueItem(Evt->Originator);
		}
	}

	// Check Object Vars
	TArray<USequenceObject*> VarObjs;
	InSeq->FindSeqObjectsByClass(USeqVar_Object::StaticClass(), VarObjs, TRUE);

	for(INT i=0; i<VarObjs.Num(); i++)
	{
		USeqVar_Object* Var = CastChecked<USeqVar_Object>( VarObjs(i) );
		if( Var->ObjValue && 
			!Var->ObjValue->HasAnyFlags(RF_Public) &&
			!InObjects.ContainsItem(Var->ObjValue) )
		{
			BadObjs.AddUniqueItem(Var->ObjValue);
		}
	}

	// If we found some bad references, print out what they are.
	if(BadObjs.Num() > 0)
	{
		FString BadObjMessage = FString::Printf( LocalizeSecure(LocalizeUnrealEd("Prefab_SequenceRefsOutsidePrefab"), *InSeq->ObjName) );
		for(INT i=0; i<BadObjs.Num(); i++)
		{
			BadObjMessage += FString::Printf( TEXT("\t%s\n"), *BadObjs(i)->GetName() );
		}
		appMsgf(AMT_OK, *BadObjMessage);

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/** 
 *	Find the Kismet Sequence that relates to this Prefab
 *	We basically find the sequence that contains all references to the objects we pass in.
 */
static USequence* FindPrefabSequence(TArray<UObject*>& InObjects)
{
	// @todo: Handle multi-level case with multiple root sequences.
	USequence* RootSeq = GWorld->GetGameSequence();
	if(!RootSeq)
	{
		return NULL;
	}

	USequence* OutSeq = NULL;

	// Iterate over each object we passed in..
	for(INT ObjIndex=0; ObjIndex<InObjects.Num(); ObjIndex++)
	{
		UObject* Obj = InObjects(ObjIndex);

		// Find all the SequenceObjects that refer to this object.
		TArray<USequenceObject*> ReferringSeqObjs;
		RootSeq->FindReferencingSequenceObjects(Obj, &ReferringSeqObjs);

		for(INT RefIndex=0; RefIndex<ReferringSeqObjs.Num(); RefIndex++)
		{
			// Find the sequence that this SeqObj is in.
			USequence* ContainSeq = Cast<USequence>( ReferringSeqObjs(RefIndex)->GetOuter() );
			if(ContainSeq)
			{
				// If we don't have a 'current containing sequence', use this
				if(!OutSeq)
				{
					OutSeq = ContainSeq;
				}
				// If we do - find the lowest sequence that contains OutSeq and ContainSeq.
				else
				{
					// If ContainSeq is already within OutSeq (or they are they same) we are fine.
					if(ContainSeq->IsIn(OutSeq) || ContainSeq == OutSeq)
					{
						// do nothing - we're ok
					}
					// If OutSeq is within ContainSeq, we pick ContainSeq as our 'containing sequence' instead
					else if(OutSeq->IsIn(ContainSeq))
					{
						OutSeq = ContainSeq;
					}
					// If no parent/child relationship, we walk up the parents of ContainSeq until we find a point that contains OutSeq as well.
					else
					{
						USequence* ParentSeq = Cast<USequence>(ContainSeq->GetOuter());
						UBOOL bFoundOutSeq = false;
						while(ParentSeq && !bFoundOutSeq)
						{
							if(OutSeq->IsIn(ParentSeq))
							{
								OutSeq = ParentSeq;
								bFoundOutSeq = true;
							}

							ParentSeq = Cast<USequence>(ParentSeq->GetOuter());
						}

						// Once you hit the root, you should have
						check(bFoundOutSeq);
					}
				}
			}
		}
	}

	return OutSeq;
}

/** 
*	Util for rmeoving InSeq from its parent sequence.
*	If InSeq is the root sequence, we empty it.
*/
static void RemoveSequenceFromParent(USequence* InSeq)
{
	USequence* ParentSeq = Cast<USequence>(InSeq->GetOuter());
	if(ParentSeq)
	{
		// Remove sequence from its parent.
		ParentSeq->RemoveObject(InSeq);
	}
	// The only condition under which the current prefab sequence does not have a sequence as an outer is if its the root.
	else
	{
		USequence* RootSeq = GWorld->GetGameSequence();
		check(InSeq == RootSeq);

		RootSeq->Modify();

		// In this case, we just empty the whole thing.
		RootSeq->SequenceObjects.Empty();
	}

	// Make sure there are no Kismet windows open on the sequence we are removing (or its child sequences).
	WxKismet::EnsureNoKismetEditingSequence(InSeq);
}

/** Util for determining if Object is valid for inclusion in a Prefab. */
static UBOOL ObjectIsValidForPrefab(UObject* InObj)
{
	AActor* Actor = Cast<AActor>(InObj);

	// Determine if an actor is of a type unsupported as part of a prefab
	UBOOL bValidActorTypeForPrefab = Actor != NULL && !Actor->IsABuilderBrush() && !Actor->IsA( AWorldInfo::StaticClass() ) && !Actor->IsA( ADefaultPhysicsVolume::StaticClass() ) 
			&& !Actor->IsA( ADominantDirectionalLight::StaticClass() ) && !Actor->IsA( ADominantPointLight::StaticClass() ) && !Actor->IsA( ADominantSpotLight::StaticClass() );
	
	if( InObj &&								// Non-null
		!InObj->IsPendingKill() &&				// Not deleted
		!InObj->IsInPrefabInstance() &&			// Not already in another prefab
		bValidActorTypeForPrefab )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/** Fills in the specified strings with the selected package and group in the Generic Browser. */
static void SetPackageAndGroupNameFromGB(FString& OutPackageName, FString& OutGroupName)
{
	UObject* PackageObj	= NULL;
	UObject* GroupObj	= NULL;

	//@todo cb: GB conversion
	OutPackageName	= PackageObj ? PackageObj->GetName() : TEXT( "MyPackage" );
	OutGroupName	= GroupObj ? GroupObj->GetFullGroupName( 0 ): TEXT("");
}

/** 
 *	Create a new Prefab based on the array of objects passed in.
 *	Will pop up a dialog box asking user for package, group and prefab name.
 *	Then looks to see if there is a Kismet sequence associated with the Objects that were passed in, and asks
 *	if we want to copy it as part of the Prefab.
 *	Then it goes over each object, creating an Archetype for it.
 *	Finally, it asks if we want to destroy the selected objects and create an instance of the new Prefab instead.
 *
 *	@param	InObjects	Array of objects to use as the basis for the Prefab. Usually includes Actors in the level.
 */
UPrefab* UUnrealEdEngine::Prefab_NewPrefab( TArray<UObject*> InObjects )
{
	// Check all objects passed in have the same Outer-most object. Otherwise, fail.
	UObject* CheckOutermost = NULL;
	for(INT i=0; i<InObjects.Num(); i++)
	{
		if( InObjects(i) )
		{
			// If first object, use its Outermost to check against all the others.
			if(CheckOutermost == NULL)
			{
				CheckOutermost = InObjects(i)->GetOutermost();
			}
			// Otherwise, check this objects Outermost again CheckOutermost
			else
			{
				if( InObjects(i)->GetOutermost() != CheckOutermost )
				{
					// If we fail, print a warning and fail.
					appMsgf( AMT_OK, *LocalizeUnrealEd("Prefab_MustHaveSameOutermost") );
					return NULL;
				}
			}
		}
	}

	// Make list of all valid obejcts to make into a prefab.
	TArray<UObject*> AddObjects;
	for(INT i=0; i<InObjects.Num(); i++)
	{
		if( ObjectIsValidForPrefab(InObjects(i)) )
		{
			AddObjects.AddItem(InObjects(i));
		}	
	}

	// Do nothing if no actors selected.
	if(AddObjects.Num() == 0)
	{
		return NULL;
	}

	FString PrefabName, PackageName, GroupName;
	SetPackageAndGroupNameFromGB( PackageName, GroupName );

	WxDlgNewArchetype dlg;
	const int Result = dlg.ShowModal(PackageName, GroupName, PrefabName);
	if ( Result == wxID_CANCEL )
	{
		return NULL;
	}

	PrefabName = dlg.GetObjectName();
	GroupName = dlg.GetGroup();
	PackageName = dlg.GetPackage();	

	UPrefab* NewPrefab = NULL;
	if ( PrefabName.Len() > 0 && PackageName.Len() > 0 )
	{
		FScopedTransaction Transaction( *LocalizeUnrealEd(TEXT("Prefab_Create")) );

		// create or load the package specified
		UPackage* Package = GEngine->CreatePackage(NULL,*PackageName);
		UPackage* Group = NULL;
		if( GroupName.Len() )
		{
			// first try loading the group, in case it already exists
			Group = LoadObject<UPackage>(Package, *GroupName, NULL, LOAD_NoWarn|LOAD_FindIfFail,NULL);
			if ( Group == NULL )
			{
				Group = GEngine->CreatePackage(Package,*GroupName);
			}
		}

		// Group will be used as the prefabs's outer, so if the group is invalid, set it to the package itself
		if ( Group == NULL )
		{
			Group = Package;
		}

		// If we still don't have an outer for the new Prefab, fail now.
		if( Group == NULL )
		{
			Transaction.Cancel();
			return NULL;
		}

		// Find the Kismet sequence that relates to the objects we put in the prefab.
		USequence* PrefabSeq = FindPrefabSequence(AddObjects);
		if(PrefabSeq)
		{
			debugf( TEXT("Found Sequence '%s' For New Prefab."), *PrefabSeq->GetName() );

			const INT SeqAnswer = appMsgf( AMT_YesNoCancel, LocalizeSecure(LocalizeUnrealEd("Prefab_FoundSeq"), *PrefabSeq->GetName(), *PrefabSeq->GetName()) );
			check(SeqAnswer == 0 || SeqAnswer == 1 || SeqAnswer == 2);

			if(SeqAnswer == 0) // Yes
			{
				// Check that this sequence doesn't reference anything outside the Prefab.
				const UBOOL bSeqOK = SequenceReferencesOnlyObjs(PrefabSeq, AddObjects);

				// If it does (if it fails the test), bail out.
				if(!bSeqOK)
				{
					Transaction.Cancel();
					return NULL;
				}

				// Do nothing - we are going to copy the sequence.
			}
			else if(SeqAnswer == 1) // No
			{
				PrefabSeq = NULL;
			}
			else // Cancel
			{
				// If we want to give up - do so now.
				Transaction.Cancel();
				return NULL;
			}
		}

		// Create new prefab object.

		NewPrefab = CastChecked<UPrefab>(StaticConstructObject(UPrefab::StaticClass(), Group, *PrefabName, RF_Public|RF_Standalone|RF_Transactional));
		Group->MarkPackageDirty(TRUE);

		// Find the 'origin' for the prefab. Locations of actors are relative to this.
		FMatrix PrefabToWorld = FindSensibleTransform(AddObjects);
		FMatrix WorldToPrefab = PrefabToWorld.Inverse();

		// maps existing objects to archetypes created from that actor
		TMap<UObject*,UObject*> InstanceToArchetypeMap;

		for(INT i=0; i<AddObjects.Num(); i++)
		{
			UObject* Obj = AddObjects(i);
			AActor* ActorInst = Cast<AActor>(Obj);
			ABrush* BrushInst = Cast<ABrush>(Obj);

			// Make name for the new archetype
			FString ArchetypeName = PrefabName + FString::Printf( TEXT("_Arc%d"), i );

			// We don't need to 'save' the actors with all the components fully initialized, so reset them here.
			if(ActorInst)
			{
				ActorInst->ClearComponents();
			}

			// Create new archetype, with an outer the same as the new Prefabs Outer.
			UObject* NewArchetype = Obj->CreateArchetype(*ArchetypeName, NewPrefab);
			check( NewArchetype->HasAllFlags(RF_Public|RF_ArchetypeObject) );

			// Re-init components again after we have created the archetype.
			if(ActorInst)
			{
				ActorInst->ForceUpdateComponents(FALSE,FALSE);
			}

			// Special Brush handling code.
			if(BrushInst)
			{
				ABrush* BrushArchetype = CastChecked<ABrush>(NewArchetype);

				FBSPOps::csgCopyBrush(BrushArchetype, BrushInst, BrushInst->PolyFlags, 0, FALSE, FALSE);
			}

			// Allow the objects to do any class-specific cleanup for being in a prefab.
			NewArchetype->OnAddToPrefab();

			// Fix up location/rotation to be relative to prefab origin.
			APrefabInstance::ApplyTransformIfActor(NewArchetype, WorldToPrefab);

			// Add Archetype to prefabs array.
			NewPrefab->PrefabArchetypes.AddItem(NewArchetype);

			// Create mapping of actor in level to the archetype we created for it.
			InstanceToArchetypeMap.Set(Obj, NewArchetype);

			// put the actor's components into the InstanceToArchetype map
			if (ActorInst)
			{
				AActor* ActorArchetype = CastChecked<AActor>(NewArchetype);
				check(ActorInst->Components.Num() == ActorArchetype->Components.Num());

				for (INT ComponentIndex = 0; ComponentIndex < ActorInst->Components.Num(); ComponentIndex++)
				{
					check(ActorInst->Components(ComponentIndex)->GetClass() == ActorArchetype->Components(ComponentIndex)->GetClass());
					InstanceToArchetypeMap.Set(ActorInst->Components(ComponentIndex), ActorArchetype->Components(ComponentIndex));
				}
			}

		}

		UpdatePropertyWindows();

		if ( InstanceToArchetypeMap.Num() > 0 )
		{
			// convert any references to the old version of the actor into either a) archetype'd versions, or b) null refs
			UPrefab::ResolveInterDependencies(InstanceToArchetypeMap, NewPrefab->GetOutermost() != CheckOutermost);
		}

		// Now we copy the Kismet sequence into the prefab.
		if(PrefabSeq)
		{
			NewPrefab->CopySequenceIntoPrefab(PrefabSeq, InstanceToArchetypeMap);
		}

		// Deselect everything, so the screenshot doesn't have any blue stuff in it...
		SelectNone( TRUE, TRUE );

		// Take a screenshot from the current 3D viewport and use that as the thumbnail.
		Prefab_CreatePreviewImage(NewPrefab);


		// If we want to replace the current actors - delete all the old ones and spawn a new prefab.
		const UBOOL bDoReplace = appMsgf( AMT_YesNo, *LocalizeUnrealEd("Prefab_ReplaceCurrentActors") );
		if(bDoReplace)
		{
			// First destroy all the selected actors
			for(INT i=0; i<AddObjects.Num(); i++)
			{
				AActor* Actor = Cast<AActor>(AddObjects(i));
				if(Actor)
				{
					GetSelectedActors()->Deselect(Actor);
					GWorld->EditorDestroyActor(Actor, TRUE);
				}
			}

			// Then destroy the Kismet sequence we moved into the Prefab.
			if(PrefabSeq)
			{
				RemoveSequenceFromParent(PrefabSeq);
			}

			// Then create instance of prefab we just created.			
			Prefab_InstancePrefab( NewPrefab, PrefabToWorld.GetOrigin(), PrefabToWorld.Rotator() );
		}

		// Update browsers to reflect change.
		GCallbackEvent->Send( CALLBACK_RefreshEditor_AllBrowsers );

		// Refresh Content Browser.
		// Note: We intentionally pass CRB_NoSync in as a flag, as otherwise the Content Browser refresh will try to select the prefab, which is undesirable for creating new prefabs.
		GCallbackEvent->Send( FCallbackEventParameters( NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageList|CBR_ObjectCreated|CBR_NoSync, NewPrefab ) );

		return NewPrefab;
	}
	else
	{
		if ( PrefabName.Len() == 0 )
		{
			appMsgf( AMT_OK, *LocalizeUnrealEd("Error_ObjectNameNotSpecified") );
		}
		else if ( PackageName.Len() == 0 )
		{
			appMsgf( AMT_OK, *LocalizeUnrealEd("Error_PackageFileNotSpecified") );
		}
		return NULL;
	}
}

/**
 *	Take the current perspective viewport view location/rotation and use that to take a snapshot
 *	and save it as a texture with the Prefab.
 */
void UUnrealEdEngine::Prefab_CreatePreviewImage( UPrefab* InPrefab )
{
	const UINT MinRenderTargetSize = 256;
	UTextureRenderTarget2D* RendTarget = GetScratchRenderTarget( MinRenderTargetSize );

	// Get the setting for the perspective viewport
	FSceneInterface* Scene = NULL;
	FViewport* Viewport = NULL;
	FEditorLevelViewportClient* LevelVC = NULL;
	FVector ViewLocation;
	FRotator ViewRotation;
	for(INT i=0; i<GApp->EditorFrame->ViewportConfigData->GetViewportCount(); i++)
	{
		// Iterate over the 4 main viewports.
		LevelVC = GApp->EditorFrame->ViewportConfigData->AccessViewport(i).ViewportWindow;
		if(LevelVC && LevelVC->ViewportType == LVT_Perspective)
		{
			ViewLocation = LevelVC->ViewLocation;
			ViewRotation = LevelVC->ViewRotation;

			Viewport = LevelVC->Viewport;
			Scene = LevelVC->GetScene();
			break;
		}
	}

	// If we found the viewport, render the scene to the probe now.
	if(LevelVC && Scene && Viewport)
	{	
		// view matrix to match current perspective viewport view orientation
		FMatrix ViewMatrix = FTranslationMatrix(-ViewLocation);
		ViewMatrix = ViewMatrix * FInverseRotationMatrix(ViewRotation);
		ViewMatrix = ViewMatrix * FMatrix(
			FPlane(0,	0,	1,	0),
			FPlane(1,	0,	0,	0),
			FPlane(0,	1,	0,	0),
			FPlane(0,	0,	0,	1));

		// projection matrix based on the fov,near/far clip settings
		const FLOAT FOV = 80.0f;
		FMatrix ProjectionMatrix = FPerspectiveMatrix(
			FOV * (FLOAT)PI / 360.0f,
			(FLOAT)RendTarget->SizeX,
			(FLOAT)RendTarget->SizeY,
			GNearClippingPlane			
			);

		FSceneViewFamilyContext ViewFamily(
			Viewport,
			Scene,
			SHOW_DefaultEditor,
			GWorld->GetTimeSeconds(),
			GWorld->GetDeltaSeconds(),
			GWorld->GetRealTimeSeconds(),
			LevelVC->IsRealtime(),
			FALSE);
		FSceneView* View = LevelVC->CalcSceneView(&ViewFamily);

		// render capture show flags use lit without shadows, fog, or post process effects
		EShowFlags CaptureShowFlags = SHOW_DefaultGame & (~SHOW_SceneCaptureUpdates) & (~SHOW_Fog) & (~SHOW_PostProcess);
		CaptureShowFlags = (CaptureShowFlags&~SHOW_ViewMode_Mask) | SHOW_ViewMode_Lit;

		// create the 2D capture probe, this is deleted on the render thread after the capture proxy renders
		FSceneCaptureProbe2D* CaptureProbe = new FSceneCaptureProbe2D(
			NULL, 
			RendTarget,
			CaptureShowFlags,
			FLinearColor::Black,
			0, 
			NULL,
			FALSE,
			FALSE,
			FALSE,
			FALSE,
			WORLD_MAX,
			WORLD_MAX,
			0.0,
			ViewMatrix,
			ProjectionMatrix 
			);

		// render the capture without relying on regular viewport draw loop by using a scene capture proxy
		FSceneCaptureProxy SceneCaptureProxy(Viewport,&ViewFamily);
		SceneCaptureProxy.Render(CaptureProbe,TRUE);

		// Convert render target to a UTexture2D with the new Prefab as an outer.
		FString PreviewName = FString::Printf( TEXT("%s_Preview"), *InPrefab->GetName() );
		InPrefab->PrefabPreview = RendTarget->ConstructTexture2D(InPrefab, PreviewName, RF_NotForServer | RF_NotForClient);
	}

}

// oscar@snda
// extern UBOOL GenerateMipChainBaseFromRaw(
// 	UTexture2D &Texture,
// 	IntermediateMipChain &OutMipChain,
// 	UINT *RawColors,
// 	UINT SrcWidth,
// 	UINT SrcHeight);

void UUnrealEdEngine::CaptuerMapTexture()
{
	FVector boundMin = FVector(0,0,0);
	FVector boundMax = FVector(0,0,0);
	FString AerialPhotoPath;
	FString AerialPhotoName;
	INT DstWidth = 2048;
	INT DstHeight = 2048;
	INT AerialPhotoDefaultSize = 512;
	INT Precent = 54;

	GWarn->BeginSlowTask(TEXT("Snapshoting ..."), TRUE);

	GConfig->GetString(TEXT("Configuration"), TEXT("AerialPhotoPath"), AerialPhotoPath, GEditorIni);
	GConfig->GetInt(TEXT("Configuration"), TEXT("AerialPhotoWidth"), DstWidth, GEditorIni);
	GConfig->GetInt(TEXT("Configuration"), TEXT("AerialPhotoHeight"), DstHeight, GEditorIni);
	GConfig->GetInt(TEXT("Configuration"), TEXT("AerialPhotoDefaultSize"), AerialPhotoDefaultSize, GEditorIni);
	GConfig->GetString(TEXT("Configuration"), TEXT("AerialPhotoName"), AerialPhotoName, GEditorIni);
	GConfig->GetInt(TEXT("Configuration"), TEXT("AerialPhotoPrecent"), Precent, GEditorIni);

	FString MapName = GWorld ? GWorld->GetMapName() : TEXT("None");
	//FString SrcTextureName = "ViewMap";
	FString PackageName = FString::Printf(TEXT("%s_Map2"), *MapName);
	UPackage* Package = GEngine->CreatePackage(NULL, *PackageName);
	Package->SetDirtyFlag(TRUE);

	// Get the setting for the perspective viewport
	FSceneInterface* Scene = NULL;
	FViewport* Viewport = NULL;
	FEditorLevelViewportClient* LevelVC = NULL;

	for(INT i=0; i<GApp->EditorFrame->ViewportConfigData->GetViewportCount(); i++)
	{
		// Iterate over the 4 main viewports.
		LevelVC = GApp->EditorFrame->ViewportConfigData->AccessViewport(i).ViewportWindow;
		if(LevelVC/* && LevelVC->ViewportType == LVT_OrthoXY*/)
		{
			Viewport = LevelVC->Viewport;
			Scene = LevelVC->GetScene();
			break;
		}
	}

	FEditorLevelViewportClient* ViewportClient = LevelVC;//GEditor->ViewportClients(i);

	EShowFlags OldShowFlags = ViewportClient->ShowFlags;
	FVector OldViewLocation = ViewportClient->ViewLocation;
	FRotator OldViewRotation = ViewportClient->ViewRotation;
	FLOAT OldOrthoZoom = ViewportClient->OrthoZoom;
	BOOL OldGameViewMode = ViewportClient->bInGameViewMode;

	TArray<AMMOVolume_MapSize*> VolumeList;
	TArray<APylon*> PylonList;
	TArray<AFogVolumeDensityInfo*> FogList;

	for (INT j = 0; j < GWorld->Levels.Num(); ++j)
	{
		ULevel* Level = GWorld->Levels(j);// Export DynamicBlockingVolume
		for (INT k = 0; k < Level->Actors.Num(); ++k)
		{
			AActor* actor = Level->Actors(k);

			if(actor)
			{				
				if(actor->IsA(AMMOVolume_MapSize::StaticClass()))
				{
					AMMOVolume_MapSize* MapVol = Cast<AMMOVolume_MapSize>(actor);
					if(MapVol && (MapVol->IsExport == FALSE))
					{
						continue;
					}

					VolumeList.AddItem(MapVol);
				}
				else if(actor->IsA(APylon::StaticClass()))
				{
					PylonList.AddItem(Cast<APylon>(actor));
				}
				else if(actor->IsA(AFogVolumeDensityInfo::StaticClass()))
				{
					FogList.AddItem(Cast<AFogVolumeDensityInfo>(actor));
				}
			}
		}
	}

	TArray<UBOOL> ShowPylonLines;
	for (INT j = 0; j < PylonList.Num(); ++j)
	{
		APylon* pylon = PylonList(j);
		ShowPylonLines.AddItem(pylon->bDonotRenderLines);
		pylon->bDonotRenderLines = TRUE;
	}

	TArray<UBOOL> EnableFogs;
	for (INT j = 0; j < FogList.Num(); ++j)
	{
		AFogVolumeDensityInfo* fog = FogList(j);
		EnableFogs.AddItem(fog->DensityComponent->bEnabled);
		fog->DensityComponent->bEnabled = FALSE;
		FComponentReattachContext ReattachContext(fog->DensityComponent);
		//fog->ConditionalUpdateComponents();
	}

	LevelVC->Invalidate();

	const INT MaxSubStep = 6;
	for (INT j = 0; j < VolumeList.Num(); ++j)
	{
		AMMOVolume_MapSize* MapVol = VolumeList(j);

		FBox ActorBox;
		ActorBox = MapVol->GetComponentsBoundingBox(TRUE);
		boundMax = ActorBox.Max;
		boundMin = ActorBox.Min;
		FVector boundSz = boundMax - boundMin;
		
		Precent = MapVol->AerialPhotoPrecent;
		//initial the PixelSize of map
		MapVol->MiniMapPixelSize = Precent;
		MapVol->WorldMapPixelSize = Precent;

		//if(MapVol->MapSizeType == EMST_TYPE1)
		//{
		//	DstWidth = DstHeight = 2048;

		//	MapVol->MiniMapTileColNum = 16;// 2048/128
		//	MapVol->MiniMapTileRowNum = 16;
		//}
		//else if(MapVol->MapSizeType == EMST_TYPE2)
		//{
		//	DstWidth = DstHeight = 1024;

		//	MapVol->MiniMapTileColNum = 8;// 1024/128
		//	MapVol->MiniMapTileRowNum = 8;
		//}
		//else
		//{
		//	DstWidth = DstHeight = 512;

		//	MapVol->MiniMapTileColNum = 4;// 512/128
		//	MapVol->MiniMapTileRowNum = 4;
		//}

		//DstWidth = DstHeight = 512;
		MapVol->MiniMapTileColNum = 2;// 512/256
		MapVol->MiniMapTileRowNum = 2;

		//modify offset in y of mini map
		MapVol->MiniMapOffsetY = 512;//DstHeight;


		MapVol->GetOutermost()->MarkPackageDirty();

		if(Precent <= 0)
			continue;

		AerialPhotoName = MapVol->GetName();

		GWarn->StatusUpdatef(j * MaxSubStep + 1, VolumeList.Num() * MaxSubStep, TEXT("Creating render target:%s..."), *AerialPhotoName);

		//
		UTextureRenderTarget2D* RendTarget = GEditor->GetScratchRenderTarget( DstWidth );
		if(!RendTarget)
		{
			appMsgf(AMT_OK, TEXT("Not enough video memory to create the in-memory texture..., size: %d x %d"), DstWidth, DstHeight);
			return;
		}

		FVector ViewLocation = ActorBox.GetCenter();
		FRotator ViewRotation = FRotator(C_DegToUnrRot * -90, 0, 0);

		Viewport = LevelVC->Viewport;
		Scene = LevelVC->GetScene();

		// view matrix to match current perspective viewport view orientation
		FMatrix ViewMatrix = FTranslationMatrix(-ViewLocation);
		ViewMatrix = ViewMatrix * FInverseRotationMatrix(ViewRotation);
		ViewMatrix = ViewMatrix * FMatrix(
			FPlane(0,	0,	1,	0),
			FPlane(1,	0,	0,	0),
			FPlane(0,	1,	0,	0),
			FPlane(0,	0,	0,	1));

		// projection matrix based on the fov,near/far clip settings
		const FLOAT FOV = 45.0f;
		FLOAT maxWorldSz = Max(Abs(boundSz.X), Abs(boundSz.Y));
		FLOAT minWorldSz = Min(Abs(boundSz.X), Abs(boundSz.Y));
		const FLOAT	Zoom = minWorldSz / RendTarget->SizeX;
		FMatrix ProjectionMatrix = FOrthoMatrix(
			Zoom * (FLOAT)RendTarget->SizeX / 2.0f,
			Zoom * (FLOAT)RendTarget->SizeY / 2.0f,
			0.5f / minWorldSz/*HALF_WORLD_MAX*/,
			minWorldSz//HALF_WORLD_MAX
			);

		FLOAT photo3DWidth = minWorldSz;
		FLOAT finalWorldMap2DSize = RendTarget->SizeX;
		//modify scale of mini map to keep the final scale a constant
		INT constPrecent = 70;
		GConfig->GetInt(TEXT("Configuration"), TEXT("AerialPhotoPrecentConst"), constPrecent, GEditorIni);
		//const INT map_SNAPSHOT_COORDINATE_STEP = 1024;
		//FLOAT constA = INT( map_SNAPSHOT_COORDINATE_STEP / constPrecent ) / FLOAT( map_SNAPSHOT_COORDINATE_STEP );
		//MapVol->MiniMapScale = FLOAT( map_SNAPSHOT_COORDINATE_STEP * constA / ( (INT)(map_SNAPSHOT_COORDINATE_STEP / Precent) ) );
		//MapVol->MiniMapScale = Min(1.0f, MapVol->MiniMapScale);
		MapVol->MiniMapPixelSize = constPrecent;
		//MapVol->MiniMapScale = photo3DWidth / RendTarget->SizeX / constPrecent;//photo3DWidth / ( finalMiniMap2DSize * MapVol->MiniMapPixelSize );
		MapVol->MiniMapScale = photo3DWidth / AerialPhotoDefaultSize / constPrecent;
		MapVol->MiniMapScale = Min(1.0f, MapVol->MiniMapScale);

		//
		MapVol->WorldMapPixelSize = photo3DWidth / AerialPhotoDefaultSize;
		//MapVol->WorldMapScale = (FLOAT)MapVol->WorldMapSizeX / RendTarget->SizeX;
		MapVol->WorldMapScale = (FLOAT)MapVol->WorldMapSizeX / AerialPhotoDefaultSize;
		MapVol->WorldMapScale = Min(1.0f, MapVol->WorldMapScale);

		//
		FSceneViewFamilyContext ViewFamily(
			Viewport,
			Scene,
			SHOW_DefaultEditor,
			GWorld->GetTimeSeconds(),
			GWorld->GetDeltaSeconds(),
			GWorld->GetRealTimeSeconds(),
			LevelVC->IsRealtime(),
			FALSE);
		FSceneView* View = LevelVC->CalcSceneView(&ViewFamily);

		// render capture show flags use lit without shadows, fog, or post process effects
		EShowFlags CaptureShowFlags = SHOW_DefaultGame & (~SHOW_SceneCaptureUpdates) & (~SHOW_Fog) & (~SHOW_PostProcess);
		CaptureShowFlags = (CaptureShowFlags&~SHOW_ViewMode_Mask) | SHOW_ViewMode_Lit;

		// create the 2D capture probe, this is deleted on the render thread after the capture proxy renders
		FSceneCaptureProbe2D* CaptureProbe = new FSceneCaptureProbe2D(
			NULL, 
			RendTarget,
			CaptureShowFlags,
			FLinearColor::Black,
			0, 
			NULL,
			FALSE,
			FALSE,
			FALSE,
			FALSE,
			WORLD_MAX,
			WORLD_MAX,
			0.0,
			ViewMatrix,
			ProjectionMatrix 
			);

		GWarn->StatusUpdatef(j * MaxSubStep + 2, VolumeList.Num() * MaxSubStep, TEXT("Rendering world map:%s..."), *AerialPhotoName);

		// render the capture without relying on regular viewport draw loop by using a scene capture proxy
		FSceneCaptureProxy SceneCaptureProxy(Viewport,&ViewFamily);
		SceneCaptureProxy.Render(CaptureProbe,TRUE);

		//}

		GWarn->StatusUpdatef(j * MaxSubStep + 3, VolumeList.Num() * MaxSubStep, TEXT("Creating world map texture:%s..."), *AerialPhotoName);
		FString strTexName;// = FString::Printf(TEXT("%s_%s"), *MapName, *AerialPhotoName);
// 		UTexture2D* DstTexture = RendTarget->ConstructTexture2D(Package, strTexName, RF_Public|RF_Standalone,
// 			TC_VectorDisplacementmap, NULL, TRUE);
		//if(MapVol->MiniMapScale < 1.0f)
		//{
		//	TArray<FColor> RawData, ScaleData;
		//	const UINT ScaledSizeX = Max((UINT)2, (UINT)(DstWidth * MapVol->MiniMapScale));
		//	const UINT ScaledSizeY = ScaledSizeX;

		//	FRenderTarget* rendTar = RendTarget->GameThread_GetRenderTargetResource();
		//	if(!rendTar)
		//	{
		//		appMsgf(AMT_OK, TEXT("Render texture accessed from game thread is empty, cannot generate the downscaled minimap texture!"));
		//		continue;
		//	}

		//	rendTar->ReadPixels(RawData);
		//	FImageUtils::ImageResize(DstWidth, DstWidth, RawData, ScaledSizeX, ScaledSizeY, ScaleData, TRUE);

		//	INT newSz = RawData.Num();
		//	RawData.Empty();
		//	RawData.AddZeroed(newSz);

		//	for (UINT y = 0; y < ScaledSizeY; ++y)
		//	{
		//		memcpy_s((BYTE*)&RawData((DstWidth - ScaledSizeY + y) * DstWidth), newSz * sizeof(FColor),
		//			(BYTE*)&ScaleData(y * ScaledSizeX), ScaledSizeX * sizeof(FColor));
		//	}

		//	strTexName = FString::Printf(TEXT("%s_%s"), *MapName, *AerialPhotoName);
		//	FCreateTexture2DParameters Params;
		//	Params.bWantSourceArt = TRUE;
		//	Params.CompressionSettings = TC_VectorDisplacementmap;
		//	Params.bSRGB = FALSE;
		//	Params.bUseAlpha = TRUE;
		//	UTexture2D* tmpTex = FImageUtils::CreateTexture2D(DstWidth, DstWidth, RawData,
		//		Package, strTexName, RF_Public|RF_Standalone, Params);
		//	tmpTex->LODGroup = TEXTUREGROUP_UI;
		//	tmpTex->PostEditChange();

		//	GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, tmpTex));
		//}
		//else
		{
			strTexName = FString::Printf(TEXT("%s_%s"), *MapName, *AerialPhotoName);
			UTexture2D* tmpTex = RendTarget->ConstructTexture2D(Package, strTexName, RF_Public|RF_Standalone,
				TC_VectorDisplacementmap, NULL, TRUE);
			tmpTex->LODGroup = TEXTUREGROUP_UI;
			tmpTex->PostEditChange();
			GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, tmpTex));
		}

		
// 		DstTexture->LODGroup = TEXTUREGROUP_UI;
// 		DstTexture->PostEditChange();
// 		GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, DstTexture));


		//------------------------------------------------------------------------------------------------
		// capture mini map

		CaptureShowFlags = SHOW_DefaultEditor & ~SHOW_StaticMeshes & ~SHOW_Fog & ~SHOW_Volumes
			& ~SHOW_Particles & ~SHOW_Sprites & ~SHOW_Terrain & ~SHOW_PostProcess & ~SHOW_SkeletalMeshes
			& ~SHOW_Splines & ~SHOW_DebugLine & ~SHOW_Bounds;

		CaptureShowFlags = SHOW_ViewMode_Wireframe | (CaptureShowFlags & ~SHOW_ViewMode_Mask) | SHOW_Paths;

		// create the 2D capture probe, this is deleted on the render thread after the capture proxy renders
		CaptureProbe = new FSceneCaptureProbe2D(
			NULL, 
			RendTarget,
			CaptureShowFlags,
			FLinearColor::Black,
			0, 
			NULL,
			FALSE,
			FALSE,
			FALSE,
			FALSE,
			WORLD_MAX,
			WORLD_MAX,
			0.0,
			ViewMatrix,
			ProjectionMatrix 
			);

		GWarn->StatusUpdatef(j * MaxSubStep + 4, VolumeList.Num() * MaxSubStep, TEXT("Rendering path map:%s..."), *AerialPhotoName);
		// render the capture without relying on regular viewport draw loop by using a scene capture proxy
		FSceneCaptureProxy SceneCaptureProxyPath(Viewport,&ViewFamily);
		SceneCaptureProxyPath.Render(CaptureProbe,TRUE);

		GWarn->StatusUpdatef(j * MaxSubStep + 5, VolumeList.Num() * MaxSubStep, TEXT("Creating path map texture:%s..."), *AerialPhotoName);

		GWarn->StatusUpdatef(j * MaxSubStep + 6, VolumeList.Num() * MaxSubStep, TEXT("Creating scaled path map texture:%s..."), *AerialPhotoName);

		/*Minimap is no longer needed, use the big map resource
		if(MapVol->MiniMapScale < 1.0f)
		{
			TArray<FColor> RawData, ScaleData;
			const UINT ScaledSizeX = Max((UINT)2, (UINT)(DstWidth * MapVol->MiniMapScale));
			const UINT ScaledSizeY = ScaledSizeX;

			FRenderTarget* rendTar = RendTarget->GameThread_GetRenderTargetResource();
			if(!rendTar)
			{
				appMsgf(AMT_OK, TEXT("The render texture accessed from the game thread is empty, cannot generate the downscaled minimap texture!"));
				continue;
			}

			rendTar->ReadPixels(RawData);
			FImageUtils::ImageResize(DstWidth, DstWidth, RawData, ScaledSizeX, ScaledSizeY, ScaleData, TRUE);

			INT newSz = RawData.Num();
			RawData.Empty();
			RawData.AddZeroed(newSz);

			for (UINT y = 0; y < ScaledSizeY; ++y)
			{
				memcpy_s((BYTE*)&RawData((DstWidth - ScaledSizeY + y) * DstWidth), newSz * sizeof(FColor),
					(BYTE*)&ScaleData(y * ScaledSizeX), ScaledSizeX * sizeof(FColor));
			}

			strTexName = FString::Printf(TEXT("%s_%s_mMap"), *MapName, *AerialPhotoName);
			FCreateTexture2DParameters Params;
			Params.bWantSourceArt = TRUE;
			Params.CompressionSettings = TC_VectorDisplacementmap;
			Params.bSRGB = FALSE;
			Params.bUseAlpha = TRUE;
			UTexture2D* tmpTex = FImageUtils::CreateTexture2D(DstWidth, DstWidth, RawData,
				Package, strTexName, RF_Public|RF_Standalone, Params);
			tmpTex->LODGroup = TEXTUREGROUP_UI;
			tmpTex->PostEditChange();

			GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, tmpTex));
		}
		else
		{
			strTexName = FString::Printf(TEXT("%s_%s_p"), *MapName, *AerialPhotoName);
			UTexture2D* tmpTex = RendTarget->ConstructTexture2D(Package, strTexName, RF_Public|RF_Standalone,
				TC_VectorDisplacementmap, NULL, TRUE);
			tmpTex->LODGroup = TEXTUREGROUP_UI;
			tmpTex->PostEditChange();
			GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, tmpTex));
		}
		*/

		//world map
		MapVol->WorldMapOffsetX = 0;
		MapVol->WorldMapOffsetY = AerialPhotoDefaultSize;//MapVol->WorldMapSizeX;
		//if(MapVol->WorldMapScale <= 1.0f)
		//{
		//	TArray<FColor> RawData, ScaleData, DstData;
		//	const UINT ScaledSizeX = Max((UINT)2, (UINT)(DstWidth * MapVol->WorldMapScale));
		//	const UINT ScaledSizeY = ScaledSizeX;

		//	FRenderTarget* rendTar = RendTarget->GameThread_GetRenderTargetResource();
		//	if(!rendTar)
		//	{
		//		appMsgf(AMT_OK, TEXT("Render texture accessed from game thread is empty, cannot generate the downscaled minimap texture!"));
		//		continue;
		//	}

		//	rendTar->ReadPixels(RawData);
		//	FImageUtils::ImageResize(DstWidth, DstHeight, RawData, ScaledSizeX, ScaledSizeY, ScaleData, TRUE);

		//	INT worldMapTileSizeX = 256;
		//	INT worldMapTileSizeY = 256;
		//	GConfig->GetInt(TEXT("Configuration"), TEXT("WorldMapTileW"), worldMapTileSizeX, GEditorIni);
		//	GConfig->GetInt(TEXT("Configuration"), TEXT("WorldMapTileH"), worldMapTileSizeY, GEditorIni);
		//	INT worldMapW = MapVol->WorldMapTileRowNum * worldMapTileSizeX;
		//	INT worldMapH = MapVol->WorldMapTileColNum * worldMapTileSizeY;

		//	INT newSz =  Max(0, worldMapW * worldMapH );
		//	DstData.Empty();
		//	DstData.AddZeroed(newSz);

		//	//
		//	MapVol->WorldMapOffsetY = ( worldMapH - ScaledSizeY ) / 2 + ScaledSizeY;
		//	INT beginRow = MapVol->WorldMapOffsetY;

		//	INT srcBeginRow = ScaledSizeY - 1;
		//	if( beginRow > worldMapH - 1 ){
		//		beginRow = worldMapH - 1;
		//		srcBeginRow = srcBeginRow - 1 - ( beginRow - worldMapH + 1 );
		//		if( srcBeginRow < 0 ){
		//			continue;
		//		}
		//	}
		//	else if( beginRow < 0 ){
		//		continue;
		//	}

		//	//
		//	MapVol->WorldMapOffsetX = ( worldMapW - ScaledSizeX ) / 2;
		//	INT beginCol = MapVol->WorldMapOffsetX;
		//	//ScaledSizeX must not exceed 66536, otherwise error
		//	INT srcBeginCol = 0;
		//	UINT cpySrcRowSize = ScaledSizeX;
		//	if( beginCol < 0 ){
		//		srcBeginCol = -beginCol;
		//		if( UINT( srcBeginCol + 1 ) > ScaledSizeX ){
		//			continue;
		//		}
		//	}
		//	else if( worldMapW - beginCol < (INT)ScaledSizeX ){
		//		if( worldMapW - beginCol <= 0 ){
		//			continue;
		//		}
		//		cpySrcRowSize = (UINT)( worldMapW - beginCol );
		//	}

		//	for (INT y = beginRow; y >= 0 && srcBeginRow >= 0; --y, --srcBeginRow)
		//	{
		//		memcpy_s((BYTE*)&DstData( y * worldMapW + beginCol), newSz * sizeof(FColor),
		//			(BYTE*)&ScaleData(srcBeginRow * ScaledSizeX + srcBeginCol), cpySrcRowSize * sizeof(FColor));
		//	}

		//	strTexName = FString::Printf(TEXT("%s_%s_wMap"), *MapName, *AerialPhotoName);
		//	FCreateTexture2DParameters Params;
		//	Params.bWantSourceArt = TRUE;
		//	Params.CompressionSettings = TC_VectorDisplacementmap;
		//	Params.bSRGB = FALSE;
		//	Params.bUseAlpha = TRUE;
		//	UTexture2D* tmpTex = FImageUtils::CreateTexture2D(worldMapW, worldMapH, DstData,
		//		Package, strTexName, RF_Public|RF_Standalone, Params);
		//	tmpTex->LODGroup = TEXTUREGROUP_UI;
		//	tmpTex->PostEditChange();

		//	GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, tmpTex));
		//}
		//else
		{
			strTexName = FString::Printf(TEXT("%s_%s_wMap"), *MapName, *AerialPhotoName);
			UTexture2D* tmpTex = RendTarget->ConstructTexture2D(Package, strTexName, RF_Public|RF_Standalone,
				TC_VectorDisplacementmap, NULL, TRUE);
			tmpTex->LODGroup = TEXTUREGROUP_UI;
			tmpTex->PostEditChange();
			GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, tmpTex));
		}
	}


	ViewportClient->ShowFlags = OldShowFlags;
	ViewportClient->ViewLocation = OldViewLocation;
	ViewportClient->ViewRotation = OldViewRotation;
	ViewportClient->OrthoZoom = OldOrthoZoom;
	ViewportClient->bInGameViewMode = OldGameViewMode;

	FString SaveFileName = AerialPhotoPath + PackageName + TEXT(".upk");

	FString ErrTxt;
	FEditorFileUtils::IsFilenameValidForSaving( SaveFileName, ErrTxt );

	//GWarn->StatusUpdatef( 0, 1, *FString::Printf( LocalizeSecure(LocalizeUnrealEd("SavingPackagef"), *PackageName )) );
	//const FScopedBusyCursor BusyCursor;
	debugf(TEXT("saving as %s"), *SaveFileName);
	if( GFileManager->IsReadOnly( *SaveFileName ) )
	{
		appMsgf( AMT_OK, *FString::Printf( LocalizeSecure(LocalizeUnrealEd("Error_CouldntWriteToFile_F"), *SaveFileName)) );
	}
	else if( !GUnrealEd->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""), *MapName, *SaveFileName) ) )
	{
		// Couldn't save.
		appMsgf( AMT_OK, *LocalizeUnrealEd("Error_CouldntSavePackage") );
	}
	else
	{
	}
	FCheckoutResult sccResult = FSourceControl::TryCheckOut(SaveFileName);
	sccResult.TryAddToDefaultChangelist();

	GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageListUI));

	for (INT j = 0; j < PylonList.Num(); ++j)
	{
		APylon* pylon = PylonList(j);
		pylon->bDonotRenderLines = ShowPylonLines(j);
	}

	for (INT j = 0; j < FogList.Num(); ++j)
	{
		AFogVolumeDensityInfo* fog = FogList(j);
		fog->DensityComponent->bEnabled = EnableFogs(j);
		FComponentReattachContext ReattachContext(fog->DensityComponent);
		//fog->ConditionalUpdateComponents();
	}

	GWarn->EndSlowTask();

	if(VolumeList.Num() == 0)
	{
		appMsgf(AMT_OK, TEXT("MapSize Volume not found. Please check whether its level is visible and whether its export property is ticked!"));
	}
}
// oscar@snda

/**
 *	Given a PrefabInstance, update the Prefab that it is an instance of using this instance.
 *	This should remove from the prefab any objects that have been deleted from this instance.
 *	We can also supply a list of objects to add to the prefab.
 *
 *	@param Instance		The PrefabInstance that we want to update using.
 *	@param NewObjects	New objects that we want to add to the prefab.
 */
void UUnrealEdEngine::Prefab_UpdateFromInstance( APrefabInstance* Instance, TArray<UObject*>& NewObjects )
{
	check(Instance);

	// Do nothing if template is NULL.
	if(!Instance->TemplatePrefab)
	{
		debugf(NAME_Warning,TEXT("Failed to update Prefab from instance '%s': reference to source Prefab lost"), *Instance->GetPathName());
		return;
	}

	// Check all objects passed in have the same Outer-most object as things in the existing PrefabInstance. Otherwise, fail.

	// First, find Outermost of the current instance.
	UObject* CheckOutermost = Instance->GetOutermost();
//	this should fix TTPRO #16925, but we'll leave the old code here just in case this ends up causing unforeseen problems down the road.
// 	for ( TMap<UObject*,UObject*>::TIterator InstIt(Instance->ArchetypeToInstanceMap); InstIt && !CheckOutermost; ++InstIt )
// 	{
// 		UObject* ArcInst = InstIt.Value();
// 		if(ArcInst)
// 		{
// 			CheckOutermost = ArcInst->GetOutermost();
// 		}
// 	}

	// Then iterate over NewObjects checking they have same Outermost.
	for(INT i=0; i<NewObjects.Num(); i++)
	{
		if( NewObjects(i) )
		{
			if( NewObjects(i)->GetOutermost() != CheckOutermost )
			{
				// If we fail, print a warning and fail.
				appMsgf( AMT_OK, *LocalizeUnrealEd("Prefab_MustHaveSameOutermost") );
				return;
			}
		}
	}

	const FScopedTransaction Transaction( *LocalizeUnrealEd(TEXT("Prefab_UndoTag_UpdatePrefab")) );

	UPrefab* Prefab = Instance->TemplatePrefab;
	Prefab->Modify();

	Instance->Modify();

	// Should never have an out-of-date instance when doing this.
	// We should have updated all instances when we opened the map in the editor.
	check(Prefab->PrefabVersion == Instance->TemplateVersion);

	// Shouldn't be doing this in-game.
	check(GIsEditor);

	/**
		Prefab Update Behaviour:

		- Delete from Prefab
			Archetype is moved from TemplateArchetype array into DeletedArchetype array.
			Any instances of this Archetype are removed from each instnace

		- Add to prefab
			Archetype is added to TemplateArchetype array.
			Instance is created for each instance.    

		- Delete from Instance
			Entry in ArchToInst map for instance points to NULL. This stop us from trying to recreate it it future.

		- Add to instance
			Makes no sense.
	*/

	//////////////////////////////////////////////////////////////////////////
	// FIRST WE SAVE OUT THE STATE OF ALL INSTANCES OF THE PREFAB

	// Build list of objects that are instances of selected prefab, but not in selected prefab instance.
	TArray<UObject*> OtherPrefabObjects;
	TArray<APrefabInstance*> OtherPrefabInstance;
	for( FActorIterator It; It; ++It )
	{
		APrefabInstance* PrefInst = Cast<APrefabInstance>(*It);
		if(	PrefInst && 
			!PrefInst->bDeleteMe && !PrefInst->IsPendingKill() &&
			PrefInst->TemplatePrefab == Prefab &&
			PrefInst != Instance)
		{
			PrefInst->Modify();
			PrefInst->SavePrefabDifferences();
		}
	}

	CheckNoSelfArchetypes(0);

	//////////////////////////////////////////////////////////////////////////
	// NOW ME MODIFY THE PREFAB ITSELF

	FRotationTranslationMatrix SelPrefabToWorld(Instance->Rotation,Instance->Location);
	FMatrix WorldToSelPrefab = SelPrefabToWorld.Inverse();

	// Iterate over the objects in the selected instance.
	for ( TMap<UObject*,UObject*>::TIterator InstIt(Instance->ArchetypeToInstanceMap); InstIt; ++InstIt )
	{
		UObject* Archetype = InstIt.Key();
		check(Archetype);

		// Archetype should either be part, or was once part, of this Prefab.
		check(Prefab->PrefabArchetypes.ContainsItem(Archetype) || Prefab->RemovedArchetypes.ContainsItem(Archetype));

		UObject* ArcInst = InstIt.Value();
		AActor* ActorInst = Cast<AActor>(ArcInst);
		ABrush* BrushInst = Cast<ABrush>(ActorInst);

		// If instance pointer is NULL - we have either deleted the instance of this object, or it was removed at some point in the past.
		// We make sure its not in the PrefabArchetypes array, and make sure it is in RemovedArchetypes.
		if(ArcInst == NULL)
		{
			Prefab->PrefabArchetypes.RemoveItem(Archetype);
			Prefab->RemovedArchetypes.AddUniqueItem(Archetype);
		}
		// If its non-null, update archetype for this Object based on the this instance properties.
		else
		{
			// Reset component stuff before turning into archetype.
			if(ActorInst)
			{
				ActorInst->ClearComponents();
			}

			// Update the archetype from the instance.
			check(ArcInst->GetArchetype() == Archetype);
			ArcInst->UpdateArchetype();

			// Re-init components again.
			if(ActorInst)
			{
				ActorInst->ForceUpdateComponents(FALSE,FALSE);
			}

			// Special Brush handling code.
			if(BrushInst)
			{
				ABrush* BrushArchetype = CastChecked<ABrush>(Archetype);
				FBSPOps::csgCopyBrush(BrushArchetype, BrushInst, BrushInst->PolyFlags, 0, FALSE, FALSE);
			}

			// Allow the objects to do any class-specific cleanup for being in a prefab.
			Archetype->OnAddToPrefab();

			// Convert location/rotation of archetype from world to local (if its an actor).
			APrefabInstance::ApplyTransformIfActor(Archetype, WorldToSelPrefab);
		}
	}

	CheckNoSelfArchetypes(1);

	// Make map of each instance in the Prefab to its archive (invert Archetype->Instance map).
	TMap<UObject*,UObject*> SelectedInstToArchMap;
	APrefabInstance::CreateInverseMap(SelectedInstToArchMap, Instance->ArchetypeToInstanceMap);

	// Mapping from old version of objects (ones that we want to add to prefab) to new ones (ones based on the new archetype).
	TMap<UObject*,UObject*> OldToNewObjMap;

	// Make array of valid objects to add to prefab.
	TArray<UObject*> AddObjects;
	for(INT i=0; i<NewObjects.Num(); i++)
	{
		if( ObjectIsValidForPrefab(NewObjects(i)) )
		{
			AddObjects.AddItem(NewObjects(i));
		}
		else
		{
			UObject* InvalidObject = NewObjects(i);

			// prefabInstance is always included in the NewObjects list and can never be added so just skip it
			if ( InvalidObject != Instance )
			{
				FString ObjectName, Reason;
				if ( InvalidObject == NULL )
				{
					Reason = TEXT("NULL Object");
				}
				else
				{
					ObjectName = InvalidObject->GetFullName();

					if ( InvalidObject->IsPendingKill() )
					{
						Reason = TEXT("Deleted");
					}

					if ( InvalidObject->IsInPrefabInstance() )
					{
						if ( Reason.Len() > 0 )
						{
							Reason += TEXT(", ");
						}

						Reason += FString::Printf(TEXT("archetype '%s' is already in prefab"), *InvalidObject->GetArchetype()->GetPathName());
					}

					AActor* InvalidActor = Cast<AActor>(InvalidObject);
					if ( InvalidActor != NULL && InvalidActor->IsABuilderBrush() )
					{
						if ( Reason.Len() > 0 )
						{
							Reason += TEXT(", ");
						}

						Reason += TEXT("Is a builder brush");
					}
				}

				if ( Reason.Len() == 0 )
				{
					Reason = TEXT("Unknown?!");
				}

				debugf(TEXT("Failed to add object %s to Prefab '%s' because it was considered invalid - Reason:%s"), *ObjectName, *Instance->GetPathName(), *Reason);
			}
		}
	}	

	// Handle adding AddObjects to the prefab.
	INT NewArcIndex = 0;
	for(INT i=0; i<AddObjects.Num(); i++)
	{
		// Check its not already part of the Prefab, or another prefab..
		UObject* NewObject = AddObjects(i);
		AActor* NewActor = Cast<AActor>(NewObject);
		ABrush* NewBrush = Cast<ABrush>(NewActor);
		UObject** FindArcRef = SelectedInstToArchMap.Find(NewObject);
		if(!FindArcRef)
		{
			// If not, create a new archetype for it and add to array.
			// Make name for the new archetype. Use the new prefab version number to get a new name.
			FString ArchetypeName = FString::Printf( TEXT("%s_%dArc%d"), *Prefab->GetName(), Prefab->PrefabVersion+1, NewArcIndex );
			NewArcIndex++;

			// Reset components before archetypification.
			if(NewActor)
			{
				NewActor->ClearComponents();
			}

			// Create new archetype, with an outer the same as the new Prefabs Outer.
			UObject* NewArchetype = NewObject->CreateArchetype(*ArchetypeName, Prefab);
			check( NewArchetype->HasAllFlags(RF_Public|RF_ArchetypeObject) );

			// Then init components again.
			if(NewActor)
			{
				NewActor->ForceUpdateComponents(FALSE,FALSE);
			}

			// Special Brush handling code.
			if(NewBrush)
			{
				ABrush* BrushArchetype = CastChecked<ABrush>(NewArchetype);
				FBSPOps::csgCopyBrush(BrushArchetype, NewBrush, NewBrush->PolyFlags, 0, FALSE, FALSE);
			}

			// Allow the objects to do any class-specific cleanup for being in a prefab.
			NewArchetype->OnAddToPrefab();

			// Fix up location/rotation to be relative to prefab origin.
			APrefabInstance::ApplyTransformIfActor(NewArchetype, WorldToSelPrefab);

			// Add Archetype to prefabs array.
			Prefab->PrefabArchetypes.AddItem(NewArchetype);

			// Now delete this instance of the object, and replace with an instance of the new Archetype.
			// This code should basically look the same as the code in 
			UObject* NewArchInst = NULL;
			if(NewActor)
			{
				NewActor->Modify();
				GWorld->EditorDestroyActor( NewActor, TRUE );
	
				AActor* NewArchActor = CastChecked<AActor>(NewArchetype); // If NewObject is an Actor, NewArchetype should be as well.
				FRotationTranslationMatrix RelTM(NewArchActor->Rotation,NewArchActor->Location);
				FMatrix WorldTM = RelTM * SelPrefabToWorld;

				// Spawn instance of new Archetype.
				NewArchInst = GWorld->SpawnActor(NewArchetype->GetClass(), NAME_None, WorldTM.GetOrigin(), WorldTM.Rotator(), NewArchActor);

				// If this is a Brush, we have to use special function to copy the UModel.
				APrefabInstance::CopyModelIfBrush(NewArchInst, NewArchetype);
			}
			else
			{
				// @todo: Handle things other than Actors.
			}

			// Update arc->inst map.
			Instance->ArchetypeToInstanceMap.Set(NewArchetype, NewArchInst);

			// Update inst->arc map.
			SelectedInstToArchMap.Set(NewArchInst, NewArchetype);

			// Update map of old to new version of the object.
			OldToNewObjMap.Set(NewObject, NewArchInst);
		}
	}

	// Then change all references in the prefab from instance refs to archetype refs.
	UPrefab::ResolveInterDependencies(SelectedInstToArchMap, Instance->GetOutermost() != Prefab->GetOutermost());

	// Update the Kismet sequence in the Prefab from this instances version.

	// If this PrefabInstance has a Kismet sequence, copy to prefab.
	if(Instance->SequenceInstance)
	{
		check(Prefab->PrefabSequence); // If there is an instance of a sequence, there must have been one in the Prefab to instance it from.

		// First we want to update the sequence so it points to the new versions of any objects that we just added.
		FArchiveReplaceObjectRef<UObject> ReplaceAr(Instance->SequenceInstance, OldToNewObjMap, false, true, true);

		// Then copy sequence into the prefab, turning any instance references into archetype references.
		Prefab->CopySequenceIntoPrefab(Instance->SequenceInstance, SelectedInstToArchMap);
	}
	// If it doesn't, see if we can find one for it. 
	else 
	{
		// Find the Kismet sequence that relates to the objects we have in the prefab.
		TArray<UObject*> AllInstanceObjects;
		for ( TMap<UObject*,UObject*>::TIterator InstIt(Instance->ArchetypeToInstanceMap); InstIt; ++InstIt )
		{
			UObject* ArcInst = InstIt.Value();
			if(ArcInst)
			{
				AllInstanceObjects.AddItem(ArcInst);
			}
		}

		// Add any new objects we are adding to Prefab. Need to find the sequence that includes those as well.
		for(INT i=0; i<AddObjects.Num(); i++)
		{
			AllInstanceObjects.AddUniqueItem(AddObjects(i));
		}

		// Find Kismet sequence which includes all refs to parts of the Prefab.
		USequence* PrefabSeq = FindPrefabSequence(AllInstanceObjects);

		// If we can, copy into Prefab
		if(PrefabSeq)
		{
			UBOOL bUseSeq = appMsgf( AMT_YesNo, LocalizeSecure(LocalizeUnrealEd("Prefab_FoundSeqYesNo"), *PrefabSeq->GetName(), *PrefabSeq->GetName()) );
			if(bUseSeq)
			{
				// Check there are no references in this sequence to stuff outside the Prefab.
				UBOOL bSeqOK = SequenceReferencesOnlyObjs(PrefabSeq, AllInstanceObjects);
				if(bSeqOK)
				{
					// Copy the sequence we found.
					Prefab->CopySequenceIntoPrefab(PrefabSeq, SelectedInstToArchMap);

					// Now we remove the sequence from the level.
					RemoveSequenceFromParent(PrefabSeq);

					// Now instance it in the right place. This handily updates the kismet window as well.
					Instance->InstanceKismetSequence(Prefab->PrefabSequence, FString(TEXT("")));
				}
				// .. If there are use no sequence.
				else
				{
					Prefab->PrefabSequence = NULL;
				}
			}
			else
			{
				Prefab->PrefabSequence = NULL;
			}
		}
		// If we can't, remove the one from the Prefab if there is one.
		else
		{
			Prefab->PrefabSequence = NULL;			
		}
	}

	// Update the preview image as well.
	SelectNone( TRUE, TRUE );
	Prefab_CreatePreviewImage(Prefab);

	// Increment prefab version number.
	(Prefab->PrefabVersion)++;

	// Selected PrefabInstance is correct- set its TemplateVersion accordingly.
	(Instance->TemplateVersion)++;

	//////////////////////////////////////////////////////////////////////////
	// PREFAB IS NOW UPDATED - PROPAGATE CHANGES TO ALL INSTANCES

	CheckNoSelfArchetypes(2);

	// Now we need to handle added/removed objects in other instances of this prefab.
	for( FActorIterator It; It; ++It )
	{
		APrefabInstance* PrefInst = Cast<APrefabInstance>(*It);
		if(	PrefInst && 
			!PrefInst->bDeleteMe && !PrefInst->IsPendingKill() &&
			PrefInst->TemplatePrefab == Prefab &&
			PrefInst != Instance )
		{
			// Make sure there are no Kismet windows open on one of the sequences used by this PrefabInstance, as it might be removed.			
			WxKismet::EnsureNoKismetEditingSequence(PrefInst->SequenceInstance);
	
			// Actually do PrefabInstance update.
			PrefInst->UpdatePrefabInstance( GetSelectedActors() );
		}
	}

	CheckNoSelfArchetypes(3);

	// Indicate resource has changed.
	Prefab->MarkPackageDirty();

	// Update browsers to reflect change.
	GCallbackEvent->Send( CALLBACK_RefreshEditor_AllBrowsers );
}

/**
 *	Convert the PrefabInstance into a set of regular actors.
 *	Also deletes the PrefabInstance actor as well.
 *	This basically changes each object instances archetype to being the class default object instead of the 
 *	archetype in the Prefab.
 *	It also renames the objects so they don't have the name of the Prefab in their name any more.
 */
void UUnrealEdEngine::Prefab_ConvertToNormalActors( APrefabInstance* Instance )
{
	const FScopedTransaction Transaction( *LocalizeUnrealEd(TEXT("Prefab_ToNormalActors")) );

	// Iterate over each object instance in this PrefabInstance
	for ( TMap<UObject*,UObject*>::TIterator InstIt(Instance->ArchetypeToInstanceMap); InstIt; ++InstIt )
	{
		UObject* ObjInst = InstIt.Value();
		if(ObjInst)
		{
			// Find all objects that are within this object, and change back to using class default objects.
			// We need to do this so we get all the Components within Actors and such.
			TArray<UObject*> ObjToRename;
			for(FObjectIterator It; It; ++It)
			{
				UObject* Obj = *It;
				if(Obj == ObjInst || Obj->IsIn(ObjInst))
				{
					ObjToRename.AddItem(Obj);
				}
			}

			// We build the list first to avoid the object iterator hitting newly renamed objects!
			for(INT i=0; i<ObjToRename.Num(); i++)
			{
				UObject* Obj = ObjToRename(i);

				// Back up for undo
				Obj->Modify();

				/*
				here we have two choices:
				A) we can reset the archetype to be the equivalent of placing a completely new actor
				B) we can remove one "layer" of archetypes, by changing the object's archetype to point to its archetype's archetype

				we'll go with A since this is closest to the existing behavior
				*/

				// change the object's archetype to point to whatever the archetype would be for a newly placed actor
				UObject* NewArchetype = Obj->GetArchetype();
				while ( NewArchetype->GetArchetype() && !NewArchetype->IsTemplate(RF_ClassDefaultObject) )
				{
					NewArchetype = NewArchetype->GetArchetype();
				}

				Obj->SetArchetype( NewArchetype );
			}
		}
	}

	// Finally delete the PrefabInstance actor.
	GetSelectedActors()->Modify();
	GetSelectedActors()->Deselect( Instance );
	GWorld->EditorDestroyActor( Instance, TRUE );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// EDACT... FUNCTIONS
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/**
 * Create an archetypes of the selected actor, and replace that actors with an instance of the new archetype.
 */
void UUnrealEdEngine::edactArchetypeSelected()
{
	// Get the selected Actor.
	AActor* ArchActor = GetSelectedActors()->GetTop<AActor>();
	if(!ArchActor || ArchActor->IsPendingKill())
	{
		return;
	}

	const FScopedTransaction Transaction( *LocalizeUnrealEd(TEXT("Archetype_Create")) );

	// This will do the 'enter a name' dialog etc.
	FString ArchetypeName, GroupName, PackageName;
	SetPackageAndGroupNameFromGB( PackageName, GroupName );

	UObject* ArchetypeObj = Archetype_CreateFromObject(ArchActor, ArchetypeName, PackageName, GroupName);

	// If we succesfully made a bew Archetype...
	if(ArchetypeObj)
	{
		// Clear any references in this archetype to non-public in other packages (eg. this will clear a Base pointer, or a ref to another actor).
		TMap<UObject*,UObject*> TempMap;
		FArchiveReplaceObjectRef<UObject> ReplaceAr(ArchetypeObj, TempMap, TRUE, TRUE, TRUE);

		// Replace the existing actor with a new actor based on the new archetype.  Replacing
		// the actor rather than simply changing its ObjectArchetype ensures that all of the
		// actor's components have the correct templates.
		AActor* NewActor = ReplaceActor( ArchActor, ArchetypeObj->GetClass(), ArchetypeObj, FALSE );

		// Update property windows.
		UpdatePropertyWindows();
	}

	NoteSelectionChange();
}

/**
 *  Update archetype of the selected actor
 */
void UUnrealEdEngine::edactUpdateArchetypeSelected()
{
	AActor* ArchActor = GetSelectedActors()->GetTop<AActor>();
	if(!ArchActor || ArchActor->IsPendingKill())
	{
		return;
	}

	check(ArchActor->GetArchetype() != ArchActor->GetClass()->GetDefaultActor());
	// Don't update if the actor is based on the default archetype
	ArchActor->UpdateArchetype();
}

/** Create a prefab from the selected actors, and replace those actors with an instance of that prefab. */
void UUnrealEdEngine::edactPrefabSelected()
{
	// Make list of selected actors.
	TArray<UObject*> SelectedObjects;
	for ( FSelectionIterator It( GetSelectedActorIterator() ) ; It ; ++It )
	{
		AActor* Actor = static_cast<AActor*>( *It );
		checkSlow( Actor->IsA(AActor::StaticClass()) );

		SelectedObjects.AddItem(Actor);
	}

	Prefab_NewPrefab(SelectedObjects);
}

/** Add the selected prefab at the clicked location. */
void UUnrealEdEngine::edactAddPrefab()
{
	// Get the prefab that we want to create an instance of.
	UPrefab* Prefab = GetSelectedObjects()->GetTop<UPrefab>();
	if( Prefab )
	{
		// Snap clicked location.
		Constraints.Snap( ClickLocation, FVector(0, 0, 0) );

		// Create a new instance at the chosen location.
		const FScopedTransaction Transaction( *LocalizeUnrealEd(TEXT("Prefab_UndoTag_AddPrefab")) );

		// First, create a new PrefabInstance actor. We have to do this a little differently then Prefab_InstancePrefab since we position the prefab differently if the user clicked on the background
		APrefabInstance* PrefabInstance = CastChecked<APrefabInstance>( GWorld->SpawnActor(APrefabInstance::StaticClass(), NAME_None, ClickLocation, FRotator(0,0,0)) );
		
		if ( PrefabInstance )
		{
			// Determine if we clicked on the background.
			const FIntPoint& CurrentMousePos = GCurrentLevelEditingViewportClient->CurrentMousePos;
			HHitProxy* HitProxy = GCurrentLevelEditingViewportClient->Viewport->GetHitProxy( CurrentMousePos.X, CurrentMousePos.Y );
			// If the hit proxy is NULL we clicked on the background
			UBOOL bClickedOnBackground = (HitProxy == NULL);

			if( bClickedOnBackground && GCurrentLevelEditingViewportClient->ViewportType == LVT_Perspective )
			{
				// Find the location the user clicked in the viewport
				FSceneViewFamilyContext ViewFamily(
					GCurrentLevelEditingViewportClient->Viewport, GCurrentLevelEditingViewportClient->GetScene(),
					GCurrentLevelEditingViewportClient->ShowFlags,
					GWorld->GetTimeSeconds(),
					GWorld->GetDeltaSeconds(),
					GWorld->GetRealTimeSeconds(),
					GCurrentLevelEditingViewportClient->IsRealtime()
					);
				FSceneView* View = GCurrentLevelEditingViewportClient->CalcSceneView( &ViewFamily );
				FViewportCursorLocation CursorLocation(View, GCurrentLevelEditingViewportClient, CurrentMousePos.X, CurrentMousePos.Y);

				// move the prefab actor
				MoveActorInFrontOfCamera( *PrefabInstance, CursorLocation.GetOrigin(), CursorLocation.GetDirection() );
			}
			
			// Instance the prefab actor.  Do this after we determined if we clicked on the background.  This way all actors in the prefab will have the same relative position.
			PrefabInstance->InstancePrefab(Prefab);

			SelectNone( FALSE, TRUE );
			SelectActor( PrefabInstance, TRUE, FALSE, FALSE );
			NoteSelectionChange();
		}
	}
}

/** Take the selected PrefabInstance, find the Prefab that its an instance of, and updated to match the instance. */
void UUnrealEdEngine::edactUpdatePrefabFromInstance()
{
	APrefabInstance* PrefabInstance = GetSelectedActors()->GetTop<APrefabInstance>();
	if(PrefabInstance && PrefabInstance->TemplatePrefab)
	{
		if ( !WarnAboutHiddenLevels( FALSE, *LocalizeUnrealEd("UpdatePrefabHiddenLevelsQ") ) )
		{
			return;
		}

		TArray<UObject*> SelectedObjects;

		//@{
		//@fixme - workaround for selection set being modified by clicking OK on this dialog if the OK button is over the same viewport used for selecting the actors
		USelection* ActorSelectionSet = GetSelectedActors();
		ActorSelectionSet->GetSelectedObjects<UObject>(SelectedObjects);

		const UBOOL bProceed = appMsgf( AMT_YesNo, LocalizeSecure(LocalizeUnrealEd("Prefab_UpdatePrefabAreYouSure"), *PrefabInstance->TemplatePrefab->GetName()) );
		if(!bProceed)
		{
			return;
		}

		ActorSelectionSet->BeginBatchSelectOperation();
		ActorSelectionSet->DeselectAll();
		for ( INT SelectionIndex = 0; SelectionIndex < SelectedObjects.Num(); SelectionIndex++ )
		{
			ActorSelectionSet->Select(SelectedObjects(SelectionIndex));
		}
		ActorSelectionSet->EndBatchSelectOperation();
		//@}

		TArray<UObject*> NewObjects;
		UBOOL bCanAddToPrefabInstance = FALSE;
		for ( FSelectionIterator It( GetSelectedActorIterator() ) ; It ; ++It )
		{
			AActor* Actor = static_cast<AActor*>( *It );
			checkSlow( Actor->IsA(AActor::StaticClass()) );

			// Do not update the prefab with any invalid actors
			bCanAddToPrefabInstance = ObjectIsValidForPrefab( Actor );

			if( bCanAddToPrefabInstance )
			{
				NewObjects.AddItem( Actor );
			}

		}

		debugf(TEXT("Updating prefab '%s' from instance '%s' with '%i' NewObjects"), *PrefabInstance->TemplatePrefab->GetPathName(), *PrefabInstance->GetPathName(), NewObjects.Num());
		for ( INT i = 0; i < NewObjects.Num(); i++ )
		{
			debugf(TEXT("*** %i) %s"), i, *NewObjects(i)->GetFullName());
		}
		Prefab_UpdateFromInstance(PrefabInstance, NewObjects);
	}
}

/** Reset a prefab instance from the prefab. */
void UUnrealEdEngine::edactResetInstanceFromPrefab()
{
	APrefabInstance* FirstPrefabInstance = GetSelectedActors()->GetTop<APrefabInstance>();
	if( FirstPrefabInstance )
	{
		const UBOOL bProceed = appMsgf( AMT_YesNo, *LocalizeUnrealEd("Prefab_ResetPrefabAreYouSure") );
		if(!bProceed)
		{
			return;
		}

		const FScopedTransaction Transaction( *LocalizeUnrealEd(TEXT("Prefab_UndoTag_ResetPrefab")) );

		for( FSelectionIterator It( GetSelectedActorIterator() ) ; It ; ++It )
		{
			APrefabInstance* PrefabInstance = Cast<APrefabInstance>(*It);
			if(	PrefabInstance != NULL && PrefabInstance->TemplatePrefab )
			{
				UPrefab* Prefab = PrefabInstance->TemplatePrefab;
				PrefabInstance->GetOutermost()->MarkPackageDirty();
				PrefabInstance->DestroyPrefab(GetSelectedActors());
				PrefabInstance->InstancePrefab(Prefab);
			}
		}
	}
}

/** Convert a prefab instance back into normal actors. */
void UUnrealEdEngine::edactPrefabInstanceToNormalActors()
{
	APrefabInstance* FirstPrefabInstance = GetSelectedActors()->GetTop<APrefabInstance>();
	if( FirstPrefabInstance )
	{
		for( FSelectionIterator It( GetSelectedActorIterator() ) ; It ; ++It )
		{
			APrefabInstance* PrefabInstance = Cast<APrefabInstance>(*It);
			if(PrefabInstance)
			{
				Prefab_ConvertToNormalActors(PrefabInstance);
			}
		}
	}
}

/** Select all Actors that are in the same PrefabInstance as any selected Actors. */
void UUnrealEdEngine::edactSelectPrefabActors()
{
	// Take copy of selection set.
	TArray<AActor*> SelActors;

	for ( FSelectionIterator It( GetSelectedActorIterator() ) ; It ; ++It )
	{
		AActor* Actor = static_cast<AActor*>( *It );
		checkSlow( Actor->IsA(AActor::StaticClass()) );

		SelActors.AddItem( Actor );
	}

	// Empty selection
	SelectNone( TRUE, TRUE );

	// For each selected actor, find its prefab instance, and then select everything in it.
	for( INT i=0; i < SelActors.Num(); i++ )
	{
		AActor* Actor = SelActors(i);
		APrefabInstance* PrefInst = Actor->FindOwningPrefabInstance();
		if(PrefInst)
		{
			// Select the PrefabInstance itself
			SelectActor(PrefInst, 1, NULL, 0);

			// Then select all the actors in the Prefab.
			TArray<AActor*> PrefabActors;
			PrefInst->GetActorsInPrefabInstance(PrefabActors);
			for(INT i=0; i<PrefabActors.Num(); i++)
			{
				SelectActor( PrefabActors(i), 1, NULL, 0 );
			}
		}
	}

	// Tell the editor we changes the selection.
	NoteSelectionChange();		
}
