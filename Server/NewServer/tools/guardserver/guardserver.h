#ifndef _GUARD_SERVER_H_
#define _GUARD_SERVER_H_

#include <list>
#include <vector>
#include <atomic>
#include "guard_files.h"
#include "guard_transport.h"
#include "guard_console.h"

#include "server_def.h"
#include "packet_allocator.h"

#include "gd_message_handler.h"


class INet;
class ISocketAcceptor;

class CGuardServer
{
public:
    CGuardServer();
    ~CGuardServer();

    static CGuardServer& GetInstance(void);

    BOOL Init(INetEventHandler* poNetEventHandler);
    BOOL UnInit(void);
    
    BOOL Close(void);
    BOOL MainLoop(void);

    INet* GetNetInterface(void)                     {return m_pNet.get();}
    IPacketAllocator* GetPacketAllocator(void)      {return &m_PacketAllocator;}

    BOOL IsRuning()                                 {return m_bRun;}
    
	void Cls();
    BOOL PrintCmdPrompt();

	BOOL ReloadConfig();

	void SetProcessingVersion(const char* szVersion)	{ strxcpy(m_szProcessingVersion, szVersion, sizeof(m_szProcessingVersion)); }
	void SetProcessingPatch(const char* szPatch)	{ strxcpy(m_szProcessingPatch, szPatch, sizeof(m_szProcessingPatch)); }

	BOOL ProcessDeployServerVersion();
	BOOL ProcessHotUpdatePatch();
    BOOL FetchCurrentVersion( char* pcszCurrentVersion, int nLen  );
    BOOL StartDeployment(bool patch, const char* name);
    bool DeploymentBusy() const noexcept { return m_deployment.busy(); }
    void StopBackgroundWork();

private:
    BOOL _UpdateScreen();
	BOOL _PrintProcessContent();
	BOOL _GetLogSubFolders(const char* pszTargetDir);
	BOOL _AsyncEraseOldestLogSubFolders(std::string strSubFolderName);
    void _PollLogCleanup();
    BOOL _ProcessOperationResult(bool patch);

	std::vector<std::string> m_roLogFolderEntries;
    static CGuardServer     ms_Instance;
	std::atomic_bool        m_bRun{false};
	UINT64					m_qwLatestTickCheckDiskSpace;

    guard_runtime::NetOwner m_pNet;
    guard_runtime::AcceptorOwner m_pSocketAcceptor;
    CPacketAllocator        m_PacketAllocator;

	CHAR					m_szProcessingVersion[128]{};
	CHAR					m_szProcessingPatch[128]{};
    guard_files::FileTask m_logCleanup;
    guard_files::FileTask m_deployment;
    bool m_deploymentIsPatch = false;
    guard_runtime::ConsoleInput m_console;
    UINT64 m_lastScreenTick = 0;
};

#endif  // _GUARD_SERVER_H_
