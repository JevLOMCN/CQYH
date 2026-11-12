#include "stdafx.h"
#include "MMOGuildManager.h"
#include "MMOFacade.h"
#include "Inc\MMOGameUIClasses.h"
#include "MMOChatManager.h"
#include "MMOMsgBoxManager.h"
#include "CommonGameEvents.h"
#include "UITextParser.h"
#include "MMOMsgButtonManager.h"
#include "MMOQuestManager.h"
#include "MMOMapDataManager.h"
#include "MMOStats.h"
#include "MMOClientVariableManager.h"
#include "FriendManager.h"
#include "MMOJourneyManager.h"
#include "MMOAchievementData.h"

using namespace ExternalMessage;

MMOGuildManager::MMOGuildManager()
{
	_OnlineMemberNotify = TRUE;
	_GuildAllyTimeProgress = 0.0f;
	_GuildEnemyTimeProgress = 0.0f;
	_GuildApplyAllyTimeProgress = 0.0f;
	_GuildBuildingUpgradeTimeProgress = 0.0f;
	_GuildWelfareUpgradeTimeProgress = 0.0f;
	_GuildWarEndProgress = 0.0f;
	_GuildWarEndTime = 0;
	_GuildWarCDProgress = 0.0f;
	_GuildWarCDTime = 0;
	_GuildWarReadyProgress = 0.0f;
	_GuildWarReadyTime = 0;
	_GuildWarProgress = 0.0f;
	_GuildWarTime = 0.0f;
	_CurSceneGuildId = 0;
	_GuildWarViewProgress = 0.0f;
	_GuildWarViewDelayTime = 0;

	_NeedUpdateGuildNameRoleMap.clear();
	_CurGuildWarState = GUILD_WAR_STAGE_NONE;

	_SabacState = 0;
	_StatueActor = NULL;
	_StatueHeadName = NULL;
	_StatueEffect = NULL;
	_StatueNpcId = 0;
	_GuildLeaveEndTime = 0;

	_GuildBuildingBaseActorMap.clear();
	_GuildBuildingBaseProgress = 0.0f;

	_GuildQueryInvalidIdTimeProgress = 0.0f;

	_GuildViewEventsProgress = 0.0f;
	_GuildViewEventsDelay = FALSE;

	_IsGuildWarStart = FALSE;
	_GuildWarRecordsProgress = 0.0f;
	_GuildWarRecordsDelay = FALSE;

	MMOUI::RegisterEventHandler(this, GameEvent_Guild);
	MMOUI::RegisterEventHandler(this, GameEvent_InviteToGuild);
	MMOUI::RegisterEventHandler(this, GameEvent_PlayerBaseInfo);
	MMOUI::RegisterEventHandler(this, GameEvent_EnterScene);
	MMOUI::RegisterEventHandler(this, GameEvent_ShowGuildView);
	MMOUI::RegisterEventHandler(this, GameEvent_LeaveSelectingRole);
	MMOUI::RegisterEventHandler(this, GameEvent_NPCAppear);
	MMOUI::RegisterEventHandler(this, GameEvent_NPCDestroyed);
	MMOUI::RegisterEventHandler(this, GameEvent_VariableInit);
	MMOUI::RegisterEventHandler(this, GameEvent_VariableChanged);
}

MMOGuildManager::~MMOGuildManager()
{
	MMOUI::UnRegisterEventHandler(this, GameEvent_Guild);
	MMOUI::UnRegisterEventHandler(this, GameEvent_InviteToGuild);
	MMOUI::UnRegisterEventHandler(this, GameEvent_PlayerBaseInfo);
	MMOUI::UnRegisterEventHandler(this, GameEvent_EnterScene);
	MMOUI::UnRegisterEventHandler(this, GameEvent_ShowGuildView);
	MMOUI::UnRegisterEventHandler(this, GameEvent_LeaveSelectingRole);
	MMOUI::UnRegisterEventHandler(this, GameEvent_NPCAppear);
	MMOUI::UnRegisterEventHandler(this, GameEvent_NPCDestroyed);
	MMOUI::UnRegisterEventHandler(this, GameEvent_VariableInit);
	MMOUI::UnRegisterEventHandler(this, GameEvent_VariableChanged);
}

void MMOGuildManager::Init()
{
	SCOPE_LOG_TIME(___INIT__, TEXT("MMOGuildManager::Init"), 0);
	MMOGuildData::getSingleton().Init();
}

void MMOGuildManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch (evt->GetEventType())
	{
	case GameEvent_VariableInit:
	case GameEvent_VariableChanged:
		{
			MMOGameEvent_VariableChange* varChgEvent = (MMOGameEvent_VariableChange*)evt;
			if (varChgEvent)
			{
				MMOGuildData::getSingleton().UpdateVariable(varChgEvent->VarType, varChgEvent->VarReferID, varChgEvent->VarID, varChgEvent->VarValue);
			}	
		}
		break;

	case GameEvent_LeaveSelectingRole:
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				// when the main character exits the scene and returns to login it is not deleted, so flags are not restored; initialize once when entering the game
				mainPlayer->inSabacZoneCount = 0;
			}
		}
		break;

	case GameEvent_InviteToGuild:
		{
			MMOGameEvent_MsgboxCallBack* msg = (MMOGameEvent_MsgboxCallBack*)(evt);
			if (msg && msg->Box)
			{
				int roleID = msg->Box->GetInt(TEXT("iteminfoid"));
				if (msg->Agree)
				{
					CMessageHandler::GetInstance().Send_GuildInviteResponse(GUILD_REVERT_AGREE, roleID);
				}
				else
				{
					CMessageHandler::GetInstance().Send_GuildInviteResponse(GUILD_REVERT_REFUSE, roleID);
				}
			}
		}
		break;

	case GameEvent_PlayerBaseInfo:
		{
			MMOUI::GetUIManager()->delegateUpdateAccessInfo();
		}
		break;

	case GameEvent_NPCAppear:
		{
			MMOGameEvent_NPCAppear* npcEvent = (MMOGameEvent_NPCAppear*)evt;
			if (npcEvent->Actor && npcEvent->Actor->IsA(ANPC::StaticClass()))
			{
				ANPC* npc = Cast<ANPC>(npcEvent->Actor);
				if (npc && npc->Template)
				{
					if (npc->GetTemplateID() == _StatueNpcId)
					{
						SabacStatue statue;
						if (MMOGuildData::getSingleton().GetStatueData(statue))
						{
							_CreateStatue(statue, npc);
						}
					}

					if (npc->Template->BuildingMesh)
					{
						_CreateBuildingBase(npc, npc->Template->BuildingMesh);
					}
				}
			}
		}
		break;

	case GameEvent_NPCDestroyed:
		{
			MMOGameEvent_NPCDestroyed* npcEvent = (MMOGameEvent_NPCDestroyed*)evt;
			if (npcEvent->Actor && npcEvent->Actor->IsA(ANPC::StaticClass()))
			{
				ANPC* npc = Cast<ANPC>(npcEvent->Actor);
				if (npc && npc->Template)
				{
					if (npc->GetTemplateID() == _StatueNpcId)
					{
						_DestroyStatue();
					}

					if (npc->Template->BuildingMesh)
					{
						_DestroyBuildingBase(npc);
					}
				}
			}
		}
		break;

	case GameEvent_EnterScene:
		{
		}
		break;

	case GameEvent_ShowGuildView:
		{
			UMMOUIGuildViewPanel* viewPanel = (UMMOUIGuildViewPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_View);
			if (viewPanel)
			{
				viewPanel->_IsNpcCalled = FALSE;
				viewPanel->Show(TRUE);
			}
		}
		break;
	}
}

void MMOGuildManager::Shutdown()
{
	MMOGuildData::getSingleton().Shutdown();
}

void MMOGuildManager::Clear()
{
	MMOGuildData::getSingleton().Clear();
	_UpdateMainActorGuildTitle(TEXT(""));

	_OnlineMemberNotify = TRUE;

	_GuildAllyTimeProgress = 0.0f;
	_GuildEnemyTimeProgress = 0.0f;
	_GuildApplyAllyTimeProgress = 0.0f;
	_GuildBuildingUpgradeTimeProgress = 0.0f;
	_GuildWelfareUpgradeTimeProgress = 0.0f;
	_GuildWarEndProgress = 0.0f;
	_GuildWarEndTime = 0;
	_GuildWarCDProgress = 0.0f;
	_GuildWarCDTime = 0;
	_GuildWarReadyProgress = 0.0f;
	_GuildWarReadyTime = 0;
	_GuildWarProgress = 0.0f;
	_GuildWarTime = 0.0f;
	_CurSceneGuildId = 0;
	_GuildWarViewProgress = 0.0f;
	_GuildWarViewDelayTime = 0;

	_NeedUpdateGuildNameRoleMap.clear();
	_CurGuildWarState = GUILD_WAR_STAGE_NONE;

	_SabacState = 0;
	_StatueActor = NULL;
	_StatueHeadName = NULL;
	_StatueEffect = NULL;

	GConfig->GetInt(TEXT("Guild"), TEXT("StatueNpcId"), _StatueNpcId, GGameIni);

	UMMOUIGuildMainPanel* mainPanel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Main);
	if (mainPanel)
	{
		mainPanel->IsShowOnlineMember = FALSE;
		mainPanel->eventCallInitShowOnlineMember();
	}

	_GuildLeaveEndTime = 0;

	GuildBuildingBaseActorMap::iterator iter = _GuildBuildingBaseActorMap.begin();
	for (; iter != _GuildBuildingBaseActorMap.end(); ++ iter)
	{
		AAbilitySkeletalMeshActor* meshActor = Cast<AAbilitySkeletalMeshActor>(UObject::GetIndexedObject(iter->second));
		if (meshActor)
		{
			meshActor->DetachComponent(meshActor->SkeletalMeshComponent);
			if(!meshActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
			{
				GWorld->DestroyActor(meshActor);
			}
			iter = _GuildBuildingBaseActorMap.erase(iter);
		}
	}
	_GuildBuildingBaseActorMap.clear();
	_GuildBuildingBaseProgress = 0.0f;

	_GuildQueryInvalidIdTimeMap.clear();
	_GuildQueryInvalidIdTimeProgress = 0.0f;

	_GuildViewEventsProgress = 0.0f;
	_GuildViewEventsDelay = FALSE;

	_IsGuildWarStart = FALSE;
	_GuildWarRecordsProgress = 0.0f;
	_GuildWarRecordsDelay = FALSE;

	ClearGuildOccupy();
}

void MMOGuildManager::_UpdateGuildTerritroyUpgrade(FLOAT deltaTime)
{
	if (MMOGuildData::getSingleton().GetGuildBuildingUpgradeList().size() > 0)
	{
		FLOAT updateTime = 1.0f;
		_GuildBuildingUpgradeTimeProgress += deltaTime;
		if (_GuildBuildingUpgradeTimeProgress > updateTime)
		{
			MMOGuildData::getSingleton().UpdateGuildBuildingUpgradeTime((INT)updateTime);
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryUpgradeList);
			MMOUI::PostGameEvent(&evt);

			_GuildBuildingUpgradeTimeProgress = 0.0f;
		}
	}

	if (MMOGuildData::getSingleton().GetGuildWelfareUpgradeList().size() > 0)
	{
		FLOAT updateTime = 1.0f;
		_GuildWelfareUpgradeTimeProgress += deltaTime;
		if (_GuildWelfareUpgradeTimeProgress > updateTime)
		{
			MMOGuildData::getSingleton().UpdateGuildWelfareUpgradeTime((INT)updateTime);
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryUpgradeList);
			MMOUI::PostGameEvent(&evt);

			_GuildWelfareUpgradeTimeProgress = 0.0f;
		}
	}
}

void MMOGuildManager::_UpdateGuildForeignTime(FLOAT deltaTime)
{
	if (MMOGuildData::getSingleton().GetGuildAllyInfoList().size() > 0)
	{
		FLOAT updateTime = 1.0f;
		_GuildAllyTimeProgress += deltaTime;
		if (_GuildAllyTimeProgress > updateTime)
		{
			MMOGuildData::getSingleton().UpdateGuildAllyTime((UINT32)updateTime);
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildAllyList);
			MMOUI::PostGameEvent(&evt);

			_GuildAllyTimeProgress = 0.0f;
		}
	}

	if (MMOGuildData::getSingleton().GetGuildEnemyInfoList().size() > 0)
	{
		FLOAT updateTime = 1.0f;
		_GuildEnemyTimeProgress += deltaTime;
		if (_GuildEnemyTimeProgress > updateTime)
		{
			MMOGuildData::getSingleton().UpdateGuildEnemyTime((UINT32)updateTime);
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildEnemyList);
			MMOUI::PostGameEvent(&evt);

			_GuildEnemyTimeProgress = 0.0f;
		}
	}

	if (MMOGuildData::getSingleton().GetGuildApplyAllyInfoList().size() > 0)
	{
		FLOAT updateTime = 1.0f;
		_GuildApplyAllyTimeProgress += deltaTime;
		if (_GuildApplyAllyTimeProgress > updateTime)
		{
			MMOGuildData::getSingleton().UpdateGuildApplyAllyTime((UINT32)updateTime);
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildApplyAllyList);
			MMOUI::PostGameEvent(&evt);

			_GuildApplyAllyTimeProgress = 0.0f;
		}
	}
}

void MMOGuildManager::_UpdateGuildWarViewTime(FLOAT deltaTime)
{
	if (_GuildWarViewDelayTime > 0)
	{
		_GuildWarViewProgress += deltaTime;
		if (_GuildWarViewProgress > 1.0f)
		{
			_GuildWarViewDelayTime --;
			_GuildWarViewProgress = 0.0f;
		}

		if (_GuildWarViewDelayTime == 0)
		{
			NotifyGuildWarSearchEnd();
		}
	}
}

void MMOGuildManager::_UpdateGuildWarReadyTime(FLOAT deltaTime)
{
	if (_GuildWarReadyTime > 0)
	{
		_GuildWarReadyProgress += deltaTime;
		if (_GuildWarReadyProgress > 1.0f)
		{
			_GuildWarReadyTime --;
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildWarReadyTime);
			MMOUI::PostGameEvent(&evt);

			_GuildWarReadyProgress = 0.0f;
		}
	}
}

void MMOGuildManager::_UpdateGuildWarTime(FLOAT deltaTime)
{
	if (_GuildWarTime > 0)
	{
		_GuildWarProgress += deltaTime;
		if (_GuildWarProgress > 1.0f)
		{
			_GuildWarTime --;
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildWarTime);
			MMOUI::PostGameEvent(&evt);

			_GuildWarProgress = 0.0f;
		}
	}
}

void MMOGuildManager::_UpdateGuildWarEndTime(FLOAT deltaTime)
{
	if (_GuildWarEndTime > 0)
	{
		_GuildWarEndProgress += deltaTime;
		if (_GuildWarEndProgress > 1.0f)
		{
			_GuildWarEndTime --;
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildWarEndTime);
			MMOUI::PostGameEvent(&evt);

			_GuildWarEndProgress = 0.0f;
		}
	}
}

void MMOGuildManager::_UpdateGuildWarCD(FLOAT deltaTime)
{
	if (_GuildWarCDTime > 0)
	{
		_GuildWarCDProgress += deltaTime;
		if (_GuildWarCDProgress > 1.0f)
		{
			_GuildWarCDTime --;
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildWarCD);
			MMOUI::PostGameEvent(&evt);

			_GuildWarCDProgress = 0.0f;
		}
	}
}

void MMOGuildManager::_UpdateGuildBuildingBasePos(FLOAT deltaTime)
{
	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == GUILD_TERRITORY_MAP_ID)
	{
		FLOAT updateTime = 2.0f;
		_GuildBuildingBaseProgress += deltaTime;
		if (_GuildBuildingBaseProgress > updateTime)
		{
			GuildBuildingBaseActorMap::iterator iter = _GuildBuildingBaseActorMap.begin();
			for (; iter != _GuildBuildingBaseActorMap.end(); ++iter)
			{
				ANPC* npc = Cast<ANPC>(MMOFacade::getSingleton().GetCharacter(INT(iter->first)));
				AAbilitySkeletalMeshActor* meshActor = Cast<AAbilitySkeletalMeshActor>(UObject::GetIndexedObject(iter->second));
				if (npc && meshActor)
				{
					meshActor->SetLocation(npc->GetFootLocation());
				}
			}

			_GuildBuildingBaseProgress = 0.0f;
		}
	}
}

void MMOGuildManager::_UpdateGuildQueryInvalidIdTime(FLOAT deltaTime)
{
	// Quest #23580
	if (_GuildQueryInvalidIdTimeMap.size() > 0)
	{
		// update every 10 minutes
		FLOAT updateTime = 600.0f;
		_GuildQueryInvalidIdTimeProgress += deltaTime;
		if (_GuildQueryInvalidIdTimeProgress > updateTime)
		{
			__time32_t localTime;
			_time32(&localTime);
			_localtime32(&localTime);

			GuildQueryInvalidIdTimeMap::iterator iter = _GuildQueryInvalidIdTimeMap.begin();
			for (; iter != _GuildQueryInvalidIdTimeMap.end();)
			{
				INT startTime = iter->second;
				INT duration = localTime - startTime;

				if (duration >= GUILD_INVALID_ID_TIME)
				{
					iter = _GuildQueryInvalidIdTimeMap.erase(iter);
				}
				else
				{
					iter ++;
				}
			}

			_GuildQueryInvalidIdTimeProgress = 0.0f;
		}
	}
}

void MMOGuildManager::_UpdateGuildViewEventsTime(FLOAT deltaTime)
{
	if (_GuildViewEventsDelay)
	{
		_GuildViewEventsProgress += deltaTime;
		if (_GuildViewEventsProgress > 2.0f)
		{
			UpdateGuildEvents();
			UpdateGuildMoneyEvents();

			_GuildViewEventsDelay = FALSE;
		}
	}
}

void MMOGuildManager::_UpdateGuildWarRecordsTime(FLOAT deltaTime)
{
	if (_GuildWarRecordsDelay)
	{
		_GuildWarRecordsProgress += deltaTime;
		if (_GuildWarRecordsProgress > 1.0f)
		{
			MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildWarRecord);
			MMOUI::PostGameEvent(&evt);

			_GuildWarRecordsDelay = FALSE;
		}
	}
}

void MMOGuildManager::Update(FLOAT deltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_GuildTime);

	_UpdateGuildForeignTime(deltaTime);
	_UpdateGuildTerritroyUpgrade(deltaTime);
	_UpdateGuildWarViewTime(deltaTime);
	_UpdateGuildWarReadyTime(deltaTime);
	_UpdateGuildWarTime(deltaTime);
	_UpdateGuildWarEndTime(deltaTime);
	_UpdateGuildWarCD(deltaTime);
	_UpdateGuildBuildingBasePos(deltaTime);
	_UpdateGuildQueryInvalidIdTime(deltaTime);
	_UpdateGuildViewEventsTime(deltaTime);
	_UpdateGuildWarRecordsTime(deltaTime);
}

void MMOGuildManager::UpdateGuildEvents()
{
	MMOGuildData::getSingleton().UpdateGuildEvents();
	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildEvents);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::UpdateGuildMoneyEvents()
{
	MMOGuildData::getSingleton().UpdateGuildMoneyEvents();
	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildMoneyEvents);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::SortGuildFullMember(INT type, UBOOL isUp)
{
	MMOGuildData::getSingleton().SortGuildFullMember(type, isUp);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildFullMemberList);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::SetOnlineMemberNotify(UBOOL notify)
{
	_OnlineMemberNotify = notify;
}

// notify
//////////////////////////////////////////////////////////////////////////

void MMOGuildManager::NotifyCreateGuildSuccess(const FString& guildName)
{
	FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.CreateSuccess"));
	text += TEXT("<");
	text += guildName;
	text += TEXT(">");
	MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.CreateSuccess"));

	if (MMOUI::GetUIManager())
	{
		UMMOUIGuildCreatePanel* panel = (UMMOUIGuildCreatePanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Create);
		if (panel && panel->IsVisible)
		{
			panel->eventCallCreateSuccess();
		}
	}
}

void MMOGuildManager::NotifyApplyToJoinGuildSuccess()
{
	MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ApplySuccess")),MsgInfoType_Table,TEXT("MMOGame.Guild.ApplySuccess"));

	// Guide quest
	MMOQuestManager::getSingleton().UpdateClientVariable(ECVT_Quest, GQT_Guild);
}

void MMOGuildManager::NotifyJoinGuildSuccess(const FString& guildName)
{
	FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.JoinSuccess"))), *guildName);
	MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.JoinSuccess"));
}

void MMOGuildManager::NotifyUpdateGuildViewList(UINT8 type, GUILD_VIEW_SUMMARY* list, UINT16 size)
{
	if (size > 0)
	{
		MMOGuildData::getSingleton().UpdateGuildViewList(type, list, size);
	}

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildViewList);
	evt._ViewType = type;
	evt._ViewSize = size;
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyUpdateGuildFindList(const GUILD_VIEW_SUMMARY& result)
{
	// if not found in the current info list, add it, for easy access from UMMOUIGuildViewPanel::ApplyForJoint
	GuildViewInfo info;
	if (!MMOGuildData::getSingleton().GetGuildViewInfo(result.dwGuildID, info))
	{
		GuildViewInfo info;
		info.Id = result.dwGuildID;
		info.Name = UTF8_TO_TCHAR(result.szGuildName);
		info.level = result.byGuildLevel;
		info.MemberCount = result.byCurrentMemberCount;
		info.LeaderName = UTF8_TO_TCHAR(result.szLeaderName);
		info.CreatorName = UTF8_TO_TCHAR(result.szCreaterName);
		info.CreateTime = result.dwCreateTime;
		info.Announce = UTF8_TO_TCHAR(result.szAnnounce);
		info.ProtectionTime = result.dwProtectionTime;
		MMOGuildData::getSingleton().AddGuildViewInfo(info);
	}

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildFindList);
	evt._FindInfo.Id = result.dwGuildID; 
	evt._FindInfo.Name = UTF8_TO_TCHAR(result.szGuildName);
	evt._FindInfo.Level = result.byGuildLevel; 
	evt._FindInfo.MemberCount = result.byCurrentMemberCount; 
	evt._FindInfo.LeaderName = UTF8_TO_TCHAR(result.szLeaderName);
	evt._FindInfo.CreatorName = UTF8_TO_TCHAR(result.szCreaterName);
	evt._FindInfo.CreateTime = result.dwCreateTime;
	evt._FindInfo.Announce = UTF8_TO_TCHAR(result.szAnnounce);
	evt._FindInfo.ProtectionTime = result.dwProtectionTime;
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::_ResetLocalGuildInfo()
{
	MMOGuildData::getSingleton().Clear();
	_UpdateMainActorGuildTitle(TEXT(""));

	MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildFullMemberList);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
	MMOUI::PostGameEvent(&evt2);

	MMOGameEvent_Guild evt3(MMOGameEvent_Guild::EGET_UpdateGuildNotice);
	MMOUI::PostGameEvent(&evt3);

	MMOGameEvent_Guild evt4(MMOGameEvent_Guild::EGET_UpdateGuildEvents);
	MMOUI::PostGameEvent(&evt4);

	MMOGameEvent_Guild evt5(MMOGameEvent_Guild::EGET_UpdateGuildMoneyEvents);
	MMOUI::PostGameEvent(&evt5);

	AMMOActor* mainActor = MMOFacade::getSingleton().GetMainActor();
	if (mainActor)
	{
		MMOGameEvent_Guild evt5(MMOGameEvent_Guild::EGET_UpdateGuildStorageRights);
		evt5._CurPost = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainActor->ServerID);
		MMOUI::PostGameEvent(&evt5);
	}

	MMOGameEvent_Guild evt6(MMOGameEvent_Guild::EGET_UpdateGuildSalary);
	MMOUI::PostGameEvent(&evt6);

	MMOGameEvent_Guild evt7(MMOGameEvent_Guild::EGET_UpdateGuildAllyList);
	MMOUI::PostGameEvent(&evt7);

	MMOGameEvent_Guild evt8(MMOGameEvent_Guild::EGET_UpdateGuildEnemyList);
	MMOUI::PostGameEvent(&evt8);
}

void MMOGuildManager::NotifyUpdateGuildBaseInfo(const GuildBaseInfo& baseInfo)
{
	MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);
	_UpdateMainActorGuildTitle(baseInfo.Name);
}

void MMOGuildManager::_UpdateMainActorGuildTitle(const FString& guildName)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if(mainPlayer)
	{
		mainPlayer->GuildName = guildName;
	}

	UMMOUISceneHint* sceneHint = Cast<UMMOUISceneHint>(MMOUI::FindUIPanel(TEXT("SceneHint")));
	if (sceneHint)
	{
		AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
		ULocalPlayer* localPlayer = Cast<ULocalPlayer>(MMOFacade::getSingleton().GetMainController()->Player);
		if(mainPlayer && localPlayer)
		{
			FLOAT HudNameDisplayRangeSquared2D = 0.0f;
			GConfig->GetFloat(TEXT("HUD"), TEXT("HudNameDisplayRangeSquared2D"), HudNameDisplayRangeSquared2D, GUIIni);
			sceneHint->UpdateHints_Player(mainPlayer, localPlayer, TRUE, HudNameDisplayRangeSquared2D);
		}
	}
}

void MMOGuildManager::_UpdateActorGuildTitle(UINT32 id, const FString& name)
{
	AMMOActor* actor = MMOFacade::getSingleton().GetCharacter(id);
	if (actor)
	{
		UMMOUISceneHint* sceneHint = Cast<UMMOUISceneHint>(MMOUI::FindUIPanel(TEXT("SceneHint")));
		if (sceneHint)
		{
			AMMOPlayer* player = Cast<AMMOPlayer>(actor);
			ULocalPlayer* localPlayer = Cast<ULocalPlayer>(MMOFacade::getSingleton().GetMainController()->Player);
			if(player && localPlayer)
			{
				player->GuildName = name;

				FLOAT HudNameDisplayRangeSquared2D = 0.0f;
				GConfig->GetFloat(TEXT("HUD"), TEXT("HudNameDisplayRangeSquared2D"), HudNameDisplayRangeSquared2D, GUIIni);
				sceneHint->UpdateHints_Player(player, localPlayer, TRUE, HudNameDisplayRangeSquared2D);
			}
		}

		MMOGameEvent_OneInt evt(GameEvent_GuildSabacStateChanged, id);
		MMOUI::PostGameEvent(&evt);
	}
}

void MMOGuildManager::_UpdateAllGuildTitle()
{
	// update nearby characters
	STLInt2IntMap* playerMap = MMOFacade::getSingleton().GetPlayerMap();
	STLInt2IntMap::iterator playerMapItor;
	for (playerMapItor = playerMap->begin(); playerMapItor != playerMap->end(); playerMapItor ++)
	{
		AMMOPlayer* player = Cast<AMMOPlayer>(MMOFacade::getSingleton().GetCharacter(playerMapItor->first));
		if (player)
		{
			MMOUI::GetUIManager()->delegateUpdateHintText(player);

			MMOGameEvent_OneInt evt(GameEvent_GuildSabacStateChanged, player->ServerID);
			MMOUI::PostGameEvent(&evt);
		}
	}

	// update main character
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		MMOUI::GetUIManager()->delegateUpdateHintText(mainPlayer);

		MMOGameEvent_OneInt evt(GameEvent_GuildSabacStateChanged, mainPlayer->ServerID);
		MMOUI::PostGameEvent(&evt);
	}
}

void MMOGuildManager::NotifyUpdateActorGuildTitle(UINT32 roleId, const FString& guildName, UINT32 guildId)
{
	if (guildName != TEXT(""))
	{
		_UpdateActorGuildTitle(roleId, guildName);
	}
	else if (guildId > 0)
	{
		GuildQueryInfo info;
		if (MMOGuildData::getSingleton().GetGuildInfoById(guildId, info))
		{
			_UpdateActorGuildTitle(roleId, info.Name);
		}
		else
		{
			// delayed update of character guild name
			_NeedUpdateGuildNameRoleMap.insert(std::make_pair(roleId, guildId));
		}
	}

	if (guildId > 0)
	{
		QueryGuildInfo(guildId);
	}
}

void MMOGuildManager::NotifyUpdateGuildNotice(const FString& notice)
{
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	baseInfo.Notice = notice;
	MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildNotice);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyUpdateGuildEvents(GUILD_EVENT_INFO* events, UINT8 size)
{
	MMOGuildData::getSingleton().UpdateGuildEvents(events, size);
	UpdateGuildEvents();
}

void MMOGuildManager::NotifyMoreGuildEvents(GUILD_EVENT_INFO* events, UINT8 size)
{
	// when querying more events the server sends all remaining guild events; disable the button to avoid duplicate events from repeated queries
	UMMOUIGuildMainPanel* panel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Main);
	if (panel)
	{
		panel->eventCallUpdateViewMoreHistoryState(FALSE);
	}

	for (INT i = 0; i < size; ++ i)
	{
		MMOGuildData::getSingleton().AddGuildEventInfo(events[i]);
	}
	UpdateGuildEvents();

	_GuildViewEventsProgress = 0.0f;
	_GuildViewEventsDelay = TRUE;
}

void MMOGuildManager::NotifyUpdateGuildMoneyEvents(GUILD_EVENT_INFO* events, UINT8 size)
{
	MMOGuildData::getSingleton().UpdateGuildMoneyEvents(events, size);
	UpdateGuildMoneyEvents();

	_GuildViewEventsProgress = 0.0f;
	_GuildViewEventsDelay = TRUE;
}

void MMOGuildManager::NotifyUpdateGuildFullMemberList(GUILD_MEMBER_INFO* list, UINT8 size)
{
	MMOGuildData::getSingleton().UpdateGuildFullMemberList(list, size);
}

void MMOGuildManager::NotifyUpdateGuildFullMemberData(UINT32 roleId, UINT8 level, INT32 sceneId, UINT32 lastLoginTime)
{
	MMOGuildData::getSingleton().UpdateGuildFullMemberData(roleId, level, sceneId, lastLoginTime);
 
	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildFullMemberList);
	MMOUI::PostGameEvent(&evt);

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer && roleId != mainPlayer->ServerID)
	{
		if (_OnlineMemberNotify)
		{
			FString memberName = FString::Printf(TEXT("ID:%d"), roleId);
			MMOGuildData::getSingleton().GetGuildFullMemberName(roleId, memberName);

			// Quest 13037
			/*
			if (lastLoginTime == 0)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MemberOnline")), *memberName);
				MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.MemberOnline"));
			} 
			else
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.MemberOffline")), *memberName);
				MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.MemberOffline"));
			}
			*/
		}
	}
}

void MMOGuildManager::NotifyUpdateGuildPendingMemberList(GUILD_APPLICANT_INFO* list, UINT8 size)
{
	MMOGuildData::getSingleton().UpdateGuildPendingMemberList(list, size);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildPendingMemberList);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyBeInviteToJoinGuild(UINT32 roleId, const FString& roleName, const FString& guildName)
{
    if (MMOUI::GetUIManager()->DeclineInviteFamily)
	{
        CMessageHandler::GetInstance().Send_GuildInviteResponse(GUILD_REVERT_REFUSE, roleId);
	}
    else
	{
		FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.InviteSuccess"))), *roleName, *guildName);
		//MMOMsgBoxManager::getSingleton().AddMessageBoxEvent(text, roleId, GameEvent_InviteToGuild);
		MMOMsgButtonManager::getSingleton().AddNewButton(text, roleId, roleName, GameEvent_InviteToGuild, E_MsgBtnShow_Guild);
	}
}

void MMOGuildManager::NotifyInviteToJoinGuild(UINT32 replyType, const FString& roleName)
{
	FString text = TEXT("");
	if (replyType == GUILD_REPLY_AGREE)
	{
		text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.AgreeToInvite"))), *roleName);
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.RefuseToInvite"));
	} 
	else
	{
		text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.RefuseToInvite"))), *roleName);
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.RefuseToInvite"));
	}
}

void MMOGuildManager::NotifyAddGuildFullMember(const GUILD_MEMBER_INFO& info)
{
	MMOGuildData::getSingleton().AddGuildFullMember(info);
	MMOGuildData::getSingleton().SortGuildFullMember(FMST_TIME, TRUE);
	
	MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildFullMemberList);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
	MMOUI::PostGameEvent(&evt2);

	// request guild achievement info
	CMessageHandler::GetInstance().Send_GuildAchievement();
}

void MMOGuildManager::NotifyChangeGuildFullMemberPost(UINT32 id, UINT8 post)
{
	UINT8 oldPost = MMOGuildData::getSingleton().GetGuildFullMemberPost(id);
	FString oldPostName = TEXT("");
	MMOGuildData::getSingleton().GetGuildPostName(oldPost, oldPostName);

	MMOGuildData::getSingleton().UpdateGuildFullMemberPost(id, post);

	UINT8 newPost = MMOGuildData::getSingleton().GetGuildFullMemberPost(id);
	FString newPostName = TEXT("");
	MMOGuildData::getSingleton().GetGuildPostName(newPost, newPostName);

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer && id == mainPlayer->ServerID)
	{
		MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
		MMOUI::PostGameEvent(&evt);
	}

	// tips
	FString roleName = TEXT("");
	MMOGuildData::getSingleton().GetGuildFullMemberName(id, roleName);
	FString text = FString::Printf((const TCHAR*)(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ChangePost"))),
		*roleName, 
		*oldPostName, 
		*newPostName);
	MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.ChangePost"));

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildFullMemberList);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyDelGuildFullMember(UINT32 id)
{
	MMOGuildData::getSingleton().DelGuildFullMember(id);
	MMOGuildData::getSingleton().SortGuildFullMember(FMST_TIME, TRUE);
	
	MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildFullMemberList);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
	MMOUI::PostGameEvent(&evt2);

	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer)
	{
		if (id == mainPlayer->ServerID)
		{
			_ResetLocalGuildInfo();
		} 
		else
		{
			_UpdateActorGuildTitle(id, TEXT(""));
		}
	}

	// request guild achievement info
	CMessageHandler::GetInstance().Send_GuildAchievement();
}

void MMOGuildManager::NotifyUpdateGuildStorageRights(UINT8 post, UINT16 rights)
{
	MMOGuildData::getSingleton().UpdateGuildStorageRights(post, rights);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildStorageRights);
	evt._CurPost = post;
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyUpdateGuildAttr(UINT32 type, UINT32 value)
{
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	switch (type)
	{
	case GUILD_ACTIVITY_VARIABLE_107:
	case GUILD_ACTIVITY_VARIABLE_108:
	case GUILD_ACTIVITY_VARIABLE_109:
	case GUILD_ACTIVITY_VARIABLE_110:
		{
			UINT16 activityType = MMOGuildData::getSingleton().GetGuildActivityTypeByVariable(type);
			GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
			UINT16 activityLevel = baseInfo.level;

			if (activityLevel > 0)
			{
				MMOGuildData::getSingleton().SetGuildActivityTime(activityType, value);
				
				MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildActivity);
				MMOUI::PostGameEvent(&evt1);

				MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildMoneyEvents);
				MMOUI::PostGameEvent(&evt2);
			}
		}
		break;

	case GUILD_ATTR_MONEY:
		{
			UINT32 oldValue = baseInfo.Money;
			baseInfo.Money = value;
			MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);
			UINT32 newValue = baseInfo.Money;

			// tips
			if (newValue > oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResAdd")), 
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Money")),
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}
			else if (newValue < oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResSub")),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Money")), 
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}

			// update fund distribution
			MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildSalary);
			MMOUI::PostGameEvent(&evt1);

			MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
			MMOUI::PostGameEvent(&evt2);
		}
		break;
	case GUILD_ATTR_LEVEL:
		{
			baseInfo.level = value;
			MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);

			MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
			MMOUI::PostGameEvent(&evt1);
		}
		break;
	//case GUILD_TOTAL_SALARY:
	//	{
	//		_GuildBaseInfo.Salary = value;

	//		// update fund distribution
	//		MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildSalary);
	//		MMOUI::PostGameEvent(&evt1);
	//	}
	//	break;
	case GUILD_ATTR_WOOD:
		{
			UINT32 oldValue = baseInfo.Wood;
			baseInfo.Wood = value;
			MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);
			UINT32 newValue = baseInfo.Wood;

			// tips
			if (newValue > oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResAdd")),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Wood")), 
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}
			else if (newValue < oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResSub")),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Wood")), 
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}

			MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
			MMOUI::PostGameEvent(&evt1);
		}
		break;
	case GUILD_ATTR_HERB:
		{
			UINT32 oldValue = baseInfo.Herb;
			baseInfo.Herb = value;
			MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);
			UINT32 newValue = baseInfo.Herb;

			// tips
			if (newValue > oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResAdd")),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Herb")),
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}
			else if (newValue < oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResSub")),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Herb")),
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}

			MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
			MMOUI::PostGameEvent(&evt1);
		}
		break;
	case GUILD_ATTR_CRYSTAL:
		{
			UINT32 oldValue = baseInfo.Crystal;
			baseInfo.Crystal = value;
			MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);
			UINT32 newValue = baseInfo.Crystal;

			// tips
			if (newValue > oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResAdd")),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Crystal")),
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}
			else if (newValue < oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResSub")),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Crystal")),
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}

			MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
			MMOUI::PostGameEvent(&evt1);
		}
		break;
	case GUILD_ATTR_SILVER:
		{
			UINT32 oldValue = baseInfo.Silver;
			baseInfo.Silver = value;
			MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);
			UINT32 newValue = baseInfo.Silver;

			// tips
			if (newValue > oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResAdd")),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Silver")),
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}
			else if (newValue < oldValue)
			{
				FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildResSub")),
					*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Silver")),
					abs((int)(newValue - oldValue)));
				MMOChatManager::getSingleton().AddSystemMessage(text);
			}

			MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
			MMOUI::PostGameEvent(&evt1);
		}
		break;
	case GUILD_ATTR_PROSPERITY:
		{
			baseInfo.Prosperity = value;
			MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);

			MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
			MMOUI::PostGameEvent(&evt1);
		}
		break;
	}

	// update basic info
	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
	MMOUI::PostGameEvent(&evt2);
}

void MMOGuildManager::NotifyUpdateGuildSalaryPercent(const UINT8 (&salaryPercent)[GUILD_POSITION_COUNT])
{
	MMOGuildData::getSingleton().UpdateGuildSalaryPercent(salaryPercent);
	
	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildSalary);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyUpdateGuildAllyList(GUILD_DIPLOMAT_INFO* list, UINT8 size)
{
	MMOGuildData::getSingleton().UpdateGuildAllyList(list, size);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildAllyList);
	MMOUI::PostGameEvent(&evt);

	_UpdateAllGuildTitle();
}

void MMOGuildManager::NotifyUpdateGuildEnemyList(GUILD_DIPLOMAT_INFO* list, UINT8 size)
{
	MMOGuildData::getSingleton().UpdateGuildEnemyList(list, size);
	
	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildEnemyList);
	MMOUI::PostGameEvent(&evt);

	_UpdateAllGuildTitle();
}

void MMOGuildManager::NotifyAddGuildAlly(UINT32 id, const FString& name, UINT8 level, UINT8 memberCount, UINT32 leftTime)
{
	MMOGuildData::getSingleton().AddGuildAlly(id, name, level, memberCount, leftTime);
	QueryGuildInfo(id);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildAllyList);
	MMOUI::PostGameEvent(&evt);

	_UpdateAllGuildTitle();

	// request guild achievement info
	CMessageHandler::GetInstance().Send_GuildAchievement();
}

void MMOGuildManager::NotifyDelGuildAlly(UINT32 id)
{
	MMOGuildData::getSingleton().DelGuildAlly(id);
	QueryGuildInfo(id);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildAllyList);
	MMOUI::PostGameEvent(&evt);

	_UpdateAllGuildTitle();

	// request guild achievement info
	CMessageHandler::GetInstance().Send_GuildAchievement();
}

void MMOGuildManager::NotifyDelGuildAllySuccess(UINT32 id)
{
	GuildQueryInfo info;
	if (MMOGuildData::getSingleton().GetGuildInfoById(id, info))
	{
		FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.DelGuildAllySuccess")), *(info.Name));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.DelGuildAllySuccess"));
	}
}

void MMOGuildManager::NotifyAddGuildEnemy(UINT32 id, const FString& name, UINT8 level, UINT8 memberCount, UINT32 leftTime)
{
	MMOGuildData::getSingleton().AddGuildEnemy(id, name, level, memberCount, leftTime);
	QueryGuildInfo(id);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildEnemyList);
	MMOUI::PostGameEvent(&evt);

	_UpdateAllGuildTitle();

	// request guild achievement info
	CMessageHandler::GetInstance().Send_GuildAchievement();
}

void MMOGuildManager::NotifyDelGuildEnemy(UINT32 id)
{
	MMOGuildData::getSingleton().DelGuildEnemy(id);
	QueryGuildInfo(id);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildEnemyList);
	MMOUI::PostGameEvent(&evt);

	_UpdateAllGuildTitle();

	// request guild achievement info
	CMessageHandler::GetInstance().Send_GuildAchievement();
}

void MMOGuildManager::NotifyUpdateGuildApplyAllyList(APPLY_ALLIANCE_INFO* list, UINT8 size)
{
	MMOGuildData::getSingleton().UpdateGuildApplyAllyList(list, size);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildApplyAllyList);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyExitGuild()
{
	MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.QuitSuccess")),MsgInfoType_Table,TEXT("MMOGame.Guild.QuitSuccess"));

	UMMOUIBase* panel = MMOUI::FindUIPanel(TEXT("GuildMain"));
	if (panel)
	{
		panel->Show(FALSE);
	}
	_ResetLocalGuildInfo();
	MMOAchievementData::getSingleton().ClearGuildAchievementInfo();

	MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_UpdateAchievementRewards, 0);
	MMOUI::PostGameEvent(&evtJourney);
}


void MMOGuildManager::NotifyBanishGuild()
{
	MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Banish")),MsgInfoType_Table,TEXT("MMOGame.Guild.Banish"));
	_ResetLocalGuildInfo();
	MMOAchievementData::getSingleton().ClearGuildAchievementInfo();

	MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_UpdateAchievementRewards, 0);
	MMOUI::PostGameEvent(&evtJourney);
}

void MMOGuildManager::NotifyGuildLeaderDemise(UINT32 oldLeaderId, UINT32 newLeaderId)
{
	MMOGuildData::getSingleton().GuildLeaderDemise(newLeaderId);

	// must update the list first, basic info uses the list info
	MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildFullMemberList);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
	MMOUI::PostGameEvent(&evt2);

	FString oldLeaderName = TEXT("");
	FString newLeaderName = TEXT("");
	MMOGuildData::getSingleton().GetGuildFullMemberName(oldLeaderId, oldLeaderName);
	MMOGuildData::getSingleton().GetGuildFullMemberName(newLeaderId, newLeaderName);
	FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.LeaderDemise")), 
		*(oldLeaderName), *(newLeaderName));
	MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.LeaderDemise"));
}

void MMOGuildManager::NotifyDisbandGuild()
{
	MMOUI::GetUIManager()->delegateAddScreenMessage(MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.DisbandSuccess")),MsgInfoType_Table,TEXT("MMOGame.Guild.DisbandSuccess"));

	UMMOUIBase* panel = MMOUI::FindUIPanel(TEXT("GuildMain"));
	if (panel)
	{
		panel->Show(FALSE);
	}

	// clear the Title of all guild members
	GuildFullMemberInfoList guildFullMemberInfoList = MMOGuildData::getSingleton().GetGuildFullMemberInfoList();
	GuildFullMemberInfoList::iterator iter = guildFullMemberInfoList.begin();
	for (; iter != guildFullMemberInfoList.end(); ++ iter )
	{
		_UpdateActorGuildTitle(iter->Id, TEXT(""));
	}

	_ResetLocalGuildInfo();
	MMOAchievementData::getSingleton().ClearGuildAchievementInfo();

	MMOGameEvent_Journey evtJourney(MMOGameEvent_Journey::EJET_UpdateAchievementRewards, 0);
	MMOUI::PostGameEvent(&evtJourney);
}

void MMOGuildManager::NotifyUpdateGuildBuildingList(const G_B_DATA (&arrBuilding)[GUILD_MAX_BUILDING_NUM])
{
	MMOGuildData::getSingleton().UpdateGuildBuildingList(arrBuilding);
}

void MMOGuildManager::NotifyUpdateGuildBuildingUpgradeList(const INT32 (&arrBuildingUpTime)[GUILD_B_NUM])
{
	//MMOGuildData::getSingleton().UpdateGuildBuildingList(arrBuildingUpTime);
	//MMOGuildData::getSingleton().ClearGuildBuildingUpgradeList();
	//for (INT i = GUILD_B_MAIN; i < GUILD_B_NUM; ++ i)
	//{
	//	GuildBuildingData buildingData;
	//	if (MMOGuildData::getSingleton().GetGuildBuildingData(i, 1, buildingData))
	//	{
	//		MMOGuildData::getSingleton().AddGuildBuilding(buildingData.BuildingId, buildingData.Level);

	//		if (arrBuildingUpTime[i] > 0)
	//		{
	//			GuildBuildingInfo upInfo;
	//			upInfo.BuildingId = buildingData.BuildingId;
	//			upInfo.BuildingLv = buildingData.Level;
	//			upInfo.UpgradeLeftTime = arrBuildingUpTime[i];

	//			__time32_t localTime;
	//			_time32(&localTime);
	//			_localtime32(&localTime);
	//			upInfo.UpgradeEndTime = localTime + arrBuildingUpTime[i];
	//			
	//			MMOGuildData::getSingleton().AddGuildBuildingUpgradeInfo(upInfo);
	//		}
	//	}
	//}
}

void MMOGuildManager::NotifyUpdateGuildWelfareInfoList(GUILD_BUY_AURA_RECORD_INFO* welfareInfoList, INT16 count)
{
	for (INT i = 0; i < count; ++ i)
	{
		MMOGuildData::getSingleton().UpdateGuildWelfareAlreadyBoughtTime(welfareInfoList[i].nType, welfareInfoList[i].byCount);
	}

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildWelfareList);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyUpdateGuildWelfareList(const INT8 (&arrWelfareLv)[GUILD_MAX_WELFARE_AURA_COUNT],  
	const INT8 (&arrWelfareUpId)[GUILD_MAX_UPGRADE_AURA_COUNT], 
	const INT32 (&arrWelfareUpTime)[GUILD_MAX_UPGRADE_AURA_COUNT])
{
	MMOGuildData::getSingleton().UpdateGuildWelfareList(arrWelfareLv, arrWelfareUpId, arrWelfareUpTime);
}

void MMOGuildManager::NotifyUpgradeGuildBuilding(UINT64 buildingId, UINT32 buildingType, INT32 time)
{
	if (!MMOGuildData::getSingleton().IsGuildBuildingUpgrading(buildingId))
	{
		GuildBuildingInfo upInfo;
		upInfo.BuildingId = buildingId;
		upInfo.BuildingType = buildingType;
		upInfo.BuildingLv = MMOGuildData::getSingleton().GetGuildBuildingLevel(buildingId);
		upInfo.UpgradeLeftTime = time;

		__time32_t localTime;
		_time32(&localTime);
		_localtime32(&localTime);
		upInfo.UpgradeEndTime = localTime + time;

		MMOGuildData::getSingleton().AddGuildBuildingUpgradeInfo(upInfo);

		GuildBuildingData buildingData;
		if (MMOGuildData::getSingleton().GetGuildBuildingData(upInfo.BuildingType, upInfo.BuildingLv, buildingData))
		{
			FString text = TEXT("");
			if (upInfo.BuildingLv == 0)
			{
				text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.Building")), 
					*(buildingData.Name));
			}
			else
			{
				text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeBuildingNow")), 
					*(buildingData.Name));
			}
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.UpgradeBuildingNow"));
		}
	}

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryUpgradeList);
	MMOUI::PostGameEvent(&evt);

	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildMoneyEvents);
	MMOUI::PostGameEvent(&evt2);
}

void MMOGuildManager::NotifyUpdateGuildBuilding(UINT64 buildingId, UINT32 buildingType)
{
	if (!MMOGuildData::getSingleton().IsGuildBuildingExist(buildingId))
	{
		// if newly created
		GuildBuildingInfo info;
		info.BuildingId = buildingId;
		info.BuildingType = buildingType;
		info.BuildingLv = 1;
		info.UpgradeLeftTime = 0;
		info.UpgradeEndTime = 0;
		MMOGuildData::getSingleton().AddGuildBuilding(info);
	} 
	else
	{
		MMOGuildData::getSingleton().UpgradeGuildBuilding(buildingId);
	}

	MMOGuildData::getSingleton().DelGuildBuildingUpgradeInfo(buildingId);

	if (buildingType == GUILD_B_MAIN)
	{
		GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
		baseInfo.level = MMOGuildData::getSingleton().GetGuildBuildingLevel(buildingId);
		MMOGuildData::getSingleton().UpdateGuildBaseInfo(baseInfo);

		MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
		MMOUI::PostGameEvent(&evt);
	}

	MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryUpgradeList);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildBuildingList);
	MMOUI::PostGameEvent(&evt2);
	
	MMOGameEvent_Guild evt3(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
	MMOUI::PostGameEvent(&evt3);

	MMOGameEvent_Guild evt4(MMOGameEvent_Guild::EGET_UpdateGuildMoneyEvents);
	MMOUI::PostGameEvent(&evt4);

	GuildBuildingData buildingData;
	if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, 1, buildingData))
	{
		// upgrade tips
		FString text = TEXT("");
		UINT32 curLevel = MMOGuildData::getSingleton().GetGuildBuildingLevel(buildingId);
		if (curLevel == 1)
		{
			text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.CreateBuildingSuccess")), 
				*(buildingData.Name));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.CreateBuildingSuccess"));
		}
		else if (curLevel > 1 && curLevel < buildingData.MaxLevel)
		{
			text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeBuildingSuccess")), 
				*(buildingData.Name), curLevel);
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.UpgradeBuildingSuccess"));
		}
		else
		{
			text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeBuildingTopSuccess")), 
				*(buildingData.Name));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.UpgradeBuildingTopSuccess"));
		}
	}
}

void MMOGuildManager::NotifyDestroyGuildBuilding(UINT64 buildingId, UINT32 buildingType)
{
	MMOGuildData::getSingleton().DelGuildBuilding(buildingId);

	MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildBuildingList);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
	MMOUI::PostGameEvent(&evt2);

	GuildBuildingData buildingData;
	if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, 1, buildingData))
	{
		// demolish tips
		FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.DestroyBuildingSuccess")), 
			*(buildingData.Name));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.DestroyBuildingSuccess"));
	}
}

void MMOGuildManager::NotifyCancelGuildBuildingUpgrade(UINT64 buildingId, UINT32 buildingType)
{
	MMOGuildData::getSingleton().DelGuildBuildingUpgradeInfo(buildingId);

	GuildBuildingData buildingData;
	if (MMOGuildData::getSingleton().GetGuildBuildingData(buildingType, 1, buildingData))
	{
		FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeCancel")), 
			*(buildingData.Name));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.UpgradeCancel"));
	}

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryUpgradeList);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyInteractBuildingNpc(UINT32 npcId, UINT32 npcTempId)
{
	if (MMOGuildData::getSingleton().IsGuildBuildingBase(npcTempId))
	{
		UMMOUIGuildBuildingCreatePanel* createPanel = (UMMOUIGuildBuildingCreatePanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_BuildingCreate);
		if (createPanel)
		{
			createPanel->ShowGuildBuildingCreatePanel(npcId, npcTempId);
		}
	}
	else if (MMOGuildData::getSingleton().IsGuildBuilding(npcTempId))
	{
		UMMOUIGuildBuildingUpgradeSinglePanel* upgradePanel = (UMMOUIGuildBuildingUpgradeSinglePanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_BuildingUpgradeSingle);
		if (upgradePanel)
		{
			upgradePanel->ShowGuildBuildingUpgradePanel(npcId, npcTempId);
		}
	}
}

void MMOGuildManager::NotifyUpgradeGuildWelfare(UINT32 welfareId, INT32 time)
{
	if (!MMOGuildData::getSingleton().IsGuildWelfareUpgrading(welfareId))
	{
		GuildWelfareUpgradeInfo upInfo;
		upInfo.WelfareId = welfareId;
		upInfo.WelfareLv = MMOGuildData::getSingleton().GetGuildWelfareLevel(welfareId);
		upInfo.UpgradeLeftTime = time;

		__time32_t localTime;
		_time32(&localTime);
		_localtime32(&localTime);
		upInfo.UpgradeEndTime = localTime + time;

		MMOGuildData::getSingleton().AddGuildWelfareUpgradeInfo(upInfo);

		GuildWelfareData welfareData;
		if (MMOGuildData::getSingleton().GetGuildWelfareData(welfareId, 0, welfareData))
		{
			FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeWelfareNow")),
				*(welfareData.Name));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.UpgradeWelfareNow"));
		}
	}

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryUpgradeList);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyUpdateGuildWelfare(UINT32 welfareId, UINT32 welfareLv)
{
	MMOGuildData::getSingleton().UpgradeGuildWelfare(welfareId, welfareLv);
	MMOGuildData::getSingleton().DelGuildWelfareUpgradeInfo(welfareId);

	MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryUpgradeList);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildWelfareList);
	MMOUI::PostGameEvent(&evt2);

	GuildWelfareData welfareData;
	if (MMOGuildData::getSingleton().GetGuildWelfareData(welfareId, welfareLv, welfareData))
	{
		// upgrade tips
		FString text = TEXT("");
		if (welfareLv < welfareData.MaxLevel)
		{
			text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeWelfareSuccess")), 
				*(welfareData.Name), welfareLv);
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.UpgradeWelfareSuccess"));
		}
		else
		{
			text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeWelfareTopSuccess")), 
				*(welfareData.Name));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.UpgradeWelfareTopSuccess"));
		}
	}
}

void MMOGuildManager::NotifyCancelGuildWelfareUpgrade(UINT32 welfareId)
{
	MMOGuildData::getSingleton().DelGuildWelfareUpgradeInfo(welfareId);

	GuildWelfareData welfareData;
	if (MMOGuildData::getSingleton().GetGuildWelfareData(welfareId, 0, welfareData))
	{
		FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.UpgradeCancel")),
			*(welfareData.Name));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.UpgradeCancel"));
	}

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryUpgradeList);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyPurchaseGuildWelfare(UINT32 welfareId, UINT32 alreadyBuyTime)
{
	MMOGuildData::getSingleton().UpdateGuildWelfareAlreadyBoughtTime(welfareId, alreadyBuyTime);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildWelfareList);
	MMOUI::PostGameEvent(&evt);

	GuildWelfareData welfareData;
	if (MMOGuildData::getSingleton().GetGuildWelfareData(welfareId, 0, welfareData))
	{
		FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.BuyWelfare")), 
			*(welfareData.Name));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.BuyWelfare"));
	}
}

void MMOGuildManager::NotifyUpdateGuildDungeonList(GUILD_DUNGEON_INFO* list, UINT32 size)
{
	// this protocol is unused, the NPC lists all dungeons
}

void MMOGuildManager::NotifyOpenDungeonSuccess()
{
	FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.OpenDungeonSuccess"));
	MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.OpenDungeonSuccess"));
}

void MMOGuildManager::NotifyDungeonEnd(UINT32 bossId, GUILD_DUNGEON_AWARD_INFO* list, UINT8 size)
{
	MMOGuildData::getSingleton().UpdateGuildDungeonAwardList(list, size);
	
	if (MMOUI::GetUIManager())
	{
		UMMOUIGuildDungeonEndPanel* guildDungeonEndPanel = (UMMOUIGuildDungeonEndPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_DungeonEnd);
		if (guildDungeonEndPanel)
		{
			guildDungeonEndPanel->SetGuildDungeonBoss(bossId);
			guildDungeonEndPanel->Show(TRUE);
		}

		UMMOUIGuildDungeonExitPanel* guildDungeonExitPanel = (UMMOUIGuildDungeonExitPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_DungeonExit);
		if (guildDungeonExitPanel)
		{
			guildDungeonExitPanel->Show(TRUE);
		}
	}
}

void MMOGuildManager::NotifyGuildWarCD(UINT32 time)
{
	// the server sends absolute time here, convert it
	__time32_t lastTime = time;
	_localtime32(&lastTime);

	__time32_t localTime;
	_time32(&localTime);
	_localtime32(&localTime);

	INT realTime = lastTime - localTime;
	if (realTime < 0)
	{
		realTime = 0;
	}

	_GuildWarCDTime = realTime;
	_GuildWarCDProgress = 0.0f;

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildWarCD);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyGuildWarState(UINT8 state,
		UINT32 attackId,
		const FString& attackName,
		UINT32 defendId, 
		const FString& defendName,
		INT32 lastTime)
{
	_CurGuildWarState = state;
	_CurAttackGuildName = attackName;
	_CurDefendGuildName = defendName;

	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	switch (state)
	{
	case GUILD_WAR_STAGE_NONE:
		{
			if (baseInfo.Id == attackId)
			{
				MMOGuildData::getSingleton().SetMyGuildWarAttackId(0);
			}

			if (baseInfo.Id == defendId)
			{
				MMOGuildData::getSingleton().SetMyGuildWarDefendId(0);
			}
		}
		break;
	case GUILD_WAR_STAGE_DECLARE:
		{
			if (baseInfo.Id == attackId)
			{
				MMOGuildData::getSingleton().SetMyGuildWarAttackId(defendId);

				INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
				FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo(curMapId);
				if (mapInfo.ServerMapType != E_est_Guild)
				{
					// message orb
					MMOMsgButtonManager_Parms Parms(EC_EventParm);
					Parms.Parm1 = GMBT_ATTACK;
					Parms.Parm2 = defendId;
					MMOMsgButtonManager::getSingleton().AddNewButton(_OpenGuildWarStartPanelAttackDelegate, Parms, E_MsgBtnShow_Guild);
				}
			}

			if (baseInfo.Id == defendId)
			{
				MMOGuildData::getSingleton().SetMyGuildWarDefendId(attackId);

				INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
				FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo(curMapId);
				if (mapInfo.ServerMapType != E_est_Guild)
				{
					// message orb
					MMOMsgButtonManager_Parms Parms(EC_EventParm);
					Parms.Parm1 = GMBT_DEFEND;
					Parms.Parm2 = attackId;
					MMOMsgButtonManager::getSingleton().AddNewButton(_OpenGuildWarStartPanelDefendDelegate, Parms, E_MsgBtnShow_Guild);
				}
			}

			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.DeclarWarSuccess"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.DeclarWarSuccess"));

			UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
			if (panel)
			{
				panel->Show(TRUE);
				panel->UpdateGuildWarHintInfo(_CurAttackGuildName, _CurDefendGuildName, *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ReadyCount")), lastTime);
				
				_GuildWarReadyTime = lastTime;
				_GuildWarReadyProgress = 0.0f;
			}
		}
		break;
	case GUILD_WAR_STAGE_BEGIN:
		{
			if (baseInfo.Id == attackId)
			{
				MMOGuildData::getSingleton().SetMyGuildWarAttackId(defendId);
			}

			if (baseInfo.Id == defendId)
			{
				MMOGuildData::getSingleton().SetMyGuildWarDefendId(attackId);
			}

			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.WarBegin"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.WarBegin"));

			UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
			if (panel)
			{
				panel->Show(TRUE);
				panel->UpdateGuildWarHintInfo(_CurAttackGuildName, _CurDefendGuildName, *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.WarCount")), lastTime);

				_GuildWarTime = lastTime;
				_GuildWarProgress = 0.0f;
			}
		}
		break;
	case GUILD_WAR_STAGE_END:
		{
			if (baseInfo.Id == attackId)
			{
				MMOGuildData::getSingleton().SetMyGuildWarAttackId(0);
			}

			if (baseInfo.Id == defendId)
			{
				MMOGuildData::getSingleton().SetMyGuildWarDefendId(0);
			}

			FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.WarEnd"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.WarEnd"));

			UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
			if (panel)
			{
				panel->UpdateGuildWarHintInfo(TEXT(""), TEXT(""), TEXT(""), 0);
				panel->Show(FALSE);
			}
		}
		break;
	}

	QueryGuildInfo(attackId);
	QueryGuildInfo(defendId);

	_UpdateAllGuildTitle();
}

void MMOGuildManager::NotifyGuildBuildingState(UINT8 buildingType, UINT8 state)
{

}

void MMOGuildManager::NotifyGuildContributeSuccess(UINT32 type, UINT32 count)
{
	FString text = TEXT("");
	FString strString = TEXT("");
	switch (type)
	{
	case GUILD_ATTR_MONEY:
		text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ContributeMoney")), count, INT(count / 10));
		strString = TEXT("MMOGame.Guild.ContributeMoney");
		break;
	case GUILD_ATTR_WOOD:
		text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ContributeWood")), count);
		strString = TEXT("MMOGame.Guild.ContributeWood");
		break;
	case GUILD_ATTR_HERB:
		text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ContributeHerb")), count);
		strString = TEXT("MMOGame.Guild.ContributeHerb");
		break;
	case GUILD_ATTR_CRYSTAL:
		text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ContributeCrystal")), count);
		strString = TEXT("MMOGame.Guild.ContributeCrystal");
		break;
	case GUILD_ATTR_SILVER:
		text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ContributeSilver")), count);
		strString = TEXT("MMOGame.Guild.ContributeSilver");
		break;
	}

	MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,strString);
}

void MMOGuildManager::NotifyGuildWarResult(GUILD_BATTLE_ACHIEVEMENT_DATA* list, UINT8 size)
{
	MMOGuildData::getSingleton().UpdateGuildWarResultList(list, size);

	_GuildWarEndTime = 30;
	_GuildWarEndProgress = 0.0f;

	MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarEnd, TRUE, "");
	/*
	// only members in the defending guild's territory can see the result
	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo(curMapId);
	if (mapInfo.ServerMapType == E_est_Guild)
	{
		if (_CurSceneGuildId != 0 && _CurSceneGuildId == DefenderId)
		{
			MMOUI::GetUIManager()->ShowPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarEnd, TRUE, "");
		}
	}
	*/
}

void MMOGuildManager::NotifyGuildMemberSalaryChange(UINT32 salary)
{
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	UINT32 oldSalary = baseInfo.Salary;
	MMOGuildData::getSingleton().UpdateMyGuildSalary(salary);

	// tips
	FString text = TEXT("");
	if (salary > oldSalary)
	{
		text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildSalaryAdd")), abs((int)(salary - oldSalary)));
	}  
	else if (salary < oldSalary)
	{

		text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildSalarySub")), abs((int)(salary - oldSalary)));
	}
	MMOChatManager::getSingleton().AddSystemMessage(text);

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyGuildWarRelation(UINT32 attackId, UINT32 defendId)
{
	if (attackId > 0)
	{
		MMOGuildData::getSingleton().SetMyGuildWarAttackId(attackId);
		QueryGuildInfo(attackId);
	}

	if (defendId > 0)
	{
		MMOGuildData::getSingleton().SetMyGuildWarDefendId(defendId);
		QueryGuildInfo(defendId);
	}
}

void MMOGuildManager::NotifyPlayerMoneyChange(EMONEY_TYPE moneyType, INT64 DeltaMoney)
{
	// update fund distribution
	MMOGameEvent_Guild evt1(MMOGameEvent_Guild::EGET_UpdateGuildSalary);
	MMOUI::PostGameEvent(&evt1);

	MMOGameEvent_Guild evt2(MMOGameEvent_Guild::EGET_UpdateGuildTerritoryInfo);
	MMOUI::PostGameEvent(&evt2);

	// update basic info
	MMOGameEvent_Guild evt3(MMOGameEvent_Guild::EGET_UpdateGuildBaseInfo);
	MMOUI::PostGameEvent(&evt3);
}

void MMOGuildManager::NotifyQueryGuildInfo(M2C_QUERY_GUILD_NAME_ACK* msg)
{
	if (!msg)
	{
		return;
	}

	MMOGuildData::getSingleton().AddOrUpdateGuildInfo(msg);

	NeedUpdateGuildNameRoleMap::iterator iter = _NeedUpdateGuildNameRoleMap.begin();
	for (; iter != _NeedUpdateGuildNameRoleMap.end();)
	{
		GuildQueryInfo info;
		if (MMOGuildData::getSingleton().GetGuildInfoById(iter->second, info))
		{
			_UpdateActorGuildTitle(iter->first, info.Name);
			iter = _NeedUpdateGuildNameRoleMap.erase(iter);
		}
		else
		{
			iter ++;
		}
	}
}

void MMOGuildManager::EnterScene(INT32 sceneTempId, INT32 sceneId)
{
	_CurSceneGuildId = 0;
	FExportMap mapInfo = UGlobalMapInfo::GetSingleton()->FindMapInfo(sceneTempId);
	if (mapInfo.ServerMapType == E_est_Guild)
	{
		_CurSceneGuildId = sceneId;
		GuildQueryInfo info;
		if (MMOGuildData::getSingleton().GetGuildInfoById(_CurSceneGuildId, info))
		{
			// enter xxx guild's territory
			FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.EnterTerritory")), *FString(info.Name));
			MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.EnterTerritory"));

			SET_WARN_COLOR(COLOR_CYAN);
			warnf(NAME_Log, 
				TEXT("[CMessageHandler::Parse_g2c_enter_scene] Enter guild map: %s [%d]"),
				*FString(info.Name), info.Id);
			CLEAR_WARN_COLOR();
		}

		switch (_CurGuildWarState)
		{
		case GUILD_WAR_STAGE_DECLARE:
			{
				UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
				if (panel)
				{
					panel->Show(TRUE);
					panel->UpdateGuildWarHintInfo(_CurAttackGuildName, _CurDefendGuildName, *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.ReadyCount")), _GuildWarReadyTime);
				}
			}
			break;
		case GUILD_WAR_STAGE_BEGIN:
			{
				UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
				if (panel)
				{
					panel->Show(TRUE);
					panel->UpdateGuildWarHintInfo(_CurAttackGuildName, _CurDefendGuildName, *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.WarCount")), _GuildWarTime);
				}
			}
			break;
		case GUILD_WAR_STAGE_END:
			{
				UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
				if (panel)
				{
					panel->UpdateGuildWarHintInfo(TEXT(""), TEXT(""), TEXT(""), 0);
					panel->Show(FALSE);
				}
			}
			break;
		}
	}

	if (_SabacState > 0)
	{
		FString curSabacOccupyGuildName = TEXT("");
		GetSabacOccupyGuildName(curSabacOccupyGuildName);

		UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
		if (panel)
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			// #19562
			//panel->Show(mainPlayer != NULL && mainPlayer->inSabacZoneCount > 0);
			panel->Show(FALSE);
			panel->UpdateGuildWarHintInfo(TEXT(""), curSabacOccupyGuildName, TEXT("Time left in the Conquest War: "), _GuildWarTime);
		}
	}
}

void MMOGuildManager::NotifyGuildFullMemberForbitChat(UINT32 roleId, UBOOL isForbid)
{
	MMOGuildData::getSingleton().SetGuildFullMemberForbitChat(roleId, isForbid);

	FString memberName = TEXT("");
	MMOGuildData::getSingleton().GetGuildFullMemberName(roleId, memberName);
	FString text = TEXT("");
	if (isForbid)
	{
		text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildForbitChatOn")), *memberName);
	}
	else
	{
		text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildForbitChatOff")), *memberName);
	}
	MMOChatManager::getSingleton().AddChatMessage(ECC_Guild, text, TEXT(""));

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildFullMemberList);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyGuildDrawSalarySuccess(INT32 salary)
{
	// tips
	FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.GuildDrawSalary")), salary);
	MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Guild.GuildDrawSalary"));
}

UBOOL MMOGuildManager::GetCurGuildWarViewInfo(GuildViewInfo& info)
{
	if (_CurGuildWarViewInfo.Id == 0)
	{
		return FALSE;
	}

	info = _CurGuildWarViewInfo;
	return TRUE;
}

void MMOGuildManager::NotifyGuildWarSearchBegin(const GuildViewInfo& info)
{
	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_BeginSearch);
	MMOUI::PostGameEvent(&evt);
	_CurGuildWarViewInfo = info;
	_GuildWarViewProgress = 0.0f;
	_GuildWarViewDelayTime = 3;
}

void MMOGuildManager::NotifyGuildWarSearchEnd()
{
	UMMOUIGuildWarViewPanel* viewPanel = (UMMOUIGuildWarViewPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarView);
	if (viewPanel)
	{
		GuildQueryInfo info;
		if (MMOGuildData::getSingleton().GetGuildInfoById(_CurGuildWarViewInfo.Id, info))
		{
			_CurGuildWarViewInfo.Name = info.Name;
		}

		PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().GetPlayerBaseInfo(_CurGuildWarViewInfo.LeaderId);
		if (playerInfo)
		{
			_CurGuildWarViewInfo.LeaderName = playerInfo->_RoleName;
			viewPanel->ShowGuildWarInfoPanel();

			// refresh match count
			CMessageHandler::GetInstance().Send_MoreGuildInfo();
		}
	}

	_GuildWarViewProgress = 0.0f;
	_GuildWarViewDelayTime = 0;
	// invalidate cached data
	_CurGuildWarViewInfo.Id = 0;

	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_CompleteSearch);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::SetOpenGuildWarStartPanelAttackDelegate(FScriptDelegate fun)
{
	_OpenGuildWarStartPanelAttackDelegate = fun;
}

void MMOGuildManager::SetOpenGuildWarStartPanelDefendDelegate(FScriptDelegate fun)
{
	_OpenGuildWarStartPanelDefendDelegate = fun;
}

void MMOGuildManager::SetOpenGuildMainPanelApplyAllyDelegate(FScriptDelegate fun)
{
	_OpenGuildMainPanelApplyAllyDelegate = fun;
}

void MMOGuildManager::NotifyUpdateMoreGuildInfo(INT32 matchWarConsumption, UINT8 matchCount)
{
	UMMOUIGuildMainPanel* mainPanel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_Main);
	if (mainPanel)
	{
		mainPanel->UpdateGuildWarSearchInfo(matchWarConsumption, matchCount);
	}
}

void MMOGuildManager::NotifyUpdateSceneVariable(INT32 mapId, UINT16 varIndex, INT32 varValue)
{
	if (mapId == SABAC_MAP_ID)
	{
		if (varIndex == SABAC_SCENE_VARIABLE_ID_START)
		{
			NotifySabacState(varValue);
		}
	}
	else if (mapId == GUILD_OCCUPY_MAP_ID_1 ||
		mapId == GUILD_OCCUPY_MAP_ID_2 ||
		mapId == GUILD_OCCUPY_MAP_ID_3 ||
		mapId == GUILD_OCCUPY_MAP_ID_4)
	{
		if (varIndex == GUILD_OCCUPY_VARIABLE_STATE)
		{
			UpdateOccupyState(mapId, varValue);
		}
		else if (varIndex == GUILD_OCCUPY_VARIABLE_WINNER)
		{
			UpdateOccupyGuild(mapId, varValue);
		}

		MMOGameEvent_Any evt(GameEvent_GuildOccupy);
		MMOUI::PostGameEvent(&evt);
	}
}

void MMOGuildManager::GetSabacOccupyGuildName(FString& name)
{
	name = TEXT("No guild controls the inner city yet");
	UINT32 curSabacOccupyGuildId = MMOGuildData::getSingleton().GetSabacOccupyGuildId();
	if (curSabacOccupyGuildId > 0)
	{
		name = TEXT("Controlling guild: ");
		GuildQueryInfo info;
		if (MMOGuildData::getSingleton().GetGuildInfoById(curSabacOccupyGuildId, info))
		{
			name += info.Name;
		}
		else
		{
			name += FString(TEXT("Unknown guild"));
		}
	}
}

UBOOL MMOGuildManager::IsSabacOccupyGuild(const FString& name)
{
	UINT32 curSabacOccupyGuildId = MMOGuildData::getSingleton().GetSabacOccupyGuildId();
	if (curSabacOccupyGuildId > 0)
	{
		GuildQueryInfo info;
		if (MMOGuildData::getSingleton().GetGuildInfoById(curSabacOccupyGuildId, info))
		{
			if (info.Name == name)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void MMOGuildManager::NotifyUpdateSabacEnrollList(G2C_SYNC_SABAC_ENROLL_LIST::ENROLL_DATA* list, UINT16 size)
{
	MMOGuildData::getSingleton().UpdateSabacEnrollGuildList(list, size);

	UMMOUISabacEnrollViewPanel* panel = (UMMOUISabacEnrollViewPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_SabacEnroll);
	if (panel)
	{
		panel->Show(TRUE);
	}
}

void MMOGuildManager::NotifySabacEnrollGuildId(UINT32 guildId)
{
	QueryGuildInfo(guildId);
	
	GuildBaseInfo baseInfo = MMOGuildData::getSingleton().GetGuildBaseInfo();
	if (baseInfo.Id == guildId)
	{
		FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SabacEnrollSuccess"));
		MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.SabacEnrollSuccess"));
	}

	GuildQueryInfo info;
	if (MMOGuildData::getSingleton().GetGuildInfoById(guildId, info))
	{
		FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SabacEnrollOther")), *(info.Name));
			
		FString marqueeColor;
		GConfig->GetString(TEXT("SabacColor"), TEXT("SabacMarqueeColor"), marqueeColor, GUIColorIni);
		INT marqueeRepeatTime = 0;
		GConfig->GetInt(TEXT("SabacColor"), TEXT("SabacMarqueeRepeatTime"), marqueeRepeatTime, GUIColorIni);
		INT marqueeShowTime = 0;
		GConfig->GetInt(TEXT("SabacColor"), TEXT("SabacMarqueeShowTime"), marqueeShowTime, GUIColorIni);

		MMOGameEvent_MarqueeNoticeInfo evt(text, marqueeColor, marqueeRepeatTime, marqueeShowTime);
		MMOUI::PostGameEvent(&evt);
	}

	// update name color
	_UpdateAllGuildTitle();
}

void MMOGuildManager::_UpdateAllGuildInfo()
{
	// update the state of all known guilds
	QueryGuildInfoByIdMap queryGuildInfoMap = MMOGuildData::getSingleton().GetQueryGuildInfoByIdMap();
	QueryGuildInfoByIdMap::iterator iter = queryGuildInfoMap.begin();
	for (; iter != queryGuildInfoMap.end(); ++ iter)
	{
		UINT32 id = iter->first;
		QueryGuildInfo(id);
	}
}

void MMOGuildManager::NotifyUpdateSabacOccupyGuild(UINT32 guildId, UBOOL showMarquee)
{
	QueryGuildInfo(guildId);
	MMOGuildData::getSingleton().UpdateSabacOccupyGuild(guildId);

	if (showMarquee)
	{
		GuildQueryInfo info;
		if (MMOGuildData::getSingleton().GetGuildInfoById(guildId, info))
		{
			FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SabacOccupy")), *(info.Name));

			FString marqueeColor;
			GConfig->GetString(TEXT("SabacColor"), TEXT("SabacMarqueeColor"), marqueeColor, GUIColorIni);
			INT marqueeRepeatTime = 0;
			GConfig->GetInt(TEXT("SabacColor"), TEXT("SabacMarqueeRepeatTime"), marqueeRepeatTime, GUIColorIni);
			INT marqueeShowTime = 0;
			GConfig->GetInt(TEXT("SabacColor"), TEXT("SabacMarqueeShowTime"), marqueeShowTime, GUIColorIni);

			MMOGameEvent_MarqueeNoticeInfo evt(text, marqueeColor, marqueeRepeatTime, marqueeShowTime);
			MMOUI::PostGameEvent(&evt);
		}
	}

	if (_SabacState > 0)
	{
		FString curSabacOccupyGuildName = TEXT("");
		GetSabacOccupyGuildName(curSabacOccupyGuildName);

		UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
		if (panel)
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			// #19562
			//panel->Show(mainPlayer != NULL && mainPlayer->inSabacZoneCount > 0);
			panel->Show(FALSE);
			panel->UpdateGuildWarHintInfo(TEXT(""), curSabacOccupyGuildName, TEXT("Time left in the Conquest War: "), _GuildWarTime);
		}
	}

	_UpdateAllGuildInfo();
	// update name color
	_UpdateAllGuildTitle();
}

void MMOGuildManager::NotifySabacEnd(UINT32 oldGuildId, UINT32 newGuildId)
{
	if (oldGuildId == newGuildId)
	{
		// the server no longer saves the previous Sabak lord's info, so defending the title cannot be prompted
		//GuildQueryInfo info;
		//if (MMOGuildData::getSingleton().GetGuildInfoById(newGuildId, info))
		//{
		//	FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SabacDefendSuccess")), *(info.Name));
		//	
		//	FString marqueeColor;
		//	GConfig->GetString(TEXT("SabacColor"), TEXT("SabacMarqueeColor"), marqueeColor, GUIColorIni);
		//	INT marqueeRepeatTime = 0;
		//	GConfig->GetInt(TEXT("SabacColor"), TEXT("SabacMarqueeRepeatTime"), marqueeRepeatTime, GUIColorIni);
		//	INT marqueeShowTime = 0;
		//	GConfig->GetInt(TEXT("SabacColor"), TEXT("SabacMarqueeShowTime"), marqueeShowTime, GUIColorIni);

		//	MMOGameEvent_MarqueeNoticeInfo evt(text, marqueeColor, marqueeRepeatTime, marqueeShowTime);
		//	MMOUI::PostGameEvent(&evt);
		//}
	}

	GuildQueryInfo info;
	if (MMOGuildData::getSingleton().GetGuildInfoById(newGuildId, info))
	{
		FString text = FString::Printf(*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SabacSuccess")), *(info.Name));

		FString marqueeColor;
		GConfig->GetString(TEXT("SabacColor"), TEXT("SabacMarqueeColor"), marqueeColor, GUIColorIni);
		INT marqueeRepeatTime = 0;
		GConfig->GetInt(TEXT("SabacColor"), TEXT("SabacMarqueeRepeatTime"), marqueeRepeatTime, GUIColorIni);
		INT marqueeShowTime = 0;
		GConfig->GetInt(TEXT("SabacColor"), TEXT("SabacMarqueeShowTime"), marqueeShowTime, GUIColorIni);

		MMOGameEvent_MarqueeNoticeInfo evt(text, marqueeColor, marqueeRepeatTime, marqueeShowTime);
		MMOUI::PostGameEvent(&evt);
	}

	_UpdateAllGuildInfo();
}

void MMOGuildManager::NotifySabacState(UINT32 sabacEndTime)
{
	_SabacState = sabacEndTime > 0 ? 1 : 0;

	if (_SabacState == 0)
	{
		UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
		if (panel)
		{
			panel->UpdateGuildWarHintInfo(TEXT(""), TEXT(""), TEXT(""), 0);
			panel->Show(FALSE);
		}
	}
	else
	{
		FString curSabacOccupyGuildName = TEXT("");
		GetSabacOccupyGuildName(curSabacOccupyGuildName);

		UMMOUIGuildWarHintPanel* panel = (UMMOUIGuildWarHintPanel*)MMOUI::FindUIPanelEx(MMOUI::GetUIManager()->Panel_Guild_WarHint);
		if (panel)
		{
			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			// #19562
			//panel->Show(mainPlayer != NULL && mainPlayer->inSabacZoneCount > 0);
			panel->Show(FALSE);

			UINT32 leftTime = 0;
			__time32_t localTime;
			_time32(&localTime);
			struct tm *today = _localtime32(&localTime); 
			if (today)
			{
				UINT32 curTime = today->tm_hour * 3600 + today->tm_min * 60 + today->tm_sec;
				UINT32 endTime = sabacEndTime / 100 * 3600 + (sabacEndTime % 100) * 60;
				leftTime = endTime > curTime ? (endTime - curTime) : 0;
			}

			panel->UpdateGuildWarHintInfo(TEXT(""), curSabacOccupyGuildName, TEXT("Time left in the Conquest War: "), leftTime);

			_GuildWarTime = leftTime;
			_GuildWarProgress = 0.0f;
		}
	}
	
	_UpdateAllGuildInfo();
	// update name color
	_UpdateAllGuildTitle();
}

void MMOGuildManager::NotifyGuildEventsHint(UINT8 type)
{
	switch (type)
	{
	case E_GUILD_NOTIFY_ROLE_APP:
		{
			class AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainPlayer->ServerID);
				if (post == GUILD_POSITION_LEADER ||
					post == GUILD_POSITION_SUBLEADER)
				{
					UMMOUIManager* uiManager = MMOUI::GetUIManager();
					if (uiManager)
					{
						uiManager->delegateUpdateMenuEffect(4, EMenuType_Child_Guild, FALSE, TRUE);
						UMMOUIGuildMainPanel* mainPanel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanelEx(uiManager->Panel_Guild_Main);
						if (mainPanel)
						{
							mainPanel->SetShowPage(2, 2);
						}
					}
				}
			}
		}
		break;
	case E_GUILD_NOTIFY_DIPLOMAT_APP:
		{
			class AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				UINT8 post = MMOGuildData::getSingleton().GetGuildFullMemberPost(mainPlayer->ServerID);
				if (post == GUILD_POSITION_LEADER ||
					post == GUILD_POSITION_SUBLEADER)
				{
					/*
					UMMOUIManager* uiManager = MMOUI::GetUIManager();
					if (uiManager)
					{
						uiManager->delegateUpdateMenuEffect(4, EMenuType_Child_Guild, FALSE, TRUE);
						UMMOUIGuildMainPanel* mainPanel = (UMMOUIGuildMainPanel*)MMOUI::FindUIPanelEx(uiManager->Panel_Guild_Main);
						if (mainPanel)
						{
							mainPanel->SetShowPage(4, 2);
						}
					}
					*/
					// message orb
					MMOMsgButtonManager_Parms Parms(EC_EventParm);
					Parms.Parm1 = GMBT_APPLY_ALLY;
					MMOMsgButtonManager::getSingleton().AddNewButton(_OpenGuildMainPanelApplyAllyDelegate, Parms, E_MsgBtnShow_GuildAlly);
				}
			}
		}
		break;
	}
}

void MMOGuildManager::NotifyApplyAllianceSuccess()
{
	FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SendAllySuccess"));
	MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Normal, TEXT("MMOGame.Guild.SendAllySuccess"));
}

void MMOGuildManager::NotifySabacWinnerLogin(const FString& winnerName)
{
	FString text = FString::Printf(TEXT("Sabak Lord [%s] is online"), *winnerName);
	MMOChatManager::getSingleton().AddChatMessage(ECC_System, text, TEXT(""));
}

void MMOGuildManager::UpdateStatueHint(UBOOL forceUpdate)
{
	SabacStatue statue;
	if (!MMOGuildData::getSingleton().GetStatueData(statue))
	{
		return;
	}

	ULocalPlayer* localPlayer = Cast<ULocalPlayer>(MMOFacade::getSingleton().GetMainController()->Player);
	if(!localPlayer)
	{
		return;
	}

	if (!_StatueActor)
	{
		return;
	}

	UMMOUISceneHint* sceneHint = Cast<UMMOUISceneHint>(MMOUI::FindUIPanel(TEXT("SceneHint")));
	if (!sceneHint)
	{
		return;
	}
	
	FLOAT HudNameDisplayRangeSquared2D = 0.0f;
	GConfig->GetFloat(TEXT("HUD"), TEXT("HudNameDisplayRangeSquared2D"), HudNameDisplayRangeSquared2D, GUIIni);
				
	if (!_StatueHeadName)
	{
		_StatueHeadName = sceneHint->AttachMovie(TEXT("PlayerNameHintText"), FString::Printf(TEXT("PlayerNameHintText%d"), _StatueActor->ServerID), 2);
		_StatueHeadName->AddToRoot();
	}

	if (!_StatueHeadName)
	{
		return;
	}
	
	// distance check, beyond the distance do nothing but hide
	FLOAT dist = (_StatueActor->Location - sceneHint->Player->Location).SizeSquared2D();
	if (dist > HudNameDisplayRangeSquared2D)
	{
		_StatueHeadName->SetVisible(false);
	}
	else
	{
		if (dist <= HudNameDisplayRangeSquared2D && dist > HudNameDisplayRangeSquared2D / 2)
		{
			_StatueHeadName->SetFloat(TEXT("alpha"), (HudNameDisplayRangeSquared2D - dist) / (HudNameDisplayRangeSquared2D / 2));
		}
		else
		{
			_StatueHeadName->SetFloat(TEXT("alpha"), 1.0);
		}
		_StatueHeadName->SetVisible(TRUE);
	}
	_StatueHeadName->SetInt(TEXT("serverID"), _StatueActor->ServerID);

	// position binding, project from 3d to 2d
	FVector2D screenPos;
	FLOAT h = 0;
	if (_StatueActor->CylinderComponent)
	{
		h = _StatueActor->CylinderComponent->CollisionHeight + 80.0f;
	}
	if (_StatueActor->UseingAvatar)
	{
		h += _StatueActor->UseingAvatar->HintNameOffsetZ;
	}
		
	UBOOL success = MMOFacade::getSingleton().GetScreenPixelFromWorld(_StatueActor->Location + FVector(0,0,h), screenPos);
	if (success)
	{
		_StatueHeadName->SetPosition(screenPos.X ,screenPos.Y);
	}

	if (!forceUpdate)
	{
		return;
	}

	sceneHint->eventSetStallTxtInfo(_StatueHeadName,TEXT(""),TEXT(""),FALSE);
	sceneHint->eventSetHpInfo(_StatueHeadName, _StatueActor->ActorAttributes->HP, _StatueActor->ActorAttributes->GetMaxHp(), 2, 0);

	FString NameColor = TEXT("");
	if (_StatueActor->ActorAttributes->ColorEx2.Len() > 0)
	{
		NameColor = _StatueActor->ActorAttributes->ColorEx2;
	}
	else if (_StatueActor->ActorAttributes->ColorEx1.Len() > 0)
	{
		NameColor = _StatueActor->ActorAttributes->ColorEx1;
	}

	sceneHint->eventSetPlayerTxt_Name(_StatueHeadName, sceneHint->GetPlayerNameFrame(_StatueActor), TEXT(""), NameColor);
	sceneHint->eventSetVipState(_StatueHeadName, _StatueActor->GetVipType());
	
	FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	UBOOL HideMapRoleName = curMapInfo.HideRoleName;
	if (HideMapRoleName)
	{
		sceneHint->eventsetEnemy(_StatueHeadName, FALSE);
	}
	else
	{
		sceneHint->eventsetEnemy(_StatueHeadName, FriendManager::getSingleton().InMyEnemyList(_StatueActor->ServerID));
	}

	sceneHint->eventSetPlayerTxt_Title(_StatueHeadName, TEXT("Title"), TEXT(""), TEXT(""));
	// guild
	FString guildName = TEXT("");
	if (!_StatueActor->GuildName.IsEmpty())
	{
		guildName += TEXT("<");
		guildName += _StatueActor->GuildName;
		guildName += TEXT(">");
	}

	sceneHint->eventSetPlayerTxt_Guild(_StatueHeadName, sceneHint->GetPlayerGuildFrame(_StatueActor->GuildName), TEXT(""));
	sceneHint->eventSetSabacState(_StatueHeadName, FALSE);
	sceneHint->eventSetSabacStatueName(_StatueHeadName, statue.RoleName, guildName, TRUE);
	sceneHint->eventUpdateDisplay(_StatueHeadName);
}

void MMOGuildManager::_CreateStatue(const SabacStatue& statue, ANPC* statueBase)
{
	// delete the old one first
	_DestroyStatue();

	if (!statueBase)
	{
		return;
	}

	FVector location = statueBase->Location;
	FRotator rotation = statueBase->Rotation;
	
	AActor* actor = MMOFacade::getSingleton().GetMainController()->eventSpawnCharacter(location, rotation);
	if (!actor)
	{
		return;
	}

	_StatueActor = Cast<AMMOPlayer>(actor);
	if (!_StatueActor)
	{
		return;
	}

	_StatueActor->SceneObjectAppearType = ESOAT_InRange;
	_StatueActor->ServerID = -1;
	_StatueActor->ActorName = statue.RoleName;
	PlayerBaseInfo* playerInfo = MMOFacade::getSingleton().GetPlayerBaseInfo(statue.RoleId);
	if (playerInfo)
	{
		_StatueActor->GuildName = playerInfo->_GuildName;
	}
	_StatueActor->ShowHeadName = TRUE;
	_StatueActor->ShowHpNumber = FALSE;
	_StatueActor->eventSetMesh(statue.RoleClass, statue.RoleSex);

	for (int i = 0; i < NES_VISIBLE_TOTAL; ++ i)
	{
		const UItem* item = MMOFacade::getSingleton().FindItemTemplate(statue.Avatar[i]);
		if (!item)
		{
			warnf(TEXT("item not find [%id]"), statue.Avatar[i]);
			continue;
		}

		if (_StatueActor->ItemManager)
		{
			_StatueActor->ItemManager->RequestTakeOn(-1, CMessageHandler::GetInstance().GetClientEquipTypeByServer(i), true, item);
		}
	}

	for (int i = EBMT_Face; i < EBMT_None; ++ i)
	{
		if (_StatueActor->ItemManager)
		{
			_StatueActor->ItemManager->ChangeEquipMeshMaterial(i, TEXT("MMO_MaterialTemplate.Character.MT_BronzeStatuary"));
		}
	}

	if (_StatueActor->ItemManager)
	{
		_StatueActor->ItemManager->UpdateOwnerData();
	}

	FLOAT scale = 2.5f;
	_StatueActor->SetDrawScale(scale);

	AArmorActor* weapon1 = Cast<AArmorActor>(_StatueActor->GetEquip(EBMT_Weapon));
	if (weapon1)
	{
		weapon1->SetDrawScale(scale);
	}

	AArmorActor* weapon2 = Cast<AArmorActor>(_StatueActor->GetEquip(EBMT_Weapon2));
	if (weapon2)
	{
		weapon2->SetDrawScale(scale);
	}

	FLOAT radius = _StatueActor->GetCollisionRadius();
	FLOAT height = _StatueActor->GetCollisionHeight();
	_StatueActor->SetCollisionSize(radius * scale, height * scale);
	_StatueActor->SetIdleAnim(FName(TEXT("POSE")));

	_StatueActor->setPhysics(PHYS_Interpolating);
	_StatueActor->SetCollisionType(COLLIDE_NoCollision);
	_StatueActor->SetCollision(FALSE, FALSE, FALSE);

	location.Z += 220.0f;
	_StatueActor->SetLocation(location);

	if (_StatueActor->IsA(UActorComponent::StaticClass()))
	{
		statueBase->AttachComponent((UActorComponent*)_StatueActor);
	}
	
	_StatueEffect = Cast<AMMONavitationEmitter>(GWorld->SpawnActor(AMMONavitationEmitter::StaticClass(), 
		NAME_None,
		_StatueActor->GetFootLocation(), 
		_StatueActor->Rotation, 
		NULL, 
		TRUE, 
		0, 
		NULL, 
		NULL, 
		TRUE, 
		NULL));
	if (_StatueEffect)
	{
		if (!_StatueActor->SabacStatueEffect.IsEmpty())
		{
			UParticleSystem* ps = (UParticleSystem*)(_StatueActor->StaticLoadObject(UParticleSystem::StaticClass(), NULL,
				*(_StatueActor->SabacStatueEffect), NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

			if (ps)
			{
				_StatueEffect->SetTemplate(ps);
				_StatueEffect->SetHidden(FALSE);
			}
		}
	}

	UpdateStatueHint(TRUE);
}

void MMOGuildManager::_DestroyStatue()
{
	if (_StatueActor && !_StatueActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
	{
		GWorld->DestroyActor(_StatueActor);
	}
	_StatueActor = NULL;

	if (_StatueEffect && !_StatueEffect->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
	{
		GWorld->DestroyActor(_StatueEffect);
	}
	_StatueEffect = NULL;

	UMMOUISceneHint* sceneHint = Cast<UMMOUISceneHint>(MMOUI::FindUIPanel(TEXT("SceneHint")));
	if (sceneHint && _StatueHeadName)
	{
		sceneHint->eventRemoveChild(_StatueHeadName);
		_StatueHeadName->RemoveFromRoot();
		_StatueHeadName = NULL;
	}
}

void MMOGuildManager::NotifyCreateStatue(const SabacStatue& statue)
{
	// Bug: #19564
//	SabacStatue oldStatue;
//	if (MMOGuildData::getSingleton().GetStatueData(oldStatue))
//	{
//		if (oldStatue.RoleId == statue.RoleId)
//		{
//			FString text = TEXT("Statue already erected");
//			MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT(""));
//			return;
//		}
//	}

	STLInt2IntMap* characterMap = MMOFacade::getSingleton().GetCharacterMap();
	if (characterMap)
	{
		STLInt2IntMap::iterator iterCharacter = characterMap->begin();
		for (; iterCharacter != characterMap->end(); ++ iterCharacter)
		{
			ANPC* npc = Cast<ANPC>(UObject::GetIndexedObject(iterCharacter->second));
			if (npc && npc->GetTemplateID() == _StatueNpcId)
			{
				_CreateStatue(statue, npc);
				break;
			}
		}
	}

	FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.SabacStatueSuccess"));
	MMOUI::GetUIManager()->delegateAddScreenMessage(text, MsgInfoType_Table, TEXT("MMOGame.Guild.SabacStatueSuccess"));
}

void MMOGuildManager::NotifyGuildLeaveCD(UINT32 endTime)
{
	_GuildLeaveEndTime = endTime;
}

INT MMOGuildManager::GetGuildLeaveCD()
{
	if (_GuildLeaveEndTime == 0)
	{
		return 0;
	}

	__time32_t destTime = _GuildLeaveEndTime;
	_localtime32(&destTime);

	__time32_t localTime;
	_time32(&localTime);
	_localtime32(&localTime);

	INT cd = destTime - localTime;
	if (cd < 0)
	{
		cd = 0;
	}

	return cd;
}

void MMOGuildManager::_CreateBuildingBase(ANPC* building, USkeletalMeshComponent* baseMesh)
{
	if (!building)
	{
		return;
	}

	_DestroyBuildingBase(building);
	
	AAbilitySkeletalMeshActor* meshActor = Cast<AAbilitySkeletalMeshActor>(GWorld->SpawnActor(AAbilitySkeletalMeshActor::StaticClass()));
	if (!meshActor)
	{
		return;
	}

	if (meshActor->SkeletalMeshComponent)
	{
		meshActor->SkeletalMeshComponent->SetSkeletalMesh(baseMesh->SkeletalMesh);
		meshActor->SkeletalMeshComponent->SetAnimTreeTemplate(baseMesh->AnimTreeTemplate);
		meshActor->SkeletalMeshComponent->ForceSkelUpdate();
		if (building->Mesh)
		{
			meshActor->SkeletalMeshComponent->SetLightingChannels(building->Mesh->LightingChannels);
			meshActor->SkeletalMeshComponent->SetLightEnvironment(building->Mesh->LightEnvironment);
		}
	}

	meshActor->SetDrawScale(building->DrawScale);
	//FLOAT radius = building->GetCollisionRadius();
	//FLOAT height = building->GetCollisionHeight();
	//meshActor->SetCollisionSize(radius * building->DrawScale, height * building->DrawScale);
	//meshActor->setPhysics(PHYS_None);
	//meshActor->SetCollision(TRUE, TRUE, TRUE);
	//meshActor->SetCollisionType(COLLIDE_BlockAll);
	meshActor->SetLocation(building->GetFootLocation());

	meshActor->AttachComponent(meshActor->SkeletalMeshComponent);

	_GuildBuildingBaseActorMap.insert(std::make_pair(building->ServerID, meshActor->GetIndex()));
}

void MMOGuildManager::_DestroyBuildingBase(ANPC* building)
{
	if (!building)
	{
		return;
	}

	GuildBuildingBaseActorMap::iterator iter = _GuildBuildingBaseActorMap.find(building->ServerID);
	if (iter != _GuildBuildingBaseActorMap.end())
	{
		AAbilitySkeletalMeshActor* meshActor = Cast<AAbilitySkeletalMeshActor>(UObject::GetIndexedObject(iter->second));
		if (meshActor)
		{
			meshActor->DetachComponent(meshActor->SkeletalMeshComponent);
			if(!meshActor->HasAnyFlags(RF_Unreachable|RF_BeginDestroyed|RF_FinishDestroyed))
			{
				GWorld->DestroyActor(meshActor);
			}
			
			_GuildBuildingBaseActorMap.erase(iter);
		}
	}
}

void MMOGuildManager::UpdateGuildBuilding(ANPC* building)
{
	if (MMOGuildData::getSingleton().IsGuildBuilding(building->GetTemplateID()))
	{
		if (building->Template && building->Template->BuildingMesh == NULL)
		{
			_DestroyBuildingBase(building);
		}
	}
}

void MMOGuildManager::QueryGuildInfo(UINT32 guildId)
{
	if (guildId == 0)
	{
		return;
	}

	// if invalid ID
	GuildQueryInvalidIdTimeMap::iterator iter = _GuildQueryInvalidIdTimeMap.find(guildId);
	if (iter != _GuildQueryInvalidIdTimeMap.end())
	{
		return;
	}

	GuildQueryInfo info;
	if (MMOGuildData::getSingleton().GetGuildInfoById(guildId, info))
	{
		CMessageHandler::GetInstance().Send_QueryGuildName(guildId, info.Crc);
	}
	else
	{
		CMessageHandler::GetInstance().Send_QueryGuildName(guildId, 0);
	}
}

void MMOGuildManager::NotifyGuildQueryInvalidId(UINT32 guildId)
{
	__time32_t localTime;
	_time32(&localTime);
	_localtime32(&localTime);

	GuildQueryInvalidIdTimeMap::iterator iter = _GuildQueryInvalidIdTimeMap.find(guildId);
	if (iter != _GuildQueryInvalidIdTimeMap.end())
	{
		iter->second = localTime;
	}
	else
	{
		_GuildQueryInvalidIdTimeMap.insert(std::make_pair(guildId, localTime));
	}
}

void MMOGuildManager::ClearGuildOccupy()
{
	_GuildOccupyStateMap.clear();
	_GuildOccupyStateMap.insert(std::make_pair(GUILD_OCCUPY_MAP_ID_1, 0));
	_GuildOccupyStateMap.insert(std::make_pair(GUILD_OCCUPY_MAP_ID_2, 0));
	_GuildOccupyStateMap.insert(std::make_pair(GUILD_OCCUPY_MAP_ID_3, 0));
	_GuildOccupyStateMap.insert(std::make_pair(GUILD_OCCUPY_MAP_ID_4, 0));

	_GuildOccupyIdMap.clear();
	_GuildOccupyIdMap.insert(std::make_pair(GUILD_OCCUPY_MAP_ID_1, 0));
	_GuildOccupyIdMap.insert(std::make_pair(GUILD_OCCUPY_MAP_ID_2, 0));
	_GuildOccupyIdMap.insert(std::make_pair(GUILD_OCCUPY_MAP_ID_3, 0));
	_GuildOccupyIdMap.insert(std::make_pair(GUILD_OCCUPY_MAP_ID_4, 0));
}

void MMOGuildManager::UpdateOccupyState(UINT32 mapId, UINT8 state)
{
	GuildOccupyStateMap::iterator iter = _GuildOccupyStateMap.find(mapId);
	if (iter != _GuildOccupyStateMap.end())
	{
		iter->second = state;
	}
}

UINT8 MMOGuildManager::GetOccupyState(UINT32 mapId)
{
	GuildOccupyStateMap::iterator iter = _GuildOccupyStateMap.find(mapId);
	if (iter != _GuildOccupyStateMap.end())
	{
		return iter->second;
	}
	return 0;
}

void MMOGuildManager::UpdateOccupyGuild(UINT32 mapId, UINT32 guildId)
{
	GuildOccupyIdMap::iterator iter = _GuildOccupyIdMap.find(mapId);
	if (iter != _GuildOccupyIdMap.end())
	{
		iter->second = guildId;
		QueryGuildInfo(guildId);
	}
}

void MMOGuildManager::GetOccupyGuildName(UINT32 mapId, FString& name)
{
	name = TEXT("No guild controls the current map");
	GuildOccupyIdMap::iterator iter = _GuildOccupyIdMap.find(mapId);
	if (iter != _GuildOccupyIdMap.end())
	{
		UINT32 guildId = iter->second;
		if (guildId > 0)
		{
			GuildQueryInfo info;
			if (MMOGuildData::getSingleton().GetGuildInfoById(guildId, info))
			{
				name = TEXT("Current holder: [");
				name += info.Name;
				name += TEXT("] guild");
			}
		}
	}
}

void MMOGuildManager::NotifyUpdateGuildWarRecords(GUILD_WAR_RECORD_INFO* records, UINT16 size)
{
	MMOGuildData::getSingleton().UpdateGuildWarRecords(records, size);
	
	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_UpdateGuildWarRecord);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyNoMoreGuildWarRecords()
{
	MMOGameEvent_Guild evt(MMOGameEvent_Guild::EGET_NoMoreGuildWarRecord);
	MMOUI::PostGameEvent(&evt);
}

void MMOGuildManager::NotifyGuildJoinWar(UINT8 warCharacter, UINT32 targetGuildID)
{
	_IsGuildWarStart = warCharacter != GUILD_WAR_NONE;
}