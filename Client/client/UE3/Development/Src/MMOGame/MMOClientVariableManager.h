#ifndef _MMOCLIENTVARIABLEMANAGER_H_
#define _MMOCLIENTVARIABLEMANAGER_H_

#include "common.h"
#include "MMOUtil.h"

// MAX_CLIENT_VARIABLE_COUNT = 128
enum EClientVariableType
{
	ECVT_Begin = 0,
	ECVT_Game_Config = ECVT_Begin,

	ECVT_Quest,												// quest variables
	
	ECVT_SkillBarSlotBegin,									// action bar settings
	ECVT_SkillBarSlotData2_,
	ECVT_SkillBarSlotData3_,
	ECVT_SkillBarSlotData4_,
	ECVT_SkillBarSlotData5_,
	ECVT_SkillBarSlotData6_,
	ECVT_SkillBarSlotData7_,
	ECVT_SkillBarSlotData8_,
	ECVT_SkillBarSlotData9_,
	ECVT_SkillBarSlotData10_,
	ECVT_SkillBarSlotData11_,
	ECVT_SkillBarSlotData12_,
	ECVT_SkillBarSlotData13_,
	ECVT_SkillBarSlotData14_,
	ECVT_SkillBarSlotEnd,

	ECVT_KeyManagerBegin,									// hotkey settings
	ECVT_KeyManager01_,
	ECVT_KeyManager02_,
	ECVT_KeyManager03_,
	ECVT_KeyManager04_,
	ECVT_KeyManager05_,
	ECVT_KeyManager06_,
	ECVT_KeyManager07_,
	ECVT_KeyManager08_,
	ECVT_KeyManager09_,
	ECVT_KeyManager10_,
	ECVT_KeyManager11_,
	ECVT_KeyManager12_,
	ECVT_KeyManager13_,
	ECVT_KeyManager14_,
	ECVT_KeyManager15_,
	ECVT_KeyManager16_,
	ECVT_KeyManager17_,
	ECVT_KeyManager18_,
	ECVT_KeyManager19_,
	ECVT_KeyManager20_,
	ECVT_KeyManager21_,
	ECVT_KeyManager22_,
	ECVT_KeyManager23_,
	ECVT_KeyManager24_,
	ECVT_KeyManager25_,
	ECVT_KeyManager26_,
	ECVT_KeyManager27_,
	ECVT_KeyManager28_,
	ECVT_KeyManager29_,
	ECVT_KeyManager30_,
	ECVT_KeyManager31_,
	ECVT_KeyManager32_,
	ECVT_KeyManager33_,
	ECVT_KeyManager34_,
	ECVT_KeyManager35_,
	ECVT_KeyManager36_,
	ECVT_KeyManager37_,
	ECVT_KeyManager38_,
	ECVT_KeyManager39_,
	ECVT_KeyManager40_,
	ECVT_KeyManager41_,
	ECVT_KeyManager42_,
	ECVT_KeyManager43_,
	ECVT_KeyManager44_,
	ECVT_KeyManager45_,
	ECVT_KeyManager46_,
	ECVT_KeyManager47_,
	ECVT_KeyManager48_,
	ECVT_KeyManager49_,
	ECVT_KeyManager50_,
	ECVT_KeyManagerEnd,

	ECVT_TraceRelation,
	ECVT_ContralType,

	ECVT_Guide_Begin,										// newbie guide (MAX_GUIDE_COUNT = 128)
	ECVT_Guide_Data_1 = ECVT_Guide_Begin,
	ECVT_Guide_Data_2,
	ECVT_Guide_Data_3,
	ECVT_Guide_Data_4,
	ECVT_Guide_End,

	ECVT_Sound_Config1 = ECVT_Guide_End,
	ECVT_Sound_Config2,

	ECVT_Achievement_Trace_Personal1,						// achievement tracking
	ECVT_Achievement_Trace_Personal2,
	ECVT_Achievement_Trace_Guild,

	ECVT_Journey_Trace,										// Mafa journal

	ECVT_Assist_Skill,										// F12 skill
	ECVT_Assist_Protect_Begin,
	ECVT_Assist_Protect_Item = ECVT_Assist_Protect_Begin,	// F12 protection (divided into eight 4-bit groups; per group: first bit records whether enabled, last three bits record item index)
	ECVT_Assist_Protect_Value1,								// F12 protection (high bits HP, low bits MP)
	ECVT_Assist_Protect_Value2,
	ECVT_Assist_Protect_Value3,
	ECVT_Assist_Protect_Value4,
	ECVT_Assist_Protect_Time1,								// F12 protection (one time per 8 bits)
	ECVT_Assist_Protect_Time2,
	ECVT_Assist_Protect_End,

	ECVT_Game_Config2 = ECVT_Assist_Protect_End,			//game_congif extension
	ECVT_End
};

#define KF_Max ((((ECVT_KeyManagerEnd - ECVT_KeyManagerBegin + 1) * 4) - 1) / 3) // maximum count (the server/client change database is only this large)

class MMOClientVariableManager : public SingletonMeyers<MMOClientVariableManager>
{
public:
	MMOClientVariableManager();
	virtual ~MMOClientVariableManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);

	void SyncClientVariables(INT32 variables[MAX_CLIENT_VARIABLE_COUNT]);
	void SetClientVariable(EClientVariableType index, INT32 value);
};

#endif