#include "stdafx.h"
#include <chrono>
#include <thread>
#include "cmd_mgr.h"
#include "guard_command.h"

#include <string>

#include "gd_message_handler.h"
#include "process_mgr.h"
#include "guardserver.h"
#include "gd_session_mgr.h"
#include "time_ex.h"

//m_CMDFuncList[] =
//{
//    { "quit",        &CCmdMgr::QuitQuard,       "quit the guard"}, 
//    { "start",       &CCmdMgr::StartServer,     "start the server"}, 
//    { "stop",        &CCmdMgr::StopServer,      "stop the server group"},
//    { "close",       &CCmdMgr::CloseServer,     "kill the server process"},
//    { "help" ,       &CCmdMgr::CmdHelp,         "cmd: start close."\
//                                      "param: -a(all server) -m(manageserver) -t(gateserver)" \ 
//                                      "-g(gameserver) -d(dbserver) -l(loginserver) -b(bsserver);" \
//                                      "-p(primary)"\
//                                      "cmd: stop"}
//};
const char* g_CmdDescribe = "guard cmd list:\n"\
                            "start [-a -m -t -g -d -l -b -o -q -p -c -r]\n" \
                            "stop [-f]\n" \
                            "close [-a -m -t -g -d -l -b -o -q]\n" \
							"check"	\
                            "quit\n\n";

CCmdMgr CCmdMgr::ms_Instance;

CCmdMgr::CCmdMgr()
{
    m_bNeedPrintCmdDes = FALSE;
}

CCmdMgr::~CCmdMgr()
{
    UnInit();
}

CCmdMgr& CCmdMgr::GetInstance(void)
{
    return ms_Instance;
}

BOOL CCmdMgr::Init()
{
    m_bNeedPrintCmdDes = FALSE;

    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_INSERT_MODE | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);

    return TRUE;
}

BOOL CCmdMgr::UnInit(void)
{
    m_bNeedPrintCmdDes = FALSE;

    return TRUE;
}

BOOL CCmdMgr::QuitGuard(INT32 nCount, const char* pcszParam)
{
    INT32 nRetCode = 0;
    
    nRetCode = CGuardServer::GetInstance().Close();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCmdMgr::StartServer(INT32 nCount, const char* pcszParam)
{
	INT32 nRetCode = 0;
	BOOL bRunRelease = FALSE;	// Start the debug version by default; -r starts the release version
	if (nCount == 0)
	{
        nRetCode = CProcessMgr::GetInstance().StartAllProcess(NULL, FALSE, bRunRelease);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        char pStartParam[128] = {0};
        BOOL bView = FALSE;
        for(INT32 i = 0; i < nCount; ++i)
		{
			if (pcszParam[i] == ' ' || pcszParam[i] == '-')
			{
				continue;
			}
			
            if(pcszParam[i] == 'p')
            {
                bView = TRUE;
            }
            //else if(pcszParam[i] == 'c')
            //{
            //    strxcat(pStartParam, "config global_config_pftest.lua", sizeof(pStartParam));
            //}
            else if(pcszParam[i] == 'c')
            {
                strxcat(pStartParam, "config global_config_final.lua", sizeof(pStartParam));
            }
            else if(pcszParam[i] == 'r')
            {
                bRunRelease = TRUE;
            }
        }

        for(INT32 i = 0; i < nCount; ++i)
        {
            switch(pcszParam[i])
            {
            case 'a':
            case 'A':
                {
                    nRetCode = CProcessMgr::GetInstance().StartAllProcess(pStartParam, bView, bRunRelease);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'm':
            case 'M':
                {
                    nRetCode = CProcessMgr::GetInstance().StartProcess(stMS, pStartParam, bView, bRunRelease);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 't':
            case 'T':
                {
                    nRetCode = CProcessMgr::GetInstance().StartProcess(stGT, pStartParam, bView, bRunRelease);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'g':
            case 'G':
                {
                    nRetCode = CProcessMgr::GetInstance().StartProcess(stGS, pStartParam, bView, bRunRelease);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'd':
            case 'D':
                {
                    nRetCode = CProcessMgr::GetInstance().StartProcess(stDB, pStartParam, bView, bRunRelease);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'l':
            case 'L':
                {
                    nRetCode = CProcessMgr::GetInstance().StartProcess(stLS, pStartParam, bView, bRunRelease);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'b':
            case 'B':
                {
                    nRetCode = CProcessMgr::GetInstance().StartProcess(stBS, pStartParam, bView, bRunRelease);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
			case 'o':
			case 'O':
				{
					nRetCode = CProcessMgr::GetInstance().StartProcess(stDL, pStartParam, bView, bRunRelease);
					LOG_PROCESS_ERROR(nRetCode);
				}
				break;
			case 'q':
			case 'Q':
			{
				nRetCode = CProcessMgr::GetInstance().StartProcess(stGM, pStartParam, bView, bRunRelease);
				LOG_PROCESS_ERROR(nRetCode);
			}
				break;
            default:
                break;
            }
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCmdMgr::StopServer(INT32 nCount, const char* pcszParam)
{
    INT32 nRetCode = 0;

    BOOL bMSRuning = CProcessMgr::GetInstance().CountRuningProcessByType(stMS);
    UINT64 qwCurrentTick = GetTickCountEx();
    while(bMSRuning)
    {
        nRetCode = CGuardServer::GetInstance().MainLoop();
        LOG_PROCESS_ERROR(nRetCode);

        if(CGDSessionMgr::GetInstance().GetMSSession().pSocketSession)
        {
            nRetCode = CGDMessageHandler::GetInstance().DoMSClose();
            LOG_CHECK_ERROR(nRetCode);
        }
		else
		{
			LOG_CHECK_ERROR(FALSE);
			break;
		}

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        bMSRuning = CProcessMgr::GetInstance().CountRuningProcessByType(stMS);

        if (GetTickCountEx() - qwCurrentTick > 10 * 1000)
        {
            CRI("stop ms failed after waiting 10s, now no wait");
			LOG_PROCESS_ERROR(FALSE);
            break;
        }
    }

    //if(nCount == 1 && pcszParam[0] == 'f' && bMSRuning)
    //{
    //    nRetCode = CProcessMgr::GetInstance().CloseAllProcess();
    //    LOG_PROCESS_ERROR(nRetCode);
    //    INF("killed all process");
    //}

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCmdMgr::CloseServer(INT32 nCount, const char* pcszParam)
{
    INT32 nRetCode = 0;
    if(nCount == 0)
    {
        nRetCode = CProcessMgr::GetInstance().CloseAllProcess();
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        for(INT32 i = 0; i < nCount; ++i)
        {
            switch(pcszParam[i])
            {
            case 'a':
            case 'A':
                {
                    nRetCode = CProcessMgr::GetInstance().CloseAllProcess();
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'm':
            case 'M':
                {
                    nRetCode = CProcessMgr::GetInstance().CloseProcess(stMS);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 't':
            case 'T':
                {
                    nRetCode = CProcessMgr::GetInstance().CloseProcess(stGT);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'g':
            case 'G':
                {
                    nRetCode = CProcessMgr::GetInstance().CloseProcess(stGS);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'd':
            case 'D':
                {
                    nRetCode = CProcessMgr::GetInstance().CloseProcess(stDB);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'l':
            case 'L':
                {
                    nRetCode = CProcessMgr::GetInstance().CloseProcess(stLS);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
            case 'b':
            case 'B':
                {
                    nRetCode = CProcessMgr::GetInstance().CloseProcess(stBS);
                    LOG_PROCESS_ERROR(nRetCode);
                }
                break;
			case 'o':
			case 'O':
				{
					nRetCode = CProcessMgr::GetInstance().CloseProcess(stDL);
					LOG_PROCESS_ERROR(nRetCode);
				}
				break;
			case 'q':
			case 'Q':
			{
				nRetCode = CProcessMgr::GetInstance().CloseProcess(stGM);
				LOG_PROCESS_ERROR(nRetCode);
			}
				break;
            default:
                break;
            }
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

BOOL CCmdMgr::CheckServerConfig(INT32 nCount, const char* pcszParam)
{
	INT32 nRetCode = 0;
	char pStartParam[] = "check";
	BOOL bView = FALSE;

	nRetCode = CProcessMgr::GetInstance().StartProcess(stGS, pStartParam, bView);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCmdMgr::ReloadServerConfig(INT32 nCount, const char* pcszParam)
{
    INT32 nRetCode = 0;
    std::string command{".reload "};
    // The former 256-byte buffer could lose its terminator on truncation.
    // Retain its intended payload bound with explicitly terminated storage.
    command.append(std::string_view{pcszParam}.substr(0, 255 - command.size()));

    BOOL bMSRuning = CProcessMgr::GetInstance().CountRuningProcessByType(stMS);
    LOG_PROCESS_ERROR( bMSRuning );

    nRetCode = CGDMessageHandler::GetInstance().DoMSCommand(command.c_str());
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CCmdMgr::ExecGMCmd(const char* szFullCmd)
{
    INT32 nRetCode = 0;
    BOOL bMSRuning = FALSE;
	LOG_PROCESS_ERROR(szFullCmd);
	LOG_PROCESS_ERROR(szFullCmd[0] == '.' );

	bMSRuning = CProcessMgr::GetInstance().CountRuningProcessByType(stMS);
	LOG_PROCESS_ERROR(bMSRuning);

	nRetCode = CGDMessageHandler::GetInstance().DoMSCommand(szFullCmd);
	LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

INT32 CCmdMgr::ExecOPCmd(const char* szFullCmd)
{
    m_bNeedPrintCmdDes = TRUE;
    if (szFullCmd == nullptr)
    {
        LOG_CHECK_ERROR(szFullCmd);
        return FALSE;
    }

    const auto command = guard_command::Parse(szFullCmd);
    if (!command)
    {
        LOG_CHECK_ERROR(command.has_value());
        return FALSE;
    }

    using CommandFunction = BOOL (CCmdMgr::*)(INT32, const char*);
    static constexpr std::array<CommandFunction, guard_command::names.size()> functions{
        &CCmdMgr::QuitGuard, &CCmdMgr::StartServer, &CCmdMgr::StopServer,
        &CCmdMgr::CloseServer, &CCmdMgr::CheckServerConfig, &CCmdMgr::ReloadServerConfig};
    const auto function = functions[static_cast<std::size_t>(command->kind)];
    // The callback receives its own terminated storage; parsing never writes to input.
    const std::string parameters{command->parameters};
    const BOOL result = (this->*function)(static_cast<INT32>(parameters.size()), parameters.c_str());
    LOG_CHECK_ERROR(result);
    m_bNeedPrintCmdDes = !result;
    return result ? TRUE : FALSE;
}

BOOL CCmdMgr::PrintCmdDescribe()
{
    if(m_bNeedPrintCmdDes)
        printf("\r%s", g_CmdDescribe);
    
    return TRUE;
}

