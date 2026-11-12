
interface IPickableActor
	native;

// Item pickup priority
enum EPickPriority
{
	//EPP_Trigger	    <DisplayName="Trigger">,
	//EPP_DropItem	<DisplayName="DropItem">,
	//EPP_NPC         <DisplayName="NPC">,
	//EPP_Totem       <DisplayName="Totem">,		
	//EPP_Retinue     <DisplayName="Retinue">,
	//EPP_Pet         <DisplayName="Pet">,
	//EPP_OtherPlayer	<DisplayName="OtherPlayer">,
	//EPP_InterpActor <DisplayName="StaticMeshActor">,
	//EPP_TeleportOrig <DisplayName="TeleportOrig">,
	//EPP_WorldGeometry,
	////EPP_StaticMeshActor <DisplayName="StaticMeshActor">,
	////EPP_Terrain     <DisplayName="Terrain">,
	////EPP_Landscape	<DisplayName="Landscape">,
	//EPP_Body	    <DisplayName="Body">,
	//EPP_Player      <DisplayName="Player">,
	//EPP_ObstacleMesh      <DisplayName="ObstacleMesh">,
	//EPP_UnTouchBody  <DisplayName="EPP_UnTouchBody">,
	//EPP_PickMax      <DisplayName="PickMax">,

	EPP_High,		// Scene triggers, interactive NPCs, all doodads
	EPP_Normal,		// Non-interactive NPCs, other players, pets, dropped items
	EPP_Low,		 // The player itself
	EPP_Floor,      // Scene
	EPP_PickMax,
};

// Mouse state definitions, extended from UE3's EMouseCursor
// Map cursor files through the Cursor section in DefaultGame.ini
enum EMMOCursorType
{
	EMC_None,
	EMC_NoChange,		// Keeps the platform client from calling setcursor so a cursor can be set elsewhere (ie using wxSetCursor).
	EMC_Arrow,
	EMC_Cross,
	EMC_SizeAll,
	EMC_SizeUpRightDownLeft,
	EMC_SizeUpLeftDownRight,
	EMC_SizeLeftRight,
	EMC_SizeUpDown,
	EMC_Hand,
	EMC_GrabHand,
	//Custom cursors for game-specific usage
	EMC_IBEAM,
	EMC_Sell,
	EMC_Buy,
	EMC_Attack,
	EMC_Pick,
	EMC_Talk,
	EMC_Dialog,
	EMC_Task,
	EMC_Repair,
	EMC_Identify,
	EMC_Signature,
	EMC_Aim,
    EMC_PickWithLock,
	EMC_Split,
	EMC_Busy,
};

native function EPickPriority GetPickPriority();
native function EMMOCursorType GetMouseCursor();
