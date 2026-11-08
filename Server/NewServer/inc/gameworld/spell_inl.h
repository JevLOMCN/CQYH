#ifndef _SPELL_INL_H_
#define _SPELL_INL_H_

inline void CSpell::SetIDLevelRune(INT32 nID, INT32 nLevel, INT32 nRuneID)
{
	m_Data.m_nID = nID;
	m_Data.m_nLevel = nLevel;
	m_Data.m_nRuneSlotID = nRuneID;
}

inline INT32 CSpell::GetID(void)
{
	return m_Data.m_nID;
}

inline INT32 CSpell::GetLevel(void)
{
	return m_Data.m_nLevel;
}

inline INT32 CSpell::GetRuneSlot(void)
{
	return m_Data.m_nRuneSlotID;
}

inline INT32 CSpell::GetAttr(INT32 eAttrType)
{
	LOG_PROCESS_ERROR(eAttrType > satInvalid && eAttrType < satTotal);

	return m_Data.m_nAttrTable[eAttrType];
Exit0:
	return -1;
}

inline INT32 CSpell::GetLengthAttr(INT32 eAttrType)
{
	LOG_PROCESS_ERROR(eAttrType > satInvalid && eAttrType < satTotal);

	return CONVERT_LENGTH(m_Data.m_nAttrTable[eAttrType]);
Exit0:
	return -1;
}

inline INT32 CSpell::GetAngleAttr(INT32 eAttrType)
{
	LOG_PROCESS_ERROR(eAttrType > satInvalid && eAttrType < satTotal);

	return CONVERT_ANGLE(m_Data.m_nAttrTable[eAttrType]);
Exit0:
	return -1;
}

inline void CSpell::CopyTo(CSpell* p)
{
	memcpy(&p->m_Data, &m_Data, sizeof(m_Data));
	memcpy(&p->m_Param, &m_Param, sizeof(m_Param));
}

inline void CSpell::CopyData(SPELL_ENTRY* pData)
{
	memcpy(pData, &m_Data, sizeof(m_Data));
}

inline SPELL_ENTRY* CSpell::GetData(void)
{
	return &m_Data;
}

inline void CSpell::SetX(INT32 nIndex, INT32 nValue)
{
	LOG_PROCESS_ERROR(nIndex >= 0 && nIndex < MAX_EX_PARAM_COUNT);

	m_Param.m_nX[nIndex] = nValue;
Exit0:
	return;
}

inline ExtraParam* CSpell::GetExParam(void)
{
	return &m_Param;
}

inline INT32 CSpell::GetSelfCoolDownFrm(void)
{
	return CONVERT_TIME(m_Data.m_nAttrTable[satSelfCoolDown]);
}

inline INT32 CSpell::GetEventCoolDownFrm(void)
{
	return CONVERT_TIME(m_Data.m_nAttrTable[satEventCoolDown]);
}

inline BOOL CSpell::CanBindSlot(INT32 nSlotID)
{
	LOG_PROCESS_ERROR(nSlotID >= qssActiveBegin && nSlotID < qssActiveEnd);
	return (1 << nSlotID & GetAttr(satBindSlot));
Exit0:
	return FALSE;
}

inline BOOL CSpell::CanShowOnUI(void)
{
	return m_Data.m_bShowOnUI;
}

inline BOOL CSpell::CanLearnByClass(INT32 nClass)
{
	return (GetClassSpellMask(nClass) & m_Data.m_nAttrTable[satClass]) > 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

inline void CSpellTemplate::InitAttr(INT32 eAttrType, INT32 nVal)
{
	CSpell::SetAttr(eAttrType, nVal);
}


inline BOOL CSpellTemplate::IsInited(void)
{
	return m_bInited;
}

inline void CSpellTemplate::ResetFlag(void)
{
	m_bInited = FALSE;
}

#endif // _spell_inl_h__

