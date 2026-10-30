#pragma once

class gmtool;
class loader;

namespace gm
{
// The entry point owns the widgets. Callbacks only borrow the current views.
class ApplicationViews final
{
public:
    ApplicationViews(gmtool& window, loader& login);
    ~ApplicationViews();
    ApplicationViews(const ApplicationViews&) = delete;
    ApplicationViews& operator=(const ApplicationViews&) = delete;
};

gmtool* current_main_window() noexcept;
loader* current_login_dialog() noexcept;
gmtool& main_window();
loader& login_dialog();
void detach_main_window(gmtool* window) noexcept;
void detach_login_dialog(loader* login) noexcept;
}
