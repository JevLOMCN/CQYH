#pragma once

#include "std_type.h"
#include "net.h"
#include "process_error.h"
#include <memory>

namespace guard_runtime
{
struct NetCloser
{
    INT32 type = netSimple;
    BOOL (*destroy)(INT32, INet*) = nullptr;
    void operator()(INet* net) const noexcept
    {
        try { LOG_CHECK_ERROR(net->UnInit()); }
        catch (...) { ERR("guard transport shutdown threw an exception"); }
        try { if (destroy) LOG_CHECK_ERROR(destroy(type, net)); }
        catch (...) { ERR("guard transport destruction threw an exception"); }
    }
};
struct AcceptorCloser
{
    INet* net = nullptr;
    void operator()(ISocketAcceptor* acceptor) const noexcept
    {
        try { LOG_CHECK_ERROR(acceptor->Close()); }
        catch (...) { ERR("guard acceptor close threw an exception"); }
        try { if (net) LOG_CHECK_ERROR(net->DestroyAcceptor(acceptor)); }
        catch (...) { ERR("guard acceptor destruction threw an exception"); }
    }
};
using NetOwner = std::unique_ptr<INet, NetCloser>;
using AcceptorOwner = std::unique_ptr<ISocketAcceptor, AcceptorCloser>;
}
