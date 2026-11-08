LoadPackage("role.pkg");
LoadPackage("item_bag.pkg");
LoadPackage("char_def.pkg");
LoadPackage("spell_book.pkg");
LoadPackage("spell.pkg")
LoadPackage("quest_list.pkg")
LoadPackage("achievement_def.pkg")
LoadPackage("achievement_list.pkg")

function main(oRole)
	local class = oRole:GetClass();
	local IsInDevMode = false
	local sex = oRole:GetSex();


	--! Unlock basic spell slots
	oRole:UnlockSpellSlot(qssNormalAtk);
	oRole:UnlockSpellSlot(qssKey10); --Previously qssMouseL
	oRole:UnlockSpellSlot(qssKey11); --Previously qssMouseR
	oRole:UnlockSpellSlot(qssKey1);
	oRole:UnlockSpellSlot(qssKey2);
	oRole:UnlockSpellSlot(qssKey3);
	oRole:UnlockSpellSlot(qssKey4);
	oRole:UnlockSpellSlot(qssKey5);
	oRole:UnlockSpellSlot(qssKey6);
	oRole:UnlockSpellSlot(qssKey7);
	oRole:UnlockSpellSlot(qssKey8);
	oRole:UnlockSpellSlot(qssKey9);
	oRole:UnlockSpellSlot(qssKey0);
	oRole:UnlockSpellSlot(qssPassive1);
	oRole:UnlockSpellSlot(qssPassive2);
	oRole:UnlockSpellSlot(qssPassive3);

	--! New-player gift pack
	oRole:GetItemBag():AddItem(331, 1, ITEMLOG_CREATE_ROLE, 0, false);
	oRole:GetItemBag():AddItem(331, 1, ITEMLOG_CREATE_ROLE, 0, false);
	oRole:GetItemBag():AddItem(331, 1, ITEMLOG_CREATE_ROLE, 0, false);
	oRole:GetItemBag():AddItem(331, 1, ITEMLOG_CREATE_ROLE, 0, false);
	oRole:GetItemBag():AddItem(331, 1, ITEMLOG_CREATE_ROLE, 0, false);

	local b = os.time();
	oRole:SetVariable(1006, b);    -- This line is absent from the old configuration; its purpose is unknown
	
	--! Initial free quest-teleport count; this line was enabled in the old configuration
	--oRole:GetAchievementList():SetVariable(avdQuestFlytoZone, 3);
	
	--! Starting gold by class
	--oRole:GetItemBag():AddMoney(1, 10000, MONEYLOG_BIRTH, 0);

	--! Starting quests
	oRole:GetQuestList():Accept(1441);

	--! Initialize anti-cheat
	oRole:SetAntiCheatLevel(2);

	--! Utility spells for testing
	--oRole:LearnSpell(5, 0, true);
	--oRole:LearnSpell(6, 0, true);
	--oRole:LearnSpell(7, 0, true);
	--oRole:LearnSpell(8, 0, true);
	--oRole:LearnSpell(9, 0, true);


	--! For testing; remove later
	-- oRole:GetItemBag():AddMoney(1, 100000000, MONEYLOG_BIRTH, 0);    -- Gold: 10 million
	-- oRole:GetItemBag():AddMoney(3, 10000000, MONEYLOG_BIRTH, 0);    -- Ingots: 100,000
	

	--! Configure character
	if not IsInDevMode then

		-- Warrior configuration
		if class == clsWarrior then

			oRole:SetLevel(1);

			-- Warrior active skills
			oRole:LearnSpell(1030, 0, true);
			oRole:EquipSpell(qssNormalAtk, 1030);

			-- Left-click replacement skills
			--oRole:LearnSpell(1430, 0, true); -- Attack-and-kill effect
			--oRole:LearnSpell(1431, 0, true); -- Assassination effect
			---oRole:LearnSpell(1440, 0, true); -- Assassination effect 2
			--oRole:LearnSpell(1432, 0, true); -- Half-moon effect
			--oRole:LearnSpell(1433, 0, true); -- Blazing-fire effect
			--oRole:LearnSpell(1434, 0, true); -- Sun-chasing effect
			-- Character equipment
			if sex==1 then  --Male 1
				oRole:GetItemBag():EquipItemOnBirth(89980113, NES_BODY); -- Chest armor
			else		--Female 2
				oRole:GetItemBag():EquipItemOnBirth(89990113, NES_BODY); -- Chest armor
			end

		--	oRole:GetItemBag():EquipItemOnBirth(99800021, NES_MAIN_HAND); -- Two-handed heavy short weapon
		  --  oRole:GetItemBag():EquipItemOnBirth(10111111, NES_HEAD); -- Head
		  --  oRole:GetItemBag():EquipItemOnBirth(10111311, NES_BODY); -- Chest armor
		 --   oRole:GetItemBag():EquipItemOnBirth(10111411, NES_LEG); -- Legs
		 --   oRole:GetItemBag():EquipItemOnBirth(10111511, NES_WAIST); -- Waist
		 --   oRole:GetItemBag():EquipItemOnBirth(10111611, NES_ARM); -- Wrist
		 --   oRole:GetItemBag():EquipItemOnBirth(10111711, NES_FOOT); -- Shoes




			--Bag
			oRole:GetItemBag():AddItem(89900001, 1, ITEMLOG_CREATE_ROLE, 0, false);

		-- Mage configuration
		elseif class == clsMage then

			oRole:SetLevel(1);

			--Mage active skills
			oRole:LearnSpell(2530, 0, true);
			oRole:EquipSpell(qssNormalAtk, 2530);

			oRole:GetItemBag():AddItem(89900001, 1, ITEMLOG_CREATE_ROLE, 0, false);

			-- Character equipment
			if sex==1 then  --Male 1
				oRole:GetItemBag():EquipItemOnBirth(89980113, NES_BODY); -- Chest armor
			else		--Female 2
				oRole:GetItemBag():EquipItemOnBirth(89990113, NES_BODY); -- Chest armor
			end

		-- Taoist configuration
		elseif class == clsTaoist then

			oRole:SetLevel(1);

			-- Taoist active skills
			oRole:LearnSpell(3000, 0, true); -- Normal attack
			oRole:EquipSpell(qssNormalAtk, 3000);

			-- Character equipment
			if sex==1 then  --Male 1
				oRole:GetItemBag():EquipItemOnBirth(89980113, NES_BODY); -- Chest armor
			else		--Female 2
				oRole:GetItemBag():EquipItemOnBirth(89990113, NES_BODY); -- Chest armor
			end

			oRole:GetItemBag():AddItem(89900001, 1, ITEMLOG_CREATE_ROLE, 0, false);

			--oRole:GetItemBag():AddItem(40004, 1, ITEMLOG_CREATE_ROLE, 0, false);
			--oRole:GetItemBag():AddItem(40014, 1, ITEMLOG_CREATE_ROLE, 0, false);
			--oRole:GetItemBag():AddItem(40103, 1, ITEMLOG_CREATE_ROLE, 0, false);

			--oRole:LearnSpell(3400, 0, true); -- Poison follow-up

		-- Assassin configuration
		elseif class == clsRogue then

			oRole:SetLevel(1);

			-- Assassin active skills
			oRole:LearnSpell(1530, 0, true); -- Normal attack (1.05 id)
			oRole:EquipSpell(qssNormalAtk, 	1530);

			oRole:GetItemBag():AddItem(89900002, 1, ITEMLOG_CREATE_ROLE, 0, false); -- Starter cleaver (Assassin only)

		-- Archer configuration
		elseif class == clsArcher then

			oRole:SetLevel(1);

			-- Archer active skills
			oRole:LearnSpell(2040, 0, true); -- Normal attack (1.05 id)
			oRole:EquipSpell(qssNormalAtk, 	2040);

			oRole:GetItemBag():AddItem(40202, 1, ITEMLOG_CREATE_ROLE, 0, false); -- Starter large quiver (800 arrows, 1.05)
			oRole:GetItemBag():AddItem(99900070, 1, ITEMLOG_CREATE_ROLE, 0, false); -- Starter wooden bow (Archer only)

		-- Dragon Lancer (Spear) configuration
		elseif class == clsSpear then

			oRole:SetLevel(1);

			oRole:LearnSpell(1200, 0, true); -- Normal attack
			oRole:EquipSpell(qssNormalAtk, 	1200);

			oRole:GetItemBag():AddItem(99900100, 1, ITEMLOG_CREATE_ROLE, 0, false); -- Starter wooden spear (Lancer only)

		-- Sorcerer (Demon) configuration
		elseif class == clsSorcerer then

			oRole:SetLevel(1);

			oRole:LearnSpell(3200, 0, true); -- Normal attack
			oRole:EquipSpell(qssNormalAtk, 	3200);

			oRole:GetItemBag():AddItem(99900199, 1, ITEMLOG_CREATE_ROLE, 0, false); -- Starter ebony stick (Sorcerer only)
		end

	end

end
