LoadPackage("gameworld.pkg")
LoadPackage("role.pkg")
LoadPackage("scene.pkg")
LoadPackage("chat_def.pkg")
LoadPackage("item_bag.pkg")

Include("script/include/btnod.lua")

local g_oGameWorld = CGameWorld:GetInstance();

EXP_TABLE = 
{
		[1]=321,
		[2]=346,
		[3]=373,
		[4]=404,
		[5]=437,
		[6]=474,
		[7]=513,
		[8]=556,
		[9]=601,
		[10]=650,
		[11]=701,
		[12]=756,
		[13]=813,
		[14]=874,
		[15]=937,
		[16]=1004,
		[17]=1073,
		[18]=1146,
		[19]=1221,
		[20]=1300,
		[21]=1381,
		[22]=1466,
		[23]=1553,
		[24]=1644,
		[25]=1737,
		[26]=1834,
		[27]=1933,
		[28]=2036,
		[29]=2141,
		[30]=2250,
		[31]=2361,
		[32]=2476,
		[33]=2593,
		[34]=2714,
		[35]=2837,
		[36]=2964,
		[37]=3093,
		[38]=3226,
		[39]=3361,
		[40]=3500,
		[41]=3641,
		[42]=3786,
		[43]=3933,
		[44]=4084,
		[45]=4237,
		[46]=4394,
		[47]=4553,
		[48]=4716,
		[49]=4881,
		[50]=5050,
		[51]=5221,
		[52]=5396,
		[53]=5573,
		[54]=5754,
		[55]=5937,
		[56]=6124,
		[57]=6313,
		[58]=6506,
		[59]=6701,
		[60]=6900,
		[61]=7101,
		[62]=7306,
		[63]=7513,
		[64]=7724,
		[65]=7937,
		[66]=8154,
		[67]=8373,
		[68]=8596,
		[69]=8821,
		[70]=9050,
		[71]=9281,
		[72]=9516,
		[73]=9753,
		[74]=9994,
		[75]=10237,
		[76]=10484,
		[77]=10733,
		[78]=10986,
		[79]=11241,
		[80]=11500,
		[81]=11761,
		[82]=12026,
		[83]=12293,
		[84]=12564,
		[85]=12837,
		[86]=13114,
		[87]=13393,
		[88]=13676,
		[89]=13961,
		[90]=14250,
		[91]=14541,
		[92]=14836,
		[93]=15133,
		[94]=15434,
		[95]=15737,
		[96]=16044,
		[97]=16353,
		[98]=16666,
		[99]=16981,
		[100]=17300,
};

function send_kill_info(owner)
	local role =  g_oGameWorld:FindValidRole(get_ctx_role_id());
	local killer = g_oGameWorld:FindValidRole(role:GetLastCasterID());
	if role and killer then
		local exp = EXP_TABLE[role:GetLevel()];
		killer:AddExp(exp, false, true);
		local message = "You have successfully slain "..role:GetName()..", received "..exp.."EXP Reward";
		local ret = role:Say(killer:GetObjID(), cltNormal, message);
		if ret then
			return brvSuccess;
		end
	end

	return brvFail;
end

Include("script/include/btnod.lua")
Include("script/scene/MirAct01/scene_bt.lua")

