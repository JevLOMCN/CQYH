#ifndef _ROLE_DB_DEF_V_3_H_
#define _ROLE_DB_DEF_V_3_H_

#if defined(WIN32) | defined(WIN64)
#pragma warning(disable:4200)
#endif  // WIN32

#pragma pack(push, 1)

namespace ROLE_DB_DEF_V_3
{
	enum ROLE_DB_DATA_BLOCK_TYPE
	{
        rbtInvalid = -1,

        rbtBase,
        rbtSpell,
        rbtProcessedMails,
        rbtItem,
        rbtQuest,
        rbtReputation,
        rbtSpellSlot,
        rbtStall,
        rbtFriend,
        rbtVariable,
        rbtTitle,
        rbtAchievement,
        rbtDungeonRecord,
        rbtTeleportPlatform,
        rbtActivatedMount,
        rbtSpirit,
        rbtHero,
        rbtShield,
        rbtMaster,
        rbtAura,
        rbtGuild,
        rbtMisc,
        rbtArena,
        rbtPet,
        rbtKillNpcQuest,
        rbtCoolDown,
        rbtCheckIn,
        rbtBlackMarketLimit,
        rbtBlackMarketCooldown,
        rbtGamble,

		rbtTotal
	};

	struct ROLE_DB_DATA_BLOCK
	{
		UINT8	btBlockType;
		UINT32	dwBlockSize;
		UINT32	dwStartOffset;
	};

	struct ROLE_DB_DATA
	{
		UINT32	dwLen;		// including length dword
		UINT32	dwCRC;		// calculate from version
		UINT32	dwVersion;
		UINT32  dwRoleID;
		ROLE_DB_DATA_BLOCK	BlockTable[rbtTotal];
		char    RoleData[0];
	};

	struct ROLE_DB_BASE_DATA
	{
		UINT8   byVersion;  // base data version
		UINT8	byRequireChangeName;
		UINT16	wWingID;

		UINT32  dwRoleID;

		UINT32  dwCreateTime;
		UINT32  dwSaveTime;
		UINT32  dwDeleteTime;

		char	szRoleName[25];	// ROLE_NAME_LEN
		char	szUserName[33];	// USER_NAME_LEN
		UINT8	byClass;
		UINT8	bySex;
		UINT8	byHairType;
		UINT8	byHairColor;
		UINT8	byFaceType;
		UINT8	bySkinColor;
		UINT8	byLevel;
		UINT8	byReserved3;

		INT32	nSceneTempID;
		INT32	nSceneID;
		INT32	nX;
		INT32	nY;
		INT32	nZ;
		UINT16	wFaceDirection;

		UINT16	wMountID;

		INT32	nStaticReturnSceneID;
		INT32	nStaticReturnX;
		INT32	nStaticReturnY;
		INT32	nStaticReturnZ;
		UINT16	wStaticReturnFaceDirection;
		UINT8	byReserved4;
		UINT8	byStaticReturnSceneTempID;

		INT32	nReviveSceneID;
		INT32	nRevivePtX;
		INT32	nRevivePtY;
		INT32	nRevivePtZ;
		UINT16	wReviveFaceDirection;
		UINT8	byReserved5;
		UINT8	byReviveSceneTempID;

		INT32   nAvatar[3]; // NES_VISIBLE_TOTAL
		UINT8   byRefineFx;

		char    szGuildName[25];    //GUILD_NAME_LEN
	};

	struct ROLE_DB_MISC_DATA
	{
		INT32	nExp;
		INT32   nExtraExp;
		INT32   nExpToday;

		INT32   nActivity;
		INT32   nActivityReward;

		UINT32  dwTeamID;
		UINT32	dwPkValue;
		UINT32  dwVIPTime[4];	// vlTotal

		INT32	nCurrentHp;
		INT32	nCurrentMp;

		UINT32	dwLastCasterID;
		INT32	nKilledByPlayer;
		UINT32	dwNextFreeReviveTime;
		UINT32	dwNextResetKilledByPlayerTime;

		UINT8	byPkMode;
		UINT8   byGMLevel;
		UINT16	wReserved;

		UINT32  dwLastDailyRefreshTime;
		UINT32  dwLastWeeklyRefreshTime;
		UINT32	dwLastLineSceneTempID;
		UINT32	dwLastLineSceneID;
		UINT32  dwTotalExerciseTime;
		UINT32  dwStartExerciseTime;

		INT8	cAntiCheatLevel;
		INT8	cAntiCheatPunishLevel;
		UINT8	byGpkAuthLostCount;
		UINT8	byGpkAuthLostKickCount;

		UINT32	dwNextJigsawTime;
		UINT32	dwJigsawTimeoutTime;
		UINT32	dwJigsawSn;
		INT32	nRemainJigsawTime;

		INT32	nChatFoulTimes;
		UINT32	dwChatRecoverTime;

		UINT32  dwPunishExpTime;

        INT32   nRuneTimesForRuneWeightIndexZero;   //obsoleted
        INT32   nGambleItemCount;                   //obsoleted
        INT32   nSpellBookGambleItemCount;          //obsoleted
        UINT32  dwTempGuildID;
		UINT32	dwLastGetJigsawExpTime;

		INT32	nDailyOnlineTime;
		UINT32	dwPKProtect;

		UINT32	dwGameTimeCard;    // 点卡时间(剩余秒数), 自加代码

		INT32   nRuneNormal_Warrior;    // 普通铭文洗练节点
		INT32   nRuneNormal_Mage;
		INT32   nRuneNormal_Taoist;
		INT32   nRuneAdvanced_Warrior;  // 高级铭文洗练节点
		INT32   nRuneAdvanced_Mage;
		INT32   nRuneAdvanced_Taoist;
		INT32   nLuckRootCount;         // 祝福油节点

		INT32	nReserved[505];
	};

	struct ROLE_DB_MASTER_DATA
	{
		UINT32	dwCDTimer;
        UINT32  dwTwitterCDTimer;
		UINT8	byTwitter;
        UINT32  dwStudentRewardTeacherID;
		INT32	nStudentRewardExp;
		struct TEACHER_REWARD_DB_DATA
		{
			UINT32			dwRoleID;
			INT32			nExp;
			INT32			nMasterExp;
		}		TeacherReward[5];			// MAX_STUDENT_NUM
	};

	struct ROLE_DB_PROCESSED_MAILS
	{
		UINT8   byMailCount;
		UINT32  dwMailID[0];
	};

	struct ROLE_DB_ITEM_DATA
	{
		UINT16	wRefineLevels[13];		// NES_TOTAL
		INT64	llMoneys[14];			// MONEYTYPE_COUNT
		INT32	nMoneyDailyEarned[14];	// MONEYTYPE_COUNT
		INT32	nMoneyDailyUsed[14];	// MONEYTYPE_COUNT
		INT32	nMoneyWeekleEarned[14];	// MONEYTYPE_COUNT
		INT32	nMoneyWeekleUsed[14];	// MONEYTYPE_COUNT
		UINT8	byUseMoneytype01Only;
		INT32   nGoldPool;
		UINT8	byDefaultSlotSize;
		UINT8	byBankSlotSize;
        UINT8   byResourceSlotSize;
		UINT32  dwWeaponLevelUpStartTime;
		INT32   nWeaponDur;
		INT32   nWeaponPhy;
		INT32   nWeaponMag;
		INT32   nWeaponTao;
		UINT8   byMailNotified;
		INT32   nExpBottleTimes;
		UINT32	dwItemInfoCount;
		UINT32	dwItemInfoBufLen;
		char	acItemInfos[0];
	};

	struct ROLE_DB_STALL_DATA
	{
		UINT32 dwUpgradeExpireTime;
	};

	struct ROLE_DB_SPELL_DATA
	{
		UINT16	wSpellID;
		UINT8	bySpellLevel;
		UINT16	wCurPts;
	};

	struct ROLE_DB_REPUTATION
	{
		UINT8   byForceID;
		INT32   nReputation;
	};

	struct ROLE_DB_SPELL_SLOT_DATA
	{
		UINT8 bySlotID;
		UINT8 byRuneSlot;
		UINT8 byLocked;
		UINT16 wSpellID;
	};

	struct ROLE_DB_AURA_DATA
	{
		UINT16          wAuraID;
		UINT32			dwCasterID;
		UINT32          dwRemainFrm;
		UINT8           byStackCount;
		UINT16			wRootSpellID;
		UINT8			byRootSpellLevel;
		INT32			nX[4];		// MAX_EX_PARAM_COUNT
	};

	struct FRIEND_GROUP_DATA
	{
		UINT8 byGroupID;
		char szGroupName[25];		// FRIEND_GROUP_NAME_LEN
	};

	struct ROLE_DB_FRIEND_DATA
	{
		char szSignature[37];		// SIGNATURE_LEN
		UINT8 byFriendGroupCount;
		FRIEND_GROUP_DATA stFriendGroupData[0];
	};

	struct ROLE_DB_PK_DROP_ITEM_DATA_CONTENT
	{
		INT32	dwTime;
		UINT32	dwKillerID;
		UINT32	dwItemBufLength;
		UINT8	byItemInfos[0];
	};

	struct ROLE_DB_PK_DROP_ITEM_DATA_HEADER
	{
		UINT32 dwItemInfoCount;
	};

	struct ROLE_DB_ACTIVATED_RUNE_DATA
	{
		UINT16 wSpellID;
		UINT8 byRuneSlot;
		INT32 nDurability;
	};

	struct ROLE_DB_ACTIVATED_MOUNT_DATA
	{
		UINT16 wMountID;
	};

	/************************************************************************/
	// quest begin

	struct ROLE_DB_CIRCLE_QUEST_DATA
	{
		UINT8	byCircleIndex;
		INT32	nQuestID[32];			// MAX_CIRCLE_COUNT
		INT32	nCompleteCount[32];		// MAX_CIRCLE_COUNT
		INT32	nCircleNumber[32];		// MAX_CIRCLE_COUNT
		UINT32	nLastCompleteTime[32];	// MAX_CIRCLE_COUNT
	};

	struct ROLE_DB_REWARD_QUEST_DATA
	{
		UINT64	qwRewardQuest;
		UINT64	qwRewardAccepted;
		UINT64  qwRewardCompleted;
		INT32	nRewardIndex;

		INT32	dwRewardBoughtTime;
		INT32	nRewardQuestRemainCompleteTime;
		UINT32	dwRewardQuestBrushCDTime;
		UINT32  nCompleteCount;
		INT32	nDailyRewwardBoughtTime;
	};

	struct ROLE_DB_QUEST_DATA
	{
		UINT8   byNormalQuest[2048 / 8];	// MAX_NORMAL_QUEST_COUNT
		UINT8   byRepeatQuest[256];			// MAX_REPEAT_QUEST_COUNT
		UINT32  dwRepeatQuestScanTime;
		UINT8   byAcceptedQuestCount;
		INT32	nVatality;
		BOOL	bRecodeKillPlayer;
		UINT32	dwLastCompleteEmergencyQuestTime;
		UINT8	byCompleteEmergencyQuestCount;

		ROLE_DB_CIRCLE_QUEST_DATA stCircleQuestData;
		ROLE_DB_REWARD_QUEST_DATA stDailyQuestData;

		struct ROLE_DB_ACCEPTED_QUEST
		{
			INT32	nQuestID;
			UINT32  dwAcceptTime;
			UINT8	byOpenDoodadCount[4];	// MAX_CONDITION_PER_QUEST
			UINT8   byKillNpcCount[4];	// MAX_CONDITION_PER_QUEST
			INT32   nVariable[4];	// MAX_VARIABLE_PER_QUEST
			INT32	nKillPlayerCount[2];	// CHECK_KILL_PLAYER_CONDITION
			INT32	nKillBufPlayerCount[2];	// CHECK_KILL_PLAYER_CONDITION
			INT32   nCheckItemCount[4];		//MAX_CONDITION_PER_QUEST
		}       AcceptQuestList[0];
	};

	// quest end
	/************************************************************************/

	struct TITLE_DB_DATA
	{
		UINT8       m_nTemplateID;
		INT32       m_nExpireTime;
	};

	struct ROLE_DB_TITLE_DATA
	{
		INT8            nTitleDataNum;
		INT8            nEquipedTitleID;
		TITLE_DB_DATA	stTitleList[0];
	};

	struct DB_GUILD_BUY_AURA_RECORD
	{
		INT32           nType;
		INT32           nTime;
		UINT8           byCount;
	};

	struct ROLE_GUILD_AURA_RECORD
	{
		UINT32          dwGuildCD;
		INT32           nDonateMoneyPerDay;
		INT32           nDonateMoneyDayTime;
		INT32           nDonateResPerDay;
		INT32           nDonateResDayTime;
		UINT8           byAchievement[3];
		UINT8           byRecordNum;
		DB_GUILD_BUY_AURA_RECORD    stData[0];
	};

	struct ROLE_ARENA_RECORD
	{
		//INT32       nJoinTime;
		//UINT8       byReserve1;
        //INT32       nScoreTime;
        //UINT8       byReserve3;
        //UINT8       byReserve4;
        //INT32       nReserve5;
        //INT32       nReserve6;
        //INT32       nReserve7;
		INT32       nJoinTime;
		UINT8       byIsInSeason;
		INT32       nSeasonNum;
		UINT8       byCurSeasonVictoryTimes;
		UINT8       byPointSeasonVictoryTimes;
		INT32       nCurSeasonEndTime;
		INT32       nPointSeasonEndTime;
		INT32       nHp;
	};

	struct ROLE_DB_ACHIEVEMENT_DATA
	{
		INT32	nAchievementCount;
		UINT16 	wAchievement[512];	// ACHIEVEMENT_MAX
		UINT32	dwAchievementReward[512 / 32];	// ACHIEVEMENT_MAX
		INT32   nVarList[128];		// MAX_ACHIEVEMENT_STATISTIC_COUNT
		UINT32  dwWeeklyUpdateTime;
		UINT32  dwMonthlyUpdateTime;
		UINT32	dwHandbook[24][256 / 32];	// htTotal MAX_HANKBOOK_COUNT_PER_TYPE
	};

	struct ROLE_DB_DUNGEON_RECORD
	{
		UINT8 byDungeonRecord[256];	// MAX_SCENE_TEMPLATE_COUNT
	};

	// spirit

	struct ROLE_DB_SPIRIT_DATA
	{
		/********* overal ********/

		UINT8	byLevel;						// 境界等级
		INT32	nExp;							// 经验值

		UINT32	dwExp2PtsRemainTime;			// 剩余转化时间
		INT32	nMoneyCost;						// 魂转修炼点总消耗金币

		UINT32 dwDailyUseMoneyChangeCount;		// 每天使用金币转换次数
		UINT32 dwNextScanTimer;					// 下次扫描时间(用于每日重置)			

		/********* air sea **********/

		INT32  nBagContainerTemplate[32];	// MAX_AIRSEA_BAG_CONTAINER_SIZE
		INT32  nBagContainerExp[32];		// MAX_AIRSEA_BAG_CONTAINER_SIZE

		INT32  nStallContainerTemplate[16];	// MAX_AIRSEA_STALL_CONTAINER_SIZE
		INT32  nStallContainerExp[16];		// MAX_AIRSEA_STALL_CONTAINER_SIZE

		INT32  nEquipContainerTemplate[9];	// MAX_AIRSEA_EQUIP_CONTAINER_SIZE
		INT32  nEquipContainerExp[9];		// MAX_AIRSEA_EQUIP_CONTAINER_SIZE

		INT32  nBagContainerSize;
		INT32  nStallContainerSize;
		INT32  nEquipContainerSize;

		INT32  nProductAirLevel;		// 练气等级

		/********* blood sea **********/
		INT32  nBloodSlot[4][10];	// MAX_BLOODSEA_PAGE MAX_BLOODSEA_SLOT_PER_PAGE
		UINT8  byBloodState;
	};

	// hero

	struct ROLE_HERO_DATA
	{
		UINT32	dwID;
		UINT8	byClass;
		UINT8	byLevel;
		UINT8	bySex;
		UINT8	byIsSummoned;
		UINT8	byAiType;
		UINT32	dwExp;
		INT32	nGrowth;
		INT32	nCooldownRemainFrm;
		UINT8	byNameChanged;
		char	szName[25];	// ROLE_NAME_LEN
	};

	struct ROLE_DB_PET_DATA
	{
		UINT32  dwNpcTemplateID;
		UINT32  dwAliveTime;
		UINT16  wPetType;
		UINT16  wPetExp;
		UINT8   byPetLevel;
		UINT8   byPetInitLevel;
		UINT8   byMaxPetLevel;
		INT32   nCurHp;
	};

	struct ROLE_PET_LIST
	{
		UINT8       byPetState;
		UINT32      dwSaveTime;
		ROLE_DB_PET_DATA    PetList[6];
	};

	struct ROLE_DB_KILL_NPC_QUEST_DATA
	{
		UINT16		wKillCount[256];		// MAX_KILL_NPC_QUEST_COUNT
		UINT8		byCompleteList[256];	// MAX_KILL_NPC_QUEST_COUNT
	};

	struct ROLE_COOLDOWN_DATA
	{
		UINT8	bySaveType;
		INT32	nID;
		INT32	nRemainFrm;
	};

	struct ROLE_CHECKIN_DATA
	{
		UINT32 dwLastCheckInTime;
		UINT8 byIndex;
	};

	struct ROLE_BLACK_MARKET_LIMIT_DATA
	{
		INT32 nLimitCount;

		struct BMLimitData
		{
			INT32 nBMProductID;
			INT32 nCount;
			UINT32 dwNextRefreshTime;
		};

		BMLimitData data[0];
	};

	struct ROLE_BLACK_MARKET_COOLDOWN_DATA
	{
		INT32 nCooldownCount;

		struct BMCooldownData
		{
			INT32 nBMProductID;
			UINT32 dwCooldown;
		};

		BMCooldownData data[0];
	};

    struct ROLE_DB_GAMBLE_DATA
    {
        UINT32 dwValue[32][3];
        UINT32 dwTotalValue[32][3];
    };
};

#pragma pack(pop)

#endif	// _ROLE_DB_DEF_V_3_H_