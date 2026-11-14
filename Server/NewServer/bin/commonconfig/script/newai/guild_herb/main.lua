LoadPackage("npc.pkg")
LoadPackage("scene.pkg")
LoadPackage("gs_guild.pkg")
LoadPackage("guild_mgr.pkg")
LoadPackage("gameworld.pkg")
LoadPackage("data_manager.pkg")
LoadPackage("drop_generator.pkg")
LoadPackage("ai.pkg")
LoadPackage("guild_def.pkg")

LoadPackage("character.pkg")
LoadPackage("scene_object.pkg")
LoadPackage("chat_def.pkg")

g_GuildMgr = CGuildMgr:GetInstance();
g_GameWorld = CGameWorld:GetInstance();

g_DropGenerator = CDropGenerator:GetInstance();


function building_say(owner)
	local processor = owner:GetAIProcessor();
	local npc = Cast(processor:GetOwner(), 'CNpc');

	--print(processor:GetVariable(2));
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

	local building = guild:GetBuildingDataByNpcID(npc_spawn_point.qwNameID);

	local building_template = CDataManager_GUILD_BUILDING_DATA_:GetInstance():GetData(building.nType * 65536 + building.nLevel);

	npc:SetTemplateData(building_template.dwDeadNpcTemplateID);

	return brvSuccess;
end


function BuildingDrop(owner)
	local processor = owner:GetAIProcessor();
	local npc = Cast(processor:GetOwner(), 'CNpc');

	local scene = npc:GetScene();
	local res_count = processor:GetVariable(1);

	print("drop herb count:"..res_count);

	if res_count > 1000 then
		-- Drop a large resource pack
		local times = 1000;

		while times < res_count do
			g_DropGenerator:GenerateDrop(scene:GetServerGroup(), 120001, 1, scene, npc:GetX(), npc:GetY(), npc:GetZ(), npc:GetObjID(), false);
			times = times + 1000;
		end
	else
		-- Drop a small pack
		local times = 100;

		while times < res_count do
			g_DropGenerator:GenerateDrop(scene:GetServerGroup(), 120101, 1, scene, npc:GetX(), npc:GetY(), npc:GetZ(), npc:GetObjID(), false);
			times = times + 100;
		end
	end

	return brvSuccess;
end


Include("script/include/btnod.lua")
Include("script/newai/sub_tree/sub_tree.lh");
Include("script/newai/guild_herb/main.uh")


