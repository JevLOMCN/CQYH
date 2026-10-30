// The real guard, message handler, command dispatcher and process registry run
// against in-memory sockets. This fixture never initializes the guard's native
// transport, starts a service/process, deploys an archive or touches a live server.
#include "stdafx.h"
#include "guardserver.h"
#include "guard_config.h"
#include "gd_session_mgr.h"
#include "process_mgr.h"
#include "native_handle.h"
#include "guard_deploy.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <thread>
#include <functional>
#include <utility>
#include "guard_packets.h"

BOOL g_bStartFromCmd = FALSE;

namespace
{
namespace fs = std::filesystem;
using namespace InternalMessage;
int checks = 0;
int failures = 0;
void check(bool value, const char* description)
{
    ++checks;
    if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", description); }
}

struct Fixture
{
    fs::path original = fs::current_path();
    fs::path base;
    fs::path root;
    explicit Fixture(const fs::path& parent)
    {
        fs::create_directories(parent);
        base = fs::canonical(parent);
        root = base / ("guard-results-" + std::to_string(GetCurrentProcessId()) + "-" +
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        if (!fs::create_directory(root)) throw std::runtime_error("fixture must be new");
        fs::current_path(root);
    }
    ~Fixture()
    {
        std::error_code error;
        fs::current_path(original, error);
        if (root.parent_path() == base && root.filename().string().starts_with("guard-results-") &&
            fs::canonical(root, error) == root && !error)
            fs::remove_all(root, error);
    }
};

void write(const fs::path& file, std::string_view value)
{
    fs::create_directories(file.parent_path());
    std::ofstream stream(file, std::ios::binary);
    stream.write(value.data(), static_cast<std::streamsize>(value.size()));
    if (!stream) throw std::runtime_error("fixture write failed");
}

class Allocator : public IPacketAllocator
{
public:
    std::map<PACKET_BUFFER*, std::unique_ptr<char[]>> packets;
    bool fail_next = false;
    PACKET_BUFFER* Alloc(INT32 size) override
    {
        if (std::exchange(fail_next, false) || size <= 0) return nullptr;
        auto buffer = std::make_unique<char[]>(size);
        const auto address = buffer.get();
        packets.emplace(address, std::move(buffer));
        return address;
    }
    void Free(PACKET_BUFFER* packet) override { check(packets.erase(packet) == 1, "packet ownership released once"); }
    BOOL IsAllocated(PACKET_BUFFER* packet) override { return packets.contains(packet); }
    UINT32 PrintMemoryStat() override { return 0; }
    void CheckError() override {}
};

class Net : public INet
{
public:
    BOOL Init(IPacketAllocator*, INetEventHandler*, void*) override { return TRUE; }
    BOOL UnInit() override { return TRUE; }
    ISocketAcceptor* CreateAcceptor() override { return nullptr; }
    BOOL DestroyAcceptor(ISocketAcceptor*) override { return TRUE; }
    ISocketSession* Connect(UINT32, UINT16) override { return nullptr; }
    BOOL MainLoop() override { return TRUE; }
    INT64 PrintMemoryStat() override { return 0; }
};

struct Ack { UINT16 message; INT32 error; std::string info; };
struct FileAck { INT32 error; std::string name; UINT16 index, total, file_index, file_total; };
class Session : public ISocketSession
{
public:
    Allocator& allocator;
    std::vector<Ack> acknowledgements;
    std::vector<FileAck> file_acknowledgements;
    std::vector<std::string> commands;
    std::vector<Ack> replies;
    std::vector<std::string> notifications;
    std::function<void()> on_close;
    std::function<void(UINT16)> on_send;
    int summaries = 0;
    int closes = 0;
    bool fail_send = false;
    bool throw_send = false;
    bool fail_close = false;
    void* data = nullptr;
    explicit Session(Allocator& value) : allocator(value) {}
    INT32 Send(PACKET_BUFFER* packet, INT32 size, INT32 offset) override
    {
        if (throw_send) throw std::runtime_error("fixture send exception");
        if (fail_send) return -1;
        const auto* header = reinterpret_cast<const MESSAGE_HEADER*>(packet + offset);
        if (header->wMsg == u2n_deploy_server_version_ack)
        {
            const auto* ack = reinterpret_cast<const U2N_DEPLOY_SERVER_VERSION_ACK*>(header);
            acknowledgements.push_back({header->wMsg, ack->nErrorCode, ack->szInfo});
        }
        else if (header->wMsg == u2n_hot_update_patch_ack)
        {
            const auto* ack = reinterpret_cast<const U2N_HOT_UPDATE_PATCH_ACK*>(header);
            acknowledgements.push_back({header->wMsg, ack->nErrorCode, ack->szInfo});
        }
        else if (header->wMsg == u2n_send_files_ack)
        {
            const auto* ack = reinterpret_cast<const U2N_SEND_FILES_ACK*>(header);
            file_acknowledgements.push_back({ack->nErrorCode, ack->szFileName, ack->wBufferIndex,
                ack->wBufferTotal, ack->wFileIndex, ack->wFileTotal});
        }
        else if (header->wMsg == u2n_exec_cmd_ack)
        {
            const auto* ack = reinterpret_cast<const U2N_UPDATE_CMD_ACK*>(header);
            replies.push_back({header->wMsg, ack->nErrorCode, ack->szResult});
        }
        else if (header->wMsg == u2n_regrister_ack)
        {
            check(size == sizeof(U2N_REGRISTER_ACK), "registration response keeps fixed wire size");
            const auto* ack = reinterpret_cast<const U2N_REGRISTER_ACK*>(header);
            replies.push_back({header->wMsg, ack->nErrorCode, ack->szVersion});
        }
        else if (header->wMsg == u2n_error)
        {
            check(size == sizeof(U2N_ERROR), "error response keeps fixed wire size");
            const auto* error = reinterpret_cast<const U2N_ERROR*>(header);
            replies.push_back({header->wMsg, error->nErrorCode, error->szErrorInfo});
        }
        else if (header->wMsg == u2n_notify)
            notifications.emplace_back(reinterpret_cast<const U2N_NOTIFY*>(header)->szContent);
        else if (header->wMsg == u2n_sync_processes_summary) ++summaries;
        else if (header->wMsg == u2m_command)
            commands.emplace_back(reinterpret_cast<const U2M_COMMAND*>(header)->szCommand);
        if (on_send) { auto callback = std::exchange(on_send, {}); callback(header->wMsg); }
        allocator.Free(packet); // The native network consumes packets on successful Send.
        return size;
    }
    INT32 SendNoPackageHead(PACKET_BUFFER*, INT32) override { return -1; }
    void* GetUserData() override { return data; }
    void SetUserData(void* value) override { data = value; }
    BOOL GetAddress(UINT32&, UINT16&) override { return FALSE; }
    BOOL GetRemoteAddress(UINT32&, UINT16&) override { return FALSE; }
    BOOL Close() override { ++closes; if (on_close) on_close(); return !fail_close; }
};

struct Runtime
{
    Allocator allocator;
    Net net;
    Session mt{allocator};
    Session ms{allocator};
    Runtime()
    {
        g_GuardConfig = {};
        check(CGDSessionMgr::GetInstance().Init(&net), "initialize session records against in-memory transport");
        check(CGDMessageHandler::GetInstance().Init(&net, &allocator), "initialize real guard handler without a native network");
        CGDSessionMgr::GetInstance().GetMTSession().pSocketSession = &mt;
    }
    ~Runtime()
    {
        CProcessMgr::GetInstance().UnInit(); // Metadata only; never terminates registered processes.
        CGDMessageHandler::GetInstance().UnInit();
        CGDSessionMgr::GetInstance().UnInit();
        check(allocator.packets.empty(), "all response packets consumed or freed");
    }
};

void incoming(Runtime& runtime)
{
    auto& handler = CGDMessageHandler::GetInstance();
    Session socket{runtime.allocator};
    handler.OnRecv(&socket, nullptr, 0);
    check(socket.closes == 1, "empty receive closes without reading a missing header");
    for (UINT32 size = 0; size < sizeof(MESSAGE_HEADER); ++size)
    {
        auto* packet = runtime.allocator.Alloc(static_cast<INT32>((std::max)(size, UINT32{1})));
        handler.OnRecv(&socket, packet, size);
        check(runtime.allocator.packets.empty(), "short receive releases its packet exactly once");
    }
    for (const UINT16 id : {UINT16{0}, static_cast<UINT16>(m2a_end), UINT16{65535}})
    {
        auto* packet = runtime.allocator.Alloc(sizeof(MESSAGE_HEADER));
        MESSAGE_HEADER header{};
        header.wMsg = id;
        std::memcpy(packet, &header, sizeof(header));
        const auto before = socket.closes;
        handler.OnRecv(&socket, packet, sizeof(header));
        check(socket.closes == before + 1 && runtime.allocator.packets.empty(),
            "unregistered and out-of-range message IDs release ownership and close");
    }
    auto* packet = runtime.allocator.Alloc(sizeof(MESSAGE_HEADER));
    handler.OnRecv(nullptr, packet, sizeof(MESSAGE_HEADER));
    check(runtime.allocator.packets.empty(), "a missing socket does not leak the received packet");
    auto& record = CGDSessionMgr::GetInstance().GetMTSession();
    runtime.mt.SetUserData(&record);
    record.qwTimer = 0;
    packet = runtime.allocator.Alloc(sizeof(MESSAGE_HEADER));
    MESSAGE_HEADER ping{};
    ping.wMsg = a2a_ping;
    std::memcpy(packet, &ping, sizeof(ping));
    handler.OnRecv(&runtime.mt, packet, sizeof(ping));
    check(record.qwTimer != 0 && runtime.mt.closes == 0 && runtime.allocator.packets.empty(),
        "registered ping dispatch refreshes its session and releases packet ownership");
    handler.BeginShutdown();
    record.qwTimer = 0;
    packet = runtime.allocator.Alloc(sizeof(ping));
    std::memcpy(packet, &ping, sizeof(ping));
    handler.OnRecv(&runtime.mt, packet, sizeof(ping));
    check(record.qwTimer == 0 && runtime.mt.closes == 1 && runtime.allocator.packets.empty(),
        "shutdown rejects business callbacks while retaining the receive allocator");
    check(handler.UnInit() && handler.Init(&runtime.net, &runtime.allocator),
        "message handling can initialize again after transport cleanup");
}

#include "test_guard_payloads.inl"

void upload(Runtime& runtime, const fs::path& root)
{
    auto& handler = CGDMessageHandler::GetInstance();
    auto& acks = runtime.mt.file_acknowledgements;
    const auto send = [&](std::string_view name, int type, UINT16 index, UINT16 total,
        std::string_view bytes, ISocketSession* owner = nullptr) {
        auto* packet = runtime.allocator.Alloc(sizeof(N2U_SEND_FILES));
        auto& message = *reinterpret_cast<N2U_SEND_FILES*>(packet);
        message.wMsg = n2u_send_files;
        message.nFileType = type;
        std::memcpy(message.szFileName, name.data(), (std::min)(name.size(), sizeof(message.szFileName)));
        message.wFileIndex = 0; message.wFileTotal = 1;
        message.wBufferIndex = index; message.wBufferTotal = total;
        message.wBufferSize = static_cast<UINT16>(bytes.size());
        std::memcpy(message.szBuffer, bytes.data(), (std::min)(bytes.size(), sizeof(message.szBuffer)));
        handler.OnRecv(owner ? owner : &runtime.mt, packet, sizeof(message));
        check(runtime.allocator.packets.empty(), "upload receive and acknowledgement release their packets");
    };
    const auto read = [](const fs::path& file) {
        std::ifstream stream(file, std::ios::binary);
        return std::string{std::istreambuf_iterator<char>{stream}, {}};
    };
    const auto versions = fs::path(g_GuardConfig.szServerVersionPath);
    const auto run = fs::path(g_GuardConfig.szServerRunPath);
    write(root / CURRENT_VERSION_FILE_NAME, "release.zip\r\n");
    send("area_config.lua", etftConfig, 0, 2, "first");
    check(acks.empty() && !fs::exists(versions / "area_config.lua"), "first config chunk remains private and unacknowledged");
    send("area_config.lua", etftConfig, 1, 2, "second");
    check(acks.size() == 1 && acks.back().error == errSuccess && acks.back().index == 1 && acks.back().total == 2 &&
        acks.back().name == "area_config.lua", "multi-chunk config acknowledges only the completed final chunk");
    check(read(versions / "release.zip-area_config.lua") == "firstsecond" &&
        read(run / "globalconfig" / "area_config.lua") == "firstsecond",
        "multi-chunk config retains its root and completes both synchronous copies");
    {
        guard::NativeHandle locked{CreateFileW((run / "globalconfig" / "area_config.lua").c_str(),
            GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr)};
        check(locked && locked.get() != INVALID_HANDLE_VALUE, "lock installed server configuration");
        send("area_config.lua", etftConfig, 0, 1, "rejected");
        check(acks.back().error != errSuccess && read(versions / "release.zip-area_config.lua") == "firstsecond" &&
            read(run / "globalconfig" / "area_config.lua") == "firstsecond",
            "failed runtime copy restores the version-prefixed configuration too");
    }
    send("archive.zip", etftVersion, 0, 1, "archive");
    check(acks.back().error == errSuccess && read(versions / "archive.zip") == "archive", "version uploads preserve archive bytes");
    std::string binary(32768, '\0');
    for (std::size_t index = 0; index < binary.size(); ++index) binary[index] = static_cast<char>(index % 256);
    send("full-block.zip", etftVersion, 0, 1, binary);
    check(acks.back().error == errSuccess && read(versions / "full-block.zip") == binary,
        "full-sized final wire payloads preserve binary bytes exactly");
    send("hot.pak", etftPatch, 0, 1, "patch");
    check(acks.back().error == errSuccess && read(fs::path(g_GuardConfig.szServerPatchPath) / "hot.pak") == "patch",
        "patch uploads use their separate root");
    for (const auto name : {std::string("../escape"), std::string("CON"), std::string(128, 'x')})
    {
        send(name, etftVersion, 0, 1, "bad");
        check(acks.back().error != errSuccess, "invalid or unterminated wire filenames fail without publishing files");
    }
    send("bad.bin", etftVersion, 0, 1, std::string(32769, 'x'));
    check(acks.back().error != errSuccess && !fs::exists(versions / "bad.bin"), "wire payload length cannot overrun its buffer");
    auto* shortPacket = runtime.allocator.Alloc(sizeof(MESSAGE_HEADER));
    reinterpret_cast<MESSAGE_HEADER*>(shortPacket)->wMsg = n2u_send_files;
    handler.OnRecv(&runtime.mt, shortPacket, sizeof(MESSAGE_HEADER));
    check(acks.back().error != errSuccess && acks.back().name.empty() && runtime.allocator.packets.empty(),
        "truncated upload records produce bounded failure acknowledgements");
    send("broken.bin", etftVersion, 0, 2, "partial");
    handler.OnDisconnected(&runtime.mt, FALSE);
    // The disconnect detaches MT. Restore its fixture slot for the next request.
    auto& mt = CGDSessionMgr::GetInstance().GetMTSession();
    mt.pSocketSession = &runtime.mt; runtime.mt.SetUserData(&mt);
    send("broken.bin", etftVersion, 1, 2, "tail");
    check(acks.back().error != errSuccess && !fs::exists(versions / "broken.bin"), "disconnect removes the incomplete upload state");
    write(root / "guard_config.lua", "function LoadFile(c,n) dofile('guardconfig/guard_area_config.lua'); "
        "c.dwPort=FixturePort; c.szServerRunPath='server runtime' end\n");
    send("guard_area_config.lua", etftConfig, 0, 2, "FixturePort=");
    send("guard_area_config.lua", etftConfig, 1, 2, "4567\n");
    const auto installed = root / "guardconfig" / "guard_area_config.lua";
    check(acks.back().error == errSuccess && g_GuardConfig.dwPort == 4567 && read(installed) == "FixturePort=4567\n",
        "guard configuration reload runs after the complete synchronous installation");
    send("guard_area_config.lua", etftConfig, 0, 1, "invalid lua !!!");
    check(acks.back().error != errSuccess && g_GuardConfig.dwPort == 4567 && read(installed) == "FixturePort=4567\n" &&
        read(versions / "release.zip-guard_area_config.lua") == "FixturePort=4567\n",
        "failed guard reload restores both the previous file and runtime configuration");
}

void deployment(Runtime& runtime, const fs::path& root)
{
    auto& server = CGuardServer::GetInstance();
    auto& handler = CGDMessageHandler::GetInstance();
    auto& acks = runtime.mt.acknowledgements;
    const auto send = [&](bool patch, std::string_view name, bool truncated = false) {
        const auto size = patch ? sizeof(N2U_HOT_UPDATE_PATCH) : sizeof(N2U_DEPLOY_SERVER_VERSION);
        auto* packet = runtime.allocator.Alloc(static_cast<INT32>(truncated ? sizeof(MESSAGE_HEADER) : size));
        reinterpret_cast<MESSAGE_HEADER*>(packet)->wMsg = patch ? n2u_hot_update_patch : n2u_deploy_server_version;
        if (!truncated)
        {
            if (patch) std::memcpy(reinterpret_cast<N2U_HOT_UPDATE_PATCH*>(packet)->szPatch, name.data(), name.size());
            else std::memcpy(reinterpret_cast<N2U_DEPLOY_SERVER_VERSION*>(packet)->szVersion, name.data(), name.size());
        }
        handler.OnRecv(&runtime.mt, packet, static_cast<UINT32>(truncated ? sizeof(MESSAGE_HEADER) : size));
    };
    const auto wait = [&](bool patch) {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
        while (server.DeploymentBusy() && std::chrono::steady_clock::now() < deadline)
        {
            if (patch) server.ProcessHotUpdatePatch(); else server.ProcessDeployServerVersion();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        check(!server.DeploymentBusy(), "owned deployment worker completion is polled and joined");
    };
    const auto run = fs::path(g_GuardConfig.szServerRunPath);
    const auto versions = fs::path(g_GuardConfig.szServerVersionPath);
    write(fs::path(g_GuardConfig.szServerPatchPath) / "worker.pak", "worker patch");
    const auto count = acks.size();
    send(true, "worker.pak");
    check(server.DeploymentBusy() && acks.size() == count && !server.ReloadConfig(),
        "patch work is asynchronous and configuration cannot change while it runs");
    check(!server.StartDeployment(true, "worker.pak"), "overlapping deployment tasks are rejected");
    wait(true);
    check(acks.back().error == errSuccess && acks.back().info == "worker.pak" &&
        fs::exists(run / "data" / "worker.pak"), "main-loop completion acknowledges the installed patch and dispatches reload");
    const auto completed = acks.size();
    server.ProcessHotUpdatePatch();
    check(acks.size() == completed, "completed worker patch is acknowledged once");
    send(true, "missing-worker.pak"); wait(true);
    check(acks.back().error == errCritical && acks.back().info == "missing-worker.pak",
        "worker IO failure reaches the main-thread failure acknowledgement");
    send(true, "ignored", true);
    check(!server.DeploymentBusy() && acks.back().error != errSuccess && acks.back().info.empty(),
        "truncated patch request does not read its missing name or start work");
    send(false, std::string(128, 'x'));
    check(!server.DeploymentBusy() && acks.back().error != errSuccess, "unterminated version request is rejected before work");

    const auto extractor = fs::path(__FILE__).parent_path().parent_path().parent_path() / "bin" / "_GUARD" / "7z.exe";
    fs::copy_file(extractor, root / "7z.exe");
    const auto source = root / "worker archive";
    write(source / "next" / "gameserver" / "new.bin", "deployed worker data");
    write(source / "next" / "_GUARD" / "never", "excluded");
    auto zipped = guard_deploy::run_process(extractor, {L"a", L"-tzip", (versions / "next.zip").native(), L"next"},
        source, root / "fixture zip.log");
    check(zipped && *zipped == 0, "create a real ZIP for packet-driven deployment integration");
    write(versions / "next.zip-area_config.lua", "new area");
    write(versions / "next.zip-gmsecurityconfig.lua", "new gm");
    write(versions / "next.zip-guard_area_config.lua", "FixturePort=6789\n");
    send(false, "next.zip");
    check(!server.DeploymentBusy() && acks.back().error == errCritical,
        "running process metadata prevents version deployment before reloading guard config");
    CProcessMgr::GetInstance().UnInit();
    send(false, "next.zip");
    check(server.DeploymentBusy() && g_GuardConfig.dwPort == 6789,
        "version deployment snapshots paths after main-thread guard configuration reload");
    wait(false);
    check(acks.back().error == errSuccess && acks.back().info == "next.zip" &&
        fs::exists(run / "gameserver" / "new.bin") && !fs::exists(root / "never"),
        "real packet-to-worker extraction publishes completion without updating the guard itself");
    check(runtime.allocator.packets.empty(), "deployment response packets release ownership");
}

void config(CGuardServer& server, const fs::path& root)
{
    const auto script = root / "guard_config.lua";
    g_GuardConfig.dwPort = 16;
    const auto original = g_GuardConfig;
    write(script, "function LoadFile(c,n) c.dwPort=99; c.szServerRunPath='partial'; error('fixture failure') end\n");
    check(!server.ReloadConfig() && std::memcmp(&original, &g_GuardConfig, sizeof(original)) == 0,
        "failed Lua configuration restores the previous complete configuration");
    write(script, "function LoadFile(c,n) c.dwPort=12345; c.szServerRunPath='runtime/../server runtime' end\n");
    check(server.ReloadConfig(), "load real Lua configuration and normalize relative paths");
    check(fs::path(g_GuardConfig.szServerRunPath) == root / "server runtime" &&
        fs::is_directory(root / "server runtime"), "relative server root becomes a normalized absolute directory");
    check(fs::path(g_GuardConfig.szGuardPath) == root && fs::path(g_GuardConfig.szServerVersionPath) == root / "version" &&
        fs::path(g_GuardConfig.szServerPatchPath) == root / "patch" && g_GuardConfig.dwPort == 12345,
        "version/patch/guard roots and other Lua fields retain their established roles");
    check(fs::is_directory(root / "version") && fs::is_directory(root / "patch"), "both transfer roots are created");
    const auto valid = g_GuardConfig;
    write(root / "blocked", "ordinary file");
    write(script, "function LoadFile(c,n) c.dwPort=1; c.szServerRunPath='blocked' end\n");
    check(!server.ReloadConfig() && std::memcmp(&valid, &g_GuardConfig, sizeof(valid)) == 0,
        "filesystem setup errors roll back the Lua configuration");
    write(script, "function LoadFile(c,n) c.dwPort=2; c.szServerRunPath=string.rep('x',500) end\n");
    check(!server.ReloadConfig() && std::memcmp(&valid, &g_GuardConfig, sizeof(valid)) == 0,
        "absolute paths that exceed the fixed configuration ABI are rejected without truncation");
    write(script, "function LoadFile(c,n) c.szServerRunPath='' end\n");
    check(!server.ReloadConfig() && std::memcmp(&valid, &g_GuardConfig, sizeof(valid)) == 0,
        "empty run directory is rejected transactionally");
}

void current(CGuardServer& server, const fs::path& root)
{
    const auto file = root / CURRENT_VERSION_FILE_NAME;
    std::array<char, 130> output{};
    output.fill('z');
    check(!server.FetchCurrentVersion(nullptr, 128) && !server.FetchCurrentVersion(output.data(), 0) && output[0] == 'z',
        "invalid output ranges are rejected without writes");
    check(!server.FetchCurrentVersion(output.data(), 128) && output[0] == '\0', "missing current version returns an empty output");
    write(file, "release.zip\r\n");
    check(server.FetchCurrentVersion(output.data(), 128) && std::string_view(output.data()) == "release.zip",
        "current version strips batch echo terminators");
    write(file, std::string(127, 'v') + "\r\n");
    check(server.FetchCurrentVersion(output.data(), 128) && output[127] == '\0' && output[128] == 'z',
        "maximum supported current-version name is terminated within the supplied range");
    write(file, std::string(128, 'v'));
    check(!server.FetchCurrentVersion(output.data(), 128) && output[0] == '\0' && output[128] == 'z',
        "former 128-byte fread overflow is rejected without an out-of-range write");
    write(file, "release.zip");
    check(!server.FetchCurrentVersion(output.data(), 4) && output[0] == '\0', "small output cannot silently truncate a version name");
    check(SetFileAttributesW(file.c_str(), FILE_ATTRIBUTE_READONLY) != 0, "make current-version fixture read-only");
    check(server.FetchCurrentVersion(output.data(), 128), "reading current version does not require write permission");
    check(SetFileAttributesW(file.c_str(), FILE_ATTRIBUTE_NORMAL) != 0, "restore fixture attributes");
}

void operations(CGuardServer& server, Runtime& runtime, const fs::path& root)
{
    const auto version = root / DEPLOY_RESULT_FILE_NAME;
    const auto patch = root / HOT_UPDATE_PATCH_RESULT_FILE_NAME;
    auto& acknowledgements = runtime.mt.acknowledgements;
    server.SetProcessingVersion("release.zip");
    check(server.ProcessDeployServerVersion() && acknowledgements.empty(), "missing deployment result remains pending");
    // Results are located by the guard root, even if the caller changes cwd.
    const auto other = root / "different cwd";
    fs::create_directory(other);
    fs::current_path(other);
    write(other / DEPLOY_RESULT_FILE_NAME, "wrong.zip");
    check(server.ProcessDeployServerVersion() && acknowledgements.empty(), "cwd result files cannot complete a guard-root deployment");
    write(version, "RELEASE.ZIP\r\n");
    check(server.ProcessDeployServerVersion() && acknowledgements.size() == 1 &&
        acknowledgements.back().error == errSuccess && acknowledgements.back().info == "release.zip" &&
        !fs::exists(version) && runtime.mt.summaries == 1, "exact case-insensitive deployment result succeeds and consumes the file");
    check(server.ProcessDeployServerVersion() && acknowledgements.size() == 1, "completed deployment is acknowledged only once");
    fs::current_path(root);
    server.SetProcessingVersion("release.zip");
    write(version, "release.zip.old\r\n");
    check(!server.ProcessDeployServerVersion() && acknowledgements.back().error == errUnknownError &&
        acknowledgements.back().info == "release.zip.old" && !fs::exists(version), "a prefix collision cannot report successful deployment");
    const auto failed = acknowledgements.size();
    check(server.ProcessDeployServerVersion() && acknowledgements.size() == failed, "failed deployment clears its pending state");

    for (const auto& value : {std::string{}, std::string(128, 'x'), std::string("embedded\0bytes", 14)})
    {
        server.SetProcessingVersion("bad-result.zip");
        write(version, value);
        check(!server.ProcessDeployServerVersion() && acknowledgements.back().error == errCritical &&
            acknowledgements.back().info == "bad-result.zip", "malformed deployment result sends a bounded critical acknowledgement");
        const auto count = acknowledgements.size();
        check(server.ProcessDeployServerVersion() && acknowledgements.size() == count, "malformed results do not cause repeated error acknowledgements");
        fs::remove(version);
    }
    const auto longest = std::string(127, 'v');
    server.SetProcessingVersion(longest.c_str());
    write(version, longest + "\r\n");
    check(server.ProcessDeployServerVersion() && acknowledgements.back().info == longest,
        "largest wire-compatible result name remains intact");

    server.SetProcessingPatch("hot.pak");
    write(patch, "hot.pak\r\n");
    check(!server.ProcessHotUpdatePatch() && acknowledgements.back().error == errGMGSExecute &&
        acknowledgements.back().info == "hot.pak", "patch reload failure is FALSE and retains its requested name");
    const auto afterFailedReload = acknowledgements.size();
    check(server.ProcessHotUpdatePatch() && acknowledgements.size() == afterFailedReload,
        "reload failure does not keep an operation pending");

    // Remote metadata is enough for the real command dispatcher's existing
    // manager check. No native PID is inspected or process started/stopped.
    check(CProcessMgr::GetInstance().AddProcessInfo(stMS, 12345, 1, FALSE), "register offline manager metadata");
    CGDSessionMgr::GetInstance().GetMSSession().pSocketSession = &runtime.ms;
    server.SetProcessingPatch("hot.pak");
    write(patch, "HOT.PAK\r\n");
    check(server.ProcessHotUpdatePatch() && acknowledgements.back().message == u2n_hot_update_patch_ack &&
        acknowledgements.back().error == errSuccess && acknowledgements.back().info == "hot.pak" &&
        runtime.ms.commands == std::vector<std::string>{".reload "},
        "successful hot update forwards reload and acknowledges the original nonempty patch name");
    server.SetProcessingPatch("hot.pak");
    write(patch, "hot.pak.extra");
    check(!server.ProcessHotUpdatePatch() && acknowledgements.back().error == errCritical &&
        runtime.ms.commands.size() == 1, "mismatched patch results never dispatch reload");

    server.SetProcessingVersion("locked.zip");
    write(version, "locked.zip");
    const auto beforeLocked = acknowledgements.size();
    {
        guard::NativeHandle file{CreateFileW(version.c_str(), GENERIC_READ, FILE_SHARE_READ,
            nullptr, OPEN_EXISTING, 0, nullptr)};
        check(file && file.get() != INVALID_HANDLE_VALUE, "lock deployment result against deletion");
        check(server.ProcessDeployServerVersion() && acknowledgements.size() == beforeLocked && fs::exists(version),
            "a result still held without delete sharing remains pending");
    }
    check(server.ProcessDeployServerVersion() && acknowledgements.back().error == errSuccess && !fs::exists(version),
        "a pending locked result is consumed after its handle closes");
    server.SetProcessingVersion("unreadable.zip");
    write(version, "unreadable.zip");
    const auto beforeUnreadable = acknowledgements.size();
    {
        guard::NativeHandle file{CreateFileW(version.c_str(), GENERIC_READ, 0,
            nullptr, OPEN_EXISTING, 0, nullptr)};
        check(file && file.get() != INVALID_HANDLE_VALUE, "lock deployment result against reads");
        check(server.ProcessDeployServerVersion() && acknowledgements.size() == beforeUnreadable,
            "an exclusive writer cannot be mistaken for a malformed completed result");
    }
    check(server.ProcessDeployServerVersion() && acknowledgements.back().error == errSuccess && !fs::exists(version),
        "exclusively locked results recover when the owner releases its handle");
    server.SetProcessingVersion("chunked.zip");
    const auto beforePartial = acknowledgements.size();
    {
        guard::NativeHandle file{CreateFileW(version.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
            nullptr, CREATE_ALWAYS, 0, nullptr)};
        DWORD written = 0;
        check(file && file.get() != INVALID_HANDLE_VALUE && WriteFile(file.get(), "chunk", 5, &written, nullptr) && written == 5,
            "create a partially written result with normal redirected-output sharing");
        check(server.ProcessDeployServerVersion() && acknowledgements.size() == beforePartial,
            "polling a partial write cannot report a mismatched deployment");
        check(WriteFile(file.get(), "ed.zip\r\n", 8, &written, nullptr) && written == 8,
            "finish the batch-style result line");
        check(server.ProcessDeployServerVersion() && acknowledgements.size() == beforePartial,
            "a complete line still owned by its writer remains pending");
    }
    check(server.ProcessDeployServerVersion() && acknowledgements.back().info == "chunked.zip" &&
        acknowledgements.back().error == errSuccess, "writer completion publishes exactly one complete deployment result");
    server.SetProcessingVersion("send-failure.zip");
    write(version, "send-failure.zip");
    runtime.mt.fail_send = true;
    check(!server.ProcessDeployServerVersion() && runtime.allocator.packets.empty(),
        "failed acknowledgement frees its packet and reports failure");
    runtime.mt.fail_send = false;
    const auto beforePoll = acknowledgements.size();
    check(server.ProcessDeployServerVersion() && acknowledgements.size() == beforePoll,
        "a failed send cannot acknowledge a consumed result repeatedly");
}
}

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2) throw std::runtime_error("workspace fixture parent required");
        Fixture fixture{argv[1]};
        Runtime runtime;
        incoming(runtime);
        CGuardServer server;
        config(server, fixture.root);
        current(server, fixture.root);
        payloads(runtime);
        registrations(runtime);
        operations(server, runtime, fixture.root);
        upload(runtime, fixture.root);
        deployment(runtime, fixture.root);
        check(server.UnInit() && server.UnInit() && !server.IsRuning(), "guard cleanup is repeatable with no native transport");
        check(CGDMessageHandler::GetInstance().UnInit() && CGDMessageHandler::GetInstance().UnInit(),
            "handler cleanup can repeat without retaining file or transport pointers");
    }
    catch (const std::exception& error) { check(false, error.what()); }
    std::printf("guard results: %d checks, %d failures\n", checks, failures);
    return failures != 0;
}
