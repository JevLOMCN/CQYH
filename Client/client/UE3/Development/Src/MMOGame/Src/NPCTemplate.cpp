#include "stdafx.h"
#include "MMOFacade.h"
#include "EnginePhysicsClasses.h"

IMPLEMENT_CLASS(UNPCTemplate);

UBOOL UNPCTemplate::IsNeedArchetype()
{
	return false;
}
//------------------------------------------------------------------------
void UNPCTemplate::DataToExcel(TArray<FString>& Row)
{
	
}
//------------------------------------------------------------------------
void UNPCTemplate::ExportExcelHeaders(TArray<FString>& Header,TArray<FString>& DefaultValues)
{
}
//------------------------------------------------------------------------
void UNPCTemplate::ImportFromExcel(const TArray<FString>& Values)
{
	if (AvatarTemplate)
	{
		if (AvatarTemplate->CollisionRadius >= 0.1f)
		{
			AvatarTemplate->AvatarDrawScale = Radius / AvatarTemplate->CollisionRadius;
		}
	}

}
//------------------------------------------------------------------------
int UNPCTemplate::GetTemplateID() const
{
	return TemplateID;
}
//------------------------------------------------------------------------
extern class FFeedbackContext* GWarn;
void UNPCTemplate::InitDataToExcel()
{
	if (AvatarTemplate)
	{
		Radius = AvatarTemplate->CollisionRadius * AvatarTemplate->AvatarDrawScale;
	}

	Radius = (Radius < MIN_NPC_RADIUS ? MIN_NPC_RADIUS : Radius);

	UNPCTemplate* npc = this;
	if (npc && npc->AvatarTemplate && npc->AvatarTemplate->SkeletalMesh)
	{
		FString MeshName = npc->AvatarTemplate->SkeletalMesh->GetPathName();
		FString PAName = MeshName + FString(TEXT("_Physics"));
		UPhysicsAsset* PhysicAssetData = Cast<UPhysicsAsset>(UObject::StaticFindObject(UPhysicsAsset::StaticClass(), ANY_PACKAGE, *PAName, TRUE));
		if (!PhysicAssetData)
		{
			PhysicAssetData = Cast<UPhysicsAsset>(UObject::StaticLoadObject(UPhysicsAsset::StaticClass(), NULL, *PAName, NULL, LOAD_NoWarn, NULL));
			if (!PhysicAssetData)
			{
				GWarn->MapCheck_Add(MCTYPE_ERROR, this, *FString::Printf(TEXT("Cannot find the physics asset for SkeletalMesh: %s PhysicsAsset: %s"), *MeshName, *PAName), NULL);
			}
		}
	}
}

//------------------------------------------------------------------------
FString UNPCTemplate::GetDisplayName() const
{
	return MMOUI::TrimNumber(DisplayName);
}
//---------------------------------------------------------------------------

FString UNPCTemplate::GetIconName()
{
	if(!HUDDisplayIcon)
	{
		return TEXT("");
	}

	FString FullName;

	UObject* Obj = HUDDisplayIcon;
	FullName = FString(Obj->GetPureName().ToString() );
	while( Obj && Obj->GetOuter() )
	{
		FullName = FString(Obj->GetOuter()->GetPureName().ToString()) + TEXT(".") + FullName;
		Obj= Obj->GetOuter();
	}
	//int i = 0;
	//FString str = FString( DisplayIcon->GetOutermost() ->GetOut());// ->TextureFileCacheName.ToString() );
	return FullName;
}

#if WITH_EDITOR
void UNPCTemplate::CheckForErrors()
{
	Super::CheckForErrors();

	for( TObjectIterator<UNPCTemplate> It ; It ; ++It )
	{
		UNPCTemplate* Obj1 = *It;
		if(Obj1 != this)
		{
			if(Obj1->DisplayName == DisplayName)
			{
				GWarn->MapCheck_Add(MCTYPE_ERROR, this, 
					*LocalizeUnrealEdMMO(TEXT("CheckError_DuplicateDisplayName")),
					NULL);
			}
		}
	}
}
#endif
