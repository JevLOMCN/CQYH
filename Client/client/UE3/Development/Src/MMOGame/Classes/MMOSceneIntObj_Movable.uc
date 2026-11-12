class MMOSceneIntObj_Movable extends SkeletalMeshActor
	placeable
	ClassGroup(__MMO__);

var(MMOSceneObj) SplineActor MovePath <DisplayName="MovePath">;
var(MMOSceneObj) SplineActor MovePathLast <DisplayName="MovePathLast">;

// If not looping, it moves back and forth
var(MMOSceneObj) bool LoopPath <DisplayName="LoopPath">;

var(MMOSceneObj) bool AlignPathNormal <DisplayName="AlignPathNormal">;
var(MMOSceneObj) bool AlignPathDirection <DisplayName="AlignPathDirection">;

var(MMOSceneObj) float StartPosOffsetPct <DisplayName="StartPosOffsetPct">;
var(MMOSceneObj) float MoveSpeed <DisplayName="MoveSpeed">;

// Current movement direction
var(MMOSceneObj) bool CurrentMoveForward;

// Current waypoint object
var SplineActor CurrentTargetPathNode;

// Previous or next valid node
var SplineActor LastValidNode;

// Current waypoint index
var int CurrentTargetPathNodeIdx;

simulated event PostBeginPlay()
{
	super.PostBeginPlay();
}

function vector GetMeshWorldPosition()
{
	return Location;
}

function SetMeshWorldPosition(Vector pos)
{
	SetLocation(pos);
}

state InActive
{
	event BeginState(Name PreviousStateName)
	{
		// PlayAnimation("InActive");
	}

	event EndState(Name NextStateName)
	{
	}

Begin:
}

state Activate
{
	event BeginState(Name PreviousStateName)
	{
		//`log("______ Movable Active");
		CurrentTargetPathNode = MovePath;
		CurrentTargetPathNodeIdx = 0;
	}

	event EndState(Name NextStateName)
	{
	}

	function vector UpdateForward(float deltaTime)
	{
		local Vector moveDir;
		//local SplineActor prevNode;

		if(CurrentTargetPathNode != none)
		{
			// Save a node first
			if(CurrentTargetPathNode.LinksFrom.Length > 0)
			{
				LastValidNode = CurrentTargetPathNode.LinksFrom[0];
			}

			if(VSize(CurrentTargetPathNode.Location - GetMeshWorldPosition()) < 60)
			{
				// If there is a next node, continue; otherwise return
				if(CurrentTargetPathNode.Connections.Length > 0)
				{
					CurrentTargetPathNode = CurrentTargetPathNode.Connections[0].ConnectTo;
				}
				else
				{
					CurrentTargetPathNode = none;
				}
			}

			if(CurrentTargetPathNode != none)
			{
				moveDir = Normal(CurrentTargetPathNode.Location - GetMeshWorldPosition());
			}
			
			if(CurrentTargetPathNode == none)
			{
				// Switch to moving backward
				CurrentMoveForward = false;
				CurrentTargetPathNode = LastValidNode;
			}
		}

		return moveDir;


		//`log("______ CurrentMoveForward" @VSize(EndNode.Location - curPos));
	}

	function vector UpdateBackward(float deltaTime)
	{
		local Vector moveDir;
		//local SplineActor nextNode;

		if(CurrentTargetPathNode != none)
		{
			// Save the next node first
			if(CurrentTargetPathNode.Connections.Length > 0)
			{
				LastValidNode = CurrentTargetPathNode.Connections[0].ConnectTo;
			}

			if(VSize(CurrentTargetPathNode.Location - GetMeshWorldPosition()) < 60)
			{
				// If there are nodes ahead, continue moving backward
				if(CurrentTargetPathNode.LinksFrom.Length > 0)
				{
					CurrentTargetPathNode = CurrentTargetPathNode.LinksFrom[0];
				}
				else
				{
					CurrentTargetPathNode = none;
				}
			}

			if(CurrentTargetPathNode != none)
			{
				moveDir = Normal(CurrentTargetPathNode.Location - GetMeshWorldPosition());
			}
			
			if(CurrentTargetPathNode == none)
			{
				// Switch to moving forward
				CurrentMoveForward = true;
				CurrentTargetPathNode = LastValidNode;
			}
		}
		
		return moveDir;
	}

	// Back-and-forth movement, return the forward direction
	function vector UpdateCruiseMovement(float deltaTime)
	{
		// Determine direction first
		if(CurrentMoveForward)
		{
			return UpdateForward(deltaTime);
		}
		else
		{
			return UpdateBackward(deltaTime);
		}
	}

	// Looping movement, return the forward direction
	function vector UpdateLoopMovement(float deltaTime)
	{
		// Determine direction first
		if(CurrentMoveForward)
		{
			return UpdateLoopForward(deltaTime);
		}
		else
		{
			return UpdateLoopBackward(deltaTime);
		}
	}

	function vector UpdateLoopForward(float deltaTime)
	{
		local Vector moveDir;

		if(CurrentTargetPathNode != none)
		{
			if(VSize(CurrentTargetPathNode.Location - GetMeshWorldPosition()) < 60)
			{
				// If there is a next node, continue; otherwise return
				if(CurrentTargetPathNode.Connections.Length > 0)
				{
					CurrentTargetPathNode = CurrentTargetPathNode.Connections[0].ConnectTo;
				}
				else
				{
					CurrentTargetPathNode = none;
				}
			}

			if(CurrentTargetPathNode != none)
			{
				moveDir = Normal(CurrentTargetPathNode.Location - GetMeshWorldPosition());
			}
			
			if(CurrentTargetPathNode == none)
			{
				CurrentTargetPathNode = MovePath;
			}
		}

		return moveDir;
	}

	function vector UpdateLoopBackward(float deltaTime)
	{
		local Vector moveDir;

		if(CurrentTargetPathNode != none)
		{
			if(VSize(CurrentTargetPathNode.Location - GetMeshWorldPosition()) < 60)
			{
				// If there are nodes ahead, continue moving backward
				if(CurrentTargetPathNode.LinksFrom.Length > 0)
				{
					CurrentTargetPathNode = CurrentTargetPathNode.LinksFrom[0];
				}
				else
				{
					CurrentTargetPathNode = none;
				}
			}

			if(CurrentTargetPathNode != none)
			{
				moveDir = Normal(CurrentTargetPathNode.Location - GetMeshWorldPosition());
			}
			
			if(CurrentTargetPathNode == none)
			{
				CurrentTargetPathNode = MovePathLast;
			}
		}
		
		return moveDir;
	}

	simulated event Tick(float deltaTime)
	{		
		local Vector curPos, moveDir;//, curDir;
		curPos = GetMeshWorldPosition();

		if(LoopPath)
		{
			moveDir = UpdateLoopMovement(deltaTime);
		}
		else
		{
			moveDir = UpdateCruiseMovement(deltaTime);
		}
	
		if(AlignPathDirection)
		{
			//curDir = vector(Rotation);
			//curDir = moveDir - curDir; 
			//SetRotation(Rotation + Rotator(curDir * deltaTime));
			SetRotation(Rotator(moveDir));
		}

		curPos += moveDir * MoveSpeed * deltaTime;
		
		SetMeshWorldPosition(curPos);

		//`log("______ Movable Tick" @CurrentMoveForward);
	}

Begin:
	//Sleep( PlayAnimation( AnimNamePrepare ) );
}

auto state Prepare
{
	event BeginState(Name PreviousStateName)
	{
		//SetMeshWorldPosition(StartNode.Location + (EndNode.Location - StartNode.Location) * StartPosOffsetPct);
	}

	event EndState(Name NextStateName)
	{
	}


	event Tick( float DeltaTime )
	{
		//if( RefVolume != none )
		//	RefVolume.Update( DeltaTime );
	}

Begin:
	//if( RefVolume != none )
	//	RefVolume.EnableVolume( );
	//Sleep( PlayAnimation( AnimNameActivate ) );
	GotoState( 'Activate' );
}

state ResetAll
{
Begin:
	GotoState( 'InActive' );
}

state Dead
{

}

DefaultProperties
{
	Begin Object Name=SkeletalMeshComponent0
		SkeletalMesh=SkeletalMesh'EditorMeshes.SkeletalMesh.DefaultSkeletalMesh'
	End Object

	//Begin Object Class=CylinderComponent Name=CollisionCylinder
	//	CollisionRadius=+0034.000000
	//	CollisionHeight=+0078.000000
	//	BlockNonZeroExtent=true
	//	BlockZeroExtent=true
	//	BlockActors=true
	//	CollideActors=true
	//End Object
	//CollisionComponent=CollisionCylinder
	//CylinderComponent=CollisionCylinder
	//Components.Add(CollisionCylinder)

	MoveSpeed=500
	AlignPathNormal=true
	AlignPathDirection=false
	CurrentMoveForward=true
	LoopPath=false

	bStatic=false
	bCollideActors=True
	bBlockActors=True
	bMovable=true
	//bAlwaysRelevant=true
	//bReplicateMovement=true
	//bOnlyDirtyReplication=true
	RemoteRole=ROLE_None
	bWorldGeometry=false
}
