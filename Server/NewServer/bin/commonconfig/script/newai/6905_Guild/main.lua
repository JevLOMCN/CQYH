Include("script/stringtable/stringtable.lh")
LoadPackage("chat_def.pkg")
Include("script/newai/sub_tree/sub_tree.lh")
LoadPackage("gameworld.pkg")
LoadPackage("role.pkg")
LoadPackage("character.pkg")
LoadPackage("npc.pkg")
LoadPackage("guild_def.pkg")
LoadPackage("item_bag.pkg")
local g_oGameWorld = CGameWorld:GetInstance();

function RoleExp(owner)
	local processor = owner:GetAIProcessor();
	local npc = Cast(processor:GetOwner(), 'CNpc');

	local role_id = get_ctx_role_id();
	local role = CGameWorld:GetInstance():FindRole(role_id);
	local exp_a = 10 * math.pow(role:GetLevel(),2) + 250 * role:GetLevel() + 1500 ;
	      -- 10 * level^2 + 250 * level + 1500
	local final_exp = 0;
	
	local r = math.random(1, 100);
	if r > 20 then  -- 80%
		local p = math.random(50,80);
		final_exp = exp_a * p / 500;	
		--npc:Say(cctScene, cltTvShow, ""..role:GetName().." prayed and gained a large amount of EXP.");
	elseif r > 5 then  -- 15%
		local p = math.random(80,200);
		final_exp = exp_a * p / 500;
	elseif r > 1 then   -- 4%
		local p = math.random(200,500);
		final_exp = exp_a * p / 500;
	else	-- 1%
		local p = math.random(500,2000);
		final_exp = exp_a * p / 500;		
		npc:Say(cctScene, cltTvShow, ""..role:GetName().." prayed and gained a large amount of EXP.");
	end
	
	role:AddExp(final_exp, false, true);
	return brvSuccess;
end

function CheckEnterCondition(owner)
	local number = role:GetVariable(1004);
	if number1 > 0 then
		return brvFail;
	else 
		return brvSuccess;
	end
end

function Setv_1(owner) --Check parity for the experience session (which should receive an odd number)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local Time = role:GetVariable(1004);
	local a =  math.fmod(Time,2); --Take the timestamp remainder
	if a == 1 then 
		role:SetVariable(1004,Time);
		return brvSuccess;
	else
		role:SetVariable(1004,Time + 1);
		local c =role:GetVariable(1004);
		return brvSuccess;
	end
	print(a);
	print(c);
end

function Setv_2(owner)--Check parity for the ingot session (which should receive an even number)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local Time = role:GetVariable(1004);
	local b =  math.fmod(Time,2); --Take the timestamp remainder
	if b == 0 then 
		role:SetVariable(1004,Time);
		return brvSuccess;
	else
		role:SetVariable(1004,Time + 1);
		return brvSuccess;
	end

end

function Check_title(owner)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());	
	local d = role:GetTitle():FindTitle(10);
	if d == true then
		return brvSuccess;
	else
		role:GetScene():Say(role:GetObjID(), cltTvShow, "You do not have the United Will title and cannot enter the exclusive reward map");
		return brvFail;
	end	

end

function CheckDay(owner)
	local a =  os.date("%w",os.time());
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	if tonumber(a) == 6 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Cannot enter the exclusive reward map on Saturday");
		return brvFail;
	else
		return brvSuccess;
	end	

end

Include("script/include/btnod.lua")
Include("script/newai/6905_Guild/main.uh")