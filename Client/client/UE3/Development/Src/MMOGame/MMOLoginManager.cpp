#include "MMOLoginManager.h"
#include "LoginBase.h"

//-------------------------ChangeMeshItemDataProcessor------------------------------
ChangeMeshItem* ChangeMeshItemDataProcessor::CreateDataObject( INT templateID, bool& needAddToMap )
{
	ChangeMeshItem* item = ChangeMeshItemList::getSingleton().FindAndModifyData(templateID);

	if(!item)
	{
		needAddToMap = true;
		item = new ChangeMeshItem;
	}
	else
	{
		needAddToMap = false;
	}

	return item;
}
void ChangeMeshItemDataProcessor::ParseData(ChangeMeshItem* obj, class CTabFile* dataRow, int rowIdx)
{
	//ChangeMeshItem* data = new ChangeMeshItem();
	obj->SubItem.MainType = TReadDataValue_FString<ChangeMeshItemList>(TEXT("MainType"), dataRow, rowIdx);
	obj->SubItem.DesplayName = TReadDataValue_FString<ChangeMeshItemList>(TEXT("DisplayName"), dataRow, rowIdx);
	obj->MainTypeValue = TReadDataValue_Int<ChangeMeshItemList>(TEXT("MainTypeValue"), dataRow, rowIdx);
	obj->SubItem.SubTypeValue = TReadDataValue_Int<ChangeMeshItemList>(TEXT("SubTypeValue"), dataRow, rowIdx);
	obj->SubItem.Class = TReadDataValue_Int<ChangeMeshItemList>(TEXT("Class"), dataRow, rowIdx);
	obj->SubItem.Sex = TReadDataValue_Int<ChangeMeshItemList>(TEXT("Sex"), dataRow, rowIdx);
	obj->SubItem.Icon = TReadDataValue_FString<ChangeMeshItemList>(TEXT("Icon"), dataRow, rowIdx);
	obj->SubItem.Mesh = TReadDataValue_FString<ChangeMeshItemList>(TEXT("Mesh"), dataRow, rowIdx);
	MMOLoginManager::getSingleton().AddToMeshItemMap(obj);
}

//-------------------------MMOLoaginManager------------------------------
MMOLoginManager::MMOLoginManager()
{
	lastQueueTime = 0;
	lastQueuePos = 0;

}


MMOLoginManager::~MMOLoginManager()
{
	// oscar: carsh at exit Reset();
}

void MMOLoginManager::Reset()
{
	ClearChangeMeshItemMap(meshItemMap_Hair);
	ClearChangeMeshItemMap(meshItemMap_HairColor);
	ClearChangeMeshItemMap(meshItemMap_Face);
	ClearChangeMeshItemMap(meshItemMap_FaceColor);
	ChangeMeshItemList::getSingleton().Clear();
}

void MMOLoginManager::ClearChangeMeshItemMap(ChangeMeshItemMap &map)
{
// 	ChangeMeshItemMap::iterator it = map.begin();
// 	for (; it != map.end(); )
// 	{
// 		ChangeMeshSubItemMap::iterator itorSub = it->second->begin();
// 		while(itorSub != it->second->end())
// 		{
// 			ChangeMeshItemVector::iterator itorItem = itorSub->second->begin();
// 			while(itorItem != itorSub->second->end())
// 			{
// 				if(*itorItem)
// 					delete *itorItem;
// 				++itorItem;
// 			}
// 
// 			delete itorSub->second;
// 			itorSub->second = NULL;
// 			itorSub++;
// 		}
// 		delete it->second;
// 		it->second = NULL;
// 		it++;
// 	}

	map.clear();
}

void MMOLoginManager::Init()
{
	Reset();
	ChangeMeshItemList::getSingleton().InitFromFile(TEXT("Apperance.txt"), true);
	lastQueueTime = 0;
	lastQueuePos = 0;
}


void MMOLoginManager::AddToMeshItemMap(ChangeMeshItem* item)
{
	switch(item->MainTypeValue)
	{
	case CMT_HAIR_MODEL:
		AddItemToMap(meshItemMap_Hair,item);
		break;
	case CMT_HAIR_COLOR:
		AddItemToMap(meshItemMap_HairColor,item);
		break;
	case CMT_FACE_MODEL:
		AddItemToMap(meshItemMap_Face,item);
		break;
	case CMT_SKIN_COLOR:
		AddItemToMap(meshItemMap_FaceColor,item);
		break;
	}	
}

void MMOLoginManager::AddItemToMap(ChangeMeshItemMap &meshItemMap, ChangeMeshItem* item)
{
	ChangeMeshItemMap::iterator itor = meshItemMap.find(item->SubItem.Class);
	if (itor != meshItemMap.end())
	{
		ChangeMeshSubItemMap::iterator subItor = itor->second->find(item->SubItem.Sex);
		if (subItor != itor->second->end())
		{
			subItor->second->push_back(&(item->SubItem));
		}
		else
		{
			ChangeMeshItemVector* itemVec = new ChangeMeshItemVector();
			itemVec->push_back(&(item->SubItem));
			itor->second->insert(make_pair(item->SubItem.Sex,itemVec));

		}
	}
	else
	{
		ChangeMeshItemVector* itemVec = new ChangeMeshItemVector();
		ChangeMeshSubItemMap* itemSubMap = new ChangeMeshSubItemMap();
		itemVec->push_back(&(item->SubItem));
		itemSubMap->insert(make_pair(item->SubItem.Sex,itemVec));
		meshItemMap.insert(make_pair(item->SubItem.Class,itemSubMap));
	}
}

ChangeMeshItemVector* MMOLoginManager::GetMeshItemListByType(ChangeMeshtype type ,INT profession, INT sex)
{
	ChangeMeshItemVector* itemVec = NULL;
	switch(type)
	{
	case CMT_HAIR_MODEL:
		itemVec = GetMeshItemListByClasAndSex(meshItemMap_Hair,profession,sex);
		break;
	case CMT_HAIR_COLOR:
		itemVec = GetMeshItemListByClasAndSex(meshItemMap_HairColor,profession,sex);
		break;
	case CMT_FACE_MODEL:
		itemVec = GetMeshItemListByClasAndSex(meshItemMap_Face,profession,sex);
		break;
	case CMT_SKIN_COLOR:
		itemVec = GetMeshItemListByClasAndSex(meshItemMap_FaceColor,profession,sex);
		break;
	}
	return itemVec;
}

ChangeMeshItemVector* MMOLoginManager::GetMeshItemListByClasAndSex(ChangeMeshItemMap& map, INT profession, INT sex)
{
	ChangeMeshItemMap::iterator itor = map.find(profession);
	if (itor != map.end())
	{
		ChangeMeshSubItemMap::iterator subItor = itor->second->find(sex);
		if (subItor != itor->second->end())
		{
			return subItor->second;
		}
	}
	return NULL;
}

UGFxObject* MMOLoginManager::CreateMeshItemListDataObject(UMMOGFXPlayer* gfxPlayer, INT profession, int sex)
{
	UGFxObject* datList = gfxPlayer->CreateArray();
	
	if(datList == NULL)
		return NULL;

	INT type = 0;
	INT nIndex = 0;

	for (int i = 0; i < CTM_ALL; i++)
	{
		if (i == CMT_SKIN_COLOR)
		{
			type++;
			continue;
		}

		UGFxObject*  dat = gfxPlayer->CreateObject(TEXT("UI_LoginWidget.AS3.AppearanceModule.GroupData_Appearance"));
		FString mainType = "";

		ChangeMeshItemVector* vec = GetMeshItemListByType((ChangeMeshtype)i,profession,sex);
		UGFxObject* InfoArray = gfxPlayer->CreateArray();

		if (vec)
		{
			ChangeMeshItemVector::iterator itemItor = vec->begin();

			while(itemItor != vec->end())
			{
				ChangeMeshSubItem* item = *itemItor;
				if ((item->Sex == LoginBase::getSingleton().CreateRoleSex || item->Sex == SMST_ALL) && LoginBase::getSingleton().CreateRoleClass == item->Class)
				{
					UGFxObject*  datItem = gfxPlayer->CreateObject(TEXT("UI_LoginWidget.AS3.AppearanceModule.ItemData_Appearance"));
					datItem->SetInt(TEXT("id"),item->SubTypeValue);
					FString cs = FString(TEXT("img://")) + GetIcon(item->Icon);
					datItem->SetString(TEXT("imageSrc"),cs);
					InfoArray->SetElementObject(item->SubTypeValue,datItem);
					mainType = item->MainType;
				}
				itemItor++;
			}
		}

		dat->SetString(TEXT("title"),mainType);
		dat->SetInt(TEXT("type"),type);
		dat->SetObject(TEXT("itemsData"),InfoArray);
		datList->SetElementObject(nIndex,dat);

		type++;
		nIndex++;
	}
	return datList;

	
}


FString MMOLoginManager::GetMeshItemTempletByType(INT type,INT profession, INT sex, INT id)
{
	FString path;
	ChangeMeshItemVector* vec = NULL;

	switch(type)
	{
	case CMT_HAIR_MODEL:
		vec = GetMeshItemListByClasAndSex(meshItemMap_Hair,profession,sex);
		break;
	case CMT_HAIR_COLOR:
		vec = GetMeshItemListByClasAndSex(meshItemMap_HairColor,profession,sex);
		break;
	case CMT_FACE_MODEL:
		vec = GetMeshItemListByClasAndSex(meshItemMap_Face,profession,sex);
		break;
	case CMT_SKIN_COLOR:
		vec = GetMeshItemListByClasAndSex(meshItemMap_FaceColor,profession,sex);
		break;
	}
	if (vec)
	{
		ChangeMeshItemVector::iterator itemItor = vec->begin();

		while(itemItor != vec->end())
		{
			ChangeMeshSubItem* item = *itemItor;
			if (item->SubTypeValue == id)
			{
				path = item->Mesh;
				break;
			}
			itemItor++;
		}
	}
	return path;
}

void MMOLoginManager::UpdateAppearance(int type, int id ,int profession, int sex)
{
	AMMOActor* player = MMOFacade::getSingleton().GetMainActor();
	if (!player)
		return;
	/*FString path = MMOLoaginManager::getSingleton().GetMeshItemTempletByType(profession,sex);*/

	switch(type)
	{
	case CMT_HAIR_MODEL:  
		{
			if(id > MAX_APPERANCE_HAIRTYPE)
				return;
			player->ItemManager->ApplyHairModel(profession,sex,id);
			player->ItemManager->ApplyHairColor(LoginBase::getSingleton().CreateRoleHairColor);
			LoginBase::getSingleton().CreateRoleHairModel = id;
		}
		break;
	case CMT_HAIR_COLOR:
		{
			player->ItemManager->ApplyHairColor(id);
			LoginBase::getSingleton().CreateRoleHairColor = id;
		}
		break;
	case CMT_FACE_MODEL:
		{
			if (id > MAX_APPERANCE_FACETYPE)
				return;
			player->ItemManager->ApplyFaceModel(profession,sex,id);
			//player->ItemManager->ApplySkinColor(LoginBase::getSingleton().CreateRoleSkinColor);
			LoginBase::getSingleton().CreateRoleFaceModel = id;
		}
		break;
	case CMT_SKIN_COLOR:
		{
			player->ItemManager->ApplySkinColor(id);
			LoginBase::getSingleton().CreateRoleSkinColor = id;
		}
		break;
	}
}

void MMOLoginManager::UpdateAllAppearance(PlayerAppearanceData data)
{
	AMMOActor* player = MMOFacade::getSingleton().GetMainActor();
	if (!player)
		return;
	if(data.HairModel <= MAX_APPERANCE_HAIRTYPE)
		player->ItemManager->ApplyHairModel(data.Class, data.Sex, data.HairModel);
	player->ItemManager->ApplyHairColor(data.HairColor);
	if(data.FaceModel <= MAX_APPERANCE_FACETYPE)
		player->ItemManager->ApplyFaceModel(data.Class, data.Sex, data.FaceModel);
	//player->ItemManager->ApplySkinColor(data.SkinColor);
}

FString MMOLoginManager::GetIcon(FString iconNmae)
{
	if (iconNmae.IsEmpty())
	{
		return TEXT("");
	}

	FString FullName;

	UTexture2D* icon = Cast<UTexture2D>(UObject::StaticFindObject(UTexture2D::StaticClass(),ANY_PACKAGE, *iconNmae, TRUE));
	if (!icon)
	{
		icon = LoadUEObject<UTexture2D>(iconNmae, TEXT("Apperance.txt"));
	}
	if( icon == NULL )
	{
		return TEXT("");
	}

	UObject* Obj = icon;

	FullName = FString( Obj->GetPureName().ToString() );

	while( Obj && Obj->GetOuter() )
	{
		FullName = FString(Obj->GetOuter()->GetPureName().ToString()) + TEXT(".") + FullName;
		Obj= Obj->GetOuter();
	}

	return FullName;

}