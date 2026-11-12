#include "stdafx.h"
//#include "MMOGameClasses.h"
#include "MMOUtil.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(AMMOServerTrigger)

void AMMOServerTrigger::RegisterToFacade()
{
	//if( MMO::Facade::getSingletonPtr() )
		MMOFacade::getSingleton().RegisterTrigger(TScriptInterface<IIServerTrigger>(this));
}
//------------------------------------------------------------------------
BYTE AMMOServerTrigger::GetTriggerType()
{
	return TriggerType;
}
//------------------------------------------------------------------------
INT AMMOServerTrigger::GetTriggerID()
{
	return TriggerID;
}
//------------------------------------------------------------------------
void AMMOServerTrigger::SetAutoID(INT Id)
{
	TriggerID = Id;
}
//------------------------------------------------------------------------
FBox AMMOServerTrigger::GetObjectServerBounds()
{
	FBox box = GetComponentsBoundingBox();	
	ConvertBoxToServer(box);
	return box;
}
//------------------------------------------------------------------------
FVector AMMOServerTrigger::GetObjectServerPosition()
{
	FVector result = Location;
	ConvertCoordC2S(result);
	result.X = (INT32)result.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	result.Y = (INT32)result.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	return result;
}
//------------------------------------------------------------------------
TArray<class ADynamicBlockingVolume*> AMMOServerTrigger::GetBlockingVolumes()
{
	return AssociatedBlockingVolume;
}
//------------------------------------------------------------------------
BYTE AMMOServerTrigger::GetServerInitState()
{
	return InitTriggerState;
}
//------------------------------------------------------------------------
FString AMMOServerTrigger::GetClientName()
{
	return GetName();
}
//------------------------------------------------------------------------
TArray<class ANPCSpawnPoint*> AMMOServerTrigger::GetNPCSpawnPoints()
{
	return NPCSpawnPoints;
}
//------------------------------------------------------------------------
void AMMOServerTrigger::MarkPackageDirty(
	UBOOL InDirty) const
{
	AActor::MarkPackageDirty(InDirty);
	/*if(GIsEditor)
	{
		EditorCollisionCylinder->CylinderRadius = CollisionCylinder->CollisionRadius;
		EditorCollisionCylinder->CylinderTopRadius = CollisionCylinder->CollisionRadius;
		EditorCollisionCylinder->CylinderHeight = CollisionCylinder->CollisionHeight;
	}*/
}
//------------------------------------------------------------------------
TArray<TScriptInterface<class IIServerTrigger> > AMMOServerTrigger::GetLinkedTrigger()
{
	return LinkedTrigger;
}
//------------------------------------------------------------------------
INT AMMOServerTrigger::GetDelayTime()
{
    return DelayTime;
}
//------------------------------------------------------------------------
TArray<struct FAbilityEvent> AMMOServerTrigger::GetAbilityList()
{
	return AbilityList;
}
//------------------------------------------------------------------------
struct FTeleportEvent AMMOServerTrigger::GetTeleportEvent()
{
	return TeleportEvent;
}
//------------------------------------------------------------------------
BYTE AMMOServerTrigger::GetTrapEventType()
{
	return TrapEventType;
}
//------------------------------------------------------------------------
TArray<TScriptInterface<class IIServerArea> > AMMOServerTrigger::GetLinkedEventArea()
{
	return LinkedEventArea;
}
//------------------------------------------------------------------------
INT AMMOServerTrigger::GetLoadingTime()
{
	return LoadingTime;
}
//------------------------------------------------------------------------
FNPCDeadEvent AMMOServerTrigger::GetNPCDeadEvent()
{
	return NPCDeadEvent;
}
//------------------------------------------------------------------------
TArray<class ATeleportOrig*> AMMOServerTrigger::GetLinkedTeleportList()
{
	return LinkedTeleportList;
}
//------------------------------------------------------------------------
// ServerTriggerEditorComponent
//------------------------------------------------------------------------
IMPLEMENT_CLASS(UServerTriggerEditorComponent);

class ServerTriggerSceneProxy : public FPrimitiveSceneProxy
{
public:
	ServerTriggerSceneProxy(const UPrimitiveComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
	{
		ServerTriggerObject = InComponent->GetOwner();
		check(ServerTriggerObject);
	}

	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI,const FSceneView* View,UINT DPGIndex,DWORD Flags = 0)
	{
		if(ServerTriggerObject->IsSelected())
		{
			typedef TScriptInterface<IIServerTrigger> IServerTrigger;
			IServerTrigger ServerTrigger = ScriptInterfaceCast<IIServerTrigger>(ServerTriggerObject);
			const TArray<class ANPCSpawnPoint*>& NPCSpawns = ServerTrigger->GetNPCSpawnPoints();
			for (INT i = 0; i < NPCSpawns.Num(); ++i)
			{
				//A newly added SpawnPoint has empty content, drawing it directly would crash
				if( NPCSpawns(i) != NULL )
					PDI->DrawLine(ServerTriggerObject->Location, NPCSpawns(i)->Location, FColor(255, 255, 0), SDPG_World);
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

	AActor* ServerTriggerObject;
};

FPrimitiveSceneProxy* UServerTriggerEditorComponent::CreateSceneProxy()
{
	return new ServerTriggerSceneProxy(this);
}
//------------------------------------------------------------------------
void UServerTriggerEditorComponent::UpdateBounds()
{
	const FLOAT NewScale = (Owner ? Owner->DrawScale : 1.0f) * 512;
	Bounds = FBoxSphereBounds(GetOrigin(),FVector(NewScale,NewScale,NewScale),appSqrt(3.0f * Square(NewScale)));
}
//------------------------------------------------------------------------
