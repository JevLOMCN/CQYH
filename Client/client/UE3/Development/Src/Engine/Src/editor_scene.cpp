#include "EnginePrivate.h"
#include "editor_scene.h"

//#if !SHIPPING_PC_GAME

#include <map>
#include <list>

#include "Core.h"
#include "SourceControl.h"

#include "zone_def.h"
#include "scene_phyx_file_def.h"
#include "nav_def.h"

INT32 g_nAdjacentX[DT_DIRECTION_8_COUNT] = 
{
	1,		//	DT_EAST,
	1,		//	DT_SOUTH_EAST,
	0,		//  DT_SOUTH,
	-1,		//  DT_SOUTH_WEST,
	-1,		//  DT_WEST,
	-1,		//  DT_NORTH_WEST,
	0,		//  DT_NORTH,
	1		//  DT_NORTH_EAST,
};

INT32 g_nAdjacentY[DT_DIRECTION_8_COUNT] = 
{
	0,		//	DT_EAST,
	1,		//	DT_SOUTH_EAST,
	1,		//  DT_SOUTH,
	1,		//  DT_SOUTH_WEST,
	0,		//  DT_WEST,
	-1,		//  DT_NORTH_WEST,
	-1,		//  DT_NORTH,
	-1		//  DT_NORTH_EAST,
};

bool g_bUseX[DT_DIRECTION_8_COUNT] = 
{
	true,		//	DT_EAST,
	true,		//	DT_SOUTH_EAST,
	false,		//  DT_SOUTH,
	false,		//  DT_SOUTH_WEST,
	true,		//  DT_WEST,
	true,		//  DT_NORTH_WEST,
	false,		//  DT_NORTH,
	false		//  DT_NORTH_EAST,
};

static std::list<UINT16> s_wZoneEdgeList[MAX_ZONE_PER_SCENE][MAX_ZONE_PER_SCENE];

CEditorScene::CEditorScene()
{
	m_strPathConfigName = TEXT("ServerSceneDataExport");
	m_szSceneName[0] = 0;
	memset(m_ZoneData, NULL, sizeof(m_ZoneData));
	m_nZoneTotal = 0;
	m_nGridTotal = 0;
}

CEditorScene::CEditorScene(const TCHAR* pStrPathConfigName)
{
	m_strPathConfigName = pStrPathConfigName;
	m_szSceneName[0] = 0;
	memset(m_ZoneData, NULL, sizeof(m_ZoneData));
	m_nZoneTotal = 0;
	m_nGridTotal = 0;
}

CEditorScene::~CEditorScene()
{
	UnInit();
}

bool CEditorScene::Init(const TCHAR* pcszName)
{
	if (pcszName == NULL)
		return FALSE;

	appStrcpy(m_szSceneName, PATH_LEN, pcszName);
	memset(m_ZoneData, NULL, sizeof(m_ZoneData));
	memset(m_szVolumeScript, 0, sizeof(m_szVolumeScript));
	m_nZoneTotal = 0;
	m_nGridTotal = 0;
	m_bModified = false;

	return TRUE;
}

bool CEditorScene::UnInit(void)
{
	m_szSceneName[0] = 0;

	for (INT32 nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	{
		for (INT32 nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
		{
			if (m_ZoneData[nZoneX][nZoneY] != NULL)
			{
				delete [] m_ZoneData[nZoneX][nZoneY];
				m_ZoneData[nZoneX][nZoneY] = NULL;
			}
		}
	}
	m_nZoneTotal = 0;
	m_nGridTotal = 0;

	return TRUE;
}

bool CEditorScene::Preprocess(void)
{
	m_nGridTotal = 0;

	for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	{
		for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
		{
			ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
			if (pZoneData)
			{
				for (int nGridX = 0; nGridX < GRID_PER_ZONE; nGridX++)
				{
					for (int nGridY = 0; nGridY < GRID_PER_ZONE; nGridY++)
					{
						if ((nZoneX * 8 + nGridX) == 399 && (nZoneY * 8 + nGridY) == 436)
							nZoneX = nZoneX;

						UINT16 usGridIndex = pZoneData->GroundGrid[nGridX][nGridY];
						UINT16 usNextGirdIndex = pZoneData->GridList[usGridIndex].uNextGrid;
						while (usNextGirdIndex)
						{
							GRID_DATA& rGrid = pZoneData->GridList[usGridIndex];
							GRID_DATA& rNextGrid = pZoneData->GridList[usNextGirdIndex];

							if (rGrid.uHighLayer + (POINT_PER_CM * 256 / GRID_LAYER_HEIGHT) >= rNextGrid.uLowLayer)
							{
								if (rGrid.uDoor == rNextGrid.uDoor && rGrid.bBlockMove == rNextGrid.bBlockMove)
								{
									rGrid.uLowLayer = rGrid.uLowLayer < rNextGrid.uLowLayer ?
										rGrid.uLowLayer : rNextGrid.uLowLayer;
									rGrid.uHighLayer = rGrid.uHighLayer > rNextGrid.uHighLayer ?
										rGrid.uHighLayer : rNextGrid.uHighLayer;
									rGrid.uNextGrid = rNextGrid.uNextGrid;
									rNextGrid.uReserved2 = 1;

									usNextGirdIndex = rGrid.uNextGrid;
								}
								else
								{
									if (rGrid.uLowLayer >= rNextGrid.uLowLayer)
										return false;

									if (rGrid.uDoor == 0 && rNextGrid.uDoor != 0)
										rGrid.uHighLayer = rNextGrid.uLowLayer;
									else if (rGrid.uDoor != 0 && rNextGrid.uDoor == 0)
										rNextGrid.uLowLayer = rGrid.uHighLayer;
									else
										rGrid.uHighLayer = rNextGrid.uLowLayer;

									usGridIndex = usNextGirdIndex;
									usNextGirdIndex = rNextGrid.uNextGrid;
								}
							}
							else
							{
								usGridIndex = usNextGirdIndex;
								usNextGirdIndex = rNextGrid.uNextGrid;
							}
						}
					}
				}

				std::map<int, int> convert_map;

				int j = 1;
				for (int i = 1; i <= pZoneData->nGridCount; i++)
				{
					if (pZoneData->GridList[i].uReserved2 == 1)
					{
						continue;
					}

					memcpy(pZoneData->GridList + j, pZoneData->GridList + i, sizeof(GRID_DATA));
					convert_map[i] = j;
					j++;
				}
				pZoneData->nGridCount = j - 1;

				for (int i = 0; i < GRID_PER_ZONE; i++)
				{
					for (int j = 0; j < GRID_PER_ZONE; j++)
					{
						pZoneData->GroundGrid[i][j] = convert_map[pZoneData->GroundGrid[i][j]];
					}
				}

				for (int i = 1; i <= pZoneData->nGridCount; i++)
				{
					pZoneData->GridList[i].uNextGrid = convert_map[pZoneData->GridList[i].uNextGrid];
				}

				m_nGridTotal += pZoneData->nGridCount;
			}
		}
	}

	for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	{
		for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
		{
			ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
			if (pZoneData)
			{
				for (int nGridX = 0; nGridX < GRID_PER_ZONE; nGridX++)
				{
					for (int nGridY = 0; nGridY < GRID_PER_ZONE; nGridY++)
					{
						static int s_nX = -1;
						static int s_nY = -1;
						if ((nZoneX * 8 + nGridX) == s_nX / 64 && (nZoneY * 8 + nGridY) == s_nY / 64)
							nZoneX = nZoneX;

						UINT16 usGridIndex = pZoneData->GroundGrid[nGridX][nGridY];
						while (usGridIndex)
						{
							UINT16 usNextGird = pZoneData->GridList[usGridIndex].uNextGrid;
							GRID_DATA* pGrid = pZoneData->GridList + usGridIndex;

							for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
							{
								int nAdjGridX = nZoneX * GRID_PER_ZONE + nGridX + g_nAdjacentX[nDirection];
								int nAdjGridY = nZoneY * GRID_PER_ZONE + nGridY + g_nAdjacentY[nDirection];

								const GRID_DATA* pcAdjGrid = _GetLowerGrid(nAdjGridX * GRID_WIDTH, nAdjGridY * GRID_WIDTH, pGrid->uHighLayer * GRID_LAYER_HEIGHT + GRID_WIDTH);
								if (pcAdjGrid == NULL || 
									(((int)pGrid->uHighLayer - (int)pcAdjGrid->uHighLayer) * GRID_LAYER_HEIGHT > GRID_WIDTH))
//									(((int)pGrid->uHighLayer - (int)pcAdjGrid->uHighLayer) * GRID_LAYER_HEIGHT < -GRID_WIDTH))
								{
									pGrid->bBorder = 1;
									break;
								}
							}

							usGridIndex = usNextGird;
						}
					}
				}
			}
		}
	}

	// obstacle check:
	for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	{
		for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
		{
			ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
			if (pZoneData)
			{
				for (int nGridX = 0; nGridX < GRID_PER_ZONE; nGridX++)
				{
					for (int nGridY = 0; nGridY < GRID_PER_ZONE; nGridY++)
					{
						UINT16 usGridIndex = pZoneData->GroundGrid[nGridX][nGridY];
						while (usGridIndex)
						{
							UINT16 usNextGird = pZoneData->GridList[usGridIndex].uNextGrid;
							GRID_DATA* pGrid = pZoneData->GridList + usGridIndex;
							INT32 nZ = pGrid->uHighLayer * GRID_LAYER_HEIGHT + 200 * POINT_PER_CM;

							for(INT32 i = 0; i < 64; i++)
							{
								bool bObstacle = false;
								for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
								{
									int nAdjGridX = nZoneX * GRID_PER_ZONE + nGridX + i * g_nAdjacentX[nDirection];
									int nAdjGridY = nZoneY * GRID_PER_ZONE + nGridY + i * g_nAdjacentY[nDirection];


									const GRID_DATA* pCurGrid = _GetLowerGrid(nAdjGridX * GRID_WIDTH, nAdjGridY * GRID_WIDTH, nZ);
									if(pCurGrid == NULL)
										continue;


									//int nAdjZ = pCurGrid->uHighLayer * GRID_LAYER_HEIGHT - 200 * POINT_PER_CM;

									if(nZ < pCurGrid->uHighLayer * GRID_LAYER_HEIGHT + 100 * POINT_PER_CM && nZ > pCurGrid->uLowLayer * GRID_LAYER_HEIGHT)
									{
										bObstacle = true;
										break;
									}

									if(pCurGrid->uNextGrid != 0)
									{
										GRID_DATA* pUpperGrid = pZoneData->GridList + pCurGrid->uNextGrid;

										if(nZ > pUpperGrid->uLowLayer * GRID_LAYER_HEIGHT) 
										{
											bObstacle = true;
											break;
										}
									}


									//if(abs(nAdjZ - nZ) > i * GRID_WIDTH)
									//{
									//	bObstacle = true;
									//	break;
									//}
								}
								if(!bObstacle)
									pGrid->uNoObstacle++;
								else
									break;
							}

							usGridIndex = usNextGird;
						}
					}
				}
			}
		}
	}

	m_bModified = true;

	return true;
}

int _get_angle(int xl, int yl, int xr, int yr)
{
	int angle = 0;
	angle = atan((float)(yr - yl) / (xr - xl)) * 180 / PI;
	if (xr < xl)
		angle += 180;
	else if (yr < yl)
		angle += 360;

	return angle;
}

float _get_len2(int xl, int yl, int xr, int yr)
{
	return (float)(xl - xr) * (xl - xr) + (float)(yl - yr) * (yl - yr);
}

struct ANGLE_DATA
{
	int nStartAngle;
	int	nEndAngle;
};

typedef std::list<ANGLE_DATA> ANGLE_DATA_LIST;

struct VERTEX_DATA
{
	VERTEX_DATA() {nMinEdgeLen = INT_MAX;}
	int				nMinEdgeLen;
	ANGLE_DATA_LIST	AngleDataList;
};

typedef std::map<int, VERTEX_DATA> VERTEX_MAP;
static VERTEX_MAP s_VertexMap;

bool CEditorScene::ProcessNavData(unsigned char* data, const int dataSize)
{
	BOOL bOutOfPoly = FALSE;

	if (data == NULL)
		goto Exit0;

	SCENE_NAV_HEADER* pHeader = (SCENE_NAV_HEADER*)data;
	NAV_EDGE_DATA* pEdgeData = (NAV_EDGE_DATA*)(data + sizeof(SCENE_NAV_HEADER) + sizeof(NAV_VERTEX_DATA) * pHeader->wVertexCount);
	NAV_VERTEX_DATA* pFixedVertexData = (NAV_VERTEX_DATA*)(pEdgeData + pHeader->wEdgeCount);

	if (pHeader->wEdgeCount >= 0xFFFE)
		goto Exit0;

	for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	{
		for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
		{
			s_wZoneEdgeList[nZoneX][nZoneY].clear();
		}
	}

	for (int i = 0; i < pHeader->wEdgeCount;)
	{
		unsigned short wStartEdge = i;
		NAV_VERTEX_DATA aabb[2];

		aabb[0].nX = aabb[0].nY = aabb[0].nZ = INT_MAX;
		aabb[1].nX = aabb[1].nY = aabb[1].nZ = 0;

		do 
		{
			NAV_VERTEX_DATA* pLeftPoint = pHeader->VertexData + pEdgeData[i].wLeftPoint;
			NAV_VERTEX_DATA* pRightPoint = pHeader->VertexData + pEdgeData[i].wRightPoint;

			aabb[0].nX = pLeftPoint->nX < aabb[0].nX ? pLeftPoint->nX : aabb[0].nX;
			aabb[0].nY = pLeftPoint->nY < aabb[0].nY ? pLeftPoint->nY : aabb[0].nY;

			aabb[1].nX = pLeftPoint->nX > aabb[1].nX ? pLeftPoint->nX : aabb[1].nX;
			aabb[1].nY = pLeftPoint->nY > aabb[1].nY ? pLeftPoint->nY : aabb[1].nY;

			aabb[0].nX = pRightPoint->nX < aabb[0].nX ? pRightPoint->nX : aabb[0].nX;
			aabb[0].nY = pRightPoint->nY < aabb[0].nY ? pRightPoint->nY : aabb[0].nY;

			aabb[1].nX = pRightPoint->nX > aabb[1].nX ? pRightPoint->nX : aabb[1].nX;
			aabb[1].nY = pRightPoint->nY > aabb[1].nY ? pRightPoint->nY : aabb[1].nY;

			i++;
		} while (pEdgeData[i].wLinkEdge[0] == wStartEdge);

		for (int nZoneX = aabb[0].nX / ZONE_WIDTH - 2; nZoneX <= aabb[1].nX / ZONE_WIDTH + 2; nZoneX++)
		{
			for (int nZoneY = aabb[0].nY / ZONE_WIDTH - 2; nZoneY < aabb[1].nY / ZONE_WIDTH + 2; nZoneY++)
			{
				if (nZoneX < 0 || nZoneY < 0 || nZoneX >= MAX_ZONE_PER_SCENE || nZoneY >= MAX_ZONE_PER_SCENE)
					continue;
				s_wZoneEdgeList[nZoneX][nZoneY].push_back(wStartEdge);
			}
		}
	}

	for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	{
		for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
		{
			ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
			if (pZoneData)
			{
				for (int i = 0; i < MAX_EDGE_INDEX_PER_ZONE; i++)
					pZoneData->wEdge[i] = 0xFFFE;

				for (int nGridX = 0; nGridX < GRID_PER_ZONE; nGridX++)
				{
					for (int nGridY = 0; nGridY < GRID_PER_ZONE; nGridY++)
					{
						static int s_nX = -1;
						static int s_nY = -1;

						if ((nZoneX * 8 + nGridX) == (s_nX / 64) && (nZoneY * 8 + nGridY) == (s_nY / 64))
							nZoneX = nZoneX;

						UINT16 usGridIndex = pZoneData->GroundGrid[nGridX][nGridY];
						while (usGridIndex)
						{
							UINT16 usNextGird = pZoneData->GridList[usGridIndex].uNextGrid;
							GRID_DATA* pGrid = pZoneData->GridList + usGridIndex;

							unsigned short wEdge = _GetEdge(pHeader->VertexData, pHeader->wVertexCount, pEdgeData, pHeader->wEdgeCount, 
								nZoneX * ZONE_WIDTH + nGridX * GRID_WIDTH + GRID_WIDTH / 2, nZoneY * ZONE_WIDTH + nGridY * GRID_WIDTH + GRID_WIDTH / 2, 
								pGrid->uHighLayer * GRID_LAYER_HEIGHT, bOutOfPoly);
							pGrid->bOutOfPoly = bOutOfPoly;

							if (pGrid->bBlockMove)
								pGrid->bOutOfPoly = true;

							int nInsertPos = -1;
							int i = 0;
							for (; i < MAX_EDGE_INDEX_PER_ZONE; i++)
							{
								if (pZoneData->wEdge[i] == wEdge)
								{
									nInsertPos = i;
									break;
								}
								if (nInsertPos == -1 && pZoneData->wEdge[i] == 0xFFFE)
								{
									nInsertPos = i;
									break;
								}
							}
							if (nInsertPos >= 0)
							{
								pZoneData->wEdge[nInsertPos] = wEdge;
								pGrid->uEdge = nInsertPos;
							}
							else
							{
								float X = (nZoneX * ZONE_WIDTH + nGridX * GRID_WIDTH);
								float Y = (nZoneY * ZONE_WIDTH + nGridY * GRID_WIDTH);
								float Z = pFixedVertexData->nZ;
								ConvertCoordS2C(X, Y, Z);
								GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Too many nav edges @XYZ= %d %d %d"), 
									(INT32)X, 
									(INT32)Y,
									(INT32)Z), NULL);	//Bruce@snda 
								goto Exit0;
							}

							usGridIndex = usNextGird;
						}
					}
				}
			}
		}
	}

	s_VertexMap.clear();
	for (int nEdgeIndex = 0; nEdgeIndex < pHeader->wEdgeCount; nEdgeIndex++)
	{
		VERTEX_DATA& rVertexData = s_VertexMap[(int)(pEdgeData[nEdgeIndex].wLeftPoint)];
		ANGLE_DATA AngleData;
		int xl = pHeader->VertexData[pEdgeData[nEdgeIndex].wLeftPoint].nX;
		int yl = pHeader->VertexData[pEdgeData[nEdgeIndex].wLeftPoint].nY;
		int xr = pHeader->VertexData[pEdgeData[nEdgeIndex].wRightPoint].nX;
		int yr = pHeader->VertexData[pEdgeData[nEdgeIndex].wRightPoint].nY;
		AngleData.nStartAngle = _get_angle(xl, yl, xr, yr);

		float edge_len = _get_len2(xl, yl, xr, yr);
		if (edge_len < (float)rVertexData.nMinEdgeLen * rVertexData.nMinEdgeLen)
			rVertexData.nMinEdgeLen = sqrt((float)edge_len);

		for (int nIndex = 0; nIndex < MAX_EDGE_PER_POLY; nIndex++)
		{
			int nLinkEdgeIndex = pEdgeData[nEdgeIndex].wLinkEdge[nIndex];
			if (nLinkEdgeIndex == 0xFFFF)
				break;
			if (pEdgeData[nLinkEdgeIndex].wRightPoint == pEdgeData[nEdgeIndex].wLeftPoint)
			{
				int xl = pHeader->VertexData[pEdgeData[nLinkEdgeIndex].wLeftPoint].nX;
				int yl = pHeader->VertexData[pEdgeData[nLinkEdgeIndex].wLeftPoint].nY;
				int xr = pHeader->VertexData[pEdgeData[nLinkEdgeIndex].wRightPoint].nX;
				int yr = pHeader->VertexData[pEdgeData[nLinkEdgeIndex].wRightPoint].nY;
				AngleData.nEndAngle = _get_angle(xr, yr, xl, yl);

				float edge_len = _get_len2(xl, yl, xr, yr);
				if (edge_len < (float)rVertexData.nMinEdgeLen * rVertexData.nMinEdgeLen)
					rVertexData.nMinEdgeLen = sqrt((float)edge_len);

				break;
			}
		}

		rVertexData.AngleDataList.push_back(AngleData);
	}

	for (VERTEX_MAP::iterator it = s_VertexMap.begin();
		it != s_VertexMap.end(); ++it)
	{
		int nVertexIndex = it->first;
		VERTEX_DATA& rVertexData = it->second;
		int nMergeCount = 0;
		while (rVertexData.AngleDataList.size() > 1 && nMergeCount < 10)
		{
			ANGLE_DATA& AngleData = *(rVertexData.AngleDataList.begin());
			for (ANGLE_DATA_LIST::iterator it2 = rVertexData.AngleDataList.begin();
				it2 != rVertexData.AngleDataList.end(); )
			{
				if (it2->nStartAngle == AngleData.nEndAngle)
				{
					AngleData.nEndAngle = it2->nEndAngle;
					it2 = rVertexData.AngleDataList.erase(it2);
				}
				else if (it2->nEndAngle == AngleData.nStartAngle)
				{
					AngleData.nStartAngle = it2->nStartAngle;
					it2 = rVertexData.AngleDataList.erase(it2);
				}
				else if (it2->nStartAngle == it2->nEndAngle)
				{
					it2 = rVertexData.AngleDataList.erase(it2);
				}
				else
				{
					++it2;
				}
			}
			nMergeCount++;
		}
	}

	for (int nVertexIndex = pHeader->wEdgeCount; nVertexIndex < pHeader->wVertexCount; nVertexIndex++)
	{
		VERTEX_MAP::iterator it = s_VertexMap.find(nVertexIndex);

		if (it != s_VertexMap.end() && it->second.AngleDataList.size() == 1)
		{
			int angle = 0;
			if (it->second.AngleDataList.begin()->nStartAngle < it->second.AngleDataList.begin()->nEndAngle)
			{
				angle = it->second.AngleDataList.begin()->nStartAngle + it->second.AngleDataList.begin()->nEndAngle;
				angle /= 2;
			}
			else
			{
				angle = 360 - it->second.AngleDataList.begin()->nStartAngle + it->second.AngleDataList.begin()->nEndAngle;
				angle = it->second.AngleDataList.begin()->nStartAngle + angle / 2;
				if (angle > 360)
					angle -= 360;
			}
			int len = it->second.nMinEdgeLen / 2;
			if (len > 3 * GRID_WIDTH)
				len = 3 * GRID_WIDTH;
			pFixedVertexData[nVertexIndex].nX = pHeader->VertexData[nVertexIndex].nX + len * cos(angle * PI / 180);
			pFixedVertexData[nVertexIndex].nY = pHeader->VertexData[nVertexIndex].nY + len * sin(angle * PI / 180);
		}
		else
		{
			float X = pHeader->VertexData[nVertexIndex].nX;
			float Y = pHeader->VertexData[nVertexIndex].nY;
			float Z = pHeader->VertexData[nVertexIndex].nZ;
			ConvertCoordS2C(X, Y, Z);
			GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, NULL, *FString::Printf(TEXT("error nav mesh vertex @XYZ= %d %d %d"), 
				(INT32)X, 
				(INT32)Y,
				(INT32)Z), NULL);		//Bruce@snda 
		}
	}

	//for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	//{
	//	for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
	//	{
	//		ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
	//		if (pZoneData)
	//		{
	//			for (int nGridX = 0; nGridX < GRID_PER_ZONE; nGridX++)
	//			{
	//				for (int nGridY = 0; nGridY < GRID_PER_ZONE; nGridY++)
	//				{
	//					UINT16 usGridIndex = pZoneData->GroundGrid[nGridX][nGridY];
	//					while (usGridIndex)
	//					{
	//						UINT16 usNextGird = pZoneData->GridList[usGridIndex].uNextGrid;
	//						GRID_DATA* pGrid = pZoneData->GridList + usGridIndex;

	//						for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
	//						{
	//							int nAdjGridX = nZoneX * GRID_PER_ZONE + nGridX + g_nAdjacentX[nDirection];
	//							int nAdjGridY = nZoneY * GRID_PER_ZONE + nGridY + g_nAdjacentY[nDirection];

	//							const GRID_DATA* pcAdjGrid = _GetLowerGrid(nAdjGridX * GRID_WIDTH, nAdjGridY * GRID_WIDTH, pGrid->uHighLayer * GRID_LAYER_HEIGHT + GRID_WIDTH);
	//							if (pcAdjGrid == NULL || pcAdjGrid->bOutOfPoly)
	//							{
	//								pGrid->bBorder = 1;
	//								break;
	//							}
	//						}

	//						usGridIndex = usNextGird;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	//{
	//	for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
	//	{
	//		ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
	//		if (pZoneData)
	//		{
	//			bool bValidGrid = false;

	//			for (int nGridX = 0; nGridX < GRID_PER_ZONE && !bValidGrid; nGridX++)
	//			{
	//				for (int nGridY = 0; nGridY < GRID_PER_ZONE && !bValidGrid; nGridY++)
	//				{
	//					UINT16 usGridIndex = pZoneData->GroundGrid[nGridX][nGridY];
	//					while (usGridIndex  && !bValidGrid)
	//					{
	//						UINT16 usNextGird = pZoneData->GridList[usGridIndex].uNextGrid;
	//						GRID_DATA* pGrid = pZoneData->GridList + usGridIndex;

	//						if (!pGrid->bOutOfPoly)
	//							bValidGrid = true;

	//						usGridIndex = usNextGird;
	//					}
	//				}
	//			}

	//			if (!bValidGrid)
	//			{
	//				delete [] pZoneData;
	//				m_ZoneData[nZoneX][nZoneY] = NULL;
	//			}
	//		}
	//	}
	//}

//	s_VertexMap.clear();

	return true;
Exit0:
	return false;
}

bool CEditorScene::SetVolumeScript(INT32 nVolumeID, const char* pcszScript)
{
    if (nVolumeID <= 0 && nVolumeID >= MAX_VOLUME_COUNT)
        return false;
    if (strlen(pcszScript) >= MAX_EMBEDDED_SCRIPT_LEN)
        return false;

    strncpy_s(m_szVolumeScript[nVolumeID], pcszScript, MAX_EMBEDDED_SCRIPT_LEN);

    return true;
}

bool CEditorScene::SetMaxVolumeTriggerCount(INT32 nVolumeID, INT32 nMaxTriggerCount)
{
	if (nVolumeID <= 0 && nVolumeID >= MAX_VOLUME_COUNT)
		return false;

	m_nMaxTriggerCount[nVolumeID] = nMaxTriggerCount;

	return true;
}

bool CEditorScene::Load(void)
{
	//TCHAR szFilePath[PATH_LEN];
	FString ExportDir;

	GConfig->GetString(TEXT("Configuration"), *m_strPathConfigName, ExportDir, GEditorIni);

	FString szFilePath = FString::Printf(TEXT("%s%s\\%s.%s"), 
		*ExportDir, m_szSceneName, m_szSceneName, TEXT("phx"));

	return LoadPhxFromFile(*szFilePath);
}

bool CEditorScene::Save(void)
{
	bool bResult = false;
	int nRetCode = 0;
	FILE* pFile = NULL;
	FString szFilePath;
	char* pszData = NULL;
	FString ExportDir;

	if (!m_bModified)
		return true;
	m_bModified = false;

	GConfig->GetString(TEXT("Configuration"), *m_strPathConfigName, ExportDir, GEditorIni);

	szFilePath = FString::Printf(TEXT("%s%s\\"), *ExportDir, m_szSceneName);
	GFileManager->MakeDirectory(*szFilePath,TRUE);

	szFilePath = FString::Printf(TEXT("%s%s\\%s.%s"), 
		*ExportDir, m_szSceneName, m_szSceneName, TEXT("phx"));

#if HAVE_SCC
	FCheckoutResult sccResult = FSourceControl::TryCheckOut(szFilePath);
#endif

	_tfopen_s(&pFile, *szFilePath, TEXT("wb"));
	if (pFile == NULL)
		goto Exit0;

	int nDataSize = sizeof(SCENE_PHYX_HEADER) + (m_nGridTotal + m_nZoneTotal) * sizeof(GRID_DATA) + m_nZoneTotal * sizeof(ZONE_DATA);
	pszData = new char[nDataSize];
	if (pszData == NULL)
		goto Exit0;

	SCENE_PHYX_HEADER* pHeader = (SCENE_PHYX_HEADER*)pszData;
	char* pszPointer = pszData + sizeof(*pHeader);

	pHeader->dwTag = SCENE_PHYX_FILE_TAG;
	pHeader->dwVersion = SCENE_PHYX_FILE_VERSION;
	pHeader->dwZoneCount = 0;
	pHeader->nMaxZoneX = pHeader->nMaxZoneY = 0;
    memcpy(pHeader->szVolumnScript, m_szVolumeScript, sizeof(pHeader->szVolumnScript));
	memcpy(pHeader->nMaxVolumeTriggerCount, m_nMaxTriggerCount, sizeof(pHeader->nMaxVolumeTriggerCount));

	for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	{
		for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
		{
			ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
			if (pZoneData)
			{
				if (pZoneData->usZoneX != nZoneX ||
					pZoneData->usZoneY != nZoneY)
				{
					goto Exit0;
				}

				memcpy(pszPointer, pZoneData, sizeof(ZONE_DATA) + sizeof(GRID_DATA) * (pZoneData->nGridCount + 1));
				pszPointer += sizeof(ZONE_DATA) + sizeof(GRID_DATA) * (pZoneData->nGridCount + 1);

				if (nZoneX > pHeader->nMaxZoneX)
					pHeader->nMaxZoneX = nZoneX;
				if (nZoneY > pHeader->nMaxZoneY)
					pHeader->nMaxZoneY = nZoneY;

				pHeader->dwZoneCount++;
			}
		}
	}

	nRetCode = fwrite(pszData, 1, nDataSize, pFile);
	if (nRetCode != nDataSize)
		goto Exit0;

	fclose(pFile);
	pFile = NULL;

#if HAVE_SCC
	sccResult.TryAddToDefaultChangelist();
#endif

	bResult = true;
Exit0:
	if (pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

	if (pszData)
	{
		delete [] pszData;
		pszData = NULL;
	}

	return bResult;
}

GRID_DATA* CEditorScene::GetGroundGrid(int nX, int nY)
{
	INT32 nZoneX = nX / ZONE_WIDTH;
	INT32 nZoneY = nY / ZONE_WIDTH;
	INT32 nGridXInZone = (nX / GRID_WIDTH) % GRID_PER_ZONE;
	INT32 nGridYInZone = (nY / GRID_WIDTH) % GRID_PER_ZONE;

	if (nZoneX < 0 || nZoneX >= MAX_ZONE_PER_SCENE ||
		nZoneY < 0 || nZoneY >= MAX_ZONE_PER_SCENE)
	{
		return NULL;
	}

	ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
	if (pZoneData == NULL)
		return NULL;

	UINT16 usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];
	if (usGridIndex == NULL || usGridIndex >= MAX_NEXT_GRID)
		return NULL;

	return pZoneData->GridList + usGridIndex;
}

GRID_DATA* CEditorScene::GetGridByIndex(int nX, int nY, int Index)
{
	if ( Index == NULL )
		return NULL;

	INT32 nZoneX = nX / ZONE_WIDTH;
	INT32 nZoneY = nY / ZONE_WIDTH;
	INT32 nGridXInZone = (nX / GRID_WIDTH) % GRID_PER_ZONE;
	INT32 nGridYInZone = (nY / GRID_WIDTH) % GRID_PER_ZONE;

	if (nZoneX < 0 || nZoneX >= MAX_ZONE_PER_SCENE ||
		nZoneY < 0 || nZoneY >= MAX_ZONE_PER_SCENE)
	{
		return NULL;
	}

	ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
	if (pZoneData == NULL)
		return NULL;

	return pZoneData->GridList + Index;
}

GRID_DATA* CEditorScene::GetGrid(int nX, int nY, int nZ)
{
	INT32 nZoneX = nX / ZONE_WIDTH;
	INT32 nZoneY = nY / ZONE_WIDTH;
	INT32 nGridXInZone = (nX / GRID_WIDTH) % GRID_PER_ZONE;
	INT32 nGridYInZone = (nY / GRID_WIDTH) % GRID_PER_ZONE;
	UINT32 uLayer = nZ / GRID_LAYER_HEIGHT;

	if (nX < 0 || nY < 0 || nZ < 0)
	{
		return NULL;
	}

	if (nZoneX < 0 || nZoneX >= MAX_ZONE_PER_SCENE ||
		nZoneY < 0 || nZoneY >= MAX_ZONE_PER_SCENE)
	{
		return NULL;
	}

	ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
	if (pZoneData == NULL)
		return NULL;

	UINT16 usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];
	while (usGridIndex != NULL && uLayer < pZoneData->GridList[usGridIndex].uLowLayer)
	{
		usGridIndex = pZoneData->GridList[usGridIndex].uNextGrid;
	}

	if (usGridIndex != NULL && uLayer < pZoneData->GridList[usGridIndex].uHighLayer)
	{
		return pZoneData->GridList + usGridIndex;
	}

	return NULL;
}

GRID_DATA* CEditorScene::CreateGrid(int nX, int nY, int nLowZ, int nHighZ)
{
	INT32 nZoneX = nX / ZONE_WIDTH;
	INT32 nZoneY = nY / ZONE_WIDTH;
	INT32 nGridXInZone = (nX / GRID_WIDTH) % GRID_PER_ZONE;
	INT32 nGridYInZone = (nY / GRID_WIDTH) % GRID_PER_ZONE;
	UINT32 uLowLayer = nLowZ / GRID_LAYER_HEIGHT;
	UINT32 uHighLayer = nHighZ / GRID_LAYER_HEIGHT;
	UINT16 usNewGridIndex = NULL;

	if (uLowLayer == uHighLayer)
		return NULL;

	if (nX < 0 || nY < 0 || nLowZ < 0 || nHighZ < 0 || nLowZ >= nHighZ)
	{
		return NULL;
	}

	if (nZoneX < 0 || nZoneX >= MAX_ZONE_PER_SCENE ||
		nZoneY < 0 || nZoneY >= MAX_ZONE_PER_SCENE)
	{
		return NULL;
	}

	ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
	if (pZoneData == NULL)
	{
		pZoneData = (ZONE_DATA*)new char[sizeof(ZONE_DATA) + sizeof(GRID_DATA) * MAX_NEXT_GRID];
		if (pZoneData == NULL)
			return NULL;

		memset(pZoneData, 0, sizeof(ZONE_DATA) + sizeof(GRID_DATA) * MAX_NEXT_GRID);
		pZoneData->usZoneX = nZoneX;
		pZoneData->usZoneY = nZoneY;
		pZoneData->nGridCount = 0;

		m_ZoneData[nZoneX][nZoneY] = pZoneData;
		m_nZoneTotal++;
	}

	

	UINT16 usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];
	if (usGridIndex == NULL)
	{
		if (pZoneData->nGridCount >= MAX_NEXT_GRID - 1)
			return NULL;
		// no ground grid
		usNewGridIndex = ++pZoneData->nGridCount;
		m_nGridTotal++;
		pZoneData->GroundGrid[nGridXInZone][nGridYInZone] = usNewGridIndex;

		pZoneData->GridList[usNewGridIndex].uLowLayer = uLowLayer;
		pZoneData->GridList[usNewGridIndex].uHighLayer = uHighLayer;
	}
	else if (uHighLayer < pZoneData->GridList[usGridIndex].uLowLayer)
	{
		if (pZoneData->nGridCount >= MAX_NEXT_GRID - 1)
			return NULL;
		// lower than ground grid
		usNewGridIndex = ++pZoneData->nGridCount;
		m_nGridTotal++;
		pZoneData->GroundGrid[nGridXInZone][nGridYInZone] = usNewGridIndex;

		pZoneData->GridList[usNewGridIndex].uLowLayer = uLowLayer;
		pZoneData->GridList[usNewGridIndex].uHighLayer = uHighLayer;
		pZoneData->GridList[usNewGridIndex].uNextGrid = usGridIndex;
	}
	else if (uLowLayer > pZoneData->GridList[usGridIndex].uHighLayer)
	{
		// higher than ground grid
		UINT16 usInsertPos = usGridIndex;
		usGridIndex = pZoneData->GridList[usGridIndex].uNextGrid;

		while (usGridIndex != NULL && uHighLayer >= pZoneData->GridList[usGridIndex].uLowLayer)
		{
			usInsertPos = usGridIndex;
			usGridIndex = pZoneData->GridList[usGridIndex].uNextGrid;
		}

		if (uHighLayer < pZoneData->GridList[usInsertPos].uLowLayer)
		{
			// impossible
			return NULL;
		}
		else if (uLowLayer > pZoneData->GridList[usInsertPos].uHighLayer)
		{
			if (pZoneData->nGridCount >= MAX_NEXT_GRID - 1)
				return NULL;
			// higher than insert pos grid and lower than next grid
			usNewGridIndex = ++pZoneData->nGridCount;
			m_nGridTotal++;

			pZoneData->GridList[usNewGridIndex].uLowLayer = uLowLayer;
			pZoneData->GridList[usNewGridIndex].uHighLayer = uHighLayer;
			pZoneData->GridList[usNewGridIndex].uNextGrid = pZoneData->GridList[usInsertPos].uNextGrid;
			pZoneData->GridList[usInsertPos].uNextGrid = usNewGridIndex;
		}
		else
		{
			usNewGridIndex = usInsertPos;
			pZoneData->GridList[usInsertPos].uLowLayer = 
				pZoneData->GridList[usInsertPos].uLowLayer < uLowLayer ? pZoneData->GridList[usInsertPos].uLowLayer : uLowLayer;
			pZoneData->GridList[usInsertPos].uHighLayer = 
				pZoneData->GridList[usInsertPos].uHighLayer > uHighLayer ? pZoneData->GridList[usInsertPos].uHighLayer : uHighLayer;
		}
	}
	else 
	{
		usNewGridIndex = usGridIndex;
		pZoneData->GridList[usGridIndex].uLowLayer = 
			pZoneData->GridList[usGridIndex].uLowLayer < uLowLayer ? pZoneData->GridList[usGridIndex].uLowLayer : uLowLayer;
		pZoneData->GridList[usGridIndex].uHighLayer = 
			pZoneData->GridList[usGridIndex].uHighLayer > uHighLayer ? pZoneData->GridList[usGridIndex].uHighLayer : uHighLayer;
	}

	if (usNewGridIndex == NULL)
		return NULL;
	else
		return pZoneData->GridList + usNewGridIndex;
}

const GRID_DATA* CEditorScene::_GetLowerGrid(INT64 llX, INT64 llY, INT64 llZ)
{
	INT32 nZoneX = llX / ZONE_WIDTH;
	INT32 nZoneY = llY / ZONE_WIDTH;
	INT32 nGridXInZone = (llX / GRID_WIDTH) % GRID_PER_ZONE;
	INT32 nGridYInZone = (llY / GRID_WIDTH) % GRID_PER_ZONE;
	UINT16 usLayer = llZ / GRID_LAYER_HEIGHT;
	ZONE_DATA* pZoneData = NULL;
	UINT16 usGridIndex = NULL;
	UINT16 usNextIndex = NULL;

	if (llX < 0 || llY < 0 || llZ < 0)
		return NULL;

	if (nZoneX < 0 || nZoneX >= MAX_ZONE_PER_SCENE)
		return NULL;

	if (nZoneY < 0 || nZoneY >= MAX_ZONE_PER_SCENE)
		return NULL;

	pZoneData = m_ZoneData[nZoneX][nZoneY];
	if (pZoneData == NULL)
		return NULL;

	usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];

	usNextIndex = pZoneData->GridList[usGridIndex].uNextGrid;
	while (usNextIndex != NULL && usLayer > pZoneData->GridList[usNextIndex].uLowLayer)
	{
		usGridIndex = usNextIndex;
		usNextIndex = pZoneData->GridList[usNextIndex].uNextGrid;
	}

	if (usGridIndex == NULL)
		return NULL;
	else
		return pZoneData->GridList + usGridIndex;
}

inline unsigned short CEditorScene::_GetEdge(NAV_VERTEX_DATA* VertexData, int nVertexCount, NAV_EDGE_DATA* EdgeData, 
	int nEdgeCount, int nX, int nY, int nZ, BOOL& rbOutOfPoly)
{
	__int64 llMinDistance = 0x7FFFFFFFFFFFFFFF;
	unsigned short wNearestEdge = 0xFFFF;

	for (std::list<UINT16>::iterator it = s_wZoneEdgeList[nX / ZONE_WIDTH][nY /  ZONE_WIDTH].begin();
		it != s_wZoneEdgeList[nX / ZONE_WIDTH][nY /  ZONE_WIDTH].end(); ++it)
	{
		unsigned short wStartEdge = *it;
		unsigned short i = wStartEdge;
		bool bFound = true;
		int zmin = INT_MAX;
		int zmax = INT_MIN;

		do 
		{
			if (bFound)
			{
				NAV_VERTEX_DATA* left = VertexData + EdgeData[i].wLeftPoint;
				NAV_VERTEX_DATA* right = VertexData + EdgeData[i].wRightPoint;

				if (!_CheckPos(nX, nY, left, right))
					bFound = false;

				if (left->nZ < zmin)
					zmin = left->nZ;
				if (left->nZ > zmax)
					zmax = left->nZ;
				if (right->nZ < zmin)
					zmin = right->nZ;
				if (right->nZ > zmax)
					zmax = right->nZ;
			}

			__int64 llDistance = ((__int64)nX - VertexData[i].nX) * ((__int64)nX - VertexData[i].nX) + 
				((__int64)nY - VertexData[i].nY) * ((__int64)nY - VertexData[i].nY) +
				((__int64)nZ - VertexData[i].nZ) * ((__int64)nZ - VertexData[i].nZ);
			if (llDistance < llMinDistance)
			{
				llMinDistance = llDistance;
				wNearestEdge = wStartEdge;
			}

			i++;
		} while (EdgeData[i].wLinkEdge[0] == wStartEdge);

		zmin -= GRID_WIDTH * 4;
		zmax += GRID_WIDTH * 4;

		if (bFound && nZ > zmin && nZ < zmax)
		{
			rbOutOfPoly = FALSE;
			return wStartEdge;
		}
	}

	rbOutOfPoly = TRUE;
	return wNearestEdge;
}

inline bool CEditorScene::_CheckPos(__int64 xp, __int64 yp, NAV_VERTEX_DATA* pLeftPoint, NAV_VERTEX_DATA* pRightPoint)
{
	__int64 xl = pLeftPoint->nX;
	__int64 yl = pLeftPoint->nY;
	__int64 xr = pRightPoint->nX;
	__int64 yr = pRightPoint->nY;

	return ((yr - yl) * xp + (xl - xr) * yp + (xr*yl - xl*yr)) <= 0;
}

bool CEditorScene::LoadPhxFromFile( const TCHAR* pPhxFilePath )
{
	if (pPhxFilePath == NULL)
	{
		return false;
	}

	bool bResult = false;
	int nRetCode = 0;
	//FILE* pFile = NULL;
	char* pszData = NULL;

	// check the file ext name == .phx;  let it go...let it go

	//fopen_s(&pFile, pPhxFilePath, "rb");
	warnf(NAME_Warning, TEXT("Loading Phx file:%s"), pPhxFilePath);

	FArchive* file = GFileManager->CreateFileReader(pPhxFilePath);

	if (file == NULL)
	{
		warnf(NAME_Error, TEXT("Faild to load Phx file:%s"), pPhxFilePath);
		goto Exit0;
	}

	//fseek(pFile, 0, SEEK_END);
	int nDataSize = file->TotalSize();// ftell(pFile);
	//fseek(pFile, 0, SEEK_SET);

	pszData = new char[nDataSize];
	if (pszData == NULL)
		goto Exit0;

	file->Serialize(pszData, nDataSize);
	//nRetCode = fread(pszData, 1, nDataSize, pFile);
	//if (nRetCode != nDataSize)
	//	goto Exit0;

	SCENE_PHYX_HEADER* pHeader = (SCENE_PHYX_HEADER*)pszData;
	char* pszPointer = pszData + sizeof(*pHeader);

	if (pHeader->dwTag != SCENE_PHYX_FILE_TAG)
		goto Exit0;

	if (pHeader->dwVersion != SCENE_PHYX_FILE_VERSION)
		goto Exit0;

	memset(m_ZoneData, NULL, sizeof(m_ZoneData));

	char* pPointer = pszData + sizeof(SCENE_PHYX_HEADER);
	for (INT32 nZoneIndex = 0; nZoneIndex < (int)pHeader->dwZoneCount; nZoneIndex++)
	{

//#if defined(CLIENT_GRID_EXT)
//		ZONE_DATA* pZoneData = (ZONE_DATA*)pPointer;
//		pPointer += sizeof(ZONE_DATA);// + sizeof(GRID_DATA) * (pZoneData->nGridCount + 1);
//
//		m_ZoneData[pZoneData->usZoneX][pZoneData->usZoneY] = 
//			(ZONE_DATA*)new char[sizeof(ZONE_DATA) + sizeof(GRID_DATA_EXT) * (pZoneData->nGridCount + 1)];
//
//		GRID_DATA_EXT* pGrid = (GRID_DATA_EXT*)(m_ZoneData[pZoneData->usZoneX][pZoneData->usZoneY] + 1);
//
//		memcpy(m_ZoneData[pZoneData->usZoneX][pZoneData->usZoneY], pZoneData, sizeof(ZONE_DATA));
//
//		for (INT nGridIndex = 0; nGridIndex <= pZoneData->nGridCount; ++nGridIndex)
//		{
//			memcpy(pGrid, pPointer, sizeof(GRID_DATA));
//			pGrid->numActors = 0;
//			memset(pGrid->pActors, 0, sizeof(pGrid->pActors));
//
//			pPointer += sizeof(GRID_DATA);
//			++pGrid;
//		}
//#else

		ZONE_DATA* pZoneData = (ZONE_DATA*)pPointer;
		pPointer += sizeof(ZONE_DATA) + sizeof(GRID_DATA) * (pZoneData->nGridCount + 1);

		m_ZoneData[pZoneData->usZoneX][pZoneData->usZoneY] = 
			(ZONE_DATA*)new char[sizeof(ZONE_DATA) + sizeof(GRID_DATA) * (pZoneData->nGridCount + 1)];
		memcpy(m_ZoneData[pZoneData->usZoneX][pZoneData->usZoneY], pZoneData, 
			sizeof(ZONE_DATA) + sizeof(GRID_DATA) * (pZoneData->nGridCount + 1));
	
//#endif

	}

	//for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	//{
	//	for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
	//	{
	//		ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
	//		if (pZoneData)
	//		{
	//			for (int nGridX = 0; nGridX < GRID_PER_ZONE; nGridX++)
	//			{
	//				for (int nGridY = 0; nGridY < GRID_PER_ZONE; nGridY++)
	//				{
	//					UINT16 usGridIndex = pZoneData->GroundGrid[nGridX][nGridY];
	//					while (usGridIndex)
	//					{
	//						UINT16 usNextGird = pZoneData->GridList[usGridIndex].uNextGrid;
	//						GRID_DATA* pGrid = pZoneData->GridList + usGridIndex;

	//						for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
	//						{
	//							int nAdjGridX = nZoneX * GRID_PER_ZONE + nGridX + g_nAdjacentX[nDirection];
	//							int nAdjGridY = nZoneY * GRID_PER_ZONE + nGridY + g_nAdjacentY[nDirection];

	//							const GRID_DATA* pcAdjGrid = _GetLowerGrid(nAdjGridX * GRID_WIDTH, nAdjGridY * GRID_WIDTH, pGrid->uHighLayer * GRID_LAYER_HEIGHT + GRID_WIDTH);
	//							if (pcAdjGrid == NULL || pcAdjGrid->bOutOfPoly)
	//							{
	//								pGrid->bBorder = 1;
	//								break;
	//							}
	//						}

	//						usGridIndex = usNextGird;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	bResult = true;

Exit0:

	if(file)
	{
		file->Close();
		delete file;
	}

// 	if (pFile)
// 	{
// 		fclose(pFile);
// 		pFile = NULL;
// 	}

	if (pszData)
	{
		delete [] pszData;
		pszData = NULL;
	}

	return bResult;
}

//#endif // !SHIPPING_PC_GAME

#if !SHIPPING_PC_GAME
extern TArray<FDynamicDrawData> g_ErrorGridBlock; // oscar@snda
extern void AddGridBox(float fGridX, float fGridY, float fGridZL, float fGridZH, FLinearColor color);

void CEditorScene::CheckForError()
{
	g_ErrorGridBlock.Empty();

	for (int nZoneX = 0; nZoneX < MAX_ZONE_PER_SCENE; nZoneX++)
	{
		for (int nZoneY = 0; nZoneY < MAX_ZONE_PER_SCENE; nZoneY++)
		{
			ZONE_DATA* pZoneData = m_ZoneData[nZoneX][nZoneY];
			if (pZoneData)
			{
				for (int nGridXInZone = 0; nGridXInZone < GRID_PER_ZONE; nGridXInZone++)
				{
					for (int nGridYInZone = 0; nGridYInZone < GRID_PER_ZONE; nGridYInZone++)
					{
						UINT16 usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];
						while (usGridIndex)
						{
							UINT16 usNextGird = pZoneData->GridList[usGridIndex].uNextGrid;
							GRID_DATA* pGrid = pZoneData->GridList + usGridIndex;

							if (pGrid->bOutOfPoly || pGrid->bBlockMove)
							{
								if (pGrid->uReserved2 == 0)
								{
									int nArea = 1;
									std::list<UINT32> OpenList;
									OpenList.push_back(((nZoneX * GRID_PER_ZONE + nGridXInZone) << 16) | (nZoneY * GRID_PER_ZONE + nGridYInZone));
									for (std::list<UINT32>::iterator it = OpenList.begin(); 
										it != OpenList.end(); )
									{
										int nGridX = *it >> 16;
										int nGridY = *it & 0xFFFF;

										for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT; nDirection++)
										{
											int nAdjGridX = nGridX + g_nAdjacentX[nDirection];
											int nAdjGridY = nGridY + g_nAdjacentY[nDirection];

											const GRID_DATA* pcAdjGrid = _GetLowerGrid(nAdjGridX * GRID_WIDTH, nAdjGridY * GRID_WIDTH, pGrid->uHighLayer * GRID_LAYER_HEIGHT + GRID_WIDTH);
											if (pcAdjGrid && (pcAdjGrid->bOutOfPoly || pcAdjGrid->bBlockMove) && pcAdjGrid->uReserved2 == 0)
											{
												((GRID_DATA*)pcAdjGrid)->uReserved2 = 1;
												OpenList.push_back((nAdjGridX << 16) | nAdjGridY);
												nArea++;
											}
										}

										it = OpenList.erase(it);
									}

									if (nArea <= 4) 
									{
										FVector gridPos;
										gridPos.X = INT64(nZoneX * ZONE_WIDTH + nGridXInZone * GRID_WIDTH) / POINT_PER_CM;
										gridPos.Y = INT64(nZoneY * ZONE_WIDTH + nGridYInZone * GRID_WIDTH) / POINT_PER_CM;
										gridPos.Z =  (pGrid->uHighLayer * GRID_LAYER_HEIGHT) / POINT_PER_CM;

										AddGridBox(gridPos.X, gridPos.Y,
											pGrid->uLowLayer * GRID_LAYER_HEIGHT / POINT_PER_CM, 
											pGrid->uHighLayer * GRID_LAYER_HEIGHT / POINT_PER_CM, FLinearColor(0, 1, 0));

										ConvertXYToClient(gridPos.X, gridPos.Y);
										GWarn->MapCheck_Add(MCTYPE_ERROR, NULL, 
											*FString::Printf(TEXT("Small blocking area: %s area: %d"), *gridPos.ToString(), nArea), *gridPos.ToString(), MCGROUP_MOBILEPLATFORM);
									}
								}
							}
							else
							{
								//for (INT32 nDirection = 0; nDirection < DT_DIRECTION_8_COUNT / 2; nDirection++)
								//{
								//	int nAdjGridX = nZoneX * GRID_PER_ZONE + nGridXInZone;
								//	int nAdjGridY = nZoneY * GRID_PER_ZONE + nGridYInZone;
								//	int len = 1;
								//	for (INT32 nIndex = 0; nIndex < 3; nIndex++)
								//	{
								//		nAdjGridX += g_nAdjacentX[nDirection];
								//		nAdjGridY += g_nAdjacentY[nDirection];

								//		const GRID_DATA* pcAdjGrid = _GetLowerGrid(nAdjGridX * GRID_WIDTH, nAdjGridY * GRID_WIDTH, pGrid->uHighLayer * GRID_LAYER_HEIGHT + GRID_WIDTH);
								//		if (pcAdjGrid  == NULL || pcAdjGrid->bOutOfPoly || pcAdjGrid->bBlockMove)
								//			break;
								//		len++;
								//	}

								//	nAdjGridX = nZoneX * GRID_PER_ZONE + nGridXInZone;
								//	nAdjGridY = nZoneY * GRID_PER_ZONE + nGridYInZone;
								//	for (INT32 nIndex = 0; nIndex < 3; nIndex++)
								//	{
								//		nAdjGridX -= g_nAdjacentX[nDirection];
								//		nAdjGridY -= g_nAdjacentY[nDirection];

								//		const GRID_DATA* pcAdjGrid = _GetLowerGrid(nAdjGridX * GRID_WIDTH, nAdjGridY * GRID_WIDTH, pGrid->uHighLayer * GRID_LAYER_HEIGHT + GRID_WIDTH);
								//		if (pcAdjGrid  == NULL || pcAdjGrid->bOutOfPoly || pcAdjGrid->bBlockMove)
								//			break;
								//		len++;
								//	}

								//	if (len < 2)
								//	{
								//		FVector gridPos;
								//		gridPos.X = INT64(nZoneX * ZONE_WIDTH + nGridXInZone * GRID_WIDTH) / POINT_PER_CM;
								//		gridPos.Y = INT64(nZoneY * ZONE_WIDTH + nGridYInZone * GRID_WIDTH) / POINT_PER_CM;
								//		gridPos.Z =  (pGrid->uHighLayer * GRID_LAYER_HEIGHT) / POINT_PER_CM;

								//		AddGridBox(gridPos.X, gridPos.Y,
								//			pGrid->uLowLayer * GRID_LAYER_HEIGHT / POINT_PER_CM, 
								//			pGrid->uHighLayer * GRID_LAYER_HEIGHT / POINT_PER_CM, FLinearColor(0, 0, 1));

								//		ConvertXYToClient(gridPos.X, gridPos.Y);
								//		GWarn->MapCheck_Add(MCTYPE_ERROR, NULL, 
								//			*FString::Printf(TEXT("Narrow passage:%s Length:%d"), *gridPos.ToString(), len), *gridPos.ToString(), MCGROUP_MOBILEPLATFORM);

								//		break;
								//	}
								//}
							}

							usGridIndex = usNextGird;
						}
					}
				}
			}
		}
	}
}
#endif