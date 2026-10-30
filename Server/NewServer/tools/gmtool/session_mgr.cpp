#include "message_handler.h"
#include "session_mgr.h"
#include "tl_config.h"
#include "log.h"
#include <algorithm>
#include <cstring>
#include <set>
#include <utility>
namespace
{
using Clock=CSessionMgr::Clock;
Clock::time_point after(Clock::time_point now,std::chrono::milliseconds interval)
{
    return now>Clock::time_point::max()-interval ? Clock::time_point::max() : now+interval;
}
}
CSessionMgr CSessionMgr::m_Instance;
CSessionMgr& CSessionMgr::GetInstance(){return m_Instance;}
CSessionMgr::CSessionMgr()=default;
CSessionMgr::~CSessionMgr()=default;
BOOL CSessionMgr::SetClock(std::function<Clock::time_point()> clock)
{
    if(m_active || m_polling || m_closing) return FALSE;
    m_now=clock ? std::move(clock) : Clock::now;return TRUE;
}
BOOL CSessionMgr::Init(INet* net)
{
    if(!net || m_active || m_polling || m_closing) return FALSE;
    static_assert(MAX_GM_SESSION_COUNT<=GM_MAX_SERVER_COUNT);
    std::array<GM_SESSION,MAX_GM_SESSION_COUNT> sessions{};
    std::array<Endpoint,MAX_GM_SESSION_COUNT> endpoints{};
    const auto now=m_now();
    std::set<INT32> ids;
    for(std::size_t index=0;index<sessions.size();++index)
    {
        auto& session=sessions[index];const auto& config=g_TLConfig.ServerList.arrGMList[index];
        session.dwType=stGM;session.dwSessionID=static_cast<UINT32>(index);
        session.deadline=now;
        if(!config.nSvrID) continue;
        if(config.nSvrID<0 || !ids.insert(config.nSvrID).second || !config.dwPort || config.dwPort>65535) return FALSE;
        const auto* name_end=static_cast<const char*>(std::memchr(config.szName,0,sizeof(config.szName)));
        if(!name_end || !std::memchr(config.szAddr,0,sizeof(config.szAddr))) return FALSE;
        auto address=inet_addr(config.szAddr);
        if(address==INADDR_NONE) return FALSE;
        if(!address) address=inet_addr(g_szLocalAddress);
        if(!address || address==INADDR_NONE) return FALSE;
        session.nSvrID=config.nSvrID;session.name.assign(config.szName,name_end);
        endpoints[index]={address,static_cast<UINT16>(config.dwPort)};
    }
    m_GMSession=std::move(sessions);m_endpoints=endpoints;m_pNet=net;
    m_nextPing.reset();m_lastPoll.reset();++m_generation;m_active=true;return TRUE;
}
BOOL CSessionMgr::CloseAllSession()
{
    if(m_closing) return TRUE;
    m_closing=true;struct Reset{bool& value;~Reset(){value=false;}} reset{m_closing};
    m_active=false;m_pNet=nullptr;++m_generation;m_nextPing.reset();m_lastPoll.reset();
    struct Closing { ISocketSession* socket{};bool issue{}; };
    std::array<Closing,MAX_GM_SESSION_COUNT> sockets{};
    for(std::size_t index=0;index<m_GMSession.size();++index)
    {
        auto& session=m_GMSession[index];sockets[index]={session.pSocketSession,!session.closing};
        session.pSocketSession=nullptr;session.connection_lifetime.reset();session.nConnectState=SESSION_DEAD;session.eState=GM_SVR_STATE_DISCONNECT;
    }
    bool success=true;
    for(const auto [socket,issue]:sockets) if(socket)
    {
        try{socket->SetUserData(nullptr);if(issue)success=(socket->Close()!=FALSE) && success;}
        catch(...){success=false;CRI("GM socket close failed");}
    }
    return success;
}
BOOL CSessionMgr::UnInit()
{
    if(m_closing) return TRUE;
    const auto closed=CloseAllSession();m_GMSession={};m_endpoints={};return closed;
}
GM_SESSION* CSessionMgr::GetGMSession(UINT32 id)
{
    return m_active && id<m_GMSession.size() && m_GMSession[id].nSvrID ? &m_GMSession[id] : nullptr;
}
GM_SESSION* CSessionMgr::Find(ISocketSession* socket)
{
    if(!m_active || !socket) return nullptr;
    const auto found=std::ranges::find_if(m_GMSession,[&](const auto& session){return session.pSocketSession==socket;});
    return found==m_GMSession.end() ? nullptr : &*found;
}
BOOL CSessionMgr::Owns(ISocketSession* socket) const
{
    return m_active && socket && std::ranges::any_of(m_GMSession,[&](const auto& session){return session.pSocketSession==socket;});
}
BOOL CSessionMgr::CheckSession()
{
    return std::ranges::any_of(m_GMSession,[](const auto& session){return session.pSocketSession!=nullptr;});
}
GM_SESSION* CSessionMgr::Connected(ISocketSession* socket)
{
    auto* session=Find(socket);
    if(!session || session->nConnectState!=SESSION_CONNECTING) return nullptr;
    session->nConnectState=SESSION_CONNECTED;session->deadline=after(m_now(),std::chrono::milliseconds(PING_SVR_TIMEOUT));return session;
}
std::optional<GM_SESSION> CSessionMgr::Disconnected(ISocketSession* socket)
{
    auto* session=Find(socket);if(!session) return {};
    auto snapshot=*session;session->pSocketSession=nullptr;session->connection_lifetime.reset();session->closing=false;
    if(session->nConnectState!=SESSION_DEAD) session->nConnectState=SESSION_DISCONNECTED;
    session->deadline=after(m_now(),std::chrono::milliseconds(RECONNECT_TIME));
    if(session->eState!=GM_SVR_STATE_NONE) session->eState=GM_SVR_STATE_DISCONNECT;
    socket->SetUserData(nullptr);return snapshot;
}
BOOL CSessionMgr::Refresh(ISocketSession* socket)
{
    auto* session=Find(socket);if(!session || session->nConnectState!=SESSION_CONNECTED) return FALSE;
    session->deadline=after(m_now(),std::chrono::milliseconds(PING_SVR_TIMEOUT));return TRUE;
}
BOOL CSessionMgr::CloseSession(ISocketSession* socket, bool reconnect)
{
    auto* session=Find(socket);if(!session) return FALSE;
    if(session->closing) return TRUE;
    const auto generation=m_generation;session->closing=true;
    struct Reset
    {
        CSessionMgr& manager;GM_SESSION* session;ISocketSession* socket;UINT64 generation;
        ~Reset(){if(manager.Generation()==generation && manager.Owns(socket))session->closing=false;}
    } reset{*this,session,socket,generation};
    session->nConnectState=reconnect ? SESSION_DISCONNECTED : SESSION_DEAD;session->deadline=after(m_now(),std::chrono::milliseconds(RECONNECT_TIME));
    return socket->Close();
}
BOOL CSessionMgr::MainLoop()
{
    if(!m_active || !m_pNet) return FALSE;
    if(m_polling || m_closing) return TRUE;
    const auto now=m_now();if(m_lastPoll && *m_lastPoll==now) return TRUE;
    m_lastPoll=now;m_polling=true;struct Reset{bool& value;~Reset(){value=false;}} reset{m_polling};
    const auto generation=m_generation;auto* const net=m_pNet;
    const auto current=[&]{return m_active && m_generation==generation && m_pNet==net;};
    bool success=true;
    if(!m_nextPing || now>=*m_nextPing)
    {
        m_nextPing=after(now,std::chrono::milliseconds(PING_SVR_INTERNAL));
        try{success=CMessageHandler::GetInstance().DoAllPing()!=FALSE;}
        catch(...){success=false;CRI("GM ping failed");}
        if(!current()) return FALSE;
    }
    for(std::size_t index=0;index<m_GMSession.size();++index)
    {
        auto& session=m_GMSession[index];if(!session.nSvrID || session.nConnectState==SESSION_DEAD) continue;
        if(session.pSocketSession)
        {
            if(now>=session.deadline)
            {
                try{success=(CloseSession(session.pSocketSession)!=FALSE) && success;}
                catch(...){success=false;CRI("GM session close failed");}
                if(!current()) return FALSE;
            }
            continue;
        }
        if(session.nConnectState!=SESSION_DISCONNECTED || now<session.deadline) continue;
        session.nConnectState=SESSION_CONNECTING;session.deadline=after(now,std::chrono::milliseconds(PING_SVR_TIMEOUT));
        ISocketSession* socket=nullptr;
        std::shared_ptr<const char> connection_lifetime;
        try{connection_lifetime=std::make_shared<const char>(0);socket=net->Connect(m_endpoints[index].address,m_endpoints[index].port);}
        catch(...){success=false;CRI("GM connection failed");}
        if(!current())
        {
            if(socket) try{socket->SetUserData(nullptr);socket->Close();}catch(...){CRI("GM orphan connection cleanup failed");}
            return FALSE;
        }
        if(!socket){session.nConnectState=SESSION_DISCONNECTED;session.deadline=after(now,std::chrono::milliseconds(RECONNECT_TIME));success=false;continue;}
        if(Owns(socket)){session.nConnectState=SESSION_DISCONNECTED;session.deadline=after(now,std::chrono::milliseconds(RECONNECT_TIME));success=false;continue;}
        session.pSocketSession=socket;session.connection_lifetime=std::move(connection_lifetime);
        try{socket->SetUserData(&session);}
        catch(...)
        {
            if(!current()) return FALSE;
            session.pSocketSession=nullptr;session.connection_lifetime.reset();session.nConnectState=SESSION_DISCONNECTED;
            session.deadline=after(now,std::chrono::milliseconds(RECONNECT_TIME));success=false;
            try{socket->SetUserData(nullptr);socket->Close();}catch(...){CRI("GM connection binding cleanup failed");}
        }
        if(!current()) return FALSE;
    }
    return success;
}
