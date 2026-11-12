#ifndef __EQUIPDECOMPOSE_MANAGER_H__
#define __EQUIPDECOMPOSE_MANAGER_H__

#include "MMOGameEvent.h"
#include "common.h"
#include "MMOUtil.h"

#define COMPOSE_CONSUME_COUNT 6
#define DECOMPOSE_MAX_COUNT 3
#define COMPOSE_SUCCESS_BOUNS 2

struct EquipComposeTemplate
{
	INT itemId;  //Scroll id
	INT equipId; //Equipment id
	INT cost;    //Gold cost
	INT matId[COMPOSE_CONSUME_COUNT];
	INT matCnt[COMPOSE_CONSUME_COUNT];
	UBOOL bBroadCast;
};

struct DecomposeResult
{
	INT itemId;
	INT cnt;
};

struct MMOGameEvent_EquipMaker : public MMOGameEvent
{
public:
	enum EEquipDecomposeEventType
	{
		EEDT_DecomposeSuccess = 0,
		EEDT_ComposeSuccess,
	};

	MMOGameEvent_EquipMaker(EEquipDecomposeEventType et)
		:
	EventType(et)
	{
		IntRst = 0;
	}

	virtual EGameEvent GetEventType() const
	{
		return GameEvent_EquipMaker;
	}

	EEquipDecomposeEventType EventType;
	INT IntRst;
};

typedef std::map<INT,EquipComposeTemplate,M_UE3_STL_ALLOC_MAP(INT,EquipComposeTemplate)> EquipComposeMap;


enum EDecomposeStatus
{
	EDS_None = 0,
	EDS_Decompose,
	EDS_Compose,
	EDS_Max,
};

class EquipDecomposeManager : public SingletonMeyers<EquipDecomposeManager>, public INativeGameEventHandler
{
public:
	EquipDecomposeManager();
	virtual ~EquipDecomposeManager();

	void Init();
	//void Shutdown();
	//void Clear();
	void ReadConfig();

	void ComposeEquip(INT itemId);

	void OnNativeGameEvent(struct MMOGameEvent* evt);

	EquipComposeTemplate& GetComposeTemplateById(INT itemId);

	FORCEINLINE TArray<DecomposeResult>& GetDecomposeResult()
	{
		return m_DecomposeResult;
	}

	FORCEINLINE UBOOL IsDecomposeEquip()
	{
		return m_eDecomposeStatus == EDS_Decompose;
	}

	FORCEINLINE void ChangeState(EDecomposeStatus es)
	{
		m_eDecomposeStatus = es;
	}

	FORCEINLINE INT GetComposedId() const
	{
		return m_iCurComposedId;
	}

	
	void BeginCompose(INT itemId);
	void EndCompose();

private:
	UBOOL DecomposeEquip(INT iConId, INT iSlotId);
	UBOOL DecomposeSkillBook(INT iConId, INT iSlotId);
public:
	void Decompose(INT iConId, INT iSlotId);

	void BeginDecompose();
	void EndDecompose();

	void RealDecompose(INT iConId, INT iSlotId);

private:
	EquipComposeMap m_EquipComposeMap;

	EDecomposeStatus m_eDecomposeStatus;

	//Save the equipment decomposition result
	TArray<DecomposeResult> m_DecomposeResult;

	INT m_iCurComposedId; //ID of the scroll currently being synthesized
};

#endif