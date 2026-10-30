#include "message_handler.h"
#include "process_error.h"
#include "gm_runtime.h"
#include "gm_configuration.h"
#include "packet_allocator.h"
#include "session_mgr.h"
#include "cmd_manager.h"
#include "data_mgr.h"
#include "log.h"
#include "tl_config.h"

gm::RuntimeHooks gm::runtime_hooks()
{
    RuntimeHooks hooks;
    hooks.log_start=[]{return CLog::GetInstance().Init("gm_tool",0xFFFFFFFF,0xFFFFFFFF,FALSE,FALSE,TRUE);};
    hooks.log_stop=[]{return CLog::GetInstance().UnInit();};
    hooks.load_config=[]
    {
        const auto result=reload_configuration("./config/gmtool_config.lua",g_TLConfig);
        if(!result) CRI("GM configuration: %s",result.error().c_str());
        else INF("loaded gmtool configuration successfully");
        return result.has_value();
    };
    hooks.allocator_create=[]()->IPacketAllocator*
    {
        auto allocator=std::make_unique<CPacketAllocator>();
        if(!allocator->Init(SMALL_PACKET_COUNT,LARGE_PACKET_COUNT))return nullptr;
        return allocator.release();
    };
    hooks.allocator_destroy=[](IPacketAllocator* allocator){delete static_cast<CPacketAllocator*>(allocator);return TRUE;};
    hooks.net_create=[](IPacketAllocator* allocator,INetEventHandler* handler)
    {
        SIMPLE_INIT_PARAM options{};
        options.nMaxAcceptor=MAX_ACCEPTOR;options.nMaxConnector=MAX_CONNECTOR;options.nMaxSessionCount=MAX_SESSION_COUNT;
        options.nSendBufSize=SEND_BUF_SIZE;options.nRecvBufSize=RECV_BUF_SIZE;options.nPacketBufferSize=PACKET_BUFFER_SIZE;
        return g_CreateNetInterface(netSimple,allocator,handler,&options);
    };
    hooks.net_destroy=[](INet* net){return g_DestroyNetInterface(netSimple,net);};
    hooks.sessions_start=[](INet* net){return CSessionMgr::GetInstance().Init(net);};
    hooks.sessions_stop=[]{return CSessionMgr::GetInstance().UnInit();};
    hooks.sessions_poll=[]{return CSessionMgr::GetInstance().MainLoop();};
    hooks.handler_start=[](INet* net,IPacketAllocator* allocator,std::shared_ptr<void> lifetime)
    {return CMessageHandler::GetInstance().Init(net,allocator,std::move(lifetime));};
    hooks.handler_stop=[]{return CMessageHandler::GetInstance().UnInit();};
    hooks.release_allocator=[](IPacketAllocator* allocator){CMessageHandler::GetInstance().ReleaseAllocator(allocator);};
    hooks.commands_start=[]{return CCmdManager::GetInstance().Init();};hooks.commands_stop=[]{return CCmdManager::GetInstance().UnInit();};
    hooks.commands_poll=[]{return CCmdManager::GetInstance().MainLoop();};
    hooks.data_start=[]{return CDataMgr::GetInstance().Init();};hooks.data_stop=[]{return CDataMgr::GetInstance().UnInit();};
    return hooks;
}
