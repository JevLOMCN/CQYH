
#include "MMOFacade.h"
#include "MMOGameClasses.h"
#include "LoadMapTipsManager.h"

LoadMapTipsManager::LoadMapTipsManager()
{
	nNextMapID = -1;
	vecMapTips.clear();
}
LoadMapTipsManager::~LoadMapTipsManager()
{

}

void LoadMapTipsManager::Init()
{
	vecMapTips.clear();
	LoadMapTipsDataManager::getSingleton().InitFromFile(TEXT("loadmaptips.txt"), true);
}
void LoadMapTipsManager::Shutdown()
{
	vecMapTips.clear();
}

void LoadMapTipsManager::AddTipsData(LoadMapTipsData& data)
{
	vecMapTips.push_back(data);
}

FString LoadMapTipsManager::RandomMapTips()
{
	const LoadMapTipsData* data = RandomData();
	if(data)
	{
		return data->strTips;
	}

	return TEXT("");
}

FString LoadMapTipsManager::RandomImage()
{
	const LoadMapTipsData* data = RandomData();
	if(data)
	{
		return data->Image;
	}

	return TEXT("");
}

INT LoadMapTipsManager::RandomNO(std::list<INT>&listRand) const
{
	DOUBLE totalWeight = 0.0;
	INT firstValid = -1;
	INT lastWeighted = -1;
	for (std::list<INT>::const_iterator it = listRand.begin(); it != listRand.end(); ++it)
	{
		const INT index = *it;
		if (index < 0 || (size_t)index >= vecMapTips.size())
			continue;
		if (firstValid < 0)
			firstValid = index;
		if (vecMapTips[index].nWeight > 0)
		{
			totalWeight += (DOUBLE)vecMapTips[index].nWeight;
			lastWeighted = index;
		}
	}
	if (lastWeighted < 0)
		return firstValid;

	DOUBLE remaining = (DOUBLE)appFrand() * totalWeight;
	for (std::list<INT>::const_iterator it = listRand.begin(); it != listRand.end(); ++it)
	{
		const INT index = *it;
		if (index < 0 || (size_t)index >= vecMapTips.size() || vecMapTips[index].nWeight <= 0)
			continue;
		if (remaining < (DOUBLE)vecMapTips[index].nWeight)
			return index;
		remaining -= (DOUBLE)vecMapTips[index].nWeight;
	}
	return lastWeighted;
}

void LoadMapTipsManager::SetNextMapID(INT nMapID)
{
	nNextMapID = nMapID;
}

void LoadMapTipsManager::SetNextMapName(FString mapName)
{
	FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoByName( mapName );
	SetNextMapID(mapInfo.ServerID);
}

const LoadMapTipsData* LoadMapTipsManager::RandomData() const
{
	if(vecMapTips.empty())
		return NULL;

	INT  nClassAll = -1;
	const AMMOPlayer* pkPlayer       =   MMOFacade::getSingleton().GetMainPlayer();

	if(pkPlayer == NULL || pkPlayer->ActorAttributes == NULL)
		return NULL;

	INT nLevel = pkPlayer->ActorAttributes->LevelAttributes.Level;
	INT nClass = pkPlayer->ActorAttributes->ClassType;
	std::list<INT> vecNOID;
	std::list<INT> vecDefaultNOID;
	BOOL bLevelFind = FALSE;
	BOOL bLevelDefFind = FALSE;

	VecLoadMapTipsData::const_iterator itr = vecMapTips.begin();
	for (INT nIndex = 0; itr != vecMapTips.end();++itr,++nIndex)
	{
		if(itr->nMapId == nNextMapID)
		{
			if(nLevel >= itr->nMinLevel && nLevel <= itr->nMaxLevel)
			{
				if(!bLevelFind)
				{
					vecNOID.clear();
					bLevelFind = TRUE;
				}
				if(nClass == itr->nClass || itr->nClass == nClassAll)
					vecNOID.push_back(nIndex);
			}
			else if(!bLevelFind)
			{
				if(nClass == itr->nClass || itr->nClass == nClassAll)
					vecNOID.push_back(nIndex);
			}
		}
		else if (itr->nMapId == 0/* && vecNOID.empty()*/)
		{
			if(nLevel >= itr->nMinLevel && nLevel <= itr->nMaxLevel)
			{
				if(!bLevelDefFind)
				{
					vecDefaultNOID.clear();
					bLevelDefFind = TRUE;
				}
				if(nClass == itr->nClass || itr->nClass == nClassAll)
					vecDefaultNOID.push_back(nIndex);
			}
			else if(!bLevelDefFind)
			{
				if(nClass == itr->nClass || itr->nClass == nClassAll)
					vecDefaultNOID.push_back(nIndex);
			}
		}
	}

	INT nRand = RandomNO(vecNOID);
	if(nRand >= 0)
		return &vecMapTips[nRand];

	nRand = RandomNO(vecDefaultNOID);
	if(nRand >= 0)
		return &vecMapTips[nRand];
	return NULL;
}

LoadMapTipsData* LoadMapTipsDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void LoadMapTipsDataProcessor::DestroyDataObject(LoadMapTipsData* obj)
{

}

void LoadMapTipsDataProcessor::ParseData(LoadMapTipsData* obj, class CTabFile* dataRow, int rowIdx)
{
	LoadMapTipsData data;
	data.nID							= TReadDataValue_Int<LoadMapTipsDataManager>(TEXT("NO"), dataRow, rowIdx);	
	if(data.nID == 0)
		return;
	data.nMapId							= TReadDataValue_Int<LoadMapTipsDataManager>(TEXT("mapID"), dataRow, rowIdx);
	data.nClass							= TReadDataValue_Int<LoadMapTipsDataManager>(TEXT("class"), dataRow, rowIdx);
	data.nMinLevel						= TReadDataValue_Int<LoadMapTipsDataManager>(TEXT("minLevel"), dataRow, rowIdx);
	data.nMaxLevel						= TReadDataValue_Int<LoadMapTipsDataManager>(TEXT("maxLevel"), dataRow, rowIdx);
	data.nWeight						= TReadDataValue_Int<LoadMapTipsDataManager>(TEXT("weight"), dataRow, rowIdx);
	data.strTips						= TReadDataValue_FString<LoadMapTipsDataManager>(TEXT("tips"), dataRow, rowIdx);
	data.Image						= TReadDataValue_FString<LoadMapTipsDataManager>(TEXT("Image"), dataRow, rowIdx);

	LoadMapTipsManager::getSingleton().AddTipsData(data);
}

void LoadMapTipsDataProcessor::PostInitData()
{
}
