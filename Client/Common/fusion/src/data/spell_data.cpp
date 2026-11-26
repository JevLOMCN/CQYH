#include "stdafx.h"
#include "spell_data.h"
#include "string_ex.h"
#include "spell_def.h"


BOOL SPELL_PROGRESS_DATA::ReadLine(INT32& rnID, const char*& rpcszName, SPELL_PROGRESS_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();

	READ_INT("SpellID", m_nID);
	READ_INT("Level", m_nLevel);
	READ_INT("ConsumePts", m_nConsumePts);
	READ_INT("RequireLevel", m_nRequireLevel);
	READ_INT("RequireItem", m_nRequireItem);
	READ_INT("RequireItemCount", m_nRequireItemCount);
	READ_INT("Pow", m_nPow);

	rnID = MakeSpellProgressKey(pData->m_nID, pData->m_nLevel);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL TRASFORM_DATA::ReadLine(INT32& rnID, const char*& rpcszName, TRASFORM_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();

	memset(pData, 0, sizeof(TRASFORM_DATA));

	READ_INT("AuraID", m_nAuraID);

	READ_INT("qssKey1", m_nEquipSpell[qssKey1]);
	READ_INT("qssKey2", m_nEquipSpell[qssKey2]);
	READ_INT("qssKey3", m_nEquipSpell[qssKey3]);
	READ_INT("qssKey4", m_nEquipSpell[qssKey4]);
	READ_INT("qssMouseL", m_nEquipSpell[qssKey10]);
	READ_INT("qssMouseR", m_nEquipSpell[qssKey11]);

	rnID = pData->m_nAuraID;

	return TRUE;
Exit0:
	return FALSE;
}

BOOL SPELL_VAR::ReadLine(INT32& rnID, const char*& rpcszName, SPELL_VAR* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();

	char szVariable[128] = { 0 };
	BOOL bIsX = FALSE;

	memset(pData->m_nVariableTable, 0, sizeof(pData->m_nVariableTable));
	memset(pData->m_nX, 0, sizeof(pData->m_nX));

	READ_INT("ID", m_nID);
	READ_INT("Level", m_nLevel);
	READ_INT("RuneSlotID", m_nRuneSlotID);

	static const char* szVariableColumn[] = { "Variable1", "Variable2", "Variable3", "Variable4" };
	static const char* szValueColumn[] = { "Value1", "Value2", "Value3", "Value4" };
	static const char* szXName[] = { "X1", "X2", "X3", "X4" };

	for (INT32 i = 0; i < MAX_EX_PARAM_COUNT;)
	{
		bIsX = FALSE;
		memset(szVariable, 0, sizeof(szVariable));

		nRetCode = pTabConfig->ReadData(szVariableColumn[i], nRow, szVariable, sizeof(szVariable) / sizeof(char));
		LOG_PROCESS_ERROR(nRetCode);

		for (INT32 j = 0; j < MAX_EX_PARAM_COUNT; j++)
		{
			if (strcmp(szVariable, szXName[j]) != 0)
				continue;

			nRetCode = READ_INT_EXPR(szValueColumn[i], m_nTempValue);
			LOG_PROCESS_ERROR(nRetCode);

			pData->m_nX[j] = pData->m_nTempValue;
			bIsX = TRUE;
			break;
		}

		if (!bIsX)
		{
			if (pData->m_nVariableTable[i][0] == 0)
			{
				nRetCode = READ_INT_EXPR(szVariableColumn[i], m_nVariableTable[i][0]);
				LOG_PROCESS_ERROR(nRetCode);

				nRetCode = READ_INT_EXPR(szValueColumn[i], m_nVariableTable[i][1]);
				LOG_PROCESS_ERROR(nRetCode);
			}
		}

		i++;
	}

	rnID = MakeSpellKey(pData->m_nID, pData->m_nLevel, pData->m_nRuneSlotID);

	return TRUE;
Exit0:
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL SPELL_ENTRY::ReadLine(INT32& rnID, const char*& rpcszName, SPELL_ENTRY* pData, CTabFile* pTabConfig, INT32 nRow)
{
	READ_START();

	char* pValue = NULL;
	char szValue[128] = { 0 };
	char* pTok = NULL;
	INT32 nNodeType = 0;

	memset(pData, 0, sizeof(SPELL_ENTRY));

	READ_INT("SpellCommonAttribute.ID", m_nID);
	READ_INT("SpellCommonAttribute.RuneSlotID", m_nRuneSlotID);

	READ_STR("ClientCommonAttribute.AbilityName", m_szName);

	READ_INT("SpellCommonAttribute.Class", m_nAttrTable[satClass]);
	READ_INT("SpellCommonAttribute.RequireLevel", m_nAttrTable[satRequireLevel]);
	READ_INT("SpellCommonAttribute.NextSpell", m_nAttrTable[satNextSpell]);
	READ_INT("SpellCommonAttribute.SwitchSpell", m_nAttrTable[satSwitchSpell]);
	READ_INT("SpellCommonAttribute.ResetTime", m_nAttrTable[satResetTime]);
	READ_INT("SpellCommonAttribute.CacheTime", m_nAttrTable[satCacheTime]);
	READ_INT("SpellCommonAttribute.NeedCache", m_nAttrTable[satNeedCache]);
	READ_INT("SpellCommonAttribute.WeaponTypeR", m_nAttrTable[satWeaponTypeR]);
	READ_INT("SpellCommonAttribute.CanEquip", m_nAttrTable[satCanEquip]);
	READ_INT("SpellCommonAttribute.BindSlot", m_nAttrTable[satBindSlot]);
	READ_INT("SpellCommonAttribute.CastType", m_nAttrTable[satCastType]);
	READ_INT("SpellCommonAttribute.JudgeType", m_nAttrTable[satJudgeType]);
	READ_INT("SpellCommonAttribute.AttackType", m_nAttrTable[satAttackType]);
	READ_INT("SpellCommonAttribute.ElementaryType", m_nAttrTable[satElementaryType]);
	READ_INT("SpellCommonAttribute.QTEType", m_nAttrTable[satQteType]);
	READ_INT("SpellCommonAttribute.LockType", m_nAttrTable[satLockType]);
	READ_INT("SpellCommonAttribute.MinDistance", m_nAttrTable[satMinDistance]);
	READ_INT("SpellCommonAttribute.MaxDistance", m_nAttrTable[satMaxDistance]);
	READ_INT("SpellCommonAttribute.MinGrids", m_nAttrTable[satMinGrids]);
	READ_INT("SpellCommonAttribute.MaxGrids", m_nAttrTable[satMaxGrids]);
	READ_INT("SpellCommonAttribute.TolerantDistance", m_nAttrTable[satTolerantDistance]);
	READ_INT("SpellCommonAttribute.TolerantGrids", m_nAttrTable[satTolerantGrids]);
	READ_INT("SpellCommonAttribute.RangeType", m_nAttrTable[satRangeType]);
	READ_INT("SpellCommonAttribute.RangeParam1", m_nAttrTable[satRangeParam1]);
	READ_INT("SpellCommonAttribute.RangeParam2", m_nAttrTable[satRangeParam2]);
	READ_INT("SpellCommonAttribute.RangeParam3", m_nAttrTable[satRangeParam3]);
	READ_INT("SpellCommonAttribute.IgnoreObstacle", m_nAttrTable[satIgnoreObstacle]);
	READ_INT("SpellCommonAttribute.UseWeaponRange", m_nAttrTable[satUseWeaponRange]);
	READ_INT("SpellCommonAttribute.IgnoreTargetRadius", m_nAttrTable[satIgnoreTargetRadius]);
	READ_INT("SpellCommonAttribute.AffectedByAtkSpeed", m_nAttrTable[satAffectedByAtkSpeed]);
	READ_INT("SpellCommonAttribute.UseDPH", m_nAttrTable[satUseDPH]);
	READ_INT("SpellCommonAttribute.IgnoreDefense", m_nAttrTable[satIgnoreDefense]);
	READ_INT("SpellCommonAttribute.IgnoreClientTarget", m_nAttrTable[satIgnoreClientTarget]);

	READ_INT("SpellCommonAttribute.MaxTargetCount", m_nAttrTable[satMaxTargetCount]);

	if (pData->m_nAttrTable[satMaxTargetCount] > MAX_SPELL_TARGET_COUNT)
	{
		WRN("Target count is too large, force set to %d. spell id: %d, rune: %d", MAX_SPELL_TARGET_COUNT, pData->m_nID, pData->m_nRuneSlotID);
		pData->m_nAttrTable[satMaxTargetCount] = MAX_SPELL_TARGET_COUNT;
	}

	READ_INT("SpellCommonAttribute.OffsetDirection", m_nAttrTable[satOffsetDirection]);
	READ_INT("SpellCommonAttribute.OffsetDistance", m_nAttrTable[satOffsetDistance]);

	READ_INT("SpellCommonAttribute.HOffsetGrids", m_nAttrTable[satHOffsetGrids]);
	READ_INT("SpellCommonAttribute.VOffsetGrids", m_nAttrTable[satVOffsetGrids]);

	// 缺少
	//SpellCommonAttribute.SceneDifficultyLimit - 场景难度限制
	//SpellCommonAttribute.SceneTypeLimit	    - 场景类型限制

	READ_INT("SpellCommonAttribute.TargetObjectType", m_nAttrTable[satTargetObjType]);
	READ_INT("SpellCommonAttribute.TargetRelationship", m_nAttrTable[satTargetRelationship]);
	READ_INT("SpellCommonAttribute.TargetRace", m_nAttrTable[satTargetSpecies]);
	READ_INT("SpellCommonAttribute.TargetSex", m_nAttrTable[satTargetSex]);
	READ_INT("SpellCommonAttribute.TargetDirection", m_nAttrTable[satTargetDirection]);
	READ_INT("SpellCommonAttribute.TargetStance", m_nAttrTable[satTargetStance]);
	READ_INT("SpellCommonAttribute.SelfStance", m_nAttrTable[satSelfStance]);
	READ_INT("SpellCommonAttribute.CostType", m_nAttrTable[satCostType]);
	READ_INT("SpellCommonAttribute.InitialCostValue", m_nAttrTable[satInitialCostValue]);
	READ_INT("SpellCommonAttribute.InitialCostValuePct", m_nAttrTable[satInitialCostValuePct]);
	READ_INT("SpellCommonAttribute.DurativeCostValue", m_nAttrTable[satDurativeCostValue]);
	READ_INT("SpellCommonAttribute.DurativeCostValuePct", m_nAttrTable[satDurativeCostValuePct]);
	READ_INT("SpellCommonAttribute.ModifierCostValue", m_nAttrTable[satModifierCostValue]);
	READ_INT("SpellCommonAttribute.ModifierCostValuePct", m_nAttrTable[satModifierCostValuePct]);
	READ_INT("SpellCommonAttribute.ItemCostID", m_nAttrTable[satItemCostID]);

	READ_INT_EXPR("SpellCommonAttribute.ItemCostType", m_nAttrTable[satItemCostType]);
	READ_INT_EXPR("SpellCommonAttribute.ItemCostSubType", m_nAttrTable[satItemCostSubType]);

	READ_INT("SpellCommonAttribute.ItemCostCount", m_nAttrTable[satItemCostCount]);
	READ_INT("SpellCommonAttribute.ConsumeItem", m_nAttrTable[satConsumeItem]);

	READ_INT("SpellCommonAttribute.DuraCost", m_nAttrTable[satDuraCost]);

	READ_INT("SpellCommonAttribute.CoolDownTriggerType", m_nAttrTable[satCoolDownTriggerType]);
	READ_INT("SpellCommonAttribute.SelfCoolDown", m_nAttrTable[satSelfCoolDown]);
	READ_INT("SpellCommonAttribute.EventCoolDown", m_nAttrTable[satEventCoolDown]);

	nRetCode = pTabConfig->ReadData("SpellCommonAttribute.GroupCoolDown", nRow, szValue, sizeof(szValue) / sizeof(char));
	LOG_PROCESS_ERROR(nRetCode);

	pValue = szValue;
	pData->m_nCdGrpCount = 0;

	while (1)
	{
		char* tok = SplitStr(pValue, ';');
		if (tok == NULL)
			break;
        LOG_PROCESS_ERROR(pData->m_nCdGrpCount < (INT32)(sizeof(pData->m_CdGrp) / sizeof(pData->m_CdGrp[0])));
        nRetCode = sscanf(tok, "%d-%d", &pData->m_CdGrp[pData->m_nCdGrpCount].m_nCdGrpID,
            &pData->m_CdGrp[pData->m_nCdGrpCount].m_nCdGrpValue);
        LOG_PROCESS_ERROR(nRetCode == 2);

		pData->m_CdGrp[pData->m_nCdGrpCount].m_nCdGrpValue = CONVERT_TIME(pData->m_CdGrp[pData->m_nCdGrpCount].m_nCdGrpValue);

		pData->m_nCdGrpCount++;
	}

	nRetCode = pTabConfig->ReadData("SpellCommonAttribute.PrepareType", nRow, nValue);
	LOG_PROCESS_ERROR(nRetCode);

	// disgusting code !!!
	switch (nValue)
	{
	case sfpptChanneled:
		nValue = spptChanneled;
		break;
	case sfpptPrepared:
	case sfpptPreparedEx:
	case sfpptInstantCast:
		nValue = spptPrepared;
		break;
	case sfpptAccumulated:
		nValue = spptAccumulated;
		break;
	default:
		LOG_PROCESS_ERROR(0);
		break;
	}

	pData->m_nAttrTable[satPrepareType] = nValue;

	READ_INT("SpellCommonAttribute.AdjustFaceDirection", m_nAttrTable[satAdjustDirection]);
	READ_INT("SpellCommonAttribute.AffectedBySpeedParam", m_nAttrTable[satAffectedByWeaponSpeed]);
	READ_INT("SpellCommonAttribute.PrepareTimeMin", m_nAttrTable[satPrepareTimeMin]);
	READ_INT("SpellCommonAttribute.PrepareTimeMax", m_nAttrTable[satPrepareTimeMax]);
	READ_INT("SpellCommonAttribute.AccumulateTimeMax", m_nAttrTable[satAccumulateTimeMax]);
	READ_INT("SpellCommonAttribute.AccumulateSeg", m_nAttrTable[satAccumulateSeg]);
	READ_INT("SpellCommonAttribute.ChannelCount", m_nAttrTable[satChannelCount]);
	READ_INT("SpellCommonAttribute.MinEffectCount", m_nAttrTable[satMinEffectCount]);
	READ_INT("SpellCommonAttribute.Interval", m_nAttrTable[satInterval]);
	READ_INT("SpellCommonAttribute.HardTime", m_nAttrTable[satHardTime]);
	READ_INT("SpellCommonAttribute.HitLevel", m_nAttrTable[satHitLevel]);
	READ_INT("SpellCommonAttribute.MoveBreak", m_nAttrTable[satMoveBreak]);
	READ_INT("SpellCommonAttribute.BreakMove", m_nAttrTable[satBreakMove]);
	READ_INT("SpellCommonAttribute.CanMoveInPrepare", m_nAttrTable[satCanMoveInPrepare]);
	READ_INT("SpellCommonAttribute.CanMoveInHardTime", m_nAttrTable[satCanMoveInHardTime]);
	READ_INT("SpellCommonAttribute.HitBreak", m_nAttrTable[satHitBreak]);
	READ_INT("SpellCommonAttribute.CancelType", m_nAttrTable[satCancelType]);
	READ_INT("SpellCommonAttribute.CancelLevel", m_nAttrTable[satCancelLevel]);
	READ_INT("SpellCommonAttribute.CancelHardTime", m_nAttrTable[satCancelHardTime]);
	READ_INT("SpellCommonAttribute.HitTag", m_nAttrTable[satHitTag]);
	READ_INT("SpellCommonAttribute.Speed", m_nAttrTable[satSpeed]);

	READ_INT("SpellCommonAttribute.IsMixEffect", m_nAttrTable[satIsMixEffect]);
	READ_INT("SpellCommonAttribute.MixEffectPhyRate", m_nAttrTable[satMixEffectPhyRate]);
	READ_INT("SpellCommonAttribute.MixEffectMagRate", m_nAttrTable[satMixEffectMagRate]);

	READ_INT("SpellCommonAttribute.EffectPct", m_nAttrTable[satEffectPct]);
	READ_INT("SpellCommonAttribute.EffectValue", m_nAttrTable[satEffectValue]);
	READ_INT("SpellCommonAttribute.EffectValueMax", m_nAttrTable[satEffectValueMax]);
	READ_INT("SpellCommonAttribute.DefaultValue", m_nAttrTable[satDefaultValue]);
	READ_INT("SpellCommonAttribute.DefaultValueMax", m_nAttrTable[satDefaultValueMax]);
	READ_INT("SpellCommonAttribute.AffectedByNpcParam", m_nAttrTable[satAffectedByNpcParam]);


	READ_INT("SpellCommonAttribute.ThreatPct", m_nAttrTable[satThreatPct]);
	READ_INT("SpellCommonAttribute.ThreatValue", m_nAttrTable[satThreatValue]);
	READ_INT("SpellCommonAttribute.ThreatMax", m_nAttrTable[satThreatMax]);
	READ_INT("SpellCommonAttribute.CanCastOnMount", m_nAttrTable[satCanCastOnMount]);
	READ_INT("SpellCommonAttribute.CasterDeadLimit", m_nAttrTable[satCasterDeadLimit]);
	READ_INT("SpellCommonAttribute.TargetDeadLimit", m_nAttrTable[satTargetDeadLimit]);
	READ_INT("SpellCommonAttribute.CasterLimitBits", m_nAttrTable[satCasterLimitBits]);
	READ_INT("SpellCommonAttribute.CasterCCLimitBits", m_nAttrTable[satCasterCCLimitBits]);
	READ_INT("SpellCommonAttribute.CancelStiff", m_nAttrTable[satCancelStiff]);
	READ_INT("SpellCommonAttribute.KnockBackRate", m_nAttrTable[satKnockBackRate]);
	READ_INT("SpellCommonAttribute.BaseStiffTime", m_nAttrTable[satBaseStiffTime]);
	READ_INT("SpellCommonAttribute.MonsterChaseDelayTime", m_nAttrTable[satMonsterChaseDelayTime]);

	READ_INT("SpellCommonAttribute.WalkDelayTime", m_nAttrTable[satWalkDelayTime]);
	READ_INT("SpellCommonAttribute.RunDelayTime", m_nAttrTable[satRunDelayTime]);
	READ_INT("SpellCommonAttribute.HardTimeAffectedByCharLevel", m_nAttrTable[satHardTimeAffectedByCharLevel]);

	READ_INT("SpellCommonAttribute.ChainEffect", m_nAttrTable[satChainEffect]);
	READ_INT("SpellCommonAttribute.TrigEvent", m_nAttrTable[satTrigEvent]);
	READ_INT("SpellCommonAttribute.SceneTypeLimit", m_nAttrTable[satSceneTypeLimit]);

	READ_INT("SpellCommonAttribute.ExCastTime1", m_ExCastPoint[0].m_nExCastTime);
	READ_INT("SpellCommonAttribute.ExCastTime2", m_ExCastPoint[1].m_nExCastTime);
	READ_INT("SpellCommonAttribute.ExCastTime3", m_ExCastPoint[2].m_nExCastTime);
	READ_INT("SpellCommonAttribute.ExCastTime4", m_ExCastPoint[3].m_nExCastTime);
	READ_INT("SpellCommonAttribute.ExCastTime5", m_ExCastPoint[4].m_nExCastTime);

	READ_INT("SpellCommonAttribute.CasterCheckNode", m_nAttrTable[satCasterCheckNode]);
	READ_INT("SpellCommonAttribute.TargetCheckNode", m_nAttrTable[satTargetCheckNode]);
	READ_INT("SpellCommonAttribute.OnLearn", m_nAttrTable[satOnLearn]);
	READ_INT("SpellCommonAttribute.OnForget", m_nAttrTable[satOnForget]);
	READ_INT("SpellCommonAttribute.OnEquip", m_nAttrTable[satOnEquip]);
	READ_INT("SpellCommonAttribute.OnUnEquip", m_nAttrTable[satOnUnEquip]);
	READ_INT("SpellCommonAttribute.OnPrepare", m_nAttrTable[satOnPrepare]);
	READ_INT("SpellCommonAttribute.OnCast", m_nAttrTable[satOnCast]);
	READ_INT("SpellCommonAttribute.OnBreak", m_nAttrTable[satOnBreak]);
	READ_INT("SpellCommonAttribute.OnBulletCreate", m_nAttrTable[satOnCreateBullet]);
	READ_INT("SpellCommonAttribute.BeforeRoll", m_nAttrTable[satBeforeRoll]);
	READ_INT("SpellCommonAttribute.AfterRoll", m_nAttrTable[satAfterRoll]);
	READ_INT("SpellCommonAttribute.BeforeEffect", m_nAttrTable[satBeforeEffect]);
	READ_INT("SpellCommonAttribute.AfterEffect", m_nAttrTable[satAfterEffect]);

	READ_INT("SpellCommonAttribute.OnExCast1", m_ExCastPoint[0].m_nExOnCast);
	READ_INT("SpellCommonAttribute.OnExCast2", m_ExCastPoint[1].m_nExOnCast);
	READ_INT("SpellCommonAttribute.OnExCast3", m_ExCastPoint[2].m_nExOnCast);
	READ_INT("SpellCommonAttribute.OnExCast4", m_ExCastPoint[3].m_nExOnCast);
	READ_INT("SpellCommonAttribute.OnExCast5", m_ExCastPoint[4].m_nExOnCast);

	READ_INT("ClientCommonAttribute.ShowOnUI", m_bShowOnUI);

	rnID = (pData->m_nID << 16) + pData->m_nRuneSlotID;
//	rpcszName = pData->m_szName;

	return TRUE;
Exit0:
	return FALSE;

}


//BOOL SPELL_NODE_DATA::ReadLine(INT32& rnID, const char*& rpcszName, SPELL_NODE_DATA* pData, CTabFile* pTabConfig, INT32 nRow)
//{
//	READ_START();
//
//	READ_INT("SpellID", m_nID);
//	READ_INT("RuneSlotID", m_nRuneSlotID);
//	READ_INT("CasterCheckNode", m_nCasterCheckNode);
//	READ_INT("TargetCheckNode", m_nTargetCheckNode);
//	READ_INT("OnLearn", m_nOnLearn);
//	READ_INT("OnForget", m_nOnForget);
//	READ_INT("OnEquip", m_nOnEquip);
//	READ_INT("OnUnEquip", m_nOnUnEquip);
//	READ_INT("OnPrepare", m_nOnPrepare);
//	READ_INT("OnCast", m_nOnCast);
//	READ_INT("OnBreak", m_nOnBreak);
//	READ_INT("OnBulletCreate", m_nOnBulletCreate);
//	READ_INT("BeforeRoll", m_nBeforeRoll);
//	READ_INT("AfterRoll", m_nAfterRoll);
//	READ_INT("BeforeEffect", m_nBeforeEffect);
//	READ_INT("AfterEffect", m_nAfterEffect);
//
//	READ_INT("OnExCast1", m_ExOnCast[0]);
//	READ_INT("OnExCast2", m_ExOnCast[1]);
//	READ_INT("OnExCast3", m_ExOnCast[2]);
//	READ_INT("OnExCast4", m_ExOnCast[3]);
//	READ_INT("OnExCast5", m_ExOnCast[4]);
//
//	rnID = (pData->m_nID << 16) + pData->m_nRuneSlotID;
//
//
//	return TRUE;
//Exit0:
//	DBG("Spell Node Data Fail: %d, Rune: %d", pData->m_nID, pData->m_nRuneSlotID);
//	return FALSE;
//}
//