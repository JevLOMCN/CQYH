#include "stdafx.h"
#include "Inc\MMOGameUIClasses.h"
#include "MMOClientVariableManager.h"
#include "MMOSkillManager.h"
#include "MMOQuestManager.h"
#include "MMOGuideManager.h"
#include "MMOAssistManager.h"
#include "MMOAchievementManager.h"
#include "MMOJourneyManager.h"
#include "MMOAssistManager.h"

using namespace ExternalMessage;

//////////////////////////////////////////////////////////////////////////
MMOClientVariableManager::MMOClientVariableManager()
{
}

MMOClientVariableManager::~MMOClientVariableManager()
{
}

void MMOClientVariableManager::Init()
{
}

void MMOClientVariableManager::Shutdown()
{
}

void MMOClientVariableManager::Clear()
{
}

void MMOClientVariableManager::Update(FLOAT deltaTime)
{
}

void MMOClientVariableManager::SyncClientVariables(INT32 variables[MAX_CLIENT_VARIABLE_COUNT])
{
	UMMOUIManager* uiManager = MMOUI::GetUIManager();
	if (!uiManager)
	{
		return;
	}

	uiManager->ApplyConfigByServer(variables[ECVT_Game_Config], variables[ECVT_Game_Config2]);

	//uiManager->ApplySoundConfigByServer(variables[ECVT_Sound_Config1],variables[ECVT_Sound_Config2]);
	
	MMOQuestManager::getSingleton().UpdateAllClientVariable(variables);

	MMOFacade::getSingleton().GCTType = (GAME_CONTROL_TYPE)variables[ECVT_ContralType];

	CONST INT buf_size = (ECVT_KeyManagerEnd-ECVT_KeyManagerBegin+1)<<2;
	UMMOKeyManager::getSingleton()->RestoreKey((BYTE*)&(variables[ECVT_KeyManagerBegin]), buf_size  );

	INT32* p = (INT32*)variables + ECVT_SkillBarSlotBegin;
	uiManager->RefreshSkillBarByServerData(p, (ECVT_SkillBarSlotEnd - ECVT_SkillBarSlotBegin + 1) << 2);

	//INT32* pSkillBarSkillData = (INT32*)variables + ECVT_SkillBarSkillBegin;
	//const INT serverSingleDataSize = sizeof(INT32);
	//CONST INT needSkillBarSkillDataSize = (ECVT_SkillBarSkillEnd - ECVT_SkillBarSkillBegin + 1) * serverSingleDataSize;
	//MMOSkillManager::getSingleton().ReflashSkillSlotRedirection(pSkillBarSkillData, needSkillBarSkillDataSize);

	uiManager->SetTraceRelationForceID(variables[ECVT_TraceRelation]);

	BYTE* buf = (BYTE*)&(variables[ECVT_Guide_Begin]);
	UINT size = (ECVT_Guide_End - ECVT_Guide_Begin) << 2;
	MMOGuideManager::getSingleton().LoadRecord(buf, size);

	// Achievement tracking
	if (variables[ECVT_Achievement_Trace_Personal1] > 0)
	{
		MMOAchievementData::getSingleton().AddTraceAchievement(variables[ECVT_Achievement_Trace_Personal1]);
	}
	if (variables[ECVT_Achievement_Trace_Personal2] > 0)
	{
		MMOAchievementData::getSingleton().AddTraceAchievement(variables[ECVT_Achievement_Trace_Personal2]);
	}
	if (HIWORD(variables[ECVT_Achievement_Trace_Guild]) > 0)
	{
		MMOAchievementData::getSingleton().AddTraceAchievement(HIWORD(variables[ECVT_Achievement_Trace_Guild]));
	}
	if (LOWORD(variables[ECVT_Achievement_Trace_Guild]) > 0)
	{
		MMOAchievementData::getSingleton().AddTraceAchievement(LOWORD(variables[ECVT_Achievement_Trace_Guild]));
	}

	MMOJourneyManager::getSingleton().LoadData(variables[ECVT_Journey_Trace]);

	UINT32 protectData[PROTECT_DATA_COUNT] = 
	{	
		variables[ECVT_Assist_Protect_Item],
		variables[ECVT_Assist_Protect_Value1],
		variables[ECVT_Assist_Protect_Value2],
		variables[ECVT_Assist_Protect_Value3],
		variables[ECVT_Assist_Protect_Value4],
		variables[ECVT_Assist_Protect_Time1],
		variables[ECVT_Assist_Protect_Time2],
	};
	MMOAssistManager::getSingleton().LoadData(variables[ECVT_Assist_Skill], protectData);
}

void MMOClientVariableManager::SetClientVariable(EClientVariableType index, INT32 value)
{
	CMessageHandler::GetInstance().Send_UpdateClientVariable(index, value);
}