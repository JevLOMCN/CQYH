class AbilityHitPointComponent extends AbilityComponent
	hidecategories(Object)
	editinlinenew
	dependson(TypeDefine)
	native;



var() int   HitPointId;
// list of effects caused by server damage
var(ServerEffect) editinline array<AbilityComponent> ServerEffectList;


// client effect
var(ClientEffect) float Degree<ClampMin = 0.0 | ClampMax = 360.0>;
var(ClientEffect) float Radius;
var(ClientEffect) float DamageValue;


var(ServerEffect) DeathStyle AbilityDeath;
var(ClientEffect) editinline array<AbilityComponent> ClientEffectList;

// ability virtual bullet effect resource
var(bullet) editinline array<AbilityEmitterComponent> BulletComponents;
var(bullet) bool bShowTheFirstBullet;
var(bullet) bool bLinkFromLastTarget;

//native function CalculateClientEffect(Actor BaseActor, MMOAbility AbilityData, Actor Target, Vector TargetPoint, float InRate);


native function PushHitPointEffects(AbilityComponent Node,int CreatorId, Actor BaseActor, Actor Target, Vector TargetPoint, float Rate, float InMaxDistance = 800);


cpptext
{
	virtual FString GetResName();
	virtual struct BaseComponent* CreateCplusplusNode();
	virtual void CopyObjectProperties(UAbilityComponent* DstComponent);
}


// event Play(Actor BaseActor, MMOAbility AbilityData, Actor Target, Vector TargetPoint, float InRate, optional int SerialId = 0)
// {
// 	CalculateClientEffect(BaseActor, AbilityData, Target, TargetPoint, InRate);
// }


// only for mmoactor
event ClientPlay(int CreatorId, Actor BaseActor, MMOAbility AbilityData, Actor Target, Vector TargetPoint, float InRate)
{
	local int i;
	for(i = 0; i < ClientEffectList.Length; ++i)
	{
		//ClientEffectList[i].Play(Target, AbilityData, BaseActor, TargetPoint, InRate);
		PushHitPointEffects(ClientEffectList[i], CreatorId, Target, BaseActor, TargetPoint, InRate);
	}
}


event ServerPlay(int CreatorId, Actor BaseActor, MMOAbility AbilityData, Actor Target, Vector TargetPoint, float InRate)
{
	local int i;
	for(i = 0; i < ServerEffectList.Length; ++i)
	{
		//ServerEffectList[i].Play(Target, AbilityData, BaseActor, TargetPoint, InRate);
		PushHitPointEffects(ServerEffectList[i], CreatorId, Target, BaseActor, TargetPoint, InRate);
	}
}

// event PlayBullet(Actor BaseActor, MMOAbility AbilityData, Actor Target, Vector TargetPoint, float InRate)
// {
// 	//if(BulletComponent!=none)
// 	//{
// 	//	//BulletComponent.Play(BaseActor, AbilityData, Target, TargetPoint, InRate);
// 	//	PushHitPointEffects(BulletComponent, BaseActor, Target, TargetPoint, InRate);
// 	//}
// }

defaultproperties
{
	LineColor=(R=1.0,G=0.0,B=0.0,A=1.0)
	Degree=90
	Radius=500
	DamageValue=100
	TitleString="HitPoint"
	bHaveTrail=false
	HitPointId=-1
	AbilityDeath=DS_Anim
	bShowTheFirstBullet=true
	bLinkFromLastTarget=false
}
