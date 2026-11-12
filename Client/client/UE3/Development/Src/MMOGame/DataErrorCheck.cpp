#include "stdafx.h"
#include "DataErrorCheck.h"
#include "ReplayManager.h"

#if !SHIPPING_PC_GAME

typedef std::map<FString, FString, ComparFString, STLAllocator< std::pair<FString, FString >, UE3STLAllocatorPolicy> > TabfileInChargeNameMap;
typedef std::map<FString, TArray<FString>, ComparFString, STLAllocator< std::pair<FString, TArray<FString> >, UE3STLAllocatorPolicy> > ErrorMsgMap;

TabfileInChargeNameMap g_inchargeNameMap;
ErrorMsgMap g_errorMsgMap;
bool g_enableLogToConsole = false;

const UINT C_MaxLogCount = 10000;
UINT g_currentLogCount = 0;

bool InitDataErrorReport()
{
#define M_InitCharge(a, b) g_inchargeNameMap.insert(std::make_pair(a, b));

	M_InitCharge(TEXT("affix.txt"), TEXT("¸ß»ªÏè"));
	M_InitCharge(TEXT("missile.txt"), TEXT("Â½³ÂÀÚ"));
	M_InitCharge(TEXT("aura.txt"), TEXT("Â½³ÂÀÚ"));
	M_InitCharge(TEXT("attribute.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("item.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("item_armor.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("drop_group.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("doodad.txt"), TEXT("ã¢Óñ"));
	M_InitCharge(TEXT("doodad_drop_data.txt"), TEXT("ã¢Óñ"));
	M_InitCharge(TEXT("npc.txt"), TEXT("ã¢Óñ"));
	M_InitCharge(TEXT("quest.txt"), TEXT("ã¢Óñ"));
	M_InitCharge(TEXT("circle_quest.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("quest_reward.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("quest_story.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("spell_var.txt"), TEXT("Â½³ÂÀÚ"));
	M_InitCharge(TEXT("spell_progress.txt"), TEXT("Â½³ÂÀÚ"));
	M_InitCharge(TEXT("spell.txt"), TEXT("Â½³ÂÀÚ"));
	M_InitCharge(TEXT("specialSkill.txt"), TEXT("Â½³ÂÀÚ"));
	M_InitCharge(TEXT("transform.txt"), TEXT("Â½³ÂÀÚ"));
	M_InitCharge(TEXT("refine.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("set.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("soul.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("weapon_rune_id.txt"), TEXT("Â½³ÂÀÚ"));
	M_InitCharge(TEXT("level.txt"), TEXT("ºú¿¡½Ü"));
	M_InitCharge(TEXT("attributes1.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("attributes2.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("role_relation.txt"), TEXT("ÕÅ¾´Ðù"));
	M_InitCharge(TEXT("title.txt"), TEXT("ÕÅ¾´Ðù"));
	M_InitCharge(TEXT("TweenData.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("mount.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("hint.txt"), TEXT("ã¢Óñ"));
	M_InitCharge(TEXT("pklist.txt"), TEXT("ÕÅÁÖ½¨"));
	M_InitCharge(TEXT("FreeTips.txt"), TEXT("ÕÅÁÖ½¨"));
	M_InitCharge(TEXT("achievement.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("achievementStat.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("guild_building.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("guild_welfare.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("guild_dungeon.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("guide.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("Shop.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("spirit_slot.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("spirit.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("air.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("blood.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("Activity.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("activity_reward.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("AuctionCategory.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("SkeletalMesh"), TEXT("ÐÜ¾ü"));
	M_InitCharge(TEXT("reward_quest_money_cost.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("gift_detail.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("help.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("journey.txt"), TEXT("Ò¶ÓñÙ»"));
	M_InitCharge(TEXT("CensoredWordList.txt"), TEXT("³Â¿­"));
	M_InitCharge(TEXT("gamble.txt"), TEXT("³Â¿­"));

	return true;
}

FString g_dataLoadingContex;

void BeginLoadData(const FString& loadingContex)
{
	g_dataLoadingContex = loadingContex;
}

void AddDataError( const FString& tabFile, const FString& msg )
{
	static bool g_InitDataErrorReport = InitDataErrorReport();

	FString msgStr = FString(TEXT("Loading ")) + g_dataLoadingContex + TEXT(" found -> ") + msg;
	if(g_enableLogToConsole)
	{
		warnf(NAME_Error, *msgStr);
	}

	if(g_currentLogCount >= C_MaxLogCount)
	{
		return;
	}

	++g_currentLogCount;

	ErrorMsgMap::iterator i = g_errorMsgMap.find(g_dataLoadingContex);
	if(i == g_errorMsgMap.end())
	{
		i = g_errorMsgMap.insert(std::make_pair(g_dataLoadingContex, TArray<FString>())).first;
	}

	i->second.AddItem(msgStr);
}

const FString& FindTabfileInChargeName(const FString& tabfile)
{
	TabfileInChargeNameMap::const_iterator i = g_inchargeNameMap.find(tabfile);
	if(i != g_inchargeNameMap.end())
	{
		return i->second;
	}

	warnf(NAME_Error, TEXT("No owner found for table: %s. Please add a table owner in the code as soon as possible!"), *tabfile);

	static FString blankStr = TEXT("Not found");
	return blankStr;
}

void ReportAllDataError()
{
	FString msgAll;
	ErrorMsgMap::iterator i = g_errorMsgMap.begin();
	for (; i != g_errorMsgMap.end(); ++i)
	{
		if(i->second.Num() > 0)
		{
			const FString& chargeName = FindTabfileInChargeName(i->first);
			warnf(NAME_Heading, TEXT("%s [%d] errors"), *i->first, i->second.Num());
			for (INT j = 0; j < i->second.Num(); ++j)
			{
				warnf(NAME_Error, *i->second(j));
			}

			msgAll += FString::Printf(TEXT("%6s : %-30s  %d errors\r\n"),
				*chargeName, *i->first, i->second.Num());
		}
	}

#if !SHIPPING_PC_GAME
	if(!msgAll.IsEmpty() && !GIsBenchmarking && !GIsEditor
#if M_ENABLE_RPMGR
		&& !g_replayMode 
#endif
		)
	{
		FString msg = FString(TEXT("Config data errors found. Please check 08.Prog\\Client\\UE3\\MMOGame\\Logs\\Launch.log to fix them:\r\n")) + msgAll;
		appMsgf(AMT_OK, *msg);
	}
#endif

	g_errorMsgMap.clear();
	g_currentLogCount = 0;
	//g_inchargeNameMap.clear();
}

void EnableErrorToConsole( bool on )
{
	g_enableLogToConsole = on;
}

#endif
