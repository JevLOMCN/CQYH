#include "data_mgr.h"

#include <algorithm>
#include <cstring>

CDataMgr CDataMgr::m_Instance;

CDataMgr::CDataMgr() = default;
CDataMgr::~CDataMgr() = default;

CDataMgr& CDataMgr::GetInstance()
{
    return m_Instance;
}

BOOL CDataMgr::Init()
{
    UnInit();
    m_initialized = true;
    return TRUE;
}

BOOL CDataMgr::UnInit()
{
    m_initialized = false;
    m_Roles.clear();
    for (std::size_t session = 0; session < m_ItemMap.size(); ++session)
    {
        m_ItemMap[session].clear();
    }
    m_AnnounceList.clear();
    return TRUE;
}

GM_ROLE_DATA* CDataMgr::StoreRole(UINT32 role_id, gm::RoleDocument document)
{
    if (!m_initialized)
        return nullptr;
    // Allocate before replacing the old entry. Other roles retain their
    // addresses as the standard map grows.
    auto role = std::make_unique<GM_ROLE_DATA>();
    role->dwRoleID = role_id;
    role->document = std::move(document);
    auto* result = role.get();
    m_Roles.insert_or_assign(role_id, std::move(role));
    return result;
}

BOOL CDataMgr::DeleteRole(GM_ROLE_DATA* role)
{
    if (!role || !m_initialized)
        return FALSE;
    // Compare owned addresses without dereferencing a foreign/stale pointer.
    const auto found = std::ranges::find_if(m_Roles, [role](const auto& entry) { return entry.second.get() == role; });
    if (found == m_Roles.end())
        return FALSE;
    m_Roles.erase(found);
    return TRUE;
}

BOOL CDataMgr::ClearRole()
{
    if (!m_initialized)
        return FALSE;
    m_Roles.clear();
    return TRUE;
}

GM_ROLE_DATA* CDataMgr::FindRole(UINT32 role_id)
{
    const auto found = m_Roles.find(role_id);
    return found == m_Roles.end() ? nullptr : found->second.get();
}

void CDataMgr::ClearItemMap(UINT32 session)
{
    if (session >= m_ItemMap.size())
        return;
    m_ItemMap[session].clear();
}

BOOL CDataMgr::ApplyItems(UINT32 session, bool clear, std::span<const ITEM_TEMPLATE_DATA> items)
{
    if (!m_initialized || session >= m_ItemMap.size())
        return FALSE;
    ITEM_MAP staged;
    for (const auto& item : items)
    {
        if (!std::memchr(item.szName, 0, sizeof(item.szName)))
            return FALSE;
        staged.try_emplace(item.dwTempID, item);
    }

    auto& destination = m_ItemMap[session];
    if (clear)
    {
        destination.swap(staged);
    }
    else
        destination.merge(staged); // First value wins, as with legacy insert().
    return TRUE;
}

std::vector<ITEM_TEMPLATE_DATA> CDataMgr::GetItems(UINT32 session) const
{
    if (!m_initialized || session >= m_ItemMap.size())
        return {};
    std::vector<ITEM_TEMPLATE_DATA> result;
    result.reserve(m_ItemMap[session].size());
    for (const auto& [id, item] : m_ItemMap[session])
        result.push_back(item);
    return result;
}

BOOL CDataMgr::ReplaceAnnounceList(std::span<const TL_ANNOUNCE_DATA> records)
{
    if (!m_initialized)
        return FALSE;
    for (const auto& record : records)
    {
        if (record.nSvrID <= 0 || record.nGroupID < 0 || record.nGroupID >= MAX_SERVER_GROUP_COUNT ||
            !std::memchr(record.szText, 0, sizeof(record.szText)))
            return FALSE;
    }
    std::vector<TL_ANNOUNCE_DATA> staged(records.begin(), records.end());
    m_AnnounceList.swap(staged);
    return TRUE;
}

BOOL CDataMgr::DelAnnounceFromList(UINT32 serial)
{
    const auto found = std::ranges::find(m_AnnounceList, serial, &TL_ANNOUNCE_DATA::dwSerial);
    if (found != m_AnnounceList.end())
    {
        // Move the entire record, including server/group identity.
        *found = m_AnnounceList.back();
        m_AnnounceList.pop_back();
    }
    return TRUE;
}




