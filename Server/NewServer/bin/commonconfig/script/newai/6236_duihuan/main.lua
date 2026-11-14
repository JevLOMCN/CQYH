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



function Option_one1(owner)--Thunder armor (male)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113001,false);
	local suipian2 = role:GetItemBag():GetItemCount(113002,false);
	local suipian3 = role:GetItemBag():GetItemCount(113003,false);
	local suipian4 = role:GetItemBag():GetItemCount(113004,false);
	local suipian5 = role:GetItemBag():GetItemCount(113005,false);
	local suipian6 = role:GetItemBag():GetItemCount(113006,false);
	local suipian7 = role:GetItemBag():GetItemCount(113007,false);
	local suipian8 = role:GetItemBag():GetItemCount(113008,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 50000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian5 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian6 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian7 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian8 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;		
	end
	role:GetItemBag():DeleteItemId(113001,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113002,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113003,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113004,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113005,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113006,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113007,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113008,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,50000,false,99980713,1);
	role:GetItemBag():AddItem(99980713,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end

function Option_one2(owner)--Thunder armor (female)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113009,false);
	local suipian2 = role:GetItemBag():GetItemCount(113010,false);
	local suipian3 = role:GetItemBag():GetItemCount(113011,false);
	local suipian4 = role:GetItemBag():GetItemCount(113012,false);
	local suipian5 = role:GetItemBag():GetItemCount(113013,false);
	local suipian6 = role:GetItemBag():GetItemCount(113014,false);
	local suipian7 = role:GetItemBag():GetItemCount(113015,false);
	local suipian8 = role:GetItemBag():GetItemCount(113016,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 50000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian5 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian6 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian7 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian8 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;		
	end
	role:GetItemBag():DeleteItemId(113009,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113010,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113011,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113012,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113013,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113014,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113015,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113016,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,50000,false,99990713,1);
	role:GetItemBag():AddItem(99990713,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end

function Option_two1(owner)--Flame armor (male)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113017,false);
	local suipian2 = role:GetItemBag():GetItemCount(113018,false);
	local suipian3 = role:GetItemBag():GetItemCount(113019,false);
	local suipian4 = role:GetItemBag():GetItemCount(113020,false);
	local suipian5 = role:GetItemBag():GetItemCount(113021,false);
	local suipian6 = role:GetItemBag():GetItemCount(113022,false);
	local suipian7 = role:GetItemBag():GetItemCount(113023,false);
	local suipian8 = role:GetItemBag():GetItemCount(113024,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 50000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian5 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian6 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian7 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian8 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;		
	end
	role:GetItemBag():DeleteItemId(113017,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113018,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113019,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113020,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113021,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113022,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113023,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113024,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,50000,false,99980723,1);
	role:GetItemBag():AddItem(99980723,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_two2(owner)--Flame armor (female)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113025,false);
	local suipian2 = role:GetItemBag():GetItemCount(113026,false);
	local suipian3 = role:GetItemBag():GetItemCount(113027,false);
	local suipian4 = role:GetItemBag():GetItemCount(113028,false);
	local suipian5 = role:GetItemBag():GetItemCount(113029,false);
	local suipian6 = role:GetItemBag():GetItemCount(113030,false);
	local suipian7 = role:GetItemBag():GetItemCount(113031,false);
	local suipian8 = role:GetItemBag():GetItemCount(113032,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 50000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian5 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian6 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian7 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian8 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;		
	end
	role:GetItemBag():DeleteItemId(113025,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113026,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113027,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113028,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113029,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113030,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113031,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113032,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,50000,false,99990723,1);
	role:GetItemBag():AddItem(99990723,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	
function Option_three1(owner)--Radiance armor (male)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113033,false);
	local suipian2 = role:GetItemBag():GetItemCount(113034,false);
	local suipian3 = role:GetItemBag():GetItemCount(113035,false);
	local suipian4 = role:GetItemBag():GetItemCount(113036,false);
	local suipian5 = role:GetItemBag():GetItemCount(113037,false);
	local suipian6 = role:GetItemBag():GetItemCount(113038,false);
	local suipian7 = role:GetItemBag():GetItemCount(113039,false);
	local suipian8 = role:GetItemBag():GetItemCount(113040,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 50000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian5 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian6 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian7 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian8 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;		
	end
	role:GetItemBag():DeleteItemId(113033,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113034,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113035,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113036,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113037,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113038,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113039,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113040,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,50000,false,99980733,1);
	role:GetItemBag():AddItem(99980733,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_three2(owner)--Radiance armor (female)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113041,false);
	local suipian2 = role:GetItemBag():GetItemCount(113042,false);
	local suipian3 = role:GetItemBag():GetItemCount(113043,false);
	local suipian4 = role:GetItemBag():GetItemCount(113044,false);
	local suipian5 = role:GetItemBag():GetItemCount(113045,false);
	local suipian6 = role:GetItemBag():GetItemCount(113046,false);
	local suipian7 = role:GetItemBag():GetItemCount(113047,false);
	local suipian8 = role:GetItemBag():GetItemCount(113048,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 50000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian5 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian6 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian7 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian8 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;		
	end
	role:GetItemBag():DeleteItemId(113041,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113042,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113043,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113044,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113045,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113046,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113047,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113048,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,50000,false,99990733,1);
	role:GetItemBag():AddItem(99990733,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_one3(owner)--Thunder necklace
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113049,false);
	local suipian2 = role:GetItemBag():GetItemCount(113050,false);
	local suipian3 = role:GetItemBag():GetItemCount(113051,false);
	local suipian4 = role:GetItemBag():GetItemCount(113052,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113049,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113050,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113051,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113052,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99920032,1);
	role:GetItemBag():AddItem(99920032,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_two3(owner)--Flame necklace
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113053,false);
	local suipian2 = role:GetItemBag():GetItemCount(113054,false);
	local suipian3 = role:GetItemBag():GetItemCount(113055,false);
	local suipian4 = role:GetItemBag():GetItemCount(113056,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113053,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113054,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113055,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113056,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99920033,1);
	role:GetItemBag():AddItem(99920033,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_three3(owner)--Radiance necklace
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113057,false);
	local suipian2 = role:GetItemBag():GetItemCount(113058,false);
	local suipian3 = role:GetItemBag():GetItemCount(113059,false);
	local suipian4 = role:GetItemBag():GetItemCount(113060,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113057,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113058,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113059,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113060,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99920034,1);
	role:GetItemBag():AddItem(99920034,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_one4(owner)--Thunder ring
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113061,false);
	local suipian2 = role:GetItemBag():GetItemCount(113062,false);
	local suipian3 = role:GetItemBag():GetItemCount(113063,false);
	local suipian4 = role:GetItemBag():GetItemCount(113064,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113061,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113062,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113063,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113064,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99930038,1);
	role:GetItemBag():AddItem(99930038,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_two4(owner)--Flame ring
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113065,false);
	local suipian2 = role:GetItemBag():GetItemCount(113066,false);
	local suipian3 = role:GetItemBag():GetItemCount(113067,false);
	local suipian4 = role:GetItemBag():GetItemCount(113068,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113065,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113066,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113067,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113068,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99930039,1);
	role:GetItemBag():AddItem(99930039,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_three4(owner)--Radiance ring
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113069,false);
	local suipian2 = role:GetItemBag():GetItemCount(113070,false);
	local suipian3 = role:GetItemBag():GetItemCount(113071,false);
	local suipian4 = role:GetItemBag():GetItemCount(113072,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113069,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113070,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113071,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113072,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99930040,1);
	role:GetItemBag():AddItem(99930040,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_one5(owner)--Thunder bracers
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113073,false);
	local suipian2 = role:GetItemBag():GetItemCount(113074,false);
	local suipian3 = role:GetItemBag():GetItemCount(113075,false);
	local suipian4 = role:GetItemBag():GetItemCount(113076,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113073,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113074,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113075,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113076,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99940030,1);
	role:GetItemBag():AddItem(99940030,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_two5(owner)--Flame bracers
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113077,false);
	local suipian2 = role:GetItemBag():GetItemCount(113078,false);
	local suipian3 = role:GetItemBag():GetItemCount(113079,false);
	local suipian4 = role:GetItemBag():GetItemCount(113080,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113077,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113078,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113079,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113080,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99940031,1);
	role:GetItemBag():AddItem(99940031,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_three5(owner)--Radiance bracers
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113081,false);
	local suipian2 = role:GetItemBag():GetItemCount(113082,false);
	local suipian3 = role:GetItemBag():GetItemCount(113083,false);
	local suipian4 = role:GetItemBag():GetItemCount(113084,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113081,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113082,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113083,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113084,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99940032,1);
	role:GetItemBag():AddItem(99940032,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_one6(owner)--Thunder belt
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113085,false);
	local suipian2 = role:GetItemBag():GetItemCount(113086,false);
	local suipian3 = role:GetItemBag():GetItemCount(113087,false);
	local suipian4 = role:GetItemBag():GetItemCount(113088,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113085,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113086,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113087,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113088,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99990715,1);
	role:GetItemBag():AddItem(99990715,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_two6(owner)--Flame belt
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113089,false);
	local suipian2 = role:GetItemBag():GetItemCount(113090,false);
	local suipian3 = role:GetItemBag():GetItemCount(113091,false);
	local suipian4 = role:GetItemBag():GetItemCount(113092,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113089,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113090,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113091,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113092,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99990725,1);
	role:GetItemBag():AddItem(99990725,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_three6(owner)--Radiance belt
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113093,false);
	local suipian2 = role:GetItemBag():GetItemCount(113094,false);
	local suipian3 = role:GetItemBag():GetItemCount(113095,false);
	local suipian4 = role:GetItemBag():GetItemCount(113096,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113093,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113094,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113095,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113096,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99990735,1);
	role:GetItemBag():AddItem(99990735,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_one7(owner)--Thunder shoes
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113097,false);
	local suipian2 = role:GetItemBag():GetItemCount(113098,false);
	local suipian3 = role:GetItemBag():GetItemCount(113099,false);
	local suipian4 = role:GetItemBag():GetItemCount(113100,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113097,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113098,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113099,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113100,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99990717,1);
	role:GetItemBag():AddItem(99990717,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_two7(owner)--Flame shoes
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113101,false);
	local suipian2 = role:GetItemBag():GetItemCount(113102,false);
	local suipian3 = role:GetItemBag():GetItemCount(113103,false);
	local suipian4 = role:GetItemBag():GetItemCount(113104,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113101,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113102,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113103,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113104,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99990727,1);
	role:GetItemBag():AddItem(99990727,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	

function Option_three7(owner)--Radiance shoes
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local username = role:GetUserName();
	local suipian1 = role:GetItemBag():GetItemCount(113105,false);
	local suipian2 = role:GetItemBag():GetItemCount(113106,false);
	local suipian3 = role:GetItemBag():GetItemCount(113107,false);
	local suipian4 = role:GetItemBag():GetItemCount(113108,false);
	local yuanbao = role:GetItemBag():GetMoney(3);
	if yuanbao < 20000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough Yuanbao.");
		return brvFail;
	elseif suipian1 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian2 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian3 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	elseif suipian4 < 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Not enough fragments.");
		return brvFail;
	end
	role:GetItemBag():DeleteItemId(113105,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113106,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113107,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():DeleteItemId(113108,1,ITEMLOG_SCRIPT,false,0);
	role:GetItemBag():ChangeYuanbao(username,20000,false,99990737,1);
	role:GetItemBag():AddItem(99990737,1,ITEMLOG_SCRIPT,0,false);
	role:GetScene():Say(role:GetObjID(), cltTvShow, "Exchange successful!");
	return brvSuccess;
end	
Include("script/newai/6236_duihuan/main.uh");