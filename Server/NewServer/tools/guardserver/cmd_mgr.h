#ifndef _CMD_MGR_H_
#define _CMD_MGR_H_

class CCmdMgr
{
public:
    CCmdMgr();
    ~CCmdMgr();

    BOOL Init();
    BOOL UnInit(void);
	static CCmdMgr& GetInstance(void);
	BOOL PrintCmdDescribe();

	INT32 ExecGMCmd(const char* szFullCmd);
	INT32 ExecOPCmd(const char* szFullCmd);
private:
	BOOL QuitGuard(INT32 nCount, const char* pcszParam);
	BOOL StartServer(INT32 nCount, const char* pcszParam);
	BOOL StopServer(INT32 nCount, const char* pcszParam);
	BOOL CloseServer(INT32 nCount, const char* pcszParam);
	BOOL CheckServerConfig(INT32 nCount, const char* pcszParam);
	BOOL ReloadServerConfig(INT32 nCount, const char* pcszParam);

private:
    static CCmdMgr  ms_Instance;

    BOOL            m_bNeedPrintCmdDes;
};


#endif  // _CMD_MGR_H_
