//
// PlayerActionQueue, player action queue
//
class PlayerActionQueue extends Object
	native;

enum CMDTYPE
{
	CMD_ALL,
	CMD_ABILITY,
	CMD_Interrupt,
};

var bool bLocked;

var array<PlayerActionEx_Base> PlayerActionList;

native function Add(PlayerActionEx_Base action);
native function CancelAll();
native function Update(float DeltaTime);

native function int GetCmdNum(CMDTYPE Type);