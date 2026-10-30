#include "stdafx.h"
#include "maintaintool.h"
#include <QtWidgets/QApplication>
#include "packet_allocator.h"
#include "message_handler.h"
#include "session_mgr.h"
#include "transmit_file_mgr.h"
#include "global_config.h"
#include "mt_config.h"
#include <cstdlib>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>

INet* g_pNet = nullptr; // Borrowed by the GUI only while Runtime owns the transport.

namespace
{
void cleanup_error(const char* name) noexcept
{
    try { ERR("maintaintool %s cleanup failed", name); } catch (...) {}
}
class Runtime
{
public:
    explicit Runtime(MaintainTool& window) : m_window{window}, m_net{nullptr, NetDeleter{&m_cleanupGood}} {}
    ~Runtime() { Stop(); }
    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;

    bool Start()
    {
        m_log = true;
        if (!CLog::GetInstance().Init("Maintaintool", g_MTConfig.nLogLevelMask, g_MTConfig.nLogOutputMask,
            g_MTConfig.bEnableLogThread, g_MTConfig.bPrintThreadID, g_MTConfig.bFlushFile)) return false;
        CLog::GetInstance().SetCustomOutput(&MaintainTool::LogOutput);
        INF("initializing maintain tool log");
        m_allocatorStarted = true;
        if (!m_allocator.Init(g_MTConfig.nSmallPacketCount, g_MTConfig.nLargePacketCount)) return false;
        SIMPLE_INIT_PARAM options{};
        options.nMaxAcceptor = g_MTConfig.nMaxAcceptor;
        options.nMaxConnector = g_MTConfig.nMaxConnector;
        options.nMaxSessionCount = g_MTConfig.nMaxSessionCount;
        options.nSendBufSize = g_MTConfig.nSendBufSize;
        options.nRecvBufSize = g_MTConfig.nRecvBufSize;
        options.nPacketBufferSize = g_MTConfig.nPacketBufferSize;
        m_net.reset(g_CreateNetInterface(netSimple, &m_allocator, &CMTMessageHandler::GetInstance(), &options));
        if (!m_net) return false;
        g_pNet = m_net.get();
        if (!m_window.Init()) return false;
        m_handler = true;
        if (!CMTMessageHandler::GetInstance().Init(m_net.get(), &m_allocator)) return false;
        m_sessions = true;
        if (!CSessionMgr::GetInstance().Init(m_net.get())) return false;
        m_transfers = true;
        if (!CTransmitFileMgr::GetInstance().Reload()) return false;
        return m_window.StartPolling() != FALSE;
    }
    bool Stop() noexcept
    {
        if (std::exchange(m_stopped, true)) return m_cleanupGood;
        // Retain the handler and allocator until transport callbacks have drained.
        CMTMessageHandler::GetInstance().BeginShutdown();
        cleanup("window", [&] { return m_window.UnInit(); });
        if (m_transfers) cleanup("transfers", [] { return CTransmitFileMgr::GetInstance().UnInit(); });
        if (m_sessions) cleanup("sessions", [] { return CSessionMgr::GetInstance().UnInit(); });
        g_pNet = nullptr;
        m_net.reset();
        if (m_handler) cleanup("handler", [] { return CMTMessageHandler::GetInstance().UnInit(); });
        if (m_allocatorStarted) cleanup("allocator", [&] { return m_allocator.UnInit(); });
        if (m_log)
        {
            CLog::GetInstance().SetCustomOutput(nullptr);
            cleanup("logger", [] { return CLog::GetInstance().UnInit(); });
        }
        return m_cleanupGood;
    }
private:
    template<class Action> void cleanup(const char* name, Action action) noexcept
    {
        try { if (action()) return; }
        catch (...) {}
        m_cleanupGood = false;
        cleanup_error(name);
    }
    struct NetDeleter
    {
        bool* good;
        void operator()(INet* net) const noexcept
        {
            try { if (g_DestroyNetInterface(netSimple, net)) return; }
            catch (...) {}
            *good = false;
            cleanup_error("transport");
        }
    };
    MaintainTool& m_window;
    bool m_cleanupGood = true, m_stopped = false, m_log = false, m_allocatorStarted = false;
    bool m_handler = false, m_sessions = false, m_transfers = false;
    CPacketAllocator m_allocator;
    std::unique_ptr<INet, NetDeleter> m_net;
};
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        QApplication application(__argc, __argv);
        MaintainTool window;
        Runtime runtime{window};
        if (!LoadMTConfig("maintaintool_config.lua") || !runtime.Start())
        {
            ERR("maintaintool startup failed");
            runtime.Stop();
            return EXIT_FAILURE;
        }
        window.show();
        window.Login();
        const auto result = application.exec();
        return runtime.Stop() ? result : EXIT_FAILURE;
    }
    catch (const std::exception& error) { ERR("maintaintool failed: %s", error.what()); }
    catch (...) { ERR("maintaintool failed with an unknown exception"); }
    return EXIT_FAILURE;
}
