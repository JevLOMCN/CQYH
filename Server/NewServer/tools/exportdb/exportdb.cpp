// exportdb.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "string_ex.h" // Required by the existing gameworld data-manager headers.
#include "sql_mgr.h"
#include "string_def.h"
#include "server_def.h"
#include "global_config.h"
#include "server_config.h"
#include "math_ex.h"
#include "game_world.h"
#include "packet_allocator.h"
#include "imessage_notifier.h"
#include "script_mgr.h"



#include "define.h"
#include "guild.h"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <string_view>

CPacketAllocator g_PacketAllocator;

class CMessageNotifier : public IMessageNotifier
{
	BOOL SendToUser(UINT32 dwClientSessionID, PACKET_BUFFER* pData, UINT32 dwSize) override { g_PacketAllocator.Free(pData); return TRUE; }
	BOOL SendToUsers(UINT32 dwGTSessionID, UINT32 dwClientSessionIDList[], INT32 nSessionCount, PACKET_BUFFER* pData, UINT32 dwSize) override
	{
		g_PacketAllocator.Free(pData); 
		return TRUE;
	}
	BOOL SendToMS(PACKET_BUFFER* pData, UINT32 dwSize) override { g_PacketAllocator.Free(pData); return TRUE; }
	BOOL SendToDL(PACKET_BUFFER* pData, UINT32 dwSize) override { g_PacketAllocator.Free(pData); return TRUE; }

	BOOL ReturnSelectRole(UINT32 dwClientSessionID) override { return TRUE; }
	BOOL SaveRoleData(CRole* poRole, BOOL bSaveFile) override { return TRUE; }
	BOOL UnlockRole(CRole* poRole) override { return TRUE; }
	BOOL _KickUser(UINT32 dwClientSessionID) override { return TRUE; }

	BOOL Logout(UINT32 dwClientSessionID) override { return TRUE; }
};


void _InitMathTable(void)
{
	std::memset(&g_MathTable, 0, sizeof(g_MathTable));

	for (int nDirection = 0; nDirection < MAX_DIRECTION_COUNT; nDirection++)
	{
		g_MathTable.nSinTable[nDirection] = static_cast<INT32>(std::floor(std::sin(static_cast<double>(nDirection) * 2 * PI / MAX_DIRECTION_COUNT) * SIN_COS_NUMBER + 0.5f));
		g_MathTable.nCosTable[nDirection] = static_cast<INT32>(std::floor(std::cos(static_cast<double>(nDirection) * 2 * PI / MAX_DIRECTION_COUNT) * SIN_COS_NUMBER + 0.5f));
	}

	for (int nDirection = 0; nDirection < MAX_DIRECTION_COUNT / 4; nDirection++)
	{
		g_MathTable.nSinSquareTable[nDirection] = g_MathTable.nSinTable[nDirection] * g_MathTable.nSinTable[nDirection];
	}

	for (int i = 0; i < MAX_SQUARE; i++)
	{
		g_MathTable.wSqrtTable[i] = static_cast<UINT16>(std::floor(std::sqrt(static_cast<double>(i)) + 0.5f));
	}
}

namespace
{
// The installed MSVC standard library does not provide <scope>. A unique_ptr
// with a custom deleter guards a borrowed lifetime token without allocating.
[[nodiscard]] auto on_exit(bool& state, auto action)
{
    auto release = [&state, action = std::move(action)](bool*) noexcept
    {
        try { action(); }
        catch (...) { state = false; }
    };
    return std::unique_ptr<bool, decltype(release)>(&state, std::move(release));
}
bool run_export(std::string database, bool& cleanup_ok)
{
    const auto pak_cleanup = on_exit(cleanup_ok, [&] { cleanup_ok = g_ClosePakFolder() && cleanup_ok; });
    if (!LoadGlobalConfig(g_szGlobalConfigFileName))
        return false;
    if (timeBeginPeriod(1) != TIMERR_NOERROR)
        return false;
    const auto timer_cleanup = on_exit(cleanup_ok, [&] { cleanup_ok = (timeEndPeriod(1) == TIMERR_NOERROR) && cleanup_ok; });
    const auto log_cleanup = on_exit(cleanup_ok, [&] { cleanup_ok = CLog::GetInstance().UnInit() && cleanup_ok; });
    if (!CLog::GetInstance().Init("exportdb", g_ServerConfig.MS.Log.nLogLevelMask,
        g_ServerConfig.MS.Log.nLogOutputMask, g_ServerConfig.MS.Log.bEnableLogThread,
        g_ServerConfig.MS.Log.bPrintThreadID, g_ServerConfig.MS.Log.bFlushFile))
        return false;
    WSADATA sockets{};
    if (WSAStartup(MAKEWORD(2, 2), &sockets) != 0)
        return false;
    const auto socket_cleanup = on_exit(cleanup_ok, [&] { cleanup_ok = (WSACleanup() == 0) && cleanup_ok; });
    const auto packet_cleanup = on_exit(cleanup_ok, [&] { cleanup_ok = g_PacketAllocator.UnInit() && cleanup_ok; });
    if (!g_PacketAllocator.Init(64, 64))
        return false;
    if (g_ServerConfig.DB.bUseIndependentDB && database.empty())
    {
        std::array<char, 256> hostname{};
        if (gethostname(hostname.data(), static_cast<int>(hostname.size())) != 0)
            return false;
        database = hostname.data();
        std::ranges::replace(database, '-', '_');
    }
    if (database.empty())
        database = g_ServerConfig.DB.szDBName;
    CSqlMgr sql;
    if (!sql.Init(g_ServerConfig.DB.szDBIPAddress, g_ServerConfig.DB.szDBAccount,
        g_ServerConfig.DB.szDBPassword, database.c_str(), g_ServerConfig.DB.szDBCharset,
        g_ServerConfig.DB.dwDBPort, g_SqlStmtList, mysqlTotal, 8))
        return false;
    const auto sql_cleanup = on_exit(cleanup_ok, [&] { cleanup_ok = sql.UnInit() && cleanup_ok; });
    _InitMathTable();
    const auto math_cleanup = on_exit(cleanup_ok, [&] { cleanup_ok = UnInitMathEx() && cleanup_ok; });
    CMessageNotifier notifier;
    const auto world_cleanup = on_exit(cleanup_ok, [&] { cleanup_ok = g_UnInitGameWorldLib() && cleanup_ok; });
    if (!g_InitGameWorldLib(GetTickCountEx(), &notifier, &g_PacketAllocator))
        return false;
    if (!CScriptMgr::GetInstance().FindScript(GM_SCRIPT_NAME))
        return false;
    // Role export/insertion helpers live in role_gameworld.cpp. The default
    // operation remains the existing guild/member export.
    return cqyh::exportdb::ExportGuilds(sql) && cqyh::exportdb::ExportMembers(sql);
}
}

int main(int argc, char** argv)
{
    if (argc == 2 && std::string_view(argv[1]) == "--help")
    {
        std::cout << "Usage: exportdb [database]\nExports guild_stat.txt and guild_member.txt using the configured database.\n";
        return EXIT_SUCCESS;
    }
    if (argc > 2 || (argc == 2 && std::string_view(argv[1]).starts_with('-')))
    {
        std::cerr << "Usage: exportdb [database]\n";
        return EXIT_FAILURE;
    }
    bool cleanup_ok = true;
    try
    {
        const std::string database = argc == 2 ? argv[1] : "";
        if (database.size() >= 256 || !run_export(database, cleanup_ok) || !cleanup_ok)
        {
            std::cerr << "exportdb: export or cleanup failed\n";
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error)
    {
        std::cerr << "exportdb: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
