#ifndef _CHAR_DEF_H_
#define _CHAR_DEF_H_

/// @file char_def.h
/// lua package file char_def.pkg

// tolua_begin

/// @enum CHAR_MOVE_STATE
enum CHAR_MOVE_STATE
{
	CMS_INVALID = 0,

	CMS_ACTIVE_MOVE,
	CMS_STAND = CMS_ACTIVE_MOVE,        ///< @line
	CMS_SIT,                            ///< @line
	CMS_WALK,							///< @line
	CMS_RUN,							///< @line
	CMS_MOVE,							///< @line
	CMS_MINE,							///< @line

	CMS_PASSIVE_MOVE,
	CMS_DASH,                           ///< @line
	CMS_KNOCK_BACK,                     ///< @line
	CMS_THROWN,							///< @line
	CMS_FREEZE,                         ///< @line
	CMS_FEAR,                           ///< @line
	CMS_DIE,                            ///< @line
	CMS_CLIENT_SHOW_TIME,               ///< @line
	CMS_STALL,                          ///< @line

	CMS_TOTAL
};

// 技能状态
enum CHAR_SPELL_STATE
{
	cspsInvalid = -1,

	cspsIdle,
	cspsCastingBegin,
	cspsChanneling = cspsCastingBegin,
	cspsPreparing,
	cspsPreAccumulating,
	cspsAccumulating,
	cspsBreaking,
	cspsOpening,
	cspsCastingEnd,
	cspsHard = cspsCastingEnd,

	cspsTotal
};

enum CHAR_SEX_TYPE
{
	cstInvalid = 0,

	cstMale,
	cstFemale,
	cstAsexual,

	cstTotal
};

enum CHAR_CLASS_TYPE
{
	clsInvalid = -1,

	clsPlayerBegin,

	clsWarrior = clsPlayerBegin,
	clsMage,
	clsRogue,
	clsArcher,
	clsTaoist,
	clsSpear,		// 1.05 Dragon Lancer (class id 5)
	clsSorcerer,	// 1.05 Demon / Sorcerer (class id 6)

	clsPlayerEnd,

	clsPlayerTotal = clsPlayerEnd,

	clsNpc = clsPlayerEnd,

	clsTotal
};

enum CHAR_SPECIES_TYPE
{
	spcInvalid = -1,

	spcNone,	   // 无类型
	spcPlayer,	   // 玩家
	spcHuman,	   // 人类
	spcHumanlike,  // 类人生物
	spcBeast,	   // 野兽
	spcInsect,	   // 昆虫
	spcPlant,	   // 植物
	spcUndead,	   // 亡灵
	spcMachine,	   // 机械
	spcDragon,	   // 龙族
	spcDemon,	   // 恶魔
	spcElements,   // 元素生物
	spcBuilding,   // 建筑

	spcTotal,
};

enum CHAR_CC_FLAG
{
	cclInvalid = -1,

	cclBegin,
	cclStun = cclBegin,			// 0
	cclFear,					// 1
	cclFury,                    // 2
	cclDazzle,					// 3
	cclSlowed,					// 4
	cclKnockBack,				// 5
	cclFreeze,					// 6
	cclSeduced,					// 7
	cclInvincible,				// 8
	cclLeech,
	cclTransportationRing,
	cclProbeNecklace,
	cclMemoSuit,
	cclInvisible,
	cclTotal,
};

enum CHAR_FLAG
{
	cfInvalid = -1,
	cfNotInCombat,
	cfInCombat,
	cfCounter,
	//	cfInvisible,
	cfOnMount,
	cfCannotBeThrown,
	cfTempUndead,
	cfImmunePhyAtk,
	cfImmuleMagAtk,
	cfCanNotBeSelectedAsTarget,
	cfBTDenyDie,
	cfBTHasFlee,
	cfAuraInvalid,
	cfCombatPoseOn,
	cfImmunePassiveMove,
	cfFury,
	cfDamageStatistics,
	cfNeverBeHit,

	cfTotal

};

enum CHAR_STANCE_FLAG
{
	csfInvalid = -1,

	csfStand,
	csfUp,
	csfDown,

	csfTotal,
};

enum CHAR_STIFF_BITS
{
	cstfBegin,
	cstfFreeze = cstfBegin,
	cstfKnockBack,
	cstfSilent,

	cstfTotal,
};

enum CHAR_DIE_REASON
{
	cdrInvalid = -1,

	cdrDefault,
	cdrSpell,
	cdrAura,
	cdrRecovery,
	cdrTimeToDie,
	cdrAIBehavior,
	cdrSilent,
	cdrLeech,
	cdrSuicide,

	cdrCount
};

enum CHAR_REVIVE_TYPE
{
	crtInvalid = 0,

	crtAtCurrentPos,
	crtInstant,
	crtFree,

	crtTotal,
};

// 玩家 => login, inrange
// Npc => inrange, spawn, revive, NoPerforSpawn
enum SCENE_OBJECT_APPEAR_TYPE
{
	soatInvalid,

	soatLogin,
	soatInRange,
	soatSpawn,
	soatRevive,
	soatNoPerforSpawn,

	soatTotal
};

enum SCENE_OBJECT_DISAPPEAR_TYPE
{
	sodtInvalid,

	sodtLeaveScene,
	sodtOutOfRange,

	sodtTotal
};

/// @enum ENUM_COMBAT_STAT_ENTRY
enum ENUM_COMBAT_STAT_ENTRY
{
	escsInvalid = -1,

	escsDeathCount,                         ///< @line
	escsRoleKillCount,                      ///< @line
	escsNpcKillCount,                       ///< @line
	escsEliteNpcKillCount,                  ///< @line
	escsBossKillCount,                      ///< @line
	escsDoodadKillCount,                    ///< @line

	escsTotalHeal,                          ///< @line
	escsTotalDamage,                        ///< @line

	escsPhyAtkCritCount,                  ///< @line
	escsMagAtkCritCount,                  ///< @line

	escsScore1,                             ///< @line
	escsScore2,                             ///< @line
	escsTotalScore,                         ///< @line

	escsTotal
};


// ========== 新增代码, 自己新增加 枚举

// 铭文洗练节点
enum ROLE_RUNE_ROOT
{
	rrrNormal,
	rrrAdvanced,
};


// tolua_end

#endif  // _CHAR_DEF_H_
