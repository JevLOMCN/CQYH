#include "stdafx.h"
#include "gm_application.h"
#include "gmtool.h"
#include "loader.h"
#include "gm_manager.h"
#include "session_mgr.h"
#include "ld_widget.h"
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTreeWidget>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

INT32 g_State = 0;

namespace
{
int checks = 0;
void check(bool condition, const char* message)
{
    ++checks;
    if (!condition) throw std::runtime_error(message);
}
}

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2) throw std::runtime_error("Expected private working directory");
        const auto previous = std::filesystem::current_path();
        const auto directory = std::filesystem::absolute(argv[1]);
        std::filesystem::create_directories(directory / "config");
        struct Restore
        {
            std::filesystem::path directory;
            ~Restore() { std::error_code error; std::filesystem::current_path(directory, error); }
        } restore{previous};
        std::filesystem::current_path(directory);
        std::ofstream configuration("config/gmtool_config.lua", std::ios::binary);
        configuration << R"(function LoadFile()
 local c=GetTLConfig()
 c.ServerList.nSvrNum=1
 local s=c.ServerList.arrGMList[0]
 s.nSvrID=7
 s.szName='idle fixture'
 s.szAddr='127.0.0.1'
 s.dwPort=39999
end
)";
        configuration.close();
        if (!configuration) throw std::runtime_error("Cannot write private configuration");

        check(!gm::current_main_window() && !gm::current_login_dialog(), "no widgets exist before QApplication");
        QApplication application(argc, argv);
        application.setQuitOnLastWindowClosed(false);
        auto& manager = CGMManager::GetInstance();
        struct Stop { ~Stop() { CGMManager::GetInstance().UnInit(); } } stop;
        for (int pass = 0; pass < 3; ++pass)
        {
            gmtool window;
            loader login;
            check(!gm::current_main_window(), "unbound window does not publish itself during construction");
            {
                gm::ApplicationViews views(window, login);
                check(gm::current_main_window() == &window && gm::current_login_dialog() == &login,
                    "entry-point scope publishes the actual widgets");
                bool duplicate_rejected = false;
                try { gm::ApplicationViews duplicate(window, login); }
                catch (const std::logic_error&) { duplicate_rejected = true; }
                check(duplicate_rejected && gm::current_main_window() == &window, "nested binding cannot replace current widgets");
                check(manager.Init() && window.Init(), "actual native runtime and main-window initialization");
                auto* tree = window.findChild<QTreeWidget*>("treeWidget_SvrList");
                check(tree && tree->topLevelItemCount() == 1 && tree->topLevelItem(0)->text(0) == QStringLiteral("idle fixture"),
                    "configured native session is shown in the actual main window");
                tree->topLevelItem(0)->setCheckState(0, Qt::Checked);
                check(window.IsServerSelected(0), "published session item is selectable");
                check(window.Init() && tree->topLevelItemCount() == 1 && !window.IsServerSelected(0),
                    "reinitialization replaces tree items and resets the borrowed cache");
                check(window.SetLDMode() && window.GetLDWidget(), "exchange view belongs to the actual tab widget");
                const auto* exchange = window.GetLDWidget();
                check(window.SetLDMode() && window.GetLDWidget() == exchange,
                    "repeated mode selection reuses the owned exchange view");
                delete window.GetLDWidget();
                check(!window.GetLDWidget(), "Qt child destruction clears the borrowed exchange pointer");
                check(window.SetLDMode() && window.GetLDWidget(), "exchange view can be recreated after destruction");
                const auto* session = CSessionMgr::GetInstance().GetGMSession(0);
                check(session && !session->pSocketSession && session->nConnectState == SESSION_DISCONNECTED,
                    "UI construction and initialization issue no connection");
                check(manager.UnInit() && !manager.IsRetiring(), "runtime retires before entering the cancellation loop");
                QTimer::singleShot(0, &login, &QDialog::reject);
                check(login.exec() == QDialog::Rejected && !manager.IsRunning(),
                    "actual login cancellation exits without starting any peer operation");
            }
            check(!gm::current_main_window() && !gm::current_login_dialog(), "binding clears before widget destruction");
            application.processEvents();
        }
        {
            auto window = std::make_unique<gmtool>();
            auto login = std::make_unique<loader>();
            gm::ApplicationViews views(*window, *login);
            check(manager.Init(), "runtime can restart for destruction checks");
            login.reset();
            check(!gm::current_login_dialog(), "destroyed login cannot be borrowed by later callbacks");
            window.reset();
            check(!gm::current_main_window() && !manager.IsRunning() && !manager.IsRetiring(),
                "direct main-window destruction detaches callbacks and retires the runtime");
        }
        bool absent_rejected = false;
        try { (void)gm::main_window(); }
        catch (const std::logic_error&) { absent_rejected = true; }
        check(absent_rejected, "reference accessor rejects absent main window");
        std::cout << checks << " GM application checks passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        CGMManager::GetInstance().UnInit();
        std::cerr << error.what() << '\n';
        return 1;
    }
}
