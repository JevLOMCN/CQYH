/********************************************************************
	created:	4:12:2013   15:29
	file:		cmd_manager.h
	author:		Allan Tao
	
	purpose:	Command manager: interaction between UI and logic layers
*********************************************************************/

#ifndef _H_CMD_MANAGER_H_
#define _H_CMD_MANAGER_H_


#ifdef _WIN32
#include <winsock2.h>
#endif
#include "std_type.h"
#include <list>
#include <functional>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
//#include "object_pool.h"
#include "predefine.h"
#include "cmd_data.h"





class CCmdManager
{
public:
    static CCmdManager &GetInstance();

protected:
    static CCmdManager     m_Instance;

    CCmdManager();
    ~CCmdManager();

public:

    BOOL    Init();
    BOOL    UnInit();
    BOOL    MainLoop();

    BOOL        Success(CommandPtr pCmd);
    BOOL        Washout(CommandPtr pCmd);

    CommandPtr  NewCmd(INT32 nType);
    BOOL        DelCmd(CommandPtr pCmd);

    BOOL        AddSvr(UINT32 dwSvrID);
    void        ClearSvrList()           { m_PreExecuteSvrList.clear(); m_Cursor = 0; }
    UINT32      GetSvrNum()                 { return m_PreExecuteSvrList.size(); }
    UINT32      First();
    UINT32      Next();

    BOOL        IsCompleteAll();

    INT32       GetResult()     { return m_nResult; }
    void        SetResult(INT32 nResult)     { m_nResult = nResult; }

public:
    BOOL        PreQuery(INT32 nCmdType, const char *pcszRoleName, CommandPtr pCmd);
    std::expected<void, std::string> ReceivePreQuery(UINT32 session, UINT32 role, std::string_view name);
    std::expected<void, std::string> ReceiveBulkReply(INT32 kind, UINT32 session, UINT32 role, INT32 result);
    BOOL        QueryRoleID(INT32 nType, const char *pcszName);
    BOOL        QueryRoleData(CommandPtr pCmd);
    BOOL        ForbidChat(CommandPtr pCmd);
    BOOL        FreezeRole(CommandPtr pCmd);
    BOOL        KickRole(CommandPtr pCmd);
    BOOL        SendMail(CommandPtr pCmd);
    BOOL        SendAnnounce(INT32 nGroupID, UINT32 dwBeginTime, INT32 nIntervalTime, INT32 nRepeatNum, const char *pcszAnnounce);
    BOOL        QueryAnnounceList(INT32 nGroupID);
    BOOL        DelAnnounce(INT32 nGroupID, UINT32 nAnnounceSerial);
    BOOL        QueryNameToID(std::map<INT32, ROLE_NAME_DATA> &map);
    std::expected<void, std::string> ReceiveNameToID(UINT32 session, INT32 index, UINT32 role,
        const std::filesystem::path& report);
    BOOL        SendGMStrCmd(const char *pcszCmdStr);

    BOOL        LoadRoleData(CommandPtr pCmd);
    BOOL        ModifyRole(UINT32 dwRoleID, UINT32 dwTargetRoleID, INT32 nLength, const char *pcszData, const char *pcszReason);

    BOOL        ExeRoleGMCmd(CommandPtr pCmd);


    // log
    BOOL        QueryLog(INT32 nLogType, UINT32 dwBeginTime, UINT32 dwEndTime, const char *pcszQueryStr);


    // cmd 
    CommandPtr GetCurrentCmd() const { return m_pCmd; }
    BOOL        SetExeServerNum(INT32 nSvrNum);
    BOOL        IsCmdExecuting(BOOL isModifyRoleCmd = FALSE);
    void        CmdEnd();
    using Clock = std::chrono::steady_clock;
    void SetClock(std::function<Clock::time_point()> clock);


private:
    INT32           m_nState;
    INT32           m_nResult;
    BOOL            m_IsCmdComplete;

    CommandPtr m_pCmd;
    struct Continuation { CommandPtr owner; std::function<BOOL()> run; };
    std::optional<Continuation> m_continuation;
    bool m_initialized = false;
    std::function<Clock::time_point()> m_now = Clock::now;
    Clock::time_point DeadlineAfter(std::chrono::seconds timeout) const;
    BOOL Submit(CommandPtr command, std::chrono::seconds timeout, std::function<BOOL()> send);

    typedef std::vector<UINT32> SVR_LIST;
    std::size_t m_Cursor = 0;
    SVR_LIST    m_PreExecuteSvrList;
};




#endif //_H_CMD_MANAGER_H_



