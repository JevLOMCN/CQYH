LoadPackage("chat_def.pkg")
LoadPackage("global_config.pkg")
LoadPackage("arena_mgr.pkg")
LoadPackage("ranklist.pkg")

Include("script/include/activity.lh")
Include("script/final_gm/ms.lua")
Include("script/activity/spawn_npc.lua")
Include("script/arena/main.lua");
Include("script/guild/main.lua");


-- Heavenly Fortune: double experience and drops
--[[
function enable_exp_drop_rate()
	SetExpRate(20000);
	SetDropRate(20000);

	local message = "The Heavenly Blessing event has started. Double EXP and double drops are enabled server-wide during the event";
	for i = 1 , 3 do
		say(cctServer, cltLighting, message);
		wait(60);
	end
end
function disable_exp_drop_rate()
	local message = "The Heavenly Blessing event is about to end";
	for i = 1 , 3 do
		say(cctServer, cltLighting, message);
		wait(60);
	end

	SetExpRate(10000);
	SetDropRate(10000);
end
]]--

--Bichon Arena
--[[
--Bichon Arena opens
function enable_bqlt()
	local msg = "[Long Fei] of Bichon Province has set up an Arena. The fierce contest begins at 20 o'clock. Warriors, hurry and sign up to show your skills!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
--Bichon Arena ends
function disable_bqlt()
	local msg = "This Arena contest ends at 20:40. Contestants can go to the Martial Arena in Bichon Province and find [Uncle Ji] to draw prizes!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
-- Bichon Arena rewards
function enable_bqltlj()
	local msg = "The portal to the Netherworld Sea is about to open. The envoy [Mysterious Mage] waits near the Mongchon docks for warriors to lend a hand!"
	for i =1 , 3 do
		say(cctServer, cltNormal, msg);
		wait(15)
	end
end
function disable_bqltlj()
	local msg = "The Arena prize draw ends at 24 o'clock. Warriors, do not miss your chance!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
]]--

-- Streamer brawl
--[[
function zhubo_luandouop()
	local msg = "The Streamer Brawl event starts at 20:00. Participating streamers, get ready in the Random Corridor and enter the event map via the teleporter A Gui."
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
function zhubo_luandoucl()
	local msg = "The Streamer Brawl event ends at 21:00. Warriors, prepare to exit."
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
]]--


--Big Gray Wolf
--[[
function enable_dhl()
	-- say(cctServer, cltLighting, message);
	local msg = "The Spot the Real Big Gray Wolf event officially opens at 19:00. The [Big Gray Wolf] will appear in Sabak. Hurry and find him!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end

function disable_dhl()
	-- say(cctServer, cltLighting, message);
	local msg = "The Spot the Real Big Gray Wolf event officially closes at 19:15. The [Big Gray Wolf] will stop appearing. Hurry and find him!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end

function spawn_dhl()
	-- say(cctServer, cltLighting, message);
	local msg = "The Spot the Real Big Gray Wolf event is in progress. The [Big Gray Wolf] has just dropped into Sabak. Hurry and find him!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
]]--


-- Experience Arena opens in 5 minutes
function enable_exp_arena()
	local msg = "The EXP Arena opens in 5 minutes. A [Mysterious Swordsman] in Mongchon invites you to join. Do not miss it!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
-- Experience Arena closes in 5 minutes
function disable_exp_arena()
	local msg = "The EXP Arena will close in 5 minutes. Warriors inside, please prepare to leave!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end

-- Royal Treasury opens in 5 minutes
function enable_treasure_arena()
	local msg = "The Royal Treasury opens in 5 minutes. Warriors can find [Storyteller Elder] in the Mongchon safe zone to sign up!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
-- Royal Treasury closes in 5 minutes
function disable_treasure_arena()
	local msg = "The Royal Treasury closes in 5 minutes. Warriors inside, get ready to leave!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
-- Greed Demon King appears in Royal Treasury
function spawn_treasure_boss()
	local msg = "The Greedy Demon King is about to appear in the Royal Treasury. Warriors, find the [Storyteller] in the Mongchon safe zone to go and slay it!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end

function guild_war()
	local msg = "The Guild War starts in ten minutes. Please go to the Guild Envoy in the Mongchon safe zone to enter the map!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end

function new_year()
	local msg = "The Ox Year event opens in ten minutes. Players, head to the Shabak map to join!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end

--Guild stronghold battle starts in 5 minutes
function enable_jdz()
	local msg = "Guild stronghold battle starts in 5 minutes. Warriors can fight over Woma Temple Floor Two, Death Valley Insect Nest, Stone Tomb Floor Four and Zuma Floor Four!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
--Guild stronghold battle ends in 5 minutes
function disable_jdz()
	local msg = "Guild stronghold battle ends in 5 minutes!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end


---------------------------------------------------------Boss spawn--------------------------------


-- Zhulong God/Hongmo Leader/Woma Leader 30 minutes
function spawn_cl_hm_wm()
	local msg = "[Chitin Dragon God] will appear in [Death Valley Floor Six] in 30 minutes, [Rainbow Demon Leader] in [Rainbow Demon Altar] in 30 minutes, and [Woma Leader] in [Woma Temple Floor Three]. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end
-- Zhulong God/Hongmo Leader/Woma Leader 10 minutes
function spawn_cl_hm_wm_2()
	local msg = "[Chitin Dragon God] will appear in [Death Valley Floor Six] in 10 minutes, [Rainbow Demon Leader] in [Rainbow Demon Altar] in 10 minutes, and [Woma Leader] in [Woma Temple Floor Three]. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end

-- Zuma Leader 30 minutes
function spawn_zm()
	local msg = "In 30 minutes, [Zuma Leader] will appear at [Zuma Leader's Lair]. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end
-- Zuma Leader 10 minutes
function spawn_zm_2()
	local msg = "In 10 minutes, [Zuma Leader] will appear at [Zuma Leader's Lair]. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end

-- Chiyue Demon 30 minutes
function spawn_cyem()
	local msg = "[Red Moon Demon] will appear in [Red Moon Demon Cave] in 30 minutes. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end
-- Chiyue Demon 10 minutes
function spawn_cyem_2()
	local msg = "[Red Moon Demon] will appear in [Red Moon Demon Cave] in 10 minutes. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end

-- Flame Demon 30 minutes
function spawn_lyem()
	local msg = "[Flame Demon] will appear in [Red Moon Demon Cave] in 30 minutes. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end
-- Flame Demon 10 minutes
function spawn_lyem_2()
	local msg = "[Flame Demon] will appear in [Red Moon Demon Cave] in 10 minutes. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end


-- Huangquan Leader/Bull Demon King 30 minutes
function spawn_hq_nm()
	local msg = "[Huangquan Leader] will appear in [Bone Demon Cave Floor Five] in 30 minutes, and [Minotaur King] in [Minotaur Temple Hall] in 30 minutes. Warriors, go and slay them!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end
-- Huangquan Leader/Bull Demon King 10 minutes
function spawn_hq_nm_2()
	local msg = "[Huangquan Leader] will appear in [Bone Demon Cave Floor Five] in 10 minutes, and [Minotaur King] in [Minotaur Temple Hall] in 10 minutes. Warriors, go and slay them!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(15)
	end
end

-- Demon Dragon Leader 30 minutes
function spawn_mljz()
	local msg = "[Demon Dragon Leader] will appear in [Demon Dragon Blood Domain] in 30 minutes. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end
-- Demon Dragon Leader 10 minutes
function spawn_mljz2()
	local msg = "[Demon Dragon Leader] will appear in [Demon Dragon Blood Domain] in 10 minutes. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end

-- Fortune Golden Pig 30 minutes
function spawn_zcjz()
	local msg = "In 30 minutes, [Lucky Golden Pig] will appear near [Mongchon Safe Zone]. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end
-- Fortune Golden Pig 10 minutes
function spawn_zcjz2()
	local msg = "In 10 minutes, [Lucky Golden Pig] will appear near [Mongchon Safe Zone]. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end
-- Fortune Golden Pig appears
function spawn_zcjz3()
	local msg = "[Lucky Golden Pig] has appeared near [Mongchon Safe Zone]. Warriors, prepare for the hunt!"
	for i =1 , 3 do
		say(cctServer, cltLighting, msg);
		wait(20)
	end
end



---------------------------------------------------------------------------------------------------



-----------------------------------------------------Spawn  Sabac Greed Demon King-----------------------------

--[[
function enable_sbk_tlmw()
	local msg = "The Greedy Demon King will invade Mafa at 22 o'clock. Warriors, go to the Sabak Palace gate (near 529, 461) and fight the Demon King!"
	for i =1 , 5 do
		say(cctServer, cltLighting, msg);
		wait(100)
	end

end


function spawn_sbk_tlmw()
	local msg = "The Greedy Demon King has appeared at the Sabak Palace gate (near 529, 461). Warriors of Mafa, go and slay it!"
	for i =1 , 5 do
		say(cctServer, cltLighting, msg);
		wait(30)
	end

end

function ChunJie_Boss()
	local message = "The 2020 Spring Festival event starts in ten minutes. Go to Mongchon City to find the Golden Pig carrying treasure. Kill it for generous rewards!";
	for i =1 , 5 do
		say(cctServer, cltLighting, message);
		wait(30);
 	end
end
]]--


------------------------------------------------------------------------------------------------------------
--Divine item drawing
function open_dice()
	--local message = "Divine Item Shop Draw Broadcast";
	--say(cctServer, cltLighting, message);
	-- dice_open();
end

function RankListAward()--Award and clear donation rankings
	SendRankListAward(1, rlActivity01, 1, 1, 8001);
	SendRankListAward(1, rlActivity01, 2, 2, 8002);
	SendRankListAward(1, rlActivity01, 3, 3, 8003);
	SendRankListAward(1, rlActivity01, 4, 10, 8004);
	SendRankListAward(1, rlActivity01, 10, 1000, 8005);

	g_RankListMgr:ClearRankList(1, rlActivity01);

end



SCENE_LIST = {
	-- { id = 3, open = '2015-05-25 15:54:00 86400 3650', close = '2015-05-25 15:59:00 86400 3650' },   -- Programmer Sanctuary
	
	{ id = 183, open = '2020-05-18 21:00:00 86400 3650', close = '2020-05-18 22:00:00 86400 3650' },    -- Experience Arena
	{ id = 184, open = '2020-05-18 20:00:00 86400 3650', close = '2020-05-18 21:00:00 86400 3650' },    -- Royal Treasury
	
	--{ id = 81, open = '2020-06-23 20:00:00 86400 3650', close = '2020-06-23 21:00:00 86400 3650' },   -- Hall of Fame
	--{ id = 81, open = '2020-06-23 21:05:00 86400 3650', close = '2020-06-23 22:00:00 86400 3650' },
};

function main()
	clear_timer();
	
	------------------------------- Code examples
	-- register_timer({ year = 2015, month = 6, day = 30, hour = 23, min = 55, sec = 0, interval = 86400, maxcount = 365 * 10 }, "xxxxx");
	-- register_timer({ year = 2015, month = 6, day = 31, hour = 0, min = 10, sec = 0, interval = 86400, maxcount = 365 * 10 }, "xxxxx");
	-------------------------------

	------------------------------- Arena
	register_timer({ year = 2020, month = 11, day = 2, hour = 20, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "enable_exp_arena");
	register_timer({ year = 2020, month = 11, day = 2, hour = 21, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "disable_exp_arena");

	register_timer({ year = 2020, month = 11, day = 4, hour = 20, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "enable_exp_arena");
	register_timer({ year = 2020, month = 11, day = 4, hour = 21, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "disable_exp_arena");

	register_timer({ year = 2020, month = 11, day = 6, hour = 20, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "enable_exp_arena");
	register_timer({ year = 2020, month = 11, day = 6, hour = 21, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "disable_exp_arena");
	
	------------------------------- Royal Treasury open / close
	-- register_timer({ year = 2020, month = 05, day = 18, hour = 19, min = 55, sec = 0, interval = 86400, maxcount = 365 * 10 }, "enable_treasure_arena");
	-- register_timer({ year = 2020, month = 05, day = 18, hour = 20, min = 55, sec = 0, interval = 86400, maxcount = 365 * 10 }, "disable_treasure_arena");
	
	------------------------------- Royal Treasury boss spawn
	-- register_timer({ year = 2020, month = 05, day = 18, hour = 20, min = 10, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_treasure_boss");
	-- register_timer({ year = 2020, month = 05, day = 18, hour = 20, min = 25, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_treasure_boss");
	-- register_timer({ year = 2020, month = 05, day = 18, hour = 20, min = 40, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_treasure_boss");
	
	------------------------------- Zhulong God/Hongmo Leader/Woma Leader (01:00 - 13:00 - 16:00 - 18:00 - 23:00)
	register_timer({ year = 2020, month = 6, day = 30, hour = 00, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm");
	register_timer({ year = 2020, month = 6, day = 30, hour = 00, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm_2");
	
	register_timer({ year = 2020, month = 6, day = 30, hour = 12, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm");
	register_timer({ year = 2020, month = 6, day = 30, hour = 12, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm_2");
	
	register_timer({ year = 2020, month = 6, day = 30, hour = 15, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm");
	register_timer({ year = 2020, month = 6, day = 30, hour = 15, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm_2");
	
	register_timer({ year = 2020, month = 6, day = 30, hour = 17, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm");
	register_timer({ year = 2020, month = 6, day = 30, hour = 17, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm_2");
	
	register_timer({ year = 2020, month = 6, day = 30, hour = 22, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm");
	register_timer({ year = 2020, month = 6, day = 30, hour = 22, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm_2");
	------------------------------- 

	------------------------------- Zuma Leader (14:30 - 17:30 - 19:30 - 00:30)
	register_timer({ year = 2020, month = 6, day = 30, hour = 14, min = 00, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zm");
	register_timer({ year = 2020, month = 6, day = 30, hour = 14, min = 20, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zm_2");
	
	register_timer({ year = 2020, month = 6, day = 30, hour = 17, min = 00, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zm");
	register_timer({ year = 2020, month = 6, day = 30, hour = 17, min = 20, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zm_2");
	
	register_timer({ year = 2020, month = 6, day = 30, hour = 19, min = 00, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zm");
	register_timer({ year = 2020, month = 6, day = 30, hour = 19, min = 20, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zm_2");
	
	register_timer({ year = 2020, month = 6, day = 30, hour = 00, min = 00, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zm");
	register_timer({ year = 2020, month = 6, day = 30, hour = 00, min = 20, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zm_2");
	------------------------------- 

	------------------------------- Chiyue Demon (20:00)
	register_timer({ year = 2020, month = 6, day = 30, hour = 19, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cyem");
	register_timer({ year = 2020, month = 6, day = 30, hour = 19, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cyem_2");
	------------------------------- 
	
	------------------------------- Flame Demon (20:00)
	-- register_timer({ year = 2020, month = 6, day = 30, hour = 20, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_lyem");
	-- register_timer({ year = 2020, month = 6, day = 30, hour = 20, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_lyem_2");
	------------------------------- 

	------------------------------- Huangquan Leader/Bull Demon King
	--register_timer({ year = 2020, month = 6, day = 30, hour = 21, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_hq_nm");
	--register_timer({ year = 2015, month = 6, day = 30, hour = 21, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_hq_nm_2");

	------------------------------- Demon Dragon Leader
	--register_timer({ year = 2020, month = 6, day = 30, hour = 10, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_mljz");
	--register_timer({ year = 2020, month = 6, day = 30, hour = 19, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_mljz");
	------------------------------- Demon Dragon Leader
	--register_timer({ year = 2015, month = 6, day = 30, hour = 10, min = 45, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_mljz2");
	--register_timer({ year = 2020, month = 6, day = 30, hour = 19, min = 45, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_mljz2");
	
	------------------------------- Fortune Golden Pig
	--register_timer({ year = 2020, month = 6, day = 30, hour = 22, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zcjz");
	--register_timer({ year = 2015, month = 6, day = 30, hour = 22, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zcjz2");
	--register_timer({ year = 2015, month = 6, day = 30, hour = 23, min = 00, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_zcjz3");

	------------------------------- Spawn Sabac Greed Demon King
	-- register_timer({ year = 2016, month = 6, day = 19, hour = 21, min = 50, sec = 0, interval = 86400, maxcount = 9  }, "enable_sbk_tlmw");
	-- register_timer({ year = 2016, month = 6, day = 19, hour = 22, min = 0, sec = 0, interval = 86400, maxcount = 9  }, "spawn_sbk_tlmw");


	-------------------------------------------- arena Experience Arena
	-- monday Monday
	register_timer({ year = 2016, month = 5, day = 9, hour = g_GWConfig.stArena.nArenaBeginHour, min = g_GWConfig.stArena.nArenaBeginMin, sec = 0, interval = 604800, maxcount = 365 * 10 }, "arena_begin");
	register_timer({ year = 2016, month = 5, day = 9, hour = g_GWConfig.stArena.nArenaEndHour, min = g_GWConfig.stArena.nArenaEndMin, sec = 0, interval = 604800, maxcount = 365 * 10}, "arena_end");

	register_timer({ year = 2016, month = 5, day = 9, hour = g_GWConfig.stArena.nArenaRewardBeginHour, min = g_GWConfig.stArena.nArenaRewardBeginMin, sec = 0, interval = 604800, maxcount = 365 * 10 }, "arena_reward_begin");
	register_timer({ year = 2016, month = 5, day = 10, hour = g_GWConfig.stArena.nArenaRewardEndHour, min = g_GWConfig.stArena.nArenaRewardEndMin, sec = 0, interval = 604800, maxcount = 365 * 10 }, "arena_reward_end");

	-- Tuesday Tuesday
	register_timer({ year = 2016, month = 5, day = 11, hour = g_GWConfig.stArena.nArenaBeginHour, min = g_GWConfig.stArena.nArenaBeginMin, sec = 0, interval = 604800, maxcount = 365 * 10 }, "arena_begin");
	register_timer({ year = 2016, month = 5, day = 11, hour = g_GWConfig.stArena.nArenaEndHour, min = g_GWConfig.stArena.nArenaEndMin, sec = 0, interval = 604800, maxcount = 365 * 10}, "arena_end");

	register_timer({ year = 2016, month = 5, day = 11, hour = g_GWConfig.stArena.nArenaRewardBeginHour, min = g_GWConfig.stArena.nArenaRewardBeginMin, sec = 0, interval = 604800, maxcount = 365 * 10 }, "arena_reward_begin");
	register_timer({ year = 2016, month = 5, day = 12, hour = g_GWConfig.stArena.nArenaRewardEndHour, min = g_GWConfig.stArena.nArenaRewardEndMin, sec = 0, interval = 604800, maxcount = 365 * 10 }, "arena_reward_end");

	-- Friday Friday
	register_timer({ year = 2016, month = 5, day = 13, hour = g_GWConfig.stArena.nArenaBeginHour, min = g_GWConfig.stArena.nArenaBeginMin, sec = 0, interval = 604800, maxcount = 365 * 10 }, "arena_begin");
	register_timer({ year = 2016, month = 5, day = 13, hour = g_GWConfig.stArena.nArenaEndHour, min = g_GWConfig.stArena.nArenaEndMin, sec = 0, interval = 604800, maxcount = 365 * 10}, "arena_end");

	register_timer({ year = 2016, month = 5, day = 13, hour = g_GWConfig.stArena.nArenaRewardBeginHour, min = g_GWConfig.stArena.nArenaRewardBeginMin, sec = 0, interval = 604800, maxcount = 365 * 10 }, "arena_reward_begin");
	register_timer({ year = 2016, month = 5, day = 14, hour = g_GWConfig.stArena.nArenaRewardEndHour, min = g_GWConfig.stArena.nArenaRewardEndMin, sec = 0, interval = 604800, maxcount = 365 * 10 }, "arena_reward_end");
	--------------------------------------------

	
	
	--[[
	-- Default code temporarily disabled; likely events from a previous server launch
	-- register_timer({ year = 2020, month = 7, day = 24, hour = 0, min = 0, sec = 0, interval = 600, maxcount = 365 * 144 }, "open_dice");
	-- register_timer({ year = 2020, month = 2, day = 12, hour = 12, min = 0, sec = 0, interval = 86400, maxcount = 365 * 10 }, "enable_exp_drop_rate");
	-- register_timer({ year = 2020, month = 2, day = 12, hour = 12, min = 57, sec = 0, interval = 86400, maxcount = 365 * 10 }, "disable_exp_drop_rate");
	-- register_timer({ year = 2020, month = 2, day = 12, hour = 19, min = 0, sec = 0, interval = 86400, maxcount = 365 * 10 }, "enable_exp_drop_rate");
	-- register_timer({ year = 2020, month = 2, day = 12, hour = 19, min = 57, sec = 0, interval = 86400, maxcount = 365 * 10 }, "disable_exp_drop_rate");
	
	-------Guild war start announcement
	--register_timer({ year = 2020, month = 2, day = 12, hour = 18, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "guild_war");
	-------New Year event
	--register_timer({ year = 2020, month = 2, day = 12, hour = 19, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "new_year");

	-------------------------Streamer brawl
	--register_timer({ year = 2020, month = 06, day = 23, hour = 19, min = 55, sec = 0, interval = 86400, maxcount = 365 * 10 }, "zhubo_luandouop");
	--register_timer({ year = 2020, month = 06, day = 23, hour = 20, min = 55, sec = 0, interval = 86400, maxcount = 365 * 10 }, "zhubo_luandoucl");

	--------------------------------------------Training Ground Arena
	-- register_timer({ year = 2020, month = 11, day = 2, hour = 19, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "enable_bqlt");
	-- register_timer({ year = 2020, month = 11, day = 2, hour = 20, min = 35, sec = 0, interval = 604800, maxcount = 365 * 10 }, "disable_bqlt");
	-- register_timer({ year = 2020, month = 11, day = 2, hour = 23, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "disable_bqltlj");

	-- register_timer({ year = 2020, month = 11, day = 4, hour = 19, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "enable_bqlt");
	-- register_timer({ year = 2020, month = 11, day = 4, hour = 20, min = 35, sec = 0, interval = 604800, maxcount = 365 * 10 }, "disable_bqlt");
	-- register_timer({ year = 2020, month = 11, day = 4, hour = 23, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "disable_bqltlj");


	-- register_timer({ year = 2020, month = 11, day = 6, hour = 19, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "enable_bqlt");
	-- register_timer({ year = 2020, month = 11, day = 6, hour = 20, min = 35, sec = 0, interval = 604800, maxcount = 365 * 10 }, "disable_bqlt");
	-- register_timer({ year = 2020, month = 11, day = 6, hour = 23, min = 55, sec = 0, interval = 604800, maxcount = 365 * 10 }, "disable_bqltlj");
	--------------------------------------------

	--Guild stronghold battle announcement
	--------------------------------------------
	-- register_timer({ year = 2020, month = 6, day = 11, hour = 13, min = 55, sec = 0, interval = 86400, maxcount = 365 * 10 }, "enable_jdz");
	-- register_timer({ year = 2020, month = 6, day = 11, hour = 14, min = 25, sec = 0, interval = 86400, maxcount = 365 * 10 }, "disable_jdz");
	--------------------------------------------
	
	--2020 Spring Festival bosses
	--------------------------------------------
	-- register_timer({ year = 2020, month = 1, day = 24, hour = 18, min = 50, sec = 0, interval = 86400, maxcount = 1 }, "ChunJie_Boss");
	-- register_timer({ year = 2020, month = 1, day = 25, hour = 18, min = 50, sec = 0, interval = 86400, maxcount = 1 }, "ChunJie_Boss");
	-- register_timer({ year = 2020, month = 1, day = 26, hour = 18, min = 50, sec = 0, interval = 86400, maxcount = 1 }, "ChunJie_Boss");
	--------------------------------------------

	--------------------------------------------Big Gray Wolf--------------------------------------------
	-- register_timer({ year = 2016, month = 6, day = 6, hour = 18, min = 55, sec = 0, interval = 86400, maxcount = 14  }, "enable_dhl");

	-- register_timer({ year = 2016, month = 6, day = 6, hour = 19, min = 0, sec = 0, interval = 86400, maxcount = 14  }, "spawn_dhl");
	-- register_timer({ year = 2016, month = 6, day = 6, hour = 19, min = 3, sec = 0, interval = 86400, maxcount = 14  }, "spawn_dhl");
	-- register_timer({ year = 2016, month = 6, day = 6, hour = 19, min = 6, sec = 0, interval = 86400, maxcount = 14  }, "spawn_dhl");
	-- register_timer({ year = 2016, month = 6, day = 6, hour = 19, min = 9, sec = 0, interval = 86400, maxcount = 14  }, "spawn_dhl");
	-- register_timer({ year = 2016, month = 6, day = 6, hour = 19, min = 12, sec = 0, interval = 86400, maxcount = 14  }, "spawn_dhl");
	-- register_timer({ year = 2016, month = 6, day = 6, hour = 19, min = 15, sec = 0, interval = 86400, maxcount = 14  }, "spawn_dhl");

	-- register_timer({ year = 2016, month = 6, day = 6, hour = 19, min = 15, sec = 0, interval = 86400, maxcount = 14 }, "disable_dhl");
	-----------------------------------------------------------------------------------------------------------------------------------------------

	-------------------Boss Spawn - Announce 30 minutes in advance
	register_timer({ year = 2015, month = 6, day = 30, hour = 0, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_wm");
	register_timer({ year = 2015, month = 6, day = 30, hour = 0, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm");

	register_timer({ year = 2015, month = 6, day = 30, hour = 12, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm");
	register_timer({ year = 2015, month = 6, day = 30, hour = 12, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm");

	register_timer({ year = 2015, month = 6, day = 30, hour = 14, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm");
	register_timer({ year = 2015, month = 6, day = 30, hour = 14, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm");

	register_timer({ year = 2015, month = 6, day = 30, hour = 16, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm");
	register_timer({ year = 2015, month = 6, day = 30, hour = 16, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm");

	register_timer({ year = 2015, month = 6, day = 30, hour = 18, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm");
	register_timer({ year = 2015, month = 6, day = 30, hour = 18, min = 30, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm");

	register_timer({ year = 2015, month = 6, day = 30, hour = 23, min = 0, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm");
	register_timer({ year = 2015, month = 6, day = 30, hour = 23, min = 0, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm");

	-------------------Boss Spawn - Announce 10 minutes in advance
	register_timer({ year = 2020, month = 6, day = 30, hour = 0, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_2");
	--register_timer({ year = 2020, month = 6, day = 30, hour = 0, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm_2");

	register_timer({ year = 2015, month = 6, day = 30, hour = 12, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_2");
	register_timer({ year = 2015, month = 6, day = 30, hour = 12, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm_2");

	register_timer({ year = 2015, month = 6, day = 30, hour = 14, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_2");
	register_timer({ year = 2015, month = 6, day = 30, hour = 15, min = 5, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm_2");

	register_timer({ year = 2015, month = 6, day = 30, hour = 16, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_2");
	register_timer({ year = 2015, month = 6, day = 30, hour = 16, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm_2");

	register_timer({ year = 2015, month = 6, day = 30, hour = 18, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_2");
	register_timer({ year = 2015, month = 6, day = 30, hour = 18, min = 50, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm_2");

	register_timer({ year = 2015, month = 6, day = 30, hour = 23, min = 20, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_cl_hm_2");
	register_timer({ year = 2015, month = 6, day = 30, hour = 23, min = 20, sec = 0, interval = 86400, maxcount = 365 * 10 }, "spawn_wm_zm_2");

	-------------------------------------------- guild
	-- register_timer({ year = 2016, month = 5, day = 9, hour = g_GuildConfig.nGuildWarReadyHour, min = g_GuildConfig.nGuildWarReadyMin, sec = 0, interval = 86400, maxcount = 365 * 10 }, "GuildWarReady");
	-- register_timer({ year = 2016, month = 5, day = 9, hour = g_GuildConfig.nGuildWarBeginHour, min = g_GuildConfig.nGuildWarBeginMin, sec = 0, interval = 86400, maxcount = 365 * 10}, "GuildWarBegin");

	-- register_timer({ year = 2016, month = 5, day = 9, hour = g_GuildConfig.nGuildWar10TipHour, min = g_GuildConfig.nGuildWar10TipMin, sec = 0, interval = 86400, maxcount = 365 * 10 }, "GuildWar10Tps");
	-- register_timer({ year = 2016, month = 5, day = 10, hour = g_GuildConfig.nGuildWarEndHour, min = g_GuildConfig.nGuildWarEndMin, sec = 0, interval = 86400, maxcount = 365 * 10 }, "GuildWarEnd");
	--------------------------------------------
	
	-------------------------------------------- function RankListAward()--Award and clear donation rankings
	--register_timer({ year = 2020, month = 10, day = 1, hour = 0, min = 0, sec = 0, interval = 86400, maxcount = 365 * 10 }, "RankListAward");
	]]--

	for k,v in ipairs(SPAWN_NPC) do
		if not v.closed then
			local func_name = "spawn_npc_"..k;
			local y,m,d,h,mm,s,i,c;
			
			if v.time == '0' then
				timer = os.time();
				tab = os.date("*t",timer)
				y = tab.year;        -- Year
				m = tab.month;       -- Month
				d = tab.day;         -- Day
				h = tab.hour;        -- Hour
				mm = tab.min + 1;    -- Minute (trigger minute = current minute + 1)
			    s = tab.sec;         -- Second
				i = 86400;           -- Lifetime, default 86400 seconds (1 day)
				c = 3650;            -- Maximum count, default 3650
			else
				y,m,d,h,mm,s,i,c = string.match(v.time, "(%d+)%-(%d+)%-(%d+)%s+(%d+):(%d+):(%d+)%s+(%d+)%s+(%d+)");
			end 
				_G[func_name] = function ()
				local npcid = v.npc[math.random(#(v.npc))];
				local pos = v.pos[math.random(#(v.pos))];
				spawn_npc(pos.scene, pos.x, pos.y, pos.z, pos.range, npcid, v.count, i - 60);
			end
			
			register_timer({ year = y, month = m, day = d, hour = h, min = mm, sec = s, interval = i, maxcount = c}, func_name);
		else
			print("spawn npc "..k.." is closed");
		end
	end

	for k,v in ipairs(SCENE_LIST) do
		if not v.closed then
			local func_name = "open_scene_"..k;
			_G[func_name] = function ()
				create_scene(MAX_SERVER_GROUP_COUNT, v.id);
			end
			local y,m,d,h,mm,s,i,c = string.match(v.open, "(%d+)%-(%d+)%-(%d+)%s+(%d+):(%d+):(%d+)%s+(%d+)%s+(%d+)");
			local tStartTime = { year = tonumber(y), month = tonumber(m), day = tonumber(d), hour = tonumber(h),
				min = tonumber(mm), sec = tonumber(s), interval = tonumber(i), maxcount = tonumber(c)};
			register_timer(tStartTime, func_name);

			func_name = "close_scene_"..k;
			_G[func_name] = function ()
				close_scene(MAX_SERVER_GROUP_COUNT, v.id, 1);
			end
			local y,m,d,h,mm,s,i,c = string.match(v.close, "(%d+)%-(%d+)%-(%d+)%s+(%d+):(%d+):(%d+)%s+(%d+)%s+(%d+)");
			local tEndTime = { year = tonumber(y), month = tonumber(m), day = tonumber(d), hour = tonumber(h),
				min = tonumber(mm), sec = tonumber(s), interval = tonumber(i), maxcount = tonumber(c)};
			register_timer(tEndTime, func_name);

			local start_time = os.time(tStartTime);
			local end_time = os.time(tEndTime);
			local current_time = os.time();

			while end_time < current_time and tStartTime.maxcount > 0 and tEndTime.maxcount > 0 do
				start_time = start_time + tStartTime.interval;
				tStartTime.maxcount = tStartTime.maxcount - 1;
				end_time = end_time + tEndTime.interval;
				tEndTime.maxcount = tEndTime.maxcount - 1;
			end

			if current_time >= start_time and (current_time + 600) < end_time then
				local t = os.date("*t", current_time + 600)
				t.maxcount = 1;
				t.interval = 1;
				register_timer(t, "open_scene_"..k);
			end
		else
			print("scene "..v.id.." is closed");
		end
	end
	
end

--[[
function OnMonthlyMainLoop(nServerGroup)--Arena ranking rewards
	SendRankListAward(nServerGroup, rlArena1v1, 1, 5, 7001);
	SendRankListAward(nServerGroup, rlArena1v1Warrior, 1, 3, 7001);
	SendRankListAward(nServerGroup, rlArena1v1Mage, 1, 3, 7001);
	SendRankListAward(nServerGroup, rlArena1v1Taoist, 1, 3, 7001);

	g_RankListMgr:ClearRankList(nServerGroup, rlArena1v1);
	g_RankListMgr:ClearRankList(nServerGroup, rlArena1v1Warrior);
	g_RankListMgr:ClearRankList(nServerGroup, rlArena1v1Mage);
	g_RankListMgr:ClearRankList(nServerGroup, rlArena1v1Taoist);
end
]]--

main();
