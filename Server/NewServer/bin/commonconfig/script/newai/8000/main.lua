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
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local yuanbao = role:GetItemBag():GetMoney(3);
	local title = role:GetTitle():FindTitle(40);
	local level = role:GetLevel();
	role:GetItemBag():ChangeYuanbao(username,0,false,ITEMLOG_SCRIPT,1);
	if title == true then
		if level < 38 then
			role:GetScene():Say(role:GetObjID(), cltTvShow, "Level below 38, entry failed.");	
			return brvFail ;
		else
			return brvSuccess ; 
		end	
	elseif yuanbao < 400 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao, entry failed.");	
		return brvFail ; 
	elseif level < 38 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Level below 38, entry failed.");	
		return brvFail ; 
	else
		role:GetItemBag():ChangeYuanbao(username,400,false,ITEMLOG_SCRIPT,1);
		role:GetTitle():AddTitle(40,4);
		 return brvSuccess ; 
	end

end

Include("script/newai/sub_tree/sub_tree.lh")
Include("script/newai/8000/main.uh")


