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


function Option_one(owner)--shiguwucun 
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local yuanbao = role:GetItemBag():GetMoney(3);
	local jinbi = role:GetItemBag():GetMoney(1);
	local zuma = role:GetItemBag():GetItemCount(2800065,false);
	local bigu = role:GetItemBag():GetItemCount(2800215,false);
	local suipian = role:GetItemBag():GetItemCount(2800216,false);
	local yinji = role:GetItemBag():GetItemCount(2800217,false);
	if yuanbao < 40000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif jinbi < 5000000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Gold.");
		return brvFail;
	elseif zuma < 5 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Zuma Essence (need 5).");
		return brvFail;
	elseif  bigu < 30 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Fewer than 30 Skeleton Arm Bones.");
		return brvFail;
	elseif suipian < 100 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Fewer than 100 Skeleton Fragments.");
		return brvFail;		
	elseif yinji < 30 then 	
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Evil Spirit Marks (need 30).");
		return brvFail;		
	end
	role:GetItemBag():ChangeYuanbao(username,40000,false,599900025,1);
	role:GetItemBag():DeleteItemId(2800065,5,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(2800215,30,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(2800216,100,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(2800217,30,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():SubMoney(1,5000000,MONEYLOG_PACKAGE,0,0);
	role:GetItemBag():AddItem(599900025, 1, ITEMLOG_SCRIPT, 0, false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Synthesis successful!");
	return brvSuccess;
	
end

function Option_two(owner)--shishanxuehai 
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local yuanbao = role:GetItemBag():GetMoney(3);
	local jinbi = role:GetItemBag():GetMoney(1);
	local zuma = role:GetItemBag():GetItemCount(2800065,false);
	--local haojiao = role:GetItemBag():GetItemCount(2800219,false);
	local suipian1 = role:GetItemBag():GetItemCount(2800220,false);
	local xuerou = role:GetItemBag():GetItemCount(2800221,false);
	if yuanbao < 60000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif jinbi < 5000000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Gold.");
		return brvFail;
	elseif zuma < 5 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Zuma Essence (need 5).");
		return brvFail;
	--elseif  haojiao < 1 then 
	--	role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Ox Demon Horns (need 1).");
	--	return brvFail;
	elseif suipian1 < 40 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Ox Demon Fragments (need 40).");
		return brvFail;		
	elseif xuerou < 150 then 	
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Ox Demon Flesh (need 150).");
		return brvFail;		
	end
	role:GetItemBag():ChangeYuanbao(username,60000,false,599900024,1);
	role:GetItemBag():DeleteItemId(2800065,5,ITEMLOG_SCRIPT,false,0);
	--role:GetItemBag():DeleteItemId(2800219,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(2800220,40,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(2800221,150,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():SubMoney(1,5000000,MONEYLOG_PACKAGE,0,0);
	role:GetItemBag():AddItem(599900024, 1, ITEMLOG_SCRIPT, 0, false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Synthesis successful!");
	return brvSuccess;
	
end

	
function Option_three(owner)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local yuanbao = role:GetItemBag():GetMoney(3);
	local jinbi = role:GetItemBag():GetMoney(1);
	local zuma = role:GetItemBag():GetItemCount(2800065,false);
	local niumo = role:GetItemBag():GetItemCount(2800222,false);
	local wuqi = role:GetItemBag():GetItemCount(599900025,false);
	if yuanbao < 50000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif jinbi < 10000000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Gold.");
		return brvFail;
	elseif zuma < 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Zuma Essence (need 10).");
		return brvFail;
	elseif  niumo < 2 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Ox Demon Antlers (need 2).");
		return brvFail;
	elseif  wuqi < 1 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "You do not have No Trace of Bones - Fragment in your bag.");
		return brvFail;
	end
	role:GetItemBag():ChangeYuanbao(username,50000,false,599900027,1);
	role:GetItemBag():DeleteItemId(2800065,10,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(2800222,2,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(599900025,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():SubMoney(1,10000000,MONEYLOG_PACKAGE,0,0);
	role:GetItemBag():AddItem(599900027, 1, ITEMLOG_SCRIPT, 0, false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
	
end

function Option_four(owner)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local yuanbao = role:GetItemBag():GetMoney(3);
	local jinbi = role:GetItemBag():GetMoney(1);
	local zuma = role:GetItemBag():GetItemCount(2800065,false);
	local huangquan = role:GetItemBag():GetItemCount(2800218,false);
	local wuqi = role:GetItemBag():GetItemCount(599900024,false);
	if yuanbao < 50000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif jinbi < 10000000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Gold.");
		return brvFail;
	elseif zuma < 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Zuma Essence (need 10).");
		return brvFail;
	elseif  huangquan < 2 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Fewer than 2 Yellow Spring Tears.");
		return brvFail;
	elseif  wuqi < 1 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "You do not have Sea of Corpses and Blood - Fragment in your bag.");
		return brvFail;
	end
	role:GetItemBag():ChangeYuanbao(username,50000,false,599900026,1);
	role:GetItemBag():DeleteItemId(2800065,10,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(2800218,2,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(599900025,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():SubMoney(1,10000000,MONEYLOG_PACKAGE,0,0);
	role:GetItemBag():AddItem(599900026, 1, ITEMLOG_SCRIPT, 0, false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
	
end


Include("script/newai/6326/main.uh");