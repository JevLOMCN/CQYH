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
local A = 0;
local B = 5;
local C = 1;
local D = 7;

function get_Total(owner)
	local total = A * 10 + B;
	return total;
end
function get_ABC(owner)
	local total = C * 100 + D;
	return total;
end
--	return brvSuccess;
--	return brvFail;

function Buy_One(owner)
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	local itemBag = role:GetItemBag();
	if A < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "The item you exchanged has no exchanges left. Please exchange another item.");
		return brvSuccess;
	end
	local count = itemBag:GetItemCount(2800357, false);--Points
	if count < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough points to exchange this item. Please check your points.");
		return brvSuccess;
	end
	itemBag:DeleteItemId(2800357, 20000, ITEMLOG_SCRIPT,false,0);--Remove points
	itemBag:AddItem(99900026, 1, ITEMLOG_SCRIPT, 0, false);--Add Dragon Slayer
	A = A - 1;
	local msg = "Congratulations "..role:GetName().."Player exchanged the best gift of the Ox Year: the Dragon Slayer Blade!"
	role:Say(cctServer, cltLighting, msg);
	return brvSuccess;
end

function Buy_Two(owner)
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	local itemBag = role:GetItemBag();
	if B < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "The item you exchanged has no exchanges left. Please exchange another item.");
		return brvSuccess;
	end
	local count = itemBag:GetItemCount(2800357, false);--Points
	if count < 5000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough points to exchange this item. Please check your points.");
		return brvSuccess;
	end
	itemBag:DeleteItemId(2800357, 5000, ITEMLOG_SCRIPT,false,0);--Remove points
	itemBag:AddItem(2800363, 1, ITEMLOG_SCRIPT, 0, false);--Add Zhanyue Swift Step Seal
	B = B - 1;
	local msg = "Congratulations "..role:GetName().."Player exchanged a Moonslash Dash Seal!"
	role:Say(cctServer, cltLighting, msg);
	return brvSuccess;
end

function Buy_Three(owner)
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	local itemBag = role:GetItemBag();
	if C < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "The item you exchanged has no exchanges left. Please exchange another item.");
		return brvSuccess;
	end
	local count = itemBag:GetItemCount(2800357, false);--Points
	if count < 2000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough points to exchange this item. Please check your points.");
		return brvSuccess;
	end
	itemBag:DeleteItemId(2800357, 2000, ITEMLOG_SCRIPT,false,0);--Remove points
	itemBag:AddItem(2800364, 1, ITEMLOG_SCRIPT, 0, false);--Add attribute transfer scroll
	C = C - 1;
	local msg = "Congratulations "..role:GetName().."Player exchanged a Weapon Attribute Transfer Scroll!"
	role:Say(cctServer, cltLighting, msg);
	return brvSuccess;
end

function Buy_Four(owner)
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	local itemBag = role:GetItemBag();
	if D < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "The item you exchanged has no exchanges left. Please exchange another item.");
		return brvSuccess;
	end
	local count = itemBag:GetItemCount(2800357, false);--Points
	if count < 2000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough points to exchange this item. Please check your points.");
		return brvSuccess;
	end
	itemBag:DeleteItemId(2800357, 2000, ITEMLOG_SCRIPT,false,0);--Remove points
	itemBag:AddItem(2800365, 1, ITEMLOG_SCRIPT, 0, false);--Add luck transfer scroll
	D = D - 1;
	local msg = "Congratulations "..role:GetName().."Player exchanged a Luck Transfer Scroll!"
	role:Say(cctServer, cltLighting, msg);
	return brvSuccess;
end

function Buy_Five(owner)
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	local itemBag = role:GetItemBag();

	local count = itemBag:GetItemCount(2800357, false);--Points
	if count < 20 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough points to exchange this item. Please check your points.");
		return brvSuccess;
	end
	itemBag:DeleteItemId(2800357, 20, ITEMLOG_SCRIPT,false,0);--Remove points
	itemBag:AddItem(2800138, 1, ITEMLOG_SCRIPT, 0, false);--Add healing medicine gift box

	return brvSuccess;
end

function Buy_Six(owner)
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	local itemBag = role:GetItemBag();

	local count = itemBag:GetItemCount(2800357, false);--Points
	if count < 30 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough points to exchange this item. Please check your points.");
		return brvSuccess;
	end
	itemBag:DeleteItemId(2800357, 30, ITEMLOG_SCRIPT,false,0);--Remove points
	itemBag:AddItem(1800015, 1, ITEMLOG_SCRIPT, 0, false);--Add Nether Sea 4 teleport scroll

	return brvSuccess;
end


Include("script/newai/6277/main.uh")
