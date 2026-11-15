#ifndef _RANKLIST_DEF_
#define _RANKLIST_DEF_

// tolua_begin

#define MAX_RANK_LIST_NODE_COUNT 5000
#define	MAX_RANK_NODE_NUM_PER_PAGE 10

enum RANK_LIST
{
	rlInvalid = -1,
    rlBegin,

    // 个人排行榜
    rlRoleBegin = rlBegin,

	rlLevel = rlRoleBegin,			// 等级排行榜
	rlLevelWarrior,
	rlLevelMage,
	rlLevelTaoist,
	rlLevelRogue,
	rlLevelArcher,	

	rlPower,					    // 战斗力排行榜
	rlPowerWarrior,
	rlPowerMage,
	rlPowerTaoist,
	rlPowerRogue,
	rlPowerArcher,	

	rlMoney,					    // 财富排行榜
	rlAchievement,				    // 成就点数排行榜
	rlMaster,					    // 名师值排行榜
	rlPkValue,					    // pk值排行榜
	rlTotalKillCount,			    // 总杀人数

	rlActivity01,
	rlActivity02,
	rlActivity03,
	rlActivity04,
	rlActivity05,
	rlActivity06,
	rlActivity07,
	rlActivity08,

	rlLevelSpear,		// 1.05 class level ranking
	rlLevelSorcerer,		// 1.05 class level ranking
	rlPowerSpear,		// 1.05 class combat power ranking
	rlPowerSorcerer,		// 1.05 class combat power ranking
	rlReserved05,
	rlReserved06,
	rlReserved07,
	rlReserved08,

	rlArena1v1,
	rlArena1v1Warrior,
	rlArena1v1Mage,
	rlArena1v1Taoist,
	rlArena1v1Reserved01,
	rlArena1v1Reserved02,

	rlArena5v5,
	rlArena5v5Reserved01,
	rlArena5v5Reserved02,
	rlArena5v5Reserved03,
	rlArena5v5Reserved04,
	rlArena5v5Reserved05,

    rlRoleEnd,

	// 家族排行榜
    rlGuildBegin = rlRoleEnd,

    rlGuild_Level = rlGuildBegin,   // 家族等级排行榜
	rlGuild_Money,				    // 家族金钱排行榜
	rlGuild_Resource_Herb,		    // 家族资源01
	rlGuild_Resource_Wood,		    // 家族资源02
	rlGuild_Resource_Crystal,	    // 家族资源03
	rlGuild_Resource_Silver,	    // 家族资源04
	rlGuild_Pow,					// 家族总战斗力
    rlGuild_War,                    // 公会战积分

    rlGuildEnd,

	// 竞技场排行榜
 //   rlArenaBegin = rlGuildEnd,

 //   rlArenaScore = rlArenaBegin,    //竞技场5v5积分
	//rlRoleArenaScore,			    // 竞技场1v1积分排行榜

 //   rlArenaEnd,

	rlTotal,
};

// tolua_end

#define MAKE_RANKLIST_EXP_KEY(_level_, _exp_) ((_level_<< 32) | _exp_)
#define GET_RANKLIST_EXP_FROM_KEY(_key_) (_key_ & 0xFFFFFFFF)

#endif //_RANKLIST_DEF_
