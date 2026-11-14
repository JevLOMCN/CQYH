guild_welfare_define = {
	owner='Chen Kai',
	ID = {
		type = 'number',
		default = 0,
		min = 1,
	},
	Name = {
		type = 'string'
		default = "Default",
		maxlen = 256,
	},
	Level = {
		type = 'number',
		default = 0,
		min = 1,
	},
	AuraID = {			-- Purchased buff ID
		type = 'link',
		default = 0,
		refertable = 'aura',
		referfield = 'ID',
	},
	Kind = {  			-- Welfare buff group
		type = 'number',
		default = 0,
		min = 0,
	},
	MaxLevel = { 		--
		type = 'number',
		default = 0,
		min = 0,
	},
	UpgradeTime = {		-- Upgrade time in seconds
		type = 'number',
		default = 0,
		min = 0,
	},
	ConditionBuildingID = {	-- Prerequisite guild building ID for upgrade
		type = 'link',
		default = 0,
		refertable = 'guild_building',
		referfield = 'ID',
	},
	ConditionBuildingLvl = {	-- Prerequisite guild building level for upgrade
		type = 'link',
		default = 0,
		refertable = 'guild_building',
		referfield = 'Level',
	},
	PurchaseLevelRequirement = {  	-- Character level required to redeem the buff
		type = 'number',
		default = 0,
		min = 0,
	},
	PurchaseMoneyConsumption = {  -- Character money required to redeem the buff
		type = 'number',
		default = 0,
		min = 0,
	},
	PurchaseGuildCurrencyConsumption = {  -- Guild contribution required to redeem the buff
		type = 'number',
		default = 0,
		min = 0,
	},
	PurchaseLimit = {  			-- Redemption count limit
		type = 'number',
		default = 0,
		min = 0,
	},
	UpgradeMoneyConsumption = {  -- Money consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},
	UpgradeHerbConsumption = { -- Herbs consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},
	UpgradeWoodConsumption = {  -- Wood consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},
	UpgradeCrystalConsumption = {  -- Crystals consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},
	UpgradeSilverConsumption = {  -- Mithril consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},

}
