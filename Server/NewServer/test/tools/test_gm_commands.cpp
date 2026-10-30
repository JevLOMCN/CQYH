#include "cmd_manager.h"
#include "message_handler.h"
#include "gm_log.h"
#include "gm_role_data.h"
#include <iostream>
#include <stdexcept>

class CRoleInfo {};
CRoleInfo* g_pRoleInfo = nullptr;
namespace
{
std::size_t checks, calls, log_calls;
std::function<BOOL(INT32)> on_send;
std::function<void()> on_log;
std::vector<char> sent_data;
std::string sent_reason;
UINT32 sent_target;
void check(bool condition, const char* message)
{
    ++checks;
    if (!condition) throw std::runtime_error(message);
}
BOOL send(INT32 type)
{
    ++calls;
    const auto active = CCmdManager::GetInstance().GetCurrentCmd();
    check(active && active->Kind() == type && active->nType == type && active->in_flight > 0, "fully owned command visible before send callback");
    active->nSvrNum = 1;
    const auto callback = on_send;
    return callback ? callback(type) : TRUE;
}
void reset()
{
    auto& manager=CCmdManager::GetInstance();manager.Init();on_send={};on_log={};calls=0;log_calls=0;
}
}
CMessageHandler CMessageHandler::ms_Instance;
CMessageHandler::CMessageHandler() : m_pNet(nullptr), m_pPacketAllocator(nullptr) {}
CMessageHandler::~CMessageHandler() = default;
CMessageHandler& CMessageHandler::GetInstance() { return ms_Instance; }
void CMessageHandler::OnAccepted(ISocketAcceptor*, ISocketSession*) {}
void CMessageHandler::OnConnected(ISocketSession*) {}
void CMessageHandler::OnRecv(ISocketSession*, PACKET_BUFFER*, UINT32) {}
void CMessageHandler::OnDisconnected(ISocketSession*, BOOL) {}
BOOL CMessageHandler::DoPreQuery(const char*) { return send(CMD_PRE_QUERY); }
BOOL CMessageHandler::DoQueryRoleID(INT32, const char*) { return send(CMD_QUERY_ROLE_ID); }
BOOL CMessageHandler::DoQueryRoleData(UINT32) { return send(CMD_QUERY_ROLE_DATA); }
BOOL CMessageHandler::DoForbidChat(UINT32, UINT32) { return send(CMD_FORBIDDEN_CHAT); }
BOOL CMessageHandler::DoFreezeRole(UINT32, UINT32) { return send(CMD_FREEZE_ROLE); }
BOOL CMessageHandler::DoKickRole(UINT32) { return send(CMD_KICK_ROLE); }
BOOL CMessageHandler::DoSendMail(UINT32, const char*, const char*, const char*, UINT32, INT32) { return send(CMD_SEND_MAIL); }
BOOL CMessageHandler::DoAnnounce(INT32, UINT32, INT32, INT32, const char*) { return send(CMD_ANNOUNCE); }
BOOL CMessageHandler::DoGetAnnounceList(INT32) { return send(CMD_GET_ANNOUNCE_LIST); }
BOOL CMessageHandler::DoDelAnnounce(INT32, UINT32) { return send(CMD_DEL_ANNOUNCE); }
BOOL CMessageHandler::DoQueryNameToID(INT32, const char*) { return send(CMD_QUERY_NAME_TO_ID); }
BOOL CMessageHandler::DoExecuteGMStrCmd(const char*) { return send(CMD_EXECUTE_GM_STR); }
BOOL CMessageHandler::DoLoadRoleData(UINT32) { return send(CMD_LOAD_ROLE_DATA); }
BOOL CMessageHandler::DoModifyRoleData(UINT32, UINT32 target, INT32 length, const char* bytes, const char* reason)
{
    sent_data.assign(bytes,bytes+length);sent_reason=reason;sent_target=target;
    return send(CMD_MODIFY_ROLE_DATA);
}
BOOL CMessageHandler::DoExeRoleGMCmd(UINT32, const char*, const char*) { return send(CMD_EXE_ROLE_GM_CMD); }
BOOL CMessageHandler::DoLog(INT32, UINT32, UINT32, const char*) { return send(CMD_LOG); }
CGMLog CGMLog::m_Instance;
CGMLog::CGMLog() = default;
CGMLog::~CGMLog() = default;
CGMLog& CGMLog::GetInstance() { return m_Instance; }
BOOL CGMLog::Log(std::string_view) { ++log_calls;if (on_log) on_log();return TRUE; }

int main()
try
{
    auto& manager=CCmdManager::GetInstance();reset();
    for (INT32 type=CMD_PRE_QUERY;type<=CMD_EXE_ROLE_GM_CMD;++type)
    {
        auto command=manager.NewCmd(type);
        check(command && command->Kind()==type && command->nType==type && command->nSvrNum==0 && command->in_flight==0, "all factory kinds initialized");
        check(std::ranges::all_of(command->dwSessionID, [](INT32 id){return id==0;}), "zeroed selected-session fields");
        std::weak_ptr<CMD_BASE> life=command;
        check(manager.DelCmd(command), "release staged command");command.reset();
        check(life.expired(), "factory result releases actual derived owner");
    }
    check(!manager.NewCmd(0) && !manager.NewCmd(999) && !manager.DelCmd({}), "invalid factory and retirement inputs");
    CRoleInfo dialog;g_pRoleInfo=&dialog;
    check(manager.IsCmdExecuting() && !manager.IsCmdExecuting(TRUE), "role editor excludes itself from busy check");g_pRoleInfo=nullptr;
    check(manager.QueryRoleID(1,"name") && manager.IsCmdExecuting(TRUE), "activate command");
    auto current=manager.GetCurrentCmd();std::weak_ptr<CMD_BASE> life=current;
    check(!manager.SendGMStrCmd("another") && manager.GetCurrentCmd()==current && calls==1, "busy command cannot be overwritten");
    manager.DelCmd(current);check(!manager.GetCurrentCmd() && !life.expired(), "callback snapshot survives retirement");
    current.reset();check(life.expired(), "retired command freed after callback snapshot");

    reset();on_send=[](INT32){return FALSE;};
    check(!manager.QueryRoleID(1,"name") && !manager.GetCurrentCmd(), "failed send clears exact active owner");
    check(manager.MainLoop() && !manager.IsCmdExecuting(), "failure leaves no dangling command");
    reset();on_send=[](INT32)->BOOL{throw std::runtime_error("send exception");};
    bool threw=false;try { manager.QueryRoleID(1,"name"); } catch(const std::runtime_error&) { threw=true; }
    check(threw && !manager.GetCurrentCmd(), "throwing send releases command");
    reset();on_send=[&](INT32 type){
        if(type==CMD_QUERY_ROLE_ID){manager.CmdEnd();check(manager.SendGMStrCmd("replacement"),"callback creates replacement command");return FALSE;}
        return TRUE;
    };
    check(!manager.QueryRoleID(1,"name") && manager.GetCurrentCmd() && manager.GetCurrentCmd()->Kind()==CMD_EXECUTE_GM_STR,"failed old send cannot retire replacement");
    reset();on_send=[&](INT32){manager.UnInit();return TRUE;};
    check(!manager.QueryRoleID(1,"name") && !manager.GetCurrentCmd(), "reentrant shutdown leaves no active owner");
    reset();on_send=[&](INT32){auto c=manager.GetCurrentCmd();c->nSvrNum=0;manager.MainLoop();check(manager.GetCurrentCmd()==c,"in-flight command cannot be retired by poll");return TRUE;};
    check(manager.QueryRoleID(1,"name"), "inline completion send");
    manager.MainLoop();check(!manager.GetCurrentCmd(), "completion retires after send frame unwinds");

    reset();auto now=CCmdManager::Clock::time_point(std::chrono::seconds(2147483648LL));
    manager.SetClock([&]{return now;});
    check(manager.QueryRoleID(1,"name"), "steady-clock activation beyond signed Unix epoch");
    auto deadline=manager.GetCurrentCmd()->deadline;
    now=deadline-std::chrono::nanoseconds(1);manager.MainLoop();check(manager.GetCurrentCmd()!=nullptr,"before precise deadline");
    on_log=[&]{check(!manager.GetCurrentCmd(),"timeout retires before logging callback");check(manager.SendGMStrCmd("new"),"logging callback can start a fresh command");};
    now=deadline;manager.MainLoop();check(manager.GetCurrentCmd() && manager.GetCurrentCmd()->nType==CMD_EXECUTE_GM_STR,"timeout cleanup preserves newly started command");
    reset();now=CCmdManager::Clock::time_point::max()-std::chrono::seconds(1);manager.SetClock([&]{return now;});
    check(manager.QueryRoleID(1,"name") && manager.GetCurrentCmd()->deadline==CCmdManager::Clock::time_point::max(),"deadline saturates without clock overflow");

    reset();auto child=manager.NewCmd(CMD_LOAD_ROLE_DATA);std::weak_ptr<CMD_BASE> child_life=child;
    std::weak_ptr<CMD_BASE> parent_life;
    on_send=[&](INT32 type){
        if(type==CMD_PRE_QUERY){auto parent=manager.GetCurrentCmd();parent_life=parent;manager.MainLoop();check(manager.LoadRoleData(child),"owned pre-query hands off to child");manager.DelCmd(parent);check(manager.GetCurrentCmd()==child,"retiring wrapper preserves active child");}
        return TRUE;
    };
    check(manager.PreQuery(CMD_LOAD_ROLE_DATA,"name",child),"pre-query uses correct allocated type");
    check(parent_life.expired() && manager.GetCurrentCmd()==child,"wrapper released after outer send frame");
    child.reset();manager.CmdEnd();check(child_life.expired(),"child and wrapper owners do not cycle");
    reset();child=manager.NewCmd(CMD_LOAD_ROLE_DATA);bool nested=false;
    on_send=[&](INT32 type)
    {
        if(type==CMD_LOAD_ROLE_DATA && !nested)
        {
            nested=true;check(manager.PreQuery(CMD_LOAD_ROLE_DATA,"name",child),"wrapper can retain an issuing child");
        }
        else if(type==CMD_PRE_QUERY)
        {
            const auto wrapper=manager.GetCurrentCmd();
            check(!manager.LoadRoleData(child),"a different active wrapper cannot reuse an in-flight child");
            check(manager.GetCurrentCmd()==wrapper,"rejected child reuse preserves active wrapper");
        }
        return TRUE;
    };
    check(manager.LoadRoleData(child) && manager.GetCurrentCmd()->Kind()==CMD_PRE_QUERY,"nested wrapper survives outer child frame");
    reset();child=manager.NewCmd(CMD_LOAD_ROLE_DATA);
    check(!manager.PreQuery(CMD_KICK_ROLE,"name",child) && calls==0,"pre-query target kind checked");
    child->nType=CMD_KICK_ROLE;
    check(!manager.KickRole(child),"immutable allocation kind prevents wrong-type cast");

    reset();std::vector<char> maximum(MAX_GM_ROLE_DATA_STR_LENGTH,'x');
    check(manager.ModifyRole(7,8,static_cast<INT32>(maximum.size()),maximum.data(),"reason"),"role payload exceeds former 30-KiB command buffer safely");
    auto modified=std::static_pointer_cast<MODIFY_ROLE_DATA>(manager.GetCurrentCmd());
    maximum[0]='y';check(modified->data.size()==MAX_GM_ROLE_DATA_STR_LENGTH && modified->data.front()=='x' && sent_data==modified->data && sent_target==8 && sent_reason=="reason","owned full-size role payload");
    manager.CmdEnd();
    check(!manager.ModifyRole(7,8,-1,maximum.data(),"reason") && !manager.ModifyRole(7,8,MAX_GM_ROLE_DATA_STR_LENGTH+1,maximum.data(),"reason"),"invalid role payload lengths rejected");
    check(!manager.ModifyRole(7,8,1,nullptr,"reason") && !manager.ModifyRole(7,8,1,maximum.data(),std::string(GM_REASON_LEN,'r').c_str()),"role payload/reason boundaries");
    reset();check(!manager.QueryRoleID(1,std::string(USER_NAME_LEN,'n').c_str()) && calls==0,"overlong input never truncated into a command");
    check(!manager.QueryRoleData(manager.NewCmd(CMD_LOAD_ROLE_DATA)),"typed submission kind checked");
    check(!manager.ForbidChat(manager.NewCmd(CMD_FORBIDDEN_CHAT)) && !manager.FreezeRole(manager.NewCmd(CMD_FREEZE_ROLE)) && !manager.KickRole(manager.NewCmd(CMD_KICK_ROLE)) && !manager.SendMail(manager.NewCmd(CMD_SEND_MAIL)),"empty batch inputs rejected before begin dereference");
    auto run=[&](INT32 kind, const std::function<BOOL()>& operation){
        manager.CmdEnd();const auto before=calls;
        check(operation() && calls==before+1,"command family reaches its send boundary");
        const auto command=manager.GetCurrentCmd();
        check(command && command->Kind()==kind && command->in_flight==0,"family owns initialized active state");
        command->nSvrNum=0;manager.MainLoop();check(!manager.GetCurrentCmd(),"family completion releases active state");
    };
    auto query=std::static_pointer_cast<QUERY_ROLE_DATA>(manager.NewCmd(CMD_QUERY_ROLE_DATA));query->dwRoleID=1;
    run(CMD_QUERY_ROLE_DATA,[&]{return manager.QueryRoleData(query);});
    auto forbid=std::static_pointer_cast<FORBIDDEN_CHAT>(manager.NewCmd(CMD_FORBIDDEN_CHAT));forbid->roleList.push_back({});forbid->roleList.front().dwRoleID=1;
    run(CMD_FORBIDDEN_CHAT,[&]{return manager.ForbidChat(forbid);});
    auto freeze=std::static_pointer_cast<FREEZE_ROLE>(manager.NewCmd(CMD_FREEZE_ROLE));freeze->roleList.push_back({});freeze->roleList.front().dwRoleID=1;freeze->llTime=4294967295;
    run(CMD_FREEZE_ROLE,[&]{return manager.FreezeRole(freeze);});
    freeze->llTime=4294967296;
    check(!manager.FreezeRole(freeze),"expiration must fit existing wire field");
    auto kick=std::static_pointer_cast<KICK_ROLE>(manager.NewCmd(CMD_KICK_ROLE));kick->roleList.push_back({});kick->roleList.front().dwRoleID=1;
    run(CMD_KICK_ROLE,[&]{return manager.KickRole(kick);});
    auto mail=std::static_pointer_cast<SEND_MAIL>(manager.NewCmd(CMD_SEND_MAIL));mail->roleList.push_back({});mail->roleList.front().dwRoleID=1;
    run(CMD_SEND_MAIL,[&]{return manager.SendMail(mail);});
    run(CMD_ANNOUNCE,[&]{return manager.SendAnnounce(1,100,5,2,"text");});
    run(CMD_GET_ANNOUNCE_LIST,[&]{return manager.QueryAnnounceList(1);});
    run(CMD_DEL_ANNOUNCE,[&]{return manager.DelAnnounce(1,-1);});
    std::map<INT32,ROLE_NAME_DATA> names;names[1]={};std::memcpy(names[1].szRoleName,"name",5);
    run(CMD_QUERY_NAME_TO_ID,[&]{return manager.QueryNameToID(names);});
    run(CMD_EXECUTE_GM_STR,[&]{return manager.SendGMStrCmd("text");});
    auto load=std::static_pointer_cast<LOAD_ROLE_DATA>(manager.NewCmd(CMD_LOAD_ROLE_DATA));load->dwRoleID=1;
    run(CMD_LOAD_ROLE_DATA,[&]{return manager.LoadRoleData(load);});
    auto execute=std::static_pointer_cast<EXE_ROLE_CMD>(manager.NewCmd(CMD_EXE_ROLE_GM_CMD));execute->dwRoleID=1;
    run(CMD_EXE_ROLE_GM_CMD,[&]{return manager.ExeRoleGMCmd(execute);});
    run(CMD_LOG,[&]{return manager.QueryLog(0,1,2,"query");});
    manager.ClearSvrList();check(manager.First()==UINT32(-1) && manager.Next()==UINT32(-1) && manager.Next()==UINT32(-1),"empty cursor remains valid");
    manager.AddSvr(4);manager.AddSvr(9);
    check(manager.First()==4 && manager.Next()==9 && manager.Next()==UINT32(-1) && manager.Next()==UINT32(-1),"bounded standard server cursor");
    manager.UnInit();on_send={};on_log={};
    check(!manager.QueryRoleID(1,"name"),"shutdown gates new submissions");
    std::cout<<checks<<" GM command checks passed\n";
    return 0;
}
catch(const std::exception& error)
{
    std::cerr<<error.what()<<'\n';return 1;
}
