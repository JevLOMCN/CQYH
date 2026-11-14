LoadPackage("gameworld.pkg")
LoadPackage("role.pkg")
LoadPackage("scene.pkg")
LoadPackage("chat_def.pkg")
LoadPackage("item_bag.pkg")

Include("script/include/btnod.lua")

local g_oGameWorld = CGameWorld:GetInstance();

function send_kill_info(owner)
	local role =  g_oGameWorld:FindValidRole(get_ctx_role_id());
	local killer = g_oGameWorld:FindValidRole(role:GetLastCasterID());
	if role and killer then
		local message = killer:GetName().."successfully slew "..role:GetName()..", received Kill Voucher * 1";
		local ret = role:GetScene():Say(cctScene, cltTvShow, message);
		killer:GetItemBag():AddItem(2800170, 1, ITEMLOG_REAL_MONEY_SHOP, 0, true);
		if ret then
			return brvSuccess;
		end
	end

	return brvFail;
end

function changePKMode(owner)
	local role =  g_oGameWorld:FindValidRole(get_ctx_role_id());
	role:ChangePkMode(3);
end

Include("script/scene/MirAct18/scene_bt.lua")

