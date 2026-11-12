#include "stdafx.h"
#include "MMOFacade.h"

IMPLEMENT_CLASS(USeqAct_Modifier);


//song@snda.com
void USeqAct_Modifier::Activated()
{
	USequenceOp::Activated(); // Required for triggering Kismet visual debugger breakpoints

	//if (InputLinks(0).bHasImpulse &&
	//	Factory != NULL)
	//{
	//	bIsSpawning = TRUE;
	//	// reset all our transient properties
	//	RemainingDelay = 0.f;
	//	SpawnedCount = 0;
	//}
	//else
	//{
	//	CheckToggle();
	//}
}

/**
 * Checks if the delay has been met, and creates a new
 * actor, choosing a spawn Point and passing the values
 * to the selected factory.  Once an actor is created then
 * all Object variables Linked as "Spawned" are set to the
 * new actor, and the output Links are activated.  The op
 * terminates once SpawnCount has been exceeded.
 * 
 * @param		DeltaTime		time since last tick
 * @return						true to indicate that all
 * 								actors have been created
 */
UBOOL USeqAct_Modifier::UpdateOp(FLOAT DeltaTime)
{
	//CheckToggle();
	//if (bEnabled && bIsSpawning)
	{
		//if ((Factory != NULL) && ((SpawnPoints.Num() > 0) || (SpawnLocations.Num() > 0)))
		//{
		//	// if the delay has been exceeded
		//	if (RemainingDelay <= 0.f)
		//	{
		//		if (SpawnPoints.Num() > 0)
		//		{
		//			// process point selection if necessary
		//			if (SpawnPoints.Num() > 1)
		//			{
		//				switch (PointSelection)
		//				{
		//				case PS_Random:
		//					// randomize the list of points
		//					for (INT Idx = 0; Idx < SpawnPoints.Num(); Idx++)
		//					{
		//						INT NewIdx = Idx + (appRand()%(SpawnPoints.Num()-Idx));
		//						SpawnPoints.SwapItems(NewIdx,Idx);
		//					}
		//					LastSpawnIdx = -1;
		//					break;
		//				case PS_Normal:
		//					break;
		//				case PS_Reverse:
		//					// reverse the list
		//					for (INT Idx = 0; Idx < SpawnPoints.Num()/2; Idx++)
		//					{
		//						SpawnPoints.SwapItems(Idx,SpawnPoints.Num()-1-Idx);
		//					}
		//					break;
		//				default:
		//					break;
		//				}
		//			}
		//			AActor *NewSpawn = NULL;
		//			INT SpawnIdx = LastSpawnIdx;
		//			for (INT Count = 0; Count < SpawnPoints.Num() && NewSpawn == NULL; Count++)
		//			{
		//				if (++SpawnIdx >= SpawnPoints.Num())
		//				{
		//					SpawnIdx = 0;
		//				}
		//				AActor *Point = SpawnPoints(SpawnIdx);
		//				if( Point )
		//				{
		//					// attempt to create the new actor
		//					CurrentSpawnIdx = SpawnIdx;
		//					NewSpawn = Factory->CreateActor(&(Point->Location),&(Point->Rotation), this);
		//					// if we created the actor
		//					if (NewSpawn != NULL)
		//					{
		//						NewSpawn->bKillDuringLevelTransition = TRUE;
		//						NewSpawn->eventSpawnedByKismet();
		//						KISMET_LOG(TEXT("Spawned %s at %s"),*NewSpawn->GetName(),*Point->GetName());
		//						// increment the spawned count
		//						SpawnedCount++;
		//						Spawned(NewSpawn);
		//						LastSpawnIdx = SpawnIdx;
		//					}
		//					else
		//					{
		//						KISMET_WARN(TEXT("Failed to spawn at %s using factory %s"),*Point->GetName(),*Factory->GetName());
		//					}
		//				}
		//			}
		//		}
		//		else
		//		{
		//			// We assume if there are Orientations, they will match the number of locations (for now, at least)
		//			UBOOL bOrientationMatchesLocation = (SpawnOrientations.Num() == SpawnLocations.Num());
		//			// process point selection if necessary
		//			if (SpawnLocations.Num() > 1)
		//			{
		//				switch (PointSelection)
		//				{
		//				case PS_Random:
		//					// randomize the list of points
		//					for (INT Idx = 0; Idx < SpawnLocations.Num(); Idx++)
		//					{
		//						INT NewIdx = Idx + (appRand()%(SpawnLocations.Num()-Idx));
		//						SpawnLocations.SwapItems(NewIdx,Idx);
		//						if (bOrientationMatchesLocation)
		//						{
		//							SpawnOrientations.SwapItems(NewIdx,Idx);
		//						}
		//					}
		//					LastSpawnIdx = -1;
		//					break;
		//				case PS_Normal:
		//					break;
		//				case PS_Reverse:
		//					// reverse the list
		//					for (INT Idx = 0; Idx < SpawnLocations.Num()/2; Idx++)
		//					{
		//						SpawnLocations.SwapItems(Idx,SpawnLocations.Num()-1-Idx);
		//						if (bOrientationMatchesLocation)
		//						{
		//							SpawnOrientations.SwapItems(Idx,SpawnLocations.Num()-1-Idx);
		//						}
		//					}
		//					break;
		//				default:
		//					break;
		//				}
		//			}
		//			AActor *NewSpawn = NULL;
		//			INT SpawnIdx = LastSpawnIdx;
		//			for (INT Count = 0; Count < SpawnLocations.Num() && NewSpawn == NULL; Count++)
		//			{
		//				if (++SpawnIdx >= SpawnLocations.Num())
		//				{
		//					SpawnIdx = 0;
		//				}
		//				FVector* Point = &(SpawnLocations(SpawnIdx));
		//				FRotator Rotator(0,0,0);
		//				
		//				if (SpawnOrientations.Num() > 0)
		//				{
		//					if (bOrientationMatchesLocation)
		//					{
		//						Rotator = SpawnOrientations(SpawnIdx).Rotation();
		//					}
		//					else
		//					{
		//						// Just use the first (possibly only) entry...
		//						Rotator = SpawnOrientations(0).Rotation();
		//					}
		//				}
		//				if (Point)
		//				{
		//					// attempt to create the new actor
		//					CurrentSpawnIdx = SpawnIdx;
		//					NewSpawn = Factory->CreateActor(Point,&Rotator, this);
		//					// if we created the actor
		//					if (NewSpawn != NULL)
		//					{
		//						NewSpawn->bKillDuringLevelTransition = TRUE;
		//						NewSpawn->eventSpawnedByKismet();
		//						KISMET_LOG(TEXT("Spawned %s at %s"),*NewSpawn->GetName(),*(Point->ToString()));
		//						// increment the spawned count
		//						SpawnedCount++;
		//						Spawned(NewSpawn);
		//						LastSpawnIdx = SpawnIdx;
		//					}
		//					else
		//					{
		//						KISMET_WARN(TEXT("Failed to spawn at %s using factory %s"),*(Point->ToString()),*Factory->GetName());
		//					}
		//				}
		//			}
		//		}
		//		// set a new delay
		//		RemainingDelay = SpawnDelay;
		//	}
		//	else
		//	{
		//		// reduce the delay
		//		RemainingDelay -= DeltaTime;
		//	}
		//	// if we've spawned the limit return true to finish the op
		//	return (SpawnedCount >= SpawnCount);
		//}
		//else
		//{
		//	// abort the spawn process
		//	if (Factory == NULL)
		//	{
		//		KISMET_WARN(TEXT("Actor factory action %s has an invalid factory!"),*GetFullName());
		//	}
		//	if (SpawnPoints.Num() == 0)
		//	{
		//		KISMET_WARN(TEXT("Actor factory action %s has no spawn points!"),*GetFullName());
		//	}
		//	return TRUE;
		//}
	}
	//else
	{
		// finish the action, no need to spawn anything
		return TRUE;
	}
	return TRUE;
}

void USeqAct_Modifier::Spawned(UObject *NewSpawn)
{
	// assign to any Linked variables
	TArray<UObject**> ObjVars;
	GetObjectVars(ObjVars,TEXT("Spawned"));
	for (INT Idx = 0; Idx < ObjVars.Num(); Idx++)
	{
		*(ObjVars(Idx)) = NewSpawn;
	}
	// activate the spawned output
	OutputLinks(0).bHasImpulse = TRUE;
}

void USeqAct_Modifier::DeActivated()
{
	// do nothing, since outputs have already been activated
	//bIsSpawning = FALSE;
}

void USeqAct_Modifier::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Reject factories that can't be used in-game because they refer to unspawnable types.
	//if ( Factory && Factory->NewActorClass )
	//{
	//	if ( Factory->NewActorClass == Factory->GetClass()->GetDefaultObject<UActorFactory>()->NewActorClass &&
	//		Factory->NewActorClass->GetDefaultActor()->bNoDelete &&
	//		(Factory->GameplayActorClass == NULL || Factory->GameplayActorClass->GetDefaultActor()->bNoDelete) )
	//	{
	//		appMsgf( AMT_OK, *FString::Printf(LocalizeSecure(LocalizeUnrealEd("ActorFactoryNotForUseByKismetF"),*Factory->GetClass()->GetName())) );
	//		Factory = NULL;
	//	}
	//}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}


FString USeqAct_Modifier::GetDetailedDescription( INT InIndex )
{
	FString locDescription;

	switch( InIndex )
	{
	case 0:
		locDescription = FString::Printf(TEXT("%d"), Id);
		break;
	case 1:
		locDescription = NodeName;
		break;
	case 2:
		{
			TArray<FString> Result;
			FString::Printf(TEXT("Properties.EMODIFIER_TARGET_TYPE.MT_Type%d"), Type + 1).ParseIntoArray(&Result, TEXT("."), FALSE);
			if(Result.Num() >= 3)
			{
				locDescription = Localize(*Result(1), *Result(2), *Result(0));
			}
			else
			{
			}
		}
		break;
	case 3:
		{
			for(INT i = 0; i < Modifiers.Num(); i++)
			{
				USeqAct_Modifier* m = Modifiers(i);
				if(_NeedMark(m))
					return FString::Printf(TEXT("Marked"));
			}
		}
		break;
	case 4:
		{
			for(INT i = 0; i < Modifiers.Num(); i++)
			{
				FString strKey;
				USeqAct_Modifier* m = Modifiers(i);
				strKey = FString::Printf(TEXT("%d,"), m->Key);
				locDescription += strKey;
			}

		}
		break;
	default:
		break;
	}

	return locDescription;
}
//song@snda.com

UBOOL USeqAct_Modifier::_NeedMark(USeqAct_Modifier* pNode)
{
	UBOOL bNeedMark = FALSE;
	if(pNode == NULL)
		return FALSE;

	if(pNode->Param1.ValueStr == "catMoveSpeed")
	{
		bNeedMark = TRUE;
	}

	if(pNode->Param2.ValueStr == "catMoveSpeed")
	{
		bNeedMark = TRUE;
	}
	if(pNode->Param3.ValueStr == "catMoveSpeed")
	{
		bNeedMark = TRUE;
	}


	return bNeedMark;
}

void USeqAct_Modifier::PostLoad()
{
	USequenceOp::PostLoad();
}

// void ModifierCheck_Inside(const TCHAR* errMsg, USeqAct_Modifier* container, USeqAct_Modifier* checkNode)
// {
// 	bool insideThis = false;
// 	for (INT j = 0; j < container->Modifiers.Num(); ++j)
// 	{
// 		USeqAct_Modifier* testModifier = container->Modifiers(j);
// 		if(testModifier == checkNode)
// 		{
// 			insideThis = true;
// 			break;
// 		}
// 	}
// 
// 	if(!insideThis)
// 	{
// 		GWarn->MapCheck_Add(MCTYPE_ERROR, container,
// 			*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(errMsg), *checkNode->GetName())), NULL);
// 	}
// }
#if WITH_EDITOR
void USeqAct_Modifier::CheckForErrors()
{
	Super::CheckForErrors();
	if(GetOutermost() == GetOuter()) // is container
	{
		INT rootNodeCnt = 0;
		TArray<USeqAct_Modifier*> unConnectedNode;
		for (INT i = 0; i < Modifiers.Num(); ++i)
		{
			USeqAct_Modifier* modifier = Modifiers(i);
			if(modifier->bRoot)
			{
				++rootNodeCnt;
			}
			
			if(modifier->bRoot)
			{
				if(modifier->ParentNode != NULL)
				{
					GWarn->MapCheck_Add(MCTYPE_ERROR, this,
						*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierRootParentNotNull")), *modifier->GetFullName())), NULL);
				}

				if(modifier->Parent == NULL)
				{
					GWarn->MapCheck_Add(MCTYPE_ERROR, this,
						*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierRootParentIsNull")), *modifier->GetFullName())), NULL);
				}
			}

			// Not RootNode but Input/Output are both empty
			if(!modifier->bRoot && !modifier->ParentNode
				&& !(modifier->FalseRetNode || modifier->FalseRetNode || modifier->NextNode))
			{
				unConnectedNode.AddItem(modifier);
			}

			if(modifier->ParentNode)
			{
				//ModifierCheck_Inside(TEXT("CheckError_ModifierParentNodeNotInThis"), this, modifier->ParentNode);

				bool insideThis = false;
				for (INT j = 0; j < Modifiers.Num(); ++j)
				{
					USeqAct_Modifier* testModifier = Modifiers(j);
					if(testModifier == modifier->ParentNode)
					{
						insideThis = true;
						break;
					}
				}

				if(!insideThis)
				{
					GWarn->MapCheck_Add(MCTYPE_ERROR, modifier,
						*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierParentNodeNotInThis")), *modifier->ParentNode->GetFullName())), NULL);
				}
			}

			if(modifier->TrueRetNode)
			{
				bool insideThis = false;
				for (INT j = 0; j < Modifiers.Num(); ++j)
				{
					USeqAct_Modifier* testModifier = Modifiers(j);
					if(testModifier == modifier->TrueRetNode)
					{
						insideThis = true;
						break;
					}
				}

				if(!insideThis)
				{
					GWarn->MapCheck_Add(MCTYPE_ERROR, modifier,
						*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierTrueRetNodeNotInThis")), *modifier->TrueRetNode->GetFullName())), NULL);
				}
			}

			if(modifier->NextNode)
			{
				bool insideThis = false;
				for (INT j = 0; j < Modifiers.Num(); ++j)
				{
					USeqAct_Modifier* testModifier = Modifiers(j);
					if(testModifier == modifier->NextNode)
					{
						insideThis = true;
						break;
					}
				}

				if(!insideThis)
				{
					GWarn->MapCheck_Add(MCTYPE_ERROR, modifier,
						*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierNextNodeNotInThis")), *modifier->NextNode->GetFullName())), NULL);
				}
			}

			if(modifier->FalseRetNode)
			{
				bool insideThis = false;
				for (INT j = 0; j < Modifiers.Num(); ++j)
				{
					USeqAct_Modifier* testModifier = Modifiers(j);
					if(testModifier == modifier->FalseRetNode)
					{
						insideThis = true;
						break;
					}
				}

				if(!insideThis)
				{
					GWarn->MapCheck_Add(MCTYPE_ERROR, modifier,
						*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierFalseRetNodeNotInThis")), *modifier->FalseRetNode->GetFullName())), NULL);
				}
			}
		}

		if(unConnectedNode.Num() > 1)
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, this,
				*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierChildUnConnectedError")),
				*GetFullName(), unConnectedNode.Num())), NULL);

		}

		if(rootNodeCnt == 0)
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, this,
				*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierRootNULL")), *GetFullName())), NULL);
		}
		else if(rootNodeCnt > 1)
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, this,
				*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierHasMultiRootNode")), *GetFullName())), NULL);
		}
	}
	else // is child node
	{
		if(!bRoot && !ParentNode)
		{
			GWarn->MapCheck_Add(MCTYPE_WARNING, this,
				*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ModifierNodeInputNULL")), *GetFullName())), NULL);
		}
	}
}
#endif