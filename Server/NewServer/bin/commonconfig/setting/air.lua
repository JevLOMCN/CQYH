
air_define = {
	owner='Zhang Jingxuan',
	ID = {		
		type = 'number',
		default = 0,
		min = 0,
	},
	Description = {		
		type = 'string',
	},
	Level = {		
		type = 'number',
		default = 0,
		min = 0,
	},
	Color = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Exp = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	NextLevelID = {
		type = 'link',
		default = 0,
		refertable = 'air',
		referfield = 'ID',
	},	
	AttributeKey1 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	AttributeType1 = {		
		type = 'string',
	},	
	Attribute1 = {
		type = 'number',
		default = 0,
		min = 0,
	},		
	AttributeKey2 = {		
		type = 'string',
	},		
	AttributeType2 = {		
		type = 'string',
	},	
	Attribute2 = {		
		type = 'string',
	},		
	ExchangeCount = {
		type = 'number',
		default = 0,
		min = 0,
	},	
}




