#include "..\stdafx.h"
#include "MMOGameClasses.h"
#define MAX_SHADOW_NUM 100
IMPLEMENT_CLASS(AInstancedShadowPlaneActor)

AInstancedShadowPlaneActor* AInstancedShadowPlaneActor::g_pInstance = NULL;
void AInstancedShadowPlaneActor::PostBeginPlay()
{

	UStaticMesh* InMesh = (UStaticMesh *)UObject::StaticLoadObject(UStaticMesh::StaticClass(),NULL,*MeshPatchName,NULL, LOAD_None, NULL);

	instancedShadowPlane = ConstructObject<UInstancedStaticMeshComponent>(UInstancedStaticMeshComponent::StaticClass(),this,NAME_None,RF_Transactional);
	instancedShadowPlane->StaticMesh = InMesh;
	instancedShadowPlane->bForceDirectLightMap = TRUE;
	instancedShadowPlane->bSelectable = TRUE;
	instancedShadowPlane->bUsePerInstanceHitProxies = TRUE;
	instancedShadowPlane->bDontResolveInstancedLightmaps = TRUE;
	instancedShadowPlane->InstancingRandomSeed = appRand();
	// [ming@snda
	// avoid large amount of foliages from building lightmaps
	instancedShadowPlane->bAcceptsSkyLight = TRUE;
	instancedShadowPlane->bUsePrecomputedShadows = FALSE;
	
	instancedShadowPlane->WindInteractive = FALSE;
	//instancedShadowPlane->ReleaseWindField( this );
	currentIndex = 0;
	InstanceList.Empty( MAX_SHADOW_NUM );
	InstanceList.Add( MAX_SHADOW_NUM );
	for( int i = 0 ; i < InstanceList.Num() ; ++ i )
	{
		InstanceList.GetData()[i] = i;
	}

	initMatrix = FMatrix::Identity;
	initMatrix.SetOrigin( FVector( -100000.f, -100000.f, -100000.f) );

	instancedShadowPlane->Bounds.Origin = FVector(0,0,0);
	instancedShadowPlane->Bounds.BoxExtent = FVector( BIG_NUMBER * 0.5f , BIG_NUMBER * 0.5f, BIG_NUMBER * 0.5f );
	instancedShadowPlane->Bounds.SphereRadius = BIG_NUMBER;
	instancedShadowPlane->LocalToWorld = FMatrix::Identity;
	ApplySeting();
	AttachComponent(instancedShadowPlane);
}

INT AInstancedShadowPlaneActor::AddInstance( FVector InstanceTransform , float Scale)
{
	if( !instancedShadowPlane || currentIndex < 0 || currentIndex >= MAX_SHADOW_NUM ||
		!InstanceList.IsValidIndex(currentIndex) || GSystemSettings.GetAllowDynamicShadows() )
		return -1;
	FMatrix mar = FMatrix::Identity;
	InstanceTransform.Z += zOffset;
	mar.SetOrigin( InstanceTransform /*-  Location*/ );
	mar = mar.ApplyScale( Scale * fixedScale );
	INT index = InstanceList(currentIndex);
	if (index < 0 || index > instancedShadowPlane->PerInstanceSMData.Num())
		return -1;
	++currentIndex;
	if (index == instancedShadowPlane->PerInstanceSMData.Num())
	{
		FInstancedStaticMeshInstanceData* NewInstanceData = new(instancedShadowPlane->PerInstanceSMData) FInstancedStaticMeshInstanceData();
		NewInstanceData->Transform = mar;
		NewInstanceData->LightmapUVBias = FVector2D( -1.0f, -1.0f );
		NewInstanceData->ShadowmapUVBias = FVector2D( -1.0f, -1.0f );
	}
	else
	{
		instancedShadowPlane->PerInstanceSMData.GetData()[index].Transform = mar;
	}
	instancedShadowPlane->bNeedsReattach = TRUE;
	//instancedShadowPlane->UpdateBounds();
	//instancedShadowPlane->Modify();
	return index;
}

void AInstancedShadowPlaneActor::ReleaseInstance(INT index)
{
	if (!instancedShadowPlane || !instancedShadowPlane->PerInstanceSMData.IsValidIndex(index) ||
		currentIndex < 0 || currentIndex > InstanceList.Num())
		return;
	for (INT i = currentIndex; i < InstanceList.Num(); ++i)
	{
		if (InstanceList(i) == index)
			return;
	}
	if( currentIndex == 0 )
		return;
	InstanceList.GetData()[ --currentIndex ] = index;
	instancedShadowPlane->PerInstanceSMData.GetData()[index].Transform = initMatrix;
	instancedShadowPlane->bNeedsReattach = TRUE;
	//instancedShadowPlane->UpdateBounds();
	//instancedShadowPlane->Modify();
}

void AInstancedShadowPlaneActor::Refresh(INT index,FVector InstanceTransform)
{
	if (!instancedShadowPlane || !instancedShadowPlane->PerInstanceSMData.IsValidIndex(index) ||
		currentIndex < 0 || currentIndex > InstanceList.Num())
		return;
	for (INT i = currentIndex; i < InstanceList.Num(); ++i)
	{
		if (InstanceList(i) == index)
			return;
	}
	FMatrix mar = instancedShadowPlane->PerInstanceSMData.GetData()[index].Transform;
	InstanceTransform.Z += zOffset;
	mar.SetOrigin( InstanceTransform );
	instancedShadowPlane->PerInstanceSMData.GetData()[index].Transform = mar;
	instancedShadowPlane->bNeedsReattach = TRUE;
	//instancedShadowPlane->UpdateBounds();
	//instancedShadowPlane->Modify();
}

// UBOOL AInstancedShadowPlaneActor::Tick( FLOAT DeltaTime, enum ELevelTick TickType )
// {
// 	Super::Tick( DeltaTime, TickType );
// 	//ConditionalUpdateComponents();
// 	return true;
// }

void AInstancedShadowPlaneActor::BeginDestroy()
{
	g_pInstance = NULL;
	Super::BeginDestroy();
}

AInstancedShadowPlaneActor* AInstancedShadowPlaneActor::GetInstancedShadowPlaneActor(UBOOL bCreateIfNone)
{
	if ( g_pInstance )
		return g_pInstance;
// 	for( TObjectIterator<AInstancedShadowPlaneActor> It;It;++It )
// 	{
// 		if( It->GetOuter() == GWorld->PersistentLevel && !It->bDeleteMe )
// 		{
// 			return *It;
// 		}
// 	}
	g_pInstance = bCreateIfNone ? Cast<AInstancedShadowPlaneActor>(GWorld->SpawnActor( AInstancedShadowPlaneActor::StaticClass() )) : NULL;
	return g_pInstance;
}

void AInstancedShadowPlaneActor::ReleaseAllShadowPlaneActor()
{
// 	for( TObjectIterator<AMMOActor> It;It;++It )
// 	{
// 		It->ReleasePlanShow();
// 	}
	STLInt2IntMap* CharacterMap = MMOFacade::getSingleton().GetCharacterMap();
	STLInt2IntMap::iterator CharacterMapItor ;
	for ( CharacterMapItor= CharacterMap->begin();  CharacterMapItor !=CharacterMap->end(); CharacterMapItor++ )
	{
		AMMOActor *pActor = Cast<AMMOActor>( UObject::GetIndexedObject( CharacterMapItor->second ) );
		if ( pActor && pActor->IsA( AMMOActor::StaticClass() ))
			pActor->ReleasePlanShow();
	}
}

void AInstancedShadowPlaneActor::InitAllShadowPlaneActor()
{
	if ( !GSystemSettings.GetAllowDynamicShadows()  )
	{
		AInstancedShadowPlaneActor::GetInstancedShadowPlaneActor( true );

// 		for( TObjectIterator<AMMOActor> It;It;++It )
// 		{
// 			It->InitPlaneShow();
// 		}
		STLInt2IntMap* CharacterMap = MMOFacade::getSingleton().GetCharacterMap();
		STLInt2IntMap::iterator CharacterMapItor ;
		for ( CharacterMapItor= CharacterMap->begin();  CharacterMapItor !=CharacterMap->end(); CharacterMapItor++ )
		{
			AMMOActor *pActor = Cast<AMMOActor>( UObject::GetIndexedObject( CharacterMapItor->second ) );
			if ( pActor && pActor->IsA( AMMOActor::StaticClass() ) )
				pActor->InitPlaneShow();
		}
	}
}

void AInstancedShadowPlaneActor::NotifyOptionChange()
{
	if ( !GSystemSettings.GetAllowDynamicShadows()  )
	{
		InitAllShadowPlaneActor();
		
		//if( g_pInstance && !g_pInstance->instancedShadowPlane->GetOuter() )
			//g_pInstance->AttachComponent(g_pInstance->instancedShadowPlane);
	}
	else
	{
		ReleaseAllShadowPlaneActor();
		//if( g_pInstance && g_pInstance->instancedShadowPlane->GetOuter() )
			//g_pInstance->DetachComponent(g_pInstance->instancedShadowPlane);
	}
}

void AInstancedShadowPlaneActor::UpdateComponentsInternal(UBOOL bCollisionUpdate)
{
	Super::UpdateComponentsInternal(bCollisionUpdate);

	//if ( instancedShadowPlane )
		//instancedShadowPlane->UpdateComponent(GWorld->Scene,this,FMatrix::Identity);
}

void AInstancedShadowPlaneActor::ApplySeting()
{
	//instancedShadowPlane->InstanceStartCullDistance			= Settings->StartCullDistance;
	//instancedShadowPlane->InstanceEndCullDistance				= Settings->EndCullDistance;
	//FLOAT CullDistance = Settings->EndCullDistance > 0 ? (FLOAT)Settings->EndCullDistance : 0.f;
	//instancedShadowPlane->DetailMode							= Settings->DetailMode;
	//instancedShadowPlane->LDMaxDrawDistance					= CullDistance;
	//instancedShadowPlane->CachedMaxDrawDistance				= CullDistance;
	instancedShadowPlane->CastShadow							= false;
	instancedShadowPlane->bCastDynamicShadow					= false;
	instancedShadowPlane->bCastStaticShadow					= false;
	instancedShadowPlane->bSelfShadowOnly						= false;
	instancedShadowPlane->bNoModSelfShadow					= false;
	instancedShadowPlane->bAcceptsDynamicDominantLightShadows	= false;
	instancedShadowPlane->bCastHiddenShadow					= false;
	instancedShadowPlane->bCastShadowAsTwoSided				= false;

	instancedShadowPlane->bAcceptsDynamicDominantLightShadows	= false;
	instancedShadowPlane->bCastHiddenShadow					= false;
	instancedShadowPlane->bCastShadowAsTwoSided				= false;

	instancedShadowPlane->AlwaysCheckCollision				= false;
	instancedShadowPlane->CollideActors						= false;
	instancedShadowPlane->BlockActors							= false;
	instancedShadowPlane->BlockNonZeroExtent					= false;
	instancedShadowPlane->BlockZeroExtent						= false;

// 	if( instancedShadowPlane->bAcceptsLights != Settings->bAcceptsLights
// 		|| instancedShadowPlane->bUsePrecomputedShadows != Settings->bUsePrecomputedShadows )
// 	{
// 		InClusterComponent->InvalidateLightingCache();
// 	}

	instancedShadowPlane->bAcceptsLights						= false;
	instancedShadowPlane->bUsePrecomputedShadows				= false;
	instancedShadowPlane->bAcceptsDynamicLights				= false;
	instancedShadowPlane->bUseOnePassLightingOnTranslucency	= false;
}