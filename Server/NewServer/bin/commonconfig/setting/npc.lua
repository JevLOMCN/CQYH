
npc_define = {
	owner='Yun Yu',
	TemplateID = {
		type = 'number',
		default = 0,
		min = 0,
	},
	DisplayName = {
		type = 'string',
		default = "DefaultNpc",
		maxlen = 256,
	},
	PatternID = {
		type = 'link',
		default = 0,
		refertable = 'npc_pattern',
		referfield = 'PatternID',
	},
	Radius = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ForceID = {
		type = 'link',
		default = 0,
		refertable = 'reputation',
		referfield = 'ForceID',
	},
	ReduceReputation = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	RivalForceID = {
		type = 'number',
		default = 0,
		min = 0,
	},
	SpawnTime = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ReviveTime = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CorpseTime = {
		type = 'number',
		default = 0,
		min = 0,
	},
	RespawnTime = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AIID = {
		type = 'link',
		default = 0,
		refertable = 'ai',
		referfield = 'AIID',
	},
	ImmunePassiveMove = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	Invincible = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	AllwaysRun = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	ShopID = {
		type = 'link',
		default = 0,
		refertable = 'shop',
		referfield = 'ID',
	},
	Index = {
		type = 'number',
		default = 0,
		min = 0,
	},
	NameID = {
		type = 'number',
		default = 0,
		min = 0,
	},
	NPCScriptName = {
		type = 'string',
		default = "DefaultNpc",
		maxlen = 256,
	},
	SpeciesType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CanbeThrown = {
		type = 'number',
		default = 0,
		min = 0,
	},
	
	DefaultDialogText = {
		type = 'string',
	},
	IdentityName = {
		type = 'string',
	},

	IdentityIcon = {
		type = 'string',
	},

	NPCIcon = {
		type = 'string',
	},

	HUDDisplayIcon = {
		type = 'string',
	},

	ActorName = {
		type = 'string',
	},

	IsEquipEnhance = {
		type = 'string',
	},

	IsPVP = {
		type = 'string',
	},

	IsMerchant = {
		type = 'string',
	},

	IsWarehouse = {
		type = 'string',
	},

	ActorType = {
		type = 'string',
	},

	FaceToActorWhenInteract = {
		type = 'string',
	},

	AvatarTemplate = {
		type = 'string',
	},	
	
	PackageFolder = {
		type = 'string',
	},	
	Quality = {
		type = 'number',
		min = 0,
		max = 10,
	},	
	
}
