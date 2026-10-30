#include <Windows.h>
#include "std_type.h"
#include <cstdio>
#include "b2c_message.h"
#include "c2g_message.h"
#include "c2m_message.h"
#include "c2t_message.h"
#include "cer_message.h"
#include "common_message.h"
#include "d2g_message.h"
#include "d2m_message.h"
#include "d2t_message.h"
#include "external_message_header.h"
#include "g2c_message.h"
#include "g2d_message.h"
#include "g2m_message.h"
#include "g2t_message.h"
#include "gmsvr_message.h"
#include "gmtl_message.h"
#include "internal_message_header.h"
#include "l2m_message.h"
#include "l2t_message.h"
#include "m2c_message.h"
#include "m2d_message.h"
#include "m2g_message.h"
#include "m2l_message.h"
#include "m2t_message.h"
#include "t2c_message.h"
#include "t2d_message.h"
#include "t2g_message.h"
#include "t2l_message.h"
#include "t2m_message.h"

int main()
{
    std::size_t checked = 0;
#define CHECK_LAYOUT(type, bytes, alignment) \
    static_assert(sizeof(type) == bytes, #type " wire size changed"); \
    static_assert(alignof(type) == alignment, #type " wire alignment changed"); \
    ++checked;
#include "windows_x64_layouts.inc"
#undef CHECK_LAYOUT
    std::printf("%zu packet/helper layout contracts passed\n", checked);
}
