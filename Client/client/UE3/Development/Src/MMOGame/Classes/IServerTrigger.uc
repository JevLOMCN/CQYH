//
// IServerTrigger interface, when sending a protocol, convert the object to this interface, get the data the protocol needs, then send it to the server.
//
interface IServerTrigger
	native;

enum ETrapEventType
{
    // Trigger NPC spawn event.
    eTET_TRAPEVENT_NPC_SPAWN <DisplayName="NPC Spawn">,
    // Blocking area control.
    eTET_TRAPEVENT_BLOCK <DisplayName="Change Block">,
    // Ability releaser event.
    eTET_TRAPEVENT_SKILL <DisplayName="Use Skill">,
    // Teleport event.
    eTET_TRAPEVENT_TELEPORT <DisplayName="Teleport">,
	// Portal open/close event.
	eTET_TRAPEVENT_TRANSMI <DisplayName="Teleport Gate">
};


enum ETriggerType
{
	eTT_TRAPTYPE_INVALID <DisplayName="Invalid">,
	// Enter area trigger.
	eTT_TRAPTYPE_ENTER_AREA <DisplayName="EnterArea">,
	// NPC NPC death trigger.
	eTT_TRAPTYPE_NPC_DEAD <DisplayName="NPC Dead">,
	// Manual trigger.
	eTT_TRAPTYPE_MANUAL <DisplayName="Manual">,
	// Progress-type mechanism.
	eTT_TRAPTYPE_LOADING <DisplayName="Loading">,
    // Ability cast trigger.
    eTT_TRAPTYPE_SKILL <DisplayName="Skill">,
    // Item trigger.
    eTT_TRAPTYPE_PROP <DisplayName="Item">,
    // Multiplayer (passive) mechanism.
    eTT_TRAPTYPE_MULTI <DisplayName="Multi">,
    // Multiplayer progress trigger.
    eTT_TRAPTYPE_MULTI_LOADING <DisplayName="Multi-Loading">,
};

enum ETriggerInitState
{
	eTIS_Invisible <DisplaName="Invisible">,
	eTIS_Off         <DisplaName="Off">,
	eTIS_On          <DisplaName="On">,
	eTIS_Loading <DisplaName="Loading">,
	eTIS_NA          <DisplaName="N/A">
};

struct native AbilityEvent
{
	var() array<MMOAbility> Ability;
	// TODO: TODO: add needed parameters
};

struct native TeleportEvent
{
	var() string TargetScene;
	var() Vector TargetPosition;
	// TODO: TODO: add needed parameters
};

struct native NPCDeadEvent
{
	var() int GroupID;
	var() array<IServerArea> TargetEventArea;
};

// Register with Facade for lookup
native function RegisterToFacade();

native function ETriggerType GetTriggerType();
native function int GetTriggerID();
native function SetAutoID(int id);

// Get object size
native function Box GetObjectServerBounds();

// get object position
native function vector GetObjectServerPosition();

// Get the associated blocking box
native function array<DynamicBlockingVolume> GetBlockingVolumes();

// Whether the initial state is open, closed, or unavailable
native function ETriggerInitState GetServerInitState();

// Return an identifier name for debugging
native function string GetClientName();

// get the associated NPC spawn point
native function array<NPCSpawnPoint> GetNPCSpawnPoints();

// Get associated mechanisms, to activate them after the current event area completes
native function array<IServerTrigger> GetLinkedTrigger();

// Mechanism delayed start time
native function int GetDelayTime();

// Trigger ability list
native function array<AbilityEvent> GetAbilityList();

// Trigger ability list
native function TeleportEvent GetTeleportEvent();

// Get trap type
native function ETrapEventType GetTrapEventType();

native function array<IServerArea> GetLinkedEventArea();

// Trigger time
native function int GetLoadingTime(); 

// NPCEvent data triggered on NPC death
native function NPCDeadEvent GetNPCDeadEvent();

// List of portals to open
native function array<TeleportOrig> GetLinkedTeleportList();