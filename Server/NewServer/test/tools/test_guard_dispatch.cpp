// Compile the real command manager. Only its process/network/guard-operation
// boundaries are substituted; this executable cannot launch or stop processes.
#include "stdafx.h"
#include "cmd_mgr.h"
#include "gd_message_handler.h"
#include "gd_session_mgr.h"
#include "guardserver.h"
#include "process_mgr.h"
#include "guard_config.h"

#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace
{
int checks = 0;
int failures = 0;
int running = 0;
int guardCloses = 0;
int guardLoops = 0;
int networkCloses = 0;
int processCloses = 0;
int allCloses = 0;
int allStarts = 0;
bool operationResult = true;
std::string message;
std::function<void()> onStart;
struct Start
{
    UINT8 type;
    std::string parameters;
    BOOL view;
    BOOL release;
};
std::vector<Start> starts;
void Check(bool value, const char* description)
{
    ++checks;
    if (!value) { ++failures; std::fprintf(stderr, "FAIL: %s\n", description); }
}
}

CGuardServer::CGuardServer() : m_bRun(FALSE), m_qwLatestTickCheckDiskSpace(0),
    m_pNet(nullptr), m_pSocketAcceptor(nullptr) {}
CGuardServer::~CGuardServer() = default;
CGuardServer& CGuardServer::GetInstance() { static CGuardServer value; return value; }
BOOL CGuardServer::Close() { ++guardCloses; return operationResult; }
BOOL CGuardServer::MainLoop() { ++guardLoops; return operationResult; }

CProcessMgr::CProcessMgr() : m_qwLastStatisTick(0) {}
CProcessMgr::~CProcessMgr() = default;
CProcessMgr& CProcessMgr::GetInstance() { static CProcessMgr value; return value; }
INT32 CProcessMgr::CountRuningProcessByType(UINT8) { return running; }
BOOL CProcessMgr::StartProcess(UINT8 type, char* parameters, BOOL view, BOOL release, BOOL)
{
    starts.push_back({type, parameters ? parameters : "", view, release});
    if (onStart) onStart();
    return operationResult;
}
BOOL CProcessMgr::StartAllProcess(char*, BOOL, BOOL) { ++allStarts; return operationResult; }
BOOL CProcessMgr::CloseProcess(UINT8) { ++processCloses; return operationResult; }
BOOL CProcessMgr::CloseAllProcess() { ++allCloses; return operationResult; }

CGDMessageHandler::CGDMessageHandler() = default;
CGDMessageHandler::~CGDMessageHandler() = default;
CGDMessageHandler& CGDMessageHandler::GetInstance() { static CGDMessageHandler value; return value; }
void CGDMessageHandler::OnAccepted(ISocketAcceptor*, ISocketSession*) {}
void CGDMessageHandler::OnConnected(ISocketSession*) {}
void CGDMessageHandler::OnRecv(ISocketSession*, PACKET_BUFFER*, UINT32) {}
void CGDMessageHandler::OnDisconnected(ISocketSession*, BOOL) {}
BOOL CGDMessageHandler::DoMSClose() { ++networkCloses; return operationResult; }
BOOL CGDMessageHandler::DoMSCommand(const char* command) { message = command; return operationResult; }
BOOL CGDMessageHandler::DoAllPing() { return TRUE; }

int main()
{
    CCmdMgr commands;
    Check(!commands.ExecOPCmd(nullptr), "null command rejected");
    Check(!commands.ExecOPCmd(""), "empty command rejected");
    Check(!commands.ExecOPCmd("unknown -g"), "unknown command rejected");
    Check(!commands.ExecOPCmd(".reload data"), "GM command rejected by operator dispatcher");
    Check(starts.empty() && guardCloses == 0 && allCloses == 0, "invalid commands have no effects");

    Check(commands.ExecOPCmd("start -g -r"), "literal start command executes");
    Check(starts.size() == 1 && starts.back().type == stGS && starts.back().release &&
        !starts.back().view && starts.back().parameters.empty(), "start flags reach process boundary");
    Check(commands.ExecOPCmd("START -m -p -c"), "upper-case command executes");
    Check(starts.size() == 2 && starts.back().type == stMS && starts.back().view &&
        !starts.back().release && starts.back().parameters == "config global_config_final.lua",
        "view and configuration flags retained");
    Check(commands.ExecOPCmd("start"), "start-all command executes");
    Check(allStarts == 1, "start-all boundary called once");
    Check(commands.ExecOPCmd("close -g"), "selected process close dispatches");
    Check(processCloses == 1, "selected close boundary called once");
    Check(commands.ExecOPCmd("close"), "close-all command dispatches");
    Check(allCloses == 1, "close-all boundary called once");
    Check(commands.ExecOPCmd("check"), "check command dispatches");
    Check(starts.size() == 3 && starts.back().type == stGS && starts.back().parameters == "check",
        "check request parameters retained");
    Check(commands.ExecOPCmd("stop"), "stop command succeeds with no running manager");
    Check(guardLoops == 0 && networkCloses == 0, "idle stop avoids polling and network operations");

    running = 1;
    Check(commands.ExecOPCmd("reload  data.lua"), "reload command dispatches");
    Check(message == ".reload  data.lua", "reload preserves parameter spacing");
    Check(commands.ExecGMCmd(".reload item"), "GM command passes directly to message boundary");
    Check(message == ".reload item", "GM command bytes retained");
    const auto longCommand = "reload " + std::string(4096, 'x');
    Check(commands.ExecOPCmd(longCommand.c_str()), "long command retains historical truncation");
    Check(message == ".reload " + std::string(247, 'x'), "existing forwarded command limit retained");

    operationResult = false;
    const auto priorStarts = starts.size();
    Check(!commands.ExecOPCmd("start -g"), "operation failure reaches caller");
    Check(starts.size() == priorStarts + 1, "failed operation attempted once");
    Check(!commands.ExecOPCmd("bogus"), "unknown command after failure rejected");
    Check(starts.size() == priorStarts + 1, "unknown command cannot reuse earlier dispatch");
    operationResult = true;
    Check(commands.ExecOPCmd("quit"), "success after failure dispatches normally");
    Check(guardCloses == 1, "quit called once");

    const auto beforeNested = starts.size();
    bool nested = false;
    onStart = [&] {
        if (!nested) { nested = true; Check(commands.ExecOPCmd("quit"), "nested command succeeds"); }
    };
    Check(commands.ExecOPCmd("start -g -m"), "outer command survives nested dispatch");
    Check(starts.size() == beforeNested + 2 && starts[beforeNested].type == stGS &&
        starts.back().type == stMS, "nested dispatch preserves outer parameter storage");
    Check(guardCloses == 2, "nested command executes once");
    onStart = {};
    Check(commands.UnInit(), "command manager cleanup succeeds");
    Check(!commands.ExecOPCmd("unknown"), "cleanup leaves no stale command");

    std::printf("guard command dispatch: %d checks, %d failures\n", checks, failures);
    return failures != 0;
}
