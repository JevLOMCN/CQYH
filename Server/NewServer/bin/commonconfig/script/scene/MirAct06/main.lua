LoadPackage("gameworld.pkg")
LoadPackage("role.pkg")
LoadPackage("scene.pkg")
LoadPackage("chat_def.pkg")
LoadPackage("rank_list_def.pkg")
LoadPackage("char_def.pkg")

Include("script/include/btnod.lua")

EXP_TABLE = 
{
		[1]=9600,
		[2]=12800,
		[3]=16800,
		[4]=21600,
		[5]=27200,
		[6]=35200,
		[7]=44800,
		[8]=56000,
		[9]=72000,
};


ACT_Pts_TABLE = 
{
		[1]=2,
		[2]=4,
		[3]=6,
		[4]=8,
		[5]=10,
		[6]=12,
		[7]=14,
		[8]=16,
		[9]=18,
};

local g_oGameWorld = CGameWorld:GetInstance();

local MAX_PASS_TIME = 86400;

function save_start_time(scene)
	scene:SetVariable(0, g_oGameWorld:GetGameTime());
	scene:SetVariable(2, 1);

	return brvSuccess;
end

function show_current_time(scene)
	local startTime = scene:GetVariable(0);
	local passedTime = g_oGameWorld:GetGameTime() - startTime;
	local min = math.floor(passedTime / 60);
	local sec = math.floor(passedTime % 60);
	local message = "Your total time spent so far is ";
	if min > 0 then
		message = message..min.."minutes";
	end
	if sec > 0 then
		message = message..sec.."s";
	end
	scene:Say(cctScene, cltTvShow, message);

	return brvSuccess;
end

function save_role_id(scene)
	scene:SetVariable(1, get_ctx_role_id());

	return brvSuccess;
end


-- add_exp also adds activity points (ACT_Pts)
function add_exp(scene, wave_id)
	local roleID = scene:GetVariable(1);
	local role = g_oGameWorld:FindValidRole(roleID);

	local message = ""

	if role then
		local exp = EXP_TABLE[wave_id];
		local apt = ACT_Pts_TABLE[wave_id];

		role:AddExp(exp, false, true);
		-- role:AddActivityPts(apt,aprYaoTa)    -- Default code adds activity points; temporarily disabled because the function is missing

		message = "Congratulations, you got No. ["..wave_id.."] waves of EXP: total ["..exp.."] EXP."
		scene:Say(cctScene, cltTvShow, message);
		return brvSuccess;
	end

	return brvFail;
end

function check_record(scene, wave_id)
	local roleID = scene:GetVariable(1);
	local role = g_oGameWorld:FindValidRole(roleID);

	local logmsg = ""
	if role then
		local best_wave_id = role:GetVariable(180);
		local best_time = role:GetVariable(181);

		local startTime = scene:GetVariable(0);
		local passedTime = g_oGameWorld:GetGameTime() - startTime;


		if passedTime > MAX_PASS_TIME  then
			passedTime = MAX_PASS_TIME;
		end

		local min = math.floor( (passedTime - 2 ) / 60);
		local sec = math.floor( (passedTime - 2 ) % 60);
		local message = "";
		if min > 0 then
			message = message..min.."minutes";
		end
		if sec > 0 then
			message = message..sec.."s";
		end

		if wave_id > best_wave_id then
			local roleClass = role:GetClass();
			role:SetVariable(180, wave_id);
			role:SetVariable(181, passedTime);
			if roleClass == clsWarrior then
				role:UpdateRankListNode(rlActivity02, wave_id, MAX_PASS_TIME - passedTime);
			elseif roleClass == clsMage then
				role:UpdateRankListNode(rlActivity03, wave_id, MAX_PASS_TIME - passedTime);
			elseif roleClass == clsTaoist then
				role:UpdateRankListNode(rlActivity04, wave_id, MAX_PASS_TIME - passedTime);
			end
			-------------------------test--------------
			logmsg = "First clear ["..wave_id.."] waves, new personal best: "..message
			scene:Say(cctScene, cltTvShow, logmsg);
			-----------------------------------------------
		elseif wave_id == best_wave_id and passedTime < best_time then
			local roleClass = role:GetClass();
			role:SetVariable(181,passedTime);
			if roleClass == clsWarrior then
				role:UpdateRankListNode(rlActivity02, wave_id, MAX_PASS_TIME - passedTime);
			elseif roleClass == clsMage then
				role:UpdateRankListNode(rlActivity03, wave_id, MAX_PASS_TIME - passedTime);
			elseif roleClass == clsTaoist then
				role:UpdateRankListNode(rlActivity04, wave_id, MAX_PASS_TIME - passedTime);
			end
			-------------------------test--------------
			logmsg = "Currently passed ["..wave_id.."] waves, new personal best: "..message
			scene:Say(cctScene, cltTvShow, logmsg);
			-----------------------------------------------			
		end
	end

	return brvSuccess;
end


Include("script/scene/MirAct06/scene_bt.lua")

