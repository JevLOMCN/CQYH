#include "UnrealEd.h"

#if !SHIPPING_PC_GAME

#include "ExportExcel.h"
#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"
#include "../../MMOGame/Inc/MMOUtil.h"
#include "SourceControl.h"
#include <set>
#include "UnObjectTools.h"

int g_debugIndex2 = 0;
UObject* g_currentProcObject = 0;

//------------------------------------------------------------------------
// Export excel functions
//------------------------------------------------------------------------
void ExportProperty(BYTE*Data, UProperty* ObjPro, PropertyNode& ParentNode, int nIndex);
void ExportPropertyToTextArray(PropertyNode &ParentNode, TArray<FString>& Items);
UBOOL ExportExcelHeader( const TCHAR* Section, TArray<FString>& Header, TArray<FString>& DefaultValue, const TCHAR* Filename );
void ImportProperty(PropertyNode& ParentNode, int& nHeadIndex, TArray<FString>& DefaultValue);
UBOOL ShouldSerializeValue( QWORD PropertyFlags);


// fangruyi
struct ParentNodeInfo
{
	UProperty* ParentProperty;
	INT ParentOffset;
	ParentNodeInfo()
	{
		ParentProperty = NULL;
		ParentOffset = 0;
	}
};
struct PropertyInfo
{
	UProperty* Node;
	FString DefaultValue;
	FString UseFlag;
	FString OverriddenName;
	TArray<ParentNodeInfo> ParentNode;
	INT Offset;
	PropertyInfo()
	{
		Node = NULL;
		Offset = 0;
		UseFlag = FString(TEXT("Both"));
		OverriddenName = FString(TEXT("None"));
	}
};
BYTE* GetAddr(TArray<ParentNodeInfo>& InList, UObject* InObj);
UBOOL ParseClass(UStruct* InClass, FString InParentName, TArray<ParentNodeInfo> InParentNodeList, INT InParentOffset, INT ArrayIdx, TArray<PropertyInfo>& PropertyList);
UBOOL ParseDefaultValue(const TCHAR* InFileName,const TCHAR* InSectionName, TArray<PropertyInfo>& PropertyList);
UBOOL ReadObject(TArray<PropertyInfo>& PropertyList, UObject* InObject, TArray<FString>& Values);
UBOOL WriteObject(TArray<PropertyInfo>& PropertyList, UObject* InObject, TArray<FString>& Headers, TArray<FString>& Values);



void SetDefaultValue(UObject *Obj, const TCHAR* Section)
{
	FString KeyValue;
	GConfig->GetString(TEXT("Excel.Type"), *Obj->GetClass()->GetName(), KeyValue, GEditorIni);

	TArray<FString> Headers, DefaultValues;
	if(ExportExcelHeader(*FString::Printf(TEXT("Excel.%s"),*KeyValue), Headers, DefaultValues, GEditorIni ))
	{
		FString MetaData;
		UProperty* ObjPro;
		PropertyNode ParentNode;
		int nHeadIndex = 1;
		for( TFieldIterator<UField> It(Obj->GetClass()); It; ++It )
		{
			UField* PreviewObj = *It;
			if (PreviewObj)
			{
				ObjPro = Cast<UProperty>(PreviewObj);
				if (ObjPro)
				{
					MetaData = ObjPro->GetMetaData(TEXT("ExportExcel"));
					if(MetaData.Len() > 0)
					{
						BYTE* Data = (BYTE*)Obj + ObjPro->Offset;
						int nIndex = appAtoi(*MetaData);
						//Used for sorting
						ExportProperty(Data, ObjPro, ParentNode, nIndex);
					}
				}
			}
		}
		//Import default values
		ImportProperty(ParentNode, nHeadIndex, DefaultValues);

	}

}

UBOOL ShouldSerializeValue( QWORD PropertyFlags)
{
	UBOOL Skip = FALSE;
	if( (PropertyFlags & (CPF_Native|CPF_Transient|CPF_DuplicateTransient|CPF_ArchetypeProperty|CPF_NonTransactional|CPF_Deprecated)) != 0 )
	{
		Skip =	((PropertyFlags & CPF_Native) != 0)
			||	((PropertyFlags & CPF_Transient) != 0 )
			||	((PropertyFlags & CPF_DuplicateTransient) != 0 )
			||	((PropertyFlags & CPF_ArchetypeProperty) != 0)
			||	((PropertyFlags & CPF_Deprecated) != 0 );
	}
	return !Skip;
}

void ExportProperty(BYTE*Data, UProperty* ObjPro, PropertyNode& ParentNode, int nIndex)//TArray<FString> &Item, int nIndex, UObject* Obj)
{
	for(;ParentNode.ChildNode.Num() <= nIndex;)
	{
		ParentNode.ChildNode.AddItem(PropertyNode());
	}

	if( !(ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UArrayProperty)
		&& !(ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStructProperty) )
	{
		//debugf(TEXT("	 [%d] %d ExportProperty:%s"), g_debugIndex2, nIndex, *ObjPro->GetNameCPP());
	}

	UPropertyValue pv;
	ObjPro->GetPropertyValue(Data, pv);
	if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStrProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *pv.StringValue;
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex2;
		++g_debugIndex2;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UNameProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *pv.NameValue->ToString();
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex2;
		++g_debugIndex2;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UIntProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), pv.IntValue);
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex2;
		++g_debugIndex2;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UFloatProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%f"), pv.FloatValue);
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		FString value = ObjPro->GetMetaData(TEXT("Convert"));
		if(value.Len() > 0)
		{
			int nData = appAtoi(*value);
			int nValue = pv.FloatValue * nData;
			ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), nValue);
		}

		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex2;
		++g_debugIndex2;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UByteProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), pv.ByteValue);
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex2;
		++g_debugIndex2;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UBoolProperty )
	{
		ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), pv.BoolValue);
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex2;
		++g_debugIndex2;
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UArrayProperty )
	{
		UArrayProperty* ArrayProp = (UArrayProperty*)ObjPro;
		for (INT ArrayIndex = 0; ArrayIndex < pv.ArrayValue->Num(); ArrayIndex++)
		{
			ExportProperty(((BYTE*)pv.ArrayValue->GetData())+ArrayIndex*ArrayProp->Inner->ElementSize, ArrayProp->Inner, ParentNode.ChildNode(nIndex), ArrayIndex);//Item, nIndex+ArrayIndex,Obj);
		}
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStructProperty )
	{
		if(((UStructProperty*)ObjPro)->Struct == NULL)
		{
			return;
		}
		UProperty* NextProperty = ((UStructProperty*)ObjPro)->Struct->PropertyLink;
		if(ObjPro->GetMetaData(TEXT("Merger")).Len() > 0)
		{
			int nMerger = 0;
			for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
			{
				for(INT i = 0; i < NextProperty->ArrayDim; ++i)
				{
					if (!ShouldSerializeValue(NextProperty->GetPropertyFlags()))
					{
						continue;
					}
					FString MetaData = NextProperty->GetMetaData(TEXT("ExportExcel"));
					if(MetaData.Len() > 0)
					{
						INT nChildIndex = appAtoi(*MetaData);
						UPropertyValue pvBool;
						NextProperty->GetPropertyValue(Data+NextProperty->Offset + NextProperty->ElementSize*i, pvBool);
						if( NextProperty->GetClass()->ClassCastFlags & CASTCLASS_UBoolProperty )
						{
							nMerger += pvBool.BoolValue << nChildIndex;
						}
					}
				}
			}
			ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), nMerger);
			ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
			ParentNode.ChildNode(nIndex).Data = Data;

			ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
			ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex2;
			++g_debugIndex2;
		}
		else
		{
			for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
			{
				for(INT i = 0; i < NextProperty->ArrayDim; ++i)
				{
					if (!ShouldSerializeValue(NextProperty->GetPropertyFlags()))
					{
						continue;
					}
					FString MetaData = NextProperty->GetMetaData(TEXT("ExportExcel"));
					if(MetaData.Len() > 0)
					{
						INT nChildIndex = appAtoi(*MetaData);
						ExportProperty(Data+NextProperty->Offset + NextProperty->ElementSize*i, NextProperty, ParentNode.ChildNode(nIndex), nChildIndex);//Item, nIndex++, Obj);
					}
				}
			}
		}
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UClassProperty )
	{		
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UObjectProperty )
	{
		ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		ParentNode.ChildNode(nIndex).Data = Data;
		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex2;
		++g_debugIndex2;
		UObject* ret = pv.ObjectValue;
		if(!ret)
		{
			return;
		}
		TScriptInterface<IIReferenceID> ReferenceObj = ScriptInterfaceCast<IIReferenceID>(ret);
		if(ReferenceObj)
		{
			ParentNode.ChildNode(nIndex).StringValue = *FString::Printf(TEXT("%d"), ReferenceObj->GetTemplateID());			
		}
		else
		{
			ParentNode.ChildNode(nIndex).StringValue = ret->GetFullName();
#if defined(_DEBUG)
			FString debugName = ret->GetFName().ToString();
			debugf(TEXT("%s"), *debugName);
#endif
		}

		//ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		//ParentNode.ChildNode(nIndex).Data = Data;
	}
	else
	{
		ParentNode.ChildNode(nIndex).StringValue = *ObjPro->GetFName().ToString();

		ParentNode.ChildNode(nIndex).DebugString = ObjPro->GetNameCPP();
		ParentNode.ChildNode(nIndex).DebugIndex = g_debugIndex2;
		++g_debugIndex2;
		//ParentNode.ChildNode(nIndex).ObjPro = ObjPro;
		//ParentNode.ChildNode(nIndex).Data = Data;
	}

}

void ExportPropertyToTextArray(PropertyNode &ParentNode, TArray<FString>& Items)
{
	if(ParentNode.ChildNode.Num() > 0)
	{
		for( INT i = 0; i < ParentNode.ChildNode.Num(); ++i )
		{
			ExportPropertyToTextArray(ParentNode.ChildNode(i), Items);
		}
	}
	else
	{
// 		debugf(TEXT("	-----%d g[%d] %s Value:%s"), 
// 			Items.Num(), ParentNode.DebugIndex, *ParentNode.DebugString,  
// 			*ParentNode.StringValue);
		Items.AddItem(ParentNode.StringValue);
	}
}

UBOOL ExportExcelHeader( const TCHAR* Section, TArray<FString>& Header, TArray<FString>& DefaultValue, const TCHAR* Filename )
{
	Header.Empty();
	DefaultValue.Empty();
	FConfigFile* File = GConfig->Find( Filename, 0 );
	if( !File )
		return 0;
	FConfigSection* Sec = File->Find( Section );
	if( !Sec )
		return 0;
	for( FConfigSection::TIterator It(*Sec); It; ++It )
	{
		new(Header) FString(FString::Printf( TEXT("%s"), *It.Key().ToString()));
		new(DefaultValue) FString(FString::Printf( TEXT("%s"), *It.Value() ));
	}
	return 1;
}

void ImportProperty(PropertyNode& ParentNode, int& nHeadIndex, TArray<FString>& DefaultValue)
{
	if(ParentNode.ChildNode.Num() > 0)
	{
		for( INT i = 0; i < ParentNode.ChildNode.Num(); ++i )
		{
			ImportProperty(ParentNode.ChildNode(i), nHeadIndex, DefaultValue);
		}

	}
	else
	{
		UProperty* ObjPro = ParentNode.ObjPro;
		BYTE* Data		= ParentNode.Data;
		if(!ObjPro || !Data)
		{
			nHeadIndex++;
			return;
		}
		UPropertyValue pv, pvt;
		//ObjPro->GetPropertyValue(Data, pv);
		ObjPro->GetPropertyValue(Data, pvt);
		if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStrProperty )
		{
			pv.StringValue = &DefaultValue(nHeadIndex);
			if(pv.StringValue != pvt.StringValue)
			{
				ObjPro->SetPropertyValue(Data, pv);
			}
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UNameProperty )
		{
			pv.NameValue = new FName(*DefaultValue(nHeadIndex));
			if(pv.NameValue != pvt.NameValue)
			{
				ObjPro->SetPropertyValue(Data, pv);
			}
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UIntProperty )
		{
			pv.IntValue = appAtoi(*DefaultValue(nHeadIndex));
			if(pv.IntValue != pvt.IntValue)
			{
				ObjPro->SetPropertyValue(Data, pv);
			}
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UFloatProperty )
		{
#if	0
			if(ObjPro->GetName() == TEXT("BaseStiffTime"))
			{
				debugf(TEXT("Debug Break"));
				for (INT j = 0; j < DefaultValue.Num(); ++j)
				{
					debugf(TEXT("\t%d=%s"), j, *DefaultValue(j));
				}
			}

			debugf(TEXT("importing float property:%s %s"), *ObjPro->GetName(), *ObjPro->GetNameCPP());

#endif

			FString value = ObjPro->GetMetaData(TEXT("Convert"));
			if(value.Len() > 0)
			{
				FLOAT nData = appAtof(*value);
				FString strVal = DefaultValue(nHeadIndex);
				pv.FloatValue = appAtof(*strVal);
				pv.FloatValue /= nData; 
			}
			else
			{
				pv.FloatValue = appAtof(*DefaultValue(nHeadIndex));
			}
			
			if(pv.FloatValue != pvt.FloatValue)
			{
				ObjPro->SetPropertyValue(Data, pv);
			}

			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UByteProperty )
		{
#ifdef _DEBUG
			UByteProperty* bp = (UByteProperty*)ObjPro;
			FString debugEnumName = bp->Enum->GetName();
#endif
			FString tmpVal = DefaultValue(nHeadIndex);
			int nValue = appAtoi(*tmpVal);
			if(nValue < 0)
				pv.ByteValue = 0;
			else
				pv.ByteValue = nValue;

			if(pv.ByteValue != pvt.ByteValue)
			{
				ObjPro->SetPropertyValue(Data, pv);
			}
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UBoolProperty )
		{
			pv.BoolValue = appAtoi(*DefaultValue(nHeadIndex));
			if(pv.BoolValue != pvt.BoolValue)
			{
				ObjPro->SetPropertyValue(Data, pv);
			}
			nHeadIndex++;
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UArrayProperty )
		{
// 			UArrayProperty* arrProp = Cast<UArrayProperty>(ObjPro);
// 			for (INT i = 0; i < arrProp->ArrayDim; ++i)
// 			{
// 				//ImportProperty(ParentNode.ChildNode(i), nHeadIndex, DefaultValue);
// 			}
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStructProperty )
		{
			if(((UStructProperty*)ObjPro)->Struct == NULL)
			{
				return;
			}
			UProperty* NextProperty = ((UStructProperty*)ObjPro)->Struct->PropertyLink;
			if(ObjPro->GetMetaData(TEXT("Merger")).Len() > 0)
			{
				int nMerger = appAtoi(*DefaultValue(nHeadIndex));;
				for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
				{
					for(INT i = 0; i < NextProperty->ArrayDim; ++i)
					{
						if (!ShouldSerializeValue(NextProperty->GetPropertyFlags()))
						{
							continue;
						}
						FString MetaData = NextProperty->GetMetaData(TEXT("ExportExcel"));
						if(MetaData.Len() > 0)
						{
							INT nChildIndex = appAtoi(*MetaData);
							UPropertyValue pvBool;
							int n = 1;
							pvBool.BoolValue = nMerger&(n << nChildIndex );
							NextProperty->SetPropertyValue(Data+NextProperty->Offset + NextProperty->ElementSize*i, pvBool);
						}
					}
				}
				nHeadIndex++;
			}
			else
			{
				for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
				{
					for(INT i = 0; i < NextProperty->ArrayDim; ++i)
					{
						if (!ShouldSerializeValue(NextProperty->GetPropertyFlags()))
						{
							continue;
						}
						FString MetaData = NextProperty->GetMetaData(TEXT("ExportExcel"));
						if(MetaData.Len() > 0)
						{
							INT nChildIndex = appAtoi(*MetaData);
							//ImportProperty()
							//ExportProperty(Data+NextProperty->Offset + NextProperty->ElementSize*i, NextProperty, ParentNode.ChildNode(nIndex), nChildIndex);
						}
					}
				}
			}
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UClassProperty )
		{
		}
		else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UObjectProperty )
		{
			UObjectProperty* objectProp = Cast<UObjectProperty>(ObjPro);
			check(objectProp);


			FString objPath = DefaultValue(nHeadIndex);

			if(objPath == TEXT("0"))
			{
				pv.ObjectValue = 0;
				ObjPro->SetPropertyValue(Data, pv);
				nHeadIndex++;
				return;
			}

			if(objectProp->PropertyClass->ImplementsInterface(UIReferenceID::StaticClass()))
			{
				INT templateID = appAtoi(*objPath);

				// find reference object by id
				pv.ObjectValue = FindReferenceObjectByID(objectProp->PropertyClass, templateID);
			}			
			else
			{
				int idx = objPath.InStr(TEXT(" "));
				if(idx != INDEX_NONE)
				{
					objPath = objPath.RightChop(idx + 1);
				}

				pv.ObjectValue = UObject::StaticLoadObject(
					objectProp->PropertyClass, NULL, *objPath, NULL, LOAD_Quiet|LOAD_NoWarn, NULL);
			}

			if(!pv.ObjectValue)
			{
				if(DefaultValue(nHeadIndex).Len() > 0)
				{
					FString FinalDisplayName = objectProp->GetMetaData(TEXT("DisplayName"));
					if(FinalDisplayName.Len() > 0)
					{
						FString LocalizedName = LocalizeProperty( *(objectProp->GetFullGroupName(TRUE)), *FinalDisplayName );	
						if (LocalizedName.Len() > 0)
						{
							FinalDisplayName = LocalizedName;
						}
						else
						{
							FString LocalizedName = LocalizeProperty( *(objectProp->GetFullGroupName(TRUE)), *(objectProp->GetName()) );	
							if (LocalizedName.Len() > 0)
							{
								FinalDisplayName = LocalizedName;
							}
						}
					}

					GWarn->MapCheck_Add(MCTYPE_ERROR, g_currentProcObject,
						*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_PropObjNotFound")), 
						*FinalDisplayName, *objectProp->PropertyClass->GetName(), *DefaultValue(nHeadIndex))), NULL);
				}
			}
			else
			{
				if(pv.ObjectValue && (pv.ObjectValue != pvt.ObjectValue))
				{
					ObjPro->SetPropertyValue(Data, pv);
				}
			}

			nHeadIndex++;
		}
		else
		{
			nHeadIndex++;
		}

	}
}

void ExportToExcel(const TArray< UObject* >& ObjectsInPackages, TArray< TArray<FString> >& data, const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, int nClassType, const FString& ExcelName)
{
#if HAVE_SCC

	if (GEditor->EditorClassHierarchy->GetClass(nClassType) == UQuest::StaticClass())
	{
		UGlobalMapInfo* GlobalMapSingleton = UGlobalMapInfo::GetSingleton();
		if (GlobalMapSingleton)
		{
			GlobalMapSingleton->LoadNPCInfoFromXMLFiles();
			GlobalMapSingleton->LoadNaviPointInfoFromXMLFiles();
		}
	}
	
#if 0
		INT nIndex = 1;
	//TArray< UObject* > ObjectsInPackages;

	//PackageTools::GetObjectsInPackages(&Packages, NULL, ObjectsInPackages);

	INT CurSaveStep = 1;
	INT TotalSteps = ObjectsInPackages.Num();

	GWarn->StatusUpdatef(CurSaveStep, TotalSteps, *FString::Printf(TEXT("Exporting to Excel...")));

	TArray< TScriptInterface<IIExcelable> > exportingObjs;

	for(INT i = 0; i < TotalSteps; ++i)
	{
		UObject* Obj = ObjectsInPackages(i);

		TScriptInterface<IIExcelable> ExcelableObj = ScriptInterfaceCast<IIExcelable>(Obj);
		if(!ExcelableObj)
		{
			continue;
		}

// 		if(Obj->IsA(UEquip::StaticClass()))
// 		{
// 			UEquip* Equip = Cast<UEquip>(Obj);
// 			if(Equip->bDefaultItem)
// 				continue;
// 		}

		exportingObjs.AddItem(ExcelableObj);
	}

	//array<String^, 2>^ ItemList;
	//String^[,] ItemList = gcnew String^[,];

	//data.AddZeroed(exportingObjs.Num() + 2);

	// If the table header has not been exported yet, export it first
	TArray<FString> Headers, DefaultValues;
	if(ExportExcelHeader(*FString::Printf(TEXT("Excel.%s"), *ExcelName), Headers, DefaultValues, GEditorIni ))
	{
		data.AddItem(Headers);
		data.AddItem(DefaultValues);
		//ItemList = gcnew array<String^, 2>(exportingObjs.Num() + 2, Headers.Num());
// 		for (INT i = 0; i < Headers.Num(); ++i)
// 		{
// 			ItemList[0, i] = CLRTools::ToString(Headers(i));
// 			data(i) = Headers(i);			
// 		}
// 
// 		for (INT i = 0; i < DefaultValues.Num(); ++i)
// 		{
// 			ItemList[1, i] = CLRTools::ToString(DefaultValues(i));
// 			data(i) = Headers(i);
// 		}

		// 		ItemList->Add(CLRTools::ToStringArray(Headers));
		// 		ItemList->Add(CLRTools::ToStringArray(DefaultValues));
	}
	else
	{
		GWarn->MapCheck_Add(MCTYPE_ERROR, NULL, 
			*LocalizeUnrealEdMMO(TEXT("CheckError_ExcelHeaderError")), NULL);
		return;
	}

	for(INT i = 0; i < exportingObjs.Num(); ++i)
	{
		TScriptInterface<IIExcelable> ExcelableObj = exportingObjs(i);

		ExcelableObj->InitDataToExcel();		
		UObject* Obj = Cast<UObject>(ExcelableObj->GetUObjectInterfaceIExcelable());
		//Obj->CheckForErrors();

		g_debugIndex2 = 0;

		//Data
		{
			g_currentProcObject = Obj;
			FString MetaData;
			UProperty* ObjPro = NULL;
			TArray<FString> Item, Item2;
			PropertyNode ParentNode;
			INT debugIdx = 0;
			for( TFieldIterator<UField> It(Obj->GetClass()); It; ++It )
			{
				UField* PreviewObj = *It;
				++debugIdx;

				if (PreviewObj)
				{
					ObjPro = Cast<UProperty>(PreviewObj);
					if (ObjPro)
					{
						MetaData = ObjPro->GetMetaData(TEXT("ExportExcel"));
						if(MetaData.Len() > 0)
						{
							//debugf(TEXT("-----%d FullName:%s class:%s"), 
							//	debugIdx, *PreviewObj->GetFullName(), *PreviewObj->GetClass()->GetNameCPP());

							INT nIndex = appAtoi(*MetaData);
							BYTE* Data = (BYTE*)Obj + ObjPro->Offset;
							ExportProperty(Data, ObjPro, ParentNode, nIndex);
						}
					}
				}
			}

			ExportPropertyToTextArray(ParentNode, Item);

			if(Item.Num() > 0)
			{
				nIndex++;

				// add folder name

				Item2.AddItem(Obj->GetPathName());

				UPackage* objPkg = Obj->GetOutermost();

				FString pkgRoot;
				FString pkgName = objPkg->GetName();
				GPackageFileCache->FindPackageFile(*pkgName, NULL, pkgRoot);

				pkgRoot = pkgRoot.Left(pkgRoot.Len() - pkgName.Len() - 5); // + "\*.upk"
				INT rvIdx = pkgRoot.InStr(TEXT("\\"), TRUE);
				if(rvIdx != INDEX_NONE)
				{
					pkgRoot = pkgRoot.Right(pkgRoot.Len() - rvIdx - 1);
				}

				Item2.AddItem(pkgRoot);

				//Item2.AddItem(FString::Printf(TEXT("%d"), nIndex++));
				Item2 += Item;
				//ItemList->Add(CLRTools::ToStringArray(Item2));
				data.AddItem(Item2);
				/*for (INT j = 0; j < Item2.Num(); ++j)
				{
					INT  idx = i + 2;
					ItemList[idx, j] = CLRTools::ToString(Item2(j));
				}*/
			}
		}

		GWarn->UpdateProgress( ++CurSaveStep, TotalSteps );
	}

#else

	INT CurSaveStep = 1;
	INT TotalSteps = ObjectsInPackages.Num();

	GWarn->StatusUpdatef(CurSaveStep, TotalSteps, *FString::Printf(TEXT("Exporting to Excel...")));

	TArray< TScriptInterface<IIExcelable> > exportingObjs;
	UClass* ExportClass = GEditor->EditorClassHierarchy->GetClass(nClassType);

	for(INT i = 0; i < TotalSteps; ++i)
	{
		UObject* Obj = ObjectsInPackages(i);

		if (ExportClass != Obj->GetClass())
		{
			continue;
		}

		TScriptInterface<IIExcelable> ExcelableObj = ScriptInterfaceCast<IIExcelable>(Obj);
		if(!ExcelableObj)
		{
			continue;
		}

		exportingObjs.AddItem(ExcelableObj);
	}

	TArray<PropertyInfo> PropertyList;
	TArray<ParentNodeInfo> ParentNodeList;
	ParseClass(ExportClass, FString(), ParentNodeList, 0, -1, PropertyList);

	ParseDefaultValue(GEditorIni, *FString::Printf(TEXT("Excel.%s"), *ExcelName), PropertyList);

	TArray<FString> Headers, DefaultValues, UseFlags;
	Headers.AddItem(FString(TEXT("Index")));
	Headers.AddItem(FString(TEXT("PackageFolder")));

	DefaultValues.AddItem(FString(TEXT("0")));
	DefaultValues.AddItem(FString(TEXT("0")));

	UseFlags.AddItem(FString(TEXT("Client")));
	UseFlags.AddItem(FString(TEXT("N/A")));

	for (INT i = 0; i < PropertyList.Num(); ++i)
	{
		Headers.AddItem(PropertyList(i).OverriddenName);
		DefaultValues.AddItem(PropertyList(i).DefaultValue);
		UseFlags.AddItem(PropertyList(i).UseFlag);
	}
	data.AddItem(Headers);
	data.AddItem(UseFlags);
	data.AddItem(DefaultValues);


	for(INT i = 0; i < exportingObjs.Num(); ++i)
	{
		TScriptInterface<IIExcelable> ExcelableObj = exportingObjs(i);

		ExcelableObj->InitDataToExcel();
		
		UObject* Obj = Cast<UObject>(ExcelableObj->GetUObjectInterfaceIExcelable());

		TArray<FString> NewNode;
		ReadObject(PropertyList, Obj, NewNode);
		data.AddItem(NewNode);

		GWarn->UpdateProgress( ++CurSaveStep, TotalSteps );
	}

#endif


#endif // HAVE_SCC
}

void AnalyzeReference(FString FolderName, TArray<FString>& PackageNames)
{
	class FArchiveAnalyzeReferences : public FArchive
	{
		UObject* BaseObject;
		TArray<FString> TypeList;
		TArray<UObject*> OutObjects;

		TArray<UObject*> SearchedObjectList;

		FArchive& operator<<( UObject*& Obj )
		{
			if (!Obj)
			{
				return *this;
			}

			if (SearchedObjectList.FindItemIndex(Obj) != INDEX_NONE)
			{
				return *this;
			}

			SearchedObjectList.AddItem(Obj);

			UObject* OuterMost = BaseObject->GetOutermost();
			if (TypeList.FindItemIndex(*Obj->GetClass()->GetName()) == INDEX_NONE)
			{
				if (Obj->IsIn(OuterMost))
				{
					//debugf(L"To Serialize:%s", *Obj->GetFullName());
					Obj->Serialize(*this);
				}
			}
			else
			{
				//debugf(L"Got a res : %s", *Obj->GetFullName());
				OutObjects.AddItem(Obj);
			}
			return *this;
		}
	public:

		void GenerateReference(UObject* InObject, TArray<FString>& InTypeList, TArray<UObject*>& InOutObjects)
		{
			check(InObject);
			TypeList = InTypeList;
			BaseObject = InObject;
			BaseObject->Serialize(*this);
			InOutObjects = this->OutObjects;
		}
	};


	struct Node
	{
		FString ObjectName;
		TArray<FString> ReferenceObjectNames;
		TArray<FString> ReferenceObjectPathNames;
	};

	struct ReferenceInfo
	{
		FString PackageName;
		TArray<Node> NodeList;
	};


	TArray<FString> ResourceTypeNames;
	ResourceTypeNames.AddItem(FString(TEXT("SkeletalMesh")));
	ResourceTypeNames.AddItem(FString(TEXT("StaticMesh")));
	ResourceTypeNames.AddItem(FString(TEXT("ParticleSystem")));
	ResourceTypeNames.AddItem(FString(TEXT("Material")));
	ResourceTypeNames.AddItem(FString(TEXT("MaterialInstanceConstant")));
	ResourceTypeNames.AddItem(FString(TEXT("MaterialInstanceTimeVarying")));
	ResourceTypeNames.AddItem(FString(TEXT("CameraAnim")));
	ResourceTypeNames.AddItem(FString(TEXT("AnimSet")));
	ResourceTypeNames.AddItem(FString(TEXT("PostProcessEffect")));
	ResourceTypeNames.AddItem(FString(TEXT("SoundCue")));
	ResourceTypeNames.AddItem(FString(TEXT("SoundNode")));
	ResourceTypeNames.AddItem(FString(TEXT("Texture2D")));
	ResourceTypeNames.AddItem(FString(TEXT("AvatarData")));

	TArray<ReferenceInfo> ReferenceInfoList;


	for (INT i = 0; i < PackageNames.Num(); ++i)
	{
		SET_WARN_COLOR(COLOR_GREEN);
		warnf(NAME_Log, TEXT("Processing package [%d/%d] %s..."), i + 1, PackageNames.Num(), *PackageNames(i));
		CLEAR_WARN_COLOR();

		FString FileName;
		if (GPackageFileCache->FindPackageFile(*PackageNames(i), NULL, FileName) == FALSE)
		{
			continue;
		}

		ReferenceInfo Info;
		Info.PackageName = FileName;

		UPackage* ToPrcessPackage = UPackage::LoadPackage(NULL, *FileName, LOAD_None);
		check(ToPrcessPackage);
		ToPrcessPackage->FullyLoad();

		for (FObjectIterator j; j; ++j)
		{
			UObject* io = *j;
			if (io->IsIn(ToPrcessPackage) && io->HasAnyFlags(RF_Public) && io->GetOuter() && io->GetOuter()->IsA(UPackage::StaticClass()))
			{
				FArchiveAnalyzeReferences Archive;
				TArray<UObject*> OutList;
				Archive.GenerateReference(io, ResourceTypeNames, OutList);

				INT idx = Info.NodeList.AddZeroed();
				Info.NodeList(idx).ObjectName = io->GetFullName();
				for (INT k = 0; k < OutList.Num(); ++k)
				{
					INT idx1 = Info.NodeList(idx).ReferenceObjectNames.AddZeroed();
					Info.NodeList(idx).ReferenceObjectNames(idx1) = OutList(k)->GetFullName();
					Info.NodeList(idx).ReferenceObjectPathNames.AddZeroed();

					UPackage* OuterMost = Cast<UPackage>(OutList(k)->GetOutermost());
					if (OuterMost)
					{
						FString FullFileName;
						if (GPackageFileCache->FindPackageFile(*OuterMost->GetName(), 0, FullFileName))
						{
							Info.NodeList(idx).ReferenceObjectPathNames(idx1) = FullFileName;
						}
					}
				}
			}
		}

		ReferenceInfoList.AddItem(Info);

		//ULinkerLoad* Linker = ToPrcessPackage->GetPackageLinker(NULL, *FileName, LOAD_Quiet|LOAD_NoWarn|LOAD_NoVerify, NULL, NULL );

		//TArray<FString> ImportObjectList;
		//ImportObjectList.AddZeroed(Linker->ImportMap.Num());
		//for (INT i = 0; i < Linker->ImportMap.Num(); ++i)
		//{
		//	if (Linker->ImportMap(i).OuterIndex != 0 && ResourceTypeNames.FindItemIndex(Linker->ImportMap(i).ClassName.ToString()) != INDEX_NONE)
		//	{
		//		FString PathName;
		//		Linker->BuildPathName(PathName, -i - 1);
		//		UObject* ImportObject = LoadObject<UObject>(NULL, *PathName, 0, 0, 0);
		//		if (ImportObject)
		//		{
		//			Info.ReferenceObjectNames.AddItem(ImportObject->GetFullName());
		//			UPackage* OuterMost = Cast<UPackage>(ImportObject->GetOutermost());
		//			if (OuterMost)
		//			{
		//				FString FullFileName;
		//				if (GPackageFileCache->FindPackageFile(*OuterMost->GetName(), 0, FullFileName))
		//				{
		//					Info.ReferenceObjectPathNames.AddItem(FullFileName);
		//				}
		//			}
		//		}
		//	}
		//}

		//ReferenceInfoList.AddItem(Info);
	}

	FString ToSaveFile = FString(TEXT("..\\..\\MMOGame\\ExportData\\")) + FolderName + FString(TEXT(".csv"));
	FArchive* CSVFile = GFileManager->CreateFileWriter( *ToSaveFile );
	if (CSVFile)
	{
		FString Head(TEXT("Package,Object,ReferencedResouce")LINE_TERMINATOR);
		CSVFile->Serialize(TCHAR_TO_ANSI(*Head), Head.Len());

		for (INT i = 0; i < ReferenceInfoList.Num(); ++i)
		{
			FString text = ReferenceInfoList(i).PackageName + FString(TEXT(", ,")LINE_TERMINATOR);
			CSVFile->Serialize(TCHAR_TO_ANSI(*text), text.Len());
			for (INT j = 0; j < ReferenceInfoList(i).NodeList.Num(); ++j)
			{
				text = FString(TEXT(" ,")) + ReferenceInfoList(i).NodeList(j).ObjectName + FString(TEXT(",")LINE_TERMINATOR);
				CSVFile->Serialize(TCHAR_TO_ANSI(*text), text.Len());

				for (INT k = 0; k < ReferenceInfoList(i).NodeList(j).ReferenceObjectNames.Num(); ++k)
				{
					text = FString(TEXT(" , ,")) + ReferenceInfoList(i).NodeList(j).ReferenceObjectNames(k) + FString(TEXT(",")) + ReferenceInfoList(i).NodeList(j).ReferenceObjectPathNames(k) + FString(LINE_TERMINATOR);
					CSVFile->Serialize(TCHAR_TO_ANSI(*text), text.Len());
				}
			}
		}

		CSVFile->Close();
		delete CSVFile;
	}
}

FString FindDataTypeSavePath(const FString& className)
{
	for (INT i = 0; i < GSys->DataTypes.Num(); ++i)
	{
		if(GSys->DataTypes(i) == className)
		{
			return GSys->DataPaths(i);
		}
	}

	return TEXT("");
}



struct FSavePkg
{
	FString Path;
	UPackage* Pkg;
};

struct FSavePkgCompare
{
	bool operator()(const FSavePkg& lhs, const FSavePkg& rhs) const
	{
		return lhs.Pkg < rhs.Pkg;
	}
};

typedef std::set< FSavePkg, FSavePkgCompare, M_UE3_STL_ALLOC(FSavePkg) > SavePackageSet;


void ImportFromExcel(TArray< TArray<FString> >& dataSource, TMap<FString, UObject* >& ObjectsInPackages, const TArray<UPackage*>& Packages, const TArray<FString>& PackageNames, const FString& Path, UClass* classType)
{
#if HAVE_SCC

#if 0
	TArray< TArray<FString> > newObjList;

	if(dataSource.Num() <= 2)
	{
		return;
	}

	INT CurSaveStep = 0;
	INT TotalSteps = ObjectsInPackages.Num();
	GWarn->StatusUpdatef(CurSaveStep, TotalSteps,*FString::Printf(TEXT("Importing Excel")));

	int rowIdx = 2; // skip table header and default data group

	for(; rowIdx < dataSource.Num(); ++rowIdx)
	{
		if(dataSource(rowIdx).Num() > 0)
		{
			FString objName = dataSource(rowIdx)(0);
			UObject** ObjPtr = ObjectsInPackages.Find(objName);
			UObject* Obj = NULL;
			if(ObjPtr && *ObjPtr)
			{
				Obj = *ObjPtr;
			}
			else
			{
				const FString& nameId = dataSource(rowIdx)(0);
				int idx2 = nameId.InStr(TEXT("."));
				if(idx2 == INDEX_NONE)
				{
					GWarn->MapCheck_Add(MCTYPE_ERROR, g_currentProcObject,
						*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_ExcelRowDataError")), rowIdx, *nameId)), NULL);
				}
				else
				{
					newObjList.AddItem(dataSource(rowIdx));
				}
				continue;
			}

			TScriptInterface<IIExcelable> ExcelableObj = ScriptInterfaceCast<IIExcelable>(Obj);
			if(!ExcelableObj)
			{
				continue;
			}

			// 			if(Obj->IsA(UEquip::StaticClass()))
			// 			{
			// 				UEquip* Equip = Cast<UEquip>(Obj);
			// 				if(Equip->bDefaultItem)
			// 					continue;
			// 			}

			FString MetaData;
			UProperty* ObjPro = 0;
			PropertyNode ParentNode;
			int nHeadIndex = 2;
			g_currentProcObject = Obj;
			for( TFieldIterator<UField> It(Obj->GetClass()); It; ++It )
			{
				UField* PreviewObj = *It;
				if (PreviewObj)
				{
					ObjPro = Cast<UProperty>(PreviewObj);
					if (ObjPro)
					{
						MetaData = ObjPro->GetMetaData(TEXT("ExportExcel"));
						if(MetaData.Len() > 0)
						{
							BYTE* Data = (BYTE*)Obj + ObjPro->Offset;
							int nIndex = appAtoi(*MetaData);
							//Used for sorting
							ExportProperty(Data, ObjPro, ParentNode, nIndex);
						}
					}
				}
			}

			TArray<FString>& StrArr = (TArray<FString>&)dataSource(rowIdx);
			//CLRTools::ToFStringArray((*ExcelData)[rowIdx], StrArr);
			//Import default values
			ImportProperty(ParentNode, nHeadIndex, StrArr);
			TScriptInterface<IIExcelable> excelObj = ScriptInterfaceCast<IIExcelable>(Obj);
			check(excelObj);
			excelObj->ImportFromExcel(StrArr);
		}

		GWarn->UpdateProgress( ++CurSaveStep, TotalSteps );
	}

	SavePackageSet newPackages;

	UObject* const BaseArchetype = UObject::StaticConstructObject(classType);

	for (int i = 0; i < newObjList.Num(); ++i)
	{
		//FString objTypePathName = newObjList(i)(0);
		// 		INT idx = objTypePathName.InStr(TEXT("\'"));
		// 		// find class type name string
		// 		if(idx == INDEX_NONE)
		// 		{
		// 			continue;
		// 		}

		// 		FString InBaseClassName = objTypePathName.Left(idx);



		UClass* TheClass = classType;
		//(UClass*)UObject::StaticFindObject(UClass::StaticClass(),
		//	ANY_PACKAGE, *InBaseClassName, TRUE);

		FString objPathName = newObjList(i)(0);//objTypePathName.Mid(idx + 1, objTypePathName.Len() - idx - 1);
		FString pkgFolderName = newObjList(i)(1);

		FString pkgName, grpName, objName;

		int idx2 = objPathName.InStr(TEXT("."));
		if(idx2 != INDEX_NONE)
		{
			pkgName = objPathName.Mid(0, idx2);
		}

		FString tmpStr = objPathName.Right(objPathName.Len() - idx2 - 1);
		//tmpStr.RightChop(1);
		int idx3 = tmpStr.InStr(TEXT("."), TRUE);
		if(idx3 != INDEX_NONE)
		{
			grpName = tmpStr.Left(idx3);
			objName = tmpStr.Right(tmpStr.Len() - idx3 - 1);
		}
		else
		{
			objName = tmpStr;
		}


		UPackage* newPkg = GEngine->CreatePackage(NULL, *pkgName);

		UPackage* newGrp = NULL;
		if( grpName.Len() )
		{
			// first try loading the group, in case it already exists
			newGrp = LoadObject<UPackage>(newPkg, *grpName, NULL, LOAD_NoWarn|LOAD_FindIfFail,NULL);
			if ( newGrp == NULL )
			{
				newGrp = GEngine->CreatePackage(newPkg, *grpName);
			}	
		}

		if ( newGrp == NULL )
		{
			newGrp = newPkg;
		}

		newPkg->SetDirtyFlag(TRUE);

		FSavePkg pkgData;
		pkgData.Pkg = newPkg;
		pkgData.Path = FindDataTypeSavePath(TheClass->GetName()) + TEXT("\\") + pkgFolderName + TEXT("\\") + pkgName + TEXT(".upk");

		GPackageFileCache->CacheDataPath(*pkgData.Path, *TheClass->GetName());

		newPackages.insert(pkgData);

		FString objTypePathName = FString::Printf(TEXT("%s\'%s\'"), *TheClass->GetName(), *objPathName);

		UObject* newObj = UObject::StaticFindObject(TheClass, newGrp, *objName);
		if(!newObj)
		{
			newObj = BaseArchetype->CreateArchetype(*objName, newGrp);

			TScriptInterface<IIExcelable> ExcelableObj = ScriptInterfaceCast<IIExcelable>(newObj);
			if(ExcelableObj)
			{
				SetDefaultValue(newObj, NULL);
			}

			FString MetaData;
			UProperty* ObjPro = 0;
			PropertyNode ParentNode;
			int nHeadIndex = 2;
			for( TFieldIterator<UField> It(newObj->GetClass()); It; ++It )
			{
				UField* PreviewObj = *It;
				if (PreviewObj)
				{
					ObjPro = Cast<UProperty>(PreviewObj);
					if (ObjPro)
					{
						MetaData = ObjPro->GetMetaData(TEXT("ExportExcel"));
						if(MetaData.Len() > 0)
						{
							BYTE* Data = (BYTE*)newObj + ObjPro->Offset;
							int nIndex = appAtoi(*MetaData);
							//Used for sorting
							ExportProperty(Data, ObjPro, ParentNode, nIndex);
						}
					}
				}
			}

			TArray<FString>& StrArr = newObjList(i);
			ImportProperty(ParentNode, nHeadIndex, StrArr);

			TScriptInterface<IIExcelable> excelObj = ScriptInterfaceCast<IIExcelable>(newObj);
			check(excelObj);
			excelObj->ImportFromExcel(StrArr);
			//newObj->CheckForErrors();

			// Inform the content browser of the newly created archetype
			GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, newObj));

		}
		else
		{			
		}

		GWarn->UpdateProgress( ++CurSaveStep, TotalSteps );
	}

	TotalSteps = (INT)newPackages.size();
	CurSaveStep = 0;
	GWarn->StatusUpdatef(CurSaveStep, TotalSteps,*FString::Printf(TEXT("Saving new packages...")));

	SavePackageSet::iterator its = newPackages.begin();
	for (; its != newPackages.end(); ++its)
	{
		FSavePkg& dat = (FSavePkg&)*its;
		UPackage* pkg = dat.Pkg;
		//pkg->FullyLoad();
		pkg->MarkPackageDirty();

		if( !GEditor->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""),
			*pkg->GetName(), *dat.Path) ) )
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, pkg, 
				*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_CanNotSavePackage")), *dat.Path)), NULL);
		}
		else
		{
			FString savePath = GFileManager->ConvertToAbsolutePath(*dat.Path);
			FCheckoutResult sccResult = FSourceControl::TryCheckOut(savePath);
			if(sccResult.AddToSCC)
			{
				sccResult.TryAddToDefaultChangelist();
			}
		}

		GWarn->UpdateProgress( ++CurSaveStep, TotalSteps );
	}

#endif


	INT CurSaveStep = 0;
	INT TotalSteps = ObjectsInPackages.Num();
	GWarn->StatusUpdatef(CurSaveStep, TotalSteps,*FString::Printf(TEXT("Importing Excel")));

	TArray<PropertyInfo> PropertyList;
	TArray<ParentNodeInfo> ParentNodeList;
	ParseClass(classType, FString(), ParentNodeList, 0, -1, PropertyList);

	SavePackageSet newPackages;

	TArray<FString> &Headers = dataSource(0);

	for (INT i = 3; i < dataSource.Num(); ++i)
	{
		UObject* CurrentObject = NULL;
		FString& ObjectName = dataSource(i)(0);
		FString& SubFolder = dataSource(i)(1);
		UObject** ppObj = ObjectsInPackages.Find(ObjectName);
		if (ppObj && *ppObj)
		{
			CurrentObject = *ppObj;
		}
		else
		{
			FString PackageName, GroupName, FileName;
			INT idx1 = ObjectName.InStr(TEXT("."), FALSE, FALSE, -1);
			INT idx2 = ObjectName.InStr(TEXT("."), FALSE, FALSE, idx1+1);
			INT idx3 = ObjectName.InStr(TEXT("."), FALSE, FALSE, idx2+1);
			if (idx1 == -1 || (idx2 != -1 && idx3 != -1))
			{
				GWarn->MapCheck_Add(MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Index Name Error Index=%s, row=%d"), *ObjectName, i), NULL);
				continue;
			}

			PackageName = ObjectName.Left(idx1);
			if (idx2 != -1)
			{
				GroupName = ObjectName.Mid(idx1 + 1, idx2 - idx1 - 1);
				FileName = ObjectName.Mid(idx2 + 1, ObjectName.Len() - idx2 - 1);
			}
			else
			{
				FileName = ObjectName.Mid(idx1 + 1, ObjectName.Len() - idx1 - 1);
			}

			UPackage* Package = GEngine->CreatePackage(NULL, *PackageName);
			if (!Package)
			{
				GWarn->MapCheck_Add(MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Create Package failed. Index=%s, row=%d"), *ObjectName, i), NULL);
				continue;
			}
			if( GroupName.Len() > 0)
			{
				Package = GEngine->CreatePackage( Package, *GroupName );
				if (!Package)
				{
					GWarn->MapCheck_Add(MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Create Package  gourp failed. Index=%s, row=%d"), *ObjectName, i), NULL);
					continue;
				}
			}

			UObject* DefaultObj = classType->GetDefaultObject();
			CurrentObject = DefaultObj->CreateArchetype(*FileName, Package, DefaultObj);

			FString pkgFolderName = SubFolder;
			FSavePkg pkgData;
			pkgData.Pkg = Package;
			pkgData.Path = FindDataTypeSavePath(classType->GetName()) + TEXT("\\") + pkgFolderName + TEXT("\\") + PackageName + TEXT(".upk");

			GPackageFileCache->CacheDataPath(*pkgData.Path, *classType->GetName());

			newPackages.insert(pkgData);
			GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, CurrentObject));

		}

		if (!CurrentObject)
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Get CurrentObject failed. Index=%s, row=%d"), *ObjectName, i), NULL);
			continue;
		}

		WriteObject(PropertyList, CurrentObject, Headers, dataSource(i));

		TScriptInterface<IIExcelable> EObj = ScriptInterfaceCast<IIExcelable>(CurrentObject);
		if (EObj)
		{
			EObj->ImportFromExcel(dataSource(i));
		}

		// Inform the content browser of the newly created archetype
		GCallbackEvent->Send(FCallbackEventParameters(NULL, CALLBACK_RefreshContentBrowser, CBR_ObjectCreated, CurrentObject));

		GWarn->UpdateProgress( ++CurSaveStep, TotalSteps );
	}


	TotalSteps = (INT)newPackages.size();
	CurSaveStep = 0;
	GWarn->StatusUpdatef(CurSaveStep, TotalSteps,*FString::Printf(TEXT("Saving new packages...")));

	SavePackageSet::iterator its = newPackages.begin();
	for (; its != newPackages.end(); ++its)
	{
		FSavePkg& dat = (FSavePkg&)*its;
		UPackage* pkg = dat.Pkg;
		//pkg->FullyLoad();
		pkg->MarkPackageDirty();

		if( !GEditor->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""),
			*pkg->GetName(), *dat.Path) ) )
		{
			GWarn->MapCheck_Add(MCTYPE_ERROR, pkg, 
				*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_CanNotSavePackage")), *dat.Path)), NULL);
		}
		else
		{
			FString savePath = GFileManager->ConvertToAbsolutePath(*dat.Path);
			FCheckoutResult sccResult = FSourceControl::TryCheckOut(savePath);
			if(sccResult.AddToSCC)
			{
				sccResult.TryAddToDefaultChangelist();
			}
		}

		GWarn->UpdateProgress( ++CurSaveStep, TotalSteps );
	}


#endif // HAV_SCC
}

// FindReferenceObjectByIDPtr g_findFun[] =
// {
// 	FindReferenceObjectByID_Ability,
// 	FindReferenceObjectByID_Item,
// 	FindReferenceObjectByID_Equip,
// 	FindReferenceObjectByID_NPCTemplate,
// 	FindReferenceObjectByID_Quest
// };

class UObject* FindReferenceObjectByID( class UClass* classType, INT templateID )
{
	class UObject* obj = 0;

#if 1
	if(_FindReferenceObjectByID(classType, templateID, obj))
	{
		return obj;
	}
#else
	const int sz = sizeof(g_findFun);
	for (int i = 0; i < sz; ++i)
	{
		if(_FindReferenceObjectByID(classType, templateID, obj))
		{
			return obj;
		}
	}
#endif
	return obj;
}

bool _FindReferenceObjectByID(class UClass* classType, INT templateID, class UObject*& outPtr)
{
	for( TObjectIterator<UObject> It ; It ; ++It )
	{
		UObject* Obj = *It;
		if(Obj && Obj->IsA(classType))
		{
			TScriptInterface<IIReferenceID> referenceObj = ScriptInterfaceCast<IIReferenceID>(Obj);
			if(referenceObj && (referenceObj->GetTemplateID() == templateID))
			{
				outPtr = Obj;
				return true;
			}
		}
	}

	return false;
}

void CheckIDError( class UClass* classType, const TArray<UPackage*>& pkgs)
{
	TArray<UObject*> objs;

#if 0
	for( TObjectIterator<UObject> It ; It ; ++It )
	{		
		UObject* obj = *It;
		if(obj->IsA(classType) && (obj->GetOutermost() != UObject::GetTransientPackage()))
		{
			objs.AddItem(obj);
		}
	}
#else
	for (INT i = 0; i < pkgs.Num(); ++i)
	{
		UPackage* pkg = pkgs(i);
		INT maxCnt = pkg->GetNetObjectCount(0);
		for (INT j = 0; j < maxCnt; ++j)
		{
			UObject* obj = pkg->GetNetObjectAtIndex(j);
			if(obj && obj->IsA(classType))
			{
				objs.AddItem(obj);
			}
		}
	}
#endif

	for (INT i = 0; i < objs.Num(); ++i)
	{
		UObject* Obj1 = objs(i);

		if(Obj1)
		{
			Obj1->CheckForErrors();

			if(classType != UMMOAbility::StaticClass()) // abilities have duplicate IDs
			{
				TScriptInterface<IIReferenceID> referenceObj1 = ScriptInterfaceCast<IIReferenceID>(Obj1);
				if(referenceObj1)
				{
					for (INT j = i + 1; j < objs.Num(); ++j)
					{
						UObject* Obj2 = objs(j);

						if(Obj2 && (Obj1 != Obj2))
						{
							TScriptInterface<IIReferenceID> referenceObj2 = ScriptInterfaceCast<IIReferenceID>(Obj2);
							if(referenceObj2)
							{
								if(referenceObj1->GetTemplateID() == referenceObj2->GetTemplateID())
								{
									GWarn->MapCheck_Add(MCTYPE_ERROR, Obj1,
										*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_IDConflict")),
										referenceObj1->GetTemplateID(), *Obj1->GetName(), *Obj2->GetName())),
										NULL);

									GWarn->MapCheck_Add(MCTYPE_ERROR, Obj2,
										*FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_IDConflict")),
										referenceObj1->GetTemplateID(), *Obj1->GetName(), *Obj2->GetName())),
										NULL);
								}
							}
						}
					}
				}
			}
		}
	}
}

// bool FindReferenceObjectByID_Item(class UClass* classType, INT templateID, class UObject*& outPtr);
// bool FindReferenceObjectByID_Equip(class UClass* classType, INT templateID, class UObject*& outPtr);
// bool FindReferenceObjectByID_NPCTemplate(class UClass* classType, INT templateID, class UObject*& outPtr);
// bool FindReferenceObjectByID_Quest(class UClass* classType, INT templateID, class UObject*& outPtr);

#define M_DEBUG_RECV_CHK 0

#if M_DEBUG_RECV_CHK
INT g_debugPadding = 0;

FString DbgChk_PrintTabs()
{
	FString str;
	for (INT i = 0; i < g_debugPadding; ++i)
	{
		str += TEXT(" ");
	}
	return str;
}
#endif

void RecursiveCheckForError(UProperty* ObjPro, UObject* obj)
{
	if(ObjPro && !(CPF_Edit & ObjPro->PropertyFlags))
	{
		return;
	}

	if(obj == obj->GetClass()->GetDefaultObject())
	{
		return;
	}

	if(ObjPro->GetName() == TEXT("ObjectArchetype"))
	{
		return;
	}

#if M_DEBUG_RECV_CHK
	++g_debugPadding;
	//warnf(NAME_Log, TEXT("%s %s->%s[%s]"), *DbgChk_PrintTabs(), *obj->GetName(), *ObjPro->GetName(), *ObjPro->GetClass()->GetName());
#endif

	if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UArrayProperty )
	{
// 		UArrayProperty* arrProp = Cast<UArrayProperty>(ObjPro);		
// 		for (INT i = 0; i < arrProp->ArrayDim; ++i)
// 		{
// 			IF(arrProp->arr(i))
// 			//ImportProperty(ParentNode.ChildNode(i), nHeadIndex, DefaultValue);
// 		}
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UStructProperty )
	{
		if(((UStructProperty*)ObjPro)->Struct == NULL)
		{
			return;
		}
		UProperty* NextProperty = ((UStructProperty*)ObjPro)->Struct->PropertyLink;
		for (; NextProperty != NULL; NextProperty = NextProperty->PropertyLinkNext)
		{
			//RecursiveCheckForError(NextProperty, obj);
		}
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UClassProperty )
	{
	}
	else if( ObjPro->GetClass()->ClassCastFlags & CASTCLASS_UObjectProperty )
	{
#if M_DEBUG_RECV_CHK
		warnf(NAME_Log, TEXT("%s %s->%s[%s]"), *DbgChk_PrintTabs(), *obj->GetName(), *ObjPro->GetName(), *ObjPro->GetClass()->GetName());
#endif

		UObjectProperty* objectProp = Cast<UObjectProperty>(ObjPro);
		check(objectProp);
	
		BYTE* addrOffset = (BYTE*)obj + ObjPro->Offset;
		UPropertyValue propVal;
		objectProp->GetPropertyValue(addrOffset, propVal);
		if(propVal.ObjectValue)
		{
			propVal.ObjectValue->CheckForErrors();

			for( TFieldIterator<UField> Itf(propVal.ObjectValue->GetClass()); Itf; ++Itf )
			{
				UField* PreviewObj = *Itf;
				if (PreviewObj)
				{
					UProperty* ObjPro = Cast<UProperty>(PreviewObj);
					if (ObjPro)
					{
						RecursiveCheckForError(ObjPro, propVal.ObjectValue);
					}
				}
			}
		}
	}

#if M_DEBUG_RECV_CHK
	--g_debugPadding;
#endif
}


void FixModifier()
{
	GWarn->MapCheck_Clear();
	GWarn->BeginSlowTask(TEXT("Fixing modifer..."), TRUE);
	
	FString cfgDir = TEXT("..\\..\\MMOGame\\Content\\MMOContent\\08_Modifier");
	cfgDir = GFileManager->ConvertToAbsolutePath(*cfgDir);

	TArray<USeqAct_Modifier*> objs;
	TArray<UPackage*> pkgs;
	TArray<FString> pkgNames;
	appFindFilesInDirectory(pkgNames, *cfgDir, TRUE, FALSE);
	for (INT i = 0; i < pkgNames.Num(); ++i)
	{
		UPackage* Package = UObject::LoadPackage( NULL, *pkgNames(i), LOAD_Verify );
		if( !Package )
		{
			warnf( NAME_Error, TEXT("Unable to load %s"), *pkgNames(i) );
			continue;;
		}

		pkgs.AddItem(Package);

		if(!Package->IsFullyLoaded())
		{
			Package->FullyLoad();
		}

		INT maxCnt = Package->GetNetObjectCount(0);
		for (INT j = 0; j < maxCnt; ++j)
		{
			UObject* obj = Package->GetNetObjectAtIndex(j);
			if(obj->IsA(USeqAct_Modifier::StaticClass()))
			{
				objs.AddItem(Cast<USeqAct_Modifier>(obj));
			}
		}
	}

	TArray<USeqAct_Modifier*> needFix;
	// Fix logical structure errors according to input/output
	for (INT i = 0; i < objs.Num(); ++i)
	{
		USeqAct_Modifier* obj = objs(i);
		if(obj->Modifiers.Num() > 1)
		{
			bool errorModifier = false;
			INT rootNodeCnt = 0;
			for (INT j = 0; j < obj->Modifiers.Num(); ++j)
			{
				USeqAct_Modifier* modifier = obj->Modifiers(j);
				// If a wrong root node is found in the middle
				if((modifier->bRoot && modifier->ParentNode))
				{
					errorModifier = true;
					modifier->ParentNode = NULL;
				}

				if(modifier->bRoot)
				{
					++rootNodeCnt;
				}

				USeqAct_Modifier* inputObj = 0;
				USeqAct_Modifier* trueObj = 0;
				USeqAct_Modifier* falseObj = 0;
				USeqAct_Modifier* nextObj = 0;

				if(modifier->InputLinks.Num() == 1)
				{
					inputObj = Cast<USeqAct_Modifier>(modifier->InputLinks(0).LinkedOp);
				}

				if(modifier->OutputLinks.Num() == 3)
				{
					if(modifier->OutputLinks(0).Links.Num() > 0)
					{
						trueObj = Cast<USeqAct_Modifier>(modifier->OutputLinks(0).Links(0).LinkedOp);
					}

					if(modifier->OutputLinks(1).Links.Num() > 0)
					{
						nextObj = Cast<USeqAct_Modifier>(modifier->OutputLinks(1).Links(0).LinkedOp);
					}

					if(modifier->OutputLinks(2).Links.Num() > 0)
					{
						falseObj = Cast<USeqAct_Modifier>(modifier->OutputLinks(2).Links(0).LinkedOp);
					}
				}
				
				if(trueObj != modifier->TrueRetNode)
				{
					//if(trueObj && !modifier->TrueRetNode)
					{
						modifier->TrueRetNode = trueObj;
						//errorModifier = true;
						obj->Modify();
						GWarn->MapCheck_Add(MCTYPE_WARNING, obj, *FString::Printf(TEXT("Fixed error TrueRetNode link for:%s"), *modifier->GetName()), NULL);
					}
				}

				if(nextObj != modifier->NextNode)
				{
					//if(nextObj && !modifier->NextNode)
					{
						modifier->NextNode = nextObj;
						//errorModifier = true;
						obj->Modify();
						GWarn->MapCheck_Add(MCTYPE_WARNING, obj, *FString::Printf(TEXT("Fixed error NextNode link for:%s"), *modifier->GetName()), NULL);
					}
				}

				if(falseObj != modifier->FalseRetNode)
				{
					//if(falseObj && !modifier->FalseRetNode)
					{
						modifier->FalseRetNode = falseObj;
						//errorModifier = true;
						obj->Modify();
						GWarn->MapCheck_Add(MCTYPE_WARNING, obj, *FString::Printf(TEXT("Fixed error FalseRetNode link for:%s"), *modifier->GetName()), NULL);
					}
				}
			}

			
			// Fix incorrect ParentNode LogicLink according to GraphLink
			for (INT j = 0; j < obj->Modifiers.Num(); ++j)
			{
				USeqAct_Modifier* modifier = obj->Modifiers(j);
				if((!modifier->bRoot && !modifier->ParentNode)
					|| (modifier->bRoot && modifier->ParentNode))
				{
					for (INT k = 0; k < obj->Modifiers.Num(); ++k)
					{
						USeqAct_Modifier* parentModifier = obj->Modifiers(k);
						if(parentModifier != modifier)
						{
							for (INT m = 0; m < parentModifier->OutputLinks.Num(); ++m)
							{
								if(parentModifier->OutputLinks(m).Links.Num() > 0)
								{
									USeqAct_Modifier* outModifier = Cast<USeqAct_Modifier>(parentModifier->OutputLinks(m).Links(0).LinkedOp);
									if(outModifier == modifier)
									{
										modifier->ParentNode = parentModifier;
										if(modifier->InputLinks.Num() != 0)
										{
											modifier->InputLinks(0).LinkedOp = parentModifier;
										}

										GWarn->MapCheck_Add(MCTYPE_INFO, obj, *FString::Printf(TEXT("Fixed error ParentNode:%s"), *modifier->GetName()), NULL);
										obj->Modify();
									}
								}
							} // for m
						}
					} // for k
				}
			} // for j

			// Choose the topmost one in the tree as the root node
			if(errorModifier || rootNodeCnt == 0 || rootNodeCnt > 1)
			{
				obj->Modify();

				for (INT j = 0; j < obj->Modifiers.Num(); ++j)
				{
					USeqAct_Modifier* modifier = obj->Modifiers(j);

					bool foundOutLink = false;
					for (INT k = 0; k < obj->Modifiers.Num(); ++k)
					{
						USeqAct_Modifier* testOutModifier = obj->Modifiers(k);
						for (INT m = 0; m < testOutModifier->OutputLinks.Num(); ++m)
						{
							for (INT n = 0; n < testOutModifier->OutputLinks(m).Links.Num(); ++n)
							{
								if(testOutModifier->OutputLinks(m).Links(n).LinkedOp == modifier)
								{
									foundOutLink = true;
									goto EndOutTest;
								}
							}
						}
					}

				EndOutTest:
					// Those without input are root nodes
					//if(modifier->InputLinks(0).LinkedOp == NULL)

					// Those never connected by output are root nodes
					if(!foundOutLink)
					{
						modifier->bRoot = TRUE;
						modifier->Parent = obj;
						modifier->ParentNode = NULL;
					}
					else
					{
						modifier->bRoot = FALSE;
						modifier->Parent = NULL;						
					}
				}

				GWarn->MapCheck_Add(MCTYPE_INFO, obj, *FString::Printf(TEXT("Fixed error RootNode")), NULL);
			}
		}
		else if(obj->Modifiers.Num() == 1)
		{
			USeqAct_Modifier* modifier = obj->Modifiers(0);
			if(!modifier->bRoot || modifier->Parent != obj)
			{
				modifier->Parent = obj;
				modifier->bRoot = TRUE;
				modifier->ParentNode = NULL;
				GWarn->MapCheck_Add(MCTYPE_INFO, obj, *FString::Printf(TEXT("Fixed single node as ParentNode:%s"),
					*modifier->GetName()), NULL);
				obj->Modify();
			}
		}			
	}

#if 0
	// Forgotten nodes, add to the list for now
	for (INT i = 0; i < objs.Num(); ++i)
	{
		USeqAct_Modifier* obj = objs(i);
		if(obj->GetOuter() != obj->GetOutermost())
		{
			USeqAct_Modifier* parent = Cast<USeqAct_Modifier>(obj->GetOuter());
			if(parent && (parent->Modifiers.FindItemIndex(obj) == INDEX_NONE))
			{
				obj->bRoot = FALSE;
				parent->Modifiers.AddItem(obj);
				GWarn->MapCheck_Add(MCTYPE_WARNING, obj, *FString::Printf(TEXT("Node was lost:%s add to parent:%s"),
					*obj->GetFullName(), *parent->GetFullName()), NULL);
			}
		}
	}
#endif

	// 	for (INT i = 0; i < needFix.Num(); ++i)
	// 	{
	// 		USeqAct_Modifier* obj = needFix(i);
	// 		
	// 	}

	CheckIDError(USeqAct_Modifier::StaticClass(), pkgs);

	GWarn->EndSlowTask();
	GWarn->MapCheck_ShowConditionally();

}

BYTE* GetAddr(TArray<ParentNodeInfo>& InList, UObject* InObj)
{
	BYTE* Ret = (BYTE*)InObj;
	for (INT i = 0; i < InList.Num(); ++i)
	{
		check(InList(i).ParentProperty->IsA(UArrayProperty::StaticClass()));
		UPropertyValue pv;

		InList(i).ParentProperty->GetPropertyValue(Ret + InList(i).ParentOffset, pv);
		Ret = (BYTE*)(pv.ArrayValue->GetData());
	}

	return Ret;
}

UBOOL ParseClass(UStruct* InClass, FString InParentName, TArray<ParentNodeInfo> InParentNodeList, INT InParentOffset, INT ArrayIdx, TArray<PropertyInfo>& PropertyList)
{
	 if (!InClass)
	 {
		 return FALSE;
	 }

	 for (UProperty* i = InClass->PropertyLink; i != NULL; i = i->PropertyLinkNext)
	 {
		 FString MetaString = i->GetMetaData(TEXT("ExportExcel"));
		 if (MetaString.Len() <= 0)
		 {
			 continue;
		 }
		 if (MetaString != FString(TEXT("Client")) && MetaString != FString(TEXT("Server")))
		 {
			 MetaString = FString(TEXT("Both"));
		 }

		 if (i->IsA(UStructProperty::StaticClass()) && i->GetMetaData(TEXT("Merger")).Len() == 0)
		 {
			 UStructProperty* usp = (UStructProperty*)i;
			 FString NewName = (InParentName.Len() == 0 ? i->GetName() : InParentName + FString(TEXT(".")) + i->GetName());
			 ParseClass(usp->Struct, NewName, InParentNodeList, InParentOffset + i->Offset, ArrayIdx, PropertyList);
			 continue;
		 }
		 else if (i->IsA(UArrayProperty::StaticClass()))
		 {
			 UClass* InputClass = Cast<UClass>(InClass);
			 check(InputClass);
			 UObject* DefaultObject = InputClass->GetDefaultObject();
			 check(DefaultObject);

			 UPropertyValue tpv;
			 BYTE* PA = GetAddr(InParentNodeList, DefaultObject);
			 i->GetPropertyValue(PA + InParentOffset + i->Offset, tpv);
			 INT Size = tpv.ArrayValue->Num();
			 if (Size == 0)
			 {
				 continue;
			 }

			 UArrayProperty* ap = (UArrayProperty*)i;
			 for (INT j = 0; j < Size; ++j)
			 {
				 if (ap->Inner->IsA(UStructProperty::StaticClass()))
				 {
					 UStructProperty* usp = (UStructProperty*)(ap->Inner);
					 TArray<ParentNodeInfo> NewList = InParentNodeList;
					 INT SubIdx = NewList.AddZeroed();
					 NewList(SubIdx).ParentProperty = ap;
					 NewList(SubIdx).ParentOffset = i->Offset + InParentOffset;
					 FString NewName = (InParentName.Len() == 0 ? ap->GetName() : InParentName + FString(TEXT(".")) + ap->GetName());
					 ParseClass(usp->Struct, NewName,  NewList, ap->Inner->ElementSize * j, j, PropertyList);
				 }
				 else
				 {
					 TArray<ParentNodeInfo> NewList = InParentNodeList;
					 INT SubIdx = NewList.AddZeroed();
					 NewList(SubIdx).ParentProperty = ap;
					 NewList(SubIdx).ParentOffset = i->Offset + InParentOffset;

					 INT idx = PropertyList.AddZeroed();
					 PropertyList(idx).Node = i;
					 PropertyList(idx).ParentNode = NewList;
					 PropertyList(idx).Offset = ap->Inner->ElementSize * j;
					 FString NewName = (InParentName.Len() == 0 ? ap->GetName() : InParentName + FString(TEXT(".")) + ap->GetName());
					 PropertyList(idx).OverriddenName = NewName + FString::Printf(TEXT("%d"), j + 1);
					 PropertyList(idx).UseFlag = MetaString;
				 }

			 }
		 }
		 else
		 {
			 INT idx = PropertyList.AddZeroed();
			 PropertyList(idx).ParentNode = InParentNodeList;
			 PropertyList(idx).Node = i;
			 PropertyList(idx).Offset = InParentOffset + i->Offset;
			 FString NewName = (InParentName.Len() == 0 ? i->GetName() : InParentName + FString(TEXT(".")) + i->GetName());
			 PropertyList(idx).OverriddenName = NewName;
			 if (ArrayIdx != -1)
			 {
				 PropertyList(idx).OverriddenName += FString::Printf(TEXT("%d"), ArrayIdx + 1);
			 }
			 PropertyList(idx).UseFlag = MetaString;
		 }
	 }

	 return TRUE;
}

UBOOL ParseDefaultValue(const TCHAR* InFileName,const TCHAR* InSectionName, TArray<PropertyInfo>& PropertyList)
{
	FConfigFile* File = GConfig->Find( InFileName, 0 );
	if( !File )
	{
		return FALSE;
	}
	FConfigSection* Sec = File->Find( InSectionName );
	if( !Sec )
	{
		return FALSE;
	}
	for( FConfigSection::TIterator It(*Sec); It; ++It )
	{
		FString ValueKey = It.Key().ToString();
		FString ValueString = It.Value();
		for (INT i = 0; i < PropertyList.Num(); ++i)
		{
			if (PropertyList(i).OverriddenName == ValueKey)
			{
				PropertyList(i).DefaultValue = ValueString;
			}
		}
	}
	return TRUE;
}

UBOOL ReadObject(TArray<PropertyInfo>& PropertyList, UObject* InObject, TArray<FString>& Values)
{
	if (!InObject)
	{
		return FALSE;
	}

	Values.AddItem(InObject->GetPathName());
	UPackage* objPkg = InObject->GetOutermost();
	FString pkgRoot;
	FString pkgName = objPkg->GetName();
	GPackageFileCache->FindPackageFile(*pkgName, NULL, pkgRoot);
	pkgRoot = pkgRoot.Left(pkgRoot.Len() - pkgName.Len() - 5); // + "\*.upk"
	INT rvIdx = pkgRoot.InStr(TEXT("\\"), TRUE);
	if(rvIdx != INDEX_NONE)
	{
		pkgRoot = pkgRoot.Right(pkgRoot.Len() - rvIdx - 1);
	}
	Values.AddItem(pkgRoot);


	for (INT i = 0; i < PropertyList.Num(); ++i)
	{
		UProperty* UP = PropertyList(i).Node;
		INT Offset = PropertyList(i).Offset;
		UPropertyValue UPV;
		INT idx = Values.AddZeroed();

		BYTE* BaseOffset = GetAddr(PropertyList(i).ParentNode, InObject);

		if (UP->IsA(UBoolProperty::StaticClass()))
		{
			UP->GetPropertyValue((BYTE*)BaseOffset + Offset, UPV);
			Values(idx) = FString::Printf(TEXT("%d"), UPV.BoolValue);
		}
		else if (UP->IsA(UByteProperty::StaticClass()))
		{
			UP->GetPropertyValue((BYTE*)(BaseOffset) + Offset, UPV);
			FString MetaData = UP->GetMetaData(TEXT("UseString"));
			if (MetaData.Len() > 0)
			{
				UByteProperty* ubp = (UByteProperty*)(UP);
				Values(idx) = ubp->Enum->GetEnum(UPV.ByteValue).ToString();
			}
			else
			{
				Values(idx) = FString::Printf(TEXT("%d"), UPV.ByteValue);
			}
			
		}
		else if (UP->IsA(UIntProperty::StaticClass()))
		{
			UP->GetPropertyValue((BYTE*)(BaseOffset) + Offset, UPV);
			Values(idx) = FString::Printf(TEXT("%d"), UPV.IntValue);
		}
		else if (UP->IsA(UFloatProperty::StaticClass()))
		{
			/*if(UP->GetName() == TEXT("Duration"))
			{
				UBuff* buf = Cast<UBuff>(InObject);
				if(buf->TemplateID == 2531)
				{
					appDebugBreak();
				}
			}*/

			UP->GetPropertyValue((BYTE*)(BaseOffset) + Offset, UPV);
			FString MetaData = UP->GetMetaData(TEXT("Convert"));
			if(MetaData.Len() > 0)
			{
				INT nData = appAtoi(*MetaData);
				FLOAT tmpVal = UPV.FloatValue * nData;
				INT nValue = (int)(tmpVal);
				Values(idx) = FString::Printf(TEXT("%d"), nValue);
			}
			else
			{
				Values(idx) = FString::Printf(TEXT("%f"), UPV.FloatValue);
			}
		}
		else if (UP->IsA(UNameProperty::StaticClass()))
		{
			UP->GetPropertyValue((BYTE*)(BaseOffset) + Offset, UPV);
			Values(idx) = FString::Printf(TEXT("%s"), *(UPV.NameValue->ToString()));
		}
		else if (UP->IsA(UStrProperty::StaticClass()))
		{
			UP->GetPropertyValue((BYTE*)(BaseOffset) + Offset, UPV);
			Values(idx) = *(UPV.StringValue);
		}
		else if (UP->IsA(UStructProperty::StaticClass()))
		{
			check(UP->GetMetaData(TEXT("Merger")).Len() > 0);
			INT MaskValue = 0;
			UStructProperty* usp = (UStructProperty*)UP;
			INT j = 0;
			for (UProperty* p = usp->Struct->PropertyLink; p; p = p->PropertyLinkNext)
			{
				if (p->GetMetaData(TEXT("ExportExcel")).Len() > 0 && p->IsA(UBoolProperty::StaticClass()))
				{
					p->GetPropertyValue((BYTE*)(BaseOffset) + Offset + p->Offset, UPV);
					MaskValue += UPV.BoolValue << j;
					++j;
				}
			}
			Values(idx) = FString::Printf(TEXT("%d"), MaskValue);
		}
		else if (UP->IsA(UObjectProperty::StaticClass()))
		{
			UP->GetPropertyValue((BYTE*)(BaseOffset) + Offset, UPV);
			if (UPV.ObjectValue)
			{
				TScriptInterface<IIReferenceID> ReferenceObj = ScriptInterfaceCast<IIReferenceID>(UPV.ObjectValue);
				if(ReferenceObj)
				{
					Values(idx) = FString::Printf(TEXT("%d"), ReferenceObj->GetTemplateID());			
				}
				else if (UPV.ObjectValue->IsA(USeqAct_Modifier::StaticClass()))
				{
					USeqAct_Modifier* sm = (USeqAct_Modifier*)(UPV.ObjectValue);
					Values(idx) = FString::Printf(TEXT("%d"), sm->GetId());
				}
				else
				{
					Values(idx) = UPV.ObjectValue->GetFullName();
				}
			}
			else
			{
				Values(idx) = FString(TEXT("0"));
			}
		}
	}

	return TRUE;
}

UBOOL WriteObject(TArray<PropertyInfo>& PropertyList, UObject* InObject, TArray<FString>& Headers, TArray<FString>& Values)
{
	if (Headers.Num() != Values.Num())
	{
		return FALSE;
	}

	TMap<FName, INT> SearchMap;
	for (INT i = 0; i < PropertyList.Num(); ++i)
	{
		SearchMap.Set(FName(*PropertyList(i).OverriddenName), i);
	}

	UBOOL bDirty = FALSE;

	for (INT i = 2; i < Headers.Num(); ++i)
	{
		FString& KeyString = Headers(i);
		FString ValueString = Values(i);

		const INT* Idx = SearchMap.Find(FName(*KeyString));
		if (Idx == NULL)
		{
			continue;
		}

		if (*Idx >= PropertyList.Num())
		{
			continue;
		}

		UProperty* UP = PropertyList(*Idx).Node;
		INT Offset = PropertyList(*Idx).Offset;
		UPropertyValue NPV, OPV;
		BYTE* BaseOffset = GetAddr(PropertyList(*Idx).ParentNode, InObject);

		UP->GetPropertyValue((BYTE*)BaseOffset + Offset, OPV);
		if (UP->IsA(UBoolProperty::StaticClass()))
		{
			NPV.BoolValue = appAtoi(*ValueString);
			if (OPV.BoolValue != NPV.BoolValue)
			{
				bDirty = TRUE;
				UP->SetPropertyValue((BYTE*)BaseOffset + Offset, NPV);
			}
		}
		else if (UP->IsA(UByteProperty::StaticClass()))
		{
			UByteProperty* ubp = (UByteProperty*)UP;
			FString MetaString = UP->GetMetaData(TEXT("UseString"));
			if (MetaString.Len() > 0)
			{
				NPV.ByteValue = ubp->Enum->FindEnumIndex(FName(*ValueString));
			}
			else
			{
				NPV.ByteValue = appAtoi(*ValueString);
			}
			
			if (OPV.ByteValue != NPV.ByteValue)
			{
				bDirty = TRUE;
				UP->SetPropertyValue((BYTE*)BaseOffset + Offset, NPV);
			}
		}
		else if (UP->IsA(UIntProperty::StaticClass()))
		{
			NPV.IntValue = appAtoi(*ValueString);
			if(OPV.IntValue != NPV.IntValue)
			{
				bDirty = TRUE;
				UP->SetPropertyValue((BYTE*)BaseOffset + Offset, NPV);
			}
		}
		else if (UP->IsA(UFloatProperty::StaticClass()))
		{
			NPV.FloatValue = appAtof(*ValueString);
			FString MetaData = UP->GetMetaData(TEXT("Convert"));
			if(MetaData.Len() > 0)
			{
				INT nData = appAtoi(*MetaData);
				INT OData = (int)(OPV.FloatValue * FLOAT(nData));
				if (INT(NPV.FloatValue) != OData)
				{
					bDirty = TRUE;
				}
				NPV.FloatValue = NPV.FloatValue / float(nData);
			}
			else
			{
				if (OPV.FloatValue != NPV.FloatValue)
				{
					bDirty = TRUE;
				}
			}
			if (bDirty)
			{
				UP->SetPropertyValue((BYTE*)BaseOffset + Offset, NPV);
			}
		}
		else if (UP->IsA(UNameProperty::StaticClass()))
		{
			NPV.NameValue = new FName(*ValueString);
			if (*OPV.NameValue != *NPV.NameValue)
			{
				bDirty = TRUE;
				UP->SetPropertyValue((BYTE*)BaseOffset + Offset, NPV);
			}
			delete NPV.NameValue;
		}
		else if (UP->IsA(UStrProperty::StaticClass()))
		{
			NPV.StringValue = &ValueString;
			if (*OPV.StringValue != *NPV.StringValue)
			{
				bDirty = TRUE;
				UP->SetPropertyValue((BYTE*)BaseOffset + Offset, NPV);
			}
		}
		else if (UP->IsA(UStructProperty::StaticClass()))
		{
			check(UP->GetMetaData(TEXT("Merger")).Len() > 0);
			INT MaskValue = appAtoi(*ValueString);
			UStructProperty* usp = (UStructProperty*)UP;
			INT j = 0;
			INT Mask = 1;
			for (UProperty* p = usp->Struct->PropertyLink; p; p = p->PropertyLinkNext)
			{
				if (p->GetMetaData(TEXT("ExportExcel")).Len() > 0 && p->IsA(UBoolProperty::StaticClass()))
				{
					INT CurrentMask = Mask << j;
					NPV.BoolValue = ((CurrentMask & MaskValue) > 0 ? TRUE : FALSE);
					p->GetPropertyValue((BYTE*)BaseOffset + Offset + p->Offset, OPV);
					if (OPV.BoolValue != NPV.BoolValue)
					{
						bDirty = TRUE;
						p->SetPropertyValue((BYTE*)(BaseOffset) + Offset + p->Offset, NPV);
					}
					++j;
				}
			}
		}
		else if (UP->IsA(UObjectProperty::StaticClass()))
		{
			UObjectProperty* objectProp = Cast<UObjectProperty>(UP);
			check(objectProp);

			NPV.ObjectValue = NULL;
			if(ValueString == TEXT("0"))
			{
				NPV.ObjectValue = NULL;
			}

			if(objectProp->PropertyClass->ImplementsInterface(UIReferenceID::StaticClass()))
			{
				INT templateID = appAtoi(*ValueString);
				NPV.ObjectValue = FindReferenceObjectByID(objectProp->PropertyClass, templateID);
			}
			else if (objectProp->PropertyClass == USeqAct_Modifier::StaticClass())
			{
				// do nothing
			}
			else
			{
				int idx = ValueString.InStr(TEXT(" "));
				if(idx != INDEX_NONE)
				{
					ValueString = ValueString.RightChop(idx + 1);
				}

				NPV.ObjectValue = UObject::StaticLoadObject(objectProp->PropertyClass, NULL, *ValueString, NULL, LOAD_Quiet|LOAD_NoWarn, NULL);
			}

			if(!NPV.ObjectValue)
			{
				if(ValueString.Len() > 0)
				{
					FString FinalDisplayName = objectProp->GetMetaData(TEXT("DisplayName"));
					if(FinalDisplayName.Len() > 0)
					{
						FString LocalizedName = LocalizeProperty( *(objectProp->GetFullGroupName(TRUE)), *FinalDisplayName );	
						if (LocalizedName.Len() > 0)
						{
							FinalDisplayName = LocalizedName;
						}
						else
						{
							FString LocalizedName = LocalizeProperty( *(objectProp->GetFullGroupName(TRUE)), *(objectProp->GetName()) );	
							if (LocalizedName.Len() > 0)
							{
								FinalDisplayName = LocalizedName;
							}
						}
					}

					GWarn->MapCheck_Add(MCTYPE_ERROR, InObject, *FString::Printf(LocalizeSecure(LocalizeUnrealEdMMO(TEXT("CheckError_PropObjNotFound")), *FinalDisplayName, *objectProp->PropertyClass->GetName(), *ValueString)), NULL);
				}
			}
			else
			{
				if (OPV.ObjectValue != NPV.ObjectValue)
				{
					bDirty = TRUE;
					UP->SetPropertyValue((BYTE*)(BaseOffset) + Offset, NPV);
				}
			}
		}
	}


	if (bDirty)
	{
		UObject* MostOuter = InObject->GetOutermost();
		if (MostOuter)
		{
			MostOuter->MarkPackageDirty();
		}
	}

	//InObject->MakePackageDirty();

	return TRUE;
}



void MoveSceneMaterial()
{

	GWarn->BeginSlowTask(TEXT("Moving Scene Material...."), TRUE);

	FString srcDir, destDir;
		
	srcDir = FString(TEXT("..\\..\\MMOGame\\Content\\MMOContent\\01_Scene\\"));
	destDir = FString(TEXT("..\\..\\MMOGame\\Content\\MMOContent\\01_Scene_Mat\\"));

#if 0
 	UPackage* newPkg = GEngine->CreatePackage(NULL, TEXT("oscartest"));
 	GUnrealEd->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""), 
 		TEXT("oscartest"), *(destDir + TEXT("sub1\\sub2\\sub3\\oscartest.upk"))));
	return;
#endif

	TArray<UMaterialInterface*> Materials;
	TArray<FString> pkgNames;
	appFindFilesInDirectory(pkgNames, *srcDir, TRUE, FALSE);

	debugf(TEXT("======== Begin find materials:%d pkgs"), pkgNames.Num());

	for (INT i = 0; i < pkgNames.Num(); ++i)
	{
		UPackage* Package = UObject::LoadPackage( NULL, *pkgNames(i), LOAD_Verify );
		if( !Package )
		{
			warnf( NAME_Error, TEXT("Unable to load %s"), *pkgNames(i) );
			continue;
		}

		if(!Package->IsFullyLoaded())
		{
			Package->FullyLoad();
		}

		for (INT j = 0; j < Package->GetNetObjectCount(0); ++j)
		{
			UObject* obj = Package->GetNetObjectAtIndex(j);
			if(obj && obj->IsA(UMaterialInterface::StaticClass()))
			{
				Materials.AddItem(Cast<UMaterialInterface>(obj));
				// debugf(TEXT("Added material:%s %s %s "), *obj->GetPathName(), *obj->GetName(), *obj->GetFullGroupName(FALSE));
				//goto jump;
			}
		}
	}
//jump:

	debugf(TEXT("======== Begin moving materials:%d materials"), Materials.Num());
		
	TMap<UPackage*, FString> newPkgSubDir;
	TArray<UPackage*> newPkgs;

	for (INT i = 0; i < Materials.Num(); ++i)
	{
		UMaterialInterface* mat = Materials(i);

		FString srcPkgName = mat->GetOutermost()->GetName();

		FString newPkgName = srcPkgName + TEXT("_Mat");
		debugf(TEXT("Creating new pkg:%s..."), *newPkgName);
 		UPackage* newPkg = GEngine->CreatePackage(NULL, *newPkgName);
		newPkgs.AddUniqueItem(newPkg);

		FString packagePath;
		FString relDir;

		UBOOL foundPkgPath = GPackageFileCache->FindPackageFile( *srcPkgName, NULL, packagePath );
		if(!foundPkgPath)
		{
			warnf(NAME_Error, TEXT("Package path not found:%s"), *srcPkgName);
		}
		else
		{
			FFilename fileNameInfo(packagePath);
			FString fileDir = fileNameInfo.GetPath();
			relDir = fileDir.RightChop(srcDir.Len()) + TEXT("\\");
			newPkgSubDir.Set(newPkg, relDir);
		}

		TArray<UObject*> RenameObjects;
		RenameObjects.AddUniqueItem(mat);

		warnf(NAME_Warning, TEXT("[%d/%d] Moving %s from %s to %s%s..."),
			i, Materials.Num(), *mat->GetName(), *srcPkgName, *relDir, *newPkgName);
		ObjectTools::g_defaultMoveToPackageName = newPkgName;
		UBOOL bSucceed =  ObjectTools::RenameObjectsWithRefs( RenameObjects, FALSE, NULL );
		ObjectTools::g_defaultMoveToPackageName.Empty();
		//PackageTools::SavePackages()
	}

	for (INT k = 0; k < newPkgs.Num(); ++k)
	{
		UPackage* pkg = newPkgs(k);
		FString PackageName = pkg->GetName();
		FString SaveSubDir = *newPkgSubDir.Find(pkg);
		FString SaveFileName = destDir + SaveSubDir + PackageName + TEXT(".upk");
		warnf(NAME_Warning, TEXT("[%d/%d] Saving %s..."), k, newPkgs.Num(), *SaveFileName);
		GUnrealEd->Exec( *FString::Printf(TEXT("OBJ SAVEPACKAGE PACKAGE=\"%s\" FILE=\"%s\""), *PackageName, *SaveFileName) );
	}

	GWarn->EndSlowTask();
	GWarn->MapCheck_ShowConditionally();
}

#endif // !SHIPPING_PC_GAME
