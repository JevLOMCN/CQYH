class ProjectileTrack extends MMOEmitter;




//var(ProjectileTrack) editinline array<AbilityComponent>   OnHitFunctionalityTemplates;
//var(ProjectileTrack) editinline array<AbilityComponent>          OnStartEffectTemplates;
//var(ProjectileTrack) editinline array<AbilityComponent>          OnHitTargetEffectTemplates;

//var(ProjectileTrack) ETrackTargetType TrackTargetType;
//var(ProjectileTrack) float Speed;
//var(ProjectileTrack) float Acc;
//var(ProjectileTrack) float MaxLineDistance  <DisplayName="max impact distance of straight line type">;
//var(ProjectileTrack) int UpDegree    <DisplayName="parabola upward angle [0,90)">;
//var(ProjectileTrack) float TimeToFinishFixRotation  <DisplayName="orientation correction time ratio (0,2)">;
//var(ProjectileTrack) ETrackLineType TrackLineType;
//var(ProjectileTrack) float CollisionSizeForTrack;
////var(ProjectileTrack) ETargetRelationType TargetRelationType  <DisplayName="target faction type">;

//var transient array<AbilityComponent>  OnHitFunctionalities;
//var transient array<AbilityComponent>  OnStartEffects;
//var transient array<AbilityComponent>  OnHitTargetEffects;

//var float CurrentSpeed;
//var bool bStarted;

//var Vector RotSpeed;

//var transient float RotatorLeftTime;


//function Init(MMOActor B, MMOAbility A, Actor C, optional Actor T, optional Vector L)
//{

//	local Vector tempVect;
//	if(TrackTargetType == ETTT_Line)
//	{
//		tempVect = L;
//		tempVect.Z = BaseCreator.Location.Z;
//		tempVect = Normal(tempVect - Location)*MaxLineDistance+tempVect;
//	}
//	else
//	{
//		tempVect = L;
//	}

//	super.Init(B, A, C, T, tempVect);

//	bStarted = false;

//	SetCollisionSize(CollisionSizeForTrack, CollisionSizeForTrack);

//	if(UpDegree < 0 || UpDegree >= 90)
//	{
//		UpDegree = 45;
//	}

//	if(TimeToFinishFixRotation <= 0)
//	{
//		TimeToFinishFixRotation = 0.1f;
//	}
//	else if(TimeToFinishFixRotation > 3.0)
//	{
//		TimeToFinishFixRotation = 3.0f;
//	}
//}


//function Start(optional Actor Other, optional vector HitLocation, optional vector HitNormal, optional TraceHitInfo HitInfo)
//{
//	local int i;
//	local float tempLen;
//	local Rotator tempRot;

//	bStarted = true;


//	OnStartEffects.Remove(0, OnStartEffects.Length);
//	OnStartEffects.Add(OnStartEffectTemplates.Length);
//	for(i = 0; i < OnStartEffectTemplates.Length; ++i)
//	{
//		OnStartEffects[i] = new OnStartEffectTemplates[i].Class (OnStartEffectTemplates[i]);
//		OnStartEffects[i].Init(BaseCreator,AbilityOwner, Creator, Target, TargetLocation);
//		OnStartEffects[i].Start(Other, HitLocation, HitNormal);
//	}

//	if(TrackTargetType == ETTT_Point)
//	{
//		CurrentSpeed = Speed;
//		if(TrackLineType == ETLT_Line)
//		{
//			Velocity = Normal(TargetLocation - Location)*CurrentSpeed;
//		}
//		else if(TrackLineType == ETLT_Parabola)
//		{
//			tempRot = Rotation;
//			tempRot.Pitch+= UpDegree*182.0444;
//			Velocity = CurrentSpeed*Normal(Vector(Normalize(tempRot)));
//			tempLen = Vsize(TargetLocation - Location);
//			RotatorLeftTime = TimeToFinishFixRotation*tempLen/CurrentSpeed;
//			RotSpeed = (Normal(TargetLocation - Location) - Normal(Velocity))/RotatorLeftTime;
//			DrawDebugLine(Location, Location+Velocity, 255,0,0);
//		}
//		else
//		{
//			// todo
//		}
//		SetRotation(Rotator(TargetLocation - Location));

//	}
//	else if(TrackLineType == ETTT_Line)
//	{
//		CurrentSpeed = Speed;
//		Velocity = Normal(TargetLocation - Location)*CurrentSpeed;
//	}
//	else if(TrackTargetType == ETTT_Actor)
//	{
//		CurrentSpeed = Speed;
//		if(TrackLineType == ETLT_Line)
//		{
//			Velocity = Normal(Target.Location - Location)*CurrentSpeed;
//		}
//		else if(TrackLineType == ETLT_Parabola)
//		{
//			tempRot = Rotation;
//			tempRot.Pitch+= UpDegree*182.0444;
//			Velocity = CurrentSpeed*Normal(Vector(Normalize(tempRot)));
//			tempLen = Vsize(Target.Location - Location);
//			RotatorLeftTime = TimeToFinishFixRotation*tempLen/CurrentSpeed;
//			RotSpeed = (Normal(Target.Location - Location) - Normal(Velocity))/RotatorLeftTime;
//			DrawDebugLine(Location, Location+Velocity, 255,0,0);
//		}
//		else
//		{
//			// todo
//		}
//		SetRotation(Rotator(Velocity));
//	}
//}

//function Tick(float DeltaTime)
//{
//	//local float ret;
//	local Vector dir, NewLocation;
//	super.Tick(DeltaTime);

//	if(!bStarted)
//	{
//		return;
//	}


//	if(TrackTargetType == ETTT_Point)
//	{
//		dir = TargetLocation - Location;

//		//ret = Dot(Velocity, dir);
//		//if(Velocity Dot dir <= 0.0f)
//		//{
//		//	bStarted = false;
//		//	OnHit();
//		//	return;
//		//}

//		CurrentSpeed += DeltaTime*Acc;
//		if(TrackLineType == ETLT_Line)
//		{
//			Velocity = Normal(dir)*CurrentSpeed;
//		}
//		else if(TrackLineType == ETLT_Parabola)
//		{
//			if(RotatorLeftTime > 0)
//			{
//				Velocity = CurrentSpeed*Normal(Normal(Velocity)+RotSpeed*DeltaTime);
//				RotatorLeftTime -= DeltaTime;
//				RotSpeed = (Normal(dir) - Normal(Velocity))/RotatorLeftTime;
//			}
//			else
//			{
//				Velocity = CurrentSpeed*Normal(dir);
//			}

//			FlushPersistentDebugLines();
			
//			DrawDebugLine(Location, Location+Velocity, 255,0,0);
//		}
//		else
//		{
//			// todo
//		}
//		SetRotation(Rotator(Velocity));
//		NewLocation = Velocity*DeltaTime + Location;
//		SetLocation(NewLocation);

//		dir = TargetLocation - Location;

//		if(Velocity Dot dir <= 0.0f)
//		{
//			bStarted = false;
//			OnHit();
//			return;
//		}
//	}
//	else if(TrackLineType == ETTT_Line)
//	{
//		dir = TargetLocation - Location;

//		if(Velocity Dot dir <= 0.0f)
//		{
//			bStarted = false;
//			OnHit();
//			return;
//		}

//		CurrentSpeed += Acc*DeltaTime;
//		Velocity = Normal(TargetLocation - Location)*CurrentSpeed;
//	}
//	else if(TrackTargetType == ETTT_Actor)
//	{
//		CurrentSpeed += DeltaTime*Acc;
//		if(TrackLineType == ETLT_Line)
//		{
//			Velocity = Normal(Target.Location - Location)*CurrentSpeed;
//		}
//		else if(TrackLineType == ETLT_Parabola)
//		{
//			if(RotatorLeftTime > 0)
//			{
//				Velocity = CurrentSpeed*Normal(Normal(Velocity)+RotSpeed*DeltaTime);
//				RotatorLeftTime -= DeltaTime;
//				RotSpeed = (Normal(Target.Location - Location) - Normal(Velocity))/RotatorLeftTime;
//			}
//			else
//			{
//				Velocity = CurrentSpeed*Normal(Target.Location - Location);
//			}

//			FlushPersistentDebugLines();
			
//			DrawDebugLine(Location, Location+Velocity, 255,0,0);
//		}
//		else
//		{
//			// todo
//		}
//		SetRotation(Rotator(Velocity));

//		dir = Target.Location - Location;

//		if(Velocity Dot dir <= 0.0f)
//		{
//			bStarted = false;
//			OnHit();
//			return;
//		}
//	}

//	//if(CalcComponentType == ECT_UsingCalcComponent)
//	//{
//	//	for(i = 0; i < CurrentFunctionComps.Length; ++i)
//	//	{
//	//		CurrentFunctionComps[i].Tick(DeltaTime);
//	//	}
//	//}
//}

////function Stop(bool bInterupted)
////{
////	local int i;
////	bStarted = false;
////	for(i = 0; i < CurrentEffectComps.Length; ++i)
////	{
////		CurrentEffectComps[i].Stop(bInterupted);
////	}

////	for(i = 0; i < CurrentFunctionComps.Length; ++i)
////	{
////		CurrentFunctionComps[i].Stop(bInterupted);
////	}

////	Lifespan = 0.15f;
////	bCurrentlyActive = false;

////	//Destroy();
////}

//function bool IsCreator(Actor T)
//{
//	if(T == none)
//	{
//		return false;
//	}
//	if(T == BaseCreator || T == Creator)
//	{
//		return true;
//	}
//	return false;
//}

//function bool IsTarget(Actor T)
//{
//	//if(T.IsA('IAbilityDamageable'))
//	//{
//	//	if(TargetRelationType == ERT_Enemy)
//	//	{
//	//		if((BaseCreator.GetFriendlyMask()&IAbilityDamageable(T).GetFriendlyMask())==1)
//	//		{
//	//			return false;
//	//		}
//	//	}
//	//	else if(TargetRelationType == ERT_Self)
//	//	{
//	//		if((BaseCreator.GetFriendlyMask()&IAbilityDamageable(T).GetFriendlyMask())==0)
//	//		{
//	//			return false;
//	//		}
//	//	}
//	//}

//	if(T == none)
//	{
//		return false;
//	}
//	if(T == Target)
//	{
//		return true;
//	}
//	return false;
//}

//function OnHit(optional Actor Other, optional vector HitLocation, optional vector HitNormal)
//{
//	local int i;
//	`log("push new functionality component");

//	OnHitTargetEffects.Remove(0, OnHitTargetEffects.Length);
//	OnHitTargetEffects.Add(OnHitTargetEffectTemplates.Length);
//	for(i = 0; i < OnHitTargetEffectTemplates.Length; ++i)
//	{
//		OnHitTargetEffects[i] = new OnHitTargetEffectTemplates[i].Class (OnHitTargetEffectTemplates[i]);
//		OnHitTargetEffects[i].Init(BaseCreator, AbilityOwner,self, Other, HitLocation);
//		OnHitTargetEffects[i].Start();
//	}


//	OnHitFunctionalities.Remove(0, OnHitFunctionalities.Length);
//	OnHitFunctionalities.Add(OnHitFunctionalityTemplates.Length);
//	for(i = 0; i < OnHitFunctionalityTemplates.Length; ++i)
//	{
//		OnHitFunctionalities[i] = new OnHitFunctionalityTemplates[i].Class (OnHitFunctionalityTemplates[i]);
//		OnHitFunctionalities[i].Init(BaseCreator, AbilityOwner,self, Other, HitLocation);
//		OnHitFunctionalities[i].Start();
//	}

//	self.Destroy();
//}



DefaultProperties
{
	//Physics=PHYS_Projectile
	//bCollideActors=false
	//bBlockActors=false
	//CollisionType=COLLIDE_NoCollision
	//UpDegree=45
	//TimeToFinishFixRotation=0.7
	//MaxLineDistance=800
}
