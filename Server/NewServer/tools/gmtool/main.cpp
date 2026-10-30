#include "stdafx.h"
#include "gmtool.h"
#include "loader.h"
#include "gm_manager.h"

INT32   g_State;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, LPSTR lpCmdLine, int nCmdShow)
{
    QApplication app(__argc, __argv);
    gmtool window;
    loader login;
    gm::ApplicationViews views(window, login);
    struct StopRuntime { ~StopRuntime(){CGMManager::GetInstance().UnInit();} } stop;
    INT32       nRetCode = 0;
    INT32       nResult  = 0;

    nRetCode = CGMManager::GetInstance().Init();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = window.Init();
    LOG_PROCESS_ERROR(nRetCode);
    {
        g_State = 1;
        if (login.exec() == QDialog::Accepted)
        {
            g_State = 2;
            return app.exec();
        }
    }
    

    return 0;
Exit0:
    return 1;
}
