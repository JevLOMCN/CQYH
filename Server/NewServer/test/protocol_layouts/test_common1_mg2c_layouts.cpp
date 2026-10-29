#include <Windows.h>
#include "std_type.h"
#include "mg2c_message.h"
static_assert(ExternalMessage::m2c_message_begin == 900);
static_assert(ExternalMessage::m2c_message_end == 1000);
static_assert(sizeof(ExternalMessage::MG2C_MESSAGE_ID) == 4);
static_assert(alignof(ExternalMessage::MG2C_MESSAGE_ID) == 4);
