#include "stdafx.h"
#include "aura_instance.h"
#include "game_world.h"
#include "modifier_tree.h"
#include "character.h"
#include "level_data.h"
#include "message_handler.h"
#include "attr_conversion_data.h"
#include "guild_mgr.h"

CAuraInstance::OnTimeupEffectFunc CAuraInstance::m_TimeupFunc[] = 
{
	NULL,
	&CAuraInstance::_HotEffect,
	&CAuraInstance::_DotEffect,
};


CAuraInstance::CAuraInstance(void)
{
	m_dwCasterID = 0;
	m_dwInstanceID = 0;
	m_nStackCount = 0;
	m_nStrengthLevel = 0;
	m_bNeedSync = FALSE;
	m_bNeedRemove = FALSE;
	m_nTotalEffectValue = 0;

	m_nEffectPct = 0;
	m_nEffectValue = 0;

	m_nDestroyFrm = 0;
	m_llAtk = 0;
	memset(m_ExtraData, 0, sizeof(m_ExtraData));
	memset(m_nExEvtFrm, 0, sizeof(m_nExEvtFrm));
	m_ListNode.pPrev = NULL;
	m_ListNode.pNext = NULL;
	m_StackNode.pNext = NULL;

	m_nSpellID = 0;
	m_nSpellLevel = 0;
	m_nCharLevel = 0;
}

CAuraInstance::~CAuraInstance(void)
{
}

void CAuraInstance::UnInit(void)
{
	CGameWorld::GetInstance().DelAuraInstance(this);
}

BOOL CAuraInstance::Init(CAura* pInfo, ExtraParam* pExParam, CCharacter* poCaster, UINT32 dwFromObjID, UINT32 dwUniqueID, INT32 nNewStackCount)
{
	m_ListNode.pNext = NULL;
	m_ListNode.pPrev = NULL;
	m_StackNode.pNext = NULL;

#ifndef _ROBOT
	LOG_PROCESS_ERROR(poCaster);
	m_dwCasterID = poCaster->GetObjID();
#else
	m_dwCasterID = dwFromObjID;
#endif

	m_dwFromObjID = dwFromObjID;
	m_dwInstanceID = dwUniqueID;
	m_nStackCount = nNewStackCount;
	m_EventIdList.clear();
	m_bNeedRemove = FALSE;
	m_nStrengthLevel = pInfo->GetAttr(aatStrengthLevel);
	m_nSpellID = 0;
	m_nSpellLevel = 0;
	m_nTotalEffectValue = 0;
	m_nCharLevel = 0;

	m_nEffectPct = 0;
	m_nEffectValue = 0;
	memset(m_ExtraData, 0, sizeof(m_ExtraData));
	memset(m_nExEvtFrm, 0, sizeof(m_nExEvtFrm));

	pInfo->CopyTo(this);
	memset(&m_ExtraParam, 0, sizeof(m_ExtraParam));

	if(pExParam != NULL)
		m_ExtraParam = *pExParam;

	ResetFrm(pInfo->GetDurationFrm(), 0, FALSE);
	m_bNeedSync = FALSE;

	for(INT32 i = 0; i < MAX_AURA_EX_EVT; i++)
	{
		if(pInfo->GetData()->m_ExEvtPoint[i].m_nExEvtTime > 0)
			m_nExEvtFrm[i] = CGameWorld::GetInstance().GetGameFrame() + CONVERT_TIME(pInfo->GetData()->m_ExEvtPoint[i].m_nExEvtTime);
	}

#ifndef _ROBOT
	{
		CAttrList& AttrList = poCaster->GetAttrList();
		INT32 eSpellAtkType = GetAttr(aatAttackType);
		LOG_PROCESS_ERROR(eSpellAtkType > spatInvalid && eSpellAtkType < spatTotal);

		INT64 llAtkHi = AttrList.GetAttr(s_MaxAtkTable[eSpellAtkType]);
		INT64 llAtkLo = AttrList.GetAttr(s_AtkTable[eSpellAtkType]);

		LOG_PROCESS_ERROR(llAtkLo <= llAtkHi);

		INT32 nWeaponType = poCaster->GetWeaponType();
		LOG_PROCESS_ERROR(nWeaponType > WEAPONTYPE_INVALID && nWeaponType < WEAPONTYPE_TOTAL);

		llAtkHi = llAtkHi * g_GWConfig.BaseWeaponSpeed[nWeaponType] / RATE_MAX;
		llAtkLo = llAtkLo * g_GWConfig.BaseWeaponSpeed[nWeaponType] / RATE_MAX;

		m_llAtk = g_Random(llAtkLo, llAtkHi + 1);
	}
#endif

	return TRUE;
Exit0:
	WRN("AuraID: %d, CasterID: %u", pInfo->GetID(), dwFromObjID);
	return FALSE;
}

BOOL CAuraInstance::OnAppend(CCharacter* poCaster, CCharacter* pChar)
{
	INT32 nRetCode = 0;

//	m_Data.m_nAttrTable[aatEffectPct] += m_Data.m_nAttrTable[aatEffectPct] * m_Data.m_nAttrTable[aatStackEfficient] / RATE_MAX;
//	m_Data.m_nAttrTable[aatEffectValue] += m_Data.m_nAttrTable[aatEffectValue] * m_Data.m_nAttrTable[aatStackEfficient] / RATE_MAX;

	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(GetAttr(aatOnAppend), pChar, &m_ExtraParam);
	LOG_PROCESS_ERROR(nRetCode >= 0);

	poCaster->TrigEvent(meoOnAppendAuraAsCaster, NULL);
	pChar->TrigEvent(meoOnAppendAura, NULL);

	return TRUE;
Exit0:
	return FALSE;
}

//BOOL CAuraInstance::BeforeStack(CCharacter* pChar)
//{
//	INT32 nRetCode = 0;
//
//	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(GetAttr(aatBeforeStack), pChar, &m_ExtraParam);
//	LOG_CHECK_ERROR(nRetCode >= 0);
//
//	return TRUE;
//Exit0:
//	return FALSE;
//}

BOOL CAuraInstance::AfterStack(CCharacter* pChar)
{
	INT32 nRetCode = 0;

	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(GetAttr(aatAfterStack), pChar, &m_ExtraParam);
	LOG_CHECK_ERROR(nRetCode >= 0);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAuraInstance::OnUnStack(CCharacter* pChar)
{
	INT32 nRetCode = 0;

	nRetCode = CModifierTree::GetInstance().RollBackGlobalNode(GetAttr(aatAfterStack), pChar, &m_ExtraParam);
	LOG_CHECK_ERROR(nRetCode >= 0);

	return TRUE;
Exit0:
	return FALSE;

}

BOOL CAuraInstance::OnTimeup(CCharacter* pChar)
{
	INT32 nRetCode = 0;

	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(GetAttr(aatOnTimeup), pChar, &m_ExtraParam);
	LOG_PROCESS_ERROR(nRetCode >= 0);

	pChar->TrigEvent(meoOnAuraTimeup, NULL);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAuraInstance::OnRemove(CCharacter* pChar, BOOL bTrigOnRemove)
{
	INT32 nRetCode = 0;

	if(bTrigOnRemove)
	{
		nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(GetAttr(aatOnRemove), pChar, &m_ExtraParam);
		LOG_CHECK_ERROR(nRetCode >= 0);
	}

	for(INT32 i = 0; i < MAX_AURA_EX_EVT; i++)
	{
		if (m_nExEvtFrm[i] > 0)
			continue;

		nRetCode = CModifierTree::GetInstance().RollBackGlobalNode(m_Data.m_ExEvtPoint[i].m_nExOnEvt, pChar, &m_ExtraParam);
		LOG_CHECK_ERROR(nRetCode >= 0);
	}

	nRetCode = CModifierTree::GetInstance().RollBackGlobalNode(GetAttr(aatOnAppend), pChar, &m_ExtraParam);
	LOG_CHECK_ERROR(nRetCode >= 0);

	pChar->TrigEvent(meoOnRemoveAura, NULL);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAuraInstance::OnDispel(CCharacter* pChar)
{
	INT32 nRetCode = 0;

	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(GetAttr(aatOnDispel), pChar, &m_ExtraParam);
	LOG_PROCESS_ERROR(nRetCode >= 0);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CAuraInstance::Update(CCharacter* pChar, INT32 nFrm, BOOL& bTimeOut)
{
	INT32 nRetCode = 0;
	bTimeOut = FALSE;

	PROCESS_SUCCESS(m_bNeedRemove);

	if(m_nStackCount <= 0)
	{
		bTimeOut = TRUE;
		return TRUE;
	}

//	SPL("Aura Timeup : %d", m_Data.m_nID);

	for(INT32 i = 0; i < MAX_AURA_EX_EVT; i++)
	{
		if(m_nExEvtFrm[i] > 0 && nFrm >= m_nExEvtFrm[i])
		{
			nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(m_Data.m_ExEvtPoint[i].m_nExOnEvt, pChar, &m_ExtraParam);
			LOG_CHECK_ERROR(nRetCode >= 0);
			m_nExEvtFrm[i] = 0;
		}
	}

	if(m_IntervalTimer.Trig(nFrm))
	{
		nRetCode = OnTimeup(pChar);
		LOG_CHECK_ERROR(nRetCode);
	}

	if(!m_Data.m_nAttrTable[aatEternal] && nFrm >= m_nDestroyFrm)
		bTimeOut = TRUE;

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}

void CAuraInstance::ResetFrm(INT32 nDurationFrm, INT32 nIntervalFrm, BOOL bNeedSync)
{
	INT32 nDelayFrm = 0;

	if(nIntervalFrm == 0)
		nIntervalFrm = GetIntervalFrm();

	nDelayFrm = GetDelayFrm();

	INT32 nTrigTimes = nIntervalFrm == 0 ? 1 : 1 + (nDurationFrm - nDelayFrm) / nIntervalFrm;

	m_IntervalTimer.Enable(nIntervalFrm, nTrigTimes, CGameWorld::GetInstance().GetGameFrame(), nDelayFrm);

	m_nDestroyFrm = CGameWorld::GetInstance().GetGameFrame() + nDurationFrm;

	m_Data.m_nAttrTable[aatDuration] = nDurationFrm * 1000 / GAME_FPS;
	m_Data.m_nAttrTable[aatInterval] = nIntervalFrm * 1000 / GAME_FPS;

	m_bNeedSync = bNeedSync;
}

UINT32 CAuraInstance::GetRemainFrm(void)
{
	return m_nDestroyFrm - CGameWorld::GetInstance().GetGameFrame();
}

void CAuraInstance::DoTimeupEffect(CCharacter* poTarget, BOOL bRunFormular)
{
	INT32 nRetCode = 0;
	INT32 nRemainHp = 0;
	INT32 nCategory = GetAttr(aatCategory);

	if(bRunFormular)
	{
		LOG_PROCESS_ERROR(nCategory > acNone && nCategory < acTotal);
		LOG_PROCESS_ERROR(m_TimeupFunc[nCategory] != NULL);
		(this->*m_TimeupFunc[nCategory])(poTarget);
	}

	nRemainHp = poTarget->GetHp() + m_nTotalEffectValue;

	poTarget->TrigEvent(meoOnAuraEffect, NULL);

#ifndef _ROBOT

	if (!m_Data.m_nAttrTable[aatHiddenToSelf])
	{
		nRetCode = CCliMessageHandler::GetInstance().DoCharAuraEffect(poTarget, this);
		LOG_CHECK_ERROR(nRetCode);
	}
#endif  // _ROBOT

	poTarget->ChangeHp(m_nTotalEffectValue, cdrAura);
Exit0:
	return;
}

void CAuraInstance::_DotEffect(CCharacter* poTarget)
{
	INT32 eAttackType = 0;
	INT64 llAtkX = 0;
	INT32 nDamageTypeValue = 0;
	INT64 llA = 0;
	INT64 llB = 0;
	INT64 llDef = 0;
	INT32 nReduce = 0;
	INT32 nAbsorbValue = 0;
	INT32 nThreat = 0;

	CCharacter* poCaster = CGameWorld::GetInstance().FindValidCharacter(m_dwCasterID);
	LOG_PROCESS_ERROR(poCaster);

	if (!m_Data.m_nAttrTable[aatIgnoreDefense])
	{
		INT64 llG1 = g_Max(poTarget->GetAttr(s_MaxDefTable[eAttackType]) - poCaster->GetAttr(s_AntiDefTable[eAttackType]), 0);
		INT64 llDef = g_Max<INT64>(llG1 - poTarget->GetAttr(s_DefTable[eAttackType]), 0);
		llDef = g_Random(0, llDef + 1) + poTarget->GetAttr(s_DefTable[eAttackType]);

		m_nEffectValue = g_Max<INT32>(m_nEffectValue - llDef, 0);

		if (poTarget->GetSpecies() == spcUndead)
			m_nEffectValue += poCaster->GetAttr(catHolyDmg);

//		if (m_oEffectFlag.CheckFlag(sfbCrit))
//		{
//			m_nEffectValue += g_Max(poCaster->GetAttr(catCritDmg) - poTarget->GetAttr(catCritDmgResist), 0);
//		}
	}

	m_nTotalEffectValue = m_nEffectValue * (RATE_MAX - poTarget->GetAttr(catDmgResist)) / RATE_MAX;

	nAbsorbValue = poTarget->GetRemainAbsorb();

	if(m_nTotalEffectValue <= nAbsorbValue)
	{
		nAbsorbValue -= m_nTotalEffectValue;
		m_nTotalEffectValue = 0;
	}
	else
	{
		m_nTotalEffectValue -= nAbsorbValue;
		nAbsorbValue = 0;
	}

	nThreat = m_nTotalEffectValue * m_Data.m_nAttrTable[aatThreatPct] / RATE_MAX + m_Data.m_nAttrTable[aatThreatValue];
	nThreat = nThreat <= m_Data.m_nAttrTable[aatThreatMax] ? nThreat : m_Data.m_nAttrTable[aatThreatMax];

//lcl
//	poTarget->GetThreatList().ModifyThreatValue(poCaster, nThreat);

	{
		CRole* poCasterRole = NULL;
		CRole* poTargetRole = NULL;

		if (poCaster->GetHostID() != 0)
		{
			if (GetTypeFromID(poCaster->GetHostID()) == otRole)
				poCasterRole = CGameWorld::GetInstance().FindValidRole(poCaster->GetHostID());
		}
		else if (poCaster->GetObjType() == otRole)
		{
			poCasterRole = (CRole*)poCaster;
		}

		if (poTarget->GetHostID() != 0)
		{
			if (GetTypeFromID(poTarget->GetHostID()) == otRole)
				poTargetRole = CGameWorld::GetInstance().FindValidRole(poTarget->GetHostID());
		}
		else if (poTarget->GetObjType() == otRole)
		{
			poTargetRole = (CRole*)poTarget;
		}

		if (poCasterRole && poTargetRole)
		{
			BOOL bIsHostileGuild = FALSE;

			if (poCasterRole->GetGuildID() != INVALID_OBJECT_ID && poTargetRole->GetGuildID() != INVALID_OBJECT_ID &&
				CGuildMgr::GetInstance().IsHostile(poCasterRole->GetGuildID(), poTargetRole->GetGuildID()))
			{
				bIsHostileGuild = TRUE;
			}

			if (poCasterRole != poTargetRole && !poCasterRole->IsRedName() && !poTargetRole->IsRedName() && !bIsHostileGuild &&
				!poTargetRole->IsGreyName() && poTargetRole->GetGrid() && poTargetRole->GetGrid()->uPkType != gptFree)
			{
				poCasterRole->SetGreyName();
			}
		}
	}

	poCaster->IncCombatStat(escsTotalDamage, m_nTotalEffectValue);

	if (poCaster->GetFlag().CheckFlag(cfDamageStatistics))
	{
		poCaster->IncDamageStatCount();
		poCaster->IncTotalDamage(m_nTotalEffectValue);
	}

	poTarget->OnHurt(poCaster, m_nTotalEffectValue);

	m_nTotalEffectValue = -m_nTotalEffectValue;

	return;
Exit0:
	CRI("AuraID: %d, CharID: %d", GetID(), poTarget->GetObjID());
	return;
}

void CAuraInstance::_HotEffect(CCharacter* poTarget)
{
	INT32 nThreat = 0;
	INT32 eAttackType = 0;

	CCharacter* poCaster = CGameWorld::GetInstance().FindValidCharacter(m_dwCasterID);
	PROCESS_ERROR(poCaster);

//	eAttackType = GetAttr(aatAttackType);
//	LOG_PROCESS_ERROR(eAttackType > spatInvalid && eAttackType < spatTotal);

//	m_nTotalEffectValue = m_nEffectValue + m_nEffectPct * m_llAtk / RATE_MAX / RATE_MAX;

	m_nTotalEffectValue = m_nEffectValue;

	poCaster->IncCombatStat(escsTotalHeal, m_nTotalEffectValue);

	nThreat = m_nTotalEffectValue * m_Data.m_nAttrTable[aatThreatPct] / RATE_MAX + m_Data.m_nAttrTable[aatThreatValue];
	nThreat = nThreat <= m_Data.m_nAttrTable[aatThreatMax] ? nThreat : m_Data.m_nAttrTable[aatThreatMax];

//lcl
//	poTarget->GetThreatList().ModifyChainThreatValue(poCaster, nThreat);

Exit0:
	return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CAuraList::CAuraList(void)
{
//	memset(m_pStaticAura, NULL, sizeof(m_pStaticAura));
//	m_DynamicAuraList.Clear();
	memset(m_TopAuraDisplay, 0, sizeof(m_TopAuraDisplay));
}

CAuraList::~CAuraList(void)
{
	UnInit();
}

void CAuraList::Init(void)
{
//	memset(m_pStaticAura, NULL, sizeof(m_pStaticAura));
//	m_DynamicAuraList.Clear();

	for(INT32 i = auetNone; i < auetTotal; i++)
	{
		m_AuraList[i].Clear();
	}

	memset(m_TopAuraDisplay, 0, sizeof(m_TopAuraDisplay));
}

void CAuraList::UnInit(void)
{
	for(INT32 i = auetNone; i < auetTotal; i++)
	{
		if(!m_AuraList[i].IsEmpty())
		{
			BASE_LIST_NODE* pNextNode = NULL;

			for(BASE_LIST_NODE* pNode = m_AuraList[i].Head().pNext; pNode != &(m_AuraList[i].Rear()); pNode = pNextNode)
			{
				CAuraInstance* poAura = node_cast(CAuraInstance*, GetListNode(), pNode);
				pNextNode = pNode->pNext;

				poAura->UnInit();
			}

			m_AuraList[i].Clear();
		}
	}
}

BOOL CAuraList::AddNewAura(CAuraInstance* poAura)
{
	INT32 nRetCode = 0;
	INT32 nDisplayGroup = 0;
	INT32 nDisplayValue = 0;
	INT32 nEffectType = poAura->GetAttr(aatType);

	LOG_PROCESS_ERROR(nEffectType > auetInvalid && nEffectType < auetTotal);
	LOG_PROCESS_ERROR(poAura);

	nDisplayGroup = poAura->GetAttr(aatDisplayGroup);
	nDisplayValue = poAura->GetAttr(aatDisplayValue);

	m_AuraList[nEffectType].PushRear(&poAura->GetListNode());

//	if(nEffectType == auetNone)
//	{
//		nRetCode = m_DynamicAuraList.PushRear(&poAura->GetListNode());
//		goto Exit1;
//	}
//
//	//LOG_PROCESS_ERROR(m_pStaticAura[nEffectType] == NULL);
//
//	if(m_pStaticAura[nEffectType] != NULL)
//	{
//		LOG_PROCESS_ERROR(m_pStaticAura[nEffectType]->NeedRemove());
//		// temporary push "to be removed" aura to dynamic list, and it will be removed when updated.
//		m_DynamicAuraList.PushRear(&m_pStaticAura[nEffectType]->GetListNode());
//	}
//
//	m_pStaticAura[nEffectType] = poAura;

Exit1:
	if(nDisplayGroup > adgInvalid && nDisplayGroup < adgTotal)
	{
		if(nDisplayValue > m_TopAuraDisplay[nDisplayGroup].nAuraDisplayValue)
		{
			m_TopAuraDisplay[nDisplayGroup].nAuraID = poAura->GetID();
			m_TopAuraDisplay[nDisplayGroup].nAuraDisplayValue = nDisplayValue;
			m_TopAuraDisplay[nDisplayGroup].nInstanceID = poAura->GetInstanceID();
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

//CAuraInstance* CAuraList::BeginDynamicAura(void)
//{
//	return node_cast(CAuraInstance*, GetListNode(), &m_DynamicAuraList.Head());
//Exit0:
//	return NULL;
//}
//
//CAuraInstance* CAuraList::NextDynamicAura(CAuraInstance* poPrevAura)
//{
//	CAuraInstance* poAura = NULL;
//
//	BASE_LIST_NODE* pNode = &poPrevAura->GetListNode();
//
//	PROCESS_ERROR(poPrevAura);
//
//	LOG_PROCESS_ERROR(pNode != &m_DynamicAuraList.Rear());
//
//	do 
//	{
//		pNode = pNode->pNext;
//		LOG_PROCESS_ERROR(pNode != NULL);
//		PROCESS_ERROR(pNode != &m_DynamicAuraList.Rear());
//
//		poAura = node_cast(CAuraInstance*, GetListNode(), pNode);
//		LOG_PROCESS_ERROR(poAura != NULL);
//	}
//	while(poAura->NeedRemove());
//
//	return poAura;
//Exit0:
//	return NULL;
//}

CAuraInstance* CAuraList::BeginAura(INT32 nEffectType)
{
	LOG_PROCESS_ERROR(nEffectType > auetInvalid && nEffectType < auetTotal);

	return node_cast(CAuraInstance*, GetListNode(), &m_AuraList[nEffectType].Head());
Exit0:
	return NULL;
}

CAuraInstance* CAuraList::NextAura(INT32 nEffectType, CAuraInstance* poPrevAura)
{
	CAuraInstance* poAura = NULL;
	BASE_LIST_NODE* pNode = NULL;

	PROCESS_ERROR(poPrevAura);

	pNode = &poPrevAura->GetListNode();

	LOG_PROCESS_ERROR(nEffectType > auetInvalid && nEffectType < auetTotal);
	LOG_PROCESS_ERROR(pNode != &m_AuraList[nEffectType].Rear());

	do 
	{
		pNode = pNode->pNext;
		LOG_PROCESS_ERROR(pNode != NULL);
		PROCESS_ERROR(pNode != &m_AuraList[nEffectType].Rear());

		poAura = node_cast(CAuraInstance*, GetListNode(), pNode);
		LOG_PROCESS_ERROR(poAura != NULL);
	}
	while(poAura->NeedRemove());

	return poAura;
Exit0:
	return NULL;

}


void CAuraList::Update(CCharacter* poOwner)
{
	for(INT32 i = auetNone; i < auetTotal; i++)
	{
		BASE_LIST_NODE* pNextNode = NULL;

		if(m_AuraList[i].IsEmpty())
			continue;

		for(BASE_LIST_NODE* pNode = m_AuraList[i].Head().pNext; pNode != &(m_AuraList[i].Rear()); pNode = pNextNode)
		{
			CAuraInstance* poAura = node_cast(CAuraInstance*, GetListNode(), pNode);
			pNextNode = pNode->pNext;

			INT32 nRetCode = poOwner->UpdateSingleAura(poAura, FALSE);
			LOG_CHECK_ERROR(nRetCode);

			if(poAura->NeedRemove())
			{
				CBaseList::Remove(&poAura->GetListNode());
				poAura->UnInit();
			}
		}
	}
}

void CAuraList::RemoveAura(CAuraInstance* poAura)
{
	INT32 nDisplayGroup = poAura->GetAttr(aatDisplayGroup);
	INT32 nDisplayValue = poAura->GetAttr(aatDisplayValue);

	CAuraInstance* poIterAura = NULL;

	poAura->SetRemoveFlag(TRUE);

	PROCESS_ERROR(nDisplayGroup > adgInvalid && nDisplayGroup < adgTotal);
	PROCESS_SUCCESS(m_TopAuraDisplay[nDisplayGroup].nAuraID != poAura->GetID());

	memset(&m_TopAuraDisplay[nDisplayGroup], 0, sizeof(AuraDisplay));

//	for(INT32 i = auetNone; i < auetTotal; i++)
//	{
//		poIterAura = GetStaticAura(i);
//		if(poIterAura == NULL)
//			continue;
//
//		if(poIterAura->GetAttr(aatDisplayGroup) == nDisplayGroup && poIterAura->GetAttr(aatDisplayValue) >= nDisplayValue)
//		{
//			m_TopAuraDisplay[nDisplayGroup].nAuraID = poIterAura->GetID();
//			m_TopAuraDisplay[nDisplayGroup].nAuraID = poIterAura->GetInstanceID();
//			m_TopAuraDisplay[nDisplayGroup].nAuraDisplayValue = poIterAura->GetAttr(aatDisplayValue);
//		}
//	}
	for(INT32 i = auetNone; i < auetTotal; i++)
	{
		poIterAura = BeginAura(i);

		while((poIterAura = NextAura(i, poIterAura)) != NULL)
		{
			if(poIterAura->GetAttr(aatDisplayGroup) == nDisplayGroup && poIterAura->GetAttr(aatDisplayValue) >= nDisplayValue)
			{
				m_TopAuraDisplay[nDisplayGroup].nAuraID = poIterAura->GetID();
				m_TopAuraDisplay[nDisplayGroup].nInstanceID = poIterAura->GetInstanceID();
				m_TopAuraDisplay[nDisplayGroup].nAuraDisplayValue = poIterAura->GetAttr(aatDisplayValue);
			}
		}
	}

Exit1:
Exit0:
	return;
}

