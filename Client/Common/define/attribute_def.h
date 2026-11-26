#ifndef _ATTRIBUTE_DEF_H_
#define _ATTRIBUTE_DEF_H_

///@file attribute_def.h
/// lua package file attribute_def.pkg

// tolua_begin
#define RATE_MAX			(10000LL)			///< @line
#define MAX_ATTR_NAME_LEN	(128)				///< @line


/// Warning: catPhyAtkHi, catPhyAtkLo, catMagAtkHi, catMagAtkLo 这四个属性是放大1w倍存储的，但并不用Rate结尾。

///@enum CHAR_ATTR_TYPE
enum CHAR_ATTR_TYPE
{
	catInvalid = 0,								///< @line

	catDefault,									///< @line
	//	catBeginMainAttr,
	//	catValue = catBeginMainAttr,
	//	catCombatParam,
	//	catEndMainAttr,
	catBeginExAttr,

	// ====================

	catPhyDef = catBeginExAttr,    // 2	 物理防御
	catMaxPhyDef,				   // 3	 最大物理防御
	catMagDef,					   // 4	 魔法防御
	catMaxMagDef,				   // 5	 最大魔法防御
	catPhyAtk,					   // 6	 物理攻击
	catMaxPhyAtk,				   // 7	 最大物理攻击
	catMagAtk,					   // 8	 魔法攻击
	catMaxMagAtk,				   // 9	 最大魔法攻击
	catTaoAtk,					   // 10 道术攻击
	catMaxTaoAtk,				   // 11 最大道术攻击
	catStabAtk,				// 12 Minimum stabbing attack
	catMaxStabAtk,				// 13 Maximum stabbing attack
	catArcheryAtk,				// 14 Minimum archery attack
	catMaxArcheryAtk,				// 15 Maximum archery attack
	catMaxHp,					   // 16 最大HP
	catMaxMp,					   // 17 最大MP
	catWalkSpeed,				   // 18 行走速度
	catRunSpeed,				   // 19 跑步速度

	catAccurate,				   // 20 准确/命中
	catAgility,					   // 21 敏捷
	catMagDodge,				   // 22 魔法闪避
	catCrit,					   // 23 暴击率
	catCritDmg,					   // 24 暴击伤害
	catCritResist,				   // 25 暴击抵抗
	catCritDmgResist,			   // 26 爆伤抵抗
	catAntiPhyDef,				   // 27 物理穿透
	catAntiMagDef,				   // 28 魔法穿透
	catDrugHpRecovery,			   // 29 药剂HP恢复
	catDrugMpRecovery,			   // 30 药剂MP恢复
	catLuck,					   // 31 幸运
	catHolyDmg,					   // 32 神圣伤害

	catHurtEnhanced,			   // 33 受击增伤 (受到攻击时下次伤害提升)
	catDmgResist,				   // 34 伤害抵抗
	catDmgEnhance,				   // 35 伤害强化
	catAtkSpeed,				   // 36 攻击速度

	catHpRecovery,				   // 37 HP恢复
	catMpRecovery,				   // 38 MP恢复
	catHpRecoveryTap,			   // 39 HP恢复系数
	catMpRecoveryTap,			   // 40 MP恢复系数
	catTapIncrease,				   // 41 系数增加?
	catAtkSpeedRate,			   // 42 攻击速度加成率

	catPoisonDef,				   // 43 毒药防御
	catPoisonRecovery,			   // 44 毒药恢复

	catPoisonResist,			   // 45 毒药抵抗
	catStunResist,				   // 46 击晕抵抗
	catRepelResist,				   // 47 击退抵抗
	catHurtResist,				   // 48 受击硬直抵抗
	catBurnResist,				   // 49 燃烧抵抗

	catWeaponWeightLmt,			   // 50 武器负重限制
	catArmorWeightLmt,			   // 51 防具负重限制
	catBagWeightLmt,			   // 52 背包负重限制

	catEndExAttr,				   // 53

	// ====================

	catDmgAbsorb = catEndExAttr,   // 53 伤害吸收
	catExtraDmg,				   // 54 额外固定伤害 - 有使用
	catExtraHardTime,			   // 55 硬直时间延长 - 有使用
	catPHitCountDown,			   // 56 物理连击间隔 - 有使用
	catPHitNum,					   // 57 物理连击次数 - 有使用
	catSpellPtsIncr,			   // 58 技能点数增益 - 有使用
	catLeech,					   // 59 吸血系数 - 有使用
	catMpToHp,					   // 60 魔转血 (MP转HP) - 有使用
	catMpToHpRemain,			   // 61 剩余法力转生命 - 有使用
	catExtraThunderDmg,			   // 62 额外雷电伤害
	catDrugHpSlowRecovery,		   // 63 药剂缓效恢复 - 有使用
	catExpRate,					   // 64 经验获取率

	catTotal,					///< @line
};

///@enum SPELL_ATTR_TYPE
enum SPELL_ATTR_TYPE
{
	satInvalid = -1,							///< @line
	satBegin,

	satClass = satBegin,						///< @line
	satRequireLevel,							///< @line

	satNextSpell,								///< @line
	satSwitchSpell,								///< @line
	satResetTime,								///< @line
	satCacheTime,								///< @line

	satWeaponTypeR,								///< @line
	satWeaponTypeL,								///< @line  没调用过

	satCanEquip,								///< @line
	satCastType,								///< @line
	satJudgeType,								///< @line
	satAttackType,								///< @line
	satElementaryType,							///< @line
	satQteType,									///< @line

	satLockType,								///< @line
	satMinDistance,								///< @line
	satMaxDistance,								///< @line
	satMinGrids,								///< @line
	satMaxGrids,								///< @line
	satTolerantDistance,						///< @line  没调用过
	satTolerantGrids,							///< @line
	satRangeType,								///< @line
	satRangeParam1,								///< @line
	satRangeParam2,								///< @line
	satRangeParam3,								///< @line
	satIgnoreObstacle,							///< @line
	satUseWeaponRange,							///< @line
	satIgnoreTargetRadius,						///< @line
	satAffectedByAtkSpeed,						///< @line
	satUseDPH,									///< @line  没调用过
	satIgnoreDefense,							///< @line
	satIgnoreClientTarget,						///< @line

	satMaxTargetCount,							///< @line
	satOffsetDirection,							///< @line
	satOffsetDistance,							///< @line

	satHOffsetGrids,							///< @line
	satVOffsetGrids,							///< @line

	satTargetObjType,							///< @line
	satTargetRelationship,						///< @line
	satTargetSpecies,							///< @line  没调用过
	satTargetSex,								///< @line  没调用过

	satTargetDirection,							///< @line  没调用过

	satTargetStance,							///< @line
	satSelfStance,								///< @line  没调用过

	satCostType,								///< @line  没调用过
	satInitialCostValue,						///< @line
	satInitialCostValuePct,						///< @line
	satDurativeCostValue,						///< @line
	satDurativeCostValuePct,					///< @line
	satModifierCostValue,						///< @line
	satModifierCostValuePct,					///< @line

	satItemCostID,								///< @line
	satItemCostCount,							///< @line
	satConsumeItem,								///< @line  没调用过

	satItemCostType,							///< @line
	satItemCostSubType,							///< @line

	satDuraCost,								///< @line

	satCoolDownTriggerType,						///< @line
	satSelfCoolDown,							///< @line
	satEventCoolDown,							///< @line
	satAdjustDirection,							///< @line
	satAffectedByWeaponSpeed,					///< @line
	satPrepareType,								///< @line
	satPrepareTimeMin,							///< @line
	satPrepareTimeMax,							///< @line
	satAccumulateTimeMax,						///< @line
	satAccumulateSeg,							///< @line
	satChannelCount,							///< @line
	satMinEffectCount,							///< @line
	satInterval,								///< @line
	satHardTime,								///< @line
	satWalkDelayTime,							///< @line
	satRunDelayTime,							///< @line

	satHitLevel,								///< @line  没调用过
	satMoveBreak,								///< @line
	satBreakMove,								///< @line
	satCanMoveInPrepare,						///< @line
	satCanMoveInHardTime,						///< @line
	satHitBreak,								///< @line
	satHardTimeAffectedByCharLevel,				///< @line

	satCancelType,								///< @line
	satCancelLevel,								///< @line
	satCancelHardTime,							///< @line

	satHitTag,									///< @line
	satSpeed,									///< @line

	satIsMixEffect,								///< @line  没调用过
	satMixEffectPhyRate,						///< @line  没调用过
	satMixEffectMagRate,						///< @line  没调用过

	satEffectPct,								///< @line

	satEffectValue,								///< @line
	satEffectValueMax,							///< @line
	satDefaultValue,							///< @line
	satDefaultValueMax,							///< @line
	satAffectedByNpcParam,

	satThreatPct,								///< @line
	satThreatValue,								///< @line
	satThreatMax,								///< @line
	satCanCastOnMount,							///< @line
	satCasterDeadLimit,							///< @line
	satTargetDeadLimit,							///< @line
	satCasterLimitBits,							///< @line
	satCasterCCLimitBits,						///< @line
	satSceneTypeLimit,							///< @line  没调用过

	satCasterCheckNode,							///< @line
	satTargetCheckNode,							///< @line
	satOnLearn,									///< @line
	satOnForget,								///< @line
	satOnEquip,									///< @line
	satOnUnEquip,								///< @line
	satOnPrepare,								///< @line
	satOnCast,									///< @line
	satOnBreak,									///< @line
	satOnCreateBullet,							///< @line
	satBeforeRoll,								///< @line
	satAfterRoll,								///< @line
	satBeforeEffect,							///< @line
	satAfterEffect,								///< @line
	satNeedCache,								///< @line

	satCancelStiff,								///< @line
	satKnockBackRate,							///< @line
	satBaseStiffTime,							///< @line
	satMonsterChaseDelayTime,					///< @line
	satChainEffect,								///< @line
	satBindSlot,								///< @line
	satTrigEvent,								///< @line

	satTotal									///< @line
};

///@enum AURA_ATTR_TYPE
enum AURA_ATTR_TYPE
{
	aatInvalid = -1,							///< @line

	aatCategory,								///< @line
	aatType,									///< @line
	aatGroup,									///< @line
	aatDuration,								///< @line
	aatDelay,									///< @line
	aatInterval,								///< @line
	aatSameIdStackType,							///< @line  没调用过
	aatDiffIdStackType,							///< @line  没调用过

	aatStackTypeSameIdSameCaster,				///< @line
	aatStackSameTypeSameCaster,					///< @line
	aatStackSameGroupSameCaster,				///< @line  没调用过
	aatStackTypeSameIdDiffCaster,				///< @line
	aatStackSameTypeDiffCaster,					///< @line
	aatStackSameGroupDiffCaster,				///< @line  没调用过


	aatTimeCover,								///< @line
	aatStackResetTime,							///< @line
	aatInitStackCount,							///< @line
	aatMaxStackCount,							///< @line
//	aatStackEfficient,							///< @line
//	aatCoexist,									///< @line
	aatStrengthLevel,							///< @line

	aatCanDispel,								///< @line
	aatCanCancel,								///< @line
	aatDispelCount,								///< @line  没调用过
	aatTrigOnRemoveWhenDispel,					///< @line
	aatTrigOnRemoveWhenTimeup,					///< @line

	aatAttackType,								///< @line
	aatJudgeType,								///< @line  没调用过
	aatElementaryType,							///< @line  没调用过

	aatEffectPctX,								///< @line
	aatEffectValueX,							///< @line
	aatEffectPct,								///< @line
	aatEffectValue,								///< @line
	aatThreatPct,								///< @line
	aatThreatValue,								///< @line
	aatThreatMax,								///< @line
	aatRemoveWhenDead,							///< @line
	aatSaveToDB,								///< @line
	aatOfflineTimeValid,						///< @line
	aatHiddenToClient,							///< @line
	aatHiddenToSelf,							///< @line
	aatControlDecay,							///< @line
	aatEffectTimeMin,							///< @line
	aatEternal,									///< @line
	aatIgnoreDefense,							///< @line
	aatIgnoreResistence,						///< @line  没调用过
	aatAffectedCharAttr,						///< @line  没调用过
	aatDisplayGroup,							///< @line
	aatDisplayValue,							///< @line
	aatResistType,								///< @line
	aatEffectDecay,								///< @line

	aatOnAppend,								///< @line
	aatBeforeStack,								///< @line  没调用过
	aatAfterStack,								///< @line
	aatOnTimeup,								///< @line
	aatOnDispel,								///< @line
	aatOnRemove,								///< @line

	aatTotal									///< @line
};

///@enum MISSILE_ATTR_TYPE
enum MISSILE_ATTR_TYPE
{
	matInvalid = -1,							///< @line

	matDefault,									///< @line
	matLockType,								///< @line  没调用过
	matMoveType,								///< @line
	matMoveSpeed,								///< @line
	matHeight,									///< @line
	matLifeTime,								///< @line
	matMaxCollisionCount,						///< @line
	matMaxTargetCountPerCollision,				///< @line
	matRangeType,								///< @line
	matRangeParam1,								///< @line
	matRangeParam2,								///< @line
	matRangeParam3,								///< @line
	matBlockShape,								///< @line  没调用过
	matBlockParam,								///< @line  没调用过
	matOffsetDirection,							///< @line
	matOffsetDistance,							///< @line
	matHOffsetGrids,							///< @line
	matVOffsetGrids,							///< @line
	matDelay,									///< @line
	matInterval,								///< @line
	matNotMultiEffect,							///< @line
	matNotHitLastTarget,						///< @line
	matBlockType,								///< @line  没调用过
	matOnInit,									///< @line
	matOnCreate,								///< @line
	matOnScan,									///< @line
	matOnTimeup,								///< @line
	matOnRemove,								///< @line
	matOnCharEnter,								///< @line
	matOnCharLeave,								///< @line

	matTotal									///< @line
};

// tolua_end

inline INT32 MakeAttrKey(INT32 nKey, INT32 nClass)
{
	return (nKey << 16) + nClass;
}

#endif // _attribute_def_h__
