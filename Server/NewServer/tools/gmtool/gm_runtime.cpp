#include "gm_runtime.h"
#include <utility>

bool gm::RuntimeHooks::Complete() const
{
    return log_start && log_stop && load_config && allocator_create && allocator_destroy && net_create && net_destroy &&
        sessions_start && sessions_stop && sessions_poll && handler_start && handler_stop && commands_start && commands_stop &&
        commands_poll && data_start && data_stop && release_allocator;
}
gm::Runtime::Runtime(RuntimeHooks hooks,std::shared_ptr<RuntimeGate> gate) : m_gate(std::move(gate)),m_hooks(std::move(hooks)),
    m_allocator(nullptr,{m_hooks.allocator_destroy,m_gate}),m_net(nullptr,{m_hooks.net_destroy,m_gate}) {}
bool gm::Runtime::Stop(bool& attempted,const std::function<BOOL()>& close) noexcept
{
    if(!std::exchange(attempted,false)) return true;
    try{if(close())return true;}catch(...){}
    m_gate->cleanup_ok=false;return false;
}
BOOL gm::Runtime::StopControllers() noexcept
{
    bool success=Stop(m_handler,m_hooks.handler_stop);
    success=Stop(m_sessions,m_hooks.sessions_stop) && success;
    success=Stop(m_commands,m_hooks.commands_stop) && success;
    return Stop(m_data,m_hooks.data_stop) && success;
}
gm::Runtime::~Runtime()
{
    StopControllers();m_net.reset();
    if(m_allocator)
        try{m_hooks.release_allocator(m_allocator.get());}catch(...){m_gate->cleanup_ok=false;}
    m_allocator.reset();Stop(m_log,m_hooks.log_stop);m_gate->retiring=false;
}
BOOL gm::Runtime::Start(INetEventHandler* handler,const std::shared_ptr<void>& lifetime,const std::function<bool()>& cancelled)
{
    if(!m_hooks.Complete()) return FALSE;
    m_log=true;if(!m_hooks.log_start() || cancelled()) return FALSE;
    if(!m_hooks.load_config() || cancelled()) return FALSE;
    m_allocator.reset(m_hooks.allocator_create());if(!m_allocator || cancelled()) return FALSE;
    m_net.reset(m_hooks.net_create(m_allocator.get(),handler));if(!m_net || cancelled()) return FALSE;
    m_sessions=true;if(!m_hooks.sessions_start(m_net.get()) || cancelled()) return FALSE;
    m_handler=true;if(!m_hooks.handler_start(m_net.get(),m_allocator.get(),lifetime) || cancelled()) return FALSE;
    m_commands=true;if(!m_hooks.commands_start() || cancelled()) return FALSE;
    m_data=true;return m_hooks.data_start() && !cancelled();
}
