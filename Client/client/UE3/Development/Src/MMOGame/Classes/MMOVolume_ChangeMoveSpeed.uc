
class MMOVolume_ChangeMoveSpeed extends TriggerVolume
	placeable
	native;

// Area ID, consistent with the server config
var(MMOServer) int ServerID;

// Multiplier the player move speed changes to
var(MMOServer) float SpeedChange;

// Register with Facade for lookup
native function RegisterToFacade();
// Get object size
native function Box GetObjectServerBounds();

// get object position
native function vector GetObjectServerPosition();

simulated function PreBeginPlay()
{	
	Super.PreBeginPlay();

	// Collision is enabled in the editor and normally disabled, notified by the server
	//if(!WorldInfo.IsPlayInEditor())
	//{
	//	SetCollision(false, false, true);
	//}

	SetCollision(false, false, false);

	RegisterToFacade();
}

DefaultProperties
{	
	SpeedChange=1.0
}