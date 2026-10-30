#include "stdafx.h"
#include "gm_application.h"
#include "gmtool.h"
#include "loader.h"
#include <QtCore/QPointer>
#include <QtCore/QThread>
#include <QtWidgets/QApplication>
#include <stdexcept>

namespace
{
struct Views
{
    QPointer<gmtool> window;
    QPointer<loader> login;
    bool bound = false;
};

Views& views()
{
    static Views value;
    return value;
}
}

gm::ApplicationViews::ApplicationViews(gmtool& window, loader& login)
{
    if (!qApp || QThread::currentThread() != qApp->thread() ||
        window.thread() != qApp->thread() || login.thread() != qApp->thread())
        throw std::logic_error("GM application views require the application thread");
    auto& state = views();
    if (state.bound)
        throw std::logic_error("GM application views are already bound");
    state.window = &window;
    state.login = &login;
    state.bound = true;
}

gm::ApplicationViews::~ApplicationViews()
{
    auto& state = views();
    state.window.clear();
    state.login.clear();
    state.bound = false;
}

gmtool* gm::current_main_window() noexcept { return views().window.data(); }
loader* gm::current_login_dialog() noexcept { return views().login.data(); }

gmtool& gm::main_window()
{
    if (auto* window = current_main_window()) return *window;
    throw std::logic_error("GM main window is unavailable");
}

loader& gm::login_dialog()
{
    if (auto* login = current_login_dialog()) return *login;
    throw std::logic_error("GM login dialog is unavailable");
}

void gm::detach_main_window(gmtool* window) noexcept
{
    if (current_main_window() == window) views().window.clear();
}

void gm::detach_login_dialog(loader* login) noexcept
{
    if (current_login_dialog() == login) views().login.clear();
}
