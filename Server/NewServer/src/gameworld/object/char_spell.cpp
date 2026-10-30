#include "stdafx.h"
#include "character.h"

#include "game_world.h"
#include "spell_data.h"
#include "spell_instance.h"
#include "spell_bullet.h"
#include "spell_config.h"
#include "message_handler.h"
#include "modifier_tree.h"
#include "aura_def.h"
#include "scene_data.h"

#ifdef _ROBOT
#include "robot_message_handler.h"
#endif

#ifdef _DEBUG
static const char* SpellStateName[] =
{
	"Idle",
	"Channeling",
	"Preparing",
	"PreAccumulating",
	"Accumulating",
	"InHardTime",
};

static const char* EvtOccName[] =
{
	"OnBulletCreate",
	"BeforeRoll",
	"AfterRollAsCaster",
	"AfterRollAsTarget",
	"OnBulletHitAsCaster",
	"OnBulletHitAsTarget",
	"AfterDamageAsCaster",
	"AfterDamageAsTarget",
	"BeforeApplyDamageAsTarget",
	"OnCharCast",
	"OnCharCastSuccess",
	"OnCharPreCast1",
	"OnCharPreCast2",
	"OnCharPreCast3",
	"OnCharPreCast4",
	"OnCharPreCast5",
	"OnTimeup",
	"OnAppendAura",
	"OnRemoveAura",
	"OnEquipItem",
	"OnAuraTimeup",
	"OnAppendAuraAsCaster",
	"OnDying",
};

#endif

CCharacter::_SpellStateFunc CCharacter::m_RunSpellStateFunc[] = 
{
	&CCharacter::_OnIdle,
	&CCharacter::_OnChanneling,
	&CCharacter::_OnPreparing,
	&CCharacter::_OnPreAccumulating,
	&CCharacter::_OnAccumulating,
	&CCharacter::_OnBreak,
	&CCharacter::_OnOpening,
	&CCharacter::_OnHard,
};

CCharacter::_SpellStateFunc CCharacter::m_CancelSpellStateFunc[] = 
{
	&CCharacter::_DefaultCancelProc,
	&CCharacter::_OnCancelChanneling,
	&CCharacter::_OnCancelPreparing,
	&CCharacter::_OnCancelPreAccumulating,
	&CCharacter::_OnCancelAccumulating,
	NULL,
	&CCharacter::_CancelOpening,
	NULL,
};

CCharacter::_StiffFunc CCharacter::m_RunStiffFunc[] = 
{
	&CCharacter::_OnStiffFreeze,
	&CCharacter::_OnStiffKnockBack,
	&CCharacter::_OnStiffSilent,
};

void CCharacter::ClearSpellForReloading(void)
{
	INT32 nRetCode = 0;
	CAuraInstance* aura = NULL;

	BreakCast(FALSE, FALSE);

	for(INT32 i = auetNone; i < auetTotal; i++)
	{
		aura = m_AuraList.BeginAura(i);

		while((aura = m_AuraList.NextAura(i, aura)) != NULL)
		{
			RemoveAura(aura, FALSE);
		}
	}
}


BOOL CCharacter::AddCooldown(UINT32 dwCooldownID, INT32& nCooldownFrm)
{
//	nCooldownFrm = nCooldownFrm * (RATE_MAX - GetAttr(catCooldownReductionRate)) / RATE_MAX;

	return m_CoolDown.Add(dwCooldownID, nCooldownFrm);
}

BOOL CCharacter::ChangeCooldown(UINT32 dwCooldownID, INT32& nCooldownFrm)
{
//	nCooldownFrm = nCooldownFrm * (RATE_MAX - GetAttr(catCooldownReductionRate)) / RATE_MAX;

	return m_CoolDown.ChangeRemainFrm(dwCooldownID, nCooldownFrm);
}

BOOL CCharacter::ResetCooldown(UINT32 dwCooldownID)
{
	m_CoolDown.Reset(dwCooldownID);

	return TRUE;
}

void CCharacter::OnHurt(CCharacter* poDealer, CBullet* poBullet, INT32 nEffectValue)
{
	INT32 nNowFrm = CGameWorld::GetInstance().GetGameFrame();

	LOG_PROCESS_ERROR(poDealer);
	LOG_PROCESS_ERROR(poBullet);
	PROCESS_ERROR(m_eMoveState != CMS_DIE);

	if (m_poCastingSpell == NULL || m_poCastingSpell->GetAttr(satHitBreak))
		CancelCast();

	OnHurt(poDealer, nEffectValue);

	if(nEffectValue >= m_nCurrentHp)
		poBullet->GetEffectFlag().SetFlag(sfbDead);
	else
	{
		if(poBullet->GetStiffFrm() > 0)
		{
			if(poBullet->GetStiffRandomNum() > GetAttr(catHurtResist))
			{
				if(CanActiveMove())
					m_oStiffFlag.SetFlag(cstfFreeze);
			}
		}
	}


Exit0:
	return;
}

void CCharacter::OnHurt(CCharacter* poDealer, INT32 nEffectValue)
{
	INT32 nNowFrm = CGameWorld::GetInstance().GetGameFrame();

	LOG_PROCESS_ERROR(poDealer);
	PROCESS_ERROR(m_eMoveState != CMS_DIE);

	if (m_dwDropOwnerID == poDealer->GetObjID())
	{
		m_nLastHurtFrm = nNowFrm;
	}
	else if (nNowFrm - m_nLastHurtFrm >= CONVERT_TIME(g_GWConfig.nLastHurtResetTime))
	{
		m_nLastHurtFrm = nNowFrm;
		m_dwDropOwnerID = poDealer->GetObjID();
	}

	if(nEffectValue >= m_nCurrentHp)
	{
		TrigEvent(meoOnDying, NULL);
	}

Exit0:
	return;
}

#ifndef _ROBOT

INT32 CCharacter::CastSpell(CSpell* poSpell, CCharacter* poTarget, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ)
{
	INT32 nRetCode = 0;
	INT32 nResult = 0;
	INT32 nCastTime = 0;
	UINT32 dwCurrentFrm = CGameWorld::GetInstance().GetGameFrame();
	INT64 x = llTargetX, y = llTargetY, z = llTargetZ;
	INT32 nInitResCost = 0;
	INT32 nExtraResCost = 0;
	INT32 nNewDirection = m_nFaceDirection;
	CSpellInstance* poCastingSpell = NULL;
	CCharacter* poLockedTarget = NULL;

	PROCESS_ERROR_RET_CODE(CGameWorld::GetInstance().CanCast(), errSpellDisabled);

	if (poSpell->GetAttr(satIgnoreClientTarget))
		poTarget = NULL;

	if(poTarget != NULL)
	{
		x = poTarget->GetX();
		y = poTarget->GetY();
		z = poTarget->GetZ();
	}

//	if (poSpell->GetID() == 5117)
//		DBG("cast 5117 !!");

	LOG_PROCESS_ERROR_RET_CODE(poSpell != NULL, errSpellNotExists);

	if(!poSpell->GetAttr(satCancelStiff))
	{
		PROCESS_ERROR_RET_CODE(!m_oStiffFlag.CheckFlag(cstfSilent), errSpellCastNoStiff);
	}

	poCastingSpell = CGameWorld::GetInstance().NewSpellInstance();
	LOG_PROCESS_ERROR_RET_CODE(poCastingSpell, errSpellUnknown);

	nRetCode = poCastingSpell->Init(poSpell, this, GetObjID(), dwCurrentFrm, m_llX, m_llY, m_llZ);
	LOG_PROCESS_ERROR_RET_CODE(nRetCode, errSpellUnknown);

	TrigEvent(meoOnCharCast, poCastingSpell);

	poCastingSpell->ResetTimeParam();

	if(m_poCastingSpell != NULL)
	{
		INT32 nCancelType = m_poCastingSpell->GetAttr(satCancelType);
		if(nCancelType == spcltInstant || (nCancelType == spcltDependOnValue && m_poCastingSpell->GetAttr(satCancelLevel) < poSpell->GetAttr(satCancelLevel)))
			BreakCast(FALSE, FALSE);
	}

	PROCESS_ERROR_RET_CODE(m_eSpellState == cspsIdle, errSpellCasting);

	if(poCastingSpell->GetAttr(satCasterDeadLimit))
		PROCESS_ERROR_RET_CODE(IsDead(), errSpellCastNoDead);
	else
		PROCESS_ERROR_RET_CODE(!IsDead(), errSpellCastNoLive);

	if(!poCastingSpell->GetAttr(satCanCastOnMount))
		PROCESS_ERROR_RET_CODE(!m_oFlag.FlagIncludeBits(1 << cfOnMount), errSpellCastNoMount);

	PROCESS_ERROR_RET_CODE(m_oFlag.FlagIncludeBits(poCastingSpell->GetAttr(satCasterLimitBits)), errSpellConditionError);
	PROCESS_ERROR_RET_CODE(m_oCCFlag.FlagNotIncludeBits(poCastingSpell->GetAttr(satCasterCCLimitBits)), errSpellCasterCCLimitBitsError);

	if (!poCastingSpell->GetAttr(satCanMoveInPrepare))
	{
		if(m_eMoveState != CMS_STAND && m_eMoveState != CMS_FREEZE)
			PROCESS_ERROR_RET_CODE(poCastingSpell->GetAttr(satBreakMove), errSpellCastNoMove);
	}

	nInitResCost = poCastingSpell->GetConsumeAttr(satInitialCostValue) + (INT64)poCastingSpell->GetConsumeAttr(satInitialCostValuePct) * GetAttr(catMaxMp) / RATE_MAX;
//	nExtraResCost = poCastingSpell->GetConsumeAttr(satModifierCostValue) + (INT64)poCastingSpell->GetConsumeAttr(satModifierCostValuePct) * GetAttr(catMaxMp) / RATE_MAX;

//	nInitResCost = nInitResCost * GetAttr(catEnergyCostModificationRate) / RATE_MAX;
//	nExtraResCost = nExtraResCost* GetAttr(catEnergyCostModificationRate) / RATE_MAX;

	if (nInitResCost + nExtraResCost >= 0)
		PROCESS_ERROR_RET_CODE(nInitResCost + nExtraResCost <= GetMp(), errSpellOutOfMana);
//	else
//		PROCESS_ERROR_RET_CODE((GetMp() - nInitResCost - nExtraResCost) <= GetAttr(catMaxMp), errSpellOutOfMana);

	nResult = _CheckCastCondition(poCastingSpell);
	PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

//	if (poCastingSpell->GetAttr(satLockType) == spltObject)
//	{
//		PROCESS_ERROR_RET_CODE(poTarget != NULL, errSpellNoTarget);
//		nNewDirection = g_GetDirection(m_llX, m_llY, poTarget->GetX(), poTarget->GetY());
//	}
//	else
//		nNewDirection = g_GetDirection(m_llX, m_llY, llTargetX, llTargetY);

	if(poCastingSpell->GetAttr(satAdjustDirection) > 0)
	{
		if(nNewDirection < 0)
			nNewDirection = GetFaceDirection();

		RotateTo(nNewDirection, 0);
	}

	nRetCode = poCastingSpell->GetScanner().Init(this, poCastingSpell, nNewDirection);
	LOG_PROCESS_ERROR_RET_CODE(nRetCode, errSpellUnknown);

	poCastingSpell->GetScanner().SetCenterPt(GetX(), GetY(), GetZ());

	switch(poCastingSpell->GetAttr(satLockType))
	{
	case spltSelf:
		poTarget = this;
	case spltObject:
		nResult = _PrepareSpell(poCastingSpell, poTarget);
		break;
	case spltSelfPos:
		x = GetX();
		y = GetY();
		z = GetZ();
		nResult = _PrepareSpell(poCastingSpell, x, y, z);
		break;
	case spltSmartPos:
		_AdjustTargetPos(poCastingSpell, x, y, z);
	case spltPos:
		nResult = CheckTargetPos(this, poCastingSpell, x, y, z, poTarget == NULL ? 0 : poTarget->GetRadius(), 0);
		PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);
		nResult = _PrepareSpell(poCastingSpell, x, y, z);
		break;
	case spltDirection:
		if(poTarget != NULL)
			_PrepareSpell(poCastingSpell, poTarget);
		else
			_PrepareSpell(poCastingSpell, x, y, z);
		break;
	default:
		break;
	}
	PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

	SetLastTargetID(0);


	if(poCastingSpell->GetAttr(satBreakMove) && m_eMoveState != CMS_STAND)
		Stand(FALSE);

	_CastSpell(poCastingSpell, nCastTime);


	m_poCastingSpell = poCastingSpell;

	if(m_poCastingSpell->GetAttr(satCoolDownTriggerType) == spcdtOnPrepare)
	{
		INT32 nCoolDownFrm = poCastingSpell->GetSelfCoolDownFrm();
		// self cooldown
		AddCooldown(MakeCoolDownID(COOLDOWN_SPELL, poCastingSpell->GetID()), nCoolDownFrm);

		// group cooldown
		{
			const SPELL_ENTRY* inf = poCastingSpell->GetData();

			for(INT32 i = 0; i < inf->m_nCdGrpCount; i++)
			{
				nCoolDownFrm = inf->m_CdGrp[i].m_nCdGrpValue;
				AddCooldown(MakeCoolDownID(COOLDOWN_GROUP, inf->m_CdGrp[i].m_nCdGrpID), nCoolDownFrm);
			}
		}
	}

	TrigEvent(meoOnCharCastSuccess, poCastingSpell);

	ApplyModifierTree(poCastingSpell->GetAttr(satOnPrepare), poCastingSpell);

	if(poCastingSpell->GetAttr(satCancelStiff) && m_oStiffFlag.GetFlag() != 0)
		Stiff(FALSE, 0, 0);

	if(poCastingSpell->GetScanner().GetLockedTarget() != 0)
		poLockedTarget = CGameWorld::GetInstance().FindValidCharacter(poCastingSpell->GetScanner().GetLockedTarget());

	poCastingSpell->GetScanner().GetTargetPt(x, y, z);
	CCliMessageHandler::GetInstance().DoCharPrepareSpell(this, poCastingSpell, poLockedTarget == NULL ? 0 : poLockedTarget->GetObjID(), x, y, z, GetWeaponType());

	ChangeMp(-nInitResCost);

	if(poCastingSpell->GetPrepareTimeMinFrm() == 0)
		_UpdateSpell();


	return nResult;
Exit0:
	if(poCastingSpell != NULL)
	{
		poCastingSpell->Release();
		poCastingSpell = NULL;
	}

	return nResult;
}

INT32 CCharacter::CastSpell(INT32 nSpellID, CCharacter* poTarget, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ)
{
	INT32 nResult = errUnknownError;

	CSpell* poSpell = GetSpellBook().GetMainSpell(nSpellID);
	LOG_PROCESS_ERROR_RET_CODE(poSpell != NULL, errSpellNotExists);

	return CastSpell(poSpell, poTarget, nTargetX, nTargetY, nTargetZ);
Exit0:
	CRI("Spell ID: %d", nSpellID);
	return nResult;
}

#else

INT32 CCharacter::CastSpell(CSpell* poSpell, CCharacter* poTarget, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ)
{
	return 0;
}

INT32 CCharacter::CastSpell(INT32 nSpellID, CCharacter* poTarget, INT64 nTargetX, INT64 nTargetY, INT64 nTargetZ)
{
	return 0;
}

#endif  // _ROBOT

INT32 CCharacter::CastInstantSpell(CSceneObject* poFromObj, CSpell* poSpell, CCharacter* poTarget, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ)
{
	INT32 nResult = 0;
	CSpellInstance inst;
	INT64 x = llTargetX, y = llTargetY, z = llTargetZ;
	INT32 nNewDirection = 0;
	INT32 nInitResCost = 0;
	BOOL bTrigSpellEvent = poFromObj->GetObjType() != otMissile;

	m_nSubSpellDepth++;

	LOG_PROCESS_ERROR_RET_CODE(m_nSubSpellDepth <= MAX_SUBSPELL_DEPTH, errSpellUnknown);

	LOG_PROCESS_ERROR_RET_CODE(poSpell, errSpellNotExists);

	nResult = inst.Init(poSpell, this, poFromObj->GetObjID(), CGameWorld::GetInstance().GetGameFrame(), m_llX, m_llY, m_llZ);
	LOG_PROCESS_ERROR_RET_CODE(nResult == errSuccess, errSpellUnknown);

	SPL("---- CastInstantSpell: %d-%d : %s", poSpell->GetID(), poSpell->GetLevel(), GetName());

	if(poFromObj->GetObjType() != otMissile)
		TrigEvent(meoOnCharCast, &inst);

	inst.ResetTimeParam();

	nResult = inst.GetScanner().Init(this, poSpell, poFromObj->GetFaceDirection());
	LOG_PROCESS_ERROR_RET_CODE(nResult == errSuccess, errSpellUnknown);

	inst.GetScanner().SetCenterPt(poFromObj->GetX(), poFromObj->GetY(), poFromObj->GetZ());

	nResult = _CheckCastCondition(&inst);
	PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

	if (inst.GetAttr(satCastType) == spctSwitch)
	{
		nInitResCost = inst.GetConsumeAttr(satInitialCostValue) + (INT64)inst.GetConsumeAttr(satInitialCostValuePct) * GetAttr(catMaxMp) / RATE_MAX;

		if (nInitResCost > 0)
			PROCESS_ERROR_RET_CODE(nInitResCost <= GetMp(), errSpellOutOfMana);
	}

	switch(poSpell->GetAttr(satLockType))
	{
	case spltSelf:
		poTarget = this;
	case spltObject:
		nResult = _PrepareSpell(&inst, poTarget);
		break;
	case spltSelfPos:
		x = GetX();
		y = GetY();
		z = GetZ();
	case spltPos:
		nResult = CheckTargetPos(poFromObj, &inst, x, y, z, poTarget == NULL ? 0 : poTarget->GetRadius(), 0);
		PROCESS_ERROR_RET_CODE(nResult == errSuccess, errSpellOutOfRange);
	case spltDirection:
		nResult = _PrepareSpell(&inst, x, y, z);
		break;
	default:
		break;
	}
	PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

	if(poSpell->GetAttr(satLockType) == spltObject)
	{
		PROCESS_ERROR_RET_CODE(poTarget != NULL, errSpellNoTarget);
		nNewDirection = g_GetDirection(m_llX, m_llY, poTarget->GetX(), poTarget->GetY());
	}
	else if(poSpell->GetAttr(satLockType) == spltPos)
		nNewDirection = g_GetDirection(m_llX, m_llY, llTargetX, llTargetY);

	if(poFromObj->GetObjType() != otMissile)
		TrigEvent(meoOnCharCastSuccess, &inst);

	// Cooldown
	{
		INT32 nCoolDownFrm = inst.GetSelfCoolDownFrm();
		// self cooldown
		AddCooldown(MakeCoolDownID(COOLDOWN_SPELL, inst.GetID()), nCoolDownFrm);

		// group cooldown
		{
			const SPELL_ENTRY* inf = inst.GetData();

			for(INT32 i = 0; i < inf->m_nCdGrpCount; i++)
			{
				nCoolDownFrm = inf->m_CdGrp[i].m_nCdGrpValue;
				AddCooldown(MakeCoolDownID(COOLDOWN_GROUP, inf->m_CdGrp[i].m_nCdGrpID), nCoolDownFrm);
			}
		}
	}

#ifndef _ROBOT
	nResult = CCliMessageHandler::GetInstance().DoCharPrepareSpell(poFromObj, &inst, poTarget == NULL ? 0 : poTarget->GetObjID(), x, y, z, GetWeaponType());
	LOG_CHECK_ERROR(nResult);
#endif  // _ROBOT

	if (inst.GetAttr(satCastType) == spctSwitch)
	{
		ChangeMp(-nInitResCost);
	}

	nResult = inst.DoEffect(this);
	LOG_CHECK_ERROR(nResult);

	m_nSubSpellDepth--;
	return errSuccess;

Exit0:
	m_nSubSpellDepth--;
	return nResult;
}

INT32 CCharacter::_PrepareSpell(CSpellInstance* poSpell, CCharacter* poTarget)
{
	INT32 nResult = errUnknownError;
	INT64 llTargetX = 0, llTargetY = 0, llTargetZ = 0;
	INT32 nDirection8 = 0;
	INT32 nDirection8V = 0;

	LOG_PROCESS_ERROR_RET_CODE(poTarget != NULL, errSpellNoTarget);

	nResult = CheckTargetPos(this, poSpell, poTarget->GetX(), poTarget->GetY(), poTarget->GetZ(), poTarget->GetRadius(), 0);
	PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

	//nResult = CheckTarget(poSpell, poTarget);
	//PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

	if(CheckTarget(poSpell, poTarget) == errSuccess)
		poSpell->GetScanner().LockTarget(poTarget);

	llTargetZ = poTarget->GetZ();

	GetOffsetedPos(poTarget->GetX(), poTarget->GetY(), poSpell->GetScanner().GetDirection(), poSpell->GetAttr(satHOffsetGrids), poSpell->GetAttr(satVOffsetGrids), llTargetX, llTargetY);

	poSpell->GetScanner().SetTargetPt(llTargetX, llTargetY, llTargetZ);

	SPL("face direction: %d", m_nFaceDirection);

	return errSuccess;
Exit0:
	return nResult;
}

INT32 CCharacter::_PrepareSpell(CSpellInstance* poSpell, INT64& nTargetX, INT64& nTargetY, INT64& nTargetZ)
{
	INT32 nNormalizedDirection = NormalizeDirection(poSpell->GetAttr(satOffsetDirection) + GetFaceDirection());
	INT32 nDirection8 = ConvertToDir8(poSpell->GetScanner().GetDirection());
	INT32 nDirection8V = NormalizeDirection8(nDirection8 - 4);

	nTargetX += poSpell->GetAttr(satHOffsetGrids) * g_nAdjacentX[nDirection8V] * GRID_WIDTH;
	nTargetY += poSpell->GetAttr(satHOffsetGrids) * g_nAdjacentY[nDirection8V] * GRID_WIDTH;

	nTargetX += poSpell->GetAttr(satVOffsetGrids) * g_nAdjacentX[nDirection8] * GRID_WIDTH;
	nTargetY += poSpell->GetAttr(satVOffsetGrids) * g_nAdjacentY[nDirection8] * GRID_WIDTH;

	poSpell->GetScanner().SetTargetPt(nTargetX, nTargetY, nTargetZ);

	return errSuccess;
}

void CCharacter::_CastSpell(CSpellInstance* poSpell, INT32& nCastTime)
{
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();
	INT32 nPrepareFrmMin, nPrepareFrmMax, nInterval = 0;

	LOG_PROCESS_ERROR(poSpell != NULL);

	nPrepareFrmMin = poSpell->GetPrepareTimeMinFrm();
	nPrepareFrmMax = poSpell->GetPrepareTimeMaxFrm();

	nInterval = poSpell->GetIntervalFrm();

	// Spell preparation type
	switch(poSpell->GetAttr(satPrepareType))
	{
	case spptChanneled:
		nCastTime = nPrepareFrmMin * 1000/GAME_FPS;//poSpell->GetAttr(satPrepareTimeMin);
		m_eSpellState = cspsChanneling;
		//m_PreparingTimer.Enable(nPrepareFrmMax, 1, nFrm, nPrepareFrmMax);
		m_CastTimer.Enable(nInterval, -1, nFrm, nPrepareFrmMin);
		break;
	case spptPrepared:
		nCastTime = nPrepareFrmMin * 1000/GAME_FPS;//poSpell->GetAttr(satPrepareTimeMin);
		m_eSpellState = cspsPreparing;
		m_PreparingTimer.Enable(nPrepareFrmMin, 1, nFrm, nPrepareFrmMin);
		break;
	case spptAccumulated:
		nCastTime = nPrepareFrmMax * 1000/GAME_FPS;//poSpell->GetAttr(satPrepareTimeMax);
		m_eSpellState = cspsPreAccumulating;
		m_PreparingTimer.Enable(nPrepareFrmMin, 1, nFrm, nPrepareFrmMin);
		break;
	default:
		break;
	}

	m_eHitTag = poSpell->GetAttr(satHitTag);

Exit0:
	return;
}

void CCharacter::_UpdateSpell(void)
{
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();
	LOG_PROCESS_ERROR(m_eSpellState > cspsInvalid && m_eSpellState < cspsTotal);

	if(m_RunSpellStateFunc[m_eSpellState] != NULL)
	{
		if(m_poCastingSpell != NULL)
		{
			m_poCastingSpell->TryExCastTrigger(this, nFrm);
		}

		BOOL bRetCode = (this->*m_RunSpellStateFunc[m_eSpellState])();
		if(!bRetCode)
			BreakCast(TRUE, FALSE);
	}

Exit0:
	return;
}

void CCharacter::_UpdateBullet(void)
{
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();
	// update bullets:
	BASE_LIST_NODE* pNextNode = NULL;
	for(BASE_LIST_NODE* pNode = m_BulletList.Head().pNext; pNode != &(m_BulletList.Rear()); pNode = pNextNode)
	{
		CBullet* poBullet = node_cast(CBullet*, GetListNode(), pNode);
		pNextNode = pNode->pNext;

		poBullet->Update(this, nFrm);

		if(poBullet->NeedRemove())
		{
			CBaseList::Remove(&poBullet->GetListNode());
			poBullet->UnInit();
		}
	}
}

//void CCharacter::_UpdateAura(void)
//{
//	INT32 nRetCode = 0;
//	CAuraInstance* poAura = NULL;
//	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();
//
//	m_AuraList.Update(this);
//}


void CCharacter::_CheckCombat(void)
{
	INT32 nRetCode = 0;

	if(!m_oFlag.CheckFlag(cfInCombat) && !m_ThreatList.IsEmpty())
	{
		_EnterCombat();
	}
	else if(m_oFlag.CheckFlag(cfInCombat) && m_ThreatList.IsEmpty())
	{
		_LeaveCombat();
	}
}

void CCharacter::_EnterCombat(void)
{
	SPL("--------- Enter Combat : %s ------------", GetName());
	m_oFlag.SetFlag(cfInCombat);
	m_oFlag.ClearFlag(cfNotInCombat);

#ifndef _ROBOT
	CCliMessageHandler::GetInstance().DoEnterCombat(this);
#endif  // _ROBOT

}

void CCharacter::_LeaveCombat(void)
{
	SPL("--------- Leave Combat : %s ------------", GetName());
	m_oFlag.ClearFlag(cfInCombat);
	m_oFlag.SetFlag(cfNotInCombat);

#ifndef _ROBOT
	CCliMessageHandler::GetInstance().DoLeaveCombat(this);
#endif  // _ROBOT

}

void CCharacter::_ClearAfterDie(void)
{
	CAuraInstance* poAura = NULL;

	BASE_LIST_NODE* pNextNode = NULL;
	for(BASE_LIST_NODE* pNode = m_BulletList.Head().pNext; pNode != &(m_BulletList.Rear()); pNode = pNextNode)
	{
		CBullet* poBullet = node_cast(CBullet*, GetListNode(), pNode);
		pNextNode = pNode->pNext;

		poBullet->SetRemoveFlag(TRUE);
	}

	// Remove all negative auras

//	for(INT32 i = auetNone; i < auetTotal; i++)
//	{
//		poAura = m_AuraList.GetStaticAura(i);
//		if(poAura == NULL)
//			continue;
//
//		if(poAura->GetAttr(aatRemoveWhenDead))
//			RemoveAura(poAura, TRUE);
//	}

	for(INT32 i = auetNone; i < auetTotal; i++)
	{
		poAura = m_AuraList.BeginAura(i);

		while((poAura = m_AuraList.NextAura(i, poAura)) != NULL)
		{
			if(poAura->GetAttr(aatRemoveWhenDead))
				RemoveAura(poAura, TRUE);
		}
	}
}

BOOL CCharacter::_CheckSpellCoolDown(CSpell* poSpell)
{
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(poSpell);

	nRetCode = m_CoolDown.CheckReady(MakeCoolDownID(COOLDOWN_SPELL, poSpell->GetID()));
	PROCESS_ERROR(nRetCode);

	{
		const SPELL_ENTRY* inf = poSpell->GetData();

		for(INT32 i = 0; i < inf->m_nCdGrpCount; i++)
		{
			nRetCode = m_CoolDown.CheckReady(MakeCoolDownID(COOLDOWN_GROUP, inf->m_CdGrp[i].m_nCdGrpID));
			PROCESS_ERROR(nRetCode);
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}


INT32 CCharacter::_CheckCastCondition(CSpellInstance* poSpell)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(poSpell);

	nResult = errSuccess;

	nRetCode = _CheckSpellCoolDown(poSpell);
	PROCESS_ERROR_RET_CODE(nRetCode, errSpellNotReady);

	if(!ApplyModifierTree(poSpell->GetAttr(satCasterCheckNode), poSpell))
		nResult = errSpellCasterCheckNodeFailed;
Exit0:
	return nResult;
}

INT32 CCharacter::CheckTarget(CSpellInstance* poSpell, CCharacter* poTarget)
{
	return CheckTarget(poTarget, poSpell->GetAttr(satTargetObjType), poSpell->GetAttr(satTargetRelationship), poSpell->GetAttr(satTargetStance), poSpell->GetAttr(satTargetDeadLimit), poSpell->GetAttr(satTargetCheckNode));
}

INT32 CCharacter::CheckTarget(CCharacter* poTarget, INT32 nTargetObjTypeLimit, INT32 nRelationShipLimit, INT32 nStanceLimit, BOOL bCorpseValid, INT32 nTargetCheckNode)
{
	INT32 nRetCode = 0;
	INT32 nResult = errSpellUnknown;
	BOOL bCanAttack = FALSE;
	INT32 eTargetObjType = 0;
	INT32 eThisObjType = 0;

	PROCESS_ERROR_RET_CODE(poTarget, errUnknownError);

	eThisObjType = GetObjType();
	eTargetObjType = poTarget->GetObjType();

	if(eTargetObjType == otRole)
	{
		CRole* poRole = static_cast<CRole*>(poTarget);
		PROCESS_ERROR_RET_CODE(poRole->GetState() == rsPlaying || poRole->GetState() == rsLogout, errSpellInValidTargetRoleState);
	}
	else if (eTargetObjType == otNpc)
	{
		CNpc* poNpc = static_cast<CNpc*>(poTarget);
		PROCESS_ERROR_RET_CODE(!poNpc->NeverBeHit(), errSpellInValidTargetNeverBeHit);
	}

	if(!bCorpseValid)
		PROCESS_ERROR_RET_CODE(!poTarget->IsDead(), errSpellInValidTargetDead);

	PROCESS_ERROR_RET_CODE((nStanceLimit & (1 << poTarget->GetStance())) != 0, errSpellInValidTargetStance);

	bCanAttack = CanAttack(poTarget);

	if (eThisObjType == otRole)
	{
		CRole* poRole = static_cast<CRole*>(this);

		if (poTarget->GetHostID() == poRole->GetObjID())
		{
			if (poRole->GetPkMode() == rpmSlayer && (nRelationShipLimit & (1 << sptrEnemy)) > 0)
				bCanAttack = TRUE;
			else
				bCanAttack = FALSE;
		}
	}

	PROCESS_ERROR_RET_CODE(nTargetObjTypeLimit & (1 << eTargetObjType), errSpellInValidTargetObjType);

	if(((nRelationShipLimit & (1 << sptrEnemy)) > 0 && bCanAttack) || ((nRelationShipLimit & (1 << sptrFriendly)) > 0 && !bCanAttack && poTarget->GetObjID() != GetObjID()) || ((nRelationShipLimit & (1 << sptrSelf)) && poTarget->GetObjID() == GetObjID()))
	{
		nResult = errSuccess;
	}
	else
		nResult = errSpellInValidTargetRelationship;

	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(nTargetCheckNode, poTarget, NULL);
	PROCESS_ERROR_RET_CODE(nRetCode == mrcTrue, errSpellInValidTargetNodeError); 

Exit0:
	return nResult;
}

INT32 CCharacter::CheckTargetPos(CSceneObject* poFromObj, CSpellInstance* poSpell, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ, INT32 nTargetRadius, INT32 nTolerantGrids)
{
	INT32 nRetCode = 0;
	INT32 nResult = errSpellOutOfRange;
	INT64 llMyGridX = 0;
	INT64 llMyGridY = 0;
	INT64 llTgGridX = llTargetX / GRID_WIDTH;
	INT64 llTgGridY = llTargetY / GRID_WIDTH;
	INT32 nRadiusGrid = nTargetRadius / GRID_WIDTH;

	LOG_PROCESS_ERROR(poFromObj);

	llMyGridX = poFromObj->GetX() / GRID_WIDTH;
	llMyGridY = poFromObj->GetY() / GRID_WIDTH;

	PROCESS_ERROR_RET_CODE(g_Abs(llMyGridX - llTgGridX) >= poSpell->GetAttr(satMinGrids) && g_Abs(llMyGridY - llTgGridY) >= poSpell->GetAttr(satMinGrids), errSpellOutOfRange);
	PROCESS_ERROR_RET_CODE(g_Abs(llMyGridX - llTgGridX) <= poSpell->GetAttr(satMaxGrids) + nTolerantGrids + nRadiusGrid && g_Abs(llMyGridY - llTgGridY) <= poSpell->GetAttr(satMaxGrids) + nTolerantGrids + nRadiusGrid, errSpellOutOfRange);

	return errSuccess;
Exit0:
	SPL("Check target pos failed: %d, spellid: %d", nResult, poSpell->GetID());
	return nResult;
}

BOOL CCharacter::CheckMinDist(CSpellInstance* poSpell, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ)
{
	INT64 llDistSq = g_GetDistance3D(m_llX, m_llY, m_llZ, llTargetX, llTargetY, llTargetZ);
	INT64 llMinDistSq = (INT64)poSpell->GetLengthAttr(satMinDistance) * (INT64)poSpell->GetLengthAttr(satMinDistance);

	PROCESS_ERROR(llDistSq >= llMinDistSq);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::CheckMaxDist(CSpellInstance* poSpell, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ, INT32 nTargetRadius)
{
	INT64 llDistSq = g_GetDistance3D(m_llX, m_llY, m_llZ, llTargetX, llTargetY, llTargetZ);
	INT64 llMaxDistSq = (INT64)poSpell->GetLengthAttr(satMaxDistance);

//	if(poSpell->GetAngleAttr(satUseWeaponRange))
//		llMaxDistSq += GetAttr(catAttackRange);

	llMaxDistSq += nTargetRadius;

	llMaxDistSq *= llMaxDistSq;

	PROCESS_ERROR(llDistSq <= llMaxDistSq);

	return TRUE;

Exit0:
	return FALSE;
}

void CCharacter::_AdjustTargetPos(CSpellInstance* poSpell, INT64& llTargetX, INT64& llTargetY, INT64& llTargetZ)
{
	INT32 nRetCode = 0;
//	INT64 llDistSq = g_GetDistance2D(m_llX, m_llY, llTargetX, llTargetY);

//	INT64 llMinDist = poSpell->GetLengthAttr(satMinDistance) + GRID_WIDTH;
//	INT64 llMaxDist = poSpell->GetLengthAttr(satMaxDistance) - GRID_WIDTH;

	INT32 nDirection = g_GetDirection(m_llX, m_llY, llTargetX, llTargetY);
	INT32 nDirection8 = ConvertToDir8(nDirection);

	INT32 nDetGridX = g_Abs(m_llX - llTargetX) / GRID_WIDTH;
	INT32 nDetGridY = g_Abs(m_llY - llTargetY) / GRID_WIDTH;
	INT32 nMaxGrids = poSpell->GetAttr(satMaxGrids);

	if (nDetGridX > nMaxGrids || nDetGridY > nMaxGrids)
	{
//		llTargetX = m_llX + g_nAdjacentX[nDirection8] * nMaxGrids * GRID_WIDTH;
//		llTargetY = m_llY + g_nAdjacentY[nDirection8] * nMaxGrids * GRID_WIDTH;
		llTargetX = m_llX + nMaxGrids * GRID_WIDTH * g_Cos(nDirection) / SIN_COS_NUMBER;
		llTargetY = m_llY + nMaxGrids * GRID_WIDTH * g_Sin(nDirection) / SIN_COS_NUMBER;
	}

//	if(llDistSq > llMaxDist * llMaxDist)
//	{
//		llTargetX = m_llX + llMaxDist * g_Cos(nDirection) / SIN_COS_NUMBER;
//		llTargetY = m_llY + llMaxDist * g_Sin(nDirection) / SIN_COS_NUMBER;
//	}
//	else if(llDistSq < llMinDist * llMinDist)
//	{
//		llTargetX = m_llX + llMinDist * g_Cos(nDirection) / SIN_COS_NUMBER;
//		llTargetY = m_llY + llMinDist * g_Sin(nDirection) / SIN_COS_NUMBER;
//	}

	llTargetZ = m_llZ;

#ifndef _ROBOT
	if (CGameWorld::GetInstance().DrawDebugLine())
	{
		CCliMessageHandler::GetInstance().DoDrawDebugLine(this, 0, m_llX, m_llY, m_llZ, llTargetX, llTargetY, llTargetZ);
	}
#endif
}


void CCharacter::AdjustTargetPt(INT32 nX, INT32 nY, INT32 nZ, INT32 nDurationFrm)
{
    INT32 nDirection;

	PROCESS_ERROR(m_poCastingSpell);
	PROCESS_ERROR(m_poCastingSpell->GetAttr(satAdjustDirection));

	nDirection = g_GetDirection(m_llX, m_llY, nX, nY);

	DBG("rotate: %s : %d", __FILE__, __LINE__);
	RotateToDir8(nDirection, nDurationFrm);

//	DBG("Adjust Target Pt: (%.2f, %.2f, %.2f)", I64_2_Double(nX), I64_2_Double(nY), I64_2_Double(nZ));

	m_poCastingSpell->GetScanner().SetTargetPt(nX, nY, nZ);
	m_poCastingSpell->GetScanner().SetDirection(nDirection);
	m_poCastingSpell->GetScanner().LockTarget(NULL);

Exit0:
	return;
}

BOOL CCharacter::PushBullet(CBullet* poBullet)
{
	INT32 nRetCode = 0;
	PROCESS_ERROR(!IsDead());

	nRetCode = m_BulletList.PushRear(&poBullet->GetListNode());
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

CCharacter* CCharacter::SelectFocus(UINT32 dwFocusID)
{
	INT32 nRetCode = 0;
	CCharacter* poFocusChar = NULL;

//	PROCESS_ERROR(dwFocusID != m_dwFocusID);

	if(dwFocusID != 0)
	{
		poFocusChar = CGameWorld::GetInstance().FindValidCharacter(dwFocusID);
		PROCESS_ERROR(poFocusChar);
	}

	m_dwFocusID = dwFocusID;

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoCharSelectFocus(this, dwFocusID);
	LOG_CHECK_ERROR(nRetCode);
#endif  // _ROBOT

	return poFocusChar;
Exit0:
	return NULL;;
}

BOOL CCharacter::RegisterEvent(UINT64 qwEventKey, INT32 nModifierNodeID, INT32 nRootSpellID, INT32 nRootSpellLevel, ExtraParam* poParam)
{
	INT32 nModifierType = 0;
	TEventMap::iterator itr;
	INT32 nSplitOccID = 0, nRegisterID = 0;
	INT32 eSource = 0;
	SpellEventNode EventNode = { 0 };
	INT32 nRetCode = 0;

	SplitEventKey(qwEventKey, nSplitOccID, eSource, nRegisterID);

	LOG_PROCESS_ERROR(nSplitOccID > meoInvalid && nSplitOccID < meoTotal);

	nModifierType = CModifierTree::GetInstance().GetGlobalNodeType(nModifierNodeID);

	if(nSplitOccID >= meoBulletEventBegin && nSplitOccID < meoBulletEventEnd)
		LOG_PROCESS_ERROR(nModifierType == mttBullet);

	else if(nSplitOccID >= meoCharEventBegin && nSplitOccID < meoCharEventEnd)
		LOG_PROCESS_ERROR(nModifierType == mttCharacter);

	itr = m_RegEventMap[nSplitOccID].find(qwEventKey);
	LOG_PROCESS_ERROR(itr == m_RegEventMap[nSplitOccID].end());

	EventNode.qwEventKey = qwEventKey;
	EventNode.nNodeID = nModifierNodeID;
	EventNode.nRootSpellID = nRootSpellID;
	EventNode.nRootSpellLevel = nRootSpellLevel;
	EventNode.bRemoved = FALSE;
	memcpy(&EventNode.Param, poParam, sizeof(ExtraParam));

	nRetCode = m_RegEventMap[nSplitOccID].insert(std::make_pair(qwEventKey, EventNode)).second;
	LOG_PROCESS_ERROR(nRetCode);

//	if (nRootSpellID == 5117)
//		DBG("reg event: 5117!!");

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::RegisterTimerEvent(UINT64 qwEventKey, INT32 nInterval, INT32 nModifierNodeID, INT32 nRootSpellID, INT32 nRootSpellLevel, ExtraParam* poParam)
{
	BOOL bRetCode = FALSE;
	INT32 nModifierType = 0;
	CSimpleTimer timer;
	INT32 nSplitOccID = 0, nRegisterID = 0;
	INT32 eSource = 0;

	SplitEventKey(qwEventKey, nSplitOccID, eSource, nRegisterID);

	INT32 nOccasionID = GetOccasionID(qwEventKey);

	LOG_PROCESS_ERROR(nOccasionID == meoOnTimeup);

	nModifierType = CModifierTree::GetInstance().GetGlobalNodeType(nModifierNodeID);
	LOG_PROCESS_ERROR(nModifierType == mttCharacter);

	bRetCode = RegisterEvent(qwEventKey, nModifierNodeID, nRootSpellID, nRootSpellLevel, poParam);
	LOG_PROCESS_ERROR(bRetCode);

	timer.Enable(nInterval, -1, CGameWorld::GetInstance().GetGameFrame(), nInterval);

	m_RegTimerMap[qwEventKey] = timer;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::RemoveEvent(UINT64 qwEventKey)
{
	INT32 nRegisterID = 0;
	INT32 nOccasionID = 0;
	INT32 eSource = 0;
	SpellEventNode EventNode = { 0 };
	INT32 nRetCode = 0;

	SplitEventKey(qwEventKey, nOccasionID, eSource, nRegisterID);
	LOG_PROCESS_ERROR(nOccasionID > meoInvalid && nOccasionID < meoTotal);

	{
		TEventMap::iterator itr = m_RegEventMap[nOccasionID].find(qwEventKey);
		LOG_PROCESS_ERROR(itr != m_RegEventMap[nOccasionID].end());

		itr->second.bRemoved = TRUE;

//		if (itr->second.nRootSpellID == 5117)
//			DBG("remove event 5117!!");
	}

	if(nOccasionID == meoOnTimeup)
		m_RegTimerMap.erase(qwEventKey);

	m_RemoveEventList.push_back(qwEventKey);

	return TRUE;
Exit0:
	return FALSE;
}

void CCharacter::_UpdateEvent(void)
{
	INT32 nRetCode = 0;

	if(!m_PendingEventNodeStack.IsEmpty())
	{
		INT3();
	}

	// update timer events:
	if (!m_RegTimerMap.empty())
	{
		INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();
		for (TTimerMap::iterator itr = m_RegTimerMap.begin(); itr != m_RegTimerMap.end(); ++itr)
		{
			CSimpleTimer& timer = itr->second;

			if (!timer.Trig(nFrm))
				continue;

			UINT64 qwEventKey = itr->first;

			INT32 nSplitOccID = 0, nRegisterID = 0;
			INT32 eSource = 0;

			SplitEventKey(qwEventKey, nSplitOccID, eSource, nRegisterID);
			LOG_CHECK_ERROR(nSplitOccID == meoOnTimeup);

			if (eSource == sesFromSpell)
			{
				CSpell* poSpell = GetSpellBook().GetMainSpell(nRegisterID);
				LOG_PROCESS_ERROR(poSpell);

				if (!m_CoolDown.CheckReady(MakeCoolDownID(COOLDOWN_SPELL_EVENT_TRIGGER, poSpell->GetID())))
					continue;

			}

			TEventMap::iterator itr_event = m_RegEventMap[meoOnTimeup].find(qwEventKey);
			if (itr_event == m_RegEventMap[meoOnTimeup].end())
				continue;

			INT32 nModifierType = CModifierTree::GetInstance().GetGlobalNodeType(itr_event->second.nNodeID);
			LOG_PROCESS_ERROR(nModifierType == mttCharacter);

			PushEventNode(&itr_event->second);
			nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(itr_event->second.nNodeID, this, &itr_event->second.Param);
			LOG_CHECK_ERROR(nRetCode >= 0);
			PopEventNode();
		}
	}

	if (!m_RemoveEventList.empty())
	{
		for (TRemoveEventList::iterator itr = m_RemoveEventList.begin(); itr != m_RemoveEventList.end(); ++itr)
		{
			UINT64 qwKey = *itr;
			INT32 nOccasionID = GetOccasionID(qwKey);

			LOG_CHECK_ERROR(nOccasionID > meoInvalid && nOccasionID < meoTotal);
			m_RegEventMap[nOccasionID].erase(qwKey);
		}

		m_RemoveEventList.clear();
	}

Exit0:
	return;
}

BOOL CCharacter::TrigEvent(INT32 nOccasionID, CSpellInstance* poPendingSpell)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(nOccasionID >= meoCharEventBegin && nOccasionID < meoCharEventEnd);
	PROCESS_ERROR(!m_RegEventMap[nOccasionID].empty());

	if(poPendingSpell)
	{
		PROCESS_ERROR(poPendingSpell->GetAttr(satTrigEvent));
	}

	for(TEventMap::iterator itr = m_RegEventMap[nOccasionID].begin(); itr != m_RegEventMap[nOccasionID].end(); ++itr)
	{
		UINT64 qwEventKey = itr->first;

		INT32 nSplitOccID = 0, nRegisterID = 0;
		INT32 eSource = 0;
		CSpell* poRootSpell = NULL;
		INT32 nModifierType = 0;

		if(itr->second.bRemoved)
			continue;

		SplitEventKey(qwEventKey, nSplitOccID, eSource, nRegisterID);
		LOG_CHECK_ERROR(nSplitOccID == nOccasionID);

		poRootSpell = GetSpellBook().GetMainSpell(itr->second.nRootSpellID);
		if(poRootSpell)
		{
			if(!m_CoolDown.CheckReady(MakeCoolDownID(COOLDOWN_SPELL_EVENT_TRIGGER, poRootSpell->GetID())))
				continue;
		}


		nModifierType = CModifierTree::GetInstance().GetGlobalNodeType(itr->second.nNodeID);
		LOG_PROCESS_ERROR(nModifierType == mttCharacter);

		if (poPendingSpell)
			PushSpell(poPendingSpell);

		PushEventNode(&itr->second);
		nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(itr->second.nNodeID, this, &itr->second.Param);
		LOG_CHECK_ERROR(nRetCode >= 0);
		PopEventNode();

		if (poPendingSpell)
			PopSpell();

		PROCESS_SUCCESS(nRetCode == mrcTrueAndBreak);
	}

Exit0:
	return FALSE;
Exit1:
	return TRUE;
}

BOOL CCharacter::TrigBulletEvent(INT32 nOccasionID, CBullet* poBullet)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poBullet);
	LOG_PROCESS_ERROR(nOccasionID >= meoBulletEventBegin && nOccasionID < meoBulletEventEnd);
	PROCESS_ERROR(poBullet->GetSpellAttr(satTrigEvent));
	//LOG_PROCESS_ERROR(m_PendingEventNodeStack.empty());

	poBullet->SetEventOwner(this);

	for(TEventMap::iterator itr = m_RegEventMap[nOccasionID].begin(); itr != m_RegEventMap[nOccasionID].end(); ++itr)
	{
		UINT64 qwEventKey = itr->first;

		INT32 nSplitOccID = 0, nRegisterID = 0;
		INT32 eSource = 0;
		CSpell* poRootSpell = NULL;
		INT32 nModifierType = 0;
		SpellEventNode* poNode = &itr->second;

		if (poNode->bRemoved)
		{
			continue;
		}

		SplitEventKey(qwEventKey, nSplitOccID, eSource, nRegisterID);
		LOG_CHECK_ERROR(nSplitOccID == nOccasionID);

//		SPL("Trig Bullet Event: %d", nOccasionID);

		poRootSpell = GetSpellBook().GetMainSpell(poNode->nRootSpellID);
		if(poRootSpell)
		{
			if(!m_CoolDown.CheckReady(MakeCoolDownID(COOLDOWN_SPELL_EVENT_TRIGGER, poRootSpell->GetID())))
				continue;
		}

//		if (itr->second.nRootSpellID == 5117)
//			DBG("trig event 5117!!");

		nModifierType = CModifierTree::GetInstance().GetGlobalNodeType(poNode->nNodeID);
		LOG_PROCESS_ERROR(nModifierType == mttBullet);

		PushEventNode(poNode);
		nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(poNode->nNodeID, poBullet, &poNode->Param);
		LOG_CHECK_ERROR(nRetCode >= 0);
		PopEventNode();
	}

	poBullet->SetEventOwner(NULL);
	return TRUE;
Exit0:
	poBullet->SetEventOwner(NULL);
	return FALSE;
}

BOOL CCharacter::RollBackEvent(INT32 nOccasionID)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(nOccasionID >= meoCharEventBegin && nOccasionID < meoCharEventEnd);

	for(TEventMap::iterator itr = m_RegEventMap[nOccasionID].begin(); itr != m_RegEventMap[nOccasionID].end(); ++itr)
	{
		UINT64 qwEventKey = itr->first;

		INT32 nSplitOccID = 0, nRegisterID = 0;
		INT32 eSource = 0;

		SplitEventKey(qwEventKey, nSplitOccID, eSource, nRegisterID);
		LOG_CHECK_ERROR(nSplitOccID == nOccasionID);

		if(eSource == sesFromSpell)
		{
			CSpell* poSpell = GetSpellBook().GetMainSpell(nRegisterID);
			LOG_PROCESS_ERROR(poSpell);

		}

		INT32 nModifierType = CModifierTree::GetInstance().GetGlobalNodeType(itr->second.nNodeID);
		LOG_PROCESS_ERROR(nModifierType == mttCharacter);

		nRetCode = CModifierTree::GetInstance().RollBackGlobalNode(itr->second.nNodeID, this, &itr->second.Param);
		LOG_CHECK_ERROR(nRetCode >= 0);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::SaveSpellEvent(INT32 nSpellID, INT32 eOccasionID, UINT64 qwEventKey)
{
	INT32 nRetCode = 0;

	CSpellSet* poSet = GetSpellBook().GetSpellSet(nSpellID);
	LOG_PROCESS_ERROR(poSet);

	nRetCode = poSet->RecordEvt(eOccasionID, qwEventKey);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

CSpellSlot* CCharacter::GetSpellSlot(INT32 nIndex)
{
	return NULL;
}


BOOL CCharacter::UnregisterSpellEvent(INT32 nSpellID, INT32 eOccasionID)
{
	INT32 nRetCode = 0;
	UINT64 qwEventKey = 0;

	CSpellSet* poSet = GetSpellBook().GetSpellSet(nSpellID);
	LOG_PROCESS_ERROR(poSet);

	qwEventKey = poSet->GetEvtID(eOccasionID);
	PROCESS_ERROR(qwEventKey != 0);

	poSet->RemoveEvt(eOccasionID);

	RemoveEvent(qwEventKey);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::AppendAllPendingAura(void)
{
	INT32 nRetCode = 0;
	CCharacter* poCaster = NULL;

	PROCESS_SUCCESS(m_PendingAuraList.empty());

	for (TPendingAuraList::iterator itr = m_PendingAuraList.begin(); itr != m_PendingAuraList.end(); ++itr)
	{
		CAuraInstance* poAuraInst = *itr;

		poCaster = CGameWorld::GetInstance().FindValidCharacter(poAuraInst->GetCasterID());
		if (poCaster == NULL)
		{
			poAuraInst->UnInit();
			continue;
		}

		nRetCode = _AuraNew(poAuraInst, poCaster);
		if (!nRetCode)
		{
			poAuraInst->UnInit();
		}
	}

	m_PendingAuraList.clear();

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}

CAuraInstance* CCharacter::AppendAura(CAura* poAuraInfo, CCharacter* poCaster, UINT32 dwFromObjID, ExtraParam* pExParam, INT32 nNewDurationFrm, INT32 nNewStackCount, INT32 nRootSpellID, INT32 nRootSpellLevel)
{
	INT32 nRetCode = 0;
	INT32 nResult = errSuccess;

	CRole* pRole = NULL;

	CAuraInstance* aura = NULL;
	CAuraInstance* poExistAura = NULL;
	CAuraInstance* poRetAura = NULL;

	CAuraInstance* poExistAura1 = NULL;
	CAuraInstance* poExistAura2 = NULL;
	CAuraInstance* poExistAura3 = NULL;
	CAuraInstance* poExistAura4 = NULL;

	INT32 eEffectType = 0;
	INT32 nGroup = 0;
	INT32 eStackTypeSameIdSameCaster = 0;
	INT32 eStackTypeSameTypeSameCaster = 0;
	INT32 eStackTypeSameGroupSameCaster = 0;

	INT32 eStackTypeSameIdDiffCaster = 0;
	INT32 eStackTypeSameTypeDiffCaster = 0;
	INT32 eStackTypeSameGroupDiffCaster = 0;

//	if (poAuraInfo->GetID() == 51170)
//		DBG("aura 51170 !!");

	LOG_PROCESS_ERROR_RET_CODE(poAuraInfo, errUnknownError);
	LOG_PROCESS_ERROR_RET_CODE(poCaster, errUnknownError);

	if(poAuraInfo->GetAttr(aatCategory) != acNone)
		PROCESS_ERROR_RET_CODE(!m_oFlag.CheckFlag(cfAuraInvalid), errAuraAppendFailed);

	nRetCode = _CheckAuraAppend(poAuraInfo);
	PROCESS_ERROR_RET_CODE(nRetCode, errAuraResisted);

	if(poAuraInfo->GetAttr(aatControlDecay))
	{
		if(nNewDurationFrm <= 0)
			nNewDurationFrm = poAuraInfo->GetDurationFrm();

		nNewDurationFrm = nNewDurationFrm * (RATE_MAX - m_nCounterControlRate) / RATE_MAX;

		SPL("New Duration: %d", nNewDurationFrm);

		if(nNewDurationFrm <= poAuraInfo->GetEffectMinFrm())
		{
			poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(g_GWConfig.nGlobalSubstituteAuraID);
			LOG_PROCESS_ERROR_RET_CODE(poAuraInfo, errAuraNotExist);
		}
	}

	eEffectType = poAuraInfo->GetAttr(aatType);
	LOG_PROCESS_ERROR_RET_CODE(eEffectType > auetInvalid && eEffectType < auetTotal, errAuraConfigError);

	eStackTypeSameIdSameCaster = poAuraInfo->GetAttr(aatStackTypeSameIdSameCaster);
	LOG_PROCESS_ERROR_RET_CODE(eStackTypeSameIdSameCaster > austInvalid && eStackTypeSameIdSameCaster < austTotal, errAuraConfigError);

	eStackTypeSameTypeSameCaster = poAuraInfo->GetAttr(aatStackSameTypeSameCaster);
	LOG_PROCESS_ERROR_RET_CODE(eStackTypeSameTypeSameCaster > austInvalid && eStackTypeSameTypeSameCaster < austTotal, errAuraConfigError);

//	eStackTypeSameGroupSameCaster = poAuraInfo->GetAttr(aatStackSameGroupSameCaster);
//	LOG_PROCESS_ERROR(eStackTypeSameGroupSameCaster > austInvalid && eStackTypeSameGroupSameCaster < austTotal);

	eStackTypeSameIdDiffCaster = poAuraInfo->GetAttr(aatStackTypeSameIdDiffCaster);
	LOG_PROCESS_ERROR_RET_CODE(eStackTypeSameIdDiffCaster > austInvalid && eStackTypeSameIdDiffCaster < austTotal, errAuraConfigError);

	eStackTypeSameTypeDiffCaster = poAuraInfo->GetAttr(aatStackSameTypeDiffCaster);
	LOG_PROCESS_ERROR_RET_CODE(eStackTypeSameTypeDiffCaster > austInvalid && eStackTypeSameTypeDiffCaster < austTotal, errAuraConfigError);

//	eStackTypeSameGroupDiffCaster = poAuraInfo->GetAttr(aatStackSameGroupDiffCaster);
//	LOG_PROCESS_ERROR(eStackTypeSameGroupDiffCaster > austInvalid && eStackTypeSameGroupDiffCaster < austTotal);

	nGroup = poAuraInfo->GetAttr(aatGroup);

	// Get the object’s attribute list
	aura = m_AuraList.BeginAura(eEffectType);

	while((aura = m_AuraList.NextAura(eEffectType, aura)) != NULL)
	{
		// If the attribute list already contains an attribute with the same ID
		if(aura->GetID() == poAuraInfo->GetID())
		{
			// If the caster is the target
			if(aura->GetCasterID() == poCaster->GetObjID())
			{
				poExistAura1 = aura;
			}
			else
			{
				poExistAura2 = aura;
			}
		}
		// If the attribute list does not contain this ID
		else if(aura->GetAttr(aatType) == eEffectType)
		{
			// If the caster is the target
			if(aura->GetCasterID() == poCaster->GetObjID())
			{
				poExistAura3 = aura;
			}
			else
			{
				poExistAura4 = aura;
			}
		}
	}

	if(poExistAura1 != NULL)
	{
		PROCESS_SUCCESS(eStackTypeSameIdSameCaster == austNone);

		if(eStackTypeSameIdSameCaster == austStack)
			poRetAura = _AuraStack(poExistAura1, poAuraInfo, poCaster, pExParam, nNewDurationFrm, nNewStackCount); 
		else if(eStackTypeSameIdSameCaster == austSubstitute)
			poRetAura = _AuraSubstitute(poExistAura1, poAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel); 
		else if(eStackTypeSameIdSameCaster == austCoexist)
			poRetAura = _AuraNew(poAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel);
	}
	else if(poExistAura2 != NULL)
	{
		PROCESS_SUCCESS(eStackTypeSameIdDiffCaster == austNone);

		if(eStackTypeSameIdDiffCaster == austStack)
			poRetAura = _AuraStack(poExistAura2, poAuraInfo, poCaster, pExParam, nNewDurationFrm, nNewStackCount); 
		else if(eStackTypeSameIdDiffCaster == austSubstitute)
			poRetAura = _AuraSubstitute(poExistAura2, poAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel); 
		else if(eStackTypeSameIdDiffCaster == austCoexist)
			poRetAura = _AuraNew(poAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel);
	}
	else if(poExistAura3 != NULL)
	{
		PROCESS_SUCCESS(eStackTypeSameTypeSameCaster == austNone);
		LOG_PROCESS_ERROR_RET_CODE(eStackTypeSameTypeSameCaster != austStack, errAuraConfigError);

		if(eStackTypeSameTypeSameCaster == austSubstitute)
			poRetAura = _AuraSubstitute(poExistAura3, poAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel); 
		else if(eStackTypeSameTypeSameCaster == austCoexist)
			poRetAura = _AuraNew(poAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel);
	}
	else if(poExistAura4 != NULL)
	{
		PROCESS_SUCCESS(eStackTypeSameTypeDiffCaster == austNone);
		LOG_PROCESS_ERROR_RET_CODE(eStackTypeSameTypeDiffCaster != austStack, errAuraConfigError);

		if(eStackTypeSameTypeDiffCaster == austSubstitute)
			poRetAura = _AuraSubstitute(poExistAura4, poAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel); 
		else if(eStackTypeSameTypeDiffCaster == austCoexist)
			poRetAura = _AuraNew(poAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel);
	}
	else
		poRetAura = _AuraNew(poAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel);

	PROCESS_ERROR_RET_CODE(poRetAura, errAuraAppendFailed);

	if(poCaster->GetObjID() != m_dwObjID && poAuraInfo->GetAttr(aatCategory) == acNegative)
		SetLastCasterID(poCaster->GetObjID());

	if(poAuraInfo->GetAttr(aatControlDecay))
	{
		m_nTotalControlCount++;
		m_nCounterControlRate += nNewDurationFrm * g_GWConfig.nControlParamA;
		m_nNextControlDecayFrm = 0;
	}
Exit1:
	return poRetAura;
Exit0:
#ifndef _ROBOT
	if (poCaster != NULL && poCaster->GetObjType() == otRole && poAuraInfo != NULL)
	{
		pRole = static_cast<CRole*>(poCaster);
		if(pRole) pRole->ErrorCodeNotify(nResult, poAuraInfo->GetID(), 0);
	}
#endif //_ROBOT
	if(poRetAura != NULL)
		poRetAura->UnInit();

	return NULL;
}

BOOL CCharacter::_CheckAuraAppend(CAura* poAura)
{
	INT32 nRetCode = 0;
	INT32 eResistType = poAura->GetAttr(aatResistType);
	INT32 eEffectType = poAura->GetAttr(aatType);

	LOG_PROCESS_ERROR(eResistType >= aurtNone && eResistType < aurtTotal);

	if (eEffectType == auetType07 || eEffectType == auetType08)
	{
		if (eResistType == aurtPoison)
		{
			nRetCode = g_Random(0, (m_AttrList.GetAttr(catPoisonResist) + 7000) / 1000) <= 6;
			PROCESS_ERROR(nRetCode);
		}
	}
	else if (eEffectType == auetType17)
	{
		if (eResistType == aurtDazzy)
		{
			nRetCode = g_Random(0, (5000 + m_AttrList.GetAttr(catPoisonResist) + m_AttrList.GetAttr(catStunResist)) / 1000) == 0;
			PROCESS_ERROR(nRetCode);
		}
	}
	else if (eEffectType == auetType24)
	{
		if (eResistType == aurtBurn)
		{
			PROCESS_ERROR(g_Random(0, RATE_MAX) >= m_AttrList.GetAttr(catBurnResist));
		}
	}
	else if (eEffectType == auetType01)
	{
		if (eResistType == aurtRepel)
		{
			PROCESS_ERROR(g_Random(0, RATE_MAX) >= m_AttrList.GetAttr(catRepelResist));
		}
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL __RefreshAuraEffect(CAura* pAuraInfo, ExtraParam* pExParam, INT32& nNewEffectPct, INT32& nNewEffectValue)
{
	INT32 nNewEffectPctX = 0;
	INT32 nNewEffectValueX = 0;

	LOG_PROCESS_ERROR(pAuraInfo);

	nNewEffectPct = pAuraInfo->GetAttr(aatEffectPct);
	nNewEffectValue = pAuraInfo->GetAttr(aatEffectValue);

	PROCESS_SUCCESS(pExParam == NULL);

	nNewEffectPctX = pAuraInfo->GetAttr(aatEffectPctX);
	nNewEffectValueX = pAuraInfo->GetAttr(aatEffectValueX);

	LOG_PROCESS_ERROR(nNewEffectPctX >= mxtUseNone && nNewEffectPctX < mxtTotal);
	LOG_PROCESS_ERROR(nNewEffectValueX >= mxtUseNone && nNewEffectValueX < mxtTotal);

	if(nNewEffectPctX > mxtUseNone)
		nNewEffectPct = pExParam->m_nX[nNewEffectPctX - 1];

	if(nNewEffectValueX > mxtUseNone)
		nNewEffectValue = pExParam->m_nX[nNewEffectValueX - 1];

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}

CAuraInstance* CCharacter::_NewAuraInstance(CAura* pAuraInfo, CCharacter* poCaster, UINT32 dwFromObjID, ExtraParam* pExParam, INT32 nNewDurationFrm, INT32 nNewStackCount, INT32 nRootSpellID, INT32 nRootSpellLevel)
{
	INT32 nRetCode = 0;
	CAuraInstance* poAuraInst = NULL;
	INT32 nInitStackCount = nNewStackCount;
	INT32 nNewEffectPct = 0;
	INT32 nNewEffectValue = 0;

	LOG_PROCESS_ERROR(pAuraInfo);

	poAuraInst = CGameWorld::GetInstance().NewAuraInstance();
	LOG_PROCESS_ERROR(poAuraInst);

	if(nNewStackCount <= 0)
		nInitStackCount = pAuraInfo->GetAttr(aatInitStackCount);

	if(m_dwAuraGenerator > 0x00FFFFFF)
		m_dwAuraGenerator = 0;

	poAuraInst->Init(pAuraInfo, pExParam, poCaster, dwFromObjID, MakeAuraInstanceKey(pAuraInfo->GetAttr(aatType), m_dwAuraGenerator++), nInitStackCount);

	poAuraInst->SetSpellID(nRootSpellID);
	poAuraInst->SetSpellLevel(nRootSpellLevel);

	if(nNewDurationFrm > 0)
		poAuraInst->ResetFrm(nNewDurationFrm, 0, FALSE);

	nRetCode = __RefreshAuraEffect(pAuraInfo, pExParam, nNewEffectPct, nNewEffectValue);
	LOG_PROCESS_ERROR(nRetCode);

	nNewEffectPct *= nInitStackCount;
	nNewEffectValue *= nInitStackCount;

	poAuraInst->SetEffectPct(nNewEffectPct);
	poAuraInst->SetEffectValue(nNewEffectValue);

	return poAuraInst;
Exit0:
	if(poAuraInst != NULL)
	{
		poAuraInst->UnInit();
	}
	return NULL;
}

BOOL CCharacter::_AuraNew(CAuraInstance* poAuraInst, CCharacter* poCaster)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poAuraInst);
	LOG_PROCESS_ERROR(poCaster);

	PushAura(poAuraInst);
	nRetCode = poAuraInst->OnAppend(poCaster, this);
	PopAura();
	LOG_CHECK_ERROR(nRetCode);

//	LOG_PROCESS_ERROR(poAuraInst->GetEffectPct() == nNewEffectPct && poAuraInst->GetEffectValue() == nNewEffectValue);

	PushAura(poAuraInst);
	nRetCode = poAuraInst->AfterStack(this);
	PopAura();
	LOG_CHECK_ERROR(nRetCode);

	m_AuraList.AddNewAura(poAuraInst);

	SPL("------ Aura New : %d, %u", poAuraInst->GetID(), GetObjID());

#ifndef _ROBOT
	if (!IsLoading())
	{
		if (!poAuraInst->GetAttr(aatHiddenToSelf))
		{
			nRetCode = CCliMessageHandler::GetInstance().DoNewAura(this, poAuraInst);
			//	LOG_CHECK_ERROR(nRetCode);
		}
	}
#endif

	return TRUE;
Exit0:
	return FALSE;
}

CAuraInstance* CCharacter::_AuraNew(CAura* pAuraInfo, CCharacter* poCaster, UINT32 dwFromObjID, ExtraParam* pExParam, INT32 nNewDurationFrm, INT32 nNewStackCount, INT32 nRootSpellID, INT32 nRootSpellLevel)
{
	INT32 nRetCode = 0;
	CAuraInstance* poAuraInst = NULL;
	INT32 nInitStackCount = nNewStackCount;
	INT32 nNewEffectPct = 0;
	INT32 nNewEffectValue = 0;

	LOG_PROCESS_ERROR(pAuraInfo);

	poAuraInst = _NewAuraInstance(pAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel);
	LOG_PROCESS_ERROR(poAuraInst);

//	if (!IsLoading())
//	{
	nRetCode = _AuraNew(poAuraInst, poCaster);
	LOG_PROCESS_ERROR(nRetCode);
//	}
//	else
//		m_PendingAuraList.push_back(poAuraInst);

	return poAuraInst;
Exit0:
	if(poAuraInst != NULL)
	{
		poAuraInst->UnInit();
	}

	return NULL;
}

CAuraInstance* CCharacter::_AuraStack(CAuraInstance* poExistAura, CAura* poAuraInfo, CCharacter* poCaster, ExtraParam* pExParam, INT32 nNewDuration, INT32 nNewStackCount)
{
	INT32 nRetCode = 0;
	INT32 nTimeType = 0;
	INT32 nRetDuration = 0;
	INT32 nDurationFrm = 0;
	INT32 nAddStackCnt = 0;
	INT32 nInitStackCount = nNewStackCount;
	INT32 nRemainFrm = 0;
	INT32 nNewEffectPct = 0;
	INT32 nNewEffectValue = 0;

	LOG_PROCESS_ERROR(poExistAura != NULL);
	LOG_PROCESS_ERROR(poAuraInfo != NULL);
	LOG_PROCESS_ERROR(poExistAura->GetID() == poAuraInfo->GetID());

	if(nNewStackCount <= 0)
		nInitStackCount = poAuraInfo->GetAttr(aatInitStackCount);

	nAddStackCnt = poExistAura->GetAttr(aatMaxStackCount) - poExistAura->GetStackCount();

	nAddStackCnt = nAddStackCnt > nInitStackCount ? nInitStackCount : nAddStackCnt;


	nDurationFrm = nNewDuration > 0 ? nNewDuration : poAuraInfo->GetDurationFrm();
	nRemainFrm = poExistAura->GetRemainFrm();

	if(poAuraInfo->GetAttr(aatStackResetTime))
		nRetDuration = nDurationFrm;
	else
	{
		nRetDuration = nDurationFrm > nRemainFrm ? nDurationFrm : nRemainFrm;
	}

	if(nRetDuration != nRemainFrm)
	{
		poExistAura->ResetFrm(nRetDuration, 0, TRUE);
	}

	PROCESS_SUCCESS(nAddStackCnt <= 0);

	nRetCode = __RefreshAuraEffect(poAuraInfo, pExParam, nNewEffectPct, nNewEffectValue);
	LOG_PROCESS_ERROR(nRetCode);

	nNewEffectPct *= nAddStackCnt;
	nNewEffectValue *= nAddStackCnt;

	if (poAuraInfo->GetAttr(aatEffectDecay))
	{
		nNewEffectPct = (poExistAura->GetEffectPct() * nRemainFrm + nNewEffectPct * nDurationFrm) / nRetDuration;
		nNewEffectValue = (poExistAura->GetEffectValue() * nRemainFrm + nNewEffectValue * nDurationFrm) / nRetDuration;
	}
	else
	{
		nNewEffectPct = poExistAura->GetEffectPct() + nNewEffectPct;
		nNewEffectValue = poExistAura->GetEffectValue() + nNewEffectValue;
	}


	PushAura(poExistAura);
	nRetCode = poExistAura->OnUnStack(this);
	PopAura();
	LOG_CHECK_ERROR(nRetCode);

	poExistAura->SetEffectPct(nNewEffectPct);
	poExistAura->SetEffectValue(nNewEffectValue);
	poExistAura->SetStrengthLevel(poAuraInfo->GetAttr(aatStrengthLevel) + poExistAura->GetStrengthLevel());
	poExistAura->SetStackCount(poExistAura->GetStackCount() + nAddStackCnt);

	SPL("------ Aura Stack : %d, %u, %d stack added", poExistAura->GetID(), GetObjID(), nAddStackCnt);

	PushAura(poExistAura);
	nRetCode = poExistAura->AfterStack(this);
	PopAura();
	LOG_CHECK_ERROR(nRetCode);

Exit1:
//#ifndef _ROBOT
//	CCliMessageHandler::GetInstance().DoAuraChange(this, poExistAura);
//#endif

	return poExistAura;
Exit0:
	return NULL;
}

CAuraInstance* CCharacter::_AuraSubstitute(CAuraInstance* poExistAura, CAura* pAuraInfo, CCharacter* poCaster, UINT32 dwFromObjID, ExtraParam* pExParam, INT32 nNewDurationFrm, INT32 nNewStackCount, INT32 nRootSpellID, INT32 nRootSpellLevel)
{
	CAuraInstance* aura = nullptr;
	if (pAuraInfo->GetAttr(aatTimeCover))
	{
		PROCESS_SUCCESS(poExistAura->GetRemainFrm() >= nNewDurationFrm);
	}

	PROCESS_SUCCESS(poExistAura->GetStrengthLevel() > pAuraInfo->GetAttr(aatStrengthLevel));

	RemoveAura(poExistAura, FALSE);

	aura = _AuraNew(pAuraInfo, poCaster, dwFromObjID, pExParam, nNewDurationFrm, nNewStackCount, nRootSpellID, nRootSpellLevel);
	LOG_PROCESS_ERROR(aura);

	SPL("------ Aura Substitute : %d, %u", poExistAura->GetID(), GetObjID());

	return aura;
Exit1:
	return poExistAura;
Exit0:
	return NULL;
}

void CCharacter::RemoveAura(CAuraInstance* poAura, BOOL bTrigOnRemove)
{
	INT32 nRetCode = 0;

	PROCESS_ERROR(!poAura->NeedRemove());

	SPL("------ Remove Aura: %d, %u", poAura->GetID(), GetObjID());

//	if (poAura->GetID() == 51170)
//		DBG("aura 51170 !!");

#ifndef _ROBOT
	if (!IsLoading())
	{
		if (!poAura->GetAttr(aatHiddenToSelf))
		{
			CCliMessageHandler::GetInstance().DoDelAura(this, poAura);
		}
	}
#endif

	m_AuraList.RemoveAura(poAura);

	PushAura(poAura);

	nRetCode = poAura->OnUnStack(this);
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = poAura->OnRemove(this, bTrigOnRemove);
	LOG_CHECK_ERROR(nRetCode);

	PopAura();

	if(poAura->GetAttr(aatControlDecay))
	{
		m_nTotalControlCount--;
		LOG_CHECK_ERROR(m_nTotalControlCount >= 0);

		m_nNextControlDecayFrm = CGameWorld::GetInstance().GetGameFrame() + g_GWConfig.nControlIntervalB;
	}

Exit0:
	return;
}

BOOL CCharacter::DispelAura(CAuraInstance* poAura, INT32 nCount)
{
	INT32 nRetCode = 0;
	INT32 nStackCount = 0;
	BOOL bTrigOnRemove = FALSE;

	LOG_PROCESS_ERROR(poAura);
	PROCESS_ERROR(!poAura->NeedRemove());
	PROCESS_ERROR(poAura->GetAttr(aatCanDispel));

	bTrigOnRemove = poAura->GetAttr(aatTrigOnRemoveWhenDispel);

	nStackCount = poAura->GetStackCount();

	SPL("Dispel Aura: %d, %u", poAura->GetID(), GetObjID());

	PushAura(poAura);
	nRetCode = poAura->OnDispel(this);
	PopAura();
	LOG_CHECK_ERROR(nRetCode);

	ReduceAuraStack(poAura, nCount);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::RemoveAuraByType(INT32 nType)
{
	INT32 nRoll = 0;
	CAuraInstance* aura = NULL;

	LOG_PROCESS_ERROR(nType > auetInvalid && nType < auetTotal);

	aura = m_AuraList.BeginAura(nType);

	while((aura = m_AuraList.NextAura(nType, aura)) != NULL)
	{
		RemoveAura(aura, TRUE);
	}

	return TRUE;
Exit0:
	return FALSE;

}

BOOL CCharacter::RemoveAuraBySubType(INT32 nSubType)
{
	INT32 nRoll = 0;
	CAuraInstance* aura = NULL;

	for(INT32 i = auetNone; i < auetTotal; i++)
	{
		aura = m_AuraList.BeginAura(i);

		while((aura = m_AuraList.NextAura(i, aura)) != NULL)
		{
			if(aura->GetAttr(aatGroup) == nSubType)
			{
				RemoveAura(aura, TRUE);
				return TRUE;
			}
		}
	}

	return FALSE;

}

void CCharacter::ReduceAuraStack(CAuraInstance* poAura, INT32 nReduceCount)
{
	SPL(">>>>>>>>>>>> Reduce Aura Stack: %d : %s", poAura->GetID(), GetName());

	INT32 nRetCode = 0;
	INT32 nStackCount = poAura->GetStackCount();

	INT32 nOldEffectPct = poAura->GetEffectPct();
	INT32 nOldEffectValue = poAura->GetEffectValue();

	INT32 nNewEffectPct = 0;
	INT32 nNewEffectValue = 0;

	nReduceCount = nReduceCount > nStackCount ? nStackCount : nReduceCount;

	LOG_PROCESS_ERROR(nReduceCount > 0);

	nNewEffectPct = nOldEffectPct * nReduceCount / nStackCount;
	nNewEffectValue = nOldEffectValue * nReduceCount / nStackCount;

	poAura->SetEffectPct(nNewEffectPct);
	poAura->SetEffectValue(nNewEffectValue);

	PushAura(poAura);
	nRetCode = poAura->OnUnStack(this);
	PopAura();
	LOG_CHECK_ERROR(nRetCode);

	poAura->SetEffectPct(nOldEffectPct - nNewEffectPct);
	poAura->SetEffectValue(nOldEffectValue - nNewEffectValue);

	poAura->SetStackCount(nStackCount - nReduceCount);

	if(poAura->GetStackCount() <= 0)
		RemoveAura(poAura, TRUE);
Exit0:
	return;
}

BOOL CCharacter::ReduceAuraStackByType(INT32 nCount, INT32 nType)
{
	INT32 nRoll = 0;
	CAuraInstance* aura = NULL;

	LOG_PROCESS_ERROR(nType > auetInvalid && nType < auetTotal);

	aura = m_AuraList.BeginAura(nType);

	while((aura = m_AuraList.NextAura(nType, aura)) != NULL)
	{
		ReduceAuraStack(aura, nCount);
	}

	return TRUE;
Exit0:
	return FALSE;

}

BOOL CCharacter::ReduceAuraStackBySubType(INT32 nCount, INT32 nSubType)
{
	INT32 nRoll = 0;
	CAuraInstance* aura = NULL;

	for(INT32 i = auetNone; i < auetTotal; i++)
	{
		aura = m_AuraList.BeginAura(i);

		while((aura = m_AuraList.NextAura(i, aura)) != NULL)
		{
			if(aura->GetAttr(aatGroup) == nSubType)
			{
				ReduceAuraStack(aura, nCount);
			}
		}
	}

	return FALSE;

}

CAuraInstance* CCharacter::GetFirstAura(INT32 nAuraID)
{
	CAuraInstance* ret = NULL;

	CAuraTemplate* t = CSpellConfig::GetInstance().FindAuraTemplate(nAuraID);
	LOG_PROCESS_ERROR(t);

	ret = m_AuraList.BeginAura(t->GetAttr(aatType));

	while((ret = m_AuraList.NextAura(t->GetAttr(aatType), ret)) != NULL)
	{
		PROCESS_SUCCESS(ret->GetID() == nAuraID);
	}

Exit1:
Exit0:
	return ret;
}

CAuraInstance* CCharacter::GetFirstAuraByType(INT32 nType)
{
	CAuraInstance* poAura = NULL;

	LOG_PROCESS_ERROR(nType > auetInvalid && nType < auetTotal);

	poAura = m_AuraList.BeginAura(nType);

	while((poAura = m_AuraList.NextAura(nType, poAura)) != NULL)
	{
		return poAura;
	}

	return poAura;
Exit0:
	return NULL;
}

CAuraInstance* CCharacter::GetFirstAuraBySubType(INT32 nSubType)
{
	CAuraInstance* poAura = NULL;

	for(INT32 i = auetNone; i < auetTotal; i++)
	{
		poAura = m_AuraList.BeginAura(i);

		while((poAura = m_AuraList.NextAura(i, poAura)) != NULL)
		{
			if(poAura->GetAttr(aatGroup) == nSubType)
			{
				return poAura;
			}
		}
	}

	return NULL;
}


CAuraInstance* CCharacter::GetAura(UINT32 dwInstanceID)
{
	CAuraInstance* ret = NULL;

	INT32 nType = GetAuraTypeFromInstanceID(dwInstanceID);
	LOG_PROCESS_ERROR(nType > auetInvalid && nType < auetTotal);

	ret = m_AuraList.BeginAura(nType);

	while((ret = m_AuraList.NextAura(nType, ret)) != NULL)
	{
		PROCESS_SUCCESS(ret->GetInstanceID() == dwInstanceID);
	}

Exit1:
	return ret;
Exit0:
	return NULL;
}


BOOL CCharacter::UpdateSingleAura(CAuraInstance* poAura, BOOL bForce)
{
	INT32 nRetCode = 0;
	BOOL bTimeOut = FALSE;
	CCharacter* poAuraCaster = NULL;

	INT32 nNowFrm = CGameWorld::GetInstance().GetGameFrame();

	if (bForce)
		nNowFrm = 0x7FFFFFFF;

	LOG_PROCESS_ERROR(poAura);

	poAuraCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
	if (!poAura->GetAttr(aatSaveToDB) && 
		(poAuraCaster == NULL || poAuraCaster->GetScene() != m_poScene || 
		 poAuraCaster->GetDistance3D(m_llX, m_llY, m_llZ) >= MAX_SPELL_EFFECT_DISTANCE * MAX_SPELL_EFFECT_DISTANCE))
	{
		RemoveAura(poAura, FALSE);
	}
	else
	{
		PushAura(poAura);
		nRetCode = poAura->Update(this, nNowFrm, bTimeOut);
		PopAura();

#ifndef _ROBOT
		if (poAura->NeedSync() && !poAura->GetAttr(aatHiddenToSelf))
		{
			nRetCode = CCliMessageHandler::GetInstance().DoAuraChange(this, poAura);
			LOG_PROCESS_ERROR(nRetCode);

			poAura->ResetSync();
		}
#endif

		if (bTimeOut || (m_oFlag.CheckFlag(cclInvincible) && (poAura->GetAttr(aatCategory) == acNegative)))
			RemoveAura(poAura, poAura->GetAttr(aatTrigOnRemoveWhenTimeup));
	}

	return TRUE;
Exit0:
	return FALSE;
}



BOOL CCharacter::_OnIdle(void)
{
	return TRUE;
}

//BOOL CCharacter::_OnPreCast(INT32 nFrm)
//{
//	return TRUE;
//}

BOOL CCharacter::_OnChanneling(void)
{
	INT32 nRetCode = 0;
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();

	LOG_PROCESS_ERROR(m_poCastingSpell);

	if(m_CastTimer.Trig(nFrm))
	{
		nRetCode = m_poCastingSpell->DoEffect(this);
		PROCESS_ERROR(nRetCode);
	}


	PROCESS_ERROR(!m_poCastingSpell->IsSelfCancened());
	PROCESS_SUCCESS(m_poCastingSpell->IsChannelOver());

//	PROCESS_SUCCESS(m_eSpellState != cspsChanneling);

	return TRUE;
Exit1:
	return BreakCast(FALSE, TRUE);
Exit0:
	return BreakCast(TRUE, FALSE);
}

BOOL CCharacter::_OnPreparing(void)
{
	INT32 nRetCode = 0;
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();

	LOG_PROCESS_ERROR(m_poCastingSpell);

	if(!m_PreparingTimer.Trig(nFrm))
		return TRUE;

	nRetCode = m_poCastingSpell->DoEffect(this);
//	LOG_CHECK_ERROR(nRetCode);

	PROCESS_SUCCESS(m_eSpellState != cspsPreparing);

	nRetCode = Hard();
	LOG_CHECK_ERROR(nRetCode);

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::_OnPreAccumulating(void)
{
	INT32 nAccumulateFrm = 0;
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();
	LOG_PROCESS_ERROR(m_poCastingSpell);

	if(!m_PreparingTimer.Trig(nFrm))
		return TRUE;

	nAccumulateFrm = m_poCastingSpell->GetPrepareTimeMaxFrm() - m_poCastingSpell->GetPrepareTimeMinFrm();

	m_eSpellState = cspsAccumulating;
	m_AccumulatingTimer.Enable(nAccumulateFrm, 1, nFrm, nAccumulateFrm);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::_OnAccumulating(void)
{
	INT32 nRetCode = 0;
	INT32 nAccumulateFrm = 0;
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();

	if(!m_AccumulatingTimer.Trig(nFrm))
		return TRUE;

	LOG_PROCESS_ERROR(m_poCastingSpell);

	nAccumulateFrm = m_poCastingSpell->GetPrepareTimeMaxFrm() - m_poCastingSpell->GetPrepareTimeMinFrm();

	nRetCode = m_poCastingSpell->DoEffect(this);
	LOG_CHECK_ERROR(nRetCode);

	PROCESS_SUCCESS(m_eSpellState != cspsAccumulating);

	nRetCode = Hard();
	LOG_CHECK_ERROR(nRetCode);

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::_OnOpening(void)
{
	INT32 nResult = 0;
	INT32 nRetCode = 0;
	CDoodad* pDoodad = NULL;
	INT64 llMaxDistance = 0;
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();

	PROCESS_SUCCESS(!m_PreparingTimer.Trig(nFrm));

	pDoodad = CGameWorld::GetInstance().FindDoodad(m_dwOperateDoodadID);
	PROCESS_ERROR(pDoodad);

	PROCESS_ERROR_RET_CODE(pDoodad->GetScene() == GetScene(), errDoodadOutOfRange);
	llMaxDistance = g_GWConfig.nDoodadOperateDistance * POINT_PER_CM;
	PROCESS_ERROR_RET_CODE(GetDistance3D(pDoodad) < llMaxDistance * llMaxDistance, errDoodadOutOfRange);

	m_eSpellState = cspsIdle;
	m_PreparingTimer.Disable();

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoOperateDoodadEnd(this, m_dwOperateDoodadID, FALSE);
#endif  // _ROBOT

    nRetCode = OpenDoodad(pDoodad, 0);
    LOG_PROCESS_ERROR(nRetCode);

Exit1:
	return TRUE;
Exit0:

	return FALSE;
}

BOOL CCharacter::_OnHard(void)
{
	INT32 nRetCode = 0;
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();
	LOG_PROCESS_ERROR(m_poCastingSpell);

	PROCESS_SUCCESS(m_poCastingSpell->IsFastStop());

	if(!m_HardTimer.Trig(nFrm))
		return TRUE;

//	DBG("End Hard : %d, Frame: %d", m_poCastingSpell->GetID(), nFrm);

Exit1:
	nRetCode = _OnCastOver();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::Hard(void)
{
	INT32 nRetCode = 0;
	INT32 nHardFrm = 0;
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();
	LOG_PROCESS_ERROR(m_poCastingSpell);

	nHardFrm = m_poCastingSpell->GetHardTimeFrm();

	if (m_poCastingSpell->GetAttr(satHardTimeAffectedByCharLevel))
	{
		INT32 nHardTime = nHardFrm * 1000 / GAME_FPS  - g_Min(g_GWConfig.nHardParamA, g_Min(g_GWConfig.nHardParamB, m_nLevel * g_GWConfig.nHardParamC) + g_GWConfig.nHardParamD * m_AttrList.GetAttr(catAtkSpeed));
		nHardFrm = CONVERT_TIME(nHardTime);
	}

	nHardFrm += CONVERT_TIME(m_AttrList.GetAttr(catExtraHardTime));

	if(m_poCastingSpell->IsFastStop())
		nHardFrm = 0;
	else if (m_poCastingSpell->GetAttr(satCanMoveInHardTime))
	{
		INT32 nWalkDelayFrm = CONVERT_TIME(m_poCastingSpell->GetAttr(satWalkDelayTime));
		INT32 nRunDelayFrm = CONVERT_TIME(m_poCastingSpell->GetAttr(satRunDelayTime));

		nWalkDelayFrm = g_Max(0, nWalkDelayFrm - m_poCastingSpell->GetPrepareTimeMinFrm() - m_poCastingSpell->GetIntervalFrm() * (m_poCastingSpell->GetAttr(satChannelCount) - 1));
		nRunDelayFrm = g_Max(0, nRunDelayFrm - m_poCastingSpell->GetPrepareTimeMinFrm() - m_poCastingSpell->GetIntervalFrm() * (m_poCastingSpell->GetAttr(satChannelCount) - 1));

//		if (m_poCastingSpell->HasHitRole() && (m_poCastingSpell->GetAttr(satAttackType) == spatMag || m_poCastingSpell->GetAttr(satAttackType) == spatTao))
//		{
//			INT32 nRandTime = 300 + g_Random(0, 1100);
//			nWalkDelayFrm = g_Max(nWalkDelayFrm, CONVERT_TIME(nRandTime));
//			nRunDelayFrm = g_Max(nRunDelayFrm, CONVERT_TIME(nRandTime));
//		}

		m_nWalkTimer = nFrm + nWalkDelayFrm;
		m_nRunTimer = nFrm + nRunDelayFrm;
	}

//	DBG("Start Hard : %d, Frame: %d %d", m_poCastingSpell->GetID(), nHardFrm, nFrm);

	if(nHardFrm <= 0)
	{
		nRetCode = _OnCastOver();
		LOG_PROCESS_ERROR(nRetCode);

		return TRUE;
	}

	m_eSpellState = cspsHard;
	m_HardTimer.Enable(nHardFrm, 1, nFrm, nHardFrm);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::Stiff(BOOL bStiff, INT32 nFrm, INT32 nParam)
{
	INT32 nRetCode = 0;

	if(bStiff)
	{
		PROCESS_ERROR(nFrm);
		PROCESS_SUCCESS(m_oStiffFlag.GetFlag() == 0);

		m_nNextStiffDecayFrm = 0;

		nFrm = nFrm * (RATE_MAX - m_nCounterStiffRate) / RATE_MAX;
		nFrm = nFrm < 0 ? 0 : nFrm;

		PROCESS_SUCCESS(nFrm <= 0);

		PROCESS_SUCCESS(CGameWorld::GetInstance().GetGameFrame() + nFrm < m_nStiffEndFrm);

		m_nStiffEndFrm = CGameWorld::GetInstance().GetGameFrame() + nFrm;

		m_nCounterStiffRate += nFrm * g_GWConfig.nStiffParamA;
		m_nCounterStiffRate = m_nCounterStiffRate > RATE_MAX ? RATE_MAX : m_nCounterStiffRate;

		for(INT32 i = cstfBegin; i < cstfTotal; i++)
		{
			if(m_oStiffFlag.CheckFlag(i))
			{
				nRetCode = (this->*m_RunStiffFunc[i])(nParam);
				LOG_CHECK_ERROR(nRetCode);
			}
		}
	}
	else
	{
		if(m_oStiffFlag.CheckFlag(cstfFreeze))
		{
#ifndef _ROBOT
			CCliMessageHandler::GetInstance().DoCharFreeze(this, FALSE);
#endif
		}

		m_oStiffFlag.ResetFlag();

		m_nNextStiffDecayFrm = CGameWorld::GetInstance().GetGameFrame() + g_GWConfig.nStiffIntervalB;
		m_nNextStiffDecayFrm = m_nNextStiffDecayFrm < 0 ? 0 : m_nNextStiffDecayFrm;
	}

Exit1:
	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::_OnStiffFreeze(INT32 nParam)
{
	INT32 nRetCode = 0;

	nRetCode = m_oStiffFlag.CheckFlag(cstfFreeze);
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = Stand(TRUE);
	LOG_PROCESS_ERROR(nRetCode);

#ifndef _ROBOT
	CCliMessageHandler::GetInstance().DoCharFreeze(this, TRUE);
#endif

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::_OnStiffKnockBack(INT32 nDirection)
{
	INT32 nRetCode = 0;
	INT64 llDistance = 0;
	INT32 nSpeed = 0;
	INT64 llDestX = 0;
	INT64 llDestY = 0;

	PROCESS_ERROR(_CanPassiveMove());

	llDistance = CONVERT_LENGTH(g_GWConfig.nKnockBackDistance);
	nSpeed = CONVERT_SPEED(g_GWConfig.nKnockBackSpeed);
	//INT32 nDirection = NormalizeDirection(MAX_DIRECTION_COUNT / 2 + m_nFaceDirection);

	llDestX = m_llX + llDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
	llDestY = m_llY + llDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

	nRetCode = KnockBack(nSpeed, llDestX, llDestY);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::_OnStiffSilent(INT32 nParam)
{
	INT32 nRetCode = 0;

	BreakCast(TRUE, TRUE);

	return TRUE;
}

#ifndef _ROBOT
void CCharacter::CancelCast(void)
{
	INT32 nFrm = CGameWorld::GetInstance().GetGameFrame();

	PROCESS_ERROR(m_eSpellState > cspsInvalid && m_eSpellState < cspsTotal);
//	PROCESS_ERROR(m_poCastingSpell);

	if(m_CancelSpellStateFunc[m_eSpellState] != NULL)
	{
		(this->*m_CancelSpellStateFunc[m_eSpellState])();
	}

Exit0:
	return;
}
#endif

#ifdef _ROBOT
void CCharacter::CancelCast(void)
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	if(GetObjType() == otRole)
	{
		poRole = static_cast<CRole*>(this);
		LOG_PROCESS_ERROR(poRole);

		nRetCode = CRobotMessageHandler::GetInstance().DoCancelCast(poRole);
		LOG_PROCESS_ERROR(nRetCode);
	}


Exit0:
	return;
}
#endif

BOOL CCharacter::_DefaultCancelProc(void)
{
	return TRUE;
}

BOOL CCharacter::_OnCancelChanneling(void)
{
	LOG_PROCESS_ERROR(m_poCastingSpell);

	m_poCastingSpell->SetSelfCancel(TRUE);

	if(m_poCastingSpell->IsChannelOver())
		BreakCast(TRUE, FALSE);

	return TRUE;
Exit0:
	return FALSE; 
}

BOOL CCharacter::_OnCancelPreparing(void)
{
	LOG_PROCESS_ERROR(m_poCastingSpell);

	BreakCast(TRUE, FALSE);

	return TRUE;
Exit0:
	return FALSE; 
}

BOOL CCharacter::_OnCancelPreAccumulating(void)
{
	LOG_PROCESS_ERROR(m_poCastingSpell);

	BreakCast(TRUE, FALSE);

	return TRUE;
Exit0:
	return FALSE; 
}

BOOL CCharacter::_OnCancelAccumulating(void)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_poCastingSpell);

	nRetCode = m_poCastingSpell->DoEffect(this);
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = Hard();
	LOG_CHECK_ERROR(nRetCode);

	return TRUE;

Exit0:
	return FALSE; 
}

BOOL CCharacter::_CancelOpening(void)
{
	INT32 nRetCode = 0;

	m_eSpellState = cspsIdle;
	m_PreparingTimer.Disable();

#ifndef _ROBOT
	CCliMessageHandler::GetInstance().DoOperateDoodadEnd(this, m_dwOperateDoodadID, TRUE);
#endif  // _ROBOT

	return TRUE;
}

BOOL CCharacter::_OnCastOver(void)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(m_poCastingSpell);

	RollbackModifierTree(m_poCastingSpell->GetAttr(satOnPrepare), m_poCastingSpell);

	for(INT32 i = 0; i < MAX_EX_CAST; i++)
		RollbackModifierTree(m_poCastingSpell->GetData()->m_ExCastPoint[i].m_nExOnCast, m_poCastingSpell);

	RollbackModifierTree(m_poCastingSpell->GetAttr(satOnCast), m_poCastingSpell);

	NextSpellSerial();

	m_CastTimer.Disable();
	m_PreparingTimer.Disable();
	m_AccumulatingTimer.Disable();

	m_eSpellState = cspsIdle;
	m_eHitTag = sphtagCommon;

	if(m_poCastingSpell != NULL)
	{
		m_poCastingSpell->Release();
		m_poCastingSpell = NULL;
	}


	return TRUE;

Exit0:
	return FALSE;
}

BOOL CCharacter::BreakCast(BOOL bIsBreak, BOOL bHard)
{
	INT32 nRetCode = 0;
	PROCESS_ERROR(m_poCastingSpell);

	m_poCastingSpell->SetSelfCancel(!bIsBreak);

	if(!bHard)
		m_poCastingSpell->SetFastStop(TRUE);

	m_eSpellState = cspsBreaking;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL CCharacter::_OnBreak(void)
{
	INT32 nRetCode = 0;
	PROCESS_ERROR(m_poCastingSpell);

#ifndef _ROBOT
	CCliMessageHandler::GetInstance().DoCharBreakCast(this, m_poCastingSpell, !m_poCastingSpell->IsSelfCancened());
#endif  // _ROBOT

	ApplyModifierTree(m_poCastingSpell->GetAttr(satOnBreak), m_poCastingSpell);

	return Hard();
Exit0:
	return FALSE;
}

BOOL CCharacter::ApplyModifierTree(INT32 nNodeID, CSpellInstance* poPendingSpell)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poPendingSpell);

	PushSpell(poPendingSpell);
	nRetCode = CModifierTree::GetInstance().ApplyGlobalNode(nNodeID, this, poPendingSpell->GetExParam());
	PopSpell();

	PROCESS_ERROR(nRetCode >= 0);

	return nRetCode;
Exit0:
	return FALSE;
}

BOOL CCharacter::RollbackModifierTree(INT32 nNodeID, CSpellInstance* poPendingSpell)
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(poPendingSpell);

	PushSpell(poPendingSpell);
	nRetCode = CModifierTree::GetInstance().RollBackGlobalNode(nNodeID, this, poPendingSpell->GetExParam());
	PopSpell();

	PROCESS_ERROR(nRetCode >= 0);

	return nRetCode;
Exit0:
	return FALSE;
}

BOOL CCharacter::DirectlyHpChange(INT32 nValue)
{
	INT32 nRetCode = 0;

	ChangeHp(nValue, cdrAura);

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoDirectlyHpChangeEffect(this, nValue);
	LOG_PROCESS_ERROR(nRetCode);
#endif
	return TRUE;
Exit0:
	return FALSE;
}

void CCharacter::CheckMissileTag(void)
{
#ifndef _ROBOT
	if (m_pGridInstance->uMissileTag2)
	{
		CMissile* poMissile = m_poScene->FindMissileByPlacedGrid(m_llX / GRID_WIDTH, m_llY / GRID_WIDTH);
		if (poMissile)
			poMissile->OnCharEnter((CCharacter*)this);
	}
#endif

	return;
}

INT32 CCharacter::EquipRune(const RUNE_DATA* pRuneData, Item* poItem)
{
	return errUnknownError;
}

BOOL CCharacter::UnEquipRune(const RUNE_DATA* pRuneData, Item* poItem)
{
	return FALSE;
}


