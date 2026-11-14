LoadPackage("common.pkg")
LoadPackage("ms_guild.pkg")
LoadPackage("ms_guild_mgr.pkg")
LoadPackage("chat_def.pkg")
LoadPackage("guild_def.pkg")
LoadPackage("global_config.pkg")
LoadPackage("ms_chat.pkg")
LoadPackage("data_manager.pkg")

Include("script/guild/guild_variables.lua")
Include("script/include/activity.lh")

g_MSGuildMgr = CMsGuildMgr:GetInstance();

function AddGuildResource(guild_name, res_type, num)
	local guild = g_MSGuildMgr:FindGuildByName(guild_name);
	if guild ~= nil then
		if res_type == GUILD_ATTR_MONEY then
			guild:AddMoney(num, GUILD_CHEST_GM, true);
		elseif res_type == GUILD_ATTR_WOOD then
			guild:AddWood(num);
		elseif res_type == GUILD_ATTR_HERB then
			guild:AddHerb(num);
		elseif res_type == GUILD_ATTR_CRYSTAL then
			guild:AddCrystal(num);
		elseif res_type == GUILD_ATTR_SILVER then
			guild:AddSilver(num);
		end
	end
end



function GuildWarEntrance(IntruderID, DefenderID)

	wait(300);

	local IntruderGuild = g_MSGuildMgr:FindGuildByID(IntruderID);
	if IntruderGuild == nil then
		return ;
	end

	local DefenderGuild = g_MSGuildMgr:FindGuildByID(DefenderID);
	if DefenderGuild == nil then
		return ;
	end

	IntruderGuild:LuaBroadcastMsgNotifyType(GUILD_NOTIFY_MATCH_WAR);

	DefenderGuild:LuaBroadcastMsgNotifyType(GUILD_NOTIFY_MATCH_WAR);

	return;
end



--[[function GuildEndHint(AttackGuildID, DefenceGuildID)
	local  pWarData;

	-- Wait time in seconds
	wait(30);

	pWarData = g_MSGuildMgr:FindBattle(AttackGuildID, DefenceGuildID);
	if pWarData.nStage ~= GUILD_WAR_STAGE_END then
		say(0, AttackGuildID, ccstLighting, "This Guild War ends in half an hour");
		say(0, DefenceGuildID, ccstLighting, "This Guild War ends in half an hour");
	end


	wait(1200);

	pWarData = g_MSGuildMgr:FindBattle(AttackGuildID, DefenceGuildID);
	if pWarData.nStage ~= GUILD_WAR_STAGE_END then
		say(0, AttackGuildID, ccstLighting, "This Guild War ends in 10 minutes");
		say(0, DefenceGuildID, ccstLighting, "This Guild War ends in 10 minutes");
	end


	wait(1710);

	pWarData = g_MSGuildMgr:FindBattle(AttackGuildID, DefenceGuildID);
	if pWarData.nStage ~= GUILD_WAR_STAGE_END then
		say(0, AttackGuildID, ccstLighting, "This Guild War ends in 1 minute");
		say(0, DefenceGuildID, ccstLighting, "This Guild War ends in 1 minute");
	end


end
]]--


-- Initialize guild buildings when the guild is created
function InitGuildBuilding(guild_id)
	local guild = g_MSGuildMgr:FindGuildByID(guild_id);
	if guild == nil then
		return errGuildCreateFailed;
	end

	print(g_GuildConfig.dwPalaceSceneTemplateID);

	guild:LuaAddBuilding(g_GuildConfig.szBMainNameID, GUILD_B_MAIN, 1, 0);
	-- Create accompanying buildings

	-- Other buildings
	guild:LuaAddBuilding(g_GuildConfig.szBBusinessNameID, GUILD_B_BUSINESS, 1, 0);
	guild:LuaAddBuilding(g_GuildConfig.szBDeityNameID, GUILD_B_DEITY, 1, 0);
	guild:LuaAddBuilding(g_GuildConfig.szBWonderNameID, GUILD_B_WONDER, 1, 0);

	return errSuccess;
end







-- Calculate resource deductions
function GuildChangeResource(npc_id, guild_id, res_type, change_type, count, reason, ctx_type)
	local guild = g_MSGuildMgr:FindGuildByID(guild_id);

	local total_count = 0;
	if res_type == GUILD_ATTR_MONEY then
		total_count = guild:GetVariable(GUILD_V_WAR_MONEY);
	elseif res_type == GUILD_ATTR_HERB then
		total_count = guild:GetVariable(GUILD_V_WAR_HERB);
	elseif res_type == GUILD_ATTR_WOOD then
		total_count = guild:GetVariable(GUILD_V_WAR_WOOD);
	elseif res_type == GUILD_ATTR_CRYSTAL then
		total_count = guild:GetVariable(GUILD_V_WAR_CRYSTAL);
	elseif res_type == GUILD_ATTR_SILVER then
		total_count = guild:GetVariable(GUILD_V_WAR_SILVER);
	end

	local is_add = false;
	local change_count = 0;
	if change_type == GUILD_RES_ADD_COUNT or change_type == GUILD_RES_DEC_COUNT then
		change_count = count;
		if change_type == GUILD_RES_ADD_COUNT then
			is_add = true;
		else
			is_add = false;
		end
	elseif change_type == GUILD_RES_ADD_PERCENT or change_type == GUILD_RES_DEC_PERCENT then
		change_count = count * total_count / 100;
		if change_type == GUILD_RES_ADD_PERCENT then
			is_add = true;
		else
			is_add = false;
		end
	end


	if res_type == GUILD_ATTR_MONEY then
		if is_add then
			guild:AddMoney(change_count, reason);
		else
			local actual_count = guild:GetMoney();
			if actual_count < change_count then
				return errGuildSubResFailed;
			end
			guild:SubMoney(change_count, reason);
		end
	elseif res_type == GUILD_ATTR_HERB then
		if is_add then
			guild:AddHerb(change_count);
		else
			local actual_count = guild:GetHerb();
			if actual_count < change_count then
				return errGuildSubResFailed;
			end
			guild:SubHerb(change_count);
		end
	elseif res_type == GUILD_ATTR_WOOD then
		if is_add then
			guild:AddWood(change_count);
		else
			local actual_count = guild:GetWood();
			if actual_count < change_count then
				return errGuildSubResFailed;
			end
			guild:SubWood(change_count);
		end
	elseif res_type == GUILD_ATTR_CRYSTAL then
		if is_add then
			guild:AddCrystal(change_count);
		else
			local actual_count = guild:GetCryStal();
			if actual_count < change_count then
				return errGuildSubResFailed;
			end
			guild:SubCrystal(change_count);
		end
	elseif res_type == GUILD_ATTR_SILVER then
		if is_add then
			guild:AddSilver(change_count);
		else
			local actual_count = guild:GetSilver();
			if actual_count < change_count then
				return errGuildSubResFailed;
			end
			guild:SubSilver(change_count);
		end
	end


	GuildSyncResChange(npc_id, guild_id, is_add, res_type, change_count, ctx_type);

	return errSuccess;
end




------------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------

-- Guild level decay
function WarResourceReduce(intruderGuildLevel, defenderGuildLevel)
	local reduce = intruderGuildLevel - defenderGuildLevel;
	if reduce > 0 then
		local r = math.min(reduce * 10, 50);
		return 100 - r;
	else
		return 100;
	end
end



function GuildWarReady()

	local begin_time = tonumber(os.time());
	local month = tonumber(os.date("%m", begin_time));
	local day = tonumber(os.date("%d", begin_time));

	for i= 0, MAX_SERVER_GROUP_COUNT,1 do
		local bValidGroup = IsValidGroup(i);
		if bValidGroup then
			local bOpen = get_server_var(i, esvtGuild, avGuildWarCanOpen);
			if bOpen == 1 then


				local pData = g_MSGuildMgr:FirstWarData(i);
				while pData ~= nil do
					local data_month = tonumber(os.date("%m", pData.dwBeginDayTime));
					local data_day = tonumber(os.date("%d", pData.dwBeginDayTime));
					if data_month == month and data_day == day then

						local poIntruder = g_MSGuildMgr:FindGuildByID(pData.dwIntruderID);
						local poDefender = g_MSGuildMgr:FindGuildByID(pData.dwDefenderID);

						if poIntruder ~= nil and poDefender ~= nil then
							CMSChatMgr:GetInstance():Say(0, poIntruder:GetID(), cltLighting, "Guild war starts in 10 minutes. Attacker: ["..poIntruder:GetName().."], Defender: ["..poDefender:GetName().."]", poIntruder:GetSvrGroup());
							CMSChatMgr:GetInstance():Say(0, poDefender:GetID(), cltLighting, "Guild war starts in 10 minutes. Attacker: ["..poIntruder:GetName().."], Defender: ["..poDefender:GetName().."]", poDefender:GetSvrGroup());

							print("guild war: fight! intruder "..poIntruder:GetID().." defender "..poDefender:GetID());
						end
					end

					pData = g_MSGuildMgr:NextWarData(i);
				end
			end
		end
	end
end



function GuildWarBegin()
	local begin_time = tonumber(os.time());
	local month = tonumber(os.date("%m", begin_time));
	local day = tonumber(os.date("%d", begin_time));

	for i= 0, MAX_SERVER_GROUP_COUNT,1 do
		local bValidGroup = IsValidGroup(i);
		if bValidGroup then
			local bOpen = get_server_var(i, esvtGuild, avGuildWarCanOpen);
			if bOpen == 1 then
				local pData = g_MSGuildMgr:FirstWarData(i);
				while pData ~= nil do
					local data_month = tonumber(os.date("%m", pData.dwBeginDayTime));
					local data_day = tonumber(os.date("%d", pData.dwBeginDayTime));
					if data_month == month and data_day == day then
						print("guild war: begin");
						local poIntruder = g_MSGuildMgr:FindGuildByID(pData.dwIntruderID);
						local poDefender = g_MSGuildMgr:FindGuildByID(pData.dwDefenderID);

						if poIntruder ~= nil and poDefender ~= nil then
							poIntruder:SetWarCharacter(GUILD_WAR_INTRUDER);
							poIntruder:SetWarTarget(poDefender:GetID());
							poIntruder:SetAttackTime(tonumber(os.time()) + g_GuildConfig.nGuildWarAttackCD);

							poDefender:SetWarCharacter(GUILD_WAR_DEFENDER);
							poDefender:SetWarTarget(poIntruder:GetID());
							poDefender:AddIntruder(poIntruder:GetID());
							poDefender:SetProtectionTime(tonumber(os.time()) + g_GuildConfig.nGuildWarProtectionTime);

							GuildSyncWarRelation(GUILD_WAR_STAGE_BEGIN, pData.dwIntruderID, pData.dwDefenderID);

							poIntruder:LuaBroadcastJoinWar(GUILD_WAR_INTRUDER, poDefender:GetID());
							poDefender:LuaBroadcastJoinWar(GUILD_WAR_DEFENDER, poIntruder:GetID());

							local nReducePercent = WarResourceReduce(poIntruder:GetLevel(), poDefender:GetLevel());

							poDefender:SetVariable(GUILD_V_WAR_MONEY, poDefender:GetMoney() * nReducePercent / 100 * g_GuildConfig.nGuildWarMoneyPlunderPercent / 10000);
							poDefender:SetVariable(GUILD_V_WAR_HERB, poDefender:GetHerb() * nReducePercent / 100 * g_GuildConfig.nGuildWarResPlunderPercent / 10000);
							poDefender:SetVariable(GUILD_V_WAR_WOOD, poDefender:GetWood() * nReducePercent / 100 * g_GuildConfig.nGuildWarResPlunderPercent / 10000);
							poDefender:SetVariable(GUILD_V_WAR_CRYSTAL, poDefender:GetCryStal() * nReducePercent / 100 * g_GuildConfig.nGuildWarResPlunderPercent / 10000);
							poDefender:SetVariable(GUILD_V_WAR_SILVER, poDefender:GetSilver() * nReducePercent / 100 * g_GuildConfig.nGuildWarResPlunderPercent / 10000);

							CMSChatMgr:GetInstance():Say(0, poIntruder:GetID(), cltLighting, "Guild war has started. Attacker: ["..poIntruder:GetName().."], Defender: ["..poDefender:GetName().."]", poIntruder:GetSvrGroup());
							CMSChatMgr:GetInstance():Say(0, poDefender:GetID(), cltLighting, "Guild war has started. Attacker: ["..poIntruder:GetName().."], Defender: ["..poDefender:GetName().."]", poDefender:GetSvrGroup());
						end
					end

					pData = g_MSGuildMgr:NextWarData(i);
				end
			end
		end
	end
end


function GuildWar10Tps()
	local begin_time = tonumber(os.time());
	local month = tonumber(os.date("%m", begin_time));
	local day = tonumber(os.date("%d", begin_time));

	for i= 0, MAX_SERVER_GROUP_COUNT,1 do
		local bValidGroup = IsValidGroup(i);
		if bValidGroup then
			local bOpen = get_server_var(i, esvtGuild, avGuildWarCanOpen);
			if bOpen == 1 then
				local pData = g_MSGuildMgr:FirstWarData(i);
				while pData ~= nil do
					local data_month = tonumber(os.date("%m", pData.dwBeginDayTime));
					local data_day = tonumber(os.date("%d", pData.dwBeginDayTime));
					if data_month == month and data_day == day then
						print("guild war: last ten min");
						local poIntruder = g_MSGuildMgr:FindGuildByID(pData.dwIntruderID);
						local poDefender = g_MSGuildMgr:FindGuildByID(pData.dwDefenderID);

						if poIntruder ~= nil and poDefender ~= nil then
							CMSChatMgr:GetInstance():Say(0, poIntruder:GetID(), cltLighting, "Guild war ends in 10 minutes", poIntruder:GetSvrGroup());
							CMSChatMgr:GetInstance():Say(0, poDefender:GetID(), cltLighting, "Guild war ends in 10 minutes", poDefender:GetSvrGroup());
						end
					end

					pData = g_MSGuildMgr:NextWarData(i);
				end
			end
		end
	end
end

function GuildWarEnd()
	local begin_time = tonumber(os.time());
	local month = tonumber(os.date("%m", begin_time));
	local day = tonumber(os.date("%d", begin_time));

	for i= 0, MAX_SERVER_GROUP_COUNT,1 do
		local bValidGroup = IsValidGroup(i);
		if bValidGroup then
			local bOpen = get_server_var(i, esvtGuild, avGuildWarCanOpen);
			if bOpen == 1 then
				local defender_num = 0;
				local defender_table = {};
				local b = false;

				local pData = g_MSGuildMgr:FirstWarData(i);
				while pData ~= nil do
					local data_month = tonumber(os.date("%m", pData.dwBeginDayTime));
					local data_day = tonumber(os.date("%d", pData.dwBeginDayTime));
					if data_month == month and data_day == day then

						for i = 1, defender_num, 1 do
							if defender_table[i] == pData.dwDefenderID then
								b = true;
								break;
							end
						end

						if b == false then
							defender_num = defender_num + 1;
							defender_table[defender_num] = pData.dwDefenderID;
						end

						b = false;
					end

					pData = g_MSGuildMgr:NextWarData(i);
				end

				print("guild war: war num "..defender_num);

				for i = 1, defender_num, 1 do
					MSGuildWarEnd(defender_table[i], GUILD_WAR_DEFENDER_WIN);
					print("guild war: war end "..defender_table[i]);
				end

				g_MSGuildMgr:ClearWarData(i);
			end
		end
	end


end



-- Guild war settlement
function GuildWarScore(defenderID, warDefenderResult)

    local       nMoney = 0;
    local       nHerb = 0;
    local       nWood = 0;
    local       nCrystal = 0;
    local       nSilver = 0;

    local       nLostMoney = 0;
    local       nLostHerb = 0;
    local       nLostWood = 0;
    local       nLostCrystal = 0;
    local       nLostSilver = 0;

    local    	poIntruder = nil;
    local		poDefender = nil;

	print("test 03");

    poDefender = g_MSGuildMgr:FindGuildByID(defenderID);

    if (warDefenderResult == GUILD_WAR_DEFENDER_LOSE) then

        nLostMoney = poDefender:GetVariable(GUILD_V_WAR_MONEY) * g_GuildConfig.nGuildWarMoneyPlunder / 10000;
        nLostHerb = poDefender:GetVariable(GUILD_V_WAR_HERB) * g_GuildConfig.nGuildWarResPlunder / 10000;
        nLostWood = poDefender:GetVariable(GUILD_V_WAR_WOOD) * g_GuildConfig.nGuildWarResPlunder / 10000;
        nLostCrystal = poDefender:GetVariable(GUILD_V_WAR_CRYSTAL) * g_GuildConfig.nGuildWarResPlunder / 10000;
        nLostSilver = poDefender:GetVariable(GUILD_V_WAR_SILVER) * g_GuildConfig.nGuildWarResPlunder / 10000;

		if poDefender:GetIntruderNum() > 0 then
			nMoney = nLostMoney / poDefender:GetIntruderNum();
			nHerb = nLostHerb / poDefender:GetIntruderNum();
			nWood = nLostWood / poDefender:GetIntruderNum();
			nCrystal = nLostCrystal / poDefender:GetIntruderNum();
			nSilver = nLostSilver / poDefender:GetIntruderNum();
		end
    end


	if (warDefenderResult == GUILD_WAR_DEFENDER_LOSE) then

        poDefender:SubMoney(nLostMoney, GUILD_HE_TYPE_DEFEND_FAILED, false);
        poDefender:SubHerb(nLostHerb);
        poDefender:SubWood(nLostWood);
        poDefender:SubCrystal(nLostCrystal);
        poDefender:SubSilver(nLostSilver);

        -- send mail
        poDefender:BroadcastMail("Defenders lost", "Our guild failed the defense! Lost guild funds: "..nLostMoney..", Grain "..nLostHerb..", Wood "..nLostWood..", Stone "..nLostCrystal..", Iron Ore "..nLostSilver);

    elseif (warDefenderResult == GUILD_WAR_DEFENDER_WIN) then
        poDefender:BroadcastMail("Defenders won", "Our guild won the defense in this Guild War");
    end

	poDefender:ClearWarCharacter(GUILD_WAR_DEFENDER);



    local intruder_id = poDefender:FirstIntruder();
    while (intruder_id ~= 0) do
		poIntruder = g_MSGuildMgr:FindGuildByID(intruder_id);

		if (warDefenderResult == GUILD_WAR_DEFENDER_LOSE) then

			poIntruder:AddMoney(nMoney, GUILD_HE_TYPE_ATTACK_SUCC, false);
			poIntruder:AddHerb(nHerb);
			poIntruder:AddWood(nWood);
			poIntruder:AddCrystal(nCrystal);
			poIntruder:AddSilver(nSilver);

			poDefender:AddGuildEvent(GUILD_HE_TYPE_DEFEND_FAILED, intruder_id, 0, 0, 0);
			poIntruder:AddGuildEvent(GUILD_HE_TYPE_ATTACK_SUCC, poDefender:GetID(), 0, 0, 0);

			-- send mail
			poIntruder:BroadcastMail("Attackers won", "Our guild attacks "..poDefender:GetName().."Guild success! Obtained: Guild Funds "..nMoney..", Grain "..nHerb..", Wood "..nWood..", Stone "..nCrystal..", Iron Ore "..nSilver);
		elseif (warDefenderResult == GUILD_WAR_DEFENDER_WIN) then

			poIntruder:AddGuildEvent(GUILD_HE_TYPE_ATTACK_FAILED, poDefender:GetID(), 0, 0, 0);
			poDefender:AddGuildEvent(GUILD_HE_TYPE_DEFEND_SUCC, intruder_id, 0, 0, 0);

			-- send mail
			poIntruder:BroadcastMail("Attackers lost", "Our guild is attacking ["..poDefender:GetName().."] Guild failed to win");
		end

		poIntruder:ClearWarCharacter(GUILD_WAR_INTRUDER);

        intruder_id = poDefender:NextIntruder();
    end

    --snprintf(szTVContent, 256, "<#T:%d>", g_GuildConfig.dwGuildWarEndAttackDefeatToAllID);
    --nRetCode = CMSChatMgr::GetInstance().Say(0, poDefender->GetID(), cltTvShow, szTVContent, poIntruderGuild->GetSvrGroup());
    --LOG_PROCESS_ERROR(nRetCode);


	poDefender:ClearIntruder();


	--print("test 04");
    return;
end


function GuildWarFinish(defenderID, warDefenderResult)

    local    	poIntruder = nil;
    local		poDefender = nil;

	local currenttime = tonumber(os.time());
	local hour = tonumber(os.date("%H", currenttime));
	local minute = tonumber(os.date("%M", currenttime));
	local sec = tonumber(os.date("%S", currenttime));

	local cur_day_time = currenttime - (hour * 3600 + minute * 60 + sec);

	--print("current day time: "..cur_day_time);

    poDefender = g_MSGuildMgr:FindGuildByID(defenderID);
	poDefender:ClearWarCharacter(GUILD_WAR_DEFENDER);


    local intruder_id = poDefender:FirstIntruder();
    while (intruder_id ~= 0) do
		poIntruder = g_MSGuildMgr:FindGuildByID(intruder_id);
		poIntruder:LuaBroadcastJoinWar(GUILD_WAR_NONE, poDefender:GetID());

		poDefender:LuaBroadcastJoinWar(GUILD_WAR_NONE, poIntruder:GetID());

		if (warDefenderResult == GUILD_WAR_DEFENDER_LOSE) then

			poDefender:AddGuildEvent(GUILD_HE_TYPE_DEFEND_FAILED, intruder_id, 0, 0, 0);
			poIntruder:AddGuildEvent(GUILD_HE_TYPE_ATTACK_SUCC, poDefender:GetID(), 0, 0, 0);

			-- send mail
			poIntruder:AddWarScore(1);

			local mail = CDataManager_MAIL_TEMPLATE_DATA_:GetInstance():GetData(g_GuildConfig.nGuildWarAttackSuccMailID);
			poIntruder:BroadcastMail(mail.m_szMailSubject, string.format(mail.m_szMailContent, poDefender:GetName()));

			mail = CDataManager_MAIL_TEMPLATE_DATA_:GetInstance():GetData(g_GuildConfig.nGuildWarDefendFailMailID);
			poDefender:BroadcastMail(mail.m_szMailSubject, mail.m_szMailContent);

			print("guild war: Attack Success, intruder "..poIntruder:GetID().." defender "..poDefender:GetID());

			CMSChatMgr:GetInstance():Say(0, poIntruder:GetID(), cltLighting, "Guild war ended. Attacker: ["..poIntruder:GetName().."] wins", poIntruder:GetSvrGroup());
			CMSChatMgr:GetInstance():Say(0, poDefender:GetID(), cltLighting, "Guild war ended. Attacker: ["..poIntruder:GetName().."] wins", poDefender:GetSvrGroup());
		elseif (warDefenderResult == GUILD_WAR_DEFENDER_WIN) then

			poIntruder:AddGuildEvent(GUILD_HE_TYPE_ATTACK_FAILED, poDefender:GetID(), 0, 0, 0);
			poDefender:AddGuildEvent(GUILD_HE_TYPE_DEFEND_SUCC, intruder_id, 0, 0, 0);

			poDefender:AddWarScore(1);
			-- send mail

			local mail = CDataManager_MAIL_TEMPLATE_DATA_:GetInstance():GetData(g_GuildConfig.nGuildWarAttackFailMailID);
			poIntruder:BroadcastMail(mail.m_szMailSubject, string.format(mail.m_szMailContent, poDefender:GetName()));

			mail = CDataManager_MAIL_TEMPLATE_DATA_:GetInstance():GetData(g_GuildConfig.nGuildWarDefendSuccMailID);
			poDefender:BroadcastMail(mail.m_szMailSubject, mail.m_szMailContent);

			print("guild war: Attack Failed, intruder "..poIntruder:GetID().." defender "..poDefender:GetID());

			CMSChatMgr:GetInstance():Say(0, poIntruder:GetID(), cltLighting, "Guild war ended. Defender: ["..poDefender:GetName().."] wins", poIntruder:GetSvrGroup());
			CMSChatMgr:GetInstance():Say(0, poDefender:GetID(), cltLighting, "Guild war ended. Defender: ["..poDefender:GetName().."] wins", poDefender:GetSvrGroup());
		end

		GuildSyncWarRelation(GUILD_WAR_STAGE_END, poIntruder:GetID(), poDefender:GetID());

		poIntruder:ClearWarCharacter(GUILD_WAR_INTRUDER);

		g_MSGuildMgr:UpdateWarResult(poIntruder:GetSvrGroup(), poIntruder:GetID(), poDefender:GetID(), cur_day_time, warDefenderResult);

        intruder_id = poDefender:NextIntruder();
    end

	poDefender:ClearIntruder();

	--print("test 04");
    return;
end

