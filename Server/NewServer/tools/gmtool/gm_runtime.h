#pragma once
#ifdef _WIN32
#include <winsock2.h>
#endif
#include "std_type.h"
#include "net.h"
#include <functional>
#include <memory>

namespace gm
{
struct RuntimeGate { bool retiring=false,cleanup_ok=true; };
struct RuntimeHooks
{
    std::function<BOOL()> log_start,log_stop,load_config;
    std::function<IPacketAllocator*()> allocator_create;
    // Owning destroy hooks consume their object even when reporting cleanup failure.
    std::function<BOOL(IPacketAllocator*)> allocator_destroy;
    std::function<INet*(IPacketAllocator*,INetEventHandler*)> net_create;
    std::function<BOOL(INet*)> net_destroy,sessions_start;
    std::function<BOOL()> sessions_stop,sessions_poll,commands_start,commands_stop,commands_poll,data_start,data_stop;
    std::function<BOOL(INet*,IPacketAllocator*,std::shared_ptr<void>)> handler_start;
    std::function<BOOL()> handler_stop;
    std::function<void(IPacketAllocator*)> release_allocator;
    bool Complete() const;
};
RuntimeHooks runtime_hooks();
template<class T> struct RuntimeDelete
{
    std::function<BOOL(T*)> destroy;
    std::shared_ptr<RuntimeGate> gate;
    void operator()(T* value) const noexcept
    {
        if(!value) return;
        try{if(!destroy(value))gate->cleanup_ok=false;}catch(...){gate->cleanup_ok=false;}
    }
};
class Runtime
{
    std::shared_ptr<RuntimeGate> m_gate;
    RuntimeHooks m_hooks;
    std::unique_ptr<IPacketAllocator,RuntimeDelete<IPacketAllocator>> m_allocator;
    std::unique_ptr<INet,RuntimeDelete<INet>> m_net;
    bool m_log=false,m_sessions=false,m_handler=false,m_commands=false,m_data=false;
    bool Stop(bool& attempted,const std::function<BOOL()>& close) noexcept;
public:
    Runtime(RuntimeHooks hooks,std::shared_ptr<RuntimeGate> gate);
    ~Runtime();
    Runtime(const Runtime&)=delete;
    Runtime& operator=(const Runtime&)=delete;
    BOOL Start(INetEventHandler* handler,const std::shared_ptr<void>& lifetime,const std::function<bool()>& cancelled);
    BOOL StopControllers() noexcept;
    BOOL PollSessions(){return m_hooks.sessions_poll();}
    BOOL PollCommands(){return m_hooks.commands_poll();}
    INet* Net() const {return m_net.get();}
};
}
