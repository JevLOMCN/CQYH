quest_define = {
	owner='Mei Liying',
	GroupID= {
		type = 'number',
		default = 0,
		min = 0,
	},
	
	AwardMoneyType = {
		type = 'number',
		default = -1,
		min = -1,
		max = 4,
	},
	AwardMoneyAmount = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AwardExp = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AwardBuffID = {
		type = 'link',
		default = 0,
		refertable = 'aura',
		referfield = 'ID',
	},
	AwardBuffLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AwardReputationForceID = {
		type = 'link',
		default = 0,
		invalidvalue = -1,
		refertable = 'reputation',
		referfield = 'ForceID',
	},
	AwardReputation = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AwardItemID1 = {
		type = 'item',
		default = 0,
	},
	AwardItemCnt1 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AwardItemID2 = {
		type = 'item',
		default = 0,
	},
	AwardItemCnt2 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AwardItemID3 = {
		type = 'item',
		default = 0,
	},
	AwardItemCnt3 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AwardItemID4 = {
		type = 'item',
		default = 0,
	},
	AwardItemCnt4 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID1 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt1 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID2 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt2 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	SelectableAwardItemID3 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt3 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	SelectableAwardItemID4 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt4 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID5 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt5 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID6 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt6 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID7 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt7 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID8 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt8 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID9 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt9 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID10 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt10  = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID11 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt11  = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID12 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt12 = {
		type = 'number',
		default = 0,
		min = 0,
	},	 
	SelectableAwardItemID13 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt13  = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID14 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt14  = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID15 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt15  = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	SelectableAwardItemID16 = {
		type = 'item',
		default = 0,
	},
	SelectableAwardItemCnt16  = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	Script = {
		type = 'string',
		default = "Default",
		maxlen = 256,
	},
	Name = {
		type = 'string',
	},
    BeforeAcceptScriptName = {
		type = 'string',
	},
    CheckKillNpcDesc1 = {
		type = 'string',
	},
    CheckKillNpcDesc2 = {
		type = 'string',
	},
    CheckKillNpcDesc3 = {
		type = 'string',
	},
    CheckKillNpcDesc4 = {
		type = 'string',
	},
    CheckItemDesc1 = {
		type = 'string',
	},
    CheckItemDesc2 = {
		type = 'string',
	},
    CheckItemDesc3 = {
		type = 'string',
	},
    CheckItemDesc4 = {
		type = 'string',
	},
    AfterCompleteScriptName = {
		type = 'string',
	},
    QuestDescript = {
		type = 'string',
	},
    QuestCompletedDescript = {
		type = 'string',
	},
    AcceptQuestDialog = {
		type = 'string',
	},
    PreSubmitQuestDialog = {
		type = 'string',
	},
    CompleteQuestDialog = {
		type = 'string',
	},
    DurationLimit = {
		type = 'number',
		default = 0,
		min = 0,
	},	
    AfterAcceptScriptName = {
		type = 'string',
	},
    BeforeCompleteScriptName = {
		type = 'string',
	},
    AwardTitle = {
		type = 'link',
		default = 0,
		refertable = 'title',
		referfield = 'ID',
		invalidvalue = 0,
	},
    AwardPts = {
		type = 'number',
		default = 0,
		min = 0,
	},	
    TalkNpcID = {
		type = 'link',
		default = 0,
		refertable = 'npc',
		referfield = 'TemplateID',
	},
    TalkVarID = {
		type = 'number',
		default = 0,
		min = -1,
	},	
    ExplorVarID = {
		type = 'number',
		default = 0,
		min = -1,
	},	
    QuestDiagSoundAccept = {
		type = 'string',
	},
    QuestDiagSoundComplete = {
		type = 'string',
	},
    QuestSoundAccept = {
		type = 'string',
	},
    QuestSoundSubmit = {
		type = 'string',
	},
    CheckVarType1 = {
		type = 'number',
		default = 0,
		min = -1,
		max = 1,
	},	
    CheckVarType2 = {
		type = 'number',
		default = 0,
		min = -1,
		max = 1,
	},	
	CheckVarDesc1 = {
		type = 'string',
	},
	CheckVarDesc2 = {
		type = 'string',
	},
	
	QuestLevel = {
		type = 'string',
	},	

    AbandonScriptName = {
		type = 'string',
	},
	
	ItemLimitDesc1 = { 
		type = 'string',
	},
	ItemLimitDesc2 = { 
		type = 'string',
	},
	ItemLimitDesc3 = { 
		type = 'string',
	},
	ItemLimitDesc4 = { 
		type = 'string',
	},
	LimitVarType1 = { 
		type = 'string',
	},
	LimitVarDesc1 = { 
		type = 'string',
	},
	LimitVarType2 = { 
		type = 'string',
	},
	LimitVarDesc2 = { 
		type = 'string',
	},
	OfferItemDesc = { 
		type = 'string',
	},
	AcceptQuestDesc = { 
		type = 'string',
	},
	StartNPCMap = { 
		type = 'number',
	},
	PackageFolder = { 
		type = 'string',
	},	
    CheckLevel = {
		type = 'number',
		default = 0,
	},		
    TalkNpcMap = {
		type = 'number',
		default = 0,
	},	
    CircleID = {
		type = 'number',
		default = 0,
	},		
    AwardGuildContribute = {
		type = 'number',
		default = 0,
	},		
    	
}	