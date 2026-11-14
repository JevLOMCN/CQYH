quest_define = {
	owner='Yun Yu',
	Index = {				--Index
		type = 'number',
		default = 0,
		min = 0,
	},
	QuestID = {				--Quest ID
		type = 'number',
		default = 0,
		min = 0,
	},
	ResetType = {				-- Recurring reset type (weekly or daily); maximum qttTotal
		type = 'number',
		default = 0,
		min = 0,
		max = 3,
	},
	ResetTime = {				--Reset time (day of the week)
		type = 'number',
		default = 0,
		min = 0,
	},
	ScriptName = {				--Invalid
		type = 'string',
		default = "Default",
		maxlen = 256,
	},
	CanAbandon = {				--Whether abandonment is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CanShare = {				--Whether sharing is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CanPublish = {				--Unknown
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	}, 
	QuestType = {				--Quest type
		type = 'number',
		default = 0,
		min = 0,
	}, 
	MaxCompleteCount = {				--Maximum completion count
		type = 'number',
		default = 0,
		min = 0,
	},
	ClassLimit = {				--Quest class restriction
		type = 'number',
		default = 0,
		min = 0,
		max = 15,
	},
	SexLimit = {				-- Maximum cstTotal
		type = 'number',
		default = 0,
		min = 0,
		max = 4,
	},
	RelationLimit = {			-- Maximum qrtTotal
		type = 'number',
		default = 0,
		min = 0,
		max = 7,
	},
	PreQuestID1 = {				--Prerequisite quest ID 1
		type = 'link',
		default = 0,
		refertable = 'quest',
		referfield = 'QuestID',
	},
	PreQuestID2 = {				--Prerequisite quest ID 2
		type = 'link',
		default = 0,
		refertable = 'quest',
		referfield = 'QuestID',
	},
	StartNpcID = {				--Starting NPC
		type = 'link',
		default = 0,
		refertable = 'npc',
		referfield = 'TemplateID',
	},
	StartItemID = {				--Item requirement for quest acceptance
		type = 'item',
		default = 0,
	},
	StartZone = {				--Invalid
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	}, 
	MinLevelLimit = {				--Buff level restriction
		type = 'link',
		default = 1,
		refertable = 'level',
		referfield = 'Level',
	},
	MaxLevelLimit = {				--Maximum level restriction
		type = 'link',
		default = 99,
		refertable = 'level',
		referfield = 'Level',
	},
	BuffIDLimit = {				--Minimum level restriction
		type = 'link',
		default = 0,
		refertable = 'aura',
		referfield = 'ID',
	},
	BuffLevelLimit = {				--Buff level restriction
		type = 'number',
		default = 0,
		min = 0,
	}, 
	MoneyTypeLimit = {			-- MONEYTYPE_COUNT
		type = 'number',
		default = -1,
		min = -1,
		max = 4,
	},
	MoneyAmountLimit = {				--Currency amount restriction
		type = 'number',
		default = 0,
		min = 0,
	}, 
	ReputationForceID = {				--Reputation type restriction
		type = 'link',
		default = 0,
		invalidvalue = -1,
		refertable = 'reputation',
		referfield = 'ForceID',
	},
	ReputationMaxLimit = {				--
		type = 'number',
		default = 0,
		min = 0,
	},
	ReputationMinLimit = {
		type = 'number',
		default = 0,
		min = 0,
	},
	YearLimit = {
		type = 'number',
		default = -1,
		min = 0,
	},
	MonthLimit = {
		type = 'number',
		default = -1,
		min = -1,
		max = 11,
	},
	StartWeekday = {
		type = 'number',
		default = -1,
		min = -1,
		max = 6,
	},
	WeekdayDuration = {
		type = 'number',
		default = 0,
		min = 0,
		max = 6,
	},
	StartMonthday = {
		type = 'number',
		default = 0,
		min = 0,
		max = 30,
	},
	MonthdayDuration = {
		type = 'number',
		default = 0,
		min = -1,
	},
	AcceptStartTime = {
		type = 'number',
		default = 0,
		min = -1,
	},
	AcceptEndTime = {
		type = 'number',
		default = 0,
		min = -1,
	},
	AcceptStartHour = {
		type = 'number',
		default = 0,
		min = -1,
	},
	AcceptStartMin = {
		type = 'number',
		default = 0,
		min = -1,
	},
	AcceptEndHour = {
		type = 'number',
		default = 0,
		min = -1,
	},
	AcceptEndMin = {
		type = 'number',
		default = 0,
		min = -1,
	},
	ItemLimitID1 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemLimitCnt1 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemLimitID2 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemLimitCnt2 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemLimitID3 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemLimitCnt3 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemLimitID4 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemLimitCnt4 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CheckkillNpcMap1 = { 
		type = 'number',
	},
	CheckkillNpcMap2 = { 
		type = 'number',
	},
	CheckkillNpcMap3 = { 
		type = 'number',
	},
	CheckkillNpcMap4 = { 
		type = 'number',
	},
	CheckkillItemMap1 = { 
		type = 'number',
	},
	CheckkillItemMap2 = { 
		type = 'number',
	},
	CheckkillItemMap3 = { 
		type = 'number',
	},
	CheckkillItemMap4 = { 
		type = 'number',
	},
	CheckKillNpcNav1 = {
		type = 'string',
	},
	CheckKillNpcNav2 = {
		type = 'string',
	},
	CheckKillNpcNav3 = {
		type = 'string',
	},
	CheckKillNpcNav4 = {
		type = 'string',
	},
	CheckKillNpcFly1 = {				--Whether quest teleport is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CheckKillNpcFly2 = {				--Whether quest teleport is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CheckKillNpcFly3 = {				--Whether quest teleport is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CheckKillNpcFly4 = {				--Whether quest teleport is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CheckkillNpcMap1 = { 
		type = 'number',
	},
	CheckkillNpcMap2 = { 
		type = 'number',
	},
	CheckkillNpcMap3 = { 
		type = 'number',
	},
	CheckkillNpcMap4 = { 
		type = 'number',
	},
	CheckItemNav1 = {
		type = 'string',
	},
	CheckItemNav2 = {
		type = 'string',
	},
	CheckItemNav3 = {
		type = 'string',
	},
	CheckItemNav4 = {
		type = 'string',
	},
	CheckItemFly1 = {				--Whether quest teleport is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CheckItemFly2 = {				--Whether quest teleport is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CheckItemFly3 = {				--Whether quest teleport is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CheckItemFly4 = {				--Whether quest teleport is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	AutoFinish = {				--Whether abandonment is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	TalkNpcFly = {				--Whether abandonment is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	StartNpcFly = {				--Whether abandonment is allowed
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	StarLevel = { 
		type = 'number',
	},
	FinishItemID = { 
		type = 'number',
	},

	LimitVarID1 = {
		type = 'item',
	},
	LimitVarValue1 = {
		type = 'number',
		default = 0,
	},
	LimitVarID2 = {
		type = 'item',
	},
	LimitVarValue2 = {
		type = 'number',
		default = 0,
	},
	OfferMoneyType = {
		type = 'number',
		default = 0,
		min = -1,
	},
	OfferMoneyAmount = {
		type = 'number',
		default = 0,
		min = 0,
	},
	OfferBuffID = {
		type = 'link',
		default = 0,
		refertable = 'aura',
		referfield = 'ID',
	},
	OfferBuffLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
	OfferItemID = {
		type = 'item',
		default = 0,
	},
	OfferItemCnt = {
		type = 'number',
		default = 0,
		min = 0,
	},
	FinishNpcID = {
		type = 'link',
		default = 0,
		refertable = 'npc',
		referfield = 'TemplateID',
	},
	CheckBuffID = {
		type = 'link',
		default = 0,
		refertable = 'aura',
		referfield = 'ID',
	},
	CheckBuffLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CheckItemID1 = {
		type = 'item',
		default = 0,
	},
	CheckItemCnt1 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CheckItemID2 = {
		type = 'item',
		default = 0,
	},
	CheckItemCnt2 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CheckItemID3 = {
		type = 'item',
		default = 0,
	},
	CheckItemCnt3 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CheckItemID4 = {
		type = 'item',
		default = 0,
	},
	CheckItemCnt4 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	CheckKillNpcID1 = {
		type = 'link',
		refertable = 'npc',
		referfield = 'TemplateID',
	},
	CheckKillNpcCnt1 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CheckKillNpcID2 = {
		type = 'link',
		refertable = 'npc',
		referfield = 'TemplateID',
	},
	CheckKillNpcCnt2 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CheckKillNpcID3 = {
		type = 'link',
		refertable = 'npc',
		referfield = 'TemplateID',
	},
	CheckKillNpcCnt3 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CheckKillNpcID4 = {
		type = 'link',
		refertable = 'npc',
		referfield = 'TemplateID',
	},
	CheckKillNpcCnt4 = {
		type = 'number',
		default = 0,
		min = 0,
	},	
	CheckVarID1 = {
		type = 'number',
		default = 0,
		min = -1,
	},
	CheckVarValue1 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CheckVarID2 = {
		type = 'number',
		default = -1,
		min = -1,
	},
	CheckVarValue2 = {
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
    GuideType = {
		type = 'number',
		default = 0,
	},		
    AutoComplete = {
		type = 'number',
		default = 0,
	},	
    DestMapId1 = {
		type = 'number',
		default = 0,
	},	
    DestX1 = {
		type = 'number',
		default = 0,
	},	
    DestY1 = {
		type = 'number',
		default = 0,
	},		
    DestZ1 = {
		type = 'number',
		default = 0,
	},		
    DestMapId2 = {
		type = 'number',
		default = 0,
	},	
    DestX2 = {
		type = 'number',
		default = 0,
	},	
    DestY2 = {
		type = 'number',
		default = 0,
	},		
    DestZ2 = {
		type = 'number',
		default = 0,
	},		
    DestMapId3 = {
		type = 'number',
		default = 0,
	},	
    DestX3 = {
		type = 'number',
		default = 0,
	},	
    DestY3 = {
		type = 'number',
		default = 0,
	},		
    DestZ3 = {
		type = 'number',
		default = 0,
	},		
    DestMapId4 = {
		type = 'number',
		default = 0,
	},	
    DestX4 = {
		type = 'number',
		default = 0,
	},	
    DestY4 = {
		type = 'number',
		default = 0,
	},		
    DestZ4 = {
		type = 'number',
		default = 0,
	},		
    DestMapId5 = {
		type = 'number',
		default = 0,
	},	
    DestX5 = {
		type = 'number',
		default = 0,
	},	
    DestY5 = {
		type = 'number',
		default = 0,
	},		
    DestZ5 = {
		type = 'number',
		default = 0,
	},		
    DestMapId6 = {
		type = 'number',
		default = 0,
	},	
    DestX6 = {
		type = 'number',
		default = 0,
	},	
    DestY6 = {
		type = 'number',
		default = 0,
	},		
    DestZ6 = {
		type = 'number',
		default = 0,
	},		
    DestMapId7 = {
		type = 'number',
		default = 0,
	},	
    DestX7 = {
		type = 'number',
		default = 0,
	},	
    DestY7 = {
		type = 'number',
		default = 0,
	},		
    DestZ7 = {
		type = 'number',
		default = 0,
	},		
    DestMapId8 = {
		type = 'number',
		default = 0,
	},	
    DestX8 = {
		type = 'number',
		default = 0,
	},	
    DestY8 = {
		type = 'number',
		default = 0,
	},		
    DestZ8 = {
		type = 'number',
		default = 0,
	},	
	
	
}	