#include "StdAfx.h"
#include "MMOFreeTipsUIManager.h"
#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "tabfile.h"

using namespace std;

MMOFreeTipsUIManager::MMOFreeTipsUIManager(void)
{


}
MMOFreeTipsUIManager::~MMOFreeTipsUIManager()
{
	DataMap::iterator iterData = m_DataMap.begin();
	for(;iterData!=m_DataMap.end();iterData++)
	{
		DataList *pList = iterData->second;
		if(pList){
			for(INT i=0; i<pList->Num(); i++)
			{
				FreeTipsData *pTipsData = (*pList)(i);
				if(pTipsData){
					delete pTipsData;
				}
			}
			pList->Empty();
			delete pList;
		}
	}
}

INT	  MMOFreeTipsUIManager::GetColumnIndex(FString ColumnName, TArray<FString>& ColumnList)
{
	for(INT i=0;i<ColumnList.Num();i++)
	{
		if(ColumnName == ColumnList(i))
		{
			return i;
		}
	}
	return -1;
}

DataList* MMOFreeTipsUIManager::GetTipsDataList(FString MovieName,FString PanelName, UBOOL bCreateList)
{
	DataMap::iterator iterData = m_DataMap.find( MovieName+PanelName );
	if(iterData!=m_DataMap.end() )
	{
		return iterData->second;
	}
	else if(bCreateList)
	{
		DataList* pDataList = new DataList();
		if(pDataList){
			m_DataMap.insert(std::make_pair(MovieName+PanelName, pDataList) );
			return pDataList;
		}
	}
	return NULL;
}

FreeTipsData*	MMOFreeTipsUIManager::GetTipsData(DataList* pDataList,FString Element1Name,FString Element2Name,FString Element3Name,FString Element4Name)
{
	if(pDataList)
	{
		FString keyName = Element1Name+Element2Name+Element3Name+Element4Name;
		for(int i=0;i<pDataList->Num();i++)
		{
			FreeTipsData* pFreeTipsData = (*pDataList)(i);
			if(pFreeTipsData && pFreeTipsData->KeyName == keyName )
			{
				return pFreeTipsData;
			}
		}
	}
	return NULL;
}

FreeTipsData*	MMOFreeTipsUIManager::GetTipsData(FString MovieName,FString PanelName,FString Element1Name,FString Element2Name,FString Element3Name,FString Element4Name, UBOOL bCreateList)
{
	DataList* pDataList = GetTipsDataList(MovieName,PanelName,bCreateList);
	return GetTipsData(pDataList, Element1Name, Element2Name, Element3Name, Element4Name);
}


UBOOL MMOFreeTipsUIManager::ReadFreeTipsConfig ()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("Facade::ReadFreeTipsConfig"), 0);

	//TArray<TArray<FString>> ArrayString2D;
	CTabFile* tabFile = NULL;
	if(!MMOFacade::getSingleton().ReadConfigString( tabFile, TEXT("FreeTips.txt"), TRUE ) )
	{
		return FALSE;
	}

//#if 0 // TABFILE
	INT num = 0;
	INT col_MovieName	 =  tabFile->GetKeyColIdx(TEXT("MovieName")	);	num++;
	INT col_PanelName	 =  tabFile->GetKeyColIdx(TEXT("PanelName")	);	num++;
	INT col_Element1Name =  tabFile->GetKeyColIdx(TEXT("Element1Name")	);	num++;
	INT col_Element2Name =  tabFile->GetKeyColIdx(TEXT("Element2Name")	);	num++;
	INT col_Element3Name =  tabFile->GetKeyColIdx(TEXT("Element3Name")	);	num++;
	INT col_Element4Name =  tabFile->GetKeyColIdx(TEXT("Element4Name")	);	num++;
	INT col_TipsScript	 =  tabFile->GetKeyColIdx(TEXT("TipsScript")	);	num++;
	INT col_TipsType	 =  tabFile->GetKeyColIdx(TEXT("TipsType")		);	num++;
	
	for(int i=1;i<tabFile->GetRowCount();i++)
	{
		FreeTipsData *pTipsData = new FreeTipsData();
		if(pTipsData){
			pTipsData->MovieName	= col_MovieName>=0		? tabFile->GetData(col_MovieName,i)		: TEXT("");
			pTipsData->PanelName	= col_PanelName>=0		? tabFile->GetData(col_PanelName,i)		: TEXT(""); 
			pTipsData->Element1Name = col_Element1Name>=0	? tabFile->GetData(col_Element1Name,i)	: TEXT(""); 
			pTipsData->Element2Name = col_Element2Name>=0	? tabFile->GetData(col_Element2Name,i)	: TEXT("");  
			pTipsData->Element3Name = col_Element3Name>=0	? tabFile->GetData(col_Element3Name,i)	: TEXT("");   
			pTipsData->Element4Name = col_Element4Name>=0	? tabFile->GetData(col_Element4Name,i)	: TEXT(""); 
			pTipsData->TipsScript	= col_TipsScript>=0		? tabFile->GetData(col_TipsScript,i)	: TEXT(""); 
			pTipsData->TipsType		= col_TipsType>=0		? tabFile->GetData(col_TipsType,i)		: TEXT(""); 

			AddTipsData(pTipsData);
		}
	}
//#endif
	delete tabFile;
	return TRUE;
}


void MMOFreeTipsUIManager::AddTipsListenerByName(UMMOUIBase* pPanel)
{
	if(pPanel==NULL)return;

	FName PlayerName = MMOUI::FindMoviePlayerName(pPanel->FriendlyName);
	FString keyName1  = PlayerName.GetNameString();
	FString keyName2  = pPanel->FriendlyName.GetNameString();


	DataMap::iterator iterFind = m_DataMap.find(keyName1+keyName2);
	if(iterFind!=m_DataMap.end() )
	{
		DataList *pList = iterFind->second;
		if(pList){
			for(INT i=0; i<pList->Num(); i++)
			{
				FreeTipsData *pTipsData = (*pList)(i);
				CallUCSetFreeTips(pPanel,pTipsData);
			}
		}
	}
}

void MMOFreeTipsUIManager::CallUCSetFreeTips(UMMOUIBase* pPanel, FreeTipsData* pTipsData)
{
	if(pTipsData){
		CallUCSetFreeTips(pPanel,pTipsData->Element1Name,pTipsData->Element2Name,pTipsData->Element3Name,pTipsData->Element4Name,pTipsData->TipsScript,pTipsData->TipsType);
	}
}

void MMOFreeTipsUIManager::CallUCSetFreeTips(UMMOUIBase* pPanel, FString Elem1,FString Elem2,FString Elem3,FString Elem4,FString TipsScript,FString TipsType)
{
	if(pPanel){
		TArray<struct FASValue> Arguments;

		Arguments.AddZeroed( 6 ); //number of arguments
		Arguments(0).S = Elem1;
		Arguments(0).Type = AS_String;

		Arguments(1).S = Elem2;
		Arguments(1).Type = AS_String;

		Arguments(2).S = Elem3;
		Arguments(2).Type = AS_String;

		Arguments(3).S = Elem4;
		Arguments(3).Type = AS_String;

		Arguments(4).S = TipsScript;
		Arguments(4).Type = AS_String;

		Arguments(5).S = TipsType;
		Arguments(5).Type = AS_String;

		pPanel->Invoke( TEXT("UCSetFreeTips") , Arguments );
	}
}


UBOOL	MMOFreeTipsUIManager::AddTipsData(FreeTipsData* pFreeTipsData)
{
	if(pFreeTipsData==NULL)return FALSE;

	DataList* pDataList = GetTipsDataList(pFreeTipsData->MovieName,pFreeTipsData->PanelName, TRUE);
	if(pDataList==NULL){
		delete pFreeTipsData;
		return FALSE;
	}

	FreeTipsData* pFindFreeTipsData = GetTipsData(pDataList,pFreeTipsData->Element1Name,pFreeTipsData->Element2Name,pFreeTipsData->Element3Name,pFreeTipsData->Element4Name);
	if(pFindFreeTipsData)
	{
		pFindFreeTipsData->TipsScript	= pFreeTipsData->TipsScript;
		pFindFreeTipsData->TipsType		= pFreeTipsData->TipsType;
		delete pFreeTipsData;
	}
	else
	{
		pDataList->AddItem(pFreeTipsData);
	}
	return TRUE;

}