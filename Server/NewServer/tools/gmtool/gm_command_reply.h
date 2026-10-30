#pragma once
#include "cmd_manager.h"
#include "session_mgr.h"
#include <algorithm>
#include <optional>
#include <utility>

namespace gm
{
// Retain the original command through UI callbacks and nested event loops.
class CommandReply
{
    CommandPtr m_owner;
    UINT32 m_peer;
    ISocketSession* m_socket;
    UINT64 m_generation;
    std::shared_ptr<const char> m_connection;
    CommandReply(CommandPtr owner, UINT32 peer, ISocketSession* socket, UINT64 generation)
        : m_owner(std::move(owner)), m_peer(peer), m_socket(socket), m_generation(generation), m_connection(m_owner->replies.at(peer).connection_lifetime)
    {
        m_owner->replies.at(peer).busy = true;
        ++m_owner->in_flight;
        ++m_owner->reply_deliveries;
    }
public:
    CommandReply(const CommandReply&) = delete;
    CommandReply& operator=(const CommandReply&) = delete;
    CommandReply(CommandReply&& other) noexcept
        : m_owner(std::exchange(other.m_owner, {})), m_peer(other.m_peer),
          m_socket(other.m_socket), m_generation(other.m_generation), m_connection(std::move(other.m_connection)) {}
    ~CommandReply()
    {
        if (!m_owner) return;
        m_owner->replies.at(m_peer).busy = false;
        --m_owner->in_flight;
        --m_owner->reply_deliveries;
    }
    static std::optional<CommandReply> Begin(INT32 kind, ISocketSession* socket)
    {
        auto owner = CCmdManager::GetInstance().GetCurrentCmd();
        auto& sessions = CSessionMgr::GetInstance();
        const auto* session = sessions.FromSocket(socket);
        if (!owner || owner->Kind() != kind || owner->nType != kind || owner->nSvrNum <= 0 ||
            !session || session->nConnectState != SESSION_CONNECTED) return {};
        const auto peer = session->dwSessionID;
        const auto found = owner->replies.find(peer);
        if (found == owner->replies.end()) return {};
        const auto& reply = found->second;
        if (!reply.connection_lifetime || reply.connection_lifetime!=session->connection_lifetime ||
            reply.socket != socket || reply.generation != sessions.Generation() ||
            !reply.issued || reply.busy || reply.done) return {};
        return CommandReply(std::move(owner), peer, socket, sessions.Generation());
    }
    const CommandPtr& Owner() const { return m_owner; }
    UINT32 Peer() const { return m_peer; }
    bool Current() const
    {
        auto& sessions = CSessionMgr::GetInstance();
        const auto* session = sessions.FromSocket(m_socket);
        return CCmdManager::GetInstance().GetCurrentCmd() == m_owner &&
            sessions.Generation() == m_generation && session && session->dwSessionID == m_peer &&
            session->nConnectState == SESSION_CONNECTED && session->connection_lifetime==m_connection;
    }
    void Complete()
    {
        auto& reply = m_owner->replies.at(m_peer);
        if (reply.done) return;
        reply.done = true;
        m_owner->nSvrNum = std::max(0, m_owner->nSvrNum - 1);
    }
};
}
