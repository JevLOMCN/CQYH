#include "stdafx.h"

IMPLEMENT_CLASS(USeqAct_BTNode);

int USeqAct_BTNode::GetKismetObjectServerType() const
{
	return (INT)kotSeqAct_BTNode;
}

void USeqAct_BTNode::ExportServerData(FArchive& inStream)
{
	Super::ExportServerData(inStream);
}

void USeqAct_BTNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const INT ParamsNum = 4;
	FString ParamDesc[ParamsNum];

	Super::PostEditChangeProperty(PropertyChangedEvent);

	ObjName = BTNodeType;
	if ( !BTNodeCutomizeType.IsEmpty() )
	{
		ObjName = BTNodeCutomizeType;
	}
	//if ( ObjName == TEXT("batSelector") )
	//{
	//	ObjColor = FColor(255,0,0);
	//}

	TCHAR GBTIni[1024] = {0};
	appStrncpy(GBTIni, *FString::Printf(TEXT("%s%s%sBTNode.ini"), *appGameConfigDir(), INI_PREFIX, GGameName), ARRAY_COUNT(GBTIni));
	GConfig->GetString( *ObjName, TEXT("p0"), ParamDesc[0], GBTIni );	//GBTIni
	GConfig->GetString( *ObjName, TEXT("p1"), ParamDesc[1], GBTIni );
	GConfig->GetString( *ObjName, TEXT("p2"), ParamDesc[2], GBTIni );
	GConfig->GetString( *ObjName, TEXT("p3"), ParamDesc[3], GBTIni );

	for ( INT i=0; i<ParamsNum; ++i )
	{
		VariableLinks(i).OverrideDelta = 0;

		//if ( ParamDesc[i].IsEmpty() )
		//	VariableLinks(i).LinkDesc = FString::Printf( TEXT("P%d"), i );
		//else
		VariableLinks(i).LinkDesc = ParamDesc[i];
	}
}

//song@snda.com
#if WITH_EDITOR
FString USeqAct_BTNode::GetAutoComment() const
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

static BOOL BuildBTIndentation(INT32 depth, FString& tabs)
{
	tabs.Empty();
	if (depth < 0 || (UINT)depth > (MAXINT / sizeof(TCHAR) - 1) / 4)
		return FALSE;
	tabs.Reserve((UINT)depth * 4);
	for (INT32 i = 0; i < depth; ++i)
		tabs += TEXT("    ");
	return TRUE;
}

BOOL USeqAct_BTNode::ExportBTNode( FString& Buffer, INT32 nDepth )
{
	USeqAct_BTNode* pNode = (USeqAct_BTNode*)this;
	INT32 nRetCode = FALSE;
	FString tabs;
	if (!BuildBTIndentation(nDepth, tabs))
		return FALSE;

	if( NULL == pNode )
		return FALSE;

	// 1 params
	TArrayNoInit<struct FSeqVarLink> VarLinks = pNode->VariableLinks;
	//assert( 4 <= VarLinks.Num() );
	TArray<FString> Params;
	for (int vl=0; vl<VarLinks.Num(); ++vl )
	{
		FSeqVarLink& InLink = VarLinks(vl);
		TArrayNoInit<class USequenceVariable*> Vars = InLink.LinkedVariables;

		//Params.AddItem( TEXT("") );

		//assert( 1 <= Vars.Num() );
		for (int v=0; v<Vars.Num(); ++v )
		{
			USequenceVariable* SeqVar = Vars(v);
			if ( !SeqVar )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode can't get Var %d"), v), NULL);
				continue;
			}

			if ( SeqVar->IsA(USeqVar_String::StaticClass()) )
			{
				USeqVar_String* VarStr = Cast<USeqVar_String>( SeqVar );
				if ( !VarStr )
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Refer to Not USeqVar_String") ), NULL);
					continue;
				}
				//Params.AddItem( FString::Printf(TEXT("%s"), *(VarStr->GetRef()) ) );
				Params.AddItem( *(VarStr->GetRef()) );
			}
			else if ( SeqVar->IsA(USeqVar_Object::StaticClass()) )
			{
				USeqVar_Object* VarObj = Cast<USeqVar_Object>( SeqVar );
				if ( !VarObj )
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Refer to Not USeqVar_Object") ), NULL);
					continue;
				}
				UObject** Obj = VarObj->GetObjectRef( 0 );
				if ( !Obj || !(*Obj) )
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf(TEXT("BTNode Cannot GetObjectRef")), NULL);
					continue;
				}
				
				TScriptInterface<IIUniqueID> InterfaceActor = ScriptInterfaceCast<IIUniqueID>( *Obj );
				if(InterfaceActor)
				{
					InterfaceActor->InitUniqueID();
					if( InterfaceActor->GetUniqueID() <= 0 )
						goto Exit0;
				}

				if ( (*Obj)->IsA( AMMOServerPoint::StaticClass() ) )
				{
					AMMOServerPoint* Point = Cast<AMMOServerPoint>( *Obj );
					if ( !Point )
					{
						GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Refer to Not AMMOServerPoint") ), NULL);
						continue;
					}

					Params.AddItem( FString::Printf( TEXT("%d"), Point->ServerUniqueID ) );
				}
				else if ( (*Obj)->IsA( AMMOVolume_Trigger::StaticClass() ) )
				{
					AMMOVolume_Trigger* Volume = Cast<AMMOVolume_Trigger>( *Obj );
					if ( !Volume )
					{
						GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Refer to Not AMMOVolume_Trigger") ), NULL);
						continue;
					}

					Params.AddItem( FString::Printf( TEXT("%d"),Volume->ServerUniqueID ) );
				}
				else if ( (*Obj)->IsA( ANPCServerInstance::StaticClass() ) )
				{
					ANPCServerInstance* Group = Cast<ANPCServerInstance>( *Obj );
					if ( !Group )
					{
						GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Refer to Not ANPCServerInstance") ), NULL);
						continue;
					}

					Params.AddItem( FString::Printf( TEXT("%d"),Group->ServerUniqueID ) );
				}
				else if ( (*Obj)->IsA( ANPCNewSpawnGroupActor::StaticClass() ) )
				{
					ANPCNewSpawnGroupActor* Group = Cast<ANPCNewSpawnGroupActor>( *Obj );
					if ( !Group )
					{
						GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Refer to Not ANPCNewSpawnGroupActor") ), NULL);
						continue;
					}
					
					Params.AddItem( FString::Printf( TEXT("%d"),Group->ServerUniqueID ) );
				}
				else if ( (*Obj)->IsA( ANPCNewSpawnGroupMediumActor::StaticClass() ) )
				{
					ANPCNewSpawnGroupMediumActor* Group = Cast<ANPCNewSpawnGroupMediumActor>( *Obj );
					if ( !Group )
					{
						GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Refer to Not ANPCNewSpawnGroupMediumActor") ), NULL);
						continue;
					}

					Params.AddItem( FString::Printf( TEXT("%d"),Group->ServerUniqueID ) );
				}
			}
			else // unkown varlink type
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Var %s Unknown type, index = %d"), *(SeqVar->VarName.ToString()), vl ), NULL);
				Params.AddItem( TEXT("-1") );
				goto Exit0;
			}
		}// for v
	}// for vl

	Buffer += tabs;
	Buffer += FString::Printf( TEXT("{ %s, "),	*(pNode->ObjName) );

	check( Params.Num() <= 4 );
	if ( Params.Num() > 0 )
	{ 
		if ( Params(0).IsEmpty() )
		{
			GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Param 0 is NULL") ), NULL);
			goto Exit0;
		}
		Buffer += FString::Printf( TEXT("param={ %s"), *(Params(0)) );
		for ( INT32 i=1; i<Params.Num(); ++i )
		{
			if ( Params(i).IsEmpty() )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode Param %d is NULL"), i ), NULL);
				break;
			}
			Buffer += FString::Printf( TEXT(", %s"), *(Params(i)) );
		}
		Buffer += TEXT(" },");
	}
	Buffer += TEXT("\n");

	// 2 children
	TArrayNoInit<struct FSeqOpOutputLink>& OutputLinks = pNode->OutputLinks;
	check( 1 == OutputLinks.Num() );
	for (int ol=0; ol<OutputLinks.Num(); ++ol )
	{
		FSeqOpOutputLink& OutLink = OutputLinks(ol);
		TArrayNoInit<struct FSeqOpOutputInputLink>& InLinks = OutLink.Links;

		TArrayNoInit<struct FSeqOpOutputInputLink> DumpLinks = InLinks;
		// bubble sort
		for (int il=0; il<DumpLinks.Num(); ++il )
		{
			USequenceOp* TargetSeqOp = DumpLinks(il).LinkedOp;
			INT TargetPosY = TargetSeqOp->ObjPosY;

			int TargetIndex = il;
			for ( int dl=il+1; dl<DumpLinks.Num(); ++dl  )
			{
				USequenceOp* SeqOp = DumpLinks(dl).LinkedOp;

				if ( SeqOp->ObjPosY < TargetPosY )	// get min
				{
					TargetPosY = SeqOp->ObjPosY;
					TargetIndex = dl;
				}
			}

			if ( TargetIndex != il )
			{
				FSeqOpOutputInputLink TempLink = DumpLinks(il);

				DumpLinks(il) = DumpLinks(TargetIndex);
				DumpLinks(TargetIndex) = TempLink;
			}
		}

		for (int il=0; il<DumpLinks.Num(); ++il )
		{
			FSeqOpOutputInputLink& InLink = DumpLinks(il);

			USequenceOp* SeqOp = InLink.LinkedOp;
			if ( !SeqOp || !SeqOp->IsA(USeqAct_BTNode::StaticClass()) )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode child %d should link a USeqAct_BTNode"), il ), NULL);
				continue;
			}

			USeqAct_BTNode* BTNode = Cast<USeqAct_BTNode>( SeqOp );
			if ( !BTNode )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("BTNode child %d can't cast to USeqAct_BTNode"), il ), NULL);
				continue;
			}

			nRetCode = BTNode->ExportBTNode( Buffer, nDepth+1 );
			if ( !nRetCode )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf( TEXT("fail to ExportBTNode child %d"), il ), NULL);
				break;
			}
		}
	}

	// end
	Buffer += tabs;
	Buffer += TEXT("},\n");

	return TRUE;
Exit0:
	return FALSE;
}

void USeqAct_BTNode::GetDynamicListValues(const DynamicListInfo& ListInfo, TArray<FString>& Values)
{
	check(ListInfo.ListName == TEXT("BTNodeTypeList"));

	TCHAR GBTIni[1024] = {0};
	appStrncpy(GBTIni, *FString::Printf(TEXT("%s%s%sBTNode.ini"), *appGameConfigDir(), INI_PREFIX, GGameName), ARRAY_COUNT(GBTIni));

	TArray<FString> StrArray;
	INT nRetCode = GConfig->GetArray( *(ListInfo.ListName), *(ListInfo.ListName), StrArray, GBTIni );
	if ( nRetCode > 0 )
	{
		for(INT i=0;i<StrArray.Num();i++)
		{
			FString BTEditConfig;
			GConfig->GetString( *StrArray(i), TEXT("BTEdit"), BTEditConfig, GBTIni );
			if ( BTEditConfig == TEXT("BET_Common") )	
			{
				Values.AddItem( StrArray(i) );
			}
			else if ( BTEditConfig == TEXT("BET_Scene") )	
			{
				if ( this->EditType == EBET_Scene )
				{
					Values.AddItem( StrArray(i) );
				}
			}
			else	// AI if no config
			{
				if ( this->EditType == EBET_AI )
				{
					Values.AddItem( StrArray(i) );
				}
			}
		}
	}
}

UBOOL USeqAct_BTNode::CanConnectVarToLink(USequenceVariable *Var, INT LinkIdx)
{
	if ( LinkIdx == 0 && Var )
	{
		UObject** Obj = NULL;
		if ( Var->IsA(USeqVar_Object::StaticClass()) )
		{
			USeqVar_Object* VarObj = Cast<USeqVar_Object>( Var );
			if ( !VarObj )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf(TEXT("BTNode Can't Cast to USeqVar_Object")), NULL);
				return FALSE;
			}

			Obj = VarObj->GetObjectRef( 0 );
			if ( !Obj || !(*Obj) )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf(TEXT("BTNode Can't GetObjectRef")), NULL);
				return FALSE;
			}
		}
		else if ( Var->IsA(USeqVar_String::StaticClass()) )
		{
			return TRUE;// always allow string
		}

		if ( ObjName == TEXT("batSpawnSingleNpc") )
		{
			if ( !Obj || !(*Obj) || !(*Obj)->IsA( ANPCServerInstance::StaticClass() ) )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf(TEXT("BTNode Refer to Not ANPCServerInstance")), NULL);
				return FALSE;
			}
		}
		else if ( ObjName == TEXT("batSpawnMediumGroup") )
		{
			if ( !Obj || !(*Obj) || !(*Obj)->IsA( ANPCNewSpawnGroupMediumActor::StaticClass() ) )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf(TEXT("BTNode Refer to Not ANPCNewSpawnGroupMediumActor")), NULL);
				return FALSE;
			}
		}
		else if ( ObjName == TEXT("batSpawnLargeGroup") )
		{
			if ( !Obj || !(*Obj) || !(*Obj)->IsA( ANPCNewSpawnGroupActor::StaticClass() ) )
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, this, *FString::Printf(TEXT("BTNode Refer to Not ANPCNewSpawnGroupActor")), NULL);
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


