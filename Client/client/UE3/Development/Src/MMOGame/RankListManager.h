#ifndef __MMORANKLISTMANAGER_H__
#define __MMORANKLISTMANAGER_H__

#include "MMOSingleton.h"
#include "MMOUtil.h"
#include "ranklist_def.h"
#include "m2c_message.h"
#include "MMOFacade.h"

using namespace ExternalMessage;

#define SUBNODE												0
#define PARENTNODE_EX										1
#define PARENTNODE											2

#define MAXRANKLISTCOUNT_PERSON     30
#define MAXRANKLISTCOUNT_GUILD      10
enum RangePage
{
	RP_Page0 = 0,
	RP_Page1 = 1,
	RP_Page2 = 2,
	RP_Page3 = 3,
	RP_Page5 = 5,
	RP_Page10 = 10,
	RP_Page20 = 20,
	RP_Page30 = 30,

};

enum RangeRdoIndex
{
	RP_Rdo0 = 0,
	RP_Rdo1 = 1,
	RP_Rdo2 = 2,
	RP_Rdo3 = 3,
	RP_Rdo4 = 4,
	RP_Rdo5 = 5,
	RP_Rdo6 = 6,
};
struct MMOGameEvent_RankList : public MMOGameEvent
{
public:
	MMOGameEvent_RankList()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_RankList;
	}
};

struct MMOGameEvent_RankList_UpdateCategoryButton : public MMOGameEvent
{
public:
	MMOGameEvent_RankList_UpdateCategoryButton()
	{
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_RankList_UpdateCategoryButton;
	}
};

typedef struct tagRankListData 
{
	tagRankListData() : Id(0), Value(0), LastRank(0) {}

	INT64 Id;
	INT64 Value;
	UINT8 LastRank;
}RankListData;
typedef std::vector<RankListData*,  M_UE3_STL_ALLOC(RankListData*) > RankVec;

typedef struct tagPageInfo
{
	tagPageInfo() : curPage(0), totalPage(0) {}

	INT curPage;
	INT totalPage;
}PageInfo;
typedef std::map<RANK_LIST, PageInfo*, M_UE3_STL_ALLOC_MAP(RANK_LIST, PageInfo*)> PageMap;

class RankListManager
	:public SingletonMeyers<RankListManager>
{
public:
	RankListManager();
	virtual ~RankListManager();

	void Clear();
	void AddData(INT64 Id, INT64 value,UINT8 lastRank);
	const RankVec& GetRankVec() const;

	void SetSelfRank(INT32 selfRank){_SelfRank = selfRank;}
	const INT32 GetSelfRank() const{return _SelfRank;}
	void SetType(RANK_LIST type);
	const RANK_LIST& GetType()const{return eRankList;}
	void SetPageIndex(RANK_LIST type, INT index);
	INT GetPageIndex() const;
	void SetPageTotal(INT total);
	INT GetPageTotal() const;
	void SetTotalType(RANK_LIST type){eRankTotal = type;}
	RANK_LIST GetTotalType()const{return eRankTotal;}

	void NotifyUpdateRankList();
	void NotifyUpdateCategoryButton();
	void NotifyReFreshList();

	INT GetRangeBtnIndex(int currentPage);

	INT getMaxRankListCount();

	void SetSelfLastRank(UINT16 rank){_SelfLastRank = rank;}
	UINT16 GetSelfLastRank(){return _SelfLastRank;}

	INT GetMaxLevel(){return maxLevel;}
	void SetMaxLevel(INT nLevel){maxLevel = nLevel;}

	BOOL IsShowRank(){return bShowRank;}
	void SetIsShowRank(BOOL v){bShowRank  = v;}
	void SetShowRank(UINT32 time);

	INT getHeroRankMaxTime(){return heroRankMaxTime;}

	INT GetMyHeroNum(){return myHeroNum;}
	void SetMyHeroNum(INT num) { myHeroNum = num;}

	tm GetShowRankTime(){return showRankTime;}
private:
	INT32	_SelfRank;
	RankVec _RankVec;
	RANK_LIST eRankList;
	RANK_LIST eRankTotal;
	PageMap _PageMap;
	UINT16 _SelfLastRank;

	INT maxLevel;

	BOOL bShowRank;
	INT heroRankMaxTime;
	INT myHeroNum;

	tm showRankTime;
};


#endif