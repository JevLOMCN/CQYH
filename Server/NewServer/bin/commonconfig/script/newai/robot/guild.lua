LoadPackage("role.pkg")
LoadPackage("global_config.pkg")
LoadPackage("asimov_user.pkg")
LoadPackage("asimov_guild.pkg")
LoadPackage("guild_def.pkg")
LoadPackage("guild_mgr.pkg")
LoadPackage("general_mgr.pkg")
LoadPackage("asimov_role.pkg")
LoadPackage("scene.pkg")


function QueryGuildInfo(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());


	local num = role:GetAsimovRole():GetViewListNum();
	if num <= 0 then
		return ;
	end
	local index = math.random(0, num);
	local targetGuildID = role:GetAsimovRole():GetViewListGuildIDByIndex(targetGuildID);
	if targetGuildID == 0 then
		return ;
	end

	user:QueryGuildInfo(targetGuildID);

end

function CreateGuild(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	--print("create guild begin");
	if role:GetGuildID() ~= 0 then
		return ;
	end

	if role:GetLevel() < 13 then
		return ;
	end

	if role:GetObjID() % 9 ~= 0 then
		return;
	end

	local nRand = math.random(0,10000);
	if nRand > 3000 then
		return;
	end

	local ret = user:GMCommand("AddItem "..GUILD_CREATE_ITEM_ID.." "..GUILD_CREATE_ITEM_NUM.." ");
	ret = user:GMCommand("Addmoney 0 "..GUILD_CREATE_MONEY.. " ");
	if role:GetLevel() < GUILD_CREATER_LEVEL then
		ret = user:GMCommand("Level "..GUILD_CREATER_LEVEL.." ");
	end

	local	guild_name = "g"..role:GetName();
	local 	announce = "please join me, the guild create by "..role:GetName();

	user:CreateGuild(guild_name, announce);
end

function ViewGuildList(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local targetGuildID = 0;
	local num = role:GetAsimovRole():GetViewListNum();
	if num > 0 then
		targetGuildID = role:GetAsimovRole():GetViewListGuildIDByIndex(targetGuildID);
	end

	local ret = user:ViewGuildList(targetGuildID);
end

function ApplyToJoinGuild(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 ~= guildID then
		return;
	end

	local targetGuildID = 0;
	local num = role:GetAsimovRole():GetViewListNum();
	if num > 0 then
		local index = math.random(0, num-1);
		targetGuildID = role:GetAsimovRole():GetViewListGuildIDByIndex(index);
	else
		--ViewGuildList(owner);

		--local num = role:GetAsimovRole():GetViewListNum();
		--if num > 0 then
		--	targetGuildID = role:GetAsimovRole():GetViewListGuildIDByIndex(targetGuildID);
		--end
		return ;
	end

	if 0 == targetGuildID then
		return ;
	end

	local ret = role:GetAsimovRole():LuaIsInGuildAppList(targetGuildID);
	if ret then
		return ;
	end

	num = role:GetAsimovRole():LuaGetGuildAppNum();
	if num == ROLE_MAX_APPLICATION then
		return ;
	end

	ret = user:ApplyToJoinGuild(targetGuildID);

end

function GetApplyGuildAppList(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	-- no right, return
	if member.byPosition > GUILD_POSITION_DEACON then
		return ;
	end

	local  ret = user:GetGuildAppList();
end

function RefuseGuildApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	-- no right, return
	if member.byPosition > GUILD_POSITION_DEACON then
		return ;
	end

	--print("guildperformance RefuseGuildApp begin");

	local app_num = guild:GetAppNum();
	if app_num == 0 then
		return ;
	end

	--print("guildperformance RefuseGuildApp 3");

	local index = math.random(0, app_num - 1);
	local app_role_id = guild:GetRoleIDByIndexFromAppList(index);


	user:RefuseGuildApp(app_role_id);

	--print("guildperformance RefuseGuildApp end");
end


function AgreeGuildApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	-- no right, return
	if member.byPosition > GUILD_POSITION_DEACON then
		return ;
	end

	local app_num = guild:GetAppNum();
	if app_num == 0 then
		return ;
	end

	local index = math.random(0, app_num - 1);
	local app_role_id = guild:GetRoleIDByIndexFromAppList(index);

	user:AgreeGuildApp(app_role_id);
end

function InviteToJoinGuild(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	if guild == nil then
		return ;
	end
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	if member.byPosition > GUILD_POSITION_DEACON then
		return ;
	end

	local ret = CGeneralMgr:GetInstance():IsValidRandRoleExist();
	if ret == false then
		return;
	end

	local solo_role_data = CGeneralMgr:GetInstance():GetRandRole(role:GetObjID());

	user:InviteToJoinGuild(solo_role_data.szRoleName);
end

function RevertGuildInvitation(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 ~= guildID then
		return;
	end

	local invitation_role_id = role:GetAsimovRole():GetInviteGuildRoleID();
	if invitation_role_id == 0 then
		return ;
	end

	local ret = 0;
	local 	rand_num = math.random(0, 10);
	if rand_num > 8 then
		ret = user:RevertGuildInvitation(GUILD_REVERT_REFUSE, invitation_role_id);
	else
		ret = user:RevertGuildInvitation(GUILD_REVERT_AGREE, invitation_role_id);
	end
end


function Disband(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);

	if guild:GetLeaderID() ~= role:GetObjID() then
		return ;
	end

	if guild:GetLevel() >= g_GuildConfig.nGuildDisbandLevelLimit then
		return ;
	end

	if guild:GetAllianceNum() > 0 then
		return;
	end

	if guild:GetEnemyNum() > 0 then
		return ;
	end

	local rand_value = math.random(0, 100);
	if rand_value < 95 then
		return ;
	end

	user:DisbandGuild();
end

function ChangeNotice(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	if member.byPosition > GUILD_POSITION_BISHOP then
		return ;
	end


	local notice = "Holy cow, a simple test string"

	user:ChangeNotice(notice);

end


function ForbidChat(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end


	if member.byPosition > GUILD_POSITION_DEACON then
		return ;
	end

	local member_num = guild:GetMemberCount();
	local index = math.random(0, member_num - 1);

	local target_member = guild:GetMemberByIndex(index);
	if target_member.dwRoleID == role:GetObjID() then
		return ;
	end

	local nOP = math.random(GUILD_CHAT_FORBID, GUILD_CHAT_UNFORBID);

	user:ForbidChat(nOP, target_member.dwRoleID);
end


function ChangePosition(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	if member.byPosition > GUILD_POSITION_ELDER then
		return ;
	end

	local position = math.random(member.byPosition, GUILD_POSITION_CIVILIAN);
	if position == member.byPosition then
		return;
	end

	local bret = guild:IsOfficeFull(position);
	if bret == true then
		return ;
	end

	local member_num = guild:GetMemberCount();
	local index = math.random(0, member_num - 1);

	local target_member = guild:GetMemberByIndex(index);
	if target_member.dwRoleID == role:GetObjID() then
		return ;
	end
	if target_member.byPosition == position then
		return ;
	end

	if member.byPosition >= target_member.byPosition then
		return ;
	end

	user:ChangePosition(target_member.dwRoleID, position);
end

function LeaveGuild(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	if guild:GetLeaderID() == role:GetObjID() then
		return ;
	end

	user:LeaveGuild();

end

function BanishRole(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end
	if member.byPosition > GUILD_POSITION_SUBLEADER then
		return ;
	end

	local member_num = guild:GetMemberCount();
	local index = math.random(0, member_num - 1);
	local target_member = guild:GetMemberByIndex(index);

	if target_member.byPosition <= member.byPosition then
		return ;
	end

	user:BanishRole(target_member.dwRoleID);

end

function Demise(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	if guild:GetLeaderID() ~= role:GetObjID() then
		return ;
	end

	local member_num = guild:GetMemberCount();
	local index = math.random(0, member_num - 1);
	local target_member = guild:GetMemberByIndex(index);

	if target_member.dwRoleID == role:GetObjID() then
		return ;
	end

	user:Demise(target_member.dwRoleID);

end

function DrawSalary(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	if guild:GetLeaderID() ~= role:GetObjID() then
		return ;
	end

	user:DrawSalary();
end

function ContributeFund(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	if guild:GetLeaderID() ~= role:GetObjID() then
		return ;
	end

	local ret = user:GMCommand("AddMoney 1 10001");

	local nType = GUILD_ATTR_MONEY;
	local index = math.random(0, 10000);
	if index < 2000 then
		nType = GUILD_ATTR_MONEY;
	elseif index < 4000 then
		nType = GUILD_ATTR_HERB;
	elseif index < 6000 then
		nType = GUILD_ATTR_WOOD;
	elseif index < 8000 then
		nType = GUILD_ATTR_CRYSTAL;
	else
		nType = GUILD_ATTR_SILVER;
	end

	user:ContributeFund(nType, 10000);
end




function ChangeWeather(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	if guild:GetLeaderID() ~= role:GetObjID() then
		return ;
	end

	local ret = user:GMCommand("AddMoney 1 10001");

	local nWeatherType = math.random(swtNormal, swtEclipse);
	local nTimeType = math.random(GUILD_CW_TIME_5M, GUILD_CW_TIME_30M);

	user:ChangeWeather(nWeatherType, nTimeType);
end

function MoreGuildInfo(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if guildID ~= 0 then
		local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
		local ret = user:GetGuildMoreInfo();
	end
end


function FindGuild(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local targetGuildID = 0;
	local num = role:GetAsimovRole():GetViewListNum();
	if num > 0 then
		targetGuildID = role:GetAsimovRole():GetViewListGuildIDByIndex(targetGuildID);
	end

	local ret = user:FindGuild(targetGuildID);
end


function MoreHistory(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	if guild:GetLeaderID() ~= role:GetObjID() then
		return ;
	end

	user:MoreHistory();

end

function MoreChest(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	if guild:GetLeaderID() ~= role:GetObjID() then
		return ;
	end

	user:MoreChest();
end

function ChangeStorageRight(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	if member.byPosition >= GUILD_POSITION_BISHOP then
		return ;
	end

	local nPosition = math.random(GUILD_POSITION_LEADER, GUILD_POSITION_CIVILIAN);
	local right = 4095;

	user:ChangeStorageRight(nPosition, right);
end

function DiplomatApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	if member.byPosition >= GUILD_POSITION_SUBLEADER then
		return ;
	end

	local ret = user:GMCommand("AddMoney 1 50000");

	local nOP = math.random(GUILD_DIPLOMAT_ALLIANCE, GUILD_DIPLOMAT_ENEMY);
	local nTimeType = math.random(GUILD_DIPLOMAT_TIME_ONE, GUILD_DIPLOMAT_TIME_THIRTY);
	--local TargetGuildID = CGeneralMgr:GetInstance():GetRandomGuild();
	--if 0 == TargetGuildID or TargetGuildID == guildID then
	--	return ;
	--end

	--local TargetGuild  = CGuildMgr:GetInstance():FindGuild(TargetGuildID);

	user:DiplomatApp(nOP, nTimeType, CGeneralMgr:GetInstance():GetRandomGuild());
end

function GetApplyUnionList(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	if member.byPosition >= GUILD_POSITION_SUBLEADER then
		return ;
	end

	user:GetApplyUnionList();
end

function ReplyUnionApp(owner)
	local processor = owner:GetAIProcessor();
    local role = Cast(processor:GetOwner(), 'CRole');
	local user =  CUserMgr:GetInstance():FindUser(role:GetSessionID());

	local guildID = role:GetGuildID();
	if 0 == guildID then
		return;
	end

	local guild  = CGuildMgr:GetInstance():FindGuild(guildID);
	local member = guild:GetMember(role:GetObjID());
	if member == nil then
		return ;
	end

	if member.byPosition >= GUILD_POSITION_SUBLEADER then
		return ;
	end

	local targetGuildID = guild:RandomApplyUnion();
	if targetGuildID == 0 then
		return ;
	end

	local nOP = math.random(GUILD_REVERT_REFUSE, GUILD_REVERT_AGREE);

	user:ReplyUnionApp(nOP, targetGuildID);
end























