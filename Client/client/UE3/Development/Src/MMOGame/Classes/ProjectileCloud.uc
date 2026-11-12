class ProjectileCloud extends MMOEmitter;


//enum EMovementType
//{
//	EMT_Independence,
//	EMT_AttachToTarget,
//};

//var(ProjectileTrack) editinline array<AbilityComponent>   OnHitFunctionalityTemplates;
//var(ProjectileTrack) editinline array<AbilityComponent>          OnStartEffectTemplates;
//var(ProjectileTrack) editinline array<AbilityComponent>          OnHitTargetEffectTemplates;

//var(ProjectileCloud) EMovementType  MovementType;
//var(ProjectileCloud) int CycleNumber;
//var(ProjectileCloud) float OneCycleTime;
//var(ProjectileCloud) float HitTime;
//var(ProjectileCloud) float RelativeHeight;
//var(ProjectileCloud) float HitRadius;
////var(ProjectileCloud)   ETargetRelationType TargetRelationType  <DisplayName="target faction type">;


//var transient array<AbilityComponent>  OnHitFunctionalities;
//var transient array<AbilityComponent>         OnStartEffects;
//var transient array<AbilityComponent>         OnHitTargetEffects;


//var array<Actor>    HitList;
//var int CurrentCycleNumber;
//var float CurrentPassTime;
//var bool bPushed;

//var bool bStarted;


//function Start(optional Actor Other, optional vector HitLocation, optional vector HitNormal, optional TraceHitInfo HitInfo)
//{
//	local int i;
//	local Vector NewLocation;

//	CurrentCycleNumber = 1;
//	CurrentPassTime = 0.0f;
//	bPushed=false;
//	bStarted=true;

//	NewLocation = TargetLocation;
//	NewLocation.Z += RelativeHeight;
//	SetLocation(NewLocation);

//	OnStartEffects.Remove(0, OnStartEffects.Length);
//	OnStartEffects.Add(OnStartEffectTemplates.Length);
//	for(i = 0; i < OnStartEffects.Length; ++i)
//	{
//		OnStartEffects[i] = new OnStartEffectTemplates[i].Class (OnStartEffectTemplates[i]);
//		OnStartEffects[i].Init(BaseCreator, AbilityOwner, Creator, Target, TargetLocation);
//		OnStartEffects[i].Start();
//	}
//}

//function Stop(bool bForceKill)
//{
//	local int i;

//	for(i = 0; i < OnStartEffects.Length; ++i)
//	{
//		OnStartEffects[i].Stop(bForceKill);
//	}
//}

//function Tick(float DeltaTime)
//{
//	local Vector NewLocation;
//	super.Tick(DeltaTime);

//	if(!bStarted)
//	{
//		return;
//	}

//	if(CurrentCycleNumber > CycleNumber)
//	{
//		self.Stop(false);
//		self.Destroy();
//		return;
//	}

//	CurrentPassTime += DeltaTime;

//	if(!bPushed)
//	{
//		if(CurrentPassTime >= HitTime)
//		{
//			bPushed = true;
//			OnCalc();
//		}
//	}

//	if(CurrentPassTime >= OneCycleTime)
//	{
//		CurrentCycleNumber++;
//		CurrentPassTime=0.0f;
//		bPushed=false;
//	}

//	if(MovementType == EMT_AttachToTarget)
//	{
//		NewLocation = Target.Location;
//		NewLocation.Z += RelativeHeight;
//		SetLocation(NewLocation);
//	}
//	else if(MovementType == EMT_Independence)
//	{
//		//NewLocation = TargetLocation;
//		//NewLocation.Z += RelativeHeight;
//		//SetLocation(NewLocation);
//	}
//}

//function bool IsNewTarget(Actor TargetActor)
//{
//	//local int i;
//	//if(IAbilityDamageable(TargetActor) != none)
//	//{
//	//	if(TargetRelationType == ETRT_Enemy)
//	//	{
//	//		if((BaseCreator.GetFriendlyMask()&IAbilityDamageable(TargetActor).GetFriendlyMask())> 0)
//	//		{
//	//			return false;
//	//		}
//	//	}
//	//	else if(TargetRelationType == ETRT_Self)
//	//	{
//	//		if((BaseCreator.GetFriendlyMask()&IAbilityDamageable(TargetActor).GetFriendlyMask())==0)
//	//		{
//	//			return false;
//	//		}
//	//	}
//	//}

//	//for(i = 0; i < HitList.Length; ++i)
//	//{
//	//	if(HitList[i] == TargetActor)
//	//	{
//	//		return false;
//	//	}
//	//}
//	return true;
//}

//function OnCalc()
//{
//	local Vector tempVec;
//	local Actor TracedActor;
//	tempVec = Location;
//	tempVec.Z -= RelativeHeight;
//	foreach BaseCreator.VisibleCollidingActors(Class'Actor', TracedActor, HitRadius, tempVec, false)
//	{
//		if(IsNewTarget(TracedActor))
//		{
//			PushEffectToTarget(TracedActor);
//			HitList.AddItem(TracedActor);
//			//`log("TracedActor is "$TracedActor);
//		}
//	}
//	DrawDebugSphere(tempVec, HitRadius, 10, 255,255,255);
//}

//function PushEffectToTarget(Actor T)
//{
//	local int i;

//	OnHitTargetEffects.Remove(0, OnHitTargetEffects.Length);
//	OnHitTargetEffects.Add(OnHitTargetEffectTemplates.Length);
//	for(i = 0; i < OnHitTargetEffects.Length; ++i)
//	{
//		OnHitTargetEffects[i] = new OnHitTargetEffectTemplates[i].Class (OnHitTargetEffectTemplates[i]);
//		OnHitTargetEffects[i].Init(BaseCreator,AbilityOwner, self, T, TargetLocation);
//		OnHitTargetEffects[i].Start();
//	}

//	OnHitFunctionalities.Remove(0, OnHitFunctionalities.Length);
//	OnHitFunctionalities.Add(OnHitFunctionalityTemplates.Length);
//	for(i = 0; i < OnHitFunctionalities.Length; ++i)
//	{
//		OnHitFunctionalities[i] = new OnHitFunctionalityTemplates[i].Class (OnHitFunctionalityTemplates[i]);
//		OnHitFunctionalities[i].Init(BaseCreator, AbilityOwner,self, T, TargetLocation);
//		OnHitFunctionalities[i].Start();
//	}
//}

DefaultProperties
{
	//bStarted=false
}
