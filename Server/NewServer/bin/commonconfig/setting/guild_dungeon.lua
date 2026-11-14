guild_dungeon_define = {
	owner='Chen Kai',
	SceneTemplateID = {
		type = 'link',
		default = 0,
		refertable = 'scene',
		referfield = 'SceneID',
	},
	ConditionBuildingLvl = {	-- Guild war hall level required to open the dungeon
		type = 'link',
		default = 0,
		refertable = 'guild_building',
		referfield = 'Level',
	},
	ConditionMoney = {	-- Gold required
		type = 'number',
		default = 0,
		min = 1,
		max = 6,
	},
	PosX = {			-- Dungeon entry X coordinate
		type = 'number',
		default = 0,
		min = 1,
	},
	PosY = {  			-- Dungeon entry Y coordinate
		type = 'number',
		default = 0,
		min = 0,
	},
	PosZ = { 			-- Dungeon entry Z coordinate
		type = 'number',
		default = 0,
		min = 0,
	},
	Direction = {  		-- Dungeon entry facing angle
		type = 'number',
		default = 0,
		min = 0,
	},

	for i = 1,8 then		-- Drops
		guild_dungeon_define['DropChance'..i] = {
			type = 'number',
			default = 0,
			min = 0,
		};
		guild_dungeon_define['DropGroupID'..i] = {
			type = 'number',
			default = 0,
			min = 0,
		};
	end

	BestTeammateNum = {  		-- Recommended dungeon player count
		type = 'number',
		default = 0,
		min = 0,
	},

	for i = 1,8 then		-- Drops
		guild_dungeon_define['Award'..i] = {
			type = 'number',
			default = 0,
			min = 0,
		};
	end

}
