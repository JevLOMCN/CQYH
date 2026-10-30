#include "message_handler.h"
#include "gm_manager.h"
#include "log.h"
#include <algorithm>
#include <exception>
#include <string_view>
#include <utility>

CGMManager CGMManager::m_Instance;
CGMManager& CGMManager::GetInstance(){return m_Instance;}
CGMManager::CGMManager()=default;
CGMManager::~CGMManager()=default;
BOOL CGMManager::Init()
{
    if(m_runtime || m_starting || m_stopping || m_gate->retiring) return FALSE;
    m_starting=true;m_cancelStart=false;m_gate->cleanup_ok=true;
    struct Reset{bool& value;~Reset(){value=false;}} reset{m_starting};
    std::shared_ptr<gm::Runtime> runtime;
    try
    {
        runtime=std::make_shared<gm::Runtime>(gm::runtime_hooks(),m_gate);
        if(runtime->Start(&CMessageHandler::GetInstance(),runtime,[&]{return m_cancelStart;}))
        {m_runtime=std::move(runtime);return TRUE;}
    }
    catch(const std::exception& error){CRI("GM initialization failed: %s",error.what());}
    catch(...){CRI("GM initialization failed");}
    if(runtime){m_gate->retiring=true;runtime->StopControllers();runtime.reset();}
    return FALSE;
}
BOOL CGMManager::UnInit()
{
    if(m_starting){m_cancelStart=true;return TRUE;}
    if(m_stopping || !m_runtime) return TRUE;
    m_stopping=true;struct Reset{bool& value;~Reset(){value=false;}} reset{m_stopping};
    auto runtime=std::move(m_runtime);m_gate->retiring=true;
    return runtime->StopControllers();
}
BOOL CGMManager::MainLoop()
{
    if(!m_runtime || m_looping || m_starting || m_stopping) return TRUE;
    m_looping=true;struct Reset{bool& value;~Reset(){value=false;}} reset{m_looping};
    // Keep native resources alive until every native frame entered here returns.
    const auto runtime=m_runtime;
    try
    {
        if(!runtime->Net()->MainLoop()) {CRI("GM transport loop failed");UnInit();return FALSE;}
        if(m_runtime!=runtime) return TRUE;
        if(!runtime->PollSessions()) WRN("GM session polling requires retry");
        if(m_runtime!=runtime) return TRUE;
        if(!runtime->PollCommands()) WRN("GM command could not complete");
        return TRUE;
    }
    catch(const std::exception& error){CRI("GM runtime failed: %s",error.what());}
    catch(...){CRI("GM runtime failed");}
    UnInit();return FALSE;
}
void CGMManager::SetGMDialogState(GM_DIALOG_STATE state){m_eGMDialogState=state;}
GM_DIALOG_STATE CGMManager::GetGMDialogState(){return m_eGMDialogState;}
const char* CGMManager::GetUserName(){return m_szUserName.data();}
const char* CGMManager::GetUserPwd(){return m_szUserPwd.data();}
namespace
{
template<std::size_t N> void copy(std::array<char,N>& target,const char* text)
{
    if(!text)return;target.fill(0);const std::string_view source(text);
    std::copy_n(source.begin(),std::min(source.size(),N-1),target.begin());
}
}
void CGMManager::SetUserName(const char* name){copy(m_szUserName,name);}
void CGMManager::SetUserPwd(const char* password){copy(m_szUserPwd,password);}
