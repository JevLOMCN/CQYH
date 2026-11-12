//
// IServerArea interface
//
interface IServerArea
	native;

// Register with Facade for lookup
native function RegisterToFacade();

// Get the corresponding ID
native function int GetAreaID();
native function SetAutoID(int id);

// Server notification protocol
event OnServerNotify(bool on);

// Get object size
native function Box GetObjectServerBounds();

// get object position
native function vector GetObjectServerPosition();

// Get the associated blocking box
//native function array<DynamicBlockingVolume> GetBlockingVolumes();

// get the associated NPC spawn point
//native function array<NPCSpawnPoint> GetNPCSpawnPoints();

// Return an identifier name for debugging
native function string GetClientName();

// Get associated mechanisms, to activate them after the current event area completes
//native function array<IServerTrigger> GetLinkedTrigger();
