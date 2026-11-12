//
// IPlayerActionEx interface, player actions
//
interface IPlayerActionEx
	native;

native function Update(float DeltaTime);
native function bool IsCompleted();
native function Cancel();
native function bool IsMoveAction();