LoadPackage("hero.pkg");
LoadPackage("role.pkg");
LoadPackage("char_def.pkg");
LoadPackage("hero_def.pkg");
LoadPackage("item_bag.pkg");
LoadPackage("spell_book.pkg");
LoadPackage("spell.pkg")

function main(oRole, oHero)
	print("Hero Created");

	local class = oHero:GetClass();

	if class == clsWarrior then

		-- Provide character weapon
		oRole:GetItemBag():EquipHeroItemOnBirth(10110211, NES_MAIN_HAND); -- Two-handed heavy short weapon (two-handed sword)

		-- Assassin configuration
	elseif class == clsRogue then

		-- Provide character weapon
		oRole:GetItemBag():EquipHeroItemOnBirth(10120211, NES_MAIN_HAND); -- Dual daggers

		-- Archer configuration
	elseif class == clsArcher then

		-- Provide character weapon
		oRole:GetItemBag():EquipHeroItemOnBirth(10130111, NES_MAIN_HAND); -- Dual crossbows

		-- Mage configuration
	elseif class == clsMage then

		-- Provide character weapon
		oRole:GetItemBag():EquipHeroItemOnBirth(10140311, NES_MAIN_HAND); -- Staff type

		-- Dragon Lancer (1.05) configuration
	elseif class == clsSpear then

		oRole:GetItemBag():EquipHeroItemOnBirth(99900100, NES_MAIN_HAND); -- Wooden spear

		-- Sorcerer (1.05) configuration
	elseif class == clsSorcerer then

		oRole:GetItemBag():EquipHeroItemOnBirth(99900199, NES_MAIN_HAND); -- Ebony wand

	end

end
