#ifndef _ITEM_DEF_H_
#define _ITEM_DEF_H_

/// @file item_def.h 
/// @brief item related definition

// tolua_begin   

#define MAX_ITEM_COUNT		(128)
#define MAX_DROP_COUNT		(128)
#define MAX_REFINE_LEVEL    (300)

///@enum EItemLog
enum EItemLog
{
    ITEMLOG_INVALID = -1,					///< invalid
    ITEMLOG_SHOP,							///< 商人购物
    ITEMLOG_LOOT,							///< 拾取
    ITEMLOG_GM_COMMAND,						///< GM指令
    ITEMLOG_QUEST_PREPAY,					///< 接受任务给予
    ITEMLOG_QUEST_REWARD,					///< 完成任务奖励
    ITEMLOG_DUNGEON_REWARD,					///< 副本奖励
    ITEMLOG_PLAYER_STALL,					///< 玩家摆摊
    ITEMLOG_PLAYER_TRADE,					///< 玩家交易
    ITEMLOG_BUY_BACK,						///< 回购
    ITEMLOG_CREATE_ROLE,					///< 创建角色
    ITEMLOG_CREATE_HERO,					///< 创建英雄
    ITEMLOG_SCRIPT,							///< 脚本
    ITEMLOG_GET_MAIL_ATTACH,				///< 邮件附件
    ITEMLOG_GUILD_STORAGE,					///< 公会仓库进出
    ITEMLOG_GUILD_DESTORY,                  ///< 公会解散
    ITEMLOG_ACHIEVEMENT_REWARD,				///< 成就奖励
    ITEMLOG_UPGRADE_SOUL,					///< 灵石升级
    ITEMLOG_REMOVE_SOUL,					///< 灵石拆除
    ITEMLOG_NPC_DROP,						///< 怪物掉落
    ITEMLOG_USER_REQUEST,					///< 玩家丢弃
    ITEMLOG_SELL,							///< 出售给商人
    ITEMLOG_CAST_SPELL,						///< 释放技能
    ITEMLOG_REFINE_ITEM,					///< 体格强化
    ITEMLOG_COMPLETE_QUEST,					///< 完成任务上交
    ITEMLOG_CREATE_GUILD,					///< 创建公会
    ITEMLOG_USE_ITEM,						///< 使用物品
    ITEMLOG_INSERT_SOUL,					///< 灵石融合
    ITEMLOG_AUCTION_SELL,					///< 拍卖行出售
    ITEMLOG_RUNE,							///< 铭文
    ITEMLOG_DECOMPOSE,						///< 物品分解
    ITEMLOG_COMPOSE,						///< 物品合成
    ITEMLOG_ACTIVITY_REWARD,				///< 活动奖励
    ITEMLOG_REVIVE,							///< 复活
    ITEMLOG_GM,								///< GM工具
    ITEMLOG_OPEN_DOODAD,					///< 打开doodad
    ITEMLOG_REAL_MONEY_SHOP,				///< 商城
    ITEMLOG_TEAM_AUCTION,					///< 组队拍卖
    ITEMLOG_TEAM_AUCTION_FAILED,			///< 组队拍卖失败
    ITEMLOG_QUEST_RECYCLE,					///< 环任务
    ITEMLOG_EXCHANGE,						///< 交易行
    ITEMLOG_ARENA_COMMON_REWARD,			///< 竞技场奖励
    ITEMLOG_SABAC_AWARD,					///< 沙巴克奖励
    ITEMLOG_REPUTATION_AWARD,				///< 声望奖励
    ITEMLOG_SPLIT,							///< 物品拆分
    ITEMLOG_MOVE,							///< 物品堆叠
    ITEMLOG_ORGANIZE_BAG,					///< 物品整理
    ITEMLOG_EQUIP_LEVELUP,					///< 装备升级
    ITEMLOG_EQUIP_LEVELUP_FAILED,			///< 装备升级失败
    ITEMLOG_EQUIP_LEVELUP_SUCCEEDED,		///< 装备升级成功
    ITEMLOG_RECOVER_USED_UP,				///< 恢复类道具用尽
    ITEMLOG_MINE,							///< 挖矿
    ITEMLOG_ENROLL_SABAC,					///< 沙巴克报名
    ITEMLOG_DEATH,							///< 死亡
    ITEMLOG_SPELL,							///< 技能效果
    ITEMLOG_ATTACK,							///< 攻击
    ITEMLOG_ATTACKED,						///< 被攻击
    ITEMLOG_TRUMPET,						///< 喇叭
    ITEMLOG_CHECKIN_AWARD,					///< 签到奖励
    ITEMLOG_AWARD,							///< 平台奖励
    ITEMLOG_BLACK_MARKET,					///< 黑市
    ITEMLOG_MATERIAL,						///< 矿石洗练
    ITEMLOG_GAMBLE,							///< 装备重铸
    ITEMLOG_PACKAGE,						///< 礼包
    ITEMLOG_IDENTIFY,						///< 物品鉴定
    ITEMLOG_RANDOMIZE,                      ///< 物品洗练
    ITEMLOG_LUCK_OIL,                       ///< 祝福油
    ITEMLOG_SIGN,                           ///< 装备签名
    ITEMLOG_STORAGE_ROLLBACK,               ///< 公会仓库物品操作失败，回滚
    ITEMLOG_SPELL_BOOK_GAMBLE,              ///< 技能书重铸
    ITEMLOG_GM_FIX_ROLE,				    ///< GM工具修正角色数据
    ITEMLOG_RUNE_ADVANCE_CONFIRM,           ///< 高级铭文洗练确认
    ITEMLOG_POTION_GAMBLE,                  ///< 炼药
    ITEMLOG_SELL_RESOURCE_ITEM,             ///< 资源包全部卖出

    ITEMLOG_EQUIP_ADD_SOCKET,               ///< 装备打孔
    ITEMLOG_EQUIP_SET_COLOR,                ///< 装备雕色
    ITEMLOG_CHANGE_EUIQP_SOCKET_COLOR,      ///< 装备孔位雕色
    ITEMLOG_ADD_DESTINY_WEAPON,             ///< 开启命运武器礼盒, 获得命运武器

    ITEMLOG_TOTAL,							///< total
};

///@enum EMoneyLog
enum EMoneyLog
{
    MONEYLOG_INVALID = -1,					///< invalid
    MONEYLOG_BUY_ITEM,						///< 商人购物
    MONEYLOG_REPAIR,						///< 装备修理
    MONEYLOG_PLAYER_STALL,					///< 玩家摆摊
    MONEYLOG_PLAYER_STALL_TAX,				///< 玩家摆摊税收
    MONEYLOG_PLAYER_TRADE,					///< 玩家交易
    MONEYLOG_PLAYER_TRADE_TAX,				///< 玩家交易税收
    MONEYLOG_BUY_BACK,						///< 回购
    MONEYLOG_REFINE_ITEM,					///< 体格强化
    MONEYLOG_BUY_SLOTS,						///< 背包或银行格子扩充
    MONEYLOG_REMOVE_SOUL,					///< 灵石拆除
    MONEYLOG_CREATE_GUILD,					///< 创建公会
    MONEYLOG_GUILD_CONTRIBUTE,				///< 公会贡献
    MONEYLOG_GUILD_POP_ITEM,				///< 公会仓库使用
    MOMEYLOG_GUILD_ENTER_DUNGEON,			///< 进入公会副本
    MONEYLOG_TELEPORT_PLATFORM,				///< 传送
    MONEYLOG_DUNGEON_SELECT_REWARD,			///< 副本奖励
    MONEYLOG_CAST_SPELL,					///< 释放技能
    MONEYLOG_MAIL_COST,						///< 邮资
    MONEYLOG_LOOT,							///< 拾取
    MONEYLOG_SELL_ITEM,						///< 出售给商人
    MONEYLOG_QUEST_PREPAY,					///< 接受任务给予
    MONEYLOG_QUEST_REWARD,					///< 完成任务奖励
    MONEYLOG_QUEST_COST,					///< 接受任务扣除
    MONEYLOG_GUILD_SALARY,					///< 公会薪资
    MONEYLOG_DUNGEON_REWARD,				///< 副本奖励
    MONEYLOG_GM_COMMAND,					///< GM指令
    MONEYLOG_SCRIPT,						///< 脚本
    MONEYLOG_IDENTIFY_ITEM,					///< 物品鉴定
    MONEYLOG_RANDOMIZE_ITEM,				///< 物品洗练
    MONEYLOG_QUEST_FLY_COST,				///< 传送至任务区域
    MONEYLOG_SIGN_ITEM_COST,				///< 装备签名
    MONEYLOG_AUCTION_SELL_COST,				///< 拍卖行出售
    MONEYLOG_AUCTION_DEPOSIT_COST,			///< 拍卖行定金
    MONEYLOG_AUCTION_LEFT_MONEY,			///< 拍卖行尾款
    MONEYLOG_CHANGE_WEATHER,				///< 改变天气
    MONEYLOG_BUY_VIP,						///< VIP购买
    MONEYLOG_DAILY_QUEST_BUY_COMPLETE_TIME,	///< 购买日常任务完成次数
    MONEYLOG_DAILY_QUEST_BUY_ONESHOT,		///< 刷新日常任务
    MONEYLOG_OPEN_GUILD_DUNGEON,			///< 打开公会副本
    MONEYLOG_GUILD_DECLARE_WAR,				///< 公会宣战
    MONEYLOG_DECOMPOSE,						///< 物品分解
    MONEYLOG_COMPOSE,						///< 物品合成
    MONEYLOG_SPIRIT_EXP2PTS,				///< 练魂
    MONEYLOG_SPIRIT_ADDAIR,					///< 练气
    MONEYLOG_SPIRIT_BUYSLOT,				///< 购买境界格子
    MONEYLOG_SPIRIT_BUYEXCHANGEAIR,			///< 装备气
    MONEYLOG_SPIRIT_BLOOD,					///< 刷血脉等级
    MONEYLOG_REVIVE,						///< 复活
    MONEYLOG_CREATE_ARENA_TEAM,				///< 创建竞技场队伍
    MONEYLOG_GUILD_BUY_AURA,				///< 公会光环购买
    MONEYLOG_GET_MAIL_ATTACH,				///< 邮件附件
    MONEYLOG_GM,							///< GM工具
    MONEYLOG_REAL_MONEY_SHOP,				///< 商城
    MONEYLOG_TEAM_AUCTION_BONUS,			///< 组队拍卖分钱
    MONEYLOG_TEAM_AUCTION_RETURN,			///< 组队拍卖返还钱
    MONEYLOG_TEAM_AUCTION_DEPOSITE,			///< 组队拍卖
    MONEYLOG_EXCHANGE,						///< 交易行
    MONEYLOG_EXCHANGE_FEE,					///< 交易行手续费
    MONEYLOG_MASTER_CZ,						///< 师徒祝贺
    MONEYLOG_ACHIEVEMENT_REWARD,			///< 成就奖励
    MONEYLOG_RUNE,							///< 铭文
    MONEYLOG_ARENA_JOIN_SEASON,				///< 加入竞技场赛季
    MONEYLOG_ARENA_RANK_REWARD,				///< 竞技场排名奖励
    MONEYLOG_ENROLL_SABAC,					///< 沙巴克报名
    MONEYLOG_EQUIP_LEVELUP,					///< 装备升级
    MONEYLOG_REPUTATION_AWARD,				///< 声望奖励
    MONEYLOG_UPGRADE_STALL,					///< 摊位升级
    MONEYLOG_MINE,							///< 采矿
    MONEYLOG_AWARD,							///< 平台奖励
    MONEYLOG_ERECT_STATUS,					///< 塑像
    MONEYLOG_FCM_LOSS,						///< 防沉迷扣除
    MONEYLOG_DAILY_LIMIT_LOSS,				///< 超过货币每日上限扣除
    MONEYLOG_WEEKLY_LIMIT_LOSS,				///< 超过货币每周上限扣除
    MONEYLOG_TOTAL_LIMIT_LOSS,				///< 超过货币总上限扣除
    MONEYLOG_CHAT_COST,						///< 聊天
    MONEYLOG_ESCORT,						///< 押镖
    MONEYLOG_BLACK_MARKET,					///< 黑市
    MONEYLOG_MATERIAL,						///< 矿石洗练
    MONEYLOG_BIRTH,							///< 创建角色
    MONEYLOG_EXP_MORAL,						///< 经验转化道义值
    MONEYLOG_TEACHER_REWARD,				///< 师傅奖励
    MONEYLOG_STUDENT_REWARD,				///< 徒弟奖励
    MONEYLOG_PACKAGE,                       ///< 礼包
    MONEYLOG_EQUIP_LEVELUP_IMMEDIATELY,     ///< 武器升级立即完成
    MONEYLOG_ARENA_REFUND,                  ///< JJC退款
    MONEYLOG_ARENA_REWARD,                  ///< JJC胜负奖励
    MONEYLOG_SELL_RESOURCE_ITEM,            ///< 资源包全部卖出

    // 自己新增加
    MONEYLOG_CUSTOM_ROLE_BUY_TIME,          ///< 自定义购买点卡
    MONEYLOG_EQUIP_LEVELUP_TAKE_OUT_IMMEDIATELY, ///< 升级武器立即取回
    MONEYLOG_GM_CMD_SUB_MONEY_YB,           ///< GM扣除自身元宝
    MONEYLOG_GM_CMD_ADD_ITEM_YBD,           ///< GM获得元宝袋

    MONEYLOG_BT_BEGIN = 10000,
    MONEYLOG_BT_01,							///< 策划自定义类型01
    MONEYLOG_BT_02,							///< 策划自定义类型02
    MONEYLOG_BT_03,							///< 策划自定义类型03
    MONEYLOG_BT_04,							///< 策划自定义类型04
    MONEYLOG_BT_05,							///< 策划自定义类型05
    MONEYLOG_BT_06,							///< 策划自定义类型06
    MONEYLOG_BT_07,							///< 策划自定义类型07
    MONEYLOG_BT_08,							///< 策划自定义类型08
    MONEYLOG_BT_09,							///< 策划自定义类型09
    MONEYLOG_BT_10,							///< 策划自定义类型10
    MONEYLOG_BT_11,							///< 策划自定义类型11
    MONEYLOG_BT_12,							///< 策划自定义类型12
    MONEYLOG_BT_13,							///< 策划自定义类型13
    MONEYLOG_BT_14,							///< 策划自定义类型14
    MONEYLOG_BT_15,							///< 策划自定义类型15
    MONEYLOG_BT_16,							///< 策划自定义类型16
    MONEYLOG_BT_17,							///< 策划自定义类型17
    MONEYLOG_BT_18,							///< 策划自定义类型18
    MONEYLOG_BT_19,							///< 策划自定义类型19
    MONEYLOG_BT_20,							///< 策划自定义类型20
    MONEYLOG_BT_21,							///< 策划自定义类型21
    MONEYLOG_BT_22,							///< 策划自定义类型22
    MONEYLOG_BT_23,							///< 策划自定义类型23
    MONEYLOG_BT_24,							///< 策划自定义类型24
    MONEYLOG_BT_25,							///< 策划自定义类型25
    MONEYLOG_BT_26,							///< 策划自定义类型26
    MONEYLOG_BT_27,							///< 策划自定义类型27
    MONEYLOG_BT_28,							///< 策划自定义类型28
    MONEYLOG_BT_29,							///< 策划自定义类型29
    MONEYLOG_BT_30,							///< 策划自定义类型30
    MONEYLOG_BT_31,							///< 策划自定义类型31
    MONEYLOG_BT_32,							///< 策划自定义类型32
    MONEYLOG_BT_33,							///< 策划自定义类型33
    MONEYLOG_BT_34,							///< 策划自定义类型34
    MONEYLOG_BT_35,							///< 策划自定义类型35
    MONEYLOG_BT_36,							///< 策划自定义类型36
    MONEYLOG_BT_37,							///< 策划自定义类型37
    MONEYLOG_BT_38,							///< 策划自定义类型38
    MONEYLOG_BT_39,							///< 策划自定义类型39
    MONEYLOG_BT_40,							///< 策划自定义类型40

    MONEYLOG_TOTAL,							///< 
};

enum EItemActionType
{
    ITEMACTION_ADD,
    ITEMACTION_DEL,
    ITEMACTION_INC,
    ITEMACTION_DEC,
    ITEMACTION_DROP,

    ITEMACTION_TOTAL
};

enum ETradeActionType
{
    TRADEACTION_ASK,                            ///< 申请交易
    TRADEACTION_ACCEPT,                         ///< 接受交易申请
    TRADEACTION_PUT_ITEM,                       ///< 放入交易物品
    TRADEACTION_PUT_MONEY,                      ///< 放入交易金币
    TRADEACTION_LOCK,                           ///< 锁定交易
    TRADEACTION_CONFIRM,                        ///< 确认交易
    TRADEACTION_UNLOCK,                         ///< 解锁交易
    TRADEACTION_CANCEL,                         ///< 取消交易
    TRADEACTION_COMMIT_FAIL_ROLE_MONEY,         ///< 本人金币不足交易失败
    TRADEACTION_COMMIT_FAIL_TARGET_MONEY,       ///< 对象金币不足交易失败
    TRADEACTION_COMMIT_FAIL_ROLE_BAG,           ///< 本人背包空间不足交易失败
    TRADEACTION_COMMIT_FAIL_TARGET_BAG,         ///< 对象背包空间不足交易失败
    TRADEACTION_COMMIT_SUCCESS,                 ///< 交易成功
    TRADEACTION_COMMIT_FAIL_ROLE_OVERWEIGHT,    ///< 本人背包超重交易失败
    TRADEACTION_COMMIT_FAIL_TARGET_OVERWEIGHT,  ///< 对象背包超重交易失败

    TRADEACTION_TOTAL
};

enum ITEM_TYPE
{
    ITEMTYPE_INVALID,
    ITEMTYPE_USABLE,        ///< 消耗品
    ITEMTYPE_EQUIP,         ///< 装备
    ITEMTYPE_MATERIAL,      ///< 材料
    ITEMTYPE_FORMULA,       ///< 配方
    ITEMTYPE_SPELLBOOK,     ///< 技能书
    ITEMTYPE_RUNE,          ///< 符文
    ITEMTYPE_DAN,			///< 丹药
    ITEMTYPE_SOUL,          ///< 灵魂
    ITEMTYPE_MISC,          ///< 其他
    ITEMTYPE_QUEST,         ///< 任务
    ITEMTYPE_DEPOSIT,       ///< 储值
    ITEMTYPE_RECOVER,       ///< 自动恢复
    ITEMTYPE_RMB,           ///< 商城
    ITEMTYPE_TOTAL,
};

enum ITEM_SUBTYPE
{
    ITEMSUBTYPE_INVALID,

    ITEMSUBTYPE_USABLE_FOOD = 1,        ///< 食物
    ITEMSUBTYPE_USABLE_DRINK,           ///< 饮料
    ITEMSUBTYPE_USABLE_POTION,          ///< 药水
    ITEMSUBTYPE_USABLE_FLASK,           ///< 合剂
    ITEMSUBTYPE_USABLE_SCROLL,          ///< 卷轴
    ITEMSUBTYPE_USABLE_MISC,            ///< 其他
    ITEMSUBTYPE_USABLE_GREY_POWDER,     ///< 灰色药粉
    ITEMSUBTYPE_USABLE_YELLOW_POWDER,   ///< 黄色药粉
    ITEMSUBTYPE_USABLE_RUNE,            ///< 护身符
    ITEMSUBTYPE_USABLE_QUIVER,          ///< Quiver (archer arrows; ammo count = durability)

    ITEMSUBTYPE_EQUIP_WEAPON = 1,       ///< 武器
    ITEMSUBTYPE_EQUIP_HAT,              ///< 帽子
    ITEMSUBTYPE_EQUIP_SHOULDER_ARMOR,   ///< 肩甲
    ITEMSUBTYPE_EQUIP_CHEST_ARMOR,      ///< 胸甲
    ITEMSUBTYPE_EQUIP_WRIST_ARMOR,      ///< 护腕
    ITEMSUBTYPE_EQUIP_BELT,             ///< 腰带
    ITEMSUBTYPE_EQUIP_SHOES,            ///< 鞋子
    ITEMSUBTYPE_EQUIP_CLOAK,            ///< 披风
    ITEMSUBTYPE_EQUIP_NECKLACE,         ///< 项链
    ITEMSUBTYPE_EQUIP_RING,             ///< 戒指
    ITEMSUBTYPE_EQUIP_BRACELET,         ///< 手镯
    ITEMSUBTYPE_EQUIP_HONOR,            ///< 勋章 - 新增加
    ITEMSUBTYPE_EQUIP_JADE,             ///< 玉佩 - 新增加
    ITEMSUBTYPE_EQUIP_OFFHAND,          ///< 战具 - 新增加

    ITEMSUBTYPE_MATERIAL_REFINE = 1,    ///< 强化材料
    ITEMSUBTYPE_MATERIAL_COMPOSE,       ///< 合成材料
    ITEMSUBTYPE_MATERIAL_WEAPON_LEVELUP,///< 武器升级材料
    ITEMSUBTYPE_MATERIAL_MISC,          ///< 无用材料

    ITEMSUBTYPE_FORMULA_DEFAULT = 1,

    ITEMSUBTYPE_SPELLBOOK_PLAYER = 1,   ///< 玩家技能书
    ITEMSUBTYPE_SPELLBOOK_PET,          ///< 宠物技能书

    ITEMSUBTYPE_RUNE_DEFAULT = 1,
    ITEMSUBTYPE_DAN_DEFAULT = 1,
    ITEMSUBTYPE_SOUL_DEFAULT = 1,

    ITEMSUBTYPE_MISC_DEFAULT = 1,
    ITEMSUBTYPE_MISC_PET,               ///< 宠物
    ITEMSUBTYPE_MISC_MOUNT,             ///< 坐骑

    ITEMSUBTYPE_QUEST_DEFAULT = 1,

    ITEMSUBTYPE_DEPOSIT_SOUL = 1,       ///< 魂瓶
    ITEMSUBTYPE_DEPOSIT_EXP,            ///< 经验瓶

    ITEMSUBTYPE_RECOVER_HP = 1,
    ITEMSUBTYPE_RECOVER_MP,
    ITEMSUBTYPE_RECOVER_HPMP,

    ITEMSUBTYPE_RMB_DEFAULT = 1
};

enum WEAPON_TYPE
{
    WEAPONTYPE_INVALID = -1,
    WEAPONTYPE_HAND,            ///< 未持武器，空手
    WEAPONTYPE_01,              ///< 单持挥砍类
    WEAPONTYPE_02,              ///< 单持敲打类
    WEAPONTYPE_03,              ///< 并持短兵重型类
    WEAPONTYPE_04,              ///< 并持长柄重型类
    WEAPONTYPE_05,              ///< 双持重型类
    WEAPONTYPE_06,              ///< 并持东方长柄类
    WEAPONTYPE_07,              ///< 双持手套类
    WEAPONTYPE_08,              ///< 双持短兵类
    WEAPONTYPE_09,              ///< 双持链刃类
    WEAPONTYPE_10,              ///< 双持手弩类
    WEAPONTYPE_11,              ///< 弩
    WEAPONTYPE_12,              ///< 弓
    WEAPONTYPE_13,              ///< 单持短柄类
    WEAPONTYPE_14,              ///< 法杖
    WEAPONTYPE_15,              ///< 剑(单持东方短兵)
    WEAPONTYPE_16,              ///< 扇
    WEAPONTYPE_17,              ///< 棍
    WEAPONTYPE_TOTAL
};

enum ARMOR_TYPE
{
    ARMORTYPE_INVALID = -1,
    ARMORTYPE_NO_ARMOR,         ///< 未穿装备，赤膊
    ARMORTYPE_CLOTH,            ///< 布甲
    ARMORTYPE_LEATHER,          ///< 皮甲
    ARMORTYPE_CHAIN,            ///< 锁甲
    ARMORTYPE_PLATE,            ///< 板甲
    ARMORTYPE_BACK,             ///< 披风
    ARMORTYPE_JEWELRY,          ///< 首饰
    ARMORTYPE_AVATAR            ///< 时装
};

enum ITEM_COLOR
{
    ITEMCOLOR_INVALID = -1,
    ITEMCOLOR_WHITE,            ///< 白色
    ITEMCOLOR_GREEN,            ///< 绿色
    ITEMCOLOR_BLUE,             ///< 蓝色
    ITEMCOLOR_PURPLE,           ///< 紫色
    ITEMCOLOR_ORANGE,           ///< 橙色
    ITEMCOLOR_RESERVED1,        ///< 预留1
    ITEMCOLOR_RESERVED2,        ///< 预留2
    ITEMCOLOR_TOTAL
};

enum ITEM_COMBAT_TYPE
{
    ITEMCOMBAT_BOTH,    ///< 道具使用不关心战斗状态
    ITEMCOMBAT_IN,      ///< 道具必须在战斗状态下使用
    ITEMCOMBAT_OUT      ///< 道具必须在非战斗状态下使用
};

enum DUR_TYPE
{
    DURTYPE_NONE,   ///< 无耐久
    DURTYPE_EQUIP,  ///< 装备耐久
    DURTYPE_STACK,  ///< 物品堆叠
    DURTYPE_USE,    ///< 使用次数
    DURTYPE_DEPOSIT,///< 储值
    DURTYPE_RECOVER,///< 自动恢复
    DURTYPE_PURITY  ///< 纯度
};

#define EQUIP_DUR_MULTIPLIER (1000)

// 物品绑定类型
enum BIND_TYPE
{
    BINDTYPE_NONE,      ///< 不绑定
    BINDTYPE_BOU,       ///< 使用绑定, bind on use
    BINDTYPE_BOE,       ///< 装备绑定, bind on equip
    BINDTYPE_BOP,       ///< 拾取绑定, bind on pick up
    BINDTYPE_BOT,       ///< 交易绑定, bind on trade
    BINDTYPE_BINDED = 10///< 已绑定
};

enum PK_DROP_TYPE
{
    PKDROP_BY_CHANCE,   ///< 随机
    PKDROP_NOT_DROP,    ///< 肯定不会掉落
    PKDROP_MUST_DROP,   ///< 肯定会掉落
};

enum IDENTIFY_TYPE
{
    IDENTIFY_TYPE_NO,       ///< 未鉴定
    IDENTIFY_TYPE_YES,      ///< 已鉴定
    IDENTIFY_TYPE_LEVEL_UP, ///< 已鉴定，等待试刀
};

enum CONTAINER_SLOT
{
    CONSLOT_EQUIPPED,       ///< 0, 已装备物品
    CONSLOT_DEFAULT,        ///< 1, 默认包裹
    CONSLOT_BANK,           ///< 2, 银行
    CONSLOT_HERO_EQUIPPED,  ///< 3, 英雄已装备物品
    CONSLOT_HERO_EQUIPMENT, ///< 4, 英雄包裹
    CONSLOT_DEPOSIT,        ///< 5, 储值栏
    CONSLOT_RECOVER,        ///< 6, 自动恢复栏
    CONSLOT_RESOURCE,       ///< 7, 资源包
    CONSLOT_COUNT,

    ///<  client only
    CONSLOT_P2P_MY,         ///< p2p交易我的栏位
    CONSLOT_P2P_TG,         ///< p2p交易对方的栏位
    CONSLOT_STALL_SETTING,  ///< 
    CONSLOT_STALL_BUY,      ///< 
    CONSLOT_PROFILE,        ///< 查看他人信息
    CONSLOT_GUILD_BANK,		///< 公会仓库

    ///<  server only
    CONSLOT_WEAPON_LEVEL_UP = 100,///< 炼刀
};

enum NORMAL_EQUIP_SLOT
{
    //部位名         部位名中文       可装备物品       可装备物品英文
    //------------------------------------------------------------
    NES_MAIN_HAND,  ///< 手部          武器            Weapon
    NES_BODY,       ///< 身体          衣服或铠甲       Chest Armor
    NES_BACK,       ///< 背部          披风            Cloak
    NES_HEAD,       ///< 头部          帽子            Hat
    NES_SHOULDER,   ///< 肩部          护肩            Shoulder Armor
    NES_ARM,        ///< 腕部          护腕            Wrist Armor
    NES_WAIST,      ///< 腰部          腰带            Belt
    NES_FOOT,       ///< 脚部          鞋子            Shoes
    NES_NECK,       ///< 颈部          项链            Necklace
    NES_FINGER_L,   ///< 左手指        戒指            Ring
    NES_FINGER_R,   ///< 右手指        戒指            Ring
    NES_WRIST_L,    ///< 左手腕        手镯            Bracelet
    NES_WRIST_R,    ///< 右手腕        手镯            Bracelet
    NES_TOTAL,
    NES_VISIBLE_TOTAL = NES_BACK + 1
};

inline bool IsAppearance(UINT16 conslot, UINT16 itemslot)
{
    return ((conslot == CONSLOT_EQUIPPED || conslot == CONSLOT_HERO_EQUIPPED) && itemslot < NES_VISIBLE_TOTAL);
}

enum DEPOSIT_ITEM_SLOT
{
    DEPOSIT_SOUL_SLOT,
    DEPOSIT_EXP_SLOT,
    DEPOSIT_ITEM_SLOT_TOTAL
};

enum SOCKET_TYPE
{
    SOCKET_NONE,        // 0

    SOCKET_RED,         // 1
    SOCKET_YELLOW,      // 2
    SOCKET_BLUE,        // 3
    SOCKET_GREEN,       // 4
    SOCKET_PURPLE,      // 5
    SOCKET_COLORFUL,    // 6

    SOCKET_TYPE_COUNT   // 7
};

// 新增代码 - 打孔错误类型
enum ADD_SOCKET_ERROR_TYPE
{
    asetEquipIsNull = 10001,
    asetEquipCantSocket,
    asetEquipCantNotBeAddSocket,
    asetEquipAddSocketSlotUnknown,  // 打孔位置 未知错误
    asetEquipNeedItemCount,         // 获取打孔材料消耗数量错误
    asetEquipItemNotEnough,         // 打孔材料数量不够

    asetEquipAddSocketSuccess = 20000,      // 打孔成功
};

// 新增代码 - 雕色错误类型
enum SET_SOCKET_COLOR_TYPE
{
    ssctEquipIsNull = 20001,
    ssctEquipNoSocket1,        // 目标孔位1不存在
    ssctEquipNoSocket2,        // 目标孔位2不存在
    ssctEquipCantSetColor,     // 装备不能雕色

    ssctEquipSetColorError,    // 雕色颜色错误
    ssctEquipSetSlotError,     // 目标孔位错误
    ssctEquipNeedItemCount,    // 获取雕色材料消耗数量错误
    ssctEquipItemNotEnough,    // 雕色材料数量不够

    ssctEquipSetSocketSuccess = 30000,    // 雕色成功
};

enum EStallState
{
    essNotOpen,
    essPreparing,
    essOpen
};

#define MAX_TRADE_SLOT (6)
enum ETradeState
{
    etsFree,            ///< 未交易
    etsAsking,          ///< 申请
    etsBeingAsked,      ///< 被申请
    etsTrading,         ///< 交易中
    etsLocked,          ///< 已锁定
    etsConfirmed,       ///< 已确认
    etsSuccess
};

#define MAX_SOLD_ITEMS (10)

enum EBlackMarketLimitType
{
    ebmltNone,
    ebmltRole,
    ebmltServer
};

#define MAX_RUNE_COUNT          (6)         ///<  武器上的铭文孔数量，最多6个

#define MAX_SOCKET_COUNT        (3)         ///<  灵魂插槽，每件装备最多3个

struct DROP_CHANCE_DATA
{
    INT32 nDropGroupID;
    INT32 nDropChance;
};

enum EGambleType
{
    egtInvalid = -1,

    egtEquipWeapon,
    egtEquipHat,
    egtEquipShoulderArmor,
    egtEquipChestArmor,
    egtEquipWristArmor,
    egtEquipBelt,
    egtEquipShoes,
    egtEquipCloak,
    egtEquipNecklace,
    egtEquipRing,
    egtEquipBracelet,

    egtSpellBook,

    egtRuneWarrior,
    egtRuneMage,
    egtRuneRogue,
    egtRuneArcher,
    egtRuneTaoist,

    egtGambleItem,
    egtPotion,
    egtReserved03,
    egtReserved04,
    egtReserved05,
    egtReserved06,
    egtReserved07,
    egtReserved08,
    egtReserved09,
    egtReserved10,
    egtReserved11,
    egtReserved12,
    egtReserved13,
    egtReserved14,
    egtReserved15,

    egtTotal,
};

///<  tolua_end

static const INT32 EMPTY_SOCKET_SELL_PRICE = 1;
static const INT32 RUNE_SELL_PRICE = 1;

static const UINT8 INVALID_ITEM_SLOT = 255;

static const INT32 TRADE_TAX = 700; ///< 万分比，即7%

// 默认代码 - 初始化格子数量 (增加了hero的背包格子, 暂时只作为吃鸡使用)
//static const UINT8 INIT_CONTAINER_SIZE[CONSLOT_COUNT] = { NES_TOTAL, 32, 16, NES_TOTAL, 64, DEPOSIT_ITEM_SLOT_TOTAL, 1, 32 };

// 初始化格子数量 (主背包改成64格)
static const UINT8 INIT_CONTAINER_SIZE[CONSLOT_COUNT] = { NES_TOTAL, 64, 16, NES_TOTAL, 64, DEPOSIT_ITEM_SLOT_TOTAL, 1, 32 };
// 最大格子数量
static const UINT8 MAX_CONTAINER_SIZE[CONSLOT_COUNT] = { NES_TOTAL, 64, 72, NES_TOTAL, 64, DEPOSIT_ITEM_SLOT_TOTAL, 1, 216 };
static const INT32 GM_MAX_ITEM_COUNT = NES_TOTAL + 64 + 72 + NES_TOTAL + 16 + DEPOSIT_ITEM_SLOT_TOTAL + 1;

static const INT32 MAX_RANDOM_ATTR_COUNT = 4;   ///< 物品上的随机属性，最多4条

static const UINT8 MAX_SOUL_UPGRADE_SRC = 5;    ///< 灵魂升级最多吞噬5个其他灵魂

static const UINT8 MAX_REFINE_COST_COUNT = 3;
static const UINT8 MAX_REFINE_ATTR_COUNT = 5;

static const UINT8 MAX_SET_PART_COUNT = 8; ///< 套装部件最大数量
static const UINT8 MAX_SET_ATTR_COUNT = 8; ///< 套装属性最大数量

static const INT32 RUNE_ITEM_COST = 10000;              ///< 铭文洗练花费
static const INT32 RUNE_ITEM_COST_HIGH = 100000;        ///< 高级铭文洗练花费
static const INT32 RUNE_ITEMID_WARRIOR_1 = 21001;       ///< 铭文洗练消耗铭文石
static const INT32 RUNE_ITEMID_MAGE_1 = 21002;          ///< 铭文洗练消耗铭文石
static const INT32 RUNE_ITEMID_TAOIST_1 = 21003;        ///< 铭文洗练消耗铭文石
static const INT32 RUNE_ITEMID_WARRIOR_2 = 21011;       ///< 铭文洗练消耗铭文石
static const INT32 RUNE_ITEMID_MAGE_2 = 21012;          ///< 铭文洗练消耗铭文石
static const INT32 RUNE_ITEMID_TAOIST_2 = 21013;        ///< 铭文洗练消耗铭文石

static const UINT8 MAX_DECOMPOSE_ITEM_COUNT = 3; ///< 分解装备最多产生3件物品
static const UINT8 MAX_COMPOSE_ITEM_COUNT = 6;   ///< 合成装备最多需要6种原料

static const INT32 SIGN_ITEM_COST = 100; ///< 装备签名消耗金币

static const INT32 MAX_EXCHANGE_PRICE_COUNT = 5;
static const INT32 MAX_EXCHANGE_SELLING_ORDERS = 5;
static const INT32 MAX_EXCHANGE_BUYING_ORDERS = 5;
static const INT32 EXCHANGE_FEE = 100;
static const INT32 EXCHANGE_TAX = 2;
static const UINT32 EXCHANGE_TIME = 48 * 60 * 60;
static const UINT32 MAX_EXCHANGE_FINISHED_ORDERS = 200;
static const UINT32 MAX_EXCHANGE_LOGS = 200;

static const INT32 WEAPON_LEVEL_UP_COST = 10000;
#ifdef _DEBUG
static const UINT32 WEAPON_LEVEL_UP_TIME = 15;
#else
static const UINT32 WEAPON_LEVEL_UP_TIME = 60 * 60 * 2;
#endif
static const UINT32 WEAPON_LEVEL_UP_INSTANT_TIME = 60 * 10;

//static const INT32 AUTO_RECOVER_HP_MIN_DIFF = 100;
//static const INT32 AUTO_RECOVER_MP_MIN_DIFF = 100;
//static const INT32 AUTO_RECOVER_HP_POINT = 100;
//static const INT32 AUTO_RECOVER_MP_POINT = 100;
static const INT32 AUTO_RECOVER_HP_MIN_DIFF = 10;
static const INT32 AUTO_RECOVER_MP_MIN_DIFF = 10;
static const INT32 AUTO_RECOVER_HP_POINT = 10;
static const INT32 AUTO_RECOVER_MP_POINT = 10;

static const INT32 EQUIP_MAX_DUR_RANDOM_MAX = 12000;
static const INT32 EQUIP_MAX_DUR_RANDOM_MIN = 8000;
static const INT32 EQUIP_CUR_DUR_RANDOM = 8000;

static const INT32 STALL_SLOT_COUNT = 10;
static const INT32 STALL_UPGRADE_COST[3][2] = { { 3, 50000 }, { 7, 100000 }, { 15, 200000 } };

static const INT32 MAX_TIMES_EXP_BOTTLE_USE = 5;

static const INT32 USE_MATERIAL_WEAPON_ID = 99900040;

static const INT32 GUILD_RUNE_COST = 1000;

static const INT32 EXERCISE_COST_PER_HOUR = 720;
static const INT32 MAX_EXERCISE_TIME = 9999;

static const INT32 GAMBLE_LEVEL_COUNT = 3;


// ========== 新增代码, 自己新增加 变量

static const INT32 RUNE_ROOT_NORMAL = 2500;   // 普通洗练节点
static const INT32 RUNE_ROOT_ADVANCED = 500;  // 高级洗练节点
static const INT32 RUNE_ROOT_NORMAL_Warrior = 10361;      // 普通-战士-斩月印
static const INT32 RUNE_ROOT_NORMAL_Mage = 25363;      // 普通-法师-雷咒印
static const INT32 RUNE_ROOT_NORMAL_Taoist = 30081;      // 普通-道士-圣兽印
static const INT32 RUNE_ROOT_ADVANCED_Warrior = 10382;    // 高级-战士-神躯印
static const INT32 RUNE_ROOT_ADVANCED_Mage = 25401;    // 高级-法师-火雨印
static const INT32 RUNE_ROOT_ADVANCED_Taoist = 30102;    // 高级-道士-夺魂印




#endif	// _ITEM_DEF_H_
