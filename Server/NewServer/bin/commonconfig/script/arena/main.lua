
LoadPackage("global_config.pkg")
LoadPackage("arena_mgr.pkg")
LoadPackage("role_data.pkg")
LoadPackage("online_role_data.pkg")
LoadPackage("ms_chat.pkg")

function arena_begin()
	for i= 0, MAX_SERVER_GROUP_COUNT,1 do
		local bValidGroup = IsValidGroup(i);
		if bValidGroup then
			print("arena begin"..i);
			local bOpen = get_server_var(i, esvtArena, avArenaCanOpen);
			if bOpen == 1 then
				CArenaMgr:GetInstance():AddFund(i, g_GWConfig.stArena.nSystemBase);
				set_server_var(i, esvtArena, avArenaIsOpen, 1);
				set_server_var(i, esvtArena, avArenaOpenTime, os.time());

				set_server_var(i, esvtArena, avArenaJJCRoleNum, 0);
				set_server_var(i, esvtArena, avArenaTicketsCount, 0);
			end
		end
	end
end

function arena_end()
	for i = 0, MAX_SERVER_GROUP_COUNT,1 do
		local bValidGroup = IsValidGroup(i);
		if bValidGroup then
			local bOpen = get_server_var(i, esvtArena, avArenaIsOpen);
			if bOpen == 1 then
				set_server_var(i, esvtArena, avArenaIsOpen, 0);
				set_server_var(i, esvtArena, avArenaRewardCanOpen, 1);
				CArenaMgr:GetInstance():CalcReward(i);
			end
		end
	end
end

function arena_reward_begin()
	for i = 0, MAX_SERVER_GROUP_COUNT,1 do
		local bValidGroup = IsValidGroup(i);
		if bValidGroup then
			local canopen = get_server_var(i, esvtArena, avArenaRewardCanOpen);
			if canopen == 1 then
				set_server_var(i, esvtArena, avArenaRewardIsOpen, 1);
			end
		end
	end
end

function arena_reward_end()
	for i = 0, MAX_SERVER_GROUP_COUNT,1 do
		local bValidGroup = IsValidGroup(i);
		if bValidGroup then
			set_server_var(i, esvtArena, avArenaRewardIsOpen, 0);
			set_server_var(i, esvtArena, avArenaRewardCanOpen, 0);
		end
	end
end


function CheckArenaActivityStatus()

	local currenttime = tonumber(os.time());
	local week_day = tonumber(os.date("%w", currenttime));
	local hour = tonumber(os.date("%H", currenttime));
	local minute = tonumber(os.date("%M", currenttime));
	local sec = tonumber(os.date("%S", currenttime));

	local cur_sec = hour * 3600 + minute * 60 + sec;

	--print("arena: check arena status, time "..cur_sec.." hour "..hour.." minute "..minute.." sec "..sec);
	--print("arena: arena begin time "..g_GWConfig.stArena.nArenaBeginTime.." end time "..g_GWConfig.stArena.nArenaEndTime);

	if ((week_day == 1) or (week_day == 3) or (week_day == 5)) then
		if (cur_sec > g_GWConfig.stArena.nArenaBeginTime) and (cur_sec < g_GWConfig.stArena.nArenaEndTime) then
			-- arena time
			for i = 0, MAX_SERVER_GROUP_COUNT,1 do
				local bValidGroup = IsValidGroup(i);
				if bValidGroup then
					local canopen = get_server_var(i, esvtArena, avArenaCanOpen);
					if canopen == 1 then
						local opentime = get_server_var(i, esvtArena, avArenaOpenTime);
						if (currenttime - opentime) > (g_GWConfig.stArena.nArenaEndTime - g_GWConfig.stArena.nArenaBeginTime) then
							-- yesterday
							print("arena: arena begin");
							arena_begin();
						else
							-- today
							local bOpen = get_server_var(i, esvtArena, avArenaIsOpen);
							if bOpen == 0 then
								print("arena: arena begin");
								arena_begin();
							end
						end
					end
				end
			end
			return ;
		elseif (cur_sec > g_GWConfig.stArena.nArenaRewardBeginTime) and (cur_sec < g_GWConfig.stArena.nArenaRewardEndTime) then
			-- arena reward time
			for i = 0, MAX_SERVER_GROUP_COUNT,1 do
				local bValidGroup = IsValidGroup(i);
				if bValidGroup then
					local canOpen = get_server_var(i, esvtArena, avArenaCanOpen);
					if canOpen == 1 then

						local opentime = get_server_var(i, esvtArena, avArenaOpenTime);
						if (currenttime - opentime) > 43200 then
							-- yesterday
							-- arena not begin, nothing to do
						else
							-- today

							local canRewardOpen = get_server_var(i, esvtArena, avArenaRewardCanOpen);
							if canRewardOpen == 0 then
								local bopen = get_server_var(i, esvtArena, avArenaIsOpen);
								if bopen == 1 then
									print("arena: arena end, reward begin");
									arena_end();
									arena_reward_begin();
								else
									-- no possible, normally. may be gm open another activity
								end
							else
								local bRewardOpen = get_server_var(i, esvtArena, avArenaRewardIsOpen);
								if bRewardOpen == 0 then
									print("arena: reward begin");
									arena_reward_begin();
								end
							end
						end
					end
				end
			end

			return ;
		end
	end

	-- all close
	for i = 0, MAX_SERVER_GROUP_COUNT,1 do
		local bValidGroup = IsValidGroup(i);
		if bValidGroup then
			set_server_var(i, esvtArena, avArenaIsOpen, 0);
			set_server_var(i, esvtArena, avArenaRewardCanOpen, 0);
			set_server_var(i, esvtArena, avArenaRewardIsOpen, 0);
		end
	end

	return ;
end



function ArenaReward(groupid, rolename)

	local chat_msg = "Congratulations ["..rolename.."] drew the arena prize! Diligence pays off, so join the arena for more chances. Warriors, do not miss it!"

	CMSChatMgr:GetInstance():Say(0, cctGroup, cltNormal, chat_msg, groupid);

	return ;
end


