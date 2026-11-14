#include "stdafx.h"
#include "logic.h"
#include "gm_input.h"
#include <QtCore/QTextCodec>
#include <array>
#include "char_def.h"
#include "item_def.h"
#include "gm_def.h"
#include "my_tree_widget_item.h"
#include "../../src/manageserver/mail/mail.h"

const char *GetBaseDataName(INT32 nBaseType)
{
    switch (nBaseType)
    {
    case GM_RBD_ID:
        return "Role ID";
    case GM_RBD_NAME:
        return "Role Name";
    case GM_RBD_GROUPID:
        return "Group";
    case GM_RBD_DELETETIME:
        return "Delete Time";
    case GM_RBD_CLASS:
        return "Class";
    case GM_RBD_SEX:
        return "Gender";
    case GM_RBD_LEVEL:
        return "Level";
    case GM_RBD_EXP:
        return "EXP";
    case GM_RBD_PKVALUE:
        return "PK Value";
    case GM_RBD_GMLEVEL:
        return "GM Level";
    case GM_RBD_MAP_ID:
        return "Map Type";
    case GM_RBD_MAP_SERIAL_ID:
        return "Map Serial No.";
    case GM_RBD_X:
        return "Pos X";
    case GM_RBD_Y:
        return "Pos Y";
    case GM_RBD_Z:
        return "Pos Z";
    case GM_RBD_LAST_LOGIN_TIME:
        return "Last Login Time";
    case GM_RBD_LAST_LOGOUT_TIME:
        return "Last Save Time";
    case GM_RBD_FORBIDDEND_CHAT_TIME:
        return "Mute Time";
    case GM_RBD_FREEZE_ROLE_TIME:
        return "GM Login Ban Time";
    case GM_RBD_FREEZE_CHEAT_ROLE:
        return "System Login Ban Time";
    case GM_RBD_GUILD_ID:
        return "Guild ID";
    case GM_RBD_MONEY_1:
        return "Bound Gold";
    case GM_RBD_MONEY_2:
        return "Gold";
    case GM_RBD_MONEY_3:
        return "Unused";
    case GM_RBD_MONEY_4:
        return "Unused";
    case GM_RBD_MONEY_5:
        return "Fatigue";
    case GM_RBD_MONEY_6:
        return "Soul";
    case GM_RBD_MONEY_7:
        return "Mentor Points";
    case GM_RBD_MONEY_8:
        return "Myriad Qi";
    case GM_RBD_MONEY_9:
        return "Battlefield Points";
    case GM_RBD_MONEY_10:
        return "Family Contribution";
    case GM_RBD_MONEY_11:
        return "Training Points";
    case GM_RBD_MONEY_12:
        return "Virtue";
    case GM_RBD_MONEY_13:
        return "Arena Points";
    case GM_RBD_MONEY_14:
        return "Achievement Points";
    default:
        break;
    }

    return "Unknown";
}

const char *GetSexString(INT32 nSexType)
{
    switch (nSexType)
    {
    case cstMale:
        return "Male";
    case cstFemale:
        return "Female";
    case cstAsexual:
        return "Demon";
        break;
    default:
        return "None";
    }

    return "None";
}

const char *GetClassString(INT32 nClassType)
{
    switch (nClassType)
    {
    case clsWarrior:
        return "Warrior";
    case clsMage:
        return "Mage";
    case clsRogue:
        return "Assassin";
    case clsArcher:
        return "Archer";
    case clsTaoist:
        return "Taoist";
    case clsSpear:
        return "Lancer";
    case clsSorcerer:
        return "Demon";
    default:
        return "None";
    }

    return "None";
}

const char *GetItemContainerString(INT32 nConSlot)
{
    switch (nConSlot)
    {
    case CONSLOT_EQUIPPED:
        return "Equipped";
    case CONSLOT_DEFAULT:
        return "Bag";
    case CONSLOT_BANK:
        return "Bank";
    case CONSLOT_HERO_EQUIPPED:
        return "Hero Equipment";
    case CONSLOT_HERO_EQUIPMENT:
        return "Hero Bag";
    case CONSLOT_DEPOSIT:
        return "Top-up Slot";
    default:
        return "None";
    }

    return "None";
}

const char *GetItemTypeString(INT32 nType)
{
    switch (nType)
    {
    case ITEMTYPE_INVALID:
        return "None";
    case ITEMTYPE_USABLE:
        return "Consumable";
    case ITEMTYPE_EQUIP:
        return "Equipment";
    case ITEMTYPE_MATERIAL:
        return "Material";
    case ITEMTYPE_FORMULA:
        return "Recipe";
    case ITEMTYPE_SPELLBOOK:
        return "Skill Book";
    case ITEMTYPE_RUNE:
        return "Rune";
    case ITEMTYPE_DAN:
        return "Elixir";
    case ITEMTYPE_SOUL:
        return "Spirit";
    case ITEMTYPE_MISC:
        return "Other";
    case ITEMTYPE_QUEST:
        return "Quest";
    case ITEMTYPE_DEPOSIT:
        return "Top-up";
    case ITEMTYPE_RMB:
        return "Mall";
    default:
        return "None";
    }

    return "None";
}

const char *GetMailAttachType(INT32 nAttachType)
{
    switch (nAttachType)
    {
    case ATTACHTYPE_NONE:
        return "No Attachment";
    case ATTACHTYPE_SIMPLE:
    case ATTACHTYPE_BUFFER:
        return "Has Attachment";
    default:
        return "None";
    }

    return "None";
}

const char * gm_itoa(INT32 i, char *pBuffer)
{
    return itoa(i, pBuffer, 10);
}

const char * gm_itoa64(INT64 i, char *pBuffer)
{
    return _i64toa(i, pBuffer, 10);
}


BOOL DisplayBaseInfoWidgetItem(CMyTreeWidgetItem* item, INT32 area, INT32 group, UINT32 role,
    BOOL online, const char* saved, UINT32 deleted, const char* name, INT32 sex, INT32 role_class, INT32 level)
{
    if (!item || !name || !saved) return FALSE;
    const auto* sex_name = GetSexString(sex);
    const auto* class_name = GetClassString(role_class);
    if (!sex_name || !class_name) return FALSE;
    const auto legacy = [](const char* text)
    {
        if (const auto* codec = QTextCodec::codecForName("GB18030")) return codec->toUnicode(text);
        return QString::fromLocal8Bit(text);
    };
    const std::array<QString,10> columns{QString::number(area), QString::number(group), QString::number(role),
    QString::fromUtf8(name), deleted ? QStringLiteral("Yes") : QStringLiteral("No"),
    online ? QStringLiteral("Online") : QStringLiteral("Offline"), QString::fromUtf8(saved),
        legacy(sex_name), legacy(class_name), QString::number(level)};
    item->SetID(static_cast<INT32>(role));
    item->setData(2, Qt::UserRole, role);
    for (int column = 0; column < static_cast<int>(columns.size()); ++column)
    {
        item->setText(column, columns[column]);
        item->setToolTip(column, columns[column]);
    }
    return TRUE;
}


const char *ParseResult(INT32 nResult)
{
    switch (nResult)
    {
    case errSuccess:
        return "Success";
    case errGMUnknownError:
        return "Unknown error";
    case errGMVersionError:
        return "GM tool version mismatch, please update";
    case errGMConnectERROR:
        return "Connection error";
    case errGMPwdError:
        return "Wrong account or password";
    case errGMRelogin:
        return "Account already logged in";
    case errGMCannotExecuteNewCmd:
        return "Memory allocation failed";
    case errGMNotExecutePermission:
        return "No permission to run this command";
    case errGMGetPermissionError:
        return "Failed to get permissions";
    case errGMUserNotFind:
        return "GM account not found after ms reply";
    case errGMDBError:
        return "Database error";
    case errGMTimeout:
        return "Command timed out";
    case errGMSendMsgToMSError:
        return "Failed to send message to ms";
    case errGMLostTLUser:
        return "GM account lost";
    case errGMCannotFindRole:
        return "Game role not found";
    case errGMLostCmd:
        return "Command lost after ms reply";
    case errGMMSNotOnline:
        return "ms is offline";
    case errGMMSExecuteCmdFailed:
        return "ms failed to run GM command";
    case errGMSaveRoleDataFailed:
        return "Failed to save role data";
    case errGMModifyRoleTypeError:
        return "Invalid command type for changing role data";
    case errGMRoleOnline:
        return "Game role is online";
    case errGMQueryZeroRoleData:
        return "No role data found";
    case errGMRoleNotFreeze:
        return "Role is not frozen";
    case errGMRoleFreezeLessOneHour:
        return "Role frozen for less than 1 hour";
    case errGMAnnounceNotExist:
        return "Announcement no longer exists";
    case errGMRoleBeOp:
        return "Role is being operated on";
    case errGMRoleLockedByOther:
        return "Another account is operating on it";
    default:
        return "Unlisted error code, contact the developers";
    }
}


const INT32 UTF8_2CHAR = (1 << 7) + (1 << 6);
const INT32 UTF8_3CHAR = UTF8_2CHAR + (1 << 5);
const INT32 UTF8_4CHAR = UTF8_3CHAR + (1 << 4);
const INT32 UTF8_5CHAR = UTF8_4CHAR + (1 << 3);
const INT32 UTF8_6CHAR = UTF8_5CHAR + (1 << 2);

BOOL FilterUtf8EnterKey(char* text)
{
    if (!text) return FALSE;
    gm::normalize_line_breaks(std::span<char>(text,std::string_view(text).size()));
    return TRUE;
}
