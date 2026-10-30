#ifndef _PROCESS_MGR_H_
#define _PROCESS_MGR_H_

#include <list>
#include <unordered_map>
#include "std_type.h"
#include "server_def.h"

struct SProcessInfo
{
	UINT8 byType = stInvalid;
	UINT8 bySessionID = 0;
	UINT8 bAuto = FALSE;
    UINT32 dwProcessID = 0;
    UINT32 dwIP = 0;
	FILETIME ftProcessCreationTime{};
};

class CProcessMgr
{
    // The guard main thread owns this registry. Notifications may reenter it.
    // Native query handles are scoped; server process lifetimes are not owned.
public:
    CProcessMgr();
    ~CProcessMgr();
    CProcessMgr(const CProcessMgr&) = delete;
    CProcessMgr& operator=(const CProcessMgr&) = delete;

    BOOL Init();
    BOOL UnInit(void);
    static CProcessMgr& GetInstance(void);

	BOOL StartProcess(UINT8 byType, char* szCmdLine, BOOL bView = FALSE, BOOL bRunRelease = FALSE, BOOL bAuto = FALSE);
    BOOL CloseProcess(UINT8 byType);
    
    BOOL StartAllProcess(char* szCmdLine, BOOL bView = FALSE, BOOL bRunRelease = FALSE);
    BOOL CloseAllProcess();

    INT32 CountRuningProcessByType(UINT8 byType);
    BOOL IsProcessRuningByID(UINT32 dwProcessID); // local host PID only
	BOOL MainLoop();
	BOOL AddProcessInfo(UINT8 byType, UINT32 dwProcessID, UINT32 dwIP, BOOL bAuto);
	BOOL SetProcessInfo(UINT32 dwProcessID, UINT8 byType, UINT8 bySessionID);
	
	BOOL SyncProcessInfo(UINT32 nState, UINT32 dwProcessID);
	BOOL SyncAllProcessInfo();
	
	using CSprocessList = std::list<SProcessInfo>;
	using CSprocessListItr = CSprocessList::iterator;
	const CSprocessList& GetProcesses() const { return m_listProcess; }

private:
	BOOL UpdateServersInfo();
	BOOL UpdateServersStatis(const SProcessInfo& process);

    struct CpuSample
    {
        FILETIME creation{};
        UINT64 systemKernel = 0;
        UINT64 systemUser = 0;
        UINT64 processKernel = 0;
        UINT64 processUser = 0;
    };
    std::unordered_map<UINT32, CpuSample> m_cpuSamples;
    bool m_updating = false;

    static CProcessMgr                        ms_Instance;
	UINT64									  m_qwLastStatisTick;

    CSprocessList                             m_listProcess;	// todo: map PID -> SProcessInfo
};

#endif  // _PROCESS_MGR_H_
