//**********************
//** npc factory class
//**********************

#ifndef MMO_NPC_FACTORY
#define MMO_NPC_FACTORY

#include "MMOSingleton.h"
#include "MMODataManager.h"

class ANPC;


class MMONpcFactory : public Singleton< MMONpcFactory >
{
protected:
	MMONpcFactory();
	virtual ~MMONpcFactory();

public:
	ANPC*	CreateNpc( INT type );
};

struct NPCTemplateDataIndex
{
	NPCTemplateDataIndex() : TemplateID(0) {}
	FString ObjectPath;
	FString DisplayName;
	INT TemplateID;
};


struct NPCTemplateIndexDataProcessor
{
	static const TCHAR* GetName()
	{
		return TEXT("npc.txt");
	}

	static INT ParseID(const FString& idStr)
	{
		return appAtoi(*idStr);
	}

	static NPCTemplateDataIndex* CreateDataObject(INT templateID, bool& needAddToMap)
	{
		needAddToMap = true;
		NPCTemplateDataIndex* item = new NPCTemplateDataIndex;
		item->TemplateID = templateID;
		return item;
	}

	static void DestroyDataObject(NPCTemplateDataIndex* obj)
	{
		delete obj;
	}

	static void ParseData(NPCTemplateDataIndex* obj, class CTabFile* dataRow, int rowIdx);
		
	static FString GetTemplateIDName()
	{
		return TEXT("TemplateID");
	}

	static FString FormatAsString(NPCTemplateDataIndex* obj)
	{
		return obj->ObjectPath;
	}

	static void PostInitData();

#if WITH_EDITOR
	static void CollectReferencedResourceFromDataTable(NPCTemplateDataIndex* obj, TSet<FString>& outResList)
	{
		UObject* dat = UObject::StaticLoadObject(UNPCTemplate::StaticClass(), NULL, *obj->ObjectPath, NULL, LOAD_NoWarn | LOAD_Quiet, NULL);
		_CollectReferencedResource(dat, outResList);

		UNPCTemplate* npcTemp = Cast<UNPCTemplate>(dat);
		if(npcTemp && npcTemp->AvatarTemplate)
		{
			_CollectReferencedResource(npcTemp->AvatarTemplate->LoadPhysicsAsset(), outResList);
		}
	}
#endif
};

class NPCTemplateDataIndexManager
	: public TDataManger< NPCTemplateDataIndex, NPCTemplateIndexDataProcessor >
{
public:

	NPCTemplateDataIndexManager() {}
	virtual ~NPCTemplateDataIndexManager() {}
};



#endif // MMO_NPC_FACTORY