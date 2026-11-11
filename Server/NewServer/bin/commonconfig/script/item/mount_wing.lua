-- 1.05 backport: mounts (and wings stub).  Loaded from script/item/main.lua via Include.
-- Item Script functions: ActivateMountCommon / ActivateMountTimeLimited / MountOne4All / ActivateMountN (legacy 2016 items)
-- item id -> mount id (mount.txt, 1.05). Names matched item.txt <-> mount.txt.
MOUNT_ITEM_TO_ID = {
	[39101] = 1,   -- 白泽·幼
	[39102] = 2,   -- 追风
	[39103] = 3,   -- 玛法之魂
	[39104] = 4,   -- 驯鹿
	[39105] = 5,   -- 琉璃鹿
	[39106] = 6,   -- 剑齿虎
	[39107] = 7,   -- 奔雷兽
	[39108] = 8,   -- 白泽
	[39109] = 9,   -- 踏风隼
	[39110] = 30,   -- 青尾
	[39111] = 11,   -- 龙髯
	[39112] = 10,   -- 覆甲獠牙
	[39113] = 13,   -- 长牙
	[39114] = 14,   -- 偃甲灵鼠
	[39115] = 15,   -- 机关木鼠
	[39116] = 16,   -- 熔火龙蜥
	[39117] = 18,   -- 牛魔
	[39118] = 45,   -- 蛮牛2
	[39119] = 12,   -- 血牙
	[39120] = 19,   -- 三尾
	[39121] = 20,   -- 玄虎
	[39122] = 21,   -- 踏云
	[39123] = 22,   -- 灵兔
	[39124] = 23,   -- 军机魔蝎
	[39125] = 24,   -- 利爪
	[39126] = 25,   -- 巅峰战熊
	[39127] = 26,   -- 军机戎车
	[39128] = 27,   -- 军机冰鳞
	[39129] = 28,   -- 军机苍鹿
	[39130] = 29,   -- 苍龙
	[39131] = 31,   -- 军机铠马
	[39132] = 32,   -- 啸月霜狼
	[39133] = 33,   -- 苍狼
	[39134] = 34,   -- 熔锋
	[39135] = 35,   -- 炎息
	[39136] = 38,   -- 龙裔霸主
	[39137] = 36,   -- 腾蛇
	[39138] = 37,   -- 丰禄
	[39140] = 39,   -- 风火轮
	[39141] = 40,   -- 上古青龙
	[39142] = 41,   -- 王者战狼
	[39143] = 42,   -- 军机三首蛟
	[39144] = 43,   -- 金蛇云辇
	[39145] = 44,   -- 破魔战车
	[39146] = 45,   -- 蛮牛
	[39147] = 46,   -- 玄螭
	[39148] = 47,   -- 紫晶蝎
	[39149] = 48,   -- 上古朱雀
	[39150] = 49,   -- 驼驼
	[39151] = 50,   -- 飞天魔毯
	[39152] = 51,   -- 天马
	[39153] = 52,   -- 闻古剑
	[39154] = 53,   -- 彤云踏雪
	[39155] = 54,   -- 上古白虎
	[39156] = 55,   -- 军机战象
	[39157] = 55,   -- 军机战象(14天)
}
MOUNT_ALL_IDS = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55}

local function _ActivateMountByID(pRole, nMountID)
	local ret = pRole:ActivateMount(nMountID);
	if ret == errSuccess then
		return errSuccess;
	end
	-- CRole::ActivateMount already sent ErrorCodeNotify (already owned / level too low): keep the item
	return errItemNoConsumeUse;
end

function ActivateMountCommon(pItem, pRole)
	local nMountID = MOUNT_ITEM_TO_ID[pItem.m_pTemplate.m_nTemplateID];
	if nMountID == nil then
		return errItemNoConsumeUse;
	end
	return _ActivateMountByID(pRole, nMountID);
end

-- time limited mount item: expiry is not implemented yet, behaves like a permanent unlock
function ActivateMountTimeLimited(pItem, pRole)
	return ActivateMountCommon(pItem, pRole);
end

-- "Mount compendium": unlock every mount
function MountOne4All(pItem, pRole)
	for _, nMountID in ipairs(MOUNT_ALL_IDS) do
		pRole:ActivateMount(nMountID);
	end
	return errSuccess;
end

-- legacy 2016 item scripts ActivateMount<N> (N = mount id): items 39101/39102/39103/39108/39109
for _, nMountID in ipairs(MOUNT_ALL_IDS) do
	_G["ActivateMount" .. nMountID] = function(pItem, pRole)
		return _ActivateMountByID(pRole, nMountID);
	end
end

-- Wings (1.05 "Illusion Wings"): the item id is the wing id of wings.txt (wings.txt gives the model and the attribute bonus).
-- Using a wing item for the first time unlocks the wing (CRole::GrantWing, shown in the wing panel "UnrealWing"), wears it
-- and uses the item up.  Using an item of an already owned wing keeps the item and wears / takes off that wing, so wings
-- can also be swapped from the bag.  Wear state is CRole::EquipWing / UnequipWing (saved with the role, synced to all clients),
-- owned wings and the wing level are role variables 2046 / 2047 (see wings_data.h).
WING_ITEM_TO_ID = {
	[39499] = 39523,   -- Emperor wing (14 days)  -> same model as 39523
}

local function _ToggleWing(pItem, pRole)
	local nItemID = pItem.m_pTemplate.m_nTemplateID;
	local nWingID = WING_ITEM_TO_ID[nItemID] or nItemID;
	if pRole:GrantWing(nWingID) then
		pRole:EquipWing(nWingID);
		pRole:SyncWingInfo();
		return errSuccess;
	end
	if pRole:GetCurrentWingID() == nWingID then
		pRole:UnequipWing();
	else
		pRole:EquipWing(nWingID);
	end
	pRole:SyncWingInfo();
	return errItemNoConsumeUse;
end

ActivateWings = _ToggleWing;
-- time limited wing item: expiry is not implemented yet, behaves like a permanent wing
ActivateWingsTimeLimited = _ToggleWing;
