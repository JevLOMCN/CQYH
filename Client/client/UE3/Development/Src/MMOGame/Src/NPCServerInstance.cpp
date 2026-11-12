#include "stdafx.h"

IMPLEMENT_CLASS(ANPCServerInstance)

UBOOL ANPCServerInstance::NeedsLoadForClient() const
{
	return FALSE;
}

void ANPCServerInstance::SetServerLocation(FVector NewLoc)
{
	// zhaojun@snda
	//if (ServerLocation != NewLoc)
	//{
		// Not needed here
// #if defined(CLIENT_GRID_EXT)
// 		gServerGridWorld.OnSyncPosFromServer(this, ServerLocation, NewLoc);
// #endif
		ServerLocation = NewLoc;
	//}
}


UAvatarData* ANPCServerInstance::GetAvatarData()
{
	if(TemplateNew)
	{
		return TemplateNew->AvatarTemplate;
	}

	return 0;
}

void ANPCServerInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if(GIsEditor)
	{
		RefresAvatarInEditor();	
	}
}
//------------------------------------------------------------------------
void ANPCServerInstance::InitItems()
{
	return;

	if(GIsEditor)
	{
		ItemManager = Cast<UItemManager>(StaticConstructObject(UItemManager::StaticClass()));

		ItemManager->Init(this);
		ItemManager->UpdateOwnerData();
	}
}
//------------------------------------------------------------------------
void ANPCServerInstance::RefresAvatarInEditor()
{
	return;

	if(!ItemManager)
	{
		InitItems();
	}

#if 1
	if(TemplateNew)
	{
		ClearComponents();
		
		//Mesh = SkeletalMeshComponent;

		if(TemplateNew->AvatarTemplate)
		{
			Mesh->SetSkeletalMesh(TemplateNew->AvatarTemplate->SkeletalMesh);
			UseingAvatar = TemplateNew->AvatarTemplate;
			RefreshAvatar();
		}

		ConditionalUpdateComponents();
	}
#else
	if(Template)
	{
		ClearComponents();
		Mesh->SetSkeletalMesh(Template->Mesh->SkeletalMesh);

		if(Template->UseingAvatar)
		{
			UseingAvatar = Template->UseingAvatar;
			RefreshAvatar();
		}

		ConditionalUpdateComponents();
	}
#endif
}
//------------------------------------------------------------------------
void ANPCServerInstance::RefresAvatarPIE()
{
	RefresAvatarInEditor();
}

//------------------------------------------------------------------------
void ANPCServerInstance::SetUniqueID(INT id)
{
	ServerUniqueID = id;
}

INT ANPCServerInstance::GetUniqueID()
{
	return ServerUniqueID;
}
//song@snda.com
void ANPCServerInstance::InitUniqueID()
{
	const int MaxSize = 1024;
	bool UniqueIDs[MaxSize];
	memset(UniqueIDs, 0, sizeof(bool)*MaxSize);
	for(TObjectIterator<ANPCServerInstance> it; it; ++it)
	{
		if( (*it)->ServerUniqueID > 0 && 
			(*it)->ServerUniqueID < MaxSize/* && !(*it)->bDeleteMe && !(*it)->bPendingDelete*/)
		{
			if ( UniqueIDs[(*it)->ServerUniqueID] )
				(*it)->ServerUniqueID = -1;// to re-arrange
			else
				UniqueIDs[(*it)->ServerUniqueID] = true;
		}
	}

	int nLastIndex = 1;
	for(TObjectIterator<ANPCServerInstance> it; it; ++it)
	{
		if((*it)->ServerUniqueID <= 0)
		{
			int i = nLastIndex;
			for(; i < MaxSize; ++i)
			{
				if(!UniqueIDs[i])
				{
					(*it)->ServerUniqueID = i;
					UniqueIDs[i] = true;

					nLastIndex = i+1;

					break;
				}
			}
			if ( i >= MaxSize )
			{
				appErrorf( TEXT("Too Much ANPCServerInstance! name: %s"), *((*it)->GetName()) );
			}
		}
	}
}
//song@snda.com