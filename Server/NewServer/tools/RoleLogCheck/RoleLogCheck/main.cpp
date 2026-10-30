#include "stdafx.h"
#include "rolelogcheck.h"
#include "dump.h"
#include "report.h"
#include "process_option.h"
#include "server_config.h"

#include <cstdlib>
#include <exception>
#include <iostream>

namespace
{
BOOL CheckGameFrame() { return FALSE; }

// Keep native SEH in an object-free boundary, outside the entry point's lifetime guards.
BOOL RunCheck(INT32 argc, char** argv)
{
    __try
    {
        return CRoleLogCheck::GetInstance().DoCheckLog(argv[1], argv[2],
            argc == 5 ? argv[3] : nullptr, argc == 5 ? argv[4] : nullptr);
    }
    __except (DumpExceptionFilter(GetExceptionInformation()))
    {
        return FALSE;
    }
}

class Runtime
{
public:
    Runtime() = default;
    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;
    bool Start()
    {
        logging = CLog::GetInstance().Init("role_log_check", lltAll, lotFile, TRUE, TRUE, TRUE);
        if (!logging)
            return false;
        g_InitReportSetting(GetServerShortName(stGS), 0, g_ServerConfig.Common.szReportAbsolutePath);
        dumping = StartDumpThread(CheckGameFrame, FALSE, "log");
        if (!dumping)
            return false;
        SetDumpOption(g_ServerConfig.Common.bShowDumpDialog);
        return CRoleLogCheck::GetInstance().Init();
    }
    ~Runtime()
    {
        CRoleLogCheck::GetInstance().UnInit();
        if (dumping)
            StopDumpThread();
        if (logging)
            CLog::GetInstance().UnInit();
    }
private:
    bool logging = false;
    bool dumping = false;
};
}

int main(int argc, char* argv[])
{
    try
    {
        if (!CRoleLogCheck::GetInstance().CheckParam(argc, argv))
            return EXIT_FAILURE;
        Runtime runtime;
        if (!runtime.Start())
        {
            std::cerr << "RoleLogCheck: initialization failed\n";
            return EXIT_FAILURE;
        }
        const bool success = RunCheck(argc, argv);
        COutput::GetInstance().Printf(success ? "log file parse complete." : "*** log file parse fail ***");
        return success ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    catch (const std::exception& error)
    {
        std::cerr << "RoleLogCheck: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
