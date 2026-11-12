
#ifndef _MMO_MALL_MANAGER_H_
#define _MMO_MALL_MANAGER_H_

#include "MMOSingleton.h"
#include "MMOGameUIClasses.h"
#include "net/message_handler.h"

#define DIAN_QUAN_TO_YUAN_BAO_FACTOR 100

enum MallStoreGoodsType
{
	MallStoreGOODS_All = -1,
	MallStoreGOODS_RENQI,	//sort by popularity
	MallStoreGOODS_COST,	//sort by price
};

enum MallStoreGoodstoUseType
{
	MallUSINGGOODS_ALL = 0,				//show all
	MallUSINGGOODS_AVAIBLEJOB,			//show those usable by this class
};

enum MallStorePageType
{
	MallStorePageHome = 0,	//home page
	MallStorePageItem,		//items
	MallStorePageSuit,		//fashion
	MallStorePageMount,		//mounts
};
struct MALLGOODSSHOWDATA
{
	INT nItemTemplateID;

	INT nHotIndex;
	INT nShowIndex;
	FString iconShowPath;
};
//list
typedef std::list<FString,				 M_UE3_STL_ALLOC(FString) >						MallBuyInfoList;
typedef std::list<REAL_MONEY_ITEM_INFO,  M_UE3_STL_ALLOC(REAL_MONEY_ITEM_INFO) >		MallGoodsList;
typedef std::list<MALLGOODSSHOWDATA,	 M_UE3_STL_ALLOC(MALLGOODSSHOWDATA) >			MallGoodsClientDataList;	

class MallManager : public SingletonMeyers<MallManager>, public INativeGameEventHandler
{
public:

	MallManager();
	virtual ~MallManager(){}

	void							Init();
	void							Shutdown();
	virtual void					OnNativeGameEvent(struct MMOGameEvent* evt);

	INT32							GetRealMoney();
	UINT32							GetMallCrc();
	const MallGoodsList&			GetGoodsList();
	const MallGoodsClientDataList&  GetGoodsClientDataList();
	const REAL_MONEY_ITEM_INFO*		GetGoodsByID(INT nGoodsID)const;
	const MallBuyInfoList&			GetBuyInfo()const;
	const MALLGOODSSHOWDATA*		GetGoodsClientData(INT nGoodsID)const;
	void							GetGoodsByPage(std::vector<INT>& vecGoods);
	const std::vector<INT>& 		GetHotGoods();
	const std::vector<INT>&			GetShowGoods();
	void							SetGoodsPageType(MallStorePageType pageType);
	INT								GetMountIDByItemID(INT nGoodsID);
	
	void							SetPage(INT nNum);
	INT								GetPage();
	INT								GetMaxPage();
	void							SetGoodsType(INT nType);
	void							SetGoodsToMainPlayerType(INT nType);
	INT								GetGoodsType();
	INT								GetGoodsUseType();
	void							TestMallInfo();
	
	void							NotifyRealMoney(INT32 nMoney);
	void							NotifyGoodsList(const M2C_SYNC_REAL_MONEY_SHOP_DATA* pkData);
	void							NotifyBuyResult(INT nGoodsID, INT nMoney);
	void							NotifyBuyInfo(const M2C_SALE_INFO*pkData);
	void							TickUpdateInfo();

	//money log
	void							UpdateMoneyLog( SHOP_LOG* log , INT count );
	TArray<SHOP_LOG>&				GetShopLog(){ return shopLogArray; }
	void							NotifyGameStateChange();
private:
	BOOL							LoadGoodsData(const TCHAR* filePath);
	void							SortGoodsList(INT nType, INT nAdd);
	void							FilterGoodsType(INT nShowType);
	static	BOOL					CompareRenQi_ADD(const REAL_MONEY_ITEM_INFO&pkLeft,const REAL_MONEY_ITEM_INFO&pkRight);
	static	BOOL					CompareRenQi_DEC(const REAL_MONEY_ITEM_INFO&pkLeft,const REAL_MONEY_ITEM_INFO&pkRight);
	static	BOOL					CompareCost_ADD(const REAL_MONEY_ITEM_INFO&pkLeft,const REAL_MONEY_ITEM_INFO&pkRight);
	static	BOOL					CompareCost_DEC(const REAL_MONEY_ITEM_INFO&pkLeft,const REAL_MONEY_ITEM_INFO&pkRight);

	void							ClearData();

private:
	MallBuyInfoList					mallBuyInfoList;
	MallGoodsList					mallGoodsList;
	MallGoodsClientDataList			mallClientDataList;
	std::vector<INT>				vecMallCurGoods;
	std::vector<INT>				vecMallShowGoods;
	std::vector<INT>				vecMallHotGoods;
	std::map<INT,INT>				mapMountID;
	MallStoreGoodsType				curGoodsType;
	MallStoreGoodstoUseType			curGoodsUseStateType;
	INT32							nRealMoney;
	UINT32							nCrc;
	INT								nPageNumType;
	INT								nCurPage;
	INT								nMaxPage;
	const INT						nNumPerPage;
	const INT						nGoldMoneyID;

	TArray<SHOP_LOG>				shopLogArray;
};
#endif