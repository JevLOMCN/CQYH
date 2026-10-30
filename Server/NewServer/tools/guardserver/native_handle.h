#ifndef CQYH_GUARD_NATIVE_HANDLE_H
#define CQYH_GUARD_NATIVE_HANDLE_H

#include <memory>
#include <windows.h>

namespace guard
{
struct CloseNativeHandle
{
    void operator()(HANDLE value) const noexcept
    {
        if (value && value != INVALID_HANDLE_VALUE)
            CloseHandle(value);
    }
};
using NativeHandle = std::unique_ptr<void, CloseNativeHandle>;
}

#endif
