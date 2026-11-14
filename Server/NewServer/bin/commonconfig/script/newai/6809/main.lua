Include("script/newai/sub_tree/sub_tree.lh")
LoadPackage("chat_def.pkg")
LoadPackage("global_config.pkg")
LoadPackage("data_manager.pkg")
LoadPackage("npc.pkg")	
LoadPackage("gameworld.pkg")
LoadPackage("role.pkg")
LoadPackage("character.pkg")
LoadPackage("item_bag.pkg")
LoadPackage("item.pkg")
Include("script/stringtable/stringtable.lh")
Include("script/newai/sub_tree/sub_tree.lh")
Include("script/include/btnod.lua")



--[[
nWeapon_Shengxing_Type ={}
  --Equipment types
    {
		[0] = ITEMSUBTYPE_EQUIP_WEAPON,		 --weapon		 1
		[1] = ITEMSUBTYPE_EQUIP_HAT,		--hat		2
		[2] = ITEMSUBTYPE_EQUIP_SHOULDER_ARMOR,	--shoulder armor	3
		[3] = ITEMSUBTYPE_EQUIP_CHEST_ARMOR,	--chest armor	4
		[4] = ITEMSUBTYPE_EQUIP_WRIST_ARMOR, --bracers5
		[5] = ITEMSUBTYPE_EQUIP_BELT,	--belt			 6
		[6] = ITEMSUBTYPE_EQUIP_SHOES,	--shoes			7
		[7] = ITEMSUBTYPE_EQUIP_CLOAK,	--cloak			  8
		[8] = ITEMSUBTYPE_EQUIP_NECKLACE, --necklace		9
		[9] = ITEMSUBTYPE_EQUIP_RING,	--ring			  	10
		[10] = ITEMSUBTYPE_EQUIP_BRACELET,	--bracelet	11
    },]]--
	




local g_oGameWorld = CGameWorld:GetInstance();

function Get_Jinbi(owner) --Get the gold cost of combining
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	local Type = weapon_one.m_pTemplate.m_bySubType;
	local cost = 0;
	
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one == 1 then  
		if Type == 1 or Type == 2 or Type == 4 then
			cost = g_GWConfig.nWeapon_Shengxing_WoMa[one] *2;		
			return cost;			
		else
			cost = g_GWConfig.nWeapon_Shengxing_WoMa[one];
			return cost;
		end
	elseif dang_one >= 2  and dang_one <= 5 then
		if Type == 1 or Type == 2 or Type == 4 then
			cost = g_GWConfig.nWeapon_Shengxing_Other[one] *2*(dang_one -1);	
		else
			cost = g_GWConfig.nWeapon_Shengxing_Other[one]*(dang_one -1);
		end
		if one <= 4 then 
			return cost;
		elseif one > 4 then
			return 0;
		end	
	end		

end

function Get_Yuanbao(owner) --Get the ingot cost of combining
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);	
	local Type = weapon_one.m_pTemplate.m_bySubType;
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	local cost = 0;	
	if one == 10 or two == 10 then
	--	role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
	--	role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one >= 2  and dang_one <= 5 then
		if Type == 1 or Type == 2 or Type == 4 then
			cost = g_GWConfig.nWeapon_Shengxing_Other[one] *2*(dang_one -1);			
		else
			cost = g_GWConfig.nWeapon_Shengxing_Other[one]*(dang_one -1);
		end
		if one > 4 then 
			return cost;
		elseif one <= 4 then
			return 0;
		end			
	else 
		return 0;
	end			

end


function Check_Money(owner)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local username = role:GetUserName();
	local yuanbao = role:GetItemBag():GetMoney(3);
	local jinbi = role:GetItemBag():GetMoney(1);
	local Type = weapon_one.m_pTemplate.m_bySubType;
	local dang_one = math.modf( itemid_one / 10000000);
	local one = itemid_one % 100;
	local cost = 0;
	
	if dang_one == 1 then    
		if Type == 1 or Type == 2 or Type == 4 then
			 cost = g_GWConfig.nWeapon_Shengxing_WoMa[one] *2;
		else
			 cost = g_GWConfig.nWeapon_Shengxing_WoMa[one];
		end
		if jinbi < cost then
			return brvFail;
		else
			return brvSuccess ; 
		end	
	elseif dang_one >= 2  and dang_one <= 5 then
		if Type == 1 or Type == 2 or Type == 4 then
			cost = g_GWConfig.nWeapon_Shengxing_Other[one] *2*(dang_one -1);
		else
			cost = g_GWConfig.nWeapon_Shengxing_Other[one]*(dang_one -1);
		end
		if one <= 4 then 
			if jinbi < cost then
				return brvFail;
			else
				return brvSuccess ; 
			end	
		elseif one > 4 then
			if yuanbao < cost then
				return brvFail;
			else
				return brvSuccess ; 
			end
		else 
			return brvFail;	
		end
	else 
		return brvFail;
	end
end

function Change_Money(owner)
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local qianghua = weapon_one.m_iteminfo.m_byLevelupPhyPoint + weapon_one.m_iteminfo.m_byLevelupMagPoint + weapon_one.m_iteminfo.m_byLevelupTaoPoint;
	local xingyun = weapon_one.m_iteminfo.m_byLuck;
	local mingwen1 = weapon_one.m_iteminfo.m_wRune[0];
	local mingwen2 = weapon_one.m_iteminfo.m_wRune[1];
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local username = role:GetUserName();
	local yuanbao = role:GetItemBag():GetMoney(3);
	local jinbi = role:GetItemBag():GetMoney(1);
	local Type = weapon_one.m_pTemplate.m_bySubType;
	local dang_one = math.modf( itemid_one / 10000000);
	local one = itemid_one % 100;
	local cost = 0;
	local VIP = role:GetVIPLevel();
	role:GetItemBag():ChangeYuanbao(username,0,false,0,1);
	if dang_one == 1 then    
		if Type == 1 or Type == 2 or Type == 4 then
			cost = g_GWConfig.nWeapon_Shengxing_WoMa[one]*2;
		else
			cost = g_GWConfig.nWeapon_Shengxing_WoMa[one];
		end
		role:GetItemBag():SubMoney(1, cost, MONEYLOG_SHENGXING, 0,0);
		role:GetItemBag():DeleteItemPos(CONSLOT_DEFAULT, 0, 1, MONEYLOG_SHENGXING, 0);
		role:GetItemBag():DeleteItemPos(CONSLOT_DEFAULT, 1, 1, MONEYLOG_SHENGXING, 0);
		if VIP == 0 then			
			role:GetItemBag(): AddItem(itemid_one +1,1,MONEYLOG_SHENGXING,0,false);
		elseif qianghua == 0 and xingyun ==0 and mingwen1 ==0 and mingwen2 ==0 then
			role:GetItemBag(): AddItem1(itemid_one +1,1,MONEYLOG_SHENGXING,0,false,weapon_two); --VIP inheritance
		else
			role:GetItemBag(): AddItem1(itemid_one +1,1,MONEYLOG_SHENGXING,0,false,weapon_one);		
		end	
		local msg = "Star upgrade succeeded, congratulations on obtaining "..role:GetItemBag():GetItemName(itemid_one + 1).."!"
			role:Say(role:GetObjID(), cltTvShow, msg);
		return brvSuccess;
	elseif dang_one >= 2  and dang_one <= 5 then
		if Type == 1 or Type == 2 or Type == 4 then
			cost = g_GWConfig.nWeapon_Shengxing_Other[one] *2*(dang_one -1);
		else
			cost = g_GWConfig.nWeapon_Shengxing_Other[one]*(dang_one -1);
		end
		if one <= 4 then 
			role:GetItemBag():SubMoney(1, cost, MONEYLOG_SHENGXING, 0,0);
			role:GetItemBag():DeleteItemPos(CONSLOT_DEFAULT, 0, 1, MONEYLOG_SHENGXING, 0);
			role:GetItemBag():DeleteItemPos(CONSLOT_DEFAULT, 1, 1, MONEYLOG_SHENGXING, 0);
			if VIP == 0 then
				role:GetItemBag(): AddItem(itemid_one +1,1,MONEYLOG_SHENGXING,0,false);
			elseif qianghua == 0 and xingyun ==0 and mingwen1 ==0 and mingwen2 ==0 then
				role:GetItemBag(): AddItem1(itemid_one +1,1,MONEYLOG_SHENGXING,0,false,weapon_two);	
			else
				role:GetItemBag(): AddItem1(itemid_one +1,1,MONEYLOG_SHENGXING,0,false,weapon_one);				
			end	
			local msg = "Star upgrade succeeded, congratulations on obtaining "..role:GetItemBag():GetItemName(itemid_one + 1).."!"
				role:Say(role:GetObjID(), cltTvShow, msg);
			return brvSuccess;
		elseif one > 4 then
			role:GetItemBag():ChangeYuanbao(username,cost,false,itemid_one,1);
			role:GetItemBag():DeleteItemPos(CONSLOT_DEFAULT, 0, 1, MONEYLOG_SHENGXING, 0);
			role:GetItemBag():DeleteItemPos(CONSLOT_DEFAULT, 1, 1, MONEYLOG_SHENGXING, 0);			
			if VIP == 0 then
				role:GetItemBag(): AddItem(itemid_one +1,1,MONEYLOG_SHENGXING,0,false);
			elseif qianghua == 0 and xingyun ==0 and mingwen1 ==0 and mingwen2 ==0 then
				role:GetItemBag(): AddItem1(itemid_one +1,1,MONEYLOG_SHENGXING,0,false,weapon_two);	
			else
				role:GetItemBag(): AddItem1(itemid_one +1,1,MONEYLOG_SHENGXING,0,false,weapon_one);
			end
			local msg = "Star upgrade succeeded, congratulations on obtaining "..role:GetItemBag():GetItemName(itemid_one + 1).."!"
				role:Say(role:GetObjID(), cltTvShow, msg);
			return brvSuccess;
		else 
			return brvFail;
		end
	else 
		return brvFail;		
	end
end


function Check_Weapon_one(owner)  --Purgatory weapon	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 1 or Type_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end

function Check_Weapon_two(owner)  --Purgatory hat	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;

	
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");		
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 2 or Type_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end

function Check_Weapon_three(owner)  --Purgatory shoulder armor	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;

	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 3 or Type_two ~= 3 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end

function Check_Weapon_four(owner)  --Purgatory chest armor	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two == nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	print(Type_one);
	print(Type_two);
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 4 or Type_two ~= 4 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end

function Check_Weapon_five(owner)  --Purgatory bracers	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 5 or Type_two ~= 5 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_six(owner)  --Purgatory belt	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 6 or Type_two ~= 6 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_seven(owner)  --Purgatory shoes	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 7 or Type_two ~= 7 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_eight(owner)  --Purgatory cloak	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 8 or Type_two ~= 8 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_nine(owner)  --Purgatory necklace	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 9 or Type_two ~= 9 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_ten(owner)  --Purgatory ring	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 10 or Type_two ~= 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_elven(owner)  --Purgatory bracelet	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 1 or  dang_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Purgatory-tier equipment.");
		return brvFail;
	elseif Type_one ~= 11 or Type_two ~= 11 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end






function Check_Weapon_One(owner)  --Judgment weapon	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 1 or Type_two ~= 1 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end

function Check_Weapon_Two(owner)  --Judgment hat	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 2 or Type_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end

function Check_Weapon_Three(owner)  --Judgment shoulder armor	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 3 or Type_two ~= 3 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end

function Check_Weapon_Four(owner)  --Judgment chest armor	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");	
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 4 or Type_two ~= 4 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end

function Check_Weapon_Five(owner)  --Judgment bracers	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");	
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 5 or Type_two ~= 5 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_Six(owner)  --Judgment belt	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");	
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 6 or Type_two ~= 6 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_Seven(owner)  --Judgment shoes	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");	
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 7 or Type_two ~= 7 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_Eight(owner)  --Judgment cloak	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");	
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 8 or Type_two ~= 8 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_Nine(owner)  --Judgment necklace	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");		
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 9 or Type_two ~= 9 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_Ten(owner)  --Judgment ring	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");	
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 10 or Type_two ~= 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end


function Check_Weapon_Elven(owner)  --Judgment bracelet	--Check that both items are identical and can be combined; return success if so
	local role = g_oGameWorld:FindValidRole(get_ctx_role_id());
	local weapon_one = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 0);
	local weapon_two = role:GetItemBag():GetItem(CONSLOT_DEFAULT, 1);
	if weapon_one == nil or weapon_two ==nil then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Place upgradable equipment in the first and second bag slots.");	
		return brvFail;
	end
	local itemid_one = weapon_one.m_iteminfo.m_nTemplateID;
	local itemid_two = weapon_two.m_iteminfo.m_nTemplateID;
	local dang_one = math.modf( itemid_one / 10000000);
	local dang_two = math.modf( itemid_two / 10000000);
	local Type_one = weapon_one.m_pTemplate.m_bySubType;
	local Type_two = weapon_two.m_pTemplate.m_bySubType;
	
	local one = itemid_one % 100;
	local two = itemid_two % 100;
	if one == 10 or two == 10 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert equipment with 1-9 stars.");	
		return brvFail;		
	elseif itemid_one ~= itemid_two or itemid_one > 59990000 or itemid_one < 19920000 or itemid_two > 59990000 or itemid_two < 19920000 then 
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert upgradable equipment.");	
		return brvFail;
	elseif dang_one ~= 2 or  dang_two ~= 2 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Please insert Verdict-tier equipment.");
		return brvFail;
	elseif Type_one ~= 11 or Type_two ~= 11 then
		role:GetScene():Say(role:GetObjID(), cltTvShow, "Equipment type does not match.");
		return brvFail;
	else
		return brvSuccess; 
	end
end

Include("script/newai/6809/main.uh")