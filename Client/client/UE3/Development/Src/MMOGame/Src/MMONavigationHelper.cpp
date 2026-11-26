
#include "stdafx.h"
#include "MMOGameClasses.h"

#if !SHIPPING_PC_GAME
extern bool g_drawPathDebug;
#endif

IMPLEMENT_CLASS(UMMONavigationHelper)

void UMMONavigationHelper::Setup(AController* Owner)
{
	_Owner = Owner;
	_NavigationHandle = _Owner ? _Owner->NavigationHandle : NULL;
	if (_NavigationHandle)
	{
		_NavigationHandle->bUltraVerbosePathDebugging = ShowDebugPath;
		_NavigationHandle->bVisualPathDebugging = ShowDebugPath;
	}
}

UBOOL UMMONavigationHelper::FindPathToDest(FVector vecDestLocation)
{
	UBOOL succ;
	if (_NavigationHandle != NULL)
	{
		//`log("____ Move FindPathToDest Location" @vecDestLocation);
// 		AMMOPlayerController *pMMOOwner = Cast<AMMOPlayerController>(_Owner);
		_NavigationHandle->PathConstraintList = NULL;
		_NavigationHandle->PathGoalList = NULL;
		_NavigationHandle->ClearConstraints();

		if (!eventPram_NavMeshPath_Toward(_NavigationHandle, vecDestLocation))
			return FALSE;

		succ = _NavigationHandle->FindPath();
#if !SHIPPING_PC_GAME
		if(g_drawPathDebug)
		{
			_NavigationHandle->DrawPathCache(FVector(EC_EventParm),TRUE);
		}
#endif

		return succ;
	}
	return FALSE;
}

void UMMONavigationHelper::BeginPathing(FVector vecDestLocation)
{
	_Destination = vecDestLocation;
	if (_NavigationHandle)
		_NavigationHandle->SetFinalDestination(vecDestLocation);
}

UBOOL UMMONavigationHelper::GetNextMoveLocation(FVector& aim, FLOAT ArrivalDistance)
{
	aim = FVector(0, 0, 0);
	FVector nextMove(0, 0, 0);
	if (_NavigationHandle && _NavigationHandle->GetNextMoveLocation(nextMove, ArrivalDistance))
	{
		aim = nextMove;
		return TRUE;
	}
	return FALSE;
}