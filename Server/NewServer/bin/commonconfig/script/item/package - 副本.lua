
PACKAGE_TABLE = {
--	{
--		func = 'sampel_func',
--		maxitemcount = 2,    --Required empty bag slots
--		checkvariable = 145, --Variable ID holding gift claim time
--		waittime = 1800,     --Required wait time in seconds
--		opentimesvariable = 152, --Variable ID holding open count
--		maxopentimes = 1,    Number of allowed openings
--		{ itemid = 1, itemcount = 5, percent = 80 },
--		{ itemid = 2, itemcount = 4, class = clsWarrior },
--		{ itemid = 3, itemcount = 3, class = clsMage },
--		{ itemid = 4, itemcount = 2, class = clsRogue },
--		{ itemid = 5, itemcount = 1, class = clsArcher },
--		{ itemid = 6, itemcount = 7, sex = cstMale },
--		{ itemid = 7, itemcount = 6, sex = cstFemale },
--		{ select = 
--			{
--				{itemid = 1, itemcount = 4, weight = 1},
--				{itemid = 2, itemcount = 3, weight = 2},
--				{itemid = 3, itemcount = 2, weight = 4},
--				{itemid = 4, itemcount = 1, weight = 8},
--			}
--		},
--		{ exp = 100 },
--		{ title = 1 },
--		{ moneytype = MONEYTYPE_02, moneyamount = 100 },
--		{ questid = 1, varid = 0, varvalue = 1 },
--		{ doodadid = 3283}, --doodad id
--		{ pkvalue = 100, chance = 50 },
--		{ announce = '{role_name} used a torch!'}, --Announcement text; "{*}" denotes replacements. See ANNOUNCE_REPL_STRING_FUNC_TABLE for replacement definitions
--		{ aura = 1 }, Receive one aura
--	},

	{
		func = 'biqiyongshi',
		maxitemcount = 6,
		{ title = 91 },
		{ moneytype = MONEYTYPE_02, moneyamount = 2000 },
		--{ itemid = 10121, itemcount = 2 },
		{ itemid = 381, itemcount = 1 },
		{ itemid = 381, itemcount = 1 },
		{ itemid = 381, itemcount = 1 },
		{ itemid = 381, itemcount = 1 },
		{ itemid = 381, itemcount = 1 },
		{ itemid = 381, itemcount = 1 },
		--{ itemid = 10552131, itemcount = 20,percent = 10 },
		--{ itemid = 10552132, itemcount = 20,percent = 10 },
		--{ itemid = 10552133, itemcount = 20,percent = 10 },
		--{ itemid = 10552134, itemcount = 20,percent = 10 },

	},

	{
		func = 'biqibazhu',
		maxitemcount = 8,
		{ title = 92 },
		{ moneytype = MONEYTYPE_02, moneyamount = 10000 },
		--{ itemid = 10610251, itemcount = 1 },
		--{ itemid = 10122, itemcount = 2 },


		--Bound item
		--{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1 },
		{ itemid = 382, itemcount = 1 },
		{ itemid = 382, itemcount = 1 },
		{ itemid = 382, itemcount = 1 },
		{ itemid = 382, itemcount = 1 },
		{ itemid = 382, itemcount = 1 },
		{ itemid = 382, itemcount = 1 },
		{ itemid = 99900026, itemcount = 1 },
		--{ itemid = 10552131, itemcount = 20,percent = 50 },
		--{ itemid = 10552132, itemcount = 20,percent = 50 },
		--{ itemid = 10552133, itemcount = 20,percent = 50 },
		--{ itemid = 10552134, itemcount = 20,percent = 50 },

	},

	{
		func = 'shengwangbaoshi1',
		maxitemcount = 5,

		{ itemid = 10110, itemcount = 1 },
		{ itemid = 10210, itemcount = 1 },
		{ itemid = 10310, itemcount = 1 },
		{ itemid = 10410, itemcount = 1 },
		{ itemid = 10510, itemcount = 1 },

	},
	{
		func = 'shengwangbaoshi2',
		maxitemcount = 5,

		{ itemid = 10120, itemcount = 1 },
		{ itemid = 10220, itemcount = 1 },
		{ itemid = 10320, itemcount = 1 },
		{ itemid = 10420, itemcount = 1 },
		{ itemid = 10520, itemcount = 1 },

	},
	{
		func = 'OnUseNovicePackage',
		maxitemcount = 2,

		{ itemid = 361, itemcount = 1 },
		{ itemid = 1000015, itemcount = 1 },

	},

	{
		func = 'OnUseNovicePackage2',
		maxitemcount = 2,

		{ itemid = 100001, itemcount = 5 },
		{ itemid = 1000015, itemcount = 1 },

	},

	{
		func = 'OnUseNovicePackage3',
		maxitemcount = 2,

		{ itemid = 100001, itemcount = 5 },


	},


	{
		func = 'OnUseTestPackage1',
		maxitemcount = 1,

		{ itemid = 361, itemcount = 1 },
		--{ itemid = 1100010, itemcount = 5 },
		--{ itemid = 2, itemcount = 100000 },
		--{ itemid = 1, itemcount = 1000},

	},

	{
		func = 'OnUseTestPackage2',
		maxitemcount = 1,
		{ itemid = 361, itemcount = 1 },
		--{ itemid = 6, itemcount = 300 },
		--{ itemid = 1100015, itemcount = 1 },
		--{ itemid = 2, itemcount = 500000 },


	},

	{
		func = 'OnUseTestPackage3',
		maxitemcount = 1,
		{ itemid = 361, itemcount = 1 },
		{ moneytype = MONEYTYPE_02, moneyamount = 100 },
		--{ itemid = 21001, itemcount = 2 },
		--{ itemid = 110001, itemcount = 5 },
		--{ itemid = 110002, itemcount = 1 },
		--{ itemid = 1100020, itemcount = 1 },
		{ moneytype = MONEYTYPE_02, moneyamount = 1000000 },
		--{ itemid = 20310121, itemcount = 1, class = clsWarrior },
		--{ itemid = 121512, itemcount = 1, class = clsWarrior },
		--{ itemid = 20340121, itemcount = 1, class = clsMage },
		--{ itemid = 121523, itemcount = 1, class = clsMage },
		--{ itemid = 20320121, itemcount = 1, class = clsRogue },
		--{ itemid = 121533, itemcount = 1, class = clsRogue },
		--{ itemid = 20336121, itemcount = 1, class = clsArcher },
		--{ itemid = 121543, itemcount = 1, class = clsArcher },
	},

	{
		func = 'OnUseTestPackage4',
		maxitemcount = 1,
		{ moneytype = MONEYTYPE_02, moneyamount = 1000000 },
	--	{ itemid = 361, itemcount = 1 },
		--{ itemid = 6, itemcount = 500 },
		--{ itemid = 20001, itemcount = 2 },
		--{ itemid = 1010001, itemcount = 1 },
		--{ itemid = 10100, itemcount = 1 },
		--{ itemid = 10200, itemcount = 1 },
		--{ itemid = 10300, itemcount = 1 },

		--{ itemid = 20310121, itemcount = 1, class = clsWarrior },
		--{ itemid = 121512, itemcount = 1, class = clsWarrior },
		--{ itemid = 20340121, itemcount = 1, class = clsMage },
		--{ itemid = 121523, itemcount = 1, class = clsMage },
		--{ itemid = 20320121, itemcount = 1, class = clsRogue },
		--{ itemid = 121533, itemcount = 1, class = clsRogue },
		--{ itemid = 20336121, itemcount = 1, class = clsArcher },
		--{ itemid = 121543, itemcount = 1, class = clsArcher },
	},

	{
		func = 'OnUseMasterPackage',
		maxitemcount = 2,
		{ moneytype = MONEYTYPE_02, moneyamount = 100 },
		--{ itemid = 122, itemcount = 5 },
		{ itemid = 100001, itemcount = 10 },

	},

	{
		func = 'OnUseMasterPackage101',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1 },
		{ itemid = 1030012, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage102',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1 },
		{ itemid = 1030012, itemcount = 2 },

	},
	{
		func = 'OnUseMasterPackage103',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1 },
		{ itemid = 1030012, itemcount = 3 },

	},
	{
		func = 'OnUseMasterPackage104',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1 },
		{ itemid = 1030012, itemcount = 4 },

	},
	{
		func = 'OnUseMasterPackage105',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1 },
		{ itemid = 1030012, itemcount = 5 },

	},
	{
		func = 'OnUseMasterPackage106',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1 },
		{ itemid = 1030012, itemcount = 6 },

	},
	{
		func = 'OnUseMasterPackage107',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1 },
		{ itemid = 1030012, itemcount = 7 },

	},
	{
		func = 'OnUseMasterPackage108',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1 },
		{ itemid = 1030012, itemcount = 8 },

	},
	{
		func = 'OnUseMasterPackage109',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1 },
		{ itemid = 1030012, itemcount = 9 },

	},
------------Items awarded to a student forced to graduate
	{
		func = 'OnUseMasterPackage110',
		maxitemcount = 2,
		{ exp = 10000 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 1030012, itemcount = 3 },

	},
------------Items awarded to a student upon normal graduation
	{
		func = 'OnUseMasterPackage111',
		maxitemcount = 2,
		{ exp = 20000 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 1030012, itemcount = 5 },

	},
	{
		func = 'OnUseMasterPackage201',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350001, itemcount = 1 },
	},
	{
		func = 'OnUseMasterPackage202',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350001, itemcount = 1 },
	},
	{
		func = 'OnUseMasterPackage203',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350001, itemcount = 1 },
	},
	{
		func = 'OnUseMasterPackage204',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350001, itemcount = 1 },
	},
	{
		func = 'OnUseMasterPackage205',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350001, itemcount = 1 },
	},
	{
		func = 'OnUseMasterPackage206',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350001, itemcount = 1 },
	},
	{
		func = 'OnUseMasterPackage207',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350001, itemcount = 1 },
	},
	{
		func = 'OnUseMasterPackage208',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350001, itemcount = 1 },
	},
	{
		func = 'OnUseMasterPackage209',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350001, itemcount = 1 },
	},

------------Items awarded to the mentor when a student graduates
	{
		func = 'OnUseMasterPackage210',
		maxitemcount = 3,
		{ exp = 15000 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 80003, itemcount = 1 },
		{ itemid = 1350002, itemcount = 1 },
	},

------------Potion pack
	{
		func = 'jcybao1',
		maxitemcount = 6,
		{ itemid = 301, itemcount = 1 },
		{ itemid = 301, itemcount = 1 },
		{ itemid = 301, itemcount = 1 },
		{ itemid = 301, itemcount = 1 },
		{ itemid = 301, itemcount = 1 },
		{ itemid = 301, itemcount = 1 },

	},
	{
		func = 'jcybao2',
		maxitemcount = 6,
		{ itemid = 302, itemcount = 1 },
		{ itemid = 302, itemcount = 1 },
		{ itemid = 302, itemcount = 1 },
		{ itemid = 302, itemcount = 1 },
		{ itemid = 302, itemcount = 1 },
		{ itemid = 302, itemcount = 1 },

	},
	{
		func = 'jcybao3',
		maxitemcount = 6,
		{ itemid = 303, itemcount = 1 },
		{ itemid = 303, itemcount = 1 },
		{ itemid = 303, itemcount = 1 },
		{ itemid = 303, itemcount = 1 },
		{ itemid = 303, itemcount = 1 },
		{ itemid = 303, itemcount = 1 },
	},
	{
		func = 'jcybao6',
		maxitemcount = 6,
		{ itemid = 306, itemcount = 1 },
		{ itemid = 306, itemcount = 1 },
		{ itemid = 306, itemcount = 1 },
		{ itemid = 306, itemcount = 1 },
		{ itemid = 306, itemcount = 1 },
		{ itemid = 306, itemcount = 1 },

	},
	{
		func = 'jcybao7',
		maxitemcount = 6,
		{ itemid = 307, itemcount = 1 },
		{ itemid = 307, itemcount = 1 },
		{ itemid = 307, itemcount = 1 },
		{ itemid = 307, itemcount = 1 },
		{ itemid = 307, itemcount = 1 },
		{ itemid = 307, itemcount = 1 },

	},
	{
		func = 'jcybao8',
		maxitemcount = 6,
		{ itemid = 308, itemcount = 1 },
		{ itemid = 308, itemcount = 1 },
		{ itemid = 308, itemcount = 1 },
		{ itemid = 308, itemcount = 1 },
		{ itemid = 308, itemcount = 1 },
		{ itemid = 308, itemcount = 1 },
	},
	{
		func = 'mfybao1',
		maxitemcount = 6,
		{ itemid = 321, itemcount = 1 },
		{ itemid = 321, itemcount = 1 },
		{ itemid = 321, itemcount = 1 },
		{ itemid = 321, itemcount = 1 },
		{ itemid = 321, itemcount = 1 },
		{ itemid = 321, itemcount = 1 },

	},
	{
		func = 'mfybao2',
		maxitemcount = 6,
		{ itemid = 322, itemcount = 1 },
		{ itemid = 322, itemcount = 1 },
		{ itemid = 322, itemcount = 1 },
		{ itemid = 322, itemcount = 1 },
		{ itemid = 322, itemcount = 1 },
		{ itemid = 322, itemcount = 1 },

	},
	{
		func = 'mfybao3',
		maxitemcount = 6,
		{ itemid = 323, itemcount = 1 },
		{ itemid = 323, itemcount = 1 },
		{ itemid = 323, itemcount = 1 },
		{ itemid = 323, itemcount = 1 },
		{ itemid = 323, itemcount = 1 },
		{ itemid = 323, itemcount = 1 },

	},
	{
		func = 'mfybao6',
		maxitemcount = 6,
		{ itemid = 326, itemcount = 1 },
		{ itemid = 326, itemcount = 1 },
		{ itemid = 326, itemcount = 1 },
		{ itemid = 326, itemcount = 1 },
		{ itemid = 326, itemcount = 1 },
		{ itemid = 326, itemcount = 1 },

	},
	{
		func = 'mfybao7',
		maxitemcount = 6,
		{ itemid = 327, itemcount = 1 },
		{ itemid = 327, itemcount = 1 },
		{ itemid = 327, itemcount = 1 },
		{ itemid = 327, itemcount = 1 },
		{ itemid = 327, itemcount = 1 },
		{ itemid = 327, itemcount = 1 },

	},
	{
		func = 'mfybao8',
		maxitemcount = 6,
		{ itemid = 328, itemcount = 1 },
		{ itemid = 328, itemcount = 1 },
		{ itemid = 328, itemcount = 1 },
		{ itemid = 328, itemcount = 1 },
		{ itemid = 328, itemcount = 1 },
		{ itemid = 328, itemcount = 1 },

	},
	{
		func = 'tysbao1',
		maxitemcount = 6,
		{ itemid = 351, itemcount = 1 },
		{ itemid = 351, itemcount = 1 },
		{ itemid = 351, itemcount = 1 },
		{ itemid = 351, itemcount = 1 },
		{ itemid = 351, itemcount = 1 },
		{ itemid = 351, itemcount = 1 },

	},
	{
		func = 'tysbao2',
		maxitemcount = 6,
		{ itemid = 352, itemcount = 1 },
		{ itemid = 352, itemcount = 1 },
		{ itemid = 352, itemcount = 1 },
		{ itemid = 352, itemcount = 1 },
		{ itemid = 352, itemcount = 1 },
		{ itemid = 352, itemcount = 1 },

	},
	{
		func = 'lsybao',
		maxitemcount = 6,
		{ itemid = 353, itemcount = 1 },
		{ itemid = 353, itemcount = 1 },
		{ itemid = 353, itemcount = 1 },
		{ itemid = 353, itemcount = 1 },
		{ itemid = 353, itemcount = 1 },
		{ itemid = 353, itemcount = 1 },

	},
	{
		func = 'wnxsbao',
		maxitemcount = 6,
		{ itemid = 354, itemcount = 1 },
		{ itemid = 354, itemcount = 1 },
		{ itemid = 354, itemcount = 1 },
		{ itemid = 354, itemcount = 1 },
		{ itemid = 354, itemcount = 1 },
		{ itemid = 354, itemcount = 1 },

	},
		{
		func = 'tysbao6',
		maxitemcount = 6,
		{ itemid = 356, itemcount = 1 },
		{ itemid = 356, itemcount = 1 },
		{ itemid = 356, itemcount = 1 },
		{ itemid = 356, itemcount = 1 },
		{ itemid = 356, itemcount = 1 },
		{ itemid = 356, itemcount = 1 },

	},
	{
		func = 'tysbao7',
		maxitemcount = 6,
		{ itemid = 357, itemcount = 1 },
		{ itemid = 357, itemcount = 1 },
		{ itemid = 357, itemcount = 1 },
		{ itemid = 357, itemcount = 1 },
		{ itemid = 357, itemcount = 1 },
		{ itemid = 357, itemcount = 1 },

	},
	{
		func = 'lsybao2',
		maxitemcount = 6,
		{ itemid = 358, itemcount = 1 },
		{ itemid = 358, itemcount = 1 },
		{ itemid = 358, itemcount = 1 },
		{ itemid = 358, itemcount = 1 },
		{ itemid = 358, itemcount = 1 },
		{ itemid = 358, itemcount = 1 },

	},
	{
		func = 'wnxsbao2',
		maxitemcount = 6,
		{ itemid = 359, itemcount = 1 },
		{ itemid = 359, itemcount = 1 },
		{ itemid = 359, itemcount = 1 },
		{ itemid = 359, itemcount = 1 },
		{ itemid = 359, itemcount = 1 },
		{ itemid = 359, itemcount = 1 },

	},

-----------Town portal scroll pack-------------

	{
		func = 'bqhcjb',
		maxitemcount = 6,
		{ itemid = 80001, itemcount = 1 },
		{ itemid = 80001, itemcount = 1 },
		{ itemid = 80001, itemcount = 1 },
		{ itemid = 80001, itemcount = 1 },
		{ itemid = 80001, itemcount = 1 },
		{ itemid = 80001, itemcount = 1 },

	},
	{
		func = 'mzhcjb',
		maxitemcount = 6,
		{ itemid = 80007, itemcount = 1 },
		{ itemid = 80007, itemcount = 1 },
		{ itemid = 80007, itemcount = 1 },
		{ itemid = 80007, itemcount = 1 },
		{ itemid = 80007, itemcount = 1 },
		{ itemid = 80007, itemcount = 1 },

	},
	{
		func = 'sbkhcjb',
		maxitemcount = 6,
		{ itemid = 80008, itemcount = 1 },
		{ itemid = 80008, itemcount = 1 },
		{ itemid = 80008, itemcount = 1 },
		{ itemid = 80008, itemcount = 1 },
		{ itemid = 80008, itemcount = 1 },
		{ itemid = 80008, itemcount = 1 },

	},
	{
		func = 'sjcsjb',
		maxitemcount = 6,
		{ itemid = 80005, itemcount = 1 },
		{ itemid = 80005, itemcount = 1 },
		{ itemid = 80005, itemcount = 1 },
		{ itemid = 80005, itemcount = 1 },
		{ itemid = 80005, itemcount = 1 },
		{ itemid = 80005, itemcount = 1 },

	},




-----------Mentorship gift packs begin-------------
	{
		func = 'OnUseMasterPackage1331501',
		maxitemcount = 2,
		{  exp = 600 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 371, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1332001',
		maxitemcount = 2,
		{  exp = 1700 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 371, itemcount = 1 },


	},
	{
		func = 'OnUseMasterPackage1332501',
		maxitemcount = 2,
		{  exp = 4800 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 371, itemcount = 1 },

	},

	{
		func = 'OnUseMasterPackage1333001',
		maxitemcount = 2,
		{  exp = 8000 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 371, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1333501',
		maxitemcount = 2,
		{  exp = 10000 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 371, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1334001',
		maxitemcount = 2,
		{  exp = 10000 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 371, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1331502',
		maxitemcount = 2,
		{  exp = 800 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 372, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1332002',
		maxitemcount = 2,
		{  exp = 2300 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 372, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1332502',
		maxitemcount = 2,
		{  exp = 6200 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 372, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1333002',
		maxitemcount = 2,
		{  exp = 10400 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 372, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1333502',
		maxitemcount = 2,
		{  exp = 15000 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 372, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1334002',
		maxitemcount = 2,
		{  exp = 15000 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 372, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1331503',
		maxitemcount = 2,
		{  exp = 950 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1332003',
		maxitemcount = 2,
		{  exp = 2800 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1332503',
		maxitemcount = 2,
		{  exp = 7600 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1333003',
		maxitemcount = 2,
		{  exp = 13000 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1333503',
		maxitemcount = 2,
		{  exp = 20000 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1334003',
		maxitemcount = 2,
		{  exp = 20000 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1344501',
		maxitemcount = 1,
		{ exp = 2000 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 371, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1345001',
		maxitemcount = 1,
		{ exp = 2000 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 371, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1345501',
		maxitemcount = 1,
		{ exp = 2000 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 371, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1344502',
		maxitemcount = 1,
		{  exp = 3000 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 372, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1345002',
		maxitemcount = 1,
		{  exp = 3000 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 372, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1345502',
		maxitemcount = 1,
		{  exp = 3000 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 372, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1344503',
		maxitemcount = 1,
		{  exp = 4000 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1345003',
		maxitemcount = 1,
		{  exp = 4000 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },

	},
	{
		func = 'OnUseMasterPackage1345503',
		maxitemcount = 1,
		{  exp = 4000 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },

	},
	{
		func = 'OnUseAddMasterExp1',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_MASTER_EXP, moneyamount = 1 },
	},
	{
		func = 'OnUseAddMasterExp80',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_MASTER_EXP, moneyamount = 80 },
	},

	{
		func = 'Hexin',
		maxitemcount = 3,

		{ itemid = 1030012, itemcount = 1 },
		{ itemid = 361, itemcount = 1},
		{ itemid = 371, itemcount = 1},

	},

	{
		func = 'Sihailihe',
		maxitemcount = 3,

		{ itemid = 1030013, itemcount = 1 },
		{ itemid = 362, itemcount = 1},
		{ itemid = 372, itemcount = 1},

	},

	{
		func = 'Longjingongli',
		maxitemcount = 3,

		{ itemid = 1030014, itemcount = 1 },
		{ itemid = 363, itemcount = 1},
		{ itemid = 373, itemcount = 1},

	},




--Increase virtue points

	{
		func = 'daoyishujuan1',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 10000 },

	},

	{
		func = 'daoyishujuan2',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 40000 },

	},

	{
		func = 'daoyishujuan3',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 70000 },

	},

	{
		func = 'daoyishujuan4',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 10000 },

	},

	{
		func = 'daoyishujuan5',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 70000 },

	},

	{
		func = 'daoyishujuan6',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 140000 },

	},

	{
		func = 'daoyishujuan7',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 10000 },

	},

	{
		func = 'daoyishujuan8',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 110000 },

	},

	{
		func = 'daoyishujuan9',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 200000 },

	},

	{
		func = 'daoyishujuan10',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 10000 },

	},

	{
		func = 'daoyishujuan11',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 170000 },

	},

	{
		func = 'daoyishujuan12',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 290000 },

	},

	{
		func = 'daoyishujuan13',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 130000 },

	},

	{
		func = 'daoyishujuan14',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 140000 },

	},

	{
		func = 'daoyishujuan15',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 150000 },

	},

	{
		func = 'daoyishujuan16',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 160000 },

	},

	{
		func = 'daoyishujuan17',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 170000 },

	},

	{
		func = 'daoyishujuan18',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 180000 },

	},

	{
		func = 'daoyishujuan19',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 190000 },

	},

	{
		func = 'daoyishujuan20',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 200000 },

	},

	{
		func = 'daoyishujuan21',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 210000 },

	},

	{
		func = 'daoyishujuan22',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 220000 },

	},

	{
		func = 'daoyishujuan23',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 230000 },

	},

	{
		func = 'daoyishujuan24',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 240000 },

	},

	{
		func = 'daoyishujuan25',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 250000 },

	},

	{
		func = 'daoyishujuan26',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 260000 },

	},

	{
		func = 'daoyishujuan27',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 270000 },

	},

	{
		func = 'daoyishujuan28',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 280000 },

	},

	{
		func = 'daoyishujuan29',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 290000 },

	},

	{
		func = 'daoyishujuan30',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 300000 },

	},

	{
		func = 'daoyishujuan31',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 310000 },

	},

	{
		func = 'daoyishujuan32',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 320000 },

	},

	{
		func = 'daoyishujuan33',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 330000 },

	},

	{
		func = 'daoyishujuan34',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 340000 },

	},

	{
		func = 'daoyishujuan35',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 350000 },

	},

	{
		func = 'daoyishujuan36',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 360000 },

	},

	{
		func = 'daoyishujuan37',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 370000 },

	},

	{
		func = 'daoyishujuan38',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 380000 },

	},

	{
		func = 'daoyishujuan39',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 390000 },

	},

	{
		func = 'daoyishujuan40',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 400000 },

	},

	{
		func = 'daoyishujuan41',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 410000 },

	},

	{
		func = 'daoyishujuan42',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 420000 },

	},

	{
		func = 'daoyishujuan43',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 430000 },

	},

	{
		func = 'daoyishujuan44',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 440000 },

	},

	{
		func = 'daoyishujuan45',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 450000 },

	},

	{
		func = 'daoyishujuan46',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 460000 },

	},

	{
		func = 'daoyishujuan47',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 470000 },

	},

	{
		func = 'daoyishujuan48',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 480000 },

	},

	{
		func = 'daoyishujuan49',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 490000 },

	},

	{
		func = 'daoyishujuan50',
		maxitemcount = 0,

		{ moneytype = MONEYTYPE_MORAL, moneyamount = 500000 },

	},



-----------Mentorship gift packs end---------------
	{
		func = 'OnUseQiYuPackage',
		maxitemcount = 3,
		{ itemid = 363, itemcount = 1 },

		{ itemid = 80003, itemcount = 15,percent = 20 },
		{ itemid = 110001, itemcount = 20,percent = 50 },

	},

	{
		func = 'OnUsePackage1200001',
		maxitemcount = 10,
		{ itemid = 10111, itemcount = 1 },
		{ itemid = 10121, itemcount = 1 },
		{ itemid = 10211, itemcount = 1 },
		{ itemid = 10221, itemcount = 1 },
		{ itemid = 10311, itemcount = 1 },
		{ itemid = 10321, itemcount = 1 },
		{ itemid = 10411, itemcount = 1 },
		{ itemid = 10421, itemcount = 1 },
		{ itemid = 10511, itemcount = 1 },
		{ itemid = 10521, itemcount = 1 },
	},

---------------------------Event gift pack--------------
	{
		func = 'ceshilihe1',
		maxitemcount = 1,    --Required empty bag slots
		checkvariable = 145, --Variable ID holding gift claim time
		waittime = 1800,     --Required wait time in seconds
		{ moneytype = MONEYTYPE_02, moneyamount = 1000 },
		{ itemid = 99980212, itemcount = 1, class = clsWarrior },
		{ itemid = 99990222, itemcount = 1, class = clsMage },
		{ itemid = 99990232, itemcount = 1, class = clsTaoist },

	},
	{
		func = 'ceshilihe2',
		maxitemcount = 1,    --Required empty bag slots
		checkvariable = 146, --Variable ID holding gift claim time
		waittime = 1800,     --Required wait time in seconds
		{ moneytype = MONEYTYPE_02, moneyamount = 3000 },
		{ itemid = 99980314, itemcount = 1, class = clsWarrior },
		{ itemid = 99990324, itemcount = 1, class = clsMage },
		{ itemid = 99990334, itemcount = 1, class = clsTaoist },

	},
	{
		func = 'ceshilihe3',
		maxitemcount = 1,    --Required empty bag slots
		checkvariable = 147, --Variable ID holding gift claim time
		waittime = 1800,     --Required wait time in seconds
		{ moneytype = MONEYTYPE_02, moneyamount = 5000 },

		{ itemid = 99980413, itemcount = 1, class = clsWarrior },
		{ itemid = 99990423, itemcount = 1, class = clsMage },
		{ itemid = 99990433, itemcount = 1, class = clsTaoist },

	},
	{
		func = 'ceshilihe4',
		maxitemcount = 1,    --Required empty bag slots
		checkvariable = 148, --Variable ID holding gift claim time
		waittime = 1800,     --Required wait time in seconds
		{ moneytype = MONEYTYPE_02, moneyamount = 7000 },

		{ itemid = 99980416, itemcount = 1, class = clsWarrior },
		{ itemid = 99990426, itemcount = 1, class = clsMage },
		{ itemid = 99990436, itemcount = 1, class = clsTaoist },

	},
	{
		func = 'ceshilihe5',
		maxitemcount = 1,    --Required empty bag slots
		checkvariable = 149, --Variable ID holding gift claim time
		waittime = 1800,     --Required wait time in seconds
		{ moneytype = MONEYTYPE_02, moneyamount = 9000 },

		{ itemid = 99980515, itemcount = 1, class = clsWarrior },
		{ itemid = 99990525, itemcount = 1, class = clsMage },
		{ itemid = 99990535, itemcount = 1, class = clsTaoist },

	},

	{
		func = 'ceshilihe6',
		maxitemcount = 1,    --Required empty bag slots
		checkvariable = 150, --Variable ID holding gift claim time
		waittime = 1800,     --Required wait time in seconds
		{ moneytype = MONEYTYPE_02, moneyamount = 11000},

		{ itemid = 99980517, itemcount = 1, class = clsWarrior },
		{ itemid = 99990527, itemcount = 1, class = clsMage },
		{ itemid = 99990537, itemcount = 1, class = clsTaoist },

	},

	{
		func = 'ceshilihe7',
		maxitemcount = 1,    --Required empty bag slots
		checkvariable = 151, --Variable ID holding gift claim time
		waittime = 1800,     --Required wait time in seconds
		{ moneytype = MONEYTYPE_02, moneyamount = 13000 },

		{ itemid = 99900030, itemcount = 1, class = clsWarrior },
		{ itemid = 99900028, itemcount = 1, class = clsMage },
		{ itemid = 99900031, itemcount = 1, class = clsTaoist },

	},

	----I Love Legend Eternal gift pack
	{
		func = 'woaicqyh',
		maxitemcount = 2,    --Required empty bag slots

		{ itemid = 99990315, itemcount = 1 },
		{ itemid = 99990317, itemcount = 1 },

	},

	------------------------Guild support gift pack

	{
		func = 'GongHuiFuChi',
		maxitemcount = 11,    --Required empty bag slots
		opentimesvariable = 208,
		maxopentimes = 1,

		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 80203, itemcount = 1, bind = true },
		{ itemid = 21001, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 21002, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 21003, itemcount = 1, class = clsTaoist, bind = true },

	},

	------------------------Territory upgrade gift pack

	{
		func = 'ShuDiShengJi',
		maxitemcount = 16,    --Required empty bag slots
		opentimesvariable = 209,
		maxopentimes = 1,

		{ itemid = 120001, itemcount = 5, bind = true },
		{ itemid = 120001, itemcount = 5, bind = true },
		{ itemid = 120001, itemcount = 5, bind = true },
		{ itemid = 120001, itemcount = 5, bind = true },
		{ itemid = 120002, itemcount = 5, bind = true },
		{ itemid = 120002, itemcount = 5, bind = true },
		{ itemid = 120002, itemcount = 5, bind = true },
		{ itemid = 120002, itemcount = 5, bind = true },
		{ itemid = 120003, itemcount = 5, bind = true },
		{ itemid = 120003, itemcount = 5, bind = true },
		{ itemid = 120003, itemcount = 5, bind = true },
		{ itemid = 120003, itemcount = 5, bind = true },
		{ itemid = 120004, itemcount = 5, bind = true },
		{ itemid = 120004, itemcount = 5, bind = true },
		{ itemid = 120004, itemcount = 5, bind = true },
		{ itemid = 120004, itemcount = 5, bind = true },

	},

	------------------------Gift rice dumpling

	{
		func = 'ZongZi',
		maxitemcount = 4,    --Required empty bag slots

		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 383, itemcount = 1, percent = 50, bind = true },
		{ itemid = 91102, itemcount = 1, percent = 30, bind = true },
		{ itemid = 80003, itemcount = 1, percent = 5, bind = true },

	},

	------------------------Homecoming equipment pack

	{
		func = 'RongGui1',
		maxitemcount = 6,    --Required empty bag slots

		{ itemid = 69910041, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 69910042, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 69910043, itemcount = 1, class = clsTaoist, bind = true },
		{ itemid = 69900009, itemcount = 1, bind = true },
		{ itemid = 69910021, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 69910022, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 69910023, itemcount = 1, class = clsTaoist, bind = true },
		{ itemid = 69910021, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 69910022, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 69910023, itemcount = 1, class = clsTaoist, bind = true },
		{ itemid = 69910031, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 69910032, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 69910033, itemcount = 1, class = clsTaoist, bind = true },
		{ itemid = 69910031, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 69910032, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 69910033, itemcount = 1, class = clsTaoist, bind = true },
		{ title = 103 },
		{ aura = 2531 },

	},
	------------------------14-day hero invitation gift pack

	{
		func = '14YingXiongTie',
		maxitemcount = 6,    --Required empty bag slots

		{ select = 
			{
				{itemid = 99920016, itemcount = 1, weight = 1000},
				{itemid = 99920020, itemcount = 1, weight = 1000},
				{itemid = 99920021, itemcount = 1, weight = 1000},
				{itemid = 99930018, itemcount = 1, weight = 1000},
				{itemid = 99930017, itemcount = 1, weight = 1000},
				{itemid = 99930014, itemcount = 1, weight = 1000},
				{itemid = 99940020, itemcount = 1, weight = 1000},
				{itemid = 99940011, itemcount = 1, weight = 1000},
				{itemid = 99940015, itemcount = 1, weight = 1000},
			},
		},
	},

	------------------------Fengyun guild-exclusive gift pack

	{
		func = 'FYGHZhuangShu',
		maxitemcount = 6,    --Required empty bag slots
		opentimesvariable = 191,
		maxopentimes = 1,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ moneytype = MONEYTYPE_02, moneyamount = 100000 },

	},

	------------------------Langya guild-exclusive gift pack

	{
		func = 'LYGHZhuangShu',
		maxitemcount = 6,    --Required empty bag slots
		opentimesvariable = 192,
		maxopentimes = 1,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ moneytype = MONEYTYPE_02, moneyamount = 200000 },

	},

	------------------------Media gift pack

	{
		func = 'MeiTi1',
		maxitemcount = 10,    --Required empty bag slots
		opentimesvariable = 193,
		maxopentimes = 1,

		{ itemid = 363, itemcount = 1 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 363, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ itemid = 373, itemcount = 1 },
		{ moneytype = MONEYTYPE_02, moneyamount = 50000 },

	},

	------------------------Alipay gift pack

	{
		func = 'ZhiFuBao1',
		maxitemcount = 5,    --Required empty bag slots
		opentimesvariable = 199,
		maxopentimes = 1,

		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },

	},

	------------------------CJ event gift pack

	{
		func = 'CJHuoDong1',
		maxitemcount = 6,    --Required empty bag slots
		opentimesvariable = 200,
		maxopentimes = 1,

		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 353, itemcount = 1, bind = true },
		{ itemid = 353, itemcount = 1, bind = true },
		{ itemid = 353, itemcount = 1, bind = true },

	},

	------------------------Open-beta media and event gift packs

	{
		func = '173TQGC',
		maxitemcount = 9,

		{ itemid = 388, itemcount = 1, bind = true },
		{ itemid = 388, itemcount = 1, bind = true },
		{ itemid = 388, itemcount = 1, bind = true },
		{ itemid = 388, itemcount = 1, bind = true },
		{ itemid = 388, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 69900008, itemcount = 1, bind = true },

	},

	{
		func = '173ZSGC',
		maxitemcount = 6,

		{ itemid = 388, itemcount = 1, bind = true },
		{ itemid = 388, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 69900007, itemcount = 1, bind = true },

	},

        {
		func = 'GongHuiZhiZun',
		maxitemcount = 19,
		opentimesvariable = 227,
		maxopentimes = 1,

		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 368, itemcount = 1, bind = true },		
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 91102, itemcount = 1, bind = true },
		{ itemid = 91102, itemcount = 1, bind = true },
		{ itemid = 91102, itemcount = 1, bind = true },
		{ itemid = 80203, itemcount = 1, bind = true },
		{ itemid = 80201, itemcount = 1, bind = true },
		{ itemid = 80202, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ title = 104 },		
	},

        {
		func = 'GongHuiDianCang',
		maxitemcount = 17,
		opentimesvariable = 228,
		maxopentimes = 1,

		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 368, itemcount = 1, bind = true },		
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 91102, itemcount = 1, bind = true },
		{ itemid = 91102, itemcount = 1, bind = true },
		{ itemid = 91102, itemcount = 1, bind = true },
		{ itemid = 80203, itemcount = 1, bind = true },
		{ itemid = 80201, itemcount = 1, bind = true },
		{ itemid = 80202, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ title = 105 },		
	},

        {
		func = 'DuoWanGC',
		maxitemcount = 12,
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 368, itemcount = 1, bind = true },		
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 80203, itemcount = 1, bind = true },
		{ itemid = 80201, itemcount = 1, bind = true },
		{ itemid = 80202, itemcount = 1, bind = true },
				
	},

        {
		func = 'XinLangGC',
		maxitemcount = 12,
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 368, itemcount = 1, bind = true },		
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 80203, itemcount = 1, bind = true },
		{ itemid = 80201, itemcount = 1, bind = true },
		{ itemid = 80202, itemcount = 1, bind = true },
				
	},

        {
		func = 'MeiTiGC',
		maxitemcount = 7,
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 80201, itemcount = 1, bind = true },
				
	},

        {
		func = 'ChuanYong',
		maxitemcount = 9,
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 387, itemcount = 1, bind = true },
				
	},

        {
		func = 'CaiYuanGunGun',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 880000 },
				
	},

        {
		func = 'LaoDaJieZhi',
		maxitemcount = 1,
		{ itemid = 69900008, itemcount = 1, bind = true },
				
	},

        {
		func = 'TianLongXiangLian',
		maxitemcount = 1,
		{ itemid = 69900009, itemcount = 1, bind = true },
				
	},

        {
		func = 'HuaTuoZaiShi',
		maxitemcount = 2,
		{ itemid = 382, itemcount = 1 },
		{ itemid = 383, itemcount = 1 },
				
	},

	{
		func = 'CanJuanHe',
		maxitemcount = 1,

		{ select = 
			{
				{itemid = 90217, itemcount = 1, weight = 2500},
				{itemid = 90218, itemcount = 1, weight = 5000},
				{itemid = 90219, itemcount = 1, weight = 2500},
			}
		},

	},


	------------------------Hero gift pack
	{
		func = 'YingXiongBSD',
		maxitemcount = 1,
		{ itemid = 99900025, itemcount = 1, class = clsWarrior },
		{ itemid = 99900028, itemcount = 1, class = clsMage },
		{ itemid = 99900027, itemcount = 1, class = clsTaoist },

	},


	------------------------Woma horn gift pack
	{
		func = 'HaoJiao',
		maxitemcount = 1,
		{ itemid = 80002, itemcount = 1, bind = true },

	},


	------------------Survey gift pack

	{
		func = 'wenjuan10',
		maxitemcount = 2,
		opentimesvariable = 153,
		maxopentimes = 1,

		{ moneytype = MONEYTYPE_02, moneyamount = 10000 },
		{ itemid = 361, itemcount = 1 },
		{ itemid = 361, itemcount = 1 },

	},
	{
		func = 'wenjuan20',
		maxitemcount = 2,
		opentimesvariable = 154,
		maxopentimes = 1,

		{ moneytype = MONEYTYPE_02, moneyamount = 20000 },
		{ itemid = 362, itemcount = 1 },
		{ itemid = 362, itemcount = 1 },

	},
	{
		func = 'wenjuan30',
		maxitemcount = 2,
		opentimesvariable = 155,
		maxopentimes = 1,

		{ moneytype = MONEYTYPE_02, moneyamount = 50000 },
		{ itemid = 381, itemcount = 1 },
		{ itemid = 381, itemcount = 1 },

	},

	------------------------Survey gift pack, money only

	{
		func = 'wenjuan0321',
	--	maxitemcount = 7,    --Required empty bag slots
		opentimesvariable = 157,
		maxopentimes = 1,
		{ moneytype = MONEYTYPE_02, moneyamount = 20000 },

	},


	------------------------Progression gift pack

	{
		func = 'ZhanShiChengZhangLiBao1',
		maxitemcount = 3,

		{ moneytype = MONEYTYPE_02, moneyamount = 10000 },
		{ itemid = 361, itemcount = 1, bind = true },
		{ itemid = 361, itemcount = 1, bind = true },
		{ itemid = 1380012, itemcount = 1, bind = true },

	},
	{
		func = 'ZhanShiChengZhangLiBao2',
		maxitemcount = 4,

		{ moneytype = MONEYTYPE_02, moneyamount = 20000 },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 99900010, itemcount = 1, bind = true },
		{ itemid = 1380013, itemcount = 1, bind = true },

	},
	{
		func = 'ZhanShiChengZhangLiBao3',
		maxitemcount = 5,

		{ moneytype = MONEYTYPE_02, moneyamount = 50000 },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 99980413, itemcount = 1, sex = cstMale, bind = true },
		{ itemid = 99990413, itemcount = 1, sex = cstFemale, bind = true },
		{ itemid = 99930016, itemcount = 1, bind = true },
		{ itemid = 1380014, itemcount = 1, bind = true },

	},
	{
		func = 'ZhanShiChengZhangLiBao4',
		maxitemcount = 8,

		{ moneytype = MONEYTYPE_02, moneyamount = 100000 },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99900019, itemcount = 1, bind = true },
		{ itemid = 99920016, itemcount = 1, bind = true },
		{ itemid = 21001, itemcount = 10, bind = true },
		{ itemid = 1380015, itemcount = 1, bind = true },

	},
	{
		func = 'ZhanShiChengZhangLiBao5',
		maxitemcount = 12,

		{ moneytype = MONEYTYPE_02, moneyamount = 300000 },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99930019, itemcount = 1, bind = true },
		{ itemid = 99940012, itemcount = 1, bind = true },
		{ itemid = 21001, itemcount = 20, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 10420, itemcount = 1, bind = true },
		{ itemid = 31034, itemcount = 1, bind = true },

	},

	{
		func = 'FaShiChengZhangLiBao1',
		maxitemcount = 3,

		{ moneytype = MONEYTYPE_02, moneyamount = 10000 },
		{ itemid = 361, itemcount = 1, bind = true },
		{ itemid = 371, itemcount = 1, bind = true },
		{ itemid = 1380022, itemcount = 1, bind = true },

	},
	{
		func = 'FaShiChengZhangLiBao2',
		maxitemcount = 4,

		{ moneytype = MONEYTYPE_02, moneyamount = 20000 },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },
		{ itemid = 99900009, itemcount = 1, bind = true },
		{ itemid = 1380023, itemcount = 1, bind = true },

	},
	{
		func = 'FaShiChengZhangLiBao3',
		maxitemcount = 5,

		{ moneytype = MONEYTYPE_02, moneyamount = 50000 },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99980423, itemcount = 1, sex = cstMale, bind = true },
		{ itemid = 99990423, itemcount = 1, sex = cstFemale, bind = true },
		{ itemid = 99930013, itemcount = 1, bind = true },
		{ itemid = 1380024, itemcount = 1, bind = true },

	},
	{
		func = 'FaShiChengZhangLiBao4',
		maxitemcount = 8,

		{ moneytype = MONEYTYPE_02, moneyamount = 100000 },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99900018, itemcount = 1, bind = true },
		{ itemid = 99920020, itemcount = 1, bind = true },
		{ itemid = 21002, itemcount = 10, bind = true },
		{ itemid = 1380025, itemcount = 1, bind = true },

	},
	{
		func = 'FaShiChengZhangLiBao5',
		maxitemcount = 12,

		{ moneytype = MONEYTYPE_02, moneyamount = 300000 },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99930020, itemcount = 1, bind = true },
		{ itemid = 99940016, itemcount = 1, bind = true },
		{ itemid = 21002, itemcount = 20, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 10320, itemcount = 1, bind = true },
		{ itemid = 32546, itemcount = 1, bind = true },

	},
	{
		func = 'DaoShiChengZhangLiBao1',
		maxitemcount = 3,

		{ moneytype = MONEYTYPE_02, moneyamount = 10000 },
		{ itemid = 361, itemcount = 1, bind = true },
		{ itemid = 371, itemcount = 1, bind = true },
		{ itemid = 1380032, itemcount = 1, bind = true },

	},
	{
		func = 'DaoShiChengZhangLiBao2',
		maxitemcount = 4,

		{ moneytype = MONEYTYPE_02, moneyamount = 20000 },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },
		{ itemid = 99900011, itemcount = 1, bind = true },
		{ itemid = 1380033, itemcount = 1, bind = true },

	},
	{
		func = 'DaoShiChengZhangLiBao3',
		maxitemcount = 5,

		{ moneytype = MONEYTYPE_02, moneyamount = 50000 },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99980433, itemcount = 1, sex = cstMale, bind = true },
		{ itemid = 99990433, itemcount = 1, sex = cstFemale, bind = true },
		{ itemid = 99930013, itemcount = 1, bind = true },
		{ itemid = 1380034, itemcount = 1, bind = true },

	},
	{
		func = 'DaoShiChengZhangLiBao4',
		maxitemcount = 8,

		{ moneytype = MONEYTYPE_02, moneyamount = 100000 },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99900021, itemcount = 1, bind = true },
		{ itemid = 99920021, itemcount = 1, bind = true },
		{ itemid = 21003, itemcount = 10, bind = true },
		{ itemid = 1380035, itemcount = 1, bind = true },

	},
	{
		func = 'DaoShiChengZhangLiBao5',
		maxitemcount = 12,

		{ moneytype = MONEYTYPE_02, moneyamount = 300000 },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99930021, itemcount = 1, bind = true },
		{ itemid = 99940010, itemcount = 1, bind = true },
		{ itemid = 21003, itemcount = 20, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 10110, itemcount = 1, bind = true },
		{ itemid = 33012, itemcount = 1, bind = true },

	},

	----------------------Revised mentor-to-student gift pack

	{
		func = 'Lv10HeXin',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },

	},
	{
		func = 'Lv10ZhuFuLiHe',
		maxitemcount = 3,

		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },
		{ itemid = 99990111, itemcount = 1, bind = true },

	},
	{
		func = 'Lv10ChuanShiLiHe',
		maxitemcount = 4,

		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },
		{ itemid = 99990111, itemcount = 1, bind = true },
		{ itemid = 99980213, itemcount = 1, sex = cstMale, bind = true },
		{ itemid = 99990213, itemcount = 1, sex = cstFemale, bind = true },

	},
	{
		func = 'Lv15HeXin',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },

	},
	{
		func = 'Lv15ZhuFuLiHe',
		maxitemcount = 3,

		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },
		{ itemid = 99900010, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 99900009, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 99900011, itemcount = 1, class = clsTaoist, bind = true },

	},
	{
		func = 'Lv15ChuanShiLiHe',
		maxitemcount = 5,

		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },
		{ itemid = 99990211, itemcount = 1, bind = true },
		{ itemid = 99980313, itemcount = 1, sex = cstMale, bind = true },
		{ itemid = 99990313, itemcount = 1, sex = cstFemale, bind = true },
		{ itemid = 99900010, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 99900009, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 99900011, itemcount = 1, class = clsTaoist, bind = true },

	},
	{
		func = 'Lv20HeXin',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },

	},
	{
		func = 'Lv20ZhuFuLiHe',
		maxitemcount = 3,

		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },
		{ itemid = 99900016, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 99900013, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 99900015, itemcount = 1, class = clsTaoist, bind = true },

	},
	{
		func = 'Lv20ChuanShiLiHe',
		maxitemcount = 6,

		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 372, itemcount = 1, bind = true },
		{ itemid = 99900016, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 99900013, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 99900015, itemcount = 1, class = clsTaoist, bind = true },
		{ itemid = 99920011, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 99920010, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 99920012, itemcount = 1, class = clsTaoist, bind = true },
		{ itemid = 99930008, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 99930009, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 99930007, itemcount = 1, class = clsTaoist, bind = true },
		{ itemid = 99940027, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 99940008, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 99940007, itemcount = 1, class = clsTaoist, bind = true },

	},
	{
		func = 'Lv25HeXin',
		maxitemcount = 2,

		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },

	},
	{
		func = 'Lv25ZhuFuLiHe',
		maxitemcount = 3,

		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99900020, itemcount = 1, bind = true },

	},
	{
		func = 'Lv25ChuanShiLiHe',
		maxitemcount = 5,

		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 99900020, itemcount = 1, bind = true },
		{ itemid = 99930010, itemcount = 1, bind = true },
		{ itemid = 99940009, itemcount = 1, bind = true },
	},


	------------------------Check-in gift pack

	{
		func = 'QianDaoJCY',
		maxitemcount = 2,

		{ itemid = 362, itemcount = 1, bind = true },

	},
	{
		func = 'QianDaoPiFeng',
		maxitemcount = 2,

		{ itemid = 99950003, itemcount = 1 },

	},
	{
		func = 'QianDaoTYS',
		maxitemcount = 1,

		{ itemid = 381, itemcount = 1, bind = true },

	},
	{
		func = 'QianDaoLingShi',
		maxitemcount = 2,

		{ itemid = 10420, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 10320, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 10110, itemcount = 1, class = clsTaoist, bind = true },

	},
	{
		func = 'QianDaoMingWen',
		maxitemcount = 2,

		{ itemid = 21001, itemcount = 5, class = clsWarrior, bind = true },
		{ itemid = 21002, itemcount = 5, class = clsMage, bind = true },
		{ itemid = 21003, itemcount = 5, class = clsTaoist, bind = true },

	},
	{
		func = 'QianDaoHeiTie',
		maxitemcount = 6,

		{ itemid = 114015, itemcount = 1, bind = true },
		{ itemid = 114015, itemcount = 1, bind = true },
		{ itemid = 114015, itemcount = 1, bind = true },
		{ itemid = 114015, itemcount = 1, bind = true },
		{ itemid = 114015, itemcount = 1, bind = true },

	},
	{
		func = 'QianDaoJinBi',
		maxitemcount = 1,

		{ moneytype = MONEYTYPE_02, moneyamount = 100000 },

	},
	{
		func = 'QianDaoShiPin',
		maxitemcount = 5,

		{ itemid = 99920022, itemcount = 1, bind = true },
		{ itemid = 99930025, itemcount = 1, percent = 30, bind = true },
		{ itemid = 99930028, itemcount = 1, percent = 30, bind = true },
		{ itemid = 99930030, itemcount = 1, percent = 10, bind = true },

	},
	{
		func = 'QianDaoJiNeng',
		maxitemcount = 2,

		{ itemid = 31036, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 32535, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 33008, itemcount = 1, class = clsTaoist, bind = true },

	},
	{
		func = 'QianDaoCJJCY',
		maxitemcount = 6,

		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },
		{ itemid = 363, itemcount = 1, bind = true },

	},
	{
		func = 'QianDaoCJMFY',
		maxitemcount = 6,

		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },
		{ itemid = 373, itemcount = 1, bind = true },

	},
	{
		func = 'QianDaoQXTYS',
		maxitemcount = 6,

		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },
		{ itemid = 382, itemcount = 1, bind = true },

	},
	{
		func = 'QianDaoZFY',
		maxitemcount = 4,

		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, bind = true },

	},

	------------------------Closed-beta media gift pack

	{
		func = '17173FC',
		maxitemcount = 3,

		{ moneytype = MONEYTYPE_02, moneyamount = 50000 },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 69900001, itemcount = 1, bind = true },

	},
	{
		func = 'XinLangFC',
		maxitemcount = 6,

		{ moneytype = MONEYTYPE_02, moneyamount = 40000 },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 69900001, itemcount = 1, bind = true },

	},
	{
		func = 'DuoWanFC',
		maxitemcount = 6,

		{ moneytype = MONEYTYPE_02, moneyamount = 40000 },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 69900001, itemcount = 1, bind = true },

	},
	{
		func = 'GongHuiFC',
		maxitemcount = 2,

		{ moneytype = MONEYTYPE_02, moneyamount = 60000 },
		{ itemid = 362, itemcount = 1, bind = true },
		{ itemid = 362, itemcount = 1, bind = true },

	},

	------------------------Mafa Legend experience gift pack

	{
		func = 'MFCSJingYan1',
		maxitemcount = 0,
		{ exp = 3300 },

	},
	{
		func = 'MFCSJingYan2',
		maxitemcount = 0,
		{ exp = 8700 },

	},
	{
		func = 'MFCSJingYan3',
		maxitemcount = 0,
		{ exp = 17000 },

	},
	{
		func = 'MFCSJingYan4',
		maxitemcount = 0,
		{ exp = 63000 },

	},
	{
		func = 'MFCSJingYan5',
		maxitemcount = 0,
		{ exp = 122000 },

	},
	{
		func = 'MFCSJingYan6',
		maxitemcount = 0,
		{ exp = 152000 },

	},
	{
		func = 'MFCSJingYan7',
		maxitemcount = 0,
		{ exp = 204000 },

	},
	{
		func = 'MFCSJingYan8',
		maxitemcount = 0,
		{ exp = 276000 },

	},
	{
		func = 'MFCSJingYan9',
		maxitemcount = 0,
		{ exp = 300000 },

	},


	------------------------Mafa Legend gold gift pack

	{
		func = 'MFCSJinBi1',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 3000 },

	},
	{
		func = 'MFCSJinBi2',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 8000 },

	},
	{
		func = 'MFCSJinBi3',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 15000 },

	},
	{
		func = 'MFCSJinBi4',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 25000 },

	},
	{
		func = 'MFCSJinBi5',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 40000 },

	},
	{
		func = 'MFCSJinBi6',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 50000 },

	},
	{
		func = 'MFCSJinBi7',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 70000 },

	},
	{
		func = 'MFCSJinBi8',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 100000 },

	},
	{
		func = 'MFCSJinBi9',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 110000 },

	},


	------------------------Royal Treasury

	{
		func = 'HuangShiBaoXiang',
		maxitemcount = 1,

		{ select = 
			{
				{itemid = 99900019, itemcount = 1, weight = 50},
				{itemid = 99900018, itemcount = 1, weight = 50},
				{itemid = 99900021, itemcount = 1, weight = 50},
				{itemid = 99920016, itemcount = 1, weight = 50},
				{itemid = 99920020, itemcount = 1, weight = 50},
				{itemid = 99920021, itemcount = 1, weight = 50},
				{itemid = 99930018, itemcount = 1, weight = 50},
				{itemid = 99930017, itemcount = 1, weight = 50},
				{itemid = 99930014, itemcount = 1, weight = 50},
				{itemid = 99940020, itemcount = 1, weight = 50},
				{itemid = 99940011, itemcount = 1, weight = 50},
				{itemid = 99940015, itemcount = 1, weight = 50},
				{itemid = 99990315, itemcount = 1, weight = 200},
				{itemid = 99990317, itemcount = 1, weight = 200},
				{itemid = 99990415, itemcount = 1, weight = 20},
				{itemid = 99990417, itemcount = 1, weight = 20},
				{itemid = 80003, itemcount = 1, weight = 300},
				{itemid = 303, itemcount = 1, weight = 2500},
				{itemid = 323, itemcount = 1, weight = 2500},
				{itemid = 352, itemcount = 1, weight = 3550},
				{itemid = 99990312, itemcount = 1, weight = 50},
				{itemid = 99990322, itemcount = 1, weight = 50},
				{itemid = 99990332, itemcount = 1, weight = 50},

			},

			announce_expensive = ' {role_name} got an item from the Royal Chest: [item_name]',
		},

	},


	------------------------Level 1 soul stone box

	{
		func = 'LingShiHe1',
		maxitemcount = 1,

		{ select = 
			{
				{itemid = 10110, itemcount = 1, weight = 50},
				{itemid = 10120, itemcount = 1, weight = 50},
				{itemid = 10220, itemcount = 1, weight = 50},
				{itemid = 10320, itemcount = 1, weight = 50},
				{itemid = 10420, itemcount = 1, weight = 50},
				{itemid = 10520, itemcount = 1, weight = 50},
			}
		},

	},

	------------------------Level 2 soul stone box

	{
		func = 'LingShiHe2',
		maxitemcount = 1,

		{ select = 
			{
				{itemid = 10111, itemcount = 1, weight = 50},
				{itemid = 10121, itemcount = 1, weight = 50},
				{itemid = 10221, itemcount = 1, weight = 50},
				{itemid = 10321, itemcount = 1, weight = 50},
				{itemid = 10421, itemcount = 1, weight = 50},
				{itemid = 10521, itemcount = 1, weight = 50},
			}
		},

	},

	{
		func = 'DecPKValue10',
		maxitemcount = 0,
		{ pkvalue = 10 },
	},
	{
		func = 'DecPKValue50',
		maxitemcount = 0,
		{ pkvalue = 50 },
	},
	{
		func = 'DecPKValue200',
		maxitemcount = 0,
		{ pkvalue = 200 },
	},
	{
		func = 'RouGanDecPKValue',
		maxitemcount = 0,
		{ pkvalue = 5, chance = 50 },
	},

	------------------------Spare gift pack

	{
		func = 'GongHuiBY',
		maxitemcount = 0,
		{ exp = 300 },

	},
	{
		func = 'GanEnBY',
		maxitemcount = 0,
		{ exp = 300 },

	},
	{
		func = 'BuChangBY',
		maxitemcount = 0,
		{ exp = 300 },

	},
	{
		func = 'YaoShuiBY',
		maxitemcount = 0,
		{ exp = 300 },

	},
	{
		func = 'ZhuangBeiBY',
		maxitemcount = 0,
		{ exp = 300 },

	},
	{
		func = 'JingYanBY',
		maxitemcount = 0,
		{ exp = 300 },

	},
	{
		func = 'ShenMiBY',
		maxitemcount = 0,
		{ exp = 300 },

	},
	{
		func = 'JingXiBY',
		maxitemcount = 0,
		{ exp = 300 },

	},

	------------------------Fireworks and cake
	{
		func = 'FireWorksCake',
		maxitemcount = 0,
		{ doodadid = 3479},
		{ announce = '{role_name} lavishly set off cake fireworks to send everyone blessings!'},
	},

		{
		func = 'FireWorks1',
		maxitemcount = 0,
		{ doodadid = 3480},
	},

	------------------------Check-in gift pack
	------------------------Medium red pack
	{
		func = 'QianDao1',
		maxitemcount = 3,
		{ itemid = 367, itemcount = 1, bind = true },
		{ itemid = 367, itemcount = 1, bind = true },
		{ itemid = 367, itemcount = 1, bind = true },
	},

	------------------------Medium blue pack
	{
		func = 'QianDao4',
		maxitemcount = 3,
		{ itemid = 377, itemcount = 1, bind = true },
		{ itemid = 377, itemcount = 1, bind = true },
		{ itemid = 377, itemcount = 1, bind = true },
	},

	------------------------Mystery gift pack
	{
		func = 'QianDao5',
		maxitemcount = 10,
		{itemid = 80003, itemcount = 1, percent = 2, bind = true},
		{itemid = 21001, itemcount = 1, percent = 5, class = clsWarrior, bind = true },
		{itemid = 21002, itemcount = 1, percent = 5, class = clsMage, bind = true },
		{itemid = 21003, itemcount = 1, percent = 5, class = clsTaoist, bind = true },
		{itemid = 2212, itemcount = 5, percent = 10, bind = true},
		{itemid = 2201, itemcount = 3, percent = 5, bind = true},
		{itemid = 705, itemcount = 1, percent = 10, bind = true},
		{itemid = 80203, itemcount = 1, percent = 3, bind = true},
		{itemid = 114020, itemcount = 1, percent = 10, bind = true},
		{itemid = 387, itemcount = 1, percent = 100, bind = true},
		{itemid = 701, itemcount = 1, percent = 10, class = clsWarrior, bind = true},
		{itemid = 702, itemcount = 1, percent = 10, class = clsMage, bind = true},
		{itemid = 703, itemcount = 1, percent = 10, class = clsTaoist, bind = true},
				
	},

	------------------------Sun water pack
	{
		func = 'QianDao6',
		maxitemcount = 3,
		{ itemid = 386, itemcount = 1, bind = true },
		{ itemid = 386, itemcount = 1, bind = true },
		{ itemid = 386, itemcount = 1, bind = true },
	},

	------------------------Mining pack
	{
		func = 'QianDao7',
		maxitemcount = 2,
		{ itemid = 99900041, itemcount = 1, bind = true },
		{ itemid = 99900040, itemcount = 1, bind = true },
		
	},

	------------------------Divine water
	{
		func = 'QianDao8',
		maxitemcount = 1,                
		{ itemid = 701, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 702, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 703, itemcount = 1, class = clsTaoist, bind = true },
		
	},

	------------------------Large red pack
	{
		func = 'QianDao10',
		maxitemcount = 3,
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 368, itemcount = 1, bind = true },
		{ itemid = 368, itemcount = 1, bind = true },
	},

	------------------------Large blue pack
	{
		func = 'QianDao11',
		maxitemcount = 3,
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
		{ itemid = 378, itemcount = 1, bind = true },
	},

	------------------------Divine water pack
	{
		func = 'QianDao13',
		maxitemcount = 2,
		{ itemid = 705, itemcount = 1, bind = true },
		{ itemid = 706, itemcount = 1, bind = true },		
	},

	------------------------Inscription stone
	{
		func = 'QianDao22',
		maxitemcount = 1,
		{ itemid = 21001, itemcount = 1, class = clsWarrior, bind = true },
		{ itemid = 21002, itemcount = 1, class = clsMage, bind = true },
		{ itemid = 21003, itemcount = 1, class = clsTaoist, bind = true },

	},

	------------------------Black iron ore
	{
		func = 'QianDao24',
		maxitemcount = 5,
		{ itemid = 114015, itemcount = 1, bind = true },
		{ itemid = 114015, itemcount = 1, bind = true },
		{ itemid = 114015, itemcount = 1, bind = true },
		{ itemid = 114015, itemcount = 1, bind = true },
		{ itemid = 114015, itemcount = 1, bind = true },
	},


	------------------------Check-in gift pack appendix
	------------------------Gold pack
	{
		func = 'QianDao50',
		maxitemcount = 0,
		{ moneytype = MONEYTYPE_02, moneyamount = 10000 },
	},


	------------------------Mercenary spirit box
	{
		func = 'YongBingLingXia',
		maxitemcount = 0,
		{ exp = 10000 },
	},


	------------------------Activity gift pack
	{
		func = 'HuoYue1',
		maxitemcount = 2,
		{ itemid = 386, itemcount = 1, bind = true },
		{ itemid = 99900042, itemcount = 1 },
	},
	{
		func = 'HuoYue2',
		maxitemcount = 2,
		{ itemid = 386, itemcount = 1, bind = true },
		{ itemid = 99900042, itemcount = 1 },
	},
	{
		func = 'HuoYue3',
		maxitemcount = 3,
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 99900042, itemcount = 1 },
		{ select = 
			{
				{itemid = 111006, itemcount = 1, weight = 50},
				{itemid = 111009, itemcount = 1, weight = 50},
			}, percent = 50
		},
	},
	{
		func = 'HuoYue4',
		maxitemcount = 3,
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 99900042, itemcount = 1 },
		{ select = 
			{
				{itemid = 111006, itemcount = 1, weight = 50},
				{itemid = 111009, itemcount = 1, weight = 50},
			}, percent = 50
		},
	},
	{
		func = 'HuoYue5',
		maxitemcount = 3,
		{ itemid = 387, itemcount = 1, bind = true },
		{ itemid = 80003, itemcount = 1, percent = 5 },
		{ select = 
			{
				{itemid = 21001, itemcount = 1, weight = 50},
				{itemid = 21002, itemcount = 1, weight = 50},
				{itemid = 21003, itemcount = 1, weight = 50},
			}, percent = 15
		},
		{ select = 
			{
				{itemid = 111006, itemcount = 1, weight = 50},
				{itemid = 111009, itemcount = 1, weight = 50},
			}, percent = 50
		},
	},


};

ANNOUNCE_REPL_STRING_FUNC_TABLE =
{
	role_name = function (pRole)
		return pRole:GetName();
	end,

	item_name = function (d)
		return d.m_szName;
	end,

};


function _award(v, class, sex, itembag, pRole, itemid)
	if v.class and v.class ~= class then
		return;
	end

	if v.sex and v.sex ~= sex then
		return;
	end

	if v.percent then
		if math.random(100) >= v.percent then
			return;
		end
	end

	if v.select then
		local totalweight = 0;
		for index,item in ipairs(v.select) do
			if item.itemid and item.itemcount and item.weight and item.weight > 0 then
				totalweight = totalweight + item.weight;
				print('debug: index:' .. index .. ', itemid:' .. item.itemid .. ', itemcount:' .. item.itemcount .. ', weight:' .. item.weight);
			else
				print('wrong setting for package select');
				return;
			end
		end
		print('debug: totalweight:' .. totalweight);
		local random_number = math.random(totalweight);
		print ('debug: random_number:' .. random_number);
		for i=1,#v.select do
			if random_number <= v.select[i].weight then
					local item_data = GetItemData(v.select[i].itemid);
					itembag:AddItem(v.select[i].itemid, v.select[i].itemcount, ITEMLOG_PACKAGE, itemid, false);
					print("debug: get item :"..item_data.m_szName);

					if v.announce_expensive and item_data.m_byExpensiveItem == 1 then
							local msg = string.gsub(v.announce_expensive, "%\[(.-)%\]",
							function (s)
									if ANNOUNCE_REPL_STRING_FUNC_TABLE[s] and type(ANNOUNCE_REPL_STRING_FUNC_TABLE[s]) == 'function' then
											return ANNOUNCE_REPL_STRING_FUNC_TABLE[s](item_data);
									end
							end
							);

							msg = string.gsub(msg, "%{(.-)%}",
							function (s)
									if ANNOUNCE_REPL_STRING_FUNC_TABLE[s] and type(ANNOUNCE_REPL_STRING_FUNC_TABLE[s]) == 'function' then
											return ANNOUNCE_REPL_STRING_FUNC_TABLE[s](pRole);
									end
							end
							);

							local scene = pRole:GetScene();

							scene:Say(cctServer, cltNormal, msg);
					end


					break;
			end
			random_number = random_number - v.select[i].weight;
		end
	elseif v.itemid and v.itemcount then
		if (v.bind) then
			itembag:AddItem(v.itemid, v.itemcount, ITEMLOG_PACKAGE, itemid, true);
		else
			itembag:AddItem(v.itemid, v.itemcount, ITEMLOG_PACKAGE, itemid, false);
		end
	elseif v.exp then
        	pRole:AddExp(v.exp,false,false);
	elseif v.moneytype and v.moneyamount then
   		itembag:AddMoney(v.moneytype, v.moneyamount, MONEYLOG_PACKAGE, 0, 0);
		if moneytype == MONEYTYPE_MASTER_EXP and pRole:GetMaster() then
			pRole:GetMaster():LuaSyncMasterExpToMS(pRole);
		end
	elseif v.title then
		pRole:GetTitle():AddTitle(v.title);
	elseif v.questid and v.questvar and v.questvalue then
		local quest = pRole:GetQuestList():GetAcceptedQuest(v.questid);
		if quest then
			pRole:GetQuestList():SetVariable(quest, v.varid, v.varvalue);
		end
	elseif v.doodadid then
		local scene = pRole:GetScene();
		local doodad = scene:SpawnDoodad(v.doodadid, pRole:GetX(), pRole:GetY(), pRole:GetZ(), pRole:GetFaceDirection());
	elseif v.pkvalue then
		local tempPkValue = pRole:GetPkValue();
		if v.chance == nil or math.random(1,100) <= v.chance then
			tempPkValue = tempPkValue - v.pkvalue;
		end
		if tempPkValue < 0 then
			tempPkValue = 0;
		end

		pRole:SetPkValue(tempPkValue);
	elseif v.aura then
		local aurainfo = CSpellConfig:GetInstance():FindAuraTemplate(v.aura);
		pRole:AppendAura(aurainfo, pRole, 0, NULL, 0, 0, 0, 1);
	end

	if v.announce then
		local message = string.gsub(v.announce, "%{(.-)%}",
			function (s)
				if ANNOUNCE_REPL_STRING_FUNC_TABLE[s] and type(ANNOUNCE_REPL_STRING_FUNC_TABLE[s]) == 'function' then
					return ANNOUNCE_REPL_STRING_FUNC_TABLE[s](pRole);
				end
			end
		);
		pRole:Say(cctGroup, cltTvShow, message);
	end

end

for _,p in ipairs(PACKAGE_TABLE) do
	if _G[p.func] then
		print('detected duplicated package function name '..p.func);
	end
	_G[p.func] = function(pItem, pRole)
		local itembag = pRole:GetItemBag();
		local free_slot_count = itembag:GetFreeSlots(CONSLOT_DEFAULT);
		local class = pRole:GetClass();
		local sex = pRole:GetSex();
		local itemid = pItem.m_pTemplate.m_nTemplateID;

		--------CQT2 additional survey by wubin 2015.03.21
		--if (pRole:GetLevel() >= 15 and pRole:GetVariable(156) < 1) then
		--	pRole:Say(pRole:GetObjID(), cltUrl, "Welcome to take survey:‘http://wenjuan.sdo.com:9091/survey/survey?surveyId=8292’");
		--	pRole:SetVariable(156, 1);
		--end
		--------CQT2 additional survey end

		if (free_slot_count < p.maxitemcount) then
			return errItemBagFull;
		end

		if (p.checkvariable and p.waittime) then
			if (pRole:GetTotalOnlineTime() - pRole:GetVariable(p.checkvariable) < p.waittime) then
				return errItemOpenPackageEarly;
			end
		end

		if (p.opentimesvariable and p.maxopentimes) then
			local open_times = pRole:GetVariable(p.opentimesvariable);
			if (open_times >= p.maxopentimes) then
				return errItemOpenPackageTooMany;
			else
				pRole:SetVariable(p.opentimesvariable, open_times+1);
			end
		end

		for _,v in ipairs(p) do
			if type(v) == 'table' then
				_award(v, class, sex, itembag, pRole, itemid);
			end
		end

		return errSuccess;

	end
end
