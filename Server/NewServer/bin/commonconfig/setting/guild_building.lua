guild_building_define = {
	owner='Chen Kai',
	ID = {
		type = 'number',
		default = 0,
		min = 1,
		max = 15,
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
		max = 6,
	},
	MaxLevel = {
		type = 'number',
		default = 0,
		min = 1,
	},
	MoneyConsumption = {  -- Money consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},
	HerbConsumption = { -- Herbs consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},
	WoodConsumption = {  -- Wood consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},
	CrystalConsumption = {  -- Crystals consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},
	SilverConsumption = {  -- Mithril consumed by upgrade
		type = 'number',
		default = 0,
		min = 0,
	},
	UpgradeTime = {	-- Upgrade time in seconds
		type = 'number',
		default = 0,
		min = 0,
	},
	RoutineConsumption = {	-- Recurring cost: guild funds consumed per hour
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
	UpgradeSpellID = {		-- Spell ID the building NPC casts on itself after upgrading
		type = 'link',
		default = 0,
		refertable = 'spell',
		referfield = 'ID',
	},
	Param1 = {		-- Designer-defined
		type = 'number',
		default = 0,
		min = 0,
	},
	Param2 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Param3 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Param4 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Effect = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Description = {
		type = 'string'
		default = "Default",
		maxlen = 256,
	},

}
