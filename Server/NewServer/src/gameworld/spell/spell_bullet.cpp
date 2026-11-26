#include "stdafx.h"
#include "spell_bullet.h"
#include "spell_data.h"
#include "character.h"
#include "game_world.h"
#include "message_handler.h"
#include "modifier_tree.h"
#include "spell_instance.h"
#include "missile.h"
#include "attr_conversion_data.h"
#include "message_handler.h"
#include "server_config.h"
#include "guild_mgr.h"
#include "level_damage_mod_data.h"

CBullet::CBullet(void)
{
	memset(&m_SpellData, 0, sizeof(m_SpellData));

	m_ListNode.pNext = NULL;
	m_ListNode.pPrev = NULL;

	m_dwCasterCharID = 0;
	m_dwTimeupFrm = 0;
	m_dwFromObjID = 0;
	m_nDelayFrm = 0;
	m_poTarget = NULL;
	m_bRemoveFlag = FALSE;
	m_nDodgeRate = 0;
	m_nParryRate = 0;
	m_nBlockRate = 0;
	m_nCritRate = 0;

	m_bFrontAttack = TRUE;
	m_bFromMissile = FALSE;
	m_bTargetLocked = FALSE;

	m_poEventOwner = NULL;

	m_nAtkHi = 0;
	m_nAtkLo = 0;
	m_llAtkRandom = 0;
	m_llAttackX = 0;
	//m_llAttackX2 = 0;
	//m_llAttackX3 = 0;
	m_nY3 = 0;
	m_llEffectValue = 0;
	m_nSpellEffectValue = 0;

	m_llTargetX = 0;
	m_llTargetY = 0;
	m_llTargetZ = 0;

	m_nParam1 = 0;
	m_nParam2 = 0;
	m_nSpellSerial = 0;
	m_nCastSerial = 0;
	m_nCasterLevel = 0;
	m_nStiffFrm = 0;
	m_nStiffRandomNum = 0;
	m_nThreatValue = 0;
	m_nEffectSerial = 0;
	m_nTargetHpChangeValue = 0;
	m_nIndex = 0;
	m_eBulletType = 0;
	m_nHitRecovery = 0;

	m_pCasterLevel = NULL;
	m_pTargetLevel = NULL;
	m_CreateFunc[betDamage] = &CBullet::_OnCreateDamage;
	m_CreateFunc[betHeal] = &CBullet::_OnCreateHeal;
	m_CreateFunc[betNone] = &CBullet::_OnCreateNone;

	m_RollFunc[spjtDirectly] = NULL;
	m_RollFunc[spjtPhy] = &CBullet::_OnRollPhy;
	m_RollFunc[spjtMag] = &CBullet::_OnRollMag;

	m_TimeupFunc[betDamage] = &CBullet::_OnTimeupDamage;
	m_TimeupFunc[betHeal] = &CBullet::_OnTimeupHeal;
	m_TimeupFunc[betNone] = &CBullet::_OnTimeupNone;

	m_AfterEffectFunc[betDamage] = &CBullet::_AfterDamageEffectProcess;
	m_AfterEffectFunc[betHeal] = &CBullet::_AfterHealEffectProcess;
	m_AfterEffectFunc[betNone] = &CBullet::_AfterNoneEffectProcess;
}

CBullet::~CBullet(void)
{
}


BOOL CBullet::Init(INT32 eEffectType, INT32 nCastSerial, INT32 nEffectSerial, INT32 nIndex, INT32 nDelayFrm)
{
	LOG_PROCESS_ERROR(eEffectType > betInvalid && eEffectType < betTotal);

	memset(&m_SpellData, 0, sizeof(m_SpellData));
	memset(&m_ExParam, 0, sizeof(m_ExParam));

	m_ListNode.pNext = NULL;
	m_ListNode.pPrev = NULL;

	m_eBulletType = eEffectType;
	m_dwCasterCharID = 0;
	m_dwTimeupFrm = 0;
	m_dwFromObjID = 0;
	m_nDelayFrm = 0;
	m_poTarget = NULL;
	m_bRemoveFlag = FALSE;
	m_nDodgeRate = 0;
	m_nParryRate = 0;
	m_nBlockRate = 0;
	m_nCritRate = 0;
	m_bFromMissile = FALSE;
	m_bTargetLocked = FALSE;

	m_oEffectFlag.ResetFlag();
	m_oSpellFlag.ResetFlag();

	m_poEventOwner = NULL;

	m_nAtkHi = 0;
	m_nAtkLo = 0;
	m_llAtkRandom = 0;
	m_llAttackX = 0;

	m_nY3 = 0;
	m_llEffectValue = 0;
	m_nSpellEffectValue = 0;

	m_nParam1 = 0;
	m_nParam2 = 0;
	m_nSpellSerial = 0;
	m_nCasterLevel = 0;
	m_nIndex = 0;
	m_nStiffFrm = 0;
	m_nStiffRandomNum = 0;
	m_nThreatValue = 0;
	m_nCastSerial = nCastSerial;
	m_nEffectSerial = nEffectSerial;
	m_nTargetHpChangeValue = 0;
	m_nIndex = nIndex;

	m_llTargetX = 0;
	m_llTargetY = 0;
	m_llTargetZ = 0;

	m_pCasterLevel = NULL;
	m_pTargetLevel = NULL;

	m_nDelayFrm = nDelayFrm;
	m_dwTimeupFrm = CGameWorld::GetInstance().GetGameFrame() + m_nDelayFrm;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBullet::_OnCreate(const SPELL_ENTRY* data, const ExtraParam* exparam, CCharacter* poCaster, CCharacter* poTarget, CSceneObject* poFromObj)
{
	INT32 nRetCode = 0;
	INT32 nMissRand = 0;
	INT32 nSpeed = data->m_nAttrTable[satSpeed];
	INT32 nAtkParam = 0;

	LOG_PROCESS_ERROR(poCaster);
	LOG_PROCESS_ERROR(poTarget);

	memcpy(&m_SpellData, data, sizeof(m_SpellData));

	m_dwCasterCharID = poCaster->GetObjID();
	m_nCasterLevel = poCaster->GetLevel();
	m_dwFromObjID = poFromObj == NULL ? 0 : poFromObj->GetObjID();

	m_poTarget = poTarget;

	m_pCasterLevel = CDataManager<ATTR_LEVEL>::GetInstance().GetData(MakeAttrKey(poCaster->GetLevel(), poCaster->GetClass()));
	LOG_PROCESS_ERROR(m_pCasterLevel);

	m_pTargetLevel = CDataManager<ATTR_LEVEL>::GetInstance().GetData(MakeAttrKey(poTarget->GetLevel(), poTarget->GetClass()));
	LOG_PROCESS_ERROR(m_pTargetLevel);

	if(exparam != NULL)
		memcpy(&m_ExParam, exparam, sizeof(m_ExParam));

	LOG_PROCESS_ERROR(m_SpellData.m_nAttrTable[satAttackType] > spatInvalid && m_SpellData.m_nAttrTable[satAttackType] < spatTotal);

	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(m_SpellData.m_nAttrTable[satOnCreateBullet], this, &m_ExParam);
	LOG_CHECK_ERROR(nRetCode >= 0);

	poCaster->TrigBulletEvent(meoOnBulletCreate, this);

	nMissRand = g_Random(0, RATE_MAX);

	{
		INT32 nRandEffectValue = g_Random(m_SpellData.m_nAttrTable[satEffectValue], m_SpellData.m_nAttrTable[satEffectValueMax]);
		INT32 nRandDefEffectValue = g_Random(m_SpellData.m_nAttrTable[satDefaultValue], m_SpellData.m_nAttrTable[satDefaultValueMax]);

		m_nSpellEffectValue = round_div(nRandEffectValue * (m_SpellData.m_nLevel + 1), 4) + nRandDefEffectValue;
	}

	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(m_SpellData.m_nAttrTable[satBeforeRoll], this, &m_ExParam);
	LOG_CHECK_ERROR(nRetCode >= 0);

	poCaster->TrigBulletEvent(meoBeforeRoll, this);

	LOG_PROCESS_ERROR(m_CreateFunc[m_eBulletType] != NULL);
	(this->*m_CreateFunc[m_eBulletType])(poCaster, poTarget);

	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(m_SpellData.m_nAttrTable[satAfterRoll], this, &m_ExParam);
	LOG_CHECK_ERROR(nRetCode >= 0);

	poCaster->TrigBulletEvent(meoAfterRollAsCaster, this);
	poTarget->TrigBulletEvent(meoAfterRollAsTarget, this);

	nRetCode = poTarget->PushBullet(this);
	PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBullet::OnCreate(CSpellInstance* poSpell, CCharacter* poCaster, CCharacter* poTarget, CSceneObject* poFromObj, BOOL bTargetLocked)
{
	INT32 nRetCode = 0;
	m_nStiffFrm = poSpell->GetStiffFrm();
	m_oSpellFlag = poSpell->GetFlagBits();
	m_nSpellSerial = poSpell->GetSpellSerial();

	poSpell->GetScanner().GetTargetPt(m_llTargetX, m_llTargetX, m_llTargetZ);

	nRetCode = _OnCreate(poSpell->GetData(), poSpell->GetExParam(), poCaster, poTarget, poFromObj);
	PROCESS_ERROR(nRetCode);

	if(m_oEffectFlag.CheckFlag(sfbNormal) || m_oEffectFlag.CheckFlag(sfbCrit))
	{
		poSpell->HitTarget(TRUE);

		if (poTarget->GetObjType() == otRole)
			poSpell->HitRole(TRUE);
	}

	m_bTargetLocked = bTargetLocked;

	poSpell->AddBullet(this);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CBullet::OnCreate(CMissile* poMissile, const ExtraParam* exparam, CCharacter* poCaster, CCharacter* poTarget, CSceneObject* poFromObj, BOOL bTargetLocked)
{
	INT32 nRetCode = 0;

	m_bFromMissile = TRUE;

	m_nSpellSerial = poMissile->GetSpellSerial();

	nRetCode = _OnCreate(poMissile->GetSpellData(), exparam, poCaster, poTarget, poFromObj);
	LOG_PROCESS_ERROR(nRetCode);

	if(m_oEffectFlag.CheckFlag(sfbNormal) || m_oEffectFlag.CheckFlag(sfbCrit))
	{
		poMissile->HitTarget(TRUE);
	}

	m_bTargetLocked = bTargetLocked;

	poMissile->AddBullet(this);

	return TRUE;
Exit0:
	return FALSE;
}

void CBullet::_OnCreateDamage(CCharacter* poCaster, CCharacter* poTarget)
{
	INT32 eAttackType = m_SpellData.m_nAttrTable[satAttackType];
	INT32 eJudgeType = m_SpellData.m_nAttrTable[satJudgeType];

	LOG_PROCESS_ERROR(poCaster);
	LOG_PROCESS_ERROR(poTarget);
	LOG_PROCESS_ERROR(eAttackType > spatInvalid && eAttackType < spatTotal);
	LOG_PROCESS_ERROR(eJudgeType > spjtInvalid && eJudgeType < spjtTotal);

	{
		INT32 nLuck = poCaster->GetAttr(catLuck);

		INT32 nLuckRand1 = g_Random(0, LUCK_LIMIT + 1 - g_Min(nLuck, LUCK_LIMIT));
		INT32 nLuckRand2 = g_Random(0, LUCK_LIMIT + 1 - g_Max(-nLuck, 0));

		INT32 nCharAtk = poCaster->GetAttr(s_AtkTable[eAttackType]);
		INT32 nCharAtkMax = poCaster->GetAttr(s_MaxAtkTable[eAttackType]);

		static INT32 s_ParamE[] =
		{
			0, 0, 1, 1,
		};

		INT32 nParamE = nCharAtkMax - nCharAtk + s_ParamE[eAttackType];
		nParamE = nParamE >= 0 ? nParamE : 0;

		m_llAttackX = m_nSpellEffectValue + nCharAtk;

		if (nLuck >= 0)
		{
			if (nLuckRand1 == 0)
				m_llAttackX += nParamE;
			else
				m_llAttackX += g_Random(0, nParamE + 1);
		}
		else
		{
			if (nLuckRand2 != 0)
				m_llAttackX += g_Random(0, nParamE + 1);
		}

		if (poTarget->GetObjType() == otNpc && (m_SpellData.m_nAttrTable[satAttackType] == spatMag || m_SpellData.m_nAttrTable[satAttackType] == spatTao))
		{
			if (m_SpellData.m_nAttrTable[satAffectedByNpcParam])
			{
				LOG_PROCESS_ERROR(g_GWConfig.nDmgToNpcParam2 > 0);
				m_llAttackX = m_llAttackX * g_GWConfig.nDmgToNpcParam1 / g_GWConfig.nDmgToNpcParam2;
			}
		}
	}

	if(!poTarget->GetCCFlag().CheckFlag(cclInvincible))
		poTarget->GetThreatList().ModifyThreatValue(poCaster, 1);

	if(m_RollFunc[eJudgeType] != NULL)
		(this->*m_RollFunc[eJudgeType])(poCaster, poTarget);

	PROCESS_ERROR(!m_oEffectFlag.CheckFlag(sfbDodge));

	if (g_Random(0, RATE_MAX) <= g_Max(0, poCaster->GetAttr(catCrit) - poTarget->GetAttr(catCritResist)))
	{
		m_oEffectFlag.SetFlag(sfbCrit);
	}
	else
		m_oEffectFlag.SetFlag(sfbNormal);


	return;
Exit0:
	return;
}

void CBullet::_OnCreateHeal(CCharacter* poCaster, CCharacter* poTarget)
{
	INT32 eAttackType = m_SpellData.m_nAttrTable[satAttackType];

	LOG_PROCESS_ERROR(poCaster);
	LOG_PROCESS_ERROR(poTarget);
	LOG_PROCESS_ERROR(eAttackType > spatInvalid && eAttackType < spatTotal);

	PROCESS_ERROR(poTarget->GetHp() < poTarget->GetAttr(catMaxHp));

	{
		INT32 nLuck = poCaster->GetAttr(catLuck);

		INT32 nCharAtk = poCaster->GetAttr(catTaoAtk);
		INT32 nCharAtkMax = poCaster->GetAttr(catMaxTaoAtk);

		INT32 nParamE = 2 * (nCharAtkMax - nCharAtk) + 1;
		nParamE = nParamE >= 0 ? nParamE : 0;

		m_llAttackX = m_nSpellEffectValue + 2 * nCharAtk;

		m_llAttackX = LuckGo(m_llAttackX, nLuck, nParamE);
	}

	m_oEffectFlag.SetFlag(sfbEmptyEffect);

	return;
Exit0:
	return;
}

void CBullet::Update(CCharacter* poTarget, INT32 nFrm)
{
	PROCESS_ERROR(!m_bRemoveFlag);
	LOG_PROCESS_ERROR(poTarget);

	if(!poTarget->IsDead())
	{
		if (nFrm < m_dwTimeupFrm)
			return;

		OnTimeup(poTarget, nFrm);
	}

Exit0:
	SetRemoveFlag(TRUE);
	return;
}

void CBullet::UnInit(void)
{
	memset(&m_SpellData, 0, sizeof(m_SpellData));
	m_poTarget = NULL;

	CGameWorld::GetInstance().DelBullet(this);
}

void CBullet::OnTimeup(CCharacter* poTarget, INT32 nFrm)
{
	INT32 nRetCode = 0;

	if(poTarget->IsDead())
		return;

	if(poTarget->GetCCFlag().CheckFlag(cclInvincible))
	{
		m_oEffectFlag.ResetFlag();
		m_oEffectFlag.SetFlag(sfbImmune);
	}

	CCharacter* poCaster = CGameWorld::GetInstance().FindValidCharacter(m_dwCasterCharID);
	LOG_PROCESS_ERROR(poCaster);

	if (!m_oEffectFlag.CheckFlag(sfbMiss) && !m_oEffectFlag.CheckFlag(sfbDodge) && !m_oEffectFlag.CheckFlag(sfbImmune))
	{
		nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(m_SpellData.m_nAttrTable[satBeforeEffect], this, &m_ExParam);
		LOG_CHECK_ERROR(nRetCode >= 0);

		poCaster->TrigBulletEvent(meoOnBulletHitAsCaster, this);
		poTarget->TrigBulletEvent(meoOnBulletHitAsTarget, this);

		m_nThreatValue = m_llAttackX * m_SpellData.m_nAttrTable[satThreatPct] / RATE_MAX + m_SpellData.m_nAttrTable[satThreatValue];
		m_nThreatValue = m_nThreatValue <= m_SpellData.m_nAttrTable[satThreatMax] ? m_nThreatValue : m_SpellData.m_nAttrTable[satThreatMax];

		LOG_PROCESS_ERROR(m_TimeupFunc[m_eBulletType] != NULL);
		(this->*m_TimeupFunc[m_eBulletType])(poTarget, poCaster, nFrm);

		nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(m_SpellData.m_nAttrTable[satAfterEffect], this, &m_ExParam);
		LOG_CHECK_ERROR(nRetCode >= 0);

		poCaster->TrigBulletEvent(meoAfterDamageAsCaster, this);
		poTarget->TrigBulletEvent(meoAfterDamageAsTarget, this);

		LOG_PROCESS_ERROR(m_AfterEffectFunc[m_eBulletType] != NULL);
		(this->*m_AfterEffectFunc[m_eBulletType])(poTarget, poCaster, nFrm);
	}



	m_nParam1 = m_nTargetHpChangeValue;

	if(!m_oEffectFlag.CheckFlag(sfbMiss))
	{
		poTarget->TrigBulletEvent(meoBeforeApplyDamageAsTarget, this);
		poTarget->ChangeHp(m_nTargetHpChangeValue, cdrSpell);
	}

	if(poTarget->GetMoveState() != CMS_DIE)
		m_oEffectFlag.ClearFlag(sfbDead);

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoCharSpellEffect(poTarget, this);
	LOG_CHECK_ERROR(nRetCode);

	if(poCaster->GetObjType() == otRole)
	{
		CRole* poRole = static_cast<CRole*>(poCaster);

		CItemBag* poBag = poRole->GetItemBag();
		LOG_PROCESS_ERROR(poBag);

		if (IsPhysicalSpellAttack(m_SpellData.m_nAttrTable[satAttackType]))
			poBag->OnPhyAttack();

		//nRetCode = CCliMessageHandler::GetInstance().DoDLSpellEffect((CRole*)poCaster, GetSpellID(), GetSpellLevel(), GetSpellRuneSlot(), poTarget->GetObjID(), m_nSpellSerial, m_nParam1, m_nParam2, m_oEffectFlag.GetFlag());
		LOG_CHECK_ERROR(nRetCode);
	}

#endif  // _ROBOT


Exit0:
	return;
}

void CBullet::_OnTimeupDamage(CCharacter* poTarget, CCharacter* poCaster, INT32 nFrm)
{
	const ATTR_LEVEL* pLevel = NULL;

	// damage type modification:
	INT32 eAttackType = GetSpellAttr(satAttackType);
	LOG_PROCESS_ERROR(eAttackType > spatInvalid && eAttackType < spatTotal);

	{
		INT64 llG1 = g_Max(poTarget->GetAttr(s_MaxDefTable[eAttackType]) - poCaster->GetAttr(s_AntiDefTable[eAttackType]), 0);
		INT64 llDef = llG1 - poTarget->GetAttr(s_DefTable[eAttackType]);
		if (llDef >= 0)
			llDef = g_Random(0, llDef + 1) + poTarget->GetAttr(s_DefTable[eAttackType]);
		else
			llDef = g_Random(0, g_Abs(llDef) + 1) + llG1;

		if (m_SpellData.m_nAttrTable[satIgnoreDefense])
			llDef = 0;

		m_llEffectValue = g_Max<INT64>(m_llAttackX - llDef, 0);

		if (poTarget->GetSpecies() == spcUndead)
			m_llEffectValue += poCaster->GetAttr(catHolyDmg);

		poTarget->SetLastCasterID(poCaster->GetObjID());

		poCaster->TrigBulletEvent(meoAfterApplyDefenceAsCaster, this);
		poTarget->TrigBulletEvent(meoAfterApplyDefenceAsTarget, this);

		if (m_oEffectFlag.CheckFlag(sfbCrit))
		{
			m_llEffectValue += g_Max(poCaster->GetAttr(catCritDmg) - poTarget->GetAttr(catCritDmgResist), 0);
		}

		poCaster->TrigBulletEvent(meoBeforeDamageEnhancedAsCaster, this);
		poTarget->TrigBulletEvent(meoBeforeDamageEnhancedAsTarget, this);

		m_llEffectValue = m_llEffectValue * (RATE_MAX + poTarget->GetAttr(catHurtEnhanced)) * (RATE_MAX + poCaster->GetAttr(catDmgEnhance)) * (RATE_MAX - poTarget->GetAttr(catDmgResist)) / RATE_MAX / RATE_MAX / RATE_MAX;
	}

	poTarget->GetThreatList().ModifyThreatValue(poCaster, m_nThreatValue);

	if (poCaster->GetFlag().CheckFlag(cfDamageStatistics))
	{
		poCaster->IncDamageStatCount();
		poCaster->IncTotalDamage(m_llEffectValue);
	}
Exit0:
	return;
}

void CBullet::_OnTimeupHeal(CCharacter* poTarget, CCharacter* poCaster, INT32 nFrm)
{
	m_llEffectValue = m_llAttackX;

//lcl
//	CThreatList& ThreatList = poTarget->GetThreatList();
//	ThreatList.ModifyChainThreatValue(poCaster, m_nThreatValue);
}

void CBullet::_AfterDamageEffectProcess(CCharacter* poTarget, CCharacter* poCaster, INT32 nFrm)
{
    INT32 nAbsorbValue = 0;
	INT32 nRetCode = 0;

	// damage absorb process;
	nAbsorbValue = poTarget->GetRemainAbsorb();

	if(m_llEffectValue <= nAbsorbValue)
	{
		nAbsorbValue -= m_llEffectValue;
		m_nTargetHpChangeValue = 0;
	}
	else
	{
		m_nTargetHpChangeValue = m_llEffectValue - nAbsorbValue;
		nAbsorbValue = 0;
	}

	poTarget->SetDamageAbsorb(nAbsorbValue);

	// combat stat begin
	poCaster->IncCombatStat(escsTotalDamage, m_nTargetHpChangeValue);

	if(m_oEffectFlag.CheckFlag(sfbCrit))
	{
		if(IsPhysicalSpellAttack(m_SpellData.m_nAttrTable[satAttackType]))
			poCaster->IncCombatStat(escsPhyAtkCritCount, 1);
		else if(m_SpellData.m_nAttrTable[satAttackType] == spatMag)
			poCaster->IncCombatStat(escsMagAtkCritCount, 1);
	}
	// combat stat end

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

	m_nStiffRandomNum = g_Random(0, RATE_MAX);

	poTarget->OnHurt(poCaster, this, m_nTargetHpChangeValue);

	if(m_nStiffFrm > 0)
	{
		INT32 nDirection = g_GetDirection(poCaster->GetX(), poCaster->GetY(), poTarget->GetX(), poTarget->GetY());
		nRetCode = poTarget->Stiff(TRUE, m_nStiffFrm, nDirection);
		LOG_CHECK_ERROR(nRetCode);
	}

	m_nTargetHpChangeValue = -m_nTargetHpChangeValue;
	return;
}

void CBullet::_AfterHealEffectProcess(CCharacter* poTarget, CCharacter* poCaster, INT32 nFrm)
{
	if (poTarget->GetObjType() != otNpc || ((CNpc*)poTarget)->GetPetType() != 0)
	{
		poTarget->ChangeTotalSpellHpCure(m_llEffectValue);
		poCaster->IncCombatStat(escsTotalHeal, m_llEffectValue);
	}
}

void CBullet::_OnCreateNone(CCharacter* poCaster, CCharacter* poTarget)
{
	m_oEffectFlag.SetFlag(sfbEmptyEffect);
}

void CBullet::_OnTimeupNone(CCharacter* poTarget, CCharacter* poCaster, INT32 nFrm)
{
	poTarget->GetThreatList().ModifyThreatValue(poCaster, m_SpellData.m_nAttrTable[satThreatValue]);
}

void CBullet::_AfterNoneEffectProcess(CCharacter* poTarget, CCharacter* poCaster, INT32 nFrm)
{

}

void CBullet::_OnRollPhy(CCharacter* poCaster, CCharacter* poTarget)
{
	if (g_Random(poTarget->GetAttr(catAgility)) >= poCaster->GetAttr(catAccurate))
		m_oEffectFlag.SetFlag(sfbDodge);
}

void CBullet::_OnRollMag(CCharacter* poCaster, CCharacter* poTarget)
{
	if (g_Random(RATE_MAX) <= poTarget->GetAttr(catMagDodge))
		m_oEffectFlag.SetFlag(sfbDodge);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CRoundTable::CRoundTable(void)
{
	m_nMaxRate = 0;
	m_nRate = 0;
	m_nNodeCount = 0;
	m_nDefaultKey = 0;
	memset(&m_Nodes, 0, sizeof(m_Nodes));
}

BOOL CRoundTable::AddNode(INT32 nKey, INT32 nRate)
{
	LOG_PROCESS_ERROR(m_nNodeCount < MAX_TABLE_NODES);
	//PROCESS_ERROR(m_nRate + nRate <= m_nMaxRate);
	if(nRate == 0)
		return TRUE;

	nRate = m_nRate + nRate >= m_nMaxRate ? m_nMaxRate - m_nRate : nRate;

	m_Nodes[m_nNodeCount].nNodeKey = nKey;
	m_Nodes[m_nNodeCount].nMin = m_nRate;
	m_Nodes[m_nNodeCount].nMax = m_nRate + nRate - 1;

	m_nRate = m_nRate + nRate;
	m_nNodeCount++;

	return TRUE;
Exit0:
	return FALSE;
}

INT32 CRoundTable::Roll(void)
{
	INT32 nRand = g_Random(m_nMaxRate);

	for(INT32 i = 0; i < m_nNodeCount; i++)
	{
		if(nRand >= m_Nodes[i].nMin && nRand <= m_Nodes[i].nMax)
			return m_Nodes[i].nNodeKey;
	}

	return m_nDefaultKey;
}

void CRoundTable::Init(INT32 nRate, INT32 nDefaultKey)
{
	m_nMaxRate = nRate;
	m_nDefaultKey = nDefaultKey;
}

