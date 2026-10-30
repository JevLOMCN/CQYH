#include "stdafx.h"
#include "transmit_file_mgr.h"
#include "maintaintool.h"
#include "message_handler.h"
#include "session_mgr.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string_view>

using namespace InternalMessage;
namespace
{
constexpr auto acknowledgementTimeout = std::chrono::seconds{60};
bool same_name(std::string_view left, std::string_view right)
{
    return std::ranges::equal(left, right, [](unsigned char a, unsigned char b) {
        return std::tolower(a) == std::tolower(b);
    });
}
bool valid_name(std::string_view name)
{
    if (name.empty() || name.size() >= sizeof(N2U_SEND_FILES::szFileName) ||
        name == "." || name == ".." || name.back() == '.' || name.back() == ' ') return false;
    for (const unsigned char c : name)
        if (c < 32 || c == 127 || std::string_view{"<>:\"/\\|?*"}.contains(static_cast<char>(c))) return false;
    const auto stem = name.substr(0, name.find('.'));
    for (const auto reserved : {"CON", "PRN", "AUX", "NUL", "CONIN$", "CONOUT$"})
        if (same_name(stem, reserved)) return false;
    return !(stem.size() == 4 && (same_name(stem.substr(0, 3), "COM") || same_name(stem.substr(0, 3), "LPT")) &&
        stem[3] >= '1' && stem[3] <= '9');
}
}

CTransmitFileMgr CTransmitFileMgr::ms_Instance;
CTransmitFileMgr& CTransmitFileMgr::GetInstance() { return ms_Instance; }
CTransmitFileMgr::Key CTransmitFileMgr::key(INT32 area, UINT32 ip)
{
    return (static_cast<Key>(static_cast<UINT32>(area)) << 32) | ip;
}
bool CTransmitFileMgr::current(Key id, const Pending& transfer) const
{
    const auto found = m_sending.find(id);
    return found != m_sending.end() && found->second == transfer;
}
BOOL CTransmitFileMgr::IsSending(INT32 area, UINT32 ip) const { return m_sending.contains(key(area, ip)); }
BOOL CTransmitFileMgr::Reload() { return UnInit(); }
BOOL CTransmitFileMgr::UnInit()
{
    // Close explicitly: a callback may still own a snapshot of the transfer.
    for (const auto& [id, transfer] : m_sending)
        for (auto& file : transfer->files) if (file.stream.is_open()) file.stream.close();
    m_sending.clear();
    return TRUE;
}
BOOL CTransmitFileMgr::StopSending(INT32 area, UINT32 ip)
{
    const auto found = m_sending.find(key(area, ip));
    if (found == m_sending.end()) return FALSE;
    auto transfer = std::move(found->second);
    m_sending.erase(found);
    for (auto& file : transfer->files) if (file.stream.is_open()) file.stream.close();
    return TRUE;
}
void CTransmitFileMgr::fail(Key id, const Pending& transfer, const char* reason)
{
    if (!current(id, transfer)) return;
    const auto area = transfer->area;
    const auto ip = transfer->ip;
    StopSending(area, ip);
    ERR("file transfer failed: %s", reason);
    if (!IsSending(area, ip)) MaintainTool::GetInstance().UpdateGuardSendProgress(area, ip, -1);
}
BOOL CTransmitFileMgr::StartSending(INT32 area, UINT32 ip, INT32 count,
    const char* names[], const char* paths[], INT32 type)
try
{
    if (count <= 0 || count > MAX_SENDING_FILES || !names || !paths) return FALSE;
    std::vector<Source> sources;
    sources.reserve(count);
    for (INT32 index=0; index<count; ++index)
    {
        if (!names[index] || !paths[index]) return FALSE;
        sources.push_back({names[index], std::filesystem::path{paths[index]}});
    }
    return StartSending(area, ip, sources, type);
}
catch (const std::exception& error) { ERR("cannot prepare file transfer: %s", error.what()); return FALSE; }

BOOL CTransmitFileMgr::StartSending(INT32 area, UINT32 ip, std::span<const Source> sources, INT32 type)
{
    if (area <= 0 || area >= MAX_AREA_COUNT || !ip || sources.empty() || sources.size() > MAX_SENDING_FILES ||
        (type != etftVersion && type != etftConfig && type != etftPatch) || IsSending(area, ip)) return FALSE;
    const auto* session = CSessionMgr::GetInstance().GetRemoteGDSession(area, ip);
    if (!session || !session->pSocketSession || !session->bConnected) return FALSE;
    try
    {
        auto transfer = std::make_shared<Transfer>();
        transfer->area = area; transfer->ip = ip; transfer->type = type;
        transfer->files.reserve(sources.size());
        for (const auto& source : sources)
        {
            if (source.path.empty() || !valid_name(source.name))
                throw std::invalid_argument("invalid transfer filename or path");
            if (std::ranges::any_of(transfer->files, [&](const auto& file) { return same_name(file.name, source.name); }))
                throw std::invalid_argument("duplicate transfer filename");
            File file;
            file.name = source.name; file.path = source.path;
            if (!std::filesystem::is_regular_file(file.path)) throw std::runtime_error("transfer source is not a regular file");
            file.stream.open(file.path, std::ios::binary | std::ios::ate);
            const auto size = file.stream.tellg();
            if (!file.stream || size < 0) throw std::runtime_error("cannot open transfer source");
            file.size = static_cast<std::uint64_t>(size);
            const auto blocks = std::max<std::uint64_t>(1, (file.size + G_FILE_SEND_BLOCK - 1) / G_FILE_SEND_BLOCK);
            if (blocks > std::numeric_limits<UINT16>::max()) throw std::runtime_error("transfer source exceeds protocol size");
            file.blocks = static_cast<UINT16>(blocks);
            file.stream.seekg(0);
            if (!file.stream) throw std::runtime_error("cannot seek transfer source");
            transfer->files.push_back(std::move(file));
        }
        m_sending.emplace(key(area, ip), std::move(transfer));
        return TRUE;
    }
    catch (const std::exception& error) { ERR("cannot start file transfer: %s", error.what()); }
    return FALSE;
}
BOOL CTransmitFileMgr::MainLoop()
{
    if (m_polling) return TRUE;
    m_polling = true;
    struct Reset { bool& value; ~Reset() { value = false; } } reset{m_polling};
    // Never retain map iterators across socket or GUI callbacks.
    std::vector<std::pair<Key, Pending>> pending{m_sending.begin(), m_sending.end()};
    for (const auto& [id, transfer] : pending)
    {
        try
        {
            for (int budget = 0; budget < SEND_BLOCK_PER_LOOP && current(id, transfer); ++budget)
            {
                if (transfer->index >= transfer->files.size()) break;
                auto& file = transfer->files[transfer->index];
                if (file.sent == file.blocks)
                {
                    if (m_clock() >= transfer->deadline)
                        fail(id, transfer, "timed out waiting for file acknowledgement");
                    break;
                }
                std::array<char, G_FILE_SEND_BLOCK> buffer{};
                const auto position = static_cast<std::uint64_t>(file.sent) * G_FILE_SEND_BLOCK;
                const auto length = static_cast<UINT16>(std::min<std::uint64_t>(G_FILE_SEND_BLOCK, file.size - position));
                file.stream.read(buffer.data(), length);
                if (file.stream.gcount() != length || file.stream.bad())
                    throw std::runtime_error("transfer source was truncated or could not be read");
                const auto block = file.sent;
                const auto fileIndex = static_cast<UINT16>(transfer->index);
                if (++file.sent == file.blocks)
                {
                    // Counted chunks avoid the extra empty read at exact block multiples.
                    if (file.stream.peek() != std::char_traits<char>::eof() || file.stream.bad())
                        throw std::runtime_error("transfer source changed size while reading");
                    file.stream.close();
                    transfer->deadline = m_clock() + acknowledgementTimeout;
                }
                const auto progress = static_cast<INT32>(100 * (fileIndex + static_cast<double>(file.sent) / file.blocks) / transfer->files.size());
                // Publish the sent index before Send: a synchronous acknowledgement is valid.
                if (!CMTMessageHandler::GetInstance().DoGDSendFiles(transfer->area, transfer->ip, transfer->type,
                    file.name.c_str(), block, file.blocks, length, buffer.data(), fileIndex, static_cast<UINT16>(transfer->files.size())))
                    throw std::runtime_error("socket rejected transfer chunk");
                if (!current(id, transfer)) break;
                MaintainTool::GetInstance().UpdateGuardSendProgress(transfer->area, transfer->ip, progress);
            }
        }
        catch (const std::exception& error) { fail(id, transfer, error.what()); }
        catch (...) { fail(id, transfer, "transfer callback threw an exception"); }
    }
    return TRUE; // One failed peer must not stop the GUI or other transfers.
}
BOOL CTransmitFileMgr::Acknowledge(INT32 area, UINT32 ip, const U2N_SEND_FILES_ACK& message)
{
    const auto id = key(area, ip);
    const auto found = m_sending.find(id);
    if (found == m_sending.end()) return TRUE; // A cancelled transfer can still have queued replies.
    auto transfer = found->second;
    if (transfer->index >= transfer->files.size()) return FALSE;
    const auto& file = transfer->files[transfer->index];
    const auto end = std::ranges::find(message.szFileName, '\0');
    if (end == std::end(message.szFileName) ||
        std::string_view{message.szFileName, static_cast<std::size_t>(end - message.szFileName)} != file.name ||
        message.wFileTotal != transfer->files.size() || message.wFileIndex != transfer->index ||
        message.wBufferTotal != file.blocks || message.wBufferIndex >= file.sent) return FALSE;
    if (message.nErrorCode != errSuccess)
    {
        StopSending(area, ip);
        INF("send file:%s failed, ErrorCode = %d", message.szFileName, message.nErrorCode);
        if (!IsSending(area, ip))
            MaintainTool::GetInstance().UpdateGuardSendProgress(area, ip, message.nErrorCode < 0 ? message.nErrorCode : -1);
        return TRUE;
    }
    if (message.wBufferIndex + 1 != file.blocks || file.sent != file.blocks) return FALSE;
    const bool completed = ++transfer->index == transfer->files.size();
    if (completed) StopSending(area, ip);
    INF("send file:%s success", message.szFileName);
    if (completed && !IsSending(area, ip))
    {
        MaintainTool::GetInstance().UpdateGuardSendProgress(area, ip, 101);
    }
    return TRUE;
}
