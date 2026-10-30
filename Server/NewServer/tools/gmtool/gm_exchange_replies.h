#pragma once
#include <string_view>
class CLDWidget;
namespace gm
{
// The main window supplies these boundaries; reply handling owns all decoded data.
CLDWidget* exchange_reply_view();
void exchange_reply_log(std::string_view text);
}
