
interface IMMOActor
	native;


// Character type, may affect some attack calculations
enum EActorClassType
{
	// None
	EACT_None <DisplayName="None">,
	
	// Scene object
	EACT_SceneObject <DisplayName="SceneObject">,

	// Local player
	EACT_PlayerSelf <DisplayName="PlayerSelf">,

	// Other players
	EACT_OtherPlayer <DisplayName="OtherPlayer">,

	// Official NPC
	EACT_NPC <DisplayName="NPC">,

	// Normal monster
	EACT_Monster <DisplayName="Monster">,

	// BOSS
	EACT_Boss <DisplayName="Boss">,

	// Monster corpse
	EACT_MonsterBody <DisplayName="MonsterBody">,

	// NPCNPC corpse
	EACT_NPCBody <DisplayName="NPCBody">,

	// Player corpse
	EACT_PlayerBody <DisplayName="PlayerBody">,

	// Doodad
	EACT_Doodad <DisplayName="Doodad">,

	// Dropped item
	EACT_DropItem <DisplayName="DropItem">,

	// TOY
	EACT_Toy <DisplayName="Toy">,

	// Pet
	EACT_Pet <DisplayName="Pet">,

	// Function NPC
	EACT_Function_NPC <DisplayName="FNPC">,

	// GM(GM is special, consider customer service later)
	EACT_GM  <DisplayName="GM">
};

native function EActorClassType GetActorType() const;