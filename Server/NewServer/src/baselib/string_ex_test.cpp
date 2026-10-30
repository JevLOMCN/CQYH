#include "stdafx.h"
#include "string_ex.h"

struct StringTestCase
{
    const char* src;
    const char* init_dst;
    const char* dst;
    size_t  size;
    size_t  ret;
};

StringTestCase g_CopyTestCase[] =
{
    {"12345678", "abcdefgh", "1234567", 8, 7},
    {"12345678", "abcdefgh", "123456", 7, 6},
    {"12345678", "abcdefgh", "12345", 6, 5},
    {"12345678", "abcdefgh", "1234", 5, 4},
    {"12345678", "abcdefgh", "123", 4, 3},
    {"12345678", "abcdefgh", "12", 3, 2},
    {"12345678", "abcdefgh", "1", 2, 1},
    {"12345678", "abcdefgh", "", 1, 0},
    {"12345678", "abcdefgh", "abcdefgh", 0, 0},
    {"12345678", "", "12345678", 9, 8},
    {"", "", "", 8, 0},
};

BOOL TestStringEx(void)
{
    for (int i = 0; i < sizeof(g_CopyTestCase) / sizeof(g_CopyTestCase[0]); i++)
    {
        char src[256];
        char dst[256];
        strcpy(src, g_CopyTestCase[i].src);
        strcpy(dst, g_CopyTestCase[i].init_dst);
        size_t ret = strxcpy(dst, src, g_CopyTestCase[i].size);
        if (ret != g_CopyTestCase[i].ret)
            return false;
        if (strcmp(dst, g_CopyTestCase[i].dst) != 0)
            return false;
    }

    return true;
}
