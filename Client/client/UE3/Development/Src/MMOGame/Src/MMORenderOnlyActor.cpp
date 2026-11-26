/**
* MMORenderOnlyActor.cpp
* a rendering-only clone of MMOActors
*/

#include "MMOPrerequisites.h"
#include "MMOGameClasses.h"

IMPLEMENT_CLASS(AMMORenderOnlyActor)

// todo: iterate through all equipments to fetch texture layer information
void AMMORenderOnlyActor::Clone( AMMOActor* baseActor )
{
	if( baseActor == NULL || baseActor->Mesh == NULL )
		return;

	// create main mesh component if not exist yet
	if( MainMesh == NULL )
	{
		MainMesh = Cast<USkeletalMeshComponent>( StaticConstructObject(
			USkeletalMeshComponent::StaticClass(), GetTransientPackage(), NAME_None, RF_Transient ) );
		if( MainMesh == NULL )
		{
			return;
		}
	}

	// set main mesh
	MainMesh->SetSkeletalMesh( baseActor->Mesh->SkeletalMesh );

	// Rebuild the destination even when a previous clone left a short list.
	EquipMeshList.Empty();
	EquipMeshList.AddZeroed( EBMT_None );

	// clone equip mesh list
	if( baseActor->ItemManager != NULL )
	{
		INT count = baseActor->ItemManager->CurrentMeshList.Num();
		//Check( count == EBMT_None );
		for( INT i = 0; i < count; ++i )
		{
			if( i >= EBMT_None )
				break;
			EquipMeshList(i) = baseActor->ItemManager->CurrentMeshList(i);
		}

		// merge body parts
		if( count > 0 )
			MergeEquipMeshes();

		// put on other parts
		for( INT i = EBMT_NoMerge + 1; i < EBMT_None; ++i )
		{
			TakeOn( EquipMeshList(i) );
		}
	}
}

void AMMORenderOnlyActor::AddEquipMeshByTemplate( UEquipMesh* temp, INT idx )
{
	if( idx < 0 || idx >= EBMT_None )
		return;
	if( EquipMeshList.Num() < EBMT_None )
		EquipMeshList.AddZeroed(EBMT_None - EquipMeshList.Num());

	UEquipMesh* pNewMesh = Cast<UEquipMesh>( StaticConstructObject(
			UEquipMesh::StaticClass(), GetTransientPackage(), NAME_None, RF_Transient, temp ) );

	EquipMeshList(idx) = pNewMesh;
}

void AMMORenderOnlyActor::MergeEquipMeshes()
{
}

void AMMORenderOnlyActor::TakeOn( UEquipMesh* em )
{
	//if( em == NULL || em->PrimComponent == NULL )
	//	return;

	//if( em->ResourceType == ECRT_SkeletalMesh )
	//{

	//}
	//else if( em->ResourceType == ECRT_StaticMesh )
	//{
	//}
}

void AMMORenderOnlyActor::TakeOff( UEquipMesh* em )
{
	//if( em == NULL || em->PrimComponent == NULL )
	//	return;
}