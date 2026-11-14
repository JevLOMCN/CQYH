Include("script/newai/sub_tree/sub_tree.lh")
LoadPackage("chat_def.pkg")
LoadPackage("global_config.pkg")
LoadPackage("data_manager.pkg")
LoadPackage("npc.pkg")	
LoadPackage("gameworld.pkg")
LoadPackage("role.pkg")
LoadPackage("character.pkg")
LoadPackage("item_bag.pkg")
LoadPackage("item.pkg")
Include("script/stringtable/stringtable.lh")
Include("script/newai/sub_tree/sub_tree.lh")
Include("script/include/btnod.lua")

local g_oGameWorld = CGameWorld:GetInstance();


-------------------- Separator

-- Create sockets
function AddSocket(slot)
	-- 11-helmet, 12-armor, 13-belt, 14-shoes, 15-shoulders, 16-bracers, 17-cloak
	local RoleObj = g_oGameWorld:FindValidRole(get_ctx_role_id());
	
	-- Return value
	local nResult = RoleObj:GetItemBag():EquipAddSocket(slot);
	
	-- Check the return value
	if nResult == 1802 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Item does not exist");
		return brvFail;
	end
	if nResult == 1816 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Currently running a stall");
		return brvFail;
	end
	if nResult == 1817 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Currently trading");
		return brvFail;
	end
	if nResult == 1836 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Cannot operate while dead");
		return brvFail;
	end
	if nResult == 10001 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Equipment does not exist");
		return brvFail;
	end
	if nResult == 10002 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "This equipment cannot be socketed");
		return brvFail;
	end
	if nResult == 10003 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "This equipment cannot be socketed any further");
		return brvFail;
	end
	if nResult == 10004 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Unknown socketing position error");
		return brvFail;
	end
	if nResult == 10005 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Error getting socket material cost amount");
		return brvFail;
	end
	if nResult == 10006 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Not enough socketing materials");
		return brvFail;
	end
	if nResult == 20000 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Socketing successful");
		return brvFail;
	end
	
	return brvSuccess;
end

function AddSocket_TouKui()
	AddSocket(11);
	return brvSuccess;
end
function AddSocket_YiFu()
	AddSocket(12);
	return brvSuccess;
end
function AddSocket_YaoDai()
	AddSocket(13);
	return brvSuccess;
end
function AddSocket_XieZi()
	AddSocket(14);
	return brvSuccess;
end
function AddSocket_HuJian()
	AddSocket(15);
	return brvSuccess;
end
function AddSocket_HuWan()
	AddSocket(16);
	return brvSuccess;
end
function AddSocket_PiFeng()
	AddSocket(17);
	return brvSuccess;
end

-------------------- Separator


-- Color engraving
function SetSocketColor(slot, color)
	-- [Socket 1] => 21-helmet, 22-armor, 23-belt, 24-shoes, 25-shoulders, 26-bracers, 27-cloak
	-- [Socket 2] => 31-helmet, 32-armor, 33-belt, 34-shoes, 35-shoulders, 36-bracers, 37-cloak
	local RoleObj = g_oGameWorld:FindValidRole(get_ctx_role_id());
	
	-- Return value
	local nResult = RoleObj:GetItemBag():SetSocketColor(slot, color);

	-- Check the return value
	if nResult == 1802 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Item does not exist");
		return brvFail;
	end
	if nResult == 1816 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Currently running a stall");
		return brvFail;
	end
	if nResult == 1817 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Currently trading");
		return brvFail;
	end
	if nResult == 1836 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Cannot operate while dead");
		return brvFail;
	end
	
	--if nResult == 10001 then
	--	RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Equipment does not exist");
	--	return brvFail;
	--end
	--if nResult == 10002 then
	--	RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "This equipment cannot be socketed");
	--	return brvFail;
	--end
	--if nResult == 10003 then
	--	RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "This equipment cannot be socketed any further");
	--	return brvFail;
	--end
	--if nResult == 10004 then
	--	RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Unknown socketing position error");
	--	return brvFail;
	--end
	--if nResult == 10005 then
	--	RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Error getting socket material cost amount");
	--	return brvFail;
	--end
	--if nResult == 10006 then
	--	RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Not enough socketing materials");
	--	return brvFail;
	--end
	if nResult == 30000 then
		RoleObj:GetScene():Say(RoleObj:GetObjID(), cltTvShow, "Carving succeeded");
		return brvFail;
	end
	
	return brvSuccess;
end


-------------------- Separator


-- Socket 1: helmet
function SetSocketColorFirst_TouKui_Red()
	SetSocketColor(21, 1);
	return brvSuccess;
end
function SetSocketColorFirst_TouKui_Yellow()
	SetSocketColor(21, 2);
	return brvSuccess;
end
function SetSocketColorFirst_TouKui_Blue()
	SetSocketColor(21, 3);
	return brvSuccess;
end
function SetSocketColorFirst_TouKui_Green()
	SetSocketColor(21, 4);
	return brvSuccess;
end
function SetSocketColorFirst_TouKui_Purple()
	SetSocketColor(21, 5);
	return brvSuccess;
end

-- Socket 1: armor
function SetSocketColorFirst_YiFu_Red()
	SetSocketColor(22, 1);
	return brvSuccess;
end
function SetSocketColorFirst_YiFu_Yellow()
	SetSocketColor(22, 2);
	return brvSuccess;
end
function SetSocketColorFirst_YiFu_Blue()
	SetSocketColor(22, 3);
	return brvSuccess;
end
function SetSocketColorFirst_YiFu_Green()
	SetSocketColor(22, 4);
	return brvSuccess;
end
function SetSocketColorFirst_YiFu_Purple()
	SetSocketColor(22, 5);
	return brvSuccess;
end

-- Socket 1: belt
function SetSocketColorFirst_YaoDai_Red()
	SetSocketColor(23, 1);
	return brvSuccess;
end
function SetSocketColorFirst_YaoDai_Yellow()
	SetSocketColor(23, 2);
	return brvSuccess;
end
function SetSocketColorFirst_YaoDai_Blue()
	SetSocketColor(23, 3);
	return brvSuccess;
end
function SetSocketColorFirst_YaoDai_Green()
	SetSocketColor(23, 4);
	return brvSuccess;
end
function SetSocketColorFirst_YaoDai_Purple()
	SetSocketColor(23, 5);
	return brvSuccess;
end

-- Socket 1: shoes
function SetSocketColorFirst_XieZi_Red()
	SetSocketColor(24, 1);
	return brvSuccess;
end
function SetSocketColorFirst_XieZi_Yellow()
	SetSocketColor(24, 2);
	return brvSuccess;
end
function SetSocketColorFirst_XieZi_Blue()
	SetSocketColor(24, 3);
	return brvSuccess;
end
function SetSocketColorFirst_XieZi_Green()
	SetSocketColor(24, 4);
	return brvSuccess;
end
function SetSocketColorFirst_XieZi_Purple()
	SetSocketColor(24, 5);
	return brvSuccess;
end

-- Socket 1: shoulders
function SetSocketColorFirst_HuJian_Red()
	SetSocketColor(25, 1);
	return brvSuccess;
end
function SetSocketColorFirst_HuJian_Yellow()
	SetSocketColor(25, 2);
	return brvSuccess;
end
function SetSocketColorFirst_HuJian_Blue()
	SetSocketColor(25, 3);
	return brvSuccess;
end
function SetSocketColorFirst_HuJian_Green()
	SetSocketColor(25, 4);
	return brvSuccess;
end
function SetSocketColorFirst_HuJian_Purple()
	SetSocketColor(25, 5);
	return brvSuccess;
end

-- Socket 1: bracers
function SetSocketColorFirst_HuWan_Red()
	SetSocketColor(26, 1);
	return brvSuccess;
end
function SetSocketColorFirst_HuWan_Yellow()
	SetSocketColor(26, 2);
	return brvSuccess;
end
function SetSocketColorFirst_HuWan_Blue()
	SetSocketColor(26, 3);
	return brvSuccess;
end
function SetSocketColorFirst_HuWan_Green()
	SetSocketColor(26, 4);
	return brvSuccess;
end
function SetSocketColorFirst_HuWan_Purple()
	SetSocketColor(26, 5);
	return brvSuccess;
end

-- Socket 1: cloak
function SetSocketColorFirst_PiFeng_Red()
	SetSocketColor(27, 1);
	return brvSuccess;
end
function SetSocketColorFirst_PiFeng_Yellow()
	SetSocketColor(27, 2);
	return brvSuccess;
end
function SetSocketColorFirst_PiFeng_Blue()
	SetSocketColor(27, 3);
	return brvSuccess;
end
function SetSocketColorFirst_PiFeng_Green()
	SetSocketColor(27, 4);
	return brvSuccess;
end
function SetSocketColorFirst_PiFeng_Purple()
	SetSocketColor(27, 5);
	return brvSuccess;
end


-------------------- Separator


-- Socket 2: helmet
function SetSocketColorSecond_TouKui_Red()
	SetSocketColor(31, 1);
	return brvSuccess;
end
function SetSocketColorSecond_TouKui_Yellow()
	SetSocketColor(31, 2);
	return brvSuccess;
end
function SetSocketColorSecond_TouKui_Blue()
	SetSocketColor(31, 3);
	return brvSuccess;
end
function SetSocketColorSecond_TouKui_Green()
	SetSocketColor(31, 4);
	return brvSuccess;
end
function SetSocketColorSecond_TouKui_Purple()
	SetSocketColor(31, 5);
	return brvSuccess;
end

-- Socket 2: armor
function SetSocketColorSecond_YiFu_Red()
	SetSocketColor(32, 1);
	return brvSuccess;
end
function SetSocketColorSecond_YiFu_Yellow()
	SetSocketColor(32, 2);
	return brvSuccess;
end
function SetSocketColorSecond_YiFu_Blue()
	SetSocketColor(32, 3);
	return brvSuccess;
end
function SetSocketColorSecond_YiFu_Green()
	SetSocketColor(32, 4);
	return brvSuccess;
end
function SetSocketColorSecond_YiFu_Purple()
	SetSocketColor(32, 5);
	return brvSuccess;
end

-- Socket 2: belt
function SetSocketColorSecond_YaoDai_Red()
	SetSocketColor(33, 1);
	return brvSuccess;
end
function SetSocketColorSecond_YaoDai_Yellow()
	SetSocketColor(33, 2);
	return brvSuccess;
end
function SetSocketColorSecond_YaoDai_Blue()
	SetSocketColor(33, 3);
	return brvSuccess;
end
function SetSocketColorSecond_YaoDai_Green()
	SetSocketColor(33, 4);
	return brvSuccess;
end
function SetSocketColorSecond_YaoDai_Purple()
	SetSocketColor(33, 5);
	return brvSuccess;
end

-- Socket 2: shoes
function SetSocketColorSecond_XieZi_Red()
	SetSocketColor(34, 1);
	return brvSuccess;
end
function SetSocketColorSecond_XieZi_Yellow()
	SetSocketColor(34, 2);
	return brvSuccess;
end
function SetSocketColorSecond_XieZi_Blue()
	SetSocketColor(34, 3);
	return brvSuccess;
end
function SetSocketColorSecond_XieZi_Green()
	SetSocketColor(34, 4);
	return brvSuccess;
end
function SetSocketColorSecond_XieZi_Purple()
	SetSocketColor(34, 5);
	return brvSuccess;
end

-- Socket 2: shoulders
function SetSocketColorSecond_HuJian_Red()
	SetSocketColor(35, 1);
	return brvSuccess;
end
function SetSocketColorSecond_HuJian_Yellow()
	SetSocketColor(35, 2);
	return brvSuccess;
end
function SetSocketColorSecond_HuJian_Blue()
	SetSocketColor(35, 3);
	return brvSuccess;
end
function SetSocketColorSecond_HuJian_Green()
	SetSocketColor(35, 4);
	return brvSuccess;
end
function SetSocketColorSecond_HuJian_Purple()
	SetSocketColor(35, 5);
	return brvSuccess;
end

-- Socket 2: bracers
function SetSocketColorSecond_HuWan_Red()
	SetSocketColor(36, 1);
	return brvSuccess;
end
function SetSocketColorSecond_HuWan_Yellow()
	SetSocketColor(36, 2);
	return brvSuccess;
end
function SetSocketColorSecond_HuWan_Blue()
	SetSocketColor(36, 3);
	return brvSuccess;
end
function SetSocketColorSecond_HuWan_Green()
	SetSocketColor(36, 4);
	return brvSuccess;
end
function SetSocketColorSecond_HuWan_Purple()
	SetSocketColor(36, 5);
	return brvSuccess;
end

-- Socket 2: cloak
function SetSocketColorSecond_PiFeng_Red()
	SetSocketColor(37, 1);
	return brvSuccess;
end
function SetSocketColorSecond_PiFeng_Yellow()
	SetSocketColor(37, 2);
	return brvSuccess;
end
function SetSocketColorSecond_PiFeng_Blue()
	SetSocketColor(37, 3);
	return brvSuccess;
end
function SetSocketColorSecond_PiFeng_Green()
	SetSocketColor(37, 4);
	return brvSuccess;
end
function SetSocketColorSecond_PiFeng_Purple()
	SetSocketColor(37, 5);
	return brvSuccess;
end


-------------------- Separator

-- Query node
function QueryRoleRootCount()
	local roleObj = g_oGameWorld:FindValidRole(get_ctx_role_id());
	
	local msg_Warrior = string.format("Warrior node-[Normal-%d]-[Advanced-%d]", roleObj:GetRuneRootCount(rrrNormal,clsWarrior), roleObj:GetRuneRootCount(rrrAdvanced,clsWarrior));
	roleObj:GetScene():Say(roleObj:GetObjID(), cltDialog, msg_Warrior);
	
	local msg_Mage = string.format("Mage node-[Normal-%d]-[Advanced-%d]", roleObj:GetRuneRootCount(rrrNormal,clsMage), roleObj:GetRuneRootCount(rrrAdvanced,clsMage));
	roleObj:GetScene():Say(roleObj:GetObjID(), cltDialog, msg_Mage);
	
	local msg_Taoist = string.format("Taoist node-[Normal-%d]-[Advanced-%d]", roleObj:GetRuneRootCount(rrrNormal,clsTaoist), roleObj:GetRuneRootCount(rrrAdvanced,clsTaoist));
	roleObj:GetScene():Say(roleObj:GetObjID(), cltDialog, msg_Taoist);
	
	local msg_Luck = string.format("Blessing Oil node-[%d]", roleObj:GetLuckRootCount());
	roleObj:GetScene():Say(roleObj:GetObjID(), cltDialog, msg_Luck);
	
	return brvSuccess;
end


function testZS()
	print("function - testZS");
	return brvSuccess;
end
function testFS()
	print("function - testFS");
	return brvSuccess;
end
function testDS()
	print("function - testDS");
	return brvSuccess;
end


-------------------- Separator

-- Default code
--[[
-- Normal color engraving
function DiaoSe_PuTong(owner)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local yuanbao = role:GetItemBag():GetMoney(3);
	local woma = role:GetItemBag():GetItemCount(2800075,false);
	local item_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	
	local rea = false;
	if item_one == nil  then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place a carvable gem in the first bag slot.");
		return brvFail;
	end
	
	local id_one = item_one.m_iteminfo.m_nTemplateID;
	
	for i = 0,5 do --Check whether the gem can be color engraved
		if id_one == g_GWConfig.nBaoShi_PuTong[i]  then
			rea = true;
			break;
		else
			rea = false;
		end	
		print(i);
	end
	
	if rea == false then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place a carvable gem in the first bag slot.");
		return brvFail;
	elseif woma < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Woma Essence (need one).");
		return brvFail;
	elseif yuanbao < 100 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;	
	end
	
	role:GetItemBag():DeleteItemId(2800075,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,100,false,6192,1);
	role:GetItemBag():DeleteItemPos(CONSLOT_DEFAULT, 0, 1, ITEMLOG_SCRIPT, 0);
	role:GetItemBag():BaoShi_DiaoSe(4800504);
	
	return brvSuccess;
end

-- Special color engraving
function DiaoSe_TeShu(owner)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local yuanbao = role:GetItemBag():GetMoney(3);
	local woma = role:GetItemBag():GetItemCount(2800075,false);
	local item_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local rea = false;
	if item_one == nil  then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place a carvable gem in the first bag slot.");
		return brvFail;
	end
	
	local id_one = item_one.m_iteminfo.m_nTemplateID;
	
	for i = 0,8 do --Check whether the gem can be color engraved
		if id_one == g_GWConfig.nBaoShi_TeShu[i]  then
			rea = true;
			break;
		else
			rea = false;
		end	
	end
	
	if rea == false then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place a carvable gem in the first bag slot.");
		return brvFail;
	elseif woma < 5 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Woma Essence (need one).");
		return brvFail;
	elseif yuanbao < 1000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;	
	end
	role:GetItemBag():DeleteItemId(2800075,5,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,1000,false,id_one,1);
	role:GetItemBag():DeleteItemPos(CONSLOT_DEFAULT, 0, 1, ITEMLOG_SCRIPT, 0);
	role:GetItemBag():BaoShi_DiaoSe(4800505);
	
	return brvSuccess;
end
]]--


Include("script/newai/6192/main.uh")