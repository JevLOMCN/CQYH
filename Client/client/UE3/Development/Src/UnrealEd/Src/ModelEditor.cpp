/*=============================================================================
	AnimSetViewerMain.cpp: AnimSet viewer main
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

	This code contains embedded portions of source code from dqconv.c Conversion routines between (regular quaternion, translation) and dual quaternion, Version 1.0.0, Copyright ?2006-2007 University of Dublin, Trinity College, All Rights Reserved, which have been altered from their original version.

	The following terms apply to dqconv.c Conversion routines between (regular quaternion, translation) and dual quaternion, Version 1.0.0:

	This software is provided 'as-is', without any express or implied warranty.  In no event will the author(s) be held liable for any damages arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
=============================================================================*/

#include "UnrealEd.h"
#include "EnginePhysicsClasses.h"
#include "EngineAnimClasses.h"
#include "ModelEditor.h"
#include "MouseDeltaTracker.h"
#include "PropertyWindow.h"
#include "DlgGenericComboEntry.h"
#include "SocketManager.h"
#include "DlgAnimationCompression.h"
#include "AnimationUtils.h"
#include "UnSkelRenderPublic.h"
#include "UnSkeletalMesh.h"
#include "GPUSkinVertexFactory.h"
#include "UnSkeletalMeshSorting.h"
#include "SkelImport.h"
#include "EngineAudioDeviceClasses.h"
// [ming@snda
#include "DlgMoveAssets.h"
#include "UnSkeletalMeshMerge.h"
#include "BusyCursor.h"
// ming@snda]

#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"

#if WITH_SIMPLYGON
#include "SkeletalMeshSimplificationWindow.h"
#endif // #if WITH_SIMPLYGON

#if WITH_MANAGED_CODE
#include "FileSystemNotificationShared.h"
#endif

static const FColor ExtraMesh1WireColor = FColor(255,215,215,255);
static const FColor ExtraMesh2WireColor = FColor(215,255,215,255);
static const FColor ExtraMesh3WireColor = FColor(215,215,255,255);

//Bruce@snda 
static const FColor ExtraMesh4WireColor = FColor(255,20,215,255);
static const FColor ExtraMesh5WireColor = FColor(215,215,215,255);
static const FColor ExtraMesh6WireColor = FColor(225,215,205,255);
static const FColor ExtraMesh7WireColor = FColor(255,205,15,255);
static const FColor ExtraMesh8WireColor = FColor(15,255,215,255);
static const FColor ExtraMesh9WireColor = FColor(215,21,255,255);
static const FColor ExtraMesh10WireColor = FColor(105,215,25,255);
static const FColor ExtraMesh11WireColor = FColor(255,215,215,255);
static const FColor ExtraMesh12WireColor = FColor(205,205,215,255);
static const FColor ExtraMesh13WireColor = FColor(215,215,255,255);
static const FColor ExtraMesh14WireColor = FColor(255,20,215,255);
static const FColor ExtraMesh15WireColor = FColor(215,25,115,255);
static const FColor ExtraMesh16WireColor = FColor(215,215,205,255);
static const FColor ExtraMesh17WireColor = FColor(255,201,15,255);
static const FColor ExtraMesh18WireColor = FColor(15,255,15,255);
static const FColor ExtraMesh19WireColor = FColor(21,210,255,255);
static const FColor ExtraMesh20WireColor = FColor(10,250,25,255);
//Bruce@snda 

static const FLOAT	AnimSetViewer_TranslateSpeed = 0.25f;
static const FLOAT	AnimSetViewer_RotateSpeed = 0.02f;
static const FLOAT	AnimSetViewer_LightRotSpeed = 40.0f;
static const FLOAT	AnimSetViewer_WindStrengthSpeed = 0.1f;
#if WITH_APEX
static const FLOAT  AnimSetViewer_WindVelocityBlendTimeSpeed = 0.001f;
#endif


FEditorModelViewportClient::FEditorModelViewportClient( WxModelViewer* InASV ):
	PreviewScene(FRotator(-8192,-8192,0),0.25f,1.f,TRUE)
{
	ModelViewer = InASV;

	// Setup defaults for the common draw helper.
	DrawHelper.bDrawPivot = FALSE;
	DrawHelper.bDrawWorldBox = FALSE;
	DrawHelper.bDrawKillZ = FALSE;
	DrawHelper.GridColorHi = FColor(80,80,80);
	DrawHelper.GridColorLo = FColor(72,72,72);
	DrawHelper.PerspectiveGridSize = 32767;

	// Create AnimNodeSequence for previewing animation playback
	InASV->PreviewAnimNode = ConstructObject<UAnimNodeSequence>(UAnimNodeSequence::StaticClass());
	InASV->PreviewAnimNode->bEditorOnlyAddRefPoseToAdditiveAnimation = TRUE;

	// Mirror node for testing that.
	InASV->PreviewAnimMirror = ConstructObject<UAnimNodeMirror>(UAnimNodeMirror::StaticClass());
	InASV->PreviewAnimMirror->Children(0).Anim = InASV->PreviewAnimNode;
	InASV->PreviewAnimMirror->bEnableMirroring = FALSE; // Default to mirroring off.

	// Morph pose node for previewing morph targets.
	InASV->PreviewMorphPose = ConstructObject<UMorphNodeMultiPose>(UMorphNodeMultiPose::StaticClass());

	// Finally, AnimTree to hold them all
	InASV->PreviewAnimTree = ConstructObject<UAnimTree>(UAnimTree::StaticClass());
	InASV->PreviewAnimTree->Children(0).Anim = InASV->PreviewAnimMirror;
	InASV->PreviewAnimTree->RootMorphNodes.AddItem(InASV->PreviewMorphPose);


	// Create a skel control to handle bone manipulation
	FSkelControlListHead SkelControlEntry(EC_EventParm);
	SkelControlEntry.BoneName = NAME_None;
	
	USkelControlSingleBone* SkelControl = ConstructObject<USkelControlSingleBone>(USkelControlSingleBone::StaticClass());

	SkelControl->bApplyTranslation = TRUE;
	SkelControl->bApplyRotation = TRUE;
	SkelControl->bAddTranslation = FALSE;
	SkelControl->bAddRotation = FALSE;
	SkelControl->BoneTranslationSpace = BCS_BoneSpace;
	SkelControl->BoneRotationSpace = BCS_BoneSpace;

	SkelControlEntry.ControlHead = SkelControl;
	InASV->PreviewAnimTree->SkelControlLists.AddItem(SkelControlEntry);

	// Create SkeletalMeshComponent for rendering skeletal mesh
	InASV->PreviewSkelComp = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass());
	InASV->PreviewSkelComp->Animations = InASV->PreviewAnimTree;
	//InASV->PreviewSkelComp->AnimSetViewerPtr = InASV;
	// Setting bUpdateSkelWhenNotRendered to true below forces updates to the renderer thread on each frame.
	// [[ When this is not set, the preview rendering in the ASV may fail because the code in
	//    USkeletalMeshComponent::Tick that tries to determine whether the component was recently rendered
	//    seems to fail due to bad timing data. ]]
	InASV->PreviewSkelComp->bUpdateSkelWhenNotRendered = TRUE;
	// Don't cache animations by default.
	InASV->PreviewSkelComp->bCacheAnimSequenceNodes = FALSE;
	PreviewScene.AddComponent(InASV->PreviewSkelComp,FMatrix::Identity);

	InASV->CollisionBoxComponent = ConstructObject<UCylinderComponent>(UCylinderComponent::StaticClass());
	InASV->CollisionBoxComponent->bDrawNonColliding = TRUE;
	InASV->CollisionBoxComponent->bDrawBoundingBox = FALSE;

	PreviewScene.AddComponent(InASV->CollisionBoxComponent, FMatrix::Identity);

	LineBatcher = ConstructObject<ULineBatchComponent>(ULineBatchComponent::StaticClass());
	PreviewScene.AddComponent(LineBatcher,FMatrix::Identity);

	ShowFlags = SHOW_DefaultEditor;

	// Set the viewport to be fully lit.
	ShowFlags &= ~SHOW_ViewMode_Mask;
	ShowFlags |= SHOW_ViewMode_Lit;
	ShowFlags |= SHOW_Collision;

	bShowBoneNames = FALSE;
	bShowFloor = FALSE;
	bShowSockets = FALSE;
	bShowMorphKeys = FALSE;

	bManipulating = FALSE;
	SocketManipulateAxis = AXIS_None;
	BoneManipulateAxis = AXIS_None;
	DragDirX = 0.f;
	DragDirY = 0.f;
	WorldManDir = FVector(0.f);
	LocalManDir = FVector(0.f);

	NearPlane = 1.0f;

	SetRealtime( TRUE );

	bAllowMayaCam = FALSE;

	bTriangleSortMode = FALSE;

	// Clothing may have been created in another RBPhysScene; release it so it can be created in the right one
	InASV->PreviewSkelComp->ReleaseApexClothing();
	InASV->PreviewSkelComp->InitApexClothing(InASV->RBPhysScene);

	// Override global realtime audio settings
	bAudioRealtimeOverride = TRUE;
	bWantAudioRealtime = FALSE;

#if WITH_SIMPLYGON
	SimplygonLogo = LoadObject<UTexture2D>(NULL, TEXT("EditorResources.SimplygonLogo"), NULL, LOAD_None, NULL);
#endif // #if WITH_SIMPLYGON
}

FEditorModelViewportClient::~FEditorModelViewportClient()
{
}

void FEditorModelViewportClient::Serialize(FArchive& Ar)
{ 
	Ar << Input; 
	Ar << PreviewScene;
}

FLinearColor FEditorModelViewportClient::GetBackgroundColor()
{
	return FColor(64,64,64);
}


#define HITPROXY_SOCKET	1
#define HITPROXY_BONE   2
void FEditorModelViewportClient::Draw(const FSceneView* View,FPrimitiveDrawInterface* PDI)
{
	// Draw common controls.
	DrawHelper.Draw(View, PDI);
	FEditorLevelViewportClient::Draw(View, PDI);
}

/*-----------------------------------------------------------------------------
FEditorModelViewportClient
-----------------------------------------------------------------------------*/

static FVector2D ClampUVRange(FLOAT U, FLOAT V)
{
	// This handles both the + and - values correctly by putting all values between positive 0 and 1
	if(abs(U) > 1.0f)
	{
		U -= appFloor(U);
	}

	if(abs(V) > 1.0f)
	{
		V -= appFloor(V);
	}

	return FVector2D(U, V);
}

void FEditorModelViewportClient::SetCameraPosition(INT InPositionType)
{
	PositionType = InPositionType;
	CameraStartTime = appSeconds();
}

void FEditorModelViewportClient::SetCameraPosition(FVector& InLocation, FRotator& InRotation)
{
	TeleportViewportCamera(InLocation, InRotation);
}


void FEditorModelViewportClient::Draw(FViewport* Viewport, FCanvas* Canvas)
{
	// Do main viewport drawing stuff.
	FEditorLevelViewportClient::Draw(Viewport, Canvas);

}

/** Util for drawing wind direction. */
void FEditorModelViewportClient::DrawWindDir(FViewport* Viewport, FCanvas* Canvas)
{
	FRotationMatrix ViewTM( this->ViewRotation );

	const INT SizeX = Viewport->GetSizeX();
	const INT SizeY = Viewport->GetSizeY();

	const FIntPoint AxisOrigin( 80, SizeY - 30 );
	const FLOAT AxisSize = 25.f;

	FVector AxisVec = AxisSize * ViewTM.InverseTransformNormal( ModelViewer->WindRot.Vector() );
	FIntPoint AxisEnd = AxisOrigin + FIntPoint( AxisVec.Y, -AxisVec.Z );
	DrawLine2D( Canvas, AxisOrigin, AxisEnd, FColor(255,255,128) );

	FString WindString = FString::Printf( TEXT("Wind: %4.1f"), ModelViewer->WindStrength );
	DrawString( Canvas, 65, SizeY - 15, *WindString, GEngine->SmallFont, FColor(255,255,128) );
	// Display windvelocityblendtime in the preview window for Apex Clothing Wind
#if WITH_APEX
	if(ModelViewer->PreviewSkelComp->GetApexClothing() != NULL)
	{
		FString WindVelBlendTimeString = FString::Printf( TEXT("BlendTime: %4.3f"), ModelViewer->WindVelocityBlendTime);
		DrawString( Canvas, 150, SizeY - 15, *WindVelBlendTimeString, GEngine->SmallFont, FColor(255,255,128));
	}
#endif
}

void FEditorModelViewportClient::Tick(FLOAT DeltaSeconds)
{
	FEditorLevelViewportClient::Tick(DeltaSeconds);
	ModelViewer->TickViewer(DeltaSeconds * ModelViewer->PlaybackSpeed);

	const FLOAT CameraRotationTime = 1.0;
	DOUBLE CurrentTime = appSeconds();
	if (CurrentTime - CameraStartTime <= CameraRotationTime)
	{
		FVector NewLocation;
		FRotator NewRotation;

		INT DesYaw = ViewRotation.Yaw;
		DesYaw = (DesYaw & 0xffff) - (16384.f * DeltaSeconds / CameraRotationTime);

		NewRotation.Yaw =  DesYaw & 0xffff;
		NewRotation.Pitch = -5461;
		NewRotation.Roll = 0;

		NewLocation = NewRotation.Vector().SafeNormal() * (-1000.f);
		SetCameraPosition(NewLocation, NewRotation);
	}
}

UBOOL FEditorModelViewportClient::InputKey(FViewport* Viewport, INT ControllerId, FName Key, EInputEvent Event,FLOAT AmountDepressed,UBOOL Gamepad)
{
	//const INT HitX = Viewport->GetMouseX();
	//const INT HitY = Viewport->GetMouseY();
	//if (Key == KEY_RightMouseButton)
	//{
	//	if (Event == IE_Pressed)
	//	{
	//		MousePressX = Viewport->GetMouseX();
	//		MousePressY = Viewport->GetMouseY();
	//	}
	//	if (Event == IE_Released)
	//	{
	//		INT x, y;
	//		x = Viewport->GetMouseX();
	//		y = Viewport->GetMouseY();
	//		if ((x - MousePressX) * (x - MousePressX) + (y - MousePressY) * (y - MousePressY) < 3)
	//		{
	//			ModelViewer->PopupAddMenu(x, y);
	//		}
	//		MousePressX = MousePressY = 0;
	//	}
	//}

	return TRUE;
}

/** Handles mouse being moved while input is captured - in this case, while a mouse button is down. */
UBOOL FEditorModelViewportClient::InputAxis(FViewport* Viewport, INT ControllerId, FName Key, FLOAT Delta, FLOAT DeltaTime, UBOOL bGamepad)
{

	// Look at which axis is being dragged and by how much
	FLOAT DragX = (Key == KEY_MouseX) ? Delta : 0.f;
	FLOAT DragY = (Key == KEY_MouseY) ? Delta : 0.f;

	MouseDeltaTracker->AddDelta( this, Key, Delta, 0 );
	const FVector DragDelta = MouseDeltaTracker->GetDelta();

	GEditor->MouseMovement += DragDelta;

	if( !DragDelta.IsZero() )
	{
		// Convert the movement delta into drag/rotation deltas
		if ( bAllowMayaCam && GEditor->bUseMayaCameraControls )
		{
			FVector TempDrag;
			FRotator TempRot;
			InputAxisMayaCam( Viewport, DragDelta, TempDrag, TempRot );
		}
		else
		{
			FVector Drag;
			FRotator Rot;
			FVector Scale;
			MouseDeltaTracker->ConvertMovementDeltaToDragRot( this, DragDelta, Drag, Rot, Scale );
			MoveViewportCamera( Drag, Rot );
		}

		MouseDeltaTracker->ReduceBy( DragDelta );
	}

	Viewport->Invalidate();

	return TRUE;
}

void FEditorModelViewportClient::MouseMove(FViewport* Viewport,INT x, INT y)
{
	ModelViewer->MouseOverSortStripIndex = INDEX_NONE;
	ModelViewer->bSortStripMoveForward = ModelViewer->SelectedSortStripIndex != INDEX_NONE && Viewport->KeyState(KEY_F);
	ModelViewer->bSortStripMoveBackward = ModelViewer->SelectedSortStripIndex != INDEX_NONE && Viewport->KeyState(KEY_B);
}

void FEditorModelViewportClient::AttachSkeletalMesh( USkeletalMeshComponent* InParent, USkeletalMesh* ChildMesh, const FTranslationMatrix& ftm )
{
	if( InParent == NULL || ChildMesh == NULL )
		return;

	USkeletalMeshComponent* smc = Cast<USkeletalMeshComponent>(
		UObject::StaticConstructObject(
		USkeletalMeshComponent::StaticClass(), UObject::GetTransientPackage()));

	smc->SetSkeletalMesh( ChildMesh );
	smc->SetParentAnimComponent( InParent );

	PreviewScene.AddComponent( smc, ftm );
	ModelViewer->BaseSkelMeshCompList.AddItem( smc );
}

void FEditorModelViewportClient::SetViewModel(UObject* InObj)
{
	UAvatarData* ad = Cast<UAvatarData>(InObj);
	ViewObj = ad;
	// clear
	for (int i = 0; i < ModelViewer->BaseSkelMeshCompList.Num(); ++i)
	{
		if (ModelViewer->BaseSkelMeshCompList(i))
		{
			PreviewScene.RemoveComponent(ModelViewer->BaseSkelMeshCompList(i));
		}
	}

	ModelViewer->BaseSkelMeshCompList.Empty();


	while(TRUE)
	{
		INT i = ModelViewer->PreviewSkelComp->Attachments.Num() -1;
		if (i >= 0)
		{
			ModelViewer->PreviewSkelComp->DetachComponent(ModelViewer->PreviewSkelComp->Attachments(i).Component);
		}
		else
		{
			break;
		}
	}



	ModelViewer->PreviewSkelComp->SetSkeletalMesh(NULL);
	for (INT i = 0; i < ModelViewer->PreviewSkelComp->Materials.Num(); ++i)
	{
		ModelViewer->PreviewSkelComp->SetMaterial(i, NULL);
	}

	// set
	if (!ad || ad->SkeletalMesh == NULL)
	{
		return;
	}

	ModelViewer->PreviewSkelComp->SetSkeletalMesh(ad->SkeletalMesh);
	ModelViewer->PreviewSkelComp->SetPhysicsAsset(ad->PhysicAsset);
	for (INT i = 0; i < ad->Materials.Num(); ++i)
	{
		ModelViewer->PreviewSkelComp->SetMaterial(i, ad->Materials(i));
	}


	// set transform
	FTranslationMatrix ftm(FVector(0, 0, ad->CollisionHeight * ad->AvatarDrawScale));
	ModelViewer->PreviewSkelComp->ConditionalUpdateTransform(ftm);

	if (!ModelViewer->PreviewMergedComp)
	{
		ModelViewer->PreviewMergedComp = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass());
		PreviewScene.AddComponent(ModelViewer->PreviewMergedComp, ModelViewer->PreviewSkelComp->CachedParentToWorld);
	}
	else
	{
		ModelViewer->PreviewMergedComp->ConditionalUpdateTransform(ModelViewer->PreviewSkelComp->CachedParentToWorld);
	}

	//if( ad->UseMergedMesh )
	//{
	//	// hair
	//	if( ad->HairList.Num() > 0 )
	//	{
	//		AttachSkeletalMesh( ModelViewer->PreviewSkelComp, ad->HairList(0), ftm );
	//	}
	//	// face
	//	if( ad->FaceList.Num() > 0 )
	//	{
	//		AttachSkeletalMesh( ModelViewer->PreviewSkelComp, ad->FaceList(0), ftm );
	//	}
	//	// body
	//	AttachSkeletalMesh( ModelViewer->PreviewSkelComp, ad->DefaultBodyMesh, ftm );
	//}

	for (INT i = 0; i < ad->DefaultMeshListTemplate.Num(); ++i)
	{
		UEquipMesh* NewMesh = ad->DefaultMeshListTemplate(i);
		if (NewMesh == NULL)
		{
			continue;
		}

		if( !ad->UseMergedMesh )
		{
			if( ad->MergeShoulder )
			{
				if( NewMesh->MeshType < EBMT_NoMerge && NewMesh->MeshType != EBMT_Face || NewMesh->MeshType == EBMT_LShoulder )
					continue;
			}
			else
			{
				if( NewMesh->MeshType < EBMT_NoMerge )
					continue;
			}
		}

		UActorComponent* NewC = NULL;

		if (NewMesh->ResourceType == ECRT_SkeletalMesh)
		{
			USkeletalMeshComponent* smc = Cast<USkeletalMeshComponent>(UObject::StaticConstructObject(USkeletalMeshComponent::StaticClass(), UObject::GetTransientPackage()));
			if (!smc)
			{
				continue;
			}
			smc->SetSkeletalMesh(NewMesh->SkeletalMesh);

			if (NewMesh->AnimationType == EUAT_BaseAnimation)
			{
				smc->SetParentAnimComponent(ModelViewer->PreviewSkelComp);
			}
			else
			{
				// todo
			}

			NewC = smc;

			// should not assign PrimComponent for NewMesh
			// or it will change the UEquipMesh data in the DefaultMeshListTemplate
			// so that when using this AvatarData for creating PIE character
			// the character will reference the component created here instead of creating a new one
			// which will cause a crash when exiting PIE	-ming
			//NewMesh->PrimComponent = (UPrimitiveComponent*)NewC;

		}
		else if (NewMesh->ResourceType == ECRT_StaticMesh)
		{
			UStaticMeshComponent* stmc = Cast<UStaticMeshComponent>(UObject::StaticConstructObject(UStaticMeshComponent::StaticClass(), UObject::GetTransientPackage())); 
			if (!stmc)
			{
				continue;
			}
			stmc->SetStaticMesh(NewMesh->StaticMesh);
			NewC = stmc;
		}
		else if (NewMesh->ResourceType == ECRT_Particle)
		{
			UParticleSystemComponent* psc = Cast<UParticleSystemComponent>(UObject::StaticConstructObject(UParticleSystemComponent::StaticClass(), UObject::GetTransientPackage())); 
			if (!psc)
			{
				continue;
			}
			psc->SetTemplate(NewMesh->ParticleSystem);
			psc->ActivateSystem();
			NewC = psc;
		}			  

		if (NewMesh->ComponentAddingType == ECAT_AddToBaseActor)
		{
			PreviewScene.AddComponent(NewC, ftm);
			if (NewC->IsA(USkeletalMeshComponent::StaticClass()))
			{
				ModelViewer->BaseSkelMeshCompList.AddItem((USkeletalMeshComponent*)NewC);
			}
		}
		else if (NewMesh->ComponentAddingType == ECAT_AttachToBaseSocket)
		{
			ModelViewer->PreviewSkelComp->AttachComponentToSocket(NewC, NewMesh->SocketName);
		}
	}

	ViewObj->RebulidMesh(ModelViewer->PreviewSkelComp, ModelViewer->PreviewMergedComp);

}

/*-----------------------------------------------------------------------------
	WxAbilityPreview
-----------------------------------------------------------------------------*/

WxModelPreview::WxModelPreview(wxWindow* InParent, wxWindowID InID, class WxModelViewer* InASV)
:	wxWindow( InParent, InID )
,	ASVPreviewVC( NULL )
{
	CreateViewport( InASV );
}

WxModelPreview::~WxModelPreview()
{
	DestroyViewport();
}

/**
 * Calls DestoryViewport(), then creates a new viewport client and associated viewport.
 */
void WxModelPreview::CreateViewport(class WxModelViewer* InASV)
{
	DestroyViewport();

	ASVPreviewVC = new FEditorModelViewportClient(InASV);
	ASVPreviewVC->Viewport = GEngine->Client->CreateWindowChildViewport(ASVPreviewVC, (HWND)GetHandle());
	check( ASVPreviewVC->Viewport );
	ASVPreviewVC->Viewport->CaptureJoystickInput(false);
}

/**
 * Destroys any existing viewport client and associated viewport.
 */
void WxModelPreview::DestroyViewport()
{
	if ( ASVPreviewVC )
	{
		GEngine->Client->CloseViewport( ASVPreviewVC->Viewport );
		ASVPreviewVC->Viewport = NULL;

		delete ASVPreviewVC;
		ASVPreviewVC = NULL;
	}
}

void WxModelPreview::OnSize( wxSizeEvent& In )
{
	if ( ASVPreviewVC )
	{
		checkSlow( ASVPreviewVC->Viewport );
		const wxRect rc = GetClientRect();
		::MoveWindow( (HWND)ASVPreviewVC->Viewport->GetWindow(), 0, 0, rc.GetWidth(), rc.GetHeight(), 1 );

		ASVPreviewVC->Viewport->Invalidate();
	}
}

void WxModelPreview::SetViewModel(UObject* InObj)
{
	if (ASVPreviewVC)
	{
		ASVPreviewVC->SetViewModel(InObj);
	}
}

BEGIN_EVENT_TABLE( WxModelPreview, wxWindow )
	EVT_SIZE( WxModelPreview::OnSize )
END_EVENT_TABLE()

/*-----------------------------------------------------------------------------
	WxModelViewer
-----------------------------------------------------------------------------*/


BEGIN_EVENT_TABLE( WxModelViewer, WxTrackableFrame )

	EVT_SIZE(WxModelViewer::OnSize)
	EVT_COMBOBOX( IDM_MODEL_WEAPONTYPE, WxModelViewer::OnWeaponTypeComboChanged )
	EVT_BUTTON( IDM_ANIMSET_ANIMSETUSE, WxModelViewer::OnAnimSetUse )
	EVT_LISTBOX( ID_LISTBOX_MODEL_ANIMSETLIST, WxModelViewer::OnAnimSetListChanged )	
	EVT_LISTBOX( ID_LISTBOX_MODEL_ANIMSEQLIST, WxModelViewer::OnAnimSeqListChanged )	
	EVT_COMMAND_SCROLL( IDM_ANIMSET_TIMESCRUB, WxModelViewer::OnTimeScrub )
	//EVT_MENU( IDM_ANIMSET_VIEWFLOOR, WxModelViewer::OnViewFloor )
	//EVT_MENU( IDM_ANIMSET_VIEWGRID, WxModelViewer::OnViewGrid )
	//EVT_MENU( IDM_ANIMSET_VIEWCOLLISION, WxModelViewer::OnViewCollision )
	EVT_BUTTON( IDM_ANIMSET_LOOPBUTTON, WxModelViewer::OnLoopAnim )
	EVT_BUTTON( IDM_ANIMSET_PLAYBUTTON, WxModelViewer::OnPlayAnim )
	EVT_TOOL( IDM_ANIMSET_SPEED_200,	WxModelViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_175,	WxModelViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_150,	WxModelViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_125,	WxModelViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_100,	WxModelViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_50, WxModelViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_25, WxModelViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_10, WxModelViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_1, WxModelViewer::OnSpeed )
	EVT_TOOL( IDM_RESET_CAMERA_POSITION, WxModelViewer::OnResetCameraPosition )
	EVT_TOOL(IDM_ROTATE_CAMERA, WxModelViewer::OnRotateCamera)
	EVT_TOOL(IDM_MODEL_EDITOR_REFRESH, WxModelViewer::OnRefreshData)
	EVT_TOOL(IDM_MODEL_SHOW_COLLISION, WxModelViewer::OnShowCollision)
	EVT_TOOL(IDM_EXPORT_MERGED_DATA, WxModelViewer::OnExportData)
	EVT_MENU(IDM_MODEL_GENERATE_COLLISION, WxModelViewer::OnGenerateCollision)
	EVT_MENU(IDM_MODEL_DUPLICATE_DEAD_EFFECTS, WxModelViewer::OnDuplicateDeadEffects)

// 	EVT_MENU(IDM_MODEL_RESIZE_TO_MALE_WARRIOR, WxModelViewer::OnResizeToMaleWarrior)
	EVT_MENU(IDM_MODEL_CHECK_PHYSIC_DATA, WxModelViewer::OnCheckPhysic)
	EVT_MENU(IDM_MODEL_CHECK_ANIM_DATA, WxModelViewer::OnCheckAnim)
	EVT_MENU(IDM_MODEL_CHECK_SOCKET_DATA_COMMON, WxModelViewer::OnCheckSocketCommon)
	EVT_MENU(IDM_MODEL_CHECK_SOCKET_DATA_HAND, WxModelViewer::OnCheckSocketHand)
	EVT_MENU(IDM_MODEL_CHECK_SOCKET_DATA_WEAPON, WxModelViewer::OnCheckSocketWeapon)
	EVT_MENU(IDM_MODEL_SHOW_COLLISION, WxModelViewer::OnShowCollision)
END_EVENT_TABLE()


static INT AnimSetViewer_ControlBorder = 4;

/*-----------------------------------------------------------------------------
WxModelViewer
-----------------------------------------------------------------------------*/
WxModelViewer::WxModelViewer(wxWindow* InParent, wxWindowID InID, UObject* EditObj)
	:	WxTrackableFrame( InParent, InID, *LocalizeUnrealEd("ModelEditor"), wxDefaultPosition, wxSize(1024, 700), wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxMAXIMIZE )
	,	FDockingParent( this )
	,	PlaybackSpeed( 1.0f )
	//,	DlgAnimationCompression( NULL )
	,	bPreviewInstanceWeights(FALSE)
	,   bDrawAllBoneInfluenceVertices(TRUE)
	,	bTransactionInProgress(FALSE)
	,	SelectedSortStripIndex(INDEX_NONE)
	,	SelectedSortStripLodIndex(INDEX_NONE)
	,	SelectedSortStripSectionIndex(INDEX_NONE)
	,	bSortStripMoveForward(FALSE)
	,	bSortStripMoveBackward(FALSE)
	,	bResampleAnimNotifyData(FALSE)
	,	bSearchAllAnimSequences(FALSE)
	,	bPromptUserToLoadAllAnimSets(TRUE)
{
	//wxEvtHandler::Connect(wxID_ANY, IDM_ANIMSET_FILLSKELETONTREE, wxCommandEventHandler(WxModelViewer::OnFillSkeletonTree));

	SelectedSkelMesh = NULL;
	SelectedAnimSet = NULL;
	SelectedAnimSeq = NULL;
	SelectedMorphSet = NULL;
	SelectedSocket = NULL;

	PreviewSkelComp = NULL;
	PreviewAnimTree = NULL;
	PreviewAnimNode = NULL;

	PreviewMergedComp = NULL;

	CurrentPlayTime = 0.f;

	bPlaying = FALSE;
	bLooping = FALSE;

	//SocketMgr = NULL;

	ModelObject = EditObj;
	AvatarObject = NULL;

	UVSetToDisplay = -1;

	GCallbackEvent->Register(CALLBACK_FileChanged, this);

	// Reset this to zero.
	WireframeCycleCounter = 0;
	bShowAdditiveBase = FALSE;
	bListenForFileChanges = FALSE;

	bShowClothMovementScale = FALSE;

	CameraPositionType = 0;

	// clear memory
	appMemzero(&VertexInfo, sizeof(VertexInfo));

	/////////////////////////////////
	// Create all the GUI stuff.

	PlayB.Load( TEXT("ASV_Play") );
	StopB.Load( TEXT("ASV_Stop") );
	LoopB.Load( TEXT("ASV_Loop") );
	NoLoopB.Load( TEXT("ASV_NoLoop") );
	AddComponentB.Load(TEXT("AED_AddActor"));
	AbilityTimeAdd.Load(TEXT("AbilityAddTime"));
	AbilityTimeSub.Load(TEXT("AbilitySubTime"));
	SpellImage.Load(TEXT("Spell"));
	CastImage.Load(TEXT("Cast"));
	


	UseButtonB.Load( TEXT("Prop_Use") );
	SearchAllB.Load( TEXT("MaterialInstanceEditor_ShowAllParams") );
	ClearB.Load( "Cancel.png" );
	ClearDisabledB.Load( "Cancel_Disabled.png" );


	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	BaseWindow = new wxSplitterWindow(this, -1);
	BaseWindow->SetMinimumPaneSize(100);

	//wxPanel* BasePanel = new wxPanel(this, -1);
	//wxBoxSizer* BaseSizer = new wxBoxSizer(wxHORIZONTAL);
	//BasePanel->SetSizer(BaseSizer);

	LeftArea = new wxSplitterWindow(BaseWindow, -1);
	LeftArea->SetMinimumPaneSize(100);
	RightArea = new wxSplitterWindow(BaseWindow, -1);
	RightArea->SetMinimumPaneSize(100);


	BaseWindow->SplitVertically(LeftArea, RightArea, GetSize().GetWidth()*0.66);
	BaseWindow->SetSashGravity(0.6);
	BaseWindow->UpdateSize();

	wxPanel* RealtimePanel = new wxPanel(LeftArea);
	RealtimePanel->SetBackgroundColour(wxColour(200, 200, 200));

	wxPanel* ShowResPanel = new wxPanel(LeftArea);
	ShowResPanel->SetBackgroundColour(wxColour(200, 200, 200));

	wxPanel* MasterPanel = new wxPanel(RightArea);
	MasterPanel->SetBackgroundColour(wxColour(200, 200, 200));

	wxPanel* SecondPanel = new wxPanel(RightArea);
	SecondPanel->SetBackgroundColour(wxColour(200, 200, 200));


	LeftArea->SplitHorizontally(RealtimePanel, ShowResPanel);
	LeftArea->SetSashGravity(0.8);
	LeftArea->SetSashPosition(GetSize().GetHeight()*0.8, true);

	RightArea->SplitHorizontally(MasterPanel, SecondPanel);
	RightArea->SetSashGravity(0.5);
	RightArea->SetSashPosition(GetSize().GetHeight()*0.7, true);

	// set res panel
	wxGridSizer* ResSizer = new wxGridSizer(3, 0, 0);
	ShowResPanel->SetSizer(ResSizer);
	wxPanel* ResPanel1 = new wxPanel(ShowResPanel);
	wxPanel* ResPanel2 = new wxPanel(ShowResPanel);
	wxPanel* ResPanel3 = new wxPanel(ShowResPanel);
	ResSizer->Add(ResPanel1, 1, wxEXPAND|wxALL);
	ResSizer->Add(ResPanel2, 1, wxEXPAND|wxALL);
	ResSizer->Add(ResPanel3, 1, wxEXPAND|wxALL);

	wxBoxSizer* Sizer1 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* Sizer2 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* Sizer3 = new wxBoxSizer(wxVERTICAL);

	ResPanel1->SetSizer(Sizer1);
	ResPanel2->SetSizer(Sizer2);
	ResPanel3->SetSizer(Sizer3);

	AvatarNameText = new wxStaticText(ResPanel1, -1, wxT("AvatarName:"));
	SkelMeshText = new wxStaticText(ResPanel1, -1, wxT("SkelMesh:"));
	AnimTreeText = new wxStaticText(ResPanel1, -1, wxT("AnimTree:"));
	PhyAssetText = new wxStaticText(ResPanel1, -1, wxT("PhysicAsset:"));
	Sizer1->Add(AvatarNameText, 0, wxGROW, 4 );
	Sizer1->Add(SkelMeshText, 0, wxGROW, 4 );
	Sizer1->Add(AnimTreeText, 0, wxGROW, 4 );
	Sizer1->Add(PhyAssetText, 0, wxGROW, 4 );

	WeaponTypeComboBox = new wxComboBox(ResPanel2, IDM_MODEL_WEAPONTYPE, wxString(), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	Sizer2->Add(WeaponTypeComboBox, 0, wxEXPAND|wxALL, 4 );
	AnimSetListBox = new wxListBox(ResPanel2, ID_LISTBOX_MODEL_ANIMSETLIST);
	Sizer2->Add(AnimSetListBox, 1, wxEXPAND|wxALL, 4 );

	AnimSeqListBox = new wxListBox(ResPanel3, ID_LISTBOX_MODEL_ANIMSEQLIST);
	Sizer3->Add(AnimSeqListBox, 1, wxEXPAND|wxALL, 4 );


	CheckData = Cast<UDataCheck>(UObject::StaticLoadObject(UDataCheck::StaticClass(), NULL, TEXT("CheckData.CheckData"), NULL,  LOAD_NoWarn, NULL));


	// realtime view
	wxBoxSizer* MidSizerV = new wxBoxSizer( wxVERTICAL );
	RealtimePanel->SetSizer(MidSizerV);

	// 3D Preview
	PreviewWindow = new WxModelPreview( RealtimePanel, -1, this );
	MidSizerV->Add( PreviewWindow, 1, wxGROW|wxALL, 4 );
	
	// Sizer for controls at bottom
	wxBoxSizer* ControlSizerH = new wxBoxSizer( wxHORIZONTAL );
	MidSizerV->Add( ControlSizerH, 0, wxGROW|wxALL, 4 );
	
	// Stop/Play/Loop buttons
	LoopButton = new wxBitmapButton( RealtimePanel, IDM_ANIMSET_LOOPBUTTON, NoLoopB);
	ControlSizerH->Add( LoopButton, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	PlayButton = new wxBitmapButton( RealtimePanel, IDM_ANIMSET_PLAYBUTTON, PlayB);
	ControlSizerH->Add( PlayButton, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	// Time scrubber
	TimeSlider = new wxSlider( RealtimePanel, IDM_ANIMSET_TIMESCRUB, 0, 0, ASV_SCRUBRANGE );
	ControlSizerH->Add( TimeSlider, 1,  wxALIGN_CENTER, 5 );

	// res select window
	wxBoxSizer* MasterBoxSizer = new wxBoxSizer(wxVERTICAL);
	MasterPanel->SetSizer(MasterBoxSizer);
	MasterProperty = new WxPropertyWindowHost;
	MasterProperty->Create(MasterPanel, this);
	MasterBoxSizer->Add(MasterProperty, 1, wxEXPAND|wxALL, 4);
	//MasterProperty->SetObject(EditObj, EPropertyWindowFlags::ShouldShowCategories);

	wxBoxSizer* SecondBoxSizer = new wxBoxSizer(wxVERTICAL);
	SecondPanel->SetSizer(SecondBoxSizer);
	SecondProperty = new WxPropertyWindowHost;
	SecondProperty->Create(SecondPanel, this);
	SecondBoxSizer->Add(SecondProperty, 1, wxEXPAND|wxALL, 4);

	ANPC* npcObj = Cast<ANPC>(EditObj);
	UAvatarData* AvatarObj = NULL;
	if (npcObj)
	{
		AvatarObj = npcObj->UseingAvatar;
		SecondProperty->SetObject(AvatarObj, EPropertyWindowFlags::ShouldShowCategories);
	}
	else
	{
		AvatarObj = Cast<UAvatarData>(EditObj);
		SecondProperty->SetObject(NULL, EPropertyWindowFlags::ShouldShowCategories);
	}

	AvatarObject = AvatarObj;

	ResetObject(EditObj);







	// primary property window
	//wxBoxSizer* PrimaryBoxSizer = new wxBoxSizer(wxVERTICAL);
	//ShowResPanel->SetSizer(PrimaryBoxSizer);
	//PreviewResProperty = new WxPropertyWindowHost;
	//PreviewResProperty->Create(ShowResPanel, this);
	//PrimaryBoxSizer->Add(PreviewResProperty, 1, wxEXPAND|wxALL, 4);


	// Set camera location
	// TODO: Set the automatically based on bounds? Update when changing mesh? Store per-mesh?
	FEditorModelViewportClient* vc = GetPreviewVC();
	check( vc );
	
	vc->ViewLocation = FVector(512,0,100);
	vc->ViewRotation = FRotator(0,32768,0);

	// Init wind dir.
	WindStrength = 10.f;
	WindRot = FRotator(0,0,0);
#if WITH_APEX
	WindVelocityBlendTime = 0.1f;
#endif
	UpdateClothWind();
	
	
	// Use default WorldInfo to define the gravity and stepping params.
	AWorldInfo* Info = (AWorldInfo*)(AWorldInfo::StaticClass()->GetDefaultObject());
	check(Info);
	FVector Gravity(0, 0, Info->DefaultGravityZ);
	RBPhysScene = CreateRBPhysScene( Gravity );
	
#if WITH_APEX
	if ( (RBPhysScene != NULL) && (RBPhysScene->ApexScene != NULL) && (GApexManager != NULL) )
	{
		// Set infinite budget here so all clothing will be visible in AnimSetViewer
		GApexManager->SetApexLODResourceBudget( RBPhysScene->ApexScene, FLT_MAX );
	}
#endif

	//RefreshData();


//	SetSize(1200,800);
	// Load Window Options.
	LoadSettings();
//
//
	// Create menu bar
	MenuBar = new WxMEMenuBar();
	AppendWindowMenu(MenuBar);
	SetMenuBar( MenuBar );
//	// disable alt. bone weighting tool first. 
//	MenuBar->EnableAltBoneWeightingMenu(FALSE,PreviewSkelComp);
//
	// Create tool bar
	ToolBar = new WxMVToolBar( this, -1 );
	SetToolBar( ToolBar );
//	// Update the FOV so it shows the last used value
//	if ( GApp && GApp->EditorFrame && GApp->EditorFrame->ViewportConfigData )
//	{
//		SetFOV( GApp->EditorFrame->ViewportConfigData->GetCustomFOV() );
//	}
//
//	// Create status bar
//	StatusBar = new WxAEVStatusBar( this, -1 );
//	SetStatusBar( StatusBar );
//
//	// Set camera location
//	// TODO: Set the automatically based on bounds? Update when changing mesh? Store per-mesh?
//	FEditorModelViewportClient* ASVPreviewVC = GetPreviewVC();
//	check( ASVPreviewVC );
//
//	ASVPreviewVC->ViewLocation = FVector(0,-256,0);
//	ASVPreviewVC->ViewRotation = FRotator(0,16384,0);	
//
//	// Init wind dir.
//	WindStrength = 10.f;
//	WindRot = FRotator(0,0,0);
//#if WITH_APEX
//	WindVelocityBlendTime = 0.1f;
//#endif
//	UpdateClothWind();
//
//
//	// Use default WorldInfo to define the gravity and stepping params.
//	AWorldInfo* Info = (AWorldInfo*)(AWorldInfo::StaticClass()->GetDefaultObject());
//	check(Info);
//	FVector Gravity(0, 0, Info->DefaultGravityZ);
//	RBPhysScene = CreateRBPhysScene( Gravity );
//
//#if WITH_APEX
//	if ( (RBPhysScene != NULL) && (RBPhysScene->ApexScene != NULL) && (GApexManager != NULL) )
//	{
//		// Set infinite budget here so all clothing will be visible in AnimSetViewer
//		GApexManager->SetApexLODResourceBudget( RBPhysScene->ApexScene, FLT_MAX );
//	}
//#endif
//
//
	// Create floor component
	UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(NULL, TEXT("EditorMeshes.PhAT_FloorBox"), NULL, LOAD_None, NULL);
	check(FloorMesh);

	EditorFloorComp = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass());
	EditorFloorComp->StaticMesh = FloorMesh;
	EditorFloorComp->BlockRigidBody = TRUE;
	EditorFloorComp->Scale = 4.f;
	vc->PreviewScene.AddComponent(EditorFloorComp,FMatrix::Identity);
//
//	EditorFloorComp->BodyInstance = ConstructObject<URB_BodyInstance>( URB_BodyInstance::StaticClass() );
//	EditorFloorComp->BodyInstance->InitBody(EditorFloorComp->StaticMesh->BodySetup, FMatrix::Identity, FVector(4.f), true, EditorFloorComp, RBPhysScene);
//
//	//hide or show floor (and update collision)
//	UpdateFloorComponent();
}


WxModelViewer::~WxModelViewer()
{
	GCallbackEvent->UnregisterAll(this);
	////make sure to only turn off file extensions if we previously as to listen to them
	//if (bListenForFileChanges)
	//{
	//	SetFileSystemNotifications(FALSE);
	//}

	SaveSettings();
	FlushRenderingCommands();

	if ( PreviewSkelComp )
	{
		PreviewSkelComp->ReleaseApexClothing();
		PreviewSkelComp->AnimSets.Empty();
	}

	PreviewWindow->DestroyViewport();


	DestroyRBPhysScene(RBPhysScene);
}

void WxModelViewer::SetWeaponIdx(int idx)
{
	if (!AvatarObject)
	{
		return;
	}

	if (idx > -1 && idx < AvatarObject->WeaponAnimSetPairs.Num())
	{
		AnimSetListBox->Clear();
		for (INT i = 0; i < AvatarObject->WeaponAnimSetPairs(idx).AnimSets.Num(); ++i)
		{
			if (AvatarObject->WeaponAnimSetPairs(idx).AnimSets(i))
			{
				FString str = AvatarObject->WeaponAnimSetPairs(idx).AnimSets(i)->GetName();
				AnimSetListBox->Append(*str);
			}
			else
			{
				break;
			}

		}

		WeaponTypeComboBox->SetSelection(idx);

		SetAnimSet(0);
	}
	else
	{
		AnimSetListBox->Clear();
		AnimSeqListBox->Clear();
		PreviewSkelComp->AnimSets.Empty();
		SelectedAnimSeq = NULL;
	}

}

void WxModelViewer::SetAnimSet(int idx)
{
	if (!AvatarObject)
	{
		return;
	}
	int WeaponIdx = WeaponTypeComboBox->GetSelection();
	if (WeaponIdx > -1 && WeaponIdx < AvatarObject->WeaponAnimSetPairs.Num())
	{
		if (idx > -1 && idx < AvatarObject->WeaponAnimSetPairs(WeaponIdx).AnimSets.Num())
		{
			AnimSeqListBox->Clear();
			if (AvatarObject->WeaponAnimSetPairs(WeaponIdx).AnimSets(idx) == NULL)
			{
				AnimSetListBox->Clear();
				AnimSeqListBox->Clear();
				PreviewSkelComp->AnimSets.Empty();
				SelectedAnimSeq = NULL;
				return;
			}
			for (INT i = 0; i < AvatarObject->WeaponAnimSetPairs(WeaponIdx).AnimSets(idx)->Sequences.Num(); ++i)
			{
				FString str = AvatarObject->WeaponAnimSetPairs(WeaponIdx).AnimSets(idx)->Sequences(i)->SequenceName.ToString();
				AnimSeqListBox->Append(*str);
			}

			PreviewSkelComp->AnimSets.Empty();
			PreviewSkelComp->AnimSets.AddItem(AvatarObject->WeaponAnimSetPairs(WeaponIdx).AnimSets(idx));

			if (idx < int(AnimSetListBox->GetCount()))
			{
				AnimSetListBox->SetSelection(idx);
			}

			SetAnimSeq(0);
		}
	}
	else
	{
		PreviewSkelComp->AnimSets.Empty();
		SelectedAnimSeq = NULL;
		bPlaying = FALSE;
		bLooping = FALSE;
	}
}

void WxModelViewer::SetAnimSeq(int idx)
{
	if (!AvatarObject)
	{
		return;
	}

	int WeaponIdx = WeaponTypeComboBox->GetSelection();
	if (WeaponIdx > -1 && WeaponIdx < AvatarObject->WeaponAnimSetPairs.Num())
	{
		INT SetIdx = AnimSetListBox->GetSelection();

		if (SetIdx > -1 && SetIdx < AvatarObject->WeaponAnimSetPairs(WeaponIdx).AnimSets.Num())
		{
			UAnimSet* as = AvatarObject->WeaponAnimSetPairs(WeaponIdx).AnimSets(SetIdx);
			if (idx > -1 && idx < as->Sequences.Num())
			{
				PreviewAnimNode->SetAnim(as->Sequences(idx)->SequenceName);
				SelectedAnimSeq = as->Sequences(idx);
			}

			if (idx < INT(AnimSeqListBox->GetCount()))
			{
				AnimSeqListBox->SetSelection(idx);
			}
		}
	}
	else
	{
		SelectedAnimSeq = NULL;
	}
}


void WxModelViewer::ResetObject(UObject* InObject)
{
	ModelObject = InObject;
	MasterProperty->SetObject(InObject, EPropertyWindowFlags::ShouldShowCategories);
	ANPC* npcObj = Cast<ANPC>(InObject);
	UAvatarData* AvatarObj = NULL;
	if (npcObj)
	{
		AvatarObj = npcObj->UseingAvatar;
		SecondProperty->SetObject(AvatarObj, EPropertyWindowFlags::ShouldShowCategories);
	}
	else
	{
		AvatarObj = Cast<UAvatarData>(InObject);
		SecondProperty->SetObject(NULL, EPropertyWindowFlags::ShouldShowCategories);
	}

	AvatarObject = AvatarObj;

	AvatarObject->DynamicLoadPhysicAsset();

	MasterProperty->RequestReconnectToData();

	WeaponTypeComboBox->Clear();
	AnimSetListBox->Clear();
	AnimSeqListBox->Clear();

	SelectedAnimSeq = NULL;

	//
	if (AvatarObj)
	{
		PreviewWindow->SetViewModel(AvatarObj);
		UEnum* eobj = FindObject<UEnum>(UTypeDefine::StaticClass(), TEXT("EWeaponType"), TRUE);

		WeaponTypeComboBox->Clear();
		for (INT i = 0; i < AvatarObj->WeaponAnimSetPairs.Num(); ++i)
		{
			if (AvatarObj->WeaponAnimSetPairs(i).AnimSets.Num())
			{
				INT  type = AvatarObj->WeaponAnimSetPairs(i).WeaponType;
				FString str = eobj->GetMetaData(L"DisplayName", type);
				FString locStr = LocalizeProperty(*(eobj->GetName()), *str);
				WeaponTypeComboBox->Append(*locStr);
			}
			else
			{
				return;
			}
		}

		FString ct;

		ct = AvatarObject->AvatarName;
		AvatarNameText->SetLabel(*ct);

		if (AvatarObject->SkeletalMesh)
		{
			ct = FString(L"SkeletalMesh : ") + AvatarObject->SkeletalMesh->GetName();
			SkelMeshText->SetForegroundColour(wxColour(0, 0, 0));
		}
		else
		{
			ct = FString(L"SkeletalMesh : NULL!!!");
			SkelMeshText->SetForegroundColour(wxColour(255, 0, 0));
		}
		SkelMeshText->SetLabel(*ct);

		if (AvatarObject->AnimationTree)
		{
			ct = FString(L"AnimTree : ") + AvatarObject->AnimationTree->GetName();
		}
		else
		{
			ct = FString(L"AnimTree : NULL!!!");
		}
		AnimTreeText->SetLabel(*ct);

		if (AvatarObject->PhysicAsset)
		{
			ct = FString(L"PhysicAsset : ") + AvatarObject->PhysicAsset->GetName();
		}
		else
		{
			ct = FString(L"PhysicAsset : NULL!!!");
		}
		PhyAssetText->SetLabel(*ct);

		UpdateAnimInfos();

		UpdateAttachment();
	}
}

void WxModelViewer::UpdateMaterial()
{
	if (!AvatarObject)
	{
		return;
	}

	for (INT i = 0; i < PreviewSkelComp->Materials.Num(); ++i)
	{
		PreviewSkelComp->SetMaterial(i, NULL);
	}

	for (INT i = 0; i < AvatarObject->Materials.Num(); ++i)
	{
		PreviewSkelComp->SetMaterial(i, AvatarObject->Materials(i));
	}
}

void WxModelViewer::UpdateAnimInfos()
{
	if (!AvatarObject)
	{
		return;
	}

	INT WeaponTypeComboBoxSelectedIndex = WeaponTypeComboBox->GetCurrentSelection();
	INT AnimSetSelectedIndex = AnimSetListBox->GetSelection();
	INT AnimSelectedIndex = AnimSeqListBox->GetSelection();

	WeaponTypeComboBox->Clear();
	AnimSetListBox->Clear();
	AnimSeqListBox->Clear();

	for (INT i = 0; i < AvatarObject->WeaponAnimSetPairs.Num(); ++i)
	{
		if (AvatarObject->WeaponAnimSetPairs(i).AnimSets.Num())
		{
			INT  type = AvatarObject->WeaponAnimSetPairs(i).WeaponType;
			UEnum* eobj = FindObject<UEnum>(UTypeDefine::StaticClass(), TEXT("EWeaponType"), TRUE);
			FString str = eobj->GetMetaData(L"DisplayName", type);
			FString locStr = LocalizeProperty(*(eobj->GetName()), *str);
			WeaponTypeComboBox->Append(*locStr);
		}
		else
		{
			WeaponTypeComboBox->Append(TEXT("NoData"));
		}
	}

	if (WeaponTypeComboBox->GetCount() == 0)
	{
		return;
	}
	else
	{
		WeaponTypeComboBoxSelectedIndex = Clamp<INT>(WeaponTypeComboBoxSelectedIndex, 0, WeaponTypeComboBox->GetCount() - 1);
		WeaponTypeComboBox->SetSelection(WeaponTypeComboBoxSelectedIndex);
	}

	for (INT i = 0; i < AvatarObject->WeaponAnimSetPairs(WeaponTypeComboBoxSelectedIndex).AnimSets.Num(); ++i)
	{
		if (AvatarObject->WeaponAnimSetPairs(WeaponTypeComboBoxSelectedIndex).AnimSets(i))
		{
			FString str = AvatarObject->WeaponAnimSetPairs(WeaponTypeComboBoxSelectedIndex).AnimSets(i)->GetName();
			AnimSetListBox->Append(*str);
		}
		else
		{
			AnimSetListBox->Append(TEXT("NoData"));
		}
	}

	if (AnimSetListBox->GetCount() == 0)
	{
		return;
	}
	else
	{
		AnimSetSelectedIndex = Clamp<INT>(AnimSetSelectedIndex, 0, AnimSetListBox->GetCount() - 1);
		AnimSetListBox->SetSelection(AnimSetSelectedIndex);
	}

	PreviewSkelComp->AnimSets.Empty();
	PreviewSkelComp->AnimSets.AddItem(AvatarObject->WeaponAnimSetPairs(WeaponTypeComboBoxSelectedIndex).AnimSets(AnimSetSelectedIndex));

	if (AvatarObject->WeaponAnimSetPairs(WeaponTypeComboBoxSelectedIndex).AnimSets(AnimSetSelectedIndex))
	{
		for (INT i = 0; i < AvatarObject->WeaponAnimSetPairs(WeaponTypeComboBoxSelectedIndex).AnimSets(AnimSetSelectedIndex)->Sequences.Num(); ++i)
		{
			FString str = AvatarObject->WeaponAnimSetPairs(WeaponTypeComboBoxSelectedIndex).AnimSets(AnimSetSelectedIndex)->Sequences(i)->SequenceName.ToString();
			AnimSeqListBox->Append(*str);
		}
	}


	if (AnimSeqListBox->GetCount() == 0)
	{
		return;
	}
	else
	{
		AnimSelectedIndex = Clamp<INT>(AnimSelectedIndex, 0, AnimSeqListBox->GetCount() - 1);
		AnimSeqListBox->SetSelection(AnimSelectedIndex);
	}

	SetAnimSeq(AnimSelectedIndex);
}

void WxModelViewer::UpdateAttachment()
{
	if (!AvatarObject)
	{
		return;
	}

	for (INT i = 0; i < PreviewSkelComp->Attachments.Num(); ++i)
	{
		if (PreviewSkelComp->Attachments(i).Component && PreviewSkelComp->Attachments(i).Component->IsA(UParticleSystemComponent::StaticClass()))
		{
			PreviewSkelComp->DetachComponent(PreviewSkelComp->Attachments(i).Component);
		}
	}

	for (INT i = 0; i < AvatarObject->AttachedParticles.Num(); ++i)
	{
		if (AvatarObject->AttachedParticles(i).EmitterTemplate)
		{
			UParticleSystemComponent* NewPSC = ConstructObject<UParticleSystemComponent>(UParticleSystemComponent::StaticClass(), PreviewSkelComp);
			check(NewPSC);
			NewPSC->SetTemplate(AvatarObject->AttachedParticles(i).EmitterTemplate);
			NewPSC->Scale = AvatarObject->AttachedParticles(i).Scale;
			PreviewSkelComp->AttachComponentToSocket(NewPSC, AvatarObject->AttachedParticles(i).AttachSocket);
		}
	}
}

void WxModelViewer::OnResetCameraPosition(wxCommandEvent& In)
{
	//FLOAT Dis = 900.f;
	CameraPositionType = 0;
	FEditorModelViewportClient* vc = GetPreviewVC();
	check( vc );
	FVector NewLocation;
	FRotator NewRotation;

	NewRotation.Yaw = 32768; 
	NewRotation.Pitch = -5461;
	NewRotation.Roll = 0;

	NewLocation = NewRotation.Vector().SafeNormal() * (-1000.f);
	vc->SetCameraPosition(NewLocation, NewRotation);
}

void WxModelViewer::OnRotateCamera(wxCommandEvent& In)
{
	CameraPositionType = (++CameraPositionType) % 4;
	FEditorModelViewportClient* vc = GetPreviewVC();
	check( vc );
	vc->SetCameraPosition(CameraPositionType);

}


void WxModelViewer::OnRefreshData(wxCommandEvent& In)
{
	ResetObject(ModelObject);
}

// [ming@snda
void WxModelViewer::CopyTexture( UTexture2DDynamic* SrcTex, UTexture2D* DstTex )
{
	FTexture2DRHIParamRef SrcTexRHI = ((FTexture2DDynamicResource*)SrcTex->Resource)->GetTexture2DRHI();
	for( INT i = 0; i < SrcTex->NumMips; ++i )
	{
		UINT stride;
		BYTE* SrcData = (BYTE*)RHILockTexture2D( SrcTexRHI, i, FALSE, stride, FALSE );

		INT SizeX = SrcTex->SizeX >> i;
		INT SizeY = SrcTex->SizeY >> i;

		extern SIZE_T CalculateImageBytes( DWORD SizeX, DWORD SizeY, DWORD SizeZ, BYTE Format );

		UINT TotalSize = CalculateImageBytes( SizeX, SizeY, 0, SrcTex->Format );

		// create new mip data
		if( i != 0 )
		{
			FTexture2DMipMap* NewMipMap = new(DstTex->Mips) FTexture2DMipMap;
			NewMipMap->SizeX = SizeX;
			NewMipMap->SizeY = SizeY;
		}

		DstTex->Mips(i).Data.Lock( LOCK_READ_WRITE );
		BYTE* DstData = (BYTE*)DstTex->Mips(i).Data.Realloc( TotalSize );

		memcpy_s( DstData, TotalSize, SrcData, TotalSize );

		DstTex->Mips(i).Data.Unlock();
		RHIUnlockTexture2D( SrcTexRHI, i, FALSE );
	}

	DstTex->UpdateResource();
}

void WxModelViewer::OnExportData( wxCommandEvent& In )
{
	if( PreviewMergedComp == NULL ||
		PreviewMergedComp->SkeletalMesh == NULL )
	{
		return;
	}

	UMMOSkeletalMeshMerge* pMergedMesh = Cast<UMMOSkeletalMeshMerge>(
		PreviewMergedComp->SkeletalMesh );
	if( pMergedMesh == NULL )
		return;

	// export merged mesh
	const FString DialogTitle( TEXT("ExportMesh") );
	WxDlgMoveAssets MoveDialog;
	MoveDialog.ConfigureNameField( FALSE );

	USkeletalMesh* pNewMesh = NULL;

	FString PackageName, GroupName, MeshName;

	MoveDialog.SetTitle( *DialogTitle );

	UBOOL bSawOKAll = FALSE;

	const int MoveDialogResult = MoveDialog.ShowModal( TEXT("NewPackage"), TEXT(""), TEXT(""), TRUE );
	if( MoveDialogResult == wxID_OK || MoveDialogResult == ID_OK_ALL )
	{
		const FScopedBusyCursor BusyCursor;

		if( MoveDialogResult == ID_OK_ALL )
			bSawOKAll = TRUE;

		PackageName = MoveDialog.GetNewPackage();
		GroupName = MoveDialog.GetNewGroup();
		MoveDialog.GetNewName( MeshName );

		UPackage* ExistPackage = UObject::FindPackage( NULL, *PackageName );
		if( !ExistPackage )
			ExistPackage = UObject::CreatePackage( NULL, *PackageName );

		if( !ExistPackage->IsFullyLoaded() )
		{	
			// Ask user to fully load
			if(appMsgf( AMT_YesNo, *FString::Printf( LocalizeSecure(LocalizeUnrealEd(TEXT("NeedsToFullyLoadPackageF")), *ExistPackage->GetName(), TEXT("Merge")) ) ) )
			{
				// Fully load package.
				GWarn->BeginSlowTask( *LocalizeUnrealEd(TEXT("FullyLoadingPackages")), TRUE );
				ExistPackage->FullyLoad();
				GWarn->EndSlowTask();
			}
			// User declined abort operation.
			else
			{
				debugf(TEXT("Aborting operation as %s was not fully loaded."),*ExistPackage->GetName());
				return;
			}
		}

		// re-merge the skeletal meshes, don't use the pMergedMesh object
		pNewMesh = Cast<USkeletalMesh>( UObject::StaticConstructObject(
			USkeletalMesh::StaticClass(), ExistPackage, *MeshName, RF_Public|RF_Standalone ) );
		if( pNewMesh == NULL )
			return;

		// mark the mesh as exported, so as to avoid rebuilding the vertex buffer
		pNewMesh->bIsExported = TRUE;

		TArray< USkeletalMesh* > SourceMeshList;
		TArray<FVector4> uvSections;
		for( INT idx = 0; idx < pMergedMesh->Parts.Num(); ++idx )
		{
			if( !pMergedMesh->Parts(idx).Mesh )
				continue;
			SourceMeshList.AddItem( pMergedMesh->Parts(idx).Mesh );

			FVector4 uv( 
				pMergedMesh->Parts(idx).OrigX, 
				pMergedMesh->Parts(idx).OrigY, 
				pMergedMesh->Parts(idx).SizeX, 
				pMergedMesh->Parts(idx).SizeY );
			uvSections.AddItem( uv );
		}

		TArray< FSkelMeshMergeSectionMapping > InForceSectionMapping;

		FSkeletalMeshMerge MeshMergeUtil( pNewMesh, SourceMeshList, InForceSectionMapping, 0 );
		MeshMergeUtil.SetUVRemapping( pMergedMesh->TargetTexSizeX, pMergedMesh->TargetTexSizeY, uvSections );
		if( !MeshMergeUtil.DoMerge( FALSE, FALSE ) )	// even with the vertex data we can't correctly rebuild the vertex buffer since the vertex order doesn't match the origin orders
		{
			return;
		}

		pNewMesh->MarkPackageDirty();
		
		//pMergedMesh->Rename( MeshName, ExistPackage, REN_Text );
		//GEditor->RenameObject( pNewMesh, ExistPackage, *MeshName, REN_None );
		GCallbackEvent->Send( FCallbackEventParameters( NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, pNewMesh ) );
		 
	}

	// export textures
	const FString TexDialogTitle( TEXT("ExportTextures") );
	WxDlgMoveAssets TexMoveDialog;
	TexMoveDialog.ConfigureNameField( FALSE );
	TexMoveDialog.SetTitle( *TexDialogTitle );

	UTexture2D* NewDiffTex = NULL;
	UTexture2D* NewSpecTex = NULL;
	UTexture2D* NewNormTex = NULL;

	FString TexName = MeshName + TEXT("_Tex");
	int TexMoveDialogResult = wxID_CANCEL;
	if( !bSawOKAll )
	{
		TexMoveDialogResult = TexMoveDialog.ShowModal( PackageName, TEXT(""), TexName, TRUE );
	}
	if( bSawOKAll || TexMoveDialogResult == wxID_OK || TexMoveDialogResult == ID_OK_ALL )
	{
		const FScopedBusyCursor BusyCursor;

		if( !bSawOKAll )
		{
			PackageName = TexMoveDialog.GetNewPackage();
			GroupName = TexMoveDialog.GetNewGroup();
			TexMoveDialog.GetNewName( TexName );
		}

		if( TexMoveDialogResult == ID_OK_ALL )
			bSawOKAll = TRUE;

		UPackage* ExistPackage = UObject::FindPackage( NULL, *PackageName );
		if( !ExistPackage )
			ExistPackage = UObject::CreatePackage( NULL, *PackageName );

		if( !ExistPackage->IsFullyLoaded() )
		{	
			// Ask user to fully load
			if(appMsgf( AMT_YesNo, *FString::Printf( LocalizeSecure(LocalizeUnrealEd(TEXT("NeedsToFullyLoadPackageF")), *ExistPackage->GetName(), TEXT("Merge")) ) ) )
			{
				// Fully load package.
				GWarn->BeginSlowTask( *LocalizeUnrealEd(TEXT("FullyLoadingPackages")), TRUE );
				ExistPackage->FullyLoad();
				GWarn->EndSlowTask();
			}
			// User declined abort operation.
			else
			{
				debugf(TEXT("Aborting operation as %s was not fully loaded."),*ExistPackage->GetName());
				return;
			}
		}

		// diffuse
		FString DiffName = TexName + TEXT("_D");
		UObject* ExistingObj = UObject::StaticFindObject( UTexture2D::StaticClass(), ExistPackage, *DiffName );
		if( pMergedMesh->MergedTex_D == NULL )
		{
			debugf( TEXT("No merged diffuse texture.") );
		}
		else
		{
			if( ExistingObj )
			{
				// ask the user if should overwrite
			}
			NewDiffTex = Cast<UTexture2D>( UObject::StaticConstructObject( 
				UTexture2D::StaticClass(), ExistPackage, *DiffName, RF_Public|RF_Standalone ) );
			
			UTexture2DDynamic* SrcTex = pMergedMesh->MergedTex_D;
			UTexture2D* DstTex = NewDiffTex;

			DstTex->Init( SrcTex->SizeX, SrcTex->SizeY, (EPixelFormat)SrcTex->Format );
			DstTex->SRGB = SrcTex->SRGB;
			DstTex->CompressionNone = SrcTex->CompressionNone;
			DstTex->MipGenSettings = TMGS_LeaveExistingMips;
			DstTex->AddressX = TA_Clamp;
			DstTex->AddressY = TA_Clamp;
			DstTex->LODGroup = TEXTUREGROUP_Character;

			CopyTexture( SrcTex, DstTex );

			NewDiffTex->MarkPackageDirty();

			GCallbackEvent->Send( FCallbackEventParameters( 
				NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, NewDiffTex ) );
		}

		// specular
		FString SpecName = TexName + TEXT("_S");
		ExistingObj = UObject::StaticFindObject( UTexture2D::StaticClass(), ExistPackage, *SpecName );
		if( pMergedMesh->MergedTex_S == NULL )
		{
			debugf( TEXT("No merged diffuse texture.") );
		}
		else
		{
			if( ExistingObj )
			{
				// ask the user if should overwrite
			}
			NewSpecTex = Cast<UTexture2D>( UObject::StaticConstructObject( 
				UTexture2D::StaticClass(), ExistPackage, *SpecName, RF_Public|RF_Standalone ) );
			
			UTexture2DDynamic* SrcTex = pMergedMesh->MergedTex_S;
			UTexture2D* DstTex = NewSpecTex;

			DstTex->Init( SrcTex->SizeX, SrcTex->SizeY, (EPixelFormat)SrcTex->Format );
			DstTex->SRGB = SrcTex->SRGB;
			DstTex->CompressionNone = SrcTex->CompressionNone;
			DstTex->MipGenSettings = TMGS_LeaveExistingMips;
			DstTex->AddressX = TA_Clamp;
			DstTex->AddressY = TA_Clamp;
			DstTex->LODGroup = TEXTUREGROUP_CharacterSpecular;

			CopyTexture( SrcTex, DstTex );

			NewSpecTex->MarkPackageDirty();

			GCallbackEvent->Send( FCallbackEventParameters( 
				NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, NewSpecTex ) );
		}
		
		// normal
		FString NormName = TexName + TEXT("_N");
		ExistingObj = UObject::StaticFindObject( UTexture2D::StaticClass(), ExistPackage, *NormName );
		if( pMergedMesh->MergedTex_N == NULL )
		{
			debugf( TEXT("No merged diffuse texture.") );
		}
		else
		{
			if( ExistingObj )
			{
				// ask the user if should overwrite
			}
			NewNormTex = Cast<UTexture2D>( UObject::StaticConstructObject( 
				UTexture2D::StaticClass(), ExistPackage, *NormName, RF_Public|RF_Standalone ) );
			
			UTexture2DDynamic* SrcTex = pMergedMesh->MergedTex_N;
			UTexture2D* DstTex = NewNormTex;

			DstTex->Init( SrcTex->SizeX, SrcTex->SizeY, (EPixelFormat)SrcTex->Format );
			DstTex->SRGB = SrcTex->SRGB;
			DstTex->CompressionNone = SrcTex->CompressionNone;
			DstTex->MipGenSettings = TMGS_LeaveExistingMips;
			DstTex->AddressX = TA_Clamp;
			DstTex->AddressY = TA_Clamp;
			DstTex->LODGroup = TEXTUREGROUP_CharacterNormalMap;

			CopyTexture( SrcTex, DstTex );

			NewNormTex->MarkPackageDirty();

			GCallbackEvent->Send( FCallbackEventParameters( 
				NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, NewNormTex ) );
		}
	}

	// export material instance
	const FString MatDialogTitle( TEXT("ExportMIC") );
	WxDlgMoveAssets MatMoveDialog;
	MatMoveDialog.ConfigureNameField( FALSE );
	MatMoveDialog.SetTitle( *MatDialogTitle );

	FString MatName = MeshName + TEXT("_Mat");
	int MatMoveDialogResult = wxID_CANCEL;
	if( !bSawOKAll )
	{
		MatMoveDialogResult = MatMoveDialog.ShowModal( PackageName, TEXT(""), MatName, TRUE );
	}
	if( bSawOKAll || MatMoveDialogResult == wxID_OK || MatMoveDialogResult == ID_OK_ALL )
	{
		const FScopedBusyCursor BusyCursor;

		if( !bSawOKAll )
		{
			PackageName = MatMoveDialog.GetNewPackage();
			GroupName = MatMoveDialog.GetNewGroup();
			MatMoveDialog.GetNewName( MatName );
		}

		if( MatMoveDialogResult == ID_OK_ALL )
			bSawOKAll = TRUE;

		UPackage* ExistPackage = UObject::FindPackage( NULL, *PackageName );
		if( !ExistPackage )
			ExistPackage = UObject::CreatePackage( NULL, *PackageName );

		if( !ExistPackage->IsFullyLoaded() )
		{	
			// Ask user to fully load
			if(appMsgf( AMT_YesNo, *FString::Printf( LocalizeSecure(LocalizeUnrealEd(TEXT("NeedsToFullyLoadPackageF")), *ExistPackage->GetName(), TEXT("Merge")) ) ) )
			{
				// Fully load package.
				GWarn->BeginSlowTask( *LocalizeUnrealEd(TEXT("FullyLoadingPackages")), TRUE );
				ExistPackage->FullyLoad();
				GWarn->EndSlowTask();
			}
			// User declined abort operation.
			else
			{
				debugf(TEXT("Aborting operation as %s was not fully loaded."),*ExistPackage->GetName());
				return;
			}
		}

		UMaterialInstance* NewMIC = Cast<UMaterialInstance>( UObject::StaticConstructObject(
			UMaterialInstanceConstant::StaticClass(), ExistPackage, *MatName, RF_Public|RF_Standalone ) );
		NewMIC->SetParent( pMergedMesh->NewMaterialTemplate );

		if( NewDiffTex )
			NewMIC->SetTextureParameterValue( "Diffuse_Map", NewDiffTex );
		if( NewSpecTex )
			NewMIC->SetTextureParameterValue( "Specular_Map", NewSpecTex );
		if( NewNormTex )
			NewMIC->SetTextureParameterValue( "Normal_Map", NewNormTex );

		if( pNewMesh != NULL && pNewMesh->Materials.Num() > 0 )
			pNewMesh->Materials(0) = NewMIC;

		NewMIC->MarkPackageDirty();

		GCallbackEvent->Send( FCallbackEventParameters( 
			NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, NewMIC ) );
	}

	GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_UpdatePackageList));
}
// ming@snda]

void WxModelViewer::GenerateCollision()
{
	if (!AvatarObject)
	{
		return;
	}

	FLOAT PreScale = PreviewSkelComp->Scale;
	PreviewSkelComp->Scale = AvatarObject->MeshScale;
	PreviewSkelComp->BeginDeferredUpdateTransform();
	PreviewSkelComp->ForceSkelUpdate();

	FLOAT MaxRadius = 0.f;
	FLOAT MaxHeight = 0.f;
	for (INT i = 0; i < PreviewSkelComp->SpaceBases.Num(); ++i)
	{
		FVector Pos = PreviewSkelComp->SpaceBases(i).GetOrigin();
		FLOAT Size = Pos.Size2D();
		if (Size > MaxRadius)
		{
			MaxRadius = Size;
		}

		if (abs(Pos.Z) > MaxHeight)
		{
			MaxHeight = abs(Pos.Z);
		}
	}

	AvatarObject->CollisionRadius = (MaxRadius > 0.f ? MaxRadius : 5.f);
	AvatarObject->CollisionHeight = (MaxHeight > 0.f ? MaxHeight : 5.f);

	PreviewSkelComp->Scale = PreScale;
	PreviewSkelComp->BeginDeferredUpdateTransform();
	PreviewSkelComp->ForceSkelUpdate();

	AvatarObject->MarkPackageDirty();
}

void WxModelViewer::OnGenerateCollision( wxCommandEvent& In )
{
	AvatarObject->MeshScale = 1.0f;
	GenerateCollision();
}

void WxModelViewer::OnDuplicateDeadEffects(wxCommandEvent& In)
{
	for (TFieldIterator<UProperty> It(AvatarObject->GetClass(), FALSE); It; ++It)
	{
		UProperty* p = *It;
		if (p->GetName() == FString(TEXT("OverrideExplosionEffects")))
		{
			DuplicateComponents(p, (BYTE*)AvatarObject);
		}
	}

	AvatarObject->MarkPackageDirty();
	MasterProperty->RequestReconnectToData();
}

void WxModelViewer::DuplicateComponents(UProperty* InProperty, BYTE* InAddr)
{
	check(InProperty);
	check(InAddr);

	if (InProperty->IsA(UObjectProperty::StaticClass()))
	{
		UPropertyValue upv;
		UBOOL bRet = InProperty->GetPropertyValue(InAddr + InProperty->Offset, upv);
		check(bRet);

		UObject* ObjectValue = upv.ObjectValue;
		if (!ObjectValue)
		{
			return;
		}

		if (InProperty->PropertyFlags & CPF_EditInline)
		{
			UObject* newObj = ConstructObject<UObject>(ObjectValue->GetClass(), AvatarObject, NAME_None, 0, ObjectValue);
			check(newObj);
			upv.ObjectValue = newObj;
			InProperty->SetPropertyValue(InAddr + InProperty->Offset, upv);

			for (TFieldIterator<UProperty> It(newObj->GetClass(), FALSE); It; ++It)
			{
				DuplicateComponents(*It, (BYTE*)newObj);
			}
		}
	}
	else if (InProperty->IsA(UStructProperty::StaticClass()))
	{
		UStructProperty* usp = (UStructProperty*)InProperty;
		for (TFieldIterator<UProperty> It(usp->Struct, FALSE); It; ++It)
		{
			DuplicateComponents(*It, InAddr + InProperty->Offset);
		}
	}
	else if (InProperty->IsA(UArrayProperty::StaticClass()))
	{
		UPropertyValue upv;
		UBOOL bRet = InProperty->GetPropertyValue(InAddr + InProperty->Offset, upv);
		check(bRet);

		UArrayProperty* uap = (UArrayProperty*)InProperty;
		FScriptArray* sarray = upv.ArrayValue;
		for (INT i = 0; i < sarray->Num(); ++i)
		{
			DuplicateComponents(uap->Inner, (BYTE*)sarray->GetData() + uap->Inner->ElementSize * i);
		}
	}
}

void WxModelViewer::OnResizeToMaleWarrior(wxCommandEvent& In)
{
	AvatarObject->AvatarDrawScale = 1.0f;
	AvatarObject->MeshScale = 1.0f;
	GenerateCollision();

	FLOAT NormalHeight = 120.f;
	// load male warrior data
	// AvatarData'AvatarListData.Avatar_Player_1'
	UAvatarData* ad = Cast<UAvatarData>(UObject::StaticLoadObject(UAvatarData::StaticClass(), NULL, TEXT("AvatarListData.Avatar_Player_1"), NULL,  LOAD_NoWarn, NULL));
	if (ad)
	{
		NormalHeight = ad->CollisionHeight;
		if (NormalHeight <= 0.f)
		{
			NormalHeight = 120.f;
		}
	}


	FLOAT Rate = NormalHeight / AvatarObject->CollisionHeight;
	AvatarObject->CollisionRadius *= Rate;
	AvatarObject->CollisionHeight = NormalHeight;
	AvatarObject->MeshScale = Rate;
	AvatarObject->MarkPackageDirty();
	MasterProperty->RequestReconnectToData();
}

void WxModelViewer::OnShowCollision( wxCommandEvent& In )
{
	if (In.IsChecked())
	{
		GetPreviewVC()->ShowFlags |= SHOW_Collision;
	}
	else
	{
		GetPreviewVC()->ShowFlags &= ~SHOW_Collision;
	}
}

void WxModelViewer::OnCheckPhysic( wxCommandEvent& In )
{
	if (!AvatarObject)
	{
		return;
	}

	GWarn->MapCheck_Clear();

	if (AvatarObject->SkeletalMesh == NULL)
	{
		GWarn->MapCheck_Add(MCTYPE_WARNING, NULL, TEXT("SkeletalMesh Is NULL") ,NULL);
		GWarn->MapCheck_Show();
		return;
	}

	AvatarObject->DynamicLoadPhysicAsset();

	MasterProperty->RequestReconnectToData();
	//SecondProperty->Rebuild();
	//GCallbackEvent->Send(CALLBACK_ObjectPropertyChanged, AvatarObject);

	if (AvatarObject->PhysicAsset == NULL)
	{
		FString ToLoadName = AvatarObject->SkeletalMesh->GetPathName() + FString(TEXT("_Physics"));
		FString Msg = FString::Printf(TEXT("Can not Load Object:%s"), *ToLoadName);
		GWarn->MapCheck_Add(MCTYPE_WARNING, NULL, *Msg, NULL);
	}
	else
	{
		GWarn->MapCheck_Add(MCTYPE_INFO, NULL, TEXT("OK! Found the PhysicAsset!"), NULL);
	}
	GWarn->MapCheck_Show();
}
void WxModelViewer::OnCheckSocketCommon( wxCommandEvent& In )
{
	GWarn->MapCheck_Clear();

	if (!AvatarObject || !CheckData)
	{
		return;
	}

	for (INT i = 0; i < CheckData->CommonSocketList.Num(); ++i)
	{
		FName ToCheckName = CheckData->CommonSocketList(i);
		USkeletalMeshSocket* ToCheckSocket = PreviewSkelComp->SkeletalMesh->FindSocket(ToCheckName);
		FString Msg;
		if (!ToCheckSocket)
		{
			Msg = FString::Printf(TEXT("Can not find Socket:%s"), *ToCheckName.ToString());
			GWarn->MapCheck_Add(MCTYPE_WARNING, NULL, *Msg, NULL);
		}
		else
		{
			Msg = FString::Printf(TEXT("Check socket(%s) : OK"), *ToCheckName.ToString());
			GWarn->MapCheck_Add(MCTYPE_INFO, NULL, *Msg, NULL);
		}
	}

	GWarn->MapCheck_Show();
}

void WxModelViewer::OnCheckSocketHand( wxCommandEvent& In )
{
	GWarn->MapCheck_Clear();

	if (!AvatarObject || !CheckData)
	{
		return;
	}

	for (INT i = 0; i < CheckData->HandSocketList.Num(); ++i)
	{
		FName ToCheckName = CheckData->HandSocketList(i);
		USkeletalMeshSocket* ToCheckSocket = PreviewSkelComp->SkeletalMesh->FindSocket(ToCheckName);
		FString Msg;
		if (!ToCheckSocket)
		{
			Msg = FString::Printf(TEXT("Can not find Socket:%s"), *ToCheckName.ToString());
			GWarn->MapCheck_Add(MCTYPE_WARNING, NULL, *Msg, NULL);
		}
		else
		{
			Msg = FString::Printf(TEXT("Check socket(%s) : OK"), *ToCheckName.ToString());
			GWarn->MapCheck_Add(MCTYPE_INFO, NULL, *Msg, NULL);
		}
	}

	GWarn->MapCheck_Show();

}

void WxModelViewer::OnCheckSocketWeapon( wxCommandEvent& In )
{
	GWarn->MapCheck_Clear();

	if (!AvatarObject || !CheckData)
	{
		return;
	}

	for (INT i = 0; i < CheckData->WeaponSocketList.Num(); ++i)
	{
		FName ToCheckName = CheckData->WeaponSocketList(i);
		USkeletalMeshSocket* ToCheckSocket = PreviewSkelComp->SkeletalMesh->FindSocket(ToCheckName);
		FString Msg;
		if (!ToCheckSocket)
		{
			Msg = FString::Printf(TEXT("Can not find Socket:%s"), *ToCheckName.ToString());
			GWarn->MapCheck_Add(MCTYPE_WARNING, NULL, *Msg, NULL);
		}
		else
		{
			Msg = FString::Printf(TEXT("Check socket(%s) : OK"), *ToCheckName.ToString());
			GWarn->MapCheck_Add(MCTYPE_INFO, NULL, *Msg, NULL);
		}
	}

	GWarn->MapCheck_Show();
}

void WxModelViewer::OnCheckAnim( wxCommandEvent& In )
{
	GWarn->MapCheck_Clear();

	if (!AvatarObject || !CheckData)
	{
		return;
	}

	for (INT i = 0; i < CheckData->CommonAnimList.Num(); ++i)
	{
		FName ToCheckName = CheckData->CommonAnimList(i);
		UAnimSequence* ToCheckAnim = PreviewSkelComp->FindAnimSequence(ToCheckName);
		FString Msg;
		if (!ToCheckAnim)
		{
			Msg = FString::Printf(TEXT("Can not find Sequence:%s"), *ToCheckName.ToString());
			GWarn->MapCheck_Add(MCTYPE_WARNING, NULL, *Msg, NULL);
		}
		else
		{
			Msg = FString::Printf(TEXT("Check AnimSequence(%s) : OK"), *ToCheckName.ToString());
			GWarn->MapCheck_Add(MCTYPE_INFO, NULL, *Msg, NULL);
		}
	}

	GWarn->MapCheck_Show();

}

/** Called to mark the beginning of a transaction entry for undo/redo 
 * Be sure to call Modify() on the UObject afterward
 * @param pcTransaction - unique transaction identifier
 */
bool WxModelViewer::BeginTransaction(const TCHAR* pcTransaction)
{
	if (TransactionInProgress())
	{
		FString kError(*LocalizeUnrealEd("Error_FailedToBeginTransaction"));
		kError += kTransactionName;
		checkf(0, TEXT("%s"), *kError);
		return FALSE;
	}

	GEditor->BeginTransaction(pcTransaction);
	kTransactionName = FString(pcTransaction);
	bTransactionInProgress = TRUE;

	return TRUE;
}

/** Called to mark the end of a transaction entry for undo/redo 
 * @param pcTransaction - unique transaction identifier (must match last call to BeginTransaction()
 */
bool WxModelViewer::EndTransaction(const TCHAR* pcTransaction)
{
	if (!TransactionInProgress())
	{
		FString kError(*LocalizeUnrealEd("Error_FailedToEndTransaction"));
		kError += kTransactionName;
		checkf(0, TEXT("%s"), *kError);
		return FALSE;
	}

	if (appStrcmp(*kTransactionName, pcTransaction) != 0)
	{
		debugf(TEXT("AnimSetViewer -   EndTransaction = %s --- Curr = %s"), 
			pcTransaction, *kTransactionName);
		return FALSE;
	}

	GEditor->EndTransaction();

	kTransactionName = TEXT("");
	bTransactionInProgress = FALSE;

	return TRUE;
}

/** Called to rollback previous begin/end transaction */
void WxModelViewer::UndoTransaction()
{
	GUnrealEd->Exec( TEXT("TRANSACTION UNDO") );
}

/** Called to redo the previously undone begin/end transaction */
void WxModelViewer::RedoTransaction()
{
	GUnrealEd->Exec( TEXT("TRANSACTION REDO") );
}

/** Have we called BeginTransaction() without an EndTransaction() */
bool WxModelViewer::TransactionInProgress()
{
	return bTransactionInProgress;
}

/** Handle global file changed events. */
void WxModelViewer::Send( ECallbackEventType InType, const FString& InString, UObject* InObject)
{
	if (bListenForFileChanges)
	{
		FFilename FileName = InString;
		FString FileExtension = FileName.GetExtension();
		//extra guard in case of other file listeners sending events
		//if (!((FileExtension == "psk") || (FileExtension == "psa") || (FileExtension == "fbx")))
		// for now we only support PSA. 
		// PSK takes too long to import and FBX is still WIP
		if (!(FileExtension == "psa"))
		{
			//not one of the file extensions to listen for.  Reject.
			return;
		}

		if ( !SelectedAnimSet )
		{
			return;
		}

		FString AnimationName = FileName.GetBaseFilename();

		//assume this isn't a file we care about
// 		UBOOL bPerformImport = NeedToReimportAnimations(*FileName, SelectedAnimSet);
// 
// 		// if we need to reimport
// 		if (bPerformImport)
// 		{
// 			//Actually perform the import of the animation
// 			GApp->LastDir[LD_PSA] = FileName.GetPath(); // Save path as default for next time.
// 
// 			UEditorEngine::ImportPSAIntoAnimSet( SelectedAnimSet, *FileName, SelectedSkelMesh, TRUE );
// 
// 			SelectedAnimSet->MarkPackageDirty();
// 
// 			// balloon message
// 			FString BalloonMessage = FString::Printf(TEXT("Animation %s Reimported: Check log for detail messages. "), *AnimationName);
// 
// 			// Display, via balloon, the results of the auto-import
// 			FShowBalloonNotification::ShowNotification( LocalizeUnrealEd("AnimSetEditor_AutoReimportTitle"), BalloonMessage, ID_BALLOON_NOTIFY_ID );
// 			// log out to log window - balloon message does not work on 64 bit
// 			debugf(*BalloonMessage);
// 
// 			//cause a refresh of the available animation sets
// 			UpdateAnimSeqList();
// 			//reselect what was previously selected
// 			SetSelectedAnimSequence( SelectedAnimSeq );
// 		}
	}
}

/**
 * This function is called when the window has been selected from within the ctrl + tab dialog.
 */
void WxModelViewer::OnSelected()
{
	Raise();
}

/**
* Called once to load AnimSetViewer settings, including window position.
*/

void WxModelViewer::LoadSettings()
{
	//// Load Window Position.
	//FWindowUtil::LoadPosSize(TEXT("AbilityEditor"), this, 256,256,1200,800);

	//// Load the preview scene
	//FEditorModelViewportClient* AbilityEditorClient = GetPreviewVC();
	//check( AbilityEditorClient );
	//AbilityEditorClient->PreviewScene.LoadSettings( TEXT( "AbilityEditor" ) );


	//// loading sash position
	//FLOAT PosBase = 0.7;
	//FLOAT PosLeft = 0.7;
	//FLOAT PosRight = 0.4;

	//GConfig->GetFloat(TEXT("AbilityEditor"), TEXT("BaseSash"), PosBase, GEditorUserSettingsIni);
	//GConfig->GetFloat(TEXT("AbilityEditor"), TEXT("LeftSash"), PosLeft, GEditorUserSettingsIni);
	//GConfig->GetFloat(TEXT("AbilityEditor"), TEXT("RightSash"), PosRight, GEditorUserSettingsIni);

	//BaseWindow->SetSashPosition(BaseWindow->GetClientSize().GetWidth()*PosBase, true);
	//LeftArea->SetSashPosition(LeftArea->GetClientSize().GetHeight()*PosLeft, true);
	//RightArea->SetSashPosition(RightArea->GetClientSize().GetHeight()*PosRight, true);

}

/**
* Writes out values we want to save to the INI file.
*/

void WxModelViewer::SaveSettings()
{
	////SaveDockingLayout();

	//// Save the preview scene
	//FEditorModelViewportClient* AbilityEditorClient = GetPreviewVC();
	//check( AbilityEditorClient );
	//AbilityEditorClient->PreviewScene.SaveSettings( TEXT( "AbilityEditor" ) );

	////// Update the custom fov setting, so the last closed animset editor window is the saved setting
	////if ( GApp && GApp->EditorFrame && GApp->EditorFrame->ViewportConfigData && ToolBar && ToolBar->FOVSlider )
	////{
	////	GApp->EditorFrame->ViewportConfigData->SetCustomFOV( ToolBar->FOVSlider->GetValue() );
	////}

	//// Save Window Position.
	//FWindowUtil::SavePosSize(TEXT("AbilityEditor"), this);

	//// save sash postion
	//FLOAT PosBase = FLOAT(BaseWindow->GetSashPosition())/FLOAT(BaseWindow->GetClientSize().GetWidth());
	//FLOAT PosLeft = FLOAT(LeftArea->GetSashPosition())/FLOAT(LeftArea->GetClientSize().GetHeight());
	//FLOAT PosRight = FLOAT(RightArea->GetSashPosition())/FLOAT(RightArea->GetClientSize().GetHeight());
	//GConfig->SetFloat(TEXT("AbilityEditor"), TEXT("BaseSash"), PosBase, GEditorUserSettingsIni);
	//GConfig->SetFloat(TEXT("AbilityEditor"), TEXT("LeftSash"), PosLeft, GEditorUserSettingsIni);
	//GConfig->SetFloat(TEXT("AbilityEditor"), TEXT("RightSash"), PosRight, GEditorUserSettingsIni);
}



/**
 * Returns a handle to the anim set viewer's preview viewport client.
 */
FEditorModelViewportClient* WxModelViewer::GetPreviewVC()
{
	checkSlow( PreviewWindow );
	return PreviewWindow->ASVPreviewVC;
}

/**
 * Serializes the preview scene so it isn't garbage collected
 *
 * @param Ar The archive to serialize with
 */
void WxModelViewer::Serialize(FArchive& Ar)
{
	FEditorModelViewportClient* ASVPreviewVC = GetPreviewVC();
	check( ASVPreviewVC );

	ASVPreviewVC->Serialize(Ar);
	//Ar << SocketPreviews;
}

///////////////////////////////////////////////////////////////////////////////////////
// Properties window NotifyHook stuff

void WxModelViewer::NotifyDestroy( void* Src )
{

}

void WxModelViewer::NotifyPreChange( void* Src, UProperty* PropertyAboutToChange )
{
//	if( PropertyAboutToChange )
//	{
//		// If it is the FaceFXAsset, NULL out the DefaultSkelMesh.
//		if( PropertyAboutToChange->GetFName() == FName(TEXT("FaceFXAsset")) )
//		{
//			if( SelectedSkelMesh && SelectedSkelMesh->FaceFXAsset )
//			{
//				SelectedSkelMesh->FaceFXAsset->DefaultSkelMesh = NULL;
//#if WITH_FACEFX
//				OC3Ent::Face::FxActor* Actor = SelectedSkelMesh->FaceFXAsset->GetFxActor();
//				if( Actor )
//				{
//					Actor->SetShouldClientRelink(FxTrue);
//				}
//#endif
//			}
//		}
//	}
}

void WxModelViewer::NotifyPostChange( void* Src, UProperty* PropertyThatChanged )
{
	if (PropertyThatChanged->GetName() == FString(TEXT("SkeletalMesh"))
		|| PropertyThatChanged->GetName() == FString(TEXT("DefaultMeshListTemplate"))
		|| PropertyThatChanged->GetName() == FString(TEXT("RandomMeshListTemplate")))
	{
		ResetObject(ModelObject);
	}
	else if (PropertyThatChanged->GetName() == FString(TEXT("Materials")))
	{
		UpdateMaterial();
	}
	else if (PropertyThatChanged->GetName() == FString(TEXT("WeaponAnimSetPairs"))
		|| PropertyThatChanged->GetName() == FString(TEXT("WeaponType"))
		|| PropertyThatChanged->GetName() == FString(TEXT("AnimSets")))
	{
		UpdateAnimInfos();
	}
	else if (PropertyThatChanged->GetName() == FString(TEXT("AttachedParticles"))
		|| PropertyThatChanged->GetName() == FString(TEXT("AttachSocket"))
		|| PropertyThatChanged->GetName() == FString(TEXT("EmitterTemplate"))
		|| PropertyThatChanged->GetName() == FString(TEXT("Scale")))
	{
		UpdateAttachment();
	}

	//if (PropertyThatChanged)
	//{
	//	if (AbilityObject)
	//	{
	//		AbilityObject->MarkPackageDirty();
	//	}
	//}

	

	//RefreshData();

	//RefreshAnimComponentTime(EAST_Spell);
	//RefreshAnimComponentTime(EAST_Cast);

//	if( PropertyThatChanged )
//	{
//		// If it is the FaceFXAsset, NULL out the DefaultSkelMesh.
//		if( PropertyThatChanged->GetFName() == FName(TEXT("FaceFXAsset")) )
//		{
//			if( SelectedSkelMesh && SelectedSkelMesh->FaceFXAsset )
//			{
//				SelectedSkelMesh->FaceFXAsset->DefaultSkelMesh = SelectedSkelMesh;
//#if WITH_FACEFX
//				OC3Ent::Face::FxActor* Actor = SelectedSkelMesh->FaceFXAsset->GetFxActor();
//				if( Actor )
//				{
//					Actor->SetShouldClientRelink(FxTrue);
//				}
//#endif
//				SelectedSkelMesh->FaceFXAsset->MarkPackageDirty();
//			}
//		}
//		else if(PropertyThatChanged->GetFName() == FName(TEXT("ClothingAssets")))
//		{
//			// If a property relating to the clothing asset assignment has changed, rebuild the apex clothing binding
//			if ( PreviewSkelComp )
//			{
//				PreviewSkelComp->ReleaseApexClothing();
//				PreviewSkelComp->InitApexClothing(RBPhysScene);
//			}
//		}
//		else if(PropertyThatChanged->GetFName() == FName(TEXT("bForceCPUSkinning")))
//		{
//			FComponentReattachContext ReattachContext(PreviewSkelComp);
//			FComponentReattachContext ReattachContextRaw(PreviewSkelCompRaw);
//			FComponentReattachContext ReattachContext1(PreviewSkelCompAux1);
//			FComponentReattachContext ReattachContext2(PreviewSkelCompAux2);
//			FComponentReattachContext ReattachContext3(PreviewSkelCompAux3);
//		}
//
//		//@todo. Do this the 'right' way...
//		bResampleAnimNotifyData = FALSE;
//		UObject* Outer = PropertyThatChanged->GetOuter();
//		UScriptStruct* ScriptStruct = Cast<UScriptStruct>(Outer);
//		if (ScriptStruct != NULL)
//		{
//			FAnimNotifyEvent* Event = (FAnimNotifyEvent*)ScriptStruct;
//			if (ScriptStruct->GetName() == TEXT("AnimNotifyEvent"))
//			{
//				bResampleAnimNotifyData = TRUE;
//			}
//		}
//		UAnimNotify_Trails* TrailsNotify = Cast<UAnimNotify_Trails>(Outer);
//		if (TrailsNotify)
//		{
//			bResampleAnimNotifyData = TRUE;
//		}
//
//		UClass* OuterClass = Cast<UClass>(Outer);
//		if (OuterClass)
//		{
//			if (OuterClass->GetName() == TEXT("AnimNotify_Trails"))
//			{
//				bResampleAnimNotifyData = TRUE;
//			}
//		}
//	}
//
//	// Might have changed UseTranslationBoneNames array. We have to fix any existing FAnimSetMeshLinkup objects in selected AnimSet.
//	// Would be nice to not do this all the time, but PropertyThatChanged seems flaky when editing arrays (sometimes NULL)
//	if( SelectedAnimSet)
//	{
//		SelectedAnimSet->BoneUseAnimTranslation.Empty();
//		SelectedAnimSet->ForceUseMeshTranslation.Empty();
//		SelectedAnimSet->LinkupCache.Empty();
//		SelectedAnimSet->SkelMesh2LinkupCache.Empty();
//
//		// We need to re-init any skeletal mesh components now, because they might still have references to linkups in this set.
//		for(TObjectIterator<USkeletalMeshComponent> It;It;++It)
//		{
//			USkeletalMeshComponent* SkelComp = *It;
//			if(!SkelComp->IsPendingKill() && !SkelComp->IsTemplate())
//			{
//				SkelComp->InitAnimTree();
//			}
//		}
//	}
//
//	// Might have changed ClothBones array - so need to rebuild cloth data.
//	// Would be nice to not do this all the time, but PropertyThatChanged seems flaky when editing arrays (sometimes NULL)
//	{
//		PreviewSkelComp->TermClothSim(NULL);
//		check(SelectedSkelMesh);
//		SelectedSkelMesh->ClearClothMeshCache();
//		SelectedSkelMesh->BuildClothMapping();
//		if(PreviewSkelComp->bEnableClothSimulation)
//		{
//			PreviewSkelComp->InitClothSim(RBPhysScene);
//		}
//		// Push any changes to cloth param to the running cloth sim.
//		PreviewSkelComp->UpdateClothParams();
//
//		PreviewSkelComp->TermSoftBodySim(NULL);
//		if(PreviewSkelComp->bEnableSoftBodySimulation)
//		{
//			PreviewSkelComp->InitSoftBodySim(RBPhysScene, TRUE);
//		}
//		PreviewSkelComp->UpdateSoftBodyParams();
//	}
//
//
//	// In case we change Origin/RotOrigin, update the components.
//	UpdateSkelComponents();
}

void WxModelViewer::NotifyExec( void* Src, const TCHAR* Cmd )
{
	GUnrealEd->NotifyExec(Src, Cmd);
}



/** When the user slides the progressive drawing slider */
void WxModelViewer::OnProgressiveSliderChanged(wxScrollEvent& In)
{
	PreviewSkelComp->ProgressiveDrawingFraction = ((FLOAT)In.GetPosition()) / 10000.f;
}


/**
 *	This function returns the name of the docking parent.  This name is used for saving and loading the layout files.
 *  @return A string representing a name to use for this docking parent.
 */
const TCHAR* WxModelViewer::GetDockingParentName() const
{
	return TEXT("ModelViewer");
}

/**
 * @return The current version of the docking parent, this value needs to be increased every time new docking windows are added or removed.
 */
const INT WxModelViewer::GetDockingParentVersion() const
{
	return 0;
}

/**
 *	Update the preview window.
 */
void WxModelViewer::UpdatePreviewWindow()
{
	if (PreviewWindow)
	{
		if (PreviewWindow->ASVPreviewVC)
		{
			if (PreviewWindow->ASVPreviewVC->Viewport)
			{
				PreviewWindow->ASVPreviewVC->Viewport->Invalidate();
			}
		}
	}
}

/** Returns the current LOD index being viewed. */
INT WxModelViewer::GetCurrentLODIndex() const
{
	INT LODIndex = 0;
	if ( PreviewSkelComp )
	{
		if ( PreviewSkelComp->ForcedLodModel > 0 )
		{
			LODIndex = PreviewSkelComp->ForcedLodModel - 1;
		}
	}
	return LODIndex;
}


WxMEMenuBar::WxMEMenuBar()
{
	ViewMenu = new wxMenu();
	Append(ViewMenu, TEXT("View"));

	EditMenu = new wxMenu();
	Append(EditMenu, TEXT("Edit"));
	EditMenu->Append(IDM_MODEL_GENERATE_COLLISION, TEXT("Generate Collision"));
	EditMenu->Append(IDM_MODEL_DUPLICATE_DEAD_EFFECTS, TEXT("Duplicate dead effects"));
// 	EditMenu->Append(IDM_MODEL_RESIZE_TO_MALE_WARRIOR, TEXT("ResizeToMaleWarriorHeight"));

	CheckMenu = new wxMenu();
	Append(CheckMenu, TEXT("Check"));
	CheckMenu->Append(IDM_MODEL_CHECK_PHYSIC_DATA, TEXT("Check/Reload Physic"));
	CheckMenu->AppendSeparator();
	CheckMenu->Append(IDM_MODEL_CHECK_ANIM_DATA, TEXT("Check Anim"));
	CheckMenu->AppendSeparator();
	CheckMenu->Append(IDM_MODEL_CHECK_SOCKET_DATA_COMMON, TEXT("Check CommonSocket"));
	CheckMenu->Append(IDM_MODEL_CHECK_SOCKET_DATA_HAND, TEXT("Check HandSocket"));
	CheckMenu->Append(IDM_MODEL_CHECK_SOCKET_DATA_WEAPON, TEXT("Check WeaponSocket"));


//	// File Menu
//	FileMenu = new wxMenu();
//	Append( FileMenu, *LocalizeUnrealEd("File") );
//
//	FileMenu->Append( IDM_ANIMSET_IMPORTMESHLOD, *LocalizeUnrealEd("ImportMeshLOD"), TEXT("") );
//	FileMenu->AppendSeparator();
//	FileMenu->Append( IDM_ANIMSET_NEWANISMET, *LocalizeUnrealEd("NewAnimSet"), TEXT("") );
//#if WITH_ACTORX
//	FileMenu->Append( IDM_ANIMSET_IMPORTPSA, *LocalizeUnrealEd("ImportPSA"), TEXT("") );
//#endif
//#if WITH_FBX
//	FileMenu->Append( IDM_ANIMSET_IMPORTFBXANIM, *LocalizeUnrealEd("ImportFbxAnim"), TEXT("") );
//	FileMenu->Append( IDM_ANIMSET_EXPORTFBXANIM, *LocalizeUnrealEd("ExportFbxAnim"), TEXT("") );
//#endif // WITH_FBX
//	FileMenu->AppendSeparator();
//	FileMenu->Append( IDM_ANIMSET_NEWMORPHSET, *LocalizeUnrealEd("NewMorphTargetSet"), TEXT("") );
//	FileMenu->Append( IDM_ANIMSET_IMPORTMORPHTARGET, *LocalizeUnrealEd("ImportMorphTarget"), TEXT("") );
//	FileMenu->Append( IDM_ANIMSET_IMPORTMORPHTARGETS, *LocalizeUnrealEd("ImportMorphTargets"), TEXT("") );
//	FileMenu->Append( IDM_ANIMSET_IMPORTMORPHTARGET_LOD, *LocalizeUnrealEd("ImportMorphTargetLOD"), TEXT("") );
//	FileMenu->Append( IDM_ANIMSET_IMPORTMORPHTARGETS_LOD, *LocalizeUnrealEd("ImportMorphTargetsLOD"), TEXT("") );
//
//	// Edit Menu
//	EditMenu = new wxMenu();
//	Append( EditMenu, *LocalizeUnrealEd("Edit") );
//	EditMenu->Append( IDM_ANIMSET_UNDO, *LocalizeUnrealEd("Undo"), *LocalizeUnrealEd("ToolTip_86") );
//	EditMenu->Append( IDM_ANIMSET_REDO, *LocalizeUnrealEd("Redo"), *LocalizeUnrealEd("ToolTip_87") );
//
//	// View Menu
//	ViewMenu = new wxMenu();
//	Append( ViewMenu, *LocalizeUnrealEd("View") );
//
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWBONES, *LocalizeUnrealEd("ShowSkeleton"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWADDITIVEBASE, *LocalizeUnrealEd("ShowAdditiveBase"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWBONENAMES, *LocalizeUnrealEd("ShowBoneNames"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWREFPOSE, *LocalizeUnrealEd("ShowReferencePose"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWWIREFRAME, *LocalizeUnrealEd("ToolTip_5"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWMIRROR, *LocalizeUnrealEd("ShowMirror"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWFLOOR, *LocalizeUnrealEd("ShowFloor"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWGRID, *LocalizeUnrealEd("ShowGrid"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWSOCKETS, *LocalizeUnrealEd("ShowSockets"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWBOUNDS, *LocalizeUnrealEd("ShowBounds"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWCOLLISION, *LocalizeUnrealEd("ShowCollision"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_ShowRawAnimation, *LocalizeUnrealEd("ShowUncompressedAnimation"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWSOFTBODYTETRA, *LocalizeUnrealEd("ShowSoftBodyTetra"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWMORPHKEYS, *LocalizeUnrealEd("ShowMorphKeys"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWALTBONEWEIGHTINGMODE, *LocalizeUnrealEd("ShowAltBoneWeightingEdit"), TEXT("") );
//	ViewMenu->AppendCheckItem( IDM_ANIMSET_VIEWCLOTHMOVEDISTSCALE, *LocalizeUnrealEd("ShowClothMovementScale"), TEXT("") );
//
//
//	// Tangent/Normal/Mirror stuff
//	ViewMenu->AppendSeparator();
//	TangentSubMenu = new wxMenu();
//	ViewMenu->Append(-1, *LocalizeUnrealEd("ShowTangent"), TangentSubMenu );
//	TangentSubMenu->AppendCheckItem (IDM_ANIMSET_VERTEX_SHOWTANGENT_AS_VECTOR, *LocalizeUnrealEd("AsVector"));
//	TangentSubMenu->AppendCheckItem (IDM_ANIMSET_VERTEX_SHOWTANGENT_AS_TEXTURE, *LocalizeUnrealEd("AsTexture"));
//
//	NormalSubMenu = new wxMenu();
//	ViewMenu->Append(-1, *LocalizeUnrealEd("ShowNormal"), NormalSubMenu );
//	NormalSubMenu->AppendCheckItem (IDM_ANIMSET_VERTEX_SHOWNORMAL_AS_VECTOR, *LocalizeUnrealEd("AsVector"));
//	NormalSubMenu->AppendCheckItem (IDM_ANIMSET_VERTEX_SHOWNORMAL_AS_TEXTURE, *LocalizeUnrealEd("AsTexture"));
//
//	ViewMenu->AppendCheckItem(IDM_ANIMSET_VERTEX_SHOWMIRROR, *LocalizeUnrealEd("ShowMirror") );
//
//	SectionSubMenu = new wxMenu();
//	ViewMenu->Append(-1, *LocalizeUnrealEd("ShowSection"), SectionSubMenu );
//	SectionSubMenu->AppendCheckItem (IDM_ANIMSET_VERTEX_SECTION_0, TEXT("Material Section 0"));
//	SectionSubMenu->AppendCheckItem (IDM_ANIMSET_VERTEX_SECTION_1, TEXT("Material Section 1"));
//	SectionSubMenu->AppendCheckItem (IDM_ANIMSET_VERTEX_SECTION_2, TEXT("Material Section 2"));
//	SectionSubMenu->AppendCheckItem (IDM_ANIMSET_VERTEX_SECTION_3, TEXT("Material Section 3"));
//	SectionSubMenu->AppendCheckItem (IDM_ANIMSET_VERTEX_SECTION_4, TEXT("Material Section 4"));
//
//	// Add a menu for displaying UV sets.
//	UVSetSubMenu = new wxMenu();
//	ViewMenu->Append(-1, *LocalizeUnrealEd("AnimSetViewer_ShowUVs"), UVSetSubMenu );
//	const UINT NumUVSetsToShow = IDM_ANIMSET_SHOW_UV_END - IDM_ANIMSET_SHOW_UV_START;
//	for( INT TexCoordIdx = 0; TexCoordIdx < NumUVSetsToShow; ++TexCoordIdx )
//	{
//		UVSetSubMenu->AppendCheckItem( IDM_ANIMSET_SHOW_UV_START+TexCoordIdx, *FString::Printf( LocalizeSecure( LocalizeUnrealEd( "AnimSetViewer_ShowUVIndex" ), TexCoordIdx ) ) );
//	}
//
//	ViewMenu->Check( IDM_ANIMSET_VIEWSOFTBODYTETRA, true );
//	ViewMenu->Check( IDM_ANIMSET_VIEWGRID, true );
//
//	// PhysX debug visualization menu
//
//	GEngine->Exec(TEXT("nxvis PHYSX_CLEAR_ALL"));
//
//	PhysXMenu = new wxMenu();
//	Append( PhysXMenu, TEXT("PhysX Debug") );
//
//	PhysXMenu->Append( IDM_ANIMSET_PHYSX_CLEAR_ALL, TEXT("PHYSX CLEAR ALL"), TEXT("") );
//	PhysX_Body = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("PhysX Body / Actor"), PhysX_Body );
//	for (INT i=IDM_ANIMSET_PHYSX_DEBUG_WORLD_AXES; i<=IDM_ANIMSET_PHYSX_DEBUG_ACTOR_AXES; i++)
//	{
//		PhysX_Body->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//
//	PhysX_Joint = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("PhysX Joints"), PhysX_Joint );
//	for (INT i=IDM_ANIMSET_PHYSX_DEBUG_JOINT_LOCAL_AXES; i<= IDM_ANIMSET_PHYSX_DEBUG_JOINT_LIMITS; i++)
//	{
//		PhysX_Joint->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//
//	PhysX_Contact = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("PhysX Contacts"), PhysX_Contact );
//	for (INT i=IDM_ANIMSET_PHYSX_DEBUG_CONTACT_POINT; i<=IDM_ANIMSET_PHYSX_DEBUG_CONTACT_FORCE; i++)
//	{
//		PhysX_Contact->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//
//
//	PhysX_Collision = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("PhysX Collision"), PhysX_Collision );
//	for (INT i=IDM_ANIMSET_PHYSX_DEBUG_COLLISION_AABBS; i<=IDM_ANIMSET_PHYSX_DEBUG_COLLISION_SKELETONS; i++)
//	{
//		PhysX_Collision->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//
//	PhysX_Fluid = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("PhysX Fluid"), PhysX_Fluid );
//	for (INT i=IDM_ANIMSET_PHYSX_DEBUG_FLUID_EMITTERS; i<=IDM_ANIMSET_PHYSX_DEBUG_FLUID_PACKET_DATA; i++)
//	{
//		PhysX_Fluid->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//
//
//	PhysX_Cloth = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("PhysX Cloth"), PhysX_Cloth );
//	for (INT i=IDM_ANIMSET_PHYSX_DEBUG_CLOTH_MESH; i<=IDM_ANIMSET_PHYSX_DEBUG_CLOTH_VALIDBOUNDS; i++)
//	{
//		PhysX_Cloth->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//
//	PhysX_SoftBody = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("PhysX Soft Body"), PhysX_SoftBody );
//	for (INT i=IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_MESH; i<=IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_VALIDBOUNDS; i++)
//	{
//		PhysX_SoftBody->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//#if WITH_APEX
//
//	GEngine->Exec(TEXT("apexvis APEX_CLEAR_ALL"));
//
//
//	PhysXMenu->AppendSeparator();
//	PhysXMenu->Append( IDM_ANIMSET_APEX_CLEAR_ALL, TEXT("APEX CLEAR ALL"), TEXT("") );
//
//
//	Apex_Clothing = new wxMenu();
//#if 0 // oscar@snda
//	PhysXMenu->Append(-1, TEXT("APEX Clothing"), Apex_Clothing );
//	for (INT i=IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_SKINNED_POSITIONS; i<=IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_ACOLLISION_UMBRELLAS; i++)
//	{
//		Apex_Clothing->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//
//	Apex_Destructible = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("APEX Destructible"), Apex_Destructible );
//	for (INT i=IDM_ANIMSET_APEX_VISUALIZE_DESTRUCTIBLE_BOUNDS; i<=IDM_ANIMSET_APEX_LOD_DISTANCE_DESTRUCTIBLE_FRAGMENT_ACTOR_POSE; i++)
//	{
//		Apex_Destructible->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//
//	Apex_Misc = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("APEX Misc"), Apex_Misc );
//	for (INT i=IDM_ANIMSET_APEX_RENDER_NORMALS; i<=IDM_ANIMSET_APEX_LOD_DISTANCE_SCALE; i++)
//	{
//		INT index = i-IDM_ANIMSET_PHYSX_CLEAR_ALL;
//		assert( index > 0 && index < (IDM_ANIMSET_LAST-IDM_ANIMSET_PHYSX_CLEAR_ALL) );
//		Apex_Misc->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//#if WITH_APEX_PARTICLES
//	Apex_Emitter = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("APEX Emitter"), Apex_Emitter );
//	for (INT i=IDM_ANIMSET_APEX_VISUALIZE_TOTAL_INJECTED_AABB; i<=IDM_ANIMSET_APEX_THRESHOLD_DISTANCE_IMPACT_EMITTER_ACTOR_NAME; i++)
//	{
//		INT index = i-IDM_ANIMSET_PHYSX_CLEAR_ALL;
//		assert( index > 0 && index < (IDM_ANIMSET_LAST-IDM_ANIMSET_PHYSX_CLEAR_ALL) );
//		Apex_Emitter->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//
//	Apex_Iofx = new wxMenu();
//	PhysXMenu->Append(-1, TEXT("APEX IFOX"), Apex_Iofx );
//	for (INT i=IDM_ANIMSET_APEX_VISUALIZE_IOFX_BOUNDING_BOX; i<=IDM_ANIMSET_APEX_VISUALIZE_IOFX_ACTOR_NAME; i++)
//	{
//		INT index = i-IDM_ANIMSET_PHYSX_CLEAR_ALL;
//		assert( index > 0 && index < (IDM_ANIMSET_LAST-IDM_ANIMSET_PHYSX_CLEAR_ALL) );
//		Apex_Iofx->AppendCheckItem(i,ANSI_TO_TCHAR( gDebugCommands[i-IDM_ANIMSET_PHYSX_CLEAR_ALL] ), TEXT("") );
//	}
//#endif
//
//#endif
//
//#endif
//
//	// Mesh Menu
//	MeshMenu = new wxMenu();
//	Append( MeshMenu, *LocalizeUnrealEd("Mesh") );
//
//	MeshMenu->AppendRadioItem( IDM_ANIMSET_LOD_AUTO, *LocalizeUnrealEd("SetLODAuto"), TEXT("") );
//	MeshMenu->AppendRadioItem( IDM_ANIMSET_LOD_BASE, *LocalizeUnrealEd("ForceLODBaseMesh"), TEXT("") );
//	MeshMenu->AppendRadioItem( IDM_ANIMSET_LOD_1, *LocalizeUnrealEd("ForceLOD1"), TEXT("") );
//	MeshMenu->AppendRadioItem( IDM_ANIMSET_LOD_2, *LocalizeUnrealEd("ForceLOD2"), TEXT("") );
//	MeshMenu->AppendRadioItem( IDM_ANIMSET_LOD_3, *LocalizeUnrealEd("ForceLOD3"), TEXT("") );
//	MeshMenu->Check( IDM_ANIMSET_LOD_AUTO, true);
//	MeshMenu->AppendSeparator();
//#if WITH_SIMPLYGON
//	MeshMenu->Append( IDM_ANIMSET_GENERATELOD, *LocalizeUnrealEd("GenerateLOD") );
//#endif // #if WITH_SIMPLYGON
//	MeshMenu->Append( IDM_ANIMSET_REMOVELOD, *LocalizeUnrealEd("RemoveLOD"), TEXT("") );
//	MeshMenu->AppendSeparator();
//	MeshMenu->Append( IDM_ANIMSET_AUTOMIRRORTABLE, *LocalizeUnrealEd("AutoMirrorTable"), TEXT("") );
//	MeshMenu->Append( IDM_ANIMSET_CHECKMIRRORTABLE, *LocalizeUnrealEd("CheckMirrorTable"), TEXT("") );
//	MeshMenu->Append( IDM_ANIMSET_COPYMIRRORTABLE, *LocalizeUnrealEd("CopyMirrorTableToClipboard"), TEXT("") );
//	MeshMenu->Append( IDM_ANIMSET_COPYMIRRORTABLEFROM, *LocalizeUnrealEd("CopyMirrorTableFromMesh"), TEXT("") );
//	MeshMenu->AppendSeparator();
//	MeshMenu->Append( IDM_ANIMSET_UPDATEBOUNDS, *LocalizeUnrealEd("UpdateBounds"), TEXT("") );
//	MeshMenu->Append( IDM_ANIMSET_COPYMESHBONES, *LocalizeUnrealEd("CopyBoneNamesToClipboard"), TEXT("") );
//	MeshMenu->Append( IDM_ANIMSET_COPYWEIGHTEDMESHBONES, *LocalizeUnrealEd("CopyWeightedBoneNamesToClipboard"), TEXT("") );
//	MeshMenu->Append( IDM_ANIMSET_MERGEMATERIALS, *LocalizeUnrealEd("MergeMeshMaterials"), TEXT("") );
//	MeshMenu->AppendSeparator();
//	MeshMenu->Append( IDM_ANIMSET_FIXUPMESHBONES, *LocalizeUnrealEd("FixupBoneNames"), TEXT("") );
//	MeshMenu->Append( IDM_ANIMSET_SWAPLODSECTIONS, *LocalizeUnrealEd("SwapLODSections"), TEXT("") );
//	MeshMenu->AppendSeparator();
//	// Socket Manager
//	MeshMenu->Append( IDM_ANIMSET_OPENSOCKETMGR, *LocalizeUnrealEd("ToolTip_6"), TEXT("") );
//
//	// AnimSet Menu
//	AnimSetMenu = new wxMenu();
//	Append( AnimSetMenu, *LocalizeUnrealEd("AnimSet") );
//
//	AnimSetMenu->Append( IDM_ANIMSET_EMPTYSET, *LocalizeUnrealEd("ResetAnimSet"), TEXT("") );
//	AnimSetMenu->AppendSeparator();
//	AnimSetMenu->Append( IDM_ANIMSET_DELETETRACK, *LocalizeUnrealEd("DeleteTrack"), TEXT("") );
//	AnimSetMenu->AppendSeparator();
//	AnimSetMenu->Append( IDM_ANIMSET_DELETEMORPHTRACK, *LocalizeUnrealEd("AnimSetViewer_DeleteMorphTracksTitle"), TEXT("") );
//	AnimSetMenu->AppendSeparator();
//	AnimSetMenu->Append( IDM_ANIMSET_COPYTRANSLATIONBONENAMES, *LocalizeUnrealEd("CopyTranslationBoneNames"), TEXT("") );
//	AnimSetMenu->AppendSeparator();
//	AnimSetMenu->Append( IDM_ANIMSET_ANALYZEANIMSET, *LocalizeUnrealEd("AnalyzeAnimSet"), TEXT("") );
//
//
//	// AnimSeq Menu
//	AnimSeqMenu = new wxMenu();
//	Append( AnimSeqMenu, *LocalizeUnrealEd("AnimSequence") );
//
//	AnimSeqMenu->Append( IDM_ANIMSET_RENAMESEQ, *LocalizeUnrealEd("RenameSequence"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_REMOVE_PREFIX, *LocalizeUnrealEd("RemovePrefixFromSelectedSequences"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_DELETESEQ, *LocalizeUnrealEd("DeleteSequence"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_COPYSEQ,  *LocalizeUnrealEd("CopySequence"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_MOVESEQ,  *LocalizeUnrealEd("MoveSequence"), TEXT("") );
//
//	AnimSeqMenu->AppendSeparator();
//	AnimSeqMenu->Append( IDM_ANIMSET_MAKESEQADDITIVE,  *LocalizeUnrealEd("MakeSequencesAdditive"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_REBUILDADDITIVE,  *LocalizeUnrealEd("RebuildAdditiveAnimation"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_ADDADDITIVETOSEQ, *LocalizeUnrealEd("AddAdditiveToSequence"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_SUBTRACTADDITIVETOSEQ, *LocalizeUnrealEd("SubtractAdditiveToSequence"), TEXT("") );
//
//	AnimSeqMenu->AppendSeparator();
//	AnimSeqMenu->Append( IDM_ANIMSET_COPYSEQNAME, *LocalizeUnrealEd("CopySequenceName"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_COPYSEQNAMELIST, *LocalizeUnrealEd("CopySequenceNameList"), TEXT("") );
//
//	AnimSeqMenu->AppendSeparator();
//	AnimSeqMenu->Append( IDM_ANIMSET_SEQAPPLYROT, *LocalizeUnrealEd("ApplyRotation"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_SEQREZERO, *LocalizeUnrealEd("ReZeroCurrentPosition"), TEXT("") );
//
//	AnimSeqMenu->AppendSeparator();
//	AnimSeqMenu->Append( IDM_ANIMSET_SEQDELBEFORE, *LocalizeUnrealEd("RemoveBeforeCurrentPosition"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_SEQDELAFTER, *LocalizeUnrealEd("RemoveAfterCurrentPosition"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_SEQDELBEGINBYTIME, *LocalizeUnrealEd("RemoveBeforeCurrentPositionByTime"), TEXT("") );
//	AnimSeqMenu->Append( IDM_ANIMSET_SEQDELENDBYTIME, *LocalizeUnrealEd("RemoveAfterCurrentPositionByTime"), TEXT("") );
//
//	// Notify Menu
//
//	NotifyMenu = new wxMenu();
//	Append( NotifyMenu, *LocalizeUnrealEd("Notifies") );
//
//	NotifyMenu->Append( IDM_ANIMSET_NOTIFYNEW, *LocalizeUnrealEd("NewNotify"), TEXT("") );
//	NotifyMenu->Append( IDM_ANIMSET_NOTIFYCOPY, *LocalizeUnrealEd("CopyNotifies"), TEXT("") );
//	NotifyMenu->Append( IDM_ANIMSET_NOTIFYSORT, *LocalizeUnrealEd("SortNotifies"), TEXT("") );
//	NotifyMenu->Append( IDM_ANIMSET_NOTIFYSHIFT, *LocalizeUnrealEd("ShiftNotifies"), TEXT("") );
//	NotifyMenu->Append( IDM_ANIMSET_NOTIFYREMOVE, *LocalizeUnrealEd("RemoveAllNotifies"), TEXT("") );
//	NotifyMenu->AppendSeparator();
//	NotifyMenu->Append( IDM_ANIMSET_NOTIFY_ENABLEALLPSYS, *LocalizeUnrealEd("NotifyEnableAllPSys"), TEXT("") );
//	NotifyMenu->Append( IDM_ANIMSET_NOTIFY_DISABLEALLPSYS, *LocalizeUnrealEd("NotifyDisableAllPSys"), TEXT("") );
//	NotifyMenu->Append( IDM_ANIMSET_NOTIFY_REFRESH_ALL_DATA, *LocalizeUnrealEd("NotifyRefreshAllData"), *LocalizeUnrealEd("NotifyRefreshAllDataHelp"));
//
//	// MorphSet Menu
//	MorphSetMenu = new wxMenu();
//	Append( MorphSetMenu, *LocalizeUnrealEd("MorphSet") );
//
//	MorphSetMenu->Append( IDM_ANIMSET_UPDATE_MORPHTARGET, *LocalizeUnrealEd("UpdateMorphTarget"), TEXT("") );
//
//	// Morph Menu
//	MorphTargetMenu = new wxMenu();
//	Append( MorphTargetMenu, *LocalizeUnrealEd("MorphTarget") );
//
//	MorphTargetMenu->Append( IDM_ANIMSET_DELETE_MORPH, *LocalizeUnrealEd("DeleteMorph"), TEXT("") );
//
//	// AnimationCompresion Menu
//	AnimationCompressionMenu = new wxMenu();
//	Append( AnimationCompressionMenu, *LocalizeUnrealEd("AnimationCompression") );
//
//	AnimationCompressionMenu->AppendCheckItem( IDM_ANIMSET_OpenAnimationCompressionDlg, *LocalizeUnrealEd("AnimationCompressionSettings"), TEXT("") );
//
//	// Alt. Bone Weight Track editing
//	AltBoneWeightingMenu = new wxMenu();
//	Append( AltBoneWeightingMenu, *LocalizeUnrealEd("AltBoneWeightingMenu") );
//
//	AltBoneWeightingMenu->Append( IDM_ANIMSET_IMPORTMESHWEIGHTS, *LocalizeUnrealEd("ImportMeshWeights"), TEXT("") );
//	AltBoneWeightingMenu->AppendCheckItem( IDM_ANIMSET_TOGGLEMESHWEIGHTS, *LocalizeUnrealEd("ToggleMeshWeights"), TEXT("") );
//	AltBoneWeightingMenu->AppendSeparator();
//	AltBoneWeightingMenu->AppendCheckItem( IDM_ANIMSET_BONEEDITING_SHOWALLMESHVERTS, *LocalizeUnrealEd("ShowAllMeshVerts"), TEXT("") );
//	AltBoneWeightingMenu->Check(IDM_ANIMSET_BONEEDITING_SHOWALLMESHVERTS, true);
//	AltBoneWeightingMenu->Append( IDM_ANIMSET_SKELETONTREE_RESETBONEBREAKS, *LocalizeUnrealEd("ResetBoneBreaks"), TEXT("") );

}
