
GLOBAL_CONFIG["GameWorldConfig"] = {
	szConfigPath = "commonconfig",

	-- init data count
        nSceneCount = 4,
        nRegionCount = 512,

        nRoleCount = 1024,
        nNpcCount = 512,
        nDoodadCount = 128,
        nVolumeCount = 128,
        nDropCount = 128,
        nMissileCount = 1024,
	nTeamCount = 128,
	nArenaTeamCount = 128,
		nItemCount = 16384,	  
        nSignalCount = 512 * 1280,

        nSpellInstanceCount = 4096,
        nSpellSetCount = 4096,
        nAuraInstanceCount = 4096,
        nBulletCount = 4096,
        nRuneCount = 128,
        nExtraModifierNodeCount = 65536,
        nThreadNodeCount = 1024,
        nBTCtxCount = 128,
        nBTNodeCount = 1024 * 64,
        nAITemplateCount = 1024 * 16,
        
        --
        nForceSyncDistance = 512,
        nNpcInteractDistance = 512,
        nDoodadOperateDistance = 512,
        nLootDistance = 9600,
        nCompareAchievementDistance = 512,
	nAdjacentDataTimeLimit = 1000 * 4,
	nSceneLoadTimeOut = 300000,
        nSaveRoleCountPerSecond = 16,
	nSaveRoleMinTime = 60,
        nEnterSceneBuffID = 2230,
	nSceneFinishBuffID = 1040,

        nPkProtectLevel = 10,
        nCounterDuration = 30 * 8,
        nGainPkValueChance = 9000,
        nGainPkValue = 100,
        nRedNamePkValueLimit = 300,
        nThreatFadeTime = 6000,
        
        nRageRecoveryParam = 150;
		nFearDist = 1000;
        
		nAbilityPtsToPerProgressPts = 1,
		nBaseSecondsConvertingPerAbilityPts = 1800,
		
	nKnockBackDistance = 250;
	nKnockBackSpeed = 2000;
	nStiffParamA = 500;
	nStiffParamB = -8000;
	nStiffIntervalB = 16; -- in frame
		nControlParamA = 500;
		nControlParamB = -8000;
		nControlIntervalB = 16;
		
		nGlobalSubstituteAuraID = 1001;
	nHeroExpGainRate = 10000;
		nHeroExpSubRate = 500;
		
	nFollowDistanceX = 1000;
	nFollowDistanceY = 1500;
	nFollowDistanceZ = 2000;
		
	nFollowSpeedB = 1000;
	nFollowSpeedC = 1500;
		
	AddReputation = 
	{
		[1] = {		-- nqNormal
			nReputation = 1,
			nMaxReputation = 7000,
		},
		[2] = {		--nqMinion
			nReputation = 0,
			nMaxReputation = 0,
		},
		[3] = {		--nqChampion
			nReputation = 2,
			nMaxReputation = 14000,
		},
		[4] = {		--nqElite
			nReputation = 3,
			nMaxReputation = 14000,
		},
		[5] = {		--nqRare
			nReputation = 4,
			nMaxReputation = 14000,
		},
		[6] = {		--nqBoss
			nReputation = 5,
			nMaxReputation = 29000,
		},
	},
		
		SubReputation = 
		{
			[1] = -1,		--nqNormal
			[2] = -1,		--nqMinion
			[3] = -2,		--nqChampion
			[4] = -3,		--nqElite
			[5] = -4,		--nqRare
			[6] = -5,		--nqBoss
		},
		
		AffixNum = 
		{
			[1] = {		-- nqNormal
				nPrefixNum = 0,
				nPostfixNum = 0,
			},	
			[2] = {		-- nqMinion
				nPrefixNum = 0,
				nPostfixNum = 0, 
			},	
			[3] = {		-- nqChampion
				nPrefixNum = 1,
				nPostfixNum = 2,
			},	
			[4] = {		-- nqElite
				nPrefixNum = 0,
				nPostfixNum = 2,
			},	
			[5] = {		-- nqRare
				nPrefixNum = 1,
				nPostfixNum = 2,
			},		  
			[6] = {		-- nqBoss
				nPrefixNum = 1,
				nPostfixNum = 2, 
			},	
		},
		HeroAiId =
		{
			[0] = {
				AiId = {
					[0] = 12,		--follow
					[1] = 13,		--active
					[2] = 14,		--passive
					[3] = 11,		--standstill
				},
			},
			[1] = {
				AiId = {
					[0] = 15,		--follow
					[1] = 16,		--active
					[2] = 17,		--passive
					[3] = 11,		--standstill
				},
			},
			[2] = {
				AiId = {
					[0] = 18,		--follow
					[1] = 19,		--active
					[2] = 20,		--passive
					[3] = 11,		--standstill
				},
			},
			[3] = {
				AiId = {
					[0] = 21,		--follow
					[1] = 22,		--active
					[2] = 23,		--passive
					[3] = 11,		--standstill
				},
			},
		},
		
		HeroDefaultName =
		{
			[0] = {
				Sex = 
				{
					[1] = { szName = 'Male Warrior' },
					[2] = { szName = 'Female Warrior' },
				},
			},
			[1] = {
				Sex = 
				{
					[1] = { szName = 'Male Mage' },
					[2] = { szName = 'Female Mage' },
				},
			},
			[2] = {
				Sex = 
				{
					[1] = { szName = 'Male Assassin' },
					[2] = { szName = 'Female Assassin' },
				},
			},
			[3] = {
				Sex = 
				{
					[1] = { szName = 'Male Archer' },
					[2] = { szName = 'Female Archer' },
				},
			},
		
		},
		
		BaseWeaponSpeed =
		{
			[0] = 10000,    -- Unarmed
			[1] = 9000,     -- One-Hand Sword
			[2] = 9000,     -- One-Hand Hammer
			[3] = 10000,    -- Two-Hand Sword
			[4] = 10000,    -- Great Axe
			[5] = 8000,     -- Twin Sword
			[6] = 15000,    -- Spear
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
		
		BaseWeaponStiffFactor =
		{
			[0] = 10000,    -- Unarmed
			[1] = 9000,     -- One-Hand Sword
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
		DodgeConst = 10000;
		
		CritConst =
		{
			[1] = 10000,	-- phy attack
			[2] = 10000,	-- mag attack	
		},
		
		DefConst =
		{
			[1] = 10000,	-- phy def 
			[2] = 10000,	-- mag def
		},
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
		
		nErrorReturnSceneTempID = 100,
        nErrorReturnX = 37058,
        nErrorReturnY = 11959,
        nErrorReturnZ = 9757,
		
		ArenaConfig = 
		{
			[1] = {							--Role
				nKValue_First = 32,
				nFirst_Count = 30,
				nKValue_Second = 16,
				nScoreMatchArea = 100,
				nInitScore = 900,
			},
			[2] = {							--Team
				nKValue_First = 32,
				nFirst_Count = 30,
				nKValue_Second = 16,
				nScoreMatchArea = 100,
				nInitScore = 900,
			},
		},

		nWeeklyMoneyLimit = 
		{
			[0] = 0,		--Abyss gold
			[1] = 0,		--Gold
			[2] = 0,
			[3] = 0,
			[4] = 0,
			[5] = 0,
			[6] = 0,
			[7] = 0,
			[8] = 1000000,		--Battlefield points
			[9] = 0,
		},

		TeamDefaultName = ' Team',
}
