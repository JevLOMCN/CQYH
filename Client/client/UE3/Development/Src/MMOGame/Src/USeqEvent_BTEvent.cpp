#include "stdafx.h"

IMPLEMENT_CLASS(USeqEvent_BTEvent);

int USeqEvent_BTEvent::GetKismetObjectServerType() const
{
	return (INT)kotSeqEvent_BTEvent;
}

void USeqEvent_BTEvent::ExportServerData(FArchive& inStream)
{
	Super::ExportServerData(inStream);

// 	TArray<UObject**> OutObjects;
// 	GetObjectVars(OutObjects);
// 
// 	INT Num = OutObjects.Num();
// 	inStream << Num;
// 
// 	for (INT i = 0; i < OutObjects.Num(); ++i)
// 	{
// 		UObject** ObjectRef = OutObjects(i);
// 		AMMOVolume_Trigger* Obj = Cast<AMMOVolume_Trigger>(*ObjectRef);
// 		if(Obj)
// 		{
// 			inStream << Obj->AreaID;
// 		}
// 		else if(*ObjectRef)
// 		{
// 			GWarn->MapCheck_Add(MCTYPE_ERROR, this,
// 				TEXT("Kismet referenced error object for SeqEvent_Server_PlayerTouchVolume"),
// 				NULL, MCGROUP_KISMET);
// 		}
// 	}
}

//song@snda.com
#if WITH_EDITOR
FString USeqEvent_BTEvent::GetAutoComment() const
{
	FString AutoComment;

	for( TFieldIterator<UProperty> It( GetClass() ); It; ++It )
	{
		UProperty* Property = *It;

		// AutoCloseCount is a special case that should only display when > 0
		if(Property && (Property->PropertyFlags & CPF_Edit))
		{
			FString MetaString = Property->GetMetaData(TEXT("autocomment"));
			if(MetaString.Len() > 0)
			{
				UNameProperty* NameProp = Cast<UNameProperty>(Property);
				if(NameProp)
				{
					FName* NamePtr = (FName*)(((BYTE*)this) + NameProp->Offset);
					AutoComment += FString::Printf(TEXT("%s"), *(NamePtr->ToString()));
				}

				UStrProperty* StrProp = Cast<UStrProperty>(Property);
				if(StrProp)
				{
					FString* StringPtr = (FString*)(((BYTE*)this) + StrProp->Offset);
					AutoComment += FString::Printf(TEXT("%s=%s "), *(StrProp->GetName()), *(*StringPtr));
				}

				UIntProperty* IntProp = Cast<UIntProperty>(Property);
				if(IntProp)
				{
					INT* IntPtr = (INT*)(((BYTE*)this) + IntProp->Offset);
					AutoComment += FString::Printf(TEXT("%s=%d "), *(IntProp->GetName()), *IntPtr);
				}

				UFloatProperty* FloatProp = Cast<UFloatProperty>(Property);
				if(FloatProp)
				{
					FLOAT* FloatPtr = (FLOAT*)(((BYTE*)this) + FloatProp->Offset);
					AutoComment += FString::Printf(TEXT("%s=%f "), *(FloatProp->GetName()), *FloatPtr);
				}

				UBoolProperty* BoolProp = Cast<UBoolProperty>(Property);
				if(BoolProp)
				{
					UBOOL bValue = *(BITFIELD*)((BYTE*)this + BoolProp->Offset) & BoolProp->BitMask;
					AutoComment += FString::Printf(TEXT("%s=%s "), *(BoolProp->GetName()), bValue ? TEXT("TRUE") : TEXT("FALSE"));
				}

				UByteProperty* ByteProp = Cast<UByteProperty>(Property);
				if (ByteProp)
				{
					BYTE* BytePtr = (BYTE*)(((BYTE*)this) + ByteProp->Offset);
					INT ByteVal = (INT)(*BytePtr);

					if (ByteProp->Enum && ByteVal < ByteProp->Enum->NumEnums())
					{
						FString EnumName;
						ByteProp->Enum->GetEnum(ByteVal).ToString( EnumName );
						AutoComment += FString::Printf(TEXT("%s "), *EnumName);
					}
					else
					{
						AutoComment += FString::Printf(TEXT("%s=%d "), *(ByteProp->GetName()), *BytePtr);
					}
				}
			}
		}
	}

	return AutoComment;
}

#if WITH_EDITOR
void USeqEvent_BTEvent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	TCHAR GBTIni[1024] = {0};
	FString ParamDesc[1];

	Super::PostEditChangeProperty(PropertyChangedEvent);

	appStrncpy(GBTIni, *FString::Printf(TEXT("%s%s%sBTEvent.ini"), *appGameConfigDir(), INI_PREFIX, GGameName), ARRAY_COUNT(GBTIni));
	GConfig->GetString( *BTEventType, TEXT("Refer"), ParamDesc[0], GBTIni );// GBTIni

	ObjName = BTEventType;

	if ( !ParamDesc[0].IsEmpty() )
	{
		VariableLinks(0).OverrideDelta = 0;
		VariableLinks(0).LinkDesc = ParamDesc[0];
	}
}
#endif

BOOL USeqEvent_BTEvent::ExportBTScript( FString& Buffer )
{
	USeqEvent_BTEvent* BTEvent = (USeqEvent_BTEvent*)this; 
	BOOL nRetCode = FALSE;

	// begin
	FString TreeID = "";
	TArrayNoInit<struct FSeqVarLink>& VarLinks = BTEvent->VariableLinks;
	for (int vl=0; vl<VariableLinks.Num(); ++vl )
	{
		FSeqVarLink& VarLink = VarLinks(vl);

		TArrayNoInit<class USequenceVariable*> Vars = VarLink.LinkedVariables;
	
		//check( Vars.Num() <= 1 );
		if( Vars.Num() > 1 )
		{
			GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent var links > 1")), NULL);
			return FALSE;
		}

		for (int v=0; v<Vars.Num(); ++v )
		{
			USequenceVariable* SeqVar = Vars(v);
			if ( !SeqVar || !SeqVar->IsA(USeqVar_Object::StaticClass()) )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent Refer to Not USeqVar_Object")), NULL);
				//appErrorf(TEXT("BTEvent Refer to Not USeqVar_Object"));
				continue;
			}

			USeqVar_Object* VarObj = Cast<USeqVar_Object>( SeqVar );
			if ( !VarObj )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent Can't Cast to USeqVar_Object")), NULL);
				//appErrorf(TEXT("BTEvent Can't Cast to USeqVar_Object"));
				continue;
			}

			UObject** Obj = VarObj->GetObjectRef( 0 );
			if ( !Obj || !(*Obj) )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent Can't GetObjectRef")), NULL);
				//appErrorf(TEXT("BTEvent Can't GetObjectRef"));
				continue;
			}
			
			TScriptInterface<IIUniqueID> InterfaceActor = ScriptInterfaceCast<IIUniqueID>( *Obj );
			if(InterfaceActor)
			{
				InterfaceActor->InitUniqueID();
				if( InterfaceActor->GetUniqueID() <= 0 )
					goto Exit0;
			}

			if ( (*Obj)->IsA( AMMOVolume_Trigger::StaticClass() ) )
			{
				AMMOVolume_Trigger* Volume = Cast<AMMOVolume_Trigger>( *Obj );
				if ( !Volume )
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent Refer to Not MMOVolume_Trigger")), NULL);
					//appErrorf(TEXT("BTEvent Refer to Not AMMOVolume_Trigger"));
					continue;
				}
				
				TreeID = FString::Printf( TEXT("%d"),Volume->ServerUniqueID );
			}
			else if ( (*Obj)->IsA( ANPCServerInstance::StaticClass() ) )
			{
				ANPCServerInstance* Group = Cast<ANPCServerInstance>( *Obj );
				if ( !Group )
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent Refer to Not NPCServerInstance")), NULL);
					//appErrorf(TEXT("BTEvent Refer to Not ANPCServerInstance"));
					continue;
				}
				
				TreeID = FString::Printf( TEXT("%d"),Group->ServerUniqueID );
			}
			else if ( (*Obj)->IsA( ANPCNewSpawnGroupActor::StaticClass() ) )
			{
				ANPCNewSpawnGroupActor* Group = Cast<ANPCNewSpawnGroupActor>( *Obj );
				if ( !Group )
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent Refer to Not NPCNewSpawnGroupActor")), NULL);
					//appErrorf(TEXT("BTEvent Refer to Not ANPCNewSpawnGroupActor"));
					continue;
				}
				
				TreeID = FString::Printf( TEXT("%d"), Group->ServerUniqueID );
			}
			else if ( (*Obj)->IsA( ANPCNewSpawnGroupMediumActor::StaticClass() ) )
			{
				ANPCNewSpawnGroupMediumActor* Group = Cast<ANPCNewSpawnGroupMediumActor>( *Obj );
				if ( !Group )
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent Refer to Not NPCNewSpawnGroupMediumActor")), NULL);
					//appErrorf(TEXT("BTEvent Refer to Not ANPCNewSpawnGroupMediumActor"));
					continue;
				}
				
				TreeID = FString::Printf( TEXT("%d"),Group->ServerUniqueID );
			}
			else // unkown varlink type
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("Unknown BTEvent Var type")), NULL);
				//appErrorf( TEXT("Unknown BTEvent Var type"));
			}

		}
	}

	// tree name
	Buffer += TEXT("\n"); 
	Buffer += FString::Printf( TEXT("%s"),*(BTEvent->BTEventType) );
	if ( !TreeID.IsEmpty() )
	{
		Buffer += FString::Printf( TEXT("_%s"), *TreeID );//.ToUpper();
	}
	Buffer += " = \n";

	// nodes
	TArrayNoInit<struct FSeqOpOutputLink>& OutputLinks = BTEvent->OutputLinks;
	for (int ol=0; ol<OutputLinks.Num(); ++ol )
	{
		FSeqOpOutputLink& OutLink = OutputLinks(ol);
		TArrayNoInit<struct FSeqOpOutputInputLink>& InLinks = OutLink.Links;
		
		//check( 1 == InLinks.Num() );
		if(InLinks.Num() != 1)
		{
			GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("Output can only have 1 link!")), NULL);
			return FALSE;
		}

		for (int il=0; il<InLinks.Num(); ++il )
		{
			FSeqOpOutputInputLink& InLink = InLinks(il);

			USequenceOp* SeqOp = InLink.LinkedOp;
			if ( !SeqOp || !SeqOp->IsA(USeqAct_BTNode::StaticClass()) )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent should link to a SeqAct_BTNode")), NULL);
				//appErrorf( TEXT("BTEvent should link to a USeqAct_BTNode"));
				continue;
			}

			USeqAct_BTNode* BTNode = Cast<USeqAct_BTNode>(SeqOp);
			nRetCode = BTNode->ExportBTNode( Buffer, 0 );
			if ( !nRetCode )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("BTEvent can't cast to SeqAct_BTNode")), NULL);
				//appErrorf( TEXT("BTEvent can't cast to USeqAct_BTNode"));
				break;
			}
			//check( Buffer[Buffer.Len()-2] == TEXT(',') );
			if(Buffer[Buffer.Len()-2] != TEXT(','))
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, BTEvent, *FString::Printf(TEXT("Buffer[Buffer.Len()-2] != TEXT(',')")), NULL);
				return FALSE;
			}
			Buffer[Buffer.Len()-2] = TEXT(';');  // latest ',' -> ';'
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

void USeqEvent_BTEvent::GetDynamicListValues(const DynamicListInfo& ListInfo, TArray<FString>& Values)
{
	check(ListInfo.ListName == TEXT("BTEventTypeList"));

	TCHAR GBTIni[1024] = {0};
	appStrncpy(GBTIni, *FString::Printf(TEXT("%s%s%sBTEvent.ini"), *appGameConfigDir(), INI_PREFIX, GGameName), ARRAY_COUNT(GBTIni));

	TArray<FString> StrArray;
	INT nRetCode = GConfig->GetArray( *(ListInfo.ListName), *(ListInfo.ListName), StrArray, GBTIni );
	if ( nRetCode > 0 )
	{
		for(INT i=0;i<StrArray.Num();i++)
		{
			FString BTEditConfig;
			 GConfig->GetString( *StrArray(i), TEXT("BTEdit"), BTEditConfig, GBTIni );
			if ((BTEditConfig == TEXT("BET_Scene") || BTEditConfig == TEXT("BET_BOTH"))	&&
				EditType == EBET_Scene)
			{
				Values.AddItem( StrArray(i) );
			}
			else if ((BTEditConfig == TEXT("BET_AI") || BTEditConfig == TEXT("BET_BOTH")) &&
				EditType == EBET_AI )
			{
				Values.AddItem( StrArray(i) );
			}
		}
	}
}

UBOOL USeqEvent_BTEvent::CanConnectVarToLink(USequenceVariable *Var, INT LinkIdx)
{
	if ( LinkIdx == 0 && Var )
	{
		UObject** Obj = NULL;
		if ( Var->IsA(USeqVar_Object::StaticClass()) )
		{
			USeqVar_Object* VarObj = Cast<USeqVar_Object>( Var );
			if ( !VarObj )
			{
				appMsgf(AMT_OK, TEXT("BTEvent Can't Cast to USeqVar_Object"));
				return FALSE;
			}

			Obj = VarObj->GetObjectRef( 0 );
			if ( !Obj || !(*Obj) )
			{
				appMsgf(AMT_OK, TEXT("BTEvent Can't GetObjectRef"));
				return FALSE;
			}
		}

		if ( BTEventType == TEXT("bstTouchVolume") )
		{
			if ( !Obj || !(*Obj) || !(*Obj)->IsA( AMMOVolume_Trigger::StaticClass() ) )
			{
				//appErrorf(TEXT("BTEvent Refer to Not AMMOVolume_Trigger"));
				return FALSE;
			}
		}
		else if ( BTEventType == TEXT("bstSingleNpcDie") )
		{
			if ( !Obj || !(*Obj) || !(*Obj)->IsA( ANPCServerInstance::StaticClass() ) )
			{
				//appErrorf(TEXT("BTEvent Refer to Not ANPCServerInstance"));
				return FALSE;
			}
		}
		else if ( BTEventType == TEXT("bstMediumGroupNpcDie") )
		{
			if ( !Obj || !(*Obj) || !(*Obj)->IsA( ANPCNewSpawnGroupMediumActor::StaticClass() ) )
			{
				//appErrorf(TEXT("BTEvent Refer to Not ANPCNewSpawnGroupMediumActor"));
				return FALSE;
			}
		}
		else if ( BTEventType == TEXT("bstLargeGroupNpcDie") )
		{
			if ( !Obj || !(*Obj) || !(*Obj)->IsA( ANPCNewSpawnGroupActor::StaticClass() ) )
			{
				//appErrorf(TEXT("BTEvent Refer to Not ANPCNewSpawnGroupActor"));
				return FALSE;
			}
		}
		else 
		{

		}
	}

	return TRUE;
}

#endif
