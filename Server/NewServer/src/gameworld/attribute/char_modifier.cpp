#include "stdafx.h"
#include "char_modifier.h"

#include "error_def.h"
#include "ai_def.h"
#include "character.h"
#include "spell_instance.h"
#include "game_world.h"
#include "region.h"
#include "spell_config.h"
#include "item_bag.h"

#include "modifier_tree.h"
#include "scene_file_mgr.h"

#include "statistic.h"
#include "message_handler.h"
#include "npc_data.h"
#include "script_mgr.h"

#if defined(WIN32) | defined(WIN64)
#pragma warning(disable:4102)
#endif	// WIN32 | WIN64

INT32 ModifierChar_A(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    nParam1 = abs(nParam1);

    LOG_PROCESS_ERROR(nParam1 > catDefault && nParam1 < catTotal);

    if (nParam1 == catWalkSpeed || nParam1 == catRunSpeed)
        nParam2 = CONVERT_SPEED(nParam2);

    poChar->GetAttrList().AddAttrA(poChar, nParam1, nParam2);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_B(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    nParam1 = abs(nParam1);

    LOG_PROCESS_ERROR(nParam1 > catDefault && nParam1 < catTotal);

    poChar->GetAttrList().AddAttrB(poChar, nParam1, nParam2);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;

}

INT32 ModifierChar_003(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    INT32 nAddValue = 0;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    nParam1 = abs(nParam1);
    LOG_PROCESS_ERROR(nParam1 > catDefault && nParam1 < catTotal);

    if (!bIsRollingBack)
        nAddValue = poAura->GetEffectPct();
    else
        nAddValue = -poAura->GetEffectPct();

    poChar->GetAttrList().AddAttrB(poChar, nParam1, nAddValue);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_004(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    INT32 nAddValue = 0;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    nParam1 = abs(nParam1);
    LOG_PROCESS_ERROR(nParam1 > catDefault && nParam1 < catTotal);

    if (!bIsRollingBack)
        nAddValue = poAura->GetEffectValue();
    else
        nAddValue = -poAura->GetEffectValue();

    if (nParam1 == catWalkSpeed || nParam1 == catRunSpeed)
        nAddValue = CONVERT_SPEED(nAddValue);

    poChar->GetAttrList().AddAttrA(poChar, nParam1, nAddValue);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_005(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    CBullet* poBullet = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    poBullet = spell->GetScanner().DoSingleBullet(spell, poChar, target, poChar, betDamage, TRUE);
    PROCESS_SUCCESS(poBullet == NULL);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_006(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRegion* region = NULL;
    CCharacter* target = NULL;
    UINT32 dwTargetID = 0;
    CSpellInstance* spell = NULL;
    CSceneObject* poLastFromObj = NULL;
    INT32 nCount = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    region = poChar->GetRegion();
    LOG_PROCESS_ERROR(region);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nRetCode = spell->GetScanner().DoBullet(spell, poChar, betDamage);
    LOG_PROCESS_ERROR(nRetCode >= 0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_007(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CMissile* missile = NULL;
    CSpellInstance* spell = NULL;
    INT32 nMissileID = nParam1;
    INT32 nLockType = spltInvalid;
    INT32 nRetCode = 0;
    INT32 nDirection8 = 0;
    INT32 nDirection8V = 0;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nDirection8 = ConvertToDir8(spell->GetScanner().GetDirection());
    nDirection8V = NormalizeDirection8(nDirection8 - 4);

    {
        INT64 llInitX, llInitY, llInitZ = 0;
        INT32 nOffsetDirection = 0;

        const MISSILE_DATA* poMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nMissileID);
        LOG_PROCESS_ERROR(poMissileData);

        missile = CGameWorld::GetInstance().NewMissile();
        LOG_PROCESS_ERROR(missile);

        if (spell->GetAttr(satLockType) == spltDirection)
        {
            llInitX = poChar->GetX();
            llInitY = poChar->GetY();
            llInitZ = poChar->GetZ();
        }
        else
        {
            spell->GetScanner().GetTargetPt(llInitX, llInitY, llInitZ);
        }

        llInitZ = llInitZ + poMissileData->m_nAttr[matHeight];

        llInitX += nParam2 * g_nAdjacentX[nDirection8V] * GRID_WIDTH;
        llInitY += nParam2 * g_nAdjacentY[nDirection8V] * GRID_WIDTH;

        llInitX += nParam3 * g_nAdjacentX[nDirection8] * GRID_WIDTH;
        llInitY += nParam3 * g_nAdjacentY[nDirection8] * GRID_WIDTH;


        //DBG("Missile Create: (%.2f, %.2f, %.2f)", I64_2_Double(llInitX), I64_2_Double(llInitY), I64_2_Double(llInitZ));

        //		DBG("Missile Create, Char Dir: %d, Target Dir: %d, Missile Dir: %d", poChar->GetFaceDirection(), nTargetDirection, nDirection);

        //		SPL("Missile Direction : %d", nDirection);

        nRetCode = missile->Init(poMissileData, poChar, spell, llInitX, llInitY, llInitZ, spell->GetScanner().GetDirection(), 0);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_008(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nFaceDirection = CONVERT_ANGLE(nParam3);
    INT32 nDirection8 = 0;

    INT64 llDestX, llDestY = 0;
    INT32 nDirection = 0;
    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    if (nFaceDirection != 0)
    {
        nDirection = NormalizeDirection(poChar->GetFaceDirection() + nFaceDirection);
        DBG("rotate: %s : %d", __FILE__, __LINE__);
        poChar->RotateToDir8(nDirection, 0);
    }
    else
    {
        nDirection = spell->GetScanner().GetDirection();
        if (nDirection < 0)
            nDirection = poChar->GetFaceDirection();
        else
            DBG("rotate: %s : %d", __FILE__, __LINE__);
        poChar->RotateToDir8(nDirection, 0);
    }

    nDirection8 = ConvertToDir8(nDirection);

    llDestX = poChar->GetX() + g_nAdjacentX[nDirection8] * nParam1 * GRID_WIDTH;
    llDestY = poChar->GetY() + g_nAdjacentY[nDirection8] * nParam1 * GRID_WIDTH;

    return poChar->Dash(CONVERT_TIME(nParam2), llDestX, llDestY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_009(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* target = NULL;
    CAura* poAuraInfo = NULL;
    CSpellInstance* spell = NULL;
    CAuraInstance* poAuraInstance = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam1);
    LOG_PROCESS_ERROR(poAuraInfo);

    poAuraInstance = target->AppendAura(poAuraInfo, poChar, spell->GetFromObjID(), (ExtraParam*)pExParam, CONVERT_TIME(nParam3), 0, spell->GetID(), spell->GetLevel());
    PROCESS_SUCCESS(poAuraInstance == NULL);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    if (poAuraInfo)
        CRI("ModifierChar_009 AuraID: %d", nParam1);
    return mrcError;
}

INT32 ModifierChar_010(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    CAura* poAuraInfo = NULL;
    CAuraInstance* poAuraInstance = NULL;
    TARGET_NODE* pTargetNode = NULL;

    INT32 nCount = 0;

    INT32 nAuraID = abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nAuraID);
    LOG_PROCESS_ERROR(poAuraInfo);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->GetScanner().BeginTarget();

    while ((pTargetNode = spell->GetScanner().NextTarget()) != NULL)
    {
        target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
        if (target == NULL)
            continue;

        if (nCount >= spell->GetAttr(satMaxTargetCount))
            break;

        poAuraInstance = target->AppendAura(poAuraInfo, poChar, spell->GetFromObjID(), (ExtraParam*)pExParam, CONVERT_TIME(nParam3), 0, spell->GetID(), spell->GetLevel());

        nCount++;
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_011(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Stun TRUE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclStun));
        poChar->IncControlCount(cclStun);
        poChar->BreakCast(TRUE, TRUE);


        if (poChar->GetMoveState() >= CMS_ACTIVE_MOVE && poChar->GetMoveState() < CMS_PASSIVE_MOVE)
        {
            poChar->Stand(TRUE);
            //			poChar->Freeze(TRUE);
        }
    }
    else
    {
        //		DBG("---------------- Stun FALSE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclStun));
        poChar->DecControlCount(cclStun);

        //		if(poChar->GetControlCount(cclStun) <= 0)
        //		{
        //			poChar->Freeze(FALSE);
        //		}
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_012(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 eAttrType = 0;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    eAttrType = abs(nParam1);

    LOG_PROCESS_ERROR(eAttrType > catDefault && eAttrType < catTotal);

    if (!bIsRollingBack)
        poChar->GetAttrList().EnableAttr(eAttrType, TRUE);
    else
        poChar->GetAttrList().EnableAttr(eAttrType, FALSE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_013(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = NULL;
    CSpellInstance* poSpell = NULL;
    INT64 llTargetX = 0, llTargetY = 0, llTargetZ = 0;
    INT32 nNormalizedDirection = 0;

    PROCESS_ERROR(!bIsRollingBack);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poSpell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(poSpell);

    nNormalizedDirection = NormalizeDirection(poSpell->GetAttr(satOffsetDirection) + poSpell->GetScanner().GetDirection());

    llTargetX = poChar->GetX() + (INT64)poSpell->GetLengthAttr(satOffsetDistance) * g_Cos(nNormalizedDirection) / SIN_COS_NUMBER;
    llTargetY = poChar->GetY() + (INT64)poSpell->GetLengthAttr(satOffsetDistance) * g_Sin(nNormalizedDirection) / SIN_COS_NUMBER;
    llTargetZ = poChar->GetZ();

    //	{
    //		INT32 nDirection8 = ConvertToDir8(nNormalizedDirection);
    //		INT32 nDirection8V = NormalizeDirection8(nDirection8 - 4);
    //
    //		llTargetX += poSpell->GetAttr(satHOffsetGrids) * g_nAdjacentX[nDirection8V] * GRID_WIDTH;
    //		llTargetY += poSpell->GetAttr(satHOffsetGrids) * g_nAdjacentY[nDirection8V] * GRID_WIDTH;
    //
    //		llTargetX += poSpell->GetAttr(satVOffsetGrids) * g_nAdjacentX[nDirection8] * GRID_WIDTH;
    //		llTargetY += poSpell->GetAttr(satVOffsetGrids) * g_nAdjacentY[nDirection8] * GRID_WIDTH;
    //
    //
    //
    //	}

    GetOffsetedPos(llTargetX, llTargetY, nNormalizedDirection, poSpell->GetAttr(satHOffsetGrids), poSpell->GetAttr(satVOffsetGrids), llTargetX, llTargetY);

    poSpell->GetScanner().SetTargetPt(llTargetX, llTargetY, llTargetZ);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_014(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* aura = NULL;
    UINT64 qwEventKey = 0;
    INT32 nRetCode = FALSE;
    CCharacter* poChar = NULL;
    INT32 nOccasionID = abs(nParam1);
    INT32 nModifierNodeID = abs(nParam2);

    LOG_PROCESS_ERROR(nOccasionID > meoInvalid && nOccasionID < meoTotal);
    LOG_PROCESS_ERROR(nOccasionID != meoOnTimeup);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    aura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(aura);

    if (!bIsRollingBack)
    {
        // register event
        nRetCode = poChar->NewEventKey(nOccasionID, sesFromAura, aura->GetID(), qwEventKey);
        LOG_PROCESS_ERROR(nRetCode);

        aura->AddEventID(qwEventKey);

        nRetCode = poChar->RegisterEvent(qwEventKey, nModifierNodeID, aura->GetSpellID(), aura->GetSpellLevel(), aura->GetExtraParam());
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
        // unregister event
    {
        CAuraInstance::TEventIdList& list = aura->GetEventIdList();
        for (CAuraInstance::TEventIdList::iterator itr = list.begin(); itr != list.end(); ++itr)
        {
            nRetCode = poChar->RemoveEvent(*itr);
            LOG_CHECK_ERROR(nRetCode);
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_015(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    SPL("Break Modifier Tree.");
    return mrcTrueAndBreak;
}

INT32 ModifierChar_016(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    //	INT32 nSpeed = CONVERT_SPEED(nParam1);
    //	INT64 llDestX, llDestY, llDestZ = 0;
    //	INT64 llOffsetDist = CONVERT_LENGTH(nParam2);
    //	INT64 llMaxDist = CONVERT_LENGTH(nParam3);
    //	INT64 llDistSQ = 0;
    //
    //	INT32 nDirection = 0;
    //
    //	CSpellInstance* spell = NULL;
    //
    //	CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    //	LOG_PROCESS_ERROR(poChar);
    //
    ////	PROCESS_ERROR(!bIsRollingBack);
    //	if(bIsRollingBack)
    //		return mrcTrue;
    //
    //	spell = poChar->GetPendingSpell();
    //	LOG_PROCESS_ERROR(spell);
    //
    //	spell->GetScanner().GetTargetPt(llDestX, llDestY, llDestZ);
    //
    //	nDirection = g_GetDirection(poChar->GetX(), poChar->GetY(), llDestX, llDestY);
    //	PROCESS_SUCCESS(nDirection < 0);
    //
    //	llDestX = llDestX + llOffsetDist * g_Cos(nDirection) / SIN_COS_NUMBER;
    //	llDestY = llDestY + llOffsetDist * g_Sin(nDirection) / SIN_COS_NUMBER;
    //
    //	if(g_GetDistance2D(poChar->GetX(), poChar->GetY(), llDestX, llDestY) > llMaxDist * llMaxDist)
    //	{
    //		llDestX = poChar->GetX() + llMaxDist * g_Cos(nDirection) / SIN_COS_NUMBER;
    //		llDestY = poChar->GetY() + llMaxDist * g_Sin(nDirection) / SIN_COS_NUMBER;
    //	}
    //
    //	poChar->RotateTo(nDirection, 0);
    //
    //	//DBG("Leap From (%.2f, %.2f) To (%.2f, %.2f)", I64_2_Double(poChar->GetX()), I64_2_Double(poChar->GetY()), I64_2_Double(llDestX), I64_2_Double(llDestY));
    //
    //	return poChar->Leap(nSpeed, llDestX, llDestY);
    //
    //Exit1:
    //	return mrcFalse;
    //Exit0:
    return mrcError;

}

INT32 ModifierChar_017(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    UINT64 qwEventKey = 0;
    INT32 nRetCode = FALSE;

    INT32 eOccasionID = abs(nParam1);
    INT32 nModifierNodeID = abs(nParam2);

    CCharacter* poChar = NULL;
    CSpell* poSpell = NULL;

    LOG_PROCESS_ERROR(eOccasionID > meoInvalid && eOccasionID < meoTotal);
    LOG_PROCESS_ERROR(eOccasionID != meoOnTimeup);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poSpell = poChar->GetEquippingSpell();
    LOG_PROCESS_ERROR(poSpell);

    if (!bIsRollingBack)
    {
        // register event
        nRetCode = poChar->NewEventKey(eOccasionID, sesFromSpell, poSpell->GetID(), qwEventKey);
        LOG_PROCESS_ERROR(nRetCode);

        poChar->SaveSpellEvent(poSpell->GetID(), eOccasionID, qwEventKey);

        nRetCode = poChar->RegisterEvent(qwEventKey, nModifierNodeID, poSpell->GetID(), poSpell->GetLevel(), poSpell->GetExParam());
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        // unregister event
        nRetCode = poChar->UnregisterSpellEvent(poSpell->GetID(), eOccasionID);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;

}

INT32 ModifierChar_018(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    CCharacter* poCaster = NULL;
    INT32 nDirection = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    //PROCESS_ERROR(!poChar->GetFlag().CheckFlag(cfImmuneControl));

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poCaster == NULL);

    nDirection = NormalizeDirection(g_GetDirection(poCaster->GetX(), poCaster->GetY(), poChar->GetX(), poChar->GetY()));
    PROCESS_SUCCESS(nDirection < 0);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Fear TRUE: %s, ref count : %d", poChar->GetName(), poChar->GetControlCount(cclFear));
        poChar->IncControlCount(cclFear);
        poChar->Fear(nDirection, poAura->GetDurationFrm());

    }
    else
    {
        //		DBG("---------------- Fear FALSE: %s, ref count : %d", poChar->GetName(), poChar->GetControlCount(cclFear));
        poChar->DecControlCount(cclFear);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_019(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nSpellID = 0, nSpellLevel = 0;
    CRole* poRole = NULL;
    CSpellSet* poSet = NULL;
    CSpellTemplate* t = NULL;
    Item* poItem = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);
    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    nSpellID = abs(nParam1);
    nSpellLevel = abs(nParam2);

    poItem = poRole->GetPendingItem();
    LOG_PROCESS_ERROR(poItem);

    if (!bIsRollingBack)
    {
        nRetCode = poRole->AddItemSpell(poItem, nSpellID, nSpellLevel);
        PROCESS_SUCCESS(!nRetCode);
    }
    else
    {
        nRetCode = poRole->DelItemSpell(poItem, nSpellID);
        PROCESS_SUCCESS(!nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_020(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nSpellID = 0, nSpellLevel = 0;

    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);
    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    nSpellID = abs(nParam1);
    nSpellLevel = abs(nParam2);

    if (!bIsRollingBack)
    {
        nRetCode = poRole->LearnSpell(nSpellID, nSpellLevel, FALSE);
        PROCESS_SUCCESS(nRetCode != errSuccess);
    }
    else
        poRole->RemoveSpell(nSpellID);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_021(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nRange = nParam1;
    INT32 nSpellID = nParam2;
    CTargetScanner scanner;
    CCharacter* target = NULL;
    CRegion* region = NULL;
    CAuraInstance* aura = NULL;
    TARGET_NODE* pTargetNode = NULL;
    CSpell* poSpell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    aura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(aura);

    nRetCode = scanner.Init(poChar, sprtCircle, nRange, 0, 0, poChar->GetFaceDirection(), INT_MAX, 1 << nParam3, 7, FALSE, FALSE, FALSE, 0);
    LOG_PROCESS_ERROR(nRetCode);

    scanner.SetCenterPt(poChar->GetX(), poChar->GetY(), poChar->GetZ());
    scanner.SetTargetPt(poChar->GetX(), poChar->GetY(), poChar->GetZ());

    region = poChar->GetRegion();
    LOG_PROCESS_ERROR(region);

    //region->Traverse9Region(scanner);
    scanner.Scan(region, poChar);

    PROCESS_SUCCESS(scanner.GetTargetCount() <= 0);

    poSpell = CSpellConfig::GetInstance().FindSpellTemplate(nSpellID, aura->GetSpellLevel(), 0);
    LOG_PROCESS_ERROR(poSpell);

    scanner.BeginTarget();

    while ((pTargetNode = scanner.NextTarget()) != NULL)
    {
        target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
        if (target == NULL)
            continue;

        if (!target->CanAttack(poChar))
            continue;

        poChar->CastInstantSpell(poChar, poSpell, target, 0, 0, 0);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_022(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nReduceCount = nParam1;
    CAuraInstance* aura = NULL;
    CCharacter* poChar = NULL;

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(nReduceCount > 0);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    aura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(aura);

    poChar->ReduceAuraStack(aura, nReduceCount);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_023(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 eSpellAttr = nParam1;
    INT32 nValue = nParam2;
    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(eSpellAttr > satInvalid && eSpellAttr < satTotal);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->SetAttr(eSpellAttr, nValue);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_024(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nAttrValue = nParam2;
    BOOL bRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    bRetCode = ModifierChar_A(poChar, bIsRollingBack, catDmgAbsorb, nAttrValue, nParam3, pExParam);
    LOG_PROCESS_ERROR(bRetCode >= 0);

    poChar->SetDamageAbsorb(nAttrValue);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_025(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* aura = NULL;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    aura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(aura);

    if (poChar->GetRemainAbsorb() <= 0)
        poChar->ReduceAuraStack(aura, aura->GetStackCount());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_026(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nInterval = CONVERT_TIME(abs(nParam1));
    INT32 nModifierNodeID = abs(nParam2);
    UINT64 qwEventKey = 0;
    INT32 nRetCode = 0;

    CSpell* poSpell = NULL;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poSpell = poChar->GetEquippingSpell();
    LOG_PROCESS_ERROR(poSpell);

    nRetCode = CModifierTree::GetInstance().GetGlobalNodeType(nModifierNodeID);
    LOG_PROCESS_ERROR(nRetCode == mttCharacter);

    if (!bIsRollingBack)
    {
        // register event
        nRetCode = poChar->NewEventKey(meoOnTimeup, sesFromSpell, poSpell->GetID(), qwEventKey);
        LOG_PROCESS_ERROR(nRetCode);

        poChar->SaveSpellEvent(poSpell->GetID(), meoOnTimeup, qwEventKey);

        nRetCode = poChar->RegisterTimerEvent(qwEventKey, nInterval, nModifierNodeID, poSpell->GetID(), poSpell->GetLevel(), poSpell->GetExParam());
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        // unregister event
        nRetCode = poChar->UnregisterSpellEvent(poSpell->GetID(), meoOnTimeup);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_027(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nSpellID = nParam1;
    INT32 nSpellLevel = 1;
    INT32 nRetCode = 0;

    CSpellInstance* spell = NULL;
    CSpell* poSpell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    if (spell != 0)
        nSpellLevel = spell->GetLevel();

    poSpell = CSpellConfig::GetInstance().FindSpellTemplate(nSpellID, nSpellLevel, 0);
    LOG_PROCESS_ERROR(poSpell);

    nRetCode = poChar->CastInstantSpell(poChar, poSpell, NULL, poChar->GetX(), poChar->GetY(), poChar->GetZ());
    PROCESS_SUCCESS(nRetCode != errSuccess);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_028(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    UINT32 dwTargetID = 0;
    CCharacter* target = NULL;

    INT32 nDirection = 0;
    INT64 llDestX = 0, llDestY = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    dwTargetID = spell->GetScanner().GetLockedTarget();

    target = CGameWorld::GetInstance().FindValidCharacter(dwTargetID);
    PROCESS_SUCCESS(target == NULL);

    nDirection = g_GetDirection(poChar->GetX(), poChar->GetY(), target->GetX(), target->GetY());

    GetOffsetedPos(target->GetX(), target->GetY(), nDirection, nParam1, nParam2, llDestX, llDestY);

    DBG("rotate: %s : %d", __FILE__, __LINE__);
    poChar->RotateToDir8(nDirection, 0);

    return poChar->Dash(0, llDestX, llDestY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_029(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Invincible TRUE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclInvincible));
        poChar->IncControlCount(cclInvincible);
    }
    else
    {
        //		DBG("---------------- Invincible FALSE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclInvincible));
        poChar->DecControlCount(cclInvincible);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_030(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* target = NULL;

    INT64 llTargetX, llTargetY, llTargetZ = 0;
    CSpellInstance* spell = NULL;

    INT32 nDirection = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    llTargetX = target->GetX();
    llTargetY = target->GetY();

    nDirection = g_GetDirection(poChar->GetX(), poChar->GetY(), llTargetX, llTargetY);
    PROCESS_SUCCESS(nDirection < 0);

    DBG("rotate: %s : %d", __FILE__, __LINE__);
    poChar->RotateToDir8(nDirection, 0);

    GetOffsetedPos(target->GetX(), target->GetY(), nDirection, nParam2, nParam3, llTargetX, llTargetY);

    return poChar->Dash(CONVERT_TIME(nParam1), llTargetX, llTargetY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_031(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    CBullet* poBullet = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    poBullet = spell->GetScanner().DoSingleBullet(spell, poChar, target, poChar, betHeal, TRUE);
    LOG_PROCESS_ERROR(poBullet);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_032(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    UINT32 dwTargetID = 0;
    INT32 nCount = 0;

    CRegion* region = NULL;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    CSceneObject* poLastFromObj = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    region = poChar->GetRegion();
    LOG_PROCESS_ERROR(region);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nRetCode = spell->GetScanner().DoBullet(spell, poChar, betHeal);
    LOG_PROCESS_ERROR(nRetCode >= 0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_033(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    WRN("ModifierChar_033 has been abolished");

Exit0:
    return mrcError;
}

INT32 ModifierChar_034(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* poSpell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(poSpell);

    poSpell->GetScanner().SetDirection(poChar->GetFaceDirection());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_035(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* poSpell = NULL;
    INT32 nMaxHp = 0;
    INT32 nHpChange = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    nHpChange = nParam1 + (poChar->GetAttr(catMaxHp) * nParam2) / RATE_MAX;

    if (nParam3 > 0)
        nHpChange = nHpChange * (RATE_MAX + poChar->GetAttr(catDrugHpRecovery)) / RATE_MAX;

    poChar->DirectlyHpChange(nHpChange);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_036(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* poSpell = NULL;
    CItemBag* poBag = NULL;
    INT32 nItemCount = 0;

    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);
    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poBag = poRole->GetItemBag();
    LOG_PROCESS_ERROR(poBag);

    nItemCount = poBag->GetItemCount(nParam1);
    PROCESS_SUCCESS(nItemCount < nParam2);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_037(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* poSpell = NULL;
    CItemBag* poBag = NULL;
    UINT32 nFrm = CGameWorld::GetInstance().GetGameFrame();

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(poSpell);

    if (nFrm - poSpell->GetStartFrm() - poSpell->GetPrepareTimeMinFrm() > poSpell->GetAccumulateTimeMaxFrm())
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_038(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nFlag = abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
        poChar->GetFlag().SetFlag(nFlag);
    else
        poChar->GetFlag().ClearFlag(nFlag);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_039(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    if (g_Random(RATE_MAX) < nParam1)
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_040(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellSlot* poSlot = NULL;
    INT32 nSlotID = abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(nSlotID >= qssSlotBegin && nSlotID < qssSlotCount);

    poSlot = poChar->GetSpellSlot(nSlotID);
    PROCESS_ERROR(poSlot);

    poSlot->SetNextTargetID(0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_041(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellSlot* poSlot = NULL;
    INT32 nPct = abs(nParam1);
    INT32 nRealPct = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetAttr(catMaxMp) > 0);

    nRealPct = poChar->GetMp() * RATE_MAX / poChar->GetAttr(catMaxMp);

    if (nRealPct >= nPct)
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_042(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellSlot* poSlot = NULL;
    INT32 nPct = abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    if (poChar->GetMp() >= nPct)
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_043(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    INT32 nRetCode = 0;
    INT32 nRadius = CONVERT_LENGTH(nParam1);
    INT32 nDirection = 0;
    INT64 llX = 0, llY = 0, llZ = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    nRetCode = target->Revive(crtAtCurrentPos, RATE_MAX);
    PROCESS_SUCCESS(!nRetCode);

    nRadius = g_Random(nRadius);
    nDirection = g_Random(MAX_DIRECTION_COUNT);

    llX = poChar->GetX() + nRadius * g_Cos(nDirection) / SIN_COS_NUMBER;
    llY = poChar->GetY() + nRadius * g_Sin(nDirection) / SIN_COS_NUMBER;
    llZ = poChar->GetZ();

    return target->JumpTo(llX, llY, llZ, target->GetFaceDirection());
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_044(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CRole* target = NULL;
    CSpellInstance* spell = NULL;
    INT32 nRetCode = 0;
    CScene* poScene = NULL;
    CSceneStaticData* pStaticData = NULL;
    const SERVER_POINT* pServerPoint = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidRole(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    poScene = target->GetScene();
    LOG_PROCESS_ERROR(poScene);

    pStaticData = poScene->GetStaticData();
    LOG_PROCESS_ERROR(pStaticData);

    pServerPoint = pStaticData->GetNearestRevivePoint(target->GetFaction(), target->GetX(), target->GetY(), target->GetZ());
    if (pServerPoint)
    {
        return target->JumpTo(pServerPoint->nX, pServerPoint->nY, pServerPoint->nZ, target->GetFaceDirection());
    }

    if (pServerPoint == NULL || !nRetCode)
        return target->JumpToReturnPos();

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_045(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    INT32 nMoneyType = abs(nParam1);
    INT32 nMoney = nParam2;
    CItemBag* poBag = NULL;

    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);
    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poBag = poRole->GetItemBag();
    LOG_PROCESS_ERROR(poBag);

    PROCESS_SUCCESS(nMoney < 0 && poBag->GetMoney(nMoneyType) < abs(nMoney));

    nRetCode = poBag->AddMoney(nMoneyType, nMoney, MONEYLOG_CAST_SPELL, 0);
    PROCESS_SUCCESS(nRetCode != errSuccess);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_046(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = NULL;
    CCharacter* target = NULL;

    CSpellInstance* spell = NULL;
    INT32 nTargetCount = nParam1;
    INT32 nCount = 0;
    UINT32 dwTargetID = 0;

    if (bIsRollingBack)
        return mrcTrue;

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    return spell->GetScanner().GetTargetCount() >= nTargetCount;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_047(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* poChar = NULL;
    CCharacter* target = NULL;
    CRegion* poRegion = NULL;
    CSceneObject* poFromObj = NULL;

    CTargetScanner scanner;
    CSpellInstance* spell = NULL;
    TARGET_NODE* pTargetNode = NULL;

    INT32 nCount = 0;
    INT64 x = 0, y = 0;
    INT64 r = nParam1;
    INT32 angle = CONVERT_ANGLE(nParam2);
    INT64 offset = CONVERT_LENGTH(nParam3);

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_ERROR(nParam1 >= 0 && nParam2 >= 0 && nParam3 >= 0);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nRetCode = scanner.Init(poChar, sprtSector, r, 0, angle, poChar->GetFaceDirection(), spell->GetAttr(satTargetObjType), spell->GetAttr(satTargetRelationship), spell->GetAttr(satTargetStance), spell->GetAttr(satTargetDeadLimit), spell->GetAttr(satUseWeaponRange), spell->GetAttr(satIgnoreObstacle), spell->GetAttr(satTargetCheckNode));
    LOG_PROCESS_ERROR(nRetCode);

    x = poChar->GetX() + offset * g_Cos(poChar->GetFaceDirection()) / SIN_COS_NUMBER;
    y = poChar->GetY() + offset * g_Sin(poChar->GetFaceDirection()) / SIN_COS_NUMBER;

    scanner.SetCenterPt(x, y, poChar->GetZ());

    poFromObj = CGameWorld::GetInstance().FindValidObj(spell->GetFromObjID());
    if (poFromObj == NULL)
        poFromObj = poChar;

    poRegion = poFromObj->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    //poRegion->Traverse9Region(scanner);
    scanner.Scan(poRegion, poChar);

    scanner.BeginTarget();

    while ((pTargetNode = scanner.NextTarget()) != NULL)
    {
        target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
        if (target == NULL)
            continue;

        spell->GetScanner().LockTarget(target);

        return mrcTrue;
    }

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_048(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = NULL;
    CCharacter* target = NULL;
    CRegion* poRegion = NULL;

    CSpellInstance* spell = NULL;
    INT32 nCount = 0;
    TARGET_NODE* pTargetNode = NULL;

    PROCESS_ERROR(!bIsRollingBack);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poRegion = poChar->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    PROCESS_SUCCESS(spell->GetScanner().GetTargetCount() <= 0);

    spell->GetScanner().BeginTarget();

    pTargetNode = spell->GetScanner().NextTarget();
    PROCESS_SUCCESS(pTargetNode == NULL);

    target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
    PROCESS_SUCCESS(target == NULL);

    spell->GetScanner().LockTarget(target);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_049(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT64 llDestX, llDestY = 0;
    INT32 nDirection8 = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    nDirection8 = ConvertToDir8(NormalizeDirection(MAX_DIRECTION_COUNT / 2 + poChar->GetFaceDirection()));

    llDestX = poChar->GetX() + nParam1 * g_nAdjacentX[nDirection8] * GRID_WIDTH;
    llDestY = poChar->GetY() + nParam1 * g_nAdjacentY[nDirection8] * GRID_WIDTH;

    return poChar->KnockBack(CONVERT_TIME(nParam2), llDestX, llDestY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_050(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    INT32 nResCost = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    if (nParam1 == 0 && nParam2 == 0)
        nResCost = spell->GetConsumeAttr(satModifierCostValue) + (INT64)spell->GetConsumeAttr(satModifierCostValuePct) * poChar->GetAttr(catMaxMp) / RATE_MAX;
    else
        nResCost = nParam1 + (INT64)nParam2 * poChar->GetAttr(catMaxMp) / RATE_MAX;

    if (nResCost >= 0)
        PROCESS_SUCCESS(nResCost > poChar->GetMp());

    if (nParam3 > 0)
        nResCost = nResCost * (RATE_MAX + poChar->GetAttr(catDrugMpRecovery)) / RATE_MAX;

    poChar->ChangeMp(-nResCost);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_051(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    INT32 nNpcTemplateID = nParam1;
    INT32 nHOffsetGrids = nParam2;
    INT64 nVOffsetGrids = nParam3;
    INT64 llTargetX, llTargetY, llTargetZ = 0;
    INT32 nRetCode = 0;
    CNpc* poNpc = NULL;
    CScene* poScene = NULL;
    CCharacter* poChar = NULL;
    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(nNpcTemplateID > 0);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    spell->GetScanner().GetTargetPt(llTargetX, llTargetY, llTargetZ);

    if (llTargetX + llTargetY + llTargetZ == 0)
    {
        llTargetX = poChar->GetX();
        llTargetY = poChar->GetY();
        llTargetZ = poChar->GetZ();
    }

    GetOffsetedPos(llTargetX, llTargetY, spell->GetScanner().GetDirection(), nHOffsetGrids, nVOffsetGrids + 1, llTargetX, llTargetY);
    llTargetZ += GRID_WIDTH;

    nRetCode = poScene->GetLowerGrid(llTargetX, llTargetY, poChar->GetZ() + GRID_WIDTH, pcZone, pcGrid);
    PROCESS_SUCCESS(!nRetCode);

    llTargetZ = pcGrid->uHighLayer * GRID_LAYER_HEIGHT;

    poNpc = poScene->SpawnNpc(nNpcTemplateID, llTargetX, llTargetY, llTargetZ, poChar->GetFaceDirection(), poChar->GetObjID(), NULL);
    PROCESS_SUCCESS(poNpc == NULL);

    {
        CAIProcessor* poAi = poNpc->GetAIProcessor();
        PROCESS_SUCCESS(poAi == NULL);

        //		if (poAi->NeedFollow())
        //			poAi->SetFollowParam(poChar->GetObjID(), llTargetX - poChar->GetX(), llTargetY - poChar->GetY());
    }

    poNpc->SetLevel(poChar->GetLevel());
    poNpc->SetHighestSpellLevel(spell->GetLevel());
    poNpc->SetIsSummoned(TRUE);

    if (poChar->GetObjType() != otNpc)
    {
        poNpc->GetAttrList().SetAttrA(poNpc, catMaxPhyAtk, poChar->GetAttr(catMaxPhyAtk) * spell->GetLevel());
        poNpc->GetAttrList().SetAttrA(poNpc, catPhyAtk, poChar->GetAttr(catPhyAtk) * spell->GetLevel());
        poNpc->GetAttrList().SetAttrA(poNpc, catMaxMagAtk, poChar->GetAttr(catMaxMagAtk) * spell->GetLevel());
        poNpc->GetAttrList().SetAttrA(poNpc, catMagAtk, poChar->GetAttr(catMagAtk) * spell->GetLevel());
        poNpc->GetAttrList().SetAttrA(poNpc, catPhyDef, poChar->GetAttr(catPhyDef));
        poNpc->GetAttrList().SetAttrA(poNpc, catMagDef, poChar->GetAttr(catMagDef));
        poNpc->GetAttrList().SetAttrA(poNpc, catMaxHp, poChar->GetAttr(catMaxHp) / 3);

        poNpc->SetHp(poNpc->GetAttr(catMaxHp));
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_052(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    INT32 nDetAngle = CONVERT_ANGLE(nParam1);
    INT32 nMaxAngle = CONVERT_ANGLE(nParam2);
    INT64 llDist = 0;
    INT64 llCenterX, llCenterY, llCenterZ = 0;
    INT64 llTargetX, llTargetY, llTargetZ = 0;
    INT32 nDirection = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR((nDetAngle < 0 && nMaxAngle <= nDetAngle) || (nDetAngle > 0 && nMaxAngle >= nDetAngle));

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    {
        CTargetScanner& scanner = spell->GetScanner();
        scanner.GetCenterPt(llCenterX, llCenterY, llCenterZ);
        scanner.GetTargetPt(llTargetX, llTargetY, llTargetZ);

        nDirection = scanner.GetDirection() + nDetAngle;

        INT32 nCosValue = g_Cos(nDirection);

        if (nCosValue != 0)
            llDist = (llTargetX - llCenterX) * SIN_COS_NUMBER / nCosValue;
        else
            llDist = (llTargetY - llCenterY) * SIN_COS_NUMBER / g_Sin(nDirection);

        llTargetX = llCenterX + llDist * g_Cos(nDirection) / SIN_COS_NUMBER;
        llTargetY = llCenterY + llDist * g_Sin(nDirection) / SIN_COS_NUMBER;

        scanner.SetDirection(nDirection);
        scanner.SetTargetPt(llTargetX, llTargetY, llTargetZ);

        PROCESS_SUCCESS((nDetAngle > 0 && nDirection > nMaxAngle) || (nDetAngle < 0 && nDirection < nMaxAngle));
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_053(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;
    INT64 llX, llY, llZ = 0;
    INT32 nRetCode = 0;

    CSpell* poSpell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    //	DBG("cast serial: %d, param1: %d", spell->GetCastSerial(), nParam1);

    PROCESS_SUCCESS(spell->GetCastSerial() != nParam1);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    spell->GetScanner().GetTargetPt(llX, llY, llZ);

    poSpell = CSpellConfig::GetInstance().FindSpellTemplate(nParam2, spell->GetLevel(), 0);
    LOG_PROCESS_ERROR(poSpell);

    nRetCode = poChar->CastInstantSpell(poChar, poSpell, target, llX, llY, llZ);
    PROCESS_SUCCESS(nRetCode != errSuccess);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_054(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nOmiga = CONVERT_ANGLE(nParam1) / GAME_FPS;
    INT32 nDurationFrm = CONVERT_TIME(nParam2);
    INT32 nDestDirection = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    nDestDirection = NormalizeDirection(poChar->GetFaceDirection() + nOmiga * nDurationFrm);

    DBG("rotate: %s : %d", __FILE__, __LINE__);
    poChar->RotateToDir8(nDestDirection, nDurationFrm);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_055(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* poSpell = NULL;
    CItemBag* poBag = NULL;
    UINT32 nFrm = CGameWorld::GetInstance().GetGameFrame();

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(poSpell);

    if ((nFrm - poSpell->GetStartFrm() - poSpell->GetPrepareTimeMinFrm()) / poSpell->GetAccumulateTimeMaxFrm() >= nParam1)
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_056(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    return poChar->GetHp() <= poChar->GetAttr(catMaxHp) * nParam1 / RATE_MAX;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_057(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    return poChar->GetHp() <= nParam1;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_058(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (bIsRollingBack)
        return mrcTrue;

    return poChar->GetExFlag().CheckFlag(nParam1);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_059(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nFlag = abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (bIsRollingBack)
    {
        poChar->GetExFlag().ClearFlag(nFlag);
    }
    else
        poChar->GetExFlag().SetFlag(nFlag);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_060(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    if (nParam1 > 0)
    {
        poAura = poChar->GetFirstAura(nParam1);
        PROCESS_SUCCESS(poAura == NULL);

        poChar->RemoveAura(poAura, TRUE);

    }

    if (nParam2 > 0)
    {
        nRetCode = poChar->RemoveAuraByType(nParam2);
        //LOG_CHECK_ERROR(nRetCode);
    }

    if (nParam3 > 0)
    {
        nRetCode = poChar->RemoveAuraBySubType(nParam3);
        //LOG_CHECK_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

// to be completed.
INT32 ModifierChar_061(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    CCharacter* poCaster = NULL;
    INT32 nSpellLevel = 0;
    CSpell* poSpell = NULL;
    INT32 nRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poCaster == NULL);

    poSpell = CSpellConfig::GetInstance().FindSpellTemplate(nParam1, poAura->GetSpellLevel(), 0);
    LOG_PROCESS_ERROR(poSpell);

    nRetCode = poCaster->CastInstantSpell(poCaster, poSpell, poChar, poChar->GetX(), poChar->GetY(), poChar->GetZ());
    PROCESS_SUCCESS(nRetCode != errSuccess);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_062(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poCaster = NULL;
    CItemBag* poBag = NULL;
    INT32 nWeaponType = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);
    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    if (bIsRollingBack)
        return mrcTrue;

    poBag = ((CRole*)poChar)->GetItemBag();

    nWeaponType = poBag->GetWeaponType();

    PROCESS_SUCCESS(nWeaponType != abs(nParam1));

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_063(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poAura->DoTimeupEffect(poChar, TRUE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_064(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;

    INT32 nDirection = 0;
    INT32 nDirection8 = 0;
    INT64 llDestX = 0, llDestY = 0;

    CCharacter* poCaster = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poCaster == NULL);

    if (nParam3 > 0)
        nDirection = poCaster->GetFaceDirection();
    else
        nDirection = g_GetDirection(poCaster->GetX(), poCaster->GetY(), poChar->GetX(), poChar->GetY());

    if (nDirection < 0)
        nDirection = g_Random(MAX_DIRECTION_COUNT);

    nDirection8 = ConvertToDir8(nDirection);

    llDestX = poChar->GetX() + nParam1 * g_nAdjacentX[nDirection8] * GRID_WIDTH;
    llDestY = poChar->GetY() + nParam1 * g_nAdjacentY[nDirection8] * GRID_WIDTH;

    return poChar->KnockBack(CONVERT_TIME(nParam2), llDestX, llDestY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_065(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CMissile* missile = NULL;
    CSpellInstance* spell = NULL;
    INT32 nMissileID = nParam1;
    INT32 nExOffsetDirection = CONVERT_ANGLE(nParam2);
    INT32 nExOffsetDistance = CONVERT_LENGTH(nParam3);
    INT32 nDirection = 0;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);


    nDirection = poChar->GetFaceDirection();

    {
        INT64 llInitX = 0, llInitY = 0, llInitZ = 0;

        const MISSILE_DATA* poMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nMissileID);
        LOG_PROCESS_ERROR(poMissileData);

        missile = CGameWorld::GetInstance().NewMissile();
        LOG_PROCESS_ERROR(missile);

        if (spell->GetAttr(satLockType) == spltDirection)
        {
            llInitX = poChar->GetX();
            llInitY = poChar->GetY();
            llInitZ = poChar->GetZ();

        }
        else if (spell->GetAttr(satLockType == spltPos))
        {
            spell->GetScanner().GetTargetPt(llInitX, llInitY, llInitZ);
        }

        llInitX = llInitX + (INT64)poMissileData->m_nAttr[matOffsetDistance] * g_Cos(nDirection) / SIN_COS_NUMBER;
        llInitY = llInitY + (INT64)poMissileData->m_nAttr[matOffsetDistance] * g_Sin(nDirection) / SIN_COS_NUMBER;
        llInitZ = llInitZ + poMissileData->m_nAttr[matHeight];

        llInitX = llInitX + (INT64)nExOffsetDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
        llInitY = llInitY + (INT64)nExOffsetDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

        nDirection = NormalizeDirection(nDirection + poMissileData->m_nAttr[matOffsetDirection]);
        nDirection = NormalizeDirection(nDirection + nExOffsetDirection);

        //		DBG("Divergent Missile Create, Char Dir: %d, Missile Dir: %d", poChar->GetFaceDirection(), nDirection);

        nRetCode = missile->Init(poMissileData, poChar, spell, llInitX, llInitY, llInitZ, nDirection, 0);
        LOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_066(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CSpellInstance* spell = NULL;
    INT32 nMaxDist = CONVERT_LENGTH(nParam1);
    INT32 nDirection = 0;
    INT64 llDist = 0;
    INT64 llX, llY, llZ = 0;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    llDist = g_Random(nMaxDist);
    nDirection = g_Random(MAX_DIRECTION_COUNT);

    spell->GetScanner().GetTargetPt(llX, llY, llZ);

    llX = llX + llDist * g_Cos(nDirection) / SIN_COS_NUMBER;
    llY = llY + llDist * g_Sin(nDirection) / SIN_COS_NUMBER;

    spell->GetScanner().SetTargetPt(llX, llY, llZ);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_067(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* aura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    //	for(INT32 i = auetNone; i < auetTotal; i++)
    //	{
    //		aura = poChar->GetAuraList().GetStaticAura(i);
    //		if(aura == NULL)
    //			continue;
    //
    //		if((aura->GetID() == nParam1 || aura->GetAttr(aatGroup) == nParam2) && aura->GetStackCount() >= nParam3)
    //			return mrcTrue;
    //	}

    for (INT32 i = auetNone; i < auetTotal; i++)
    {
        aura = poChar->GetAuraList().BeginAura(i);

        while ((aura = poChar->GetAuraList().NextAura(i, aura)) != NULL)
        {
            if ((aura->GetID() == nParam1 || aura->GetAttr(aatGroup) == nParam2) && aura->GetStackCount() >= nParam3)
                return mrcTrue;
        }
    }

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_068(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CRole* poRole = NULL;

    CSpellInstance* spell = NULL;
    CSpellSet* poSet = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);
    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    LOG_PROCESS_ERROR(poRole);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poRole->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poSet = poRole->GetSpellBook().GetSpellSet(spell->GetID());
    LOG_PROCESS_ERROR(poSet);


    //	poSlot = poRole->GetSpellSlot(poRole->GetLastSlot());
    //	LOG_PROCESS_ERROR(poSlot);

    poSet->RecordNextSpell(poChar, nParam1, CONVERT_TIME(nParam2));

    spell->SetAttr(satNextSpell, nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

// to be completed.
INT32 ModifierChar_069(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraTemplate* t = NULL;
    CAuraInstance* inst = NULL;
    CSpell* spell = NULL;
    UINT32 dwFromObjID = 0;
    INT32 nSpellID = 0;
    INT32 nSpellLevel = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    t = CSpellConfig::GetInstance().FindAuraTemplate(nParam1);
    LOG_PROCESS_ERROR(t);

    dwFromObjID = poChar->GetObjID();

    spell = poChar->GetEquippingSpell();
    if (spell == NULL)
    {
        CSpellInstance* poInst = poChar->GetPendingSpell();
        if (poInst != NULL)
        {
            dwFromObjID = poInst->GetFromObjID();
        }

        spell = poInst;
    }
    if (spell != NULL)
    {
        nSpellID = spell->GetID();
        nSpellLevel = spell->GetLevel();
    }

    inst = poChar->AppendAura(t, poChar, dwFromObjID, (ExtraParam*)pExParam, CONVERT_TIME(nParam3), 0, nSpellID, nSpellLevel);
    PROCESS_SUCCESS(inst == NULL);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_070(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CSpellInstance* spell = NULL;
    CRegion* poRegion = NULL;
    CSceneObject* poFromObj = NULL;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    //	poRegion = poChar->GetRegion();
    //	LOG_PROCESS_ERROR(poRegion);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poFromObj = CGameWorld::GetInstance().FindValidObj(spell->GetFromObjID());
    if (poFromObj == NULL)
        poFromObj = poChar;

    poRegion = poFromObj->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    spell->ClearBulletList();

    spell->GetScanner().ClearTarget();
    spell->GetScanner().Scan(poRegion, poChar);

    if (nParam1 > 0)
        spell->GetScanner().LockTarget(NULL);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_071(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    //PROCESS_ERROR(!poChar->GetFlag().CheckFlag(cfImmuneControl));

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Slowed TRUE: %s, ref count : %d", poChar->GetName(), poChar->GetControlCount(cclSlowed));
        poChar->IncControlCount(cclSlowed);
        poChar->BreakCast(TRUE, TRUE);
    }
    else
    {
        //		DBG("---------------- Slowed FALSE: %s, ref count : %d", poChar->GetName(), poChar->GetControlCount(cclSlowed));
        poChar->DecControlCount(cclSlowed);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_072(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* aura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    aura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(aura);

    if ((aura->GetID() == nParam1 || (aura->GetAttr(aatGroup) == nParam2) && nParam2 != 0) && aura->GetStackCount() >= nParam3)
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_073(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    //	CNpc* poNpc = NULL;
    //
    //	CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    //	LOG_PROCESS_ERROR(poChar);
    //
    //	PROCESS_ERROR(!bIsRollingBack);
    //
    //	PROCESS_SUCCESS(poChar->GetObjType() != otNpc);
    //
    //	poNpc = static_cast<CNpc*>(poChar);
    //
    //	PROCESS_SUCCESS(poNpc->GetQuality() >= nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_074(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    //	CSpellInstance* spell = NULL;
    //	INT32 nSpeed = CONVERT_SPEED(nParam1);
    //	INT64 llMaxDist = CONVERT_LENGTH(nParam2);
    //	INT32 nMissileID = nParam3;
    //	INT64 llDestX, llDestY, llDestZ = 0;
    //	INT32 nDirection = 0;
    //	INT32 nRetCode = 0;
    //	CMissile* missile = NULL;
    //	INT64 llDistance = 0;
    //
    //	CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    //	LOG_PROCESS_ERROR(poChar);
    //
    //	//PROCESS_ERROR(!bIsRollingBack);
    //	if(bIsRollingBack)
    //		return mrcTrue;
    //
    //	spell = poChar->GetPendingSpell();
    //	LOG_PROCESS_ERROR(spell);
    //
    //	spell->GetScanner().GetTargetPt(llDestX, llDestY, llDestZ);
    //
    //	nDirection = g_GetDirection(poChar->GetX(), poChar->GetY(), llDestX, llDestY);
    //	PROCESS_SUCCESS(nDirection < 0);
    //
    //	llDistance = g_GetDistance2D(poChar->GetX(), poChar->GetY(), llDestX, llDestY);
    //
    //	if(llDistance > llMaxDist * llMaxDist)
    //	{
    //		llDestX = poChar->GetX() + llMaxDist * g_Cos(nDirection) / SIN_COS_NUMBER;
    //		llDestY = poChar->GetY() + llMaxDist * g_Sin(nDirection) / SIN_COS_NUMBER;
    //		llDistance = llMaxDist * llMaxDist;
    //	}
    //
    //	//DBG("Leap From (%.2f, %.2f) To (%.2f, %.2f)", I64_2_Double(poChar->GetX()), I64_2_Double(poChar->GetY()), I64_2_Double(llDestX), I64_2_Double(llDestY));
    //
    //	poChar->RotateTo(nDirection, 0);
    //
    //	nRetCode = poChar->Leap(nSpeed, llDestX, llDestY);
    //	PROCESS_SUCCESS(!nRetCode);
    //
    //	{
    //		const MISSILE_DATA* pMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nMissileID);
    //		LOG_PROCESS_ERROR(pMissileData);
    //		
    //		missile = CGameWorld::GetInstance().NewMissile();
    //		LOG_PROCESS_ERROR(missile);
    //
    //		llDistance = (INT64)sqrt((double)llDistance);
    //		missile->SetAttr(matRangeParam1, (INT32)llDistance);
    //
    //		nRetCode = missile->Init(pMissileData, poChar, spell, poChar->GetX(), poChar->GetY(), poChar->GetZ(), nDirection, 0);
    //		LOG_PROCESS_ERROR(nRetCode);
    //	}
    //
    //	return mrcTrue;
    //Exit1:
    //	return mrcFalse;
    //Exit0:
    return mrcError;
}

INT32 ModifierChar_075(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetFirstAura(nParam1);
    PROCESS_SUCCESS(poAura == NULL);

    nRetCode = poChar->DispelAura(poAura, nParam2);
    PROCESS_SUCCESS(!nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_076(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetFirstAuraByType(nParam1);
    PROCESS_SUCCESS(poAura == NULL);

    nRetCode = poChar->DispelAura(poAura, nParam2);
    PROCESS_SUCCESS(!nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_077(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetFirstAuraBySubType(nParam1);
    PROCESS_SUCCESS(poAura == NULL);

    nRetCode = poChar->DispelAura(poAura, nParam2);
    PROCESS_SUCCESS(!nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_078(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);


Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_079(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CSpellInstance* spell = NULL;
    CRegion* poRegion = NULL;
    UINT32 dwLockedTargetID = 0;
    CCharacter* target = NULL;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    dwLockedTargetID = spell->GetScanner().GetLockedTarget();

    PROCESS_SUCCESS(dwLockedTargetID == 0);

    target = CGameWorld::GetInstance().FindValidCharacter(dwLockedTargetID);
    PROCESS_SUCCESS(target == NULL);

    PROCESS_SUCCESS(target->IsDead());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_080(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    return poChar->BreakCast(TRUE, FALSE);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_081(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nSpeed = CONVERT_SPEED(nParam1);
    INT64 llOffsetDist = CONVERT_LENGTH(nParam2);
    INT64 llMaxDist = CONVERT_LENGTH(nParam3);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;
    TARGET_NODE* pTargetNode = NULL;

    INT32 nDirection = 0;
    INT64 llTargetX, llTargetY, llTargetZ = 0;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    pTargetNode = spell->GetScanner().NextTarget();
    PROCESS_SUCCESS(pTargetNode == NULL);

    target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
    PROCESS_SUCCESS(target == 0);

    nDirection = g_GetDirection(poChar->GetX(), poChar->GetY(), target->GetX(), target->GetY());
    PROCESS_SUCCESS(nDirection < 0);

    if (g_GetDistance3D(poChar->GetX(), poChar->GetY(), poChar->GetZ(), target->GetX(), target->GetY(), target->GetZ()) < llOffsetDist * llOffsetDist)
        return mrcTrue;

    llTargetX = target->GetX() + llOffsetDist * g_Cos(nDirection) / SIN_COS_NUMBER;
    llTargetY = target->GetY() + llOffsetDist * g_Sin(nDirection) / SIN_COS_NUMBER;

    if (g_GetDistance2D(poChar->GetX(), poChar->GetY(), llTargetX, llTargetY) > llMaxDist * llMaxDist)
    {
        llTargetX = poChar->GetX() + llMaxDist * g_Cos(nDirection) / SIN_COS_NUMBER;
        llTargetY = poChar->GetY() + llMaxDist * g_Sin(nDirection) / SIN_COS_NUMBER;
    }

    return poChar->Dash(nSpeed, llTargetX, llTargetY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_082(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->SetX(nParam1, nParam2);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_083(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    PROCESS_SUCCESS(nParam1 != spell->GetID());
    PROCESS_SUCCESS((nParam2 & (1 << spell->GetRuneSlot())) == 0);

    if (nParam3 != 0)
        PROCESS_SUCCESS(nParam3 != spell->GetLevel());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_084(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Freeze TRUE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclFreeze));
        poChar->IncControlCount(cclFreeze);
        poChar->Freeze(TRUE);
    }
    else
    {
        //		DBG("---------------- Freeze FALSE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclFreeze));
        poChar->DecControlCount(cclFreeze);

        if (poChar->GetControlCount(cclFreeze) <= 0)
        {
            poChar->Freeze(FALSE);
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_085(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CCharacter* poLockedChar = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poLockedChar = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(poLockedChar == NULL);

    if ((nParam1 & (1 << poLockedChar->GetObjType())) != 0 && (nParam2 & (1 << poLockedChar->GetSpecies())) != 0)
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_086(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        nRetCode = poChar->SetStance(nParam1);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        poChar->SetStance(csfStand);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_087(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nSpeed = CONVERT_SPEED(nParam1);
    INT64 llOffsetDist = CONVERT_LENGTH(nParam2);
    INT64 llMaxDist = CONVERT_LENGTH(nParam3);

    INT64 llTargetX, llTargetY, llTargetZ = 0;
    CAuraInstance* poAura = NULL;
    CCharacter* poCaster = NULL;

    INT32 nDirection = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poCaster == NULL);

    nDirection = g_GetDirection(poCaster->GetX(), poCaster->GetY(), poChar->GetX(), poChar->GetY());
    PROCESS_SUCCESS(nDirection < 0);

    if (g_GetDistance3D(poChar->GetX(), poChar->GetY(), poChar->GetZ(), poCaster->GetX(), poCaster->GetY(), poCaster->GetZ()) < llOffsetDist * llOffsetDist)
        return mrcTrue;

    if (llOffsetDist > 0)
    {
        llTargetX = poCaster->GetX() + llOffsetDist * g_Cos(nDirection) / SIN_COS_NUMBER;
        llTargetY = poCaster->GetY() + llOffsetDist * g_Sin(nDirection) / SIN_COS_NUMBER;

    }
    else
    {
        llTargetX = poCaster->GetX();
        llTargetY = poCaster->GetY();
    }

    if (g_GetDistance2D(poChar->GetX(), poChar->GetY(), llTargetX, llTargetY) > llMaxDist * llMaxDist)
    {
        nDirection = NormalizeDirection(nDirection + MAX_DIRECTION_COUNT / 2);

        llTargetX = poChar->GetX() + llMaxDist * g_Cos(nDirection) / SIN_COS_NUMBER;
        llTargetY = poChar->GetY() + llMaxDist * g_Sin(nDirection) / SIN_COS_NUMBER;
    }

    return poChar->Dash(nSpeed, llTargetX, llTargetY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_088(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);
#ifndef _ROBOT
    return CCliMessageHandler::GetInstance().DoSyncFlagPos(poChar);
#endif
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_089(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    BOOL bSameDirection = (nParam3 != 0);
    CSpellInstance* poSpell = NULL;
    CCharacter* target = NULL;

    INT64 llDestX, llDestY = 0;
    INT32 nDirection8 = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    //PROCESS_ERROR(!bIsRollingBack);
    if (bIsRollingBack)
        return mrcTrue;

    poSpell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(poSpell);

    target = CGameWorld::GetInstance().FindValidCharacter(poSpell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    nDirection8 = ConvertToDir8(bSameDirection ? poChar->GetFaceDirection() : g_GetDirection(poChar->GetX(), poChar->GetY(), target->GetX(), target->GetY()));
    PROCESS_SUCCESS(nDirection8 < 0);

    llDestX = poChar->GetX() + g_nAdjacentX[nDirection8] * nParam1 * GRID_WIDTH;
    llDestY = poChar->GetY() + g_nAdjacentY[nDirection8] * nParam1 * GRID_WIDTH;

    return target->Thrown(CONVERT_TIME(nParam2), llDestX, llDestY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_090(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = NULL;
    CCharacter* target = NULL;
    CRegion* poRegion = NULL;

    CSpellInstance* spell = NULL;
    TARGET_NODE* pTargetNode = NULL;

    if (bIsRollingBack)
        return mrcTrue;

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poRegion = poChar->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->GetScanner().BeginTarget();

    while ((pTargetNode = spell->GetScanner().NextTarget()) != NULL)
    {
        target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
        if (target == NULL)
            continue;

        if (!target->GetFlag().CheckFlag(cfCannotBeThrown))
        {
            spell->GetScanner().LockTarget(target);
            return mrcTrue;
        }
    }
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_091(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nOmiga = CONVERT_ANGLE(nParam1) / GAME_FPS;
    INT32 nMaxDirection = CONVERT_ANGLE(nParam2);
    INT32 nDuration = 0;

    CCharacter* poLockedTarget = NULL;
    CSpellInstance* spell = NULL;
    INT32 nDirection = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poLockedTarget = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(poLockedTarget == NULL);

    nDirection = NormalizeDirection(g_GetDirection(poChar->GetX(), poChar->GetY(), poLockedTarget->GetX(), poLockedTarget->GetY()));
    PROCESS_SUCCESS(nDirection < 0);

    nDuration = nOmiga > 0 ? nDirection / nOmiga : 0;

    //	DBG("----------modifier 91-----------");
    //	DBG("RotateTo: %d", nDirection);
    //	DBG("TargetPos: %d, %d", poLockedTarget->GetX() / GRID_WIDTH, poLockedTarget->GetY() / GRID_WIDTH);
    //	DBG("--------------------------------");

    return poChar->RotateTo(nDirection, nDuration);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_092(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nNpcTemplateID = nParam1;
    INT32 nOffsetDirection = CONVERT_ANGLE(nParam2);
    INT64 llOffsetDistance = CONVERT_LENGTH(nParam3);
    INT64 llTargetX, llTargetY, llTargetZ = 0;
    CNpc* poNpc = NULL;
    CScene* poScene = NULL;
    CCharacter* poChar = NULL;
    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;
    //	CCharacter* poCaster = NULL;
    //	CAuraInstance* poAura = NULL;

    PROCESS_ERROR(!bIsRollingBack);
    LOG_PROCESS_ERROR(nNpcTemplateID > 0);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    llTargetX = poChar->GetX() + llOffsetDistance * g_Cos(nOffsetDirection) / SIN_COS_NUMBER;
    llTargetY = poChar->GetY() + llOffsetDistance * g_Sin(nOffsetDirection) / SIN_COS_NUMBER;

    nRetCode = poScene->GetLowerGrid(llTargetX, llTargetY, poChar->GetZ() + GRID_WIDTH, pcZone, pcGrid);
    LOG_PROCESS_ERROR(nRetCode);

    llTargetZ = pcGrid->uHighLayer * GRID_LAYER_HEIGHT;

    GetOffsetedPos(llTargetX, llTargetY, poChar->GetFaceDirection(), 0, 1, llTargetX, llTargetY);

    poNpc = poScene->SpawnNpc(nNpcTemplateID, llTargetX, llTargetY, llTargetZ, poChar->GetFaceDirection(), poChar->GetObjID(), NULL);
    PROCESS_SUCCESS(poNpc == NULL);

    {
        CAIProcessor* poAi = poNpc->GetAIProcessor();
        PROCESS_SUCCESS(poAi == NULL);

        //		if (poAi->NeedFollow())
        //			poAi->SetFollowParam(poChar->GetObjID(), llTargetX - poChar->GetX(), llTargetY - poChar->GetY());
    }


    poNpc->SetIsSummoned(TRUE);

    if (poChar->GetObjType() != otNpc)
    {
        poNpc->GetAttrList().SetAttrA(poNpc, catMaxPhyAtk, poChar->GetAttr(catMaxPhyAtk));
        poNpc->GetAttrList().SetAttrA(poNpc, catPhyAtk, poChar->GetAttr(catPhyAtk));
        poNpc->GetAttrList().SetAttrA(poNpc, catMaxMagAtk, poChar->GetAttr(catMaxMagAtk));
        poNpc->GetAttrList().SetAttrA(poNpc, catMagAtk, poChar->GetAttr(catMagAtk));
        poNpc->GetAttrList().SetAttrA(poNpc, catPhyDef, poChar->GetAttr(catPhyDef));
        poNpc->GetAttrList().SetAttrA(poNpc, catMagDef, poChar->GetAttr(catMagDef));
        poNpc->GetAttrList().SetAttrA(poNpc, catMaxHp, poChar->GetAttr(catMaxHp) / 3);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_093(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->GetScanner().ResetScanner(spell);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_094(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CRole* poRole = NULL;
    CItemBag* poBag = NULL;
    Item* poItem = NULL;
    INT32 nWeaponTypeWanted = abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);
    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    PROCESS_ERROR(!bIsRollingBack);

    poRole = static_cast<CRole*>(poChar);

    poBag = poRole->GetItemBag();
    LOG_PROCESS_ERROR(poBag);

    poItem = poRole->GetPendingItem();

    if (poItem != NULL)
    {
        // equip item.
        LOG_PROCESS_ERROR(poItem->m_pTemplate->m_byType == ITEMTYPE_EQUIP);
        LOG_PROCESS_ERROR(poItem->m_pTemplate->m_bySubType == ITEMSUBTYPE_EQUIP_WEAPON);

        EQUIP_DATA* pData = (EQUIP_DATA*)(poItem->m_pTemplate);

        PROCESS_SUCCESS(pData->m_byEquipType != nWeaponTypeWanted);
    }
    else
    {
        // unequip item.
        PROCESS_SUCCESS(nWeaponTypeWanted != 0);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_095(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    if (poChar->GetObjType() == otNpc)
    {
        CNpc* poNpc = (CNpc*)poChar;
        poNpc->SetGenerateDrop(nParam1);
    }

    poChar->ChangeHp(-poChar->GetAttr(catMaxHp), cdrAura);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_096(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CNpc* poNpc = NULL;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (poChar->GetObjType() == otNpc)
        poNpc = static_cast<CNpc*>(poChar);
    else if (poChar->GetObjType() == otRole)
        poRole = static_cast<CRole*>(poChar);

    if (!bIsRollingBack)
    {
        if (poRole != NULL)
            //poChar->SetCanbeThrown(FALSE);
            poChar->GetFlag().SetFlag(cfCannotBeThrown);
        else if (poNpc != NULL && poNpc->GetNpcData()->bCanbeThrown)
            poChar->GetFlag().SetFlag(cfCannotBeThrown);
        //poChar->SetCanbeThrown(FALSE);
    }
    else
    {
        if (poRole != NULL)
            //poChar->SetCanbeThrown(TRUE);
            poChar->GetFlag().ClearFlag(cfCannotBeThrown);
        else if (poNpc != NULL && poNpc->GetNpcData()->bCanbeThrown)
            //poChar->SetCanbeThrown(TRUE);
            poChar->GetFlag().ClearFlag(cfCannotBeThrown);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_097(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT64 llDist = CONVERT_LENGTH(nParam1);
    INT32 nDirection = CONVERT_ANGLE(nParam2);
    INT64 llDestX, llDestY = 0;
    CScene* poScene = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    nDirection = NormalizeDirection(nDirection + poChar->GetFaceDirection());

    llDestX = poChar->GetX() + llDist * g_Cos(nDirection) / SIN_COS_NUMBER;
    llDestY = poChar->GetY() + llDist * g_Sin(nDirection) / SIN_COS_NUMBER;

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    if (poScene->IsObstacled(poChar->GetX(), poChar->GetY(), poChar->GetZ(), llDestX, llDestY, poScene->GetLowerZ(poChar->GetX(), poChar->GetY(), poChar->GetZ() + GRID_WIDTH), TRUE))
        return mrcFalse;

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_098(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* poChar = NULL;
    CCharacter* target = NULL;
    CRegion* poRegion = NULL;
    CSceneObject* poFromObj = NULL;

    CTargetScanner scanner;
    CSpellInstance* spell = NULL;
    TARGET_NODE* pTargetNode = NULL;

    INT32 nCount = 0;
    INT64 x = 0, y = 0;
    INT64 len = nParam1;
    INT32 wid = nParam2;

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_ERROR(nParam1 >= 0 && nParam2 >= 0);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nRetCode = scanner.Init(poChar, sprtRectangle, len, wid, 0, poChar->GetFaceDirection(), spell->GetAttr(satTargetObjType), spell->GetAttr(satTargetRelationship), spell->GetAttr(satTargetStance), spell->GetAttr(satTargetDeadLimit), spell->GetAttr(satUseWeaponRange), spell->GetAttr(satIgnoreObstacle), spell->GetAttr(satTargetCheckNode));
    LOG_PROCESS_ERROR(nRetCode);

    poFromObj = CGameWorld::GetInstance().FindValidObj(spell->GetFromObjID());
    if (poFromObj == NULL)
        poFromObj = poChar;

    poRegion = poFromObj->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    scanner.SetCenterPt(poChar->GetX(), poChar->GetY(), poChar->GetZ());
    scanner.SetDirection(poChar->GetFaceDirection());

    scanner.Scan(poRegion, poChar);

    scanner.BeginTarget();

    while ((pTargetNode = scanner.NextTarget()) != NULL)
    {
        target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
        if (target == NULL)
            continue;

        spell->GetScanner().LockTarget(target);

        return mrcTrue;
    }

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_099(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CSpellInstance* spell = NULL;
    CRegion* poRegion = NULL;
    CSceneObject* poFromObj = NULL;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poFromObj = CGameWorld::GetInstance().FindValidObj(spell->GetFromObjID());
    if (poFromObj == NULL)
        poFromObj = poChar;

    poRegion = poFromObj->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    spell->GetScanner().Scan(poRegion, poChar);
    spell->GetScanner().LockTarget(NULL);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_100(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nOffsetDirection = CONVERT_ANGLE(nParam2);
    INT64 llOffsetDistance = CONVERT_LENGTH(nParam3);
    INT64 llTargetX, llTargetY, llTargetZ = 0;
    CCharacter* poChar = NULL;
    CNpc* poNpc = NULL;
    CNpc* poSelfNpc = NULL;
    CScene* poScene = NULL;
    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;

    PROCESS_ERROR(!bIsRollingBack);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otNpc);
    poSelfNpc = static_cast<CNpc*>(poChar);

    poScene = poSelfNpc->GetScene();
    LOG_PROCESS_ERROR(poScene);

    llTargetX = poSelfNpc->GetX() + llOffsetDistance * g_Cos(nOffsetDirection) / SIN_COS_NUMBER;
    llTargetY = poSelfNpc->GetY() + llOffsetDistance * g_Sin(nOffsetDirection) / SIN_COS_NUMBER;

    nRetCode = poScene->GetLowerGrid(llTargetX, llTargetY, poSelfNpc->GetZ() + GRID_WIDTH, pcZone, pcGrid);
    LOG_PROCESS_ERROR(nRetCode);

    llTargetZ = pcGrid->uHighLayer * GRID_LAYER_HEIGHT;

    GetOffsetedPos(llTargetX, llTargetY, poChar->GetFaceDirection(), 0, 1, llTargetX, llTargetY);

    poNpc = poScene->SpawnNpc(poSelfNpc->GetNpcData()->nTemplateID, llTargetX, llTargetY, llTargetZ, poSelfNpc->GetFaceDirection(), poSelfNpc->GetObjID(), NULL);
    PROCESS_SUCCESS(poNpc == NULL);

    {
        CAIProcessor* poAi = poNpc->GetAIProcessor();
        PROCESS_SUCCESS(poAi == NULL);

        //		if (poAi->NeedFollow())
        //			poAi->SetFollowParam(poChar->GetObjID(), llTargetX - poChar->GetX(), llTargetY - poChar->GetY());
    }


    poNpc->SetLevel(poSelfNpc->GetLevel());
    poNpc->SetHp(poSelfNpc->GetHp());
    poNpc->SetGenerateDrop(FALSE);
    poNpc->SetIsSummoned(TRUE);

    poSelfNpc->GetAttrList().CopyTo(&poNpc->GetAttrList());

    if (nParam1 > 0)
        poSelfNpc->SetHp(0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_101(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Client Show Time TRUE : %s", poChar->GetName());
        poChar->ClientShowTime(TRUE);
    }
    else
    {
        //		DBG("---------------- Client Show Time FALSE : %s", poChar->GetName());
        poChar->ClientShowTime(FALSE);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_102(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT64 llOffsetDist = CONVERT_LENGTH(nParam1);
    INT32 nOffsetDirection = CONVERT_ANGLE(nParam2);
    INT64 llMaxDist = CONVERT_LENGTH(nParam3);
    INT64 llDestX, llDestY, llDestZ = 0;
    INT32 nDirection = 0;

    CAuraInstance* poAura = NULL;
    CCharacter* poAuraCaster = NULL;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poAuraCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poAuraCaster == NULL);

    nDirection = poAuraCaster->GetFaceDirection() + nOffsetDirection;

    llDestX = poAuraCaster->GetX() + llOffsetDist * g_Cos(nDirection) / SIN_COS_NUMBER;
    llDestY = poAuraCaster->GetY() + llOffsetDist * g_Sin(nDirection) / SIN_COS_NUMBER;
    llDestZ = poAuraCaster->GetZ() + GRID_WIDTH;

    return poChar->JumpTo(llDestX, llDestY, llDestZ, poChar->GetFaceDirection());
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_103(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    CCharacter* poAuraCaster = NULL;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poAuraCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poAuraCaster == NULL);

    poAuraCaster->BreakCast(TRUE, TRUE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_104(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;
    TARGET_NODE* pTargetNode = NULL;

    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poChar);

    if (bIsRollingBack)
        return mrcTrue;

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    pTargetNode = spell->GetScanner().LoopNextTarget();
    PROCESS_SUCCESS(pTargetNode == NULL);

    target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
    PROCESS_SUCCESS(target == NULL);

    spell->GetScanner().LockTarget(target);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_105(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT64 nDistance = CONVERT_LENGTH(nParam1);
    INT32 nSpeed = CONVERT_SPEED(nParam2);

    INT64 llDestX = 0, llDestY = 0;
    INT64 llTargetX = 0, llTargetY = 0, llTargetZ = 0;
    INT32 nDirection = 0;
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    nDirection = g_GetDirection(poChar->GetX(), poChar->GetY(), target->GetX(), target->GetY());
    PROCESS_SUCCESS(nDirection < 0);

    DBG("rotate: %s : %d", __FILE__, __LINE__);
    poChar->RotateToDir8(nDirection, 0);

    llDestX = poChar->GetX() + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
    llDestY = poChar->GetY() + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

    return poChar->Dash(nSpeed, llDestX, llDestY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_106(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT64 nDistance = CONVERT_LENGTH(nParam1);
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    PROCESS_SUCCESS(poChar->GetDistance3D(target) > nDistance * nDistance);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_107(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CMissile* missile = NULL;
    CSpellInstance* spell = NULL;
    INT32 nMissileID = nParam1;
    INT32 nDirection8 = 0;
    INT32 nDirection8V = 0;
    INT32 nLockType = spltInvalid;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    {
        INT64 llInitX = 0, llInitY = 0, llInitZ = 0;

        const MISSILE_DATA* poMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nMissileID);
        LOG_PROCESS_ERROR(poMissileData);

        missile = CGameWorld::GetInstance().NewMissile();
        LOG_PROCESS_ERROR(missile);

        llInitX = poChar->GetX();
        llInitY = poChar->GetY();

        //		llInitX += nParam2 * g_nAdjacentX[nDirection8V] * GRID_WIDTH;
        //		llInitY += nParam2 * g_nAdjacentY[nDirection8V] * GRID_WIDTH;
        //
        //		llInitX += nParam3 * g_nAdjacentX[nDirection8] * GRID_WIDTH;
        //		llInitY += nParam3 * g_nAdjacentY[nDirection8] * GRID_WIDTH;

        GetOffsetedPos(llInitX, llInitY, spell->GetScanner().GetDirection(), nParam2, nParam3, llInitX, llInitY);

        llInitZ = poChar->GetZ() + poMissileData->m_nAttr[matHeight];

        nRetCode = missile->Init(poMissileData, poChar, spell, llInitX, llInitY, llInitZ, spell->GetScanner().GetDirection(), 0);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_108(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CCharacter* poLockedTarget = NULL;
    INT64 llDist2 = 0;
    INT64 llDistCM2 = 0;
    INT64 llTmp = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poLockedTarget = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(poLockedTarget == NULL);

    llDist2 = g_GetDistance2D(poChar->GetX(), poChar->GetY(), poLockedTarget->GetX(), poLockedTarget->GetY());
    llDistCM2 = llDist2 / POINT_PER_CM / POINT_PER_CM;

    llTmp = llDistCM2 / 64 / 64;

    PROCESS_SUCCESS(llTmp > MAX_SQUARE);

    llTmp = g_Sqrt((UINT16)llTmp);

    spell->SetAttr(satRangeParam1, llTmp * 64);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_109(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CCharacter* poLockedTarget = NULL;
    INT32 A = 0;
    INT32 nAccumulatedFrm = 0;
    INT32 nResult = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    PROCESS_SUCCESS(spell->GetAccumulateTimeMaxFrm() <= 0);

    nAccumulatedFrm = CGameWorld::GetInstance().GetGameFrame() - spell->GetStartFrm();

    A = nAccumulatedFrm / spell->GetAccumulateTimeMaxFrm();

    A = A > spell->GetAttr(satAccumulateSeg) ? spell->GetAttr(satAccumulateSeg) : A;

    nResult = (A * nParam1 + RATE_MAX) * spell->GetAttr(satEffectPct) / RATE_MAX + A * nParam2;

    spell->SetAttr(satEffectPct, nResult);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_110(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    if (nParam1 > 0)
    {
        poAura = poChar->GetFirstAura(nParam1);
        PROCESS_SUCCESS(poAura == NULL);
    }

    if (nParam2 > 0)
    {
        poAura = poChar->GetFirstAuraByType(nParam2);
        PROCESS_SUCCESS(poAura == NULL);
    }

    if (nParam3 > 0)
    {
        poAura = poChar->GetFirstAuraBySubType(nParam3);
        PROCESS_SUCCESS(poAura == NULL);
    }

    PROCESS_SUCCESS(poAura == NULL);

    poChar->ReduceAuraStack(poAura, 1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_111(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    UINT64 qwEventKey = 0;
    INT32 nRetCode = FALSE;

    INT32 eOccasionID = abs(nParam1);
    INT32 nModifierNodeID = abs(nParam2);

    CCharacter* poChar = NULL;
    CSpell* poSpell = NULL;

    LOG_PROCESS_ERROR(eOccasionID > meoInvalid && eOccasionID < meoTotal);
    LOG_PROCESS_ERROR(eOccasionID != meoOnTimeup);

    poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poSpell = poChar->GetCastingSpell();
    LOG_PROCESS_ERROR(poSpell);

    if (!bIsRollingBack)
    {
        // register event
        nRetCode = poChar->NewEventKey(eOccasionID, sesFromSpell, poSpell->GetID(), qwEventKey);
        LOG_PROCESS_ERROR(nRetCode);

        poChar->SaveSpellEvent(poSpell->GetID(), eOccasionID, qwEventKey);

        nRetCode = poChar->RegisterEvent(qwEventKey, nModifierNodeID, poSpell->GetID(), poSpell->GetLevel(), poSpell->GetExParam());
        LOG_PROCESS_ERROR(nRetCode);

        //		DBG("register spell event: %d, rune: %d, roleid: %d, event key: 0x%llx", poSpell->GetID(), poSpell->GetRuneSlot(), poChar->GetObjID(), qwEventKey);
    }
    else
    {
        // unregister event

        nRetCode = poChar->UnregisterSpellEvent(poSpell->GetID(), eOccasionID);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_112(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT64 llX = 0, llY = 0, llZ = 0;
    INT32 eObjType = 0;

    const ZONE_DATA* pcZone = NULL;
    const GRID_DATA* pcGrid = NULL;
    GRID_INSTANCE_DATA* pGridInstance = NULL;
    CScene* poScene = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    llX = poChar->GetX();
    llY = poChar->GetY();
    llZ = poChar->GetZ() + GRID_WIDTH;

    nRetCode = poScene->GetLowerGrid(llX, llY, llZ, pcZone, pcGrid);
    LOG_PROCESS_ERROR(nRetCode);

    PROCESS_SUCCESS(pcGrid->bBlockMove);
    PROCESS_SUCCESS(pcGrid->bOutOfPoly);

    pGridInstance = poScene->GetGridInstance(pcZone, pcGrid);
    PROCESS_SUCCESS(pGridInstance == NULL);

    eObjType = poChar->GetObjType();

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_113(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    for (INT32 i = qssSlotBegin; i < qssSlotCount; i++)
    {
        CSpellSlot* poSlot = poChar->GetSpellSlot(i);
        if (poSlot == NULL)
            continue;

        CSpellSet* poSet = poChar->GetSpellBook().GetSpellSet(poSlot->GetMainSpellID());
        if (poSet == NULL)
            continue;

        CSpell* poSpell = CSpellConfig::GetInstance().FindSpellTemplate(poSet->GetID(), poSet->GetLevel(), poSet->GetRuneSlot());
        if (poSpell == NULL)
            continue;

        INT32 nCoolDownFrm = 0;

        poChar->ResetCooldown(MakeCoolDownID(COOLDOWN_SPELL, poSet->GetID()));

        {
            const SPELL_ENTRY* inf = poSpell->GetData();

            for (INT32 i = 0; i < inf->m_nCdGrpCount; i++)
            {
                poChar->ResetCooldown(MakeCoolDownID(COOLDOWN_GROUP, inf->m_CdGrp[i].m_nCdGrpID));
            }
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_114(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRegion* poRegion = NULL;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poRegion = poChar->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    PROCESS_ERROR(!bIsRollingBack);

    struct CTraverse9Region
    {
        INT64 llRadius;
        UINT32 dwSelfID;
        INT32 nHpPctLimit;
        INT64 llX, llY, llZ;
        BOOL bHasTarget;

        BOOL operator()(CCharacter* poChar)
        {
            if (g_GetDistance3D(llX, llY, llZ, poChar->GetX(), poChar->GetY(), poChar->GetZ()) <= llRadius * llRadius)
            {
                if (poChar->GetObjID() != dwSelfID && poChar->GetAttr(catMaxHp) > 0)
                {
                    if (poChar->GetHp() * RATE_MAX / poChar->GetAttr(catMaxHp) < nHpPctLimit)
                        bHasTarget = TRUE;
                }
            }
            return TRUE;
        }

        BOOL operator()(CRegion* poRegion)
        {
            INT32 nRetCode = 0;

            nRetCode = poRegion->TraverseCharacter(*this);
            LOG_PROCESS_ERROR(nRetCode);

            return TRUE;
        Exit0:
            return FALSE;
        }
    };

    CTraverse9Region TraverseChar;
    TraverseChar.llRadius = CONVERT_LENGTH(nParam1);
    TraverseChar.nHpPctLimit = nParam2;
    TraverseChar.llX = poChar->GetX();
    TraverseChar.llY = poChar->GetY();
    TraverseChar.llZ = poChar->GetZ();
    TraverseChar.dwSelfID = poChar->GetObjID();
    TraverseChar.bHasTarget = FALSE;

    nRetCode = poRegion->Traverse9Region(TraverseChar);
    LOG_PROCESS_ERROR(nRetCode);

    if (TraverseChar.bHasTarget)
        return mrcTrue;

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_115(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;
    CSpellInstance* poSpell = NULL;
    CCharacter* poLockedTarget = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(poSpell);

    poLockedTarget = CGameWorld::GetInstance().FindValidCharacter(poSpell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(poLockedTarget == NULL);

    if (nParam1 > 0)
    {
        poAura = poLockedTarget->GetFirstAura(nParam1);
        PROCESS_SUCCESS(poAura == NULL);

        poLockedTarget->RemoveAura(poAura, TRUE);

    }

    if (nParam2 > 0)
    {
        nRetCode = poLockedTarget->RemoveAuraByType(nParam2);
        //LOG_CHECK_ERROR(nRetCode);
    }

    if (nParam3 > 0)
    {
        nRetCode = poLockedTarget->RemoveAuraBySubType(nParam3);
        //LOG_CHECK_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_116(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CScene* poScene = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    PROCESS_SUCCESS(poScene->GetWeather() != nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_117(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poCaster = NULL;
    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poCaster == NULL);

    PROCESS_SUCCESS(poCaster->IsDead());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_118(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poChar->RemoveAura(poAura, TRUE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_119(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 eSpellAttr = nParam1;
    INT32 nValue = nParam2;
    INT32 nPct = nParam3;
    INT32 nAttrValue = 0;
    CSpellInstance* spell = NULL;
    INT32 nCurValue = 0;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(eSpellAttr > satInvalid && eSpellAttr < satTotal);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nAttrValue = spell->GetAttr(eSpellAttr) * (RATE_MAX + nParam3) / RATE_MAX + nValue;

    spell->SetAttr(eSpellAttr, nAttrValue);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_120(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nMaxHp = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    nMaxHp = poChar->GetAttr(catMaxHp);

    ModifierChar_003(poChar, bIsRollingBack, catMaxHp, 0, 0, pExParam);
    ModifierChar_004(poChar, bIsRollingBack, catMaxHp, 0, 0, pExParam);

    poChar->ChangeHp(poChar->GetAttr(catMaxHp) - nMaxHp, 0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_121(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CNpc* poNpc = NULL;
    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poNpc = CGameWorld::GetInstance().FindNpc(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(poNpc == NULL);

    PROCESS_SUCCESS(poNpc->GetQuality() <= nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_122(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CScene* poScene = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    return CGameWorld::GetInstance().IsDaytime();
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_123(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nValue = 0;
    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    LOG_PROCESS_ERROR(nParam1 >= 0 && nParam1 < MAX_EX_PARAM_COUNT);

    nValue = spell->GetExParam()->m_nX[nParam1] * (RATE_MAX + nParam3) / RATE_MAX + nParam2;

    spell->SetX(nParam1, nValue);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_124(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        poAura = poChar->GetPendingAura();
        LOG_PROCESS_ERROR(poAura);

        const TRASFORM_DATA* pData = CDataManager<TRASFORM_DATA>::GetInstance().GetData(poAura->GetID());
        LOG_PROCESS_ERROR(pData);

        poChar->Transform(TRUE);

        for (INT32 i = qssSlotBegin; i < qssSlotCount; i++)
        {
            if (pData->m_nEquipSpell[i] != 0)
            {
                CSpellSlot* poSlot = poChar->GetSpellSlot(i);

                if (poSlot != NULL)
                {
                    poSlot->Equip(poChar, pData->m_nEquipSpell[i]);
                }
            }
        }
    }
    else
    {
        poChar->Transform(FALSE);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

// to be completed.
INT32 ModifierChar_125(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CAuraInstance* poAura = NULL;
    CCharacter* poCaster = NULL;
    INT32 nSpellLevel = 0;
    CSpell* poSpell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poCaster == NULL);

    poSpell = CSpellConfig::GetInstance().FindSpellTemplate(nParam1, nSpellLevel, 0);
    LOG_PROCESS_ERROR(poSpell);

    nRetCode = poChar->CastInstantSpell(poChar, poSpell, poCaster, poCaster->GetX(), poCaster->GetY(), poCaster->GetZ());
    PROCESS_SUCCESS(nRetCode != errSuccess);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_126(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* poCaster = NULL;
    CAIProcessor* poAiProcessor = NULL;
    CAuraInstance* poAura = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    if (!bIsRollingBack)
    {
        if (poChar->GetObjType() == otRole)
        {
            nRetCode = poChar->SetAIProcessor(nParam1);
            LOG_PROCESS_ERROR(nRetCode);
        }

        //		DBG("---------------- Seduce TRUE: %s, ref count : %d", poChar->GetName(), poChar->GetControlCount(cclSeduced));
        poChar->IncControlCount(cclSeduced);
        poChar->SetSeducerID(poAura->GetCasterID());

        poAiProcessor = poChar->GetAIProcessor();
        LOG_PROCESS_ERROR(poAiProcessor);

        poAiProcessor->EnterIdle();

    }
    else
    {
        //		DBG("---------------- Seduce FALSE: %s, ref count : %d", poChar->GetName(), poChar->GetControlCount(cclSeduced));
        poChar->DecControlCount(cclSeduced);
        poChar->SetSeducerID(0);

        poAiProcessor = poChar->GetAIProcessor();
        LOG_PROCESS_ERROR(poAiProcessor);

        poChar->GetThreatList().Clear();

        poAiProcessor->EnterIdle();

        if (poChar->GetObjType() == otRole)
        {
            nRetCode = poChar->SetAIProcessor(poChar->GetOriginalAiID());
            LOG_PROCESS_ERROR(nRetCode);
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_127(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 eMoveState = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    eMoveState = poChar->GetMoveState();

    //	if(eMoveState != CMS_STAND && eMoveState != CMS_SIT && eMoveState != CMS_STALL && eMoveState != CMS_FREEZE && eMoveState != CMS_DIE)
    if (eMoveState == CMS_RUN || eMoveState == CMS_WALK || eMoveState == CMS_MOVE)
        return mrcTrue;

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_128(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    INT32 nInitValue = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    if (poAura->GetAttr(aatCategory) == acNegative)
        nInitValue = -1;
    else if (poAura->GetAttr(aatCategory) == acPositive)
        nInitValue = 1;

    poAura->SetTotalEffectValue(nInitValue * poAura->GetEffectPct() * poChar->GetAttr(catMaxHp) / RATE_MAX);

    poAura->DoTimeupEffect(poChar, FALSE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_129(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 eAuraAttr = nParam1;
    INT32 nValue = nParam2;
    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(eAuraAttr > aatInvalid && eAuraAttr < aatTotal);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poAura->SetAttr(eAuraAttr, nValue);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_130(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT64 nDistance = CONVERT_LENGTH(nParam1);
    INT32 nDuration = CONVERT_TIME(nParam2);
    BOOL bSameDirection = (nParam3 != 0);
    CSpellInstance* poSpell = NULL;
    CCharacter* target = NULL;

    INT64 llDestX, llDestY = 0;
    INT32 nDirection = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(poSpell);

    target = CGameWorld::GetInstance().FindValidCharacter(poSpell->GetScanner().GetLockedTarget());
    LOG_PROCESS_ERROR(target);

    nDirection = bSameDirection ? poChar->GetFaceDirection() : g_GetDirection(poChar->GetX(), poChar->GetY(), target->GetX(), target->GetY());
    PROCESS_ERROR(nDirection >= 0);

    llDestX = target->GetX() + nDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
    llDestY = target->GetY() + nDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

    return target->Thrown(nDuration, llDestX, llDestY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_131(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    //	INT64 nDistance = CONVERT_LENGTH(nParam1);
    //	INT32 nSpeed = CONVERT_SPEED(nParam2);
    //	INT32 nFaceDirection = CONVERT_ANGLE(nParam3);
    //
    //	INT64 llDestX, llDestY = 0;
    //	INT32 nDirection = 0;
    //	CSpellInstance* spell = NULL;
    //
    //	CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    //	LOG_PROCESS_ERROR(poChar);
    //
    //	//PROCESS_ERROR(!bIsRollingBack);
    //	if(bIsRollingBack)
    //		return mrcTrue;
    //
    //	spell = poChar->GetPendingSpell();
    //	LOG_PROCESS_ERROR(spell);
    //
    //	if(nFaceDirection != 0)
    //	{
    //		nDirection = NormalizeDirection(poChar->GetFaceDirection() + nFaceDirection);
    //		poChar->RotateTo(nDirection, 0);
    //	}
    //	else
    //	{
    //		nDirection = spell->GetScanner().GetDirection();
    //		if(nDirection < 0)
    //			nDirection = poChar->GetFaceDirection();
    //		else
    //			poChar->RotateTo(nDirection, 0);
    //	}
    //
    //	llDestX = poChar->GetX() + nDistance * g_Cos(nDirection)/SIN_COS_NUMBER;
    //	llDestY = poChar->GetY() + nDistance * g_Sin(nDirection)/SIN_COS_NUMBER;
    //
    //	//DBG("Charge From (%llu, %llu, %llu) To (%llu, %llu)", poChar->GetX(), poChar->GetY(), poChar->GetZ(), llDestX, llDestY);
    //	SPL("Leap Direction : %d", nDirection);
    //
    //	return poChar->Leap(nSpeed, llDestX, llDestY);
    //Exit1:
    //	return mrcFalse;
    //Exit0:
    return mrcError;
}

INT32 ModifierChar_132(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    //	INT32 nSpeed = CONVERT_SPEED(nParam1);
    //	INT64 llOffsetDist = CONVERT_LENGTH(nParam2);
    //	INT64 llMaxDist = CONVERT_LENGTH(nParam3);
    //	CCharacter* target = NULL;
    //
    //	INT64 llTargetX, llTargetY, llTargetZ = 0;
    //	CSpellInstance* spell = NULL;
    //
    //	INT32 nDirection = 0;
    //
    //	CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    //	LOG_PROCESS_ERROR(poChar);
    //
    //	//PROCESS_ERROR(!bIsRollingBack);
    //	if(bIsRollingBack)
    //		return mrcTrue;
    //
    //	spell = poChar->GetPendingSpell();
    //	LOG_PROCESS_ERROR(spell);
    //
    //	target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    //	PROCESS_SUCCESS(target == NULL);
    //
    //	//spell->GetScanner().GetTargetPt(llTargetX, llTargetY, llTargetZ);
    //
    //	//DBG("Charge From (%llu, %llu, %llu) To (%llu, %llu)", poChar->GetX(), poChar->GetY(), poChar->GetZ(), llTargetX, llTargetY);
    //
    //	llTargetX = target->GetX();
    //	llTargetY = target->GetY();
    //
    //	nDirection = g_GetDirection(poChar->GetX(), poChar->GetY(), llTargetX, llTargetY);
    //	PROCESS_SUCCESS(nDirection < 0);
    //
    //	if (g_GetDistance3D(poChar->GetX(), poChar->GetY(), poChar->GetZ(), llTargetX, llTargetY, llTargetZ) < llOffsetDist * llOffsetDist)
    //		return mrcTrue;
    //
    ////	if(nDirection != poChar->GetFaceDirection())
    ////		poChar->RotateTo(nDirection, 0);
    //
    //	llTargetX = llTargetX + llOffsetDist * g_Cos(nDirection) / SIN_COS_NUMBER;
    //	llTargetY = llTargetY + llOffsetDist * g_Sin(nDirection) / SIN_COS_NUMBER;
    //
    //	if(g_GetDistance2D(poChar->GetX(), poChar->GetY(), llTargetX, llTargetY) > llMaxDist * llMaxDist)
    //	{
    //		llTargetX = poChar->GetX() + llMaxDist * g_Cos(nDirection) / SIN_COS_NUMBER;
    //		llTargetY = poChar->GetY() + llMaxDist * g_Sin(nDirection) / SIN_COS_NUMBER;
    //	}
    //
    //	return poChar->Leap(nSpeed, llTargetX, llTargetY);
    //Exit1:
    //	return mrcFalse;
    //Exit0:
    return mrcError;
}

INT32 ModifierChar_133(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nSpeed = CONVERT_SPEED(nParam1);
    INT32 nOffsetDir = CONVERT_ANGLE(nParam2);
    INT64 llOffsetDist = CONVERT_LENGTH(nParam2);
    CAuraInstance* poAura = NULL;
    CSceneObject* poFromObj = NULL;

    INT32 nDirection = 0;
    INT32 llDestX = 0, llDestY = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poFromObj = CGameWorld::GetInstance().FindValidObj(poAura->GetFromObjID());
    PROCESS_SUCCESS(poFromObj == NULL);

    nDirection = g_GetDirection(poFromObj->GetX(), poFromObj->GetY(), poChar->GetX(), poChar->GetY());
    nDirection = NormalizeDirection(nDirection + nOffsetDir);

    llDestX = poFromObj->GetX() + g_Cos(nDirection) * llOffsetDist / SIN_COS_NUMBER;
    llDestY = poFromObj->GetY() + g_Sin(nDirection) * llOffsetDist / SIN_COS_NUMBER;

    return poChar->KnockBack(nSpeed, llDestX, llDestY);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_134(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    CCharacter* poCaster = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    if (!bIsRollingBack)
    {
        poChar->SetDamageShareCharID(poAura->GetCasterID());
    }
    else
    {
        poChar->SetDamageShareCharID(0);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_135(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    SpellEventNode* pNode = NULL;
    CSpell* poRootSpell = NULL;
    INT32 nCooldownFrm = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    pNode = poChar->GetPendingEventNode();
    LOG_PROCESS_ERROR(pNode);

    poRootSpell = poChar->GetSpellBook().GetMainSpell(pNode->nRootSpellID);
    LOG_PROCESS_ERROR(poRootSpell);

    nCooldownFrm = poRootSpell->GetEventCoolDownFrm();

    return poChar->AddCooldown(MakeCoolDownID(COOLDOWN_SPELL_EVENT_TRIGGER, poRootSpell->GetID()), nCooldownFrm);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_136(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CRI("ModifierChar_136 no longer supported.");

    return mrcError;
}

INT32 ModifierChar_137(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CRI("ModifierChar_137 no longer supported.");

    return mrcError;
}

INT32 ModifierChar_138(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->GetFlagBits().SetFlag(nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_139(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CAuraInstance* poAura = NULL;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    //	for(INT32 i = auetNone; i < auetTotal; i++)
    //	{
    //		poAura = poChar->GetAuraList().GetStaticAura(i);
    //		if(poAura == NULL)
    //			continue;
    //
    //		if(poAura->GetAttr(aatCategory) == acNegative)
    //		{
    //			poChar->RemoveAura(poAura, TRUE);
    //		}
    //	}

    for (INT32 i = auetNone; i < auetTotal; i++)
    {
        poAura = poChar->GetAuraList().BeginAura(i);

        while ((poAura = poChar->GetAuraList().NextAura(i, poAura)) != NULL)
        {
            if (poAura->GetAttr(aatCategory) == acNegative)
            {
                poChar->RemoveAura(poAura, TRUE);
            }
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_140(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CScene* poScene = NULL;
    INT32 nWeatherType = nParam1;
    INT32 nDuration = CONVERT_TIME(nParam2);
    INT32 nRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    return poScene->SetWeather(nParam1, nDuration);
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_141(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CScene* poScene = NULL;
    INT32 nSlot = g_Abs(nParam1);
    INT32 nSpellID = g_Abs(nParam2);
    INT32 nRuneSlot = g_Abs(nParam3);
    CSpellSlot* poSlot = NULL;
    SpellEventNode* poNode = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(nSlot >= qssSlotBegin && nSlot < qssSlotCount);

    poNode = poChar->GetPendingEventNode();
    LOG_PROCESS_ERROR(poNode);

    poSlot = poChar->GetSpellSlot(nSlot);
    LOG_PROCESS_ERROR(poSlot);

    //	poSlot->PushExtSpell(poChar, nSpellID, poNode->nRootSpellLevel, nRuneSlot);

    //	if(!bIsRollingBack)
    //	{
    //		poSlot = poChar->GetSpellSlot(nSlot);
    //		LOG_PROCESS_ERROR(poSlot);
    //
    //		poSlot->PushExtSpell(poChar, nSpellID, poNode->nRootSpellLevel, nRuneSlot);
    //	}
    //	else
    //	{
    //		poSlot = poChar->GetSpellSlot(nSlot);
    //		LOG_PROCESS_ERROR(poSlot);
    //
    //		poSlot->ClearExtSpell();
    //
    //	}

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_142(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poCaster = NULL;
    INT32 nSlotID = nParam1;
    INT32 nSpellID = nParam2;
    CSpellSlot* poSlot = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);
    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    PROCESS_ERROR(!bIsRollingBack);

    poSlot = poChar->GetSpellSlot(nSlotID);
    LOG_PROCESS_ERROR(poSlot);

    //	poSet = poSlot->GetMainSpell(poChar);
    //	PROCESS_SUCCESS(poSet == NULL);

    PROCESS_SUCCESS(poSlot->GetMainSpellID() != nSpellID);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_143(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    INT32 nAttr1 = abs(nParam1);
    INT32 nAttr2 = abs(nParam2);
    INT32 nValue = abs(nParam3);
    INT32 nRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(nAttr1 > catDefault && nAttr1 < catTotal);
    LOG_PROCESS_ERROR(nAttr2 > catDefault && nAttr2 < catTotal);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    if (!bIsRollingBack)
    {
        INT32 nAttrValue = (INT64)poChar->GetAttr(nAttr1) * nParam3 / RATE_MAX;

        nRetCode = poAura->SetExtraData(0, nAttrValue);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = ModifierChar_A(poChar, FALSE, nAttr2, nAttrValue, 0, NULL);
    }
    else
    {
        INT32 nAttrValue = 0;
        nRetCode = poAura->GetExtraData(0, nAttrValue);
        LOG_PROCESS_ERROR(nRetCode);

        poAura->ResetExtraData(0);

        nRetCode = ModifierChar_A(poChar, TRUE, nAttr2, -nAttrValue, 0, NULL);
    }

    return nRetCode;
Exit0:
    return mrcError;
}

INT32 ModifierChar_144(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    INT32 nAttr1 = abs(nParam1);
    INT32 nAttr2 = abs(nParam2);
    INT32 nValue = abs(nParam3);
    INT32 nRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(nAttr1 > catDefault && nAttr1 < catTotal);
    LOG_PROCESS_ERROR(nAttr2 > catDefault && nAttr2 < catTotal);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    if (!bIsRollingBack)
    {
        INT32 nAttrValue = (INT64)poChar->GetAttr(nAttr1) * nParam3 / RATE_MAX;

        nRetCode = poAura->SetExtraData(1, nAttrValue);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = ModifierChar_B(poChar, FALSE, nAttr2, nAttrValue, 0, NULL);
    }
    else
    {
        INT32 nAttrValue = 0;

        nRetCode = poAura->GetExtraData(1, nAttrValue);
        LOG_PROCESS_ERROR(nRetCode);

        poAura->ResetExtraData(1);

        nRetCode = ModifierChar_B(poChar, TRUE, nAttr2, -nAttrValue, 0, NULL);
    }

    return nRetCode;
Exit0:
    return mrcError;
}

INT32 ModifierChar_145(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    INT32 nAttr1 = abs(nParam1);
    INT32 nPctValue = nParam2;
    INT32 nValue = nParam3;
    INT32 nRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(nAttr1 > catDefault && nAttr1 < catTotal);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    if (!bIsRollingBack)
    {
        INT32 nTestValue = 0;
        INT32 nAttrValueA = poChar->GetMp() * nParam3;
        INT32 nAttrValueB = poChar->GetMp() * nParam2;

        nRetCode = poAura->GetExtraData(0, nTestValue);
        LOG_PROCESS_ERROR(!nRetCode);

        nRetCode = poAura->GetExtraData(1, nTestValue);
        LOG_PROCESS_ERROR(!nRetCode);

        poAura->SetExtraData(0, nAttrValueA);
        poAura->SetExtraData(1, nAttrValueB);

        nRetCode = ModifierChar_A(poChar, FALSE, nAttr1, nAttrValueA, 0, NULL);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = ModifierChar_B(poChar, FALSE, nAttr1, nAttrValueB, 0, NULL);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        INT32 nAttrValueA = 0;
        INT32 nAttrValueB = 0;

        nRetCode = poAura->GetExtraData(0, nAttrValueA);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = poAura->GetExtraData(1, nAttrValueB);
        LOG_PROCESS_ERROR(nRetCode);

        poAura->ResetExtraData(0);
        poAura->ResetExtraData(1);

        nRetCode = ModifierChar_A(poChar, TRUE, nAttr1, -nAttrValueA, 0, NULL);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = ModifierChar_B(poChar, TRUE, nAttr1, -nAttrValueB, 0, NULL);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return nRetCode;
Exit0:
    return mrcError;
}

INT32 ModifierChar_146(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nAttr1 = abs(nParam1);

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(nAttr1 > catDefault && nAttr1 < catTotal);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    if (!bIsRollingBack)
    {
        INT32 nAddValue = 0;
        INT32 nAttrValue = poChar->GetAttr(nAttr1);

        if (nParam3 - nAttrValue >= nParam2)
            nAddValue = nParam2;
        else if (nParam3 - nAttrValue >= 0)
            nAddValue = nParam3 - nAttrValue;

        poAura->SetExtraData(0, nAddValue);

        nRetCode = ModifierChar_A(poChar, FALSE, nAttr1, nAddValue, 0, NULL);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        INT32 nAddValue = 0;

        nRetCode = poAura->GetExtraData(0, nAddValue);
        LOG_PROCESS_ERROR(nRetCode);

        poAura->ResetExtraData(0);

        nRetCode = ModifierChar_A(poChar, TRUE, nAttr1, -nAddValue, 0, NULL);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return nRetCode;
Exit0:
    return mrcError;
}

INT32 ModifierChar_147(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    if (nParam1 > 0)
    {
        poAura = poChar->GetFirstAura(nParam1);
        PROCESS_SUCCESS(poAura != NULL);
    }

    if (nParam2 > 0)
    {
        poAura = poChar->GetFirstAuraByType(nParam2);
        PROCESS_SUCCESS(poAura != NULL);
    }

    if (nParam3 > 0)
    {
        poAura = poChar->GetFirstAuraBySubType(nParam3);
        PROCESS_SUCCESS(poAura != NULL);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_148(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nSignalType = nParam1;
    INT32 nLiveFrm = CONVERT_TIME(nParam2);
    INT32 nProcessTimes = nParam3;
    CAIProcessor* poAI = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(nSignalType > sigInvalid && nSignalType < sigEnd);

    LOG_PROCESS_ERROR(nLiveFrm > 0); //&& nLiveFrames < SIGNAL_MAX_LIVE_FRAME);
    if (nLiveFrm > SIGNAL_MAX_LIVE_FRAME)
        nLiveFrm = SIGNAL_MAX_LIVE_FRAME;

    LOG_PROCESS_ERROR(nProcessTimes > 0);// && nProcessTimes < SIGNAL_MAX_PROCESS_TIMES );
    if (nProcessTimes > SIGNAL_MAX_PROCESS_TIMES)
        nProcessTimes = SIGNAL_MAX_PROCESS_TIMES;

    poAI = poChar->GetAIProcessor();
    LOG_PROCESS_ERROR(poAI);

    nRetCode = poAI->IssueSignal(nSignalType, nLiveFrm, nProcessTimes, 0);
    PROCESS_SUCCESS(!nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_149(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CMissile* poMissile = NULL;
    CSpellInstance* poSpell = NULL;
    CCharacter* poLockedTarget = NULL;
    INT64 llX = 0, llY = 0;
    INT32 nOffsetDir = CONVERT_ANGLE(nParam2);
    INT64 llOffsetDist = CONVERT_LENGTH(nParam3);
    INT32 nFaceDir = 0;
    INT32 nRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetPendingSpell();
    PROCESS_ERROR(poSpell);

    poLockedTarget = CGameWorld::GetInstance().FindValidCharacter(poSpell->GetScanner().GetLockedTarget());
    PROCESS_ERROR(poLockedTarget);

    {
        const MISSILE_DATA* poMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nParam1);
        LOG_PROCESS_ERROR(poMissileData);

        poMissile = CGameWorld::GetInstance().NewMissile();
        LOG_PROCESS_ERROR(poMissile);

        llX = poChar->GetX() + llOffsetDist * g_Cos(nOffsetDir) / SIN_COS_NUMBER;
        llY = poChar->GetY() + llOffsetDist * g_Sin(nOffsetDir) / SIN_COS_NUMBER;

        nFaceDir = g_GetDirection(llX, llY, poLockedTarget->GetX(), poLockedTarget->GetY());

        nRetCode = poMissile->Init(poMissileData, poChar, poSpell, llX, llY, poChar->GetZ(), nFaceDir, 0);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_150(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CNpc* poNpc = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_SUCCESS(poChar->GetObjType() != otNpc);

    poNpc = static_cast<CNpc*>(poChar);

    PROCESS_SUCCESS(!poNpc->IsPet());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_151(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CDoodad* poDoodad = NULL;
    CScene* poScene = NULL;

    INT32 nDoodadID = nParam1;
    INT32 nOffsetDir = CONVERT_ANGLE(nParam2);
    INT64 llOffsetDist = CONVERT_LENGTH(nParam3);
    INT64 llX = 0, llY = 0, llZ = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    llX = poChar->GetX() + llOffsetDist * g_Cos(nOffsetDir) / SIN_COS_NUMBER;
    llY = poChar->GetY() + llOffsetDist * g_Sin(nOffsetDir) / SIN_COS_NUMBER;
    llZ = poChar->GetZ();

    poDoodad = poScene->SpawnDoodad(nDoodadID, llX, llY, llZ, poChar->GetFaceDirection());
    LOG_PROCESS_ERROR(poDoodad);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_152(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    BOOL bIsDot = nParam2;
    INT32 nAuraID = nParam1;
    INT32 nAddDuration = CONVERT_TIME(nParam3);
    INT32 nDurationFrm = 0;
    INT32 nIntervalFrm = 0;

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetFirstAura(nAuraID);
    PROCESS_SUCCESS(poAura == NULL);

    nDurationFrm = poAura->GetDurationFrm() + nAddDuration;

    if (bIsDot)
        nIntervalFrm = poAura->GetIntervalFrm() + nAddDuration;
    else
        nIntervalFrm = 0;

    poAura->ResetFrm(nDurationFrm, nIntervalFrm, TRUE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_153(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* poSpell = NULL;
    INT32 nLevel = nParam1;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetPendingSpell();
    PROCESS_ERROR(poSpell);

    return poSpell->GetLevel() >= nLevel;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_154(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* poSpell = NULL;
    CSpellSlot* poSlot = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetPendingSpell();
    PROCESS_ERROR(poSpell);

    poSlot = poChar->GetSpellSlot(nParam1);
    PROCESS_ERROR(poSlot);

    PROCESS_SUCCESS(poSpell->GetScanner().GetLockedTarget() != 0);

    PROCESS_SUCCESS(poSpell->GetScanner().GetLockedTarget() != poSlot->GetNextTargetID());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_155(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellSet* poSpell = NULL;
    CAuraInstance* poAura = NULL;
    INT32 nSpellLevel = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetSpellBook().GetSpellSet(poAura->GetSpellID());
    PROCESS_SUCCESS(poSpell == NULL);

    nSpellLevel = poSpell->GetLevel();

    PROCESS_SUCCESS(nSpellLevel <= poAura->GetSpellLevel());

    poAura->SetSpellLevel(nSpellLevel);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_156(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAura* poAuraInfo = NULL;
    CSpellInstance* spell = NULL;
    CAuraInstance* poAuraInstance = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam1);
    LOG_PROCESS_ERROR(poAuraInfo);

    {
        INT32 s = 0;
        INT32 a = 0;
        INT32 nDurationFrm = 0;

        if (poChar->GetAttr(catMagAtk) > 0)
        {
            if (poChar->GetAttr(catMaxMagAtk) > poChar->GetAttr(catMagAtk))
                s = poChar->GetAttr(catMagAtk) + g_Random(0, poChar->GetAttr(catMaxMagAtk) - poChar->GetAttr(catMagAtk) + 1);
            else
                s = poChar->GetAttr(catMagAtk);
        }

        nDurationFrm = GAME_FPS * (round_div(s * (spell->GetLevel() + 1), 4) + g_Random(spell->GetAttr(satDefaultValue), spell->GetAttr(satDefaultValueMax)) + nParam3);

        poAuraInstance = poChar->AppendAura(poAuraInfo, poChar, spell->GetFromObjID(), (ExtraParam*)pExParam, nDurationFrm, 0, spell->GetID(), spell->GetLevel());
        PROCESS_SUCCESS(poAuraInstance == NULL);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_157(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CMissile* missile = NULL;
    CSpellInstance* spell = NULL;
    INT32 nMissileID = nParam1;
    INT32 nDirection = CONVERT_ANGLE(nParam2);
    INT32 nLockType = spltInvalid;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    {
        INT64 llInitX, llInitY, llInitZ = 0;
        INT32 nNewLifeTime = 0;

        const MISSILE_DATA* poMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nMissileID);
        LOG_PROCESS_ERROR(poMissileData);

        missile = CGameWorld::GetInstance().NewMissile();
        LOG_PROCESS_ERROR(missile);

        if (spell->GetAttr(satLockType) == spltDirection)
        {
            llInitX = poChar->GetX();
            llInitY = poChar->GetY();
            llInitZ = poChar->GetZ();
        }
        else
        {
            spell->GetScanner().GetTargetPt(llInitX, llInitY, llInitZ);
            llInitX = llInitX + spell->GetLengthAttr(satOffsetDistance) * g_Cos(spell->GetAttr(satOffsetDirection) + poChar->GetFaceDirection()) / SIN_COS_NUMBER;
            llInitY = llInitY + spell->GetLengthAttr(satOffsetDistance) * g_Sin(spell->GetAttr(satOffsetDirection) + poChar->GetFaceDirection()) / SIN_COS_NUMBER;;
        }

        llInitZ = llInitZ + poMissileData->m_nAttr[matHeight];

        nNewLifeTime = GAME_FPS * (round_div(10 * (spell->GetLevel() + 1), 4) + g_Random(spell->GetAttr(satDefaultValue), spell->GetAttr(satDefaultValueMax)) + g_Random(poChar->GetAttr(catMagAtk), poChar->GetAttr(catMaxMagAtk)));

        nRetCode = missile->Init(poMissileData, poChar, spell, llInitX, llInitY, llInitZ, nDirection, nNewLifeTime);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_158(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* poSpell = NULL;
    CSpellSlot* poSlot = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetPendingSpell();
    PROCESS_ERROR(poSpell);

    if (poSpell->GetLevel() * nParam1 + nParam2 > g_Random(0, nParam3))
        return mrcTrue;

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_159(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    CBullet* poBullet = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    poBullet = spell->GetScanner().DoSingleBullet(spell, poChar, target, poChar, betNone, TRUE);
    PROCESS_SUCCESS(poBullet == NULL);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_160(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    {
        CAIProcessor* poAI = poChar->GetAIProcessor();
        PROCESS_SUCCESS(poAI == NULL);

        poChar->GetThreatList().Clear();
        poAI->SetTargetID(0);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_161(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRole* poRole = NULL;
    CItemBag* poBag = NULL;
    INT32 nA = 0, nB = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    poBag = poRole->GetItemBag();
    LOG_PROCESS_ERROR(poBag);

    nA = poBag->SearchDefaultContainer(nParam3, nParam1);
    PROCESS_SUCCESS(nA == INVALID_ITEM_SLOT);

    nB = poBag->SearchDefaultContainer(nParam3, nParam2);
    if (nB == INVALID_ITEM_SLOT)
        return mrcTrue;

    if (nA < nB)
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_162(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRole* poRole = NULL;
    CItemBag* poBag = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    poBag = poRole->GetItemBag();
    LOG_PROCESS_ERROR(poBag);

    nRetCode = poBag->LoseItemDur(nParam1, nParam2, ITEMLOG_SPELL);
    PROCESS_SUCCESS(!nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_163(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    CAura* poAuraInfo = NULL;
    CAuraInstance* poAuraInstance = NULL;
    TARGET_NODE* pTargetNode = NULL;
    INT32 nNewDurationFrm = 0;

    INT32 nCount = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam1);
    LOG_PROCESS_ERROR(poAuraInfo);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    {
        INT32 nB = 0, nD = 0, nE = 0;

        nB = round_div((6 + 2 * spell->GetLevel()) * nParam2, 12) + g_Random(spell->GetAttr(satDefaultValue), spell->GetAttr(satDefaultValueMax));
        nD = nB + nParam3 * poChar->GetAttr(catTaoAtk);
        nE = poChar->GetAttr(catMaxTaoAtk) - poChar->GetAttr(catTaoAtk) + 1;
        nE = nE < 0 ? 0 : nE;
        nE *= nParam3;
        nNewDurationFrm = GAME_FPS * LuckGo(nD, poChar->GetAttr(catLuck), nE);
    }

    spell->GetScanner().BeginTarget();

    while ((pTargetNode = spell->GetScanner().NextTarget()) != NULL)
    {
        target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
        if (target == NULL)
            continue;

        if (nCount >= spell->GetAttr(satMaxTargetCount))
            break;


        poAuraInstance = target->AppendAura(poAuraInfo, poChar, spell->GetFromObjID(), (ExtraParam*)pExParam, nNewDurationFrm, 0, spell->GetID(), spell->GetLevel());
        nCount++;
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_164(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    CAura* poAuraInfo = NULL;
    CAuraInstance* poAuraInstance = NULL;
    TARGET_NODE* pTargetNode = NULL;

    INT32 nCount = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam1);
    LOG_PROCESS_ERROR(poAuraInfo);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->GetScanner().BeginTarget();

    while ((pTargetNode = spell->GetScanner().NextTarget()) != NULL)
    {
        target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
        if (target == NULL)
            continue;

        if (nCount >= spell->GetAttr(satMaxTargetCount))
            break;

        INT32 nNewDurationFrm = 0;
        INT32 nB = 0, nC = 0;
        INT32 nTaoAtk = poChar->GetAttr(catTaoAtk);
        INT32 nMaxTaoAtk = poChar->GetAttr(catMaxTaoAtk);

        nB = round_div((6 + 2 * spell->GetLevel()) * nParam2, 12) + g_Random(spell->GetAttr(satDefaultValue), spell->GetAttr(satDefaultValueMax));

        if (nTaoAtk > 0)
        {
            if (nMaxTaoAtk > nTaoAtk)
                nC = g_Random(nTaoAtk, nMaxTaoAtk + 1);
            else
                nC = nTaoAtk;
        }

        nNewDurationFrm = GAME_FPS * (nB + nParam3 * nC);

        poAuraInstance = target->AppendAura(poAuraInfo, poChar, spell->GetFromObjID(), (ExtraParam*)pExParam, nNewDurationFrm, 0, spell->GetID(), spell->GetLevel());
        LOG_CHECK_ERROR(poAuraInstance);
        nCount++;
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_165(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poChar->SetHpTap(0);
    poChar->SetMpTap(0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_166(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    INT32 nAttr = g_Abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    LOG_PROCESS_ERROR(nAttr > catDefault && nAttr < catTotal);
    LOG_PROCESS_ERROR(nParam2 != 0);

    if (!bIsRollingBack)
    {
        poAura->SetCharLevel(poChar->GetLevel());
    }

    poChar->GetAttrList().AddAttrA(poChar, nAttr, poAura->GetCharLevel() / nParam2 + nParam3);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_167(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poCaster = NULL;
    INT64 llTargetX = 0, llTargetY = 0;
    INT32 nDirection8 = 0;
    INT32 nRetCode = 0;

    CScene* poScene = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    nDirection8 = ConvertToDir8(poChar->GetFaceDirection());

    llTargetX = poChar->GetX() + g_nAdjacentX[nDirection8] * nParam1 * GRID_WIDTH;
    llTargetY = poChar->GetY() + g_nAdjacentY[nDirection8] * nParam1 * GRID_WIDTH;

    {
        CRegion* poRegion = poScene->GetRegion(llTargetX / REGION_WIDTH, llTargetY / REGION_WIDTH);
        PROCESS_SUCCESS(poRegion == NULL);

        struct CScanEnemy
        {
            CCharacter* poCaster;
            INT64 llTargetGridX;
            INT64 llTargetGridY;
            BOOL bHasEnemy;
            BOOL bCheckMovement;

            BOOL operator()(CRegion* poRegion)
            {
                INT32 nRetCode = poRegion->TraverseCharacter(*this);
                return nRetCode;
            }

            BOOL operator()(CCharacter* poTarget)
            {
                INT32 nDirection8 = ConvertToDir8(g_GetDirection(poCaster->GetX(), poCaster->GetY(), poTarget->GetX(), poTarget->GetY()));

                if (g_Abs(nDirection8 - ConvertToDir8(poCaster->GetFaceDirection())) <= 2)
                {
                    if (g_Abs(poTarget->GetX() / GRID_WIDTH - llTargetGridX) <= (poTarget->GetRadius() / GRID_WIDTH) &&
                        g_Abs(poTarget->GetY() / GRID_WIDTH - llTargetGridY) <= (poTarget->GetRadius() / GRID_WIDTH))
                    {
                        if (!bCheckMovement || (bCheckMovement && (poTarget->GetMoveState() == CMS_WALK || poTarget->GetMoveState() == CMS_RUN)))
                        {
                            bHasEnemy = TRUE;
                            return FALSE;
                        }
                    }
                }
                return TRUE;
            }
        };

        CScanEnemy scan;
        scan.poCaster = poChar;
        scan.llTargetGridX = llTargetX / GRID_WIDTH;
        scan.llTargetGridY = llTargetY / GRID_WIDTH;
        scan.bHasEnemy = FALSE;
        scan.bCheckMovement = nParam2 != 0;

        //		poRegion->TraverseCharacter(scan);
        poRegion->Traverse9Region(scan);

        if (scan.bHasEnemy)
            return mrcTrue;
    }

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_168(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poChar->GetAttrList().AddAttrBase(poChar, catPHitCountDown, -1);

    //	DBG("count down: %d", poChar->GetAttr(catPHitCountDown));

    if (poChar->GetAttr(catPHitCountDown) == poChar->GetAttr(catPHitNum))
    {
        //		DBG("hit!");
        return mrcTrue;
    }

    if (poChar->GetAttr(catPHitCountDown) <= 0)
    {
        poChar->GetAttrList().SetAttrBase(poChar, catPHitCountDown, 7 - nParam1);
        poChar->GetAttrList().SetAttrBase(poChar, catPHitNum, g_Random(0, poChar->GetAttr(catPHitCountDown)));

        //		DBG("count down: %d, num: %d", poChar->GetAttr(catPHitCountDown), poChar->GetAttr(catPHitNum));
    }

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_169(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT64 llGridX = 0, llGridY = 0;
    INT32 nDirection8 = 0;

    CCharacter* poTargetChar = NULL;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    nDirection8 = ConvertToDir8(poRole->GetFaceDirection());

    llGridX = poRole->GetX() / GRID_WIDTH + g_nAdjacentX[nDirection8] * nParam1;
    llGridY = poRole->GetY() / GRID_WIDTH + g_nAdjacentY[nDirection8] * nParam1;

    poTargetChar = CGameWorld::GetInstance().FindValidCharacter(poRole->GetClientUploadTargetID());
    PROCESS_SUCCESS(poTargetChar == NULL);

    if (g_Abs(poTargetChar->GetX() / GRID_WIDTH - llGridX) <= poTargetChar->GetRadius() / GRID_WIDTH && g_Abs(poTargetChar->GetY() / GRID_WIDTH - llGridY) <= poTargetChar->GetRadius() / GRID_WIDTH)
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_170(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nRetCode = spell->GetScanner().DoBullet(spell, poChar, betNone);
    LOG_PROCESS_ERROR(nRetCode >= 0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_171(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nNewFrm = 0;
    CAuraInstance* aura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    aura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(aura);

    nNewFrm = g_Random(nParam1, nParam2) * GAME_FPS;

    aura->ResetFrm(nNewFrm, 0, TRUE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_172(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellSlot* poSlot = NULL;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    poSlot = poRole->GetSpellSlot(poRole->GetLastSlot());
    PROCESS_SUCCESS(poSlot == NULL);

    if (nParam1 != 0)
    {
        PROCESS_SUCCESS(poSlot->GetMainSpellID() != nParam1);
    }

    //	poSlot->ClearNextSpell();

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_173(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    INT64 llSpellGridX = 0, llSpellGridY = 0;
    INT64 llTargetGridX = 0, llTargetGridY = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    llSpellGridX = spell->GetScanner().GetTargetX() / GRID_WIDTH;
    llSpellGridY = spell->GetScanner().GetTargetY() / GRID_WIDTH;

    llTargetGridX = target->GetX() / GRID_WIDTH;
    llTargetGridY = target->GetY() / GRID_WIDTH;

    PROCESS_SUCCESS(g_Abs(llSpellGridX - llTargetGridX) > spell->GetAttr(satTolerantGrids) || g_Abs(llSpellGridY - llTargetGridY) > spell->GetAttr(satTolerantGrids));

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_174(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    ////WRN("Execute Function - %s", __FUNCTION__);

    CRegion* poRegion = NULL;
    INT32 nRetCode = 0;
    TARGET_NODE* pTargetNode = NULL;
    CTargetScanner Scanner;
    CSpell* poSpell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poRegion = poChar->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    poSpell = CSpellConfig::GetInstance().FindSpellTemplate(nParam2, 1, 0);
    LOG_PROCESS_ERROR(poSpell);

    nRetCode = Scanner.Init(poChar, poSpell->GetAttr(satRangeType),
        poSpell->GetAttr(satRangeParam1), poSpell->GetAttr(satRangeParam2), poSpell->GetAngleAttr(satRangeParam3),
        poChar->GetFaceDirection(), poSpell->GetAttr(satTargetObjType), poSpell->GetAttr(satTargetRelationship), poSpell->GetAttr(satTargetStance),
        poSpell->GetAttr(satTargetDeadLimit), poSpell->GetAttr(satUseWeaponRange), poSpell->GetAttr(satIgnoreObstacle), poSpell->GetAttr(satTargetCheckNode));

    Scanner.SetCenterPt(poChar->GetX(), poChar->GetY(), poChar->GetZ());
    Scanner.SetDirection(poChar->GetFaceDirection());

    {
        INT64 nTargetX = poChar->GetX(), nTargetY = poChar->GetY(), nTargetZ = poChar->GetZ();
        INT32 nNormalizedDirection = NormalizeDirection(poSpell->GetAttr(satOffsetDirection) + Scanner.GetDirection());
        INT32 nDirection8 = ConvertToDir8(Scanner.GetDirection());
        INT32 nDirection8V = NormalizeDirection8(nDirection8 - 4);

        nTargetX += poSpell->GetAttr(satHOffsetGrids) * g_nAdjacentX[nDirection8V] * GRID_WIDTH;
        nTargetY += poSpell->GetAttr(satHOffsetGrids) * g_nAdjacentY[nDirection8V] * GRID_WIDTH;

        nTargetX += poSpell->GetAttr(satVOffsetGrids) * g_nAdjacentX[nDirection8] * GRID_WIDTH;
        nTargetY += poSpell->GetAttr(satVOffsetGrids) * g_nAdjacentY[nDirection8] * GRID_WIDTH;

        Scanner.SetTargetPt(nTargetX, nTargetY, nTargetZ);
    }


    nRetCode = Scanner.Scan(poRegion, poChar);
    PROCESS_ERROR(nRetCode);

    PROCESS_SUCCESS(Scanner.GetTargetCount() < nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_175(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nStep = 0;
    CSpellInstance* spell = NULL;
    CNpc* poPet = NULL;
    CScene* poScene = NULL;
    INT32 nMaxLevel = 0;
    CRole* poMaster = NULL;
    INT64 llTargetX = 0, llTargetY = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poMaster = static_cast<CRole*>(poChar);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nMaxLevel = 2 * spell->GetLevel() + 1;

    llTargetX = poChar->GetX();
    llTargetY = poChar->GetY();

    GetOffsetedPos(llTargetX, llTargetY, poChar->GetFaceDirection(), 0, 1, llTargetX, llTargetY);

    poPet = poScene->SpawnNpc(nParam1, llTargetX, llTargetY, poChar->GetZ(), poChar->GetFaceDirection(), poChar->GetObjID(), NULL);
    PROCESS_SUCCESS(poPet == NULL);
    nStep = 1;
    INF("pet debug log: ModifierChar_175 HostID[%d] OBJID[%d]", poMaster->GetObjID(), poPet->GetObjID());

    nRetCode = poPet->TransformToPet(nParam2, spell->GetLevel(), spell->GetLevel(), nMaxLevel, CONVERT_TIME(3600ll * 24 * 10 * 1000), 0);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = poMaster->AddPet(poPet->GetObjID(), nParam2);
    LOG_PROCESS_ERROR(nRetCode);
    nStep = 2;

    // refresh
    poPet->SetHp(poPet->GetAttr(catMaxHp));

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    if (2 <= nStep)
    {
        nRetCode = poMaster->DelPet(poPet->GetObjID());
        LOG_CHECK_ERROR(nRetCode);
    }
    if (1 <= nStep)
    {
        LOG_CHECK_ERROR(poPet->GetScene());
        if (poPet->GetScene())
        {
            nRetCode = poPet->GetScene()->RemoveNpc(poPet);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = poPet->UnInit();
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CGameWorld::GetInstance().DelNpc(poPet);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
    return mrcError;
}

INT32 ModifierChar_176(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nStep = 0;
    CSpellInstance* spell = NULL;
    CNpc* poPet = NULL;
    CScene* poScene = NULL;
    INT32 nMaxLevel = 0;
    CRole* poMaster = NULL;
    INT64 llTargetX = 0, llTargetY = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poMaster = static_cast<CRole*>(poChar);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nMaxLevel = 2 * spell->GetLevel() + 1;

    llTargetX = poChar->GetX();
    llTargetY = poChar->GetY();

    GetOffsetedPos(llTargetX, llTargetY, poChar->GetFaceDirection(), 0, 1, llTargetX, llTargetY);

    poPet = poScene->SpawnNpc(nParam1, llTargetX, llTargetY, poChar->GetZ(), poChar->GetFaceDirection(), poChar->GetObjID(), NULL);
    PROCESS_SUCCESS(poPet == NULL);
    nStep = 1;
    INF("pet debug log: ModifierChar_176 HostID[%d] OBJID[%d]", poMaster->GetObjID(), poPet->GetObjID());

    nRetCode = poPet->TransformToPet(nParam2, spell->GetLevel(), spell->GetLevel(), nMaxLevel, CONVERT_TIME(3600ll * 24 * 10 * 1000), 0);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = poMaster->AddPet(poPet->GetObjID(), nParam2);
    LOG_PROCESS_ERROR(nRetCode);
    nStep = 2;

    // refresh
    poPet->SetHp(poPet->GetAttr(catMaxHp));

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    if (2 <= nStep)
    {
        nRetCode = poMaster->DelPet(poPet->GetObjID());
        LOG_CHECK_ERROR(nRetCode);
    }
    if (1 <= nStep)
    {
        LOG_CHECK_ERROR(poPet->GetScene());
        if (poPet->GetScene())
        {
            nRetCode = poPet->GetScene()->RemoveNpc(poPet);
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = poPet->UnInit();
            LOG_CHECK_ERROR(nRetCode);

            nRetCode = CGameWorld::GetInstance().DelNpc(poPet);
            LOG_CHECK_ERROR(nRetCode);
        }
    }
    return mrcError;
}

INT32 ModifierChar_177(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nResult = 0;

    CScene* poScene = NULL;
    CRole* poRole = NULL;
    CLuaScript* poScript = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    // instant move.

    poScript = CScriptMgr::GetInstance().FindScript(ITEM_SCRIPT_NAME);
    LOG_PROCESS_ERROR(poScript);

    nRetCode = poScript->CallFunction("_random_jump", "odd:d", poRole, "CRole", poRole->GetRevivePos().nSceneTempID, poRole->GetRevivePos().nSceneID, &nResult);
    LOG_PROCESS_ERROR(nRetCode);

    PROCESS_SUCCESS(nResult != errSuccess);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_178(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CRegion* poRegion = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poRegion = poChar->GetRegion();
    LOG_PROCESS_ERROR(poRegion);

    {
        struct Scan9Region
        {
            INT32 nGridLimit;
            INT32 nClearRate;
            INT64 llCenterGridX;
            INT64 llCenterGridY;
            UINT32 dwHostID;

            BOOL operator()(CRegion* poRegion)
            {
                INT32 nRetCode = 0;

                nRetCode = poRegion->TraverseNpc(*this);
                LOG_PROCESS_ERROR(nRetCode);

                return TRUE;
            Exit0:
                return FALSE;

            }
            BOOL operator()(CNpc* poNpc)
            {
                CAIProcessor* poAI = NULL;
                INT64 llGridX = 0;
                INT64 llGridY = 0;

                PROCESS_ERROR(!poNpc->GetNpcData()->bIgnoreInvisible);

                poAI = poNpc->GetAIProcessor();
                LOG_PROCESS_ERROR(poAI);

                llGridX = poNpc->GetX() / GRID_WIDTH;
                llGridY = poNpc->GetY() / GRID_WIDTH;

                if (g_Abs(llGridX - llCenterGridX) <= nGridLimit && g_Abs(llGridY - llCenterGridY) <= nGridLimit)
                {
                    PROCESS_ERROR(g_Random(0, nClearRate) == 0);
                }

                if (poNpc->GetHostID() != dwHostID)
                {
                    poNpc->GetThreatList().RemoveThreatNode(dwHostID);

                    if (poAI->GetTargetID() == dwHostID)
                        poAI->SetTargetID(0);
                }

            Exit0:
                return TRUE;
            }
        };

        Scan9Region scan;
        scan.llCenterGridX = poChar->GetX() / GRID_WIDTH;
        scan.llCenterGridY = poChar->GetY() / GRID_WIDTH;
        scan.nClearRate = nParam2;
        scan.nGridLimit = nParam1;
        scan.dwHostID = poChar->GetObjID();

        nRetCode = poRegion->Traverse9Region(scan);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_179(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nDirection8 = 0;
    INT32 nInvertDirection8 = 0;
    INT64 llDestX = 0, llDestY = 0;
    INT32 nInfectedTargetCount = 0;
    INT32 nSpeed = CONVERT_TIME(nParam1);
    CSpellInstance* spell = NULL;
    TARGET_NODE* pNode = NULL;
    CBullet* poBullet = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    LOG_PROCESS_ERROR(spell->GetAttr(satMaxTargetCount) <= MAX_SPELL_TARGET_COUNT);

    LOG_PROCESS_ERROR(nParam1 > 0);

    {
        CTargetScanner& Scanner = spell->GetScanner();
        CCharacter* poStopChar = NULL;
        INT64 llOriginX = poChar->GetX();
        INT64 llOriginY = poChar->GetY();

        struct InfectedTarget
        {
            CCharacter* target;
            INT64 llDestX;
            INT64 llDestY;
        };

        InfectedTarget Ift[MAX_SPELL_TARGET_COUNT] = { 0 };

        nDirection8 = ConvertToDir8(Scanner.GetDirection());
        nInvertDirection8 = NormalizeDirection8(nDirection8 - 4);

        llDestX = poChar->GetX() + g_nAdjacentX[nDirection8] * GRID_WIDTH;
        llDestY = poChar->GetY() + g_nAdjacentY[nDirection8] * GRID_WIDTH;


        Scanner.BeginTarget();

        while ((pNode = Scanner.NextTarget()) != NULL && nInfectedTargetCount < spell->GetAttr(satMaxTargetCount))
        {
            CCharacter* target = CGameWorld::GetInstance().FindValidCharacter(pNode->dwTargetID);
            if (target == NULL)
                continue;

            if (target->GetLevel() < poChar->GetLevel())
            {
                Ift[nInfectedTargetCount].target = target;
                nInfectedTargetCount++;
                continue;
            }

            poStopChar = target;
            break;
        }

        if (poStopChar != NULL)
        {
            llDestX = poStopChar->GetX() + g_nAdjacentX[nInvertDirection8] * GRID_WIDTH * (nInfectedTargetCount + 1);
            llDestY = poStopChar->GetY() + g_nAdjacentY[nInvertDirection8] * GRID_WIDTH * (nInfectedTargetCount + 1);
        }

        if (nInfectedTargetCount > 0)
        {
            for (INT32 i = nInfectedTargetCount - 1; i >= 0; i--)
            {
                INT32 nDistGrids1 = g_Max(g_Abs((Ift[i].target->GetX() - poChar->GetX()) / GRID_WIDTH), g_Abs((Ift[i].target->GetY() - poChar->GetY()) / GRID_WIDTH));

                Ift[i].llDestX = llDestX + g_nAdjacentX[nDirection8] * GRID_WIDTH * (i + 1);
                Ift[i].llDestY = llDestY + g_nAdjacentY[nDirection8] * GRID_WIDTH * (i + 1);

                INT32 nDistGrids2 = g_Max(g_Abs((Ift[i].llDestX - poChar->GetX()) / GRID_WIDTH), g_Abs((Ift[i].llDestY - poChar->GetY()) / GRID_WIDTH));

                if (nDistGrids2 <= nDistGrids1)
                    continue;

                nRetCode = Ift[i].target->KnockBack(nSpeed, Ift[i].llDestX, Ift[i].llDestY);
                if (nRetCode)
                {
                    CAuraInstance* poAuraInst = NULL;

                    poAuraInst = Ift[i].target->GetFirstAura(nParam3);

                    if (poAuraInst == NULL);
                    else if (Ift[i].llDestX == Ift[i].target->GetX() && Ift[i].llDestY == Ift[i].target->GetY())
                        continue;

                    CAura* poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam3);
                    LOG_PROCESS_ERROR(poAuraInfo);

                    poAuraInst = Ift[i].target->AppendAura(poAuraInfo, poChar, poChar->GetObjID(), NULL, 0, 0, 0, 0);
                    LOG_PROCESS_ERROR(poAuraInst);
                }
            }
        }

        nRetCode = poChar->Dash(nSpeed, llDestX, llDestY);
        if (!nRetCode)
        {
            if (nInfectedTargetCount <= 0)
            {
                CAuraInstance* poAuraInst = NULL;
                CAura* poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam2);
                LOG_PROCESS_ERROR(poAuraInfo);

                poAuraInst = poChar->AppendAura(poAuraInfo, poChar, poChar->GetObjID(), NULL, 0, 0, 0, 0);
                LOG_PROCESS_ERROR(poAuraInst);
            }

            PROCESS_SUCCESS(TRUE);
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_180(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CMissile* missile = NULL;
    CSpellInstance* spell = NULL;
    INT32 nMissileID = nParam1;
    INT32 nDirection = 0;
    INT32 nLockType = spltInvalid;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    nDirection = spell->GetScanner().GetDirection();

    {
        INT64 llInitX, llInitY, llInitZ = 0;
        INT32 nOffsetDirection = 0;
        INT32 nNewLifeFrm = 0;
        INT32 nA = 0, nB = 0, nC = 0;

        const MISSILE_DATA* poMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nMissileID);
        LOG_PROCESS_ERROR(poMissileData);

        missile = CGameWorld::GetInstance().NewMissile();
        LOG_PROCESS_ERROR(missile);

        nOffsetDirection = NormalizeDirection(nDirection + poMissileData->m_nAttr[matOffsetDirection]);

        if (spell->GetAttr(satLockType) == spltDirection)
        {
            llInitX = poChar->GetX();
            llInitY = poChar->GetY();
            llInitZ = poChar->GetZ();
        }
        else
        {
            spell->GetScanner().GetTargetPt(llInitX, llInitY, llInitZ);
            llInitX = llInitX + spell->GetLengthAttr(satOffsetDistance) * g_Cos(spell->GetAttr(satOffsetDirection) + poChar->GetFaceDirection()) / SIN_COS_NUMBER;
            llInitY = llInitY + spell->GetLengthAttr(satOffsetDistance) * g_Sin(spell->GetAttr(satOffsetDirection) + poChar->GetFaceDirection()) / SIN_COS_NUMBER;;
        }

        llInitX = llInitX + (INT64)poMissileData->m_nAttr[matOffsetDistance] * g_Cos(nOffsetDirection) / SIN_COS_NUMBER;
        llInitY = llInitY + (INT64)poMissileData->m_nAttr[matOffsetDistance] * g_Sin(nOffsetDirection) / SIN_COS_NUMBER;
        llInitZ = llInitZ + poMissileData->m_nAttr[matHeight];

        nB = round_div((6 + 2 * spell->GetLevel()) * nParam2, 12) + g_Random(spell->GetAttr(satDefaultValue), spell->GetAttr(satDefaultValueMax));

        if (poChar->GetAttr(catTaoAtk) > 0)
            nC = poChar->GetAttr(catTaoAtk);

        if (poChar->GetAttr(catMaxTaoAtk) > poChar->GetAttr(catTaoAtk))
            nC = nC + g_Random(0, poChar->GetAttr(catMaxTaoAtk) - poChar->GetAttr(catTaoAtk) + 1);

        nNewLifeFrm = GAME_FPS * (nB + nParam3 * nC);

        nRetCode = missile->Init(poMissileData, poChar, spell, llInitX, llInitY, llInitZ, nDirection, nNewLifeFrm);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_181(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nInvertDirection8 = 0;
    INT64 llDestX = 0, llDestY = 0;
    CAuraInstance* poAura = NULL;
    CCharacter* poCaster = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    LOG_PROCESS_ERROR(poCaster);

    nInvertDirection8 = ConvertToDir8(g_GetDirection(poCaster->GetX(), poCaster->GetY(), poChar->GetX(), poChar->GetY()));

    llDestX = poChar->GetX() + g_nAdjacentX[nInvertDirection8] * GRID_WIDTH * nParam1;
    llDestY = poChar->GetY() + g_nAdjacentY[nInvertDirection8] * GRID_WIDTH * nParam1;

    nRetCode = poChar->KnockBack(CONVERT_TIME(nParam2), llDestX, llDestY);
    PROCESS_SUCCESS(!nRetCode);

    poChar->IncMoveDelay(CONVERT_TIME(nParam3));

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_182(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nDirection8 = 0;
    INT64 llDestX = 0, llDestY = 0;
    INT32 nGrids = 0;

    CAuraInstance* poAura = NULL;
    CCharacter* poCaster = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    LOG_PROCESS_ERROR(poCaster);

    nDirection8 = ConvertToDir8(g_GetDirection(poCaster->GetX(), poCaster->GetY(), poChar->GetX(), poChar->GetY()));

    nGrids = g_Max(0, poAura->GetSpellLevel() - 1) + g_Random(0, nParam1) + 1;

    llDestX = poChar->GetX() + g_nAdjacentX[nDirection8] * GRID_WIDTH * nGrids;
    llDestY = poChar->GetY() + g_nAdjacentY[nDirection8] * GRID_WIDTH * nGrids;

    nRetCode = poChar->KnockBack(CONVERT_TIME(nParam3), llDestX, llDestY);
    PROCESS_SUCCESS(!nRetCode);

    if (poChar->GetObjType() == otNpc)
    {
        poChar->IncMoveDelay(CONVERT_TIME(nParam2 * nGrids));
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_183(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    //	INT32 nRetCode = 0;
    //	CNpc* poNpc = NULL;
    //	CNpc* poPet = NULL;
    //	CRole* poMaster = NULL;
    //	CScene* poScene = NULL;
    //	CAuraInstance* poAura = NULL;
    //	INT32 nTemplateID = 0;
    //	INT32 nMaxLevel = 0;
    //	INT32 nPetLevel = 0;
    //	INT32 nNpcLevel = 0;
    //	INT32 nFaceDirection = 0;
    //	INT32 nWalkSpeed = 0;
    //	INT32 nAttackSpeed = 0;
    //	CAIProcessor* poAi = NULL;
    //
    //	CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    //	LOG_PROCESS_ERROR(poChar);
    //
    //	LOG_PROCESS_ERROR(!bIsRollingBack);
    //
    //	poAura = poChar->GetPendingAura();
    //	LOG_PROCESS_ERROR(poAura);
    //
    //	poScene = poChar->GetScene();
    //	LOG_PROCESS_ERROR(poScene);
    //
    //	poMaster = CGameWorld::GetInstance().FindValidRole(poAura->GetCasterID());
    //	LOG_PROCESS_ERROR(poMaster);
    //
    //	// Seduced.
    //	LOG_PROCESS_ERROR(poChar->GetObjType() == otNpc);
    //
    //	poNpc = static_cast<CNpc*>(poChar);
    //
    //	nNpcLevel = poNpc->GetLevel();
    //
    //	if (poNpc->GetPetType() != 0)
    //	{
    //		nPetLevel = poNpc->GetPetLevel();
    //	}
    //
    //	nTemplateID = poNpc->GetNpcData()->nTemplateID;
    //
    //	poNpc->SetGenerateDrop(FALSE);
    //
    //	nFaceDirection = poNpc->GetFaceDirection();
    //
    //	nRetCode = poNpc->Die(cdrSilent);
    //	LOG_PROCESS_ERROR(nRetCode);
    //
    //	nRetCode = poNpc->RemoveCorpse();
    //	LOG_PROCESS_ERROR(nRetCode);
    //
    //	poPet = poScene->SpawnPet(nTemplateID, nNpcLevel, nqNormal1, poNpc->GetX(), poNpc->GetY(), poNpc->GetZ(), nFaceDirection, poAura->GetCasterID(), NULL, soatNoPerforSpawn);
    //	LOG_PROCESS_ERROR(poPet);
    //
    //	nMaxLevel = 2 * poAura->GetSpellLevel() + 1;
    //
    //	nRetCode = poPet->TransformToPet(nParam1, nPetLevel, nMaxLevel, poAura->GetRemainFrm(), 0);
    //	LOG_PROCESS_ERROR(nRetCode);
    //
    //	poPet->SetGenerateDrop(FALSE);
    //
    //	nRetCode = poMaster->AddPet(poPet->GetObjID(), nParam1);
    //	LOG_PROCESS_ERROR(nRetCode);
    //
    //	poAi = poPet->GetAIProcessor();
    //	LOG_PROCESS_ERROR(poAi);
    //
    //	nWalkSpeed = CONVERT_TIME(1500 - poAura->GetSpellLevel() * 200);
    //	nAttackSpeed = CONVERT_TIME(2000 - poAura->GetSpellLevel() * 200);
    //
    //	nWalkSpeed = nWalkSpeed < poChar->GetAttr(catWalkSpeed) ? nWalkSpeed : poChar->GetAttr(catWalkSpeed);
    //	nAttackSpeed = nAttackSpeed < poAi->GetGlobalCDDelayFrame() ? nAttackSpeed : poAi->GetGlobalCDDelayFrame();
    //
    //	poAi->SetGlobalCDDelayFrame(nAttackSpeed);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_184(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRole* poRole = NULL;
    CItemBag* poBag = NULL;
    INT32 nA = 0, nB = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    poBag = poRole->GetItemBag();
    LOG_PROCESS_ERROR(poBag);

    nA = poBag->SearchDefaultContainer(nParam3, nParam1);
    PROCESS_SUCCESS(nA == INVALID_ITEM_SLOT);

    nB = poBag->SearchDefaultContainer(nParam3, nParam2);
    PROCESS_SUCCESS(nB == INVALID_ITEM_SLOT);

    if (nA < nB)
    {
        Item* item = poBag->GetItem(CONSLOT_DEFAULT, nA);
        LOG_PROCESS_ERROR(item);

        nRetCode = poBag->LoseItemDur(item->m_pTemplate->m_nTemplateID, 1, ITEMLOG_SPELL);
        LOG_PROCESS_ERROR(nRetCode);

        return mrcTrue;
    }
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_185(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nHpRecovery = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    if (nParam2 == 0)
    {
        nHpRecovery = nParam1 * (RATE_MAX + poChar->GetAttr(catDrugHpRecovery) + poChar->GetAttr(catDrugHpSlowRecovery)) / RATE_MAX;
    }
    else
    {
        nHpRecovery = nParam1;
    }

    poChar->ChangeTotalDrugCureHp(nHpRecovery);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_186(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poChar->ChangeTotalDrugCureMp(nParam1 * (RATE_MAX + poChar->GetAttr(catDrugMpRecovery)) / RATE_MAX);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_187(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CScene* poScene = NULL;
    CNpc* poNpc = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    PROCESS_SUCCESS(poChar->GetObjType() != otNpc);

    poNpc = static_cast<CNpc*>(poChar);

    poNpc->SetGlobalCDDelayFrame(g_Max<INT64>(poNpc->GetGlobalCDDelayFrame() - CONVERT_TIME(nParam1), 0));

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_188(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nAbilityPts = 0;

    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    if (nParam2 > 0)
        nAbilityPts = 3;
    else
        nAbilityPts = g_Random(0, 3) + 1;

    nRetCode = poRole->AddSpellPts(nParam1, nAbilityPts);
    PROCESS_SUCCESS(!nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_189(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CAIProcessor* poAI = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		nRetCode = poChar->SetAIProcessor(nParam1);
        //		LOG_PROCESS_ERROR(nRetCode);

        poAI = poChar->GetAIProcessor();
        LOG_PROCESS_ERROR(poAI);

        nRetCode = poAI->EnterIdle();
        LOG_PROCESS_ERROR(nRetCode);

        poChar->GetThreatList().Clear();
    }
    else
    {
        //		nRetCode = poChar->SetAIProcessor(poChar->GetOriginalAiID());
        poAI = poChar->GetAIProcessor();
        LOG_PROCESS_ERROR(poAI);

        nRetCode = poAI->EnterIdle();
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_190(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    PROCESS_SUCCESS(poRole->GetPetCount() >= nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_191(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CCharacter* poTargetChar = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poTargetChar = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    LOG_PROCESS_ERROR(poTargetChar);

    PROCESS_SUCCESS(spell->GetScanner().CheckTarget(poChar, poTargetChar) < 0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_192(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpell* spell = NULL;
    CRole* poRole = NULL;
    INT32 nSlot = g_Abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetEquippingSpell();
    LOG_PROCESS_ERROR(spell);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    nRetCode = poRole->EquipSpell(nSlot, spell->GetID());
    PROCESS_SUCCESS(!nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_193(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    INT64 llSpellGridX = 0, llSpellGridY = 0;
    INT64 llTargetGridX = 0, llTargetGridY = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    llSpellGridX = spell->GetScanner().GetTargetX() / GRID_WIDTH;
    llSpellGridY = spell->GetScanner().GetTargetY() / GRID_WIDTH;

    llTargetGridX = target->GetX() / GRID_WIDTH;
    llTargetGridY = target->GetY() / GRID_WIDTH;

    PROCESS_SUCCESS(g_Abs(llSpellGridX - llTargetGridX) > nParam1 || g_Abs(llSpellGridY - llTargetGridY) > nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_194(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    INT64 llSpellGridX = 0, llSpellGridY = 0;
    INT64 llTargetGridX = 0, llTargetGridY = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    PROCESS_SUCCESS(target->GetHp() >= target->GetAttr(catMaxHp));

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_195(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellSlot* poSlot = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poSlot = poChar->GetSpellSlot(nParam1);
    LOG_PROCESS_ERROR(poSlot);

    PROCESS_SUCCESS(poSlot->GetMainSpellID() != nParam2);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_196(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    PROCESS_SUCCESS(poAura->GetSpellLevel() > nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_197(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CMissile* missile = NULL;
    CSpellInstance* spell = NULL;
    INT32 nMissileID = nParam1;
    INT32 nDirection = CONVERT_ANGLE(nParam2);
    INT32 nLockType = spltInvalid;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    {
        INT64 llInitX, llInitY, llInitZ = 0;

        const MISSILE_DATA* poMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nMissileID);
        LOG_PROCESS_ERROR(poMissileData);

        missile = CGameWorld::GetInstance().NewMissile();
        LOG_PROCESS_ERROR(missile);

        if (spell->GetAttr(satLockType) == spltDirection)
        {
            llInitX = poChar->GetX();
            llInitY = poChar->GetY();
            llInitZ = poChar->GetZ();
        }
        else
        {
            spell->GetScanner().GetTargetPt(llInitX, llInitY, llInitZ);
            llInitX = llInitX + spell->GetLengthAttr(satOffsetDistance) * g_Cos(spell->GetAttr(satOffsetDirection) + poChar->GetFaceDirection()) / SIN_COS_NUMBER;
            llInitY = llInitY + spell->GetLengthAttr(satOffsetDistance) * g_Sin(spell->GetAttr(satOffsetDirection) + poChar->GetFaceDirection()) / SIN_COS_NUMBER;;
        }

        llInitZ = llInitZ + poMissileData->m_nAttr[matHeight];

        nRetCode = missile->Init(poMissileData, poChar, spell, llInitX, llInitY, llInitZ, nDirection, 0);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_198(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    INT32 eMoveState = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    LOG_PROCESS_ERROR(target);

    eMoveState = target->GetMoveState();

    if (nParam1 == 1)
        PROCESS_SUCCESS(eMoveState != CMS_WALK);
    else if (nParam1 == 2)
        PROCESS_SUCCESS(eMoveState != CMS_RUN);
    else
        PROCESS_SUCCESS(target->GetMoveState() != CMS_RUN && target->GetMoveState() != CMS_WALK);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_199(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nEffectValue = 0;
    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    nEffectValue = g_Random(0, poAura->GetStackCount() * 5) + poAura->GetStackCount() * nParam1;

    poAura->SetEffectValue(nEffectValue);

    poAura->DoTimeupEffect(poChar, TRUE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_200(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;
    CSpellInstance* spell = NULL;
    TARGET_NODE* pTargetNode = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->GetScanner().BeginTarget();

    while ((pTargetNode = spell->GetScanner().NextTarget()) != NULL)
    {
        CCharacter* target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
        if (target == NULL)
            continue;

        if (nParam1 > 0)
        {
            poAura = target->GetFirstAura(nParam1);

            if (poAura != NULL && poAura->GetCasterID() == poChar->GetObjID())
                target->RemoveAura(poAura, TRUE);
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_201(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Knockback TRUE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclKnockBack));
        poChar->IncControlCount(cclKnockBack);
    }
    else
    {
        //		DBG("---------------- Knockback FALSE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclKnockBack));
        poChar->DecControlCount(cclKnockBack);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

static INT32 __GetHpChangeValue(UINT32 dwLevel, INT32 nParam)
{
    if (dwLevel == 0)
        return 200;

    return __GetHpChangeValue(dwLevel - 1, nParam) + (dwLevel + 2) * nParam;
}

INT32 ModifierChar_202(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAuraInst = NULL;
    CNpc* poPet = NULL;
    INT32 nHpChange = 0;
    INT32 nPetLevel = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otNpc);

    poPet = static_cast<CNpc*>(poChar);

    poAuraInst = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAuraInst);

    if (!bIsRollingBack)
    {
        nPetLevel = poPet->GetPetLevel();
        nHpChange = 20 * nPetLevel * nPetLevel + 60 * nPetLevel + 200;

        poPet->GetAttrList().AddAttrA(poPet, catMaxHp, nHpChange);

        if (poPet->GetPetHpRestore() > 0)
        {
            poPet->SetHp(poPet->GetPetHpRestore());
            poPet->SetPetHpRestore(0);
        }

        poAuraInst->SetTotalEffectValue(nHpChange);
    }
    else
    {
        nHpChange = poAuraInst->GetTotalEffectValue();

        poPet->GetAttrList().AddAttrA(poPet, catMaxHp, -nHpChange);

        poPet->SetPetHpRestore(poPet->GetHp());

        poPet->ChangeHp(1, cdrDefault);
        poPet->ChangeHp(-1, cdrDefault);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_203(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT64 llGridX = 0, llGridY = 0;
    INT64 llTgGridX = 0, llTgGridY = 0;

    CCharacter* poTargetChar = NULL;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    llGridX = poRole->GetX() / GRID_WIDTH;
    llGridY = poRole->GetY() / GRID_WIDTH;

    poTargetChar = CGameWorld::GetInstance().FindValidCharacter(poRole->GetClientUploadTargetID());
    PROCESS_SUCCESS(poTargetChar == NULL);

    if (nParam1 != 0)
        PROCESS_SUCCESS(poTargetChar->GetMoveState() != CMS_WALK && poTargetChar->GetMoveState() != CMS_RUN);

    if (nParam2 > 0)
    {
        llTgGridX = poTargetChar->GetX() / GRID_WIDTH;
        llTgGridY = poTargetChar->GetY() / GRID_WIDTH;

        PROCESS_SUCCESS(g_Abs(llTgGridX - llGridX) > (poTargetChar->GetRadius() / GRID_WIDTH + nParam2) || g_Abs(llTgGridY - llGridY) > (poTargetChar->GetRadius() / GRID_WIDTH + nParam2));
    }

    return mrcTrue;

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_204(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Polymorph TRUE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclFury));
        poChar->IncControlCount(cclFury);
    }
    else
    {
        //		DBG("---------------- Polymorph FALSE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclFury));
        poChar->DecControlCount(cclFury);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_205(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    CSpell* subspell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    subspell = CSpellConfig::GetInstance().FindSpellTemplate(nParam1, spell->GetLevel(), 0);
    LOG_PROCESS_ERROR(subspell);

    nRetCode = poChar->CastInstantSpell(poChar, subspell, target, target->GetX(), target->GetY(), target->GetZ());
    PROCESS_SUCCESS(nRetCode != errSuccess);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_206(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CSpell* subspell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    subspell = CSpellConfig::GetInstance().FindSpellTemplate(nParam1, spell->GetLevel(), 0);
    LOG_PROCESS_ERROR(subspell);

    {
        CTargetScanner& Scanner = spell->GetScanner();

        nRetCode = poChar->CastInstantSpell(poChar, subspell, NULL, Scanner.GetTargetX(), Scanner.GetTargetY(), Scanner.GetTargetZ());
        PROCESS_SUCCESS(nRetCode != errSuccess);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_207(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Leech TRUE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclLeech));
        poChar->IncControlCount(cclLeech);
    }
    else
    {
        //		DBG("---------------- Leech FALSE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclLeech));
        poChar->DecControlCount(cclLeech);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_208(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_SUCCESS(!poChar->GetCCFlag().CheckFlag(nParam1));

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_209(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;
    CSpell* poSpell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poSpell = CSpellConfig::GetInstance().FindSpellTemplate(nParam1, poAura->GetSpellLevel(), 0);
    LOG_PROCESS_ERROR(poSpell);

    nRetCode = poChar->CastInstantSpell(poChar, poSpell, poChar, poChar->GetX(), poChar->GetY(), poChar->GetZ());
    PROCESS_SUCCESS(nRetCode != errSuccess);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_210(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAura* poAuraInfo = NULL;
    CAuraInstance* poAuraInst = NULL;

    INT32 nAuraID = g_Abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam1);
        LOG_PROCESS_ERROR(poAuraInfo);

        poAuraInst = poChar->AppendAura(poAuraInfo, poChar, poChar->GetObjID(), NULL, CONVERT_TIME(nParam2), 0, 0, 0);
        LOG_PROCESS_ERROR(poAuraInst);
    }
    else
    {
        poAuraInst = poChar->GetFirstAura(nAuraID);
        LOG_PROCESS_ERROR(poAuraInst);

        poChar->ReduceAuraStack(poAuraInst, 1);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_211(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- MemoSuit TRUE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclMemoSuit));
        poChar->IncControlCount(cclMemoSuit);
    }
    else
    {
        //		DBG("---------------- MemoSuit FALSE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclMemoSuit));
        poChar->DecControlCount(cclMemoSuit);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_212(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Probe Necklace TRUE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclProbeNecklace));
        poChar->IncControlCount(cclProbeNecklace);
    }
    else
    {
        //		DBG("---------------- Probe Necklace FALSE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclProbeNecklace));
        poChar->DecControlCount(cclProbeNecklace);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_213(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        //		DBG("---------------- Transportation Ring TRUE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclTransportationRing));
        poChar->IncControlCount(cclTransportationRing);
    }
    else
    {
        //		DBG("---------------- Transportation Ring FALSE: %u, ref count : %d", poChar->GetObjID(), poChar->GetControlCount(cclTransportationRing));
        poChar->DecControlCount(cclTransportationRing);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_214(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;

    CAuraInstance* poAura = NULL;
    CSpell* poSpell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    if (poChar->GetMp() * nParam2 / nParam1 >= poAura->GetEffectValue())
    {
        poChar->ChangeMp(poAura->GetTotalEffectValue() * nParam1 / nParam2);
        poAura->SetTotalEffectValue(0);
    }
    else
    {
        poAura->SetTotalEffectValue(-poAura->GetTotalEffectValue() - poChar->GetMp() * nParam2 / nParam1);
        poChar->ChangeMp(-poChar->GetAttr(catMaxMp));
    }


    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_215(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CSpell* subspell = NULL;
    CCharacter* target = NULL;
    INT32 nMoveTimer = 0;
    INT32 nMoveBeginFrm = 0;
    INT32 nMoveElapseFrm = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    {
        INT32 nOffsetGridX = g_Abs(spell->GetScanner().GetTargetX() / GRID_WIDTH - target->GetX() / GRID_WIDTH);
        INT32 nOffsetGridY = g_Abs(spell->GetScanner().GetTargetY() / GRID_WIDTH - target->GetY() / GRID_WIDTH);

        PROCESS_SUCCESS(g_Min(nOffsetGridX, nOffsetGridY) > 2);
    }

    if (target->GetMoveState() == CMS_RUN)
    {
        INT32 nIndex = 0;
        INT32 nRandomNum = 0;

        nMoveTimer = target->GetMoveTimer();
        nMoveBeginFrm = nMoveTimer - target->GetAttr(catRunSpeed);
        nMoveElapseFrm = CGameWorld::GetInstance().GetGameFrame() - nMoveBeginFrm;

        nIndex = sizeof(g_GWConfig.MagHitRateToRunningTarget) / sizeof(INT32) * nMoveElapseFrm / target->GetAttr(catRunSpeed);
        LOG_PROCESS_ERROR(nIndex < sizeof(g_GWConfig.MagHitRateToRunningTarget) / sizeof(INT32));

        nRandomNum = g_Random(0, RATE_MAX);

        PROCESS_SUCCESS(nRandomNum >= g_GWConfig.MagHitRateToRunningTarget[nIndex]);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_216(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRole* poRole = NULL;
    CItemBag* poBag = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    poBag = poRole->GetItemBag();
    LOG_PROCESS_ERROR(poBag);

    nRetCode = poBag->LoseItemDur(CONSLOT_EQUIPPED, nParam1, nParam2, nParam3, ITEMLOG_SPELL);
    PROCESS_SUCCESS(!nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_217(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRole* poRole = NULL;
    INT32 nPetCount = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = (CRole*)poChar;

    nPetCount = poRole->GetPetCount();

    if (nParam2 > 0)
    {
        nRetCode = poRole->ClearPet(nParam1);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = poRole->ClearPet();
        LOG_PROCESS_ERROR(nRetCode);
    }


    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_218(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    for (INT32 i = qssSlotBegin; i < qssSlotCount; i++)
    {
        CSpellSlot* poSlot = poChar->GetSpellSlot(i);
        if (poSlot == NULL)
            continue;

        if (poSlot->GetMainSpellID() == spell->GetID())
            continue;

        CSpellSet* poSet = poChar->GetSpellBook().GetSpellSet(poSlot->GetMainSpellID());
        if (poSet == NULL)
            continue;

        CSpell* poSpell = CSpellConfig::GetInstance().FindSpellTemplate(poSet->GetID(), poSet->GetLevel(), poSet->GetRuneSlot());
        if (poSpell == NULL)
            continue;

        INT32 nCoolDownFrm = 0;

        poChar->ResetCooldown(MakeCoolDownID(COOLDOWN_SPELL, poSet->GetID()));

        {
            const SPELL_ENTRY* inf = poSpell->GetData();

            for (INT32 i = 0; i < inf->m_nCdGrpCount; i++)
            {
                poChar->ResetCooldown(MakeCoolDownID(COOLDOWN_GROUP, inf->m_CdGrp[i].m_nCdGrpID));
            }
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_219(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_SUCCESS(poChar->GetAttr(catPHitCountDown) - 1 != poChar->GetAttr(catPHitNum));

    return mrcTrue;
Exit1:
    poChar->DoSyncAttrChange(catPHitNum);
    poChar->DoSyncAttrChange(catPHitCountDown);

    //	CRI("!!! ModifierChar_219: catPHitNum: [%d], catPHitCountDown: [%d], name: [%s]", poChar->GetAttr(catPHitNum), poChar->GetAttr(catPHitCountDown), poChar->GetName());

    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_220(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellInstance* spell = NULL;
    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_SUCCESS(poChar->GetPendingItemSlot() != nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_221(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CNpc* poNpc = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_SUCCESS(poChar->GetObjType() != otNpc);

    poNpc = static_cast<CNpc*>(poChar);

    PROCESS_SUCCESS(poNpc->GetQuality() >= nParam1);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_222(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CSpell* subspell = NULL;
    CRole* poRole = NULL;
    CCharacter* poTargetChar = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    poTargetChar = CGameWorld::GetInstance().FindValidCharacter(poRole->GetClientUploadTargetID());
    PROCESS_SUCCESS(poTargetChar == NULL);

    spell->GetScanner().LockTarget(poTargetChar);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_223(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nThisConvValue = g_Abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        poChar->MpToHp(nThisConvValue);
    }
    else
    {
        poChar->HpToMp(nThisConvValue);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_224(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    if (!bIsRollingBack)
    {
        poChar->IncControlCount(cclInvisible);
    }
    else
    {
        poChar->DecControlCount(cclInvisible);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_225(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CSpell* subspell = NULL;
    CRole* poRole = NULL;
    CCharacter* poTargetChar = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->GetScanner().LockTarget(NULL);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_226(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellSet* poSet = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poSet = poChar->GetSpellBook().GetSpellSet(nParam1);
    LOG_PROCESS_ERROR(poSet);

    PROCESS_SUCCESS(poSet->GetRuneSlot() != nParam2);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_227(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CAuraInstance* poAura = NULL;
    CCharacter* poAuraCaster = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poAuraCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poAuraCaster == NULL);

    nRetCode = poChar->RotateTo(g_GetDirection(poChar->GetX(), poChar->GetY(), poAuraCaster->GetX(), poAuraCaster->GetY()), 0);
    PROCESS_SUCCESS(!nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_228(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CNpc* poNpc = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    if (poChar->GetObjType() == otNpc)
    {
        poNpc = static_cast<CNpc*>(poChar);

        PROCESS_SUCCESS(poNpc->IsPet());
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_229(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CNpc* poNpc = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    PROCESS_SUCCESS(poChar->GetObjType() != otNpc);

    poNpc = static_cast<CNpc*>(poChar);

    if (nParam2 == 0)
        PROCESS_SUCCESS(((poNpc->GetNpcData()->nSpellEffectGroup) & nParam1) == 0);
    else
        PROCESS_SUCCESS(((poNpc->GetNpcData()->nSpellEffectGroup) & nParam1) != 0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_230(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nDirection8 = 0;
    INT32 nInvertDirection8 = 0;
    INT64 llDestX = 0, llDestY = 0;
    INT32 nInfectedTargetCount = 0;
    INT32 nSpeed = CONVERT_TIME(nParam1);
    INT32 nDashGridDist = 0;
    CSpellInstance* spell = NULL;
    TARGET_NODE* pNode = NULL;
    CBullet* poBullet = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    LOG_PROCESS_ERROR(spell->GetAttr(satMaxTargetCount) <= MAX_SPELL_TARGET_COUNT);

    LOG_PROCESS_ERROR(nParam1 > 0);

    {
        CTargetScanner& Scanner = spell->GetScanner();
        CCharacter* poStopChar = NULL;
        INT64 llOriginX = poChar->GetX();
        INT64 llOriginY = poChar->GetY();

        struct InfectedTarget
        {
            CCharacter* target;
            INT64 llDestX;
            INT64 llDestY;
        };

        InfectedTarget Ift[MAX_SPELL_TARGET_COUNT] = { 0 };

        nDirection8 = ConvertToDir8(Scanner.GetDirection());
        nInvertDirection8 = NormalizeDirection8(nDirection8 - 4);

        llDestX = poChar->GetX() + g_nAdjacentX[nDirection8] * GRID_WIDTH;
        llDestY = poChar->GetY() + g_nAdjacentY[nDirection8] * GRID_WIDTH;


        Scanner.BeginTarget();

        while ((pNode = Scanner.NextTarget()) != NULL && nInfectedTargetCount < spell->GetAttr(satMaxTargetCount))
        {
            CCharacter* target = CGameWorld::GetInstance().FindValidCharacter(pNode->dwTargetID);
            if (target == NULL)
                continue;

            if (target->GetLevel() < poChar->GetLevel())
            {
                Ift[nInfectedTargetCount].target = target;
                nInfectedTargetCount++;
                continue;
            }

            poStopChar = target;
            break;
        }

        if (poStopChar != NULL)
        {
            llDestX = poStopChar->GetX() + g_nAdjacentX[nInvertDirection8] * GRID_WIDTH * (nInfectedTargetCount + 1);
            llDestY = poStopChar->GetY() + g_nAdjacentY[nInvertDirection8] * GRID_WIDTH * (nInfectedTargetCount + 1);
        }

        nDashGridDist = g_Max(g_Abs((llDestX - poChar->GetX()) / GRID_WIDTH), g_Abs((llDestY - poChar->GetY()) / GRID_WIDTH));

        if (nInfectedTargetCount > 0)
        {
            for (INT32 i = nInfectedTargetCount - 1; i >= 0; i--)
            {
                INT32 nDistGrids1 = g_Max(g_Abs((Ift[i].target->GetX() - poChar->GetX()) / GRID_WIDTH), g_Abs((Ift[i].target->GetY() - poChar->GetY()) / GRID_WIDTH));

                Ift[i].llDestX = llDestX + g_nAdjacentX[nDirection8] * GRID_WIDTH * (i + 1);
                Ift[i].llDestY = llDestY + g_nAdjacentY[nDirection8] * GRID_WIDTH * (i + 1);

                INT32 nDistGrids2 = g_Max(g_Abs((Ift[i].llDestX - poChar->GetX()) / GRID_WIDTH), g_Abs((Ift[i].llDestY - poChar->GetY()) / GRID_WIDTH));

                if (nDistGrids2 <= nDistGrids1)
                    continue;

                nRetCode = Ift[i].target->KnockBack(nSpeed, Ift[i].llDestX, Ift[i].llDestY);
                if (nRetCode)
                {
                    CAuraInstance* poAuraInst = NULL;

                    poAuraInst = Ift[i].target->GetFirstAura(nParam3);

                    if (poAuraInst == NULL);
                    else if (Ift[i].llDestX == Ift[i].target->GetX() && Ift[i].llDestY == Ift[i].target->GetY())
                        continue;

                    CAura* poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam3);
                    LOG_PROCESS_ERROR(poAuraInfo);

                    poAuraInst = Ift[i].target->AppendAura(poAuraInfo, poChar, poChar->GetObjID(), NULL, 0, spell->GetLevel() + 2, spell->GetID(), spell->GetLevel());
                    LOG_PROCESS_ERROR(poAuraInst);
                }
            }
        }

        nRetCode = poChar->Dash(nSpeed, llDestX, llDestY);
        if (!nRetCode)
        {
            if (nInfectedTargetCount <= 0)
            {
                CAuraInstance* poAuraInst = NULL;
                CAura* poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam2);
                LOG_PROCESS_ERROR(poAuraInfo);

                poAuraInst = poChar->AppendAura(poAuraInfo, poChar, poChar->GetObjID(), NULL, 0, nDashGridDist, spell->GetID(), spell->GetLevel());
                LOG_PROCESS_ERROR(poAuraInst);
            }

            PROCESS_SUCCESS(TRUE);
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_231(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 nDirection8 = 0;
    INT32 nInvertDirection8 = 0;
    INT64 llDestX = 0, llDestY = 0;
    INT32 nInfectedTargetCount = 0;
    INT32 nSpeed = CONVERT_TIME(nParam1);
    CSpellInstance* spell = NULL;
    TARGET_NODE* pNode = NULL;
    CBullet* poBullet = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    LOG_PROCESS_ERROR(spell->GetAttr(satMaxTargetCount) <= MAX_SPELL_TARGET_COUNT);

    LOG_PROCESS_ERROR(nParam1 > 0);

    {
        CTargetScanner& Scanner = spell->GetScanner();
        CCharacter* poStopChar = NULL;
        INT64 llOriginX = poChar->GetX();
        INT64 llOriginY = poChar->GetY();

        struct InfectedTarget
        {
            CCharacter* target;
            INT64 llDestX;
            INT64 llDestY;
        };

        InfectedTarget Ift[MAX_SPELL_TARGET_COUNT] = { 0 };

        nDirection8 = ConvertToDir8(Scanner.GetDirection());
        nInvertDirection8 = NormalizeDirection8(nDirection8 - 4);

        llDestX = poChar->GetX() + g_nAdjacentX[nDirection8] * GRID_WIDTH;
        llDestY = poChar->GetY() + g_nAdjacentY[nDirection8] * GRID_WIDTH;


        Scanner.BeginTarget();

        while ((pNode = Scanner.NextTarget()) != NULL && nInfectedTargetCount < spell->GetAttr(satMaxTargetCount))
        {
            CCharacter* target = CGameWorld::GetInstance().FindValidCharacter(pNode->dwTargetID);
            if (target == NULL)
                continue;

            if (target->GetLevel() < poChar->GetLevel())
            {
                Ift[nInfectedTargetCount].target = target;
                nInfectedTargetCount++;
                continue;
            }

            poStopChar = target;
            break;
        }

        if (poStopChar != NULL)
        {
            llDestX = poStopChar->GetX() + g_nAdjacentX[nInvertDirection8] * GRID_WIDTH * (nInfectedTargetCount + 1);
            llDestY = poStopChar->GetY() + g_nAdjacentY[nInvertDirection8] * GRID_WIDTH * (nInfectedTargetCount + 1);
        }

        if (nInfectedTargetCount > 0)
        {
            for (INT32 i = nInfectedTargetCount - 1; i >= 0; i--)
            {
                INT32 nDistGrids1 = g_Max(g_Abs((Ift[i].target->GetX() - poChar->GetX()) / GRID_WIDTH), g_Abs((Ift[i].target->GetY() - poChar->GetY()) / GRID_WIDTH));

                Ift[i].llDestX = llDestX + g_nAdjacentX[nDirection8] * GRID_WIDTH * (i + 1);
                Ift[i].llDestY = llDestY + g_nAdjacentY[nDirection8] * GRID_WIDTH * (i + 1);

                INT32 nDistGrids2 = g_Max(g_Abs((Ift[i].llDestX - poChar->GetX()) / GRID_WIDTH), g_Abs((Ift[i].llDestY - poChar->GetY()) / GRID_WIDTH));

                if (nDistGrids2 <= nDistGrids1)
                    continue;

                nRetCode = Ift[i].target->KnockBack(nSpeed, Ift[i].llDestX, Ift[i].llDestY);
                if (nRetCode)
                {
                    CAuraInstance* poAuraInst = Ift[i].target->GetFirstAura(nParam3);

                    if (poAuraInst != NULL)
                    {
                        Ift[i].target->ReduceAuraStack(poAuraInst, 1);
                    }
                    else
                    {
                        CAura* poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam3);
                        LOG_PROCESS_ERROR(poAuraInfo);

                        poAuraInst = Ift[i].target->AppendAura(poAuraInfo, poChar, poChar->GetObjID(), NULL, 0, spell->GetAttr(satChannelCount) - spell->GetCastSerial() + 1, spell->GetID(), spell->GetLevel());
                        LOG_PROCESS_ERROR(poAuraInst);

                    }
                }
            }
        }

        nRetCode = poChar->Dash(nSpeed, llDestX, llDestY);
        PROCESS_SUCCESS(!nRetCode);
        {
            CAuraInstance* poAuraInst = poChar->GetFirstAura(nParam2);
            if (poAuraInst != NULL)
            {
                poChar->ReduceAuraStack(poAuraInst, 1);
            }
        }

    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_232(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    CMissile* missile = NULL;
    CSpellInstance* spell = NULL;
    INT32 nMissileID = nParam1;
    INT32 nDirection = CONVERT_ANGLE(nParam2);
    INT32 nLockType = spltInvalid;
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    {
        INT64 llInitX, llInitY, llInitZ = 0;
        INT64 x[5] = { 0 }, y[5] = { 0 };
        INT32 a = 0, b = 0;
        INT32 nMaxMagAtk = poChar->GetAttr(catMaxMagAtk);
        INT32 nMagAtk = poChar->GetAttr(catMagAtk);

        const MISSILE_DATA* poMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nMissileID);
        LOG_PROCESS_ERROR(poMissileData);

        if (spell->GetAttr(satLockType) == spltDirection)
        {
            llInitX = poChar->GetX();
            llInitY = poChar->GetY();
            llInitZ = poChar->GetZ();
        }
        else
        {
            spell->GetScanner().GetTargetPt(llInitX, llInitY, llInitZ);
            llInitX = llInitX + spell->GetLengthAttr(satOffsetDistance) * g_Cos(spell->GetAttr(satOffsetDirection) + poChar->GetFaceDirection()) / SIN_COS_NUMBER;
            llInitY = llInitY + spell->GetLengthAttr(satOffsetDistance) * g_Sin(spell->GetAttr(satOffsetDirection) + poChar->GetFaceDirection()) / SIN_COS_NUMBER;;
        }

        llInitZ = llInitZ + poMissileData->m_nAttr[matHeight];

        a = round_div(nParam2 * (spell->GetLevel() + 1), 4) + nParam3;
        b = nMaxMagAtk > nMagAtk ? (nMagAtk + g_Random(nMaxMagAtk - nMagAtk + 1)) / 2 : nMagAtk / 2;

        x[0] = llInitX; y[0] = llInitY;
        x[1] = llInitX - GRID_WIDTH; y[1] = llInitY;
        x[2] = llInitX; y[2] = llInitY + GRID_WIDTH;
        x[3] = llInitX + GRID_WIDTH; y[3] = llInitY;
        x[4] = llInitX; y[4] = llInitY - GRID_WIDTH;

        for (INT32 i = 0; i < 5; i++)
        {
            missile = CGameWorld::GetInstance().NewMissile();

            if (missile != NULL)
            {
                nRetCode = missile->Init(poMissileData, poChar, spell, x[i], y[i], llInitZ, nDirection, (a + b) * GAME_FPS);
            }
            else
                LOG_CHECK_ERROR(FALSE);
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_233(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAura* poAuraInfo = NULL;
    CSpellInstance* spell = NULL;
    CAuraInstance* poAuraInstance = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam1);
    LOG_PROCESS_ERROR(poAuraInfo);

    {
        INT32 nDurationFrm = 0;

        nDurationFrm = g_Min(g_Max((600 - poChar->GetLevel() * 10) / 16, 5), nParam3);

        poAuraInstance = poChar->AppendAura(poAuraInfo, poChar, spell->GetFromObjID(), (ExtraParam*)pExParam, nDurationFrm, 0, spell->GetID(), spell->GetLevel());
        PROCESS_SUCCESS(poAuraInstance == NULL);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_234(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CNpc* poNpc = NULL;
    INT32 nSpellEffectGroup = 0;
    CSpellInstance* spell = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    poNpc = CGameWorld::GetInstance().FindNpc(spell->GetScanner().GetLockedTarget());
    LOG_PROCESS_ERROR(poNpc);

    nSpellEffectGroup = poNpc->GetNpcData()->nSpellEffectGroup;

    PROCESS_SUCCESS((nSpellEffectGroup & nParam1) == 0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_235(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRate = 0;
    CNpc* poNpc = NULL;
    CCharacter* host = NULL;
    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_SUCCESS(poChar->GetObjType() != otNpc);

    poNpc = static_cast<CNpc*>(poChar);

    if (nParam3 <= 0)
    {
        // Default code
        nRate = nParam1 * poNpc->GetPetLevel() + nParam2;
        PROCESS_SUCCESS(g_Random(0, RATE_MAX) > nRate);

        // Get the owner object here - temporarily disabled
        {
            //host = CGameWorld::GetInstance().FindValidCharacter(poChar->GetHostID());
            //if (host)
            //{
            //    // 1 300
            //    // 2 500
            //    // 3 700
            //    // 4 900
            //    // 5 1200
            //    // 6 1700
            //    // 7 2500
            //    // 8 3300
            //    // 9 5000
            //    INT32 nLuck = host->GetAttr(catLuck);
            //    switch (nLuck)
            //    {
            //    case 1: nRate += 300; break;
            //    case 2: nRate += 500; break;
            //    case 3: nRate += 700; break;
            //    case 4: nRate += 900; break;
            //    case 5: nRate += 1200; break;
            //    case 6: nRate += 1700; break;
            //    case 7: nRate += 2500; break;
            //    case 8: nRate += 3300; break;
            //    case 9: nRate += 5000; break;
            //    }
            //
            //    // Default code: spirit beast chance is 3% per level, or 21% at level 7
            //    nRate += nParam1 * poNpc->GetPetLevel() + nParam2;
            //    PROCESS_SUCCESS(g_Random(0, RATE_MAX) > nRate);
            //
            //    // Copied code triggered by the charm potion
            //    //poAura = host->GetFirstAura(45780);
            //    //if (poAura)
            //    //{
            //    //    PROCESS_SUCCESS(g_Random(0, RATE_MAX) <= 5000);
            //    //}
            //    //else
            //    //{
            //    //    // Default code: spirit beast chance is 3% per level, or 21% at level 7
            //    //    nRate = nParam1 * poNpc->GetPetLevel() + nParam2;
            //    //    PROCESS_SUCCESS(g_Random(0, RATE_MAX) > nRate);
            //    //}
            //}
            //else
            //{
            //    // Default code
            //    nRate = nParam1 * poNpc->GetPetLevel() + nParam2;
            //    PROCESS_SUCCESS(g_Random(0, RATE_MAX) > nRate);
            //}
        }
    }
    else
    {
        nRate = nParam1 * poNpc->GetPetLevel() + (nParam3 - 1) * poNpc->GetPetLevel() + nParam2;
        PROCESS_SUCCESS(g_Random(0, RATE_MAX) > nRate);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_236(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CRole* poRole = NULL;
    CScene* poScene = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poScene = poChar->GetScene();
    LOG_PROCESS_ERROR(poScene);

    PROCESS_SUCCESS(poScene->GetSceneTempID() != nParam1);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);
    poRole = static_cast<CRole*>(poChar);

    poRole->AddExp(nParam2 * poRole->GetLevel() + nParam3, FALSE, FALSE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_237(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;

    INT64 llTargetX = 0, llTargetY = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    GetOffsetedPos(target->GetX(), target->GetY(), target->GetFaceDirection(), nParam2, nParam1, llTargetX, llTargetY);

    nRetCode = poChar->JumpTo(llTargetX, llTargetY, target->GetZ(), poChar->GetFaceDirection());
    LOG_PROCESS_ERROR(nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_238(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;
    INT32 nFaceAngleDiff = 0;
    INT32 nTargetDirDiff = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    if (nParam1 == 0)
        return mrcTrue;

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    nFaceAngleDiff = NormalizeDirection(target->GetFaceDirection() - poChar->GetFaceDirection());
    nTargetDirDiff = NormalizeDirection(poChar->GetFaceDirection() - g_GetDirection(poChar->GetX(), poChar->GetY(), target->GetX(), target->GetY()));

    PROCESS_ERROR(g_Cos(nTargetDirDiff) >= 0);

    if (nParam1 == 1 && g_Cos(nFaceAngleDiff) < 0)
        return mrcTrue;

    if (nParam1 == 2 && g_Cos(nFaceAngleDiff) >= 0)
        return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_239(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    INT32 eAttackType = 0;
    INT32 nEffectValue = 0;
    CAuraInstance* poAura = NULL;
    CCharacter* poCaster = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poCaster == NULL);

    eAttackType = poAura->GetAttr(aatAttackType);
    LOG_PROCESS_ERROR(eAttackType > spatInvalid && eAttackType < spatTotal);

    nEffectValue = poAura->GetEffectValue() + g_Max(poCaster->GetAttr(s_MaxAtkTable[eAttackType]) + nParam3, 0) * nParam2 / nParam1;

    poAura->SetEffectValue(nEffectValue);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_240(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    PROCESS_SUCCESS(poChar->GetObjType() != otRole);

    poRole = static_cast<CRole*>(poChar);

    PROCESS_SUCCESS(poRole->GetPkMode() != rpmSlayer);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_241(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;
    CNpc* poNpc = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    PROCESS_SUCCESS(target->GetObjType() != otNpc);

    poNpc = static_cast<CNpc*>(target);

    PROCESS_SUCCESS(!poNpc->IsPet());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_242(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CSpell* subspell = NULL;
    CCharacter* target = NULL;
    INT32 nMoveTimer = 0;
    INT32 nMoveBeginFrm = 0;
    INT32 nMoveElapseFrm = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    //	{
    //		INT32 nOffsetGridX = g_Abs(spell->GetScanner().GetTargetX() / GRID_WIDTH - target->GetX() / GRID_WIDTH);
    //		INT32 nOffsetGridY = g_Abs(spell->GetScanner().GetTargetY() / GRID_WIDTH - target->GetY() / GRID_WIDTH);
    //
    //		PROCESS_SUCCESS(g_Min(nOffsetGridX, nOffsetGridY) >= 2);
    //	}

    DBG("242 move state: %d", target->GetMoveState());


    if (target->GetMoveState() == CMS_RUN)
    {
        INT32 nIndex = 0;
        INT32 nRandomNum = 0;

        nMoveTimer = target->GetMoveTimer();
        nMoveBeginFrm = nMoveTimer - target->GetAttr(catRunSpeed);
        nMoveElapseFrm = CGameWorld::GetInstance().GetGameFrame() - nMoveBeginFrm;

        DBG("run elapse frm: %d", nMoveElapseFrm);

        nIndex = sizeof(g_GWConfig.PhyHitRateToRunningTarget) / sizeof(INT32) * nMoveElapseFrm / target->GetAttr(catRunSpeed);
        LOG_PROCESS_ERROR(nIndex < sizeof(g_GWConfig.PhyHitRateToRunningTarget) / sizeof(INT32));

        nRandomNum = g_Random(0, RATE_MAX);

        PROCESS_SUCCESS(nRandomNum >= g_GWConfig.PhyHitRateToRunningTarget[nIndex]);
    }
    else if (target->GetMoveState() == CMS_WALK)
    {
        INT32 nIndex = 0;
        INT32 nRandomNum = 0;

        nMoveTimer = target->GetMoveTimer();
        nMoveBeginFrm = nMoveTimer - target->GetAttr(catWalkSpeed);
        nMoveElapseFrm = CGameWorld::GetInstance().GetGameFrame() - nMoveBeginFrm;

        DBG("walk elapse frm: %d", nMoveElapseFrm);

        nIndex = sizeof(g_GWConfig.PhyHitRateToWalkingTarget) / sizeof(INT32) * nMoveElapseFrm / target->GetAttr(catRunSpeed);
        LOG_PROCESS_ERROR(nIndex < sizeof(g_GWConfig.PhyHitRateToWalkingTarget) / sizeof(INT32));

        nRandomNum = g_Random(0, RATE_MAX);

        PROCESS_SUCCESS(nRandomNum >= g_GWConfig.PhyHitRateToWalkingTarget[nIndex]);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_243(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* target = NULL;
    CSpellInstance* spell = NULL;
    CAura* poAuraInfo = NULL;
    CAuraInstance* poAuraInstance = NULL;
    TARGET_NODE* pTargetNode = NULL;

    INT32 nCount = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAuraInfo = CSpellConfig::GetInstance().FindAuraTemplate(nParam1);
    LOG_PROCESS_ERROR(poAuraInfo);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    spell->GetScanner().BeginTarget();

    while ((pTargetNode = spell->GetScanner().NextTarget()) != NULL)
    {
        target = CGameWorld::GetInstance().FindValidCharacter(pTargetNode->dwTargetID);
        if (target == NULL)
            continue;

        if (nCount >= spell->GetAttr(satMaxTargetCount))
            break;

        INT32 nNewDurationFrm = 0;
        INT32 nB = 0, nC = 0;
        INT32 nTaoAtk = poChar->GetAttr(catTaoAtk);
        INT32 nMaxTaoAtk = poChar->GetAttr(catMaxTaoAtk);

        nB = round_div((6 + 2 * spell->GetLevel()) * nParam2, 12) + g_Random(spell->GetAttr(satDefaultValue), spell->GetAttr(satDefaultValueMax));

        if (nTaoAtk > 0)
        {
            if (nMaxTaoAtk > nTaoAtk)
                nC = g_Random(nTaoAtk, nMaxTaoAtk + 1);
            else
                nC = nTaoAtk;
        }

        nNewDurationFrm = GAME_FPS * (nB + nParam3 * nC);

        if (poChar->GetLevel() - 1 + g_Random(4) > target->GetLevel())
        {
            poAuraInstance = target->AppendAura(poAuraInfo, poChar, spell->GetFromObjID(), (ExtraParam*)pExParam, nNewDurationFrm, 0, spell->GetID(), spell->GetLevel());
            if (poAuraInstance)
                nCount++;
        }
    }

    if (nCount > 0)
        return mrcTrue;

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_244(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    spell->GetScanner().SetTargetPt(target->GetX(), target->GetY(), target->GetZ());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_245(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CNpc* poNpc = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_SUCCESS(poChar->GetObjType() != otNpc);

    poNpc = static_cast<CNpc*>(poChar);

    PROCESS_SUCCESS(!poNpc->GetNpcData()->bIgnoreInvisible);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_246(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    nRetCode = poRole->ClearRandomPet();
    LOG_PROCESS_ERROR(nRetCode);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_247(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    if (!bIsRollingBack)
        poRole->Hide(TRUE);
    else
        poRole->Hide(FALSE);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_248(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpell* poSpell = NULL;
    INT32 nLevel = nParam1;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poSpell = poChar->GetEquippingSpell();
    PROCESS_ERROR(poSpell);

    return poSpell->GetLevel() >= nLevel;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_249(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(spell->GetScanner().GetLockedTarget());
    PROCESS_SUCCESS(target == NULL);

    PROCESS_SUCCESS(target->GetLevel() >= poChar->GetLevel());

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_250(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CCharacter* target = NULL;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    target = CGameWorld::GetInstance().FindValidCharacter(poRole->GetClientUploadTargetID());
    PROCESS_SUCCESS(target == NULL);

    nRetCode = poRole->CheckTargetPos(poRole, spell, target->GetX(), target->GetY(), target->GetZ(), target->GetRadius(), spell->GetAttr(satTolerantGrids));
    PROCESS_SUCCESS(nRetCode != errSuccess);

    nRetCode = poRole->CheckTarget(spell, target);
    PROCESS_SUCCESS(nRetCode != errSuccess);

    spell->GetScanner().LockTarget(target);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

// Custom functions; some implementations are unknown and left empty for now
INT32 ModifierChar_251(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    CCharacter* poCaster = NULL;
    CScene* poScene = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    poCaster = CGameWorld::GetInstance().FindValidCharacter(poAura->GetCasterID());
    PROCESS_SUCCESS(poCaster == NULL);

    if (poChar->GetScene() == poCaster->GetScene())
    {
        if (g_GetDistance3D(poChar->GetX(), poChar->GetY(), poChar->GetZ(), poCaster->GetX(), poCaster->GetY(), poCaster->GetZ()) < (nParam1 << 10) * (nParam1 << 10))
            return mrcTrue;
    }

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}
INT32 ModifierChar_252(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 nRetCode = 0;
    CSpellInstance* spell = NULL;
    CMissile* missile = NULL;
    INT32 nMissileID = nParam1;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    {
        INT64 llInitX, llInitY, llInitZ = 0;
        INT32 nOffsetDirection = 0;

        const MISSILE_DATA* poMissileData = CDataManager<MISSILE_DATA>::GetInstance().GetData(nMissileID);
        LOG_PROCESS_ERROR(poMissileData);

        missile = CGameWorld::GetInstance().NewMissile();
        LOG_PROCESS_ERROR(missile);

        if (spell->GetAttr(satLockType) == spltDirection)
        {
            llInitX = poChar->GetX();
            llInitY = poChar->GetY();
            llInitZ = poChar->GetZ();
        }
        else
        {
            spell->GetScanner().GetTargetPt(llInitX, llInitY, llInitZ);
            llInitX = poChar->GetX() + (INT64)spell->GetLengthAttr(satOffsetDistance) * g_Cos(poChar->GetFaceDirection() + spell->GetAttr(satOffsetDirection)) / SIN_COS_NUMBER;
            llInitY = poChar->GetY() + (INT64)spell->GetLengthAttr(satOffsetDistance) * g_Sin(poChar->GetFaceDirection() + spell->GetAttr(satOffsetDirection)) / SIN_COS_NUMBER;
        }

        GetOffsetedPos(llInitX, llInitY, 0, nParam2, nParam3, llInitX, llInitY);
        llInitZ = llInitZ + poMissileData->m_nAttr[matHeight];

        nRetCode = missile->Init(poMissileData, poChar, spell, llInitX, llInitY, llInitZ, spell->GetScanner().GetDirection(), 0);
        LOG_PROCESS_ERROR(nRetCode);
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}
INT32 ModifierChar_253(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* aura = NULL;
    INT32 v10 = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    for (INT32 i = auetNone; i < auetTotal; i++)
    {
        aura = poChar->GetAuraList().BeginAura(i);

        while ((aura = poChar->GetAuraList().NextAura(i, aura)) != NULL)
        {
            if (aura->GetID() == nParam1 || aura->GetAttr(aatType) == nParam2)
            {
                aura->ResetFrm(aura->GetRemainFrm() - 16 * nParam3 / 1000, 0, TRUE);
            }
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}
INT32 ModifierChar_254(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 v5 = 0;
    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    v5 = (nParam1 + 10000) * poAura->GetTotalEffectValue();

    poAura->SetTotalEffectValue(v5 / 10000);
    PROCESS_SUCCESS(v5 / 10000 != 0);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}
INT32 ModifierChar_255(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    if (poAura->GetID() == nParam1 || (poAura->GetAttr(aatGroup) == nParam2) && nParam2 != 0)
        return mrcTrue;

Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}
INT32 ModifierChar_256(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CAuraInstance* poAura = NULL;
    INT32 v7 = 0;
    INT32 v4 = 0;
    INT32 v6 = 0;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);

    poAura = poChar->GetPendingAura();
    LOG_PROCESS_ERROR(poAura);

    v7 = poChar->GetAttr(catMaxMp);
    v4 = poAura->GetAttr(aatEffectPct) * v7;
    v6 = poAura->GetAttr(aatEffectValue) + v4 / 10000;

    poChar->ChangeMp(v6);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}
INT32 ModifierChar_257(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT32 v7 = g_Abs(nParam1);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    //LOG_PROCESS_ERROR(!bIsRollingBack);    // Not present in the IDA output; added locally

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    if (!bIsRollingBack)
    {
        //poChar->SetSceneExpRate(v7, 0);    // CRole::SetSceneExpRate is not present in this code
    }
    else
    {
        //poChar->SetSceneExpRate(v7, nParam2);    // CRole::SetSceneExpRate is not present in this code
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}
INT32 ModifierChar_258(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CRole* poRole = NULL;
    CNpc* poNpc = NULL;
    CAuraInstance* poAura = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);    // Not present in the IDA output; added locally

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);
    for (INT32 i = 0; i < poRole->GetPetCount(); i++)
    {
        poNpc = CGameWorld::GetInstance().FindNpc(poRole->GetPetNpcID(i));

        if (poNpc)
        {
            if (nParam1 > 0)
            {
                poAura = poNpc->GetFirstAura(nParam1);
                PROCESS_SUCCESS(poAura == NULL);

                poNpc->RemoveAura(poAura, TRUE);
            }
            if (nParam2 > 0)
            {
                poNpc->RemoveAuraByType(nParam2);
            }
            if (nParam3 > 0)
            {
                poNpc->RemoveAuraBySubType(nParam3);
            }
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}
INT32 ModifierChar_259(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    INT64 v17 = 0;
    INT64 v19 = -1;
    INT64 v18 = nParam1 << 10;

    CSpellInstance* spell = NULL;
    CRole* poRole = NULL;
    CNpc* poNpc = NULL;
    CNpc* pov15 = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);    // Not present in the IDA output; added locally

    spell = poChar->GetPendingSpell();
    LOG_PROCESS_ERROR(spell);

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);
    for (INT32 i = 0; i < poRole->GetPetCount(); i++)
    {
        poNpc = CGameWorld::GetInstance().FindNpc(poRole->GetPetNpcID(i));
        if (poNpc)
        {
            if (poNpc->GetScene() == poRole->GetScene())
            {
                v17 = g_GetDistance3D(poRole->GetX(), poRole->GetY(), poRole->GetZ(), poNpc->GetX(), poNpc->GetY(), poNpc->GetZ());

                // Unknown enum value for -1; follow the IDA output for now
                if (v17 < -1 && v17 < v18 * v18)
                {
                    v19 = v17;
                    pov15 = poNpc;
                }
            }
        }
    }

    PROCESS_SUCCESS(pov15 == NULL);

    spell->GetScanner().LockTarget(pov15);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

// Contains an unknown function
INT32 ModifierChar_260(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);    // Not present in the IDA output; added locally

    //poChar->未知函数(nParam1); // Unknown function placeholder

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

// Contains an unknown function
INT32 ModifierChar_261(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);    // Not present in the IDA output; added locally

    for (INT32 i = 0; i < 3; ++i)
    {
        if ((nParam1 & (1 << i)) != 0)
        {
            //poChar->未知函数(i); // Unknown function placeholder
        }
    }

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_262(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CSpellSet* poSet = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);    // Not present in the IDA output; added locally

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poSet = poChar->GetSpellBook().GetSpellSet(nParam1);
    LOG_PROCESS_ERROR(poSet);

    PROCESS_SUCCESS(poSet->GetRuneSlot() != nParam2);

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}

INT32 ModifierChar_263(void* poTarget, BOOL bIsRollingBack, INT32 nParam1, INT32 nParam2, INT32 nParam3, void* pExParam)
{
    //WRN("Execute Function - %s", __FUNCTION__);

    CItemBag* poBag = NULL;
    CRole* poRole = NULL;

    CCharacter* poChar = static_cast<CCharacter*>(poTarget);
    LOG_PROCESS_ERROR(poChar);

    LOG_PROCESS_ERROR(!bIsRollingBack);    // Not present in the IDA output; added locally

    LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);

    poRole = static_cast<CRole*>(poChar);

    poBag = poRole->GetItemBag();
    LOG_PROCESS_ERROR(poBag);

    poBag->CheckWeight();

    return mrcTrue;
Exit1:
    return mrcFalse;
Exit0:
    return mrcError;
}



ModifierFunc g_CharModifier[cmiCount] =
{
    { NULL, FALSE },
    { ModifierChar_A, TRUE },	  // Change walk or run speed; parameter 1 specifies the change type (absolute value)
    { ModifierChar_B, TRUE },	  // Change selected attributes by an absolute value
    { ModifierChar_003, TRUE },	  // Change selected attributes by the percentage returned by the aura’s GetEffectPct()
    { ModifierChar_004, TRUE },	  // Change walk or run speed; parameter 1 specifies the change type (percentage from the aura’s GetEffectPct())
    { ModifierChar_005, FALSE },  // Apply damage to targets affected by the current spell
    { ModifierChar_006, FALSE },  // Apply damage to the area affected by the current spell
    { ModifierChar_007, FALSE },
    { ModifierChar_008, FALSE },
    { ModifierChar_009, FALSE },
    { ModifierChar_010, FALSE },
    { ModifierChar_011, TRUE },
    { ModifierChar_012, TRUE },
    { ModifierChar_013, FALSE },
    { ModifierChar_014, TRUE },
    { ModifierChar_015, FALSE },
    { ModifierChar_016, FALSE },
    { ModifierChar_017, TRUE },
    { ModifierChar_018, TRUE },
    { ModifierChar_019, TRUE },
    { ModifierChar_020, TRUE },
    { ModifierChar_021, FALSE },
    { ModifierChar_022, FALSE },
    { ModifierChar_023, FALSE },
    { ModifierChar_024, TRUE },
    { ModifierChar_025, FALSE },
    { ModifierChar_026, TRUE },
    { ModifierChar_027, FALSE },
    { ModifierChar_028, FALSE },
    { ModifierChar_029, TRUE },
    { ModifierChar_030, FALSE },
    { ModifierChar_031, FALSE },
    { ModifierChar_032, FALSE },
    { ModifierChar_033, FALSE },
    { ModifierChar_034, FALSE },
    { ModifierChar_035, FALSE },
    { ModifierChar_036, FALSE },
    { ModifierChar_037, FALSE },
    { ModifierChar_038, TRUE },
    { ModifierChar_039, FALSE },
    { ModifierChar_040, FALSE },
    { ModifierChar_041, FALSE },
    { ModifierChar_042, FALSE },
    { ModifierChar_043, FALSE },
    { ModifierChar_044, FALSE },
    { ModifierChar_045, FALSE },
    { ModifierChar_046, FALSE },
    { ModifierChar_047, FALSE },
    { ModifierChar_048, FALSE },
    { ModifierChar_049, FALSE },
    { ModifierChar_050, FALSE },
    { ModifierChar_051, FALSE },
    { ModifierChar_052, FALSE },
    { ModifierChar_053, FALSE },
    { ModifierChar_054, FALSE },
    { ModifierChar_055, FALSE },
    { ModifierChar_056, FALSE },
    { ModifierChar_057, FALSE },
    { ModifierChar_058, FALSE },
    { ModifierChar_059, TRUE },
    { ModifierChar_060, FALSE },
    { ModifierChar_061, FALSE },
    { ModifierChar_062, FALSE },
    { ModifierChar_063, FALSE },
    { ModifierChar_064, FALSE },
    { ModifierChar_065, FALSE },
    { ModifierChar_066, FALSE },
    { ModifierChar_067, FALSE },
    { ModifierChar_068, FALSE },
    { ModifierChar_069, FALSE },
    { ModifierChar_070, FALSE },
    { ModifierChar_071, TRUE },
    { ModifierChar_072, FALSE },
    { ModifierChar_073, FALSE },
    { ModifierChar_074, FALSE },
    { ModifierChar_075, FALSE },
    { ModifierChar_076, FALSE },
    { ModifierChar_077, FALSE },
    { ModifierChar_078, FALSE },
    { ModifierChar_079, FALSE },
    { ModifierChar_080, FALSE },
    { ModifierChar_081, FALSE },
    { ModifierChar_082, FALSE },
    { ModifierChar_083, FALSE },
    { ModifierChar_084, TRUE },
    { ModifierChar_085, FALSE },
    { ModifierChar_086, TRUE },
    { ModifierChar_087, FALSE },
    { ModifierChar_088, FALSE },
    { ModifierChar_089, FALSE },
    { ModifierChar_090, FALSE },
    { ModifierChar_091, FALSE },
    { ModifierChar_092, FALSE },
    { ModifierChar_093, FALSE },
    { ModifierChar_094, FALSE },
    { ModifierChar_095, FALSE },
    { ModifierChar_096, TRUE },
    { ModifierChar_097, FALSE },
    { ModifierChar_098, FALSE },
    { ModifierChar_099, FALSE },
    { ModifierChar_100, FALSE },
    { ModifierChar_101, TRUE },
    { ModifierChar_102, FALSE },
    { ModifierChar_103, FALSE },
    { ModifierChar_104, FALSE },
    { ModifierChar_105, FALSE },
    { ModifierChar_106, FALSE },
    { ModifierChar_107, FALSE },
    { ModifierChar_108, FALSE },
    { ModifierChar_109, FALSE },
    { ModifierChar_110, FALSE },
    { ModifierChar_111, TRUE },
    { ModifierChar_112, TRUE },
    { ModifierChar_113, FALSE },
    { ModifierChar_114, FALSE },
    { ModifierChar_115, FALSE },
    { ModifierChar_116, FALSE },
    { ModifierChar_117, FALSE },
    { ModifierChar_118, FALSE },
    { ModifierChar_119, FALSE },
    { ModifierChar_120, TRUE },
    { ModifierChar_121, FALSE },
    { ModifierChar_122, FALSE },
    { ModifierChar_123, FALSE },
    { ModifierChar_124, TRUE },
    { ModifierChar_125, FALSE },
    { ModifierChar_126, TRUE },
    { ModifierChar_127, FALSE },
    { ModifierChar_128, FALSE },
    { ModifierChar_129, FALSE },
    { ModifierChar_130, FALSE },
    { ModifierChar_131, FALSE },
    { ModifierChar_132, FALSE },
    { ModifierChar_133, FALSE },
    { ModifierChar_134, TRUE },
    { ModifierChar_135, FALSE },
    { ModifierChar_136, TRUE },
    { ModifierChar_137, FALSE },
    { ModifierChar_138, FALSE },
    { ModifierChar_139, FALSE },
    { ModifierChar_140, FALSE },
    { ModifierChar_141, FALSE },
    { ModifierChar_142, FALSE },
    { ModifierChar_143, TRUE },
    { ModifierChar_144, TRUE },
    { ModifierChar_145, TRUE },
    { ModifierChar_146, TRUE },
    { ModifierChar_147, FALSE },
    { ModifierChar_148, FALSE },
    { ModifierChar_149, FALSE },
    { ModifierChar_150, FALSE },
    { ModifierChar_151, FALSE },
    { ModifierChar_152, FALSE },
    { ModifierChar_153, FALSE },
    { ModifierChar_154, FALSE },
    { ModifierChar_155, FALSE },
    { ModifierChar_156, FALSE },
    { ModifierChar_157, FALSE },
    { ModifierChar_158, FALSE },
    { ModifierChar_159, FALSE },
    { ModifierChar_160, FALSE },
    { ModifierChar_161, FALSE },
    { ModifierChar_162, FALSE },
    { ModifierChar_163, FALSE },
    { ModifierChar_164, FALSE },
    { ModifierChar_165, FALSE },
    { ModifierChar_166, TRUE },
    { ModifierChar_167, FALSE },
    { ModifierChar_168, FALSE },
    { ModifierChar_169, FALSE },
    { ModifierChar_170, FALSE },
    { ModifierChar_171, FALSE },
    { ModifierChar_172, FALSE },
    { ModifierChar_173, FALSE },
    { ModifierChar_174, FALSE },
    { ModifierChar_175, FALSE },
    { ModifierChar_176, FALSE },
    { ModifierChar_177, FALSE },
    { ModifierChar_178, FALSE },
    { ModifierChar_179, FALSE },
    { ModifierChar_180, FALSE },
    { ModifierChar_181, FALSE },
    { ModifierChar_182, FALSE },
    { ModifierChar_183, FALSE },
    { ModifierChar_184, FALSE },
    { ModifierChar_185, FALSE },
    { ModifierChar_186, FALSE },
    { ModifierChar_187, FALSE },
    { ModifierChar_188, FALSE },
    { ModifierChar_189, TRUE },
    { ModifierChar_190, FALSE },
    { ModifierChar_191, FALSE },
    { ModifierChar_192, FALSE },
    { ModifierChar_193, FALSE },
    { ModifierChar_194, FALSE },
    { ModifierChar_195, FALSE },
    { ModifierChar_196, FALSE },
    { ModifierChar_197, FALSE },
    { ModifierChar_198, FALSE },
    { ModifierChar_199, FALSE },
    { ModifierChar_200, FALSE },
    { ModifierChar_201, TRUE },
    { ModifierChar_202, TRUE },
    { ModifierChar_203, FALSE },
    { ModifierChar_204, TRUE },
    { ModifierChar_205, FALSE },
    { ModifierChar_206, FALSE },
    { ModifierChar_207, TRUE },
    { ModifierChar_208, FALSE },
    { ModifierChar_209, FALSE },
    { ModifierChar_210, TRUE },
    { ModifierChar_211, TRUE },
    { ModifierChar_212, TRUE },
    { ModifierChar_213, TRUE },
    { ModifierChar_214, FALSE },
    { ModifierChar_215, FALSE },
    { ModifierChar_216, FALSE },
    { ModifierChar_217, FALSE },
    { ModifierChar_218, FALSE },
    { ModifierChar_219, FALSE },
    { ModifierChar_220, FALSE },
    { ModifierChar_221, FALSE },
    { ModifierChar_222, FALSE },
    { ModifierChar_223, TRUE },
    { ModifierChar_224, TRUE },
    { ModifierChar_225, FALSE },
    { ModifierChar_226, FALSE },
    { ModifierChar_227, FALSE },
    { ModifierChar_228, FALSE },
    { ModifierChar_229, FALSE },
    { ModifierChar_230, FALSE },
    { ModifierChar_231, FALSE },
    { ModifierChar_232, FALSE },
    { ModifierChar_233, FALSE },
    { ModifierChar_234, FALSE },
    { ModifierChar_235, FALSE },
    { ModifierChar_236, FALSE },
    { ModifierChar_237, FALSE },
    { ModifierChar_238, FALSE },
    { ModifierChar_239, FALSE },
    { ModifierChar_240, FALSE },
    { ModifierChar_241, FALSE },
    { ModifierChar_242, FALSE },
    { ModifierChar_243, FALSE },
    { ModifierChar_244, FALSE },
    { ModifierChar_245, FALSE },
    { ModifierChar_246, FALSE },
    { ModifierChar_247, TRUE },
    { ModifierChar_248, FALSE },
    { ModifierChar_249, FALSE },
    { ModifierChar_250, FALSE },

    // Additional functions
    { ModifierChar_251, FALSE },
    { ModifierChar_252, FALSE },
    { ModifierChar_253, FALSE },
    { ModifierChar_254, FALSE },
    { ModifierChar_255, FALSE },
    { ModifierChar_256, FALSE },
    { ModifierChar_257, TRUE },
    { ModifierChar_258, FALSE },
    { ModifierChar_259, FALSE },
    { ModifierChar_260, FALSE },
    { ModifierChar_261, FALSE },
    { ModifierChar_262, FALSE },
    { ModifierChar_263, FALSE },
};

