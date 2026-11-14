
LoadPackage("common.pkg")
LoadPackage("guild_def.pkg")
LoadPackage("npc.pkg")
LoadPackage("scene.pkg")
LoadPackage("gs_guild.pkg")
LoadPackage("guild_mgr.pkg")
LoadPackage("gameworld.pkg")
LoadPackage("data_manager.pkg")

LoadPackage("character.pkg")
LoadPackage("scene_object.pkg")
LoadPackage("chat_def.pkg")
LoadPackage("guild_def.pkg")

g_GuildMgr = CGuildMgr:GetInstance();
g_GameWorld = CGameWorld:GetInstance();



function building_say(owner)
	local processor = owner:GetAIProcessor();
	local npc = Cast(processor:GetOwner(), 'CNpc');

	--print(processor:GetVariable(2));
	local scene = npc:GetScene();
	local x,y,z = ConvertCoordN2O( npc:GetX()/1024, npc:GetY()/1024, npc:GetZ()/1024 )
	local x1 = math.floor(x/100);
	local y1 = math.floor(y/100);

	npc:Say(cctScene, cltTvShow, npc:GetNpcData().szDisplayName.."("..x1..","..y1..") is under attack");
	return brvSuccess;
end


function BuildingDead(owner)
	local processor = owner:GetAIProcessor();
	local npc = Cast(processor:GetOwner(), 'CNpc');

	local scene = npc:GetScene();
	local guild = g_GuildMgr:FindGuild(scene:GetSceneID());

	local npc_spawn_point = npc:GetSpawnPoint();

	local building = guild:GetBuildingDataByNpcID(npc_spawn_point.qwNameID);
	      print( building.nLevel );

	local building_template = CDataManager_GUILD_BUILDING_DATA_:GetInstance():GetData(building.nType * 65536 + building.nLevel);

	npc:SetTemplateData(building_template.dwDeadNpcTemplateID);

	print("guild war: war end "..scene:GetSceneID());

	return brvSuccess;
end


Include("script/include/btnod.lua")
Include("script/newai/sub_tree/sub_tree.lh");
Include("script/newai/6872_GuildWonderAI/main.uh");

