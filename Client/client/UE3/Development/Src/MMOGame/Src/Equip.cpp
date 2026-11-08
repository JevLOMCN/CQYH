#include "stdafx.h"
#include "item_def.h"
#include "MMOFacade.h"
#include "MMOGameUIClasses.h"
#include "MMODataManager.h"


void UEquip::SetSilhouette( UBOOL show, FVector sc )
{
	for(INT i = 0; i < MeshList.Num(); ++i)
	{
		MeshList(i)->SetSilhouette( show, sc );
	}
}

#if WITH_EDITOR
void UEquip::CheckForErrors()
{
	Super::CheckForErrors();

	if(!EquipAppearenceTemplate)
	{
		GWarn->MapCheck_Add(MCTYPE_ERROR, this, 
			*LocalizeUnrealEdMMO(TEXT("CheckError_EquipAppearenceTemplateIsNull")),
			NULL);
	}

	if(!GetDisplayLargeIcon())
	{
		GWarn->MapCheck_Add(MCTYPE_ERROR, this, 
			*LocalizeUnrealEdMMO(TEXT("CheckError_DisplayLargeIcon")),
			NULL);
	}

	if(!GetDisplayLargeIconFemale())
	{
		GWarn->MapCheck_Add(MCTYPE_ERROR, this, 
			*LocalizeUnrealEdMMO(TEXT("CheckError_DisplayLargeIconF")),
			NULL);
	}

	
}
#endif
//------------------------------------------------------------------------
void UEquip::Init(BYTE pc, BYTE gt, UBOOL useAdvancedApperance)
{
	UEquipMesh* em = 0;

	TArray<UEquipMesh*>* AdditionalMeshList = NULL;

	//debugf( TEXT("[MMO Info] Init equip ID[%d]:%s:%s"), ItemId, *GetDisplayName(), bMale ? TEXT("Male") : TEXT("Female"));

	MeshList.Empty();

	// APPEARANCE WITH 'A' NOT 'E' ...
	UEquipAppearence* pEquipAppTemp = GetEquipAppearenceTemplate();
	if (useAdvancedApperance && GetAdvancedEquipAppearenceTemplate())
	{
		pEquipAppTemp = GetAdvancedEquipAppearenceTemplate();
	}

	if( pEquipAppTemp == NULL )
	{
		return;
	}

	switch( pc )
	{
	case EPCT_PlayerClassA:
		{
			// 战士
			if( gt == EGT_Male )
			{
				em = pEquipAppTemp->MeshList_WA_M;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_WA_M;
			}
			else if( gt == EGT_Female )
			{
				em = pEquipAppTemp->MeshList_WA_F;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_WA_F;
			}
		}
		break;
	case EPCT_PlayerClassB:
		{
			// 法师
			if( gt == EGT_Male )
			{
				em = pEquipAppTemp->MeshList_MA_M;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_MA_M;
			}
			else if( gt == EGT_Female )
			{
				em = pEquipAppTemp->MeshList_MA_F;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_MA_F;
			}
		}
		break;
	case EPCT_PlayerClassC:
		{
			if( gt == EGT_Male )
			{
				em = pEquipAppTemp->MeshList_AG_M;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_AG_M;
			}
			else if( gt == EGT_Female )
			{
				em = pEquipAppTemp->MeshList_AG_F;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_AG_F;
			}
		}
		break;
	case EPCT_PlayerClassD:
		{
			if( gt == EGT_Male )
			{
				em = pEquipAppTemp->MeshList_AR_M;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_AR_M;
			}
			else if( gt == EGT_Female )
			{
				em = pEquipAppTemp->MeshList_AR_F;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_AR_F;
			}
		}
		break;
	case EPCT_PlayerClassE:
		{
			// 道士
			if( gt == EGT_Male )
			{
				em = pEquipAppTemp->MeshList_TA_M;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_TA_M;
			}
			else if( gt == EGT_Female )
			{
				em = pEquipAppTemp->MeshList_TA_F;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_TA_F;
			}
		}
		break;
	// 1.05 backport: Lancer / Sorcerer have no MeshList_* in the 2016 EquipAppearence template yet
	case EPCT_PlayerClassG:
		{
			if( gt == EGT_Male )
			{
				em = pEquipAppTemp->MeshList_SP_M;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_SP_M;
			}
			else if( gt == EGT_Female )
			{
				em = pEquipAppTemp->MeshList_SP_F;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_SP_F;
			}
		}
		break;
	case EPCT_PlayerClassH:
		{
			if( gt == EGT_Male )
			{
				em = pEquipAppTemp->MeshList_YS_M;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_YS_M;
			}
			else if( gt == EGT_Female )
			{
				em = pEquipAppTemp->MeshList_YS_F;
				AdditionalMeshList = &pEquipAppTemp->AdditionalMeshList_YS_F;
			}
		}
		break;
	case EPCT_PlayerClassF:
		break;
	case EPCT_None:
		break;
	default:
		break;
	}

	if( em != NULL )
	{
		// 找到了职业性别对应的EM，无需继续处理
		UEquipMesh* EquipMesh = Cast<UEquipMesh>( StaticConstructObject(
			em->GetClass(), this, NAME_None, 0, em ) );
		MeshList.AddItem( EquipMesh );

		if( AdditionalMeshList )
		{
			for( INT i = 0; i < AdditionalMeshList->Num(); ++i )
			{
				em = (*AdditionalMeshList)(i);
				EquipMesh = Cast<UEquipMesh>( StaticConstructObject(
					em->GetClass(), this, NAME_None, 0, em ) );
				MeshList.AddItem( EquipMesh );
			}
		}

		return;
	}

	// 未找到对应的EM，查看通用EM列表
	TArray<UEquipMesh*>& ToSelectSet = (gt == EGT_Male ? pEquipAppTemp->MeshListTemplate_M : pEquipAppTemp->MeshListTemplate_F);
	TArray<UEquipMesh*>& ToSelectRandomSet = (gt == EGT_Male ? pEquipAppTemp->RandomMeshListTemplate_M : pEquipAppTemp->RandomMeshListTemplate_F);

	TArray<UEquipMesh*> SelectSet;
	for (INT i = 0; i < ToSelectSet.Num(); ++i)
	{
		em = ToSelectSet(i);
		if (em)
		{
			SelectSet.AddItem(em);
			for (INT j = 0; j < ToSelectRandomSet.Num(); ++j)
			{
				if (ToSelectRandomSet(j) && ToSelectRandomSet(j)->MeshType == em->MeshType)
				{
					SelectSet.AddItem(ToSelectRandomSet(j));
				}
			}
			
			em = SelectSet(RandHelper(SelectSet.Num()));
			em = Cast<UEquipMesh>(StaticConstructObject(em->GetClass(), this, NAME_None, 0, em));
			MeshList.AddItem(em);
			SelectSet.Empty();
		}
	}
}
//------------------------------------------------------------------------



//void UEquip::SetVisible(UBOOL bVisible)
//{
//	//for (INT i = 0; i < MeshList.Num(); ++i)
//	//{
//	//	if (MeshList(i))
//	//	{
//	//		MeshList(i)->SetVisible(bVisible);
//	//	}
//	//}
//}


//void UEquip::AddAttachedEmitter(class AMMOEmitter* InEmitter,INT InIndex)
//{
//	//if (InIndex >= 0 && InIndex < MeshList.Num())
//	//{
//	//	if (MeshList(InIndex))
//	//	{
//	//		MeshList(InIndex)->AddAttachedEmitter(InEmitter);
//	//	}
//
//	//}
//}
//void UEquip::DestroyAttachedEmitters()
//{
//	//for (INT i = 0; i < MeshList.Num(); ++i)
//	//{
//	//	if (MeshList(i))
//	//	{
//	//		MeshList(i)->DestroyAttachedEmitters();
//	//	}
//	//}
//}

UBOOL UEquip::OnTakeOn(class AMMOActor* Target, UBOOL UseAdvancedAppearence)
{
	check(Target);

	UItem::OnTakeOn(Target, UseAdvancedAppearence);

	BaseActor = Target;

	if (MeshList.Num() == 0)
	{
		debugf(TEXT("[MMO Debug] Mesh list not initialized."));
		if (Target->ActorAttributes != NULL)
		{
			Init(Target->ActorAttributes->ClassType, Target->ActorAttributes->GenderType, UseAdvancedAppearence);
		}
	}

	for (INT i = 0; i < MeshList.Num(); ++i)
	{
		UEquipMesh* em = MeshList(i);

		if (em && em->OnTakeOn(Target, Target->ItemManager->UseMergedMesh))
		{
			// special case for hair... some helmet may have hair set, and the default hair is not put off..
			if( Target->ItemManager->CurrentMeshList(em->MeshType) != NULL )
			{
				Target->ItemManager->CurrentMeshList(em->MeshType)->OnTakeOff( Target, Target->ItemManager->UseMergedMesh );
			}
			//if (em->MeshType == EBMT_Weapon)
			//{
			//	if (em->SocketName == FName(TEXT("Item_WeaponLeftHand")))
			//	{
			//		Target->ItemManager->CurrentMeshList(EBMT_Weapon) = em;
			//	}
			//	else if (em->SocketName == FName(TEXT("Item_WeaponRightHand")))
			//	{
			//		Target->ItemManager->CurrentMeshList(EBMT_Weapon2) = em;
			//	}
			//	else
			//	{
			//		debugf(TEXT("[MMO Debug] Wrong weapon socket name"));
			//	}
			//}
			//else
			//{
				Target->ItemManager->CurrentMeshList(em->MeshType) = em;
			//}
		}
	}

	return TRUE;
}

UBOOL UEquip::OnTakeOff(class AMMOActor* Target)
{
	check(Target);
	UItem::OnTakeOff(Target);

	BaseActor = NULL;

	//DestroyAttachedEmitters();

	for (INT i = 0; i < MeshList.Num(); ++i)
	{
		UEquipMesh* em = MeshList(i);
		if (em && em->OnTakeOff(Target, Target->ItemManager->UseMergedMesh))
		{
			if (em->MeshType >= EBMT_None)
			{
				warnf(L"MeshType >= EBMT_None, %s", *(em->GetFullName()));
				continue;
			}
			Target->ItemManager->CurrentMeshList(em->MeshType) = NULL;
		}
	}
	return TRUE;
}

void UEquip::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	//ShowWeaponType = (EquipType == EET_Normal_MainHand);
}
//------------------------------------------------------------------------
// UBOOL UEquip::OnDrop( class ADropItemActor* Target,UBOOL bNew,FVector DropLocation,FRotator DropRotation )
// {
// 	return UItem::OnDrop(Target,bNew,DropLocation,DropRotation);
// }

//void UEquip::PlayAnim(FName Anim,UBOOL bLoop,FLOAT InRate,FLOAT InStartTime)
//{
//	for (INT i = 0; i < MeshList.Num(); ++i)
//	{
//		if (MeshList(i))
//		{
//			MeshList(i)->PlayAnim(Anim, bLoop, InRate, InStartTime);
//		}
//	}
//}

//void UEquip::PlayAnimByKey(FName Anim,UBOOL bLoop,FLOAT InRate,FLOAT InStartTime,INT InKey)
//{
//	for (INT i = 0; i < MeshList.Num(); ++i)
//	{
//		if (MeshList(i))
//		{
//			MeshList(i)->PlayAnimByKey(Anim, bLoop, InRate, InStartTime, InKey);
//		}
//	}
//}
//
//void UEquip::StopAnimByKey(INT InKey)
//{
//	for (INT i = 0; i < MeshList.Num(); ++i)
//	{
//		if (MeshList(i))
//		{
//			MeshList(i)->StopAnimByKey(InKey);
//		}
//	}
//}

//void UEquip::OnAnimEnd(class UAnimNodeSequence* SeqNode,FLOAT PlayedTime,FLOAT ExcessTime)
//{
//	for (INT i = 0; i < MeshList.Num(); ++i)
//	{
//		if (MeshList(i))
//		{
//			MeshList(i)->OnAnimEnd(SeqNode, PlayedTime, ExcessTime);
//		}
//	}
//
//}
//
//void UEquip::RefreshAnim()
//{
//	for (INT i = 0; i < MeshList.Num(); ++i)
//	{
//		if (MeshList(i))
//		{
//			MeshList(i)->RefreshAnim();
//		}
//	}
//
//}

UBOOL UEquip::HasAdditiveAttributes() const
{
	//for (INT i = 0; i < AddAttrList.Num(); ++i)
	//{
	//	if(AddAttrList(i).AddAttr1 != ECHART_Default
	//		|| AddAttrList(i).AddAttr2 != ECHART_Default)
	//	{
	//		return TRUE;
	//	}
	//}
	return FALSE;
}


FString UEquip::GetLargeIconName(bool isMale, int actorClass)
{
	FString FullName;

	UTexture2D* displayLargeIcon = isMale ? GetDisplayLargeIcon() : GetDisplayLargeIconFemale();

	if( !displayLargeIcon )
	{
		if( !GetEquipAppearenceTemplate() 
			|| (GetEquipAppearenceTemplate()
			&& (GetEquipAppearenceTemplate()->WeaponType != 0
			&& GetEquipAppearenceTemplate()->EquipType == 0)) )
		{
			return TEXT("MMO_UIIcon.testIcon.Weapon");
		}

		switch( GetEquipAppearenceTemplate()->EquipType )
		{
		case ITEMSUBTYPE_EQUIP_BELT:
			return TEXT("MMO_UIIcon.testIcon.Waist");
		case  ITEMSUBTYPE_EQUIP_CHEST_ARMOR:
			return TEXT("MMO_UIIcon.testIcon.Body");
		case  ITEMSUBTYPE_EQUIP_CLOAK:
			return TEXT("MMO_UIIcon.testIcon.Back");
		case ITEMSUBTYPE_EQUIP_HAT:
			return TEXT("MMO_UIIcon.testIcon.Head");
		case ITEMSUBTYPE_EQUIP_SHOES:
			return TEXT("MMO_UIIcon.testIcon.Foot");
		case ITEMSUBTYPE_EQUIP_SHOULDER_ARMOR:
			return TEXT("MMO_UIIcon.testIcon.Shoulder");
//		case ITEMSUBTYPE_EQUIP_PANTS:
//			return TEXT("MMO_UIIcon.testIcon.Leg");
		case ITEMSUBTYPE_EQUIP_WEAPON:
			return TEXT("MMO_UIIcon.testIcon.Weapon");
			//case ITEMSUBTYPE_EQUIP_RING:
			//case ITEMSUBTYPE_EQUIP_WRIST_ARMOR:
		}

		return GetIconName( actorClass );
	}

	UObject* Obj = displayLargeIcon;
	FullName = FString( Obj->GetPureName().ToString() );
	while( Obj && Obj->GetOuter() )
	{
		FullName = FString(Obj->GetOuter()->GetPureName().ToString()) + TEXT(".") + FullName;
		Obj= Obj->GetOuter();
	}
	FullName +=GetIconNamePostfixBy( actorClass );

	return FullName;

}

FString UEquip::GetIconName(int nState)
{
	FString equipIconName = UItem::GetIconName( 0 );
	equipIconName += GetIconNamePostfixBy( nState );

	return equipIconName;
}

FString UEquip::GetIconNamePostfixBy(int actorClass)
{
	if( IsIconSame ){
		return TEXT("");
	}

	switch( actorClass )
	{
	case EPCT_PlayerClassA: return TEXT("_Wa"); break;
	case EPCT_PlayerClassB: return TEXT("_Ma"); break;
	case EPCT_PlayerClassC: return TEXT("_As"); break;
	case EPCT_PlayerClassD: return TEXT("_Ar"); break;
	case EPCT_PlayerClassE: return TEXT("_Ta"); break;
	}

	return TEXT("_Wa");
}

UEquipAppearence* UEquip::GetEquipAppearenceTemplate()
{
	if(EquipAppearenceTemplate)
	{
		return EquipAppearenceTemplate;
	}

	STAT_AUTO_CYCLE_COUNTER((FString(TEXT("Equip:")) + EquipAppearenceTemplate_Str));

	EquipAppearenceTemplate = LoadUEObject<UEquipAppearence>(EquipAppearenceTemplate_Str, TEXT("item_armor.txt"));

	return EquipAppearenceTemplate;
}

UEquipAppearence* UEquip::GetAdvancedEquipAppearenceTemplate()
{
	if (AdvancedEquipAppearenceTemplate)
	{
		return AdvancedEquipAppearenceTemplate;
	}

	STAT_AUTO_CYCLE_COUNTER((FString(TEXT("EquipAdv:")) + AdvancedEquipAppearenceTemplate_Str));

	AdvancedEquipAppearenceTemplate = LoadUEObject<UEquipAppearence>(AdvancedEquipAppearenceTemplate_Str, TEXT("item_armor.txt"));

	return AdvancedEquipAppearenceTemplate;
}

UBOOL UEquip::IsWeapon()
{
	if(GetEquipAppearenceTemplate() != NULL)	
	{
		return GetEquipAppearenceTemplate()->EquipType == EET_Normal_MainHand;
	}

	return FALSE;
}

UBOOL UEquip::IsEquip()
{
	if(GetEquipAppearenceTemplate() != NULL)	
	{
		return GetEquipAppearenceTemplate()->EquipType != EET_Normal_MainHand;	
	}

	return FALSE;
}