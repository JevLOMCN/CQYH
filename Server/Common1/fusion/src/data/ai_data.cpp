#include "stdafx.h"
#include "ai_data.h"

#include "scene_def.h"
#include "attribute_def.h"
#include "math_ex.h"
#include "read_data_helper.h"
#include "script_mgr.h"

static constexpr const char* s_NewSpellName[nastTotal] =
{
    "InvalidSpell",
    "NormalSpell1",
    "NormalSpell2",
    "HelpFriendSpell1",
    "HelpFriendSpell2",
    "CorpseSpell",
    "SignalIssuerSpell",
    "SignalTargetSpell",
    "NormalSpell3",
    "NormalSpell4",
    "EnterFightSpell",
    "ChaseSpell",
    "InitSpell1",
    "InitSpell2",
    "InitSpell3",
    "DeadSpell",
    "IdleSpell"
};

BOOL NEW_AI_DATA::ReadLine(INT32& rnID, const char*& rpcszName, NEW_AI_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
    READ_START();

    READ_INT("AIID", nAIID);
    READ_STR("ScriptName", szScriptName);

    READ_SPEED("WalkSpeed", nWalkSpeed);
	READ_SPEED("RunSpeed", nRunSpeed);
    pData->nRotateSpeed = 180;
    READ_INT("Patrol", bPatrol);
    READ_INT("Follow", bFollow);
	READ_INT("CanRun", bCanRun);

///	READ_INT("AlwaysRun", bAlwaysRun);

    READ_INT("ScanEnemyRange", nScanEnemyRange);
    READ_INT("ScanFriendRange", nScanFriendRange);
    READ_INT("ChaseRange", nChaseRange);
    READ_INT("PreferDistance", nPreferDistance);
    READ_INT("WithdrawThreshold", nWithdrawThreshold);
    READ_INT("ChaseThreshold", nChaseThreshold);
    READ_INT("ChaseDelayTime", nChaseDelayTime);

    READ_INT("InitAuraID1", nInitAuraID[0]);
    READ_INT("InitAuraID2", nInitAuraID[1]);
    READ_INT("InitAuraID3", nInitAuraID[2]);
    READ_INT("LiveTime", nLiveTime);
	READ_INT("DisappearOnTimeout", bDisappearOnTimeout);

    READ_INT("WanderRange", nWanderRange);
    READ_INT("WanderInterval", nWanderInterval);

    for (INT32 nIndex = nastNormalSpell1; nIndex < nastTotal; nIndex++)
    {
        char szField[256];

        snprintf(szField, 256, "%s", s_NewSpellName[nIndex]);
        READ_INT(szField, nSpellID[nIndex]);

        snprintf(szField, 256, "%sChance", s_NewSpellName[nIndex]);
        READ_INT(szField, nSpellChance[nIndex]);

        snprintf(szField, 256, "%sCDTime", s_NewSpellName[nIndex]);
        READ_INT(szField, nSpellCDTime[nIndex]);

        pData->nSelfHpPercent[nIndex] = RATE_MAX;
    }

    READ_INT("GlobalCDTime", nGlobalCDTime);
    READ_INT("SelfHpPercent1", nSelfHpPercent[nastNormalSpell1]);
    READ_INT("SelfHpPercent2", nSelfHpPercent[nastNormalSpell2]);
    READ_INT("SelfHpPercent3", nSelfHpPercent[nastNormalSpell3]);
    READ_INT("SelfHpPercent4", nSelfHpPercent[nastNormalSpell4]);
    READ_INT("FriendlyNpcTemplateID", nFriendlyNpcTemplateID);
    READ_INT("FriendlyCorpse", bFriendlyCorpse);
    READ_INT("CorpseTemplateID", nCorpseTemplateID);
    READ_INT("IssuerSpellSignalType", nIssuerSpellSignalType);
    READ_INT("IssuerSpellSignalRange", nIssuerSpellSignalRange);
    READ_INT("TargetSpellSignalType", nTargetSpellSignalType);
    READ_INT("TargetSpellSignalRange", nTargetSpellSignalRange);

    READ_INT("FleeHpPercent", nFleeHpPercent);
    READ_INT("FleeChance", nFleeChance);
    READ_INT("FleeDuration", nFleeDuration);

    READ_INT("StepAroundChance", nStepAroundChance);
    READ_INT("StepAroundCDTime", nStepAroundCDTime);
    READ_INT("ChangeTargetChance", nChangeTargetChance);
    READ_INT("ChangeTargetCDTime", nChangeTargetCDTime);
    READ_INT("SpreadThreatCount", nSpreadThreatCount);

    READ_INT("Param0", nParam[0]);
    READ_INT("Param1", nParam[1]);
    READ_INT("Param2", nParam[2]);
    READ_INT("Param3", nParam[3]);
    READ_INT("Param4", nParam[4]);
    READ_INT("Param5", nParam[5]);
    READ_INT("Param6", nParam[6]);
    READ_INT("Param7", nParam[7]);

    READ_END(nAIID);
}
