Include("script/stringtable/stringtable.lh")
LoadPackage("chat_def.pkg")
LoadPackage("gameworld.pkg")
LoadPackage("role.pkg")
LoadPackage("scene.pkg")
LoadPackage("npc.pkg")
LoadPackage("character.pkg")
LoadPackage("item_bag.pkg")
LoadPackage("data_manager.pkg")
LoadPackage("title.pkg")



local g_oGameWorld = CGameWorld:GetInstance();

function Check_Yuanbao(owner)
	-- Temporarily changed to deduct 10,000 gold
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	
	local moneyret = role:GetItemBag():GetMoney(1);
	if moneyret >= 10000 then
		role:GetItemBag():SubMoney(1, 10000, MONEYLOG_TELEPORT_PLATFORM, 0);
		return brvSuccess;
	else
		return brvFail;
	end
	
	
    -- Default code requires deducting ingots; temporarily disabled
	-- local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	-- local username = role:GetUserName();
	-- local yuanbao = role:GetItemBag():GetMoney(3);
	-- local title = role:GetTitle():FindTitle(40);
	-- local level = role:GetLevel();
	-- role:GetItemBag():ChangeYuanbao(username,0,false,ITEMLOG_SCRIPT,1);
	-- if title == true then
	-- 	if level < 20 then 
	-- 		role:GetScene():Say(role:GetObjID(), cltTvShow, "Level below 20, entry failed.");
	-- 		return brvFail ; 
	-- 	else	
	-- 		return brvSuccess ; 
	-- 	end	
	-- elseif yuanbao < 800 then 
	-- 	role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao, entry failed.");	
	-- 	return brvFail ; 
	-- elseif level < 20 then 
	-- 	role:GetScene():Say(role:GetObjID(), cltTvShow, "Level below 20, entry failed.");	
	-- 	return brvFail ; 
	-- else
	-- 	role:GetItemBag():ChangeYuanbao(username,800,false,ITEMLOG_SCRIPT,1);
	-- 	role:GetTitle():AddTitle(40,4);
	-- 	 return brvSuccess ; 
	-- end

end

Include("script/newai/sub_tree/sub_tree.lh")
Include("script/newai/6114_YouLingWai/main.uh")


