#ifndef _CHARACTER_INL_H_
#define _CHARACTER_INL_H_

inline INT32 CCharacter::GetClass(void) const
{
	return m_nClass;
}

inline INT32 CCharacter::GetSex(void) const
{
	return m_nSex;
}

inline INT32 CCharacter::GetLevel(void) const
{
	return m_nLevel;
}

//inline INT32 CCharacter::GetFaction(void)
//{
//	return m_nFaction;
//}

inline INT32 CCharacter::GetStance(void)
{
	return m_eStance;
}

inline BOOL CCharacter::SetStance(INT32 eValue)
{
	LOG_PROCESS_ERROR(eValue > csfInvalid && eValue < csfTotal);
	m_eStance = eValue;

	return TRUE;
Exit0:
	return FALSE;
}

inline void CCharacter::SetClass(INT32 nClass)
{
	m_nClass = nClass;
}

inline void CCharacter::SetSex(INT32 nSex)
{
	m_nSex = nSex;
}

inline INT32 CCharacter::GetHp(void)
{
	return m_nCurrentHp;
}

inline INT32 CCharacter::GetMp(void)
{
	return m_nCurrentMp;
}

inline void CCharacter::SetHp(INT32 nHp)
{
	if(nHp <= 0)
		nHp = 0;

	INT32 nMaxHp = 0;

	PROCESS_ERROR(nHp != m_nCurrentHp);

	nMaxHp = m_AttrList.GetAttr(catMaxHp);

	if(nMaxHp < nHp)
		nHp = nMaxHp;

	m_nCurrentHp = nHp;
	m_bHpChanged = TRUE;

#ifndef _ROBOT
	if(m_nCurrentHp <= 0 && m_eMoveState != CMS_DIE)
		Die(0);
#endif

Exit0:
	return;
}

//inline void CCharacter::SetMp(INT32 nMp)
//{
//	INT32 nMaxMp = m_AttrList.GetAttr(catMaxMp);
//
//	if(nMaxMp < nMp)
//		nMp = nMaxMp;
//	else if(nMp < 0)
//		nMp = 0;
//
//	m_nCurrentMp = nMp;
//}

inline void CCharacter::ChangeMp(INT32 nValue)
{
	INT32 nMaxMp = 0;
	PROCESS_ERROR(nValue != 0);

	nMaxMp = m_AttrList.GetAttr(catMaxMp);
	m_nCurrentMp += nValue;

//	INF("Mp Change : %d, %s.", nValue, GetName());

	if (m_nCurrentMp >= nMaxMp)
	{
		m_nTotalDrugCureMp = 0;
		m_nCurrentMp = nMaxMp;
	}

	m_nCurrentMp = m_nCurrentMp < 0 ? 0 : m_nCurrentMp;
	m_bMpChanged = TRUE;

Exit0:
	return;
}

inline void CCharacter::ChangeTotalDrugCureHp(INT32 nValue)
{
	m_nTotalDrugCureHp += nValue;
}

inline void CCharacter::ChangeTotalDrugCureMp(INT32 nValue)
{
	m_nTotalDrugCureMp += nValue;
}

inline void CCharacter::SetHpTap(INT32 nValue)
{
	m_nHpTap = nValue;
}

inline void CCharacter::SetMpTap(INT32 nValue)
{
	m_nMpTap = nValue;
}

inline void CCharacter::SetHpLeech(INT32 nValue)
{
	m_nCurrentHpLeech = nValue;
}

inline INT32 CCharacter::GetHpLeech(void)
{
	return m_nCurrentHpLeech;
}

inline void CCharacter::SetDamageAbsorb(INT32 nValue)
{
	m_nRemainDamageAbsorb = nValue > 0 ? nValue : 0;
}

inline INT32 CCharacter::GetRemainAbsorb(void)
{
	return m_nRemainDamageAbsorb;
}

inline BOOL CCharacter::AddDamageType(INT32 eDamageType)
{
	LOG_PROCESS_ERROR(eDamageType > spdtInvalid && eDamageType < spdtTotal);

	m_nDamagTypeBits |= (1 << eDamageType);

	return TRUE;
Exit0:
	return FALSE;
}

inline INT32 CCharacter::GetDamageTypeBits(void)
{
	return m_nDamagTypeBits;
}

inline INT32 CCharacter::GetAttr(INT32 eAttrType)
{
	//return m_nAttrTable[eAttrType];
	return m_AttrList.GetAttr(eAttrType);
}

inline void CCharacter::SetCanRun(BOOL bCanRun)
{
	m_bCanRun = bCanRun;
}

inline INT32 CCharacter::GetMoveState(void)
{
	return m_eMoveState;
}

inline INT64 CCharacter::GetDestX(void)
{
	return m_llDestX;
}

inline INT64 CCharacter::GetDestY(void)
{
	return m_llDestY;
}

inline INT32 CCharacter::GetHomeSceneID(void)
{
	return m_nHomeSceneID;
}

inline INT64 CCharacter::GetHomeX(void)
{
	return m_llHomeX;
}

inline INT64 CCharacter::GetHomeY(void)
{
	return m_llHomeY;
}

inline INT64 CCharacter::GetHomeZ(void)
{
	return m_llHomeZ;
}

inline INT32 CCharacter::GetMoveTimer(void)
{
	return m_nMoveTimer;
}

inline BOOL CCharacter::IsCasting(void)
{
	return m_poCastingSpell != NULL;
}

inline INT32 CCharacter::NextSpellSerial(void)
{
	return m_nSpellSerial++;
}

inline INT32 CCharacter::GetSpellSerial(void)
{
	return m_nSpellSerial;
}

inline UINT32 CCharacter::GetFocusID(void)
{
	return m_dwFocusID;
}

inline void CCharacter::SetHostID(UINT32 dwCharID)
{
    m_dwHostID = dwCharID;
}

inline UINT32 CCharacter::GetHostID(void)
{
	return m_dwHostID;
}

inline UINT32 CCharacter::GetExclusiveOwnerID(void)
{
	return m_dwExclusiveOwnerID;
}

inline void CCharacter::SetExclusiveOwnerID(UINT32 dwCharID)
{
	m_dwExclusiveOwnerID = dwCharID;
}

inline void CCharacter::SetDamageShareCharID(UINT32 dwID)
{
	m_dwDamageShareCharID = dwID;
}

inline UINT32 CCharacter::GetDamageShareCharID(void)
{
	return m_dwDamageShareCharID;
}


inline CAttrList& CCharacter::GetAttrList(void)
{
	return m_AttrList;
}

inline CCoolDown& CCharacter::GetCoolDown(void)
{
	return m_CoolDown;
}

#ifdef _ROBOT
inline void CCharacter::SetCastingSpell(CSpellInstance* poSpell, INT32 nCastingOverFrm)
{
	m_poCastingSpell = poSpell;
	m_nCastingOverFrm = nCastingOverFrm;
}
#endif

inline void CCharacter::ClearSubSpellDepth(void)
{
	m_nSubSpellDepth = 0;
}

inline CSpellInstance* CCharacter::GetCastingSpell(void)
{
	return m_poCastingSpell;
}

inline CSpellInstance* CCharacter::GetPendingSpell(void)
{
	SimpleListNode* pNode = m_PendingSpellStack.Top();
	PROCESS_ERROR(pNode);

	return node_cast(CSpellInstance*, GetStackNode(), pNode);
Exit0:
	return NULL;
}

inline CAuraInstance* CCharacter::GetPendingAura(void)
{
	SimpleListNode* pNode = m_PendingAuraStack.Top();
	PROCESS_ERROR(pNode);

	return node_cast(CAuraInstance*, GetStackNode(), pNode);
Exit0:
	return NULL;
}

inline SpellEventNode* CCharacter::GetPendingEventNode(void)
{
	SimpleListNode* pNode = m_PendingEventNodeStack.Top();
	PROCESS_ERROR(pNode);

	return node_cast(SpellEventNode*, m_StackNode, pNode);
Exit0:
	return NULL;
}

inline void CCharacter::PushSpell(CSpellInstance* poSpell)
{
	m_PendingSpellStack.Push(&poSpell->GetStackNode());
}

inline void CCharacter::PopSpell(void)
{
	m_PendingSpellStack.Pop();
}

inline void CCharacter::PushAura(CAuraInstance* poAura)
{
	m_PendingAuraStack.Push(&poAura->GetStackNode());
}

inline void CCharacter::PopAura(void)
{
	m_PendingAuraStack.Pop();
}

inline void CCharacter::PushEventNode(SpellEventNode* pNode)
{
	m_PendingEventNodeStack.Push(&pNode->m_StackNode);
}

inline void CCharacter::PopEventNode(void)
{
	m_PendingEventNodeStack.Pop();
}

inline CAuraList& CCharacter::GetAuraList(void)
{
	return m_AuraList;
}

inline UINT32 CCharacter::GetLastCasterID()
{
	return m_dwLastCasterID;
}

inline UINT32 CCharacter::GetLastCasterTimer()
{
    return m_dwLastCasterTimer;
}

inline UINT32 CCharacter::GetLastTargetID(void)
{
    return m_dwLastTargetID;
}

inline UINT32 CCharacter::GetLastTargetTimer(void)
{
    return m_dwLastTargetTimer;
}

inline void CCharacter::SetDropOwnerID(UINT32 dwID)
{
	m_dwDropOwnerID = dwID;
}

inline UINT32 CCharacter::GetDropOwnerID(void)
{
	return m_dwDropOwnerID;
}


inline INT32 CCharacter::GetSpellState(void)
{
	return m_eSpellState;
}

inline CFlagBits& CCharacter::GetFlag(void)
{
	return m_oFlag;
}

inline CFlagBits& CCharacter::GetExFlag(void)
{
	return m_oExFlag;
}

inline CFlagBits& CCharacter::GetCCFlag(void)
{
	return m_oCCFlag;
}

inline CFlagBits& CCharacter::GetStiffFlag(void)
{
	return m_oStiffFlag;
}

inline INT32 CCharacter::GetCurrentMountID(void)
{
	return m_nCurrentMountID;
}

inline INT32 CCharacter::GetCurrentWingID(void)
{
	return m_nCurrentWingID;
}

inline BOOL CCharacter::IncControlCount(INT32 nControlType)
{
	LOG_PROCESS_ERROR(nControlType >= cclBegin && nControlType < cclTotal);

	m_btControlCount[nControlType]++;

	m_oCCFlag.SetFlag(nControlType);

	return TRUE;
Exit0:
	return FALSE;
}

inline BOOL CCharacter::DecControlCount(INT32 nControlType)
{
	LOG_PROCESS_ERROR(nControlType >= cclBegin && nControlType < cclTotal);
//	LOG_PROCESS_ERROR(m_btControlCount[nControlType] > 0);
	if (m_btControlCount[nControlType] <= 0)
	{
		return FALSE;
	}

	m_btControlCount[nControlType]--;

	if(m_btControlCount[nControlType] <= 0)
	{
		m_oCCFlag.ClearFlag(nControlType);
	}

	return TRUE;
Exit0:
	return FALSE;
}

inline INT32 CCharacter::GetControlCount(INT32 nControlType)
{
	LOG_PROCESS_ERROR(nControlType > cclInvalid && nControlType < cclTotal);

	return m_btControlCount[nControlType];
Exit0:
	return 0;
}

inline void CCharacter::SetSeducerID(UINT32 dwID)
{
	m_dwSeducerID = dwID;
}

inline UINT32 CCharacter::GetSeducerID(void)
{
	return m_dwSeducerID;
}

//BOOL CCharacter::NewEventKey(INT32 eOccasionType, UINT32& dwEventKey)
//{
//	LOG_PROCESS_ERROR(eOccasionType > meoInvalid && eOccasionType < meoTotal);
//
//	if(m_dwEventGenerator >= 0x0FFFFFFF)
//		m_dwEventGenerator = 0;
//
//	m_dwEventGenerator++;
//
//	dwEventKey = (eOccasionType << 28) + m_dwEventGenerator;
//
//	return TRUE;
//Exit0:
//	return FALSE;
//}

inline BOOL CCharacter::NewEventKey(INT32 eOccasionID, INT32 eSource, INT32 nRegisterID, UINT64& qwEventKey)
{
	LOG_PROCESS_ERROR(eSource > sesInvalid && eSource < sesTotal);
	LOG_PROCESS_ERROR(eOccasionID > meoInvalid && eOccasionID < meoTotal);

	m_dwEventGenerator++;

	SpellEventKey Key;
	Key.uOccID = eOccasionID;
	Key.uSource = eSource;
	Key.uRegisterID = nRegisterID;
	Key.uGenerator = m_dwEventGenerator;

	qwEventKey = Key.qwEventKey;

	SpellEventKey Key1;

	Key1.qwEventKey = qwEventKey;

	return TRUE;
Exit0:
	return FALSE;
}

inline void CCharacter::SetPendingItem(Item* poItem)
{
	m_poPendingItem = poItem;
}

inline Item* CCharacter::GetPendingItem(void)
{
	return m_poPendingItem;
}

inline void CCharacter::SetPendingItemSlot(INT32 nSlot)
{
	m_nPendingItemSlot = nSlot;
}

inline INT32 CCharacter::GetPendingItemSlot(void)
{
	return m_nPendingItemSlot;
}

// Dead = TRUE, alive = FALSE
inline BOOL CCharacter::IsDead(void)
{
	return m_eMoveState == CMS_DIE;
}

inline CThreatList& CCharacter::GetThreatList(void)
{
    return m_ThreatList;
}

inline CAIProcessor * CCharacter::GetAIProcessor(void)
{
	return m_poAIProcessor;
}

inline INT32 CCharacter::GetOriginalAiID(void)
{
	return m_nOriginAiID;
}

inline void CCharacter::SetNewAiID(INT32 nID)
{
	m_nNewAiID = nID;
}

inline void CCharacter::SetGlobalCDDelayFrame(INT32 nFrame)
{
	m_nGlobalCDDelayFrame = nFrame;
}

inline INT32 CCharacter::GetGlobalCDDelayFrame(void)
{
	return m_nGlobalCDDelayFrame;
}


inline void CCharacter::SetCombatStatNode(COMBAT_STAT_NODE* pNode)
{
    m_pStatNode = pNode;
}

inline INT32 CCharacter::GetCombatStat(INT32 nField)
{
    LOG_PROCESS_ERROR(nField > escsInvalid && nField < escsTotal);
    LOG_PROCESS_ERROR(m_pStatNode);

    return m_pStatNode->m_Data[nField];
Exit0:
    return 0;
}

inline  void CCharacter::SetCombatStat(INT32 nField, INT32 nValue)
{
    PROCESS_ERROR(m_pStatNode);
    LOG_PROCESS_ERROR(nField > escsInvalid && nField < escsTotal);

    m_pStatNode->m_Data[nField] = nValue;
Exit0:
    return;
}

inline void CCharacter::IncCombatStat(INT32 nField, INT32 nValue)
{
    PROCESS_ERROR(m_pStatNode);
    LOG_PROCESS_ERROR(nField > escsInvalid && nField < escsTotal);

    m_pStatNode->m_Data[nField] += nValue;
Exit0:
    return;
}

inline void CCharacter::IncDamageStatCount(void)
{
	m_nDamageCount++;
}

inline void CCharacter::IncTotalDamage(INT32 nValue)
{
	m_nTotalDamage += nValue;
}

inline void CCharacter::ResetDamageStat(void)
{
	m_nDamageCount = 0;
	m_nTotalDamage = 0;
}

inline INT32 CCharacter::GetDamageStatCount(void)
{
	return m_nDamageCount;
}

inline INT32 CCharacter::GetTotalDamage(void)
{
	return m_nTotalDamage;
}

#ifdef _ROBOT
inline void CCharacter::SetMoveState(INT32 eMoveState)
{
	m_eMoveState = eMoveState;
}
#endif	// _ROBOT



#endif // _character_inl_h__