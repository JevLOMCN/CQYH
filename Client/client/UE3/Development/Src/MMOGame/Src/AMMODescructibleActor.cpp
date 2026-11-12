#include "stdafx.h"
//#include "MMOGameClasses.h"
#include "MMOUtil.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(AMMODescructibleActor)

void AMMODescructibleActor::RegisterToFacade()
{
	//if( Facade::getSingletonPtr() )
	MMOFacade::getSingleton().RegisterTrigger(TScriptInterface<IIServerTrigger>(this));
}
//------------------------------------------------------------------------
BYTE AMMODescructibleActor::GetTriggerType()
{
	return TriggerType;
}
//------------------------------------------------------------------------
INT AMMODescructibleActor::GetTriggerID()
{
	return TriggerID;
}
//------------------------------------------------------------------------
void AMMODescructibleActor::SetAutoID(INT Id)
{
	TriggerID = Id;
}
//------------------------------------------------------------------------
FBox AMMODescructibleActor::GetObjectServerBounds()
{
	FBox box = GetComponentsBoundingBox();	
	ConvertBoxToServer(box);
	return box;
}
//------------------------------------------------------------------------
FVector AMMODescructibleActor::GetObjectServerPosition()
{
	FVector result = Location;
	ConvertCoordC2S(result);
	result.X = (INT32)result.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	result.Y = (INT32)result.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	return result;
}
//------------------------------------------------------------------------
TArray<class ADynamicBlockingVolume*> AMMODescructibleActor::GetBlockingVolumes()
{
	return AssociatedBlockingVolume;
}
//------------------------------------------------------------------------
BYTE AMMODescructibleActor::GetServerInitState()
{
	return InitTriggerState;
}
//------------------------------------------------------------------------
FString AMMODescructibleActor::GetClientName()
{
	return GetName();
}
//------------------------------------------------------------------------
TArray<class ANPCSpawnPoint*> AMMODescructibleActor::GetNPCSpawnPoints()
{
	return NPCSpawnPoints;
}
//------------------------------------------------------------------------
TArray<TScriptInterface<class IIServerTrigger> > AMMODescructibleActor::GetLinkedTrigger()
{
	return LinkedTrigger;
}
//------------------------------------------------------------------------
INT AMMODescructibleActor::GetDelayTime()
{
    return DelayTime;
}
//------------------------------------------------------------------------
TArray<struct FAbilityEvent> AMMODescructibleActor::GetAbilityList()
{
	return AbilityList;
}
//------------------------------------------------------------------------
struct FTeleportEvent AMMODescructibleActor::GetTeleportEvent()
{
	return TeleportEvent;
}
//------------------------------------------------------------------------
BYTE AMMODescructibleActor::GetTrapEventType()
{
	return TrapEventType;
}
//------------------------------------------------------------------------
TArray<TScriptInterface<class IIServerArea> > AMMODescructibleActor::GetLinkedEventArea()
{
	return LinkedEventArea;
}
//------------------------------------------------------------------------
INT AMMODescructibleActor::GetLoadingTime()
{
	return LoadingTime;
}
//------------------------------------------------------------------------
FNPCDeadEvent AMMODescructibleActor::GetNPCDeadEvent()
{
	return NPCDeadEvent;
}
//------------------------------------------------------------------------
TArray<class ATeleportOrig*> AMMODescructibleActor::GetLinkedTeleportList()
{
	return LinkedTeleportList;
}

UBOOL AMMODescructibleActor::DrawMyHUD(class AMMOHud* H)
{
#if 0
	//local Vector screenPos;
	//local float healthPercent;
	//local LinearColor healthColor;
	//local Vector healthColorVec;
	//local Vector objectIconWorldPos;
	//local float cameraDistFromPos;
	//local float iconWidth, iconHeight;
	//local float IconDistanceScale;

	if(!EnableDrawHealthBar)
		return false;

	FVector objectIconWorldPos = Location + ( HealthBarPositionOffset /*>> Rotation*/ );
	FVector screenPos = H->Canvas->Project( objectIconWorldPos );
	FLOAT cameraDistFromPos = objectIconWorldPos.Distance( H->PlayerOwner->PlayerCamera->ViewTarget.POV.Location );
	FLOAT IconDistanceScale = Clamp<FLOAT>( 1.0 - cameraDistFromPos/4000 , 0.25 , 1 ) * ( H->Canvas->ClipY / 768.0f ) * HealthBarScale;

	FLOAT iconWidth = 166*IconDistanceScale;
	FLOAT iconHeight = 18*IconDistanceScale;

	if( !H->IsOnCanvas( screenPos , iconWidth , iconHeight ) )
		return false;

	FLOAT healthPercent = GetHealthPercent();
	FVector healthColorVec = Lerp( UnhealthyColor , HealthyColor , healthPercent );
	
	//Don't understand this, comment out for now...
	if( TimeSince(LastTakeDamageTime) < 2)
		healthColorVec *= 1 + 13.0 * Abs( sin ( WorldInfo->TimeSeconds*7 ) );

	FLinearColor healthColor;
	healthColor.R = healthColorVec.X;
	healthColor.G = healthColorVec.Y;
	healthColor.B = healthColorVec.Z;

	MyHealthHUDMatInstance->SetScalarParameterValue( "Percent" , healthPercent );
	MyHealthHUDMatInstance->SetVectorParameterValue( "BarColor" , healthColor );

	//H->Canvas->SetPos( screenPos.x-iconWidth/2,screenPos.y-iconHeight/2);
	H->Canvas->DrawMaterialTile( MyHealthHUDMatInstance , screenPos.X-iconWidth/2 , screenPos.Y-iconHeight/2 , 1 , iconWidth , iconHeight , 0 , 0 , iconWidth , iconHeight );
#endif
	return true;
}

FLOAT AMMODescructibleActor::GetHealthPercent()
{
	return FLOAT(ActorAttributes->HP) / float(ActorAttributes->MaxHP);
}

void AMMODescructibleActor::DrawHUDPostGFx( AHUD* H )
{
}
