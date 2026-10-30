
#ifndef _ONLINE_ROLE_DATA_H_
#define _ONLINE_ROLE_DATA_H_

#include "string_def.h"
#include "scene_def.h"
#include "friend_def.h"
#include "master_def.h"
#include "queue_match_def.h"
#include "base_list.h"
#include <map>
#include <set>

struct MS_ROLE_DATA;
struct MS_USER;

#define MAX_POS_COUNT               (4)
#define MAX_JOIN_TEAM_ROLE_COUNT    (4)

struct MS_ONLINE_ROLE_DATA : public BASE_LIST_NODE
{
    // essence info
    MS_USER*            pUser;
    MS_ROLE_DATA*       pRoleData;

    // basic info
    INT32               nGMLevel;
    INT32				nCurrentHp;
    INT32				nCurrentMp;
    INT32				nMaxHp;
    INT32				nMaxMp;
	BOOL				bEnabledTeamRecall;

    // frequency limits
    UINT32				dwLastSpeakTime;
    UINT32              dwLastQueryRoleInfoTime;
    UINT8               byQueryRoleInfoCount;

    //////////////////////////////////////////////////////////////////////////
    // scene session info
    //////////////////////////////////////////////////////////////////////////
    UINT32   		    dwCliSessionID;
    UINT32   		    dwGSSessionID;
    UINT32              dwTargetGSSessionID;

    INT32               nPreEnterSceneTempID;	// redundant of PosList[0] ?
    INT32               nPreEnterSceneID;

    ABSOLUTE_SCENE_POS  PosList[MAX_POS_COUNT];
    INT32               nPosIndex;
    INT32               nPosCount;
    UINT32              dwSaveTime;

    //////////////////////////////////////////////////////////////////////////
    // joining
    //////////////////////////////////////////////////////////////////////////
	QUEUE_DATA			stBattleQueue;
	//QUEUE_DATA			stArenaQueue;

    typedef std::map<UINT32, UINT8> TeamInviteMap;
    TeamInviteMap       m_TeamInvite;
    UINT32              dwJoinTeamRoleID[MAX_JOIN_TEAM_ROLE_COUNT];
    UINT32              dwWaitReqRoleID;    // used when joining team or joining arena

    //////////////////////////////////////////////////////////////////////////
    // Friend
    //////////////////////////////////////////////////////////////////////////
    UINT8 m_byFriendState;
    char m_szSignature[SIGNATURE_LEN];
    typedef std::map<UINT32, UINT8> FriendMap;
    FriendMap m_friends;
    typedef std::set<UINT32> FanMap;
    FanMap m_fans;

    typedef std::list<UINT32>   PERSONAL_ENEMY_LIST;    
    PERSONAL_ENEMY_LIST m_personalEnemy;// fifo

    // Opposite of enemies: players this character has killed
    typedef std::set<UINT32>   PE_KILL_LIST;
    PE_KILL_LIST        m_KillList;

    void BroadCastInfoChange();

    //////////////////////////////////////////////////////////////////////////
    // Master
    //////////////////////////////////////////////////////////////////////////
    UINT8               bTwitter;
    UINT32              dwBeDeterminedRoleID;
    UINT32              dwMasterTwitterCD;
    INT64				llMasterExp;
    UINT32              dwMasterCD;
    BASE_LIST_NODE      stMasterTwitterNode;

    struct MASTER_SEND_APP 
    {
        UINT32          dwRoleID;
        INT32           nWashoutTime;
    };
    UINT8               byMasterSendAppNum;
    MASTER_SEND_APP     arrMasterSendApp[MAX_MASTER_SEND_APP_NUM];
    UINT8               byMasterRecvAppNum;
    UINT32              arrMasterRecvApp[MAX_MASTER_RECV_APP_NUM];

    //////////////////////////////////////////////////////////////////////////
    // Guild
    //////////////////////////////////////////////////////////////////////////
    UINT32      dwGuildInviterID;

    //////////////////////////////////////////////////////////////////////////
    // Shop
    //////////////////////////////////////////////////////////////////////////
    struct ShopLog
    {
        INT32 nItemID;
        INT32 nItemCount;
        INT32 nCost;
        UINT32 dwShopTime;
    };
    std::list<ShopLog> m_lstShopLog;
    UINT8 m_byShopLogLoaded;
    void SendShopLog();
    void AddShopLog(const ShopLog& shoplog);
};

BOOL LoadFriends(std::list<void*>& rResult, UINT32 roleID, UINT32 clientSessionID);

#endif


