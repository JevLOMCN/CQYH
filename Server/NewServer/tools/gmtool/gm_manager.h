#pragma once
#ifdef _WIN32
#include <winsock2.h>
#endif
#include "std_type.h"
#include "predefine.h"
#include "string_def.h"
#include "gm_runtime.h"
#include <array>
#include <memory>

class CGMManager
{
protected:
    CGMManager();
    virtual ~CGMManager();
    static CGMManager m_Instance;
public:
    static CGMManager& GetInstance();
    BOOL Init();
    BOOL UnInit();
    BOOL MainLoop();
    bool IsRunning() const {return m_runtime!=nullptr;}
    bool IsRetiring() const {return m_gate->retiring;}
    bool CleanupSucceeded() const {return m_gate->cleanup_ok;}
    void SetGMDialogState(GM_DIALOG_STATE state);
    GM_DIALOG_STATE GetGMDialogState();
    const char* GetUserName();
    const char* GetUserPwd();
    void SetUserName(const char* name);
    void SetUserPwd(const char* password);
private:
    std::shared_ptr<gm::RuntimeGate> m_gate=std::make_shared<gm::RuntimeGate>();
    std::shared_ptr<gm::Runtime> m_runtime;
    bool m_starting=false,m_stopping=false,m_looping=false,m_cancelStart=false;
    std::array<char,USER_NAME_LEN> m_szUserName{};
    std::array<char,GM_TOOL_PASSWORD_LEN> m_szUserPwd{};
    GM_DIALOG_STATE m_eGMDialogState=DIALOG_NONE;
};
