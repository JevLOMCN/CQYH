/********************************************************************
	created:	13:1:2014   14:48
	file:		cmd_data.h
	author:		Allan Tao
	
	purpose:	
*********************************************************************/

#ifndef _H_CMD_DATA_H_
#define _H_CMD_DATA_H_

#include "std_type.h"
#include <array>
#include <chrono>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include <filesystem>
#include "gm_def.h"
#include "predefine.h"
#include "string_def.h"
#include "chat_def.h"
#include "tl_config.h"
class ISocketSession;


enum CMD_TYPE_ENUM
{
    CMD_PRE_QUERY       = 1,

    CMD_QUERY_ROLE_ID   = 2,
    CMD_QUERY_ROLE_DATA = 3,

    CMD_FORBIDDEN_CHAT  = 4,
    CMD_FREEZE_ROLE     = 5,
    CMD_KICK_ROLE       = 6,
    CMD_ANNOUNCE        = 7,
    CMD_GET_ANNOUNCE_LIST = 8,
    CMD_DEL_ANNOUNCE    = 9,
    CMD_SEND_MAIL       = 10,
    CMD_LOG             = 11,
    CMD_EXECUTE_GM_STR  = 12,

    CMD_LOAD_ROLE_DATA = 13,
    CMD_MODIFY_ROLE_DATA = 14,

    CMD_QUERY_NAME_TO_ID = 15,

    CMD_EXE_ROLE_GM_CMD = 16,
    

};

struct CMD_BASE 
{
    struct ReplyPeer
    {
        ISocketSession* socket{};
        UINT64 generation{};
        bool issued=false, busy=false, done=false;
        std::shared_ptr<const char> connection_lifetime;
    };
    std::map<UINT32, ReplyPeer> replies;
    std::size_t reply_deliveries=0;
    INT32 nType{};
    INT32 nSvrNum{};
    std::array<INT32, MAX_SESSION_COUNT> dwSessionID{};
    std::chrono::steady_clock::time_point deadline{};
    std::size_t in_flight = 0;
    UINT32 reply_peer = UINT32(-1);
    std::shared_ptr<const char> reply_connection_lifetime;
    INT32 Kind() const { return allocation_kind_; }
private:
    friend class CCmdManager;
    INT32 allocation_kind_{};
};

using CommandPtr = std::shared_ptr<CMD_BASE>;



struct PRE_QUERY : CMD_BASE
{
    char        szRoleName[ROLE_NAME_LEN];
    INT32       nCmdType;
    CommandPtr pCmd;
};

struct QUERY_ROLE_ID : CMD_BASE 
{
    char        szName[USER_NAME_LEN];
    INT32       nQueryType;
};

struct QUERY_ROLE_DATA : CMD_BASE
{
    //INT32   nRoleNum;
    UINT32  dwRoleID;
    char    szRoleName[ROLE_NAME_LEN];
};

struct ROLE_RESULT 
{
    char    szRoleName[ROLE_NAME_LEN];
    UINT32  dwRoleID;
    INT32   nResult;
};

struct FORBIDDEN_CHAT : CMD_BASE 
{
    INT64   llTime;
    std::filesystem::path report_path;
    char    szRoleName[ROLE_NAME_LEN];
    std::list<ROLE_RESULT>  roleList;
    std::list<ROLE_RESULT>  roleResList;
};

struct FREEZE_ROLE : CMD_BASE 
{
    INT64   llTime;
    std::filesystem::path report_path;
    char    szRoleName[ROLE_NAME_LEN];
    std::list<ROLE_RESULT>  roleList;
    std::list<ROLE_RESULT>  roleResList;
};

struct KICK_ROLE : CMD_BASE 
{
    std::filesystem::path report_path;
    char    szRoleName[ROLE_NAME_LEN];
    std::list<ROLE_RESULT>  roleList;
    std::list<ROLE_RESULT>  roleResList;
};

struct SEND_MAIL : CMD_BASE
{
    std::filesystem::path report_path;
    char    szRoleName[ROLE_NAME_LEN];
    std::list<ROLE_RESULT>  roleList;
    std::list<ROLE_RESULT>  roleResList;

    char    szTitle[MAIL_SUBJECT_LEN];
    char    szContent[MAIL_CONTENT_LEN];
    char    szReason[GM_REASON_LEN];
    UINT32  dwItemID;
    INT32   nItemNum;
};

struct ANNOUNCE : CMD_BASE 
{
    INT32       nGroupID;
    UINT32      dwBeginTime;
    INT32       nIntervalTime;
    INT32       nRepeatNum;
    char        szAnnounce[GM_ANNOUNCE_LENGTH];
};

struct GET_ANNOUNCE_LIST : CMD_BASE 
{
    INT32 nGroupID;
};

struct DEL_ANNOUNCE : CMD_BASE 
{
    INT32       nGroupID;
    UINT32      dwAnnounceSerial;
};

struct LOG_DATA : CMD_BASE 
{
    ISocketSession* reply_socket=nullptr;
    bool reply_busy=false;
    bool reply_done=false;
    INT32       nLogType;
    UINT32      dwBeginTime;
    UINT32      dwEndTime;
    char        szQueryStr[MAX_GM_COMMAND_LEN];
};


struct EXECUTE_GM_STR : CMD_BASE 
{
    char        szStr[MAX_GM_COMMAND_LEN];
};

struct LOAD_ROLE_DATA : CMD_BASE 
{
    UINT32      dwRoleID;
    char        szRoleName[ROLE_NAME_LEN];
};

struct MODIFY_ROLE_DATA : CMD_BASE
{
    UINT32      dwRoleID;
    char        szRoleName[ROLE_NAME_LEN];
    UINT32      dwTargetRoleID;
    char        szReason[GM_REASON_LEN];
    std::vector<char> data;
};

struct ROLE_NAME_DATA 
{
    UINT32      dwRoleID;
    char        szRoleName[ROLE_NAME_LEN];
};

struct QUERY_NAME_TO_ID : CMD_BASE 
{
    std::map<INT32, ROLE_NAME_DATA> RoleNameMap;
    INT32 current_index{};
    UINT32 session_id = UINT32(-1);
    bool pending_next = false;
};

struct EXE_ROLE_CMD : CMD_BASE 
{
    UINT32      dwRoleID;
    char        szRoleName[ROLE_NAME_LEN];
    char        szGM[MAX_GM_COMMAND_LEN];
    char        szReason[GM_REASON_LEN];
};

#endif //_H_CMD_DATA_H_
