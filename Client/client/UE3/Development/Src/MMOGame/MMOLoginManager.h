#ifndef _LOAGIN_MANAGER_H_
#define _LOAGIN_MANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameEvent.h"
#include "MMODataManager.h"

#define MAX_EQUIPMESH_NUM 9
#define MAX_APPERANCE_HAIRTYPE 12
#define MAX_APPERANCE_FACETYPE 12

enum CreateModule
{
	CM_CREATE,
	CM_CREATE_APPERANCE,
};

enum ChangeMeshtype
{
	CMT_HAIR_MODEL,
	CMT_FACE_MODEL,
	CMT_HAIR_COLOR,
	CMT_SKIN_COLOR,
	CTM_ALL,
};

enum ChangeMeshSexType
{
	SMST_ALL,
	SMST_MALE,
	SMST_FMALE,
	
};

struct ChangeMeshSubItem
{
	FString MainType;
	FString DesplayName;
	INT Class;
	INT SubTypeValue;
	FString Icon;
	FString Mesh;
	INT Sex;
};
struct ChangeMeshItem
{
	INT MainTypeValue;
	ChangeMeshSubItem SubItem;

};

struct PlayerAppearanceData
{
	INT Class;
	INT Sex;
	UINT8 HairColor;
	UINT8 HairModel;
	UINT8 SkinColor;
	UINT8 FaceModel;
};

struct ChangeMeshItemDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("Apperance.txt");
	}


	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}


	static ChangeMeshItem* CreateDataObject(INT templateID, bool& needAddToMap);

	static void DestroyDataObject(ChangeMeshItem* obj)
	{
		if(obj)
		{
			delete obj;
		}
	}

	static void ParseData(ChangeMeshItem* obj, class CTabFile* dataRow, int rowIdx);

	static FString GetTemplateIDName()
	{
		return TEXT("Index");
	}

	static FString FormatAsString(ChangeMeshItem* obj)
	{
		return FString::Printf(TEXT("ChangeMeshItem[%s]%d"), *obj->SubItem.MainType, obj->MainTypeValue);
	}

	static void PostInitData(){}

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(ChangeMeshItem* obj, TSet<FString>& outResList)
	{
		if(obj->MainTypeValue == CMT_HAIR_MODEL)
		{
			UEquipMesh* equipMesh = LoadUEObject<UEquipMesh>(obj->SubItem.Mesh, ChangeMeshItemDataProcessor::GetName());
			if(equipMesh)
			{
				equipMesh->CollectReferencedResource(outResList);
			}
			else
			{
				warnf(NAME_Error, TEXT("Outfit-change hairstyle config not found: %s"), *obj->SubItem.Mesh);
			}
		}
		else if(obj->MainTypeValue == CMT_FACE_MODEL)
		{
			UEquipMesh* equipMesh = LoadUEObject<UEquipMesh>(obj->SubItem.Mesh, ChangeMeshItemDataProcessor::GetName());
			if(equipMesh)
			{
				equipMesh->CollectReferencedResource(outResList);
			}
			else
			{
				warnf(NAME_Error, TEXT("Outfit-change face type config not found: %s"), *obj->SubItem.Mesh);
			}
		}

		UTexture2D* icon = LoadUEObject<UTexture2D>(obj->SubItem.Icon, ChangeMeshItemDataProcessor::GetName());
		if(icon)
		{
			_CollectReferencedResource(icon, outResList);
		}
		else
		{
			warnf(NAME_Error, TEXT("Outfit-change icon texture not found: %s"), *obj->SubItem.Icon);
		}
	}
#endif
};

class ChangeMeshItemList : public TDataManger<ChangeMeshItem, ChangeMeshItemDataProcessor>
{
public:

	ChangeMeshItemList(){}
	virtual ~ChangeMeshItemList(){}
};

typedef std::vector<ChangeMeshSubItem*> ChangeMeshItemVector;
typedef std::map<INT,ChangeMeshItemVector*> ChangeMeshSubItemMap;
typedef std::map<INT,ChangeMeshSubItemMap*> ChangeMeshItemMap;


struct MMOGameEvent_Login_Queue : public MMOGameEvent
{
public:

	MMOGameEvent_Login_Queue(INT32 total, INT32 selfPos)
		: nTotal(total)
		, nSelfPos(selfPos)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_UpdateLoginQueue; }

	INT32 nTotal;
	INT32 nSelfPos;
};

struct MMOGameEvent_Login_ShowSetUp : public MMOGameEvent
{
public:

	MMOGameEvent_Login_ShowSetUp(BOOL v)
		: show(v)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_ShowLoginSetUp; }

	BOOL show;
};

struct MMOGameEvent_Login_ShowInputBox : public MMOGameEvent
{
public:

	MMOGameEvent_Login_ShowInputBox(BOOL v)
		: show(v)
	{}

	virtual EGameEvent GetEventType() const{ return GameEvent_ShowLoginInputBox; }

	BOOL show;
};

class MMOLoginManager : public SingletonMeyers<MMOLoginManager>, public INativeGameEventHandler
{
public:

	MMOLoginManager();
	virtual ~MMOLoginManager();

	void Init();
	void Reset();
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt){};

	void AddToMeshItemMap(ChangeMeshItem* item);
	void AddItemToMap(ChangeMeshItemMap& meshItemMap, ChangeMeshItem* item);

	UGFxObject* CreateMeshItemListDataObject(UMMOGFXPlayer* gfxPlayer, INT profession, int sex);
	
	FString GetMeshItemTempletByType(INT type,INT profession, INT sex, INT id);

	ChangeMeshItemVector* GetMeshItemListByType(ChangeMeshtype type ,INT profession, INT sex);
	ChangeMeshItemVector* GetMeshItemListByClasAndSex(ChangeMeshItemMap& map, INT profession, INT sex);


	void UpdateAppearance(int type, int id ,int profession, int sex);
	void UpdateAllAppearance(PlayerAppearanceData data);

	FString GetIcon(FString iconNmae);

	void ClearChangeMeshItemMap(ChangeMeshItemMap &map);

	INT GetLastQueueTime(){return lastQueueTime;}
	void SetLastQueueTime(INT time){lastQueueTime = time;}

	INT GetLastQueuePos(){return lastQueuePos;}
	void SetLastQueuePos(INT pos){lastQueuePos = pos;}

public:
	ChangeMeshItemMap meshItemMap_Hair;
	ChangeMeshItemMap meshItemMap_HairColor;
	ChangeMeshItemMap meshItemMap_Face;
	ChangeMeshItemMap meshItemMap_FaceColor;

	INT lastQueueTime;
	INT lastQueuePos;

};

#endif