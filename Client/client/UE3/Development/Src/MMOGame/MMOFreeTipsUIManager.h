/**
 * jimZhang@sdna.com 2013.12.4
 *
 */

#ifndef __MMOFREETIPSUIMANAGER_H__
#define __MMOFREETIPSUIMANAGER_H__

#include <list>
#include <vector>
#include <map>

#include "MMOUtil.h"

//#define FORMATDATA(vName,tName,tTpye)  p->vName = TTT()

struct FreeTipsData
{
	INT		ID;
	FString MovieName;
	FString PanelName;
	FString Element1Name;
	FString Element2Name;
	FString Element3Name;
	FString Element4Name;
	FString TipsScript;
	FString TipsType;

	FString ClassesName;
	FString KeyName;

	/** Constructors */
	FreeTipsData():ID(0) 
	{
	}
	FreeTipsData(EEventParm):ID(0)
	{
		appMemzero(this, sizeof(FreeTipsData));
	}
};

typedef TArray<FreeTipsData*> DataList;
typedef std::map<FString, DataList*, ComparFString, STLAllocator< std::pair<FString, DataList*>, UE3STLAllocatorPolicy> > DataMap;


class MMOFreeTipsUIManager : public SingletonMeyers<MMOFreeTipsUIManager>
{

protected:


	DataMap  m_DataMap;

public:
	MMOFreeTipsUIManager();
	virtual ~MMOFreeTipsUIManager();
	UBOOL ReadFreeTipsConfig ();

public:
	INT	  GetColumnIndex(FString ColumnName, TArray<FString>& ColumnList);
	//add tips for a specified component of an mmouibase
	void AddTipsListenerByName(UMMOUIBase* pPanel);
	void CallUCSetFreeTips(UMMOUIBase* pPanel, FString Elem1,FString Elem2,FString Elem3,FString Elem4,FString TipsScript,FString TipsType);
	void CallUCSetFreeTips(UMMOUIBase* pPanel, FreeTipsData* pTipsData);
	//get
	DataList*		GetTipsDataList(FString MovieName,FString PanelName, UBOOL bCreateList);
	FreeTipsData*	GetTipsData(DataList* pDataList,FString Element1Name,FString Element2Name,FString Element3Name,FString Element4Name);
	FreeTipsData*	GetTipsData(FString MovieName,FString PanelName,FString Element1Name,FString Element2Name,FString Element3Name,FString Element4Name, UBOOL bCreateList);
	UBOOL			AddTipsData(FreeTipsData* pFreeTipsData);

};

#endif