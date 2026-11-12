/**
 * file: MMONavigationHelper.uc
 * this class wraps the useful functions of a NavigationHandle and offer
 * interfaces to the owner( controller ) to let Player and AI use the same logic.
 *
 */

class MMONavigationHelper extends Object
	native
	config(MMOPlayerController);

var Controller _Owner;
var NavigationHandle _NavigationHandle;

var Vector _Destination;
var Vector _DestinationTemp;

var ParticleSystem _PathPointEffect;    // Waypoint effect

var config bool ShowDebugPath;
var config int PathEffectInterval;
var config int PathEffectHeight;
var config int PathEffectFadeDist;

native function Setup(Controller owner);
native function bool FindPathToDest(Vector vecDestLocation);
native function BeginPathing(Vector vecDestLocation);
native function bool GetNextMoveLocation(out Vector aim, float ArrivalDistance);

event bool Pram_NavMeshPath_Toward(NavigationHandle NavHandle, Vector Point)
{
	class'NavMeshPath_Toward'.static.TowardPoint( NavHandle, Point );
	NavHandle.PopulatePathfindingParamCache();
	return class 'NavMeshGoal_At'.static.AtLocation(NavHandle, Point);  // zhaojun@snda, zhaojun@snda, fix pathfinding bug
	
	/*return class'NavMeshGoal_At'.static.AtActor(NavHandle,
		MMOPlayerController(m_Owner).TargetLocationMarker,
		MMOPlayerController(m_Owner).Pawn.GetCollisionRadius());*/
}

//function Setup( Controller owner )
//{
//	m_Owner = owner;
//	m_NavigationHandle = m_Owner.NavigationHandle;
//	m_NavigationHandle.bUltraVerbosePathDebugging = ShowDebugPath;
//	m_NavigationHandle.bVisualPathDebugging = ShowDebugPath;
//}

//function bool FindPathToDest( Vector vecDestLocation )
//{
//	local bool succ;
//	if( m_NavigationHandle != none )
//	{
//		//`log("____ Move FindPathToDest Location" @vecDestLocation);

//		m_NavigationHandle.PathConstraintList = none;
//		m_NavigationHandle.PathGoalList = none;
//		m_NavigationHandle.ClearConstraints();
//		class'NavMeshPath_Toward'.static.TowardPoint( m_NavigationHandle, vecDestLocation );
//		m_NavigationHandle.PopulatePathfindingParamCache();
//		//class'NavMeshGoal_At'.static.AtLocation( m_NavigationHandle, vecDestLocation );
//		class'NavMeshGoal_At'.static.AtActor(m_NavigationHandle,
//			MMOPlayerController(m_Owner).TargetLocationMarker,
//			MMOPlayerController(m_Owner).Pawn.GetCollisionRadius());

//		succ = m_NavigationHandle.FindPath( );
//		if(MMOPlayerController(m_Owner).DebugShowNavPath)
//		{
//			class'Actor'.static.FlushPersistentDebugLines();
//			m_NavigationHandle.DrawPathCache(, true);
//		}

//		return succ;
//	}
//	return false;
//}

//function BeginPathing( Vector vecDestLocation )
//{
//	m_NavigationHandle.SetFinalDestination( vecDestLocation );
//	m_vecDestination = vecDestLocation;
//}

//function bool GetNextMoveLocation( out Vector aim, float ArrivalDistance )
//{
//	//if( m_NavigationHandle.PointReachable( m_vecDestination ) )
//	//{
//	//	aim = m_vecDestination;
//	//	return true;
//	//}
//	//else
//	//{
//		if( m_NavigationHandle.GetNextMoveLocation( aim, ArrivalDistance ) )
//		{
//		//	m_NavigationHandle.SuggestMovePreparation( aim, m_Owner );
//			return true;
//		}
//	//}
//	return false;
//}

DefaultProperties
{
	_Owner=none;
	_NavigationHandle=none;
	_PathPointEffect=ParticleSystem'FX_Arrow.01Par.P_Arrow_001'
}
