#include "stdafx.h"
#include "MMOCrowd.h"

IMPLEMENT_CLASS(AMMOCrowdSystem);

void AMMOCrowdSystem::ResetAllInstances()
{
	ForceUpdateComponents(FALSE,FALSE);
}

//------------------------------------------------------------------------
void AMMOCrowdSystem::PostLoad()
{
	Super::PostLoad();

	if( GIsGame )
	{
// 		UInstancedStaticMeshComponent* comp = 
// 			ConstructObject<UInstancedStaticMeshComponent>(UInstancedStaticMeshComponent::StaticClass(),
// 			this,NAME_None,RF_Transactional);
// 		comp->StaticMesh = StaticMeshRes;
// 		comp->bForceDirectLightMap = TRUE;
// 		comp->bSelectable = TRUE;
// 		comp->bUsePerInstanceHitProxies = TRUE;
// 		comp->bDontResolveInstancedLightmaps = TRUE;
// 		comp->InstancingRandomSeed = appRand();
// 		comp->PerInstanceSMData.AddZeroed();
// 		//comp->PerInstanceSMData(i).
// 
// 		InstanceComponents.AddItem(comp);
		//this->AttachComponent(comp);

		//ConditionalUpdateComponents();
	}
	
	if(GIsEditor )
	{
	}
}
//------------------------------------------------------------------------
void AMMOCrowdSystem::TickSpecial( FLOAT DeltaSeconds )
{
// 	if(CrowdController)
// 	{
// 		CrowdController->Tick(DeltaSeconds);
// 	}
}
//------------------------------------------------------------------------
void AMMOCrowdSystem::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(PropertyChangedEvent.Property->GetFName() == FName(TEXT("RenderType")))
	{
		UseSkeletalMesh = RenderType == EIRT_SkeletalMesh;
		UseStaticMesh = RenderType == EIRT_StaticMesh;
		UseSprite = RenderType == EIRT_Sprite;
	}
}
//------------------------------------------------------------------------
class UMMOCrowdInstance* AMMOCrowdSystem::GetInstanceByID(INT instId)
{
	if(CrowdInstanceData.IsValidIndex(instId))
	{
		return CrowdInstanceData(instId);
	}

	return NULL;
}
//------------------------------------------------------------------------
// ServerTriggerEditorComponent
//------------------------------------------------------------------------
IMPLEMENT_CLASS(UMMOCrowdDebugComponent);

class CrowdDebugSceneProxy : public FPrimitiveSceneProxy
{
public:
	CrowdDebugSceneProxy(const UPrimitiveComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
	{
		CrowdSystem = Cast<AMMOCrowdSystem>(InComponent->GetOwner());
		check(CrowdSystem);
	}

	void DrawCircle(FPrimitiveDrawInterface* PDI, const FVector& pos, FLOAT Radius, const FLinearColor& clr)
	{
		FVector lastDrawPos = pos + FVector(Radius, 0, 0);
		for (INT i = 30; i <= 360; i += 30)
		{
			FLOAT t = (FLOAT)i / 360.0f * 2 * PI;
			FLOAT x = appCos(t) * Radius;
			FLOAT y = appSin(t) * Radius;
			
			FVector drawPos = pos;
			drawPos.X += x;
			drawPos.Y += y;
			
			PDI->DrawLine(lastDrawPos, drawPos, clr, SDPG_World);
			lastDrawPos = drawPos;
		}
	}


	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI,const FSceneView* View,UINT DPGIndex,DWORD Flags = 0)
	{
		if(GIsEditor)
		{
			FBox bound = FBox::BuildAABB(CrowdSystem->Location, CrowdSystem->InitOffsetExtent);
			DrawWireBox(PDI, bound, FColor(255, 255, 0), SDPG_World);

			bound = FBox::BuildAABB(CrowdSystem->Location, CrowdSystem->WrapBoundingExtent);
			DrawWireBox(PDI, bound, FColor(255, 0, 0), SDPG_World);
		}

		if(CrowdSystem->RenderEditorData)
		{
			for (INT i = 0; i < CrowdSystem->CrowdInstanceData.Num(); ++i)
			{
				UMMOCrowdInstance* inst = CrowdSystem->CrowdInstanceData(i);
				if(inst)
				{
					PDI->DrawPoint(inst->GetLocation(), FLinearColor(1, 0, 0), 2, SDPG_World);
					PDI->DrawLine(inst->GetLocation(), inst->GetLocation() + inst->GetDirection() * 20, FLinearColor(1, 1, 0), SDPG_World);

					if(CrowdSystem->DrawDebugLine)
					{
						DrawCircle(PDI, inst->GetLocation(), inst->GetBRadius(), FLinearColor(0, 1, 0));				
						DrawCircle(PDI, inst->GetLocation(), inst->SteeringBehavior->ViewDistance, FLinearColor(1, 1, 0));
						PDI->DrawLine(inst->GetLocation(), inst->GetLocation() + inst->SteeringBehavior->SteeringForce, FLinearColor(1, 0, 0), SDPG_World);

						if(inst->IsTagged())
						{
							PDI->DrawLine(inst->GetLocation(), inst->GetLocation() + FVector(0, 0, 100), FLinearColor(1, 0, 0), SDPG_World);
						}
					}
				}
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View)
	{
		FPrimitiveViewRelevance Result;
		Result.bDynamicRelevance = TRUE;
		Result.SetDPG( SDPG_World,TRUE );
		return Result;
	}

	virtual DWORD GetMemoryFootprint( void ) const { return( sizeof( *this ) + GetAllocatedSize() ); }
	DWORD GetAllocatedSize( void ) const { return( FPrimitiveSceneProxy::GetAllocatedSize() ); }

protected:

	AMMOCrowdSystem* CrowdSystem;
};

FPrimitiveSceneProxy* UMMOCrowdDebugComponent::CreateSceneProxy()
{
	return new CrowdDebugSceneProxy(this);
}
//------------------------------------------------------------------------
void UMMOCrowdDebugComponent::UpdateBounds()
{
	const FLOAT NewScale = (Owner ? Owner->DrawScale : 1.0f) * 512;
	Bounds = FBoxSphereBounds(GetOrigin(),FVector(NewScale,NewScale,NewScale),appSqrt(3.0f * Square(NewScale)));
}
//------------------------------------------------------------------------
