#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOAchievementManager.h"
#include <time.h>
#include "MMOGuideManager.h"
#include "MMOJourneyManager.h"
#include "MMOGuildData.h"

IMPLEMENT_CLASS(UMMOUIAchievementPanel)

void UMMOUIAchievementPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Achievement);
	EventAdapter = new TGameEventAdapter<UMMOUIAchievementPanel>(this, evts);
}

void UMMOUIAchievementPanel::UnInit()
{
	Super::UnInit();
}

void UMMOUIAchievementPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);

	if(v)
	{
		if (MMOAchievementManager::getSingleton().IsCompare())
		{
			eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.CompareAchievement")));
		} 
		else
		{
			eventCallSetWindowTitle(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.Achievement")));
		}

		UBOOL hasGuild = MMOGuildData::getSingleton().GetGuildBaseInfo().Id > 0;
		eventCallHasGuild(hasGuild);
		eventCallSetState(MMOAchievementManager::getSingleton().IsCompare());

		if (MMOAchievementManager::getSingleton().IsCompare())
		{
			UpdateAchievementCagetory(AT_NORMAL);
			UpdateAchievementSummary(AT_NORMAL);
			UpdateAchievementData(0, 1);
			UpdateAchievementList(0, 1);
		}
		else
		{
			if (_GetRewardAchievementId > 0)
			{
				ShowAchievementListById(_GetRewardAchievementId);
				_GetRewardAchievementId = 0;
			}
			else
			{
				if (_CurBaseClass == AC_GUILD1 ||
					_CurBaseClass == AC_GUILD2)
				{
					UpdateAchievementCagetory(AT_GUILD);
					UpdateAchievementSummary(AT_GUILD);
				}
				else
				{
					UpdateAchievementCagetory(AT_NORMAL);
					UpdateAchievementSummary(AT_NORMAL);
				}
				UpdateAchievementData(_CurBaseClass, _CurSubClass);
				UpdateAchievementList(_CurBaseClass, _CurSubClass);
			}
		}

		// if still in a guild last time, case where the guild was disbanded
		if (!hasGuild)
		{
			if (_CurBaseClass == AC_GUILD1 ||
				_CurBaseClass == AC_GUILD2)
			{
				_CurBaseClass = 0;
				_CurSubClass = 0;
				UpdateAchievementCagetory(AT_NORMAL);
				UpdateAchievementSummary(AT_NORMAL);
				UpdateAchievementData(0, 1);
				UpdateAchievementList(0, 1);
			}
		}

		UMMOUIManager* uiManager = MMOUI::GetUIManager();
		if (uiManager)
		{
			uiManager->delegateUpdateMenuEffect(1, EMenuType_Child_Achievement, TRUE, FALSE);
		}

		MMOGameEvent_Guide evtGuide(EGUET_ShowAchievementPanel);
		MMOUI::PostGameEvent(&evtGuide);
	}
	else
	{
		MMOAchievementManager::getSingleton().SetCompare(FALSE);
	}
}

void UMMOUIAchievementPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
{
	if (!IsVisible)
	{
		return;
	}

	if (gameEvent->GetEventType() != GameEvent_Achievement)
	{
		return;
	}

	MMOGameEvent_Achievement* achEvent = (MMOGameEvent_Achievement*)gameEvent;
	if (achEvent == NULL)
	{
		return;
	}

	switch(achEvent->_EventType)
	{
	case MMOGameEvent_Achievement::EAET_UpdateAchievementList:
		{
			UINT32 baseCount = MMOAchievementData::getSingleton().GetBaseClassAchievementCompleteCount(_CurBaseClass);
			UINT32 subCount = MMOAchievementData::getSingleton().GetSubClassAchievementCompleteCount(_CurBaseClass, _CurSubClass);
			eventCallUpdateCagetoryComplete(_CurBaseClass, baseCount, _CurSubClass, subCount);

			UpdateAchievementData(_CurBaseClass, _CurSubClass);
			UpdateAchievementList(_CurBaseClass, _CurSubClass);
		}
		break;
	}
}

void UMMOUIAchievementPanel::ShowAchievementListByClass(INT baseClass, INT subClass)
{
	MMOAchievementManager::getSingleton().SetCompare(FALSE);
	if (baseClass == AC_GUILD1 ||
		baseClass == AC_GUILD2)
	{
		UpdateAchievementCagetory(AT_GUILD);
	}
	else
	{
		UpdateAchievementCagetory(AT_NORMAL);
	}
	UpdateAchievementData(baseClass, subClass);
	UpdateAchievementList(baseClass, subClass);

	eventCallSetState(FALSE);
	eventCallShowAchievementList();
}

void UMMOUIAchievementPanel::ShowAchievementListById(INT id)
{
	if (id < ACHIEVEMENT_MAX)
	{
		AchievementData data;
		if (MMOAchievementData::getSingleton().GetCommonAchievementData(LOWORD(id), data))
		{
			UpdateAchievementCagetory(AT_NORMAL);
			UpdateAchievementData(data.BaseClass, data.SubClass);
			UpdateAchievementList(data.BaseClass, data.SubClass);
		}
	}
	else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
	{
		UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(id);
		GuildAchievementData data;
		if (MMOAchievementData::getSingleton().GetGuildAchievementData(dataId, data))
		{
			UpdateAchievementCagetory(AT_GUILD);
			UpdateAchievementData(data.BaseClass, data.SubClass);
			UpdateAchievementList(data.BaseClass, data.SubClass);
		}
	}
	else
	{
		UINT16 killId = HIWORD(id);
		UINT16 killIndex = LOWORD(id);
		KillNpcAchievementData data;
		if (MMOAchievementData::getSingleton().GetKillNpcAchievementData(killId, data))
		{
			UpdateAchievementCagetory(AT_NORMAL);
			UpdateAchievementData(data.BaseClass, data.SubClass);
			UpdateAchievementList(data.BaseClass, data.SubClass);
		}	
	}

	MMOAchievementManager::getSingleton().SetCompare(FALSE);
	eventCallSetState(FALSE);
	eventCallShowAchievementListItem(id);
}

void UMMOUIAchievementPanel::UpdateAchievementCagetory(int type)
{
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	INT objectIndex = 0;
	UGFxObject* object = GFXPlayer->CreateObject("ActionScript3.Achievement.AchievementData");
	object->SetString(TEXT("label"), MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Summary")));
	object->SetInt(TEXT("index"), objectIndex);
	object->SetInt(TEXT("type"), 0);
	object->SetInt(TEXT("deepth"), 0);
	object->SetBool(TEXT("visible"), true);
	object->SetInt(TEXT("baseClass"), AC_TOTAL);
	object->SetInt(TEXT("subClass"), AC_TOTAL);
	object->SetString(TEXT("percent"), "");
	dataProvider->SetElementObject(objectIndex, object);
	objectIndex ++;

	INT classBegin = AC_TOTAL;
	INT classEnd = AC_TOTAL;

	if (type == AT_NORMAL)
	{
		classBegin = AC_ROLE;
		classEnd = AC_EPIC;
	} 
	else
	{
		classBegin = AC_GUILD1;
		classEnd = AC_GUILD2;
	}

	for (INT i = classBegin; i <= classEnd; ++ i)
	{
		UBOOL findLoc = FALSE;
		FString baseClassName = MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.Achievement.")) + 
			MMOAchievementData::S_AchievementClassName[i] + appItoa(0), findLoc);
		
		// returning "?" means invalid Key
		// INT errorPos = baseClassName.InStr(TEXT("?"));
		if (!findLoc)
		{
			continue;
		}

		UGFxObject* baseClassObject = GFXPlayer->CreateObject(TEXT("ActionScript3.Achievement.AchievementData"));
		if (baseClassObject)
		{
			UINT32 completeCount = MMOAchievementData::getSingleton().GetBaseClassAchievementCompleteCount(i);
			UINT32 totalCount = MMOAchievementData::getSingleton().GetBaseClassAchievementTotalCount(i);
			// #19469
			FString percent = FString::Printf(TEXT("%d"), completeCount);

			baseClassObject->SetString(TEXT("label"), baseClassName);
			baseClassObject->SetInt(TEXT("index"), objectIndex);
			baseClassObject->SetInt(TEXT("type"), 1);
			baseClassObject->SetInt(TEXT("deepth"), 0);
			baseClassObject->SetBool(TEXT("visible"), true);
			baseClassObject->SetInt(TEXT("baseClass"), i);
			baseClassObject->SetInt(TEXT("subClass"), 0);
			baseClassObject->SetString(TEXT("percent"), percent);
			dataProvider->SetElementObject(objectIndex, baseClassObject);
			objectIndex ++;
		}

		INT subClassIndex = 1;
		FString subClassName = MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.Achievement.")) + 
			MMOAchievementData::S_AchievementClassName[i] + appItoa(subClassIndex), findLoc);

		while (findLoc)
		{
			UGFxObject* subClassObject = GFXPlayer->CreateObject(TEXT("ActionScript3.Achievement.AchievementData"));
			if (subClassObject)
			{
				UINT32 completeCount = MMOAchievementData::getSingleton().GetSubClassAchievementCompleteCount(i, subClassIndex);
				UINT32 totalCount = MMOAchievementData::getSingleton().GetSubClassAchievementTotalCount(i, subClassIndex);
				// #19469
				FString percent = FString::Printf(TEXT("(%d)"), completeCount);

				subClassObject->SetString(TEXT("label"), subClassName);
				subClassObject->SetInt(TEXT("index"), objectIndex);
				subClassObject->SetInt(TEXT("type"), 1);
				subClassObject->SetInt(TEXT("deepth"), 1);
				subClassObject->SetBool(TEXT("visible"), true);
				subClassObject->SetInt(TEXT("baseClass"), i);
				subClassObject->SetInt(TEXT("subClass"), subClassIndex);
				subClassObject->SetString(TEXT("percent"), percent);
				dataProvider->SetElementObject(objectIndex, subClassObject);
				objectIndex ++;
			}

			subClassIndex ++;
			subClassName = MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.Achievement.")) + 
				MMOAchievementData::S_AchievementClassName[i] + appItoa(subClassIndex), findLoc);
		}
	}
	
	eventCallUpdateCagetoryData(dataProvider);
	eventCallSetTab(type);
}

void UMMOUIAchievementPanel::UpdateAchievementSummary(int type)
{
	UGFxObject* dataSub = GFXPlayer->CreateArray();
	if (!dataSub)
	{
		return;
	}

	INT classBegin = AC_TOTAL;
	INT classEnd = AC_TOTAL;

	if (type == AT_NORMAL)
	{
		classBegin = AC_ROLE;
		classEnd = AC_EPIC;
	} 
	else
	{
		classBegin = AC_GUILD1;
		classEnd = AC_GUILD2;
	}

	UINT32 curTotal = 0;
	UINT32 maxTotal = 0;

	int index = 0;
	for (INT i = classBegin; i <= classEnd; ++ i )
	{
		UBOOL findLoc = FALSE;
		FString baseClassName = MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.Achievement.")) + 
			MMOAchievementData::S_AchievementClassName[i] + appItoa(0), findLoc);

		//INT errorPos = baseClassName.InStr(TEXT("?"));
		if (!findLoc)
		{
			continue;
		}

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetInt(TEXT("summary_base_class"), i);
			object->SetString(TEXT("summary_sub_name"), baseClassName);
			UINT32 curCount = MMOAchievementData::getSingleton().GetBaseClassAchievementCompleteCount(i);
			object->SetInt(TEXT("summary_sub_cur"), curCount);
			UINT32 maxCount = MMOAchievementData::getSingleton().GetBaseClassAchievementTotalCount(i);
			maxCount = maxCount == 0 ? 1 : maxCount;
			object->SetInt(TEXT("summary_sub_max"), maxCount);

			curTotal += curCount;
			maxTotal += maxCount;

			dataSub->SetElementObject(index, object);
			index ++;
		}
	}

	FString recentTitle = MMOUI::ParseLocalizedPropertyPath(FString(TEXT("MMOGame.Achievement.Recent")));
	UGFxObject* dataRecent = GFXPlayer->CreateArray();
	if (!dataRecent)
	{
		return;
	}

	INT recentId[MAX_RECENT_COMPLETE_COUNT];
	INT recentDate[MAX_RECENT_COMPLETE_COUNT];
	MMOAchievementData::getSingleton().GetAchievementRecentComplete(recentId, recentDate);

	index = 0;
	for (INT i = 0; i < MAX_RECENT_COMPLETE_COUNT; ++ i)
	{
		UINT32 id = recentId[i];
		if (id == 0)
		{
			continue;
		}

		FString icon = TEXT("");
		FString name = TEXT("");
		UINT32 baseClass = 0;
		UINT32 subClass = 1;

		if (id < ACHIEVEMENT_MAX)
		{
			AchievementData data;
			if (MMOAchievementData::getSingleton().GetCommonAchievementData(LOWORD(id), data))
			{
				icon = data.Icon;
				name = data.Name;
				baseClass = data.BaseClass;
				subClass = data.SubClass;
			}
		}
		else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
		{
			UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(id);
			GuildAchievementData data;
			if (MMOAchievementData::getSingleton().GetGuildAchievementData(dataId, data))
			{
				icon = data.Icon;
				name = data.Name;
				baseClass = data.BaseClass;
				subClass = data.SubClass;
			}
		}
		else
		{
			UINT16 killId = HIWORD(id);
			UINT16 killIndex = LOWORD(id);
			KillNpcAchievementData data;
			if (MMOAchievementData::getSingleton().GetKillNpcAchievementData(killId, data))
			{
				icon = data.Icon;
				name = data.Name[killIndex];
				baseClass = data.BaseClass;
				subClass = data.SubClass;
			}
		}

		if (type == AT_NORMAL)
		{
			if (baseClass >= AC_GUILD1 && baseClass <= AC_GUILD2)
			{
				continue;
			}
		} 
		else
		{
			if (baseClass >= AC_ROLE && baseClass <= AC_EPIC)
			{
				continue;
			}
		}

		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetInt(TEXT("summary_recent_id"), id);
			object->SetString(TEXT("summary_recent_icon"), icon);
			object->SetString(TEXT("summary_recent_name"), name);

			FString obtainDate = TEXT("");
			__time32_t localTime = recentDate[i] * 86400;
			struct tm *date = _localtime32(&localTime); 
			if (date)
			{
				obtainDate = recentDate[i] > 0 ? FString::Printf(TEXT("%d/%d/%d"), date->tm_year + 1900, date->tm_mon + 1, date->tm_mday) : TEXT("");
			}
			object->SetString(TEXT("summary_recent_date"), obtainDate);
			dataRecent->SetElementObject(index, object);
			index ++;
		}
	}

	eventCallUpdateAchievementSummaryData(curTotal, maxTotal, dataSub, recentTitle, dataRecent);
}

void UMMOUIAchievementPanel::UpdateAchievementData(INT baseClass, INT subClass)
{
	// points, title, completion progress
	UBOOL isCompare = MMOAchievementManager::getSingleton().IsCompare();
	FString roleName = TEXT("");
	AMMOActor* actor = MMOFacade::getSingleton().GetMainActor();
	if (actor)
	{
		roleName = actor->GetActorName();// >ActorName;
	}
	UINT32 point = MMOAchievementData::getSingleton().GetAchievementPoint();
	UINT32 complete = MMOAchievementData::getSingleton().GetSubClassAchievementCompleteCount(baseClass, subClass);
	UINT32 total = MMOAchievementData::getSingleton().GetSubClassAchievementTotalCount(baseClass, subClass);
	total = total == 0 ? 1 : total;

	FString roleNameComp = isCompare ? MMOAchievementManager::getSingleton().GetRoleNameCompare() : TEXT("");
	UINT32 pointComp = isCompare ? MMOAchievementData::getSingleton().GetAchievementPointCompare() : 0;
	UINT32 completeComp = isCompare ? MMOAchievementData::getSingleton().GetSubClassAchievementCompleteCountCompare(baseClass, subClass) : 0;
	UINT32 totalComp = isCompare ? MMOAchievementData::getSingleton().GetSubClassAchievementTotalCountCompare(baseClass, subClass) : 0;
	totalComp = totalComp == 0 ? 1 : totalComp;
	eventCallUpdateAchievementData(roleName, point, complete, total, roleNameComp, pointComp, completeComp, totalComp);
}

void UMMOUIAchievementPanel::_UpdateCommonAchievementInfo(UGFxObject* dataProvider, int& index, UINT32 id)
{
	if (!dataProvider)
	{
		return;
	}

	AchievementData data;
	if (!MMOAchievementData::getSingleton().GetCommonAchievementData(LOWORD(id), data))
	{
		return;
	}

	AchievementInfo info;
	if (!MMOAchievementData::getSingleton().GetAchievementInfo(id, info))
	{
		return;
	}

	// hidden achievement, not obtained, do not show
	if (data.Hide && !info.IsObtain)
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (!object)
	{
		return;
	}

	// basic info
	object->SetInt(TEXT("achievement_id"), data.Id);
	object->SetBool(TEXT("achievement_is_obtain"), info.IsObtain);
	object->SetString(TEXT("achievement_icon"), data.Icon);
	object->SetString(TEXT("achievement_name"), data.Name);
	object->SetString(TEXT("achievement_desc"), data.Desc);
	object->SetInt(TEXT("achievement_point"), data.Point);

	FString ObtainDate = TEXT("");
	__time32_t localTime = info.Date * 86400;
	struct tm *date = _localtime32(&localTime); 
	if (date)
	{
		ObtainDate = info.IsObtain ? FString::Printf(TEXT("%d/%d/%d"), date->tm_year + 1900, date->tm_mon + 1, date->tm_mday) : TEXT("");
	}
	object->SetString(TEXT("achievement_date"), ObtainDate);

	// whether the prerequisite achievement is obtained
	UBOOL isPreObtain1 = data.PreAchievement[0] == 0 || MMOAchievementData::getSingleton().IsAchievementObtain(data.PreAchievement[0]);
	UBOOL isPreObtain2 = data.PreAchievement[1] == 0 || MMOAchievementData::getSingleton().IsAchievementObtain(data.PreAchievement[1]);
	object->SetBool(TEXT("achievement_is_pre_obtain"), isPreObtain1 && isPreObtain2);

	// reward info
	// since there is currently only one reward ui (item or title), use the first one for now
	object->SetInt(TEXT("achievement_reward_type"), data.RewardType[0]);
	object->SetString(TEXT("achievement_reward_item"), TEXT(""));
	object->SetString(TEXT("achievement_reward_title"), TEXT(""));
	if (data.RewardType[0] == adtItem)
	{
		UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.RewardValue[0]);
		if (item)
		{
			//FString iconName = FString::Printf(TEXT("img://%s"), *MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));
			object->SetInt(TEXT("achievement_reward_id"), data.RewardValue[0]);
			object->SetString(TEXT("achievement_reward_icon"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));
		}
	}
	else if (data.RewardType[0] == adtTitle)
	{
		TitleData* title = MMOFacade::getSingleton().GetTitleData(data.RewardValue[0]);
		if (title)
		{
			FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Title"))), *(title->TitleName));
			object->SetString(TEXT("achievement_reward_title"), text);
		}
	}
	else if (data.RewardType[0] == adtExp)
	{
		FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Exp"))), data.RewardValue[0]);
		object->SetString(TEXT("achievement_reward_exp"), text);
	}
	object->SetBool(TEXT("achievement_is_get_reward"), info.IsGetReward);

	// detail info
	object->SetInt(TEXT("achievement_detail"), data.DetailType);
	if (data.DetailType == ADT_CONDITION)
	{
		for (INT i = 0; i < MAX_CONDITION_TYPE_COUNT; ++ i)
		{
			if (data.ConditionType[i] != adInvalid)
			{
				FString variable = FString(TEXT("achievement_condition_name")) + appItoa(i);
				object->SetString(variable, data.ConditionDesc[i]);
				variable = FString(TEXT("achievement_condition_complete")) + appItoa(i);
				object->SetBool(variable, (info.CurComplete & (1 << i)) > 0);
			}
			else
			{
				FString variable = FString(TEXT("achievement_condition_name")) + appItoa(i);
				object->SetString(variable, TEXT(""));
				variable = FString(TEXT("achievement_condition_complete")) + appItoa(i);
				object->SetBool(variable, FALSE);
			}
		}
	}
	else if (data.DetailType == ADT_PROGRESS)
	{
		UINT32 cur = abs(info.CurProgress);
		UINT32 max = abs(info.MaxProgress);
		cur = cur >= max ? max : cur;
		object->SetInt(TEXT("achievement_progress_cur"), cur);
		object->SetInt(TEXT("achievement_progress_max"), max == 0 ? 1 : max);
	}

	// comparison info
	UBOOL isCompare = MMOAchievementManager::getSingleton().IsCompare();
	object->SetBool(TEXT("achievement_is_compare"), isCompare);
	object->SetString(TEXT("achievement_icon_comp"), data.Icon);

	localTime = info.DateCompare * 86400;
	date = _localtime32(&localTime);
	ObtainDate = info.IsObtainCompare ? FString::Printf(TEXT("%d/%d/%d"), date->tm_year + 1900, date->tm_mon + 1, date->tm_mday) : TEXT("");
	object->SetString(TEXT("achievement_date_comp"), ObtainDate);

	object->SetBool(TEXT("achievement_is_obtain_comp"), info.IsObtainCompare);
	object->SetBool(TEXT("achievement_is_trace"), info.IsTraced);

	dataProvider->SetElementObject(index, object);
	index ++;
}

void UMMOUIAchievementPanel::_UpdateGuildAchievementInfo(UGFxObject* dataProvider, int& index, UINT32 id)
{
	if (!dataProvider)
	{
		return;
	}

	UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(id);
	GuildAchievementData data;
	if (!MMOAchievementData::getSingleton().GetGuildAchievementData(dataId, data))
	{
		return;
	}

	AchievementInfo info;
	if (!MMOAchievementData::getSingleton().GetAchievementInfo(id, info))
	{
		return;
	}

	// hidden achievement, not obtained, do not show
	if (data.Hide && !info.IsObtain)
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (!object)
	{
		return;
	}

	// basic info
	object->SetInt(TEXT("achievement_id"), id);
	object->SetBool(TEXT("achievement_is_obtain"), info.IsObtain);
	object->SetString(TEXT("achievement_icon"), data.Icon);
	object->SetString(TEXT("achievement_name"), data.Name);
	object->SetString(TEXT("achievement_desc"), data.Desc);
	object->SetString(TEXT("achievement_date"), TEXT(""));
	object->SetBool(TEXT("achievement_is_pre_obtain"), TRUE);

	// reward info
	// since there is currently only one reward ui (item or title), use the first one for now
	object->SetString(TEXT("achievement_reward_item"), TEXT(""));
	object->SetString(TEXT("achievement_reward_title"), TEXT(""));

	for (int i = 0; i < MAX_GUILD_ACHIEVEMENT_AWARD_COUNT; ++ i)
	{
		if (data.RewardType[i] == GA_AWARD_POINT)
		{
			object->SetInt(TEXT("achievement_point"), data.RewardCount[i]);
		}
		if (data.RewardType[i] == GA_AWARD_ITEM)
		{
			object->SetInt(TEXT("achievement_reward_type"), adtItem);
			UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.RewardTemp[i]);
			if (item)
			{
				object->SetInt(TEXT("achievement_reward_id"), data.RewardTemp[i]);
				object->SetString(TEXT("achievement_reward_icon"), item->GetIconName());
			}
		}
		else if (data.RewardType[i] == GA_AWARD_TITLE)
		{
			object->SetInt(TEXT("achievement_reward_type"), adtTitle);
			TitleData* title = MMOFacade::getSingleton().GetTitleData(data.RewardTemp[i]);
			if (title)
			{
				FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Title"))), *(title->TitleName));
				object->SetString(TEXT("achievement_reward_title"), text);
			}
		}
		else if (data.RewardType[i] == GA_AWARD_EXP)
		{
			object->SetInt(TEXT("achievement_reward_type"), adtExp);
			FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Exp"))), data.RewardCount[i]);
			object->SetString(TEXT("achievement_reward_exp"), text);
		}

		else if (data.RewardType[i] == GA_AWARD_POINT)
		{
			object->SetInt(TEXT("achievement_reward_type"), adtExp);
			FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Point"))), data.RewardCount[i]);
			object->SetString(TEXT("achievement_reward_exp"), text);
		}
	}

	object->SetBool(TEXT("achievement_is_get_reward"), info.IsGetReward);

	// detail info
	object->SetInt(TEXT("achievement_detail"), ADT_NULL);
	
	// comparison info
	object->SetBool(TEXT("achievement_is_compare"), FALSE);
	object->SetString(TEXT("achievement_icon_comp"), data.Icon);
	object->SetString(TEXT("achievement_date_comp"), TEXT(""));

	object->SetBool(TEXT("achievement_is_obtain_comp"), info.IsObtainCompare);
	object->SetBool(TEXT("achievement_is_trace"), info.IsTraced);

	dataProvider->SetElementObject(index, object);
	index ++;
}

void UMMOUIAchievementPanel::_UpdateKillNpcAchievementInfo(UGFxObject* dataProvider, int& index, UINT32 id)
{
	if (!dataProvider)
	{
		return;
	}

	UINT16 killId = HIWORD(id);
	UINT16 killIndex = LOWORD(id);
	KillNpcAchievementData data;
	if (!MMOAchievementData::getSingleton().GetKillNpcAchievementData(killId, data))
	{
		return;
	}

	AchievementInfo info;
	if (!MMOAchievementData::getSingleton().GetAchievementInfo(id, info))
	{
		return;
	}

	// hidden achievement, not obtained, do not show
	if (data.Hide[killIndex] && !info.IsObtain)
	{
		return;
	}

	UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
	if (!object)
	{
		return;
	}

	// basic info
	object->SetInt(TEXT("achievement_id"), id);
	object->SetBool(TEXT("achievement_is_obtain"), info.IsObtain);
	object->SetString(TEXT("achievement_icon"), data.Icon);
	object->SetString(TEXT("achievement_name"), data.Name[killIndex]);
	FString killDesc = FString::Printf(*(data.Desc), data.KillCount[killIndex]);
	object->SetString(TEXT("achievement_desc"), killDesc);
	object->SetInt(TEXT("achievement_point"), data.Point[killIndex]);
	object->SetInt(TEXT("achievement_require_amount"), data.KillCount[killIndex]);
	
	FString ObtainDate = TEXT("");
	__time32_t localTime = info.Date * 86400;
	struct tm *date = _localtime32(&localTime); 
	if (date)
	{
		ObtainDate = info.IsObtain && info.Date > 0 ? FString::Printf(TEXT("%d/%d/%d"), date->tm_year + 1900, date->tm_mon + 1, date->tm_mday) : TEXT("");
	}
	object->SetString(TEXT("achievement_date"), ObtainDate);
	object->SetBool(TEXT("achievement_is_pre_obtain"), TRUE);

	// reward info
	// since there is currently only one reward ui (item or title), use the first one for now
	object->SetInt(TEXT("achievement_reward_type"), data.RewardExp[killIndex] > 0 ? adtExp : adtInvalid);
	object->SetString(TEXT("achievement_reward_item"), TEXT(""));
	object->SetString(TEXT("achievement_reward_title"), TEXT(""));
	if (data.RewardExp[killIndex] > 0)
	{
		FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.Exp"))), data.RewardExp[killIndex]);
		object->SetString(TEXT("achievement_reward_exp"), text);
	}
	object->SetBool(TEXT("achievement_is_get_reward"), info.IsGetReward);

	// detail info
	object->SetInt(TEXT("achievement_detail"), ADT_PROGRESS);
	UINT32 cur = abs(info.CurProgress);
	UINT32 max = abs(info.MaxProgress);
	cur = cur >= max ? max : cur;
	object->SetInt(TEXT("achievement_progress_cur"), cur);
	object->SetInt(TEXT("achievement_progress_max"), max == 0 ? 1 : max);

	// comparison info
	object->SetBool(TEXT("achievement_is_compare"), FALSE);
	object->SetString(TEXT("achievement_icon_comp"), data.Icon);

	localTime = info.DateCompare * 86400;
	date = _localtime32(&localTime);
	ObtainDate = info.IsObtainCompare ? FString::Printf(TEXT("%d/%d/%d"), date->tm_year + 1900, date->tm_mon + 1, date->tm_mday) : TEXT("");
	object->SetString(TEXT("achievement_date_comp"), ObtainDate);

	object->SetBool(TEXT("achievement_is_obtain_comp"), info.IsObtainCompare);
	object->SetBool(TEXT("achievement_is_trace"), info.IsTraced);

	dataProvider->SetElementObject(index, object);
	index ++;
}

void UMMOUIAchievementPanel::UpdateAchievementList(INT baseClass, INT subClass)
{	
	UGFxObject* dataProvider = GFXPlayer->CreateArray();
	if (!dataProvider)
	{
		return;
	}

	AchievementClassMap classMap = MMOAchievementData::getSingleton().GetAchievementClassMap();
	UINT32 classKey = MMOAchievementData::getSingleton().GetAchievementClassKey(baseClass, subClass);
	AchievementClassMap::iterator iterClass = classMap.find(classKey);
	if (iterClass == classMap.end())
	{
		eventCallUpdateAchievementListData(dataProvider);
		eventCallUpdateAchievementList(dataProvider);
		return;
	}

	INT index = 0;
	AchievementIdList achievementList = iterClass->second;
	AchievementIdList::iterator iterList = achievementList.begin();
	for (; iterList != achievementList.end(); ++ iterList)
	{
		if (*iterList < ACHIEVEMENT_MAX)
		{
			_UpdateCommonAchievementInfo(dataProvider, index, *iterList);
		}
		else if (*iterList >= ACHIEVEMENT_MAX && *iterList < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
		{
			_UpdateGuildAchievementInfo(dataProvider, index, *iterList);
		}
		else
		{
			_UpdateKillNpcAchievementInfo(dataProvider, index, *iterList);
		}
	}

	if (_CurBaseClass == baseClass && _CurSubClass == subClass)
	{
		eventCallUpdateAchievementListData(dataProvider);
	} 
	else
	{
		eventCallUpdateAchievementList(dataProvider);
	}

	_CurBaseClass = baseClass;
	_CurSubClass = subClass;
}

void UMMOUIAchievementPanel::SetRewardAchievementId(int id)
{
	_GetRewardAchievementId = id;
}

//////////////////////////////////////////////////////////////////////////

void UMMOUIAchievementPanel::ASUpdateCagetoryData(INT type)
{
	UpdateAchievementCagetory(type);
}

void UMMOUIAchievementPanel::ASUpdateSummaryData(INT type)
{
	UpdateAchievementSummary(type);
}

void UMMOUIAchievementPanel::ASUpdateAchievementData(INT baseClass, INT subClass)
{
	UpdateAchievementData(baseClass, subClass);
}

void UMMOUIAchievementPanel::ASUpdateAchievementList(INT baseClass, INT subClass)
{
	UpdateAchievementList(baseClass, subClass);
}

void UMMOUIAchievementPanel::ASShowAchievementListByClass(INT baseClass, INT subClass)
{
	ShowAchievementListByClass(baseClass, subClass);
}

void UMMOUIAchievementPanel::ASShowAchievementListById(INT id)
{
	ShowAchievementListById(id);
}

void UMMOUIAchievementPanel::ASObtainRewardItem(INT id)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}

	if (mainPlayer->eventIsDead())
	{
		FString text = TEXT("Cannot claim rewards while dead");
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return;
	}

	if (MMOAchievementData::getSingleton().IsAchievementRewardObtain(id))
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.ERROR_CODE_DEF.errAchievementRewarded"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, text);
		return;
	}

	if (id < ACHIEVEMENT_MAX)
	{
		CMessageHandler::GetInstance().Send_ObtainRewardItem(id); 
	}
	else if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
	{
		UINT16 dataId = MMOAchievementData::getSingleton().GetGuildAchievementDataId(id);
		CMessageHandler::GetInstance().Send_ObtainGuildReward(dataId);
	}
	else
	{
		UINT16 killId = HIWORD(id);
		UINT8 killIndex = LOWORD(id);
		CMessageHandler::GetInstance().Send_KillNpcAchievementComplete(killId, killIndex); 
	}
}

void UMMOUIAchievementPanel::ASTriggerAchievementTrace(INT id, UBOOL trace)
{
	if (trace)
	{
		if (id >= ACHIEVEMENT_MAX && id < ACHIEVEMENT_MAX + GUILD_MAX_ACHIEVEMENT_NUM)
		{
			AchievementIdSet guildSet = MMOAchievementData::getSingleton().GetGuildAchievementBeTraced();
			if (guildSet.size() < MAX_TRACE_ACHIEVEMENT_COUNT)
			{
				MMOAchievementData::getSingleton().AddTraceAchievement(id);
			}
			else
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.BeTraced")), (INT)MAX_TRACE_ACHIEVEMENT_COUNT);
				MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Achievement.BeTraced"));
			}
		}
		else
		{
			AchievementIdSet personalSet = MMOAchievementData::getSingleton().GetPersonalAchievementBeTraced();
			if (personalSet.size() < MAX_TRACE_ACHIEVEMENT_COUNT)
			{
				MMOAchievementData::getSingleton().AddTraceAchievement(id);
			}
			else
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Achievement.BeTraced")), (INT)MAX_TRACE_ACHIEVEMENT_COUNT);
				MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Achievement.BeTraced"));
			}
		}
	}
	else
	{
		MMOAchievementData::getSingleton().RemoveTraceAchievement(id);
	}

	UpdateAchievementList(_CurBaseClass, _CurSubClass);
}