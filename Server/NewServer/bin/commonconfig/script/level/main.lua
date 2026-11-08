LoadPackage("role.pkg");
LoadPackage("char_def.pkg");
LoadPackage("spell_book.pkg");
LoadPackage("spell.pkg");
LoadPackage("character.pkg");
LoadPackage("chat_def.pkg");
LoadPackage("item_bag.pkg");

local SlotTable = {};
SlotTable[49] = qssKey0;
SlotTable[50] = qssKey1;
SlotTable[51] = qssKey2;
SlotTable[52] = qssKey3;
SlotTable[53] = qssKey4;
SlotTable[54] = qssKey5;
SlotTable[55] = qssKey6;
SlotTable[56] = qssKey7;
SlotTable[57] = qssKey8;
SlotTable[58] = qssKey9;
SlotTable[59] = qssKey10;
SlotTable[60] = qssKey11;

--! Spell acquisition table
local WarriorSkillGet = {};
WarriorSkillGet[41] = 1031; -- Basic Swordsmanship
WarriorSkillGet[42] = 1032; -- Attack Swordsmanship
WarriorSkillGet[43] = 1033; -- Assassination Swordsmanship
WarriorSkillGet[44] = 1034; -- Half-Moon Slash
WarriorSkillGet[45] = 1035; -- Savage Charge
WarriorSkillGet[46] = 1036; -- Blazing Swordsmanship
WarriorSkillGet[47] = 1037; -- Lion’s Roar
WarriorSkillGet[48] = 1038; -- Sun-Chasing Swordsmanship
WarriorSkillGet[49] = 0--1039; -- Whirlwind Slash
WarriorSkillGet[50] = 0--1040; -- Fury
WarriorSkillGet[51] = 0--1041; -- Shield Wall
WarriorSkillGet[52] = 0;
WarriorSkillGet[53] = 0;
WarriorSkillGet[54] = 0;
WarriorSkillGet[55] = 0;
WarriorSkillGet[56] = 0;
WarriorSkillGet[57] = 0;
WarriorSkillGet[58] = 0;
WarriorSkillGet[59] = 0;
WarriorSkillGet[60] = 0;

local MageSkillGet = {};
MageSkillGet[41] = 2531; -- Fireball
MageSkillGet[42] = 2532; -- Repelling Fire Ring
MageSkillGet[43] = 2541; -- Light of Seduction
MageSkillGet[44] = 2544; -- Hellfire
MageSkillGet[45] = 2533; -- Lightning
MageSkillGet[46] = 2538; -- Teleport
MageSkillGet[47] = 2545; -- Explosive Flame
MageSkillGet[48] = 2534; -- Firewall
MageSkillGet[49] = 2536; -- Lightning Beam
MageSkillGet[50] = 2546; -- Hell Lightning
MageSkillGet[51] = 2535; -- Magic Shield
MageSkillGet[52] = 2547; -- Holy Word
MageSkillGet[53] = 2537; -- Ice Roar
MageSkillGet[54] = 2550; -- Ice Palm
MageSkillGet[55] = 2549; -- Greater Fireball
MageSkillGet[56] = 2539; -- Skyfire
MageSkillGet[57] = 2540; -- Meteor Shower
MageSkillGet[58] = 0--2548; -- Moonlight
MageSkillGet[59] = 0--2542; -- Magic Devouring Shield
MageSkillGet[60] = 0--2543; -- Magic Overload

local TaoistSkillGet = {};
TaoistSkillGet[41] = 3002; -- Heal
TaoistSkillGet[42] = 3001; -- Spirit Combat Technique
TaoistSkillGet[43] = 3004; -- Poison
TaoistSkillGet[44] = 3005; -- Soul Fire Talisman
TaoistSkillGet[45] = 3003; -- Summon Undead
TaoistSkillGet[46] = 3009; -- Invisibility
TaoistSkillGet[47] = 3014; -- Group Invisibility
TaoistSkillGet[48] = 3006; -- Ghost Shield
TaoistSkillGet[49] = 3007; -- Holy Armor
TaoistSkillGet[50] = 3011; -- Demon Trap
TaoistSkillGet[51] = 3018; -- Qi Wave
TaoistSkillGet[52] = 3012; -- Group Heal
TaoistSkillGet[53] = 3008; -- Summon Divine Beast
TaoistSkillGet[54] = 3015; -- Boundless Qi
TaoistSkillGet[55] = 3010; -- Blood Drain
TaoistSkillGet[56] = 0--3016; -- Confinement
TaoistSkillGet[57] = 0--3017; -- Purification
TaoistSkillGet[58] = 0--3013; -- Body Protection
TaoistSkillGet[59] = 0;
TaoistSkillGet[60] = 0;

local RogueSkillGet = {};
RogueSkillGet[41] = 0;
RogueSkillGet[42] = 0;
RogueSkillGet[43] = 0;
RogueSkillGet[44] = 0;
RogueSkillGet[45] = 0;
RogueSkillGet[46] = 0;
RogueSkillGet[47] = 0;
RogueSkillGet[48] = 0;
RogueSkillGet[49] = 0;
RogueSkillGet[50] = 0;
RogueSkillGet[51] = 0;
RogueSkillGet[52] = 0;
RogueSkillGet[53] = 0;
RogueSkillGet[54] = 0;
RogueSkillGet[55] = 0;
RogueSkillGet[56] = 0;
RogueSkillGet[57] = 0;
RogueSkillGet[58] = 0;
RogueSkillGet[59] = 0;
RogueSkillGet[60] = 0;

local ArcherSkillGet = {};
ArcherSkillGet[41] = 0;
ArcherSkillGet[42] = 0;
ArcherSkillGet[43] = 0;
ArcherSkillGet[44] = 0;
ArcherSkillGet[45] = 0;
ArcherSkillGet[46] = 0;
ArcherSkillGet[47] = 0;
ArcherSkillGet[48] = 0;
ArcherSkillGet[49] = 0;
ArcherSkillGet[50] = 0;
ArcherSkillGet[51] = 0;
ArcherSkillGet[52] = 0;
ArcherSkillGet[53] = 0;
ArcherSkillGet[54] = 0;
ArcherSkillGet[55] = 0;
ArcherSkillGet[56] = 0;
ArcherSkillGet[57] = 0;
ArcherSkillGet[58] = 0;
ArcherSkillGet[59] = 0;
ArcherSkillGet[60] = 0;


local SkillTable = {WarriorSkillGet, MageSkillGet, RogueSkillGet, ArcherSkillGet, TaoistSkillGet};

--! Automatic equipment table
local WarriorSkillAutoEquip = {}
WarriorSkillAutoEquip[49] = 1033;
WarriorSkillAutoEquip[50] = 1034;
WarriorSkillAutoEquip[51] = 1035;
WarriorSkillAutoEquip[52] = 1036;
WarriorSkillAutoEquip[53] = 1037;
WarriorSkillAutoEquip[54] = 1038;
WarriorSkillAutoEquip[55] = 0;--1040;
WarriorSkillAutoEquip[56] = 0;--1039;
WarriorSkillAutoEquip[57] = 0;--1041;
WarriorSkillAutoEquip[58] = 0;
WarriorSkillAutoEquip[59] = 0;
WarriorSkillAutoEquip[60] = 0;

local MageSkillAutoEquip = {}
MageSkillAutoEquip[49] = 2531;
MageSkillAutoEquip[50] = 2532;
MageSkillAutoEquip[51] = 2541;
MageSkillAutoEquip[52] = 2544;
MageSkillAutoEquip[53] = 2533;
MageSkillAutoEquip[54] = 2538;
MageSkillAutoEquip[55] = 2545;
MageSkillAutoEquip[56] = 2534;
MageSkillAutoEquip[57] = 2536;
MageSkillAutoEquip[58] = 2546;
MageSkillAutoEquip[59] = 2535;
MageSkillAutoEquip[60] = 2547;

local TaoistSkillAutoEquip = {}
TaoistSkillAutoEquip[49] = 3002;
TaoistSkillAutoEquip[50] = 3004;
TaoistSkillAutoEquip[51] = 3005;
TaoistSkillAutoEquip[52] = 3003;
TaoistSkillAutoEquip[53] = 3009;
TaoistSkillAutoEquip[54] = 3014;
TaoistSkillAutoEquip[55] = 3006;
TaoistSkillAutoEquip[56] = 3007;
TaoistSkillAutoEquip[57] = 3011;
TaoistSkillAutoEquip[58] = 3018;
TaoistSkillAutoEquip[59] = 3012;
TaoistSkillAutoEquip[60] = 3008;

local RougeSkillAutoEquip = {}
RougeSkillAutoEquip[49] = 0;
RougeSkillAutoEquip[50] = 0;
RougeSkillAutoEquip[51] = 0;
RougeSkillAutoEquip[52] = 0;
RougeSkillAutoEquip[53] = 0;
RougeSkillAutoEquip[54] = 0;
RougeSkillAutoEquip[55] = 0;
RougeSkillAutoEquip[56] = 0;
RougeSkillAutoEquip[57] = 0;
RougeSkillAutoEquip[58] = 0;
RougeSkillAutoEquip[59] = 0;
RougeSkillAutoEquip[60] = 0;

local ArcherSkillAutoEquip = {}
ArcherSkillAutoEquip[49] = 0;
ArcherSkillAutoEquip[50] = 0;
ArcherSkillAutoEquip[51] = 0;
ArcherSkillAutoEquip[52] = 0;
ArcherSkillAutoEquip[53] = 0;
ArcherSkillAutoEquip[54] = 0;
ArcherSkillAutoEquip[55] = 0;
ArcherSkillAutoEquip[56] = 0;
ArcherSkillAutoEquip[57] = 0;
ArcherSkillAutoEquip[58] = 0;
ArcherSkillAutoEquip[59] = 0;
ArcherSkillAutoEquip[60] = 0;


local SkillEquipTable = {WarriorSkillAutoEquip, MageSkillAutoEquip, RougeSkillAutoEquip, ArcherSkillAutoEquip, TaoistSkillAutoEquip};


--! Spells of the classes added by the 1.05 backport (Rogue, Archer, Dragon Lancer, Sorcerer).
--! Auto-learned at the level required by the 1.05 spell.txt (RequireLevel, same as the 1.05 emulator's level-up learning).
--! Entries are {required level, spell id}; spells are learned at spell level 0 (upgraded through spell_progress.txt).
--! The beginner-book skills (1549, 1540, 1215, 1219, 2055, 2059, 3217, 3218) and sub-spells are not auto-learned.
local BackportSkillGet = {};
BackportSkillGet[clsRogue] = {
	{7, 1531}, {9, 1534}, {18, 1542}, {22, 1532}, {23, 1533}, {28, 1537}, {30, 1535}, {32, 1536},
	{35, 1538}, {40, 1546}, {41, 1541}, {45, 1547}, {47, 1539}, {50, 1548}, {51, 1545},
};
BackportSkillGet[clsArcher] = {
	{7, 2041}, {10, 2042}, {16, 2043}, {21, 2044}, {25, 2045}, {30, 2046}, {33, 2056},
	{35, 2048}, {40, 2049}, {41, 2050}, {45, 2057}, {47, 2051}, {50, 2058}, {51, 2052},
};
BackportSkillGet[clsSpear] = {
	{7, 1201}, {12, 1202}, {17, 1203}, {24, 1204}, {28, 1205}, {30, 1206}, {33, 1207}, {35, 1208},
	{37, 1209}, {40, 1213}, {41, 1210}, {45, 1214}, {47, 1211}, {50, 1218}, {51, 1212},
};
BackportSkillGet[clsSorcerer] = {
	{7, 3201}, {12, 3216}, {17, 3202}, {20, 3203}, {26, 3205}, {30, 3204}, {33, 3206}, {35, 3207},
	{37, 3208}, {40, 3213}, {41, 3209}, {43, 3210}, {45, 3214}, {47, 3211}, {50, 3215}, {51, 3212},
};

--! Learn every backport spell whose required level has been reached (already-known spells are ignored by LearnSpell)
local function LearnBackportSkills(oRole, new_level)
	local list = BackportSkillGet[oRole:GetClass()];
	if list == nil then
		return;
	end
	for i = 1, #list do
		if list[i][1] <= new_level then
			oRole:LearnSpell(list[i][2], 0, true);
		end
	end
end

--! Level-up logic
function main(oRole, old_level, new_level)

	LearnBackportSkills(oRole, new_level);

	--print("oldlevel: "..old_level..", newlevel: "..new_level);

	if new_level == 61 then

		local class = oRole:GetClass();

		if (new_level - old_level > 1) and class == clsTaoist then
			oRole:GetItemBag():AddItem(40004, 1, ITEMLOG_CREATE_ROLE, 0, false);
			oRole:GetItemBag():AddItem(40014, 1, ITEMLOG_CREATE_ROLE, 0, false);
			oRole:GetItemBag():AddItem(40103, 1, ITEMLOG_CREATE_ROLE, 0, false);
		end


		local classinskill = class + 1

		-- Level-up functionality
		for next_level = old_level + 1, new_level do

			-- Acquire spell
			if SkillTable[classinskill][next_level] ~= nil then
				if type(SkillTable[classinskill][next_level]) == "number" then
					if SkillTable[classinskill][next_level] ~= 0 then
						oRole:LearnSpell(SkillTable[classinskill][next_level], 0, true);
					end
				elseif type(SkillTable[classinskill][next_level]) == "table" then
					for i =1, #SkillTable[classinskill][next_level] do
						if SkillTable[classinskill][next_level][i] ~= 0 then
							oRole:LearnSpell(SkillTable[classinskill][next_level][i], 0, true);
						end
					end
				end
			end


			-- Automatically equip spells
			if SkillEquipTable[classinskill][next_level] ~= nil then

				if type(SkillEquipTable[classinskill][next_level]) == "number" then
					if type(SlotTable[next_level]) == "number" then
						oRole:EquipSpell(SlotTable[next_level], SkillEquipTable[classinskill][next_level]);
					elseif type(SlotTable[next_level]) == "table" then
						if type(SlotTable[next_level][1]) == "number" then
							oRole:EquipSpell(SlotTable[next_level][1], SkillEquipTable[classinskill][next_level]);
						end
					end
				elseif type(SkillEquipTable[classinskill][next_level]) == "table" then
					if type(SlotTable[next_level]) ~= "table" then
						print("slot num is not match skill equip");
					else
						local maxlength = #SkillEquipTable[classinskill][next_level];
						if maxlength < #SlotTable[next_level] then
							maxlength = #SlotTable[next_level]
						end
						for i = 1, maxlength do
							if type(SkillEquipTable[classinskill][next_level][i]) == "number" then
								oRole:EquipSpell(SlotTable[next_level][i], SkillEquipTable[classinskill][next_level][i]);
							end
						end
					end
				end
			end

		end
	end

---------Send mail based on player level

	--Level 17 VIP verification by xxp
		--[[if new_level>= 17 then
			if oRole:GetVIPLevel() < 1 then
				oRole:SetLevel(16)
			end
		end
				





	--Survey by xxp

		if new_level== 17 then
			if oRole:GetVIPLevel() < 1 then
				oRole:Say(oRole:GetObjID(), cltUrl, "Welcome to take survey:‘http://www.cqyh.info/toPayAccount.html?surveyId=9438’")
				end
		end]]--

end

