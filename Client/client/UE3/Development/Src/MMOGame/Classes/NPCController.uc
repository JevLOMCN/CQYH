//
// npc controller
//
class NPCController extends AIController
	dependson(NPC)
	config(MMONPC);

var MMOActor    m_TargetActor;          // the actor this NPC is interacting with

// navigaion variables
var MMONavigationHelper m_NavHelper;
var Vector      m_vecDestination;          // the destination this NPC is going to
var Vector      m_vecLocOnPath;            // temp location on the path to the destination

var Vector      m_vecSpotPlayerLoc;        // location that the player was spotted
var Vector      m_vecSpotPlayerSelfLoc;    // self location when the player was spotted
var Vector      m_vecEscapeLoc;            // location the npc started escape
var Vector      m_vecEscapeDir;            // escape direction
var int         m_nEscapeChance;           // max chances for the NPC to escape, 1 for most of the time

/*
var Vector      m_vecBeatBackDir;
var float       m_fBeatBackTimeTotal;      // note: the value should NOT be larger than the length of the hurt animation.
var float       m_fBeatBackTimePast;
var bool        m_bBeatBack;
var bool        UseHurtAnim;
*/

var float       AttackIdleTime;

var array< MMOActor >   ActorsWithinSight;

var float       AttackAngle;
var int         AttackSlot;
var Vector      AttackOffset;
var float       FRandNum;

var float       m_fLogicUpdateTime;
var float       m_fLogicPastTime;

// climb related date
var Volume      TargetVolumeForClimbing;
var bool        IsClimbing;
var bool        ShouldJump;
var Vector      ClimbTranslate;

var int NextAbilityID;      // next ability to use

var MMOJumpDownPoint    JumpDownPoint;

var float       LastFallingVelocity;

var int         CurrentPatrolNode;

var int BeatBackDist;

//var Sequence  AISequence;     // can use the kismet to do AI


/// Hit temporary attributes
var transient AbilityHitEffectComponent CurrentHitEffect;

/** Set direction for the NPC
 * @param lookAt the poing this NPC is going to look at
 */
function SetDirection( Vector lookAt )
{
	local Vector PawnLocationXY;
	local Vector DestLocationXY;

	return;

	PawnLocationXY.X = Pawn.Location.X;
	PawnLocationXY.Y = Pawn.Location.Y;

	DestLocationXY.X = lookAt.X;
	DestLocationXY.Y = lookAt.Y;

	ClientSetRotation( Rotator( DestLocationXY - PawnLocationXY ) );
	//Pawn.SetDesiredRotation( Rotator( DestLocationXY - PawnLocationXY ) );
}

/** post beging play, initial the NavHelper
 */
simulated event PostBeginPlay()
{
	//local array< SequenceObject > seqs;
	//local SequenceObject seq;

	super.PostBeginPlay();

	m_NavHelper = new( self ) class'MMONavigationHelper';
	m_NavHelper.Setup( self );

	CurrentPatrolNode = 0;
	

	// for AI using Kismet
	//WorldInfo.GetGameSequence().FindSeqObjectsByClass( class'Sequence', false, seqs );
	//foreach seqs( seq )
	//{
	//	if( seq.Name == 'NPC_AI' )
	//	{
	//		AISequence = new(self) class'Sequence'(seq);
	//		break;
	//	}
	//}
	//if( AISequence != none )
	//{
	//	seqs.Remove( 0, seqs.Length );
	//	AISequence.FindSeqObjectsByClass( class'SeqEvent_SequenceActivated', false, seqs );
	//	`Log( "Seq length: "@seqs.Length );
	//	foreach seqs( seq )
	//	{
	//		`Log( "Seq name: "@seq.Name );
	//		SeqEvent_SequenceActivated(seq).CheckActivate( self, self, false );
	//	}
	//}
}


function AbilityEnd(int id)
{
	// Ability end response function
}

/** found the target player and prepare to attack
 *  could happen by seeing the player or attacked by the player etc.
 */
function SpotPlayer( MMOActor targetPlayer )
{
	if( m_TargetActor != targetPlayer )
	{
		m_TargetActor = targetPlayer;
		m_vecSpotPlayerLoc = m_TargetActor.Location;
		m_vecSpotPlayerSelfLoc = Pawn.Location;
		//`Log( "Spot Player "@targetPlayer.Name );
	}
	if( NPC(Pawn).bDebugRanges )
	{
		NPC(Pawn).TraceRangeCylinder.SetHidden( false );
		NPC(Pawn).TraceRangeCylinder.SetTranslation( m_vecSpotPlayerLoc );
	}
}

/** check whether the TargetLoc is within a certain range of the OrigLoc
 */
function bool IsWithinRange( Vector OrigLoc, Vector TargetLoc, float Range )
{
	local Vector diff;
	diff = OrigLoc - TargetLoc;
	if( diff.X * diff.X + diff.Y * diff.Y + diff.Z * diff.Z <= Range * Range )
		return true;
	return false;
}

function bool IsTooCloseToTarget(MMOActor Target)
{
	local float len;
	if(Target == none)
	{
		return false;
	}

	len = Target.GetCollisionRadius() + Pawn.GetCollisionRadius();

	if(len*len > VSizeSq(Target.Location - Pawn.Location))
	{
		return true;
	}
	return false;
}

/** the npc I'm following changes its destination so I should follow
 */
function NotifyDestChange( Vector newDest )
{
	if( IsInState( 'Idle' ) && NPC(Pawn).WanderType == EWanderType_Follow )
	{
	}
}


/** NPC Behaviors */
// todo: use different behaviors in different states to make a better AI

/** Idle state, the NPC will standing along or walking around in a certain area depends on its pawn's WanderType */
auto state Idle
{
	/*
	event EndState( Name NextStateName )
	{
		super.EndState( NextStateName );
		StopLatentExecution();
		Pawn.ZeroMovementVariables();
		//`Log(self$" Pawn is "$pawn$"Leave Idle State from "@NextStateName );
	}

	event BeginState( Name PreviousStateName )
	{
		super.BeginState( PreviousStateName );
		//`Log( self$" Pawn is "$pawn$"[MMO_Debug]Enter Idle State from "@PreviousStateName );
	}

	event PoppedState()
	{
		super.PoppedState();
	}

	event PushedState()
	{		
		super.PushedState();
		//if (Pawn != None)
		//{
			//this controls the physics asset, i.e. the skeletal animation controlling character movement.
			//Pawn.SetMovementPhysics();
		//}

		if( Pawn == none )
		{
			return;
		}
		Pawn.SetMovementPhysics();
	}

	/** get the next destination this NPC want to go to
	 *  @return true if found a good location
	 */
	function bool GetNextDest( )
	{
		local float diffX;
		local float diffY;
		local float randDist, randAngle;
		local Vector dest;
		local PathNode pnode;
		if( Pawn == none )
			return false;
		if( NPC( Pawn ).WanderType == EWanderType_Stand )
		{
			return false;
		}
		else if( NPC(Pawn).WanderType == EWanderType_Follow )
		{
			if( NPC(Pawn).NPCToFollow == none )
			{
				NPC(Pawn).WanderType = EWanderType_Stand;
				return false;
			}
			m_vecDestination = NPCController( NPC(Pawn).NPCToFollow.Controller ).m_vecDestination + NPC(Pawn).TransToLead;
			return true;
		}
		else if( NPC(Pawn).WanderType == EWanderType_Patrol )
		{
			if( NPC(Pawn).PatrolPath.Length <= 1 )
				return false;
			++CurrentPatrolNode;
			if( CurrentPatrolNode >= NPC(Pawn).PatrolPath.Length )
			{
				CurrentPatrolNode = 0;
			}
			if( m_NavHelper.FindPathToDest( NPC(Pawn).PatrolPath[CurrentPatrolNode].Location ) )
			{
				m_vecDestination = NPC(Pawn).PatrolPath[CurrentPatrolNode].Location;
				return true;
			}
			else
			{   // search for a reachable point
				foreach NPC(Pawn).PatrolPath( pnode )
				{
					//if( m_NavHelper.FindPathToDest( pnode.Location ) )
					if( PointReachable( pnode.Location ) )
					{
						m_vecDestination = pnode.Location;
						return true;
					}
				}
			}
		}
		else// if( IsWithinRange( NPC(Pawn).m_vecOrigLoc, Pawn.Location, NPC(Pawn).m_fPatrolRange ) )
		{
			randDist = Rand( NPC(Pawn).m_fPatrolRange );
			randAngle = Rand( 6.28 );
			diffX = cos( randAngle ) * randDist;
			diffY = sin( randAngle ) * randDist;
			//dest = NPC(Pawn).m_vecOrigLoc;
			dest = Pawn.Location;
			dest.X += diffX;
			dest.Y += diffY;
			//if( m_NavHelper.FindPathToDest( dest ) )
			if( PointReachable( dest ) )
			{
				m_vecDestination = dest;
				return true;
			}
		}
		//else
		//{   // the NPC walked too far away( maybe because it was tracing a player ), walk back to origin location
		//	m_vecDestination = NPC(Pawn).m_vecOrigLoc;
			//StopLatentExecution( );
			//Pawn.Acceleration.X = 0;
			//Pawn.Acceleration.Y = 0;
			//Pawn.Acceleration.Z = 0;
		//	return true;
		//}
		return false;
	}

	/** AI to select the proper player to attack
	 */
	function MMOPlayer SelectPlayerToAttack( )
	{
		local MMOPlayer result;
		if( ActorsWithinSight.Length != 0 )
		{
			result = MMOPlayer( ActorsWithinSight[0] );
			ActorsWithinSight.Remove( 0, ActorsWithinSight.Length );
			return result;
		}
		return none;
	}

	simulated event Tick( float deltaTime )
	{
		local MMOPlayer playerActor;

		if( Pawn == none )
			return;

		if( NPC(Pawn).IsMonster() )
		{
			if( m_TargetActor != none )
			{
				GotoState( 'TracePlayer' );
				return;
			}
			else if( NPC(Pawn).m_bIsPossitive )
			{   // search for player within sight range
				//foreach VisibleActors( class'MMOActor', playerActor, NPC(Pawn).m_fSightRange )
				//{
				//	if( playerActor.Class == class'MMOPlayer' )
				//		ActorsWithinSight.AddItem( playerActor );
				//}
				m_fLogicPastTime += deltaTime;
				if( m_fLogicPastTime >= m_fLogicUpdateTime )
				{
					m_fLogicPastTime = 0.f;
					foreach VisibleActors( class'MMOPlayer', playerActor, NPC(Pawn).m_fSightRange, Pawn.Location)
					{
						if( IsWithinRange( Pawn.Location, playerActor.Location, NPC(Pawn).m_fSightRange ) )
						{
							ActorsWithinSight.AddItem( playerActor );
						}
					}
					playerActor = SelectPlayerToAttack( );
					if( playerActor != none )
					{
						SpotPlayer( playerActor );
						StopLatentExecution( );
						Pawn.ZeroMovementVariables();

						GotoState( 'TracePlayer' );
						return;
					}
					else
					{
						MMOActor(Pawn).PlayRandomAnim();
					}
				}
			}
		}
	}
Init:
	Sleep( 1 + FRand() * 5 );
	goto( 'Begin' );

Begin:
	if( Pawn == none )
		goto( 'NEXT_ACTION' );

	MMOActor(Pawn).MoveType = EMT_Walk;
	Pawn.GroundSpeed = MMOActor(Pawn).GetMoveSpeed();
	//MMOActor(Pawn).IsWalking = true;
	if( NPC(Pawn).bDebugRanges )
	{
		NPC(Pawn).TraceRangeCylinder.SetHidden( true );
	}
	if( GetNextDest( ) )
	{
		Pawn.SetMovementPhysics();
		//m_NavHelper.BeginPathing( m_vecDestination );
		//while( Pawn != none && !Pawn.ReachedPoint( m_vecDestination, none ) )
		//{
		//	if( PointReachable( m_vecDestination ) )
		//	{
				//SetDirection( m_vecDestination );
				MoveTo( m_vecDestination );
			//}
			//else if( m_NavHelper.GetNextMoveLocation( m_vecLocOnPath, Pawn.GetCollisionRadius( ) ) )
			//{
			//	//SetDirection( m_vecLocOnPath );
			//	`Log( "Idle MoveTo Pathing"@Pawn.Name );
			//	MoveTo( m_vecLocOnPath );
			//}
		//	else
		//	{
				//`Log( "Can't find the next move location" );
		//		goto( 'NEXT_ACTION' );
		//	}
		//}
	}
NEXT_ACTION:
	Sleep( 3 );
	goto( 'Begin' );    // the idle state will be break by events such as take damage or mouse hit

*/
}

state TracePlayer
{
	event PoppedState( )
	{
		m_TargetActor = none;
		super.PoppedState( );
	}

	event PushedState( )
	{
		super.PushedState( );

		Pawn.SetPhysics( PHYS_WALKING );

		m_vecDestination = m_TargetActor.Location;
		m_fLogicPastTime = 0.f;
	}

	event BeginState( Name PreviousStateName )
	{
		super.BeginState( PreviousStateName );
		//`Log( self$"Enter TracePlayer State from "@PreviousStateName );

		Pawn.SetMovementPhysics( );

		ChooseNextAbility( );
		//MMOActor( Pawn ).CurrentAttackRange = NPC( Pawn ).GetAttackRange( NextAbilityID )*0.6;

		if( LockAttackPosition( ) )
		{
			m_vecDestination = m_TargetActor.Location + AttackOffset;
		}
		else
		{
			//`Log( "[MMO_Debug]Can't find attack position" );
			m_vecDestination = m_TargetActor.Location;
		}
		m_fLogicPastTime = 0.f;

		//`Log( "[MMO_Debug]Trace Player"@PreviousStateName@AttackSlot );

		if( NPC(Pawn).HideWeaponWhenRunning )
		{
			NPC(Pawn).HideNPCWeapon( true );
		}
	}

	event EndState( Name NextStateName )
	{
		super.EndState( NextStateName );

		//`Log( self$"Leave TracePlayer State To "@NextStateName );
		StopLatentExecution();
		Pawn.ZeroMovementVariables();

		FreeAttackPosition();

		if( NPC(Pawn).HideWeaponWhenRunning )
		{
			NPC(Pawn).HideNPCWeapon( false );
		}
	}

	simulated event Tick( float deltaTime )
	{
		if( Pawn == none )
		{
			//`Log( "Pawn missed! Goto Idle State." );
			m_TargetActor = none;
			GotoState( 'Idle' );
			return;
		}
		if( m_TargetActor != none )
		{
			if( IsWithinRange( Pawn.location, m_TargetActor.Location, MMOActor( Pawn ).CurrentAttackRange ) )
			{
				if( AttackSlot == -1 )
				{
					GotoState( 'Watch' );
					return;
				}
				else
				{
					GotoState( 'Position' );
					return;
				}
			}
			if( NPC(Pawn).ActorType == EACT_Boss )
			{
				if( !IsWithinRange( NPC(Pawn).m_vecOrigLoc, m_TargetActor.Location, NPC(Pawn).m_fTraceRange ) )
				{
					//GotoState( 'RunBack' );
					m_TargetActor = none;
					GotoState( 'Idle' );
					return;
				}
			}
			else
			{
				if( !IsWithinRange( m_vecSpotPlayerLoc, m_TargetActor.Location, NPC(Pawn).m_fTraceRange ) )
				{
					//GotoState( 'RunBack' );
					m_TargetActor = none;
					GotoState( 'Idle' );
					return;
				}
			}
		}
		else
		{
			//GotoState( 'RunBack' );
			GotoState( 'Idle' );
			return;
		}

		m_fLogicPastTime += deltaTime;
		//`log("20, TargetActor = "$m_TargetActor);
		//`log("m_fLogicPastTime="$m_fLogicPastTime$"  deltaTime="$deltaTime);
		if( m_fLogicPastTime >= m_fLogicUpdateTime && m_TargetActor != none )
		{
			//if( m_NavHelper.FindPathToDest( m_vecDestination ) )
			//{
			//	m_vecDestination = m_TargetActor.Location;
			//}
			//else
			//{
			//	m_vecDestination = Pawn.Location;
			//}
			if( AttackSlot != -1 )
			{
				m_vecDestination = m_TargetActor.Location + AttackOffset;
				if( !m_NavHelper.FindPathToDest( m_vecDestination ) )
				{
					FreeAttackPosition();
					m_vecDestination = m_TargetActor.Location;
				}
				else
				{
				StopLatentExecution( );
					m_NavHelper.BeginPathing( m_vecDestination );
				}
			}
			else
			{
				m_vecDestination = m_TargetActor.Location;
			}
			//StopLatentExecution( );
			m_fLogicPastTime -= m_fLogicUpdateTime;
		}
	}

Begin:
	StopLatentExecution();
	Pawn.Acceleration.X = 0;
	Pawn.Acceleration.Y = 0;
	Pawn.Acceleration.Z = 0;
	Sleep( 0.4f );
	if( Pawn == none || m_TargetActor == none )
	{
		GotoState( 'Idle' );
		Goto('End');
	}
	if( IsWithinRange( Pawn.location, m_TargetActor.Location, MMOActor( Pawn ).CurrentAttackRange ) )
	{
		GotoState( 'Attack' );
		Goto('End');
	}

	//MMOActor(Pawn).MoveType = EMT_Run;
	//Pawn.GroundSpeed = MMOActor(Pawn).GetMoveSpeed();
	//MMOActor(Pawn).IsWalking = false;
	//MMOActor(Pawn).SetMoveType(EMT_Walk);

	while( Pawn != none)
	{
		//if(Pawn.ReachedPoint( m_vecDestination, none ))
		//{`log("c");
		//	GotoState('Attack');
		//	break;
		//}
		if( PointReachable( m_vecDestination ) )
		{
			//SetDirection( m_vecDestination );
			MoveTo( m_vecDestination );
		}
		else
		{
			//m_NavHelper.BeginPathing( m_vecDestination );
			if( m_NavHelper.GetNextMoveLocation( m_vecLocOnPath, Pawn.GetCollisionRadius( ) ) )
			{
				//SetDirection( m_vecLocOnPath );
				MoveTo( m_vecLocOnPath );
			}
			else
			{
				//`Log( "[MMO_Debug]Can't find path while tracing player." );
				GotoState( 'Watch' );
			//	Goto('End');
				break;
			}
		}
	}

	//
	//if( AttackSlot == -1 )
	//{
	//	GotoState( 'Watch' );
	//}
	//else
	//{
	//	GotoState( 'Position' );
	//}
End:
}

state RunBack
{
	/*
	ignores NotifyTakeHit;

	event BeginState( name PreviousStateName )
	{
		super.BeginState( PreviousStateName );
		//`Log( "Enter RunBack from "@PreviousStateName );
		if( NPC(Pawn).HideWeaponWhenRunning )
		{
			NPC(Pawn).HideNPCWeapon( true );
		}
	}

	event EndState( name NextStateName )
	{
		//`Log( "Leave RunBack To "@NextStateName );
		if( NPC(Pawn).HideWeaponWhenRunning )
		{
			NPC(Pawn).HideNPCWeapon( false );
		}
	}

Begin:
	if(Pawn == none)
	{
		GotoState('Idle');
Goto('End');
	}

	MMOActor(Pawn).MoveType = EMT_Run;
	Pawn.GroundSpeed = MMOActor(Pawn).GetMoveSpeed();
	//MMOActor(Pawn).IsWalking = false;
	//m_vecDestination = NPC( Pawn ).m_vecOrigLoc;
	m_vecDestination = m_vecSpotPlayerSelfLoc;
	m_TargetActor = none;
	//if( m_NavHelper.FindPathToDest( m_vecDestination ) )
	//{
		//while( Pawn != none && !Pawn.ReachedPoint( m_vecDestination, none ) )
		//{
			if( PointReachable( m_vecDestination ) )
			{
				//SetDirection( m_vecDestination );
				MoveTo( m_vecDestination );
				//`Log( "RunBack MoveTo Direct" );
			}
		//	else
		//	{
		//		m_NavHelper.BeginPathing( m_vecDestination );
		//		if( m_NavHelper.GetNextMoveLocation( m_vecLocOnPath, Pawn.GetCollisionRadius( ) ) )
		//		{
		//			//SetDirection( m_vecLocOnPath );
		//			MoveTo( m_vecLocOnPath );
		//			`Log( "RunBack MoveTo Pathing" );
		//		}
		//		else
		//		{
		//			`Log( "Can't find path for NPC" );
		//			break;
		//		}
		//	}
		//}
	//}
	Sleep( 0.5f );
	NPC( Pawn ).m_vecOrigLoc = Pawn.Location;
	GotoState( 'Idle' );
End:

*/
}

function bool IsUsingAbility()
{
	return MMOActor(Pawn).IsUsingAbility();
}

// Hurt state is a hurt state that can interrupt other states, e.g. hit stun, knockback, knock-up, knockdown; normal hurt should not enter this state
// this state is mutually exclusive with other states
state Hurt
{
	event BeginState(Name PreviousStateName)
	{
		super.BeginState(PreviousStateName);
		//ScriptTrace();
		StopLatentExecution();
	}

	event EndState(Name NextStateName)
	{
		super.BeginState(NextStateName);
		//MMOActor(Pawn).BeatEffectType = EHET_None;
	}

	event PoppedState( )
	{
		super.PoppedState();
		//MMOActor(Pawn).BeatEffectType = EHET_None;
	}

	event PushedState( )
	{
		super.PushedState( );
		StopLatentExecution();
	}

	//function TakeEffect(MMODamage Damage)
	//{
	//}

	event Tick( float DeltaTime )
	{
		//local MMOActor Target;
		//Target = MMOActor(Pawn);

		//if(Target.BeatEffectType == EHET_None)
		//{
		//	`warn("error 828:NPCController");
		//	PopState();
		//}
		//else if(Target.BeatEffectType == EHET_BeatAway)
		//{
		//	if(MMOActor(Pawn).Physics != PHYS_Falling)
		//	{
		//		PopState();
		//	}
		//}
		//else if(Target.BeatEffectType == EHET_BeatFallDown)
		//{
		//	Target.BeatFallDownLeftTime -= DeltaTime;
		//	if(Target.BeatFallDownLeftTime <= 0)
		//	{
		//		PopState();
		//	}
		//}
		//else if(Target.BeatEffectType == EHET_HeavyBeat)
		//{
		//	Target.BeatFaintLeftTime -= DeltaTime;
		//	if(Target.BeatFaintLeftTime <= 0)
		//	{
		//		PopState();
		//	}
		//}
	}
}

/** positioning */
state Position
{
	event BeginState( name PreviousStateName )
	{
		super.BeginState( PreviousStateName );
		//`Log( self$"Enter Position State from "@PreviousStateName );
		if( NPC(Pawn).HideWeaponWhenRunning )
		{
			NPC(Pawn).HideNPCWeapon( true );
		}
	}

	event EndState( name NextStateName )
	{
		//`Log( self$"Leave position State from "@NextStateName );
		if( NPC(Pawn).HideWeaponWhenRunning )
		{
			NPC(Pawn).HideNPCWeapon( false );
		}
	}

	simulated event Tick( float deltaTime )
	{
		if( !IsWithinRange( Pawn.Location, m_TargetActor.Location, MMOActor( Pawn ).CurrentAttackRange ) )
		{
			GotoState( 'TracePlayer' );
		}
	}

Begin:
	if( m_TargetActor == none )
	{
		GotoState( 'Idle' );
		Goto('End');
	}

	m_vecDestination = m_TargetActor.Location + AttackOffset;

	if( PointReachable( m_vecDestination ) )
	{
		//SetDirection( m_vecDestination );
		MoveTo( m_vecDestination, m_TargetActor );
		GotoState( 'Attack' );
	}
	else
	{
		//`Log( "[MMO_Debug]Can't do Position" );
		GotoState( 'Watch' );
	}
End:
}

function bool LockAttackPosition( )
{
	//local float collen,dis;
	//if( m_TargetActor != none )
	//{
	//	if( AttackSlot != -1 )
	//		FreeAttackPosition( );
	//	if( m_TargetActor.AttackSlotMgr.TakeProperSlot( Pawn.Location, AttackAngle, AttackSlot ) )
	//	{
	//		collen = Pawn.GetCollisionRadius() + m_TargetActor.GetCollisionRadius();
	//		FRandNum = FRand( );
	//		dis = MMOActor(Pawn).CurrentAttackRange*FRandNum + (1-FRandNum)*collen;

	//		AttackOffset.X = dis * cos( AttackAngle );
	//		AttackOffset.Y = dis * sin( AttackAngle );
	//		return true;
	//	}
	//}
	return false;
}

function FreeAttackPosition( )
{
	//if( m_TargetActor != none && AttackSlot != -1 )
	//{
	//	m_TargetActor.AttackSlotMgr.FreeSlot( AttackSlot );
	//	//AttackOffset.X = 0;
	//	//AttackOffset.Y = 0;
	//	AttackSlot = -1;
	//}
}

state Watch
{
Begin:
	//`Log( "Enter Watch State" );
	StopLatentExecution();
	Pawn.ZeroMovementVariables();
	Focus = m_TargetActor;
	//MMOActor(Pawn).EnableAttackState( true );
	Sleep( 2.f );
	GotoState( 'TracePlayer' );
	//`Log( self$"Leave watch State " );
}

/** in attack state the NPC gives the target player one hit */
state Attack
{
	simulated event Tick( float deltaTime )
	{
		//CheckRange( );
	}

	event EndState( name NextStateName )
	{
		//Pawn.LockDesiredRotation( false );
		`Log( self$"Leave Attack State from "@NextStateName );
		super.EndState( NextStateName );
	}

	event BeginState(name PreviousStateName)
	{
		super.BeginState(PreviousStateName);
		`Log( self$"[MMO_Debug]Enter Attack State from "@PreviousStateName );
	}

	function CheckRange( )
	{
		//|| IsTooCloseToTarget(m_TargetActor)
		// check if the player is out of attack range 
		if( !IsWithinRange( Pawn.Location, m_TargetActor.Location, MMOActor( Pawn ).CurrentAttackRange ))
		{   // target trying to escape, follow up
			//MMOActor( Pawn ).ResetAnimBlender( );
			GotoState( 'TracePlayer' );
		}
	}


Begin:
	if( m_TargetActor == none )
	{
		GotoState( 'Idle' );
		Goto('End');
	}
	CheckRange( );
	//StopLatentExecution( );
	Pawn.Acceleration.X = 0;
	Pawn.Acceleration.Y = 0;
	Pawn.Acceleration.Z = 0;
	Focus = none;
	//MMOActor( Pawn ).UseAbility( NextAbilityID, m_TargetActor, m_TargetActor.Location);
	//`log("Attacking State");
	//Pawn.LockDesiredRotation( true );
	while( MMOActor( Pawn ).IsUsingAbility() )
	{
		//SetDirection( m_TargetActor.Location );
		Sleep( 0.1f );
	}
	GotoState( 'AttackIdle' );
End:
}

state AttackIdle
{

	event BeginState(name PreviousStateName)
	{
		super.BeginState(PreviousStateName);
		//`Log( self$"Enter AttackIdle State from "@PreviousStateName );
	}
	event EndState( Name NextStateName )
	{
		super.EndState( NextStateName );
		//`Log( self$"Leave AttackIdle State from "@NextStateName );
	}
	simulated event Tick( float deltaTime )
	{
		AttackIdleTime += deltaTime;
	//	CheckRange( );
	}

	function CheckRange( )
	{
		// check if the player is out of attack range 
		if( !IsWithinRange( Pawn.Location, m_TargetActor.Location, MMOActor( Pawn ).CurrentAttackRange ) )
		{   // target trying to escape, follow up
			//MMOActor( Pawn ).ResetAnimBlender( );
			GotoState( 'TracePlayer' );
		}
	}

Begin:
	AttackIdleTime = 0.f;
	ChooseNextAbility( );
	Focus = m_TargetActor;
	// play attack_idle animation
	while( AttackIdleTime < NPC( Pawn ).AttackInterval )
	{
		//SetDirection( m_TargetActor.Location );
		Sleep( 0.1f );
	}

	// stagger the attack start time
	Sleep(FRand());

	GotoState( 'Attack' );
}

state Escape
{
	ignores NotifyTakeHit;

	simulated event Tick( float deltaTime )
	{
		//`Log( "Escaping" );
	}
Begin:
	m_vecDestination = m_vecSpotPlayerSelfLoc;
	//NavigationHandle.SetFinalDestination( m_vecDestination /*m_vecEscapeLoc + m_vecEscapeDir * NPC(Pawn).m_fEscapeDistance*/ );
	//if( NavigationHandle.ComputeValidFinalDestination( m_vecDestination ) )
	//{
		m_NavHelper.BeginPathing( m_vecDestination );
		while( !Pawn.ReachedPoint( m_vecDestination, none ) )
		{
			if( m_NavHelper.GetNextMoveLocation( m_vecLocOnPath, Pawn.GetCollisionRadius( ) ) )
			{
				//SetDirection( m_vecLocOnPath );
				MoveTo( m_vecLocOnPath );
			}
			else
			{
				`Log( "Can't find path for NPC" );
				break;
			}
		}
	//}

	m_nEscapeChance -= 1;
	Sleep( 0.5f );
	GotoState( 'TracePlayer' );
}

/** */
state Dead
{
	//function TakeEffect(MMODamage Damage)
	//{
	//	local float force;

	//	if(Damage.HitEffectComponent.IsA('AbilityHitEffectComponent'))
	//	{
	//		force = AbilityHitEffectComponent(Damage.HitEffectComponent).XYMomentum;
	//	}
	//	else
	//	{
	//		force = 1000;
	//	}

	//	// TODO:
	//	force = 1000;
		
	//	// whip the corpse
	//	Pawn.Mesh.AddImpulse(Normal(Pawn.Location - Damage.HitEffectComponent.Creator.Location) * force);
	//}

Begin:
}

/// flee by jumping off a cliff
state JumpDown
{
Begin:
	NPC(Pawn).CouldJumpDown = false;    // prevent a second jump
	m_vecDestination = JumpDownPoint.Location;
	if( m_NavHelper.FindPathToDest( m_vecDestination ) )
	{
		while( !Pawn.ReachedPoint( m_vecDestination, none ) )
		{
			if( PointReachable( m_vecDestination ) )
			{
				//SetDirection( m_vecDestination );
				MoveTo( m_vecDestination );
			}
			else
			{
				m_NavHelper.BeginPathing( m_vecDestination );
				if( m_NavHelper.GetNextMoveLocation( m_vecLocOnPath, Pawn.GetCollisionRadius( ) ) )
				{
					//SetDirection( m_vecLocOnPath );
					MoveTo( m_vecLocOnPath );
				}
				else
				{
					GotoState( 'TracePlayer' );
				}
			}
		}
		// jump down
		//ClientSetRotation( JumpDownPoint.Rotation );
		Pawn.SetDesiredRotation( JumpDownPoint.Rotation, true );
		ClimbTranslate = Pawn.Mesh.GetBoneLocation( 'Bip01' );
		//MMOActor( Pawn ).PlayCustomAnim( NPC(Pawn).m_NameJumpDown, false,,,0 );
		MMOActor( Pawn ).PlayAnimation( NPC(Pawn).m_NameJumpDown );
		while( MMOActor( Pawn ).IsPlayingAnimationByName(NPC(Pawn).m_NameJumpDown))
		{
			Sleep( 0 );
		}
		ClimbTranslate = Pawn.Mesh.GetBoneLocation( 'Bip01' ) - ClimbTranslate;
		//Pawn.Mesh.bPauseAnims = true;
		//ClientSetLocation( Pawn.Location + ClimbTranslate );
		Pawn.SetLocation( Pawn.Location + ClimbTranslate );
		ClimbTranslate.X = 0;
		ClimbTranslate.Y = 0;
		ClimbTranslate.Z = 0;
		//Pawn.LockDesiredRotation( false );
		Pawn.SetPhysics(PHYS_Falling);
		Pawn.Velocity.Z = -400;
		Sleep( 3.f );
		Pawn.LockDesiredRotation( false );
		GotoState( 'WaitBack' );
	}
	else
	{
		GotoState( 'TracePlayer' );
	}
}

/// determine whether it can fall back into the scene
function bool CouldDropBack( )
{
	return true;
}

/// wait to return after jumping off a cliff
state WaitBack
{
	function SetInSky( )
	{
		local Vector v;
		v.X = 0.f;
		v.Y = 0.f;
		v.Z = 1000.f;
		Pawn.Move( v );
	}
Begin:
	while( !CouldDropBack( ) )
	{
		Sleep( 1.f );
	}
	SetInSky( );
	GotoState( 'DropDown' );
}

/// descend from the sky
state DropDown
{
Begin:
	LastFallingVelocity = Pawn.Velocity.Z;
	while( true )
	{
		Sleep( 0.1 );
		if( Pawn.Velocity.Z < LastFallingVelocity )
		{
			`Log( "Land on ground" );
			//MMOActor( Pawn ).PlayCustomAnim( NPC(Pawn).m_NameLand, false );
			MMOActor( Pawn ).PlayAnimation( NPC(Pawn).m_NameLand );
			while( MMOActor( Pawn ).IsPlayingAnimationByName(NPC(Pawn).m_NameLand))
			{
				Sleep( 0 );
			}
			break;
		}
		LastFallingVelocity = Pawn.Velocity.Z;
	}
	m_TargetActor = none;
	GotoState( 'Idle' );
}

function StartClimbToward( Volume target )
{
	if( !IsInState( 'Climbing' ) )
	{
		TargetVolumeForClimbing = target;
		GotoState( 'Climbing' );
	}
}

state Climbing
{
	ignores NotifyTakeHit;

	event BeginState( Name PreviousStateName )
	{
		super.BeginState( PreviousStateName );

		Pawn.Mesh.RootMotionMode = RMM_Translate;
		//MMOActor(Pawn).SetRootBoneAxisOption( RBA_Translate, RBA_Translate, RBA_Translate );

		StopLatentExecution();
		Pawn.Acceleration.X = 0;
		Pawn.Acceleration.Y = 0;
		Pawn.Acceleration.Z = 0;
	}


	event EndState( Name NextStateName )
	{
		super.EndState( NextStateName );

		Pawn.Mesh.RootMotionMode = RMM_Ignore;
		//MMOActor(Pawn).SetRootBoneAxisOption( RBA_Default, RBA_Default, RBA_Default );

		NPC(Pawn).m_vecOrigLoc = Pawn.Location;
	}

	function bool CheckJump( )
	{
		local float dist;
		local float rand;
		dist = TargetVolumeForClimbing.Location.Z - Pawn.Location.Z - ClimbTranslate.Z * 2;
		//if( dist < NPC(Pawn).ClimbUpDistance )
		//{
		//	IsClimbing = false;
		//	ShouldJump = false;
		//	return true;
		//}
		//else if( dist < NPC(Pawn).JumpUpDistance )
		//{
		//	IsClimbing = false;
		//	ShouldJump = true;
		//	return true;
		//}
		if( dist <= 0 )
		{
			rand = FRand();
			if( rand > 0.5 )
			{
				ShouldJump = true;
			}
			else
			{
				ShouldJump = false;
			}
			IsClimbing = false;
			return true;
		}
		return false;
	}

Begin:
	if( TargetVolumeForClimbing == none || Pawn.Mesh.GetAnimLength( NPC( Pawn ).m_NameClimbAnim ) < 0.1f )
		GotoState( 'Idle' );
	//`Log( "[MMO]StartClimb." );
	IsClimbing = true;
	//MMOActor( Pawn ).PlayCustomAnim( NPC( Pawn ).m_NameClimbAnim, true, 0, ,0, ,true );
	ClimbTranslate.X = 0;
	ClimbTranslate.Y = 0;
	ClimbTranslate.Z = 0;
	//Pawn.Mesh.RootMotionMode = RMM_Translate;
	while( IsClimbing )
	{
		if( Pawn == none || TargetVolumeForClimbing == none )
		{
			m_TargetActor = none;
			GotoState( 'Idle' );
		}
		//MMOActor( Pawn ).PlayCustomAnim( NPC( Pawn ).m_NameClimbAnim, false, 0, , 0 );
		MMOActor( Pawn ).PlayAnimation( NPC(Pawn).m_NameClimbAnim, ,, , RBA_Translate, );
		//MMOActor(Pawn).SetRootBoneAxisOption( RBA_Translate, RBA_Translate, RBA_Translate );
		Sleep( 0 );     // prevent runaway loop
		//Pawn.Move( ClimbTranslate );
		ClimbTranslate = Pawn.Mesh.GetBoneLocation( 'Bip01', 1 );
		while( MMOActor( Pawn ).IsPlayingAnimationByName(NPC(Pawn).m_NameClimbAnim))
		{
			Sleep( 0 );
		}
		ClimbTranslate = Pawn.Mesh.GetBoneLocation( 'Bip01', 1 ) - ClimbTranslate;
		ClimbTranslate.X = 0;
		ClimbTranslate.Y = 0;
		//Pawn.SetLocation( Pawn.Location + ClimbTranslate );
		if( CheckJump() )
			break;
		//else
		//	MMOActor( Pawn ).ResetAnimBlender( );
	}

	if( ShouldJump )
	{
		//MMOActor( Pawn ).PlayCustomAnim( NPC( Pawn ).m_NameClimbJumpAnim, false, 0, , 0 );
		MMOActor( Pawn ).PlayAnimation( NPC(Pawn).m_NameClimbJumpAnim, , ,, RBA_Translate, );
		//MMOActor(Pawn).SetRootBoneAxisOption( RBA_Translate, RBA_Translate, RBA_Translate );
		//Pawn.SetLocation( Pawn.Location + ClimbTranslate );
		ClimbTranslate = Pawn.Mesh.GetBoneLocation( 'Bip01' );
		while( MMOActor( Pawn ).IsPlayingAnimationByName(NPC(Pawn).m_NameClimbJumpAnim))
		{
			Sleep( 0 );
		}
		ClimbTranslate = Pawn.Mesh.GetBoneLocation( 'Bip01' ) - ClimbTranslate;
		//Pawn.SetLocation( Pawn.Location + ClimbTranslate );
		//MMOActor( Pawn ).ResetAnimBlender( );
	}
	else
	{
		//MMOActor( Pawn ).PlayCustomAnim( NPC( Pawn ).m_NameClimbOverAnim, false, 0, , 0 );
		MMOActor( Pawn ).PlayAnimation( NPC(Pawn).m_NameClimbOverAnim, ,, , RBA_Translate, );
		//MMOActor(Pawn).SetRootBoneAxisOption( RBA_Translate, RBA_Translate, RBA_Translate );
		//Pawn.SetLocation( Pawn.Location + ClimbTranslate );
		ClimbTranslate = Pawn.Mesh.GetBoneLocation( 'Bip01' );
		while( MMOActor( Pawn ).IsPlayingAnimationByName(NPC(Pawn).m_NameClimbOverAnim))
		{
			Sleep( 0 );
		}
		ClimbTranslate = Pawn.Mesh.GetBoneLocation( 'Bip01' ) - ClimbTranslate;
		//Pawn.SetLocation( Pawn.Location + ClimbTranslate );
		//MMOActor( Pawn ).ResetAnimBlender( );
	}
	//Pawn.Mesh.RootMotionMode = RMM_Ignore;
	//MMOActor(Pawn).SetRootBoneAxisOption( RBA_Default, RBA_Default, RBA_Default );
	GotoState( 'Idle' );
}

state WaitInCage
{
Begin:
	while( true )
	{
		Sleep( 1.f );
	}
}

// decision making for the next ability to use
function int ChooseNextAbility( )
{
	//NextAbilityID = MMOActor(Pawn).GetRandAbility();
	//MMOActor( Pawn ).CurrentAttackRange = NPC( Pawn ).GetAttackRange( NextAbilityID );
	return NextAbilityID;
}

defaultproperties
{
	m_TargetActor = none
	//m_fBeatBackTimeTotal = 0.2
	//m_fBeatBackTimePast = 0
	m_nEscapeChance = 1
	AttackSlot = -1
	AttackOffset = ( X = 0, Y = 0, Z = 0 )
	m_fLogicUpdateTime = 0.25f
	NextAbilityID = 0
}
