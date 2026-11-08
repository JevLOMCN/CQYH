# 1.05 Backport Roadmap

Goal: bring the v1.05 retail client's content and features into our 2016 source client and server (branch `1.05-backport`).
Tick items off as they are done. Notes in *italics* are unverified.

## References
- 1.05 export (cooked assets): `H:\1.05 Client Export\MMOGame` (`CookedPC\NNNNN.pak\...`, `Config\00000.pak\*.txt`)
- Packed 1.05 client: `G:\Mir3DEMU\1.0.5.50\传奇永恒\CQYH`
- Working emulator (ground truth for 1.05 ids/tables): `G:\Mir3DEMU\GitHub\Eternal Legend Latest\Eternal-Legend`
- Decoded 1.05 config tables (336 of 361 files): `%TEMP%\cfg105\` (decoder `%TEMP%\decode105.ps1`)

## Class ids (1.05)
| Id | Class | Model group | Login Kismet prefix | Status |
|---|---|---|---|---|
| 0 | Warrior 战士 | WA | `WA-` | in 2016 |
| 1 | Mage 法师 | MA | `MA-` | in 2016 |
| 2 | Rogue/Assassin 刺客 | AG | `AG-` | in 2016, unfinished |
| 3 | Archer 弓手 | AR | `AR-` | in 2016, unfinished |
| 4 | Taoist 道士 | TA | `TA-` | in 2016 |
| 5 | Lancer/Spear 龙枪 | SP | `SP-` | **new** |
| 6 | Sorcerer/"Demon" 妖师 | YS | `SO-` | **new**, assets encoded |

## Findings so far
- Package format identical (version 894, licensee 0). 1.05 has 14,795 packages vs 10,620 ours; 4,181 new.
- 256 packages use a non-standard magic ("encoded"): all 105 `YS` character packages, 85 couture sets, mounts, wings, some monsters/scenes. Six byte-substitution tables; our `FArchiveEncode` (seed 897502) does **not** decode them. Algorithm unknown.
- 1.05 `UI_Login.Login` swf (extracted to `LoginWidget.swf`) already has all seven `BtnProf_*` buttons.
- 1.05 login map `Login_fast.udk` has `SP-`/`SO-` events; our `Login_n.udk` does not.
- Scripts: of 127 `MMOGame.*` classes used by 1.05 packages, 124 exist in our source. Missing: `AbilityEquipMeshExchangeComponent`, `AbilityRoleMountComponent`, `MMOStatusProc_ExchangeSkill`. (Encoded packages not scanned.)

## Phase 1: Character select / create screen
- [x] 1.05 login movie, scene (`Login_fast`), icons and appearance data installed; all seven classes can be created, with previews, descriptions, four featured skills and names
- [x] Client enum matches the server: `A..E` = 0-4, **`G` Lancer = 5, `H` Sorcerer = 6, `F` npc = 7, one` = 8**; class names in `StringMap`; `SP-`/`SO-` Kismet events (preview and appearance step)
- [x] Heads/hair/face show for Rogue, Archer, Lancer (missing character packages copied into `Content`); Demon works with the decoded `YS_*_Encode` packages
- [ ] Swf calls not yet fed: `UCSetCreateModuleVersion`, `UCSetSelectRole`; turn buttons `ASCreateTurn*` renamed/missing in the 1.05 swf
- [ ] Missing login particle materials (`FX_Warrior_SE_Alondra`, `FX_Sword1H_99`): not in the 1.05 export

## Phase 2: Client class support
- [x] Class enum, `char_def.h` (client copy), spell class struct (`bClassType7/8`), animation tables, equip class checks, HUD globe materials
- [x] Models: 1.05 `GamePlayData`, `AvatarListData`, `MMO_Creat_Icon`, `MMO_UIIcon` installed; Lancer assets copied; Sorcerer packages decoded (see Phase 4) and installed
- [x] Item parser knows `clsSpear` / `clsSorcerer` (starter weapons equip); per-class gear mesh lists (`MeshList_AG/AR/SP/YS`) in `EquipAppearence` + `Equip.cpp`
- [x] **Normal attack works for all new classes**: generic `AttackClassGeneric` handler (Rogue 1530, Archer 2040, Lancer 1200, Sorcerer 3200); client spell tables got the four normal attacks
- [x] **Class skill sets** (data side): client + server `spell.txt` / `spell_var.txt` / `spell_progress.txt` / `rune.txt` rows for all Rogue, Archer, Lancer and Sorcerer spells and inscriptions (+ Strengthem), server `spell_node.txt` rows, auto-learn at the 1.05 `RequireLevel` in `level/main.lua`, Rogue/Archer ability packages (+ dependencies) copied into `Content`. *Caveat: server `spell_node.txt` rows are templates (ranged targeted skills reuse the Mage fireball nodes, everything else has empty nodes), because the real 1.05 modifier-tree nodes are not in `modifier_tree.mdf`; no in-game test yet; no skill-book items, missiles or auras for the new skills*
- [ ] Shared abilities for new classes: server now has 1.05's 05_CommonAbility rows (4003, 4004/4005, 4010-4054, 10000-10001, 40102-40540, 40200-40201, 19500; client keeps mask 31 for the former 223 rows); the 01_Players/06_EquipSkill/Taoist-Mage 223 helpers (wings, brother, Wushuang, ...) were not added; client loader still maps the class mask with the old 5-class layout (`MMOSkillManager::ConvertSpellType`)
- [x] Per-class switches (~dozen): `MMOGFXPlayer`, `MMOGuildData`, `MMOJourneyData`, `MMOUITeam`, `MMOTargetManager`, `MMOUIJourney*`, `MMOUIMsgInfo`, `MMOGFXPlayer_KillRecord`, `MMOUIHandbookPanel`, `UITextParser`, `UMMOUIRankListPanel`, `MMOGuideManager`, `MMOAttributeAccountManager` (G/H reuse Warrior/Mage rows), `MMOUIReputationPanel`; spell class mask loader (`ConvertSpellType`) now maps Lancer/Sorcerer/npc. *(G/H reuse Warrior/Mage flash frames, voices and reputation rewards; `MMOPlayerController.uc` SaveSlotBind still only handles A/B, as in 2016)*
- [ ] Face-icon textures `MMOUIFaceIcon.Player.class5/class6`: 1.05 package staged as `MMOUIFaceIcon.upk.new-1.05` in `Content\MMOContent\08.UI` (game was running and locked the file): close the game, rename it over `MMOUIFaceIcon.upk` (2016 original already backed up as `.orig-2016`)
- [x] Missing script classes (script stubs written; encoded packages rescanned, no further missing; `AbilityEquipMeshExchangeComponent`, `AbilityRoleMountComponent`, `MMOStatusProc_ExchangeSkill`)
- [x] Armor/cap/boots/shoulder/jewelry/weapon item data for the new classes up to level 40: 777 rows appended to client + server `item_armor.txt` (Stab/Archery folded into Phy); 1.05 `AG_Set`, `AR_Set`, `All_Honors/Jades/Offhand_app`, `Dro_*`, icon and FX packages copied into `Content`, 13 older `EquipAppearence` packages replaced (backups `.orig-2016`; `All_Necklaces_app` still pending, file was locked by the running game). *Not done: 118 items above level 40, 3 `Couture_Set` items; the client has no couture feature (no `Couture` in source/config), so couture sets are not ported. No in-game test yet*
## Phase 3: Server class support
- [x] `char_def.h`: add `clsSpear`, `clsSorcerer` before `clsPlayerEnd` (1.05 names) *(source edited, needs rebuild; `clsNpc` is now 7)*
- [x] Lua bindings (4 files), `[clsPlayerEnd]` arrays, ranklist/statistic switches *(Spear/Sorcerer only feed the overall level/power ranklists; no per-class lists or kill counters)*
- [x] Quest sentinel: ClassLimit == 6` means "any class" (`quest_list.cpp:184,647`); Demon is id 6. *Done differently:* any non-player value means "any", and `quest.txt` was migrated 6 -> 8 (the value the 1.05 tables use), in both `commonconfig\quest.txt` and `setting\quest.txt`
- [x] Item `RequireClass` bitmask (fits 0-6) *(spell `Class` mask handled by `GetClassSpellMask`: Spear 64, Sorcerer 128, monsters 32, as in the 1.05 spell table)*
- [ ] Data: attributes1/2, refine_attr, reputation columns, spell.txt, newroleconfig.lua, create_role / hero_create / level scripts *(all done except `level` and item/gs_gm gift scripts; spell.txt has only the 4 normal attacks 1530/2040/1200/3200, no class skill sets; Spear/Sorcerer attributes2/refine_attr are copies of Warrior/Mage)*
- [x] Fix Rogue/Archer data: spell 2000 and 1500 missing (now 1530/2040), starting item 99800003 missing (now 99900070 bow / 89900002 cleaver). *Still open: level-up skills all zero, no class skill sets, quivers 40201/40202 now exist (`ITEMSUBTYPE_USABLE_QUIVER` added; starter Archer gets 40202; sold in shops 5/15/25/35/62/69 - needs rebuild + in-game check)*
- [x] Convert 1.05 tables to our formats (1.05 `attributes1` has 4 extra columns, etc.) *(stab/archery attack folded into phys attack)*
- [x] NPC shops (server + client `shop.txt`, `item.txt`): skill-book shop 5000 now sells the Rogue/Archer/Lancer books (8 Archer/Lancer book items added), weapon/ring/bracelet/necklace shops 1001/1011/1201-1203/6 sell the new classes' low-level gear, plus 1.05 consumables that already existed in our item tables; prices from the emulator. *Still open:* Sorcerer books are not sold in any 1.05 shop; 1.05-only potion packs (391-395, 401-405, ...); shops 61, 63-68/14/6461/6485/6997/7000/7001/8000 have NPCs but no rows; generic item rows (potions, scrolls) list only 5 classes in `RequireClass`, so Lancer/Sorcerer may be unable to use them

## Phase 4: Encoded packages (needed for Sorcerer and later content)
- [x] Recover the 6 substitution tables (data-driven, not the generator): decoder `H:\1.05 Backport assets\decode_pkg\decode_pkg.py`; all 256 files decoded and verified structurally; the Sorcerer files load in the client
- [ ] Decode-on-load support in the engine (`FFileManagerMPQ` / `UnLinker`)
- [x] Scan decoded packages for further missing script classes

## Phase 5: Wider 1.05 content (separate projects)
- [ ] Config/script gap: 162 new config files, 1.05 `.u` scripts vs our source
- [ ] Maps (1,286 Mir2 map files), scenes, map snapshots
- [ ] New skills/FX for existing classes, couture, wings, mounts, monsters
- [ ] New systems (physique, mingwu, legend_power, dragon soul, etc.)

## Housekeeping
- Server: start with `Server\NewServer\bin\start_servers_wt.ps1` (Windows Terminal tabs)
- Client: `Binaries\Win64\MMOGame.exe -sdot -log -dx9 -ip=1,1,127.0.0.1,8701,127.0.0.1,8701,Legend`
- TEMP diagnostics still in the client (remove before committing): `[SetMeshDiag]`/`[MeshDiag]` (`MMOActor.cpp`, `ItemManager.cpp`), `[AtkDiag]` (`FSMIO.cpp`), `[SlotDiag]` (`message_handler_Combat.cpp`)
- Never commit: `MMOEditorUserSettings.ini`, generated headers, shader `.bin`, and the replaced game packages in `Content` (all gitignored, backups are `*.orig-2016`)
