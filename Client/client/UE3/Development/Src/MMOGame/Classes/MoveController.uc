/*
 * Movement control of all objects except the main character is executed by this class; this base class has only this function
 * If the game involves local AI objects, inherit from this class or use another AI Controller
 * 
 * 
 */
class MoveController extends AIController 
	native
	implements(IClientInteraction);

// transient
var bool bNeedFindPath; // whether pathfinding is needed to move to the target point

var transient EPhysics  OldPhysic;
var transient Vector    NextLocation;

//interface
native function OnLeftMouseKey(Actor srcActor,bool bRelease,Vector StartTrace,Vector EndTrace);

event PostBeginPlay()
{
	Super.PostBeginPlay();

	if ( !bDeleteMe && NavigationHandle == none)
	{
		InitNavigationHandle();
	}
}

event Possess(Pawn inPawn, bool bVehicleTransition)
{
	Pawn = inPawn;
}

function ScriptMoveTo(Vector Des, optional Vector FocusPoint, optional bool bFindPath = False)
{
	local vector zOffset;
	zOffset.X = 0;
	zOffset.Y = 0;
	if(CylinderComponent(CollisionComponent) != none)
	{
		zOffset.Z = CylinderComponent(CollisionComponent).CollisionHeight;
	}
	else
	{
		zOffset.Z = 0;
	}

	if(IsInState('ScriptMove'))
	{
		//`log("Last Move not finished, Call Move again");
		PopState();
		StopLatentExecution();
		GotoState('Idle');
	}

	bNeedFindPath = bFindPath;
	//MMOActor(Pawn).SetMoveType(EMT_Run);
	
	// add half the monster's height, the server uses the center of mass position

	SetDestinationPosition(Des + zOffset);

	PushState('ScriptMove');
}

function ScriptStopMove()
{
	if(IsInState('ScriptMove'))
	{
		PopState();
	}
}

auto state Idle
{
	// do nothing
}

state ScriptMove
{
	event PoppedState( )
	{
		local Vector Point;
		//`log("ScriptedMoveTo -> PoppedState");
		Pawn.ZeroMovementVariables();
		//MMOActor(Pawn).SetMoveType(EMT_Walk);
		//Pawn.SetPhysics(OldPhysic);
		Pawn.ZeroMovementVariables();
		Point = vect(0,0,0);
		SetFocalPoint(Point);
	}

	event PushedState( )
	{
		//`log("ScriptedMoveTo -> PushedState");
		OldPhysic = Pawn.Physics;
		//Pawn.SetPhysics(PHYS_NavMeshWalking);
	}

	function bool FindNavMeshPath()
	{
		if(NavigationHandle == none)
		{
			`warn("NavigationHandle is none");
			return false;
		}

		NavigationHandle.PathGoalList = none;
		NavigationHandle.PathConstraintList = none;

		class'NavMeshPath_Toward'.static.TowardPoint( NavigationHandle, GetDestinationPosition());
		class'NavMeshGoal_At'.static.AtLocation( NavigationHandle, GetDestinationPosition(), ,false);


		if(NavigationHandle.FindPath())
		{
			//NavigationHandle.DrawPathCache();
			return true;
		}
		else
		{
			return false;
		}
	}

	event Tick(float DeltaTime)
	{
		super.Tick(DeltaTime);

		ClientSetRotation(RInterpTo(Pawn.Rotation, MMOActor(Pawn).TargetRotation, DeltaTime, MMOActor(Pawn).RotationSpeed));
	}


Begin:

	NavigationHandle.SetFinalDestination(GetDestinationPosition());

	if(bNeedFindPath && !NavigationHandle.PointReachable(GetDestinationPosition()))
	{
		if(FindNavMeshPath())
		{
			//FlushPersistentDebugLines();
		}
		else
		{
			//`warn("can not find nav mesh path!");
			SetDestinationPosition(Pawn.Location);
		}
	}
	else
	{
		//SetFocalPoint(GetDestinationPosition());
		//DrawDebugLine(Pawn.Location, GetDestinationPosition(), 0,255,0,true);
		MMOActor( Pawn ).SetDirection(GetDestinationPosition());
		MoveTo(GetDestinationPosition(), none, -Pawn.GetCollisionRadius()+5);
	}

	while(Pawn != none)
	{
		if(VSize(Pawn.Location - GetDestinationPosition()) < 2 || Pawn.ReachedPoint(GetDestinationPosition(), none))
		{
			break;
		}
		if(NavigationHandle.GetNextMoveLocation(NextLocation, Pawn.GetCollisionRadius()))
		{
			//SetFocalPoint(NextLocation);
			//DrawDebugLine(Pawn.Location, NextLocation, Rand(256),Rand(256),Rand(256),true);
			MMOActor( Pawn ).SetDirection(GetDestinationPosition());
			MoveTo(NextLocation, none, -Pawn.GetCollisionRadius()+5);
		}
		else
		{
			break;
		}
	}
	PopState();
}





DefaultProperties
{
	bNeedFindPath = false
}
