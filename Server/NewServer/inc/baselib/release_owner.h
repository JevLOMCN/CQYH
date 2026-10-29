#ifndef CQYH_BASELIB_RELEASE_OWNER_H
#define CQYH_BASELIB_RELEASE_OWNER_H

#include <cstdio>
#include <memory>

namespace cqyh
{
// SDK interfaces destroy themselves through Release(), not a public destructor.
struct ReleaseDeleter
{
    template<class T>
    void operator()(T* value) const noexcept
    {
        try { value->Release(); }
        catch (...) { std::fputs("SDK Release failed\n", stderr); }
    }
};
template<class T>
using ReleasePtr = std::unique_ptr<T, ReleaseDeleter>;

template<class T>
bool StopAndRelease(ReleasePtr<T>& value) noexcept
{
    if (!value)
        return true;
    bool stopped = true;
    try { value->Uninitialize(); }
    catch (...)
    {
        std::fputs("SDK Uninitialize failed\n", stderr);
        stopped = false;
    }
    value.reset();
    return stopped;
}
}
#endif // CQYH_BASELIB_RELEASE_OWNER_H
