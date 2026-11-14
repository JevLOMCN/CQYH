LoadPackage("npc.pkg")
LoadPackage("scene.pkg")
LoadPackage("gs_guild.pkg")
LoadPackage("guild_mgr.pkg")
LoadPackage("gameworld.pkg")
LoadPackage("data_manager.pkg")
LoadPackage("drop_generator.pkg")
LoadPackage("character.pkg")
LoadPackage("chat_def.pkg")
g_GuildMgr = CGuildMgr:GetInstance();
g_GameWorld = CGameWorld:GetInstance();

g_DropGenerator = CDropGenerator:GetInstance();






function building_say(owner)
	local processor = owner:GetAIProcessor();
	local npc = Cast(processor:GetOwner(), 'CNpc');

	local scene = npc:GetScene();
	local x,y,z = ConvertCoordN2O( npc:GetX()/1024, npc:GetY()/1024, npc:GetZ()/1024 )
	local x1 = math.floor(x/100);
	local y1 = math.floor(y/100);

	npc:Say(cctScene, cltTvShow, npc:GetNpcData().szDisplayName.."("..x1..","..y1..") is in battle");
	return brvSuccess;
end



function BuildingDead(owner)
	local processor = owner:GetAIProcessor();
	local npc = Cast(processor:GetOwner(), 'CNpc');

	local scene = npc:GetScene();
	local guild = g_GuildMgr:FindGuild(scene:GetSceneID());
	local npc_spawn_point = npc:GetSpawnPoint();
	if npc_spawn_point == nil then
		return brvFail;
	end
	local building = guild:GetBuildingDataByNpcID(npc_spawn_point.qwNameID);
	if building == nil then
		return brvFail;
	end
	local building_template = CDataManager_GUILD_BUILDING_DATA_:GetInstance():GetData(building.nType * 65536 + building.nLevel);

	--scene:RemoveNpc(npc);
	--npc:SetLevel(building.nLevel);
	--scene:AddNpc(npc, soatSpawn);
	npc:SetTemplateData(building_template.dwDeadNpcTemplateID);

	return brvSuccess;
end


Include("script/include/btnod.lua")
Include("script/newai/sub_tree/sub_tree.lh");
Include("script/newai/guild_Tow/main.uh")


--function AfterBuildingDead(owner)
	--local processor = owner:GetAIProcessor();
	--local npc = Cast(processor:GetOwner(), 'CNpc');
--
	--local scene = npc:GetScene();
--
	--local guild = g_GuildMgr:FindGuild(scene:GetSceneID());
--
	---- Step 1: spawn the treasure chest doodad
	----local attack_user_num = scene:GetVariable(2);
	--local defend_user_num = scene:GetVariable(1);	-- This guild variable is calculated in the scene tree script
--
	--local pSpawnPoint = npc:GetSpawnPoint();
	--local gs_building_data = guild:GetBuildingDataByNpcID(pSpawnPoint.qwNameID);
	--local pBuildingData = CDataManager_GUILD_BUILDING_DATA_:GetInstance():GetData(gs_building_data.nType * 65536 + gs_building_data.nLevel);
	--local Max_guild_num = guild:GetMaxMemberCount();
--
	--local doodad_id = 0;
	--if defend_user_num < (Max_guild_num / 3) then
		--doodad_id = pBuildingData.dwFewMembersReward;		-- Less than one third
	--elseif defend_user_num < (Max_guild_num * 2 / 3) then
		--doodad_id = pBuildingData.dwSomeMembersReward;		-- More than one third and less than two thirds
	--else
		--doodad_id = pBuildingData.dwManyMembersReward;		-- More than two thirds
	--end
--
	--local dwRoleID = get_ctx_role_id();
	--local pRole = g_GameWorld:FindRole(dwRoleID);
	--scene:SpawnDoodad(doodad_id, pRole:GetX(), pRole:GetY(), pRole:GetZ(), pRole:GetFaceDirection());
--
--
	---- Step 2: speak
	--local war_data = g_GuildMgr:FindBattleByDefendGuildID(scene:GetSceneID());
--
	--pRole:Say(scene:GetSceneID(), pBuildingData.dwDefenderMsgID, cltLighting);
	--pRole:Say(war_data.dwIntruderID, pBuildingData.dwIntruderMsgID, cltLighting);
--
--end


--function GetGBuildingTemplateData(owner)
	--local processor = owner:GetAIProcessor();
	--if nil == processor then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local npc = Cast(processor:GetOwner(), 'CNpc');
	--if nil == npc then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local scene = npc:GetScene();
	--if nil == scene then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local guild = g_GuildMgr:FindGuild(scene:GetSceneID());
	--if guild == nil then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local p_spawn_point = npc:GetSpawnPoint();
	--if p_spawn_point == nil then
		--return 0;
	--end
--
	--local gs_building_data = guild:GetBuildingDataByNpcID(p_spawn_point:qwNameID);
	--if gs_building_data == nil then
		--return 0;
	--end
--
	--local pBuildingData = CDataManager_GUILD_BUILDING_DATA_:GetInstance():GetData(gs_building_data:nType * 65536 + gs_building_data:nLevel);
	--if nil == pBuildingData then
		--return 0;
	--end
--
	--return 1;
--end
--
--
--
--
--
--function GetDoodadTemplateID(owner)
	--local processor = owner:GetAIProcessor();
	--if nil == processor then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local npc = Cast(processor:GetOwner(), 'CNpc');
	--if nil == npc then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local scene = npc:GetScene();
	--if nil == scene then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	----local attack_user_num = scene:GetVariable(2);
	--local defend_user_num = scene:GetVariable(1);	-- This guild variable is calculated in the scene tree script
--
--
	--local guild = g_GuildMgr:FindGuild(scene:GetSceneID());
	--if guild == nil then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local pSpawnPoint = npc:GetSpawnPoint();
	--if pSpawnPoint == nil then
		--return 0;
	--end
--
	--local gs_building_data = guild:GetBuildingDataByNpcID(pSpawnPoint:qwNameID);
	--if gs_building_data == nil then
		--return 0;
	--end
--
	--local pBuildingData = CDataManager_GUILD_BUILDING_DATA_:GetInstance():GetData(gs_building_data:nType * 65536 + gs_building_data:nLevel);
	--if nil == pBuildingData then
		--return 0;
	--end
--
	--local Max_guild_num = guild:GetMaxMemberCount();
--
	--if defend_user_num < (Max_guild_num / 3) then
		--return pBuildingData:dwFewMembersReward;		-- Less than one third
	--elseif defend_user_num < (Max_guild_num * 2 / 3) then
		--return pBuildingData:dwSomeMembersReward;		-- More than one third and less than two thirds
	--else
		--return pBuildingData:dwManyMembersReward;		-- More than two thirds
	--end
--end
--
--
--function GetSelfGuildID(owner)
	--local processor = owner:GetAIProcessor();
	--if nil == processor then
		--return 0;
	--end
--
	--local npc = Cast(processor:GetOwner(), 'CNpc');
	--if nil == npc then
		--return 0;
	--end
--
	--local scene = npc:GetScene();
	--if nil == scene then
		--return 0;
	--end
--
	--return scene:GetSceneID();
--end
--
--function GetSelfMsgID(owner)
--
	--local processor = owner:GetAIProcessor();
	--if nil == processor then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local npc = Cast(processor:GetOwner(), 'CNpc');
	--if nil == npc then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local scene = npc:GetScene();
	--if nil == scene then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local guild = g_GuildMgr:FindGuild(scene:GetSceneID());
	--if guild == nil then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local pSpawnPoint = npc:GetSpawnPoint();
	--if pSpawnPoint == nil then
		--return 0;
	--end
--
	--local gs_building_data = guild:GetBuildingDataByNpcID(pSpawnPoint:qwNameID);
	--if gs_building_data == nil then
		--return 0;
	--end
--
	--local pBuildingData = CDataManager_GUILD_BUILDING_DATA_:GetInstance():GetData(gs_building_data:nType * 65536 + gs_building_data:nLevel);
	--if nil == pBuildingData then
		--return 0;
	--end
--
	--return pBuildingData:dwDefenderMsgID;
--end
--
--
--
--function GetIntruderGuildID(owner)
	--local processor = owner:GetAIProcessor();
	--if nil == processor then
		--return 0;
	--end
--
	--local npc = Cast(processor:GetOwner(), 'CNpc');
	--if nil == npc then
		--return 0;
	--end
--
	--local scene = npc:GetScene();
	--if nil == scene then
		--return 0;
	--end
--
	--local war_data = g_GuildMgr:FindBattleByDefendGuildID(scene:GetSceneID());
	--if war_data == nil then
		--return 0;
	--end
--
	--return war_data:dwIntruderID;
--end
--
--
--function GetIntruderMsgID(owner)
--
	--local processor = owner:GetAIProcessor();
	--if nil == processor then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local npc = Cast(processor:GetOwner(), 'CNpc');
	--if nil == npc then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local scene = npc:GetScene();
	--if nil == scene then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local guild = g_GuildMgr:FindGuild(scene:GetSceneID());
	--if guild == nil then
		--return 0;			-- Return 0 on failure; a normal common treasure chest can also be used
	--end
--
	--local pSpawnPoint = npc:GetSpawnPoint();
	--if pSpawnPoint == nil then
		--return 0;
	--end
--
	--local gs_building_data = guild:GetBuildingDataByNpcID(pSpawnPoint:qwNameID);
	--if gs_building_data == nil then
		--return 0;
	--end
--
	--local pBuildingData = CDataManager_GUILD_BUILDING_DATA_:GetInstance():GetData(gs_building_data:nType * 65536 + gs_building_data:nLevel);
	--if nil == pBuildingData then
		--return 0;
	--end
--
	--return pBuildingData:dwIntruderMsgID;
--end
--



