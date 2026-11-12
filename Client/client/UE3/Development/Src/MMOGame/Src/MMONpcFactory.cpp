//**********************
//** npc factory class
//**********************

#include "stdafx.h"
#include "MMONpcFactory.h"
#include "MMOUtil.h"
#include "MMOQuestData.h"

template<> MMONpcFactory* Singleton<MMONpcFactory>::ms_Singleton = NULL;
//------------------------------------------------------------------------
MMONpcFactory::MMONpcFactory()
{
}
//------------------------------------------------------------------------
MMONpcFactory::~MMONpcFactory()
{
}
//------------------------------------------------------------------------
typedef std::map< INT, AActor*, M_UE3_STL_ALLOC_MAP(INT, AActor*) >	NpcTemplateMap;
//------------------------------------------------------------------------
ANPC* MMONpcFactory::CreateNpc( INT type )
{
	static NpcTemplateMap templates;
	NpcTemplateMap::iterator itr = templates.find( type );
	if( itr != templates.end() )
	{	// finds tempalte, spawn with the template
		return Cast<ANPC>( GWorld->SpawnActor(ANPC::StaticClass(), NAME_None, FVector(0,0,0),
			FRotator(0,0,0), itr->second, true ) );
	}
	else
	{	// load template and store it
		TCHAR fileName[MAX_PATH];
		switch( type )
		{
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		default:
			// not a registered type, return NULL
			return NULL;
		}
		AActor* temp = Cast<AActor>(UObject::StaticLoadObject( 
			ANPC::StaticClass(), NULL, NULL, fileName, 0, NULL ));
		templates[type] = temp;
		return Cast<ANPC>( GWorld->SpawnActor(ANPC::StaticClass(), NAME_None, FVector(0,0,0),
			FRotator(0,0,0), temp, true ) );
	}
}

void NPCTemplateIndexDataProcessor::ParseData( NPCTemplateDataIndex* obj, class CTabFile* dataRow, int rowIdx )
{
	obj->ObjectPath = TReadDataValue_FString<NPCTemplateDataIndexManager>(TEXT("Index"), dataRow, rowIdx);
	obj->DisplayName = TReadDataValue_FString<NPCTemplateDataIndexManager>(TEXT("DisplayName"), dataRow, rowIdx);
	// add by zhangrui05@snda: quests need a list of monster groups
	INT groupId = TReadDataValue_Int<NPCTemplateDataIndexManager>(TEXT("GroupID"), dataRow, rowIdx);
	INT monsterId = TReadDataValue_Int<NPCTemplateDataIndexManager>(TEXT("TemplateID"), dataRow, rowIdx);
	if (groupId > 0)
	{
		MMOQuestData::getSingleton().AddMonsterGroupData(groupId, monsterId);
	}
}

void NPCTemplateIndexDataProcessor::PostInitData()
{

}
