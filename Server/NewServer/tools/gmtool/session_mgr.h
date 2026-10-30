#pragma once
#include "std_type.h"
#include "predefine.h"
#include "server_def.h"
#include <array>
#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
class INet;
class ISocketSession;
struct GM_SESSION
{
    UINT32 dwType{};
    ISocketSession* pSocketSession=nullptr; // Native transport owns the socket.
    // Distinguish reconnects even when the native transport reuses a socket address.
    std::shared_ptr<const char> connection_lifetime;
    bool closing=false;
    std::chrono::steady_clock::time_point deadline{};
    INT32 nConnectState=SESSION_DISCONNECTED;
    UINT32 dwSessionID{};
    GM_SVR_STATE eState=GM_SVR_STATE_NONE;
    INT32 nSvrID{};
    std::string name;
};
class CSessionMgr
{
protected:
    CSessionMgr();
    ~CSessionMgr();
    static CSessionMgr m_Instance;
public:
    using Clock=std::chrono::steady_clock;
    static CSessionMgr& GetInstance();
    BOOL Init(INet* net);
    BOOL UnInit();
    GM_SESSION* GetGMSession(UINT32 id);
    GM_SESSION* FromSocket(ISocketSession* socket) { return Find(socket); }
    BOOL CheckSession();
    BOOL CloseAllSession();
    BOOL MainLoop();
    BOOL SetClock(std::function<Clock::time_point()> clock);
    GM_SESSION* Connected(ISocketSession* socket);
    std::optional<GM_SESSION> Disconnected(ISocketSession* socket);
    BOOL Refresh(ISocketSession* socket);
    BOOL Owns(ISocketSession* socket) const;
    BOOL CloseSession(ISocketSession* socket, bool reconnect = true);
    UINT64 Generation() const { return m_generation; }
protected:
    INet* m_pNet=nullptr;
    std::array<GM_SESSION,MAX_GM_SESSION_COUNT> m_GMSession{};
private:
    struct Endpoint { UINT32 address{};UINT16 port{}; };
    std::array<Endpoint,MAX_GM_SESSION_COUNT> m_endpoints{};
    std::function<Clock::time_point()> m_now=Clock::now;
    std::optional<Clock::time_point> m_nextPing,m_lastPoll;
    bool m_active=false,m_polling=false,m_closing=false;
    UINT64 m_generation=0;
    GM_SESSION* Find(ISocketSession* socket);
};

namespace gm
{
// An owned identity keeps an old binding distinct even if the socket address is reused.
struct ConnectionBinding
{
    UINT32 id;
    ISocketSession* socket;
    UINT64 generation;
    std::shared_ptr<const char> lifetime;
    bool operator==(const ConnectionBinding&) const = default;
    bool Current() const
    {
        auto& sessions=CSessionMgr::GetInstance();
        const auto* session=sessions.GetGMSession(id);
        return lifetime && sessions.Generation()==generation && session &&
            session->nConnectState==SESSION_CONNECTED && session->pSocketSession==socket &&
            session->connection_lifetime==lifetime;
    }
};
inline std::optional<ConnectionBinding> connection_binding(UINT32 id)
{
    auto& sessions=CSessionMgr::GetInstance();
    const auto* session=sessions.GetGMSession(id);
    if (!session || !session->pSocketSession || !session->connection_lifetime ||
        session->nConnectState!=SESSION_CONNECTED) return {};
    return ConnectionBinding{id,session->pSocketSession,sessions.Generation(),session->connection_lifetime};
}
}

