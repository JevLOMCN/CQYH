# 1.05 Backport Roadmap

Goal: bring the v1.05 retail client's content and features into our 2016 source client and server (branch `1.05-backport`).

**Status:** the asset/data migration is done (classes, maps, gear, skill data, mounts, encoded packages). What remains is **code**: systems that 1.05 ships tables and UI for, but that our C++ client/server and `MMOGame.u` do not implement. This file now tracks those systems. Tick items off as they are done. Notes in *italics* are unverified.

## References
- 1.05 export (cooked assets): `H:\1.05 Client Export\MMOGame` (`CookedPC\NNNNN.pak\...`, `Config\00000.pak\*.txt`)
- Packed 1.05 client: `G:\Mir3DEMU\1.0.5.50\传奇永恒\CQYH`
- Working emulator: `G:\Mir3DEMU\GitHub\Eternal Legend Latest\Eternal-Legend`. Ground truth for 1.05 ids/tables only. It implements login / char-select / enter-world and has **no** packet or logic for Physique, Mingwu, Wushuang, LegendPower, DragonSoul or Couture. Message ids/layouts for these must be invented (our own `gen_msg` on both ends) or recovered from the 1.05 client binary (`MMOGame.exe` / `MMOGame.u`).
- Decoded 1.05 config tables (336 of 361 files): `%TEMP%\cfg105\` (decoder `%TEMP%\decode105.ps1`)
- Encoded-package decoder: `H:\1.05 Backport assets\decode_pkg\decode_pkg.py`; decoded output `H:\1.05 Backport assets\decoded\`
- Earlier analysis notes (job tmp dir `C:\Users\johne\.claude\jobs\4b3f839e\tmp\`): `systems_plan.md`, `config_gap.md`, `u_script_gap.md`

## Class ids (1.05)
| Id | Class | Model group | Login Kismet prefix |
|---|---|---|---|
| 0 | Warrior 战士 | WA | `WA-` |
| 1 | Mage 法师 | MA | `MA-` |
| 2 | Rogue/Assassin 刺客 | AG | `AG-` |
| 3 | Archer 弓手 | AR | `AR-` |
| 4 | Taoist 道士 | TA | `TA-` |
| 5 | Lancer/Spear 龙枪 | SP | `SP-` |
| 6 | Sorcerer/"Demon" 妖师 | YS | `SO-` |

Client enum: `A..E` = 0-4, `G` Lancer = 5, `H` Sorcerer = 6, `F` npc = 7. Server `clsNpc` = 7.

## Done (asset / data migration)
- **Character select/create**: 1.05 login movie, scene (`Login_fast`), icons, appearance data; all seven classes creatable with previews, descriptions, skills, names.
- **Client class support**: enums, `char_def.h`, spell class struct, animation tables, equip class checks, HUD, ~dozen per-class switches, `ConvertSpellType`, generic `AttackClassGeneric` (normal attacks work for all new classes), per-class gear mesh lists.
- **Server class support**: `char_def.h`, Lua bindings, `RequireClass` bitmask, spell `Class` mask (`GetClassSpellMask`), quest "any class" sentinel migrated 6 -> 8, create_role/hero_create/level scripts, Rogue/Archer data fixes, quivers.
- **Class skill data**: spell / spell_var / spell_progress / rune rows for all new-class skills; ~160 damage skills have server effects (17 nodes in `modifier_tree.mdf`); 40 skill books incl. Sorcerer; shops; 777 + 118 gear rows; item/rune/shop load failures fixed.
- **Maps**: all 106 missing 1.05 scenes ported (collision, scene rows, NPC templates, doodads, 692 portals, quests, drops). Spawn failures 1166 -> 161.
- **Mounts** wired end to end (mount.txt 54 rows, on/off mount packets, `mount_wing.lua`); **wings** = data + packages only.
- **Encoded packages**: all 256 decoded and verified; Sorcerer files load in the client.
- **Script classes**: stubs for `AbilityEquipMeshExchangeComponent`, `AbilityRoleMountComponent`, `MMOStatusProc_ExchangeSkill`; decoded packages rescanned, no further missing.
- Tables extended: attribute, achievement (+51), kill_npc_quest, set, title, and others (see `config_gap.md` for the full per-table status).

## Prerequisite for every system below
- [ ] **Rebuild `MMOGame.u` with the missing 1.05 script classes**: `MMOUIMartialRoadMap`, `MMOUIMirBible`, and the `UC*` callbacks / panel classes for Physique, CQPower, Couture, DragonSpirit, Wushuang, etc. (name-level gap only, no decompiler; 1.05 `MMOGame.u` is store-compressed so many names are invisible). Script compile must use the 64-bit `Binaries\Win64\MMOGame.exe make ...` (the Win32 exe crashes and deletes `MMOGame.u`).
- [ ] **Attribute enum additions** (server `attribute_def.h` + `.uc`): `catStabAtk`, `catMaxStabAtk`, `catArcheryAtk`, `catMaxArcheryAtk`, `catFreeze/Slow/Burning/Bleeding` (+Resist), `catPhyHeal`, `catMageHeal`, `catMF`, `catMin/MaxIgnoreDefDmg`, `catMaxNormalMonsterHurtDeepen`, `catBossMonsterHurtDeepen`, `catAntiPhyDefRate`, `catAntiMagDefRate`, `catMagAccurate`. About 4,200 `attribute.txt` rows and the attributes1/2 base-stat columns wait on these.
- [ ] **Engine decode-on-load** (`FFileManagerMPQ` / `UnLinker`) so encoded packages need not be pre-decoded.

Every system needs: table loader(s) (client and/or server), role DB field(s), message pair(s) (`gen_msg`), UI (swf + natives + `.uc`), and modifier injection through the existing attribute path.

## Systems needing code (recommended order)
Server sources are GBK: see memory note before editing.

### 1. Legend Power (CQPower), effort M (4-6 days), best cost/benefit
- Tables: `legend_power` (142 rows: 20 levels x 7 classes, attr type/value pairs, cost items 95001/95002).
- UI: `UI_CQPower.upk` (`CQPower.swf`), hooks in `UI_CharactorWidget` (`Btn_CqPower`) and `UI_PlayerInfoWidget` (`ShowCqPower`). Natives: `UCNotifyCqPowerUpgradeSuccess`, `UCSetCqPowerUpCsum`, `UCEquipCqPowerSkill`.
- Server: one role int (level), upgrade handler (consume item, add attrs via modifier tree), loader.
- Needs items 95001/95002 added.
- [ ] loader  [ ] role DB field  [ ] msgs  [ ] upgrade handler  [ ] client natives + panel

### 2. Couture, effort S (stopgap) / M-L (real, 6-9 days)
- Tables: `Couture` (59), `CoutureSet` (37), `EquipGroupEffect` (114); `ActivateCouture` items 100101+. Assets: `Couture_Set.upk`, `MMO_ItemIcon_Couture.upk`, 85 decoded packages in `decoded\12_EncodeRes_Couture`. Copy under unique base names.
- **Stopgap, no client code**: add the 5 chest couture sets as ordinary `item_armor` rows (`ARMORTYPE_AVATAR`, `EquipAppearenceTemplate` = `Couture_Set.Couture_body_NN`); server already has `nAvatar[3]` and the client `EquippedItemList_Avatar`. Per-class/gender package selection needs verifying at runtime. Heads (face + hair) and feet (FX only) are not covered: no avatar head slot (`NES_VISIBLE_TOTAL` stops at BACK; adding one means a role DB version bump).
- **Real**: role DB (unlocked bitmap, worn id per slot, show flag), loaders, wear/unwear/show handlers, set bonuses via `set.txt`-style path, appearance broadcast carries couture ids, client overlay in `Equip.cpp`/`MMOActor`, wardrobe tab (`CharacterWidget_Couture` exists only in the 1.05 `UI_CharactorWidget.upk`, which needs the matching `.u` classes).
- Transform buffs 44003/44007 (`AbilityEquipMeshExchangeComponent`) swap the whole look temporarily: data class exists, no C++ node.
- [ ] copy assets  [ ] chest avatar items (stopgap)  [ ] real wardrobe

### 3. Dragon Soul + Physique (share the MartialRoadMap host), effort L
Do together: both live in `UI_MartialRoadMap.upk` (needs `MMOUIMartialRoadMap`).
- **Dragon Soul** (M-L, 8-12 days): `dragon_soul` (49-51), `dragon_soul_upgrade` (205). Per-role level / sub_level / exp; upgrade handler (items 140001-3, exp_param table); rune buff activation (`rune_buffer_id` -> aura); achievements `adDragonSoulEnable` / `ActiveRune`; PlayerInfo panel 7 `dragonSpiritSlot`.
- **Physique** (L, 12-18 days, largest): `Physique` (808), `physique_breakthrough` (26), `physique_wash` (26). Equipment per-slot level + washable pre/post-suffix affixes. Per-role DB blob (part level, current affix ids, record slots); open/query, break-through, wash (with lock), apply/revert record; inject into char_modifier (`patPre_*` are skill-modifier ids); cost items 90500/90502/90504/90506 (present). UI: `UI_Physique.upk` (`Physique.swf`), physique tab in MartialRoadMap, `PhysiquePanel` / `PhysiqueProgressBar`. Natives `PhysiqueData`, `UpdatePhysiqueListData`, `PhysiqueLevelPower`.
- Needs items 140001-3 and 140201-3 added.
- [ ] MartialRoadMap host  [ ] Dragon Soul  [ ] Physique

### 4. Mingwu + Wushuang (+ strengthen_skill), effort L (10-15 days), blocked on spell/aura port
- Tables: `mingwu` (121), `wushuang` (7), `wushuang_skill_upgrade` (152), `strengthen_skill` (16) + `_upgrade` (44). 7 classes x 17 mingwu + 5 wushuang skills.
- Blocked: spells 4800-4900 (0 of 119 mingwu spell ids in our `spell.txt`), 1132.. strengthen skills, and new auras. Items 140201-3 missing.
- Server: learn/upgrade handlers (exp + items), mingwu slotting, spell + Support_Skill modifiers. Client: skill-tree tab (`UI_SkillTree`), tooltips (`Desc_LvUp/Lv10/Lv20/Lv30`), icons in `MMO_UIIcon.upk`.
- [ ] spells/auras  [ ] tables + handlers  [ ] UI

### 5. Skill effects still missing
- [ ] Buffs, auras, summons, DoTs for the new-class skills have no effect (damage skills work). `spell_node.txt` rows are templates; real 1.05 modifier-tree nodes are not in `modifier_tree.mdf`.
- [ ] `aura` / `missile` tables: 1.05 client view has only the "Both" columns (aura 23 of 60), so server-only effect columns are unknown; cannot port safely from this source. Key format differs too.
- [ ] 11 `rune` rows for spells 1533 / 3209 / 3216 still fail to load.
- [ ] No missiles for the new skills. 01_Players / 06_EquipSkill / Taoist-Mage 223 helper abilities (wings, brother, Wushuang, ...) not added.
- [ ] Generic item rows (potions, scrolls) list only 5 classes in `RequireClass`; Lancer/Sorcerer may be unable to use them.
- [ ] Rogue/Archer level-up skills all zero.

### 6. Wings and mounts
- [ ] Wings: display code and server code (currently data + packages only). Tables `wings` (29), `wings_level` (62), `UnrealWing` / `wingProperty` script names.
- [ ] Mount rider pose per mount (`MountType` field was backed out). `mount_power` (282 rows) and `LastMountPowerSum` not ported.

### 7. World map / dungeons / NPC AI
- [ ] 1.05 `UI_WorldMap` swf needs client AS support (boss tracking, jump-to-pos); `boss_pos`.
- [ ] Teleport-NPC AI trees need engine node types.
- [ ] ~90 maps have no walk-in portal (GM `.jump <name>` only); Sabak gate portals and MirAct49 ids 128-134 exceed `MAX_TELEPORT_POINT_PER_SCENE=128`.
- [ ] Server `dungeon.txt` rows for new dungeons; 1.05 `Activity.txt` (46 vs our 12); MirAct27/28 have no server data.
- [ ] Per-map big-map background art; 27 stand-in monster models left (44 earlier).

### 8. Other features, each its own M-L project
Tables are in `config_gap.md` section B; 46 UI packages are missing in total. Pick as wanted:
- **Equipment v2**: `equip_refine`, `equip_refine_attribute`, `equip_compose`, `equip_convert`, `equip_levelup_advanced` + `armor_levelup` (5330 rows), `equip_socket_rfin`, `refine_rank_attribute`, `socket_rank_attribute`, `adv_inc_weapon_luck`, `gamble_attribute`, `weighted_random_attribute`, `npc_item_random_attribute`. UI: `UI_EquipRefineAttr`, `UI_EquipSocket*`, `UI_EquipUpgrade*`, `UI_JewelryRefine`.
- **Equip soul / excellent equip**: `equip_soul_*`, `excellent_refine_*`, `excellent_equip_*`, `holy_attack`, `soul_stone` / `soul_stone_attr`.
- **Consign (auction v2)**: `consign` (1503 rows, new file), `AuctionCategory` (5 ids differ), `UI_Consign`. Extends our existing auction.
- **Monthly / weekly / gift**: `monthly_card`, `monthlyCard`, `weekly_package`, `WeeklyThemePackage*`, `optional_gift` (454), `UI_MonthlyCard`, `UI_RechargeMonthly`, `UI_OptionalGift`, `UI_GiftPackage`. New currencies in `money` (14-19) need server enum + `item_def` logic.
- **Growth guides**: `path2throne_*` (7-day growth), `grow_up` (101), `seven_days_list`, `FunctionUnlock` (149), `DailyGuide`, `mirBible` (`MMOUIMirBible`), `checkin` (new Type/Highlight columns; loader reads positional rewards, needs review).
- **PvP / events**: Chaos battle (`Chaos_rules`, `ChaosAward`), arena (`ArenaEquip`, `PvpWrestleRwd`), Sabak contest (`UI_SabacContest`), brotherhood (`brother_*`), treasure hunt (`treasurehunt`, `hunting`, `serial_scene`), `feat_*`, wanted/bounty, `UI_SecretRift`, `UI_SuppressDemons`, `UI_MilitaryOrders`.
- **Living skills / misc**: `compound_formula`, `ore_herb_formula`, `recycle_*` + `item_convert`, `rune_Carve`, `ChantSpell`, `AutoSupply`, `ItemSource`, `elite_notice`, `activity_tower_reward`, `UI_RemakeBook/Factory`, `UI_ExpBooster`, `UI_Honor`.
- **Chat filter**: `CensoredWordList_Chat` / `_ChatRecive` (loaders exist, data not shipped).
- **Achievements / handbook**: ~186 achievements need new condition types (`adDragonSoul*`, `adWushuang*`, `adBrother*`, `adFriend*`, `adRuneCarve`, `adWrestleRank`, `adCompleteQuest`, `adEquipDress`); `handbook` +1327 and `UI_HandbookRune` wait on item/npc merge.
- **Item-id dependent tables** (port with the item merge, in order): `Compose`, `decompose`, `drop_group`, `AssistDrop`. Need server-only columns defaulted.
- `mail_template` layout changed (text moved to StringMap) and breaks our loader. `StringMap`: 5,738 common rows differ, merge via the translation pipeline, not append. `level.AwakenPower` and levels 61-63 need awaken support.

## Open leftovers (small)
- [ ] Swf calls not yet fed on char create: `UCSetCreateModuleVersion`, `UCSetSelectRole`; turn buttons `ASCreateTurn*` renamed/missing in the 1.05 swf.
- [ ] Missing login particle materials `FX_Warrior_SE_Alondra`, `FX_Sword1H_99`: not in the 1.05 export.
- [ ] Face-icon textures `MMOUIFaceIcon.Player.class5/class6`: close the game, rename `MMOUIFaceIcon.upk.new-1.05` over `MMOUIFaceIcon.upk` in `Content\MMOContent\08.UI` (2016 original is `.orig-2016`).
- [ ] `All_Necklaces_app` still pending (file was locked by the running game).
- [ ] `MMOPlayerController.uc` SaveSlotBind only handles A/B (as in 2016).
- [ ] Sorcerer books and some 1.05-only potion packs not sold in shops; shops 61, 63-68/14/6461/6485/6997/7000/7001/8000 have NPCs but no rows.
- [ ] Server `spell_node.txt` for ranged skills reuses Mage fireball nodes; no in-game test of the new-class gear and skills yet.
- [ ] Spear/Sorcerer `attributes2` / `refine_attr` are copies of Warrior/Mage. Spear/Sorcerer only feed the overall level/power ranklists.
- [ ] Couture sets are not covered by the gear backport (3 `Couture_Set` items skipped).
- Staged (client files read-only in the working tree): `quest_doodad`, `quest_story` (chapters 6-11), `hint`, `TweenData`. See `MERGE.md`.

## Housekeeping
- Server: start with `Server\NewServer\bin\start_servers_wt.ps1` (Windows Terminal tabs). Needs ~6 min to start. `certserver` is not started by it: run `certserver\certserver64_d.exe /console` from `bin` first.
- Game server debug build error MessageBox at init is disabled via `globalconfig/gameworldconfig.lua` `bShowErrorDialog=false`.
- Client: `Binaries\Win64\MMOGame.exe -sdot -log -dx11 -ip=1,1,127.0.0.1,8701,127.0.0.1,8701,Legend`
- TEMP diagnostics still in the client (remove before committing): `[SetMeshDiag]`/`[MeshDiag]` (`MMOActor.cpp`, `ItemManager.cpp`), `[AtkDiag]` (`FSMIO.cpp`), `[SlotDiag]` (`message_handler_Combat.cpp`)
- Never commit: `MMOEditorUserSettings.ini`, generated headers, shader `.bin`, and the replaced game packages in `Content` (all gitignored, backups are `*.orig-2016`)
