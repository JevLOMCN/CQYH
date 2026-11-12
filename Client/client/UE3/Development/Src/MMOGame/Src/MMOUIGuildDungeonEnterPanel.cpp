#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "MMOGuildManager.h"
#include "UITextParser.h"
#include <time.h>


IMPLEMENT_CLASS(UMMOUIGuildDungeonEnterPanel)

void UMMOUIGuildDungeonEnterPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_Guild);
	EventAdapter = new TGameEventAdapter<UMMOUIGuildDungeonEnterPanel>(this, evts);
}

void UMMOUIGuildDungeonEnterPanel::PostOnShow(UBOOL v)
{
	Super::PostOnShow(v);
	if (v)
	{
	}
}

void UMMOUIGuildDungeonEnterPanel::OnNativeGameEvent(struct MMOGameEvent* gameEvent)
{
	if (!IsVisible)
	{
		return;
	}

	if (gameEvent->GetEventType() != GameEvent_Guild)
	{
		return;
	}

	MMOGameEvent_Guild* guildEvent = (MMOGameEvent_Guild*)gameEvent;
	if (!guildEvent)
	{
		return;
	}
}

void UMMOUIGuildDungeonEnterPanel::ShowGuildDungeonEnterPanel(UINT32 dungeonId)
{
	Show(TRUE);
	UpdateGuildDungeonData(dungeonId);
}

void UMMOUIGuildDungeonEnterPanel::UpdateGuildDungeonData(UINT32 dungeonId)
{
	GuildDungeonDataMap guildDungeonDataMap = MMOGuildData::getSingleton().GetGuildDungeonDataMap();
	GuildDungeonDataMap::iterator iter = guildDungeonDataMap.find(dungeonId);
	if (iter != guildDungeonDataMap.end())
	{
		GuildDungeonData data = iter->second;
		UGFxObject* object = GFXPlayer->CreateObject(TEXT("Object"));
		if (object)
		{
			object->SetInt(TEXT("guild_dun_id"), data.DungeonId);
			object->SetString(TEXT("guild_dun_name"), data.DungeonName);
			object->SetString(TEXT("guild_dun_desc"), data.DungeonDesc);

			GuildBuildingData preBuildingData;
			MMOGuildData::getSingleton().GetGuildBuildingData(data.BuildingReq, data.BuildingLvReq, preBuildingData);
			UINT32 preBuildingLv = MMOGuildData::getSingleton().GetGuildBuildingMaxLevel(data.BuildingReq);

			FString buildingReq = FString::Printf(preBuildingLv >= data.BuildingLvReq ? TEXT("<font color='#FFFFFF'>%s: Lv %d</font>") : TEXT("<font color='#FF0000'>%s: Lv %d</font>"), *(preBuildingData.Name), data.BuildingLvReq);
			object->SetString(TEXT("guild_dun_building_lv_req"), buildingReq);

			AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
			if (mainPlayer)
			{
				FString money = TEXT("");
				if ((UINT32)(mainPlayer->GetMoney(MONEYTYPE_02)) < data.MoneyReq)
				{
					money = FString::Printf(TEXT("<font color='#FF0000'>%s</font>"), *UITextParser::HTMLMoneyIconString(MONEYTYPE_02, data.MoneyReq));
				} 
				else
				{
					money = FString::Printf(TEXT("<font color='#FFFFFF'>%s</font>"), *UITextParser::HTMLMoneyIconString(MONEYTYPE_02, data.MoneyReq));
				}
				object->SetString(TEXT("guild_dun_money_req"), money);
			}

			FString memberReq = data.MemberCountLimit > 0 ? appItoa(data.MemberCountLimit) : MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Guild.NoLimit"));
			object->SetString(TEXT("guild_dun_member_count_req"), memberReq);

			object->SetString(TEXT("guild_dun_active_req"), TEXT("0"));

			UGFxObject* awardArray = GFXPlayer->CreateArray();
			if (awardArray)
			{
				for (int i = 0; i < GUILD_DUNGEON_AWARD_NUM; ++ i)
				{
					UGFxObject* awardObject = GFXPlayer->CreateObject(TEXT("Object"));
					if (awardObject)
					{
						UItem* item = MMOFacade::getSingleton().FindItemTemplate(data.Award[i]);
						if (item)
						{
							awardObject->SetInt(TEXT("guild_dun_award_id"), item->GetTemplateID());
							awardObject->SetInt(TEXT("guild_dun_award_index"), i);
							awardObject->SetString(TEXT("guild_dun_award_icon_name"), MMOFacade::getSingleton().GetItemIconNamePersonalized( item ));
							awardObject->SetString(TEXT("guild_dun_award_item_name"), item->GetDisplayName());

							awardArray->SetElementObject(i, awardObject);	
						}
					}
				}

				object->SetObject(TEXT("guild_dun_awards"), awardArray);
			}
			eventCallUpdateGuildDungeonData(object);
		}
	}
}
//////////////////////////////////////////////////////////////////////////

void UMMOUIGuildDungeonEnterPanel::ASEnterDungeon(INT dungeonId)
{
	CMessageHandler::GetInstance().Send_EnterGuildDungeon(dungeonId);
}