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


function Option_one(title)--Redeem title 1
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local title = role:GetTitle():GetEquipedTitleID();
	local yuanbao = role:GetItemBag():GetMoney(3);
	local suipian = role:GetItemBag():GetItemCount(2800101,false);
	if yuanbao < 1000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Fewer than 1 Mending Fragment.");
		return brvFail;		
	end
	role:GetItemBag():ChangeYuanbao(username,1000,false,106,1);
	role:GetItemBag():DeleteItemId(2800101,1,ITEMLOG_SCRIPT,false,0);
	role:GetTitle():AddTitle(106);
	role:GetTitle():EquipTile(106);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
	
end

function Option_two(title)--Redeem title 2
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local title = role:GetTitle():GetEquipedTitleID();
	if title ~= 106 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equip the designated title before exchanging.");
		return brvFail;
	end
	local yuanbao = role:GetItemBag():GetMoney(3);
	local suipian = role:GetItemBag():GetItemCount(2800101,false);
	if yuanbao < 4000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian < 4 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Fewer than 4 Mending Fragments.");
		return brvFail;	
	end
	role:GetItemBag():ChangeYuanbao(username,4000,false,44,1);
	role:GetItemBag():DeleteItemId(2800101,4,ITEMLOG_SCRIPT,false,0);
	role:GetTitle():UnEquipTitle();
	role:GetTitle():DeleteTitle(106,0);
	role:GetTitle():AddTitle(44);
	role:GetTitle():EquipTile(44);	
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end

	
function Option_three(owner)--Redeem title 3
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local title = role:GetTitle():GetEquipedTitleID();
	if title ~= 44 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equip the designated title before exchanging.");
		return brvFail;
	end
	local yuanbao = role:GetItemBag():GetMoney(3);
	local suipian = role:GetItemBag():GetItemCount(2800101,false);
	if yuanbao < 8000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian < 8 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Fewer than 8 Mending Fragments.");
		return brvFail;	
	end
	role:GetItemBag():ChangeYuanbao(username,8000,false,45,1);
	role:GetItemBag():DeleteItemId(2800101,8,ITEMLOG_SCRIPT,false,0);
	role:GetTitle():DeleteTitle(44,0);
	role:GetTitle():AddTitle(45);
	role:GetTitle():EquipTile(45);	
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end
	
function Option_four(owner)--Redeem title 4
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local title = role:GetTitle():GetEquipedTitleID();
	if title ~= 45 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equip the designated title before exchanging.");
		return brvFail;
	end
	local yuanbao = role:GetItemBag():GetMoney(3);
	local suipian = role:GetItemBag():GetItemCount(2800101,false);
	if yuanbao < 16000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian < 16 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Fewer than 16 Mending Fragments.");
		return brvFail;	
	end
	role:GetItemBag():ChangeYuanbao(username,16000,false,46,1);
	role:GetItemBag():DeleteItemId(2800101,16,ITEMLOG_SCRIPT,false,0);
	role:GetTitle():DeleteTitle(45,0);
	role:GetTitle():AddTitle(46);
	role:GetTitle():EquipTile(46);	
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end

function Option_five(owner)--Redeem title 5
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local title = role:GetTitle():GetEquipedTitleID();
	if title ~= 46 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equip the designated title before exchanging.");
		return brvFail;
	end
	local yuanbao = role:GetItemBag():GetMoney(3);
	local suipian = role:GetItemBag():GetItemCount(2800101,false);
	if yuanbao < 32000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian < 32 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Fewer than 32 Mending Fragments.");
		return brvFail;	
	end
	role:GetItemBag():ChangeYuanbao(username,32000,false,47,1);
	role:GetItemBag():DeleteItemId(2800101,32,ITEMLOG_SCRIPT,false,0);
	role:GetTitle():DeleteTitle(46,0);
	role:GetTitle():AddTitle(47);
	role:GetTitle():EquipTile(47);	
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end


function Option_six(owner)--Redeem title 6
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,0);
	local title = role:GetTitle():GetEquipedTitleID();
	if title == 71 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "You already have this title.");
		return brvFail;
	end
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 5000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	end
	role:GetItemBag():ChangeYuanbao(username,5000,false,71,1);
	role:GetTitle():AddTitle(71);
	role:GetTitle():EquipTile(71);	
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end
Include("script/newai/6145_NPC/main.uh");