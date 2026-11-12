#include "stdafx.h"
#include "ExportServerData.h"
#include "SourceControl.h"
#include "Exporter_Kismet.h"
#include "Exporter_NPCSpawnGroup.h"
#include "Exporter_TriggerVolume.h"
#include "Exporter_SingleNPCSpawnPoint.h"
#include "Exporter_TagNode.h"
#include "Exporter_ChessNode.h"
#include "Exporter_DoodadSpawn.h"

#include "Exporter_Teleports.h"
#include "Exporter_NPCInstanceInfos.h"
#include "Exporter_NPCNewSpawnGroup.h"
#include "Exporter_SearchPoint.h"
#include "Exporter_MapMapSizeVolume.h"
#include "Exporter_NaviPoint.h"
#include "Exporter_TriggerVolume.h"

#if !SHIPPING_PC_GAME

//------------------------------------------------------------------------
// ExportBinaryServerScene
//------------------------------------------------------------------------
void ExportBinaryServerScene()
{
#if WITH_EDITOR
	GServerSceneExporterList.Empty();

	//
	// Register all exporters (note that the order affects the order of exported data)
	//
	//------------------------------------------------------------------------
	//REG_SERVER_EXPORTER(Exporter_TriggerVolume);
	//REG_SERVER_EXPORTER(Exporter_NPCSpawnGroup);
	//REG_SERVER_EXPORTER(Exporter_Kismet);
    //REG_SERVER_EXPORTER(Exporter_ChessNode);
    REG_SERVER_EXPORTER(Exporter_SingleNPCSpawnPoint);
    REG_SERVER_EXPORTER(Exporter_NPCNewSpawnGroup);
	REG_SERVER_EXPORTER(Exporter_DoodadSpawn);
    REG_SERVER_EXPORTER(Exporter_TagNode);
	//------------------------------------------------------------------------

	//
	// Register all extension exporters (handle global data export that needs secondary processing)
	//
	//------------------------------------------------------------------------
	REG_GLOBAL_EXPORTEREX(Exporter_Teleports);
	REG_GLOBAL_EXPORTEREX(Exporter_SearchPoint);
	REG_GLOBAL_EXPORTEREX(Exporter_NPCInstanceInfos);
	REG_GLOBAL_EXPORTEREX(Exporter_MapMapSizeVolume);
	REG_GLOBAL_EXPORTEREX(Exporter_NaviPoint);
	REG_GLOBAL_EXPORTEREX(Exporter_TriggerVolume);
	//------------------------------------------------------------------------
	
	INT GlobalAutoID = 0;
	for (INT i = 0; i < GGlobalDataExporterList.Num(); ++i)
	{
		GGlobalDataExporterList(i)->ProcessData(GlobalAutoID);
	}

	GlobalAutoID = 0;
	// Preprocess once first
	for (INT i = 0; i < GServerSceneExporterList.Num(); ++i)
	{
		GServerSceneExporterList(i)->PrePorcess(GlobalAutoID);
	}

	// Then export
	FString ExportDir;
	GConfig->GetString(TEXT("Configuration"), TEXT("ServerSceneDataExport"), ExportDir, GEditorIni);
	FString PathName = ExportDir + GWorld->GetWorldInfo()->GetMapName() + TEXT("\\");
	::_wmkdir(*PathName);
	FString fileName = PathName + GWorld->GetWorldInfo()->GetMapName() + TEXT(".scn");

	FBufferArchive    memAr;
	//Allocate enough space first, to prevent the address from changing on a later ReAllocate, which would invalidate modifications to SCENE_FILE_BLOCK_HEADER
	//@In case the scene file has problems, look here first hemu@snda 120508
	memAr.Add(  1024*1024*5  );
	//Write SCENE_FILE_HEADER header
	SCENE_FILE_HEADER fileHeader = {0};
	fileHeader.dwTag = SCENE_FILE_TAG;
	fileHeader.dwVersion = SCENE_FILE_VERSION;
	//Note: GServerSceneExporterList.Num() != BlockCount
	fileHeader.nBlockCount  = 0;
	for (INT i = 0; i < GServerSceneExporterList.Num(); ++i)
	{
		fileHeader.nBlockCount += GServerSceneExporterList(i)->GetBlockNumber( );
	}
	memAr.ByteOrderSerialize(&fileHeader, sizeof(SCENE_FILE_HEADER));

	//Write each BlockHead under the Exporter
	INT32	nTotalOffset = 0;
	for (INT i = 0; i < GServerSceneExporterList.Num(); ++i)
	{
		//Pass in, read out the Offset of each blkHdrs
		GServerSceneExporterList(i)->WriteHeader(memAr);
	}
	//Write the actual Data
	for (INT i = 0; i < GServerSceneExporterList.Num(); ++i)
	{
		GServerSceneExporterList(i)->WriteData(memAr);
	}

	// Before creating the file, first try a P4 Checkout to determine whether this file already exists
#if HAVE_SCC
	FCheckoutResult sccResult = FSourceControl::TryCheckOut(fileName);
#endif

	FArchive* ar = GFileManager->CreateFileWriter(*fileName);
	if(ar)
	{
		ar->ByteOrderSerialize(memAr.GetData(), memAr.Tell());
		ar->Flush();
		ar->Close();
	}
	else
	{
		warnf( TEXT("ExportBinaryServerScene failed to create file:%s"), *fileName);
	}

#if HAVE_SCC
	sccResult.TryAddToDefaultChangelist();
#endif

	GWorld->MarkPackageDirty();

#endif
}



// james@snda
//#if !SHIPPING_PC_GAME
/************************************************************************/
// Gen Grid
#include "grid_def.h"
#include "zone_def.h"
#include "scene_def.h"
#include "editor_scene.h"
#include "UnTerrain.h"
#include "EngineTerrainClasses.h"
#include "LandscapeDataAccess.h"

// triBoxOverlap begin
#define X 0
#define Y 1
#define Z 2

#define CROSS(dest,v1,v2) \
	dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
	dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
	dest[2]=v1[0]*v2[1]-v1[1]*v2[0]; 

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
	dest[0]=v1[0]-v2[0]; \
	dest[1]=v1[1]-v2[1]; \
	dest[2]=v1[2]-v2[2]; 

#define FINDMINMAX(x0,x1,x2,min,max) \
	min = max = x0;   \
	if(x1<min) min=x1;\
	if(x1>max) max=x1;\
	if(x2<min) min=x2;\
	if(x2>max) max=x2;

int planeBoxOverlap(float normal[3],float d, float maxbox[3])
{
	int q;
	float vmin[3],vmax[3];
	for(q=X;q<=Z;q++)
	{
		if(normal[q]>0.0f)
		{
			vmin[q]=-maxbox[q];
			vmax[q]=maxbox[q];
		}
		else
		{
			vmin[q]=maxbox[q];
			vmax[q]=-maxbox[q];
		}
	}
	if(DOT(normal,vmin)+d>0.0f) return 0;
	if(DOT(normal,vmax)+d>=0.0f) return 1;

	return 0;
}


/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
	p0 = a*v0[Y] - b*v0[Z];			       	   \
	p2 = a*v2[Y] - b*v2[Z];			       	   \
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
	p0 = a*v0[Y] - b*v0[Z];			           \
	p1 = a*v1[Y] - b*v1[Z];			       	   \
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)			   \
	p0 = -a*v0[X] + b*v0[Z];		      	   \
	p2 = -a*v2[X] + b*v2[Z];	       	       	   \
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)			   \
	p0 = -a*v0[X] + b*v0[Z];		      	   \
	p1 = -a*v1[X] + b*v1[Z];	     	       	   \
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)			   \
	p1 = a*v1[X] - b*v1[Y];			           \
	p2 = a*v2[X] - b*v2[Y];			       	   \
	if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)			   \
	p0 = a*v0[X] - b*v0[Y];				   \
	p1 = a*v1[X] - b*v1[Y];			           \
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
	if(min>rad || max<-rad) return 0;

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3])
{

	/*    use separating axis theorem to test overlap between triangle and box */
	/*    need to test for overlap in these directions: */
	/*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
	/*       we do not even need to test these) */
	/*    2) normal of the triangle */
	/*    3) crossproduct(edge from tri, {x,y,z}-directin) */
	/*       this gives 3x3=9 more tests */
	float v0[3],v1[3],v2[3];
	//	float axis[3];
	float min,max,d,p0,p1,p2,rad,fex,fey,fez;  
	float normal[3],e0[3],e1[3],e2[3];

	/* This is the fastest branch on Sun */
	/* move everything so that the boxcenter is in (0,0,0) */
	SUB(v0,triverts[0],boxcenter);
	SUB(v1,triverts[1],boxcenter);
	SUB(v2,triverts[2],boxcenter);

	/* compute triangle edges */
	SUB(e0,v1,v0);      /* tri edge 0 */
	SUB(e1,v2,v1);      /* tri edge 1 */
	SUB(e2,v0,v2);      /* tri edge 2 */

	/* Bullet 3:  */
	/*  test the 9 tests first (this was faster) */
	fex = fabs(e0[X]);
	fey = fabs(e0[Y]);
	fez = fabs(e0[Z]);
	AXISTEST_X01(e0[Z], e0[Y], fez, fey);
	AXISTEST_Y02(e0[Z], e0[X], fez, fex);
	AXISTEST_Z12(e0[Y], e0[X], fey, fex);

	fex = fabs(e1[X]);
	fey = fabs(e1[Y]);
	fez = fabs(e1[Z]);
	AXISTEST_X01(e1[Z], e1[Y], fez, fey);
	AXISTEST_Y02(e1[Z], e1[X], fez, fex);
	AXISTEST_Z0(e1[Y], e1[X], fey, fex);

	fex = fabs(e2[X]);
	fey = fabs(e2[Y]);
	fez = fabs(e2[Z]);
	AXISTEST_X2(e2[Z], e2[Y], fez, fey);
	AXISTEST_Y1(e2[Z], e2[X], fez, fex);
	AXISTEST_Z12(e2[Y], e2[X], fey, fex);

	/* Bullet 1: */
	/*  first test overlap in the {x,y,z}-directions */
	/*  find min, max of the triangle each direction, and test for overlap in */
	/*  that direction -- this is equivalent to testing a minimal AABB around */
	/*  the triangle against the AABB */

	/* test in X-direction */
	FINDMINMAX(v0[X],v1[X],v2[X],min,max);
	if(min>boxhalfsize[X] || max<-boxhalfsize[X]) return 0;

	/* test in Y-direction */
	FINDMINMAX(v0[Y],v1[Y],v2[Y],min,max);
	if(min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return 0;

	/* test in Z-direction */
	FINDMINMAX(v0[Z],v1[Z],v2[Z],min,max);
	if(min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return 0;

	/* Bullet 2: */
	/*  test if the box intersects the plane of the triangle */
	/*  compute plane equation of triangle: normal*x+d=0 */
	CROSS(normal,e0,e1);
	d=-DOT(normal,v0);  /* plane eq: normal.x+d=0 */
	if(!planeBoxOverlap(normal,d,boxhalfsize)) return 0;

	return 1;   /* box and triangle overlaps */
}

#undef X
#undef Y
#undef Z
// triBoxOverlap end

struct PROCESS_TRIANGLE
{
	float			m_fTriangle[3][3];
	CEditorScene*	m_pScene;
	AActor*			m_Actor;

	BOOL TryOctree(float fWorldX, float fWorldY, float fWorldZ,
		int nCellLenX, int nCellLenY, int nCellLenZ)
	{
		BOOL bRetCode = FALSE;

		if (nCellLenX == 0)  nCellLenX = 1;
		if (nCellLenY == 0)  nCellLenY = 1;
		if (nCellLenZ == 0)  nCellLenZ = 1;

		float center[3] = {
			fWorldX + nCellLenX * GRID_WIDTH * 0.5f / POINT_PER_CM - 0.5f,
			fWorldY + nCellLenY * GRID_WIDTH * 0.5f / POINT_PER_CM - 0.5f,
			fWorldZ + nCellLenZ * GRID_LAYER_HEIGHT * 0.5f / POINT_PER_CM - 0.5f,
		};

		int nLogicX = center[0] * POINT_PER_CM;
		int nLogicY = center[1] * POINT_PER_CM;
		int nLogicZ = center[2] * POINT_PER_CM;

		int ret = 0;
		// check whether a grid has been marked as obstacle.
		if (nCellLenX == 1 && nCellLenY == 1 && nCellLenZ == 1)
		{
			GRID_DATA* pGridData = m_pScene->GetGrid(nLogicX, nLogicY, nLogicZ);
			if (pGridData)
				return TRUE;
		}

		float boxhalfsize[3] = {    
			nCellLenX * GRID_WIDTH * 0.5f / POINT_PER_CM - 8,
			nCellLenY * GRID_WIDTH * 0.5f / POINT_PER_CM - 8,
			nCellLenZ * GRID_LAYER_HEIGHT * 0.5f / POINT_PER_CM,
		};

		if (ret == 0)
		{
			ret = triBoxOverlap(center, boxhalfsize, m_fTriangle);
		}
		if (ret == 0)
			return TRUE;

		if (nCellLenX == 1 && nCellLenY == 1 && nCellLenZ == 1)
		{
			if (nLogicZ < 0)
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, m_Actor, *FString::Printf(TEXT("Actor is below zero (%.2f, %.2f, %.2f)"), 
					center[0], center[1], center[2]), NULL);
				return FALSE;
			}

			GRID_DATA* pGridData = m_pScene->CreateGrid(nLogicX, nLogicY, nLogicZ, nLogicZ + GRID_LAYER_HEIGHT);
			if (pGridData == NULL)
			{
				GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, m_Actor, *FString::Printf(TEXT("There are too many grids around actor (%.2f, %.2f, %.2f)"), 
					center[0], center[1], center[2]), NULL);
				return FALSE;
			}

			return TRUE;
		}

		bRetCode = TryOctree(
			fWorldX, 
			fWorldY, 
			fWorldZ, 
			nCellLenX / 2, nCellLenY / 2, nCellLenZ / 2);
		if (!bRetCode) 
			return FALSE;

		if (nCellLenZ > 1)
		{
			bRetCode = TryOctree(
				fWorldX, 
				fWorldY, 
				fWorldZ + GRID_LAYER_HEIGHT * (nCellLenZ / 2) / POINT_PER_CM,
				nCellLenX / 2, nCellLenY / 2, nCellLenZ - nCellLenZ / 2);
			if (!bRetCode) 
				return FALSE;
		}

		if (nCellLenX > 1)
		{
			bRetCode = TryOctree(
				fWorldX + GRID_WIDTH * (nCellLenX / 2) / POINT_PER_CM, 
				fWorldY, 
				fWorldZ,
				nCellLenX - nCellLenX / 2, nCellLenY / 2, nCellLenZ / 2);
			if (!bRetCode) 
				return FALSE;
		}
		if (nCellLenX > 1 && nCellLenZ > 1)
		{
			bRetCode = TryOctree(
				fWorldX + GRID_WIDTH * (nCellLenX / 2) / POINT_PER_CM, 
				fWorldY, 
				fWorldZ + GRID_LAYER_HEIGHT * (nCellLenZ / 2) / POINT_PER_CM, 
				nCellLenX - nCellLenX / 2, nCellLenY / 2, nCellLenZ - nCellLenZ / 2);
			if (!bRetCode) 
				return FALSE;
		}

		if (nCellLenY > 1)
		{
			bRetCode = TryOctree(
				fWorldX, 
				fWorldY + GRID_WIDTH * (nCellLenY / 2) / POINT_PER_CM, 
				fWorldZ,
				nCellLenX / 2, nCellLenY - nCellLenY / 2, nCellLenZ / 2);
			if (!bRetCode) 
				return FALSE;
		}

		if (nCellLenY > 1 && nCellLenZ > 1)
		{
			bRetCode = TryOctree(
				fWorldX, 
				fWorldY + GRID_WIDTH * (nCellLenY / 2) / POINT_PER_CM, 
				fWorldZ + GRID_LAYER_HEIGHT * (nCellLenZ / 2) / POINT_PER_CM,
				nCellLenX / 2, nCellLenY - nCellLenY / 2, nCellLenZ - nCellLenZ / 2);
			if (!bRetCode) 
				return FALSE;
		}

		if (nCellLenX > 1 && nCellLenY > 1)
		{
			bRetCode = TryOctree(
				fWorldX + GRID_WIDTH * (nCellLenX / 2) / POINT_PER_CM, 
				fWorldY + GRID_WIDTH * (nCellLenY / 2) / POINT_PER_CM, 
				fWorldZ,
				nCellLenX - nCellLenX / 2, nCellLenY - nCellLenY / 2, nCellLenZ / 2);
			if (!bRetCode) 
				return FALSE;
		}

		bRetCode = TryOctree(
			fWorldX + GRID_WIDTH * (nCellLenX / 2) / POINT_PER_CM, 
			fWorldY + GRID_WIDTH * (nCellLenY / 2) / POINT_PER_CM, 
			fWorldZ + GRID_LAYER_HEIGHT * (nCellLenZ / 2) / POINT_PER_CM,
			nCellLenX - nCellLenX / 2, nCellLenY - nCellLenY / 2, nCellLenZ - nCellLenZ / 2);
		if (!bRetCode) 
			return FALSE;

		return TRUE;
	}
};

#define MIN_THREE(a, b, c)	(((a) < (b) ? (a) : (b)) < (c) ? ((a) < (b) ? (a) : (b)) : (c))
#define MAX_THREE(a, b, c)	(((a) > (b) ? (a) : (b)) > (c) ? ((a) > (b) ? (a) : (b)) : (c))
TArray<DRAW_GRID_DATA>	g_GridVerts;
TArray<FDynamicDrawData> g_ErrorGridBlock; // oscar@snda

CEditorScene g_Scene;
CEditorScene g_ReviseScene(TEXT("EditorGridDataExport"));
bool g_bShowGrid =  false;
int g_nCenterZoneX = 1;
int g_nCenterZoneY = 1;
int g_nRange = 1;


void GetgridCentrPos( FVector2D &grid, FVector2D &centr, float &size )
{
	static float scalef = 0.7071068f;
	centr = ( grid * GRID_WIDTH + FVector2D( 1, 1 ) * GRID_WIDTH * 0.5f ) / POINT_PER_CM;
	ConvertXYToClient( centr.X, centr.Y );
	//centr = centr + FVector2D( 64.f/scalef, 64.f/scalef );
	size = 64.f;
}

BOOL TestOctree( float (&fTriangle)[3][3],float fWorldX, float fWorldY, float fWorldZ,
	int nCellLenX, int nCellLenY, int nCellLenZ, TArray<FVector2D> &gridSet)
{
	BOOL bRetCode = FALSE;

	if (nCellLenX == 0)  nCellLenX = 1;
	if (nCellLenY == 0)  nCellLenY = 1;
	if (nCellLenZ == 0)  nCellLenZ = 1;

	float center[3] = {
		fWorldX + nCellLenX * GRID_WIDTH * 0.5f / POINT_PER_CM - 0.5f,
		fWorldY + nCellLenY * GRID_WIDTH * 0.5f / POINT_PER_CM - 0.5f,
		fWorldZ + nCellLenZ * GRID_LAYER_HEIGHT * 0.5f / POINT_PER_CM - 0.5f,
	};

	int nLogicX = center[0] * POINT_PER_CM;
	int nLogicY = center[1] * POINT_PER_CM;
	int nLogicZ = center[2] * POINT_PER_CM;

	int ret = 0;


	float boxhalfsize[3] = {    
		nCellLenX * GRID_WIDTH * 0.5f / POINT_PER_CM - 8,
		nCellLenY * GRID_WIDTH * 0.5f / POINT_PER_CM - 8,
		nCellLenZ * GRID_LAYER_HEIGHT * 0.5f / POINT_PER_CM,
	};

	if (ret == 0)
	{
		ret = triBoxOverlap(center, boxhalfsize, fTriangle);
	}
	if (ret == 0)
		return TRUE;

	if (nCellLenX == 1 && nCellLenY == 1 && nCellLenZ == 1)
	{
		if (nLogicZ < 0)
		{
			//GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, m_Actor, *FString::Printf(TEXT("Actor is below zero (%.2f, %.2f, %.2f)"), 
				//center[0], center[1], center[2]), NULL);
			return FALSE;
		}

		FVector2D vct( fWorldX*POINT_PER_CM/GRID_WIDTH,  fWorldY*POINT_PER_CM/GRID_WIDTH );;
		int m;
		if ( !gridSet.FindItem( vct, m ) )
			gridSet.AddItem( vct );
		return TRUE;
	}

	bRetCode = TestOctree(fTriangle,
		fWorldX, 
		fWorldY, 
		fWorldZ, 
		nCellLenX / 2, nCellLenY / 2, nCellLenZ / 2, gridSet);
	if (!bRetCode) 
		return FALSE;

	if (nCellLenZ > 1)
	{
		bRetCode = TestOctree(fTriangle,
			fWorldX, 
			fWorldY, 
			fWorldZ + GRID_LAYER_HEIGHT * (nCellLenZ / 2) / POINT_PER_CM,
			nCellLenX / 2, nCellLenY / 2, nCellLenZ - nCellLenZ / 2, gridSet);
		if (!bRetCode) 
			return FALSE;
	}

	if (nCellLenX > 1)
	{
		bRetCode = TestOctree(fTriangle,
			fWorldX + GRID_WIDTH * (nCellLenX / 2) / POINT_PER_CM, 
			fWorldY, 
			fWorldZ,
			nCellLenX - nCellLenX / 2, nCellLenY / 2, nCellLenZ / 2, gridSet);
		if (!bRetCode) 
			return FALSE;
	}
	if (nCellLenX > 1 && nCellLenZ > 1)
	{
		bRetCode = TestOctree(fTriangle,
			fWorldX + GRID_WIDTH * (nCellLenX / 2) / POINT_PER_CM, 
			fWorldY, 
			fWorldZ + GRID_LAYER_HEIGHT * (nCellLenZ / 2) / POINT_PER_CM, 
			nCellLenX - nCellLenX / 2, nCellLenY / 2, nCellLenZ - nCellLenZ / 2, gridSet);
		if (!bRetCode) 
			return FALSE;
	}

	if (nCellLenY > 1)
	{
		bRetCode = TestOctree(fTriangle,
			fWorldX, 
			fWorldY + GRID_WIDTH * (nCellLenY / 2) / POINT_PER_CM, 
			fWorldZ,
			nCellLenX / 2, nCellLenY - nCellLenY / 2, nCellLenZ / 2, gridSet);
		if (!bRetCode) 
			return FALSE;
	}

	if (nCellLenY > 1 && nCellLenZ > 1)
	{
		bRetCode = TestOctree(fTriangle,
			fWorldX, 
			fWorldY + GRID_WIDTH * (nCellLenY / 2) / POINT_PER_CM, 
			fWorldZ + GRID_LAYER_HEIGHT * (nCellLenZ / 2) / POINT_PER_CM,
			nCellLenX / 2, nCellLenY - nCellLenY / 2, nCellLenZ - nCellLenZ / 2, gridSet);
		if (!bRetCode) 
			return FALSE;
	}

	if (nCellLenX > 1 && nCellLenY > 1)
	{
		bRetCode = TestOctree(fTriangle,
			fWorldX + GRID_WIDTH * (nCellLenX / 2) / POINT_PER_CM, 
			fWorldY + GRID_WIDTH * (nCellLenY / 2) / POINT_PER_CM, 
			fWorldZ,
			nCellLenX - nCellLenX / 2, nCellLenY - nCellLenY / 2, nCellLenZ / 2, gridSet);
		if (!bRetCode) 
			return FALSE;
	}

	bRetCode = TestOctree(fTriangle,
		fWorldX + GRID_WIDTH * (nCellLenX / 2) / POINT_PER_CM, 
		fWorldY + GRID_WIDTH * (nCellLenY / 2) / POINT_PER_CM, 
		fWorldZ + GRID_LAYER_HEIGHT * (nCellLenZ / 2) / POINT_PER_CM,
		nCellLenX - nCellLenX / 2, nCellLenY - nCellLenY / 2, nCellLenZ - nCellLenZ / 2, gridSet);
	if (!bRetCode) 
		return FALSE;

	return TRUE;
}
void ProcessCollisionTriangleToGridSet( float (&fTriangle)[3][3], const AActor* Actor, TArray<FVector2D> &gridSet )
{
	float aabb[2][3];
	int grid_aabb[2][3];

	ConvertXYToServer(fTriangle[0][0], fTriangle[0][1]);
	ConvertXYToServer(fTriangle[1][0], fTriangle[1][1]);
	ConvertXYToServer(fTriangle[2][0], fTriangle[2][1]);

	aabb[0][0] = MIN_THREE(fTriangle[0][0], fTriangle[1][0], fTriangle[2][0]);
	aabb[0][1] = MIN_THREE(fTriangle[0][1], fTriangle[1][1], fTriangle[2][1]);
	aabb[0][2] = MIN_THREE(fTriangle[0][2], fTriangle[1][2], fTriangle[2][2]);

	aabb[1][0] = MAX_THREE(fTriangle[0][0], fTriangle[1][0], fTriangle[2][0]);
	aabb[1][1] = MAX_THREE(fTriangle[0][1], fTriangle[1][1], fTriangle[2][1]);
	aabb[1][2] = MAX_THREE(fTriangle[0][2], fTriangle[1][2], fTriangle[2][2]);

	grid_aabb[0][0] = floor(aabb[0][0] * POINT_PER_CM / GRID_WIDTH);
	grid_aabb[0][1] = floor(aabb[0][1] * POINT_PER_CM / GRID_WIDTH);
	grid_aabb[0][2] = floor(aabb[0][2] * POINT_PER_CM / GRID_LAYER_HEIGHT);

	grid_aabb[1][0] = ceil(aabb[1][0] * POINT_PER_CM / GRID_WIDTH) + 1;
	grid_aabb[1][1] = ceil(aabb[1][1] * POINT_PER_CM / GRID_WIDTH) + 1;
	grid_aabb[1][2] = ceil(aabb[1][2] * POINT_PER_CM / GRID_LAYER_HEIGHT) + 1;

	int nGridXLen = Max(1, (int)((grid_aabb[1][0] - grid_aabb[0][0])));
	int nGridYLen = Max(1, (int)((grid_aabb[1][1] - grid_aabb[0][1])));
	int nGridZLen = Max(1, (int)((grid_aabb[1][2] - grid_aabb[0][2])));
	TestOctree( fTriangle, grid_aabb[0][0] * GRID_WIDTH / POINT_PER_CM, 
		grid_aabb[0][1] * GRID_WIDTH / POINT_PER_CM, 
		grid_aabb[0][2] * GRID_LAYER_HEIGHT / POINT_PER_CM, 
		nGridXLen, nGridYLen, nGridZLen,gridSet);

}

BOOL ProcessCollisionTriangle(CEditorScene* pScene, float fTriangle[3][3], AActor* Actor)
{
	PROCESS_TRIANGLE ProcessTriangle;
	float aabb[2][3];
	int grid_aabb[2][3];

	ConvertXYToServer(fTriangle[0][0], fTriangle[0][1]);
	ConvertXYToServer(fTriangle[1][0], fTriangle[1][1]);
	ConvertXYToServer(fTriangle[2][0], fTriangle[2][1]);

	aabb[0][0] = MIN_THREE(fTriangle[0][0], fTriangle[1][0], fTriangle[2][0]);
	aabb[0][1] = MIN_THREE(fTriangle[0][1], fTriangle[1][1], fTriangle[2][1]);
	aabb[0][2] = MIN_THREE(fTriangle[0][2], fTriangle[1][2], fTriangle[2][2]);

	aabb[1][0] = MAX_THREE(fTriangle[0][0], fTriangle[1][0], fTriangle[2][0]);
	aabb[1][1] = MAX_THREE(fTriangle[0][1], fTriangle[1][1], fTriangle[2][1]);
	aabb[1][2] = MAX_THREE(fTriangle[0][2], fTriangle[1][2], fTriangle[2][2]);

	grid_aabb[0][0] = floor(aabb[0][0] * POINT_PER_CM / GRID_WIDTH);
	grid_aabb[0][1] = floor(aabb[0][1] * POINT_PER_CM / GRID_WIDTH);
	grid_aabb[0][2] = floor(aabb[0][2] * POINT_PER_CM / GRID_LAYER_HEIGHT);

	grid_aabb[1][0] = ceil(aabb[1][0] * POINT_PER_CM / GRID_WIDTH) + 1;
	grid_aabb[1][1] = ceil(aabb[1][1] * POINT_PER_CM / GRID_WIDTH) + 1;
	grid_aabb[1][2] = ceil(aabb[1][2] * POINT_PER_CM / GRID_LAYER_HEIGHT) + 1;

	int nGridXLen = Max(1, (int)((grid_aabb[1][0] - grid_aabb[0][0])));
	int nGridYLen = Max(1, (int)((grid_aabb[1][1] - grid_aabb[0][1])));
	int nGridZLen = Max(1, (int)((grid_aabb[1][2] - grid_aabb[0][2])));

	ProcessTriangle.m_pScene = pScene;
	ProcessTriangle.m_Actor = Actor;
	memcpy(ProcessTriangle.m_fTriangle, fTriangle, sizeof(ProcessTriangle.m_fTriangle));

	return ProcessTriangle.TryOctree(grid_aabb[0][0] * GRID_WIDTH / POINT_PER_CM, 
		grid_aabb[0][1] * GRID_WIDTH / POINT_PER_CM, 
		grid_aabb[0][2] * GRID_LAYER_HEIGHT / POINT_PER_CM, 
		nGridXLen, nGridYLen, nGridZLen);
}

BOOL ExportLandscape(CEditorScene* pScene, ULandscapeHeightfieldCollisionComponent* CollisionComp, ULandscapeComponent* Component, ALandscape* landscape)
{
#if WITH_EDITOR
	BOOL bResult = FALSE;
	INT32 nRetCode = 0;

	//FLandscapeComponentDataInterface landscapData(Component);

	ULandscapeInfo* Info = Component->GetLandscapeInfo();
	TArray<FName> CandidateLayers;
	TArray<BYTE*> CandidateDataPtrs;
	// Channel remapping
//#if PACK_WEIGHTMAPS
	INT TexOffset = 0;
	if( Component->WeightmapTextures.Num() > 0 )
		TexOffset = Square(Component->WeightmapTextures(0)->SizeX) * 4;
	INT ChannelOffsets[8] = {STRUCT_OFFSET(FColor,R),STRUCT_OFFSET(FColor,G),STRUCT_OFFSET(FColor,B),STRUCT_OFFSET(FColor,A),
							TexOffset + STRUCT_OFFSET(FColor,R),TexOffset + STRUCT_OFFSET(FColor,G),TexOffset + STRUCT_OFFSET(FColor,B),TexOffset + STRUCT_OFFSET(FColor,A) };
	//INT ChannelOffsets[4] = {STRUCT_OFFSET(FColor,R),STRUCT_OFFSET(FColor,G),STRUCT_OFFSET(FColor,B),STRUCT_OFFSET(FColor,A)};

	UBOOL bExistingLayerMismatch = FALSE;
	INT DataLayerIdx = INDEX_NONE;

	TArray<FColor*> WeightmapTextureMipData;
	for( INT Idx = 0; Idx < Component->WeightmapTextures.Num(); Idx++ )
	{
		WeightmapTextureMipData.AddItem(
			(FColor*)Component->WeightmapTextures(Idx)->Mips(Component->CollisionMipLevel).Data.Lock(LOCK_READ_ONLY) );

		//Component->WeightmapTextures(Idx)->Mips(Component->CollisionMipLevel).Data.Unlock();
	}



	// Find the layers we're interested in
	for( INT AllocIdx = 0; AllocIdx < Component->WeightmapLayerAllocations.Num(); AllocIdx++ )
	{
		const FWeightmapLayerAllocationInfo& AllocInfo = Component->WeightmapLayerAllocations(AllocIdx);
		const FLandscapeLayerStruct* LayerInfo = Info->LayerInfoMap.FindRef(AllocInfo.LayerName);
		if( AllocInfo.LayerName == ALandscape::DataWeightmapName || (LayerInfo && LayerInfo->LayerInfoObj && LayerInfo->LayerInfoObj->PhysMaterial) )
		{
			INT Idx = CandidateLayers.AddItem(AllocInfo.LayerName);
			CandidateDataPtrs.AddItem(((BYTE*)WeightmapTextureMipData(AllocInfo.WeightmapTextureIndex)) + ChannelOffsets[AllocInfo.WeightmapTextureChannel]);

			// Check if we still match the collision component.
			if( Idx >= CollisionComp->ComponentLayers.Num() || CollisionComp->ComponentLayers(Idx) != AllocInfo.LayerName )
			{
				bExistingLayerMismatch = TRUE;
			}

			if (AllocInfo.LayerName == ALandscape::DataWeightmapName)
			{
				DataLayerIdx = Idx;
				//CollisionComponent->bIncludeHoles = TRUE;
				bExistingLayerMismatch = TRUE; // always rebuild whole component for hole
			}
		}
	}

	INT CollisionSubsectionSizeVerts = ((Component->SubsectionSizeQuads+1)>>Component->CollisionMipLevel);
	INT CollisionSubsectionSizeQuads = CollisionSubsectionSizeVerts-1;
	INT CollisionSizeVerts = Component->NumSubsections * CollisionSubsectionSizeQuads+1;
//	INT MipSizeU = (Component->WeightmapTextures(0)->SizeX) >> Component->CollisionMipLevel;

	WORD* Heights = (WORD*)CollisionComp->CollisionHeightData.Lock(LOCK_READ_ONLY);
	BYTE* DominantLayers = CollisionComp->DominantLayerData.GetElementCount() > 0 ? (BYTE*)CollisionComp->DominantLayerData.Lock(LOCK_READ_ONLY) : NULL;
	INT NumHeights = Square(CollisionComp->CollisionSizeQuads+1);
	check(CollisionComp->CollisionHeightData.GetElementCount()==NumHeights);
	FVector* pVecList = new FVector[NumHeights];
	float MinHeight = 9999999;

	for( INT Y=0;Y<=CollisionComp->CollisionSizeQuads;Y++ )
	{
		for( INT X=0;X<=CollisionComp->CollisionSizeQuads;X++ )
		{
			pVecList[X + Y * (CollisionComp->CollisionSizeQuads+1)] = 
				CollisionComp->LocalToWorld.TransformFVector(FVector(X, Y, ((FLOAT)Heights[X + Y * (CollisionComp->CollisionSizeQuads+1)] - 32768.f) * LANDSCAPE_ZSCALE));

			//pVecList[X + Y * (CollisionComp->CollisionSizeQuads+1)] = landscapData.GetWorldVertex(X, Y);

			if (MinHeight > pVecList[X + Y * (CollisionComp->CollisionSizeQuads+1)].Z)
				MinHeight = pVecList[X + Y * (CollisionComp->CollisionSizeQuads+1)].Z;
		}
	}

	CollisionComp->CollisionHeightData.Unlock();

	if (MinHeight < 0)
	{
		GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Landscape min height is below zero : %.2f"), MinHeight), NULL);
		MinHeight = 0;
	}

	int nLowZ = MinHeight * POINT_PER_CM;
	if (nLowZ > 0)
		nLowZ = nLowZ - GRID_LAYER_HEIGHT;

	for( INT Y=0;Y<CollisionComp->CollisionSizeQuads;Y++ )
	{
		for( INT X=0;X<CollisionComp->CollisionSizeQuads;X++ )
		{
			float H[3][3];

#if defined(_DEBUG) && 0
			BYTE val = DominantLayers[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)];
			debugf(TEXT("%x\t"), val);
#endif

			if (DominantLayers)
			{
				BYTE val = DominantLayers[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)];

				if(DataLayerIdx == INDEX_NONE)
				{
					if(val == 0)
					{

					}
				}
				else if(val == 255)
				{
					// Not empty
				}				
				//else if(val == 0) // Directly a hole, without physical material
				//{
				//	continue;
				//}
				else if(val == DataLayerIdx) // Hole when using physical material
				{
#ifdef _DEBUG
					//INT TexX = CollisionSubsectionSizeVerts * 0 + X;
					//INT TexY = CollisionSubsectionSizeVerts * 0 + Y;
					//INT DataOffset = (TexX + TexY * MipSizeU) * sizeof(FColor);

					//BYTE LayerWeight = CandidateDataPtrs(val)[DataOffset];
					//if (LayerWeight > 170) // 255 * 0.66
					//{
					//	continue;
					//}
					//else
					//{
					//	continue;
					//}
					continue;
#else
					continue;
#endif
				}
			}


			H[0][0] = pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].X;
			H[0][1] = pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Y;
			H[0][2] = pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Z;

			H[1][0] = pVecList[(X+1) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].X;
			H[1][1] = pVecList[(X+1) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Y;
			H[1][2] = pVecList[(X+1) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Z;

			H[2][0] = pVecList[(X+0) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].X;
			H[2][1] = pVecList[(X+0) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Y;
			H[2][2] = pVecList[(X+0) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Z;

			nRetCode = ProcessCollisionTriangle(pScene, H, landscape);
			if (!nRetCode)
				goto Exit0;

			H[0][0] = pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].X;
			H[0][1] = pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Y;
			H[0][2] = pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Z;

			H[1][0] = pVecList[(X+1) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].X;
			H[1][1] = pVecList[(X+1) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Y;
			H[1][2] = pVecList[(X+1) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Z;

			H[2][0] = pVecList[(X+1) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].X;
			H[2][1] = pVecList[(X+1) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Y;
			H[2][2] = pVecList[(X+1) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Z;

			nRetCode = ProcessCollisionTriangle(pScene, H, landscape);
			if (!nRetCode)
				goto Exit0;

			//H[0][0] = pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Z;
			//H[0][2] = pVecList[(X+0) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Z;
			//H[2][0] = pVecList[(X+1) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Z;
			//H[2][2] = pVecList[(X+1) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Z;

			//H[0][1] = (H[0][0] + H[0][2]) / 2;
			//H[1][0] = (H[2][0] + H[0][0]) / 2;
			//H[1][2] = (H[0][2] + H[2][2]) / 2;
			//H[2][1] = (H[2][0] + H[2][2]) / 2;
			//H[1][1] = (H[0][0] + H[2][2]) / 2;

			////H[0][0] = pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Z;
			////H[0][4] = pVecList[(X+0) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Z;
			////H[4][0] = pVecList[(X+1) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Z;
			////H[4][4] = pVecList[(X+1) + (Y+1) * (CollisionComp->CollisionSizeQuads+1)].Z;

			////H[0][2] = (H[0][0] + H[0][4]) / 2;
			////H[0][1] = (H[0][0] + H[0][2]) / 2;
			////H[0][3] = (H[0][2] + H[0][4]) / 2;

			////H[2][0] = (H[0][0] + H[4][0]) / 2;
			////H[1][0] = (H[0][0] + H[2][0]) / 2;
			////H[3][0] = (H[2][0] + H[4][0]) / 2;

			////H[2][4] = (H[0][4] + H[4][4]) / 2;
			////H[1][4] = (H[0][4] + H[2][4]) / 2;
			////H[3][4] = (H[2][4] + H[4][4]) / 2;

			////H[4][2] = (H[4][0] + H[4][4]) / 2;
			////H[4][1] = (H[4][0] + H[4][2]) / 2;
			////H[4][3] = (H[4][2] + H[4][4]) / 2;

			////H[2][2] = (H[0][0] + H[4][4]) / 2;
			////H[1][1] = (H[0][0] + H[2][2]) / 2;
			////H[3][3] = (H[2][2] + H[4][4]) / 2;

			////H[1][2] = (H[0][2] + H[2][2]) / 2;
			////H[1][3] = (H[1][2] + H[1][4]) / 2;

			////H[2][1] = (H[2][0] + H[2][2]) / 2;
			////H[2][3] = (H[2][2] + H[2][4]) / 2;

			////H[3][2] = (H[2][2] + H[4][2]) / 2;
			////H[3][1] = (H[3][0] + H[3][2]) / 2;

			//for (int i = 0; i < 2; i++)
			//{
			//	for (int j = 0; j < 2; j++)
			//	{
			//		float Height = H[i][j] > H[i + 1][j] ? H[i][j] : H[i + 1][j];
			//		Height = Height > H[i][j + 1] ? Height : H[i][j + 1];
			//		Height = Height > H[i + 1][j + 1] ? Height : H[i + 1][j + 1];

			//		int nX = ceil(pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].X + 0.5) * POINT_PER_CM + GRID_WIDTH * i;
			//		int nY = ceil(pVecList[(X+0) + (Y+0) * (CollisionComp->CollisionSizeQuads+1)].Y + 0.5) * POINT_PER_CM + GRID_WIDTH * j;
			//		int nHighZ = Height * POINT_PER_CM;

			//		if (nLowZ >= nHighZ)
			//		{
			//			GWarn->MapCheck_Add( MCTYPE_ERROR, NULL, TEXT("Landscape has vertex below 0!"), NULL );
			//			goto Exit0;
			//		}

			//		GRID_DATA* pGrid = pScene->CreateGrid(nX, nY, nLowZ, nHighZ);
			//		if (pGrid == NULL)
			//		{
			//			GWarn->MapCheck_Add( MCTYPE_WARNING, NULL, TEXT("Create server grid failed!"), NULL );
			//			goto Exit0;
			//		}
			//	}
			//}
			// next_vert:
			// 			;


		}
	}

	bResult = TRUE;
Exit0:

	for( INT Idx = 0; Idx < Component->WeightmapTextures.Num(); Idx++ )
	{
		Component->WeightmapTextures(Idx)->Mips(Component->CollisionMipLevel).Data.Unlock();
	}

	if (DominantLayers)
		CollisionComp->DominantLayerData.Unlock();

	delete[] pVecList;

	return bResult;
#else
	return TRUE;
#endif
}

void GetActorCollisionGridSet( const AActor* Actor, TArray<FVector2D> &gridSet )
{
#if WITH_EDITOR
	int nRetCode = 0;
	float fTriangle[3][3];

	if (Actor == NULL)
		return;


	for (INT ComponentIndex = 0; ComponentIndex < Actor->AllComponents.Num(); ComponentIndex++)
	{
		UPrimitiveComponent* PrimComponent = Cast<UPrimitiveComponent>(Actor->AllComponents(ComponentIndex));
		if (PrimComponent == NULL)
			continue;

		if (PrimComponent && PrimComponent->BlockNonZeroExtent && PrimComponent->BlockActors)
		{
			FMatrix& LocalToWorld = PrimComponent->LocalToWorld;
			const UBOOL bFlipCullMode = LocalToWorld.RotDeterminant() < 0.0f;

			UStaticMeshComponent *SMC = Cast<UStaticMeshComponent>(PrimComponent);
			if ((Actor->bCollideComplex && SMC) ||
				(SMC && SMC->StaticMesh && !SMC->StaticMesh->UseSimpleLineCollision))
			{
				if (SMC->StaticMesh )
				{
					FStaticMeshRenderData* StaticMeshRenderData = SMC->StaticMesh->GetLODForExport(0);

					for (INT iTriangle = 0; iTriangle < SMC->StaticMesh->kDOPTree.Triangles.Num(); iTriangle++)
					{
						const FkDOPCollisionTriangle<WORD>& Triangle = SMC->StaticMesh->kDOPTree.Triangles(iTriangle);

						FVector Pos = LocalToWorld.TransformFVector(StaticMeshRenderData->PositionVertexBuffer.VertexPosition(Triangle.v1));
						fTriangle[0][0] = Pos.X;
						fTriangle[0][1] = Pos.Y;
						fTriangle[0][2] = Pos.Z;

						Pos =  LocalToWorld.TransformFVector(StaticMeshRenderData->PositionVertexBuffer.VertexPosition(Triangle.v2));
						fTriangle[1][0] = Pos.X;
						fTriangle[1][1] = Pos.Y;
						fTriangle[1][2] = Pos.Z;

						Pos =  LocalToWorld.TransformFVector(StaticMeshRenderData->PositionVertexBuffer.VertexPosition(Triangle.v3));
						fTriangle[2][0] = Pos.X;
						fTriangle[2][1] = Pos.Y;
						fTriangle[2][2] = Pos.Z;
						ProcessCollisionTriangleToGridSet( fTriangle, Actor, gridSet );
					}
				}

				continue;
			}

			URB_BodySetup* BodySetup = PrimComponent->GetRBBodySetup();
			if (BodySetup != NULL)
			{
				FKAggregateGeom& AggGeom = BodySetup->AggGeom;
				for (INT iElem = 0; iElem < AggGeom.ConvexElems.Num(); iElem++)
				{
					FKConvexElem& ConEl = AggGeom.ConvexElems(iElem);
					INT NumTris = ConEl.FaceTriData.Num()/3;

					for (INT iTriangle = 0; iTriangle < NumTris; iTriangle++)
					{
						FVector Pos = LocalToWorld.TransformFVector(ConEl.VertexData(ConEl.FaceTriData((iTriangle * 3) + 0)));
						fTriangle[0][0] = Pos.X;
						fTriangle[0][1] = Pos.Y;
						fTriangle[0][2] = Pos.Z;

						Pos = LocalToWorld.TransformFVector(ConEl.VertexData(ConEl.FaceTriData((iTriangle * 3) + 1)));
						fTriangle[1][0] = Pos.X;
						fTriangle[1][1] = Pos.Y;
						fTriangle[1][2] = Pos.Z;

						Pos = LocalToWorld.TransformFVector(ConEl.VertexData(ConEl.FaceTriData((iTriangle * 3) + 2)));
						fTriangle[2][0] = Pos.X;
						fTriangle[2][1] = Pos.Y;
						fTriangle[2][2] = Pos.Z;
						ProcessCollisionTriangleToGridSet( fTriangle, Actor, gridSet );
					}
				}
				continue;
			}

			USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(PrimComponent);
			if (SkeletalMesh && SkeletalMesh->SkeletalMesh && SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs.Num() > 0)
			{
				for (INT iTriangle = 0; iTriangle < SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).KDOPTree.Triangles.Num(); iTriangle++)
				{
					const FkDOPCollisionTriangle<WORD>& Triangle = SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).KDOPTree.Triangles(iTriangle);

					FVector Pos = LocalToWorld.TransformFVector(SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).CollisionVerts(Triangle.v1));
					fTriangle[0][0] = Pos.X;
					fTriangle[0][1] = Pos.Y;
					fTriangle[0][2] = Pos.Z;

					Pos =  LocalToWorld.TransformFVector(SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).CollisionVerts(Triangle.v2));
					fTriangle[1][0] = Pos.X;
					fTriangle[1][1] = Pos.Y;
					fTriangle[1][2] = Pos.Z;

					Pos =  LocalToWorld.TransformFVector(SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).CollisionVerts(Triangle.v3));
					fTriangle[2][0] = Pos.X;
					fTriangle[2][1] = Pos.Y;
					fTriangle[2][2] = Pos.Z;
					ProcessCollisionTriangleToGridSet( fTriangle, Actor, gridSet );
				}
			}
		}
	}
#endif
}

BOOL ExportActor(CEditorScene* pScene, AActor* Actor)
{
#if WITH_EDITOR
	int nRetCode = 0;
	float fTriangle[3][3];

	if (Actor == NULL)
		return FALSE;

	FString LayerNameConfig;
	GConfig->GetString(TEXT("Configuration"), TEXT("LayerNameConfig"), LayerNameConfig, GEditorIni);
	LayerNameConfig = LayerNameConfig.ToUpper();
	FString actorLayer = Actor->Layer.ToString();
	actorLayer = actorLayer.ToUpper();
	if(actorLayer.InStr(LayerNameConfig) == INDEX_NONE)
	{
		return TRUE; // Not in the Path Layer, so it doesn't block pathfinding
	}

	for (INT ComponentIndex = 0; ComponentIndex < Actor->AllComponents.Num(); ComponentIndex++)
	{
		UPrimitiveComponent* PrimComponent = Cast<UPrimitiveComponent>(Actor->AllComponents(ComponentIndex));
		if (PrimComponent == NULL)
			continue;

		if (PrimComponent && PrimComponent->BlockNonZeroExtent && PrimComponent->BlockActors)
		{
			FMatrix& LocalToWorld = PrimComponent->LocalToWorld;
			const UBOOL bFlipCullMode = LocalToWorld.RotDeterminant() < 0.0f;

			UStaticMeshComponent *SMC = Cast<UStaticMeshComponent>(PrimComponent);
			if ((Actor->bCollideComplex && SMC) ||
				(SMC && SMC->StaticMesh && !SMC->StaticMesh->UseSimpleLineCollision))
			{
				if (SMC->StaticMesh )
				{
					FStaticMeshRenderData* StaticMeshRenderData = SMC->StaticMesh->GetLODForExport(0);

					for (INT iTriangle = 0; iTriangle < SMC->StaticMesh->kDOPTree.Triangles.Num(); iTriangle++)
					{
						const FkDOPCollisionTriangle<WORD>& Triangle = SMC->StaticMesh->kDOPTree.Triangles(iTriangle);

						FVector Pos = LocalToWorld.TransformFVector(StaticMeshRenderData->PositionVertexBuffer.VertexPosition(Triangle.v1));
						fTriangle[0][0] = Pos.X;
						fTriangle[0][1] = Pos.Y;
						fTriangle[0][2] = Pos.Z;

						Pos =  LocalToWorld.TransformFVector(StaticMeshRenderData->PositionVertexBuffer.VertexPosition(Triangle.v2));
						fTriangle[1][0] = Pos.X;
						fTriangle[1][1] = Pos.Y;
						fTriangle[1][2] = Pos.Z;

						Pos =  LocalToWorld.TransformFVector(StaticMeshRenderData->PositionVertexBuffer.VertexPosition(Triangle.v3));
						fTriangle[2][0] = Pos.X;
						fTriangle[2][1] = Pos.Y;
						fTriangle[2][2] = Pos.Z;

						nRetCode = ProcessCollisionTriangle(pScene, fTriangle, Actor);
						if (!nRetCode)
							return FALSE;
					}
				}

				continue;
			}

			URB_BodySetup* BodySetup = PrimComponent->GetRBBodySetup();
			if (BodySetup != NULL)
			{
				FKAggregateGeom& AggGeom = BodySetup->AggGeom;
				for (INT iElem = 0; iElem < AggGeom.ConvexElems.Num(); iElem++)
				{
					FKConvexElem& ConEl = AggGeom.ConvexElems(iElem);
					INT NumTris = ConEl.FaceTriData.Num()/3;

					for (INT iTriangle = 0; iTriangle < NumTris; iTriangle++)
					{
						FVector Pos = LocalToWorld.TransformFVector(ConEl.VertexData(ConEl.FaceTriData((iTriangle * 3) + 0)));
						fTriangle[0][0] = Pos.X;
						fTriangle[0][1] = Pos.Y;
						fTriangle[0][2] = Pos.Z;

						Pos = LocalToWorld.TransformFVector(ConEl.VertexData(ConEl.FaceTriData((iTriangle * 3) + 1)));
						fTriangle[1][0] = Pos.X;
						fTriangle[1][1] = Pos.Y;
						fTriangle[1][2] = Pos.Z;

						Pos = LocalToWorld.TransformFVector(ConEl.VertexData(ConEl.FaceTriData((iTriangle * 3) + 2)));
						fTriangle[2][0] = Pos.X;
						fTriangle[2][1] = Pos.Y;
						fTriangle[2][2] = Pos.Z;

						nRetCode = ProcessCollisionTriangle(pScene, fTriangle, Actor);
						if (!nRetCode)
							return FALSE;
					}
				}
				continue;
			}

			USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(PrimComponent);
			if (SkeletalMesh && SkeletalMesh->SkeletalMesh && SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs.Num() > 0)
			{
				for (INT iTriangle = 0; iTriangle < SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).KDOPTree.Triangles.Num(); iTriangle++)
				{
					const FkDOPCollisionTriangle<WORD>& Triangle = SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).KDOPTree.Triangles(iTriangle);

					FVector Pos = LocalToWorld.TransformFVector(SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).CollisionVerts(Triangle.v1));
					fTriangle[0][0] = Pos.X;
					fTriangle[0][1] = Pos.Y;
					fTriangle[0][2] = Pos.Z;

					Pos =  LocalToWorld.TransformFVector(SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).CollisionVerts(Triangle.v2));
					fTriangle[1][0] = Pos.X;
					fTriangle[1][1] = Pos.Y;
					fTriangle[1][2] = Pos.Z;

					Pos =  LocalToWorld.TransformFVector(SkeletalMesh->SkeletalMesh->PerPolyBoneKDOPs(0).CollisionVerts(Triangle.v3));
					fTriangle[2][0] = Pos.X;
					fTriangle[2][1] = Pos.Y;
					fTriangle[2][2] = Pos.Z;

					nRetCode = ProcessCollisionTriangle(pScene, fTriangle, Actor);
					if (!nRetCode)
						return FALSE;
				}
			}
		}
	}
#endif
	return TRUE;
}

BOOL ExportBSP(CEditorScene* pScene)
{
#if WITH_EDITOR
	int nRetCode = 0;

	IInterface_PylonGeometryProvider* GeomInt = InterfaceCast<IInterface_PylonGeometryProvider>(GEngine);
	if (GeomInt != NULL)
	{
		TArray<FVector> GeomVerts;
		TArray<INT> GeomFaces;

		GeomInt->GetBSPTriangles(GeomVerts, GeomFaces);

		for (int nIndex = 0; nIndex < GeomFaces.Num() / 3; nIndex++)
		{
			float fTriangle[3][3];

			FVector Pos = GeomVerts(GeomFaces(nIndex * 3));
			fTriangle[0][0] = Pos.X;
			fTriangle[0][1] = Pos.Y;
			fTriangle[0][2] = Pos.Z;

			Pos =  GeomVerts(GeomFaces(nIndex * 3 + 1));
			fTriangle[1][0] = Pos.X;
			fTriangle[1][1] = Pos.Y;
			fTriangle[1][2] = Pos.Z;

			Pos =  GeomVerts(GeomFaces(nIndex * 3 + 2));
			fTriangle[2][0] = Pos.X;
			fTriangle[2][1] = Pos.Y;
			fTriangle[2][2] = Pos.Z;

			nRetCode = ProcessCollisionTriangle(pScene, fTriangle, NULL);
			if (!nRetCode)
				return FALSE;
		}
	}

#endif
	return TRUE;
}

BOOL ExportTerrainActor(ATerrain* TerrainActor, CEditorScene* pScene)
{
	int nRetCode = 0;

	for (INT y = 0; y < TerrainActor->NumVerticesY - 1; y++)
	{
		for (INT x = 0 ; x < TerrainActor->NumVerticesX-1; x++)
		{
			if (TerrainActor->IsTerrainQuadVisible(x, y))
			{
				float fTriangle[3][3];

				FVector Pos = TerrainActor->GetWorldVertex(x,y);
				fTriangle[0][0] = Pos.X;
				fTriangle[0][1] = Pos.Y;
				fTriangle[0][2] = Pos.Z;

				Pos =  TerrainActor->GetWorldVertex(x+1,y);
				fTriangle[1][0] = Pos.X;
				fTriangle[1][1] = Pos.Y;
				fTriangle[1][2] = Pos.Z;

				Pos =  TerrainActor->GetWorldVertex(x+1,y+1);
				fTriangle[2][0] = Pos.X;
				fTriangle[2][1] = Pos.Y;
				fTriangle[2][2] = Pos.Z;

				nRetCode = ProcessCollisionTriangle(pScene, fTriangle, TerrainActor);
				if (!nRetCode)
					return FALSE;

				Pos = TerrainActor->GetWorldVertex(x,y);
				fTriangle[0][0] = Pos.X;
				fTriangle[0][1] = Pos.Y;
				fTriangle[0][2] = Pos.Z;

				Pos =  TerrainActor->GetWorldVertex(x+1,y+1);
				fTriangle[1][0] = Pos.X;
				fTriangle[1][1] = Pos.Y;
				fTriangle[1][2] = Pos.Z;

				Pos =  TerrainActor->GetWorldVertex(x,y+1);
				fTriangle[2][0] = Pos.X;
				fTriangle[2][1] = Pos.Y;
				fTriangle[2][2] = Pos.Z;

				nRetCode = ProcessCollisionTriangle(pScene, fTriangle, TerrainActor);
				if (!nRetCode)
					return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL ExportBlockingVolume(ABlockingVolume* BlockingVolume, CEditorScene* pScene)
{
	int nRetCode = 0;
	int AABB[2][3];

	FBox box = BlockingVolume->GetComponentsBoundingBox();

	AABB[0][0] = box.Min.X * POINT_PER_CM / GRID_WIDTH;
	AABB[0][1] = box.Min.Y * POINT_PER_CM / GRID_WIDTH;
	AABB[0][2] = box.Min.Z * POINT_PER_CM;
	AABB[1][0] = box.Max.X * POINT_PER_CM / GRID_WIDTH;
	AABB[1][1] = box.Max.Y * POINT_PER_CM / GRID_WIDTH;
	AABB[1][2] = box.Max.Z * POINT_PER_CM;

	for (int nGridX = AABB[0][0]; nGridX <= AABB[1][0]; nGridX++)
	{
		for (int nGridY = AABB[0][1]; nGridY <= AABB[1][1]; nGridY++)
		{
			float x = nGridX * GRID_WIDTH / POINT_PER_CM;
			float y = nGridY * GRID_WIDTH / POINT_PER_CM;
			ConvertXYToServer(x, y);
			GRID_DATA* pGrid = pScene->CreateGrid((INT32)(x * POINT_PER_CM), (INT32)(y * POINT_PER_CM), AABB[0][2], AABB[1][2]);
			if (pGrid == NULL)
				return FALSE;

			x = (nGridX * GRID_WIDTH + GRID_WIDTH / 2) / POINT_PER_CM;
			y = (nGridY * GRID_WIDTH + GRID_WIDTH / 2 ) / POINT_PER_CM;
			ConvertXYToServer(x, y);
			pGrid = pScene->CreateGrid((INT32)(x * POINT_PER_CM), (INT32)(y * POINT_PER_CM), AABB[0][2], AABB[1][2]);
			if (pGrid == NULL)
				return FALSE;
		}
	}

	return TRUE;
}

typedef bool (*MODIFY_GRID_FUNC)(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV);

bool SetGridTrigger(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;
    int nVolumeIndex = 0;
    int nEmptySlot = 0;
	//int nVolumeID = pVolume->AreaID;
    int nVolumeID = pVolume->ServerUniqueID;
    for (int i = 1; i < MAX_VOLUME_PER_ZONE; i++)
    {
        if (nEmptySlot == 0 && pZoneData->byVolume[i] == 0)
            nEmptySlot = i;
        if (pZoneData->byVolume[i] == nVolumeID)
        {
            nVolumeIndex = i;
            break;
        }
    }
    if (nVolumeIndex == 0)
    {
        if (nEmptySlot == 0)
        {
            return false;
        }
        pZoneData->byVolume[nEmptySlot] = nVolumeID;
        nVolumeIndex = nEmptySlot;
    }

    pGridData->uVolume = nVolumeIndex;

    return true;
}

bool SetGridPkType(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	pGridData->uPkType = pVolume->PKType;
    return true;
}

bool SetGridCanStall(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	pGridData->uStallLevel = pVolume->StallLevel;

	return true;
}

bool SetGridQuest(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	pGridData->bQuest = true;

	if (pVolume->Quests.Num() > MAX_QUEST_PER_ZONE)
		return false;

	for (INT i = 0; i < pVolume->Quests.Num(); ++ i)
	{
		pZoneData->nQuestID[i] = pVolume->Quests(i);
	}
	
    return true;
}

bool SetGridBlockType(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Blocking* pVolume = Cast<AMMOVolume_Blocking>(pV);
	if (pVolume == NULL)
		return false;

	if (pVolume->BlockMove)
		pGridData->bBlockMove = true;
	if (pVolume->BlockLeap)
		pGridData->bBlockLeap = true;
	if (pVolume->BlockMissile)
		pGridData->bBlockMissile = true;
	if (pVolume->BlockBullet)
		pGridData->bBlockBullet = true;
	if (pVolume->RoleBlock)
		pGridData->bNotBlockChar = true;

	return true;
}

bool SetGridDoor(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	int nDoorIndex = 0;
	int nEmptySlot = 0;
	if (pVolume->DoorID > 0)
	{
		for (int i = 1; i < MAX_DOOR_PER_ZONE; i++)
		{
			if (pZoneData->byDoor[i] == 0)
				nEmptySlot = i;
			if (pZoneData->byDoor[i] == pVolume->DoorID)
			{
				nDoorIndex = i;
				break;
			}
		}
		if (nDoorIndex == 0)
		{
			if (nEmptySlot == 0)
			{
				return false;
			}
			pZoneData->byDoor[nEmptySlot] = pVolume->DoorID;
			nDoorIndex = nEmptySlot;
		}
		pGridData->uDoor = nDoorIndex;
	}

	return true;
}

bool SetGridGuide(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	// todo:

	return true;
}

bool SetGridMine(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	pGridData->uMineLevel = pVolume->nMineLevel;

	return true;
}

bool SetGridTeleport(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	pGridData->bCannotTeleport = true;

	return true;
}

bool SetGridRoleLoseItem(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	pGridData->bRoleCannotLoseItem = true;

	return true;
}

bool SetGridRoleDropItem(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	pGridData->bRoleCannotDropItem = true;

	return true;
}

bool SetGridDropItem(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	pGridData->bCannotDropItem = true;

	return true;
}

bool SetGridSlayerArea(ZONE_DATA* pZoneData, GRID_DATA* pGridData, AVolume* pV)
{
	AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(pV);
	if (pVolume == NULL)
		return false;

	pGridData->bSlayerArea = true;

	return true;
}

MODIFY_GRID_FUNC g_ModifyGridFuncList[] =
{
    SetGridTrigger,
    SetGridPkType,
    SetGridCanStall,
	NULL,
    SetGridQuest,
	SetGridDoor,
	SetGridGuide,
	SetGridMine,
	SetGridTeleport,
	SetGridRoleLoseItem,
	SetGridRoleDropItem,
	SetGridDropItem,
	SetGridSlayerArea,
};

bool TraverseVolume(AVolume* pVolume, CEditorScene* pScene, MODIFY_GRID_FUNC func)
{
	//debugf(TEXT("TraverseVolume %s"), *pVolume->GetName());
    float AABB[2][3];

	FBox box = pVolume->GetComponentsBoundingBox();
	ConvertBoxToServer(box);

	AABB[0][0] = box.Min.X;
	AABB[0][1] = box.Min.Y;
	AABB[0][2] = box.Min.Z;
	AABB[1][0] = box.Max.X;
	AABB[1][1] = box.Max.Y;
	AABB[1][2] = box.Max.Z;

    int nAABB[2][3] = {0};

    nAABB[0][0] = floor(AABB[0][0] * POINT_PER_CM / GRID_WIDTH);
    nAABB[0][1] = floor(AABB[0][1] * POINT_PER_CM / GRID_WIDTH);
    nAABB[0][2] = floor(AABB[0][2] * POINT_PER_CM / GRID_LAYER_HEIGHT);

    nAABB[1][0] = floor(AABB[1][0] * POINT_PER_CM / GRID_WIDTH);
    nAABB[1][1] = floor(AABB[1][1] * POINT_PER_CM / GRID_WIDTH);
    nAABB[1][2] = floor(AABB[1][2] * POINT_PER_CM / GRID_LAYER_HEIGHT);

    TArrayNoInit<FKConvexElem>& rConvexElems = pVolume->BrushComponent->BrushAggGeom.ConvexElems;

    for (int nGridX = nAABB[0][0]; nGridX <= nAABB[1][0]; nGridX++)
    {
        for (int nGridY = nAABB[0][1]; nGridY <= nAABB[1][1]; nGridY++)
        {
            int nZoneX = nGridX / GRID_PER_ZONE;
            int nZoneY = nGridY / GRID_PER_ZONE;
            int nGridXInZone = nGridX % GRID_PER_ZONE;
            int nGridYInZone = nGridY % GRID_PER_ZONE;

            if (nZoneX < 0 || nZoneX >= MAX_ZONE_PER_SCENE ||
                nZoneY < 0 || nZoneY >= MAX_ZONE_PER_SCENE)
            {
                return false;
            }

            ZONE_DATA* pZoneData = pScene->GetZone(nZoneX, nZoneY);
            if (pZoneData != NULL)
            {
                UINT16 usGridIndex = pZoneData->GroundGrid[nGridXInZone][nGridYInZone];
                while (usGridIndex != NULL && (int)pZoneData->GridList[usGridIndex].uLowLayer < nAABB[1][2])
                {
                    if ((int)pZoneData->GridList[usGridIndex].uHighLayer > nAABB[0][2])
                    {
                        FVector point;
                        FVector normal;
                        FLOAT distance;
                        bool bInConvex = true;

                        point.X = (nZoneX * ZONE_WIDTH + nGridXInZone * GRID_WIDTH + GRID_WIDTH / 2) / POINT_PER_CM;
                        point.Y = (nZoneY * ZONE_WIDTH + nGridYInZone * GRID_WIDTH + GRID_WIDTH / 2) / POINT_PER_CM;
                        point.Z = pZoneData->GridList[usGridIndex].uHighLayer * GRID_LAYER_HEIGHT / POINT_PER_CM; 

						ConvertXYToClient(point.X, point.Y);

                        point = pVolume->BrushComponent->LocalToWorld.Inverse().TransformFVector(point);

                        for (int iConvex = 0; iConvex < rConvexElems.Num(); iConvex++)
                        {
                            FKConvexElem& rConvexElem = rConvexElems(iConvex);
                            if (!rConvexElem.PointIsWithin(point, normal, distance))
                            {
                                bInConvex = false;
                                break;
                            }
                        }

                        if (bInConvex)
                        {
                            if (!func(pZoneData, pZoneData->GridList + usGridIndex, pVolume))
                                return false;
                        }
                    }

                    usGridIndex = pZoneData->GridList[usGridIndex].uNextGrid;
                }
            }
        }
    }

    return true;
}

void AddGridBox(float fGridX, float fGridY, float fGridZL, float fGridZH, FLinearColor color)
{
	color.A = 0.3f;

	FDynamicDrawData dat;
	dat.Vertex.Add(8);

	INT gridWidth = GRID_WIDTH / POINT_PER_CM;
	
	dat.Vertex(0) = FVector(fGridX, fGridY, fGridZL);
	dat.Vertex(3) = FVector(fGridX + gridWidth, fGridY, fGridZL);
	dat.Vertex(2) = FVector(fGridX + gridWidth, fGridY + gridWidth, fGridZL);
	dat.Vertex(1) = FVector(fGridX, fGridY + gridWidth, fGridZL);

	dat.Vertex(4) = FVector(fGridX, fGridY, fGridZH);
	dat.Vertex(7) = FVector(fGridX + gridWidth, fGridY, fGridZH);
	dat.Vertex(6) = FVector(fGridX + gridWidth, fGridY + gridWidth, fGridZH);
	dat.Vertex(5) = FVector(fGridX, fGridY + gridWidth, fGridZH);
		
	for (int i = 0; i < 8; i++)
	{
		ConvertXYToClient(dat.Vertex(i).Position.X, dat.Vertex(i).Position.Y);
		dat.Vertex(i).Color = color;
	}

	dat.Index.Add(12 * 3);

	INT faces[] = {
		0, 2, 1 ,
		0, 3, 2 ,

		4, 5, 6 ,
		4, 6, 7 ,

		0, 1, 4 ,
		1, 5, 4 ,

		3, 7, 2 ,
		7, 6, 2 ,

		1, 2, 6 ,
		1, 6, 5 ,

		0, 7, 3 ,
		0, 4, 7
	};

	for (INT i = 0; i < 12 * 3; ++i)
	{		
		dat.Index(i) = faces[i];
	}			

	g_ErrorGridBlock.AddItem(dat);
}

void AddGrid(float fGridX, float fGridY, float fGridZL, float fGridZH, FLinearColor color)
{
	float width = GRID_WIDTH / POINT_PER_CM - 8;
	color.A = 0.7f;

	g_GridVerts.AddItem(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + 8, fGridZL), color));
	g_GridVerts.AddItem(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + 8, fGridZL), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + 8, fGridZL), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + 8, fGridZH), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + 8, fGridZH), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + 8, fGridZH), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + 8, fGridZH), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + 8, fGridZL), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + width, fGridZL), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + width, fGridZL), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + width, fGridZL), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + width, fGridZH), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + width, fGridZH), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + width, fGridZH), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + width, fGridZH), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + width, fGridZL), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + 8, fGridZL), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + width, fGridZL), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + 8, fGridZL), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + width, fGridZL), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + 8, fGridZH), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + width, fGridY + width, fGridZH), color));

	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + 8, fGridZH), color));
	g_GridVerts.Push(DRAW_GRID_DATA(FVector(fGridX + 8, fGridY + width, fGridZH), color));
}

BOOL RefreshGrid(void)
{
	float fCenterZoneX = (g_nCenterZoneX * ZONE_WIDTH + ZONE_WIDTH / 2) / POINT_PER_CM;
	float fCenterZoneY = (g_nCenterZoneY * ZONE_WIDTH + ZONE_WIDTH / 2) / POINT_PER_CM;
	g_GridVerts.Empty();

	ConvertXYToServer(fCenterZoneX, fCenterZoneY);
	INT32 nCenterZoneX = fCenterZoneX * POINT_PER_CM / ZONE_WIDTH;
	INT32 nCenterZoneY = fCenterZoneY * POINT_PER_CM / ZONE_WIDTH;

	for (int nZoneX = nCenterZoneX - g_nRange; nZoneX <= nCenterZoneX + g_nRange; nZoneX++)
	{
		for (int nZoneY = nCenterZoneY - g_nRange; nZoneY <= nCenterZoneY + g_nRange; nZoneY++)
		{
			ZONE_DATA* pZoneData = g_Scene.GetZone(nZoneX, nZoneY);
			if (pZoneData)
			{
				for (int nGridX = 0; nGridX < GRID_PER_ZONE; nGridX++)
				{
					for (int nGridY = 0; nGridY < GRID_PER_ZONE; nGridY++)
					{
						if ((nZoneX * 8 + nGridX) == 126 && (nZoneY * 8 + nGridY) == 64)
							nZoneX = nZoneX;

						UINT16 usGridIndex = pZoneData->GroundGrid[nGridX][nGridY];
						while (usGridIndex)
						{
							FLinearColor color;
							//int nNoObstacle = pZoneData->GridList[usGridIndex].uNoObstacle;
							//if(nNoObstacle >= 10)
							//	nNoObstacle = 10;

							//float r = (float)nNoObstacle / 10.0f;

							if (pZoneData->GridList[usGridIndex].uDoor > 0)
								color = FLinearColor(0, 0, 1.0);
							else if (pZoneData->GridList[usGridIndex].bOutOfPoly)
								color = FLinearColor(1.0, 0, 0);
							else if (pZoneData->GridList[usGridIndex].uVolume > 0)
								color = FLinearColor(0, 1.0, 1.0);
							else if (pZoneData->GridList[usGridIndex].bQuest > 0)
								color = FLinearColor(1.0, 1.0, 0);
							else if (pZoneData->GridList[usGridIndex].uPkType == gptSafe)
								color = FLinearColor(0, 1.0, 0);
							else if (pZoneData->GridList[usGridIndex].uPkType == gptFree)
								color = FLinearColor(1.0, 0.5, 0);
							else if (pZoneData->GridList[usGridIndex].uMineLevel == 1)
								color = FLinearColor(0.2, 0.2, 0.2);
							else if (pZoneData->GridList[usGridIndex].uMineLevel == 2)
								color = FLinearColor(0.5, 0, 0);
							else
								color = FLinearColor(1.0, 1.0, 1.0);

							AddGrid((nZoneX * ZONE_WIDTH + nGridX * GRID_WIDTH) / POINT_PER_CM,
								(nZoneY * ZONE_WIDTH + nGridY * GRID_WIDTH) / POINT_PER_CM,
								pZoneData->GridList[usGridIndex].uLowLayer * GRID_LAYER_HEIGHT / POINT_PER_CM, 
								pZoneData->GridList[usGridIndex].uHighLayer * GRID_LAYER_HEIGHT / POINT_PER_CM, color);
							usGridIndex = pZoneData->GridList[usGridIndex].uNextGrid;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < g_GridVerts.Num(); i++)
	{
		ConvertXYToClient(g_GridVerts(i).pos.X, g_GridVerts(i).pos.Y);
	}

	return TRUE;
}

BOOL LoadGrid(void)
{
	int nRetCode = 0;
	FString MapName = GWorld->GetMapName();

	nRetCode = g_Scene.UnInit();
	if (!nRetCode)
		goto Exit0;

	nRetCode = g_Scene.Init(*MapName);
	if (!nRetCode)
		goto Exit0;

	nRetCode = g_Scene.Load();
	if (!nRetCode)
		goto Exit0;

	nRetCode = g_ReviseScene.UnInit();
	if (!nRetCode)
		goto Exit0;

	nRetCode = g_ReviseScene.Init(*MapName);
	if (!nRetCode)
		goto Exit0;

	nRetCode = g_ReviseScene.Load();
	if (!nRetCode)
		goto Exit0;

	return TRUE;
Exit0:
	return FALSE;
}

bool MarkGridInRange(AMMOServerPoint* pSvrPoint, INT64 x, INT64 y)
{
//	INT64 nX = pSvrPoint->Location.X * POINT_PER_CM;
//	INT64 nY = pSvrPoint->Location.Y * POINT_PER_CM;
	INT64 nZ = pSvrPoint->Location.Z * POINT_PER_CM;
//	INT64 nRangeLmt = pSvrPoint->Range * POINT_PER_CM;

	GRID_DATA* pGrid = (GRID_DATA*)g_Scene._GetLowerGrid(x, y, nZ + 5000);
	if(pGrid == NULL)
		goto Exit0;

//	INT64 nDetX = x - nX;
//	INT64 nDetY = y - nY;

//	if(nDetX * nDetX + nDetY * nDetY <= nRangeLmt * nRangeLmt)
//	{
	ZONE_DATA* pZone = g_Scene.GetZone(x / ZONE_WIDTH, y / ZONE_WIDTH);
	if(pZone == NULL)
	{
		GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, pSvrPoint, *FString::Printf(TEXT("Failed to add revive pt, invalid zone.")), NULL);
		goto Exit0;
	}

	pZone->byServerPointIndex = pSvrPoint->ServerUniqueID;
	pGrid->bInServerPointRange = true;
	return true;
//	}

Exit0:
	return false;
}


UBOOL AScout::GenerateGridData(void)
{
#if WITH_EDITOR
	int nRetCode = 0;
	wchar_t szMessage[1024];

	FString MapName = GWorld->GetMapName();

	// oscar@snda
	for(TObjectIterator<APylon> it; it; ++it)
	{
		if(!(*it)->NavGuid.IsValid())
		{
			(*it)->NavGuid = appCreateGuid();
		}
	}
	// oscar@snda

	nRetCode = g_Scene.UnInit();
	if (!nRetCode)
		goto Exit0;

	nRetCode = g_Scene.Init(*MapName);
	if (!nRetCode)
		goto Exit0;

	GWarn->StatusUpdatef(0, 0, TEXT("Building grid"));

	nRetCode = ExportBSP(&g_Scene);
	if (!nRetCode)
	{
//		GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Failed to export bsp")), NULL);
		goto Exit0;
	}

	for (INT iLevel = 0; iLevel < GWorld->Levels.Num(); iLevel++)
	{
		ULevel* Level = GWorld->Levels(iLevel);
		if (Level == NULL)
			continue;

		for (INT iActor = 0; iActor < Level->Actors.Num(); iActor++)
		{
			FString ActorName;

			AActor* Actor = Level->Actors(iActor);
			Actor->GetName(ActorName);
			
#ifdef _DEBUG
			if (ActorName == TEXT("StaticMeshActor_2321"))
				iActor = iActor;
#endif

			UBOOL bCanExport = (Actor != NULL) &&
				Actor->bBlockActors && Actor->bCollideActors &&
				Actor->CollisionType != COLLIDE_NoCollision &&
				Actor->CollisionType != COLLIDE_BlockWeapons;

			// Static MESH, if it auto-hides, don't export
			if (Actor && Actor->IsA(AStaticMeshActor::StaticClass()) && ((AStaticMeshActor*)(Actor))->bCanHidden)
			{
				bCanExport = FALSE;
			}

			// landscape actors
			ALandscapeProxy* LandscapeActor = Cast<ALandscapeProxy>(Actor);
			if (LandscapeActor != NULL && Actor->CollisionType != COLLIDE_NoCollision)
			{
				ALandscape* landscape = Cast<ALandscape>(Actor);
				//if(landscape)
				{
					for (INT iComponent = 0; iComponent < LandscapeActor->LandscapeComponents.Num(); iComponent++)
					{
						ULandscapeComponent* Component = LandscapeActor->LandscapeComponents(iComponent);
						//						ULandscapeHeightfieldCollisionComponent* Component = LandscapeActor->CollisionComponents(iComponent);
						if (Component)
						{
							ULandscapeInfo* Info = Component->GetLandscapeInfo();
							QWORD ComponentKey = ALandscape::MakeKey(Component->SectionBaseX,Component->SectionBaseY);
							ULandscapeHeightfieldCollisionComponent* CollisionComponent = Info ? Info->XYtoCollisionComponentMap.FindRef(ComponentKey) : NULL;

							if (CollisionComponent)
							{
								nRetCode = ExportLandscape(&g_Scene, CollisionComponent, Component, landscape);
								if (!nRetCode)
								{
									GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, Actor, *FString::Printf(TEXT("Failed to export landscape %s"), *ActorName), NULL);
									goto Exit0;
								}
							}
						}
					}

					continue;
				}
			}

			// skip all terrain actors
			ATerrain* TerrainActor = Cast<ATerrain>(Actor);
			if (TerrainActor != NULL)
			{
				nRetCode = ExportTerrainActor(TerrainActor, &g_Scene);
				if (!nRetCode)
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, Actor, *FString::Printf(TEXT("Failed to export terrain %s"), *ActorName), NULL);
					goto Exit0;
				}

				continue;
			}

			// special case for fractures with cores
			//AFracturedStaticMeshActor* FractureActor = Cast<AFracturedStaticMeshActor>(Actor);
			//if (FractureActor != NULL &&
			//	FractureActor->FracturedStaticMeshComponent != NULL &&
			//	FractureActor->FracturedStaticMeshComponent->GetCoreFragmentIndex() != INDEX_NONE &&
			//	(FractureActor->bBlockActors || FractureActor->bPathColliding))
			//{
			//	bCanExport = TRUE;
			//}

			ABlockingVolume* BlockingVolume = Cast<ABlockingVolume>(Actor);
			if (BlockingVolume)
			{
				nRetCode = ExportBlockingVolume(BlockingVolume, &g_Scene);
				if (!nRetCode)
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, Actor, *FString::Printf(TEXT("Failed to export terrain %s"), *ActorName), NULL);
					goto Exit0;
				}

				continue;
			}

			// regular export
			if (bCanExport)
			{
				DWORD dwStartTime = GetTickCount();

				nRetCode = ExportActor(&g_Scene, Actor);
				if (!nRetCode)
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, Actor, *FString::Printf(TEXT("Failed to export npc %s"), *ActorName), NULL);
					goto Exit0;
				}

				INT32 nInterval = GetTickCount() - dwStartTime;
				wsprintf(szMessage, L"%s : %d\n", *ActorName, nInterval);
				GWarn->StatusUpdatef(0, 0, szMessage);
			}
		}
	}

	nRetCode = g_Scene.Preprocess();
	if (!nRetCode)
	{
		GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Failed to merge grid data of scene %s"), *MapName), NULL);
		goto Exit0;
	}

    for (INT iLevel = 0; iLevel < GWorld->Levels.Num(); iLevel++)
    {
        ULevel* Level = GWorld->Levels(iLevel);
        if (Level == NULL)
            continue;

        for (INT iActor = 0; iActor < Level->Actors.Num(); iActor++)
        {
            FString ActorName;
            AActor* Actor = Level->Actors(iActor);
            Actor->GetName(ActorName);

            AMMOVolume_Trigger* pVolume = Cast<AMMOVolume_Trigger>(Actor);
            if (pVolume)
            {
				if (pVolume->VolumeType >= EVolumeTriggerType_MAX || pVolume->VolumeType == EVVT_Surface)
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, pVolume, *FString::Printf(TEXT("error volume type %s"), *ActorName), NULL);
					goto Exit0;
				}

                nRetCode = TraverseVolume(pVolume, &g_Scene, g_ModifyGridFuncList[pVolume->VolumeType]);
                if (!nRetCode)
                {
                    GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, pVolume, *FString::Printf(TEXT("Failed to add volume %s"), *ActorName), NULL);
                    goto Exit0;
                }
                if (pVolume->VolumeType == EVTT_Trigger)
				{
					//g_Scene.SetVolumeScript(pVolume->AreaID, TCHAR_TO_ANSI(*pVolume->BehaviorTreeName));
					nRetCode = g_Scene.SetVolumeScript(pVolume->ServerUniqueID, TCHAR_TO_ANSI(*pVolume->BehaviorTreeName));
					if (!nRetCode)
					{
						GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, pVolume, *FString::Printf(TEXT("volume %s ID invalid"), *ActorName), NULL);
						goto Exit0;
					}
					//g_Scene.SetMaxVolumeTriggerCount(pVolume->AreaID, pVolume->MaxTriggerCount);
					g_Scene.SetMaxVolumeTriggerCount(pVolume->ServerUniqueID, pVolume->MaxTriggerCount);
                }
                continue;
            }
			AMMOVolume_Blocking* pBlocking = Cast<AMMOVolume_Blocking>(Actor);
			if (pBlocking)
			{
				nRetCode = TraverseVolume(pBlocking, &g_Scene, SetGridBlockType);
				if (!nRetCode)
				{
					GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, pBlocking, *FString::Printf(TEXT("Failed to add volume %s"), *ActorName), NULL);
					goto Exit0;
				}
				continue;
			}

			// Mark Grids In Range.
			/************************************************************************/
			// chr!5@snda
			AMMOServerPoint* pSvrPoint = Cast<AMMOServerPoint>(Actor);
			if (pSvrPoint && pSvrPoint->DisplayName.Len() > 1)
			{
				if(pSvrPoint->DisplayName[0] == TEXT('#') && pSvrPoint->DisplayName[1] == TEXT('R') )
				{
					INT32 nRange = 0;

					float fX = pSvrPoint->Location.X;
					float fY = pSvrPoint->Location.Y;

					ConvertXYToServer(fX, fY);
					fX *= POINT_PER_CM;
					fY *= POINT_PER_CM;

					INT64 nRangeLmt = pSvrPoint->Range * POINT_PER_CM;

					for(INT64 x = fX - nRangeLmt; x < fX + nRangeLmt; x += GRID_WIDTH)
					{
						for(INT64 y = fY - nRangeLmt; y < fY + nRangeLmt; y += GRID_WIDTH)
						{
							if(!MarkGridInRange(pSvrPoint, x, y))
								continue;
						}
					}

					//do 
					//{
					//	bool bOver = true;

					//	for(INT64 x = nX - nRange * GRID_WIDTH; x <= nX + nRange * GRID_WIDTH; x += GRID_WIDTH)
					//	{
					//		if(MarkGridInRange(pSvrPoint, x, nY - nRange * GRID_WIDTH))
					//			bOver = false;
					//		if(MarkGridInRange(pSvrPoint, x, nY + nRange * GRID_WIDTH))
					//			bOver = false;
					//	}


					//	for(INT64 y = nY - nRange * GRID_WIDTH; y <= nY + nRange * GRID_WIDTH; y += GRID_WIDTH)
					//	{
					//		if(MarkGridInRange(pSvrPoint, nX - nRange * GRID_WIDTH, y))
					//			bOver = false;
					//		if(MarkGridInRange(pSvrPoint, nX + nRange * GRID_WIDTH, y))
					//			bOver = false;
					//	}

					//	if(bOver)
					//		break;

					//	nRange++;

					//} while (true);
				}

			}
		}
	}

	return TRUE;
Exit0:
	GWarn->MapCheck_Add(MapCheckType::MCTYPE_ERROR, NULL, *FString::Printf(TEXT("Failed to build grid data of scene %s"), *MapName), NULL);

	return FALSE; 
#else
	return TRUE;
#endif
}
// Gen Grid
/************************************************************************/
// james@snda
#else // SHIPPING_PC_GAME

#if WITH_EDITOR
UBOOL AScout::GenerateGridData(void)
{
	return TRUE;
}
#endif

#endif // #if !SHIPPING_PC_GAME
