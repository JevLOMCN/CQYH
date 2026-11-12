//
// Scene interactive moving objects
//
class MMOKActor extends KActorSpawnable  implements(IAbilityDamageable)
	native;


// Get faction ID to calculate damage
event int GetFriendlyMask()
{
	return 0;
}

// if processed return true, else false
event ProcessDamage(DamageData Damage)
{
	local int i;
	local Vector Momentum;
	Initialize();
	if(Damage.AbilityData == none)
	{
		return;
	}

	for(i = 0; i < Damage.AbilityData.ClientEffectList.Length; ++i)
	{
		if(Damage.AbilityData.ClientEffectList[i].IsA('AbilityHitEffectComponent'))
		{
			Momentum = AbilityHitEffectComponent(Damage.AbilityData.ClientEffectList[i]).GetMomentum(Damage.BaseActor, self);
			CollisionComponent.AddImpulse(Momentum);
		}
	}
}

event bool IsDead()
{
	return false;
}

DefaultProperties
{
	bDeleteMe=false
	bHidden=false
	LifeSpan=10
	bCollideWorld=true
	bCollideActors=true
	bBlocksNavigation=false
	bBlockActors=false
	bWakeOnLevelStart=true
	bDisableClientSidePawnInteractions=false
		
	Begin Object Name=StaticMeshComponent0
	    LightingChannels=(Gameplay_2=TRUE)
	End Object

}