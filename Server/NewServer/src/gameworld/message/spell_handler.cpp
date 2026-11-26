#include "stdafx.h"
#include "message_handler.h"
#include "imessage_notifier.h"
#include "g2c_message.h"

#include "zlib.h"
#include "server_def.h"
#include "id_def.h"
#include "game_world.h"
#include "role.h"
#include "spell_data.h"
#include "spell.h"
#include "spell_config.h"
#include "server_config.h"

#ifndef _ROBOT

using namespace ExternalMessage;

void CCliMessageHandler::OnCastSpellEx(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	C2G_CAST_SPELL_EX* pMsg = (C2G_CAST_SPELL_EX*)pData;
	CCharacter* poTarget = NULL;
	CSpellSet* poSpellSet = NULL;
	CSpellSet* poSwitchSet = NULL;

	CSpell* poSpell = NULL;
	INT32 nResetFrm = 0;
	INT32 nCacheFrm = 0;
	CSpellInstance* poCastingSpell = NULL;
	INT64 nTargetX = ConvertCoordN2S(pMsg->wTargetX);
	INT64 nTargetY = ConvertCoordN2S(pMsg->wTargetY);
	INT64 nTargetZ = ConvertCoordN2S(pMsg->wTargetZ);

	PROCESS_ERROR_RET_CODE(!poRole->IsFreeMode(), errFreeMode);

	// Turn to face the target
	if (pMsg->byRotateToTarget)
	{
		INT32 nNewDirection = g_GetDirection(poRole->GetX(), poRole->GetY(), nTargetX, nTargetY);

		if(nNewDirection < 0)
			nNewDirection = poRole->GetFaceDirection();

		if(nNewDirection != poRole->GetFaceDirection())
		{ 
			INT32 nMirDir8 = g_GetMirDirection8(poRole->GetX() / GRID_WIDTH, poRole->GetY() / GRID_WIDTH, nTargetX / GRID_WIDTH, nTargetY / GRID_WIDTH);
			poRole->RotateToMirDir8(nMirDir8, 0);

			//DBG("----------msg -----------");
			//DBG("RotateTo: %d", NormalizeDirection(g_GetDirection(poRole->GetX(), poRole->GetY(), nTargetX, nTargetY)));
			//DBG("TargetPos: %d, %d", nTargetX / GRID_WIDTH, nTargetY / GRID_WIDTH);
			//DBG("--------------------------------");
		}
	}

	// Get the spell object
	poSpellSet = poRole->GetSpellBook().GetSpellSet(pMsg->wSpellID);

	PROCESS_ERROR_RET_CODE(poSpellSet != NULL, errSpellNotExists);
	//PROCESS_ERROR_RET_CODE(poSpellSet->IsEnabled(), errSpellNotExists);

	//DBG("req serial: %d", pMsg->bySerial);

	poSpell = poSpellSet->GetSpell();
	LOG_PROCESS_ERROR(poSpell);

	{
		// Whether to cancel the current spell
		CSpellInstance* poCastingSpell = poRole->GetCastingSpell();    // Get the currently casting spell
		if (poCastingSpell != NULL)
		{
			PROCESS_ERROR_RET_CODE(poCastingSpell->GetID() != poSpell->GetID(), errSpellCasting);

			INT32 nCancelType = poCastingSpell->GetAttr(satCancelType);

			if (nCancelType == spcltInstant || (nCancelType == spcltDependOnValue && poSpell->GetAttr(satCancelLevel) > poCastingSpell->GetAttr(satCancelLevel)))
			{
				// SPL("Cancel spell: %d", poCastingSpell->GetID());
				poRole->BreakCast(TRUE, FALSE);
				poRole->CacheSpell(pMsg->wSpellID, pMsg->bySerial, pMsg->dwTargetID, nTargetX, nTargetY, nTargetZ);
				return;
			}
			else if (nCancelType == spcltNone || (nCancelType == spcltDependOnValue && poSpell->GetAttr(satCancelLevel) <= poCastingSpell->GetAttr(satCancelLevel)))
			{
				if (nCancelType == spcltDependOnValue)
				{
					if (poSpell->GetAttr(satCancelLevel) == poCastingSpell->GetAttr(satCancelLevel))
					{
						if (poCastingSpell->GetAttr(satCancelHardTime))
						{
							if (!IsPhysicalSpellAttack(poCastingSpell->GetAttr(satAttackType)) &&
								IsPhysicalSpellAttack(poSpell->GetAttr(satAttackType)))
							{
								poCastingSpell->SetFastStop(TRUE);
								poRole->CacheSpell(pMsg->wSpellID, pMsg->bySerial, pMsg->dwTargetID, nTargetX, nTargetY, nTargetZ);
								return;
							}
						}
					}
				}

				PROCESS_ERROR_RET_CODE(poSpell->GetAttr(satNeedCache), errSpellCasting);

				// cache spell:
				PROCESS_ERROR_RET_CODE(poCastingSpell->GetCacheTimeFrm() > 0, errSpellCasting);
				poRole->CacheSpell(pMsg->wSpellID, pMsg->bySerial, pMsg->dwTargetID, nTargetX, nTargetY, nTargetZ);

				return;
			}
			//}
		}
	}

	poRole->SetClientUploadTargetID(pMsg->dwTargetID);

	poTarget = CGameWorld::GetInstance().FindValidCharacter(pMsg->dwTargetID);

	poRole->SetSpellSerial(pMsg->bySerial);

	nResult = poRole->CastSpell(poSpell, poTarget, nTargetX, nTargetY, nTargetZ);
	PROCESS_ERROR(nResult == errSuccess);

	poCastingSpell = poRole->GetCastingSpell();

	if (poCastingSpell != NULL)
	{
		poSpellSet->RecordNextSpell(poRole, poCastingSpell->GetAttr(satNextSpell), poCastingSpell->GetResetTimeFrm());
	}

	return;
Exit0:

	if (nResult == errUnknownError)
	{
		if (poSpellSet != NULL)
			poSpellSet->RecordNextSpell(poRole, 0, 0);

		KICK_ROLE(poRole, ktLogged);
	}
	else if (nResult == errSpellCastNoMove)
	{
		nRetCode = poRole->CacheSpell(pMsg->wSpellID, pMsg->bySerial, pMsg->dwTargetID, nTargetX, nTargetY, nTargetZ);
		LOG_CHECK_ERROR(nRetCode);
	}
	else
		poRole->ErrorCodeNotify( nResult, pMsg->wSpellID, pMsg->bySerial);
	return;
}

void CCliMessageHandler::OnDirectlyCastSpell(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	CSpell* poSpell = NULL;
	CCharacter* poTarget = NULL;
	C2G_DIRECTLY_CAST_SPELL* pMsg = (C2G_DIRECTLY_CAST_SPELL*)pData;

	poSpell = CSpellConfig::GetInstance().FindSpellTemplate(pMsg->dwSpellID, pMsg->bySpellLevel, 0);
	LOG_PROCESS_ERROR_RET_CODE(poSpell != NULL, errSpellNotExists);
	LOG_PROCESS_ERROR_RET_CODE(poSpell->GetAttr(satCastType) == spctDirectly, errUnknownError);

	poTarget = CGameWorld::GetInstance().FindValidCharacter(pMsg->dwTargetID);


	nResult = poRole->CastSpell(poSpell, poTarget, ConvertCoordN2S(pMsg->wTargetX), ConvertCoordN2S(pMsg->wTargetY), ConvertCoordN2S(pMsg->wTargetZ));
	PROCESS_ERROR(nResult == errSuccess);

	return;
Exit0:
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);
	return;
}

void CCliMessageHandler::OnCastSwitchSpell(CRole* poRole, const char* pData)
{
	INT32 nResult = errUnknownError;
	INT32 nRetCode = 0;
	CSpell* poSpell = NULL;
	CSpellSet* poSet = NULL;
	C2G_CAST_SWITCH_SPELL* pMsg = (C2G_CAST_SWITCH_SPELL*)pData;

	poSet = poRole->GetSpellBook().GetSpellSet(pMsg->wSpellID);
	LOG_PROCESS_ERROR_RET_CODE(poSet != NULL, errSpellNotExists);

	poSpell = poSet->GetSpell();
	LOG_PROCESS_ERROR_RET_CODE(poSpell != NULL, errSpellNotExists);
	LOG_PROCESS_ERROR_RET_CODE(poSpell->GetAttr(satCastType) == spctSwitch, errUnknownError);

	nResult = poRole->CastInstantSpell(poRole, poSpell, poRole, poRole->GetX(), poRole->GetY(), poRole->GetZ());
	LOG_PROCESS_ERROR_RET_CODE(nResult == errSuccess, nResult);

	return;
Exit0:
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify( nResult, pMsg->wSpellID, 0);
	return;
}

void CCliMessageHandler::OnCancelCast(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_ROLE_CANCEL_CAST* pMsg = (C2G_ROLE_CANCEL_CAST*)pData;

	CSpellInstance* spell = poRole->GetPendingSpell();

	if (spell != NULL && spell->GetID() == pMsg->wSpellID)
	{
		poRole->CancelCast();
	}

	return;
}

void CCliMessageHandler::OnSelectFocus(CRole* poRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_SELECT_FOCUS* pMsg = (C2G_SELECT_FOCUS*)pData;

	CCharacter* poFocusChar = poRole->SelectFocus(pMsg->dwFocusID);

	if (poFocusChar)
		DoSyncFocusInfo(poRole, poFocusChar);

	return;
Exit0:
	KICK_ROLE(poRole, ktLogged);

	return;
}

void CCliMessageHandler::OnAdjustTargetPt(CRole* poRole, const char* pData)
{
	C2G_ROLE_ADJUST_TARGET_POINT* pMsg = (C2G_ROLE_ADJUST_TARGET_POINT*)pData;

	poRole->AdjustTargetPt(ConvertCoordN2S(pMsg->wTargetX), ConvertCoordN2S(pMsg->wTargetY), ConvertCoordN2S(pMsg->wTargetZ), 0);

	return;
}

void CCliMessageHandler::OnChangePkMode(CRole* pRole, const char* pData)
{
	INT32 nRetCode = 0;
	C2G_CHANGE_PK_MODE* pMsg = (C2G_CHANGE_PK_MODE*)pData;

	nRetCode = pRole->ChangePkMode(pMsg->byMode);
	LOG_PROCESS_ERROR(nRetCode);

	return;
Exit0:
	KICK_ROLE(pRole, ktLogged);
	return;
}

void CCliMessageHandler::OnEquipSpell(CRole* poRole, const char* pData)
{
	INT32 nResult = 0;
	C2G_EQUIP_SPELL* pMsg = (C2G_EQUIP_SPELL*)pData;

	nResult = poRole->EquipSpell(pMsg->bySlot, pMsg->wSpellID);
	LOG_PROCESS_ERROR(nResult != errUnknownError);

	return;
Exit0:
	KICK_ROLE(poRole, ktLogged);
	return;
}

void CCliMessageHandler::OnCancelAura(CRole* poRole, const char* pData)
{
	INT32 nResult = 0;
	C2G_CANCEL_AURA* pMsg = (C2G_CANCEL_AURA*)pData;

	CAuraInstance* poAura = poRole->GetAura(pMsg->dwInstanceID);
	LOG_PROCESS_ERROR(poAura);

	PROCESS_ERROR_RET_CODE(poAura->GetAttr(aatCanCancel), errAuraCannotCancel);

	poRole->RemoveAura(poAura, TRUE);

	return;
Exit0:
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);
	return;

}

void CCliMessageHandler::OnSpellLog(CRole* poRole, const char* pData)
{
	INT32 nResult = 0;
	INT32 nErrCode = errSuccess;

	C2G_SPELL_LOG* pMsg = (C2G_SPELL_LOG*)pData;
	INT32 nTargetX = ConvertCoordN2S(pMsg->wTargetX);
	INT32 nTargetY = ConvertCoordN2S(pMsg->wTargetY);
	INT32 nTargetZ = ConvertCoordN2S(pMsg->wTargetZ);

	CSpellSet* poSet = poRole->GetSpellBook().GetSpellSet(pMsg->wSpellID);
	LOG_PROCESS_ERROR_RET_CODE(poSet, errSpellNotExists);

	if (pMsg->byNotEnoughMp)
		nErrCode = errSpellOutOfMana;
	else if (pMsg->byInCd)
		nErrCode = errSpellNotReady;

	//DoDLCastSpell(poRole, pMsg->wSpellID, poSet->GetLevel(), poSet->GetRuneSlot(), pMsg->dwTargetID, nTargetX, nTargetY, nTargetZ, pMsg->nCurrentMp, pMsg->nCdRemain, nErrCode, pMsg->dwTime, FALSE, pMsg->byIsShiftPressed);

	return;
Exit0:
	if (nResult == errUnknownError)
		KICK_ROLE(poRole, ktLogged);
	else
		poRole->ErrorCodeNotify(nResult, 0, 0);
	return;
}

void CCliMessageHandler::OnClearSpellCache(CRole* poRole, const char* pData)
{
	C2G_CLEAR_SPELL_CACHE* pMsg = (C2G_CLEAR_SPELL_CACHE*)pData;
	poRole->ClearSpellCache();

	DoClearSpellCache(poRole, pMsg->nTag);
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


BOOL CCliMessageHandler::DoLearnSpell(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel)
{
	INT32 nRetCode = 0;

	G2C_ROLE_LEARN_SPELL* pMsg = (G2C_ROLE_LEARN_SPELL*)m_poPacketAllocator->Alloc(sizeof(G2C_ROLE_LEARN_SPELL));
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_role_learn_spell;
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->wSpellID = nSpellID;
	pMsg->bySpellLevel = nSpellLevel;

	nRetCode = _Broadcast9Region(poRole->GetRegion(), (char*)pMsg, sizeof(G2C_ROLE_LEARN_SPELL));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoRemoveSpell(CRole* poRole, INT32 nSpellID)
{
	INT32 nRetCode = 0;

	G2C_ROLE_REMOVE_SPELL* pMsg = (G2C_ROLE_REMOVE_SPELL*)m_poPacketAllocator->Alloc(sizeof(G2C_ROLE_REMOVE_SPELL));
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_role_remove_spell;
	pMsg->wSpellID = nSpellID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ROLE_REMOVE_SPELL));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoLearnItemSpell(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel, INT32 nItemSlot)
{
	INT32 nRetCode = 0;

	G2C_ROLE_LEARN_ITEM_SPELL* pMsg = (G2C_ROLE_LEARN_ITEM_SPELL*)m_poPacketAllocator->Alloc(sizeof(G2C_ROLE_LEARN_ITEM_SPELL));
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_role_learn_item_spell;
	pMsg->wSpellID = nSpellID;
	pMsg->bySpellLevel = nSpellLevel;
	pMsg->byItemSlot = nItemSlot;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ROLE_LEARN_ITEM_SPELL));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoRemoveItemSpell(CRole* poRole, INT32 nSpellID, INT32 nItemSlot)
{
	INT32 nRetCode = 0;

	G2C_ROLE_REMOVE_ITEM_SPELL* pMsg = (G2C_ROLE_REMOVE_ITEM_SPELL*)m_poPacketAllocator->Alloc(sizeof(G2C_ROLE_REMOVE_ITEM_SPELL));
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_role_remove_item_spell;
	pMsg->wSpellID = nSpellID;
	pMsg->byItemSlot = nItemSlot;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ROLE_REMOVE_ITEM_SPELL));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncSpellBook(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_SPELL_BOOK* pMsg = NULL;
	CSpellSet* e = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_SPELL_BOOK*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_spell_book;
	pMsg->wSize = sizeof(G2C_SYNC_SPELL_BOOK);
	{
		INT32 i = 0;
		ISpellBook& oSpellBook = poRole->GetSpellBook();

		oSpellBook.BeginSpellSet();

		while ((e = oSpellBook.NextSpellSet()) != NULL)
		{
//			if (!e->IsEnabled())
//				continue;

			pMsg->astSpellData[i].wID = e->GetID();
			pMsg->astSpellData[i].bySpellLevel = e->GetActualLevel();
			pMsg->astSpellData[i].byRuneSlot = e->GetActualRuneSlot();
			pMsg->astSpellData[i].wCurPts = e->GetPts();
			i++;
			pMsg->wSize += sizeof(SPELL_DATA);
		}
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncSpellSlot(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_SPELL_SLOT* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_SPELL_SLOT*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_SPELL_SLOT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_spell_slot;

	for (INT32 i = qssSlotBegin; i < qssSlotCount; i++)
	{
		CSpellSlot* poSlot = poRole->GetSpellSlot(i);
		LOG_PROCESS_ERROR(poSlot);

		pMsg->astSlot[i].bySlotID = i;
		pMsg->astSlot[i].wSpellID = poSlot->GetMainSpellID();
		pMsg->astSlot[i].byLocked = poSlot->IsLocked();
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_SPELL_SLOT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncAuraList(CRole* poRole)
{
	INT32 nRetCode = 0;
	G2C_SYNC_AURA_LIST* pMsg = NULL;
	CAuraInstance* aura = NULL;
	INT32 nCount = 0;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_AURA_LIST*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_aura_list;
	pMsg->wSize = sizeof(G2C_SYNC_AURA_LIST);

	for (INT32 i = auetNone; i < auetTotal; i++)
	{
		aura = poRole->GetAuraList().BeginAura(i);

		while ((aura = poRole->GetAuraList().NextAura(i, aura)) != NULL)
		{
			pMsg->astAuraData[nCount].dwInstanceID = aura->GetInstanceID();
			pMsg->astAuraData[nCount].wID = aura->GetID();
			pMsg->astAuraData[nCount].byStackCount = aura->GetStackCount();
			pMsg->astAuraData[nCount].dwRemainDuration = aura->GetRemainFrm() * 1000 / GAME_FPS;
			pMsg->astAuraData[nCount].dwMaxDuration = aura->GetDurationFrm() * 1000 / GAME_FPS;
			nCount++;
			pMsg->wSize += sizeof(AURA_DATA);
		}
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}


BOOL CCliMessageHandler::DoCharPrepareSpell(CSceneObject* poFromObj, CSpellInstance* poSpell, UINT32 dwTargetID, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ, INT32 nWeaponType)
{
	INT32 nRetCode = 0;
	G2C_CHAR_PREPARE_SPELL* pMsg = NULL;

	LOG_PROCESS_ERROR(poFromObj);

	pMsg = (G2C_CHAR_PREPARE_SPELL*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_PREPARE_SPELL));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_prepare_spell;
	pMsg->dwCasterID = poFromObj->GetObjID();
	pMsg->wSpellID = (UINT16)poSpell->GetID();
	pMsg->bySpellLevel = (UINT8)poSpell->GetLevel();
	pMsg->byRuneSlot = (UINT8)poSpell->GetRuneSlot();
	pMsg->dwTargetID = dwTargetID;
	pMsg->wTargetX = ConvertCoordS2N(llTargetX);
	pMsg->wTargetY = ConvertCoordS2N(llTargetY);
	pMsg->wTargetZ = ConvertCoordS2N(llTargetZ);
	pMsg->bySerial = poSpell->GetSpellSerial();
	pMsg->wWeaponSpeedFactor = poSpell->GetWeaponSpeedFactor();
	pMsg->wAtkSpeedFactor = poSpell->GetAtkSpeedFactor();

	nRetCode = _Broadcast9Region(poFromObj->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_PREPARE_SPELL));
	LOG_PROCESS_ERROR(nRetCode);

//	INF("obj %d DoCharPrepareSpell %d", poFromObj->GetObjID(), poSpell->GetID());

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoCharExCastSpell(CSceneObject* poFromObj, CSpellInstance* poSpell, CCharacter* poCaster)
{
	INT32 nRetCode = 0;
	G2C_CHAR_EX_CAST_SPELL* pMsg = NULL;

	LOG_PROCESS_ERROR(poFromObj);
	LOG_PROCESS_ERROR(poSpell);
	LOG_PROCESS_ERROR(poCaster);

	PROCESS_ERROR(poSpell->GetBulletCount() > 0);

	pMsg = (G2C_CHAR_EX_CAST_SPELL*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_ex_cast_spell;
	pMsg->wSize = sizeof(G2C_CHAR_EX_CAST_SPELL);
	pMsg->SpellCastInfo.dwCasterID = poFromObj->GetObjID();
	pMsg->SpellCastInfo.wSpellID = (UINT16)poSpell->GetID();
	pMsg->SpellCastInfo.bySpellLevel = (UINT8)poSpell->GetLevel();
	pMsg->SpellCastInfo.byRuneSlot = (UINT8)poSpell->GetRuneSlot();
	pMsg->SpellCastInfo.bySerial = (UINT8)poSpell->GetSpellSerial();
	pMsg->SpellCastInfo.byCastID = poSpell->GetCastSerial();
	pMsg->byEffectSerial = poSpell->GetEffectSerial();
	pMsg->byTargetCount = 0;

	{
		CBullet* poBullet = NULL;

		poSpell->BeginBullet();

		while ((poBullet = poSpell->NextBullet()) != NULL)
		{
			CCharacter* poTarget = poBullet->GetTarget();
			if (poTarget == NULL)
				continue;

			pMsg->astTargetList[pMsg->byTargetCount].dwTargetID = poTarget->GetObjID();
			pMsg->astTargetList[pMsg->byTargetCount].wFlag = poBullet->GetEffectFlag().GetFlag();
			pMsg->astTargetList[pMsg->byTargetCount].wEffectDelayTime = poBullet->GetDelayFrame() * 1000 / GAME_FPS;
			pMsg->byTargetCount++;
			pMsg->wSize += sizeof(TARGET_DATA);
		}
	}

	nRetCode = _Broadcast9Region(poFromObj->GetRegion(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharCastSpell(CSceneObject* poFromObj, CSpellInstance* poSpell, CCharacter* poCaster)
{
	INT32 nRetCode = 0;
	G2C_CHAR_CAST_SPELL* pMsg = NULL;

	LOG_PROCESS_ERROR(poFromObj);
	LOG_PROCESS_ERROR(poSpell);
	LOG_PROCESS_ERROR(poCaster);

	pMsg = (G2C_CHAR_CAST_SPELL*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_cast_spell;
	pMsg->wSize = sizeof(G2C_CHAR_CAST_SPELL);
	pMsg->SpellCastInfo.dwCasterID = poFromObj->GetObjID();
	pMsg->SpellCastInfo.wSpellID = (UINT16)poSpell->GetID();
	pMsg->SpellCastInfo.bySpellLevel = (UINT8)poSpell->GetLevel();
	pMsg->SpellCastInfo.byRuneSlot = (UINT8)poSpell->GetRuneSlot();
	pMsg->SpellCastInfo.bySerial = (UINT8)poSpell->GetSpellSerial();
	pMsg->SpellCastInfo.byCastID = poSpell->GetCastSerial();
	pMsg->byTargetCount = 0;

	{
		CBullet* poBullet = NULL;

		poSpell->BeginBullet();

		while ((poBullet = poSpell->NextBullet()) != NULL)
		{
			CCharacter* poTarget = poBullet->GetTarget();
			if (poTarget == NULL)
				continue;

			pMsg->astTargetList[pMsg->byTargetCount].dwTargetID = poTarget->GetObjID();
			pMsg->astTargetList[pMsg->byTargetCount].wFlag = poBullet->GetEffectFlag().GetFlag();
			pMsg->astTargetList[pMsg->byTargetCount].wEffectDelayTime = poBullet->GetDelayFrame() * 1000 / GAME_FPS;
			pMsg->byTargetCount++;
			pMsg->wSize += sizeof(TARGET_DATA);
		}
	}

	nRetCode = _Broadcast9Region(poFromObj->GetRegion(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;

Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

//BOOL CCliMessageHandler::DoCharSpellPreEffect(CCharacter* poTarget, CBullet* poBullet)
//{
//	INT32 nRetCode = 0;
//	G2C_CHAR_SPELL_PRE_EFFECT* pMsg = NULL;
//
//	LOG_PROCESS_ERROR(poTarget);
//	LOG_PROCESS_ERROR(poBullet);
//
//	pMsg = (G2C_CHAR_SPELL_PRE_EFFECT*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_SPELL_PRE_EFFECT));
//	LOG_PROCESS_ERROR(pMsg);
//
//	pMsg->wMsg = g2c_char_spell_pre_effect;
//	//pMsg->dwCasterID = poFromObj->GetObjID();
//	pMsg->dwCasterID = poBullet->GetFromObjID();
//
//	pMsg->dwTargetID = poTarget->GetObjID();
//	pMsg->wSpellID = (UINT16)poBullet->GetSpellID();
//	pMsg->bySpellLevel= (UINT8)poBullet->GetSpellLevel();
//	pMsg->byRuneSlot = (UINT8)poBullet->GetSpellRuneSlot();
//	pMsg->bySerial = (UINT8)poBullet->GetSpellSerial();
//	pMsg->byCastID = poBullet->GetCastSerial();
//	pMsg->wEffectDelayTime = poBullet->GetDelayFrame() * 1000 / GAME_FPS;
//	pMsg->wFlag = poBullet->GetFlag().GetFlag();
//	pMsg->byEffectSerial = poBullet->GetEffectSerial();
//	pMsg->byIndex = poBullet->GetIndex();
//
//	nRetCode = _Broadcast9Region(poTarget->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_SPELL_PRE_EFFECT));
//	LOG_PROCESS_ERROR(nRetCode);
//
//	return TRUE;
//Exit0:
//	if (pMsg)
//		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
//	return FALSE;
//}


BOOL CCliMessageHandler::DoCharSpellEffect(CCharacter* poTarget, CBullet* poBullet)
{
	INT32 nRetCode = 0;
	G2C_CHAR_SPELL_EFFECT* pMsg = NULL;

	LOG_PROCESS_ERROR(poTarget);
	LOG_PROCESS_ERROR(poBullet);

	pMsg = (G2C_CHAR_SPELL_EFFECT*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_SPELL_EFFECT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_spell_effect;

	pMsg->SpellCastInfo.dwCasterID = poBullet->GetCasterID();
	pMsg->SpellCastInfo.wSpellID = (UINT16)poBullet->GetSpellID();
	pMsg->SpellCastInfo.bySpellLevel = (UINT8)poBullet->GetSpellLevel();
	pMsg->SpellCastInfo.byRuneSlot = (UINT8)poBullet->GetSpellRuneSlot();
	pMsg->SpellCastInfo.bySerial = (UINT8)poBullet->GetSpellSerial();
	pMsg->SpellCastInfo.byCastID = poBullet->GetCastSerial();
	pMsg->dwTargetID = poTarget->GetObjID();
	pMsg->byIsMissileEffect = poBullet->IsFromMissile();
	pMsg->nParam1 = poBullet->GetParam1();
	pMsg->shParam2 = (INT16)poBullet->GetParam2();
	pMsg->wStiffTime = (UINT16)poBullet->GetStiffFrm() * 1000 / GAME_FPS;
	pMsg->wFlag = poBullet->GetEffectFlag().GetFlag();
	pMsg->byEffectSerial = poBullet->GetEffectSerial();

	nRetCode = _Broadcast9Region(poTarget->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_SPELL_EFFECT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharAuraEffect(CCharacter* poTarget, CAuraInstance* poAura)
{
	INT32 nRetCode = 0;
	G2C_AURA_EFFECT* pMsg = NULL;

	LOG_PROCESS_ERROR(poTarget);

	pMsg = (G2C_AURA_EFFECT*)m_poPacketAllocator->Alloc(sizeof(G2C_AURA_EFFECT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_aura_effect;
	pMsg->dwInstanceID = poAura->GetInstanceID();
	pMsg->nEffectValue = poAura->GetTotalEffectValue();
	pMsg->wAuraID = poAura->GetID();
	pMsg->dwCasterID = poAura->GetCasterID();
	pMsg->dwTargetID = poTarget->GetObjID();

	if (poAura->GetAttr(aatHiddenToClient))
	{
		PROCESS_ERROR(poTarget->GetObjType() == otRole);
		nRetCode = m_poNotifier->SendToUser(((CRole*)poTarget)->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_AURA_EFFECT));
	}
	else
		nRetCode = _Broadcast9Region(poTarget->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_AURA_EFFECT));

	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}


BOOL CCliMessageHandler::DoCharBreakCast(CCharacter* poCharacter, CSpellInstance* poSpell, BOOL bIsBreak)
{
	INT32 nRetCode = 0;
	G2C_CHAR_BREAK_CAST* pMsg = NULL;

	LOG_PROCESS_ERROR(poCharacter);

	pMsg = (G2C_CHAR_BREAK_CAST*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_BREAK_CAST));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_break_cast;
	pMsg->SpellCastInfo.dwCasterID = poCharacter->GetObjID();
	pMsg->SpellCastInfo.wSpellID = (UINT16)poSpell->GetID();
	pMsg->SpellCastInfo.bySpellLevel = (UINT8)poSpell->GetLevel();
	pMsg->SpellCastInfo.byRuneSlot = poSpell->GetRuneSlot();
	pMsg->SpellCastInfo.bySerial = poSpell->GetSpellSerial();
	pMsg->SpellCastInfo.byCastID = poSpell->GetCastSerial();
	pMsg->byIsBreak = bIsBreak;

	nRetCode = _Broadcast9Region(poCharacter->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_BREAK_CAST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCastOver(CRole* poRole, INT32 nSpellID, INT32 nSerialID)
{
	INT32 nRetCode = 0;
	G2C_ROLE_CAST_OVER* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_ROLE_CAST_OVER*)m_poPacketAllocator->Alloc(sizeof(G2C_ROLE_CAST_OVER));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_role_cast_over;
	pMsg->wSpellID = (UINT16)nSpellID;
	pMsg->bySerial = nSerialID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ROLE_CAST_OVER));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoEquipSpell(CRole* poRole, INT32 nSlotID, INT32 nSpellID, INT32 nRuneSlot, INT32 nSpellLevel)
{
	INT32 nRetCode = 0;
	G2C_EQUIP_SPELL* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_EQUIP_SPELL*)m_poPacketAllocator->Alloc(sizeof(G2C_EQUIP_SPELL));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_equip_spell;
	pMsg->bySlot = nSlotID;
	pMsg->wSpellID = nSpellID;
	pMsg->byRuneSlot = nRuneSlot;
	pMsg->bySpellLevel = nSpellLevel;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_EQUIP_SPELL));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}
BOOL CCliMessageHandler::DoEquipRune(CRole* poRole, INT32 nSpellID, INT32 nRuneSlot, INT32 nSpellLevel)
{
	INT32 nRetCode = 0;
	G2C_EQUIP_RUNE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_EQUIP_RUNE*)m_poPacketAllocator->Alloc(sizeof(G2C_EQUIP_RUNE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_equip_rune;
	pMsg->wSpellID = nSpellID;
	pMsg->byRuneSlot = nRuneSlot;
	pMsg->bySpellLevel = nSpellLevel;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_EQUIP_RUNE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}
BOOL CCliMessageHandler::DoUnlockSpellSlot(CRole* poRole, INT32 nSlotID)
{
	INT32 nRetCode = 0;
	G2C_UNLOCK_SPELL_SLOT* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_UNLOCK_SPELL_SLOT*)m_poPacketAllocator->Alloc(sizeof(G2C_UNLOCK_SPELL_SLOT));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_unlock_spell_slot;
	pMsg->bySlot = nSlotID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_UNLOCK_SPELL_SLOT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoCharSelectFocus(CCharacter* poCharacter, UINT32 dwFocusID)
{
	INT32 nRetCode = 0;
	G2C_CHAR_SELECT_FOCUS* pMsg = NULL;

	LOG_PROCESS_ERROR(poCharacter);

	pMsg = (G2C_CHAR_SELECT_FOCUS*)m_poPacketAllocator->Alloc(sizeof(G2C_CHAR_SELECT_FOCUS));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_char_select_focus;
	pMsg->dwCharID = poCharacter->GetObjID();
	pMsg->dwFocusID = dwFocusID;

	nRetCode = _Broadcast9Region(poCharacter->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CHAR_SELECT_FOCUS));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncFocusInfo(CRole* poRole, CCharacter* poFocusChar)
{
	INT32 nRetCode = 0;
	G2C_FOCUS_INFO* pMsg = NULL;
	CAuraInstance* aura = NULL;

	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poFocusChar);

	pMsg = (G2C_FOCUS_INFO*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_focus_info;
	pMsg->wSize = sizeof(G2C_FOCUS_INFO);
	pMsg->dwFocusID = poFocusChar->GetObjID();
	pMsg->nCurrentHp = poFocusChar->GetHp();
	pMsg->nCurrentMp = poFocusChar->GetMp();
	pMsg->nMaxHp = poFocusChar->GetAttrList().GetAttr(catMaxHp);
	pMsg->nMaxMp = poFocusChar->GetAttrList().GetAttr(catMaxMp);
	pMsg->byAuraCount = 0;

	for (INT32 i = auetNone; i < auetTotal; i++)
	{
		aura = poFocusChar->GetAuraList().BeginAura(i);

		while ((aura = poFocusChar->GetAuraList().NextAura(i, aura)) != NULL)
		{
			if (aura->GetAttr(aatHiddenToClient))
				continue;

			pMsg->astAuraData[pMsg->byAuraCount].dwInstanceID = aura->GetInstanceID();
			pMsg->astAuraData[pMsg->byAuraCount].wID = aura->GetID();
			pMsg->astAuraData[pMsg->byAuraCount].byStackCount = aura->GetStackCount();
			pMsg->astAuraData[pMsg->byAuraCount].dwRemainDuration = aura->GetRemainFrm() * 1000 / GAME_FPS;
			pMsg->astAuraData[pMsg->byAuraCount].dwMaxDuration = aura->GetDurationFrm() * 1000 / GAME_FPS;
			pMsg->byAuraCount++;
			pMsg->wSize += sizeof(AURA_DATA);
		}
	}

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoNewAura(CCharacter* poChar, CAuraInstance* poAura)
{
	INT32 nRetCode = 0;
	G2C_NEW_AURA* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_NEW_AURA*)m_poPacketAllocator->Alloc(sizeof(G2C_NEW_AURA));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_new_aura;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->dwInstanceID = poAura->GetInstanceID();
	pMsg->wAuraID = poAura->GetID();
	pMsg->byStackCount = poAura->GetStackCount();
	pMsg->dwDuration = poAura->GetAttr(aatDuration);
	pMsg->dwCasterID = poAura->GetCasterID();

	if (poAura->GetAttr(aatHiddenToClient))
	{
		PROCESS_ERROR(poChar->GetObjType() == otRole);
		nRetCode = m_poNotifier->SendToUser(((CRole*)poChar)->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_NEW_AURA));
	}
	else
		nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_NEW_AURA));

	PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDelAura(CCharacter* poChar, CAuraInstance* poAura)
{
	INT32 nRetCode = 0;
	G2C_DEL_AURA* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);
	LOG_PROCESS_ERROR(poAura);

	pMsg = (G2C_DEL_AURA*)m_poPacketAllocator->Alloc(sizeof(G2C_DEL_AURA));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_del_aura;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->dwInstanceID = poAura->GetInstanceID();

	if (poAura->GetAttr(aatHiddenToClient))
	{
		PROCESS_ERROR(poChar->GetObjType() == otRole);
		nRetCode = m_poNotifier->SendToUser(((CRole*)poChar)->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_DEL_AURA));
	}
	else
		nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_DEL_AURA));

	PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoAuraChange(CCharacter* poChar, CAuraInstance* poAura)
{
	INT32 nRetCode = 0;
	G2C_AURA_CHANGE* pMsg = NULL;

	LOG_PROCESS_ERROR(poChar);
	LOG_PROCESS_ERROR(poAura);

	pMsg = (G2C_AURA_CHANGE*)m_poPacketAllocator->Alloc(sizeof(G2C_AURA_CHANGE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_aura_change;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->AuraData.dwInstanceID = poAura->GetInstanceID();
	pMsg->AuraData.dwRemainDuration = poAura->GetRemainFrm() * 1000 / GAME_FPS;
	pMsg->AuraData.dwMaxDuration = poAura->GetDurationFrm() * 1000 / GAME_FPS;
	pMsg->AuraData.byStackCount = poAura->GetStackCount();

	if (poAura->GetAttr(aatHiddenToClient))
	{
		PROCESS_ERROR(poChar->GetObjType() == otRole);
		nRetCode = m_poNotifier->SendToUser(((CRole*)poChar)->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_AURA_CHANGE));
	}
	else
		nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_AURA_CHANGE));

	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoAddCooldown(CRole* poRole, UINT32 dwCooldownID, INT32 nCooldownFrm)
{
	INT32 nRetCode = 0;
	G2C_ADD_COOLDOWN* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_ADD_COOLDOWN*)m_poPacketAllocator->Alloc(sizeof(G2C_ADD_COOLDOWN));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_add_cooldown;
	pMsg->dwCooldownID = dwCooldownID;
	pMsg->dwCooldownValue = nCooldownFrm * 1000 / GAME_FPS;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ADD_COOLDOWN));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoChangePkMode(CRole* poRole)
{
	INT32 nRetCode = 0;

	INT32 nSize = sizeof(G2C_SYNC_PK_MODE);

	G2C_SYNC_PK_MODE* pMsg = (G2C_SYNC_PK_MODE*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_sync_pk_mode;
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->byPkMode = poRole->GetPkMode();

	nRetCode = _Broadcast9Region(poRole->GetRegion(), (char*)pMsg, nSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoEnterCombat(CCharacter* poChar)
{
	INT32 nRetCode = 0;
	G2C_ENTER_COMBAT* pMsg = NULL;
	INT32 nSize = sizeof(G2C_ENTER_COMBAT);

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_ENTER_COMBAT*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_enter_combat;
	pMsg->dwCharID = poChar->GetObjID();

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ENTER_COMBAT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoLeaveCombat(CCharacter* poChar)
{
	INT32 nRetCode = 0;
	G2C_LEAVE_COMBAT* pMsg = NULL;
	INT32 nSize = sizeof(G2C_LEAVE_COMBAT);

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_LEAVE_COMBAT*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_leave_combat;
	pMsg->dwCharID = poChar->GetObjID();

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_LEAVE_COMBAT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncFaction(CRole* poRole)
{
	INT32 nRetCode = 0;

	G2C_SYNC_FACTION* pMsg = (G2C_SYNC_FACTION*)m_poPacketAllocator->Alloc(sizeof(G2C_SYNC_FACTION));
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_sync_faction;
	pMsg->dwRoleID = poRole->GetObjID();
	pMsg->byFaction = poRole->GetFaction();

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (char*)pMsg, sizeof(G2C_SYNC_FACTION));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoAddToThreatList(CRole* poRole, UINT32 dwCharID)
{
	INT32 nRetCode = 0;

	INT32 nSize = sizeof(G2C_ADDTO_THREATLIST);

	G2C_ADDTO_THREATLIST* pMsg = (G2C_ADDTO_THREATLIST*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_addto_threatlist;
	pMsg->dwCharID = dwCharID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_ADDTO_THREATLIST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoDelFromThreatList(CRole* poRole, UINT32 dwCharID)
{
	INT32 nRetCode = 0;

	INT32 nSize = sizeof(G2C_DELFROM_THREATLIST);

	G2C_DELFROM_THREATLIST* pMsg = (G2C_DELFROM_THREATLIST*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_delfrom_threatlist;
	pMsg->dwCharID = dwCharID;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_DELFROM_THREATLIST));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}

BOOL CCliMessageHandler::DoSyncSpellPts(CRole* poRole, INT32 nSpellID, INT32 nPts)
{
	INT32 nRetCode = 0;

	INT32 nSize = sizeof(G2C_SYNC_SPELL_PTS);

	G2C_SYNC_SPELL_PTS* pMsg = (G2C_SYNC_SPELL_PTS*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_sync_spell_pts;
	pMsg->wSpellID = nSpellID;
	pMsg->wPts = nPts;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SYNC_SPELL_PTS));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}


BOOL CCliMessageHandler::DoSpellLevelUp(CRole* poRole, INT32 nSpellID, INT32 nSpellLevel)
{
	INT32 nRetCode = 0;

	INT32 nSize = sizeof(G2C_SPELL_LEVEL_UP);

	G2C_SPELL_LEVEL_UP* pMsg = (G2C_SPELL_LEVEL_UP*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_spell_level_up;
	pMsg->wSpellID = nSpellID;
	pMsg->byLevel = nSpellLevel;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_SPELL_LEVEL_UP));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoLeechSoul(CRole* poRole, UINT32 dwFromObjID, INT32 nSoulPts)
{
	INT32 nRetCode = 0;

	INT32 nSize = sizeof(G2C_LEECH_SOUL);

	G2C_LEECH_SOUL* pMsg = (G2C_LEECH_SOUL*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	LOG_PROCESS_ERROR(poRole);

	pMsg->wMsg = g2c_leech_soul;
	pMsg->dwFromObjID = dwFromObjID;
	pMsg->dwToObjID = poRole->GetObjID();
	pMsg->nSoulPts = nSoulPts;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_LEECH_SOUL));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoDirectlyHpChangeEffect(CCharacter* poChar, INT32 nHpChange)
{
	INT32 nRetCode = 0;
	G2C_DIRECTLY_HP_CHANGE_EFFECT* pMsg = NULL;
	INT32 nSize = sizeof(G2C_DIRECTLY_HP_CHANGE_EFFECT);

	LOG_PROCESS_ERROR(poChar);

	pMsg = (G2C_DIRECTLY_HP_CHANGE_EFFECT*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_directly_hp_change_effect;
	pMsg->dwCharID = poChar->GetObjID();
	pMsg->nHpChange = nHpChange;

	nRetCode = _Broadcast9Region(poChar->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_DIRECTLY_HP_CHANGE_EFFECT));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoSyncRoleSpellData(CRole* poRole, CRole* poTargetRole, UINT32 dwCRC)
{
	INT32 nRetCode = 0;
	G2C_SYNC_ROLE_SPELL_DATA* pMsg = NULL;
	UINT32 dwNewCRC = 0;
	CSpellSet* e = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_SYNC_ROLE_SPELL_DATA*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_sync_role_spell_data;
	pMsg->wSize = sizeof(G2C_SYNC_ROLE_SPELL_DATA);
	{
		INT32 i = 0;
		for (INT32 nSlot = qssActiveBegin; nSlot < qssActiveEnd; nSlot++)
		{
			CSpellSlot* poSlot = poTargetRole->GetSpellSlot(nSlot);
			if (poSlot == NULL)
			{
				LOG_CHECK_ERROR(FALSE);
				continue;
			}

			CSpellSet* poSet = poRole->GetSpellBook().GetSpellSet(poSlot->GetMainSpellID());///poSlot->GetMainSpell(poRole);
			if (poSet == NULL)
				continue;

			pMsg->SpellData[i].wID = poSet->GetID();
			pMsg->SpellData[i].bySpellLevel = poSet->GetActualLevel();
			pMsg->SpellData[i].byRuneSlot = poSet->GetActualRuneSlot();
			i++;
			pMsg->wSize += sizeof(SPELL_DATA);

		}
	}

	dwNewCRC = crc32(0, (const Bytef*)pMsg, pMsg->wSize);
	PROCESS_ERROR(dwNewCRC != dwCRC);

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoClearSpellCache(CRole* poRole, INT32 nTag)
{
	INT32 nRetCode = 0;
	G2C_CLEAR_SPELL_CACHE* pMsg = NULL;

	LOG_PROCESS_ERROR(poRole);

	pMsg = (G2C_CLEAR_SPELL_CACHE*)m_poPacketAllocator->Alloc(sizeof(G2C_CLEAR_SPELL_CACHE));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_clear_spell_cache;
	pMsg->nTag = nTag;

	nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, sizeof(G2C_CLEAR_SPELL_CACHE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

struct CTraverseMissile
{
	G2C_SYNC_MISSILE_LIST* SyncMissileList;
	INT32 nCount;

	BOOL operator()(CRegion* poRegion)
	{
		LOG_PROCESS_ERROR(poRegion);

		return poRegion->TraverseMissile(*this);
	Exit0:
		return FALSE;
	}

	BOOL operator()(CMissile* poMissile)
	{
		LOG_PROCESS_ERROR(poMissile);
		LOG_PROCESS_ERROR(nCount < MAX_SYNC_DOODAD_COUNT);
		{
			MISSILE_APPEAR_DATA& rAppearData = SyncMissileList->astAppearData[nCount];

			rAppearData.dwMissileID = poMissile->GetObjID();
			rAppearData.dwOwnerID = poMissile->GetOwnerID();
			rAppearData.wTemplateID = poMissile->GetTemplateID();
			rAppearData.wCurrentX = ConvertCoordS2N(poMissile->GetX());
			rAppearData.wCurrentY = ConvertCoordS2N(poMissile->GetY());
			rAppearData.wCurrentZ = ConvertCoordS2N(poMissile->GetZ());
			rAppearData.wLifeFrm = poMissile->GetLifeTimeFrm();
			rAppearData.wDestX = ConvertCoordS2N(poMissile->GetDestX());
			rAppearData.wDestY = ConvertCoordS2N(poMissile->GetDestY());
			rAppearData.wDestZ = ConvertCoordS2N(poMissile->GetDestZ());
			rAppearData.wFaceDirection = poMissile->GetFaceDirection();

			nCount++;
		}

		return TRUE;
	Exit0:
		return FALSE;
	}
};


BOOL CCliMessageHandler::DoSyncMissileList(CRole* poRole, CRegion* poRegion)
{
    INT32 nRetCode = 0;
    G2C_SYNC_MISSILE_LIST* pMsg = NULL;
    CTraverseMissile TraverseMissile;

    LOG_PROCESS_ERROR(poRegion);
    LOG_PROCESS_ERROR(poRole);

    pMsg = (G2C_SYNC_MISSILE_LIST*)m_poPacketAllocator->Alloc(MAX_MESSAGE_SIZE);
    LOG_PROCESS_ERROR(pMsg);

    pMsg->wMsg = g2c_sync_missile_list;

    TraverseMissile.SyncMissileList = pMsg;
    TraverseMissile.nCount = 0;

    nRetCode = poRegion->Traverse9Region(TraverseMissile);
    LOG_CHECK_ERROR(nRetCode);

    if (TraverseMissile.nCount > 0)
    {
        pMsg->wSize = sizeof(G2C_SYNC_MISSILE_LIST) + sizeof(MISSILE_APPEAR_DATA) * TraverseMissile.nCount;
        nRetCode = m_poNotifier->SendToUser(poRole->GetSessionID(), (PACKET_BUFFER*)pMsg, pMsg->wSize);
        LOG_CHECK_ERROR(nRetCode);
    }
    else
    {
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    }

    return TRUE;
Exit0:
    if (pMsg)
        m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
    return FALSE;
}

BOOL CCliMessageHandler::DoMissileAppear(CMissile* poMissile, CRegion* poDestRegion)
{
	INT32 nRetCode = 0;
	G2C_MISSILE_APPEAR* pMsg = NULL;
    INT64 nDistance = 0LL;

	LOG_PROCESS_ERROR(poMissile);

	pMsg = (G2C_MISSILE_APPEAR*)m_poPacketAllocator->Alloc(sizeof(G2C_MISSILE_APPEAR));
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_missile_appear;
	// send char appear message
	pMsg->stAppearData.dwMissileID = poMissile->GetObjID();
	pMsg->stAppearData.wTemplateID = poMissile->GetTemplateID();
	pMsg->stAppearData.wCurrentX = ConvertCoordS2N(poMissile->GetX());
	pMsg->stAppearData.wCurrentY = ConvertCoordS2N(poMissile->GetY());
	pMsg->stAppearData.wCurrentZ = ConvertCoordS2N(poMissile->GetZ());
	pMsg->stAppearData.wLifeFrm = poMissile->GetLifeTimeFrm();
	pMsg->stAppearData.wDestX = ConvertCoordS2N(poMissile->GetDestX());
	pMsg->stAppearData.wDestY = ConvertCoordS2N(poMissile->GetDestY());
	pMsg->stAppearData.wDestZ = ConvertCoordS2N(poMissile->GetDestZ());
	pMsg->stAppearData.wFaceDirection = poMissile->GetFaceDirection();
	pMsg->stAppearData.dwOwnerID = poMissile->GetOwnerID();

#ifndef _ROBOT
	if (poDestRegion == NULL)
	{
		nRetCode = _Broadcast9Region(poMissile->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_MISSILE_APPEAR));
		LOG_PROCESS_ERROR(nRetCode);

	}
	else
	{
		nRetCode = _BroadcastRegion(poDestRegion, (PACKET_BUFFER*)pMsg, sizeof(G2C_MISSILE_APPEAR));
		LOG_PROCESS_ERROR(nRetCode);
	}
#endif

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;
}

BOOL CCliMessageHandler::DoMissileMove(CMissile* poMissile)
{
	INT32 nRetCode = 0;
	G2C_MISSILE_MOVE* pMsg = NULL;
	INT32 nSize = sizeof(G2C_MISSILE_MOVE);
	INT32 nGridDiff = 0;

	LOG_PROCESS_ERROR(poMissile);

	pMsg = (G2C_MISSILE_MOVE*)m_poPacketAllocator->Alloc(nSize);
	LOG_PROCESS_ERROR(pMsg);

	pMsg->wMsg = g2c_missile_move;
	pMsg->dwMissileID = poMissile->GetObjID();
//	pMsg->wRemainFrm2 = g_GetDistance2D(poMissile->GetX(), poMissile->GetY(), poMissile->GetDestX(), poMissile->GetDestY()) / poMissile->GetAttr(matMoveSpeed) * poMissile->GetAttr(matMoveSpeed) / (1000 * 1000 / GAME_FPS / GAME_FPS);
	pMsg->wDestX = ConvertCoordS2N(poMissile->GetDestX());
	pMsg->wDestY = ConvertCoordS2N(poMissile->GetDestY());
	pMsg->wDestZ = ConvertCoordS2N(poMissile->GetDestZ());
	pMsg->nFaceDirection = poMissile->GetFaceDirection();

	pMsg->wRemainFrm = g_Max(g_Abs(poMissile->GetDestX() - poMissile->GetX()), g_Abs(poMissile->GetDestY() - poMissile->GetY())) * poMissile->GetAttr(matMoveSpeed) / GRID_WIDTH;

	nRetCode = _Broadcast9Region(poMissile->GetRegion(), (PACKET_BUFFER*)pMsg, sizeof(G2C_MISSILE_MOVE));
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	if (pMsg)
		m_poPacketAllocator->Free((PACKET_BUFFER*)pMsg);
	return FALSE;

}



#endif  // _ROBOT
