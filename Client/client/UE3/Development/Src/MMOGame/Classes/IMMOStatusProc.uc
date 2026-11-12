//
// State presentation handling interface
//
interface IMMOStatusProc
	native;

event OnExecute(MMOActor actor);
native function Update(MMOActor actor);