/*
** Lua binding: enum
** Generated automatically by tolua++-1.0.92 on 02/11/26 14:42:23.
*/

#include "stdafx.h"

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int tolua_enum_open (lua_State* tolua_S);

#include "../../define/error_def.h"
#include "../../define/id_def.h"
#include "../../define/server_def.h"
#include "../../define/npc_def.h"
#include "../../define/role_def.h"
#include "../../define/doodad_def.h"
#include "../../define/char_def.h"
#include "../../define/missile_def.h"
#include "../../define/grid_def.h"
#include "../../define/scene_def.h"
#include "../../define/scene_file_def.h"
#include "../../define/attribute_def.h"
#include "../../define/modifier_def.h"
#include "../../define/spell_def.h"
#include "../../define/aura_def.h"
#include "../../define/ai_def.h"
#include "../../define/chat_def.h"
#include "../../define/friend_def.h"
#include "../../define/item_def.h"
#include "../../define/money_def.h"
#include "../../define/quest_def.h"
#include "../../define/team_def.h"
#include "../../define/room_def.h"
#include "../../define/adventure_def.h"
#include "../../define/exchange_def.h"
#include "../../define/achievement_def.h"
#include "../../define/master_def.h"
#include "../../define/queue_match_def.h"
#include "../../define/variable_def.h"
#include "../../define/guild_def.h"
#include "../../define/gm_svr_def.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"GRID_DATA");
 tolua_usertype(tolua_S,"DROP_CHANCE_DATA");
 tolua_usertype(tolua_S,"ABSOLUTE_SCENE_POS");
 tolua_usertype(tolua_S,"GRID_INSTANCE_DATA");
 tolua_usertype(tolua_S,"QUEUE_DATA");
 tolua_usertype(tolua_S,"ENTER_POS");
}

/* function: MakeObjectID */
#ifndef TOLUA_DISABLE_tolua_enum_MakeObjectID00
static int tolua_enum_MakeObjectID00(lua_State* tolua_S)
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
  unsigned int dwType = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int dwIndex = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
 {
  unsigned int tolua_ret = ( unsigned int)  MakeObjectID(dwType,dwIndex);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MakeObjectID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GetTypeFromID */
#ifndef TOLUA_DISABLE_tolua_enum_GetTypeFromID00
static int tolua_enum_GetTypeFromID00(lua_State* tolua_S)
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
  unsigned int dwObjID = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
 {
  signed int tolua_ret = ( signed int)  GetTypeFromID(dwObjID);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTypeFromID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GetIndexFromID */
#ifndef TOLUA_DISABLE_tolua_enum_GetIndexFromID00
static int tolua_enum_GetIndexFromID00(lua_State* tolua_S)
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
  unsigned int dwObjID = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
 {
  signed int tolua_ret = ( signed int)  GetIndexFromID(dwObjID);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetIndexFromID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uNextGrid of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uNextGrid
static int tolua_get_GRID_DATA_uNextGrid(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uNextGrid'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uNextGrid);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uNextGrid of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uNextGrid
static int tolua_set_GRID_DATA_uNextGrid(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uNextGrid'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uNextGrid = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uNoObstacle of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uNoObstacle
static int tolua_get_GRID_DATA_uNoObstacle(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uNoObstacle'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uNoObstacle);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uNoObstacle of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uNoObstacle
static int tolua_set_GRID_DATA_uNoObstacle(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uNoObstacle'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uNoObstacle = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uVolume of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uVolume
static int tolua_get_GRID_DATA_uVolume(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uVolume'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uVolume);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uVolume of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uVolume
static int tolua_set_GRID_DATA_uVolume(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uVolume'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uVolume = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uDoor of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uDoor
static int tolua_get_GRID_DATA_uDoor(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uDoor'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uDoor);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uDoor of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uDoor
static int tolua_set_GRID_DATA_uDoor(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uDoor'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uDoor = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uPkType of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uPkType
static int tolua_get_GRID_DATA_uPkType(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uPkType'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uPkType);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uPkType of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uPkType
static int tolua_set_GRID_DATA_uPkType(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uPkType'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uPkType = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bBorder of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bBorder
static int tolua_get_GRID_DATA_bBorder(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBorder'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bBorder);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bBorder of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bBorder
static int tolua_set_GRID_DATA_bBorder(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBorder'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bBorder = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uStallLevel of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uStallLevel
static int tolua_get_GRID_DATA_uStallLevel(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uStallLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uStallLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uStallLevel of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uStallLevel
static int tolua_set_GRID_DATA_uStallLevel(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uStallLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uStallLevel = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bQuest of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bQuest
static int tolua_get_GRID_DATA_bQuest(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bQuest'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bQuest);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bQuest of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bQuest
static int tolua_set_GRID_DATA_bQuest(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bQuest'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bQuest = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bBlockMove of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bBlockMove
static int tolua_get_GRID_DATA_bBlockMove(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockMove'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bBlockMove);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bBlockMove of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bBlockMove
static int tolua_set_GRID_DATA_bBlockMove(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockMove'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bBlockMove = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bBlockLeap of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bBlockLeap
static int tolua_get_GRID_DATA_bBlockLeap(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockLeap'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bBlockLeap);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bBlockLeap of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bBlockLeap
static int tolua_set_GRID_DATA_bBlockLeap(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockLeap'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bBlockLeap = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bBlockMissile of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bBlockMissile
static int tolua_get_GRID_DATA_bBlockMissile(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockMissile'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bBlockMissile);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bBlockMissile of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bBlockMissile
static int tolua_set_GRID_DATA_bBlockMissile(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockMissile'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bBlockMissile = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bBlockBullet of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bBlockBullet
static int tolua_get_GRID_DATA_bBlockBullet(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockBullet'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bBlockBullet);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bBlockBullet of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bBlockBullet
static int tolua_set_GRID_DATA_bBlockBullet(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bBlockBullet'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bBlockBullet = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bNotBlockChar of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bNotBlockChar
static int tolua_get_GRID_DATA_bNotBlockChar(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bNotBlockChar'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bNotBlockChar);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bNotBlockChar of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bNotBlockChar
static int tolua_set_GRID_DATA_bNotBlockChar(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bNotBlockChar'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bNotBlockChar = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bMine of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bMine
static int tolua_get_GRID_DATA_bMine(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bMine'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bMine);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bMine of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bMine
static int tolua_set_GRID_DATA_bMine(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bMine'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bMine = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotTeleport of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bCannotTeleport
static int tolua_get_GRID_DATA_bCannotTeleport(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotTeleport'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bCannotTeleport);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotTeleport of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bCannotTeleport
static int tolua_set_GRID_DATA_bCannotTeleport(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotTeleport'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotTeleport = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bRoleCannotLoseItem of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bRoleCannotLoseItem
static int tolua_get_GRID_DATA_bRoleCannotLoseItem(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bRoleCannotLoseItem'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bRoleCannotLoseItem);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bRoleCannotLoseItem of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bRoleCannotLoseItem
static int tolua_set_GRID_DATA_bRoleCannotLoseItem(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bRoleCannotLoseItem'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bRoleCannotLoseItem = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bRoleCannotDropItem of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bRoleCannotDropItem
static int tolua_get_GRID_DATA_bRoleCannotDropItem(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bRoleCannotDropItem'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bRoleCannotDropItem);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bRoleCannotDropItem of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bRoleCannotDropItem
static int tolua_set_GRID_DATA_bRoleCannotDropItem(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bRoleCannotDropItem'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bRoleCannotDropItem = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bSlayerArea of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bSlayerArea
static int tolua_get_GRID_DATA_bSlayerArea(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bSlayerArea'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bSlayerArea);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bSlayerArea of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bSlayerArea
static int tolua_set_GRID_DATA_bSlayerArea(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bSlayerArea'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bSlayerArea = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bCannotDropItem of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bCannotDropItem
static int tolua_get_GRID_DATA_bCannotDropItem(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotDropItem'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bCannotDropItem);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bCannotDropItem of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bCannotDropItem
static int tolua_set_GRID_DATA_bCannotDropItem(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bCannotDropItem'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bCannotDropItem = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uMineLevel of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uMineLevel
static int tolua_get_GRID_DATA_uMineLevel(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uMineLevel'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uMineLevel);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uMineLevel of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uMineLevel
static int tolua_set_GRID_DATA_uMineLevel(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uMineLevel'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uMineLevel = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bTeleportPoint of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bTeleportPoint
static int tolua_get_GRID_DATA_bTeleportPoint(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bTeleportPoint'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bTeleportPoint);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bTeleportPoint of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bTeleportPoint
static int tolua_set_GRID_DATA_bTeleportPoint(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bTeleportPoint'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bTeleportPoint = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uReserved1 of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uReserved1
static int tolua_get_GRID_DATA_uReserved1(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uReserved1'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uReserved1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uReserved1 of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uReserved1
static int tolua_set_GRID_DATA_uReserved1(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uReserved1'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uReserved1 = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uReserved2 of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uReserved2
static int tolua_get_GRID_DATA_uReserved2(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uReserved2'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uReserved2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uReserved2 of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uReserved2
static int tolua_set_GRID_DATA_uReserved2(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uReserved2'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uReserved2 = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uLowLayer of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uLowLayer
static int tolua_get_GRID_DATA_uLowLayer(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uLowLayer'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uLowLayer);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uLowLayer of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uLowLayer
static int tolua_set_GRID_DATA_uLowLayer(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uLowLayer'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uLowLayer = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uHighLayer of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uHighLayer
static int tolua_get_GRID_DATA_uHighLayer(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uHighLayer'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uHighLayer);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uHighLayer of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uHighLayer
static int tolua_set_GRID_DATA_uHighLayer(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uHighLayer'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uHighLayer = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uEdge of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uEdge
static int tolua_get_GRID_DATA_uEdge(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uEdge'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uEdge);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uEdge of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uEdge
static int tolua_set_GRID_DATA_uEdge(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uEdge'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uEdge = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bOutOfPoly of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bOutOfPoly
static int tolua_get_GRID_DATA_bOutOfPoly(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bOutOfPoly'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bOutOfPoly);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bOutOfPoly of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bOutOfPoly
static int tolua_set_GRID_DATA_bOutOfPoly(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bOutOfPoly'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bOutOfPoly = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: bInServerPointRange of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_bInServerPointRange
static int tolua_get_GRID_DATA_bInServerPointRange(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInServerPointRange'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bInServerPointRange);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bInServerPointRange of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_bInServerPointRange
static int tolua_set_GRID_DATA_bInServerPointRange(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bInServerPointRange'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bInServerPointRange = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uReserved3 of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_DATA_uReserved3
static int tolua_get_GRID_DATA_uReserved3(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uReserved3'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uReserved3);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uReserved3 of class  GRID_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_DATA_uReserved3
static int tolua_set_GRID_DATA_uReserved3(lua_State* tolua_S)
{
  GRID_DATA* self = (GRID_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uReserved3'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uReserved3 = ((unsigned)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uCharRef of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_INSTANCE_DATA_unsigned_uCharRef
static int tolua_get_GRID_INSTANCE_DATA_unsigned_uCharRef(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uCharRef'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uCharRef);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uCharRef of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_INSTANCE_DATA_unsigned_uCharRef
static int tolua_set_GRID_INSTANCE_DATA_unsigned_uCharRef(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uCharRef'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uCharRef = ((unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uDropRef of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_INSTANCE_DATA_unsigned_uDropRef
static int tolua_get_GRID_INSTANCE_DATA_unsigned_uDropRef(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uDropRef'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uDropRef);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uDropRef of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_INSTANCE_DATA_unsigned_uDropRef
static int tolua_set_GRID_INSTANCE_DATA_unsigned_uDropRef(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uDropRef'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uDropRef = ((unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uMissileTag1 of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_INSTANCE_DATA_unsigned_uMissileTag1
static int tolua_get_GRID_INSTANCE_DATA_unsigned_uMissileTag1(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uMissileTag1'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uMissileTag1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uMissileTag1 of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_INSTANCE_DATA_unsigned_uMissileTag1
static int tolua_set_GRID_INSTANCE_DATA_unsigned_uMissileTag1(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uMissileTag1'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uMissileTag1 = ((unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uMissileTag2 of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_INSTANCE_DATA_unsigned_uMissileTag2
static int tolua_get_GRID_INSTANCE_DATA_unsigned_uMissileTag2(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uMissileTag2'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uMissileTag2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uMissileTag2 of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_INSTANCE_DATA_unsigned_uMissileTag2
static int tolua_set_GRID_INSTANCE_DATA_unsigned_uMissileTag2(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uMissileTag2'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uMissileTag2 = ((unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: uReserved of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_GRID_INSTANCE_DATA_unsigned_uReserved
static int tolua_get_GRID_INSTANCE_DATA_unsigned_uReserved(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uReserved'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->uReserved);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: uReserved of class  GRID_INSTANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_GRID_INSTANCE_DATA_unsigned_uReserved
static int tolua_set_GRID_INSTANCE_DATA_unsigned_uReserved(lua_State* tolua_S)
{
  GRID_INSTANCE_DATA* self = (GRID_INSTANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'uReserved'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->uReserved = ((unsigned short)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSceneTempID of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_get_ABSOLUTE_SCENE_POS_nSceneTempID
static int tolua_get_ABSOLUTE_SCENE_POS_nSceneTempID(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSceneTempID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSceneTempID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSceneTempID of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_set_ABSOLUTE_SCENE_POS_nSceneTempID
static int tolua_set_ABSOLUTE_SCENE_POS_nSceneTempID(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSceneTempID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSceneTempID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSceneID of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_get_ABSOLUTE_SCENE_POS_nSceneID
static int tolua_get_ABSOLUTE_SCENE_POS_nSceneID(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSceneID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSceneID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSceneID of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_set_ABSOLUTE_SCENE_POS_nSceneID
static int tolua_set_ABSOLUTE_SCENE_POS_nSceneID(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
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

/* get function: nX of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_get_ABSOLUTE_SCENE_POS_nX
static int tolua_get_ABSOLUTE_SCENE_POS_nX(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nX'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nX);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nX of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_set_ABSOLUTE_SCENE_POS_nX
static int tolua_set_ABSOLUTE_SCENE_POS_nX(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nX'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nX = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nY of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_get_ABSOLUTE_SCENE_POS_nY
static int tolua_get_ABSOLUTE_SCENE_POS_nY(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nY'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nY);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nY of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_set_ABSOLUTE_SCENE_POS_nY
static int tolua_set_ABSOLUTE_SCENE_POS_nY(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nY'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nY = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nZ of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_get_ABSOLUTE_SCENE_POS_nZ
static int tolua_get_ABSOLUTE_SCENE_POS_nZ(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nZ'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nZ);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nZ of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_set_ABSOLUTE_SCENE_POS_nZ
static int tolua_set_ABSOLUTE_SCENE_POS_nZ(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nZ'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nZ = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nFaceDirection of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_get_ABSOLUTE_SCENE_POS_nFaceDirection
static int tolua_get_ABSOLUTE_SCENE_POS_nFaceDirection(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFaceDirection'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nFaceDirection);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nFaceDirection of class  ABSOLUTE_SCENE_POS */
#ifndef TOLUA_DISABLE_tolua_set_ABSOLUTE_SCENE_POS_nFaceDirection
static int tolua_set_ABSOLUTE_SCENE_POS_nFaceDirection(lua_State* tolua_S)
{
  ABSOLUTE_SCENE_POS* self = (ABSOLUTE_SCENE_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nFaceDirection'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nFaceDirection = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSceneTempID of class  ENTER_POS */
#ifndef TOLUA_DISABLE_tolua_get_ENTER_POS_nSceneTempID
static int tolua_get_ENTER_POS_nSceneTempID(lua_State* tolua_S)
{
  ENTER_POS* self = (ENTER_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSceneTempID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSceneTempID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSceneTempID of class  ENTER_POS */
#ifndef TOLUA_DISABLE_tolua_set_ENTER_POS_nSceneTempID
static int tolua_set_ENTER_POS_nSceneTempID(lua_State* tolua_S)
{
  ENTER_POS* self = (ENTER_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSceneTempID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nSceneTempID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nSceneID of class  ENTER_POS */
#ifndef TOLUA_DISABLE_tolua_get_ENTER_POS_nSceneID
static int tolua_get_ENTER_POS_nSceneID(lua_State* tolua_S)
{
  ENTER_POS* self = (ENTER_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nSceneID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nSceneID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nSceneID of class  ENTER_POS */
#ifndef TOLUA_DISABLE_tolua_set_ENTER_POS_nSceneID
static int tolua_set_ENTER_POS_nSceneID(lua_State* tolua_S)
{
  ENTER_POS* self = (ENTER_POS*)  tolua_tousertype(tolua_S,1,0);
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

/* get function: bySide of class  ENTER_POS */
#ifndef TOLUA_DISABLE_tolua_get_ENTER_POS_unsigned_bySide
static int tolua_get_ENTER_POS_unsigned_bySide(lua_State* tolua_S)
{
  ENTER_POS* self = (ENTER_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bySide'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->bySide);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: bySide of class  ENTER_POS */
#ifndef TOLUA_DISABLE_tolua_set_ENTER_POS_unsigned_bySide
static int tolua_set_ENTER_POS_unsigned_bySide(lua_State* tolua_S)
{
  ENTER_POS* self = (ENTER_POS*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'bySide'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->bySide = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* function: I64_2_Double */
#ifndef TOLUA_DISABLE_tolua_enum_I64_2_Double00
static int tolua_enum_I64_2_Double00(lua_State* tolua_S)
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
  signed int llPos = (( signed int)  tolua_tonumber(tolua_S,1,0));
 {
  double tolua_ret = (double)  I64_2_Double(llPos);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'I64_2_Double'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ConvertCoordS2N */
#ifndef TOLUA_DISABLE_tolua_enum_ConvertCoordS2N00
static int tolua_enum_ConvertCoordS2N00(lua_State* tolua_S)
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
  signed int llCoordination = (( signed int)  tolua_tonumber(tolua_S,1,0));
 {
  unsigned short tolua_ret = ( unsigned short)  ConvertCoordS2N(llCoordination);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ConvertCoordS2N'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ConvertCoordN2S */
#ifndef TOLUA_DISABLE_tolua_enum_ConvertCoordN2S00
static int tolua_enum_ConvertCoordN2S00(lua_State* tolua_S)
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
  unsigned short wCoordination = (( unsigned short)  tolua_tonumber(tolua_S,1,0));
 {
  signed int tolua_ret = ( signed int)  ConvertCoordN2S(wCoordination);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ConvertCoordN2S'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ConvertCoordO2N */
#ifndef TOLUA_DISABLE_tolua_enum_ConvertCoordO2N00
static int tolua_enum_ConvertCoordO2N00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnX = (( signed int)  tolua_tonumber(tolua_S,1,0));
  signed int rnY = (( signed int)  tolua_tonumber(tolua_S,2,0));
  signed int rnZ = (( signed int)  tolua_tonumber(tolua_S,3,0));
 {
  ConvertCoordO2N(rnX,rnY,rnZ);
 tolua_pushnumber(tolua_S,(lua_Number)rnX);
 tolua_pushnumber(tolua_S,(lua_Number)rnY);
 tolua_pushnumber(tolua_S,(lua_Number)rnZ);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ConvertCoordO2N'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ConvertCoordN2O */
#ifndef TOLUA_DISABLE_tolua_enum_ConvertCoordN2O00
static int tolua_enum_ConvertCoordN2O00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int rnX = (( signed int)  tolua_tonumber(tolua_S,1,0));
  signed int rnY = (( signed int)  tolua_tonumber(tolua_S,2,0));
  signed int rnZ = (( signed int)  tolua_tonumber(tolua_S,3,0));
 {
  ConvertCoordN2O(rnX,rnY,rnZ);
 tolua_pushnumber(tolua_S,(lua_Number)rnX);
 tolua_pushnumber(tolua_S,(lua_Number)rnY);
 tolua_pushnumber(tolua_S,(lua_Number)rnZ);
 }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ConvertCoordN2O'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: MakeSpellKey */
#ifndef TOLUA_DISABLE_tolua_enum_MakeSpellKey00
static int tolua_enum_MakeSpellKey00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int nID = (( signed int)  tolua_tonumber(tolua_S,1,0));
  signed int nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0));
  signed int nRuneID = (( signed int)  tolua_tonumber(tolua_S,3,0));
 {
  signed int tolua_ret = ( signed int)  MakeSpellKey(nID,nLevel,nRuneID);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MakeSpellKey'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: MakeSpellProgressKey */
#ifndef TOLUA_DISABLE_tolua_enum_MakeSpellProgressKey00
static int tolua_enum_MakeSpellProgressKey00(lua_State* tolua_S)
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
  signed int nID = (( signed int)  tolua_tonumber(tolua_S,1,0));
  signed int nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0));
 {
  signed int tolua_ret = ( signed int)  MakeSpellProgressKey(nID,nLevel);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MakeSpellProgressKey'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: MakeAuraKey */
#ifndef TOLUA_DISABLE_tolua_enum_MakeAuraKey00
static int tolua_enum_MakeAuraKey00(lua_State* tolua_S)
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
  signed int nID = (( signed int)  tolua_tonumber(tolua_S,1,0));
  signed int nLevel = (( signed int)  tolua_tonumber(tolua_S,2,0));
 {
  signed int tolua_ret = ( signed int)  MakeAuraKey(nID,nLevel);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MakeAuraKey'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: MakeAuraInstanceKey */
#ifndef TOLUA_DISABLE_tolua_enum_MakeAuraInstanceKey00
static int tolua_enum_MakeAuraInstanceKey00(lua_State* tolua_S)
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
  unsigned int nType = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int dwGenerator = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
 {
  unsigned int tolua_ret = ( unsigned int)  MakeAuraInstanceKey(nType,dwGenerator);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MakeAuraInstanceKey'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GetAuraTypeFromInstanceID */
#ifndef TOLUA_DISABLE_tolua_enum_GetAuraTypeFromInstanceID00
static int tolua_enum_GetAuraTypeFromInstanceID00(lua_State* tolua_S)
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
  unsigned int dwAuraInstanceID = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
 {
  signed int tolua_ret = ( signed int)  GetAuraTypeFromInstanceID(dwAuraInstanceID);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAuraTypeFromInstanceID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: MakeCoolDownID */
#ifndef TOLUA_DISABLE_tolua_enum_MakeCoolDownID00
static int tolua_enum_MakeCoolDownID00(lua_State* tolua_S)
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
  signed int nID = (( signed int)  tolua_tonumber(tolua_S,2,0));
 {
  signed int tolua_ret = ( signed int)  MakeCoolDownID(nType,nID);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MakeCoolDownID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: ParseCoolDownID */
#ifndef TOLUA_DISABLE_tolua_enum_ParseCoolDownID00
static int tolua_enum_ParseCoolDownID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  signed int ID = (( signed int)  tolua_tonumber(tolua_S,1,0));
  signed int nType = (( signed int)  tolua_tonumber(tolua_S,2,0));
  signed int nID = (( signed int)  tolua_tonumber(tolua_S,3,0));
 {
  ParseCoolDownID(ID,nType,nID);
 tolua_pushnumber(tolua_S,(lua_Number)nType);
 tolua_pushnumber(tolua_S,(lua_Number)nID);
 }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ParseCoolDownID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: IsAppearance */
#ifndef TOLUA_DISABLE_tolua_enum_IsAppearance00
static int tolua_enum_IsAppearance00(lua_State* tolua_S)
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
  unsigned short conslot = (( unsigned short)  tolua_tonumber(tolua_S,1,0));
  unsigned short itemslot = (( unsigned short)  tolua_tonumber(tolua_S,2,0));
 {
 int tolua_ret = (int)
IsAppearance(conslot,itemslot);
 tolua_pushboolean(tolua_S,(int)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsAppearance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nDropGroupID of class  DROP_CHANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DROP_CHANCE_DATA_nDropGroupID
static int tolua_get_DROP_CHANCE_DATA_nDropGroupID(lua_State* tolua_S)
{
  DROP_CHANCE_DATA* self = (DROP_CHANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nDropGroupID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nDropGroupID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nDropGroupID of class  DROP_CHANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DROP_CHANCE_DATA_nDropGroupID
static int tolua_set_DROP_CHANCE_DATA_nDropGroupID(lua_State* tolua_S)
{
  DROP_CHANCE_DATA* self = (DROP_CHANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nDropGroupID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nDropGroupID = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nDropChance of class  DROP_CHANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_DROP_CHANCE_DATA_nDropChance
static int tolua_get_DROP_CHANCE_DATA_nDropChance(lua_State* tolua_S)
{
  DROP_CHANCE_DATA* self = (DROP_CHANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nDropChance'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->nDropChance);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nDropChance of class  DROP_CHANCE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_DROP_CHANCE_DATA_nDropChance
static int tolua_set_DROP_CHANCE_DATA_nDropChance(lua_State* tolua_S)
{
  DROP_CHANCE_DATA* self = (DROP_CHANCE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nDropChance'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nDropChance = (( signed int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: byQueue of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEUE_DATA_unsigned_byQueue
static int tolua_get_QUEUE_DATA_unsigned_byQueue(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'byQueue'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->byQueue);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: byQueue of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEUE_DATA_unsigned_byQueue
static int tolua_set_QUEUE_DATA_unsigned_byQueue(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'byQueue'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->byQueue = (( unsigned char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwJoinConfirmTimeOut of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEUE_DATA_unsigned_dwJoinConfirmTimeOut
static int tolua_get_QUEUE_DATA_unsigned_dwJoinConfirmTimeOut(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwJoinConfirmTimeOut'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwJoinConfirmTimeOut);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwJoinConfirmTimeOut of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEUE_DATA_unsigned_dwJoinConfirmTimeOut
static int tolua_set_QUEUE_DATA_unsigned_dwJoinConfirmTimeOut(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwJoinConfirmTimeOut'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwJoinConfirmTimeOut = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwJoinSceneTempID of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEUE_DATA_unsigned_dwJoinSceneTempID
static int tolua_get_QUEUE_DATA_unsigned_dwJoinSceneTempID(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwJoinSceneTempID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwJoinSceneTempID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwJoinSceneTempID of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEUE_DATA_unsigned_dwJoinSceneTempID
static int tolua_set_QUEUE_DATA_unsigned_dwJoinSceneTempID(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwJoinSceneTempID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwJoinSceneTempID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwJoinSceneID of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEUE_DATA_unsigned_dwJoinSceneID
static int tolua_get_QUEUE_DATA_unsigned_dwJoinSceneID(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwJoinSceneID'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwJoinSceneID);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwJoinSceneID of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEUE_DATA_unsigned_dwJoinSceneID
static int tolua_set_QUEUE_DATA_unsigned_dwJoinSceneID(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwJoinSceneID'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwJoinSceneID = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dwJoinSide of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_get_QUEUE_DATA_unsigned_dwJoinSide
static int tolua_get_QUEUE_DATA_unsigned_dwJoinSide(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwJoinSide'",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->dwJoinSide);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dwJoinSide of class  QUEUE_DATA */
#ifndef TOLUA_DISABLE_tolua_set_QUEUE_DATA_unsigned_dwJoinSide
static int tolua_set_QUEUE_DATA_unsigned_dwJoinSide(lua_State* tolua_S)
{
  QUEUE_DATA* self = (QUEUE_DATA*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dwJoinSide'",NULL);
 if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dwJoinSide = (( unsigned int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* function: MakeVariableID */
#ifndef TOLUA_DISABLE_tolua_enum_MakeVariableID00
static int tolua_enum_MakeVariableID00(lua_State* tolua_S)
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
  unsigned int dwType = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int dwIndex = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
 {
  unsigned int tolua_ret = ( unsigned int)  MakeVariableID(dwType,dwIndex);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MakeVariableID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GetVarTypeFromID */
#ifndef TOLUA_DISABLE_tolua_enum_GetVarTypeFromID00
static int tolua_enum_GetVarTypeFromID00(lua_State* tolua_S)
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
  unsigned int dwVarID = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
 {
  signed int tolua_ret = ( signed int)  GetVarTypeFromID(dwVarID);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetVarTypeFromID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GetVarIndexFromID */
#ifndef TOLUA_DISABLE_tolua_enum_GetVarIndexFromID00
static int tolua_enum_GetVarIndexFromID00(lua_State* tolua_S)
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
  unsigned int dwVarID = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
 {
  signed int tolua_ret = ( signed int)  GetVarIndexFromID(dwVarID);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetVarIndexFromID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: MakeServerKey */
#ifndef TOLUA_DISABLE_tolua_enum_MakeServerKey00
static int tolua_enum_MakeServerKey00(lua_State* tolua_S)
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
  unsigned int type = (( unsigned int)  tolua_tonumber(tolua_S,1,0));
  unsigned int id = (( unsigned int)  tolua_tonumber(tolua_S,2,0));
 {
  unsigned int tolua_ret = ( unsigned int)  MakeServerKey(type,id);
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MakeServerKey'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_enum_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_constant(tolua_S,"errUnknownError",errUnknownError);
  tolua_constant(tolua_S,"errSuccess",errSuccess);
  tolua_constant(tolua_S,"errCritical",errCritical);
  tolua_constant(tolua_S,"errDatabase",errDatabase);
  tolua_constant(tolua_S,"errLoginBegin",errLoginBegin);
  tolua_constant(tolua_S,"errLoginAuthFailed",errLoginAuthFailed);
  tolua_constant(tolua_S,"errLoginUnknownError",errLoginUnknownError);
  tolua_constant(tolua_S,"errLoginServerNotReady",errLoginServerNotReady);
  tolua_constant(tolua_S,"errLoginRepeat",errLoginRepeat);
  tolua_constant(tolua_S,"errLoginRepeatKicked",errLoginRepeatKicked);
  tolua_constant(tolua_S,"errLoginErrorUserNameOrPassword",errLoginErrorUserNameOrPassword);
  tolua_constant(tolua_S,"errLoginTooManyOnlineRole",errLoginTooManyOnlineRole);
  tolua_constant(tolua_S,"errLoginLocked",errLoginLocked);
  tolua_constant(tolua_S,"errLoginPrecreate",errLoginPrecreate);
  tolua_constant(tolua_S,"errLoginErrorVersion",errLoginErrorVersion);
  tolua_constant(tolua_S,"errLoginCreateRoleTooMany",errLoginCreateRoleTooMany);
  tolua_constant(tolua_S,"errLoginCreateRoleDupeName",errLoginCreateRoleDupeName);
  tolua_constant(tolua_S,"errLoginCreateRoleCensoredWord",errLoginCreateRoleCensoredWord);
  tolua_constant(tolua_S,"errLoginCreateRoleNameLength",errLoginCreateRoleNameLength);
  tolua_constant(tolua_S,"errLoginCreateRoleNameServerNotConnected",errLoginCreateRoleNameServerNotConnected);
  tolua_constant(tolua_S,"errLoginCreateRoleNsDupeName",errLoginCreateRoleNsDupeName);
  tolua_constant(tolua_S,"errLoginCreateRoleNsDbSysErr",errLoginCreateRoleNsDbSysErr);
  tolua_constant(tolua_S,"errLoginDeleteRoleNotFound",errLoginDeleteRoleNotFound);
  tolua_constant(tolua_S,"errLoginDeleteRoleDeleted",errLoginDeleteRoleDeleted);
  tolua_constant(tolua_S,"errLoginDeleteRoleMaster",errLoginDeleteRoleMaster);
  tolua_constant(tolua_S,"errLoginDeleteRoleGuild",errLoginDeleteRoleGuild);
  tolua_constant(tolua_S,"errLoginDeleteRoleAuction",errLoginDeleteRoleAuction);
  tolua_constant(tolua_S,"errLoginDeleteRoleExchange",errLoginDeleteRoleExchange);
  tolua_constant(tolua_S,"errLoginUnDeleteRoleNotFound",errLoginUnDeleteRoleNotFound);
  tolua_constant(tolua_S,"errLoginRoleBusy",errLoginRoleBusy);
  tolua_constant(tolua_S,"errLoginRoleFrozen",errLoginRoleFrozen);
  tolua_constant(tolua_S,"errLoginTimeUnknown",errLoginTimeUnknown);
  tolua_constant(tolua_S,"errLogoutNotPermit",errLogoutNotPermit);
  tolua_constant(tolua_S,"errLoginTooManyUser",errLoginTooManyUser);
  tolua_constant(tolua_S,"errLoginWrongServer",errLoginWrongServer);
  tolua_constant(tolua_S,"errLoginCreateRoleForeverDelete",errLoginCreateRoleForeverDelete);
  tolua_constant(tolua_S,"errLoginNeedDynamicPassword",errLoginNeedDynamicPassword);
  tolua_constant(tolua_S,"errLoginDynamicPasswordSuccess",errLoginDynamicPasswordSuccess);
  tolua_constant(tolua_S,"errLoginDynamicPasswordFailed",errLoginDynamicPasswordFailed);
  tolua_constant(tolua_S,"errLoginDynamicPasswordVerified",errLoginDynamicPasswordVerified);
  tolua_constant(tolua_S,"errLoginDynamicPasswordTooOften",errLoginDynamicPasswordTooOften);
  tolua_constant(tolua_S,"errLoginEnd",errLoginEnd);
  tolua_constant(tolua_S,"errMoveBegin",errMoveBegin);
  tolua_constant(tolua_S,"errMoveFail",errMoveFail);
  tolua_constant(tolua_S,"errMineFail",errMineFail);
  tolua_constant(tolua_S,"errMoveEnd",errMoveEnd);
  tolua_constant(tolua_S,"errJumpBegin",errJumpBegin);
  tolua_constant(tolua_S,"errJumpWrongState",errJumpWrongState);
  tolua_constant(tolua_S,"errJumpCasting",errJumpCasting);
  tolua_constant(tolua_S,"errJumpInCombat",errJumpInCombat);
  tolua_constant(tolua_S,"errJumpStallOpen",errJumpStallOpen);
  tolua_constant(tolua_S,"errJumpTrading",errJumpTrading);
  tolua_constant(tolua_S,"errJumpWrongItem",errJumpWrongItem);
  tolua_constant(tolua_S,"errJumpWrongPos",errJumpWrongPos);
  tolua_constant(tolua_S,"errJumpSceneFobidden",errJumpSceneFobidden);
  tolua_constant(tolua_S,"errJumpDead",errJumpDead);
  tolua_constant(tolua_S,"errJumpArrest",errJumpArrest);
  tolua_constant(tolua_S,"errJumpChangeLineBreak",errJumpChangeLineBreak);
  tolua_constant(tolua_S,"errJumpEnd",errJumpEnd);
  tolua_constant(tolua_S,"errReviveBegin",errReviveBegin);
  tolua_constant(tolua_S,"errReviveStillAlive",errReviveStillAlive);
  tolua_constant(tolua_S,"errReviveLackOfItem",errReviveLackOfItem);
  tolua_constant(tolua_S,"errReviveInProgress",errReviveInProgress);
  tolua_constant(tolua_S,"errReviveNoGraveyard",errReviveNoGraveyard);
  tolua_constant(tolua_S,"errReviveForbidden",errReviveForbidden);
  tolua_constant(tolua_S,"errReviveNotReady",errReviveNotReady);
  tolua_constant(tolua_S,"errReviveUnknown",errReviveUnknown);
  tolua_constant(tolua_S,"errReviveEnd",errReviveEnd);
  tolua_constant(tolua_S,"errSpellBegin",errSpellBegin);
  tolua_constant(tolua_S,"errSpellCasting",errSpellCasting);
  tolua_constant(tolua_S,"errSpellDisabled",errSpellDisabled);
  tolua_constant(tolua_S,"errSpellWeaponTypeError",errSpellWeaponTypeError);
  tolua_constant(tolua_S,"errSpellNotExists",errSpellNotExists);
  tolua_constant(tolua_S,"errSpellNoTarget",errSpellNoTarget);
  tolua_constant(tolua_S,"errSpellErrorType",errSpellErrorType);
  tolua_constant(tolua_S,"errSpellConditionError",errSpellConditionError);
  tolua_constant(tolua_S,"errSpellCasterCCLimitBitsError",errSpellCasterCCLimitBitsError);
  tolua_constant(tolua_S,"errSpellCastNoMove",errSpellCastNoMove);
  tolua_constant(tolua_S,"errSpellCastNoMount",errSpellCastNoMount);
  tolua_constant(tolua_S,"errSpellCastNoDead",errSpellCastNoDead);
  tolua_constant(tolua_S,"errSpellCastNoLive",errSpellCastNoLive);
  tolua_constant(tolua_S,"errSpellCastNoStiff",errSpellCastNoStiff);
  tolua_constant(tolua_S,"errSpellCasterCheckNodeFailed",errSpellCasterCheckNodeFailed);
  tolua_constant(tolua_S,"errSpellInValidTargetRoleState",errSpellInValidTargetRoleState);
  tolua_constant(tolua_S,"errSpellInValidTargetNodeError",errSpellInValidTargetNodeError);
  tolua_constant(tolua_S,"errSpellInValidTargetDead",errSpellInValidTargetDead);
  tolua_constant(tolua_S,"errSpellInValidTargetStance",errSpellInValidTargetStance);
  tolua_constant(tolua_S,"errSpellInValidTargetRelationship",errSpellInValidTargetRelationship);
  tolua_constant(tolua_S,"errSpellInValidTargetObjType",errSpellInValidTargetObjType);
  tolua_constant(tolua_S,"errSpellInValidTargetNeverBeHit",errSpellInValidTargetNeverBeHit);
  tolua_constant(tolua_S,"errSpellInvalidDirection",errSpellInvalidDirection);
  tolua_constant(tolua_S,"errSpellOutOfRange",errSpellOutOfRange);
  tolua_constant(tolua_S,"errSpellObstacled",errSpellObstacled);
  tolua_constant(tolua_S,"errSpellOutOfMana",errSpellOutOfMana);
  tolua_constant(tolua_S,"errSpellNotReady",errSpellNotReady);
  tolua_constant(tolua_S,"errSpellInHardTime",errSpellInHardTime);
  tolua_constant(tolua_S,"errSpellLackOfItem",errSpellLackOfItem);
  tolua_constant(tolua_S,"errSpellEquipDisabled",errSpellEquipDisabled);
  tolua_constant(tolua_S,"errSpellCannotBeEquipped",errSpellCannotBeEquipped);
  tolua_constant(tolua_S,"errSpellEquipBindSlot",errSpellEquipBindSlot);
  tolua_constant(tolua_S,"errSpellEquipPassiveSlotError",errSpellEquipPassiveSlotError);
  tolua_constant(tolua_S,"errSpellSlotError",errSpellSlotError);
  tolua_constant(tolua_S,"errSpellSlotLocked",errSpellSlotLocked);
  tolua_constant(tolua_S,"errSpellSlotNoDuplicate",errSpellSlotNoDuplicate);
  tolua_constant(tolua_S,"errSpellLearningClassError",errSpellLearningClassError);
  tolua_constant(tolua_S,"errSpellLearningLevelError",errSpellLearningLevelError);
  tolua_constant(tolua_S,"errSpellLearnt",errSpellLearnt);
  tolua_constant(tolua_S,"errRuneEquipError",errRuneEquipError);
  tolua_constant(tolua_S,"errRuneEquipClassError",errRuneEquipClassError);
  tolua_constant(tolua_S,"errRuneNotExists",errRuneNotExists);
  tolua_constant(tolua_S,"errRuneNotActivated",errRuneNotActivated);
  tolua_constant(tolua_S,"errPracticeOutOfPts",errPracticeOutOfPts);
  tolua_constant(tolua_S,"errPracticeInProgress",errPracticeInProgress);
  tolua_constant(tolua_S,"errPracticeLevelTooLow",errPracticeLevelTooLow);
  tolua_constant(tolua_S,"errPracticeAlreadyMax",errPracticeAlreadyMax);
  tolua_constant(tolua_S,"errSpellEquipNotReady",errSpellEquipNotReady);
  tolua_constant(tolua_S,"errSpellSceneLimit",errSpellSceneLimit);
  tolua_constant(tolua_S,"errSpellUnknown",errSpellUnknown);
  tolua_constant(tolua_S,"errSpellEnd",errSpellEnd);
  tolua_constant(tolua_S,"errAuraBegin",errAuraBegin);
  tolua_constant(tolua_S,"errAuraCannotCancel",errAuraCannotCancel);
  tolua_constant(tolua_S,"errAuraResisted",errAuraResisted);
  tolua_constant(tolua_S,"errAuraNotExist",errAuraNotExist);
  tolua_constant(tolua_S,"errAuraConfigError",errAuraConfigError);
  tolua_constant(tolua_S,"errAuraAppendFailed",errAuraAppendFailed);
  tolua_constant(tolua_S,"errAuraEnd",errAuraEnd);
  tolua_constant(tolua_S,"errItemBegin",errItemBegin);
  tolua_constant(tolua_S,"errItemBagFull",errItemBagFull);
  tolua_constant(tolua_S,"errItemInvalidPos",errItemInvalidPos);
  tolua_constant(tolua_S,"errItemTypeMismatch",errItemTypeMismatch);
  tolua_constant(tolua_S,"errItemEquipMismatch",errItemEquipMismatch);
  tolua_constant(tolua_S,"errItemUnsatisfied",errItemUnsatisfied);
  tolua_constant(tolua_S,"errItemCannotDestroy",errItemCannotDestroy);
  tolua_constant(tolua_S,"errItemDropTooMany",errItemDropTooMany);
  tolua_constant(tolua_S,"errItemCannotUnEquip",errItemCannotUnEquip);
  tolua_constant(tolua_S,"errItemCannotSell",errItemCannotSell);
  tolua_constant(tolua_S,"errItemNotFound",errItemNotFound);
  tolua_constant(tolua_S,"errItemNotEnough",errItemNotEnough);
  tolua_constant(tolua_S,"errItemBinded",errItemBinded);
  tolua_constant(tolua_S,"errItemCannotSplit",errItemCannotSplit);
  tolua_constant(tolua_S,"errItemCannotUse",errItemCannotUse);
  tolua_constant(tolua_S,"errItemFull",errItemFull);
  tolua_constant(tolua_S,"errItemNotFull",errItemNotFull);
  tolua_constant(tolua_S,"errItemExpBottleMaxTimes",errItemExpBottleMaxTimes);
  tolua_constant(tolua_S,"errItemWrongCombatState",errItemWrongCombatState);
  tolua_constant(tolua_S,"errItemCannotUseInArena",errItemCannotUseInArena);
  tolua_constant(tolua_S,"errItemNoNeedRepair",errItemNoNeedRepair);
  tolua_constant(tolua_S,"errItemCannotRepair",errItemCannotRepair);
  tolua_constant(tolua_S,"errItemMaxRefineLevel",errItemMaxRefineLevel);
  tolua_constant(tolua_S,"errItemRefineError",errItemRefineError);
  tolua_constant(tolua_S,"errItemStall",errItemStall);
  tolua_constant(tolua_S,"errItemTrading",errItemTrading);
  tolua_constant(tolua_S,"errItemNotEnoughExp",errItemNotEnoughExp);
  tolua_constant(tolua_S,"errItemNotEnoughMoneytype01",errItemNotEnoughMoneytype01);
  tolua_constant(tolua_S,"errItemNotEnoughMoneytype02",errItemNotEnoughMoneytype02);
  tolua_constant(tolua_S,"errItemNotEnoughMoney",errItemNotEnoughMoney);
  tolua_constant(tolua_S,"errItemNotEnoughReputation",errItemNotEnoughReputation);
  tolua_constant(tolua_S,"errItemNotEnoughBuildingLevel",errItemNotEnoughBuildingLevel);
  tolua_constant(tolua_S,"errItemNotReady",errItemNotReady);
  tolua_constant(tolua_S,"errItemScriptUseFail",errItemScriptUseFail);
  tolua_constant(tolua_S,"errItemCannotFindScript",errItemCannotFindScript);
  tolua_constant(tolua_S,"errItemCannotUpgradeSoul",errItemCannotUpgradeSoul);
  tolua_constant(tolua_S,"errItemUpgradeSoulLimit",errItemUpgradeSoulLimit);
  tolua_constant(tolua_S,"errItemCannotAddSoul",errItemCannotAddSoul);
  tolua_constant(tolua_S,"errItemAlreadySocketed",errItemAlreadySocketed);
  tolua_constant(tolua_S,"errItemSocketNotMatch",errItemSocketNotMatch);
  tolua_constant(tolua_S,"errItemNotSocketed",errItemNotSocketed);
  tolua_constant(tolua_S,"errItemRuneError",errItemRuneError);
  tolua_constant(tolua_S,"errItemRuneNotEnoughMaterial",errItemRuneNotEnoughMaterial);
  tolua_constant(tolua_S,"errItemComposeError",errItemComposeError);
  tolua_constant(tolua_S,"errItemRoleDead",errItemRoleDead);
  tolua_constant(tolua_S,"errItemRoleCombat",errItemRoleCombat);
  tolua_constant(tolua_S,"errItemEquipped",errItemEquipped);
  tolua_constant(tolua_S,"errItemDailyMoneyExceeded",errItemDailyMoneyExceeded);
  tolua_constant(tolua_S,"errItemWeekMoneyExceeded",errItemWeekMoneyExceeded);
  tolua_constant(tolua_S,"errItemTotalMoneyExceeded",errItemTotalMoneyExceeded);
  tolua_constant(tolua_S,"errItemMaxLuck",errItemMaxLuck);
  tolua_constant(tolua_S,"errItemNotIdentified",errItemNotIdentified);
  tolua_constant(tolua_S,"errItemAlreadyIdentified",errItemAlreadyIdentified);
  tolua_constant(tolua_S,"errItemCannotSign",errItemCannotSign);
  tolua_constant(tolua_S,"errItemNotSigned",errItemNotSigned);
  tolua_constant(tolua_S,"errItemAlreadySigned",errItemAlreadySigned);
  tolua_constant(tolua_S,"errItemOthersSigned",errItemOthersSigned);
  tolua_constant(tolua_S,"errItemSelfSigned",errItemSelfSigned);
  tolua_constant(tolua_S,"errItemCannotAuction",errItemCannotAuction);
  tolua_constant(tolua_S,"errItemNotEnoughPkValue",errItemNotEnoughPkValue);
  tolua_constant(tolua_S,"errItemCannotLevelup",errItemCannotLevelup);
  tolua_constant(tolua_S,"errItemLevelUpInProcess",errItemLevelUpInProcess);
  tolua_constant(tolua_S,"errItemLevelUpNotFinished",errItemLevelUpNotFinished);
  tolua_constant(tolua_S,"errItemDurLow",errItemDurLow);
  tolua_constant(tolua_S,"errItemWeaponNotTried",errItemWeaponNotTried);
  tolua_constant(tolua_S,"errItemNotEnoughBlackIron",errItemNotEnoughBlackIron);
  tolua_constant(tolua_S,"errItemLevelupError",errItemLevelupError);
  tolua_constant(tolua_S,"errItemNoLevelUp",errItemNoLevelUp);
  tolua_constant(tolua_S,"errItemWeaponTooHeavy",errItemWeaponTooHeavy);
  tolua_constant(tolua_S,"errItemArmorTooHeavy",errItemArmorTooHeavy);
  tolua_constant(tolua_S,"errItemBagTooHeavy",errItemBagTooHeavy);
  tolua_constant(tolua_S,"errItemOpenPackageEarly",errItemOpenPackageEarly);
  tolua_constant(tolua_S,"errItemOpenPackageTooMany",errItemOpenPackageTooMany);
  tolua_constant(tolua_S,"errItemRoleNotFound",errItemRoleNotFound);
  tolua_constant(tolua_S,"errItemNotSabacGuild",errItemNotSabacGuild);
  tolua_constant(tolua_S,"errItemBlackMarketItemNotFound",errItemBlackMarketItemNotFound);
  tolua_constant(tolua_S,"errItemBlackMarketNotFound",errItemBlackMarketNotFound);
  tolua_constant(tolua_S,"errItemBlackMarketSold",errItemBlackMarketSold);
  tolua_constant(tolua_S,"errItemBlackMarketCooldown",errItemBlackMarketCooldown);
  tolua_constant(tolua_S,"errItemMaterialWeaopn",errItemMaterialWeaopn);
  tolua_constant(tolua_S,"errItemNoExerciseTime",errItemNoExerciseTime);
  tolua_constant(tolua_S,"errItemExerciseTimeNotEmpty",errItemExerciseTimeNotEmpty);
  tolua_constant(tolua_S,"errItemCannotDecompose",errItemCannotDecompose);
  tolua_constant(tolua_S,"errItemUnknown",errItemUnknown);
  tolua_constant(tolua_S,"errItemWeaponLevelUpTakeoutImmediatelyTooLate",errItemWeaponLevelUpTakeoutImmediatelyTooLate);
  tolua_constant(tolua_S,"errItemTooLowLevel",errItemTooLowLevel);
  tolua_constant(tolua_S,"errItemNoConsumeUse",errItemNoConsumeUse);
  tolua_constant(tolua_S,"errItemNotDoubleRune",errItemNotDoubleRune);
  tolua_constant(tolua_S,"errItemNewbieRune",errItemNewbieRune);
  tolua_constant(tolua_S,"errItemEnd",errItemEnd);
  tolua_constant(tolua_S,"errMountBegin",errMountBegin);
  tolua_constant(tolua_S,"errMountOutOfItem",errMountOutOfItem);
  tolua_constant(tolua_S,"errMountAlreadyExists",errMountAlreadyExists);
  tolua_constant(tolua_S,"errMountLevelLimited",errMountLevelLimited);
  tolua_constant(tolua_S,"errMountNotExist",errMountNotExist);
  tolua_constant(tolua_S,"errMountAuraError",errMountAuraError);
  tolua_constant(tolua_S,"errMountEnd",errMountEnd);
  tolua_constant(tolua_S,"errDropBegin",errDropBegin);
  tolua_constant(tolua_S,"errDropNotFound",errDropNotFound);
  tolua_constant(tolua_S,"errDropFaraway",errDropFaraway);
  tolua_constant(tolua_S,"errDropProtected",errDropProtected);
  tolua_constant(tolua_S,"errDropOtherStand",errDropOtherStand);
  tolua_constant(tolua_S,"errDropFast",errDropFast);
  tolua_constant(tolua_S,"errDropBindied",errDropBindied);
  tolua_constant(tolua_S,"errDropIsBidding",errDropIsBidding);
  tolua_constant(tolua_S,"errDropFcm",errDropFcm);
  tolua_constant(tolua_S,"errDropEnd",errDropEnd);
  tolua_constant(tolua_S,"errShopBegin",errShopBegin);
  tolua_constant(tolua_S,"errNotEnoughMoney",errNotEnoughMoney);
  tolua_constant(tolua_S,"errShopNotFound",errShopNotFound);
  tolua_constant(tolua_S,"errShopItemNotFound",errShopItemNotFound);
  tolua_constant(tolua_S,"errShopTooMany",errShopTooMany);
  tolua_constant(tolua_S,"errShopTypeSell",errShopTypeSell);
  tolua_constant(tolua_S,"errVipShopNotFound",errVipShopNotFound);
  tolua_constant(tolua_S,"errShopEnd",errShopEnd);
  tolua_constant(tolua_S,"errStallBegin",errStallBegin);
  tolua_constant(tolua_S,"errStallWrongState",errStallWrongState);
  tolua_constant(tolua_S,"errStallForbid",errStallForbid);
  tolua_constant(tolua_S,"errStallOthersOpen",errStallOthersOpen);
  tolua_constant(tolua_S,"errStallWrongMoveState",errStallWrongMoveState);
  tolua_constant(tolua_S,"errStallTrading",errStallTrading);
  tolua_constant(tolua_S,"errStallInvalidSlot",errStallInvalidSlot);
  tolua_constant(tolua_S,"errStallSlotExist",errStallSlotExist);
  tolua_constant(tolua_S,"errStallSlotNotExist",errStallSlotNotExist);
  tolua_constant(tolua_S,"errStallAlreadyInStall",errStallAlreadyInStall);
  tolua_constant(tolua_S,"errStallItemCountError",errStallItemCountError);
  tolua_constant(tolua_S,"errStallMoneyExceeded",errStallMoneyExceeded);
  tolua_constant(tolua_S,"errStallNotFound",errStallNotFound);
  tolua_constant(tolua_S,"errStallFaraway",errStallFaraway);
  tolua_constant(tolua_S,"errStallNotEnoughItems",errStallNotEnoughItems);
  tolua_constant(tolua_S,"errStallItemExpired",errStallItemExpired);
  tolua_constant(tolua_S,"errStallEnd",errStallEnd);
  tolua_constant(tolua_S,"errQuestBegin",errQuestBegin);
  tolua_constant(tolua_S,"errQuestAlreadyAccepted",errQuestAlreadyAccepted);
  tolua_constant(tolua_S,"errQuestAlreadyCompleted",errQuestAlreadyCompleted);
  tolua_constant(tolua_S,"errQuestLogFull",errQuestLogFull);
  tolua_constant(tolua_S,"errQuestNotCompletePreQuest",errQuestNotCompletePreQuest);
  tolua_constant(tolua_S,"errQuestTalkNpcTooFar",errQuestTalkNpcTooFar);
  tolua_constant(tolua_S,"errQuestTalkNotCompleted",errQuestTalkNotCompleted);
  tolua_constant(tolua_S,"errQuestNpcTooFar",errQuestNpcTooFar);
  tolua_constant(tolua_S,"errQuestSex",errQuestSex);
  tolua_constant(tolua_S,"errQuestClass",errQuestClass);
  tolua_constant(tolua_S,"errQuestBuffNotFound",errQuestBuffNotFound);
  tolua_constant(tolua_S,"errQuestLowReputation",errQuestLowReputation);
  tolua_constant(tolua_S,"errQuestHighReputation",errQuestHighReputation);
  tolua_constant(tolua_S,"errQuestTime",errQuestTime);
  tolua_constant(tolua_S,"errQuestLevel",errQuestLevel);
  tolua_constant(tolua_S,"errQuestNotEnoughItem",errQuestNotEnoughItem);
  tolua_constant(tolua_S,"errQuestLimitVar",errQuestLimitVar);
  tolua_constant(tolua_S,"errQuestNotEnoughMoney",errQuestNotEnoughMoney);
  tolua_constant(tolua_S,"errQuestMaxCompleted",errQuestMaxCompleted);
  tolua_constant(tolua_S,"errQuestNotEnoughKill",errQuestNotEnoughKill);
  tolua_constant(tolua_S,"errQuestTooMuchMoney",errQuestTooMuchMoney);
  tolua_constant(tolua_S,"errQuestCanNotAbandon",errQuestCanNotAbandon);
  tolua_constant(tolua_S,"errQuestShareUnavailable",errQuestShareUnavailable);
  tolua_constant(tolua_S,"errQuestNoShareTeam",errQuestNoShareTeam);
  tolua_constant(tolua_S,"errQuestNoSharer",errQuestNoSharer);
  tolua_constant(tolua_S,"errQuestShareNotSameScene",errQuestShareNotSameScene);
  tolua_constant(tolua_S,"errQuestCannotShare",errQuestCannotShare);
  tolua_constant(tolua_S,"errQuestCannotFindScript",errQuestCannotFindScript);
  tolua_constant(tolua_S,"errQuestScriptUseFail",errQuestScriptUseFail);
  tolua_constant(tolua_S,"errQuestItemBagFull",errQuestItemBagFull);
  tolua_constant(tolua_S,"errQuestTimeLimit",errQuestTimeLimit);
  tolua_constant(tolua_S,"errQuestZone",errQuestZone);
  tolua_constant(tolua_S,"errQuestGrid",errQuestGrid);
  tolua_constant(tolua_S,"errQuestExplore",errQuestExplore);
  tolua_constant(tolua_S,"errQuestVarIndex",errQuestVarIndex);
  tolua_constant(tolua_S,"errQuestSelectItemClass",errQuestSelectItemClass);
  tolua_constant(tolua_S,"errQuestCheckLevel",errQuestCheckLevel);
  tolua_constant(tolua_S,"errQuestDailyCompleteFull",errQuestDailyCompleteFull);
  tolua_constant(tolua_S,"errQuestDailyBrushCDTime",errQuestDailyBrushCDTime);
  tolua_constant(tolua_S,"errQuestDailyCompleteTimeOut",errQuestDailyCompleteTimeOut);
  tolua_constant(tolua_S,"errQuestCircleDailyComplete",errQuestCircleDailyComplete);
  tolua_constant(tolua_S,"errQuestRewardQuestComplete",errQuestRewardQuestComplete);
  tolua_constant(tolua_S,"errQuestRewardBoughtTimeUp",errQuestRewardBoughtTimeUp);
  tolua_constant(tolua_S,"errQuestRecycle",errQuestRecycle);
  tolua_constant(tolua_S,"errQuestNoMaster",errQuestNoMaster);
  tolua_constant(tolua_S,"errQuestTeamMaster",errQuestTeamMaster);
  tolua_constant(tolua_S,"errQuestCoolDown",errQuestCoolDown);
  tolua_constant(tolua_S,"errQuestKillPlayer",errQuestKillPlayer);
  tolua_constant(tolua_S,"errQuestInCombat",errQuestInCombat);
  tolua_constant(tolua_S,"errQuestKillBufPlayer",errQuestKillBufPlayer);
  tolua_constant(tolua_S,"errQuestDoodad",errQuestDoodad);
  tolua_constant(tolua_S,"errQuestCheckItem",errQuestCheckItem);
  tolua_constant(tolua_S,"errAcceptQuestScriptUseFail",errAcceptQuestScriptUseFail);
  tolua_constant(tolua_S,"errCompleteQuestScriptUseFail",errCompleteQuestScriptUseFail);
  tolua_constant(tolua_S,"errQuestUnknownError",errQuestUnknownError);
  tolua_constant(tolua_S,"errQuestEnd",errQuestEnd);
  tolua_constant(tolua_S,"errNpcBegin",errNpcBegin);
  tolua_constant(tolua_S,"errNpcNotFound",errNpcNotFound);
  tolua_constant(tolua_S,"errNpcTooFar",errNpcTooFar);
  tolua_constant(tolua_S,"errNpcOpponentTarget",errNpcOpponentTarget);
  tolua_constant(tolua_S,"errNpcDataNil",errNpcDataNil);
  tolua_constant(tolua_S,"errNpcDialogTimeout",errNpcDialogTimeout);
  tolua_constant(tolua_S,"errNpcNoSuchFunction",errNpcNoSuchFunction);
  tolua_constant(tolua_S,"errNpcInCombat",errNpcInCombat);
  tolua_constant(tolua_S,"errNpcEnd",errNpcEnd);
  tolua_constant(tolua_S,"errDoodadBegin",errDoodadBegin);
  tolua_constant(tolua_S,"errDoodadNotExist",errDoodadNotExist);
  tolua_constant(tolua_S,"errDoodadOutOfRange",errDoodadOutOfRange);
  tolua_constant(tolua_S,"errDoodadCannotOperate",errDoodadCannotOperate);
  tolua_constant(tolua_S,"errDoodadSpellCasting",errDoodadSpellCasting);
  tolua_constant(tolua_S,"errDoodadEnd",errDoodadEnd);
  tolua_constant(tolua_S,"errTeamBegin",errTeamBegin);
  tolua_constant(tolua_S,"errTeamInvitationSent",errTeamInvitationSent);
  tolua_constant(tolua_S,"errTeamApplicationSent",errTeamApplicationSent);
  tolua_constant(tolua_S,"errTeamFailed",errTeamFailed);
  tolua_constant(tolua_S,"errTeamOffLine",errTeamOffLine);
  tolua_constant(tolua_S,"errTeamInBattleQueue",errTeamInBattleQueue);
  tolua_constant(tolua_S,"errTeamInArenaQueue",errTeamInArenaQueue);
  tolua_constant(tolua_S,"errTeamAlreadyInTeam",errTeamAlreadyInTeam);
  tolua_constant(tolua_S,"errTeamFull",errTeamFull);
  tolua_constant(tolua_S,"errTeamRefuse",errTeamRefuse);
  tolua_constant(tolua_S,"errTeamNotLeader",errTeamNotLeader);
  tolua_constant(tolua_S,"errTeamNameError",errTeamNameError);
  tolua_constant(tolua_S,"errTeamNotPermit",errTeamNotPermit);
  tolua_constant(tolua_S,"errTeamNotInTeam",errTeamNotInTeam);
  tolua_constant(tolua_S,"errTeamNotExist",errTeamNotExist);
  tolua_constant(tolua_S,"errTeamRefusedInvitation",errTeamRefusedInvitation);
  tolua_constant(tolua_S,"errTeamInvitationRefused",errTeamInvitationRefused);
  tolua_constant(tolua_S,"errTeamRefusedApplication",errTeamRefusedApplication);
  tolua_constant(tolua_S,"errTeamApplicationRefused",errTeamApplicationRefused);
  tolua_constant(tolua_S,"errTeamAssignedTypeError",errTeamAssignedTypeError);
  tolua_constant(tolua_S,"errTeamRequestNotAvailable",errTeamRequestNotAvailable);
  tolua_constant(tolua_S,"errTeamAuctionBidTooLow",errTeamAuctionBidTooLow);
  tolua_constant(tolua_S,"errTeamAuctionNotEnoughMoney",errTeamAuctionNotEnoughMoney);
  tolua_constant(tolua_S,"errTeamAuctionNotBegin",errTeamAuctionNotBegin);
  tolua_constant(tolua_S,"errAdjacentDataTimeLimit",errAdjacentDataTimeLimit);
  tolua_constant(tolua_S,"errTeamNoKickInAuction",errTeamNoKickInAuction);
  tolua_constant(tolua_S,"errTeamSceneForbidden",errTeamSceneForbidden);
  tolua_constant(tolua_S,"errTeamEnd",errTeamEnd);
  tolua_constant(tolua_S,"errArenaBegin",errArenaBegin);
  tolua_constant(tolua_S,"errArenaGetRewardSucc",errArenaGetRewardSucc);
  tolua_constant(tolua_S,"errArenaNotInRewardTime",errArenaNotInRewardTime);
  tolua_constant(tolua_S,"errArenaNoReward",errArenaNoReward);
  tolua_constant(tolua_S,"errArenaNotLucky",errArenaNotLucky);
  tolua_constant(tolua_S,"errArenaCannotUseItem",errArenaCannotUseItem);
  tolua_constant(tolua_S,"errArenaUseItemLimit",errArenaUseItemLimit);
  tolua_constant(tolua_S,"errArenaMatchTimeout",errArenaMatchTimeout);
  tolua_constant(tolua_S,"errArenaLucky",errArenaLucky);
  tolua_constant(tolua_S,"errArenaMatchFailed",errArenaMatchFailed);
  tolua_constant(tolua_S,"errArenaEnd",errArenaEnd);
  tolua_constant(tolua_S,"errBattleBegin",errBattleBegin);
  tolua_constant(tolua_S,"errBattleQueueNow",errBattleQueueNow);
  tolua_constant(tolua_S,"errBattleNotQueue",errBattleNotQueue);
  tolua_constant(tolua_S,"errBattleConfirmTimeout",errBattleConfirmTimeout);
  tolua_constant(tolua_S,"errBattleEnd",errBattleEnd);
  tolua_constant(tolua_S,"errSceneBegin",errSceneBegin);
  tolua_constant(tolua_S,"errSceneTooFarFromTeleport",errSceneTooFarFromTeleport);
  tolua_constant(tolua_S,"errSceneTooFarFromTeleportPlatform",errSceneTooFarFromTeleportPlatform);
  tolua_constant(tolua_S,"errSceneNotTeamLeader",errSceneNotTeamLeader);
  tolua_constant(tolua_S,"errSceneNotTeamMember",errSceneNotTeamMember);
  tolua_constant(tolua_S,"errSceneErrorDungeonLevel",errSceneErrorDungeonLevel);
  tolua_constant(tolua_S,"errSceneErrorDungeonDifficulty",errSceneErrorDungeonDifficulty);
  tolua_constant(tolua_S,"errSceneNoEnterRight",errSceneNoEnterRight);
  tolua_constant(tolua_S,"errSceneNoEnterMaxRole",errSceneNoEnterMaxRole);
  tolua_constant(tolua_S,"errSceneNotExist",errSceneNotExist);
  tolua_constant(tolua_S,"errSceneCreateSceneFailed",errSceneCreateSceneFailed);
  tolua_constant(tolua_S,"errSceneDestroySceneFailed",errSceneDestroySceneFailed);
  tolua_constant(tolua_S,"errScenePreEnterFailed",errScenePreEnterFailed);
  tolua_constant(tolua_S,"errSceneNoFatigue",errSceneNoFatigue);
  tolua_constant(tolua_S,"errSceneEnterCountLimit",errSceneEnterCountLimit);
  tolua_constant(tolua_S,"errSceneNoLevel",errSceneNoLevel);
  tolua_constant(tolua_S,"errSceneTooLowLevel",errSceneTooLowLevel);
  tolua_constant(tolua_S,"errSceneTooHighLevel",errSceneTooHighLevel);
  tolua_constant(tolua_S,"errSceneEnd",errSceneEnd);
  tolua_constant(tolua_S,"errChatBegin",errChatBegin);
  tolua_constant(tolua_S,"errChatCountOut",errChatCountOut);
  tolua_constant(tolua_S,"errChatTooFrequent",errChatTooFrequent);
  tolua_constant(tolua_S,"errChatServiceUnavaible",errChatServiceUnavaible);
  tolua_constant(tolua_S,"errChatTargetNotFound",errChatTargetNotFound);
  tolua_constant(tolua_S,"errGroupChatWithItem",errGroupChatWithItem);
  tolua_constant(tolua_S,"errChatForbidden",errChatForbidden);
  tolua_constant(tolua_S,"errChatSceneForbidden",errChatSceneForbidden);
  tolua_constant(tolua_S,"errChatLevelTooLow",errChatLevelTooLow);
  tolua_constant(tolua_S,"errChatMoneyNotEnough",errChatMoneyNotEnough);
  tolua_constant(tolua_S,"errChatSafeAreaForbidden",errChatSafeAreaForbidden);
  tolua_constant(tolua_S,"errChatEnd",errChatEnd);
  tolua_constant(tolua_S,"errFriendBegin",errFriendBegin);
  tolua_constant(tolua_S,"errFriendNotFound",errFriendNotFound);
  tolua_constant(tolua_S,"errFriendAlreadyExists",errFriendAlreadyExists);
  tolua_constant(tolua_S,"errFriendNotMutual",errFriendNotMutual);
  tolua_constant(tolua_S,"errFriendNotOnline",errFriendNotOnline);
  tolua_constant(tolua_S,"errFriendFull",errFriendFull);
  tolua_constant(tolua_S,"errEnemyNotFind",errEnemyNotFind);
  tolua_constant(tolua_S,"errFriendEnd",errFriendEnd);
  tolua_constant(tolua_S,"errTitleBegin",errTitleBegin);
  tolua_constant(tolua_S,"errTitleNotFind",errTitleNotFind);
  tolua_constant(tolua_S,"errTitleCondNotSatisfy",errTitleCondNotSatisfy);
  tolua_constant(tolua_S,"errTitleEnd",errTitleEnd);
  tolua_constant(tolua_S,"errTradeBegin",errTradeBegin);
  tolua_constant(tolua_S,"errTradeAskSent",errTradeAskSent);
  tolua_constant(tolua_S,"errTradeWrongState",errTradeWrongState);
  tolua_constant(tolua_S,"errTradeTargetNotFound",errTradeTargetNotFound);
  tolua_constant(tolua_S,"errTradeTargetFaraway",errTradeTargetFaraway);
  tolua_constant(tolua_S,"errTradeTargetBusy",errTradeTargetBusy);
  tolua_constant(tolua_S,"errTradeDifferentGroup",errTradeDifferentGroup);
  tolua_constant(tolua_S,"errTradeBagFull",errTradeBagFull);
  tolua_constant(tolua_S,"errTradeUnknown",errTradeUnknown);
  tolua_constant(tolua_S,"errTradeSceneFobidden",errTradeSceneFobidden);
  tolua_constant(tolua_S,"errTradeChangeNotAllowed",errTradeChangeNotAllowed);
  tolua_constant(tolua_S,"errTradeNeedIntimacy",errTradeNeedIntimacy);
  tolua_constant(tolua_S,"errTradeOverWeight",errTradeOverWeight);
  tolua_constant(tolua_S,"errTradeEnd",errTradeEnd);
  tolua_constant(tolua_S,"errMasterBegin",errMasterBegin);
  tolua_constant(tolua_S,"errMstFailed",errMstFailed);
  tolua_constant(tolua_S,"errMasterNotMaster",errMasterNotMaster);
  tolua_constant(tolua_S,"errMasterStudFull",errMasterStudFull);
  tolua_constant(tolua_S,"errMasterTeacherNotOnline",errMasterTeacherNotOnline);
  tolua_constant(tolua_S,"errMasterStudentNotOnline",errMasterStudentNotOnline);
  tolua_constant(tolua_S,"errMasterStudHighLevel",errMasterStudHighLevel);
  tolua_constant(tolua_S,"errMasterStudhasMasterAlready",errMasterStudhasMasterAlready);
  tolua_constant(tolua_S,"errMasterStudAppFull",errMasterStudAppFull);
  tolua_constant(tolua_S,"errMasterTargetStudAppFull",errMasterTargetStudAppFull);
  tolua_constant(tolua_S,"errMasterNoStudApp",errMasterNoStudApp);
  tolua_constant(tolua_S,"errMasterNoTeacherApp",errMasterNoTeacherApp);
  tolua_constant(tolua_S,"errMstApplyFull",errMstApplyFull);
  tolua_constant(tolua_S,"errMstApplyerFull",errMstApplyerFull);
  tolua_constant(tolua_S,"errMasterAlreadyApply",errMasterAlreadyApply);
  tolua_constant(tolua_S,"errMstRefuseApply",errMstRefuseApply);
  tolua_constant(tolua_S,"errMasterStudNotOnline",errMasterStudNotOnline);
  tolua_constant(tolua_S,"errMasterApplayMstCD",errMasterApplayMstCD);
  tolua_constant(tolua_S,"errMasterFireStudentCD",errMasterFireStudentCD);
  tolua_constant(tolua_S,"errMasterTargetFireStudCD",errMasterTargetFireStudCD);
  tolua_constant(tolua_S,"errMasterNoMaster",errMasterNoMaster);
  tolua_constant(tolua_S,"errMasterAlreadyGrad",errMasterAlreadyGrad);
  tolua_constant(tolua_S,"errMstCannotCZ",errMstCannotCZ);
  tolua_constant(tolua_S,"errMstHaveNoStu",errMstHaveNoStu);
  tolua_constant(tolua_S,"errMstMissApplication",errMstMissApplication);
  tolua_constant(tolua_S,"errMasterRoleNotExist",errMasterRoleNotExist);
  tolua_constant(tolua_S,"errMasterAddExpFailed",errMasterAddExpFailed);
  tolua_constant(tolua_S,"errMasterHighLevel",errMasterHighLevel);
  tolua_constant(tolua_S,"errMasterOpFast",errMasterOpFast);
  tolua_constant(tolua_S,"errMasterNotCZLevel",errMasterNotCZLevel);
  tolua_constant(tolua_S,"errMasterNeedGetRewardExp",errMasterNeedGetRewardExp);
  tolua_constant(tolua_S,"errMasterEnd",errMasterEnd);
  tolua_constant(tolua_S,"errMailBegin",errMailBegin);
  tolua_constant(tolua_S,"errMailUnknown",errMailUnknown);
  tolua_constant(tolua_S,"errMailReceiverNotFound",errMailReceiverNotFound);
  tolua_constant(tolua_S,"errMailReceiverFull",errMailReceiverFull);
  tolua_constant(tolua_S,"errMailNotFound",errMailNotFound);
  tolua_constant(tolua_S,"errMailCostNotEnough",errMailCostNotEnough);
  tolua_constant(tolua_S,"errMailAttachNotFound",errMailAttachNotFound);
  tolua_constant(tolua_S,"errMailSendTooFast",errMailSendTooFast);
  tolua_constant(tolua_S,"errMailAttachCDTime",errMailAttachCDTime);
  tolua_constant(tolua_S,"errMailSceneForbidden",errMailSceneForbidden);
  tolua_constant(tolua_S,"errMailEnd",errMailEnd);
  tolua_constant(tolua_S,"errAchievementBegin",errAchievementBegin);
  tolua_constant(tolua_S,"errAchievementLevel",errAchievementLevel);
  tolua_constant(tolua_S,"errAchievementExperience",errAchievementExperience);
  tolua_constant(tolua_S,"errAchievementPkValue",errAchievementPkValue);
  tolua_constant(tolua_S,"errAchievementForceReputation",errAchievementForceReputation);
  tolua_constant(tolua_S,"errAchievementVariousForceCount",errAchievementVariousForceCount);
  tolua_constant(tolua_S,"errAchievementEquip",errAchievementEquip);
  tolua_constant(tolua_S,"errAchievementEquipColor",errAchievementEquipColor);
  tolua_constant(tolua_S,"errAchievementEquipBodyColor",errAchievementEquipBodyColor);
  tolua_constant(tolua_S,"errAchievementSet",errAchievementSet);
  tolua_constant(tolua_S,"errAchievementSetCount",errAchievementSetCount);
  tolua_constant(tolua_S,"errAchievementEquipSet",errAchievementEquipSet);
  tolua_constant(tolua_S,"errAchievementBankSlotCount",errAchievementBankSlotCount);
  tolua_constant(tolua_S,"errAchievementTotalSlotCount",errAchievementTotalSlotCount);
  tolua_constant(tolua_S,"errAchievementItemBagSlotCount",errAchievementItemBagSlotCount);
  tolua_constant(tolua_S,"errAchievementEquipBagSlotCount",errAchievementEquipBagSlotCount);
  tolua_constant(tolua_S,"errAchievementMaterialBagSlotCount",errAchievementMaterialBagSlotCount);
  tolua_constant(tolua_S,"errAchievementTotalMoneyType",errAchievementTotalMoneyType);
  tolua_constant(tolua_S,"errAchievementKillPlayerTotalCount",errAchievementKillPlayerTotalCount);
  tolua_constant(tolua_S,"errAchievementEnterScene",errAchievementEnterScene);
  tolua_constant(tolua_S,"errAchievementCompleteQuest",errAchievementCompleteQuest);
  tolua_constant(tolua_S,"errAchievementCompleteQuestCount",errAchievementCompleteQuestCount);
  tolua_constant(tolua_S,"errAchievementCompleteTypeQuest",errAchievementCompleteTypeQuest);
  tolua_constant(tolua_S,"errAchievementSpellLevelSpecified",errAchievementSpellLevelSpecified);
  tolua_constant(tolua_S,"errAchievementSpellLevel",errAchievementSpellLevel);
  tolua_constant(tolua_S,"errAchievementLearnSpell",errAchievementLearnSpell);
  tolua_constant(tolua_S,"errAchievementSpellCount",errAchievementSpellCount);
  tolua_constant(tolua_S,"errAchievementActiveSpellCount",errAchievementActiveSpellCount);
  tolua_constant(tolua_S,"errAchievementRuneCount",errAchievementRuneCount);
  tolua_constant(tolua_S,"errAchievementEquipPassiveSpellCount",errAchievementEquipPassiveSpellCount);
  tolua_constant(tolua_S,"errAchievementEquipPassiveSpellIDCount",errAchievementEquipPassiveSpellIDCount);
  tolua_constant(tolua_S,"errAchievementAchievement",errAchievementAchievement);
  tolua_constant(tolua_S,"errAchievementAchievementCount",errAchievementAchievementCount);
  tolua_constant(tolua_S,"errAchievementGetItem",errAchievementGetItem);
  tolua_constant(tolua_S,"errAchievementItemLuck",errAchievementItemLuck);
  tolua_constant(tolua_S,"errAchievementItemRefine",errAchievementItemRefine);
  tolua_constant(tolua_S,"errAchievementTitle",errAchievementTitle);
  tolua_constant(tolua_S,"errAchievementTitleCount",errAchievementTitleCount);
  tolua_constant(tolua_S,"errAchievementFriendCount",errAchievementFriendCount);
  tolua_constant(tolua_S,"errAchievementLeaveStudentCount",errAchievementLeaveStudentCount);
  tolua_constant(tolua_S,"errAchievementLeaveMaster",errAchievementLeaveMaster);
  tolua_constant(tolua_S,"errAchievementCreateTime",errAchievementCreateTime);
  tolua_constant(tolua_S,"errAchievementDailyCompleteQuestCount",errAchievementDailyCompleteQuestCount);
  tolua_constant(tolua_S,"errAchievementMainCompleteQuestCount",errAchievementMainCompleteQuestCount);
  tolua_constant(tolua_S,"errAchievementSubCompleteQuestCount",errAchievementSubCompleteQuestCount);
  tolua_constant(tolua_S,"errAchievementHiddenCompleteQuestCount",errAchievementHiddenCompleteQuestCount);
  tolua_constant(tolua_S,"errAchievementUseTypeItemCount",errAchievementUseTypeItemCount);
  tolua_constant(tolua_S,"errAchievementUseItem101Count",errAchievementUseItem101Count);
  tolua_constant(tolua_S,"errAchievementKillWithNoGuilty",errAchievementKillWithNoGuilty);
  tolua_constant(tolua_S,"errAchievementVariableCheckError",errAchievementVariableCheckError);
  tolua_constant(tolua_S,"errAchievementSpiritLevel",errAchievementSpiritLevel);
  tolua_constant(tolua_S,"errAchievementPlayerNotFound",errAchievementPlayerNotFound);
  tolua_constant(tolua_S,"errAchievementPlayerNotInSameScene",errAchievementPlayerNotInSameScene);
  tolua_constant(tolua_S,"errAchievementPlayerTooFar",errAchievementPlayerTooFar);
  tolua_constant(tolua_S,"errAchievementRewarded",errAchievementRewarded);
  tolua_constant(tolua_S,"errAchievementNotCompleted",errAchievementNotCompleted);
  tolua_constant(tolua_S,"errAchievementAlreadyCompleted",errAchievementAlreadyCompleted);
  tolua_constant(tolua_S,"errAchievementMount",errAchievementMount);
  tolua_constant(tolua_S,"errAchievementHero",errAchievementHero);
  tolua_constant(tolua_S,"errAchievementRewardBagFull",errAchievementRewardBagFull);
  tolua_constant(tolua_S,"errAchievementSpiritBuyAirBagSlotCount",errAchievementSpiritBuyAirBagSlotCount);
  tolua_constant(tolua_S,"errAchievementSpiritBuyAirStallSlotCount",errAchievementSpiritBuyAirStallSlotCount);
  tolua_constant(tolua_S,"errAchievementSpiritBuyAirEquipSlotCount",errAchievementSpiritBuyAirEquipSlotCount);
  tolua_constant(tolua_S,"errAchievementSpiritBloodPageCount",errAchievementSpiritBloodPageCount);
  tolua_constant(tolua_S,"errAchievementSpiritBloodPageCompleteCount",errAchievementSpiritBloodPageCompleteCount);
  tolua_constant(tolua_S,"errAchievementSpiritBloodPageLevel",errAchievementSpiritBloodPageLevel);
  tolua_constant(tolua_S,"errAchievementTotalRefineTime",errAchievementTotalRefineTime);
  tolua_constant(tolua_S,"errAchievementTeamAuctionCount",errAchievementTeamAuctionCount);
  tolua_constant(tolua_S,"errAchievementVip",errAchievementVip);
  tolua_constant(tolua_S,"errAchievementColorItemComposeCount",errAchievementColorItemComposeCount);
  tolua_constant(tolua_S,"errAchievementItemComposeCount",errAchievementItemComposeCount);
  tolua_constant(tolua_S,"errAchievementColorItemDeComposeCount",errAchievementColorItemDeComposeCount);
  tolua_constant(tolua_S,"errAchievementItemDeComposeCount",errAchievementItemDeComposeCount);
  tolua_constant(tolua_S,"errAchievementRuneWeaponCount",errAchievementRuneWeaponCount);
  tolua_constant(tolua_S,"errAchievementExplore",errAchievementExplore);
  tolua_constant(tolua_S,"errAchievementEnd",errAchievementEnd);
  tolua_constant(tolua_S,"errGuildBegin",errGuildBegin);
  tolua_constant(tolua_S,"errGuildFailed",errGuildFailed);
  tolua_constant(tolua_S,"errGuildInternalError",errGuildInternalError);
  tolua_constant(tolua_S,"errGuildMsgError",errGuildMsgError);
  tolua_constant(tolua_S,"errGuildSqlError",errGuildSqlError);
  tolua_constant(tolua_S,"errGuildClientParamError",errGuildClientParamError);
  tolua_constant(tolua_S,"errGuildNumLimit",errGuildNumLimit);
  tolua_constant(tolua_S,"errGuildQueryFailed",errGuildQueryFailed);
  tolua_constant(tolua_S,"errGuildCreateFailed",errGuildCreateFailed);
  tolua_constant(tolua_S,"errGuildSendTVMsgError",errGuildSendTVMsgError);
  tolua_constant(tolua_S,"errGuildSendMailFailed",errGuildSendMailFailed);
  tolua_constant(tolua_S,"errGuildViewGuildFailed",errGuildViewGuildFailed);
  tolua_constant(tolua_S,"errGuildNoGuild",errGuildNoGuild);
  tolua_constant(tolua_S,"errGuildNotFind",errGuildNotFind);
  tolua_constant(tolua_S,"errGuildLeaderNotExist",errGuildLeaderNotExist);
  tolua_constant(tolua_S,"errGuildNotMember",errGuildNotMember);
  tolua_constant(tolua_S,"errGuildCannotFindMember",errGuildCannotFindMember);
  tolua_constant(tolua_S,"errGuildAddMemberFailed",errGuildAddMemberFailed);
  tolua_constant(tolua_S,"errGuildAddEventFailed",errGuildAddEventFailed);
  tolua_constant(tolua_S,"errGuildAddChestEventFailed",errGuildAddChestEventFailed);
  tolua_constant(tolua_S,"errGuildDeleteGuildAppFailed",errGuildDeleteGuildAppFailed);
  tolua_constant(tolua_S,"errGuildDestoryFailed",errGuildDestoryFailed);
  tolua_constant(tolua_S,"errGuildNoticeSizeError",errGuildNoticeSizeError);
  tolua_constant(tolua_S,"errGuildAnnounceSizeError",errGuildAnnounceSizeError);
  tolua_constant(tolua_S,"errGuildSetChatFlagError",errGuildSetChatFlagError);
  tolua_constant(tolua_S,"errGuildCannotSetSelfPosition",errGuildCannotSetSelfPosition);
  tolua_constant(tolua_S,"errGuildSetPositionFailed",errGuildSetPositionFailed);
  tolua_constant(tolua_S,"errGuildSubRoleMoneyFailed",errGuildSubRoleMoneyFailed);
  tolua_constant(tolua_S,"errGuildSubMoneyFailed",errGuildSubMoneyFailed);
  tolua_constant(tolua_S,"errGuildAddMoneyFailed",errGuildAddMoneyFailed);
  tolua_constant(tolua_S,"errGuildDonateLimit",errGuildDonateLimit);
  tolua_constant(tolua_S,"errGuildAddWoodFailed",errGuildAddWoodFailed);
  tolua_constant(tolua_S,"errGuildAddHerbFailed",errGuildAddHerbFailed);
  tolua_constant(tolua_S,"errGuildAddCrystalFailed",errGuildAddCrystalFailed);
  tolua_constant(tolua_S,"errGuildAddSilverFailed",errGuildAddSilverFailed);
  tolua_constant(tolua_S,"errGuildSubResFailed",errGuildSubResFailed);
  tolua_constant(tolua_S,"errGuildAddAllianceAppFailed",errGuildAddAllianceAppFailed);
  tolua_constant(tolua_S,"errGuildAddEnemyFailed",errGuildAddEnemyFailed);
  tolua_constant(tolua_S,"errGuildCannotAddYourself",errGuildCannotAddYourself);
  tolua_constant(tolua_S,"errGuildAllianceAppNotExist",errGuildAllianceAppNotExist);
  tolua_constant(tolua_S,"errGuildAllianceAppTimeTypeError",errGuildAllianceAppTimeTypeError);
  tolua_constant(tolua_S,"errGuildSameName",errGuildSameName);
  tolua_constant(tolua_S,"errGuildCensoredWord",errGuildCensoredWord);
  tolua_constant(tolua_S,"errGuildLowerLevel",errGuildLowerLevel);
  tolua_constant(tolua_S,"errGuildNotEnoughMoney",errGuildNotEnoughMoney);
  tolua_constant(tolua_S,"errGuildNotEnoughContribution",errGuildNotEnoughContribution);
  tolua_constant(tolua_S,"errGuildNoItem",errGuildNoItem);
  tolua_constant(tolua_S,"errGuildApplyGuildFailed",errGuildApplyGuildFailed);
  tolua_constant(tolua_S,"errGuildPositionError",errGuildPositionError);
  tolua_constant(tolua_S,"errGuildAppFull",errGuildAppFull);
  tolua_constant(tolua_S,"errGuildCannotDemiseMyself",errGuildCannotDemiseMyself);
  tolua_constant(tolua_S,"errGuildAlreadyJoin",errGuildAlreadyJoin);
  tolua_constant(tolua_S,"errGuildAlreadyApply",errGuildAlreadyApply);
  tolua_constant(tolua_S,"errGuildNoRight",errGuildNoRight);
  tolua_constant(tolua_S,"errGuildMemberCountFull",errGuildMemberCountFull);
  tolua_constant(tolua_S,"errGuildTargetNotOnline",errGuildTargetNotOnline);
  tolua_constant(tolua_S,"errGuildGuildHighLevel",errGuildGuildHighLevel);
  tolua_constant(tolua_S,"errGuildInviteSuccessfully",errGuildInviteSuccessfully);
  tolua_constant(tolua_S,"errGuildJoinGuildLevelLimit",errGuildJoinGuildLevelLimit);
  tolua_constant(tolua_S,"errGuildSamePosition",errGuildSamePosition);
  tolua_constant(tolua_S,"errGuildPositionLow",errGuildPositionLow);
  tolua_constant(tolua_S,"errGuildPosHumanFull",errGuildPosHumanFull);
  tolua_constant(tolua_S,"errGuildPosLeader",errGuildPosLeader);
  tolua_constant(tolua_S,"errGuildNotLeader",errGuildNotLeader);
  tolua_constant(tolua_S,"errGuildDrawSalaryFailed",errGuildDrawSalaryFailed);
  tolua_constant(tolua_S,"errGuildNoSalary",errGuildNoSalary);
  tolua_constant(tolua_S,"errGuildDrawSalaryAlready",errGuildDrawSalaryAlready);
  tolua_constant(tolua_S,"errGuildDiplomatTimeError",errGuildDiplomatTimeError);
  tolua_constant(tolua_S,"errGuildMoneyLack",errGuildMoneyLack);
  tolua_constant(tolua_S,"errGuildAllianceFull",errGuildAllianceFull);
  tolua_constant(tolua_S,"errGuildIsEnemy",errGuildIsEnemy);
  tolua_constant(tolua_S,"errGuildIsAlliance",errGuildIsAlliance);
  tolua_constant(tolua_S,"errGuildNotAlliance",errGuildNotAlliance);
  tolua_constant(tolua_S,"errGuildAllianceAppAlreadyDel",errGuildAllianceAppAlreadyDel);
  tolua_constant(tolua_S,"errGuildAllianceAppAlready",errGuildAllianceAppAlready);
  tolua_constant(tolua_S,"errGuildAppNotExist",errGuildAppNotExist);
  tolua_constant(tolua_S,"errGuildRoleNotExist",errGuildRoleNotExist);
  tolua_constant(tolua_S,"errGuildJoinCD",errGuildJoinCD);
  tolua_constant(tolua_S,"errGuildTargetJoinCD",errGuildTargetJoinCD);
  tolua_constant(tolua_S,"errGuildStorageTargetSlotError",errGuildStorageTargetSlotError);
  tolua_constant(tolua_S,"errGuildStorageSlotUsed",errGuildStorageSlotUsed);
  tolua_constant(tolua_S,"errGuildStorageItemNotExist",errGuildStorageItemNotExist);
  tolua_constant(tolua_S,"errGuildStorageSlotUsing",errGuildStorageSlotUsing);
  tolua_constant(tolua_S,"errGuildDelHaveAlliance",errGuildDelHaveAlliance);
  tolua_constant(tolua_S,"errGuildDelHaveEnemy",errGuildDelHaveEnemy);
  tolua_constant(tolua_S,"errGuildLeaderCannotDelRole",errGuildLeaderCannotDelRole);
  tolua_constant(tolua_S,"errGuildNoMoreHistory",errGuildNoMoreHistory);
  tolua_constant(tolua_S,"errGuildNoMoreChest",errGuildNoMoreChest);
  tolua_constant(tolua_S,"errGuildConfigError",errGuildConfigError);
  tolua_constant(tolua_S,"errGuildContributionLimit",errGuildContributionLimit);
  tolua_constant(tolua_S,"errGuildStorageCD",errGuildStorageCD);
  tolua_constant(tolua_S,"errGuildChangeAnnounceSucc",errGuildChangeAnnounceSucc);
  tolua_constant(tolua_S,"errGuildWarTime",errGuildWarTime);
  tolua_constant(tolua_S,"errGuildWelfareNeedUpgrade",errGuildWelfareNeedUpgrade);
  tolua_constant(tolua_S,"errGuildWelfareAuraIDError",errGuildWelfareAuraIDError);
  tolua_constant(tolua_S,"errGuildBuildingLevelLower",errGuildBuildingLevelLower);
  tolua_constant(tolua_S,"errGuildNoMoreGuildMoney",errGuildNoMoreGuildMoney);
  tolua_constant(tolua_S,"errGuildNoMoreWood",errGuildNoMoreWood);
  tolua_constant(tolua_S,"errGuildNoMoreHerb",errGuildNoMoreHerb);
  tolua_constant(tolua_S,"errGuildNoMoreCrystal",errGuildNoMoreCrystal);
  tolua_constant(tolua_S,"errGuildNoMoreSilver",errGuildNoMoreSilver);
  tolua_constant(tolua_S,"errGuildMaxLevel",errGuildMaxLevel);
  tolua_constant(tolua_S,"errGuildWelfareBufferNotExist",errGuildWelfareBufferNotExist);
  tolua_constant(tolua_S,"errGuildBuyAuraFailed",errGuildBuyAuraFailed);
  tolua_constant(tolua_S,"errGuildAuraPurchaseLimit",errGuildAuraPurchaseLimit);
  tolua_constant(tolua_S,"errGuildWelfareUpgrading",errGuildWelfareUpgrading);
  tolua_constant(tolua_S,"errGuildUpgradingCountLimit",errGuildUpgradingCountLimit);
  tolua_constant(tolua_S,"errGuildBuildingIDError",errGuildBuildingIDError);
  tolua_constant(tolua_S,"errGuildBuildingNpcIDError",errGuildBuildingNpcIDError);
  tolua_constant(tolua_S,"errGuildBuildingConfigError",errGuildBuildingConfigError);
  tolua_constant(tolua_S,"errGuildBuildingAlreadyExist",errGuildBuildingAlreadyExist);
  tolua_constant(tolua_S,"errGuildBuildingNotExist",errGuildBuildingNotExist);
  tolua_constant(tolua_S,"errGuildBuildingUpgrading",errGuildBuildingUpgrading);
  tolua_constant(tolua_S,"errGuildBuildingUpgradeError",errGuildBuildingUpgradeError);
  tolua_constant(tolua_S,"errGuildBuildingDead",errGuildBuildingDead);
  tolua_constant(tolua_S,"errGuildErrorBuildingTypeForNpc",errGuildErrorBuildingTypeForNpc);
  tolua_constant(tolua_S,"errGuildBuildingMaxLevel",errGuildBuildingMaxLevel);
  tolua_constant(tolua_S,"errGuildBTypeNotMatch",errGuildBTypeNotMatch);
  tolua_constant(tolua_S,"errGuildBMaxNum",errGuildBMaxNum);
  tolua_constant(tolua_S,"errGuildCannotDel",errGuildCannotDel);
  tolua_constant(tolua_S,"errGuildCannotCreate",errGuildCannotCreate);
  tolua_constant(tolua_S,"errGuildBaseBuildingFailed",errGuildBaseBuildingFailed);
  tolua_constant(tolua_S,"errGuildUpgradeDataNotExist",errGuildUpgradeDataNotExist);
  tolua_constant(tolua_S,"errGuildEnterSceneFailed",errGuildEnterSceneFailed);
  tolua_constant(tolua_S,"errGuildWeatherAlreadyBeChanged",errGuildWeatherAlreadyBeChanged);
  tolua_constant(tolua_S,"errGuildWrongMap",errGuildWrongMap);
  tolua_constant(tolua_S,"errGuildWelfareBeFrozen",errGuildWelfareBeFrozen);
  tolua_constant(tolua_S,"errGuildAlreadyOpenDungeon",errGuildAlreadyOpenDungeon);
  tolua_constant(tolua_S,"errGuildOpenDungeonFailed",errGuildOpenDungeonFailed);
  tolua_constant(tolua_S,"errGuildEnterDungeonFailed",errGuildEnterDungeonFailed);
  tolua_constant(tolua_S,"errGuildCannotFindDungeon",errGuildCannotFindDungeon);
  tolua_constant(tolua_S,"errGuildNotDungeonLeader",errGuildNotDungeonLeader);
  tolua_constant(tolua_S,"errGuildDeclareFailed",errGuildDeclareFailed);
  tolua_constant(tolua_S,"errGuildCannotDeclaredYourself",errGuildCannotDeclaredYourself);
  tolua_constant(tolua_S,"errGuildWarLevelLimit",errGuildWarLevelLimit);
  tolua_constant(tolua_S,"errGuildInProtectionTime",errGuildInProtectionTime);
  tolua_constant(tolua_S,"errGuildAttackCD",errGuildAttackCD);
  tolua_constant(tolua_S,"errGuildAllianceCannotBeDeclared",errGuildAllianceCannotBeDeclared);
  tolua_constant(tolua_S,"errGuildInDeclareTime",errGuildInDeclareTime);
  tolua_constant(tolua_S,"errGuildDungeonMapClosing",errGuildDungeonMapClosing);
  tolua_constant(tolua_S,"errGuildMatchGuildFailed",errGuildMatchGuildFailed);
  tolua_constant(tolua_S,"errGuildMatchWarCountLimit",errGuildMatchWarCountLimit);
  tolua_constant(tolua_S,"errGuildInWar",errGuildInWar);
  tolua_constant(tolua_S,"errGuildNotWarTime",errGuildNotWarTime);
  tolua_constant(tolua_S,"errGuildBeFightingToday",errGuildBeFightingToday);
  tolua_constant(tolua_S,"errGuildAchiAlreadyReward",errGuildAchiAlreadyReward);
  tolua_constant(tolua_S,"errGuildAchiNotComplete",errGuildAchiNotComplete);
  tolua_constant(tolua_S,"errGuildAchiRewardSucc",errGuildAchiRewardSucc);
  tolua_constant(tolua_S,"errGuildActivityConfigError",errGuildActivityConfigError);
  tolua_constant(tolua_S,"errGuildAcAlreadyOpen",errGuildAcAlreadyOpen);
  tolua_constant(tolua_S,"errGuildTodayAlreadyAcceptEscort",errGuildTodayAlreadyAcceptEscort);
  tolua_constant(tolua_S,"errGuildTodayNotOpenEscort",errGuildTodayNotOpenEscort);
  tolua_constant(tolua_S,"errGuildEscortCarBeenKilled",errGuildEscortCarBeenKilled);
  tolua_constant(tolua_S,"errGuildEscortCarTimeOut",errGuildEscortCarTimeOut);
  tolua_constant(tolua_S,"errGuildDelAllianceSuccess",errGuildDelAllianceSuccess);
  tolua_constant(tolua_S,"errGuildDeclearWarMustEnemy",errGuildDeclearWarMustEnemy);
  tolua_constant(tolua_S,"errGuildCannotLeaveGuildInCombat",errGuildCannotLeaveGuildInCombat);
  tolua_constant(tolua_S,"errGuildCannotBanishTargetInCombat",errGuildCannotBanishTargetInCombat);
  tolua_constant(tolua_S,"errGuildEscortLevelNotEnough",errGuildEscortLevelNotEnough);
  tolua_constant(tolua_S,"errGuildPageEnd",errGuildPageEnd);
  tolua_constant(tolua_S,"errGuildCannotLeaveInWar",errGuildCannotLeaveInWar);
  tolua_constant(tolua_S,"errGuildCannotDisbandInWar",errGuildCannotDisbandInWar);
  tolua_constant(tolua_S,"errGuildNotActivityTime",errGuildNotActivityTime);
  tolua_constant(tolua_S,"errGuildWarTargetLevelLimit",errGuildWarTargetLevelLimit);
  tolua_constant(tolua_S,"errGuildWarTargetAlreadyBeDeclared",errGuildWarTargetAlreadyBeDeclared);
  tolua_constant(tolua_S,"errGuildWarClose",errGuildWarClose);
  tolua_constant(tolua_S,"errGuildEnd",errGuildEnd);
  tolua_constant(tolua_S,"errSpiritBegin",errSpiritBegin);
  tolua_constant(tolua_S,"errSpiritNotOpen",errSpiritNotOpen);
  tolua_constant(tolua_S,"errSpiritDailyChangeCount",errSpiritDailyChangeCount);
  tolua_constant(tolua_S,"errSpiritMoneyNotEnough",errSpiritMoneyNotEnough);
  tolua_constant(tolua_S,"errSpiritPtsNotEnough",errSpiritPtsNotEnough);
  tolua_constant(tolua_S,"errSpiritChangeCDTime",errSpiritChangeCDTime);
  tolua_constant(tolua_S,"errSpiritLevel",errSpiritLevel);
  tolua_constant(tolua_S,"errSpiritProductLevel",errSpiritProductLevel);
  tolua_constant(tolua_S,"errSpiritUpExpNotEnough",errSpiritUpExpNotEnough);
  tolua_constant(tolua_S,"errSpiritUpLevelMax",errSpiritUpLevelMax);
  tolua_constant(tolua_S,"errSpiritAirSeaBagFull",errSpiritAirSeaBagFull);
  tolua_constant(tolua_S,"errSpiritExchangeMoney",errSpiritExchangeMoney);
  tolua_constant(tolua_S,"errSpiritAirSlotNotEmpty",errSpiritAirSlotNotEmpty);
  tolua_constant(tolua_S,"errSpiritAirSlotEmpty",errSpiritAirSlotEmpty);
  tolua_constant(tolua_S,"errSpiritAirEquipSameGroup",errSpiritAirEquipSameGroup);
  tolua_constant(tolua_S,"errSpiritDailyExp2PtsOut",errSpiritDailyExp2PtsOut);
  tolua_constant(tolua_S,"errSpiritExp2PtsMoneyOut",errSpiritExp2PtsMoneyOut);
  tolua_constant(tolua_S,"errSpiritBloodAlreadyOpen",errSpiritBloodAlreadyOpen);
  tolua_constant(tolua_S,"errSpiritRoleLevelLimit",errSpiritRoleLevelLimit);
  tolua_constant(tolua_S,"errSpiritBloodSlotNumber",errSpiritBloodSlotNumber);
  tolua_constant(tolua_S,"errSpiritBloodNotOpen",errSpiritBloodNotOpen);
  tolua_constant(tolua_S,"errSpiritBloodSlotLevel",errSpiritBloodSlotLevel);
  tolua_constant(tolua_S,"errSpiritSpellPts",errSpiritSpellPts);
  tolua_constant(tolua_S,"errSpiritExp2PtsMaxCount",errSpiritExp2PtsMaxCount);
  tolua_constant(tolua_S,"errSpiritEnd",errSpiritEnd);
  tolua_constant(tolua_S,"errHeroBegin",errHeroBegin);
  tolua_constant(tolua_S,"errHeroInvalid",errHeroInvalid);
  tolua_constant(tolua_S,"errHeroAlreadyExist",errHeroAlreadyExist);
  tolua_constant(tolua_S,"errHeroNotExist",errHeroNotExist);
  tolua_constant(tolua_S,"errHeroSummonFailed",errHeroSummonFailed);
  tolua_constant(tolua_S,"errHeroNotReady",errHeroNotReady);
  tolua_constant(tolua_S,"errHeroContainerNotEmpty",errHeroContainerNotEmpty);
  tolua_constant(tolua_S,"errHeroAiInvalid",errHeroAiInvalid);
  tolua_constant(tolua_S,"errHeroAlreadyHaveName",errHeroAlreadyHaveName);
  tolua_constant(tolua_S,"errHeroAlreadySummoned",errHeroAlreadySummoned);
  tolua_constant(tolua_S,"errHeroAlreadyRecalled",errHeroAlreadyRecalled);
  tolua_constant(tolua_S,"errHeroEnd",errHeroEnd);
  tolua_constant(tolua_S,"errShieldBegin",errShieldBegin);
  tolua_constant(tolua_S,"errNotFindTargetRole",errNotFindTargetRole);
  tolua_constant(tolua_S,"errAlreadyShield",errAlreadyShield);
  tolua_constant(tolua_S,"errNotFindInShieldList",errNotFindInShieldList);
  tolua_constant(tolua_S,"errShieldedListFull",errShieldedListFull);
  tolua_constant(tolua_S,"errCannotShieldYourself",errCannotShieldYourself);
  tolua_constant(tolua_S,"errShieldEnd",errShieldEnd);
  tolua_constant(tolua_S,"errAuctionBegin",errAuctionBegin);
  tolua_constant(tolua_S,"errAuctionItemNotFound",errAuctionItemNotFound);
  tolua_constant(tolua_S,"errAuctionSellingFull",errAuctionSellingFull);
  tolua_constant(tolua_S,"errAuctionBuyingFull",errAuctionBuyingFull);
  tolua_constant(tolua_S,"errAuctionQuerying",errAuctionQuerying);
  tolua_constant(tolua_S,"errAuctionSellingPriceLow",errAuctionSellingPriceLow);
  tolua_constant(tolua_S,"errAuctionBuyingPriceLow",errAuctionBuyingPriceLow);
  tolua_constant(tolua_S,"errAuctionSellerCannotBuy",errAuctionSellerCannotBuy);
  tolua_constant(tolua_S,"errAuctionBuyerCannotBuy",errAuctionBuyerCannotBuy);
  tolua_constant(tolua_S,"errAuctionOnBid",errAuctionOnBid);
  tolua_constant(tolua_S,"errAuctionNotOpen",errAuctionNotOpen);
  tolua_constant(tolua_S,"errAuctionUnknown",errAuctionUnknown);
  tolua_constant(tolua_S,"errAuctionEnd",errAuctionEnd);
  tolua_constant(tolua_S,"errExchangeBegin",errExchangeBegin);
  tolua_constant(tolua_S,"errExchangeOrderFull",errExchangeOrderFull);
  tolua_constant(tolua_S,"errExchangeOrderNotFound",errExchangeOrderNotFound);
  tolua_constant(tolua_S,"errExchangeQuerying",errExchangeQuerying);
  tolua_constant(tolua_S,"errExchangeSceneForbidden",errExchangeSceneForbidden);
  tolua_constant(tolua_S,"errExchangeEnd",errExchangeEnd);
  tolua_constant(tolua_S,"errActivityBegin",errActivityBegin);
  tolua_constant(tolua_S,"errLackofActivity",errLackofActivity);
  tolua_constant(tolua_S,"errActivityRewardAccepted",errActivityRewardAccepted);
  tolua_constant(tolua_S,"errActivityEnd",errActivityEnd);
  tolua_constant(tolua_S,"errRMShopBegin",errRMShopBegin);
  tolua_constant(tolua_S,"errRealMoneyShopNotOpen",errRealMoneyShopNotOpen);
  tolua_constant(tolua_S,"errCannotConnectSndaServer",errCannotConnectSndaServer);
  tolua_constant(tolua_S,"errRealMoneyBuyNotEnoughMoney",errRealMoneyBuyNotEnoughMoney);
  tolua_constant(tolua_S,"errRealMoneyShopItemNotFound",errRealMoneyShopItemNotFound);
  tolua_constant(tolua_S,"errRealMoneyShopFriendNotFound",errRealMoneyShopFriendNotFound);
  tolua_constant(tolua_S,"errRealMoneyShopCrcFailed",errRealMoneyShopCrcFailed);
  tolua_constant(tolua_S,"errBillingSystemError",errBillingSystemError);
  tolua_constant(tolua_S,"errTimeBillingNotOpen",errTimeBillingNotOpen);
  tolua_constant(tolua_S,"errTimeBillingNotEnough",errTimeBillingNotEnough);
  tolua_constant(tolua_S,"errTimeBillBusy",errTimeBillBusy);
  tolua_constant(tolua_S,"errRMShopEnd",errRMShopEnd);
  tolua_constant(tolua_S,"errRankListBegin",errRankListBegin);
  tolua_constant(tolua_S,"errRoleNotOnline",errRoleNotOnline);
  tolua_constant(tolua_S,"errRankListEnd",errRankListEnd);
  tolua_constant(tolua_S,"errSabacBegin",errSabacBegin);
  tolua_constant(tolua_S,"errSabacNotGuildLeader",errSabacNotGuildLeader);
  tolua_constant(tolua_S,"errSabacNotEnoughMoney",errSabacNotEnoughMoney);
  tolua_constant(tolua_S,"errSabacNotEnoughItem",errSabacNotEnoughItem);
  tolua_constant(tolua_S,"errSabacEnrolled",errSabacEnrolled);
  tolua_constant(tolua_S,"errSabacOccupied",errSabacOccupied);
  tolua_constant(tolua_S,"errSabacFighting",errSabacFighting);
  tolua_constant(tolua_S,"errSabacErectStatue",errSabacErectStatue);
  tolua_constant(tolua_S,"errSabacErectNotEnoughMoney",errSabacErectNotEnoughMoney);
  tolua_constant(tolua_S,"errSabacEnd",errSabacEnd);
  tolua_constant(tolua_S,"errReputationBegin",errReputationBegin);
  tolua_constant(tolua_S,"errRepReceiveAwardSuccess",errRepReceiveAwardSuccess);
  tolua_constant(tolua_S,"errRepAlreadyReceived",errRepAlreadyReceived);
  tolua_constant(tolua_S,"errRepNotEnoughReputation",errRepNotEnoughReputation);
  tolua_constant(tolua_S,"errReputationEnd",errReputationEnd);
  tolua_constant(tolua_S,"errPetBegin",errPetBegin);
  tolua_constant(tolua_S,"errPetChangeStateSuccess",errPetChangeStateSuccess);
  tolua_constant(tolua_S,"errPetStateSame",errPetStateSame);
  tolua_constant(tolua_S,"errPetEnd",errPetEnd);
  tolua_constant(tolua_S,"errCheckInBegin",errCheckInBegin);
  tolua_constant(tolua_S,"errCheckInNotExist",errCheckInNotExist);
  tolua_constant(tolua_S,"errCheckInAlreadyDid",errCheckInAlreadyDid);
  tolua_constant(tolua_S,"errCheckInLevelLimit",errCheckInLevelLimit);
  tolua_constant(tolua_S,"errCheckInClosed",errCheckInClosed);
  tolua_constant(tolua_S,"errCheckInEnd",errCheckInEnd);
  tolua_constant(tolua_S,"errAwardBegin",errAwardBegin);
  tolua_constant(tolua_S,"errAwardNotEnabled",errAwardNotEnabled);
  tolua_constant(tolua_S,"errAwardReqSentFailed",errAwardReqSentFailed);
  tolua_constant(tolua_S,"errAwardFailed",errAwardFailed);
  tolua_constant(tolua_S,"errAwardFailed_1",errAwardFailed_1);
  tolua_constant(tolua_S,"errAwardFailed_2",errAwardFailed_2);
  tolua_constant(tolua_S,"errAwardFailed_3",errAwardFailed_3);
  tolua_constant(tolua_S,"errAwardFailed_4",errAwardFailed_4);
  tolua_constant(tolua_S,"errAwardFailed_5",errAwardFailed_5);
  tolua_constant(tolua_S,"errAwardFailed_6",errAwardFailed_6);
  tolua_constant(tolua_S,"errAwardFailed_7",errAwardFailed_7);
  tolua_constant(tolua_S,"errAwardFailed_8",errAwardFailed_8);
  tolua_constant(tolua_S,"errAwardFailed_9",errAwardFailed_9);
  tolua_constant(tolua_S,"errAwardEnd",errAwardEnd);
  tolua_constant(tolua_S,"errAntiCheatBegin",errAntiCheatBegin);
  tolua_constant(tolua_S,"errAntiCheatSuccess",errAntiCheatSuccess);
  tolua_constant(tolua_S,"errAntiCheatError",errAntiCheatError);
  tolua_constant(tolua_S,"errAntiCheatErrorKick",errAntiCheatErrorKick);
  tolua_constant(tolua_S,"errAntiCheatErrorTimeoutReturn",errAntiCheatErrorTimeoutReturn);
  tolua_constant(tolua_S,"errAntiCheatExit",errAntiCheatExit);
  tolua_constant(tolua_S,"errAntiCheatNotReady",errAntiCheatNotReady);
  tolua_constant(tolua_S,"errAntiCheatEnd",errAntiCheatEnd);
  tolua_constant(tolua_S,"errGMBegin",errGMBegin);
  tolua_constant(tolua_S,"errGMNoRight",errGMNoRight);
  tolua_constant(tolua_S,"errGMGSExecute",errGMGSExecute);
  tolua_constant(tolua_S,"errGMNotExecute",errGMNotExecute);
  tolua_constant(tolua_S,"errGMNoCommand",errGMNoCommand);
  tolua_constant(tolua_S,"errGMExecuteFailed",errGMExecuteFailed);
  tolua_constant(tolua_S,"errGMCannotConnectGMSvr",errGMCannotConnectGMSvr);
  tolua_constant(tolua_S,"errGMKickYou",errGMKickYou);
  tolua_constant(tolua_S,"errGMUnknownError",errGMUnknownError);
  tolua_constant(tolua_S,"errGMVersionError",errGMVersionError);
  tolua_constant(tolua_S,"errGMConnectERROR",errGMConnectERROR);
  tolua_constant(tolua_S,"errGMPwdError",errGMPwdError);
  tolua_constant(tolua_S,"errGMRelogin",errGMRelogin);
  tolua_constant(tolua_S,"errGMCannotExecuteNewCmd",errGMCannotExecuteNewCmd);
  tolua_constant(tolua_S,"errGMNotExecutePermission",errGMNotExecutePermission);
  tolua_constant(tolua_S,"errGMGetPermissionError",errGMGetPermissionError);
  tolua_constant(tolua_S,"errGMUserNotFind",errGMUserNotFind);
  tolua_constant(tolua_S,"errGMDBError",errGMDBError);
  tolua_constant(tolua_S,"errGMTimeout",errGMTimeout);
  tolua_constant(tolua_S,"errGMSendMsgToMSError",errGMSendMsgToMSError);
  tolua_constant(tolua_S,"errGMLostTLUser",errGMLostTLUser);
  tolua_constant(tolua_S,"errGMCannotFindRole",errGMCannotFindRole);
  tolua_constant(tolua_S,"errGMLostCmd",errGMLostCmd);
  tolua_constant(tolua_S,"errGMMSNotOnline",errGMMSNotOnline);
  tolua_constant(tolua_S,"errGMMSExecuteCmdFailed",errGMMSExecuteCmdFailed);
  tolua_constant(tolua_S,"errGMSaveRoleDataFailed",errGMSaveRoleDataFailed);
  tolua_constant(tolua_S,"errGMModifyRoleTypeError",errGMModifyRoleTypeError);
  tolua_constant(tolua_S,"errGMRoleOnline",errGMRoleOnline);
  tolua_constant(tolua_S,"errGMQueryZeroRoleData",errGMQueryZeroRoleData);
  tolua_constant(tolua_S,"errGMRoleNotFreeze",errGMRoleNotFreeze);
  tolua_constant(tolua_S,"errGMRoleFreezeLessOneHour",errGMRoleFreezeLessOneHour);
  tolua_constant(tolua_S,"errGMAnnounceNotExist",errGMAnnounceNotExist);
  tolua_constant(tolua_S,"errGMRoleBeOp",errGMRoleBeOp);
  tolua_constant(tolua_S,"errGMRoleLockedByOther",errGMRoleLockedByOther);
  tolua_constant(tolua_S,"errGMStrCommandFailed",errGMStrCommandFailed);
  tolua_constant(tolua_S,"errGMStrCmdExeFailed",errGMStrCmdExeFailed);
  tolua_constant(tolua_S,"errGMCmdExecuting",errGMCmdExecuting);
  tolua_constant(tolua_S,"errGMEnd",errGMEnd);
  tolua_constant(tolua_S,"errMiscBegin",errMiscBegin);
  tolua_constant(tolua_S,"errDisconnectTimeOut",errDisconnectTimeOut);
  tolua_constant(tolua_S,"errFreeMode",errFreeMode);
  tolua_constant(tolua_S,"errRoleLowLevel",errRoleLowLevel);
  tolua_constant(tolua_S,"errArrestRoleOffline",errArrestRoleOffline);
  tolua_constant(tolua_S,"errArrestClosed",errArrestClosed);
  tolua_constant(tolua_S,"errArrestSceneNoPunish",errArrestSceneNoPunish);
  tolua_constant(tolua_S,"errGpkAuthKick",errGpkAuthKick);
  tolua_constant(tolua_S,"errMiscEnd",errMiscEnd);
  tolua_constant(tolua_S,"errNsUnknownError",errNsUnknownError);
  tolua_constant(tolua_S,"errNsSuccess",errNsSuccess);
  tolua_constant(tolua_S,"errNsNameDuplicated",errNsNameDuplicated);
  tolua_constant(tolua_S,"errNsNameDBSysErr",errNsNameDBSysErr);
  tolua_constant(tolua_S,"errNsNameUnknown",errNsNameUnknown);
  tolua_constant(tolua_S,"errNsTooManyUser",errNsTooManyUser);
 tolua_constant(tolua_S,"INVALID_OBJECT_ID",INVALID_OBJECT_ID);
 tolua_constant(tolua_S,"OBJECT_TYPE_MASK",OBJECT_TYPE_MASK);
 tolua_constant(tolua_S,"OBJECT_ID_MASK",OBJECT_ID_MASK);
 tolua_constant(tolua_S,"MAX_OBJECT_ID",MAX_OBJECT_ID);
  tolua_constant(tolua_S,"otInvalid",otInvalid);
  tolua_constant(tolua_S,"otRole",otRole);
  tolua_constant(tolua_S,"otNpc",otNpc);
  tolua_constant(tolua_S,"otHero",otHero);
  tolua_constant(tolua_S,"otDoodad",otDoodad);
  tolua_constant(tolua_S,"otMissile",otMissile);
  tolua_constant(tolua_S,"otDrop",otDrop);
  tolua_constant(tolua_S,"otGuild",otGuild);
  tolua_constant(tolua_S,"otTeam",otTeam);
  tolua_constant(tolua_S,"otBattleTeam",otBattleTeam);
  tolua_constant(tolua_S,"otChannel",otChannel);
  tolua_constant(tolua_S,"otTrumpet",otTrumpet);
  tolua_constant(tolua_S,"otTotal",otTotal);
#ifndef TOLUA_DISABLE_tolua_enum_MakeObjectID00
 tolua_function(tolua_S,"MakeObjectID",tolua_enum_MakeObjectID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_GetTypeFromID00
 tolua_function(tolua_S,"GetTypeFromID",tolua_enum_GetTypeFromID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_GetIndexFromID00
 tolua_function(tolua_S,"GetIndexFromID",tolua_enum_GetIndexFromID00);
#endif //#ifndef TOLUA_DISABLE
  tolua_constant(tolua_S,"sgsInvalid",sgsInvalid);
  tolua_constant(tolua_S,"sgsInitializing",sgsInitializing);
  tolua_constant(tolua_S,"sgsSelfReady",sgsSelfReady);
  tolua_constant(tolua_S,"sgsAllReady",sgsAllReady);
  tolua_constant(tolua_S,"sgsUnInitializing",sgsUnInitializing);
  tolua_constant(tolua_S,"sgsClosing",sgsClosing);
  tolua_constant(tolua_S,"sgsClosed",sgsClosed);
  tolua_constant(tolua_S,"sgsTotal",sgsTotal);
  tolua_constant(tolua_S,"stInvalid",stInvalid);
  tolua_constant(tolua_S,"stGD",stGD);
  tolua_constant(tolua_S,"stMS",stMS);
  tolua_constant(tolua_S,"stDB",stDB);
  tolua_constant(tolua_S,"stBS",stBS);
  tolua_constant(tolua_S,"stLS",stLS);
  tolua_constant(tolua_S,"stGT",stGT);
  tolua_constant(tolua_S,"stGS",stGS);
  tolua_constant(tolua_S,"stGM",stGM);
  tolua_constant(tolua_S,"stDL",stDL);
  tolua_constant(tolua_S,"stMM",stMM);
  tolua_constant(tolua_S,"stMG",stMG);
  tolua_constant(tolua_S,"stClient",stClient);
  tolua_constant(tolua_S,"stMT",stMT);
  tolua_constant(tolua_S,"stCS",stCS);
  tolua_constant(tolua_S,"stNS",stNS);
  tolua_constant(tolua_S,"stLC",stLC);
  tolua_constant(tolua_S,"stGPK",stGPK);
  tolua_constant(tolua_S,"stTL",stTL);
  tolua_constant(tolua_S,"stAll",stAll);
  tolua_constant(tolua_S,"stTotal",stTotal);
  tolua_constant(tolua_S,"csdtInvalid",csdtInvalid);
  tolua_constant(tolua_S,"csdtLogout",csdtLogout);
  tolua_constant(tolua_S,"csdtTimeout",csdtTimeout);
  tolua_constant(tolua_S,"csdtClient",csdtClient);
  tolua_constant(tolua_S,"csdtDB",csdtDB);
  tolua_constant(tolua_S,"csdtGS",csdtGS);
  tolua_constant(tolua_S,"csdtGT",csdtGT);
  tolua_constant(tolua_S,"csdtMS",csdtMS);
  tolua_constant(tolua_S,"csdtMG",csdtMG);
  tolua_constant(tolua_S,"csdtMM",csdtMM);
  tolua_constant(tolua_S,"csdtTotal",csdtTotal);
  tolua_constant(tolua_S,"ktInvalid",ktInvalid);
  tolua_constant(tolua_S,"ktImmediately",ktImmediately);
  tolua_constant(tolua_S,"ktDelayed",ktDelayed);
  tolua_constant(tolua_S,"ktLogged",ktLogged);
  tolua_constant(tolua_S,"ktTotal",ktTotal);
  tolua_constant(tolua_S,"Group0",Group0);
  tolua_constant(tolua_S,"Group1",Group1);
  tolua_constant(tolua_S,"Group2",Group2);
  tolua_constant(tolua_S,"Group3",Group3);
  tolua_constant(tolua_S,"Group4",Group4);
  tolua_constant(tolua_S,"Group5",Group5);
  tolua_constant(tolua_S,"Group6",Group6);
  tolua_constant(tolua_S,"Group7",Group7);
  tolua_constant(tolua_S,"Group8",Group8);
  tolua_constant(tolua_S,"Group9",Group9);
  tolua_constant(tolua_S,"Group10",Group10);
  tolua_constant(tolua_S,"Group11",Group11);
  tolua_constant(tolua_S,"Group12",Group12);
  tolua_constant(tolua_S,"Group13",Group13);
  tolua_constant(tolua_S,"Group14",Group14);
  tolua_constant(tolua_S,"Group15",Group15);
  tolua_constant(tolua_S,"Group16",Group16);
  tolua_constant(tolua_S,"Group17",Group17);
  tolua_constant(tolua_S,"Group18",Group18);
  tolua_constant(tolua_S,"Group19",Group19);
  tolua_constant(tolua_S,"Group20",Group20);
  tolua_constant(tolua_S,"Group21",Group21);
  tolua_constant(tolua_S,"Group22",Group22);
  tolua_constant(tolua_S,"Group23",Group23);
  tolua_constant(tolua_S,"Group24",Group24);
  tolua_constant(tolua_S,"Group25",Group25);
  tolua_constant(tolua_S,"Group26",Group26);
  tolua_constant(tolua_S,"Group27",Group27);
  tolua_constant(tolua_S,"Group28",Group28);
  tolua_constant(tolua_S,"Group29",Group29);
  tolua_constant(tolua_S,"Group30",Group30);
  tolua_constant(tolua_S,"Group31",Group31);
  tolua_constant(tolua_S,"GroupAll",GroupAll);
 tolua_constant(tolua_S,"SNDA_APPID",SNDA_APPID);
 tolua_constant(tolua_S,"FCM_APPID",FCM_APPID);
 tolua_constant(tolua_S,"MAX_GS_SESSION_COUNT",MAX_GS_SESSION_COUNT);
 tolua_constant(tolua_S,"MAX_GT_SESSION_COUNT",MAX_GT_SESSION_COUNT);
 tolua_constant(tolua_S,"MAX_MS_SESSION_COUNT",MAX_MS_SESSION_COUNT);
 tolua_constant(tolua_S,"MAX_GM_SESSION_COUNT",MAX_GM_SESSION_COUNT);
 tolua_constant(tolua_S,"MAX_GM_TL_SESSION_COUNT",MAX_GM_TL_SESSION_COUNT);
 tolua_constant(tolua_S,"MAX_MG_SESSION_COUNT",MAX_MG_SESSION_COUNT);
 tolua_constant(tolua_S,"MAX_PROCESS_COUNT_PER_GUARD",MAX_PROCESS_COUNT_PER_GUARD);
 tolua_constant(tolua_S,"MAX_SERVER_GROUP_COUNT",MAX_SERVER_GROUP_COUNT);
 tolua_constant(tolua_S,"MAX_AREA_COUNT",MAX_AREA_COUNT);
 tolua_constant(tolua_S,"MAX_GUARD_COUNT_PER_AREA",MAX_GUARD_COUNT_PER_AREA);
 tolua_constant(tolua_S,"MAX_CLIENT_SESSION_COUNT",MAX_CLIENT_SESSION_COUNT);
 tolua_constant(tolua_S,"MAX_BS_CLIENT_SESSION_COUNT",MAX_BS_CLIENT_SESSION_COUNT);
 tolua_constant(tolua_S,"MAX_EXTERNAL_IP_COUNT",MAX_EXTERNAL_IP_COUNT);
 tolua_constant(tolua_S,"MAX_NPC_ID",MAX_NPC_ID);
 tolua_constant(tolua_S,"MAX_NPC_FORCE_ID",MAX_NPC_FORCE_ID);
 tolua_constant(tolua_S,"INVALID_FORCE_ID",INVALID_FORCE_ID);
 tolua_constant(tolua_S,"MAX_NPC_LEVEL",MAX_NPC_LEVEL);
 tolua_constant(tolua_S,"MIN_NPC_RADIUS",MIN_NPC_RADIUS);
 tolua_constant(tolua_S,"MAX_LEVEL_DIFF",MAX_LEVEL_DIFF);
 tolua_constant(tolua_S,"MAX_PET_LEVEL",MAX_PET_LEVEL);
 tolua_constant(tolua_S,"CASTER_WASHOUT_TIME",CASTER_WASHOUT_TIME);
 tolua_constant(tolua_S,"TARGET_WASHOUT_TIME",TARGET_WASHOUT_TIME);
  tolua_constant(tolua_S,"npcInvalid",npcInvalid);
  tolua_constant(tolua_S,"npcSpawn",npcSpawn);
  tolua_constant(tolua_S,"npcAlive",npcAlive);
  tolua_constant(tolua_S,"npcCorpse",npcCorpse);
  tolua_constant(tolua_S,"npcRespawn",npcRespawn);
  tolua_constant(tolua_S,"npcRevive",npcRevive);
  tolua_constant(tolua_S,"npcTotal",npcTotal);
  tolua_constant(tolua_S,"nqInvalid",nqInvalid);
  tolua_constant(tolua_S,"nqNormal1",nqNormal1);
  tolua_constant(tolua_S,"nqNormal2",nqNormal2);
  tolua_constant(tolua_S,"nqNormal3",nqNormal3);
  tolua_constant(tolua_S,"nqNormal4",nqNormal4);
  tolua_constant(tolua_S,"nqNormal5",nqNormal5);
  tolua_constant(tolua_S,"nqElite",nqElite);
  tolua_constant(tolua_S,"nqRare",nqRare);
  tolua_constant(tolua_S,"nqBoss",nqBoss);
  tolua_constant(tolua_S,"nqTotal",nqTotal);
  tolua_constant(tolua_S,"npctInvalid",npctInvalid);
  tolua_constant(tolua_S,"npctNpc",npctNpc);
  tolua_constant(tolua_S,"npctPet",npctPet);
  tolua_constant(tolua_S,"npctTotal",npctTotal);
  tolua_constant(tolua_S,"petInvalid",petInvalid);
  tolua_constant(tolua_S,"petTotal",petTotal);
  tolua_constant(tolua_S,"petBornInvalid",petBornInvalid);
  tolua_constant(tolua_S,"petBornHoly",petBornHoly);
  tolua_constant(tolua_S,"petBornSkeleton",petBornSkeleton);
  tolua_constant(tolua_S,"petBornByMage",petBornByMage);
  tolua_constant(tolua_S,"petBornTotal",petBornTotal);
  tolua_constant(tolua_S,"psInvalid",psInvalid);
  tolua_constant(tolua_S,"psIdle",psIdle);
  tolua_constant(tolua_S,"psFight",psFight);
  tolua_constant(tolua_S,"psTotal",psTotal);
  tolua_constant(tolua_S,"PET_AIV_MAGE_PET_SPELL_ID",PET_AIV_MAGE_PET_SPELL_ID);
  tolua_constant(tolua_S,"PET_AIV_FIGHT_STATE_INDEX",PET_AIV_FIGHT_STATE_INDEX);
  tolua_constant(tolua_S,"PET_AI_CONTROL",PET_AI_CONTROL);
  tolua_constant(tolua_S,"PET_AI_IS_EXTRA_WALK",PET_AI_IS_EXTRA_WALK);
  tolua_constant(tolua_S,"PET_AI_MAGE_CHASE_THRESHOLD",PET_AI_MAGE_CHASE_THRESHOLD);
  tolua_constant(tolua_S,"PET_AI_MAGE_PREFER_DISTANCE",PET_AI_MAGE_PREFER_DISTANCE);
  tolua_constant(tolua_S,"PET_AI_GO_HOME",PET_AI_GO_HOME);
 tolua_constant(tolua_S,"MAX_ROLE_FRACTION_ID",MAX_ROLE_FRACTION_ID);
 tolua_constant(tolua_S,"MAX_ROLE_LEVEL",MAX_ROLE_LEVEL);
 tolua_constant(tolua_S,"MAX_PK_VALUE",MAX_PK_VALUE);
 tolua_constant(tolua_S,"MIN_REPUTATION",MIN_REPUTATION);
 tolua_constant(tolua_S,"MAX_ACTIVITY_LEVEL",MAX_ACTIVITY_LEVEL);
 tolua_constant(tolua_S,"MAX_USABLE_ROLE_PER_USER",MAX_USABLE_ROLE_PER_USER);
 tolua_constant(tolua_S,"MAX_DELETE_ROLE_PER_USER",MAX_DELETE_ROLE_PER_USER);
 tolua_constant(tolua_S,"MAX_ROLE_PER_USER",MAX_ROLE_PER_USER);
 tolua_constant(tolua_S,"MAX_ROLE_HAIR_TYPE",MAX_ROLE_HAIR_TYPE);
 tolua_constant(tolua_S,"MAX_ROLE_HAIR_COLOR",MAX_ROLE_HAIR_COLOR);
 tolua_constant(tolua_S,"MAX_ROLE_FACE_TYPE",MAX_ROLE_FACE_TYPE);
 tolua_constant(tolua_S,"MAX_ROLE_SKIN_COLOR",MAX_ROLE_SKIN_COLOR);
 tolua_constant(tolua_S,"MOMENTARY_KILL_NPC_TIEM",MOMENTARY_KILL_NPC_TIEM);
 tolua_constant(tolua_S,"MIN_MOMENTARY_KILL_NPC_COUNT",MIN_MOMENTARY_KILL_NPC_COUNT);
 tolua_constant(tolua_S,"MIN_CONSECUTIVE_KILL_NPC_COUNT",MIN_CONSECUTIVE_KILL_NPC_COUNT);
 tolua_constant(tolua_S,"MAX_CONSECUTIVE_KILL_NPC_FRAME",MAX_CONSECUTIVE_KILL_NPC_FRAME);
 tolua_constant(tolua_S,"MAX_CONSECTIVE_KILL_PLAYER_FRAME",MAX_CONSECTIVE_KILL_PLAYER_FRAME);
 tolua_constant(tolua_S,"MAX_CONSECUTIVE_KILL_PLAYER_COUNT",MAX_CONSECUTIVE_KILL_PLAYER_COUNT);
 tolua_constant(tolua_S,"MAX_QUERY_ROLE_INFO_PER_SEC",MAX_QUERY_ROLE_INFO_PER_SEC);
 tolua_constant(tolua_S,"MAX_VIP_MONTH",MAX_VIP_MONTH);
 tolua_constant(tolua_S,"MAX_GM_LEVEL",MAX_GM_LEVEL);
 tolua_constant(tolua_S,"MAX_PET_PER_ROLE",MAX_PET_PER_ROLE);
 tolua_constant(tolua_S,"MAX_REPUTATION_AWARD_COUNT",MAX_REPUTATION_AWARD_COUNT);
 tolua_constant(tolua_S,"ROLE_DELAY_LOGOUT_FRAME",ROLE_DELAY_LOGOUT_FRAME);
 tolua_constant(tolua_S,"ROLE_DELAY_LOGOUT_FRAME_ABNORMAL",ROLE_DELAY_LOGOUT_FRAME_ABNORMAL);
 tolua_constant(tolua_S,"ROLE_DELAY_LOGOUT_FRAME",ROLE_DELAY_LOGOUT_FRAME);
 tolua_constant(tolua_S,"ROLE_DELAY_LOGOUT_FRAME_ABNORMAL",ROLE_DELAY_LOGOUT_FRAME_ABNORMAL);
 tolua_constant(tolua_S,"DEFAULT_ROLE_ANTI_CHEAT_LEVEL",DEFAULT_ROLE_ANTI_CHEAT_LEVEL);
 tolua_constant(tolua_S,"MAX_ROLE_ANTI_CHEAT_LEVEL",MAX_ROLE_ANTI_CHEAT_LEVEL);
 tolua_constant(tolua_S,"MAX_ROLE_ANTI_CHEAT_PUNISH_LEVEL",MAX_ROLE_ANTI_CHEAT_PUNISH_LEVEL);
 tolua_constant(tolua_S,"MAX_ROLE_GPK_PUNISH_LEVEL",MAX_ROLE_GPK_PUNISH_LEVEL);
  tolua_constant(tolua_S,"rltInvalid",rltInvalid);
  tolua_constant(tolua_S,"rltReturnLogin",rltReturnLogin);
  tolua_constant(tolua_S,"rltReturnSelectRole",rltReturnSelectRole);
  tolua_constant(tolua_S,"rltDisconnect",rltDisconnect);
  tolua_constant(tolua_S,"rltKickout",rltKickout);
  tolua_constant(tolua_S,"rltTotal",rltTotal);
  tolua_constant(tolua_S,"rsInvalid",rsInvalid);
  tolua_constant(tolua_S,"rsPreEntered",rsPreEntered);
  tolua_constant(tolua_S,"rsWaitRoleData",rsWaitRoleData);
  tolua_constant(tolua_S,"rsWaitMSApprove",rsWaitMSApprove);
  tolua_constant(tolua_S,"rsWaitClientLoad",rsWaitClientLoad);
  tolua_constant(tolua_S,"rsPlaying",rsPlaying);
  tolua_constant(tolua_S,"rsInJump",rsInJump);
  tolua_constant(tolua_S,"rsJumpInGSWaitClientLoad",rsJumpInGSWaitClientLoad);
  tolua_constant(tolua_S,"rsWaitUnlockRoleData",rsWaitUnlockRoleData);
  tolua_constant(tolua_S,"rsLogout",rsLogout);
  tolua_constant(tolua_S,"rsTotal",rsTotal);
  tolua_constant(tolua_S,"fcmsFull",fcmsFull);
  tolua_constant(tolua_S,"fcmsHalf",fcmsHalf);
  tolua_constant(tolua_S,"fcmsNone",fcmsNone);
  tolua_constant(tolua_S,"fcmsTotal",fcmsTotal);
  tolua_constant(tolua_S,"faceInvalid",faceInvalid);
  tolua_constant(tolua_S,"faceTotal",faceTotal);
  tolua_constant(tolua_S,"rpmInvalid",rpmInvalid);
  tolua_constant(tolua_S,"rpmPeace",rpmPeace);
  tolua_constant(tolua_S,"rpmGuild",rpmGuild);
  tolua_constant(tolua_S,"rpmTeam",rpmTeam);
  tolua_constant(tolua_S,"rpmSlayer",rpmSlayer);
  tolua_constant(tolua_S,"rpmJustice",rpmJustice);
  tolua_constant(tolua_S,"rpmMaster",rpmMaster);
  tolua_constant(tolua_S,"rpmTotal",rpmTotal);
  tolua_constant(tolua_S,"rprInvalid",rprInvalid);
  tolua_constant(tolua_S,"rprNormal",rprNormal);
  tolua_constant(tolua_S,"rprRedName",rprRedName);
  tolua_constant(tolua_S,"rprCounterStrike",rprCounterStrike);
  tolua_constant(tolua_S,"rprOpponentGuildMember",rprOpponentGuildMember);
  tolua_constant(tolua_S,"rprLuckyKill",rprLuckyKill);
  tolua_constant(tolua_S,"rprNoPunishment",rprNoPunishment);
  tolua_constant(tolua_S,"rprTotal",rprTotal);
  tolua_constant(tolua_S,"rncInvalid",rncInvalid);
  tolua_constant(tolua_S,"rncWhite",rncWhite);
  tolua_constant(tolua_S,"rncYellow",rncYellow);
  tolua_constant(tolua_S,"rncRed",rncRed);
  tolua_constant(tolua_S,"rncTotal",rncTotal);
  tolua_constant(tolua_S,"ervtInvalid",ervtInvalid);
  tolua_constant(tolua_S,"ervtTotal",ervtTotal);
  tolua_constant(tolua_S,"rfrInvalid",rfrInvalid);
  tolua_constant(tolua_S,"rfrDefault",rfrDefault);
  tolua_constant(tolua_S,"rfrFriend",rfrFriend);
  tolua_constant(tolua_S,"rfrEnemy",rfrEnemy);
  tolua_constant(tolua_S,"rfrTotal",rfrTotal);
  tolua_constant(tolua_S,"vlLevel0",vlLevel0);
  tolua_constant(tolua_S,"vlLevel1",vlLevel1);
  tolua_constant(tolua_S,"vlLevel2",vlLevel2);
  tolua_constant(tolua_S,"vlLevel3",vlLevel3);
  tolua_constant(tolua_S,"vlTotal",vlTotal);
  tolua_constant(tolua_S,"MSG_PP_PERSONAL_ENEMY",MSG_PP_PERSONAL_ENEMY);
  tolua_constant(tolua_S,"aprInvalid",aprInvalid);
  tolua_constant(tolua_S,"aprWuDouChang",aprWuDouChang);
  tolua_constant(tolua_S,"aprBaoku",aprBaoku);
  tolua_constant(tolua_S,"aprYanWuChang",aprYanWuChang);
  tolua_constant(tolua_S,"aprPray",aprPray);
  tolua_constant(tolua_S,"aprEscort",aprEscort);
  tolua_constant(tolua_S,"aprGuildBoss",aprGuildBoss);
  tolua_constant(tolua_S,"aprSpring",aprSpring);
  tolua_constant(tolua_S,"aprQuest",aprQuest);
  tolua_constant(tolua_S,"aprYaoTa",aprYaoTa);
  tolua_constant(tolua_S,"aprYouMingSea",aprYouMingSea);
  tolua_constant(tolua_S,"aprTotal",aprTotal);
 tolua_constant(tolua_S,"MAX_DOODAD_LEVEL",MAX_DOODAD_LEVEL);
  tolua_constant(tolua_S,"dtInvalid",dtInvalid);
  tolua_constant(tolua_S,"dtResource",dtResource);
  tolua_constant(tolua_S,"dtTreasureBox",dtTreasureBox);
  tolua_constant(tolua_S,"dtQuest",dtQuest);
  tolua_constant(tolua_S,"dtDestrucible",dtDestrucible);
  tolua_constant(tolua_S,"dtCorpse",dtCorpse);
  tolua_constant(tolua_S,"dtTotal",dtTotal);
  tolua_constant(tolua_S,"dstInvalid",dstInvalid);
  tolua_constant(tolua_S,"dstOre",dstOre);
  tolua_constant(tolua_S,"dstHerb",dstHerb);
  tolua_constant(tolua_S,"dstFishingPool",dstFishingPool);
  tolua_constant(tolua_S,"dstItem",dstItem);
  tolua_constant(tolua_S,"dstBook",dstBook);
  tolua_constant(tolua_S,"dstTotal",dstTotal);
  tolua_constant(tolua_S,"CMS_INVALID",CMS_INVALID);
  tolua_constant(tolua_S,"CMS_ACTIVE_MOVE",CMS_ACTIVE_MOVE);
  tolua_constant(tolua_S,"CMS_STAND",CMS_STAND);
  tolua_constant(tolua_S,"CMS_SIT",CMS_SIT);
  tolua_constant(tolua_S,"CMS_WALK",CMS_WALK);
  tolua_constant(tolua_S,"CMS_RUN",CMS_RUN);
  tolua_constant(tolua_S,"CMS_MOVE",CMS_MOVE);
  tolua_constant(tolua_S,"CMS_MINE",CMS_MINE);
  tolua_constant(tolua_S,"CMS_PASSIVE_MOVE",CMS_PASSIVE_MOVE);
  tolua_constant(tolua_S,"CMS_DASH",CMS_DASH);
  tolua_constant(tolua_S,"CMS_KNOCK_BACK",CMS_KNOCK_BACK);
  tolua_constant(tolua_S,"CMS_THROWN",CMS_THROWN);
  tolua_constant(tolua_S,"CMS_FREEZE",CMS_FREEZE);
  tolua_constant(tolua_S,"CMS_FEAR",CMS_FEAR);
  tolua_constant(tolua_S,"CMS_DIE",CMS_DIE);
  tolua_constant(tolua_S,"CMS_CLIENT_SHOW_TIME",CMS_CLIENT_SHOW_TIME);
  tolua_constant(tolua_S,"CMS_STALL",CMS_STALL);
  tolua_constant(tolua_S,"CMS_TOTAL",CMS_TOTAL);
  tolua_constant(tolua_S,"cspsInvalid",cspsInvalid);
  tolua_constant(tolua_S,"cspsIdle",cspsIdle);
  tolua_constant(tolua_S,"cspsCastingBegin",cspsCastingBegin);
  tolua_constant(tolua_S,"cspsChanneling",cspsChanneling);
  tolua_constant(tolua_S,"cspsPreparing",cspsPreparing);
  tolua_constant(tolua_S,"cspsPreAccumulating",cspsPreAccumulating);
  tolua_constant(tolua_S,"cspsAccumulating",cspsAccumulating);
  tolua_constant(tolua_S,"cspsBreaking",cspsBreaking);
  tolua_constant(tolua_S,"cspsOpening",cspsOpening);
  tolua_constant(tolua_S,"cspsCastingEnd",cspsCastingEnd);
  tolua_constant(tolua_S,"cspsHard",cspsHard);
  tolua_constant(tolua_S,"cspsTotal",cspsTotal);
  tolua_constant(tolua_S,"cstInvalid",cstInvalid);
  tolua_constant(tolua_S,"cstMale",cstMale);
  tolua_constant(tolua_S,"cstFemale",cstFemale);
  tolua_constant(tolua_S,"cstAsexual",cstAsexual);
  tolua_constant(tolua_S,"cstTotal",cstTotal);
  tolua_constant(tolua_S,"clsInvalid",clsInvalid);
  tolua_constant(tolua_S,"clsPlayerBegin",clsPlayerBegin);
  tolua_constant(tolua_S,"clsWarrior",clsWarrior);
  tolua_constant(tolua_S,"clsMage",clsMage);
  tolua_constant(tolua_S,"clsRogue",clsRogue);
  tolua_constant(tolua_S,"clsArcher",clsArcher);
  tolua_constant(tolua_S,"clsTaoist",clsTaoist);
  tolua_constant(tolua_S,"clsPlayerEnd",clsPlayerEnd);
  tolua_constant(tolua_S,"clsPlayerTotal",clsPlayerTotal);
  tolua_constant(tolua_S,"clsNpc",clsNpc);
  tolua_constant(tolua_S,"clsTotal",clsTotal);
  tolua_constant(tolua_S,"spcInvalid",spcInvalid);
  tolua_constant(tolua_S,"spcNone",spcNone);
  tolua_constant(tolua_S,"spcPlayer",spcPlayer);
  tolua_constant(tolua_S,"spcHuman",spcHuman);
  tolua_constant(tolua_S,"spcHumanlike",spcHumanlike);
  tolua_constant(tolua_S,"spcBeast",spcBeast);
  tolua_constant(tolua_S,"spcInsect",spcInsect);
  tolua_constant(tolua_S,"spcPlant",spcPlant);
  tolua_constant(tolua_S,"spcUndead",spcUndead);
  tolua_constant(tolua_S,"spcMachine",spcMachine);
  tolua_constant(tolua_S,"spcDragon",spcDragon);
  tolua_constant(tolua_S,"spcDemon",spcDemon);
  tolua_constant(tolua_S,"spcElements",spcElements);
  tolua_constant(tolua_S,"spcBuilding",spcBuilding);
  tolua_constant(tolua_S,"spcTotal",spcTotal);
  tolua_constant(tolua_S,"cclInvalid",cclInvalid);
  tolua_constant(tolua_S,"cclBegin",cclBegin);
  tolua_constant(tolua_S,"cclStun",cclStun);
  tolua_constant(tolua_S,"cclFear",cclFear);
  tolua_constant(tolua_S,"cclFury",cclFury);
  tolua_constant(tolua_S,"cclDazzle",cclDazzle);
  tolua_constant(tolua_S,"cclSlowed",cclSlowed);
  tolua_constant(tolua_S,"cclKnockBack",cclKnockBack);
  tolua_constant(tolua_S,"cclFreeze",cclFreeze);
  tolua_constant(tolua_S,"cclSeduced",cclSeduced);
  tolua_constant(tolua_S,"cclInvincible",cclInvincible);
  tolua_constant(tolua_S,"cclLeech",cclLeech);
  tolua_constant(tolua_S,"cclTransportationRing",cclTransportationRing);
  tolua_constant(tolua_S,"cclProbeNecklace",cclProbeNecklace);
  tolua_constant(tolua_S,"cclMemoSuit",cclMemoSuit);
  tolua_constant(tolua_S,"cclInvisible",cclInvisible);
  tolua_constant(tolua_S,"cclTotal",cclTotal);
  tolua_constant(tolua_S,"cfInvalid",cfInvalid);
  tolua_constant(tolua_S,"cfNotInCombat",cfNotInCombat);
  tolua_constant(tolua_S,"cfInCombat",cfInCombat);
  tolua_constant(tolua_S,"cfCounter",cfCounter);
  tolua_constant(tolua_S,"cfOnMount",cfOnMount);
  tolua_constant(tolua_S,"cfCannotBeThrown",cfCannotBeThrown);
  tolua_constant(tolua_S,"cfTempUndead",cfTempUndead);
  tolua_constant(tolua_S,"cfImmunePhyAtk",cfImmunePhyAtk);
  tolua_constant(tolua_S,"cfImmuleMagAtk",cfImmuleMagAtk);
  tolua_constant(tolua_S,"cfCanNotBeSelectedAsTarget",cfCanNotBeSelectedAsTarget);
  tolua_constant(tolua_S,"cfBTDenyDie",cfBTDenyDie);
  tolua_constant(tolua_S,"cfBTHasFlee",cfBTHasFlee);
  tolua_constant(tolua_S,"cfAuraInvalid",cfAuraInvalid);
  tolua_constant(tolua_S,"cfCombatPoseOn",cfCombatPoseOn);
  tolua_constant(tolua_S,"cfImmunePassiveMove",cfImmunePassiveMove);
  tolua_constant(tolua_S,"cfFury",cfFury);
  tolua_constant(tolua_S,"cfDamageStatistics",cfDamageStatistics);
  tolua_constant(tolua_S,"cfNeverBeHit",cfNeverBeHit);
  tolua_constant(tolua_S,"cfTotal",cfTotal);
  tolua_constant(tolua_S,"csfInvalid",csfInvalid);
  tolua_constant(tolua_S,"csfStand",csfStand);
  tolua_constant(tolua_S,"csfUp",csfUp);
  tolua_constant(tolua_S,"csfDown",csfDown);
  tolua_constant(tolua_S,"csfTotal",csfTotal);
  tolua_constant(tolua_S,"cstfBegin",cstfBegin);
  tolua_constant(tolua_S,"cstfFreeze",cstfFreeze);
  tolua_constant(tolua_S,"cstfKnockBack",cstfKnockBack);
  tolua_constant(tolua_S,"cstfSilent",cstfSilent);
  tolua_constant(tolua_S,"cstfTotal",cstfTotal);
  tolua_constant(tolua_S,"cdrInvalid",cdrInvalid);
  tolua_constant(tolua_S,"cdrDefault",cdrDefault);
  tolua_constant(tolua_S,"cdrSpell",cdrSpell);
  tolua_constant(tolua_S,"cdrAura",cdrAura);
  tolua_constant(tolua_S,"cdrRecovery",cdrRecovery);
  tolua_constant(tolua_S,"cdrTimeToDie",cdrTimeToDie);
  tolua_constant(tolua_S,"cdrAIBehavior",cdrAIBehavior);
  tolua_constant(tolua_S,"cdrSilent",cdrSilent);
  tolua_constant(tolua_S,"cdrLeech",cdrLeech);
  tolua_constant(tolua_S,"cdrSuicide",cdrSuicide);
  tolua_constant(tolua_S,"cdrCount",cdrCount);
  tolua_constant(tolua_S,"crtInvalid",crtInvalid);
  tolua_constant(tolua_S,"crtAtCurrentPos",crtAtCurrentPos);
  tolua_constant(tolua_S,"crtInstant",crtInstant);
  tolua_constant(tolua_S,"crtFree",crtFree);
  tolua_constant(tolua_S,"crtTotal",crtTotal);
  tolua_constant(tolua_S,"soatInvalid",soatInvalid);
  tolua_constant(tolua_S,"soatLogin",soatLogin);
  tolua_constant(tolua_S,"soatInRange",soatInRange);
  tolua_constant(tolua_S,"soatSpawn",soatSpawn);
  tolua_constant(tolua_S,"soatRevive",soatRevive);
  tolua_constant(tolua_S,"soatNoPerforSpawn",soatNoPerforSpawn);
  tolua_constant(tolua_S,"soatTotal",soatTotal);
  tolua_constant(tolua_S,"sodtInvalid",sodtInvalid);
  tolua_constant(tolua_S,"sodtLeaveScene",sodtLeaveScene);
  tolua_constant(tolua_S,"sodtOutOfRange",sodtOutOfRange);
  tolua_constant(tolua_S,"sodtTotal",sodtTotal);
  tolua_constant(tolua_S,"escsInvalid",escsInvalid);
  tolua_constant(tolua_S,"escsDeathCount",escsDeathCount);
  tolua_constant(tolua_S,"escsRoleKillCount",escsRoleKillCount);
  tolua_constant(tolua_S,"escsNpcKillCount",escsNpcKillCount);
  tolua_constant(tolua_S,"escsEliteNpcKillCount",escsEliteNpcKillCount);
  tolua_constant(tolua_S,"escsBossKillCount",escsBossKillCount);
  tolua_constant(tolua_S,"escsDoodadKillCount",escsDoodadKillCount);
  tolua_constant(tolua_S,"escsTotalHeal",escsTotalHeal);
  tolua_constant(tolua_S,"escsTotalDamage",escsTotalDamage);
  tolua_constant(tolua_S,"escsPhyAtkCritCount",escsPhyAtkCritCount);
  tolua_constant(tolua_S,"escsMagAtkCritCount",escsMagAtkCritCount);
  tolua_constant(tolua_S,"escsScore1",escsScore1);
  tolua_constant(tolua_S,"escsScore2",escsScore2);
  tolua_constant(tolua_S,"escsTotalScore",escsTotalScore);
  tolua_constant(tolua_S,"escsTotal",escsTotal);
  tolua_constant(tolua_S,"rrrNormal",rrrNormal);
  tolua_constant(tolua_S,"rrrAdvanced",rrrAdvanced);
  tolua_constant(tolua_S,"mmtInvalid",mmtInvalid);
  tolua_constant(tolua_S,"mmtLine",mmtLine);
  tolua_constant(tolua_S,"mmtAccordingToTerrain",mmtAccordingToTerrain);
  tolua_constant(tolua_S,"mmtStatic",mmtStatic);
  tolua_constant(tolua_S,"mmtLockTarget",mmtLockTarget);
  tolua_constant(tolua_S,"mmtTotal",mmtTotal);
  tolua_constant(tolua_S,"mcgRole",mcgRole);
  tolua_constant(tolua_S,"mcgNpc",mcgNpc);
  tolua_constant(tolua_S,"mcgDoodad",mcgDoodad);
  tolua_constant(tolua_S,"mltInvalid",mltInvalid);
  tolua_constant(tolua_S,"mltSelf",mltSelf);
  tolua_constant(tolua_S,"mltObject",mltObject);
  tolua_constant(tolua_S,"mltPos",mltPos);
  tolua_constant(tolua_S,"mltTotal",mltTotal);
 tolua_constant(tolua_S,"GRID_LAYER_HEIGHT_BIT",GRID_LAYER_HEIGHT_BIT);
 tolua_constant(tolua_S,"NEXT_GRID_BIT",NEXT_GRID_BIT);
 tolua_constant(tolua_S,"MAX_NEXT_GRID",MAX_NEXT_GRID);
 tolua_constant(tolua_S,"GRID_VOLUME_BIT",GRID_VOLUME_BIT);
 tolua_constant(tolua_S,"GRID_DOOR_BIT",GRID_DOOR_BIT);
 tolua_constant(tolua_S,"EDGE_INDEX_BIT",EDGE_INDEX_BIT);
 tolua_constant(tolua_S,"MAX_DOOR_PER_ZONE",MAX_DOOR_PER_ZONE);
 tolua_constant(tolua_S,"MAX_VOLUME_PER_ZONE",MAX_VOLUME_PER_ZONE);
 tolua_constant(tolua_S,"MAX_EDGE_INDEX_PER_ZONE",MAX_EDGE_INDEX_PER_ZONE);
 tolua_constant(tolua_S,"MAX_CHAR_PER_GRID",MAX_CHAR_PER_GRID);
 tolua_constant(tolua_S,"MAX_DROP_PER_GRID",MAX_DROP_PER_GRID);
 tolua_constant(tolua_S,"MAX_QUEST_PER_ZONE",MAX_QUEST_PER_ZONE);
  tolua_constant(tolua_S,"gptInvalid",gptInvalid);
  tolua_constant(tolua_S,"gptNormal",gptNormal);
  tolua_constant(tolua_S,"gptFree",gptFree);
  tolua_constant(tolua_S,"gptSafe",gptSafe);
  tolua_constant(tolua_S,"gptTotal",gptTotal);
 tolua_cclass(tolua_S,"GRID_DATA","GRID_DATA","",NULL);
 tolua_beginmodule(tolua_S,"GRID_DATA");
  tolua_variable(tolua_S,"uNextGrid",tolua_get_GRID_DATA_uNextGrid,tolua_set_GRID_DATA_uNextGrid);
  tolua_variable(tolua_S,"uNoObstacle",tolua_get_GRID_DATA_uNoObstacle,tolua_set_GRID_DATA_uNoObstacle);
  tolua_variable(tolua_S,"uVolume",tolua_get_GRID_DATA_uVolume,tolua_set_GRID_DATA_uVolume);
  tolua_variable(tolua_S,"uDoor",tolua_get_GRID_DATA_uDoor,tolua_set_GRID_DATA_uDoor);
  tolua_variable(tolua_S,"uPkType",tolua_get_GRID_DATA_uPkType,tolua_set_GRID_DATA_uPkType);
  tolua_variable(tolua_S,"bBorder",tolua_get_GRID_DATA_bBorder,tolua_set_GRID_DATA_bBorder);
  tolua_variable(tolua_S,"uStallLevel",tolua_get_GRID_DATA_uStallLevel,tolua_set_GRID_DATA_uStallLevel);
  tolua_variable(tolua_S,"bQuest",tolua_get_GRID_DATA_bQuest,tolua_set_GRID_DATA_bQuest);
  tolua_variable(tolua_S,"bBlockMove",tolua_get_GRID_DATA_bBlockMove,tolua_set_GRID_DATA_bBlockMove);
  tolua_variable(tolua_S,"bBlockLeap",tolua_get_GRID_DATA_bBlockLeap,tolua_set_GRID_DATA_bBlockLeap);
  tolua_variable(tolua_S,"bBlockMissile",tolua_get_GRID_DATA_bBlockMissile,tolua_set_GRID_DATA_bBlockMissile);
  tolua_variable(tolua_S,"bBlockBullet",tolua_get_GRID_DATA_bBlockBullet,tolua_set_GRID_DATA_bBlockBullet);
  tolua_variable(tolua_S,"bNotBlockChar",tolua_get_GRID_DATA_bNotBlockChar,tolua_set_GRID_DATA_bNotBlockChar);
  tolua_variable(tolua_S,"bMine",tolua_get_GRID_DATA_bMine,tolua_set_GRID_DATA_bMine);
  tolua_variable(tolua_S,"bCannotTeleport",tolua_get_GRID_DATA_bCannotTeleport,tolua_set_GRID_DATA_bCannotTeleport);
  tolua_variable(tolua_S,"bRoleCannotLoseItem",tolua_get_GRID_DATA_bRoleCannotLoseItem,tolua_set_GRID_DATA_bRoleCannotLoseItem);
  tolua_variable(tolua_S,"bRoleCannotDropItem",tolua_get_GRID_DATA_bRoleCannotDropItem,tolua_set_GRID_DATA_bRoleCannotDropItem);
  tolua_variable(tolua_S,"bSlayerArea",tolua_get_GRID_DATA_bSlayerArea,tolua_set_GRID_DATA_bSlayerArea);
  tolua_variable(tolua_S,"bCannotDropItem",tolua_get_GRID_DATA_bCannotDropItem,tolua_set_GRID_DATA_bCannotDropItem);
  tolua_variable(tolua_S,"uMineLevel",tolua_get_GRID_DATA_uMineLevel,tolua_set_GRID_DATA_uMineLevel);
  tolua_variable(tolua_S,"bTeleportPoint",tolua_get_GRID_DATA_bTeleportPoint,tolua_set_GRID_DATA_bTeleportPoint);
  tolua_variable(tolua_S,"uReserved1",tolua_get_GRID_DATA_uReserved1,tolua_set_GRID_DATA_uReserved1);
  tolua_variable(tolua_S,"uReserved2",tolua_get_GRID_DATA_uReserved2,tolua_set_GRID_DATA_uReserved2);
  tolua_variable(tolua_S,"uLowLayer",tolua_get_GRID_DATA_uLowLayer,tolua_set_GRID_DATA_uLowLayer);
  tolua_variable(tolua_S,"uHighLayer",tolua_get_GRID_DATA_uHighLayer,tolua_set_GRID_DATA_uHighLayer);
  tolua_variable(tolua_S,"uEdge",tolua_get_GRID_DATA_uEdge,tolua_set_GRID_DATA_uEdge);
  tolua_variable(tolua_S,"bOutOfPoly",tolua_get_GRID_DATA_bOutOfPoly,tolua_set_GRID_DATA_bOutOfPoly);
  tolua_variable(tolua_S,"bInServerPointRange",tolua_get_GRID_DATA_bInServerPointRange,tolua_set_GRID_DATA_bInServerPointRange);
  tolua_variable(tolua_S,"uReserved3",tolua_get_GRID_DATA_uReserved3,tolua_set_GRID_DATA_uReserved3);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"GRID_INSTANCE_DATA","GRID_INSTANCE_DATA","",NULL);
 tolua_beginmodule(tolua_S,"GRID_INSTANCE_DATA");
  tolua_variable(tolua_S,"uCharRef",tolua_get_GRID_INSTANCE_DATA_unsigned_uCharRef,tolua_set_GRID_INSTANCE_DATA_unsigned_uCharRef);
  tolua_variable(tolua_S,"uDropRef",tolua_get_GRID_INSTANCE_DATA_unsigned_uDropRef,tolua_set_GRID_INSTANCE_DATA_unsigned_uDropRef);
  tolua_variable(tolua_S,"uMissileTag1",tolua_get_GRID_INSTANCE_DATA_unsigned_uMissileTag1,tolua_set_GRID_INSTANCE_DATA_unsigned_uMissileTag1);
  tolua_variable(tolua_S,"uMissileTag2",tolua_get_GRID_INSTANCE_DATA_unsigned_uMissileTag2,tolua_set_GRID_INSTANCE_DATA_unsigned_uMissileTag2);
  tolua_variable(tolua_S,"uReserved",tolua_get_GRID_INSTANCE_DATA_unsigned_uReserved,tolua_set_GRID_INSTANCE_DATA_unsigned_uReserved);
 tolua_endmodule(tolua_S);
 tolua_constant(tolua_S,"POINT_PER_CM",POINT_PER_CM);
 tolua_constant(tolua_S,"POINT_PER_METER",POINT_PER_METER);
 tolua_constant(tolua_S,"GAME_FPS",GAME_FPS);
 tolua_constant(tolua_S,"GRID_WIDTH",GRID_WIDTH);
 tolua_constant(tolua_S,"GRID_LAYER_HEIGHT",GRID_LAYER_HEIGHT);
 tolua_constant(tolua_S,"ZONE_WIDTH",ZONE_WIDTH);
 tolua_constant(tolua_S,"REGION_WIDTH",REGION_WIDTH);
 tolua_constant(tolua_S,"MAX_SCENE_WIDTH",MAX_SCENE_WIDTH);
 tolua_constant(tolua_S,"MAX_REGION_PER_SCENE",MAX_REGION_PER_SCENE);
 tolua_constant(tolua_S,"MAX_ZONE_PER_SCENE",MAX_ZONE_PER_SCENE);
 tolua_constant(tolua_S,"GRID_PER_ZONE",GRID_PER_ZONE);
 tolua_constant(tolua_S,"MAX_MOVE_SPEED",MAX_MOVE_SPEED);
 tolua_constant(tolua_S,"MAX_NPC_SPAWN_POINT_PER_SMALL_GROUP",MAX_NPC_SPAWN_POINT_PER_SMALL_GROUP);
 tolua_constant(tolua_S,"MAX_NPC_SMALL_GROUP_PER_MEDIUM_GROUP",MAX_NPC_SMALL_GROUP_PER_MEDIUM_GROUP);
 tolua_constant(tolua_S,"MAX_NPC_MEDIUM_GROUP_PER_LARGE_GROUP",MAX_NPC_MEDIUM_GROUP_PER_LARGE_GROUP);
 tolua_constant(tolua_S,"MAX_NPC_MEDIUM_GROUP_COUNT",MAX_NPC_MEDIUM_GROUP_COUNT);
 tolua_constant(tolua_S,"MAX_NPC_LARGE_GROUP_COUNT",MAX_NPC_LARGE_GROUP_COUNT);
 tolua_constant(tolua_S,"MAX_NPC_TEMPLATE_COUNT",MAX_NPC_TEMPLATE_COUNT);
 tolua_constant(tolua_S,"MAX_SINGLE_NPC_COUNT",MAX_SINGLE_NPC_COUNT);
 tolua_constant(tolua_S,"MAX_DOODAD_SPAWN_GROUP_COUNT",MAX_DOODAD_SPAWN_GROUP_COUNT);
 tolua_constant(tolua_S,"MAX_DOODAD_SPAWN_POINT_COUNT",MAX_DOODAD_SPAWN_POINT_COUNT);
 tolua_constant(tolua_S,"MAX_DOODAD_COUNT_PER_GROUP",MAX_DOODAD_COUNT_PER_GROUP);
 tolua_constant(tolua_S,"MAX_SCENE_ENTER_DISTANCE",MAX_SCENE_ENTER_DISTANCE);
 tolua_constant(tolua_S,"MAX_DOOR_ID",MAX_DOOR_ID);
 tolua_constant(tolua_S,"MAX_SCENE_TEMPLATE_COUNT",MAX_SCENE_TEMPLATE_COUNT);
 tolua_constant(tolua_S,"MAX_VOLUME_COUNT",MAX_VOLUME_COUNT);
 tolua_constant(tolua_S,"MAX_RANDOM_WEATHER_COUNT",MAX_RANDOM_WEATHER_COUNT);
 tolua_constant(tolua_S,"MAX_SYNC_CHAR_COUNT",MAX_SYNC_CHAR_COUNT);
 tolua_constant(tolua_S,"MAX_SYNC_DOODAD_COUNT",MAX_SYNC_DOODAD_COUNT);
 tolua_constant(tolua_S,"MAX_VELOCITY_Z",MAX_VELOCITY_Z);
 tolua_constant(tolua_S,"GLOBAL_GRAVITY",GLOBAL_GRAVITY);
 tolua_constant(tolua_S,"SCENE_READY_TIME",SCENE_READY_TIME);
 tolua_constant(tolua_S,"MAX_SIDE_COUNT",MAX_SIDE_COUNT);
 tolua_constant(tolua_S,"MAX_FACTION_COUNT",MAX_FACTION_COUNT);
 tolua_constant(tolua_S,"ROLE_HEIGHT",ROLE_HEIGHT);
 tolua_constant(tolua_S,"SYNC_BATTLE_MEMBER_TIME",SYNC_BATTLE_MEMBER_TIME);
 tolua_constant(tolua_S,"MAX_LINE_SCENE_NUMBER",MAX_LINE_SCENE_NUMBER);
 tolua_constant(tolua_S,"DAYTIME_HOURS",DAYTIME_HOURS);
 tolua_constant(tolua_S,"DL_RECORD_ROLET_INTERVAL",DL_RECORD_ROLET_INTERVAL);
 tolua_constant(tolua_S,"MAX_MINE_DROP_GROUP",MAX_MINE_DROP_GROUP);
 tolua_constant(tolua_S,"MAX_MINE_LEVEL",MAX_MINE_LEVEL);
  tolua_constant(tolua_S,"swtInvalid",swtInvalid);
  tolua_constant(tolua_S,"swtNormal",swtNormal);
  tolua_constant(tolua_S,"swtFog",swtFog);
  tolua_constant(tolua_S,"swtThunder",swtThunder);
  tolua_constant(tolua_S,"swtRain",swtRain);
  tolua_constant(tolua_S,"swtSnow",swtSnow);
  tolua_constant(tolua_S,"swtStorm",swtStorm);
  tolua_constant(tolua_S,"swtEclipse",swtEclipse);
  tolua_constant(tolua_S,"swtTotal",swtTotal);
  tolua_constant(tolua_S,"sdtInvalid",sdtInvalid);
  tolua_constant(tolua_S,"sdtDay",sdtDay);
  tolua_constant(tolua_S,"sdtNight",sdtNight);
  tolua_constant(tolua_S,"sdtTotal",sdtTotal);
  tolua_constant(tolua_S,"estInvalid",estInvalid);
  tolua_constant(tolua_S,"estStatic",estStatic);
  tolua_constant(tolua_S,"estDungeon",estDungeon);
  tolua_constant(tolua_S,"estBattleField",estBattleField);
  tolua_constant(tolua_S,"estArena",estArena);
  tolua_constant(tolua_S,"estLine",estLine);
  tolua_constant(tolua_S,"estBirth",estBirth);
  tolua_constant(tolua_S,"estGuild",estGuild);
  tolua_constant(tolua_S,"estGuildDungeon",estGuildDungeon);
  tolua_constant(tolua_S,"estUnique",estUnique);
  tolua_constant(tolua_S,"estSoloDungeon",estSoloDungeon);
  tolua_constant(tolua_S,"estTotal",estTotal);
  tolua_constant(tolua_S,"lssInvalid",lssInvalid);
  tolua_constant(tolua_S,"lssNormal",lssNormal);
  tolua_constant(tolua_S,"lssBusy",lssBusy);
  tolua_constant(tolua_S,"lssFull",lssFull);
  tolua_constant(tolua_S,"lssTotal",lssTotal);
  tolua_constant(tolua_S,"bodyInvalid",bodyInvalid);
  tolua_constant(tolua_S,"body0x0",body0x0);
  tolua_constant(tolua_S,"body1x1",body1x1);
  tolua_constant(tolua_S,"body3x3",body3x3);
  tolua_constant(tolua_S,"bodyTotal",bodyTotal);
 tolua_cclass(tolua_S,"ABSOLUTE_SCENE_POS","ABSOLUTE_SCENE_POS","",NULL);
 tolua_beginmodule(tolua_S,"ABSOLUTE_SCENE_POS");
  tolua_variable(tolua_S,"nSceneTempID",tolua_get_ABSOLUTE_SCENE_POS_nSceneTempID,tolua_set_ABSOLUTE_SCENE_POS_nSceneTempID);
  tolua_variable(tolua_S,"nSceneID",tolua_get_ABSOLUTE_SCENE_POS_nSceneID,tolua_set_ABSOLUTE_SCENE_POS_nSceneID);
  tolua_variable(tolua_S,"nX",tolua_get_ABSOLUTE_SCENE_POS_nX,tolua_set_ABSOLUTE_SCENE_POS_nX);
  tolua_variable(tolua_S,"nY",tolua_get_ABSOLUTE_SCENE_POS_nY,tolua_set_ABSOLUTE_SCENE_POS_nY);
  tolua_variable(tolua_S,"nZ",tolua_get_ABSOLUTE_SCENE_POS_nZ,tolua_set_ABSOLUTE_SCENE_POS_nZ);
  tolua_variable(tolua_S,"nFaceDirection",tolua_get_ABSOLUTE_SCENE_POS_nFaceDirection,tolua_set_ABSOLUTE_SCENE_POS_nFaceDirection);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"ENTER_POS","ENTER_POS","",NULL);
 tolua_beginmodule(tolua_S,"ENTER_POS");
  tolua_variable(tolua_S,"nSceneTempID",tolua_get_ENTER_POS_nSceneTempID,tolua_set_ENTER_POS_nSceneTempID);
  tolua_variable(tolua_S,"nSceneID",tolua_get_ENTER_POS_nSceneID,tolua_set_ENTER_POS_nSceneID);
  tolua_variable(tolua_S,"bySide",tolua_get_ENTER_POS_unsigned_bySide,tolua_set_ENTER_POS_unsigned_bySide);
 tolua_endmodule(tolua_S);
#ifndef TOLUA_DISABLE_tolua_enum_I64_2_Double00
 tolua_function(tolua_S,"I64_2_Double",tolua_enum_I64_2_Double00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_ConvertCoordS2N00
 tolua_function(tolua_S,"ConvertCoordS2N",tolua_enum_ConvertCoordS2N00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_ConvertCoordN2S00
 tolua_function(tolua_S,"ConvertCoordN2S",tolua_enum_ConvertCoordN2S00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_ConvertCoordO2N00
 tolua_function(tolua_S,"ConvertCoordO2N",tolua_enum_ConvertCoordO2N00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_ConvertCoordN2O00
 tolua_function(tolua_S,"ConvertCoordN2O",tolua_enum_ConvertCoordN2O00);
#endif //#ifndef TOLUA_DISABLE
  tolua_constant(tolua_S,"sptInvalid",sptInvalid);
  tolua_constant(tolua_S,"sptCustom",sptCustom);
  tolua_constant(tolua_S,"sptRevive",sptRevive);
  tolua_constant(tolua_S,"sptReplay",sptReplay);
  tolua_constant(tolua_S,"sptTotal",sptTotal);
 tolua_constant(tolua_S,"RATE_MAX",RATE_MAX);
 tolua_constant(tolua_S,"MAX_ATTR_NAME_LEN",MAX_ATTR_NAME_LEN);
  tolua_constant(tolua_S,"catInvalid",catInvalid);
  tolua_constant(tolua_S,"catDefault",catDefault);
  tolua_constant(tolua_S,"catBeginExAttr",catBeginExAttr);
  tolua_constant(tolua_S,"catPhyDef",catPhyDef);
  tolua_constant(tolua_S,"catMaxPhyDef",catMaxPhyDef);
  tolua_constant(tolua_S,"catMagDef",catMagDef);
  tolua_constant(tolua_S,"catMaxMagDef",catMaxMagDef);
  tolua_constant(tolua_S,"catPhyAtk",catPhyAtk);
  tolua_constant(tolua_S,"catMaxPhyAtk",catMaxPhyAtk);
  tolua_constant(tolua_S,"catMagAtk",catMagAtk);
  tolua_constant(tolua_S,"catMaxMagAtk",catMaxMagAtk);
  tolua_constant(tolua_S,"catTaoAtk",catTaoAtk);
  tolua_constant(tolua_S,"catMaxTaoAtk",catMaxTaoAtk);
  tolua_constant(tolua_S,"catMaxHp",catMaxHp);
  tolua_constant(tolua_S,"catMaxMp",catMaxMp);
  tolua_constant(tolua_S,"catWalkSpeed",catWalkSpeed);
  tolua_constant(tolua_S,"catRunSpeed",catRunSpeed);
  tolua_constant(tolua_S,"catAccurate",catAccurate);
  tolua_constant(tolua_S,"catAgility",catAgility);
  tolua_constant(tolua_S,"catMagDodge",catMagDodge);
  tolua_constant(tolua_S,"catCrit",catCrit);
  tolua_constant(tolua_S,"catCritDmg",catCritDmg);
  tolua_constant(tolua_S,"catCritResist",catCritResist);
  tolua_constant(tolua_S,"catCritDmgResist",catCritDmgResist);
  tolua_constant(tolua_S,"catAntiPhyDef",catAntiPhyDef);
  tolua_constant(tolua_S,"catAntiMagDef",catAntiMagDef);
  tolua_constant(tolua_S,"catDrugHpRecovery",catDrugHpRecovery);
  tolua_constant(tolua_S,"catDrugMpRecovery",catDrugMpRecovery);
  tolua_constant(tolua_S,"catLuck",catLuck);
  tolua_constant(tolua_S,"catHolyDmg",catHolyDmg);
  tolua_constant(tolua_S,"catHurtEnhanced",catHurtEnhanced);
  tolua_constant(tolua_S,"catDmgResist",catDmgResist);
  tolua_constant(tolua_S,"catDmgEnhance",catDmgEnhance);
  tolua_constant(tolua_S,"catAtkSpeed",catAtkSpeed);
  tolua_constant(tolua_S,"catHpRecovery",catHpRecovery);
  tolua_constant(tolua_S,"catMpRecovery",catMpRecovery);
  tolua_constant(tolua_S,"catHpRecoveryTap",catHpRecoveryTap);
  tolua_constant(tolua_S,"catMpRecoveryTap",catMpRecoveryTap);
  tolua_constant(tolua_S,"catTapIncrease",catTapIncrease);
  tolua_constant(tolua_S,"catAtkSpeedRate",catAtkSpeedRate);
  tolua_constant(tolua_S,"catPoisonDef",catPoisonDef);
  tolua_constant(tolua_S,"catPoisonRecovery",catPoisonRecovery);
  tolua_constant(tolua_S,"catPoisonResist",catPoisonResist);
  tolua_constant(tolua_S,"catStunResist",catStunResist);
  tolua_constant(tolua_S,"catRepelResist",catRepelResist);
  tolua_constant(tolua_S,"catHurtResist",catHurtResist);
  tolua_constant(tolua_S,"catBurnResist",catBurnResist);
  tolua_constant(tolua_S,"catWeaponWeightLmt",catWeaponWeightLmt);
  tolua_constant(tolua_S,"catArmorWeightLmt",catArmorWeightLmt);
  tolua_constant(tolua_S,"catBagWeightLmt",catBagWeightLmt);
  tolua_constant(tolua_S,"catEndExAttr",catEndExAttr);
  tolua_constant(tolua_S,"catDmgAbsorb",catDmgAbsorb);
  tolua_constant(tolua_S,"catExtraDmg",catExtraDmg);
  tolua_constant(tolua_S,"catExtraHardTime",catExtraHardTime);
  tolua_constant(tolua_S,"catPHitCountDown",catPHitCountDown);
  tolua_constant(tolua_S,"catPHitNum",catPHitNum);
  tolua_constant(tolua_S,"catSpellPtsIncr",catSpellPtsIncr);
  tolua_constant(tolua_S,"catLeech",catLeech);
  tolua_constant(tolua_S,"catMpToHp",catMpToHp);
  tolua_constant(tolua_S,"catMpToHpRemain",catMpToHpRemain);
  tolua_constant(tolua_S,"catExtraThunderDmg",catExtraThunderDmg);
  tolua_constant(tolua_S,"catDrugHpSlowRecovery",catDrugHpSlowRecovery);
  tolua_constant(tolua_S,"catExpRate",catExpRate);
  tolua_constant(tolua_S,"catTotal",catTotal);
  tolua_constant(tolua_S,"satInvalid",satInvalid);
  tolua_constant(tolua_S,"satBegin",satBegin);
  tolua_constant(tolua_S,"satClass",satClass);
  tolua_constant(tolua_S,"satRequireLevel",satRequireLevel);
  tolua_constant(tolua_S,"satNextSpell",satNextSpell);
  tolua_constant(tolua_S,"satSwitchSpell",satSwitchSpell);
  tolua_constant(tolua_S,"satResetTime",satResetTime);
  tolua_constant(tolua_S,"satCacheTime",satCacheTime);
  tolua_constant(tolua_S,"satWeaponTypeR",satWeaponTypeR);
  tolua_constant(tolua_S,"satWeaponTypeL",satWeaponTypeL);
  tolua_constant(tolua_S,"satCanEquip",satCanEquip);
  tolua_constant(tolua_S,"satCastType",satCastType);
  tolua_constant(tolua_S,"satJudgeType",satJudgeType);
  tolua_constant(tolua_S,"satAttackType",satAttackType);
  tolua_constant(tolua_S,"satElementaryType",satElementaryType);
  tolua_constant(tolua_S,"satQteType",satQteType);
  tolua_constant(tolua_S,"satLockType",satLockType);
  tolua_constant(tolua_S,"satMinDistance",satMinDistance);
  tolua_constant(tolua_S,"satMaxDistance",satMaxDistance);
  tolua_constant(tolua_S,"satMinGrids",satMinGrids);
  tolua_constant(tolua_S,"satMaxGrids",satMaxGrids);
  tolua_constant(tolua_S,"satTolerantDistance",satTolerantDistance);
  tolua_constant(tolua_S,"satTolerantGrids",satTolerantGrids);
  tolua_constant(tolua_S,"satRangeType",satRangeType);
  tolua_constant(tolua_S,"satRangeParam1",satRangeParam1);
  tolua_constant(tolua_S,"satRangeParam2",satRangeParam2);
  tolua_constant(tolua_S,"satRangeParam3",satRangeParam3);
  tolua_constant(tolua_S,"satIgnoreObstacle",satIgnoreObstacle);
  tolua_constant(tolua_S,"satUseWeaponRange",satUseWeaponRange);
  tolua_constant(tolua_S,"satIgnoreTargetRadius",satIgnoreTargetRadius);
  tolua_constant(tolua_S,"satAffectedByAtkSpeed",satAffectedByAtkSpeed);
  tolua_constant(tolua_S,"satUseDPH",satUseDPH);
  tolua_constant(tolua_S,"satIgnoreDefense",satIgnoreDefense);
  tolua_constant(tolua_S,"satIgnoreClientTarget",satIgnoreClientTarget);
  tolua_constant(tolua_S,"satMaxTargetCount",satMaxTargetCount);
  tolua_constant(tolua_S,"satOffsetDirection",satOffsetDirection);
  tolua_constant(tolua_S,"satOffsetDistance",satOffsetDistance);
  tolua_constant(tolua_S,"satHOffsetGrids",satHOffsetGrids);
  tolua_constant(tolua_S,"satVOffsetGrids",satVOffsetGrids);
  tolua_constant(tolua_S,"satTargetObjType",satTargetObjType);
  tolua_constant(tolua_S,"satTargetRelationship",satTargetRelationship);
  tolua_constant(tolua_S,"satTargetSpecies",satTargetSpecies);
  tolua_constant(tolua_S,"satTargetSex",satTargetSex);
  tolua_constant(tolua_S,"satTargetDirection",satTargetDirection);
  tolua_constant(tolua_S,"satTargetStance",satTargetStance);
  tolua_constant(tolua_S,"satSelfStance",satSelfStance);
  tolua_constant(tolua_S,"satCostType",satCostType);
  tolua_constant(tolua_S,"satInitialCostValue",satInitialCostValue);
  tolua_constant(tolua_S,"satInitialCostValuePct",satInitialCostValuePct);
  tolua_constant(tolua_S,"satDurativeCostValue",satDurativeCostValue);
  tolua_constant(tolua_S,"satDurativeCostValuePct",satDurativeCostValuePct);
  tolua_constant(tolua_S,"satModifierCostValue",satModifierCostValue);
  tolua_constant(tolua_S,"satModifierCostValuePct",satModifierCostValuePct);
  tolua_constant(tolua_S,"satItemCostID",satItemCostID);
  tolua_constant(tolua_S,"satItemCostCount",satItemCostCount);
  tolua_constant(tolua_S,"satConsumeItem",satConsumeItem);
  tolua_constant(tolua_S,"satItemCostType",satItemCostType);
  tolua_constant(tolua_S,"satItemCostSubType",satItemCostSubType);
  tolua_constant(tolua_S,"satDuraCost",satDuraCost);
  tolua_constant(tolua_S,"satCoolDownTriggerType",satCoolDownTriggerType);
  tolua_constant(tolua_S,"satSelfCoolDown",satSelfCoolDown);
  tolua_constant(tolua_S,"satEventCoolDown",satEventCoolDown);
  tolua_constant(tolua_S,"satAdjustDirection",satAdjustDirection);
  tolua_constant(tolua_S,"satAffectedByWeaponSpeed",satAffectedByWeaponSpeed);
  tolua_constant(tolua_S,"satPrepareType",satPrepareType);
  tolua_constant(tolua_S,"satPrepareTimeMin",satPrepareTimeMin);
  tolua_constant(tolua_S,"satPrepareTimeMax",satPrepareTimeMax);
  tolua_constant(tolua_S,"satAccumulateTimeMax",satAccumulateTimeMax);
  tolua_constant(tolua_S,"satAccumulateSeg",satAccumulateSeg);
  tolua_constant(tolua_S,"satChannelCount",satChannelCount);
  tolua_constant(tolua_S,"satMinEffectCount",satMinEffectCount);
  tolua_constant(tolua_S,"satInterval",satInterval);
  tolua_constant(tolua_S,"satHardTime",satHardTime);
  tolua_constant(tolua_S,"satWalkDelayTime",satWalkDelayTime);
  tolua_constant(tolua_S,"satRunDelayTime",satRunDelayTime);
  tolua_constant(tolua_S,"satHitLevel",satHitLevel);
  tolua_constant(tolua_S,"satMoveBreak",satMoveBreak);
  tolua_constant(tolua_S,"satBreakMove",satBreakMove);
  tolua_constant(tolua_S,"satCanMoveInPrepare",satCanMoveInPrepare);
  tolua_constant(tolua_S,"satCanMoveInHardTime",satCanMoveInHardTime);
  tolua_constant(tolua_S,"satHitBreak",satHitBreak);
  tolua_constant(tolua_S,"satHardTimeAffectedByCharLevel",satHardTimeAffectedByCharLevel);
  tolua_constant(tolua_S,"satCancelType",satCancelType);
  tolua_constant(tolua_S,"satCancelLevel",satCancelLevel);
  tolua_constant(tolua_S,"satCancelHardTime",satCancelHardTime);
  tolua_constant(tolua_S,"satHitTag",satHitTag);
  tolua_constant(tolua_S,"satSpeed",satSpeed);
  tolua_constant(tolua_S,"satIsMixEffect",satIsMixEffect);
  tolua_constant(tolua_S,"satMixEffectPhyRate",satMixEffectPhyRate);
  tolua_constant(tolua_S,"satMixEffectMagRate",satMixEffectMagRate);
  tolua_constant(tolua_S,"satEffectPct",satEffectPct);
  tolua_constant(tolua_S,"satEffectValue",satEffectValue);
  tolua_constant(tolua_S,"satEffectValueMax",satEffectValueMax);
  tolua_constant(tolua_S,"satDefaultValue",satDefaultValue);
  tolua_constant(tolua_S,"satDefaultValueMax",satDefaultValueMax);
  tolua_constant(tolua_S,"satAffectedByNpcParam",satAffectedByNpcParam);
  tolua_constant(tolua_S,"satThreatPct",satThreatPct);
  tolua_constant(tolua_S,"satThreatValue",satThreatValue);
  tolua_constant(tolua_S,"satThreatMax",satThreatMax);
  tolua_constant(tolua_S,"satCanCastOnMount",satCanCastOnMount);
  tolua_constant(tolua_S,"satCasterDeadLimit",satCasterDeadLimit);
  tolua_constant(tolua_S,"satTargetDeadLimit",satTargetDeadLimit);
  tolua_constant(tolua_S,"satCasterLimitBits",satCasterLimitBits);
  tolua_constant(tolua_S,"satCasterCCLimitBits",satCasterCCLimitBits);
  tolua_constant(tolua_S,"satSceneTypeLimit",satSceneTypeLimit);
  tolua_constant(tolua_S,"satCasterCheckNode",satCasterCheckNode);
  tolua_constant(tolua_S,"satTargetCheckNode",satTargetCheckNode);
  tolua_constant(tolua_S,"satOnLearn",satOnLearn);
  tolua_constant(tolua_S,"satOnForget",satOnForget);
  tolua_constant(tolua_S,"satOnEquip",satOnEquip);
  tolua_constant(tolua_S,"satOnUnEquip",satOnUnEquip);
  tolua_constant(tolua_S,"satOnPrepare",satOnPrepare);
  tolua_constant(tolua_S,"satOnCast",satOnCast);
  tolua_constant(tolua_S,"satOnBreak",satOnBreak);
  tolua_constant(tolua_S,"satOnCreateBullet",satOnCreateBullet);
  tolua_constant(tolua_S,"satBeforeRoll",satBeforeRoll);
  tolua_constant(tolua_S,"satAfterRoll",satAfterRoll);
  tolua_constant(tolua_S,"satBeforeEffect",satBeforeEffect);
  tolua_constant(tolua_S,"satAfterEffect",satAfterEffect);
  tolua_constant(tolua_S,"satNeedCache",satNeedCache);
  tolua_constant(tolua_S,"satCancelStiff",satCancelStiff);
  tolua_constant(tolua_S,"satKnockBackRate",satKnockBackRate);
  tolua_constant(tolua_S,"satBaseStiffTime",satBaseStiffTime);
  tolua_constant(tolua_S,"satMonsterChaseDelayTime",satMonsterChaseDelayTime);
  tolua_constant(tolua_S,"satChainEffect",satChainEffect);
  tolua_constant(tolua_S,"satBindSlot",satBindSlot);
  tolua_constant(tolua_S,"satTrigEvent",satTrigEvent);
  tolua_constant(tolua_S,"satTotal",satTotal);
  tolua_constant(tolua_S,"aatInvalid",aatInvalid);
  tolua_constant(tolua_S,"aatCategory",aatCategory);
  tolua_constant(tolua_S,"aatType",aatType);
  tolua_constant(tolua_S,"aatGroup",aatGroup);
  tolua_constant(tolua_S,"aatDuration",aatDuration);
  tolua_constant(tolua_S,"aatDelay",aatDelay);
  tolua_constant(tolua_S,"aatInterval",aatInterval);
  tolua_constant(tolua_S,"aatSameIdStackType",aatSameIdStackType);
  tolua_constant(tolua_S,"aatDiffIdStackType",aatDiffIdStackType);
  tolua_constant(tolua_S,"aatStackTypeSameIdSameCaster",aatStackTypeSameIdSameCaster);
  tolua_constant(tolua_S,"aatStackSameTypeSameCaster",aatStackSameTypeSameCaster);
  tolua_constant(tolua_S,"aatStackSameGroupSameCaster",aatStackSameGroupSameCaster);
  tolua_constant(tolua_S,"aatStackTypeSameIdDiffCaster",aatStackTypeSameIdDiffCaster);
  tolua_constant(tolua_S,"aatStackSameTypeDiffCaster",aatStackSameTypeDiffCaster);
  tolua_constant(tolua_S,"aatStackSameGroupDiffCaster",aatStackSameGroupDiffCaster);
  tolua_constant(tolua_S,"aatTimeCover",aatTimeCover);
  tolua_constant(tolua_S,"aatStackResetTime",aatStackResetTime);
  tolua_constant(tolua_S,"aatInitStackCount",aatInitStackCount);
  tolua_constant(tolua_S,"aatMaxStackCount",aatMaxStackCount);
  tolua_constant(tolua_S,"aatStrengthLevel",aatStrengthLevel);
  tolua_constant(tolua_S,"aatCanDispel",aatCanDispel);
  tolua_constant(tolua_S,"aatCanCancel",aatCanCancel);
  tolua_constant(tolua_S,"aatDispelCount",aatDispelCount);
  tolua_constant(tolua_S,"aatTrigOnRemoveWhenDispel",aatTrigOnRemoveWhenDispel);
  tolua_constant(tolua_S,"aatTrigOnRemoveWhenTimeup",aatTrigOnRemoveWhenTimeup);
  tolua_constant(tolua_S,"aatAttackType",aatAttackType);
  tolua_constant(tolua_S,"aatJudgeType",aatJudgeType);
  tolua_constant(tolua_S,"aatElementaryType",aatElementaryType);
  tolua_constant(tolua_S,"aatEffectPctX",aatEffectPctX);
  tolua_constant(tolua_S,"aatEffectValueX",aatEffectValueX);
  tolua_constant(tolua_S,"aatEffectPct",aatEffectPct);
  tolua_constant(tolua_S,"aatEffectValue",aatEffectValue);
  tolua_constant(tolua_S,"aatThreatPct",aatThreatPct);
  tolua_constant(tolua_S,"aatThreatValue",aatThreatValue);
  tolua_constant(tolua_S,"aatThreatMax",aatThreatMax);
  tolua_constant(tolua_S,"aatRemoveWhenDead",aatRemoveWhenDead);
  tolua_constant(tolua_S,"aatSaveToDB",aatSaveToDB);
  tolua_constant(tolua_S,"aatOfflineTimeValid",aatOfflineTimeValid);
  tolua_constant(tolua_S,"aatHiddenToClient",aatHiddenToClient);
  tolua_constant(tolua_S,"aatHiddenToSelf",aatHiddenToSelf);
  tolua_constant(tolua_S,"aatControlDecay",aatControlDecay);
  tolua_constant(tolua_S,"aatEffectTimeMin",aatEffectTimeMin);
  tolua_constant(tolua_S,"aatEternal",aatEternal);
  tolua_constant(tolua_S,"aatIgnoreDefense",aatIgnoreDefense);
  tolua_constant(tolua_S,"aatIgnoreResistence",aatIgnoreResistence);
  tolua_constant(tolua_S,"aatAffectedCharAttr",aatAffectedCharAttr);
  tolua_constant(tolua_S,"aatDisplayGroup",aatDisplayGroup);
  tolua_constant(tolua_S,"aatDisplayValue",aatDisplayValue);
  tolua_constant(tolua_S,"aatResistType",aatResistType);
  tolua_constant(tolua_S,"aatEffectDecay",aatEffectDecay);
  tolua_constant(tolua_S,"aatOnAppend",aatOnAppend);
  tolua_constant(tolua_S,"aatBeforeStack",aatBeforeStack);
  tolua_constant(tolua_S,"aatAfterStack",aatAfterStack);
  tolua_constant(tolua_S,"aatOnTimeup",aatOnTimeup);
  tolua_constant(tolua_S,"aatOnDispel",aatOnDispel);
  tolua_constant(tolua_S,"aatOnRemove",aatOnRemove);
  tolua_constant(tolua_S,"aatTotal",aatTotal);
  tolua_constant(tolua_S,"matInvalid",matInvalid);
  tolua_constant(tolua_S,"matDefault",matDefault);
  tolua_constant(tolua_S,"matLockType",matLockType);
  tolua_constant(tolua_S,"matMoveType",matMoveType);
  tolua_constant(tolua_S,"matMoveSpeed",matMoveSpeed);
  tolua_constant(tolua_S,"matHeight",matHeight);
  tolua_constant(tolua_S,"matLifeTime",matLifeTime);
  tolua_constant(tolua_S,"matMaxCollisionCount",matMaxCollisionCount);
  tolua_constant(tolua_S,"matMaxTargetCountPerCollision",matMaxTargetCountPerCollision);
  tolua_constant(tolua_S,"matRangeType",matRangeType);
  tolua_constant(tolua_S,"matRangeParam1",matRangeParam1);
  tolua_constant(tolua_S,"matRangeParam2",matRangeParam2);
  tolua_constant(tolua_S,"matRangeParam3",matRangeParam3);
  tolua_constant(tolua_S,"matBlockShape",matBlockShape);
  tolua_constant(tolua_S,"matBlockParam",matBlockParam);
  tolua_constant(tolua_S,"matOffsetDirection",matOffsetDirection);
  tolua_constant(tolua_S,"matOffsetDistance",matOffsetDistance);
  tolua_constant(tolua_S,"matHOffsetGrids",matHOffsetGrids);
  tolua_constant(tolua_S,"matVOffsetGrids",matVOffsetGrids);
  tolua_constant(tolua_S,"matDelay",matDelay);
  tolua_constant(tolua_S,"matInterval",matInterval);
  tolua_constant(tolua_S,"matNotMultiEffect",matNotMultiEffect);
  tolua_constant(tolua_S,"matNotHitLastTarget",matNotHitLastTarget);
  tolua_constant(tolua_S,"matBlockType",matBlockType);
  tolua_constant(tolua_S,"matOnInit",matOnInit);
  tolua_constant(tolua_S,"matOnCreate",matOnCreate);
  tolua_constant(tolua_S,"matOnScan",matOnScan);
  tolua_constant(tolua_S,"matOnTimeup",matOnTimeup);
  tolua_constant(tolua_S,"matOnRemove",matOnRemove);
  tolua_constant(tolua_S,"matOnCharEnter",matOnCharEnter);
  tolua_constant(tolua_S,"matOnCharLeave",matOnCharLeave);
  tolua_constant(tolua_S,"matTotal",matTotal);
  tolua_constant(tolua_S,"mxtUseNone",mxtUseNone);
  tolua_constant(tolua_S,"mxtUseX1",mxtUseX1);
  tolua_constant(tolua_S,"mxtUseX2",mxtUseX2);
  tolua_constant(tolua_S,"mxtUseX3",mxtUseX3);
  tolua_constant(tolua_S,"mxtUseX4",mxtUseX4);
  tolua_constant(tolua_S,"mxtTotal",mxtTotal);
  tolua_constant(tolua_S,"mttInvalid",mttInvalid);
  tolua_constant(tolua_S,"mttCharacter",mttCharacter);
  tolua_constant(tolua_S,"mttBullet",mttBullet);
  tolua_constant(tolua_S,"mttMissile",mttMissile);
  tolua_constant(tolua_S,"mttTotal",mttTotal);
  tolua_constant(tolua_S,"mrcError",mrcError);
  tolua_constant(tolua_S,"mrcFalse",mrcFalse);
  tolua_constant(tolua_S,"mrcTrue",mrcTrue);
  tolua_constant(tolua_S,"mrcTrueAndBreak",mrcTrueAndBreak);
  tolua_constant(tolua_S,"meoInvalid",meoInvalid);
  tolua_constant(tolua_S,"meoBulletEventBegin",meoBulletEventBegin);
  tolua_constant(tolua_S,"meoOnBulletCreate",meoOnBulletCreate);
  tolua_constant(tolua_S,"meoBeforeRoll",meoBeforeRoll);
  tolua_constant(tolua_S,"meoAfterRollAsCaster",meoAfterRollAsCaster);
  tolua_constant(tolua_S,"meoAfterRollAsTarget",meoAfterRollAsTarget);
  tolua_constant(tolua_S,"meoOnBulletHitAsCaster",meoOnBulletHitAsCaster);
  tolua_constant(tolua_S,"meoOnBulletHitAsTarget",meoOnBulletHitAsTarget);
  tolua_constant(tolua_S,"meoAfterApplyDefenceAsCaster",meoAfterApplyDefenceAsCaster);
  tolua_constant(tolua_S,"meoAfterApplyDefenceAsTarget",meoAfterApplyDefenceAsTarget);
  tolua_constant(tolua_S,"meoBeforeDamageEnhancedAsCaster",meoBeforeDamageEnhancedAsCaster);
  tolua_constant(tolua_S,"meoBeforeDamageEnhancedAsTarget",meoBeforeDamageEnhancedAsTarget);
  tolua_constant(tolua_S,"meoAfterDamageAsCaster",meoAfterDamageAsCaster);
  tolua_constant(tolua_S,"meoAfterDamageAsTarget",meoAfterDamageAsTarget);
  tolua_constant(tolua_S,"meoBeforeApplyDamageAsTarget",meoBeforeApplyDamageAsTarget);
  tolua_constant(tolua_S,"meoBulletEventEnd",meoBulletEventEnd);
  tolua_constant(tolua_S,"meoCharEventBegin",meoCharEventBegin);
  tolua_constant(tolua_S,"meoOnCharCast",meoOnCharCast);
  tolua_constant(tolua_S,"meoOnCharCastSuccess",meoOnCharCastSuccess);
  tolua_constant(tolua_S,"meoOnCharPreCast1",meoOnCharPreCast1);
  tolua_constant(tolua_S,"meoOnCharPreCast2",meoOnCharPreCast2);
  tolua_constant(tolua_S,"meoOnCharPreCast3",meoOnCharPreCast3);
  tolua_constant(tolua_S,"meoOnCharPreCast4",meoOnCharPreCast4);
  tolua_constant(tolua_S,"meoOnCharPreCast5",meoOnCharPreCast5);
  tolua_constant(tolua_S,"meoOnTimeup",meoOnTimeup);
  tolua_constant(tolua_S,"meoOnAppendAura",meoOnAppendAura);
  tolua_constant(tolua_S,"meoOnRemoveAura",meoOnRemoveAura);
  tolua_constant(tolua_S,"meoOnEquipItem",meoOnEquipItem);
  tolua_constant(tolua_S,"meoOnAuraTimeup",meoOnAuraTimeup);
  tolua_constant(tolua_S,"meoOnAuraEffect",meoOnAuraEffect);
  tolua_constant(tolua_S,"meoOnAppendAuraAsCaster",meoOnAppendAuraAsCaster);
  tolua_constant(tolua_S,"meoOnDying",meoOnDying);
  tolua_constant(tolua_S,"meoCharEventEnd",meoCharEventEnd);
  tolua_constant(tolua_S,"meoTotal",meoTotal);
  tolua_constant(tolua_S,"cmiInvalid",cmiInvalid);
  tolua_constant(tolua_S,"cmi_A",cmi_A);
  tolua_constant(tolua_S,"cmi_B",cmi_B);
  tolua_constant(tolua_S,"cmi_003",cmi_003);
  tolua_constant(tolua_S,"cmi_004",cmi_004);
  tolua_constant(tolua_S,"cmi_005",cmi_005);
  tolua_constant(tolua_S,"cmi_006",cmi_006);
  tolua_constant(tolua_S,"cmi_007",cmi_007);
  tolua_constant(tolua_S,"cmi_008",cmi_008);
  tolua_constant(tolua_S,"cmi_009",cmi_009);
  tolua_constant(tolua_S,"cmi_010",cmi_010);
  tolua_constant(tolua_S,"cmi_011",cmi_011);
  tolua_constant(tolua_S,"cmi_012",cmi_012);
  tolua_constant(tolua_S,"cmi_013",cmi_013);
  tolua_constant(tolua_S,"cmi_014",cmi_014);
  tolua_constant(tolua_S,"cmi_015",cmi_015);
  tolua_constant(tolua_S,"cmi_016",cmi_016);
  tolua_constant(tolua_S,"cmi_017",cmi_017);
  tolua_constant(tolua_S,"cmi_018",cmi_018);
  tolua_constant(tolua_S,"cmi_019",cmi_019);
  tolua_constant(tolua_S,"cmi_020",cmi_020);
  tolua_constant(tolua_S,"cmi_021",cmi_021);
  tolua_constant(tolua_S,"cmi_022",cmi_022);
  tolua_constant(tolua_S,"cmi_023",cmi_023);
  tolua_constant(tolua_S,"cmi_024",cmi_024);
  tolua_constant(tolua_S,"cmi_025",cmi_025);
  tolua_constant(tolua_S,"cmi_026",cmi_026);
  tolua_constant(tolua_S,"cmi_027",cmi_027);
  tolua_constant(tolua_S,"cmi_028",cmi_028);
  tolua_constant(tolua_S,"cmi_029",cmi_029);
  tolua_constant(tolua_S,"cmi_030",cmi_030);
  tolua_constant(tolua_S,"cmi_031",cmi_031);
  tolua_constant(tolua_S,"cmi_032",cmi_032);
  tolua_constant(tolua_S,"cmi_033",cmi_033);
  tolua_constant(tolua_S,"cmi_034",cmi_034);
  tolua_constant(tolua_S,"cmi_035",cmi_035);
  tolua_constant(tolua_S,"cmi_036",cmi_036);
  tolua_constant(tolua_S,"cmi_037",cmi_037);
  tolua_constant(tolua_S,"cmi_038",cmi_038);
  tolua_constant(tolua_S,"cmi_039",cmi_039);
  tolua_constant(tolua_S,"cmi_040",cmi_040);
  tolua_constant(tolua_S,"cmi_041",cmi_041);
  tolua_constant(tolua_S,"cmi_042",cmi_042);
  tolua_constant(tolua_S,"cmi_043",cmi_043);
  tolua_constant(tolua_S,"cmi_044",cmi_044);
  tolua_constant(tolua_S,"cmi_045",cmi_045);
  tolua_constant(tolua_S,"cmi_046",cmi_046);
  tolua_constant(tolua_S,"cmi_047",cmi_047);
  tolua_constant(tolua_S,"cmi_048",cmi_048);
  tolua_constant(tolua_S,"cmi_049",cmi_049);
  tolua_constant(tolua_S,"cmi_050",cmi_050);
  tolua_constant(tolua_S,"cmi_051",cmi_051);
  tolua_constant(tolua_S,"cmi_052",cmi_052);
  tolua_constant(tolua_S,"cmi_053",cmi_053);
  tolua_constant(tolua_S,"cmi_054",cmi_054);
  tolua_constant(tolua_S,"cmi_055",cmi_055);
  tolua_constant(tolua_S,"cmi_056",cmi_056);
  tolua_constant(tolua_S,"cmi_057",cmi_057);
  tolua_constant(tolua_S,"cmi_058",cmi_058);
  tolua_constant(tolua_S,"cmi_059",cmi_059);
  tolua_constant(tolua_S,"cmi_060",cmi_060);
  tolua_constant(tolua_S,"cmi_061",cmi_061);
  tolua_constant(tolua_S,"cmi_062",cmi_062);
  tolua_constant(tolua_S,"cmi_063",cmi_063);
  tolua_constant(tolua_S,"cmi_064",cmi_064);
  tolua_constant(tolua_S,"cmi_065",cmi_065);
  tolua_constant(tolua_S,"cmi_066",cmi_066);
  tolua_constant(tolua_S,"cmi_067",cmi_067);
  tolua_constant(tolua_S,"cmi_068",cmi_068);
  tolua_constant(tolua_S,"cmi_069",cmi_069);
  tolua_constant(tolua_S,"cmi_070",cmi_070);
  tolua_constant(tolua_S,"cmi_071",cmi_071);
  tolua_constant(tolua_S,"cmi_072",cmi_072);
  tolua_constant(tolua_S,"cmi_073",cmi_073);
  tolua_constant(tolua_S,"cmi_074",cmi_074);
  tolua_constant(tolua_S,"cmi_075",cmi_075);
  tolua_constant(tolua_S,"cmi_076",cmi_076);
  tolua_constant(tolua_S,"cmi_077",cmi_077);
  tolua_constant(tolua_S,"cmi_078",cmi_078);
  tolua_constant(tolua_S,"cmi_079",cmi_079);
  tolua_constant(tolua_S,"cmi_080",cmi_080);
  tolua_constant(tolua_S,"cmi_081",cmi_081);
  tolua_constant(tolua_S,"cmi_082",cmi_082);
  tolua_constant(tolua_S,"cmi_083",cmi_083);
  tolua_constant(tolua_S,"cmi_084",cmi_084);
  tolua_constant(tolua_S,"cmi_085",cmi_085);
  tolua_constant(tolua_S,"cmi_086",cmi_086);
  tolua_constant(tolua_S,"cmi_087",cmi_087);
  tolua_constant(tolua_S,"cmi_088",cmi_088);
  tolua_constant(tolua_S,"cmi_089",cmi_089);
  tolua_constant(tolua_S,"cmi_090",cmi_090);
  tolua_constant(tolua_S,"cmi_091",cmi_091);
  tolua_constant(tolua_S,"cmi_092",cmi_092);
  tolua_constant(tolua_S,"cmi_093",cmi_093);
  tolua_constant(tolua_S,"cmi_094",cmi_094);
  tolua_constant(tolua_S,"cmi_095",cmi_095);
  tolua_constant(tolua_S,"cmi_096",cmi_096);
  tolua_constant(tolua_S,"cmi_097",cmi_097);
  tolua_constant(tolua_S,"cmi_098",cmi_098);
  tolua_constant(tolua_S,"cmi_099",cmi_099);
  tolua_constant(tolua_S,"cmi_100",cmi_100);
  tolua_constant(tolua_S,"cmi_101",cmi_101);
  tolua_constant(tolua_S,"cmi_102",cmi_102);
  tolua_constant(tolua_S,"cmi_103",cmi_103);
  tolua_constant(tolua_S,"cmi_104",cmi_104);
  tolua_constant(tolua_S,"cmi_105",cmi_105);
  tolua_constant(tolua_S,"cmi_106",cmi_106);
  tolua_constant(tolua_S,"cmi_107",cmi_107);
  tolua_constant(tolua_S,"cmi_108",cmi_108);
  tolua_constant(tolua_S,"cmi_109",cmi_109);
  tolua_constant(tolua_S,"cmi_110",cmi_110);
  tolua_constant(tolua_S,"cmi_111",cmi_111);
  tolua_constant(tolua_S,"cmi_112",cmi_112);
  tolua_constant(tolua_S,"cmi_113",cmi_113);
  tolua_constant(tolua_S,"cmi_114",cmi_114);
  tolua_constant(tolua_S,"cmi_115",cmi_115);
  tolua_constant(tolua_S,"cmi_116",cmi_116);
  tolua_constant(tolua_S,"cmi_117",cmi_117);
  tolua_constant(tolua_S,"cmi_118",cmi_118);
  tolua_constant(tolua_S,"cmi_119",cmi_119);
  tolua_constant(tolua_S,"cmi_120",cmi_120);
  tolua_constant(tolua_S,"cmi_121",cmi_121);
  tolua_constant(tolua_S,"cmi_122",cmi_122);
  tolua_constant(tolua_S,"cmi_123",cmi_123);
  tolua_constant(tolua_S,"cmi_124",cmi_124);
  tolua_constant(tolua_S,"cmi_125",cmi_125);
  tolua_constant(tolua_S,"cmi_126",cmi_126);
  tolua_constant(tolua_S,"cmi_127",cmi_127);
  tolua_constant(tolua_S,"cmi_128",cmi_128);
  tolua_constant(tolua_S,"cmi_129",cmi_129);
  tolua_constant(tolua_S,"cmi_130",cmi_130);
  tolua_constant(tolua_S,"cmi_131",cmi_131);
  tolua_constant(tolua_S,"cmi_132",cmi_132);
  tolua_constant(tolua_S,"cmi_133",cmi_133);
  tolua_constant(tolua_S,"cmi_134",cmi_134);
  tolua_constant(tolua_S,"cmi_135",cmi_135);
  tolua_constant(tolua_S,"cmi_136",cmi_136);
  tolua_constant(tolua_S,"cmi_AllNpcSpecies",cmi_AllNpcSpecies);
  tolua_constant(tolua_S,"cmi_138",cmi_138);
  tolua_constant(tolua_S,"cmi_139",cmi_139);
  tolua_constant(tolua_S,"cmi_140",cmi_140);
  tolua_constant(tolua_S,"cmi_141",cmi_141);
  tolua_constant(tolua_S,"cmi_142",cmi_142);
  tolua_constant(tolua_S,"cmi_143",cmi_143);
  tolua_constant(tolua_S,"cmi_144",cmi_144);
  tolua_constant(tolua_S,"cmi_145",cmi_145);
  tolua_constant(tolua_S,"cmi_146",cmi_146);
  tolua_constant(tolua_S,"cmi_147",cmi_147);
  tolua_constant(tolua_S,"cmi_148",cmi_148);
  tolua_constant(tolua_S,"cmi_149",cmi_149);
  tolua_constant(tolua_S,"cmi_150",cmi_150);
  tolua_constant(tolua_S,"cmi_151",cmi_151);
  tolua_constant(tolua_S,"cmi_152",cmi_152);
  tolua_constant(tolua_S,"cmi_153",cmi_153);
  tolua_constant(tolua_S,"cmi_154",cmi_154);
  tolua_constant(tolua_S,"cmi_155",cmi_155);
  tolua_constant(tolua_S,"cmi_156",cmi_156);
  tolua_constant(tolua_S,"cmi_157",cmi_157);
  tolua_constant(tolua_S,"cmi_158",cmi_158);
  tolua_constant(tolua_S,"cmi_159",cmi_159);
  tolua_constant(tolua_S,"cmi_160",cmi_160);
  tolua_constant(tolua_S,"cmi_161",cmi_161);
  tolua_constant(tolua_S,"cmi_162",cmi_162);
  tolua_constant(tolua_S,"cmi_163",cmi_163);
  tolua_constant(tolua_S,"cmi_164",cmi_164);
  tolua_constant(tolua_S,"cmi_165",cmi_165);
  tolua_constant(tolua_S,"cmi_166",cmi_166);
  tolua_constant(tolua_S,"cmi_167",cmi_167);
  tolua_constant(tolua_S,"cmi_168",cmi_168);
  tolua_constant(tolua_S,"cmi_169",cmi_169);
  tolua_constant(tolua_S,"cmi_170",cmi_170);
  tolua_constant(tolua_S,"cmi_171",cmi_171);
  tolua_constant(tolua_S,"cmi_172",cmi_172);
  tolua_constant(tolua_S,"cmi_173",cmi_173);
  tolua_constant(tolua_S,"cmi_174",cmi_174);
  tolua_constant(tolua_S,"cmi_175",cmi_175);
  tolua_constant(tolua_S,"cmi_176",cmi_176);
  tolua_constant(tolua_S,"cmi_177",cmi_177);
  tolua_constant(tolua_S,"cmi_178",cmi_178);
  tolua_constant(tolua_S,"cmi_179",cmi_179);
  tolua_constant(tolua_S,"cmi_180",cmi_180);
  tolua_constant(tolua_S,"cmi_181",cmi_181);
  tolua_constant(tolua_S,"cmi_182",cmi_182);
  tolua_constant(tolua_S,"cmi_183",cmi_183);
  tolua_constant(tolua_S,"cmi_184",cmi_184);
  tolua_constant(tolua_S,"cmi_185",cmi_185);
  tolua_constant(tolua_S,"cmi_186",cmi_186);
  tolua_constant(tolua_S,"cmi_187",cmi_187);
  tolua_constant(tolua_S,"cmi_188",cmi_188);
  tolua_constant(tolua_S,"cmi_189",cmi_189);
  tolua_constant(tolua_S,"cmi_190",cmi_190);
  tolua_constant(tolua_S,"cmi_191",cmi_191);
  tolua_constant(tolua_S,"cmi_192",cmi_192);
  tolua_constant(tolua_S,"cmi_193",cmi_193);
  tolua_constant(tolua_S,"cmi_194",cmi_194);
  tolua_constant(tolua_S,"cmi_195",cmi_195);
  tolua_constant(tolua_S,"cmi_196",cmi_196);
  tolua_constant(tolua_S,"cmi_197",cmi_197);
  tolua_constant(tolua_S,"cmi_198",cmi_198);
  tolua_constant(tolua_S,"cmi_199",cmi_199);
  tolua_constant(tolua_S,"cmi_200",cmi_200);
  tolua_constant(tolua_S,"cmi_201",cmi_201);
  tolua_constant(tolua_S,"cmi_202",cmi_202);
  tolua_constant(tolua_S,"cmi_203",cmi_203);
  tolua_constant(tolua_S,"cmi_204",cmi_204);
  tolua_constant(tolua_S,"cmi_205",cmi_205);
  tolua_constant(tolua_S,"cmi_206",cmi_206);
  tolua_constant(tolua_S,"cmi_207",cmi_207);
  tolua_constant(tolua_S,"cmi_208",cmi_208);
  tolua_constant(tolua_S,"cmi_209",cmi_209);
  tolua_constant(tolua_S,"cmi_210",cmi_210);
  tolua_constant(tolua_S,"cmi_211",cmi_211);
  tolua_constant(tolua_S,"cmi_212",cmi_212);
  tolua_constant(tolua_S,"cmi_213",cmi_213);
  tolua_constant(tolua_S,"cmi_214",cmi_214);
  tolua_constant(tolua_S,"cmi_215",cmi_215);
  tolua_constant(tolua_S,"cmi_216",cmi_216);
  tolua_constant(tolua_S,"cmi_217",cmi_217);
  tolua_constant(tolua_S,"cmi_218",cmi_218);
  tolua_constant(tolua_S,"cmi_219",cmi_219);
  tolua_constant(tolua_S,"cmi_220",cmi_220);
  tolua_constant(tolua_S,"cmi_221",cmi_221);
  tolua_constant(tolua_S,"cmi_222",cmi_222);
  tolua_constant(tolua_S,"cmi_223",cmi_223);
  tolua_constant(tolua_S,"cmi_224",cmi_224);
  tolua_constant(tolua_S,"cmi_225",cmi_225);
  tolua_constant(tolua_S,"cmi_226",cmi_226);
  tolua_constant(tolua_S,"cmi_227",cmi_227);
  tolua_constant(tolua_S,"cmi_228",cmi_228);
  tolua_constant(tolua_S,"cmi_229",cmi_229);
  tolua_constant(tolua_S,"cmi_230",cmi_230);
  tolua_constant(tolua_S,"cmi_231",cmi_231);
  tolua_constant(tolua_S,"cmi_232",cmi_232);
  tolua_constant(tolua_S,"cmi_233",cmi_233);
  tolua_constant(tolua_S,"cmi_234",cmi_234);
  tolua_constant(tolua_S,"cmi_235",cmi_235);
  tolua_constant(tolua_S,"cmi_236",cmi_236);
  tolua_constant(tolua_S,"cmi_237",cmi_237);
  tolua_constant(tolua_S,"cmi_238",cmi_238);
  tolua_constant(tolua_S,"cmi_239",cmi_239);
  tolua_constant(tolua_S,"cmi_240",cmi_240);
  tolua_constant(tolua_S,"cmi_241",cmi_241);
  tolua_constant(tolua_S,"cmi_242",cmi_242);
  tolua_constant(tolua_S,"cmi_243",cmi_243);
  tolua_constant(tolua_S,"cmi_244",cmi_244);
  tolua_constant(tolua_S,"cmi_245",cmi_245);
  tolua_constant(tolua_S,"cmi_246",cmi_246);
  tolua_constant(tolua_S,"cmi_247",cmi_247);
  tolua_constant(tolua_S,"cmi_248",cmi_248);
  tolua_constant(tolua_S,"cmi_249",cmi_249);
  tolua_constant(tolua_S,"cmi_250",cmi_250);
  tolua_constant(tolua_S,"cmi_251",cmi_251);
  tolua_constant(tolua_S,"cmi_252",cmi_252);
  tolua_constant(tolua_S,"cmi_253",cmi_253);
  tolua_constant(tolua_S,"cmi_254",cmi_254);
  tolua_constant(tolua_S,"cmi_255",cmi_255);
  tolua_constant(tolua_S,"cmi_256",cmi_256);
  tolua_constant(tolua_S,"cmi_257",cmi_257);
  tolua_constant(tolua_S,"cmi_258",cmi_258);
  tolua_constant(tolua_S,"cmi_259",cmi_259);
  tolua_constant(tolua_S,"cmi_260",cmi_260);
  tolua_constant(tolua_S,"cmi_261",cmi_261);
  tolua_constant(tolua_S,"cmi_262",cmi_262);
  tolua_constant(tolua_S,"cmi_263",cmi_263);
  tolua_constant(tolua_S,"cmiCount",cmiCount);
 tolua_constant(tolua_S,"MAX_SPELL_LEVEL",MAX_SPELL_LEVEL);
 tolua_constant(tolua_S,"MAX_RUNE_SLOT",MAX_RUNE_SLOT);
 tolua_constant(tolua_S,"MAX_EX_PARAM_COUNT",MAX_EX_PARAM_COUNT);
 tolua_constant(tolua_S,"MAX_SPELL_NAME",MAX_SPELL_NAME);
 tolua_constant(tolua_S,"MAX_CD_GROUP",MAX_CD_GROUP);
 tolua_constant(tolua_S,"MAX_EX_CAST",MAX_EX_CAST);
 tolua_constant(tolua_S,"MAX_SPELL_TARGET_COUNT",MAX_SPELL_TARGET_COUNT);
 tolua_constant(tolua_S,"MAX_SPELL_EFFECT_DISTANCE",MAX_SPELL_EFFECT_DISTANCE);
 tolua_constant(tolua_S,"MAX_SUBSPELL_DEPTH",MAX_SUBSPELL_DEPTH);
  tolua_constant(tolua_S,"qssSlotInvalid",qssSlotInvalid);
  tolua_constant(tolua_S,"qssSlotBegin",qssSlotBegin);
  tolua_constant(tolua_S,"qssNormalAtk",qssNormalAtk);
  tolua_constant(tolua_S,"qssQteBegin",qssQteBegin);
  tolua_constant(tolua_S,"qssBossQTE",qssBossQTE);
  tolua_constant(tolua_S,"qssQteEnd",qssQteEnd);
  tolua_constant(tolua_S,"qssPassiveBegin",qssPassiveBegin);
  tolua_constant(tolua_S,"qssPassive1",qssPassive1);
  tolua_constant(tolua_S,"qssPassive2",qssPassive2);
  tolua_constant(tolua_S,"qssPassive3",qssPassive3);
  tolua_constant(tolua_S,"qssPassiveEnd",qssPassiveEnd);
  tolua_constant(tolua_S,"qssActiveBegin",qssActiveBegin);
  tolua_constant(tolua_S,"qssKey0",qssKey0);
  tolua_constant(tolua_S,"qssKey1",qssKey1);
  tolua_constant(tolua_S,"qssKey2",qssKey2);
  tolua_constant(tolua_S,"qssKey3",qssKey3);
  tolua_constant(tolua_S,"qssKey4",qssKey4);
  tolua_constant(tolua_S,"qssKey5",qssKey5);
  tolua_constant(tolua_S,"qssKey6",qssKey6);
  tolua_constant(tolua_S,"qssKey7",qssKey7);
  tolua_constant(tolua_S,"qssKey8",qssKey8);
  tolua_constant(tolua_S,"qssKey9",qssKey9);
  tolua_constant(tolua_S,"qssKey10",qssKey10);
  tolua_constant(tolua_S,"qssKey11",qssKey11);
  tolua_constant(tolua_S,"qssActiveEnd",qssActiveEnd);
  tolua_constant(tolua_S,"qssSlotCount",qssSlotCount);
  tolua_constant(tolua_S,"sfbInvalid",sfbInvalid);
  tolua_constant(tolua_S,"sfbRoundTableBegin",sfbRoundTableBegin);
  tolua_constant(tolua_S,"sfbCrit",sfbCrit);
  tolua_constant(tolua_S,"sfbBlock",sfbBlock);
  tolua_constant(tolua_S,"sfbDodge",sfbDodge);
  tolua_constant(tolua_S,"sfbParry",sfbParry);
  tolua_constant(tolua_S,"sfbMiss",sfbMiss);
  tolua_constant(tolua_S,"sfbNormal",sfbNormal);
  tolua_constant(tolua_S,"sfbRoundTableEnd",sfbRoundTableEnd);
  tolua_constant(tolua_S,"sfbImmune",sfbImmune);
  tolua_constant(tolua_S,"sfbDead",sfbDead);
  tolua_constant(tolua_S,"sfbEmptyEffect",sfbEmptyEffect);
  tolua_constant(tolua_S,"sfbTotal",sfbTotal);
  tolua_constant(tolua_S,"spctInvalid",spctInvalid);
  tolua_constant(tolua_S,"spctPassive",spctPassive);
  tolua_constant(tolua_S,"spctActive",spctActive);
  tolua_constant(tolua_S,"spctSubSpell",spctSubSpell);
  tolua_constant(tolua_S,"spctDirectly",spctDirectly);
  tolua_constant(tolua_S,"spctSwitch",spctSwitch);
  tolua_constant(tolua_S,"spctTotal",spctTotal);
  tolua_constant(tolua_S,"spjtInvalid",spjtInvalid);
  tolua_constant(tolua_S,"spjtDirectly",spjtDirectly);
  tolua_constant(tolua_S,"spjtPhy",spjtPhy);
  tolua_constant(tolua_S,"spjtMag",spjtMag);
  tolua_constant(tolua_S,"spjtTotal",spjtTotal);
  tolua_constant(tolua_S,"spatInvalid",spatInvalid);
  tolua_constant(tolua_S,"spatNone",spatNone);
  tolua_constant(tolua_S,"spatPhy",spatPhy);
  tolua_constant(tolua_S,"spatMag",spatMag);
  tolua_constant(tolua_S,"spatTao",spatTao);
  tolua_constant(tolua_S,"spatTotal",spatTotal);
  tolua_constant(tolua_S,"spdtInvalid",spdtInvalid);
  tolua_constant(tolua_S,"spdtNone",spdtNone);
  tolua_constant(tolua_S,"spdtDamageTypeBegin",spdtDamageTypeBegin);
  tolua_constant(tolua_S,"spdtPhy",spdtPhy);
  tolua_constant(tolua_S,"spdtEarth",spdtEarth);
  tolua_constant(tolua_S,"spdtFire",spdtFire);
  tolua_constant(tolua_S,"spdtWater",spdtWater);
  tolua_constant(tolua_S,"spdtWind",spdtWind);
  tolua_constant(tolua_S,"spdtThunder",spdtThunder);
  tolua_constant(tolua_S,"spdtLight",spdtLight);
  tolua_constant(tolua_S,"spdtShadow",spdtShadow);
  tolua_constant(tolua_S,"spdtNature",spdtNature);
  tolua_constant(tolua_S,"spdtDamageTypeEnd",spdtDamageTypeEnd);
  tolua_constant(tolua_S,"spdtTotal",spdtTotal);
  tolua_constant(tolua_S,"sprtInvalid",sprtInvalid);
  tolua_constant(tolua_S,"sprtSingle",sprtSingle);
  tolua_constant(tolua_S,"sprtCircle",sprtCircle);
  tolua_constant(tolua_S,"sprtSector",sprtSector);
  tolua_constant(tolua_S,"sprtArc",sprtArc);
  tolua_constant(tolua_S,"sprtRectangle",sprtRectangle);
  tolua_constant(tolua_S,"sprtRearHalfCircle",sprtRearHalfCircle);
  tolua_constant(tolua_S,"sprtGridBegin",sprtGridBegin);
  tolua_constant(tolua_S,"sprtGridLine",sprtGridLine);
  tolua_constant(tolua_S,"sprtGridRect",sprtGridRect);
  tolua_constant(tolua_S,"sprtGridCross",sprtGridCross);
  tolua_constant(tolua_S,"sprtGridX",sprtGridX);
  tolua_constant(tolua_S,"sprtGridU",sprtGridU);
  tolua_constant(tolua_S,"sprtGridSingle",sprtGridSingle);
  tolua_constant(tolua_S,"sprtGridLineTransverse",sprtGridLineTransverse);
  tolua_constant(tolua_S,"sprtGridMultiLine",sprtGridMultiLine);
  tolua_constant(tolua_S,"sprtGridEnd",sprtGridEnd);
  tolua_constant(tolua_S,"sprtTotal",sprtTotal);
  tolua_constant(tolua_S,"spltInvalid",spltInvalid);
  tolua_constant(tolua_S,"spltSelf",spltSelf);
  tolua_constant(tolua_S,"spltObject",spltObject);
  tolua_constant(tolua_S,"spltDirection",spltDirection);
  tolua_constant(tolua_S,"spltPos",spltPos);
  tolua_constant(tolua_S,"spltSelfPos",spltSelfPos);
  tolua_constant(tolua_S,"spltSmartPos",spltSmartPos);
  tolua_constant(tolua_S,"spltTotal",spltTotal);
  tolua_constant(tolua_S,"spcotInvalid",spcotInvalid);
  tolua_constant(tolua_S,"spcotFury",spcotFury);
  tolua_constant(tolua_S,"spcotEnergy",spcotEnergy);
  tolua_constant(tolua_S,"spcotMana",spcotMana);
  tolua_constant(tolua_S,"spcotWhite",spcotWhite);
  tolua_constant(tolua_S,"spcotTotal",spcotTotal);
  tolua_constant(tolua_S,"sptstInvalid",sptstInvalid);
  tolua_constant(tolua_S,"sptstMale",sptstMale);
  tolua_constant(tolua_S,"sptstFemale",sptstFemale);
  tolua_constant(tolua_S,"sptstNone",sptstNone);
  tolua_constant(tolua_S,"sptptInvalid",sptptInvalid);
  tolua_constant(tolua_S,"sptptFront",sptptFront);
  tolua_constant(tolua_S,"sptptBack",sptptBack);
  tolua_constant(tolua_S,"sptrInvalid",sptrInvalid);
  tolua_constant(tolua_S,"sptrSelf",sptrSelf);
  tolua_constant(tolua_S,"sptrFriendly",sptrFriendly);
  tolua_constant(tolua_S,"sptrEnemy",sptrEnemy);
  tolua_constant(tolua_S,"sptrTotal",sptrTotal);
  tolua_constant(tolua_S,"sptotInvalid",sptotInvalid);
  tolua_constant(tolua_S,"sptotNone",sptotNone);
  tolua_constant(tolua_S,"sptotPlayer",sptotPlayer);
  tolua_constant(tolua_S,"sptotHuman",sptotHuman);
  tolua_constant(tolua_S,"sptotHumanlike",sptotHumanlike);
  tolua_constant(tolua_S,"sptotBeast",sptotBeast);
  tolua_constant(tolua_S,"sptotInsect",sptotInsect);
  tolua_constant(tolua_S,"sptotPlant",sptotPlant);
  tolua_constant(tolua_S,"sptotUndead",sptotUndead);
  tolua_constant(tolua_S,"sptotMachine",sptotMachine);
  tolua_constant(tolua_S,"sptotDragon",sptotDragon);
  tolua_constant(tolua_S,"sptotDemon",sptotDemon);
  tolua_constant(tolua_S,"sptotElements",sptotElements);
  tolua_constant(tolua_S,"sptotBuilding",sptotBuilding);
  tolua_constant(tolua_S,"sptotTotal",sptotTotal);
  tolua_constant(tolua_S,"spcdtInvalid",spcdtInvalid);
  tolua_constant(tolua_S,"spcdtOnPrepare",spcdtOnPrepare);
  tolua_constant(tolua_S,"spcdtOnCast",spcdtOnCast);
  tolua_constant(tolua_S,"spcdtTotal",spcdtTotal);
  tolua_constant(tolua_S,"sfpptInvalid",sfpptInvalid);
  tolua_constant(tolua_S,"sfpptChanneled",sfpptChanneled);
  tolua_constant(tolua_S,"sfpptPrepared",sfpptPrepared);
  tolua_constant(tolua_S,"sfpptPreparedEx",sfpptPreparedEx);
  tolua_constant(tolua_S,"sfpptInstantCast",sfpptInstantCast);
  tolua_constant(tolua_S,"sfpptAccumulated",sfpptAccumulated);
  tolua_constant(tolua_S,"sfpptTotal",sfpptTotal);
  tolua_constant(tolua_S,"spptInvalid",spptInvalid);
  tolua_constant(tolua_S,"spptChanneled",spptChanneled);
  tolua_constant(tolua_S,"spptPrepared",spptPrepared);
  tolua_constant(tolua_S,"spptAccumulated",spptAccumulated);
  tolua_constant(tolua_S,"spptTotal",spptTotal);
  tolua_constant(tolua_S,"spcltInvalid",spcltInvalid);
  tolua_constant(tolua_S,"spcltNone",spcltNone);
  tolua_constant(tolua_S,"spcltDependOnValue",spcltDependOnValue);
  tolua_constant(tolua_S,"spcltInstant",spcltInstant);
  tolua_constant(tolua_S,"spcltTotal",spcltTotal);
  tolua_constant(tolua_S,"spctagInvalid",spctagInvalid);
  tolua_constant(tolua_S,"spctagNoMove",spctagNoMove);
  tolua_constant(tolua_S,"spctagNoCast",spctagNoCast);
  tolua_constant(tolua_S,"spctagTotal",spctagTotal);
  tolua_constant(tolua_S,"sphtagInvalid",sphtagInvalid);
  tolua_constant(tolua_S,"sphtagCommon",sphtagCommon);
  tolua_constant(tolua_S,"sphtagDefence",sphtagDefence);
  tolua_constant(tolua_S,"sphtagSuperArmor",sphtagSuperArmor);
  tolua_constant(tolua_S,"sphtagInvincible",sphtagInvincible);
  tolua_constant(tolua_S,"sphtagTotal",sphtagTotal);
  tolua_constant(tolua_S,"spslInvalid",spslInvalid);
  tolua_constant(tolua_S,"spslBreakCast",spslBreakCast);
  tolua_constant(tolua_S,"spslBreakMovement",spslBreakMovement);
  tolua_constant(tolua_S,"spslKnockBack",spslKnockBack);
  tolua_constant(tolua_S,"spslTotal",spslTotal);
  tolua_constant(tolua_S,"spqteInvalid",spqteInvalid);
  tolua_constant(tolua_S,"spqteNone",spqteNone);
  tolua_constant(tolua_S,"spqteCounter",spqteCounter);
  tolua_constant(tolua_S,"spqteChase",spqteChase);
  tolua_constant(tolua_S,"spqteTerminate",spqteTerminate);
  tolua_constant(tolua_S,"spqteTotal",spqteTotal);
  tolua_constant(tolua_S,"betInvalid",betInvalid);
  tolua_constant(tolua_S,"betBegin",betBegin);
  tolua_constant(tolua_S,"betDamage",betDamage);
  tolua_constant(tolua_S,"betHeal",betHeal);
  tolua_constant(tolua_S,"betNone",betNone);
  tolua_constant(tolua_S,"betTotal",betTotal);
#ifndef TOLUA_DISABLE_tolua_enum_MakeSpellKey00
 tolua_function(tolua_S,"MakeSpellKey",tolua_enum_MakeSpellKey00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_MakeSpellProgressKey00
 tolua_function(tolua_S,"MakeSpellProgressKey",tolua_enum_MakeSpellProgressKey00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_MakeAuraKey00
 tolua_function(tolua_S,"MakeAuraKey",tolua_enum_MakeAuraKey00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_MakeAuraInstanceKey00
 tolua_function(tolua_S,"MakeAuraInstanceKey",tolua_enum_MakeAuraInstanceKey00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_GetAuraTypeFromInstanceID00
 tolua_function(tolua_S,"GetAuraTypeFromInstanceID",tolua_enum_GetAuraTypeFromInstanceID00);
#endif //#ifndef TOLUA_DISABLE
  tolua_constant(tolua_S,"COOLDOWN_INVALID",COOLDOWN_INVALID);
  tolua_constant(tolua_S,"COOLDOWN_GROUP",COOLDOWN_GROUP);
  tolua_constant(tolua_S,"COOLDOWN_SPELL",COOLDOWN_SPELL);
  tolua_constant(tolua_S,"COOLDOWN_ITEM",COOLDOWN_ITEM);
  tolua_constant(tolua_S,"COOLDOWN_QUEST",COOLDOWN_QUEST);
  tolua_constant(tolua_S,"COOLDOWN_AI",COOLDOWN_AI);
  tolua_constant(tolua_S,"COOLDOWN_SPELL_EVENT_TRIGGER",COOLDOWN_SPELL_EVENT_TRIGGER);
  tolua_constant(tolua_S,"COOLDOWN_MISC",COOLDOWN_MISC);
  tolua_constant(tolua_S,"COOLDOWN_SCRIPT_SPELL",COOLDOWN_SCRIPT_SPELL);
  tolua_constant(tolua_S,"COOLDOWN_SPECIAL_ITEM",COOLDOWN_SPECIAL_ITEM);
  tolua_constant(tolua_S,"COOLDOWN_GUILD",COOLDOWN_GUILD);
  tolua_constant(tolua_S,"COOLDOWN_TOTAL",COOLDOWN_TOTAL);
  tolua_constant(tolua_S,"COOLDOWN_MISC_INVALID",COOLDOWN_MISC_INVALID);
  tolua_constant(tolua_S,"COOLDOWN_MISC_HERO_REVIVE",COOLDOWN_MISC_HERO_REVIVE);
  tolua_constant(tolua_S,"COOLDOWN_MISC_ARREST",COOLDOWN_MISC_ARREST);
  tolua_constant(tolua_S,"COOLDOWN_MISC_TOTAL",COOLDOWN_MISC_TOTAL);
  tolua_constant(tolua_S,"COOLDOWN_SAVE_TYPE_INVALID",COOLDOWN_SAVE_TYPE_INVALID);
  tolua_constant(tolua_S,"COOLDOWN_SAVE_TYPE_OFFLINE_TIME_VALID",COOLDOWN_SAVE_TYPE_OFFLINE_TIME_VALID);
  tolua_constant(tolua_S,"COOLDOWN_SAVE_TYPE_OFFLINE_TIME_INVALID",COOLDOWN_SAVE_TYPE_OFFLINE_TIME_INVALID);
#ifndef TOLUA_DISABLE_tolua_enum_MakeCoolDownID00
 tolua_function(tolua_S,"MakeCoolDownID",tolua_enum_MakeCoolDownID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_ParseCoolDownID00
 tolua_function(tolua_S,"ParseCoolDownID",tolua_enum_ParseCoolDownID00);
#endif //#ifndef TOLUA_DISABLE
  tolua_constant(tolua_S,"ptInvalid",ptInvalid);
  tolua_constant(tolua_S,"ptNone",ptNone);
  tolua_constant(tolua_S,"ptNormal",ptNormal);
  tolua_constant(tolua_S,"ptFast",ptFast);
  tolua_constant(tolua_S,"ptTotal",ptTotal);
  tolua_constant(tolua_S,"nptInvalid",nptInvalid);
  tolua_constant(tolua_S,"nptType1",nptType1);
  tolua_constant(tolua_S,"nptType2",nptType2);
  tolua_constant(tolua_S,"nptType3",nptType3);
  tolua_constant(tolua_S,"nptCount",nptCount);
  tolua_constant(tolua_S,"rftInvalid",rftInvalid);
  tolua_constant(tolua_S,"rftFragA",rftFragA);
  tolua_constant(tolua_S,"rftFragB",rftFragB);
  tolua_constant(tolua_S,"rftFragC",rftFragC);
  tolua_constant(tolua_S,"rftFragD",rftFragD);
  tolua_constant(tolua_S,"rftFragE",rftFragE);
  tolua_constant(tolua_S,"rftCount",rftCount);
  tolua_constant(tolua_S,"rdtInvalid",rdtInvalid);
  tolua_constant(tolua_S,"rdtNever",rdtNever);
  tolua_constant(tolua_S,"rdtByUseTimes",rdtByUseTimes);
  tolua_constant(tolua_S,"rdtByDuration",rdtByDuration);
  tolua_constant(tolua_S,"rdtCount",rdtCount);
 tolua_constant(tolua_S,"MAX_AURA_EX_EVT",MAX_AURA_EX_EVT);
  tolua_constant(tolua_S,"acInvalid",acInvalid);
  tolua_constant(tolua_S,"acNone",acNone);
  tolua_constant(tolua_S,"acPositive",acPositive);
  tolua_constant(tolua_S,"acNegative",acNegative);
  tolua_constant(tolua_S,"acTotal",acTotal);
  tolua_constant(tolua_S,"auetInvalid",auetInvalid);
  tolua_constant(tolua_S,"auetNone",auetNone);
  tolua_constant(tolua_S,"auetType01",auetType01);
  tolua_constant(tolua_S,"auetType02",auetType02);
  tolua_constant(tolua_S,"auetType03",auetType03);
  tolua_constant(tolua_S,"auetType04",auetType04);
  tolua_constant(tolua_S,"auetType05",auetType05);
  tolua_constant(tolua_S,"auetType06",auetType06);
  tolua_constant(tolua_S,"auetType07",auetType07);
  tolua_constant(tolua_S,"auetType08",auetType08);
  tolua_constant(tolua_S,"auetType09",auetType09);
  tolua_constant(tolua_S,"auetType10",auetType10);
  tolua_constant(tolua_S,"auetType11",auetType11);
  tolua_constant(tolua_S,"auetType12",auetType12);
  tolua_constant(tolua_S,"auetType13",auetType13);
  tolua_constant(tolua_S,"auetType14",auetType14);
  tolua_constant(tolua_S,"auetType15",auetType15);
  tolua_constant(tolua_S,"auetType16",auetType16);
  tolua_constant(tolua_S,"auetType17",auetType17);
  tolua_constant(tolua_S,"auetType18",auetType18);
  tolua_constant(tolua_S,"auetType19",auetType19);
  tolua_constant(tolua_S,"auetType20",auetType20);
  tolua_constant(tolua_S,"auetType21",auetType21);
  tolua_constant(tolua_S,"auetType22",auetType22);
  tolua_constant(tolua_S,"auetType23",auetType23);
  tolua_constant(tolua_S,"auetType24",auetType24);
  tolua_constant(tolua_S,"auetTotal",auetTotal);
  tolua_constant(tolua_S,"austInvalid",austInvalid);
  tolua_constant(tolua_S,"austNone",austNone);
  tolua_constant(tolua_S,"austSubstitute",austSubstitute);
  tolua_constant(tolua_S,"austStack",austStack);
  tolua_constant(tolua_S,"austCoexist",austCoexist);
  tolua_constant(tolua_S,"austTotal",austTotal);
  tolua_constant(tolua_S,"adgInvalid",adgInvalid);
  tolua_constant(tolua_S,"adgBegin",adgBegin);
  tolua_constant(tolua_S,"adgGroupA",adgGroupA);
  tolua_constant(tolua_S,"adgGroupB",adgGroupB);
  tolua_constant(tolua_S,"adgGroupC",adgGroupC);
  tolua_constant(tolua_S,"adgGroupD",adgGroupD);
  tolua_constant(tolua_S,"adgGroupE",adgGroupE);
  tolua_constant(tolua_S,"adgTotal",adgTotal);
  tolua_constant(tolua_S,"aurtNone",aurtNone);
  tolua_constant(tolua_S,"aurtPoison",aurtPoison);
  tolua_constant(tolua_S,"aurtDazzy",aurtDazzy);
  tolua_constant(tolua_S,"aurtFreeze",aurtFreeze);
  tolua_constant(tolua_S,"aurtBurn",aurtBurn);
  tolua_constant(tolua_S,"aurtRepel",aurtRepel);
  tolua_constant(tolua_S,"aurtTotal",aurtTotal);
  tolua_constant(tolua_S,"sigInvalid",sigInvalid);
  tolua_constant(tolua_S,"sigSpreadHate",sigSpreadHate);
  tolua_constant(tolua_S,"sigAssistOnce",sigAssistOnce);
  tolua_constant(tolua_S,"sigAssist",sigAssist);
  tolua_constant(tolua_S,"sigHelpSpellHeal",sigHelpSpellHeal);
  tolua_constant(tolua_S,"sigHelpSpellHarm",sigHelpSpellHarm);
  tolua_constant(tolua_S,"sigUserDefine",sigUserDefine);
  tolua_constant(tolua_S,"sigEnd",sigEnd);
  tolua_constant(tolua_S,"apsInvalid",apsInvalid);
  tolua_constant(tolua_S,"apsLine",apsLine);
  tolua_constant(tolua_S,"apsLineBack",apsLineBack);
  tolua_constant(tolua_S,"apsCircle",apsCircle);
  tolua_constant(tolua_S,"apsTotal",apsTotal);
 tolua_constant(tolua_S,"MAX_CHAT_MESSAGE_LEN",MAX_CHAT_MESSAGE_LEN);
 tolua_constant(tolua_S,"CHAT_SYSTEM_ID",CHAT_SYSTEM_ID);
  tolua_constant(tolua_S,"cctInvalid",cctInvalid);
  tolua_constant(tolua_S,"cctScreen",cctScreen);
  tolua_constant(tolua_S,"cctScene",cctScene);
  tolua_constant(tolua_S,"cctGroup",cctGroup);
  tolua_constant(tolua_S,"cctServer",cctServer);
  tolua_constant(tolua_S,"cctTotal",cctTotal);
  tolua_constant(tolua_S,"cltInvalid",cltInvalid);
  tolua_constant(tolua_S,"cltNormal",cltNormal);
  tolua_constant(tolua_S,"cltLighting",cltLighting);
  tolua_constant(tolua_S,"cltTvShow",cltTvShow);
  tolua_constant(tolua_S,"cltUrl",cltUrl);
  tolua_constant(tolua_S,"cltDialog",cltDialog);
  tolua_constant(tolua_S,"cltTrumpet",cltTrumpet);
  tolua_constant(tolua_S,"cltGMTrumpet",cltGMTrumpet);
  tolua_constant(tolua_S,"cltTotal",cltTotal);
 tolua_constant(tolua_S,"MAX_FRIEND_COUNT",MAX_FRIEND_COUNT);
 tolua_constant(tolua_S,"MAX_FRIEND_GROUP_COUNT",MAX_FRIEND_GROUP_COUNT);
 tolua_constant(tolua_S,"FRIEND_GROUP_ENEMY",FRIEND_GROUP_ENEMY);
 tolua_constant(tolua_S,"FRIEND_GROUP_NAME_LEN",FRIEND_GROUP_NAME_LEN);
 tolua_constant(tolua_S,"SIGNATURE_LEN",SIGNATURE_LEN);
 tolua_constant(tolua_S,"MAX_PERSONAL_ENEMY_COUNT",MAX_PERSONAL_ENEMY_COUNT);
  tolua_constant(tolua_S,"FS_ONLINE",FS_ONLINE);
  tolua_constant(tolua_S,"FS_BUSY",FS_BUSY);
  tolua_constant(tolua_S,"FS_AFK",FS_AFK);
  tolua_constant(tolua_S,"FS_OFFLINE",FS_OFFLINE);
 tolua_constant(tolua_S,"MAX_ITEM_COUNT",MAX_ITEM_COUNT);
 tolua_constant(tolua_S,"MAX_DROP_COUNT",MAX_DROP_COUNT);
 tolua_constant(tolua_S,"MAX_REFINE_LEVEL",MAX_REFINE_LEVEL);
  tolua_constant(tolua_S,"ITEMLOG_INVALID",ITEMLOG_INVALID);
  tolua_constant(tolua_S,"ITEMLOG_SHOP",ITEMLOG_SHOP);
  tolua_constant(tolua_S,"ITEMLOG_LOOT",ITEMLOG_LOOT);
  tolua_constant(tolua_S,"ITEMLOG_GM_COMMAND",ITEMLOG_GM_COMMAND);
  tolua_constant(tolua_S,"ITEMLOG_QUEST_PREPAY",ITEMLOG_QUEST_PREPAY);
  tolua_constant(tolua_S,"ITEMLOG_QUEST_REWARD",ITEMLOG_QUEST_REWARD);
  tolua_constant(tolua_S,"ITEMLOG_DUNGEON_REWARD",ITEMLOG_DUNGEON_REWARD);
  tolua_constant(tolua_S,"ITEMLOG_PLAYER_STALL",ITEMLOG_PLAYER_STALL);
  tolua_constant(tolua_S,"ITEMLOG_PLAYER_TRADE",ITEMLOG_PLAYER_TRADE);
  tolua_constant(tolua_S,"ITEMLOG_BUY_BACK",ITEMLOG_BUY_BACK);
  tolua_constant(tolua_S,"ITEMLOG_CREATE_ROLE",ITEMLOG_CREATE_ROLE);
  tolua_constant(tolua_S,"ITEMLOG_CREATE_HERO",ITEMLOG_CREATE_HERO);
  tolua_constant(tolua_S,"ITEMLOG_SCRIPT",ITEMLOG_SCRIPT);
  tolua_constant(tolua_S,"ITEMLOG_GET_MAIL_ATTACH",ITEMLOG_GET_MAIL_ATTACH);
  tolua_constant(tolua_S,"ITEMLOG_GUILD_STORAGE",ITEMLOG_GUILD_STORAGE);
  tolua_constant(tolua_S,"ITEMLOG_GUILD_DESTORY",ITEMLOG_GUILD_DESTORY);
  tolua_constant(tolua_S,"ITEMLOG_ACHIEVEMENT_REWARD",ITEMLOG_ACHIEVEMENT_REWARD);
  tolua_constant(tolua_S,"ITEMLOG_UPGRADE_SOUL",ITEMLOG_UPGRADE_SOUL);
  tolua_constant(tolua_S,"ITEMLOG_REMOVE_SOUL",ITEMLOG_REMOVE_SOUL);
  tolua_constant(tolua_S,"ITEMLOG_NPC_DROP",ITEMLOG_NPC_DROP);
  tolua_constant(tolua_S,"ITEMLOG_USER_REQUEST",ITEMLOG_USER_REQUEST);
  tolua_constant(tolua_S,"ITEMLOG_SELL",ITEMLOG_SELL);
  tolua_constant(tolua_S,"ITEMLOG_CAST_SPELL",ITEMLOG_CAST_SPELL);
  tolua_constant(tolua_S,"ITEMLOG_REFINE_ITEM",ITEMLOG_REFINE_ITEM);
  tolua_constant(tolua_S,"ITEMLOG_COMPLETE_QUEST",ITEMLOG_COMPLETE_QUEST);
  tolua_constant(tolua_S,"ITEMLOG_CREATE_GUILD",ITEMLOG_CREATE_GUILD);
  tolua_constant(tolua_S,"ITEMLOG_USE_ITEM",ITEMLOG_USE_ITEM);
  tolua_constant(tolua_S,"ITEMLOG_INSERT_SOUL",ITEMLOG_INSERT_SOUL);
  tolua_constant(tolua_S,"ITEMLOG_AUCTION_SELL",ITEMLOG_AUCTION_SELL);
  tolua_constant(tolua_S,"ITEMLOG_RUNE",ITEMLOG_RUNE);
  tolua_constant(tolua_S,"ITEMLOG_DECOMPOSE",ITEMLOG_DECOMPOSE);
  tolua_constant(tolua_S,"ITEMLOG_COMPOSE",ITEMLOG_COMPOSE);
  tolua_constant(tolua_S,"ITEMLOG_ACTIVITY_REWARD",ITEMLOG_ACTIVITY_REWARD);
  tolua_constant(tolua_S,"ITEMLOG_REVIVE",ITEMLOG_REVIVE);
  tolua_constant(tolua_S,"ITEMLOG_GM",ITEMLOG_GM);
  tolua_constant(tolua_S,"ITEMLOG_OPEN_DOODAD",ITEMLOG_OPEN_DOODAD);
  tolua_constant(tolua_S,"ITEMLOG_REAL_MONEY_SHOP",ITEMLOG_REAL_MONEY_SHOP);
  tolua_constant(tolua_S,"ITEMLOG_TEAM_AUCTION",ITEMLOG_TEAM_AUCTION);
  tolua_constant(tolua_S,"ITEMLOG_TEAM_AUCTION_FAILED",ITEMLOG_TEAM_AUCTION_FAILED);
  tolua_constant(tolua_S,"ITEMLOG_QUEST_RECYCLE",ITEMLOG_QUEST_RECYCLE);
  tolua_constant(tolua_S,"ITEMLOG_EXCHANGE",ITEMLOG_EXCHANGE);
  tolua_constant(tolua_S,"ITEMLOG_ARENA_COMMON_REWARD",ITEMLOG_ARENA_COMMON_REWARD);
  tolua_constant(tolua_S,"ITEMLOG_SABAC_AWARD",ITEMLOG_SABAC_AWARD);
  tolua_constant(tolua_S,"ITEMLOG_REPUTATION_AWARD",ITEMLOG_REPUTATION_AWARD);
  tolua_constant(tolua_S,"ITEMLOG_SPLIT",ITEMLOG_SPLIT);
  tolua_constant(tolua_S,"ITEMLOG_MOVE",ITEMLOG_MOVE);
  tolua_constant(tolua_S,"ITEMLOG_ORGANIZE_BAG",ITEMLOG_ORGANIZE_BAG);
  tolua_constant(tolua_S,"ITEMLOG_EQUIP_LEVELUP",ITEMLOG_EQUIP_LEVELUP);
  tolua_constant(tolua_S,"ITEMLOG_EQUIP_LEVELUP_FAILED",ITEMLOG_EQUIP_LEVELUP_FAILED);
  tolua_constant(tolua_S,"ITEMLOG_EQUIP_LEVELUP_SUCCEEDED",ITEMLOG_EQUIP_LEVELUP_SUCCEEDED);
  tolua_constant(tolua_S,"ITEMLOG_RECOVER_USED_UP",ITEMLOG_RECOVER_USED_UP);
  tolua_constant(tolua_S,"ITEMLOG_MINE",ITEMLOG_MINE);
  tolua_constant(tolua_S,"ITEMLOG_ENROLL_SABAC",ITEMLOG_ENROLL_SABAC);
  tolua_constant(tolua_S,"ITEMLOG_DEATH",ITEMLOG_DEATH);
  tolua_constant(tolua_S,"ITEMLOG_SPELL",ITEMLOG_SPELL);
  tolua_constant(tolua_S,"ITEMLOG_ATTACK",ITEMLOG_ATTACK);
  tolua_constant(tolua_S,"ITEMLOG_ATTACKED",ITEMLOG_ATTACKED);
  tolua_constant(tolua_S,"ITEMLOG_TRUMPET",ITEMLOG_TRUMPET);
  tolua_constant(tolua_S,"ITEMLOG_CHECKIN_AWARD",ITEMLOG_CHECKIN_AWARD);
  tolua_constant(tolua_S,"ITEMLOG_AWARD",ITEMLOG_AWARD);
  tolua_constant(tolua_S,"ITEMLOG_BLACK_MARKET",ITEMLOG_BLACK_MARKET);
  tolua_constant(tolua_S,"ITEMLOG_MATERIAL",ITEMLOG_MATERIAL);
  tolua_constant(tolua_S,"ITEMLOG_GAMBLE",ITEMLOG_GAMBLE);
  tolua_constant(tolua_S,"ITEMLOG_PACKAGE",ITEMLOG_PACKAGE);
  tolua_constant(tolua_S,"ITEMLOG_IDENTIFY",ITEMLOG_IDENTIFY);
  tolua_constant(tolua_S,"ITEMLOG_RANDOMIZE",ITEMLOG_RANDOMIZE);
  tolua_constant(tolua_S,"ITEMLOG_LUCK_OIL",ITEMLOG_LUCK_OIL);
  tolua_constant(tolua_S,"ITEMLOG_SIGN",ITEMLOG_SIGN);
  tolua_constant(tolua_S,"ITEMLOG_STORAGE_ROLLBACK",ITEMLOG_STORAGE_ROLLBACK);
  tolua_constant(tolua_S,"ITEMLOG_SPELL_BOOK_GAMBLE",ITEMLOG_SPELL_BOOK_GAMBLE);
  tolua_constant(tolua_S,"ITEMLOG_GM_FIX_ROLE",ITEMLOG_GM_FIX_ROLE);
  tolua_constant(tolua_S,"ITEMLOG_RUNE_ADVANCE_CONFIRM",ITEMLOG_RUNE_ADVANCE_CONFIRM);
  tolua_constant(tolua_S,"ITEMLOG_POTION_GAMBLE",ITEMLOG_POTION_GAMBLE);
  tolua_constant(tolua_S,"ITEMLOG_SELL_RESOURCE_ITEM",ITEMLOG_SELL_RESOURCE_ITEM);
  tolua_constant(tolua_S,"ITEMLOG_EQUIP_ADD_SOCKET",ITEMLOG_EQUIP_ADD_SOCKET);
  tolua_constant(tolua_S,"ITEMLOG_EQUIP_SET_COLOR",ITEMLOG_EQUIP_SET_COLOR);
  tolua_constant(tolua_S,"ITEMLOG_CHANGE_EUIQP_SOCKET_COLOR",ITEMLOG_CHANGE_EUIQP_SOCKET_COLOR);
  tolua_constant(tolua_S,"ITEMLOG_ADD_DESTINY_WEAPON",ITEMLOG_ADD_DESTINY_WEAPON);
  tolua_constant(tolua_S,"ITEMLOG_TOTAL",ITEMLOG_TOTAL);
  tolua_constant(tolua_S,"MONEYLOG_INVALID",MONEYLOG_INVALID);
  tolua_constant(tolua_S,"MONEYLOG_BUY_ITEM",MONEYLOG_BUY_ITEM);
  tolua_constant(tolua_S,"MONEYLOG_REPAIR",MONEYLOG_REPAIR);
  tolua_constant(tolua_S,"MONEYLOG_PLAYER_STALL",MONEYLOG_PLAYER_STALL);
  tolua_constant(tolua_S,"MONEYLOG_PLAYER_STALL_TAX",MONEYLOG_PLAYER_STALL_TAX);
  tolua_constant(tolua_S,"MONEYLOG_PLAYER_TRADE",MONEYLOG_PLAYER_TRADE);
  tolua_constant(tolua_S,"MONEYLOG_PLAYER_TRADE_TAX",MONEYLOG_PLAYER_TRADE_TAX);
  tolua_constant(tolua_S,"MONEYLOG_BUY_BACK",MONEYLOG_BUY_BACK);
  tolua_constant(tolua_S,"MONEYLOG_REFINE_ITEM",MONEYLOG_REFINE_ITEM);
  tolua_constant(tolua_S,"MONEYLOG_BUY_SLOTS",MONEYLOG_BUY_SLOTS);
  tolua_constant(tolua_S,"MONEYLOG_REMOVE_SOUL",MONEYLOG_REMOVE_SOUL);
  tolua_constant(tolua_S,"MONEYLOG_CREATE_GUILD",MONEYLOG_CREATE_GUILD);
  tolua_constant(tolua_S,"MONEYLOG_GUILD_CONTRIBUTE",MONEYLOG_GUILD_CONTRIBUTE);
  tolua_constant(tolua_S,"MONEYLOG_GUILD_POP_ITEM",MONEYLOG_GUILD_POP_ITEM);
  tolua_constant(tolua_S,"MOMEYLOG_GUILD_ENTER_DUNGEON",MOMEYLOG_GUILD_ENTER_DUNGEON);
  tolua_constant(tolua_S,"MONEYLOG_TELEPORT_PLATFORM",MONEYLOG_TELEPORT_PLATFORM);
  tolua_constant(tolua_S,"MONEYLOG_DUNGEON_SELECT_REWARD",MONEYLOG_DUNGEON_SELECT_REWARD);
  tolua_constant(tolua_S,"MONEYLOG_CAST_SPELL",MONEYLOG_CAST_SPELL);
  tolua_constant(tolua_S,"MONEYLOG_MAIL_COST",MONEYLOG_MAIL_COST);
  tolua_constant(tolua_S,"MONEYLOG_LOOT",MONEYLOG_LOOT);
  tolua_constant(tolua_S,"MONEYLOG_SELL_ITEM",MONEYLOG_SELL_ITEM);
  tolua_constant(tolua_S,"MONEYLOG_QUEST_PREPAY",MONEYLOG_QUEST_PREPAY);
  tolua_constant(tolua_S,"MONEYLOG_QUEST_REWARD",MONEYLOG_QUEST_REWARD);
  tolua_constant(tolua_S,"MONEYLOG_QUEST_COST",MONEYLOG_QUEST_COST);
  tolua_constant(tolua_S,"MONEYLOG_GUILD_SALARY",MONEYLOG_GUILD_SALARY);
  tolua_constant(tolua_S,"MONEYLOG_DUNGEON_REWARD",MONEYLOG_DUNGEON_REWARD);
  tolua_constant(tolua_S,"MONEYLOG_GM_COMMAND",MONEYLOG_GM_COMMAND);
  tolua_constant(tolua_S,"MONEYLOG_SCRIPT",MONEYLOG_SCRIPT);
  tolua_constant(tolua_S,"MONEYLOG_IDENTIFY_ITEM",MONEYLOG_IDENTIFY_ITEM);
  tolua_constant(tolua_S,"MONEYLOG_RANDOMIZE_ITEM",MONEYLOG_RANDOMIZE_ITEM);
  tolua_constant(tolua_S,"MONEYLOG_QUEST_FLY_COST",MONEYLOG_QUEST_FLY_COST);
  tolua_constant(tolua_S,"MONEYLOG_SIGN_ITEM_COST",MONEYLOG_SIGN_ITEM_COST);
  tolua_constant(tolua_S,"MONEYLOG_AUCTION_SELL_COST",MONEYLOG_AUCTION_SELL_COST);
  tolua_constant(tolua_S,"MONEYLOG_AUCTION_DEPOSIT_COST",MONEYLOG_AUCTION_DEPOSIT_COST);
  tolua_constant(tolua_S,"MONEYLOG_AUCTION_LEFT_MONEY",MONEYLOG_AUCTION_LEFT_MONEY);
  tolua_constant(tolua_S,"MONEYLOG_CHANGE_WEATHER",MONEYLOG_CHANGE_WEATHER);
  tolua_constant(tolua_S,"MONEYLOG_BUY_VIP",MONEYLOG_BUY_VIP);
  tolua_constant(tolua_S,"MONEYLOG_DAILY_QUEST_BUY_COMPLETE_TIME",MONEYLOG_DAILY_QUEST_BUY_COMPLETE_TIME);
  tolua_constant(tolua_S,"MONEYLOG_DAILY_QUEST_BUY_ONESHOT",MONEYLOG_DAILY_QUEST_BUY_ONESHOT);
  tolua_constant(tolua_S,"MONEYLOG_OPEN_GUILD_DUNGEON",MONEYLOG_OPEN_GUILD_DUNGEON);
  tolua_constant(tolua_S,"MONEYLOG_GUILD_DECLARE_WAR",MONEYLOG_GUILD_DECLARE_WAR);
  tolua_constant(tolua_S,"MONEYLOG_DECOMPOSE",MONEYLOG_DECOMPOSE);
  tolua_constant(tolua_S,"MONEYLOG_COMPOSE",MONEYLOG_COMPOSE);
  tolua_constant(tolua_S,"MONEYLOG_SPIRIT_EXP2PTS",MONEYLOG_SPIRIT_EXP2PTS);
  tolua_constant(tolua_S,"MONEYLOG_SPIRIT_ADDAIR",MONEYLOG_SPIRIT_ADDAIR);
  tolua_constant(tolua_S,"MONEYLOG_SPIRIT_BUYSLOT",MONEYLOG_SPIRIT_BUYSLOT);
  tolua_constant(tolua_S,"MONEYLOG_SPIRIT_BUYEXCHANGEAIR",MONEYLOG_SPIRIT_BUYEXCHANGEAIR);
  tolua_constant(tolua_S,"MONEYLOG_SPIRIT_BLOOD",MONEYLOG_SPIRIT_BLOOD);
  tolua_constant(tolua_S,"MONEYLOG_REVIVE",MONEYLOG_REVIVE);
  tolua_constant(tolua_S,"MONEYLOG_CREATE_ARENA_TEAM",MONEYLOG_CREATE_ARENA_TEAM);
  tolua_constant(tolua_S,"MONEYLOG_GUILD_BUY_AURA",MONEYLOG_GUILD_BUY_AURA);
  tolua_constant(tolua_S,"MONEYLOG_GET_MAIL_ATTACH",MONEYLOG_GET_MAIL_ATTACH);
  tolua_constant(tolua_S,"MONEYLOG_GM",MONEYLOG_GM);
  tolua_constant(tolua_S,"MONEYLOG_REAL_MONEY_SHOP",MONEYLOG_REAL_MONEY_SHOP);
  tolua_constant(tolua_S,"MONEYLOG_TEAM_AUCTION_BONUS",MONEYLOG_TEAM_AUCTION_BONUS);
  tolua_constant(tolua_S,"MONEYLOG_TEAM_AUCTION_RETURN",MONEYLOG_TEAM_AUCTION_RETURN);
  tolua_constant(tolua_S,"MONEYLOG_TEAM_AUCTION_DEPOSITE",MONEYLOG_TEAM_AUCTION_DEPOSITE);
  tolua_constant(tolua_S,"MONEYLOG_EXCHANGE",MONEYLOG_EXCHANGE);
  tolua_constant(tolua_S,"MONEYLOG_EXCHANGE_FEE",MONEYLOG_EXCHANGE_FEE);
  tolua_constant(tolua_S,"MONEYLOG_MASTER_CZ",MONEYLOG_MASTER_CZ);
  tolua_constant(tolua_S,"MONEYLOG_ACHIEVEMENT_REWARD",MONEYLOG_ACHIEVEMENT_REWARD);
  tolua_constant(tolua_S,"MONEYLOG_RUNE",MONEYLOG_RUNE);
  tolua_constant(tolua_S,"MONEYLOG_ARENA_JOIN_SEASON",MONEYLOG_ARENA_JOIN_SEASON);
  tolua_constant(tolua_S,"MONEYLOG_ARENA_RANK_REWARD",MONEYLOG_ARENA_RANK_REWARD);
  tolua_constant(tolua_S,"MONEYLOG_ENROLL_SABAC",MONEYLOG_ENROLL_SABAC);
  tolua_constant(tolua_S,"MONEYLOG_EQUIP_LEVELUP",MONEYLOG_EQUIP_LEVELUP);
  tolua_constant(tolua_S,"MONEYLOG_REPUTATION_AWARD",MONEYLOG_REPUTATION_AWARD);
  tolua_constant(tolua_S,"MONEYLOG_UPGRADE_STALL",MONEYLOG_UPGRADE_STALL);
  tolua_constant(tolua_S,"MONEYLOG_MINE",MONEYLOG_MINE);
  tolua_constant(tolua_S,"MONEYLOG_AWARD",MONEYLOG_AWARD);
  tolua_constant(tolua_S,"MONEYLOG_ERECT_STATUS",MONEYLOG_ERECT_STATUS);
  tolua_constant(tolua_S,"MONEYLOG_FCM_LOSS",MONEYLOG_FCM_LOSS);
  tolua_constant(tolua_S,"MONEYLOG_DAILY_LIMIT_LOSS",MONEYLOG_DAILY_LIMIT_LOSS);
  tolua_constant(tolua_S,"MONEYLOG_WEEKLY_LIMIT_LOSS",MONEYLOG_WEEKLY_LIMIT_LOSS);
  tolua_constant(tolua_S,"MONEYLOG_TOTAL_LIMIT_LOSS",MONEYLOG_TOTAL_LIMIT_LOSS);
  tolua_constant(tolua_S,"MONEYLOG_CHAT_COST",MONEYLOG_CHAT_COST);
  tolua_constant(tolua_S,"MONEYLOG_ESCORT",MONEYLOG_ESCORT);
  tolua_constant(tolua_S,"MONEYLOG_BLACK_MARKET",MONEYLOG_BLACK_MARKET);
  tolua_constant(tolua_S,"MONEYLOG_MATERIAL",MONEYLOG_MATERIAL);
  tolua_constant(tolua_S,"MONEYLOG_BIRTH",MONEYLOG_BIRTH);
  tolua_constant(tolua_S,"MONEYLOG_EXP_MORAL",MONEYLOG_EXP_MORAL);
  tolua_constant(tolua_S,"MONEYLOG_TEACHER_REWARD",MONEYLOG_TEACHER_REWARD);
  tolua_constant(tolua_S,"MONEYLOG_STUDENT_REWARD",MONEYLOG_STUDENT_REWARD);
  tolua_constant(tolua_S,"MONEYLOG_PACKAGE",MONEYLOG_PACKAGE);
  tolua_constant(tolua_S,"MONEYLOG_EQUIP_LEVELUP_IMMEDIATELY",MONEYLOG_EQUIP_LEVELUP_IMMEDIATELY);
  tolua_constant(tolua_S,"MONEYLOG_ARENA_REFUND",MONEYLOG_ARENA_REFUND);
  tolua_constant(tolua_S,"MONEYLOG_ARENA_REWARD",MONEYLOG_ARENA_REWARD);
  tolua_constant(tolua_S,"MONEYLOG_SELL_RESOURCE_ITEM",MONEYLOG_SELL_RESOURCE_ITEM);
  tolua_constant(tolua_S,"MONEYLOG_CUSTOM_ROLE_BUY_TIME",MONEYLOG_CUSTOM_ROLE_BUY_TIME);
  tolua_constant(tolua_S,"MONEYLOG_EQUIP_LEVELUP_TAKE_OUT_IMMEDIATELY",MONEYLOG_EQUIP_LEVELUP_TAKE_OUT_IMMEDIATELY);
  tolua_constant(tolua_S,"MONEYLOG_GM_CMD_SUB_MONEY_YB",MONEYLOG_GM_CMD_SUB_MONEY_YB);
  tolua_constant(tolua_S,"MONEYLOG_GM_CMD_ADD_ITEM_YBD",MONEYLOG_GM_CMD_ADD_ITEM_YBD);
  tolua_constant(tolua_S,"MONEYLOG_BT_BEGIN",MONEYLOG_BT_BEGIN);
  tolua_constant(tolua_S,"MONEYLOG_BT_01",MONEYLOG_BT_01);
  tolua_constant(tolua_S,"MONEYLOG_BT_02",MONEYLOG_BT_02);
  tolua_constant(tolua_S,"MONEYLOG_BT_03",MONEYLOG_BT_03);
  tolua_constant(tolua_S,"MONEYLOG_BT_04",MONEYLOG_BT_04);
  tolua_constant(tolua_S,"MONEYLOG_BT_05",MONEYLOG_BT_05);
  tolua_constant(tolua_S,"MONEYLOG_BT_06",MONEYLOG_BT_06);
  tolua_constant(tolua_S,"MONEYLOG_BT_07",MONEYLOG_BT_07);
  tolua_constant(tolua_S,"MONEYLOG_BT_08",MONEYLOG_BT_08);
  tolua_constant(tolua_S,"MONEYLOG_BT_09",MONEYLOG_BT_09);
  tolua_constant(tolua_S,"MONEYLOG_BT_10",MONEYLOG_BT_10);
  tolua_constant(tolua_S,"MONEYLOG_BT_11",MONEYLOG_BT_11);
  tolua_constant(tolua_S,"MONEYLOG_BT_12",MONEYLOG_BT_12);
  tolua_constant(tolua_S,"MONEYLOG_BT_13",MONEYLOG_BT_13);
  tolua_constant(tolua_S,"MONEYLOG_BT_14",MONEYLOG_BT_14);
  tolua_constant(tolua_S,"MONEYLOG_BT_15",MONEYLOG_BT_15);
  tolua_constant(tolua_S,"MONEYLOG_BT_16",MONEYLOG_BT_16);
  tolua_constant(tolua_S,"MONEYLOG_BT_17",MONEYLOG_BT_17);
  tolua_constant(tolua_S,"MONEYLOG_BT_18",MONEYLOG_BT_18);
  tolua_constant(tolua_S,"MONEYLOG_BT_19",MONEYLOG_BT_19);
  tolua_constant(tolua_S,"MONEYLOG_BT_20",MONEYLOG_BT_20);
  tolua_constant(tolua_S,"MONEYLOG_BT_21",MONEYLOG_BT_21);
  tolua_constant(tolua_S,"MONEYLOG_BT_22",MONEYLOG_BT_22);
  tolua_constant(tolua_S,"MONEYLOG_BT_23",MONEYLOG_BT_23);
  tolua_constant(tolua_S,"MONEYLOG_BT_24",MONEYLOG_BT_24);
  tolua_constant(tolua_S,"MONEYLOG_BT_25",MONEYLOG_BT_25);
  tolua_constant(tolua_S,"MONEYLOG_BT_26",MONEYLOG_BT_26);
  tolua_constant(tolua_S,"MONEYLOG_BT_27",MONEYLOG_BT_27);
  tolua_constant(tolua_S,"MONEYLOG_BT_28",MONEYLOG_BT_28);
  tolua_constant(tolua_S,"MONEYLOG_BT_29",MONEYLOG_BT_29);
  tolua_constant(tolua_S,"MONEYLOG_BT_30",MONEYLOG_BT_30);
  tolua_constant(tolua_S,"MONEYLOG_BT_31",MONEYLOG_BT_31);
  tolua_constant(tolua_S,"MONEYLOG_BT_32",MONEYLOG_BT_32);
  tolua_constant(tolua_S,"MONEYLOG_BT_33",MONEYLOG_BT_33);
  tolua_constant(tolua_S,"MONEYLOG_BT_34",MONEYLOG_BT_34);
  tolua_constant(tolua_S,"MONEYLOG_BT_35",MONEYLOG_BT_35);
  tolua_constant(tolua_S,"MONEYLOG_BT_36",MONEYLOG_BT_36);
  tolua_constant(tolua_S,"MONEYLOG_BT_37",MONEYLOG_BT_37);
  tolua_constant(tolua_S,"MONEYLOG_BT_38",MONEYLOG_BT_38);
  tolua_constant(tolua_S,"MONEYLOG_BT_39",MONEYLOG_BT_39);
  tolua_constant(tolua_S,"MONEYLOG_BT_40",MONEYLOG_BT_40);
  tolua_constant(tolua_S,"MONEYLOG_TOTAL",MONEYLOG_TOTAL);
  tolua_constant(tolua_S,"ITEMACTION_ADD",ITEMACTION_ADD);
  tolua_constant(tolua_S,"ITEMACTION_DEL",ITEMACTION_DEL);
  tolua_constant(tolua_S,"ITEMACTION_INC",ITEMACTION_INC);
  tolua_constant(tolua_S,"ITEMACTION_DEC",ITEMACTION_DEC);
  tolua_constant(tolua_S,"ITEMACTION_DROP",ITEMACTION_DROP);
  tolua_constant(tolua_S,"ITEMACTION_TOTAL",ITEMACTION_TOTAL);
  tolua_constant(tolua_S,"TRADEACTION_ASK",TRADEACTION_ASK);
  tolua_constant(tolua_S,"TRADEACTION_ACCEPT",TRADEACTION_ACCEPT);
  tolua_constant(tolua_S,"TRADEACTION_PUT_ITEM",TRADEACTION_PUT_ITEM);
  tolua_constant(tolua_S,"TRADEACTION_PUT_MONEY",TRADEACTION_PUT_MONEY);
  tolua_constant(tolua_S,"TRADEACTION_LOCK",TRADEACTION_LOCK);
  tolua_constant(tolua_S,"TRADEACTION_CONFIRM",TRADEACTION_CONFIRM);
  tolua_constant(tolua_S,"TRADEACTION_UNLOCK",TRADEACTION_UNLOCK);
  tolua_constant(tolua_S,"TRADEACTION_CANCEL",TRADEACTION_CANCEL);
  tolua_constant(tolua_S,"TRADEACTION_COMMIT_FAIL_ROLE_MONEY",TRADEACTION_COMMIT_FAIL_ROLE_MONEY);
  tolua_constant(tolua_S,"TRADEACTION_COMMIT_FAIL_TARGET_MONEY",TRADEACTION_COMMIT_FAIL_TARGET_MONEY);
  tolua_constant(tolua_S,"TRADEACTION_COMMIT_FAIL_ROLE_BAG",TRADEACTION_COMMIT_FAIL_ROLE_BAG);
  tolua_constant(tolua_S,"TRADEACTION_COMMIT_FAIL_TARGET_BAG",TRADEACTION_COMMIT_FAIL_TARGET_BAG);
  tolua_constant(tolua_S,"TRADEACTION_COMMIT_SUCCESS",TRADEACTION_COMMIT_SUCCESS);
  tolua_constant(tolua_S,"TRADEACTION_COMMIT_FAIL_ROLE_OVERWEIGHT",TRADEACTION_COMMIT_FAIL_ROLE_OVERWEIGHT);
  tolua_constant(tolua_S,"TRADEACTION_COMMIT_FAIL_TARGET_OVERWEIGHT",TRADEACTION_COMMIT_FAIL_TARGET_OVERWEIGHT);
  tolua_constant(tolua_S,"TRADEACTION_TOTAL",TRADEACTION_TOTAL);
  tolua_constant(tolua_S,"ITEMTYPE_INVALID",ITEMTYPE_INVALID);
  tolua_constant(tolua_S,"ITEMTYPE_USABLE",ITEMTYPE_USABLE);
  tolua_constant(tolua_S,"ITEMTYPE_EQUIP",ITEMTYPE_EQUIP);
  tolua_constant(tolua_S,"ITEMTYPE_MATERIAL",ITEMTYPE_MATERIAL);
  tolua_constant(tolua_S,"ITEMTYPE_FORMULA",ITEMTYPE_FORMULA);
  tolua_constant(tolua_S,"ITEMTYPE_SPELLBOOK",ITEMTYPE_SPELLBOOK);
  tolua_constant(tolua_S,"ITEMTYPE_RUNE",ITEMTYPE_RUNE);
  tolua_constant(tolua_S,"ITEMTYPE_DAN",ITEMTYPE_DAN);
  tolua_constant(tolua_S,"ITEMTYPE_SOUL",ITEMTYPE_SOUL);
  tolua_constant(tolua_S,"ITEMTYPE_MISC",ITEMTYPE_MISC);
  tolua_constant(tolua_S,"ITEMTYPE_QUEST",ITEMTYPE_QUEST);
  tolua_constant(tolua_S,"ITEMTYPE_DEPOSIT",ITEMTYPE_DEPOSIT);
  tolua_constant(tolua_S,"ITEMTYPE_RECOVER",ITEMTYPE_RECOVER);
  tolua_constant(tolua_S,"ITEMTYPE_RMB",ITEMTYPE_RMB);
  tolua_constant(tolua_S,"ITEMTYPE_TOTAL",ITEMTYPE_TOTAL);
  tolua_constant(tolua_S,"ITEMSUBTYPE_INVALID",ITEMSUBTYPE_INVALID);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_FOOD",ITEMSUBTYPE_USABLE_FOOD);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_DRINK",ITEMSUBTYPE_USABLE_DRINK);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_POTION",ITEMSUBTYPE_USABLE_POTION);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_FLASK",ITEMSUBTYPE_USABLE_FLASK);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_SCROLL",ITEMSUBTYPE_USABLE_SCROLL);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_MISC",ITEMSUBTYPE_USABLE_MISC);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_GREY_POWDER",ITEMSUBTYPE_USABLE_GREY_POWDER);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_YELLOW_POWDER",ITEMSUBTYPE_USABLE_YELLOW_POWDER);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_RUNE",ITEMSUBTYPE_USABLE_RUNE);
  tolua_constant(tolua_S,"ITEMSUBTYPE_USABLE_QUIVER",ITEMSUBTYPE_USABLE_QUIVER);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_WEAPON",ITEMSUBTYPE_EQUIP_WEAPON);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_HAT",ITEMSUBTYPE_EQUIP_HAT);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_SHOULDER_ARMOR",ITEMSUBTYPE_EQUIP_SHOULDER_ARMOR);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_CHEST_ARMOR",ITEMSUBTYPE_EQUIP_CHEST_ARMOR);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_WRIST_ARMOR",ITEMSUBTYPE_EQUIP_WRIST_ARMOR);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_BELT",ITEMSUBTYPE_EQUIP_BELT);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_SHOES",ITEMSUBTYPE_EQUIP_SHOES);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_CLOAK",ITEMSUBTYPE_EQUIP_CLOAK);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_NECKLACE",ITEMSUBTYPE_EQUIP_NECKLACE);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_RING",ITEMSUBTYPE_EQUIP_RING);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_BRACELET",ITEMSUBTYPE_EQUIP_BRACELET);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_HONOR",ITEMSUBTYPE_EQUIP_HONOR);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_JADE",ITEMSUBTYPE_EQUIP_JADE);
  tolua_constant(tolua_S,"ITEMSUBTYPE_EQUIP_OFFHAND",ITEMSUBTYPE_EQUIP_OFFHAND);
  tolua_constant(tolua_S,"ITEMSUBTYPE_MATERIAL_REFINE",ITEMSUBTYPE_MATERIAL_REFINE);
  tolua_constant(tolua_S,"ITEMSUBTYPE_MATERIAL_COMPOSE",ITEMSUBTYPE_MATERIAL_COMPOSE);
  tolua_constant(tolua_S,"ITEMSUBTYPE_MATERIAL_WEAPON_LEVELUP",ITEMSUBTYPE_MATERIAL_WEAPON_LEVELUP);
  tolua_constant(tolua_S,"ITEMSUBTYPE_MATERIAL_MISC",ITEMSUBTYPE_MATERIAL_MISC);
  tolua_constant(tolua_S,"ITEMSUBTYPE_FORMULA_DEFAULT",ITEMSUBTYPE_FORMULA_DEFAULT);
  tolua_constant(tolua_S,"ITEMSUBTYPE_SPELLBOOK_PLAYER",ITEMSUBTYPE_SPELLBOOK_PLAYER);
  tolua_constant(tolua_S,"ITEMSUBTYPE_SPELLBOOK_PET",ITEMSUBTYPE_SPELLBOOK_PET);
  tolua_constant(tolua_S,"ITEMSUBTYPE_RUNE_DEFAULT",ITEMSUBTYPE_RUNE_DEFAULT);
  tolua_constant(tolua_S,"ITEMSUBTYPE_DAN_DEFAULT",ITEMSUBTYPE_DAN_DEFAULT);
  tolua_constant(tolua_S,"ITEMSUBTYPE_SOUL_DEFAULT",ITEMSUBTYPE_SOUL_DEFAULT);
  tolua_constant(tolua_S,"ITEMSUBTYPE_MISC_DEFAULT",ITEMSUBTYPE_MISC_DEFAULT);
  tolua_constant(tolua_S,"ITEMSUBTYPE_MISC_PET",ITEMSUBTYPE_MISC_PET);
  tolua_constant(tolua_S,"ITEMSUBTYPE_MISC_MOUNT",ITEMSUBTYPE_MISC_MOUNT);
  tolua_constant(tolua_S,"ITEMSUBTYPE_QUEST_DEFAULT",ITEMSUBTYPE_QUEST_DEFAULT);
  tolua_constant(tolua_S,"ITEMSUBTYPE_DEPOSIT_SOUL",ITEMSUBTYPE_DEPOSIT_SOUL);
  tolua_constant(tolua_S,"ITEMSUBTYPE_DEPOSIT_EXP",ITEMSUBTYPE_DEPOSIT_EXP);
  tolua_constant(tolua_S,"ITEMSUBTYPE_RECOVER_HP",ITEMSUBTYPE_RECOVER_HP);
  tolua_constant(tolua_S,"ITEMSUBTYPE_RECOVER_MP",ITEMSUBTYPE_RECOVER_MP);
  tolua_constant(tolua_S,"ITEMSUBTYPE_RECOVER_HPMP",ITEMSUBTYPE_RECOVER_HPMP);
  tolua_constant(tolua_S,"ITEMSUBTYPE_RMB_DEFAULT",ITEMSUBTYPE_RMB_DEFAULT);
  tolua_constant(tolua_S,"WEAPONTYPE_INVALID",WEAPONTYPE_INVALID);
  tolua_constant(tolua_S,"WEAPONTYPE_HAND",WEAPONTYPE_HAND);
  tolua_constant(tolua_S,"WEAPONTYPE_01",WEAPONTYPE_01);
  tolua_constant(tolua_S,"WEAPONTYPE_02",WEAPONTYPE_02);
  tolua_constant(tolua_S,"WEAPONTYPE_03",WEAPONTYPE_03);
  tolua_constant(tolua_S,"WEAPONTYPE_04",WEAPONTYPE_04);
  tolua_constant(tolua_S,"WEAPONTYPE_05",WEAPONTYPE_05);
  tolua_constant(tolua_S,"WEAPONTYPE_06",WEAPONTYPE_06);
  tolua_constant(tolua_S,"WEAPONTYPE_07",WEAPONTYPE_07);
  tolua_constant(tolua_S,"WEAPONTYPE_08",WEAPONTYPE_08);
  tolua_constant(tolua_S,"WEAPONTYPE_09",WEAPONTYPE_09);
  tolua_constant(tolua_S,"WEAPONTYPE_10",WEAPONTYPE_10);
  tolua_constant(tolua_S,"WEAPONTYPE_11",WEAPONTYPE_11);
  tolua_constant(tolua_S,"WEAPONTYPE_12",WEAPONTYPE_12);
  tolua_constant(tolua_S,"WEAPONTYPE_13",WEAPONTYPE_13);
  tolua_constant(tolua_S,"WEAPONTYPE_14",WEAPONTYPE_14);
  tolua_constant(tolua_S,"WEAPONTYPE_15",WEAPONTYPE_15);
  tolua_constant(tolua_S,"WEAPONTYPE_16",WEAPONTYPE_16);
  tolua_constant(tolua_S,"WEAPONTYPE_17",WEAPONTYPE_17);
  tolua_constant(tolua_S,"WEAPONTYPE_TOTAL",WEAPONTYPE_TOTAL);
  tolua_constant(tolua_S,"ARMORTYPE_INVALID",ARMORTYPE_INVALID);
  tolua_constant(tolua_S,"ARMORTYPE_NO_ARMOR",ARMORTYPE_NO_ARMOR);
  tolua_constant(tolua_S,"ARMORTYPE_CLOTH",ARMORTYPE_CLOTH);
  tolua_constant(tolua_S,"ARMORTYPE_LEATHER",ARMORTYPE_LEATHER);
  tolua_constant(tolua_S,"ARMORTYPE_CHAIN",ARMORTYPE_CHAIN);
  tolua_constant(tolua_S,"ARMORTYPE_PLATE",ARMORTYPE_PLATE);
  tolua_constant(tolua_S,"ARMORTYPE_BACK",ARMORTYPE_BACK);
  tolua_constant(tolua_S,"ARMORTYPE_JEWELRY",ARMORTYPE_JEWELRY);
  tolua_constant(tolua_S,"ARMORTYPE_AVATAR",ARMORTYPE_AVATAR);
  tolua_constant(tolua_S,"ITEMCOLOR_INVALID",ITEMCOLOR_INVALID);
  tolua_constant(tolua_S,"ITEMCOLOR_WHITE",ITEMCOLOR_WHITE);
  tolua_constant(tolua_S,"ITEMCOLOR_GREEN",ITEMCOLOR_GREEN);
  tolua_constant(tolua_S,"ITEMCOLOR_BLUE",ITEMCOLOR_BLUE);
  tolua_constant(tolua_S,"ITEMCOLOR_PURPLE",ITEMCOLOR_PURPLE);
  tolua_constant(tolua_S,"ITEMCOLOR_ORANGE",ITEMCOLOR_ORANGE);
  tolua_constant(tolua_S,"ITEMCOLOR_RESERVED1",ITEMCOLOR_RESERVED1);
  tolua_constant(tolua_S,"ITEMCOLOR_RESERVED2",ITEMCOLOR_RESERVED2);
  tolua_constant(tolua_S,"ITEMCOLOR_TOTAL",ITEMCOLOR_TOTAL);
  tolua_constant(tolua_S,"ITEMCOMBAT_BOTH",ITEMCOMBAT_BOTH);
  tolua_constant(tolua_S,"ITEMCOMBAT_IN",ITEMCOMBAT_IN);
  tolua_constant(tolua_S,"ITEMCOMBAT_OUT",ITEMCOMBAT_OUT);
  tolua_constant(tolua_S,"DURTYPE_NONE",DURTYPE_NONE);
  tolua_constant(tolua_S,"DURTYPE_EQUIP",DURTYPE_EQUIP);
  tolua_constant(tolua_S,"DURTYPE_STACK",DURTYPE_STACK);
  tolua_constant(tolua_S,"DURTYPE_USE",DURTYPE_USE);
  tolua_constant(tolua_S,"DURTYPE_DEPOSIT",DURTYPE_DEPOSIT);
  tolua_constant(tolua_S,"DURTYPE_RECOVER",DURTYPE_RECOVER);
  tolua_constant(tolua_S,"DURTYPE_PURITY",DURTYPE_PURITY);
 tolua_constant(tolua_S,"EQUIP_DUR_MULTIPLIER",EQUIP_DUR_MULTIPLIER);
  tolua_constant(tolua_S,"BINDTYPE_NONE",BINDTYPE_NONE);
  tolua_constant(tolua_S,"BINDTYPE_BOU",BINDTYPE_BOU);
  tolua_constant(tolua_S,"BINDTYPE_BOE",BINDTYPE_BOE);
  tolua_constant(tolua_S,"BINDTYPE_BOP",BINDTYPE_BOP);
  tolua_constant(tolua_S,"BINDTYPE_BOT",BINDTYPE_BOT);
  tolua_constant(tolua_S,"BINDTYPE_BINDED",BINDTYPE_BINDED);
  tolua_constant(tolua_S,"PKDROP_BY_CHANCE",PKDROP_BY_CHANCE);
  tolua_constant(tolua_S,"PKDROP_NOT_DROP",PKDROP_NOT_DROP);
  tolua_constant(tolua_S,"PKDROP_MUST_DROP",PKDROP_MUST_DROP);
  tolua_constant(tolua_S,"IDENTIFY_TYPE_NO",IDENTIFY_TYPE_NO);
  tolua_constant(tolua_S,"IDENTIFY_TYPE_YES",IDENTIFY_TYPE_YES);
  tolua_constant(tolua_S,"IDENTIFY_TYPE_LEVEL_UP",IDENTIFY_TYPE_LEVEL_UP);
  tolua_constant(tolua_S,"CONSLOT_EQUIPPED",CONSLOT_EQUIPPED);
  tolua_constant(tolua_S,"CONSLOT_DEFAULT",CONSLOT_DEFAULT);
  tolua_constant(tolua_S,"CONSLOT_BANK",CONSLOT_BANK);
  tolua_constant(tolua_S,"CONSLOT_HERO_EQUIPPED",CONSLOT_HERO_EQUIPPED);
  tolua_constant(tolua_S,"CONSLOT_HERO_EQUIPMENT",CONSLOT_HERO_EQUIPMENT);
  tolua_constant(tolua_S,"CONSLOT_DEPOSIT",CONSLOT_DEPOSIT);
  tolua_constant(tolua_S,"CONSLOT_RECOVER",CONSLOT_RECOVER);
  tolua_constant(tolua_S,"CONSLOT_RESOURCE",CONSLOT_RESOURCE);
  tolua_constant(tolua_S,"CONSLOT_COUNT",CONSLOT_COUNT);
  tolua_constant(tolua_S,"CONSLOT_P2P_MY",CONSLOT_P2P_MY);
  tolua_constant(tolua_S,"CONSLOT_P2P_TG",CONSLOT_P2P_TG);
  tolua_constant(tolua_S,"CONSLOT_STALL_SETTING",CONSLOT_STALL_SETTING);
  tolua_constant(tolua_S,"CONSLOT_STALL_BUY",CONSLOT_STALL_BUY);
  tolua_constant(tolua_S,"CONSLOT_PROFILE",CONSLOT_PROFILE);
  tolua_constant(tolua_S,"CONSLOT_GUILD_BANK",CONSLOT_GUILD_BANK);
  tolua_constant(tolua_S,"CONSLOT_WEAPON_LEVEL_UP",CONSLOT_WEAPON_LEVEL_UP);
  tolua_constant(tolua_S,"NES_MAIN_HAND",NES_MAIN_HAND);
  tolua_constant(tolua_S,"NES_BODY",NES_BODY);
  tolua_constant(tolua_S,"NES_BACK",NES_BACK);
  tolua_constant(tolua_S,"NES_HEAD",NES_HEAD);
  tolua_constant(tolua_S,"NES_SHOULDER",NES_SHOULDER);
  tolua_constant(tolua_S,"NES_ARM",NES_ARM);
  tolua_constant(tolua_S,"NES_WAIST",NES_WAIST);
  tolua_constant(tolua_S,"NES_FOOT",NES_FOOT);
  tolua_constant(tolua_S,"NES_NECK",NES_NECK);
  tolua_constant(tolua_S,"NES_FINGER_L",NES_FINGER_L);
  tolua_constant(tolua_S,"NES_FINGER_R",NES_FINGER_R);
  tolua_constant(tolua_S,"NES_WRIST_L",NES_WRIST_L);
  tolua_constant(tolua_S,"NES_WRIST_R",NES_WRIST_R);
  tolua_constant(tolua_S,"NES_TOTAL",NES_TOTAL);
  tolua_constant(tolua_S,"NES_VISIBLE_TOTAL",NES_VISIBLE_TOTAL);
#ifndef TOLUA_DISABLE_tolua_enum_IsAppearance00
 tolua_function(tolua_S,"IsAppearance",tolua_enum_IsAppearance00);
#endif //#ifndef TOLUA_DISABLE
  tolua_constant(tolua_S,"DEPOSIT_SOUL_SLOT",DEPOSIT_SOUL_SLOT);
  tolua_constant(tolua_S,"DEPOSIT_EXP_SLOT",DEPOSIT_EXP_SLOT);
  tolua_constant(tolua_S,"DEPOSIT_ITEM_SLOT_TOTAL",DEPOSIT_ITEM_SLOT_TOTAL);
  tolua_constant(tolua_S,"SOCKET_NONE",SOCKET_NONE);
  tolua_constant(tolua_S,"SOCKET_RED",SOCKET_RED);
  tolua_constant(tolua_S,"SOCKET_YELLOW",SOCKET_YELLOW);
  tolua_constant(tolua_S,"SOCKET_BLUE",SOCKET_BLUE);
  tolua_constant(tolua_S,"SOCKET_GREEN",SOCKET_GREEN);
  tolua_constant(tolua_S,"SOCKET_PURPLE",SOCKET_PURPLE);
  tolua_constant(tolua_S,"SOCKET_COLORFUL",SOCKET_COLORFUL);
  tolua_constant(tolua_S,"SOCKET_TYPE_COUNT",SOCKET_TYPE_COUNT);
  tolua_constant(tolua_S,"asetEquipIsNull",asetEquipIsNull);
  tolua_constant(tolua_S,"asetEquipCantSocket",asetEquipCantSocket);
  tolua_constant(tolua_S,"asetEquipCantNotBeAddSocket",asetEquipCantNotBeAddSocket);
  tolua_constant(tolua_S,"asetEquipAddSocketSlotUnknown",asetEquipAddSocketSlotUnknown);
  tolua_constant(tolua_S,"asetEquipNeedItemCount",asetEquipNeedItemCount);
  tolua_constant(tolua_S,"asetEquipItemNotEnough",asetEquipItemNotEnough);
  tolua_constant(tolua_S,"asetEquipAddSocketSuccess",asetEquipAddSocketSuccess);
  tolua_constant(tolua_S,"ssctEquipIsNull",ssctEquipIsNull);
  tolua_constant(tolua_S,"ssctEquipNoSocket1",ssctEquipNoSocket1);
  tolua_constant(tolua_S,"ssctEquipNoSocket2",ssctEquipNoSocket2);
  tolua_constant(tolua_S,"ssctEquipCantSetColor",ssctEquipCantSetColor);
  tolua_constant(tolua_S,"ssctEquipSetColorError",ssctEquipSetColorError);
  tolua_constant(tolua_S,"ssctEquipSetSlotError",ssctEquipSetSlotError);
  tolua_constant(tolua_S,"ssctEquipNeedItemCount",ssctEquipNeedItemCount);
  tolua_constant(tolua_S,"ssctEquipItemNotEnough",ssctEquipItemNotEnough);
  tolua_constant(tolua_S,"ssctEquipSetSocketSuccess",ssctEquipSetSocketSuccess);
  tolua_constant(tolua_S,"essNotOpen",essNotOpen);
  tolua_constant(tolua_S,"essPreparing",essPreparing);
  tolua_constant(tolua_S,"essOpen",essOpen);
 tolua_constant(tolua_S,"MAX_TRADE_SLOT",MAX_TRADE_SLOT);
  tolua_constant(tolua_S,"etsFree",etsFree);
  tolua_constant(tolua_S,"etsAsking",etsAsking);
  tolua_constant(tolua_S,"etsBeingAsked",etsBeingAsked);
  tolua_constant(tolua_S,"etsTrading",etsTrading);
  tolua_constant(tolua_S,"etsLocked",etsLocked);
  tolua_constant(tolua_S,"etsConfirmed",etsConfirmed);
  tolua_constant(tolua_S,"etsSuccess",etsSuccess);
 tolua_constant(tolua_S,"MAX_SOLD_ITEMS",MAX_SOLD_ITEMS);
  tolua_constant(tolua_S,"ebmltNone",ebmltNone);
  tolua_constant(tolua_S,"ebmltRole",ebmltRole);
  tolua_constant(tolua_S,"ebmltServer",ebmltServer);
 tolua_constant(tolua_S,"MAX_RUNE_COUNT",MAX_RUNE_COUNT);
 tolua_constant(tolua_S,"MAX_SOCKET_COUNT",MAX_SOCKET_COUNT);
 tolua_cclass(tolua_S,"DROP_CHANCE_DATA","DROP_CHANCE_DATA","",NULL);
 tolua_beginmodule(tolua_S,"DROP_CHANCE_DATA");
  tolua_variable(tolua_S,"nDropGroupID",tolua_get_DROP_CHANCE_DATA_nDropGroupID,tolua_set_DROP_CHANCE_DATA_nDropGroupID);
  tolua_variable(tolua_S,"nDropChance",tolua_get_DROP_CHANCE_DATA_nDropChance,tolua_set_DROP_CHANCE_DATA_nDropChance);
 tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"egtInvalid",egtInvalid);
  tolua_constant(tolua_S,"egtEquipWeapon",egtEquipWeapon);
  tolua_constant(tolua_S,"egtEquipHat",egtEquipHat);
  tolua_constant(tolua_S,"egtEquipShoulderArmor",egtEquipShoulderArmor);
  tolua_constant(tolua_S,"egtEquipChestArmor",egtEquipChestArmor);
  tolua_constant(tolua_S,"egtEquipWristArmor",egtEquipWristArmor);
  tolua_constant(tolua_S,"egtEquipBelt",egtEquipBelt);
  tolua_constant(tolua_S,"egtEquipShoes",egtEquipShoes);
  tolua_constant(tolua_S,"egtEquipCloak",egtEquipCloak);
  tolua_constant(tolua_S,"egtEquipNecklace",egtEquipNecklace);
  tolua_constant(tolua_S,"egtEquipRing",egtEquipRing);
  tolua_constant(tolua_S,"egtEquipBracelet",egtEquipBracelet);
  tolua_constant(tolua_S,"egtSpellBook",egtSpellBook);
  tolua_constant(tolua_S,"egtRuneWarrior",egtRuneWarrior);
  tolua_constant(tolua_S,"egtRuneMage",egtRuneMage);
  tolua_constant(tolua_S,"egtRuneRogue",egtRuneRogue);
  tolua_constant(tolua_S,"egtRuneArcher",egtRuneArcher);
  tolua_constant(tolua_S,"egtRuneTaoist",egtRuneTaoist);
  tolua_constant(tolua_S,"egtGambleItem",egtGambleItem);
  tolua_constant(tolua_S,"egtPotion",egtPotion);
  tolua_constant(tolua_S,"egtReserved03",egtReserved03);
  tolua_constant(tolua_S,"egtReserved04",egtReserved04);
  tolua_constant(tolua_S,"egtReserved05",egtReserved05);
  tolua_constant(tolua_S,"egtReserved06",egtReserved06);
  tolua_constant(tolua_S,"egtReserved07",egtReserved07);
  tolua_constant(tolua_S,"egtReserved08",egtReserved08);
  tolua_constant(tolua_S,"egtReserved09",egtReserved09);
  tolua_constant(tolua_S,"egtReserved10",egtReserved10);
  tolua_constant(tolua_S,"egtReserved11",egtReserved11);
  tolua_constant(tolua_S,"egtReserved12",egtReserved12);
  tolua_constant(tolua_S,"egtReserved13",egtReserved13);
  tolua_constant(tolua_S,"egtReserved14",egtReserved14);
  tolua_constant(tolua_S,"egtReserved15",egtReserved15);
  tolua_constant(tolua_S,"egtTotal",egtTotal);
  tolua_constant(tolua_S,"MONEYTYPE_INVALID",MONEYTYPE_INVALID);
  tolua_constant(tolua_S,"MONEYTYPE_01",MONEYTYPE_01);
  tolua_constant(tolua_S,"MONEYTYPE_02",MONEYTYPE_02);
  tolua_constant(tolua_S,"MONEYTYPE_03",MONEYTYPE_03);
  tolua_constant(tolua_S,"MONEYTYPE_04",MONEYTYPE_04);
  tolua_constant(tolua_S,"MONEYTYPE_FATIGUE",MONEYTYPE_FATIGUE);
  tolua_constant(tolua_S,"MONEYTYPE_SOUL_PTS",MONEYTYPE_SOUL_PTS);
  tolua_constant(tolua_S,"MONEYTYPE_MASTER_EXP",MONEYTYPE_MASTER_EXP);
  tolua_constant(tolua_S,"MONEYTYPE_AIR",MONEYTYPE_AIR);
  tolua_constant(tolua_S,"MONEYTYPE_BF",MONEYTYPE_BF);
  tolua_constant(tolua_S,"MONEYTYPE_GUILD",MONEYTYPE_GUILD);
  tolua_constant(tolua_S,"MONEYTYPE_SPELL_PTS",MONEYTYPE_SPELL_PTS);
  tolua_constant(tolua_S,"MONEYTYPE_MORAL",MONEYTYPE_MORAL);
  tolua_constant(tolua_S,"MONEYTYPE_ARENA",MONEYTYPE_ARENA);
  tolua_constant(tolua_S,"MONEYTYPE_ACHIEVEMENTPTS",MONEYTYPE_ACHIEVEMENTPTS);
  tolua_constant(tolua_S,"MONEYTYPE_COUNT",MONEYTYPE_COUNT);
 tolua_constant(tolua_S,"MAX_NORMAL_QUEST_COUNT",MAX_NORMAL_QUEST_COUNT);
 tolua_constant(tolua_S,"MAX_NORMAL_QUEST_BYTE_SIZE",MAX_NORMAL_QUEST_BYTE_SIZE);
 tolua_constant(tolua_S,"REPEAT_QUEST_START_ID",REPEAT_QUEST_START_ID);
 tolua_constant(tolua_S,"MAX_REPEAT_QUEST_COUNT",MAX_REPEAT_QUEST_COUNT);
 tolua_constant(tolua_S,"MAX_REPEAT_QUEST_CYCLE",MAX_REPEAT_QUEST_CYCLE);
 tolua_constant(tolua_S,"MAX_ACCEPTED_NORMAL_QUEST_COUNT",MAX_ACCEPTED_NORMAL_QUEST_COUNT);
 tolua_constant(tolua_S,"MAX_ACCEPTED_EMERGENCY_QUEST_COUNT",MAX_ACCEPTED_EMERGENCY_QUEST_COUNT);
 tolua_constant(tolua_S,"MAX_CONDITION_PER_QUEST",MAX_CONDITION_PER_QUEST);
 tolua_constant(tolua_S,"MAX_REWARD_PER_QUEST",MAX_REWARD_PER_QUEST);
 tolua_constant(tolua_S,"MAX_SELECTABLE_REWARD_COUNT",MAX_SELECTABLE_REWARD_COUNT);
 tolua_constant(tolua_S,"MAX_DIALOG_CONTENT_LEN",MAX_DIALOG_CONTENT_LEN);
 tolua_constant(tolua_S,"INVALID_QUEST_ID",INVALID_QUEST_ID);
 tolua_constant(tolua_S,"MAX_FLY_SPOT_PER_QUEST",MAX_FLY_SPOT_PER_QUEST);
 tolua_constant(tolua_S,"CIRCLE_QUEST_START_ID",CIRCLE_QUEST_START_ID);
 tolua_constant(tolua_S,"EMERGENCY_QUEST_START_ID",EMERGENCY_QUEST_START_ID);
 tolua_constant(tolua_S,"MAX_EMERGENCY_QUEST_COUNT",MAX_EMERGENCY_QUEST_COUNT);
 tolua_constant(tolua_S,"MAX_EMERGENCY_QUEST_COMPLETE_PER_DAY",MAX_EMERGENCY_QUEST_COMPLETE_PER_DAY);
 tolua_constant(tolua_S,"MAX_EMERGENCY_QUEST_PER_ROLE",MAX_EMERGENCY_QUEST_PER_ROLE);
 tolua_constant(tolua_S,"MAX_CHECK_VAR_COUNT",MAX_CHECK_VAR_COUNT);
 tolua_constant(tolua_S,"MAX_KILL_NPC_QUEST_COUNT",MAX_KILL_NPC_QUEST_COUNT);
 tolua_constant(tolua_S,"MAX_KILL_COUNT_PER_NPC",MAX_KILL_COUNT_PER_NPC);
  tolua_constant(tolua_S,"qatInvalid",qatInvalid);
  tolua_constant(tolua_S,"qatNpc",qatNpc);
  tolua_constant(tolua_S,"qatShare",qatShare);
  tolua_constant(tolua_S,"qatPublish",qatPublish);
  tolua_constant(tolua_S,"qatEmegency",qatEmegency);
  tolua_constant(tolua_S,"qatItem",qatItem);
  tolua_constant(tolua_S,"qatTotal",qatTotal);
  tolua_constant(tolua_S,"qttInvalid",qttInvalid);
  tolua_constant(tolua_S,"qttNoReset",qttNoReset);
  tolua_constant(tolua_S,"qttDay",qttDay);
  tolua_constant(tolua_S,"qttWeek",qttWeek);
  tolua_constant(tolua_S,"qttTotal",qttTotal);
  tolua_constant(tolua_S,"qrtInvalid",qrtInvalid);
  tolua_constant(tolua_S,"qrtMaster",qrtMaster);
  tolua_constant(tolua_S,"qrtDisciple",qrtDisciple);
  tolua_constant(tolua_S,"qrtHusband",qrtHusband);
  tolua_constant(tolua_S,"qrtWife",qrtWife);
  tolua_constant(tolua_S,"qrtGuildLeader",qrtGuildLeader);
  tolua_constant(tolua_S,"qrtGuildMember",qrtGuildMember);
  tolua_constant(tolua_S,"qrtTeamMaster",qrtTeamMaster);
  tolua_constant(tolua_S,"qrtTeamHusband",qrtTeamHusband);
  tolua_constant(tolua_S,"qrtTeamGuild",qrtTeamGuild);
  tolua_constant(tolua_S,"qrtTotal",qrtTotal);
  tolua_constant(tolua_S,"qtInvalid",qtInvalid);
  tolua_constant(tolua_S,"qtTrunk",qtTrunk);
  tolua_constant(tolua_S,"qtBranch",qtBranch);
  tolua_constant(tolua_S,"qtDaily",qtDaily);
  tolua_constant(tolua_S,"qtReward",qtReward);
  tolua_constant(tolua_S,"qtCircle",qtCircle);
  tolua_constant(tolua_S,"qtActivity",qtActivity);
  tolua_constant(tolua_S,"qtGuild",qtGuild);
  tolua_constant(tolua_S,"qtMaster",qtMaster);
  tolua_constant(tolua_S,"qtWedding",qtWedding);
  tolua_constant(tolua_S,"qtAdventure",qtAdventure);
  tolua_constant(tolua_S,"qtGuide",qtGuide);
  tolua_constant(tolua_S,"qtEmegency",qtEmegency);
  tolua_constant(tolua_S,"qtTotal",qtTotal);
 tolua_constant(tolua_S,"MAX_TEAM_MEMBER_COUNT",MAX_TEAM_MEMBER_COUNT);
 tolua_constant(tolua_S,"MAX_ADJACENT_ROLE_NOTEAM_COUNT",MAX_ADJACENT_ROLE_NOTEAM_COUNT);
 tolua_constant(tolua_S,"MAX_ADJACENT_TEAM_COUNT",MAX_ADJACENT_TEAM_COUNT);
 tolua_constant(tolua_S,"TEAM_SYNC_MEMBERINFO_TO_MS",TEAM_SYNC_MEMBERINFO_TO_MS);
 tolua_constant(tolua_S,"TEAM_SYNC_MEMBERINFO_TO_CLIENT",TEAM_SYNC_MEMBERINFO_TO_CLIENT);
 tolua_constant(tolua_S,"TEAM_SYNC_MEMBERINFO_STOP",TEAM_SYNC_MEMBERINFO_STOP);
 tolua_constant(tolua_S,"MAX_TEAM_COUNT_PER_PAGE",MAX_TEAM_COUNT_PER_PAGE);
 tolua_constant(tolua_S,"MAX_TEAM_PAGE_COUNT",MAX_TEAM_PAGE_COUNT);
 tolua_constant(tolua_S,"MAX_TEAM_COUNT_PER_ACTIVITY",MAX_TEAM_COUNT_PER_ACTIVITY);
 tolua_constant(tolua_S,"MAX_TEAM_ACTIVITY",MAX_TEAM_ACTIVITY);
 tolua_constant(tolua_S,"MAX_TEAM_DUNGEON",MAX_TEAM_DUNGEON);
 tolua_constant(tolua_S,"TEAM_AUCTION_BID_FRAME_INTERVAL",TEAM_AUCTION_BID_FRAME_INTERVAL);
 tolua_constant(tolua_S,"TEAM_AUCTION_ITEM_INIT_INDEX",TEAM_AUCTION_ITEM_INIT_INDEX);
  tolua_constant(tolua_S,"etatInvite",etatInvite);
  tolua_constant(tolua_S,"etatEnter",etatEnter);
  tolua_constant(tolua_S,"etarAgree",etarAgree);
  tolua_constant(tolua_S,"etarDisAgree",etarDisAgree);
  tolua_constant(tolua_S,"etaInvalid",etaInvalid);
  tolua_constant(tolua_S,"etaFreeForAll",etaFreeForAll);
  tolua_constant(tolua_S,"etaTeamAuction",etaTeamAuction);
  tolua_constant(tolua_S,"etaLeaderPick",etaLeaderPick);
  tolua_constant(tolua_S,"etaTotal",etaTotal);
  tolua_constant(tolua_S,"etsOnline",etsOnline);
  tolua_constant(tolua_S,"etsOffline",etsOffline);
  tolua_constant(tolua_S,"etaatInvalid",etaatInvalid);
  tolua_constant(tolua_S,"etaatActivity",etaatActivity);
  tolua_constant(tolua_S,"etaatDungeon",etaatDungeon);
  tolua_constant(tolua_S,"etaatTotal",etaatTotal);
  tolua_constant(tolua_S,"etbsInvalid",etbsInvalid);
  tolua_constant(tolua_S,"etbsBidding",etbsBidding);
  tolua_constant(tolua_S,"etbsAbandon",etbsAbandon);
  tolua_constant(tolua_S,"etbsTotal",etbsTotal);
  tolua_constant(tolua_S,"TAA_ADDITEM",TAA_ADDITEM);
  tolua_constant(tolua_S,"TAA_START_AUCTION",TAA_START_AUCTION);
  tolua_constant(tolua_S,"TAA_BID",TAA_BID);
  tolua_constant(tolua_S,"TAA_ABANDON_SELF_REQUEST",TAA_ABANDON_SELF_REQUEST);
  tolua_constant(tolua_S,"TAA_ABANDON_LEAVE_SCENE",TAA_ABANDON_LEAVE_SCENE);
  tolua_constant(tolua_S,"TAA_REWARD_MONEY",TAA_REWARD_MONEY);
  tolua_constant(tolua_S,"TAA_WIN_ITEM",TAA_WIN_ITEM);
  tolua_constant(tolua_S,"TAA_LEADER_GOT_ITEM",TAA_LEADER_GOT_ITEM);
  tolua_constant(tolua_S,"TAA_RETURN_DEPOSIT",TAA_RETURN_DEPOSIT);
  tolua_constant(tolua_S,"TAA_FINISH_AUCTION",TAA_FINISH_AUCTION);
  tolua_constant(tolua_S,"TAA_TOTAL",TAA_TOTAL);
  tolua_constant(tolua_S,"TAMR_NO_CHANGE",TAMR_NO_CHANGE);
  tolua_constant(tolua_S,"TAMR_OFFER_PRICE",TAMR_OFFER_PRICE);
  tolua_constant(tolua_S,"TAMR_ABANDON",TAMR_ABANDON);
  tolua_constant(tolua_S,"TAMR_REWARD",TAMR_REWARD);
  tolua_constant(tolua_S,"TAMR_RETURN_DEPOSITE",TAMR_RETURN_DEPOSITE);
 tolua_constant(tolua_S,"MAX_ADVENTURE_TRIGGER_PARAM",MAX_ADVENTURE_TRIGGER_PARAM);
 tolua_constant(tolua_S,"MAX_ADVENTURE_REWARD_PARAM",MAX_ADVENTURE_REWARD_PARAM);
  tolua_constant(tolua_S,"attInvalid",attInvalid);
  tolua_constant(tolua_S,"attKillNpc",attKillNpc);
  tolua_constant(tolua_S,"attDestoryDoodad",attDestoryDoodad);
  tolua_constant(tolua_S,"attRefineEquip",attRefineEquip);
  tolua_constant(tolua_S,"attAddSoul",attAddSoul);
  tolua_constant(tolua_S,"attRandomizeEquip",attRandomizeEquip);
  tolua_constant(tolua_S,"att5MinOnlineTime",att5MinOnlineTime);
  tolua_constant(tolua_S,"attServerTime",attServerTime);
  tolua_constant(tolua_S,"attTotal",attTotal);
  tolua_constant(tolua_S,"adrInvalid",adrInvalid);
  tolua_constant(tolua_S,"adrSpawnNpcAtPoint",adrSpawnNpcAtPoint);
  tolua_constant(tolua_S,"adrSpawnNpcNearby",adrSpawnNpcNearby);
  tolua_constant(tolua_S,"adrSpawnNpcSurround",adrSpawnNpcSurround);
  tolua_constant(tolua_S,"adrSpawnDoodadAtPoint",adrSpawnDoodadAtPoint);
  tolua_constant(tolua_S,"adrSpawnDoodadNearby",adrSpawnDoodadNearby);
  tolua_constant(tolua_S,"adrSendMail",adrSendMail);
  tolua_constant(tolua_S,"adrTotal",adrTotal);
  tolua_constant(tolua_S,"eotInvalid",eotInvalid);
  tolua_constant(tolua_S,"eotBuy",eotBuy);
  tolua_constant(tolua_S,"eotSell",eotSell);
  tolua_constant(tolua_S,"eosInvalid",eosInvalid);
  tolua_constant(tolua_S,"eosEffective",eosEffective);
  tolua_constant(tolua_S,"eosCanceled",eosCanceled);
  tolua_constant(tolua_S,"eosExpired",eosExpired);
  tolua_constant(tolua_S,"eosFinished",eosFinished);
  tolua_constant(tolua_S,"eosError",eosError);
  tolua_constant(tolua_S,"eosTotal",eosTotal);
 tolua_constant(tolua_S,"MAX_ORDER_DETAIL_NUM",MAX_ORDER_DETAIL_NUM);
 tolua_constant(tolua_S,"MAX_CON_PER_ACHIEVEMENT",MAX_CON_PER_ACHIEVEMENT);
 tolua_constant(tolua_S,"ACHIEVEMENT_MAX",ACHIEVEMENT_MAX);
 tolua_constant(tolua_S,"ACHIEVEMENT_PK_VALUE",ACHIEVEMENT_PK_VALUE);
 tolua_constant(tolua_S,"INVALID_ACHIEVEMENT_ID",INVALID_ACHIEVEMENT_ID);
 tolua_constant(tolua_S,"MAX_REWARD_PER_ACHIEVEMENT",MAX_REWARD_PER_ACHIEVEMENT);
 tolua_constant(tolua_S,"MAX_ACHIEVEMENT_STATISTIC_COUNT",MAX_ACHIEVEMENT_STATISTIC_COUNT);
 tolua_constant(tolua_S,"MAX_ACHIEVEMENT_WEEKLY_UPDATE_TIME",MAX_ACHIEVEMENT_WEEKLY_UPDATE_TIME);
 tolua_constant(tolua_S,"MAX_ACHIEVEMENT_MONTHLY_UPDATE_TIME",MAX_ACHIEVEMENT_MONTHLY_UPDATE_TIME);
 tolua_constant(tolua_S,"MAX_HANKBOOK_COUNT_PER_TYPE",MAX_HANKBOOK_COUNT_PER_TYPE);
  tolua_constant(tolua_S,"htInvalid",htInvalid);
  tolua_constant(tolua_S,"htNpcBegin",htNpcBegin);
  tolua_constant(tolua_S,"htNpc_Normal",htNpc_Normal);
  tolua_constant(tolua_S,"htNpc_Elite",htNpc_Elite);
  tolua_constant(tolua_S,"htNPc_Boss",htNPc_Boss);
  tolua_constant(tolua_S,"htNpc_Function",htNpc_Function);
  tolua_constant(tolua_S,"htNpc_Story",htNpc_Story);
  tolua_constant(tolua_S,"htNpcEnd",htNpcEnd);
  tolua_constant(tolua_S,"htMap_Wild",htMap_Wild);
  tolua_constant(tolua_S,"htMap_Dungeon",htMap_Dungeon);
  tolua_constant(tolua_S,"htMap_Activity",htMap_Activity);
  tolua_constant(tolua_S,"htItem_Compose",htItem_Compose);
  tolua_constant(tolua_S,"htItem_Stone",htItem_Stone);
  tolua_constant(tolua_S,"htItem_Story",htItem_Story);
  tolua_constant(tolua_S,"htEquip_Begin",htEquip_Begin);
  tolua_constant(tolua_S,"htEquip_Set",htEquip_Set);
  tolua_constant(tolua_S,"htEquip_Weapon",htEquip_Weapon);
  tolua_constant(tolua_S,"htEquip_Jewelry",htEquip_Jewelry);
  tolua_constant(tolua_S,"htEquip_Armor",htEquip_Armor);
  tolua_constant(tolua_S,"htEquip_End",htEquip_End);
  tolua_constant(tolua_S,"htRune_Warrior",htRune_Warrior);
  tolua_constant(tolua_S,"htRune_Mage",htRune_Mage);
  tolua_constant(tolua_S,"htRune_Rogue",htRune_Rogue);
  tolua_constant(tolua_S,"htRune_Archer",htRune_Archer);
  tolua_constant(tolua_S,"htRune_Taoist",htRune_Taoist);
  tolua_constant(tolua_S,"htAir_Green",htAir_Green);
  tolua_constant(tolua_S,"htAir_Blue",htAir_Blue);
  tolua_constant(tolua_S,"htAir_Purple",htAir_Purple);
  tolua_constant(tolua_S,"htTotal",htTotal);
  tolua_constant(tolua_S,"adtInvalid",adtInvalid);
  tolua_constant(tolua_S,"adtTitle",adtTitle);
  tolua_constant(tolua_S,"adtItem",adtItem);
  tolua_constant(tolua_S,"adtExp",adtExp);
  tolua_constant(tolua_S,"adtTotal",adtTotal);
  tolua_constant(tolua_S,"acrtInvalid",acrtInvalid);
  tolua_constant(tolua_S,"acrtWeek",acrtWeek);
  tolua_constant(tolua_S,"acrtMonth",acrtMonth);
  tolua_constant(tolua_S,"acrtTotal",acrtTotal);
  tolua_constant(tolua_S,"adInvalid",adInvalid);
  tolua_constant(tolua_S,"adLevel",adLevel);
  tolua_constant(tolua_S,"adExperience",adExperience);
  tolua_constant(tolua_S,"adPkValue",adPkValue);
  tolua_constant(tolua_S,"adForceReputation",adForceReputation);
  tolua_constant(tolua_S,"adVariousForceCount",adVariousForceCount);
  tolua_constant(tolua_S,"adGetItem",adGetItem);
  tolua_constant(tolua_S,"adItemLuck",adItemLuck);
  tolua_constant(tolua_S,"adItemRefine",adItemRefine);
  tolua_constant(tolua_S,"adRefineLevel",adRefineLevel);
  tolua_constant(tolua_S,"adEquip",adEquip);
  tolua_constant(tolua_S,"adEquipColor",adEquipColor);
  tolua_constant(tolua_S,"adEquipBodyColor",adEquipBodyColor);
  tolua_constant(tolua_S,"adSet",adSet);
  tolua_constant(tolua_S,"adSetCount",adSetCount);
  tolua_constant(tolua_S,"adBankSlotCount",adBankSlotCount);
  tolua_constant(tolua_S,"adBagSlotCount",adBagSlotCount);
  tolua_constant(tolua_S,"adItemBagSlotCount",adItemBagSlotCount);
  tolua_constant(tolua_S,"adEquipSlotCount",adEquipSlotCount);
  tolua_constant(tolua_S,"adMaterialBagSlotCount",adMaterialBagSlotCount);
  tolua_constant(tolua_S,"adTotalMoneyType",adTotalMoneyType);
  tolua_constant(tolua_S,"adKillPlayerTotalCount",adKillPlayerTotalCount);
  tolua_constant(tolua_S,"adEnterScene",adEnterScene);
  tolua_constant(tolua_S,"adCompleteQuest",adCompleteQuest);
  tolua_constant(tolua_S,"adCompleteQuestCount",adCompleteQuestCount);
  tolua_constant(tolua_S,"adSpellLevelSpecified",adSpellLevelSpecified);
  tolua_constant(tolua_S,"adSpellLevel",adSpellLevel);
  tolua_constant(tolua_S,"adLearnSpell",adLearnSpell);
  tolua_constant(tolua_S,"adSpellCount",adSpellCount);
  tolua_constant(tolua_S,"adActiveSpellCount",adActiveSpellCount);
  tolua_constant(tolua_S,"adRuneCount",adRuneCount);
  tolua_constant(tolua_S,"adEquipPassiveSpellCount",adEquipPassiveSpellCount);
  tolua_constant(tolua_S,"adEquipPassiveSpellIDCount",adEquipPassiveSpellIDCount);
  tolua_constant(tolua_S,"adAchievement",adAchievement);
  tolua_constant(tolua_S,"adAchievementCount",adAchievementCount);
  tolua_constant(tolua_S,"adTitle",adTitle);
  tolua_constant(tolua_S,"adTitleCount",adTitleCount);
  tolua_constant(tolua_S,"adFriendCount",adFriendCount);
  tolua_constant(tolua_S,"adLeaveStudentCount",adLeaveStudentCount);
  tolua_constant(tolua_S,"adCreateTime",adCreateTime);
  tolua_constant(tolua_S,"adVariable",adVariable);
  tolua_constant(tolua_S,"adRoleVariable",adRoleVariable);
  tolua_constant(tolua_S,"adKilledByNpcIDCount",adKilledByNpcIDCount);
  tolua_constant(tolua_S,"adKillNpcIDCount",adKillNpcIDCount);
  tolua_constant(tolua_S,"adKillNpcGroupIDCount",adKillNpcGroupIDCount);
  tolua_constant(tolua_S,"adUseItemIDCount",adUseItemIDCount);
  tolua_constant(tolua_S,"adHero",adHero);
  tolua_constant(tolua_S,"adSpiritLevel",adSpiritLevel);
  tolua_constant(tolua_S,"adSpiritAirColorCount",adSpiritAirColorCount);
  tolua_constant(tolua_S,"adSpiritAirColorLevel",adSpiritAirColorLevel);
  tolua_constant(tolua_S,"adSpiritBuyAirEquipSlotCount",adSpiritBuyAirEquipSlotCount);
  tolua_constant(tolua_S,"adSpiritBuyAirBagSlotCount",adSpiritBuyAirBagSlotCount);
  tolua_constant(tolua_S,"adSpiritBuyAirStallSlotCount",adSpiritBuyAirStallSlotCount);
  tolua_constant(tolua_S,"adSpiritBuyBloodSlotCount",adSpiritBuyBloodSlotCount);
  tolua_constant(tolua_S,"adSpiritBloodPageCount",adSpiritBloodPageCount);
  tolua_constant(tolua_S,"adSpiritBloodPageCompleteCount",adSpiritBloodPageCompleteCount);
  tolua_constant(tolua_S,"adSpiritBloodPageLevel",adSpiritBloodPageLevel);
  tolua_constant(tolua_S,"adEquipDecompose",adEquipDecompose);
  tolua_constant(tolua_S,"adEquipColorDecompose",adEquipColorDecompose);
  tolua_constant(tolua_S,"adEquipCompose",adEquipCompose);
  tolua_constant(tolua_S,"adEquipColorCompose",adEquipColorCompose);
  tolua_constant(tolua_S,"adVip",adVip);
  tolua_constant(tolua_S,"adSubscribeVipTotalMonth",adSubscribeVipTotalMonth);
  tolua_constant(tolua_S,"adJoinTeamCount",adJoinTeamCount);
  tolua_constant(tolua_S,"adAuctionBuyItemCount",adAuctionBuyItemCount);
  tolua_constant(tolua_S,"adAuctionSellItemCount",adAuctionSellItemCount);
  tolua_constant(tolua_S,"adRanklist",adRanklist);
  tolua_constant(tolua_S,"adColorItemCompose",adColorItemCompose);
  tolua_constant(tolua_S,"adColorItemDeCompose",adColorItemDeCompose);
  tolua_constant(tolua_S,"adRuneWeaponCount",adRuneWeaponCount);
  tolua_constant(tolua_S,"adExplore",adExplore);
  tolua_constant(tolua_S,"adHandbook",adHandbook);
  tolua_constant(tolua_S,"adTotal",adTotal);
  tolua_constant(tolua_S,"avdInvalid",avdInvalid);
  tolua_constant(tolua_S,"avdForSceneBegin",avdForSceneBegin);
  tolua_constant(tolua_S,"avdSceneTempID",avdSceneTempID);
  tolua_constant(tolua_S,"avdSceneID",avdSceneID);
  tolua_constant(tolua_S,"avdReward_1",avdReward_1);
  tolua_constant(tolua_S,"avdReward_2",avdReward_2);
  tolua_constant(tolua_S,"avdReward_3",avdReward_3);
  tolua_constant(tolua_S,"avdForSceneEnd",avdForSceneEnd);
  tolua_constant(tolua_S,"avdStatisticBegin",avdStatisticBegin);
  tolua_constant(tolua_S,"avdDestoryItemCount",avdDestoryItemCount);
  tolua_constant(tolua_S,"avdPickMoneyTotal",avdPickMoneyTotal);
  tolua_constant(tolua_S,"avdQuestMoneyTotal",avdQuestMoneyTotal);
  tolua_constant(tolua_S,"avdSaleMoneyTotal",avdSaleMoneyTotal);
  tolua_constant(tolua_S,"avdRelieveMoneyTotal",avdRelieveMoneyTotal);
  tolua_constant(tolua_S,"avdReliveRMBTotal",avdReliveRMBTotal);
  tolua_constant(tolua_S,"avdRepairMoneyTotal",avdRepairMoneyTotal);
  tolua_constant(tolua_S,"avdRepairMoneyMax",avdRepairMoneyMax);
  tolua_constant(tolua_S,"avdKillNpcTotalCount",avdKillNpcTotalCount);
  tolua_constant(tolua_S,"avdSelfDieCount",avdSelfDieCount);
  tolua_constant(tolua_S,"avdPVPKilledByPlayer",avdPVPKilledByPlayer);
  tolua_constant(tolua_S,"avdPVPKilledByNpc",avdPVPKilledByNpc);
  tolua_constant(tolua_S,"avdKillWarriorCount",avdKillWarriorCount);
  tolua_constant(tolua_S,"avdKillMageCount",avdKillMageCount);
  tolua_constant(tolua_S,"avdKillRogueCount",avdKillRogueCount);
  tolua_constant(tolua_S,"avdKillArcherCount",avdKillArcherCount);
  tolua_constant(tolua_S,"avdKillPkValuePlayerCount",avdKillPkValuePlayerCount);
  tolua_constant(tolua_S,"avdKillWithNoGuilty",avdKillWithNoGuilty);
  tolua_constant(tolua_S,"avdEnterDungeonSceneCount",avdEnterDungeonSceneCount);
  tolua_constant(tolua_S,"avdCompleteStoryQuestCount",avdCompleteStoryQuestCount);
  tolua_constant(tolua_S,"avdCompleteNormalQuestCount",avdCompleteNormalQuestCount);
  tolua_constant(tolua_S,"avdCompleteCircleQuestCount",avdCompleteCircleQuestCount);
  tolua_constant(tolua_S,"avdCompleteRewardQuestCount",avdCompleteRewardQuestCount);
  tolua_constant(tolua_S,"avdCompleteEmegencyQuestCount",avdCompleteEmegencyQuestCount);
  tolua_constant(tolua_S,"avdBrushRewardQuestCount",avdBrushRewardQuestCount);
  tolua_constant(tolua_S,"avdBuyRewardQuestCompleteCount",avdBuyRewardQuestCompleteCount);
  tolua_constant(tolua_S,"avdAbandonQuestCount",avdAbandonQuestCount);
  tolua_constant(tolua_S,"avdMaxHPHit",avdMaxHPHit);
  tolua_constant(tolua_S,"avdMaxHPBeHit",avdMaxHPBeHit);
  tolua_constant(tolua_S,"avdUseItemCount_Useable",avdUseItemCount_Useable);
  tolua_constant(tolua_S,"avdUseItemCount_SpellBoke",avdUseItemCount_SpellBoke);
  tolua_constant(tolua_S,"avdOnlineTime",avdOnlineTime);
  tolua_constant(tolua_S,"avdLeaveMaster",avdLeaveMaster);
  tolua_constant(tolua_S,"avdTotalRefineTime",avdTotalRefineTime);
  tolua_constant(tolua_S,"avdRefineFailTime",avdRefineFailTime);
  tolua_constant(tolua_S,"avdFirstBloodCount",avdFirstBloodCount);
  tolua_constant(tolua_S,"avdIdentifyItemCount",avdIdentifyItemCount);
  tolua_constant(tolua_S,"avdDropItemBeKilled",avdDropItemBeKilled);
  tolua_constant(tolua_S,"avdStatisticEnd",avdStatisticEnd);
  tolua_constant(tolua_S,"avdQuestFlytoZone",avdQuestFlytoZone);
  tolua_constant(tolua_S,"avdLastOfflineTime",avdLastOfflineTime);
  tolua_constant(tolua_S,"avdRewardQuestCompleteCount",avdRewardQuestCompleteCount);
  tolua_constant(tolua_S,"avdKillPlayerLevelDuarionBegin",avdKillPlayerLevelDuarionBegin);
  tolua_constant(tolua_S,"avdKillPlayerLevelDuarion1",avdKillPlayerLevelDuarion1);
  tolua_constant(tolua_S,"avdKillPlayerLevelDuarion2",avdKillPlayerLevelDuarion2);
  tolua_constant(tolua_S,"avdKillPlayerLevelDuarion3",avdKillPlayerLevelDuarion3);
  tolua_constant(tolua_S,"avdEquipDecompose",avdEquipDecompose);
  tolua_constant(tolua_S,"avdEquipCompose",avdEquipCompose);
  tolua_constant(tolua_S,"avdTeamAuctionCount",avdTeamAuctionCount);
  tolua_constant(tolua_S,"avdVip",avdVip);
  tolua_constant(tolua_S,"avdSubscribeVipMonth",avdSubscribeVipMonth);
  tolua_constant(tolua_S,"avdJoinTeamCount",avdJoinTeamCount);
  tolua_constant(tolua_S,"avdComposeOrangeItem",avdComposeOrangeItem);
  tolua_constant(tolua_S,"avdComposePurpleItem",avdComposePurpleItem);
  tolua_constant(tolua_S,"avdDecomposeOrangeItem",avdDecomposeOrangeItem);
  tolua_constant(tolua_S,"avdDecomposePurpleItem",avdDecomposePurpleItem);
  tolua_constant(tolua_S,"avdComposeItemCount",avdComposeItemCount);
  tolua_constant(tolua_S,"avdDeComposeItemCount",avdDeComposeItemCount);
  tolua_constant(tolua_S,"avdAuctionSellCount",avdAuctionSellCount);
  tolua_constant(tolua_S,"avdAuctionBid",avdAuctionBid);
  tolua_constant(tolua_S,"avdMaxConsecutiveKillNpcCount",avdMaxConsecutiveKillNpcCount);
  tolua_constant(tolua_S,"avdMaxMomentaryKillNpcCount",avdMaxMomentaryKillNpcCount);
  tolua_constant(tolua_S,"avdMaxConsecutiveKillPlayerCount",avdMaxConsecutiveKillPlayerCount);
  tolua_constant(tolua_S,"avdRealMoneyToItemCount",avdRealMoneyToItemCount);
  tolua_constant(tolua_S,"avdRealMoneyToMoney02Count",avdRealMoneyToMoney02Count);
  tolua_constant(tolua_S,"avdSpiritLevelUP",avdSpiritLevelUP);
  tolua_constant(tolua_S,"avdKillNpcNormal",avdKillNpcNormal);
  tolua_constant(tolua_S,"avdKillNpcElit",avdKillNpcElit);
  tolua_constant(tolua_S,"avdKillNpcBoss",avdKillNpcBoss);
  tolua_constant(tolua_S,"avdCompleteGuildQuest",avdCompleteGuildQuest);
  tolua_constant(tolua_S,"avdCompleteAdventureQuest",avdCompleteAdventureQuest);
  tolua_constant(tolua_S,"avdItemRepairCount",avdItemRepairCount);
  tolua_constant(tolua_S,"avdRuneAddCount",avdRuneAddCount);
  tolua_constant(tolua_S,"avdCompleteQuestTotalCount",avdCompleteQuestTotalCount);
  tolua_constant(tolua_S,"avdKillNpcRare",avdKillNpcRare);
  tolua_constant(tolua_S,"avdKillNpcCorpse",avdKillNpcCorpse);
  tolua_constant(tolua_S,"avdLevelOnlineTime",avdLevelOnlineTime);
  tolua_constant(tolua_S,"avdRankPow",avdRankPow);
  tolua_constant(tolua_S,"avdRankLevel",avdRankLevel);
  tolua_constant(tolua_S,"avdRankMoney",avdRankMoney);
  tolua_constant(tolua_S,"avdRankPkValue",avdRankPkValue);
  tolua_constant(tolua_S,"avdRankMasterExp",avdRankMasterExp);
  tolua_constant(tolua_S,"avdRankAchievementPts",avdRankAchievementPts);
  tolua_constant(tolua_S,"avdExeciseTime",avdExeciseTime);
  tolua_constant(tolua_S,"avdTotal",avdTotal);
 tolua_constant(tolua_S,"MAX_STUDENT_NUM",MAX_STUDENT_NUM);
 tolua_constant(tolua_S,"GRADUATE_LEVEL",GRADUATE_LEVEL);
 tolua_constant(tolua_S,"STUD_LEVEL_LOWER_LIMIT",STUD_LEVEL_LOWER_LIMIT);
 tolua_constant(tolua_S,"REGET_MASTER_CD",REGET_MASTER_CD);
 tolua_constant(tolua_S,"APPLY_VALID_TIME",APPLY_VALID_TIME);
 tolua_constant(tolua_S,"SEND_MASTER_INFO_NUM",SEND_MASTER_INFO_NUM);
 tolua_constant(tolua_S,"SEND_SOLO_INFO_NUM",SEND_SOLO_INFO_NUM);
 tolua_constant(tolua_S,"MAX_CZ_TEXT_LENGTH",MAX_CZ_TEXT_LENGTH);
 tolua_constant(tolua_S,"MAX_MASTER_SEND_APP_NUM",MAX_MASTER_SEND_APP_NUM);
 tolua_constant(tolua_S,"MAX_MASTER_RECV_APP_NUM",MAX_MASTER_RECV_APP_NUM);
  tolua_constant(tolua_S,"MASTER_NONE",MASTER_NONE);
  tolua_constant(tolua_S,"MASTER_STUDENT",MASTER_STUDENT);
  tolua_constant(tolua_S,"MASTER_TEACHER",MASTER_TEACHER);
  tolua_constant(tolua_S,"MASTER_BECOME_TEACHER",MASTER_BECOME_TEACHER);
  tolua_constant(tolua_S,"MASTER_CONG_BEGIN",MASTER_CONG_BEGIN);
  tolua_constant(tolua_S,"MASTER_CONG_BY_FREE",MASTER_CONG_BY_FREE);
  tolua_constant(tolua_S,"MASTER_CONG_BY_MONEY",MASTER_CONG_BY_MONEY);
  tolua_constant(tolua_S,"MASTER_CONG_BY_GOLD",MASTER_CONG_BY_GOLD);
  tolua_constant(tolua_S,"MASTER_CONG_END",MASTER_CONG_END);
  tolua_constant(tolua_S,"MTAT_FROM_COMMON",MTAT_FROM_COMMON);
  tolua_constant(tolua_S,"MTAT_FROM_TWITTER",MTAT_FROM_TWITTER);
  tolua_constant(tolua_S,"MST_APPLY_RECRUIT_SUCC",MST_APPLY_RECRUIT_SUCC);
  tolua_constant(tolua_S,"MST_CANCEL_RECRUIT_SUCC",MST_CANCEL_RECRUIT_SUCC);
  tolua_constant(tolua_S,"MST_SYNC_GS_CD",MST_SYNC_GS_CD);
  tolua_constant(tolua_S,"MST_SYNC_GS_TWITTER_CD",MST_SYNC_GS_TWITTER_CD);
  tolua_constant(tolua_S,"MST_SYNC_PUNISH_EXP",MST_SYNC_PUNISH_EXP);
  tolua_constant(tolua_S,"MST_SYNC_MASTER_EXP_DEC",MST_SYNC_MASTER_EXP_DEC);
 tolua_constant(tolua_S,"MAX_BATTLE_TEMPLATE_COUNT",MAX_BATTLE_TEMPLATE_COUNT);
 tolua_constant(tolua_S,"MAX_ARENA_GROUP_COUNT",MAX_ARENA_GROUP_COUNT);
 tolua_constant(tolua_S,"MAX_ARENA_MATCH_GROUP",MAX_ARENA_MATCH_GROUP);
 tolua_constant(tolua_S,"MAX_ARENA_TEMPLATE_COUNT",MAX_ARENA_TEMPLATE_COUNT);
 tolua_constant(tolua_S,"MAX_ABNORMAL_LEAVE_TIME",MAX_ABNORMAL_LEAVE_TIME);
 tolua_constant(tolua_S,"MAX_JOIN_CONFIRM_TIME",MAX_JOIN_CONFIRM_TIME);
  tolua_constant(tolua_S,"bttInvalid",bttInvalid);
  tolua_constant(tolua_S,"bttFixed",bttFixed);
  tolua_constant(tolua_S,"btt5V5",btt5V5);
  tolua_constant(tolua_S,"btt10V10",btt10V10);
  tolua_constant(tolua_S,"btt15V15",btt15V15);
  tolua_constant(tolua_S,"btt10V10V10",btt10V10V10);
  tolua_constant(tolua_S,"btt20V20",btt20V20);
  tolua_constant(tolua_S,"bttTotal",bttTotal);
  tolua_constant(tolua_S,"bfInvalid",bfInvalid);
  tolua_constant(tolua_S,"bfFirst",bfFirst);
  tolua_constant(tolua_S,"bfSecond",bfSecond);
  tolua_constant(tolua_S,"bfThrid",bfThrid);
  tolua_constant(tolua_S,"bfFourth",bfFourth);
  tolua_constant(tolua_S,"bfTotal",bfTotal);
  tolua_constant(tolua_S,"qsInvalid",qsInvalid);
  tolua_constant(tolua_S,"qsNotInQueue",qsNotInQueue);
  tolua_constant(tolua_S,"qsInQueue",qsInQueue);
  tolua_constant(tolua_S,"qsQueueConfirm",qsQueueConfirm);
  tolua_constant(tolua_S,"qsTotal",qsTotal);
 tolua_cclass(tolua_S,"QUEUE_DATA","QUEUE_DATA","",NULL);
 tolua_beginmodule(tolua_S,"QUEUE_DATA");
  tolua_variable(tolua_S,"byQueue",tolua_get_QUEUE_DATA_unsigned_byQueue,tolua_set_QUEUE_DATA_unsigned_byQueue);
  tolua_variable(tolua_S,"dwJoinConfirmTimeOut",tolua_get_QUEUE_DATA_unsigned_dwJoinConfirmTimeOut,tolua_set_QUEUE_DATA_unsigned_dwJoinConfirmTimeOut);
  tolua_variable(tolua_S,"dwJoinSceneTempID",tolua_get_QUEUE_DATA_unsigned_dwJoinSceneTempID,tolua_set_QUEUE_DATA_unsigned_dwJoinSceneTempID);
  tolua_variable(tolua_S,"dwJoinSceneID",tolua_get_QUEUE_DATA_unsigned_dwJoinSceneID,tolua_set_QUEUE_DATA_unsigned_dwJoinSceneID);
  tolua_variable(tolua_S,"dwJoinSide",tolua_get_QUEUE_DATA_unsigned_dwJoinSide,tolua_set_QUEUE_DATA_unsigned_dwJoinSide);
 tolua_endmodule(tolua_S);
 tolua_constant(tolua_S,"MAX_SCENE_VARIABLE_COUNT",MAX_SCENE_VARIABLE_COUNT);
 tolua_constant(tolua_S,"MAX_ROLE_VARIABLE_COUNT",MAX_ROLE_VARIABLE_COUNT);
 tolua_constant(tolua_S,"MAX_CLIENT_VARIABLE_COUNT",MAX_CLIENT_VARIABLE_COUNT);
 tolua_constant(tolua_S,"MAX_GUILD_VARIABLE_COUNT",MAX_GUILD_VARIABLE_COUNT);
 tolua_constant(tolua_S,"MAX_VARIABLE_PER_QUEST",MAX_VARIABLE_PER_QUEST);
 tolua_constant(tolua_S,"MAX_AI_VARIABLE_COUNT",MAX_AI_VARIABLE_COUNT);
 tolua_constant(tolua_S,"VARIABLE_ID_MASK",VARIABLE_ID_MASK);
 tolua_constant(tolua_S,"INVALID_VARIABLE_ID",INVALID_VARIABLE_ID);
  tolua_constant(tolua_S,"vtInvalid",vtInvalid);
  tolua_constant(tolua_S,"vtRole",vtRole);
  tolua_constant(tolua_S,"vtScene",vtScene);
  tolua_constant(tolua_S,"vtGuild",vtGuild);
  tolua_constant(tolua_S,"vtQuest",vtQuest);
  tolua_constant(tolua_S,"vtItemRecycle",vtItemRecycle);
  tolua_constant(tolua_S,"vtKillNpcCount",vtKillNpcCount);
  tolua_constant(tolua_S,"vtClient",vtClient);
  tolua_constant(tolua_S,"vtAI",vtAI);
  tolua_constant(tolua_S,"vtAISpell",vtAISpell);
  tolua_constant(tolua_S,"vtServer",vtServer);
  tolua_constant(tolua_S,"vtKillDoodad",vtKillDoodad);
  tolua_constant(tolua_S,"vtAchievement",vtAchievement);
  tolua_constant(tolua_S,"vtKillPlayerCount",vtKillPlayerCount);
  tolua_constant(tolua_S,"vtKillItemNpcCount",vtKillItemNpcCount);
  tolua_constant(tolua_S,"vtTotal",vtTotal);
  tolua_constant(tolua_S,"vRole",vRole);
  tolua_constant(tolua_S,"vScene",vScene);
  tolua_constant(tolua_S,"vGuild",vGuild);
  tolua_constant(tolua_S,"vQuest",vQuest);
  tolua_constant(tolua_S,"vClient",vClient);
  tolua_constant(tolua_S,"vAI",vAI);
  tolua_constant(tolua_S,"vServer",vServer);
  tolua_constant(tolua_S,"svtHighPrice",svtHighPrice);
  tolua_constant(tolua_S,"svtLowPrice",svtLowPrice);
  tolua_constant(tolua_S,"svtMaxSingleCount",svtMaxSingleCount);
  tolua_constant(tolua_S,"svtMaxDailyCount",svtMaxDailyCount);
  tolua_constant(tolua_S,"svtAutoOrderEnabled",svtAutoOrderEnabled);
  tolua_constant(tolua_S,"svtMaxCountPerPrice",svtMaxCountPerPrice);
  tolua_constant(tolua_S,"svtActiveOrderEnabled",svtActiveOrderEnabled);
  tolua_constant(tolua_S,"svtNextMonthlyUpdateTime",svtNextMonthlyUpdateTime);
  tolua_constant(tolua_S,"svtServerStartIndex",svtServerStartIndex);
  tolua_constant(tolua_S,"svtStartRewardRate",svtStartRewardRate);
  tolua_constant(tolua_S,"svtSabacWinner0",svtSabacWinner0);
  tolua_constant(tolua_S,"svtSabacWinner1",svtSabacWinner1);
  tolua_constant(tolua_S,"svtSabacWinner2",svtSabacWinner2);
  tolua_constant(tolua_S,"svtSabacWinner3",svtSabacWinner3);
  tolua_constant(tolua_S,"svtSabacWinner4",svtSabacWinner4);
  tolua_constant(tolua_S,"svtSabacWinner5",svtSabacWinner5);
  tolua_constant(tolua_S,"svtSabacDoor",svtSabacDoor);
  tolua_constant(tolua_S,"svtSabacWall1",svtSabacWall1);
  tolua_constant(tolua_S,"svtSabacWall2",svtSabacWall2);
  tolua_constant(tolua_S,"svtSabacWall3",svtSabacWall3);
  tolua_constant(tolua_S,"svtSabacArcher1",svtSabacArcher1);
  tolua_constant(tolua_S,"svtSabacArcher2",svtSabacArcher2);
  tolua_constant(tolua_S,"svtSabacArcher3",svtSabacArcher3);
  tolua_constant(tolua_S,"svtSabacArcher4",svtSabacArcher4);
  tolua_constant(tolua_S,"svtSabacArcher5",svtSabacArcher5);
  tolua_constant(tolua_S,"svtSabacArcher6",svtSabacArcher6);
  tolua_constant(tolua_S,"svtSabacArcher7",svtSabacArcher7);
  tolua_constant(tolua_S,"svtSabacArcher8",svtSabacArcher8);
  tolua_constant(tolua_S,"svtSabacArcher9",svtSabacArcher9);
  tolua_constant(tolua_S,"svtSabacArcher10",svtSabacArcher10);
  tolua_constant(tolua_S,"svtSabacArcher11",svtSabacArcher11);
  tolua_constant(tolua_S,"svtSabacArcher12",svtSabacArcher12);
  tolua_constant(tolua_S,"svtArenaFund",svtArenaFund);
  tolua_constant(tolua_S,"svtEnd",svtEnd);
#ifndef TOLUA_DISABLE_tolua_enum_MakeVariableID00
 tolua_function(tolua_S,"MakeVariableID",tolua_enum_MakeVariableID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_GetVarTypeFromID00
 tolua_function(tolua_S,"GetVarTypeFromID",tolua_enum_GetVarTypeFromID00);
#endif //#ifndef TOLUA_DISABLE
#ifndef TOLUA_DISABLE_tolua_enum_GetVarIndexFromID00
 tolua_function(tolua_S,"GetVarIndexFromID",tolua_enum_GetVarIndexFromID00);
#endif //#ifndef TOLUA_DISABLE
  tolua_constant(tolua_S,"esvtInvalid",esvtInvalid);
  tolua_constant(tolua_S,"esvtDefault",esvtDefault);
  tolua_constant(tolua_S,"esvtRune",esvtRune);
  tolua_constant(tolua_S,"esvtRuneRefreshTime",esvtRuneRefreshTime);
  tolua_constant(tolua_S,"esvtItemDropLimitType0",esvtItemDropLimitType0);
  tolua_constant(tolua_S,"esvtItemDropLimitType0RefreshTime",esvtItemDropLimitType0RefreshTime);
  tolua_constant(tolua_S,"esvtItemDropLimitType1",esvtItemDropLimitType1);
  tolua_constant(tolua_S,"esvtItemDropLimitType1RefreshTime",esvtItemDropLimitType1RefreshTime);
  tolua_constant(tolua_S,"esvtItemDropLimitType2",esvtItemDropLimitType2);
  tolua_constant(tolua_S,"esvtItemDropLimitType2RefreshTime",esvtItemDropLimitType2RefreshTime);
  tolua_constant(tolua_S,"esvtItemDropLimitLuck2Necklace",esvtItemDropLimitLuck2Necklace);
  tolua_constant(tolua_S,"esvtItemDropLimitLuck2NecklaceRefreshTime",esvtItemDropLimitLuck2NecklaceRefreshTime);
  tolua_constant(tolua_S,"esvtWorldGambleCount",esvtWorldGambleCount);
  tolua_constant(tolua_S,"esvtPreviousWorldGambleCount",esvtPreviousWorldGambleCount);
  tolua_constant(tolua_S,"esvtArena",esvtArena);
  tolua_constant(tolua_S,"esvtGuild",esvtGuild);
  tolua_constant(tolua_S,"esvtSceneLostFaction",esvtSceneLostFaction);
  tolua_constant(tolua_S,"esvtTotal",esvtTotal);
#ifndef TOLUA_DISABLE_tolua_enum_MakeServerKey00
 tolua_function(tolua_S,"MakeServerKey",tolua_enum_MakeServerKey00);
#endif //#ifndef TOLUA_DISABLE
 tolua_constant(tolua_S,"GUILD_CREATER_LEVEL",GUILD_CREATER_LEVEL);
 tolua_constant(tolua_S,"GUILD_CREATE_MONEY",GUILD_CREATE_MONEY);
 tolua_constant(tolua_S,"GUILD_CREATE_ITEM_ID",GUILD_CREATE_ITEM_ID);
 tolua_constant(tolua_S,"GUILD_CREATE_ITEM_NUM",GUILD_CREATE_ITEM_NUM);
 tolua_constant(tolua_S,"MAX_GUILD_NUM_SYSTEM_LIMIT",MAX_GUILD_NUM_SYSTEM_LIMIT);
 tolua_constant(tolua_S,"MAX_APPLY_GUILD_ROLE_NUM",MAX_APPLY_GUILD_ROLE_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_MEMBER_COUNT",GUILD_MAX_MEMBER_COUNT);
 tolua_constant(tolua_S,"MAX_VIEW_GUILD_LIST_COUNT",MAX_VIEW_GUILD_LIST_COUNT);
 tolua_constant(tolua_S,"ROLE_MAX_APPLICATION",ROLE_MAX_APPLICATION);
 tolua_constant(tolua_S,"GUILD_MAX_ALLIANCE_NUM",GUILD_MAX_ALLIANCE_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_ENEMY_NUM",GUILD_MAX_ENEMY_NUM);
 tolua_constant(tolua_S,"GUILD_DATA_NTF_HISTORY_NUM",GUILD_DATA_NTF_HISTORY_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_CHEST_NTF_NUM",GUILD_MAX_CHEST_NTF_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_HISTORY_EVENT_NUM",GUILD_MAX_HISTORY_EVENT_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_STORAGE_EVENT_NUM",GUILD_MAX_STORAGE_EVENT_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_CHEST_EVENT_NUM",GUILD_MAX_CHEST_EVENT_NUM);
 tolua_constant(tolua_S,"GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG",GUILD_STORAGE_MAX_ITEM_NUM_PER_TAG);
 tolua_constant(tolua_S,"GUILD_STORAGE_MAX_TAG_NUM",GUILD_STORAGE_MAX_TAG_NUM);
 tolua_constant(tolua_S,"GUILD_STORAGE_LOW_LIMIT",GUILD_STORAGE_LOW_LIMIT);
 tolua_constant(tolua_S,"GUILD_MAX_APPLY_ALLIANCE_NUM",GUILD_MAX_APPLY_ALLIANCE_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_WELFARE_AURA_COUNT",GUILD_MAX_WELFARE_AURA_COUNT);
 tolua_constant(tolua_S,"GUILD_MAX_UPGRADE_AURA_COUNT",GUILD_MAX_UPGRADE_AURA_COUNT);
 tolua_constant(tolua_S,"GUILD_MAX_UPGRADE_BUILDING_COUNT",GUILD_MAX_UPGRADE_BUILDING_COUNT);
 tolua_constant(tolua_S,"GUILD_MAX_DUNGEON_COUNT",GUILD_MAX_DUNGEON_COUNT);
 tolua_constant(tolua_S,"ONE_DAY_SECOND",ONE_DAY_SECOND);
 tolua_constant(tolua_S,"GUILD_UPDATE_RANKLIST_STARTTIME",GUILD_UPDATE_RANKLIST_STARTTIME);
 tolua_constant(tolua_S,"GUILD_UPDATE_RANKLIST_ENDIME",GUILD_UPDATE_RANKLIST_ENDIME);
 tolua_constant(tolua_S,"GUILD_DUNGEON_AWARD_NUM",GUILD_DUNGEON_AWARD_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_ITEM_BUFFER_LENGTH",GUILD_MAX_ITEM_BUFFER_LENGTH);
 tolua_constant(tolua_S,"GUILD_MAX_BUILDING_LEVEL",GUILD_MAX_BUILDING_LEVEL);
 tolua_constant(tolua_S,"GUILD_MAX_BUILDING_NUM",GUILD_MAX_BUILDING_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_GATE_NUM",GUILD_MAX_GATE_NUM);
 tolua_constant(tolua_S,"MAX_SABAC_ENROLL_COUNT",MAX_SABAC_ENROLL_COUNT);
 tolua_constant(tolua_S,"GUILD_MAX_ACHIEVEMENT_NUM",GUILD_MAX_ACHIEVEMENT_NUM);
 tolua_constant(tolua_S,"GUILD_MAX_ACHI_ARRAY_LENGTH",GUILD_MAX_ACHI_ARRAY_LENGTH);
 tolua_constant(tolua_S,"GUILD_WAR_RECORD_NUM_PER_PAGE",GUILD_WAR_RECORD_NUM_PER_PAGE);
 tolua_constant(tolua_S,"MAX_TIME_DEVIATION",MAX_TIME_DEVIATION);
 tolua_constant(tolua_S,"MAX_GUILD_CORE_MEMBER",MAX_GUILD_CORE_MEMBER);
  tolua_constant(tolua_S,"GUILD_NOTIFY_ROLE_APP",GUILD_NOTIFY_ROLE_APP);
  tolua_constant(tolua_S,"GUILD_NOTIFY_DIPLOMAT_APP",GUILD_NOTIFY_DIPLOMAT_APP);
  tolua_constant(tolua_S,"GUILD_NOTIFY_MATCH_WAR",GUILD_NOTIFY_MATCH_WAR);
  tolua_constant(tolua_S,"GUILD_VIEW_UP",GUILD_VIEW_UP);
  tolua_constant(tolua_S,"GUILD_VIEW_DOWN",GUILD_VIEW_DOWN);
  tolua_constant(tolua_S,"GUILD_POSITION_NONE",GUILD_POSITION_NONE);
  tolua_constant(tolua_S,"GUILD_POSITION_LEADER",GUILD_POSITION_LEADER);
  tolua_constant(tolua_S,"GUILD_POSITION_SUBLEADER",GUILD_POSITION_SUBLEADER);
  tolua_constant(tolua_S,"GUILD_POSITION_ELDER",GUILD_POSITION_ELDER);
  tolua_constant(tolua_S,"GUILD_POSITION_BISHOP",GUILD_POSITION_BISHOP);
  tolua_constant(tolua_S,"GUILD_POSITION_DEACON",GUILD_POSITION_DEACON);
  tolua_constant(tolua_S,"GUILD_POSITION_SUBDEACON",GUILD_POSITION_SUBDEACON);
  tolua_constant(tolua_S,"GUILD_POSITION_CIVILIAN",GUILD_POSITION_CIVILIAN);
  tolua_constant(tolua_S,"GUILD_POSITION_TOTAL",GUILD_POSITION_TOTAL);
 tolua_constant(tolua_S,"GUILD_POSITION_COUNT",GUILD_POSITION_COUNT);
  tolua_constant(tolua_S,"G_POS_RIGHT_NONE",G_POS_RIGHT_NONE);
  tolua_constant(tolua_S,"G_POS_RIGHT_INVITE",G_POS_RIGHT_INVITE);
  tolua_constant(tolua_S,"G_POS_RIGHT_RECRUIT",G_POS_RIGHT_RECRUIT);
  tolua_constant(tolua_S,"G_POS_RIGHT_FORBID_CHAT",G_POS_RIGHT_FORBID_CHAT);
  tolua_constant(tolua_S,"G_POS_RIGHT_NOTICE",G_POS_RIGHT_NOTICE);
  tolua_constant(tolua_S,"G_POS_RIGHT_SET_STORAGE_RIGHT",G_POS_RIGHT_SET_STORAGE_RIGHT);
  tolua_constant(tolua_S,"G_POS_RIGHT_OPEN_DUNGEON",G_POS_RIGHT_OPEN_DUNGEON);
  tolua_constant(tolua_S,"G_POS_RIGHT_UPGRADE_BUILDING",G_POS_RIGHT_UPGRADE_BUILDING);
  tolua_constant(tolua_S,"G_POS_RIGHT_UPGRADE_WELFARE",G_POS_RIGHT_UPGRADE_WELFARE);
  tolua_constant(tolua_S,"G_POS_RIGHT_CHANGE_POS",G_POS_RIGHT_CHANGE_POS);
  tolua_constant(tolua_S,"G_POS_RIGHT_BANISH",G_POS_RIGHT_BANISH);
  tolua_constant(tolua_S,"G_POS_RIGHT_DIPLOMAT",G_POS_RIGHT_DIPLOMAT);
  tolua_constant(tolua_S,"G_POS_RIGHT_ACTIVITY",G_POS_RIGHT_ACTIVITY);
  tolua_constant(tolua_S,"G_STORAGE_1_POP",G_STORAGE_1_POP);
  tolua_constant(tolua_S,"G_STORAGE_2_POP",G_STORAGE_2_POP);
  tolua_constant(tolua_S,"G_STORAGE_3_POP",G_STORAGE_3_POP);
  tolua_constant(tolua_S,"G_STORAGE_4_POP",G_STORAGE_4_POP);
  tolua_constant(tolua_S,"G_STORAGE_5_POP",G_STORAGE_5_POP);
  tolua_constant(tolua_S,"G_STORAGE_6_POP",G_STORAGE_6_POP);
  tolua_constant(tolua_S,"G_STORAGE_1_PUSH",G_STORAGE_1_PUSH);
  tolua_constant(tolua_S,"G_STORAGE_2_PUSH",G_STORAGE_2_PUSH);
  tolua_constant(tolua_S,"G_STORAGE_3_PUSH",G_STORAGE_3_PUSH);
  tolua_constant(tolua_S,"G_STORAGE_4_PUSH",G_STORAGE_4_PUSH);
  tolua_constant(tolua_S,"G_STORAGE_5_PUSH",G_STORAGE_5_PUSH);
  tolua_constant(tolua_S,"G_STORAGE_6_PUSH",G_STORAGE_6_PUSH);
  tolua_constant(tolua_S,"GUILD_FLAG_IS_CHAT_FORBIDDEN",GUILD_FLAG_IS_CHAT_FORBIDDEN);
  tolua_constant(tolua_S,"G_DIPLOMATIC_TIME_1",G_DIPLOMATIC_TIME_1);
  tolua_constant(tolua_S,"G_DIPLOMATIC_TIME_2",G_DIPLOMATIC_TIME_2);
  tolua_constant(tolua_S,"G_DIPLOMATIC_TIME_3",G_DIPLOMATIC_TIME_3);
  tolua_constant(tolua_S,"G_DIPLOMATIC_TIME_4",G_DIPLOMATIC_TIME_4);
  tolua_constant(tolua_S,"GUILD_REVERT_REFUSE",GUILD_REVERT_REFUSE);
  tolua_constant(tolua_S,"GUILD_REVERT_AGREE",GUILD_REVERT_AGREE);
  tolua_constant(tolua_S,"GUILD_CHAT_FORBID",GUILD_CHAT_FORBID);
  tolua_constant(tolua_S,"GUILD_CHAT_UNFORBID",GUILD_CHAT_UNFORBID);
  tolua_constant(tolua_S,"GUILD_RES_ADD_COUNT",GUILD_RES_ADD_COUNT);
  tolua_constant(tolua_S,"GUILD_RES_DEC_COUNT",GUILD_RES_DEC_COUNT);
  tolua_constant(tolua_S,"GUILD_RES_ADD_PERCENT",GUILD_RES_ADD_PERCENT);
  tolua_constant(tolua_S,"GUILD_RES_DEC_PERCENT",GUILD_RES_DEC_PERCENT);
  tolua_constant(tolua_S,"GUILD_ATTR_BEGIN",GUILD_ATTR_BEGIN);
  tolua_constant(tolua_S,"GUILD_ATTR_MONEY",GUILD_ATTR_MONEY);
  tolua_constant(tolua_S,"GUILD_ATTR_LEVEL",GUILD_ATTR_LEVEL);
  tolua_constant(tolua_S,"GUILD_ATTR_SALARY",GUILD_ATTR_SALARY);
  tolua_constant(tolua_S,"GUILD_ATTR_HERB",GUILD_ATTR_HERB);
  tolua_constant(tolua_S,"GUILD_ATTR_WOOD",GUILD_ATTR_WOOD);
  tolua_constant(tolua_S,"GUILD_ATTR_CRYSTAL",GUILD_ATTR_CRYSTAL);
  tolua_constant(tolua_S,"GUILD_ATTR_SILVER",GUILD_ATTR_SILVER);
  tolua_constant(tolua_S,"GUILD_ATTR_PROSPERITY",GUILD_ATTR_PROSPERITY);
  tolua_constant(tolua_S,"GUILD_ATTR_ATTACK_CD",GUILD_ATTR_ATTACK_CD);
  tolua_constant(tolua_S,"GUILD_ATTR_PROTECTION_TIME",GUILD_ATTR_PROTECTION_TIME);
  tolua_constant(tolua_S,"GUILD_AGREE_BY_APPLY",GUILD_AGREE_BY_APPLY);
  tolua_constant(tolua_S,"GUILD_AGREE_BY_INVITE",GUILD_AGREE_BY_INVITE);
  tolua_constant(tolua_S,"GUILD_LEAVE_ACTIVE",GUILD_LEAVE_ACTIVE);
  tolua_constant(tolua_S,"GUILD_LEAVE_PASSIVE",GUILD_LEAVE_PASSIVE);
  tolua_constant(tolua_S,"GUILD_DIPLOMAT_NEUTRALLY",GUILD_DIPLOMAT_NEUTRALLY);
  tolua_constant(tolua_S,"GUILD_DIPLOMAT_ALLIANCE",GUILD_DIPLOMAT_ALLIANCE);
  tolua_constant(tolua_S,"GUILD_DIPLOMAT_ENEMY",GUILD_DIPLOMAT_ENEMY);
  tolua_constant(tolua_S,"GUILD_DIPLOMAT_TIME_BEGIN",GUILD_DIPLOMAT_TIME_BEGIN);
  tolua_constant(tolua_S,"GUILD_DIPLOMAT_TIME_ONE",GUILD_DIPLOMAT_TIME_ONE);
  tolua_constant(tolua_S,"GUILD_DIPLOMAT_TIME_THREE",GUILD_DIPLOMAT_TIME_THREE);
  tolua_constant(tolua_S,"GUILD_DIPLOMAT_TIME_SEVEN",GUILD_DIPLOMAT_TIME_SEVEN);
  tolua_constant(tolua_S,"GUILD_DIPLOMAT_TIME_THIRTY",GUILD_DIPLOMAT_TIME_THIRTY);
  tolua_constant(tolua_S,"GUILD_DIPLOMAT_TIME_END",GUILD_DIPLOMAT_TIME_END);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_BEGIN",GUILD_HE_TYPE_BEGIN);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_CREATE",GUILD_HE_TYPE_CREATE);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_JOIN",GUILD_HE_TYPE_JOIN);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_LEAVE",GUILD_HE_TYPE_LEAVE);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_BANISH",GUILD_HE_TYPE_BANISH);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_POSITION_CHANGE",GUILD_HE_TYPE_POSITION_CHANGE);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_BUILDING_LVLUP",GUILD_HE_TYPE_BUILDING_LVLUP);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_SKILL_LVLUP",GUILD_HE_TYPE_SKILL_LVLUP);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_DEMISE",GUILD_HE_TYPE_DEMISE);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_UNION",GUILD_HE_TYPE_UNION);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_ENEMY",GUILD_HE_TYPE_ENEMY);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_WAR",GUILD_HE_TYPE_WAR);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_BUILDING_CREATE",GUILD_HE_TYPE_BUILDING_CREATE);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_BUILDING_DESTORY",GUILD_HE_TYPE_BUILDING_DESTORY);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_BUILDING_DEL",GUILD_HE_TYPE_BUILDING_DEL);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_BUILDING_REVIVE",GUILD_HE_TYPE_BUILDING_REVIVE);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_ATTACK_SUCC",GUILD_HE_TYPE_ATTACK_SUCC);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_ATTACK_FAILED",GUILD_HE_TYPE_ATTACK_FAILED);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_DEFEND_SUCC",GUILD_HE_TYPE_DEFEND_SUCC);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_DEFEND_FAILED",GUILD_HE_TYPE_DEFEND_FAILED);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_OPEN_ACTIVITY",GUILD_HE_TYPE_OPEN_ACTIVITY);
  tolua_constant(tolua_S,"GUILD_HE_CANCEL_ALLIANCE",GUILD_HE_CANCEL_ALLIANCE);
  tolua_constant(tolua_S,"GUILD_HE_TYPE_END",GUILD_HE_TYPE_END);
  tolua_constant(tolua_S,"GUILD_CHEST_BEGIN",GUILD_CHEST_BEGIN);
  tolua_constant(tolua_S,"GUILD_CHEST_CREATE_BUILDING",GUILD_CHEST_CREATE_BUILDING);
  tolua_constant(tolua_S,"GUILD_CHEST_ACTIVITY",GUILD_CHEST_ACTIVITY);
  tolua_constant(tolua_S,"GUILD_CHEST_CONTRIBUTE",GUILD_CHEST_CONTRIBUTE);
  tolua_constant(tolua_S,"GUILD_CHEST_B_UPGRADE",GUILD_CHEST_B_UPGRADE);
  tolua_constant(tolua_S,"GUILD_CHEST_W_UPGRADE",GUILD_CHEST_W_UPGRADE);
  tolua_constant(tolua_S,"GUILD_CHEST_ALLIANCE",GUILD_CHEST_ALLIANCE);
  tolua_constant(tolua_S,"GUILD_CHEST_ARMY",GUILD_CHEST_ARMY);
  tolua_constant(tolua_S,"GUILD_CHEST_WAR_SACK_A_1",GUILD_CHEST_WAR_SACK_A_1);
  tolua_constant(tolua_S,"GUILD_CHEST_WAR_SACK_A_2",GUILD_CHEST_WAR_SACK_A_2);
  tolua_constant(tolua_S,"GUILD_CHEST_WAR_SACK_D_1",GUILD_CHEST_WAR_SACK_D_1);
  tolua_constant(tolua_S,"GUILD_CHEST_WAR_SACK_D_2",GUILD_CHEST_WAR_SACK_D_2);
  tolua_constant(tolua_S,"GUILD_CHEST_WAR_WIN",GUILD_CHEST_WAR_WIN);
  tolua_constant(tolua_S,"GUILD_CHEST_WAR_LOSE",GUILD_CHEST_WAR_LOSE);
  tolua_constant(tolua_S,"GUILD_CHEST_CANCEL_BUILDING_UPGRADE",GUILD_CHEST_CANCEL_BUILDING_UPGRADE);
  tolua_constant(tolua_S,"GUILD_CHEST_CANCEL_WELFARE_UPGRADE",GUILD_CHEST_CANCEL_WELFARE_UPGRADE);
  tolua_constant(tolua_S,"GUILD_CHEST_DEL_BUILDING",GUILD_CHEST_DEL_BUILDING);
  tolua_constant(tolua_S,"GUILD_CHEST_OPEN_DUNGEON",GUILD_CHEST_OPEN_DUNGEON);
  tolua_constant(tolua_S,"GUILD_CHEST_COMPLETE_DUNGEON",GUILD_CHEST_COMPLETE_DUNGEON);
  tolua_constant(tolua_S,"GUILD_CHEST_WAR_PLUNDER",GUILD_CHEST_WAR_PLUNDER);
  tolua_constant(tolua_S,"GUILD_CHEST_WAR_PLUNDER_EXTRA",GUILD_CHEST_WAR_PLUNDER_EXTRA);
  tolua_constant(tolua_S,"GUILD_CHEST_ROUTINE_CONSUME",GUILD_CHEST_ROUTINE_CONSUME);
  tolua_constant(tolua_S,"GUILD_CHEST_DECLARE_WAR",GUILD_CHEST_DECLARE_WAR);
  tolua_constant(tolua_S,"GUILD_CHEST_MATCH_WAR",GUILD_CHEST_MATCH_WAR);
  tolua_constant(tolua_S,"GUILD_CHEST_BUILDING_DAMAGE",GUILD_CHEST_BUILDING_DAMAGE);
  tolua_constant(tolua_S,"GUILD_CHEST_GM",GUILD_CHEST_GM);
  tolua_constant(tolua_S,"GUILD_CHEST_END",GUILD_CHEST_END);
  tolua_constant(tolua_S,"STORAGE_HE_TYPE_BEGIN",STORAGE_HE_TYPE_BEGIN);
  tolua_constant(tolua_S,"STORAGE_HE_TYPE_PUSH",STORAGE_HE_TYPE_PUSH);
  tolua_constant(tolua_S,"STORAGE_HE_TYPE_POP",STORAGE_HE_TYPE_POP);
  tolua_constant(tolua_S,"GUILD_STORAGE_TAG_1",GUILD_STORAGE_TAG_1);
  tolua_constant(tolua_S,"GUILD_STORAGE_TAG_2",GUILD_STORAGE_TAG_2);
  tolua_constant(tolua_S,"GUILD_STORAGE_TAG_3",GUILD_STORAGE_TAG_3);
  tolua_constant(tolua_S,"GUILD_STORAGE_TAG_4",GUILD_STORAGE_TAG_4);
  tolua_constant(tolua_S,"GUILD_STORAGE_TAG_5",GUILD_STORAGE_TAG_5);
  tolua_constant(tolua_S,"GUILD_STORAGE_TAG_6",GUILD_STORAGE_TAG_6);
  tolua_constant(tolua_S,"GUILD_REPLY_REFUSE",GUILD_REPLY_REFUSE);
  tolua_constant(tolua_S,"GUILD_REPLY_AGREE",GUILD_REPLY_AGREE);
  tolua_constant(tolua_S,"GUILD_B_NONE",GUILD_B_NONE);
  tolua_constant(tolua_S,"GUILD_B_MAIN",GUILD_B_MAIN);
  tolua_constant(tolua_S,"GUILD_B_BUSINESS",GUILD_B_BUSINESS);
  tolua_constant(tolua_S,"GUILD_B_DEITY",GUILD_B_DEITY);
  tolua_constant(tolua_S,"GUILD_B_HOUSE",GUILD_B_HOUSE);
  tolua_constant(tolua_S,"GUILD_B_STORAGE",GUILD_B_STORAGE);
  tolua_constant(tolua_S,"GUILD_B_STAFF",GUILD_B_STAFF);
  tolua_constant(tolua_S,"GUILD_B_WONDER",GUILD_B_WONDER);
  tolua_constant(tolua_S,"GUILD_B_HERB",GUILD_B_HERB);
  tolua_constant(tolua_S,"GUILD_B_WOOD",GUILD_B_WOOD);
  tolua_constant(tolua_S,"GUILD_B_CRYSTAL",GUILD_B_CRYSTAL);
  tolua_constant(tolua_S,"GUILD_B_SILVER",GUILD_B_SILVER);
  tolua_constant(tolua_S,"GUILD_B_HERB_STORAGE",GUILD_B_HERB_STORAGE);
  tolua_constant(tolua_S,"GUILD_B_WOOD_STORAGE",GUILD_B_WOOD_STORAGE);
  tolua_constant(tolua_S,"GUILD_B_CRYSTAL_STORAGE",GUILD_B_CRYSTAL_STORAGE);
  tolua_constant(tolua_S,"GUILD_B_SILVER_STORAGE",GUILD_B_SILVER_STORAGE);
  tolua_constant(tolua_S,"GUILD_B_FUNATIONAL_NUM",GUILD_B_FUNATIONAL_NUM);
  tolua_constant(tolua_S,"GUILD_B_GATE",GUILD_B_GATE);
  tolua_constant(tolua_S,"GUILD_B_WALL",GUILD_B_WALL);
  tolua_constant(tolua_S,"GUILD_B_ARMSHOP",GUILD_B_ARMSHOP);
  tolua_constant(tolua_S,"GUILD_B_ARROW",GUILD_B_ARROW);
  tolua_constant(tolua_S,"GUILD_B_TURRET",GUILD_B_TURRET);
  tolua_constant(tolua_S,"GUILD_B_GROUP_ATTACK",GUILD_B_GROUP_ATTACK);
  tolua_constant(tolua_S,"GUILD_B_MAGIC",GUILD_B_MAGIC);
  tolua_constant(tolua_S,"GUILD_B_CURE",GUILD_B_CURE);
  tolua_constant(tolua_S,"GUILD_B_BLOCK",GUILD_B_BLOCK);
  tolua_constant(tolua_S,"GUILD_B_NUM",GUILD_B_NUM);
  tolua_constant(tolua_S,"GUILD_B_PROTECTOR",GUILD_B_PROTECTOR);
  tolua_constant(tolua_S,"GUILD_CW_TIME_NONE",GUILD_CW_TIME_NONE);
  tolua_constant(tolua_S,"GUILD_CW_TIME_5M",GUILD_CW_TIME_5M);
  tolua_constant(tolua_S,"GUILD_CW_TIME_10M",GUILD_CW_TIME_10M);
  tolua_constant(tolua_S,"GUILD_CW_TIME_30M",GUILD_CW_TIME_30M);
  tolua_constant(tolua_S,"GUILD_CW_TIME_END",GUILD_CW_TIME_END);
  tolua_constant(tolua_S,"GUILD_STATE_NONE",GUILD_STATE_NONE);
  tolua_constant(tolua_S,"GUILD_STATE_STOP_WELFARE",GUILD_STATE_STOP_WELFARE);
  tolua_constant(tolua_S,"GUILD_ENTER_OWNER",GUILD_ENTER_OWNER);
  tolua_constant(tolua_S,"GUILD_ENTER_ALLIANCE",GUILD_ENTER_ALLIANCE);
 tolua_constant(tolua_S,"GUILD_MAX_INTRUDER_NUM",GUILD_MAX_INTRUDER_NUM);
  tolua_constant(tolua_S,"GUILD_WAR_NONE",GUILD_WAR_NONE);
  tolua_constant(tolua_S,"GUILD_WAR_INTRUDER",GUILD_WAR_INTRUDER);
  tolua_constant(tolua_S,"GUILD_WAR_DEFENDER",GUILD_WAR_DEFENDER);
  tolua_constant(tolua_S,"GUILD_WAR_STAGE_NONE",GUILD_WAR_STAGE_NONE);
  tolua_constant(tolua_S,"GUILD_WAR_STAGE_DECLARE",GUILD_WAR_STAGE_DECLARE);
  tolua_constant(tolua_S,"GUILD_WAR_STAGE_BEGIN",GUILD_WAR_STAGE_BEGIN);
  tolua_constant(tolua_S,"GUILD_WAR_STAGE_END",GUILD_WAR_STAGE_END);
  tolua_constant(tolua_S,"GUILD_WAR_DEFENDER_NONE",GUILD_WAR_DEFENDER_NONE);
  tolua_constant(tolua_S,"GUILD_WAR_DEFENDER_WIN",GUILD_WAR_DEFENDER_WIN);
  tolua_constant(tolua_S,"GUILD_WAR_DEFENDER_LOSE",GUILD_WAR_DEFENDER_LOSE);
  tolua_constant(tolua_S,"G_B_UPGRADE_BY_NPC",G_B_UPGRADE_BY_NPC);
  tolua_constant(tolua_S,"G_B_UPGRADE_BY_PANEL",G_B_UPGRADE_BY_PANEL);
  tolua_constant(tolua_S,"GUILD_BUILDING_ENABLE",GUILD_BUILDING_ENABLE);
  tolua_constant(tolua_S,"GUILD_BUILDING_DISABLE",GUILD_BUILDING_DISABLE);
  tolua_constant(tolua_S,"GUILD_CS_UPGRADING",GUILD_CS_UPGRADING);
  tolua_constant(tolua_S,"GUILD_CS_UPDATE",GUILD_CS_UPDATE);
  tolua_constant(tolua_S,"GUILD_CS_DEL",GUILD_CS_DEL);
  tolua_constant(tolua_S,"sabNotEnroll",sabNotEnroll);
  tolua_constant(tolua_S,"sabEnrolled",sabEnrolled);
  tolua_constant(tolua_S,"sabOccupied",sabOccupied);
  tolua_constant(tolua_S,"GUILD_ACHI_BEGIN",GUILD_ACHI_BEGIN);
  tolua_constant(tolua_S,"GUILD_ACHI_NONE",GUILD_ACHI_NONE);
  tolua_constant(tolua_S,"GUILD_ACHI_MEMBER",GUILD_ACHI_MEMBER);
  tolua_constant(tolua_S,"GUILD_ACHI_ENEMY",GUILD_ACHI_ENEMY);
  tolua_constant(tolua_S,"GUILD_ACHI_ALLIANCE",GUILD_ACHI_ALLIANCE);
  tolua_constant(tolua_S,"GUILD_ACHI_TAKE_POS_1",GUILD_ACHI_TAKE_POS_1);
  tolua_constant(tolua_S,"GUILD_ACHI_TAKE_POS_2",GUILD_ACHI_TAKE_POS_2);
  tolua_constant(tolua_S,"GUILD_ACHI_TYPE_NUM",GUILD_ACHI_TYPE_NUM);
  tolua_constant(tolua_S,"GA_AWARD_NONE",GA_AWARD_NONE);
  tolua_constant(tolua_S,"GA_AWARD_POINT",GA_AWARD_POINT);
  tolua_constant(tolua_S,"GA_AWARD_EXP",GA_AWARD_EXP);
  tolua_constant(tolua_S,"GA_AWARD_ITEM",GA_AWARD_ITEM);
  tolua_constant(tolua_S,"GA_AWARD_TITLE",GA_AWARD_TITLE);
  tolua_constant(tolua_S,"GA_AWARD_NUM",GA_AWARD_NUM);
  tolua_constant(tolua_S,"avGuildWarCanOpen",avGuildWarCanOpen);
 tolua_constant(tolua_S,"MAX_GM_USER",MAX_GM_USER);
 tolua_constant(tolua_S,"MAX_ITEM_BUFFER_SIZE",MAX_ITEM_BUFFER_SIZE);
 tolua_constant(tolua_S,"MAX_GMSVR_ROLE_KEY_NUM",MAX_GMSVR_ROLE_KEY_NUM);
  tolua_constant(tolua_S,"GM_CMD_NONE",GM_CMD_NONE);
  tolua_constant(tolua_S,"GM_CMD_PRE_QUERY",GM_CMD_PRE_QUERY);
  tolua_constant(tolua_S,"GM_CMD_LOCK_ROLE",GM_CMD_LOCK_ROLE);
  tolua_constant(tolua_S,"GM_CMD_QUERY_ROLE_BASE_INFO",GM_CMD_QUERY_ROLE_BASE_INFO);
  tolua_constant(tolua_S,"GM_CMD_QUERY_ROLE_DATA",GM_CMD_QUERY_ROLE_DATA);
  tolua_constant(tolua_S,"GM_CMD_FORBID_CHAT",GM_CMD_FORBID_CHAT);
  tolua_constant(tolua_S,"GM_CMD_FREEZE_ROLE",GM_CMD_FREEZE_ROLE);
  tolua_constant(tolua_S,"GM_CMD_KICK_ROLE",GM_CMD_KICK_ROLE);
  tolua_constant(tolua_S,"GM_CMD_ANNOUNCE",GM_CMD_ANNOUNCE);
  tolua_constant(tolua_S,"GM_CMD_GET_ANNOUNCE_LIST",GM_CMD_GET_ANNOUNCE_LIST);
  tolua_constant(tolua_S,"GM_CMD_DEL_ANNOUNCE",GM_CMD_DEL_ANNOUNCE);
  tolua_constant(tolua_S,"GM_CMD_SEND_MAIL",GM_CMD_SEND_MAIL);
  tolua_constant(tolua_S,"GM_CMD_BLACK_HOUSE",GM_CMD_BLACK_HOUSE);
  tolua_constant(tolua_S,"GM_CMD_LOG",GM_CMD_LOG);
  tolua_constant(tolua_S,"GM_CMD_EXE_GAME_CMD",GM_CMD_EXE_GAME_CMD);
  tolua_constant(tolua_S,"GM_CMD_LOAD_ROLE",GM_CMD_LOAD_ROLE);
  tolua_constant(tolua_S,"GM_CMD_MODIFY_ROLE",GM_CMD_MODIFY_ROLE);
  tolua_constant(tolua_S,"GM_CMD_QUERY_MAIL_DETAIL",GM_CMD_QUERY_MAIL_DETAIL);
  tolua_constant(tolua_S,"GM_CMD_QUERY_NAME_TO_ID",GM_CMD_QUERY_NAME_TO_ID);
  tolua_constant(tolua_S,"GM_CMD_ROLE_EXE_GM_CMD",GM_CMD_ROLE_EXE_GM_CMD);
  tolua_constant(tolua_S,"GM_CMD_FIX_ROLE",GM_CMD_FIX_ROLE);
  tolua_constant(tolua_S,"GM_CMD_LOAD_MAIL",GM_CMD_LOAD_MAIL);
  tolua_constant(tolua_S,"GM_CMD_DEL_MAIL",GM_CMD_DEL_MAIL);
  tolua_constant(tolua_S,"GM_CMD_ADD_MAIL_CD",GM_CMD_ADD_MAIL_CD);
  tolua_constant(tolua_S,"GM_CMD_QUERY_STATIS_INFO",GM_CMD_QUERY_STATIS_INFO);
  tolua_constant(tolua_S,"GM_CMD_QUERY_ROLE_ID",GM_CMD_QUERY_ROLE_ID);
  tolua_constant(tolua_S,"GM_CMD_EXE_GM_STR",GM_CMD_EXE_GM_STR);
  tolua_constant(tolua_S,"GM_CMD_SYS_EXCHANGE_INFO",GM_CMD_SYS_EXCHANGE_INFO);
  tolua_constant(tolua_S,"GM_CMD_NUM",GM_CMD_NUM);
  tolua_constant(tolua_S,"CMD_STEP_1",CMD_STEP_1);
  tolua_constant(tolua_S,"CMD_STEP_2",CMD_STEP_2);
  tolua_constant(tolua_S,"CMD_STEP_3",CMD_STEP_3);
  tolua_constant(tolua_S,"CMD_STEP_4",CMD_STEP_4);
  tolua_constant(tolua_S,"CMD_STEP_5",CMD_STEP_5);
  tolua_constant(tolua_S,"CMD_STEP_6",CMD_STEP_6);
  tolua_constant(tolua_S,"CMD_STEP_7",CMD_STEP_7);
  tolua_constant(tolua_S,"CMD_STEP_8",CMD_STEP_8);
  tolua_constant(tolua_S,"GM_RECORD_FORBID_CHAT",GM_RECORD_FORBID_CHAT);
  tolua_constant(tolua_S,"GM_RECORD_FREEZE_ROLE",GM_RECORD_FREEZE_ROLE);
  tolua_constant(tolua_S,"GM_RECORD_FREEZE_CHEAT_ROLE",GM_RECORD_FREEZE_CHEAT_ROLE);
  tolua_constant(tolua_S,"TL_STATE_BUSY",TL_STATE_BUSY);
  tolua_constant(tolua_S,"TL_STATE_IDLE",TL_STATE_IDLE);
  tolua_constant(tolua_S,"GM_ROLE_PRE_LOCK",GM_ROLE_PRE_LOCK);
  tolua_constant(tolua_S,"GM_ROLE_LOCK",GM_ROLE_LOCK);
  tolua_constant(tolua_S,"GM_ROLE_PRE_UNLOCK",GM_ROLE_PRE_UNLOCK);
  tolua_constant(tolua_S,"GM_ROLE_UNLOCK",GM_ROLE_UNLOCK);
  tolua_constant(tolua_S,"GM_PERMISSION_A",GM_PERMISSION_A);
  tolua_constant(tolua_S,"GM_PERMISSION_PM",GM_PERMISSION_PM);
  tolua_constant(tolua_S,"GM_PERMISSION_3",GM_PERMISSION_3);
  tolua_constant(tolua_S,"GM_PERMISSION_4",GM_PERMISSION_4);
  tolua_constant(tolua_S,"GM_PERMISSION_L",GM_PERMISSION_L);
  tolua_constant(tolua_S,"GM_PERMISSION_6",GM_PERMISSION_6);
  tolua_constant(tolua_S,"GM_PERMISSION_GM",GM_PERMISSION_GM);
  tolua_constant(tolua_S,"GM_PERMISSION_8",GM_PERMISSION_8);
  tolua_constant(tolua_S,"GM_PERMISSION_9",GM_PERMISSION_9);
  tolua_constant(tolua_S,"GM_PERMISSION_GUEST",GM_PERMISSION_GUEST);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_enum (lua_State* tolua_S) {
 return tolua_enum_open(tolua_S);
};
#endif

