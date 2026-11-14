
---Global variables
spell_define = {
	owner='Zhang Jingxuan',
	ID = {	--SPELL_NODE (spell_node.txt) references only SPELL_VAR; SPELL_VAR (spell_var.txt) references only SPELL_ENTRY (spell.txt); SPELL_ENTRY references only RUNE_ENTRY (rune.txt); RUNE_ENTRY references...
		type = 'rule',
		rule = [[
			local tabconfig = CCheckData:GetInstance():GetDataByName("rune");
			local rowcount = CCheckData:GetInstance():GetTabRowCount(tabconfig);
			local id = CCheckData:GetInstance():GetNumber(tab, "ID", rownumber);
			local runeslot = CCheckData:GetInstance():GetNumber(tab, "RuneSlotID", rownumber);
			local canequip = CCheckData:GetInstance():GetNumber(tab, "CanEquip", rownumber);
			if runeslot == 0 or canequip == 0 then
				return true;
			end
			for row1 = 0, rowcount - 1 do
				if (id == CCheckData:GetInstance():GetNumber(tabconfig, "SpellID", row1)) and
					(runeslot == CCheckData:GetInstance():GetNumber(tabconfig, "SlotID", row1)) then
					return true;
				end
			end
			CCheckData:GetInstance():PrintWrn("Cannot find the rune config entry with id "..id.."Rune slots: "..runeslot.."'s Rune");
			return false]],
	},
	PackageFolder = {
		type = 'string',
		maxlen = 256,
		default = '',
	},
	RuneSlotID = {
		type = 'number',
		default = 0,
	},
	RequireLevel = {
		type = 'number',
		default = 0,
		min = 0,
		max = 100,
	},
	Index = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Class = {
		type = 'number',
		default = 0,
		min = 0,
	},
	NextSpell = {
		type = 'link',
		default = 0,
		refertable = "spell",
		referfield = "ID",
	},
	ResetTime	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CacheTime	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	WeaponTypeR	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	WeaponTypeL	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	NeedCache = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	CanEquip = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	TrigEvent= {
		type = 'number',
		default = 0,
		min = 0,
	},
	QTEType	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CastType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	JudgeType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AttackType	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	DamageType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	LockType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MinDistance	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MaxDistance	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	TolerantDistance = {
		type = 'number',
		default = 0,
		min = 0,
	},
	RangeType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	RangeParam1 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	RangeParam2 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	RangeParam3	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	IgnoreObstacle = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	UseWeaponRange = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MaxTargetCount = {
		type = 'number',
		default = 0,
		min = 0,
	},
	OffsetDirection	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	OffsetDistance = {
		type = 'number',
		default = 0,
	},
	TargetObjectType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	TargetRelationship = {
		type = 'number',
		default = 0,
		min = 0,
	},
	TargetRace = {
		type = 'number',
		default = 0,
		min = 0,
	},
	TargetSex = {
		type = 'number',
		default = 0,
		min = 0,
	},
	TargetDirection = {
		type = 'number',
		default = 0,
		min = 0,
	},
	TargetStance = {
		type = 'number',
		default = 0,
		min = 0,
	},
	SelfStance = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CostType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	InitialCostValue = {
		type = 'number',
		default = 0,
	},
	InitialCostValuePct = {
		type = 'number',
		default = 0,
		min = 0,
	},
	DurativeCostValue = {
		type = 'number',
		default = 0,
		min = 0,
	},
	DurativeCostValuePct = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ModifierCostValue = {
		type = 'number',
		default = 0,
	},
	ModifierCostValuePct = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemCostID = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ItemCostCount = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ConsumeItem = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CoolDownTriggerType = {
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
	PrepareType = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AdjustFaceDirection	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AffectedBySpeedParam = {
		type = 'number',
		default = 0,
		min = 0,
	},
	PrepareTimeMin = {
		type = 'number',
		default = 0,
		min = 0,
	},
	PrepareTimeMax = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AccumulateTimeMax = {
		type = 'number',
		default = 0,
		min = 0,
	},
	AccumulateSeg = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ChannelCount = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Interval = {
		type = 'number',
		default = 0,
		min = 0,
	},
	HardTime = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Power = {
		type = 'number',
		default = 0,
		min = 0,
	},
	HitLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
	MoveBreak = {
		type = 'number',
		default = 0,
		min = 0,
	},
	BreakMove = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CanMoveInPrepare = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CanCastOnMount = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CanMoveInHardTime = {
		type = 'number',
		default = 0,
		min = 0,
	},
	HitBreak = {
		type = 'number',
		default = 0,
		min = 0,
	},
	Type = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CancelLevel = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CancelHardTime = {
		type = 'number',
		default = 0,
		min = 0,
		max = 1,
	},
	HitTag = {
		type = 'number',
		default = 0,
		min = 0,
	};
	Speed = {
		type = 'number',
		default = 0,
		min = 0,
	},
	EffectPct = {
		type = 'number',
		default = 0,
		min = 0,
	},
	EffectValue = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ThreatPct = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ThreatValue = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ThreatMax = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CasterDeadLimit	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	TargetDeadLimit	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CasterLimitBits = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CasterCCLimitBits = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ExCastTime1	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ExCastTime2	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ExCastTime3	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ExCastTime4	 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ExCastTime5 = {
		type = 'number',
		default = 0,
		min = 0,
	},
	CancelStiff = {
		type = 'number',
		default = 0,
		min = 0,
	},
	KnockBackRate = {
		type = 'number',
		default = 0,
		min = 0,
	},
	BaseStiffTime = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ChainEffect = {
		type = 'number',
		default = 0,
		min = 0,
	},
	BindSlot = {
		type = 'number',
		default = 0,
		maxlen = 15,
	},

	Name = {
		type = 'string',
		default = "Default",
		maxlen = 256,
	},
	Describe = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	LearnDesc = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	EffectDesc = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	LevelUpDesc = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	OpenDesc = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	CloseDesc = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientName = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientDescribe = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientDisplayIcon = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientRotateToMousePoint = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientNormalAttack = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientUseHitEffectByWeaponBodyMatrialType = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientRotateSpeed = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientHitGroupInfo = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientShowOnUI = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientShowLeadUI = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientCanLMouseEquip = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientRequestCancelOnRelease = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
	ClientPickTarget = {
		type = 'string',
		default = "Default",
		maxlen = 1024,
	},
}
