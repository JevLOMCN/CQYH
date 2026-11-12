#include "stdafx.h"
#include "MMOFacade.h"
#include "ActivityManager.h"
#include "UITextParser.h"
#include "LinkManager.h"
#include "MMOMapDataManager.h"
#include "DoodadManager.h"		
#include "CommonGameEvents.h"
#include "MMOGuideManager.h"
#include "MMOGameClasses.h"
#include "MMOMsgBoxManager.h"
#include "MMOQuestManager.h"
#include "MMODailySignInManager.h"
#include "MMOGuildManager.h"
#include "client_option_def.h"

ActivityManager::ActivityManager()
{
	MMOUI::RegisterEventHandler(this, GameEvent_VariableInit);
	MMOUI::RegisterEventHandler(this, GameEvent_VariableChanged);
}

ActivityManager::~ActivityManager()
{
	MMOUI::UnRegisterEventHandler(this, GameEvent_VariableInit);
	MMOUI::UnRegisterEventHandler(this, GameEvent_VariableChanged);
}

void ActivityManager::Init()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("ActivityManager::Init"), 0);

	_FBDataList.clear();
	_ActivityDataList.clear();
	_RewardBarDataList.clear();

	_ActivityTeamListData.Empty(MAX_TEAM_COUNT_PER_PAGE);

	MMOActivityDataManager::getSingleton().InitFromFile(TEXT("Activity.txt"), true);
	MMOActivityRewardBarDataManager::getSingleton().InitFromFile(TEXT("activity_reward.txt"), true);

	_LoadFBData();

	_CurActivityId = -1;
	_CurActivityTeamPage = 0;
	_ActiveValue = 0;
	_RewardBit = 0;

	_ClearVariable();
}

void ActivityManager::Shutdown()
{
	_FBDataList.clear();
	_ActivityDataList.clear();
	_RewardBarDataList.clear();
}

void ActivityManager::Clear()
{
	_ActivityCurShowDataList.clear();
	_QuitTodayActivityList.clear();
	_ActiveValue = 0;

	_ClearVariable();
}

void ActivityManager::Update(FLOAT deltaTime)
{

}

void ActivityManager::_ClearVariable()
{
	_ActivityVariableMap.clear();
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_1, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_2, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_3, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_4, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_5, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_6, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_7, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_8, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_9, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_10, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_11, 0));
	_ActivityVariableMap.insert(std::make_pair(EARV_ID_12, 0));
}

void ActivityManager::_UpdateVariable(UINT8 varType, INT32 referId, UINT16 index, INT32 val)
{
	if (varType == vtRole)
	{
		ActivityVariableMap::iterator iter = _ActivityVariableMap.find(index);
		if (iter != _ActivityVariableMap.end())
		{
			iter->second = val;
		}
	}
}

INT32 ActivityManager::_GetVariableValue(UINT8 varType, UINT16 index)
{
	if (varType == vtRole)
	{
		ActivityVariableMap::iterator iter = _ActivityVariableMap.find(index);
		if (iter != _ActivityVariableMap.end())
		{
			return iter->second;
		}
	}

	return 0;
}

INT32 ActivityManager::_GetVariableCompleteCount(UINT8 varType, UINT16 index)
{
	INT32 count = 0;

	if (varType == vtRole)
	{
		switch (index)
		{
		case EARV_ID_2:// Whether the guild stronghold war BUFF has been claimed
			count = _GetVariableValue(varType, index) > 0 ? 1 : 0;
			break;

		case EARV_ID_4:// Last entry time to Youming Sea
		case EARV_ID_5:// Last guild prayer time
		case EARV_ID_6:// Last entry time to Nine-Layer Demon Tower
		case EARV_ID_7:// Last entry time to Royal Treasury
		case EARV_ID_8:// Last entry time to Experience Arena
		case EARV_ID_10:// Last time the character did guild trading
		case EARV_ID_11:// Time the character last soaked in the hot spring today
		case EARV_ID_12:// Training Ground daily count time
			{
				int lastDay = 0;
				int lastHour = 0;
				__time32_t lastTime = _GetVariableValue(varType, index);
				struct tm *lastDate = _localtime32(&lastTime);
				if (lastTime > 0 && lastDate)
				{
					lastDay = lastDate->tm_mday;
					lastHour = lastDate->tm_hour;
				}

				int localDay = 0;
				int localHour = 0;
				__time32_t localTime;
				_time32(&localTime);
				struct tm *localDate = _localtime32(&localTime);
				if (localTime > 0 && localDate)
				{
					localDay = localDate->tm_mday;
					localHour = localDate->tm_hour;
				}

				/*
				// Reset version at 4:00 AM
				// Quest #25369
				if (lastDay < localDay - 1)
				{
					count = 0;
				}
				else if (lastDay == localDay - 1)
				{
					if (lastHour < 4)
					{
						count = 0;
					}
					else
					{
						if (localHour < 4)
						{
							if (index == EARV_ID_4)
							{
								count = _GetVariableValue(varType, EARV_ID_3);
							}
							else if (index == EARV_ID_5)
							{
								count = _GetVariableValue(varType, EARV_ID_1);
							}
							else if (index == EARV_ID_12)
							{
								count = _GetVariableValue(varType, EARV_ID_9);
							}
							else
							{
								count = 1;
							}
						}
						else
						{
							count = 0;
						}
					}
				}
				else if (lastDay == localDay)
				{
					if (lastHour < 4)
					{
						if (localHour < 4)
						{
							if (index == EARV_ID_4)
							{
								count = _GetVariableValue(varType, EARV_ID_3);
							}
							else if (index == EARV_ID_5)
							{
								count = _GetVariableValue(varType, EARV_ID_1);
							}
							else if (index == EARV_ID_12)
							{
								count = _GetVariableValue(varType, EARV_ID_9);
							}
							else
							{
								count = 1;
							}
						}
						else
						{
							count = 0;
						}
					}
					else
					{
						if (index == EARV_ID_4)
						{
							count = _GetVariableValue(varType, EARV_ID_3);
						}
						else if (index == EARV_ID_5)
						{
							count = _GetVariableValue(varType, EARV_ID_1);
						}
						else if (index == EARV_ID_12)
						{
							count = _GetVariableValue(varType, EARV_ID_9);
						}
						else
						{
							count = 1;
						}
					}
				}
				else
				{
					count = 0;
				}
				*/

				// Reset version at 0:00 AM
				if (lastDay < localDay)
				{
					count = 0;
				}
				else if (lastDay == localDay)
				{
					if (index == EARV_ID_4)
					{
						count = _GetVariableValue(varType, EARV_ID_3);
					}
					else if (index == EARV_ID_5)
					{
						count = _GetVariableValue(varType, EARV_ID_1);
					}
					else if (index == EARV_ID_12)
					{
						count = _GetVariableValue(varType, EARV_ID_9);
					}
					else
					{
						count = 1;
					}
				}
				else
				{
					count = 0;
				}
			}
			break;
		}
	}

	return count;
}

// Activity state update event
void ActivityManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch (evt->GetEventType())
	{
	case GameEvent_VariableInit:
	case GameEvent_VariableChanged:
		{
			MMOGameEvent_VariableChange* varChgEvent = (MMOGameEvent_VariableChange*)evt;
			if (varChgEvent)
			{
				_UpdateVariable(varChgEvent->VarType, varChgEvent->VarReferID, varChgEvent->VarID, varChgEvent->VarValue);
			}
		}
		break;
	}
}

// Activity data
const ActivityData* ActivityManager::GetActivityData(INT activityId) const
{
	if(activityId < 0)
		return NULL;
	ActivityDataList::const_iterator itr  = _ActivityDataList.begin();
	for (; itr != _ActivityDataList.end(); ++itr)
	{
		if ((*itr).nActivityID == activityId)
			return &(*itr);
	}
	return NULL;
}
const ActivityDataList& ActivityManager::GetActivityDataList() const
{
	return _ActivityCurShowDataList;
}

const ActivityDataList& ActivityManager::GetCompleteActivityDataList() const
{
	return _ActivityDataList;
}

const FBActivityData* ActivityManager::GetFBActivityData(INT FBId) const
{
	if(FBId < 0)
		return NULL;
	FBActivityDataList::const_iterator itr  = _FBDataList.begin();
	for (; itr != _FBDataList.end(); ++itr)
	{
		if ((*itr).nFBID == FBId)
			return &(*itr);
	}
	return NULL;
}

const FBActivityDataList& ActivityManager::GetFBActivityDataList() const
{
	return _FBDataList;
}

INT ActivityManager::GetActivityAcceptMaxTimes(INT activityId)
{
	return 3;
}

void ActivityManager::_LoadFBData()
{
	const TArray<FDungeonInfo> & tArrFB = UGlobalMapInfo::GetSingleton()->GetDungeonInfoList();
	for (INT n = 0; n < tArrFB.Num(); ++n)
	{
		const FDungeonInfo& fbInfo = tArrFB(n);
		const FExportMap* pkMap = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer(fbInfo.MapID);
		if(pkMap == NULL)
			continue;
		FBActivityData data;
		data.nFBID = fbInfo.DungeonInfoID;
		data.ndestMapID = pkMap->ServerID;
		data.sFBName = pkMap->MapDisplayName;
		if(pkMap->FinishTarget > 0)
		{
			data.nAcceptNpcID = pkMap->FinishTarget;
			//data.sPosName =  pkMap->FinishTarget->GetDisplayName();
		}
		else
			continue;

		if(fbInfo.MinLevel >= 99)
			continue;

		if(fbInfo.MaxLevel == 0)
			data.sLevelString = FString::Printf(TEXT("%d"),fbInfo.MinLevel);
		else
			data.sLevelString = FString::Printf(TEXT("%d-%d"),fbInfo.MinLevel,fbInfo.MaxLevel);

		data.nCheckLevelMin = fbInfo.MinLevel;
		data.nCheckLevelMax = fbInfo.MaxLevel;
		data.nRewardInfo.nRewardType = 7;
		data.nRewardInfo.nArrItemID[0]= fbInfo.DungeonSpecialRewardsID[0];
		data.nRewardInfo.nArrItemID[1]= fbInfo.DungeonSpecialRewardsID[1];
		data.nRewardInfo.nArrItemID[2]= fbInfo.DungeonSpecialRewardsID[2];
		data.nRewardInfo.nArrItemID[3]= fbInfo.DungeonSpecialRewardsID[3];
		data.nRewardInfo.nArrItemID[4]= fbInfo.DungeonSpecialRewardsID[4];
		data.nRewardInfo.nArrItemID[5]= fbInfo.DungeonSpecialRewardsID[5];
		data.nRewardInfo.nArrItemID[6]= fbInfo.DungeonSpecialRewardsID[6];
		data.nRewardInfo.nArrItemID[7]= fbInfo.DungeonSpecialRewardsID[7];
		data.nRewardInfo.nArrItemID[8]= fbInfo.DungeonSpecialRewardsID[8];
		data.nRewardInfo.nArrItemID[9]= fbInfo.DungeonSpecialRewardsID[9];

		//--------------------------------------------------------------------------
		data.sDecrible = pkMap->MapDescript;
		data.nFBDifficulty = fbInfo.Difficulty;

		FString difficultyTitleFormat = TEXT("MMOGame.FBEnter.DifficultyTitleFormat");
		difficultyTitleFormat += appItoa( data.nFBDifficulty - 2);//E_esd_Easy = 2;
		data.sDifficultyString = MMOUI::ParseLocalizedPropertyPath( difficultyTitleFormat );

		INT limitPeople = fbInfo.RoleLimit;
		FString peopleString;
		if( limitPeople == 1 )
			peopleString = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBEnter.PeopleStringFormat3") );
		else
			peopleString = FString::Printf( *MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.FBEnter.PeopleStringFormat2") ), limitPeople );
		data.nPersonNumMin = limitPeople;
		data.sPersonString = peopleString;
		data.nFBCostActiveValue = fbInfo.Fatigue;
		_FBDataList.push_back(data);
	}
}
void ActivityManager::_UnloadActivityData()
{
	_ActivityDataList.clear();
	_ActivityCurShowDataList.clear();
}

void ActivityManager::NotifyActivityVariableErvtActivity(INT32 nValue)
{
	_ActiveValue = nValue;
	//if(_MaxActiveValue < _ActiveValue) 
	//	_MaxActiveValue = _ActiveValue + 30;
	MMOGameEvent_Any evt1( GameEvent_ActivityActive);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Any evt2(GameEvent_ActivityRewardState);
	MMOUI::PostGameEvent(&evt2);
}
void ActivityManager::NotifyActivityVariableErvtActivityReward(INT32 nValue)
{
	_RewardBit = nValue;
	MMOGameEvent_Any evt( GameEvent_ActivityRewardState);
	MMOUI::PostGameEvent(&evt);
}
void ActivityManager::NotifyActivityVariableErvtQuestComNum(INT32 nValue)
{
	_RewardQuestComNum = nValue;
}
bool ActivityManager::CompareAdd_StartTime(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nTimeInfo.CompareValue(rightData.nTimeInfo);
}
bool ActivityManager::CompareDec_StartTime(const ActivityData& leftData, const ActivityData& rightData)
{
	return !leftData.nTimeInfo.CompareValue(rightData.nTimeInfo);
}

bool ActivityManager::CompareAdd_Reward(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nRewardInfo.nRewardVitality < rightData.nRewardInfo.nRewardVitality;
}
bool ActivityManager::CompareDec_Reward(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nRewardInfo.nRewardVitality > rightData.nRewardInfo.nRewardVitality;
}
bool ActivityManager::CompareAdd_PersonNum(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nPersonNumMin < rightData.nPersonNumMin;
}
bool ActivityManager::CompareDec_PersonNum(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nPersonNumMin > rightData.nPersonNumMin;
}

bool ActivityManager::CompareAdd_StarNum(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nHotStarNum < rightData.nHotStarNum;
}
bool ActivityManager::CompareDec_StarNum(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nHotStarNum > rightData.nHotStarNum;
}
bool ActivityManager::CompareAdd_State(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nTimeInfo.nState < rightData.nTimeInfo.nState;
}
bool ActivityManager::CompareDec_State(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nTimeInfo.nState > rightData.nTimeInfo.nState;
}
bool ActivityManager::CompareAdd_CompleteState(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nHaveDoneTimes < rightData.nHaveDoneTimes;
}
bool ActivityManager::CompareDec_CompleteState(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nHaveDoneTimes > rightData.nHaveDoneTimes;;
}
bool ActivityManager::CompareAdd_Level(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nCheckLevelMin < rightData.nCheckLevelMin;;
}
bool ActivityManager::CompareDec_Level(const ActivityData& leftData, const ActivityData& rightData)
{
	return leftData.nCheckLevelMin > rightData.nCheckLevelMin;;
}
bool ActivityManager::FBCompareAdd_CostActiveValue(const FBActivityData& leftData, const FBActivityData& rightData)
{
	return leftData.nFBCostActiveValue < rightData.nFBCostActiveValue;
}
bool ActivityManager::FBCompareDec_CostActiveValue(const FBActivityData& leftData, const FBActivityData& rightData)
{
	return leftData.nFBCostActiveValue > rightData.nFBCostActiveValue;
}

bool ActivityManager::FBCompareAdd_Dificulty(const FBActivityData& leftData, const FBActivityData& rightData)
{
	return leftData.nFBDifficulty < rightData.nFBDifficulty;
}
bool ActivityManager::FBCompareDec_Dificulty(const FBActivityData& leftData, const FBActivityData& rightData)
{
	return leftData.nFBDifficulty > rightData.nFBDifficulty;
}

bool ActivityManager::FBCompareAdd_Level(const FBActivityData& leftData, const FBActivityData& rightData)
{
	return leftData.nCheckLevelMin < rightData.nCheckLevelMin;
}
bool ActivityManager::FBCompareDec_Level(const FBActivityData& leftData, const FBActivityData& rightData)
{
	return leftData.nCheckLevelMin > rightData.nCheckLevelMin;
}

bool ActivityManager::FBCompareAdd_PersonNum(const FBActivityData& leftData, const FBActivityData& rightData)
{
	return leftData.nPersonNumMin < rightData.nPersonNumMin;
}
bool ActivityManager::FBCompareDec_PersonNum(const FBActivityData& leftData, const FBActivityData& rightData)
{
	return leftData.nPersonNumMin > rightData.nPersonNumMin;
}
void ActivityManager::SortActivityList(INT nType,INT nAdd)
{ 
	if(_CurShowType != (INT)EActivityCopy)
		SortActivityDataList(nType,nAdd);
	else
		SortFBActivityDataList(nType,nAdd);
}
void ActivityManager::SortActivityDataList(INT nType,INT nAdd)
{
	switch ( nType )
	{
	case EActivitySortHead_Time:
		{
			if(nAdd)
				_ActivityCurShowDataList.sort(CompareAdd_StartTime);
			else
				_ActivityCurShowDataList.sort(CompareDec_StartTime);
			break;
		}
	case EActivitySortHead_Reward:
		{
			if(nAdd)
				_ActivityCurShowDataList.sort(CompareAdd_Reward);
			else
				_ActivityCurShowDataList.sort(CompareDec_Reward);
			break;

		}
	case EActivitySortHead_PersonNum:
		{
			if(nAdd)
				_ActivityCurShowDataList.sort(CompareAdd_PersonNum);
			else
				_ActivityCurShowDataList.sort(CompareDec_PersonNum);
			break;
		}
	case EActivitySortHead_State:
		{
			if(nAdd)
				_ActivityCurShowDataList.sort(CompareAdd_State);
			else
				_ActivityCurShowDataList.sort(CompareDec_State);
			break;
		}
	case EActivitySortHead_CompleteStatus:
		{
			if(nAdd)
				_ActivityCurShowDataList.sort(CompareAdd_CompleteState);
			else
				_ActivityCurShowDataList.sort(CompareDec_CompleteState);
			break;
		}
	case EActivitySortHead_Star:
		{
			if(nAdd)
				_ActivityCurShowDataList.sort(CompareAdd_StarNum);
			else
				_ActivityCurShowDataList.sort(CompareDec_StarNum);
			break;
		}
	case EFBSortHead_Level:
		{
			if(nAdd)
				_ActivityCurShowDataList.sort(CompareAdd_Level);
			else
				_ActivityCurShowDataList.sort(CompareDec_Level);
			break;
		}
	}
}
void ActivityManager::SortFBActivityDataList(INT nType,INT nAdd)
{
	switch ( nType )
	{
	case EActivitySortHead_PersonNum:
		{
			if(nAdd)
				_FBDataList.sort(FBCompareAdd_PersonNum);
			else
				_FBDataList.sort(FBCompareDec_PersonNum);
			break;
		}
	case EActivitySortHead_CompleteStatus:
		{
			if(nAdd)
				_FBDataList.sort(FBCompareAdd_Dificulty);
			else
				_FBDataList.sort(FBCompareDec_Dificulty);
			break;
		}

	case EFBSortHead_Level:
		{
			if(nAdd)
				_FBDataList.sort(FBCompareAdd_Level);
			else
				_FBDataList.sort(FBCompareDec_Level);
			break;
		}
	case EFBSortHead_CostActiveValue:
		{
			if(nAdd)
				_FBDataList.sort(FBCompareAdd_CostActiveValue);
			else
				_FBDataList.sort(FBCompareDec_CostActiveValue);
			break;
		}
	}
}
void ActivityManager::SortTodayActivityList(INT nType,INT nAdd)
{
	switch(nType)
	{
	case EActivitySortHead_Reward:
		{
			if(nAdd)
				_QuitTodayActivityList.sort(CompareAdd_Reward);
			else
				_QuitTodayActivityList.sort(CompareDec_Reward);
			break;

		}
	case EActivitySortHead_State:
		{
			if(nAdd)
				_QuitTodayActivityList.sort(CompareAdd_State);
			else
				_QuitTodayActivityList.sort(CompareDec_State);
			break;
		}
	case EActivitySortHead_CompleteStatus:
		{
			if(nAdd)
				_QuitTodayActivityList.sort(CompareAdd_CompleteState);
			else
				_QuitTodayActivityList.sort(CompareDec_CompleteState);
			break;
		}
	}
}
EActivityState ActivityTime::GetState()
{
	if(nStartTime_year == 0 && nStartTime_month == 0&&nStartTime_day == 0&&nStartTime_hour ==0&&
		nEndTime_year ==0 && nEndTime_month == 0 && nEndTime_day == 0 && nEndTime_hour == 0)
		return EStarting;
	time_t nowT = time(NULL);
	tm     *pkTp = localtime(&nowT);
	static time_t	endT;
	tm      tp;
	INT nsYear = (nStartTime_year == 0)? (pkTp->tm_year + 1900):nStartTime_year;
	INT nsMon   = (nStartTime_month == 0)? (pkTp->tm_mon + 1):nStartTime_month;
	INT nDay    = (nStartTime_day == 0)?pkTp->tm_mday:nStartTime_day;
	INT nHour   = (nStartTime_hour == 0)?pkTp->tm_hour:nStartTime_hour;
	appMemzero(&tp, sizeof(tm));

	tp.tm_year = nsYear - 1900;
	tp.tm_mon  = nsMon - 1;
	tp.tm_mday = nDay;
	tp.tm_hour = nHour;
	tp.tm_min  = nStartTime_minute;

	startT = mktime(&tp);

	nsYear = (nEndTime_year == 0)? (pkTp->tm_year + 1900):nEndTime_year;
	nsMon   = (nEndTime_month == 0)? (pkTp->tm_mon + 1):nEndTime_month;
	nDay    = (nEndTime_day == 0)?pkTp->tm_mday:nEndTime_day;
	nHour   = (nEndTime_hour == 0)?pkTp->tm_hour:nEndTime_hour;

	tp.tm_year = nsYear - 1900;
	tp.tm_mon  = nsMon - 1;
	tp.tm_mday = nDay;
	tp.tm_hour = nHour;
	tp.tm_min  = nEndTime_minute;
	endT   = mktime(&tp);

	if(nowT < startT)
		nState = ENOStart;
	else if (nowT > endT)
		nState = EActivityOver;
	else if(nowT >= startT && nowT <= endT)
		nState = EStarting;
	else if(nowT < startT && nowT +300 > startT)
		nState = EWillStart;
	else
		nState = EActivityOver;
	return nState;
}
void  ActivityManager::FilterTodayActivity()
{
	time_t nowT = time(NULL);
	tm     *tp = localtime(&nowT);
	if(tp == 0)
		return;
	_QuitTodayActivityList.clear();
	ActivityDataList::iterator itr  = _ActivityDataList.begin();
	for (; itr != _ActivityDataList.end(); ++itr)
	{
		ActivityData& data = (*itr);
		if(data.nTimeInfo.GetState() == EStarting)
			_QuitTodayActivityList.push_back(data);
			
	}
}
void ActivityManager::FilterActivityType(INT nActivityType,INT nRewardType)
{
	_CurShowType = nActivityType;
	if(_CurShowType != (INT)EActivityCopy)
	{
		_ActivityCurShowDataList.clear();
		ActivityDataList::const_iterator itr  = _ActivityDataList.begin();
		for (; itr != _ActivityDataList.end(); ++itr)
		{
			const ActivityData& data = (*itr);
			if(data.nRewardInfo.nRewardType&nRewardType)
			{
				if(nActivityType == EActivityHot )
				{
					AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
					if(mainPlayer&&mainPlayer->ActorAttributes->LevelAttributes.Level >= data.nCheckLevelMin)
						_ActivityCurShowDataList.push_back(data);
				}
				else
				{
					_ActivityCurShowDataList.push_back(data);
				}
			}
		}
	}
	else
	{
		//Dungeon: show directly without filtering
	}
}
void ActivityManager::SetActivityList(UMMOGFXPlayer* gfxPlayer, UGFxObject* dataProvider)
{
	if(gfxPlayer == NULL || dataProvider == NULL)
		return;
	if(_CurShowType != (INT)EActivityCopy)
		SetActivityDataList(gfxPlayer,dataProvider);
	else
		SetFBActivityDataList(gfxPlayer,dataProvider);	
}

void ActivityManager::_GetActivityStateString(EActivityState eState, FString& stateString)
{
	stateString = TEXT("");
	if (eState == ENOStart)
	{
		stateString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint1"));
	}
	else if (eState == EWillStart)
	{
		stateString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint2"));
	}
	else if (eState == EStarting)
	{
		stateString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint3"));
	}
	else
	{
		stateString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint4"));
	}
}

void ActivityManager::_GetGuildActivityTimeString(UINT16 activityType, FString& timeString)
{
	timeString = TEXT("");
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	if (baseInfo.Id == 0)
	{
		timeString = TEXT("TBD");
		return;
	}

	UINT16 activityLevel = baseInfo.level;
	UINT32 activityId = MAKELONG(activityLevel, activityType);

	GuildActivityInfoMap guildActivityInfoMap = MMOGuildData::getSingleton().GetGuildActivityInfoMap();
	GuildActivityInfoMap::iterator iter = guildActivityInfoMap.find(activityType);
	if (iter != guildActivityInfoMap.end())
	{
		GuildActivityData data;
		if (MMOGuildData::getSingleton().GetGuildActivityData(activityId, data))
		{
			if (iter->second.State == GAS_NONE)
			{
				timeString = TEXT("TBD");
			}
			else
			{
				if (iter->second.StartTime > 0)
				{
					__time32_t startTime = iter->second.StartTime;
					struct tm *startDate = _localtime32(&startTime);
					int startHour = startDate->tm_hour;
					int startMin = startDate->tm_min;

					__time32_t endTime = iter->second.StartTime + data.Duration;
					struct tm *endDate = _localtime32(&endTime);
					int endHour = endDate->tm_hour;
					int endMin = endDate->tm_min;

					if (startDate && endDate)
					{
						timeString = FString::Printf(TEXT("%d:%02d-%d:%02d"), startHour, startMin, endHour, endMin);
					}
				}
			}
		}
	}
}

void ActivityManager::_GetGuildActivityStateString(UINT16 activityType, FString& stateString)
{
	stateString = TEXT("");
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	if (baseInfo.Id == 0)
	{
		stateString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint1"));
		return;
	}

	UINT16 activityLevel = baseInfo.level;
	UINT32 activityId = MAKELONG(activityLevel, activityType);

	GuildActivityInfoMap guildActivityInfoMap = MMOGuildData::getSingleton().GetGuildActivityInfoMap();
	GuildActivityInfoMap::iterator iter = guildActivityInfoMap.find(activityType);
	if (iter != guildActivityInfoMap.end())
	{
		GuildActivityData data;
		if (MMOGuildData::getSingleton().GetGuildActivityData(activityId, data))
		{
			if (iter->second.State == GAS_NONE)
			{
				stateString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint1"));
			}
			else if (iter->second.State == GAS_OPEN)
			{
				stateString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint3"));
			}
			else if (iter->second.State == GAS_OVER)
			{
				stateString = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint4"));
			}
		}
	}
}

void ActivityManager::SetActivityDataList(UMMOGFXPlayer* gfxPlayer, UGFxObject* dataProvider)
{
	if(gfxPlayer == NULL || dataProvider == NULL)
		return;
	ActivityDataList::iterator iter = _ActivityCurShowDataList.begin();
	
	for (INT index = 0; iter != _ActivityCurShowDataList.end(); ++ iter)
	{
		//timeLimit of 0 means no time limit
		INT findIndex = 0;
		if (iter->aTimeLimit.Num() > 0)
		{
			int day = 0;
			__time32_t localTime;
			_time32(&localTime);
			struct tm *localDate = _localtime32(&localTime);
			if (localDate)
			{
				day = localDate->tm_wday;
			}

			findIndex = 0;
			if (!iter->aTimeLimit.FindItem(day, findIndex))
			{
				continue;
			}
		}

		FString strTime = TEXT("");
		FString strState = TEXT("");
		if (iter->nGuildActivityType > 0)
		{
			_GetGuildActivityTimeString(iter->nGuildActivityType, strTime);
			_GetGuildActivityStateString(iter->nGuildActivityType, strState);
		} 
		else
		{
			strTime = iter->nTimeInfo.sTimeStr;
			EActivityState eState = iter->nTimeInfo.GetState();
			_GetActivityStateString(eState, strState);
		}

		UGFxObject* object = gfxPlayer->CreateObject(TEXT("UI_ActivityWidget.ActivityAS3.ActivityData"));
		if (object == 0)
		{
			continue;
		}

		object->SetInt(TEXT("Id"), iter->nActivityID);
		object->SetString(TEXT("Name"), iter->sActivityName);
		object->SetString(TEXT("Time"), strTime);
		object->SetString(TEXT("PersonNum"), iter->sPersonString);
		object->SetInt(TEXT("Activity"), iter->nRewardInfo.nRewardVitality);
		object->SetBool(TEXT("IsVip"), iter->nVipLevel > 0);

		FString strComplete = TEXT("--");
		if (iter->nCompleteType == EACPT_None)
		{
			strComplete = TEXT("--");
		} 
		else if (iter->nCompleteType == EACPT_Quest)
		{
			INT nCompleteNum = 0;
			INT nCanComNumMax = iter->nPerDayJoinNum;

			if (iter->nLinkQuestType == E_qt_Reward)
			{
				nCompleteNum = _RewardQuestComNum;
				nCanComNumMax =  MMOQuestData::getSingleton().GetRewardQuestAvailableLimit();
			}
			else if (iter->nLinkQuestType == E_qt_Circle)
			{
				CircleQuestInfo dataInfo;
				MMOQuestData::getSingleton().GetCircleQuestInfo(iter->nLinkQuestID, dataInfo);
				nCompleteNum = dataInfo.CircleRound;
			}
			else if (iter->nLinkQuestType == E_qt_Emegency)
			{
				nCompleteNum = MMOQuestData::getSingleton().GetEmergencyQuestCompleteCount();
				nCanComNumMax = iter->nPerDayJoinNum;
			}
			else
			{
				nCompleteNum = MMOQuestData::getSingleton().GetCompletedQuestCount(iter->nLinkQuestID);
			}

			if (nCompleteNum >= nCanComNumMax)
			{
				nCompleteNum = nCanComNumMax;
				strState = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint5"));
			}

			strComplete = FString::Printf(TEXT("%d / %d"),nCompleteNum,nCanComNumMax);
		}
		else if (iter->nCompleteType == EACPT_Time)
		{
			INT nCompleteNum = (iter->nTimeInfo.GetState() == EActivityOver) ? 1 : 0;
			INT nCanComNumMax = 1;
			strComplete = FString::Printf(TEXT("%d / %d"),nCompleteNum,nCanComNumMax);
		}
		else if (iter->nCompleteType == EACPT_SignIn)
		{
			const SSignInRewardState &curSignInState = MMODailySignInManager::getSingleton().GetMyCurSignInState();
			INT nCompleteNum = curSignInState._CanSignIn ? 0 : 1;
			INT nCanComNumMax = 1;

			if (nCompleteNum >= nCanComNumMax)
			{
				nCompleteNum = nCanComNumMax;
				strState = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint5"));
			}

			strComplete = FString::Printf(TEXT("%d / %d"),nCompleteNum,nCanComNumMax);
		}
		else if (iter->nCompleteType == EACPT_Variable)
		{
			INT nCompleteNum = _GetVariableCompleteCount(vtRole, iter->nRoleVariable);
			INT nCanComNumMax = iter->nPerDayJoinNum;

			if (nCompleteNum >= nCanComNumMax)
			{
				nCompleteNum = nCanComNumMax;
				strState = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint5"));
			}

			strComplete = FString::Printf(TEXT("%d / %d"), nCompleteNum, nCanComNumMax);
		}

		if (!(CMessageHandler::GetInstance().GetClientOption() & CLIENT_OPTION_BIT_ACTIVITY_VALUE))
		{
			strComplete = TEXT("--");
		}

		object->SetString(TEXT("State"), strState);	
		object->SetString(TEXT("CompleteState"), strComplete);

		object->SetInt(TEXT("StarNum"),  iter->nHotStarNum);
		//object->SetString(TEXT("Describe"), iter->sDecrible);
		object->SetString(TEXT("Level"), iter->sLevelString);

		dataProvider->SetElementObject(index, object);
		index ++;
	}

	if (_ActivityCurShowDataList.size() > 0)
	{
		iter = _ActivityCurShowDataList.begin();
		if (iter != _ActivityCurShowDataList.end())
		{
			_CurActivityId = iter->nActivityID;
		}
	}
}
void ActivityManager::SetFBActivityDataList(UMMOGFXPlayer* gfxPlayer, UGFxObject* dataProvider)
{
	if(gfxPlayer == NULL || dataProvider == NULL)
		return;
	FBActivityDataList::iterator iter = _FBDataList.begin();
	for (INT index = 0;iter != _FBDataList.end(); ++ iter)
	{
		UGFxObject* object = gfxPlayer->CreateObject(TEXT("UI_ActivityWidget.ActivityAS3.ActivityData"));
		if(object == 0)
			continue;
		object->SetInt(TEXT("Id"), iter->nFBID);
		object->SetString(TEXT("Name"), iter->sFBName);
		object->SetString(TEXT("PersonNum"), iter->sPersonString);
		object->SetInt(TEXT("Activity"), iter->nFBCostActiveValue);
		object->SetString(TEXT("CompleteState"), iter->sDifficultyString);
		object->SetString(TEXT("Level"), iter->sLevelString);

		dataProvider->SetElementObject(index, object);
		index ++;
	}
}
void ActivityManager::SetActivityTeamMemberInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nTeamID)
{
	if(!gfxPlayer || !object)
		return;
	_CurActivityTipsTeamId = nTeamID;
	/*TeamMemberListMap::iterator itor = _TeamMemberListCache.find(nTeamID);
	if(itor != _TeamMemberListCache.end())
	{
		TArray<ActivityTeamMember>& memberList = itor->second;
		INT idx = 0;
		for(INT i = 0; i < memberList.Num(); ++i)
		{
			////////////////
			UGFxObject* playerItem = gfxPlayer->CreateObject(TEXT("Object"));
			
			++idx;
		}
	}
	else
	{
		//query from server
	}*/
	AMMOPlayer *mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer == NULL)
		return;
	UGFxObject* memberArray = gfxPlayer->CreateArray();
	object->SetObject(TEXT("MemberArray"),memberArray);
	object->SetString(TEXT("title_0"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.Name")));
	object->SetString(TEXT("title_2"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.Class")));
	object->SetString(TEXT("title_3"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.Level")));
	object->SetString(TEXT("level_limit_title"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.LevelLimit")));
	object->SetString(TEXT("guild_limit_title"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.GuidLimit")));
	object->SetString(TEXT("class_limit_title"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.ClassLimit")));
	if(ActivityManager::getSingleton().GetActivityFilterType() == EActivityCopy)
	{
		//Dungeon
		object->SetInt(TEXT("TipsType"), 1);
		object->SetString(TEXT("title_1"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.HuoYue")));
	}
	else
	{
		//Activity
		object->SetInt(TEXT("TipsType"), 0);
		object->SetString(TEXT("title_1"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.Times")));
	}

	INT idx = 0;
	for(INT i = 0; i < _ActivityTeamListData.Num(); ++i)
	{
		if(_ActivityTeamListData(i).TeamId == nTeamID)
		{
			object->SetString(TEXT("ActivityName"), _ActivityTeamListData(i).Name);
			for(INT k = 0; k < _ActivityTeamListData(i).PlayerCount; ++k)
			{
				PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(_ActivityTeamListData(i).MemberId[k]);
				if (pPlayerBaseInfo)
				{
					UGFxObject* memberObj = gfxPlayer->CreateObject("Object");
					memberObj->SetString(TEXT("Name"), pPlayerBaseInfo->_RoleName);
					if(ActivityManager::getSingleton().GetActivityFilterType() == EActivityCopy)
					{
						memberObj->SetInt(TEXT("HuoYue"), 0);
					}
					else
					{
						memberObj->SetInt(TEXT("CurTimes"), 0);
						memberObj->SetInt(TEXT("MaxTimes"), 0);
					}
					memberObj->SetString(TEXT("ClassName"), MMOUI::ParseLocalizedPropertyPath( FString("MMOGame.School.SchoolName") + appItoa(pPlayerBaseInfo->_RoleClass)));
					memberObj->SetInt(TEXT("Level"), pPlayerBaseInfo->_Level);

					if(mainPlayer->ServerID == pPlayerBaseInfo->_RoleID)
						memberObj->SetBool("bIsMine",true);
					else
						memberObj->SetBool("bIsMine",false);
					memberArray->SetElementObject(idx++, memberObj);
				}
			}
			break;
		}
	}
}

void ActivityManager::SetActivityTeamInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nActivityID, INT nPage)
{
	if(!gfxPlayer || !object)
		return;
	/*ActivityTeamListMap::iterator itor = _ActivityTeamListCache.find(nActivityID);
	if(itor != _ActivityTeamListCache.end())
	{
		TArray<ActivityTeam>& teamList = itor->second;
		INT idx = 0;
		for(INT i = 0; i < teamList.Num(); ++i)
		{
			UGFxObject* teamItem = gfxPlayer->CreateObject(TEXT("Object"));
			++idx;
		}
	}
	else
	{

	}*/
	if(_CurShowType != (INT)EActivityCopy)
	{
		const ActivityData* pkActivity = GetActivityData(_CurActivityId);
		if(pkActivity)
		{
			//If it is a single-player activity or dungeon, don't show the team
			if(pkActivity->nPersonNumMin <= 1)
			{
				UGFxObject* teamItem = gfxPlayer->CreateObject(TEXT("Object"));
				teamItem->SetInt(TEXT("TeamId"), -1);
				object->SetElementObject(0, teamItem);
				return;
			}
		}
	}
	else
	{
		const FBActivityData*	pkFB = 	GetFBActivityData(_CurActivityId);
		if(pkFB&&pkFB->nPersonNumMin <= 1)
		{
			UGFxObject* teamItem = gfxPlayer->CreateObject(TEXT("Object"));
			teamItem->SetInt(TEXT("TeamId"), -1);
			object->SetElementObject(0, teamItem);
			return;
		}
	}
	INT idx = 0;
	for(INT i = 0; i < _ActivityTeamListData.Num(); ++i)
	{
		if(_ActivityTeamListData(i).TeamId > 0)
		{
			UGFxObject* teamItem = gfxPlayer->CreateObject(TEXT("Object"));
			teamItem->SetString(TEXT("TeamName"), _ActivityTeamListData(i).Name);
			//teamItem->SetInt(TEXT("TeamLevel"), _ActivityTeamListData(i).Level);
			teamItem->SetInt(TEXT("TeamLevel"), GetActivityTeamLevel(_ActivityTeamListData(i).TeamId));
			teamItem->SetInt(TEXT("MemberCount"), _ActivityTeamListData(i).PlayerCount);
			teamItem->SetInt(TEXT("TeamId"), _ActivityTeamListData(i).TeamId);

			if(TeamManager::getSingleton().GetMyTeamID() == _ActivityTeamListData(i).TeamId)
				teamItem->SetBool("bMyTeam",true);
			else
				teamItem->SetBool("bMyTeam",false);
			object->SetElementObject(idx, teamItem);
			++idx;
		}
	}
}

void ActivityManager::SetActivityInviteList(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nType, UBOOL bActivity)
{
	//bActivity: activity or dungeon
	//nType 0 friend, 1 recent teammate, 2 guild member
}

void ActivityManager::SetActivityDetailInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nActivityID)
{
	if (gfxPlayer == 0 || object == 0)
		return;
	const ActivityData* pkActivity = GetActivityData(nActivityID);
	if (!pkActivity)
	{
		ClearActivityDetailInfo(gfxPlayer,object);
		return;
	}

	FString conditionDesc;
	if (pkActivity->nCheckLevelMin >= 0)
		conditionDesc = pkActivity->sLevelString;

	if (pkActivity->nVipLevel > 0)
		conditionDesc += TEXT("VIP player");

	conditionDesc += '\n';
	if (pkActivity->nPersonNumMin >= 0)
		conditionDesc += pkActivity->sPersonString;

	object->SetString(TEXT("activityName"),pkActivity->sActivityName);
	object->SetString(TEXT("conditionTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitle1")));
	object->SetString(TEXT("conditionLimited"), conditionDesc);
	
	// Quest-giver NPC
	object->SetString(TEXT("positionTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitle2")));

	for (INT i = 0; i < pkActivity->ndestMapID.Num(); ++i)
	{
		FExportMap* mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfoPointer(pkActivity->ndestMapID(i));
		if (mapInfo)
		{
			conditionDesc = mapInfo->MapDisplayName;
			conditionDesc += '\n';
		}
		else
		{
			conditionDesc = TEXT("");
		}
	}

	for (INT i = 0; i < pkActivity->sNpcName.Num(); ++i)
	{
		if (i >= pkActivity->nAcceptNpcID.Num() ||
			i >= pkActivity->ndestMapID.Num())
		{
			continue;
		}

		if (pkActivity->sNpcName(i) == TEXT("None"))
		{
			conditionDesc += pkActivity->sNpcName(i);
		}
		else
		{
			conditionDesc += FString::Printf(TEXT("<font color='#14a800'>%s</font>"), 
				*LinkManager::HtmlLink_NPCNav(pkActivity->sNpcName(i), 
				pkActivity->nAcceptNpcID(i),
				pkActivity->ndestMapID(i),
				FALSE));
			conditionDesc += '\n';
		}
	}

	object->SetPureString(TEXT("activityPosition"), conditionDesc);
	object->SetString(TEXT("describleTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitle3")));
	object->SetString(TEXT("describle"), pkActivity->sDecrible);

	// Item reward
	object->SetString(TEXT("rewardTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitle4")));
}
void ActivityManager::SetFBDetailInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object, INT nFBID)
{
	if (gfxPlayer == 0 || object == 0)
		return;
	const FBActivityData* pkFB = GetFBActivityData(nFBID);
	if (!pkFB)
	{
		ClearFBDetailInfo(gfxPlayer,object);
		return;
	}

	object->SetString(TEXT("activityName"),pkFB->sFBName);
	object->SetString(TEXT("conditionTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitleFB1")));
	//object->SetString(TEXT("conditionTitle"),TEXT("Target BOSS"));
	UNPCTemplate* acceptNpc = NULL;
	if (pkFB->nAcceptNpcID > 0)
	{
		acceptNpc = MMOFacade::getSingleton().FindNPCTemplate(pkFB->nAcceptNpcID);
	}
	object->SetString(TEXT("conditionLimited"), acceptNpc != NULL ? acceptNpc->GetDisplayName() : TEXT(""));
	object->SetString(TEXT("positionTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitleFB2")));

	/*INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	FTeleportLink* teleportLink = UGlobalMapInfo::GetSingleton()->GetMapLink(curMapId, pkFB->ndestMapID);
	INT xPos = 0;INT yPos = 0; INT zPos = 0;
	if(teleportLink)
	{
		xPos = teleportLink->From.Location.X;
		yPos = teleportLink->From.Location.Y;
		zPos = teleportLink->From.Location.Z;
	}*/
	// Quest-giver NPC
	FString conditionDesc = FString::Printf(TEXT("<font color='#14a800'>%s</font>"), *LinkManager::HtmlLink_NPCNav(pkFB->sFBName,pkFB->nAcceptNpcID, pkFB->ndestMapID, TRUE));

	object->SetPureString(TEXT("activityPosition"), conditionDesc);

	object->SetString(TEXT("describleTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitleFB3")));
	object->SetString(TEXT("describle"), pkFB->sDecrible);

	object->SetString(TEXT("rewardTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitleFB4")));
	// Item reward
}
void ActivityManager::ClearActivityDetailInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (gfxPlayer == 0 || object == 0)
		return;

	object->SetString(TEXT("activityName"),TEXT(""));
	object->SetString(TEXT("conditionTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitle1")));
	object->SetString(TEXT("conditionLimited"), TEXT(""));

	// Quest-giver NPC
	object->SetString(TEXT("positionTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitle2")));
	object->SetString(TEXT("activityPosition"), TEXT(""));
	object->SetString(TEXT("describleTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitle3")));
	object->SetString(TEXT("describle"), TEXT(""));
	object->SetString(TEXT("rewardTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitle4")));
}
void ActivityManager::ClearFBDetailInfo(UMMOGFXPlayer* gfxPlayer, UGFxObject* object)
{
	if (gfxPlayer == 0 || object == 0)
		return;

	object->SetString(TEXT("activityName"),TEXT(""));
	object->SetString(TEXT("conditionTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitleFB1")));
	//object->SetString(TEXT("conditionTitle"),TEXT("Target BOSS"));
	object->SetString(TEXT("conditionLimited"), TEXT(""));
	object->SetString(TEXT("positionTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitleFB2")));
	// Quest-giver NPC
	object->SetString(TEXT("activityPosition"), TEXT(""));
	object->SetString(TEXT("describleTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitleFB3")));
	object->SetString(TEXT("describle"), TEXT(""));
	object->SetString(TEXT("rewardTitle"),MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Activity.DetailTitleFB4")));
}
void ActivityManager::SetQuitHintActivityList(UMMOGFXPlayer* gfxPlayer,UGFxObject* object)
{
	if(gfxPlayer == 0 || object == 0)
		return;
	FString strDesc;
	ActivityDataList::iterator iter = _QuitTodayActivityList.begin();
	for (INT n = 0; iter != _QuitTodayActivityList.end(); ++iter)
	{
		UGFxObject *pkData = gfxPlayer->CreateObject("UI_ActivityWidget.ActivityAS3.ActivityData");
		if(pkData == 0)
			continue;

		FString strTime = TEXT("");
		FString strState = TEXT("");
		if (iter->nGuildActivityType > 0)
		{
			_GetGuildActivityTimeString(iter->nGuildActivityType, strTime);
			_GetGuildActivityStateString(iter->nGuildActivityType, strState);
		}
		else
		{
			strTime = iter->nTimeInfo.sTimeStr;
			EActivityState eState = iter->nTimeInfo.GetState();
			_GetActivityStateString(eState, strState);
		}

		pkData->SetString(TEXT("Name"), (*iter).sActivityName);

		FString strComplete = TEXT("--");
		if (iter->nCompleteType == EACPT_None)
		{
			strComplete = TEXT("--");
		} 
		else if (iter->nCompleteType == EACPT_Quest)
		{
			INT nCompleteNum = 0;
			INT nCanComNumMax = iter->nPerDayJoinNum;

			if (iter->nLinkQuestType == E_qt_Reward)
			{
				nCompleteNum = _RewardQuestComNum;
				nCanComNumMax =  MMOQuestData::getSingleton().GetRewardQuestAvailableLimit();
			}
			else if (iter->nLinkQuestType == E_qt_Circle)
			{
				CircleQuestInfo dataInfo;
				MMOQuestData::getSingleton().GetCircleQuestInfo(iter->nLinkQuestID, dataInfo);
				nCompleteNum = dataInfo.CircleRound;
			}
			else if (iter->nLinkQuestType == E_qt_Emegency)
			{
				nCompleteNum = MMOQuestData::getSingleton().GetEmergencyQuestCompleteCount();
				nCanComNumMax = MAX_EMERGENCY_QUEST_COMPLETE_PER_DAY;
			}
			else
			{
				nCompleteNum = MMOQuestData::getSingleton().GetCompletedQuestCount(iter->nLinkQuestID);
			}

			if (nCompleteNum >= nCanComNumMax)
			{
				nCompleteNum = nCanComNumMax;
				strState = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint5"));
			}

			strComplete = FString::Printf(TEXT("%d / %d"),nCompleteNum,nCanComNumMax);
		}
		else if (iter->nCompleteType == EACPT_Time)
		{
			INT nCompleteNum = (iter->nTimeInfo.GetState() == EActivityOver) ? 1 : 0;
			INT nCanComNumMax = 1;
			strComplete = FString::Printf(TEXT("%d / %d"),nCompleteNum,nCanComNumMax);
		}
		else if (iter->nCompleteType == EACPT_SignIn)
		{
			const SSignInRewardState &curSignInState = MMODailySignInManager::getSingleton().GetMyCurSignInState();
			INT nCompleteNum = curSignInState._CanSignIn ? 0 : 1;
			INT nCanComNumMax = 1;

			if (nCompleteNum >= nCanComNumMax)
			{
				nCompleteNum = nCanComNumMax;
				strState = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint5"));
			}

			strComplete = FString::Printf(TEXT("%d / %d"),nCompleteNum,nCanComNumMax);
		}
		else if (iter->nCompleteType == EACPT_Variable)
		{
			INT nCompleteNum = _GetVariableCompleteCount(vtRole, iter->nRoleVariable);
			INT nCanComNumMax = iter->nPerDayJoinNum;

			if (nCompleteNum >= nCanComNumMax)
			{
				nCompleteNum = nCanComNumMax;
				strState = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Activity.TimeHint5"));
			}

			strComplete = FString::Printf(TEXT("%d / %d"), nCompleteNum, nCanComNumMax);
		}
		
		if (!(CMessageHandler::GetInstance().GetClientOption() & CLIENT_OPTION_BIT_ACTIVITY_VALUE))
		{
			strComplete = TEXT("--");
		}

		pkData->SetString(TEXT("State"),strState);
		pkData->SetString(TEXT("CompleteState"), strComplete);
		pkData->SetInt(TEXT("Activity"),(*iter).nRewardInfo.nRewardVitality);

		for (INT i = 0; i < (*iter).sNpcName.Num(); ++i)
		{
			if (i >= (*iter).nAcceptNpcID.Num() ||
				i >= (*iter).ndestMapID.Num())
			{
				continue;
			}

			strDesc += FString::Printf(TEXT(" <font color='#14a800'>%s</font> "),
				*LinkManager::HtmlLink_NPCNav((*iter).sNpcName(i), (*iter).nAcceptNpcID(i), (*iter).ndestMapID(i), TRUE));
		}

		pkData->SetPureString(TEXT("Position"), strDesc);
		object->SetElementObject(n++,pkData);
	}
}
void ActivityManager::SetRewardBarInfo(UMMOGFXPlayer* gfxPlayer,UGFxObject* object)
{
	if(gfxPlayer == 0 || object == 0)
		return;
	RewardBarDataList::iterator itr = _RewardBarDataList.begin();
	for (INT n = 0; itr != _RewardBarDataList.end(); ++itr)
	{
		UGFxObject *pkData = gfxPlayer->CreateObject("UI_ActivityWidget.ActivityAS3.ActivityRewardBarItemData");
		if(pkData == 0)
			continue;
		pkData->SetInt("Pos", itr->nVitality);
		pkData->SetInt("RewardID",itr->nRewardItemID);
		pkData->SetInt("nType",itr->nBoxLevel);
		pkData->SetInt("nIndex",n);
		//pkData->SetInt("IconName",itr->nVitality);
		pkData->SetInt("nState",GetRewardState(n));
		object->SetElementObject(n++,pkData);
	}
}

INT ActivityManager::GetActivityTeamLevel(INT nTeamId)
{
	INT Ret = 0;
	INT Count = 1;
	for(INT i = 0; i < _ActivityTeamListData.Num(); ++i)
	{
		if(_ActivityTeamListData(i).TeamId == nTeamId)
		{
			Count = _ActivityTeamListData(i).PlayerCount;
			for(INT k = 0; k < Count; ++k)
			{
				PlayerBaseInfo* pPlayerBaseInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo(_ActivityTeamListData(i).MemberId[k]);
				if(pPlayerBaseInfo)
				{
					Ret += pPlayerBaseInfo->_Level;
				}
			}

			break;
		}
	}
	
	return (INT)(Ret / Count);
}

INT ActivityManager::GetActiveValue()
{
	return _ActiveValue;
}
INT ActivityManager::GetRewardState(INT nIndex)
{
	//0 is not claimable, 1 is claimable but not yet claimed, 2 is already claimed
	INT nState = -1;
	INT nHave = _RewardBit&(0x0001<<(nIndex + 1));
	if (nHave == 0)
	{
		//0 not claimed today _RewardBit
		const RewardBarItem* pkRew = GetRewardBarCondition(nIndex);
		if(pkRew)
		{
			if(_ActiveValue >= pkRew->nVitality)
				nState = 1;
			else
				nState = 0;
		}
	}
	else if(nHave > 0)
	{
		//1 claimed today _RewardBit
		nState = 2;
	}
	return nState;
}
const RewardBarItem* ActivityManager::GetRewardBarCondition(INT nIndex)const
{
	RewardBarDataList::const_iterator itr = _RewardBarDataList.begin();
	for (INT n = 0; itr != _RewardBarDataList.end(); ++itr,n++)
	{
		if( n == nIndex)
		{
			return &(*itr);
		}
	}
	return 0;
}
void ActivityManager::AddActivityData(ActivityData& data)
{
	INT nType = 0;
 
	_ActivityDataList.push_back(data);
}

void ActivityManager::AddActivityRewardBarData(RewardBarItem& data)
{
	//if(data.nVitality >= _MaxActiveValue)
	//	_MaxActiveValue = data.nVitality + 30;
	if(data.nVitality >= _MaxActiveValue)
		_MaxActiveValue = data.nVitality;
	_RewardBarDataList.push_back(data);
}
INT ActivityManager::GetMaxActiveValue()
{
	return _MaxActiveValue;	 
}

INT ActivityManager::GetCurActivityId()
{
	return _CurActivityId;
}

void ActivityManager::SetCurActivityId(INT nActivityId)
{
	if(nActivityId > 0)
		_CurActivityId = nActivityId;
}

INT ActivityManager::GetCurActivityTeamPage()
{
	return _CurActivityTeamPage;
}

void ActivityManager::SetCurActivityTeamPage(INT nPage)
{
	_CurActivityTeamPage = nPage;
}

INT ActivityManager::GetSelectedActivityTeam()
{
	return _CurActivityTeamIndex;
}

void ActivityManager::SetSelectedActivityTeam(INT nIndex)
{
	_CurActivityTeamIndex = nIndex;
}

INT ActivityManager::GetActivityFilterType()
{
	return _CurShowType;
}

INT ActivityManager::GetActivityTipsTeamId()
{
	return _CurActivityTipsTeamId;
}

void ActivityManager::ClearActivityTeamList()
{
	_ActivityTeamListData.Empty(MAX_TEAM_COUNT_PER_PAGE);
}

void ActivityManager::UpdateActivityTeamList(const char* buf)
{
	BOOL bSingle = FALSE;
	if(_CurShowType != (INT)EActivityCopy)
	{
		const ActivityData* pkActivity = GetActivityData(_CurActivityId);
		if(pkActivity&&pkActivity->nPersonNumMin <= 1)
			bSingle = TRUE;
	}
	else
	{
		const FBActivityData*	pkFB = 	GetFBActivityData(_CurActivityId);
		if(pkFB&&pkFB->nPersonNumMin <= 1)
			bSingle = TRUE;
	}
	_ActivityTeamListData.Empty(MAX_TEAM_COUNT_PER_PAGE);
	if(!bSingle)
	{
		M2C_ACTIVITY_QUERY_TEAM* msg = (M2C_ACTIVITY_QUERY_TEAM*)buf;
		for(INT i = 0; i < MAX_TEAM_COUNT_PER_PAGE; ++i)
		{
			if(msg->stRoomTeam[i].nTeamID <= 0)
				continue;
			ActivityTeam team;
			team.TeamId = msg->stRoomTeam[i].nTeamID;
			team.LeaderId = msg->stRoomTeam[i].dwLeaderID;
			//_ActivityTeamListData(i).Level = msg->stRoomTeam[i].nTeamLevel;
			team.Name = UTF8_TO_TCHAR(msg->stRoomTeam[i].szTeamName);
			INT MemberNum = 0;
			for(INT k = 0; k < MAX_TEAM_MEMBER_COUNT; ++k)
			{
				if(msg->stRoomTeam[i].nMemberID[k] > 0)
				{
					team.MemberId[k] = msg->stRoomTeam[i].nMemberID[k];
					++MemberNum;
				}
			}
			team.PlayerCount = MemberNum;
			_ActivityTeamListData.AddItem(team);
		}
	}

	MMOGameEvent_Activity evt(MMOGameEvent_Activity::EActivity_UpdateActivityTeamList);
	MMOUI::PostGameEvent(&evt);
}
const ActivityTeam*	ActivityManager::GetRandTeamCurActivity() const
{
	INT nMaxNum = _ActivityTeamListData.Num();
	if(nMaxNum > 0)
	{
		INT nRand = appRand()%nMaxNum;
		nRand = Clamp(nRand,0,nMaxNum);
		return &_ActivityTeamListData(nRand);
	}
	return NULL;
}

UBOOL ActivityManager::IsActivity(INT questId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return FALSE;
	}

	ActivityDataList::iterator iter = _ActivityDataList.begin();
	for (; iter != _ActivityDataList.end(); ++ iter)
	{
		if (iter->nLinkQuestType == quest->QuestType)
		{
			if (iter->nLinkQuestType == E_qt_Reward)
			{
				return TRUE;
			}
			else if (iter->nLinkQuestType == E_qt_Circle)
			{
				if (iter->nLinkQuestID == quest->CircleID)
				{
					return TRUE;
				}
			}
			else
			{
				if (iter->nLinkQuestID == quest->GetTemplateID())
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

INT ActivityManager::GetActivityVipLevel(INT questId)
{
	const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(questId);
	if (!quest)
	{
		return 0;
	}

	ActivityDataList::iterator iter = _ActivityDataList.begin();
	for (; iter != _ActivityDataList.end(); ++ iter)
	{
		if (iter->nLinkQuestType == quest->QuestType)
		{
			if (iter->nLinkQuestType == E_qt_Reward)
			{
				return iter->nVipLevel;
			}
			else if (iter->nLinkQuestType == E_qt_Circle)
			{
				if (iter->nLinkQuestID == quest->CircleID)
				{
					return iter->nVipLevel;
				}
			}
			else
			{
				if (iter->nLinkQuestID == quest->GetTemplateID())
				{
					return iter->nVipLevel;
				}
			}
		}
	}

	return 0;
}

ActivityTeam* ActivityManager::GetActivityTeamById(INT TeamId)
{
	for(INT i = 0; i < _ActivityTeamListData.Num(); ++i)
	{
		if(_ActivityTeamListData(i).TeamId == TeamId)
			return &_ActivityTeamListData(i);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
ActivityData* ActivityDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	//No need for new
	needAddToMap = false;
	return NULL;
}

void ActivityDataProcessor::DestroyDataObject(ActivityData* obj)
{
	if(obj)
	{
		delete obj;
		obj = NULL;
	}
}

void ActivityDataProcessor::ParseData(ActivityData* obj, class CTabFile* dataRow, int rowIdx)
{
	ActivityData data;
	data.nActivityID							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("activityID"), dataRow, rowIdx);	
	if(data.nActivityID == 0)
		return;
	
	data.nLinkQuestType							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("linkQuestType"), dataRow, rowIdx);
	data.nLinkQuestID							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("linkQuestID"), dataRow, rowIdx);
	data.sActivityName							= TReadDataValue_FString<MMOActivityDataManager>(TEXT("activityName"), dataRow, rowIdx);
	data.nVipLevel								= TReadDataValue_Int<MMOActivityDataManager>(TEXT("vipLevel"), dataRow, rowIdx);
	data.nCheckLevelMin							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("checkLevelMin"), dataRow, rowIdx);
	data.nCheckLevelMax							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("checkLevelMax"), dataRow, rowIdx);
	data.nHotStarNum							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("hotStarNum"), dataRow, rowIdx);
	data.nPersonNumMin							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("personNumMin"), dataRow, rowIdx);
	data.nPersonNumMax							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("personNumMax"), dataRow, rowIdx);
	//data.nHaveDoneTimes						= TReadDataValue_Int<MMOActivityDataManager>(TEXT("Level"), dataRow, rowIdx);
	data.nPerDayJoinNum							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("perDayJoinNum"), dataRow, rowIdx);
	data.nCompleteType							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("completeType"), dataRow, rowIdx);
	data.nCompleteMap							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("completeMap"), dataRow, rowIdx);
	//data.nActivityType						= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardVitality"), dataRow, rowIdx);
	INT	nType1									= TReadDataValue_Int<MMOActivityDataManager>(TEXT("IsRewardExp"), dataRow, rowIdx);
	INT	nType2									= TReadDataValue_Int<MMOActivityDataManager>(TEXT("IsRewardMoney"), dataRow, rowIdx);
	INT	nType3									= TReadDataValue_Int<MMOActivityDataManager>(TEXT("IsRewardTreasure"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[0]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID1"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[1]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID2"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[2]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID3"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[3]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID4"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[4]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID5"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[5]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID6"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[6]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID7"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[7]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID8"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[8]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID9"), dataRow, rowIdx);
	data.nRewardInfo.nArrItemID[9]				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardItemID10"), dataRow, rowIdx);

	data.nGuildActivityType						= TReadDataValue_Int<MMOActivityDataManager>(TEXT("linkGuildActivityType"), dataRow, rowIdx);
	data.nRoleVariable							= TReadDataValue_Int<MMOActivityDataManager>(TEXT("roleVariable"), dataRow, rowIdx);

	FString dataStr								= TReadDataValue_FString<MMOActivityDataManager>(TEXT("timeLimit"), dataRow, rowIdx);
	TArray<FString> dataArray;
	dataStr.ParseIntoArray(&dataArray, TEXT(","), 0);
	for (INT i = 0; i < dataArray.Num(); ++ i)
	{
		data.aTimeLimit.AddItem(appAtoi(*(dataArray(i))));
	}

	data.nTimeInfo.sTimeStr						= TReadDataValue_FString<MMOActivityDataManager>(TEXT("timeString"), dataRow, rowIdx);
	data.nTimeInfo.nStartTime_year				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("starYear"), dataRow, rowIdx);
	data.nTimeInfo.nStartTime_month				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("startMonth"), dataRow, rowIdx);
	data.nTimeInfo.nStartTime_day				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("startDay"), dataRow, rowIdx);
	data.nTimeInfo.nStartTime_weekDay			= TReadDataValue_Int<MMOActivityDataManager>(TEXT("startWeekDay"), dataRow, rowIdx);
	data.nTimeInfo.nStartTime_hour				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("startHour"), dataRow, rowIdx);
	data.nTimeInfo.nStartTime_minute			= TReadDataValue_Int<MMOActivityDataManager>(TEXT("startMinute"), dataRow, rowIdx);
	data.nTimeInfo.nEndTime_year				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("endYear"), dataRow, rowIdx);
	data.nTimeInfo.nEndTime_month				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("endMonth"), dataRow, rowIdx);
	data.nTimeInfo.nEndTime_day					= TReadDataValue_Int<MMOActivityDataManager>(TEXT("endDay"), dataRow, rowIdx);
	data.nTimeInfo.nEndTime_weekDay				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("endWeekDay"), dataRow, rowIdx);
	data.nTimeInfo.nEndTime_hour				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("endHour"), dataRow, rowIdx);
	data.nTimeInfo.nEndTime_minute				= TReadDataValue_Int<MMOActivityDataManager>(TEXT("endMinute"), dataRow, rowIdx);

	dataStr										= TReadDataValue_FString<MMOActivityDataManager>(TEXT("acceptNpcName"), dataRow, rowIdx);
	dataStr.ParseIntoArray(&dataArray, TEXT(","), 0);
	for (INT i = 0; i < dataArray.Num(); ++i)
	{
		data.sNpcName.AddItem(dataArray(i));
	}

	dataStr = TReadDataValue_FString<MMOActivityDataManager>(TEXT("acceptNpcID"), dataRow, rowIdx);
	dataStr.ParseIntoArray(&dataArray, TEXT(","), 0);
	for (INT i = 0; i < dataArray.Num(); ++i)
	{
		data.nAcceptNpcID.AddItem(appAtoi(*(dataArray(i))));
	}

	dataStr = TReadDataValue_FString<MMOActivityDataManager>(TEXT("destMapId"), dataRow, rowIdx);
	dataStr.ParseIntoArray(&dataArray, TEXT(","), 0);
	for (INT i = 0; i < dataArray.Num(); ++i)
	{
		data.ndestMapID.AddItem(appAtoi(*(dataArray(i))));
	}

	data.sLevelString							= TReadDataValue_FString<MMOActivityDataManager>(TEXT("levelString"), dataRow, rowIdx);
	data.sPersonString							= TReadDataValue_FString<MMOActivityDataManager>(TEXT("personString"), dataRow, rowIdx);
	data.sDecrible								= TReadDataValue_FString<MMOActivityDataManager>(TEXT("activityDecs"), dataRow, rowIdx);
	data.nRewardInfo.nRewardVitality			= TReadDataValue_Int<MMOActivityDataManager>(TEXT("rewardVitality"), dataRow, rowIdx);
	if(nType1 > 0)
		data.nRewardInfo.nRewardType += 1;
	if(nType2 > 0)
		data.nRewardInfo.nRewardType += 2;
	if(nType3 > 0)
		data.nRewardInfo.nRewardType += 4;
	ActivityManager::getSingleton().AddActivityData(data);
}

void ActivityDataProcessor::PostInitData()
{
}


RewardBarItem* ActivityRewardBarDataProcessor::CreateDataObject(INT templateID, bool& needAddToMap)
{
	needAddToMap = false;
	return NULL;
}

void ActivityRewardBarDataProcessor::DestroyDataObject(RewardBarItem* obj)
{
	if(obj)
	{
		delete obj;
		obj = NULL;
	}
}

void ActivityRewardBarDataProcessor::ParseData(RewardBarItem* obj, class CTabFile* dataRow, int rowIdx)
{
	RewardBarItem data;
	data.nID				= TReadDataValue_Int<MMOActivityRewardBarDataManager>(TEXT("ID"), dataRow, rowIdx);
	data.nVitality			= TReadDataValue_Int<MMOActivityRewardBarDataManager>(TEXT("ActivityValue"), dataRow, rowIdx);
	data.nRewardItemID		= TReadDataValue_Int<MMOActivityRewardBarDataManager>(TEXT("RewardItemID"), dataRow, rowIdx);
	data.nRewardItemCount	= TReadDataValue_Int<MMOActivityRewardBarDataManager>(TEXT("RewardItemCount"), dataRow, rowIdx);
	data.nBoxLevel		    = TReadDataValue_Int<MMOActivityRewardBarDataManager>(TEXT("BoxLevel"), dataRow, rowIdx);
	ActivityManager::getSingleton().AddActivityRewardBarData(data);
}

void ActivityRewardBarDataProcessor::PostInitData()
{
}
