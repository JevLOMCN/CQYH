#include "stdafx.h"
#include "sol_bindings.h"
#include "stdlib.h"
#include "scene_object.h"
#include "scene_object.h"
#include "character.h"
#include "aura_instance.h"
#include "spell_instance.h"
#include "../../../common/define/role_def.h"
#include "role.h"
#include "reputation.h"
#include "../../../inc/gameworld/role_guild_data.h"
#include "../../../inc/gameworld/trade.h"
#include "../../../inc/gameworld/item.h"
#include "../../../../Common/define/grid_def.h"
#include "../../../inc/gameworld/region.h"
#include "../../../inc/gameworld/cooldown.h"
#include "../../../inc/gameworld/ai_processor.h"
#include "../../../inc/gameworld/friend.h"
#include "../../../inc/gameworld/master.h"
#include "../../../inc/gameworld/npc.h"
#include "../../../inc/gameworld/title.h"
#include "../../../../Common/fusion/inc/rune_data.h"
#include "../../../inc/gameworld/spell_book.h"
#include "../../../inc/gameworld/hero.h"
#include "../../../../Common/define/zone_def.h"
#include "../../../inc/gameworld/attr_list.h"
#include "../../../inc/gameworld/spirit.h"
#include "../../../inc/gameworld/scene.h"
#include "../../../inc/gameworld/arena.h"
#include "../../../../Common/define/scene_def.h"
#include "../../../inc/gameworld/asimov_role.h"
#include "../../../inc/gameworld/stall.h"
#include "../../../inc/gameworld/shield_list.h"
#include "../../../inc/gameworld/spell_slot.h"
#include "../../../inc/gameworld/aura.h"
#include "../../../inc/gameworld/achievement_list.h"
#include "../../../inc/gameworld/quest_list.h"
#include "../../../inc/gameworld/item_bag.h"
#include "../../../../Common/define/spell_def.h"
#include "../../../inc/baselib/math_ex.h"
#include "../../../inc/gameworld/target_scanner.h"

int sol_role_open(lua_State* L)
{
    sol::state_view lua(L);
    sol::table globals = lua.globals();
    auto type1 = cqyh::lua::usertype<CSceneObject>(L, globals, "CSceneObject", "CSceneObject");
    type1.set_function("GetObjID", [](CSceneObject* self) -> decltype(auto) { return self->GetObjID(); });
    type1.set_function("GetObjType", [](CSceneObject* self) -> decltype(auto) { return self->GetObjType(); });
    type1.set_function("GetScene", [](CSceneObject* self) -> decltype(auto) { return self->GetScene(); });
    type1.set_function("GetRegion", [](CSceneObject* self) -> decltype(auto) { return self->GetRegion(); });
    type1.set_function("GetZone", [](CSceneObject* self) -> decltype(auto) { return self->GetZone(); });
    type1.set_function("GetGrid", [](CSceneObject* self) -> decltype(auto) { return self->GetGrid(); });
    type1.set_function("GetGridInstance", [](CSceneObject* self) -> decltype(auto) { return self->GetGridInstance(); });
    type1.set_function("SetX", [](CSceneObject* self, INT64 fX) -> decltype(auto) { self->SetX(fX); });
    type1.set_function("SetY", [](CSceneObject* self, INT64 fY) -> decltype(auto) { self->SetY(fY); });
    type1.set_function("SetZ", [](CSceneObject* self, INT64 fZ) -> decltype(auto) { self->SetZ(fZ); });
    type1.set_function("GetX", [](CSceneObject* self) -> decltype(auto) { return self->GetX(); });
    type1.set_function("GetY", [](CSceneObject* self) -> decltype(auto) { return self->GetY(); });
    type1.set_function("GetZ", [](CSceneObject* self) -> decltype(auto) { return self->GetZ(); });
    type1.set_function("SetFaceDirection", [](CSceneObject* self, signed int nFaceDirection) -> decltype(auto) { self->SetFaceDirection(nFaceDirection); });
    type1.set_function("GetFaceDirection", [](CSceneObject* self) -> decltype(auto) { return self->GetFaceDirection(); });
    type1.set_function("SetBodyType", [](CSceneObject* self, signed int nBodyType) -> decltype(auto) { self->SetBodyType(nBodyType); });
    type1.set_function("GetBodyType", [](CSceneObject* self) -> decltype(auto) { return self->GetBodyType(); });
    type1.set_function("CanTeleport", [](CSceneObject* self) -> decltype(auto) { return static_cast<bool>(self->CanTeleport()); });
    type1.set_function("GetObjID", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetObjID(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetObjID(); }));
    type1.set_function("GetObjType", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetObjType(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetObjType(); }));
    type1.set_function("GetScene", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetScene(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetScene(); }));
    type1.set_function("GetRegion", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetRegion(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetRegion(); }));
    type1.set_function("GetZone", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetZone(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetZone(); }));
    type1.set_function("GetGrid", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetGrid(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetGrid(); }));
    type1.set_function("GetGridInstance", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetGridInstance(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetGridInstance(); }));
    type1.set_function("SetX", sol::overload([](CSceneObject* self, INT64 fX) -> decltype(auto) { self->SetX(fX); }, [](CSceneObject* self, INT64 fX) -> decltype(auto) { self->SetX(fX); }));
    type1.set_function("SetY", sol::overload([](CSceneObject* self, INT64 fY) -> decltype(auto) { self->SetY(fY); }, [](CSceneObject* self, INT64 fY) -> decltype(auto) { self->SetY(fY); }));
    type1.set_function("SetZ", sol::overload([](CSceneObject* self, INT64 fZ) -> decltype(auto) { self->SetZ(fZ); }, [](CSceneObject* self, INT64 fZ) -> decltype(auto) { self->SetZ(fZ); }));
    type1.set_function("GetX", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetX(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetX(); }));
    type1.set_function("GetY", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetY(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetY(); }));
    type1.set_function("GetZ", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetZ(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetZ(); }));
    type1.set_function("SetFaceDirection", sol::overload([](CSceneObject* self, signed int nFaceDirection) -> decltype(auto) { self->SetFaceDirection(nFaceDirection); }, [](CSceneObject* self, signed int nFaceDirection) -> decltype(auto) { self->SetFaceDirection(nFaceDirection); }));
    type1.set_function("GetFaceDirection", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetFaceDirection(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetFaceDirection(); }));
    type1.set_function("SetBodyType", sol::overload([](CSceneObject* self, signed int nBodyType) -> decltype(auto) { self->SetBodyType(nBodyType); }, [](CSceneObject* self, signed int nBodyType) -> decltype(auto) { self->SetBodyType(nBodyType); }));
    type1.set_function("GetBodyType", sol::overload([](CSceneObject* self) -> decltype(auto) { return self->GetBodyType(); }, [](CSceneObject* self) -> decltype(auto) { return self->GetBodyType(); }));
    type1.set_function("CanTeleport", sol::overload([](CSceneObject* self) -> decltype(auto) { return static_cast<bool>(self->CanTeleport()); }, [](CSceneObject* self) -> decltype(auto) { return static_cast<bool>(self->CanTeleport()); }));
    auto type2 = cqyh::lua::usertype<CAuraInstance>(L, globals, "CAuraInstance", "CAuraInstance", sol::base_classes, sol::bases<CAura>());
    type2.set_function("GetInstanceID", [](CAuraInstance* self) -> decltype(auto) { return self->GetInstanceID(); });
    auto type3 = cqyh::lua::usertype<CCoolDown>(L, globals, "CCoolDown", "CCoolDown");
    type3.set_function("Add", [](CCoolDown* self, signed int nKey, signed int nInterval) -> decltype(auto) { return static_cast<bool>(self->Add(nKey,nInterval)); });
    type3.set_function("Reset", [](CCoolDown* self, signed int nKey) -> decltype(auto) { self->Reset(nKey); });
    type3.set_function("CheckReady", [](CCoolDown* self, signed int nKey) -> decltype(auto) { return static_cast<bool>(self->CheckReady(nKey)); });
    type3.set_function("GetRemainFrm", [](CCoolDown* self, signed int nKey) -> decltype(auto) { return self->GetRemainFrm(nKey); });
    type3.set_function("Enable", [](CCoolDown* self, bool bEnabled) -> decltype(auto) { self->Enable(bEnabled); });
    auto type4 = cqyh::lua::usertype<CTargetScanner>(L, globals, "CTargetScanner", "CTargetScanner");
    type4.set_function("new", [](sol::table) -> decltype(auto) { return new CTargetScanner(); });
    type4.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<CTargetScanner>(new CTargetScanner()); });
    type4.set_function("delete", [](CTargetScanner* self) -> decltype(auto) { delete self; });
    type4.set_function("GetTargetX", [](CTargetScanner* self) -> decltype(auto) { return self->GetTargetX(); });
    type4.set_function("GetTargetY", [](CTargetScanner* self) -> decltype(auto) { return self->GetTargetY(); });
    type4.set_function("GetTargetZ", [](CTargetScanner* self) -> decltype(auto) { return self->GetTargetZ(); });
    auto type5 = cqyh::lua::usertype<CSpellInstance>(L, globals, "CSpellInstance", "CSpellInstance");
    type5.set_function("new", [](sol::table) -> decltype(auto) { return new CSpellInstance(); });
    type5.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<CSpellInstance>(new CSpellInstance()); });
    type5.set_function("delete", [](CSpellInstance* self) -> decltype(auto) { delete self; });
    type5.set_function("GetScanner", [](CSpellInstance* self) -> decltype(auto) { return self->GetScanner(); });
    auto type6 = cqyh::lua::usertype<CCharacter>(L, globals, "CCharacter", "CCharacter", sol::base_classes, sol::bases<CSceneObject>());
    type6.set_function("JumpTo", [](CCharacter* self, INT64 llX, INT64 llY, INT64 llZ, signed int nFaceDirection) -> decltype(auto) { return static_cast<bool>(self->JumpTo(llX,llY,llZ,nFaceDirection)); });
    type6.set_function("GetClass", [](CCharacter* self) -> decltype(auto) { return self->GetClass(); });
    type6.set_function("GetSex", [](CCharacter* self) -> decltype(auto) { return self->GetSex(); });
    type6.set_function("GetLevel", [](CCharacter* self) -> decltype(auto) { return self->GetLevel(); });
    type6.set_function("GetName", [](CCharacter* self) -> decltype(auto) { return self->GetName(); });
    type6.set_function("SetClass", [](CCharacter* self, signed int nClass) -> decltype(auto) { self->SetClass(nClass); });
    type6.set_function("SetSex", [](CCharacter* self, signed int nSex) -> decltype(auto) { self->SetSex(nSex); });
    type6.set_function("SetLevel", [](CCharacter* self, signed int nLevel) -> decltype(auto) { return static_cast<bool>(self->SetLevel(nLevel)); });
    type6.set_function("GetAttr", [](CCharacter* self, signed int eAttrType) -> decltype(auto) { return self->GetAttr(eAttrType); });
    type6.set_function("GetAttrList", [](CCharacter* self) -> decltype(auto) { return self->GetAttrList(); });
    type6.set_function("SetInCutScene", [](CCharacter* self, bool bInCutScene, signed int nDurationFrm) -> decltype(auto) { self->SetInCutScene(bInCutScene,nDurationFrm); });
    type6.set_function("MoveTo", [](CCharacter* self, signed int nDestX, signed int nDestY) -> decltype(auto) { return static_cast<bool>(self->MoveTo(nDestX,nDestY)); });
    type6.set_function("Die", [](CCharacter* self, signed int nParam) -> decltype(auto) { return static_cast<bool>(self->Die(nParam)); });
    type6.set_function("Revive", [](CCharacter* self, signed int nParam, signed int nHpPercent) -> decltype(auto) { return static_cast<bool>(self->Revive(nParam,nHpPercent)); });
    type6.set_function("RotateTo", [](CCharacter* self, signed int nFaceDirection, signed int nDurationFrm) -> decltype(auto) { self->RotateTo(nFaceDirection,nDurationFrm); });
    type6.set_function("SetCanRun", [](CCharacter* self, bool bCanRun) -> decltype(auto) { self->SetCanRun(bCanRun); });
    type6.set_function("GetMoveState", [](CCharacter* self) -> decltype(auto) { return self->GetMoveState(); });
    type6.set_function("SetHome", [](CCharacter* self, signed int nSceneID, signed int nHomeX, signed int nHomeY, signed int nHomeZ) -> decltype(auto) { self->SetHome(nSceneID,nHomeX,nHomeY,nHomeZ); });
    type6.set_function("GetHomeX", [](CCharacter* self) -> decltype(auto) { return self->GetHomeX(); });
    type6.set_function("GetHomeY", [](CCharacter* self) -> decltype(auto) { return self->GetHomeY(); });
    type6.set_function("GetHomeZ", [](CCharacter* self) -> decltype(auto) { return self->GetHomeZ(); });
    type6.set_function("CancelCast", [](CCharacter* self) -> decltype(auto) { self->CancelCast(); });
    type6.set_function("IsCasting", [](CCharacter* self) -> decltype(auto) { return static_cast<bool>(self->IsCasting()); });
    type6.set_function("EquipRune", [](CCharacter* self, const RUNE_DATA* pRuneData, Item* poItem) -> decltype(auto) { return self->EquipRune(pRuneData,poItem); });
    type6.set_function("UnEquipRune", [](CCharacter* self, const RUNE_DATA* pRuneData, Item* poItem) -> decltype(auto) { return static_cast<bool>(self->UnEquipRune(pRuneData,poItem)); });
    type6.set_function("GetSpellSlot", [](CCharacter* self, signed int nIndex) -> decltype(auto) { return self->GetSpellSlot(nIndex); });
    type6.set_function("CastSpell", [](CCharacter* self, signed int nSpellID, CCharacter* poTarget, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ) -> decltype(auto) { return self->CastSpell(nSpellID,poTarget,nTargetX,nTargetY,nTargetZ); });
    type6.set_function("PlayAction", [](CCharacter* self, unsigned int dwActionID) -> decltype(auto) { return static_cast<bool>(self->PlayAction(dwActionID)); });
    type6.set_function("GetCastingSpell", [](CCharacter* self) -> decltype(auto) { return self->GetCastingSpell(); });
    type6.set_function("GetHp", [](CCharacter* self) -> decltype(auto) { return self->GetHp(); });
    type6.set_function("SetHp", [](CCharacter* self, signed int nHp) -> decltype(auto) { self->SetHp(nHp); });
    type6.set_function("ChangeHp", [](CCharacter* self, signed int nValue, signed int nReason) -> decltype(auto) { self->ChangeHp(nValue,nReason); });
    type6.set_function("GetMp", [](CCharacter* self) -> decltype(auto) { return self->GetMp(); });
    type6.set_function("ChangeMp", [](CCharacter* self, signed int nValue) -> decltype(auto) { self->ChangeMp(nValue); });
    type6.set_function("CanAttack", [](CCharacter* self, CCharacter* poChar) -> decltype(auto) { return static_cast<bool>(self->CanAttack(poChar)); });
    type6.set_function("GetSpellBook", [](CCharacter* self) -> decltype(auto) { return self->GetSpellBook(); });
    type6.set_function("AppendAura", [](CCharacter* self, CAura* pAuraInfo, CCharacter* poCaster, unsigned int dwFromObjID, ExtraParam* pExParam, signed int nNewDurationFrm, signed int nNewStackCount, signed int nSpellID, signed int nSpellLevel) -> decltype(auto) { return self->AppendAura(pAuraInfo,poCaster,dwFromObjID,pExParam,nNewDurationFrm,nNewStackCount,nSpellID,nSpellLevel); });
    type6.set_function("GetFirstAura", [](CCharacter* self, signed int nAuraID) -> decltype(auto) { return self->GetFirstAura(nAuraID); });
    type6.set_function("RemoveAura", [](CCharacter* self, CAuraInstance* pAura, bool bTrigOnRemove) -> decltype(auto) { self->RemoveAura(pAura,bTrigOnRemove); });
    type6.set_function("RemoveAuraByType", [](CCharacter* self, signed int nType) -> decltype(auto) { return static_cast<bool>(self->RemoveAuraByType(nType)); });
    type6.set_function("RemoveAuraBySubType", [](CCharacter* self, signed int nSubType) -> decltype(auto) { return static_cast<bool>(self->RemoveAuraBySubType(nSubType)); });
    type6.set_function("IsDead", [](CCharacter* self) -> decltype(auto) { return static_cast<bool>(self->IsDead()); });
    type6.set_function("GetFlag", [](CCharacter* self) -> decltype(auto) { return self->GetFlag(); });
    type6.set_function("GetExFlag", [](CCharacter* self) -> decltype(auto) { return self->GetExFlag(); });
    type6.set_function("GetCCFlag", [](CCharacter* self) -> decltype(auto) { return self->GetCCFlag(); });
    type6.set_function("IncControlCount", [](CCharacter* self, signed int nControlType) -> decltype(auto) { return static_cast<bool>(self->IncControlCount(nControlType)); });
    type6.set_function("DecControlCount", [](CCharacter* self, signed int nControlType) -> decltype(auto) { return static_cast<bool>(self->DecControlCount(nControlType)); });
    type6.set_function("GetControlCount", [](CCharacter* self, signed int nControlType) -> decltype(auto) { return self->GetControlCount(nControlType); });
    type6.set_function("SetLastCasterID", [](CCharacter* self, unsigned int dwID) -> decltype(auto) { self->SetLastCasterID(dwID); });
    type6.set_function("GetLastCasterID", [](CCharacter* self) -> decltype(auto) { return self->GetLastCasterID(); });
    type6.set_function("GetLastCasterTimer", [](CCharacter* self) -> decltype(auto) { return self->GetLastCasterTimer(); });
    type6.set_function("SetLastTargetID", [](CCharacter* self, unsigned int dwObjID) -> decltype(auto) { self->SetLastTargetID(dwObjID); });
    type6.set_function("GetLastTargetID", [](CCharacter* self) -> decltype(auto) { return self->GetLastTargetID(); });
    type6.set_function("GetLastTargetTimer", [](CCharacter* self) -> decltype(auto) { return self->GetLastTargetTimer(); });
    type6.set_function("SetDropOwnerID", [](CCharacter* self, unsigned int dwID) -> decltype(auto) { self->SetDropOwnerID(dwID); });
    type6.set_function("GetDropOwnerID", [](CCharacter* self) -> decltype(auto) { return self->GetDropOwnerID(); });
    type6.set_function("SelectFocus", [](CCharacter* self, unsigned int dwTargetID) -> decltype(auto) { self->SelectFocus(dwTargetID); });
    type6.set_function("GetFocusID", [](CCharacter* self) -> decltype(auto) { return self->GetFocusID(); });
    type6.set_function("Say", [](CCharacter* self, unsigned int dwTargetID, unsigned int dwLocation, const char* pcszMessage) -> decltype(auto) { return self->Say(dwTargetID,dwLocation,pcszMessage); });
    type6.set_function("SendMail", [](CCharacter* self, unsigned int dwRoleID, signed int nMailTemplateID) -> decltype(auto) { return static_cast<bool>(self->SendMail(dwRoleID,nMailTemplateID)); });
    type6.set_function("SendMail", sol::overload([](CCharacter* self, unsigned int dwRoleID, signed int nMailTemplateID, signed int nItemID, signed int nItemCount) -> decltype(auto) { return static_cast<bool>(self->SendMail(dwRoleID,nMailTemplateID,nItemID,nItemCount)); }, [](CCharacter* self, unsigned int dwRoleID, signed int nMailTemplateID) -> decltype(auto) { return static_cast<bool>(self->SendMail(dwRoleID,nMailTemplateID)); }));
    type6.set_function("SetAIProcessor", [](CCharacter* self, unsigned int nAIID) -> decltype(auto) { return static_cast<bool>(self->SetAIProcessor(nAIID)); });
    type6.set_function("GetAIProcessor", [](CCharacter* self) -> decltype(auto) { return self->GetAIProcessor(); });
    type6.set_function("GetOriginalAiID", [](CCharacter* self) -> decltype(auto) { return self->GetOriginalAiID(); });
    type6.set_function("SetGlobalCDDelayFrame", [](CCharacter* self, signed int nFrame) -> decltype(auto) { self->SetGlobalCDDelayFrame(nFrame); });
    type6.set_function("GetGlobalCDDelayFrame", [](CCharacter* self) -> decltype(auto) { return self->GetGlobalCDDelayFrame(); });
    type6.set_function("GetCoolDown", [](CCharacter* self) -> decltype(auto) { return self->GetCoolDown(); });
    type6.set_function("GetHostID", [](CCharacter* self) -> decltype(auto) { return self->GetHostID(); });
    type6.set_function("Mine", [](CCharacter* self, INT64 llDestX, INT64 llDestY) -> decltype(auto) { return static_cast<bool>(self->Mine(llDestX,llDestY)); });
    auto type7 = cqyh::lua::usertype<CFlagBits>(L, globals, "CFlagBits", "CFlagBits");
    type7.set_function("new", [](sol::table) -> decltype(auto) { return new CFlagBits(); });
    type7.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<CFlagBits>(new CFlagBits()); });
    type7.set_function("CheckFlag", [](CFlagBits* self, unsigned int eBit) -> decltype(auto) { return static_cast<bool>(self->CheckFlag(eBit)); });
    type7.set_function("SetFlag", [](CFlagBits* self, unsigned int eBit) -> decltype(auto) { self->SetFlag(eBit); });
    type7.set_function("ClearFlag", [](CFlagBits* self, unsigned int eBit) -> decltype(auto) { self->ClearFlag(eBit); });
    type7.set_function("ResetFlag", [](CFlagBits* self) -> decltype(auto) { self->ResetFlag(); });
    type7.set_function("FlagIncludeBits", [](CFlagBits* self, unsigned int dwMask) -> decltype(auto) { return static_cast<bool>(self->FlagIncludeBits(dwMask)); });
    type7.set_function("FlagNotIncludeBits", [](CFlagBits* self, unsigned int dwMask) -> decltype(auto) { return static_cast<bool>(self->FlagNotIncludeBits(dwMask)); });
    type7.set_function("GetFlag", [](CFlagBits* self) -> decltype(auto) { return self->GetFlag(); });
    globals["MAX_ROLE_FRACTION_ID"] = static_cast<lua_Number>(MAX_ROLE_FRACTION_ID);
    globals["MAX_ROLE_LEVEL"] = static_cast<lua_Number>(MAX_ROLE_LEVEL);
    globals["MAX_PK_VALUE"] = static_cast<lua_Number>(MAX_PK_VALUE);
    globals["MIN_REPUTATION"] = static_cast<lua_Number>(MIN_REPUTATION);
    globals["MAX_ACTIVITY_LEVEL"] = static_cast<lua_Number>(MAX_ACTIVITY_LEVEL);
    globals["MAX_USABLE_ROLE_PER_USER"] = static_cast<lua_Number>(MAX_USABLE_ROLE_PER_USER);
    globals["MAX_DELETE_ROLE_PER_USER"] = static_cast<lua_Number>(MAX_DELETE_ROLE_PER_USER);
    globals["MAX_ROLE_PER_USER"] = static_cast<lua_Number>(MAX_ROLE_PER_USER);
    globals["MAX_ROLE_HAIR_TYPE"] = static_cast<lua_Number>(MAX_ROLE_HAIR_TYPE);
    globals["MAX_ROLE_HAIR_COLOR"] = static_cast<lua_Number>(MAX_ROLE_HAIR_COLOR);
    globals["MAX_ROLE_FACE_TYPE"] = static_cast<lua_Number>(MAX_ROLE_FACE_TYPE);
    globals["MAX_ROLE_SKIN_COLOR"] = static_cast<lua_Number>(MAX_ROLE_SKIN_COLOR);
    globals["MOMENTARY_KILL_NPC_TIEM"] = static_cast<lua_Number>(MOMENTARY_KILL_NPC_TIEM);
    globals["MIN_MOMENTARY_KILL_NPC_COUNT"] = static_cast<lua_Number>(MIN_MOMENTARY_KILL_NPC_COUNT);
    globals["MIN_CONSECUTIVE_KILL_NPC_COUNT"] = static_cast<lua_Number>(MIN_CONSECUTIVE_KILL_NPC_COUNT);
    globals["MAX_CONSECUTIVE_KILL_NPC_FRAME"] = static_cast<lua_Number>(MAX_CONSECUTIVE_KILL_NPC_FRAME);
    globals["MAX_CONSECTIVE_KILL_PLAYER_FRAME"] = static_cast<lua_Number>(MAX_CONSECTIVE_KILL_PLAYER_FRAME);
    globals["MAX_CONSECUTIVE_KILL_PLAYER_COUNT"] = static_cast<lua_Number>(MAX_CONSECUTIVE_KILL_PLAYER_COUNT);
    globals["MAX_QUERY_ROLE_INFO_PER_SEC"] = static_cast<lua_Number>(MAX_QUERY_ROLE_INFO_PER_SEC);
    globals["MAX_VIP_MONTH"] = static_cast<lua_Number>(MAX_VIP_MONTH);
    globals["MAX_GM_LEVEL"] = static_cast<lua_Number>(MAX_GM_LEVEL);
    globals["MAX_PET_PER_ROLE"] = static_cast<lua_Number>(MAX_PET_PER_ROLE);
    globals["MAX_REPUTATION_AWARD_COUNT"] = static_cast<lua_Number>(MAX_REPUTATION_AWARD_COUNT);
    globals["ROLE_DELAY_LOGOUT_FRAME"] = static_cast<lua_Number>(ROLE_DELAY_LOGOUT_FRAME);
    globals["ROLE_DELAY_LOGOUT_FRAME_ABNORMAL"] = static_cast<lua_Number>(ROLE_DELAY_LOGOUT_FRAME_ABNORMAL);
    globals["ROLE_DELAY_LOGOUT_FRAME"] = static_cast<lua_Number>(ROLE_DELAY_LOGOUT_FRAME);
    globals["ROLE_DELAY_LOGOUT_FRAME_ABNORMAL"] = static_cast<lua_Number>(ROLE_DELAY_LOGOUT_FRAME_ABNORMAL);
    globals["DEFAULT_ROLE_ANTI_CHEAT_LEVEL"] = static_cast<lua_Number>(DEFAULT_ROLE_ANTI_CHEAT_LEVEL);
    globals["MAX_ROLE_ANTI_CHEAT_LEVEL"] = static_cast<lua_Number>(MAX_ROLE_ANTI_CHEAT_LEVEL);
    globals["MAX_ROLE_ANTI_CHEAT_PUNISH_LEVEL"] = static_cast<lua_Number>(MAX_ROLE_ANTI_CHEAT_PUNISH_LEVEL);
    globals["MAX_ROLE_GPK_PUNISH_LEVEL"] = static_cast<lua_Number>(MAX_ROLE_GPK_PUNISH_LEVEL);
    globals["rltInvalid"] = static_cast<lua_Number>(rltInvalid);
    globals["rltReturnLogin"] = static_cast<lua_Number>(rltReturnLogin);
    globals["rltReturnSelectRole"] = static_cast<lua_Number>(rltReturnSelectRole);
    globals["rltDisconnect"] = static_cast<lua_Number>(rltDisconnect);
    globals["rltKickout"] = static_cast<lua_Number>(rltKickout);
    globals["rltTotal"] = static_cast<lua_Number>(rltTotal);
    globals["rsInvalid"] = static_cast<lua_Number>(rsInvalid);
    globals["rsPreEntered"] = static_cast<lua_Number>(rsPreEntered);
    globals["rsWaitRoleData"] = static_cast<lua_Number>(rsWaitRoleData);
    globals["rsWaitMSApprove"] = static_cast<lua_Number>(rsWaitMSApprove);
    globals["rsWaitClientLoad"] = static_cast<lua_Number>(rsWaitClientLoad);
    globals["rsPlaying"] = static_cast<lua_Number>(rsPlaying);
    globals["rsInJump"] = static_cast<lua_Number>(rsInJump);
    globals["rsJumpInGSWaitClientLoad"] = static_cast<lua_Number>(rsJumpInGSWaitClientLoad);
    globals["rsWaitUnlockRoleData"] = static_cast<lua_Number>(rsWaitUnlockRoleData);
    globals["rsLogout"] = static_cast<lua_Number>(rsLogout);
    globals["rsTotal"] = static_cast<lua_Number>(rsTotal);
    globals["fcmsFull"] = static_cast<lua_Number>(fcmsFull);
    globals["fcmsHalf"] = static_cast<lua_Number>(fcmsHalf);
    globals["fcmsNone"] = static_cast<lua_Number>(fcmsNone);
    globals["fcmsTotal"] = static_cast<lua_Number>(fcmsTotal);
    globals["faceInvalid"] = static_cast<lua_Number>(faceInvalid);
    globals["faceTotal"] = static_cast<lua_Number>(faceTotal);
    globals["rpmInvalid"] = static_cast<lua_Number>(rpmInvalid);
    globals["rpmPeace"] = static_cast<lua_Number>(rpmPeace);
    globals["rpmGuild"] = static_cast<lua_Number>(rpmGuild);
    globals["rpmTeam"] = static_cast<lua_Number>(rpmTeam);
    globals["rpmSlayer"] = static_cast<lua_Number>(rpmSlayer);
    globals["rpmJustice"] = static_cast<lua_Number>(rpmJustice);
    globals["rpmMaster"] = static_cast<lua_Number>(rpmMaster);
    globals["rpmTotal"] = static_cast<lua_Number>(rpmTotal);
    globals["rprInvalid"] = static_cast<lua_Number>(rprInvalid);
    globals["rprNormal"] = static_cast<lua_Number>(rprNormal);
    globals["rprRedName"] = static_cast<lua_Number>(rprRedName);
    globals["rprCounterStrike"] = static_cast<lua_Number>(rprCounterStrike);
    globals["rprOpponentGuildMember"] = static_cast<lua_Number>(rprOpponentGuildMember);
    globals["rprLuckyKill"] = static_cast<lua_Number>(rprLuckyKill);
    globals["rprNoPunishment"] = static_cast<lua_Number>(rprNoPunishment);
    globals["rprTotal"] = static_cast<lua_Number>(rprTotal);
    globals["rncInvalid"] = static_cast<lua_Number>(rncInvalid);
    globals["rncWhite"] = static_cast<lua_Number>(rncWhite);
    globals["rncYellow"] = static_cast<lua_Number>(rncYellow);
    globals["rncRed"] = static_cast<lua_Number>(rncRed);
    globals["rncTotal"] = static_cast<lua_Number>(rncTotal);
    globals["ervtInvalid"] = static_cast<lua_Number>(ervtInvalid);
    globals["ervtTotal"] = static_cast<lua_Number>(ervtTotal);
    globals["rfrInvalid"] = static_cast<lua_Number>(rfrInvalid);
    globals["rfrDefault"] = static_cast<lua_Number>(rfrDefault);
    globals["rfrFriend"] = static_cast<lua_Number>(rfrFriend);
    globals["rfrEnemy"] = static_cast<lua_Number>(rfrEnemy);
    globals["rfrTotal"] = static_cast<lua_Number>(rfrTotal);
    globals["vlLevel0"] = static_cast<lua_Number>(vlLevel0);
    globals["vlLevel1"] = static_cast<lua_Number>(vlLevel1);
    globals["vlLevel2"] = static_cast<lua_Number>(vlLevel2);
    globals["vlLevel3"] = static_cast<lua_Number>(vlLevel3);
    globals["vlTotal"] = static_cast<lua_Number>(vlTotal);
    globals["MSG_PP_PERSONAL_ENEMY"] = static_cast<lua_Number>(MSG_PP_PERSONAL_ENEMY);
    globals["aprInvalid"] = static_cast<lua_Number>(aprInvalid);
    globals["aprWuDouChang"] = static_cast<lua_Number>(aprWuDouChang);
    globals["aprBaoku"] = static_cast<lua_Number>(aprBaoku);
    globals["aprYanWuChang"] = static_cast<lua_Number>(aprYanWuChang);
    globals["aprPray"] = static_cast<lua_Number>(aprPray);
    globals["aprEscort"] = static_cast<lua_Number>(aprEscort);
    globals["aprGuildBoss"] = static_cast<lua_Number>(aprGuildBoss);
    globals["aprSpring"] = static_cast<lua_Number>(aprSpring);
    globals["aprQuest"] = static_cast<lua_Number>(aprQuest);
    globals["aprYaoTa"] = static_cast<lua_Number>(aprYaoTa);
    globals["aprYouMingSea"] = static_cast<lua_Number>(aprYouMingSea);
    globals["aprTotal"] = static_cast<lua_Number>(aprTotal);
    auto type8 = cqyh::lua::usertype<CSpellSlot>(L, globals, "CSpellSlot", "CSpellSlot");
    type8.set_function("new", [](sol::table) -> decltype(auto) { return new CSpellSlot(); });
    type8.set_function("new_local", [](sol::table) -> decltype(auto) { return std::unique_ptr<CSpellSlot>(new CSpellSlot()); });
    type8.set_function("delete", [](CSpellSlot* self) -> decltype(auto) { delete self; });
    type8.set_function("GetMainSpellID", [](CSpellSlot* self) -> decltype(auto) { return self->GetMainSpellID(); });
    auto type9 = cqyh::lua::usertype<CRole>(L, globals, "CRole", "CRole", sol::base_classes, sol::bases<CCharacter, CSceneObject>());
    type9.set_function("Init", [](CRole* self, unsigned int dwObjID, unsigned int dwSessionID) -> decltype(auto) { return static_cast<bool>(self->Init(dwObjID,dwSessionID)); });
    type9.set_function("UnInit", [](CRole* self) -> decltype(auto) { return static_cast<bool>(self->UnInit()); });
    type9.set_function("GetName", [](CRole* self) -> decltype(auto) { return self->GetName(); });
    type9.set_function("GetSessionID", [](CRole* self) -> decltype(auto) { return self->GetSessionID(); });
    type9.set_function("GetServerGroup", [](CRole* self) -> decltype(auto) { return self->GetServerGroup(); });
    type9.set_function("GetSceneTempID", [](CRole* self) -> decltype(auto) { return self->GetSceneTempID(); });
    type9.set_function("GetSceneID", [](CRole* self) -> decltype(auto) { return self->GetSceneID(); });
    type9.set_function("LearnSpell", [](CRole* self, signed int nSpellID, signed int nSpellLevel, bool bNeedSave) -> decltype(auto) { return static_cast<bool>(self->LearnSpell(nSpellID,nSpellLevel,bNeedSave)); });
    type9.set_function("EquipSpell", [](CRole* self, signed int nSlotID, signed int nSpellID) -> decltype(auto) { return self->EquipSpell(nSlotID,nSpellID); });
    type9.set_function("UnlockSpellSlot", [](CRole* self, signed int nSlotID) -> decltype(auto) { return static_cast<bool>(self->UnlockSpellSlot(nSlotID)); });
    type9.set_function("GetSpellSlot", [](CRole* self, signed int nSlotID) -> decltype(auto) { return self->GetSpellSlot(nSlotID); });
    type9.set_function("AddSpellPts", [](CRole* self, signed int nSpellID, signed int nPts) -> decltype(auto) { return static_cast<bool>(self->AddSpellPts(nSpellID,nPts)); });
    type9.set_function("RemoveSpell", [](CRole* self, signed int nSpellID) -> decltype(auto) { return static_cast<bool>(self->RemoveSpell(nSpellID)); });
    type9.set_function("EquipRune", [](CRole* self, signed int nSpell, signed int nRuneSlot) -> decltype(auto) { return static_cast<bool>(self->EquipRune(nSpell,nRuneSlot)); });
    type9.set_function("SetLastSlot", [](CRole* self, signed int nSlotID) -> decltype(auto) { self->SetLastSlot(nSlotID); });
    type9.set_function("GetLastSlot", [](CRole* self) -> decltype(auto) { return self->GetLastSlot(); });
    type9.set_function("ClearAllCooldown", [](CRole* self) -> decltype(auto) { self->ClearAllCooldown(); });
    type9.set_function("GetItemBag", [](CRole* self) -> decltype(auto) { return self->GetItemBag(); });
    type9.set_function("GetStall", [](CRole* self) -> decltype(auto) { return self->GetStall(); });
    type9.set_function("GetQuestList", [](CRole* self) -> decltype(auto) { return self->GetQuestList(); });
    type9.set_function("SetFreeMode", [](CRole* self, unsigned char byFreeMode) -> decltype(auto) { self->SetFreeMode(byFreeMode); });
    type9.set_function("IsFreeMode", [](CRole* self) -> decltype(auto) { return static_cast<bool>(self->IsFreeMode()); });
    type9.set_function("GetFaction", [](CRole* self) -> decltype(auto) { return self->GetFaction(); });
    type9.set_function("SetFaction", [](CRole* self, signed int nFaction) -> decltype(auto) { self->SetFaction(nFaction); });
    type9.set_function("GetReputation", [](CRole* self) -> decltype(auto) { return self->GetReputation(); });
    type9.set_function("GetTeamID", [](CRole* self) -> decltype(auto) { return self->GetTeamID(); });
    type9.set_function("GetTrade", [](CRole* self) -> decltype(auto) { return self->GetTrade(); });
    type9.set_function("GetTitle", [](CRole* self) -> decltype(auto) { return self->GetTitle(); });
    type9.set_function("GetMaster", [](CRole* self) -> decltype(auto) { return self->GetMaster(); });
    type9.set_function("GetGuildID", [](CRole* self) -> decltype(auto) { return self->GetGuildID(); });
    type9.set_function("GetRoleGuildData", [](CRole* self) -> decltype(auto) { return self->GetRoleGuildData(); });
    type9.set_function("GetAsimovRole", [](CRole* self) -> decltype(auto) { return self->GetAsimovRole(); });
    type9.set_function("GetFriend", [](CRole* self) -> decltype(auto) { return self->GetFriend(); });
    type9.set_function("GetShieldList", [](CRole* self) -> decltype(auto) { return self->GetShieldList(); });
    type9.set_function("GetArena", [](CRole* self) -> decltype(auto) { return self->GetArena(); });
    type9.set_function("GetHero", [](CRole* self) -> decltype(auto) { return self->GetHero(); });
    type9.set_function("AddExp", [](CRole* self, signed int nExp, bool bByKillNpc, bool byDailyLimitExp) -> decltype(auto) { self->AddExp(nExp,bByKillNpc,byDailyLimitExp); });
    type9.set_function("GetExp", [](CRole* self) -> decltype(auto) { return self->GetExp(); });
    type9.set_function("GetExtraExp", [](CRole* self) -> decltype(auto) { return self->GetExtraExp(); });
    type9.set_function("AddExtraExp", [](CRole* self, signed int nExp) -> decltype(auto) { self->AddExtraExp(nExp); });
    type9.set_function("TryRevive", [](CRole* self, signed int nType) -> decltype(auto) { return self->TryRevive(nType); });
    type9.set_function("ActivateMount", [](CRole* self, signed int nMountID) -> decltype(auto) { return self->ActivateMount(nMountID); });
    type9.set_function("OnMount", [](CRole* self, signed int nMountID) -> decltype(auto) { return static_cast<bool>(self->OnMount(nMountID)); });
    type9.set_function("OffMount", [](CRole* self) -> decltype(auto) { self->OffMount(); });
    type9.set_function("EquipWing", [](CRole* self, signed int nWingID) -> decltype(auto) { return self->EquipWing(nWingID); });
    type9.set_function("UnequipWing", [](CRole* self) -> decltype(auto) { self->UnequipWing(); });
    type9.set_function("GetCurrentWingID", [](CRole* self) -> decltype(auto) { return self->GetCurrentWingID(); });
    type9.set_function("GrantWing", [](CRole* self, signed int nWingID) -> decltype(auto) { return static_cast<bool>(self->GrantWing(nWingID)); });
    type9.set_function("HasWing", [](CRole* self, signed int nWingID) -> decltype(auto) { return static_cast<bool>(self->HasWing(nWingID)); });
    type9.set_function("GetWingLevel", [](CRole* self) -> decltype(auto) { return self->GetWingLevel(); });
    type9.set_function("SyncWingInfo", [](CRole* self) -> decltype(auto) { self->SyncWingInfo(false); });
    type9.set_function("GetVIPLevel", [](CRole* self) -> decltype(auto) { return self->GetVIPLevel(); });
    type9.set_function("SubscribeVIP", [](CRole* self, signed int nVIPLevel, unsigned int dwMonth) -> decltype(auto) { return self->SubscribeVIP(nVIPLevel,dwMonth); });
    type9.set_function("GetVIPTime", [](CRole* self, signed int nVIPLevel) -> decltype(auto) { return self->GetVIPTime(nVIPLevel); });
    type9.set_function("GetTotalOnlineTime", [](CRole* self) -> decltype(auto) { return self->GetTotalOnlineTime(); });
    type9.set_function("GetLevelOnlineTime", [](CRole* self) -> decltype(auto) { return self->GetLevelOnlineTime(); });
    type9.set_function("ChangePkMode", [](CRole* self, signed int nMode) -> decltype(auto) { self->ChangePkMode(nMode); });
    type9.set_function("IsRedName", [](CRole* self) -> decltype(auto) { return static_cast<bool>(self->IsRedName()); });
    type9.set_function("GetState", [](CRole* self) -> decltype(auto) { return self->GetState(); });
    type9.set_function("SetVariable", [](CRole* self, signed int nIndex, signed int nValue) -> decltype(auto) { self->SetVariable(nIndex,nValue); });
    type9.set_function("GetVariable", [](CRole* self, signed int nIndex) -> decltype(auto) { return self->GetVariable(nIndex); });
    type9.set_function("GetClientVariable", [](CRole* self, signed int nIndex) -> decltype(auto) { return self->GetClientVariable(nIndex); });
    type9.set_function("SetClientVariable", [](CRole* self, signed int nIndex, signed int nValue) -> decltype(auto) { return static_cast<bool>(self->SetClientVariable(nIndex,nValue)); });
    type9.set_function("GetPkValue", [](CRole* self) -> decltype(auto) { return self->GetPkValue(); });
    type9.set_function("SetPkValue", [](CRole* self, unsigned int dwPkValue) -> decltype(auto) { self->SetPkValue(dwPkValue); });
    type9.set_function("GetAchievementList", [](CRole* self) -> decltype(auto) { return self->GetAchievementList(); });
    type9.set_function("HireHero", [](CRole* self, signed int nLevel, signed int nClass, signed int nSex, unsigned int dwHeroExp, signed int eAiType, bool bFirstTime) -> decltype(auto) { return self->HireHero(nLevel,nClass,nSex,dwHeroExp,eAiType,bFirstTime); });
    type9.set_function("FireHero", [](CRole* self) -> decltype(auto) { return self->FireHero(); });
    type9.set_function("SummonHero", [](CRole* self) -> decltype(auto) { return self->SummonHero(); });
    type9.set_function("RecallHero", [](CRole* self, bool bNotify) -> decltype(auto) { return self->RecallHero(bNotify); });
    type9.set_function("GetSpirit", [](CRole* self) -> decltype(auto) { return self->GetSpirit(); });
    type9.set_function("CheckDistance", [](CRole* self, CSceneObject* poSceneObject) -> decltype(auto) { return static_cast<bool>(self->CheckDistance(poSceneObject)); });
    type9.set_function("GetInteractNpc", [](CRole* self) -> decltype(auto) { return self->GetInteractNpc(); });
    type9.set_function("Interact", [](CRole* self, CNpc* poNpc) -> decltype(auto) { return static_cast<bool>(self->Interact(poNpc)); });
    type9.set_function("AddActivity", [](CRole* self, signed int nActivity) -> decltype(auto) { self->AddActivity(nActivity); });
    type9.set_function("GetActivity", [](CRole* self) -> decltype(auto) { return self->GetActivity(); });
    type9.set_function("AddActivityReward", [](CRole* self, signed int nActivityRewardIndex) -> decltype(auto) { self->AddActivityReward(nActivityRewardIndex); });
    type9.set_function("GetActivityRewardByIndex", [](CRole* self, signed int nActivityRewardIndex) -> decltype(auto) { return static_cast<bool>(self->GetActivityRewardByIndex(nActivityRewardIndex)); });
    type9.set_function("GetActivityReward", [](CRole* self) -> decltype(auto) { return static_cast<bool>(self->GetActivityReward()); });
    type9.set_function("ClearActivity", [](CRole* self) -> decltype(auto) { self->ClearActivity(); });
    type9.set_function("GetFirstMail", [](CRole* self) -> decltype(auto) { return self->GetFirstMail(); });
    type9.set_function("GetFirstAttach", [](CRole* self) -> decltype(auto) { return self->GetFirstAttach(); });
    type9.set_function("GetRandomAuctionItemID", [](CRole* self) -> decltype(auto) { return self->GetRandomAuctionItemID(); });
    type9.set_function("GetFirstSellingAuction", [](CRole* self) -> decltype(auto) { return self->GetFirstSellingAuction(); });
    type9.set_function("GetFirstBuyingAuction", [](CRole* self) -> decltype(auto) { return self->GetFirstBuyingAuction(); });
    type9.set_function("Hide", [](CRole* self, bool bHidden) -> decltype(auto) { self->Hide(bHidden); });
    type9.set_function("IsHidden", [](CRole* self) -> decltype(auto) { return static_cast<bool>(self->IsHidden()); });
    type9.set_function("GetGMLevel", [](CRole* self) -> decltype(auto) { return self->GetGMLevel(); });
    type9.set_function("SetGMLevel", [](CRole* self, signed int nGMLevel) -> decltype(auto) { self->SetGMLevel(nGMLevel); });
    type9.set_function("SetFcmState", [](CRole* self, unsigned char eFcmState) -> decltype(auto) { self->SetFcmState(eFcmState); });
    type9.set_function("GetFcmState", [](CRole* self) -> decltype(auto) { return self->GetFcmState(); });
    type9.set_function("Kick", [](CRole* self) -> decltype(auto) { self->Kick(); });
    type9.set_function("Kick", sol::overload([](CRole* self, signed int eKickType, const char* pcszFileName, const char* pcszFuncName, signed int nLineNum) -> decltype(auto) { self->Kick(eKickType,pcszFileName,pcszFuncName,nLineNum); }, [](CRole* self) -> decltype(auto) { self->Kick(); }));
    type9.set_function("ErrorCodeNotify", [](CRole* self, signed int nErrCode, signed int nParam1, signed int nParam2) -> decltype(auto) { return static_cast<bool>(self->ErrorCodeNotify(nErrCode,nParam1,nParam2)); });
    type9.set_function("ForceSyncPos", [](CRole* self) -> decltype(auto) { return static_cast<bool>(self->ForceSyncPos()); });
    type9.set_function("TransferRole", [](CRole* self, INT64 llDestX, INT64 llDestY, INT64 llDestZ, signed int nRange, signed int nCount) -> decltype(auto) { return static_cast<bool>(self->TransferRole(llDestX,llDestY,llDestZ,nRange,nCount)); });
    type9.set_function("GetRevivePos", [](CRole* self) -> decltype(auto) { return self->GetRevivePos(); });
    type9.set_function("GenerateAntiCheatJigsaw", [](CRole* self) -> decltype(auto) { self->GenerateAntiCheatJigsaw(); });
    type9.set_function("GetPetState", [](CRole* self) -> decltype(auto) { return self->GetPetState(); });
    type9.set_function("SetPetState", [](CRole* self, signed int nState) -> decltype(auto) { self->SetPetState(nState); });
    type9.set_function("AddPet", [](CRole* self, unsigned int dwNpcID, signed int nPetType) -> decltype(auto) { return static_cast<bool>(self->AddPet(dwNpcID,nPetType)); });
    type9.set_function("GetPetCount", [](CRole* self) -> decltype(auto) { return self->GetPetCount(); });
    type9.set_function("GetMaxPetCount", [](CRole* self) -> decltype(auto) { return self->GetMaxPetCount(); });
    type9.set_function("GetPetNpcID", [](CRole* self, signed int nIndex) -> decltype(auto) { return self->GetPetNpcID(nIndex); });
    type9.set_function("GMCreateGuild", [](CRole* self, const char* pszGuildName_arg, const char* pszAnnounce_arg) -> decltype(auto) { char* pszGuildName = const_cast<char*>(pszGuildName_arg); char* pszAnnounce = const_cast<char*>(pszAnnounce_arg); return static_cast<bool>(self->GMCreateGuild(pszGuildName,pszAnnounce)); });
    type9.set_function("GetJumpRandomNumber", [](CRole* self) -> decltype(auto) { return self->GetJumpRandomNumber(); });
    type9.set_function("QueryRoleLocation", [](CRole* self, const char* pcszRoleName) -> decltype(auto) { return self->QueryRoleLocation(pcszRoleName); });
    type9.set_function("RecallTeamMember", [](CRole* self) -> decltype(auto) { return self->RecallTeamMember(); });
    type9.set_function("IsSabacGuildMember", [](CRole* self) -> decltype(auto) { return static_cast<bool>(self->IsSabacGuildMember()); });
    type9.set_function("CheckIn", [](CRole* self) -> decltype(auto) { return self->CheckIn(); });
    type9.set_function("GetStartExerciseTime", [](CRole* self) -> decltype(auto) { return self->GetStartExerciseTime(); });
    type9.set_function("SetStartExerciseTime", [](CRole* self, unsigned int dwTime) -> decltype(auto) { self->SetStartExerciseTime(dwTime); });
    type9.set_function("UpdateRankListNode", [](CRole* self, signed int nRankListType, signed int nValue1, signed int nValue2) -> decltype(auto) { return static_cast<bool>(self->UpdateRankListNode(nRankListType,nValue1,nValue2)); });
    type9.set_function("GenerateAntiCheatJigsaw", sol::overload([](CRole* self) -> decltype(auto) { self->GenerateAntiCheatJigsaw(); }, [](CRole* self) -> decltype(auto) { self->GenerateAntiCheatJigsaw(); }));
    type9.set_function("SetAntiCheatLevel", [](CRole* self, signed int nLevel) -> decltype(auto) { self->SetAntiCheatLevel(nLevel); });
    type9.set_function("GetGameTimeCard_UINT32", [](CRole* self) -> decltype(auto) { return self->GetGameTimeCard_UINT32(); });
    type9.set_function("AddGameTimeCard", [](CRole* self, unsigned int time) -> decltype(auto) { self->AddGameTimeCard(time); });
    type9.set_function("NotifyRole_GameTimeCard", [](CRole* self) -> decltype(auto) { self->NotifyRole_GameTimeCard(); });
    type9.set_function("GetRuneRootCount", [](CRole* self, signed int nType, signed int cls) -> decltype(auto) { return self->GetRuneRootCount(nType,cls); });
    type9.set_function("IncRuneRootCount", [](CRole* self, signed int nType, signed int cls) -> decltype(auto) { self->IncRuneRootCount(nType,cls); });
    type9.set_function("NotifyRole_RuneRootCount", [](CRole* self, signed int nType, signed int cls) -> decltype(auto) { self->NotifyRole_RuneRootCount(nType,cls); });
    type9.set_function("GetLuckRootCount", [](CRole* self) -> decltype(auto) { return self->GetLuckRootCount(); });
    type9.set_function("IncLuckRootCount", [](CRole* self) -> decltype(auto) { self->IncLuckRootCount(); });
    type9.set_function("NotifyRole_LuckRootCount", [](CRole* self) -> decltype(auto) { self->NotifyRole_LuckRootCount(); });
    auto type10 = cqyh::lua::usertype<CReputation>(L, globals, "CReputation", "CReputation");
    type10.set_function("GetValue", [](CReputation* self, signed int nForceID) -> decltype(auto) { return self->GetValue(nForceID); });
    type10.set_function("SetValue", [](CReputation* self, signed int nForceID, signed int nNewValue) -> decltype(auto) { self->SetValue(nForceID,nNewValue); });
    type10.set_function("ModifyValue", [](CReputation* self, signed int nForceID, signed int nValue) -> decltype(auto) { self->ModifyValue(nForceID,nValue); });
    type10.set_function("ActiveForce", [](CReputation* self, signed int nForceID) -> decltype(auto) { self->ActiveForce(nForceID); });
    type10.set_function("ReceiveAward", [](CReputation* self, signed int nForceID) -> decltype(auto) { return self->ReceiveAward(nForceID); });
    return 1;
}
