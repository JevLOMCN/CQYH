
#ifndef _MMO_HEAVEN_MANAGER__H_
#define _MMO_HEAVEN_MANAGER__H_

#include "MMOSingleton.h"
#include "MMODataManager.h"
#include "common.h"
#include "role_db_def.h"
#include "spirit_def.h"

	//a qi item
	struct GasItem
	{
		INT					nUniqueID;		//unique ID used by the client
		INT					nSlotIndex;		//position index
		INT					nContainerID;	//ID of the containing container
		INT					nTempID;		//associated qi template
		INT					nCurExp;		//current experience
		GasItem()
		{
			appMemzero(this, sizeof(GasItem));
		}
	};

	//qi slot
	struct GasSlot
	{
		INT					 nUniqueID;		//associated qi unique ID
		INT					 nLocked;		//locked
		INT					 nDisable;		//not unlocked, unavailable
		INT					 nTagID;
		GasSlot()
		{
			nUniqueID = 0;
			nLocked = 0;
			nDisable = 0;
			nTagID   = -1;
		}
	};

	//blood slot
	struct BloodSlot
	{
		INT						 nPage;
		INT						 nIndex;
		INT						 nValue;
		INT						 nState;
		FString					 sName;
		BloodSlot()
		{
			appMemzero(this, sizeof(BloodSlot));
		}
	};
	//-----------------------------config------------------
	//realm level attribute config
	struct HeavenLevelConfig
	{
		INT						 nLevel;			//current realm level
		INT						 nAddExp;			//how much experience to add
		INT						 nAddHeavenPts;		//how many cultivation points to add
		INT						 nSoulCost;			//how much soul is consumed
		INT						 nNeedTime;			//time needed for conversion
		INT						 VIPOfflineDiscount1;	//Vip offline conversion discount
		INT						 VIPOfflineDiscount2;	//Vip offline conversion discount
		INT						 VIPOfflineDiscount3;	//Vip offline conversion discount

		INT						 VipOnlineChangeDiscount1;	//Vip online conversion discount
		INT						 VipOnlineChangeDiscount2;	//Vip online conversion discount
		INT						 VipOnlineChangeDiscount3;	//Vip online conversion discount
		INT						 nNeedMoney;		//money needed to buy a conversion
		INT						 nMaxExp;			//experience for the next level
		INT						 nExp2PtsRate;		//conversion efficiency
		INT						 nAtriNodeID1;		//attribute
		INT						 nAtriNodeID2;
		INT						 nDailyExp2PtsMaxCount;//max conversions per day
		HeavenLevelConfig()
		{
			appMemzero(this, sizeof(HeavenLevelConfig));
		}
	};

	//qi template attribute config
	struct HeavenGasItemConfig
	{
		INT						nID;
		INT						nLevel;				//qi level
		INT						nQuality;			//quality
		INT						nExp;				//experience
		INT						nNextLevelID;		//next level ID
		INT						nAtriNodeID1;
		INT						nAtriNodeID2;
		INT						nExchangeCost;		//amount of Wanfa qi needed to exchange for qi
		INT						nGroupID;
		INT						nNextLevelExp;
		INT						nJob;
		INT						nCanExchange;
		FString					name;
		FString					Icon0;
		FString					Icon1;
		HeavenGasItemConfig()
		{
			appMemzero(this, sizeof(HeavenGasItemConfig));
		}
	};

	//blood template attribute config
	struct HeavenBloodConfig
	{
		INT						nPage;				//bloodline type
		INT						nSlot;				//slot
		INT						nLevel;				//level 0-10
		INT						nHeavenPtsCost;		//cultivation points consumed
		INT						nRate;				//success rate
		INT						nLevelLimit;		//level limit
		INT						nPageLevelLimit;	//realm level limit of the page
		INT						nAttValue1;
		INT						nAttValue2;
		HeavenBloodConfig()
		{
			appMemzero(this, sizeof(HeavenBloodConfig));
		}
	};
	//------------------------------------------------------------------

	struct HeavenTitleTool
	{
		INT						GetStartNum(INT nLevel);
		INT						GetLevelTitleIndex(INT nLevel);
		INT						GetStarTitleIndex(INT nLevel);
		FString					GetLevelTitle(INT nLevel);
		FString					GetStarTitle(INT nLevel);
		FString					GetLevel(INT nLevel);
		FString					_IndexGetLevelTitle(INT nIndex);
		FString					_IndexGetStarTitle(INT nIndex);
		HeavenTitleTool();
	private:
		TArray<FString>			levelTitle;
		TArray<FString>			starTitle;
	};

	struct SlotOpenTaxData
	{
		INT nSlotID;
		INT	nHeavenLevel;
		INT nMoney;
		SlotOpenTaxData()
		{
			appMemzero(this,sizeof(SlotOpenTaxData));
		}
	};
	enum HeavenConvertType
	{
		HeavenConvertType_OnLineConvert = 0,
		HeavenConvertType_JustConvert,
		HeavenConvertType_OutLineConvert,
	};
	enum HeavenGasOperateType
	{
		HeavenGasOperateType_Invalid = 0,
		HeavenGasOperateType_Practise,
		HeavenGasOperateType_Merge,
		HeavenGasOperateType_Exchange,
	};
	//realm event notification
	struct MMOGameEvent_Heaven : public MMOGameEvent
	{
	public:
		enum EHeavenEventType
		{
			EHeavenEvent_SynData,
			EHeavenEvent_UpdateGasItem,
			EHeavenEvent_UpdateGasSlot,
			EHeavenEvent_UpdateLevel,
			EHeavenEvent_UpdateExp,
			EHeavenEvent_PractisePhrase,
			EHeavenEvent_BloodStateInit,
			EHeavenEvent_TargetSlotState,
			EHeavenEvent_LogInfo,
			EHeavenEvent_UpdateNeedTime,
			EHeavenEvent_BloodStateUpdate,
			EHeavenEvent_BloodStateOpen,
			EHeavenEvent_PractiseAnimation,
			EHeavenEvent_MergeAnimation,
		};

		MMOGameEvent_Heaven(EHeavenEventType eventType)
			: EventType(eventType)
		{
		}

		virtual EGameEvent GetEventType() const
		{
			return GameEvent_Heaven;
		}

		EHeavenEventType	EventType;
		INT					SlotID;
		INT					SlotContainerID;
		INT					nValue;
	};


	struct HeavenLevelDataProcessor
	{
		static const TCHAR* GetName()
		{
			return TEXT("spirit.txt");
		}

		static INT ParseID(const FString& idStr)
		{
			return appAtoi(*idStr);
		}

		static HeavenLevelConfig* CreateDataObject(INT templateID, bool& needAddToMap);

		static void DestroyDataObject(HeavenLevelConfig* obj);

		static void ParseData(HeavenLevelConfig* obj, class CTabFile* dataRow, int rowIdx);

		static FString GetTemplateIDName()
		{
			return TEXT("Level");
		}
		
		static FString FormatAsString(HeavenLevelConfig* obj)
		{
			return "HeavenLevelConfig";
		}

		static void PostInitData();

#if WITH_EDITOR
		static void CollectReferencedResourceFromDataTable(HeavenLevelConfig* obj, TSet<FString>& outResList)
		{
			//_CollectReferencedResource(obj, outResList);
		}
#endif
	};

	typedef TDataManger< HeavenLevelConfig, HeavenLevelDataProcessor > MMOHeavenLevelDataManager;

	struct HeavenGasItemDataProcessor
	{
		static const TCHAR* GetName()
		{
			return TEXT("air.txt");
		}

		static INT ParseID(const FString& idStr)
		{
			return appAtoi(*idStr);
		}

		static HeavenGasItemConfig* CreateDataObject(INT templateID, bool& needAddToMap);

		static void DestroyDataObject(HeavenGasItemConfig* obj);

		static void ParseData(HeavenGasItemConfig* obj, class CTabFile* dataRow, int rowIdx);

		static FString GetTemplateIDName()
		{
			return TEXT("ID");
		}

		static FString FormatAsString(HeavenGasItemConfig* obj)
		{
			return obj->name;
		}

		static void PostInitData();

#if WITH_EDITOR
		static void CollectReferencedResourceFromDataTable(HeavenGasItemConfig* obj, TSet<FString>& outResList)
		{
			//_CollectReferencedResource(obj, outResList);
		}
#endif
	};

	typedef TDataManger< HeavenGasItemConfig, HeavenGasItemDataProcessor >   MMOHeavenGasItemDataManager;

	struct HeavenBloodDataProcessor
	{
		static const TCHAR* GetName()
		{
			return TEXT("blood.txt");
		}

		static INT ParseID(const FString& idStr)
		{
			return appAtoi(*idStr);
		}

		static HeavenBloodConfig* CreateDataObject(INT templateID, bool& needAddToMap);

		static void DestroyDataObject(HeavenBloodConfig* obj);

		static void ParseData(HeavenBloodConfig* obj, class CTabFile* dataRow, int rowIdx);

		static FString GetTemplateIDName()
		{
			return TEXT("Page");
		}

		static FString FormatAsString(HeavenBloodConfig* obj)
		{
			return "HeavenBloodConfig";
		}

		static void PostInitData();

#if WITH_EDITOR
		static void CollectReferencedResourceFromDataTable(HeavenBloodConfig* obj, TSet<FString>& outResList)
		{
			//_CollectReferencedResource(obj, outResList);
		}
#endif
	};

	typedef TDataManger< HeavenBloodConfig, HeavenBloodDataProcessor > MMOHeavenBloodDataManager;

	struct HeavenJustConvertData
	{
		INT nID;
		INT nTimesCount;
		INT nCostDiscount;
	};
	struct HeavenJustConvertProcessor
	{
		static const TCHAR* GetName()
		{
			return TEXT("spirit_change.txt");
		}

		static INT ParseID(const FString& idStr)
		{
			return appAtoi(*idStr);
		}

		static HeavenJustConvertData* CreateDataObject(INT templateID, bool& needAddToMap);

		static void DestroyDataObject(HeavenJustConvertData* obj);

		static void ParseData(HeavenJustConvertData* obj, class CTabFile* dataRow, int rowIdx);

		static FString GetTemplateIDName()
		{
			return TEXT("Index");
		}

		static FString FormatAsString(HeavenJustConvertData* obj)
		{
			return "HeavenJustConvertData";
		}

		static void PostInitData();

#if WITH_EDITOR
		static void CollectReferencedResourceFromDataTable(HeavenJustConvertData* obj, TSet<FString>& outResList)
		{
			//_CollectReferencedResource(obj, outResList);
		}
#endif
	};

	typedef TDataManger< HeavenJustConvertData, HeavenJustConvertProcessor > MMOHeavenJustConvertManager;

	//list
	typedef std::list<HeavenBloodConfig,	M_UE3_STL_ALLOC(HeavenBloodConfig) >		HeavenBloodDataList;
	typedef std::list<GasItem,				M_UE3_STL_ALLOC(GasItem) >					GasItemList;
	typedef std::list<HeavenJustConvertData,M_UE3_STL_ALLOC(HeavenJustConvertData) >	HeavenConvertList;
	typedef std::list<INT,					M_UE3_STL_ALLOC(INT) >						HeavenAttList;
	typedef std::map<INT, HeavenGasItemConfig*, M_UE3_STL_ALLOC_MAP(INT, HeavenGasItemConfig*) >	HeavenGasTemplateMap;
	typedef std::map<INT, HeavenLevelConfig*, M_UE3_STL_ALLOC_MAP(INT, HeavenLevelConfig*) >		HeavenLevelDataMap;

	//realm manager class
	class MMOHeavenManager : public SingletonMeyers<MMOHeavenManager>, public INativeGameEventHandler
	{
	public:

		MMOHeavenManager();
		virtual ~MMOHeavenManager(){}

		void							Init();
		void							Shutdown();
		void							Clear();
	
		virtual void					OnNativeGameEvent(struct MMOGameEvent* evt);
		void							Update(FLOAT deltaTime);
		const HeavenGasTemplateMap&     GetGasItemDataMap();
		HeavenAttList&					GetHeavenAttributeList();
		const TArray<FString>&			GetConverterLog();
		const GasItem*					GetGasItemBySlot(INT nContainerID, INT nSlotIndex);
		const HeavenGasItemConfig*		GetGasTemplate(INT nTempID);
		const HeavenLevelConfig*		GetHeavenCurLevelData();
		const HeavenLevelConfig*		GetHeavenLevelData(INT nLevel);
		const HeavenBloodConfig*		GetHeavenBloodData(INT nPage,INT nSlot,INT nLevel);
		const GasSlot*					GetTargetSlot()const;
		const BloodSlot*				GetBloodSlot(INT nPage,INT nIndex)const;
		INT								GetStarNum(INT nLevel);
		INT								GetStarTitleIndex(INT nLevel);
		INT								GetLevelTitleIndex(INT nLevel);
		FString							GetLevelTitle(INT nLevel);
		FString							GetLevelString(INT nLevel);
		FString							GetStarTitle(INT nLevel);
		void							GetHeavenTips(FString& tips,INT nLevel);
		void							GetHeavenGasItemTips(FString& tips,INT nContainerID,INT nSlotID);
		void							GetHeavenBloodTips(FString&tips,INT nPage,INT nSlot);
		void							GetHeavenGasTipsByGasID(FString& tips,INT nGasID,INT nCurExp = -1);
		INT								GetEquipContainerSize(){return arrSlotGasEquip.Num();};
		INT								GetBagContainerSize(){return arrSlotGasBag.Num();};
		INT								GetBankContainerSize(){return arrSlotGasBank.Num();};
		void							ChangeTargetGasItem(INT nSrcSlot);
		INT 							GetNeedTime();
		UINT32							GetTodayConvertTimes();
		UINT32							GetTodayConvertMaxTimes();
		INT								GetNeedMoneyOpenSlot(INT nContainerID, INT nSlotIndex);
		INT								GetNeedMoneyConvert();
		void							ClearTargetSlot();
		INT								GetBtnPractiseState(INT nIndex);
		void							GetEquipGasPowerDescrible(FString& strPower);
		const SlotOpenTaxData*			GetSlotEquipTax(INT nIndex) const;
		INT								GetEquipGasPower();
		BOOL							GetBloodLockState(INT nPage);
		INT								GetGasPowerByID(INT nGasID);
		INT								GetCanEquipSlotID();
		INT								GetNextLevelExp();
		INT                             GetNextLevelPts();
		//server notify
		void							NotifyMoneyChange(INT nMoneyType,INT nMoney,INT nOldMoney);
		void							NotifyLevelChange(INT nLevel);
		void							NotifyUpdateHeavenData(const ROLE_DB_SPIRIT_DATA* pkData);
		void							NotifyUpdateHeavenExp(INT nExp);
		void							NotifyUpdateHeavenLevel(INT nLevel);
		void							NotifyUpdatePractiseBtnIndex(INT nIndex);
		void							NotifyUpdateGasChange(INT nExp,INT nGasID,INT nContainerID,INT nSlotIndex);
		void							NotifyOpenGasSlot(INT nContainerID,INT nSlotIndex);
		void							NotifyUpdateBloodState(INT nPage,INT nIndex, INT nNum);
		void							ParseHeavenError(INT32 errorId, INT32 param1, INT32 param2, FString& errorStr);
		void							NotifyGameStateChange();
		void							NotifyBloodOpen(INT nPage);

		//client send message prediction
		BOOL							CheckGasBagFull();
		BOOL							CheckSoultoConvert();
		BOOL							CheckMoneytoOpenSlot(INT nContainerID, INT nSlotID);
		BOOL							CheckXiuLiantoPractise(INT nIndex);
		BOOL							CheckGasMoneytoExchange(INT nGasID);
		BOOL							CheckLeveltoOpenSlot(INT nContainerID, INT nSlotID);
		BOOL							CheckHeavenLeveltoEquipSlot(INT nContainerID, INT nSlotID);
		BOOL							CheckHavePurpleGasBag();
		BOOL							GenerateHeavenMergeGasHint(FString& hint);
		void							SetConvertType(INT nType);
		INT								GetConvertType();	
		FString							GetConvertStr();	
		void							SetGasOperateType(INT nType);
		INT								GetGasOperateType(){return nGasOperateType;}
		GasItem*						GetItemBySlot(INT nContainerID,INT nSlotID);
		void 							GenerateGasTipsByNodeID(FString& tips,INT nNondeID1,INT nNodeID2,bool bRow = true);
		void							AddGasItemTemplateData(INT nKey,HeavenGasItemConfig* data);
		void							AddHeavenLevelData(INT nKey,HeavenLevelConfig* data);
		void							AddHeavenBloodData(HeavenBloodConfig& data);
		void							AddHeavenConvertData(HeavenJustConvertData& data);
		void							SetLastOffLineTime(UINT32 dwTime);
		BOOL							GetGasSkillArr(FString& tips,INT nID);
	private:
		GasItem*						GetItemByID(INT nID);
		GasSlot*						GetSlotByIndex(INT nContainerID,INT nSlotID);
		BloodSlot*						GetBloodSlotByIndex(INT nPage, INT nIndex);
		HeavenLevelConfig*				GetHeavenLevelDataIn(INT nLevel);
		HeavenGasItemConfig*			GetGasTemplateConfig(INT nTempID);
		void							AddNewGas(GasItem& data);
		void							DelGasItem(INT nUniqueID);
		INT								GenerateUniqueID();
		void							CheckSendOutLineConverter(INT nLevel);
		void							UpdateCheckSendNormalConverter();
		void							RefreshTargetItem();
		INT								GetGasExpLevelUp(HeavenGasItemConfig*pkGas,INT nExp);
		void							LoadHeavenData();
		BOOL							LoadOpenSlotCondition(const TCHAR* filePath);
		void							CheckSoulValue();
		void							CheckLevelValue();
		void							CheckSetNeedTime(BOOL bForce = FALSE);
		void							ShowNewGasHint(INT nGasID);
		INT								GetJustConvertDiscount();
		void							GerateLogInfo(INT nExp,INT nSpell);
	private:
		TArray<FString>					 converterLogLookUp;	
		//blood sea slot
		TArray<BloodSlot>				 arrSlotBlood;
		//qi bag, equipment slot, warehouse slot
		TArray<GasSlot>					 arrSlotGasBag;
		TArray<GasSlot>					 arrSlotGasBank;
		TArray<GasSlot>					 arrSlotGasEquip;
		TArray<SlotOpenTaxData>			 arrSlotTax;
		//synthesis target slot
		GasSlot*						 pkTargetGasSlot;
		//all of the player's qi items
		GasItemList						 myGasItemList;
		//template data
		HeavenGasTemplateMap			 gasItemTemplateMap;
		HeavenLevelDataMap				 heavenLevelDataMap;
		HeavenBloodDataList				 heavenBloodDataList;
		UINT32							 dwLastOfflineTime;		// last offline time
		UINT32							 dwLastExp2PtsTime;		// last conversion time
		INT32							 nDailyExp2PtsTime;		// conversions today
		INT								 nCostMoneyAll;
		INT								 nCurHeavenExp;
		time_t							 tNowTime;
		INT								 nNowNeedTime;
		INT								 gasPractiseState;
		HeavenTitleTool					 heavenTitleTool;
		INT								 nBloodState[MAX_BLOODSEA_PAGE];
		//generates qi unique ID, for internal use
		INT								 curMaxID;
		INT								 nHeavenLevel;
		BOOL							 bSoulLimited;
		BOOL							 bLevelLimited;
		BOOL							 bServerWaiting;
		INT								 nCurConvertType;
		INT							     nGasOperateType;
		INT							     nTempGasOperateType;
		//attribute column values
		HeavenAttList					 heavenAttList;
		HeavenConvertList				 heavenConvertList;
	public:
		const static INT				 oneKeyPracticeLevel = 10;			
	};
#endif