Include("script/stringtable/stringtable.lh")
LoadPackage("chat_def.pkg")
LoadPackage("guild_def.pkg")
LoadPackage("npc.pkg")
LoadPackage("scene.pkg")
LoadPackage("gs_guild.pkg")
LoadPackage("guild_mgr.pkg")
LoadPackage("item.pkg")
LoadPackage("item_bag.pkg")
LoadPackage('gameworld.pkg');
LoadPackage("role.pkg");
LoadPackage("title.pkg");
Include("script/newai/sub_tree/sub_tree.lh");

	
local g_oGameWorld = CGameWorld:GetInstance();
local Warrior_Top = 0;
local Mage_Top = 0;
local Taoist_Top = 0;
local Warrior_RoleID = 0;
local Mage_RoleID = 0;
local Taoist_RoleID = 0;

function get_power(owner)
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	local cls = role:GetClass();
	if cls == clsWarrior then
		return Warrior_Top;
	elseif cls == clsMage then
		return Mage_Top;
	elseif cls == clsTaoist then
		return Taoist_Top;
	else
		return brvFail;
	end
end

function Occupied_Top(owner)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local cls = role:GetClass();
	if cls == clsWarrior then
		local role_power = role:GetPow();
		if Warrior_RoleID ==  get_ctx_role_id() then
			role:SetVariable(1009, 1009);
			role:GetScene():Say(role:GetObjID(), cltTvShow, "Congratulations, you became Number One Under Heaven and gained a twenty percent Final Damage bonus status.");
			local msg = "Congratulations "..role:GetName().."Become the best in the world and gain a twenty percent Final Damage bonus status.";
			role:Say(cctServer, cltLighting, msg);
			return brvSuccess;
		end
		if role_power > Warrior_Top then
			local Last_Top_role = CGameWorld:GetInstance():FindValidRole(Warrior_RoleID);
			if Last_Top_role ~= nil then
				Last_Top_role:SetVariable(1009, 0);
				Last_Top_role:GetScene():Say(Last_Top_role:GetObjID(), cltTvShow, "Your title of Number One Under Heaven was taken by someone else. Raise your power and take it back.");
			end
			Warrior_Top = role_power;
			Warrior_RoleID = get_ctx_role_id();
			role:SetVariable(1009, 1009);
			role:GetScene():Say(role:GetObjID(), cltTvShow, "Congratulations, you became Number One Under Heaven and gained a twenty percent Final Damage bonus status.");
			local msg = "Congratulations "..role:GetName().."Become the best in the world and gain a twenty percent Final Damage bonus status.";
			role:Say(cctServer, cltLighting, msg);
			return brvSuccess;	
		end
	elseif cls == clsMage then
		local role_power = role:GetPow();
		if Mage_RoleID ==  get_ctx_role_id() then
			role:SetVariable(1009, 1009);
			role:GetScene():Say(role:GetObjID(), cltTvShow, "Congratulations, you became Number One Under Heaven and gained a twenty percent Final Damage bonus status.");
			local msg = "Congratulations "..role:GetName().."Become the best in the world and gain a twenty percent Final Damage bonus status.";
			role:Say(cctServer, cltLighting, msg);
			return brvSuccess;
		end
		if role_power > Mage_Top then
			local Last_Top_role = CGameWorld:GetInstance():FindValidRole(Mage_RoleID);
			if Last_Top_role ~= nil then
				Last_Top_role:SetVariable(1009, 0);
				Last_Top_role:GetScene():Say(Last_Top_role:GetObjID(), cltTvShow, "Your title of Number One Under Heaven was taken by someone else. Raise your power and take it back.");
			end
			Mage_Top = role_power;
			Mage_RoleID = get_ctx_role_id();
			role:SetVariable(1009, 1009);
			role:GetScene():Say(role:GetObjID(), cltTvShow, "Congratulations, you became Number One Under Heaven and gained a twenty percent Final Damage bonus status.");
			local msg = "Congratulations "..role:GetName().."Become the best in the world and gain a twenty percent Final Damage bonus status.";
			role:Say(cctServer, cltLighting, msg);
			return brvSuccess;	
		end
	elseif cls == clsTaoist then
		local role_power = role:GetPow();
		if Taoist_RoleID ==  get_ctx_role_id() then
			role:SetVariable(1009, 1009);
			role:GetScene():Say(role:GetObjID(), cltTvShow, "Congratulations, you became Number One Under Heaven and gained a twenty percent Final Damage bonus status.");
			local msg = "Congratulations "..role:GetName().."Become the best in the world and gain a twenty percent Final Damage bonus status.";
			role:Say(cctServer, cltLighting, msg);
			return brvSuccess;
		end
		if role_power > Taoist_Top then
			local Last_Top_role = CGameWorld:GetInstance():FindValidRole(Taoist_RoleID);
			if Last_Top_role ~= nil then
				Last_Top_role:SetVariable(1009, 0);
				Last_Top_role:GetScene():Say(Last_Top_role:GetObjID(), cltTvShow, "Your title of Number One Under Heaven was taken by someone else. Raise your power and take it back.");
			end
			Taoist_Top = role_power;
			Taoist_RoleID = get_ctx_role_id();
			role:SetVariable(1009, 1009);
			role:GetScene():Say(role:GetObjID(), cltTvShow, "Congratulations, you became Number One Under Heaven and gained a twenty percent Final Damage bonus status.");
			local msg = "Congratulations "..role:GetName().."Become the best in the world and gain a twenty percent Final Damage bonus status.";
			role:Say(cctServer, cltLighting, msg);
			return brvSuccess;
		end
	else
		return brvFail;
	end
	return brvFail;
end

Include("script/newai/6179/main.uh")