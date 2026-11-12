#ifndef _MMONAVIGATIONMANAGER_H_
#define _MMONAVIGATIONMANAGER_H_

#include "MMOSingleton.h"
#include "GraphUtil/SparseGraph.h"
#include "GraphUtil/GraphAlgorithms.h"
#include <list>
#include <map>
#include "MMOGameEvent.h"
#include "MMOUtil.h"

// the show/hide rules of the signposts imitate Douzhanshen:
// use the angle between the vector from the character to the signpost and the signpost's pointing direction (dot product: >=0: angle <= 90 degrees, <0: angle > 90 degrees) to decide whether the character is in front of or behind the signpost; hide when in front, show when behind;
// also, to avoid signposts behind disappearing before the character gets near on U-shaped routes (since they are behind the character), the max distance at which the character can affect a signpost is limited to 1024 (1024 * 1024 = 1048576), an empirical value, about one screen
#define PATH_POINT_PLAYER_DIST_SQUARED_MAX 1048576
// waypoint update interval
#define PATH_POINT_UPDATE_TIME			   0.5f
// waypoint disappear time
#define PATH_EFFECT_DISAPPEAR_TIME         0.5f
// waypoint pulse period
#define PATH_EFFECT_WAVE_TIME              4.0f

enum
{
	INVALID_MAP_ID = -1,
	MAX_PATH_EFFECT_NUM = 20,
};

// navigation algorithm
enum NaviMode
{
	NM_DIJKSTRA = 0,		// DIJKSTRA
	NM_ASTAR,				// ASTAR
	NM_ASTARCOST			// ASTAR BY COST
};

// navigation node
class NaviNode
{  
public:
	NaviNode() : _Index(invalid_node_index), _MapId(INVALID_MAP_ID)
	{}
	NaviNode(INT index) : _Index(index), _MapId(INVALID_MAP_ID)
	{}

	virtual ~NaviNode()
	{}

	INT Index() const { return _Index; }
	void SetIndex(INT index) { _Index = index; }

	INT MapId() const { return _MapId; }
	void SetMapId(INT mapId) { _MapId = mapId; }

protected:
	INT _Index;
	INT _MapId;
};   

// navigation link
class NaviLink
{
public:
	NaviLink(INT from, INT to, double cost) : _Cost(cost), _From(from), _To(to) 
	{}
	NaviLink(INT from, INT to) : _Cost(1.0), _From(from), _To(to) 
	{}
	NaviLink() : _Cost(1.0), _From(INVALID_MAP_ID), _To(INVALID_MAP_ID) 
	{}

	virtual ~NaviLink() {}

	INT From() const { return _From; }
	void SetFrom(INT mapId){ _From = mapId; }

	INT To() const { return _To; }
	void SetTo(INT mapId) { _To = mapId; }

	double Cost() const { return _Cost; }
	void SetCost(double cost) { _Cost = cost; }

protected:
	INT _From;
	INT _To;
	double _Cost;
};

typedef SparseGraph<NaviNode, NaviLink> NaviGraph;
typedef std::map<INT, class NaviNode, M_UE3_STL_ALLOC_MAP(INT, class NaviNode)> NaviNodeMap;	//MapId->Index

typedef std::list<NaviNode, M_UE3_STL_ALLOC(NaviNode)> NaviNodeList;
typedef std::list<NaviLink, M_UE3_STL_ALLOC(NaviLink)> NaviLinkList;

// navigation type
enum NaviType
{
	NT_NULL = 0,
	NT_NAV_POINT,					// navigation point
	NT_NPC,							// navigate to NPC
	NT_POINT,						// arbitrary coordinate point
	NT_DOODAD,						// DOODAD
};

// navigation event
struct MMOGameEvent_Navigation : public MMOGameEvent
{
public:
	enum NaviEventType
	{
		NET_NULL = 0,
		NET_BEGIN,				// begin navigation
		NET_END,				// end navigation
		NET_ENTER_SCENE			// cross map
	};

	MMOGameEvent_Navigation()
	{
		_EventType = NET_NULL;
	}

	virtual EGameEvent GetEventType() const { return GameEvent_Navigation; }

public:
	NaviEventType _EventType;
};

struct NaviInfo
{
	NaviType Type;
	FString PointName;
	INT MapId;
	INT NpcId;
	INT DoodadId;
	FVector Point;
	UBOOL NeedMove;

	NaviInfo() : Type(NT_NULL), 
		PointName(TEXT("")), 
		MapId(INVALID_MAP_ID), 
		NpcId(0), 
		Point(0.0, 0.0, 0.0), 
		DoodadId(0), 
		NeedMove(FALSE)
	{}
};

struct PathPointInfo
{
	FVector Pos;
	UBOOL NeedDisappear;
	FLOAT DisappearTime;

	PathPointInfo() : Pos(0.0, 0.0, 0.0), NeedDisappear(FALSE), DisappearTime(0.0)
	{}
};

class MMONavigationManager : public SingletonMeyers<MMONavigationManager>, public INativeGameEventHandler
{
public:
	MMONavigationManager();
	virtual ~MMONavigationManager();

	void Init();
	void Shutdown();
	void Clear();
	void Update(FLOAT deltaTime);
	virtual void OnNativeGameEvent(struct MMOGameEvent* evt);

	void StopNavigate(UBOOL stopMove = TRUE);

	// find and move toward the corresponding teleport
	UBOOL FindAndMoveToTeleport(INT from, INT to, INT npcId = 0, UBOOL needMove = FALSE);

	void EnterScene();

	UBOOL IsNavigating(){ return _IsNavigating; }

	void InitPathEffect();
	void ShutdownPathEffect();
	void ClearPathEffect();

	

protected:
	UBOOL _Navigate(INT mapId, const FString& pointName, UBOOL needMove);
	UBOOL _NavigateDoodad(INT mapId, const FString& pointName, INT doodadId, UBOOL needMove);
	UBOOL _Navigate(INT mapId, INT npcId, UBOOL needMove);
	UBOOL _Navigate(INT mapId, FVector point, UBOOL needMove);

protected:
	void _AddNaviNode(INT mapId);
	void _AddNaviLink(INT from, INT to, double cost);
	INT _GetNaviNodeMapId(INT index);

	void _SearchByDijkstra(INT from, INT to, std::list<INT>& list);
	void _SearchByAStar(INT from, INT to, std::list<INT>& list);
	void _SearchByAStarCost(INT from, INT to, std::list<INT>& list);

	UBOOL _GenNaviNodeList(INT from, INT to);
	INT _GetNextMapId(INT curIndex);

	void _StopNavigate();
	void _BeginNavigate();

	// waypoint effect
	void _FiltPathPoint(FVector& pos);
	void _GeneratePathPoint();
	void _UpdatePathPoint(FLOAT deltaTime);
	void _DrawPathEffect(FLOAT deltaTime);


	

	// CQ version: client pathfinding is still a navmesh, and some walkable cells are unreachable on the navmesh; for unreachable coordinate points, apply a suitable offset here
	UBOOL _GeneratePath(const FVector& srcPoint, const FVector& dstPoint, UBOOL withOffset = FALSE);
public:
	UBOOL Navigate(INT mapId, INT npcId, UBOOL needMove);										// for NPC pathfinding
	UBOOL Navigate(INT mapId, const FString& pointName, UBOOL needMove);						// for navigation point pathfinding
	UBOOL NavigateDoodad(INT mapId, const FString& pointName, INT doodadId, UBOOL needMove);	// for doodad pathfinding
	UBOOL Navigate(INT mapId, FVector point, UBOOL needMove);									// for arbitrary coordinate point pathfinding

protected:
	NaviMode _NaviMode;
	NaviGraph* _NaviGraph;
	NaviNodeMap _NaviNodeMap;

	NaviNodeList _NaviNodeList;
	NaviLinkList _NaviLinkList;

	NaviInfo _NaviInfo;
	UBOOL _IsNavigating;

	TArray<PathPointInfo> _PathPointList;
	TArray<AMMONavitationEmitter*> _PathEffectList;
	FLOAT _PathPointProgress;
	FLOAT _PathEffectProgress;
};

#endif