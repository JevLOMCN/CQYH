#include "stdafx.h"
#include "role.h"

#include "zlib.h"
#include "role_db_def.h"
#include "role_db_base_data_conv.h"

#include "role_db_def.h"
#include "server_config.h"
#include "level_data.h"
#include "game_world.h"
#include "spell_config.h"

typedef BOOL(CRole::*LOAD_ROLE_DATA_FUNC)(const char* pcData, UINT32 dwSize);

BOOL CRole::Load(const char* pcData, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	ROLE_DB_DATA* pRoleData = NULL;
	UINT32 dwRealDataSize = 0;

	static LOAD_ROLE_DATA_FUNC s_LoadFuncList[] =
	{
        NULL,
        &CRole::_Load_v_1,
        &CRole::_Load_v_2,
        &CRole::_Load_v_3,
	};

	m_bIsLoading = TRUE;

	LOG_PROCESS_ERROR(pcData);

	// header
	LOG_PROCESS_ERROR(dwSize >= sizeof(ROLE_DB_DATA));
	pRoleData = (ROLE_DB_DATA*)pcData;

	LOG_PROCESS_ERROR(pRoleData->dwLen == dwSize);
	LOG_PROCESS_ERROR(pRoleData->dwCRC ==
		crc32(0, (const Bytef*)&pRoleData->dwVersion, pRoleData->dwLen - sizeof(pRoleData->dwLen) - sizeof(pRoleData->dwCRC)));
	LOG_PROCESS_ERROR(pRoleData->dwVersion > 0 && pRoleData->dwVersion < sizeof(s_LoadFuncList) / sizeof(s_LoadFuncList[0]));

	if (pRoleData->dwVersion < ROLE_DB_DATA_CURRENT_VERSION)
		INF("load old role %d db data %d %d", m_dwObjID, pRoleData->dwVersion, pRoleData->dwLen);

	// Default code changed to force use of the function at index 0
	nRetCode = (this->*s_LoadFuncList[pRoleData->dwVersion])(pcData, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	if (GetLevel() == 0)
	{
		nRetCode = _InitNewRole();
		LOG_PROCESS_ERROR(nRetCode);
	}

	TryWeeklyRefresh();
	TryDailyRefresh();

	// wing panel: the level bonuses are not part of the saved attributes
	ApplyWingLevelAttr(0, GetWingLevel());

	if (m_nCurrentHp > m_AttrList.GetAttr(catMaxHp))
		m_nCurrentHp = m_AttrList.GetAttr(catMaxHp);
	if (m_nCurrentMp > m_AttrList.GetAttr(catMaxMp))
		m_nCurrentMp = m_AttrList.GetAttr(catMaxMp);

	if (m_nCurrentHp <= 0)
	{
		m_eMoveState = CMS_DIE;
	}
	else
		m_eMoveState = CMS_STAND;

	if (m_nGMLevel > 0 && !m_bInJump && g_GWConfig.nGMAuraID != 0)
	{
		CAuraInstance* poAuraInst = NULL;
		CAura* poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(g_GWConfig.nGMAuraID);
		if (poAuraInfo)
		{
			poAuraInst = AppendAura(poAuraInfo, this, m_dwObjID, NULL, 0, 0, 0, 0);
			LOG_CHECK_ERROR(poAuraInst);
		}
	}

	m_bHpChanged = TRUE;
	m_bMpChanged = TRUE;

	m_bIsLoading = FALSE;

	return TRUE;
Exit0:

	CRI("failed to load role %d db data", m_dwObjID);

	m_bIsLoading = FALSE;
	return FALSE;
}

BOOL CRole::_LoadBaseData(const char* pcData, UINT32 dwSize)
{
	INT32 nRetCode = 0;
	ROLE_DB_BASE_DATA BaseData;

	LOG_PROCESS_ERROR(pcData);

	nRetCode = ConvertToBaseDataCurrentVersion(&BaseData, pcData, dwSize);
	LOG_PROCESS_ERROR(nRetCode);

	// Default code, temporarily disabled
	LOG_PROCESS_ERROR(BaseData.byVersion == ROLE_DB_BASE_DATA_CURRENT_VERSION);

	m_dwCreateTime = BaseData.dwCreateTime;
	m_dwSaveTime = BaseData.dwSaveTime;

	strxcpy(m_szName, BaseData.szRoleName, ROLE_NAME_LEN);
	strxcpy(m_szUserName, BaseData.szUserName, USER_NAME_LEN);
	SetClass(BaseData.byClass);
	SetSex(BaseData.bySex);
	m_byHairType = BaseData.byHairType;
	m_byHairColor = BaseData.byHairColor;
	m_byFaceType = BaseData.byFaceType;
	m_bySkinColor = BaseData.bySkinColor;

	_ComputeAttribute(BaseData.byLevel);
	m_nLevel = BaseData.byLevel;

	m_StaticReturnPos.nSceneTempID = BaseData.byStaticReturnSceneTempID;
	m_StaticReturnPos.nSceneID = BaseData.nStaticReturnSceneID;
	m_StaticReturnPos.nX = BaseData.nStaticReturnX;
	m_StaticReturnPos.nY = BaseData.nStaticReturnY;
	m_StaticReturnPos.nZ = BaseData.nStaticReturnZ;
	m_StaticReturnPos.nFaceDirection = BaseData.wStaticReturnFaceDirection;

	m_RevivePos.nSceneTempID = BaseData.byReviveSceneTempID;
	m_RevivePos.nSceneID = BaseData.nReviveSceneID;
	m_RevivePos.nX = BaseData.nRevivePtX;
	m_RevivePos.nY = BaseData.nRevivePtY;
	m_RevivePos.nZ = BaseData.nRevivePtZ;
	m_RevivePos.nFaceDirection = BaseData.wStaticReturnFaceDirection;

    if (BaseData.wMountID != 0)
        OnMount(BaseData.wMountID);

    if (BaseData.wWingID != 0)
        EquipWing(BaseData.wWingID);

    return TRUE;
Exit0:
    return FALSE;
}
