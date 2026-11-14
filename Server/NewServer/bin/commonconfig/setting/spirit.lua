spirit_define = {
	owner='Zhang Jingxuan',
	Level = {
		type = 'number',
		default = 0,
	},	
	Experience = {
		type = 'number',
		default = 0,
	},		
	DailyChangePtsTime = {
		type = 'number',
		default = 0,
	},	
	SpiritUpItem = {
		type = 'item',
		default = 0,
	},
	SpiritUpLevel = {
		type = 'link',
		default = 0,
		refertable = 'level',
		referfield = 'Level',
	},
	SpiritUpQuestID = {
		type = 'link',
		default = 0,
		refertable = 'quest',
		referfield = 'QuestID',
	},
	MaxPtsByMonster = {
		type = 'number',
		default = 0,
	},
	MaxPtsByMoney = {
		type = 'number',
		default = 0,
	},	
	MaxPtsByQuest = {
		type = 'number',
		default = 0,
	},	
	Exp2PtsCDTime = {
		type = 'number',
		default = 0,
	},	
	ClearCDMoneyCount = {
		type = 'number',
		default = 0,
	},	
}








