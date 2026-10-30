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
        return "角色ID";
    case GM_RBD_NAME:
        return "角色名";
    case GM_RBD_GROUPID:
        return "区组";
    case GM_RBD_DELETETIME:
        return "删号时间";
    case GM_RBD_CLASS:
        return "职业";
    case GM_RBD_SEX:
        return "性别";
    case GM_RBD_LEVEL:
        return "等级";
    case GM_RBD_EXP:
        return "经验";
    case GM_RBD_PKVALUE:
        return "PK值";
    case GM_RBD_GMLEVEL:
        return "GM等级";
    case GM_RBD_MAP_ID:
        return "地图类型";
    case GM_RBD_MAP_SERIAL_ID:
        return "地图流水号";
    case GM_RBD_X:
        return "坐标X";
    case GM_RBD_Y:
        return "坐标Y";
    case GM_RBD_Z:
        return "坐标Z";
    case GM_RBD_LAST_LOGIN_TIME:
        return "上次登录时间";
    case GM_RBD_LAST_LOGOUT_TIME:
        return "上次数据保存时间";
    case GM_RBD_FORBIDDEND_CHAT_TIME:
        return "禁言时间";
    case GM_RBD_FREEZE_ROLE_TIME:
        return "GM人工禁止登陆时间";
    case GM_RBD_FREEZE_CHEAT_ROLE:
        return "系统禁止登陆时间";
    case GM_RBD_GUILD_ID:
        return "公会ID";
    case GM_RBD_MONEY_1:
        return "绑金";
    case GM_RBD_MONEY_2:
        return "金币";
    case GM_RBD_MONEY_3:
        return "未使用";
    case GM_RBD_MONEY_4:
        return "未使用";
    case GM_RBD_MONEY_5:
        return "疲劳度";
    case GM_RBD_MONEY_6:
        return "魂";
    case GM_RBD_MONEY_7:
        return "名师值";
    case GM_RBD_MONEY_8:
        return "万法之气";
    case GM_RBD_MONEY_9:
        return "战场点数";
    case GM_RBD_MONEY_10:
        return "家族贡献";
    case GM_RBD_MONEY_11:
        return "修炼点";
    case GM_RBD_MONEY_12:
        return "道义值";
    case GM_RBD_MONEY_13:
        return "竞技场点数";
    case GM_RBD_MONEY_14:
        return "成就点数";
    default:
        break;
    }

    return "未知";
}

const char *GetSexString(INT32 nSexType)
{
    switch (nSexType)
    {
    case cstMale:
        return "男";
    case cstFemale:
        return "女";
    case cstAsexual:
        return "妖";
        break;
    default:
        return "空";
    }

    return "空";
}

const char *GetClassString(INT32 nClassType)
{
    switch (nClassType)
    {
    case clsWarrior:
        return "战士";
    case clsMage:
        return "法师";
    case clsRogue:
        return "刺客";
    case clsArcher:
        return "弓手";
    case clsTaoist:
        return "道士";
    default:
        return "空";
    }

    return "空";
}

const char *GetItemContainerString(INT32 nConSlot)
{
    switch (nConSlot)
    {
    case CONSLOT_EQUIPPED:
        return "已装备";
    case CONSLOT_DEFAULT:
        return "背包";
    case CONSLOT_BANK:
        return "银行";
    case CONSLOT_HERO_EQUIPPED:
        return "英雄装备";
    case CONSLOT_HERO_EQUIPMENT:
        return "英雄包裹";
    case CONSLOT_DEPOSIT:
        return "储值栏";
    default:
        return "空";
    }

    return "空";
}

const char *GetItemTypeString(INT32 nType)
{
    switch (nType)
    {
    case ITEMTYPE_INVALID:
        return "空";
    case ITEMTYPE_USABLE:
        return "消耗品";
    case ITEMTYPE_EQUIP:
        return "装备";
    case ITEMTYPE_MATERIAL:
        return "材料";
    case ITEMTYPE_FORMULA:
        return "配方";
    case ITEMTYPE_SPELLBOOK:
        return "技能书";
    case ITEMTYPE_RUNE:
        return "符文";
    case ITEMTYPE_DAN:
        return "丹药";
    case ITEMTYPE_SOUL:
        return "灵魂";
    case ITEMTYPE_MISC:
        return "其他";
    case ITEMTYPE_QUEST:
        return "任务";
    case ITEMTYPE_DEPOSIT:
        return "储值";
    case ITEMTYPE_RMB:
        return "商城";
    default:
        return "空";
    }

    return "空";
}

const char *GetMailAttachType(INT32 nAttachType)
{
    switch (nAttachType)
    {
    case ATTACHTYPE_NONE:
        return "无附件";
    case ATTACHTYPE_SIMPLE:
    case ATTACHTYPE_BUFFER:
        return "有附件";
    default:
        return "空";
    }

    return "空";
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
        return "成功";
    case errGMUnknownError:
        return "未知错误";
    case errGMVersionError:
        return "GM工具版本错误，请更新版本";
    case errGMConnectERROR:
        return "连接错误";
    case errGMPwdError:
        return "账号or密码错";
    case errGMRelogin:
        return "账号重复登陆";
    case errGMCannotExecuteNewCmd:
        return "内存分配失败";
    case errGMNotExecutePermission:
        return "没有执行该命令的权限";
    case errGMGetPermissionError:
        return "获取权限失败";
    case errGMUserNotFind:
        return "从ms返回后，未找到GM账号";
    case errGMDBError:
        return "数据库错误";
    case errGMTimeout:
        return "指令执行超时";
    case errGMSendMsgToMSError:
        return "向ms发送消息失败";
    case errGMLostTLUser:
        return "丢失gm账号";
    case errGMCannotFindRole:
        return "找不到游戏角色";
    case errGMLostCmd:
        return "从ms返回后丢失指令";
    case errGMMSNotOnline:
        return "ms不在线";
    case errGMMSExecuteCmdFailed:
        return "ms执行gm指令失败";
    case errGMSaveRoleDataFailed:
        return "保存游戏角色数据失败";
    case errGMModifyRoleTypeError:
        return "更改角色的数据的命令类型错误";
    case errGMRoleOnline:
        return "游戏角色在线";
    case errGMQueryZeroRoleData:
        return "未查询到游戏角色数据";
    case errGMRoleNotFreeze:
        return "角色未被冻结";
    case errGMRoleFreezeLessOneHour:
        return "角色冻结时间小于1小时";
    case errGMAnnounceNotExist:
        return "公告不存在了";
    case errGMRoleBeOp:
        return "该角色被操作";
    case errGMRoleLockedByOther:
        return "其他账号正在操作";
    default:
        return "未统计的错误码，请联系程序";
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
