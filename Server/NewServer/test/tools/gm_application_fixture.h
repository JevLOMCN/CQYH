#pragma once
#include "stdafx.h"
#include "gmtool.h"
#include "loader.h"
#include "gm_manager.h"
#include "session_mgr.h"
#include "message_handler.h"
#include "cmd_manager.h"
#include "data_mgr.h"
#include "tl_config.h"
#include "gmtl_message.h"
#include "error_def.h"
#include "log.h"
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtGui/QFontDatabase>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>

INT32 g_State=1;
using namespace InternalMessage;
namespace
{
int checks=0;
void check(bool value,const char* message){++checks;if(!value)throw std::runtime_error(message);}
struct Socket;struct Net;
struct State
{
    IPacketAllocator* allocator{};Net* net{};
    std::size_t allocations{},frees{},userdata_reads{},closes{};
    std::vector<std::pair<UINT16,std::vector<char>>> sent;
    std::function<void(Socket&,UINT16)> send_hook;
    bool accept=true, reuse_connections=false;
};
std::shared_ptr<State> current;
struct Allocator final:IPacketAllocator
{
    std::shared_ptr<State> state;
    std::map<char*,std::unique_ptr<char[]>> packets;
    explicit Allocator(std::shared_ptr<State> value):state(std::move(value)){state->allocator=this;}
    ~Allocator(){if(!packets.empty())std::abort();state->allocator=nullptr;}
    PACKET_BUFFER* Alloc(INT32 size) override
    {auto data=std::make_unique<char[]>(size);auto* raw=data.get();packets.emplace(raw,std::move(data));++state->allocations;return raw;}
    void Free(PACKET_BUFFER* packet) override{if(packets.erase(packet)!=1)std::abort();++state->frees;}
    BOOL IsAllocated(PACKET_BUFFER* packet) override{return packets.contains(packet);}
    UINT32 PrintMemoryStat() override{return static_cast<UINT32>(packets.size());}
    void CheckError() override{}
};
struct Socket final:ISocketSession
{
    std::shared_ptr<State> state;void* data{};
    explicit Socket(std::shared_ptr<State> value):state(std::move(value)){}
    INT32 Send(PACKET_BUFFER* packet,INT32 size,INT32) override
    {
        const auto context=state;
        MESSAGE_HEADER header;std::memcpy(&header,packet,sizeof(header));
        context->sent.emplace_back(header.wMsg,std::vector<char>(packet,packet+size));
        const auto hook=context->send_hook;if(hook)hook(*this,header.wMsg);
        if(!context->accept)return 0;
        context->allocator->Free(packet);return size;
    }
    INT32 SendNoPackageHead(PACKET_BUFFER*,INT32) override{return 0;}
    void* GetUserData() override{++state->userdata_reads;return data;}
    void SetUserData(void* value) override{data=value;}
    BOOL GetAddress(UINT32& address,UINT16& port) override{address=inet_addr("127.0.0.1");port=39999;return TRUE;}
    BOOL GetRemoteAddress(UINT32&,UINT16&) override{return FALSE;}
    BOOL Close() override{++state->closes;CMessageHandler::GetInstance().OnDisconnected(this,FALSE);return TRUE;}
};
struct Net final:INet
{
    std::shared_ptr<State> state;std::vector<std::unique_ptr<Socket>> sockets;
    explicit Net(std::shared_ptr<State> value):state(std::move(value)){state->net=this;}
    ~Net(){for(const auto& socket:sockets)if(socket->data)std::abort();state->net=nullptr;}
    BOOL Init(IPacketAllocator*,INetEventHandler*,void*) override{return TRUE;}
    BOOL UnInit() override{return TRUE;}
    ISocketAcceptor* CreateAcceptor() override{return nullptr;}
    BOOL DestroyAcceptor(ISocketAcceptor*) override{return TRUE;}
    ISocketSession* Connect(UINT32,UINT16 port) override
    {
        if(state->reuse_connections && port>=3000 && port-3000<sockets.size())return sockets[port-3000].get();
        auto socket=std::make_unique<Socket>(state);auto* raw=socket.get();sockets.push_back(std::move(socket));return raw;
    }
    BOOL MainLoop() override{return TRUE;}
    INT64 PrintMemoryStat() override{return 0;}
};
template<class T> T message(UINT16 id){T value{};value.wMsg=id;return value;}
template<class T> void deliver(Socket& socket,const T& value)
{
    auto* buffer=socket.state->allocator->Alloc(sizeof(value));std::memcpy(buffer,&value,sizeof(value));
    CMessageHandler::GetInstance().OnRecv(&socket,buffer,sizeof(value));
}
void verification(Socket& socket,INT32 result=errSuccess,bool exchange=false)
{auto reply=message<GM2TL_VERIFY_ACK>(gm2tl_verify_ack);reply.nResult=result;reply.bLD=exchange;deliver(socket,reply);}
void server_error(Socket& socket, std::string_view text)
{
    MESSAGE_HEADER header{};header.wMsg=gm2tl_error_str;
    const auto size=sizeof(GM2TL_ERROR_STR)+text.size()+1;
    auto* buffer=socket.state->allocator->Alloc(static_cast<INT32>(size));
    std::memcpy(buffer,&header,sizeof(header));
    std::memcpy(buffer+sizeof(GM2TL_ERROR_STR),text.data(),text.size());buffer[size-1]=0;
    CMessageHandler::GetInstance().OnRecv(&socket,buffer,static_cast<UINT32>(size));
}
std::size_t sent(UINT16 id)
{return std::ranges::count_if(current->sent,[&](const auto& value){return value.first==id;});}
void start(gmtool& window)
{
    current=std::make_shared<State>();
    check(CGMManager::GetInstance().Init() && window.Init(),"fixture production runtime/window starts");
    check(CGMManager::GetInstance().MainLoop() && current->net->sockets.size()==2,"fake transport binds two production sessions");
    for(auto& socket:current->net->sockets)
    {
        CMessageHandler::GetInstance().OnConnected(socket.get());
        auto reply=message<GM2TL_REGISTER_ACK>(gm2tl_register_ack);reply.nResult=errSuccess;deliver(*socket,reply);
    }
    check(CSessionMgr::GetInstance().GetGMSession(0)->eState==GM_SVR_STATE_REGISTERED,"actual registration reply publishes state");
}
void stop()
{
    const auto context=current;context->send_hook={};
    check(CGMManager::GetInstance().UnInit() && !context->allocator && !context->net,"native fixture resources retire");
    check(context->allocations==context->frees && !context->userdata_reads,"packets balanced and replies never read native userdata");
}
void credentials(loader& login,QString name=QStringLiteral("fixture"),QString password=QStringLiteral("password"))
{
    login.findChild<QLineEdit*>("userNameEdit")->setText(name);
    login.findChild<QLineEdit*>("userPwdEdit")->setText(password);
}
bool ready(loader& login){return login.findChild<QPushButton*>("loginButton")->isEnabled();}
void pending(loader& login){credentials(login);login.Login();check(!ready(login),"login owns a pending attempt before sending");}
}

gm::RuntimeHooks gm::runtime_hooks()
{
    const auto state=current;RuntimeHooks hooks;
    hooks.log_start=[] {return TRUE;};hooks.log_stop=[] {return TRUE;};
    hooks.load_config=[]
    {
        g_TLConfig={};g_TLConfig.ServerList.nSvrNum=2;
        for(int index=0;index<2;++index)
        {auto& server=g_TLConfig.ServerList.arrGMList[index];server.nSvrID=index+1;server.dwPort=3000+index;
         std::memcpy(server.szName,"fixture",8);std::memcpy(server.szAddr,"127.0.0.1",10);}
        return TRUE;
    };
    hooks.allocator_create=[state]()->IPacketAllocator*{return new Allocator(state);};
    hooks.allocator_destroy=[](IPacketAllocator* value){delete static_cast<Allocator*>(value);return TRUE;};
    hooks.net_create=[state](IPacketAllocator*,INetEventHandler*)->INet*{return new Net(state);};
    hooks.net_destroy=[](INet* value){delete static_cast<Net*>(value);return TRUE;};
    hooks.sessions_start=[](INet* net){return CSessionMgr::GetInstance().Init(net);};
    hooks.sessions_stop=[]{return CSessionMgr::GetInstance().UnInit();};hooks.sessions_poll=[]{return CSessionMgr::GetInstance().MainLoop();};
    hooks.handler_start=[](INet* net,IPacketAllocator* allocator,std::shared_ptr<void> owner){return CMessageHandler::GetInstance().Init(net,allocator,std::move(owner));};
    hooks.handler_stop=[]{return CMessageHandler::GetInstance().UnInit();};
    hooks.release_allocator=[](IPacketAllocator* allocator){CMessageHandler::GetInstance().ReleaseAllocator(allocator);};
    hooks.commands_start=[]{return CCmdManager::GetInstance().Init();};hooks.commands_stop=[]{return CCmdManager::GetInstance().UnInit();};
    hooks.commands_poll=[]{return CCmdManager::GetInstance().MainLoop();};
    hooks.data_start=[]{return CDataMgr::GetInstance().Init();};hooks.data_stop=[]{return CDataMgr::GetInstance().UnInit();};return hooks;
}

