// this cpp source file is generated automatically by tools, please do not manually modify.
#include "stdafx.h"
#include "errdef.h"

// @brief
// You may see this phenomenon :
// variables with "STR_" as their prifix, maybe have a suffix
// the suffix means which format output symbols have appeared in function like printf
// Sample :  "%s" turns to be "_S"
namespace  ERR_ENTER_GS
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");

    const INT32 ID_ENTERGAMEFAILED = 1;
    const TCHAR* STR_ENTERGAMEFAILED = _SDTSource2T("GBK", "Failed to enter game");

    const INT32 ID_EXISTEDINGAMESERVER = 2;
    const TCHAR* STR_EXISTEDINGAMESERVER = _SDTSource2T("GBK", "Already in game");

    const INT32 ID_TASKLOADSTATUSFAILD = 3;
    const TCHAR* STR_TASKLOADSTATUSFAILD = _SDTSource2T("GBK", "Failed to load quest info");
}

namespace  ERR_MOVE
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");

    const INT32 ID_TIMEOUT = 1;
    const TCHAR* STR_TIMEOUT = _SDTSource2T("GBK", "Timeout");

    const INT32 ID_MOREPKG = 2;
    const TCHAR* STR_MOREPKG = _SDTSource2T("GBK", "Too many packets sent");

    const INT32 ID_DIRERR = 3;
    const TCHAR* STR_DIRERR = _SDTSource2T("GBK", "Direction value out of range");

    const INT32 ID_MOVEERR = 4;
    const TCHAR* STR_MOVEERR = _SDTSource2T("GBK", "Move error");

    const INT32 ID_GETOBJERR = 5;
    const TCHAR* STR_GETOBJERR = _SDTSource2T("GBK", "OBJID error");

    const INT32 ID_MOVETRYERR = 6;
    const TCHAR* STR_MOVETRYERR = _SDTSource2T("GBK", "Move type error");

    const INT32 ID_NOTACTIVE = 7;
    const TCHAR* STR_NOTACTIVE = _SDTSource2T("GBK", "Character cannot act right now");
}

namespace  ERR_CHANGE_DIR
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");

    const INT32 ID_OUTOFMAX = 1;
    const TCHAR* STR_OUTOFMAX = _SDTSource2T("GBK", "Direction exceeds maximum range");
}

namespace  ERR_DRESS
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");

    const INT32 ID_UNVALIDITEM = 1;
    const TCHAR* STR_UNVALIDITEM = _SDTSource2T("GBK", "Requested equipment is invalid");

    const INT32 ID_NOTMATCHTYPE = 2;
    const TCHAR* STR_NOTMATCHTYPE = _SDTSource2T("GBK", "Requested equipment type does not match");

    const INT32 ID_SRCPOSNOITEM = 3;
    const TCHAR* STR_SRCPOSNOITEM = _SDTSource2T("GBK", "No equipment at source position");

    const INT32 ID_TARGETPOSHAVEITEM = 4;
    const TCHAR* STR_TARGETPOSHAVEITEM = _SDTSource2T("GBK", "Equipment at target position, cannot equip");

    const INT32 ID_POSNOTCHANGE = 5;
    const TCHAR* STR_POSNOTCHANGE = _SDTSource2T("GBK", "Position not changed");

    const INT32 ID_NOTMATCHOBJID = 6;
    const TCHAR* STR_NOTMATCHOBJID = _SDTSource2T("GBK", "OBJID does not match");

    const INT32 ID_SPACENOTENOUGH = 7;
    const TCHAR* STR_SPACENOTENOUGH = _SDTSource2T("GBK", "Not enough bag space");

    const INT32 ID_SERVERCHECKINVALID = 8;
    const TCHAR* STR_SERVERCHECKINVALID = _SDTSource2T("GBK", "Server bag check invalid");
}

namespace  ERR_SKILL_ATTACK
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");

    const INT32 ID_USE_SKILL_FAILED = 1;
    const TCHAR* STR_USE_SKILL_FAILED = _SDTSource2T("GBK", "Ability use error");
}

namespace  ERR_NPC_TALK
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");
}

namespace  ERR_SYNC_TIME
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");
}

namespace  ERR_TASK_OPERATE
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Quest operation successful");

    const INT32 ID_OPERATE_FAILED = 1;
    const TCHAR* STR_OPERATE_FAILED = _SDTSource2T("GBK", "Quest operation error");

    const INT32 ID_INSTANCE_TASK_FAILED = 2;
    const TCHAR* STR_INSTANCE_TASK_FAILED = _SDTSource2T("GBK", "Dungeon quest error");
}

namespace  ERR_ITEM_SMITHING
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");

    const INT32 ID_NOCHANGE = 1;
    const TCHAR* STR_NOCHANGE = _SDTSource2T("GBK", "Failed, attributes unchanged");

    const INT32 ID_OVERFLOW = 2;
    const TCHAR* STR_OVERFLOW = _SDTSource2T("GBK", "Failed, maximum forging count reached");

    const INT32 ID_DESTROY = 3;
    const TCHAR* STR_DESTROY = _SDTSource2T("GBK", "Failed, and the item was destroyed");
}

namespace  ERR_ITEM_USE
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");

    const INT32 ID_DESTROY = 1;
    const TCHAR* STR_DESTROY = _SDTSource2T("GBK", "Operation successful, and the item was destroyed");

    const INT32 ID_INVALIDPARA = 2;
    const TCHAR* STR_INVALIDPARA = _SDTSource2T("GBK", "Failed, invalid parameter");

    const INT32 ID_UNUSED = 3;
    const TCHAR* STR_UNUSED = _SDTSource2T("GBK", "Failed, item cannot be used");
}

namespace  ERR_TRAP
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");

    const INT32 ID_INTERNEL_ERROR = 1;
    const TCHAR* STR_INTERNEL_ERROR = _SDTSource2T("GBK", "Internal error");

    const INT32 ID_BAD_TRAP_ID = 2;
    const TCHAR* STR_BAD_TRAP_ID = _SDTSource2T("GBK", "Trap ID error");

    const INT32 ID_NOT_ONTIME = 3;
    const TCHAR* STR_NOT_ONTIME = _SDTSource2T("GBK", "Trigger time error");

    const INT32 ID_PLAYER_CAN_NOT_OPEN = 4;
    const TCHAR* STR_PLAYER_CAN_NOT_OPEN = _SDTSource2T("GBK", "Player cannot open this trap");

    const INT32 ID_BAD_TRAP_STATUS = 4;
    const TCHAR* STR_BAD_TRAP_STATUS = _SDTSource2T("GBK", "Trap cannot be opened right now");
}

