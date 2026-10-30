#include <Windows.h>
#include "std_type.h"
#include <cstdio>
#include "b2c_message.h"
#include "c2g_message.h"
#include "c2m_message.h"
#include "c2mg_messsage.h"
#include "c2t_message.h"
#include "external_message_header.h"
#include "g2c_message.h"
#include "m2c_message.h"
#include "t2c_message.h"

int main()
{
    std::size_t checked = 0;
#define CHECK_LAYOUT(type, bytes, alignment) \
    static_assert(sizeof(type) == bytes, #type " Common1 wire size changed"); \
    static_assert(alignof(type) == alignment, #type " Common1 wire alignment changed"); \
    ++checked;
#include "common1_windows_x64_layouts.inc"
#undef CHECK_LAYOUT
    std::printf("%zu Common1 packet/helper layout contracts passed\n", checked);
}
