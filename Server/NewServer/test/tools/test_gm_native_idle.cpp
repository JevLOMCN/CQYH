#include "message_handler.h"
#include "gm_manager.h"
#include "gm_session_events.h"
#include "session_mgr.h"
#include "server_mgr.h"
#include "cmd_manager.h"
#include "gm_log.h"
#include "tl_config.h"
#include "log.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

TL_CONFIG g_TLConfig{};
class CRoleInfo {};
CRoleInfo* g_pRoleInfo=nullptr;
namespace
{
std::size_t checks{},unexpected_events{};
void check(bool value,const char* text){++checks;if(!value)throw std::runtime_error(text);}
void config(const std::string& source)
{
    std::ofstream output("config/gmtool_config.lua",std::ios::binary|std::ios::trunc);output<<source;output.close();
    check(static_cast<bool>(output),"fixture configuration saved");
}
}
CServerMgr CServerMgr::m_Instance;
CServerMgr& CServerMgr::GetInstance(){return m_Instance;}
BOOL CServerMgr::Init(){m_nSelectedSessionNum=0;return TRUE;}
UINT32 CServerMgr::GetSessionIDByIndex(INT32){return UINT32(-1);}
void CServerMgr::ResetSelectedServer(){m_nSelectedSessionNum=0;}
CGMLog CGMLog::m_Instance;
CGMLog::CGMLog()=default;CGMLog::~CGMLog()=default;
CGMLog& CGMLog::GetInstance(){return m_Instance;}
BOOL CGMLog::Log(std::string_view){++unexpected_events;return FALSE;}
void CMessageHandler::OnAccepted(ISocketAcceptor*,ISocketSession*){++unexpected_events;}
void gm::session_disconnected(const GM_SESSION&,UINT64){++unexpected_events;}
int main(int argc,char** argv)
{
    try
    {
        check(argc==2,"isolated output directory argument");
        const auto directory=std::filesystem::absolute(argv[1]);std::filesystem::create_directories(directory/"config");
        struct Restore{std::filesystem::path old=std::filesystem::current_path();~Restore(){std::error_code ignored;std::filesystem::current_path(old,ignored);}} restore;
        std::filesystem::current_path(directory);
        auto& manager=CGMManager::GetInstance();
        struct Stop{CGMManager& manager;~Stop(){manager.UnInit();}} stop{manager};
        config("function LoadFile() error('fixture invalid configuration') end\n");
        check(!manager.Init() && !manager.IsRunning() && !manager.IsRetiring(),"actual logger/configuration failure rolls back before transport");
        config(R"(function LoadFile()
 local c=GetTLConfig()
 c.ServerList.nSvrNum=1
 local s=c.ServerList.arrGMList[0]
 s.nSvrID=7
 s.szName='idle fixture'
 s.szAddr='127.0.0.1'
 s.dwPort=39999
end
)");
        for(int pass=0;pass<3;++pass)
        {
            check(manager.Init() && manager.IsRunning(),"actual native runtime initializes after prior retirement");
            auto* session=CSessionMgr::GetInstance().GetGMSession(0);
            check(session && session->name=="idle fixture" && session->nConnectState==SESSION_DISCONNECTED && !session->pSocketSession,
                "idle native session stays unconnected before host loop polling");
            check(!CSessionMgr::GetInstance().CheckSession() && !CCmdManager::GetInstance().GetCurrentCmd(),"idle native startup owns no peer socket or command");
            // Do not call MainLoop: the native host loop is the only path that issues Connect.
            check(manager.UnInit() && manager.CleanupSucceeded() && !manager.IsRunning() && !manager.IsRetiring(),
                "actual transport/pools/logger retire synchronously without outstanding frames");
            check(!CSessionMgr::GetInstance().GetGMSession(0) && manager.UnInit(),"native teardown is idempotent and releases session metadata");
        }
        check(unexpected_events==0,"native idle startup/teardown emits no peer receive/login/GM/UI operation");
        std::cout<<checks<<" GM native idle checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';CGMManager::GetInstance().UnInit();return 1;}
}

void CMessageHandler::OnGMRegisterAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMVerifyAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMLoginAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMItemListNtf(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMLogoutAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMPreQueryAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMQueryRoleID(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMQueryRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMForbidChatAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMFreezeRoleAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMKickRoleAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMSendMailAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMAnnouceAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMGetAnnounceAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMDelAnnounce(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMLogAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMExecuteGMStrAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMLoadRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMModifyRoleDataAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMQueryNameToIDAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMErrorStr(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMRoleExeGMCmd(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMSelectGroupAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMQueryExchangeLogAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMQueryExchangeHistoryAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMExchangePrice(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMQueryOrderDetailAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMAddSystemOrderAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMDelSystemOrderAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMQuerySysExchangeInfoAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMGetExchangeLimitAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}

void CMessageHandler::OnGMSetExchangeLimitAck(PACKET_BUFFER*,UINT32,ISocketSession*) {++unexpected_events;}
