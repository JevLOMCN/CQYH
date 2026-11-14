
spell_var_define = {
	owner='Zhang Jingxuan',
	ID = {	--SPELL_NODE (spell_node.txt) references only SPELL_VAR; SPELL_VAR (spell_var.txt) references only SPELL_ENTRY (spell.txt); SPELL_ENTRY references only RUNE_ENTRY (rune.txt); RUNE_ENTRY references...
		type = 'rule',
		rule = [[
			local tabconfig = CCheckData:GetInstance():GetDataByName("spell");
			local rowcount = CCheckData:GetInstance():GetTabRowCount(tabconfig);
			local id = CCheckData:GetInstance():GetNumber(tab, "ID", rownumber);
			local runeslot = CCheckData:GetInstance():GetNumber(tab, "RuneSlotID", rownumber);
			if runeslot == 0 then
				return true;
			end
			for row1 = 0, rowcount - 1 do
				if (id == CCheckData:GetInstance():GetNumber(tabconfig, "ID", row1)) and
					(runeslot == CCheckData:GetInstance():GetNumber(tabconfig, "RuneSlotID", row1)) then
					return true;
				end
			end
			CCheckData:GetInstance():PrintWrn("fail to find id = "..id.." and runeslot = "..runeslot.." in rune.txt");
			return true;
		]],
	},
	ID = {
		type = 'number',
		default = 0,
	},
	RuneSlotID = {
		type = 'number',
		default = 0,
	},
	Level = {
		type = 'number',
		default = 0,
		min = 0,
		max = 100,
	},
	Variable1 = {
		type = 'number',
		default = 0,
	},
	Value1 = {
		type = 'number',
		default = 0,
	},
	Variable2 = {
		type = 'number',
		default = 0,
	},
	Value2 = {
		type = 'number',
		default = 0,
	},
	Variable3 = {
		type = 'number',
		default = 0,
	},
	Value3 = {
		type = 'number',
		default = 0,
	},
	Variable4 = {
		type = 'number',
		default = 0,
	},
	Value4 = {
		type = 'number',
		default = 0,
	},
	DetailDescription = {
		type = 'string',
		default = '',
	},
}
