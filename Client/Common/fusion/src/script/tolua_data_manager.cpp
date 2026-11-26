/*
** Lua binding: data_manager
** Generated automatically by tolua++-1.0.92 on 11/08/18 22:50:31.
*/

#include "stdafx.h"

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"
#include "../../../utility/lua_array_index.h"
#include "../../../utility/fixed_string.h"

/* Exported function */
TOLUA_API int tolua_data_manager_open (lua_State* tolua_S);

#include "../../../common/fusion/inc/ai_data.h"
#include "../../../common/fusion/inc/aura_data.h"
#include "../../../common/fusion/inc/doodad_data.h"
#include "../../../common/fusion/inc/init_reputation_data.h"
#include "../../../common/fusion/inc/item_data.h"
#include "../../../common/fusion/inc/level_data.h"
#include "../../../common/fusion/inc/missile_data.h"
#include "../../../common/fusion/inc/npc_data.h"
#include "../../../common/fusion/inc/npc_relation_data.h"
#include "../../../common/fusion/inc/quest_data.h"
#include "../../../common/fusion/inc/role_relation_data.h"
#include "../../../common/fusion/inc/scene_data.h"
#include "../../../common/fusion/inc/shop_data.h"
#include "../../../common/fusion/inc/spell_data.h"
#include "../../../common/fusion/inc/attr_conversion_data.h"
#include "../../../common/fusion/inc/achievement_data.h"
#include "../../../common/fusion/inc/title_data.h"
#include "../../../common/fusion/inc/guild_building_data.h"
#include "../../../common/fusion/inc/guild_activity.h"
#include "../../../common/fusion/inc/soul_data.h"
#include "../../../common/fusion/inc/pet_data.h"
#include "../../../common/fusion/inc/mail_template_data.h"
#include "data_manager.h"
#include "ai_data.h"
#include "aura_data.h"
#include "doodad_data.h"
#include "init_reputation_data.h"
#include "item_data.h"
#include "level_data.h"
#include "missile_data.h"
#include "npc_data.h"
#include "npc_relation_data.h"
#include "quest_data.h"
#include "role_relation_data.h"
#include "scene_data.h"
#include "shop_data.h"
#include "spell_data.h"
#include "attr_conversion_data.h"
#include "title_data.h"
#include "guild_building_data.h"
#include "guild_activity.h"
#include "soul_data.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_TITLE_TEMPLATE__TitleCondition (lua_State* tolua_S)
{
 TITLE_TEMPLATE::TitleCondition* self = (TITLE_TEMPLATE::TitleCondition*) tolua_tousertype(tolua_S,1,0);
 Mtolua_delete(self);
 return 0;
}

static int tolua_collect_SCENE_DATA__WEATHER_DATA (lua_State* tolua_S)
{
 SCENE_DATA::WEATHER_DATA* self = (SCENE_DATA::WEATHER_DATA*) tolua_tousertype(tolua_S,1,0);
 Mtolua_delete(self);
 return 0;
}

static int tolua_collect_ITEM_DATA__CD_GROUP (lua_State* tolua_S)
{
 ITEM_DATA::CD_GROUP* self = (ITEM_DATA::CD_GROUP*) tolua_tousertype(tolua_S,1,0);
 Mtolua_delete(self);
 return 0;
}

static int tolua_collect_AURA_ENTRY__EX_EVT_POINT (lua_State* tolua_S)
{
 AURA_ENTRY::EX_EVT_POINT* self = (AURA_ENTRY::EX_EVT_POINT*) tolua_tousertype(tolua_S,1,0);
 Mtolua_delete(self);
 return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"ACHIEVEMENT_DATA");
 tolua_usertype(tolua_S,"NPC_RELATION_DATA");
 tolua_usertype(tolua_S,"CDataManager<SHOP_DATA>");
 tolua_usertype(tolua_S,"CDataManager<MAIL_TEMPLATE_DATA>");
 tolua_usertype(tolua_S,"REWARD_QUEST_MONEY_COST_DATA");
 tolua_usertype(tolua_S,"GUILD_ACTIVITY_DATA");
 tolua_usertype(tolua_S,"CDataManager<GUILD_ACTIVITY_DATA>");
 tolua_usertype(tolua_S,"TITLE_TEMPLATE::TitleCondition");
 tolua_usertype(tolua_S,"NPC_DATA");
 tolua_usertype(tolua_S,"MAIL_TEMPLATE_DATA");
 tolua_usertype(tolua_S,"CDataManager<SOUL_DATA>");
 tolua_usertype(tolua_S,"ITEM_DATA::CD_GROUP");
 tolua_usertype(tolua_S,"ROLE_RELATION_DATA");
 tolua_usertype(tolua_S,"CDataManager<GUILD_BUILDING_DATA>");
 tolua_usertype(tolua_S,"CDataManager<HERO_LEVEL_DATA>");
 tolua_usertype(tolua_S,"CDataManager<NEW_AI_DATA>");
 tolua_usertype(tolua_S,"CDataManager<ACHIEVEMENT_DATA>");
 tolua_usertype(tolua_S,"ATTR_LEVEL");
 tolua_usertype(tolua_S,"SPELL_VAR");
 tolua_usertype(tolua_S,"CWeightRand");
 tolua_usertype(tolua_S,"CDataManager<INIT_REPUTATION_DATA>");
 tolua_usertype(tolua_S,"SHOP_DATA");
 tolua_usertype(tolua_S,"TITLE_TEMPLATE");
 tolua_usertype(tolua_S,"CDataManager<SPELL_VAR>");
 tolua_usertype(tolua_S,"EQUIP_DATA");
 tolua_usertype(tolua_S,"CTabFile");
 tolua_usertype(tolua_S,"CDataManager<DOODAD_DATA>");
 tolua_usertype(tolua_S,"QUEST_REWARD_DATA");
 tolua_usertype(tolua_S,"DOODAD_DATA");
 tolua_usertype(tolua_S,"CDataManager<SCENE_DATA>");
 tolua_usertype(tolua_S,"ROLE_LEVEL_DATA");
 tolua_usertype(tolua_S,"SOUL_DATA");
 tolua_usertype(tolua_S,"CDataManager<ROLE_RELATION_DATA>");
 tolua_usertype(tolua_S,"CDataManager<QUEST_DATA>");
 tolua_usertype(tolua_S,"SCENE_DATA::WEATHER_DATA");
 tolua_usertype(tolua_S,"ITEM_DATA");
 tolua_usertype(tolua_S,"AURA_ENTRY::EX_EVT_POINT");
 tolua_usertype(tolua_S,"CDataManager<NPC_RELATION_DATA>");
 tolua_usertype(tolua_S,"CDataManager<NPC_DATA>");
 tolua_usertype(tolua_S,"CDataManager<MISSILE_DATA>");
 tolua_usertype(tolua_S,"CDataManager<TITLE_TEMPLATE>");
 tolua_usertype(tolua_S,"PET_DATA");
 tolua_usertype(tolua_S,"CDataManager<ROLE_LEVEL_DATA>");
 tolua_usertype(tolua_S,"CDataManager<ITEM_DATA>");
 tolua_usertype(tolua_S,"CDataManager<EQUIP_DATA>");
 tolua_usertype(tolua_S,"SPELL_ENTRY");
 tolua_usertype(tolua_S,"CDataManager<ATTR_LEVEL>");
 tolua_usertype(tolua_S,"INIT_REPUTATION_DATA");
 tolua_usertype(tolua_S,"ATTRIBUTE_DATA");
 tolua_usertype(tolua_S,"NEW_AI_DATA");
 tolua_usertype(tolua_S,"AURA_ENTRY");
 tolua_usertype(tolua_S,"CDataManager<SPELL_ENTRY>");
 tolua_usertype(tolua_S,"HANDBOOK_DATA");
 tolua_usertype(tolua_S,"ADVENTURE_DATA");
 tolua_usertype(tolua_S,"GUILD_BUILDING_DATA");
 tolua_usertype(tolua_S,"HERO_LEVEL_DATA");
 tolua_usertype(tolua_S,"QUEST_RECYCLE_DATA");
 tolua_usertype(tolua_S,"QUEST_DATA");
 tolua_usertype(tolua_S,"MISSILE_DATA");
 tolua_usertype(tolua_S,"CDataManager<AURA_ENTRY>");
 tolua_usertype(tolua_S,"SCENE_DATA");
}

/* get function: nAIID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nAIID
static int tolua_get_NEW_AI_DATA_nAIID(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAIID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAIID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAIID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nAIID
static int tolua_set_NEW_AI_DATA_nAIID(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAIID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAIID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szScriptName of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_szScriptName
static int tolua_get_NEW_AI_DATA_szScriptName(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szScriptName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szScriptName, ClientFixedString::Length(self->szScriptName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szScriptName of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_szScriptName
static int tolua_set_NEW_AI_DATA_szScriptName(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szScriptName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szScriptName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nWalkSpeed of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nWalkSpeed
static int tolua_get_NEW_AI_DATA_nWalkSpeed(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWalkSpeed'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nWalkSpeed);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nWalkSpeed of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nWalkSpeed
static int tolua_set_NEW_AI_DATA_nWalkSpeed(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWalkSpeed'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nWalkSpeed = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRunSpeed of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nRunSpeed
static int tolua_get_NEW_AI_DATA_nRunSpeed(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRunSpeed'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRunSpeed);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRunSpeed of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nRunSpeed
static int tolua_set_NEW_AI_DATA_nRunSpeed(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRunSpeed'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRunSpeed = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRotateSpeed of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nRotateSpeed
static int tolua_get_NEW_AI_DATA_nRotateSpeed(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRotateSpeed'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRotateSpeed);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRotateSpeed of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nRotateSpeed
static int tolua_set_NEW_AI_DATA_nRotateSpeed(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRotateSpeed'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRotateSpeed = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bPatrol of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_bPatrol
static int tolua_get_NEW_AI_DATA_bPatrol(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bPatrol'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bPatrol);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bPatrol of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_bPatrol
static int tolua_set_NEW_AI_DATA_bPatrol(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bPatrol'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bPatrol = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bFollow of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_bFollow
static int tolua_get_NEW_AI_DATA_bFollow(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bFollow'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bFollow);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bFollow of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_bFollow
static int tolua_set_NEW_AI_DATA_bFollow(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bFollow'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bFollow = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCanRun of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_bCanRun
static int tolua_get_NEW_AI_DATA_bCanRun(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanRun'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCanRun);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCanRun of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_bCanRun
static int tolua_set_NEW_AI_DATA_bCanRun(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanRun'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCanRun = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bAlwaysRun of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_bAlwaysRun
static int tolua_get_NEW_AI_DATA_bAlwaysRun(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bAlwaysRun'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bAlwaysRun);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bAlwaysRun of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_bAlwaysRun
static int tolua_set_NEW_AI_DATA_bAlwaysRun(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bAlwaysRun'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bAlwaysRun = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nScanEnemyRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nScanEnemyRange
static int tolua_get_NEW_AI_DATA_nScanEnemyRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nScanEnemyRange'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nScanEnemyRange);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nScanEnemyRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nScanEnemyRange
static int tolua_set_NEW_AI_DATA_nScanEnemyRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nScanEnemyRange'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nScanEnemyRange = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nScanFriendRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nScanFriendRange
static int tolua_get_NEW_AI_DATA_nScanFriendRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nScanFriendRange'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nScanFriendRange);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nScanFriendRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nScanFriendRange
static int tolua_set_NEW_AI_DATA_nScanFriendRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nScanFriendRange'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nScanFriendRange = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nChaseRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nChaseRange
static int tolua_get_NEW_AI_DATA_nChaseRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChaseRange'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nChaseRange);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nChaseRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nChaseRange
static int tolua_set_NEW_AI_DATA_nChaseRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChaseRange'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nChaseRange = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nPreferDistance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nPreferDistance
static int tolua_get_NEW_AI_DATA_nPreferDistance(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreferDistance'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nPreferDistance);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nPreferDistance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nPreferDistance
static int tolua_set_NEW_AI_DATA_nPreferDistance(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreferDistance'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nPreferDistance = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nWithdrawThreshold of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nWithdrawThreshold
static int tolua_get_NEW_AI_DATA_nWithdrawThreshold(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWithdrawThreshold'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nWithdrawThreshold);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nWithdrawThreshold of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nWithdrawThreshold
static int tolua_set_NEW_AI_DATA_nWithdrawThreshold(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWithdrawThreshold'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nWithdrawThreshold = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nChaseThreshold of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nChaseThreshold
static int tolua_get_NEW_AI_DATA_nChaseThreshold(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChaseThreshold'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nChaseThreshold);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nChaseThreshold of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nChaseThreshold
static int tolua_set_NEW_AI_DATA_nChaseThreshold(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChaseThreshold'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nChaseThreshold = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nChaseDelayTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nChaseDelayTime
static int tolua_get_NEW_AI_DATA_nChaseDelayTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChaseDelayTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nChaseDelayTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nChaseDelayTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nChaseDelayTime
static int tolua_set_NEW_AI_DATA_nChaseDelayTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChaseDelayTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nChaseDelayTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nInitAuraID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_NEW_AI_DATA_nInitAuraID
static int tolua_get_data_manager_NEW_AI_DATA_nInitAuraID(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nInitAuraID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_AI_INIT_AURA_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nInitAuraID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nInitAuraID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_NEW_AI_DATA_nInitAuraID
static int tolua_set_data_manager_NEW_AI_DATA_nInitAuraID(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nInitAuraID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_AI_INIT_AURA_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nInitAuraID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLiveTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nLiveTime
static int tolua_get_NEW_AI_DATA_nLiveTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLiveTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLiveTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLiveTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nLiveTime
static int tolua_set_NEW_AI_DATA_nLiveTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLiveTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLiveTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bDisappearOnTimeout of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_bDisappearOnTimeout
static int tolua_get_NEW_AI_DATA_bDisappearOnTimeout(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bDisappearOnTimeout'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bDisappearOnTimeout);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bDisappearOnTimeout of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_bDisappearOnTimeout
static int tolua_set_NEW_AI_DATA_bDisappearOnTimeout(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bDisappearOnTimeout'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bDisappearOnTimeout = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nWanderRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nWanderRange
static int tolua_get_NEW_AI_DATA_nWanderRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWanderRange'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nWanderRange);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nWanderRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nWanderRange
static int tolua_set_NEW_AI_DATA_nWanderRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWanderRange'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nWanderRange = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nWanderInterval of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nWanderInterval
static int tolua_get_NEW_AI_DATA_nWanderInterval(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWanderInterval'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nWanderInterval);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nWanderInterval of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nWanderInterval
static int tolua_set_NEW_AI_DATA_nWanderInterval(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWanderInterval'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nWanderInterval = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSpellID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_NEW_AI_DATA_nSpellID
static int tolua_get_data_manager_NEW_AI_DATA_nSpellID(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSpellID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=nastTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSpellID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSpellID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_NEW_AI_DATA_nSpellID
static int tolua_set_data_manager_NEW_AI_DATA_nSpellID(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSpellID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=nastTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nSpellID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSpellChance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_NEW_AI_DATA_nSpellChance
static int tolua_get_data_manager_NEW_AI_DATA_nSpellChance(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSpellChance, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=nastTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSpellChance[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSpellChance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_NEW_AI_DATA_nSpellChance
static int tolua_set_data_manager_NEW_AI_DATA_nSpellChance(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSpellChance, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=nastTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nSpellChance[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSpellCDTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_NEW_AI_DATA_nSpellCDTime
static int tolua_get_data_manager_NEW_AI_DATA_nSpellCDTime(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSpellCDTime, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=nastTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSpellCDTime[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSpellCDTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_NEW_AI_DATA_nSpellCDTime
static int tolua_set_data_manager_NEW_AI_DATA_nSpellCDTime(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSpellCDTime, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=nastTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nSpellCDTime[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSelfHpPercent of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_NEW_AI_DATA_nSelfHpPercent
static int tolua_get_data_manager_NEW_AI_DATA_nSelfHpPercent(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSelfHpPercent, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=nastTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSelfHpPercent[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSelfHpPercent of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_NEW_AI_DATA_nSelfHpPercent
static int tolua_set_data_manager_NEW_AI_DATA_nSelfHpPercent(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSelfHpPercent, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=nastTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nSelfHpPercent[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nGlobalCDTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nGlobalCDTime
static int tolua_get_NEW_AI_DATA_nGlobalCDTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nGlobalCDTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nGlobalCDTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nGlobalCDTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nGlobalCDTime
static int tolua_set_NEW_AI_DATA_nGlobalCDTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nGlobalCDTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nGlobalCDTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nFriendlyNpcTemplateID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nFriendlyNpcTemplateID
static int tolua_get_NEW_AI_DATA_nFriendlyNpcTemplateID(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFriendlyNpcTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nFriendlyNpcTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nFriendlyNpcTemplateID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nFriendlyNpcTemplateID
static int tolua_set_NEW_AI_DATA_nFriendlyNpcTemplateID(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFriendlyNpcTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nFriendlyNpcTemplateID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bFriendlyCorpse of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_bFriendlyCorpse
static int tolua_get_NEW_AI_DATA_bFriendlyCorpse(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bFriendlyCorpse'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bFriendlyCorpse);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bFriendlyCorpse of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_bFriendlyCorpse
static int tolua_set_NEW_AI_DATA_bFriendlyCorpse(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bFriendlyCorpse'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bFriendlyCorpse = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCorpseTemplateID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nCorpseTemplateID
static int tolua_get_NEW_AI_DATA_nCorpseTemplateID(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCorpseTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCorpseTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCorpseTemplateID of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nCorpseTemplateID
static int tolua_set_NEW_AI_DATA_nCorpseTemplateID(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCorpseTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCorpseTemplateID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nIssuerSpellSignalType of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nIssuerSpellSignalType
static int tolua_get_NEW_AI_DATA_nIssuerSpellSignalType(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nIssuerSpellSignalType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nIssuerSpellSignalType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nIssuerSpellSignalType of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nIssuerSpellSignalType
static int tolua_set_NEW_AI_DATA_nIssuerSpellSignalType(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nIssuerSpellSignalType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nIssuerSpellSignalType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nIssuerSpellSignalRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nIssuerSpellSignalRange
static int tolua_get_NEW_AI_DATA_nIssuerSpellSignalRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nIssuerSpellSignalRange'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nIssuerSpellSignalRange);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nIssuerSpellSignalRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nIssuerSpellSignalRange
static int tolua_set_NEW_AI_DATA_nIssuerSpellSignalRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nIssuerSpellSignalRange'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nIssuerSpellSignalRange = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTargetSpellSignalType of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nTargetSpellSignalType
static int tolua_get_NEW_AI_DATA_nTargetSpellSignalType(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTargetSpellSignalType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTargetSpellSignalType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTargetSpellSignalType of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nTargetSpellSignalType
static int tolua_set_NEW_AI_DATA_nTargetSpellSignalType(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTargetSpellSignalType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTargetSpellSignalType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTargetSpellSignalRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nTargetSpellSignalRange
static int tolua_get_NEW_AI_DATA_nTargetSpellSignalRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTargetSpellSignalRange'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTargetSpellSignalRange);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTargetSpellSignalRange of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nTargetSpellSignalRange
static int tolua_set_NEW_AI_DATA_nTargetSpellSignalRange(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTargetSpellSignalRange'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTargetSpellSignalRange = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nFleeHpPercent of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nFleeHpPercent
static int tolua_get_NEW_AI_DATA_nFleeHpPercent(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFleeHpPercent'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nFleeHpPercent);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nFleeHpPercent of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nFleeHpPercent
static int tolua_set_NEW_AI_DATA_nFleeHpPercent(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFleeHpPercent'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nFleeHpPercent = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nFleeChance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nFleeChance
static int tolua_get_NEW_AI_DATA_nFleeChance(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFleeChance'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nFleeChance);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nFleeChance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nFleeChance
static int tolua_set_NEW_AI_DATA_nFleeChance(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFleeChance'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nFleeChance = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nFleeDuration of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nFleeDuration
static int tolua_get_NEW_AI_DATA_nFleeDuration(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFleeDuration'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nFleeDuration);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nFleeDuration of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nFleeDuration
static int tolua_set_NEW_AI_DATA_nFleeDuration(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFleeDuration'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nFleeDuration = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nStepAroundChance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nStepAroundChance
static int tolua_get_NEW_AI_DATA_nStepAroundChance(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStepAroundChance'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nStepAroundChance);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nStepAroundChance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nStepAroundChance
static int tolua_set_NEW_AI_DATA_nStepAroundChance(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStepAroundChance'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nStepAroundChance = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nStepAroundCDTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nStepAroundCDTime
static int tolua_get_NEW_AI_DATA_nStepAroundCDTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStepAroundCDTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nStepAroundCDTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nStepAroundCDTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nStepAroundCDTime
static int tolua_set_NEW_AI_DATA_nStepAroundCDTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStepAroundCDTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nStepAroundCDTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nChangeTargetChance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nChangeTargetChance
static int tolua_get_NEW_AI_DATA_nChangeTargetChance(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChangeTargetChance'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nChangeTargetChance);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nChangeTargetChance of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nChangeTargetChance
static int tolua_set_NEW_AI_DATA_nChangeTargetChance(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChangeTargetChance'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nChangeTargetChance = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nChangeTargetCDTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nChangeTargetCDTime
static int tolua_get_NEW_AI_DATA_nChangeTargetCDTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChangeTargetCDTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nChangeTargetCDTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nChangeTargetCDTime of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nChangeTargetCDTime
static int tolua_set_NEW_AI_DATA_nChangeTargetCDTime(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nChangeTargetCDTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nChangeTargetCDTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSpreadThreatCount of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NEW_AI_DATA_nSpreadThreatCount
static int tolua_get_NEW_AI_DATA_nSpreadThreatCount(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpreadThreatCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSpreadThreatCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSpreadThreatCount of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NEW_AI_DATA_nSpreadThreatCount
static int tolua_set_NEW_AI_DATA_nSpreadThreatCount(lua_State* tolua_S)
{
  NEW_AI_DATA* self = (NEW_AI_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpreadThreatCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSpreadThreatCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nParam of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_NEW_AI_DATA_nParam
static int tolua_get_data_manager_NEW_AI_DATA_nParam(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nParam, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_AI_DATA_PARAM_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nParam[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nParam of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_NEW_AI_DATA_nParam
static int tolua_set_data_manager_NEW_AI_DATA_nParam(lua_State* tolua_S)
{
 int tolua_index;
  NEW_AI_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NEW_AI_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nParam, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_AI_DATA_PARAM_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nParam[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  NEW_AI_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_NEW_AI_DATA_ReadLine00
static int tolua_data_manager_NEW_AI_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"NEW_AI_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"NEW_AI_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  NEW_AI_DATA* pData = ((NEW_AI_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
NEW_AI_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nID of class  AURA_ENTRY */
#ifndef TOLUA_DISABLE_tolua_get_AURA_ENTRY_m_nID
static int tolua_get_AURA_ENTRY_m_nID(lua_State* tolua_S)
{
  AURA_ENTRY* self = (AURA_ENTRY*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nID of class  AURA_ENTRY */
#ifndef TOLUA_DISABLE_tolua_set_AURA_ENTRY_m_nID
static int tolua_set_AURA_ENTRY_m_nID(lua_State* tolua_S)
{
  AURA_ENTRY* self = (AURA_ENTRY*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nAttrTable of class  AURA_ENTRY */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_AURA_ENTRY_m_nAttrTable
static int tolua_get_data_manager_AURA_ENTRY_m_nAttrTable(lua_State* tolua_S)
{
 int tolua_index;
  AURA_ENTRY* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (AURA_ENTRY*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nAttrTable, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=aatTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nAttrTable[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nAttrTable of class  AURA_ENTRY */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_AURA_ENTRY_m_nAttrTable
static int tolua_set_data_manager_AURA_ENTRY_m_nAttrTable(lua_State* tolua_S)
{
 int tolua_index;
  AURA_ENTRY* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (AURA_ENTRY*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nAttrTable, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=aatTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_nAttrTable[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nExEvtTime of class  EX_EVT_POINT */
#ifndef TOLUA_DISABLE_tolua_get_AURA_ENTRY__EX_EVT_POINT_m_nExEvtTime
static int tolua_get_AURA_ENTRY__EX_EVT_POINT_m_nExEvtTime(lua_State* tolua_S)
{
  AURA_ENTRY::EX_EVT_POINT* self = (AURA_ENTRY::EX_EVT_POINT*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nExEvtTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nExEvtTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nExEvtTime of class  EX_EVT_POINT */
#ifndef TOLUA_DISABLE_tolua_set_AURA_ENTRY__EX_EVT_POINT_m_nExEvtTime
static int tolua_set_AURA_ENTRY__EX_EVT_POINT_m_nExEvtTime(lua_State* tolua_S)
{
  AURA_ENTRY::EX_EVT_POINT* self = (AURA_ENTRY::EX_EVT_POINT*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nExEvtTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nExEvtTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nExOnEvt of class  EX_EVT_POINT */
#ifndef TOLUA_DISABLE_tolua_get_AURA_ENTRY__EX_EVT_POINT_m_nExOnEvt
static int tolua_get_AURA_ENTRY__EX_EVT_POINT_m_nExOnEvt(lua_State* tolua_S)
{
  AURA_ENTRY::EX_EVT_POINT* self = (AURA_ENTRY::EX_EVT_POINT*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nExOnEvt'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nExOnEvt);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nExOnEvt of class  EX_EVT_POINT */
#ifndef TOLUA_DISABLE_tolua_set_AURA_ENTRY__EX_EVT_POINT_m_nExOnEvt
static int tolua_set_AURA_ENTRY__EX_EVT_POINT_m_nExOnEvt(lua_State* tolua_S)
{
  AURA_ENTRY::EX_EVT_POINT* self = (AURA_ENTRY::EX_EVT_POINT*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nExOnEvt'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nExOnEvt = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_ExEvtPoint of class  AURA_ENTRY */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_AURA_ENTRY_m_ExEvtPoint
static int tolua_get_data_manager_AURA_ENTRY_m_ExEvtPoint(lua_State* tolua_S)
{
 int tolua_index;
  AURA_ENTRY* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (AURA_ENTRY*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_ExEvtPoint, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_AURA_EX_EVT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)&self->m_ExEvtPoint[tolua_index],"AURA_ENTRY::EX_EVT_POINT");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_ExEvtPoint of class  AURA_ENTRY */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_AURA_ENTRY_m_ExEvtPoint
static int tolua_set_data_manager_AURA_ENTRY_m_ExEvtPoint(lua_State* tolua_S)
{
 int tolua_index;
  AURA_ENTRY* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (AURA_ENTRY*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_ExEvtPoint, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_AURA_EX_EVT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_ExEvtPoint[tolua_index] = *((AURA_ENTRY::EX_EVT_POINT*)  tolua_tousertype(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  AURA_ENTRY */
#ifndef TOLUA_DISABLE_tolua_data_manager_AURA_ENTRY_ReadLine00
static int tolua_data_manager_AURA_ENTRY_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"AURA_ENTRY",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"AURA_ENTRY",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  AURA_ENTRY* pData = ((AURA_ENTRY*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
AURA_ENTRY::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTemplateID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nTemplateID
static int tolua_get_DOODAD_DATA_nTemplateID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTemplateID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nTemplateID
static int tolua_set_DOODAD_DATA_nTemplateID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTemplateID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szName of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_szName
static int tolua_get_DOODAD_DATA_szName(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szName, ClientFixedString::Length(self->szName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szName of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_szName
static int tolua_set_DOODAD_DATA_szName(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLevel of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nLevel
static int tolua_get_DOODAD_DATA_nLevel(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLevel of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nLevel
static int tolua_set_DOODAD_DATA_nLevel(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nType of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nType
static int tolua_get_DOODAD_DATA_nType(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nType of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nType
static int tolua_set_DOODAD_DATA_nType(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSubType of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nSubType
static int tolua_get_DOODAD_DATA_nSubType(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSubType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSubType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSubType of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nSubType
static int tolua_set_DOODAD_DATA_nSubType(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSubType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSubType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBodyType of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nBodyType
static int tolua_get_DOODAD_DATA_nBodyType(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBodyType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBodyType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBodyType of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nBodyType
static int tolua_set_DOODAD_DATA_nBodyType(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBodyType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBodyType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nOperateDuration of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nOperateDuration
static int tolua_get_DOODAD_DATA_nOperateDuration(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOperateDuration'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nOperateDuration);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nOperateDuration of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nOperateDuration
static int tolua_set_DOODAD_DATA_nOperateDuration(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOperateDuration'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nOperateDuration = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMinOpenCount of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nMinOpenCount
static int tolua_get_DOODAD_DATA_nMinOpenCount(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinOpenCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMinOpenCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMinOpenCount of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nMinOpenCount
static int tolua_set_DOODAD_DATA_nMinOpenCount(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinOpenCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMinOpenCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxOpenCount of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nMaxOpenCount
static int tolua_get_DOODAD_DATA_nMaxOpenCount(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxOpenCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxOpenCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxOpenCount of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nMaxOpenCount
static int tolua_set_DOODAD_DATA_nMaxOpenCount(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxOpenCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxOpenCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRequireItemID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nRequireItemID
static int tolua_get_DOODAD_DATA_nRequireItemID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRequireItemID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRequireItemID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRequireItemID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nRequireItemID
static int tolua_set_DOODAD_DATA_nRequireItemID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRequireItemID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRequireItemID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCostItem of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_bCostItem
static int tolua_get_DOODAD_DATA_bCostItem(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCostItem'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCostItem);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCostItem of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_bCostItem
static int tolua_set_DOODAD_DATA_bCostItem(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCostItem'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCostItem = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRequireVarID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nRequireVarID
static int tolua_get_DOODAD_DATA_nRequireVarID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRequireVarID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRequireVarID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRequireVarID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nRequireVarID
static int tolua_set_DOODAD_DATA_nRequireVarID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRequireVarID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRequireVarID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRequireVarValue of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nRequireVarValue
static int tolua_get_DOODAD_DATA_nRequireVarValue(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRequireVarValue'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRequireVarValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRequireVarValue of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nRequireVarValue
static int tolua_set_DOODAD_DATA_nRequireVarValue(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRequireVarValue'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRequireVarValue = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSpawnNpcID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nSpawnNpcID
static int tolua_get_DOODAD_DATA_nSpawnNpcID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpawnNpcID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSpawnNpcID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSpawnNpcID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nSpawnNpcID
static int tolua_set_DOODAD_DATA_nSpawnNpcID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpawnNpcID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSpawnNpcID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSpawnNpcChance of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nSpawnNpcChance
static int tolua_get_DOODAD_DATA_nSpawnNpcChance(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpawnNpcChance'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSpawnNpcChance);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSpawnNpcChance of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nSpawnNpcChance
static int tolua_set_DOODAD_DATA_nSpawnNpcChance(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpawnNpcChance'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSpawnNpcChance = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAuraID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nAuraID
static int tolua_get_DOODAD_DATA_nAuraID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAuraID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAuraID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAuraID of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nAuraID
static int tolua_set_DOODAD_DATA_nAuraID(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAuraID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAuraID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLifeFrm of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_nLifeFrm
static int tolua_get_DOODAD_DATA_nLifeFrm(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLifeFrm'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLifeFrm);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLifeFrm of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_nLifeFrm
static int tolua_set_DOODAD_DATA_nLifeFrm(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLifeFrm'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLifeFrm = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szScriptName of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_szScriptName
static int tolua_get_DOODAD_DATA_szScriptName(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szScriptName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szScriptName, ClientFixedString::Length(self->szScriptName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szScriptName of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DOODAD_DATA_szScriptName
static int tolua_set_DOODAD_DATA_szScriptName(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szScriptName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szScriptName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: pAdventureData of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DOODAD_DATA_pAdventureData_ptr
static int tolua_get_DOODAD_DATA_pAdventureData_ptr(lua_State* tolua_S)
{
  DOODAD_DATA* self = (DOODAD_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'pAdventureData'",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)self->pAdventureData,"const ADVENTURE_DATA");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  DOODAD_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_DOODAD_DATA_ReadLine00
static int tolua_data_manager_DOODAD_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"DOODAD_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"DOODAD_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  DOODAD_DATA* pData = ((DOODAD_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
DOODAD_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nForceID of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_INIT_REPUTATION_DATA_nForceID
static int tolua_get_INIT_REPUTATION_DATA_nForceID(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nForceID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nForceID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nForceID of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_INIT_REPUTATION_DATA_nForceID
static int tolua_set_INIT_REPUTATION_DATA_nForceID(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nForceID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nForceID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nInitValue of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_INIT_REPUTATION_DATA_nInitValue
static int tolua_get_INIT_REPUTATION_DATA_nInitValue(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitValue'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nInitValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nInitValue of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_INIT_REPUTATION_DATA_nInitValue
static int tolua_set_INIT_REPUTATION_DATA_nInitValue(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitValue'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nInitValue = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxValue of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_INIT_REPUTATION_DATA_nMaxValue
static int tolua_get_INIT_REPUTATION_DATA_nMaxValue(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxValue'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxValue of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_INIT_REPUTATION_DATA_nMaxValue
static int tolua_set_INIT_REPUTATION_DATA_nMaxValue(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxValue'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxValue = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bActive of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_INIT_REPUTATION_DATA_bActive
static int tolua_get_INIT_REPUTATION_DATA_bActive(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bActive'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bActive);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bActive of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_INIT_REPUTATION_DATA_bActive
static int tolua_set_INIT_REPUTATION_DATA_bActive(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bActive'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bActive = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCanAttackWithFreePkMode of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_INIT_REPUTATION_DATA_bCanAttackWithFreePkMode
static int tolua_get_INIT_REPUTATION_DATA_bCanAttackWithFreePkMode(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanAttackWithFreePkMode'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCanAttackWithFreePkMode);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCanAttackWithFreePkMode of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_INIT_REPUTATION_DATA_bCanAttackWithFreePkMode
static int tolua_set_INIT_REPUTATION_DATA_bCanAttackWithFreePkMode(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanAttackWithFreePkMode'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCanAttackWithFreePkMode = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bSabacMode of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_INIT_REPUTATION_DATA_bSabacMode
static int tolua_get_INIT_REPUTATION_DATA_bSabacMode(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bSabacMode'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bSabacMode);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bSabacMode of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_INIT_REPUTATION_DATA_bSabacMode
static int tolua_set_INIT_REPUTATION_DATA_bSabacMode(lua_State* tolua_S)
{
  INIT_REPUTATION_DATA* self = (INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bSabacMode'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bSabacMode = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  INIT_REPUTATION_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_INIT_REPUTATION_DATA_ReadLine00
static int tolua_data_manager_INIT_REPUTATION_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"INIT_REPUTATION_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"INIT_REPUTATION_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  INIT_REPUTATION_DATA* pData = ((INIT_REPUTATION_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
INIT_REPUTATION_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nTemplateID of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nTemplateID
static int tolua_get_ITEM_DATA_m_nTemplateID(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nTemplateID of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nTemplateID
static int tolua_set_ITEM_DATA_m_nTemplateID(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nTemplateID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_szName of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_szName
static int tolua_get_ITEM_DATA_m_szName(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->m_szName, ClientFixedString::Length(self->m_szName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_szName of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_szName
static int tolua_set_ITEM_DATA_m_szName(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->m_szName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byItemLevel of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byItemLevel
static int tolua_get_ITEM_DATA_unsigned_m_byItemLevel(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byItemLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byItemLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byItemLevel of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byItemLevel
static int tolua_set_ITEM_DATA_unsigned_m_byItemLevel(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byItemLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byItemLevel = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byType
static int tolua_get_ITEM_DATA_unsigned_m_byType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byType
static int tolua_set_ITEM_DATA_unsigned_m_byType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byType = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_bySubType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_bySubType
static int tolua_get_ITEM_DATA_unsigned_m_bySubType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bySubType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_bySubType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_bySubType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_bySubType
static int tolua_set_ITEM_DATA_unsigned_m_bySubType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bySubType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_bySubType = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byUseInCombat of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byUseInCombat
static int tolua_get_ITEM_DATA_unsigned_m_byUseInCombat(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byUseInCombat'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byUseInCombat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byUseInCombat of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byUseInCombat
static int tolua_set_ITEM_DATA_unsigned_m_byUseInCombat(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byUseInCombat'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byUseInCombat = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byUseInArena of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byUseInArena
static int tolua_get_ITEM_DATA_unsigned_m_byUseInArena(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byUseInArena'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byUseInArena);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byUseInArena of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byUseInArena
static int tolua_set_ITEM_DATA_unsigned_m_byUseInArena(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byUseInArena'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byUseInArena = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byRequireLevel of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byRequireLevel
static int tolua_get_ITEM_DATA_unsigned_m_byRequireLevel(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byRequireLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byRequireLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byRequireLevel of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byRequireLevel
static int tolua_set_ITEM_DATA_unsigned_m_byRequireLevel(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byRequireLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byRequireLevel = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byRequireClass of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byRequireClass
static int tolua_get_ITEM_DATA_unsigned_m_byRequireClass(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byRequireClass'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byRequireClass);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byRequireClass of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byRequireClass
static int tolua_set_ITEM_DATA_unsigned_m_byRequireClass(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byRequireClass'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byRequireClass = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byRequireGender of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byRequireGender
static int tolua_get_ITEM_DATA_unsigned_m_byRequireGender(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byRequireGender'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byRequireGender);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byRequireGender of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byRequireGender
static int tolua_set_ITEM_DATA_unsigned_m_byRequireGender(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byRequireGender'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byRequireGender = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byMaxEquip of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byMaxEquip
static int tolua_get_ITEM_DATA_unsigned_m_byMaxEquip(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byMaxEquip'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byMaxEquip);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byMaxEquip of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byMaxEquip
static int tolua_set_ITEM_DATA_unsigned_m_byMaxEquip(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byMaxEquip'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byMaxEquip = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMaxOwn of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nMaxOwn
static int tolua_get_ITEM_DATA_m_nMaxOwn(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMaxOwn'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMaxOwn);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMaxOwn of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nMaxOwn
static int tolua_set_ITEM_DATA_m_nMaxOwn(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMaxOwn'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMaxOwn = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byColor of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byColor
static int tolua_get_ITEM_DATA_unsigned_m_byColor(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byColor'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byColor);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byColor of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byColor
static int tolua_set_ITEM_DATA_unsigned_m_byColor(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byColor'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byColor = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byBindType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byBindType
static int tolua_get_ITEM_DATA_unsigned_m_byBindType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byBindType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byBindType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byBindType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byBindType
static int tolua_set_ITEM_DATA_unsigned_m_byBindType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byBindType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byBindType = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byDurType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byDurType
static int tolua_get_ITEM_DATA_unsigned_m_byDurType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byDurType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byDurType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byDurType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byDurType
static int tolua_set_ITEM_DATA_unsigned_m_byDurType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byDurType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byDurType = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nDur of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nDur
static int tolua_get_ITEM_DATA_m_nDur(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDur'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nDur);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nDur of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nDur
static int tolua_set_ITEM_DATA_m_nDur(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDur'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nDur = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nWeight of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nWeight
static int tolua_get_ITEM_DATA_m_nWeight(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nWeight'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nWeight);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nWeight of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nWeight
static int tolua_set_ITEM_DATA_m_nWeight(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nWeight'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nWeight = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byCanDelete of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byCanDelete
static int tolua_get_ITEM_DATA_unsigned_m_byCanDelete(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byCanDelete'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byCanDelete);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byCanDelete of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byCanDelete
static int tolua_set_ITEM_DATA_unsigned_m_byCanDelete(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byCanDelete'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byCanDelete = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byCanStorage of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byCanStorage
static int tolua_get_ITEM_DATA_unsigned_m_byCanStorage(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byCanStorage'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byCanStorage);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byCanStorage of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byCanStorage
static int tolua_set_ITEM_DATA_unsigned_m_byCanStorage(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byCanStorage'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byCanStorage = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byCanSell of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byCanSell
static int tolua_get_ITEM_DATA_unsigned_m_byCanSell(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byCanSell'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byCanSell);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byCanSell of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byCanSell
static int tolua_set_ITEM_DATA_unsigned_m_byCanSell(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byCanSell'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byCanSell = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nSellPrice of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nSellPrice
static int tolua_get_ITEM_DATA_m_nSellPrice(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSellPrice'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nSellPrice);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nSellPrice of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nSellPrice
static int tolua_set_ITEM_DATA_m_nSellPrice(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSellPrice'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nSellPrice = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nSpellID of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nSpellID
static int tolua_get_ITEM_DATA_m_nSpellID(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSpellID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nSpellID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nSpellID of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nSpellID
static int tolua_set_ITEM_DATA_m_nSpellID(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSpellID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nSpellID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nSpellLevel of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nSpellLevel
static int tolua_get_ITEM_DATA_m_nSpellLevel(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSpellLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nSpellLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nSpellLevel of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nSpellLevel
static int tolua_set_ITEM_DATA_m_nSpellLevel(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSpellLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nSpellLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nSelfCoolDown of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nSelfCoolDown
static int tolua_get_ITEM_DATA_m_nSelfCoolDown(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSelfCoolDown'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nSelfCoolDown);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nSelfCoolDown of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nSelfCoolDown
static int tolua_set_ITEM_DATA_m_nSelfCoolDown(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSelfCoolDown'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nSelfCoolDown = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nHandbookType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nHandbookType
static int tolua_get_ITEM_DATA_m_nHandbookType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nHandbookType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nHandbookType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nHandbookType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nHandbookType
static int tolua_set_ITEM_DATA_m_nHandbookType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nHandbookType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nHandbookType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nHandbookIndex of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nHandbookIndex
static int tolua_get_ITEM_DATA_m_nHandbookIndex(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nHandbookIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nHandbookIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nHandbookIndex of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nHandbookIndex
static int tolua_set_ITEM_DATA_m_nHandbookIndex(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nHandbookIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nHandbookIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nCdGrpID of class  CD_GROUP */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA__CD_GROUP_m_nCdGrpID
static int tolua_get_ITEM_DATA__CD_GROUP_m_nCdGrpID(lua_State* tolua_S)
{
  ITEM_DATA::CD_GROUP* self = (ITEM_DATA::CD_GROUP*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCdGrpID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nCdGrpID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nCdGrpID of class  CD_GROUP */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA__CD_GROUP_m_nCdGrpID
static int tolua_set_ITEM_DATA__CD_GROUP_m_nCdGrpID(lua_State* tolua_S)
{
  ITEM_DATA::CD_GROUP* self = (ITEM_DATA::CD_GROUP*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCdGrpID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nCdGrpID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nCdGrpValue of class  CD_GROUP */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA__CD_GROUP_m_nCdGrpValue
static int tolua_get_ITEM_DATA__CD_GROUP_m_nCdGrpValue(lua_State* tolua_S)
{
  ITEM_DATA::CD_GROUP* self = (ITEM_DATA::CD_GROUP*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCdGrpValue'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nCdGrpValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nCdGrpValue of class  CD_GROUP */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA__CD_GROUP_m_nCdGrpValue
static int tolua_set_ITEM_DATA__CD_GROUP_m_nCdGrpValue(lua_State* tolua_S)
{
  ITEM_DATA::CD_GROUP* self = (ITEM_DATA::CD_GROUP*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCdGrpValue'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nCdGrpValue = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nCdGrpCount of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nCdGrpCount
static int tolua_get_ITEM_DATA_m_nCdGrpCount(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCdGrpCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nCdGrpCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nCdGrpCount of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nCdGrpCount
static int tolua_set_ITEM_DATA_m_nCdGrpCount(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCdGrpCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nCdGrpCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_CdGrp of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ITEM_DATA_m_CdGrp
static int tolua_get_data_manager_ITEM_DATA_m_CdGrp(lua_State* tolua_S)
{
 int tolua_index;
  ITEM_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ITEM_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_CdGrp, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CD_GROUP)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)&self->m_CdGrp[tolua_index],"ITEM_DATA::CD_GROUP");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_CdGrp of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ITEM_DATA_m_CdGrp
static int tolua_set_data_manager_ITEM_DATA_m_CdGrp(lua_State* tolua_S)
{
 int tolua_index;
  ITEM_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ITEM_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_CdGrp, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CD_GROUP)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_CdGrp[tolua_index] = *((ITEM_DATA::CD_GROUP*)  tolua_tousertype(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nDropLimitDuration of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ITEM_DATA_m_nDropLimitDuration
static int tolua_get_data_manager_ITEM_DATA_m_nDropLimitDuration(lua_State* tolua_S)
{
 int tolua_index;
  ITEM_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ITEM_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nDropLimitDuration, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nDropLimitDuration[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nDropLimitDuration of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ITEM_DATA_m_nDropLimitDuration
static int tolua_set_data_manager_ITEM_DATA_m_nDropLimitDuration(lua_State* tolua_S)
{
 int tolua_index;
  ITEM_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ITEM_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nDropLimitDuration, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_nDropLimitDuration[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nDropLimitCount of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ITEM_DATA_m_nDropLimitCount
static int tolua_get_data_manager_ITEM_DATA_m_nDropLimitCount(lua_State* tolua_S)
{
 int tolua_index;
  ITEM_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ITEM_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nDropLimitCount, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nDropLimitCount[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nDropLimitCount of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ITEM_DATA_m_nDropLimitCount
static int tolua_set_data_manager_ITEM_DATA_m_nDropLimitCount(lua_State* tolua_S)
{
 int tolua_index;
  ITEM_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ITEM_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nDropLimitCount, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_nDropLimitCount[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nDropFromServerStart of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nDropFromServerStart
static int tolua_get_ITEM_DATA_m_nDropFromServerStart(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDropFromServerStart'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nDropFromServerStart);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nDropFromServerStart of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nDropFromServerStart
static int tolua_set_ITEM_DATA_m_nDropFromServerStart(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDropFromServerStart'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nDropFromServerStart = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byLog of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byLog
static int tolua_get_ITEM_DATA_unsigned_m_byLog(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byLog'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byLog);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byLog of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byLog
static int tolua_set_ITEM_DATA_unsigned_m_byLog(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byLog'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byLog = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byStat of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byStat
static int tolua_get_ITEM_DATA_unsigned_m_byStat(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byStat'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byStat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byStat of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byStat
static int tolua_set_ITEM_DATA_unsigned_m_byStat(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byStat'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byStat = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byExpensiveItem of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byExpensiveItem
static int tolua_get_ITEM_DATA_unsigned_m_byExpensiveItem(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byExpensiveItem'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byExpensiveItem);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byExpensiveItem of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byExpensiveItem
static int tolua_set_ITEM_DATA_unsigned_m_byExpensiveItem(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byExpensiveItem'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byExpensiveItem = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byDropBroadcast of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byDropBroadcast
static int tolua_get_ITEM_DATA_unsigned_m_byDropBroadcast(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byDropBroadcast'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byDropBroadcast);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byDropBroadcast of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byDropBroadcast
static int tolua_set_ITEM_DATA_unsigned_m_byDropBroadcast(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byDropBroadcast'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byDropBroadcast = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_bPkDrop of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_bPkDrop
static int tolua_get_ITEM_DATA_unsigned_m_bPkDrop(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bPkDrop'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_bPkDrop);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_bPkDrop of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_bPkDrop
static int tolua_set_ITEM_DATA_unsigned_m_bPkDrop(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bPkDrop'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_bPkDrop = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_bTeamAuction of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_bTeamAuction
static int tolua_get_ITEM_DATA_unsigned_m_bTeamAuction(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bTeamAuction'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_bTeamAuction);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_bTeamAuction of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_bTeamAuction
static int tolua_set_ITEM_DATA_unsigned_m_bTeamAuction(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bTeamAuction'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_bTeamAuction = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_bAuction of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_bAuction
static int tolua_get_ITEM_DATA_unsigned_m_bAuction(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bAuction'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_bAuction);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_bAuction of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_bAuction
static int tolua_set_ITEM_DATA_unsigned_m_bAuction(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bAuction'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_bAuction = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nAuctionFee of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ITEM_DATA_m_nAuctionFee
static int tolua_get_data_manager_ITEM_DATA_m_nAuctionFee(lua_State* tolua_S)
{
 int tolua_index;
  ITEM_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ITEM_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nAuctionFee, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_AUCTION_TIME_INTERVAL_TYPE)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nAuctionFee[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nAuctionFee of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ITEM_DATA_m_nAuctionFee
static int tolua_set_data_manager_ITEM_DATA_m_nAuctionFee(lua_State* tolua_S)
{
 int tolua_index;
  ITEM_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ITEM_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nAuctionFee, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_AUCTION_TIME_INTERVAL_TYPE)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_nAuctionFee[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byAuctionType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_unsigned_m_byAuctionType
static int tolua_get_ITEM_DATA_unsigned_m_byAuctionType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byAuctionType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byAuctionType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byAuctionType of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_unsigned_m_byAuctionType
static int tolua_set_ITEM_DATA_unsigned_m_byAuctionType(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byAuctionType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byAuctionType = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nDefaultAuctionPrice of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nDefaultAuctionPrice
static int tolua_get_ITEM_DATA_m_nDefaultAuctionPrice(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDefaultAuctionPrice'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nDefaultAuctionPrice);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nDefaultAuctionPrice of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nDefaultAuctionPrice
static int tolua_set_ITEM_DATA_m_nDefaultAuctionPrice(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDefaultAuctionPrice'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nDefaultAuctionPrice = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nPlayerDropTime of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nPlayerDropTime
static int tolua_get_ITEM_DATA_m_nPlayerDropTime(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nPlayerDropTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nPlayerDropTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nPlayerDropTime of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nPlayerDropTime
static int tolua_set_ITEM_DATA_m_nPlayerDropTime(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nPlayerDropTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nPlayerDropTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nDropGroupID of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nDropGroupID
static int tolua_get_ITEM_DATA_m_nDropGroupID(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDropGroupID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nDropGroupID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nDropGroupID of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nDropGroupID
static int tolua_set_ITEM_DATA_m_nDropGroupID(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDropGroupID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nDropGroupID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nShopTypeSell of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nShopTypeSell
static int tolua_get_ITEM_DATA_m_nShopTypeSell(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nShopTypeSell'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nShopTypeSell);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nShopTypeSell of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nShopTypeSell
static int tolua_set_ITEM_DATA_m_nShopTypeSell(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nShopTypeSell'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nShopTypeSell = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nCanDecompose of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nCanDecompose
static int tolua_get_ITEM_DATA_m_nCanDecompose(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCanDecompose'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nCanDecompose);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nCanDecompose of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nCanDecompose
static int tolua_set_ITEM_DATA_m_nCanDecompose(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCanDecompose'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nCanDecompose = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nTradeNeedIntimacy of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nTradeNeedIntimacy
static int tolua_get_ITEM_DATA_m_nTradeNeedIntimacy(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTradeNeedIntimacy'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nTradeNeedIntimacy);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nTradeNeedIntimacy of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nTradeNeedIntimacy
static int tolua_set_ITEM_DATA_m_nTradeNeedIntimacy(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTradeNeedIntimacy'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nTradeNeedIntimacy = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nVarIndex of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nVarIndex
static int tolua_get_ITEM_DATA_m_nVarIndex(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nVarIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nVarIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nVarIndex of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nVarIndex
static int tolua_set_ITEM_DATA_m_nVarIndex(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nVarIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nVarIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nClearGambleCount of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nClearGambleCount
static int tolua_get_ITEM_DATA_m_nClearGambleCount(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nClearGambleCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nClearGambleCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nClearGambleCount of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nClearGambleCount
static int tolua_set_ITEM_DATA_m_nClearGambleCount(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nClearGambleCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nClearGambleCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nTradeOverWeight of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nTradeOverWeight
static int tolua_get_ITEM_DATA_m_nTradeOverWeight(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTradeOverWeight'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nTradeOverWeight);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nTradeOverWeight of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nTradeOverWeight
static int tolua_set_ITEM_DATA_m_nTradeOverWeight(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTradeOverWeight'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nTradeOverWeight = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nResource of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_nResource
static int tolua_get_ITEM_DATA_m_nResource(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nResource'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nResource);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nResource of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_nResource
static int tolua_set_ITEM_DATA_m_nResource(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nResource'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nResource = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_szScript of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ITEM_DATA_m_szScript
static int tolua_get_ITEM_DATA_m_szScript(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szScript'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->m_szScript, ClientFixedString::Length(self->m_szScript));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_szScript of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ITEM_DATA_m_szScript
static int tolua_set_ITEM_DATA_m_szScript(lua_State* tolua_S)
{
  ITEM_DATA* self = (ITEM_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szScript'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->m_szScript, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  ITEM_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_ITEM_DATA_ReadLine00
static int tolua_data_manager_ITEM_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"ITEM_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"ITEM_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  ITEM_DATA* pData = ((ITEM_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
ITEM_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GetItemData */
#ifndef TOLUA_DISABLE_tolua_data_manager_GetItemData00
static int tolua_data_manager_GetItemData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int nTemplateID = (( signed int)  tolua_tonumber(tolua_S,1,0));
 {
  const ITEM_DATA* tolua_ret = (const ITEM_DATA*)  GetItemData(nTemplateID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ITEM_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetItemData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nLevel of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nLevel
static int tolua_get_ROLE_LEVEL_DATA_m_nLevel(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nLevel of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nLevel
static int tolua_set_ROLE_LEVEL_DATA_m_nLevel(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMaxExp of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nMaxExp
static int tolua_get_ROLE_LEVEL_DATA_m_nMaxExp(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMaxExp'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMaxExp);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMaxExp of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nMaxExp
static int tolua_set_ROLE_LEVEL_DATA_m_nMaxExp(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMaxExp'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMaxExp = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nPow of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nPow
static int tolua_get_ROLE_LEVEL_DATA_m_nPow(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nPow'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nPow);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nPow of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nPow
static int tolua_set_ROLE_LEVEL_DATA_m_nPow(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nPow'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nPow = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMoneyType01DailyUseLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nMoneyType01DailyUseLimit
static int tolua_get_ROLE_LEVEL_DATA_m_nMoneyType01DailyUseLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyType01DailyUseLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMoneyType01DailyUseLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMoneyType01DailyUseLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nMoneyType01DailyUseLimit
static int tolua_set_ROLE_LEVEL_DATA_m_nMoneyType01DailyUseLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyType01DailyUseLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMoneyType01DailyUseLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMoneyType02DailyEarnLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nMoneyType02DailyEarnLimit
static int tolua_get_ROLE_LEVEL_DATA_m_nMoneyType02DailyEarnLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyType02DailyEarnLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMoneyType02DailyEarnLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMoneyType02DailyEarnLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nMoneyType02DailyEarnLimit
static int tolua_set_ROLE_LEVEL_DATA_m_nMoneyType02DailyEarnLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyType02DailyEarnLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMoneyType02DailyEarnLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMoneyTypeBFTotalLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeBFTotalLimit
static int tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeBFTotalLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyTypeBFTotalLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMoneyTypeBFTotalLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMoneyTypeBFTotalLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeBFTotalLimit
static int tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeBFTotalLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyTypeBFTotalLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMoneyTypeBFTotalLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMoneyTypeBFWeeklyEarnLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeBFWeeklyEarnLimit
static int tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeBFWeeklyEarnLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyTypeBFWeeklyEarnLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMoneyTypeBFWeeklyEarnLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMoneyTypeBFWeeklyEarnLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeBFWeeklyEarnLimit
static int tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeBFWeeklyEarnLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyTypeBFWeeklyEarnLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMoneyTypeBFWeeklyEarnLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMoneyTypeArenaTotalLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeArenaTotalLimit
static int tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeArenaTotalLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyTypeArenaTotalLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMoneyTypeArenaTotalLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMoneyTypeArenaTotalLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeArenaTotalLimit
static int tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeArenaTotalLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyTypeArenaTotalLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMoneyTypeArenaTotalLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMoneyTypeArenaWeeklyEarnLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeArenaWeeklyEarnLimit
static int tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeArenaWeeklyEarnLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyTypeArenaWeeklyEarnLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMoneyTypeArenaWeeklyEarnLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMoneyTypeArenaWeeklyEarnLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeArenaWeeklyEarnLimit
static int tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeArenaWeeklyEarnLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMoneyTypeArenaWeeklyEarnLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMoneyTypeArenaWeeklyEarnLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nDailyExpLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nDailyExpLimit
static int tolua_get_ROLE_LEVEL_DATA_m_nDailyExpLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDailyExpLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nDailyExpLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nDailyExpLimit of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nDailyExpLimit
static int tolua_set_ROLE_LEVEL_DATA_m_nDailyExpLimit(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDailyExpLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nDailyExpLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMaxExtraExp of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nMaxExtraExp
static int tolua_get_ROLE_LEVEL_DATA_m_nMaxExtraExp(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMaxExtraExp'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMaxExtraExp);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMaxExtraExp of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nMaxExtraExp
static int tolua_set_ROLE_LEVEL_DATA_m_nMaxExtraExp(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMaxExtraExp'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMaxExtraExp = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nDailyExtraExp of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nDailyExtraExp
static int tolua_get_ROLE_LEVEL_DATA_m_nDailyExtraExp(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDailyExtraExp'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nDailyExtraExp);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nDailyExtraExp of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nDailyExtraExp
static int tolua_set_ROLE_LEVEL_DATA_m_nDailyExtraExp(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nDailyExtraExp'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nDailyExtraExp = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nGoldPoolPer5Min of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nGoldPoolPer5Min
static int tolua_get_ROLE_LEVEL_DATA_m_nGoldPoolPer5Min(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nGoldPoolPer5Min'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nGoldPoolPer5Min);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nGoldPoolPer5Min of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nGoldPoolPer5Min
static int tolua_set_ROLE_LEVEL_DATA_m_nGoldPoolPer5Min(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nGoldPoolPer5Min'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nGoldPoolPer5Min = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nExerciseExpPerHour of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nExerciseExpPerHour
static int tolua_get_ROLE_LEVEL_DATA_m_nExerciseExpPerHour(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nExerciseExpPerHour'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nExerciseExpPerHour);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nExerciseExpPerHour of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nExerciseExpPerHour
static int tolua_set_ROLE_LEVEL_DATA_m_nExerciseExpPerHour(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nExerciseExpPerHour'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nExerciseExpPerHour = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nOnlineExtraExpPer5Min of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_LEVEL_DATA_m_nOnlineExtraExpPer5Min
static int tolua_get_ROLE_LEVEL_DATA_m_nOnlineExtraExpPer5Min(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nOnlineExtraExpPer5Min'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nOnlineExtraExpPer5Min);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nOnlineExtraExpPer5Min of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_LEVEL_DATA_m_nOnlineExtraExpPer5Min
static int tolua_set_ROLE_LEVEL_DATA_m_nOnlineExtraExpPer5Min(lua_State* tolua_S)
{
  ROLE_LEVEL_DATA* self = (ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nOnlineExtraExpPer5Min'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nOnlineExtraExpPer5Min = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  ROLE_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_ROLE_LEVEL_DATA_ReadLine00
static int tolua_data_manager_ROLE_LEVEL_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"ROLE_LEVEL_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"ROLE_LEVEL_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  ROLE_LEVEL_DATA* pData = ((ROLE_LEVEL_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
ROLE_LEVEL_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nLevel of class  HERO_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_HERO_LEVEL_DATA_m_nLevel
static int tolua_get_HERO_LEVEL_DATA_m_nLevel(lua_State* tolua_S)
{
  HERO_LEVEL_DATA* self = (HERO_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nLevel of class  HERO_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_HERO_LEVEL_DATA_m_nLevel
static int tolua_set_HERO_LEVEL_DATA_m_nLevel(lua_State* tolua_S)
{
  HERO_LEVEL_DATA* self = (HERO_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMaxExp of class  HERO_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_HERO_LEVEL_DATA_m_nMaxExp
static int tolua_get_HERO_LEVEL_DATA_m_nMaxExp(lua_State* tolua_S)
{
  HERO_LEVEL_DATA* self = (HERO_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMaxExp'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMaxExp);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMaxExp of class  HERO_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_HERO_LEVEL_DATA_m_nMaxExp
static int tolua_set_HERO_LEVEL_DATA_m_nMaxExp(lua_State* tolua_S)
{
  HERO_LEVEL_DATA* self = (HERO_LEVEL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMaxExp'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMaxExp = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  HERO_LEVEL_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_HERO_LEVEL_DATA_ReadLine00
static int tolua_data_manager_HERO_LEVEL_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"HERO_LEVEL_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"HERO_LEVEL_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  HERO_LEVEL_DATA* pData = ((HERO_LEVEL_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
HERO_LEVEL_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GetMaxExp */
#ifndef TOLUA_DISABLE_tolua_data_manager_GetMaxExp00
static int tolua_data_manager_GetMaxExp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int nObjType = (( signed int)  tolua_tonumber(tolua_S,1,0));
  signed int nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0));
 {
  signed int tolua_ret = ( signed int)  GetMaxExp(nObjType,nLevel);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaxExp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nTemplateID of class  MISSILE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_MISSILE_DATA_m_nTemplateID
static int tolua_get_MISSILE_DATA_m_nTemplateID(lua_State* tolua_S)
{
  MISSILE_DATA* self = (MISSILE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nTemplateID of class  MISSILE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_MISSILE_DATA_m_nTemplateID
static int tolua_set_MISSILE_DATA_m_nTemplateID(lua_State* tolua_S)
{
  MISSILE_DATA* self = (MISSILE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nTemplateID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nAttr of class  MISSILE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_MISSILE_DATA_m_nAttr
static int tolua_get_data_manager_MISSILE_DATA_m_nAttr(lua_State* tolua_S)
{
 int tolua_index;
  MISSILE_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (MISSILE_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nAttr, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=matTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nAttr[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nAttr of class  MISSILE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_MISSILE_DATA_m_nAttr
static int tolua_set_data_manager_MISSILE_DATA_m_nAttr(lua_State* tolua_S)
{
 int tolua_index;
  MISSILE_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (MISSILE_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nAttr, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=matTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_nAttr[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  MISSILE_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_MISSILE_DATA_ReadLine00
static int tolua_data_manager_MISSILE_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"MISSILE_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"MISSILE_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  MISSILE_DATA* pData = ((MISSILE_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
MISSILE_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTemplateID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nTemplateID
static int tolua_get_NPC_DATA_nTemplateID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTemplateID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nTemplateID
static int tolua_set_NPC_DATA_nTemplateID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTemplateID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nClientTemplateID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nClientTemplateID
static int tolua_get_NPC_DATA_nClientTemplateID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nClientTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nClientTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nClientTemplateID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nClientTemplateID
static int tolua_set_NPC_DATA_nClientTemplateID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nClientTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nClientTemplateID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szDisplayName of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_szDisplayName
static int tolua_get_NPC_DATA_szDisplayName(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szDisplayName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szDisplayName, ClientFixedString::Length(self->szDisplayName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szDisplayName of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_szDisplayName
static int tolua_set_NPC_DATA_szDisplayName(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szDisplayName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szDisplayName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szName of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_szName
static int tolua_get_NPC_DATA_szName(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szName, ClientFixedString::Length(self->szName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szName of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_szName
static int tolua_set_NPC_DATA_szName(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLevel of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nLevel
static int tolua_get_NPC_DATA_nLevel(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLevel of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nLevel
static int tolua_set_NPC_DATA_nLevel(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nPatternID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nPatternID
static int tolua_get_NPC_DATA_nPatternID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPatternID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nPatternID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nPatternID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nPatternID
static int tolua_set_NPC_DATA_nPatternID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPatternID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nPatternID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nQuality of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nQuality
static int tolua_get_NPC_DATA_nQuality(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nQuality'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nQuality);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nQuality of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nQuality
static int tolua_set_NPC_DATA_nQuality(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nQuality'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nQuality = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBroadcast of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nBroadcast
static int tolua_get_NPC_DATA_nBroadcast(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBroadcast'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBroadcast);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBroadcast of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nBroadcast
static int tolua_set_NPC_DATA_nBroadcast(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBroadcast'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBroadcast = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nForceID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nForceID
static int tolua_get_NPC_DATA_nForceID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nForceID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nForceID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nForceID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nForceID
static int tolua_set_NPC_DATA_nForceID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nForceID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nForceID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bReduceReputation of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bReduceReputation
static int tolua_get_NPC_DATA_bReduceReputation(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bReduceReputation'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bReduceReputation);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bReduceReputation of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bReduceReputation
static int tolua_set_NPC_DATA_bReduceReputation(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bReduceReputation'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bReduceReputation = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSpecies of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nSpecies
static int tolua_get_NPC_DATA_nSpecies(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpecies'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSpecies);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSpecies of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nSpecies
static int tolua_set_NPC_DATA_nSpecies(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpecies'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSpecies = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRivalForceID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nRivalForceID
static int tolua_get_NPC_DATA_nRivalForceID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRivalForceID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRivalForceID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRivalForceID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nRivalForceID
static int tolua_set_NPC_DATA_nRivalForceID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRivalForceID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRivalForceID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nGroupID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nGroupID
static int tolua_get_NPC_DATA_nGroupID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nGroupID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nGroupID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nGroupID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nGroupID
static int tolua_set_NPC_DATA_nGroupID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nGroupID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nGroupID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBodyType of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nBodyType
static int tolua_get_NPC_DATA_nBodyType(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBodyType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBodyType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBodyType of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nBodyType
static int tolua_set_NPC_DATA_nBodyType(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBodyType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBodyType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSpellEffectGroup of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nSpellEffectGroup
static int tolua_get_NPC_DATA_nSpellEffectGroup(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpellEffectGroup'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSpellEffectGroup);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSpellEffectGroup of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nSpellEffectGroup
static int tolua_set_NPC_DATA_nSpellEffectGroup(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpellEffectGroup'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSpellEffectGroup = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSpawnTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nSpawnTime
static int tolua_get_NPC_DATA_nSpawnTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpawnTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSpawnTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSpawnTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nSpawnTime
static int tolua_set_NPC_DATA_nSpawnTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSpawnTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSpawnTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nReviveTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nReviveTime
static int tolua_get_NPC_DATA_nReviveTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReviveTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nReviveTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nReviveTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nReviveTime
static int tolua_set_NPC_DATA_nReviveTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReviveTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nReviveTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCorpseTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nCorpseTime
static int tolua_get_NPC_DATA_nCorpseTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCorpseTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCorpseTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCorpseTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nCorpseTime
static int tolua_set_NPC_DATA_nCorpseTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCorpseTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCorpseTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMinRespawnTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nMinRespawnTime
static int tolua_get_NPC_DATA_nMinRespawnTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinRespawnTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMinRespawnTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMinRespawnTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nMinRespawnTime
static int tolua_set_NPC_DATA_nMinRespawnTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinRespawnTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMinRespawnTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxRespawnTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nMaxRespawnTime
static int tolua_get_NPC_DATA_nMaxRespawnTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxRespawnTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxRespawnTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxRespawnTime of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nMaxRespawnTime
static int tolua_set_NPC_DATA_nMaxRespawnTime(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxRespawnTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxRespawnTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAIID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nAIID
static int tolua_get_NPC_DATA_nAIID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAIID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAIID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAIID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nAIID
static int tolua_set_NPC_DATA_nAIID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAIID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAIID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bImmunePassiveMove of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bImmunePassiveMove
static int tolua_get_NPC_DATA_bImmunePassiveMove(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bImmunePassiveMove'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bImmunePassiveMove);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bImmunePassiveMove of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bImmunePassiveMove
static int tolua_set_NPC_DATA_bImmunePassiveMove(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bImmunePassiveMove'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bImmunePassiveMove = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInvincible of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInvincible
static int tolua_get_NPC_DATA_bInvincible(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInvincible'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInvincible);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInvincible of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInvincible
static int tolua_set_NPC_DATA_bInvincible(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInvincible'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInvincible = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bAlwaysRun of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bAlwaysRun
static int tolua_get_NPC_DATA_bAlwaysRun(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bAlwaysRun'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bAlwaysRun);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bAlwaysRun of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bAlwaysRun
static int tolua_set_NPC_DATA_bAlwaysRun(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bAlwaysRun'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bAlwaysRun = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCanbeThrown of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bCanbeThrown
static int tolua_get_NPC_DATA_bCanbeThrown(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanbeThrown'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCanbeThrown);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCanbeThrown of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bCanbeThrown
static int tolua_set_NPC_DATA_bCanbeThrown(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanbeThrown'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCanbeThrown = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bIgnoreInvisible of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bIgnoreInvisible
static int tolua_get_NPC_DATA_bIgnoreInvisible(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bIgnoreInvisible'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bIgnoreInvisible);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bIgnoreInvisible of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bIgnoreInvisible
static int tolua_set_NPC_DATA_bIgnoreInvisible(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bIgnoreInvisible'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bIgnoreInvisible = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCounterInvisible of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bCounterInvisible
static int tolua_get_NPC_DATA_bCounterInvisible(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCounterInvisible'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCounterInvisible);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCounterInvisible of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bCounterInvisible
static int tolua_set_NPC_DATA_bCounterInvisible(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCounterInvisible'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCounterInvisible = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAttackRange of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nAttackRange
static int tolua_get_NPC_DATA_nAttackRange(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAttackRange'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAttackRange);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAttackRange of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nAttackRange
static int tolua_set_NPC_DATA_nAttackRange(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAttackRange'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAttackRange = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bExclusive of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bExclusive
static int tolua_get_NPC_DATA_bExclusive(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bExclusive'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bExclusive);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bExclusive of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bExclusive
static int tolua_set_NPC_DATA_bExclusive(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bExclusive'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bExclusive = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bNeverBeHit of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bNeverBeHit
static int tolua_get_NPC_DATA_bNeverBeHit(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bNeverBeHit'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bNeverBeHit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bNeverBeHit of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bNeverBeHit
static int tolua_set_NPC_DATA_bNeverBeHit(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bNeverBeHit'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bNeverBeHit = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bProfit of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bProfit
static int tolua_get_NPC_DATA_bProfit(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bProfit'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bProfit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bProfit of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bProfit
static int tolua_set_NPC_DATA_bProfit(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bProfit'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bProfit = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nExpRate of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nExpRate
static int tolua_get_NPC_DATA_nExpRate(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nExpRate'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nExpRate);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nExpRate of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nExpRate
static int tolua_set_NPC_DATA_nExpRate(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nExpRate'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nExpRate = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSoulRate of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nSoulRate
static int tolua_get_NPC_DATA_nSoulRate(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSoulRate'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSoulRate);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSoulRate of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nSoulRate
static int tolua_set_NPC_DATA_nSoulRate(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSoulRate'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSoulRate = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bDailyExp of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bDailyExp
static int tolua_get_NPC_DATA_bDailyExp(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bDailyExp'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bDailyExp);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bDailyExp of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bDailyExp
static int tolua_set_NPC_DATA_bDailyExp(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bDailyExp'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bDailyExp = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bAuraInvalid of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bAuraInvalid
static int tolua_get_NPC_DATA_bAuraInvalid(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bAuraInvalid'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bAuraInvalid);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bAuraInvalid of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bAuraInvalid
static int tolua_set_NPC_DATA_bAuraInvalid(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bAuraInvalid'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bAuraInvalid = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nKillPlayerVarIndex of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nKillPlayerVarIndex
static int tolua_get_NPC_DATA_nKillPlayerVarIndex(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nKillPlayerVarIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nKillPlayerVarIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nKillPlayerVarIndex of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nKillPlayerVarIndex
static int tolua_set_NPC_DATA_nKillPlayerVarIndex(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nKillPlayerVarIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nKillPlayerVarIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBeKilledVarIndex of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nBeKilledVarIndex
static int tolua_get_NPC_DATA_nBeKilledVarIndex(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBeKilledVarIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBeKilledVarIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBeKilledVarIndex of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nBeKilledVarIndex
static int tolua_set_NPC_DATA_nBeKilledVarIndex(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBeKilledVarIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBeKilledVarIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nShopID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nShopID
static int tolua_get_NPC_DATA_nShopID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nShopID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nShopID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nShopID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nShopID
static int tolua_set_NPC_DATA_nShopID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nShopID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nShopID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBlackMarketID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nBlackMarketID
static int tolua_get_NPC_DATA_nBlackMarketID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBlackMarketID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBlackMarketID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBlackMarketID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nBlackMarketID
static int tolua_set_NPC_DATA_nBlackMarketID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBlackMarketID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBlackMarketID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractBank of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractBank
static int tolua_get_NPC_DATA_bInteractBank(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractBank'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractBank);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractBank of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractBank
static int tolua_set_NPC_DATA_bInteractBank(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractBank'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractBank = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractGuildBank of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractGuildBank
static int tolua_get_NPC_DATA_bInteractGuildBank(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractGuildBank'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractGuildBank);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractGuildBank of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractGuildBank
static int tolua_set_NPC_DATA_bInteractGuildBank(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractGuildBank'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractGuildBank = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractRewardQuest of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractRewardQuest
static int tolua_get_NPC_DATA_bInteractRewardQuest(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractRewardQuest'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractRewardQuest);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractRewardQuest of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractRewardQuest
static int tolua_set_NPC_DATA_bInteractRewardQuest(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractRewardQuest'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractRewardQuest = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractCreateGuild of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractCreateGuild
static int tolua_get_NPC_DATA_bInteractCreateGuild(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractCreateGuild'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractCreateGuild);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractCreateGuild of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractCreateGuild
static int tolua_set_NPC_DATA_bInteractCreateGuild(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractCreateGuild'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractCreateGuild = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractGuildSalary of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractGuildSalary
static int tolua_get_NPC_DATA_bInteractGuildSalary(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractGuildSalary'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractGuildSalary);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractGuildSalary of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractGuildSalary
static int tolua_set_NPC_DATA_bInteractGuildSalary(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractGuildSalary'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractGuildSalary = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractAuction of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractAuction
static int tolua_get_NPC_DATA_bInteractAuction(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractAuction'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractAuction);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractAuction of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractAuction
static int tolua_set_NPC_DATA_bInteractAuction(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractAuction'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractAuction = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractRune of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractRune
static int tolua_get_NPC_DATA_bInteractRune(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractRune'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractRune);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractRune of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractRune
static int tolua_set_NPC_DATA_bInteractRune(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractRune'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractRune = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractSoul of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractSoul
static int tolua_get_NPC_DATA_bInteractSoul(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractSoul'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractSoul);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractSoul of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractSoul
static int tolua_set_NPC_DATA_bInteractSoul(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractSoul'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractSoul = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractWeaponLevelUpPutIn of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractWeaponLevelUpPutIn
static int tolua_get_NPC_DATA_bInteractWeaponLevelUpPutIn(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractWeaponLevelUpPutIn'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractWeaponLevelUpPutIn);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractWeaponLevelUpPutIn of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractWeaponLevelUpPutIn
static int tolua_set_NPC_DATA_bInteractWeaponLevelUpPutIn(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractWeaponLevelUpPutIn'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractWeaponLevelUpPutIn = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractWeaponLevelUpTakeOut of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractWeaponLevelUpTakeOut
static int tolua_get_NPC_DATA_bInteractWeaponLevelUpTakeOut(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractWeaponLevelUpTakeOut'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractWeaponLevelUpTakeOut);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractWeaponLevelUpTakeOut of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractWeaponLevelUpTakeOut
static int tolua_set_NPC_DATA_bInteractWeaponLevelUpTakeOut(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractWeaponLevelUpTakeOut'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractWeaponLevelUpTakeOut = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractSpecialRepair of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractSpecialRepair
static int tolua_get_NPC_DATA_bInteractSpecialRepair(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractSpecialRepair'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractSpecialRepair);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractSpecialRepair of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractSpecialRepair
static int tolua_set_NPC_DATA_bInteractSpecialRepair(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractSpecialRepair'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractSpecialRepair = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractGamble of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractGamble
static int tolua_get_NPC_DATA_bInteractGamble(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractGamble'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractGamble);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractGamble of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractGamble
static int tolua_set_NPC_DATA_bInteractGamble(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractGamble'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractGamble = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractExercise of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractExercise
static int tolua_get_NPC_DATA_bInteractExercise(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractExercise'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractExercise);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractExercise of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractExercise
static int tolua_set_NPC_DATA_bInteractExercise(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractExercise'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractExercise = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractSpellBookGamble of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractSpellBookGamble
static int tolua_get_NPC_DATA_bInteractSpellBookGamble(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractSpellBookGamble'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractSpellBookGamble);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractSpellBookGamble of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractSpellBookGamble
static int tolua_set_NPC_DATA_bInteractSpellBookGamble(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractSpellBookGamble'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractSpellBookGamble = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInteractArenaReward of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bInteractArenaReward
static int tolua_get_NPC_DATA_bInteractArenaReward(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractArenaReward'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bInteractArenaReward);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInteractArenaReward of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bInteractArenaReward
static int tolua_set_NPC_DATA_bInteractArenaReward(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInteractArenaReward'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInteractArenaReward = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCorpseDoodadID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nCorpseDoodadID
static int tolua_get_NPC_DATA_nCorpseDoodadID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCorpseDoodadID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCorpseDoodadID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCorpseDoodadID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nCorpseDoodadID
static int tolua_set_NPC_DATA_nCorpseDoodadID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCorpseDoodadID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCorpseDoodadID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bDieIDMonitor of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bDieIDMonitor
static int tolua_get_NPC_DATA_bDieIDMonitor(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bDieIDMonitor'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bDieIDMonitor);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bDieIDMonitor of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bDieIDMonitor
static int tolua_set_NPC_DATA_bDieIDMonitor(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bDieIDMonitor'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bDieIDMonitor = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bStatMoney of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_bStatMoney
static int tolua_get_NPC_DATA_bStatMoney(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bStatMoney'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bStatMoney);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bStatMoney of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_bStatMoney
static int tolua_set_NPC_DATA_bStatMoney(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bStatMoney'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bStatMoney = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBigDropInterval of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nBigDropInterval
static int tolua_get_NPC_DATA_nBigDropInterval(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBigDropInterval'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBigDropInterval);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBigDropInterval of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nBigDropInterval
static int tolua_set_NPC_DATA_nBigDropInterval(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBigDropInterval'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBigDropInterval = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRuneGroup of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nRuneGroup
static int tolua_get_NPC_DATA_nRuneGroup(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRuneGroup'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRuneGroup);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRuneGroup of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nRuneGroup
static int tolua_set_NPC_DATA_nRuneGroup(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRuneGroup'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRuneGroup = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nHandbookType of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nHandbookType
static int tolua_get_NPC_DATA_nHandbookType(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nHandbookType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nHandbookType of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nHandbookType
static int tolua_set_NPC_DATA_nHandbookType(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nHandbookType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nHandbookIndex of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nHandbookIndex
static int tolua_get_NPC_DATA_nHandbookIndex(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nHandbookIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nHandbookIndex of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nHandbookIndex
static int tolua_set_NPC_DATA_nHandbookIndex(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nHandbookIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nKillNpcQuestID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_nKillNpcQuestID
static int tolua_get_NPC_DATA_nKillNpcQuestID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nKillNpcQuestID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nKillNpcQuestID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nKillNpcQuestID of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_nKillNpcQuestID
static int tolua_set_NPC_DATA_nKillNpcQuestID(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nKillNpcQuestID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nKillNpcQuestID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: pAdventureData of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_pAdventureData_ptr
static int tolua_get_NPC_DATA_pAdventureData_ptr(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'pAdventureData'",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)self->pAdventureData,"const ADVENTURE_DATA");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szScriptName of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_DATA_szScriptName
static int tolua_get_NPC_DATA_szScriptName(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szScriptName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szScriptName, ClientFixedString::Length(self->szScriptName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szScriptName of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_DATA_szScriptName
static int tolua_set_NPC_DATA_szScriptName(lua_State* tolua_S)
{
  NPC_DATA* self = (NPC_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szScriptName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szScriptName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  NPC_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_NPC_DATA_ReadLine00
static int tolua_data_manager_NPC_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"NPC_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"NPC_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  NPC_DATA* pData = ((NPC_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
NPC_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nForceID of class  NPC_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_NPC_RELATION_DATA_nForceID
static int tolua_get_NPC_RELATION_DATA_nForceID(lua_State* tolua_S)
{
  NPC_RELATION_DATA* self = (NPC_RELATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nForceID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nForceID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nForceID of class  NPC_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_NPC_RELATION_DATA_nForceID
static int tolua_set_NPC_RELATION_DATA_nForceID(lua_State* tolua_S)
{
  NPC_RELATION_DATA* self = (NPC_RELATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nForceID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nForceID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRelation of class  NPC_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_NPC_RELATION_DATA_nRelation
static int tolua_get_data_manager_NPC_RELATION_DATA_nRelation(lua_State* tolua_S)
{
 int tolua_index;
  NPC_RELATION_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NPC_RELATION_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nRelation, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_NPC_FORCE_ID)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRelation[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRelation of class  NPC_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_NPC_RELATION_DATA_nRelation
static int tolua_set_data_manager_NPC_RELATION_DATA_nRelation(lua_State* tolua_S)
{
 int tolua_index;
  NPC_RELATION_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (NPC_RELATION_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nRelation, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_NPC_FORCE_ID)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nRelation[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  NPC_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_NPC_RELATION_DATA_ReadLine00
static int tolua_data_manager_NPC_RELATION_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"NPC_RELATION_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"NPC_RELATION_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  NPC_RELATION_DATA* pData = ((NPC_RELATION_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
NPC_RELATION_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nQuestID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nQuestID
static int tolua_get_QUEST_DATA_nQuestID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nQuestID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nQuestID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nQuestID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nQuestID
static int tolua_set_QUEST_DATA_nQuestID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nQuestID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nQuestID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: eResetType of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_eResetType
static int tolua_get_QUEST_DATA_eResetType(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'eResetType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->eResetType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: eResetType of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_eResetType
static int tolua_set_QUEST_DATA_eResetType(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'eResetType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->eResetType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nQuestType of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nQuestType
static int tolua_get_QUEST_DATA_nQuestType(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nQuestType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nQuestType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nQuestType of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nQuestType
static int tolua_set_QUEST_DATA_nQuestType(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nQuestType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nQuestType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCircleID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nCircleID
static int tolua_get_QUEST_DATA_nCircleID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCircleID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCircleID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCircleID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nCircleID
static int tolua_set_QUEST_DATA_nCircleID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCircleID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCircleID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nQuality of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nQuality
static int tolua_get_QUEST_DATA_nQuality(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nQuality'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nQuality);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nQuality of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nQuality
static int tolua_set_QUEST_DATA_nQuality(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nQuality'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nQuality = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwResetTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_unsigned_dwResetTime
static int tolua_get_QUEST_DATA_unsigned_dwResetTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwResetTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwResetTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwResetTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_unsigned_dwResetTime
static int tolua_set_QUEST_DATA_unsigned_dwResetTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwResetTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwResetTime = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCanAbandon of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_bCanAbandon
static int tolua_get_QUEST_DATA_bCanAbandon(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanAbandon'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCanAbandon);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCanAbandon of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_bCanAbandon
static int tolua_set_QUEST_DATA_bCanAbandon(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanAbandon'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCanAbandon = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCanShare of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_bCanShare
static int tolua_get_QUEST_DATA_bCanShare(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanShare'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCanShare);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCanShare of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_bCanShare
static int tolua_set_QUEST_DATA_bCanShare(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanShare'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCanShare = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCanPublish of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_bCanPublish
static int tolua_get_QUEST_DATA_bCanPublish(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanPublish'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCanPublish);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCanPublish of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_bCanPublish
static int tolua_set_QUEST_DATA_bCanPublish(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCanPublish'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCanPublish = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nEmergencyCoolDown of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nEmergencyCoolDown
static int tolua_get_QUEST_DATA_nEmergencyCoolDown(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nEmergencyCoolDown'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nEmergencyCoolDown);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nEmergencyCoolDown of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nEmergencyCoolDown
static int tolua_set_QUEST_DATA_nEmergencyCoolDown(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nEmergencyCoolDown'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nEmergencyCoolDown = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nDurationLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_unsigned_nDurationLimit
static int tolua_get_QUEST_DATA_unsigned_nDurationLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nDurationLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nDurationLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nDurationLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_unsigned_nDurationLimit
static int tolua_set_QUEST_DATA_unsigned_nDurationLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nDurationLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nDurationLimit = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_szName
static int tolua_get_QUEST_DATA_szName(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szName, ClientFixedString::Length(self->szName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_szName
static int tolua_set_QUEST_DATA_szName(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSceneID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nSceneID
static int tolua_get_data_manager_QUEST_DATA_nSceneID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSceneID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_FLY_SPOT_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSceneID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSceneID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nSceneID
static int tolua_set_data_manager_QUEST_DATA_nSceneID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nSceneID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_FLY_SPOT_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nSceneID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nX of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nX
static int tolua_get_data_manager_QUEST_DATA_nX(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nX, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_FLY_SPOT_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nX[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nX of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nX
static int tolua_set_data_manager_QUEST_DATA_nX(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nX, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_FLY_SPOT_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nX[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nY of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nY
static int tolua_get_data_manager_QUEST_DATA_nY(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nY, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_FLY_SPOT_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nY[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nY of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nY
static int tolua_set_data_manager_QUEST_DATA_nY(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nY, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_FLY_SPOT_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nY[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nZ of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nZ
static int tolua_get_data_manager_QUEST_DATA_nZ(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nZ, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_FLY_SPOT_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nZ[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nZ of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nZ
static int tolua_set_data_manager_QUEST_DATA_nZ(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nZ, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_FLY_SPOT_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nZ[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxCompletedCount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nMaxCompletedCount
static int tolua_get_QUEST_DATA_nMaxCompletedCount(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxCompletedCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxCompletedCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxCompletedCount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nMaxCompletedCount
static int tolua_set_QUEST_DATA_nMaxCompletedCount(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxCompletedCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxCompletedCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nClassLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nClassLimit
static int tolua_get_QUEST_DATA_nClassLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nClassLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nClassLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nClassLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nClassLimit
static int tolua_set_QUEST_DATA_nClassLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nClassLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nClassLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSexLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nSexLimit
static int tolua_get_QUEST_DATA_nSexLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSexLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSexLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSexLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nSexLimit
static int tolua_set_QUEST_DATA_nSexLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSexLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSexLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRelationLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nRelationLimit
static int tolua_get_QUEST_DATA_nRelationLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRelationLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRelationLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRelationLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nRelationLimit
static int tolua_set_QUEST_DATA_nRelationLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRelationLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRelationLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nPreQuestID1 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nPreQuestID1
static int tolua_get_QUEST_DATA_nPreQuestID1(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreQuestID1'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nPreQuestID1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nPreQuestID1 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nPreQuestID1
static int tolua_set_QUEST_DATA_nPreQuestID1(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreQuestID1'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nPreQuestID1 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nPreQuestID2 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nPreQuestID2
static int tolua_get_QUEST_DATA_nPreQuestID2(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreQuestID2'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nPreQuestID2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nPreQuestID2 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nPreQuestID2
static int tolua_set_QUEST_DATA_nPreQuestID2(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreQuestID2'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nPreQuestID2 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nStartNpcID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nStartNpcID
static int tolua_get_QUEST_DATA_nStartNpcID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStartNpcID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nStartNpcID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nStartNpcID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nStartNpcID
static int tolua_set_QUEST_DATA_nStartNpcID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStartNpcID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nStartNpcID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nStartItemID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nStartItemID
static int tolua_get_QUEST_DATA_nStartItemID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStartItemID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nStartItemID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nStartItemID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nStartItemID
static int tolua_set_QUEST_DATA_nStartItemID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStartItemID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nStartItemID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bStartZone of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_bStartZone
static int tolua_get_QUEST_DATA_bStartZone(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bStartZone'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bStartZone);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bStartZone of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_bStartZone
static int tolua_set_QUEST_DATA_bStartZone(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bStartZone'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bStartZone = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMinLevelLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nMinLevelLimit
static int tolua_get_QUEST_DATA_nMinLevelLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinLevelLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMinLevelLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMinLevelLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nMinLevelLimit
static int tolua_set_QUEST_DATA_nMinLevelLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinLevelLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMinLevelLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxLevelLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nMaxLevelLimit
static int tolua_get_QUEST_DATA_nMaxLevelLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxLevelLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxLevelLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxLevelLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nMaxLevelLimit
static int tolua_set_QUEST_DATA_nMaxLevelLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxLevelLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxLevelLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBuffIDLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nBuffIDLimit
static int tolua_get_QUEST_DATA_nBuffIDLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBuffIDLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBuffIDLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBuffIDLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nBuffIDLimit
static int tolua_set_QUEST_DATA_nBuffIDLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBuffIDLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBuffIDLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBuffLevelLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nBuffLevelLimit
static int tolua_get_QUEST_DATA_nBuffLevelLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBuffLevelLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBuffLevelLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBuffLevelLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nBuffLevelLimit
static int tolua_set_QUEST_DATA_nBuffLevelLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBuffLevelLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBuffLevelLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMoneyTypeLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nMoneyTypeLimit
static int tolua_get_QUEST_DATA_nMoneyTypeLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMoneyTypeLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMoneyTypeLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMoneyTypeLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nMoneyTypeLimit
static int tolua_set_QUEST_DATA_nMoneyTypeLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMoneyTypeLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMoneyTypeLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMoneyAmountLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nMoneyAmountLimit
static int tolua_get_QUEST_DATA_nMoneyAmountLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMoneyAmountLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMoneyAmountLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMoneyAmountLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nMoneyAmountLimit
static int tolua_set_QUEST_DATA_nMoneyAmountLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMoneyAmountLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMoneyAmountLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nReputationForceID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nReputationForceID
static int tolua_get_QUEST_DATA_nReputationForceID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReputationForceID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nReputationForceID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nReputationForceID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nReputationForceID
static int tolua_set_QUEST_DATA_nReputationForceID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReputationForceID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nReputationForceID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nReputationMinLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nReputationMinLimit
static int tolua_get_QUEST_DATA_nReputationMinLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReputationMinLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nReputationMinLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nReputationMinLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nReputationMinLimit
static int tolua_set_QUEST_DATA_nReputationMinLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReputationMinLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nReputationMinLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nReputationMaxLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nReputationMaxLimit
static int tolua_get_QUEST_DATA_nReputationMaxLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReputationMaxLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nReputationMaxLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nReputationMaxLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nReputationMaxLimit
static int tolua_set_QUEST_DATA_nReputationMaxLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReputationMaxLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nReputationMaxLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nYearLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nYearLimit
static int tolua_get_QUEST_DATA_nYearLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nYearLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nYearLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nYearLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nYearLimit
static int tolua_set_QUEST_DATA_nYearLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nYearLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nYearLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMonthLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nMonthLimit
static int tolua_get_QUEST_DATA_nMonthLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMonthLimit'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMonthLimit);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMonthLimit of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nMonthLimit
static int tolua_set_QUEST_DATA_nMonthLimit(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMonthLimit'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMonthLimit = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nStartWeekday of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nStartWeekday
static int tolua_get_QUEST_DATA_nStartWeekday(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStartWeekday'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nStartWeekday);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nStartWeekday of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nStartWeekday
static int tolua_set_QUEST_DATA_nStartWeekday(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStartWeekday'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nStartWeekday = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nWeekdayDuration of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nWeekdayDuration
static int tolua_get_QUEST_DATA_nWeekdayDuration(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWeekdayDuration'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nWeekdayDuration);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nWeekdayDuration of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nWeekdayDuration
static int tolua_set_QUEST_DATA_nWeekdayDuration(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWeekdayDuration'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nWeekdayDuration = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nStartMonthday of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nStartMonthday
static int tolua_get_QUEST_DATA_nStartMonthday(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStartMonthday'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nStartMonthday);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nStartMonthday of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nStartMonthday
static int tolua_set_QUEST_DATA_nStartMonthday(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nStartMonthday'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nStartMonthday = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMonthdayDuration of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nMonthdayDuration
static int tolua_get_QUEST_DATA_nMonthdayDuration(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMonthdayDuration'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMonthdayDuration);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMonthdayDuration of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nMonthdayDuration
static int tolua_set_QUEST_DATA_nMonthdayDuration(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMonthdayDuration'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMonthdayDuration = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwAcceptStartTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_unsigned_dwAcceptStartTime
static int tolua_get_QUEST_DATA_unsigned_dwAcceptStartTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwAcceptStartTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwAcceptStartTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwAcceptStartTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_unsigned_dwAcceptStartTime
static int tolua_set_QUEST_DATA_unsigned_dwAcceptStartTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwAcceptStartTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwAcceptStartTime = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwAcceptEndTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_unsigned_dwAcceptEndTime
static int tolua_get_QUEST_DATA_unsigned_dwAcceptEndTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwAcceptEndTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwAcceptEndTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwAcceptEndTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_unsigned_dwAcceptEndTime
static int tolua_set_QUEST_DATA_unsigned_dwAcceptEndTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwAcceptEndTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwAcceptEndTime = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAcceptStartHour of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nAcceptStartHour
static int tolua_get_QUEST_DATA_nAcceptStartHour(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptStartHour'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAcceptStartHour);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAcceptStartHour of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nAcceptStartHour
static int tolua_set_QUEST_DATA_nAcceptStartHour(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptStartHour'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAcceptStartHour = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAcceptStartMin of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nAcceptStartMin
static int tolua_get_QUEST_DATA_nAcceptStartMin(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptStartMin'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAcceptStartMin);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAcceptStartMin of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nAcceptStartMin
static int tolua_set_QUEST_DATA_nAcceptStartMin(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptStartMin'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAcceptStartMin = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAcceptEndHour of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nAcceptEndHour
static int tolua_get_QUEST_DATA_nAcceptEndHour(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptEndHour'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAcceptEndHour);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAcceptEndHour of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nAcceptEndHour
static int tolua_set_QUEST_DATA_nAcceptEndHour(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptEndHour'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAcceptEndHour = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAcceptEndMin of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nAcceptEndMin
static int tolua_get_QUEST_DATA_nAcceptEndMin(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptEndMin'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAcceptEndMin);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAcceptEndMin of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nAcceptEndMin
static int tolua_set_QUEST_DATA_nAcceptEndMin(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptEndMin'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAcceptEndMin = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAcceptStartDayTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nAcceptStartDayTime
static int tolua_get_QUEST_DATA_nAcceptStartDayTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptStartDayTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAcceptStartDayTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAcceptStartDayTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nAcceptStartDayTime
static int tolua_set_QUEST_DATA_nAcceptStartDayTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptStartDayTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAcceptStartDayTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAcceptEndDayTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nAcceptEndDayTime
static int tolua_get_QUEST_DATA_nAcceptEndDayTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptEndDayTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAcceptEndDayTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAcceptEndDayTime of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nAcceptEndDayTime
static int tolua_set_QUEST_DATA_nAcceptEndDayTime(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAcceptEndDayTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAcceptEndDayTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: vecItemLimitID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_vecItemLimitID
static int tolua_get_data_manager_QUEST_DATA_vecItemLimitID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecItemLimitID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=4)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->vecItemLimitID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: vecItemLimitID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_vecItemLimitID
static int tolua_set_data_manager_QUEST_DATA_vecItemLimitID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecItemLimitID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=4)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->vecItemLimitID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: vecItemLimitCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_vecItemLimitCnt
static int tolua_get_data_manager_QUEST_DATA_vecItemLimitCnt(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecItemLimitCnt, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=4)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->vecItemLimitCnt[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: vecItemLimitCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_vecItemLimitCnt
static int tolua_set_data_manager_QUEST_DATA_vecItemLimitCnt(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecItemLimitCnt, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=4)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->vecItemLimitCnt[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLimitVarID1 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nLimitVarID1
static int tolua_get_QUEST_DATA_nLimitVarID1(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLimitVarID1'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLimitVarID1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLimitVarID1 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nLimitVarID1
static int tolua_set_QUEST_DATA_nLimitVarID1(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLimitVarID1'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLimitVarID1 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLimitVarValue1 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nLimitVarValue1
static int tolua_get_QUEST_DATA_nLimitVarValue1(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLimitVarValue1'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLimitVarValue1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLimitVarValue1 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nLimitVarValue1
static int tolua_set_QUEST_DATA_nLimitVarValue1(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLimitVarValue1'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLimitVarValue1 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLimitVarID2 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nLimitVarID2
static int tolua_get_QUEST_DATA_nLimitVarID2(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLimitVarID2'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLimitVarID2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLimitVarID2 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nLimitVarID2
static int tolua_set_QUEST_DATA_nLimitVarID2(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLimitVarID2'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLimitVarID2 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLimitVarValue2 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nLimitVarValue2
static int tolua_get_QUEST_DATA_nLimitVarValue2(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLimitVarValue2'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLimitVarValue2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLimitVarValue2 of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nLimitVarValue2
static int tolua_set_QUEST_DATA_nLimitVarValue2(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLimitVarValue2'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLimitVarValue2 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szBeforeAcceptScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_szBeforeAcceptScriptName
static int tolua_get_data_manager_QUEST_DATA_szBeforeAcceptScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szBeforeAcceptScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->szBeforeAcceptScriptName[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szBeforeAcceptScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_szBeforeAcceptScriptName
static int tolua_set_data_manager_QUEST_DATA_szBeforeAcceptScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szBeforeAcceptScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->szBeforeAcceptScriptName[tolua_index] = ((  char)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nOfferMoneyType of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nOfferMoneyType
static int tolua_get_QUEST_DATA_nOfferMoneyType(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferMoneyType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nOfferMoneyType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nOfferMoneyType of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nOfferMoneyType
static int tolua_set_QUEST_DATA_nOfferMoneyType(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferMoneyType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nOfferMoneyType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nOfferMoneyAmount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nOfferMoneyAmount
static int tolua_get_QUEST_DATA_nOfferMoneyAmount(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferMoneyAmount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nOfferMoneyAmount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nOfferMoneyAmount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nOfferMoneyAmount
static int tolua_set_QUEST_DATA_nOfferMoneyAmount(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferMoneyAmount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nOfferMoneyAmount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nOfferBuffID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nOfferBuffID
static int tolua_get_QUEST_DATA_nOfferBuffID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferBuffID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nOfferBuffID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nOfferBuffID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nOfferBuffID
static int tolua_set_QUEST_DATA_nOfferBuffID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferBuffID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nOfferBuffID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nOfferBuffLevel of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nOfferBuffLevel
static int tolua_get_QUEST_DATA_nOfferBuffLevel(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferBuffLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nOfferBuffLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nOfferBuffLevel of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nOfferBuffLevel
static int tolua_set_QUEST_DATA_nOfferBuffLevel(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferBuffLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nOfferBuffLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nOfferItemID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nOfferItemID
static int tolua_get_QUEST_DATA_nOfferItemID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferItemID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nOfferItemID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nOfferItemID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nOfferItemID
static int tolua_set_QUEST_DATA_nOfferItemID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferItemID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nOfferItemID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nOfferItemCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nOfferItemCnt
static int tolua_get_QUEST_DATA_nOfferItemCnt(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferItemCnt'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nOfferItemCnt);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nOfferItemCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nOfferItemCnt
static int tolua_set_QUEST_DATA_nOfferItemCnt(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nOfferItemCnt'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nOfferItemCnt = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szAfterAcceptScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_szAfterAcceptScriptName
static int tolua_get_data_manager_QUEST_DATA_szAfterAcceptScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szAfterAcceptScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->szAfterAcceptScriptName[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szAfterAcceptScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_szAfterAcceptScriptName
static int tolua_set_data_manager_QUEST_DATA_szAfterAcceptScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szAfterAcceptScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->szAfterAcceptScriptName[tolua_index] = ((  char)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nFinishNpc of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nFinishNpc
static int tolua_get_QUEST_DATA_nFinishNpc(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFinishNpc'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nFinishNpc);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nFinishNpc of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nFinishNpc
static int tolua_set_QUEST_DATA_nFinishNpc(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFinishNpc'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nFinishNpc = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nFinishItemID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nFinishItemID
static int tolua_get_QUEST_DATA_nFinishItemID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFinishItemID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nFinishItemID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nFinishItemID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nFinishItemID
static int tolua_set_QUEST_DATA_nFinishItemID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFinishItemID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nFinishItemID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckBuffID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nCheckBuffID
static int tolua_get_QUEST_DATA_nCheckBuffID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckBuffID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckBuffID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckBuffID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nCheckBuffID
static int tolua_set_QUEST_DATA_nCheckBuffID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckBuffID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCheckBuffID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckBuffLevel of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nCheckBuffLevel
static int tolua_get_QUEST_DATA_nCheckBuffLevel(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckBuffLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckBuffLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckBuffLevel of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nCheckBuffLevel
static int tolua_set_QUEST_DATA_nCheckBuffLevel(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckBuffLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCheckBuffLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckLevel of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nCheckLevel
static int tolua_get_QUEST_DATA_nCheckLevel(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckLevel of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nCheckLevel
static int tolua_set_QUEST_DATA_nCheckLevel(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCheckLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillPlayerLevlStart of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillPlayerLevlStart
static int tolua_get_data_manager_QUEST_DATA_nCheckKillPlayerLevlStart(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillPlayerLevlStart, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillPlayerLevlStart[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillPlayerLevlStart of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillPlayerLevlStart
static int tolua_set_data_manager_QUEST_DATA_nCheckKillPlayerLevlStart(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillPlayerLevlStart, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillPlayerLevlStart[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillPlayerLevlEnd of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillPlayerLevlEnd
static int tolua_get_data_manager_QUEST_DATA_nCheckKillPlayerLevlEnd(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillPlayerLevlEnd, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillPlayerLevlEnd[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillPlayerLevlEnd of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillPlayerLevlEnd
static int tolua_set_data_manager_QUEST_DATA_nCheckKillPlayerLevlEnd(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillPlayerLevlEnd, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillPlayerLevlEnd[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillPlayerCount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillPlayerCount
static int tolua_get_data_manager_QUEST_DATA_nCheckKillPlayerCount(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillPlayerCount, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillPlayerCount[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillPlayerCount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillPlayerCount
static int tolua_set_data_manager_QUEST_DATA_nCheckKillPlayerCount(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillPlayerCount, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillPlayerCount[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillBufPlayerBufID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillBufPlayerBufID
static int tolua_get_data_manager_QUEST_DATA_nCheckKillBufPlayerBufID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillBufPlayerBufID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillBufPlayerBufID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillBufPlayerBufID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillBufPlayerBufID
static int tolua_set_data_manager_QUEST_DATA_nCheckKillBufPlayerBufID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillBufPlayerBufID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillBufPlayerBufID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillBufPlayerCount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillBufPlayerCount
static int tolua_get_data_manager_QUEST_DATA_nCheckKillBufPlayerCount(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillBufPlayerCount, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillBufPlayerCount[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillBufPlayerCount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillBufPlayerCount
static int tolua_set_data_manager_QUEST_DATA_nCheckKillBufPlayerCount(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillBufPlayerCount, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=CHECK_KILL_PLAYER_CONDITION)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillBufPlayerCount[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillNpcID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillNpcID
static int tolua_get_data_manager_QUEST_DATA_nCheckKillNpcID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillNpcID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillNpcID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillNpcID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillNpcID
static int tolua_set_data_manager_QUEST_DATA_nCheckKillNpcID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillNpcID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillNpcID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillNpcGroupID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillNpcGroupID
static int tolua_get_data_manager_QUEST_DATA_nCheckKillNpcGroupID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillNpcGroupID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillNpcGroupID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillNpcGroupID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillNpcGroupID
static int tolua_set_data_manager_QUEST_DATA_nCheckKillNpcGroupID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillNpcGroupID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillNpcGroupID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillNpcCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillNpcCnt
static int tolua_get_data_manager_QUEST_DATA_nCheckKillNpcCnt(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillNpcCnt, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillNpcCnt[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillNpcCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillNpcCnt
static int tolua_set_data_manager_QUEST_DATA_nCheckKillNpcCnt(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillNpcCnt, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillNpcCnt[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillItemNpcID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillItemNpcID
static int tolua_get_data_manager_QUEST_DATA_nCheckKillItemNpcID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillItemNpcID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillItemNpcID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillItemNpcID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillItemNpcID
static int tolua_set_data_manager_QUEST_DATA_nCheckKillItemNpcID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillItemNpcID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillItemNpcID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillItemNpcRate of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillItemNpcRate
static int tolua_get_data_manager_QUEST_DATA_nCheckKillItemNpcRate(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillItemNpcRate, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillItemNpcRate[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillItemNpcRate of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillItemNpcRate
static int tolua_set_data_manager_QUEST_DATA_nCheckKillItemNpcRate(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillItemNpcRate, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillItemNpcRate[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillItemCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillItemCnt
static int tolua_get_data_manager_QUEST_DATA_nCheckKillItemCnt(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillItemCnt, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillItemCnt[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillItemCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillItemCnt
static int tolua_set_data_manager_QUEST_DATA_nCheckKillItemCnt(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillItemCnt, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillItemCnt[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillDoodadID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillDoodadID
static int tolua_get_data_manager_QUEST_DATA_nCheckKillDoodadID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillDoodadID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillDoodadID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillDoodadID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillDoodadID
static int tolua_set_data_manager_QUEST_DATA_nCheckKillDoodadID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillDoodadID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillDoodadID[tolua_index] = (( unsigned int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckKillDoodadCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckKillDoodadCnt
static int tolua_get_data_manager_QUEST_DATA_nCheckKillDoodadCnt(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillDoodadCnt, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckKillDoodadCnt[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckKillDoodadCnt of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckKillDoodadCnt
static int tolua_set_data_manager_QUEST_DATA_nCheckKillDoodadCnt(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckKillDoodadCnt, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckKillDoodadCnt[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckItemID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckItemID
static int tolua_get_data_manager_QUEST_DATA_nCheckItemID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckItemID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckItemID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckItemID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckItemID
static int tolua_set_data_manager_QUEST_DATA_nCheckItemID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckItemID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckItemID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckItemCount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckItemCount
static int tolua_get_data_manager_QUEST_DATA_nCheckItemCount(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckItemCount, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckItemCount[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckItemCount of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckItemCount
static int tolua_set_data_manager_QUEST_DATA_nCheckItemCount(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckItemCount, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CONDITION_PER_QUEST)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckItemCount[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckReputationForceID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nCheckReputationForceID
static int tolua_get_QUEST_DATA_nCheckReputationForceID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckReputationForceID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckReputationForceID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckReputationForceID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nCheckReputationForceID
static int tolua_set_QUEST_DATA_nCheckReputationForceID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckReputationForceID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCheckReputationForceID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckReputationForceValue of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nCheckReputationForceValue
static int tolua_get_QUEST_DATA_nCheckReputationForceValue(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckReputationForceValue'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckReputationForceValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckReputationForceValue of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nCheckReputationForceValue
static int tolua_set_QUEST_DATA_nCheckReputationForceValue(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCheckReputationForceValue'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCheckReputationForceValue = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckVarID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckVarID
static int tolua_get_data_manager_QUEST_DATA_nCheckVarID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckVarID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CHECK_VAR_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckVarID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckVarID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckVarID
static int tolua_set_data_manager_QUEST_DATA_nCheckVarID(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckVarID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CHECK_VAR_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckVarID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCheckVarValue of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_nCheckVarValue
static int tolua_get_data_manager_QUEST_DATA_nCheckVarValue(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckVarValue, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CHECK_VAR_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCheckVarValue[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCheckVarValue of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_nCheckVarValue
static int tolua_set_data_manager_QUEST_DATA_nCheckVarValue(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nCheckVarValue, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CHECK_VAR_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nCheckVarValue[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szBeforeCompleteScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_szBeforeCompleteScriptName
static int tolua_get_data_manager_QUEST_DATA_szBeforeCompleteScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szBeforeCompleteScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->szBeforeCompleteScriptName[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szBeforeCompleteScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_szBeforeCompleteScriptName
static int tolua_set_data_manager_QUEST_DATA_szBeforeCompleteScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szBeforeCompleteScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->szBeforeCompleteScriptName[tolua_index] = ((  char)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTalkNpcID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nTalkNpcID
static int tolua_get_QUEST_DATA_nTalkNpcID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTalkNpcID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTalkNpcID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTalkNpcID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nTalkNpcID
static int tolua_set_QUEST_DATA_nTalkNpcID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTalkNpcID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTalkNpcID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTalkVarID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nTalkVarID
static int tolua_get_QUEST_DATA_nTalkVarID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTalkVarID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTalkVarID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTalkVarID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nTalkVarID
static int tolua_set_QUEST_DATA_nTalkVarID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTalkVarID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTalkVarID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nExploreVarID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_nExploreVarID
static int tolua_get_QUEST_DATA_nExploreVarID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nExploreVarID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nExploreVarID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nExploreVarID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_nExploreVarID
static int tolua_set_QUEST_DATA_nExploreVarID(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nExploreVarID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nExploreVarID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szAfterCompleteScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_szAfterCompleteScriptName
static int tolua_get_data_manager_QUEST_DATA_szAfterCompleteScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szAfterCompleteScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->szAfterCompleteScriptName[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szAfterCompleteScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_szAfterCompleteScriptName
static int tolua_set_data_manager_QUEST_DATA_szAfterCompleteScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szAfterCompleteScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->szAfterCompleteScriptName[tolua_index] = ((  char)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: stRewardData of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_DATA_stRewardData
static int tolua_get_QUEST_DATA_stRewardData(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'stRewardData'",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)&self->stRewardData,"QUEST_REWARD_DATA");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: stRewardData of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_DATA_stRewardData
static int tolua_set_QUEST_DATA_stRewardData(lua_State* tolua_S)
{
  QUEST_DATA* self = (QUEST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'stRewardData'",NULL);
 if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"QUEST_REWARD_DATA",0,&tolua_err)))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->stRewardData = *((QUEST_REWARD_DATA*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szAbandonScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_QUEST_DATA_szAbandonScriptName
static int tolua_get_data_manager_QUEST_DATA_szAbandonScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szAbandonScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->szAbandonScriptName[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szAbandonScriptName of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_QUEST_DATA_szAbandonScriptName
static int tolua_set_data_manager_QUEST_DATA_szAbandonScriptName(lua_State* tolua_S)
{
 int tolua_index;
  QUEST_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (QUEST_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->szAbandonScriptName, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=COMMON_NAME_LEN)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->szAbandonScriptName[tolua_index] = ((  char)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_QUEST_DATA_ReadLine00
static int tolua_data_manager_QUEST_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"QUEST_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"QUEST_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  QUEST_DATA* pData = ((QUEST_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
QUEST_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CheckValidQuestID of class  QUEST_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_QUEST_DATA_CheckValidQuestID00
static int tolua_data_manager_QUEST_DATA_CheckValidQuestID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"QUEST_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int nQuestType = (( signed int)  tolua_tonumber(tolua_S,2,0));
  signed int nQuestID = (( signed int)  tolua_tonumber(tolua_S,3,0));
 {
 int tolua_ret = (int)
QUEST_DATA::CheckValidQuestID(nQuestType,nQuestID);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CheckValidQuestID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTime of class  REWARD_QUEST_MONEY_COST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_REWARD_QUEST_MONEY_COST_DATA_nTime
static int tolua_get_REWARD_QUEST_MONEY_COST_DATA_nTime(lua_State* tolua_S)
{
  REWARD_QUEST_MONEY_COST_DATA* self = (REWARD_QUEST_MONEY_COST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTime of class  REWARD_QUEST_MONEY_COST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_REWARD_QUEST_MONEY_COST_DATA_nTime
static int tolua_set_REWARD_QUEST_MONEY_COST_DATA_nTime(lua_State* tolua_S)
{
  REWARD_QUEST_MONEY_COST_DATA* self = (REWARD_QUEST_MONEY_COST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMoneyCost of class  REWARD_QUEST_MONEY_COST_DATA */
#ifndef TOLUA_DISABLE_tolua_get_REWARD_QUEST_MONEY_COST_DATA_nMoneyCost
static int tolua_get_REWARD_QUEST_MONEY_COST_DATA_nMoneyCost(lua_State* tolua_S)
{
  REWARD_QUEST_MONEY_COST_DATA* self = (REWARD_QUEST_MONEY_COST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMoneyCost'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMoneyCost);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMoneyCost of class  REWARD_QUEST_MONEY_COST_DATA */
#ifndef TOLUA_DISABLE_tolua_set_REWARD_QUEST_MONEY_COST_DATA_nMoneyCost
static int tolua_set_REWARD_QUEST_MONEY_COST_DATA_nMoneyCost(lua_State* tolua_S)
{
  REWARD_QUEST_MONEY_COST_DATA* self = (REWARD_QUEST_MONEY_COST_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMoneyCost'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMoneyCost = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  REWARD_QUEST_MONEY_COST_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_REWARD_QUEST_MONEY_COST_DATA_ReadLine00
static int tolua_data_manager_REWARD_QUEST_MONEY_COST_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"REWARD_QUEST_MONEY_COST_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"REWARD_QUEST_MONEY_COST_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  REWARD_QUEST_MONEY_COST_DATA* pData = ((REWARD_QUEST_MONEY_COST_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
REWARD_QUEST_MONEY_COST_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nItemLevel of class  QUEST_RECYCLE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_RECYCLE_DATA_nItemLevel
static int tolua_get_QUEST_RECYCLE_DATA_nItemLevel(lua_State* tolua_S)
{
  QUEST_RECYCLE_DATA* self = (QUEST_RECYCLE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nItemLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nItemLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nItemLevel of class  QUEST_RECYCLE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_RECYCLE_DATA_nItemLevel
static int tolua_set_QUEST_RECYCLE_DATA_nItemLevel(lua_State* tolua_S)
{
  QUEST_RECYCLE_DATA* self = (QUEST_RECYCLE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nItemLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nItemLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nItemColor of class  QUEST_RECYCLE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_RECYCLE_DATA_nItemColor
static int tolua_get_QUEST_RECYCLE_DATA_nItemColor(lua_State* tolua_S)
{
  QUEST_RECYCLE_DATA* self = (QUEST_RECYCLE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nItemColor'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nItemColor);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nItemColor of class  QUEST_RECYCLE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_RECYCLE_DATA_nItemColor
static int tolua_set_QUEST_RECYCLE_DATA_nItemColor(lua_State* tolua_S)
{
  QUEST_RECYCLE_DATA* self = (QUEST_RECYCLE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nItemColor'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nItemColor = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nItemSubType of class  QUEST_RECYCLE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_RECYCLE_DATA_nItemSubType
static int tolua_get_QUEST_RECYCLE_DATA_nItemSubType(lua_State* tolua_S)
{
  QUEST_RECYCLE_DATA* self = (QUEST_RECYCLE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nItemSubType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nItemSubType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nItemSubType of class  QUEST_RECYCLE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_RECYCLE_DATA_nItemSubType
static int tolua_set_QUEST_RECYCLE_DATA_nItemSubType(lua_State* tolua_S)
{
  QUEST_RECYCLE_DATA* self = (QUEST_RECYCLE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nItemSubType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nItemSubType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nVarIndex of class  QUEST_RECYCLE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEST_RECYCLE_DATA_nVarIndex
static int tolua_get_QUEST_RECYCLE_DATA_nVarIndex(lua_State* tolua_S)
{
  QUEST_RECYCLE_DATA* self = (QUEST_RECYCLE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nVarIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nVarIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nVarIndex of class  QUEST_RECYCLE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEST_RECYCLE_DATA_nVarIndex
static int tolua_set_QUEST_RECYCLE_DATA_nVarIndex(lua_State* tolua_S)
{
  QUEST_RECYCLE_DATA* self = (QUEST_RECYCLE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nVarIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nVarIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  QUEST_RECYCLE_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_QUEST_RECYCLE_DATA_ReadLine00
static int tolua_data_manager_QUEST_RECYCLE_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"QUEST_RECYCLE_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"QUEST_RECYCLE_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  QUEST_RECYCLE_DATA* pData = ((QUEST_RECYCLE_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
QUEST_RECYCLE_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nFactionID of class  ROLE_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ROLE_RELATION_DATA_nFactionID
static int tolua_get_ROLE_RELATION_DATA_nFactionID(lua_State* tolua_S)
{
  ROLE_RELATION_DATA* self = (ROLE_RELATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFactionID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nFactionID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nFactionID of class  ROLE_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ROLE_RELATION_DATA_nFactionID
static int tolua_set_ROLE_RELATION_DATA_nFactionID(lua_State* tolua_S)
{
  ROLE_RELATION_DATA* self = (ROLE_RELATION_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFactionID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nFactionID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRelation of class  ROLE_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ROLE_RELATION_DATA_nRelation
static int tolua_get_data_manager_ROLE_RELATION_DATA_nRelation(lua_State* tolua_S)
{
 int tolua_index;
  ROLE_RELATION_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ROLE_RELATION_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nRelation, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_ROLE_FRACTION_ID)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRelation[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRelation of class  ROLE_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ROLE_RELATION_DATA_nRelation
static int tolua_set_data_manager_ROLE_RELATION_DATA_nRelation(lua_State* tolua_S)
{
 int tolua_index;
  ROLE_RELATION_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ROLE_RELATION_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nRelation, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_ROLE_FRACTION_ID)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nRelation[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  ROLE_RELATION_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_ROLE_RELATION_DATA_ReadLine00
static int tolua_data_manager_ROLE_RELATION_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"ROLE_RELATION_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"ROLE_RELATION_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  ROLE_RELATION_DATA* pData = ((ROLE_RELATION_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
ROLE_RELATION_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSceneID of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nSceneID
static int tolua_get_SCENE_DATA_nSceneID(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSceneID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSceneID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSceneID of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nSceneID
static int tolua_set_SCENE_DATA_nSceneID(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSceneID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSceneID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szConfigName of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_szConfigName
static int tolua_get_SCENE_DATA_szConfigName(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szConfigName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szConfigName, ClientFixedString::Length(self->szConfigName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szConfigName of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_szConfigName
static int tolua_set_SCENE_DATA_szConfigName(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szConfigName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szConfigName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szMapDisplayName of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_szMapDisplayName
static int tolua_get_SCENE_DATA_szMapDisplayName(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szMapDisplayName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szMapDisplayName, ClientFixedString::Length(self->szMapDisplayName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szMapDisplayName of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_szMapDisplayName
static int tolua_set_SCENE_DATA_szMapDisplayName(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szMapDisplayName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szMapDisplayName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nType of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nType
static int tolua_get_SCENE_DATA_nType(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nType of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nType
static int tolua_set_SCENE_DATA_nType(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxRoleCount of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nMaxRoleCount
static int tolua_get_SCENE_DATA_nMaxRoleCount(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxRoleCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxRoleCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxRoleCount of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nMaxRoleCount
static int tolua_set_SCENE_DATA_nMaxRoleCount(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxRoleCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxRoleCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nFinishTarget of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nFinishTarget
static int tolua_get_SCENE_DATA_nFinishTarget(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFinishTarget'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nFinishTarget);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nFinishTarget of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nFinishTarget
static int tolua_set_SCENE_DATA_nFinishTarget(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFinishTarget'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nFinishTarget = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotUseScroll of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotUseScroll
static int tolua_get_SCENE_DATA_bCannotUseScroll(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotUseScroll'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotUseScroll);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotUseScroll of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotUseScroll
static int tolua_set_SCENE_DATA_bCannotUseScroll(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotUseScroll'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotUseScroll = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotUseTeleportRing of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotUseTeleportRing
static int tolua_get_SCENE_DATA_bCannotUseTeleportRing(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotUseTeleportRing'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotUseTeleportRing);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotUseTeleportRing of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotUseTeleportRing
static int tolua_set_SCENE_DATA_bCannotUseTeleportRing(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotUseTeleportRing'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotUseTeleportRing = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotBeRecalled of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotBeRecalled
static int tolua_get_SCENE_DATA_bCannotBeRecalled(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotBeRecalled'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotBeRecalled);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotBeRecalled of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotBeRecalled
static int tolua_set_SCENE_DATA_bCannotBeRecalled(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotBeRecalled'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotBeRecalled = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotTrade of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotTrade
static int tolua_get_SCENE_DATA_bCannotTrade(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotTrade'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotTrade);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotTrade of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotTrade
static int tolua_set_SCENE_DATA_bCannotTrade(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotTrade'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotTrade = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotLoseDurability of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotLoseDurability
static int tolua_get_SCENE_DATA_bCannotLoseDurability(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotLoseDurability'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotLoseDurability);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotLoseDurability of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotLoseDurability
static int tolua_set_SCENE_DATA_bCannotLoseDurability(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotLoseDurability'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotLoseDurability = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotSendMail of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotSendMail
static int tolua_get_SCENE_DATA_bCannotSendMail(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotSendMail'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotSendMail);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotSendMail of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotSendMail
static int tolua_set_SCENE_DATA_bCannotSendMail(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotSendMail'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotSendMail = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotUseExchange of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotUseExchange
static int tolua_get_SCENE_DATA_bCannotUseExchange(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotUseExchange'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotUseExchange);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotUseExchange of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotUseExchange
static int tolua_set_SCENE_DATA_bCannotUseExchange(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotUseExchange'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotUseExchange = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotUseAuction of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotUseAuction
static int tolua_get_SCENE_DATA_bCannotUseAuction(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotUseAuction'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotUseAuction);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotUseAuction of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotUseAuction
static int tolua_set_SCENE_DATA_bCannotUseAuction(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotUseAuction'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotUseAuction = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotJoinTeam of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotJoinTeam
static int tolua_get_SCENE_DATA_bCannotJoinTeam(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotJoinTeam'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotJoinTeam);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotJoinTeam of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotJoinTeam
static int tolua_set_SCENE_DATA_bCannotJoinTeam(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotJoinTeam'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotJoinTeam = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bEnableAntiCheat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bEnableAntiCheat
static int tolua_get_SCENE_DATA_bEnableAntiCheat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bEnableAntiCheat'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bEnableAntiCheat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bEnableAntiCheat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bEnableAntiCheat
static int tolua_set_SCENE_DATA_bEnableAntiCheat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bEnableAntiCheat'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bEnableAntiCheat = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bBlockRole of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bBlockRole
static int tolua_get_SCENE_DATA_bBlockRole(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockRole'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bBlockRole);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bBlockRole of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bBlockRole
static int tolua_set_SCENE_DATA_bBlockRole(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockRole'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bBlockRole = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotGroupChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotGroupChat
static int tolua_get_SCENE_DATA_bCannotGroupChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotGroupChat'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotGroupChat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotGroupChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotGroupChat
static int tolua_set_SCENE_DATA_bCannotGroupChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotGroupChat'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotGroupChat = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotSceneChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotSceneChat
static int tolua_get_SCENE_DATA_bCannotSceneChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotSceneChat'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotSceneChat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotSceneChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotSceneChat
static int tolua_set_SCENE_DATA_bCannotSceneChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotSceneChat'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotSceneChat = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotScreenChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotScreenChat
static int tolua_get_SCENE_DATA_bCannotScreenChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotScreenChat'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotScreenChat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotScreenChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotScreenChat
static int tolua_set_SCENE_DATA_bCannotScreenChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotScreenChat'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotScreenChat = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotWhisperChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotWhisperChat
static int tolua_get_SCENE_DATA_bCannotWhisperChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotWhisperChat'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotWhisperChat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotWhisperChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotWhisperChat
static int tolua_set_SCENE_DATA_bCannotWhisperChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotWhisperChat'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotWhisperChat = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotTeamChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotTeamChat
static int tolua_get_SCENE_DATA_bCannotTeamChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotTeamChat'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotTeamChat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotTeamChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotTeamChat
static int tolua_set_SCENE_DATA_bCannotTeamChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotTeamChat'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotTeamChat = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotGuildChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotGuildChat
static int tolua_get_SCENE_DATA_bCannotGuildChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotGuildChat'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotGuildChat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotGuildChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotGuildChat
static int tolua_set_SCENE_DATA_bCannotGuildChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotGuildChat'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotGuildChat = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotFriendChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bCannotFriendChat
static int tolua_get_SCENE_DATA_bCannotFriendChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotFriendChat'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bCannotFriendChat);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotFriendChat of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bCannotFriendChat
static int tolua_set_SCENE_DATA_bCannotFriendChat(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotFriendChat'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotFriendChat = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMinPortalLevel of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nMinPortalLevel
static int tolua_get_SCENE_DATA_nMinPortalLevel(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinPortalLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMinPortalLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMinPortalLevel of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nMinPortalLevel
static int tolua_set_SCENE_DATA_nMinPortalLevel(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinPortalLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMinPortalLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxPortalLevel of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nMaxPortalLevel
static int tolua_get_SCENE_DATA_nMaxPortalLevel(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxPortalLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxPortalLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxPortalLevel of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nMaxPortalLevel
static int tolua_set_SCENE_DATA_nMaxPortalLevel(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxPortalLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxPortalLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nWeather of class  WEATHER_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA__WEATHER_DATA_nWeather
static int tolua_get_SCENE_DATA__WEATHER_DATA_nWeather(lua_State* tolua_S)
{
  SCENE_DATA::WEATHER_DATA* self = (SCENE_DATA::WEATHER_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWeather'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nWeather);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nWeather of class  WEATHER_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA__WEATHER_DATA_nWeather
static int tolua_set_SCENE_DATA__WEATHER_DATA_nWeather(lua_State* tolua_S)
{
  SCENE_DATA::WEATHER_DATA* self = (SCENE_DATA::WEATHER_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWeather'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nWeather = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nWeight of class  WEATHER_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA__WEATHER_DATA_nWeight
static int tolua_get_SCENE_DATA__WEATHER_DATA_nWeight(lua_State* tolua_S)
{
  SCENE_DATA::WEATHER_DATA* self = (SCENE_DATA::WEATHER_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWeight'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nWeight);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nWeight of class  WEATHER_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA__WEATHER_DATA_nWeight
static int tolua_set_SCENE_DATA__WEATHER_DATA_nWeight(lua_State* tolua_S)
{
  SCENE_DATA::WEATHER_DATA* self = (SCENE_DATA::WEATHER_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWeight'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nWeight = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMinDuration of class  WEATHER_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA__WEATHER_DATA_nMinDuration
static int tolua_get_SCENE_DATA__WEATHER_DATA_nMinDuration(lua_State* tolua_S)
{
  SCENE_DATA::WEATHER_DATA* self = (SCENE_DATA::WEATHER_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinDuration'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMinDuration);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMinDuration of class  WEATHER_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA__WEATHER_DATA_nMinDuration
static int tolua_set_SCENE_DATA__WEATHER_DATA_nMinDuration(lua_State* tolua_S)
{
  SCENE_DATA::WEATHER_DATA* self = (SCENE_DATA::WEATHER_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinDuration'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMinDuration = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxDuration of class  WEATHER_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA__WEATHER_DATA_nMaxDuration
static int tolua_get_SCENE_DATA__WEATHER_DATA_nMaxDuration(lua_State* tolua_S)
{
  SCENE_DATA::WEATHER_DATA* self = (SCENE_DATA::WEATHER_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxDuration'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxDuration);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxDuration of class  WEATHER_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA__WEATHER_DATA_nMaxDuration
static int tolua_set_SCENE_DATA__WEATHER_DATA_nMaxDuration(lua_State* tolua_S)
{
  SCENE_DATA::WEATHER_DATA* self = (SCENE_DATA::WEATHER_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxDuration'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxDuration = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: WeatherData of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_SCENE_DATA_WeatherData
static int tolua_get_data_manager_SCENE_DATA_WeatherData(lua_State* tolua_S)
{
 int tolua_index;
  SCENE_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (SCENE_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->WeatherData, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_RANDOM_WEATHER_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)&self->WeatherData[tolua_index],"SCENE_DATA::WEATHER_DATA");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: WeatherData of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_SCENE_DATA_WeatherData
static int tolua_set_data_manager_SCENE_DATA_WeatherData(lua_State* tolua_S)
{
 int tolua_index;
  SCENE_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (SCENE_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->WeatherData, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_RANDOM_WEATHER_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->WeatherData[tolua_index] = *((SCENE_DATA::WEATHER_DATA*)  tolua_tousertype(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMinWeatherRefreshInterval of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nMinWeatherRefreshInterval
static int tolua_get_SCENE_DATA_nMinWeatherRefreshInterval(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinWeatherRefreshInterval'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMinWeatherRefreshInterval);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMinWeatherRefreshInterval of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nMinWeatherRefreshInterval
static int tolua_set_SCENE_DATA_nMinWeatherRefreshInterval(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinWeatherRefreshInterval'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMinWeatherRefreshInterval = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxWeatherRefreshInterval of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nMaxWeatherRefreshInterval
static int tolua_get_SCENE_DATA_nMaxWeatherRefreshInterval(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxWeatherRefreshInterval'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxWeatherRefreshInterval);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxWeatherRefreshInterval of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nMaxWeatherRefreshInterval
static int tolua_set_SCENE_DATA_nMaxWeatherRefreshInterval(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxWeatherRefreshInterval'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxWeatherRefreshInterval = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: RandWeather of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_RandWeather
static int tolua_get_SCENE_DATA_RandWeather(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'RandWeather'",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)&self->RandWeather,"CWeightRand");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: RandWeather of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_RandWeather
static int tolua_set_SCENE_DATA_RandWeather(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'RandWeather'",NULL);
 if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"CWeightRand",0,&tolua_err)))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->RandWeather = *((CWeightRand*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMinPlayerLevel of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nMinPlayerLevel
static int tolua_get_SCENE_DATA_nMinPlayerLevel(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinPlayerLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMinPlayerLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMinPlayerLevel of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nMinPlayerLevel
static int tolua_set_SCENE_DATA_nMinPlayerLevel(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinPlayerLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMinPlayerLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxPlayerLevel of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nMaxPlayerLevel
static int tolua_get_SCENE_DATA_nMaxPlayerLevel(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxPlayerLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxPlayerLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxPlayerLevel of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nMaxPlayerLevel
static int tolua_set_SCENE_DATA_nMaxPlayerLevel(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxPlayerLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxPlayerLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nInitCount of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nInitCount
static int tolua_get_SCENE_DATA_nInitCount(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nInitCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nInitCount of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nInitCount
static int tolua_set_SCENE_DATA_nInitCount(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nInitCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nInitCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSideCount of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nSideCount
static int tolua_get_SCENE_DATA_nSideCount(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSideCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSideCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSideCount of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nSideCount
static int tolua_set_SCENE_DATA_nSideCount(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSideCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSideCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBattleType of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nBattleType
static int tolua_get_SCENE_DATA_nBattleType(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBattleType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBattleType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBattleType of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nBattleType
static int tolua_set_SCENE_DATA_nBattleType(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBattleType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBattleType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMinRoleLimitPerSide of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nMinRoleLimitPerSide
static int tolua_get_SCENE_DATA_nMinRoleLimitPerSide(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinRoleLimitPerSide'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMinRoleLimitPerSide);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMinRoleLimitPerSide of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nMinRoleLimitPerSide
static int tolua_set_SCENE_DATA_nMinRoleLimitPerSide(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMinRoleLimitPerSide'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMinRoleLimitPerSide = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxRoleLimitPerSide of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nMaxRoleLimitPerSide
static int tolua_get_SCENE_DATA_nMaxRoleLimitPerSide(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxRoleLimitPerSide'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxRoleLimitPerSide);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxRoleLimitPerSide of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nMaxRoleLimitPerSide
static int tolua_set_SCENE_DATA_nMaxRoleLimitPerSide(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxRoleLimitPerSide'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxRoleLimitPerSide = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nHandbookType of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nHandbookType
static int tolua_get_SCENE_DATA_nHandbookType(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nHandbookType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nHandbookType of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nHandbookType
static int tolua_set_SCENE_DATA_nHandbookType(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nHandbookType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nHandbookIndex of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nHandbookIndex
static int tolua_get_SCENE_DATA_nHandbookIndex(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nHandbookIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nHandbookIndex of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nHandbookIndex
static int tolua_set_SCENE_DATA_nHandbookIndex(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nHandbookIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLoginJumpSceneTempID of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nLoginJumpSceneTempID
static int tolua_get_SCENE_DATA_nLoginJumpSceneTempID(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLoginJumpSceneTempID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLoginJumpSceneTempID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLoginJumpSceneTempID of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nLoginJumpSceneTempID
static int tolua_set_SCENE_DATA_nLoginJumpSceneTempID(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLoginJumpSceneTempID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLoginJumpSceneTempID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLoginJumpX of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nLoginJumpX
static int tolua_get_SCENE_DATA_nLoginJumpX(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLoginJumpX'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLoginJumpX);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLoginJumpX of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nLoginJumpX
static int tolua_set_SCENE_DATA_nLoginJumpX(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLoginJumpX'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLoginJumpX = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLoginJumpY of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nLoginJumpY
static int tolua_get_SCENE_DATA_nLoginJumpY(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLoginJumpY'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLoginJumpY);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLoginJumpY of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nLoginJumpY
static int tolua_set_SCENE_DATA_nLoginJumpY(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLoginJumpY'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLoginJumpY = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLoginJumpZ of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_nLoginJumpZ
static int tolua_get_SCENE_DATA_nLoginJumpZ(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLoginJumpZ'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLoginJumpZ);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLoginJumpZ of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_nLoginJumpZ
static int tolua_set_SCENE_DATA_nLoginJumpZ(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLoginJumpZ'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLoginJumpZ = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bEnableIntimacy of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bEnableIntimacy
static int tolua_get_SCENE_DATA_bEnableIntimacy(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bEnableIntimacy'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bEnableIntimacy);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bEnableIntimacy of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bEnableIntimacy
static int tolua_set_SCENE_DATA_bEnableIntimacy(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bEnableIntimacy'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bEnableIntimacy = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bGpkPunish of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bGpkPunish
static int tolua_get_SCENE_DATA_bGpkPunish(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bGpkPunish'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bGpkPunish);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bGpkPunish of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bGpkPunish
static int tolua_set_SCENE_DATA_bGpkPunish(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bGpkPunish'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bGpkPunish = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bGpkKick of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SCENE_DATA_bGpkKick
static int tolua_get_SCENE_DATA_bGpkKick(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bGpkKick'",NULL);
#endif
 tolua_pushboolean(tolua_S,(int)self->bGpkKick);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bGpkKick of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SCENE_DATA_bGpkKick
static int tolua_set_SCENE_DATA_bGpkKick(lua_State* tolua_S)
{
  SCENE_DATA* self = (SCENE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bGpkKick'",NULL);
 if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bGpkKick = ((BOOL)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  SCENE_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_SCENE_DATA_ReadLine00
static int tolua_data_manager_SCENE_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"SCENE_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"SCENE_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  SCENE_DATA* pData = ((SCENE_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
SCENE_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nShopID of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nShopID
static int tolua_get_SHOP_DATA_m_nShopID(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nShopID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nShopID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nShopID of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nShopID
static int tolua_set_SHOP_DATA_m_nShopID(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nShopID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nShopID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nIndex of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nIndex
static int tolua_get_SHOP_DATA_m_nIndex(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nIndex of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nIndex
static int tolua_set_SHOP_DATA_m_nIndex(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_szShopName of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_szShopName
static int tolua_get_SHOP_DATA_m_szShopName(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szShopName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->m_szShopName, ClientFixedString::Length(self->m_szShopName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_szShopName of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_szShopName
static int tolua_set_SHOP_DATA_m_szShopName(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szShopName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->m_szShopName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nItemID of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nItemID
static int tolua_get_SHOP_DATA_m_nItemID(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nItemID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nItemID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nItemID of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nItemID
static int tolua_set_SHOP_DATA_m_nItemID(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nItemID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nItemID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nItemCount of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nItemCount
static int tolua_get_SHOP_DATA_m_nItemCount(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nItemCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nItemCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nItemCount of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nItemCount
static int tolua_set_SHOP_DATA_m_nItemCount(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nItemCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nItemCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nCostItemID1 of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nCostItemID1
static int tolua_get_SHOP_DATA_m_nCostItemID1(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCostItemID1'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nCostItemID1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nCostItemID1 of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nCostItemID1
static int tolua_set_SHOP_DATA_m_nCostItemID1(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCostItemID1'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nCostItemID1 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nCostValue1 of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nCostValue1
static int tolua_get_SHOP_DATA_m_nCostValue1(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCostValue1'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nCostValue1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nCostValue1 of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nCostValue1
static int tolua_set_SHOP_DATA_m_nCostValue1(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCostValue1'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nCostValue1 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nCostItemID2 of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nCostItemID2
static int tolua_get_SHOP_DATA_m_nCostItemID2(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCostItemID2'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nCostItemID2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nCostItemID2 of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nCostItemID2
static int tolua_set_SHOP_DATA_m_nCostItemID2(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCostItemID2'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nCostItemID2 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nCostValue2 of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nCostValue2
static int tolua_get_SHOP_DATA_m_nCostValue2(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCostValue2'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nCostValue2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nCostValue2 of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nCostValue2
static int tolua_set_SHOP_DATA_m_nCostValue2(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nCostValue2'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nCostValue2 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nRequireForceID of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nRequireForceID
static int tolua_get_SHOP_DATA_m_nRequireForceID(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nRequireForceID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nRequireForceID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nRequireForceID of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nRequireForceID
static int tolua_set_SHOP_DATA_m_nRequireForceID(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nRequireForceID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nRequireForceID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nReputation of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nReputation
static int tolua_get_SHOP_DATA_m_nReputation(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nReputation'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nReputation);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nReputation of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nReputation
static int tolua_set_SHOP_DATA_m_nReputation(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nReputation'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nReputation = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nRequireGuildBuildingID of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nRequireGuildBuildingID
static int tolua_get_SHOP_DATA_m_nRequireGuildBuildingID(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nRequireGuildBuildingID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nRequireGuildBuildingID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nRequireGuildBuildingID of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nRequireGuildBuildingID
static int tolua_set_SHOP_DATA_m_nRequireGuildBuildingID(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nRequireGuildBuildingID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nRequireGuildBuildingID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nBuildingLevel of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nBuildingLevel
static int tolua_get_SHOP_DATA_m_nBuildingLevel(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nBuildingLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nBuildingLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nBuildingLevel of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nBuildingLevel
static int tolua_set_SHOP_DATA_m_nBuildingLevel(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nBuildingLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nBuildingLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nShopTypeSell of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_nShopTypeSell
static int tolua_get_SHOP_DATA_m_nShopTypeSell(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nShopTypeSell'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nShopTypeSell);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nShopTypeSell of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SHOP_DATA_m_nShopTypeSell
static int tolua_set_SHOP_DATA_m_nShopTypeSell(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nShopTypeSell'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nShopTypeSell = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_pItemTemplate of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SHOP_DATA_m_pItemTemplate_ptr
static int tolua_get_SHOP_DATA_m_pItemTemplate_ptr(lua_State* tolua_S)
{
  SHOP_DATA* self = (SHOP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_pItemTemplate'",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)self->m_pItemTemplate,"const ITEM_DATA");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  SHOP_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_SHOP_DATA_ReadLine00
static int tolua_data_manager_SHOP_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"SHOP_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"SHOP_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  SHOP_DATA* pData = ((SHOP_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
SHOP_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nLevel of class  ATTR_LEVEL */
#ifndef TOLUA_DISABLE_tolua_get_ATTR_LEVEL_m_nLevel
static int tolua_get_ATTR_LEVEL_m_nLevel(lua_State* tolua_S)
{
  ATTR_LEVEL* self = (ATTR_LEVEL*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nLevel of class  ATTR_LEVEL */
#ifndef TOLUA_DISABLE_tolua_set_ATTR_LEVEL_m_nLevel
static int tolua_set_ATTR_LEVEL_m_nLevel(lua_State* tolua_S)
{
  ATTR_LEVEL* self = (ATTR_LEVEL*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nClass of class  ATTR_LEVEL */
#ifndef TOLUA_DISABLE_tolua_get_ATTR_LEVEL_m_nClass
static int tolua_get_ATTR_LEVEL_m_nClass(lua_State* tolua_S)
{
  ATTR_LEVEL* self = (ATTR_LEVEL*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nClass'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nClass);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nClass of class  ATTR_LEVEL */
#ifndef TOLUA_DISABLE_tolua_set_ATTR_LEVEL_m_nClass
static int tolua_set_ATTR_LEVEL_m_nClass(lua_State* tolua_S)
{
  ATTR_LEVEL* self = (ATTR_LEVEL*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nClass'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nClass = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMainAttr of class  ATTR_LEVEL */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ATTR_LEVEL_m_nMainAttr
static int tolua_get_data_manager_ATTR_LEVEL_m_nMainAttr(lua_State* tolua_S)
{
 int tolua_index;
  ATTR_LEVEL* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ATTR_LEVEL*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nMainAttr, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=catTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMainAttr[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMainAttr of class  ATTR_LEVEL */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ATTR_LEVEL_m_nMainAttr
static int tolua_set_data_manager_ATTR_LEVEL_m_nMainAttr(lua_State* tolua_S)
{
 int tolua_index;
  ATTR_LEVEL* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ATTR_LEVEL*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nMainAttr, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=catTotal)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_nMainAttr[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  ATTR_LEVEL */
#ifndef TOLUA_DISABLE_tolua_data_manager_ATTR_LEVEL_ReadLine00
static int tolua_data_manager_ATTR_LEVEL_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"ATTR_LEVEL",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"ATTR_LEVEL",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  ATTR_LEVEL* pData = ((ATTR_LEVEL*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
ATTR_LEVEL::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAchievementID of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ACHIEVEMENT_DATA_nAchievementID
static int tolua_get_ACHIEVEMENT_DATA_nAchievementID(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAchievementID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAchievementID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAchievementID of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ACHIEVEMENT_DATA_nAchievementID
static int tolua_set_ACHIEVEMENT_DATA_nAchievementID(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAchievementID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAchievementID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szName of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ACHIEVEMENT_DATA_szName
static int tolua_get_ACHIEVEMENT_DATA_szName(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szName, ClientFixedString::Length(self->szName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szName of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ACHIEVEMENT_DATA_szName
static int tolua_set_ACHIEVEMENT_DATA_szName(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: vecAchievementType of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementType
static int tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementType(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecAchievementType, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CON_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->vecAchievementType[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: vecAchievementType of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementType
static int tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementType(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecAchievementType, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CON_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->vecAchievementType[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: vecAchievementValue1 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementValue1
static int tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementValue1(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecAchievementValue1, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CON_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->vecAchievementValue1[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: vecAchievementValue1 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementValue1
static int tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementValue1(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecAchievementValue1, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CON_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->vecAchievementValue1[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: vecAchievementValue2 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementValue2
static int tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementValue2(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecAchievementValue2, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CON_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->vecAchievementValue2[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: vecAchievementValue2 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementValue2
static int tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementValue2(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecAchievementValue2, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CON_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->vecAchievementValue2[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: vecAchievementValue3 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementValue3
static int tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementValue3(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecAchievementValue3, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CON_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->vecAchievementValue3[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: vecAchievementValue3 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementValue3
static int tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementValue3(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->vecAchievementValue3, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_CON_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->vecAchievementValue3[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAwardSlotCount of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ACHIEVEMENT_DATA_nAwardSlotCount
static int tolua_get_ACHIEVEMENT_DATA_nAwardSlotCount(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAwardSlotCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAwardSlotCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAwardSlotCount of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ACHIEVEMENT_DATA_nAwardSlotCount
static int tolua_set_ACHIEVEMENT_DATA_nAwardSlotCount(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAwardSlotCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAwardSlotCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nPreAchievementID1 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ACHIEVEMENT_DATA_nPreAchievementID1
static int tolua_get_ACHIEVEMENT_DATA_nPreAchievementID1(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreAchievementID1'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nPreAchievementID1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nPreAchievementID1 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ACHIEVEMENT_DATA_nPreAchievementID1
static int tolua_set_ACHIEVEMENT_DATA_nPreAchievementID1(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreAchievementID1'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nPreAchievementID1 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nPreAchievementID2 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ACHIEVEMENT_DATA_nPreAchievementID2
static int tolua_get_ACHIEVEMENT_DATA_nPreAchievementID2(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreAchievementID2'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nPreAchievementID2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nPreAchievementID2 of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ACHIEVEMENT_DATA_nPreAchievementID2
static int tolua_set_ACHIEVEMENT_DATA_nPreAchievementID2(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPreAchievementID2'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nPreAchievementID2 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRewardType of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ACHIEVEMENT_DATA_nRewardType
static int tolua_get_data_manager_ACHIEVEMENT_DATA_nRewardType(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nRewardType, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_REWARD_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRewardType[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRewardType of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ACHIEVEMENT_DATA_nRewardType
static int tolua_set_data_manager_ACHIEVEMENT_DATA_nRewardType(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nRewardType, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_REWARD_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nRewardType[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRewardID of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_ACHIEVEMENT_DATA_nRewardID
static int tolua_get_data_manager_ACHIEVEMENT_DATA_nRewardID(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nRewardID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_REWARD_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRewardID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRewardID of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_ACHIEVEMENT_DATA_nRewardID
static int tolua_set_data_manager_ACHIEVEMENT_DATA_nRewardID(lua_State* tolua_S)
{
 int tolua_index;
  ACHIEVEMENT_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (ACHIEVEMENT_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nRewardID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_REWARD_PER_ACHIEVEMENT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nRewardID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAchievementPts of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_get_ACHIEVEMENT_DATA_nAchievementPts
static int tolua_get_ACHIEVEMENT_DATA_nAchievementPts(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAchievementPts'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAchievementPts);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAchievementPts of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_set_ACHIEVEMENT_DATA_nAchievementPts
static int tolua_set_ACHIEVEMENT_DATA_nAchievementPts(lua_State* tolua_S)
{
  ACHIEVEMENT_DATA* self = (ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAchievementPts'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAchievementPts = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  ACHIEVEMENT_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_ACHIEVEMENT_DATA_ReadLine00
static int tolua_data_manager_ACHIEVEMENT_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"ACHIEVEMENT_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"ACHIEVEMENT_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  ACHIEVEMENT_DATA* pData = ((ACHIEVEMENT_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
ACHIEVEMENT_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nHandbookID of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_get_HANDBOOK_DATA_nHandbookID
static int tolua_get_HANDBOOK_DATA_nHandbookID(lua_State* tolua_S)
{
  HANDBOOK_DATA* self = (HANDBOOK_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nHandbookID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nHandbookID of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_set_HANDBOOK_DATA_nHandbookID
static int tolua_set_HANDBOOK_DATA_nHandbookID(lua_State* tolua_S)
{
  HANDBOOK_DATA* self = (HANDBOOK_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nHandbookID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nHandbookType of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_get_HANDBOOK_DATA_nHandbookType
static int tolua_get_HANDBOOK_DATA_nHandbookType(lua_State* tolua_S)
{
  HANDBOOK_DATA* self = (HANDBOOK_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nHandbookType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nHandbookType of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_set_HANDBOOK_DATA_nHandbookType
static int tolua_set_HANDBOOK_DATA_nHandbookType(lua_State* tolua_S)
{
  HANDBOOK_DATA* self = (HANDBOOK_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHandbookType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nHandbookType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nIndex of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_get_HANDBOOK_DATA_nIndex
static int tolua_get_HANDBOOK_DATA_nIndex(lua_State* tolua_S)
{
  HANDBOOK_DATA* self = (HANDBOOK_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nIndex of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_set_HANDBOOK_DATA_nIndex
static int tolua_set_HANDBOOK_DATA_nIndex(lua_State* tolua_S)
{
  HANDBOOK_DATA* self = (HANDBOOK_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nID of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_HANDBOOK_DATA_nID
static int tolua_get_data_manager_HANDBOOK_DATA_nID(lua_State* tolua_S)
{
 int tolua_index;
  HANDBOOK_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (HANDBOOK_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_HANDBOOKITEM_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nID of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_HANDBOOK_DATA_nID
static int tolua_set_data_manager_HANDBOOK_DATA_nID(lua_State* tolua_S)
{
 int tolua_index;
  HANDBOOK_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (HANDBOOK_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->nID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=MAX_HANDBOOKITEM_COUNT)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->nID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: szValue of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_get_HANDBOOK_DATA_szValue
static int tolua_get_HANDBOOK_DATA_szValue(lua_State* tolua_S)
{
  HANDBOOK_DATA* self = (HANDBOOK_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szValue'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->szValue, ClientFixedString::Length(self->szValue));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: szValue of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_set_HANDBOOK_DATA_szValue
static int tolua_set_HANDBOOK_DATA_szValue(lua_State* tolua_S)
{
  HANDBOOK_DATA* self = (HANDBOOK_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'szValue'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->szValue, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  HANDBOOK_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_HANDBOOK_DATA_ReadLine00
static int tolua_data_manager_HANDBOOK_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"HANDBOOK_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"HANDBOOK_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  HANDBOOK_DATA* pData = ((HANDBOOK_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
HANDBOOK_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nTemplateID of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_get_TITLE_TEMPLATE_unsigned_m_nTemplateID
static int tolua_get_TITLE_TEMPLATE_unsigned_m_nTemplateID(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nTemplateID of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_set_TITLE_TEMPLATE_unsigned_m_nTemplateID
static int tolua_set_TITLE_TEMPLATE_unsigned_m_nTemplateID(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nTemplateID = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_szName of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_get_TITLE_TEMPLATE_m_szName
static int tolua_get_TITLE_TEMPLATE_m_szName(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szName'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->m_szName, ClientFixedString::Length(self->m_szName));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_szName of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_set_TITLE_TEMPLATE_m_szName
static int tolua_set_TITLE_TEMPLATE_m_szName(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szName'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->m_szName, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nValidTime of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_get_TITLE_TEMPLATE_m_nValidTime
static int tolua_get_TITLE_TEMPLATE_m_nValidTime(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nValidTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nValidTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nValidTime of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_set_TITLE_TEMPLATE_m_nValidTime
static int tolua_set_TITLE_TEMPLATE_m_nValidTime(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nValidTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nValidTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nPow of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_get_TITLE_TEMPLATE_nPow
static int tolua_get_TITLE_TEMPLATE_nPow(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPow'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nPow);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nPow of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_set_TITLE_TEMPLATE_nPow
static int tolua_set_TITLE_TEMPLATE_nPow(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nPow'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nPow = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nAttrID of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_TITLE_TEMPLATE_m_nAttrID
static int tolua_get_data_manager_TITLE_TEMPLATE_m_nAttrID(lua_State* tolua_S)
{
 int tolua_index;
  TITLE_TEMPLATE* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (TITLE_TEMPLATE*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nAttrID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=TitleAttrTableSize)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nAttrID[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nAttrID of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_TITLE_TEMPLATE_m_nAttrID
static int tolua_set_data_manager_TITLE_TEMPLATE_m_nAttrID(lua_State* tolua_S)
{
 int tolua_index;
  TITLE_TEMPLATE* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (TITLE_TEMPLATE*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nAttrID, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=TitleAttrTableSize)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_nAttrID[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nNodeCount of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_get_TITLE_TEMPLATE_m_nNodeCount
static int tolua_get_TITLE_TEMPLATE_m_nNodeCount(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nNodeCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nNodeCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nNodeCount of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_set_TITLE_TEMPLATE_m_nNodeCount
static int tolua_set_TITLE_TEMPLATE_m_nNodeCount(lua_State* tolua_S)
{
  TITLE_TEMPLATE* self = (TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nNodeCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nNodeCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nNodeIDs of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_TITLE_TEMPLATE_m_nNodeIDs
static int tolua_get_data_manager_TITLE_TEMPLATE_m_nNodeIDs(lua_State* tolua_S)
{
 int tolua_index;
  TITLE_TEMPLATE* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (TITLE_TEMPLATE*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nNodeIDs, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=TitleAttrTableSize)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nNodeIDs[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nNodeIDs of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_TITLE_TEMPLATE_m_nNodeIDs
static int tolua_set_data_manager_TITLE_TEMPLATE_m_nNodeIDs(lua_State* tolua_S)
{
 int tolua_index;
  TITLE_TEMPLATE* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (TITLE_TEMPLATE*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nNodeIDs, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=TitleAttrTableSize)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_nNodeIDs[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: shConType of class  TitleCondition */
#ifndef TOLUA_DISABLE_tolua_get_TITLE_TEMPLATE__TitleCondition_shConType
static int tolua_get_TITLE_TEMPLATE__TitleCondition_shConType(lua_State* tolua_S)
{
  TITLE_TEMPLATE::TitleCondition* self = (TITLE_TEMPLATE::TitleCondition*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'shConType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->shConType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: shConType of class  TitleCondition */
#ifndef TOLUA_DISABLE_tolua_set_TITLE_TEMPLATE__TitleCondition_shConType
static int tolua_set_TITLE_TEMPLATE__TitleCondition_shConType(lua_State* tolua_S)
{
  TITLE_TEMPLATE::TitleCondition* self = (TITLE_TEMPLATE::TitleCondition*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'shConType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->shConType = (( signed short)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: shConValue of class  TitleCondition */
#ifndef TOLUA_DISABLE_tolua_get_TITLE_TEMPLATE__TitleCondition_shConValue
static int tolua_get_TITLE_TEMPLATE__TitleCondition_shConValue(lua_State* tolua_S)
{
  TITLE_TEMPLATE::TitleCondition* self = (TITLE_TEMPLATE::TitleCondition*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'shConValue'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->shConValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: shConValue of class  TitleCondition */
#ifndef TOLUA_DISABLE_tolua_set_TITLE_TEMPLATE__TitleCondition_shConValue
static int tolua_set_TITLE_TEMPLATE__TitleCondition_shConValue(lua_State* tolua_S)
{
  TITLE_TEMPLATE::TitleCondition* self = (TITLE_TEMPLATE::TitleCondition*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'shConValue'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->shConValue = (( signed short)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_Condition of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_TITLE_TEMPLATE_m_Condition
static int tolua_get_data_manager_TITLE_TEMPLATE_m_Condition(lua_State* tolua_S)
{
 int tolua_index;
  TITLE_TEMPLATE* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (TITLE_TEMPLATE*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_Condition, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=2)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)&self->m_Condition[tolua_index],"TITLE_TEMPLATE::TitleCondition");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_Condition of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_TITLE_TEMPLATE_m_Condition
static int tolua_set_data_manager_TITLE_TEMPLATE_m_Condition(lua_State* tolua_S)
{
 int tolua_index;
  TITLE_TEMPLATE* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (TITLE_TEMPLATE*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_Condition, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=2)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_Condition[tolua_index] = *((TITLE_TEMPLATE::TitleCondition*)  tolua_tousertype(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  TITLE_TEMPLATE */
#ifndef TOLUA_DISABLE_tolua_data_manager_TITLE_TEMPLATE_ReadLine00
static int tolua_data_manager_TITLE_TEMPLATE_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"TITLE_TEMPLATE",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"TITLE_TEMPLATE",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  TITLE_TEMPLATE* pData = ((TITLE_TEMPLATE*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
TITLE_TEMPLATE::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwID
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwID
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLevel of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nLevel
static int tolua_get_GUILD_BUILDING_DATA_nLevel(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLevel of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nLevel
static int tolua_set_GUILD_BUILDING_DATA_nLevel(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxLevel of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nMaxLevel
static int tolua_get_GUILD_BUILDING_DATA_nMaxLevel(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxLevel of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nMaxLevel
static int tolua_set_GUILD_BUILDING_DATA_nMaxLevel(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMaxNum of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nMaxNum
static int tolua_get_GUILD_BUILDING_DATA_nMaxNum(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxNum'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMaxNum);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMaxNum of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nMaxNum
static int tolua_set_GUILD_BUILDING_DATA_nMaxNum(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMaxNum'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMaxNum = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nProsperity of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nProsperity
static int tolua_get_GUILD_BUILDING_DATA_nProsperity(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nProsperity'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nProsperity);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nProsperity of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nProsperity
static int tolua_set_GUILD_BUILDING_DATA_nProsperity(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nProsperity'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nProsperity = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMoneyConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nMoneyConsumption
static int tolua_get_GUILD_BUILDING_DATA_nMoneyConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMoneyConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMoneyConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMoneyConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nMoneyConsumption
static int tolua_set_GUILD_BUILDING_DATA_nMoneyConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMoneyConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMoneyConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTotalMoneyConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nTotalMoneyConsumption
static int tolua_get_GUILD_BUILDING_DATA_nTotalMoneyConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalMoneyConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTotalMoneyConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTotalMoneyConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nTotalMoneyConsumption
static int tolua_set_GUILD_BUILDING_DATA_nTotalMoneyConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalMoneyConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTotalMoneyConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nHerbConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nHerbConsumption
static int tolua_get_GUILD_BUILDING_DATA_nHerbConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHerbConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nHerbConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nHerbConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nHerbConsumption
static int tolua_set_GUILD_BUILDING_DATA_nHerbConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nHerbConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nHerbConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTotalHerbConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nTotalHerbConsumption
static int tolua_get_GUILD_BUILDING_DATA_nTotalHerbConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalHerbConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTotalHerbConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTotalHerbConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nTotalHerbConsumption
static int tolua_set_GUILD_BUILDING_DATA_nTotalHerbConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalHerbConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTotalHerbConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nWoodConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nWoodConsumption
static int tolua_get_GUILD_BUILDING_DATA_nWoodConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWoodConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nWoodConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nWoodConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nWoodConsumption
static int tolua_set_GUILD_BUILDING_DATA_nWoodConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nWoodConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nWoodConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTotalWoodConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nTotalWoodConsumption
static int tolua_get_GUILD_BUILDING_DATA_nTotalWoodConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalWoodConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTotalWoodConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTotalWoodConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nTotalWoodConsumption
static int tolua_set_GUILD_BUILDING_DATA_nTotalWoodConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalWoodConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTotalWoodConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nCrystalConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nCrystalConsumption
static int tolua_get_GUILD_BUILDING_DATA_nCrystalConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCrystalConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nCrystalConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nCrystalConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nCrystalConsumption
static int tolua_set_GUILD_BUILDING_DATA_nCrystalConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nCrystalConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nCrystalConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTotalCrystalConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nTotalCrystalConsumption
static int tolua_get_GUILD_BUILDING_DATA_nTotalCrystalConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalCrystalConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTotalCrystalConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTotalCrystalConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nTotalCrystalConsumption
static int tolua_set_GUILD_BUILDING_DATA_nTotalCrystalConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalCrystalConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTotalCrystalConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSilverConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nSilverConsumption
static int tolua_get_GUILD_BUILDING_DATA_nSilverConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSilverConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSilverConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSilverConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nSilverConsumption
static int tolua_set_GUILD_BUILDING_DATA_nSilverConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSilverConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSilverConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nTotalSilverConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nTotalSilverConsumption
static int tolua_get_GUILD_BUILDING_DATA_nTotalSilverConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalSilverConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nTotalSilverConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nTotalSilverConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nTotalSilverConsumption
static int tolua_set_GUILD_BUILDING_DATA_nTotalSilverConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nTotalSilverConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nTotalSilverConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nUpgradeTime of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nUpgradeTime
static int tolua_get_GUILD_BUILDING_DATA_nUpgradeTime(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nUpgradeTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nUpgradeTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nUpgradeTime of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nUpgradeTime
static int tolua_set_GUILD_BUILDING_DATA_nUpgradeTime(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nUpgradeTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nUpgradeTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRoutineConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nRoutineConsumption
static int tolua_get_GUILD_BUILDING_DATA_nRoutineConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRoutineConsumption'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRoutineConsumption);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRoutineConsumption of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nRoutineConsumption
static int tolua_set_GUILD_BUILDING_DATA_nRoutineConsumption(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRoutineConsumption'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRoutineConsumption = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwCdtnBaseNpcID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwCdtnBaseNpcID
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwCdtnBaseNpcID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwCdtnBaseNpcID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwCdtnBaseNpcID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwCdtnBaseNpcID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwCdtnBaseNpcID
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwCdtnBaseNpcID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwCdtnBaseNpcID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwCdtnBaseNpcID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwConditionBuildingID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwConditionBuildingID
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwConditionBuildingID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwConditionBuildingID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwConditionBuildingID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwConditionBuildingID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwConditionBuildingID
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwConditionBuildingID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwConditionBuildingID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwConditionBuildingID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nConditionBuildingLvl of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nConditionBuildingLvl
static int tolua_get_GUILD_BUILDING_DATA_nConditionBuildingLvl(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConditionBuildingLvl'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nConditionBuildingLvl);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nConditionBuildingLvl of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nConditionBuildingLvl
static int tolua_set_GUILD_BUILDING_DATA_nConditionBuildingLvl(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConditionBuildingLvl'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nConditionBuildingLvl = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwUpgradingSpellID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwUpgradingSpellID
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwUpgradingSpellID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwUpgradingSpellID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwUpgradingSpellID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwUpgradingSpellID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwUpgradingSpellID
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwUpgradingSpellID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwUpgradingSpellID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwUpgradingSpellID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwNpcTemplateID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwNpcTemplateID
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwNpcTemplateID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwNpcTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwNpcTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwNpcTemplateID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwNpcTemplateID
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwNpcTemplateID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwNpcTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwNpcTemplateID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwDeadNpcTemplateID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwDeadNpcTemplateID
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwDeadNpcTemplateID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwDeadNpcTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwDeadNpcTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwDeadNpcTemplateID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwDeadNpcTemplateID
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwDeadNpcTemplateID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwDeadNpcTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwDeadNpcTemplateID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nReviveTime of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_nReviveTime
static int tolua_get_GUILD_BUILDING_DATA_unsigned_nReviveTime(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReviveTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nReviveTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nReviveTime of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_nReviveTime
static int tolua_set_GUILD_BUILDING_DATA_unsigned_nReviveTime(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nReviveTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nReviveTime = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nParam1 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nParam1
static int tolua_get_GUILD_BUILDING_DATA_nParam1(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam1'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nParam1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nParam1 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nParam1
static int tolua_set_GUILD_BUILDING_DATA_nParam1(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam1'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nParam1 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nParam2 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nParam2
static int tolua_get_GUILD_BUILDING_DATA_nParam2(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam2'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nParam2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nParam2 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nParam2
static int tolua_set_GUILD_BUILDING_DATA_nParam2(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam2'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nParam2 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nParam3 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nParam3
static int tolua_get_GUILD_BUILDING_DATA_nParam3(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam3'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nParam3);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nParam3 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nParam3
static int tolua_set_GUILD_BUILDING_DATA_nParam3(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam3'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nParam3 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nParam4 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nParam4
static int tolua_get_GUILD_BUILDING_DATA_nParam4(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam4'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nParam4);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nParam4 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nParam4
static int tolua_set_GUILD_BUILDING_DATA_nParam4(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam4'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nParam4 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nParam5 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_nParam5
static int tolua_get_GUILD_BUILDING_DATA_nParam5(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam5'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nParam5);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nParam5 of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_nParam5
static int tolua_set_GUILD_BUILDING_DATA_nParam5(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nParam5'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nParam5 = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwIntruderMsgID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwIntruderMsgID
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwIntruderMsgID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwIntruderMsgID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwIntruderMsgID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwIntruderMsgID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwIntruderMsgID
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwIntruderMsgID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwIntruderMsgID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwIntruderMsgID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwDefenderMsgID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwDefenderMsgID
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwDefenderMsgID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwDefenderMsgID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwDefenderMsgID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwDefenderMsgID of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwDefenderMsgID
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwDefenderMsgID(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwDefenderMsgID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwDefenderMsgID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwFewMembersReward of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwFewMembersReward
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwFewMembersReward(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwFewMembersReward'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwFewMembersReward);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwFewMembersReward of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwFewMembersReward
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwFewMembersReward(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwFewMembersReward'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwFewMembersReward = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwSomeMembersReward of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwSomeMembersReward
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwSomeMembersReward(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwSomeMembersReward'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwSomeMembersReward);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwSomeMembersReward of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwSomeMembersReward
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwSomeMembersReward(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwSomeMembersReward'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwSomeMembersReward = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwManyMembersReward of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_BUILDING_DATA_unsigned_dwManyMembersReward
static int tolua_get_GUILD_BUILDING_DATA_unsigned_dwManyMembersReward(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwManyMembersReward'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwManyMembersReward);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwManyMembersReward of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_BUILDING_DATA_unsigned_dwManyMembersReward
static int tolua_set_GUILD_BUILDING_DATA_unsigned_dwManyMembersReward(lua_State* tolua_S)
{
  GUILD_BUILDING_DATA* self = (GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwManyMembersReward'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwManyMembersReward = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasName of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_GUILD_BUILDING_DATA_HasName00
static int tolua_data_manager_GUILD_BUILDING_DATA_HasName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"GUILD_BUILDING_DATA",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
 int tolua_ret = (int)
GUILD_BUILDING_DATA::HasName();
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  GUILD_BUILDING_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_GUILD_BUILDING_DATA_ReadLine00
static int tolua_data_manager_GUILD_BUILDING_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"GUILD_BUILDING_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"GUILD_BUILDING_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  GUILD_BUILDING_DATA* pData = ((GUILD_BUILDING_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
GUILD_BUILDING_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nType of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nType
static int tolua_get_GUILD_ACTIVITY_DATA_nType(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nType of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nType
static int tolua_set_GUILD_ACTIVITY_DATA_nType(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLevel of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nLevel
static int tolua_get_GUILD_ACTIVITY_DATA_nLevel(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLevel of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nLevel
static int tolua_set_GUILD_ACTIVITY_DATA_nLevel(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nGuildVariableIndex of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nGuildVariableIndex
static int tolua_get_GUILD_ACTIVITY_DATA_nGuildVariableIndex(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nGuildVariableIndex'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nGuildVariableIndex);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nGuildVariableIndex of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nGuildVariableIndex
static int tolua_set_GUILD_ACTIVITY_DATA_nGuildVariableIndex(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nGuildVariableIndex'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nGuildVariableIndex = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLastTime of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nLastTime
static int tolua_get_GUILD_ACTIVITY_DATA_nLastTime(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLastTime'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLastTime);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLastTime of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nLastTime
static int tolua_set_GUILD_ACTIVITY_DATA_nLastTime(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLastTime'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLastTime = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nRepeatNum of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nRepeatNum
static int tolua_get_GUILD_ACTIVITY_DATA_nRepeatNum(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRepeatNum'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nRepeatNum);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nRepeatNum of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nRepeatNum
static int tolua_set_GUILD_ACTIVITY_DATA_nRepeatNum(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nRepeatNum'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nRepeatNum = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBaseBuildingType of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nBaseBuildingType
static int tolua_get_GUILD_ACTIVITY_DATA_nBaseBuildingType(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBaseBuildingType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBaseBuildingType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBaseBuildingType of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nBaseBuildingType
static int tolua_set_GUILD_ACTIVITY_DATA_nBaseBuildingType(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBaseBuildingType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBaseBuildingType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBaseBuildingLevel of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nBaseBuildingLevel
static int tolua_get_GUILD_ACTIVITY_DATA_nBaseBuildingLevel(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBaseBuildingLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBaseBuildingLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBaseBuildingLevel of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nBaseBuildingLevel
static int tolua_set_GUILD_ACTIVITY_DATA_nBaseBuildingLevel(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBaseBuildingLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBaseBuildingLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nConsumeMoney of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nConsumeMoney
static int tolua_get_GUILD_ACTIVITY_DATA_nConsumeMoney(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeMoney'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nConsumeMoney);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nConsumeMoney of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nConsumeMoney
static int tolua_set_GUILD_ACTIVITY_DATA_nConsumeMoney(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeMoney'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nConsumeMoney = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nConsumeHerb of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nConsumeHerb
static int tolua_get_GUILD_ACTIVITY_DATA_nConsumeHerb(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeHerb'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nConsumeHerb);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nConsumeHerb of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nConsumeHerb
static int tolua_set_GUILD_ACTIVITY_DATA_nConsumeHerb(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeHerb'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nConsumeHerb = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nConsumeWood of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nConsumeWood
static int tolua_get_GUILD_ACTIVITY_DATA_nConsumeWood(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeWood'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nConsumeWood);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nConsumeWood of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nConsumeWood
static int tolua_set_GUILD_ACTIVITY_DATA_nConsumeWood(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeWood'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nConsumeWood = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nConsumeCrystal of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nConsumeCrystal
static int tolua_get_GUILD_ACTIVITY_DATA_nConsumeCrystal(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeCrystal'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nConsumeCrystal);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nConsumeCrystal of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nConsumeCrystal
static int tolua_set_GUILD_ACTIVITY_DATA_nConsumeCrystal(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeCrystal'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nConsumeCrystal = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nConsumeSilver of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_nConsumeSilver
static int tolua_get_GUILD_ACTIVITY_DATA_nConsumeSilver(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeSilver'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nConsumeSilver);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nConsumeSilver of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_nConsumeSilver
static int tolua_set_GUILD_ACTIVITY_DATA_nConsumeSilver(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nConsumeSilver'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nConsumeSilver = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: ncltLightingID of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GUILD_ACTIVITY_DATA_ncltLightingID
static int tolua_get_GUILD_ACTIVITY_DATA_ncltLightingID(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncltLightingID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->ncltLightingID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: ncltLightingID of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GUILD_ACTIVITY_DATA_ncltLightingID
static int tolua_set_GUILD_ACTIVITY_DATA_ncltLightingID(lua_State* tolua_S)
{
  GUILD_ACTIVITY_DATA* self = (GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncltLightingID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->ncltLightingID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  GUILD_ACTIVITY_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_GUILD_ACTIVITY_DATA_ReadLine00
static int tolua_data_manager_GUILD_ACTIVITY_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"GUILD_ACTIVITY_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"GUILD_ACTIVITY_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  GUILD_ACTIVITY_DATA* pData = ((GUILD_ACTIVITY_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
GUILD_ACTIVITY_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nTemplateID of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SOUL_DATA_m_nTemplateID
static int tolua_get_SOUL_DATA_m_nTemplateID(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nTemplateID of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SOUL_DATA_m_nTemplateID
static int tolua_set_SOUL_DATA_m_nTemplateID(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nTemplateID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nAttrID of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SOUL_DATA_m_nAttrID
static int tolua_get_SOUL_DATA_m_nAttrID(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nAttrID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nAttrID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nAttrID of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SOUL_DATA_m_nAttrID
static int tolua_set_SOUL_DATA_m_nAttrID(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nAttrID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nAttrID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nMatchSocket of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SOUL_DATA_m_nMatchSocket
static int tolua_get_SOUL_DATA_m_nMatchSocket(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMatchSocket'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nMatchSocket);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nMatchSocket of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SOUL_DATA_m_nMatchSocket
static int tolua_set_SOUL_DATA_m_nMatchSocket(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nMatchSocket'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nMatchSocket = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nSoulLevel of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SOUL_DATA_m_nSoulLevel
static int tolua_get_SOUL_DATA_m_nSoulLevel(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSoulLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nSoulLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nSoulLevel of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SOUL_DATA_m_nSoulLevel
static int tolua_set_SOUL_DATA_m_nSoulLevel(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nSoulLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nSoulLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nPriceValue of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SOUL_DATA_m_nPriceValue
static int tolua_get_SOUL_DATA_m_nPriceValue(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nPriceValue'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nPriceValue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nPriceValue of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SOUL_DATA_m_nPriceValue
static int tolua_set_SOUL_DATA_m_nPriceValue(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nPriceValue'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nPriceValue = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nUpgradeSoulID of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SOUL_DATA_m_nUpgradeSoulID
static int tolua_get_SOUL_DATA_m_nUpgradeSoulID(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nUpgradeSoulID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nUpgradeSoulID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nUpgradeSoulID of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SOUL_DATA_m_nUpgradeSoulID
static int tolua_set_SOUL_DATA_m_nUpgradeSoulID(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nUpgradeSoulID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nUpgradeSoulID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nRemovePrice of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SOUL_DATA_m_nRemovePrice
static int tolua_get_SOUL_DATA_m_nRemovePrice(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nRemovePrice'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nRemovePrice);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nRemovePrice of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_set_SOUL_DATA_m_nRemovePrice
static int tolua_set_SOUL_DATA_m_nRemovePrice(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nRemovePrice'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nRemovePrice = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_pAttrData of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_get_SOUL_DATA_m_pAttrData_ptr
static int tolua_get_SOUL_DATA_m_pAttrData_ptr(lua_State* tolua_S)
{
  SOUL_DATA* self = (SOUL_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_pAttrData'",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)self->m_pAttrData,"const ATTRIBUTE_DATA");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  SOUL_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_SOUL_DATA_ReadLine00
static int tolua_data_manager_SOUL_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"SOUL_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"SOUL_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  SOUL_DATA* pData = ((SOUL_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
SOUL_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nID of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nID
static int tolua_get_PET_DATA_nID(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nID of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nID
static int tolua_set_PET_DATA_nID(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAIID of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nAIID
static int tolua_get_PET_DATA_nAIID(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAIID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAIID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAIID of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nAIID
static int tolua_set_PET_DATA_nAIID(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAIID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAIID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBornType of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nBornType
static int tolua_get_PET_DATA_nBornType(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBornType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBornType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBornType of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nBornType
static int tolua_set_PET_DATA_nBornType(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBornType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBornType = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nLevel of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nLevel
static int tolua_get_PET_DATA_nLevel(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nLevel of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nLevel
static int tolua_set_PET_DATA_nLevel(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nAccurate of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nAccurate
static int tolua_get_PET_DATA_nAccurate(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAccurate'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nAccurate);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nAccurate of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nAccurate
static int tolua_set_PET_DATA_nAccurate(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nAccurate'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nAccurate = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBonusphyAtkMax of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nBonusphyAtkMax
static int tolua_get_PET_DATA_nBonusphyAtkMax(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBonusphyAtkMax'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBonusphyAtkMax);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBonusphyAtkMax of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nBonusphyAtkMax
static int tolua_set_PET_DATA_nBonusphyAtkMax(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBonusphyAtkMax'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBonusphyAtkMax = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBonusHPMaxA of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nBonusHPMaxA
static int tolua_get_PET_DATA_nBonusHPMaxA(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBonusHPMaxA'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBonusHPMaxA);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBonusHPMaxA of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nBonusHPMaxA
static int tolua_set_PET_DATA_nBonusHPMaxA(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBonusHPMaxA'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBonusHPMaxA = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nBonusHPMaxB of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nBonusHPMaxB
static int tolua_get_PET_DATA_nBonusHPMaxB(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBonusHPMaxB'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nBonusHPMaxB);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nBonusHPMaxB of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nBonusHPMaxB
static int tolua_set_PET_DATA_nBonusHPMaxB(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nBonusHPMaxB'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nBonusHPMaxB = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMagDef of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nMagDef
static int tolua_get_PET_DATA_nMagDef(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMagDef'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMagDef);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMagDef of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nMagDef
static int tolua_set_PET_DATA_nMagDef(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMagDef'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMagDef = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nMagDefMax of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_get_PET_DATA_nMagDefMax
static int tolua_get_PET_DATA_nMagDefMax(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMagDefMax'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nMagDefMax);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nMagDefMax of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_set_PET_DATA_nMagDefMax
static int tolua_set_PET_DATA_nMagDefMax(lua_State* tolua_S)
{
  PET_DATA* self = (PET_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nMagDefMax'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nMagDefMax = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  PET_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_PET_DATA_ReadLine00
static int tolua_data_manager_PET_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"PET_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"PET_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  PET_DATA* pData = ((PET_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
PET_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GetPetData */
#ifndef TOLUA_DISABLE_tolua_data_manager_GetPetData00
static int tolua_data_manager_GetPetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int nType = (( signed int)  tolua_tonumber(tolua_S,1,0));
  signed int nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0));
 {
  const PET_DATA* tolua_ret = (const PET_DATA*)  GetPetData(nType,nLevel);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const PET_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nTemplateID of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_MAIL_TEMPLATE_DATA_m_nTemplateID
static int tolua_get_MAIL_TEMPLATE_DATA_m_nTemplateID(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nTemplateID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nTemplateID of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_MAIL_TEMPLATE_DATA_m_nTemplateID
static int tolua_set_MAIL_TEMPLATE_DATA_m_nTemplateID(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nTemplateID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nTemplateID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_szMailSubject of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_MAIL_TEMPLATE_DATA_m_szMailSubject
static int tolua_get_MAIL_TEMPLATE_DATA_m_szMailSubject(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szMailSubject'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->m_szMailSubject, ClientFixedString::Length(self->m_szMailSubject));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_szMailSubject of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_MAIL_TEMPLATE_DATA_m_szMailSubject
static int tolua_set_MAIL_TEMPLATE_DATA_m_szMailSubject(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szMailSubject'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->m_szMailSubject, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_szMailContent of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_MAIL_TEMPLATE_DATA_m_szMailContent
static int tolua_get_MAIL_TEMPLATE_DATA_m_szMailContent(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szMailContent'",NULL);
#endif
 if (!self) return 0;
 lua_pushlstring(tolua_S, self->m_szMailContent, ClientFixedString::Length(self->m_szMailContent));
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_szMailContent of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_MAIL_TEMPLATE_DATA_m_szMailContent
static int tolua_set_MAIL_TEMPLATE_DATA_m_szMailContent(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_szMailContent'",NULL);
 if (!tolua_isstring(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
 if (!self) return 0;
 ClientFixedString::Assign(self->m_szMailContent, tolua_tostring(tolua_S,2,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nItemID of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_MAIL_TEMPLATE_DATA_m_nItemID
static int tolua_get_MAIL_TEMPLATE_DATA_m_nItemID(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nItemID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nItemID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nItemID of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_MAIL_TEMPLATE_DATA_m_nItemID
static int tolua_set_MAIL_TEMPLATE_DATA_m_nItemID(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nItemID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nItemID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nItemCount of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_MAIL_TEMPLATE_DATA_m_nItemCount
static int tolua_get_MAIL_TEMPLATE_DATA_m_nItemCount(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nItemCount'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nItemCount);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nItemCount of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_MAIL_TEMPLATE_DATA_m_nItemCount
static int tolua_set_MAIL_TEMPLATE_DATA_m_nItemCount(lua_State* tolua_S)
{
  MAIL_TEMPLATE_DATA* self = (MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_nItemCount'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_nItemCount = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  MAIL_TEMPLATE_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_MAIL_TEMPLATE_DATA_ReadLine00
static int tolua_data_manager_MAIL_TEMPLATE_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"MAIL_TEMPLATE_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"MAIL_TEMPLATE_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  MAIL_TEMPLATE_DATA* pData = ((MAIL_TEMPLATE_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
MAIL_TEMPLATE_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byEquipType of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_EQUIP_DATA_unsigned_m_byEquipType
static int tolua_get_EQUIP_DATA_unsigned_m_byEquipType(lua_State* tolua_S)
{
  EQUIP_DATA* self = (EQUIP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byEquipType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byEquipType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byEquipType of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_EQUIP_DATA_unsigned_m_byEquipType
static int tolua_set_EQUIP_DATA_unsigned_m_byEquipType(lua_State* tolua_S)
{
  EQUIP_DATA* self = (EQUIP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byEquipType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byEquipType = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_byIdentified of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_EQUIP_DATA_unsigned_m_byIdentified
static int tolua_get_EQUIP_DATA_unsigned_m_byIdentified(lua_State* tolua_S)
{
  EQUIP_DATA* self = (EQUIP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byIdentified'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_byIdentified);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_byIdentified of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_EQUIP_DATA_unsigned_m_byIdentified
static int tolua_set_EQUIP_DATA_unsigned_m_byIdentified(lua_State* tolua_S)
{
  EQUIP_DATA* self = (EQUIP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_byIdentified'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_byIdentified = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_bySocketCountMin of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_EQUIP_DATA_unsigned_m_bySocketCountMin
static int tolua_get_EQUIP_DATA_unsigned_m_bySocketCountMin(lua_State* tolua_S)
{
  EQUIP_DATA* self = (EQUIP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bySocketCountMin'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_bySocketCountMin);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_bySocketCountMin of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_EQUIP_DATA_unsigned_m_bySocketCountMin
static int tolua_set_EQUIP_DATA_unsigned_m_bySocketCountMin(lua_State* tolua_S)
{
  EQUIP_DATA* self = (EQUIP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bySocketCountMin'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_bySocketCountMin = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_bySocketCountMax of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_EQUIP_DATA_unsigned_m_bySocketCountMax
static int tolua_get_EQUIP_DATA_unsigned_m_bySocketCountMax(lua_State* tolua_S)
{
  EQUIP_DATA* self = (EQUIP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bySocketCountMax'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_bySocketCountMax);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_bySocketCountMax of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_EQUIP_DATA_unsigned_m_bySocketCountMax
static int tolua_set_EQUIP_DATA_unsigned_m_bySocketCountMax(lua_State* tolua_S)
{
  EQUIP_DATA* self = (EQUIP_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'm_bySocketCountMax'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->m_bySocketCountMax = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: m_nFixAttr of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_get_data_manager_EQUIP_DATA_m_nFixAttr
static int tolua_get_data_manager_EQUIP_DATA_m_nFixAttr(lua_State* tolua_S)
{
 int tolua_index;
  EQUIP_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (EQUIP_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nFixAttr, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=FixAttrTableSize)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->m_nFixAttr[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: m_nFixAttr of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_set_data_manager_EQUIP_DATA_m_nFixAttr
static int tolua_set_data_manager_EQUIP_DATA_m_nFixAttr(lua_State* tolua_S)
{
 int tolua_index;
  EQUIP_DATA* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (EQUIP_DATA*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
 tolua_Error tolua_err;
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 if (!self || !ClientLuaArray::Index(tolua_tonumber(tolua_S,2,0), self->m_nFixAttr, tolua_index)) return 0;
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=FixAttrTableSize)
 tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->m_nFixAttr[tolua_index] = (( signed int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReadLine of class  EQUIP_DATA */
#ifndef TOLUA_DISABLE_tolua_data_manager_EQUIP_DATA_ReadLine00
static int tolua_data_manager_EQUIP_DATA_ReadLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"EQUIP_DATA",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,4,"EQUIP_DATA",0,&tolua_err) ||
 !tolua_isusertype(tolua_S,5,"CTabFile",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnID = (( signed int)  tolua_tonumber(tolua_S,2,0));
  const char* rpcszName = ((const char*)  tolua_tostring(tolua_S,3,0));
  EQUIP_DATA* pData = ((EQUIP_DATA*)  tolua_tousertype(tolua_S,4,0));
  CTabFile* pTabConfig = ((CTabFile*)  tolua_tousertype(tolua_S,5,0));
  signed int nRow = (( signed int)  tolua_tonumber(tolua_S,6,0));
 {
 int tolua_ret = (int)
EQUIP_DATA::ReadLine(rnID,rpcszName,pData,pTabConfig,nRow);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 tolua_pushnumber(tolua_S,(lua_Number)rnID);
 tolua_pushstring(tolua_S,(const char*)rpcszName);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReadLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<NEW_AI_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetInstance00
static int tolua_data_manager_CDataManager_NEW_AI_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<NEW_AI_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<NEW_AI_DATA>& tolua_ret = (CDataManager<NEW_AI_DATA>&)  CDataManager<NEW_AI_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<NEW_AI_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<NEW_AI_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_NEW_AI_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NEW_AI_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NEW_AI_DATA>* self = (CDataManager<NEW_AI_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<NEW_AI_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetData00
static int tolua_data_manager_CDataManager_NEW_AI_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NEW_AI_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NEW_AI_DATA>* self = (CDataManager<NEW_AI_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const NEW_AI_DATA* tolua_ret = (const NEW_AI_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const NEW_AI_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<NEW_AI_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetID00
static int tolua_data_manager_CDataManager_NEW_AI_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NEW_AI_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NEW_AI_DATA>* self = (CDataManager<NEW_AI_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<NEW_AI_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_NEW_AI_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NEW_AI_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NEW_AI_DATA>* self = (CDataManager<NEW_AI_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const NEW_AI_DATA* tolua_ret = (const NEW_AI_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const NEW_AI_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<NEW_AI_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetNextData00
static int tolua_data_manager_CDataManager_NEW_AI_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NEW_AI_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NEW_AI_DATA>* self = (CDataManager<NEW_AI_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const NEW_AI_DATA* tolua_ret = (const NEW_AI_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const NEW_AI_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<AURA_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetInstance00
static int tolua_data_manager_CDataManager_AURA_ENTRY__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<AURA_ENTRY>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<AURA_ENTRY>& tolua_ret = (CDataManager<AURA_ENTRY>&)  CDataManager<AURA_ENTRY>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<AURA_ENTRY>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<AURA_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetDataCount00
static int tolua_data_manager_CDataManager_AURA_ENTRY__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<AURA_ENTRY>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<AURA_ENTRY>* self = (CDataManager<AURA_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<AURA_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetData00
static int tolua_data_manager_CDataManager_AURA_ENTRY__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<AURA_ENTRY>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<AURA_ENTRY>* self = (CDataManager<AURA_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const AURA_ENTRY* tolua_ret = (const AURA_ENTRY*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const AURA_ENTRY");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<AURA_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetID00
static int tolua_data_manager_CDataManager_AURA_ENTRY__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<AURA_ENTRY>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<AURA_ENTRY>* self = (CDataManager<AURA_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<AURA_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetFirstData00
static int tolua_data_manager_CDataManager_AURA_ENTRY__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<AURA_ENTRY>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<AURA_ENTRY>* self = (CDataManager<AURA_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const AURA_ENTRY* tolua_ret = (const AURA_ENTRY*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const AURA_ENTRY");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<AURA_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetNextData00
static int tolua_data_manager_CDataManager_AURA_ENTRY__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<AURA_ENTRY>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<AURA_ENTRY>* self = (CDataManager<AURA_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const AURA_ENTRY* tolua_ret = (const AURA_ENTRY*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const AURA_ENTRY");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<DOODAD_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetInstance00
static int tolua_data_manager_CDataManager_DOODAD_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<DOODAD_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<DOODAD_DATA>& tolua_ret = (CDataManager<DOODAD_DATA>&)  CDataManager<DOODAD_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<DOODAD_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<DOODAD_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_DOODAD_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<DOODAD_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<DOODAD_DATA>* self = (CDataManager<DOODAD_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<DOODAD_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetData00
static int tolua_data_manager_CDataManager_DOODAD_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<DOODAD_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<DOODAD_DATA>* self = (CDataManager<DOODAD_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const DOODAD_DATA* tolua_ret = (const DOODAD_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const DOODAD_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<DOODAD_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetID00
static int tolua_data_manager_CDataManager_DOODAD_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<DOODAD_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<DOODAD_DATA>* self = (CDataManager<DOODAD_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<DOODAD_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_DOODAD_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<DOODAD_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<DOODAD_DATA>* self = (CDataManager<DOODAD_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const DOODAD_DATA* tolua_ret = (const DOODAD_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const DOODAD_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<DOODAD_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetNextData00
static int tolua_data_manager_CDataManager_DOODAD_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<DOODAD_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<DOODAD_DATA>* self = (CDataManager<DOODAD_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const DOODAD_DATA* tolua_ret = (const DOODAD_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const DOODAD_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<EQUIP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetInstance00
static int tolua_data_manager_CDataManager_EQUIP_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<EQUIP_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<EQUIP_DATA>& tolua_ret = (CDataManager<EQUIP_DATA>&)  CDataManager<EQUIP_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<EQUIP_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<EQUIP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_EQUIP_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<EQUIP_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<EQUIP_DATA>* self = (CDataManager<EQUIP_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<EQUIP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetData00
static int tolua_data_manager_CDataManager_EQUIP_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<EQUIP_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<EQUIP_DATA>* self = (CDataManager<EQUIP_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const EQUIP_DATA* tolua_ret = (const EQUIP_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const EQUIP_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<EQUIP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetID00
static int tolua_data_manager_CDataManager_EQUIP_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<EQUIP_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<EQUIP_DATA>* self = (CDataManager<EQUIP_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<EQUIP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_EQUIP_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<EQUIP_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<EQUIP_DATA>* self = (CDataManager<EQUIP_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const EQUIP_DATA* tolua_ret = (const EQUIP_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const EQUIP_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<EQUIP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetNextData00
static int tolua_data_manager_CDataManager_EQUIP_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<EQUIP_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<EQUIP_DATA>* self = (CDataManager<EQUIP_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const EQUIP_DATA* tolua_ret = (const EQUIP_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const EQUIP_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<INIT_REPUTATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetInstance00
static int tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<INIT_REPUTATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<INIT_REPUTATION_DATA>& tolua_ret = (CDataManager<INIT_REPUTATION_DATA>&)  CDataManager<INIT_REPUTATION_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<INIT_REPUTATION_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<INIT_REPUTATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<INIT_REPUTATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<INIT_REPUTATION_DATA>* self = (CDataManager<INIT_REPUTATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<INIT_REPUTATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetData00
static int tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<INIT_REPUTATION_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<INIT_REPUTATION_DATA>* self = (CDataManager<INIT_REPUTATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const INIT_REPUTATION_DATA* tolua_ret = (const INIT_REPUTATION_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const INIT_REPUTATION_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<INIT_REPUTATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetID00
static int tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<INIT_REPUTATION_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<INIT_REPUTATION_DATA>* self = (CDataManager<INIT_REPUTATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<INIT_REPUTATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<INIT_REPUTATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<INIT_REPUTATION_DATA>* self = (CDataManager<INIT_REPUTATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const INIT_REPUTATION_DATA* tolua_ret = (const INIT_REPUTATION_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const INIT_REPUTATION_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<INIT_REPUTATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetNextData00
static int tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<INIT_REPUTATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<INIT_REPUTATION_DATA>* self = (CDataManager<INIT_REPUTATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const INIT_REPUTATION_DATA* tolua_ret = (const INIT_REPUTATION_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const INIT_REPUTATION_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<ITEM_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetInstance00
static int tolua_data_manager_CDataManager_ITEM_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<ITEM_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<ITEM_DATA>& tolua_ret = (CDataManager<ITEM_DATA>&)  CDataManager<ITEM_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<ITEM_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<ITEM_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_ITEM_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ITEM_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ITEM_DATA>* self = (CDataManager<ITEM_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<ITEM_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetData00
static int tolua_data_manager_CDataManager_ITEM_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ITEM_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ITEM_DATA>* self = (CDataManager<ITEM_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const ITEM_DATA* tolua_ret = (const ITEM_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ITEM_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<ITEM_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetID00
static int tolua_data_manager_CDataManager_ITEM_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ITEM_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ITEM_DATA>* self = (CDataManager<ITEM_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<ITEM_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_ITEM_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ITEM_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ITEM_DATA>* self = (CDataManager<ITEM_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const ITEM_DATA* tolua_ret = (const ITEM_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ITEM_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<ITEM_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetNextData00
static int tolua_data_manager_CDataManager_ITEM_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ITEM_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ITEM_DATA>* self = (CDataManager<ITEM_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const ITEM_DATA* tolua_ret = (const ITEM_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ITEM_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<ROLE_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetInstance00
static int tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<ROLE_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<ROLE_LEVEL_DATA>& tolua_ret = (CDataManager<ROLE_LEVEL_DATA>&)  CDataManager<ROLE_LEVEL_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<ROLE_LEVEL_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<ROLE_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_LEVEL_DATA>* self = (CDataManager<ROLE_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<ROLE_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetData00
static int tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_LEVEL_DATA>* self = (CDataManager<ROLE_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const ROLE_LEVEL_DATA* tolua_ret = (const ROLE_LEVEL_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ROLE_LEVEL_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<ROLE_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetID00
static int tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_LEVEL_DATA>* self = (CDataManager<ROLE_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<ROLE_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_LEVEL_DATA>* self = (CDataManager<ROLE_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const ROLE_LEVEL_DATA* tolua_ret = (const ROLE_LEVEL_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ROLE_LEVEL_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<ROLE_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetNextData00
static int tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_LEVEL_DATA>* self = (CDataManager<ROLE_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const ROLE_LEVEL_DATA* tolua_ret = (const ROLE_LEVEL_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ROLE_LEVEL_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<HERO_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetInstance00
static int tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<HERO_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<HERO_LEVEL_DATA>& tolua_ret = (CDataManager<HERO_LEVEL_DATA>&)  CDataManager<HERO_LEVEL_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<HERO_LEVEL_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<HERO_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<HERO_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<HERO_LEVEL_DATA>* self = (CDataManager<HERO_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<HERO_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetData00
static int tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<HERO_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<HERO_LEVEL_DATA>* self = (CDataManager<HERO_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const HERO_LEVEL_DATA* tolua_ret = (const HERO_LEVEL_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const HERO_LEVEL_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<HERO_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetID00
static int tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<HERO_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<HERO_LEVEL_DATA>* self = (CDataManager<HERO_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<HERO_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<HERO_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<HERO_LEVEL_DATA>* self = (CDataManager<HERO_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const HERO_LEVEL_DATA* tolua_ret = (const HERO_LEVEL_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const HERO_LEVEL_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<HERO_LEVEL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetNextData00
static int tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<HERO_LEVEL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<HERO_LEVEL_DATA>* self = (CDataManager<HERO_LEVEL_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const HERO_LEVEL_DATA* tolua_ret = (const HERO_LEVEL_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const HERO_LEVEL_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<MISSILE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetInstance00
static int tolua_data_manager_CDataManager_MISSILE_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<MISSILE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<MISSILE_DATA>& tolua_ret = (CDataManager<MISSILE_DATA>&)  CDataManager<MISSILE_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<MISSILE_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<MISSILE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_MISSILE_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MISSILE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MISSILE_DATA>* self = (CDataManager<MISSILE_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<MISSILE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetData00
static int tolua_data_manager_CDataManager_MISSILE_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MISSILE_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MISSILE_DATA>* self = (CDataManager<MISSILE_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const MISSILE_DATA* tolua_ret = (const MISSILE_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const MISSILE_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<MISSILE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetID00
static int tolua_data_manager_CDataManager_MISSILE_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MISSILE_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MISSILE_DATA>* self = (CDataManager<MISSILE_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<MISSILE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_MISSILE_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MISSILE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MISSILE_DATA>* self = (CDataManager<MISSILE_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const MISSILE_DATA* tolua_ret = (const MISSILE_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const MISSILE_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<MISSILE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetNextData00
static int tolua_data_manager_CDataManager_MISSILE_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MISSILE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MISSILE_DATA>* self = (CDataManager<MISSILE_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const MISSILE_DATA* tolua_ret = (const MISSILE_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const MISSILE_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<NPC_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetInstance00
static int tolua_data_manager_CDataManager_NPC_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<NPC_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<NPC_DATA>& tolua_ret = (CDataManager<NPC_DATA>&)  CDataManager<NPC_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<NPC_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<NPC_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_NPC_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_DATA>* self = (CDataManager<NPC_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<NPC_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetData00
static int tolua_data_manager_CDataManager_NPC_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_DATA>* self = (CDataManager<NPC_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const NPC_DATA* tolua_ret = (const NPC_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const NPC_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<NPC_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetID00
static int tolua_data_manager_CDataManager_NPC_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_DATA>* self = (CDataManager<NPC_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<NPC_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_NPC_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_DATA>* self = (CDataManager<NPC_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const NPC_DATA* tolua_ret = (const NPC_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const NPC_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<NPC_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetNextData00
static int tolua_data_manager_CDataManager_NPC_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_DATA>* self = (CDataManager<NPC_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const NPC_DATA* tolua_ret = (const NPC_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const NPC_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<NPC_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetInstance00
static int tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<NPC_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<NPC_RELATION_DATA>& tolua_ret = (CDataManager<NPC_RELATION_DATA>&)  CDataManager<NPC_RELATION_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<NPC_RELATION_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<NPC_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_RELATION_DATA>* self = (CDataManager<NPC_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<NPC_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetData00
static int tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_RELATION_DATA>* self = (CDataManager<NPC_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const NPC_RELATION_DATA* tolua_ret = (const NPC_RELATION_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const NPC_RELATION_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<NPC_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetID00
static int tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_RELATION_DATA>* self = (CDataManager<NPC_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<NPC_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_RELATION_DATA>* self = (CDataManager<NPC_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const NPC_RELATION_DATA* tolua_ret = (const NPC_RELATION_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const NPC_RELATION_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<NPC_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetNextData00
static int tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<NPC_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<NPC_RELATION_DATA>* self = (CDataManager<NPC_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const NPC_RELATION_DATA* tolua_ret = (const NPC_RELATION_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const NPC_RELATION_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<QUEST_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetInstance00
static int tolua_data_manager_CDataManager_QUEST_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<QUEST_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<QUEST_DATA>& tolua_ret = (CDataManager<QUEST_DATA>&)  CDataManager<QUEST_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<QUEST_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<QUEST_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_QUEST_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<QUEST_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<QUEST_DATA>* self = (CDataManager<QUEST_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<QUEST_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetData00
static int tolua_data_manager_CDataManager_QUEST_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<QUEST_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<QUEST_DATA>* self = (CDataManager<QUEST_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const QUEST_DATA* tolua_ret = (const QUEST_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const QUEST_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<QUEST_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetID00
static int tolua_data_manager_CDataManager_QUEST_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<QUEST_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<QUEST_DATA>* self = (CDataManager<QUEST_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<QUEST_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_QUEST_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<QUEST_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<QUEST_DATA>* self = (CDataManager<QUEST_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const QUEST_DATA* tolua_ret = (const QUEST_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const QUEST_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<QUEST_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetNextData00
static int tolua_data_manager_CDataManager_QUEST_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<QUEST_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<QUEST_DATA>* self = (CDataManager<QUEST_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const QUEST_DATA* tolua_ret = (const QUEST_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const QUEST_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<ROLE_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetInstance00
static int tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<ROLE_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<ROLE_RELATION_DATA>& tolua_ret = (CDataManager<ROLE_RELATION_DATA>&)  CDataManager<ROLE_RELATION_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<ROLE_RELATION_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<ROLE_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_RELATION_DATA>* self = (CDataManager<ROLE_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<ROLE_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetData00
static int tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_RELATION_DATA>* self = (CDataManager<ROLE_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const ROLE_RELATION_DATA* tolua_ret = (const ROLE_RELATION_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ROLE_RELATION_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<ROLE_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetID00
static int tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_RELATION_DATA>* self = (CDataManager<ROLE_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<ROLE_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_RELATION_DATA>* self = (CDataManager<ROLE_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const ROLE_RELATION_DATA* tolua_ret = (const ROLE_RELATION_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ROLE_RELATION_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<ROLE_RELATION_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetNextData00
static int tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ROLE_RELATION_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ROLE_RELATION_DATA>* self = (CDataManager<ROLE_RELATION_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const ROLE_RELATION_DATA* tolua_ret = (const ROLE_RELATION_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ROLE_RELATION_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<SCENE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetInstance00
static int tolua_data_manager_CDataManager_SCENE_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<SCENE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<SCENE_DATA>& tolua_ret = (CDataManager<SCENE_DATA>&)  CDataManager<SCENE_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<SCENE_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<SCENE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_SCENE_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SCENE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SCENE_DATA>* self = (CDataManager<SCENE_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<SCENE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetData00
static int tolua_data_manager_CDataManager_SCENE_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SCENE_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SCENE_DATA>* self = (CDataManager<SCENE_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const SCENE_DATA* tolua_ret = (const SCENE_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SCENE_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<SCENE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetID00
static int tolua_data_manager_CDataManager_SCENE_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SCENE_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SCENE_DATA>* self = (CDataManager<SCENE_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<SCENE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_SCENE_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SCENE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SCENE_DATA>* self = (CDataManager<SCENE_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const SCENE_DATA* tolua_ret = (const SCENE_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SCENE_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<SCENE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetNextData00
static int tolua_data_manager_CDataManager_SCENE_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SCENE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SCENE_DATA>* self = (CDataManager<SCENE_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const SCENE_DATA* tolua_ret = (const SCENE_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SCENE_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<SHOP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetInstance00
static int tolua_data_manager_CDataManager_SHOP_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<SHOP_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<SHOP_DATA>& tolua_ret = (CDataManager<SHOP_DATA>&)  CDataManager<SHOP_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<SHOP_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<SHOP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_SHOP_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SHOP_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SHOP_DATA>* self = (CDataManager<SHOP_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<SHOP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetData00
static int tolua_data_manager_CDataManager_SHOP_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SHOP_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SHOP_DATA>* self = (CDataManager<SHOP_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const SHOP_DATA* tolua_ret = (const SHOP_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SHOP_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<SHOP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetID00
static int tolua_data_manager_CDataManager_SHOP_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SHOP_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SHOP_DATA>* self = (CDataManager<SHOP_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<SHOP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_SHOP_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SHOP_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SHOP_DATA>* self = (CDataManager<SHOP_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const SHOP_DATA* tolua_ret = (const SHOP_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SHOP_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<SHOP_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetNextData00
static int tolua_data_manager_CDataManager_SHOP_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SHOP_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SHOP_DATA>* self = (CDataManager<SHOP_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const SHOP_DATA* tolua_ret = (const SHOP_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SHOP_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<SPELL_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetInstance00
static int tolua_data_manager_CDataManager_SPELL_ENTRY__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<SPELL_ENTRY>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<SPELL_ENTRY>& tolua_ret = (CDataManager<SPELL_ENTRY>&)  CDataManager<SPELL_ENTRY>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<SPELL_ENTRY>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<SPELL_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetDataCount00
static int tolua_data_manager_CDataManager_SPELL_ENTRY__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_ENTRY>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_ENTRY>* self = (CDataManager<SPELL_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<SPELL_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetData00
static int tolua_data_manager_CDataManager_SPELL_ENTRY__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_ENTRY>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_ENTRY>* self = (CDataManager<SPELL_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const SPELL_ENTRY* tolua_ret = (const SPELL_ENTRY*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SPELL_ENTRY");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<SPELL_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetID00
static int tolua_data_manager_CDataManager_SPELL_ENTRY__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_ENTRY>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_ENTRY>* self = (CDataManager<SPELL_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<SPELL_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetFirstData00
static int tolua_data_manager_CDataManager_SPELL_ENTRY__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_ENTRY>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_ENTRY>* self = (CDataManager<SPELL_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const SPELL_ENTRY* tolua_ret = (const SPELL_ENTRY*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SPELL_ENTRY");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<SPELL_ENTRY> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetNextData00
static int tolua_data_manager_CDataManager_SPELL_ENTRY__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_ENTRY>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_ENTRY>* self = (CDataManager<SPELL_ENTRY>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const SPELL_ENTRY* tolua_ret = (const SPELL_ENTRY*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SPELL_ENTRY");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<SPELL_VAR> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetInstance00
static int tolua_data_manager_CDataManager_SPELL_VAR__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<SPELL_VAR>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<SPELL_VAR>& tolua_ret = (CDataManager<SPELL_VAR>&)  CDataManager<SPELL_VAR>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<SPELL_VAR>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<SPELL_VAR> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetDataCount00
static int tolua_data_manager_CDataManager_SPELL_VAR__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_VAR>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_VAR>* self = (CDataManager<SPELL_VAR>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<SPELL_VAR> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetData00
static int tolua_data_manager_CDataManager_SPELL_VAR__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_VAR>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_VAR>* self = (CDataManager<SPELL_VAR>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const SPELL_VAR* tolua_ret = (const SPELL_VAR*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SPELL_VAR");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<SPELL_VAR> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetID00
static int tolua_data_manager_CDataManager_SPELL_VAR__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_VAR>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_VAR>* self = (CDataManager<SPELL_VAR>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<SPELL_VAR> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetFirstData00
static int tolua_data_manager_CDataManager_SPELL_VAR__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_VAR>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_VAR>* self = (CDataManager<SPELL_VAR>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const SPELL_VAR* tolua_ret = (const SPELL_VAR*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SPELL_VAR");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<SPELL_VAR> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetNextData00
static int tolua_data_manager_CDataManager_SPELL_VAR__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SPELL_VAR>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SPELL_VAR>* self = (CDataManager<SPELL_VAR>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const SPELL_VAR* tolua_ret = (const SPELL_VAR*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SPELL_VAR");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<ATTR_LEVEL> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetInstance00
static int tolua_data_manager_CDataManager_ATTR_LEVEL__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<ATTR_LEVEL>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<ATTR_LEVEL>& tolua_ret = (CDataManager<ATTR_LEVEL>&)  CDataManager<ATTR_LEVEL>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<ATTR_LEVEL>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<ATTR_LEVEL> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetDataCount00
static int tolua_data_manager_CDataManager_ATTR_LEVEL__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ATTR_LEVEL>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ATTR_LEVEL>* self = (CDataManager<ATTR_LEVEL>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<ATTR_LEVEL> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetData00
static int tolua_data_manager_CDataManager_ATTR_LEVEL__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ATTR_LEVEL>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ATTR_LEVEL>* self = (CDataManager<ATTR_LEVEL>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const ATTR_LEVEL* tolua_ret = (const ATTR_LEVEL*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ATTR_LEVEL");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<ATTR_LEVEL> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetID00
static int tolua_data_manager_CDataManager_ATTR_LEVEL__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ATTR_LEVEL>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ATTR_LEVEL>* self = (CDataManager<ATTR_LEVEL>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<ATTR_LEVEL> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetFirstData00
static int tolua_data_manager_CDataManager_ATTR_LEVEL__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ATTR_LEVEL>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ATTR_LEVEL>* self = (CDataManager<ATTR_LEVEL>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const ATTR_LEVEL* tolua_ret = (const ATTR_LEVEL*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ATTR_LEVEL");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<ATTR_LEVEL> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetNextData00
static int tolua_data_manager_CDataManager_ATTR_LEVEL__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ATTR_LEVEL>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ATTR_LEVEL>* self = (CDataManager<ATTR_LEVEL>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const ATTR_LEVEL* tolua_ret = (const ATTR_LEVEL*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ATTR_LEVEL");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<ACHIEVEMENT_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetInstance00
static int tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<ACHIEVEMENT_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<ACHIEVEMENT_DATA>& tolua_ret = (CDataManager<ACHIEVEMENT_DATA>&)  CDataManager<ACHIEVEMENT_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<ACHIEVEMENT_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<ACHIEVEMENT_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ACHIEVEMENT_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ACHIEVEMENT_DATA>* self = (CDataManager<ACHIEVEMENT_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<ACHIEVEMENT_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetData00
static int tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ACHIEVEMENT_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ACHIEVEMENT_DATA>* self = (CDataManager<ACHIEVEMENT_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const ACHIEVEMENT_DATA* tolua_ret = (const ACHIEVEMENT_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ACHIEVEMENT_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<ACHIEVEMENT_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetID00
static int tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ACHIEVEMENT_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ACHIEVEMENT_DATA>* self = (CDataManager<ACHIEVEMENT_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<ACHIEVEMENT_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ACHIEVEMENT_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ACHIEVEMENT_DATA>* self = (CDataManager<ACHIEVEMENT_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const ACHIEVEMENT_DATA* tolua_ret = (const ACHIEVEMENT_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ACHIEVEMENT_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<ACHIEVEMENT_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetNextData00
static int tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<ACHIEVEMENT_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<ACHIEVEMENT_DATA>* self = (CDataManager<ACHIEVEMENT_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const ACHIEVEMENT_DATA* tolua_ret = (const ACHIEVEMENT_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const ACHIEVEMENT_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<TITLE_TEMPLATE> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetInstance00
static int tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<TITLE_TEMPLATE>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<TITLE_TEMPLATE>& tolua_ret = (CDataManager<TITLE_TEMPLATE>&)  CDataManager<TITLE_TEMPLATE>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<TITLE_TEMPLATE>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<TITLE_TEMPLATE> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetDataCount00
static int tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<TITLE_TEMPLATE>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<TITLE_TEMPLATE>* self = (CDataManager<TITLE_TEMPLATE>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<TITLE_TEMPLATE> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetData00
static int tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<TITLE_TEMPLATE>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<TITLE_TEMPLATE>* self = (CDataManager<TITLE_TEMPLATE>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const TITLE_TEMPLATE* tolua_ret = (const TITLE_TEMPLATE*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const TITLE_TEMPLATE");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<TITLE_TEMPLATE> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetID00
static int tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<TITLE_TEMPLATE>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<TITLE_TEMPLATE>* self = (CDataManager<TITLE_TEMPLATE>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<TITLE_TEMPLATE> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetFirstData00
static int tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<TITLE_TEMPLATE>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<TITLE_TEMPLATE>* self = (CDataManager<TITLE_TEMPLATE>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const TITLE_TEMPLATE* tolua_ret = (const TITLE_TEMPLATE*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const TITLE_TEMPLATE");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<TITLE_TEMPLATE> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetNextData00
static int tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<TITLE_TEMPLATE>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<TITLE_TEMPLATE>* self = (CDataManager<TITLE_TEMPLATE>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const TITLE_TEMPLATE* tolua_ret = (const TITLE_TEMPLATE*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const TITLE_TEMPLATE");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<GUILD_BUILDING_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetInstance00
static int tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<GUILD_BUILDING_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<GUILD_BUILDING_DATA>& tolua_ret = (CDataManager<GUILD_BUILDING_DATA>&)  CDataManager<GUILD_BUILDING_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<GUILD_BUILDING_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<GUILD_BUILDING_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_BUILDING_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_BUILDING_DATA>* self = (CDataManager<GUILD_BUILDING_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<GUILD_BUILDING_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetData00
static int tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_BUILDING_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_BUILDING_DATA>* self = (CDataManager<GUILD_BUILDING_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const GUILD_BUILDING_DATA* tolua_ret = (const GUILD_BUILDING_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const GUILD_BUILDING_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<GUILD_BUILDING_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetID00
static int tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_BUILDING_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_BUILDING_DATA>* self = (CDataManager<GUILD_BUILDING_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<GUILD_BUILDING_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_BUILDING_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_BUILDING_DATA>* self = (CDataManager<GUILD_BUILDING_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const GUILD_BUILDING_DATA* tolua_ret = (const GUILD_BUILDING_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const GUILD_BUILDING_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<GUILD_BUILDING_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetNextData00
static int tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_BUILDING_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_BUILDING_DATA>* self = (CDataManager<GUILD_BUILDING_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const GUILD_BUILDING_DATA* tolua_ret = (const GUILD_BUILDING_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const GUILD_BUILDING_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<GUILD_ACTIVITY_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetInstance00
static int tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<GUILD_ACTIVITY_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<GUILD_ACTIVITY_DATA>& tolua_ret = (CDataManager<GUILD_ACTIVITY_DATA>&)  CDataManager<GUILD_ACTIVITY_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<GUILD_ACTIVITY_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<GUILD_ACTIVITY_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_ACTIVITY_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_ACTIVITY_DATA>* self = (CDataManager<GUILD_ACTIVITY_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<GUILD_ACTIVITY_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetData00
static int tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_ACTIVITY_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_ACTIVITY_DATA>* self = (CDataManager<GUILD_ACTIVITY_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const GUILD_ACTIVITY_DATA* tolua_ret = (const GUILD_ACTIVITY_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const GUILD_ACTIVITY_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<GUILD_ACTIVITY_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetID00
static int tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_ACTIVITY_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_ACTIVITY_DATA>* self = (CDataManager<GUILD_ACTIVITY_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<GUILD_ACTIVITY_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_ACTIVITY_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_ACTIVITY_DATA>* self = (CDataManager<GUILD_ACTIVITY_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const GUILD_ACTIVITY_DATA* tolua_ret = (const GUILD_ACTIVITY_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const GUILD_ACTIVITY_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<GUILD_ACTIVITY_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetNextData00
static int tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<GUILD_ACTIVITY_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<GUILD_ACTIVITY_DATA>* self = (CDataManager<GUILD_ACTIVITY_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const GUILD_ACTIVITY_DATA* tolua_ret = (const GUILD_ACTIVITY_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const GUILD_ACTIVITY_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<SOUL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetInstance00
static int tolua_data_manager_CDataManager_SOUL_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<SOUL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<SOUL_DATA>& tolua_ret = (CDataManager<SOUL_DATA>&)  CDataManager<SOUL_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<SOUL_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<SOUL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_SOUL_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SOUL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SOUL_DATA>* self = (CDataManager<SOUL_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<SOUL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetData00
static int tolua_data_manager_CDataManager_SOUL_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SOUL_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SOUL_DATA>* self = (CDataManager<SOUL_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const SOUL_DATA* tolua_ret = (const SOUL_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SOUL_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<SOUL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetID00
static int tolua_data_manager_CDataManager_SOUL_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SOUL_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SOUL_DATA>* self = (CDataManager<SOUL_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<SOUL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_SOUL_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SOUL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SOUL_DATA>* self = (CDataManager<SOUL_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const SOUL_DATA* tolua_ret = (const SOUL_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SOUL_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<SOUL_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetNextData00
static int tolua_data_manager_CDataManager_SOUL_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<SOUL_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<SOUL_DATA>* self = (CDataManager<SOUL_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const SOUL_DATA* tolua_ret = (const SOUL_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const SOUL_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInstance of class  CDataManager<MAIL_TEMPLATE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetInstance00
static int tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetInstance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"CDataManager<MAIL_TEMPLATE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  CDataManager<MAIL_TEMPLATE_DATA>& tolua_ret = (CDataManager<MAIL_TEMPLATE_DATA>&)  CDataManager<MAIL_TEMPLATE_DATA>::GetInstance();
  tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CDataManager<MAIL_TEMPLATE_DATA>");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInstance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDataCount of class  CDataManager<MAIL_TEMPLATE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetDataCount00
static int tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetDataCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MAIL_TEMPLATE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MAIL_TEMPLATE_DATA>* self = (CDataManager<MAIL_TEMPLATE_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDataCount'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetDataCount();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDataCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetData of class  CDataManager<MAIL_TEMPLATE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetData00
static int tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MAIL_TEMPLATE_DATA>",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MAIL_TEMPLATE_DATA>* self = (CDataManager<MAIL_TEMPLATE_DATA>*)  tolua_tousertype(tolua_S,1,0);
  signed int nDataID = (( signed int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetData'", NULL);
#endif
 {
  const MAIL_TEMPLATE_DATA* tolua_ret = (const MAIL_TEMPLATE_DATA*)  self->GetData(nDataID);
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const MAIL_TEMPLATE_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetID of class  CDataManager<MAIL_TEMPLATE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetID00
static int tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MAIL_TEMPLATE_DATA>",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MAIL_TEMPLATE_DATA>* self = (CDataManager<MAIL_TEMPLATE_DATA>*)  tolua_tousertype(tolua_S,1,0);
  const char* pcszName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetID'", NULL);
#endif
 {
  signed int tolua_ret = ( signed int)  self->GetID(pcszName);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFirstData of class  CDataManager<MAIL_TEMPLATE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetFirstData00
static int tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetFirstData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MAIL_TEMPLATE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MAIL_TEMPLATE_DATA>* self = (CDataManager<MAIL_TEMPLATE_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFirstData'", NULL);
#endif
 {
  const MAIL_TEMPLATE_DATA* tolua_ret = (const MAIL_TEMPLATE_DATA*)  self->GetFirstData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const MAIL_TEMPLATE_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFirstData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextData of class  CDataManager<MAIL_TEMPLATE_DATA> */
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetNextData00
static int tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetNextData00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CDataManager<MAIL_TEMPLATE_DATA>",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CDataManager<MAIL_TEMPLATE_DATA>* self = (CDataManager<MAIL_TEMPLATE_DATA>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextData'", NULL);
#endif
 {
  const MAIL_TEMPLATE_DATA* tolua_ret = (const MAIL_TEMPLATE_DATA*)  self->GetNextData();
  tolua_pushusertype(tolua_S,(void*)tolua_ret,"const MAIL_TEMPLATE_DATA");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextData'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_data_manager_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_constant(tolua_S,"MAX_AI_ID",MAX_AI_ID);
 tolua_constant(tolua_S,"MAX_AI_INIT_AURA_COUNT",MAX_AI_INIT_AURA_COUNT);
 tolua_constant(tolua_S,"MAX_AI_DATA_PARAM_COUNT",MAX_AI_DATA_PARAM_COUNT);
  tolua_constant(tolua_S,"nastInvalid",nastInvalid);
  tolua_constant(tolua_S,"nastNormalSpell1",nastNormalSpell1);
  tolua_constant(tolua_S,"nastNormalSpell2",nastNormalSpell2);
  tolua_constant(tolua_S,"nastHelpFriendSpell1",nastHelpFriendSpell1);
  tolua_constant(tolua_S,"nastHelpFriendSpell2",nastHelpFriendSpell2);
  tolua_constant(tolua_S,"nastCorpseSpell",nastCorpseSpell);
  tolua_constant(tolua_S,"nastSignalIssuerSpell",nastSignalIssuerSpell);
  tolua_constant(tolua_S,"nastSignalTargetSpell",nastSignalTargetSpell);
  tolua_constant(tolua_S,"nastNormalSpell3",nastNormalSpell3);
  tolua_constant(tolua_S,"nastNormalSpell4",nastNormalSpell4);
  tolua_constant(tolua_S,"nastEnterFightSpell",nastEnterFightSpell);
  tolua_constant(tolua_S,"nastChaseSpell",nastChaseSpell);
  tolua_constant(tolua_S,"nastInitSpell1",nastInitSpell1);
  tolua_constant(tolua_S,"nastInitSpell2",nastInitSpell2);
  tolua_constant(tolua_S,"nastInitSpell3",nastInitSpell3);
  tolua_constant(tolua_S,"nastDeadSpell",nastDeadSpell);
  tolua_constant(tolua_S,"nastIdleSpell",nastIdleSpell);
  tolua_constant(tolua_S,"nastTotal",nastTotal);
  tolua_constant(tolua_S,"nastGlobalCDTimer",nastGlobalCDTimer);
  tolua_constant(tolua_S,"nastLiveTimer",nastLiveTimer);
  tolua_constant(tolua_S,"nastFleeTimer",nastFleeTimer);
  tolua_constant(tolua_S,"nastThreatSingalTimer",nastThreatSingalTimer);
  tolua_constant(tolua_S,"nastChanceTargetTimer",nastChanceTargetTimer);
  tolua_constant(tolua_S,"nastScanNearbyTimer",nastScanNearbyTimer);
  tolua_constant(tolua_S,"nastStepAroundTimer",nastStepAroundTimer);
  tolua_constant(tolua_S,"nastWithdrawTimer",nastWithdrawTimer);
  tolua_constant(tolua_S,"nastCustom",nastCustom);
 tolua_cclass(tolua_S,"NEW_AI_DATA","NEW_AI_DATA","",NULL);
 tolua_beginmodule(tolua_S,"NEW_AI_DATA");
  tolua_variable(tolua_S,"nAIID",tolua_get_NEW_AI_DATA_nAIID,tolua_set_NEW_AI_DATA_nAIID);
  tolua_variable(tolua_S,"szScriptName",tolua_get_NEW_AI_DATA_szScriptName,tolua_set_NEW_AI_DATA_szScriptName);
  tolua_variable(tolua_S,"nWalkSpeed",tolua_get_NEW_AI_DATA_nWalkSpeed,tolua_set_NEW_AI_DATA_nWalkSpeed);
  tolua_variable(tolua_S,"nRunSpeed",tolua_get_NEW_AI_DATA_nRunSpeed,tolua_set_NEW_AI_DATA_nRunSpeed);
  tolua_variable(tolua_S,"nRotateSpeed",tolua_get_NEW_AI_DATA_nRotateSpeed,tolua_set_NEW_AI_DATA_nRotateSpeed);
  tolua_variable(tolua_S,"bPatrol",tolua_get_NEW_AI_DATA_bPatrol,tolua_set_NEW_AI_DATA_bPatrol);
  tolua_variable(tolua_S,"bFollow",tolua_get_NEW_AI_DATA_bFollow,tolua_set_NEW_AI_DATA_bFollow);
  tolua_variable(tolua_S,"bCanRun",tolua_get_NEW_AI_DATA_bCanRun,tolua_set_NEW_AI_DATA_bCanRun);
  tolua_variable(tolua_S,"bAlwaysRun",tolua_get_NEW_AI_DATA_bAlwaysRun,tolua_set_NEW_AI_DATA_bAlwaysRun);
  tolua_variable(tolua_S,"nScanEnemyRange",tolua_get_NEW_AI_DATA_nScanEnemyRange,tolua_set_NEW_AI_DATA_nScanEnemyRange);
  tolua_variable(tolua_S,"nScanFriendRange",tolua_get_NEW_AI_DATA_nScanFriendRange,tolua_set_NEW_AI_DATA_nScanFriendRange);
  tolua_variable(tolua_S,"nChaseRange",tolua_get_NEW_AI_DATA_nChaseRange,tolua_set_NEW_AI_DATA_nChaseRange);
  tolua_variable(tolua_S,"nPreferDistance",tolua_get_NEW_AI_DATA_nPreferDistance,tolua_set_NEW_AI_DATA_nPreferDistance);
  tolua_variable(tolua_S,"nWithdrawThreshold",tolua_get_NEW_AI_DATA_nWithdrawThreshold,tolua_set_NEW_AI_DATA_nWithdrawThreshold);
  tolua_variable(tolua_S,"nChaseThreshold",tolua_get_NEW_AI_DATA_nChaseThreshold,tolua_set_NEW_AI_DATA_nChaseThreshold);
  tolua_variable(tolua_S,"nChaseDelayTime",tolua_get_NEW_AI_DATA_nChaseDelayTime,tolua_set_NEW_AI_DATA_nChaseDelayTime);
  tolua_array(tolua_S,"nInitAuraID",tolua_get_data_manager_NEW_AI_DATA_nInitAuraID,tolua_set_data_manager_NEW_AI_DATA_nInitAuraID);
  tolua_variable(tolua_S,"nLiveTime",tolua_get_NEW_AI_DATA_nLiveTime,tolua_set_NEW_AI_DATA_nLiveTime);
  tolua_variable(tolua_S,"bDisappearOnTimeout",tolua_get_NEW_AI_DATA_bDisappearOnTimeout,tolua_set_NEW_AI_DATA_bDisappearOnTimeout);
  tolua_variable(tolua_S,"nWanderRange",tolua_get_NEW_AI_DATA_nWanderRange,tolua_set_NEW_AI_DATA_nWanderRange);
  tolua_variable(tolua_S,"nWanderInterval",tolua_get_NEW_AI_DATA_nWanderInterval,tolua_set_NEW_AI_DATA_nWanderInterval);
  tolua_array(tolua_S,"nSpellID",tolua_get_data_manager_NEW_AI_DATA_nSpellID,tolua_set_data_manager_NEW_AI_DATA_nSpellID);
  tolua_array(tolua_S,"nSpellChance",tolua_get_data_manager_NEW_AI_DATA_nSpellChance,tolua_set_data_manager_NEW_AI_DATA_nSpellChance);
  tolua_array(tolua_S,"nSpellCDTime",tolua_get_data_manager_NEW_AI_DATA_nSpellCDTime,tolua_set_data_manager_NEW_AI_DATA_nSpellCDTime);
  tolua_array(tolua_S,"nSelfHpPercent",tolua_get_data_manager_NEW_AI_DATA_nSelfHpPercent,tolua_set_data_manager_NEW_AI_DATA_nSelfHpPercent);
  tolua_variable(tolua_S,"nGlobalCDTime",tolua_get_NEW_AI_DATA_nGlobalCDTime,tolua_set_NEW_AI_DATA_nGlobalCDTime);
  tolua_variable(tolua_S,"nFriendlyNpcTemplateID",tolua_get_NEW_AI_DATA_nFriendlyNpcTemplateID,tolua_set_NEW_AI_DATA_nFriendlyNpcTemplateID);
  tolua_variable(tolua_S,"bFriendlyCorpse",tolua_get_NEW_AI_DATA_bFriendlyCorpse,tolua_set_NEW_AI_DATA_bFriendlyCorpse);
  tolua_variable(tolua_S,"nCorpseTemplateID",tolua_get_NEW_AI_DATA_nCorpseTemplateID,tolua_set_NEW_AI_DATA_nCorpseTemplateID);
  tolua_variable(tolua_S,"nIssuerSpellSignalType",tolua_get_NEW_AI_DATA_nIssuerSpellSignalType,tolua_set_NEW_AI_DATA_nIssuerSpellSignalType);
  tolua_variable(tolua_S,"nIssuerSpellSignalRange",tolua_get_NEW_AI_DATA_nIssuerSpellSignalRange,tolua_set_NEW_AI_DATA_nIssuerSpellSignalRange);
  tolua_variable(tolua_S,"nTargetSpellSignalType",tolua_get_NEW_AI_DATA_nTargetSpellSignalType,tolua_set_NEW_AI_DATA_nTargetSpellSignalType);
  tolua_variable(tolua_S,"nTargetSpellSignalRange",tolua_get_NEW_AI_DATA_nTargetSpellSignalRange,tolua_set_NEW_AI_DATA_nTargetSpellSignalRange);
  tolua_variable(tolua_S,"nFleeHpPercent",tolua_get_NEW_AI_DATA_nFleeHpPercent,tolua_set_NEW_AI_DATA_nFleeHpPercent);
  tolua_variable(tolua_S,"nFleeChance",tolua_get_NEW_AI_DATA_nFleeChance,tolua_set_NEW_AI_DATA_nFleeChance);
  tolua_variable(tolua_S,"nFleeDuration",tolua_get_NEW_AI_DATA_nFleeDuration,tolua_set_NEW_AI_DATA_nFleeDuration);
  tolua_variable(tolua_S,"nStepAroundChance",tolua_get_NEW_AI_DATA_nStepAroundChance,tolua_set_NEW_AI_DATA_nStepAroundChance);
  tolua_variable(tolua_S,"nStepAroundCDTime",tolua_get_NEW_AI_DATA_nStepAroundCDTime,tolua_set_NEW_AI_DATA_nStepAroundCDTime);
  tolua_variable(tolua_S,"nChangeTargetChance",tolua_get_NEW_AI_DATA_nChangeTargetChance,tolua_set_NEW_AI_DATA_nChangeTargetChance);
  tolua_variable(tolua_S,"nChangeTargetCDTime",tolua_get_NEW_AI_DATA_nChangeTargetCDTime,tolua_set_NEW_AI_DATA_nChangeTargetCDTime);
  tolua_variable(tolua_S,"nSpreadThreatCount",tolua_get_NEW_AI_DATA_nSpreadThreatCount,tolua_set_NEW_AI_DATA_nSpreadThreatCount);
  tolua_array(tolua_S,"nParam",tolua_get_data_manager_NEW_AI_DATA_nParam,tolua_set_data_manager_NEW_AI_DATA_nParam);
#ifndef TOLUA_DISABLE_tolua_data_manager_NEW_AI_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_NEW_AI_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"AURA_ENTRY","AURA_ENTRY","",NULL);
 tolua_beginmodule(tolua_S,"AURA_ENTRY");
  tolua_variable(tolua_S,"m_nID",tolua_get_AURA_ENTRY_m_nID,tolua_set_AURA_ENTRY_m_nID);
  tolua_array(tolua_S,"m_nAttrTable",tolua_get_data_manager_AURA_ENTRY_m_nAttrTable,tolua_set_data_manager_AURA_ENTRY_m_nAttrTable);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"EX_EVT_POINT","AURA_ENTRY::EX_EVT_POINT","",tolua_collect_AURA_ENTRY__EX_EVT_POINT);
  #else
  tolua_cclass(tolua_S,"EX_EVT_POINT","AURA_ENTRY::EX_EVT_POINT","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"EX_EVT_POINT");
   tolua_variable(tolua_S,"m_nExEvtTime",tolua_get_AURA_ENTRY__EX_EVT_POINT_m_nExEvtTime,tolua_set_AURA_ENTRY__EX_EVT_POINT_m_nExEvtTime);
   tolua_variable(tolua_S,"m_nExOnEvt",tolua_get_AURA_ENTRY__EX_EVT_POINT_m_nExOnEvt,tolua_set_AURA_ENTRY__EX_EVT_POINT_m_nExOnEvt);
  tolua_endmodule(tolua_S);
  tolua_array(tolua_S,"m_ExEvtPoint",tolua_get_data_manager_AURA_ENTRY_m_ExEvtPoint,tolua_set_data_manager_AURA_ENTRY_m_ExEvtPoint);
#ifndef TOLUA_DISABLE_tolua_data_manager_AURA_ENTRY_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_AURA_ENTRY_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"DOODAD_DATA","DOODAD_DATA","",NULL);
 tolua_beginmodule(tolua_S,"DOODAD_DATA");
  tolua_variable(tolua_S,"nTemplateID",tolua_get_DOODAD_DATA_nTemplateID,tolua_set_DOODAD_DATA_nTemplateID);
  tolua_variable(tolua_S,"szName",tolua_get_DOODAD_DATA_szName,tolua_set_DOODAD_DATA_szName);
  tolua_variable(tolua_S,"nLevel",tolua_get_DOODAD_DATA_nLevel,tolua_set_DOODAD_DATA_nLevel);
  tolua_variable(tolua_S,"nType",tolua_get_DOODAD_DATA_nType,tolua_set_DOODAD_DATA_nType);
  tolua_variable(tolua_S,"nSubType",tolua_get_DOODAD_DATA_nSubType,tolua_set_DOODAD_DATA_nSubType);
  tolua_variable(tolua_S,"nBodyType",tolua_get_DOODAD_DATA_nBodyType,tolua_set_DOODAD_DATA_nBodyType);
  tolua_variable(tolua_S,"nOperateDuration",tolua_get_DOODAD_DATA_nOperateDuration,tolua_set_DOODAD_DATA_nOperateDuration);
  tolua_variable(tolua_S,"nMinOpenCount",tolua_get_DOODAD_DATA_nMinOpenCount,tolua_set_DOODAD_DATA_nMinOpenCount);
  tolua_variable(tolua_S,"nMaxOpenCount",tolua_get_DOODAD_DATA_nMaxOpenCount,tolua_set_DOODAD_DATA_nMaxOpenCount);
  tolua_variable(tolua_S,"nRequireItemID",tolua_get_DOODAD_DATA_nRequireItemID,tolua_set_DOODAD_DATA_nRequireItemID);
  tolua_variable(tolua_S,"bCostItem",tolua_get_DOODAD_DATA_bCostItem,tolua_set_DOODAD_DATA_bCostItem);
  tolua_variable(tolua_S,"nRequireVarID",tolua_get_DOODAD_DATA_nRequireVarID,tolua_set_DOODAD_DATA_nRequireVarID);
  tolua_variable(tolua_S,"nRequireVarValue",tolua_get_DOODAD_DATA_nRequireVarValue,tolua_set_DOODAD_DATA_nRequireVarValue);
  tolua_variable(tolua_S,"nSpawnNpcID",tolua_get_DOODAD_DATA_nSpawnNpcID,tolua_set_DOODAD_DATA_nSpawnNpcID);
  tolua_variable(tolua_S,"nSpawnNpcChance",tolua_get_DOODAD_DATA_nSpawnNpcChance,tolua_set_DOODAD_DATA_nSpawnNpcChance);
  tolua_variable(tolua_S,"nAuraID",tolua_get_DOODAD_DATA_nAuraID,tolua_set_DOODAD_DATA_nAuraID);
  tolua_variable(tolua_S,"nLifeFrm",tolua_get_DOODAD_DATA_nLifeFrm,tolua_set_DOODAD_DATA_nLifeFrm);
  tolua_variable(tolua_S,"szScriptName",tolua_get_DOODAD_DATA_szScriptName,tolua_set_DOODAD_DATA_szScriptName);
  tolua_variable(tolua_S,"pAdventureData",tolua_get_DOODAD_DATA_pAdventureData_ptr,NULL);
#ifndef TOLUA_DISABLE_tolua_data_manager_DOODAD_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_DOODAD_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"INIT_REPUTATION_DATA","INIT_REPUTATION_DATA","",NULL);
 tolua_beginmodule(tolua_S,"INIT_REPUTATION_DATA");
  tolua_variable(tolua_S,"nForceID",tolua_get_INIT_REPUTATION_DATA_nForceID,tolua_set_INIT_REPUTATION_DATA_nForceID);
  tolua_variable(tolua_S,"nInitValue",tolua_get_INIT_REPUTATION_DATA_nInitValue,tolua_set_INIT_REPUTATION_DATA_nInitValue);
  tolua_variable(tolua_S,"nMaxValue",tolua_get_INIT_REPUTATION_DATA_nMaxValue,tolua_set_INIT_REPUTATION_DATA_nMaxValue);
  tolua_variable(tolua_S,"bActive",tolua_get_INIT_REPUTATION_DATA_bActive,tolua_set_INIT_REPUTATION_DATA_bActive);
  tolua_variable(tolua_S,"bCanAttackWithFreePkMode",tolua_get_INIT_REPUTATION_DATA_bCanAttackWithFreePkMode,tolua_set_INIT_REPUTATION_DATA_bCanAttackWithFreePkMode);
  tolua_variable(tolua_S,"bSabacMode",tolua_get_INIT_REPUTATION_DATA_bSabacMode,tolua_set_INIT_REPUTATION_DATA_bSabacMode);
#ifndef TOLUA_DISABLE_tolua_data_manager_INIT_REPUTATION_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_INIT_REPUTATION_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"ITEM_DATA","ITEM_DATA","",NULL);
 tolua_beginmodule(tolua_S,"ITEM_DATA");
  tolua_variable(tolua_S,"m_nTemplateID",tolua_get_ITEM_DATA_m_nTemplateID,tolua_set_ITEM_DATA_m_nTemplateID);
  tolua_variable(tolua_S,"m_szName",tolua_get_ITEM_DATA_m_szName,tolua_set_ITEM_DATA_m_szName);
  tolua_variable(tolua_S,"m_byItemLevel",tolua_get_ITEM_DATA_unsigned_m_byItemLevel,tolua_set_ITEM_DATA_unsigned_m_byItemLevel);
  tolua_variable(tolua_S,"m_byType",tolua_get_ITEM_DATA_unsigned_m_byType,tolua_set_ITEM_DATA_unsigned_m_byType);
  tolua_variable(tolua_S,"m_bySubType",tolua_get_ITEM_DATA_unsigned_m_bySubType,tolua_set_ITEM_DATA_unsigned_m_bySubType);
  tolua_variable(tolua_S,"m_byUseInCombat",tolua_get_ITEM_DATA_unsigned_m_byUseInCombat,tolua_set_ITEM_DATA_unsigned_m_byUseInCombat);
  tolua_variable(tolua_S,"m_byUseInArena",tolua_get_ITEM_DATA_unsigned_m_byUseInArena,tolua_set_ITEM_DATA_unsigned_m_byUseInArena);
  tolua_variable(tolua_S,"m_byRequireLevel",tolua_get_ITEM_DATA_unsigned_m_byRequireLevel,tolua_set_ITEM_DATA_unsigned_m_byRequireLevel);
  tolua_variable(tolua_S,"m_byRequireClass",tolua_get_ITEM_DATA_unsigned_m_byRequireClass,tolua_set_ITEM_DATA_unsigned_m_byRequireClass);
  tolua_variable(tolua_S,"m_byRequireGender",tolua_get_ITEM_DATA_unsigned_m_byRequireGender,tolua_set_ITEM_DATA_unsigned_m_byRequireGender);
  tolua_variable(tolua_S,"m_byMaxEquip",tolua_get_ITEM_DATA_unsigned_m_byMaxEquip,tolua_set_ITEM_DATA_unsigned_m_byMaxEquip);
  tolua_variable(tolua_S,"m_nMaxOwn",tolua_get_ITEM_DATA_m_nMaxOwn,tolua_set_ITEM_DATA_m_nMaxOwn);
  tolua_variable(tolua_S,"m_byColor",tolua_get_ITEM_DATA_unsigned_m_byColor,tolua_set_ITEM_DATA_unsigned_m_byColor);
  tolua_variable(tolua_S,"m_byBindType",tolua_get_ITEM_DATA_unsigned_m_byBindType,tolua_set_ITEM_DATA_unsigned_m_byBindType);
  tolua_variable(tolua_S,"m_byDurType",tolua_get_ITEM_DATA_unsigned_m_byDurType,tolua_set_ITEM_DATA_unsigned_m_byDurType);
  tolua_variable(tolua_S,"m_nDur",tolua_get_ITEM_DATA_m_nDur,tolua_set_ITEM_DATA_m_nDur);
  tolua_variable(tolua_S,"m_nWeight",tolua_get_ITEM_DATA_m_nWeight,tolua_set_ITEM_DATA_m_nWeight);
  tolua_variable(tolua_S,"m_byCanDelete",tolua_get_ITEM_DATA_unsigned_m_byCanDelete,tolua_set_ITEM_DATA_unsigned_m_byCanDelete);
  tolua_variable(tolua_S,"m_byCanStorage",tolua_get_ITEM_DATA_unsigned_m_byCanStorage,tolua_set_ITEM_DATA_unsigned_m_byCanStorage);
  tolua_variable(tolua_S,"m_byCanSell",tolua_get_ITEM_DATA_unsigned_m_byCanSell,tolua_set_ITEM_DATA_unsigned_m_byCanSell);
  tolua_variable(tolua_S,"m_nSellPrice",tolua_get_ITEM_DATA_m_nSellPrice,tolua_set_ITEM_DATA_m_nSellPrice);
  tolua_variable(tolua_S,"m_nSpellID",tolua_get_ITEM_DATA_m_nSpellID,tolua_set_ITEM_DATA_m_nSpellID);
  tolua_variable(tolua_S,"m_nSpellLevel",tolua_get_ITEM_DATA_m_nSpellLevel,tolua_set_ITEM_DATA_m_nSpellLevel);
  tolua_variable(tolua_S,"m_nSelfCoolDown",tolua_get_ITEM_DATA_m_nSelfCoolDown,tolua_set_ITEM_DATA_m_nSelfCoolDown);
  tolua_variable(tolua_S,"m_nHandbookType",tolua_get_ITEM_DATA_m_nHandbookType,tolua_set_ITEM_DATA_m_nHandbookType);
  tolua_variable(tolua_S,"m_nHandbookIndex",tolua_get_ITEM_DATA_m_nHandbookIndex,tolua_set_ITEM_DATA_m_nHandbookIndex);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"CD_GROUP","ITEM_DATA::CD_GROUP","",tolua_collect_ITEM_DATA__CD_GROUP);
  #else
  tolua_cclass(tolua_S,"CD_GROUP","ITEM_DATA::CD_GROUP","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"CD_GROUP");
   tolua_variable(tolua_S,"m_nCdGrpID",tolua_get_ITEM_DATA__CD_GROUP_m_nCdGrpID,tolua_set_ITEM_DATA__CD_GROUP_m_nCdGrpID);
   tolua_variable(tolua_S,"m_nCdGrpValue",tolua_get_ITEM_DATA__CD_GROUP_m_nCdGrpValue,tolua_set_ITEM_DATA__CD_GROUP_m_nCdGrpValue);
  tolua_endmodule(tolua_S);
  tolua_variable(tolua_S,"m_nCdGrpCount",tolua_get_ITEM_DATA_m_nCdGrpCount,tolua_set_ITEM_DATA_m_nCdGrpCount);
  tolua_array(tolua_S,"m_CdGrp",tolua_get_data_manager_ITEM_DATA_m_CdGrp,tolua_set_data_manager_ITEM_DATA_m_CdGrp);
  tolua_array(tolua_S,"m_nDropLimitDuration",tolua_get_data_manager_ITEM_DATA_m_nDropLimitDuration,tolua_set_data_manager_ITEM_DATA_m_nDropLimitDuration);
  tolua_array(tolua_S,"m_nDropLimitCount",tolua_get_data_manager_ITEM_DATA_m_nDropLimitCount,tolua_set_data_manager_ITEM_DATA_m_nDropLimitCount);
  tolua_variable(tolua_S,"m_nDropFromServerStart",tolua_get_ITEM_DATA_m_nDropFromServerStart,tolua_set_ITEM_DATA_m_nDropFromServerStart);
  tolua_variable(tolua_S,"m_byLog",tolua_get_ITEM_DATA_unsigned_m_byLog,tolua_set_ITEM_DATA_unsigned_m_byLog);
  tolua_variable(tolua_S,"m_byStat",tolua_get_ITEM_DATA_unsigned_m_byStat,tolua_set_ITEM_DATA_unsigned_m_byStat);
  tolua_variable(tolua_S,"m_byExpensiveItem",tolua_get_ITEM_DATA_unsigned_m_byExpensiveItem,tolua_set_ITEM_DATA_unsigned_m_byExpensiveItem);
  tolua_variable(tolua_S,"m_byDropBroadcast",tolua_get_ITEM_DATA_unsigned_m_byDropBroadcast,tolua_set_ITEM_DATA_unsigned_m_byDropBroadcast);
  tolua_variable(tolua_S,"m_bPkDrop",tolua_get_ITEM_DATA_unsigned_m_bPkDrop,tolua_set_ITEM_DATA_unsigned_m_bPkDrop);
  tolua_variable(tolua_S,"m_bTeamAuction",tolua_get_ITEM_DATA_unsigned_m_bTeamAuction,tolua_set_ITEM_DATA_unsigned_m_bTeamAuction);
  tolua_variable(tolua_S,"m_bAuction",tolua_get_ITEM_DATA_unsigned_m_bAuction,tolua_set_ITEM_DATA_unsigned_m_bAuction);
  tolua_array(tolua_S,"m_nAuctionFee",tolua_get_data_manager_ITEM_DATA_m_nAuctionFee,tolua_set_data_manager_ITEM_DATA_m_nAuctionFee);
  tolua_variable(tolua_S,"m_byAuctionType",tolua_get_ITEM_DATA_unsigned_m_byAuctionType,tolua_set_ITEM_DATA_unsigned_m_byAuctionType);
  tolua_variable(tolua_S,"m_nDefaultAuctionPrice",tolua_get_ITEM_DATA_m_nDefaultAuctionPrice,tolua_set_ITEM_DATA_m_nDefaultAuctionPrice);
  tolua_variable(tolua_S,"m_nPlayerDropTime",tolua_get_ITEM_DATA_m_nPlayerDropTime,tolua_set_ITEM_DATA_m_nPlayerDropTime);
  tolua_variable(tolua_S,"m_nDropGroupID",tolua_get_ITEM_DATA_m_nDropGroupID,tolua_set_ITEM_DATA_m_nDropGroupID);
  tolua_variable(tolua_S,"m_nShopTypeSell",tolua_get_ITEM_DATA_m_nShopTypeSell,tolua_set_ITEM_DATA_m_nShopTypeSell);
  tolua_variable(tolua_S,"m_nCanDecompose",tolua_get_ITEM_DATA_m_nCanDecompose,tolua_set_ITEM_DATA_m_nCanDecompose);
  tolua_variable(tolua_S,"m_nTradeNeedIntimacy",tolua_get_ITEM_DATA_m_nTradeNeedIntimacy,tolua_set_ITEM_DATA_m_nTradeNeedIntimacy);
  tolua_variable(tolua_S,"m_nVarIndex",tolua_get_ITEM_DATA_m_nVarIndex,tolua_set_ITEM_DATA_m_nVarIndex);
  tolua_variable(tolua_S,"m_nClearGambleCount",tolua_get_ITEM_DATA_m_nClearGambleCount,tolua_set_ITEM_DATA_m_nClearGambleCount);
  tolua_variable(tolua_S,"m_nTradeOverWeight",tolua_get_ITEM_DATA_m_nTradeOverWeight,tolua_set_ITEM_DATA_m_nTradeOverWeight);
  tolua_variable(tolua_S,"m_nResource",tolua_get_ITEM_DATA_m_nResource,tolua_set_ITEM_DATA_m_nResource);
  tolua_variable(tolua_S,"m_szScript",tolua_get_ITEM_DATA_m_szScript,tolua_set_ITEM_DATA_m_szScript);
#ifndef TOLUA_DISABLE_tolua_data_manager_ITEM_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_ITEM_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
#ifndef TOLUA_DISABLE_tolua_data_manager_GetItemData00
 tolua_function(tolua_S,"GetItemData",tolua_data_manager_GetItemData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_cclass(tolua_S,"ROLE_LEVEL_DATA","ROLE_LEVEL_DATA","",NULL);
 tolua_beginmodule(tolua_S,"ROLE_LEVEL_DATA");
  tolua_variable(tolua_S,"m_nLevel",tolua_get_ROLE_LEVEL_DATA_m_nLevel,tolua_set_ROLE_LEVEL_DATA_m_nLevel);
  tolua_variable(tolua_S,"m_nMaxExp",tolua_get_ROLE_LEVEL_DATA_m_nMaxExp,tolua_set_ROLE_LEVEL_DATA_m_nMaxExp);
  tolua_variable(tolua_S,"m_nPow",tolua_get_ROLE_LEVEL_DATA_m_nPow,tolua_set_ROLE_LEVEL_DATA_m_nPow);
  tolua_variable(tolua_S,"m_nMoneyType01DailyUseLimit",tolua_get_ROLE_LEVEL_DATA_m_nMoneyType01DailyUseLimit,tolua_set_ROLE_LEVEL_DATA_m_nMoneyType01DailyUseLimit);
  tolua_variable(tolua_S,"m_nMoneyType02DailyEarnLimit",tolua_get_ROLE_LEVEL_DATA_m_nMoneyType02DailyEarnLimit,tolua_set_ROLE_LEVEL_DATA_m_nMoneyType02DailyEarnLimit);
  tolua_variable(tolua_S,"m_nMoneyTypeBFTotalLimit",tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeBFTotalLimit,tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeBFTotalLimit);
  tolua_variable(tolua_S,"m_nMoneyTypeBFWeeklyEarnLimit",tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeBFWeeklyEarnLimit,tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeBFWeeklyEarnLimit);
  tolua_variable(tolua_S,"m_nMoneyTypeArenaTotalLimit",tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeArenaTotalLimit,tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeArenaTotalLimit);
  tolua_variable(tolua_S,"m_nMoneyTypeArenaWeeklyEarnLimit",tolua_get_ROLE_LEVEL_DATA_m_nMoneyTypeArenaWeeklyEarnLimit,tolua_set_ROLE_LEVEL_DATA_m_nMoneyTypeArenaWeeklyEarnLimit);
  tolua_variable(tolua_S,"m_nDailyExpLimit",tolua_get_ROLE_LEVEL_DATA_m_nDailyExpLimit,tolua_set_ROLE_LEVEL_DATA_m_nDailyExpLimit);
  tolua_variable(tolua_S,"m_nMaxExtraExp",tolua_get_ROLE_LEVEL_DATA_m_nMaxExtraExp,tolua_set_ROLE_LEVEL_DATA_m_nMaxExtraExp);
  tolua_variable(tolua_S,"m_nDailyExtraExp",tolua_get_ROLE_LEVEL_DATA_m_nDailyExtraExp,tolua_set_ROLE_LEVEL_DATA_m_nDailyExtraExp);
  tolua_variable(tolua_S,"m_nGoldPoolPer5Min",tolua_get_ROLE_LEVEL_DATA_m_nGoldPoolPer5Min,tolua_set_ROLE_LEVEL_DATA_m_nGoldPoolPer5Min);
  tolua_variable(tolua_S,"m_nExerciseExpPerHour",tolua_get_ROLE_LEVEL_DATA_m_nExerciseExpPerHour,tolua_set_ROLE_LEVEL_DATA_m_nExerciseExpPerHour);
  tolua_variable(tolua_S,"m_nOnlineExtraExpPer5Min",tolua_get_ROLE_LEVEL_DATA_m_nOnlineExtraExpPer5Min,tolua_set_ROLE_LEVEL_DATA_m_nOnlineExtraExpPer5Min);
#ifndef TOLUA_DISABLE_tolua_data_manager_ROLE_LEVEL_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_ROLE_LEVEL_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"HERO_LEVEL_DATA","HERO_LEVEL_DATA","",NULL);
 tolua_beginmodule(tolua_S,"HERO_LEVEL_DATA");
  tolua_variable(tolua_S,"m_nLevel",tolua_get_HERO_LEVEL_DATA_m_nLevel,tolua_set_HERO_LEVEL_DATA_m_nLevel);
  tolua_variable(tolua_S,"m_nMaxExp",tolua_get_HERO_LEVEL_DATA_m_nMaxExp,tolua_set_HERO_LEVEL_DATA_m_nMaxExp);
#ifndef TOLUA_DISABLE_tolua_data_manager_HERO_LEVEL_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_HERO_LEVEL_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
#ifndef TOLUA_DISABLE_tolua_data_manager_GetMaxExp00
 tolua_function(tolua_S,"GetMaxExp",tolua_data_manager_GetMaxExp00);
#endif //#ifndef TOLUA_DISABLE
 tolua_cclass(tolua_S,"MISSILE_DATA","MISSILE_DATA","",NULL);
 tolua_beginmodule(tolua_S,"MISSILE_DATA");
  tolua_variable(tolua_S,"m_nTemplateID",tolua_get_MISSILE_DATA_m_nTemplateID,tolua_set_MISSILE_DATA_m_nTemplateID);
  tolua_array(tolua_S,"m_nAttr",tolua_get_data_manager_MISSILE_DATA_m_nAttr,tolua_set_data_manager_MISSILE_DATA_m_nAttr);
#ifndef TOLUA_DISABLE_tolua_data_manager_MISSILE_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_MISSILE_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"NPC_DATA","NPC_DATA","",NULL);
 tolua_beginmodule(tolua_S,"NPC_DATA");
  tolua_variable(tolua_S,"nTemplateID",tolua_get_NPC_DATA_nTemplateID,tolua_set_NPC_DATA_nTemplateID);
  tolua_variable(tolua_S,"nClientTemplateID",tolua_get_NPC_DATA_nClientTemplateID,tolua_set_NPC_DATA_nClientTemplateID);
  tolua_variable(tolua_S,"szDisplayName",tolua_get_NPC_DATA_szDisplayName,tolua_set_NPC_DATA_szDisplayName);
  tolua_variable(tolua_S,"szName",tolua_get_NPC_DATA_szName,tolua_set_NPC_DATA_szName);
  tolua_variable(tolua_S,"nLevel",tolua_get_NPC_DATA_nLevel,tolua_set_NPC_DATA_nLevel);
  tolua_variable(tolua_S,"nPatternID",tolua_get_NPC_DATA_nPatternID,tolua_set_NPC_DATA_nPatternID);
  tolua_variable(tolua_S,"nQuality",tolua_get_NPC_DATA_nQuality,tolua_set_NPC_DATA_nQuality);
  tolua_variable(tolua_S,"nBroadcast",tolua_get_NPC_DATA_nBroadcast,tolua_set_NPC_DATA_nBroadcast);
  tolua_variable(tolua_S,"nForceID",tolua_get_NPC_DATA_nForceID,tolua_set_NPC_DATA_nForceID);
  tolua_variable(tolua_S,"bReduceReputation",tolua_get_NPC_DATA_bReduceReputation,tolua_set_NPC_DATA_bReduceReputation);
  tolua_variable(tolua_S,"nSpecies",tolua_get_NPC_DATA_nSpecies,tolua_set_NPC_DATA_nSpecies);
  tolua_variable(tolua_S,"nRivalForceID",tolua_get_NPC_DATA_nRivalForceID,tolua_set_NPC_DATA_nRivalForceID);
  tolua_variable(tolua_S,"nGroupID",tolua_get_NPC_DATA_nGroupID,tolua_set_NPC_DATA_nGroupID);
  tolua_variable(tolua_S,"nBodyType",tolua_get_NPC_DATA_nBodyType,tolua_set_NPC_DATA_nBodyType);
  tolua_variable(tolua_S,"nSpellEffectGroup",tolua_get_NPC_DATA_nSpellEffectGroup,tolua_set_NPC_DATA_nSpellEffectGroup);
  tolua_variable(tolua_S,"nSpawnTime",tolua_get_NPC_DATA_nSpawnTime,tolua_set_NPC_DATA_nSpawnTime);
  tolua_variable(tolua_S,"nReviveTime",tolua_get_NPC_DATA_nReviveTime,tolua_set_NPC_DATA_nReviveTime);
  tolua_variable(tolua_S,"nCorpseTime",tolua_get_NPC_DATA_nCorpseTime,tolua_set_NPC_DATA_nCorpseTime);
  tolua_variable(tolua_S,"nMinRespawnTime",tolua_get_NPC_DATA_nMinRespawnTime,tolua_set_NPC_DATA_nMinRespawnTime);
  tolua_variable(tolua_S,"nMaxRespawnTime",tolua_get_NPC_DATA_nMaxRespawnTime,tolua_set_NPC_DATA_nMaxRespawnTime);
  tolua_variable(tolua_S,"nAIID",tolua_get_NPC_DATA_nAIID,tolua_set_NPC_DATA_nAIID);
  tolua_variable(tolua_S,"bImmunePassiveMove",tolua_get_NPC_DATA_bImmunePassiveMove,tolua_set_NPC_DATA_bImmunePassiveMove);
  tolua_variable(tolua_S,"bInvincible",tolua_get_NPC_DATA_bInvincible,tolua_set_NPC_DATA_bInvincible);
  tolua_variable(tolua_S,"bAlwaysRun",tolua_get_NPC_DATA_bAlwaysRun,tolua_set_NPC_DATA_bAlwaysRun);
  tolua_variable(tolua_S,"bCanbeThrown",tolua_get_NPC_DATA_bCanbeThrown,tolua_set_NPC_DATA_bCanbeThrown);
  tolua_variable(tolua_S,"bIgnoreInvisible",tolua_get_NPC_DATA_bIgnoreInvisible,tolua_set_NPC_DATA_bIgnoreInvisible);
  tolua_variable(tolua_S,"bCounterInvisible",tolua_get_NPC_DATA_bCounterInvisible,tolua_set_NPC_DATA_bCounterInvisible);
  tolua_variable(tolua_S,"nAttackRange",tolua_get_NPC_DATA_nAttackRange,tolua_set_NPC_DATA_nAttackRange);
  tolua_variable(tolua_S,"bExclusive",tolua_get_NPC_DATA_bExclusive,tolua_set_NPC_DATA_bExclusive);
  tolua_variable(tolua_S,"bNeverBeHit",tolua_get_NPC_DATA_bNeverBeHit,tolua_set_NPC_DATA_bNeverBeHit);
  tolua_variable(tolua_S,"bProfit",tolua_get_NPC_DATA_bProfit,tolua_set_NPC_DATA_bProfit);
  tolua_variable(tolua_S,"nExpRate",tolua_get_NPC_DATA_nExpRate,tolua_set_NPC_DATA_nExpRate);
  tolua_variable(tolua_S,"nSoulRate",tolua_get_NPC_DATA_nSoulRate,tolua_set_NPC_DATA_nSoulRate);
  tolua_variable(tolua_S,"bDailyExp",tolua_get_NPC_DATA_bDailyExp,tolua_set_NPC_DATA_bDailyExp);
  tolua_variable(tolua_S,"bAuraInvalid",tolua_get_NPC_DATA_bAuraInvalid,tolua_set_NPC_DATA_bAuraInvalid);
  tolua_variable(tolua_S,"nKillPlayerVarIndex",tolua_get_NPC_DATA_nKillPlayerVarIndex,tolua_set_NPC_DATA_nKillPlayerVarIndex);
  tolua_variable(tolua_S,"nBeKilledVarIndex",tolua_get_NPC_DATA_nBeKilledVarIndex,tolua_set_NPC_DATA_nBeKilledVarIndex);
  tolua_variable(tolua_S,"nShopID",tolua_get_NPC_DATA_nShopID,tolua_set_NPC_DATA_nShopID);
  tolua_variable(tolua_S,"nBlackMarketID",tolua_get_NPC_DATA_nBlackMarketID,tolua_set_NPC_DATA_nBlackMarketID);
  tolua_variable(tolua_S,"bInteractBank",tolua_get_NPC_DATA_bInteractBank,tolua_set_NPC_DATA_bInteractBank);
  tolua_variable(tolua_S,"bInteractGuildBank",tolua_get_NPC_DATA_bInteractGuildBank,tolua_set_NPC_DATA_bInteractGuildBank);
  tolua_variable(tolua_S,"bInteractRewardQuest",tolua_get_NPC_DATA_bInteractRewardQuest,tolua_set_NPC_DATA_bInteractRewardQuest);
  tolua_variable(tolua_S,"bInteractCreateGuild",tolua_get_NPC_DATA_bInteractCreateGuild,tolua_set_NPC_DATA_bInteractCreateGuild);
  tolua_variable(tolua_S,"bInteractGuildSalary",tolua_get_NPC_DATA_bInteractGuildSalary,tolua_set_NPC_DATA_bInteractGuildSalary);
  tolua_variable(tolua_S,"bInteractAuction",tolua_get_NPC_DATA_bInteractAuction,tolua_set_NPC_DATA_bInteractAuction);
  tolua_variable(tolua_S,"bInteractRune",tolua_get_NPC_DATA_bInteractRune,tolua_set_NPC_DATA_bInteractRune);
  tolua_variable(tolua_S,"bInteractSoul",tolua_get_NPC_DATA_bInteractSoul,tolua_set_NPC_DATA_bInteractSoul);
  tolua_variable(tolua_S,"bInteractWeaponLevelUpPutIn",tolua_get_NPC_DATA_bInteractWeaponLevelUpPutIn,tolua_set_NPC_DATA_bInteractWeaponLevelUpPutIn);
  tolua_variable(tolua_S,"bInteractWeaponLevelUpTakeOut",tolua_get_NPC_DATA_bInteractWeaponLevelUpTakeOut,tolua_set_NPC_DATA_bInteractWeaponLevelUpTakeOut);
  tolua_variable(tolua_S,"bInteractSpecialRepair",tolua_get_NPC_DATA_bInteractSpecialRepair,tolua_set_NPC_DATA_bInteractSpecialRepair);
  tolua_variable(tolua_S,"bInteractGamble",tolua_get_NPC_DATA_bInteractGamble,tolua_set_NPC_DATA_bInteractGamble);
  tolua_variable(tolua_S,"bInteractExercise",tolua_get_NPC_DATA_bInteractExercise,tolua_set_NPC_DATA_bInteractExercise);
  tolua_variable(tolua_S,"bInteractSpellBookGamble",tolua_get_NPC_DATA_bInteractSpellBookGamble,tolua_set_NPC_DATA_bInteractSpellBookGamble);
  tolua_variable(tolua_S,"bInteractArenaReward",tolua_get_NPC_DATA_bInteractArenaReward,tolua_set_NPC_DATA_bInteractArenaReward);
  tolua_variable(tolua_S,"nCorpseDoodadID",tolua_get_NPC_DATA_nCorpseDoodadID,tolua_set_NPC_DATA_nCorpseDoodadID);
  tolua_variable(tolua_S,"bDieIDMonitor",tolua_get_NPC_DATA_bDieIDMonitor,tolua_set_NPC_DATA_bDieIDMonitor);
  tolua_variable(tolua_S,"bStatMoney",tolua_get_NPC_DATA_bStatMoney,tolua_set_NPC_DATA_bStatMoney);
  tolua_variable(tolua_S,"nBigDropInterval",tolua_get_NPC_DATA_nBigDropInterval,tolua_set_NPC_DATA_nBigDropInterval);
  tolua_variable(tolua_S,"nRuneGroup",tolua_get_NPC_DATA_nRuneGroup,tolua_set_NPC_DATA_nRuneGroup);
  tolua_variable(tolua_S,"nHandbookType",tolua_get_NPC_DATA_nHandbookType,tolua_set_NPC_DATA_nHandbookType);
  tolua_variable(tolua_S,"nHandbookIndex",tolua_get_NPC_DATA_nHandbookIndex,tolua_set_NPC_DATA_nHandbookIndex);
  tolua_variable(tolua_S,"nKillNpcQuestID",tolua_get_NPC_DATA_nKillNpcQuestID,tolua_set_NPC_DATA_nKillNpcQuestID);
  tolua_variable(tolua_S,"pAdventureData",tolua_get_NPC_DATA_pAdventureData_ptr,NULL);
  tolua_variable(tolua_S,"szScriptName",tolua_get_NPC_DATA_szScriptName,tolua_set_NPC_DATA_szScriptName);
#ifndef TOLUA_DISABLE_tolua_data_manager_NPC_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_NPC_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"NPC_RELATION_DATA","NPC_RELATION_DATA","",NULL);
 tolua_beginmodule(tolua_S,"NPC_RELATION_DATA");
  tolua_variable(tolua_S,"nForceID",tolua_get_NPC_RELATION_DATA_nForceID,tolua_set_NPC_RELATION_DATA_nForceID);
  tolua_array(tolua_S,"nRelation",tolua_get_data_manager_NPC_RELATION_DATA_nRelation,tolua_set_data_manager_NPC_RELATION_DATA_nRelation);
#ifndef TOLUA_DISABLE_tolua_data_manager_NPC_RELATION_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_NPC_RELATION_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"QUEST_DATA","QUEST_DATA","",NULL);
 tolua_beginmodule(tolua_S,"QUEST_DATA");
  tolua_variable(tolua_S,"nQuestID",tolua_get_QUEST_DATA_nQuestID,tolua_set_QUEST_DATA_nQuestID);
  tolua_variable(tolua_S,"eResetType",tolua_get_QUEST_DATA_eResetType,tolua_set_QUEST_DATA_eResetType);
  tolua_variable(tolua_S,"nQuestType",tolua_get_QUEST_DATA_nQuestType,tolua_set_QUEST_DATA_nQuestType);
  tolua_variable(tolua_S,"nCircleID",tolua_get_QUEST_DATA_nCircleID,tolua_set_QUEST_DATA_nCircleID);
  tolua_variable(tolua_S,"nQuality",tolua_get_QUEST_DATA_nQuality,tolua_set_QUEST_DATA_nQuality);
  tolua_variable(tolua_S,"dwResetTime",tolua_get_QUEST_DATA_unsigned_dwResetTime,tolua_set_QUEST_DATA_unsigned_dwResetTime);
  tolua_variable(tolua_S,"bCanAbandon",tolua_get_QUEST_DATA_bCanAbandon,tolua_set_QUEST_DATA_bCanAbandon);
  tolua_variable(tolua_S,"bCanShare",tolua_get_QUEST_DATA_bCanShare,tolua_set_QUEST_DATA_bCanShare);
  tolua_variable(tolua_S,"bCanPublish",tolua_get_QUEST_DATA_bCanPublish,tolua_set_QUEST_DATA_bCanPublish);
  tolua_variable(tolua_S,"nEmergencyCoolDown",tolua_get_QUEST_DATA_nEmergencyCoolDown,tolua_set_QUEST_DATA_nEmergencyCoolDown);
  tolua_variable(tolua_S,"nDurationLimit",tolua_get_QUEST_DATA_unsigned_nDurationLimit,tolua_set_QUEST_DATA_unsigned_nDurationLimit);
  tolua_variable(tolua_S,"szName",tolua_get_QUEST_DATA_szName,tolua_set_QUEST_DATA_szName);
  tolua_array(tolua_S,"nSceneID",tolua_get_data_manager_QUEST_DATA_nSceneID,tolua_set_data_manager_QUEST_DATA_nSceneID);
  tolua_array(tolua_S,"nX",tolua_get_data_manager_QUEST_DATA_nX,tolua_set_data_manager_QUEST_DATA_nX);
  tolua_array(tolua_S,"nY",tolua_get_data_manager_QUEST_DATA_nY,tolua_set_data_manager_QUEST_DATA_nY);
  tolua_array(tolua_S,"nZ",tolua_get_data_manager_QUEST_DATA_nZ,tolua_set_data_manager_QUEST_DATA_nZ);
  tolua_variable(tolua_S,"nMaxCompletedCount",tolua_get_QUEST_DATA_nMaxCompletedCount,tolua_set_QUEST_DATA_nMaxCompletedCount);
  tolua_variable(tolua_S,"nClassLimit",tolua_get_QUEST_DATA_nClassLimit,tolua_set_QUEST_DATA_nClassLimit);
  tolua_variable(tolua_S,"nSexLimit",tolua_get_QUEST_DATA_nSexLimit,tolua_set_QUEST_DATA_nSexLimit);
  tolua_variable(tolua_S,"nRelationLimit",tolua_get_QUEST_DATA_nRelationLimit,tolua_set_QUEST_DATA_nRelationLimit);
  tolua_variable(tolua_S,"nPreQuestID1",tolua_get_QUEST_DATA_nPreQuestID1,tolua_set_QUEST_DATA_nPreQuestID1);
  tolua_variable(tolua_S,"nPreQuestID2",tolua_get_QUEST_DATA_nPreQuestID2,tolua_set_QUEST_DATA_nPreQuestID2);
  tolua_variable(tolua_S,"nStartNpcID",tolua_get_QUEST_DATA_nStartNpcID,tolua_set_QUEST_DATA_nStartNpcID);
  tolua_variable(tolua_S,"nStartItemID",tolua_get_QUEST_DATA_nStartItemID,tolua_set_QUEST_DATA_nStartItemID);
  tolua_variable(tolua_S,"bStartZone",tolua_get_QUEST_DATA_bStartZone,tolua_set_QUEST_DATA_bStartZone);
  tolua_variable(tolua_S,"nMinLevelLimit",tolua_get_QUEST_DATA_nMinLevelLimit,tolua_set_QUEST_DATA_nMinLevelLimit);
  tolua_variable(tolua_S,"nMaxLevelLimit",tolua_get_QUEST_DATA_nMaxLevelLimit,tolua_set_QUEST_DATA_nMaxLevelLimit);
  tolua_variable(tolua_S,"nBuffIDLimit",tolua_get_QUEST_DATA_nBuffIDLimit,tolua_set_QUEST_DATA_nBuffIDLimit);
  tolua_variable(tolua_S,"nBuffLevelLimit",tolua_get_QUEST_DATA_nBuffLevelLimit,tolua_set_QUEST_DATA_nBuffLevelLimit);
  tolua_variable(tolua_S,"nMoneyTypeLimit",tolua_get_QUEST_DATA_nMoneyTypeLimit,tolua_set_QUEST_DATA_nMoneyTypeLimit);
  tolua_variable(tolua_S,"nMoneyAmountLimit",tolua_get_QUEST_DATA_nMoneyAmountLimit,tolua_set_QUEST_DATA_nMoneyAmountLimit);
  tolua_variable(tolua_S,"nReputationForceID",tolua_get_QUEST_DATA_nReputationForceID,tolua_set_QUEST_DATA_nReputationForceID);
  tolua_variable(tolua_S,"nReputationMinLimit",tolua_get_QUEST_DATA_nReputationMinLimit,tolua_set_QUEST_DATA_nReputationMinLimit);
  tolua_variable(tolua_S,"nReputationMaxLimit",tolua_get_QUEST_DATA_nReputationMaxLimit,tolua_set_QUEST_DATA_nReputationMaxLimit);
  tolua_variable(tolua_S,"nYearLimit",tolua_get_QUEST_DATA_nYearLimit,tolua_set_QUEST_DATA_nYearLimit);
  tolua_variable(tolua_S,"nMonthLimit",tolua_get_QUEST_DATA_nMonthLimit,tolua_set_QUEST_DATA_nMonthLimit);
  tolua_variable(tolua_S,"nStartWeekday",tolua_get_QUEST_DATA_nStartWeekday,tolua_set_QUEST_DATA_nStartWeekday);
  tolua_variable(tolua_S,"nWeekdayDuration",tolua_get_QUEST_DATA_nWeekdayDuration,tolua_set_QUEST_DATA_nWeekdayDuration);
  tolua_variable(tolua_S,"nStartMonthday",tolua_get_QUEST_DATA_nStartMonthday,tolua_set_QUEST_DATA_nStartMonthday);
  tolua_variable(tolua_S,"nMonthdayDuration",tolua_get_QUEST_DATA_nMonthdayDuration,tolua_set_QUEST_DATA_nMonthdayDuration);
  tolua_variable(tolua_S,"dwAcceptStartTime",tolua_get_QUEST_DATA_unsigned_dwAcceptStartTime,tolua_set_QUEST_DATA_unsigned_dwAcceptStartTime);
  tolua_variable(tolua_S,"dwAcceptEndTime",tolua_get_QUEST_DATA_unsigned_dwAcceptEndTime,tolua_set_QUEST_DATA_unsigned_dwAcceptEndTime);
  tolua_variable(tolua_S,"nAcceptStartHour",tolua_get_QUEST_DATA_nAcceptStartHour,tolua_set_QUEST_DATA_nAcceptStartHour);
  tolua_variable(tolua_S,"nAcceptStartMin",tolua_get_QUEST_DATA_nAcceptStartMin,tolua_set_QUEST_DATA_nAcceptStartMin);
  tolua_variable(tolua_S,"nAcceptEndHour",tolua_get_QUEST_DATA_nAcceptEndHour,tolua_set_QUEST_DATA_nAcceptEndHour);
  tolua_variable(tolua_S,"nAcceptEndMin",tolua_get_QUEST_DATA_nAcceptEndMin,tolua_set_QUEST_DATA_nAcceptEndMin);
  tolua_variable(tolua_S,"nAcceptStartDayTime",tolua_get_QUEST_DATA_nAcceptStartDayTime,tolua_set_QUEST_DATA_nAcceptStartDayTime);
  tolua_variable(tolua_S,"nAcceptEndDayTime",tolua_get_QUEST_DATA_nAcceptEndDayTime,tolua_set_QUEST_DATA_nAcceptEndDayTime);
  tolua_array(tolua_S,"vecItemLimitID",tolua_get_data_manager_QUEST_DATA_vecItemLimitID,tolua_set_data_manager_QUEST_DATA_vecItemLimitID);
  tolua_array(tolua_S,"vecItemLimitCnt",tolua_get_data_manager_QUEST_DATA_vecItemLimitCnt,tolua_set_data_manager_QUEST_DATA_vecItemLimitCnt);
  tolua_variable(tolua_S,"nLimitVarID1",tolua_get_QUEST_DATA_nLimitVarID1,tolua_set_QUEST_DATA_nLimitVarID1);
  tolua_variable(tolua_S,"nLimitVarValue1",tolua_get_QUEST_DATA_nLimitVarValue1,tolua_set_QUEST_DATA_nLimitVarValue1);
  tolua_variable(tolua_S,"nLimitVarID2",tolua_get_QUEST_DATA_nLimitVarID2,tolua_set_QUEST_DATA_nLimitVarID2);
  tolua_variable(tolua_S,"nLimitVarValue2",tolua_get_QUEST_DATA_nLimitVarValue2,tolua_set_QUEST_DATA_nLimitVarValue2);
  tolua_array(tolua_S,"szBeforeAcceptScriptName",tolua_get_data_manager_QUEST_DATA_szBeforeAcceptScriptName,tolua_set_data_manager_QUEST_DATA_szBeforeAcceptScriptName);
  tolua_variable(tolua_S,"nOfferMoneyType",tolua_get_QUEST_DATA_nOfferMoneyType,tolua_set_QUEST_DATA_nOfferMoneyType);
  tolua_variable(tolua_S,"nOfferMoneyAmount",tolua_get_QUEST_DATA_nOfferMoneyAmount,tolua_set_QUEST_DATA_nOfferMoneyAmount);
  tolua_variable(tolua_S,"nOfferBuffID",tolua_get_QUEST_DATA_nOfferBuffID,tolua_set_QUEST_DATA_nOfferBuffID);
  tolua_variable(tolua_S,"nOfferBuffLevel",tolua_get_QUEST_DATA_nOfferBuffLevel,tolua_set_QUEST_DATA_nOfferBuffLevel);
  tolua_variable(tolua_S,"nOfferItemID",tolua_get_QUEST_DATA_nOfferItemID,tolua_set_QUEST_DATA_nOfferItemID);
  tolua_variable(tolua_S,"nOfferItemCnt",tolua_get_QUEST_DATA_nOfferItemCnt,tolua_set_QUEST_DATA_nOfferItemCnt);
  tolua_array(tolua_S,"szAfterAcceptScriptName",tolua_get_data_manager_QUEST_DATA_szAfterAcceptScriptName,tolua_set_data_manager_QUEST_DATA_szAfterAcceptScriptName);
  tolua_variable(tolua_S,"nFinishNpc",tolua_get_QUEST_DATA_nFinishNpc,tolua_set_QUEST_DATA_nFinishNpc);
  tolua_variable(tolua_S,"nFinishItemID",tolua_get_QUEST_DATA_nFinishItemID,tolua_set_QUEST_DATA_nFinishItemID);
  tolua_variable(tolua_S,"nCheckBuffID",tolua_get_QUEST_DATA_nCheckBuffID,tolua_set_QUEST_DATA_nCheckBuffID);
  tolua_variable(tolua_S,"nCheckBuffLevel",tolua_get_QUEST_DATA_nCheckBuffLevel,tolua_set_QUEST_DATA_nCheckBuffLevel);
  tolua_variable(tolua_S,"nCheckLevel",tolua_get_QUEST_DATA_nCheckLevel,tolua_set_QUEST_DATA_nCheckLevel);
  tolua_array(tolua_S,"nCheckKillPlayerLevlStart",tolua_get_data_manager_QUEST_DATA_nCheckKillPlayerLevlStart,tolua_set_data_manager_QUEST_DATA_nCheckKillPlayerLevlStart);
  tolua_array(tolua_S,"nCheckKillPlayerLevlEnd",tolua_get_data_manager_QUEST_DATA_nCheckKillPlayerLevlEnd,tolua_set_data_manager_QUEST_DATA_nCheckKillPlayerLevlEnd);
  tolua_array(tolua_S,"nCheckKillPlayerCount",tolua_get_data_manager_QUEST_DATA_nCheckKillPlayerCount,tolua_set_data_manager_QUEST_DATA_nCheckKillPlayerCount);
  tolua_array(tolua_S,"nCheckKillBufPlayerBufID",tolua_get_data_manager_QUEST_DATA_nCheckKillBufPlayerBufID,tolua_set_data_manager_QUEST_DATA_nCheckKillBufPlayerBufID);
  tolua_array(tolua_S,"nCheckKillBufPlayerCount",tolua_get_data_manager_QUEST_DATA_nCheckKillBufPlayerCount,tolua_set_data_manager_QUEST_DATA_nCheckKillBufPlayerCount);
  tolua_array(tolua_S,"nCheckKillNpcID",tolua_get_data_manager_QUEST_DATA_nCheckKillNpcID,tolua_set_data_manager_QUEST_DATA_nCheckKillNpcID);
  tolua_array(tolua_S,"nCheckKillNpcGroupID",tolua_get_data_manager_QUEST_DATA_nCheckKillNpcGroupID,tolua_set_data_manager_QUEST_DATA_nCheckKillNpcGroupID);
  tolua_array(tolua_S,"nCheckKillNpcCnt",tolua_get_data_manager_QUEST_DATA_nCheckKillNpcCnt,tolua_set_data_manager_QUEST_DATA_nCheckKillNpcCnt);
  tolua_array(tolua_S,"nCheckKillItemNpcID",tolua_get_data_manager_QUEST_DATA_nCheckKillItemNpcID,tolua_set_data_manager_QUEST_DATA_nCheckKillItemNpcID);
  tolua_array(tolua_S,"nCheckKillItemNpcRate",tolua_get_data_manager_QUEST_DATA_nCheckKillItemNpcRate,tolua_set_data_manager_QUEST_DATA_nCheckKillItemNpcRate);
  tolua_array(tolua_S,"nCheckKillItemCnt",tolua_get_data_manager_QUEST_DATA_nCheckKillItemCnt,tolua_set_data_manager_QUEST_DATA_nCheckKillItemCnt);
  tolua_array(tolua_S,"nCheckKillDoodadID",tolua_get_data_manager_QUEST_DATA_nCheckKillDoodadID,tolua_set_data_manager_QUEST_DATA_nCheckKillDoodadID);
  tolua_array(tolua_S,"nCheckKillDoodadCnt",tolua_get_data_manager_QUEST_DATA_nCheckKillDoodadCnt,tolua_set_data_manager_QUEST_DATA_nCheckKillDoodadCnt);
  tolua_array(tolua_S,"nCheckItemID",tolua_get_data_manager_QUEST_DATA_nCheckItemID,tolua_set_data_manager_QUEST_DATA_nCheckItemID);
  tolua_array(tolua_S,"nCheckItemCount",tolua_get_data_manager_QUEST_DATA_nCheckItemCount,tolua_set_data_manager_QUEST_DATA_nCheckItemCount);
  tolua_variable(tolua_S,"nCheckReputationForceID",tolua_get_QUEST_DATA_nCheckReputationForceID,tolua_set_QUEST_DATA_nCheckReputationForceID);
  tolua_variable(tolua_S,"nCheckReputationForceValue",tolua_get_QUEST_DATA_nCheckReputationForceValue,tolua_set_QUEST_DATA_nCheckReputationForceValue);
  tolua_array(tolua_S,"nCheckVarID",tolua_get_data_manager_QUEST_DATA_nCheckVarID,tolua_set_data_manager_QUEST_DATA_nCheckVarID);
  tolua_array(tolua_S,"nCheckVarValue",tolua_get_data_manager_QUEST_DATA_nCheckVarValue,tolua_set_data_manager_QUEST_DATA_nCheckVarValue);
  tolua_array(tolua_S,"szBeforeCompleteScriptName",tolua_get_data_manager_QUEST_DATA_szBeforeCompleteScriptName,tolua_set_data_manager_QUEST_DATA_szBeforeCompleteScriptName);
  tolua_variable(tolua_S,"nTalkNpcID",tolua_get_QUEST_DATA_nTalkNpcID,tolua_set_QUEST_DATA_nTalkNpcID);
  tolua_variable(tolua_S,"nTalkVarID",tolua_get_QUEST_DATA_nTalkVarID,tolua_set_QUEST_DATA_nTalkVarID);
  tolua_variable(tolua_S,"nExploreVarID",tolua_get_QUEST_DATA_nExploreVarID,tolua_set_QUEST_DATA_nExploreVarID);
  tolua_array(tolua_S,"szAfterCompleteScriptName",tolua_get_data_manager_QUEST_DATA_szAfterCompleteScriptName,tolua_set_data_manager_QUEST_DATA_szAfterCompleteScriptName);
  tolua_variable(tolua_S,"stRewardData",tolua_get_QUEST_DATA_stRewardData,tolua_set_QUEST_DATA_stRewardData);
  tolua_array(tolua_S,"szAbandonScriptName",tolua_get_data_manager_QUEST_DATA_szAbandonScriptName,tolua_set_data_manager_QUEST_DATA_szAbandonScriptName);
#ifndef TOLUA_DISABLE_tolua_data_manager_QUEST_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_QUEST_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_QUEST_DATA_CheckValidQuestID00
  tolua_function(tolua_S,"CheckValidQuestID",tolua_data_manager_QUEST_DATA_CheckValidQuestID00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"REWARD_QUEST_MONEY_COST_DATA","REWARD_QUEST_MONEY_COST_DATA","",NULL);
 tolua_beginmodule(tolua_S,"REWARD_QUEST_MONEY_COST_DATA");
  tolua_variable(tolua_S,"nTime",tolua_get_REWARD_QUEST_MONEY_COST_DATA_nTime,tolua_set_REWARD_QUEST_MONEY_COST_DATA_nTime);
  tolua_variable(tolua_S,"nMoneyCost",tolua_get_REWARD_QUEST_MONEY_COST_DATA_nMoneyCost,tolua_set_REWARD_QUEST_MONEY_COST_DATA_nMoneyCost);
#ifndef TOLUA_DISABLE_tolua_data_manager_REWARD_QUEST_MONEY_COST_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_REWARD_QUEST_MONEY_COST_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"QUEST_RECYCLE_DATA","QUEST_RECYCLE_DATA","",NULL);
 tolua_beginmodule(tolua_S,"QUEST_RECYCLE_DATA");
  tolua_variable(tolua_S,"nItemLevel",tolua_get_QUEST_RECYCLE_DATA_nItemLevel,tolua_set_QUEST_RECYCLE_DATA_nItemLevel);
  tolua_variable(tolua_S,"nItemColor",tolua_get_QUEST_RECYCLE_DATA_nItemColor,tolua_set_QUEST_RECYCLE_DATA_nItemColor);
  tolua_variable(tolua_S,"nItemSubType",tolua_get_QUEST_RECYCLE_DATA_nItemSubType,tolua_set_QUEST_RECYCLE_DATA_nItemSubType);
  tolua_variable(tolua_S,"nVarIndex",tolua_get_QUEST_RECYCLE_DATA_nVarIndex,tolua_set_QUEST_RECYCLE_DATA_nVarIndex);
#ifndef TOLUA_DISABLE_tolua_data_manager_QUEST_RECYCLE_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_QUEST_RECYCLE_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"ROLE_RELATION_DATA","ROLE_RELATION_DATA","",NULL);
 tolua_beginmodule(tolua_S,"ROLE_RELATION_DATA");
  tolua_variable(tolua_S,"nFactionID",tolua_get_ROLE_RELATION_DATA_nFactionID,tolua_set_ROLE_RELATION_DATA_nFactionID);
  tolua_array(tolua_S,"nRelation",tolua_get_data_manager_ROLE_RELATION_DATA_nRelation,tolua_set_data_manager_ROLE_RELATION_DATA_nRelation);
#ifndef TOLUA_DISABLE_tolua_data_manager_ROLE_RELATION_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_ROLE_RELATION_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"SCENE_DATA","SCENE_DATA","",NULL);
 tolua_beginmodule(tolua_S,"SCENE_DATA");
  tolua_variable(tolua_S,"nSceneID",tolua_get_SCENE_DATA_nSceneID,tolua_set_SCENE_DATA_nSceneID);
  tolua_variable(tolua_S,"szConfigName",tolua_get_SCENE_DATA_szConfigName,tolua_set_SCENE_DATA_szConfigName);
  tolua_variable(tolua_S,"szMapDisplayName",tolua_get_SCENE_DATA_szMapDisplayName,tolua_set_SCENE_DATA_szMapDisplayName);
  tolua_variable(tolua_S,"nType",tolua_get_SCENE_DATA_nType,tolua_set_SCENE_DATA_nType);
  tolua_variable(tolua_S,"nMaxRoleCount",tolua_get_SCENE_DATA_nMaxRoleCount,tolua_set_SCENE_DATA_nMaxRoleCount);
  tolua_variable(tolua_S,"nFinishTarget",tolua_get_SCENE_DATA_nFinishTarget,tolua_set_SCENE_DATA_nFinishTarget);
  tolua_variable(tolua_S,"bCannotUseScroll",tolua_get_SCENE_DATA_bCannotUseScroll,tolua_set_SCENE_DATA_bCannotUseScroll);
  tolua_variable(tolua_S,"bCannotUseTeleportRing",tolua_get_SCENE_DATA_bCannotUseTeleportRing,tolua_set_SCENE_DATA_bCannotUseTeleportRing);
  tolua_variable(tolua_S,"bCannotBeRecalled",tolua_get_SCENE_DATA_bCannotBeRecalled,tolua_set_SCENE_DATA_bCannotBeRecalled);
  tolua_variable(tolua_S,"bCannotTrade",tolua_get_SCENE_DATA_bCannotTrade,tolua_set_SCENE_DATA_bCannotTrade);
  tolua_variable(tolua_S,"bCannotLoseDurability",tolua_get_SCENE_DATA_bCannotLoseDurability,tolua_set_SCENE_DATA_bCannotLoseDurability);
  tolua_variable(tolua_S,"bCannotSendMail",tolua_get_SCENE_DATA_bCannotSendMail,tolua_set_SCENE_DATA_bCannotSendMail);
  tolua_variable(tolua_S,"bCannotUseExchange",tolua_get_SCENE_DATA_bCannotUseExchange,tolua_set_SCENE_DATA_bCannotUseExchange);
  tolua_variable(tolua_S,"bCannotUseAuction",tolua_get_SCENE_DATA_bCannotUseAuction,tolua_set_SCENE_DATA_bCannotUseAuction);
  tolua_variable(tolua_S,"bCannotJoinTeam",tolua_get_SCENE_DATA_bCannotJoinTeam,tolua_set_SCENE_DATA_bCannotJoinTeam);
  tolua_variable(tolua_S,"bEnableAntiCheat",tolua_get_SCENE_DATA_bEnableAntiCheat,tolua_set_SCENE_DATA_bEnableAntiCheat);
  tolua_variable(tolua_S,"bBlockRole",tolua_get_SCENE_DATA_bBlockRole,tolua_set_SCENE_DATA_bBlockRole);
  tolua_variable(tolua_S,"bCannotGroupChat",tolua_get_SCENE_DATA_bCannotGroupChat,tolua_set_SCENE_DATA_bCannotGroupChat);
  tolua_variable(tolua_S,"bCannotSceneChat",tolua_get_SCENE_DATA_bCannotSceneChat,tolua_set_SCENE_DATA_bCannotSceneChat);
  tolua_variable(tolua_S,"bCannotScreenChat",tolua_get_SCENE_DATA_bCannotScreenChat,tolua_set_SCENE_DATA_bCannotScreenChat);
  tolua_variable(tolua_S,"bCannotWhisperChat",tolua_get_SCENE_DATA_bCannotWhisperChat,tolua_set_SCENE_DATA_bCannotWhisperChat);
  tolua_variable(tolua_S,"bCannotTeamChat",tolua_get_SCENE_DATA_bCannotTeamChat,tolua_set_SCENE_DATA_bCannotTeamChat);
  tolua_variable(tolua_S,"bCannotGuildChat",tolua_get_SCENE_DATA_bCannotGuildChat,tolua_set_SCENE_DATA_bCannotGuildChat);
  tolua_variable(tolua_S,"bCannotFriendChat",tolua_get_SCENE_DATA_bCannotFriendChat,tolua_set_SCENE_DATA_bCannotFriendChat);
  tolua_variable(tolua_S,"nMinPortalLevel",tolua_get_SCENE_DATA_nMinPortalLevel,tolua_set_SCENE_DATA_nMinPortalLevel);
  tolua_variable(tolua_S,"nMaxPortalLevel",tolua_get_SCENE_DATA_nMaxPortalLevel,tolua_set_SCENE_DATA_nMaxPortalLevel);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"WEATHER_DATA","SCENE_DATA::WEATHER_DATA","",tolua_collect_SCENE_DATA__WEATHER_DATA);
  #else
  tolua_cclass(tolua_S,"WEATHER_DATA","SCENE_DATA::WEATHER_DATA","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"WEATHER_DATA");
   tolua_variable(tolua_S,"nWeather",tolua_get_SCENE_DATA__WEATHER_DATA_nWeather,tolua_set_SCENE_DATA__WEATHER_DATA_nWeather);
   tolua_variable(tolua_S,"nWeight",tolua_get_SCENE_DATA__WEATHER_DATA_nWeight,tolua_set_SCENE_DATA__WEATHER_DATA_nWeight);
   tolua_variable(tolua_S,"nMinDuration",tolua_get_SCENE_DATA__WEATHER_DATA_nMinDuration,tolua_set_SCENE_DATA__WEATHER_DATA_nMinDuration);
   tolua_variable(tolua_S,"nMaxDuration",tolua_get_SCENE_DATA__WEATHER_DATA_nMaxDuration,tolua_set_SCENE_DATA__WEATHER_DATA_nMaxDuration);
  tolua_endmodule(tolua_S);
  tolua_array(tolua_S,"WeatherData",tolua_get_data_manager_SCENE_DATA_WeatherData,tolua_set_data_manager_SCENE_DATA_WeatherData);
  tolua_variable(tolua_S,"nMinWeatherRefreshInterval",tolua_get_SCENE_DATA_nMinWeatherRefreshInterval,tolua_set_SCENE_DATA_nMinWeatherRefreshInterval);
  tolua_variable(tolua_S,"nMaxWeatherRefreshInterval",tolua_get_SCENE_DATA_nMaxWeatherRefreshInterval,tolua_set_SCENE_DATA_nMaxWeatherRefreshInterval);
  tolua_variable(tolua_S,"RandWeather",tolua_get_SCENE_DATA_RandWeather,tolua_set_SCENE_DATA_RandWeather);
  tolua_variable(tolua_S,"nMinPlayerLevel",tolua_get_SCENE_DATA_nMinPlayerLevel,tolua_set_SCENE_DATA_nMinPlayerLevel);
  tolua_variable(tolua_S,"nMaxPlayerLevel",tolua_get_SCENE_DATA_nMaxPlayerLevel,tolua_set_SCENE_DATA_nMaxPlayerLevel);
  tolua_variable(tolua_S,"nInitCount",tolua_get_SCENE_DATA_nInitCount,tolua_set_SCENE_DATA_nInitCount);
  tolua_variable(tolua_S,"nSideCount",tolua_get_SCENE_DATA_nSideCount,tolua_set_SCENE_DATA_nSideCount);
  tolua_variable(tolua_S,"nBattleType",tolua_get_SCENE_DATA_nBattleType,tolua_set_SCENE_DATA_nBattleType);
  tolua_variable(tolua_S,"nMinRoleLimitPerSide",tolua_get_SCENE_DATA_nMinRoleLimitPerSide,tolua_set_SCENE_DATA_nMinRoleLimitPerSide);
  tolua_variable(tolua_S,"nMaxRoleLimitPerSide",tolua_get_SCENE_DATA_nMaxRoleLimitPerSide,tolua_set_SCENE_DATA_nMaxRoleLimitPerSide);
  tolua_variable(tolua_S,"nHandbookType",tolua_get_SCENE_DATA_nHandbookType,tolua_set_SCENE_DATA_nHandbookType);
  tolua_variable(tolua_S,"nHandbookIndex",tolua_get_SCENE_DATA_nHandbookIndex,tolua_set_SCENE_DATA_nHandbookIndex);
  tolua_variable(tolua_S,"nLoginJumpSceneTempID",tolua_get_SCENE_DATA_nLoginJumpSceneTempID,tolua_set_SCENE_DATA_nLoginJumpSceneTempID);
  tolua_variable(tolua_S,"nLoginJumpX",tolua_get_SCENE_DATA_nLoginJumpX,tolua_set_SCENE_DATA_nLoginJumpX);
  tolua_variable(tolua_S,"nLoginJumpY",tolua_get_SCENE_DATA_nLoginJumpY,tolua_set_SCENE_DATA_nLoginJumpY);
  tolua_variable(tolua_S,"nLoginJumpZ",tolua_get_SCENE_DATA_nLoginJumpZ,tolua_set_SCENE_DATA_nLoginJumpZ);
  tolua_variable(tolua_S,"bEnableIntimacy",tolua_get_SCENE_DATA_bEnableIntimacy,tolua_set_SCENE_DATA_bEnableIntimacy);
  tolua_variable(tolua_S,"bGpkPunish",tolua_get_SCENE_DATA_bGpkPunish,tolua_set_SCENE_DATA_bGpkPunish);
  tolua_variable(tolua_S,"bGpkKick",tolua_get_SCENE_DATA_bGpkKick,tolua_set_SCENE_DATA_bGpkKick);
#ifndef TOLUA_DISABLE_tolua_data_manager_SCENE_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_SCENE_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"SHOP_DATA","SHOP_DATA","",NULL);
 tolua_beginmodule(tolua_S,"SHOP_DATA");
  tolua_variable(tolua_S,"m_nShopID",tolua_get_SHOP_DATA_m_nShopID,tolua_set_SHOP_DATA_m_nShopID);
  tolua_variable(tolua_S,"m_nIndex",tolua_get_SHOP_DATA_m_nIndex,tolua_set_SHOP_DATA_m_nIndex);
  tolua_variable(tolua_S,"m_szShopName",tolua_get_SHOP_DATA_m_szShopName,tolua_set_SHOP_DATA_m_szShopName);
  tolua_variable(tolua_S,"m_nItemID",tolua_get_SHOP_DATA_m_nItemID,tolua_set_SHOP_DATA_m_nItemID);
  tolua_variable(tolua_S,"m_nItemCount",tolua_get_SHOP_DATA_m_nItemCount,tolua_set_SHOP_DATA_m_nItemCount);
  tolua_variable(tolua_S,"m_nCostItemID1",tolua_get_SHOP_DATA_m_nCostItemID1,tolua_set_SHOP_DATA_m_nCostItemID1);
  tolua_variable(tolua_S,"m_nCostValue1",tolua_get_SHOP_DATA_m_nCostValue1,tolua_set_SHOP_DATA_m_nCostValue1);
  tolua_variable(tolua_S,"m_nCostItemID2",tolua_get_SHOP_DATA_m_nCostItemID2,tolua_set_SHOP_DATA_m_nCostItemID2);
  tolua_variable(tolua_S,"m_nCostValue2",tolua_get_SHOP_DATA_m_nCostValue2,tolua_set_SHOP_DATA_m_nCostValue2);
  tolua_variable(tolua_S,"m_nRequireForceID",tolua_get_SHOP_DATA_m_nRequireForceID,tolua_set_SHOP_DATA_m_nRequireForceID);
  tolua_variable(tolua_S,"m_nReputation",tolua_get_SHOP_DATA_m_nReputation,tolua_set_SHOP_DATA_m_nReputation);
  tolua_variable(tolua_S,"m_nRequireGuildBuildingID",tolua_get_SHOP_DATA_m_nRequireGuildBuildingID,tolua_set_SHOP_DATA_m_nRequireGuildBuildingID);
  tolua_variable(tolua_S,"m_nBuildingLevel",tolua_get_SHOP_DATA_m_nBuildingLevel,tolua_set_SHOP_DATA_m_nBuildingLevel);
  tolua_variable(tolua_S,"m_nShopTypeSell",tolua_get_SHOP_DATA_m_nShopTypeSell,tolua_set_SHOP_DATA_m_nShopTypeSell);
  tolua_variable(tolua_S,"m_pItemTemplate",tolua_get_SHOP_DATA_m_pItemTemplate_ptr,NULL);
#ifndef TOLUA_DISABLE_tolua_data_manager_SHOP_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_SHOP_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"ATTR_LEVEL","ATTR_LEVEL","",NULL);
 tolua_beginmodule(tolua_S,"ATTR_LEVEL");
  tolua_variable(tolua_S,"m_nLevel",tolua_get_ATTR_LEVEL_m_nLevel,tolua_set_ATTR_LEVEL_m_nLevel);
  tolua_variable(tolua_S,"m_nClass",tolua_get_ATTR_LEVEL_m_nClass,tolua_set_ATTR_LEVEL_m_nClass);
  tolua_array(tolua_S,"m_nMainAttr",tolua_get_data_manager_ATTR_LEVEL_m_nMainAttr,tolua_set_data_manager_ATTR_LEVEL_m_nMainAttr);
#ifndef TOLUA_DISABLE_tolua_data_manager_ATTR_LEVEL_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_ATTR_LEVEL_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"ACHIEVEMENT_DATA","ACHIEVEMENT_DATA","",NULL);
 tolua_beginmodule(tolua_S,"ACHIEVEMENT_DATA");
  tolua_variable(tolua_S,"nAchievementID",tolua_get_ACHIEVEMENT_DATA_nAchievementID,tolua_set_ACHIEVEMENT_DATA_nAchievementID);
  tolua_variable(tolua_S,"szName",tolua_get_ACHIEVEMENT_DATA_szName,tolua_set_ACHIEVEMENT_DATA_szName);
  tolua_array(tolua_S,"vecAchievementType",tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementType,tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementType);
  tolua_array(tolua_S,"vecAchievementValue1",tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementValue1,tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementValue1);
  tolua_array(tolua_S,"vecAchievementValue2",tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementValue2,tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementValue2);
  tolua_array(tolua_S,"vecAchievementValue3",tolua_get_data_manager_ACHIEVEMENT_DATA_vecAchievementValue3,tolua_set_data_manager_ACHIEVEMENT_DATA_vecAchievementValue3);
  tolua_variable(tolua_S,"nAwardSlotCount",tolua_get_ACHIEVEMENT_DATA_nAwardSlotCount,tolua_set_ACHIEVEMENT_DATA_nAwardSlotCount);
  tolua_variable(tolua_S,"nPreAchievementID1",tolua_get_ACHIEVEMENT_DATA_nPreAchievementID1,tolua_set_ACHIEVEMENT_DATA_nPreAchievementID1);
  tolua_variable(tolua_S,"nPreAchievementID2",tolua_get_ACHIEVEMENT_DATA_nPreAchievementID2,tolua_set_ACHIEVEMENT_DATA_nPreAchievementID2);
  tolua_array(tolua_S,"nRewardType",tolua_get_data_manager_ACHIEVEMENT_DATA_nRewardType,tolua_set_data_manager_ACHIEVEMENT_DATA_nRewardType);
  tolua_array(tolua_S,"nRewardID",tolua_get_data_manager_ACHIEVEMENT_DATA_nRewardID,tolua_set_data_manager_ACHIEVEMENT_DATA_nRewardID);
  tolua_variable(tolua_S,"nAchievementPts",tolua_get_ACHIEVEMENT_DATA_nAchievementPts,tolua_set_ACHIEVEMENT_DATA_nAchievementPts);
#ifndef TOLUA_DISABLE_tolua_data_manager_ACHIEVEMENT_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_ACHIEVEMENT_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"HANDBOOK_DATA","HANDBOOK_DATA","",NULL);
 tolua_beginmodule(tolua_S,"HANDBOOK_DATA");
  tolua_variable(tolua_S,"nHandbookID",tolua_get_HANDBOOK_DATA_nHandbookID,tolua_set_HANDBOOK_DATA_nHandbookID);
  tolua_variable(tolua_S,"nHandbookType",tolua_get_HANDBOOK_DATA_nHandbookType,tolua_set_HANDBOOK_DATA_nHandbookType);
  tolua_variable(tolua_S,"nIndex",tolua_get_HANDBOOK_DATA_nIndex,tolua_set_HANDBOOK_DATA_nIndex);
  tolua_array(tolua_S,"nID",tolua_get_data_manager_HANDBOOK_DATA_nID,tolua_set_data_manager_HANDBOOK_DATA_nID);
  tolua_variable(tolua_S,"szValue",tolua_get_HANDBOOK_DATA_szValue,tolua_set_HANDBOOK_DATA_szValue);
#ifndef TOLUA_DISABLE_tolua_data_manager_HANDBOOK_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_HANDBOOK_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"TITLE_TEMPLATE","TITLE_TEMPLATE","",NULL);
 tolua_beginmodule(tolua_S,"TITLE_TEMPLATE");
  tolua_variable(tolua_S,"m_nTemplateID",tolua_get_TITLE_TEMPLATE_unsigned_m_nTemplateID,tolua_set_TITLE_TEMPLATE_unsigned_m_nTemplateID);
  tolua_variable(tolua_S,"m_szName",tolua_get_TITLE_TEMPLATE_m_szName,tolua_set_TITLE_TEMPLATE_m_szName);
  tolua_variable(tolua_S,"m_nValidTime",tolua_get_TITLE_TEMPLATE_m_nValidTime,tolua_set_TITLE_TEMPLATE_m_nValidTime);
  tolua_variable(tolua_S,"nPow",tolua_get_TITLE_TEMPLATE_nPow,tolua_set_TITLE_TEMPLATE_nPow);
  tolua_array(tolua_S,"m_nAttrID",tolua_get_data_manager_TITLE_TEMPLATE_m_nAttrID,tolua_set_data_manager_TITLE_TEMPLATE_m_nAttrID);
  tolua_variable(tolua_S,"m_nNodeCount",tolua_get_TITLE_TEMPLATE_m_nNodeCount,tolua_set_TITLE_TEMPLATE_m_nNodeCount);
  tolua_array(tolua_S,"m_nNodeIDs",tolua_get_data_manager_TITLE_TEMPLATE_m_nNodeIDs,tolua_set_data_manager_TITLE_TEMPLATE_m_nNodeIDs);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"TitleCondition","TITLE_TEMPLATE::TitleCondition","",tolua_collect_TITLE_TEMPLATE__TitleCondition);
  #else
  tolua_cclass(tolua_S,"TitleCondition","TITLE_TEMPLATE::TitleCondition","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"TitleCondition");
   tolua_variable(tolua_S,"shConType",tolua_get_TITLE_TEMPLATE__TitleCondition_shConType,tolua_set_TITLE_TEMPLATE__TitleCondition_shConType);
   tolua_variable(tolua_S,"shConValue",tolua_get_TITLE_TEMPLATE__TitleCondition_shConValue,tolua_set_TITLE_TEMPLATE__TitleCondition_shConValue);
  tolua_endmodule(tolua_S);
  tolua_array(tolua_S,"m_Condition",tolua_get_data_manager_TITLE_TEMPLATE_m_Condition,tolua_set_data_manager_TITLE_TEMPLATE_m_Condition);
#ifndef TOLUA_DISABLE_tolua_data_manager_TITLE_TEMPLATE_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_TITLE_TEMPLATE_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"GUILD_BUILDING_DATA","GUILD_BUILDING_DATA","",NULL);
 tolua_beginmodule(tolua_S,"GUILD_BUILDING_DATA");
  tolua_variable(tolua_S,"dwID",tolua_get_GUILD_BUILDING_DATA_unsigned_dwID,tolua_set_GUILD_BUILDING_DATA_unsigned_dwID);
  tolua_variable(tolua_S,"nLevel",tolua_get_GUILD_BUILDING_DATA_nLevel,tolua_set_GUILD_BUILDING_DATA_nLevel);
  tolua_variable(tolua_S,"nMaxLevel",tolua_get_GUILD_BUILDING_DATA_nMaxLevel,tolua_set_GUILD_BUILDING_DATA_nMaxLevel);
  tolua_variable(tolua_S,"nMaxNum",tolua_get_GUILD_BUILDING_DATA_nMaxNum,tolua_set_GUILD_BUILDING_DATA_nMaxNum);
  tolua_variable(tolua_S,"nProsperity",tolua_get_GUILD_BUILDING_DATA_nProsperity,tolua_set_GUILD_BUILDING_DATA_nProsperity);
  tolua_variable(tolua_S,"nMoneyConsumption",tolua_get_GUILD_BUILDING_DATA_nMoneyConsumption,tolua_set_GUILD_BUILDING_DATA_nMoneyConsumption);
  tolua_variable(tolua_S,"nTotalMoneyConsumption",tolua_get_GUILD_BUILDING_DATA_nTotalMoneyConsumption,tolua_set_GUILD_BUILDING_DATA_nTotalMoneyConsumption);
  tolua_variable(tolua_S,"nHerbConsumption",tolua_get_GUILD_BUILDING_DATA_nHerbConsumption,tolua_set_GUILD_BUILDING_DATA_nHerbConsumption);
  tolua_variable(tolua_S,"nTotalHerbConsumption",tolua_get_GUILD_BUILDING_DATA_nTotalHerbConsumption,tolua_set_GUILD_BUILDING_DATA_nTotalHerbConsumption);
  tolua_variable(tolua_S,"nWoodConsumption",tolua_get_GUILD_BUILDING_DATA_nWoodConsumption,tolua_set_GUILD_BUILDING_DATA_nWoodConsumption);
  tolua_variable(tolua_S,"nTotalWoodConsumption",tolua_get_GUILD_BUILDING_DATA_nTotalWoodConsumption,tolua_set_GUILD_BUILDING_DATA_nTotalWoodConsumption);
  tolua_variable(tolua_S,"nCrystalConsumption",tolua_get_GUILD_BUILDING_DATA_nCrystalConsumption,tolua_set_GUILD_BUILDING_DATA_nCrystalConsumption);
  tolua_variable(tolua_S,"nTotalCrystalConsumption",tolua_get_GUILD_BUILDING_DATA_nTotalCrystalConsumption,tolua_set_GUILD_BUILDING_DATA_nTotalCrystalConsumption);
  tolua_variable(tolua_S,"nSilverConsumption",tolua_get_GUILD_BUILDING_DATA_nSilverConsumption,tolua_set_GUILD_BUILDING_DATA_nSilverConsumption);
  tolua_variable(tolua_S,"nTotalSilverConsumption",tolua_get_GUILD_BUILDING_DATA_nTotalSilverConsumption,tolua_set_GUILD_BUILDING_DATA_nTotalSilverConsumption);
  tolua_variable(tolua_S,"nUpgradeTime",tolua_get_GUILD_BUILDING_DATA_nUpgradeTime,tolua_set_GUILD_BUILDING_DATA_nUpgradeTime);
  tolua_variable(tolua_S,"nRoutineConsumption",tolua_get_GUILD_BUILDING_DATA_nRoutineConsumption,tolua_set_GUILD_BUILDING_DATA_nRoutineConsumption);
  tolua_variable(tolua_S,"dwCdtnBaseNpcID",tolua_get_GUILD_BUILDING_DATA_unsigned_dwCdtnBaseNpcID,tolua_set_GUILD_BUILDING_DATA_unsigned_dwCdtnBaseNpcID);
  tolua_variable(tolua_S,"dwConditionBuildingID",tolua_get_GUILD_BUILDING_DATA_unsigned_dwConditionBuildingID,tolua_set_GUILD_BUILDING_DATA_unsigned_dwConditionBuildingID);
  tolua_variable(tolua_S,"nConditionBuildingLvl",tolua_get_GUILD_BUILDING_DATA_nConditionBuildingLvl,tolua_set_GUILD_BUILDING_DATA_nConditionBuildingLvl);
  tolua_variable(tolua_S,"dwUpgradingSpellID",tolua_get_GUILD_BUILDING_DATA_unsigned_dwUpgradingSpellID,tolua_set_GUILD_BUILDING_DATA_unsigned_dwUpgradingSpellID);
  tolua_variable(tolua_S,"dwNpcTemplateID",tolua_get_GUILD_BUILDING_DATA_unsigned_dwNpcTemplateID,tolua_set_GUILD_BUILDING_DATA_unsigned_dwNpcTemplateID);
  tolua_variable(tolua_S,"dwDeadNpcTemplateID",tolua_get_GUILD_BUILDING_DATA_unsigned_dwDeadNpcTemplateID,tolua_set_GUILD_BUILDING_DATA_unsigned_dwDeadNpcTemplateID);
  tolua_variable(tolua_S,"nReviveTime",tolua_get_GUILD_BUILDING_DATA_unsigned_nReviveTime,tolua_set_GUILD_BUILDING_DATA_unsigned_nReviveTime);
  tolua_variable(tolua_S,"nParam1",tolua_get_GUILD_BUILDING_DATA_nParam1,tolua_set_GUILD_BUILDING_DATA_nParam1);
  tolua_variable(tolua_S,"nParam2",tolua_get_GUILD_BUILDING_DATA_nParam2,tolua_set_GUILD_BUILDING_DATA_nParam2);
  tolua_variable(tolua_S,"nParam3",tolua_get_GUILD_BUILDING_DATA_nParam3,tolua_set_GUILD_BUILDING_DATA_nParam3);
  tolua_variable(tolua_S,"nParam4",tolua_get_GUILD_BUILDING_DATA_nParam4,tolua_set_GUILD_BUILDING_DATA_nParam4);
  tolua_variable(tolua_S,"nParam5",tolua_get_GUILD_BUILDING_DATA_nParam5,tolua_set_GUILD_BUILDING_DATA_nParam5);
  tolua_variable(tolua_S,"dwIntruderMsgID",tolua_get_GUILD_BUILDING_DATA_unsigned_dwIntruderMsgID,tolua_set_GUILD_BUILDING_DATA_unsigned_dwIntruderMsgID);
  tolua_variable(tolua_S,"dwDefenderMsgID",tolua_get_GUILD_BUILDING_DATA_unsigned_dwDefenderMsgID,tolua_set_GUILD_BUILDING_DATA_unsigned_dwDefenderMsgID);
  tolua_variable(tolua_S,"dwFewMembersReward",tolua_get_GUILD_BUILDING_DATA_unsigned_dwFewMembersReward,tolua_set_GUILD_BUILDING_DATA_unsigned_dwFewMembersReward);
  tolua_variable(tolua_S,"dwSomeMembersReward",tolua_get_GUILD_BUILDING_DATA_unsigned_dwSomeMembersReward,tolua_set_GUILD_BUILDING_DATA_unsigned_dwSomeMembersReward);
  tolua_variable(tolua_S,"dwManyMembersReward",tolua_get_GUILD_BUILDING_DATA_unsigned_dwManyMembersReward,tolua_set_GUILD_BUILDING_DATA_unsigned_dwManyMembersReward);
#ifndef TOLUA_DISABLE_tolua_data_manager_GUILD_BUILDING_DATA_HasName00
  tolua_function(tolua_S,"HasName",tolua_data_manager_GUILD_BUILDING_DATA_HasName00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_GUILD_BUILDING_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_GUILD_BUILDING_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"GUILD_ACTIVITY_DATA","GUILD_ACTIVITY_DATA","",NULL);
 tolua_beginmodule(tolua_S,"GUILD_ACTIVITY_DATA");
  tolua_variable(tolua_S,"nType",tolua_get_GUILD_ACTIVITY_DATA_nType,tolua_set_GUILD_ACTIVITY_DATA_nType);
  tolua_variable(tolua_S,"nLevel",tolua_get_GUILD_ACTIVITY_DATA_nLevel,tolua_set_GUILD_ACTIVITY_DATA_nLevel);
  tolua_variable(tolua_S,"nGuildVariableIndex",tolua_get_GUILD_ACTIVITY_DATA_nGuildVariableIndex,tolua_set_GUILD_ACTIVITY_DATA_nGuildVariableIndex);
  tolua_variable(tolua_S,"nLastTime",tolua_get_GUILD_ACTIVITY_DATA_nLastTime,tolua_set_GUILD_ACTIVITY_DATA_nLastTime);
  tolua_variable(tolua_S,"nRepeatNum",tolua_get_GUILD_ACTIVITY_DATA_nRepeatNum,tolua_set_GUILD_ACTIVITY_DATA_nRepeatNum);
  tolua_variable(tolua_S,"nBaseBuildingType",tolua_get_GUILD_ACTIVITY_DATA_nBaseBuildingType,tolua_set_GUILD_ACTIVITY_DATA_nBaseBuildingType);
  tolua_variable(tolua_S,"nBaseBuildingLevel",tolua_get_GUILD_ACTIVITY_DATA_nBaseBuildingLevel,tolua_set_GUILD_ACTIVITY_DATA_nBaseBuildingLevel);
  tolua_variable(tolua_S,"nConsumeMoney",tolua_get_GUILD_ACTIVITY_DATA_nConsumeMoney,tolua_set_GUILD_ACTIVITY_DATA_nConsumeMoney);
  tolua_variable(tolua_S,"nConsumeHerb",tolua_get_GUILD_ACTIVITY_DATA_nConsumeHerb,tolua_set_GUILD_ACTIVITY_DATA_nConsumeHerb);
  tolua_variable(tolua_S,"nConsumeWood",tolua_get_GUILD_ACTIVITY_DATA_nConsumeWood,tolua_set_GUILD_ACTIVITY_DATA_nConsumeWood);
  tolua_variable(tolua_S,"nConsumeCrystal",tolua_get_GUILD_ACTIVITY_DATA_nConsumeCrystal,tolua_set_GUILD_ACTIVITY_DATA_nConsumeCrystal);
  tolua_variable(tolua_S,"nConsumeSilver",tolua_get_GUILD_ACTIVITY_DATA_nConsumeSilver,tolua_set_GUILD_ACTIVITY_DATA_nConsumeSilver);
  tolua_variable(tolua_S,"ncltLightingID",tolua_get_GUILD_ACTIVITY_DATA_ncltLightingID,tolua_set_GUILD_ACTIVITY_DATA_ncltLightingID);
#ifndef TOLUA_DISABLE_tolua_data_manager_GUILD_ACTIVITY_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_GUILD_ACTIVITY_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"ATTRIBUTE_DATA","ATTRIBUTE_DATA","",NULL);
 tolua_beginmodule(tolua_S,"ATTRIBUTE_DATA");
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"SOUL_DATA","SOUL_DATA","",NULL);
 tolua_beginmodule(tolua_S,"SOUL_DATA");
  tolua_variable(tolua_S,"m_nTemplateID",tolua_get_SOUL_DATA_m_nTemplateID,tolua_set_SOUL_DATA_m_nTemplateID);
  tolua_variable(tolua_S,"m_nAttrID",tolua_get_SOUL_DATA_m_nAttrID,tolua_set_SOUL_DATA_m_nAttrID);
  tolua_variable(tolua_S,"m_nMatchSocket",tolua_get_SOUL_DATA_m_nMatchSocket,tolua_set_SOUL_DATA_m_nMatchSocket);
  tolua_variable(tolua_S,"m_nSoulLevel",tolua_get_SOUL_DATA_m_nSoulLevel,tolua_set_SOUL_DATA_m_nSoulLevel);
  tolua_variable(tolua_S,"m_nPriceValue",tolua_get_SOUL_DATA_m_nPriceValue,tolua_set_SOUL_DATA_m_nPriceValue);
  tolua_variable(tolua_S,"m_nUpgradeSoulID",tolua_get_SOUL_DATA_m_nUpgradeSoulID,tolua_set_SOUL_DATA_m_nUpgradeSoulID);
  tolua_variable(tolua_S,"m_nRemovePrice",tolua_get_SOUL_DATA_m_nRemovePrice,tolua_set_SOUL_DATA_m_nRemovePrice);
  tolua_variable(tolua_S,"m_pAttrData",tolua_get_SOUL_DATA_m_pAttrData_ptr,NULL);
#ifndef TOLUA_DISABLE_tolua_data_manager_SOUL_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_SOUL_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"PET_DATA","PET_DATA","",NULL);
 tolua_beginmodule(tolua_S,"PET_DATA");
  tolua_variable(tolua_S,"nID",tolua_get_PET_DATA_nID,tolua_set_PET_DATA_nID);
  tolua_variable(tolua_S,"nAIID",tolua_get_PET_DATA_nAIID,tolua_set_PET_DATA_nAIID);
  tolua_variable(tolua_S,"nBornType",tolua_get_PET_DATA_nBornType,tolua_set_PET_DATA_nBornType);
  tolua_variable(tolua_S,"nLevel",tolua_get_PET_DATA_nLevel,tolua_set_PET_DATA_nLevel);
  tolua_variable(tolua_S,"nAccurate",tolua_get_PET_DATA_nAccurate,tolua_set_PET_DATA_nAccurate);
  tolua_variable(tolua_S,"nBonusphyAtkMax",tolua_get_PET_DATA_nBonusphyAtkMax,tolua_set_PET_DATA_nBonusphyAtkMax);
  tolua_variable(tolua_S,"nBonusHPMaxA",tolua_get_PET_DATA_nBonusHPMaxA,tolua_set_PET_DATA_nBonusHPMaxA);
  tolua_variable(tolua_S,"nBonusHPMaxB",tolua_get_PET_DATA_nBonusHPMaxB,tolua_set_PET_DATA_nBonusHPMaxB);
  tolua_variable(tolua_S,"nMagDef",tolua_get_PET_DATA_nMagDef,tolua_set_PET_DATA_nMagDef);
  tolua_variable(tolua_S,"nMagDefMax",tolua_get_PET_DATA_nMagDefMax,tolua_set_PET_DATA_nMagDefMax);
#ifndef TOLUA_DISABLE_tolua_data_manager_PET_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_PET_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
#ifndef TOLUA_DISABLE_tolua_data_manager_GetPetData00
 tolua_function(tolua_S,"GetPetData",tolua_data_manager_GetPetData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_cclass(tolua_S,"MAIL_TEMPLATE_DATA","MAIL_TEMPLATE_DATA","",NULL);
 tolua_beginmodule(tolua_S,"MAIL_TEMPLATE_DATA");
  tolua_variable(tolua_S,"m_nTemplateID",tolua_get_MAIL_TEMPLATE_DATA_m_nTemplateID,tolua_set_MAIL_TEMPLATE_DATA_m_nTemplateID);
  tolua_variable(tolua_S,"m_szMailSubject",tolua_get_MAIL_TEMPLATE_DATA_m_szMailSubject,tolua_set_MAIL_TEMPLATE_DATA_m_szMailSubject);
  tolua_variable(tolua_S,"m_szMailContent",tolua_get_MAIL_TEMPLATE_DATA_m_szMailContent,tolua_set_MAIL_TEMPLATE_DATA_m_szMailContent);
  tolua_variable(tolua_S,"m_nItemID",tolua_get_MAIL_TEMPLATE_DATA_m_nItemID,tolua_set_MAIL_TEMPLATE_DATA_m_nItemID);
  tolua_variable(tolua_S,"m_nItemCount",tolua_get_MAIL_TEMPLATE_DATA_m_nItemCount,tolua_set_MAIL_TEMPLATE_DATA_m_nItemCount);
#ifndef TOLUA_DISABLE_tolua_data_manager_MAIL_TEMPLATE_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_MAIL_TEMPLATE_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"EQUIP_DATA","EQUIP_DATA","ITEM_DATA",NULL);
 tolua_beginmodule(tolua_S,"EQUIP_DATA");
  tolua_variable(tolua_S,"m_byEquipType",tolua_get_EQUIP_DATA_unsigned_m_byEquipType,tolua_set_EQUIP_DATA_unsigned_m_byEquipType);
  tolua_variable(tolua_S,"m_byIdentified",tolua_get_EQUIP_DATA_unsigned_m_byIdentified,tolua_set_EQUIP_DATA_unsigned_m_byIdentified);
  tolua_variable(tolua_S,"m_bySocketCountMin",tolua_get_EQUIP_DATA_unsigned_m_bySocketCountMin,tolua_set_EQUIP_DATA_unsigned_m_bySocketCountMin);
  tolua_variable(tolua_S,"m_bySocketCountMax",tolua_get_EQUIP_DATA_unsigned_m_bySocketCountMax,tolua_set_EQUIP_DATA_unsigned_m_bySocketCountMax);
  tolua_array(tolua_S,"m_nFixAttr",tolua_get_data_manager_EQUIP_DATA_m_nFixAttr,tolua_set_data_manager_EQUIP_DATA_m_nFixAttr);
#ifndef TOLUA_DISABLE_tolua_data_manager_EQUIP_DATA_ReadLine00
  tolua_function(tolua_S,"ReadLine",tolua_data_manager_EQUIP_DATA_ReadLine00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_NEW_AI_DATA_","CDataManager<NEW_AI_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_NEW_AI_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_NEW_AI_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_NEW_AI_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_NEW_AI_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_NEW_AI_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_NEW_AI_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NEW_AI_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_NEW_AI_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_AURA_ENTRY_","CDataManager<AURA_ENTRY>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_AURA_ENTRY_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_AURA_ENTRY__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_AURA_ENTRY__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_AURA_ENTRY__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_AURA_ENTRY__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_AURA_ENTRY__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_AURA_ENTRY__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_AURA_ENTRY__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_DOODAD_DATA_","CDataManager<DOODAD_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_DOODAD_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_DOODAD_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_DOODAD_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_DOODAD_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_DOODAD_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_DOODAD_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_DOODAD_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_DOODAD_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_EQUIP_DATA_","CDataManager<EQUIP_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_EQUIP_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_EQUIP_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_EQUIP_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_EQUIP_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_EQUIP_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_EQUIP_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_EQUIP_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_EQUIP_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_INIT_REPUTATION_DATA_","CDataManager<INIT_REPUTATION_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_INIT_REPUTATION_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_INIT_REPUTATION_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_ITEM_DATA_","CDataManager<ITEM_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_ITEM_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_ITEM_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_ITEM_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_ITEM_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_ITEM_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_ITEM_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ITEM_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_ITEM_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_ROLE_LEVEL_DATA_","CDataManager<ROLE_LEVEL_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_ROLE_LEVEL_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_ROLE_LEVEL_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_HERO_LEVEL_DATA_","CDataManager<HERO_LEVEL_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_HERO_LEVEL_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_HERO_LEVEL_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_MISSILE_DATA_","CDataManager<MISSILE_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_MISSILE_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_MISSILE_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_MISSILE_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_MISSILE_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_MISSILE_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_MISSILE_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MISSILE_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_MISSILE_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_NPC_DATA_","CDataManager<NPC_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_NPC_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_NPC_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_NPC_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_NPC_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_NPC_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_NPC_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_NPC_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_NPC_RELATION_DATA_","CDataManager<NPC_RELATION_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_NPC_RELATION_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_NPC_RELATION_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_QUEST_DATA_","CDataManager<QUEST_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_QUEST_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_QUEST_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_QUEST_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_QUEST_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_QUEST_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_QUEST_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_QUEST_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_QUEST_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_ROLE_RELATION_DATA_","CDataManager<ROLE_RELATION_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_ROLE_RELATION_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_ROLE_RELATION_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_SCENE_DATA_","CDataManager<SCENE_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_SCENE_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_SCENE_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_SCENE_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_SCENE_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_SCENE_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_SCENE_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SCENE_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_SCENE_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_SHOP_DATA_","CDataManager<SHOP_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_SHOP_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_SHOP_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_SHOP_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_SHOP_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_SHOP_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_SHOP_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SHOP_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_SHOP_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_SPELL_ENTRY_","CDataManager<SPELL_ENTRY>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_SPELL_ENTRY_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_SPELL_ENTRY__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_SPELL_ENTRY__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_SPELL_ENTRY__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_SPELL_ENTRY__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_SPELL_ENTRY__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_ENTRY__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_SPELL_ENTRY__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_SPELL_VAR_","CDataManager<SPELL_VAR>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_SPELL_VAR_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_SPELL_VAR__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_SPELL_VAR__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_SPELL_VAR__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_SPELL_VAR__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_SPELL_VAR__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SPELL_VAR__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_SPELL_VAR__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_ATTR_LEVEL_","CDataManager<ATTR_LEVEL>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_ATTR_LEVEL_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_ATTR_LEVEL__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_ATTR_LEVEL__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_ATTR_LEVEL__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_ATTR_LEVEL__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_ATTR_LEVEL__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ATTR_LEVEL__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_ATTR_LEVEL__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_ACHIEVEMENT_DATA_","CDataManager<ACHIEVEMENT_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_ACHIEVEMENT_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_ACHIEVEMENT_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_TITLE_TEMPLATE_","CDataManager<TITLE_TEMPLATE>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_TITLE_TEMPLATE_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_TITLE_TEMPLATE__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_GUILD_BUILDING_DATA_","CDataManager<GUILD_BUILDING_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_GUILD_BUILDING_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_GUILD_BUILDING_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_GUILD_ACTIVITY_DATA_","CDataManager<GUILD_ACTIVITY_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_GUILD_ACTIVITY_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_GUILD_ACTIVITY_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_SOUL_DATA_","CDataManager<SOUL_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_SOUL_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_SOUL_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_SOUL_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_SOUL_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_SOUL_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_SOUL_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_SOUL_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_SOUL_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"CDataManager_MAIL_TEMPLATE_DATA_","CDataManager<MAIL_TEMPLATE_DATA>","",NULL);
 tolua_beginmodule(tolua_S,"CDataManager_MAIL_TEMPLATE_DATA_");
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetInstance00
  tolua_function(tolua_S,"GetInstance",tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetInstance00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetDataCount00
  tolua_function(tolua_S,"GetDataCount",tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetDataCount00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetData00
  tolua_function(tolua_S,"GetData",tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetID00
  tolua_function(tolua_S,"GetID",tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetFirstData00
  tolua_function(tolua_S,"GetFirstData",tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetFirstData00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetNextData00
  tolua_function(tolua_S,"GetNextData",tolua_data_manager_CDataManager_MAIL_TEMPLATE_DATA__GetNextData00);
#endif //#ifndef TOLUA_DISABLE
 tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_data_manager (lua_State* tolua_S) {
 return tolua_data_manager_open(tolua_S);
};
#endif

