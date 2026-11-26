#ifndef CLIENT_FIXED_STRING_H
#define CLIENT_FIXED_STRING_H

#include <cstddef>
#include <cstring>

namespace ClientFixedString
{
    template<size_t Size>
    size_t Length(const char (&value)[Size])
    {
        size_t length = 0;
        while (length < Size && value[length])
            ++length;
        return length;
    }

    template<size_t Size>
    void Assign(char (&output)[Size], const char* input)
    {
        size_t length = 0;
        if (input)
            while (length < Size - 1 && input[length])
                ++length;
        if (length)
            std::memmove(output, input, length);
        std::memset(output + length, 0, Size - length);
    }
}

#endif
