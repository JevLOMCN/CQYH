#ifndef _USER_MGR_H_
#define _USER_MGR_H_

#include "user.h"
#include "object_pool.h"
#include <map>
#include "server_def.h"

class CUserMgr
{
public:
    CUserMgr();
    virtual ~CUserMgr();

    BOOL                Init(UINT32 dwMaxUsers);
    BOOL                UnInit(void);
    static CUserMgr&    GetInstance(void);

    UINT32      Count();
	inline INT32 GetCountPerGroup(INT32 nServerGroup);
	inline INT32 GetCountByIP(const char* pcszIDAddress);

	MS_USER*    NewUser(const CHAR* pcszUserName, UINT32 dwClientSessionID, UINT32 dwClientIp,
		const char* pcszMac, const char* pcszOASessionID, const char* pcszPGT, BOOL bAdult, BOOL bIpWhiteList, BOOL bUserWhiteList);
    BOOL        DelUser(MS_USER* poUser);
    template <class TFunc>
    inline BOOL TraverseUser(TFunc& rFunc);

    BOOL        AttachRoleToUser(MS_USER* poUser, UINT32 dwRoleID);
    BOOL        DetachRoleFromUser(MS_USER* poUser);

    MS_USER*    FindUserBySessionID(UINT32 dwSessionID);
    UINT32      FindSessionIDByUserName(const CHAR* pcszUserName);
    MS_USER*    FindUserByUserName(const CHAR* pcszUserName);

	inline INT32 GetWaitUserCount(INT32 nServerGroup);
	void PushWaitUser(INT32 nServerGroup, MS_USER* poUser);
	void RemoveWaitUser(MS_USER* poUser);
	void ProcessQueue(INT32 nServerGroup);

    BOOL CheckAllUserTimeCard();

private:
    static CUserMgr             ms_Instance;
    CObjectPool<MS_USER>        m_oUserPool;

    typedef std::map<UINT32 , MS_USER*>             CSessionID2UserMap;
    typedef CSessionID2UserMap::iterator            CSessionID2UserMapItr;

    struct ltstr
    {
        bool operator()(const CHAR* s1, const CHAR* s2) const
        {
            return strncmp(s1, s2, USER_NAME_LEN) < 0;
        }
    };
    typedef std::map<const char*, UINT32, ltstr>    CUserKey2SessionIDMap;
    typedef CUserKey2SessionIDMap::iterator         CUserKey2SessionIDMapItr;
	typedef std::multimap<UINT32, UINT32>			MAP_IP_2_SESSION_ID;

    CSessionID2UserMap		m_mapUser;
    CUserKey2SessionIDMap	m_mapUserKey2Session;
	INT32					m_nUserCount[MAX_SERVER_GROUP_COUNT];

	MAP_IP_2_SESSION_ID		m_mapIP2SessionID;

	CBaseList				m_UserWaitList[MAX_SERVER_GROUP_COUNT];
	INT32					m_nWaitListSize[MAX_SERVER_GROUP_COUNT];
};

template <class TFunc>
inline BOOL CUserMgr::TraverseUser(TFunc& rFunc)
{
    INT32 nRetCode = 0;

    for (CSessionID2UserMap::iterator it = m_mapUser.begin();
        it != m_mapUser.end(); ++it)
    {
        nRetCode = rFunc(it->first, it->second);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return TRUE;
Exit0:
    return FALSE;
}

inline INT32 CUserMgr::GetCountPerGroup(INT32 nServerGroup)
{
	LOG_PROCESS_ERROR(nServerGroup >= 0 && nServerGroup < MAX_SERVER_GROUP_COUNT);
	return m_nUserCount[nServerGroup];
Exit0:
	return 0;
}

inline INT32 CUserMgr::GetCountByIP(const char* pcszIPAddress)
{
	UINT32 dwIPAddress{};
	LOG_PROCESS_ERROR(pcszIPAddress);

	dwIPAddress = inet_addr(pcszIPAddress);

	return (UINT32)m_mapIP2SessionID.count(dwIPAddress);
Exit0:
	return 0;
}

inline INT32 CUserMgr::GetWaitUserCount(INT32 nServerGroup)
{
	LOG_PROCESS_ERROR(nServerGroup >= 0 && nServerGroup < MAX_SERVER_GROUP_COUNT);
	return m_nWaitListSize[nServerGroup];
Exit0:
	return 0;
}

#endif 


