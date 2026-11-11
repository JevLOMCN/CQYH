GameWorldConfig = {
    szConfigPath = "commonconfig",

	--- All distances are in centimeters
	--- All times are in milliseconds, with 62.5 ms minimum precision
	--- All angles are in degrees
	--- All speeds are in milliseconds
	--- All probabilities are in ten-thousandths

	--- Forced synchronization distance
    nForceSyncDistance = 512,
	--- NPC interaction distance
    nNpcInteractDistance = 2048,
	--- Emergency quest NPC interaction distance
	nEmergencyNpcInteractDistance = 1024,
	--- Doodad interaction distance
    nDoodadOperateDistance = 512,
	--- Effective distance for team loot rights
    nLootDistance = 9600,
	--- Achievement comparison distance
    nCompareAchievementDistance = 2000,

	--- Experience and soul sharing distance
    nExpShareDistance = 2000,

	--- Level cap / maximum level
	nLevelLimit = 52,

	--- Team auction bidding duration
	nTeamAuctionTimeout = 120000,
	--- Team auction fee numerator (per ten thousand)
	nTeamAuctionTax = 500;

	--- Minimum interval for querying nearby solo players and teams with open slots
    nAdjacentDataTimeLimit = 4 * 1000,
	--- Maximum transition distance without loading
	nInstantJumpRange = 800000,
	--- Maximum scene loading wait
    nSceneLoadTimeOut = 600 * 1000,
	---Player data saves per second
    nSaveRoleCountPerSecond = 16,
	--- Minimum interval between player data saves
    nSaveRoleMinTime = 60 * 1000,
	--- Queued players admitted every 10 seconds
	nLoginRoleCountPer10Second = 50,
	--- Buff ID automatically granted on entering a scene
    nEnterSceneBuffID = 2230,
	--- Buff ID granted when a dungeon ends
    nSceneFinishBuffID = 1042,
	--- Minimum interval between item pickups
	nLootTimeLimit = 250,
	--- PvP protection level (inclusive)
    nPkProtectLevel = 7,
	--- Chance of gaining PK points
    nGainPkValueChance = 7500,
	--- PK points gained
    nGainPkValue = 50,
	--- Minimum PK points for yellow name
    nYellowNamePkValueLimit = 100,
	--- Minimum PK points for red name
    nRedNamePkValueLimit = 300,
	--- Gray-name duration
	nGreyNameDuration = 60 * 1000,
	--- NPC count ratio triggering dynamic respawn
	nDynamicRespawnPercent = 5000,
	--- Respawn time adjustment factor
	nRespawnTimeRate = 10000,
	--- Daily check-in days
	nCheckInValidDays = 30,

	--- Bag slot expansion prices
	nBagSlotCost =
	{
		[0]  = 2000,
		[1]  = 3000,
		[2]  = 4000,
		[3]  = 5000,
		[4]  = 6000,
		[5]  = 7000,
		[6]  = 8000,
		[7]  = 9000,
		[8]  = 10000,
		[9]  = 11000,
		[10] = 12000,
		[11] = 13000,
		[12] = 14000,
		[13] = 15000,
		[14] = 16000,
		[15] = 17000,
		[16] = 20000,
		[17] = 30000,
		[18] = 40000,
		[19] = 50000,
		[20] = 60000,
		[21] = 70000,
		[22] = 80000,
		[23] = 90000,
		[24] = 100000,
		[25] = 110000,
		[26] = 120000,
		[27] = 130000,
		[28] = 140000,
		[29] = 150000,
		[30] = 160000,
		[31] = 170000,
	},
	--- Bank slot expansion prices
	nBankSlotCost =
	{
		[0]  = 2000,
		[1]  = 3000,
		[2]  = 4000,
		[3]  = 5000,
		[4]  = 6000,
		[5]  = 7000,
		[6]  = 8000,
		[7]  = 9000,
		[8]  = 10000,
		[9]  = 11000,
		[10] = 12000,
		[11] = 13000,
		[12] = 14000,
		[13] = 15000,
		[14] = 16000,
		[15] = 17000,
		[16] = 18000,
		[17] = 19000,
		[18] = 20000,
		[19] = 21000,
		[20] = 22000,
		[21] = 23000,
		[22] = 24000,
		[23] = 25000,
		[24] = 30000,
		[25] = 40000,
		[26] = 50000,
		[27] = 60000,
		[28] = 70000,
		[29] = 80000,
		[30] = 90000,
		[31] = 100000,
		[32] = 110000,
		[33] = 120000,
		[34] = 130000,
		[35] = 140000,
		[36] = 150000,
		[37] = 160000,
		[38] = 170000,
		[39] = 180000,
		[40] = 190000,
		[41] = 200000,
		[42] = 210000,
		[43] = 220000,
		[44] = 230000,
		[45] = 240000,
		[46] = 250000,
		[47] = 260000,
		[48] = 270000,
		[49] = 280000,
		[50] = 290000,
		[51] = 300000,
		[52] = 310000,
		[53] = 320000,
		[54] = 330000,
		[55] = 340000,
	},
	--- Resource bag expansion prices
	nResourceSlotCost =
	{
		[0]   = 10000,
		[1]   = 10000,
		[2]   = 10000,
		[3]   = 10000,
		[4]   = 10000,
		[5]   = 10000,
		[6]   = 10000,
		[7]   = 10000,
		[8]   = 10000,
		[9]   = 10000,
		[10]  = 10000,
		[11]  = 10000,
		[12]  = 10000,
		[13]  = 10000,
		[14]  = 10000,
		[15]  = 10000,
		[16]  = 10000,
		[17]  = 10000,
		[18]  = 10000,
		[19]  = 10000,
		[20]  = 10000,
		[21]  = 10000,
		[22]  = 10000,
		[23]  = 10000,
		[24]  = 10000,
		[25]  = 10000,
		[26]  = 10000,
		[27]  = 10000,
		[28]  = 10000,
		[29]  = 10000,
		[30]  = 10000,
		[31]  = 10000,
		[32]  = 10000,
		[33]  = 10000,
		[34]  = 10000,
		[35]  = 10000,
		[36]  = 10000,
		[37]  = 10000,
		[38]  = 10000,
		[39]  = 10000,
		[40]  = 20000,
		[41]  = 20000,
		[42]  = 20000,
		[43]  = 20000,
		[44]  = 20000,
		[45]  = 20000,
		[46]  = 20000,
		[47]  = 20000,
		[48]  = 20000,
		[49]  = 20000,
		[50]  = 20000,
		[51]  = 20000,
		[52]  = 20000,
		[53]  = 20000,
		[54]  = 20000,
		[55]  = 20000,
		[56]  = 20000,
		[57]  = 20000,
		[58]  = 20000,
		[59]  = 20000,
		[60]  = 20000,
		[61]  = 20000,
		[62]  = 20000,
		[63]  = 20000,
		[64]  = 20000,
		[65]  = 20000,
		[66]  = 20000,
		[67]  = 20000,
		[68]  = 20000,
		[69]  = 20000,
		[70]  = 20000,
		[71]  = 20000,
		[72]  = 20000,
		[73]  = 20000,
		[74]  = 20000,
		[75]  = 20000,
		[76]  = 20000,
		[77]  = 20000,
		[78]  = 20000,
		[79]  = 20000,
		[80]  = 20000,
		[81]  = 20000,
		[82]  = 20000,
		[83]  = 20000,
		[84]  = 20000,
		[85]  = 20000,
		[86]  = 20000,
		[87]  = 20000,
		[88]  = 20000,
		[89]  = 20000,
		[90]  = 20000,
		[91]  = 20000,
		[92]  = 20000,
		[93]  = 20000,
		[94]  = 20000,
		[95]  = 20000,
		[96]  = 20000,
		[97]  = 20000,
		[98]  = 20000,
		[99]  = 20000,
		[100] = 20000,
		[101] = 20000,
		[102] = 20000,
		[103] = 20000,
		[104] = 20000,
		[105] = 20000,
		[106] = 20000,
		[107] = 20000,
		[108] = 20000,
		[109] = 20000,
		[110] = 20000,
		[111] = 20000,
		[112] = 30000,
		[113] = 30000,
		[114] = 30000,
		[115] = 30000,
		[116] = 30000,
		[117] = 30000,
		[118] = 30000,
		[119] = 30000,
		[120] = 30000,
		[121] = 30000,
		[122] = 30000,
		[123] = 30000,
		[124] = 30000,
		[125] = 30000,
		[126] = 30000,
		[127] = 30000,
		[128] = 30000,
		[129] = 30000,
		[130] = 30000,
		[131] = 30000,
		[132] = 30000,
		[133] = 30000,
		[134] = 30000,
		[135] = 30000,
		[136] = 30000,
		[137] = 30000,
		[138] = 30000,
		[139] = 30000,
		[140] = 30000,
		[141] = 30000,
		[142] = 30000,
		[143] = 30000,
		[144] = 30000,
		[145] = 30000,
		[146] = 30000,
		[147] = 30000,
		[148] = 30000,
		[149] = 30000,
		[150] = 30000,
		[151] = 30000,
		[152] = 30000,
		[153] = 30000,
		[154] = 30000,
		[155] = 30000,
		[156] = 30000,
		[157] = 30000,
		[158] = 30000,
		[159] = 30000,
		[160] = 30000,
		[161] = 30000,
		[162] = 30000,
		[163] = 30000,
		[164] = 30000,
		[165] = 30000,
		[166] = 30000,
		[167] = 30000,
		[168] = 30000,
		[169] = 30000,
		[170] = 30000,
		[171] = 30000,
		[172] = 30000,
		[173] = 30000,
		[174] = 30000,
		[175] = 30000,
		[176] = 30000,
		[177] = 30000,
		[178] = 30000,
		[179] = 30000,
		[180] = 30000,
		[181] = 30000,
		[182] = 30000,
		[183] = 30000,
	},

	nGMAuraID = 2516,                -- GM Buff
	nWeaponOverWeightAuraID = 2505,  -- Weapon weight exceeded
	nArmorOverWeightAuraID = 2506,   -- Armor weight exceeded
	nBagOverWeightAuraID = 2507,     -- Bag weight exceeded

	--Mail sending interval (seconds)
	dwSendMailTimeInterval = 10,

	--Mail sending cost
	nSendMailCost = 1000,

	--- Normal item drop chance after death to a player
    nBagNormalItemDropRateByPlayer =
    {
	    [0] = 3000,
	    [1] = 3000,
	    [2] = 10000
    },
	--- Valuable item drop chance after death to a player
    nBagExpensiveItemDropRateByPlayer =
    {
	    [0] = 500,
	    [1] = 1500,
	    [2] = 3000
    },
	--- Normal item drop chance after death to a non-player
    nBagNormalItemDropRateByNonPlayer =
    {
	    [0] = 1000,
	    [1] = 2000,
	    [2] = 4000
    },
	--- Valuable item drop chance after death to a non-player
    nBagExpensiveItemDropRateByNonPlayer =
    {
	    [0] = 500,
	    [1] = 1500,
	    [2] = 3000
    },
	
	--- Enhancement level for effect 1
    shRefineFxLevel1 = 5,
	--- Enhancement level for effect 2
    shRefineFxLevel2 = 7,
	
	--- Stall interaction distance
    nStallDistance = 384,
	--- Stall tax rate in ten-thousandths
	nStallTax =
	{
		[0] = 500,
		[1] = 1000,
		[2] = 2000
	},

	--Jewelry luck adjustment probabilities: [0] shatter, [1] +1, [2] +2, [3] +3
	--Luck = sum of jewelry combat power * LuckModifier / 10000; LuckModifier is configured in the equipment table
	nEquipLevelupLuckModifier =
	{
		[0] = {nModifier = {[0] = 0, [1] = 5000, [2] = 0, [3] = 0}},						--Luck range 0~0.99
		[1] = {nModifier = {[0] = 0, [1] = 10000, [2] = 0, [3] = 0}},						--Luck range 1~1.99
		[2] = {nModifier = {[0] = 0, [1] = 0, [2] = 10000, [3] = 0}},						--Luck range 2~2.99
		[3] = {nModifier = {[0] = 0, [1] = 0, [2] = 0, [3] = 10000}},						--Luck range 3~3.99
		[4] = {nModifier = {[0] = 0, [1] = 5000, [2] = 5000, [3] = 0}},						--Luck range 4~4.99
		[5] = {nModifier = {[0] = 0, [1] = 0, [2] = 5000, [3] = 5000}},						--Luck range 5~5.99
		[6] = {nModifier = {[0] = 10000, [1] = 0, [2] = 0, [3] = 0}},						--Luck range 6+
	},
	
	--Black iron purity adjustment probabilities: [0] -2, [1] -1, [2] unchanged, [3] +1
	nEquipLevelupPurityModifier =
	{
		[0] = {nModifier = {[0] = 0, [1] = 0, [2] = 0, [3] = 0}},					  		--Purity range 0~9
		[1] = {nModifier = {[0] = 10000, [1] = 0, [2] = 0, [3] = 0}},					  	--Purity range 10~19
		[2] = {nModifier = {[0] = 0, [1] = 10000, [2] = 0, [3] = 0}},					  	--Purity range 20~29
		[3] = {nModifier = {[0] = 0, [1] = 0, [2] = 10000, [3] = 0}},					  	--Purity range 30~39
		[4] = {nModifier = {[0] = 0, [1] = 0, [2] = 0, [3] = 10000}},					  	--Purity range 40~49
		[5] = {nModifier = {[0] = 2000, [1] = 2000, [2] = 2000, [3] = 2000}},				--Purity range 50+
	},

	--- Divine item event
	nDiceTime = 60;
	--Monster spawn IDs for the divine item event
	nDiceNpcID =
	{
		[0] = 3500,
		[1] = 3501,
		[2] = 3502,
		[3] = 3503,
		[4] = 3504,
		[5] = 3505,
		[6] = 3506,
		[7] = 3507,
		[8] = 3508,
		[9] = 3509,
		[10] = 3510,
		[11] = 3511
	},
	--Monster spawn positions for the divine item event
	nDiceNpcPos = 
	{
		[0] = {nScene = 142, nX = 57600, nY = 47500, nZ = 5000, nRange = 0, nLiveTime = 3600},
		[1] = {nScene = 147, nX = 61300, nY = 56300, nZ = 5000, nRange = 0, nLiveTime = 3600},
		[2] = {nScene = 145, nX = 39300, nY = 42800, nZ = 5000, nRange = 0, nLiveTime = 3600},
		[3] = {nScene = 152, nX = 36500, nY = 48100, nZ = 5000, nRange = 0, nLiveTime = 3600},
		[4] = {nScene = 148, nX = 67100, nY = 20700, nZ = 5000, nRange = 0, nLiveTime = 3600},
		[5] = {nScene = 149, nX = 39100, nY = 39200, nZ = 5000, nRange = 0, nLiveTime = 3600}
	},
	--Gold cost to retrieve a weapon upgrade immediately
	nEquipLevelupImmediateCost = 100000,

	--- Threat decay time (never decays in dungeons)
    nThreatFadeTime = 30000,
    nRageRecoveryParam = 150;
    nFearDist = 1000;

    nAbilityPtsToPerProgressPts = 1,
    nBaseSecondsConvertingPerAbilityPts = 1800,

    nKnockBackDistance = 250;
    nKnockBackSpeed = 2000;

	-- NPC damage coefficient adjustment: x = x * nDmgToNpcParam1 / nDmgToNpcParam2
	nDmgToNpcParam1 = 6;
	nDmgToNpcParam2 = 5;


	-- Natural HP/MP recovery factors
	nHpRecoveryParam = 75;
	nMpRecoveryParam = 18;

	nHpTapWalk = -10;
	nMpTapWalk = 0;
	nHpTapRun = -60;
	nMpTapRun = -10;
	nHpTapPhyAtk = -30;
	nMpTapPhyAtk = -100;
	nHpTapMagAtk = 0;
	nMpTapMagAtk = -450;
	nHpTapDig = -30;
	nMpTapDig = -50;

	-- Potion HP/MP recovery factors
	nDrugCureParam1 = 10;
	nDrugCureParam2 = 5;

	nDrugCureTapRunning = -1;
	nDrugCureTapMining = -2;
	nDrugCureTapMin = 1;

	-- Spell HP/MP recovery factors
	nHpSpellCureMax = 300;
	nHpSpellCure = 5;

	-- Stagger parameters
    nStiffParamA = 500;
    nStiffParamB = -8000;
    nStiffIntervalB = 16; -- in frame

    nControlParamA = 80;
    nControlParamB = -2000;
    nControlIntervalB = 16;

	nHardParamA = 813;
	nHardParamB = 400;
	nHardParamC = 13;
	nHardParamD = 60;

    nGlobalSubstituteAuraID = 1001;

    nHeroExpGainRate = 10000;
    nHeroExpSubRate = 500;

    nAIIDWhenSeduced = 9993;

	nLastHurtResetTime = 7000;

	--- Reputation gained and maximum reputation for killing NPCs of each quality
    AddReputation =
    {
	    [1] = {		-- nqNormal1
		    nReputation = 1,
		    nMaxReputation = 100000,
	    },
	    [2] = {		--nqNormal2
		    nReputation = 1,
		    nMaxReputation = 100000,
	    },
	    [3] = {		--nqNormal3
		    nReputation = 1,
		    nMaxReputation = 100000,
	    },
	    [4] = {		--nqNormal4
		    nReputation = 1,
		    nMaxReputation = 100000,
	    },
	    [5] = {		--nqNormal5
		    nReputation = 1,
		    nMaxReputation = 100000,
	    },
	    [6] = {		--nqElite
		    nReputation = 10,
		    nMaxReputation = 100000,
	    },
	    [7] = {		--nqRare
		    nReputation = 20,
		    nMaxReputation = 100000,
	    },
	    [8] = {		--nqBoss
		    nReputation = 100,
		    nMaxReputation = 100000,
	    },
    },

	--- Reputation lost for killing NPCs of each quality
    SubReputation =
    {
	    [1] = -1,		--nqNormal1
	    [2] = -1,		--nqNormal2
	    [3] = -2,		--nqNormal3
	    [4] = -2,		--nqNormal4
	    [5] = -2,		--nqNormal5
	    [6] = -3,		--nqElite
	    [7] = -4,		--nqRare
	    [8] = -5,		--nqBoss
    },

    HeroAiId =
    {
	    [0] = {
		    AiId = {
			    [0] = 12,		--follow
			    [1] = 13,		--active
			    [2] = 14,		--passive
			    [3] = 1,		--standstill
		    },
	    },
	    [1] = {
		    AiId = {
			    [0] = 15,		--follow
			    [1] = 16,		--active
			    [2] = 17,		--passive
			    [3] = 1,		--standstill
		    },
	    },
	    [2] = {
		    AiId = {
			    [0] = 18,		--follow
			    [1] = 19,		--active
			    [2] = 20,		--passive
			    [3] = 1,		--standstill
		    },
	    },
	    [3] = {
		    AiId = {
			    [0] = 21,		--follow
			    [1] = 22,		--active
			    [2] = 23,		--passive
			    [3] = 1,		--standstill
		    },
	    },
    },

    HeroDefaultName =
    {
	    [0] = {
		    Sex =
		    {
			    [1] = { szName = '男战士' },
			    [2] = { szName = '女战士' },
		    },
	    },
	    [1] = {
		    Sex =
		    {
			    [1] = { szName = '男法师' },
			    [2] = { szName = '女法师' },
		    },
	    },
	    [2] = {
		    Sex =
		    {
			    [1] = { szName = '男刺客' },
			    [2] = { szName = '女刺客' },
		    },
	    },
	    [3] = {
		    Sex =
		    {
			    [1] = { szName = '男弓箭手' },
			    [2] = { szName = '女弓箭手' },
		    },
	    },

    },

    BaseWeaponSpeed =
    {
	    [0] = 10000,    -- Unarmed
	    [1] = 10000,    -- One-Hand Sword
	    [2] = 9000,     -- One-Hand Hammer
	    [3] = 10000,    -- Two-Hand Sword
	    [4] = 10000,    -- Great Axe
	    [5] = 8000,     -- Twin Sword
	    [6] = 12000,    -- Spear
	    [7] = 15000,    -- Glove
	    [8] = 10000,    -- Dagger
	    [9] = 12000,    -- Chain Blade
	    [10] = 10000,   -- Twin CrossBow
	    [11] = 20000,   -- CrossBow
	    [12] = 15000,   -- Bow
	    [13] = 8000,    -- Knife
	    [14] = 10000,   -- Staff
	    [15] = 10000,   -- Great Shield
	    [16] = 10000,   -- Small Shield
	    [17] = 10000,   -- Magic
    },

    BaseWeaponStiffFactor =
    {
	    [0] = 10000,    -- Unarmed
	    [1] = 10000,    -- One-Hand Sword
	    [2] = 9000,     -- One-Hand Hammer
	    [3] = 10000,    -- Two-Hand Sword
	    [4] = 10000,    -- Great Axe
	    [5] = 8000,     -- Twin Sword
	    [6] = 12000,    -- Spear
	    [7] = 15000,    -- Glove
	    [8] = 10000,    -- Dagger
	    [9] = 15000,    -- Chain Blade
	    [10] = 10000,   -- Twin CrossBow
	    [11] = 20000,   -- CrossBow
	    [12] = 15000,   -- Bow
	    [13] = 8000,    -- Knife
	    [14] = 10000,   -- Staff
	    [15] = 10000,   -- Great Shield
	    [16] = 10000,   -- Small Shield
	    [17] = 10000,   -- Magic
    },

    DodgeConst = 12000;

    CritConst =
    {
	    [1] = 12000,	-- phy attack
	    [2] = 12000,	-- mag attack
    },

    DefConst =
    {
	    [1] = 12000,	-- phy def
	    [2] = 12000,	-- mag def
    },

    ReviveParam =
    {
	    [1] = {		-- crtAtCurrentPos
		    nRequireItemID = 100001;
		    nRequireItemCount = 1,
--		    nRequireItemMaxCount = 20000,
--		    nDelayTime = 0,
		    nHpPercent = 10000,
	    },

	    [2] = {		-- crtInstant
		    nRequireItemID = MONEYTYPE_02,
		    nRequireItemCount = 100,
--		    nRequireItemMaxCount = 1,
--		    nDelayTime = 10000,
		    nHpPercent = 10000,
	    },

	    [3] = {		-- crtFree
		    nRequireItemID = 0,
		    nRequireItemCount = 0,
--		    nRequireItemMaxCount = 0,
--		    nDelayTime = 0,
		    nHpPercent = 3000,
	    },
    },

	-- Forced revival time
    nReviveTimeout = 5 * 60 * 1000;

    -------Revival delay: Min(nFreeReviveParam1, nFreeReviveParam2 x nKilledByPlayer)

	nReviveTimesResetTime = 180000;
	nFreeReviveParam1 = 0;
	nFreeReviveParam2 = 0;

    NormalPracticeParam =
    {
	    [1] = {
		    nSeconds = 1800,
		    nConvertedAbilityPts = 1,
	    },
	    [2] = {
		    nSeconds = 10800,
		    nConvertedAbilityPts = 6,
	    },
	    [3] = {
		    nSeconds = 86400,
		    nConvertedAbilityPts = 48,
	    },
    },

    nFastPracticeRate = 360,

    FastPracticeMoneyCost =
    {
	    [1] = 5,		--MONEYTYPE_01
	    [2] = 1,		--MONEYTYPE_02
    },

    FastPracticeSpeedupCost =
    {
	    [1] = {
		    nMoneyType = 1,
		    nMoneyCount = 100,
		    nRate = 3,
	    },
	    [2] = {
		    nMoneyType = 2,
		    nMoneyCount = 200,
		    nRate = 5,
	    },
	    [3] = {
		    nMoneyType = 2,
		    nMoneyCount = 300,
		    nRate = 10,
	    },
    },

	-- Array index represents frames already elapsed
	MagHitRateToRunningTarget =
	{
		[0] = 7500,
		[1] = 7500,
		[2] = 7500,
		[3] = 5000,
		[4] = 5000,
		[5] = 2500,
		[6] = 2500,
		[7] = 2500,
		[8] = 0,
		[9] = 0,
		[10] = 1000,
		[11] = 2500,
		[12] = 2500,
	},
	--Extra hit chance uses the Bonus check; account for the cat-step trait by reducing hit chance
	PhyHitRateToWalkingTarget =
	{
		[0] = 2500,
		[1] = 2000,
		[2] = 1500,
		[3] = 1500,
		[4] = 1000,
		[5] = 0,
		[6] = 0,
		[7] = 0,
		[8] = 0,
		[9] = 0,
		[10] = 0,
		[11] = 0,
		[12] = 0,
	},
	--Extra hit chance uses the Bonus check
	PhyHitRateToRunningTarget =
	{
		[0] = 7500,
		[1] = 5000,
		[2] = 4000,
		[3] = 3000,
		[4] = 1500,
		[5] = 1000,
		[6] = 1000,
		[7] = 0,
		[8] = 0,
		[9] = 0,
		[10] = 0,
		[11] = 0,
		[12] = 0,
	},

	--- Fallback return point
    nErrorReturnSceneTempID = 145 ,
	ErrorReturnPos = {x=31200, y=55200, z=5000},

	--- Red-name village
    nRedNameSceneTempID = 147,
	RedNameReturnPos = {x=16350, y=36587, z=6456},

	--- Default respawn point
    nDefaultReviveSceneTempID = 142 ,
	DefaultRevivePos = {x=25990, y=56905, z=4750},

    TeamDefaultName = '的队伍',

	--- VIP benefit configuration
    VIPData = {
        [vlLevel0] = { --- Non-VIP
            nDecreasePkValuePer5Min = 1,		--- Threat points that decay every 5 minutes
            nRewardQuestCount = 0,				--- Bounty quest count
            nDailyFatigue = 100,				--- Daily activity points gained
            nDailyUseMoneyType01Percent = 0,	--- Daily bound-gold spending limit bonus
        },
        [vlLevel1] = {
            nDecreasePkValuePer5Min = 2,
            nRewardQuestCount = 1,
            nDailyFatigue = 110,
            nDailyUseMoneyType01Percent = 3000,
        },
        [vlLevel2] = {
            nDecreasePkValuePer5Min = 3,
            nRewardQuestCount = 2,
            nDailyFatigue = 120,
            nDailyUseMoneyType01Percent = 6000,
        },
        [vlLevel3] = {
            nDecreasePkValuePer5Min = 5,
            nRewardQuestCount = 4,
            nDailyFatigue = 150,
            nDailyUseMoneyType01Percent = 10000,
        },
    },

	--- Per-player team experience reduction factor by team size
    nTeamExpRate =
    {
        [1] = 10000,
        [2] = 12000,
        [3] = 13000,
        [4] = 14000,
        [5] = 15000,
        [6] = 16000,
        [7] = 17000,
        [8] = 18000,
        [9] = 19000,
        [10] = 20000,
        [11] = 21000,
        [12] = 22000,
        [13] = 23000,
        [14] = 24000,
        [15] = 25000,
        [16] = 26000,
        [17] = 27000,
        [18] = 28000,
        [19] = 29000,
        [20] = 30000,
    },
	--- Experience bonus by level difference
    nLevelExpRate =
    {
        [0] = 5000,  --- NpcLevel - RoleLevel = -15
        [1] = 6000,  --- NpcLevel - RoleLevel = -14
        [2] = 7000,  --- NpcLevel - RoleLevel = -13
        [3] = 8000,  --- NpcLevel - RoleLevel = -12
        [4] = 9000,  --- NpcLevel - RoleLevel = -11
        [5] = 10000,  --- NpcLevel - RoleLevel = -10
        [6] = 10000,  --- NpcLevel - RoleLevel = -9
        [7] = 10000,  --- NpcLevel - RoleLevel = -8
        [8] = 10000,  --- NpcLevel - RoleLevel = -7
        [9] = 10000,  --- NpcLevel - RoleLevel = -6
        [10] = 10000,  --- NpcLevel - RoleLevel = -5
        [11] = 10000,  --- NpcLevel - RoleLevel = -4
        [12] = 10000,  --- NpcLevel - RoleLevel = -3
        [13] = 10000,  --- NpcLevel - RoleLevel = -2
        [14] = 10000,  --- NpcLevel - RoleLevel = -1
        [15] = 10000,  --- NpcLevel - RoleLevel = 0
        [16] = 10000,  --- NpcLevel - RoleLevel = 1
        [17] = 10000,  --- NpcLevel - RoleLevel = 2
        [18] = 10000,  --- NpcLevel - RoleLevel = 3
        [19] = 10000,  --- NpcLevel - RoleLevel = 4
        [20] = 10000,  --- NpcLevel - RoleLevel = 5
        [21] = 10000,  --- NpcLevel - RoleLevel = 6
        [22] = 10000,  --- NpcLevel - RoleLevel = 7
        [23] = 10000,  --- NpcLevel - RoleLevel = 8
        [24] = 10000,  --- NpcLevel - RoleLevel = 9
        [25] = 10000,  --- NpcLevel - RoleLevel = 10
        [26] = 10000,  --- NpcLevel - RoleLevel = 11
        [27] = 10000,  --- NpcLevel - RoleLevel = 12
        [28] = 10000,  --- NpcLevel - RoleLevel = 13
        [29] = 10000,  --- NpcLevel - RoleLevel = 14
        [30] = 10000,  --- NpcLevel - RoleLevel = 15
    },

	--- Mentorship experience bonus
	nTeacherExpRate = 1500,
	nStudentExpRate = 5000,

	--- Additional experience bonus
    nExtraExpRate = 10000,
	
	--- Global experience bonus
    nGlobalExpRate = 200000,  -- Default 10000
	
	--- Experience penalty rate
	nPunishExpRate = 5000,
	--- Experience penalty duration (seconds)
	nPunishExpDuration = 6000 * 60,
	----Characters above the level limit cannot gain experience from monsters
	nKillNpcGetExp = 60;

	--- Additional double-experience factor by level difference
	nLevelDiffExtraExpRate =
	{
		[0] = 10000,		[1] = 10000,		[2] = 10000,		[3] = 10000,		[4] = 10000,
		[5] = 15000,		[6] = 15000,		[7] = 15000,		[8] = 15000,		[9] = 15000,
		[10] = 20000,		[11] = 20000,		[12] = 20000,		[13] = 20000,		[14] = 20000,
		[15] = 20000,		[16] = 20000,		[17] = 20000,		[18] = 20000,		[19] = 20000,
		[20] = 20000,		[21] = 20000,		[22] = 20000,		[23] = 20000,		[24] = 20000,
		[25] = 20000,		[26] = 20000,		[27] = 20000,		[28] = 20000,		[29] = 20000,
		[30] = 20000,		[31] = 20000,		[32] = 20000,		[33] = 20000,		[34] = 20000,
		[35] = 20000,		[36] = 20000,		[37] = 20000,		[38] = 20000,		[39] = 20000,
		[40] = 20000,		[41] = 20000,		[42] = 20000,		[43] = 20000,		[44] = 20000,
		[45] = 20000,		[46] = 20000,		[47] = 20000,		[48] = 20000,		[49] = 20000,
		[50] = 20000,		[51] = 20000,		[52] = 20000,		[53] = 20000,		[54] = 20000,
		[55] = 20000,		[56] = 20000,		[57] = 20000,		[58] = 20000,		[59] = 20000,
		[60] = 20000,		[61] = 20000,		[62] = 20000,		[63] = 20000,		[64] = 20000,
		[65] = 20000,		[66] = 20000,		[67] = 20000,		[68] = 20000,		[69] = 20000,
		[70] = 20000,		[71] = 20000,		[72] = 20000,		[73] = 20000,		[74] = 20000,
		[75] = 20000,		[76] = 20000,		[77] = 20000,		[78] = 20000,		[79] = 20000,
		[80] = 20000,		[81] = 20000,		[82] = 20000,		[83] = 20000,		[84] = 20000,
		[85] = 20000,		[86] = 20000,		[87] = 20000,		[88] = 20000,		[89] = 20000,
		[90] = 20000,		[91] = 20000,		[92] = 20000,		[93] = 20000,		[94] = 20000,
		[95] = 20000,		[96] = 20000,		[97] = 20000,		[98] = 20000,		[99] = 20000
	},


	--- Item drop adjustment when attacking lower-level NPCs
    nDropChanceRate =
    {
        [0] = 10000,
        [1] = 10000,
        [2] = 10000,
        [3] = 10000,
        [4] = 10000,
        [5] = 10000,
        [6] = 9500,
        [7] = 9000,
        [8] = 8500,
        [9] = 8000,
        [10] = 7500,
        [11] = 7000,
        [12] = 6500,
        [13] = 6000,
        [14] = 5500,
        [15] = 5000,
        [16] = 4500,
        [17] = 4000,
        [18] = 4000,
        [19] = 4000,
        [20] = 4000,
        [21] = 4000,
        [22] = 4000,
        [23] = 4000,
        [24] = 4000,
        [25] = 4000,
        [26] = 4000,
        [27] = 4000,
        [28] = 4000,
        [29] = 4000,
	[30] = 4000,
        [31] = 4000,
        [32] = 4000,
        [33] = 4000,
        [34] = 4000,
        [35] = 4000,
        [36] = 4000,
        [37] = 4000,
        [38] = 4000,
        [39] = 4000,
	[40] = 4000,
        [41] = 4000,
        [42] = 4000,
        [43] = 4000,
        [44] = 4000,
        [45] = 4000,
        [46] = 4000,
        [47] = 4000,
        [48] = 4000,
        [49] = 4000,
	[50] = 4000,
        [51] = 4000,
        [52] = 4000,
        [53] = 4000,
        [54] = 4000,
        [55] = 4000,
        [56] = 4000,
        [57] = 4000,
        [58] = 4000,
        [59] = 4000,
	[60] = 4000,
        [61] = 4000,
        [62] = 4000,
        [63] = 4000,
        [64] = 4000,
        [65] = 4000,
        [66] = 4000,
        [67] = 4000,
        [68] = 4000,
        [69] = 4000,
	[70] = 4000,
        [71] = 4000,
        [72] = 4000,
        [73] = 4000,
        [74] = 4000,
        [75] = 4000,
        [76] = 4000,
        [77] = 4000,
        [78] = 4000,
        [79] = 4000,
	[80] = 4000,
        [81] = 4000,
        [82] = 4000,
        [83] = 4000,
        [84] = 4000,
        [85] = 4000,
        [86] = 4000,
        [87] = 4000,
        [88] = 4000,
        [89] = 4000,
    	[90] = 4000,
        [91] = 4000,
        [92] = 4000,
        [93] = 4000,
        [94] = 4000,
        [95] = 4000,
        [96] = 4000,
        [97] = 4000,
        [98] = 4000,
        [99] = 4000
    },
	--- Global item drop rate
    nGlobalDropRate = 100000,

	--- Free character level
	nFreeRoleLevel = 16,
	nFreeRoleTime = 86400,
	nFreeRoleSceneID = 143,
	nFreeRoleScenePosX = 148701184,
	nFreeRoleScenePosY = 76218368,
	nFreeRoleScenePosZ = 44777472,
	nFreeRoleSceneFaceDirection = 7168,
	nFreeModeCheckTimePeriod = 10,

	--
	-- Experience penalty for initiating mentorship termination, in ten-thousandths
	nMasterRewardExpPunishRate = 5000,
	-- Maximum level for recommending mentors to unmentored characters
	nMasterFindTeacherLevelLimit = 15,
	-- Mentor recommendation acceptance cooldown, seconds
    nMasterTwitterCD = 60,

	-- master
	nMasterLvlOneStudentNum = 3,
	nMasterLevelTwoStudentNum = 4,
	nMasterLevelThreeStudentNum = 5,

	nMasterLvlTwoExp = 2000000,
	nMasterLvlThreeExp = 5000000,

	nMasterGradGiftMailIDForS = 4060,
	nMasterGradGiftMailIDForT = 4059,

	-- Mentor point deduction for dismissing a student, in ten-thousandths
	nMasterFireStudentPercent = 10000000,

	--Mentor graduation experience factor
	nTeacherExpRewardRate = 900,
	--Student graduation experience factor
	nStudentExpRewardRate = 300,
	--Mentor graduation gold factor
	nTeacherExp2MoneyRate = 3333,
	--Student graduation gold factor
	nStudentExp2MoneyRate = 3333,
	--Leveling virtue point factor
	nMoralRate = 1000,
	--Graduation mentor-point factor
	nMasterExpRate = 1000,


	--- Dungeon rating parameters
	DungeonScore = {
	    nTimeScoreRate = { 0, 20000, 15000, 12000, 10000, 8000 },
	    nAttackScoreRate = { 0, 3000, 5000, 7000, 9000, 10000 },
	    nTotalAttackValue = { 0, 4, 6, 8, 10, 12 },
        nKillBossScore = 0,
        nKillEliteNpcScore = 0,
        nSwiftTimeScore = 0,
        nNeverDieScore = 0,
	    nGoalScoreDiff = { 999, 12, 9, 6, 3, 0 },
	    nTotalScore = { 0, 5, 8, 11, 14, 16 },
	},

	--- Debug options
	DebugOption = {
	    debugai = false,
	    bShowErrorDialog = false,
	    debugsp = false,
	    dropstat = false,
		anticheat = true,
		nRetryTimesOnTransferError = 3,
		gpkauth = true,
		logfps = false,
		loghd = false,
		swapfreelist = false,
		debugitem = false,
		minestat = false,
		npcstat = false,
		bActivityRewardEnabled =  true,
	},

	--- GPK system configuration
	GpkPunishOption = {
		-- Whether to enable the bypass system
		bEnableSidewayPunish = true,
		-- Whether to send heartbeat packets
		bEnableGpkKeepAlive = true,
		-- Whether to disconnect on heartbeat timeout
		bEnableKeepAliveTimeoutKick = false,
		-- Whether to disconnect on heartbeat error code 4
		bEnableKeepAliveReturn4Kick = false,
		-- Minimum number of players to arrest
		nCatchMin = 300,
		-- Maximum number of players to arrest
		nCatchMax = 700,
		-- Arrest queue length
		nCatchDelayQueueLen = 600,
		-- Arrest queue timeout (seconds)
		nCatchDelayTimeout = 600,
		-- Client GPK missed-report limit
		nClientGpkLostMax = 5,
		-- Client GPK missed-report penalty (gold-pool penalty level: 1, 2, 3)
		nClientGpkLostPunishType = 2,
		-- Client GPK disconnect limit
		nClientGpkKickMax = 2,
		-- Client GPK heartbeat interval (seconds)
		nKeepAliveInternal = 120,
		-- Whether to disconnect only in specific scenes
		bOnlyKickInSpecialScene = true,
	},


	stArena = {
		nWashoutTime = {
			[0] = 1,
			[1] = 9,
			[2] = 50,
			[3] = 120
		},
		nSystemFollow = 10000,
		nSystemBase = 15000000,
		nRoleConsumption = 2000000,
		nSceneTemplateID = 71,
		nSystemFollowCount = 20000000,
		nTotalRewardNum = 100,
		nGetRewardPercent = 9000,
		nRewardChatMsgID = 0,

		nArenaBeginHour = 20,
		nArenaBeginMin	= 0,
		nArenaBeginTime = 72000,

		nArenaEndHour = 20,
		nArenaEndMin = 40,
		nArenaEndTime = 74400,

		nArenaRewardBeginHour = 5,
		nArenaRewardBeginMin = 26,
		nArenaRewardBeginTime = 74400,

		nArenaRewardEndHour = 5,
		nArenaRewardEndMin = 27,
		nArenaRewardEndTime = 86400,

		nActivityLastTime = 2400,
	},

	-- Anti-cheat puzzle issuance interval (seconds)
	nAntiCheatInterval =
	{
		[0] = 15 * 60,
		[1] = 22 * 60,
		[2] = 30 * 60,
		[3] = 45 * 60,
		[4] = 60 * 60,
		[5] = 90 * 60,
	};

	-- Anti-cheat penalty duration
	nAntiCheatPunishTime =
	{
		[0] = 0,
		[1] = 15 * 60,
		[2] = 60 * 60,
		[3] = 180 * 60,
	};

	-- Determine puzzle issuance time for new characters
	nInitAntiCheatLevel = 5;
	-- Puzzle timeout (seconds)
	nAntiCheatTimeout = 600;
	-- Puzzle issuance interval variation rate
	nAntiCheatIntervalFloatPct = 2000;
	-- Whether to disconnect players
	nAntiCheatKickRole = 1;

--------------Sabac battle

	Sabac = {
		bEnabled = true,
		nSceneTempID = 152,
		OccupiedZoneLeftTop = {x=89665, y=49306, z=4355},
		OccupiedZoneRightBottom = {x=94727, y=49306, z=7935},
		WarZone1LeftTop = {x=1293, y=49236, z=3871},
		WarZone1RightBottom = {x=88125, y=49236, z=8524},
		WarZone2LeftTop = {x=89665, y=49306, z=4355},
		WarZone2RightBottom = {x=94727, y=49306, z=7935},


----Respawn point
		DefendRevivePos = {x=50152, y=47707, z=6373},
		nAttackReviveSceneID = 147,
		AttackRevivePos = {x=39442, y=55254, z=6612},
		nOtherReviveSceneID = 147,
		OtherRevivePos = {x=39442, y=55254, z=6612},

-----Start and end time
		nStartTime = 2000,    -- Starts at 20:00
		nEndTime   = 2300,    -- Ends at 23:00
		nStartOccupyTime = 2010,      -- Palace occupation becomes available at 20:10
		nErectStatusCost = 100000,    -- Statue
		nEnrollCost = 1000000,        -- Registration fee: 1,000,000 gold
		nEnrollItemID = 90196,        -- Registration item: Zuma portrait
		nEnrollItemCount = 1,         -- Registration item quantity

----Gold required to erect a statue
		nErectStatusCost = 100000,

----Victory rewards
	--	nLeaderAwardItemID = 0,
	--	nMemberAwardItemID = 0,

------Announcement messages
		nAwardMailTitleStringID = 2013,
		nAwardMailContentStringID = 2014,
	},
------ Survey
	Survey = {
		[0] = {nMaxLevel = 100, 	szSurveyPath = 'http://wenjuan.sdo.com:9091/survey/survey?surveyId=13834'},
		[1] = {nMaxLevel = 100, 	szSurveyPath = 'http://wenjuan.sdo.com:9091/survey/survey?surveyId=9439'},
		[2] = {nMaxLevel = 100, 	szSurveyPath = 'http://wenjuan.sdo.com:9091/survey/survey?surveyId=9441'},

	},

	nSceneBusyPercent = 6000,
	nSceneFullPercent = 8000,

----- Guild expiry time in seconds; minimum period is one day
	nWashoutDeletedGuildTime = 604800,
----- Mentorship expiry time in seconds; minimum period is one day
	nWashoutDeletedMasterTime = 604800,

	nTeleportRingCDTime = 10000,
	nProbeNecklaceCDTime = 10000,
	nMemoSuitCDTime = 180000,
	szRecallFailMessage = "无法对%s使用天地合一功能",

----- Mining
	nMineInterval = 1438,
	nMineMaxValue = 500000,
	nMineCDTime = 20 * 60 * 1000,
	nMineDropRate = 10000,  -- Currently unused

	ChatConfig = {
		nNormalMsgInterval = 1000,
        nGroupMsgInterval = 1000,
		nMaxDailyGroupMsg = 1000000,
		nTrumpetItemID = 2201,
		nGroupChatLevel = 7,
		nGroupChatCost = 1000;
		nGlobalChatAwardInterval = 2000;	-- Added chat violation reduction interval (milliseconds)
		nAutoForbidTime = 600;				-- Added automatic mute duration (seconds)
		nMaxFoulTimes = 10;					-- Added maximum violation count
		bCanChatInSafeArea = true;			-- Added setting for chat in safe areas
	},

	GpkPunishGoldPool = {
		[1]=-10000,
		[2]=-100000,
		[3]=-500000,
	},

	ArrestConfig = {
		nSceneTempID = 179,
		Pos = { x= 9500, y = 11000, z = 2100 },
		nDefaultDuration = 12 * 3600 * 1000,
	},

	-- pet,unit: sec
	nPetDisappearTime = 0,

	-- exp_bottle experience bottle
	nExpBottlePortion = 10000,  --Storage rate
	nExpBottleFactor = 4000,    --Storage rate after loss

	-- low level
	nRoleLowLevel = 17,

	-- server moneytype_02 limit: hourly
	nServerMoneytype02Limit = 2000000000,
	-- use rune weight 2
	nRuneTimesBeforeWeight2 = 10000,

	nGambleItemCountBeforeDropGroup2 = 180000,

	-- spell book gambole
	nSpellBookGambleCostItemID = 111003,
	nSpellBookGambleCostItemCount = 60,
	nSpellBookGambleDropGroupID = 30000,
	nSpellBookGambleDropGroupID2 = 30001,
	dwSpellBookGambleSuccessRate = 2000,
	nSpellBookGambleItemCountBeforeDropGroup2 = 36000,

	-- Daily friendship point limit
	nMaxIntimacyEveryday = 335,
	-- Maximum friendship points, not exceeding 65535
	nMaxIntimacy = 10000,
	-- Friendship gained by teammates in the same scene
	nTeamIntimacyValue = 2,
	-- Team friendship refresh interval, minimum 5 minutes
	nUpdateTeamIntimacyInterval = 5 * 60 * 1000,
	-- Friendship gained from chat
	nChatIntimacyValue = 1,
	-- Chat friendship refresh interval
	nUpdateChatIntimacyInterval = 12 * 60 * 1000,
	-- Friendship gained from guild activity
	nGuildIntimacyValue = 50,
	-- Guild friendship refresh interval, minimum 30 minutes
	nUpdateGuildIntimacyInterval = 1440 * 60 * 1000,
	-- Friendship required to trade
	nTradeNeedIntimacyValue = 1000,

--------Premium auction house

	--Whether auction house is open
	bAuctionOpen = true;
	--Auction transaction tax in ten-thousandths
	nAuctionTax = 200,
	--Tax on forfeited auction deposits in ten-thousandths
	nAuctionDepositTax = 1000,
	--Maximum auction listings
	nMaxAuctionSellingItems = 5,
	--Maximum bids
	nMaxAuctionBuyingItems = 5,
	--Maximum sold items
	nMaxAuctionSoldItems = 100,
	--Maximum purchased items
	nMaxAuctionBoughtItems = 100,
	--Three auction durations (hours)
	nAuctionTimeInterval =
	{
		[0] = 6,
		[1] = 12,
		[2] = 24
	},
	--Public auction cost
	nAuctionOpenCost = 1000000,
	--Cooldown for gold in mail (seconds)
	nAuctionMoneyCDTime = 86400,
	--Auction anti-sniping window (seconds)
	nAuctionFinishDelayTime = 120,
	--Auction balance payment deadline (hours)
	nAuctionWaitForPaymentTime = 24,

	--Whether to enable dynamic password verification
	bNeedDynamicPasswordVerification = false,
	--Dynamic password verification interval (seconds)
	nDynamicPasswordCheckInterval = 7200,

	--Line-change wait time
	nChangeLineDuration = 5 * 1000,

	----Client auction-house open button
	dwClientOption = 7,
	
	-------Safe-area idle rewards: add double experience
	ExtraExpPos1 = {
		nSceneTempID = 143,
		x = 62700,
		y = 48000,
		z = 43840,
		nRange = 128 * 20,
	},
	ExtraExpPos2 = {
		nSceneTempID = 147,
		x = 39100,
		y = 55300,
		z = 6576,
		nRange = 128 * 18,

	},
	
};

function convert(pos)
	pos.x, pos.y, pos.z = ConvertCoordO2N(pos.x, pos.y, pos.z);
end

function convert_rectangle(lt, rb)
	local p1 = { x = lt.x, y = lt.y, z = lt.z };
	local p2 = { x = lt.x, y = rb.y, z = lt.z };
	local p3 = { x = rb.x, y = rb.y, z = lt.z };
	local p4 = { x = rb.x, y = lt.y, z = lt.z };

	convert(p1);
	convert(p2);
	convert(p3);
	convert(p4);

	lt.x = p2.x;
	lt.y = p1.y;
	rb.x = p4.x;
	rb.y = p3.y;
end

convert(GameWorldConfig.ErrorReturnPos);
convert(GameWorldConfig.RedNameReturnPos);
convert(GameWorldConfig.DefaultRevivePos);

convert_rectangle(GameWorldConfig.Sabac.OccupiedZoneLeftTop, GameWorldConfig.Sabac.OccupiedZoneRightBottom);
convert_rectangle(GameWorldConfig.Sabac.WarZone1LeftTop, GameWorldConfig.Sabac.WarZone1RightBottom);
convert_rectangle(GameWorldConfig.Sabac.WarZone2LeftTop, GameWorldConfig.Sabac.WarZone2RightBottom);

convert(GameWorldConfig.Sabac.DefendRevivePos);
convert(GameWorldConfig.Sabac.AttackRevivePos);
convert(GameWorldConfig.Sabac.OtherRevivePos);

convert(GameWorldConfig.ArrestConfig.Pos);

convert(GameWorldConfig.ExtraExpPos1);
convert(GameWorldConfig.ExtraExpPos2);

Include("globalconfig/censored_word_list.lua");
Include("globalconfig/game_world_table.lua");
