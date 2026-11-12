#ifndef __LOGICCOMMAND_H__
#define __LOGICCOMMAND_H__

#if 0

#include "MMOAny.h"


class ILogicCommandFactory;

//------------------------------------------------------------------------
// logic command type
//------------------------------------------------------------------------
enum ELogicCommandType
{
	LCT_Invalid = -1,
	LCT_Action,				// action
	LCT_StopAction,			// stop action
	LCT_Move,				// move
	LCT_StopMove,			// stop move
	LCT_AbilityUse,			// use ability directly
	LCT_AbilitySpell,		// decomposed ability: charge
	LCT_AbilityCast,		// decomposed ability: cast
	LCT_AbilityGuide,		// decomposed ability: channel
	LCT_LifeAbility,		// life skill
	LCT_Max
};

//------------------------------------------------------------------------
// logic command parameters
//------------------------------------------------------------------------
struct LogicCommandParameter
{
	int CommandTypeID;
	int LogicCount;
	Any CommandData;
};

//------------------------------------------------------------------------
// logic command data structure wrapper
//------------------------------------------------------------------------
class LogicCommand// : public FUseSystemMallocForNew
{
public:

	LogicCommand(void);
	virtual ~LogicCommand(void);

	UINT GetStartTime() const;
	INT GetLogicCount() const;

	void CleanUp();

	ELogicCommandType GetCommandID() const { return _cmdType; }
	void SetCommandID(ELogicCommandType val) { _cmdType = val; }

	virtual bool Modify(const MMO::LogicCommand* pCmd);

	static void RegisterLogicCommandFactory(int commandTypeID, ILogicCommandFactory* factory);
	static ILogicCommandFactory* GetLogicCommandFactory(int commandTypeID);

protected:

	// start time
	UINT	_startTime;

	// logic count
	INT		_logicCount;

	// ability type
	ELogicCommandType _cmdType;

	// logic command factory management
	typedef std::map<int, ILogicCommandFactory*> LogicFactoryMap;
	static LogicFactoryMap g_logicFactoryMap;

	void SetStartTime(UINT startTime);
	void SetLogicCount(INT logicCount);
};

//------------------------------------------------------------------------
// logic command object factory
//------------------------------------------------------------------------
class ILogicCommandFactory
{
public:

	virtual MMO::LogicCommand* Create(const LogicCommandParameter& par) = 0;
};

template<class FactoryType, int CmdTypeID>
struct TRegLogicCmdFactory
{
	FactoryType Factory;

	TRegLogicCmdFactory()
	{
		LogicCommand::RegisterLogicCommandFactory(CmdTypeID, &Factory);
	}
};

#define M_REG_LOGIC_CMD_FACTORY(CmdTypeID, FactoryClass) static MMO::TRegLogicCmdFactory<FactoryClass, CmdTypeID> g_##FactoryClass;


#endif
#endif