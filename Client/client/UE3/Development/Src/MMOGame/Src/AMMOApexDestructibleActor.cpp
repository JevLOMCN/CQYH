#include "stdafx.h"
//#include "MMOGameClasses.h"
//#include "MMOUtil.h"
//#include "MMOFacade.h"
#include <NxDestructibleAsset.h>
#include <NxDestructibleActor.h>

IMPLEMENT_CLASS(AMMOApexDestructibleActor)

void AMMOApexDestructibleActor::TickAuthoritative( FLOAT DeltaSeconds )
{
	Super::TickAuthoritative(DeltaSeconds);

	//if(StaticDestructibleComponent->ApexDestructibleActor && DisableCollisionWhenExplode)
	//{
		//const physx::apex::NxDestructibleParameters& para = StaticDestructibleComponent->ApexDestructibleActor->getDestructibleParameters();
		//debugf(TEXT("----- %s %f %f"), *GetName(), para.damageCap, para.damageThreshold);
	//}
}

void AMMOApexDestructibleActor::TakeDamage(INT Damage,
	class AController* EventInstigator,FVector HitLocation,FVector Momentum,
	class UClass* DamageType,
	struct FTraceHitInfo HitInfo/*=FTraceHitInfo(EC_EventParm)*/,
	class AActor* DamageCauser/*=NULL*/)
{
	if (CurHealth > 0)
	{
		// Do nothing, e.g. could shake a bit

	}
	else
	{
		AApexDestructibleActor::TakeDamage(Damage, EventInstigator, HitLocation, Momentum, 
			DamageType, HitInfo, DamageCauser);
	}
}

void AMMOApexDestructibleActor::TakeRadiusDamage(class AController* InstigatedBy,
	FLOAT BaseDamage,FLOAT DamageRadius,class UClass* DamageType,FLOAT Momentum,
	FVector HurtOrigin,UBOOL bFullDamage,class AActor* DamageCauser,
	FLOAT DamageFalloffExponent/*=1.000000*/)
{
	if (CurHealth > 0)
	{
		// Do nothing, e.g. could shake a bit

	}
	else
	{
		AApexDestructibleActor::TakeRadiusDamage(InstigatedBy, BaseDamage,
			DamageRadius,DamageType,Momentum, HurtOrigin,bFullDamage, DamageCauser,
			DamageFalloffExponent);
	}
}


//void AMMODescructibleActor::RegisterToFacade()
//{
//	if( Facade::getSingletonPtr() )
//		MMOFacade::getSingleton().RegisterTrigger(TScriptInterface<IIServerTrigger>(this));
//}
////------------------------------------------------------------------------
//BYTE AMMODescructibleActor::GetTriggerType()
//{
//	return TriggerType;
//}
////------------------------------------------------------------------------
//INT AMMODescructibleActor::GetTriggerID()
//{
//	return TriggerID;
//}
////------------------------------------------------------------------------
//void AMMODescructibleActor::SetAutoID(INT Id)
//{
//	TriggerID = Id;
//}
////------------------------------------------------------------------------
//FBox AMMODescructibleActor::GetObjectServerBounds()
//{
//	FBox box = GetComponentsBoundingBox();	
//	return Util::ConvertToServerAABB(box);;
//}
////------------------------------------------------------------------------
//FVector AMMODescructibleActor::GetObjectServerPosition()
//{
//	return Util::ConvertToServerCood(Location);
//}
////------------------------------------------------------------------------
//TArray<class ADynamicBlockingVolume*> AMMODescructibleActor::GetBlockingVolumes()
//{
//	return AssociatedBlockingVolume;
//}
////------------------------------------------------------------------------
//BYTE AMMODescructibleActor::GetServerInitState()
//{
//	return InitTriggerState;
//}
////------------------------------------------------------------------------
//FString AMMODescructibleActor::GetClientName()
//{
//	return GetName();
//}
////------------------------------------------------------------------------
//TArray<class ANPCSpawnPoint*> AMMODescructibleActor::GetNPCSpawnPoints()
//{
//	return NPCSpawnPoints;
//}
////------------------------------------------------------------------------
//TArray<TScriptInterface<class IIServerTrigger> > AMMODescructibleActor::GetLinkedTrigger()
//{
//	return LinkedTrigger;
//}
////------------------------------------------------------------------------
//INT AMMODescructibleActor::GetDelayTime()
//{
//    return DelayTime;
//}
////------------------------------------------------------------------------
//TArray<struct FAbilityEvent> AMMODescructibleActor::GetAbilityList()
//{
//	return AbilityList;
//}
////------------------------------------------------------------------------
//struct FTeleportEvent AMMODescructibleActor::GetTeleportEvent()
//{
//	return TeleportEvent;
//}
////------------------------------------------------------------------------
//BYTE AMMODescructibleActor::GetTrapEventType()
//{
//	return TrapEventType;
//}
////------------------------------------------------------------------------
//TArray<TScriptInterface<class IIServerArea> > AMMODescructibleActor::GetLinkedEventArea()
//{
//	return LinkedEventArea;
//}
////------------------------------------------------------------------------
//INT AMMODescructibleActor::GetLoadingTime()
//{
//	return LoadingTime;
//}
////------------------------------------------------------------------------
//FNPCDeadEvent AMMODescructibleActor::GetNPCDeadEvent()
//{
//	return NPCDeadEvent;
//}
////------------------------------------------------------------------------
//TArray<class ATeleportOrig*> AMMODescructibleActor::GetLinkedTeleportList()
//{
//	return LinkedTeleportList;
//}
