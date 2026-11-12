//
// Safe zone, player actions need to change after entering this zone
//

class MMOVolume_SafeZone extends PhysicsVolume
	native
	dependson(MMOActor)
	placeable;

simulated event PawnEnteredVolume(Pawn P)
{
	//local MMOActor actor;

	super.PawnEnteredVolume(P);

	//actor = MMOActor(P);

	//if(actor != none)
	//{
	//	actor.OnEnterSafeZone();
	//}
}

simulated event PawnLeavingVolume(Pawn P)
{
	//	local MMOActor actor;

	super.PawnLeavingVolume(P);

	//actor = MMOActor(P);

	//if(actor != none)
	//{
	//	actor.OnLeavingSafeZone();
	//}
}

simulated event PhysicsChangedFor(Actor Other)
{
	super.PhysicsChangedFor(Other);
}

defaultproperties
{
}
