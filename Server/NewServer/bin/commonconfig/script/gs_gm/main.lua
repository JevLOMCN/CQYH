LoadPackage("gameworld.pkg");
LoadPackage("scene.pkg");
LoadPackage("character.pkg");
LoadPackage("role.pkg");
LoadPackage("npc.pkg");
LoadPackage("item.pkg");
LoadPackage("item_bag.pkg");
LoadPackage("teleport.pkg");
LoadPackage("attr_list.pkg");
LoadPackage("attribute_def.pkg");
LoadPackage("ai.pkg")
LoadPackage("char_def.pkg")
LoadPackage("math_ex.pkg")
LoadPackage("quest_list.pkg")
LoadPackage("title.pkg")
LoadPackage("master.pkg")
LoadPackage("data_manager.pkg")
LoadPackage("achievement_list.pkg")
LoadPackage("friend.pkg")
LoadPackage("chat_def.pkg")
LoadPackage("hero.pkg")
LoadPackage("money_def.pkg")
LoadPackage("master.pkg")
LoadPackage("string_table.pkg")
LoadPackage("spell.pkg")
LoadPackage("spell_book.pkg")
LoadPackage("id_def.pkg")
LoadPackage("doodad.pkg")
LoadPackage("spirit.pkg")
LoadPackage("spirit_def.pkg")
LoadPackage("global_config.pkg")
LoadPackage("math_ex.pkg")
LoadPackage("gamble.pkg")

Include("script/include/jump_table.lh")
Include("script/stringtable/stringtable.lh")
Include("script/gs_gm/data_config.lh")

g_oGameWorld = CGameWorld:GetInstance();
g_oSceneFileMgr = CSceneFileMgr:GetInstance();

TransformSpellIDList =
{
5100,5375,5190,
5250,5451,5452,
5454,5453,5186,
5115,5232,
5239,5233,
5650,5652,5653,5692,5651,
5236,5237,5238,
5371,
5469,5470,5471,
5108,5533,5465,
5114,5112,5303,5304,
5116,5117,5456,
5521,5528,5526,5524,5525,
5522,5527,5529,5523,
5376,5378,5379,5381,5377,5380,
5500,5507,5501,5504,5505,5503,5502,5506,5508,5509,
5390,5391,5392,
3575,5420,5421,
5422,5423,3574,
5459,5460,5461,5462,5463,5464,5141,
5483,5484,5485,
5486,5487,5488,
5393,5395,5394,
5808,5809,5810,5811,5812,5813,5814,5815,5816,
5166,5280,5424,
5290,5293,5291,
5386,5384,
5474,5475,
5477,5476,5419,
5478,5479,5480,5481,5482,
5382,5385,5388,5389,5387,
5319,5317,5318,5320,5455,5833,5315,5316,5834,5835,
5209,5472,5473,
5187,5397,5398,
3539,3540,5411,3730,3541,
3542,3543,5412,3731,3544,
5590,5589,5582,5795,5585,5796,5580,5797,
5413,5414,5415,5416,5417,5418,
5705,5710,5708,5706,5707,5712,5709,5711,
5120,5140,5351,
5169,5170,5265,
5491,5492,5493,
5157,5158,5276,5494,
5495,5496,5497,5498,5499,
5102,5457,5511,5512,5425,
5878,5825,5826,5827,5828,5829,5830,5831,5832,
5202,5203,
5138,
5269,5270,5396,
5466,5467,5468,
5289,5294,5292,5288,
5271,5273,
5713,5714,5715,5716,5724,5722,5458,5741,
5198,5200,5199,
5191,5192,5193,5205,
3518,3519,3530,3520,3521,3522,
3523,3524,5196,5235,3525,
5125,5136,5137,5350,
5630,5635,5636,5634,5632,
5640,5641,5644,5645,5642,5643,
5436,5437,5195,5443,
5438,5439,5440,5441,5442,
5857,5591,5593,5858,5859,5860,5595,5594,5862,5861,
5321,5231,5432,
5324,5323,5436,
5197,5592,5194,
3512,3514,5322,
5848,5849,5850,5851,5852,5853,5854,5855,5856,
5260,5261,5262,
3501,3502,3503,3504,
5325,3515,3516,
5869,5870,5871,5872,5873,5874,5877,5876,5875,
5255,5257,
5183,5184,5186,
3506,3507,3509,3508,
5430,5431,5433,5434,
5836,5837,5840,5841,5839,5842,5843,5844,5838,5847,5867,5846,5845,5868,
5329,5330,5352,
5216,5624,5219,3517,
5173,5174,5175,
3535,5189,3536,3537,3538,
5206,5182,3528,3527,3526,3529,
5660,5661,5662,5664,5666,5663,5665,
5428,5429,
5222,5223,3511,
5177,5178,3547,
5144,5145,3548,
5693,
5560,5566,5564,5561,5562,5563,5571,5565,
5104,5489,5490,
5113,5306,
5427,
5100,5139,3560,
5119,5248,5118,5353,5426,
5732,5733,5734,5735,5736,5737,5738,5739,5740,
6033,6034,6035,
5364,5365,3615,
5366,5370,
5367,5368,5369,
5784,5787,5786,5785,5788,
3608,3609,5371,3610,
3614,3606,3607,
3611,3612,3613,
5372,5373,5371,5374,
3531,
5149,5154,3533,
5150,5168,3534,
5879,5584,5581,5580,5586,5587,5589,5583,5900,
3565,3566,
3567,3568,3569,
3570,3571,3572,3573,
3622,3623,3665,
3627,3624,3629,3630,
3625,3626,
3631,3632,3633,3634,3635,
3616,3618,3617,
3619,3620,
5890,5891,5892,5893,5894,5895,5896,
3621,
3670,3671,3672,3673,
3674,
3675,3676,3677,
5817,5818,5819,5820,5821,5822,5823,5824,
3721,3722,3723,3724,3725,
3678,3679,3680,3681,
3682,3683,3684,3685,3686,
3687,3688,3689,
3690,3691,3692,3693,3694,
3695,3696,3697,3698,3699,3700,3701,3702,
3703,3704,3705,3706,3707,3708,
3709,3710,3711,3712,3713,3714,
5916,5917,5918,5919,5920,5921,5922,5923,5924,
3753,3754,3755,
3756,3757,3758,3759,3760,
3761,3762,3763,3764,3765,
3766,3767,3768,3773,3769,3770,3771,3772,
5910,5911,5912,5913,5914,5915,
3726,3727,3728,3729,
3732,3733,3734,3735,
3736,3737,3738,3739,3740,3741,3742,
3743,3744,3745,3746,3747,3748,
3631,3632,3633,3634,3635,
3651,3652,3653,3654,3655,3656,3657,3658,3659,
3636,3637,3638,3639,3640,3641,
3642,3643,3644,3645,3646,3647,3648,3649,3650,
5926,5927,5928,5929,5930,5931,5932,
5670,5671,5684,5676,5883,5882,5674,5675,5672,5673,
5789,5790,5791,5792,5793,5794
}

TheTransformMonsterIDList =
{
{"smyl", 300, 300, 600},
{"smzs", 301, 300, 500},
{"smgs", 302, 300, 500},
{"smzssl", 303, 300, 500},
{"dmz", 304, 300, 500},
{"pxsmjs", 305, 300, 550},
{"tmsmjs", 306, 300, 410},
{"jdsmjs", 307, 300, 450},
{"jssl", 308, 300, 410},
{"xyjssl",309, 300, 650},
{"zw", 310, 300, 450},
{"dysw",311, 300, 400},
{"sjc",312, 300, 450},
{"hfcx",313, 300, 0},
{"hf", 314, 300, 500},
{"zdc", 315, 300, 450},
{"mccs", 316, 300, 600},
{"fccs", 317, 300, 450},
{"xysjc", 318, 300, 450},
{"ccssl", 319, 300, 500},
{"mhgbl", 320, 300, 400},
{"xsll", 321, 300, 500},
{"zbm", 322, 300, 700},
{"bf", 323, 300, 500},
{"bfcx", 324, 300, 0},
{"xyzbm", 325, 300, 500},
{"xybf", 326, 300, 500},
{"emsw", 327, 450, 500},
{"smck", 328, 300, 600},
{"smfs", 329, 300, 500},
{"ryzyz", 330, 300, 450},
{"byzyz", 331, 300, 450},
{"smngssl", 332, 300, 580},
{"ztrsl", 333, 300, 450},
{"lr", 334, 300, 500},
{"ksxyr", 335, 300, 700},
{"wzxyr", 336, 300, 500},
{"xyrfs", 337, 300, 450},
{"sm", 338, 300, 500},
{"xyrsl", 339, 300, 450},
{"tl", 340, 300, 450},
{"jskl", 341, 300, 450},
{"lm", 342, 300, 650},
{"dyq", 343, 300, 675},
{"ym", 344, 300, 450},
{"yl", 345, 300, 600},
{"xyxsll", 346, 300, 500},
{"xyym", 347, 300, 450},
{"ss", 348, 300, 500},
{"rsyz", 349, 300, 450},
{"ysyz", 350, 300, 450},
{"xysw", 351, 300, 500},
{"swyz", 352, 300, 450},
{"sx", 353, 300, 450},
{"xysx", 354, 300, 450},
{"sskl", 355, 300, 450},
{"xyjssl2", 356, 300, 650},
{"dysw2", 357, 300, 400},
{"smngssl2", 358, 300, 580},
{"lr2", 359, 300, 500},
{"ccssl2", 360, 300, 500},
{"emsw2", 361,450, 500},
{"jskl2", 362,300, 450},
{"zdc2", 363,300,450},
{"dx", 364,300, 400},
{"lx", 366, 300, 400},
{"yg", 367, 300, 250},
{"cssyr", 368, 300, 400},
{"kl", 369, 300, 310},
{"jxdx", 371, 300, 500},
{"jxlx", 372, 300, 550},
{"kljl", 365, 300, 600},
{"kljl2",370, 300, 600},
{"syrzs",373, 300, 400},
{"sdyr", 374, 300, 500},
{"syrsl",375, 600, 600},
{"syrsl2",392,600, 600},
{"ryzs", 376, 300, 450},
{"rygs", 377, 1800, 400},
{"syrzsyz", 378, 300, 450},
{"xyryzs", 379, 300, 500},
{"jg", 380, 600, 300},
{"slm", 381, 300, 350},
{"ryfs", 382, 1800, 400},
{"xyrygs", 383, 1800, 400},
{"ryfssl", 384, 1800, 450},
{"ryfssl2",414, 1800, 450},
{"szdfs", 385, 300, 500},
{"szch", 386, 300, 450},
{"xsl", 387, 300, 350},
{"xyrygsyz", 388, 1800, 400},
{"mlsy", 389, 2000, 0},
{"mlsy2", 390, 2000, 0},
{"mlsy3", 391, 500, 500},
{"jyrzs", 393, 400, 500},
{"ybnck", 394, 300, 500},
{"ybnss", 395, 1800, 450},
{"ybnfssl", 396, 1800, 500},
{"zl", 397, 399, 650},
{"jyrtm", 398, 500, 500},
{"jyrtm2", 399, 500, 500},
{"hdnzs", 400, 300, 500},
{"hdnck", 401, 300, 580},
{"hdngs", 402, 1800, 450},
{"x", 403, 300, 450, 400},
{"jysznzs", 404, 300, 500},
{"hdntm", 405, 1800, 500},
{"hdntm2", 406, 1800, 500},
{"kshdjs", 407, 300, 500},
{"tmhdjs", 408, 300, 410},
{"jdhdjs", 409, 300, 450},
{"jsq", 410, 300, 650},
{"xyjdhdjs", 411, 300, 450},
{"sls", 412, 1000, 0},
{"sls2", 413, 1000, 0},
{"slscs", 415, 500, 0},
{"jg2", 416, 600, 300},
{"mlsy4", 417, 500, 500},
{"dcr", 418, 300, 400},
{"dpm", 419, 300, 400},
{"bsr", 420, 300, 400},
{"bsrzs", 421, 300, 400},
{"xybsr", 422, 300, 400},
{"xydpm", 423, 300, 400},
{"dcrzw", 424, 300, 400},
{"js1", 425, 300, 400},
{"js2", 426, 300, 400},
{"js3", 427, 300, 400},
{"js4", 428, 300, 400},
{"js5", 429, 300, 400},
{"xyjs", 430, 300, 400},
{"xydjs", 431, 300, 400},
{"swk", 432, 300, 400},
{"swk2", 433, 300, 400},
{"swpc", 434, 300, 400},
{"zbjs", 435, 300, 400},
{"ygyz", 436, 300, 400},
{"gthfs", 437, 300, 400},
{"hfs", 438, 300, 400},
{"xyxm", 439, 300, 500},
{"kjmw", 440, 300, 500},
{"mhx", 441, 300, 400},
{"mhl", 442, 300, 500},
{"mhhdnfs", 443, 1200, 500},
{"ybnsstm", 444, 300, 500},
{"ybnsstm2", 445, 300, 500},
{"ysrch", 446, 300, 500},
{"xyysrch", 447, 300, 500},
{"xyysrch2", 448, 300, 500},
{"xssg", 449, 400, 500},
{"xssg2", 450, 400, 500},
{"kjc",451,300,400},
{"cql",452,400,650},
{"djc",453,500,580},
{"jxdjc1",454,500,580},
{"jxdjc2",455,500,580},
{"cqlw1",456,500,600},
{"cqlw2",457,500,600},
{"mljz1",458,500,600},
{"mljz2",459,500,600},
{"hazs",460,1500,400},
{"wmws",461,300,450},
{"wmys",462,300,400},
{"wmzj",463,500,300},
{"hywm1",464,300,500},
{"hywm2",465,300,500},
{"wmhw1",466,300,450},
{"wmhw2",467,300,450},
{"wmjz1",468,400,500},
{"wmjz2",469,400,500},
{"jxwg",470,500,500},
{"jxrc",471,500,450},
{"ptqc",472,400,450},
{"xeqc1",473,500,550},
{"xeqc2",474,500,550},
{"cls",475,2000,0},
{"hseq",476,300,400},
{"wyz",477,400,400},
{"hmzw1",478,450,500},
{"hmzw2",479,450,500},
{"hmxw",480,600,400},
{"hmjz1",481,500,500},
{"hmjz2",482,500,500},
{"bsrys",483,400,650},
{"bsrtl1",484,1200,600},
{"bsrtl2",485,1200,600},
{"stbsr",486,400,600},
{"stjg1",487,500,650},
{"stjg2",488,500,650},
{"hqjz1",489,500,500},
{"hqjz2",490,500,500},
{"cqhl1",491,700,600},
{"cqhl2",492,700,600},
{"hlzy1",493,1200,300},
{"hlzy2",494,400,300},
{"ttf",495,300,400},
{"jgyz",496,600,400}
}

TheWarriorClassSkill = {1001,1008,1004,1006,1007,1014,1017,1012,1011,1002,1005,1015,1018,1020,1010,1016,1019,1013,1003,1009}
TheAssassinClassSkill = {1507,1502,1510,1504,1521,1506,1519,1509,1511,1513,1517,1516,1520,1508,1518,1505,1514,1512,1515,1522}
TheArcherClassSkill = {2001,2003,2006,2008,2015,2013,2004,2007,2012,2009,2019,2005,2002,2010,2014,2018,2011,2017,2020,2016}
TheMageClassSkill = {2501,2504,2506,2514,2503,2509,2513,2521,2515,2512,2502,2505,2519,2517,2516,2510,2522,2520,2524,2523}
TheClassSkill = {TheWarriorClassSkill, TheMageClassSkill, TheAssassinClassSkill, TheArcherClassSkill};

WarRune = {10311,10312,10313,10314,10321,10322,10323,10324,10331,10332,10333,10341,10342,10343,10351,10352,10353,10354,10355,10361,10362,10363}
MagRune = {25311,25321,25411,25412,25413,25414,25415,25416,25417,25441,25331,25332,25381,25451,25341,25342,25361,25362,25363,25461,25351,25352,25472,25473,25371,25372,25491}
TaoRune = {30021,30022,30011,30012,30013,30041,30042,30043,30045,30051,30052,30053,30054,30031,30032,30033,30091,30141,30063,30071,30111,30121,30081,30082,30083}

TheWarriorEquipForLevel40 = {
50910141, --Sword and shield
50910241, --Greatsword
50910341, --Dual axes
50911141, --Head
50911241, --Shoulders
50911341, --Body
50911441, --Legs
50911541, --Waist
50911641, --Wrists
50911741, --Feet
50911841, --Cloak
50952141, --Bracelet
50952142, --Bracelet
50952241, --Necklace
50952341, --Ring
50952342 --Ring
}
TheAssassinEquipForLevel40 = {
50920141, --Sword and shield
50920241, --Greatsword
50920341, --Dual axes
50921141, --Head
50921241, --Shoulders
50921341, --Body
50921441, --Legs
50921541, --Waist
50921641, --Wrists
50921741, --Feet
50921841, --Cloak
50952141, --Bracelet
50952142, --Bracelet
50952241, --Necklace
50952341, --Ring
50952342 --Ring
}
TheArcherEquipForLevel40 = {
50930141, --Dual crossbows
50930241, --Heavy crossbow
50930341, --Bow
50931141, --Head
50931241, --Shoulders
50931341, --Body
50931441, --Legs
50931541, --Waist
50931641, --Wrists
50931741, --Feet
50931841, --Cloak
50952141, --Bracelet
50952142, --Bracelet
50952241, --Necklace
50952341, --Ring
50952342 --Ring
}
TheMageEquipForLevel40 = {
50940141, --Ritual dagger
50940241, --Hammer and shield
50940341, --Staff
50941141, --Head
50941241, --Shoulders
50941341, --Body
50941441, --Legs
50941541, --Waist
50941641, --Wrists
50941741, --Feet
50941841, --Cloak
50952141, --Bracelet
50952142, --Bracelet
50952241, --Necklace
50952341, --Ring
50952342 --Ring
}
TheClassEquipForLevel40 = {TheWarriorEquipForLevel40, TheMageEquipForLevel40, TheAssassinEquipForLevel40, TheArcherEquipForLevel40}
TheBodyPartEnum=
{
NES_MAIN_HAND,
NES_HEAD,
NES_SHOULDER,
NES_BODY,
NES_LEG,
NES_WAIST,
NES_ARM,
NES_FOOT,
NES_BACK,
NES_WRIST_L,
NES_WRIST_R,
NES_NECK,
NES_FINGER_L,
NES_FINGER_R
}



-- Begin new GM commands

-- Spawn items
function AddItemToRole(ItemId, Count, RoleID)
    print("执行Lua函数 AddItemToRole(ItemId, Count, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		if type(ItemId) == 'string' then
			ItemId = CDataManager_ITEM_DATA_:GetInstance():GetID(ItemId);
			if not ItemId then
				ItemId = CDataManager_EQUIP_DATA_:GetInstance():GetID(ItemId);
			end
		end
		if not Count then
			Count = 1;
		end
	
	    oRole:GetItemBag():AddItem(ItemId, Count, ITEMLOG_GM_COMMAND, 0, false);
		print("GM命令 - 刷道具, 物品ID: ["..ItemId.."], 数量: ["..Count.."], 角色名字: ["..oRole:GetName().."]");
	end
end

-- Add currency
function AddMoneyToRole(MoneyType, Count, RoleID)
    print("执行Lua函数 AddMoneyToRole(MoneyType, Count, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		oRole:GetItemBag():AddMoney(MoneyType, Count, MONEYLOG_GM_COMMAND, 0);
		print("GM命令 - 增加货币, 货币类型: ["..MoneyType.."], 数量: ["..Count.."], 角色名字: ["..oRole:GetName().."]");
	end
end

-- Deduct currency
function SubMoneyToRole(MoneyType, Count, RoleID)
    print("执行Lua函数 SubMoneyToRole(MoneyType, Count, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		oRole:GetItemBag():SubMoney(MoneyType, Count, MONEYLOG_GM_COMMAND, 0);
		print("GM命令 - 扣除货币, 货币类型: ["..MoneyType.."], 数量: ["..Count.."], 角色名字: ["..oRole:GetName().."]");
	end
end

-- Add experience
function AddExpToRole(n, RoleID)
	print("执行Lua函数 AddExpToRole(n, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		oRole:AddExp(n, false, false);
		print("GM命令 - 增加经验, 数值: ["..n.."], 角色名字: ["..oRole:GetName().."]");
	end
end

-- Set level
function SetLevelToRole(n, RoleID)
	print("执行Lua函数 SetLevelToRole(n, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		oRole:SetLevel(n);
		print("GM命令 - 设置等级, 数值: ["..n.."], 角色名字: ["..oRole:GetName().."]");
	end
end

-- Complete quest
function CompleteQuestToRole(questID, RoleID)
	print("执行Lua函数 CompleteQuestToRole(questID, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		print("GM命令 - 完成任务, 任务ID: ["..questID.."], 角色名字: ["..oRole:GetName().."]");
		local oQuestList = oRole:GetQuestList();
		return oQuestList:Complete(questID);
	end
end

-- Accept quest
function AcceptQuestToRole(questID, RoleID)
	print("执行Lua函数 AcceptQuestToRole(questID, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		print("GM命令 - 接受任务, 任务ID: ["..questID.."], 角色名字: ["..oRole:GetName().."]");
		local oQuestList = oRole:GetQuestList();
		return oQuestList:Accept(questID);
	end
end

-- Learn skill
function LearnSpellToRole(nID, nLevel, RoleID)
	print("执行Lua函数 LearnSpellToRole(nID, nLevel, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		oRole:LearnSpell(nID, nLevel, true)
		print("GM命令 - 学习技能, 技能ID: ["..nID.."], 等级: ["..nLevel.."], 角色名字: ["..oRole:GetName().."]");
	end
end

-- Remove skill
function RemoveSpellToRole(v, RoleID)
	print("执行Lua函数 RemoveSpellToRole(v, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		oRole:RemoveSpell(v);
		print("GM命令 - 删除技能, 技能ID: ["..MoneyType.."], 角色名字: ["..oRole:GetName().."]");
	end
end

-- Add title
function AddTitleToRole(id, RoleID)
	print("执行Lua函数 AddTitleToRole(id, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		oRole:GetTitle():AddTitle(id)
		print("GM命令 - 增加称号, 称号ID: ["..id.."], 角色名字: ["..oRole:GetName().."]");
	end
end

-- Remove title
function DeleteTitleToRole(id, delType, RoleID)
	print("执行Lua函数 DeleteTitleToRole(id, delType, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		oRole:GetTitle():DeleteTitle(id, delType)
		print("GM命令 - 删除称号, 称号ID: ["..id.."], 角色名字: ["..oRole:GetName().."]");
	end
end

function SetFreeModeToRole(byFreeMode, RoleID)
	print("执行Lua函数 SetFreeModeToRole(byFreeMode, RoleID)");
	
	local oRole = g_oGameWorld:FindRole(RoleID);
	if oRole ~= nil then
		oRole:SetFreeMode(byFreeMode);
		print("GM命令 - 设置FreeMode标志, 标志: ["..byFreeMode.."], 角色名字: ["..oRole:GetName().."]");
	end
end

function qfgg(location,message)
	print("执行Lua函数 qfgg(location,message)");
	
	TargetRole:Say(cctGroup, cltLighting, message);    -- All players on the server: scrolling blue text at the top and yellow text in chat
	-- TargetRole:Say(cctGroup, cltTvShow, message);    -- All players on the server: yellow text in the middle of the screen
	
	print("GM命令 - 发送全服公告, 内容: ["..message.."]");
end

-- End new GM commands


function GSID()
	return g_oGameWorld:GetGsSessionID();
end

function Jump(...)
    local param = {...}
    local paramCount = #param;
	local scene = TargetRole:GetScene();
	local staticData;

	if paramCount == 1 and jump_table[param[1]] then
		local scene_name = param[1];
		paramCount = #(jump_table[scene_name].pos);
		param = {};
		for i=1, paramCount do
			param[i] = jump_table[scene_name].pos[i];
		end
	end

    if paramCount == 1 then
        g_oGameWorld:JumpToRole(TargetRole, param[1]);
    elseif paramCount == 2 then
		param[1], param[2], _ = ConvertCoordO2N(param[1], param[2], 0);

        local posList = get_near_pos(TargetRole:GetSceneTempID(),
            param[1] * POINT_PER_CM, param[2] * POINT_PER_CM, 1000000 * POINT_PER_CM, 256, 1)
        g_oGameWorld:JumpTo(TargetRole, TargetRole:GetSceneTempID(), TargetRole:GetSceneID(),
			posList[1].x, posList[1].y, posList[1].z,
		TargetRole:GetFaceDirection());
    elseif paramCount == 3 then
		param[1], param[2], _ = ConvertCoordO2N(param[1], param[2], 0);
        local posList = get_near_pos(TargetRole:GetSceneTempID(),
            param[1] * POINT_PER_CM, param[2] * POINT_PER_CM, param[3] * POINT_PER_CM, 256, 1)
        g_oGameWorld:JumpTo(TargetRole, TargetRole:GetSceneTempID(), TargetRole:GetSceneID(),
			posList[1].x, posList[1].y, posList[1].z, TargetRole:GetFaceDirection());
    elseif paramCount == 4 then
		param[3], param[4], _ = ConvertCoordO2N(param[3], param[4], 0);
        local posList = get_near_pos(param[1],
            param[3] * POINT_PER_CM, param[4] * POINT_PER_CM, 1000000 * POINT_PER_CM, 256, 1)
        g_oGameWorld:JumpTo(TargetRole, param[1], param[2],
            posList[1].x, posList[1].y, posList[1].z, TargetRole:GetFaceDirection());
    elseif paramCount == 5 then
		param[3], param[4], param[5] = ConvertCoordO2N(param[3], param[4], param[5]);
        local posList = get_near_pos(param[1],
            param[3] * POINT_PER_CM, param[4] * POINT_PER_CM, param[5] * POINT_PER_CM, 256, 1)
        g_oGameWorld:JumpTo(TargetRole, param[1], param[2],
            posList[1].x, posList[1].y, posList[1].z, TargetRole:GetFaceDirection());
    else
        print("error jump command")
    end
end

function TakeRole(role_name)
    g_oGameWorld:TakeRole(TargetRole, role_name);
end

function SummonNpc(nTemplateID)
	if type(nTemplateID) == 'string' then
		nTemplateID = CDataManager_NPC_DATA_:GetInstance():GetID(nTemplateID);
	end
    local oScene = TargetRole:GetScene();
    local oNpc = oScene:SpawnNpc(nTemplateID, TargetRole:GetX(), TargetRole:GetY(),
		TargetRole:GetZ() + GRID_WIDTH, TargetRole:GetFaceDirection(), 0, nil);

    return oNpc;
end

function CallPet(nTemplateID, nPetType)
	if type(nTemplateID) == 'string' then
		nTemplateID = CDataManager_NPC_DATA_:GetInstance():GetID(nTemplateID);
	end

	if TargetRole:GetPetCount() >= TargetRole:GetMaxPetCount() then
		return ;
	end

    local oScene = TargetRole:GetScene();
    local oNpc = oScene:SpawnNpc(nTemplateID, TargetRole:GetX(), TargetRole:GetY(),
		TargetRole:GetZ() + GRID_WIDTH, TargetRole:GetFaceDirection(), TargetRole:GetObjID(), nil);

	--oNpc:SetPetWalkSpeed(16);
	oNpc:TransformToPet(nPetType, 0, 1, 7, 800, 0);
	TargetRole:AddPet(oNpc:GetObjID(), nPetType);

    return oNpc;
end

function AddPetExp(nExp)
	local nCount = TargetRole:GetPetCount();
	for i = 1, nCount do
		local dwNpcID = TargetRole:GetPetNpcID(i - 1);
		local oNpc = g_oGameWorld:FindNpc(dwNpcID);
		oNpc:AddPetExp(nExp);
	end
end

function ChangePetState(nState)
	local 	dwNpcID = 0;
	if nState > psFight and nState < psIdle then
		return ;
	end

	if TargetRole:GetPetState() == nState then
		return ;
	end

	TargetRole:SetPetState(nState);

	for i = 1, TargetRole:GetPetCount() do
		dwNpcID = TargetRole:GetPetNpcID(i - 1);
		local oNpc = g_oGameWorld:FindNpc(dwNpcID);
		oNpc:ChangePetAI(nState);
	end

end

function ShowPetLevel()
	local nCount = TargetRole:GetPetCount();
	for i = 1, nCount do
		local dwNpcID = TargetRole:GetPetNpcID(i - 1);
		local oNpc = g_oGameWorld:FindNpc(dwNpcID);

		echo("level "..oNpc:GetPetLevel().." maxlevel "..oNpc:GetPetMaxLevel().." initlevel "..oNpc:GetPetInitLevel())
	end
end

--function LuaConvertCoordO2N(rnX, rnY, rnZ)
--	local k = 70710;
--	local b = 100000;
--	local llX = rnX;
--	local llY = rnY;
--	rnX = ((llX * k - llY * k + (MAX_SCENE_WIDTH / POINT_PER_CM) * b / 2) / b);
--	rnY = ((llX * k + llY * k) / b);

--	return rnX, rnY, rnZ;
--end

function JumpToGuildMap(nEnterMapPosX, nEnterMapPosY, nEnterMapPosZ)
	if TargetRole:GetGuildID() == 0 then
		return ;
	end

	--local nEnterMapPosX = 15300;
	--local nEnterMapPosY = 17700;
	--local nEnterMapPosZ = 6400;

	nEnterMapPosX, nEnterMapPosY, nEnterMapPosZ = ConvertCoordO2N(nEnterMapPosX, nEnterMapPosY, nEnterMapPosZ);

	--g_oGameWorld:JumpTo(TargetRole, 127, TargetRole:GetGuildID(),
	--nEnterMapPosX * POINT_PER_CM, nEnterMapPosY * POINT_PER_CM, nEnterMapPosZ * POINT_PER_CM, 1);

	return ;
end

function AddSpellPts(id, pts)
	TargetRole:AddSpellPts(id, pts);
end

function Kill(dwObjectID)
	if not dwObjectID then
		dwObjectID = TargetRole:GetFocusID();
	end
	local nObjectType = GetTypeFromID(dwObjectID);
	if nObjectType == otNpc or nObjectType == otRole or nObjectType == otHero then
        local oChar = g_oGameWorld:FindCharacter(dwObjectID)
        oChar:SetLastCasterID(TargetRole:GetObjID());
        oChar:SetDropOwnerID(TargetRole:GetObjID());
        oChar:SetHp(0)
    elseif nObjectType == otDoodad then
        local oDoodad = g_oGameWorld:FindDoodad(dwObjectID)
        while oDoodad:GetOpenCount() > 0 do
            oDoodad:OnOpen(TargetRole);
        end
    end
end

function AddExp(n, bykillnpc)
		TargetRole:AddExp(n, bykillnpc, false);
end

function AddExtraExp(n)
		TargetRole:AddExtraExp(n);
end

function LearnSpell(nID, nLevel)
        TargetRole:LearnSpell(nID, nLevel, true)
end

function CastPos(id, level, x, y, z)
        TargetRole:CastSpell(id, level, nil, x, y, z)
end

function AddItem(id, count)
    if type(id) == 'string' then
		local newid = CDataManager_ITEM_DATA_:GetInstance():GetID(id);
		if newid == 0 then
		    newid = CDataManager_EQUIP_DATA_:GetInstance():GetID(id);
		end
		id = newid;
    end
    if not count then
        count = 1;
    end
    TargetRole:GetItemBag():AddItem(id, count, ITEMLOG_GM_COMMAND, 0, false)
end

function SetRefineLevel(slot, level)
	TargetRole:GetItemBag():SetRefineLevel(slot, level);
end

function AddMoney(moneytype, count)
	TargetRole:GetItemBag():AddMoney(moneytype, count, MONEYLOG_GM_COMMAND, 0);
end

function AddMasterExp(count)
	TargetRole:GetItemBag():AddMoney(MONEYTYPE_MASTER_EXP, count, MONEYLOG_GM_COMMAND, 0, 0);
	TargetRole:GetMaster():LuaSyncMasterExpToMS(TargetRole);
end

function SubMoney(moneytype, count)
    TargetRole:GetItemBag():SubMoney(moneytype, count, MONEYLOG_GM_COMMAND, 0);
end

function PrintGoldPool()
    echo("gold_pool: " .. TargetRole:GetItemBag():GetGoldPool());
	echo("gold_pool_chance: " .. TargetRole:GetItemBag():GetChanceByGoldPool());
end

function AddAbilityPts(count)
    TargetRole:GetItemBag():AddMoney(MONEYTYPE_ABILITY_PTS, count, MONEYLOG_GM_COMMAND, 0, 0);
end

function AddAllPotion()
	AddItem(104, 5, ITEMLOG_SCRIPT, 0, false);
	AddItem(105, 5, ITEMLOG_SCRIPT, 0, false);
	AddItem(106, 5, ITEMLOG_SCRIPT, 0, false);
	AddItem(107, 5, ITEMLOG_SCRIPT, 0, false);
	AddItem(108, 5, ITEMLOG_SCRIPT, 0, false);
end

function SetMoney(moneytype, count)
	TargetRole:GetItemBag():SetMoney(moneytype, count);
end

function SetReputation(force, reputation)
	TargetRole:GetReputation():SetValue(force, reputation);
end

function TestDurAttack(times)
    local itembag = TargetRole:GetItemBag();
    if times < 1 then times = 1 end
    if times > 1000 then times = 1000 end
    for i = 1, times do
        itembag:LoseWeaponDur();
    end
end

function TestDurAttacked(times)
    local itembag = TargetRole:GetItemBag();
    if times < 1 then times = 1 end
    if times > 1000 then times = 1000 end
    for i = 1, times do
        itembag:LoseArmorDur(math.random(1,100));
    end
end

function SetSpeed(v)
	if v < 1000/GAME_FPS then
		v = 1000/GAME_FPS;
	end

	TargetRole:GetAttrList():SetAttrBase(TargetRole, catWalkSpeed, v * GAME_FPS / 1000);
	TargetRole:GetAttrList():SetAttrBase(TargetRole, catRunSpeed, v * GAME_FPS / 1000);
end

function RemoveSpell(v)
		TargetRole:RemoveSpell(v);
end

function Suicide()
	Kill(TargetRole:GetObjID());
end

function Revive()
		TargetRole:Revive(0,10000);
end

function FindItemPosInEquipBag(nItemTemplateID)
    local bag = TargetRole:GetItemBag()
	for i = 0, 31 do
		local item = bag:GetItem(CONSLOT_DEFAULT, i);
		if item then
			if (item.m_pTemplate.m_nTemplateID == nItemTemplateID) then
				return i;
			end
		end
	end
	print("failed to find item "..nItemTemplateID.." pos in equip bag");
end

function SetLevel(n)
	TargetRole:SetLevel(n);
--[[
	if n == 41 then
		local class = TargetRole:GetClass();
		local TargetClass = class + 1;
		--print("TargetClass="..TargetClass);
		AddMoney( 10, 9999999);
		for j = 1 , 4 do
			--print("j="..j.."#TheClassSkill[TargetClass]="..#TheClassSkill[TargetClass]);
			for i=1 , #TheClassSkill[TargetClass] do
				SpellLevelUp(TheClassSkill[TargetClass][i])
			end
		end
		for m = 3, #TheClassEquipForLevel40[TargetClass] do
			n = m - 2
			TargetRole:GetItemBag():EquipItemOnBirth(TheClassEquipForLevel40[TargetClass][m], TheBodyPartEnum[n])
		end
	end
--]]

end

function SetPetDebug()
		local num = TargetRole:GetPetCount();

		for i = 0, num - 1 do
			local npcid = TargetRole:GetPetNpcID(i);
			local npc = CGameWorld:GetInstance():FindValidCharacter(npcid);

			if npc:GetAIProcessor():GetDebugFlag() > 0 then
				npc:GetAIProcessor():SetDebugFlag(0);
			else
				npc:GetAIProcessor():SetDebugFlag(1);
			end
		end
end

function FullPetHp()
		local num = TargetRole:GetPetCount();

		for i = 0, num - 1 do
			local npcid = TargetRole:GetPetNpcID(i);
			local npc = CGameWorld:GetInstance():FindValidCharacter(npcid);
			npc:SetHp(npc:GetAttr(catMaxHp));
		end
end

function FullHp()
		local attrlist = TargetRole:GetAttrList();
		local maxhp = attrlist:GetAttr(catMaxHp);

		TargetRole:ChangeHp(maxhp, 0);
end

function FullMp()
		local attrlist = TargetRole:GetAttrList();
		local maxmp = attrlist:GetAttr(catMaxMp);

		TargetRole:ChangeMp(maxmp);
end

function SetHp(n)
		TargetRole:ChangeHp(n - TargetRole:GetHp(), 0);
end

function EnableCD(enabled)
	if enabled then
		TargetRole:GetCoolDown():Enable(true);
	else
		TargetRole:GetCoolDown():Enable(false);
		TargetRole:ClearAllCooldown();
	end
end

function DrawDebugLine(draw)
	if draw then
		g_oGameWorld:SetDrawDebugLine(true);
	else
		g_oGameWorld:SetDrawDebugLine(false);
	end
end

function CheckConsume(check)
	g_oGameWorld:SetCheckConsume(check);
end

function hoo()
    local oScene = TargetRole:GetScene()
    local oNpc = oScene:SpawnNpc(1, 1, nqNormal, TargetRole:GetX(), TargetRole:GetY(), TargetRole:GetZ(), 0, 0, nil);
    local ai = oNpc:GetNpcAI()

    if ai then
	oNpc:ChangeHp(10000, 0)
	local retval = ai:SetAIScript("hoo.lua")
	if not retval then
	    print("Set ai script fail.")
	    return
	end

	ai:SetAIVar(2, TargetRole:GetObjID())
	return
    end
end

function ToggleDoor(DoorID)
    local oScene = TargetRole:GetScene();
    oScene:SetDoorState(DoorID, not oScene:IsDoorClosed(DoorID));
end

function SummonDoodad(nTemplateID)
    local oScene = TargetRole:GetScene();
    local oDoodad = oScene:SpawnDoodad(nTemplateID, TargetRole:GetX(), TargetRole:GetY(), TargetRole:GetZ(), TargetRole:GetFaceDirection());

    return oDoodad;
end

function God()
	if TargetRole:GetControlCount(cclInvincible) > 0 then
	    while TargetRole:GetControlCount(cclInvincible) > 0 do
    	    TargetRole:DecControlCount(cclInvincible);
    	end
	else
	    TargetRole:IncControlCount(cclInvincible);
	end
end

pos_dis = 400
pos_num = 16

pos_offset = {}

for i=1, pos_num do
    pos_offset[i] = {}
    pos_offset[i].x = pos_dis * math.cos(math.pi * 2 * i / pos_num)
    pos_offset[i].y = pos_dis * math.sin(math.pi * 2 * i / pos_num)
end

function Surround()
    local x = TargetRole:GetX()
    local y = TargetRole:GetY()
    local z = TargetRole:GetZ()

    local scene = TargetRole:GetScene()
    local dir = TargetRole:GetFaceDirection()
    for i = 1, pos_num do
        local targetx = x + pos_offset[i].x * POINT_PER_CM * cos(dir + MAX_DIRECTION_COUNT * 3 / 4) / SIN_COS_NUMBER +
	    pos_offset[i].y * POINT_PER_CM * cos(dir) / SIN_COS_NUMBER
        local targety = y + pos_offset[i].x * POINT_PER_CM * sin(dir + MAX_DIRECTION_COUNT * 3 / 4) / SIN_COS_NUMBER +
	    pos_offset[i].y * POINT_PER_CM * sin(dir) / SIN_COS_NUMBER

        local target = get_near_pos(scene:GetSceneTempID(),
				    targetx, targety, z, 500, 1)

        if #target >= 1 then
	        scene:SpawnNpc(101,1,1, target[1].x, target[1].y, target[1].z, 0, 0, nil)
    	end
    end

end

function Focus()
    local FocusID = TargetRole:GetFocusID();
    return g_oGameWorld:FindCharacter(FocusID);
end

function PkMode(mode)
	TargetRole:ChangePkMode(mode);
end

function echo(text)
	TargetRole:GetScene():Say(TargetRole:GetObjID(), cltNormal, text);
end

function PetInfo()
    local char = Focus();
    if char:GetObjType() == otNpc then
	    local npc = Cast(char, "CNpc");
    	echo("type="..npc:GetPetType());
    	echo("level="..npc:GetPetLevel());
    	echo("max level="..npc:GetPetMaxLevel());
		echo("init level="..npc:GetPetInitLevel());
		echo("pet exp="..npc:GetPetExp());
	end
end

function FocusInfo()
    local char = Focus();
    if char:GetObjType() == otNpc then
	    local npc = Cast(char, "CNpc");
    	echo("id="..npc:GetNpcData().nTemplateID);
    	echo("ai="..npc:GetAIProcessor():GetAIID());
    	echo("quality="..npc:GetQuality());
    elseif char:GetObjType() == otRole then
        local role = Cast(char, "CRole");
        echo("pkvalue="..role:GetPkValue());
        echo("vip="..role:GetVIPLevel()..", "..math.ceil((TargetRole:GetVIPTime(TargetRole:GetVIPLevel()) - g_oGameWorld:GetGameTime()) / 86400).." day");
    end

	echo("name="..char:GetName());
	echo("level="..char:GetLevel());
	echo("hp="..char:GetHp().."/"..char:GetAttrList():GetAttr(catMaxHp));
	echo("mp="..char:GetMp().."/"..char:GetAttrList():GetAttr(catMaxMp));

	local rx = TargetRole:GetX();
	local ry = TargetRole:GetY();
	local rz = TargetRole:GetZ();
	local fx = char:GetX();
	local fy = char:GetY();
	local fz = char:GetZ();

	local xx, yy, zz = ConvertCoordN2O(fx/POINT_PER_CM, fy/POINT_PER_CM, fz/POINT_PER_CM);
	echo("pos=("..math.ceil(xx)..", "..math.ceil(yy)..", "..math.ceil(zz)..")");

	echo("dir="..(char:GetFaceDirection() * 8 / MAX_DIRECTION_COUNT));

	local d = math.sqrt((rx-fx)*(rx-fx)+(ry-fy)*(ry-fy)+(rz-fz)*(rz-fz));
	echo("distance="..(d/POINT_PER_CM));
end

function SelfInfo()
	echo("name="..TargetRole:GetName());
	echo("level="..TargetRole:GetLevel());
	echo("hp="..TargetRole:GetHp().."/"..TargetRole:GetAttrList():GetAttr(catMaxHp));
	echo("mp="..TargetRole:GetMp().."/"..TargetRole:GetAttrList():GetAttr(catMaxMp));

	local fx = TargetRole:GetX();
	local fy = TargetRole:GetY();
	local fz = TargetRole:GetZ();
	fx, fy, fz = ConvertCoordN2O(fx/POINT_PER_CM, fy/POINT_PER_CM, fz/POINT_PER_CM);
	echo("pos=("..math.ceil(fx)..", "..math.ceil(fy)..", "..math.ceil(fz)..")");

    echo("pkvalue="..TargetRole:GetPkValue());
    echo("vip="..TargetRole:GetVIPLevel()..", "..math.ceil((TargetRole:GetVIPTime(TargetRole:GetVIPLevel()) - g_oGameWorld:GetGameTime()) / 86400).." day");
	echo("online_time="..TargetRole:GetTotalOnlineTime());
	echo("level_online_time="..TargetRole:GetLevelOnlineTime());
	echo("activity_pts="..TargetRole:GetActivityPts());
end

function SceneInfo()
    local scene = TargetRole:GetScene();

    echo("template id="..scene:GetSceneTempID());
    echo("instance id="..scene:GetSceneID());
    echo("weather="..scene:GetWeather());
    echo("role count="..scene:GetRoleCount());
    echo("npc count="..scene:GetNpcCount());

end

function CompleteQuest(questID)
	local oQuestList = TargetRole:GetQuestList();
	return oQuestList:Complete(questID);
end

function AcceptQuest(questID)
	local oQuestList = TargetRole:GetQuestList();
	return oQuestList:Accept(questID);
end

function AddTitle(id)
	TargetRole:GetTitle():AddTitle(id)
end

function DeleteTitle(id, delType)
	TargetRole:GetTitle():DeleteTitle(id, delType)
end


function EquipTitle(id)
	TargetRole:GetTitle():EquipTile(id)
end

function UnEquipTitle()
	TargetRole:GetTitle():UnEquipTitle()
end

function EquipRune(spell_id, slot_id, rune_slot)
	local ret = TargetRole:ActivateRune(spell_id, rune_slot);
	if ret ~= errSuccess then
		print("Activate rune failed");
	end

	print(spell_id..", "..slot_id..", "..rune_slot);

	ret = TargetRole:EquipRune(slot_id, rune_slot);
	if ret ~= errSuccess then
		print("Equip rune failed");
	end

end

function AddRuneMtrl()
	AddItem(4000, 5, ITEMLOG_SCRIPT, 0, false);
	AddItem(4001, 5, ITEMLOG_SCRIPT, 0, false);
	AddItem(4002, 5, ITEMLOG_SCRIPT, 0, false);
	AddItem(4003, 5, ITEMLOG_SCRIPT, 0, false);
	AddItem(4004, 5, ITEMLOG_SCRIPT, 0, false);
end

function Huhuhu()
    AddMoney( MONEYTYPE_01, 999999);
    AddMoney( MONEYTYPE_02, 999999);
    AddMoney( MONEYTYPE_03, 999999);
    AddMoney( MONEYTYPE_04, 999999);
    AddMoney( MONEYTYPE_ABILITY_PTS, 999999);

    for i = 1, 5 do
       AddItem(125, 99, ITEMLOG_SCRIPT, 0, false);
	   AddItem(126, 99, ITEMLOG_SCRIPT, 0, false);
	   AddItem(127, 99, ITEMLOG_SCRIPT, 0, false);
	   AddItem(128, 99, ITEMLOG_SCRIPT, 0, false);
	   AddItem(129, 99, ITEMLOG_SCRIPT, 0, false);
    end
end

function StartMount(nSlot)
    local FocusID = TargetRole:GetFocusID();
    local focus = g_oGameWorld:FindCharacter(FocusID);

    if focus ~= nil then
		return TargetRole:StartMount(focus, nSlot);
	end
end

function LeaveMount()
	TargetRole:LeaveMount();
end

function ActivateMount()
	TargetRole:ActivateMount(1);
	TargetRole:ActivateMount(2);
	TargetRole:ActivateMount(3);
end

function Master()
	call_ms_func("script/master/main.lua", "MakeOneToBeMaster", TargetRole:GetObjID(), 0, 0);
end


function CompleteAchievement(achievementID)
	local oAchievementList = TargetRole:GetAchievementList();
	oAchievementList:Complete(achievementID);
end

function SetPkValue(pkvalue)
	TargetRole:SetPkValue(pkvalue);
end

function SetQV(QuestID, VarIndex, Value)
	local oQuestList = TargetRole:GetQuestList();
	local oQuest = oQuestList:GetAcceptedQuest(QuestID);
	oQuestList:SetVariable(oQuest, VarIndex, Value);
end


function Power(power_level)
	power_level=5

    local class = TargetRole:GetClass();
    local role = TargetRole;
    local power_config = POWER_TABLE[power_level][class];

    if power_config.level then
        role:SetLevel(power_config.level);
        role:GetItemBag():AddMoney(MONEYTYPE_01, 10000000, MONEYLOG_GM_COMMAND, 0, 0);
        role:GetItemBag():AddMoney(MONEYTYPE_02, 10000000, MONEYLOG_GM_COMMAND, 0, 0);
        role:GetItemBag():AddMoney(MONEYTYPE_SPELL_PTS, 500000000, MONEYLOG_GM_COMMAND, 0, 0);
        role:GetItemBag():SetUseMoneytype01Only(0);
    end
    if power_config.spirit_level then
        role:GetSpirit():SetLevel(power_config.spirit_level);
    end
    if power_config.refine_level then
        for i = NES_MAIN_HAND, NES_TOTAL - 1 do
            if i ~= NES_AVATAR then
                role:GetItemBag():SetRefineLevel(i, power_config.refine_level);
            end
        end
    end
    if power_config.blood_level then
        for i = 0, MAX_BLOODSEA_PAGE - 1 do
            role:GetSpirit():OpenBlood(i);
            for j = 0, MAX_BLOODSEA_SLOT_PER_PAGE - 1 do
                role:GetSpirit():SetBlood(i, j, power_config.blood_level);
            end
        end
    end
    if power_config.spell_level then
        role:GetSpellBook():BeginSpellSet();
        local spell_set = role:GetSpellBook():NextSpellSet();
        while spell_set do
            local spell_level = spell_set:GetLevel();
            while spell_level < power_config.spell_level do
                role:SpellLevelUp(spell_set:GetID());
                spell_level = spell_level + 1;
            end
            spell_set = role:GetSpellBook():NextSpellSet();
        end
    end
    if power_config.equip then
        local bag = role:GetItemBag();

        for i = NES_MAIN_HAND, NES_TOTAL - 1 do
            if power_config.equip[i] then
                local equip_item = CItemMgr:GetInstance():GenerateItem(power_config.equip[i].id, 1, 'main.lua', 1189);
                if power_config.equip[i].attr then
                    for i, v in ipairs(power_config.equip[i].attr) do
                        equip_item:SetAttribute(i-1, v);
                    end
                end
                if power_config.equip[i].soul then
                    for i, v in ipairs(power_config.equip[i].soul) do
                        local soul_data = CDataManager_SOUL_DATA_:GetInstance():GetData(v);
                        if soul_data then
                            equip_item:SetSocketColor(i-1, soul_data.m_nMatchSocket);
                            equip_item:SetSoul(i-1, v);
                        end
                    end
                end

                bag:AddItem(equip_item, ITEMLOG_GM_COMMAND, 0, false);
                local item_pos = equip_item.m_iteminfo.m_byItemSlot;
                bag:IdentifyItem(CONSLOT_DEFAULT, item_pos);
                bag:MoveItem(CONSLOT_DEFAULT, item_pos, CONSLOT_EQUIPPED, i);
            end
        end
    end
    if power_config.air then
        for i = 0, MAX_AIRSEA_EQUIP_CONTAINER_SIZE - 1 do
            role:GetSpirit():AddSlot(sasbtEquip, 1);
            if power_config.air[i] then
                local air_pos = role:GetSpirit():AddAir(power_config.air[i], 0);
                role:GetSpirit():MoveAir(sasbtBag, air_pos, sasbtEquip, i);
            end
        end
    end
	FullHp();
	FullMp();

end


function HireHero(level, cls, sex)
	TargetRole:HireHero(level, cls, sex, 0, 1, true);
end

function SetHeroAi(aitype)
	local hero = TargetRole:GetHero();
	if hero == nil then
		return;
	end

	hero:SetAiType(aitype);
end

function AddHeroExp(n)
	local hero = TargetRole:GetHero();
	if hero == nil then
		return;
	end

	hero:AddExp(n);
end

function SetHeroLevel(n)
	local hero = TargetRole:GetHero();
	if hero == nil then
		return;
	end

	hero:SetLevel(n);
end

function HeroDie()
	local hero = TargetRole:GetHero();
	if hero == nil then
		return;
	end
	hero:SetHp(0);

end

function HeroRevive()
	local hero = TargetRole:GetHero();
	if hero == nil then
		return;
	end
	hero:Revive(0, 10000, 10000);

end

function AddPersonalEnemyByGM(dwRoleID)
	TargetRole:GetFriend():AddPersonalEnemy(TargetRole, dwRoleID);
end


function Luck(n)
	TargetRole:GetItemBag():IncreaseWeaponLuck(n);
end

function TestBT(type, p0, p1, p2, p3)
	pp0 = p0 or 0;
	pp1 = p1 or 0;
	pp2 = p2 or 0;
	pp3 = p3 or 0;
	test_bt_action(TargetRole, type, pp0, pp1, pp2, pp3);
end

function Kick(id)
    local oRole = g_oGameWorld:FindRole(id);
	if oRole ~= nil then
		oRole:Kick();
	end
end

function SetCanCast(v)
	g_oGameWorld:SetCanCast(v);
end

function Weather(w, d)
	TargetRole:GetScene():SetWeather(w, d * GAME_FPS);
end

function PrintAIData(dwAIID)
	data = find_ai_data(dwAIID);
	print("AI "..dwAIID..": data.szScriptName="..data.szScriptName..":");
end

function PrintNpcBT(dwCharID)
	if not dwCharID then
		dwCharID = TargetRole:GetFocusID();
	end
    local oChar = g_oGameWorld:FindCharacter(dwCharID)
    if oChar then
	   local oProcessor = oChar:GetAIProcessor();
	   if oProcessor then
		   oProcessor:PrintAllBT();
	   end
    end
end

function SetRune(nSpellID, nRuneSlot)
--	local SpellBook = TargetRole:GetSpellBook();
	TargetRole:EquipRune(nSpellID, nRuneSlot);
end

function EquipSpell(nSlotID, nSpellID)
	TargetRole:EquipSpell(nSlotID, nSpellID);
end

--[[VIP command: for GM use, level represents the equipment type
When level is 0, month represents the inscription ID; when level is any value other than 1, it represents the green attribute ID.
0 means weapon
	Pass the inscription to add in month: add 100000 for m_wRune[0] or 200000 for m_wRune[1].
	A month value below 100000 adds a green attribute to the weapon.
1 means body armor
2 means back
3 means head
4 means shoulder
5 means arm
6 means waist
7 means foot
8 means neck
9 means left ring (finger_L)
10 means right ring (finger_R)
11 means left bracelet (wrist_L)
12 means right bracelet (wrist_R)
]]--
function VIP (level, month)
	local bag = TargetRole:GetItemBag();
	local equip = bag:GetItem(0, level);
	if equip == nil then
		return;
	end
	if level == 0 and month > 100000 then
		if month > 200000 then
			equip.m_iteminfo.m_wRune[1] = month - 200000;
		else
			equip.m_iteminfo.m_wRune[0] = month - 100000;
		end
	else
		if month>100000 then
			equip.m_iteminfo.m_dwSocketCombination = month-100000;
		elseif equip.m_iteminfo.m_wRandomAttrID[0]  == 0 then
			equip.m_iteminfo.m_wRandomAttrID[0] = month;
		elseif equip.m_iteminfo.m_wRandomAttrID[1]  == 0 then
			equip.m_iteminfo.m_wRandomAttrID[1] = month;
		elseif equip.m_iteminfo.m_wRandomAttrID[2]  == 0 then
			equip.m_iteminfo.m_wRandomAttrID[2] = month;
		elseif equip.m_iteminfo.m_wRandomAttrID[3]  == 0 then
			equip.m_iteminfo.m_wRandomAttrID[3] = month;
		end	
	end
end

g_ClassRemap = {
		[0] = 1,
		[1] = 4,
		[2] = 2,
		[3] = 3,
};
g_RMBLevel = {
		[0] = 0,
		[1] = 0.5,
		[2] = 1,
		[3] = 2,
		[4] = 3,
		[5] = 4,
};
g_SuitValue = {
		[0]	= 1,
		[1] = 1,
		[2] = 1,
		[3] = 4,
		[4] = 4,
		[5] = 6,
		[6] = 9,
		[7]	= 9,
		[8] = 9,
		[9] = 9,
		[10] = 9,
};
function QAInit()
	print("######################################")
	local bag = TargetRole:GetItemBag();
	local nLevel = TargetRole:GetLevel();
	local nClass = g_ClassRemap[TargetRole:GetClass()];
	local nSuit=g_SuitValue[math.floor(nLevel / 7)];


	local nItemIDhead=99900000+nClass*10000+nSuit*100

	for nPart = 11, 33 do
		local nItemID=nItemIDhead+nPart

		AddItem(nItemID, 1)


		local itemPos=FindItemPosInEquipBag(nItemID)
		if itemPos then

			for i=0,13 do
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, i);
			end
			else
				print("*********not found: "..nItemID.."**********")
		end
		--m_byItemSlot




		if nPart == 31 then

			local itemPos=FindItemPosInEquipBag(nItemID)
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_WRIST_L);
				print("111111111111111111111111111111"..itemPos)

			end

			AddItem(nItemID, 1)
			itemPos=FindItemPosInEquipBag(nItemID)
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_WRIST_R);
			else
				print("0000000000000000000000000000"..itemPos)
			end

		elseif nPart == 33 then

			local itemPos=FindItemPosInEquipBag(nItemID)
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_FINGER_L);
			end

			AddItem(nItemID, 1)
			itemPos=FindItemPosInEquipBag(nItemID)
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_FINGER_R);
			end

		elseif nPart == 32 then
			local itemPos=FindItemPosInEquipBag(nItemID)
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_NECK);
			end
		end

	end
end

function QAaddAt(count)
	TargetRole:GetAttrList():SetAttrA(TargetRole, catHpRecovery, count);
	TargetRole:GetAttrList():SetAttrA(TargetRole, catStrength, count);
	TargetRole:GetAttrList():SetAttrA(TargetRole, catAgility, count);
	TargetRole:GetAttrList():SetAttrA(TargetRole, catIntellect, count);
	TargetRole:GetAttrList():SetAttrA(TargetRole, catStamina, count);
	TargetRole:GetAttrList():SetAttrA(TargetRole, catSpirit, count);
end

function QAaddhpmp(count)
	TargetRole:GetAttrList():SetAttrA(TargetRole, catMaxHp, count);
	TargetRole:GetAttrList():SetAttrA(TargetRole, catMaxMp, count);
end
function AddSpiritPts(count)
	local oSpirit = TargetRole:GetSpirit();
	oSpirit:AddPTs(count);
end

function SetSpiritAirLevel(level)
	TargetRole:GetSpirit():SetProductAirLevel(level);
end

function SetSpiritLevel(level)
	TargetRole:GetSpirit():SetLevel(level);
end

function TryRevive(type)
	TargetRole:TryRevive(type);
end

function DropMax()
	g_oGameWorld:ToggleGlobalDropMax();
end

function DropRate(r)
	if (r) then
		g_oGameWorld:SetGlobalDropRate(r);
	end
	echo("GlobalDropRate=" .. g_oGameWorld:GetGlobalDropRate())
end

function ExpRate(r)
	if (r) then
		g_oGameWorld:SetGlobalExpRate(r);
	end
	echo("GlobalExpRate=" .. g_oGameWorld:GetGlobalExpRate())
end

function AddAura(id)
	local info = CSpellConfig:GetInstance():FindAuraTemplate(id);
	if info == nil then
		return;
	end

	TargetRole:AppendAura(info, TargetRole, 0, NULL, 0, 0, 0, 1);
end

function LearnTransformSpell()
	for i =1, #TransformSpellIDList do
		TargetRole:LearnSpell(TransformSpellIDList[i], 1, true);
	end
end

function SetAttackRange(value)
	TargetRole:GetAttrList():SetAttrBase(TargetRole, catAttackRange, value * POINT_PER_CM);
end

function SetMoveSpeed(value)
	TargetRole:GetAttrList():SetAttrBase(TargetRole, catMoveSpeed, value * POINT_PER_CM/1000);
end

function SetAttackValue(value)
	LowValue = value
	HighValue = value + 3000000
	print("-----HighValue="..HighValue)
	TargetRole:GetAttrList():SetAttrBase(TargetRole, catPhyAtkHi, HighValue);
	TargetRole:GetAttrList():SetAttrBase(TargetRole, catPhyAtkLo, LowValue);
	--TargetRole:GetAttrList():SetAttrBase(TargetRole, catMagAtkHi, HighValue);
	--TargetRole:GetAttrList():SetAttrBase(TargetRole, catMagAtkLO, LowValue);
end


function SetMonsterConvert(MonsterName)
	print("====="..MonsterName)

	for i=1, #TheTransformMonsterIDList do
		if type(TheTransformMonsterIDList[i][1]) == "string" then
			print("=====++++++++++"..TheTransformMonsterIDList[i][1])

			if MonsterName == TheTransformMonsterIDList[i][1] then

				print("=====-------+++++"..TheTransformMonsterIDList[i][2])
				print("=====********+++"..TheTransformMonsterIDList[i][3])
				AddAura(TheTransformMonsterIDList[i][2])
				SetAttackRange(TheTransformMonsterIDList[i][3])
				SetMoveSpeed(TheTransformMonsterIDList[i][4])
				SetAttackValue(3000000)
			end

		end

	end

end

function StandardCombatDataTest()
	AddAura(9);
end
function StandardSkillLevelUp()
	SpellLevelUp(9);
end

function Sync()
	TargetRole:ForceSyncPos();
end

function SpellLevelUp(id)
	--TargetRole:SpellLevelUp(id);
	print("role call "..id.." SpellLevelUp obsolete");
end

function SetOption(option)
    if g_GWConfig.DebugOption[option] ~= nil then
        g_GWConfig.DebugOption[option] = not g_GWConfig.DebugOption[option];
        echo('switch '..option..' to '..tostring(g_GWConfig.DebugOption[option]));
    else
        echo('unknown option name '..option);
    end
end

function AddAttrA(attr, n)
	TargetRole:GetAttrList():AddAttrA(TargetRole, attr, n);
end

function CollectLogNow()
	CGameWorld:GetInstance():DLCollectLogNow(TargetRole);
end

function LearnAllSpell()
	local SpellConfig = CSpellConfig:GetInstance();
	local cls = TargetRole:GetClass();
	local level = TargetRole:GetLevel();

        SpellConfig:BeginSpell();
        local spell_t = SpellConfig:NextSpell();
        while spell_t do
			local CanLearn = spell_t:CanLearnByClass(cls);

			if CanLearn and level >= spell_t:GetAttr(satRequireLevel) and spell_t:GetRuneSlot() == 0 and
				spell_t:GetAttr(satCastType) == spctActive and spell_t:GetLevel() == 0 then
				print(spell_t:GetID()..' : '..spell_t:GetRuneSlot()..' : '..spell_t:GetLevel()..' : '..spell_t:GetAttr(satCastType))
				TargetRole:LearnSpell(spell_t:GetID(), 0, true);
			end

        	spell_t = SpellConfig:NextSpell();
        end
end

function SKillTest(WeaponType)
	if type(WeaponType) ~= "string" then
		return
	end
	SetLevel(60);

	local WarriorEquip = {
	10410121, --Sword and shield
	10410221, --Greatsword
	10410321, --Dual axes
	10411121, --Helmet
	10411221, --Shoulder armor
	10411321, --Armor
	10411421, --Leg armor
	10411521, --Waist armor
	10411621, --Bracers
	10411721, --Combat boots
	10411821 --Cloak
	};
	local AssassinEquip = {
	10420121, --Spear
	10420221, --Dagger
	10420321, --Chain blade
	10421121, --Helmet
	10421221, --Shoulder armor
	10421321, --Armor
	10421421, --Leg armor
	10421521, --Waist armor
	10421621, --Bracers
	10421721, --Combat boots
	10421821 --Cloak
	};
	local ArcherEquip = {
	10430121, --Dual crossbows
	10430221, --Heavy crossbow
	10430321, --Bow
	10431121, --Helmet
	10431221, --Shoulder armor
	10431321, --Armor
	10431421, --Leg armor
	10431521, --Waist armor
	10431621, --Bracers
	10431721, --Combat boots
	10431821 --Cloak
	};
	local MageEquip = {
	10440121, --Spellbook
	10440221, --Magic hammer
	10440321, --Staff
	10441121, --Helmet
	10441221, --Shoulder armor
	10441321, --Armor
	10441421, --Leg armor
	10441521, --Waist armor
	10441621, --Bracers
	10441721, --Combat boots
	10441821 --Cloak
	};
	local EquipGroup = {WarriorEquip,MageEquip,AssassinEquip,ArcherEquip};
	local bangleGroup   = {10452121,10452122,10452123,10452124};
	local necklaceGroup = {10452221,10452222,10452223,10452224};
	local ringGroup     = {10452321,10452322,10452323,10452324};
	local OtherEquipGroup = {bangleGroup,ringGroup,necklaceGroup};
	local bag = TargetRole:GetItemBag();
	local class = TargetRole:GetClass();
	for i = 1, #EquipGroup[class+1] do
		AddItem(EquipGroup[class+1][i], 1);
		local itemPos=FindItemPosInEquipBag(EquipGroup[class+1][i]);
		if itemPos then
			for i=0,10 do
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, i);
			end
		end
	end

	local TempGroup = {};
	for i=1, #OtherEquipGroup do
		for j=1, 2 do
			local Nrandnum1 = math.random(1,10000)
			if Nrandnum1 <= 2500 then
				TempGroup[#TempGroup+1] = OtherEquipGroup[i][1];
			elseif Nrandnum1 > 2500 and Nrandnum1 <= 5000 then
				TempGroup[#TempGroup+1] = OtherEquipGroup[i][2];
			elseif Nrandnum1 > 5000 and Nrandnum1 <= 7500 then
				TempGroup[#TempGroup+1] = OtherEquipGroup[i][3];
			elseif Nrandnum1 > 7500 and Nrandnum1 <= 10000 then
				TempGroup[#TempGroup+1] = OtherEquipGroup[i][4];
			end
		end
	end

	for i=1, #TempGroup -1 do
		AddItem(TempGroup[i], 1);
		local itemPos=FindItemPosInEquipBag(TempGroup[i]);
		if i==1 then
			bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_WRIST_L);
		elseif i==2 then
			bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_WRIST_R);
		elseif i==3 then
			bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_FINGER_L);
		elseif i==4 then
			bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_FINGER_R);
		else
			bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_NECK);
		end
	end

	if class == clsWarrior then
		if WeaponType == "jd" then
			local itemPos=FindItemPosInEquipBag(10410121);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1022);
			TargetRole:EquipSpell(qssMouseR, 1020);
			TargetRole:EquipSpell(qssKey1, 1003);
			TargetRole:EquipSpell(qssKey2, 1005);
			TargetRole:EquipSpell(qssKey3, 1014);
			TargetRole:EquipSpell(qssKey4, 1011);
			SetRune(1022, 1, 1);
			SetRune(1020, 7, 1);
			SetRune(1003, 7, 1);
			SetRune(1005, 4, 1);
			SetRune(1014, 5, 1);
			SetRune(1011, 1, 1);
		elseif WeaponType == "sf" then
			local itemPos=FindItemPosInEquipBag(10410321);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1022);
			TargetRole:EquipSpell(qssMouseR, 1009);
			TargetRole:EquipSpell(qssKey1, 1004);
			TargetRole:EquipSpell(qssKey2, 1010);
			TargetRole:EquipSpell(qssKey3, 1012);
			TargetRole:EquipSpell(qssKey4, 1016);
			SetRune(1022, 1, 1);
			SetRune(1009, 4, 1);
			SetRune(1004, 6, 1);
			SetRune(1010, 7, 1);
			SetRune(1012, 6, 1);
			SetRune(1016, 7, 1);
		elseif WeaponType == "jj" then
			local itemPos=FindItemPosInEquipBag(10410221);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1022);
			TargetRole:EquipSpell(qssMouseR, 1023);
			TargetRole:EquipSpell(qssKey1, 1002);
			TargetRole:EquipSpell(qssKey2, 1024);
			TargetRole:EquipSpell(qssKey3, 1025);
			TargetRole:EquipSpell(qssKey4, 1016);
			SetRune(1022, 1, 1);
			SetRune(1023, 5, 1);
			SetRune(1002, 2, 1);
			SetRune(1024, 5, 1);
			SetRune(1025, 2, 1);
			SetRune(1016, 6, 1);
		end
	elseif class == clsMage then
		if WeaponType == "fz" then
			local itemPos=FindItemPosInEquipBag(10440321);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2525);
			TargetRole:EquipSpell(qssMouseR, 2513);
			TargetRole:EquipSpell(qssKey1, 2526);
			TargetRole:EquipSpell(qssKey2, 2509);
			TargetRole:EquipSpell(qssKey3, 2503);
			TargetRole:EquipSpell(qssKey4, 2523);
			SetRune(2525, 2, 1);
			SetRune(2513, 2, 1);
			SetRune(2526, 2, 1);
			SetRune(2509, 9, 1);
			SetRune(2503, 1, 1);
			SetRune(2523, 3, 1);
		elseif WeaponType == "cd" then
			local itemPos=FindItemPosInEquipBag(10440221);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2501);
			TargetRole:EquipSpell(qssMouseR, 2515);
			TargetRole:EquipSpell(qssKey1, 2512);
			TargetRole:EquipSpell(qssKey2, 2517);
			TargetRole:EquipSpell(qssKey3, 2524);
			TargetRole:EquipSpell(qssKey4, 2505);
			SetRune(2501, 4, 1);
			SetRune(2515, 1, 1);
			SetRune(2512, 5, 1);
			SetRune(2517, 2, 1);
			SetRune(2524, 6, 1);
			SetRune(2505, 5, 1);
		elseif WeaponType == "fd" then
			local itemPos=FindItemPosInEquipBag(10440121);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2501);
			TargetRole:EquipSpell(qssMouseR, 2513);
			TargetRole:EquipSpell(qssKey1, 2526);
			TargetRole:EquipSpell(qssKey2, 2516);
			TargetRole:EquipSpell(qssKey3, 2524);
			TargetRole:EquipSpell(qssKey4, 2510);
			SetRune(2501, 3, 1);
			SetRune(2513, 4, 1);
			SetRune(2526, 5, 1);
			SetRune(2516, 2, 1);
			SetRune(2524, 4, 1);
			SetRune(2510, 1, 1);
		end
	elseif class == clsRogue then
		if WeaponType == "bs" then
			local itemPos=FindItemPosInEquipBag(10420221);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1508);
			TargetRole:EquipSpell(qssMouseR, 1502);
			TargetRole:EquipSpell(qssKey1, 1509);
			TargetRole:EquipSpell(qssKey2, 1519);
			TargetRole:EquipSpell(qssKey3, 1524);
			TargetRole:EquipSpell(qssKey4, 1512);
			SetRune(1508, 1, 1);
			SetRune(1502, 2, 1);
			SetRune(1509, 7, 1);
			SetRune(1519, 3, 1);
			SetRune(1524, 3, 1);
			SetRune(1512, 2, 1);
		elseif WeaponType == "cq" then
			local itemPos=FindItemPosInEquipBag(10420121);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1507);
			TargetRole:EquipSpell(qssMouseR, 1504);
			TargetRole:EquipSpell(qssKey1, 1510);
			TargetRole:EquipSpell(qssKey2, 1520);
			TargetRole:EquipSpell(qssKey3, 1518);
			TargetRole:EquipSpell(qssKey4, 1505);
			SetRune(1507, 6, 1);
			SetRune(1504, 4, 1);
			SetRune(1510, 5, 1);
			SetRune(1520, 4, 1);
			SetRune(1518, 2, 1);
			SetRune(1505, 4, 1);
		elseif WeaponType == "lr" then
			local itemPos=FindItemPosInEquipBag(10420321);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1506);
			TargetRole:EquipSpell(qssMouseR, 1513);
			TargetRole:EquipSpell(qssKey1, 1517);
			TargetRole:EquipSpell(qssKey2, 1521);
			TargetRole:EquipSpell(qssKey3, 1524);
			TargetRole:EquipSpell(qssKey4, 1522);
			SetRune(1506, 3, 1);
			SetRune(1513, 1, 1);
			SetRune(1517, 2, 1);
			SetRune(1521, 6, 1);
			SetRune(1524, 1, 1);
			SetRune(1522, 2, 1);
		end
	elseif class == clsArcher then
		if WeaponType == "g" then
			local itemPos=FindItemPosInEquipBag(10430321);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2001);
			TargetRole:EquipSpell(qssMouseR, 2008);
			TargetRole:EquipSpell(qssKey1, 2014);
			TargetRole:EquipSpell(qssKey2, 2009);
			TargetRole:EquipSpell(qssKey3, 2017);
			TargetRole:EquipSpell(qssKey4, 2020);
			SetRune(2001, 6, 1);
			SetRune(2008, 2, 1);
			SetRune(2014, 2, 1);
			SetRune(2009, 3, 1);
			SetRune(2017, 6, 1);
			SetRune(2020, 5, 1);
		elseif WeaponType == "sn" then
			local itemPos=FindItemPosInEquipBag(10430121);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2001);
			TargetRole:EquipSpell(qssMouseR, 2004);
			TargetRole:EquipSpell(qssKey1, 2006);
			TargetRole:EquipSpell(qssKey2, 2018);
			TargetRole:EquipSpell(qssKey3, 2019);
			TargetRole:EquipSpell(qssKey4, 2005);
			SetRune(2001, 5, 1);
			SetRune(2004, 3, 1);
			SetRune(2006, 4, 1);
			SetRune(2018, 6, 1);
			SetRune(2019, 4, 1);
			SetRune(2005, 5, 1);
		elseif WeaponType == "zn" then
			local itemPos=FindItemPosInEquipBag(10430221);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2001);
			TargetRole:EquipSpell(qssMouseR, 2012);
			TargetRole:EquipSpell(qssKey1, 2014);
			TargetRole:EquipSpell(qssKey2, 2015);
			TargetRole:EquipSpell(qssKey3, 2007);
			TargetRole:EquipSpell(qssKey4, 2011);
			SetRune(2001, 4, 1);
			SetRune(2012, 2, 1);
			SetRune(2014, 1, 1);
			SetRune(2015, 6, 1);
			SetRune(2007, 7, 1);
			SetRune(2011, 3, 1);
		end
	end

	SetLevel(25);



end

function SSKillTest(WeaponType)

	if type(WeaponType) ~= "string" then
		return
	end

	local class = TargetRole:GetClass();
	local bag = TargetRole:GetItemBag();
	if class == clsWarrior then
		if WeaponType == "jd" then
			local itemPos=FindItemPosInEquipBag(10410121);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1022);
			TargetRole:EquipSpell(qssMouseR, 1020);
			TargetRole:EquipSpell(qssKey1, 1003);
			TargetRole:EquipSpell(qssKey2, 1005);
			TargetRole:EquipSpell(qssKey3, 1014);
			TargetRole:EquipSpell(qssKey4, 1011);
			SetRune(1022, 1, 1);
			SetRune(1020, 7, 1);
			SetRune(1003, 7, 1);
			SetRune(1005, 4, 1);
			SetRune(1014, 5, 1);
			SetRune(1011, 1, 1);
		elseif WeaponType == "sf" then
			local itemPos=FindItemPosInEquipBag(10410321);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1022);
			TargetRole:EquipSpell(qssMouseR, 1009);
			TargetRole:EquipSpell(qssKey1, 1004);
			TargetRole:EquipSpell(qssKey2, 1010);
			TargetRole:EquipSpell(qssKey3, 1012);
			TargetRole:EquipSpell(qssKey4, 1016);
			SetRune(1022, 1, 1);
			SetRune(1009, 4, 1);
			SetRune(1004, 6, 1);
			SetRune(1010, 7, 1);
			SetRune(1012, 6, 1);
			SetRune(1016, 7, 1);
		elseif WeaponType == "jj" then
			local itemPos=FindItemPosInEquipBag(10410221);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1022);
			TargetRole:EquipSpell(qssMouseR, 1023);
			TargetRole:EquipSpell(qssKey1, 1002);
			TargetRole:EquipSpell(qssKey2, 1024);
			TargetRole:EquipSpell(qssKey3, 1025);
			TargetRole:EquipSpell(qssKey4, 1016);
			SetRune(1022, 1, 1);
			SetRune(1023, 5, 1);
			SetRune(1002, 2, 1);
			SetRune(1024, 5, 1);
			SetRune(1025, 2, 1);
			SetRune(1016, 6, 1);
		end
	elseif class == clsMage then
		if WeaponType == "fz" then
			local itemPos=FindItemPosInEquipBag(10440321);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2525);
			TargetRole:EquipSpell(qssMouseR, 2513);
			TargetRole:EquipSpell(qssKey1, 2526);
			TargetRole:EquipSpell(qssKey2, 2509);
			TargetRole:EquipSpell(qssKey3, 2503);
			TargetRole:EquipSpell(qssKey4, 2523);
			SetRune(2525, 2, 1);
			SetRune(2513, 2, 1);
			SetRune(2526, 2, 1);
			SetRune(2509, 9, 1);
			SetRune(2503, 1, 1);
			SetRune(2523, 3, 1);
		elseif WeaponType == "cd" then
			local itemPos=FindItemPosInEquipBag(10440221);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2501);
			TargetRole:EquipSpell(qssMouseR, 2515);
			TargetRole:EquipSpell(qssKey1, 2512);
			TargetRole:EquipSpell(qssKey2, 2517);
			TargetRole:EquipSpell(qssKey3, 2524);
			TargetRole:EquipSpell(qssKey4, 2505);
			SetRune(2501, 4, 1);
			SetRune(2515, 1, 1);
			SetRune(2512, 5, 1);
			SetRune(2517, 2, 1);
			SetRune(2524, 6, 1);
			SetRune(2505, 5, 1);
		elseif WeaponType == "fd" then
			local itemPos=FindItemPosInEquipBag(10440121);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2501);
			TargetRole:EquipSpell(qssMouseR, 2513);
			TargetRole:EquipSpell(qssKey1, 2526);
			TargetRole:EquipSpell(qssKey2, 2516);
			TargetRole:EquipSpell(qssKey3, 2524);
			TargetRole:EquipSpell(qssKey4, 2510);
			SetRune(2501, 3, 1);
			SetRune(2513, 4, 1);
			SetRune(2526, 5, 1);
			SetRune(2516, 2, 1);
			SetRune(2524, 4, 1);
			SetRune(2510, 1, 1);
		end
	elseif class == clsRogue then
		if WeaponType == "bs" then
			local itemPos=FindItemPosInEquipBag(10420221);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1508);
			TargetRole:EquipSpell(qssMouseR, 1502);
			TargetRole:EquipSpell(qssKey1, 1509);
			TargetRole:EquipSpell(qssKey2, 1519);
			TargetRole:EquipSpell(qssKey3, 1524);
			TargetRole:EquipSpell(qssKey4, 1512);
			SetRune(1508, 1, 1);
			SetRune(1502, 2, 1);
			SetRune(1509, 7, 1);
			SetRune(1519, 3, 1);
			SetRune(1524, 3, 1);
			SetRune(1512, 2, 1);
		elseif WeaponType == "cq" then
			local itemPos=FindItemPosInEquipBag(10420121);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1507);
			TargetRole:EquipSpell(qssMouseR, 1504);
			TargetRole:EquipSpell(qssKey1, 1510);
			TargetRole:EquipSpell(qssKey2, 1520);
			TargetRole:EquipSpell(qssKey3, 1518);
			TargetRole:EquipSpell(qssKey4, 1505);
			SetRune(1507, 6, 1);
			SetRune(1504, 4, 1);
			SetRune(1510, 5, 1);
			SetRune(1520, 4, 1);
			SetRune(1518, 2, 1);
			SetRune(1505, 4, 1);
		elseif WeaponType == "lr" then
			local itemPos=FindItemPosInEquipBag(10420321);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 1506);
			TargetRole:EquipSpell(qssMouseR, 1513);
			TargetRole:EquipSpell(qssKey1, 1517);
			TargetRole:EquipSpell(qssKey2, 1521);
			TargetRole:EquipSpell(qssKey3, 1524);
			TargetRole:EquipSpell(qssKey4, 1522);
			SetRune(1506, 3, 1);
			SetRune(1513, 1, 1);
			SetRune(1517, 2, 1);
			SetRune(1521, 6, 1);
			SetRune(1524, 1, 1);
			SetRune(1522, 2, 1);
		end
	elseif class == clsArcher then
		if WeaponType == "g" then
			local itemPos=FindItemPosInEquipBag(10430321);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2001);
			TargetRole:EquipSpell(qssMouseR, 2008);
			TargetRole:EquipSpell(qssKey1, 2014);
			TargetRole:EquipSpell(qssKey2, 2009);
			TargetRole:EquipSpell(qssKey3, 2017);
			TargetRole:EquipSpell(qssKey4, 2020);
			SetRune(2001, 6, 1);
			SetRune(2008, 2, 1);
			SetRune(2014, 2, 1);
			SetRune(2009, 3, 1);
			SetRune(2017, 6, 1);
			SetRune(2020, 5, 1);
		elseif WeaponType == "sn" then
			local itemPos=FindItemPosInEquipBag(10430121);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2001);
			TargetRole:EquipSpell(qssMouseR, 2004);
			TargetRole:EquipSpell(qssKey1, 2006);
			TargetRole:EquipSpell(qssKey2, 2018);
			TargetRole:EquipSpell(qssKey3, 2019);
			TargetRole:EquipSpell(qssKey4, 2005);
			SetRune(2001, 5, 1);
			SetRune(2004, 3, 1);
			SetRune(2006, 4, 1);
			SetRune(2018, 6, 1);
			SetRune(2019, 4, 1);
			SetRune(2005, 5, 1);
		elseif WeaponType == "zn" then
			local itemPos=FindItemPosInEquipBag(10430221);
			if itemPos then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_MAIN_HAND);
			end
			TargetRole:EquipSpell(qssMouseL, 2001);
			TargetRole:EquipSpell(qssMouseR, 2012);
			TargetRole:EquipSpell(qssKey1, 2014);
			TargetRole:EquipSpell(qssKey2, 2015);
			TargetRole:EquipSpell(qssKey3, 2007);
			TargetRole:EquipSpell(qssKey4, 2011);
			SetRune(2001, 4, 1);
			SetRune(2012, 2, 1);
			SetRune(2014, 1, 1);
			SetRune(2015, 6, 1);
			SetRune(2007, 7, 1);
			SetRune(2011, 3, 1);
		end
	end

end


function ChangeEquipment(Index, MatchType)
	if type(Index) ~= "number" then
		return
	end
	if type(MatchType) == "number" and MatchType > 0 then
		MatchType = 1
	else
		MatchType = 0
	end

	if Index <= 0 or Index > 8 then
		return
	end

	local LVRequest = 10 + (Index -1)*5	;
	if Index == 8 then
		LVRequest = 50;
	end
	LVRequest = LVRequest + math.random(0,2);
	if LVRequest > 50 then
		LVRequest = 50;
	end
	if LVRequest == 25 then
		LVRequest = 26;
	end

	SetLevel(LVRequest);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catMaxPhyAtk, 100);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catMaxMagAtk, 100);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catMaxTaoAtk, 100);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catWeaponWeightLmt, 500);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catArmorWeightLmt, 500);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catBagWeightLmt, 500);

	local WarriorEquip = {
		--Set 01, level 10
		{
			--Male
			{
				{
					99900005, --Short sword
					99990111, --Bronze helmet
					0,
					99980113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900005, --Short sword
					99990111, --Bronze helmet
					0,
					99990113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		--Set 02, level 15
		{
			--Male
			{
				{
					99900010, --Eight Wastes
					99990211, --Magic helmet
					0,
					99980213, --Light armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920005, --Black crystal necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900010, --Eight Wastes
					99990211, --Magic helmet
					0,
					99990213, --Light armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920005, --Black crystal necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		--Set 03, level 20
		{
			--Male
			{
				{
					99900034, --Blade of Prayer
					99990311, --Skull helmet
					0,
					99980313, --Medium armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					69900004, --Junyi ring
					99930008, --Black crystal ring
					99920011, --Devil necklace
					99940027, --Magic bracelet
					99940027  --Magic bracelet
				}
			},
			--Female
			{
				{
					99900034, --Blade of Prayer
					99990311, --Skull helmet
					0,
					99990313, --Medium armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					69900004, --Junyi ring
					99930008, --Black crystal ring
					99920011, --Devil necklace
					99940027, --Magic bracelet
					99940027  --Magic bracelet
				}

			}
		},
		--Set 04, level 26
		{
			--Male
			{
				{
					99900019, --Purgatory
					99990511, --Black iron helmet
					0,
					99980513, --War God armor
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					0
				},
				{
					99930019, --Strength ring
					99930019, --Strength ring
					99920017, --Green necklace
					99940012, --Knight bracelet
					99940012  --Knight bracelet
				}
			},
			--Female
			{
				{
					99900019, --Purgatory
					99990511, --Black iron helmet
					0,
					99990513, --War God armor
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					0
				},
				{
					99930019, --Strength ring
					99930019, --Strength ring
					99920017, --Green necklace
					99940012, --Knight bracelet
					99940012  --Knight bracelet
				}
			}
		},
		--Set 05, level 30
		{
			--Male
			{
				{
					99900022, --Moon in the Well
					99990511, --Black iron helmet
					0,
					99980513, --War God armor
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930019, --Strength ring
					99930019, --Strength ring
					99920017, --Green necklace
					99940012, --Knight bracelet
					99940012  --Knight bracelet
				}
			},
			--Female
			{
				{
					99900022, --Moon in the Well
					99990511, --Black iron helmet
					0,
					99990513, --War God armor
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930019, --Strength ring
					99930019, --Strength ring
					99920017, --Green necklace
					99940012, --Knight bracelet
					99940012  --Knight bracelet
				}
			}
		},
		--Set 06, level 35
		{
			--Male
			{
				{
					99900022, --Moon in the Well
					99990511, --Black iron helmet
					0,
					99980513, --War God armor
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930019, --Strength ring
					99930019, --Strength ring
					99920017, --Green necklace
					99940012, --Knight bracelet
					99940012  --Knight bracelet
				}
			},
			--Female
			{
				{
					99900022, --Moon in the Well
					99990511, --Black iron helmet
					0,
					99990513, --War God armor
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930019, --Strength ring
					99930019, --Strength ring
					99920017, --Green necklace
					99940012, --Knight bracelet
					99940012  --Knight bracelet
				}
			}
		},
		--Set 07, level 40
		{
			--Male
			{
				{
					99900025, --Judgment
					99990511, --Black iron helmet
					0,
					99980513, --War God armor
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930019, --Strength ring
					99930019, --Strength ring
					99920017, --Green necklace
					99940012, --Knight bracelet
					99940012  --Knight bracelet
				}
			},
			--Female
			{
				{
					99900025, --Judgment
					99990511, --Black iron helmet
					0,
					99990513, --Holy War armor
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930019, --Strength ring
					99930019, --Strength ring
					99920017, --Green necklace
					99940012, --Knight bracelet
					99940012  --Knight bracelet
				}
			}
		},
		--Set 08, level 50
		{
			--Male
			{
				{
					99900026, --Dragon Slayer
					99990611, --Holy War helmet
					0,
					99980613, --Heavenly Demon armor
					0,
					99990515, --Bronze belt
					0,
					99990517, --Purple silk boots
					99950004, --Heavenly Demon Fury
				},
				{
					99930033, --Holy War ring
					99930033, --Holy War ring
					99920027, --Holy War necklace
					99940024, --Holy War bracelet
					99940024  --Holy War bracelet
				}
			},
			--Female
			{
				{
					99900026, --Dragon Slayer
					99990611, --Holy War helmet
					0,
					99990613, --Holy War armor
					0,
					99990515, --Bronze belt
					0,
					99990517, --Purple silk boots
					99950004, --Heavenly Demon Fury
				},
				{
					99930033, --Holy War ring
					99930033, --Holy War ring
					99920027, --Holy War necklace
					99940024, --Holy War bracelet
					99940024  --Holy War bracelet
				}
			}
		}
	};
	local MageEquip = {
		-- Set 01, level 10
		{
			--Male
			{
				{
					99900003, --Ebony sword
					0,
					0,
					99980113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930003, --Hexagonal ring
					99930003, --Hexagonal ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900003, --Ebony sword
					0, 		  --Magic headscarf     99990121
					0, 		  --Soft cloth shoulder armor 99990122
					99990113, --Cloth armor     99980123  robe <cloth armor in Legend>
					0,  	  --99990124 Cloth leggings <not in Legend>
					0, 		  --Silver-trimmed belt 99990125
					0, 		  --Fine cloth bracers 99990126
					0,		  --99990127 Tanned leather boots <not in Legend>
					0 		  --Thin cotton cloak 99990128
				},
				{
					99930003, --Hexagonal ring
					99930003, --Hexagonal ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		-- Set 02, level 15
		{
			--Male
			{
				{
					99900009, --Sea Soul
					99990221, --Magic helmet
					0,
					99980213, --Light armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930006, --Cast iron ring
					99930006, --Cast iron ring
					99920006, --Platinum necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900009, --Sea Soul
					99990221, --Magic helmet
					0,
					99990213, --Light armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930006, --Cast iron ring
					99930006, --Cast iron ring
					99920006, --Platinum necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		-- Set 03, level 20
		{
			--Male
			{
				{
					99900013, --Crescent Moon
					99990221, --Magic helmet
					0,
					99980313, --Medium armor
					0,
					99990315,
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930009, --Snake eye ring
					99930009, --Snake eye ring
					99920010, --Amber necklace
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			},
			--Female
			{
				{
					99900013, --Crescent Moon
					99990221, --Magic helmet
					0,
					99990313, --Medium armor
					0,
					99990315,
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930009, --Snake eye ring
					99930009, --Snake eye ring
					99920010, --Amber necklace
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			}
		},
		-- Set 04, level 25
		{
			--Male
			{
				{
					99900018, --Magic wand
					99910001, --Memory helmet
					0,
					99980523, --Demon robe
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					0
				},
				{
					99930020, --Purple Spiral
					99930020, --Purple Spiral
					99920019, --Demon bell
					99940016, --Dragon bracelet
					99940016  --Dragon bracelet
				}
			},
			--Female
			{
				{
					99900018, --Magic wand
					99910001, --Memory helmet
					0,
					99990523, --Demon robe
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					0
				},
				{
					99930020, --Purple Spiral
					99930020, --Purple Spiral
					99920019, --Demon bell
					99940016, --Dragon bracelet
					99940016  --Dragon bracelet
				}
			}
		},
		-- Set 05, level 30
		{
			--Male
			{
				{
					99900023, --Blood Drinker
					99910001, --Memory helmet
					0,
					99980523, --Demon robe
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930020, --Purple Spiral
					99930020, --Purple Spiral
					99920019, --Demon bell
					99940016, --Dragon bracelet
					99940016  --Dragon bracelet
				}
			},
			--Female
			{
				{
					99900023, --Blood Drinker
					99910001, --Memory helmet
					0,
					99990523, --Demon robe
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930020, --Purple Spiral
					99930020, --Purple Spiral
					99920019, --Demon bell
					99940016, --Dragon bracelet
					99940016  --Dragon bracelet
				}
			}
		},
		-- Set 06, level 35
		{
			--Male
			{
				{
					99900023, --Blood Drinker
					99910001, --Memory helmet
					0,
					99980523, --Demon robe
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930020, --Purple Spiral
					99930020, --Purple Spiral
					99920019, --Demon bell
					99940016, --Dragon bracelet
					99940016  --Dragon bracelet
				}
			},
			--Female
			{
				{
					99900023, --Blood Drinker
					99910001, --Memory helmet
					0,
					99990523, --Demon robe
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930020, --Purple Spiral
					99930020, --Purple Spiral
					99920019, --Demon bell
					99940016, --Dragon bracelet
					99940016  --Dragon bracelet
				}
			}
		},
		-- Set 07, level 40
		{
			--Male
			{
				{
					99900028, --Bone jade scepter
					99910001, --Memory helmet
					0,
					99980523, --Demon robe
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930020, --Purple Spiral
					99930020, --Purple Spiral
					99920019, --Demon bell
					99940016, --Dragon bracelet
					99940016  --Dragon bracelet
				}
			},
			--Female
			{
				{
					99900028, --Bone jade scepter
					99910001, --Memory helmet
					0,
					99990523, --Demon robe
					0,
					99990415, --Iron belt
					0,
					99990417, --Deerskin boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930020, --Purple Spiral
					99930020, --Purple Spiral
					99920019, --Demon bell
					99940016, --Dragon bracelet
					99940016  --Dragon bracelet
				}
			}
		},
		-- Set 08, level 50
		{
			--Male
			{
				{
					99900029, --Soul Devouring staff
					99990621, --Magic God helmet
					0,
					99980623, --Magic God cloak
					0,
					99990515, --Bronze belt
					0,
					99990517, --Purple silk boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930034, --Magic God ring
					99930034, --Magic God ring
					99920028, --Magic God necklace
					99940025, --Magic God bracelet
					99940025  --Magic God bracelet
				}
			},
			--Female
			{
				{
					99900029, --Soul Devouring staff
					99990621, --Magic God helmet
					0,
					99990623, --Feathered robe
					0,
					99990515, --Bronze belt
					0,
					99990517, --Purple silk boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930034, --Magic God ring
					99930034, --Magic God ring
					99920028, --Magic God necklace
					99940025, --Magic God bracelet
					99940025  --Magic God bracelet
				}
			}
		}
	};
	local TaoistEquip = {
		--Set 01, level 10
		{
			--Male
			{
				{
					99900005, --Short sword
					99990111, --Bronze helmet
					0,
					99980113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930002, --Glass ring
					99930002, --Glass ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900005, --Short sword
					99990111, --Bronze helmet
					0,
					99990113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930002, --Glass ring
					99930002, --Glass ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		--Set 02, level 15
		{
			--Male
			{
				{
					99900011, --Half Moon
					99990211, --Magic helmet
					0,
					99990213, --Light armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930002, --Glass ring
					99930002, --Glass ring
					99920007, --White tiger tooth necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900011, --Half Moon
					99990211, --Magic helmet
					0,
					99990213, --Light armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930002, --Glass ring
					99930002, --Glass ring
					99920007, --White tiger tooth necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		--Set 03, level 20
		{
			--Male
			{
				{
					99900015, --Demon Subduer
					99990211, --Magic helmet
					0,
					99980313, --Medium armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930007, --Pearl ring
					99930007, --Pearl ring
					99920012, --Phoenix pearl
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			},
			--Female
			{
				{
					99900015, --Demon Subduer
					99990211, --Magic helmet
					0,
					99990313, --Medium armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930007, --Pearl ring
					99930007, --Pearl ring
					99920012, --Phoenix pearl
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			}
		},
		--Set 04, level 25
		{
			--Male
			{
				{
					99900024, --Wuji staff
					99990411, --Taoist helmet
					0,
					99980533, --Ghost armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930021, --Titan ring
					99930021, --Titan ring
					99920018, --Soul necklace
					99940010, --Three-eye bracelet
					99940010  --Three-eye bracelet
				}
			},
			--Female
			{
				{
					99900024, --Wuji staff
					99990411, --Taoist helmet
					0,
					99990533, --Ghost armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					0
				},
				{
					99930021, --Titan ring
					99930021, --Titan ring
					99920018, --Soul necklace
					99940010, --Three-eye bracelet
					99940010  --Three-eye bracelet
				}
			}
		},
		--Set 05, level 30
		{
			--Male
			{
				{
					99900024, --Wuji staff
					99910001, --Memory helmet
					0,
					99980533, --Ghost armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930021, --Titan ring
					99930021, --Titan ring
					99920018, --Soul necklace
					99940010, --Three-eye bracelet
					99940010  --Three-eye bracelet
				}
			},
			--Female
			{
				{
					99900024, --Wuji staff
					99910001, --Memory helmet
					0,
					99990533, --Ghost armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930021, --Titan ring
					99930021, --Titan ring
					99920018, --Soul necklace
					99940010, --Three-eye bracelet
					99940010  --Three-eye bracelet
				}
			}
		},
		--Set 06, level 35
		{
			--Male
			{
				{
					99900024, --Wuji staff
					99910001, --Memory helmet
					0,
					99980533, --Ghost armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930021, --Titan ring
					99930021, --Titan ring
					99920018, --Soul necklace
					99940010, --Three-eye bracelet
					99940010  --Three-eye bracelet
				}
			},
			--Female
			{
				{
					99900024, --Wuji staff
					99910001, --Memory helmet
					0,
					99990533, --Ghost armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930021, --Titan ring
					99930021, --Titan ring
					99920018, --Soul necklace
					99940010, --Three-eye bracelet
					99940010  --Three-eye bracelet
				}
			}
		},
		--Set 07, level 40
		{
			--Male
			{
				{
					99900027, --Dragon-pattern sword
					99910001, --Memory helmet
					0,
					99980533, --Ghost armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930021, --Titan ring
					99930021, --Titan ring
					99920018, --Soul necklace
					99940010, --Three-eye bracelet
					99940010  --Three-eye bracelet
				}
			},
			--Female
			{
				{
					99900027, --Dragon-pattern sword
					99910001, --Memory helmet
					0,
					99990533, --Ghost armor
					0,
					99990315, --Hide belt
					0,
					99990317, --Cloth shoes
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930021, --Titan ring
					99930021, --Titan ring
					99920018, --Soul necklace
					99940010, --Three-eye bracelet
					99940010  --Three-eye bracelet
				}
			}
		},
		--Set 08, level 50
		{
			--Male
			{
				{
					99900031, --Free Spirit fan
					99990631, --Heavenly Venerable helmet
					0,
					99980633, --Heavenly Venerable robe
					0,
					99990515, --Bronze belt
					0,
					99990517, --Purple silk boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930035, --Heavenly Venerable ring
					99930035, --Heavenly Venerable ring
					99920029, --Heavenly Venerable necklace
					99940026, --Heavenly Venerable bracelet
					99940026  --Heavenly Venerable bracelet
				}
			},
			--Female
			{
				{
					99900031, --Free Spirit fan
					99990631, --Heavenly Venerable helmet
					0,
					99990633, --Celestial Master robe
					0,
					99990515, --Bronze belt
					0,
					99990517, --Purple silk boots
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930035, --Heavenly Venerable ring
					99930035, --Heavenly Venerable ring
					99920029, --Heavenly Venerable necklace
					99940026, --Heavenly Venerable bracelet
					99940026  --Heavenly Venerable bracelet
				}
			}
		}
	};

	local WarriorEquip_AVG = {
		--Set 01, level 10
		{
			--Male
			{
				{
					99900006, --Iron sword
					99990111, --Bronze helmet
					0,
					99980113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900006, --Iron sword
					99990111, --Bronze helmet
					0,
					99990113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		--Set 02, level 15
		{
			--Male
			{
				{
					99900010, --Eight Wastes
					99990211, --Magic helmet
					0,
					99980213, --Light armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900010, --Eight Wastes
					99990211, --Magic helmet
					0,
					99990213, --Light armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		--Set 03, level 20
		{
			--Male
			{
				{
					99900016, --Horse-slaying blade
					99990211, --Magic helmet
					0,
					99980313, --Medium armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930008, --Black crystal ring
					99930008, --Black crystal ring
					99920011, --Devil necklace
					99940027, --Magic bracelet
					99940027  --Magic bracelet
				}
			},
			--Female
			{
				{
					99900016, --Horse-slaying blade
					99990211, --Magic helmet
					0,
					99990313, --Medium armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930008, --Black crystal ring
					99930008, --Black crystal ring
					99920011, --Devil necklace
					99940027, --Magic bracelet
					99940027  --Magic bracelet
				}

			}
		},
		--Set 04, level 26
		{
			--Male
			{
				{
					99900020, --Frost
					99990311, --Skull helmet
					0,
					99980413, --Heavy armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930016, --Coral ring
					99930016, --Coral ring
					99920013, --Blue jade necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			},
			--Female
			{
				{
					99900020, --Frost
					99990311, --Skull helmet
					0,
					99990413, --Heavy armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930016, --Coral ring
					99930016, --Coral ring
					99920013, --Blue jade necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			}
		},
		--Set 05, level 30
		{
			--Male
			{
				{
					99900020, --Frost
					99990311, --Skull helmet
					0,
					99980413, --Heavy armor
					0,
					0,
					0,
					0,
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930016, --Coral ring
					69900008, --Boss ring
					99920013, --Blue jade necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			},
			--Female
			{
				{
					99900020, --Frost
					99990311, --Skull helmet
					0,
					99990413, --Heavy armor
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930016, --Coral ring
					69900008, --Boss ring
					99920013, --Blue jade necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			}
		},
		--Set 06, level 35
		{
			--Male
			{
				{
					99900020, --Frost
					99990311, --Skull helmet
					0,
					99980413, --Heavy armor
					0,
					0,
					0,
					0,
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930018, --Dragon ring
					69900008, --Boss ring
					99920013, --Blue jade necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			},
			--Female
			{
				{
					99900020, --Frost
					99990311, --Skull helmet
					0,
					99990413, --Heavy armor
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930018, --Dragon ring
					69900008, --Boss ring
					99920013, --Blue jade necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			}
		},
		--Set 07, level 40
		{
			--Male
			{
				{
					99900019, --Purgatory
					99990311, --Skull helmet
					0,
					99980413, --Heavy armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930018, --Dragon ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			},
			--Female
			{
				{
					99900022, --Moon in the Well
					99990311, --Skull helmet
					0,
					99990413, --Heavy armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930018, --Dragon ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			}
		},
		--Set 08, level 50
		{
			--Male
			{
				{
					99900019, --Purgatory
					99990311, --Skull helmet
					0,
					99980513, --War God armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930018, --Dragon ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			},
			--Female
			{
				{
					99900019, --Purgatory
					99990311, --Skull helmet
					0,
					99990513, --War God armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930018, --Dragon ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940019, --Death gloves
					99940019  --Death gloves
				}
			}
		}
	};
	local MageEquip_AVG = {
		-- Set 01, level 10
		{
			--Male
			{
				{
					99900003, --Ebony sword
					99990111, --Bronze helmet
					0,
					99980113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930003, --Hexagonal ring
					99930003, --Hexagonal ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900003, --Ebony sword
					99990111, --Bronze helmet
					0,
					99990113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930003, --Hexagonal ring
					99930003, --Hexagonal ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		-- Set 02, level 15
		{
			--Male
			{
				{
					99900009, --Sea Soul
					99990221, --Magic helmet
					0,
					99980213, --Light armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930003, --Hexagonal ring
					99930003, --Hexagonal ring
					99920004, --Ebony necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900009, --Sea Soul
					99990221, --Magic helmet
					0,
					99990213, --Light armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930003, --Hexagonal ring
					99930003, --Hexagonal ring
					99920004, --Ebony necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		-- Set 03, level 20
		{
			--Male
			{
				{
					99900013, --Crescent Moon
					99990211, --Magic helmet
					0,
					99980313, --Medium armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930009, --Snake eye ring
					99930009, --Snake eye ring
					99920006, --Platinum necklace
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			},
			--Female
			{
				{
					99900013, --Crescent Moon
					99990211, --Magic helmet
					0,
					99990313, --Medium armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930009, --Snake eye ring
					99930009, --Snake eye ring
					99920006, --Platinum necklace
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			}
		},
		-- Set 04, level 25
		{
			--Male
			{
				{
					99900013, --Crescent Moon
					99990411, --Taoist helmet
					0,
					99980423, --Magic robe
					0,
					0,
					0,
					0,
					0
				},
				{
					99930013, --Demon Slayer ring
					99930013, --Demon Slayer ring
					99920015, --Magnifying glass
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			},
			--Female
			{
				{
					99900013, --Crescent Moon
					99990411, --Taoist helmet
					0,
					99990423, --Magic robe
					0,
					0,
					0,
					0,
					0
				},
				{
					99930013, --Demon Slayer ring
					99930013, --Demon Slayer ring
					99920015, --Magnifying glass
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			}
		},
		-- Set 05, level 30
		{
			--Male
			{
				{
					69910042, --Magic wand
					99990411, --Taoist helmet
					0,
					99980423, --Magic robe
					0,
					0,
					0,
					0,
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930013, --Demon Slayer ring
					69900006, --Coiled Dragon ring
					99920015, --Magnifying glass
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			},
			--Female
			{
				{
					99900018, --Magic wand
					99990411, --Taoist helmet
					0,
					99990423, --Magic robe
					0,
					0,
					0,
					0,
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930013, --Demon Slayer ring
					69900008, --Boss ring
					99920015, --Magnifying glass
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			}
		},
		-- Set 06, level 35
		{
			--Male
			{
				{
					69910042, --Blood Sacrifice
					99990411, --Taoist helmet
					0,
					99980423, --Magic robe
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930017, --Ruby ring
					69900008, --Boss ring
					99920015, --Magnifying glass
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			},
			--Female
			{
				{
					69910042, --Blood Sacrifice
					99990411, --Taoist helmet
					0,
					99990423, --Magic robe
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930017, --Ruby ring
					69900008, --Boss ring
					99920015, --Magnifying glass
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			}
		},
		-- Set 07, level 40
		{
			--Male
			{
				{
					99900018, --Magic wand
					99990411, --Taoist helmet
					0,
					99980423, --Magic robe
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930017, --Ruby ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			},
			--Female
			{
				{
					99900018, --Magic wand
					99990411, --Taoist helmet
					0,
					99990423, --Magic robe
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930017, --Ruby ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			}
		},
		-- Set 08, level 50
		{
			--Male
			{
				{
					99900018, --Magic wand
					99990411, --Taoist helmet
					0,
					99980523, --Demon robe
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930017, --Ruby ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			},
			--Female
			{
				{
					99900018, --Magic wand
					99990411, --Taoist helmet
					0,
					99990523, --Demon robe
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930017, --Ruby ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940008, --Ebony bracelet
					99940008  --Ebony bracelet
				}
			}
		}
	};
	local TaoistEquip_AVG = {
		--Set 01, level 10
		{
			--Male
			{
				{
					99900006, --Iron sword
					99990111, --Bronze helmet
					0,
					99980113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900006, --Iron sword
					99990111, --Bronze helmet
					0,
					99990113, --Cloth armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		--Set 02, level 15
		{
			--Male
			{
				{
					99900011, --Half Moon
					99990211, --Magic helmet
					0,
					99990213, --Light armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			},
			--Female
			{
				{
					99900011, --Half Moon
					99990211, --Magic helmet
					0,
					99990213, --Light armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930004, --Horn ring
					99930004, --Horn ring
					99920001, --Gold necklace
					99940004, --Large bracelet
					99940004  --Large bracelet
				}
			}
		},
		--Set 03, level 20
		{
			--Male
			{
				{
					99900015, --Demon Subduer
					99990211, --Magic helmet
					0,
					99980313, --Medium armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930007, --Pearl ring
					99930007, --Pearl ring
					99920012, --Phoenix pearl
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			},
			--Female
			{
				{
					99900015, --Demon Subduer
					99990211, --Magic helmet
					0,
					99990313, --Medium armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930007, --Pearl ring
					99930007, --Pearl ring
					99920012, --Phoenix pearl
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			}
		},
		--Set 04, level 25
		{
			--Male
			{
				{
					99900015, --Demon Subduer
					99990431, --Taoist helmet
					0,
					99980433, --Soul armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930013, --Demon Slayer ring
					99930013, --Demon Slayer ring
					99920014, --Bamboo flute
					99940019, --Death gloves
					99940019  --Death gloves
				}
			},
			--Female
			{
				{
					99900015, --Demon Subduer
					99990431, --Taoist helmet
					0,
					99990433, --Soul armor
					0,
					0,
					0,
					0,
					0
				},
				{
					99930013, --Demon Slayer ring
					99930013, --Demon Slayer ring
					99920014, --Bamboo flute
					99940019, --Death gloves
					99940019  --Death gloves
				}
			}
		},
		--Set 05, level 30
		{
			--Male
			{
				{
					99900015, --Demon Subduer
					99990431, --Taoist helmet
					0,
					99980433, --Soul armor
					0,
					0,
					0,
					0,
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930013, --Demon Slayer ring
					69900006, --Coiled Dragon ring
					99920014, --Bamboo flute
					99940019, --Death gloves
					99940019  --Death gloves
				}
			},
			--Female
			{
				{
					99900015, --Demon Subduer
					99990431, --Taoist helmet
					0,
					99990433, --Soul armor
					0,
					0,
					0,
					0,
					99950003, --Dragon Sigh: Splendid
				},
				{
					99930013, --Demon Slayer ring
					69900006, --Coiled Dragon ring
					99920014, --Bamboo flute
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			}
		},
		--Set 06, level 35
		{
			--Male
			{
				{
					99900015, --Demon Subduer
					99990411, --Taoist helmet
					0,
					99980433, --Soul armor
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930014, --Platinum ring
					69900008, --Boss ring
					99920014, --Bamboo flute
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			},
			--Female
			{
				{
					99900015, --Demon Subduer
					99990411, --Taoist helmet
					0,
					99990433, --Soul armor
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930014, --Platinum ring
					69900008, --Boss ring
					99920014, --Bamboo flute
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			}
		},
		--Set 07, level 40
		{
			--Male
			{
				{
					99900021, --Silver Serpent
					99990411, --Taoist helmet
					0,
					99980433, --Soul armor
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930014, --Platinum ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			},
			--Female
			{
				{
					99900021, --Silver Serpent
					99990411, --Taoist helmet
					0,
					99990433, --Soul armor
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930014, --Platinum ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			}
		},
		--Set 08, level 50
		{
			--Male
			{
				{
					99900021, --Silver Serpent
					99990411, --Taoist helmet
					0,
					99980533, --Ghost armor
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930014, --Platinum ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			},
			--Female
			{
				{
					99900021, --Silver Serpent
					99990411, --Taoist helmet
					0,
					99990533, --Ghost armor
					0,
					0,
					0,
					0,
					99950005, --Dragon Sigh: Dazzling
				},
				{
					99930014, --Platinum ring
					69900008, --Boss ring
					69900009, --Heavenly Dragon necklace
					99940007, --Taoist bracelet
					99940007  --Taoist bracelet
				}
			}
		}
	};


	local FKeyTable = {};
	FKeyTable[1] = qssKey0;
	FKeyTable[2] = qssKey1;
	FKeyTable[3] = qssKey2;
	FKeyTable[4] = qssKey3;
	FKeyTable[5] = qssKey4;
	FKeyTable[6] = qssKey5;
	FKeyTable[7] = qssKey6;
	FKeyTable[8] = qssKey7;
	FKeyTable[9] = qssKey8;
	FKeyTable[10] = qssKey9;
	FKeyTable[11] = qssKey10;
	FKeyTable[12] = qssKey11;

	local WarriorSkillList = {};
	WarriorSkillList[7] = 1031; -- Basic Swordsmanship
	WarriorSkillList[19] = 1032; -- Attack Swordsmanship
	WarriorSkillList[25] = 1033; -- Thrust Swordsmanship
	WarriorSkillList[28] = 1034; -- Half Moon Slash
	WarriorSkillList[30] = 1035; -- Barbaric Charge
	WarriorSkillList[35] = 1036; -- Blazing Swordsmanship
	--WarriorSkillList[38] = 1037; -- Lion Roar
	--WarriorSkillList[47] = 1038; -- Sun Chasing Swordsmanship

	local MageSkillList = {};
	MageSkillList[7] = 2531; -- Fireball
	MageSkillList[12] = 2532; -- Repelling Fire Ring
	MageSkillList[13] = 2541; -- Light of Temptation
	MageSkillList[16] = 2544; -- Hellfire
	MageSkillList[17] = 2533; -- Lightning
	MageSkillList[19] = 2538; -- Teleport
	MageSkillList[20] = 2549; -- Greater Fireball
	MageSkillList[22] = 2545; -- Exploding Flame
	MageSkillList[24] = 2534; -- Fire Wall
	MageSkillList[26] = 2536; -- Lightning Beam
	MageSkillList[30] = 2546; -- Hell Lightning
	MageSkillList[31] = 2535; -- Magic Shield
	MageSkillList[32] = 2547; -- Holy Word
	MageSkillList[35] = 2537; -- Ice Roar
	--MageSkillList[36] = 2550; -- Ice Palm
	--MageSkillList[38] = 2539; -- Heavenly Fire
	--MageSkillList[47] = 2540; -- Meteor Shower

	local TaoistSkillList = {};
	TaoistSkillList[7] = 3002; -- Heal
	TaoistSkillList[9] = 3001; -- Spiritual Combat
	TaoistSkillList[14] = 3004; -- Poison
	TaoistSkillList[18] = 3005; -- Soul Fire Talisman
	TaoistSkillList[19] = 3003; -- Summon Undead
	TaoistSkillList[20] = 3009; -- Invisibility
	TaoistSkillList[21] = 3014; -- Group Invisibility
	TaoistSkillList[22] = 3006; -- Ghost Shield
	TaoistSkillList[25] = 3007; -- Holy Armor
	TaoistSkillList[28] = 3011; -- Demon Trap
	--TaoistSkillList[29] = 3018; -- Qi Wave
	TaoistSkillList[33] = 3012; -- Group Heal
	TaoistSkillList[35] = 3008; -- Summon Divine Beast
	--TaoistSkillList[36] = 3015; -- Wuji Energy
	--TaoistSkillList[47] = 3010; -- Blood Drain

	local WarriorSkillAutoEquipList = {};
	WarriorSkillAutoEquipList[25] = {1033,1};
	WarriorSkillAutoEquipList[28] = {1034,2};
	WarriorSkillAutoEquipList[30] = {1035,3};
	WarriorSkillAutoEquipList[35] = {1036,4};
	--WarriorSkillAutoEquipList[38] = {1037,5};
	--WarriorSkillAutoEquipList[47] = {1038,6};
	local MageSkillAutoEquipList = {};
	MageSkillAutoEquipList[7] = {2531,1};
	MageSkillAutoEquipList[12] = {2532,2};
	MageSkillAutoEquipList[13] = {2541,3};
	MageSkillAutoEquipList[16] = {2544,4};
	MageSkillAutoEquipList[17] = {2533,5};
	MageSkillAutoEquipList[19] = {2538,6};
	MageSkillAutoEquipList[20] = {2549,7};
	MageSkillAutoEquipList[22] = {2545,8};
	MageSkillAutoEquipList[24] = {2534,9};
	MageSkillAutoEquipList[26] = {2536,10};
	MageSkillAutoEquipList[30] = {2546,11};
	MageSkillAutoEquipList[31] = {2535,12};
	local TaoistSkillAutoEquipList = {};
	TaoistSkillAutoEquipList[7] = {3002,1};
	TaoistSkillAutoEquipList[14] = {3004,2};
	TaoistSkillAutoEquipList[18] = {3005,3};
	TaoistSkillAutoEquipList[19] = {3003,4};
	TaoistSkillAutoEquipList[20] = {3009,5};
	TaoistSkillAutoEquipList[21] = {3014,6};
	TaoistSkillAutoEquipList[22] = {3006,7};
	TaoistSkillAutoEquipList[25] = {3007,8};
	TaoistSkillAutoEquipList[28] = {3011,9};
	--TaoistSkillAutoEquipList[29] = {3018,10};
	TaoistSkillAutoEquipList[33] = {3012,11};
	TaoistSkillAutoEquipList[35] = {3008,12};

	local WarriorSkillLevelIndex = {};
	WarriorSkillLevelIndex[10] = {{1031,0,1}};
	WarriorSkillLevelIndex[15] = {{1031,1,2}};
	WarriorSkillLevelIndex[20] = {{1031,2,3},{1032,0,1}};
	WarriorSkillLevelIndex[25] = {{1032,1,3},{1033,0,1}};
	WarriorSkillLevelIndex[30] = {{1033,1,3},{1034,0,1},{1035,0,1}};
	WarriorSkillLevelIndex[35] = {{1034,1,3},{1035,1,2},{1036,0,1}};
	--WarriorSkillLevelIndex[40] = {{1035,2,3},{1036,1,3},{1037,0,1}};
	WarriorSkillLevelIndex[40] = {{1035,2,3},{1036,1,3}};
	--WarriorSkillLevelIndex[50] = {{1037,1,3},{1038,1,2}};
	local MageSkillLevelIndex = {};
	MageSkillLevelIndex[10] = {{2531,0,1}};
	MageSkillLevelIndex[15] = {{2531,1,2}, {2532,0,2}, {2541,0,1}};
	MageSkillLevelIndex[20] = {{2531,2,3}, {2532,2,3}, {2541,1,2}, {2544,0,1}, {2533,0,2}, {2538,0,1}, {2549,0,1}};
	MageSkillLevelIndex[25] = {{2541,2,3}, {2544,1,2}, {2533,2,3}, {2538,1,3}, {2549,1,3}, {2545,0,1}, {2534,0,1}};
	MageSkillLevelIndex[30] = {{2544,2,3}, {2545,1,2}, {2534,1,2}, {2536,0,2}, {2546,0,1}};
	MageSkillLevelIndex[35] = {{2545,2,3}, {2534,2,3}, {2536,2,3}, {2546,1,3}, {2535,0,2}, {2547,0,2}, {2537,0,1}};
	--MageSkillLevelIndex[40] = {{2535,2,3}, {2547,2,3}, {2537,1,3}, {2539,0,1}};
	MageSkillLevelIndex[40] = {{2535,2,3}, {2547,2,3}, {2537,1,3}};
	--MageSkillLevelIndex[50] = {{2539,1,3}, {2540,0,2}};
	local TaoistSkillLevelIndex = {};
	TaoistSkillLevelIndex[10] = {{3002,0,1},{3001,0,1}};
	TaoistSkillLevelIndex[15] = {{3002,1,2},{3001,1,2},{3004,0,1}};
	TaoistSkillLevelIndex[20] = {{3002,2,3},{3001,2,3},{3004,1,3},{3005,0,1},{3003,0,1},{3009,0,1}};
	TaoistSkillLevelIndex[25] = {{3005,1,3},{3003,1,2},{3009,1,2},{3014,0,2},{3006,0,2},{3007,0,1}};
	--TaoistSkillLevelIndex[30] = {{3003,2,3},{3009,2,3},{3014,2,3},{3006,2,3},{3007,1,3},{3011,0,2},{3018,0,1}};
	TaoistSkillLevelIndex[30] = {{3003,2,3},{3009,2,3},{3014,2,3},{3006,2,3},{3007,1,3},{3011,0,2}};
	--TaoistSkillLevelIndex[35] = {{3011,2,3},{3018,1,3},{3012,0,2},{3008,0,1}};
	TaoistSkillLevelIndex[35] = {{3011,2,3},{3012,0,2},{3008,0,1}};
	--TaoistSkillLevelIndex[40] = {{3012,2,3},{3008,1,3},{3015,0,2}};
	TaoistSkillLevelIndex[40] = {{3012,2,3},{3008,1,3}};
	--TaoistSkillLevelIndex[50] = {{3015,2,3},{3010,0,2}};

	local ClassSkillGroup = {WarriorSkillList, MageSkillList, TaoistSkillList};
	local ClassAutoEquipGroup = {WarriorSkillAutoEquipList, MageSkillAutoEquipList, TaoistSkillAutoEquipList};
	local ClassSkillLevelupGroup = {WarriorSkillLevelIndex,MageSkillLevelIndex,TaoistSkillLevelIndex};

	local CurrentLevel= TargetRole:GetLevel();


	local EquipGroup_High = {WarriorEquip,MageEquip,TaoistEquip};
	local EquipGroup_AVG = {WarriorEquip_AVG,MageEquip_AVG,TaoistEquip_AVG};
	local EquipGroup = EquipGroup_AVG;
	if MatchType > 0 then
		EquipGroup = EquipGroup_High;
	end

	local sex = TargetRole:GetSex()
	local bag = TargetRole:GetItemBag();
	local class = TargetRole:GetClass();
	if class == 4 then
		class = 2;
	end

	for i = 1, #EquipGroup[class+1][Index][sex][1] do
		if EquipGroup[class+1][Index][sex][1][i] ~= 0 then

			AddItem(EquipGroup[class+1][Index][sex][1][i], 1);

			local itemPos=FindItemPosInEquipBag(EquipGroup[class+1][Index][sex][1][i]);
			if itemPos then
				for i=0,10 do
					bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, i);
				end
			end
		end
	end

	for j = 1, #EquipGroup[class+1][Index][sex][2] do
		if EquipGroup[class+1][Index][sex][2][j] ~= 0 then

			AddItem(EquipGroup[class+1][Index][sex][2][j], 1);

			local itemPos=FindItemPosInEquipBag(EquipGroup[class+1][Index][sex][2][j]);
			if j == 1 then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_FINGER_L);
			elseif j == 2 then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_FINGER_R);
			elseif j == 3 then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_NECK);
			elseif j == 4 then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_WRIST_L);
			elseif j == 5 then
				bag:MoveItem(CONSLOT_DEFAULT, itemPos, CONSLOT_EQUIPPED, NES_WRIST_R);
			end
		end
	end

	TargetRole:GetAttrList():AddAttrA(TargetRole, catMaxPhyAtk, -100);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catMaxMagAtk, -100);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catMaxTaoAtk, -100);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catWeaponWeightLmt, -500);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catArmorWeightLmt, -500);
	TargetRole:GetAttrList():AddAttrA(TargetRole, catBagWeightLmt, -500);


	--First remove skills from all slots; handle the warrior Attack Swordsmanship skill separately
	--for i=1, #FKeyTable do
		--TargetRole:EquipSpell(FKeyTable[i], 0);
	--end
	--if Index <= 2 then
		--TargetRole:EquipSpell(qssPassive2, 0);
	--end

	for i = 1, LVRequest do
		if ClassSkillGroup[class+1][i] ~= nil and ClassSkillGroup[class+1][i] ~= 0 then
			if MatchType > 0 then
				TargetRole:LearnSpell(ClassSkillGroup[class+1][i], 0, true);
			else
			    if ClassSkillGroup[class+1][i] ~= 1034 and ClassSkillGroup[class+1][i] ~= 1035 and ClassSkillGroup[class+1][i] ~= 1036 and ClassSkillGroup[class+1][i] ~= 2535 and ClassSkillGroup[class+1][i] ~= 2537 and ClassSkillGroup[class+1][i] ~= 2547 and ClassSkillGroup[class+1][i] ~= 3008 and ClassSkillGroup[class+1][i] ~= 3012 and ClassSkillGroup[class+1][i] ~= 3011 then
					TargetRole:LearnSpell(ClassSkillGroup[class+1][i], 0, true);
				end
			end
		end
	end

	if CurrentLevel > LVRequest then
		for j = LVRequest+1, #ClassSkillGroup[class+1] do
			if ClassSkillGroup[class+1][j] ~= nil and ClassSkillGroup[class+1][j] ~= 0 then
				TargetRole:RemoveSpell(ClassSkillGroup[class+1][j]);
			end
		end
	end

	for k = 1, LVRequest do
		if ClassAutoEquipGroup[class+1][k] ~= nil then
			if MatchType > 0 then
				TargetRole:EquipSpell(FKeyTable[ClassAutoEquipGroup[class+1][k][2]], ClassAutoEquipGroup[class+1][k][1]);
			else
			    if ClassAutoEquipGroup[class+1][k][1] ~= 1034 and ClassAutoEquipGroup[class+1][k][1] ~= 1035 and ClassAutoEquipGroup[class+1][k][1] ~= 1036 and ClassAutoEquipGroup[class+1][k][1] ~= 2535 and ClassAutoEquipGroup[class+1][k][1] ~= 2537 and ClassAutoEquipGroup[class+1][k][1] ~= 2547 and ClassAutoEquipGroup[class+1][k][1] ~= 3008 and ClassAutoEquipGroup[class+1][k][1] ~= 3012 and ClassAutoEquipGroup[class+1][k][1] ~= 3011 then
					TargetRole:EquipSpell(FKeyTable[ClassAutoEquipGroup[class+1][k][2]], ClassAutoEquipGroup[class+1][k][1]);
				end
			end
		end
	end

	for k = 1 , LVRequest do
		if ClassSkillLevelupGroup[class+1][k] ~= nil then
			for j = 1, #ClassSkillLevelupGroup[class+1][k] do
				for i= 1, (ClassSkillLevelupGroup[class+1][k][j][3] - ClassSkillLevelupGroup[class+1][k][j][2]) do
					if MatchType > 0 then
						TargetRole:AddSpellPts(ClassSkillLevelupGroup[class+1][k][j][1], 2400);
					else
						if ClassSkillLevelupGroup[class+1][k][j][1] ~= 1034 and ClassSkillLevelupGroup[class+1][k][j][1] ~= 1035 and ClassSkillLevelupGroup[class+1][k][j][1] ~= 1036 and ClassSkillLevelupGroup[class+1][k][j][1] ~= 2535 and ClassSkillLevelupGroup[class+1][k][j][1] ~= 2537 and ClassSkillLevelupGroup[class+1][k][j][1] ~= 2547 and ClassSkillLevelupGroup[class+1][k][j][1] ~= 3008 and ClassSkillLevelupGroup[class+1][k][j][1] ~= 3012 and ClassSkillLevelupGroup[class+1][k][j][1] ~= 3011 then
							TargetRole:AddSpellPts(ClassSkillLevelupGroup[class+1][k][j][1], 2400);
						end
					end
				end
			end
		end
	end

	if class == 2 then
		local PoisonPos1 = FindItemPosInEquipBag(40004);
		local PoisonPos2 = FindItemPosInEquipBag(40014);
		local PoisonPos3 = FindItemPosInEquipBag(40103);
		if PoisonPos1 == nil then
			TargetRole:GetItemBag():AddItem(40004, 1, ITEMLOG_CREATE_ROLE, 0, false);
		end
		if PoisonPos2 == nil then
			TargetRole:GetItemBag():AddItem(40014, 1, ITEMLOG_CREATE_ROLE, 0, false);
		end
		if PoisonPos3 == nil then
			TargetRole:GetItemBag():AddItem(40103, 1, ITEMLOG_CREATE_ROLE, 0, false);
		end
	end

end

function CreateGuild(name, announce)
	TargetRole:GMCreateGuild(name, announce);
end

function SetMsg(id, state)
	if type(id) == 'string' then
		if id == 'all' then
			if state ~= nil then
				for i=8,511 do
					msg(i, state);
				end
			else
				for i=8,511 do
					msg(i, not msg(i));
				end
			end
		end
	elseif type(id) == 'number' then
		if state ~= nil then
			msg(id, state);
		else
			msg(id, not msg(id));
		end
	end

end

function Hide()
	TargetRole:Hide(not TargetRole:IsHidden());
end

function CreateAllWeapon()

	local AllWeapon = {
						99900002,
						99900003,
						99900004,
						99900005,
						99900006,
						99900007,
						99900008,
						99900009,
						99900010,
						99900011,
						99900012,
						99900013,
						99900014,
						99900015,
						99900016,
						99900017,
						99900018,
						99900019,
						99900020,
						99900021,
						99900022,
						99900023,
						99900024,
						99900025,
						99900026,
						99900027,
						99900028,
						99900029,
						99900030,
						99900031,
						99900032,
						99900033,
						99900034,
						99900035,
						99900036
					   }
	for i = 1, #AllWeapon do
		AddItem(AllWeapon[i], 1);
	end

end

function RandomRune()

	local m = 1;
	local class = TargetRole:GetClass();
	if class == 0 then
		m = math.random(1,#WarRune);
		TargetRole:GetItemBag():GetItem(1,0).m_iteminfo.m_wRune[0] = WarRune[m];
	end
	if class == 1 then
		m = math.random(1,#MagRune);
		TargetRole:GetItemBag():GetItem(1,0).m_iteminfo.m_wRune[0] = MagRune[m];
	end
	if class == 4 then
		m = math.random(1,#TaoRune);
		TargetRole:GetItemBag():GetItem(1,0).m_iteminfo.m_wRune[0] = TaoRune[m];
	end

end

function OccupySabac()
	occupy_sabac(TargetRole);
end

function AddExerciseTime(hour)
	TargetRole:SetStartExerciseTime(TargetRole:GetStartExerciseTime() - hour * 3600);
end

function KillAllNpc()
	local scene = TargetRole:GetScene();
	scene:GMKillAllNpc(TargetRole);
end

function GpkKeepAlive(on)
	g_GWConfig.GpkPunishOption.bEnableGpkKeepAlive = on;
end

function GpkTimeoutKick(on)
	g_GWConfig.GpkPunishOption.bEnableKeepAliveTimeoutKick = on;
end

function GpkReturn4Kick(on)
	g_GWConfig.GpkPunishOption.bEnableKeepAliveReturn4Kick = on;
end

function SetKNV(v)
	TargetRole:SetKillNpcValue(v);
end

function GetTotalGambleCount(type, level)
	echo(TargetRole:GetGamble():GetTotalValue(type, level));
end

function GetGambleCount(type, level)
	echo(TargetRole:GetGamble():GetValue(type, level));
end

function SetGambleCount(type, level, value)
	echo(TargetRole:GetGamble():SetValue(type, level, value));
end

function MingWen(runeid1, runeid2)
	local bag = TargetRole:GetItemBag();
	local weapon = bag:GetItem(CONSLOT_EQUIPPED, NES_MAIN_HAND);
	if weapon == nil then
		return;
	end
	weapon.m_iteminfo.m_wRune[0] = runeid1;
	weapon.m_iteminfo.m_wRune[1] = runeid2;
end

function XingYun(v)
	local bag = TargetRole:GetItemBag();
	local weapon = bag:GetItem(CONSLOT_EQUIPPED, NES_MAIN_HAND);
	if weapon == nil then
		return;
	end
	weapon.m_iteminfo.m_byLuck = v;
end

function QiangHua(class, v)
	local bag = TargetRole:GetItemBag();
	local weapon = bag:GetItem(CONSLOT_EQUIPPED, NES_MAIN_HAND);
	if weapon == nil then
		return;
	end
	if class == 1 then
		weapon.m_iteminfo.m_byLevelupPhyPoint = v;
	elseif class == 2 then
		weapon.m_iteminfo.m_byLevelupMagPoint = v;
	elseif class == 3 then
		weapon.m_iteminfo.m_byLevelupTaoPoint = v;
	end
	weapon.m_iteminfo.m_byLevelupTimes = 2;
end

function LvZi(class, index, v)
	local bag = TargetRole:GetItemBag();
	local equip = bag:GetItem(CONSLOT_EQUIPPED, class);
	if equip == nil then
		return;
	end
	equip.m_iteminfo.m_wRandomAttrID[index] = v;
end

function DaKong(class, index, v)
	local bag = TargetRole:GetItemBag();
	local equip = bag:GetItem(CONSLOT_EQUIPPED, class);
	if equip == nil then
		return;
	end
	equip.m_iteminfo.m_dwSocketCombination = v;
end

Include("script/gs_gm/robot.lua")
Include("script/gs_gm/gm_command.lua");


-- 1.05 backport: mount GM helpers   (.ActivateAllMounts()  .RideMount(51)  .UnrideMount())
function ActivateAllMounts()
	for i = 1, 60 do
		TargetRole:ActivateMount(i);
	end
end

function RideMount(id)
	TargetRole:OffMount();
	TargetRole:ActivateMount(id);
	TargetRole:OnMount(id);
end

function UnrideMount()
	TargetRole:OffMount();
end

-- 1.05 backport: wing GM helpers   (.WearWing(39501)  .RemoveWing()  .AddAllWings())
function WearWing(id)
	TargetRole:GrantWing(id);
	TargetRole:EquipWing(id);
	TargetRole:SyncWingInfo();
end

function RemoveWing()
	TargetRole:UnequipWing();
	TargetRole:SyncWingInfo();
end

function AddAllWings()
	for id = 39501, 39527 do
		TargetRole:GrantWing(id);
		TargetRole:GetItemBag():AddItem(id, 1, ITEMLOG_GM_COMMAND, 0, false);
	end
	TargetRole:GetItemBag():AddItem(39499, 1, ITEMLOG_GM_COMMAND, 0, false);
	TargetRole:SyncWingInfo();
end

-- wing panel upgrade materials (wings_level.txt): 90411/90412/90413 and 111008/110011   (.AddWingMaterials(1000))
function AddWingMaterials(n)
	for _, id in ipairs({ 90411, 90412, 90413, 111008, 110011 }) do
		TargetRole:GetItemBag():AddItem(id, n, ITEMLOG_GM_COMMAND, 0, false);
	end
end
