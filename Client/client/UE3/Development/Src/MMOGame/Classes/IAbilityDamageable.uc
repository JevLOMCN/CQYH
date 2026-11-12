//
// IAbilityDamageable interface, objects that can take damage can implement this interface
//
interface IAbilityDamageable
	dependson(TypeDefine)
	native;


// damage data
struct native DamageData
{
	// Whether this is damage data determined by the server
	var bool bServerDamageData;
	// Damage source, may be a player, an NPC, a trap in the scene, etc.
	var Actor BaseActor;
	var int CreatorId;
	// Damage value
	var int DamageValue;
	var EDamageResultType DamageType;
	// Mainly because the client HP and server HP may differ, so client HP of 0 is not fully trusted as death; this important info comes from the server
	var bool bCauseDead;
	// Hit effect template
	var AbilityHitPointComponent AbilityData;

	var EElementaryType ElementaryType;
};

// Get faction ID to calculate damage
event int GetFriendlyMask();

// if processed return true, else false
event ProcessDamage(DamageData Damage);

event bool IsDead();