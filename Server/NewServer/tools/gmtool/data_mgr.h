/********************************************************************
	created:	19:2:2014   10:38
	file:		data_mgr.h
	author:		Allan Tao
	
	purpose:	role data and so on.
*********************************************************************/


#ifndef _H_DATA_MGR_H_
#define _H_DATA_MGR_H_

#include "std_type.h"
#include "string_def.h"
#include "item_def.h"
#include "quest_def.h"
#include "mail_def.h"
#include "money_def.h"
#include "server_def.h"
#include "gm_def.h"
#include "gm_role_data.h"
#include <array>
#include <map>
#include <memory>
#include <span>
#include <vector>


struct GM_ITEM_DATA
{
    UINT64      llGuid;
    UINT32      dwTemplateID;
    char        szItemName[MAX_ITEM_NAME_LEN];
    INT32       nConSlot;
    INT32       nItemSlot;
    INT32       nType;
    INT32       nNum;
};

struct GM_QUEST_DATA
{
    UINT32      dwTemplateID;
    char        szQuestName[COMMON_NAME_LEN];
};

struct GM_MAIL_DATA
{
    UINT32      dwMailID;
    UINT32      dwMailTime;
    UINT8       byMailType;
    UINT8       byIsNew;
    char        szMailSubject[MAIL_SUBJECT_LEN];
    UINT8       byAttachType;
};


struct GM_ROLE_DATA
{
    UINT32      dwRoleID;
    gm::RoleDocument document;
    // A weak editor reference identifies this cache entry across replacement/clear.
    const std::shared_ptr<const char> lifetime=std::make_shared<const char>(0);
};


struct ITEM_TEMPLATE_DATA
{
    UINT32      dwTempID;
    char        szName[MAX_ITEM_NAME_LEN];
};


struct TL_ANNOUNCE_DATA
{
    INT32       nGroupID;
    INT32       nSvrID;
    UINT32      dwSerial;
    UINT32      dwIntervalTime;
    UINT32      dwRepeatNum;
    char        szText[GM_ANNOUNCE_LENGTH];
};



class CDataMgr
{
protected:
    static CDataMgr     m_Instance;

    CDataMgr();
    ~CDataMgr();

public:
    static CDataMgr&       GetInstance();

    BOOL Init();
    BOOL UnInit();

    // role data
    GM_ROLE_DATA*   StoreRole(UINT32 role_id, gm::RoleDocument document);
    BOOL            DeleteRole(GM_ROLE_DATA*);
    BOOL            ClearRole();
    GM_ROLE_DATA*   FindRole(UINT32 dwRoleID);

    // item template
    void        ClearItemMap(UINT32 dwSessionID);
    BOOL        ApplyItems(UINT32 session, bool clear, std::span<const ITEM_TEMPLATE_DATA> items);
    std::vector<ITEM_TEMPLATE_DATA> GetItems(UINT32 session) const;

    // announce
    const std::vector<TL_ANNOUNCE_DATA>& GetAnnounceList() const { return m_AnnounceList; }
    BOOL        ReplaceAnnounceList(std::span<const TL_ANNOUNCE_DATA> records);
    void        ClearAnnounceList()         { m_AnnounceList.clear(); }
    BOOL        DelAnnounceFromList(UINT32 dwAnnounceSerial);

private:
    // role data
    std::map<UINT32, std::unique_ptr<GM_ROLE_DATA>> m_Roles;
    bool m_initialized = false;



    // item template
    typedef     std::map<UINT32, ITEM_TEMPLATE_DATA>    ITEM_MAP;
    std::array<ITEM_MAP, MAX_GM_SESSION_COUNT> m_ItemMap;

    // announce 
    std::vector<TL_ANNOUNCE_DATA> m_AnnounceList;

};














#endif //_H_DATA_MGR_H_






