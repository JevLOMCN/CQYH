scene_define = {
	owner='Yun Yu',
	SceneID = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Type = {			-- Scene type, up to 6 types; maximum estGuild
		type = 'number',
		default = 1,
		min = 0,
		max = 6,
	},
	Level = {
		type = 'number',
		default = 1,
		min = 0,
	},
	FinishTarget = {  
		type = 'link',
		default = 0,
		refertable = 'npc',
		referfield = 'TemplateID',
	},
	Reward1_group = {
		type = 'link',
		default = 0,
		refertable = 'drop_group',
		referfield = 'DropGroupID',
	},
	Reward2_group = {
		type = 'link',
		default = 0,
		refertable = 'drop_group',
		referfield = 'DropGroupID',
	},
	Reward3_group = {
		type = 'link',
		default = 0,
		refertable = 'drop_group',
		referfield = 'DropGroupID',
	},
	Reward1_cost = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Reward2_cost = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Reward3_cost = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Reward1_free = {
		type = 'link',
		default = 0,
		refertable = 'drop_group',
		referfield = 'DropGroupID',
	},
	Reward2_free = {
		type = 'link',
		default = 0,
		refertable = 'drop_group',
		referfield = 'DropGroupID',
	},
	Reward3_free = {
		type = 'link',
		default = 0,
		refertable = 'drop_group',
		referfield = 'DropGroupID',
	},
	ItemID = {
		type = 'link',
		default = 0,
		refertable = 'item',
		referfield = 'ID',
	},
	ItemIsAll = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemNumber = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Fatigue = {
		type = 'number',
		default = 0,
		min = 0,
	},
	StandardTime = {
		type = 'number',
		default = 0,
		min = 0,
	},
	StandardNpcValue = {
		type = 'number',
		default = 0,
		min = 0,
	},
	StandardDoodadValue = {
		type = 'number',
		default = 0,
		min = 0,
	},
	StandardAttackValue = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MaxRoleCount = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ConfigName = {
		type = 'string',
		default = "Default",
		maxlen = 256,
	},
	bDropItems = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	SceneStyle = {
		type = 'number',
		default = 0,
		min = 0,
	},
	BattleType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	BattleSideCount = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MinLimitPerSide = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MaxLimitPerSide = {
		type = 'number',
		default = 0,
		min = 0,
	},
	InitLineCount = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MinPlayerLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MaxPlayerLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Script = {
		type = 'string',
		default = "Default",
		maxlen = 256,
	},
	bPK = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	bReducedDurability = {
		type = 'number',
		default = 0,
		min = 0,
	},
	DropItemDurability = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MapName = {
		type = 'string',
		default = "Default",
		maxlen = 256,
	},
}