Include("script/stringtable/stringtable.lh")
LoadPackage("chat_def.pkg")
Include("script/newai/sub_tree/sub_tree.lh")
LoadPackage("gameworld.pkg");
LoadPackage("role.pkg");
LoadPackage("item_bag.pkg");
LoadPackage("error_def.pkg");
LoadPackage("rank_list_def.pkg");
LoadPackage("character.pkg")
LoadPackage("npc.pkg")
LoadPackage("guild_def.pkg")
LoadPackage("gs_guild.pkg")
LoadPackage("guild_mgr.pkg")
LoadPackage("data_manager.pkg")
LoadPackage("ai.pkg")
LoadPackage("money_def.pkg")
LoadPackage("item.pkg");
LoadPackage("global_config.pkg")

g_oGameWorld = CGameWorld:GetInstance();

function get_frequency()
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	
	local guild = CGuildMgr:GetInstance():FindGuild(role:GetGuildID());
	local guild_level = guild:GetLevel();

	local enter_time = role:GetVariable(998);
	local old_day = math.modf((enter_time + 28800) / 86400);
	local new_day = math.modf(( os.time() + 28800) / 86400);
	if old_day < new_day then
		local enter_num = role:SetVariable(999 , 0);
		return guild_level;
	end
	local enter_num = role:GetVariable(999);
	return guild_level - enter_num;
end

function check_frequency(owner)
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	--print("check_frequency start");
	local guild = CGuildMgr:GetInstance():FindGuild(role:GetGuildID());
	local guild_level = guild:GetLevel();
	local enter_num = role:GetVariable(999);

	if guild_level > enter_num then
		--print("check_frequency end1");
		return brvSuccess;
	end
	--print("check_frequency end2");
	return brvFail;
end

function lottery(owner)
	local role = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	--print("lottery start");
	local totalweight = 1000000;
	local random_number = math.random(totalweight);
	--print("random_number is" .. random_number);
	if random_number < 300000 then
		role:GetItemBag():AddMoney(1, 20000, MONEYLOG_PACKAGE, 0,0);
	elseif random_number < 600000 then
		role:AddExp(100000, false,false);
	elseif random_number < 750000 then
		local cls = role:GetClass();
		if cls == clsWarrior then
			role:GetItemBag():AddItem(21001, 1, MONEYLOG_PACKAGE, 0, true);
		elseif cls == clsMage then
			role:GetItemBag():AddItem(21002, 1, MONEYLOG_PACKAGE, 0, true);
		elseif cls == clsTaoist then
			role:GetItemBag():AddItem(21003, 1, MONEYLOG_PACKAGE, 0, true);
		end
	elseif random_number < 900000 then
		role:GetItemBag():AddItem(80003, 1, MONEYLOG_PACKAGE, 0, true);--Blessing oil
	elseif random_number < 950000 then
		role:GetItemBag():AddItem(1800040, 1, MONEYLOG_PACKAGE, 0, true);--Special repair scroll
	elseif random_number < 975000 then
		role:GetItemBag():AddItem(2800341, 1, MONEYLOG_PACKAGE, 0, true);--Normal gem; create a gift pack that randomly awards one of all gems
	else
		role:GetItemBag():AddItem(2800340, 1, MONEYLOG_PACKAGE, 0, true);--Ice-splitting dog fragment; create a gift pack that randomly awards one fragment
	end
	--print("lottery end");
	return brvSuccess;
end

function DaKong(owner)
	local pRole = CGameWorld:GetInstance():FindValidRole(get_ctx_role_id());
	local bag = pRole:GetItemBag();

	local weapon = bag:GetItem(CONSLOT_DEFAULT, 0);
	local equip_data = CDataManager_EQUIP_DATA_:GetInstance():GetData(weapon.m_iteminfo.m_nTemplateID);
	if weapon == nil or equip_data == nil then
		pRole:GetScene():Say(pRole:GetObjID(), cltTvShow, "Place the equipment to socket in the first item slot before using this item.");
		return brvFail;
	end

	local itemid = weapon.m_iteminfo.m_nTemplateID;
	local bind = weapon.m_iteminfo.m_byBindType;
	local dur = weapon.m_iteminfo.m_nMaxDur;
	local socketcount = weapon.m_iteminfo.m_dwSocketCombination;

	if weapon.m_iteminfo.m_nSoulID[0] ~= 0 or  weapon.m_iteminfo.m_nSoulID[1] ~= 0 or  weapon.m_iteminfo.m_nSoulID[2] ~= 0 then
		pRole:GetScene():Say(pRole:GetObjID(), cltTvShow, "Remove the gems before using this item.");
		return errItemNoConsumeUse;
	end

	if socketcount < 10 then 
		pRole:GetScene():Say(pRole:GetObjID(), cltTvShow, "Must have two sockets to perform advanced socketing.");
		return brvFail;
	end

	if bind == 10 then
		pRole:GetScene():Say(pRole:GetObjID(), cltTvShow, "Bound equipment cannot be socketed.");
		return brvFail;
	end
	if dur - weapon.m_iteminfo.m_nCurDur > 1000 then 
		pRole:GetScene():Say(pRole:GetObjID(), cltTvShow, "Unrepaired equipment cannot be socketed.");
		return brvFail;	
	end

	local ret = bag:GenerateSocket(true,1);
	if ret == 1 then 
		pRole:GetScene():Say(pRole:GetObjID(), cltTvShow, "Socketing successful.");
	end
	return brvSuccess;
end
Include("script/newai/6904_Guild/main.uh")