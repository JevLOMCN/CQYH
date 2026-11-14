
item_define = {
	owner='Tan Yong',
	ID = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Type = {								-- 	Item type, up to 11 types; ITEMTYPE_TOTAL
		type = 'number',
		default = 0,
		min = 0,
		max = 11,
	},
	SubType = {								--  Item subtype, up to 15 types; maximum ITEMSUBTYPE_EQUIP_AVATAR
		type = 'number',
		default = 0,
		min = 0,
		max = 15,
	},
	RequireLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
	RequireClass = {
		type = 'number',
		default = 0,
		min = 0,
		max = 15,
	},
	RequireGender = {						--  Sex, up to 3 values; maximum cstTotal
		type = 'number',
		default = 0,
		min = 0,
		max = 3,
	},
	MaxEquip = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MaxOwn = {
		type = 'number',
		default = 0,
		min = 0,
		max = 99999,
	},
	Color = {								--  Color, up to 7 values; maximum ITEMCOLOR_TOTAL
		type = 'number',
		default = 0,
		min = 0,
		max = 7,
	},
	BindType = {
		type = 'number',
		default = 0,
		min = 0,
		max = 10,
	},
	DurType = {								-- Durability type, up to 3 values; maximum DURTYPE_USE
		type = 'number',
		default = 0,
		min = 0,
		max = 3,
	},
	Dur = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Price = {
		type = 'number',
		default = 0,
		min = 0,
	},
	SpellID = {
		type = 'link',
		default = 0,
		refertable = 'spell_var',
		referfield = 'ID',
	},
	SpellLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Script = {
		type = 'string',
		default = "Default",
		maxlen = 256,
	},
	Index = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Name = {
		type = 'string',
		default = "Default",
		maxlen = 256,
	},
	UsedStatus = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Drop = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Storage = {
		type = 'number',
		default = 0,
		min = 0,
	},
	SelfCoolDown = {
		type = 'number',
		default = 0,
		min = 0,
	},
	GroupCoolDown = {
		type = 'number',
		default = 0,
		min = 0,
	},
	DisplayIcon = {
		type = 'string',
	},
	Description = {
		type = 'string',
	},
	DropMesh = {
		type = 'string',
	},
	SoundMaterialType = {
		type = 'string',
	},
	SoundActionType = {
		type = 'string',
	},
	ClothNoiseType = {
		type = 'string',
	},
	ItemLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
}