#ifndef CLIENT_LUA_ARRAY_INDEX_H
#define CLIENT_LUA_ARRAY_INDEX_H

#include <stddef.h>
#include <limits.h>

namespace ClientLuaArray
{
    template<typename T, size_t Size>
    bool Index(double value, const T (&)[Size], int& index)
    {
        // Check before conversion: NaN, infinity and large Lua numbers cannot
        // safely be converted to an integer index.
        if (!(value >= 0 && value < Size && value <= INT_MAX))
            return false;
        index = static_cast<int>(value);
        return true;
    }
}

#endif
