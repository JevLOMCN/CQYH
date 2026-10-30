#include "stdafx.h"
#include "bt_action.h"

#include "lua_script.h"
#include "script_mgr.h"

#include "guild_config.h"

#include "error_def.h"
#include "string_def.h"
#include "ai_processor.h"	// The actual behavior is implemented in CAIProcessor
#include "game_world.h"		// FindValidCharacter, GetString
#include "bt_manager.h"     // ms_ActionFuncList
#include "spell_config.h"
#include "chat_def.h"       // cctScene
#include "item_bag.h"
#include "scene_file_mgr.h"
#include "guild_mgr.h"
#include "mail_template_data.h"
#include "dump.h"
#include "npc_data.h"
#include "guild_building_data.h"
#include "level_data.h"
#include "ai_data.h"
#include "scene_def.h"
#include "sabac_mgr.h"
#include "pet_data.h"

#ifdef _ROBOT
#include "robot_message_handler.h"
#else
#include "message_handler.h"
#endif

inline INT32 _CheckOperator(INT32 nOperator, INT64 nLeftValue, INT64 nRightValue)
{
	switch (nOperator)
	{
	case boLT:       // less than
		if (nLeftValue < nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boLTE:      // less than or equal to
		if (nLeftValue <= nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boGT:       // greater than
		if (nLeftValue > nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boGTE:      // greater than or equal to
		if (nLeftValue >= nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boEQ:       // equal to
		if (nLeftValue == nRightValue)
			return brvSuccess;
		else
			return brvFail;
	case boNEQ:      // not equal to
		if (nLeftValue != nRightValue)
			return brvSuccess;
		else
			return brvFail;
	default:
		return brvError;
	}
}

CCharacter* _GetTargetChar(BTCustomCtx* pCtx, INT32 nTargetType)
{
	CCharacter* poTargetChar = NULL;

	switch (nTargetType)
	{
	case tarSelf:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		poTargetChar = pCtx->poChar;
	}
		break;
	case tarAITarget:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetTargetID());
	}
		break;
	case tarHost:
	{
		if (pCtx->eOwnerType == botAI)
		{
			poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetHostID());
		}
	}
		break;
	case tarNearestEnemy:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetNearestEnemyID());
	}
		break;
	case tarFarthestEnemy:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetFarthestEnemyID());
	}
		break;
	case tarCastingEnemy:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetCastingEnemyID());
	}
		break;
	case tarCorpse:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetCorpseCharID());
	}
		break;
	case tarAuraChar:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetAuraCharID());
	}
		break;
	case tarNearestFriend:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetNearestFriendID());
	}
		break;
	case tarSignalIssuer:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetSignalIssuerID());
	}
		break;
	case tarSignalIssuerTarget:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetSignalIssuerID());
		LOG_PROCESS_ERROR(poTargetChar);
		LOG_PROCESS_ERROR(poTargetChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(poTargetChar->GetAIProcessor()->GetTargetID());
	}
		break;
	case tarRole:
	{
		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->dwRoleID);
	}
		break;
	case tarRoleKiller:
		poTargetChar = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poTargetChar)
		{
			if (poTargetChar->IsDead())
				poTargetChar = CGameWorld::GetInstance().FindValidRole(poTargetChar->GetLastCasterID());
			else
				poTargetChar = NULL;
		}
		break;
	case tarRolePet1:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 0 < poRole->GetPetCount())
			poTargetChar = CGameWorld::GetInstance().FindNpc(poRole->GetPetNpcID(0));

		break;
	}
	case tarRolePet2:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 1 < poRole->GetPetCount())
			poTargetChar = CGameWorld::GetInstance().FindNpc(poRole->GetPetNpcID(1));

		break;
	}
	case tarRolePet3:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 2 < poRole->GetPetCount())
			poTargetChar = CGameWorld::GetInstance().FindNpc(poRole->GetPetNpcID(2));

		break;
	}
	case tarRolePet4:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 3 < poRole->GetPetCount())
			poTargetChar = CGameWorld::GetInstance().FindNpc(poRole->GetPetNpcID(3));

		break;
	}
	case tarRolePet5:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 4 < poRole->GetPetCount())
			poTargetChar = CGameWorld::GetInstance().FindNpc(poRole->GetPetNpcID(4));

		break;
	}
	case tarRolePet6:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 5 < poRole->GetPetCount()) 
			poTargetChar = CGameWorld::GetInstance().FindNpc(poRole->GetPetNpcID(5));

		break;
	}
	case tarNearestEnemyRole:
	{
		LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
		LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

		poTargetChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetNearestEnemyRoleID());
	}
		break;
	case tarDirection:
	case tarPosition:
	case tarHome:
		break;
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return poTargetChar;
Exit0:
	return NULL;
}

BOOL _CastSpell(CCharacter* poCaster, INT32 nSpellID, CCharacter* poTarget, INT64 llTargetX, INT64 llTargetY, INT64 llTargetZ)
{
	INT32 nRetCode = 0;

	nRetCode = poCaster->CastSpell(nSpellID, poTarget, llTargetX, llTargetY, llTargetZ);

#if defined(_DEBUG) & !defined(_FINAL)
	if (g_GWConfig.DebugOption.debugai)
	{
		char szMessage[256] = { 0 };
		if (g_pReadDataScript)
		{
			char szName[256] = "unknown code";
			nRetCode = g_pReadDataScript->CallFunction("get_error_name", "d:s", nRetCode, szName, 256);
			LOG_CHECK_ERROR(nRetCode);

			sprintf(szMessage, "spell %d : %s", nSpellID, szName);
		}

		if (szMessage[0] == 0)
		{
			sprintf(szMessage, "spell %d : %d", nSpellID, nRetCode);
		}
		poCaster->Say(cctScreen, cltNormal, szMessage);
	}
#endif

	PROCESS_ERROR(nRetCode == errSuccess);

	return TRUE;
Exit0:
	return FALSE;
}

INT32 bfReturnSuccess(BTCustomCtx* pCtx, INT32 nParam[])
{
	return brvSuccess;
}
INT32 bfReturnFail(BTCustomCtx* pCtx, INT32 nParam[])
{
	return brvFail;
}

INT32 bfCheckInCombat(BTCustomCtx* pCtx, INT32 nParam[])
{
	if (pCtx->poChar->GetThreatList().IsEmpty())
		return brvFail;
	else
		return brvSuccess;
}

INT32 bfCheckHostInCombat(BTCustomCtx* pCtx, INT32 nParam[])
{
	CCharacter* poHost = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetHostID());
	if (poHost == NULL)
		return brvFail;

	if (poHost->GetThreatList().IsEmpty())
		return brvFail;
	else
		return brvSuccess;
}

INT32 bfRecvMsg(BTCustomCtx* pCtx, INT32 nParam[])
{
#ifdef _ROBOT
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetObjType() == otRole);

	nRetCode = pCtx->poChar->GetAIProcessor()->RegisterMsg(nParam[0]);
	LOG_PROCESS_ERROR(nRetCode);

	pCtx->eRunningType = brtWaitMessage;
	pCtx->nWaitFrame = CGameWorld::GetInstance().GetGameFrame() + 30 * GAME_FPS;

	return brvRunning;
Exit0:
#endif	// _ROBOT
	return brvError;
}

INT32 bfCheckCCFlag(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(nParam[0] > cfInvalid && nParam[0] < cfTotal);

	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (pCtx->poChar->GetCCFlag().CheckFlag(nParam[0]))
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfCheckFlag(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(nParam[0] > cfInvalid && nParam[0] < cfTotal);

	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (pCtx->poChar->GetFlag().CheckFlag(nParam[0]))
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfCheckCharOutOfHomeRange(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nTargetType = nParam[0];
	INT32 nDistance = CONVERT_LENGTH(nParam[1]);
	INT64 llDistance = 0LL;
	CCharacter* poTarget = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
	LOG_PROCESS_ERROR(pCtx->poChar->GetScene());
	LOG_PROCESS_ERROR(pCtx->poChar->GetScene()->GetSceneData());

	poTarget = _GetTargetChar(pCtx, nTargetType);
	PROCESS_SUCCESS(NULL == poTarget);// can't find char means char has out of range
	PROCESS_SUCCESS(poTarget->GetScene() != pCtx->poChar->GetScene());

	// check distance to home point
	llDistance = g_GetDistance3D(poTarget->GetX(), poTarget->GetY(), poTarget->GetZ(),
		pCtx->poChar->GetHomeX(), pCtx->poChar->GetHomeY(), pCtx->poChar->GetHomeZ());
	if (llDistance <= (INT64)nDistance * nDistance)
		return brvFail;
Exit1:
	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCheckCharHp(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nTargetType = nParam[0];
	INT32 nOperator = nParam[1];
	INT32 nPercent = nParam[2];
	INT32 nPoints = nParam[3];
	INT32 nRetCode = 0;
	CCharacter* pChar = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pChar = _GetTargetChar(pCtx, nTargetType);
	if (NULL == pChar)
		return brvFail;

	if (nPercent >= 0)
	{
		nRetCode = _CheckOperator(nOperator, pChar->GetHp() * RATE_MAX, nPercent * pCtx->poChar->GetAttr(catMaxHp));
	}
	else if (nPoints >= 0)
	{
		nRetCode = _CheckOperator(nOperator, pChar->GetHp(), nPoints);
	}

#ifdef _ROBOT
	if (pChar->GetHp()<=0 && pChar->GetMoveState()!=CMS_DIE)
	{
		CRI("bfCheckCharHp type %d char %s hp 0 but state %d", nTargetType, pChar->GetName(), pChar->GetMoveState());
		//INT3();
	}
#endif //_ROBOT
	return nRetCode;

Exit0:
	return brvError;
}

INT32 bfCheckCharMoveState(BTCustomCtx* pCtx, INT32 nParam[])
{
	CCharacter* pChar = NULL;
	INT32 nTargetType = nParam[0];
	INT32 nState = nParam[1];

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pChar = _GetTargetChar(pCtx, nTargetType);
	if (NULL == pChar)
		return brvFail;

	if (pChar->GetMoveState() != nState)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCheckDirection(BTCustomCtx* pCtx, INT32 nParam[])
{
	CCharacter* pChar = NULL;
	INT32 nTargetType = nParam[0];
	INT32 nRange = CONVERT_ANGLE(nParam[1]);
	INT32 nDirection = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (nTargetType == tarSelf)
		nDirection = pCtx->poChar->GetFaceDirection();
	else
	{
		pChar = _GetTargetChar(pCtx, nTargetType);
		if (NULL == pChar)
			return brvFail;

		nDirection = g_GetDirection(pCtx->poChar->GetX(), pCtx->poChar->GetY(), pChar->GetX(), pChar->GetY());
	}

	if (abs(nDirection - pCtx->poChar->GetFaceDirection()) < nRange)
		return brvSuccess;

	return brvFail;
Exit0:
	return brvError;
}

INT32 bfCheckCharHpMeet(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nTargetType = nParam[0];
	INT32 nOperator = nParam[1];
	INT32 nPercent = nParam[2];
	INT32 nAIVarIndex = nParam[3];
	CCharacter* pChar = NULL;
	INT64 nPoints = 0LL;
	INT32 nLatestHp = 0;

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
	pChar = _GetTargetChar(pCtx, nTargetType);
	if (NULL == pChar)
		return brvFail;

	nPoints = (INT64)pChar->GetAttr(catMaxHp) * nPercent / RATE_MAX;
	nLatestHp = pCtx->poChar->GetAIProcessor()->GetVariable(nAIVarIndex);
	if (nLatestHp == 0)
		pCtx->poChar->GetAIProcessor()->SetVariable(nAIVarIndex, pChar->GetHp());

	if (!_CheckOperator(nOperator, nLatestHp, (INT32)nPoints)
		&& _CheckOperator(nOperator, pChar->GetHp(), (INT32)nPoints))
	{
		pCtx->poChar->GetAIProcessor()->SetVariable(nAIVarIndex, pChar->GetHp());
		return brvSuccess;
	}

	pCtx->poChar->GetAIProcessor()->SetVariable(nAIVarIndex, pChar->GetHp());

	return brvFail;
Exit0:
	return brvError;
}
INT32 bfCheckCharCasting(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nTargetType = nParam[0];
	CCharacter* pChar = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pChar = _GetTargetChar(pCtx, nTargetType);
	if (NULL == pChar)
		return brvFail;

	if (pChar->IsCasting())
	{
		return brvSuccess;
	}
	return brvFail;
Exit0:
	return brvError;
}

INT32 bfCheckDice(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nDice = g_Random(RATE_MAX);    
	return _CheckOperator(nParam[0], nDice, nParam[1]);
}

INT32 bfCheckIntValue(BTCustomCtx* pCtx, INT32 nParam[])
{
	return _CheckOperator(nParam[1], nParam[0], nParam[2]);
}

INT32 bfGBuildingDead(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32       nRetCode = 0;
	CNpc * poNpc = NULL;
	CGSGuild    *poGSGuild = NULL;
	INT32       nDefendUserNum = 0;
	const GUILD_BUILDING_DATA *pTemplateData = NULL;
	GS_G_B_DATA     *pBuildingData = NULL;
	UINT32      dwDoodadID = 0;
    UINT64      qwReviveTime = 0;
	INT32       nMaxMemberCount = 0;
	INT32       nTempCount = 0;
	CRole       *poRole = NULL;
	CDoodad     *poDoodad = NULL;
	GUILD_WAR_DATA      *pWarData = NULL;

#ifndef _ROBOT

	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	poNpc = (CNpc*)pCtx->poChar;

	LOG_PROCESS_ERROR(poNpc->GetScene());
	LOG_PROCESS_ERROR(poNpc->GetSpawnPoint());

	poGSGuild = CGuildMgr::GetInstance().FindGuild(poNpc->GetScene()->GetSceneID());
	LOG_PROCESS_ERROR(poGSGuild);

	pBuildingData = poGSGuild->GetBuildingDataByNpcID(poNpc->GetSpawnPoint()->qwNameID);
	LOG_PROCESS_ERROR(pBuildingData);

    qwReviveTime = poNpc->GetNpcData()->nReviveTime * 1000 / GAME_FPS / 1000;
    nRetCode = CCliMessageHandler::GetInstance().DoMSGuildChangeBuildingState(poGSGuild->GetID(), 
        poNpc->GetSpawnPoint()->qwNameID, CGameWorld::GetInstance().GetGameTime() + qwReviveTime);
    LOG_PROCESS_ERROR(nRetCode);

	//nDefendUserNum = poNpc->GetScene()->GetVariable(1);

	//pTemplateData = GetGuildBuildingData(pBuildingData->nType, pBuildingData->nLevel);
	//LOG_PROCESS_ERROR(pTemplateData);

	//nMaxMemberCount = poGSGuild->GetMaxMemberCount();
	//nTempCount = nMaxMemberCount / 3;
	//if (nDefendUserNum < nTempCount)
	//	dwDoodadID = pTemplateData->dwFewMembersReward;
	//else if (nDefendUserNum < nTempCount * 2)
	//	dwDoodadID = pTemplateData->dwSomeMembersReward;
	//else
	//	dwDoodadID = pTemplateData->dwManyMembersReward;

	//poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	//LOG_PROCESS_ERROR(poRole);
	//LOG_PROCESS_ERROR(poRole->GetScene() == poNpc->GetScene());

	//poDoodad = poNpc->GetScene()->SpawnDoodad(dwDoodadID, poRole->GetX(), poRole->GetY(), poRole->GetZ(), poRole->GetFaceDirection());
	//LOG_PROCESS_ERROR(poDoodad);

	//pWarData = CGuildMgr::GetInstance().FindBattleByDefendGuildID(poGSGuild->GetID());
	//LOG_PROCESS_ERROR(pWarData);

	//nRetCode = poRole->Say(poGSGuild->GetID(), cltLighting, pTemplateData->dwDefenderMsgID);
	//LOG_PROCESS_ERROR(errSuccess == nRetCode);

	//nRetCode = poRole->Say(pWarData->dwIntruderID, cltLighting, pTemplateData->dwIntruderMsgID);
	//LOG_PROCESS_ERROR(errSuccess == nRetCode);

#endif // !_ROBOT

	return brvSuccess;
Exit0:
	return brvError;
}


INT32 bfCheckHost(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32       nTargetType = nParam[0];

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    if (nTargetType == tarSelf)
    {
        if (pCtx->poChar->GetHostID() == 0)
            return brvFail;
        else
            return brvSuccess;
    }
    else if (nTargetType == tarAITarget)
    {
        if (pCtx->poChar->GetHostID() != 0 && pCtx->poChar->GetHostID() == pCtx->poChar->GetAIProcessor()->GetTargetID())
            return brvSuccess;
        else
            return brvFail;
    }

Exit0:
    return brvError;
}

INT32 bfMagePetCastSpellToTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CCharacter* pTargetCharacter = NULL;
    INT32 nSpellID = 0;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    nSpellID = pCtx->poChar->GetAIProcessor()->GetVariable(PET_AIV_MAGE_PET_SPELL_ID);
    if (nSpellID == 0 && pCtx->poChar->GetObjType() == otNpc)
    {
        LOG_PROCESS_ERROR(((CNpc*)pCtx->poChar)->GetNpcData());
        CRI("mage pet cast spell, spell is 0. npctype[%d], pet type[%d]", 
            ((CNpc*)pCtx->poChar)->GetNpcData()->nTemplateID, ((CNpc*)pCtx->poChar)->GetPetType());
    }
    LOG_PROCESS_ERROR(nSpellID != 0);

    pTargetCharacter = _GetTargetChar(pCtx, tarAITarget);
    if (NULL == pTargetCharacter)
        return brvFail;

#ifndef _ROBOT
    nRetCode = _CastSpell(pCtx->poChar, nSpellID, pTargetCharacter, pTargetCharacter->GetX(), pTargetCharacter->GetY(), pTargetCharacter->GetZ());
#else
    nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell((CRole*)pCtx->poChar, nSpellID, pTargetCharacter, 0, 0, 0);
#endif
    if (nRetCode != errSuccess)
        return brvFail;

    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfPetScanEnemy(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    nRetCode = pCtx->poChar->GetAIProcessor()->PetScanEnemy(nParam[0], CONVERT_LENGTH(nParam[1]), nParam[2]);
    if (nRetCode)
        return brvSuccess;
    else
        return brvFail;
Exit0:
    return brvError;
}

INT32 bfJumpToHost(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32       nRetCode = 0;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
    LOG_PROCESS_ERROR(pCtx->poChar->GetHostID() != 0);

    nRetCode = pCtx->poChar->GetAIProcessor()->JumpToHost();
    if (FALSE == nRetCode)
        return brvFail;

    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfPetFollow(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    nRetCode = pCtx->poChar->GetAIProcessor()->PetFollow();
    if (FALSE == nRetCode)
        return brvFail;

    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfPetCheckHostDirection(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CCharacter  *poHost = NULL;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
    LOG_PROCESS_ERROR(pCtx->poChar->GetHostID() != 0);

    poHost = CGameWorld::GetInstance().FindCharacter(pCtx->poChar->GetHostID());
    LOG_PROCESS_ERROR(poHost);

    if (poHost->GetFaceDirection() == pCtx->poChar->GetAIProcessor()->GetHostOldDirection())
        return brvSuccess;

    return brvFail;
Exit0:
    return brvError;
}

INT32 bfCheckThreatList(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CCharacter  *poHost = NULL;
    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
    PROCESS_SUCCESS(pCtx->poChar->GetAIProcessor()->GetTargetID() == 0);

    if (pCtx->poChar->GetHostID() != 0)
    {
        poHost = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetHostID());
        if (poHost)
        {
            nRetCode = poHost->GetThreatList().CheckThreat(pCtx->poChar->GetAIProcessor()->GetTargetID());
            if (nRetCode == TRUE)
                return brvSuccess;

            nRetCode = poHost->GetThreatList().CheckThreaten(pCtx->poChar->GetAIProcessor()->GetTargetID());
            if (nRetCode == TRUE)
                return brvSuccess;
        }
    }

    nRetCode = pCtx->poChar->GetThreatList().CheckThreat(pCtx->poChar->GetAIProcessor()->GetTargetID());
    if (TRUE == nRetCode)
        return brvSuccess;
Exit1:
    return brvFail;
Exit0:
    return brvError;
}

INT32 bfPetChangeChaseTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32       nRetCode = 0;
    UINT32      dwCurrentTime = CGameWorld::GetInstance().GetGameTime();
    CCharacter  *poHost = NULL;
    CCharacter  *poTarget = NULL;
    UINT32      dwTargetID = 0;
    UINT64      llMaxDistance3D = 0;
    UINT64      llDistance3D = 0;
    UINT64      llMaxDistance = CONVERT_LENGTH(nParam[0]);

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
    LOG_PROCESS_ERROR(pCtx->poChar->GetHostID() != 0);

    poHost = CGameWorld::GetInstance().FindCharacter(pCtx->poChar->GetHostID());
    LOG_PROCESS_ERROR(poHost);
    if (poHost->GetScene() == NULL)
        return brvFail;

    if (dwTargetID == 0 && poHost->GetLastTargetTimer() + TARGET_WASHOUT_TIME >= dwCurrentTime && poHost->GetLastTargetID() != 0)
    {
        if (GetTypeFromID(poHost->GetLastTargetID()) == otRole)
            dwTargetID = poHost->GetLastTargetID();
    }

    if (dwTargetID == 0 && poHost->GetLastCasterTimer() + CASTER_WASHOUT_TIME > dwCurrentTime)
    {
        if (pCtx->poChar->GetAIProcessor()->GetTargetID() != poHost->GetLastCasterID())
            dwTargetID = poHost->GetLastCasterID();
    }
    
    if (dwTargetID == 0 && pCtx->poChar->GetLastCasterTimer() + CASTER_WASHOUT_TIME > dwCurrentTime)
    {
        if (GetTypeFromID(pCtx->poChar->GetLastCasterID()) == otNpc)
            dwTargetID = pCtx->poChar->GetLastCasterID();
    }

    PROCESS_SUCCESS(dwTargetID == 0 || dwTargetID == pCtx->poChar->GetAIProcessor()->GetTargetID());

    poTarget = CGameWorld::GetInstance().FindValidCharacter(dwTargetID);
    PROCESS_SUCCESS(poTarget == NULL);
    PROCESS_SUCCESS(poTarget->IsDead() != FALSE);
    PROCESS_SUCCESS(poTarget->GetObjID() == pCtx->poChar->GetObjID());
    PROCESS_SUCCESS(poTarget->GetScene() != pCtx->poChar->GetScene());
    PROCESS_SUCCESS(poTarget->GetFlag().CheckFlag(cfNeverBeHit) == TRUE);

    llMaxDistance3D = llMaxDistance * llMaxDistance;
    llDistance3D = g_GetDistance3D(
        pCtx->poChar->GetX(), pCtx->poChar->GetY(), pCtx->poChar->GetZ(),
        poTarget->GetX(), poTarget->GetY(), poTarget->GetZ());
    PROCESS_SUCCESS(llDistance3D > llMaxDistance3D);

    pCtx->poChar->GetAIProcessor()->SetTargetID(dwTargetID);

    return brvSuccess;
Exit1:
    return brvFail;
Exit0:
    return brvError;
}

INT32 bfPetChangeFightTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32       nRetCode = 0;
    UINT32      dwCurrentTime = CGameWorld::GetInstance().GetGameTime();
    CCharacter  *poHost = NULL;
    CCharacter  *poTarget = NULL;
    THREAT_NODE *pNode = NULL;
    UINT32      dwTargetID = 0;
    UINT64      llMaxDistance3D = 0;
    UINT64      llDistance3D = 0;
    UINT64      llNearestDistance3D = 0;
    UINT64      llMaxDistance = CONVERT_LENGTH(nParam[0]);

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
    LOG_PROCESS_ERROR(pCtx->poChar->GetHostID() != 0);

    poHost = CGameWorld::GetInstance().FindCharacter(pCtx->poChar->GetHostID());
    LOG_PROCESS_ERROR(poHost);
    if (poHost->GetScene() == NULL)
        return brvFail;

    if (dwTargetID == 0 && poHost->GetLastTargetTimer() + TARGET_WASHOUT_TIME >= dwCurrentTime && poHost->GetLastTargetID() != 0)
    {
        if (GetTypeFromID(poHost->GetLastTargetID()) == otRole)
            dwTargetID = poHost->GetLastTargetID();
    }

    //if (dwTargetID == 0 && pCtx->poChar->GetLastCasterTimer() + CASTER_WASHOUT_TIME > dwCurrentTime)
    //{
    //    if (GetTypeFromID(pCtx->poChar->GetLastCasterID()) == otNpc/* && pCtx->poChar->GetAIProcessor()->GetTargetID() != pCtx->poChar->GetLastCasterID()*/)
    //    {
    //        DBG("PET CHANGE FIGHT TARGET  my target[%u], caster[%u]",
    //            pCtx->poChar->GetAIProcessor()->GetTargetID(), pCtx->poChar->GetLastCasterID());

    //        poTarget = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetLastCasterID());
    //        if (poTarget && poTarget->IsDead() == FALSE)
    //            dwTargetID = pCtx->poChar->GetLastCasterID();
    //    }
    //        
    //}

    //if (dwTargetID == 0)
        //DBG("PET CHANGE FIGHT TARGET  no caster");

    llMaxDistance3D = llMaxDistance * llMaxDistance;
    llNearestDistance3D = llMaxDistance3D;
    
    if (dwTargetID == 0)
    {
        CBaseList& rThreatList = pCtx->poChar->GetThreatList().GetThreatenList();
        for (pNode = (THREAT_NODE*)rThreatList.Head().pNext; pNode != &(rThreatList.Rear()); pNode = (THREAT_NODE*)pNode->pNext)
        {
            if (pNode->dwCharID == poHost->GetObjID() || pCtx->poChar->GetAIProcessor()->GetTargetID() == pNode->dwCharID)
                continue;
            poTarget = CGameWorld::GetInstance().FindValidCharacter(pNode->dwCharID);
            if (poTarget == NULL)
                continue;
            if (pCtx->poChar->CanAttack(poTarget) == FALSE)
                continue;
            if (poTarget->GetObjType() != otNpc || (poTarget->GetGrid() && poTarget->GetGrid()->uPkType == gptSafe))
                continue;
            if (poTarget->GetFlag().CheckFlag(cfNeverBeHit) == TRUE)
                continue;

            if (((CNpc*)poTarget)->GetNpcData() && ((CNpc*)poTarget)->GetNpcData()->bCounterInvisible == FALSE)    // Ranged-monster tag described by design...
            {
                dwTargetID = poTarget->GetObjID();
                continue;
            }

            llDistance3D = g_GetDistance3D(
                pCtx->poChar->GetX(), pCtx->poChar->GetY(), pCtx->poChar->GetZ(),
                poTarget->GetX(), poTarget->GetY(), poTarget->GetZ());

            if (llNearestDistance3D > llDistance3D)
            {
                llNearestDistance3D = llDistance3D;
                dwTargetID = poTarget->GetObjID();
            }
        }
    }

    if (dwTargetID == 0 && poHost->GetLastCasterTimer() + CASTER_WASHOUT_TIME > dwCurrentTime)
    {
        if (pCtx->poChar->GetAIProcessor()->GetTargetID() != poHost->GetLastCasterID())
        {
            dwTargetID = poHost->GetLastCasterID();
        }
    }

    if (dwTargetID == 0 && poHost->GetLastTargetTimer() + TARGET_WASHOUT_TIME > dwCurrentTime)
    {
        if (GetTypeFromID(pCtx->poChar->GetLastTargetID()) == otNpc)
        {
            poTarget = CGameWorld::GetInstance().FindValidCharacter(poHost->GetLastTargetID());
            if (poTarget && poTarget->GetHostID() != poHost->GetObjID())
            {
                dwTargetID = poHost->GetLastTargetID();
            }
        }
    }

    PROCESS_SUCCESS(dwTargetID == 0 || dwTargetID == pCtx->poChar->GetAIProcessor()->GetTargetID());

    poTarget = CGameWorld::GetInstance().FindValidCharacter(dwTargetID);
    PROCESS_SUCCESS(poTarget == NULL);
    PROCESS_SUCCESS(poTarget->IsDead() != FALSE);
    PROCESS_SUCCESS(poTarget->GetObjID() == pCtx->poChar->GetObjID());
    PROCESS_SUCCESS(poTarget->GetScene() != pCtx->poChar->GetScene());
    PROCESS_SUCCESS(poTarget->GetFlag().CheckFlag(cfNeverBeHit) == TRUE);

    llDistance3D = g_GetDistance3D(
        pCtx->poChar->GetX(), pCtx->poChar->GetY(), pCtx->poChar->GetZ(),
        poTarget->GetX(), poTarget->GetY(), poTarget->GetZ());
    PROCESS_SUCCESS(llDistance3D > llMaxDistance3D);

    //DBG("change target : llDistance3D[%lld] llMaxDistance3D[%lld] nParam0[%d]", llDistance3D, llMaxDistance3D, nParam[0]);

    pCtx->poChar->GetAIProcessor()->SetTargetID(dwTargetID);

    return brvSuccess;
Exit1:
    return brvFail;
Exit0:
    return brvError;
}

INT32 bfPetGetCaster(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32       nRetCode = 0;
    UINT32      dwCurrentTime = CGameWorld::GetInstance().GetGameTime();
    CCharacter  *poHost = NULL;
    CCharacter  *poTarget = NULL;

    UINT64      llMaxDistance3D = 0;
    UINT64      llDistance3D = 0;
    UINT64      llMaxDistance = CONVERT_LENGTH(nParam[0]);

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
    LOG_PROCESS_ERROR(pCtx->poChar->GetHostID() != 0);

    poHost = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetHostID());
    LOG_PROCESS_ERROR(poHost);

    if (pCtx->poChar->GetLastCasterTimer() + CASTER_WASHOUT_TIME > dwCurrentTime)
    {
        if (GetTypeFromID(pCtx->poChar->GetLastCasterID()) == otNpc)
            return brvFail;

        poTarget = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetTargetID());
        if (poTarget == NULL)
            return brvFail;

        if (poTarget->IsDead() != FALSE)
            return brvFail;

        if (poTarget->GetScene() != pCtx->poChar->GetScene())
            return brvFail;

        if (poTarget->GetObjID() == pCtx->poChar->GetObjID())
            return brvFail;

        llMaxDistance3D = llMaxDistance * llMaxDistance;
        llDistance3D = g_GetDistance3D(
            pCtx->poChar->GetX(), pCtx->poChar->GetY(), pCtx->poChar->GetZ(),
            poTarget->GetX(), poTarget->GetY(), poTarget->GetZ());

        if (llDistance3D > llMaxDistance3D)
            return brvFail;

        if (pCtx->poChar->GetAIProcessor()->GetTargetID() == pCtx->poChar->GetLastCasterID())
            return brvFail;

        pCtx->poChar->GetAIProcessor()->SetTargetID(pCtx->poChar->GetLastCasterID());
        return brvSuccess;
    }

    return brvFail;
Exit0:
    return brvError;
}

INT32 bfCheckThreatFadeTime(BTCustomCtx* pCtx, INT32 nParam[])
{
    THREAT_NODE* pNode = nullptr;
    INT32       nRetCode = 0;
    CCharacter  *poOwner = NULL;
    CCharacter  *poTarget = NULL;

    INT32       nOwnerType = nParam[0];
    INT32       nTargetType = nParam[1];
    INT32       nFadeTime = CONVERT_TIME(nParam[2]);

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    poOwner = _GetTargetChar(pCtx, nOwnerType);
    LOG_PROCESS_ERROR(poOwner);

    poTarget = _GetTargetChar(pCtx, nTargetType);
    LOG_PROCESS_ERROR(poTarget);

    pNode = (THREAT_NODE*)(poOwner->GetThreatList().GetThreatList().Head().pNext);
    while (pNode != &(poOwner->GetThreatList().GetThreatList().Rear()))
    {
        if (pNode->dwCharID == poTarget->GetObjID() && (pNode->nLastUpdateLoop + nFadeTime) > CGameWorld::GetInstance().GetGameFrame())
        {
            return brvSuccess;
        }
        pNode = (THREAT_NODE*)pNode->pNext;
    }
    
    return brvFail;
Exit0:
    return brvError;
}

INT32 bftCheckThreatenFadeTime(BTCustomCtx* pCtx, INT32 nParam[])
{
    THREAT_NODE* pNode = nullptr;
    INT32       nRetCode = 0;
    CCharacter  *poOwner = NULL;
    CCharacter  *poTarget = NULL;

    INT32       nOwnerType = nParam[0];
    INT32       nTargetType = nParam[1];
    INT32       nFadeTime = CONVERT_TIME(nParam[2]);

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    poOwner = _GetTargetChar(pCtx, nOwnerType);
    LOG_PROCESS_ERROR(poOwner);

    poTarget = _GetTargetChar(pCtx, nTargetType);
    LOG_PROCESS_ERROR(poTarget);

    pNode = (THREAT_NODE*)(poOwner->GetThreatList().GetThreatenList().Head().pNext);
    while (pNode != &(poOwner->GetThreatList().GetThreatenList().Rear()))
    {
        if (pNode->dwCharID == poTarget->GetObjID() && (pNode->nLastUpdateLoop + nFadeTime) > CGameWorld::GetInstance().GetGameFrame())
        {
            return brvSuccess;
        }
        pNode = (THREAT_NODE*)pNode->pNext;
    }

    return brvFail;
Exit0:
    return brvError;
}

INT32 bfPetWander(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;

    INT32 nRange = CONVERT_LENGTH(nParam[0]);
    INT32 nInterval = CONVERT_TIME(nParam[1]);

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    nRetCode = pCtx->poChar->GetAIProcessor()->PetWander(nRange, nInterval);
    if (!nRetCode)
        return brvFail;

    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfPetCheckValidTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    UINT32  dwTargetID = 0;
    CCharacter  *poTarget = NULL;
    BASE_LIST_NODE* pNode = NULL;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    dwTargetID = pCtx->poChar->GetAIProcessor()->GetTargetID();
    if ( 0 == dwTargetID
        || dwTargetID == pCtx->poChar->GetHostID()
        || dwTargetID == pCtx->poChar->GetObjID())
    {
        return brvFail;
    }

    poTarget = CGameWorld::GetInstance().FindValidCharacter(dwTargetID);
    LOG_PROCESS_ERROR(poTarget);

    if (poTarget->GetCCFlag().CheckFlag(cclInvisible) == FALSE)
        return brvSuccess;
        
    if (pCtx->poChar->GetAIProcessor()->IgnoreInvisible())
        return brvSuccess;

    for (pNode = pCtx->poChar->GetThreatList().GetThreatList().Head().pNext; pNode != &(pCtx->poChar->GetThreatList().GetThreatList().Rear()); pNode = pNode->pNext)
    {
        CCharacter* poTargetChar = CGameWorld::GetInstance().FindValidCharacter(((THREAT_NODE*)pNode)->dwCharID);
        if (poTargetChar == NULL)
            continue;
        if (poTargetChar->GetObjID() == dwTargetID)
        {
            if (pCtx->poChar->GetAIProcessor()->CounterInvisible() == TRUE)
                return brvSuccess;
            return brvFail;
        }  
    }

    return brvFail;
Exit0:
    return brvError;
}

INT32 bfPetCheckNeedChase(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    UINT32  dwTargetID = 0;
    CNpc    *poNpc = NULL;
    const PET_DATA  *pData = NULL;
    CCharacter* poTarget = NULL;
    INT32 nChaseThreshold = CONVERT_LENGTH(nParam[0]);

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
    LOG_PROCESS_ERROR(pCtx->poChar->GetObjType() == otNpc);

    poNpc = (CNpc *)pCtx->poChar;

    pData = GetPetData(poNpc->GetPetType(), poNpc->GetPetLevel());
    LOG_PROCESS_ERROR(pData);

    if (pData->nBornType == petBornByMage)
        nChaseThreshold = pCtx->poChar->GetAIProcessor()->GetVariable(PET_AI_MAGE_CHASE_THRESHOLD);

    poTarget = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetTargetID());
    if (poTarget == NULL)
        return brvFail;

    nRetCode = pCtx->poChar->GetAIProcessor()->NeedChase(poTarget, nChaseThreshold);
    if (nRetCode)
        return brvSuccess;

    return brvFail;
Exit0:
    return brvError;
}

INT32 bfPetChase(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CNpc    *poNpc = NULL;
    const PET_DATA  *pData = NULL;

    INT32 nPreferDistance = CONVERT_LENGTH(nParam[0]);
    INT32 nChaseThreshold = CONVERT_LENGTH(nParam[1]);
    INT32 nChaseDelayFrame = CONVERT_TIME(nParam[2]);

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    poNpc = (CNpc *)pCtx->poChar;
    pData = GetPetData(poNpc->GetPetType(), poNpc->GetPetLevel());
    LOG_PROCESS_ERROR(pData);

    if (pData->nBornType == petBornByMage)
    {
        nChaseThreshold = pCtx->poChar->GetAIProcessor()->GetVariable(PET_AI_MAGE_CHASE_THRESHOLD);
        nPreferDistance = nChaseThreshold = pCtx->poChar->GetAIProcessor()->GetVariable(PET_AI_MAGE_PREFER_DISTANCE);
    }

    nRetCode = pCtx->poChar->GetAIProcessor()->Chase(nPreferDistance, nChaseThreshold, nChaseDelayFrame);
    if (FALSE == nRetCode)
        return brvFail;

    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfPetCheckRoleState(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CCharacter  *poHost = NULL;
    CRole       *poRole = NULL;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    poHost = CGameWorld::GetInstance().FindCharacter(pCtx->poChar->GetHostID());
    LOG_PROCESS_ERROR(poHost);

    LOG_PROCESS_ERROR(poHost->GetObjType() == otRole);
    poRole = (CRole *)poHost;

    if (poRole->GetState() == rsPlaying)
        return brvSuccess;

    return brvFail;
Exit0:
    return brvError;
}

INT32 bfPetStopMove(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CCharacter  *poHost = NULL;
    CRole       *poRole = NULL;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    nRetCode = pCtx->poChar->GetAIProcessor()->Stop();
    
    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfPetDelSbFromThreatenList(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CCharacter  *poTarget = NULL;
    UINT32  dwTargetID = 0;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    dwTargetID = pCtx->poChar->GetAIProcessor()->GetTargetID();
    if (dwTargetID == 0)
        return brvSuccess;

    poTarget = CGameWorld::GetInstance().FindValidCharacter(dwTargetID);
    PROCESS_SUCCESS(poTarget == NULL);

    poTarget->GetThreatList().RemoveThreatNode(pCtx->poChar->GetObjID());
    //poTarget->GetThreatList().RemoveThreatNode(pCtx->poChar->GetHostID());

    pCtx->poChar->GetAIProcessor()->SetTargetID(0);

Exit1:
    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfPetDelSbFromThreatList(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CCharacter  *poTarget = NULL;
    UINT32  dwTargetID = 0;
    CCharacter  *poHost = NULL;

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    dwTargetID = pCtx->poChar->GetAIProcessor()->GetTargetID();
    if (dwTargetID == 0)
        return brvSuccess;

    poTarget = CGameWorld::GetInstance().FindValidCharacter(dwTargetID);
    PROCESS_SUCCESS(poTarget == NULL);

    poHost = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetHostID());
    LOG_PROCESS_ERROR(poHost);

    pCtx->poChar->GetThreatList().RemoveThreatNode(dwTargetID);
    poHost->GetThreatList().RemoveThreatNode(dwTargetID);

Exit1:
    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfPetCheckHostTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32       nRetCode = 0;
    CCharacter  *poHost = NULL;
    INT32       otType = nParam[0];
    UINT32      dwCurrentTime = CGameWorld::GetInstance().GetGameTime();

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    poHost = CGameWorld::GetInstance().FindCharacter(pCtx->poChar->GetHostID());
    LOG_PROCESS_ERROR(poHost);

    if (poHost->GetLastTargetTimer() + TARGET_WASHOUT_TIME >= dwCurrentTime && poHost->GetLastTargetID() != 0)
    {
        if (GetTypeFromID(poHost->GetLastTargetID()) == otType)
            return brvSuccess;
    }
    
    return brvFail;
Exit0:
    return brvError;
}

INT32 bfPetClearHostTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32       nRetCode = 0;
    CCharacter  *poHost = NULL;
    INT32       otType = nParam[0];
    UINT32      dwCurrentTime = CGameWorld::GetInstance().GetGameTime();

    LOG_PROCESS_ERROR(pCtx->poChar);
    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    poHost = CGameWorld::GetInstance().FindCharacter(pCtx->poChar->GetHostID());
    LOG_PROCESS_ERROR(poHost);

    poHost->SetLastCasterID(0);

    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfSetAvoidGroundEffect(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CNpc* poNpc = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar->GetObjType() == otNpc);
	poNpc = (CNpc*)pCtx->poChar;

	poNpc->SetAvoidGroundEffect(nParam[0]);

	return brvFail;
Exit0:
	return brvError;
}

INT32 bfChangeGuildResource(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32       nRetCode = 0;
    CNpc        *poNpc = NULL;

    INT32       nChangeType = nParam[0];
    INT32       nResourceType = nParam[1];
    INT32       nCount = nParam[2];
    INT32       nCtxType = nParam[3];

    LOG_PROCESS_ERROR(pCtx->poChar->GetObjType() == otNpc);
    poNpc = (CNpc*)pCtx->poChar;
    LOG_PROCESS_ERROR(poNpc->GetScene());

    pCtx->eRunningType = brtWaitMSGuildData;
    pCtx->nWaitFrame = CGameWorld::GetInstance().GetGameFrame() + CONVERT_TIME(5000);   // 3-second timeout

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSNpcChangeGuildRes(poNpc->GetObjID(), poNpc->GetScene()->GetSceneID(), nChangeType, nResourceType, nCount, GUILD_CHEST_BUILDING_DAMAGE, nCtxType);
	LOG_PROCESS_ERROR(nRetCode);
#endif

    return brvRunning;
Exit0:
    return brvError;
}

INT32 bfCheckBossState(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (pCtx->poChar->GetAIProcessor()->GetBossState() == nParam[0])
		return brvSuccess;
	else
		return brvFail;

Exit0:
	return brvError;
}

INT32 bfCheckDistance(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nTargetType = nParam[0];
	INT32 nOperator = nParam[1];
	INT32 nDistance = nParam[2] * POINT_PER_CM;

	CCharacter* pChar = NULL;
	INT64 llDistance3D;
	INT64 llMaxDistance3D;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	llMaxDistance3D = (INT64)nDistance * nDistance;

	pChar = _GetTargetChar(pCtx, nTargetType);
	if (pChar == NULL || pChar->GetScene() != pCtx->poChar->GetScene())
	{
		llDistance3D = LLONG_MAX;
	}
	else
	{
		llDistance3D = g_GetDistance3D(
			pCtx->poChar->GetX(), pCtx->poChar->GetY(), pCtx->poChar->GetZ(),
			pChar->GetX(), pChar->GetY(), pChar->GetZ());
	}

	return _CheckOperator(nOperator, llDistance3D, llMaxDistance3D);

Exit0:
	return brvError;
}

INT32 bfCheckCDTimer(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	nRetCode = pCtx->poChar->GetCoolDown().CheckReady(MakeCoolDownID(nParam[0], nParam[1]));
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
}

INT32 bfRoleCheckRoleVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	return _CheckOperator(nParam[0], poRole->GetVariable(nParam[1]), nParam[2]);

Exit0:
	return brvError;
}

INT32 bfCheckSceneVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	return _CheckOperator(nParam[0], poScene->GetVariable(nParam[1]), nParam[2]);

Exit0:
	return brvError;
}

INT32 bfCheckServerVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	return _CheckOperator(nParam[0], CGameWorld::GetInstance().GetServerVariable(poScene->GetServerGroup(), nParam[1]), nParam[2]);

Exit0:
	return brvError;
}

INT32 bfCheckDateTime(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nTime = 0;
	const tm_ex& tm = CGameWorld::GetInstance().GetLocalTime();

	if (nParam[0] != 0 && nParam[0] != tm.tm_wday)
		return brvFail;

	nTime = tm.tm_hour * 10000 + tm.tm_min * 100 + tm.tm_sec;
	if (nTime < nParam[1] || nTime > nParam[2])
		return brvFail;

	return brvSuccess;
}

INT32 bfCheckWeather(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	LOG_PROCESS_ERROR((nParam[0] == 0 || nParam[0] == 1) ||
		(nParam[0] == -1 && nParam[1] * nParam[2] * nParam[3] > 0));

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	if ((nParam[0] > -1 && nParam[0] < 2 && CGameWorld::GetInstance().IsDaytime() == nParam[0]) &&
		((nParam[1] > 0 && poScene->GetWeather() == nParam[1]) ||
		(nParam[2] > 0 && poScene->GetWeather() == nParam[2]) ||
		(nParam[3] > 0 && poScene->GetWeather() == nParam[3])))
	{
		return brvSuccess;
	}

	return brvFail;

Exit0:
	return brvError;
}

INT32 bfCompareSceneVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	return _CheckOperator(nParam[0], poScene->GetVariable(nParam[1]), poScene->GetVariable(nParam[2]));

Exit0:
	return brvError;
}

INT32 bfRoleCheckGuildVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	// todo

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleCheckQuestVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* pRole = NULL;
	const ACCEPTED_QUEST* pcAcceptedQuest = NULL;

	pRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(pRole);

	pcAcceptedQuest = pRole->GetQuestList().GetAcceptedQuest(nParam[1]);
	if (pcAcceptedQuest == NULL)
		return brvFail;

	return _CheckOperator(nParam[0], pRole->GetQuestList().GetVariable(pcAcceptedQuest, nParam[2]), nParam[3]);

Exit0:
	return brvError;
}

INT32 bfRoleCheckHasQuest(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* pRole = NULL;
	const ACCEPTED_QUEST* pcAcceptedQuest = NULL;

	pRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(pRole);

	pcAcceptedQuest = pRole->GetQuestList().GetAcceptedQuest(nParam[0]);

	if (NULL == pcAcceptedQuest)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCheckAIVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	return _CheckOperator(nParam[0], pCtx->poChar->GetAIProcessor()->GetVariable(nParam[1]), nParam[2]);

Exit0:
	return brvError;
}

INT32 bfCheckDoorState(BTCustomCtx* pCtx, INT32 nParam[])
{
	CScene* poScene = NULL;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	if (((poScene->GetDoorState() & (1LL << nParam[0])) > 0) ^ nParam[1])
		return brvFail;
	else
		return brvSuccess;
Exit0:

	return brvError;
}

INT32 bfRoleCheckFaction(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (poRole->GetFaction() == nParam[0])
		return brvSuccess;
	else
		return brvFail;

Exit0:
	return brvError;
}

INT32 bfPrint(BTCustomCtx* pCtx, INT32 nParam[])
{
	if (pCtx->eOwnerType == botAI)
	{
		LOG_PROCESS_ERROR(pCtx->poChar->GetName());

		INF("char (name=%s, ID=%d): %d", pCtx->poChar->GetName(), pCtx->poChar->GetObjID(), nParam[0]);
	}
	else if (pCtx->eOwnerType == botScene)
	{
		LOG_PROCESS_ERROR(pCtx->poScene->GetSceneTempID());

		INF("scene (tempID=%d, ID=%d): %d", pCtx->poScene->GetSceneTempID(), pCtx->poScene->GetSceneID(), nParam[0]);
	}
	else
	{
//		DBG("UNKnown bt ctx owner type: %d", pCtx->eOwnerType);
	}
	return brvSuccess;
Exit0:
//	DBG("batPrint error");
	return brvError;
}

INT32 bfSetTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (nParam[0] > 0)
	{
		pCtx->poChar->GetAIProcessor()->SetTargetID(nParam[0]);
	}
	else
	{
		if (nParam[1] != 0)
		{
			CCharacter* poChar = _GetTargetChar(pCtx, nParam[1]);
			if (poChar)
				pCtx->poChar->GetAIProcessor()->SetTargetID(poChar->GetObjID());
		}
		else
			pCtx->poChar->GetAIProcessor()->SetTargetID(0);
	}

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetRoleAsTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	pCtx->poChar->GetAIProcessor()->SetTargetID(pCtx->dwRoleID);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetTargetAsRole(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->poChar->GetAIProcessor()->GetTargetID());
	LOG_PROCESS_ERROR(poRole);

	pCtx->dwRoleID = pCtx->poChar->GetAIProcessor()->GetTargetID();

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetLastCaster(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nTargetType = nParam[0];

	CCharacter* poTargetChar = _GetTargetChar(pCtx, nTargetType);
	PROCESS_ERROR(poTargetChar);

	pCtx->poChar->SetLastCasterID(poTargetChar->GetObjID());

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetScanEnemyParam(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->SetScanEnemyParam(CONVERT_LENGTH(nParam[0]));

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetScanFriendParam(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->SetScanFriendParam(CONVERT_LENGTH(nParam[0]), nParam[1]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetScanCorpseParam(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->SetScanCorpseParam(nParam[0], nParam[1]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetScanAuraCharParam(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->SetScanAuraParam(nParam[0]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfScanEnemy(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->ScanEnemy();
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfScanNearby(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->ScanNearby();

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfScanRedName(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->ScanRedName(nParam[0]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfScanSabacWinner(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->ScanSabacWinner();

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfScanSabacEnemy(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->ScanSabacEnemy();

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfEnterIdle(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->EnterIdle();
	LOG_PROCESS_ERROR(nRetCode);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfEnterFight(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->EnterFight();
	LOG_PROCESS_ERROR(nRetCode);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCheckCanMove(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (!pCtx->poChar->CanActiveMove() || pCtx->poChar->GetAIProcessor()->GetMoveState() == amsFlee)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCheckCanCastSpell(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pAimCharacter = NULL;
	CScene* pScene = NULL;
	INT32 nTargetType = nParam[0];
	INT32 nSpellID = nParam[1];
	INT64 llTargetX = 0;
	INT64 llTargetY = 0;
	INT64 llTargetZ = 0;

	LOG_PROCESS_ERROR(pCtx->poChar);

	if (pCtx->poChar->GetSpellState() != cspsIdle)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCastSpellToTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pTargetCharacter = NULL;

	INT32 nSpellID = nParam[0];
	INT32 nTargetType = nParam[1];

	LOG_PROCESS_ERROR(pCtx->poChar);

	if (nTargetType == tarHome ||
		nTargetType == tarPosition ||
		nTargetType == tarDirection)
	{
		return brvError;
	}
	else
	{
		pTargetCharacter = _GetTargetChar(pCtx, nTargetType);
		if (NULL == pTargetCharacter)
			return brvFail;

#ifndef _ROBOT
		nRetCode = _CastSpell(pCtx->poChar, nSpellID, pTargetCharacter, pTargetCharacter->GetX(), pTargetCharacter->GetY(), pTargetCharacter->GetZ());
#else
		nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell((CRole*)pCtx->poChar, nSpellID, pTargetCharacter, 0, 0, 0);
#endif
	}

	if (nRetCode != errSuccess)
		return brvFail;

	return brvSuccess;

Exit0:
	return brvError;
}

INT32 bfCastSlotSpellToTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* poTargetChar = NULL;
	CScene* pScene = NULL;
	CRole* poRole = NULL;

	INT32 nSlotID = nParam[0];
	INT32 nTargetType = nParam[1];
	INT64 llTargetX = 0;
	INT64 llTargetY = 0;
	INT64 llTargetZ = 0;

	CSpellSlot* poSlot = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
	LOG_PROCESS_ERROR(pCtx->poChar->GetObjType() == otRole);

	poRole = (CRole*)(pCtx->poChar);

	poSlot = pCtx->poChar->GetSpellSlot(nSlotID);
	LOG_PROCESS_ERROR(poSlot);

	poTargetChar = _GetTargetChar(pCtx, nTargetType);
	if (poTargetChar == NULL)
		return brvFail;

	nRetCode = _CastSpell(pCtx->poChar, poSlot->GetMainSpellID(), poTargetChar, poTargetChar->GetX(), poTargetChar->GetY(), poTargetChar->GetZ());
	if (nRetCode != errSuccess)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCastSpellToPos(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;

	INT32 nSpellID = nParam[0];

	INT64 llTargetX = CONVERT_LENGTH(nParam[1]);
	INT64 llTargetY = CONVERT_LENGTH(nParam[2]);
	INT64 llTargetZ = CONVERT_LENGTH(nParam[3]);

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

#ifndef _ROBOT
	nRetCode = _CastSpell(pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#else   // _ROBOT
	nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell(
		(CRole*)pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#endif  // _ROBOT

	if (nRetCode != errSuccess)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCastSpellToTargetPos(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* poTarget = NULL;
	CScene* pScene = NULL;
	INT64 llTargetX = 0LL;
	INT64 llTargetY = 0LL;
	INT64 llTargetZ = 0LL;

	INT64 llDistance = 0LL;
	INT32 nDirection = 0;

	INT32 nSpellID = nParam[0];
	INT32 nTargetType = nParam[1];
	INT32 nDirectionOffset = CONVERT_ANGLE(nParam[2]);
	INT32 nDistanceOffset = CONVERT_LENGTH(nParam[3]);

	LOG_PROCESS_ERROR(pCtx->poChar);

	if (nTargetType == tarPosition ||
		nTargetType == tarDirection)// TODO: delete
	{
		return brvError;
	}
	else if (nTargetType == tarSelf)
	{
		nDirection = (pCtx->poChar->GetFaceDirection() + nDirectionOffset - MAX_DIRECTION_COUNT / 4 + MAX_DIRECTION_COUNT) % MAX_DIRECTION_COUNT;

		llTargetX = pCtx->poChar->GetX() + nDistanceOffset * g_Cos(nDirection) / SIN_COS_NUMBER;
		llTargetY = pCtx->poChar->GetY() + nDistanceOffset * g_Sin(nDirection) / SIN_COS_NUMBER;

		pScene = pCtx->poChar->GetScene();
		LOG_PROCESS_ERROR(pScene);
		llTargetZ = pScene->GetLowerZ(llTargetX, llTargetY, pCtx->poChar->GetZ() + GRID_WIDTH);

#ifndef _ROBOT
		nRetCode = _CastSpell(pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#else   // _ROBOT
		nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell(
			(CRole*)pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#endif  // _ROBOT
	}
	else if (nTargetType == tarHome)
	{
		nDirection = g_GetDirection(pCtx->poChar->GetHomeX(), pCtx->poChar->GetHomeY(), pCtx->poChar->GetX(), pCtx->poChar->GetY());
		nDirection = (nDirection + nDirectionOffset - MAX_DIRECTION_COUNT / 4 + MAX_DIRECTION_COUNT) % MAX_DIRECTION_COUNT;

		llDistance = g_GetDistance3D(pCtx->poChar->GetX(), pCtx->poChar->GetY(), pCtx->poChar->GetZ(),
			pCtx->poChar->GetHomeX(), pCtx->poChar->GetHomeY(), pCtx->poChar->GetHomeZ()) + nDistanceOffset;

		llTargetX = pCtx->poChar->GetHomeX() - llDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
		llTargetY = pCtx->poChar->GetHomeY() - llDistance * g_Sin(nDirection) / SIN_COS_NUMBER;
		//llTargetZ = pCtx->poChar->GetHomeZ();

		pScene = pCtx->poChar->GetScene();
		LOG_PROCESS_ERROR(pScene);
		llTargetZ = pScene->GetLowerZ(llTargetX, llTargetY, pCtx->poChar->GetHomeZ() + GRID_WIDTH);

#ifndef _ROBOT
		nRetCode = _CastSpell(pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#else   // _ROBOT
		nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell(
			(CRole*)pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#endif  // _ROBOT
	}
	else if (nTargetType == tarAITarget)
	{
		poTarget = _GetTargetChar(pCtx, nTargetType);
		if (NULL == poTarget)
			return brvFail;

		nDirection = g_GetDirection(poTarget->GetX(), poTarget->GetY(), pCtx->poChar->GetX(), pCtx->poChar->GetY());
		nDirection = (nDirection + nDirectionOffset - MAX_DIRECTION_COUNT / 4 + MAX_DIRECTION_COUNT) % MAX_DIRECTION_COUNT;
		llDistance = nDistanceOffset;

		llTargetX = poTarget->GetX() + llDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
		llTargetY = poTarget->GetY() + llDistance * g_Sin(nDirection) / SIN_COS_NUMBER;

		pScene = pCtx->poChar->GetScene();
		LOG_PROCESS_ERROR(pScene);
		llTargetZ = pScene->GetLowerZ(llTargetX, llTargetY, poTarget->GetZ() + GRID_WIDTH);

#ifndef _ROBOT
		nRetCode = _CastSpell(pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#else
		nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell((CRole*)pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#endif
	}

	if (nRetCode != errSuccess)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}
INT32 bfCastSpellToDir(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;
	INT64 llTargetX = 0LL;
	INT64 llTargetY = 0LL;
	INT64 llTargetZ = 0LL;
	INT64 llDistance = 128; // direction spell choose a arbitrary distance

	INT32 nSpellID = nParam[0];
	INT32 nDirection = nParam[1];

	LOG_PROCESS_ERROR(pCtx->poChar);

	llTargetX = llTargetX - llDistance * g_Cos(nDirection) / SIN_COS_NUMBER;
	llTargetY = llTargetY - llDistance * g_Sin(nDirection) / SIN_COS_NUMBER;
	//llTargetZ = pCtx->poChar->GetZ();

	pScene = pCtx->poChar->GetScene();
	LOG_PROCESS_ERROR(pScene);
	llTargetZ = pScene->GetLowerZ(llTargetX, llTargetY, pCtx->poChar->GetZ() + GRID_WIDTH);

#ifndef _ROBOT
	nRetCode = _CastSpell(pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#else
	nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell((CRole*)pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#endif

	if (nRetCode != errSuccess)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}
INT32 bfCastSpellToTargetDir(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pTargetCharacter = NULL;
	CScene* pScene = NULL;
	INT64 llTargetX = 0LL;
	INT64 llTargetY = 0LL;
	INT64 llTargetZ = 0LL;

	INT64 llDistance = 128; // direction spell choose a arbitrary distance
	INT32 nTargetFaceDirection = 0;

	INT32 nSpellID = nParam[0];
	INT32 nTargetType = nParam[1];
	INT32 nDirectionOffset = nParam[2];

	LOG_PROCESS_ERROR(pCtx->poChar);

	if (nTargetType == tarPosition ||
		nTargetType == tarDirection)// TODO: delete
	{
		return brvError;
	}
	else if (nTargetType == tarHome)
	{
		nTargetFaceDirection = g_GetDirection(pCtx->poChar->GetX(), pCtx->poChar->GetY(), pCtx->poChar->GetHomeX(), pCtx->poChar->GetHomeY());

		llTargetX = pCtx->poChar->GetHomeX() + llDistance * g_Cos(nTargetFaceDirection + nDirectionOffset) / SIN_COS_NUMBER;
		llTargetY = pCtx->poChar->GetHomeY() + llDistance * g_Sin(nTargetFaceDirection + nDirectionOffset) / SIN_COS_NUMBER;

		pScene = pCtx->poChar->GetScene();
		LOG_PROCESS_ERROR(pScene);
		llTargetZ = pScene->GetLowerZ(llTargetX, llTargetY, pCtx->poChar->GetHomeZ() + GRID_WIDTH);

#ifndef _ROBOT
		nRetCode = _CastSpell(pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#else   // _ROBOT
		nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell(
			(CRole*)pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#endif  // _ROBOT
	}
	else
	{
		pTargetCharacter = _GetTargetChar(pCtx, nTargetType);
		if (NULL == pTargetCharacter)
			return brvFail;

		nTargetFaceDirection = g_GetDirection(pCtx->poChar->GetX(), pCtx->poChar->GetY(), pTargetCharacter->GetX(), pTargetCharacter->GetY());

		llTargetX = pTargetCharacter->GetX() + llDistance * g_Cos(nTargetFaceDirection + nDirectionOffset) / SIN_COS_NUMBER;
		llTargetY = pTargetCharacter->GetY() + llDistance * g_Sin(nTargetFaceDirection + nDirectionOffset) / SIN_COS_NUMBER;
		pScene = pCtx->poChar->GetScene();
		LOG_PROCESS_ERROR(pScene);
		llTargetZ = pScene->GetLowerZ(llTargetX, llTargetY, pTargetCharacter->GetZ() + GRID_WIDTH);

#ifndef _ROBOT
		nRetCode = _CastSpell(pCtx->poChar, nSpellID, pTargetCharacter, llTargetX, llTargetY, llTargetZ);
#else
		nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell((CRole*)pCtx->poChar, nSpellID, pTargetCharacter, llTargetX, llTargetY, llTargetZ);
#endif
	}

	if (nRetCode != errSuccess)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}


INT32 bfBreakCast(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	LOG_PROCESS_ERROR(pCtx->poChar);

	nRetCode = pCtx->poChar->BreakCast(nParam[0], nParam[1]);
	if (nRetCode)
	{
		pCtx->eRunningType = brtWaitBreaking;
		return brvRunning;
	}

	return brvSuccess;
Exit0:
	return brvError;
}

#define BAT_CASTSPELL_WAIT( action )						\
INT32 action##Wait(BTCustomCtx* pCtx, INT32 nParam[])		\
{															\
	INT32 nRetCode = FALSE;									\
															\
	LOG_PROCESS_ERROR(pCtx->poChar);							\
															\
	nRetCode = action(pCtx, nParam);						\
	PROCESS_ERROR(nRetCode == brvSuccess);					\
															\
	if (pCtx->poChar->GetCastingSpell() == NULL)			\
		return brvSuccess;									\
															\
	pCtx->eRunningType = brtWaitCasting;					\
	return brvRunning;										\
Exit0:														\
	return brvError;										\
}

BAT_CASTSPELL_WAIT(bfCastSpellToTarget)
BAT_CASTSPELL_WAIT(bfCastSlotSpellToTarget)
BAT_CASTSPELL_WAIT(bfCastSpellToPos)
BAT_CASTSPELL_WAIT(bfCastSpellToTargetPos)
BAT_CASTSPELL_WAIT(bfCastSpellToDir)
BAT_CASTSPELL_WAIT(bfCastSpellToTargetDir)


INT32 bfWaitCastSpell(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = FALSE;

	LOG_PROCESS_ERROR(pCtx->poChar);

	if (pCtx->poChar->GetCastingSpell() == NULL)
		return brvSuccess;
	
	pCtx->eRunningType = brtWaitCasting;
	return brvRunning;

Exit0:
	return brvError;
}

INT32 bfIssueSignal(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRegion* pRegion = NULL;

	INT32 nSignalType = nParam[0];
	INT32 nLiveFrames = CONVERT_TIME(nParam[1]);
	INT32 nProcessTimes = nParam[2];
	INT32 nSignalParam = nParam[3];

	LOG_PROCESS_ERROR(nSignalType > 0);

	LOG_PROCESS_ERROR(nLiveFrames > 0); //&& nLiveFrames < SIGNAL_MAX_LIVE_FRAME);
	if (nLiveFrames > SIGNAL_MAX_LIVE_FRAME)
		nLiveFrames = SIGNAL_MAX_LIVE_FRAME;

	LOG_PROCESS_ERROR(nProcessTimes > 0);// && nProcessTimes < SIGNAL_MAX_PROCESS_TIMES );
	if (nProcessTimes > SIGNAL_MAX_PROCESS_TIMES)
		nProcessTimes = SIGNAL_MAX_PROCESS_TIMES;

	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->IssueSignal(nSignalType, nLiveFrames, nProcessTimes, nSignalParam);
	if (!nRetCode)
	{
		return brvFail;
	}

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfPickSignal(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRegion* pRegion = NULL;
	stSignal* pFoundSignal = NULL;

	INT32 nSignalType = nParam[0];
	INT32 nScanDistance = CONVERT_LENGTH(nParam[1]);
	BOOL bCheckParam = nParam[2];
	INT32 nSignalParam = nParam[3];

	LOG_PROCESS_ERROR(nSignalType > 0 && nSignalType < sigEnd);
	LOG_PROCESS_ERROR(nScanDistance > 0);

	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pFoundSignal = pCtx->poChar->GetAIProcessor()->PickSignal(nScanDistance, nSignalType);

	if (NULL == pFoundSignal || (bCheckParam && pFoundSignal->nParam != nSignalParam))
	{
		return brvFail;
	}

	return brvSuccess;

Exit0:
	return brvError;
}

INT32 bfGetTargetFromSignal(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	UINT32 dwSignalIssuerID = 0;
	CCharacter* pIssuer = NULL;
	UINT32 dwTargetID = 0;
	CCharacter* pTarget = NULL;

	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	dwSignalIssuerID = pCtx->poChar->GetAIProcessor()->GetSignalIssuerID();

	pIssuer = CGameWorld::GetInstance().FindCharacter(dwSignalIssuerID);
	if (NULL == pIssuer)
		return brvFail;

	LOG_PROCESS_ERROR(pIssuer->GetAIProcessor());
	dwTargetID = pIssuer->GetAIProcessor()->GetTargetID();

	pTarget = CGameWorld::GetInstance().FindCharacter(dwTargetID);
	if (NULL == pTarget || pTarget->GetHp() <= 0)
		return brvFail;

//	DBG("character %d successfully bfGetTargetFromSignalIssuer, set dwTargetID = %d", pCtx->poChar->GetObjID(), dwTargetID);

	pCtx->poChar->GetAIProcessor()->SetTargetID(dwTargetID);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCastSpell(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pAimCharacter = NULL;
	CScene* pScene = NULL;

	INT32 nTargetType = nParam[0];
	INT32 nSpellID = nParam[1];
	INT64 llTargetX = 0;
	INT64 llTargetY = 0;
	INT64 llTargetZ = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (nTargetType == tarPosition)
	{
		llTargetX = CONVERT_LENGTH(nParam[2]);
		llTargetY = CONVERT_LENGTH(nParam[3]);

		pScene = pCtx->poChar->GetScene();
		PROCESS_ERROR(pScene);
		llTargetZ = pScene->GetLowerZ(llTargetX, llTargetY, pCtx->poChar->GetZ() + GRID_WIDTH);

#ifndef _ROBOT
		nRetCode = _CastSpell(pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#else   // _ROBOT
		nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell(
			(CRole*)pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#endif  // _ROBOT
	}
	else if (nTargetType == tarDirection)
	{
		pScene = pCtx->poChar->GetScene();
		PROCESS_ERROR(pScene);

		INT32 nDirection = CONVERT_ANGLE(nParam[2]);
		INT32 nNormalizedDirection = NormalizeDirection(nDirection);
		llTargetX = pCtx->poChar->GetX() + 128 * g_Cos(nNormalizedDirection) / SIN_COS_NUMBER;
		llTargetY = pCtx->poChar->GetY() + 128 * g_Sin(nNormalizedDirection) / SIN_COS_NUMBER;
		llTargetZ = pScene->GetLowerZ(llTargetX, llTargetY, pCtx->poChar->GetZ() + GRID_WIDTH);

#ifndef _ROBOT
		nRetCode = _CastSpell(pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#else   // _ROBOT
		nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell(
			(CRole*)pCtx->poChar, nSpellID, NULL, llTargetX, llTargetY, llTargetZ);
#endif  // _ROBOT

	}
	else
	{
		pAimCharacter = _GetTargetChar(pCtx, nTargetType);
		if (NULL == pAimCharacter)
			return brvFail;

#ifndef _ROBOT
		nRetCode = _CastSpell(pCtx->poChar, nSpellID, pAimCharacter, pAimCharacter->GetX(), pAimCharacter->GetY(), pAimCharacter->GetZ());
#else
		nRetCode = CRobotMessageHandler::GetInstance().DoCastSpell(
			(CRole*)pCtx->poChar, nSpellID, pAimCharacter, 0, 0, 0);
#endif
	}
	if (errSuccess == nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfAddAura(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pChar = NULL;
	CAura* pAura = NULL;
	CAuraInstance* pAuraInstance = NULL;

	INT32 nTargetType = nParam[0];
	INT32 nAuraID = nParam[1];

	pChar = _GetTargetChar(pCtx, nTargetType);
	if (NULL == pChar)
		return brvFail;

	pAura = CSpellConfig::GetInstance().FindAuraTemplate(nAuraID);
	LOG_PROCESS_ERROR(pAura);

	if (pCtx->eOwnerType == botAI)
	{
		pAuraInstance = pChar->AppendAura(pAura, pCtx->poChar, pCtx->poChar->GetObjID(), NULL, 0, 0, 0, 0);
	}
	else
	{
		pAuraInstance = pChar->AppendAura(pAura, pChar, pChar->GetObjID(), NULL, 0, 0, 0, 0);
	}
	if (pAuraInstance)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfReduceAura(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pChar = NULL;
	CAura* pAura = NULL;
	CAuraInstance* pAuraInstance = NULL;

	INT32 nTargetType = nParam[0];
	INT32 nAuraID = nParam[1];
	INT32 nStackCount = nParam[2];

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pChar = _GetTargetChar(pCtx, nTargetType);
	if (NULL == pChar)
		return brvFail;

	pAuraInstance = pChar->GetFirstAura(nAuraID);
	LOG_PROCESS_ERROR(pAuraInstance);

	pChar->ReduceAuraStack(pAuraInstance, nStackCount);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRemoveAura(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pChar = NULL;
	CAuraInstance* pAuraInstance = NULL;

	INT32 nTargetType = nParam[0];
	INT32 nAuraID = nParam[1];
	BOOL nTriggerOnRemove = nParam[2];

	pChar = _GetTargetChar(pCtx, nTargetType);
	if (NULL == pChar)
		return brvFail;

	pAuraInstance = pChar->GetFirstAura(nAuraID);
	if (pAuraInstance)
	{
		pChar->RemoveAura(pAuraInstance, nTriggerOnRemove);
		return brvSuccess;
	}

	return brvFail;
}

INT32 bfCopyAura(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pChar = NULL;
	CAura* pAura = NULL;
	CAuraInstance* pAuraInstance = NULL;
	CAuraInstance* pNewAuraInstance = NULL;

	INT32 nTargetType = nParam[0];
	INT32 nAuraID = nParam[1];
	INT32 nAuraType = nParam[2];    // TODO:

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pChar = _GetTargetChar(pCtx, nTargetType);
	if (NULL == pChar)
		return brvFail;

	pAuraInstance = pChar->GetFirstAura(nAuraID);
	LOG_PROCESS_ERROR(pAuraInstance);

	pAura = CSpellConfig::GetInstance().FindAuraTemplate(pAuraInstance->GetID());
	LOG_PROCESS_ERROR(pAura);

	pNewAuraInstance = pChar->AppendAura(pAura, pCtx->poChar, pCtx->poChar->GetObjID(), NULL, 0, 0, 0, 0);
	if (pNewAuraInstance)
		return brvSuccess;
	else
		return brvFail;
	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCheckAura(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pChar = NULL;
	CAuraInstance* pAuraInstance = NULL;

	INT32 nTargetType = nParam[0];
	INT32 nAuraID = nParam[1];
	BOOL nTriggerOnRemove = nParam[2];

	pChar = _GetTargetChar(pCtx, nTargetType);
	if (NULL == pChar)
		return brvFail;

	pAuraInstance = pChar->GetFirstAura(nAuraID);
	if (pAuraInstance)
	{
		return brvSuccess;
	}

	return brvFail;
}

INT32 bfWander(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	INT32 nRange = CONVERT_LENGTH(nParam[0]);
	INT32 nInterval = CONVERT_TIME(nParam[1]);

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->Wander(nRange, nInterval);
	if (!nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfGoHome(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->GoHome();
	if (!nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 btSetHome(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetScene());

	pCtx->poChar->SetHome(pCtx->poChar->GetScene()->GetSceneTempID(),
		pCtx->poChar->GetX(), pCtx->poChar->GetY(), pCtx->poChar->GetZ());

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfFlee(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	INT32 nTime = CONVERT_TIME(nParam[0]);

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->Flee(nTime);
	if (!nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfChase(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	INT32 nPreferDistance = CONVERT_LENGTH(nParam[0]);
	INT32 nChaseThreshold = CONVERT_LENGTH(nParam[1]);
	INT32 nChaseDelayFrame = CONVERT_TIME(nParam[2]);

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->Chase(nPreferDistance, nChaseThreshold, nChaseDelayFrame);
	if (FALSE == nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfWithdraw(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	INT32 nPreferDistance = CONVERT_LENGTH(nParam[0]);
	INT32 nChaseThreshold = CONVERT_LENGTH(nParam[1]);

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->Withdraw(nPreferDistance, nChaseThreshold);
	if (FALSE == nRetCode)
		return brvFail;

#if defined(_DEBUG) & !defined(_FINAL)
	if (g_GWConfig.DebugOption.debugai)
	{
		char szMessage[256];
		sprintf(szMessage, "withdraw %d %d %d", nParam[0], nParam[1], nParam[2]);

		pCtx->poChar->Say(cctScreen, cltNormal, szMessage);
	}
#endif

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfCheckNeedChase(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* poTarget = NULL;
	INT32 nChaseThreshold = CONVERT_LENGTH(nParam[0]);

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	poTarget = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetTargetID());
	if (poTarget == NULL)
		return brvFail;

	nRetCode = pCtx->poChar->GetAIProcessor()->NeedChase(poTarget, nChaseThreshold);
	if (nRetCode)
		return brvSuccess;

	return brvFail;
Exit0:
	return brvError;
}

INT32 bfCheckNeedWithdraw(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* poTarget = NULL;
	INT32 nWithdrawThreshold = CONVERT_LENGTH(nParam[0]);

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	poTarget = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetTargetID());
	LOG_PROCESS_ERROR(poTarget);

	nRetCode = pCtx->poChar->GetAIProcessor()->NeedWithdraw(poTarget, nWithdrawThreshold);
	if (FALSE == nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetFollowParam(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	UINT32 dwFollowHostID = nParam[0];
	INT32 nOffsetX = nParam[1];
	INT32 nOffsetY = nParam[2];
    INT32 bUseHostSpeed = nParam[3];

	if (dwFollowHostID == 0)
		dwFollowHostID = pCtx->poChar->GetHostID();
	if (dwFollowHostID == 0)
	{
		if (pCtx->poChar->GetObjType() == otNpc)
		{
			CNpc* poNpc = (CNpc*)pCtx->poChar;
			if (poNpc->GetSpawnPoint() && poNpc->GetSpawnPoint()->nFollowHostID != INVALID_OBJECT_ID)
			{
				CNpc* poHostNpc = poNpc->GetScene()->FindNpc(poNpc->GetSpawnPoint()->nFollowHostID);
				if (poHostNpc)
				{
					dwFollowHostID = poHostNpc->GetObjID();
					nOffsetX = poNpc->GetSpawnPoint()->nFollowOffsetX;
					nOffsetY = poNpc->GetSpawnPoint()->nFollowOffsetY;
				}
			}
		}
	}

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
	pCtx->poChar->GetAIProcessor()->SetFollowParam(dwFollowHostID, CONVERT_LENGTH(nOffsetX), CONVERT_LENGTH(nOffsetY), bUseHostSpeed);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfFollow(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	nRetCode = pCtx->poChar->GetAIProcessor()->Follow();
	if (FALSE == nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfMoveTo(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pTarget = NULL;

	INT32 nTargetType = nParam[0];
	INT64 llTargetX = 0;
	INT64 llTargetY = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (nTargetType == tarPosition)
	{
		INT32 nZ = 0;
		ConvertCoordO2N(nParam[1], nParam[2], nZ);
		llTargetX = CONVERT_LENGTH(nParam[1]);
		llTargetY = CONVERT_LENGTH(nParam[2]);
	}
	else if (nTargetType == tarRelativePosition)
	{
		INT32 nOffsetX = CONVERT_LENGTH(nParam[1]);
		INT32 nOffsetY = CONVERT_LENGTH(nParam[2]);
		INT32 nDirectionY = g_GetDirection(pCtx->poChar->GetX(), pCtx->poChar->GetY(), pTarget->GetX(), pTarget->GetY());
		INT32 nDirectionX = (nDirectionY + (MAX_DIRECTION_COUNT * 3 / 4)) % MAX_DIRECTION_COUNT;

		llTargetX = pCtx->poChar->GetX() + nOffsetX * g_Cos(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Cos(nDirectionY) / SIN_COS_NUMBER;
		llTargetY = pCtx->poChar->GetY() + nOffsetX * g_Sin(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Sin(nDirectionY) / SIN_COS_NUMBER;
	}
	else if (nTargetType == tarHome)
	{
		llTargetX = pCtx->poChar->GetAIProcessor()->GetOwner()->GetHomeX();
		llTargetY = pCtx->poChar->GetAIProcessor()->GetOwner()->GetHomeY();
	}
	else
	{
		pTarget = _GetTargetChar(pCtx, nTargetType);
		LOG_PROCESS_ERROR(pTarget);

		llTargetX = pTarget->GetX();
		llTargetY = pTarget->GetY();
	}

	nRetCode = pCtx->poChar->GetAIProcessor()->MoveTo(llTargetX, llTargetY);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfSyncMoveTo(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pTarget = NULL;

	INT32 nTargetType = nParam[0];
	INT64 llTargetX = 0;
	INT64 llTargetY = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (nTargetType == tarPosition)
	{
		INT32 nZ = 0;
		ConvertCoordO2N(nParam[1], nParam[2], nZ);
		llTargetX = CONVERT_LENGTH(nParam[1]);
		llTargetY = CONVERT_LENGTH(nParam[2]);
	}
	else if (nTargetType == tarRelativePosition)
	{
		pTarget = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetTargetID());
		PROCESS_ERROR(pTarget);

		INT32 nOffsetX = CONVERT_LENGTH(nParam[1]);
		INT32 nOffsetY = CONVERT_LENGTH(nParam[2]);
		INT32 nDirectionY = g_GetDirection(pCtx->poChar->GetX(), pCtx->poChar->GetY(), pTarget->GetX(), pTarget->GetY());
		INT32 nDirectionX = (nDirectionY + (MAX_DIRECTION_COUNT * 3 / 4)) % MAX_DIRECTION_COUNT;

		llTargetX = pCtx->poChar->GetX() + nOffsetX * g_Cos(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Cos(nDirectionY) / SIN_COS_NUMBER;
		llTargetY = pCtx->poChar->GetY() + nOffsetX * g_Sin(nDirectionX) / SIN_COS_NUMBER + nOffsetY * g_Sin(nDirectionY) / SIN_COS_NUMBER;
	}
	else if (nTargetType == tarHome)
	{
		llTargetX = pCtx->poChar->GetHomeX();
		llTargetY = pCtx->poChar->GetHomeY();
	}
	else
	{
		pTarget = _GetTargetChar(pCtx, nTargetType);
		LOG_PROCESS_ERROR(pTarget);
		
		llTargetX = pTarget->GetX();
		llTargetY = pTarget->GetY();
	}

	nRetCode = pCtx->poChar->GetAIProcessor()->SyncMoveTo(llTargetX, llTargetY);
	if (nRetCode)
	{
		pCtx->eRunningType = brtSyncMove;
		return brvRunning;
	}
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfTeleportTo(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	INT32 nX = nParam[0];
	INT32 nY = nParam[1];
	INT32 nZ = nParam[2];
	ConvertCoordO2N(nX, nY, nZ);

	nRetCode = pCtx->poChar->JumpTo(nX * POINT_PER_CM, nY * POINT_PER_CM, nZ * POINT_PER_CM, pCtx->poChar->GetFaceDirection());
	if (FALSE == nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRotateTo(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	INT32 nTargetType = nParam[0];
	INT32 nDirectionDiff = CONVERT_ANGLE(nParam[1]);
	INT32 nDuration = CONVERT_TIME(nParam[2]);
	INT32 nSpeed = CONVERT_ANGLE(nParam[3]);

	CCharacter* pCharAim = NULL;
	INT32 nDirection = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (nTargetType == tarDirection)
	{
		nDirection = nDirectionDiff;
	}
	else
	{
		pCharAim = _GetTargetChar(pCtx, nTargetType);
		if (NULL == pCharAim)
			return brvFail;

		if (pCharAim == pCtx->poChar)
		{
			nDirection = pCtx->poChar->GetFaceDirection();
		}
		else
		{
			nDirection = g_GetDirection(pCtx->poChar->GetX(), pCtx->poChar->GetY(), pCharAim->GetX(), pCharAim->GetY());
		}

		nDirection = NormalizeDirection(nDirection + nDirectionDiff);
	}

	if (pCtx->poChar->GetFaceDirection() == nDirection)
		return brvSuccess;

	if (nSpeed > 0)
	{
		nDirectionDiff = (nDirection - pCtx->poChar->GetFaceDirection() + MAX_DIRECTION_COUNT) % MAX_DIRECTION_COUNT;
		if (nDirectionDiff > MAX_DIRECTION_COUNT / 2)
			nDirectionDiff -= MAX_DIRECTION_COUNT / 2;
		nDuration = nDirectionDiff * GAME_FPS / nSpeed;
		if (nDuration == 0)
			nDuration = 1;
	}

	nRetCode = pCtx->poChar->RotateToDir8(nDirection, nDuration);
	if (!nRetCode)
		return brvFail;

#ifdef _ROBOT
	nRetCode = CRobotMessageHandler::GetInstance().DoRoleRotate((CRole*)(pCtx->poChar), nDirection);
	if (!nRetCode)
		return brvFail;
#endif // _ROBOT

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSyncRotateTo(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	nRetCode = bfRotateTo(pCtx, nParam);
	LOG_PROCESS_ERROR(nRetCode);

	if (pCtx->poChar->IsRotating())
	{
		pCtx->eRunningType = brtSyncRotate;
		return brvRunning;
	}

	return brvSuccess;
Exit0:
	return brvError;
}

BOOL _GetNextTargetFromThreatList(CBaseList &rThreatList, CCharacter *poChar, CCharacter *poHost, UINT32 &dwFirstTargetID, UINT32 &dwNextTargetID, INT64 llChaseRange, BOOL bIgnoreRole)
{
    INT32   nRetCode = 0;
    BOOL    bGetNextTarget = FALSE;
    BASE_LIST_NODE* pNode = NULL;
    INT64   llDistance = 0;
	INT64	llMinDistance = LLONG_MAX;

    LOG_PROCESS_ERROR(poChar);

	for (pNode = rThreatList.Head().pNext; pNode != &(rThreatList.Rear()); pNode = pNode->pNext)
	{
		CCharacter* poTargetChar = CGameWorld::GetInstance().FindValidCharacter(((THREAT_NODE*)pNode)->dwCharID);
		if (poTargetChar == NULL)
			continue;
		if (poTargetChar == poChar)
			continue;
		if (bIgnoreRole == TRUE && poTargetChar->GetObjType() == otRole)
			continue;
		if (poTargetChar->IsDead())
			continue;
		if (poTargetChar->GetCCFlag().CheckFlag(cclInvincible))
			continue;
		if (poTargetChar->GetScene() != poChar->GetScene())
			continue;
		if (poTargetChar->GetCCFlag().CheckFlag(cclInvisible))
		{
			if (!poChar->GetAIProcessor()->IgnoreInvisible() && !poChar->GetAIProcessor()->CounterInvisible())
				continue;
		}
        if (poChar->GetHostID() == poTargetChar->GetObjID())
            continue;
        if (poHost && poHost->CanAttack(poTargetChar) == FALSE)
            continue;

        llDistance = g_GetDistance3D(poTargetChar->GetX(), poTargetChar->GetY(), poTargetChar->GetZ(),
            poChar->GetHomeX(), poChar->GetHomeY(), poChar->GetHomeZ());
        if (llDistance >= llChaseRange)
            continue;

        //LOG_PROCESS_ERROR(poTargetChar->GetScene());
        //if (poTargetChar->GetScene()->IsObstacled(poChar, poTargetChar, FALSE))
        //    continue;

        if (0 == dwFirstTargetID)
            dwFirstTargetID = poTargetChar->GetObjID();

		if (llDistance < llMinDistance)
		{
			dwNextTargetID = poTargetChar->GetObjID();
			llMinDistance = llDistance;
		}
    }

    return TRUE;
Exit0:
    return FALSE;
}

UINT32 _PetGetTargetFromThreatList(CCharacter* poChar, INT64 llChaseRange)
{
    INT32 nRetCode = 0;
    UINT32 dwFirstTargetID = 0;
    UINT32 dwNextTargetID = 0;
    CCharacter* poHost = NULL;
    BOOL bIgnoreRole = FALSE;

    LOG_PROCESS_ERROR(poChar);
    if (poChar->GetHostID() > 0)
    {
        if (poChar->GetObjType() == otNpc && ((CNpc*)poChar)->GetPetType() > 0)
            bIgnoreRole = TRUE;
        poHost = CGameWorld::GetInstance().FindValidCharacter(poChar->GetHostID());
    }
    if (poHost)
    {
        nRetCode = _GetNextTargetFromThreatList(poHost->GetThreatList().GetThreatenList(), poChar, poHost, dwFirstTargetID, dwNextTargetID, llChaseRange, FALSE);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = _GetNextTargetFromThreatList(poHost->GetThreatList().GetThreatList(), poChar, poHost, dwFirstTargetID, dwNextTargetID, llChaseRange, FALSE);
        LOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = _GetNextTargetFromThreatList(poChar->GetThreatList().GetThreatenList(), poChar, NULL, dwFirstTargetID, dwNextTargetID, llChaseRange, bIgnoreRole);
    LOG_PROCESS_ERROR(nRetCode);

    if (0 == dwNextTargetID)
        dwNextTargetID = dwFirstTargetID;

Exit0:
    return dwNextTargetID;
}

INT32 bfPetGetTargetFromThreatList(BTCustomCtx* pCtx, INT32 nParam[])
{
    UINT32 dwTargetID = 0;
    INT64 llChaseRange = CONVERT_LENGTH(nParam[0]) * CONVERT_LENGTH(nParam[0]);
    CBaseList& rThreadList = pCtx->poChar->GetThreatList().GetThreatList();

    LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    dwTargetID = _PetGetTargetFromThreatList(pCtx->poChar, llChaseRange);
    if (dwTargetID > 0)
    {
        pCtx->poChar->GetAIProcessor()->SetTargetID(dwTargetID);

#if defined(_DEBUG) & !defined(_FINAL)
        if (g_GWConfig.DebugOption.debugai)
        {
            char szMessage[256];
            CCharacter* poTargetChar = CGameWorld::GetInstance().FindValidCharacter(dwTargetID);
            if (poTargetChar && poTargetChar->GetName())
                sprintf(szMessage, "get target from threat list : %d %s", dwTargetID, poTargetChar->GetName());
            else
                sprintf(szMessage, "get target from threat list : NULL");

            pCtx->poChar->Say(cctScreen, cltNormal, szMessage);

        }
#endif
        return brvSuccess;
    }
    else
    {
        pCtx->poChar->GetAIProcessor()->SetTargetID(0);
        return brvFail;
    }


Exit0:
    return brvError;
}

UINT32 _GetTargetFromThreatList(CCharacter* poChar, INT64 llChaseRange)
{
	INT32 nRetCode = 0;
    UINT32 dwFirstTargetID = 0;
    UINT32 dwNextTargetID = 0;
    CCharacter* poHost = NULL;
    BOOL bIgnoreRole = FALSE;

	LOG_PROCESS_ERROR(poChar);

    nRetCode = _GetNextTargetFromThreatList(poChar->GetThreatList().GetThreatList(), poChar, NULL, dwFirstTargetID, dwNextTargetID, llChaseRange, bIgnoreRole);
    LOG_PROCESS_ERROR(nRetCode);

    if (0 == dwNextTargetID)
        dwNextTargetID = dwFirstTargetID;

Exit0:
    return dwNextTargetID;
}

INT32 bfGetTargetFromThreatList(BTCustomCtx* pCtx, INT32 nParam[])
{
	UINT32 dwTargetID = 0;
	INT64 llChaseRange = CONVERT_LENGTH(nParam[0]) * CONVERT_LENGTH(nParam[0]);
	CBaseList& rThreadList = pCtx->poChar->GetThreatList().GetThreatList();

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

    dwTargetID = _GetTargetFromThreatList(pCtx->poChar, llChaseRange);
	if (dwTargetID > 0)
	{
		pCtx->poChar->GetAIProcessor()->SetTargetID(dwTargetID);

#if defined(_DEBUG) & !defined(_FINAL)
		if (g_GWConfig.DebugOption.debugai)
		{
			char szMessage[256];
			CCharacter* poTargetChar = CGameWorld::GetInstance().FindValidCharacter(dwTargetID);
			if (poTargetChar && poTargetChar->GetName())
				sprintf(szMessage, "get target from threat list : %d %s", dwTargetID, poTargetChar->GetName());
			else
				sprintf(szMessage, "get target from threat list : NULL");

			pCtx->poChar->Say(cctScreen, cltNormal, szMessage);

		}
#endif
		return brvSuccess;
	}
    else
    {
        pCtx->poChar->GetAIProcessor()->SetTargetID(0);
        return brvFail;
    }


Exit0:
	return brvError;
}

INT32 bfAddThreatListFromSignal(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = FALSE;
	CCharacter* pSignalIssuer = NULL;
	BASE_LIST_NODE* pNode = NULL;
	INT32 nThreatValue = nParam[0];

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
	LOG_PROCESS_ERROR(nThreatValue > 0);

	pSignalIssuer = CGameWorld::GetInstance().FindCharacter(pCtx->poChar->GetAIProcessor()->GetSignalIssuerID());
	if (NULL == pSignalIssuer)
		return brvFail;

	pNode = pSignalIssuer->GetThreatList().GetThreatList().Head().pNext;
	while (pNode != &pSignalIssuer->GetThreatList().GetThreatList().Rear())
	{
		THREAT_NODE* pThreatNode = (THREAT_NODE*)pNode;

		CCharacter* poThreatChar = CGameWorld::GetInstance().FindCharacter(pThreatNode->dwCharID);
		if (poThreatChar && pCtx->poChar->CanAttack(poThreatChar))
		{
			nRetCode = pCtx->poChar->GetThreatList().ModifyThreatValue(poThreatChar, nThreatValue);
			LOG_CHECK_ERROR(nRetCode);
		}

		pNode = pNode->pNext;
	}

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfScanHostThreatList(BTCustomCtx* pCtx, INT32 nParam[])
{
	UINT32 dwTargetID = 0;
	INT64 llDistance = 0;

	CCharacter* pChar = NULL;
	THREAT_NODE* pNode = NULL;
	CAIProcessor* poAiProcessor = NULL;
	CCharacter* poHost = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar);

	poAiProcessor = pCtx->poChar->GetAIProcessor();
	LOG_PROCESS_ERROR(poAiProcessor);

	{ // linux jump to label 'Exit0'
		poHost = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetHostID());
        if (poHost == NULL)
            return brvFail;

		CBaseList& rThreadList = poHost->GetThreatList().GetThreatList();

		pNode = (THREAT_NODE*)rThreadList.Head().pNext;
		while (pNode != &(rThreadList.Rear()))
		{
			pChar = CGameWorld::GetInstance().FindValidCharacter(pNode->dwCharID);
			if (pChar)
			{
				llDistance = g_GetDistance2D(pCtx->poChar->GetHomeX(), pCtx->poChar->GetHomeY(), pChar->GetX(), pChar->GetY());

				dwTargetID = pNode->dwCharID;
				break;

				//			if( llScanRange > llDistance )
				//			{
				//				dwTargetID = pNode->dwCharID;
				//				break;
				//			}
			}

			pNode = (THREAT_NODE*)pNode->pNext;
		}
	}

	if (dwTargetID > 0)
	{
		pCtx->poChar->GetAIProcessor()->SetTargetID(dwTargetID);
		return brvSuccess;
	}
	else
	{
		return brvFail;
	}

Exit0:
	return brvError;
}

INT32 bfFindFirstTargetByID(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	INT32 nNpcID = nParam[0];
	INT64 llMaxDist = CONVERT_LENGTH(nParam[1]);
	CRegion* poRegion = pCtx->poChar->GetRegion();

	LOG_PROCESS_ERROR(poRegion);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->SetTargetID(0);

	{
		struct ScanNpc
		{
			INT32 nNpcID;
			INT64 llMaxDist;
			CCharacter* poChar;

			BOOL operator()(CNpc* poNpc)
			{
				if (poNpc->GetNpcData()->nTemplateID == nNpcID)
				{
					if (poChar->GetDistance3D(poNpc) <= llMaxDist * llMaxDist)
					{
						poChar->GetAIProcessor()->SetTargetID(poNpc->GetObjID());
						return FALSE;
					}
				}
				return TRUE;
			}

			BOOL operator()(CRegion* poRegion)
			{
				return poRegion->TraverseNpc(*this);
			}
		};

		ScanNpc scanner;
		scanner.nNpcID = nNpcID;
		scanner.llMaxDist = llMaxDist;
		scanner.poChar = pCtx->poChar;
		poRegion->Traverse9Region(scanner);
	}

	if (pCtx->poChar->GetAIProcessor()->GetTargetID() == 0)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfAddTargetThreat(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* pTarget = NULL;

	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pTarget = CGameWorld::GetInstance().FindValidCharacter(pCtx->poChar->GetAIProcessor()->GetTargetID());
	if (pTarget)
	{
		nRetCode = pCtx->poChar->GetThreatList().ModifyThreatValue(pTarget, nParam[0]);
		LOG_PROCESS_ERROR(nRetCode);

		return brvSuccess;
	}

	return brvFail;

Exit0:
	return brvError;
}

INT32 bfSay(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	char cszWords[MAX_CHAT_MESSAGE_LEN];
	const char* pcszMessage = NULL;
	UINT32 dwTargetID = nParam[0];
	INT32 nStringID = nParam[1];

	UINT32 dwLocation = cltNormal;

	if (nParam[2] != cltInvalid)
	{
		dwLocation = nParam[2];
	}

	if (nStringID > SERVER_STRING_START_ID) // server translate
	{
		pcszMessage = CGameWorld::GetInstance().GetString(nStringID);
		LOG_PROCESS_ERROR(pcszMessage);
	}
	else // client need
	{
		snprintf(cszWords, PATH_LEN, "<#P0:%d><#T:%d>", nParam[3], nStringID);
		pcszMessage = cszWords;
	}

	if (pCtx->eOwnerType == botAI)
	{
		nRetCode = pCtx->poChar->Say(dwTargetID, dwLocation, pcszMessage);
		LOG_PROCESS_ERROR(nRetCode);
	}
	else if (pCtx->eOwnerType == botScene)
	{
		nRetCode = pCtx->poScene->Say(dwTargetID, dwLocation, pcszMessage);
		LOG_PROCESS_ERROR(nRetCode);
	}

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetBossState(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetAIProcessor()->SetBossState(nParam[0]);

//	DBG("------------------>>> Set Boss State: %d", nParam[0]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfWait(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nInterval = CONVERT_TIME(nParam[0]);
	INT32 nIntervalCeiling = CONVERT_TIME(nParam[1]);

	LOG_PROCESS_ERROR(nInterval > 0);
	if (nIntervalCeiling > nInterval)
	{
		nInterval = g_Random(nInterval, nIntervalCeiling);
	}

	pCtx->eRunningType = brtWait;
	pCtx->nWaitFrame = CGameWorld::GetInstance().GetGameFrame() + nInterval;

	return brvRunning;
Exit0:
	return brvError;
}

INT32 bfSetFlag(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pCtx->poChar->GetFlag().SetFlag(nParam[0]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfClearFlag(BTCustomCtx* pCtx, INT32 nParam[])
{
	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);

	pCtx->poChar->GetFlag().ClearFlag(nParam[0]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfPlayAction(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	INT32 nActionID = nParam[0];

	nRetCode = pCtx->poChar->PlayAction(nActionID);
	if (!nRetCode)
		return brvFail;

#if defined(_DEBUG) & !defined(_FINAL)
	if (g_GWConfig.DebugOption.debugai)
	{
		char szMessage[256];
		sprintf(szMessage, "play action %d", nParam[0]);

		pCtx->poChar->Say(cctScreen, cltNormal, szMessage);
	}
#endif

	return brvSuccess;
}

INT32 bfSetCDTimer(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	INT32 nKey = nParam[0];
	INT32 nIndex = nParam[1];
	INT32 nInterval = nParam[2];
	INT32 nIntervalCeiling = nParam[3];

	nInterval = CONVERT_TIME(nInterval);
	nIntervalCeiling = CONVERT_TIME(nIntervalCeiling);

	if (nIntervalCeiling > 0 && nIntervalCeiling >= nInterval)
	{
		nInterval = g_Random(nInterval, nIntervalCeiling);
	}

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
	nInterval += pCtx->poChar->GetGlobalCDDelayFrame();

	nRetCode = pCtx->poChar->GetCoolDown().Add(MakeCoolDownID(nKey, nIndex), nInterval);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;

Exit0:
	return brvError;
}

INT32 bfSetSceneVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	if (nParam[2] == 0)
		poScene->SetVariable(nParam[0], nParam[1]);
	else
		poScene->SetVariable(nParam[0], poScene->GetVariable(nParam[0]) + nParam[2]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetServerVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);
#ifndef _ROBOT
	if (nParam[2] == 0)
		CCliMessageHandler::GetInstance().DoMSSetServerVariable(poScene->GetServerGroup(), nParam[0], TRUE, nParam[1]);
	else
		CCliMessageHandler::GetInstance().DoMSSetServerVariable(poScene->GetServerGroup(), nParam[0], FALSE, nParam[2]);
#endif
	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleSetRoleVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (nParam[2] == 0)
		poRole->SetVariable(nParam[0], nParam[1]);
	else
		poRole->SetVariable(nParam[0], poRole->GetVariable(nParam[0]) + nParam[2]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleSetGuildVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	// todo

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleSetQuestVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	const ACCEPTED_QUEST* pcAcceptedQuest = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	pcAcceptedQuest = poRole->GetQuestList().GetAcceptedQuest(nParam[0]);
	LOG_PROCESS_ERROR(pcAcceptedQuest);

	if (nParam[3] == 0)
	{
		poRole->GetQuestList().SetVariable(pcAcceptedQuest, nParam[1], nParam[2]);
	}
	else
	{
		poRole->GetQuestList().SetVariable(pcAcceptedQuest, nParam[1],
			poRole->GetQuestList().GetVariable(pcAcceptedQuest, nParam[1]) + nParam[3]);
	}

	return brvSuccess;
Exit0:
	return brvFail;
}

INT32 bfRoleSetCombatStat(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(nParam[0] >= escsScore1 && nParam[0] <= escsTotalScore);

	if (nParam[2] == 0)
	{
		poRole->SetCombatStat(nParam[0], nParam[1]);
	}
	else
	{
		poRole->IncCombatStat(nParam[0], nParam[2]);
	}

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleTeleportToPoint(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	CScene  *poScene = NULL;
	const SERVER_POINT *    pPoint = NULL;
	CSceneStaticData  *pcSceneData = NULL;

	INT32 nSceneTemplateID = nParam[0];
	INT32 nSceneID = nParam[1];
	INT32 nServerPointID = nParam[2];

	LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
	LOG_PROCESS_ERROR(pCtx->poChar);

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poRole->GetScene());

	pcSceneData = CSceneFileMgr::GetInstance().GetStaticData(nSceneTemplateID);
	LOG_PROCESS_ERROR(pcSceneData);

	pPoint = pcSceneData->GetServerPoint(nServerPointID);
	LOG_PROCESS_ERROR(pPoint);

	if (0 == nSceneTemplateID || (poRole->GetScene()->GetSceneTempID() == nSceneTemplateID && poRole->GetScene()->GetSceneID() == nSceneID))
	{
		poScene = poRole->GetScene();
		LOG_PROCESS_ERROR(poScene);

		nRetCode = poRole->JumpTo(pPoint->nX, pPoint->nY, pPoint->nZ, poRole->GetFaceDirection());
	}
	else
	{
		const SCENE_DATA* pSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nSceneTemplateID);
		LOG_PROCESS_ERROR(pSceneData);

		if (pSceneData->nType == estGuild)
		{
			nRetCode = CGameWorld::GetInstance().JumpTo(poRole, nSceneTemplateID,
				nSceneID, pPoint->nX, pPoint->nY, pPoint->nZ, poRole->GetFaceDirection());
		}
		else
		{
			nRetCode = CGameWorld::GetInstance().JumpTo(poRole, nSceneTemplateID,
				0, pPoint->nX, pPoint->nY, pPoint->nZ, poRole->GetFaceDirection());
		}
	}

	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;

Exit0:
	return brvError;
}

INT32 bfRoleTeleport(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	INT32 X = nParam[0];
	INT32 Y = nParam[1];
	INT32 Z = nParam[2];
	INT32 nSceneTempID = nParam[3];
	ConvertCoordO2N(X, Y, Z);

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poRole->GetScene());

	if (0 == nSceneTempID || poRole->GetScene()->GetSceneTempID() == nSceneTempID)
	{
		CScene* poScene = poRole->GetScene();
		LOG_PROCESS_ERROR(poScene);

		nRetCode = poRole->JumpTo(X * POINT_PER_CM, Y * POINT_PER_CM, Z * POINT_PER_CM, poRole->GetFaceDirection());
	}
	else
	{
		const SCENE_DATA* pSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(nSceneTempID);
		LOG_PROCESS_ERROR(pSceneData);

        if (pSceneData->nType == estGuild)
        {
            LOG_PROCESS_ERROR(poRole->GetGuildID() != 0);
            nRetCode = CGameWorld::GetInstance().JumpTo(poRole, nSceneTempID,
                poRole->GetGuildID(), X * POINT_PER_CM, Y * POINT_PER_CM, Z * POINT_PER_CM, poRole->GetFaceDirection());
        }
		else
		{
			nRetCode = CGameWorld::GetInstance().JumpTo(poRole, nSceneTempID,
				0, X * POINT_PER_CM, Y * POINT_PER_CM, Z * POINT_PER_CM, poRole->GetFaceDirection());
		}
	}

	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;

Exit0:
	return brvError;
}

INT32 bfRoleReturnStaticScene(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	nRetCode = poRole->JumpToReturnPos();
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleSetReputation(BTCustomCtx* pCtx, INT32 nParam[])
{
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (nParam[2] == 0)
		poRole->GetReputation().SetValue(nParam[0], nParam[1]);
	else
		poRole->GetReputation().ModifyValue(nParam[0], nParam[2]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleCheckMoney(BTCustomCtx* pCtx, INT32 nParam[])
{
    CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
    LOG_PROCESS_ERROR(poRole);

    return _CheckOperator(nParam[0], poRole->GetItemBag()->GetMoney(nParam[1]), nParam[2]);
Exit0:
    return brvError;
}

INT32 bfRoleAddMoney(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* poChar = NULL;
	CRole* poRole = NULL;

	poChar = _GetTargetChar(pCtx, nParam[0]);
	LOG_PROCESS_ERROR(poChar);

	LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);
	poRole = (CRole*)poChar;

	nRetCode = poRole->GetItemBag()->AddMoney(nParam[1], nParam[2], (EMoneyLog)nParam[3], 0);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleSubMoney(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* poChar = NULL;
	CRole* poRole = NULL;

	poChar = _GetTargetChar(pCtx, nParam[0]);
	LOG_PROCESS_ERROR(poChar);

	LOG_PROCESS_ERROR(poChar->GetObjType() == otRole);
	poRole = (CRole*)poChar;

	nRetCode = poRole->GetItemBag()->SubMoney(nParam[1], nParam[2], (EMoneyLog)nParam[3], 0);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleCheckItemCount(BTCustomCtx* pCtx, INT32 nParam[])
{
    CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
    LOG_PROCESS_ERROR(poRole);

    return _CheckOperator(nParam[0], poRole->GetItemBag()->GetItemCount(nParam[1], nParam[3]), nParam[2]);
Exit0:
    return brvError;
}

INT32 bfRoleAddItem(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	nRetCode = poRole->GetItemBag()->AddItem(nParam[0], nParam[1], ITEMLOG_SCRIPT, nParam[2], FALSE);

	if (nRetCode == errSuccess)
		return brvSuccess;
	else
	{
		if (nRetCode == errItemBagFull)
		{
			nRetCode = poRole->SendMail(poRole->GetObjID(), mtiBagFullItem, nParam[0], nParam[1]);
			LOG_CHECK_ERROR(nRetCode);

			if (nRetCode)
				return brvSuccess;
		}
	}
	CRI("failed to add item %d-%d to role (%s:%d)", nParam[0], nParam[1], poRole->GetName(), poRole->GetObjID());

	return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleDelItem(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CRole* poRole = NULL;

    poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
    LOG_PROCESS_ERROR(poRole);

    nRetCode = poRole->GetItemBag()->DeleteItemId(nParam[0], nParam[1], ITEMLOG_SCRIPT, nParam[2], nParam[3]);

    if (nRetCode)
        return brvSuccess;
    else
        return brvFail;
Exit0:
    return brvError;
}

INT32 bfRoleCheckInTeam(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	UINT32 dwTeamID = INVALID_OBJECT_ID;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	dwTeamID = poRole->GetTeamID();

	if (dwTeamID != INVALID_OBJECT_ID)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleLeaveTeam(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	UINT32 dwTeamID = INVALID_OBJECT_ID;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	nRetCode = poRole->ForceLeaveTeam();
	if (nRetCode > 0)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleSetFaction(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	INT32 nFaction = nParam[0];
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	poRole->SetFaction(nFaction);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleCheckSelfScene(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32   nRetCode = 0;
	CRole   *poRole = NULL;
	CScene  *poScene = NULL;

	if (botScene == pCtx->eOwnerType)
	{
		poScene = pCtx->poScene;
	}
	else if (botAI == pCtx->eOwnerType)
	{
		LOG_PROCESS_ERROR(pCtx->poChar);
		poScene = pCtx->poChar->GetScene();
	}
	LOG_PROCESS_ERROR(poScene);

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (poScene->GetSceneID() != poRole->GetGuildID())
	{
		return brvFail;
	}

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleCheckAllianceScene(BTCustomCtx *pCtx, INT32 nParam[])
{
    INT32   nRetCode = 0;
    CRole   *poRole = NULL;
    CScene  *poScene = NULL;
    CGSGuild *poSceneOwner = NULL;

    if (botScene == pCtx->eOwnerType)
    {
        poScene = pCtx->poScene;
    }
    else if (botAI == pCtx->eOwnerType)
    {
        LOG_PROCESS_ERROR(pCtx->poChar);
        poScene = pCtx->poChar->GetScene();
    }
    LOG_PROCESS_ERROR(poScene);

    poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
    LOG_PROCESS_ERROR(poRole);

    poSceneOwner = CGuildMgr::GetInstance().FindGuild(poScene->GetSceneID());
    LOG_PROCESS_ERROR(poSceneOwner);

    nRetCode = poSceneOwner->IsAlliance(poRole->GetGuildID());
    if (nRetCode == FALSE)
        return brvFail;

    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfSendAwardInfo(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	INT32 nBatchNumber = 0;
	CRole   *poRole = NULL;
	char  szAwardNumber[32];

	memset(szAwardNumber, 0, sizeof(szAwardNumber));

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	nBatchNumber = nParam[0];
	snprintf(szAwardNumber, 32, "%d", nBatchNumber);

#ifndef _ROBOT

	nRetCode = CCliMessageHandler::GetInstance().DoMSSendAward2Ump(poRole->GetObjID(), 0, szAwardNumber);
	LOG_PROCESS_ERROR(nRetCode);

#endif //_ROBOT

	return brvSuccess;
Exit0:
	return brvError;
}

struct CTraverseRoleInSceneToDistributeExp
{
	INT32	m_nExp;
	INT32	m_nAuraID;
	INT32	m_nTableIndex;
	bool operator()(CRole* poRole)
	{
		INT32 nLevel = 0;
		INT32 nExp = 0;
		if (m_nAuraID != 0)
			PROCESS_ERROR(poRole->GetFirstAura(m_nAuraID));

		nLevel = poRole->GetLevel();
		if (nLevel < 1)
			nLevel = 1;
		else if (nLevel >= sizeof(g_GWConfig.DistributeExp[m_nTableIndex].nLevelTable) / sizeof(g_GWConfig.DistributeExp[m_nTableIndex].nLevelTable[0]))
			nLevel = sizeof(g_GWConfig.DistributeExp[m_nTableIndex].nLevelTable) / sizeof(g_GWConfig.DistributeExp[m_nTableIndex].nLevelTable[0]) - 1;
		nExp = m_nExp * g_GWConfig.DistributeExp[m_nTableIndex].nLevelTable[nLevel] / RATE_MAX;
		if (nExp > nLevel * 20)
			nExp = nLevel * 20;

		poRole->AddExp(nExp, FALSE, TRUE);

	Exit0:
		return TRUE;
	}
};

INT32 bfDistributeExp(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	INT32 nRoleCount = 0;
	CScene* poScene = NULL;
	INT32 nTableIndex = nParam[0];
	INT32 nAuraID = nParam[1];
	CTraverseRoleInSceneToDistributeExp  TraverseRoleInScene;

	if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	else if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	LOG_PROCESS_ERROR(poScene);

	nRoleCount = poScene->GetRoleCount();
	if (nRoleCount > 0)
	{
		if (nRoleCount >= sizeof(g_GWConfig.DistributeExp[nTableIndex].nRoleCountTable) / sizeof(g_GWConfig.DistributeExp[nTableIndex].nRoleCountTable[0]))
			nRoleCount = sizeof(g_GWConfig.DistributeExp[nTableIndex].nRoleCountTable) / sizeof(g_GWConfig.DistributeExp[nTableIndex].nRoleCountTable[0]) - 1;

		TraverseRoleInScene.m_nExp = g_GWConfig.DistributeExp[nTableIndex].nRoleCountTable[nRoleCount];
		TraverseRoleInScene.m_nAuraID = nAuraID;
		TraverseRoleInScene.m_nTableIndex = nTableIndex;
		nRetCode = poScene->TraverseRole(TraverseRoleInScene);
		if (!nRetCode)
			return brvFail;
	}

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfClearThreatList(BTCustomCtx* pCtx, INT32 nParam[])
{
    pCtx->poChar->GetThreatList().Clear();

    return brvSuccess;
}

INT32 bfAddExp(BTCustomCtx* pCtx, INT32 nParam[])
{
	CCharacter* poTarget = NULL;
	CRole* poRole = NULL;
	
	poTarget = _GetTargetChar(pCtx, nParam[0]);
	if (poTarget == NULL || poTarget->GetObjType() != otRole)
		return brvFail;

	poRole = (CRole*)poTarget;
	poRole->AddExp(nParam[1], FALSE, TRUE);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleCheckInteractOption(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (poRole->CheckOption(nParam[0]))
		return brvSuccess;

	return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleCheckVIP(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	INT32 nVIPLevel = nParam[0];

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (poRole->GetVIPLevel() >= nVIPLevel)
		return brvSuccess;

	return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleCheckLevel(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	INT32 nVIPLevel = nParam[0];

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	return _CheckOperator(nParam[0], poRole->GetLevel(), nParam[1]);
Exit0:
	return brvError;
}

INT32 bfRoleIsSabacGuildMember(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (poRole->GetGuildID() != 0)
	{
		if (poRole->GetGuildID() == CSabacMgr::GetInstance().GetOccupyGuildID(poRole->GetServerGroup()))
			return brvSuccess;
	}

	return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleIsSabacGuildLeader(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	CGSGuild* poGuild = NULL;

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (poRole->GetGuildID() != 0)
	{
		if (poRole->GetGuildID() == CSabacMgr::GetInstance().GetOccupyGuildID(poRole->GetServerGroup()))
		{
			poGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
			LOG_PROCESS_ERROR(poGuild);

			if (poGuild->GetLeaderID() == poRole->GetObjID())
				return brvSuccess;
		}
	}
	return brvFail;

Exit0:
	return brvError;
}

INT32 bfInvincible(BTCustomCtx* pCtx, INT32 nParam[])
{
	if (nParam[0] > 0)
		pCtx->poChar->IncControlCount(cclInvincible);
	else
		pCtx->poChar->DecControlCount(cclInvincible);

	return brvSuccess;
}

INT32 bfCheckHeroWeaponEquipped(BTCustomCtx* pCtx, INT32 nParam[])
{
	CHero* poHero = NULL;
	CRole* poRole = NULL;
	CItemBag* poBag = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar->GetObjType() == otHero);

	poHero = (CHero*)(pCtx->poChar);
	poRole = poHero->GetHostRole();
	LOG_PROCESS_ERROR(poRole);

	poBag = poRole->GetItemBag();
	LOG_PROCESS_ERROR(poBag);

	if (poBag->GetHeroWeaponType() != WEAPONTYPE_HAND)
		return brvSuccess;

	return brvFail;
Exit0:
	return brvError;
}

INT32 bfCheckTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nTargetType = nParam[0];
	CAIProcessor* poAIProcessor = pCtx->poChar->GetAIProcessor();

	switch (nTargetType)
	{
	case tarAITarget:
		PROCESS_ERROR(poAIProcessor->GetTargetID() != INVALID_OBJECT_ID);
		break;
	case tarHost:
		PROCESS_ERROR(pCtx->poChar->GetHostID() != INVALID_OBJECT_ID);
		break;
	case tarNearestEnemy:
		PROCESS_ERROR(poAIProcessor->GetNearestEnemyID() != INVALID_OBJECT_ID);
		break;
	case tarFarthestEnemy:
		PROCESS_ERROR(poAIProcessor->GetFarthestEnemyID() != INVALID_OBJECT_ID);
		break;
	case tarCastingEnemy:
		PROCESS_ERROR(poAIProcessor->GetCastingEnemyID() != INVALID_OBJECT_ID);
		break;
	case tarCorpse:
		PROCESS_ERROR(poAIProcessor->GetCorpseCharID() != INVALID_OBJECT_ID);
		break;
	case tarAuraChar:
		PROCESS_ERROR(poAIProcessor->GetAuraCharID() != INVALID_OBJECT_ID);
		break;
	case tarNearestFriend:
		PROCESS_ERROR(poAIProcessor->GetNearestFriendID() != INVALID_OBJECT_ID);
		break;
	case tarSignalIssuer:
		PROCESS_ERROR(poAIProcessor->GetSignalIssuerID() != INVALID_OBJECT_ID);
		break;
	case tarSignalIssuerTarget:
	{
		PROCESS_ERROR(poAIProcessor->GetSignalIssuerID() != INVALID_OBJECT_ID);
		CCharacter* poTarget = CGameWorld::GetInstance().FindValidCharacter(poAIProcessor->GetSignalIssuerID());
		PROCESS_ERROR(poTarget);
		PROCESS_ERROR(poTarget->GetAIProcessor() && poTarget->GetAIProcessor()->GetTargetID());
	}
		break;
	case tarRolePet1:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 0 < poRole->GetPetCount())
			PROCESS_ERROR(poRole->GetPetNpcID(0) != INVALID_OBJECT_ID);
		break;
	}
	case tarRolePet2:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 1 < poRole->GetPetCount())
			PROCESS_ERROR(poRole->GetPetNpcID(1) != INVALID_OBJECT_ID);
		break;
	}
	case tarRolePet3:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 2 < poRole->GetPetCount())
			PROCESS_ERROR(poRole->GetPetNpcID(2) != INVALID_OBJECT_ID);
		break;
	}
	case tarRolePet4:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 3 < poRole->GetPetCount())
			PROCESS_ERROR(poRole->GetPetNpcID(3) != INVALID_OBJECT_ID);
		break;
	}
	case tarRolePet5:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 4 < poRole->GetPetCount())
			PROCESS_ERROR(poRole->GetPetNpcID(4) != INVALID_OBJECT_ID);
		break;
	}
	case tarRolePet6:
	{
		CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
		if (poRole && 5 < poRole->GetPetCount())
			PROCESS_ERROR(poRole->GetPetNpcID(5) != INVALID_OBJECT_ID);
		break;
	}
	default:
		LOG_PROCESS_ERROR(FALSE);
		break;
	}

	return brvSuccess;
Exit0:
	return brvFail;
}

INT32 bfCheckTargetType(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nTargetType = nParam[0];
    INT32 nObjectType = nParam[1];
    CAIProcessor* poAIProcessor = pCtx->poChar->GetAIProcessor();

    switch (nTargetType)
    {
    case tarAITarget:
    {
        PROCESS_ERROR(poAIProcessor->GetTargetID() != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poAIProcessor->GetTargetID()) == nObjectType);
    }
        break;
    case tarHost:
    {
        PROCESS_ERROR(pCtx->poChar->GetHostID() != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(pCtx->poChar->GetHostID()) == nObjectType);
    }
        break;
    case tarNearestEnemy:
    {
        PROCESS_ERROR(poAIProcessor->GetNearestEnemyID() != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poAIProcessor->GetNearestEnemyID()) == nObjectType);
    }
        break;
    case tarFarthestEnemy:
    {
        PROCESS_ERROR(poAIProcessor->GetFarthestEnemyID() != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poAIProcessor->GetFarthestEnemyID()) == nObjectType);
    }
        break;
    case tarCastingEnemy:
    {
        PROCESS_ERROR(poAIProcessor->GetCastingEnemyID() != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poAIProcessor->GetCastingEnemyID()) == nObjectType);
    }
        break;
    case tarCorpse:
    {
        PROCESS_ERROR(poAIProcessor->GetCorpseCharID() != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poAIProcessor->GetCorpseCharID()) == nObjectType);
    }
        break;
    case tarAuraChar:
    {
        PROCESS_ERROR(poAIProcessor->GetAuraCharID() != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poAIProcessor->GetAuraCharID()) == nObjectType);
    }
        break;
    case tarNearestFriend:
    {
        PROCESS_ERROR(poAIProcessor->GetNearestFriendID() != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poAIProcessor->GetNearestFriendID()) == nObjectType);
    }
        break;
    case tarSignalIssuer:
    {
        PROCESS_ERROR(poAIProcessor->GetSignalIssuerID() != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poAIProcessor->GetSignalIssuerID()) == nObjectType);
    }
        break;
    case tarSignalIssuerTarget:
    {
        PROCESS_ERROR(poAIProcessor->GetSignalIssuerID() != INVALID_OBJECT_ID);
        CCharacter* poTarget = CGameWorld::GetInstance().FindValidCharacter(poAIProcessor->GetSignalIssuerID());
        PROCESS_ERROR(poTarget);
        PROCESS_ERROR(poTarget->GetAIProcessor() && poTarget->GetAIProcessor()->GetTargetID());
        PROCESS_ERROR(GetTypeFromID(poTarget->GetAIProcessor()->GetTargetID()) == nObjectType);
    }
        break;
    case tarRolePet1:
    {
        CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
        if (poRole && 0 < poRole->GetPetCount())
            PROCESS_ERROR(poRole->GetPetNpcID(0) != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poRole->GetPetNpcID(0)) == nObjectType);
    }
        break;
    case tarRolePet2:
    {
        CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
        if (poRole && 1 < poRole->GetPetCount())
            PROCESS_ERROR(poRole->GetPetNpcID(1) != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poRole->GetPetNpcID(1)) == nObjectType);
    }
        break;
    case tarRolePet3:
    {
        CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
        if (poRole && 2 < poRole->GetPetCount())
            PROCESS_ERROR(poRole->GetPetNpcID(2) != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poRole->GetPetNpcID(2)) == nObjectType);
    }
        break;
    case tarRolePet4:
    {
        CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
        if (poRole && 3 < poRole->GetPetCount())
            PROCESS_ERROR(poRole->GetPetNpcID(3) != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poRole->GetPetNpcID(3)) == nObjectType);
    }
        break;
    case tarRolePet5:
    {
        CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
        if (poRole && 4 < poRole->GetPetCount())
            PROCESS_ERROR(poRole->GetPetNpcID(4) != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poRole->GetPetNpcID(4)) == nObjectType);
    }
        break;
    case tarRolePet6:
    {
        CRole* poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
        if (poRole && 5 < poRole->GetPetCount())
            PROCESS_ERROR(poRole->GetPetNpcID(5) != INVALID_OBJECT_ID);
        PROCESS_ERROR(GetTypeFromID(poRole->GetPetNpcID(5)) == nObjectType);
    }
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
        break;
    }

    return brvSuccess;
Exit0:
    return brvFail;
}


INT32 bfSuicide(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	BOOL bGenerateDrop = nParam[0];
	BOOL bDisappear = nParam[1];

	if (pCtx->poChar->GetObjType() == otNpc)
	{
		CNpc* poNpc = (CNpc*)pCtx->poChar;
		poNpc->SetGenerateDrop(bGenerateDrop);
	}

	if (bDisappear)
		pCtx->poChar->DeleteSelf();
	else
		nRetCode = pCtx->poChar->Die(cdrSuicide);

	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
}

INT32 bfSpawnNpc(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;
	CNpc* pNpc = NULL;
	INT32 nDirection = 0;

	LOG_PROCESS_ERROR(pCtx);
	if (pCtx->eOwnerType == botAI)
	{
		LOG_PROCESS_ERROR(pCtx->poChar);
		pScene = pCtx->poChar->GetScene();
		nDirection = pCtx->poChar->GetFaceDirection();
	}
	else if (pCtx->eOwnerType == botScene)
	{
		LOG_PROCESS_ERROR(pCtx->poScene);
		pScene = pCtx->poScene;
		nDirection = CONVERT_ANGLE(225);
	}
	LOG_PROCESS_ERROR(pScene);

	ConvertCoordO2N(nParam[1], nParam[2], nParam[3]);

	pNpc = pScene->SpawnNpc(nParam[0], nParam[1] * POINT_PER_CM, nParam[2] * POINT_PER_CM, nParam[3] * POINT_PER_CM, nDirection, 0, NULL);
	if (pNpc)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfSpawnNpcAtPoint(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;
	CNpc* pNpc = NULL;
	const SERVER_POINT* pServerPoint = NULL;

	LOG_PROCESS_ERROR(pCtx);
	if (pCtx->eOwnerType == botAI)
	{
		pScene = pCtx->poChar->GetScene();
	}
	else if (pCtx->eOwnerType == botScene)
	{
		pScene = pCtx->poScene;
	}
	LOG_PROCESS_ERROR(pScene);
	LOG_PROCESS_ERROR(pScene->GetStaticData());

	pServerPoint = pScene->GetStaticData()->GetServerPoint(nParam[1]);
	LOG_PROCESS_ERROR(pServerPoint);

	pNpc = pScene->SpawnNpc(nParam[0], pServerPoint->nX, pServerPoint->nY, pServerPoint->nZ, CONVERT_ANGLE(nParam[2]), 0, NULL);
	if (pNpc)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfSpawnSingleNpc(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CNpc* pNpc = NULL;
	CScene* pScene = NULL;

	if (pCtx->eOwnerType == botAI)
		pScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		pScene = pCtx->poScene;
	LOG_PROCESS_ERROR(pScene);

	pNpc = pScene->SpawnSingleNpc(nParam[0]);
	if (pNpc)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfSpawnMediumGroup(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;

	if (pCtx->eOwnerType == botAI)
		pScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		pScene = pCtx->poScene;
	LOG_PROCESS_ERROR(pScene);

	nRetCode = pScene->SpawnNpcMediumGroup(nParam[0]);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfSpawnLargeGroup(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;

	if (pCtx->eOwnerType == botAI)
		pScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		pScene = pCtx->poScene;
	LOG_PROCESS_ERROR(pScene);

	nRetCode = pScene->SpawnNpcLargeGroup(nParam[0]);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfSpawnDoodad(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	CDoodad* poDoodad = NULL;
	INT32 nDirection = 0;

	LOG_PROCESS_ERROR(pCtx);
	if (pCtx->eOwnerType == botAI)
	{
		LOG_PROCESS_ERROR(pCtx->poChar);
		poScene = pCtx->poChar->GetScene();
		nDirection = pCtx->poChar->GetFaceDirection();
	}
	else if (pCtx->eOwnerType == botScene)
	{
		LOG_PROCESS_ERROR(pCtx->poScene);
		poScene = pCtx->poScene;
		nDirection = CONVERT_ANGLE(225);
	}
	LOG_PROCESS_ERROR(poScene);

	poDoodad = poScene->SpawnDoodad(nParam[0],
		nParam[1] * POINT_PER_CM, nParam[2] * POINT_PER_CM, nParam[3] * POINT_PER_CM, nDirection);
	if (poDoodad)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfSpawnDoodadAtPoint(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;
	CDoodad* poDoodad = NULL;
	const SERVER_POINT* pServerPoint = NULL;

	LOG_PROCESS_ERROR(pCtx);
	if (pCtx->eOwnerType == botAI)
	{
		pScene = pCtx->poChar->GetScene();
	}
	else if (pCtx->eOwnerType == botScene)
	{
		pScene = pCtx->poScene;
	}
	LOG_PROCESS_ERROR(pScene);
	LOG_PROCESS_ERROR(pScene->GetStaticData());

	pServerPoint = pScene->GetStaticData()->GetServerPoint(nParam[1]);
	LOG_PROCESS_ERROR(pServerPoint);

	poDoodad = pScene->SpawnDoodad(nParam[0],
		pServerPoint->nX, pServerPoint->nY, pServerPoint->nZ, CONVERT_ANGLE(nParam[2]));
	if (poDoodad)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfSpawnDoodadGroup(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;

	if (pCtx->eOwnerType == botAI)
		pScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		pScene = pCtx->poScene;
	LOG_PROCESS_ERROR(pScene);

	nRetCode = pScene->SpawnDoodadGroup(nParam[0]);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfCheckSceneRuning(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;

	if (pCtx->eOwnerType == botAI)
		pScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		pScene = pCtx->poScene;
	LOG_PROCESS_ERROR(pScene);

	nRetCode = (pScene->GetState() == edstRuning);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfFinishScene(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;

	if (pCtx->eOwnerType == botAI)
		pScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		pScene = pCtx->poScene;
	LOG_PROCESS_ERROR(pScene);

	// todo_tq 11 temp code  Allan Tao[10.12.2013] 
	//if (pScene->GetSceneTempID() == 127)
	//{
	//    DumpExceptionFilter(NULL);
	//}

	nRetCode = pScene->Close(CONVERT_TIME(nParam[0]));
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfCalculateDungeonScore(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* pScene = NULL;

	if (pCtx->eOwnerType == botAI)
		pScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		pScene = pCtx->poScene;
	LOG_PROCESS_ERROR(pScene);

	nRetCode = pScene->CalculateDungeonScore();
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_TO_SET_REPUTATION
{
	INT32 m_nFactionID;
	INT32 m_nForceID;
	INT32 m_nReputationValue;
	bool operator()(CRole* poRole)
	{
		if (poRole->GetFaction() == m_nFactionID)
			poRole->GetReputation().ModifyValue(m_nForceID, m_nReputationValue);
		return TRUE;
	};
};

INT32 bfSetReputationToFaction(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	TRAVERSE_SCENE_ROLE_TO_SET_REPUTATION SetReputation;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	SetReputation.m_nFactionID = nParam[0];
	SetReputation.m_nForceID = nParam[1];
	SetReputation.m_nReputationValue = nParam[2];

	nRetCode = poScene->TraverseRole(SetReputation);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_TO_ADD_MONEY
{
	INT32 m_nFactionID;
	INT32 m_nMoneyType;
	INT32 m_nMoneyAmount;
	INT32 m_nReason;
	INT32 m_nSceneTempID;
	bool operator()(CRole* poRole)
	{
		if (poRole->GetFaction() == m_nFactionID)
			poRole->GetItemBag()->AddMoney(m_nMoneyType, m_nMoneyAmount, (EMoneyLog)m_nReason, 0);
		return TRUE;
	};
};

INT32 bfAddMoneyToFaction(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	TRAVERSE_SCENE_ROLE_TO_ADD_MONEY AddMoney;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	AddMoney.m_nFactionID = nParam[0];
	AddMoney.m_nMoneyType = nParam[1];
	AddMoney.m_nMoneyAmount = nParam[2];
	AddMoney.m_nReason = nParam[3];
	AddMoney.m_nSceneTempID = poScene->GetSceneTempID();

	nRetCode = poScene->TraverseRole(AddMoney);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_TO_SUB_MONEY
{
	INT32 m_nFactionID;
	INT32 m_nMoneyType;
	INT32 m_nMoneyAmount;
	INT32 m_nReason;
	INT32 m_nSceneTempID;
	bool operator()(CRole* poRole)
	{
		if (poRole->GetFaction() == m_nFactionID)
			poRole->GetItemBag()->SubMoney(m_nMoneyType, m_nMoneyAmount, (EMoneyLog)m_nReason, 0);
		return TRUE;
	};
};

INT32 bfSubMoneyToFaction(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	TRAVERSE_SCENE_ROLE_TO_SUB_MONEY SubMoney;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	SubMoney.m_nFactionID = nParam[0];
	SubMoney.m_nMoneyType = nParam[1];
	SubMoney.m_nMoneyAmount = nParam[2];
	SubMoney.m_nReason = nParam[3];
	SubMoney.m_nSceneTempID = poScene->GetSceneTempID();

	nRetCode = poScene->TraverseRole(SubMoney);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_TO_ADD_AURA
{
	INT32 m_nFactionID;
	INT32 m_nAuraID;
	bool operator()(CRole* poRole)
	{
		CAura* pAura = NULL;
		CAuraInstance* pAuraInstance = NULL;
		if (poRole->GetFaction() == m_nFactionID)
		{
			pAura = CSpellConfig::GetInstance().FindAuraTemplate(m_nAuraID);
			if (NULL == pAura)
				return FALSE;

			pAuraInstance = poRole->AppendAura(pAura, poRole, poRole->GetObjID(), NULL, 0, 0, 0, 0);
			if (NULL == pAuraInstance)
				return FALSE;
		}
		return TRUE;
	};
};
INT32 bfAddAuraToFaction(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	TRAVERSE_SCENE_ROLE_TO_ADD_AURA AddAura;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	AddAura.m_nFactionID = nParam[0];
	AddAura.m_nAuraID = nParam[1];

	nRetCode = poScene->TraverseRole(AddAura);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_TO_REMOVE_AURA
{
	INT32 m_nFactionID;
	INT32 m_nAuraID;
	BOOL  m_bTriggerOnRemove;
	bool operator()(CRole* poRole)
	{
		CAura* pAura = NULL;
		CAuraInstance* pAuraInstance = NULL;
		if (poRole->GetFaction() == m_nFactionID)
		{
			pAuraInstance = poRole->GetFirstAura(m_nAuraID);
			if (NULL == pAuraInstance)
				return FALSE;
			poRole->RemoveAura(pAuraInstance, m_bTriggerOnRemove);
		}
		return TRUE;
	};
};

INT32 bfRemoveAuraToFaction(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	TRAVERSE_SCENE_ROLE_TO_REMOVE_AURA RemoveAura;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	RemoveAura.m_nFactionID = nParam[0];
	RemoveAura.m_nAuraID = nParam[1];
	RemoveAura.m_bTriggerOnRemove = nParam[2];

	nRetCode = poScene->TraverseRole(RemoveAura);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_TO_ADD_ITEM
{
	INT32 m_nFactionID;
	INT32 m_nItemID;
	INT32 m_nItemCount;
	INT32 m_nUserDefined;
	bool operator()(CRole* poRole)
	{
		INT32 nRetCode = 0;

		if (poRole->GetFaction() == m_nFactionID)
		{
			nRetCode = poRole->GetItemBag()->AddItem(m_nItemID, m_nItemCount, ITEMLOG_SCRIPT, m_nUserDefined, FALSE);
			if (nRetCode != errSuccess)
			{
				if (nRetCode == errItemBagFull)
				{
					nRetCode = poRole->SendMail(poRole->GetObjID(), mtiBagFullItem, m_nItemID, m_nItemCount);
					LOG_CHECK_ERROR(nRetCode);

					if (nRetCode)
						return TRUE;
				}

				CRI("failed to add item %d-%d to role (%s:%d)", m_nItemID, m_nItemCount, poRole->GetName(), poRole->GetObjID());
			}
		}
		return TRUE;
	};
};

INT32 bfAddItemToFaction(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	TRAVERSE_SCENE_ROLE_TO_ADD_ITEM AddItem;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	AddItem.m_nFactionID = nParam[0];
	AddItem.m_nItemID = nParam[1];
	AddItem.m_nItemCount = nParam[2];
	AddItem.m_nUserDefined = nParam[3];

	nRetCode = poScene->TraverseRole(AddItem);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_TO_SET_COMBAT_STAT
{
	INT32 m_nFactionID;
	INT32 m_nField;
	INT32 m_nAbsoluteValue;
	INT32 m_nRelativeValue;
	bool operator()(CRole* poRole)
	{
		INT32 nRetCode = 0;

		if (poRole->GetFaction() == m_nFactionID)
		{
			if (m_nRelativeValue == 0)
				poRole->SetCombatStat(m_nField, m_nAbsoluteValue);
			else
				poRole->IncCombatStat(m_nField, m_nRelativeValue);
		}
		return TRUE;
	};
};

INT32 bfSetCombatStatToFaction(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	TRAVERSE_SCENE_ROLE_TO_SET_COMBAT_STAT SetCombatStat;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	SetCombatStat.m_nFactionID = nParam[0];
	SetCombatStat.m_nField = nParam[1];
	SetCombatStat.m_nAbsoluteValue = nParam[2];
	SetCombatStat.m_nRelativeValue = nParam[3];

	LOG_PROCESS_ERROR(nParam[1] >= escsScore1 && nParam[1] <= escsTotalScore);

	nRetCode = poScene->TraverseRole(SetCombatStat);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfOpenDoor(BTCustomCtx* pCtx, INT32 nParam[])
{
	CScene* poScene = NULL;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	poScene->SetDoorState(nParam[0], FALSE);

	return brvSuccess;
Exit0:

	return brvError;
}

INT32 bfCloseDoor(BTCustomCtx* pCtx, INT32 nParam[])
{
	CScene* poScene = NULL;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	poScene->SetDoorState(nParam[0], TRUE);

	return brvSuccess;
Exit0:

	return brvError;
}

INT32 bfToggleDoor(BTCustomCtx* pCtx, INT32 nParam[])
{
	CScene* poScene = NULL;

	if (pCtx->eOwnerType == botAI)
		poScene = pCtx->poChar->GetScene();
	else if (pCtx->eOwnerType == botScene)
		poScene = pCtx->poScene;
	LOG_PROCESS_ERROR(poScene);

	if (poScene->IsDoorClosed(nParam[0]))
		poScene->SetDoorState(nParam[0], FALSE);
	else
		poScene->SetDoorState(nParam[0], TRUE);

	return brvSuccess;
Exit0:

	return brvError;
}

INT32 bfSetAlwaysRun(BTCustomCtx* pCtx, INT32 nParam[])
{
	CNpc* poNpc = NULL;

	LOG_PROCESS_ERROR(pCtx);
	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetObjType() == otNpc);

	poNpc = static_cast<CNpc*>(pCtx->poChar);

	poNpc->SetAlwaysRun(nParam[0]);

	return brvSuccess;
Exit0:

	return brvError;
}

INT32 bfSetCharHp(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nTargetType = nParam[0];
	INT32 nPoints = nParam[1];
	INT32 nPercentOfMax = nParam[2];
	CCharacter* pChar = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	pChar = _GetTargetChar(pCtx, nTargetType);
	PROCESS_ERROR(pChar);

	if (nPoints >= 0)
	{
		pChar->ChangeHp(nPoints - pChar->GetHp(), cdrAIBehavior);
	}
	else if (nPercentOfMax >= 0)
	{
		nPoints = pChar->GetAttr(catMaxHp) * nPercentOfMax / RATE_MAX;
		pChar->ChangeHp(nPoints - pChar->GetHp(), cdrAIBehavior);
	}

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetAIVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());

	if (nParam[2] == 0)
		pCtx->poChar->GetAIProcessor()->SetVariable(nParam[0], nParam[1]);
	else
		pCtx->poChar->GetAIProcessor()->SetVariable(nParam[0], pCtx->poChar->GetAIProcessor()->GetVariable(nParam[0]) + nParam[2]);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSetPatrolParam(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = FALSE;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
	pCtx->poChar->GetAIProcessor()->SetPatrolParam(nParam[0], nParam[1]);

	return brvSuccess;

Exit0:
	return brvError;
}

INT32 bfPatrol(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = FALSE;

	LOG_PROCESS_ERROR(pCtx->poChar->GetAIProcessor());
	nRetCode = pCtx->poChar->GetAIProcessor()->Patrol();
	if (!nRetCode)
		return brvFail;

	return brvSuccess;

Exit0:
	return brvError;
}

INT32 bfStepAroundTarget(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CCharacter* poTarget = NULL;

	INT32 nPreferDistance = CONVERT_LENGTH(nParam[0]);
	INT32 nChaseThreshold = CONVERT_LENGTH(nParam[1]);

	CAIProcessor* poProcessor = NULL;
	LOG_PROCESS_ERROR(pCtx->poChar);
	poProcessor = pCtx->poChar->GetAIProcessor();
	LOG_PROCESS_ERROR(poProcessor);

	poTarget = _GetTargetChar(pCtx, tarAITarget);
	LOG_PROCESS_ERROR(poTarget);

	nRetCode = poProcessor->StepAround(poTarget, nPreferDistance, nChaseThreshold);
	if (nRetCode)
	{
#if defined(_DEBUG) & !defined(_FINAL)
		if (g_GWConfig.DebugOption.debugai)
		{
			char szMessage[256];
			sprintf(szMessage, "step around %d %d %d", nParam[0], nParam[1], nParam[2]);

			pCtx->poChar->Say(cctScreen, cltNormal, szMessage);
		}
#endif

		pCtx->eRunningType = brtStepAround;
		return brvRunning;
	}
	else
		return brvFail;

Exit0:
	return brvError;
}

INT32 bfSendMail(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32       nRetCode = 0;
	UINT32  dwRoleID = nParam[0];
	INT32   nSubjectID = nParam[1];
	INT32   nContentID = nParam[2];
	UINT32  dwItemID = nParam[3];

	LOG_PROCESS_ERROR(pCtx->poChar);

	nRetCode = pCtx->poChar->SendMail(dwRoleID, mtiBagFullItem, dwItemID, 1);
	LOG_PROCESS_ERROR(nRetCode);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfSummonNpc(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	CCharacter* poChar = NULL;
	CNpc* pNpc = NULL;
	INT32 nDirectionX, nDirectionY, nFaceDirection;
	INT64 llOffsetX, llOffsetY, llX, llY;

	if (pCtx->eOwnerType == botAI)
		poChar = pCtx->poChar;
	else if (pCtx->eOwnerType == botScene)
		poChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poChar);

	poScene = poChar->GetScene();
	LOG_PROCESS_ERROR(poScene);

	llOffsetX = CONVERT_LENGTH(nParam[1]);
	llOffsetY = CONVERT_LENGTH(nParam[2]);
	nDirectionY = poChar->GetFaceDirection();
	nDirectionX = (nDirectionY + (MAX_DIRECTION_COUNT * 3 / 4)) % MAX_DIRECTION_COUNT;
	llX = poChar->GetX() + llOffsetX * g_Cos(nDirectionX) / SIN_COS_NUMBER + llOffsetY * g_Cos(nDirectionY) / SIN_COS_NUMBER;
	llY = poChar->GetY() + llOffsetX * g_Sin(nDirectionX) / SIN_COS_NUMBER + llOffsetY * g_Sin(nDirectionY) / SIN_COS_NUMBER;
	nFaceDirection = poChar->GetFaceDirection() + CONVERT_ANGLE(nParam[3]);

	pNpc = poScene->SpawnNpc(nParam[0], llX, llY, poChar->GetZ(), nFaceDirection, 0, NULL);
	if (pNpc)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}

INT32 bfSummonDoodad(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CScene* poScene = NULL;
	CCharacter* poChar = NULL;
	CDoodad* pDoodad = NULL;
	INT32 nLevel, nDirectionX, nDirectionY, nFaceDirection;
	INT64 llX, llY;

	if (pCtx->eOwnerType == botAI)
		poChar = pCtx->poChar;
	else if (pCtx->eOwnerType == botScene)
		poChar = CGameWorld::GetInstance().FindValidCharacter(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poChar);

	poScene = poChar->GetScene();
	LOG_PROCESS_ERROR(poScene);

	nLevel = poChar->GetLevel();
	llX = CONVERT_LENGTH(nParam[1]);
	llY = CONVERT_LENGTH(nParam[2]);
	nDirectionY = poChar->GetFaceDirection();
	nDirectionX = (nDirectionY + (MAX_DIRECTION_COUNT * 3 / 4)) % MAX_DIRECTION_COUNT;
	llX = poChar->GetX() + llX * g_Cos(nDirectionX) / SIN_COS_NUMBER + llY * g_Cos(nDirectionY) / SIN_COS_NUMBER;
	llY = poChar->GetY() + llX * g_Sin(nDirectionX) / SIN_COS_NUMBER + llY * g_Sin(nDirectionY) / SIN_COS_NUMBER;
	nFaceDirection = poChar->GetFaceDirection() + CONVERT_ANGLE(nParam[3]);

	pDoodad = poScene->SpawnDoodad(nParam[0], llX, llY, poChar->GetZ(), nFaceDirection, 0);
	if (pDoodad)
		return brvSuccess;
	else
		return brvFail;
Exit0:
	return brvError;
}


INT32 bfCloseGuildDungeon(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32       nRetCode = 0;
	CScene      *poScene = NULL;

	if (pCtx->eOwnerType == botAI)
	{
		LOG_PROCESS_ERROR(pCtx->poChar);
		LOG_PROCESS_ERROR(pCtx->poChar->GetScene());
		poScene = pCtx->poChar->GetScene();
	}
	else if (pCtx->eOwnerType == botScene)
	{
		LOG_PROCESS_ERROR(pCtx->poScene);
		poScene = pCtx->poScene;
	}
	else
		LOG_PROCESS_ERROR(FALSE);

	nRetCode = CloseGuildDungeonCommission(poScene->GetSceneTempID(), poScene->GetSceneID());
	LOG_PROCESS_ERROR(nRetCode);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfGuildWarTime(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    UINT32  dwGuildID = 0;
    CRole   *poRole = NULL;
    CScene  *poScene = NULL;
    CGSGuild    *poGuild = NULL;

    if (pCtx->eOwnerType == botAI)
    {
        LOG_PROCESS_ERROR(pCtx->poChar);
        LOG_PROCESS_ERROR(pCtx->poChar->GetScene());
        //poScene = pCtx->poChar->GetScene();
        poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
        LOG_PROCESS_ERROR(poRole);
        dwGuildID = poRole->GetGuildID();
    }
    else if (pCtx->eOwnerType == botScene)
    {
        LOG_PROCESS_ERROR(pCtx->poScene);
        poScene = pCtx->poScene;
        dwGuildID = poScene->GetSceneID();
    }
    else
        LOG_PROCESS_ERROR(FALSE);

    poGuild = CGuildMgr::GetInstance().FindGuild(dwGuildID);
    LOG_PROCESS_ERROR(poGuild);
#ifndef _ROBOT
	nRetCode = poGuild->IsWarTime();
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;
#endif
Exit0:
    return brvError;
}

INT32 bfGuildEndWar(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32       nRetCode = 0;
	CScene      *poScene = NULL;
	GUILD_WAR_DATA  *pWarData = NULL;

	if (pCtx->eOwnerType == botAI)
	{
		LOG_PROCESS_ERROR(pCtx->poChar);
		LOG_PROCESS_ERROR(pCtx->poChar->GetScene());
		poScene = pCtx->poChar->GetScene();
	}
	else if (pCtx->eOwnerType == botScene)
	{
		LOG_PROCESS_ERROR(pCtx->poScene);
		poScene = pCtx->poScene;
	}
	else
		LOG_PROCESS_ERROR(FALSE);

#ifndef _ROBOT
    nRetCode = CCliMessageHandler::GetInstance().DoMSGuildEndWar(poScene->GetSceneID(), nParam[0]);
    LOG_PROCESS_ERROR(nRetCode);
#endif

    // Chen Kai said locking is no longer needed; after it ends, allow them to kill freely
    //nRetCode = GuildWarLockRole(poScene);
    //LOG_PROCESS_ERROR(nRetCode);

    // end
    if (poScene->GetStaticData() && poScene->GetStaticData()->GetBT(evtGuildWarEnd))
    {
        nRetCode = poScene->RunBT(evtGuildWarEnd, 0, poScene->GetStaticData()->GetBT(evtGuildWarEnd));
        LOG_CHECK_ERROR(nRetCode);
    }

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleJumpToTargetGuildScene(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32       nRetCode = 0;
	INT32       x = nParam[0];
	INT32       y = nParam[1];
	INT32       z = nParam[2];
	UINT32      dwSceneID = nParam[3];
	const SCENE_DATA      *pSceneData = NULL;
	CRole       *poRole = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar);
	LOG_PROCESS_ERROR(pCtx->poChar->GetScene());

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	pSceneData = CDataManager<SCENE_DATA>::GetInstance().GetData(g_GuildConfig.dwSceneTemplateID);
	LOG_PROCESS_ERROR(pSceneData);

	nRetCode = CGameWorld::GetInstance().JumpTo(poRole, g_GuildConfig.dwSceneTemplateID, dwSceneID,
		x * POINT_PER_CM, y * POINT_PER_CM, z * POINT_PER_CM, poRole->GetFaceDirection());
	LOG_PROCESS_ERROR(nRetCode);

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleCheckDailyVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	UINT32 dwVarValue = 0;
	UINT32 dwOldDay = 0;
	UINT32 dwToday = 0;
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar);

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	dwVarValue = poRole->GetVariable(nParam[0]);
	dwOldDay = (dwVarValue + CGameWorld::GetInstance().GetTimeZone()) / 86400;
	dwToday = (CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone()) / 86400;
	if (dwOldDay < dwToday - nParam[1])
		return brvSuccess;
	else
		return brvFail;

Exit0:
	return brvError;
}

INT32 bfRoleUpdateDailyVariable(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	LOG_PROCESS_ERROR(pCtx->poChar);

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (nParam[1] == 0)
	{
		poRole->SetVariable(nParam[0], CGameWorld::GetInstance().GetGameTime());
	}
	else
	{
		UINT32 dwVarValue = poRole->GetVariable(nParam[0]);
		if (dwVarValue < poRole->GetCreateTime())
			dwVarValue = poRole->GetCreateTime() - 86400;

		UINT32 dwOldWeek = (dwVarValue + CGameWorld::GetInstance().GetTimeZone() + 4 * 86400) / (86400 * 7);
		UINT32 dwCurrentWeek = (CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone() + 4 * 86400) / (86400 * 7);
		if (dwOldWeek < dwCurrentWeek)
			poRole->SetVariable(nParam[0], dwCurrentWeek * 86400 * 7 - 4 * 86400 - CGameWorld::GetInstance().GetTimeZone() + 43200);
		else if (dwOldWeek == dwCurrentWeek)
		{
			UINT32 dwOldDay = (dwVarValue + CGameWorld::GetInstance().GetTimeZone()) / 86400;
			UINT32 dwToday = (CGameWorld::GetInstance().GetGameTime() + CGameWorld::GetInstance().GetTimeZone()) / 86400;
			if (dwOldDay < dwToday)
				poRole->SetVariable(nParam[0], dwVarValue + 86400);
			else
				return brvFail;
		}
		else
			return brvFail;
	}

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleEnterCutScene(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	INT32 nDurationFrm = CONVERT_TIME(nParam[0]);

	LOG_PROCESS_ERROR(pCtx->poChar);

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	if (poRole->GetFlag().CheckFlag(cfInCombat))
		return brvFail;

	poRole->SetInCutScene(TRUE, nDurationFrm);

	return brvSuccess;
Exit0:
	return brvError;
}


INT32 bfRoleEnrollSabac(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	CGSGuild* poGuild = NULL;

#ifndef _ROBOT
	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	poGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
	if (poGuild == NULL)
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacNotGuildLeader, 0, 0);
		LOG_CHECK_ERROR(nRetCode);
		return brvFail;
	}

	if (poGuild->GetLeaderID() != poRole->GetObjID())
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacNotGuildLeader, 0, 0);
		LOG_CHECK_ERROR(nRetCode);
		return brvFail;
	}

	if (poGuild->IsOccupiedSabac())
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacOccupied, 0, 0);
		LOG_CHECK_ERROR(nRetCode);
		return brvFail;
	}

	if (poGuild->HasEnrolledSabac())
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacEnrolled, 0, 0);
		LOG_CHECK_ERROR(nRetCode);
		return brvFail;
	}

	if (g_GWConfig.Sabac.nEnrollItemID > 0 && g_GWConfig.Sabac.nEnrollItemCount > 0 &&
		poRole->GetItemBag()->GetItemCount(g_GWConfig.Sabac.nEnrollItemID) < g_GWConfig.Sabac.nEnrollItemCount)
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacNotEnoughItem, 0, 0);
		LOG_CHECK_ERROR(nRetCode);

		return brvFail;
	}

	nRetCode = poRole->GetItemBag()->SubMoney(
		MONEYTYPE_02, g_GWConfig.Sabac.nEnrollCost, MONEYLOG_ENROLL_SABAC, 0);
	if (nRetCode != errSuccess)
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacNotEnoughMoney, 0, 0);
		LOG_CHECK_ERROR(nRetCode);
		return brvFail;
	}

	if (g_GWConfig.Sabac.nEnrollItemID > 0 && g_GWConfig.Sabac.nEnrollItemCount > 0)
	{
		nRetCode = poRole->GetItemBag()->DeleteItemId(
			g_GWConfig.Sabac.nEnrollItemID, g_GWConfig.Sabac.nEnrollItemCount, ITEMLOG_ENROLL_SABAC, FALSE, 0);
		if (nRetCode != errSuccess)
		{
			LOG_CHECK_ERROR(FALSE);

			nRetCode = poRole->ErrorCodeNotify( errSabacNotEnoughItem, 0, 0);
			LOG_CHECK_ERROR(nRetCode);

			return brvFail;
		}
	}

	nRetCode = CCliMessageHandler::GetInstance().DoMSEnrollSabac(poGuild->GetID(), CGameWorld::GetInstance().GetGameTime());
	LOG_PROCESS_ERROR(nRetCode);

	return brvSuccess;
Exit0:
#endif	// _ROBOT

	return brvError;
}

INT32 bfRoleViewSabacEnrollList(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
#ifndef _ROBOT
	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	nRetCode = CCliMessageHandler::GetInstance().DoSyncSabacEnrollList(poRole);
	LOG_PROCESS_ERROR(nRetCode);

	return brvSuccess;
Exit0:
#endif	// _ROBOT

	return brvError;
}

INT32 bfRoleErectStatue(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	CGSGuild* poGSGuild = NULL;

#ifndef _ROBOT
	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	poGSGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
	if (poGSGuild == NULL)
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacErectStatue, 0, 0);
		LOG_CHECK_ERROR(nRetCode);

		return brvFail;
	}

	if (poRole->GetObjID() != poGSGuild->GetLeaderID())
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacErectStatue, 0, 0);
		LOG_CHECK_ERROR(nRetCode);

		return brvFail;
	}

	if (CSabacMgr::GetInstance().GetOccupyGuildID(poRole->GetServerGroup()) != poRole->GetGuildID())
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacErectStatue, 0, 0);
		LOG_CHECK_ERROR(nRetCode);

		return brvFail;
	}

	nRetCode = poRole->GetItemBag()->SubMoney(
		MONEYTYPE_02, g_GWConfig.Sabac.nErectStatusCost, MONEYLOG_ERECT_STATUS, 0);
	if (nRetCode != errSuccess)
	{
		nRetCode = poRole->ErrorCodeNotify( errSabacErectNotEnoughMoney, 0, 0);
		LOG_CHECK_ERROR(nRetCode);
		return brvFail;
	}

	nRetCode = CCliMessageHandler::GetInstance().DoMSErectStatue(poRole);
	LOG_PROCESS_ERROR(nRetCode);

	return brvSuccess;
Exit0:
#endif	// _ROBOT

	return brvError;
}

INT32 bfRoleAddDailyExtraExp(BTCustomCtx* pCtx, INT32 nParam[])
{
	const ROLE_LEVEL_DATA* pcLevelData = nullptr;
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	INT32 nAverageLevel = 0;
	INT32 nLevelDiff = 0;

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	pcLevelData = CDataManager<ROLE_LEVEL_DATA>::GetInstance().GetData(poRole->GetLevel());
	LOG_PROCESS_ERROR(pcLevelData);

	nAverageLevel = CGameWorld::GetInstance().GetAverageLevel(poRole->GetServerGroup());
	if (poRole->GetLevel() < nAverageLevel)
		nLevelDiff = nAverageLevel - poRole->GetLevel();

	poRole->AddExtraExp((INT64)pcLevelData->m_nDailyExtraExp * g_GWConfig.nLevelDiffExtraExpRate[nLevelDiff] / RATE_MAX);

	return brvSuccess;
Exit0:

	return brvError;
}

INT32 bfRoleCheckCDTimer(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	nRetCode = poRole->GetCoolDown().CheckReady(MakeCoolDownID(nParam[0], nParam[1]));
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;

Exit0:

	return brvError;
}

INT32 bfRoleSetCDTimer(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	INT32 nKey = nParam[0];
	INT32 nIndex = nParam[1];
	INT32 nInterval = nParam[2];
	INT32 nIntervalCeiling = nParam[3];

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	nInterval = CONVERT_TIME(nInterval);
	nIntervalCeiling = CONVERT_TIME(nIntervalCeiling);

	if (nIntervalCeiling > 0 && nIntervalCeiling >= nInterval)
	{
		nInterval = g_Random(nInterval, nIntervalCeiling);
	}

	nRetCode = poRole->GetCoolDown().Add(MakeCoolDownID(nKey, nIndex), nInterval);
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;

Exit0:

	return brvError;
}

INT32 bfRoleJumpToRevivePos(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	CRole* poRole = NULL;

	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	nRetCode = poRole->JumpToRevivePos();
	if (nRetCode)
		return brvSuccess;
	else
		return brvFail;

Exit0:

	return brvError;
}

INT32 bfRoleErrorCodeNotify(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nResult = 0;
	CRole* poRole = NULL;

#ifndef _ROBOT
	poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);

	nResult = poRole->ErrorCodeNotify( nParam[0], nParam[1], nParam[2]);
	LOG_PROCESS_ERROR(poRole);
#endif // _ROBOT

	return brvSuccess;
Exit0:
	return brvError;
}

INT32 bfRoleCheckGuild(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nResult = 0;
    CRole* poRole = NULL;
    CGSGuild    *poGuild = NULL;

    LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);

    poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
    LOG_PROCESS_ERROR(poRole);

    if (poRole->GetGuildID() == 0)
        return brvFail;

    poGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    if (poGuild == NULL)
        return brvFail;

    return brvSuccess;
Exit0:
    return brvError;
}

INT32 bfRoleCheckPosition(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32   nRetCode = 0;
    INT32 nResult = 0;
    CRole* poRole = NULL;
	CGSGuild    *poGuild = NULL;
#ifndef _ROBOT
	GS_GUILD_MEMBER *poMember = NULL;
#endif

    LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);

    poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
    LOG_PROCESS_ERROR(poRole);

    poGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    LOG_PROCESS_ERROR(poGuild);

#ifndef _ROBOT
    poMember = poGuild->GetMember(poRole->GetObjID());
    LOG_PROCESS_ERROR(poMember);

    switch (nParam[0])
    {
    case boLT:
        return _CheckOperator(boLT, nParam[1], poMember->nPosition);
    case boLTE:
        return _CheckOperator(boLTE, nParam[1], poMember->nPosition);
    case boGT:
        return _CheckOperator(boGT, nParam[1], poMember->nPosition);
    case boGTE:
        return _CheckOperator(boGTE, nParam[1], poMember->nPosition);
    case boEQ:
        return _CheckOperator(boEQ, nParam[1], poMember->nPosition);
    case boNEQ:
        return _CheckOperator(boNEQ, nParam[1], poMember->nPosition);
    default:
        LOG_PROCESS_ERROR(nRetCode);
        break;
    }

#endif
    return brvFail;
Exit0:
    return brvError;
}

INT32 bfRoleCheckGuildActivity(BTCustomCtx* pCtx, INT32 nParam[])
{
    INT32 nRetCode = 0;
    CRole* poRole = NULL;
    CGSGuild    *poGuild = NULL;

    LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);

    poRole = CGameWorld::GetInstance().FindRole(pCtx->dwRoleID);
    LOG_PROCESS_ERROR(poRole);

    poGuild = CGuildMgr::GetInstance().FindGuild(poRole->GetGuildID());
    LOG_PROCESS_ERROR(poGuild);
#ifndef _ROBOT
    nRetCode = poGuild->IsActivityOpen(nParam[0]);
    if (nRetCode == TRUE)
		return brvSuccess;
#endif

    return brvFail;
Exit0:
    return brvError;
}

INT32 bfRoleTemporaryCharge(BTCustomCtx* pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;

#ifndef _ROBOT
	nRetCode = CCliMessageHandler::GetInstance().DoMSTemporaryCharge(pCtx->dwRoleID);
	if (nRetCode == TRUE)
		return brvSuccess;
#endif

	return brvFail;
Exit0:
	return brvError;
}

INT32 bfRoleOpenDialog(BTCustomCtx* pCtx, INT32 nParam[])
{
#ifndef _ROBOT
	INT32 nRetCode = 0;
	CRole* poRole = NULL;
	CNpc* poNpc = NULL;
	const char* pcszString = NULL;
	char cszWords[MAX_CHAT_MESSAGE_LEN];

	INT32 nStringID = nParam[0];
	INT32 nTimeout = CONVERT_TIME(nParam[1]);

	if (nStringID > SERVER_STRING_START_ID) // server translate
	{
		pcszString = CGameWorld::GetInstance().GetString(nStringID);
		LOG_PROCESS_ERROR(pcszString);
	}
	else // client need
	{
		snprintf(cszWords, PATH_LEN, "<#P0:%d><#P1:%d><#T:%d>", nParam[2], nParam[3], nStringID);
		pcszString = cszWords;
	}

	poRole = CGameWorld::GetInstance().FindValidRole(pCtx->dwRoleID);
	LOG_PROCESS_ERROR(poRole);
	LOG_PROCESS_ERROR(poRole->GetScene());

	LOG_PROCESS_ERROR(pCtx->eOwnerType == botAI);
	LOG_PROCESS_ERROR(pCtx->poChar->GetObjType() == otNpc);
	poNpc = (CNpc*)pCtx->poChar;

	nRetCode = CCliMessageHandler::GetInstance().DoOpenDialog(poRole, poNpc->GetObjID(), pcszString);
	LOG_PROCESS_ERROR(nRetCode);

	pCtx->eRunningType = brtWaitSelect;
	pCtx->nWaitFrame = CGameWorld::GetInstance().GetGameFrame() + nTimeout;

	return brvRunning;

Exit0:
#endif  // _ROBOT
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_TO_JUMP
{
	bool operator()(CRole* poRole)
	{
		if (poRole)
		{
			poRole->JumpToReturnPos();
		}
		return TRUE;
	};
};

INT32 bfTeleportSceneRoleToReturnPos(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32   nRetCode = 0;
	CScene      *poScene = NULL;
	GUILD_WAR_DATA  *pWarData = NULL;
	TRAVERSE_SCENE_ROLE_TO_JUMP     JumpReturnPos;

	LOG_PROCESS_ERROR(pCtx->eOwnerType == botScene);
	LOG_PROCESS_ERROR(pCtx->poScene);
	poScene = pCtx->poScene;

	nRetCode = poScene->TraverseRole(JumpReturnPos);
	if (!nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

struct TRAVERSE_GUILD_ROLE_TO_JUMP
{
	bool operator()(CRole* poRole)
	{
		if (poRole && poRole->GetScene())
		{
			if (poRole->GetScene()->GetSceneID() != poRole->GetGuildID())
			{
				poRole->JumpToReturnPos();
			}
		}
		return TRUE;
	};
};

INT32 bfTpOtherGuildRoleToReturnPos(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32   nRetCode = 0;
	CScene      *poScene = NULL;
	GUILD_WAR_DATA  *pWarData = NULL;
	TRAVERSE_GUILD_ROLE_TO_JUMP     JumpReturnPos;

	LOG_PROCESS_ERROR(pCtx->eOwnerType == botScene);
	LOG_PROCESS_ERROR(pCtx->poScene);
	poScene = pCtx->poScene;

	nRetCode = poScene->TraverseRole(JumpReturnPos);
	if (!nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_TO_SET_FACTION
{
	INT32       nFaction;
	bool operator()(CRole* poRole)
	{
		if (poRole)
		{
			poRole->SetFaction(nFaction);
		}
		return TRUE;
	};
};

INT32 bfSceneRoleSetFaction(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	TRAVERSE_SCENE_ROLE_TO_SET_FACTION  setFaction;

	LOG_PROCESS_ERROR(pCtx->eOwnerType == botScene);
	LOG_PROCESS_ERROR(pCtx->poScene);

	setFaction.nFaction = nParam[0];

	nRetCode = pCtx->poScene->TraverseRole(setFaction);
	if (!nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

struct TRAVERSE_SCENE_ROLE_ARENA_DEFEAT
{
	bool operator()(CRole* poRole)
	{
		INT32       nRetCode = 0;

		if (poRole)
		{
			nRetCode = poRole->GetArena().Defeated();
			LOG_CHECK_ERROR(nRetCode);
		}
		return TRUE;
	};
};

INT32 bfSceneEndArena(BTCustomCtx *pCtx, INT32 nParam[])
{
	INT32 nRetCode = 0;
	TRAVERSE_SCENE_ROLE_ARENA_DEFEAT  arenaDefeat;

	LOG_PROCESS_ERROR(pCtx->eOwnerType == botScene);
	LOG_PROCESS_ERROR(pCtx->poScene);

	nRetCode = pCtx->poScene->TraverseRole(arenaDefeat);
	if (!nRetCode)
		return brvFail;

	return brvSuccess;
Exit0:
	return brvError;
}

#define REG_BT_FUNC(__type__, __func__, __owner_mask__)   \
{ __type__, #__type__, __func__, (INT16)__owner_mask__}

CBTManager::ACTION_FUNC_ENTRY CBTManager::ms_ActionFuncList[btTotal] = 
{
	/************************************************************************/
	/* Common                                                               */
	/************************************************************************/
	
	REG_BT_FUNC(batInvalid,                     NULL,                               0),
	
	// decider
	REG_BT_FUNC(batSelector,                    NULL,                               botAll),
	REG_BT_FUNC(batSequencer,                   NULL,                               botAll),
	REG_BT_FUNC(batParallel,                    NULL,                               botAll),
	REG_BT_FUNC(batNot,                         NULL,                               botAll),
	REG_BT_FUNC(batDebug,                       NULL,                               botAll),
	REG_BT_FUNC(batRandom,                      NULL,                               botAll),
	REG_BT_FUNC(batRepeat,                      NULL,                               botAll),
	REG_BT_FUNC(batBegin,                       NULL,                               botAll),
	
	REG_BT_FUNC(batReturnSuccess,               bfReturnSuccess,                    botAll),
	REG_BT_FUNC(batReturnFail,                  bfReturnFail,                       botAll),
	REG_BT_FUNC(batPrint,                       bfPrint,                            botAll),
	
	REG_BT_FUNC(batCheckDice,                   bfCheckDice,                        botAll),
	REG_BT_FUNC(batCheckSceneVariable,          bfCheckSceneVariable,               botAll),
	REG_BT_FUNC(batCheckServerVariable,			bfCheckServerVariable,				botAll),
	REG_BT_FUNC(batCheckDateTime,				bfCheckDateTime,					botAll),
	REG_BT_FUNC(batCheckWeather,                bfCheckWeather,                     botAll),
	REG_BT_FUNC(batCompareSceneVariable,        bfCompareSceneVariable,             botAll),
	REG_BT_FUNC(batCheckDoorState,              bfCheckDoorState,                   botAll),
	
	REG_BT_FUNC(batWait,                        bfWait,                             botAll),
	REG_BT_FUNC(batOpenDoor,                    bfOpenDoor,                         botAll),
	REG_BT_FUNC(batCloseDoor,                   bfCloseDoor,                        botAll),
	REG_BT_FUNC(batToggleDoor,                  bfToggleDoor,                       botAll),
	REG_BT_FUNC(batSay,                         bfSay,                              botAll),
	REG_BT_FUNC(batSetSceneVariable,            bfSetSceneVariable,                 botAll),
	REG_BT_FUNC(batSetServerVariable,			bfSetServerVariable,				botAll),
	REG_BT_FUNC(batSpawnNpc,                    bfSpawnNpc,                         botAll),
	REG_BT_FUNC(batSpawnNpcAtPoint,             bfSpawnNpcAtPoint,                  botAll),
	REG_BT_FUNC(batSpawnSingleNpc,              bfSpawnSingleNpc,                   botAll),
	REG_BT_FUNC(batSpawnMediumGroup,            bfSpawnMediumGroup,                 botAll),
	REG_BT_FUNC(batSpawnLargeGroup,             bfSpawnLargeGroup,                  botAll),
	
	REG_BT_FUNC(batSpawnDoodad,                 bfSpawnDoodad,                      botAll),
	REG_BT_FUNC(batSpawnDoodadAtPoint,          bfSpawnDoodadAtPoint,               botAll),
	REG_BT_FUNC(batSpawnDoodadGroup,            bfSpawnDoodadGroup,                 botAll),
	
	REG_BT_FUNC(batCheckSceneRuning,            bfCheckSceneRuning,                 botAll),
	REG_BT_FUNC(batFinishScene,                 bfFinishScene,                      botAll),
	REG_BT_FUNC(batCalculateDungeonScore,       bfCalculateDungeonScore,            botAll),
	
	REG_BT_FUNC(batSetReputationToFaction,      bfSetReputationToFaction,           botAll),
	REG_BT_FUNC(batAddMoneyToFaction,           bfAddMoneyToFaction,                botAll),
	REG_BT_FUNC(batSubMoneyToFaction,           bfSubMoneyToFaction,                botAll),
	REG_BT_FUNC(batAddAuraToFaction,            bfAddAuraToFaction,                 botAll),
	REG_BT_FUNC(batRemoveAuraToFaction,         bfRemoveAuraToFaction,              botAll),
	REG_BT_FUNC(batAddItemToFaction,            bfAddItemToFaction,                 botAll),
	REG_BT_FUNC(batSetCombatStatToFaction,      bfSetCombatStatToFaction,           botAll),
	REG_BT_FUNC(batGuildEndWar,                 bfGuildEndWar,                      botAll),
	REG_BT_FUNC(batCloseGuildDungeon,           bfCloseGuildDungeon,                botAll),
    REG_BT_FUNC(batGuildWarTime,                bfGuildWarTime,                     botAll),
	
	/************************************************************************/
	/* Role                                                                 */
	/************************************************************************/
	REG_BT_FUNC(batRoleCheckRoleVariable,       bfRoleCheckRoleVariable,            botAll),
	REG_BT_FUNC(batRoleCheckGuildVariable,      bfRoleCheckGuildVariable,           botAll),
	REG_BT_FUNC(batRoleCheckHasQuest,           bfRoleCheckHasQuest,                botAll),
	REG_BT_FUNC(batRoleCheckQuestVariable,      bfRoleCheckQuestVariable,           botAll),
	REG_BT_FUNC(batRoleCheckFaction,            bfRoleCheckFaction,                 botAll),
	REG_BT_FUNC(batCheckInTeam,					bfRoleCheckInTeam,                  botAll),
	REG_BT_FUNC(batRoleCheckInteractOption,     bfRoleCheckInteractOption,          botAll),
	REG_BT_FUNC(batRoleCheckVIP,                bfRoleCheckVIP,                     botAll),
	REG_BT_FUNC(batRoleCheckLevel,				bfRoleCheckLevel,					botAll),
	REG_BT_FUNC(batRoleIsSabacGuildMember,		bfRoleIsSabacGuildMember,			botAll),
	REG_BT_FUNC(batRoleIsSabacGuildLeader,		bfRoleIsSabacGuildLeader,			botAll),

	REG_BT_FUNC(batRoleSetRoleVariable,         bfRoleSetRoleVariable,              botAll),
	REG_BT_FUNC(batRoleSetGuildVariable,        bfRoleSetGuildVariable,             botAll),
	REG_BT_FUNC(batRoleSetQuestVariable,        bfRoleSetQuestVariable,             botAll),
	REG_BT_FUNC(batRoleSetCombatStat,           bfRoleSetCombatStat,                botAll),
	REG_BT_FUNC(batRoleTeleportToPoint,         bfRoleTeleportToPoint,              botAll),
	REG_BT_FUNC(batRoleTeleport,                bfRoleTeleport,                     botAll),
	REG_BT_FUNC(batRoleReturnStaticScene,       bfRoleReturnStaticScene,            botAll),
	REG_BT_FUNC(batRoleSetReputation,           bfRoleSetReputation,                botAll),
    REG_BT_FUNC(batRoleCheckMoney,              bfRoleCheckMoney,                   botAll),
	REG_BT_FUNC(batRoleAddMoney,                bfRoleAddMoney,                     botAll),
	REG_BT_FUNC(batRoleSubMoney,                bfRoleSubMoney,                     botAll),
    REG_BT_FUNC(batRoleCheckItemCount,          bfRoleCheckItemCount,               botAll),
	REG_BT_FUNC(batRoleAddItem,                 bfRoleAddItem,                      botAll),
    REG_BT_FUNC(batRoleDelItem,                 bfRoleDelItem,                      botAll),
	REG_BT_FUNC(batRoleOpenDialog,              bfRoleOpenDialog,                   botAll),
	REG_BT_FUNC(batRoleLeaveTeam,               bfRoleLeaveTeam,                    botAll),
	REG_BT_FUNC(batRoleSetFaction,              bfRoleSetFaction,                   botAll),
	REG_BT_FUNC(batRoleCheckSelfScene,          bfRoleCheckSelfScene,               botAll),
    REG_BT_FUNC(batRoleCheckAllianceScene,      bfRoleCheckAllianceScene,           botAll), 
	REG_BT_FUNC(batRoleJumpToTargetGuildScene,  bfRoleJumpToTargetGuildScene,       botAll),
	REG_BT_FUNC(batRoleCheckDailyVariable,      bfRoleCheckDailyVariable,           botAll),
	REG_BT_FUNC(batRoleUpdateDailyVariable,     bfRoleUpdateDailyVariable,          botAll),
	REG_BT_FUNC(batRoleEnterCutScene,			bfRoleEnterCutScene,		        botAll),
	REG_BT_FUNC(batRoleEnrollSabac, 			bfRoleEnrollSabac,					botAll),
	REG_BT_FUNC(batRoleViewSabacEnrollList,		bfRoleViewSabacEnrollList,			botAll),
	REG_BT_FUNC(batRoleErectStatue, 			bfRoleErectStatue,					botAll),
	REG_BT_FUNC(batRoleAddDailyExtraExp,		bfRoleAddDailyExtraExp,				botAll),
	REG_BT_FUNC(batRoleCheckCDTimer,			bfRoleCheckCDTimer,					botAll),
	REG_BT_FUNC(batRoleSetCDTimer,				bfRoleSetCDTimer,					botAll),
	REG_BT_FUNC(batRoleJumpToRevivePos,			bfRoleJumpToRevivePos,				botAll),
	REG_BT_FUNC(batRoleErrorCodeNotify,			bfRoleErrorCodeNotify,				botAll),
    REG_BT_FUNC(batRoleCheckGuild,              bfRoleCheckGuild,                   botAll),
    REG_BT_FUNC(batRoleCheckPosition,           bfRoleCheckPosition,                botAll),
    REG_BT_FUNC(batRoleCheckGuildActivity,      bfRoleCheckGuildActivity,           botAll),
	REG_BT_FUNC(batRoleTemporaryCharge,			bfRoleTemporaryCharge,				botAll),
	

	/************************************************************************/
	/* Char                                                                 */
	/************************************************************************/
	REG_BT_FUNC(batCheckCharMoveState,          bfCheckCharMoveState,               botAll),
	REG_BT_FUNC(batCheckDirection,              bfCheckDirection,                   botAll),
	REG_BT_FUNC(batCheckCharHp,                 bfCheckCharHp,                      botAll),
	REG_BT_FUNC(batAddAura,                     bfAddAura,                          botAll),    
	REG_BT_FUNC(batReduceAura,                  bfReduceAura,                       botAll),   
	REG_BT_FUNC(batRemoveAura,                  bfRemoveAura,                       botAll),  
	REG_BT_FUNC(batCopyAura,                    bfCopyAura,                         botAll),
	REG_BT_FUNC(batCheckAura,                   bfCheckAura,                        botAll),
	REG_BT_FUNC(batRotateTo,                    bfRotateTo,                         botAll),
	REG_BT_FUNC(batSyncRotateTo,                bfSyncRotateTo,                     botAll),
	REG_BT_FUNC(batMoveTo,                      bfMoveTo,                           botAll),
	REG_BT_FUNC(batSyncMoveTo,                  bfSyncMoveTo,                       botAll),
	REG_BT_FUNC(batTeleportTo,                  bfTeleportTo,                       botAll),
	REG_BT_FUNC(batSuicide,                     bfSuicide,                          botAll),
	REG_BT_FUNC(batSetCharHp,                   bfSetCharHp,                        botAll),
	REG_BT_FUNC(batSendMail,                    bfSendMail,                         botAll),
	REG_BT_FUNC(batSummonNpc,                   bfSummonNpc,                        botAll),
	REG_BT_FUNC(batSummonDoodad,                bfSummonDoodad,                     botAll),
	REG_BT_FUNC(batSendAwardInfo,				bfSendAwardInfo,		            botAll),
	REG_BT_FUNC(batDistributeExp,				bfDistributeExp,					botAll),
    REG_BT_FUNC(batClearThreatList,             bfClearThreatList,                  botAll),
    REG_BT_FUNC(batAddExp,						bfAddExp,							botAll),


	/************************************************************************/
	/* AI                                                                   */
	/************************************************************************/
	REG_BT_FUNC(batRecvMsg,						bfRecvMsg,							botAIBit),
	REG_BT_FUNC(batCheckCCFlag,					bfCheckCCFlag,						botAIBit),
	REG_BT_FUNC(batCheckFlag,                   bfCheckFlag,                        botAIBit),
	REG_BT_FUNC(batCheckInCombat,               bfCheckInCombat,                    botAIBit),
	REG_BT_FUNC(batCheckHostInCombat,           bfCheckHostInCombat,                botAIBit),
	REG_BT_FUNC(batCheckCharOutOfHomeRange,     bfCheckCharOutOfHomeRange,          botAIBit),
	REG_BT_FUNC(batCheckCharHpMeet,             bfCheckCharHpMeet,                  botAIBit),
	REG_BT_FUNC(batCheckCharCasting,            bfCheckCharCasting,                 botAIBit),
	REG_BT_FUNC(batCheckBossState,              bfCheckBossState,                   botAIBit),
	REG_BT_FUNC(batCheckDistance,               bfCheckDistance,                    botAIBit),
	REG_BT_FUNC(batCheckCDTimer,                bfCheckCDTimer,                     botAIBit),
	REG_BT_FUNC(batCheckAIVariable,             bfCheckAIVariable,                  botAIBit),
	REG_BT_FUNC(batCheckHeroWeaponEquipped,     bfCheckHeroWeaponEquipped,          botAIBit),
	REG_BT_FUNC(batCheckTarget,                 bfCheckTarget,                      botAIBit),
    REG_BT_FUNC(batCheckTargetType,             bfCheckTargetType,                  botAIBit),
	
	REG_BT_FUNC(batWander,                      bfWander,                           botAIBit),
	REG_BT_FUNC(batGoHome,                      bfGoHome,                           botAIBit),
	REG_BT_FUNC(batSetHome,                     btSetHome,                          botAIBit),
	REG_BT_FUNC(batFlee,                        bfFlee,                             botAIBit),
	REG_BT_FUNC(batChase,                       bfChase,                            botAIBit),
	REG_BT_FUNC(batWithdraw,                    bfWithdraw,                         botAIBit),
	REG_BT_FUNC(batSetFollowParam,              bfSetFollowParam,                   botAIBit),
	REG_BT_FUNC(batFollow,                      bfFollow,                           botAIBit),
	REG_BT_FUNC(batInvincible,					bfInvincible,	                    botAIBit),
	
	REG_BT_FUNC(batEnterIdle,                   bfEnterIdle,                        botAIBit),
	REG_BT_FUNC(batEnterFight,                  bfEnterFight,                       botAIBit),
	REG_BT_FUNC(batCastSpell,                   bfCastSpell,                        botAIBit),
	
	REG_BT_FUNC(batSetTarget,                   bfSetTarget,                        botAIBit),
	REG_BT_FUNC(batSetRoleAsTarget,             bfSetRoleAsTarget,                  botAIBit),
	REG_BT_FUNC(batSetTargetAsRole,             bfSetTargetAsRole,                  botAIBit),
	REG_BT_FUNC(batSetLastCaster,               bfSetLastCaster,                    botAIBit),
	
	REG_BT_FUNC(batSetScanEnemyParam,           bfSetScanEnemyParam,                botAIBit),
	REG_BT_FUNC(batSetScanFriendParam,          bfSetScanFriendParam,               botAIBit),
	REG_BT_FUNC(batSetScanCorpseParam,          bfSetScanCorpseParam,               botAIBit),
	REG_BT_FUNC(batSetScanAuraCharParam,        bfSetScanAuraCharParam,             botAIBit),
	REG_BT_FUNC(batScanEnemy,                   bfScanEnemy,                        botAIBit),
	REG_BT_FUNC(batScanNearby,                  bfScanNearby,                       botAIBit),
	REG_BT_FUNC(batScanRedName,                 bfScanRedName,                      botAIBit),
	REG_BT_FUNC(batScanSabacWinner,             bfScanSabacWinner,                  botAIBit),
	REG_BT_FUNC(batScanSabacEnemy,              bfScanSabacEnemy,					botAIBit),
	
	REG_BT_FUNC(batGetTargetFromThreatList,     bfGetTargetFromThreatList,          botAIBit),
    REG_BT_FUNC(batPetGetTargetFromThreatList,  bfPetGetTargetFromThreatList,       botAIBit),
	REG_BT_FUNC(batScanHostThreatList,          bfScanHostThreatList,               botAIBit),
	
	REG_BT_FUNC(batAddTargetThreat,             bfAddTargetThreat,                  botAIBit),
	REG_BT_FUNC(batSetBossState,                bfSetBossState,                     botAIBit),
	REG_BT_FUNC(batSetFlag,                     bfSetFlag,                          botAIBit),
	REG_BT_FUNC(batClearFlag,                   bfClearFlag,                        botAIBit),
	REG_BT_FUNC(batPlayAction,					bfPlayAction,						botAIBit),
	REG_BT_FUNC(batSetCDTimer,                  bfSetCDTimer,                       botAIBit),
	REG_BT_FUNC(batSetAIVariable,               bfSetAIVariable,                    botAIBit),
	REG_BT_FUNC(batStepAroundTarget,            bfStepAroundTarget,                 botAIBit),
	REG_BT_FUNC(batCheckNeedChase,              bfCheckNeedChase,                   botAIBit),
	REG_BT_FUNC(batCheckNeedWithdraw,           bfCheckNeedWithdraw,                botAIBit),
	REG_BT_FUNC(batCheckCanMove,                bfCheckCanMove,                     botAIBit),
	REG_BT_FUNC(batCheckCanCastSpell,           bfCheckCanCastSpell,                botAIBit),
	REG_BT_FUNC(batCastSpellToTarget,           bfCastSpellToTarget,                botAIBit),
	REG_BT_FUNC(batCastSlotSpellToTarget,       bfCastSlotSpellToTarget,            botAIBit),
	REG_BT_FUNC(batCastSpellToPos,              bfCastSpellToPos,                   botAIBit),
	REG_BT_FUNC(batCastSpellToTargetPos,        bfCastSpellToTargetPos,             botAIBit),
	REG_BT_FUNC(batCastSpellToDir,              bfCastSpellToDir,                   botAIBit),
	REG_BT_FUNC(batCastSpellToTargetDir,        bfCastSpellToTargetDir,             botAIBit),
	REG_BT_FUNC(batSyncCastSpellToTarget,       bfCastSpellToTargetWait,            botAIBit),
	REG_BT_FUNC(batSyncCastSpellToPos,          bfCastSpellToPosWait,               botAIBit),
	REG_BT_FUNC(batSyncCastSpellToTargetPos,    bfCastSpellToTargetPosWait,         botAIBit),
	REG_BT_FUNC(batSyncCastSpellToDir,          bfCastSpellToDirWait,               botAIBit),
	REG_BT_FUNC(batSyncCastSpellToTargetDir,    bfCastSpellToTargetDirWait,         botAIBit),
	REG_BT_FUNC(batSyncCastSlotSpellToTarget,   bfCastSlotSpellToTargetWait,        botAIBit),
	REG_BT_FUNC(batWaitCastSpell, 				bfWaitCastSpell,					botAIBit),
	REG_BT_FUNC(batSyncBreakCast,	   	        bfBreakCast,                        botAIBit),
	
	REG_BT_FUNC(batIssueSignal,                 bfIssueSignal,                      botAIBit),
	REG_BT_FUNC(batPickSignal,                  bfPickSignal,                       botAIBit),
	REG_BT_FUNC(batGetTargetFromSignal,         bfGetTargetFromSignal,              botAIBit),
	REG_BT_FUNC(batSetPatrolParam,              bfSetPatrolParam,                   botAIBit),
	REG_BT_FUNC(batPatrol,                      bfPatrol,                           botAIBit),
	REG_BT_FUNC(batAddThreatListFromSignal,     bfAddThreatListFromSignal,          botAIBit),
	REG_BT_FUNC(batCheckIntValue,			    bfCheckIntValue,		            botAIBit),
	REG_BT_FUNC(batGBuildingDead,               bfGBuildingDead,                    botAIBit),
    REG_BT_FUNC(batCheckHost,                   bfCheckHost,                        botAIBit),
    REG_BT_FUNC(batMagePetCastSpellToTarget,    bfMagePetCastSpellToTarget,         botAIBit),
    REG_BT_FUNC(batPetScanEnemy,                bfPetScanEnemy,                     botAIBit),
    REG_BT_FUNC(batJumpToHost,                  bfJumpToHost,                       botAIBit),
    REG_BT_FUNC(batPetFollow,                   bfPetFollow,                        botAIBit),
    REG_BT_FUNC(batPetCheckHostDirection,       bfPetCheckHostDirection,            botAIBit),
    REG_BT_FUNC(batCheckThreatList,             bfCheckThreatList,                  botAIBit),
    REG_BT_FUNC(batPetChangeChaseTarget,        bfPetChangeChaseTarget,             botAIBit),
    REG_BT_FUNC(batPetChangeFightTarget,        bfPetChangeFightTarget,             botAIBit),
    REG_BT_FUNC(batPetGetCaster,                bfPetGetCaster,                     botAIBit),
    REG_BT_FUNC(batCheckThreatFadeTime,         bfCheckThreatFadeTime,              botAIBit),
    REG_BT_FUNC(batCheckThreatenFadeTime,       bftCheckThreatenFadeTime,           botAIBit),
    REG_BT_FUNC(batPetWander,                   bfPetWander,                        botAIBit),
    REG_BT_FUNC(batPetCheckValidTarget,         bfPetCheckValidTarget,              botAIBit),
    REG_BT_FUNC(batPetCheckNeedChase,           bfPetCheckNeedChase,                botAIBit),
    REG_BT_FUNC(batPetChase,                    bfPetChase,                         botAIBit),
    REG_BT_FUNC(batPetCheckRoleState,           bfPetCheckRoleState,                botAIBit),
    REG_BT_FUNC(batPetStopMove,                 bfPetStopMove,                      botAIBit),
    REG_BT_FUNC(batPetDelSbFromThreatenList,    bfPetDelSbFromThreatenList,         botAIBit),
    REG_BT_FUNC(batPetDelSbFromThreatList,      bfPetDelSbFromThreatList,           botAIBit),
    REG_BT_FUNC(batPetCheckHostTarget,          bfPetCheckHostTarget,               botAIBit),
    REG_BT_FUNC(batPetClearHostTarget,          bfPetClearHostTarget,               botAIBit),
	REG_BT_FUNC(batSetAvoidGroundEffect,		bfSetAvoidGroundEffect,				botAIBit),
    REG_BT_FUNC(batChangeGuildResource,         bfChangeGuildResource,              botAIBit),
    REG_BT_FUNC(batSetAlwaysRun,				bfSetAlwaysRun,						botAll),
	
	
	/************************************************************************/
	/* Scene                                                                */
	/************************************************************************/
	REG_BT_FUNC(batTeleportSceneRoleToReturnPos,bfTeleportSceneRoleToReturnPos,     botSceneBit),
	REG_BT_FUNC(batTpOtherGuildRoleToReturnPos, bfTpOtherGuildRoleToReturnPos,      botSceneBit),
	REG_BT_FUNC(batSceneRoleSetFaction,         bfSceneRoleSetFaction,              botSceneBit),
	REG_BT_FUNC(batSceneEndArena,               bfSceneEndArena,                    botSceneBit),

	{ 0, 0},
};
