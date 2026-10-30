#include "stdafx.h"
#include "math_ex.h"
#include "cert_math.h"
#include "packet_allocator.h"
#include "cert_message_handler.h"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <format>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <system_error>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{
char serviceName[] = "certification server";
constexpr wchar_t serviceNameWide[] = L"certification server";
std::atomic_bool stopRequested{false};
std::atomic<DWORD> serviceError{NO_ERROR};
std::mutex statusMutex;
SERVICE_STATUS_HANDLE statusHandle = nullptr;
SERVICE_STATUS status{};

bool reportStatus(DWORD state, DWORD error = NO_ERROR)
{
    const std::lock_guard lock{statusMutex};
    if (!statusHandle) return false;
    if (state == SERVICE_STOP_PENDING && status.dwCurrentState == SERVICE_STOPPED) return false;
    if (stopRequested && (state == SERVICE_START_PENDING || state == SERVICE_RUNNING))
        state = SERVICE_STOP_PENDING;
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = state;
    status.dwControlsAccepted = state == SERVICE_RUNNING ? SERVICE_ACCEPT_STOP : 0;
    status.dwWin32ExitCode = error;
    status.dwServiceSpecificExitCode = 0;
    const bool pending = state == SERVICE_START_PENDING || state == SERVICE_STOP_PENDING;
    status.dwCheckPoint = pending ? status.dwCheckPoint + 1 : 0;
    status.dwWaitHint = pending ? 10'000 : 0;
    return SetServiceStatus(statusHandle, &status) != FALSE;
}

struct ServiceCloser
{
    void operator()(SC_HANDLE handle) const noexcept { CloseServiceHandle(handle); }
};
using ServiceOwner = std::unique_ptr<std::remove_pointer_t<SC_HANDLE>, ServiceCloser>;

std::filesystem::path executablePath()
{
    std::vector<wchar_t> buffer(256);
    for (;;)
    {
        const auto count = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (!count) throw std::system_error(GetLastError(), std::system_category(), "GetModuleFileNameW");
        if (count < buffer.size()) return std::filesystem::path{std::wstring(buffer.data(), count)};
        if (buffer.size() >= 32'768) throw std::length_error("cert executable path exceeds Windows limit");
        buffer.resize(buffer.size() * 2);
    }
}

template<class Work>
void cleanup(bool& success, const char* name, Work&& work) noexcept
{
    try
    {
        if (work()) return;
        ERR("cert cleanup failed: %s", name);
    }
    catch (const std::exception& error) { ERR("cert cleanup %s: %s", name, error.what()); }
    catch (...) { ERR("cert cleanup threw: %s", name); }
    success = false;
}

struct NetCloser
{
    bool* success;
    void operator()(INet* net) const noexcept
    {
        cleanup(*success, "network shutdown", [&] { return net->UnInit(); });
        cleanup(*success, "network destruction", [&] { return g_DestroyNetInterface(netSimple, net); });
    }
};
struct AcceptorCloser
{
    INet* net;
    bool* success;
    void operator()(ISocketAcceptor* acceptor) const noexcept
    {
        cleanup(*success, "acceptor close", [&] { return acceptor->Close(); });
        cleanup(*success, "acceptor destruction", [&] { return net->DestroyAcceptor(acceptor); });
    }
};

struct Runtime
{
    bool success = true;
    bool finished = false;
    bool logAttempted = false, allocatorAttempted = false, handlerAttempted = false;
    CPacketAllocator packets;
    std::unique_ptr<INet, NetCloser> net{nullptr, {&success}};
    std::unique_ptr<ISocketAcceptor, AcceptorCloser> acceptor{nullptr, {nullptr, &success}};
    ~Runtime() { shutdown(); }

    bool shutdown() noexcept
    {
        if (std::exchange(finished, true)) return success;
        g_MessageHandler.BeginShutdown();
        acceptor.reset();
        net.reset(); // Handler and packet pool outlive all network callbacks.
        if (handlerAttempted) cleanup(success, "message handler", [] { return g_MessageHandler.UnInit(); });
        if (allocatorAttempted) cleanup(success, "packet allocator", [&] { return packets.UnInit(); });
        if (logAttempted) cleanup(success, "logger", [] { return CLog::GetInstance().UnInit(); });
        return success;
    }
};

bool initialize(Runtime& runtime)
{
    runtime.logAttempted = true;
    if (!CLog::GetInstance().Init("cert_server", lltAll, lotAll, FALSE, FALSE, TRUE)) return false;
    cert::initialize_math_table(g_MathTable);
    runtime.allocatorAttempted = true;
    if (!runtime.packets.Init(1024, 256)) return false;
    SIMPLE_INIT_PARAM parameters{1, 1, 32, 1024 * 1024, 1024 * 1024, 4};
    runtime.net.reset(g_CreateNetInterface(netSimple, &runtime.packets, &g_MessageHandler, &parameters));
    if (!runtime.net) return false;
    runtime.handlerAttempted = true;
    if (!g_MessageHandler.Init(runtime.net.get(), &runtime.packets)) return false;
    runtime.acceptor = decltype(runtime.acceptor){runtime.net->CreateAcceptor(), {runtime.net.get(), &runtime.success}};
    if (!runtime.acceptor || !runtime.acceptor->Open(0, 8111)) return false;
    INF("opened CS listen port at 0.0.0.0:8111");
    INF("CS initialized successfully");
    return true;
}

bool serve()
{
    using namespace std::chrono_literals;
    Runtime runtime;
    bool success = false;
    try
    {
        if (initialize(runtime))
        {
            success = reportStatus(SERVICE_RUNNING);
            while (success && !stopRequested)
            {
                success = runtime.net->MainLoop() != FALSE;
                if (success) std::this_thread::sleep_for(1ms);
            }
        }
    }
    catch (const std::exception& error) { success = false; ERR("cert runtime failed: %s", error.what()); }
    catch (...) { success = false; ERR("cert runtime threw an exception"); }
    return runtime.shutdown() && success;
}
}

BOOL Install()
{
    const auto binary = std::format(L"\"{}\"", executablePath().wstring());
    const ServiceOwner scm{OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE)};
    if (!scm) return FALSE;
    const ServiceOwner service{CreateServiceW(scm.get(), serviceNameWide, serviceNameWide,
        SERVICE_QUERY_STATUS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
        binary.c_str(), nullptr, nullptr, L"", nullptr, nullptr)};
    return service != nullptr;
}

BOOL Uninstall()
{
    const ServiceOwner scm{OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT)};
    if (!scm) return FALSE;
    const ServiceOwner service{OpenServiceW(scm.get(), serviceNameWide, SERVICE_STOP | DELETE)};
    if (!service) return FALSE;
    SERVICE_STATUS result{};
    if (!ControlService(service.get(), SERVICE_CONTROL_STOP, &result) &&
        GetLastError() != ERROR_SERVICE_NOT_ACTIVE) return FALSE;
    return DeleteService(service.get());
}

void WINAPI ServiceCtrlHandler(DWORD control)
{
    if (control == SERVICE_CONTROL_STOP)
    {
        stopRequested = true;
        reportStatus(SERVICE_STOP_PENDING);
    }
    else if (control == SERVICE_CONTROL_INTERROGATE)
    {
        const std::lock_guard lock{statusMutex};
        if (statusHandle) SetServiceStatus(statusHandle, &status);
    }
}

void WINAPI ServiceMain(DWORD, LPSTR*)
{
    const auto handle = RegisterServiceCtrlHandlerA(serviceName, ServiceCtrlHandler);
    if (!handle)
    {
        serviceError = ERROR_PROCESS_ABORTED;
        return;
    }
    {
        const std::lock_guard lock{statusMutex};
        statusHandle = handle;
    }
    const bool success = reportStatus(SERVICE_START_PENDING) && serve();
    const DWORD error = success ? NO_ERROR : ERROR_PROCESS_ABORTED;
    serviceError = error;
    if (!reportStatus(SERVICE_STOPPED, error)) serviceError = ERROR_PROCESS_ABORTED;
    const std::lock_guard lock{statusMutex};
    statusHandle = nullptr;
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR command, int)
{
    stopRequested = false;
    serviceError = NO_ERROR;
    {
        const std::lock_guard lock{statusMutex};
        statusHandle = nullptr;
        status = {};
        status.dwCurrentState = SERVICE_STOPPED;
    }
    try
    {
        if (_stricmp(command, "/install") == 0) return Install() ? 0 : 1;
        if (_stricmp(command, "/uninstall") == 0) return Uninstall() ? 0 : 1;
        SERVICE_TABLE_ENTRYA table[]{{serviceName, ServiceMain}, {nullptr, nullptr}};
        return StartServiceCtrlDispatcherA(table) && serviceError == NO_ERROR ? 0 : 1;
    }
    catch (const std::exception& error) { ERR("cert entry failed: %s", error.what()); }
    catch (...) { ERR("cert entry threw an exception"); }
    return 1;
}
