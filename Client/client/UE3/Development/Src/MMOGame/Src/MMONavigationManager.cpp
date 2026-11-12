#include "stdafx.h"
#include "MMOGameClasses.h"
#include "MMONavigationManager.h"
#include "MMOFacade.h"
#include "MMOChatManager.h"
//#include "UGlobalMapInfo.h" // .h automatically included for the uc class, must not be included manually elsewhere
#include "MMOMapDataManager.h"
#include "FSMIO.h"
#include "DoodadManager.h"

using namespace ExternalMessage;

IMPLEMENT_CLASS(AMMONavitationEmitter)

extern FVector CurReachablePoint;

MMONavigationManager::MMONavigationManager()
{
	MMOUI::RegisterEventHandler(this, GameEvent_Navigation);
	MMOUI::RegisterEventHandler(this, GameEvent_UpdateMovePath);

	_NaviMode = NM_DIJKSTRA;
	_NaviGraph = NULL;
	_IsNavigating = FALSE;

	_PathPointProgress = 0.0f;
	_PathEffectProgress = 0.0f;
}

MMONavigationManager::~MMONavigationManager()
{
	MMOUI::UnRegisterEventHandler(this, GameEvent_Navigation);
	MMOUI::UnRegisterEventHandler(this, GameEvent_UpdateMovePath);
}

void MMONavigationManager::Init()
{	
	SCOPE_LOG_TIME(___INIT__, TEXT("MMONavigationManager::Init"), 0);

	Clear();
	_NaviGraph = new NaviGraph(true);
	_NaviMode = NM_DIJKSTRA;

	const TArrayNoInit<struct FTeleportLink>& teleportLinks = UGlobalMapInfo::GetSingleton()->TeleportLinks;
	for (INT i = 0; i < teleportLinks.Num(); ++ i)
	{
		FTeleportLink teleportLink = teleportLinks(i);
		INT fromMapId = UGlobalMapInfo::GetSingleton()->FindMapID(teleportLink.FromMapName);
		INT toMapId = UGlobalMapInfo::GetSingleton()->FindMapID(teleportLink.ToMapName);

		if (fromMapId == toMapId)
		{
			continue;
		}

		// start point
		_AddNaviNode(fromMapId);

		// end point
		_AddNaviNode(toMapId);

		// Link
		_AddNaviLink(fromMapId, toMapId, 1.0);
	}
}

void MMONavigationManager::Shutdown()
{
	ShutdownPathEffect();
	Clear();
	if (_NaviGraph)
	{
		delete _NaviGraph;
		_NaviGraph = NULL;
	}
}

void MMONavigationManager::Clear()
{
	_NaviNodeMap.clear();
	_NaviNodeList.clear();
	_NaviLinkList.clear();
	_IsNavigating = FALSE;

	_PathPointProgress = 0.0f;
	_PathEffectProgress = 0.0f;
}

void MMONavigationManager::Update(FLOAT deltaTime)
{
	CTRL_TICK("MMONavigationManager");

	if (_IsNavigating)
	{
		_UpdatePathPoint(deltaTime);
		_DrawPathEffect(deltaTime);
	}
}

void MMONavigationManager::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch(evt->GetEventType())
	{
	case GameEvent_Navigation:
		{
			MMOGameEvent_Navigation* naviEvent = (MMOGameEvent_Navigation*)evt;
			check(naviEvent);
		}
		break;
	case GameEvent_UpdateMovePath:
		{
		}
		break;
	}
}

UBOOL MMONavigationManager::Navigate(INT mapId, const FString& pointName, UBOOL needMove)
{
	StopNavigate(FALSE);

	_NaviInfo.Type = NT_NAV_POINT;
	_NaviInfo.MapId = mapId;
	_NaviInfo.PointName = pointName;
	_NaviInfo.NeedMove = needMove;

	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		if (!_Navigate(mapId, pointName, needMove))
		{
			FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
			return FALSE;
		}
	}
	else
	{
		if (!_GenNaviNodeList(curMapId, mapId))
		{
			FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			return FALSE;
		}
		else
		{
			if (!_Navigate(mapId, pointName, needMove))
			{
				FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
				return FALSE;
			}
		}
	}

	return TRUE;
}

UBOOL MMONavigationManager::Navigate(INT mapId, INT npcId, UBOOL needMove)
{
	StopNavigate(FALSE);

	_NaviInfo.Type = NT_NPC;
	_NaviInfo.MapId = mapId;
	_NaviInfo.NpcId = npcId;
	_NaviInfo.NeedMove = needMove;

	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		if (!_Navigate(mapId, npcId, needMove))
		{
			FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			return FALSE;
		}
	}
	else
	{
		if (!_GenNaviNodeList(curMapId, mapId))
		{
			FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			return FALSE;
		}
		else
		{
			if (!_Navigate(mapId, npcId, needMove))
			{
				FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
				return FALSE;
			}
		}
	}

	return TRUE;
}

UBOOL MMONavigationManager::Navigate(INT mapId, FVector point, UBOOL needMove)
{
	StopNavigate(FALSE);

	_NaviInfo.Type = NT_POINT;
	_NaviInfo.MapId = mapId;
	_NaviInfo.Point = point;
	_NaviInfo.NeedMove = needMove;

	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		if (!_Navigate(mapId, point, needMove))
		{
			//FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
			//MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
			return FALSE;
		}
	}
	else
	{
		if (!_GenNaviNodeList(curMapId, mapId))
		{
			//FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
			//MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
			return FALSE;
		}
		else
		{
			if (!_Navigate(mapId, point, needMove))
			{
				//FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
				//MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavPointNotFound"));
				return FALSE;
			}
		}
	}

	return TRUE;
}

UBOOL MMONavigationManager::NavigateDoodad(INT mapId, const FString& pointName, INT doodadId, UBOOL needMove)
{
	StopNavigate(FALSE);

	_NaviInfo.Type = NT_DOODAD;
	_NaviInfo.MapId = mapId;
	_NaviInfo.PointName = pointName;
	_NaviInfo.DoodadId = doodadId;
	_NaviInfo.NeedMove = needMove;

	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		if (!_NavigateDoodad(mapId, pointName, doodadId, needMove))
		{
			FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			return FALSE;
		}
	}
	else
	{
		if (!_GenNaviNodeList(curMapId, mapId))
		{
			FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
			return FALSE;
		}
		else
		{
			if (!_NavigateDoodad(mapId, pointName, doodadId, needMove))
			{
				FString error = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
				MMOUI::GetUIManager()->delegateAddScreenMessage(error,MsgInfoType_Table,TEXT("MMOErrorInfo.Quest.QuestErrNavNpcNotFound"));
				return FALSE;
			}
		}
	}

	return TRUE;
}

void MMONavigationManager::StopNavigate(UBOOL stopMove)
{
	_StopNavigate();

	if (stopMove)
	{
		MMOFacade::getSingleton().GetMainController()->eventClearAllCmd();
		AMMOPlayerController* playerControl = MMOFacade::getSingleton().GetMainController();
		if (playerControl)
		{
			playerControl->StopMove(TRUE);
		}
	}
}

void MMONavigationManager::EnterScene()
{
	// if crossing maps
	if (_NaviNodeList.size() > 0)
	{
		// continue
		switch (_NaviInfo.Type)
		{
		case NT_NAV_POINT:
			_Navigate(_NaviInfo.MapId, _NaviInfo.PointName, _NaviInfo.NeedMove);
			break;
		case NT_NPC:
			_Navigate(_NaviInfo.MapId, _NaviInfo.NpcId, _NaviInfo.NeedMove);
			break;
		case NT_POINT:
			_Navigate(_NaviInfo.MapId, _NaviInfo.Point, _NaviInfo.NeedMove);
			break;
		case NT_DOODAD:
			_NavigateDoodad(_NaviInfo.MapId, _NaviInfo.PointName, _NaviInfo.DoodadId, _NaviInfo.NeedMove);
			break;
		}
	}
}

void MMONavigationManager::_AddNaviNode(INT mapId)
{
	if (!_NaviGraph)
	{
		return;
	}

	NaviNodeMap::iterator iter = _NaviNodeMap.find(mapId);
	if (iter == _NaviNodeMap.end())
	{
		NaviNode node;
		node.SetMapId(mapId);
		node.SetIndex(_NaviNodeMap.size());
		_NaviGraph->AddNode(node);
		_NaviNodeMap.insert(std::make_pair(mapId, node));
	}
}

void MMONavigationManager::_AddNaviLink(INT from, INT to, double cost)
{
	if (!_NaviGraph)
	{
		return;
	}

	NaviNodeMap::iterator iterFrom = _NaviNodeMap.find(from);
	NaviNodeMap::iterator iterTo = _NaviNodeMap.find(to);
	if (iterFrom != _NaviNodeMap.end() && iterTo != _NaviNodeMap.end())
	{
		NaviLink link(iterFrom->second.Index(), iterTo->second.Index(), cost);
		_NaviGraph->AddEdge(link);
		_NaviLinkList.push_back(link);
	}
}

INT MMONavigationManager::_GetNaviNodeMapId(INT index)
{
	NaviNodeMap::iterator iter = _NaviNodeMap.begin();
	for (; iter != _NaviNodeMap.end(); ++ iter)
	{
		if (iter->second.Index() == index)
		{
			return iter->first;
		}
	}

	return INVALID_MAP_ID;
}

void MMONavigationManager::_SearchByDijkstra(INT from, INT to, std::list<INT>& list)
{
	list.clear();
	NaviNodeMap::iterator iterFrom = _NaviNodeMap.find(from);
	NaviNodeMap::iterator iterTo = _NaviNodeMap.find(to);
	if (iterFrom != _NaviNodeMap.end() && iterTo != _NaviNodeMap.end())
	{
		Graph_SearchDijkstra<NaviGraph> search(*_NaviGraph, iterFrom->second.Index(), iterTo->second.Index());
		std::list<INT> indexList = search.GetPathToTarget();
		std::list<INT>::iterator iterIndex = indexList.begin();
		for (; iterIndex != indexList.end(); ++ iterIndex)
		{
			INT mapId = _GetNaviNodeMapId(*iterIndex);
			if (mapId != INVALID_MAP_ID)
			{
				list.push_back(mapId);
			}
		}
	}
}

void MMONavigationManager::_SearchByAStar(INT from, INT to, std::list<INT>& list)
{
	list.clear();
}

void MMONavigationManager::_SearchByAStarCost(INT from, INT to, std::list<INT>& list)
{
	list.clear();
}

UBOOL MMONavigationManager::_GenNaviNodeList(INT from, INT to)
{
	if (from == 0 || to == 0)
	{
		return FALSE;
	}

	std::list<INT> mapList;
	mapList.clear();

	switch (_NaviMode)
	{
	case NM_DIJKSTRA:
		_SearchByDijkstra(from, to, mapList);
		break;
	case NM_ASTAR:
		_SearchByAStar(from, to, mapList);
		break;
	case NM_ASTARCOST:
		_SearchByAStarCost(from, to, mapList);
		break;
	}

	_NaviNodeList.clear();
	std::list<INT>::iterator iterMap = mapList.begin();
	for (; iterMap != mapList.end(); ++ iterMap)
	{
		NaviNodeMap::iterator iterNode = _NaviNodeMap.find(*iterMap);
		if (iterNode != _NaviNodeMap.end())
		{
			_NaviNodeList.push_back(iterNode->second);
		}
	}

	return _NaviNodeList.size() > 0;
}

INT MMONavigationManager::_GetNextMapId(INT mapId)
{
	NaviNodeList::iterator iter = _NaviNodeList.begin();
	for (; iter != _NaviNodeList.end(); ++ iter)
	{
		if (iter->MapId() == mapId)
		{
			iter ++;
			break;
		}
	}

	if (iter != _NaviNodeList.end())
	{
		return iter->MapId();
	}

	return INVALID_MAP_ID;
}


UBOOL MMONavigationManager::FindAndMoveToTeleport(INT from, INT to, INT npcId, UBOOL needMove)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return FALSE;
	}

	TArray<FTeleportLink*> linkList;
	int linkCount = UGlobalMapInfo::GetSingleton()->GetMapLinks(from, to, linkList);
	if (linkCount == 0)
	{
		return FALSE;
	}

	for (INT i = 0; i < linkList.Num(); ++ i)
	{
		FTeleportLink* link = linkList(i);
		if (link && 
			_GeneratePath(mainPlayer->Location, link->From.Location, TRUE) && 
			(npcId == 0 || (npcId != 0 && npcId == link->To.NpcId)))
		{
			if (needMove)
			{
				CMessageHandler::GetInstance().Send_Clear_Spell_Cache();
				MMOFacade::getSingleton().GetMainController()->eventClearAllCmd();
				MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToActor(NULL, link->From.Location, link->From.Radius * 0.9f);
				for (FActorIterator iter; iter; ++ iter)
				{
					ATeleportOrig* teleportActor = Cast<ATeleportOrig>(*iter);
					if (teleportActor && teleportActor->GetName() == link->From.Name)
					{
						MMOFacade::getSingleton().GetMainController()->eventAddCmdInteract(TScriptInterface<IIInteractableActor>(teleportActor), 0);
						break;
					}
				}
			}

			_BeginNavigate();
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMONavigationManager::_GeneratePath(const FVector& srcPoint, const FVector& dstPoint, UBOOL withOffset)
{
	FVector naviOffset[17] = 
	{
		FVector(0,0,0),
		FVector(64,0,0), FVector(0,64,0), FVector(-64,0,0), FVector(0,-64,0),
		FVector(128,0,0), FVector(0,128,0), FVector(-128,0,0), FVector(0,-128,0),
		FVector(256,0,0), FVector(0,256,0), FVector(-256,0,0), FVector(0,-256,0),
		FVector(512,0,0), FVector(0,512,0), FVector(-512,0,0), FVector(0,-512,0)
	};

	if (!withOffset)
	{
		return gServerGridWorld.GeneratePath(srcPoint, dstPoint) > 0;
	}

	for (INT i = 0; i < 17; i ++)
	{
		if (gServerGridWorld.GeneratePath(srcPoint, dstPoint + naviOffset[i]))
		{
			return TRUE;
		}
	}

	return FALSE;
}

UBOOL MMONavigationManager::_Navigate(INT mapId, const FString& pointName, UBOOL needMove)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return FALSE;
	}

	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		_NaviNodeList.clear();
		AMMONavPoint* naviPoint = 0;
		for (TObjectIterator<AMMONavPoint> iter; iter; ++ iter)
		{
			AMMONavPoint* point = *iter;
			if(point && point->ConfigName == pointName)
			{
				// first check whether the current map is directly reachable; if not, consider in-map teleport points on the current map
				if (_GeneratePath(mainPlayer->Location, point->Location, TRUE))
				{
					if (needMove)
					{
						CMessageHandler::GetInstance().Send_Clear_Spell_Cache();
						MMOFacade::getSingleton().GetMainController()->eventClearAllCmd();

						switch (point->NavPointType)
						{
						case ENPT_Fight:
							{
								if (point->TargetNpc)
								{
									MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToActor(NULL, point->Location, 10);
									MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToActor(NULL, point->Location, 10, point->TargetNpc->TemplateID, false);
									//UMMOAbility* defaultAbility = MMOTargetManager::getSingleton().GetDefaultAbility();
									UMMOAbility* defaultAbility = FSMIO::getSingleton().Ability.GetDefaultAbility();
									//MMOFacade::getSingleton().GetMainController()->eventAddCmdUseAbility(defaultAbility, NULL, FVector(0.0, 0.0, 0.0), FALSE, point->TargetNpc->TemplateID);
									MMOFacade::getSingleton().GetMainController()->eventAddCmdAutoAttack(point->TargetNpc->TemplateID, NULL);
								}
							}
							break;

						default:
							MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToPoint(point->Location, TRUE);
							break;
						}
					}

					_BeginNavigate();
					return TRUE;
				}
				else
				{
					// move to the teleport point leading to other positions on this map
					// because it must cross maps once more, _NaviNodeList must have data
					_NaviNodeList.push_back(curMapId);
					return FindAndMoveToTeleport(curMapId, curMapId, 0, needMove);
				}
			}
		}

		// if the map's waypoint Actor is not found, look in the global data (defect #10295)
		UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
		const TArray<FNaviPointInfo>* mapNavPoints = NULL;
		if( globalMapInfo != NULL ){
			mapNavPoints = globalMapInfo->GetAllNavPointsInMap( mapId );
		}
		if( mapNavPoints == NULL ){
			return FALSE;
		}
		const TArray<FNaviPointInfo> &mapNavPointsList = *mapNavPoints;

		for (INT i = 0; i < mapNavPointsList.Num(); ++ i)
		{
			const FNaviPointInfo* navInfo = &mapNavPointsList(i);
			if (navInfo && navInfo->Name == pointName)
			{
				// first check whether the current map is directly reachable; if not, consider in-map teleport points on the current map
				if (_GeneratePath(mainPlayer->Location, navInfo->Position, TRUE))
				{
					if (needMove)
					{
						MMOFacade::getSingleton().GetMainController()->eventClearAllCmd();
						CMessageHandler::GetInstance().Send_Clear_Spell_Cache();
						MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToPoint(navInfo->Position, TRUE);
					}

					_BeginNavigate();
					return TRUE;
				}
				else
				{
					// move to the teleport point leading to other positions on this map
					// because it must cross maps once more, _NaviNodeList must have data
					_NaviNodeList.push_back(curMapId);
					return FindAndMoveToTeleport(curMapId, curMapId, 0, needMove);
				}
			}
		}
	}
	else
	{
		INT nextMapId = _GetNextMapId(curMapId);
		if (nextMapId != INVALID_MAP_ID)
		{
			if (FindAndMoveToTeleport(curMapId, nextMapId, 0, needMove))
			{
				return TRUE;
			}
			else
			{
				// if the adjacent map's teleport point is unreachable, consider in-map teleport points on the current map
				// move to the teleport point leading to other positions on this map
				// because it must cross maps once more, _NaviNodeList must have data
				_NaviNodeList.push_back(curMapId);
				return FindAndMoveToTeleport(curMapId, curMapId, 0, needMove);
			}
		}
	}

	return FALSE;
}

UBOOL MMONavigationManager::_Navigate(INT mapId, INT npcId, UBOOL needMove)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return FALSE;
	}

	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		_NaviNodeList.clear();

		// found the npc
		const FNPCInstanceInfo* npcInfo = NULL;
		const TArray<FNPCInstanceInfo*>& npcInMapInfoList = *(MMOMapDataManager::getSingleton().GetNpcInstanceInMap(mapId));
		for(INT i = 0; i < npcInMapInfoList.Num(); ++ i)
		{
			if (npcInMapInfoList(i) && npcInMapInfoList(i)->TemplateID == npcId)
			{
				npcInfo = npcInMapInfoList(i);
				break;
			}
		}

		if (npcInfo)
		{
			// first check whether the current map is directly reachable; if not, consider in-map teleport points on the current map
			if (_GeneratePath(mainPlayer->Location, npcInfo->Position, TRUE))
			{
				if (needMove)
				{
					CMessageHandler::GetInstance().Send_Clear_Spell_Cache();
					if (MMOFacade::getSingleton().GetMainPlayer())
					{
						FVector npcPos = npcInfo->Position;
						FVector playerPos = MMOFacade::getSingleton().GetMainPlayer()->Location;
						if (FVector(npcPos - playerPos).Size() > MMOFacade::getSingleton().GetMainPlayer()->GetMaxInteractRadius())
						{
							MMOFacade::getSingleton().GetMainController()->eventClearAllCmd();
							MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToActor(NULL, npcInfo->Position, MMOFacade::getSingleton().GetMainPlayer()->GetMaxInteractRadius());
						}
					}

					MMOFacade::getSingleton().GetMainController()->eventAddCmdInteract(TScriptInterface<IIInteractableActor>(), MMOFacade::getSingleton().GetMainPlayer()->GetMaxInteractRadius(), npcInfo->TemplateID);
				}
				
				_BeginNavigate();
				return TRUE;
			}
			else
			{
				// move to the teleport point leading to other positions on this map
				// because it must cross maps once more, _NaviNodeList must have data
				_NaviNodeList.push_back(curMapId);
				return FindAndMoveToTeleport(curMapId, curMapId, npcInfo->TemplateID, needMove);
			}
		}
		else
		{
			const UNPCTemplate* npcTemp = MMOFacade::getSingleton().FindNPCTemplate(npcId);
			if (npcTemp && (npcTemp->ActorType == EACT_Boss || npcTemp->ActorType == EACT_Monster))
			{
				const TArray<FNPCGroupInfo>* pNpcGroups = MMOMapDataManager::getSingleton().GetNpcGroupsInMap( curMapId );
				if (pNpcGroups)
				{
					const TArray<FNPCGroupInfo> &npcGroups = *pNpcGroups;
					for(INT i = 0; i < npcGroups.Num(); ++ i)
					{
						const FNPCGroupInfo &groupNpcInfo = npcGroups(i);
						if (groupNpcInfo.MapID == curMapId && groupNpcInfo.NpcTemplateID == npcId)
						{
							// first check whether the current map is directly reachable; if not, consider in-map teleport points on the current map
							if (_GeneratePath(mainPlayer->Location, groupNpcInfo.Position, TRUE))
							{
								if (needMove)
								{
									if (MMOFacade::getSingleton().GetMainPlayer())
									{
										FVector npcPos = groupNpcInfo.Position;
										FVector playerPos = MMOFacade::getSingleton().GetMainPlayer()->Location;
										if (FVector(npcPos - playerPos).Size() > MMOFacade::getSingleton().GetMainPlayer()->GetMaxInteractRadius())
										{
											MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToActor(NULL, groupNpcInfo.Position, MMOFacade::getSingleton().GetMainPlayer()->GetMaxInteractRadius());
											MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToActor(NULL, groupNpcInfo.Position, MMOFacade::getSingleton().GetMainPlayer()->GetMaxInteractRadius(), groupNpcInfo.NpcTemplateID, false);
										}
									}

									MMOFacade::getSingleton().GetMainController()->eventAddCmdAutoAttack(groupNpcInfo.NpcTemplateID, NULL);
								}

								_BeginNavigate();
								return TRUE;
							}
							else
							{
								// move to the teleport point leading to other positions on this map
								// because it must cross maps once more, _NaviNodeList must have data
								_NaviNodeList.push_back(curMapId);
								return FindAndMoveToTeleport(curMapId, curMapId, 0, needMove);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		INT nextMapId = _GetNextMapId(curMapId);
		if (nextMapId != INVALID_MAP_ID)
		{
			// move to the teleport point leading to the next map
			if (FindAndMoveToTeleport(curMapId, nextMapId, 0, needMove))
			{
				return TRUE;
			}
			else
			{
				// if the adjacent map's teleport point is unreachable, consider in-map teleport points on the current map
				// move to the teleport point leading to other positions on this map
				// because it must cross maps once more, _NaviNodeList must have data
				_NaviNodeList.push_back(curMapId);
				return FindAndMoveToTeleport(curMapId, curMapId, 0, needMove);
			}
		}
	}

	return FALSE;
}

UBOOL MMONavigationManager::_Navigate(INT mapId, FVector point, UBOOL needMove)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return FALSE;
	}

	// first check whether the grid cell at the coordinate has pathfinding data
	ZONE_DATA* zoneData = NULL;
	GRID_DATA* gridData = NULL;
	gServerGridWorld.GetZoneAndGrid(point, zoneData, gridData);
	if (!gridData)
	{
		return FALSE;
	}

	if (gridData->bOutOfPoly)
	{
		return FALSE;
	}

	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		_NaviNodeList.clear();

		// first check whether the current map is directly reachable; if not, consider in-map teleport points on the current map
		if (_GeneratePath(mainPlayer->Location, point, TRUE))
		{
			if (needMove)
			{
				CMessageHandler::GetInstance().Send_Clear_Spell_Cache();
				MMOFacade::getSingleton().GetMainController()->eventClearAllCmd();
				MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToPoint(point, TRUE);
			}

			_BeginNavigate();
			return TRUE;
		}
	}
	else
	{
		INT nextMapId = _GetNextMapId(curMapId);
		if (nextMapId != INVALID_MAP_ID)
		{
			// move to the teleport point leading to the next map
			if (FindAndMoveToTeleport(curMapId, nextMapId, 0, needMove))
			{
				return TRUE;
			}
			else
			{
				// if the adjacent map's teleport point is unreachable, consider in-map teleport points on the current map
				// move to the teleport point leading to other positions on this map
				// because it must cross maps once more, _NaviNodeList must have data
				_NaviNodeList.push_back(curMapId);
				return FindAndMoveToTeleport(curMapId, curMapId, 0, needMove);
			}
		}
	}

	return FALSE;
}

UBOOL MMONavigationManager::_NavigateDoodad(INT mapId, const FString& pointName, INT doodadId, UBOOL needMove)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return FALSE;
	}

	INT curMapId = MMOMapDataManager::getSingleton().GetCurMap().ServerID;
	if (curMapId == mapId)
	{
		_NaviNodeList.clear();

		UGlobalMapInfo* globalMapInfo = UGlobalMapInfo::GetSingleton();
		const TArray<FNaviPointInfo>* mapNavPoints = NULL;
		if( globalMapInfo != NULL ){
			mapNavPoints = globalMapInfo->GetAllNavPointsInMap( mapId );
		}
		if( mapNavPoints == NULL ){
			return FALSE;
		}
		const TArray<FNaviPointInfo> &mapNavPointsList = *mapNavPoints;

		for (INT i = 0; i < mapNavPointsList.Num(); ++ i)
		{
			const FNaviPointInfo* navInfo = &mapNavPointsList(i);
			if (navInfo && navInfo->Name == pointName)
			{
				// first check whether the current map is directly reachable; if not, consider in-map teleport points on the current map
				if (_GeneratePath(mainPlayer->Location, navInfo->Position, TRUE))
				{
					if (needMove)
					{
						CMessageHandler::GetInstance().Send_Clear_Spell_Cache();
						MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToPoint(navInfo->Position, TRUE);
						MMOFacade::getSingleton().GetMainController()->eventAddCmdMoveToActor(NULL, navInfo->Position, 10, doodadId, FALSE, FALSE, TRUE);
						MMOFacade::getSingleton().GetMainController()->eventAddCmdInteract(TScriptInterface<IIInteractableActor>(), MMOFacade::getSingleton().GetMainPlayer()->GetMaxInteractRadius(), doodadId, TRUE);
					}

					_BeginNavigate();
					return TRUE;
				}
				else
				{
					// move to the teleport point leading to other positions on this map
					// because it must cross maps once more, _NaviNodeList must have data
					_NaviNodeList.push_back(curMapId);
					return FindAndMoveToTeleport(curMapId, curMapId, 0, needMove);
				}
			}
		}
	}
	else
	{
		INT nextMapId = _GetNextMapId(curMapId);
		if (nextMapId != INVALID_MAP_ID)
		{
			// move to the teleport point leading to the next map
			if (FindAndMoveToTeleport(curMapId, nextMapId, 0, needMove))
			{
				return TRUE;
			}
			else
			{
				// if the adjacent map's teleport point is unreachable, consider in-map teleport points on the current map
				// move to the teleport point leading to other positions on this map
				// because it must cross maps once more, _NaviNodeList must have data
				_NaviNodeList.push_back(curMapId);
				return FindAndMoveToTeleport(curMapId, curMapId, 0, needMove);
			}
		}
	}

	return FALSE;
}

void MMONavigationManager::_BeginNavigate()
{
	_IsNavigating = TRUE;
	_GeneratePathPoint();

	FSMIO::getSingleton().Ctrl.StartNavigation();
	
	MMOGameEvent_Any evt(GameEvent_StartNavigate);
	MMOUI::PostGameEvent(&evt);

	MMOGameEvent_Any evt1(GameEvent_UpdateMovePath);
	MMOUI::PostGameEvent(&evt1);

	//FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Navigation.Start"));
	//MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Navigation.Start"));
}

void MMONavigationManager::_StopNavigate()
{
	// Stop is called before crossing maps, so it cannot be cleared here
	//_NaviNodeList.clear();

	_IsNavigating = FALSE;

	_NaviInfo.Type = NT_NULL;
	_NaviInfo.MapId = 0;
	_NaviInfo.NpcId = 0;
	_NaviInfo.NeedMove = FALSE;

	ClearPathEffect();
	_PathPointList.Empty();

	MMOGameEvent_Any evt(GameEvent_StopNavigate);
	MMOUI::PostGameEvent(&evt);

	//FString text = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Navigation.Stop"));
	//MMOUI::GetUIManager()->delegateAddScreenMessage(text,MsgInfoType_Table,TEXT("MMOGame.Navigation.Stop"));

}

void MMONavigationManager::_FiltPathPoint(FVector& pos)
{
	AMMOPlayerController* mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if (mainPlayerCtrl)
	{
		FVector startPos = FVector(pos.X, pos.Y, pos.Z + 1000000);
		FVector endPos = FVector(pos.X, pos.Y, pos.Z - 1000000);
		FCheckResult* pickResult = MMO::PickPoint(startPos, endPos, mainPlayerCtrl->Pawn);
		if (pickResult)
		{
			pos = pickResult->Location;
			if (pickResult->Actor)
			{
				AMMOActor* mmoActor = Cast<AMMOActor>(pickResult->Actor);
				if (mmoActor)
				{
					pos = mmoActor->GetFootLocation();
				}
			}

			UMMONavigationHelper* naviHelper = mainPlayerCtrl->m_NavHelper;
			if (naviHelper)
			{
				pos.Z += naviHelper->PathEffectHeight;
			}
		}
	}
}

void MMONavigationManager::_GeneratePathPoint()
{
	if (!GWorld)
	{
		return;
	}

	if (!GWorld->GetWorldInfo())
	{
		return;
	}

	if (!_IsNavigating)
	{
		return;
	}

	AMMOPlayerController* mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if (!mainPlayerCtrl)
	{
		return;
	}

	UMMONavigationHelper* naviHelper = mainPlayerCtrl->m_NavHelper;
	if (!naviHelper)
	{
		return;
	}

	ClearPathEffect();

	// original waypoint list
	TArray<FVector> pathPointArray;
	if (mainPlayerCtrl->Pawn && naviHelper->_NavigationHandle)
	{
		//pathPointList.Push(mainPlayerCtrl->Pawn->Location);
		//TArray<FVector> movePoints;
		//naviHelper->_NavigationHandle->CopyMovePointsFromPathCache(*(naviHelper->_NavigationHandle->FinalDestination), movePoints);
		//pathPointList.Append(movePoints);
		//pathPointList.Push(CurReachablePoint);
		//pathPointList.Push(*(naviHelper->_NavigationHandle->FinalDestination));

		// hack: currently the order is character moves first, then _IsNavigating is set, so when entering here the waypoint index must use the previous point, zhaojun@snda
		INT32 pathPointIndex = gServerGridWorld.FetchArrayPointer() - 1;
		FVector pathPoint;
		while (gServerGridWorld.FetchPath(pathPoint, pathPointIndex))
		{
			pathPointArray.Push(pathPoint);
			++ pathPointIndex;
		}
	}

	// because some waypoints from FetchPath have a z value of 0.0, which makes the interpolation computed from waypoint distances wrong later,
	// so here all are uniformly set to 0, interpolated on the same plane, and Z is computed again in _UpdatePath
	for (INT i = 0; i < pathPointArray.Num(); ++ i)
	{
		pathPointArray(i).Z = 0.0;
	}

	// interpolated waypoint list
	INT interval = naviHelper->PathEffectInterval;
	if (interval <= 0)
	{
		interval = 400;
	}

	_PathPointList.Empty();
	PathPointInfo ppInfo;
	ppInfo.Pos = pathPointArray(0);
	ppInfo.NeedDisappear = FALSE;
	ppInfo.DisappearTime = 0.0f;
	_PathPointList.Push(ppInfo);


	//for (INT i = 0; i < pathPointArray.Num() - 1; ++ i)
	//{
	//	FVector curPathPointPos = pathPointArray(i);
	//	FVector nextPathPointPos = pathPointArray(i + 1);

	//	FLOAT dist = curPathPointPos.Distance(nextPathPointPos);
	//	
	//	if (dist < interval)
	//	{
	//		// too close, discard directly
	//		continue;
	//	}
	//	
	//	INT interCount = dist / interval;
	//	for (INT j = 0; j < interCount; ++ j)
	//	{
	//		FVector interDir = nextPathPointPos - curPathPointPos;
	//		interDir.Normalize();
	//		FVector interPathPointPos = curPathPointPos + interDir * interval * (j + 1);

	//		ppInfo.Pos = interPathPointPos;
	//		ppInfo.NeedDisappear = FALSE;
	//		ppInfo.DisappearTime = 0.0f;
	//		_PathPointList.Push(ppInfo);
	//	}
	//}

	
	FLOAT fLeakDis = 0;

	for (INT i = 0; i < pathPointArray.Num() - 1; ++ i)
	{
		const FVector& s = pathPointArray(i);
		const FVector& e = pathPointArray(i + 1);

		if (s != e)
		{
			FVector d = e - s;
			float Len = d.Size();
			d.Normalize();

			// equation
			// p = s + (t-fleakDis)*d
			// t [0]


			Len += fLeakDis;

			INT n = Len / interval;

			for (INT j = 1; j <= n; ++ j)
			{
				FVector p = s + (j * interval - fLeakDis) * d;

				ppInfo.Pos = p;
				ppInfo.NeedDisappear = FALSE;
				ppInfo.DisappearTime = 0.0f;
				_PathPointList.Push(ppInfo);
			}

			fLeakDis = Len - n * interval;

			check(fLeakDis < interval);
		}
	}

	if (_PathPointList.Num() < 2)
	{
		// fewer than two points, do not generate
		_PathPointList.Empty();
	}
}

void MMONavigationManager::_UpdatePathPoint(FLOAT deltaTime)
{
	_PathPointProgress += deltaTime;
	if (_PathPointProgress < PATH_POINT_UPDATE_TIME)
	{
		return;
	}

	_PathPointProgress = 0.0f;

	if (!GWorld)
	{
		return;
	}

	if (!GWorld->GetWorldInfo())
	{
		return;
	}

	AMMOPlayerController* mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if (!mainPlayerCtrl)
	{
		return;
	}

	if (!mainPlayerCtrl->Pawn)
	{
		return;
	}

	
	if (_PathPointList.Num() == 0)
	{
		ClearPathEffect();
		return;
	}

	FVector lastPathPointDir;
	for (INT i = 0; i < _PathPointList.Num(); ++ i)
	{
		PathPointInfo& ppInfo = _PathPointList(i);
		if (ppInfo.NeedDisappear)
		{
			if (ppInfo.DisappearTime > 0.0f)
			{
				ppInfo.DisappearTime -= PATH_POINT_UPDATE_TIME;
			}

			continue;
		}

		_FiltPathPoint(_PathPointList(i).Pos);
		FVector playerPos = mainPlayerCtrl->Pawn->Location;
		FVector pathPointPos = _PathPointList(i).Pos;
		FLOAT playerPathPointDistSquared = playerPos.DistanceSquared(pathPointPos);

		FVector pathPointToPlayerDir = FVector(playerPos - pathPointPos);
		FVector pathPointToPlayerDirN = pathPointToPlayerDir.SafeNormal();
		FVector pathPointDir = pathPointToPlayerDirN;
		if (i < _PathPointList.Num() - 1)
		{
			_FiltPathPoint(_PathPointList(i + 1).Pos);
			FVector nextPathPointPos = _PathPointList(i + 1).Pos;
			pathPointDir = FVector(nextPathPointPos - pathPointPos);
		}
		else
		{
			pathPointDir = lastPathPointDir;
		}
		lastPathPointDir = pathPointDir;
		FVector pathPointDirN = pathPointDir.SafeNormal();

		if (playerPathPointDistSquared < PATH_POINT_PLAYER_DIST_SQUARED_MAX &&
			pathPointDirN.Dot2d(pathPointToPlayerDirN) >= 0)
		{
			ppInfo.NeedDisappear = TRUE;
			ppInfo.DisappearTime = PATH_EFFECT_DISAPPEAR_TIME;
		}
	}
}

void MMONavigationManager::InitPathEffect()
{
	if (!GWorld)
	{
		return;
	}

	if (!GWorld->GetWorldInfo())
	{
		return;
	}

	AMMOPlayerController* mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
	if (!mainPlayerCtrl)
	{
		return;
	}

	UMMONavigationHelper* naviHelper = mainPlayerCtrl->m_NavHelper;
	if (!naviHelper)
	{
		return;
	}

	_PathPointList.Empty();
	_PathEffectList.Empty();
	for (INT i = 0; i < MAX_PATH_EFFECT_NUM; ++ i)
	{
		if (naviHelper->_PathPointEffect)
		{
			AMMONavitationEmitter* emitter = Cast<AMMONavitationEmitter>(GWorld->SpawnActor(AMMONavitationEmitter::StaticClass(), 
				NAME_None,
				FVector(0.0, 0.0, 0.0), 
				FRotator(0.0, 0.0, 0.0), 
				NULL, 
				TRUE, 
				0, 
				NULL, 
				NULL, 
				TRUE, 
				NULL));
			if (emitter)
			{
				emitter->SetTemplate(naviHelper->_PathPointEffect);
				emitter->SetHidden(TRUE);
				_PathEffectList.Push(emitter);
			}
		}
	}
}

void MMONavigationManager::ShutdownPathEffect()
{
	if (!GWorld)
	{
		return;
	}

	if (!GWorld->GetWorldInfo())
	{
		return;
	}

	// QA found it easily crashes on a hard exit, possibly because GWorld has already destroyed all Actors
//	for (INT i = 0; i < _PathEffectList.Num(); ++ i)
//	{
//		AMMONavitationEmitter* emitter = _PathEffectList(i);
//		if (emitter)
//		{
//			GWorld->DestroyActor(emitter);
//			emitter = NULL;
//		}
//	}
	
	_PathEffectList.Empty();
	_PathPointList.Empty();
}

void MMONavigationManager::ClearPathEffect()
{
	for (INT i = 0; i < _PathEffectList.Num(); ++ i)
	{
		AMMONavitationEmitter* emitter = _PathEffectList(i);
		if (emitter)
		{
 			emitter->SetHidden(TRUE);
			AMMOPlayerController* mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
			if (mainPlayerCtrl && mainPlayerCtrl->Pawn)
			{
				emitter->SetLocation(mainPlayerCtrl->Pawn->Location);
				emitter->SetRotation(FRotator(EC_EventParm));
			}
		}
	}
}

void MMONavigationManager::_DrawPathEffect(FLOAT deltaTime)
{
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (!mainPlayer)
	{
		return;
	}
	
	if (_PathPointList.Num() == 0 || mainPlayer->IsInMatinee)
	{
		ClearPathEffect();
		return;
	}
	
	// 4 seconds per cycle
	_PathEffectProgress += deltaTime;
	if (_PathEffectProgress > PATH_EFFECT_WAVE_TIME)
	{
		_PathEffectProgress = 0.0f;
	}

	int psIndex = 0;
	FRotator lastPathPointDir = FRotator(EC_EventParm);
	for (INT i = 0; i < _PathPointList.Num(); ++ i)
	{
		PathPointInfo& ppInfo = _PathPointList(i);
		if (ppInfo.NeedDisappear && ppInfo.DisappearTime <= 0.001f)
		{
			continue;
		}

		AMMONavitationEmitter* emitter = NULL;
		if (psIndex < _PathEffectList.Num())
		{
			emitter = _PathEffectList(psIndex);
			psIndex ++;
		}

		if (!emitter)
		{
			continue;
		}

		FVector curPathPointPos = _PathPointList(i).Pos;

		// determine direction
		FRotator curPathPointDir = FRotator(EC_EventParm);
		if (i < _PathPointList.Num() - 1)
		{
			PathPointInfo& nextPPInfo = _PathPointList(i + 1);
			curPathPointDir = FVector(nextPPInfo.Pos - curPathPointPos).Rotation();
		}
		else
		{
			curPathPointDir = lastPathPointDir;
		}
		lastPathPointDir = curPathPointDir;

		emitter->SetHidden(FALSE);
		curPathPointPos.Z += 50 * sinf(_PathEffectProgress * 2.0F * PI / PATH_EFFECT_WAVE_TIME + i) + 50;
		emitter->SetLocation(curPathPointPos);
		emitter->SetRotation(curPathPointDir);
	}

	// hide the remaining effects
	for (INT i = psIndex; i < _PathEffectList.Num(); ++ i)
	{
		AMMONavitationEmitter* emitter = _PathEffectList(i);
		if (emitter)
		{
			emitter->SetHidden(TRUE);
			AMMOPlayerController* mainPlayerCtrl = MMOFacade::getSingleton().GetMainController();
			if (mainPlayerCtrl && mainPlayerCtrl->Pawn)
			{
				emitter->SetLocation(mainPlayerCtrl->Pawn->Location);
				emitter->SetRotation(FRotator(EC_EventParm));
			}
		}
	}
}