/*=============================================================================
	AnimSetViewerMain.cpp: AnimSet viewer main
	Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

	This code contains embedded portions of source code from dqconv.c Conversion routines between (regular quaternion, translation) and dual quaternion, Version 1.0.0, Copyright ?2006-2007 University of Dublin, Trinity College, All Rights Reserved, which have been altered from their original version.

	The following terms apply to dqconv.c Conversion routines between (regular quaternion, translation) and dual quaternion, Version 1.0.0:

	This software is provided 'as-is', without any express or implied warranty.  In no event will the author(s) be held liable for any damages arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
=============================================================================*/

#include "UnrealEd.h"
#include "EnginePhysicsClasses.h"
#include "EngineAnimClasses.h"
#include "AnimSetViewer.h"
#include "AbilityEditor.h"
#include "MouseDeltaTracker.h"
#include "PropertyWindow.h"
#include "DlgGenericComboEntry.h"
#include "SocketManager.h"
#include "DlgAnimationCompression.h"
#include "AnimationUtils.h"
#include "UnSkelRenderPublic.h"
#include "UnSkeletalMesh.h"
#include "GPUSkinVertexFactory.h"
#include "UnSkeletalMeshSorting.h"
#include "SkelImport.h"
#include "EngineAudioDeviceClasses.h"
#include "Engine.h"

#include <vector>
using namespace std;


#include "../../Engine/Inc/EngineClasses.h"
#include "../../Engine/Inc/EngineMeshClasses.h"
#include "../../Engine/Inc/EngineAnimClasses.h"
#include "../../Engine/Inc/EngineAIClasses.h"
#include "../../GameFramework/Inc/GameFrameworkClasses.h"
#include "../../MMOGame/Inc/MMOGameClasses.h"

#if WITH_SIMPLYGON
#include "SkeletalMeshSimplificationWindow.h"
#endif // #if WITH_SIMPLYGON

#if WITH_MANAGED_CODE
#include "FileSystemNotificationShared.h"
#endif


IMPLEMENT_CLASS(UAbilitySkelComponent);

static const FColor ExtraMesh1WireColor = FColor(255,215,215,255);
static const FColor ExtraMesh2WireColor = FColor(215,255,215,255);
static const FColor ExtraMesh3WireColor = FColor(215,215,255,255);

static const FLOAT	AnimSetViewer_TranslateSpeed = 0.25f;
static const FLOAT	AnimSetViewer_RotateSpeed = 0.02f;
static const FLOAT	AnimSetViewer_LightRotSpeed = 40.0f;
static const FLOAT	AnimSetViewer_WindStrengthSpeed = 0.1f;
#if WITH_APEX
static const FLOAT  AnimSetViewer_WindVelocityBlendTimeSpeed = 0.001f;
#endif

/*
*  Hit proxy for game stats visualization
*  Routes the rendered item selected to the proper visualizer
*/
struct HVertInfluenceProxy : public HHitProxy
{
	DECLARE_HIT_PROXY(HVertInfluenceProxy, HHitProxy);

	/** LOD this vert is for */
	INT LODIdx;
	/** Influence being used */
	INT InfluenceIdx;
	/** Vert index corresponding to the sprite */
	INT VertIndex;
	/** Is it already part of the influence swap */
	UBOOL bContributingInfluence;

	HVertInfluenceProxy(INT InLODIdx, INT InInfluenceIdx, INT InVertIndex, UBOOL bInContributingInfluence):
	HHitProxy(HPP_UI),
		LODIdx(InLODIdx), InfluenceIdx(InInfluenceIdx), VertIndex(InVertIndex), bContributingInfluence(bInContributingInfluence) {}
};

/**
 * A hit proxy class for sockets.
 */
struct HASVSocketProxy : public HHitProxy
{
	DECLARE_HIT_PROXY( HASVSocketProxy, HHitProxy );

	INT		SocketIndex;

	HASVSocketProxy(INT InSocketIndex)
		:	HHitProxy( HPP_UI )
		,	SocketIndex( InSocketIndex )
	{}
};


/*
 *  Hit proxy for vertex info
 *  Select which vertex and will show the proper information
 */
struct HVertexInfoProxy : public HHitProxy
{
	DECLARE_HIT_PROXY(HVertexInfoProxy, HHitProxy);

	/** Vert index corresponding to the sprite */
	INT		VertexID;
	FVector VertexPosition;
	TArray<INT>	InfluencedIndices;
	TArray<FLOAT>	InfluencedWeights;
	
	HVertexInfoProxy(const INT InVertexID, const FVector& InVertexPosition):
	HHitProxy(HPP_UI),
	VertexID(InVertexID), VertexPosition(InVertexPosition) {}

	void AddInfluences(TArray<INT> Indices, TArray<FLOAT> Weights)
	{
		InfluencedIndices.Empty();
		InfluencedIndices.Append(Indices);
		InfluencedWeights.Empty();
		InfluencedWeights.Append(Weights);
	}
};

// /** Draw the verts that are part of the cloth, colorised to show how far they can move from animated pose */
// void // DrawClothMovementDistanceScale(FPrimitiveDrawInterface* PDI, const USkeletalMeshComponent* SkelComp)
// {
// 	if(!SkelComp || !SkelComp->SkeletalMesh)
// 	{
// 		return;
// 	}
// 
// 	USkeletalMesh* SkelMesh = SkelComp->SkeletalMesh;
// 
// 	// Check array is right size before we draw
// 	if(SkelMesh->ClothToGraphicsVertMap.Num() != SkelMesh->ClothMovementScale.Num())
// 	{
// 		return;
// 	}
// 
// 	for(INT VertIdx=0; VertIdx<SkelMesh->ClothToGraphicsVertMap.Num(); VertIdx++)
// 	{
// 		UBOOL bFreeVert = (VertIdx < SkelMesh->NumFreeClothVerts);
// 
// 		// Find the index of the graphics vertex that corresponds to this cloth vertex
// 		INT GraphicsIndex = SkelMesh->ClothToGraphicsVertMap(VertIdx);
// 		FVector SkinnedPos = SkelComp->GetSkinnedVertexPosition(GraphicsIndex);
// 		FVector WorldPos = SkelComp->LocalToWorld.TransformFVector(SkinnedPos); // Transform into world space
// 
// 		FLinearColor RedColor(1,0,0);
// 		FLinearColor BlueColor(0,0,1);
// 
// 		FLinearColor VertColor(1,1,1);
// 		if(bFreeVert)
// 		{
// 			FLOAT DistanceScale = SkelMesh->ClothMovementScale(VertIdx);
// 			VertColor = Lerp(BlueColor, RedColor, DistanceScale);
// 		}
// 
// 		PDI->DrawPoint(WorldPos, VertColor, 5.0f, SDPG_World);
// 	}
// }

/*
 *   Skin and draw as point sprites all given vertices in a skeletal mesh
 *  @param PDI - Draw interface
 *  @param SkelComp - Skeletal component to draw verts from
 *  @param BoneInfluenceMappings - Array of vertex indices to draw
 *  @param VertexColor - color to draw the sprites
 *  @param bContributingVerts - do these verts contribute to the influence 
 */
// void // DrawVertInfluenceLocations(FPrimitiveDrawInterface* PDI, const USkeletalMeshComponent* SkelComp, const TSet<INT>& BoneInfluenceVerts, const FLinearColor& VertexColor, UBOOL bContributingVerts)
// {
// 	if (BoneInfluenceVerts.Num() > 0)
// 	{
// 		DWORD StatusRegister = VectorGetControlRegister();
// 		VectorSetControlRegister( StatusRegister | VECTOR_ROUND_TOWARD_ZERO );
// 
// 		FMatrix LocalToWorldMat = SkelComp->LocalToWorld;
// 
// 		//@TODO handle multiple LOD and influence tracks
// 		if (ensure (SkelComp->SkeletalMesh->LODModels.IsValidIndex( SkelComp->PredictedLODLevel ) ))
// 		{
// 			const INT LODIdx = SkelComp->PredictedLODLevel;
// 			const INT InfluenceIdx = 0;
// 			const FStaticLODModel& LODModel = SkelComp->SkeletalMesh->LODModels(LODIdx);
// 			const INT NumVerts = BoneInfluenceVerts.Num();
// 
// 			//Calculate the current bone transforms
// 			TArray<FBoneAtom> RefToLocal;
// 			UpdateRefToLocalMatrices(RefToLocal, SkelComp, LODIdx);
// 			const FBoneAtom* RESTRICT ReferenceToLocal = RefToLocal.GetTypedData();
// 
// 			//Find the delimiters that separate chunks in the vertex buffer
// 			TArray<INT> ChunkMarkers;
// 			for(INT ChunkIndex = 0;ChunkIndex < LODModel.Chunks.Num();ChunkIndex++)
// 			{
// 				const FSkelMeshChunk& Chunk = LODModel.Chunks(ChunkIndex);
// 				ChunkMarkers.AddItem(Chunk.BaseVertexIndex + Chunk.GetNumVertices());
// 			}
// 
// 			MS_ALIGN(16) FLOAT Float128[4] GCC_ALIGN(4);
// 			FVector* TransformedPosition = (FVector*)Float128;
// 
// 			INT CurrentChunk = 0;
// 			FVector WorldPos;
// 			const FGPUSkinVertexBase* SrcRigidVertex = NULL;
// 
// 			UBOOL bUseInfluences = bContributingVerts && SkelComp->LODInfo(LODIdx).bAlwaysUseInstanceWeights && LODModel.VertexInfluences.Num() > 0 && LODModel.VertexInfluences(InfluenceIdx).Influences.Num() > 0;
// 			for( TSet<INT>::TConstIterator VertIter( BoneInfluenceVerts ); VertIter != NULL; ++VertIter )
// 			{
// 				//Get the bind pose position of the vertex
// 				const INT VertexBufferIndex = *VertIter;
// 				SrcRigidVertex = LODModel.VertexBufferGPUSkin.GetVertexPtr(VertexBufferIndex);
// 				const FVector PosePosition = LODModel.VertexBufferGPUSkin.GetVertexPosition(SrcRigidVertex);
// 
// 				//Find the chunk that this vert belongs to (assumes vert list is sorted so we only ever increase chunk index)
// 				if (VertexBufferIndex >= ChunkMarkers(CurrentChunk))
// 				{
// 					while (VertexBufferIndex >= ChunkMarkers(CurrentChunk) && CurrentChunk < ChunkMarkers.Num() - 1)
// 					{
// 						CurrentChunk++;
// 					}
// 				}
// 
// 				if (CurrentChunk < LODModel.Chunks.Num())
// 				{
// 					//Get the bone map relevant for this vertex
// 					const WORD* RESTRICT BoneMap = LODModel.Chunks(CurrentChunk).BoneMap.GetTypedData();
// 
// 					//Get the bone influences from the original mesh or the swapped bone influences array
// 					const BYTE* RESTRICT BoneIndices;
// 					const BYTE* RESTRICT BoneWeights;
// 					if (bUseInfluences)
// 					{
// 						BoneIndices = LODModel.VertexInfluences(InfluenceIdx).Influences(VertexBufferIndex).Bones.InfluenceBones;
// 						BoneWeights = LODModel.VertexInfluences(InfluenceIdx).Influences(VertexBufferIndex).Weights.InfluenceWeights;
// 					}
// 					else
// 					{
// 						BoneIndices = SrcRigidVertex->InfluenceBones;
// 						BoneWeights = SrcRigidVertex->InfluenceWeights;
// 					}
// 
// 					//Skin the vertex
// 					VectorRegister SrcPoints;
// 					VectorRegister DstPoints;
// 					SrcPoints = VectorLoadFloat3_W1( &PosePosition );
// 
// 					VectorRegister Weights = VectorMultiply( VectorLoadByte4(BoneWeights), VECTOR_INV_255 );
// 					VectorResetFloatRegisters(); // Need to call this to be able to use regular floating point registers again after Unpack and VectorLoadByte4.
// #if QUAT_SKINNING
// 					// Dual Quaternion part
// 					// Linearly blend DQs
// 					FBoneQuat DualQuat;
// 					DualQuat.SetBoneAtom(ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_0]]]);
// 					VectorRegister Weight0 = VectorReplicate( Weights, INFLUENCE_0 );
// 					VectorRegister BlendDQ0	= VectorMultiply( VectorLoad( &DualQuat.DQ1[0] ), Weight0 );
// 					VectorRegister BlendDQ1	= VectorMultiply( VectorLoad( &DualQuat.DQ2[0] ), Weight0 );
// 					FLOAT	Scale = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_0]]].GetScale();
// 					VectorRegister BlendScale = VectorMultiply( VectorLoadFloat1( &Scale ), Weight0 );
// 
// 					// Save first DQ0 for testing shortest route for blending
// 					VectorRegister BaseQuat = BlendDQ0;
// 					DualQuat.SetBoneAtom(ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_1]]]);
// 
// 					VectorRegister Weight1 = VectorReplicate( Weights, INFLUENCE_1 );
// 
// 					VectorRegister DQ0 = VectorLoad( &DualQuat.DQ1[0] );
// 					VectorRegister DQ1 = VectorLoad( &DualQuat.DQ2[0] );
// 
// 					// blend scale - need to be done before negate weight
// 					Scale = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_1]]].GetScale();
// 					BlendScale	= VectorMultiplyAdd( VectorLoadFloat1( &Scale ), Weight1, BlendScale );
// 
// 					// If not shortest route, negate weight
// 					if ( VectorAnyGreaterThan(VectorZero(), VectorDot4( BaseQuat, DQ0 )) )
// 					{
// 						Weight1 = VectorNegate(Weight1);
// 					}
// 
// 					BlendDQ0	= VectorMultiplyAdd( DQ0 , Weight1, BlendDQ0 );
// 					BlendDQ1	= VectorMultiplyAdd( DQ1 , Weight1, BlendDQ1 );
// 
// 					DualQuat.SetBoneAtom(ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_2]]]);
// 
// 					VectorRegister Weight2 = VectorReplicate( Weights, INFLUENCE_2 );
// 
// 					DQ0 = VectorLoad( &DualQuat.DQ1[0] );
// 					DQ1 = VectorLoad( &DualQuat.DQ2[0] );
// 
// 					Scale = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_2]]].GetScale();
// 					BlendScale	= VectorMultiplyAdd( VectorLoadFloat1( &Scale ), Weight2, BlendScale );
// 
// 					// If not shortest route, negate weight
// 					if ( VectorAnyGreaterThan(VectorZero(), VectorDot4( BaseQuat, DQ0 )) )
// 					{
// 						Weight2 = VectorNegate(Weight2);
// 					}
// 
// 					BlendDQ0	= VectorMultiplyAdd( DQ0 , Weight2, BlendDQ0 );
// 					BlendDQ1	= VectorMultiplyAdd( DQ1 , Weight2, BlendDQ1 );
// 
// 					DualQuat.SetBoneAtom(ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_3]]]);
// 					VectorRegister Weight3 = VectorReplicate( Weights, INFLUENCE_3 );
// 					DQ0 = VectorLoad( &DualQuat.DQ1[0] );
// 					DQ1 = VectorLoad( &DualQuat.DQ2[0] );
// 
// 					Scale = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_3]]].GetScale();
// 					BlendScale	= VectorMultiplyAdd( VectorLoadFloat1( &Scale ), Weight3, BlendScale );
// 
// 					// If not shortest route, negate weight
// 					if ( VectorAnyGreaterThan(VectorZero(), VectorDot4( BaseQuat, DQ0 )) )
// 					{
// 						Weight3 = VectorNegate(Weight3);
// 					}
// 
// 					BlendDQ0	= VectorMultiplyAdd( DQ0 , Weight3, BlendDQ0 );
// 					BlendDQ1	= VectorMultiplyAdd( DQ1 , Weight3, BlendDQ1 );
// 
// 					// Scale the position
// 					SrcPoints = VectorSet_W1(VectorMultiply(SrcPoints, BlendScale));
// 
// 					// Normalize
// 					VectorRegister RecipLen = VectorReciprocalLen(BlendDQ0);
// 					BlendDQ0 = VectorMultiply(BlendDQ0, RecipLen);
// 					BlendDQ1 = VectorMultiply(BlendDQ1, RecipLen);
// 
// 					// Cache variables to transform
// 					VectorRegister BlendDQ0YZW = VectorSet_W0(VectorSwizzle(BlendDQ0, 1, 2, 3, 0)); 
// 					VectorRegister BlendDQ1YZW = VectorSet_W0(VectorSwizzle(BlendDQ1, 1, 2, 3, 0)); 
// 					VectorRegister BlendDQ0XXXX = VectorReplicate(BlendDQ0, 0);
// 					VectorRegister BlendDQ1XXXX = VectorReplicate(BlendDQ1, 0);
// 
// 					/* 
// 					* Dual Quaternion - http://isg.cs.tcd.ie/projects/DualQuaternions/
// 					* Convert DQ to Matrix
// 					* This is faster in our case since it calculates DQ result once and reuse it for position/tangents
// 					* If you use transform, you need to transform at least 3 times - more expensive (a lot of cross products)
// 					* DQToMatrix does not work with Scale - so I'm saving this for reference in the future
// 					*/
// 					DstPoints = VectorMultiplyAdd(VECTOR_2222, VectorCross(BlendDQ0YZW, VectorMultiplyAdd(BlendDQ0XXXX, SrcPoints, VectorCross(BlendDQ0YZW, SrcPoints))), SrcPoints);
// 					VectorRegister Trans = VectorMultiplyAdd(BlendDQ0XXXX, BlendDQ1YZW, VectorAdd(VectorNegate(VectorMultiply(BlendDQ1XXXX, BlendDQ0YZW)), VectorCross(BlendDQ0YZW, BlendDQ1YZW)));
// 					DstPoints = VectorMultiplyAdd(VECTOR_2222, Trans, DstPoints);
// #else
// 					//Bone 1
// 					const FMatrix BoneMatrix0 = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_0]]].ToMatrix();
// 					VectorRegister Weight0 = VectorReplicate( Weights, INFLUENCE_0 );
// 					VectorRegister M00	= VectorMultiply( VectorLoadAligned( &BoneMatrix0.M[0][0] ), Weight0 );
// 					VectorRegister M10	= VectorMultiply( VectorLoadAligned( &BoneMatrix0.M[1][0] ), Weight0 );
// 					VectorRegister M20	= VectorMultiply( VectorLoadAligned( &BoneMatrix0.M[2][0] ), Weight0 );
// 					VectorRegister M30	= VectorMultiply( VectorLoadAligned( &BoneMatrix0.M[3][0] ), Weight0 );
// 
// 					//Bone 2
// 					const FMatrix BoneMatrix1 = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_1]]].ToMatrix();
// 					VectorRegister Weight1 = VectorReplicate( Weights, INFLUENCE_1 );
// 					M00	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix1.M[0][0] ), Weight1, M00 );
// 					M10	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix1.M[1][0] ), Weight1, M10 );
// 					M20	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix1.M[2][0] ), Weight1, M20 );
// 					M30	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix1.M[3][0] ), Weight1, M30 );
// 
// 					//Bone 3
// 					const FMatrix BoneMatrix2 = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_2]]].ToMatrix();
// 					VectorRegister Weight2 = VectorReplicate( Weights, INFLUENCE_2 );
// 					M00	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix2.M[0][0] ), Weight2, M00 );
// 					M10	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix2.M[1][0] ), Weight2, M10 );
// 					M20	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix2.M[2][0] ), Weight2, M20 );
// 					M30	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix2.M[3][0] ), Weight2, M30 );
// 
// 					//Bone 4
// 					const FMatrix BoneMatrix3 = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_3]]].ToMatrix();
// 					VectorRegister Weight3 = VectorReplicate( Weights, INFLUENCE_3 );
// 					M00	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix3.M[0][0] ), Weight3, M00 );
// 					M10	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix3.M[1][0] ), Weight3, M10 );
// 					M20	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix3.M[2][0] ), Weight3, M20 );
// 					M30	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix3.M[3][0] ), Weight3, M30 );
// 
// 					//Transform the point
// 					VectorRegister N_xxxx = VectorReplicate( SrcPoints, 0 );
// 					VectorRegister N_yyyy = VectorReplicate( SrcPoints, 1 );
// 					VectorRegister N_zzzz = VectorReplicate( SrcPoints, 2 );
// 					DstPoints = VectorMultiplyAdd( N_xxxx, M00, VectorMultiplyAdd( N_yyyy, M10, VectorMultiplyAdd( N_zzzz, M20, M30 ) ) );
// #endif
// 					// Write to 16-byte aligned memory:
// 					VectorStore( DstPoints, TransformedPosition );
// 
// 					// Render the now skinned point sprite
// 					WorldPos = LocalToWorldMat.TransformFVector(*TransformedPosition);
// 
// 					PDI->SetHitProxy(new HVertInfluenceProxy(LODIdx, InfluenceIdx, VertexBufferIndex, bContributingVerts));
// 					PDI->DrawPoint(WorldPos, VertexColor, 5.0f, SDPG_World);
// 					PDI->SetHitProxy(NULL);
// 				}
// 			}
// 		}
// 	}
// }
// 
// /*
//  *   Skin and draw as point sprites all vertices in a skeletal mesh	not found in the given sets
//  *  @param PDI - Draw interface
//  *  @param SkelComp - Skeletal component to draw verts from
//  *  @param InfluenceVerts - Array of vertex indices not to draw
//  *  @param NonInfluenceVerts - Array of vertex indices not to draw
//  */
// void // DrawAllOtherVertexLocations(FPrimitiveDrawInterface* PDI, const USkeletalMeshComponent* SkelComp, const TSet<INT>& InfluenceVerts, const TSet<INT>& NonInfluenceVerts )
// {
// 	TSet<INT> AllOtherVerts;
// 
// 	if ( SkelComp->SkeletalMesh )
// 	{
// 		const INT LODIdx = ::Clamp(SkelComp->PredictedLODLevel, SkelComp->MinLodModel, SkelComp->SkeletalMesh->LODModels.Num()-1);
// 		const FStaticLODModel& LODModel = SkelComp->SkeletalMesh->LODModels(LODIdx);
// 		const INT NumVertices = LODModel.VertexBufferGPUSkin.GetNumVertices();
// 		for (INT VertIdx=0; VertIdx<NumVertices; VertIdx++)
// 		{
// 			if (InfluenceVerts.Contains(VertIdx) || NonInfluenceVerts.Contains(VertIdx))
// 			{
// 				continue;
// 			}
// 
// 			AllOtherVerts.Add(VertIdx);
// 		}
// 
// 		// DrawVertInfluenceLocations(PDI, SkelComp, AllOtherVerts, FLinearColor(0.f, 0.f, 1.f), FALSE);
// 	}
// }
// 
// #pragma warning(push)
// #pragma warning(disable : 4730) //mixing _m64 and floating point expressions may result in incorrect code
// 
// /** Get Chunk Index from Material Index 
//  * Go through all LOD Sections to find which section uses input Material Index 
//  * This function also considers LODMaterialMap
//  */
// INT GetChunkIndexFromMaterialIndex(USkeletalMeshComponent* SkelComp, INT MaterialIndex)
// {
// 	const INT LODIdx = ::Clamp(SkelComp->PredictedLODLevel, 0, SkelComp->SkeletalMesh->LODModels.Num()-1);
// 	FStaticLODModel& LOD = SkelComp->SkeletalMesh->LODModels(LODIdx);
// 	const FSkeletalMeshLODInfo& Info = SkelComp->SkeletalMesh->LODInfo(LODIdx);
// 
// 	INT SelectedChunkIndex=INDEX_NONE;	
// 
// 	for ( INT SectionID = 0; SectionID<LOD.Sections.Num(); ++SectionID )
// 	{
// 		const FSkelMeshSection& Section = LOD.Sections(SectionID);
// 
// 		// If we are at a dropped LOD, route material index through the LODMaterialMap in the LODInfo struct.
// 		INT UseMaterialIndex = Section.MaterialIndex;			
// 		if(Section.MaterialIndex < Info.LODMaterialMap.Num())
// 		{
// 			UseMaterialIndex = Info.LODMaterialMap(Section.MaterialIndex);
// 			UseMaterialIndex = ::Clamp( UseMaterialIndex, 0, SkelComp->SkeletalMesh->Materials.Num() );
// 		}
// 
// 		// check if used material index is same as selected mateiral index
// 		if (MaterialIndex == UseMaterialIndex)
// 		{
// 			SelectedChunkIndex = Section.ChunkIndex;
// 			break;
// 		}
// 	}
// 
// 	return SelectedChunkIndex;
// }
// 
// template<typename VertexType>
// void // DrawVertexInformation( FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* SkelComp, INT SelectedMaterialIndex, INT SelectedVertexIndex, UBOOL bDrawTangent, UBOOL bDrawNormal )
// {
// 	DWORD StatusRegister = VectorGetControlRegister();
// 	VectorSetControlRegister( StatusRegister | VECTOR_ROUND_TOWARD_ZERO );
// 
// 	const INT RigidInfluenceIndex = SkinningTools::GetRigidInfluenceIndex();
// 	FMatrix LocalToWorldMat = SkelComp->LocalToWorld;
// 
// 	//@TODO handle multiple LOD and influence tracks
// 	const INT LODIdx = ::Clamp(SkelComp->PredictedLODLevel, 0, SkelComp->SkeletalMesh->LODModels.Num()-1);
// 	const INT InfluenceIdx = 0;
// 	FStaticLODModel& LOD = SkelComp->SkeletalMesh->LODModels(LODIdx);
// 
// 	INT SelectedChunkIndex=GetChunkIndexFromMaterialIndex(SkelComp, SelectedMaterialIndex);	
// 	if (LOD.Chunks.IsValidIndex(SelectedChunkIndex) == FALSE)
// 	{
// 		return;
// 	}
// 
// 	//Calculate the current bone transforms
// 	TArray<FBoneAtom> RefToLocal;
// 	UpdateRefToLocalMatrices(RefToLocal, SkelComp, LODIdx);
// 	const FBoneAtom* RESTRICT ReferenceToLocal = RefToLocal.GetTypedData();
// 
// 	INT CachedFinalVerticesNum = LOD.NumVertices;
// 
// 	// TODO: LS
// 	if(SkelComp->SkeletalMesh && SkelComp->SkeletalMesh->bEnableClothTearing && (SkelComp->SkeletalMesh->ClothWeldingMap.Num() == 0))
// 	{
// 		CachedFinalVerticesNum += SkelComp->SkeletalMesh->ClothTearReserve;
// 	}
// 	
//  	TArray<FFinalSkinVertex> TransformedVertices;
// 	TArray<TArray<INT>> InfluencedIndicesList;
// 	TArray<TArray<FLOAT>> InfluencedWeightsList;
// 
// 	TransformedVertices.Empty(CachedFinalVerticesNum);
// 	TransformedVertices.Add(CachedFinalVerticesNum);
// 	InfluencedIndicesList.Empty(CachedFinalVerticesNum);
// 	InfluencedIndicesList.AddZeroed(CachedFinalVerticesNum);
// 	InfluencedWeightsList.Empty(CachedFinalVerticesNum);
// 	InfluencedWeightsList.AddZeroed(CachedFinalVerticesNum);
// 
// 	TArray<INT> MorphVertIndices;
// 	UINT NumValidMorphs = GetMorphVertexIndices(SkelComp->ActiveMorphs,LODIdx,MorphVertIndices);
// 	UBOOL bUseInfluences = SkelComp->LODInfo(LODIdx).bAlwaysUseInstanceWeights && LOD.VertexInfluences.Num() > 0 && LOD.VertexInfluences(InfluenceIdx).Influences.Num() > 0;
// 	INT CurBaseVertIdx = 0;
// 	// VertexCopy for morph. Need to allocate right struct
// 	// To avoid re-allocation, create 2 statics, and assign right struct
// 	VertexType  VertexCopy;
// 	if (LOD.Sections.IsValidIndex(SelectedChunkIndex))
// 	{
// 		FSkelMeshChunk& Chunk = LOD.Chunks(SelectedChunkIndex);
// 
// 		// Prefetch all bone indices
// 		WORD* BoneMap = Chunk.BoneMap.GetTypedData();
// 		PREFETCH( BoneMap );
// 		PREFETCH( BoneMap + 64 );
// 		PREFETCH( BoneMap + 128 );
// 		PREFETCH( BoneMap + 192 );
// 
// 		VertexType* SrcRigidVertex = NULL;
// 		if (Chunk.GetNumRigidVertices() > 0)
// 		{
// 			// Prefetch first vertex
// 			PREFETCH( LOD.VertexBufferGPUSkin.GetVertexPtr(Chunk.GetRigidVertexBufferIndex()) );
// 		}
// 		FFinalSkinVertex * DestVertex = TransformedVertices.GetData();
// 
// 		for(INT VertexIndex = 0;VertexIndex < Chunk.GetNumRigidVertices();VertexIndex++,DestVertex++)
// 		{
// 			INT VertexBufferIndex = Chunk.GetRigidVertexBufferIndex() + VertexIndex;
// 			SrcRigidVertex = (VertexType*)LOD.VertexBufferGPUSkin.GetVertexPtr(VertexBufferIndex);
// 			PREFETCH_NEXT_CACHE_LINE( SrcRigidVertex );	// Prefetch next vertices
// 			VertexType* MorphedVertex = SrcRigidVertex;
// 
// 			if( NumValidMorphs ) 
// 			{
// 				MorphedVertex = &VertexCopy;
// 				UpdateMorphedVertex<VertexType>( &LOD.VertexBufferGPUSkin, SkelComp->ActiveMorphs, *MorphedVertex, *SrcRigidVertex, CurBaseVertIdx, LODIdx, MorphVertIndices );
// 			}
// 
// 			VectorRegister SrcPoints[3];
// 			VectorRegister DstPoints[3];
// 			const FVector VertexPosition = LOD.VertexBufferGPUSkin.GetVertexPosition(MorphedVertex);
// 			SrcPoints[0] = VectorLoadFloat3_W1( &VertexPosition );
// 			SrcPoints[1] = Unpack3( &MorphedVertex->TangentX.Vector.Packed );
// 			SrcPoints[2] = Unpack4( &MorphedVertex->TangentZ.Vector.Packed );
// 			VectorResetFloatRegisters(); // Need to call this to be able to use regular floating point registers again after Unpack().
// 
// 			BYTE BoneIndex;
// 			if (bUseInfluences)
// 			{
// 				BoneIndex = LOD.VertexInfluences(InfluenceIdx).Influences(VertexBufferIndex).Bones.InfluenceBones[RigidInfluenceIndex];
// 			}
// 			else
// 			{
// 				BoneIndex =	MorphedVertex->InfluenceBones[RigidInfluenceIndex];
// 			}
// 
// 			InfluencedIndicesList(CurBaseVertIdx).AddItem(BoneIndex);
// 			InfluencedWeightsList(CurBaseVertIdx).AddItem(1.0f);
// 
// 			const FMatrix BoneMatrix = ReferenceToLocal[BoneMap[BoneIndex]].ToMatrix();
// 			VectorRegister M00	= VectorLoadAligned( &BoneMatrix.M[0][0] );
// 			VectorRegister M10	= VectorLoadAligned( &BoneMatrix.M[1][0] );
// 			VectorRegister M20	= VectorLoadAligned( &BoneMatrix.M[2][0] );
// 			VectorRegister M30	= VectorLoadAligned( &BoneMatrix.M[3][0] );
// 
// 			VectorRegister N_xxxx = VectorReplicate( SrcPoints[0], 0 );
// 			VectorRegister N_yyyy = VectorReplicate( SrcPoints[0], 1 );
// 			VectorRegister N_zzzz = VectorReplicate( SrcPoints[0], 2 );
// 			DstPoints[0] = VectorMultiplyAdd( N_xxxx, M00, VectorMultiplyAdd( N_yyyy, M10, VectorMultiplyAdd( N_zzzz, M20, M30 ) ) );
// 
// 			N_xxxx = VectorReplicate( SrcPoints[1], 0 );
// 			N_yyyy = VectorReplicate( SrcPoints[1], 1 );
// 			N_zzzz = VectorReplicate( SrcPoints[1], 2 );
// 			DstPoints[1] = VectorMultiplyAdd( N_xxxx, M00, VectorMultiplyAdd( N_yyyy, M10, VectorMultiply( N_zzzz, M20 ) ) );
// 
// 			N_xxxx = VectorReplicate( SrcPoints[2], 0 );
// 			N_yyyy = VectorReplicate( SrcPoints[2], 1 );
// 			N_zzzz = VectorReplicate( SrcPoints[2], 2 );
// 			DstPoints[2] = VectorMultiplyAdd( N_xxxx, M00, VectorMultiplyAdd( N_yyyy, M10, VectorMultiply( N_zzzz, M20 ) ) );
// 
// 			// carry over the W component (sign of basis determinant) 
// 			DstPoints[2] = VectorMultiplyAdd( VECTOR_0001, SrcPoints[2], DstPoints[2] );
// 
// 			// Write to 16-byte aligned memory:
// 			VectorStore( DstPoints[0], &DestVertex->Position );
// 			Pack3( DstPoints[1], &DestVertex->TangentX.Vector.Packed );
// 			Pack4( DstPoints[2], &DestVertex->TangentZ.Vector.Packed );
// 			VectorResetFloatRegisters(); // Need to call this to be able to use regular floating point registers again after Pack().
// 
// 			CurBaseVertIdx++;
// 		}
// 
// 		VertexType* SrcSoftVertex = NULL;
// 		if (Chunk.GetNumSoftVertices() > 0)
// 		{
// 			// Prefetch first vertex
// 			PREFETCH( LOD.VertexBufferGPUSkin.GetVertexPtr(Chunk.GetSoftVertexBufferIndex()) );
// 		}
// 		for(INT VertexIndex = 0;VertexIndex < Chunk.GetNumSoftVertices();VertexIndex++,DestVertex++)
// 		{
// 			const INT VertexBufferIndex = Chunk.GetSoftVertexBufferIndex() + VertexIndex;
// 			SrcSoftVertex = (VertexType*)LOD.VertexBufferGPUSkin.GetVertexPtr(VertexBufferIndex);
// 			PREFETCH_NEXT_CACHE_LINE( SrcSoftVertex );	// Prefetch next vertices
// 			VertexType* MorphedVertex = SrcSoftVertex;
// 			if( NumValidMorphs ) 
// 			{
// 				MorphedVertex = &VertexCopy;
// 				UpdateMorphedVertex<VertexType>( &LOD.VertexBufferGPUSkin, SkelComp->ActiveMorphs, *MorphedVertex, *SrcSoftVertex, CurBaseVertIdx, LODIdx, MorphVertIndices );
// 			}
// 
// 			const BYTE* RESTRICT BoneIndices;
// 			const BYTE* RESTRICT BoneWeights;
// 
// 			if (bUseInfluences)
// 			{
// 				BoneIndices = LOD.VertexInfluences(InfluenceIdx).Influences(VertexBufferIndex).Bones.InfluenceBones;
// 				BoneWeights = LOD.VertexInfluences(InfluenceIdx).Influences(VertexBufferIndex).Weights.InfluenceWeights;
// 			}
// 			else
// 			{
// 				BoneIndices = MorphedVertex->InfluenceBones;
// 				BoneWeights = MorphedVertex->InfluenceWeights;
// 
// 				for (INT I=0; I<4; ++I)
// 				{
// 					if ( BoneWeights[I] != 0 )
// 					{
// 						InfluencedIndicesList(CurBaseVertIdx).AddItem(BoneIndices[I]);
// 						FLOAT Weight = BoneWeights[I];
// 						InfluencedWeightsList(CurBaseVertIdx).AddItem(Weight/255.f);
// 					}
// 				}
// 			}
// 
// 			static VectorRegister	SrcPoints[3];
// 			VectorRegister			DstPoints[3];
// 			const FVector VertexPosition = LOD.VertexBufferGPUSkin.GetVertexPosition(MorphedVertex);
// 			SrcPoints[0] = VectorLoadFloat3_W1( &VertexPosition );
// 			SrcPoints[1] = Unpack3( &MorphedVertex->TangentX.Vector.Packed );
// 			SrcPoints[2] = Unpack4( &MorphedVertex->TangentZ.Vector.Packed );
// 			VectorRegister Weights = VectorMultiply( VectorLoadByte4(BoneWeights), VECTOR_INV_255 );
// 			VectorResetFloatRegisters(); // Need to call this to be able to use regular floating point registers again after Unpack and VectorLoadByte4.
// 
// 	#if QUAT_SKINNING
// 			// Dual Quaternion part
// 			// Linearly blend DQs
// 			FBoneQuat DualQuat;
// 			DualQuat.SetBoneAtom(ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_0]]]);
// 			VectorRegister Weight0 = VectorReplicate( Weights, INFLUENCE_0 );
// 			VectorRegister BlendDQ0	= VectorMultiply( VectorLoad( &DualQuat.DQ1[0] ), Weight0 );
// 			VectorRegister BlendDQ1	= VectorMultiply( VectorLoad( &DualQuat.DQ2[0] ), Weight0 );
// 			FLOAT Scale = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_0]]].GetScale();
// 			VectorRegister BlendScale = VectorMultiply( VectorLoadFloat1( &Scale ), Weight0 );
// 
// 			if ( Chunk.MaxBoneInfluences > 1 )
// 			{
// 				// Save first DQ0 for testing shortest route for blending
// 				VectorRegister BaseQuat = BlendDQ0;
// 				DualQuat.SetBoneAtom(ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_1]]]);
// 
// 				VectorRegister Weight1 = VectorReplicate( Weights, INFLUENCE_1 );
// 
// 				VectorRegister DQ0 = VectorLoad( &DualQuat.DQ1[0] );
// 				VectorRegister DQ1 = VectorLoad( &DualQuat.DQ2[0] );
// 
// 				// blend scale - need to be done before negate weight
// 				Scale = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_1]]].GetScale();
// 				BlendScale	= VectorMultiplyAdd( VectorLoadFloat1( &Scale ), Weight1, BlendScale );
// 
// 				// If not shortest route, negate weight
// 				if ( VectorAnyGreaterThan(VectorZero(), VectorDot4( BaseQuat, DQ0 )) )
// 				{
// 					Weight1 = VectorNegate(Weight1);
// 				}
// 
// 				BlendDQ0	= VectorMultiplyAdd( DQ0 , Weight1, BlendDQ0 );
// 				BlendDQ1	= VectorMultiplyAdd( DQ1 , Weight1, BlendDQ1 );
// 
// 				if ( Chunk.MaxBoneInfluences > 2 )
// 				{
// 					DualQuat.SetBoneAtom(ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_2]]]);
// 
// 					VectorRegister Weight2 = VectorReplicate( Weights, INFLUENCE_2 );
// 
// 					DQ0 = VectorLoad( &DualQuat.DQ1[0] );
// 					DQ1 = VectorLoad( &DualQuat.DQ2[0] );
// 
// 					// blend scale - need to be done before negate weight
// 					Scale = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_2]]].GetScale();
// 					BlendScale	= VectorMultiplyAdd( VectorLoadFloat1( &Scale ), Weight2, BlendScale );
// 
// 					// If not shortest route, negate weight
// 					if ( VectorAnyGreaterThan(VectorZero(), VectorDot4( BaseQuat, DQ0 )) )
// 					{
// 						Weight2 = VectorNegate(Weight2);
// 					}
// 
// 					BlendDQ0	= VectorMultiplyAdd( DQ0 , Weight2, BlendDQ0 );
// 					BlendDQ1	= VectorMultiplyAdd( DQ1 , Weight2, BlendDQ1 );
// 
// 					if ( Chunk.MaxBoneInfluences > 3 )
// 					{
// 						DualQuat.SetBoneAtom(ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_3]]]);
// 						VectorRegister Weight3 = VectorReplicate( Weights, INFLUENCE_3 );
// 						DQ0 = VectorLoad( &DualQuat.DQ1[0] );
// 						DQ1 = VectorLoad( &DualQuat.DQ2[0] );
// 						// blend scale - need to be done before negate weight
// 						Scale = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_3]]].GetScale();
// 						BlendScale	= VectorMultiplyAdd( VectorLoadFloat1( &Scale ), Weight3, BlendScale );
// 
// 						// If not shortest route, negate weight
// 						if ( VectorAnyGreaterThan(VectorZero(), VectorDot4( BaseQuat, DQ0 )) )
// 						{
// 							Weight3 = VectorNegate(Weight3);
// 						}
// 
// 						BlendDQ0	= VectorMultiplyAdd( DQ0 , Weight3, BlendDQ0 );
// 						BlendDQ1	= VectorMultiplyAdd( DQ1 , Weight3, BlendDQ1 );
// 					}
// 				}
// 			}
// 
// 			// Scale the position
// 			SrcPoints[0] = VectorSet_W1(VectorMultiply(SrcPoints[0], BlendScale));
// 
// 			// Normalize
// 			VectorRegister RecipLen = VectorReciprocalLen(BlendDQ0);
// 			BlendDQ0 = VectorMultiply(BlendDQ0, RecipLen);
// 			BlendDQ1 = VectorMultiply(BlendDQ1, RecipLen);
// 
// 			// Cache variables to transform
// 			VectorRegister BlendDQ0YZW = VectorSet_W0(VectorSwizzle(BlendDQ0, 1, 2, 3, 0)); 
// 			VectorRegister BlendDQ1YZW = VectorSet_W0(VectorSwizzle(BlendDQ1, 1, 2, 3, 0)); 
// 			VectorRegister BlendDQ0XXXX = VectorReplicate(BlendDQ0, 0);
// 			VectorRegister BlendDQ1XXXX = VectorReplicate(BlendDQ1, 0);
// 
// 			/* 
// 			* Dual Quaternion - http://isg.cs.tcd.ie/projects/DualQuaternions/
// 			* Convert DQ to Matrix
// 			* This is faster in our case since it calculates DQ result once and reuse it for position/tangents
// 			* If you use transform, you need to transform at least 3 times - more expensive (a lot of cross products)
// 			* DQToMatrix does not work with Scale - so I'm saving this for reference in the future
// 			*/
// 			DstPoints[0] = VectorMultiplyAdd(VECTOR_2222, VectorCross(BlendDQ0YZW, VectorMultiplyAdd(BlendDQ0XXXX, SrcPoints[0], VectorCross(BlendDQ0YZW, SrcPoints[0]))), SrcPoints[0]);
// 			VectorRegister Trans = VectorMultiplyAdd(BlendDQ0XXXX, BlendDQ1YZW, VectorAdd(VectorNegate(VectorMultiply(BlendDQ1XXXX, BlendDQ0YZW)), VectorCross(BlendDQ0YZW, BlendDQ1YZW)));
// 			DstPoints[0] = VectorMultiplyAdd(VECTOR_2222, Trans, DstPoints[0]);
// 
// 			DstPoints[1] = VectorMultiplyAdd(VECTOR_2222, VectorCross(BlendDQ0YZW, VectorMultiplyAdd(BlendDQ0XXXX, SrcPoints[1], VectorCross(BlendDQ0YZW, SrcPoints[1]))), SrcPoints[1]);
// 			DstPoints[2] = VectorMultiplyAdd(VECTOR_2222, VectorCross(BlendDQ0YZW, VectorMultiplyAdd(BlendDQ0XXXX, SrcPoints[2], VectorCross(BlendDQ0YZW, SrcPoints[2]))), SrcPoints[2]);
// 	#else
// 
// 			const FMatrix BoneMatrix0 = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_0]]].ToMatrix();
// 			VectorRegister Weight0 = VectorReplicate( Weights, INFLUENCE_0 );
// 			VectorRegister M00	= VectorMultiply( VectorLoadAligned( &BoneMatrix0.M[0][0] ), Weight0 );
// 			VectorRegister M10	= VectorMultiply( VectorLoadAligned( &BoneMatrix0.M[1][0] ), Weight0 );
// 			VectorRegister M20	= VectorMultiply( VectorLoadAligned( &BoneMatrix0.M[2][0] ), Weight0 );
// 			VectorRegister M30	= VectorMultiply( VectorLoadAligned( &BoneMatrix0.M[3][0] ), Weight0 );
// 
// 			if ( Chunk.MaxBoneInfluences > 1 )
// 			{
// 				const FMatrix BoneMatrix1 = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_1]]].ToMatrix();
// 				VectorRegister Weight1 = VectorReplicate( Weights, INFLUENCE_1 );
// 				M00	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix1.M[0][0] ), Weight1, M00 );
// 				M10	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix1.M[1][0] ), Weight1, M10 );
// 				M20	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix1.M[2][0] ), Weight1, M20 );
// 				M30	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix1.M[3][0] ), Weight1, M30 );
// 
// 				if ( Chunk.MaxBoneInfluences > 2 )
// 				{
// 					const FMatrix BoneMatrix2 = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_2]]].ToMatrix();
// 					VectorRegister Weight2 = VectorReplicate( Weights, INFLUENCE_2 );
// 					M00	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix2.M[0][0] ), Weight2, M00 );
// 					M10	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix2.M[1][0] ), Weight2, M10 );
// 					M20	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix2.M[2][0] ), Weight2, M20 );
// 					M30	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix2.M[3][0] ), Weight2, M30 );
// 
// 					if ( Chunk.MaxBoneInfluences > 3 )
// 					{
// 						const FMatrix BoneMatrix3 = ReferenceToLocal[BoneMap[BoneIndices[INFLUENCE_3]]].ToMatrix();
// 						VectorRegister Weight3 = VectorReplicate( Weights, INFLUENCE_3 );
// 						M00	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix3.M[0][0] ), Weight3, M00 );
// 						M10	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix3.M[1][0] ), Weight3, M10 );
// 						M20	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix3.M[2][0] ), Weight3, M20 );
// 						M30	= VectorMultiplyAdd( VectorLoadAligned( &BoneMatrix3.M[3][0] ), Weight3, M30 );
// 					}
// 				}
// 			}
// 
// 			VectorRegister N_xxxx = VectorReplicate( SrcPoints[0], 0 );
// 			VectorRegister N_yyyy = VectorReplicate( SrcPoints[0], 1 );
// 			VectorRegister N_zzzz = VectorReplicate( SrcPoints[0], 2 );
// 			DstPoints[0] = VectorMultiplyAdd( N_xxxx, M00, VectorMultiplyAdd( N_yyyy, M10, VectorMultiplyAdd( N_zzzz, M20, M30 ) ) );
// 
// 			DstPoints[1] = VectorZero();
// 			N_xxxx = VectorReplicate( SrcPoints[1], 0 );
// 			N_yyyy = VectorReplicate( SrcPoints[1], 1 );
// 			N_zzzz = VectorReplicate( SrcPoints[1], 2 );
// 			DstPoints[1] = VectorNormalize(VectorMultiplyAdd( N_xxxx, M00, VectorMultiplyAdd( N_yyyy, M10, VectorMultiply( N_zzzz, M20 ) ) ));
// 
// 			N_xxxx = VectorReplicate( SrcPoints[2], 0 );
// 			N_yyyy = VectorReplicate( SrcPoints[2], 1 );
// 			N_zzzz = VectorReplicate( SrcPoints[2], 2 );
// 			DstPoints[2] = VectorZero();
// 			DstPoints[2] = VectorNormalize(VectorMultiplyAdd( N_xxxx, M00, VectorMultiplyAdd( N_yyyy, M10, VectorMultiply( N_zzzz, M20 ) ) ));
// 
// 			// carry over the W component (sign of basis determinant) 
// 			DstPoints[2] = VectorMultiplyAdd( VECTOR_0001, SrcPoints[2], DstPoints[2] );
// 	#endif
// 
// 			// Write to 16-byte aligned memory:
// 			VectorStore( DstPoints[0], &DestVertex->Position );
// 			Pack3( DstPoints[1], &DestVertex->TangentX.Vector.Packed );
// 			Pack4( DstPoints[2], &DestVertex->TangentZ.Vector.Packed );
// 			VectorResetFloatRegisters(); // Need to call this to be able to use regular floating point registers again after Pack().
// 
// 			// Copy UVs.
// 			FVector2D UVs = LOD.VertexBufferGPUSkin.GetVertexUV(Chunk.GetSoftVertexBufferIndex()+VertexIndex,0);
// 			DestVertex->U = UVs.X;
// 			DestVertex->V = UVs.Y;
// 
// 			CurBaseVertIdx++;
// 		}
// 	}
// 
// 	// now render information you need
// 	FVector Tangent, Normal, Position, WorldPos;
// 	UBOOL Selected, MinusZ;
// 	FColor DrawColor;
// 	for (INT I=0; I<TransformedVertices.Num(); ++I)
// 	{
// 		// selected
// 		Selected = (I == SelectedVertexIndex);
// 	
// 		// set proxy with information
// 		Position = TransformedVertices(I).Position;
// 		WorldPos = LocalToWorldMat.TransformFVector(Position);
// 		HVertexInfoProxy * Proxy = new HVertexInfoProxy(I, WorldPos);
// 		Proxy->AddInfluences(InfluencedIndicesList(I), InfluencedWeightsList(I));
// 		PDI->SetHitProxy(Proxy);
// 
// 		// draw tangent
// 		if ( bDrawTangent )
// 		{
// 			Tangent = TransformedVertices(I).TangentX;
// 			MinusZ = (Tangent.Z <= 0.f);
// 
// 			Tangent *= 5.f; 
// 			Tangent += Position;
// 
// 			Tangent = LocalToWorldMat.TransformFVector(Tangent);
// 			
// 			if ( MinusZ )
// 			{
// 				DrawColor = FColor(0, 0, 255);
// 			}
// 			else
// 			{
// 				DrawColor = FColor(255, 0, 0);
// 			}
// 
// 			if ( Selected )
// 			{
// 				PDI->DrawLine(WorldPos, Tangent, DrawColor, SDPG_Foreground, 0.5f);
// 			}
// 			else
// 			{
// 				PDI->DrawLine(WorldPos, Tangent, DrawColor, SDPG_World);
// 			}
// 		}
// 
// 		// draw normal
// 		if ( bDrawNormal )
// 		{
// 			Normal = TransformedVertices(I).TangentZ;
// 			MinusZ = (Normal.Z <= 0.f);
// 
// 			Normal *= 5.f;
// 			Normal += Position;
// 
// 			Normal = LocalToWorldMat.TransformFVector(Normal);
// 
// 			if ( MinusZ )
// 			{
// 				DrawColor = FColor(0, 255, 0);
// 			}
// 			else
// 			{
// 				DrawColor = FColor(255, 255, 0);
// 			}
// 
// 			if ( Selected )
// 			{
// 				PDI->DrawLine(WorldPos, Normal, DrawColor, SDPG_Foreground, 0.5f);
// 			}
// 			else
// 			{
// 				PDI->DrawLine(WorldPos, Normal, DrawColor, SDPG_World);
// 			}
// 		}
// 
// 		// if selected, draw highlight point
// 		if ( I == SelectedVertexIndex )
// 		{
// 			PDI->DrawPoint(WorldPos, FColor(255, 255, 0), 10.f, SDPG_World);
// 		}
// 		else
// 		{
// 			PDI->DrawPoint(WorldPos, FColor(255, 255, 255), 3.f, SDPG_World);
// 		}
// 
// 		PDI->SetHitProxy(NULL);
// 	}
// }

//#pragma warning(pop)

FEditorAbilityViewportClient::FEditorAbilityViewportClient( WxAbilityViewer* InASV ):
	PreviewScene(FRotator(-8192,-8192,0),0.25f,1.f,TRUE)
{

	bDraggingComponent = FALSE;
	MaxTime = 4.0;
	PixelsPerSecond = 400.f;
	EditObject = NULL;


	bDraggingTrackPos = FALSE;
	DraggingNotifyIndex = -1;
	DraggingMetadataIndex = -1;
	DraggingMetadataKeyIndex = -1;

	AbilityViewer = InASV;

	// Setup defaults for the common draw helper.
	DrawHelper.bDrawPivot = FALSE;
	DrawHelper.bDrawWorldBox = FALSE;
	DrawHelper.bDrawKillZ = FALSE;
	DrawHelper.GridColorHi = FColor(80,80,80);
	DrawHelper.GridColorLo = FColor(72,72,72);
	DrawHelper.PerspectiveGridSize = 32767;

	PreviewPostProcessChain = ConstructObject<UPostProcessChain>(UPostProcessChain::StaticClass());
	PreviewPostProcessChain->AddToRoot();

	//// Create AnimNodeSequence for previewing animation playback
	//InASV->PreviewAnimNode = ConstructObject<UAnimNodeSequence>(UAnimNodeSequence::StaticClass());
	//InASV->PreviewAnimNode->bEditorOnlyAddRefPoseToAdditiveAnimation = TRUE;

	//// Morph pose node for previewing morph targets.
	//InASV->PreviewMorphPose = ConstructObject<UMorphNodeMultiPose>(UMorphNodeMultiPose::StaticClass());

	//// Finally, AnimTree to hold them all
	//InASV->PreviewAnimTree = ConstructObject<UAnimTree>(UAnimTree::StaticClass());
	//InASV->PreviewAnimTree->Children(0).Anim = InASV->PreviewAnimNode;
	////InASV->PreviewAnimTree->RootMorphNodes.AddItem(InASV->PreviewMorphPose);


	//// Create a skel control to handle bone manipulation
	//FSkelControlListHead SkelControlEntry(EC_EventParm);
	//SkelControlEntry.BoneName = NAME_None;
	
	//USkelControlSingleBone* SkelControl = ConstructObject<USkelControlSingleBone>(USkelControlSingleBone::StaticClass());

	//SkelControl->bApplyTranslation = TRUE;
	//SkelControl->bApplyRotation = TRUE;
	//SkelControl->bAddTranslation = FALSE;
	//SkelControl->bAddRotation = FALSE;
	//SkelControl->BoneTranslationSpace = BCS_BoneSpace;
	//SkelControl->BoneRotationSpace = BCS_BoneSpace;

	//SkelControlEntry.ControlHead = SkelControl;
	//InASV->PreviewAnimTree->SkelControlLists.AddItem(SkelControlEntry);

	// Create SkeletalMeshComponent for rendering skeletal mesh
	InASV->PreviewSkelComp = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass());
	UAnimTree* NewTree = ConstructObject<UAnimTree>(UAnimTree::StaticClass());
	check(NewTree);
	NewTree->Children(0).Anim = ConstructObject<UAnimNodeSequence>(UAnimNodeSequence::StaticClass());
	InASV->PreviewSkelComp->Animations = NewTree;
	InASV->PreviewSkelComp->InitAnimTree(TRUE);
	InASV->PreviewSkelComp->bUpdateSkelWhenNotRendered = TRUE;
	// Don't cache animations by default.
	InASV->PreviewSkelComp->bCacheAnimSequenceNodes = FALSE;
	PreviewScene.AddComponent(InASV->PreviewSkelComp,FMatrix::Identity);

	LineBatcher = ConstructObject<ULineBatchComponent>(ULineBatchComponent::StaticClass());
	PreviewScene.AddComponent(LineBatcher,FMatrix::Identity);

	AbilityAttackRangeLineBatch = ConstructObject<ULineBatchComponent>(ULineBatchComponent::StaticClass());
	PreviewScene.AddComponent(AbilityAttackRangeLineBatch, FMatrix::Identity);


	// target skeletalmesh
	InASV->PreviewSkelCompTarget = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass());
	InASV->PreviewSkelCompTarget->AddToRoot();
	NewTree = ConstructObject<UAnimTree>(UAnimTree::StaticClass());
	check(NewTree);
	NewTree->Children(0).Anim = ConstructObject<UAnimNodeSequence>(UAnimNodeSequence::StaticClass());
	InASV->PreviewSkelCompTarget->Animations = NewTree;
	InASV->PreviewSkelCompTarget->InitAnimTree(TRUE);
	InASV->PreviewSkelCompTarget->bUpdateSkelWhenNotRendered = TRUE;
	InASV->PreviewSkelCompTarget->bCacheAnimSequenceNodes = FALSE;
	PreviewScene.AddComponent(InASV->PreviewSkelCompTarget,FMatrix::Identity);

	//InASV->PreviewSkelCompAux3 = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass());
	//InASV->PreviewSkelCompAux3->ParentAnimComponent = InASV->PreviewSkelComp;
	//InASV->PreviewSkelCompAux3->WireframeColor = ExtraMesh3WireColor;
	//InASV->PreviewSkelCompAux3->bUseOnePassLightingOnTranslucency = TRUE;
	//PreviewScene.AddComponent(InASV->PreviewSkelCompAux3,FMatrix::Identity);

	ShowFlags = SHOW_DefaultEditor;

	// Set the viewport to be fully lit.
	ShowFlags &= ~SHOW_ViewMode_Mask;
	ShowFlags |= SHOW_ViewMode_Lit;

	bShowBoneNames = FALSE;
	bShowFloor = FALSE;
	bShowSockets = FALSE;
	bShowMorphKeys = FALSE;

	bManipulating = FALSE;
	SocketManipulateAxis = AXIS_None;
	BoneManipulateAxis = AXIS_None;
	DragDirX = 0.f;
	DragDirY = 0.f;
	WorldManDir = FVector(0.f);
	LocalManDir = FVector(0.f);

	NearPlane = 1.0f;

	SetRealtime( TRUE );

	bAllowMayaCam = FALSE;

	bTriangleSortMode = FALSE;

	// Clothing may have been created in another RBPhysScene; release it so it can be created in the right one
	InASV->PreviewSkelComp->ReleaseApexClothing();
	InASV->PreviewSkelComp->InitApexClothing(InASV->RBPhysScene);

	// Override global realtime audio settings
	bAudioRealtimeOverride = TRUE;
	bWantAudioRealtime = FALSE;

#if WITH_SIMPLYGON
	SimplygonLogo = LoadObject<UTexture2D>(NULL, TEXT("EditorResources.SimplygonLogo"), NULL, LOAD_None, NULL);
#endif // #if WITH_SIMPLYGON
}

FEditorAbilityViewportClient::~FEditorAbilityViewportClient()
{
}

void FEditorAbilityViewportClient::Serialize(FArchive& Ar)
{ 
	Ar << Input; 
	Ar << PreviewScene;
}

FLinearColor FEditorAbilityViewportClient::GetBackgroundColor()
{
	return FColor(64,64,64);
}

// /*
//  *   Draw the rotation/translation delta during widget manipulation
//  *	@Canvas - canvas to drawn on
//  *	@XPos - Screen space x coord
//  *	@YPos - Screen space y coord
//  *	@ManipAxis - axis currently being manipulated
//  *	@MoveMode - movement mode (rotation/translation)
//  *	@Rotation - rotation delta
//  *	@Translation - translation delta
//  */
// void DrawAngles(FCanvas* Canvas, INT XPos, INT YPos, EAxis ManipAxis, EASVSocketMoveMode MoveMode, const FRotator& Rotation, const FVector& Translation)
// {
// 	FString OutputString(TEXT(""));
// 	if (MoveMode == SMM_Rotate && Rotation.IsZero() == FALSE)
// 	{
// 		//Only one value moves at a time
// 		const FVector EulerAngles = Rotation.Euler();
// 		if (ManipAxis == AXIS_X)
// 		{
// 			OutputString += FString::Printf(TEXT("Roll: %0.2f"), EulerAngles.X);
// 		}
// 		else if (ManipAxis == AXIS_Y)
// 		{
// 			OutputString += FString::Printf(TEXT("Pitch: %0.2f"), EulerAngles.Y);
// 		}
// 		else if (ManipAxis == AXIS_Z)
// 		{
// 			OutputString += FString::Printf(TEXT("Yaw: %0.2f"), EulerAngles.Z);
// 		}
// 	}
// 	else if (MoveMode == SMM_Translate && Translation.IsZero() == FALSE)
// 	{
// 		//Only one value moves at a time
// 		if (ManipAxis == AXIS_X)
// 		{
// 			OutputString += FString::Printf(TEXT(" %0.2f"), Translation.X);
// 		}
// 		else if (ManipAxis == AXIS_Y)
// 		{
// 			OutputString += FString::Printf(TEXT(" %0.2f"), Translation.Y);
// 		}
// 		else if (ManipAxis == AXIS_Z)
// 		{
// 			OutputString += FString::Printf(TEXT(" %0.2f"), Translation.Z);
// 		}
// 	}
// 
// 	if (OutputString.Len() > 0)
// 	{
// 		DrawString(Canvas, XPos, YPos, *OutputString, GEngine->SmallFont, FLinearColor::White);
// 	}
// }


#define HITPROXY_SOCKET	1
#define HITPROXY_BONE   2
void FEditorAbilityViewportClient::Draw(const FSceneView* View,FPrimitiveDrawInterface* PDI)
{
	// Draw common controls.
	DrawHelper.Draw(View, PDI);

	// Generate matrices for all sockets if we are drawing sockets.
	//if(bShowSockets)
	//{
	//	for(INT i=0; i<AbilityViewer->SelectedSkelMesh->Sockets.Num(); i++)
	//	{
	//		USkeletalMeshSocket* Socket = AbilityViewer->SelectedSkelMesh->Sockets(i);

	//		FMatrix SocketTM;
	//		if( Socket->GetSocketMatrix(SocketTM, AbilityViewer->PreviewSkelComp) )
	//		{
	//			PDI->SetHitProxy( new HASVSocketProxy(i) );
	//			DrawWireDiamond(PDI, SocketTM, 2.f, FColor(255,128,128), SDPG_Foreground );
	//			PDI->SetHitProxy( NULL );

	//			// Draw widget for selected socket if Socket Manager is up.
	//			if(AbilityViewer->SocketMgr && AbilityViewer->SelectedSocket && AbilityViewer->SelectedSocket == Socket)
	//			{
	//				const EAxis HighlightAxis = SocketManipulateAxis;

	//				// Info1 is just an int to determine if its coming from socket manager or bone manipulation, Info2 unused
	//				if(AbilityViewer->SocketMgr->MoveMode == SMM_Rotate)
	//				{
	//					FUnrealEdUtils::DrawWidget(View, PDI, SocketTM, HITPROXY_SOCKET, 0, HighlightAxis, WMM_Rotate);
	//				}
	//				else
	//				{
	//					FUnrealEdUtils::DrawWidget(View, PDI, SocketTM, HITPROXY_SOCKET, 0, HighlightAxis, WMM_Translate);
	//				}
	//			}
	//		}
	//	}
	//}
	//
 //   if (AbilityViewer->SelectedSocket == NULL && AbilityViewer->PreviewSkelComp && AbilityViewer->PreviewSkelComp->BonesOfInterest.Num() == 1)
	//{
	//	const EAxis HighlightAxis = BoneManipulateAxis;
	//	FMatrix BoneMatrix = AbilityViewer->PreviewSkelComp->GetBoneMatrix(AbilityViewer->PreviewSkelComp->BonesOfInterest(0));
	//	// Info1 is just an int to determine if its coming from socket manager or bone manipulation, Info2 unused
	//	if(AbilityViewer->BoneMoveMode == SMM_Rotate)
	//	{
	//		FUnrealEdUtils::DrawWidget(View, PDI, BoneMatrix, HITPROXY_BONE, 0, HighlightAxis, WMM_Rotate);
	//	}
	//	else
	//	{
	//		FUnrealEdUtils::DrawWidget(View, PDI, BoneMatrix, HITPROXY_BONE, 0, HighlightAxis, WMM_Translate);
	//	}
	//}

	//if (AbilityViewer->PreviewSkelComp->bDrawBoneInfluences)
	//{
	//	const WxAbilityViewer::FLODInfluenceInfo& LODInfluenceInfo = AbilityViewer->BoneInfluenceLODInfo(AbilityViewer->PreviewSkelComp->PredictedLODLevel);
 //       //Draw the influenced swapped set
	//	// DrawVertInfluenceLocations(PDI, AnimSetViewer->PreviewSkelComp, LODInfluenceInfo.InfluencedBoneVerts, FLinearColor::White, TRUE);
	//	//Draw the non-swapped influence set
	//	// DrawVertInfluenceLocations(PDI, AnimSetViewer->PreviewSkelComp, LODInfluenceInfo.NonInfluencedBoneVerts, FLinearColor(1.0f, 0.0f, 0.0f), FALSE);
	//	//Draw all other vertices
	//	if (AbilityViewer->bDrawAllBoneInfluenceVertices)
	//	{
	//	   // DrawAllOtherVertexLocations(PDI, AnimSetViewer->PreviewSkelComp, LODInfluenceInfo.InfluencedBoneVerts, LODInfluenceInfo.NonInfluencedBoneVerts);
	//	}
	//}

	//if (AnimSetViewer->VertexInfo.bShowTangent || AnimSetViewer->VertexInfo.bShowNormal)
	//{
	//	//Draw the influenced swapped set
	//	if ( AnimSetViewer->PreviewSkelComp->SkeletalMesh->bUseFullPrecisionUVs )
	//	{
	//		// DrawVertexInformation<TGPUSkinVertexFloat32Uvs<>>(PDI, AnimSetViewer->PreviewSkelComp, AnimSetViewer->VertexInfo.SelectedMaterialIndex, AnimSetViewer->VertexInfo.SelectedVertexIndex, AnimSetViewer->VertexInfo.bShowTangent, AnimSetViewer->VertexInfo.bShowNormal);
	//	}
	//	else
	//	{
	//		// DrawVertexInformation<TGPUSkinVertexFloat16Uvs<>>(PDI, AnimSetViewer->PreviewSkelComp, AnimSetViewer->VertexInfo.SelectedMaterialIndex, AnimSetViewer->VertexInfo.SelectedVertexIndex, AnimSetViewer->VertexInfo.bShowTangent, AnimSetViewer->VertexInfo.bShowNormal);
	//	}
	//}

	//if(AnimSetViewer->bShowClothMovementScale)
	//{
	//	// DrawClothMovementDistanceScale(PDI, AnimSetViewer->PreviewSkelComp);
	//}

	FEditorLevelViewportClient::Draw(View, PDI);
}

//struct TrackSlots
//{
//	struct Slot
//	{
//		int X, W;
//	};
//	TArray<Slot> Slots;
//
//	bool AddSlot(int X, int W)
//	{
//		for (INT Idx = 0; Idx < Slots.Num(); Idx++)
//		{
//			if (X >= Slots(Idx).X && X < Slots(Idx).X + Slots(Idx).W)
//			{
//				return FALSE;
//			}
//			if (Slots(Idx).X >= X && Slots(Idx).X < X + W)
//			{
//				return FALSE;
//			}
//		}
//		INT Idx = Slots.AddZeroed();
//		Slots(Idx).X = X;
//		Slots(Idx).W = W;
//		return TRUE;
//	}
//};

struct HNotifyProxy : public HHitProxy
{
	DECLARE_HIT_PROXY( HNotifyProxy, HHitProxy );
	INT NotifyIdx;
	UBOOL bTail;
	HNotifyProxy(INT InNotifyIdx, UBOOL InbTail = FALSE)
		:	HHitProxy( HPP_UI )
		,	NotifyIdx( InNotifyIdx )
		,	bTail( InbTail )
	{}
};


struct HAbilityComponentProxy : public HHitProxy
{
	DECLARE_HIT_PROXY(HAbilityComponentProxy, HHitProxy);
	INT ComponentIdx;
	AbilityStateType StateType;
	AbilityComponentProxyType Type;
	UBOOL bTail;
	HAbilityComponentProxy(AbilityComponentProxyType InType = EACPT_Component, INT Idx = 0, AbilityStateType InStateType = EAST_Spell, UBOOL InbTail = FALSE)
		: HHitProxy(HPP_UI)
		, Type(InType)
		, ComponentIdx(Idx)
		, StateType(InStateType)
		, bTail(InbTail)
	{
	}
};

struct HBoneControlProxy : public HHitProxy
{
	DECLARE_HIT_PROXY( HBoneControlProxy, HHitProxy );
	INT ControlModifierID;
	INT TimeModifierID;
	HBoneControlProxy(INT InControlModifierID, INT InTimeModifierID)
		:	HHitProxy( HPP_UI )
		,	ControlModifierID( InControlModifierID )
		,	TimeModifierID( InTimeModifierID )
	{}
};

struct HTrackPosProxy : public HHitProxy
{
	DECLARE_HIT_PROXY( HTrackPosProxy, HHitProxy );
	HTrackPosProxy()
		:	HHitProxy( HPP_UI )
	{}
};



/*-----------------------------------------------------------------------------
FEditorAbilityViewportClient
-----------------------------------------------------------------------------*/

static FVector2D ClampUVRange(FLOAT U, FLOAT V)
{
	// This handles both the + and - values correctly by putting all values between positive 0 and 1
	if(abs(U) > 1.0f)
	{
		U -= appFloor(U);
	}

	if(abs(V) > 1.0f)
	{
		V -= appFloor(V);
	}

	return FVector2D(U, V);
}

FString FEditorAbilityViewportClient::GetResName(FComponentNode* node)
{
	FString Ret(L"No Data");
	if (node->Component != NULL)
	{
		if (node->Component->IsA(UAbilitySoundCueComponent::StaticClass()))
		{
			UAbilitySoundCueComponent* comp = (UAbilitySoundCueComponent*)(node->Component);
			if (comp->SoundPlayType == ESPT_CustomSound)
			{
				if (comp->Sounds.Num() > 0)
				{
					Ret = comp->Sounds(0)->GetName();
				}
			}
			else
			{
				if (comp->PreviewSound)
				{
					Ret = comp->PreviewSound->GetName();
				}
			}
		}
		else if (node->Component->IsA(UAbilityEmitterComponent::StaticClass()))
		{
			UAbilityEmitterComponent* comp = (UAbilityEmitterComponent*)(node->Component);
			if (comp->ParticleSystemTemplate)
			{
				Ret = comp->ParticleSystemTemplate->GetName();
			}
		}
		else if (node->Component->IsA(UAbilityDecalComponent::StaticClass()))
		{
			UAbilityDecalComponent* comp = (UAbilityDecalComponent*)(node->Component);
			if (comp->ImpactDecalMaterials.Num() > 0 && comp->ImpactDecalMaterials(0))
			{
				Ret = comp->ImpactDecalMaterials(0)->GetName();
			}
		}
		else if (node->Component->IsA(UCameraShakeComponent::StaticClass()))
		{
			UCameraShakeComponent* comp = (UCameraShakeComponent*)(node->Component);
			if (comp->AnimToPlay)
			{
				Ret = comp->AnimToPlay->GetName();
			}
		}
		else if (node->Component->IsA(UAbilityHitPointComponent::StaticClass()))
		{
			Ret = FString(L"HitPoint");
		}
	}
	return Ret;
}

void FEditorAbilityViewportClient::DrawAbilityStateTime(FCanvas* InCanvas,
	INT InBaseX, 
	INT InSizeX, 
	INT& CurrentY,
	INT InTrackLineHalfHeight,
	FLOAT  InPixelsPerSecond,
	FLinearColor& VisibleColor
	)
{
	UMMOAbility* ability = AbilityViewer->AbilityObject;
	if (!ability)
	{
		return;
	}

	UBOOL bVisible = TRUE;
	UBOOL bEditable = TRUE; //

	FLinearColor EditableColor = FLinearColor(0.9294, 0.1098, 0.1412);

	FLinearColor UseColor = bEditable ? EditableColor : VisibleColor;
	INT SpellBaseX = InBaseX, CastBaseX = InBaseX, SpellLen = 0, CastLen = 0, MaxLen = 0;
	INT StringWidth = 0, StringHeight = 0;

	FLOAT SpellTime = 0.f, CastTime = 0.f, FixTime = 0.f, MaxTime = 0.f;
	INT CycleNumber = 1;

	FLOAT TimeFactor = AbilityViewer->GetWeaponTimeFactor();
	SpellTime = AbilityViewer->GetAbilitySpellTime();
	CastTime = AbilityViewer->GetAbilityCastTime(TRUE);
	
	MaxTime = SpellTime + CastTime;

	SpellLen = InPixelsPerSecond * SpellTime;
	CastLen = InPixelsPerSecond * CastTime;
	MaxLen = SpellLen + CastLen;

	SpellBaseX = InBaseX;
	CastBaseX = SpellBaseX + SpellLen;

	//if (ability->SpellCommonAttribute.PrepareType == ES_PT_Type4)
	//{
	//	CastBaseX = InBaseX;
	//	SpellLen = 0;
	//}
	//else
	//{
	//	CastBaseX = SpellBaseX + SpellLen;
	//}
	

	//base line
	DrawLine2D(InCanvas, FVector2D(SpellBaseX, CurrentY), FVector2D(SpellBaseX + SpellLen + CastLen, CurrentY), UseColor);
	DrawLine2D(InCanvas, FVector2D(SpellBaseX, CurrentY), FVector2D(SpellBaseX, CurrentY - InTrackLineHalfHeight*1.5), UseColor);
	DrawLine2D(InCanvas, FVector2D(SpellBaseX+SpellLen, CurrentY), FVector2D(SpellBaseX + SpellLen, CurrentY - InTrackLineHalfHeight*1.5), UseColor);
	DrawLine2D(InCanvas, FVector2D(SpellBaseX+SpellLen + CastLen, CurrentY), FVector2D(SpellBaseX + SpellLen + CastLen, CurrentY - InTrackLineHalfHeight*1.5), UseColor);

	StringSize( GEngine->SmallFont,  StringWidth, StringHeight, L"Ability State");
	INT tx = SpellBaseX + (SpellLen + CastLen)/2 - StringWidth*0.5;
	INT ty = CurrentY + 1;
	DrawString(InCanvas, tx, ty, L"Ability State", GEngine->SmallFont, UseColor);

	TArray<TrackSlots> Tracks;
	INT BoxX, BoxW, TrackIdx = -1, MiniY = CurrentY;
	FVector2D BoxStart, BoxEnd;
	FString Name;
	

	for (INT i = 0; i < 2; ++i)
	{
		if (i == 0)
		{
			bEditable =  AbilityViewer->bEditSpellState; 
		}
		else
		{
			bEditable =  !AbilityViewer->bEditSpellState; 
		}
		UseColor = bEditable ? EditableColor : VisibleColor;

		if (i == 0 && ability->SpellCommonAttribute.PrepareType == ES_PT_Type4)
		{
			continue;
		}

		Name = i ? FString(L"Cast") : FString(L"Spell");

		StringSize( GEngine->SmallFont,  StringWidth, StringHeight, *Name);
		BoxX = i ? CastBaseX : SpellBaseX;
		BoxW = i ? StringWidth + CastLen : StringWidth + SpellLen;
		TrackIdx = -1;
		for (INT j = 0; j < Tracks.Num(); j++)
		{
			if (Tracks(j).AddSlot(BoxX,BoxW))
			{
				TrackIdx = j;
				break;
			}
		}
		if (TrackIdx == -1)
		{
			// add new track, no room on existing ones
			TrackIdx = Tracks.AddItem(TrackSlots());
			Tracks(TrackIdx).AddSlot(BoxX,BoxW);
		}
		INT PosY = CurrentY - InTrackLineHalfHeight * 1.5 - StringHeight*0.5;
		PosY -= (StringHeight + 10) * (TrackIdx);

		if (PosY < MiniY)
		{
			MiniY = PosY;
		}

		INT PosX = i ? CastBaseX : SpellBaseX;
		INT Len = i ? CastLen : SpellLen;

		BoxStart.X = PosX - StringWidth*0.5;
		BoxStart.Y = PosY - StringHeight*0.5;
		BoxEnd.X = PosX + StringWidth*0.5;
		BoxEnd.Y = PosY + StringHeight*0.5;
		FLinearColor TempColor = UseColor;
		if (bEditable)
		{
			if (ComponentIdx == 0 && Type == EACPT_AbilityTime && StateType == AbilityStateType(i))
			{
				TempColor *= abs(FLOAT(GetTickCount()%2000)/1000.f - 1.0)*3.0 + 0.1f;
			}
		}
		DrawBox2D(InCanvas, BoxStart, BoxEnd, TempColor);
		DrawString(InCanvas, BoxStart.X, BoxStart.Y, *Name, GEngine->SmallFont, TempColor);
		DrawLine2D(InCanvas, FVector2D(BoxStart.X + StringWidth, PosY), FVector2D(BoxStart.X + Len, PosY), UseColor);
		if (bEditable)
		{
			if (InCanvas->IsHitTesting())
			{
				InCanvas->SetHitProxy(new HAbilityComponentProxy(EACPT_AbilityTime, 0, AbilityStateType(i), FALSE));
			}
		}
		BoxStart.X += Len;
		BoxEnd.X += Len;
		DrawBox2D(InCanvas, BoxStart, BoxEnd, TempColor);
		DrawString(InCanvas, BoxStart.X, BoxStart.Y, *Name, GEngine->SmallFont, TempColor);

		if (bEditable)
		{
			if (InCanvas->IsHitTesting())
			{
				InCanvas->SetHitProxy(NULL);
			}
		}

	}

	CurrentY = MiniY - StringHeight - InTrackLineHalfHeight;
	// draw cast time
}

ComponentTrackInfo* FEditorAbilityViewportClient::GetTrackInfo(FString InClassName)
{
	for (INT i = 0; i < TrackInfos.Num(); ++i)
	{
		if (TrackInfos(i).ClassName == InClassName)
		{
			return &TrackInfos(i);
		}
	}
	return NULL;
}

void FEditorAbilityViewportClient::SetCameraPosition(INT InPositionType)
{
	PositionType = InPositionType;
	CameraStartTime = appSeconds();
}

void FEditorAbilityViewportClient::SetCameraPosition(FVector& InLocation, FRotator& InRotation)
{
	TeleportViewportCamera(InLocation, InRotation);
}


void FEditorAbilityViewportClient::DrawComponents(FCanvas* InCanvas, INT InBaseX, INT InBaseY)
{
	UMMOAbility* ability = AbilityViewer->AbilityObject;
	if (!ability)
	{
		return;
	}

	// build track infos
	for (INT i = 0; i < TrackInfos.Num(); ++i)
	{
		TrackInfos(i).Tracks.Empty();
	}
	TrackInfos.Empty();

	TArray<FComponentNode>* SpellList = NULL;
	TArray<FComponentNode>* CastList = NULL;
	UBOOL bFound = ability->GetEffectComponentList(AbilityViewer->CurrentWeaponType, &SpellList, &CastList);
	if (bFound == FALSE)
	{
		return;
	}

	for (INT i = 0; i < SpellList->Num(); ++i)
	{
		if ((*SpellList)(i).Component)
		{
			ComponentTrackInfo* TrackInfo = GetTrackInfo((*SpellList)(i).Component->GetClass()->GetName());
			if (!TrackInfo && AbilityViewer->IsComponentNeedToDraw((*SpellList)(i).Component))
			{
				ComponentTrackInfo NewNode;
				NewNode.ClassName = (*SpellList)(i).Component->GetClass()->GetName();
				TrackInfos.AddItem(NewNode);
			}
		}
	}

	for (INT i = 0; i < CastList->Num(); ++i)
	{
		if ((*CastList)(i).Component)
		{
			ComponentTrackInfo* TrackInfo = GetTrackInfo((*CastList)(i).Component->GetClass()->GetName());
			if (!TrackInfo && AbilityViewer->IsComponentNeedToDraw((*CastList)(i).Component))
			{
				ComponentTrackInfo NewNode;
				NewNode.ClassName = (*CastList)(i).Component->GetClass()->GetName();
				TrackInfos.AddItem(NewNode);
			}
		}
	}

	INT PosY = InBaseY;
	INT TrackHeight = 10;
	INT StringWidth = 10, StringHeight = 10;
	INT AbilityLast = PixelsPerSecond * AbilityViewer->GetAbilityLastTime();

	FLinearColor DisableColor(0.9, 0.9, 0.9, 1.0);
	FLinearColor UseColor = DisableColor;
	FLinearColor DynamicColor = DisableColor;

	for (INT i = 0; i < TrackInfos.Num(); ++i)
	{
		ComponentTrackInfo* TrackInfo = &TrackInfos(i);
		INT MaxHeight = 0;

		INT PosX, PosDistance;
		// spell components first, then cast components
		for (INT i = 0; i < 2; ++i)
		{
			if (i == 0)
			{
				PosX = InBaseX;
				PosDistance = PixelsPerSecond * AbilityViewer->GetAbilitySpellTime();
			}
			else
			{
				PosX = InBaseX + PixelsPerSecond * AbilityViewer->GetAbilitySpellTime();
				PosDistance = PixelsPerSecond * AbilityViewer->GetAbilityCastTime();
			}

			INT Idx = -1;
			while(TRUE)
			{
				++Idx;

				if (i == 0)
				{
					if (Idx >= SpellList->Num())
					{
						break;
					}
				}
				else
				{
					if (Idx >= CastList->Num())
					{
						break;
					}
				}

				FComponentNode* Node = NULL;
				if (i == 0)
				{
					Node = &((*SpellList)(Idx));
				}
				else
				{
					Node = &((*CastList)(Idx));
				}

				if (Node->Component == NULL)
				{
					continue;
				}

				if (i == 0 && AbilityViewer->bEditSpellState)
				{
					UseColor = Node->Component->LineColor;
					DynamicColor = UseColor;
					if (ComponentIdx == Idx)
					{
						DynamicColor *= abs(FLOAT(GetTickCount()%2000)/1000.f - 1.0)*3.0 + 0.1f;
					}
				}
				else if (i == 1 && !AbilityViewer->bEditSpellState)
				{
					UseColor = Node->Component->LineColor;
					DynamicColor = UseColor;
					if (ComponentIdx == Idx)
					{
						DynamicColor *= abs(FLOAT(GetTickCount()%2000)/1000.f - 1.0)*3.0 + 0.1f;
					}
				}
				else
				{
					UseColor = DisableColor;
					DynamicColor = UseColor;
				}

				if (Node->Component->GetClass()->GetName() != TrackInfo->ClassName)
				{
					continue;
				}

				if (TrackInfo->Tracks.Num() == 0)
				{
					DrawLine2D(InCanvas, FVector2D(InBaseX, PosY - TrackHeight), FVector2D(InBaseX, PosY), UseColor);
					DrawLine2D(InCanvas, FVector2D(InBaseX, PosY), FVector2D(InBaseX + PosDistance, PosY), UseColor);
					DrawLine2D(InCanvas, FVector2D(InBaseX + PosDistance, PosY - TrackHeight), FVector2D(InBaseX + PosDistance, PosY), UseColor);

					DrawLine2D(InCanvas, FVector2D(InBaseX + PosDistance, PosY - TrackHeight), FVector2D(InBaseX + PosDistance, PosY), UseColor);
					DrawLine2D(InCanvas, FVector2D(InBaseX + PosDistance, PosY), FVector2D(InBaseX + AbilityLast, PosY), UseColor);
					DrawLine2D(InCanvas, FVector2D(InBaseX + AbilityLast, PosY - TrackHeight), FVector2D(InBaseX + AbilityLast, PosY), UseColor);
					FString& Title = Node->Component->TitleString;
					StringSize( GEngine->MediumFont,  StringWidth, StringHeight, *Title);
					// DrawString(Canvas, tx, ty, L"Animation", GEngine->SmallFont, UseColor);
					DrawString(InCanvas, InBaseX - StringWidth - 2, PosY - StringHeight * 0.5, *Title, GEngine->MediumFont, UseColor);
				}

				//TArray<TrackSlots> Tracks;
				INT StartX, StartY, EndX;
				FString ResName = Node->Component->GetResName();
				StringSize(GEngine->SmallFont, StringWidth, StringHeight, *ResName);
				StartX = PosX + Node->StartTime * PixelsPerSecond;

				FLOAT ResLen = 0.f;
				if (Node->Component->StopType == ACBT_StopWhenConstDurationPass)
				{
					ResLen = Node->Component->GetResLength(AbilityViewer->PreviewSkelComp);
				}
				else if (Node->Component->StopType == ACBT_StopWhenAbilityStateEnd)
				{
					if (i == 0)	// spell
					{
						ResLen = AbilityViewer->GetAbilitySpellTime() - Node->StartTime;
					}
					else
					{
						ResLen = AbilityViewer->GetAbilityCastTime() - Node->StartTime;
					}
				}
				else// if (Node->Component->StopType == ACBT_StopWhenAbilityEnd)
				{
					if (i == 0)
					{
						ResLen = AbilityViewer->GetAbilitySpellTime() - Node->StartTime + AbilityViewer->GetAbilityCastTime(TRUE);
					}
					else
					{
						ResLen = AbilityViewer->GetAbilityCastTime(TRUE)  - Node->StartTime;
					}
				}
				EndX = StartX + PixelsPerSecond * ResLen;

				INT BoxX = StartX - StringWidth * 0.5;
				INT BoxW = StringWidth + PixelsPerSecond * ResLen;
				INT TrackIdx = -1;
				for (INT k = 0; k < TrackInfo->Tracks.Num(); k++)
				{
					if (TrackInfo->Tracks(k).AddSlot(BoxX,BoxW))
					{
						TrackIdx = k;
						break;
					}
				}
				if (TrackIdx == -1)
				{
					// add new track, no room on existing ones
					TrackIdx = TrackInfo->Tracks.AddItem(TrackSlots());
					TrackInfo->Tracks(TrackIdx).AddSlot(BoxX,BoxW);
				}

				StartY = PosY - TrackHeight * 1.5 - (TrackIdx + 1) * (StringHeight+10);

				if (TrackHeight * 1.5 + (TrackIdx + 2) * (StringHeight+10) > MaxHeight)
				{
					MaxHeight = TrackHeight * 1.5 + (TrackIdx + 2) * (StringHeight+10);
				}

				DrawLine2D(InCanvas, FVector2D(StartX, PosY), FVector2D(StartX, StartY), DynamicColor);

				if (InCanvas->IsHitTesting())
				{
					InCanvas->SetHitProxy(new HAbilityComponentProxy(EACPT_Component, Idx, AbilityStateType(i), FALSE));
				}

				DrawBox2D(InCanvas, FVector2D(StartX - StringWidth * 0.5, StartY - StringHeight), FVector2D(StartX + StringWidth * 0.5, StartY), DynamicColor);
				DrawString(InCanvas, StartX - StringWidth * 0.5, StartY - StringHeight, *ResName, GEngine->SmallFont, DynamicColor);

				if (InCanvas->IsHitTesting())
				{
					InCanvas->SetHitProxy(NULL);
				}

				if (Node->Component->bHaveTrail)
				{
					DrawLine2D(InCanvas, FVector2D(EndX, PosY), FVector2D(EndX, StartY), DynamicColor);
					if (InCanvas->IsHitTesting())
					{
						InCanvas->SetHitProxy(new HAbilityComponentProxy(EACPT_Component, Idx, AbilityStateType(i), TRUE));
					}
					DrawBox2D(InCanvas, FVector2D(EndX - StringWidth * 0.5, StartY - StringHeight), FVector2D(EndX + StringWidth * 0.5, StartY), DynamicColor);
					DrawString(InCanvas, EndX - StringWidth * 0.5, StartY - StringHeight, *ResName, GEngine->SmallFont, DynamicColor);
					if (InCanvas->IsHitTesting())
					{
						InCanvas->SetHitProxy(NULL);
					}

					DrawLine2D(InCanvas, FVector2D(StartX + StringWidth * 0.5, StartY - StringHeight * 0.5), FVector2D(EndX - StringWidth * 0.5,StartY - StringHeight * 0.5), DynamicColor);
				}
	
			}
		}

		PosY -= MaxHeight + 15;
	}
}



void FEditorAbilityViewportClient::Draw(FViewport* Viewport, FCanvas* Canvas)
{
	// Do main viewport drawing stuff.
	FEditorLevelViewportClient::Draw(Viewport, Canvas);

	// draw ability components
	
	const INT SizeX = Viewport->GetSizeX();
	const INT SizeY = Viewport->GetSizeY();

	UBOOL bEditable = TRUE, bVisible = TRUE;

	INT StringWidth = 0, StringHeight = 0;
	StringSize( GEngine->SmallFont,  StringWidth, StringHeight, TEXT("Ability State Time") );

	INT TrackLineHalfHeight = 5;

	INT BaseY = SizeY - 2 - (StringHeight > TrackLineHalfHeight ? StringHeight : TrackLineHalfHeight);
	INT CurrentY = BaseY;

	INT BaseX = 80;
	INT MiniBoxSize = 10;

	INT Offset = 0;

	//INT BoxX, BoxW;
	//TArray<TrackSlots> Tracks;

	FLinearColor VisibleColor(0.686, 0.686, 0.686);
	FLOAT HighLineFactor = 1.5;
	FLinearColor EditableColor, UseColor;
	FLOAT ActiveColorFactor = 1.5;

	FVector2D BoxStart, BoxEnd;

	UMMOAbility* ability = AbilityViewer->AbilityObject;
	if (!ability)
	{
		return;
	}

	AbilityViewer->MaxTrackLength = AbilityViewer->GetAbilitySpellTime() + AbilityViewer->GetAbilityCastTime(TRUE) + 1.5f;

	PixelsPerSecond = (FLOAT)(SizeX - (2*BaseX))/AbilityViewer->MaxShowTrackLength;

	FLOAT tx = 0, ty = 0;


	// draw base time track
	CurrentY = BaseY;

	FLinearColor BaseTrackColor(1.0, 1.0, 1.0);
	DrawLine2D(Canvas, FVector2D(BaseX, CurrentY - TrackLineHalfHeight), FVector2D(BaseX, CurrentY + TrackLineHalfHeight), BaseTrackColor);
	DrawLine2D(Canvas, FVector2D(SizeX - BaseX, CurrentY - TrackLineHalfHeight), FVector2D(SizeX - BaseX, CurrentY + TrackLineHalfHeight), BaseTrackColor);
	DrawLine2D(Canvas, FVector2D(BaseX, CurrentY), FVector2D(SizeX - BaseX, CurrentY), BaseTrackColor);
	for (FLOAT i = 0; i <= AbilityViewer->MaxTrackLength; i += 1)
	{
		INT PosX = PixelsPerSecond * i + BaseX;
		DrawLine2D(Canvas, FVector2D(PosX, CurrentY), FVector2D(PosX, CurrentY - TrackLineHalfHeight*0.7), BaseTrackColor);
		FString TimeText = FString::Printf(L"%d sec", INT(i));
		DrawString(Canvas, PosX, CurrentY + 1, *TimeText, GEngine->SmallFont, BaseTrackColor);
	}

	for (FLOAT i = 0; i <= AbilityViewer->MaxShowTrackLength; i += 0.1)
	{
		INT PosX = PixelsPerSecond * i + BaseX;
		DrawLine2D(Canvas, FVector2D(PosX, CurrentY), FVector2D(PosX, CurrentY - TrackLineHalfHeight*0.35), BaseTrackColor);
	}

	CurrentY -= TrackLineHalfHeight*2 + StringHeight;

	DrawAbilityStateTime(Canvas, BaseX, SizeX, CurrentY, TrackLineHalfHeight, PixelsPerSecond, BaseTrackColor);

	DrawComponents(Canvas, BaseX, CurrentY);

	// draw current time line
	INT PosX = BaseX + PixelsPerSecond * AbilityViewer->CurrentPlayTime;
	DrawLine2D(Canvas, FVector2D(PosX, 0), FVector2D(PosX, SizeY), FLinearColor(0.2, 1.0f, 0.2f));

	DrawLine2D(Canvas, FVector2D(BaseX, 0), FVector2D(BaseX, SizeY), FLinearColor(0.6, 0.8, 0.01));
	PosX = BaseX + PixelsPerSecond * AbilityViewer->GetAbilitySpellTime();
	DrawLine2D(Canvas, FVector2D(PosX, 0), FVector2D(PosX, SizeY), FLinearColor(0.6, 0.8, 0.01));

	if (ability->SpellCommonAttribute.PrepareType == ES_PT_Type1)	// channeling
	{
		FLOAT CastTime = 0.f;
		INT CastCount = 0;
		FLOAT FixTime = 0.f;
		ability->GetCastTime(CastTime, CastCount, FixTime);
		FLOAT TimeFactor = AbilityViewer->GetWeaponTimeFactor();
		if (ability->SpellCommonAttribute.AffectedBySpeedParam)
		{
			CastTime *= TimeFactor;
			FixTime *= TimeFactor;
		}
		for (INT i = 1; i < CastCount; ++i)
		{
			PosX = BaseX + PixelsPerSecond * (AbilityViewer->GetAbilitySpellTime() + CastTime * i);
			DrawLine2D(Canvas, FVector2D(PosX, SizeY * 0.9), FVector2D(PosX, SizeY), FLinearColor(0.6, 0.8, 0.01));
		}

		PosX += ability->SpellCommonAttribute.HardTime * PixelsPerSecond;
		PosX = BaseX + PixelsPerSecond * AbilityViewer->GetAbilityLastTime();
		DrawLine2D(Canvas, FVector2D(PosX, 0), FVector2D(PosX, SizeY), FLinearColor(0.6, 0.8, 0.01));
	}
	else
	{
		PosX = BaseX + PixelsPerSecond * AbilityViewer->GetAbilityLastTime();
		DrawLine2D(Canvas, FVector2D(PosX, 0), FVector2D(PosX, SizeY), FLinearColor(0.6, 0.8, 0.01));
	}







//	// Clear out the lines from the previous frame
//	PreviewScene.ClearLineBatcher();
//	ULineBatchComponent* LineBatcher = PreviewScene.GetLineBatcher();
//
//	INT XL, YL;
//	StringSize( GEngine->SmallFont,  XL, YL, TEXT("L") );
//
//	FSceneViewFamilyContext ViewFamily(Viewport,GetScene(),ShowFlags,GWorld->GetTimeSeconds(),GWorld->GetDeltaSeconds(),GWorld->GetRealTimeSeconds());
//	FSceneView* View = CalcSceneView(&ViewFamily);
//
//	const INT HalfX = Viewport->GetSizeX()/2;
//	const INT HalfY = Viewport->GetSizeY()/2;
//
//	check(AnimSetViewer->PreviewSkelComp);
//	const INT LODIndex = ::Clamp(AnimSetViewer->PreviewSkelComp->PredictedLODLevel, 0, AnimSetViewer->SelectedSkelMesh->LODModels.Num()-1);
//
//	FStaticLODModel& LODModel = AnimSetViewer->SelectedSkelMesh->LODModels( LODIndex );
//	const FSkelMeshComponentLODInfo& MeshLODInfo = AnimSetViewer->PreviewSkelComp->LODInfo( LODIndex );
//
//#if WITH_SIMPLYGON
//	if ( AnimSetViewer->SelectedSkelMesh->bHasBeenSimplified && SimplygonLogo && SimplygonLogo->Resource )
//	{
//		const FLOAT LogoSizeX = 64.0f;
//		const FLOAT LogoSizeY = 40.65f;
//		const FLOAT Padding = 6.0f;
//		const FLOAT LogoX = Viewport->GetSizeX() - Padding - LogoSizeX;
//		const FLOAT LogoY = Padding;
//
//		DrawTile(
//			Canvas,
//			LogoX,
//			LogoY,
//			LogoSizeX,
//			LogoSizeY,
//			0.0f,
//			0.0f,
//			1.0f,
//			1.0f,
//			FLinearColor::White,
//			SimplygonLogo->Resource,
//			SE_BLEND_Opaque );
//	}
//#endif // #if WITH_SIMPLYGON
//
//	// Make sure the textures for the mesh component are always resident while viewing the mesh in the animset viewer. Note that we do not
//	// block and update all resources here, so on the initial drawing in the viewer, the textures will have to stream in.
//	if ( MeshComp )
//	{
//		MeshComp->PrestreamTextures( 30.0f, FALSE );
//	}
//
//	if( bShowBoneNames && SkelMesh )
//	{
//		for(INT i=0; i< LODModel.RequiredBones.Num(); i++)
//		{
//			const INT BoneIndex	= LODModel.RequiredBones(i);
//
//			// If previewing a specific chunk, only show the bone names that belong to it
//			if ((MeshComp->ChunkIndexPreview >= 0) && !LODModel.Chunks(MeshComp->ChunkIndexPreview).BoneMap.ContainsItem(BoneIndex))
//			{
//				continue;
//			}
//
//			FColor& BoneColor = SkelMesh->RefSkeleton(BoneIndex).BoneColor;
//			if( BoneColor.A != 0 )
//			{
//				const FVector BonePos = MeshComp->LocalToWorld.TransformFVector(MeshComp->SpaceBases(BoneIndex).GetOrigin());
//
//				const FPlane proj = View->Project(BonePos);
//				if( proj.W > 0.f )
//				{
//					const INT XPos = HalfX + ( HalfX * proj.X );
//					const INT YPos = HalfY + ( HalfY * (proj.Y * -1) );
//
//					const FName BoneName		= SkelMesh->RefSkeleton(BoneIndex).Name;
//					const FString BoneString	= FString::Printf( TEXT("%d: %s"), BoneIndex, *BoneName.ToString() );
//
//					DrawString(Canvas,XPos, YPos, *BoneString, GEngine->SmallFont, BoneColor);
//				}
//			}
//		}
//	}
//
//	// Draw socket names if desired.
//	if( bShowSockets  || AnimSetViewer->SocketMgr )
//	{
//		for(INT i=0; i<AnimSetViewer->SelectedSkelMesh->Sockets.Num(); i++)
//		{
//			USkeletalMeshSocket* Socket = AnimSetViewer->SelectedSkelMesh->Sockets(i);
//
//			FMatrix SocketTM;
//			if( Socket->GetSocketMatrix(SocketTM, MeshComp) )
//			{
//				const FVector SocketPos	= SocketTM.GetOrigin();
//				const FPlane proj		= View->Project( SocketPos );
//				if(proj.W > 0.f)
//				{
//					const INT XPos = HalfX + ( HalfX * proj.X );
//					const INT YPos = HalfY + ( HalfY * (proj.Y * -1) );
//					DrawString(Canvas, XPos, YPos, *Socket->SocketName.ToString(), GEngine->SmallFont, FColor(255,196,196));
//
//					if (bManipulating && AnimSetViewer->SelectedSocket == Socket)
//					{
//						//Figure out the text height
//						FTextSizingParameters Parameters(GEngine->SmallFont, 1.0f, 1.0f);
//						UCanvas::CanvasStringSize(Parameters, *Socket->SocketName.ToString());
//						INT YL = appTrunc(Parameters.DrawYL);
//
//						//DrawAngles(Canvas, XPos, YPos + YL, 
//							//		SocketManipulateAxis, AnimSetViewer->SocketMgr->MoveMode, 
//								//	AnimSetViewer->SelectedSocket->RelativeRotation,
//									//AnimSetViewer->SelectedSocket->RelativeLocation);
//					}
//				}
//			}
//		}
//	}
//
//	// Draw manipulation angles for bones
//	if (bManipulating && AnimSetViewer->PreviewSkelComp->BonesOfInterest.Num() == 1)
//	{
//		FSkelControlListHead& SkelListControl = AnimSetViewer->PreviewAnimTree->SkelControlLists(0);
//		USkelControlSingleBone* SkelControl = Cast<USkelControlSingleBone>(SkelListControl.ControlHead);
//		if (SkelControl)
//		{
//			FMatrix BoneMatrix = AnimSetViewer->PreviewSkelComp->GetBoneMatrix(AnimSetViewer->PreviewSkelComp->BonesOfInterest(0));
//			const FPlane proj = View->Project(BoneMatrix.GetOrigin());
//			if( proj.W > 0.f )
//			{
//				const INT XPos = HalfX + ( HalfX * proj.X );
//				const INT YPos = HalfY + ( HalfY * (proj.Y * -1) );
//
//				//DrawAngles(Canvas, XPos, YPos, BoneManipulateAxis, AnimSetViewer->BoneMoveMode, SkelControl->BoneRotation, SkelControl->BoneTranslation);
//			}
//		}
//	}
//
//	// Draw stats about the mesh
//	const FBoxSphereBounds& SkelBounds = AnimSetViewer->PreviewSkelComp->Bounds;
//	const FPlane ScreenPosition = View->Project(SkelBounds.Origin);
//	const FLOAT ScreenRadius = Max((FLOAT)HalfX * View->ProjectionMatrix.M[0][0], (FLOAT)HalfY * View->ProjectionMatrix.M[1][1]) * SkelBounds.SphereRadius / Max(ScreenPosition.W,1.0f);
//	const FLOAT LODFactor = ScreenRadius / 320.0f;
//
//	INT NumBonesInUse;
//	INT NumChunksInUse;
//	INT NumSectionsInUse;
//	FString WeightUsage;
//
//	// for full weight swap show new section/chunk info instead of base info
//	if (AnimSetViewer != NULL && 
//		AnimSetViewer->bPreviewInstanceWeights && 
//		MeshLODInfo.InstanceWeightUsage == IWU_FullSwap &&
//		LODModel.VertexInfluences.Num() > 0)
//	{
//		NumBonesInUse  = LODModel.VertexInfluences(0).RequiredBones.Num();
//		NumChunksInUse =  LODModel.VertexInfluences(0).Chunks.Num();
//		NumSectionsInUse = LODModel.VertexInfluences(0).Sections.Num();
//		WeightUsage = LocalizeUnrealEd("ToggleMeshWeightsFull");
//	}
//	else
//	{
//		NumBonesInUse = LODModel.RequiredBones.Num();
//		NumChunksInUse = LODModel.Chunks.Num();
//		NumSectionsInUse = LODModel.Sections.Num();
//	}
//
//	FString InfoString = FString::Printf( TEXT("LOD [%d] Bones:%d Polys:%d (Display Factor:%3.2f, FOV:%3.0f)"), 
//		LODIndex, 
//		NumBonesInUse,	   
//		LODModel.GetTotalFaces(), 
//		LODFactor,
//		ViewFOV);
//
//	INT CurYOffset=5;
//	DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//
//	CurYOffset += 1; // --
//
//
//	UINT TotalRigidVertices = 0;
//	UINT TotalSoftVertices = 0;
//	for(INT ChunkIndex = 0;ChunkIndex < LODModel.Chunks.Num();ChunkIndex++)
//	{
//		INT ChunkRigidVerts = LODModel.Chunks(ChunkIndex).GetNumRigidVertices();
//		INT ChunkSoftVerts = LODModel.Chunks(ChunkIndex).GetNumSoftVertices();
//
//		InfoString = FString::Printf( TEXT(" [Chunk %d] Verts:%d (Rigid:%d Soft:%d)"), 
//			ChunkIndex,
//			ChunkRigidVerts + ChunkSoftVerts,
//			ChunkRigidVerts,
//			ChunkSoftVerts );
//
//		CurYOffset += YL + 2;
//		DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(200,200,200) );
//
//		TotalRigidVertices += ChunkRigidVerts;
//		TotalSoftVertices += ChunkSoftVerts;
//	}
//
//	InfoString = FString::Printf( TEXT("TOTAL Verts:%d (Rigid:%d Soft:%d)"), 
//		LODModel.NumVertices,
//		TotalRigidVertices,
//		TotalSoftVertices );
//
//	CurYOffset += 1; // --
//
//
//	CurYOffset += YL + 2;
//	DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//
//	InfoString = FString::Printf( TEXT("Chunks:%d Sections:%d %s"), 
//		NumChunksInUse, 
//		NumSectionsInUse,
//		*WeightUsage
//		);
//
//	CurYOffset += YL + 2;
//	DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//
//	if ( AnimSetViewer->VertexInfo.bShowNormal || AnimSetViewer->VertexInfo.bShowTangent )
//	{
//		if (AnimSetViewer->VertexInfo.SelectedVertexIndex != 0)
//		{
//			InfoString = FString::Printf( TEXT("Selected Vertex Index:%d Vertex Position: %s"), 
//					AnimSetViewer->VertexInfo.SelectedVertexIndex, 
//					*AnimSetViewer->VertexInfo.SelectedVertexPosition.ToString());
//
//			CurYOffset += YL + 2;
//			DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//
//// 			INT SelectedChunkIndex=GetChunkIndexFromMaterialIndex(AnimSetViewer->PreviewSkelComp, AnimSetViewer->VertexInfo.SelectedMaterialIndex);	
//// 			const WORD* RESTRICT BoneMap = LODModel.Chunks(SelectedChunkIndex).BoneMap.GetTypedData();
//// 
//// 			for ( INT I=0; I<AnimSetViewer->VertexInfo.BoneIndices.Num(); ++I )
//// 			{
//// 				INT BoneIndex = BoneMap[AnimSetViewer->VertexInfo.BoneIndices(I)];
//// 				FMatrix TM = AnimSetViewer->PreviewSkelComp->GetBoneMatrix(BoneIndex);
//// 				InfoString = FString::Printf( TEXT("Bone (%s) Weight (%0.2f) [%s]"),*AnimSetViewer->SelectedSkelMesh->RefSkeleton(BoneIndex).Name.ToString(), AnimSetViewer->VertexInfo.BoneWeights(I), *TM.ToString() );
//// 
//// 				CurYOffset += YL + 2;
//// 				DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//// 			}
//		}
//	}
//
//	if( AnimSetViewer->SelectedAnimSeq && AnimSetViewer->PreviewAnimNode && LODModel.RequiredBones.Num() > 0 )
//	{
//		const INT RootBoneIndex	= LODModel.RequiredBones(0);
//
//		if( MeshComp && MeshComp->SpaceBases.Num() > RootBoneIndex )
//		{
//			const UAnimSequence* AnimSeq = AnimSetViewer->SelectedAnimSeq;
//			UAnimSet* AnimSet = AnimSeq->GetAnimSet();
//			check(AnimSet);
//			if( AnimSetViewer->PreviewAnimNode->AnimLinkupIndex != INDEX_NONE )
//			{
//				FAnimSetMeshLinkup* AnimLinkup = &AnimSet->LinkupCache(AnimSetViewer->PreviewAnimNode->AnimLinkupIndex);
//				check(AnimLinkup);
//				const INT RootBoneTrackIndex = AnimLinkup->BoneToTrackTable(RootBoneIndex);
//				if(RootBoneTrackIndex != INDEX_NONE)
//				{
//					FMemMark Mark(GMainThreadMemStack);
//					FBoneAtom FirstFrameAtom;
//
//					// I just like to get current curvekeys and apply
//					FCurveKeyArray CurveKeys;
//					AnimSeq->GetBoneAtom(FirstFrameAtom, RootBoneTrackIndex, 0.f, FALSE, MeshComp->bUseRawData);
//
//					FBoneAtom CurrentFrameAtom;
//					AnimSeq->GetBoneAtom(CurrentFrameAtom, RootBoneTrackIndex, AnimSetViewer->PreviewAnimNode->CurrentTime, FALSE, MeshComp->bUseRawData, &CurveKeys);
//
//					MeshComp->ApplyCurveKeys(CurveKeys);
//
//					const FVector RootBoneDelta = MeshComp->LocalToWorld.TransformNormal(CurrentFrameAtom.GetTranslation() - FirstFrameAtom.GetTranslation());
//					InfoString = FString::Printf( TEXT("Root Bone Delta: %s"), *RootBoneDelta.ToString());
//
//					CurYOffset += YL + 2;
//					DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//
//					Mark.Pop();
//				}
//			}
//		}
//
//		if( AnimSetViewer->PreviewSkelComp != NULL )
//		{
//			const UAnimSequence* AnimSeq = AnimSetViewer->SelectedAnimSeq;
//			UAnimSet* AnimSet = AnimSeq->GetAnimSet();
//			check(AnimSet);
//			if( AnimSetViewer->PreviewAnimNode->AnimLinkupIndex != INDEX_NONE )
//			{
//				FAnimSetMeshLinkup* AnimLinkup = &AnimSet->LinkupCache(AnimSetViewer->PreviewAnimNode->AnimLinkupIndex);
//				check(AnimLinkup);
//
//				const UASVSkelComponent* PrevSkelComp = AnimSetViewer->PreviewSkelComp;
//				for( INT InterestIdx = 0; InterestIdx < PrevSkelComp->BonesOfInterest.Num(); InterestIdx++ )
//				{
//					const INT BoneIdx = PrevSkelComp->BonesOfInterest(InterestIdx);
//
//					const INT BoneTrackIdx = AnimLinkup->BoneToTrackTable(BoneIdx);
//					if( BoneTrackIdx != INDEX_NONE )
//					{
//						if( BoneIdx > INDEX_NONE && BoneIdx < PrevSkelComp->SpaceBases.Num() )
//						{
//							FMemMark Mark(GMainThreadMemStack);
//
//							FName BoneName = (PrevSkelComp->SkeletalMesh != NULL && PrevSkelComp->SkeletalMesh->RefSkeleton.IsValidIndex(BoneIdx)) ? PrevSkelComp->SkeletalMesh->RefSkeleton(BoneIdx).Name : NAME_None;
//
//							FBoneAtom CurrentFrameAtom;
//							FCurveKeyArray CurveKeys;
//							AnimSeq->GetBoneAtom(CurrentFrameAtom, BoneTrackIdx, AnimSetViewer->PreviewAnimNode->CurrentTime, FALSE, MeshComp->bUseRawData, &CurveKeys);
//
//							MeshComp->ApplyCurveKeys(CurveKeys);
//
//							// would like to show relative to root, so will need to apply parent space base, otherwise, it's all local atom
//							INT ParentIndex = (PrevSkelComp->SkeletalMesh != NULL && PrevSkelComp->SkeletalMesh->RefSkeleton.IsValidIndex(BoneIdx)) ? PrevSkelComp->SkeletalMesh->RefSkeleton(BoneIdx).ParentIndex : INDEX_NONE;
//							if (ParentIndex !=INDEX_NONE)
//							{
//								CurrentFrameAtom = CurrentFrameAtom*PrevSkelComp->SpaceBases(ParentIndex);
//							}
//
//							const FVector BoneDelta = MeshComp->LocalToWorld.TransformNormal(CurrentFrameAtom.GetTranslation() - PrevSkelComp->SpaceBases(LODModel.RequiredBones(0)).GetOrigin());
//
//							InfoString = FString::Printf( TEXT("%s -- Bone Delta: %s"), *BoneName.ToString(), *BoneDelta.ToString() );
//							CurYOffset += YL + 2;
//							DrawString( Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//
//							InfoString = FString::Printf( TEXT("%s -- Bone Rotation: %s"), *BoneName.ToString(), *CurrentFrameAtom.GetRotation().Rotator().GetNormalized().ToString() );
//							CurYOffset += YL + 2;
//							DrawString( Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//							Mark.Pop();
//						}
//					}
//				}
//			}
//		}
//	}
//
//	// If bAnimRotationOnly has been changed, display warning that the sequence needs to be recompressed
//	if( AnimSetViewer->SelectedAnimSeq && AnimSetViewer->SelectedAnimSet && (AnimSetViewer->SelectedAnimSeq->bWasCompressedWithoutTranslations != AnimSetViewer->SelectedAnimSet->bAnimRotationOnly))
//	{
//		InfoString = FString::Printf( TEXT("Warning: %s needs to be recompressed; displaying RAW animation data instead"), *AnimSetViewer->SelectedAnimSeq->SequenceName.ToString());
//
//		CurYOffset += YL + 2;
//		DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,0,0) );
//	}
//
//	// Draw anim notify viewer.
//	if( AnimSetViewer->SelectedAnimSeq )
//	{
//		const INT NotifyViewBorderX = 10;
//		const INT NotifyViewBorderY = 10;
//		const INT NotifyViewEndHeight = 10;
//
//		const INT SizeX = Viewport->GetSizeX();
//		const INT SizeY = Viewport->GetSizeY();
//
//		const UAnimSequence* AnimSeq = AnimSetViewer->SelectedAnimSeq;
//
//		const FLOAT PixelsPerSecond = (FLOAT)(SizeX - (2*NotifyViewBorderX))/AnimSeq->SequenceLength;
//
//		// Draw ends of track
//		DrawLine2D(Canvas, FVector2D(NotifyViewBorderX, SizeY - NotifyViewBorderY - NotifyViewEndHeight), FVector2D(NotifyViewBorderX, SizeY - NotifyViewBorderY), FColor(255,255,255) );
//		DrawLine2D(Canvas, FVector2D(SizeX - NotifyViewBorderX, SizeY - NotifyViewBorderY - NotifyViewEndHeight), FVector2D(SizeX - NotifyViewBorderX, SizeY - NotifyViewBorderY), FColor(255,255,255) );
//
//		// Draw track itself
//		const INT TrackY = SizeY - NotifyViewBorderY - (0.5f*NotifyViewEndHeight);
//		DrawLine2D(Canvas, FVector2D(NotifyViewBorderX, TrackY), FVector2D(SizeX - NotifyViewBorderX, TrackY), FColor(255,255,255) );
//
//		// Draw notifies on the track
//		TArray<TrackSlots> Tracks;
//		INT NotifyY = TrackY;
//		for(INT i=0; i<AnimSeq->Notifies.Num(); i++)
//		{
//			const FAnimNotifyEvent& NotifyEvent = AnimSeq->Notifies(i);
//
//			INT NotifyX = NotifyViewBorderX + (PixelsPerSecond * NotifyEvent.Time);
//
//			// Get color from notify object, if present
//			FColor NotifyColor = (NotifyEvent.Notify) ? NotifyEvent.Notify->GetEditorColor() : FColor(255,200,200);
//
//			// Combine comment from notify struct and from function on object
//			FString NotifyComment;
//			if (NotifyEvent.Comment != NAME_None)
//			{
//				NotifyComment = FString::Printf(TEXT("[%s]"),*NotifyEvent.Comment.ToString());
//			}
//			if(NotifyEvent.Notify)
//			{
//				NotifyComment += NotifyEvent.Notify->GetEditorComment();
//			}
//
//			// guarantee some sort of label
//			if( NotifyComment.Len() == 0 )
//			{
//				NotifyComment = TEXT("??");
//			}
//			// do the deed
//			{
//				INT W, H;
//				// figure out what height to draw this notify at
//				StringSize(GEngine->SmallFont, W, H, *NotifyComment);
//				
//				INT LabelX = Clamp(NotifyX - W/2,NotifyViewBorderX,SizeX-NotifyViewBorderX-W);
//				INT TrackIdx = -1;
//
//				INT TrackW = W;
//				if( NotifyEvent.Duration > 0.f )
//				{
//					// Double track with for extra label
//					TrackW *= 2.f;
//					// Stretch track for the duration
//					TrackW += (NotifyEvent.Duration / AnimSeq->SequenceLength) * (SizeX - (2.f * NotifyViewBorderX));
//				}
//				for (INT Idx = 0; Idx < Tracks.Num(); Idx++)
//				{
//					if (Tracks(Idx).AddSlot(LabelX,TrackW))
//					{
//						TrackIdx = Idx;
//						break;
//					}
//				}
//				if (TrackIdx == -1)
//				{
//					// add new track, no room on existing ones
//					TrackIdx = Tracks.AddItem(TrackSlots());
//					Tracks(TrackIdx).AddSlot(LabelX,TrackW);
//				}
//				NotifyY = TrackY - ((H + 8) * (TrackIdx + 1));
//				
//				// draw a line from the track to see the actual position
//				DrawLine2D(Canvas, FVector2D(NotifyX, NotifyY + H), FVector2D(NotifyX, TrackY), NotifyColor );
//				// draw a box and comment with a hit proxy for time dragging
//				if (Canvas->IsHitTesting())	{Canvas->SetHitProxy(new HNotifyProxy(i));}
//					DrawTile(Canvas,LabelX-2,NotifyY-2,W+2,H+2,0.f,0.f,1.f,1.f,FLinearColor(0.05f,0.05f,0.05f,1.f));
//					DrawBox2D(Canvas,FVector2D( LabelX - 2, NotifyY - 2 ),FVector2D( LabelX + W + 1, NotifyY + H + 1 ),NotifyColor );
//					DrawString(Canvas, LabelX, NotifyY, *NotifyComment, GEngine->SmallFont, NotifyColor );
//					// if we've got a duration then draw a matching notifying widget
//					if (NotifyEvent.Duration > 0.f)
//					{
//						INT RightEdge = LabelX + W + 2;
//						NotifyX = NotifyViewBorderX + (PixelsPerSecond * (NotifyEvent.Time + NotifyEvent.Duration));
//						LabelX = Clamp(NotifyX - W/2,NotifyViewBorderX,SizeX-NotifyViewBorderX-W);
//						// draw a line connecting the two boxes
//						DrawLine2D(Canvas,FVector2D(LabelX-2,NotifyY + H/2),FVector2D(RightEdge,NotifyY + H/2),NotifyColor);
//						if (Canvas->IsHitTesting())	{Canvas->SetHitProxy(new HNotifyProxy(i,TRUE));}
//						DrawBox2D(Canvas,FVector2D( LabelX - 2, NotifyY - 2 ),FVector2D( LabelX + W + 1, NotifyY + H + 1 ),NotifyColor );
//						DrawString(Canvas, LabelX, NotifyY, *NotifyComment, GEngine->SmallFont, NotifyColor );
//						DrawLine2D(Canvas, FVector2D(NotifyX, NotifyY + H), FVector2D(NotifyX, TrackY), NotifyColor );
//					}
//				if (Canvas->IsHitTesting())	{Canvas->SetHitProxy(NULL);}
//			}
//		}
//		
//		// Bone Control Modifiers are displayed in animset viewer 
//		// Using same track slot with notifier, and display which bone control and what is strength
//		for(INT MetadataIndex=0; MetadataIndex<AnimSeq->MetaData.Num(); MetadataIndex++)
//		{
//			UAnimMetaData* Metadata = AnimSeq->MetaData(MetadataIndex);
//			UAnimMetaData_SkelControlKeyFrame* MetadataKeyFrame = Cast<UAnimMetaData_SkelControlKeyFrame>(Metadata);
//			if( MetadataKeyFrame )
//			{
//				for (INT KeyIndex=0; KeyIndex<MetadataKeyFrame->KeyFrames.Num(); KeyIndex++)
//				{
//					FTimeModifier& Key = MetadataKeyFrame->KeyFrames(KeyIndex);
//					const INT NotifyX = NotifyViewBorderX + (PixelsPerSecond * Key.Time);
//
//					// Get color from notify object, if present
//					FColor NotifyColor = FColor(112,154,209);
//
//					// Combine comment from notify struct and from function on object
//					FString BoneControlName;
//					if( MetadataKeyFrame->SkelControlNameList.Num() > 0 )
//					{
//						BoneControlName = FString::Printf( TEXT("%s"), (MetadataKeyFrame->SkelControlNameList(0)==NAME_None)?TEXT("None"): *MetadataKeyFrame->SkelControlNameList(0).GetNameString());
//						for(INT NameIdx=1; NameIdx<MetadataKeyFrame->SkelControlNameList.Num(); NameIdx++)
//						{
//							BoneControlName = FString::Printf( TEXT("%s,%s"), *BoneControlName, (MetadataKeyFrame->SkelControlNameList(NameIdx)==NAME_None)?TEXT("None"): *MetadataKeyFrame->SkelControlNameList(NameIdx).GetNameString());
//						}
//					}
//					else
//					{
//						BoneControlName = TEXT("None");
//					}
//					FString NotifyComment = FString::Printf(TEXT("[%s:%0.2f]"),*BoneControlName, Key.TargetStrength);
//
//					// do the deed
//					{
//						INT W, H;
//						// figure out what height to draw this notify at
//						StringSize(GEngine->SmallFont, W, H, *NotifyComment);
//						INT LabelX = Clamp(NotifyX - W/2,NotifyViewBorderX,SizeX-NotifyViewBorderX-W);
//						INT TrackIdx = -1;
//						for (INT Idx = 0; Idx < Tracks.Num(); Idx++)
//						{
//							if (Tracks(Idx).AddSlot(LabelX,W))
//							{
//								TrackIdx = Idx;
//								break;
//							}
//						}
//						if (TrackIdx == -1)
//						{
//							// add new track, no room on existing ones
//							TrackIdx = Tracks.AddItem(TrackSlots());
//							Tracks(TrackIdx).AddSlot(LabelX,W);
//						}
//						NotifyY = TrackY - ((H + 8) * (TrackIdx + 1));
//
//						// draw a line from the track to see the actual position
//						DrawLine2D(Canvas, FVector2D(NotifyX, NotifyY + H), FVector2D(NotifyX, TrackY), NotifyColor );
//						// draw a box and comment with a hit proxy for time dragging
//						if (Canvas->IsHitTesting())	{Canvas->SetHitProxy(new HBoneControlProxy(MetadataIndex, KeyIndex));}
//						DrawTile(Canvas,LabelX-2,NotifyY-2,W+2,H+2,0.f,0.f,1.f,1.f,FLinearColor(0.05f,0.05f,0.05f,1.f));
//						DrawBox2D(Canvas,FVector2D( LabelX - 2, NotifyY - 2 ),FVector2D( LabelX + W + 1, NotifyY + H + 1 ),NotifyColor );
//						DrawString(Canvas, LabelX, NotifyY, *NotifyComment, GEngine->SmallFont, NotifyColor );
//						if (Canvas->IsHitTesting())	{Canvas->SetHitProxy(NULL);}
//					}
//				}
//			}
//		}
//
//		// Draw current position on the track.
//		if (Canvas->IsHitTesting())	{Canvas->SetHitProxy(new HTrackPosProxy());}
//		const INT CurrentPosX = NotifyViewBorderX + (PixelsPerSecond * AnimSetViewer->PreviewAnimNode->CurrentTime);
//			DrawTriangle2D(Canvas, 
//				FVector2D(CurrentPosX, TrackY-1), FVector2D(0.f, 0.f), 
//				FVector2D(CurrentPosX+5, TrackY-6), FVector2D(0.f, 0.f), 
//				FVector2D(CurrentPosX-5, TrackY-6), FVector2D(0.f, 0.f), 
//				bDraggingTrackPos ? FColor(200,0,200) : FColor(255,255,255) );
//		if (Canvas->IsHitTesting())	{Canvas->SetHitProxy(NULL);}
//
//		INT XS, YS;
//		// Draw morph curve if show morph curve on and if curve data exists
//		if ( bShowMorphKeys && AnimSeq->CurveData.Num() && AnimSetViewer->PreviewAnimNode )
//		{
//			FMemMark Mark(GMainThreadMemStack);
//
//			FCurveKeyArray CurveKeys;
//			AnimSeq->GetCurveData(AnimSetViewer->PreviewAnimNode->CurrentTime, AnimSetViewer->PreviewAnimNode->bLooping, CurveKeys);
//
//			for ( INT CurveIdx = 0; CurveIdx < CurveKeys.Num(); ++CurveIdx )
//			{
//				UMorphTarget * MorphTarget = AnimSetViewer->PreviewSkelComp->FindMorphTarget(CurveKeys(CurveIdx).CurveName);
//				INT NumOfVertices = 0;
//				if (MorphTarget)
//				{
//					const INT LODIndex = ::Clamp(AnimSetViewer->PreviewSkelComp->PredictedLODLevel, 0, AnimSetViewer->SelectedSkelMesh->LODModels.Num()-1);
//					const FMorphTargetLODModel & MorphLOD = MorphTarget->MorphLODModels(LODIndex);
//					NumOfVertices = MorphLOD.Vertices.Num();
//				}
//				InfoString = FString::Printf(TEXT("Morph Target Name: %s, Weight: %0.4f, Verts:%d"), 
//					*CurveKeys(CurveIdx).CurveName.GetNameString(), 
//					CurveKeys(CurveIdx).Weight, 
//					NumOfVertices);
//
//				StringSize(GEngine->SmallFont, XS, YS, *InfoString);
//
//				DrawString(Canvas,SizeX - XS - XL, (YS + 5.f)*CurveIdx + YL, *InfoString, GEngine->SmallFont, FColor(200,10,10));
//			}
//			Mark.Pop();
//		}
//
//		// Draw anim position
//		InfoString = FString::Printf(TEXT("Pos: %3.1f pct, Time: %4.4fs, Len: %4.4fs"), 
//						100.f*AnimSetViewer->PreviewAnimNode->CurrentTime/AnimSeq->SequenceLength, 
//						AnimSetViewer->PreviewAnimNode->CurrentTime, 
//						AnimSeq->SequenceLength);
//
//		StringSize(GEngine->SmallFont, XS, YS, *InfoString);
//		DrawString(Canvas,SizeX - XS - XL - NotifyViewBorderX, SizeY - YS - YL - NotifyViewBorderY - NotifyViewEndHeight, *InfoString, GEngine->SmallFont, FColor(255,255,255));
//
//		// Draw if it's an ADDITIVE animation
//		if( AnimSetViewer->SelectedAnimSeq->bIsAdditive )
//		{
//			InfoString = FString::Printf(TEXT("ADDITIVE ANIMATION (Reference: %s)"), *AnimSetViewer->SelectedAnimSeq->AdditiveRefName.ToString());
//			CurYOffset += YL + 2;
//			DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,0,0) );
//		}
//		else if( AnimSetViewer->SelectedAnimSeq->RelatedAdditiveAnimSeqs.Num() > 0)
//		{
//			// list out additive animation that has been created
//			InfoString = FString::Printf(TEXT("Base/Target pose for the following additive animation(s) : "));
//			CurYOffset += YL + 2;
//			DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(180,0,0) );
//			for (INT I=0; I<AnimSetViewer->SelectedAnimSeq->RelatedAdditiveAnimSeqs.Num(); ++I)
//			{
//				InfoString = FString::Printf(TEXT("%s:%s"), *AnimSetViewer->SelectedAnimSeq->GetAnimSet()->GetPathName(),  
//					*AnimSetViewer->SelectedAnimSeq->RelatedAdditiveAnimSeqs(I)->SequenceName.GetNameString());
//				CurYOffset += YL + 2;
//				DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(180,0,0) );
//			}
//		}
//	}
//
//	// If doing cloth sim, show wind direction
//	if(AnimSetViewer->PreviewSkelComp->bEnableClothSimulation)
//	{
//		INT NumClothVerts = AnimSetViewer->SelectedSkelMesh->ClothToGraphicsVertMap.Num();
//		INT NumFixedClothVerts = NumClothVerts - AnimSetViewer->SelectedSkelMesh->NumFreeClothVerts;
//
//		InfoString = FString::Printf( TEXT("Cloth Verts:%d (%d Fixed)"), 
//			NumClothVerts, 
//			NumFixedClothVerts );
//
//		CurYOffset += YL + 2;
//		DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//	}
//
//	if(AnimSetViewer->PreviewSkelComp->bEnableClothSimulation || AnimSetViewer->PreviewSkelComp->GetApexClothing() != NULL)
//	{
//		DrawWindDir(Viewport, Canvas);
//	}
//
//		
//	if(AnimSetViewer->SelectedSkelMesh && AnimSetViewer->SelectedSkelMesh->SoftBodySurfaceToGraphicsVertMap.Num() > 0)
//	{
//		INT NumSurfaceVerts	= AnimSetViewer->SelectedSkelMesh->SoftBodySurfaceToGraphicsVertMap.Num();
//		INT NumSurfaceTris	= AnimSetViewer->SelectedSkelMesh->SoftBodySurfaceIndices.Num() / 3;
//		INT NumTetraVerts	= AnimSetViewer->SelectedSkelMesh->SoftBodyTetraVertsUnscaled.Num();				
//		INT NumTetras		= AnimSetViewer->SelectedSkelMesh->SoftBodyTetraIndices.Num() / 4;		
//
//		InfoString = FString::Printf( TEXT("Soft-Body Surface Triangles: %d, Surface Verts: %d, Tetras: %d, Tetra Verts: %d"), 
//			NumSurfaceTris, NumSurfaceVerts, NumTetras, NumTetraVerts);
//
//		CurYOffset += YL + 2;
//		DrawString(Canvas, 5, CurYOffset, *InfoString, GEngine->SmallFont, FColor(255,255,255) );
//	}
//
//	if(bTriangleSortMode)
//	{
//		if( AnimSetViewer->SelectedSortStripIndex == INDEX_NONE )
//		{
//			// Check if we have a TRISORT_Custom section.
//			UBOOL bFoundCustom=FALSE;
//			for( INT LodIndex=0;LodIndex<AnimSetViewer->SelectedSkelMesh->LODModels.Num();LodIndex++ )
//			{
//				FStaticLODModel& LODModel = AnimSetViewer->SelectedSkelMesh->LODModels(LodIndex);
//				for( INT SectionIndex=0;SectionIndex < LODModel.Sections.Num();SectionIndex++ )
//				{
//					FSkelMeshSection& Section = LODModel.Sections(SectionIndex);
//					if( Section.TriangleSorting == TRISORT_Custom || Section.TriangleSorting == TRISORT_CustomLeftRight )
//					{
//						bFoundCustom = TRUE;
//						break;
//					}
//				}
//				if( bFoundCustom )
//				{
//					break;
//				}
//			}
//			InfoString = bFoundCustom ? LocalizeUnrealEd("ClickToSelectSortStrip") : LocalizeUnrealEd("NoCustomSortSections");
//		}
//		else
//		{
//			if( AnimSetViewer->bSortStripMoveForward )
//			{
//				InfoString = AnimSetViewer->MouseOverSortStripIndex==INDEX_NONE ?
//					LocalizeUnrealEd("SelectStripToMoveInFrontOf") : LocalizeUnrealEd("ClickToMoveInFront");
//			}
//			else
//			if( AnimSetViewer->bSortStripMoveBackward )
//			{
//				InfoString = AnimSetViewer->MouseOverSortStripIndex==INDEX_NONE ?
//					LocalizeUnrealEd("SelectStripToMoveBehind") : LocalizeUnrealEd("ClickToMoveBehind");
//			}
//			else
//			{
//				InfoString = LocalizeUnrealEd("PressBorFToSort");
//			}
//		}
//
//		// Draw the appropriate text for the current triangle sorting operation.
//		DrawString(Canvas, 5, Viewport->GetSizeY() - 80, *InfoString, GEngine->SmallFont, FColor(255,255,0) );
//	}
//
//	if( AnimSetViewer->UVSetToDisplay != -1 )
//	{
//		CurYOffset += YL + 2;
//		DrawString(Canvas, 5, CurYOffset, *FString::Printf( TEXT("Showing UV set %d for LOD %d"), AnimSetViewer->UVSetToDisplay, LODIndex ), GEngine->SmallFont, FColor(255,255,255) );
//		//calculate scaling
//		const UINT BorderWidth = 5;
//		const UINT MinY = 80 + BorderWidth;
//		const UINT MinX = BorderWidth;
//		const FVector2D Origin(MinX, MinY);
//		const UINT SizeOfSquare = Min(Viewport->GetSizeX() - MinX, Viewport->GetSizeY() - MinY) - BorderWidth;
//
//		//draw texture border
//		DrawLine2D(Canvas, Origin, FVector2D(MinX + SizeOfSquare, MinY), FLinearColor::White);
//		DrawLine2D(Canvas, FVector2D(MinX + SizeOfSquare, MinY), FVector2D(MinX + SizeOfSquare, MinY + SizeOfSquare), FLinearColor::White);
//		DrawLine2D(Canvas, FVector2D(MinX + SizeOfSquare, MinY + SizeOfSquare), FVector2D(MinX, MinY + SizeOfSquare), FLinearColor::White);
//		DrawLine2D(Canvas, FVector2D(MinX, MinY + SizeOfSquare), Origin, FLinearColor::White);   
//
//		if( AnimSetViewer->UVSetToDisplay < (INT)LODModel.NumTexCoords )
//		{
//			//draw triangles
//			TArray<DWORD> Indices;
//			LODModel.MultiSizeIndexContainer.GetIndexBuffer( Indices );
//			UINT NumIndices = LODModel.MultiSizeIndexContainer.GetIndexBuffer()->Num();
//			for (UINT i = 0; i < NumIndices - 2; i += 3)
//			{
//				FVector2D UV1(LODModel.VertexBufferGPUSkin.GetVertexUV(Indices(i), AnimSetViewer->UVSetToDisplay )); 
//				FVector2D UV2(LODModel.VertexBufferGPUSkin.GetVertexUV(Indices(i + 1), AnimSetViewer->UVSetToDisplay )); 
//				FVector2D UV3(LODModel.VertexBufferGPUSkin.GetVertexUV(Indices(i + 2), AnimSetViewer->UVSetToDisplay )); 
//
//				// Draw lines in black unless the UVs are outside of the 0.0 - 1.0 range.  For out-of-bounds
//				// UVs, we'll draw the line segment in red
//				FLinearColor UV12LineColor = FLinearColor::Black;
//				if( UV1.X < -0.01f || UV1.X > 1.01f ||
//					UV2.X < -0.01f || UV2.X > 1.01f ||
//					UV1.Y < -0.01f || UV1.Y > 1.01f ||
//					UV2.Y < -0.01f || UV2.Y > 1.01f )
//				{
//					UV12LineColor = FLinearColor( 0.6f, 0.0f, 0.0f );
//				}
//				FLinearColor UV23LineColor = FLinearColor::Black;
//				if( UV3.X < -0.01f || UV3.X > 1.01f ||
//					UV2.X < -0.01f || UV2.X > 1.01f ||
//					UV3.Y < -0.01f || UV3.Y > 1.01f ||
//					UV2.Y < -0.01f || UV2.Y > 1.01f )
//				{
//					UV23LineColor = FLinearColor( 0.6f, 0.0f, 0.0f );
//				}
//				FLinearColor UV31LineColor = FLinearColor::Black;
//				if( UV3.X < -0.01f || UV3.X > 1.01f ||
//					UV1.X < -0.01f || UV1.X > 1.01f ||
//					UV3.Y < -0.01f || UV3.Y > 1.01f ||
//					UV1.Y < -0.01f || UV1.Y > 1.01f )
//				{
//					UV31LineColor = FLinearColor( 0.6f, 0.0f, 0.0f );
//				}
//
//				UV1 = ClampUVRange(UV1.X, UV1.Y) * SizeOfSquare + Origin;
//				UV2 = ClampUVRange(UV2.X, UV2.Y) * SizeOfSquare + Origin;
//				UV3 = ClampUVRange(UV3.X, UV3.Y) * SizeOfSquare + Origin;
//
//				DrawLine2D(Canvas, UV1, UV2, UV12LineColor);
//				DrawLine2D(Canvas, UV2, UV3, UV23LineColor);
//				DrawLine2D(Canvas, UV3, UV1, UV31LineColor);
//			}
//		}
//	}

}

/** Util for drawing wind direction. */
void FEditorAbilityViewportClient::DrawWindDir(FViewport* Viewport, FCanvas* Canvas)
{
	FRotationMatrix ViewTM( this->ViewRotation );

	const INT SizeX = Viewport->GetSizeX();
	const INT SizeY = Viewport->GetSizeY();

	const FIntPoint AxisOrigin( 80, SizeY - 30 );
	const FLOAT AxisSize = 25.f;

	FVector AxisVec = AxisSize * ViewTM.InverseTransformNormal( AbilityViewer->WindRot.Vector() );
	FIntPoint AxisEnd = AxisOrigin + FIntPoint( AxisVec.Y, -AxisVec.Z );
	DrawLine2D( Canvas, AxisOrigin, AxisEnd, FColor(255,255,128) );

	FString WindString = FString::Printf( TEXT("Wind: %4.1f"), AbilityViewer->WindStrength );
	DrawString( Canvas, 65, SizeY - 15, *WindString, GEngine->SmallFont, FColor(255,255,128) );
	// Display windvelocityblendtime in the preview window for Apex Clothing Wind
#if WITH_APEX
	if(AbilityViewer->PreviewSkelComp->GetApexClothing() != NULL)
	{
		FString WindVelBlendTimeString = FString::Printf( TEXT("BlendTime: %4.3f"), AbilityViewer->WindVelocityBlendTime);
		DrawString( Canvas, 150, SizeY - 15, *WindVelBlendTimeString, GEngine->SmallFont, FColor(255,255,128));
	}
#endif
}

void FEditorAbilityViewportClient::Tick(FLOAT DeltaSeconds)
{
	FEditorLevelViewportClient::Tick(DeltaSeconds);
	AbilityViewer->TickViewer(DeltaSeconds * AbilityViewer->PlaybackSpeed);

	const FLOAT CameraRotationTime = 1.0;
	DOUBLE CurrentTime = appSeconds();
	if (CurrentTime - CameraStartTime <= CameraRotationTime)
	{
		FVector NewLocation;
		FRotator NewRotation;

		INT DesYaw = ViewRotation.Yaw;
		DesYaw = (DesYaw & 0xffff) - (16384.f * DeltaSeconds / CameraRotationTime);

		NewRotation.Yaw =  DesYaw & 0xffff;
		NewRotation.Pitch = -5461;
		NewRotation.Roll = 0;

		NewLocation = NewRotation.Vector().SafeNormal() * (-1000.f);
		SetCameraPosition(NewLocation, NewRotation);
	}
}

UBOOL FEditorAbilityViewportClient::InputKey(FViewport* Viewport, INT ControllerId, FName Key, EInputEvent Event,FLOAT AmountDepressed,UBOOL Gamepad)
{
	const INT HitX = Viewport->GetMouseX();
	const INT HitY = Viewport->GetMouseY();
	if (Key == KEY_LeftMouseButton)
	{
		HHitProxy* HitResult = Viewport->GetHitProxy(HitX, HitY);
		if (HitResult)
		{
			if (Event == IE_Pressed)
			{
				if (HitResult->IsA(HAbilityComponentProxy::StaticGetType()))
				{
					HAbilityComponentProxy* HitCompProxy = (HAbilityComponentProxy*)HitResult;
					bDraggingComponent = TRUE;
					Type = HitCompProxy->Type;
					ComponentIdx = HitCompProxy->ComponentIdx;
					StateType = HitCompProxy->StateType;
					bTail = HitCompProxy->bTail;
					if (Type == EACPT_Component)
					{
						AbilityViewer->ShowProperty(StateType, ComponentIdx);
					}
				}
			}
		}

		if (Event == IE_Released)
		{
			bDraggingComponent = FALSE;
			if (EditObject)
			{
				AbilityViewer->AbilityObject->MarkPackageDirty();
				GCallbackEvent->Send(CALLBACK_ObjectPropertyChanged, EditObject);
				GCallbackEvent->Send(CALLBACK_ObjectPropertyChanged, AbilityViewer->AbilityObject);
				EditObject = NULL;
			}
		}
	}
	else if (Key == KEY_Delete)
	{
		if (Type == EACPT_Component)
		{
			AbilityViewer->DeleteComponent(Type, StateType, ComponentIdx, bTail);
			ComponentIdx = -1;
		}
	}
	else if (Key == KEY_RightMouseButton)
	{
		if (Event == IE_Pressed)
		{
			MousePressX = Viewport->GetMouseX();
			MousePressY = Viewport->GetMouseY();


			HHitProxy* HitResult = Viewport->GetHitProxy(HitX, HitY);
			if (HitResult)
			{
				if (Event == IE_Pressed)
				{
					if (HitResult->IsA(HAbilityComponentProxy::StaticGetType()))
					{
						HAbilityComponentProxy* HitCompProxy = (HAbilityComponentProxy*)HitResult;
						if (HitCompProxy->Type == EACPT_Component)
						{
							UAbilityComponent* NewSelected = AbilityViewer->GetSelectComponentByType(HitCompProxy->StateType, HitCompProxy->ComponentIdx);
							if (NewSelected)
							{
								AbilityViewer->CurrentSelectedComponent = NewSelected;
							}
						}
					}
				}
			}
		}
		if (Event == IE_Released)
		{
			INT x, y;
			x = Viewport->GetMouseX();
			y = Viewport->GetMouseY();
			if ((x - MousePressX) * (x - MousePressX) + (y - MousePressY) * (y - MousePressY) < 3)
			{
				AbilityViewer->PopupAddMenu(x, y);
			}
			MousePressX = MousePressY = 0;
		}
	}

	// update anim list
	AbilityViewer->RefreshAnimInfoList();

	//// Handle viewport screenshot.
	//InputTakeScreenshot( Viewport, Key, Event );

	return TRUE;
}

/** Handles mouse being moved while input is captured - in this case, while a mouse button is down. */
UBOOL FEditorAbilityViewportClient::InputAxis(FViewport* Viewport, INT ControllerId, FName Key, FLOAT Delta, FLOAT DeltaTime, UBOOL bGamepad)
{

	// Look at which axis is being dragged and by how much
	FLOAT DragX = (Key == KEY_MouseX) ? Delta : 0.f;
	FLOAT DragY = (Key == KEY_MouseY) ? Delta : 0.f;
	if (bDraggingComponent)
	{
		UMMOAbility* ability = AbilityViewer->AbilityObject;
		if (!ability)
		{
			return TRUE;
		}

		TArray<FComponentNode>* SpellEffectList = NULL;
		TArray<FComponentNode>* CastEffectList = NULL;

		UBOOL bFound = ability->GetEffectComponentList(AbilityViewer->CurrentWeaponType, &SpellEffectList, &CastEffectList);
		if (!bFound)
		{
			return TRUE;
		}


		FLOAT TimeFix = DragX/PixelsPerSecond;

		if (Type == EACPT_Component)
		{
			FLOAT LastTime = 0.f;
			FComponentNode* Node = NULL;
			if (StateType == EAST_Spell)
			{
				LastTime = AbilityViewer->GetAbilitySpellTime();
				if (ComponentIdx >= 0 && ComponentIdx < (*SpellEffectList).Num())
				{
					Node = &((*SpellEffectList)(ComponentIdx));
				}
			}
			else
			{
				LastTime = AbilityViewer->GetAbilityCastTime();
				if (ComponentIdx >= 0 && ComponentIdx < (*CastEffectList).Num())
				{
					Node = &((*CastEffectList)(ComponentIdx));
				}
			}
			if (!Node)
			{
				return  TRUE;
			}

			if (Node->Component == NULL)
			{
				return TRUE;
			}

			EditObject = Node->Component;

			if (bTail)
			{
				if (Node->Component->StopType == ACBT_StopWhenConstDurationPass)
				{
					FLOAT LastTime = Node->Component->GetResLength(AbilityViewer->PreviewSkelComp);
					LastTime += TimeFix;
					Node->Component->SetResLength(AbilityViewer->PreviewSkelComp, LastTime);
				}
			}
			else
			{
				FLOAT LastTime = 0;
				if (StateType == EAST_Spell)
				{
					LastTime = AbilityViewer->GetAbilitySpellTime();
				}
				else
				{
					LastTime = AbilityViewer->GetAbilityCastTime();
				}

				FLOAT NewTime = Node->StartTime;
				NewTime += TimeFix;
				if (NewTime <= 0)
				{
					NewTime = 0;
				}
				Node->StartTime = NewTime; 
			}


		}
		else if (Type == EACPT_AbilityTime)
		{
			if (StateType == EAST_Spell)
			{
				if (ComponentIdx == 0)	// MinCastTime
				{
					FLOAT NewTime = AbilityViewer->GetAbilitySpellTime() + TimeFix;
					if (NewTime < 0.f)
					{
						NewTime = 0;
					}

					AbilityViewer->SetAbilitySpellTime(NewTime);
				}
			}
			else if (StateType == EAST_Cast)
			{
				FLOAT NewTime = 0;

				NewTime = AbilityViewer->GetAbilityCastTime() + TimeFix;
				if (NewTime < 0.f)
				{
					NewTime = 0.f;
				}
				AbilityViewer->SetAbiltiyCastTime(NewTime);
			}

			EditObject = ability;
		}
		else
		{
			//impossible
		}


	}
	else
	{
		MouseDeltaTracker->AddDelta( this, Key, Delta, 0 );
		const FVector DragDelta = MouseDeltaTracker->GetDelta();

		GEditor->MouseMovement += DragDelta;

		if( !DragDelta.IsZero() )
		{
			// Convert the movement delta into drag/rotation deltas
			if ( bAllowMayaCam && GEditor->bUseMayaCameraControls )
			{
				FVector TempDrag;
				FRotator TempRot;
				InputAxisMayaCam( Viewport, DragDelta, TempDrag, TempRot );
			}
			else
			{
				FVector Drag;
				FRotator Rot;
				FVector Scale;
				MouseDeltaTracker->ConvertMovementDeltaToDragRot( this, DragDelta, Drag, Rot, Scale );
				MoveViewportCamera( Drag, Rot );
			}

			MouseDeltaTracker->ReduceBy( DragDelta );
		}
	}

	// update anim list
	AbilityViewer->RefreshAnimInfoList();

//	// Get some useful info about buttons being held down
//	UBOOL bCtrlDown = Viewport->KeyState(KEY_LeftControl) || Viewport->KeyState(KEY_RightControl);
//	UBOOL bShiftDown = Viewport->KeyState(KEY_LeftShift) || Viewport->KeyState(KEY_RightShift);
//	UBOOL bLightMoveDown = Viewport->KeyState(KEY_L);
//	UBOOL bWindMoveDown = Viewport->KeyState(KEY_W);
//	UBOOL bRightMouseDown = Viewport->KeyState(KEY_RightMouseButton);
//
//	// Look at which axis is being dragged and by how much
//	FLOAT DragX = (Key == KEY_MouseX) ? Delta : 0.f;
//	FLOAT DragY = (Key == KEY_MouseY) ? Delta : 0.f;
//
//	if (bDraggingTrackPos)
//	{
//		FLOAT NewPos = AnimSetViewer->PreviewAnimNode->CurrentTime + (DragX/400.f * AnimSetViewer->SelectedAnimSeq->SequenceLength);
//		AnimSetViewer->PreviewAnimNode->SetPosition(NewPos,FALSE);
//		AnimSetViewer->PreviewAnimNodeRaw->SetPosition(NewPos,FALSE);
//	}
//	else if (DraggingNotifyIndex != -1)
//	{
//		if (DraggingNotifyIndex >= 0 && DraggingNotifyIndex < AnimSetViewer->SelectedAnimSeq->Notifies.Num())
//		{
//			// check to see if we're dragging the tail, and should update duration instead of time
//			if (bDraggingNotifyTail)
//			{
//				FLOAT &Duration = AnimSetViewer->SelectedAnimSeq->Notifies(DraggingNotifyIndex).Duration;
//				Duration = Max(0.f,Duration + (DragX/400.f * AnimSetViewer->SelectedAnimSeq->SequenceLength));
//				// clamp by the end of the anim seq
//				FLOAT Time = AnimSetViewer->SelectedAnimSeq->Notifies(DraggingNotifyIndex).Time;
//				if (Time + Duration > AnimSetViewer->SelectedAnimSeq->SequenceLength)
//				{
//					Duration = AnimSetViewer->SelectedAnimSeq->SequenceLength - Time;
//				}
//				// if shift is down then match the anim track pos
//				if (bShiftDown)
//				{
//					AnimSetViewer->PreviewAnimNode->SetPosition(Time,FALSE);
//					AnimSetViewer->PreviewAnimNodeRaw->SetPosition(Time,FALSE);
//				}
//			}
//			else
//			{
//				FLOAT &Time = AnimSetViewer->SelectedAnimSeq->Notifies(DraggingNotifyIndex).Time;
//				Time = Clamp<FLOAT>(Time + (DragX/400.f * AnimSetViewer->SelectedAnimSeq->SequenceLength),0.f,AnimSetViewer->SelectedAnimSeq->SequenceLength);
//				// check to see if the duration needs to be clamped
//				FLOAT &Duration = AnimSetViewer->SelectedAnimSeq->Notifies(DraggingNotifyIndex).Duration;
//				if (Time + Duration > AnimSetViewer->SelectedAnimSeq->SequenceLength)
//				{
//					debugf(TEXT("clamping: %.2f vs %.2f"),Time+Duration,AnimSetViewer->SelectedAnimSeq->SequenceLength);
//					Duration = AnimSetViewer->SelectedAnimSeq->SequenceLength - Time;
//				}
//				// if shift is down then match the anim track pos
//				if (bShiftDown)
//				{
//					AnimSetViewer->PreviewAnimNode->SetPosition(Time,FALSE);
//					AnimSetViewer->PreviewAnimNodeRaw->SetPosition(Time,FALSE);
//				}
//			}
//		}
//	}
//	else if( DraggingMetadataKeyIndex != -1 )
//	{
//		if (DraggingMetadataIndex >= 0 && DraggingMetadataIndex < AnimSetViewer->SelectedAnimSeq->MetaData.Num())
//		{
//			UAnimMetaData_SkelControlKeyFrame* SkelControlKeyFrame = Cast<UAnimMetaData_SkelControlKeyFrame>(AnimSetViewer->SelectedAnimSeq->MetaData(DraggingMetadataIndex));
//			if( SkelControlKeyFrame )
//			{
//				FLOAT &Time = SkelControlKeyFrame->KeyFrames(DraggingMetadataKeyIndex).Time;
//				Time = Clamp<FLOAT>(Time + (DragX/200.f * AnimSetViewer->SelectedAnimSeq->SequenceLength),0.f,AnimSetViewer->SelectedAnimSeq->SequenceLength);
//				// if shift is down then match the anim track pos
//				if (bShiftDown)
//				{
//					AnimSetViewer->PreviewAnimNode->SetPosition(Time,FALSE);
//					AnimSetViewer->PreviewAnimNodeRaw->SetPosition(Time,FALSE);
//				}
//			}
//		}
//	}
//	else if(bManipulating)
//	{
//		FLOAT DragMag = (DragX * DragDirX) + (DragY * DragDirY);
//
//		if(AnimSetViewer->SocketMgr && AnimSetViewer->SelectedSocket && SocketManipulateAxis != AXIS_None)
//		{
//			if(AnimSetViewer->SocketMgr->MoveMode == SMM_Rotate)
//			{
//				FQuat CurrentQuat = AnimSetViewer->SelectedSocket->RelativeRotation.Quaternion();
//				FQuat DeltaQuat( LocalManDir, -DragMag * AnimSetViewer_RotateSpeed );
//				FQuat NewQuat = CurrentQuat * DeltaQuat;
//
//				AnimSetViewer->SelectedSocket->RelativeRotation = FRotator(NewQuat);
//
//				AnimSetViewer->SocketMgr->UpdateTextEntry();
//			}
//			else
//			{
//				FRotationMatrix SocketRotTM( AnimSetViewer->SelectedSocket->RelativeRotation );
//				FVector SocketMove = SocketRotTM.TransformNormal( LocalManDir * DragMag * AnimSetViewer_TranslateSpeed );
//
//				AnimSetViewer->SelectedSocket->RelativeLocation += SocketMove;
//			}
//
//			AnimSetViewer->UpdateSocketPreviews();
//
//			AnimSetViewer->SelectedSkelMesh->MarkPackageDirty();
//		}
//		
//		if (BoneManipulateAxis != AXIS_None && AnimSetViewer->PreviewSkelComp && AnimSetViewer->PreviewSkelComp->BonesOfInterest.Num() == 1)
//		{
//			//Get the skel control manipulating this bone
//			FSkelControlListHead& SkelListControl =  AnimSetViewer->PreviewAnimTree->SkelControlLists(0);
//			USkelControlSingleBone* SkelControl = Cast<USkelControlSingleBone>(SkelListControl.ControlHead);
//			if (SkelControl)
//			{
//				//Get the accumulation from control up to this point
//				FRotationTranslationMatrix CurrentSkelControlMat(SkelControl->BoneRotation, SkelControl->BoneTranslation);
//
//				if(AnimSetViewer->BoneMoveMode == SMM_Rotate)
//				{
//					FLOAT ManipulateRotation = -AnimSetViewer_RotateSpeed * DragMag;
//					ManipulateRotation = AnimSetViewer_RotateSpeed * appFloor(ManipulateRotation/AnimSetViewer_RotateSpeed);
//
//					//Calculate the new delta rotation
//					FQuat DeltaQuat(LocalManDir, ManipulateRotation);
//					FQuatRotationTranslationMatrix ManipulateMatrix( DeltaQuat, FVector(0.f) );
//					SkelControl->BoneRotation = (ManipulateMatrix * CurrentSkelControlMat).Rotator();
//				}
//				else
//				{
//					FLOAT ManipulateTranslation = AnimSetViewer_TranslateSpeed * DragMag;
//					ManipulateTranslation = AnimSetViewer_TranslateSpeed * appFloor(ManipulateTranslation/AnimSetViewer_TranslateSpeed);
//					
//					//Calculate the new orientation of the axis after skel control 
//					FVector BoneSpaceDir = CurrentSkelControlMat.TransformNormal(LocalManDir);
//					SkelControl->BoneTranslation += (BoneSpaceDir * ManipulateTranslation);
//				}
//			}
//		}
//		if (AnimSetViewer->SelectedSocket)
//		{
//			GCallbackEvent->Send( CALLBACK_ObjectPropertyChanged, AnimSetViewer->SelectedSocket );
//		}
//	}
//	else if(bLightMoveDown)
//	{
//		FRotator LightDir = PreviewScene.GetLightDirection();
//		
//		LightDir.Yaw += -DragX * AnimSetViewer_LightRotSpeed;
//		LightDir.Pitch += -DragY * AnimSetViewer_LightRotSpeed;
//
//		PreviewScene.SetLightDirection(LightDir);
//	}
//	else if(bWindMoveDown)
//	{
//		// Display parameter in the preview window for Apex Clothing Wind
//		if(bRightMouseDown)
//		{
//
//			// If right mouse button and ctrl is down, turn Y movement into modifying windvelocityblendtime
//			if(bCtrlDown)
//			{
//#if WITH_APEX
//				AnimSetViewer->WindVelocityBlendTime = ::Max(AnimSetViewer->WindVelocityBlendTime + DragY * AnimSetViewer_WindVelocityBlendTimeSpeed, 0.0f);
//#endif
//			}
//			// If right mouse button is down, turn Y movement into modifying strength
//			else
//			{
//				AnimSetViewer->WindStrength = ::Max(AnimSetViewer->WindStrength + (DragY * AnimSetViewer_WindStrengthSpeed), 0.f);
//			}
//		}
//		// Otherwise, orbit the wind
//		else
//		{
//			AnimSetViewer->WindRot.Yaw += -DragX * AnimSetViewer_LightRotSpeed;
//			AnimSetViewer->WindRot.Pitch += -DragY * AnimSetViewer_LightRotSpeed;
//		}
//
//		AnimSetViewer->UpdateClothWind();
//	}
//	// If we are not manipulating an axis, use the MouseDeltaTracker to update camera.
//	else
//	{
		//MouseDeltaTracker->AddDelta( this, Key, Delta, 0 );
		//const FVector DragDelta = MouseDeltaTracker->GetDelta();

		//GEditor->MouseMovement += DragDelta;

		//if( !DragDelta.IsZero() )
		//{
		//	// Convert the movement delta into drag/rotation deltas
		//	if ( bAllowMayaCam && GEditor->bUseMayaCameraControls )
		//	{
		//		FVector TempDrag;
		//		FRotator TempRot;
		//		InputAxisMayaCam( Viewport, DragDelta, TempDrag, TempRot );
		//	}
		//	else
		//	{
		//		FVector Drag;
		//		FRotator Rot;
		//		FVector Scale;
		//		MouseDeltaTracker->ConvertMovementDeltaToDragRot( this, DragDelta, Drag, Rot, Scale );
		//		MoveViewportCamera( Drag, Rot );
		//	}

		//	MouseDeltaTracker->ReduceBy( DragDelta );
		//}
	//}

	Viewport->Invalidate();

	return TRUE;
}

void FEditorAbilityViewportClient::MouseMove(FViewport* Viewport,INT x, INT y)
{
	AbilityViewer->MouseOverSortStripIndex = INDEX_NONE;
	AbilityViewer->bSortStripMoveForward = AbilityViewer->SelectedSortStripIndex != INDEX_NONE && Viewport->KeyState(KEY_F);
	AbilityViewer->bSortStripMoveBackward = AbilityViewer->SelectedSortStripIndex != INDEX_NONE && Viewport->KeyState(KEY_B);

	//// If we are not currently moving the widget - update the ManipulateAxis to the one we are mousing over.
	//if(!bManipulating)
	//{
	//	INT	HitX = Viewport->GetMouseX();
	//	INT HitY = Viewport->GetMouseY();

	//	HHitProxy*	HitResult = Viewport->GetHitProxy(HitX,HitY);
	//	if(HitResult && HitResult->IsA(HWidgetUtilProxy::StaticGetType()))
	//	{
	//		HWidgetUtilProxy* WidgetProxy = (HWidgetUtilProxy*)HitResult;
	//		if (WidgetProxy->Info1 == HITPROXY_BONE)
	//		{  
	//			BoneManipulateAxis = WidgetProxy->Axis;
	//			SocketManipulateAxis = AXIS_None;
	//		}
	//		else
	//		{
	//			BoneManipulateAxis = AXIS_None;
	//			SocketManipulateAxis = WidgetProxy->Axis;
	//		}
	//	}
	//	else
	//	if( HitResult && HitResult->IsA(HSortStripProxy::StaticGetType()) )
	//	{
	//		if( AbilityViewer->bSortStripMoveBackward || AbilityViewer->bSortStripMoveForward )
	//		{
	//			HSortStripProxy* SortStripProxy = (HSortStripProxy*)HitResult;
	//			if( SortStripProxy->LODModelIndex == AbilityViewer->SelectedSortStripLodIndex &&
	//				SortStripProxy->SectionIndex == AbilityViewer->SelectedSortStripSectionIndex )
	//			{
	//				AbilityViewer->MouseOverSortStripIndex = SortStripProxy->StripIndex;
	//			}			
	//		}
	//	}
	//	else 
	//	{
	//		BoneManipulateAxis = AXIS_None;
	//		SocketManipulateAxis = AXIS_None;
	//	}
	//}
}

/*-----------------------------------------------------------------------------
	UASVSkelComponent
-----------------------------------------------------------------------------*/

 /** Create the scene proxy needed for rendering a ASV skeletal mesh */
 FPrimitiveSceneProxy* UAbilitySkelComponent::CreateSceneProxy()
 {
 	//FAbilitySkelSceneProxy* Result = NULL;
 
 	//// only create a scene proxy for rendering if
 	//// properly initialized
 	//if( SkeletalMesh && 
 	//	SkeletalMesh->LODModels.IsValidIndex(PredictedLODLevel) &&
 	//	!bHideSkin &&
 	//	MeshObject )
 	//{
 	//	const FColor WireframeMeshOverlayColor(102,205,170,255);
 	//	Result = ::new FAbilitySkelSceneProxy(this, BoneColor, WireframeMeshOverlayColor);
 	//}
 
 	//return Result;

	 return NULL;
 }
 
 /**
  * Function that returns whether or not CPU skinning should be applied
  * Allows the editor to override the skinning state for editor tools
  */
 UBOOL UAbilitySkelComponent::ShouldCPUSkin()
 {
 	return (ColorRenderMode!=ESCRM_None);
 }
 
 /** 
  * Function to operate on mesh object after its created, 
  * but before it's attached.
  * @param MeshObject - Mesh Object owned by this component
  */
 void UAbilitySkelComponent::PostInitMeshObject(FSkeletalMeshObject* MeshObject)
 {
 	if (MeshObject)
 	{
 		//Setup the bone weights we want to render
 		switch (ColorRenderMode)
 		{
 		case ESCRM_VertexTangent:
 		case ESCRM_VertexNormal:
 		case ESCRM_VertexMirror:
 			MeshObject->EnableColorModeRendering((SkinColorRenderMode)ColorRenderMode);
 			break;
 		case ESCRM_BoneWeights:
 			MeshObject->EnableBlendWeightRendering(TRUE , BonesOfInterest);
 			break;
 		default:
 			MeshObject->EnableColorModeRendering(ESCRM_None);
 			break;
 		}
 	}
 }
 
 /**
 * Update material information depending on color render mode 
 * Refresh/replace materials 
 */
 void UAbilitySkelComponent::ApplyColorRenderMode(INT InColorRenderMode)
 {
 	SkinColorRenderMode OldColorRenderMode = (SkinColorRenderMode)ColorRenderMode;
 	if ( InColorRenderMode >= ESCRM_None && InColorRenderMode<ESCRM_Max )
 	{
 		ColorRenderMode = InColorRenderMode;
 	}
 
 	if ( OldColorRenderMode == ESCRM_None  && ColorRenderMode != ESCRM_None)
 	{
 		// back up all materials
 		SkelMaterials.Empty();
 		INT NumMaterials = GetNumElements();
 		for (INT i=0; i<NumMaterials; i++)
 		{
 			SkelMaterials.AddItem(GetMaterial(i));
 		}
 
 		// add mesh edges
 		WxAbilityViewer* ASV = (WxAbilityViewer*)AnimSetViewerPtr;
 		ASV->PreviewWindow->ASVPreviewVC->ShowFlags |= SHOW_MeshEdges;
 	}
 
 	switch (ColorRenderMode)
 	{
 	case ESCRM_VertexTangent:
 		{
 			INT NumMaterials = GetNumElements();
 			for (INT i=0; i<NumMaterials; i++)
 			{
 				SetMaterial(i, GEngine->TangentColorMaterial);
 			}
 		}
 		break;
 	case ESCRM_VertexNormal:
 	case ESCRM_VertexMirror:
 	case ESCRM_BoneWeights:
 		{
 			INT NumMaterials = GetNumElements();
 			for (INT i=0; i<NumMaterials; i++)
 			{
 				SetMaterial(i, GEngine->BoneWeightMaterial);
 			}
 		}
 		break;
 	default:
 		{
 			if ( SkelMaterials.Num() > 0 )
 			{
 				INT NumMaterials = GetNumElements();
 				check(NumMaterials == SkelMaterials.Num());
 				for (INT i=0; i<NumMaterials; i++)
 				{
 					SetMaterial(i, SkelMaterials(i));
 				}
 
 			}
 
 			SkelMaterials.Empty();
 
 			// remove mesh edges
 			WxAbilityViewer* ASV = (WxAbilityViewer*)AnimSetViewerPtr;
 			ASV->PreviewWindow->ASVPreviewVC->ShowFlags &= ~SHOW_MeshEdges;
 		}
 		break;
 	}
 }

/*-----------------------------------------------------------------------------
	WxAbilityPreview
-----------------------------------------------------------------------------*/

WxAbilityPreview::WxAbilityPreview(wxWindow* InParent, wxWindowID InID, class WxAbilityViewer* InASV)
:	wxWindow( InParent, InID )
,	ASVPreviewVC( NULL )
{
	CreateViewport( InASV );
}

WxAbilityPreview::~WxAbilityPreview()
{
	DestroyViewport();
}

/**
 * Calls DestoryViewport(), then creates a new viewport client and associated viewport.
 */
void WxAbilityPreview::CreateViewport(class WxAbilityViewer* InASV)
{
	DestroyViewport();

	ASVPreviewVC = new FEditorAbilityViewportClient(InASV);
	ASVPreviewVC->Viewport = GEngine->Client->CreateWindowChildViewport(ASVPreviewVC, (HWND)GetHandle());
	check( ASVPreviewVC->Viewport );
	ASVPreviewVC->Viewport->CaptureJoystickInput(false);
}

/**
 * Destroys any existing viewport client and associated viewport.
 */
void WxAbilityPreview::DestroyViewport()
{
	if ( ASVPreviewVC )
	{
		GEngine->Client->CloseViewport( ASVPreviewVC->Viewport );
		ASVPreviewVC->Viewport = NULL;

		delete ASVPreviewVC;
		ASVPreviewVC = NULL;
	}
}

void WxAbilityPreview::OnSize( wxSizeEvent& In )
{
	if ( ASVPreviewVC )
	{
		checkSlow( ASVPreviewVC->Viewport );
		const wxRect rc = GetClientRect();
		::MoveWindow( (HWND)ASVPreviewVC->Viewport->GetWindow(), 0, 0, rc.GetWidth(), rc.GetHeight(), 1 );

		ASVPreviewVC->Viewport->Invalidate();
	}
}

BEGIN_EVENT_TABLE( WxAbilityPreview, wxWindow )
	EVT_SIZE( WxAbilityPreview::OnSize )
END_EVENT_TABLE()

/*-----------------------------------------------------------------------------
	WxAbilityViewer
-----------------------------------------------------------------------------*/


BEGIN_EVENT_TABLE( WxAbilityViewer, WxTrackableFrame )

	EVT_COMMAND_RANGE( IDM_EFFECT_NODE_ADD_START, IDM_EFFECT_NODE_ADD_END, wxEVT_COMMAND_MENU_SELECTED, WxAbilityViewer::OnAddEffectNode)

	EVT_MENU(IDM_SELECT_WEAPON_TYPE, WxAbilityViewer::OnSelectWeaponType)
	EVT_MENU(IDM_ADD_WEAPON_TYPE, WxAbilityViewer::OnAddWeaponType)
	EVT_MENU(IDM_DELETE_WEAPON_TYPE, WxAbilityViewer::OnDeleteWeaponType)
	EVT_MENU(IDM_COPY_DEFAULT_LIST_TO_CURRENT_LIST, WxAbilityViewer::OnCopyDefaultComponentsToCurrent)
	EVT_MENU(IMD_COPY_DEFAULT_SOUND_TO_CURRENT_LIST, WxAbilityViewer::OnCopyDefaultSoundComponentsToCurrent)
	EVT_MENU(IDM_COPY_CURRENT_COMPONENT, WxAbilityViewer::OnCopyComponent)
	EVT_MENU(IDM_PASTY_CURRENT_COMPONENT, WxAbilityViewer::OnPasteComponent)


	EVT_COMMAND_RANGE( IDM_EFFECT_NODE_SHOW_START, IDM_EFFECT_NODE_SHOW_END, wxEVT_COMMAND_MENU_SELECTED, WxAbilityViewer::OnShowEffectNode)

	EVT_MENU(IDM_SHOW_CALC_SIMULATE, WxAbilityViewer::OnShowCalcSimulate)
	

	EVT_BUTTON(IDB_SUB_ABILITY_TIME, WxAbilityViewer::OnAddTrackLine)
	EVT_BUTTON(IDB_ADD_ABILITY_TIME, WxAbilityViewer::OnSubTrackLine)
	EVT_BUTTON(IDB_SWITCH_EDIT_STATE, WxAbilityViewer::SwithEditState)


	EVT_SIZE(WxAbilityViewer::OnSize)
	EVT_BUTTON( IDM_ANIMSET_SKELMESHUSE, WxAbilityViewer::OnSkelMeshUse )
	EVT_COMBOBOX( IDM_ANIMSET_ANIMSETCOMBO, WxAbilityViewer::OnAnimSetComboChanged )
	EVT_BUTTON( IDM_ANIMSET_ANIMSETUSE, WxAbilityViewer::OnAnimSetUse )
	EVT_BUTTON( IDM_ANIMSET_ANIMSEQSEARCHALL, WxAbilityViewer::OnAnimSeqSearchTypeChange )
	EVT_TEXT( IDM_ANIMSET_ANIMSEQFILTER, WxAbilityViewer::OnAnimSeqFilterTextChanged )
	EVT_TEXT_ENTER( IDM_ANIMSET_ANIMSEQFILTER, WxAbilityViewer::OnAnimSeqFilterEnterPressed )
	EVT_BUTTON( IDM_ANIMSET_ANIMSEQCLEARFILTER, WxAbilityViewer::OnClearAnimSeqFilter )
	EVT_UPDATE_UI( IDM_ANIMSET_ANIMSEQCLEARFILTER, WxAbilityViewer::OnClearAnimSeqFilter_UpdateUI)
	EVT_TIMER( IDM_ANIMSET_ANIMSEQFILTERTIMER, WxAbilityViewer::OnAnimSeqFilterTimer )
	EVT_LISTBOX( IDM_ANIMSET_ANIMSEQLIST, WxAbilityViewer::OnAnimSeqListChanged )
#if WITH_ACTORX
	EVT_MENU( IDM_ANIMSET_IMPORTPSA, WxAbilityViewer::OnImportPSA )
#endif
#if WITH_FBX
	EVT_MENU( IDM_ANIMSET_IMPORTFBXANIM, WxAbilityViewer::OnImportFbxAnim )
	EVT_MENU( IDM_ANIMSET_EXPORTFBXANIM, WxAbilityViewer::OnExportFbxAnim )
#endif // WITH_FBX
	EVT_MENU( IDM_ANIMSET_UNDO, WxAbilityViewer::OnMenuEditUndo )
	EVT_MENU( IDM_ANIMSET_REDO, WxAbilityViewer::OnMenuEditRedo )
#if WITH_SIMPLYGON
	EVT_MENU( IDM_ANIMSET_GENERATELOD, WxAbilityViewer::OnGenerateLOD )
#endif // #if WITH_SIMPLYGON
	EVT_MENU( IDM_ANIMSET_IMPORTMESHLOD, WxAbilityViewer::OnImportMeshLOD )
	EVT_MENU( IDM_ANIMSET_IMPORTMESHWEIGHTS, WxAbilityViewer::OnImportMeshWeights )	
	EVT_MENU( IDM_ANIMSET_TOGGLEMESHWEIGHTS, WxAbilityViewer::OnToggleMeshWeights )		
	EVT_MENU( IDM_ANIMSET_BONEEDITING_SHOWALLMESHVERTS, WxAbilityViewer::OnToggleShowAllMeshVerts )	
	EVT_MENU( IDM_ANIMSET_NEWANISMET, WxAbilityViewer::OnNewAnimSet )
	EVT_MENU( IDM_ANIMSET_NEWMORPHSET, WxAbilityViewer::OnNewMorphTargetSet )
	EVT_MENU( IDM_ANIMSET_IMPORTMORPHTARGET, WxAbilityViewer::OnImportMorphTarget )
	EVT_MENU( IDM_ANIMSET_IMPORTMORPHTARGETS, WxAbilityViewer::OnImportMorphTargets )
	EVT_MENU( IDM_ANIMSET_IMPORTMORPHTARGET_LOD, WxAbilityViewer::OnImportMorphTargetLOD )		
	EVT_MENU( IDM_ANIMSET_IMPORTMORPHTARGETS_LOD, WxAbilityViewer::OnImportMorphTargetsLOD )		
	EVT_COMBOBOX( IDM_ANIMSET_MORPHSETCOMBO, WxAbilityViewer::OnMorphSetComboChanged )
	EVT_BUTTON( IDM_ANIMSET_MORPHSETUSE, WxAbilityViewer::OnMorphSetUse )
	EVT_COMMAND_RANGE( IDM_ANIMSET_MORPHTARGETS_START, IDM_ANIMSET_MORPHTARGETS_END, wxEVT_COMMAND_TEXT_UPDATED, WxAbilityViewer::OnMorphTargetTextChanged )
	EVT_COMMAND_RANGE( IDM_ANIMSET_MORPHTARGETWEIGHTS_START, IDM_ANIMSET_MORPHTARGETWEIGHTS_END, wxEVT_COMMAND_SLIDER_UPDATED, WxAbilityViewer::OnMorphTargetWeightChanged )
	EVT_COMMAND_RANGE( IDM_ANIMSET_MORPHTARGETSELECT_START, IDM_ANIMSET_MORPHTARGETSELECT_END, wxEVT_COMMAND_CHECKBOX_CLICKED, WxAbilityViewer::OnSelectMorphTarget )

	EVT_BUTTON(IDM_ANIMSET_RESETMORPHTARGETPREVIEW, WxAbilityViewer::OnResetMorphTargetPreview)
	EVT_CHECKBOX(IDM_ANIMSET_SELECTALLMORPHTARGETS, WxAbilityViewer::OnSelectAllMorphTargets)
	EVT_COMMAND_SCROLL( IDM_ANIMSET_TIMESCRUB, WxAbilityViewer::OnTimeScrub )
	EVT_MENU( IDM_ANIMSET_VIEWBONES, WxAbilityViewer::OnViewBones )
	EVT_MENU( IDM_ANIMSET_ShowRawAnimation, WxAbilityViewer::OnShowRawAnimation )
	EVT_MENU( IDM_ANIMSET_VIEWBONENAMES, WxAbilityViewer::OnViewBoneNames )
	EVT_MENU( IDM_ANIMSET_VIEWFLOOR, WxAbilityViewer::OnViewFloor )
	EVT_MENU( IDM_ANIMSET_VIEWWIREFRAME, WxAbilityViewer::OnViewWireframe )
	EVT_MENU( IDM_ANIMSET_VIEWADDITIVEBASE, WxAbilityViewer::OnViewAdditiveBase )
	EVT_MENU( IDM_ANIMSET_VIEWGRID, WxAbilityViewer::OnViewGrid )
	EVT_MENU( IDM_ANIMSET_VIEWSOCKETS, WxAbilityViewer::OnViewSockets )
	EVT_MENU( IDM_ANIMSET_VIEWMORPHKEYS, WxAbilityViewer::OnViewMorphKeys )
	EVT_MENU( IDM_ANIMSET_VIEWREFPOSE, WxAbilityViewer::OnViewRefPose )
	EVT_MENU( IDM_ANIMSET_VIEWMIRROR, WxAbilityViewer::OnViewMirror )
	EVT_MENU( IDM_ANIMSET_VIEWBOUNDS, WxAbilityViewer::OnViewBounds )
	EVT_MENU( IDM_ANIMSET_VIEWCOLLISION, WxAbilityViewer::OnViewCollision )
	EVT_MENU( IDM_ANIMSET_VIEWSOFTBODYTETRA, WxAbilityViewer::OnViewSoftBodyTetra )
	EVT_MENU( IDM_ANIMSET_VIEWALTBONEWEIGHTINGMODE, WxAbilityViewer::OnEditAltBoneWeightingMode )
	EVT_MENU( IDM_ANIMSET_VIEWCLOTHMOVEDISTSCALE, WxAbilityViewer::OnViewClothMoveDistScale )
	EVT_MENU( IDM_ANIMSET_VERTEX_SHOWTANGENT_AS_VECTOR, WxAbilityViewer::OnViewVertexMode )
	EVT_MENU( IDM_ANIMSET_VERTEX_SHOWTANGENT_AS_TEXTURE, WxAbilityViewer::OnViewVertexMode )
	EVT_MENU( IDM_ANIMSET_VERTEX_SHOWNORMAL_AS_VECTOR, WxAbilityViewer::OnViewVertexMode )
	EVT_MENU( IDM_ANIMSET_VERTEX_SHOWNORMAL_AS_TEXTURE, WxAbilityViewer::OnViewVertexMode )
	EVT_MENU( IDM_ANIMSET_VERTEX_SHOWMIRROR, WxAbilityViewer::OnViewVertexMode )
	EVT_MENU( IDM_ANIMSET_VERTEX_SECTION_0, WxAbilityViewer::OnSectionSelected )
	EVT_MENU( IDM_ANIMSET_VERTEX_SECTION_1, WxAbilityViewer::OnSectionSelected )
	EVT_MENU( IDM_ANIMSET_VERTEX_SECTION_2, WxAbilityViewer::OnSectionSelected )
	EVT_MENU( IDM_ANIMSET_VERTEX_SECTION_3, WxAbilityViewer::OnSectionSelected )
	EVT_MENU( IDM_ANIMSET_VERTEX_SECTION_4, WxAbilityViewer::OnSectionSelected )
	EVT_BUTTON( IDM_ANIMSET_LOOPBUTTON, WxAbilityViewer::OnLoopAnim )
	EVT_BUTTON( IDM_ANIMSET_PLAYBUTTON, WxAbilityViewer::OnPlayAnim )
	EVT_MENU( IDM_ANIMSET_EMPTYSET, WxAbilityViewer::OnEmptySet )
	EVT_MENU_RANGE( IDM_ANIMSET_SHOW_UV_START, IDM_ANIMSET_SHOW_UV_END, WxAbilityViewer::OnShowUVSet )
	EVT_UPDATE_UI_RANGE( IDM_ANIMSET_SHOW_UV_START, IDM_ANIMSET_SHOW_UV_END, WxAbilityViewer::OnShowUVSet_UpdateUI )
	EVT_MENU( IDM_ANIMSET_DELETETRACK, WxAbilityViewer::OnDeleteTrack )
	EVT_MENU( IDM_ANIMSET_DELETEMORPHTRACK, WxAbilityViewer::OnDeleteMorphTrack )
	EVT_MENU( IDM_ANIMSET_COPYTRANSLATIONBONENAMES, WxAbilityViewer::OnCopyTranslationBoneNames )
	EVT_MENU( IDM_ANIMSET_ANALYZEANIMSET, WxAbilityViewer::OnAnalyzeAnimSet )
	EVT_MENU( IDM_ANIMSET_RENAMESEQ, WxAbilityViewer::OnRenameSequence )
	EVT_MENU( IDM_ANIMSET_REMOVE_PREFIX, WxAbilityViewer::OnRemovePrefixFromSequences )
	EVT_MENU( IDM_ANIMSET_DELETESEQ, WxAbilityViewer::OnDeleteSequence )
	EVT_MENU( IDM_ANIMSET_COPYSEQ, WxAbilityViewer::OnCopySequence )
	EVT_MENU( IDM_ANIMSET_MOVESEQ, WxAbilityViewer::OnMoveSequence )
	EVT_MENU( IDM_ANIMSET_MAKESEQADDITIVE, WxAbilityViewer::OnMakeSequencesAdditive )
	EVT_MENU( IDM_ANIMSET_ADDADDITIVETOSEQ, WxAbilityViewer::OnAddAdditiveAnimationToSelectedSequence )
	EVT_MENU( IDM_ANIMSET_REBUILDADDITIVE, WxAbilityViewer::OnRebuildAdditiveAnimation )
	EVT_MENU( IDM_ANIMSET_SUBTRACTADDITIVETOSEQ, WxAbilityViewer::OnAddAdditiveAnimationToSelectedSequence )
	EVT_MENU( IDM_ANIMSET_SEQAPPLYROT, WxAbilityViewer::OnSequenceApplyRotation )
	EVT_MENU( IDM_ANIMSET_SEQREZERO, WxAbilityViewer::OnSequenceReZero )
	EVT_MENU( IDM_ANIMSET_SEQDELBEFORE, WxAbilityViewer::OnSequenceCrop )
	EVT_MENU( IDM_ANIMSET_SEQDELAFTER, WxAbilityViewer::OnSequenceCrop )
	EVT_MENU( IDM_ANIMSET_SEQDELBEGINBYTIME, WxAbilityViewer::OnSequenceCropByFrames )
	EVT_MENU( IDM_ANIMSET_SEQDELENDBYTIME, WxAbilityViewer::OnSequenceCropByFrames )
	EVT_MENU( IDM_ANIMSET_NOTIFYNEW, WxAbilityViewer::OnNewNotify )
	EVT_MENU( IDM_ANIMSET_NOTIFYSORT, WxAbilityViewer::OnNotifySort )
	EVT_MENU( IDM_ANIMSET_NOTIFY_ENABLEALLPSYS, WxAbilityViewer::OnAllParticleNotifies )
	EVT_MENU( IDM_ANIMSET_NOTIFY_DISABLEALLPSYS, WxAbilityViewer::OnAllParticleNotifies )
	EVT_MENU( IDM_ANIMSET_COPYSEQNAME, WxAbilityViewer::OnCopySequenceName )
	EVT_MENU( IDM_ANIMSET_COPYSEQNAMELIST, WxAbilityViewer::OnCopySequenceNameList )
	EVT_MENU( IDM_ANIMSET_UPDATEBOUNDS, WxAbilityViewer::OnUpdateBounds )
	EVT_MENU( IDM_ANIMSET_COPYMESHBONES, WxAbilityViewer::OnCopyMeshBoneNames )
	EVT_MENU( IDM_ANIMSET_COPYWEIGHTEDMESHBONES, WxAbilityViewer::OnCopyWeightedMeshBoneNames )
	EVT_MENU( IDM_ANIMSET_FIXUPMESHBONES, WxAbilityViewer::OnFixupMeshBoneNames )
	EVT_MENU( IDM_ANIMSET_SWAPLODSECTIONS, WxAbilityViewer::OnSwapLODSections )
	EVT_MENU( IDM_ANIMSET_MERGEMATERIALS, WxAbilityViewer::OnMergeMaterials )
	EVT_MENU( IDM_ANIMSET_AUTOMIRRORTABLE, WxAbilityViewer::OnAutoMirrorTable )
	EVT_MENU( IDM_ANIMSET_CHECKMIRRORTABLE, WxAbilityViewer::OnCheckMirrorTable )
	EVT_MENU( IDM_ANIMSET_COPYMIRRORTABLE, WxAbilityViewer::OnCopyMirrorTable )
	EVT_MENU( IDM_ANIMSET_COPYMIRRORTABLEFROM, WxAbilityViewer::OnCopyMirroTableFromMesh )
	EVT_MENU( IDM_ANIMSET_NOTIFYCOPY, WxAbilityViewer::OnNotifyCopy )
	EVT_MENU( IDM_ANIMSET_NOTIFYSHIFT, WxAbilityViewer::OnNotifyShift )
	EVT_TOOL( IDM_ANIMSET_OPENSOCKETMGR, WxAbilityViewer::OnOpenSocketMgr )
	EVT_TOOL( IDM_ANIMSET_OpenAnimationCompressionDlg, WxAbilityViewer::OnOpenAnimationCompressionDlg )
	EVT_BUTTON( IDM_ANIMSET_NEWSOCKET, WxAbilityViewer::OnNewSocket )
	EVT_BUTTON( IDM_ANIMSET_DELSOCKET, WxAbilityViewer::OnDeleteSocket )
	EVT_LISTBOX( IDM_ANIMSET_SOCKETLIST, WxAbilityViewer::OnClickSocket )
	EVT_LISTBOX_DCLICK( IDM_ANIMSET_SOCKETLIST, WxAbilityViewer::OnRenameSockets )
	EVT_TOOL( IDM_ANIMSET_SOCKET_TRANSLATE, WxAbilityViewer::OnSocketMoveMode )
	EVT_TOOL( IDM_ANIMSET_SOCKET_ROTATE, WxAbilityViewer::OnSocketMoveMode )
	EVT_TOOL( IDM_ANIMSET_CLEARPREVIEWS, WxAbilityViewer::OnClearPreviewMeshes )
	EVT_TOOL( IDM_ANIMSET_COPYSOCKETS, WxAbilityViewer::OnCopySockets )
	EVT_TOOL( IDM_ANIMSET_PASTESOCKETS, WxAbilityViewer::OnPasteSockets )
	EVT_MENU( IDM_ANIMSET_LOD_AUTO, WxAbilityViewer::OnForceLODLevel )
	EVT_MENU( IDM_ANIMSET_LOD_BASE, WxAbilityViewer::OnForceLODLevel )
	EVT_MENU( IDM_ANIMSET_LOD_1, WxAbilityViewer::OnForceLODLevel )
	EVT_MENU( IDM_ANIMSET_LOD_2, WxAbilityViewer::OnForceLODLevel )
	EVT_MENU( IDM_ANIMSET_LOD_3, WxAbilityViewer::OnForceLODLevel )
	EVT_MENU( IDM_ANIMSET_REMOVELOD,  WxAbilityViewer::OnRemoveLOD )
	EVT_COMBOBOX( IDM_ANIMSET_CHUNKS, WxAbilityViewer::OnViewChunk )
	EVT_COMBOBOX( IDM_ANIMSET_SECTIONS, WxAbilityViewer::OnViewSection )
	EVT_TOOL( IDM_ANIMSET_SPEED_100,	WxAbilityViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_50, WxAbilityViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_25, WxAbilityViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_10, WxAbilityViewer::OnSpeed )
	EVT_TOOL( IDM_ANIMSET_SPEED_1, WxAbilityViewer::OnSpeed )
	EVT_TOOL( IDM_RESET_CAMERA_POSITION, WxAbilityViewer::OnResetCameraPosition )
	EVT_TOOL(IDM_ROTATE_CAMERA, WxAbilityViewer::OnRotateCamera)

	//Bruce@snda
	EVT_TOOL( IDM_EXCHANGE_CONFIG, WxAbilityViewer::OnExchangeConfig )
	EVT_TOOL( IDM_REFRESH_VIEW_CONFIG, WxAbilityViewer::OnRefreshViewAndConfig)

	EVT_TOOL( IDM_WARRIOR_SETTINGS, WxAbilityViewer::OnWarriorSettings )
	EVT_TOOL( IDM_FEMALE_WARRIOR_SETTINGS, WxAbilityViewer::OnFemaleWarriorSettings )
	EVT_TOOL( IDM_WIZARD_SETTINGS, WxAbilityViewer::OnWizardSettings )
	EVT_TOOL( IDM_FEMALE_WIZARD_SETTINGS, WxAbilityViewer::OnFemaleWizardSettings )
	EVT_TOOL( IDM_ARCHER_SETTINGS, WxAbilityViewer::OnArcherSettings )
	EVT_TOOL( IDM_FEMALE_ARCHER_SETTINGS, WxAbilityViewer::OnFemaleArcherSettings )
	EVT_TOOL( IDM_ASSASSIN_SETTINGS, WxAbilityViewer::OnAssassinSettings )
	EVT_TOOL( IDM_FEMALE_ASSASSIN_SETTINGS, WxAbilityViewer::OnFemaleAssassinSettings )

	EVT_COMBOBOX( IDM_ABILITYEDITOR_SWITCH_WEAPON_TYPE, WxAbilityViewer::OnSwitchWeaponType )
	EVT_COMBOBOX( IDM_ABILITYEDITOR_WEAPON_TYPE, WxAbilityViewer::OnAddWeaponTypeByComboBox )
	//Bruce@snda

	EVT_MENU( IDM_ANIMSET_DELETE_MORPH, WxAbilityViewer::OnDeleteMorphTarget )
	EVT_MENU( IDM_ANIMSET_UPDATE_MORPHTARGET, WxAbilityViewer::OnUpdateMorphTarget)
	EVT_TOOL( IDM_ANIMSET_TOGGLECLOTH, WxAbilityViewer::OnToggleClothSim )
	EVT_TOOL( IDM_ANIMSET_SOFTBODYGENERATE, WxAbilityViewer::OnSoftBodyGenerate )
	EVT_TOOL( IDM_ANIMSET_SOFTBODYTOGGLESIM, WxAbilityViewer::OnSoftBodyToggleSim )


	EVT_TREE_ITEM_RIGHT_CLICK( IDM_ANIMSET_SKELETONTREECTRL, WxAbilityViewer::OnSkeletonTreeItemRightClick )
	EVT_TREE_SEL_CHANGED( IDM_ANIMSET_SKELETONTREECTRL, WxAbilityViewer::OnSkeletonTreeSelectionChange )
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_SHOWBONE, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_HIDEBONE, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_SETBONECOLOR, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_SHOWCHILDBONE, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_HIDECHILDBONE, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_SETCHILDBONECOLOR, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_COPYBONENAME, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_CALCULATEBONEBREAKS, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_CALCULATEBONEBREAKS_AUTODETECT, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_CALCULATEBONEBREAKS_RIGIDPREFERRED, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_DELETEBONEBREAK, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_RESETBONEBREAKS, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)
	EVT_MENU( IDM_ANIMSET_SKELETONTREE_SHOWBLENDWEIGHTS, WxAbilityViewer::OnSkeletonTreeMenuHandleCommand)

	EVT_TOOL(IDM_ANIMSET_COPY, WxAbilityViewer::OnCopySelectedAnimSeqToClipboard)

	EVT_COMMAND_SCROLL( IDM_ANIMSET_PROGRESSIVESLIDER, WxAbilityViewer::OnProgressiveSliderChanged )
	EVT_TOOL( IDM_ANIMSET_FOVRESET, WxAbilityViewer::OnFOVReset)
	EVT_COMMAND_SCROLL( IDM_ANIMSET_FOVSLIDER, WxAbilityViewer::OnFOVSliderChanged )
	EVT_TOOL( IDM_ANIMSET_TRIANGLESORTMODE, WxAbilityViewer::OnToggleTriangleSortMode )
	EVT_TOOL( IDM_ANIMSET_TRIANGLESORTMODELR, WxAbilityViewer::OnToggleTriangleSortModeLR )
	EVT_MENU( IDM_ANIMSET_ACTIVELY_LISTEN_FOR_FILE_CHANGES, WxAbilityViewer::OnToggleActiveFileListen)
	EVT_UPDATE_UI( IDM_ANIMSET_ACTIVELY_LISTEN_FOR_FILE_CHANGES, WxAbilityViewer::UI_ActiveFileListen)

	EVT_MENU(IDM_ANIMSET_PHYSX_CLEAR_ALL, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_WORLD_AXES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_BODY_AXES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_BODY_MASS_AXES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_BODY_LIN_VELOCITY, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_BODY_ANG_VELOCITY, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_BODY_JOINT_GROUPS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_JOINT_LOCAL_AXES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_JOINT_WORLD_AXES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_JOINT_LIMITS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CONTACT_POINT, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CONTACT_NORMAL, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CONTACT_ERROR, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CONTACT_FORCE, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_ACTOR_AXES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_AABBS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_SHAPES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_AXES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_COMPOUNDS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_VNORMALS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_FNORMALS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_EDGES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_SPHERES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_STATIC, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_DYNAMIC, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_FREE, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_CCD, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_COLLISION_SKELETONS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_EMITTERS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_POSITION, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_VELOCITY, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_KERNEL_RADIUS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_BOUNDS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_PACKETS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_MOTION_LIMIT, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_DYN_COLLISION, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_STC_COLLISION, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_MESH_PACKETS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_DRAINS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_FLUID_PACKET_DATA, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_MESH, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_COLLISIONS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_SELFCOLLISIONS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_WORKPACKETS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_SLEEP, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_SLEEP_VERTEX, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_TEARABLE_VERTICES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_TEARING, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_ATTACHMENT, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_CLOTH_VALIDBOUNDS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_MESH, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_COLLISIONS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_WORKPACKETS, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_SLEEP, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_SLEEP_VERTEX, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_TEARABLE_VERTICES, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_TEARING, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_ATTACHMENT, WxAbilityViewer::OnPhysXDebug )
	EVT_MENU(IDM_ANIMSET_PHYSX_DEBUG_SOFTBODY_VALIDBOUNDS, WxAbilityViewer::OnPhysXDebug )
#if WITH_APEX
	EVT_MENU(IDM_ANIMSET_APEX_CLEAR_ALL, WxAbilityViewer::OnApexDebug )
#if 0 // oscar@snda
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_SKINNED_POSITIONS, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_SPHERE_COLLISION, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_MAX_DISTANCE_OUT, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_MAX_DISTANCE_IN, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_ALL_SKIN_MAP, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_BAD_SKIN_MAP, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_SKIN_MAP, WxAbilityViewer::OnApexDebug )
// 	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_RENDER_NORMALS, WxAbilityViewer::OnApexDebug )
// 	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_RENDER_TANGENTS, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_PHYSICS_MESH_WIRE, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_PHYSICS_MESH_SOLID, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_SKELETON, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_BONE_FRAMES, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_BONE_NAMES, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_VELOCITIES, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_GRAPHICAL_VERTEX_BONES, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_PHYSICAL_VERTEX_BONES, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_CLOTHING_ACOLLISION_UMBRELLAS, WxAbilityViewer::OnApexDebug )

	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_DESTRUCTIBLE_BOUNDS, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_DESTRUCTIBLE_SUPPORT, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_DESTRUCTIBLE_ACTOR_POSE, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_DESTRUCTIBLE_FRAGMENT_ACTOR_POSE, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_DESTRUCTIBLE_ACTOR_NAME, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_LOD_DISTANCE_DESTRUCTIBLE_ACTOR_POSE, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_LOD_DISTANCE_DESTRUCTIBLE_ACTOR_NAME, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_LOD_DISTANCE_DESTRUCTIBLE_FRAGMENT_ACTOR_POSE, WxAbilityViewer::OnApexDebug )

	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_LOD_BENEFITS, WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_LOD_DISTANCE_SCALE, WxAbilityViewer::OnApexDebug )
#if WITH_APEX_PARTICLES
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_TOTAL_INJECTED_AABB,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_APEX_EMITTER_ACTOR_POSE,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_THRESHOLD_DISTANCE_APEX_EMITTER_ACTOR_POSE,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_APEX_EMITTER_ACTOR_NAME,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_THRESHOLD_DISTANCE_APEX_EMITTER_ACTOR_NAME,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_GROUND_EMITTER_SPHERE,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_GROUND_EMITTER_GRID,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_GROUND_EMITTER_RAYCAST,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_GROUND_EMITTER_ACTOR_POSE,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_GROUND_EMITTER_ACTOR_NAME,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_IMPACT_EMITTER_RAYCAST,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_IMPACT_EMITTER_ACTOR_NAME,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_THRESHOLD_DISTANCE_IMPACT_EMITTER_ACTOR_NAME,WxAbilityViewer::OnApexDebug )

	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_IOFX_BOUNDING_BOX,WxAbilityViewer::OnApexDebug )
	EVT_MENU(IDM_ANIMSET_APEX_VISUALIZE_IOFX_ACTOR_NAME,WxAbilityViewer::OnApexDebug )
#endif
#endif

#endif

END_EVENT_TABLE()


static INT AnimSetViewer_ControlBorder = 4;

/*-----------------------------------------------------------------------------
WxMorphTargetPane
-----------------------------------------------------------------------------*/
// WxMorphTargetPane::WxMorphTargetPane(wxWindow* InParent)
// 		:	wxScrolledWindow( InParent, -1, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL )
// {
//  	SetScrollbars(1,1,0,0);
//  	// Set the x and y scrolling increments.  This is necessary for the scrolling to work!
//  	SetScrollRate( 1, 1 );
// 
//  	MorphTargetSizer = new wxFlexGridSizer(5);
//  	// second column is growable
// 	MorphTargetSizer->AddGrowableCol(1);
//  	MorphTargetSizer->AddGrowableCol(3);
//  	MorphTargetSizer->SetFlexibleDirection(wxHORIZONTAL);
// 	MorphTargetSizer->SetHGap(5);
// 	MorphTargetSizer->SetVGap(10);
// 	SetSizer(MorphTargetSizer);
// }
// 
// /**
// * Layout all children components for input list
// */
// void WxMorphTargetPane::LayoutWindows( const TArray<UMorphTarget*>& MorphTargetList )
// {
// 	// clean up
// 	MorphTargetSizer->Clear(true);
// 
// 	MorphTargetTextNames.Empty();
// 	MorphTargetWeightSliders.Empty();
// 	MorphTargetSelectChecks.Empty();
// 
// 	// verify if less than 100 - this is due to resourceIDs. 
// 	// You'd need to reserver more resource ID if you'd like more than 100. 
// 	check (MorphTargetList.Num() < 100);
// 
// 	MorphTargetTextNames.Add(MorphTargetList.Num());
// 	MorphTargetWeightSliders.Add(MorphTargetList.Num());
// 	MorphTargetSelectChecks.Add(MorphTargetList.Num());
// 
// 	Freeze();
// 
// 	const wxString SelectText(TEXT("Select"));
// 	const wxString Min(TEXT("0")), Max(TEXT("1"));
// 
// 	// go through items, add to the panel
// 	for(INT I=0; I<MorphTargetList.Num(); I++)
// 	{
// 		UMorphTarget* Target = MorphTargetList(I);
// 		FString TargetString = FString::Printf( TEXT("%s"), *Target->GetName() );
// 
// 		// Add the item 
// 		// Add slider 0 <slider> 1
// 		wxStaticText * MinText = new wxStaticText( this, -1, Min);
// 		MorphTargetSizer->Add( MinText, 0, wxALIGN_CENTER|wxALL);
// 		
// 		MorphTargetWeightSliders(I) = new wxSlider( this, IDM_ANIMSET_MORPHTARGETWEIGHTS_START+I, 0, 0, 100, wxDefaultPosition, wxSize(150, 20));
// 		MorphTargetSizer->Add( MorphTargetWeightSliders(I), 1, wxALIGN_CENTER|wxALL|wxLEFT|wxRIGHT|wxEXPAND);
// 
// 		wxStaticText * MaxText = new wxStaticText( this, -1, Max);
// 		MorphTargetSizer->Add( MaxText, 0, wxALIGN_CENTER|wxALL, AnimSetViewer_ControlBorder);
// 		
// 		// Add text box for name
// 		MorphTargetTextNames(I) = new wxTextCtrl( this, IDM_ANIMSET_MORPHTARGETS_START+I, wxString(*TargetString), wxDefaultPosition, wxSize(100, 15));
// 		MorphTargetSizer->Add( MorphTargetTextNames(I), 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, AnimSetViewer_ControlBorder);
// 
// 		// selection check box - used for deletion/property window
// 		MorphTargetSelectChecks(I) = new wxCheckBox( this, IDM_ANIMSET_MORPHTARGETSELECT_START+I, SelectText, wxDefaultPosition);
// 		MorphTargetSizer->Add( MorphTargetSelectChecks(I), 0, wxALIGN_CENTER|wxALL, AnimSetViewer_ControlBorder);
// 	}
// 
// 	// Tell the scrolling window how large the virtual area is.
// 	FitInside();
// 	SetVirtualSize( MorphTargetSizer->GetSize() );
// 	
// 	Thaw();
// 	Layout();
// }
// 
// /*
// * Return TRUE, if the index item is selected
// */
// UBOOL WxMorphTargetPane::IsSelected( INT Index )
// {
// 	check (MorphTargetSelectChecks.Num() > Index);
// 
// 	return MorphTargetSelectChecks(Index)->GetValue();
// }
// 
// /*
// * Reset all weights on slider list
// */
// void WxMorphTargetPane::ResetAllWeights()
// {
// 	for ( INT I=0; I<MorphTargetWeightSliders.Num(); ++I )
// 	{
// 		MorphTargetWeightSliders(I)->SetValue(0);
// 	}
// }
// 
// /*
// * Select all if bSelect is true. Deselect otherwise. 
// */
// void WxMorphTargetPane::SelectAll( UBOOL bSelect )
// {
// 	for ( INT I=0; I<MorphTargetSelectChecks.Num(); ++I )
// 	{
// 		MorphTargetSelectChecks(I)->SetValue(bSelect == TRUE);
// 	}
// }

/*-----------------------------------------------------------------------------
WxAbilityViewer
-----------------------------------------------------------------------------*/
WxAbilityViewer::WxAbilityViewer(wxWindow* InParent, wxWindowID InID, UObject* EditObj, UGenericBrowserType_Ability* InSpawner)
	:	WxTrackableFrame( InParent, InID, *LocalizeUnrealEd("AbilityEditor"), wxDefaultPosition, wxSize(1024, 700), wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxMAXIMIZE )
	,	FDockingParent( this )
	,	PlaybackSpeed( 1.0f )
	//,	DlgAnimationCompression( NULL )
	,	bPreviewInstanceWeights(FALSE)
	,   bDrawAllBoneInfluenceVertices(TRUE)
	,	bTransactionInProgress(FALSE)
	,	SelectedSortStripIndex(INDEX_NONE)
	,	SelectedSortStripLodIndex(INDEX_NONE)
	,	SelectedSortStripSectionIndex(INDEX_NONE)
	,	bSortStripMoveForward(FALSE)
	,	bSortStripMoveBackward(FALSE)
	,	bResampleAnimNotifyData(FALSE)
	,	bSearchAllAnimSequences(FALSE)
	,	bPromptUserToLoadAllAnimSets(TRUE)
{
	//wxEvtHandler::Connect(wxID_ANY, IDM_ANIMSET_FILLSKELETONTREE, wxCommandEventHandler(WxAbilityViewer::OnFillSkeletonTree));

	PreviewSkelComp = NULL;
	PreviewMergedComp = NULL;

	PreviewSkelCompTarget = NULL;
	PreviewTargetAnimTree = NULL;
	PreviewTargetAnimNode = NULL;

	CurrentCopyedComponent = NULL;
	CurrentSelectedComponent = NULL;



	MaxShowTrackLength = 3.0;
	MaxTrackLength = 3.0;
	CurrentPlayTime = 0.f;

	CurrentCycleCount = -1;

	TargetLocalToWorld = FMatrix::Identity;


	bPlaying = FALSE;
	bLooping = FALSE;

	bMeshChange = TRUE;

	//SocketMgr = NULL;

	AbilityObject = Cast<UMMOAbility>(EditObj);
	check(AbilityObject);

	INT IDX = IDM_EFFECT_NODE_ADD_START;
	ClassMap.Empty();
	for( TObjectIterator<UClass> It ; It ; ++It )
	{
		if (It->IsChildOf(UAbilityComponent::StaticClass()) && *It != UAbilityComponent::StaticClass())
		{
			ClassMap.Set(IDX, *It);
			IDX++;
		}
	}


	CurrentWeaponType = AbilityObject->DefaultWeaponType;

	PreviewWeaponList.Empty();

	GenericBrowserPointer = InSpawner;

	UVSetToDisplay = -1;

	GCallbackEvent->Register(CALLBACK_FileChanged, this);

	// Reset this to zero.
	WireframeCycleCounter = 0;
	bShowAdditiveBase = FALSE;
	bListenForFileChanges = FALSE;

	bShowClothMovementScale = FALSE;

	bEditSpellState = FALSE;

	CameraPositionType = 0;

	CurrentCycleCount = 0;

	DisplayFlag = 0xffffffff;

	// clear memory
	appMemzero(&VertexInfo, sizeof(VertexInfo));

	/////////////////////////////////
	// Create all the GUI stuff.

	PlayB.Load( TEXT("ASV_Play") );
	StopB.Load( TEXT("ASV_Stop") );
	LoopB.Load( TEXT("ASV_Loop") );
	NoLoopB.Load( TEXT("ASV_NoLoop") );
	AddComponentB.Load(TEXT("AED_AddActor"));
	AbilityTimeAdd.Load(TEXT("AbilityAddTime"));
	AbilityTimeSub.Load(TEXT("AbilitySubTime"));
	SpellImage.Load(TEXT("Spell"));
	CastImage.Load(TEXT("Cast"));
	


	UseButtonB.Load( TEXT("Prop_Use") );
	SearchAllB.Load( TEXT("MaterialInstanceEditor_ShowAllParams") );
	ClearB.Load( "Cancel.png" );
	ClearDisabledB.Load( "Cancel_Disabled.png" );


	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	BaseWindow = new wxSplitterWindow(this, -1);
	BaseWindow->SetMinimumPaneSize(100);

	//wxPanel* BasePanel = new wxPanel(this, -1);
	//wxBoxSizer* BaseSizer = new wxBoxSizer(wxHORIZONTAL);
	//BasePanel->SetSizer(BaseSizer);

	LeftArea = new wxSplitterWindow(BaseWindow, -1);
	LeftArea->SetMinimumPaneSize(100);
	RightArea = new wxSplitterWindow(BaseWindow, -1);
	RightArea->SetMinimumPaneSize(100);

	BaseWindow->SplitVertically(LeftArea, RightArea, GetSize().GetWidth()*0.66);
	BaseWindow->SetSashGravity(0.6);
	BaseWindow->UpdateSize();

	wxPanel* RealtimePanel = new wxPanel(LeftArea);
	RealtimePanel->SetBackgroundColour(wxColour(200, 200, 200));

	wxPanel* ShowResPanel = new wxPanel(LeftArea);
	ShowResPanel->SetBackgroundColour(wxColour(200, 200, 200));

	wxPanel* AbilityPanel = new wxPanel(RightArea);
	AbilityPanel->SetBackgroundColour(wxColour(200, 200, 200));

	wxPanel* EditDataPanel = new wxPanel(RightArea);
	EditDataPanel->SetBackgroundColour(wxColour(200, 200, 200));


	LeftArea->SplitHorizontally(RealtimePanel, ShowResPanel);
	LeftArea->SetSashGravity(0.8);
	//LeftArea->SetSashPosition(GetSize().GetHeight()*0.8, true);

	RightArea->SplitHorizontally(AbilityPanel, EditDataPanel);
	RightArea->SetSashGravity(0.3);
	//RightArea->SetSashPosition(GetSize().GetHeight()*0.7, true);


	// realtime view
	wxBoxSizer* MidSizerV = new wxBoxSizer( wxVERTICAL );
	RealtimePanel->SetSizer(MidSizerV);

	// 3D Preview
	PreviewWindow = new WxAbilityPreview( RealtimePanel, -1, this );
	MidSizerV->Add( PreviewWindow, 1, wxGROW|wxALL, 4 );
	
	// Sizer for controls at bottom
	wxBoxSizer* ControlSizerH = new wxBoxSizer( wxHORIZONTAL );
	MidSizerV->Add( ControlSizerH, 0, wxGROW|wxALL, 4 );
	
	// Stop/Play/Loop buttons
	LoopButton = new wxBitmapButton( RealtimePanel, IDM_ANIMSET_LOOPBUTTON, NoLoopB);
	ControlSizerH->Add( LoopButton, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	PlayButton = new wxBitmapButton( RealtimePanel, IDM_ANIMSET_PLAYBUTTON, PlayB);
	ControlSizerH->Add( PlayButton, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	// Time scrubber
	TimeSlider = new wxSlider( RealtimePanel, IDM_ANIMSET_TIMESCRUB, 0, 0, ASV_SCRUBRANGE );
	ControlSizerH->Add( TimeSlider, 1,  wxALIGN_CENTER, 5 );
	//ControlSizerH->Add( TimeSlider, 1, wxGROW|wxALL, 0 );

	ControlSizerH->Add(new wxBitmapButton(RealtimePanel, IDB_ADD_ABILITY_TIME, AbilityTimeAdd), 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	ControlSizerH->Add(new wxBitmapButton(RealtimePanel, IDB_SUB_ABILITY_TIME, AbilityTimeSub), 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);

	SwithEditStateButton = new wxBitmapButton(RealtimePanel, IDB_SWITCH_EDIT_STATE, CastImage);
	ControlSizerH->Add(SwithEditStateButton, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	
	BaseMenu = new wxMenu;

	AddMenu = new wxMenu;

	for (TMap<INT, UClass*>::TIterator i(ClassMap); i; ++i)
	{
		AddMenu->Append(i.Key(), *(i.Value()->GetName()));
	}
	BaseMenu->AppendSubMenu(AddMenu, wxT("AddComponent"));

	wxMenu* WeaponTypeMenu = new wxMenu;
	WeaponTypeMenu->Append(IDM_SELECT_WEAPON_TYPE, wxT("SwitchWeaponType"));
	WeaponTypeMenu->Append(IDM_ADD_WEAPON_TYPE, wxT("AddNewWeaponType"));
	WeaponTypeMenu->Append(IDM_DELETE_WEAPON_TYPE, wxT("DeleteWeaponType"));
	WeaponTypeMenu->Append(IDM_COPY_DEFAULT_LIST_TO_CURRENT_LIST, wxT("CopyDefaultComponentsToCurrent"));
	WeaponTypeMenu->Append(IMD_COPY_DEFAULT_SOUND_TO_CURRENT_LIST, wxT("CopyDefaultSoundComponentsToCurrent"));
	BaseMenu->AppendSubMenu(WeaponTypeMenu, wxT("WeaponType"));

	wxMenu* CopyMenu = new wxMenu;
	CopyMenu->Append(IDM_COPY_CURRENT_COMPONENT, wxT("Copy"));
	CopyMenu->Append(IDM_PASTY_CURRENT_COMPONENT, wxT("Paste"));
	BaseMenu->AppendSubMenu(CopyMenu, wxT("CopyComponent"));


	DisplayMenu = new wxMenu;
	for (TMap<INT, UClass*>::TIterator i(ClassMap); i; ++i)
	{
		DisplayMenu->AppendCheckItem(i.Key() + IDM_EFFECT_NODE_SHOW_START - IDM_EFFECT_NODE_ADD_START, *(i.Value()->GetName()));
	}
	DisplayMenu->AppendCheckItem(IDM_SHOW_CALC_SIMULATE, wxT("CalculateLine"));

	BaseMenu->AppendSubMenu(DisplayMenu, wxT("DisplaySetting"));




	// ability property window
	wxBoxSizer* AbilityBoxSizer = new wxBoxSizer(wxVERTICAL);
	AbilityPanel->SetSizer(AbilityBoxSizer);
	AbilityProperty = new WxPropertyWindowHost;
	AbilityProperty->Create(AbilityPanel, this);
	AbilityBoxSizer->Add(AbilityProperty, 1, wxEXPAND|wxALL, 4);
	AbilityProperty->SetObject(AbilityObject, EPropertyWindowFlags::ShouldShowCategories);

	wxBoxSizer* ComponentBoxSizer = new wxBoxSizer(wxVERTICAL);
	EditDataPanel->SetSizer(ComponentBoxSizer);
	ComponentProperty = new WxPropertyWindowHost;
	ComponentProperty->Create(EditDataPanel, this);
	ComponentBoxSizer->Add(ComponentProperty, 1, wxEXPAND|wxALL, 4);
	ComponentProperty->SetObject(NULL, EPropertyWindowFlags::ShouldShowCategories);



	// resources property window
	wxBoxSizer* ResBoxSizer = new wxBoxSizer(wxVERTICAL);
	ShowResPanel->SetSizer(ResBoxSizer);
	PreviewResProperty = new WxPropertyWindowHost;
	PreviewResProperty->Create(ShowResPanel, this);
	ResBoxSizer->Add(PreviewResProperty, 1, wxEXPAND|wxALL, 4);

	UMMOAbility* AbilityData = AbilityObject;
	check(AbilityData);
	if (AbilityData->PreviewResource == NULL)
	{
		AbilityData->PreviewResource = 	ConstructObject<UAbilityPreviewResources>(UAbilityPreviewResources::StaticClass(), AbilityObject);
		AbilityData->MarkPackageDirty();
	}

	PreviewResObject = NULL;


	// Create floor component
	UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(NULL, TEXT("EditorMeshes.PhAT_FloorBox"), NULL, LOAD_None, NULL);
	check(FloorMesh);

	EditorFloorComp = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass());
	EditorFloorComp->StaticMesh = FloorMesh;
	EditorFloorComp->BlockRigidBody = TRUE;
	EditorFloorComp->Scale = 4.f;
	GetPreviewScene()->AddComponent(EditorFloorComp,FMatrix::Identity);



	RefreshData();

	//// Update the FOV so it shows the last used value
	//if ( GApp && GApp->EditorFrame && GApp->EditorFrame->ViewportConfigData )
	//{
	//	SetFOV( GApp->EditorFrame->ViewportConfigData->GetCustomFOV() );
	//}

	// Set camera location
	// TODO: Set the automatically based on bounds? Update when changing mesh? Store per-mesh?
	FEditorAbilityViewportClient* vc = GetPreviewVC();
	check( vc );
	
	//Bruce@snda
	//vc->ViewLocation = FVector(512,0,100);
	//vc->ViewRotation = FRotator(0,32768,0);

	vc->ViewLocation = FVector(143.42043, 587.48114, 426.85638);
	vc->ViewRotation = FRotator(-4661, 49113, 0);
	//Bruce@snda

	// Init wind dir.
	WindStrength = 10.f;
	WindRot = FRotator(0,0,0);
#if WITH_APEX
	WindVelocityBlendTime = 0.1f;
#endif
	UpdateClothWind();
	
	
	// Use default WorldInfo to define the gravity and stepping params.
	AWorldInfo* Info = (AWorldInfo*)(AWorldInfo::StaticClass()->GetDefaultObject());
	check(Info);
	FVector Gravity(0, 0, Info->DefaultGravityZ);
	RBPhysScene = CreateRBPhysScene( Gravity );
	
#if WITH_APEX
	if ( (RBPhysScene != NULL) && (RBPhysScene->ApexScene != NULL) && (GApexManager != NULL) )
	{
		// Set infinite budget here so all clothing will be visible in AnimSetViewer
		GApexManager->SetApexLODResourceBudget( RBPhysScene->ApexScene, FLT_MAX );
	}
#endif




//	SetSize(1200,800);
	// Load Window Options.
	LoadSettings();
//
//
//	// Create menu bar
//	MenuBar = new WxAEVMenuBar();
//	AppendWindowMenu(MenuBar);
//	SetMenuBar( MenuBar );
//	// disable alt. bone weighting tool first. 
//	MenuBar->EnableAltBoneWeightingMenu(FALSE,PreviewSkelComp);
//
	// Create tool bar
	ToolBar = new WxAEVToolBar( this, -1 );
	SetToolBar( ToolBar );

//	// Update the FOV so it shows the last used value
//	if ( GApp && GApp->EditorFrame && GApp->EditorFrame->ViewportConfigData )
//	{
//		SetFOV( GApp->EditorFrame->ViewportConfigData->GetCustomFOV() );
//	}
//
//	// Create status bar
//	StatusBar = new WxAEVStatusBar( this, -1 );
//	SetStatusBar( StatusBar );
//
//	// Set camera location
//	// TODO: Set the automatically based on bounds? Update when changing mesh? Store per-mesh?
//	FEditorAbilityViewportClient* ASVPreviewVC = GetPreviewVC();
//	check( ASVPreviewVC );
//
//	ASVPreviewVC->ViewLocation = FVector(0,-256,0);
//	ASVPreviewVC->ViewRotation = FRotator(0,16384,0);	
//
//	// Init wind dir.
//	WindStrength = 10.f;
//	WindRot = FRotator(0,0,0);
//#if WITH_APEX
//	WindVelocityBlendTime = 0.1f;
//#endif
//	UpdateClothWind();
//
//
//	// Use default WorldInfo to define the gravity and stepping params.
//	AWorldInfo* Info = (AWorldInfo*)(AWorldInfo::StaticClass()->GetDefaultObject());
//	check(Info);
//	FVector Gravity(0, 0, Info->DefaultGravityZ);
//	RBPhysScene = CreateRBPhysScene( Gravity );
//
//#if WITH_APEX
//	if ( (RBPhysScene != NULL) && (RBPhysScene->ApexScene != NULL) && (GApexManager != NULL) )
//	{
//		// Set infinite budget here so all clothing will be visible in AnimSetViewer
//		GApexManager->SetApexLODResourceBudget( RBPhysScene->ApexScene, FLT_MAX );
//	}
//#endif
//
//
//	// Create floor component
//	UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(NULL, TEXT("EditorMeshes.PhAT_FloorBox"), NULL, LOAD_None, NULL);
//	check(FloorMesh);
//
//	EditorFloorComp = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass());
//	EditorFloorComp->StaticMesh = FloorMesh;
//	EditorFloorComp->BlockRigidBody = TRUE;
//	EditorFloorComp->Scale = 4.f;
//	ASVPreviewVC->PreviewScene.AddComponent(EditorFloorComp,FMatrix::Identity);
//
//	EditorFloorComp->BodyInstance = ConstructObject<URB_BodyInstance>( URB_BodyInstance::StaticClass() );
//	EditorFloorComp->BodyInstance->InitBody(EditorFloorComp->StaticMesh->BodySetup, FMatrix::Identity, FVector(4.f), true, EditorFloorComp, RBPhysScene);
//
//	//hide or show floor (and update collision)
//	UpdateFloorComponent();
	//Bruce@snda
	UpdateSwitchWeaponTypePreview();
	UpdateWeaponTypePreview();
	//Bruce@snda
}


WxAbilityViewer::~WxAbilityViewer()
{
	GCallbackEvent->UnregisterAll(this);
	////make sure to only turn off file extensions if we previously as to listen to them
	if (bListenForFileChanges)
	{
		SetFileSystemNotifications(FALSE);
	}

	GenericBrowserPointer->AbilityEditorPointer = NULL;

	DeletePoolComponents();

	SaveSettings();
	FlushRenderingCommands();

	if ( PreviewSkelComp )
	{
		PreviewSkelComp->ReleaseApexClothing();
		PreviewSkelComp->AnimSets.Empty();
	}

	PreviewWindow->DestroyViewport();


	DestroyRBPhysScene(RBPhysScene);
}

UAnimNodeSequence* WxAbilityViewer::GetMeshAnimNodeSequence(USkeletalMeshComponent* InSM)
{
	if (!InSM)
	{
		return NULL;
	}
	UAnimTree* AnimTree = Cast<UAnimTree>(InSM->Animations);
	if (AnimTree == NULL)
	{
		return NULL;
	}

	return Cast<UAnimNodeSequence>(AnimTree->Children(0).Anim);
}

void WxAbilityViewer::AttachSkeletalMesh( USkeletalMeshComponent* InParent, USkeletalMesh* ChildMesh )
{
	if( InParent == NULL || ChildMesh == NULL )
		return;

	USkeletalMeshComponent* smc = Cast<USkeletalMeshComponent>(
		UObject::StaticConstructObject(
		USkeletalMeshComponent::StaticClass(), UObject::GetTransientPackage()));

	smc->SetSkeletalMesh( ChildMesh );
	smc->SetParentAnimComponent( InParent );

	GetPreviewScene()->AddComponent( smc, InParent->LocalToWorld);
	OtherSkeletalMeshComponentList.AddItem( smc );
}

void WxAbilityViewer::ParseEquipMesh(USkeletalMeshComponent* InSM, UEquipMesh* InEquipMesh, UBOOL bWeapon)
{
	if (!InSM || !InEquipMesh)
	{
		return;
	}

	UActorComponent* NewC = NULL;

	if (InEquipMesh->ArmorTemplate)
	{

	}
	else
	{
		USkeletalMeshComponent* smc = Cast<USkeletalMeshComponent>(UObject::StaticConstructObject(USkeletalMeshComponent::StaticClass(), UObject::GetTransientPackage()));
		check(smc);
		smc->SetSkeletalMesh(InEquipMesh->SkeletalMesh);

		smc->SetParentAnimComponent(InSM);

		GetPreviewScene()->AddComponent(smc, InSM->LocalToWorld);

		NewC = smc;
	}

	if (NewC && NewC->IsA(USkeletalMeshComponent::StaticClass()))
	{
		if (InSM == PreviewSkelComp)
		{
			OtherSkeletalMeshComponentList.AddItem((USkeletalMeshComponent*)NewC);
		}
		else
		{
			TargetOtherSkeletalMeshComponentList.AddItem((USkeletalMeshComponent*)NewC);
		}
	}

	//if (InEquipMesh->ResourceType == ECRT_SkeletalMesh)
	//{
	//	USkeletalMeshComponent* smc = Cast<USkeletalMeshComponent>(UObject::StaticConstructObject(USkeletalMeshComponent::StaticClass(), UObject::GetTransientPackage()));
	//	check(smc);
	//	smc->SetSkeletalMesh(InEquipMesh->SkeletalMesh);

	//	smc->SetParentAnimComponent(InSM);

	//	GetPreviewScene()->AddComponent(smc, InSM->LocalToWorld);
	//	if (NewC->IsA(USkeletalMeshComponent::StaticClass()))
	//	{
	//		if (InSM == PreviewSkelComp)
	//		{
	//			OtherSkeletalMeshComponentList.AddItem((USkeletalMeshComponent*)NewC);
	//		}
	//		else
	//		{
	//			TargetOtherSkeletalMeshComponentList.AddItem((USkeletalMeshComponent*)NewC);
	//		}

	//	}

	//	if (InEquipMesh->AnimationType == EUAT_BaseAnimation)
	//	{
	//		smc->SetParentAnimComponent(InSM);
	//	}
	//	else if (InEquipMesh->AnimationType == EUAT_SelfAnimation)
	//	{
	//		UAnimTree* NewTree = ConstructObject<UAnimTree>(UAnimTree::StaticClass());
	//		check(NewTree);
	//		NewTree->Children(0).Anim = ConstructObject<UAnimNodeSequence>(UAnimNodeSequence::StaticClass());
	//		smc->Animations = NewTree;
	//		smc->InitAnimTree(TRUE);
	//		smc->AnimSets.AddItem(InEquipMesh->AnimSet);
	//		if (bWeapon)
	//		{
	//			PreviewWeaponList.AddItem(smc);
	//		}
	//	}

	//	NewC = smc;
	//}
	//else if (InEquipMesh->ResourceType == ECRT_StaticMesh)
	//{
	//	UStaticMeshComponent* stmc = Cast<UStaticMeshComponent>(UObject::StaticConstructObject(UStaticMeshComponent::StaticClass(), UObject::GetTransientPackage())); 
	//	if (stmc)
	//	{
	//		stmc->SetStaticMesh(InEquipMesh->StaticMesh);
	//		NewC = stmc;

	//		if (bWeapon)
	//		{
	//			PreviewWeaponList.AddItem(stmc);
	//		}
	//	}
	//}
	//else if (InEquipMesh->ResourceType == ECRT_Particle)
	//{
	//	UParticleSystemComponent* psc = Cast<UParticleSystemComponent>(UObject::StaticConstructObject(UParticleSystemComponent::StaticClass(), UObject::GetTransientPackage())); 
	//	check(psc);
	//	psc->SetTemplate(InEquipMesh->ParticleSystem);
	//	psc->ActivateSystem();
	//	NewC = psc;
	//}			  

	//if (InEquipMesh->ComponentAddingType == ECAT_AddToBaseActor)
	//{
	//	GetPreviewScene()->AddComponent(NewC, InSM->LocalToWorld);
	//	if (NewC->IsA(USkeletalMeshComponent::StaticClass()))
	//	{
	//		if (InSM == PreviewSkelComp)
	//		{
	//			OtherSkeletalMeshComponentList.AddItem((USkeletalMeshComponent*)NewC);
	//		}
	//		else
	//		{
	//			TargetOtherSkeletalMeshComponentList.AddItem((USkeletalMeshComponent*)NewC);
	//		}
	//		
	//	}
	//}
	//else if (InEquipMesh->ComponentAddingType == ECAT_AttachToBaseSocket)
	//{
	//	InSM->AttachComponentToSocket(NewC, InEquipMesh->SocketName);
	//}
}

BYTE WxAbilityViewer::GetWeaponType(UAbilityPreviewResources* InRes)
{
	return CurrentWeaponType;
}

void WxAbilityViewer::ResetPreviewMeshLocation()
{
	if (PreviewSkelComp)
	{
		PreviewSkelComp->ConditionalUpdateTransform(FMatrix::Identity);
	}

	if (PreviewSkelCompTarget)
	{
		FRotator NewRotation(0, 32768, 0);
		FMatrix TargetMat = FRotationMatrix(NewRotation) * FTranslationMatrix(AbilityObject->PreviewResource->PositionOffset);
		if (AbilityObject->PreviewResource->Target)
		{
			PreviewSkelCompTarget->ConditionalUpdateTransform(TargetMat);
		}
	}
}


void WxAbilityViewer::RefreshData()
{
	UMMOAbility* data = AbilityObject;
	if (data)
	{
		// clear
		PreviewWeaponList.Empty();

		for (INT i = 0; i < OtherSkeletalMeshComponentList.Num(); ++i)
		{
			if (OtherSkeletalMeshComponentList(i) != PreviewMergedComp)
			{
				GetPreviewScene()->RemoveComponent(OtherSkeletalMeshComponentList(i));
				OtherSkeletalMeshComponentList(i)->MarkPendingKill();
			}
		}

		OtherSkeletalMeshComponentList.Empty();
		if (PreviewMergedComp)
		{
			OtherSkeletalMeshComponentList.AddItem(PreviewMergedComp);
		}

		for (INT i = 0; i < TargetOtherSkeletalMeshComponentList.Num(); ++i)
		{
			GetPreviewScene()->RemoveComponent(TargetOtherSkeletalMeshComponentList(i));
			TargetOtherSkeletalMeshComponentList(i)->MarkPendingKill();
		}

		TargetOtherSkeletalMeshComponentList.Empty();

		while(PreviewSkelComp->Attachments.Num() > 0)
		{
			PreviewSkelComp->DetachComponent(PreviewSkelComp->Attachments(PreviewSkelComp->Attachments.Num() - 1).Component);
		}
		PreviewSkelComp->AnimSets.Empty();

		GamePlayData = Cast<UGamePlayDataManager>(LoadObject<UGamePlayDataManager>(NULL, TEXT("gameplaydata.gameplaydata"), NULL, LOAD_None, NULL));
		check(GamePlayData);

		PreviewResObject = Cast<UAbilityPreviewResources>(data->PreviewResource);

		// floor
		if (PreviewResObject->PreviewFloor)
		{
			EditorFloorComp->SetStaticMesh(PreviewResObject->PreviewFloor);
			EditorFloorComp->ConditionalUpdateTransform(FMatrix::Identity);
		}

		//FTranslationMatrix ftm(PreviewResObject->FloorPosition);
		//if (EditorFloorComp)
		//{
		//	EditorFloorComp->ConditionalUpdateTransform(ftm);
		//}

		// fill data
		AbilityProperty->SetObject(data, EPropertyWindowFlags::ShouldShowCategories);
		PreviewResProperty->SetObject(PreviewResObject, EPropertyWindowFlags::ShouldShowCategories);

		UAbilityPreviewResources* apr = PreviewResObject;
		check(apr);

		// set window name
		FString ObjName = AbilityObject->GetFullName();
		FString TypeName = GetWeaponTypeName(CurrentWeaponType);
		FString TitleStr = ObjName + FString(TEXT("  Edit Type: ")) + TypeName;
		SetTitle(wxString(*TitleStr));


		// get weapon type first
		TArray<UEquipMesh*> WeaponList;
		UBOOL bFound = FALSE;
		for (INT i = 0; i < apr->WeaponList.Num(); ++i)
		{
			if (apr->WeaponList(i)
				&& apr->WeaponList(i)->GetEquipAppearenceTemplate()
				&& (CurrentWeaponType == apr->WeaponList(i)->GetEquipAppearenceTemplate()->WeaponType))
			{
				bFound = TRUE;
				if (apr->bMale)
				{
					WeaponList = apr->WeaponList(i)->GetEquipAppearenceTemplate()->MeshListTemplate_M;
				}
				else
				{
					WeaponList = apr->WeaponList(i)->GetEquipAppearenceTemplate()->MeshListTemplate_F;
				}
			}
		}


		// fill attacker data
		if (apr->Attacker)
		{
			FLOAT AttackerDrawScale = apr->Attacker->AvatarDrawScale * apr->Attacker->MeshScale;
			PreviewSkelComp->Scale = AttackerDrawScale;
			PreviewSkelComp->SetSkeletalMesh(apr->Attacker->SkeletalMesh);
			PreviewSkelComp->SetPhysicsAsset(apr->Attacker->PhysicAsset);
			PreviewSkelComp->ConditionalUpdateTransform();

			for (INT i = 0; i < apr->Attacker->WeaponAnimSetPairs.Num(); ++i)
			{
				if (CurrentWeaponType == apr->Attacker->WeaponAnimSetPairs(i).WeaponType)
				{
					PreviewSkelComp->AnimSets = apr->Attacker->WeaponAnimSetPairs(i).AnimSets;
					break;
				}
			}


			if (apr->Attacker->NeedToBeMerged())
			{
				if (!PreviewMergedComp)
				{
					PreviewMergedComp = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass());
					//PreviewMergedComp->Scale = AttackerDrawScale;
					GetPreviewScene()->AddComponent(PreviewMergedComp, PreviewSkelComp->CachedParentToWorld);
					OtherSkeletalMeshComponentList.AddItem(PreviewMergedComp);
				}

				AbilityObject->PreviewResource->RebulidMesh(PreviewSkelComp, PreviewMergedComp);

			}
			else
			{
				PreviewMergedComp = PreviewSkelComp;
			}

			//if( apr->Attacker->UseMergedMesh )
			//{
			//	// hair
			//	if( apr->Attacker->HairList.Num() > 0 )
			//	{
			//		AttachSkeletalMesh( PreviewSkelComp, apr->Attacker->HairList(0) );
			//	}
			//	// face
			//	if( apr->Attacker->FaceList.Num() > 0 )
			//	{
			//		AttachSkeletalMesh( PreviewSkelComp, apr->Attacker->FaceList(0) );
			//	}
			//	// body
			//	AttachSkeletalMesh( PreviewSkelComp, apr->Attacker->DefaultBodyMesh );
			//	
			//	// other meshes
			//	for (INT i = 0; i < apr->Attacker->DefaultMeshListTemplate.Num(); ++i)
			//	{
			//		if (apr->Attacker->DefaultMeshListTemplate(i))
			//		{
			//			ParseEquipMesh(PreviewSkelComp, apr->Attacker->DefaultMeshListTemplate(i));
			//		}
			//	}
			//}
			//else
			{
				for (INT i = 0; i < apr->Attacker->DefaultMeshListTemplate.Num(); ++i)
				{
					if (apr->Attacker->DefaultMeshListTemplate(i))
					{
						ParseEquipMesh(PreviewSkelComp, apr->Attacker->DefaultMeshListTemplate(i));
					}
				}
			}

			if (WeaponList.Num() > 0)
			{
				for (INT i = 0; i < WeaponList.Num(); ++i)
				{
					if (WeaponList(i))
					{
						ParseEquipMesh(PreviewSkelComp, WeaponList(i), TRUE);
					}
				}
			}

			PreviewSkelComp->BeginDeferredReattach();
			PreviewSkelComp->InitAnimTree();
		}


		//FRotator NewRotation(0, 32768, 0);
		//TargetLocalToWorld = FRotationMatrix(NewRotation) * FTranslationMatrix(data->PreviewResource->PositionOffset);
		if (apr->Target)
		{
			//PreviewSkelCompTarget->ConditionalUpdateTransform(TargetLocalToWorld);

			PreviewSkelCompTarget->SetSkeletalMesh(apr->Target->SkeletalMesh);
			PreviewSkelCompTarget->SetPhysicsAsset(apr->Target->PhysicAsset);

			for (INT i = 0; i < apr->Target->WeaponAnimSetPairs.Num(); ++i)
			{
				if (apr->Target->WeaponAnimSetPairs(i).AnimSets.Num() != 0)
				{
					PreviewSkelCompTarget->AnimSets = apr->Target->WeaponAnimSetPairs(i).AnimSets;
					break;
				}
			}

			for (INT i = 0; i < apr->Target->DefaultMeshListTemplate.Num(); ++i)
			{
				ParseEquipMesh(PreviewSkelCompTarget, apr->Target->DefaultMeshListTemplate(i));
			}

			PreviewSkelCompTarget->BeginDeferredReattach();

			PreviewSkelCompTarget->ConditionalUpdateTransform(TargetLocalToWorld);

			PreviewSkelCompTarget->InitAnimTree(TRUE);
		}

		RefreshAnimInfoList();
	}
}

void WxAbilityViewer::ResetAbilityObject(UObject* InObject)
{
	UMMOAbility* data = Cast<UMMOAbility>(InObject);
	if (!data)
	{
		return;
	}
	AbilityObject = data;
	RefreshData();
}

void WxAbilityViewer::UpdateOtherComponents()
{
	if (PreviewMergedComp && PreviewSkelComp)
	{
		PreviewMergedComp->UpdateSkelPose();
		PreviewMergedComp->ConditionalUpdateTransform(PreviewSkelComp->LocalToWorld.GetMatrixWithoutScale());
	}


	for (INT i = 0; i < OtherSkeletalMeshComponentList.Num(); ++i)
	{
		if (OtherSkeletalMeshComponentList(i) && OtherSkeletalMeshComponentList(i) != PreviewMergedComp)
		{
			OtherSkeletalMeshComponentList(i)->ConditionalUpdateTransform(PreviewSkelComp->LocalToWorld);
			OtherSkeletalMeshComponentList(i)->UpdateSkelPose();
		}
	}

	for (INT i = 0; i < TargetOtherSkeletalMeshComponentList.Num(); ++i)
	{
		if (TargetOtherSkeletalMeshComponentList(i))
		{
			TargetOtherSkeletalMeshComponentList(i)->UpdateSkelPose();
			TargetOtherSkeletalMeshComponentList(i)->ConditionalUpdateTransform(PreviewSkelCompTarget->LocalToWorld);
		}
	}
}

void WxAbilityViewer::SetAnimPosition(FLOAT InPosition)
{
	UAnimNodeSequence * PreviewAnimNode = GetMeshAnimNodeSequence(PreviewSkelComp);
	if (!PreviewAnimNode)
	{
		return;
	}
	FLOAT PositionTime = InPosition * MaxTrackLength;
	for (INT i = AbilityAnimInfoList.Num() -1; i >= 0; --i)
	{
		if (PositionTime >= AbilityAnimInfoList(i).StartTime && PositionTime <= (AbilityAnimInfoList(i).StartTime + AbilityAnimInfoList(i).LastTime))
		{

			PreviewAnimNode->SetAnim(AbilityAnimInfoList(i).AnimName);
			PreviewAnimNode->SetPosition(PositionTime - AbilityAnimInfoList(i).StartTime + AbilityAnimInfoList(i).AnimStartTime, false);

			for (INT j = 0; j < PreviewWeaponList.Num(); ++j)
			{
				if (PreviewWeaponList(j))
				{
					PreviewAnimNode = GetMeshAnimNodeSequence(Cast<USkeletalMeshComponent>(PreviewWeaponList(j)));
					if (PreviewAnimNode)
					{
						PreviewAnimNode->SetAnim(AbilityAnimInfoList(i).AnimName);
						PreviewAnimNode->SetPosition(PositionTime - AbilityAnimInfoList(i).StartTime + AbilityAnimInfoList(i).AnimStartTime, false);
					}
				}
			}

			PreviewSkelComp->UpdateSkelPose();
			UpdateOtherComponents();
		}
	}
}

void WxAbilityViewer::RefreshAnimInfoList()
{
	UMMOAbility* ability = AbilityObject;
	if (!ability)
	{
		return;
	}

	AbilityAnimInfoList.Empty();

	FLOAT SpellTime = GetAbilitySpellTime();
	FLOAT CastTime = GetAbilityCastTime();

	TArray<FComponentNode>* SpellEffectList = NULL;
	TArray<FComponentNode>* CastEffectList = NULL;

	UBOOL bFound = AbilityObject->GetEffectComponentList(CurrentWeaponType, &SpellEffectList, &CastEffectList);
	if (!bFound)
	{
		return;
	}

	for (INT i = 0; i < (*SpellEffectList).Num(); ++i)
	{
		if ((*SpellEffectList)(i).Component && (*SpellEffectList)(i).Component->IsA(UAbilityAnimationComponent::StaticClass()))
		{
			UAbilityAnimationComponent* comp = (UAbilityAnimationComponent*)((*SpellEffectList)(i).Component);
			UAnimSequence* Seq = NULL;
			for (INT j = 0; j < comp->Anims.Num(); ++j)
			{
				Seq = GetAnimSeq(comp->Anims(j));
				if (Seq)
				{
					break;
				}
			}
			if (!Seq)
			{
				continue;
			}


			INT InsertIdx = -1;
			for (INT k = 0; k < AbilityAnimInfoList.Num(); ++k)
			{
				if ((*SpellEffectList)(i).StartTime < AbilityAnimInfoList(k).StartTime)
				{
					InsertIdx = k;
					break;
				}
			}

			INT idx = -1;
			if (InsertIdx != -1)
			{
				AbilityAnimInfoList.InsertZeroed(InsertIdx);
				idx = InsertIdx;
			}
			else
			{
				 idx = AbilityAnimInfoList.AddZeroed();
			}
			
			AbilityAnimInfoList(idx).bSpell = TRUE;
			AbilityAnimInfoList(idx).AnimName = Seq->SequenceName;
			AbilityAnimInfoList(idx).StartTime = (*SpellEffectList)(i).StartTime;
			AbilityAnimInfoList(idx).AnimStartTime = FLOAT(comp->StartFrame) / 30.f;
			
			if (comp->bLoop)
			{
				AbilityAnimInfoList(idx).LastTime = comp->LoopTime;
			}
			else
			{
				AbilityAnimInfoList(idx).LastTime = Seq->SequenceLength / comp->Rate;
			}
		}
	}

	for (INT i = 0; i < (*CastEffectList).Num(); ++i)
	{
		if ((*CastEffectList)(i).Component && (*CastEffectList)(i).Component->IsA(UAbilityAnimationComponent::StaticClass()))
		{
			UAbilityAnimationComponent* comp = (UAbilityAnimationComponent*)((*CastEffectList)(i).Component);
			UAnimSequence* Seq = NULL;
			for (INT j = 0; j < comp->Anims.Num(); ++j)
			{
				Seq = GetAnimSeq(comp->Anims(j));
				if (Seq)
				{
					break;
				}
			}
			if (!Seq)
			{
				continue;
			}

			INT InsertIdx = -1;
			for (INT k = 0; k < AbilityAnimInfoList.Num(); ++k)
			{
				if ((*CastEffectList)(i).StartTime + SpellTime < AbilityAnimInfoList(k).StartTime)
				{
					InsertIdx = k;
					break;
				}
			}

			INT idx = -1;
			if (InsertIdx != -1)
			{
				AbilityAnimInfoList.InsertZeroed(InsertIdx);
				idx = InsertIdx;
			}
			else
			{
				idx = AbilityAnimInfoList.AddZeroed();
			}

			AbilityAnimInfoList(idx).bSpell = FALSE;
			AbilityAnimInfoList(idx).AnimName = Seq->SequenceName;
			AbilityAnimInfoList(idx).StartTime = (*CastEffectList)(i).StartTime + SpellTime;
			AbilityAnimInfoList(idx).AnimStartTime = FLOAT(comp->StartFrame) / 30.f;
			if (comp->bLoop)
			{
				AbilityAnimInfoList(idx).LastTime = comp->LoopTime;
			}
			else
			{
				AbilityAnimInfoList(idx).LastTime = Seq->SequenceLength / comp->Rate;
			}
		}
	}

	for (INT i = 0; i < AbilityAnimInfoList.Num() - 1; ++i)
	{
		FLOAT EndTime = AbilityAnimInfoList(i).StartTime + AbilityAnimInfoList(i).LastTime;
		FLOAT NextStartTime = AbilityAnimInfoList(i + 1).StartTime;
		if (EndTime > NextStartTime)
		{
			AbilityAnimInfoList(i).LastTime = AbilityAnimInfoList(i + 1).StartTime - AbilityAnimInfoList(i).StartTime;
		}
	}
}

void WxAbilityViewer::ShowProperty(AbilityStateType Type, INT ComponentIdx)
{
	if (!AbilityObject)
	{
		return;
	}

	TArray<FComponentNode>* SpellEffectList = NULL;
	TArray<FComponentNode>* CastEffectList = NULL;

	UBOOL bFound = AbilityObject->GetEffectComponentList(CurrentWeaponType, &SpellEffectList, &CastEffectList);
	if (!bFound)
	{
		return;
	}

	FComponentNode* node = NULL;
	if (Type == EAST_Spell)
	{
		if (ComponentIdx >= 0 && ComponentIdx < (*SpellEffectList).Num())
		{
			node = &((*SpellEffectList)(ComponentIdx));
		}
	}
	else
	{
		if (ComponentIdx >= 0 && ComponentIdx < (*CastEffectList).Num())
		{
			node = &((*CastEffectList)(ComponentIdx));
		}
	}
	if (node && node->Component)
	{
		ComponentProperty->SetObject(node->Component, EPropertyWindowFlags::ShouldShowCategories);
	}
	else
	{
		ComponentProperty->SetObject(NULL, EPropertyWindowFlags::ShouldShowCategories);
	}
}

void WxAbilityViewer::DeleteComponent(AbilityComponentProxyType Type, AbilityStateType StateType, INT CompIdx, UBOOL bTail)
{
	if (!AbilityObject)
	{
		return;
	}

	TArray<FComponentNode>* SpellEffectList = NULL;
	TArray<FComponentNode>* CastEffectList = NULL;

	UBOOL bFound = AbilityObject->GetEffectComponentList(CurrentWeaponType, &SpellEffectList, &CastEffectList);
	if (!bFound)
	{
		return;
	}

	if (Type == EACPT_Component)
	{
		if (StateType == EAST_Spell)
		{
			if (CompIdx >= 0 && CompIdx < (*SpellEffectList).Num())
			{
				(*SpellEffectList).Remove(CompIdx);
			}
		}
		else
		{
			if (CompIdx >= 0 && CompIdx < (*CastEffectList).Num())
			{
				(*CastEffectList).Remove(CompIdx);
			}
		}
	}
}

UAbilityComponent* WxAbilityViewer::GetSelectComponentByType(AbilityStateType Type, INT ComponentIdx)
{
	if (!AbilityObject)
	{
		return NULL;
	}

	TArray<FComponentNode>* SpellEffectList = NULL;
	TArray<FComponentNode>* CastEffectList = NULL;

	UBOOL bFound = AbilityObject->GetEffectComponentList(CurrentWeaponType, &SpellEffectList, &CastEffectList);
	if (!bFound)
	{
		return NULL;
	}

	FComponentNode* node = NULL;
	if (Type == EAST_Spell)
	{
		if (ComponentIdx >= 0 && ComponentIdx < (*SpellEffectList).Num())
		{
			node = &((*SpellEffectList)(ComponentIdx));
		}
	}
	else
	{
		if (ComponentIdx >= 0 && ComponentIdx < (*CastEffectList).Num())
		{
			node = &((*CastEffectList)(ComponentIdx));
		}
	}
	if (node)
	{
		return node->Component;
	}
	else
	{
		return NULL;
	}

}

void WxAbilityViewer::OnAddEffectNode(wxCommandEvent& In)
{
	UClass** TC = ClassMap.Find(In.GetId());
	if ((*TC) && (*TC) != UAbilityComponent::StaticClass())
	{		  
		AbilityObject->AddComponent(*TC, bEditSpellState, CurrentWeaponType);
		AbilityObject->MarkPackageDirty();
		AbilityObject->PostEditChange();
	}
}

void WxAbilityViewer::OnResetCameraPosition(wxCommandEvent& In)
{
	//FLOAT Dis = 900.f;
	CameraPositionType = 0;
	FEditorAbilityViewportClient* vc = GetPreviewVC();
	check( vc );
	FVector NewLocation;
	FRotator NewRotation;

	NewRotation.Yaw = 32768; 
	NewRotation.Pitch = -5461;
	NewRotation.Roll = 0;

	NewLocation = NewRotation.Vector().SafeNormal() * (-1000.f);
	vc->SetCameraPosition(NewLocation, NewRotation);
}

void WxAbilityViewer::OnRotateCamera(wxCommandEvent& In)
{
	CameraPositionType = (++CameraPositionType) % 4;
	FEditorAbilityViewportClient* vc = GetPreviewVC();
	check( vc );
	vc->SetCameraPosition(CameraPositionType);

	//ConvertAllAbilityTime();

}

//Bruce@snda
void WxAbilityViewer::UpdateSwitchWeaponTypePreview()
{
	if (AbilityObject == NULL)
	{
		return;
	}

	std::vector<INT> TypeList;
	TArray<FString>  ArrayString;
	INT SelectIdx = 0;

	TypeList.push_back(AbilityObject->DefaultWeaponType);
	FString NewNode = GetWeaponTypeName(AbilityObject->DefaultWeaponType);
	ArrayString.AddItem( NewNode );

	if (CurrentWeaponType == AbilityObject->DefaultWeaponType)
	{
		SelectIdx = 0;
	}

	for (INT i = 0; i < AbilityObject->OverrideComponentList.Num(); ++i)
	{
		FString NewNode = GetWeaponTypeName(AbilityObject->OverrideComponentList(i).Type);
		ArrayString.AddItem( NewNode );
		TypeList.push_back(AbilityObject->OverrideComponentList(i).Type);

		if (AbilityObject->OverrideComponentList(i).Type == CurrentWeaponType)
		{
			SelectIdx = ArrayString.Num() - 1;
		}
	}

	ToolBar->SwitchWeaponTypeComboBox->Clear();

	for (INT ChunkIdx = 0; ChunkIdx < ArrayString.Num(); ++ChunkIdx)
	{
		ToolBar->SwitchWeaponTypeComboBox->Append( *ArrayString(ChunkIdx) );
	}

	ToolBar->SwitchWeaponTypeComboBox->SetSelection(0);
}

void WxAbilityViewer::UpdateWeaponTypePreview()
{
	if (AbilityObject == NULL)
	{
		return;
	}

	std::vector<INT> TypeList;
	TArray<FString> ArrayString;
	for (INT i = 0; i < EWT_MAX; ++i)
	{
		UBOOL bFind = FALSE;
		for (INT j = 0; j < AbilityObject->OverrideComponentList.Num(); ++j)
		{
			if (i == AbilityObject->OverrideComponentList(j).Type)
			{
				bFind = TRUE;
				break;
			}
		}
		if (i == CurrentWeaponType)
		{
			bFind = TRUE;
		}
		if (bFind == FALSE)
		{
			FString NewNode = GetWeaponTypeName(i);
			ArrayString.AddItem( NewNode );
			TypeList.push_back(i);
		}
	}

	ToolBar->WeaponTypeComboBox->Clear();

	for (INT ChunkIdx = 0; ChunkIdx < ArrayString.Num(); ++ChunkIdx)
	{
		ToolBar->WeaponTypeComboBox->Append( *ArrayString(ChunkIdx) );
	}

	ToolBar->WeaponTypeComboBox->SetSelection(0);
}

void WxAbilityViewer::OnSwitchWeaponType(wxCommandEvent& In)
{
	if (AbilityObject == NULL)
	{
		return;
	}

	std::vector<INT> TypeList;
	wxArrayString ArrayString;
	INT SelectIdx = 0;

	TypeList.push_back(AbilityObject->DefaultWeaponType);
	FString NewNode = GetWeaponTypeName(AbilityObject->DefaultWeaponType);
	ArrayString.Add(wxString(*NewNode));
	if (CurrentWeaponType == AbilityObject->DefaultWeaponType)
	{
		SelectIdx = 0;
	}

	for (INT i = 0; i < AbilityObject->OverrideComponentList.Num(); ++i)
	{
		FString NewNode = GetWeaponTypeName(AbilityObject->OverrideComponentList(i).Type);
		ArrayString.Add(wxString(*NewNode));
		TypeList.push_back(AbilityObject->OverrideComponentList(i).Type);

		if (AbilityObject->OverrideComponentList(i).Type == CurrentWeaponType)
		{
			SelectIdx = ArrayString.GetCount() - 1;
		}
	}

	INT idx = ToolBar->SwitchWeaponTypeComboBox->GetSelection();
	if (CurrentWeaponType != TypeList[idx])
	{
		CurrentWeaponType = TypeList[idx];
		RefreshData();
	}
}

void WxAbilityViewer::OnAddWeaponTypeByComboBox(wxCommandEvent& In)
{
	std::vector<INT> TypeList;
	wxArrayString ArrayString;
	for (INT i = 0; i < EWT_MAX; ++i)
	{
		UBOOL bFind = FALSE;
		for (INT j = 0; j < AbilityObject->OverrideComponentList.Num(); ++j)
		{
			if (i == AbilityObject->OverrideComponentList(j).Type)
			{
				bFind = TRUE;
				break;
			}
		}
		if (i == CurrentWeaponType)
		{
			bFind = TRUE;
		}
		if (bFind == FALSE)
		{
			FString NewNode = GetWeaponTypeName(i);
			ArrayString.Add(wxString(*NewNode));
			TypeList.push_back(i);
		}
	}

	INT idx = ToolBar->WeaponTypeComboBox->GetSelection();

	CurrentWeaponType = TypeList[idx];
	idx = AbilityObject->OverrideComponentList.AddZeroed();
	if ( idx < AbilityObject->OverrideComponentList.Num() )
	{
		AbilityObject->OverrideComponentList(idx).Type = CurrentWeaponType;
	}

	RefreshData();

	UpdateSwitchWeaponTypePreview();
	UpdateWeaponTypePreview();
}

void WxAbilityViewer::OnExchangeConfig(wxCommandEvent& In)
{	
	UMMOAbility* data = AbilityObject;

	if ( data )
	{
		UAbilityPreviewResources* apr = Cast<UAbilityPreviewResources>( data->PreviewResource );
		check(apr);

		apr->PreEditChange( NULL );		
		if ( apr->Attacker && apr->Target )
		{
			UAvatarData* TempAvatar = apr->Target;
			apr->Target = apr->Attacker;
			apr->Attacker= TempAvatar;
		}
		apr->PostEditChange();
		apr->MarkPackageDirty();
	}	

	RefreshData();	
}

void WxAbilityViewer::OnRefreshViewAndConfig(wxCommandEvent& In)
{
	RefreshData();	
}

void WxAbilityViewer::SettingsFunction( FString AttackerStr, FString TargetStr )
{
	UMMOAbility* data = AbilityObject;

	if ( data )
	{
		UAbilityPreviewResources* apr = Cast<UAbilityPreviewResources>( data->PreviewResource );
		check(apr);

		apr->PreEditChange( NULL );	

		apr->Attacker = Cast<UAvatarData>(UObject::StaticLoadObject(UAvatarData::StaticClass(), NULL, *AttackerStr, NULL,  LOAD_NoWarn, NULL)); 
		apr->Target = Cast<UAvatarData>(UObject::StaticLoadObject(UAvatarData::StaticClass(), NULL, *TargetStr, NULL,  LOAD_NoWarn, NULL)); 

		apr->PostEditChange();
		apr->MarkPackageDirty();
	}	

	RefreshData();	
}

void WxAbilityViewer::OnWarriorSettings(wxCommandEvent& In)
{
	SettingsFunction( LocalizeUnrealEdMMO( "WarriorAttackerSettings" ), LocalizeUnrealEdMMO( "WarriorTargetSettings" ) );
}
	
void WxAbilityViewer::OnFemaleWarriorSettings(wxCommandEvent& In)
{
	SettingsFunction( LocalizeUnrealEdMMO( "FemaleWarriorAttackerSettings" ), LocalizeUnrealEdMMO( "FemaleWarriorTargetSettings" ) );
}

void WxAbilityViewer::OnWizardSettings(wxCommandEvent& In)
{
	SettingsFunction( LocalizeUnrealEdMMO( "WizardAttackerSettings" ), LocalizeUnrealEdMMO( "WizardTargetSettings" ) );
}

void WxAbilityViewer::OnFemaleWizardSettings(wxCommandEvent& In)
{
	SettingsFunction( LocalizeUnrealEdMMO( "FemaleWizardAttackerSettings" ), LocalizeUnrealEdMMO( "FemaleWizardTargetSettings" ) );	
}

void WxAbilityViewer::OnArcherSettings(wxCommandEvent& In)
{
	SettingsFunction( LocalizeUnrealEdMMO( "ArcherAttackerSettings" ), LocalizeUnrealEdMMO( "ArcherTargetSettings" ) );
}

void WxAbilityViewer::OnFemaleArcherSettings(wxCommandEvent& In)
{
	SettingsFunction( LocalizeUnrealEdMMO( "FemaleArcherAttackerSettings" ), LocalizeUnrealEdMMO( "FemaleArcherTargetSettings" ) );
}

void WxAbilityViewer::OnAssassinSettings(wxCommandEvent& In)
{
	SettingsFunction( LocalizeUnrealEdMMO( "AssassinAttackerSettings" ), LocalizeUnrealEdMMO( "AssassinTargetSettings" ) );
}

void WxAbilityViewer::OnFemaleAssassinSettings(wxCommandEvent& In)
{
	SettingsFunction( LocalizeUnrealEdMMO( "FemaleAssassinAttackerSettings" ), LocalizeUnrealEdMMO( "FemaleAssassinTargetSettings" ) );
}
//Bruce@snda

void WxAbilityViewer::OnAddTrackLine(wxCommandEvent& In)
{
	MaxShowTrackLength += 1.f;
}
void WxAbilityViewer::OnSubTrackLine(wxCommandEvent& In)
{
	MaxShowTrackLength -= 1.f;
	if (MaxShowTrackLength <= 1)
	{
		MaxShowTrackLength = 1.f;
	}
}

void WxAbilityViewer::SwithEditState(wxCommandEvent& In)
{
	bEditSpellState = !bEditSpellState;
	if (bEditSpellState)
	{
		SwithEditStateButton->SetBitmapLabel(SpellImage);
	}
	else
	{
		SwithEditStateButton->SetBitmapLabel(CastImage);
	}

	PreviewWindow->ASVPreviewVC->ComponentIdx = -1;
	SwithEditStateButton->Refresh();
}

//void WxAbilityViewer::OnAddHitPoint(wxCommandEvent& In)
//{
//	UMMOAbility* ability = AbilityObject;
//	if (!ability)
//	{
//		return;
//	}
//
//	ability->AddComponent(UAbilityHitPointComponent::StaticClass(), bEditSpellState, CurrentWeaponType);
//	ability->MarkPackageDirty();
//}


void WxAbilityViewer::PopupAddMenu(INT x, INT y)
{

	// display menu items
	UBOOL bChecked = FALSE;
	for (INT i = IDM_EFFECT_NODE_SHOW_START; i < IDM_EFFECT_NODE_SHOW_START + ClassMap.Num(); ++i)
	{
		bChecked = IsShowItemChecked(i);
		DisplayMenu->Check(i, (bChecked==0 ? false : true));
	}
	bChecked = IsShowItemChecked(IDM_SHOW_CALC_SIMULATE);
	DisplayMenu->Check(IDM_SHOW_CALC_SIMULATE, (bChecked==0 ? false : true));


	// add menu items
	for (INT i = IDM_EFFECT_NODE_SHOW_START; i < IDM_EFFECT_NODE_SHOW_START + ClassMap.Num(); ++i)
	{
		bChecked = IsShowItemChecked(i);
		BaseMenu->Enable(i + IDM_EFFECT_NODE_ADD_START - IDM_EFFECT_NODE_SHOW_START, (bChecked==0 ? false : true));
	}

	PopupMenu(BaseMenu);
}



void WxAbilityViewer::SetPlayAbility(UBOOL bPlay)
{
	UAnimNodeSequence * PreviewAnimNode = GetMeshAnimNodeSequence(PreviewSkelComp);
	if (bPlay)
	{
		ULineBatchComponent* LineBatch = GetLineBatchComponent();
		if (LineBatch)
		{
			LineBatch->BatchedLines.Empty();
			LineBatch->bNeedsReattach = TRUE;
			LineBatch->BeginDeferredReattach();
		}
	}

	bPlaying = bPlay;
	if (bPlaying)
	{
		if (MaxTrackLength - CurrentPlayTime < 0.1)
		{
			CurrentPlayTime = 0.f;
			CurrentCycleCount = 0;
		}

		//if (PreviewAnimNode && PreviewAnimNode->AnimSeq)
		//{
		//	PreviewAnimNode->PlayAnim(PreviewAnimNode->bLooping, PreviewAnimNode->Rate, PreviewAnimNode->CurrentTime);
		//}
	}
	else
	{
		//if (PreviewAnimNode && PreviewAnimNode->AnimSeq)
		//{
		//	PreviewAnimNode->StopAnim();
		//}
	}
}

void WxAbilityViewer::SetLoopAbility(UBOOL bLoop)
{
	bLooping = bLoop;
}




void WxAbilityViewer::PlayAnimation(FAnimationNode* node)
{
	if (node == NULL)
	{
		return;
	}

	UAnimNodeSequence * PreviewAnimNode = GetMeshAnimNodeSequence(PreviewSkelComp);

	SetSelectedAnim(node->animation);
	if (PreviewAnimNode)
	{
		PreviewAnimNode->PlayAnim(node->bLoop, node->Rate, 0.0);
		//PreviewAnimNodeRaw->PlayAnim(node->bLoop, node->Rate, 0.0);
		if (node->bLoop)
		{
			LoopAnimLeftTime = node->LastTime;
		}
	}
	else
	{
		return;
	}
	
}

FLOAT WxAbilityViewer::GetAbilityLastTime()
{
	return GetAbilitySpellTime() + GetAbilityCastTime(TRUE);
}

FLOAT WxAbilityViewer::GetAbilitySpellTime()
{
	if (!AbilityObject)
	{
		return 0.f;
	}
	UMMOAbility* ability = AbilityObject;
	FLOAT Ret = 0.f;
	FLOAT wf = GetWeaponTimeFactor();
	ability->GetSpellTime(Ret);
	if (AbilityObject->SpellCommonAttribute.AffectedBySpeedParam)
	{
		return Ret * wf;
	}
	else
	{
		return Ret;
	}
}

void WxAbilityViewer::SetAbilitySpellTime(FLOAT NewTime)
{
	check(AbilityObject);
	FLOAT wf = GetWeaponTimeFactor();
	if (AbilityObject->SpellCommonAttribute.AffectedBySpeedParam)
	{
		NewTime /= wf;
	}
	AbilityObject->SetSpellTime(NewTime);
}
void WxAbilityViewer::SetAbiltiyCastTime(FLOAT NewTime)
{
	check(AbilityObject);
	FLOAT wf = GetWeaponTimeFactor();
	if (AbilityObject->SpellCommonAttribute.AffectedBySpeedParam)
	{
		NewTime /= wf;
	}
	AbilityObject->SetCastTime(NewTime);
}
FLOAT WxAbilityViewer::GetAbilityCastTime(UBOOL bGetMaxTime)
{
	check(AbilityObject);
	FLOAT wf = GetWeaponTimeFactor();
	FLOAT CastTime = 0.f, FixTime = 0.f;
	INT CycleNumber = 1;
	AbilityObject->GetCastTime(CastTime, CycleNumber, FixTime);
	if (AbilityObject->SpellCommonAttribute.AffectedBySpeedParam)
	{
		CastTime *= wf;
		FixTime *= wf;
	}
	if (bGetMaxTime)
	{
		return CastTime * FLOAT(CycleNumber) + FixTime;
	}
	else
	{
		return CastTime;
	}
}

FLOAT WxAbilityViewer::GetWeaponTimeFactor()
{
	BYTE WT = CurrentWeaponType;
	if (GamePlayData)
	{
		return GamePlayData->GetWeaponSpeedFactor(WT);
	}
	else
	{
		return 1.0;
	}
}



FPreviewScene* WxAbilityViewer::GetPreviewScene()
{
	return &(PreviewWindow->ASVPreviewVC->PreviewScene);

}

void WxAbilityViewer::ClearAllLines()
{
	GetLineBatchComponent()->BatchedLines.Empty();
	GetLineBatchComponent()->BeginDeferredReattach();
}

void WxAbilityViewer::DrawLine(FVector& Start, FVector& End, FLinearColor& Color)
{
	GetLineBatchComponent()->DrawLine(Start, End, Color, SDPG_World);
}


ULineBatchComponent* WxAbilityViewer::GetLineBatchComponent()
{
	return PreviewWindow->ASVPreviewVC->AbilityAttackRangeLineBatch;  
	//return PreviewWindow->ASVPreviewVC->LineBatcher;
}

void WxAbilityViewer::ClearAllComponents()
{
	if (!PreviewSkelComp)
	{
		return;
	}

	for (INT i = 0; i < PreviewSkelComp->Attachments.Num(); ++i)
	{
		if (PreviewSkelComp->Attachments(i).Component != NULL)
		{
			if (PreviewSkelComp->Attachments(i).Component->IsA(UParticleSystemComponent::StaticClass()))
			{
				UParticleSystemComponent* comp = Cast<UParticleSystemComponent>(PreviewSkelComp->Attachments(i).Component);
				if (comp)
				{
					comp->DeactivateSystem();
				}
			}
		}
	}

	TArray<UMeshComponent*> ComponentList;
	for (INT i = 0; i < PreviewSkelComp->Attachments.Num(); ++i)
	{
		UActorComponent* ic = PreviewSkelComp->Attachments(i).Component;
		UMeshComponent* im = Cast<UMeshComponent>(ic);
		if (im == NULL)
		{
			PreviewSkelComp->DetachComponent(ic);
		}
	}

	UAnimNodeSequence * PreviewAnimNode = GetMeshAnimNodeSequence(PreviewSkelComp);
	if (PreviewAnimNode)
	{
		PreviewAnimNode->StopAnim();
	}

	FPreviewScene* ps = GetPreviewScene();
	for (INT i = 0; i < EditorAbilityComponentList.Num(); ++i)
	{
		UActorComponent* node = EditorAbilityComponentList(i)->GetComponent();
		if (node)
		{
			ps->RemoveComponent(EditorAbilityComponentList(i)->GetComponent());

			EditorAbilityComponentList(i)->ReturnComponentToPool();
		}

		delete EditorAbilityComponentList(i);
		EditorAbilityComponentList(i) = NULL;
	}

	EditorAbilityComponentList.Empty();

	if (GetPreviewVC() && GetPreviewVC()->PreviewPostProcessChain && GetPreviewVC()->PreviewPostProcessChain->Effects.Num() > 0)
	{
		GetPreviewVC()->PreviewPostProcessChain->Effects.Empty();
	}
}

void WxAbilityViewer::OnHitPoint(UAbilityComponent* InHitPointComponent)
{
	//UAnimNodeSequence* AnimNode = GetMeshAnimNodeSequence(PreviewSkelCompTarget);
	//if (AnimNode)
	//{
	//	AnimNode->SetAnim(FName(TEXT("wound1")));
	//	AnimNode->PlayAnim();
	//}

	UAbilityHitPointComponent* HitPoint = Cast<UAbilityHitPointComponent>(InHitPointComponent);
	if (HitPoint)
	{
		for (INT i = 0; i < HitPoint->ServerEffectList.Num(); ++i)
		{
			if (HitPoint->ServerEffectList(i) != NULL)
			{
				OnStartComponent(HitPoint->ServerEffectList(i), 0.f, TRUE);
			}
		}
	}

	BYTE ResultType = AbilityObject->PreviewResource->ResultType;
	FHitGroup HitGroup;
	HitGroup.WeaponType = HitGroup.DirectionType = HitGroup.WeaponHitType = 0;
	BYTE wType = EWT_Weapon_0;
	TArray<UEquipMesh*> WeaponList;
	for (INT i = 0; i < AbilityObject->PreviewResource->WeaponList.Num(); ++i)
	{
		if (AbilityObject->PreviewResource->WeaponList(i)
			&& AbilityObject->PreviewResource->WeaponList(i)->GetEquipAppearenceTemplate()
			&& CurrentWeaponType == AbilityObject->PreviewResource->WeaponList(i)->GetEquipAppearenceTemplate()->WeaponType)
		{
			wType = AbilityObject->PreviewResource->WeaponList(i)->GetEquipAppearenceTemplate()->WeaponType;
			if (AbilityObject->PreviewResource->bMale)
			{
				WeaponList = AbilityObject->PreviewResource->WeaponList(i)->GetEquipAppearenceTemplate()->MeshListTemplate_M;
			}
			else
			{
				WeaponList = AbilityObject->PreviewResource->WeaponList(i)->GetEquipAppearenceTemplate()->MeshListTemplate_F;
			}
			break;
		}
	}
	for (INT i = 0; i < AbilityObject->ClientCommonAttribute.HitGroupInfo.Num(); ++i)
	{
		if (wType == AbilityObject->ClientCommonAttribute.HitGroupInfo(i).WeaponType)
		{
			HitGroup = AbilityObject->ClientCommonAttribute.HitGroupInfo(i);
			break;
		}
	}

	INT dep1 = -1;
	INT dep2 = 1;

	if (ResultType == DRT_Result2 || ResultType == DRT_Result3 || ResultType == DRT_Result4 || ResultType == DRT_Result5)
	{
		//dep = Facade::getSingleton().GetDamgeTemplateData(ResultType, 0, 0, 0);
		dep1 = GamePlayData->GetDamageEffectListIndex(ResultType, 0, 0, 0);
	}
	else if (ResultType == DRT_Result6)	// Normal attack
	{
		if (HitGroup.WeaponHitType == EWHT_HitType1
			|| HitGroup.WeaponHitType == EWHT_HitType2
			|| HitGroup.WeaponHitType == EWHT_HitType4
			|| HitGroup.WeaponHitType == EWHT_HitType5)
		{
			dep1 = GamePlayData->GetDamageEffectListIndex(ResultType, HitGroup.WeaponHitType, HitGroup.DirectionType, 0);
		}
		else
		{
			dep1 = GamePlayData->GetDamageEffectListIndex(ResultType, HitGroup.WeaponHitType, 0, 0);
		}

		dep2 = GamePlayData->GetDamageEffectListIndex(ResultType, 0, 0, AbilityObject->SpellCommonAttribute.ElementaryType);
	}
	else if (ResultType == DRT_Result7)	 // Explosive crit
	{
		dep1 = GamePlayData->GetDamageEffectListIndex(ResultType, HitGroup.WeaponHitType, 0, 0);
		dep2 = GamePlayData->GetDamageEffectListIndex(ResultType, 0, 0, AbilityObject->SpellCommonAttribute.ElementaryType);
	}

	if (dep1 != -1)
	{
		for (INT i = 0; i < GamePlayData->DamageTemplateData->DamageEffectList(dep1).EffectList.Num(); ++i)
		{
			OnStartComponent(GamePlayData->DamageTemplateData->DamageEffectList(dep1).EffectList(i), CurrentPlayTime, TRUE);
		}
	}

	if (dep2 != -1)
	{
		for (INT i = 0; i < GamePlayData->DamageTemplateData->DamageEffectList(dep2).EffectList.Num(); ++i)
		{
			OnStartComponent(GamePlayData->DamageTemplateData->DamageEffectList(dep2).EffectList(i), CurrentPlayTime, TRUE);
		}
	}
}


void WxAbilityViewer::TickAbility(FLOAT DeltaTime)
{
	ULineBatchComponent* LineBatch = GetLineBatchComponent();
	if (LineBatch)
	{
		LineBatch->Tick(DeltaTime);
	}

	if (bPlaying)
	{
		OnUpdateMovement(DeltaTime);

		OnUpdatePreviewComponents(DeltaTime);

		UpdateMirrorList(DeltaTime);


		FLOAT NewTime = CurrentPlayTime + DeltaTime;
		UMMOAbility* ability = AbilityObject;
		FLOAT WeaponTimeFactor = GetWeaponTimeFactor();
		FLOAT SpellTime;
		AbilityObject->GetSpellTime(SpellTime);
		if (SpellTime < 0.0001)
		{
			SpellTime = 0.0001;
		}
		if (AbilityObject->SpellCommonAttribute.AffectedBySpeedParam)
		{
			SpellTime *= WeaponTimeFactor;
		}
		

		FLOAT CastTime, FixTime;
		INT MaxCycleNumber;
		FLOAT MaxCastTime = 0.f;
		AbilityObject->GetCastTime(CastTime, MaxCycleNumber, FixTime);
		if (AbilityObject->SpellCommonAttribute.AffectedBySpeedParam)
		{
			CastTime *= WeaponTimeFactor;
			FixTime *= WeaponTimeFactor;
		}

		MaxCastTime = CastTime * MaxCycleNumber + FixTime;

		//// check weapon state
		//if (CurrentPlayTime <= 0.000001f)
		//{
		//	HideWeapon(AbilityObject->ClientCommonAttribute.bHideWeaponInSpell);
		//}


		TArray<FComponentNode>* SpellEffectList = NULL;
		TArray<FComponentNode>* CastEffectList = NULL;

		UBOOL bFound = AbilityObject->GetEffectComponentList(CurrentWeaponType, &SpellEffectList, &CastEffectList);

		if (!bFound)
		{
			return;
		}

		// spell effect

		// spell effect
		for (INT i = 0; i < SpellEffectList->Num(); ++i)
		{
			if ((*SpellEffectList)(i).Component == NULL)
			{
				continue;
			}

			if (CurrentPlayTime <= (*SpellEffectList)(i).StartTime && (*SpellEffectList)(i).StartTime < NewTime)
			{
				OnStartComponent((*SpellEffectList)(i).Component, CurrentPlayTime);
			}
		}

		// cast spell
		FLOAT TempCastTime = 0.f, TempNewCastTime = 0.f;

		if (CurrentCycleCount == 0)
		{
			if (NewTime > SpellTime)
			{
				CurrentCycleCount = 1;
				OnNotifyAbilityStateEnd();
				OnAbilityCast();
				TempCastTime = 0.f;
				TempNewCastTime = NewTime - SpellTime;
			}
		}
		else
		{
			if (MaxCycleNumber == 1)
			{
				TempCastTime = CurrentPlayTime - SpellTime;
				TempNewCastTime = NewTime - SpellTime;
			}
			else
			{
				TempCastTime = CurrentPlayTime - SpellTime - (CurrentCycleCount - 1) * CastTime;
				TempNewCastTime = NewTime - SpellTime - (CurrentCycleCount - 1) * CastTime;
				if (TempNewCastTime > CastTime)
				{
					if (CurrentCycleCount < MaxCycleNumber)
					{
						CurrentCycleCount++;
						OnNotifyAbilityStateEnd();
						OnAbilityCast();
						TempCastTime = 0.f;
						TempNewCastTime = TempNewCastTime - CastTime;
					}
				}
			}
		}

		if (CurrentCycleCount >= 1)
		{
			for (INT i = 0; i < (*CastEffectList).Num(); ++i)
			{
				if ((*CastEffectList)(i).Component == NULL)
				{
					continue;
				}

				if (TempCastTime <= (*CastEffectList)(i).StartTime && (*CastEffectList)(i).StartTime < TempNewCastTime)
				{
					OnStartComponent((*CastEffectList)(i).Component, CurrentPlayTime - SpellTime);
				}
			}
		}

		CurrentPlayTime = NewTime;

		if (CurrentPlayTime >= SpellTime + MaxCastTime)
		{
			//HideWeapon(FALSE);
		}

		if (CurrentPlayTime >= MaxTrackLength + 0.0)
		{
			if (bLooping)
			{
				CurrentPlayTime = 0.f;
				CurrentCycleCount = 0;
				ULineBatchComponent* LineBatch = GetLineBatchComponent();
				if (LineBatch)
				{
					LineBatch->BatchedLines.Empty();
					LineBatch->bNeedsReattach = TRUE;
					LineBatch->BeginDeferredReattach();
				}
			}
			else
			{
				CurrentPlayTime = MaxTrackLength + 0.0;
				bPlaying = false;
			}

			ResetPreviewMeshLocation();

			ClearAllMirror();

			ClearAllComponents();
		}
	}
}

void WxAbilityViewer::HideWeapon(bool bHidden)
{
	if (!AbilityObject)
	{
		return;
	}

	for (INT i = 0; i < PreviewWeaponList.Num(); ++i)
	{
		if (PreviewWeaponList(i))
		{
			PreviewWeaponList(i)->SetHiddenEditor(bHidden);
		}
	}
}

void WxAbilityViewer::SetVisible(UBOOL bAttacker, UBOOL bVisible)
{
	if (bAttacker)
	{
		if (PreviewSkelComp)
		{
			PreviewSkelComp->SetHiddenEditor(!bVisible);
		}

		if (PreviewMergedComp)
		{
			PreviewMergedComp->SetHiddenEditor(!bVisible);
		}
	}
	else
	{
		if (PreviewSkelCompTarget)
		{
			PreviewSkelCompTarget->SetHiddenEditor(!bVisible);
		}
	}
}

void WxAbilityViewer::SetScale(UBOOL bAttacker, FVector InScale)
{
	if (bAttacker)
	{
		if (PreviewSkelComp)
		{
			PreviewSkelComp->Scale3D = InScale;
		}

		if (PreviewMergedComp)
		{
			PreviewMergedComp->Scale3D = InScale;
		}
	}
	else
	{
		if (PreviewSkelCompTarget)
		{
			PreviewSkelCompTarget->Scale3D = InScale;
		}
	}
}

// 
void WxAbilityViewer::OnAbilityCast()
{
	if (!AbilityObject)
	{
		return;
	}

	//// check weather need to hide weapon
	//HideWeapon(AbilityObject->ClientCommonAttribute.bHideWeaponInCast);


	TArray<FComponentNode>* SpellEffectList = NULL;
	TArray<FComponentNode>* CastEffectList = NULL;

	UBOOL bFound = AbilityObject->GetEffectComponentList(CurrentWeaponType, &SpellEffectList, &CastEffectList);

	if (bFound)
	{
		// bullet
		for (INT i = 0; i < (*CastEffectList).Num(); ++i)
		{
			if ((*CastEffectList)(i).Component && (*CastEffectList)(i).Component->IsA(UAbilityHitPointComponent::StaticClass()))
			{
				UAbilityHitPointComponent* HitPoint = Cast<UAbilityHitPointComponent>((*CastEffectList)(i).Component);
				if (HitPoint)
				{
					// bullet
					for (INT i = 0; i < HitPoint->BulletComponents.Num(); ++i)
					{
						if (HitPoint->BulletComponents(i))
						{
							OnStartComponentEmitter(HitPoint->BulletComponents(i), this, TRUE, 0.f);
						}
					}
				}
			}
		}


	}

	// create missile
	if (AbilityObject->SpellCommonAttribute.OnCast)
	{
		USeqAct_Modifier* sm = NULL;
		for (INT i = 0; i < AbilityObject->SpellCommonAttribute.OnCast->Modifiers.Num(); ++i)
		{
			sm = AbilityObject->SpellCommonAttribute.OnCast->Modifiers(i);
			if (sm && sm->Key == 7 && sm->bRoot)
			{
				break;
			}
			else
			{
				sm = NULL;
			}
		}

		if (sm)
		{
			if (MissileList.Num() == 0)
			{
				LoadMissleData();
			}

			if (MissileList.Num() == 0)
			{
				return;																 
			}

			for (INT i = 0; i < MissileList.Num(); ++i)
			{
				UMissile* md = MissileList(i);
				INT Param1 = _tstoi(*sm->Param1.ValueStr);
				INT Param2 = _tstoi(*sm->Param2.ValueStr);
				INT Param3 = _tstoi(*sm->Param3.ValueStr);

				if (md->Id == Param1 && md->ParticleSystemTemplate)
				{
					EditorMissleComponent* emc = new EditorMissleComponent;
					emc->Viewer = this;
					emc->Speed = FVector(1, 0, 0) * md->Speed;
					emc->Duration = md->LifeTime;
					emc->DynamicComponent = GetParticleSystemComponentFromPool();
					emc->DynamicComponent->SetTemplate(md->ParticleSystemTemplate);
					emc->DynamicComponent->ActivateSystem();
					FLOAT degree = Param2;
					degree = degree * 3.1415926 / 180.f;
					FLOAT dx = Param3;
					FLOAT dy = appTan(degree) * dx;
					FVector offset(dx, 0, dy);
					FTranslationMatrix tm(offset);
					FMatrix NewMat = tm * PreviewSkelComp->CachedParentToWorld;
					GetPreviewScene()->AddComponent(emc->DynamicComponent, NewMat);
					EditorAbilityComponentList.AddItem(emc);
				}
			}
		}
	}

	// draw damage calculating lines
	if (IsShowItemChecked(IDM_SHOW_CALC_SIMULATE))
	{
		ULineBatchComponent* LineBatch = GetLineBatchComponent();
		if (LineBatch)
		{
			LineBatch->BatchedLines.Empty();
			LineBatch->bNeedsReattach = TRUE;

			INT CheckDelta = 400;
			FLinearColor Color(1.0, 0, 0, 1);
			// offset matrix
			FRotator OffsetRot(0, 0, 0);
			FVector OffsetVector;
			FMatrix OriginMat;

			// self
			if (AbilityObject->SpellCommonAttribute.LockType == E_splt_Self ||
				AbilityObject->SpellCommonAttribute.LockType == E_splt_Direction ||
				AbilityObject->SpellCommonAttribute.LockType == E_splt_SelfPos)
			{
				OffsetRot.Yaw = 65536.f * FLOAT(AbilityObject->SpellCommonAttribute.OffsetDirection) / 360.f;
				OffsetVector = FVector(AbilityObject->SpellCommonAttribute.OffsetDistance, 0, 0);
			}
			else
			{
				OffsetRot.Yaw = 65536.f * FLOAT(AbilityObject->SpellCommonAttribute.OffsetDirection) / 360.f;
				OffsetVector = FVector(AbilityObject->SpellCommonAttribute.OffsetDistance, 0, 0);
				if (AbilityObject->PreviewResource)
				{
					OffsetVector += AbilityObject->PreviewResource->PositionOffset;
				}
			}

			FRotationTranslationMatrix rtm(OffsetRot, OffsetVector);
			OriginMat = rtm * PreviewSkelComp->CachedParentToWorld;

			//FVector Ogr = NewMat.GetOrigin();
			TArray<ULineBatchComponent::FLine> Lines;
			FVector Pos1, Pos2;


			if (AbilityObject->SpellCommonAttribute.RangeType == E_sprt_Single)	// target object
			{

			}
			else if (AbilityObject->SpellCommonAttribute.RangeType == E_sprt_Circle)	// cycle
			{
				FLOAT Radius1 = AbilityObject->SpellCommonAttribute.RangeParam1;
				FLOAT Radius2 = AbilityObject->SpellCommonAttribute.RangeParam2;
				for (INT i = 0; i < 65536; i += CheckDelta)
				{
					Pos1.X = Radius1 * GMath.CosTab(i);
					Pos1.Y = Radius1 * GMath.SinTab(i);
					Pos1.Z = 0;
					Pos2.X = Radius2 * GMath.CosTab(i);
					Pos2.Y = Radius2 * GMath.SinTab(i);
					Pos2.Z = 0;
					Pos1 = OriginMat.TransformFVector(Pos1);
					Pos2 = OriginMat.TransformFVector(Pos2);
					Lines.AddItem(ULineBatchComponent::FLine(Pos1, Pos2, Color, LineBatch->DefaultLifeTime, 1, SDPG_World));
				}
			}
			//else if (AbilityObject->SpellCommonAttribute.RangeType == ESR_Type3)	// cycle1 - cycle2
			//{
			//	FLOAT Radius1 = AbilityObject->SpellCommonAttribute.RangeParam1;
			//	FLOAT Radius2 = AbilityObject->SpellCommonAttribute.RangeParam2;
			//	for (INT i = 0; i < 65536; i += CheckDelta)
			//	{
			//		Pos1.X = Radius1 * GMath.CosTab(i);
			//		Pos1.Y = Radius1 * GMath.SinTab(i);
			//		Pos1.Z = 0;

			//		Pos2.X = Radius2 * GMath.CosTab(i);
			//		Pos2.Y = Radius2 * GMath.SinTab(i);
			//		Pos2.Z = 0;

			//		Pos1 = OriginMat.TransformFVector(Pos1);
			//		Pos2 = OriginMat.TransformFVector(Pos2);

			//		Lines.AddItem(ULineBatchComponent::FLine(Pos1, Pos2, Color, LineBatch->DefaultLifeTime, 1, SDPG_World));
			//	}
			//}
			else if (AbilityObject->SpellCommonAttribute.RangeType == E_sprt_Sector)
			{
				FLOAT Radius = AbilityObject->SpellCommonAttribute.RangeParam1;
				INT Yaw1 = PreviewSkelComp->CachedParentToWorld.Rotator().Yaw - AbilityObject->SpellCommonAttribute.RangeParam3 * 65536 / 360 / 2;
				INT Yaw2 = PreviewSkelComp->CachedParentToWorld.Rotator().Yaw + AbilityObject->SpellCommonAttribute.RangeParam3 * 65536 / 360 / 2;
				for (INT i = Yaw1; i < Yaw2; i += CheckDelta)
				{
					Pos1.X = Radius * GMath.CosTab(i);
					Pos1.Y = Radius * GMath.SinTab(i);
					Pos1.Z = 0;

					Pos2.X = 0;
					Pos2.Y = 0;
					Pos2.Z = 0;

					Pos1 = OriginMat.TransformFVector(Pos1);
					Pos2 = OriginMat.TransformFVector(Pos2);

					Lines.AddItem(ULineBatchComponent::FLine(Pos1, Pos2, Color, LineBatch->DefaultLifeTime, 1, SDPG_World));
				}
			}
			else if (AbilityObject->SpellCommonAttribute.RangeType == E_sprt_Arc)
			{
				FLOAT Radius1 = AbilityObject->SpellCommonAttribute.RangeParam1;
				FLOAT Radius2 = AbilityObject->SpellCommonAttribute.RangeParam2;
				INT Yaw1 = PreviewSkelComp->CachedParentToWorld.Rotator().Yaw - AbilityObject->SpellCommonAttribute.RangeParam3 * 65536 / 360 / 2;
				INT Yaw2 = PreviewSkelComp->CachedParentToWorld.Rotator().Yaw + AbilityObject->SpellCommonAttribute.RangeParam3 * 65536 / 360 / 2;
				for (INT i = Yaw1; i < Yaw2; i += CheckDelta)
				{
					Pos1.X = Radius1 * GMath.CosTab(i);
					Pos1.Y = Radius1 * GMath.SinTab(i);
					Pos1.Z = 0;

					Pos2.X = Radius2 * GMath.CosTab(i);
					Pos2.Y = Radius2 * GMath.SinTab(i);
					Pos2.Z = 0;

					Pos1 = OriginMat.TransformFVector(Pos1);
					Pos2 = OriginMat.TransformFVector(Pos2);

					Lines.AddItem(ULineBatchComponent::FLine(Pos1, Pos2, Color, LineBatch->DefaultLifeTime, 1, SDPG_World));
				}
			}
			else if (AbilityObject->SpellCommonAttribute.RangeType == E_sprt_Rectangle)
			{
				FLOAT Radius1 = AbilityObject->SpellCommonAttribute.RangeParam1;	// chang
				FLOAT Radius2 = AbilityObject->SpellCommonAttribute.RangeParam2;
				for (FLOAT i = -Radius2 / 2.0; i < Radius2 / 2.0; i += 10)
				{
					Pos1.X = 0;
					Pos1.Y = i;
					Pos1.Z = 0;

					Pos2.X = Radius1;
					Pos2.Y = i;
					Pos2.Z = 0;

					Pos1 = OriginMat.TransformFVector(Pos1);
					Pos2 = OriginMat.TransformFVector(Pos2);

					Lines.AddItem(ULineBatchComponent::FLine(Pos1, Pos2, Color, LineBatch->DefaultLifeTime, 1, SDPG_World));
				}
			}

			LineBatch->DrawLines(Lines);
			LineBatch->BeginDeferredReattach();
		}
	}

}


void EditorAbilityComponent::Init(UAbilityComponent* InData, WxAbilityViewer* InViewer)
{
	check(InData);
	check(InViewer);
	Viewer = InViewer;
	TemplateData = InData;
	bEnd = FALSE;
	LastTime = 0.f;
}

void EditorAbilityComponent::Tick(FLOAT Delta)
{
	if (bEnd)
	{
		return;
	}

	LastTime += Delta;
	if (TemplateData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime > TemplateData->Duration)
		{
			OnAction(FALSE);
		}
	}
}

void EditorAbilityComponent::OnAction(UBOOL bOn)
{
	bEnd = !bOn;
}

USkeletalMeshComponent* EditorAbilityComponent::GetBaseMesh()
{
	if (TemplateData->bInHitPoint)
	{
		return Viewer->PreviewSkelCompTarget;
	}
	else
	{
		return Viewer->PreviewSkelComp;
	}
}

void EditorAbilityComponent::OnStateEnd()
{
	if (TemplateData && TemplateData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnAction(FALSE);
	}
}

void EditorAbilityComponent::OnAbilityEnd()
{
	if (TemplateData->StopType == ACBT_StopWhenAbilityEnd)
	{
		OnAction(FALSE);
	}
}

void EditorMissleComponent::ReturnComponentToPool()
{
	if (DynamicComponent)
	{
		DynamicComponent->DeactivateSystem();
		DynamicComponent->KillParticlesForced();
		Viewer->ReturnParticleSystemComponentToPool(DynamicComponent);
		DynamicComponent = NULL;
	}
}

void EditorMissleComponent::Tick(FLOAT DeltaTime)
{
	if (LastTime > Duration)
	{
		return;
	}

	if (DynamicComponent)
	{
		DynamicComponent->Tick(DeltaTime);
	}

	LastTime += DeltaTime;
	if (DynamicComponent)
	{
		FVector NewLocation = DynamicComponent->CachedParentToWorld.GetOrigin();
		NewLocation += Speed * DeltaTime;
		FRotator NewRotation = DynamicComponent->CachedParentToWorld.Rotator();
		FRotationTranslationMatrix rtm(NewRotation, NewLocation);
		DynamicComponent->ConditionalUpdateTransform(rtm);
	}
}


////////////////////////// materail effect preview    ///////////////////////////////////////////////

void WxAbilityViewer::OnStartComponentMaterial(UAbilityComp_MaterialEffect* InComponent, UBOOL bTarget)
{
	if (InComponent == NULL)
	{
		return;
	}
	check(AbilityObject);
	EditorMaterialComponent* node = new EditorMaterialComponent;
	node->AbilityData = AbilityObject;
	node->Viewer = this;
	node->MaterialData = InComponent;
	EditorAbilityComponentList.AddItem(node);
	node->OnStart();
}

void EditorMaterialComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}

	LastTime += DeltaTime;

	if (MaterialData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime > MaterialData->Duration)
		{
			OnStop();
		}
	}
}

void EditorMaterialComponent::OnStateEnd()
{
	if (MaterialData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorMaterialComponent::OnAbilityEnd()
{
	if (MaterialData->StopType == ACBT_StopWhenAbilityEnd)
	{
		OnStop();
	}
}

void EditorMaterialComponent::OnStop()
{
	bEnd = TRUE;
	USkeletalMeshComponent* BaseComponent = NULL;
	if (MaterialData->bInHitPoint)
	{
		BaseComponent = Viewer->PreviewSkelCompTarget;
	}
	else
	{
		BaseComponent = Viewer->PreviewMergedComp;
		if (Viewer->PreviewMergedComp == NULL)
		{
			BaseComponent = Viewer->PreviewSkelComp;
		}
	}

	if (BaseComponent == NULL)
	{
		return;
	}

	BaseComponent->CloseEffect(InstId);

}

void EditorMaterialComponent::OnStart()
{
	USkeletalMeshComponent* BaseComponent = NULL;
	if (MaterialData->bInHitPoint)
	{
		BaseComponent = Viewer->PreviewSkelCompTarget;
	}
	else
	{
		BaseComponent = Viewer->PreviewMergedComp;
		if (Viewer->PreviewMergedComp == NULL)
		{
			BaseComponent = Viewer->PreviewSkelComp;
		}
	}

	if (BaseComponent == NULL)
	{
		return;
	}

	BaseComponent->RebulidMeshMaterialList();

	if (BaseComponent)
	{
		if (MaterialData->EffectType == EMET_ChangeMaterialProperty)
		{
			UMaterialInstanceTimeVaryingEffectNode* InData = Viewer->GamePlayData->GetMaterialEffectData(MaterialData->EffectTemplateId);
			if (InData)
			{
				BaseComponent->ChangeMaterialProperties(InstId, InData);
			}
		}
		else if (MaterialData->EffectType == EMET_LoadOpacityMaterial)
		{
			BaseComponent->SwitchToTransparentMaterial(InstId);
		}
		else if (MaterialData->EffectType == EMET_ChangeMaterial)
		{
			BaseComponent->ChangeMaterial(InstId, MaterialData->EffectMaterialInstance, TRUE);
		}
	}
}

void EditorDecalComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}

	LastTime += DeltaTime;

	if (DecalData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime > DecalData->Duration)
		{
			OnStop();
		}
	}

}

void EditorDecalComponent::OnStateEnd()
{
	if (DecalData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorDecalComponent::OnAbilityEnd()
{
	if (DecalData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorDecalComponent::OnStop()
{
	if (DecalComponent)
	{
		Viewer->GetPreviewScene()->RemoveComponent(DecalComponent);
		DecalComponent->MarkPendingKill();
		DecalComponent = NULL;
	}

	bEnd = TRUE;
}

void EditorDecalComponent::OnStart()
{
	USkeletalMeshComponent* BaseComponent = NULL;
	if (DecalData->bInHitPoint)
	{
		BaseComponent = Viewer->PreviewSkelCompTarget;
	}
	else
	{
		BaseComponent = Viewer->PreviewSkelComp;
	}

	if (BaseComponent == NULL)
	{
		return;
	}

	if (DecalData->ImpactDecalMaterials.Num() <= 0)
	{
		return;
	}

	UMaterialInterface* Mat = DecalData->ImpactDecalMaterials(RandHelper(DecalData->ImpactDecalMaterials.Num()));
	if (Mat == NULL)
	{
		return;
	}

	UMaterialInstanceTimeVarying* insMat = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass());
	insMat->SetParent(Mat);
	insMat->SetDuration(DecalData->Duration);
	insMat->SetLinearColorParameterCycleTime(FName(TEXT("DecalAlpha")), DecalData->Duration);

	FLOAT yaw = appFrand() * 65536.f;
	if (!DecalData->bUseRandomRotation)
	{
		yaw = FLOAT(INT(DecalData->Rotation) % 360) * 65536.f / 360.f;
	}
	FRotationTranslationMatrix ftm(FRotator(0, yaw, 0), DecalData->RelativeLocation);
	FMatrix Matx = ftm * BaseComponent->LocalToWorld;

	FVector StartPoint = Matx.GetOrigin();
	FVector EndPoint = StartPoint + FVector(0, 0, -500);
	FCheckResult Ret;
	UBOOL bNotFound = Viewer->EditorFloorComp->LineCheck(Ret, EndPoint, StartPoint, FVector(0, 0, 0), 0);
	if (bNotFound)
	{
		return;
	}

	FLOAT larm = appFrand();
	FLOAT Size = DecalData->ImpactDecalSizeMin * larm + DecalData->ImpactDecalSizeMax * (1 - larm);
	FLOAT SizeX = Size * DecalData->ScaleX;
	FLOAT SizeY = Size * DecalData->ScaleY;

	DecalComponent = ConstructObject<UDecalComponent>(UDecalComponent::StaticClass());

	DecalComponent->Location = Ret.Location;
	DecalComponent->Orientation =  FVector(0, 0, -1).Rotation();   //(-Ret.Normal).Rotation();
	DecalComponent->SetDecalMaterial(insMat);
	DecalComponent->Width = SizeX;
	DecalComponent->Height = SizeY;
	DecalComponent->FarPlane = DecalData->ImpactDecalDepth * 0.5;
	DecalComponent->NearPlane = -DecalComponent->FarPlane;
	DecalComponent->bNoClip = TRUE;
	DecalComponent->DecalRotation = (0xffff - INT(Matx.Rotator().Yaw) & 0xffff) * 360.f / 65536.f;
	DecalComponent->HitComponent = Viewer->EditorFloorComp;
	DecalComponent->bProjectOnTerrain = TRUE;
	DecalComponent->bProjectOnStaticMeshes = TRUE;
	DecalComponent->bProjectOnSkeletalMeshes = FALSE;
	DecalComponent->HitBone = NAME_None;
	DecalComponent->HitNodeIndex = INDEX_NONE;
	DecalComponent->HitLevelIndex = INDEX_NONE;
	DecalComponent->DepthBias = -0.00006;
	DecalComponent->BlendRange = FVector2D(89.5, 180);
	DecalComponent->SetHiddenEditor(FALSE);
	DecalComponent->bMovableDecal = TRUE;

	Viewer->GetPreviewScene()->AddComponent(DecalComponent, Matx);
}

void EditorAbilityMeshComponent::OnStart()
{
	if (MeshData == NULL || MeshData->SkeletalMeshRes == NULL)
	{
		return;
	}

	USkeletalMeshComponent* BaseComponent = NULL;
	if (MeshData->bInHitPoint)
	{
		BaseComponent = Viewer->PreviewSkelCompTarget;
	}
	else
	{
		BaseComponent = Viewer->PreviewSkelComp;
	}

	if (BaseComponent == NULL)
	{
		return;
	}


	MeshComponent = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass());
	MeshComponent->SetSkeletalMesh(MeshData->SkeletalMeshRes);
	MeshComponent->SetAnimTreeTemplate(MeshData->AnimTreeTemplate);
	MeshComponent->AnimSets.AddItem(MeshData->AnimSet);
	MeshComponent->Scale = MeshData->Scale;

	UAnimNodeBlend* ActionLoopBlendNode = Cast<UAnimNodeBlend>(MeshComponent->FindAnimNode(TEXT("WeaponActionLoopBlendNode")));
	if (!ActionLoopBlendNode)
	{
		OnStop();
		return;
	}

	UAnimNodeSequence* ActionNode = Cast<UAnimNodeSequence>(MeshComponent->FindAnimNode(TEXT("Action")));
	if (!ActionNode)
	{
		OnStop();
		return;
	}

	ActionLoopBlendNode->SetBlendTarget(0.f, 0.f);
	FLOAT StartTime = FLOAT(MeshData->StartFrame) / 30.f;
	ActionNode->SetAnim(MeshData->AnimName);
	ActionNode->PlayAnim(FALSE, MeshData->Rate, StartTime);

	//if (MeshData->OverridenMaterialInstance)
	//{
	//	MeshComponent->SetMaterial(0, MeshData->OverridenMaterialInstance);
	//}

	for (INT i = 0; i < MeshData->Materials.Num(); ++i)
	{
		if (MeshData->Materials(i))
		{
			UMaterialInstanceTimeVarying* mat = Cast<UMaterialInstanceTimeVarying>(MeshData->Materials(i));
			if (mat)
			{
				mat->SetAllParametersStartTime(0.f);
			}
		}
		MeshComponent->SetMaterial(i, MeshData->Materials(i));
	}


	FMatrix SpawnMat = MeshData->GetSpawnMatrix(
		BaseComponent, 
		Viewer->PreviewWeaponList.Num() >= 1 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(0)) : NULL,
		Viewer->PreviewWeaponList.Num() >= 2 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(1)) : NULL,
		Viewer->TargetLocalToWorld.GetOrigin(),
		MeshData->AttachSocketName,
		MeshData->SourceType, 
		MeshData->RelativeLocation, 
		MeshData->RelativeRotation
		);

	Viewer->GetPreviewScene()->AddComponent(MeshComponent, SpawnMat);
}

void EditorAbilityMeshComponent::OnStop()
{
	bEnd = TRUE;
	if (MeshComponent)
	{
		Viewer->GetPreviewScene()->RemoveComponent(MeshComponent);
		MeshComponent = NULL;
	}
}

void EditorAbilityMeshComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}

	LastTime += DeltaTime;

	if (MeshData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime >= MeshData->Duration)
		{
			OnStop();
			return;
		}
	}


	if (!MeshComponent)
	{
		return;
	}

	UBOOL bAttach = (MeshData->bAttach && MeshData->SourceType != EST_TargetPoint);
	if (bAttach)
	{
		USkeletalMeshComponent* BaseComponent = NULL;
		if (MeshData->bInHitPoint)
		{
			BaseComponent = Viewer->PreviewSkelCompTarget;
		}
		else
		{
			BaseComponent = Viewer->PreviewSkelComp;
		}

		if (BaseComponent == NULL)
		{
			return;
		}

		FMatrix SpawnMat = MeshData->GetSpawnMatrix(
			BaseComponent, 
			Viewer->PreviewWeaponList.Num() >= 1 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(0)) : NULL,
			Viewer->PreviewWeaponList.Num() >= 2 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(1)) : NULL,
			Viewer->TargetLocalToWorld.GetOrigin(),
			MeshData->AttachSocketName,
			MeshData->SourceType, 
			MeshData->RelativeLocation, 
			MeshData->RelativeRotation
			);

		MeshComponent->ConditionalUpdateTransform(SpawnMat);
	}

	MeshComponent->TickAnimNodes(DeltaTime);
	MeshComponent->UpdateSkelPose(DeltaTime);
	MeshComponent->ConditionalUpdateTransform(MeshComponent->CachedParentToWorld);

}
void EditorAbilityMeshComponent::OnStateEnd()
{
	if (MeshData && MeshData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorAbilityMeshComponent::OnAbilityEnd()
{
	if (MeshData && MeshData->StopType == ACBT_StopWhenAbilityEnd)
	{
		OnStop();
	}
}

void EditorStaticMeshComponent::Init(UAbilityComponent* InData, WxAbilityViewer* InViewer)
{
	__super::Init(InData, InViewer);

	MeshData = Cast<UAbilityStaticMeshComponent>(InData);
}

void EditorStaticMeshComponent::Tick(FLOAT DeltaTime)
{
	__super::Tick(DeltaTime);

	if (bEnd)
	{
		return;
	}

	if (!MeshComponent)
	{
		return;
	}

	FMatrix ToUseMatrix = FMatrix::Identity;

	UBOOL bAttach = (MeshData->bAttach && MeshData->SourceType != EST_TargetPoint);
	if (bAttach)
	{
		USkeletalMeshComponent* BaseComponent = GetBaseMesh();

		if (BaseComponent == NULL)
		{
			return;
		}

		FMatrix SpawnMat = MeshData->GetSpawnMatrix(
			BaseComponent, 
			Viewer->PreviewWeaponList.Num() >= 1 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(0)) : NULL,
			Viewer->PreviewWeaponList.Num() >= 2 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(1)) : NULL,
			Viewer->TargetLocalToWorld.GetOrigin(),
			MeshData->AttachSocketName,
			MeshData->SourceType, 
			MeshData->RelativeLocation, 
			MeshData->RelativeRotation
			);

		ToUseMatrix = SpawnMat;
	}
	else
	{
		ToUseMatrix = MeshComponent->CachedParentToWorld;

		if (LastTime < MeshData->MoveTime)
		{
			ToUseMatrix = FTranslationMatrix(MeshData->MoveSpeed * DeltaTime) * ToUseMatrix;
		}
	}

	ToUseMatrix.RemoveScaling();

	if (LastTime < MeshData->RotatoTime)
	{
		ToUseMatrix = FRotationMatrix(MeshData->RotatoSpeed * DeltaTime) * ToUseMatrix;
	}

	if (LastTime < MeshData->ScaleTime)
	{
		FVector NewScale = MeshComponent->Scale3D + MeshData->ScaleSpeed * DeltaTime;
		MeshComponent->Scale3D = NewScale;
	}

	MeshComponent->ConditionalUpdateTransform(ToUseMatrix);
	

}


void EditorStaticMeshComponent::OnAction(UBOOL bOn)
{
	__super::OnAction(bOn);
	
	if (bOn)
	{
		if (MeshData == NULL || MeshData->MeshRes == NULL)
		{
			return;
		}

		USkeletalMeshComponent* BaseComponent = GetBaseMesh();

		if (BaseComponent == NULL)
		{
			return;
		}


		MeshComponent = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass());
		MeshComponent->SetStaticMesh(MeshData->MeshRes);

		MeshComponent->Scale = MeshData->Scale;

		for (INT i = 0; i < MeshData->OverrideMaterials.Num(); ++i)
		{
			if (MeshData->OverrideMaterials(i))
			{
				UMaterialInstanceTimeVarying* mat = Cast<UMaterialInstanceTimeVarying>(MeshData->OverrideMaterials(i));
				if (mat)
				{
					mat->SetAllParametersStartTime(0.f);
				}
				MeshComponent->SetMaterial(i, MeshData->OverrideMaterials(i));
			}
		}


		FMatrix SpawnMat = MeshData->GetSpawnMatrix(
			BaseComponent, 
			Viewer->PreviewWeaponList.Num() >= 1 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(0)) : NULL,
			Viewer->PreviewWeaponList.Num() >= 2 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(1)) : NULL,
			Viewer->TargetLocalToWorld.GetOrigin(),
			MeshData->AttachSocketName,
			MeshData->SourceType, 
			MeshData->RelativeLocation, 
			MeshData->RelativeRotation
			);

		SpawnMat.RemoveScaling();

		Viewer->GetPreviewScene()->AddComponent(MeshComponent, SpawnMat);
		MeshComponent->ConditionalUpdateTransform();

		INT Num = MeshComponent->GetNumElements();
		for (INT i = 0; i < Num; ++i)
		{
			UMaterialInstance* umi = Cast<UMaterialInstance>(MeshComponent->GetMaterial(i));
			if (umi)
			{
				UMaterialInstance* ToUseMat = umi;
				if (MeshData->bUseForShowEffectArea)
				{
					if (umi->IsA(UMaterialInstanceTimeVarying::StaticClass()))
					{
						ToUseMat = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass());
					}
					else
					{
						ToUseMat = ConstructObject<UMaterialInstanceConstant>(UMaterialInstanceConstant::StaticClass());
					}

					ToUseMat->SetParent(umi);

					if (MeshData->Parameter1Name != NAME_None)
					{
						ToUseMat->SetScalarParameterValue(MeshData->Parameter1Name, MeshData->Parameter1Value);
					}

					FMatrix WorldToLocal = MeshComponent->LocalToWorld.Inverse();
					FBoxSphereBounds OriginalBound = MeshComponent->Bounds.TransformBy(WorldToLocal);
					FLOAT Radius = OriginalBound.SphereRadius * 0.70710678;
					if (Radius <= 0)
					{
						Radius = 1;
					}
					ToUseMat->SetScalarParameterValue(FName(TEXT("Radius")), Radius);

					if (MeshData->Parameter2Name != NAME_None)
					{
						FLOAT Parameter2Value = MeshData->Parameter2Value;

						FVector ScaleVector = WorldToLocal.GetScaleVector();
						Parameter2Value = Parameter2Value * abs(ScaleVector.Y);

						if (MeshData->Parameter1Value <1)
						{
							Parameter2Value = Parameter2Value / Radius;
						}
						else if (MeshData->Parameter1Value < 2)
						{

						}
						else if (MeshData->Parameter1Value < 3)
						{
							Parameter2Value = Parameter2Value / Radius;
						}
						else if (MeshData->Parameter1Value < 4)
						{
							Parameter2Value = MeshData->Parameter2Value * 3.1415926 * 2.0 / 360.0;
						}
						ToUseMat->SetScalarParameterValue(MeshData->Parameter2Name, Parameter2Value);
					}
				}
				MeshComponent->SetMaterial(i, ToUseMat);
			}
		}
	}
	else
	{
		if (MeshComponent)
		{
			Viewer->GetPreviewScene()->RemoveComponent(MeshComponent);
			MeshComponent = NULL;
		}
	}
}

void EditorPostProcessComponent::OnStart()
{
	if (PostProcessData == NULL || PostProcessData->PostEffect == NULL)
	{
		return;
	}

	Viewer->GetPreviewVC()->PreviewPostProcessChain->Effects.AddItem(PostProcessData->PostEffect);
}

void EditorPostProcessComponent::OnStop()
{
	bEnd = TRUE;
	Viewer->GetPreviewVC()->PreviewPostProcessChain->Effects.RemoveItem(PostProcessData->PostEffect);
}

void EditorPostProcessComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}

	LastTime += DeltaTime;

	if (PostProcessData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime >= PostProcessData->Duration)
		{
			OnStop();
			return;
		}
	}
}
void EditorPostProcessComponent::OnStateEnd()
{
	if (PostProcessData && PostProcessData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorPostProcessComponent::OnAbilityEnd()
{
	if (PostProcessData && PostProcessData->StopType == ACBT_StopWhenAbilityEnd)
	{
		OnStop();
	}
}

void EditorActionsComponent::OnStart()
{
	if (ActionsData == NULL)
	{
		return;
	}
}

void EditorActionsComponent::OnStop()
{
	bEnd = TRUE;
}

void EditorActionsComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}

	FLOAT NewTime = LastTime + DeltaTime;

	for (INT i = 0; i < ActionsData->ActionsComponentList.Num(); ++i)
	{
		if (ActionsData->ActionsComponentList(i).Component == NULL)
		{
			continue;
		}

		if (LastTime <= ActionsData->ActionsComponentList(i).StartTime && ActionsData->ActionsComponentList(i).StartTime < NewTime)
		{
			Viewer->OnStartComponent(ActionsData->ActionsComponentList(i).Component, LastTime, ActionsData->bInHitPoint);
		}
	}

	LastTime = NewTime;

	if (ActionsData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime >= ActionsData->Duration)
		{
			OnStop();
			return;
		}
	}
}
void EditorActionsComponent::OnStateEnd()
{
	if (ActionsData && ActionsData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorActionsComponent::OnAbilityEnd()
{
	if (ActionsData && ActionsData->StopType == ACBT_StopWhenAbilityEnd)
	{
		OnStop();
	}
}


//

void EditorVisibleComponent::OnStart()
{
	if (VisibleData == NULL)
	{
		return;
	}

	Viewer->SetVisible(!VisibleData->bInHitPoint, FALSE);
}

void EditorVisibleComponent::OnStop()
{
	bEnd = TRUE;
	Viewer->SetVisible(!VisibleData->bInHitPoint, TRUE);
}

void EditorVisibleComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}
	LastTime += DeltaTime;

	if (VisibleData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime >= VisibleData->Duration)
		{
			OnStop();
			return;
		}
	}
}
void EditorVisibleComponent::OnStateEnd()
{
	if (VisibleData && VisibleData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorVisibleComponent::OnAbilityEnd()
{
	if (VisibleData && VisibleData->StopType == ACBT_StopWhenAbilityEnd)
	{
		OnStop();
	}
}



//
void EditorScaleComponent::OnStart()
{
	if (ScaleData == NULL)
	{
		return;
	}

	Viewer->SetScale(!ScaleData->bInHitPoint, ScaleData->Scale3D);
}

void EditorScaleComponent::OnStop()
{
	bEnd = TRUE;
	Viewer->SetScale(!ScaleData->bInHitPoint, FVector(1, 1, 1));
}

void EditorScaleComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}
	LastTime += DeltaTime;

	if (ScaleData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime >= ScaleData->Duration)
		{
			OnStop();
			return;
		}
	}
}
void EditorScaleComponent::OnStateEnd()
{
	if (ScaleData && ScaleData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorScaleComponent::OnAbilityEnd()
{
	if (ScaleData && ScaleData->StopType == ACBT_StopWhenAbilityEnd)
	{
		OnStop();
	}
}


//

void EditorPhysicSimulateComponent::OnStart()
{
	if (PhysicSimulateData == NULL)
	{
		return;
	}
	USkeletalMeshComponent* SimulateMesh = NULL;

	if (PhysicSimulateData->bInHitPoint)
	{
		SimulateMesh = Viewer->PreviewSkelCompTarget;
	}
	else
	{
		SimulateMesh = Viewer->PreviewSkelComp;
	}

	if (SimulateMesh->PhysicsAsset == NULL)
	{
		return;
	}

	if (SimulateMesh->PhysicsAssetInstance == NULL)
	{
		SimulateMesh->SetHasPhysicsAssetInstance(TRUE, TRUE);
	}

	if (SimulateMesh->PhysicsAssetInstance == NULL)
	{
		return;
	}

	SimulateMesh->PhysicsWeight = PhysicSimulateData->PhysicWeight;

	if (PhysicSimulateData->Type == SimulateType_Ragdoll)
	{
		SimulateMesh->PhysicsAssetInstance->SetAllBodiesFixed(FALSE);
		SimulateMesh->WakeRigidBody();
	}
	else if (PhysicSimulateData->Type == SimulateType_Fully)
	{
		SimulateMesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
		FName RootName = (SimulateMesh->SkeletalMesh && SimulateMesh->SkeletalMesh->RefSkeleton.IsValidIndex(0) ? SimulateMesh->SkeletalMesh->RefSkeleton(0).Name : NAME_None);
		SimulateMesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(RootName, SimulateMesh->PhysicsAsset, SimulateMesh, TRUE);
	}
	else if (PhysicSimulateData->Type == SimulateType_PartialSimulate)
	{
		//if (BaseActor->Mesh->PhysicsAssetInstance != NULL)
		//{
		//	BaseActor->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
		//	for (INT i = 0; i < BaseActor->PawnAnimInfo.PhysicSimulateRootBones.Num(); ++i)
		//	{
		//		BaseActor->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(BaseActor->PawnAnimInfo.PhysicSimulateRootBones(i), BaseActor->Mesh->PhysicsAsset, BaseActor->Mesh, TRUE);
		//	}
		//}
	}
	else if (PhysicSimulateData->Type == SimulateType_CustomSimulate)
	{
		SimulateMesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
		for (INT i = 0; i < PhysicSimulateData->SimulateRootBones.Num(); ++i)
		{
			SimulateMesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(PhysicSimulateData->SimulateRootBones(i), SimulateMesh->PhysicsAsset, SimulateMesh, TRUE);
		}
	}


}

void EditorPhysicSimulateComponent::OnStop()
{
	bEnd = TRUE;

	if (PhysicSimulateData == NULL)
	{
		return;
	}
	USkeletalMeshComponent* SimulateMesh = NULL;

	if (PhysicSimulateData->bInHitPoint)
	{
		SimulateMesh = Viewer->PreviewSkelCompTarget;
	}
	else
	{
		SimulateMesh = Viewer->PreviewSkelComp;
	}

	if (SimulateMesh->PhysicsAsset == NULL)
	{
		return;
	}

	if (SimulateMesh->PhysicsAssetInstance == NULL)
	{
		return;
	}

	SimulateMesh->PhysicsWeight = 0.f;

	if (PhysicSimulateData->Type == SimulateType_Ragdoll)
	{
		SimulateMesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
		SimulateMesh->PutRigidBodyToSleep();
	}
	else if (PhysicSimulateData->Type == SimulateType_Fully)
	{
		SimulateMesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
		FName RootName = (SimulateMesh->SkeletalMesh && SimulateMesh->SkeletalMesh->RefSkeleton.IsValidIndex(0) ? SimulateMesh->SkeletalMesh->RefSkeleton(0).Name : NAME_None);
		SimulateMesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(RootName, SimulateMesh->PhysicsAsset, SimulateMesh, TRUE);
	}
	else if (PhysicSimulateData->Type == SimulateType_PartialSimulate)
	{
		//if (BaseActor->Mesh->PhysicsAssetInstance != NULL)
		//{
		//	BaseActor->Mesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
		//	for (INT i = 0; i < BaseActor->PawnAnimInfo.PhysicSimulateRootBones.Num(); ++i)
		//	{
		//		BaseActor->Mesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(BaseActor->PawnAnimInfo.PhysicSimulateRootBones(i), BaseActor->Mesh->PhysicsAsset, BaseActor->Mesh, TRUE);
		//	}
		//}
	}
	else if (PhysicSimulateData->Type == SimulateType_CustomSimulate)
	{
		SimulateMesh->PhysicsAssetInstance->SetAllBodiesFixed(TRUE);
		for (INT i = 0; i < PhysicSimulateData->SimulateRootBones.Num(); ++i)
		{
			SimulateMesh->PhysicsAssetInstance->ForceAllBodiesBelowUnfixed(PhysicSimulateData->SimulateRootBones(i), SimulateMesh->PhysicsAsset, SimulateMesh, TRUE);
		}
	}
}

void EditorPhysicSimulateComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}
	LastTime += DeltaTime;

	if (PhysicSimulateData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime >= PhysicSimulateData->Duration)
		{
			OnStop();
			return;
		}
	}
}
void EditorPhysicSimulateComponent::OnStateEnd()
{
	if (PhysicSimulateData && PhysicSimulateData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorPhysicSimulateComponent::OnAbilityEnd()
{
	if (PhysicSimulateData && PhysicSimulateData->StopType == ACBT_StopWhenAbilityEnd)
	{
		OnStop();
	}
}




//
void EditorThrowComponent::Init(UAbilityComponent* InData, WxAbilityViewer* InViewer)
{
	EditorAbilityComponent::Init(InData, InViewer);

	ThrowData = Cast<UAbilityThrowComponent>(InData);
}


void EditorThrowComponent::Tick(FLOAT DeltaTime)
{
	EditorAbilityComponent::Tick(DeltaTime);

	if (!bEnd)
	{
	   ULineBatchComponent* lbc = Viewer->GetLineBatchComponent();
	   check(lbc);

	   UMeshComponent* WeaponMesh1 = (Viewer->PreviewWeaponList.Num() >= 1 ? Viewer->PreviewWeaponList(0) : NULL);
	   UMeshComponent* WeaponMesh2 = (Viewer->PreviewWeaponList.Num() >= 2 ? Viewer->PreviewWeaponList(1) : NULL);

	   FMatrix NewMat = ThrowData->GetSpawnMatrix(Viewer->PreviewSkelComp, Cast<USkeletalMeshComponent>(WeaponMesh1), Cast<USkeletalMeshComponent>(WeaponMesh2), Viewer->TargetLocalToWorld.GetOrigin(), ThrowData->AttachSocketName, ThrowData->SourceType, ThrowData->RelativeLocation, ThrowData->RelativeRotation);
	   FVector NewPoint = NewMat.GetOrigin();
	   lbc->DrawLine(LastPoint, NewPoint, FLinearColor(1.0, 1.0, 1.0), SDPG_World, 1.0);
	   LastPoint = NewPoint;

	   lbc->BeginDeferredReattach();
	}
}

void EditorThrowComponent::OnAction(UBOOL bOn)
{
	EditorAbilityComponent::OnAction(bOn);

	ULineBatchComponent* lbc = Viewer->GetLineBatchComponent();
	check(lbc);

	UMeshComponent* WeaponMesh1 = (Viewer->PreviewWeaponList.Num() >= 1 ? Viewer->PreviewWeaponList(0) : NULL);
	UMeshComponent* WeaponMesh2 = (Viewer->PreviewWeaponList.Num() >= 2 ? Viewer->PreviewWeaponList(1) : NULL);

	FMatrix NewMat = ThrowData->GetSpawnMatrix(Viewer->PreviewSkelComp, Cast<USkeletalMeshComponent>(WeaponMesh1), Cast<USkeletalMeshComponent>(WeaponMesh2), Viewer->TargetLocalToWorld.GetOrigin(), ThrowData->AttachSocketName, ThrowData->SourceType, ThrowData->RelativeLocation, ThrowData->RelativeRotation);

	if (bOn)
	{
		LastPoint = NewMat.GetOrigin();
	}
	else
	{
		FVector Des = Viewer->PreviewSkelComp->LocalToWorld.Rotator().Vector().SafeNormal();
		Des = Des * Viewer->AbilityObject->PreviewResource->ThrowDistance;
		Des += Viewer->PreviewSkelComp->LocalToWorld.GetOrigin();
		FMatrix BaseMat = FRotationTranslationMatrix(Viewer->PreviewSkelComp->LocalToWorld.Rotator(), NewMat.GetOrigin());
		FVector P0 = FVector(0, 0, 0);
		FVector P1 = BaseMat.InverseTransformFVector(Des);
		FVector T0 = ThrowData->T0;
		FVector T1 = P1 + ThrowData->T1;

		for (INT i = 0; i < 100; ++i)
		{
			FLOAT ct = FLOAT(i) / 100.f;
			FVector NewPointTemp = CubicInterp<FVector, FLOAT>(P0, T0, P1, T1, ct);
			FVector NewPoint = BaseMat.TransformFVector(NewPointTemp);
			lbc->DrawLine(LastPoint, NewPoint, FLinearColor(1.0, 0.0, 1.0), SDPG_World, 1.0);
			LastPoint = NewPoint;
		}
		lbc->BeginDeferredReattach();
	}
}



//

void EditorMirrorComponent::Init(UAbilityComponent* InData, WxAbilityViewer* InViewer)
{
	EditorAbilityComponent::Init(InData, InViewer);
	MirrorData = Cast<UAbilityMirrorComponent>(InData);
}

void EditorMirrorComponent::Tick(FLOAT DeltaTime)
{
	EditorAbilityComponent::Tick(DeltaTime);

	if (bEnd)
	{
		return;
	}

	NextCreateTime -= DeltaTime;
	if (NextCreateTime <= 0.f)
	{
		INT NewMirrorId = SpawnMirror();
		MirrorIdList.push_back(NewMirrorId);
		if (MirrorIdList.size() > size_t(MirrorData->MaxCount))
		{
			ClearMirror(MirrorIdList.front());
			MirrorIdList.pop_front();
		}
		NextCreateTime = MirrorData->Interval;
	}
}

void EditorMirrorComponent::OnAction(UBOOL bOn)
{
	EditorAbilityComponent::OnAction(bOn);

	if (bOn)
	{
		Tick(0.001f);
	}
	else
	{
	}
}

INT EditorMirrorComponent::SpawnMirror()
{
	USkeletalMeshComponent* BaseMesh = GetBaseMesh();
	if (BaseMesh == NULL)
	{
		return 0;
	}

	USkeletalMeshComponent* NewMesh = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass(), BaseMesh);
	check(NewMesh);
	NewMesh->SetSkeletalMesh(BaseMesh->SkeletalMesh);
	NewMesh->SetParentAnimComponent(BaseMesh);
	NewMesh->ForceSkelUpdate();

	Viewer->GetPreviewScene()->AddComponent(NewMesh, BaseMesh->LocalToWorld);

	UMaterialInstanceTimeVarying* MaterialToUseTemplate = MirrorData->DefaultMaterial;
	if (MirrorData->CustomMaterial)
	{
		MaterialToUseTemplate = MirrorData->CustomMaterial;
	}

	UMaterialInstanceTimeVarying* MaterialToUse = ConstructObject<UMaterialInstanceTimeVarying>(UMaterialInstanceTimeVarying::StaticClass(), NewMesh);
	if (!MaterialToUse)
	{
		return 0;
	}

	MaterialToUse->SetParent(MaterialToUseTemplate);
	//MaterialToUse->SetScalarParameterValue(FName(TEXT("AlphaBlend")), InLiftSpan);
	MaterialToUse->ResetScalarStartTime(FName(TEXT("AlphaBlend")), 0.f);
	MaterialToUse->SetLinearColorParameterValue(FName(TEXT("Color")), MirrorData->ColorValue);

	for (INT i = 0; i < BaseMesh->Attachments.Num(); ++i)
	{
		if (BaseMesh->Attachments(i).Component)
		{
			if (BaseMesh->Attachments(i).Component->IsA(USkeletalMeshComponent::StaticClass()))
			{
				//todo
			}
			else if (BaseMesh->Attachments(i).Component->IsA(UStaticMeshComponent::StaticClass()))
			{
				UStaticMeshComponent* TempateMesh = (UStaticMeshComponent*)(BaseMesh->Attachments(i).Component);
				UStaticMeshComponent* NewComponent =ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), NewMesh);
				if (!NewComponent)
				{
					continue;
				}

				NewComponent->SetStaticMesh(TempateMesh->StaticMesh);

				INT MatNum = NewComponent->GetNumElements();
				for (INT j = 0; j < MatNum; ++j)
				{
					NewComponent->SetMaterial(j, MaterialToUse);
				}

				NewMesh->AttachComponentToSocket(NewComponent, BaseMesh->Attachments(i).BoneName);
			}
		}
	}


	TArray<USkeletalMeshComponent*> MeshList;

	for (INT i = 0; i < Viewer->OtherSkeletalMeshComponentList.Num(); ++i)
	{
		if (Viewer->OtherSkeletalMeshComponentList(i) == NULL)
		{
			continue;
		}

		if (Viewer->OtherSkeletalMeshComponentList(i) == BaseMesh)
		{
			continue;
		}

		USkeletalMeshComponent* TemplateMesh = Cast<USkeletalMeshComponent>(Viewer->OtherSkeletalMeshComponentList(i));
		if (TemplateMesh == NULL)
		{
			continue;
		}

		USkeletalMeshComponent* NewComponent = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass(), NewMesh);
		if (NewComponent == NULL)
		{
			continue;
		}

		NewComponent->SetSkeletalMesh(TemplateMesh->SkeletalMesh);
		INT MatNum = NewComponent->GetNumElements();
		for (INT j = 0; j < MatNum; ++j)
		{
			NewComponent->SetMaterial(j, MaterialToUse);
		}
		NewComponent->SetParentAnimComponent(BaseMesh);
		NewComponent->ForceSkelUpdate();
		Viewer->GetPreviewScene()->AddComponent(NewComponent, BaseMesh->LocalToWorld);
		MeshList.AddItem(NewComponent);
	}



	NewMesh->BeginDeferredReattach();

	//NewActor->SetTickIsDisabled(TRUE);

	INT idx = Viewer->EditorMirrorList.AddZeroed();
	Viewer->EditorMirrorList(idx).LiftSpan = MirrorData->MirrorLifeSpan;
	Viewer->EditorMirrorList(idx).MeshList.AddItem(NewMesh);
	if (MeshList.Num() != 0)
	{
		Viewer->EditorMirrorList(idx).MeshList.Append(MeshList);
	}

	return idx;
}

void EditorMirrorComponent::ClearMirror(INT InIdx)
{
	Viewer->ClearMirror(InIdx);
}



//

void EditorPositionTransformComponent::OnStart()
{
	if (PositionTransformData == NULL)
	{
		return;
	}
	SimulateMesh = NULL;
	if (PositionTransformData->bInHitPoint)
	{
		SimulateMesh = Viewer->PreviewSkelCompTarget;
	}
	else
	{
		SimulateMesh = Viewer->PreviewSkelComp;
	}

	if (!SimulateMesh)
	{
		return;
	}

	if (PositionTransformData->Values.Points.Num() == 0)
	{
		return;
	}

	StartMatrix = SimulateMesh->CachedParentToWorld;
}

void EditorPositionTransformComponent::OnStop()
{
	bEnd = TRUE;

	if (PositionTransformData == NULL)
	{
		return;
	}

	if (!SimulateMesh)
	{
		return;
	}

	if (PositionTransformData->EndPositionType == EndPositionType_CurrentPoint)
	{

	}
	else if (PositionTransformData->EndPositionType == EndPositionType_StartPoint)
	{
		SimulateMesh->ConditionalUpdateTransform(StartMatrix);
	}
	else if (PositionTransformData->EndPositionType == EndPositionType_EndPoint)
	{
		FVector EndPoint = PositionTransformData->Values.Points.Last().OutVal;
		FMatrix EndMat = FTranslationMatrix(EndPoint) * StartMatrix;
		SimulateMesh->ConditionalUpdateTransform(EndMat);
	}
}

void EditorPositionTransformComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}

	if (!SimulateMesh)
	{
		return;
	}

	LastTime += DeltaTime;

	FLOAT alp = LastTime / PositionTransformData->Duration;

	if (PositionTransformData->bResizePointTimeToComponentLifespan)
	{
		alp *= PositionTransformData->Values.Points.Last().InVal;
	}
	else
	{
		alp = LastTime;
	}

	FVector NewPos = PositionTransformData->Values.Eval(alp, FVector(0, 0, 0));
	FMatrix NewMat = FTranslationMatrix(NewPos) * StartMatrix;
	SimulateMesh->ConditionalUpdateTransform(NewMat);




	if (PositionTransformData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime >= PositionTransformData->Duration)
		{
			OnStop();
			return;
		}
	}
}
void EditorPositionTransformComponent::OnStateEnd()
{
	if (PositionTransformData && PositionTransformData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorPositionTransformComponent::OnAbilityEnd()
{
	if (PositionTransformData && PositionTransformData->StopType == ACBT_StopWhenAbilityEnd)
	{
		OnStop();
	}
}








void WxAbilityViewer::LoadMissleData()
{
	TArray<FString> list;
	FString dir = TEXT("..\\..\\MMOGame\\Content\\MMOContent\\09_Missile");

	appFindFilesInDirectory(list, *dir, TRUE, FALSE);

	MissileList.Empty();

	for (int i = 0; i < list.Num(); ++i)
	{
		const FString& PackageName = FPackageFileCache::PackageFromPath(*list(i));
		UPackage* Package = FindObject<UPackage>(NULL, *PackageName, TRUE);
		if ( Package != NULL )
		{
			Package->FullyLoad();
		}
		else
		{
			Package = UObject::LoadPackage( NULL, *list(i), LOAD_None );
		}

		if( Package == NULL )
		{
			continue;
		}

		for (INT i = 0; i < Package->NetObjects.Num(); ++i)
		{
			UObject* obj = Package->NetObjects(i);
			UMissile* md = Cast<UMissile>(obj);
			if (md)
			{
				MissileList.AddItem(md);
			}
		}
	}

}

void WxAbilityViewer::AddErrorMessage(FString ErrorMessage)
{

}

extern FParticleDataManager	GParticleDataManager;

void WxAbilityViewer::OnUpdatePreviewComponents(FLOAT DeltaTime)
{
	for (INT i = 0; i < EditorAbilityComponentList.Num(); ++i)
	{
		EditorAbilityComponentList(i)->Tick(DeltaTime);
	}

	GParticleDataManager.UpdateDynamicData();
}

void WxAbilityViewer::OnUpdateMovement(FLOAT DeltaTime)
{
	if (AbilityObject->PreviewResource->bShowMovement)
	{
		FVector NewPos = PreviewSkelComp->LocalToWorld.GetOrigin() + AbilityObject->PreviewResource->Velocity * DeltaTime;
		FMatrix Mat = FRotationTranslationMatrix(PreviewSkelComp->LocalToWorld.Rotator(), NewPos);
		PreviewSkelComp->ConditionalUpdateTransform(Mat);
	}
}

void WxAbilityViewer::UpdateMirrorList(FLOAT DeltaTime)
{
	for (INT i = 0; i < EditorMirrorList.Num(); ++i)
	{
		if (EditorMirrorList(i).MeshList.Num() > 0)
		{
			EditorMirrorList(i).LiftSpan -= DeltaTime;
			if (EditorMirrorList(i).LiftSpan <= 0.f)
			{
				ClearMirror(i);
			}
		}
	}
}

void WxAbilityViewer::ClearMirror(INT InIdx)
{
	if (InIdx >= 0 && InIdx < EditorMirrorList.Num())
	{
		if (EditorMirrorList(InIdx).MeshList.Num() > 0)
		{
			for (INT i = 0; i < EditorMirrorList(InIdx).MeshList.Num(); ++i)
			{
				GetPreviewScene()->RemoveComponent(EditorMirrorList(InIdx).MeshList(i));
			}
			EditorMirrorList(InIdx).MeshList.Empty();
		}
	}
}

void WxAbilityViewer::ClearAllMirror()
{
	for (INT i = 0; i < EditorMirrorList.Num(); ++i)
	{
		if (EditorMirrorList(i).MeshList.Num() > 0)
		{
			ClearMirror(i);
		}
	}

	EditorMirrorList.Empty();

}

void WxAbilityViewer::OnNotifyAbilityStateEnd()
{
	for (INT i = 0; i < EditorAbilityComponentList.Num(); ++i)
	{
		EditorAbilityComponentList(i)->OnStateEnd();
	}
}

void WxAbilityViewer::OnNotifyAbilityEnd()
{
	for (INT i = 0; i < EditorAbilityComponentList.Num(); ++i)
	{
		EditorAbilityComponentList(i)->OnAbilityEnd();
	}
}


void EditorEmitterComponent::ReturnComponentToPool()
{
	if (DynamicComponent)
	{
		Viewer->ParticleSystemComponentPool.AddItem(DynamicComponent);
		DynamicComponent = NULL;
	}

	if (BeamEndDynamicComponent)
	{
		Viewer->ParticleSystemComponentPool.AddItem(BeamEndDynamicComponent);
		BeamEndDynamicComponent = NULL;
	}
}

void EditorEmitterComponent::OnStateEnd()
{
	if (EmitterData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorEmitterComponent::OnAbilityEnd()
{
	if (EmitterData->StopType == ACBT_StopWhenAbilityEnd)
	{
		//bEnd = TRUE;
		OnStop();
	}
}

void EditorEmitterComponent::OnStop()
{
	if (DynamicComponent)
	{
		DynamicComponent->DeactivateSystem();
		DynamicComponent->SetStopSpawning(-1, TRUE);
		if (DynamicComponent->GetMaxLifespan() <= 0.f) // loop emitter
		{
			DynamicComponent->KillParticlesForced();
		}
	}

	if (BeamEndDynamicComponent)
	{
		BeamEndDynamicComponent->DeactivateSystem();
		BeamEndDynamicComponent->SetStopSpawning(-1, TRUE);
		if (DynamicComponent->GetMaxLifespan() <= 0.f) // loop emitter
		{
			DynamicComponent->KillParticlesForced();
		}
	}

	if (PointLightComponent)
	{
		Viewer->GetPreviewScene()->RemoveComponent(PointLightComponent);
		PointLightComponent->MarkPendingKill();
	}

	bEnd = TRUE;

	for (INT i = 0; i < EmitterData->EmitterEndEffectList.Num(); ++i)
	{
		if (EmitterData->EmitterEndEffectList(i))
		{
			Viewer->OnStartComponent(EmitterData->EmitterEndEffectList(i), 0, FALSE);
		}
	}

}

void EditorEmitterComponent::OnStart()
{
	//if (EmitterData->RandParticleSystemTemplates.Num() == 0)
	//{
	//	if (EmitterData->ParticleSystemTemplate == NULL)
	//	{
	//		Viewer->AddErrorMessage(FString(L"No ParticleSystemTemplate!!!"));
	//		OnStop();
	//		return;
	//	}
	//}

	USkeletalMeshComponent* SrcPreviewSkelComp = NULL;
	USkeletalMeshComponent* DstPreviewSkelComp = NULL;
	USkeletalMeshComponent* SurroundSkelComp = NULL;
	USkeletalMeshComponent* SurroundSkelCompTarget = NULL;
	UMeshComponent* WeaponMesh1 = NULL;
	UMeshComponent* WeaponMesh2 = NULL;

	if (EmitterData->bInHitPoint)
	{
		SrcPreviewSkelComp = Viewer->PreviewSkelCompTarget;
		DstPreviewSkelComp = Viewer->PreviewSkelComp;
		SurroundSkelComp = Viewer->PreviewSkelCompTarget;
		SurroundSkelCompTarget = Viewer->PreviewMergedComp;
	}
	else
	{
		SrcPreviewSkelComp = Viewer->PreviewSkelComp;
		DstPreviewSkelComp = Viewer->PreviewSkelCompTarget;
		SurroundSkelComp = Viewer->PreviewMergedComp;
		SurroundSkelCompTarget = Viewer->PreviewSkelCompTarget;

		WeaponMesh1 = (Viewer->PreviewWeaponList.Num() >= 1 ? Viewer->PreviewWeaponList(0) : NULL);
		WeaponMesh2 = (Viewer->PreviewWeaponList.Num() >= 2 ? Viewer->PreviewWeaponList(1) : NULL);
	}

	if (SrcPreviewSkelComp == NULL)
	{
		Viewer->AddErrorMessage(FString(L"1 SrcPreviewSkelComp is null, can not preview"));
		OnStop();
		return;
	}

	UParticleSystemComponent* NewComponent = Viewer->GetParticleSystemComponentFromPool();
	EElementaryType EleType = EETT_Elementary0;
	if (AbilityData->PreviewResource)
	{
		EleType = (EElementaryType)AbilityData->PreviewResource->OverriddenElementaryType;
	}
	UParticleSystem* pt = EmitterData->GetParticleSystemByElementaryType(EleType);

	if (pt == NULL)
	{
		Viewer->AddErrorMessage(FString(L"No ParticleSystemTemplate!!!"));
		OnStop();
		return;
	}

	NewComponent->SetTemplate(pt);
	NewComponent->Scale = EmitterData->Scale;
	NewComponent->BeginDeferredUpdateTransform();
	NewComponent->ActivateSystem(TRUE);

	DynamicComponent = NewComponent;


	SpawnMat = EmitterData->GetSocketMatrix(
		SrcPreviewSkelComp, 
		Viewer->PreviewWeaponList.Num() >= 1 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(0)) : NULL,
		Viewer->PreviewWeaponList.Num() >= 2 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(1)) : NULL,
		Viewer->TargetLocalToWorld.GetOrigin(),
		FALSE
		);

	if (EmitterData->bUseBaseRotation == FALSE)
	{
		SpawnMat = FScaleRotationTranslationMatrix(SpawnMat.GetScaleVector(), EmitterData->RelativeRotation, SpawnMat.GetOrigin());
	}
	

	FPreviewScene* ViewScene = Viewer->GetPreviewScene();
	if (!ViewScene)
	{
		OnStop();
		return;
	}

	// fix spawn mat
	if (EmitterData->EmitterType == EMIT_Surround)
	{
		if (EmitterData->SourceType == EST_BaseActor)
		{
			DynamicComponent->SetMeshComponentParameter(FName(TEXT("VertSurfaceActor")), SurroundSkelComp);
		}
		else if (EmitterData->SourceType == EST_TargetActor)
		{
			DynamicComponent->SetMeshComponentParameter(FName(TEXT("VertSurfaceActor")), SurroundSkelCompTarget);
		}
		else if (EmitterData->SourceType == EST_BaseWeapon1)
		{
			DynamicComponent->SetMeshComponentParameter(FName(TEXT("VertSurfaceActor")), WeaponMesh1);
		}
		else if (EmitterData->SourceType == EST_BaseWeapon2)
		{
			DynamicComponent->SetMeshComponentParameter(FName(TEXT("VertSurfaceActor")), WeaponMesh2);
		}
	}
	else if (EmitterData->EmitterType == EMIT_Rotate)
	{
		FVector SpawnLocation = SpawnMat.GetOrigin(); // + InComponent->RelativeLocation;
		FRotator SpawnRotation = EmitterData->FirstSpawnDirection;
		SpawnLocation += EmitterData->FirstSpawnDirection.Vector().SafeNormal() * EmitterData->RotateRadius;
		FRotationTranslationMatrix rtm(SpawnRotation, SpawnLocation);
		SpawnMat = rtm * SpawnMat;
	}
	else if (EmitterData->EmitterType == EMIT_FlyTo)
	{
		OverwriteTargetType = EmitterData->ProjectileTargetType;
		if (EmitterData->ProjectileTargetType == EPTT_Actor)
		{
			if (bBullet)
			{
				if (DstPreviewSkelComp->SkeletalMesh == NULL)
				{
					OverwriteTargetType = EPTT_UseCasterDirection;
				}
			}
			else
			{
				OverwriteTargetType = EPTT_UseCasterDirection;
			}
		}

		if (OverwriteTargetType == EPTT_UseCasterDirection)
		{
			SpawnMat = FRotationTranslationMatrix(SrcPreviewSkelComp->Rotation, SpawnMat.GetOrigin());
			FRotationMatrix frm(EmitterData->FlyDirection + EmitterData->DirectionDelta * StartTime);
			SpawnMat = frm * SpawnMat;
		}
	}
	else
	{
		// do not need to fix spawn mat
	}

	// fix lifespan
	if (EmitterData->EmitterType == EMIT_FlyTo)
	{
		FVector DstPoint(0, 0, 600);
		if (OverwriteTargetType == EPTT_Actor)
		{
			DstPoint = DstPreviewSkelComp->GetOrigin();
		}
		else if (OverwriteTargetType == EPTT_TargetPoint)
		{
			DstPoint = Viewer->TargetLocalToWorld.GetOrigin();
		}
		else if (OverwriteTargetType == EPTT_UseCasterDirection)
		{
			DstPoint = SpawnMat.GetOrigin() + SpawnMat.Rotator().Vector().SafeNormal() * AbilityData->SpellCommonAttribute.MaxDistance;
		}

		LifeSpan = (DstPoint - SpawnMat.GetOrigin()).Size() / EmitterData->Speed;

		if (EmitterData->TrackTimeType == ETTT_ConstTime)
		{
			LifeSpan = EmitterData->FlyTime;
		}

		if (bBullet)
		{
			LifeSpan = (DstPoint - SpawnMat.GetOrigin()).Size() / AbilityData->SpellCommonAttribute.Speed;
		}
	}
	else
	{
		LifeSpan = EmitterData->Duration;
	}

	if (!EmitterData->bScaleByStandardCollision)
	{
		SpawnMat.ExtractScaling();
	}

	ViewScene->AddComponent(NewComponent, SpawnMat);

	if (EmitterData->EmitterType == EMIT_ConnectTo)
	{
		if (EmitterData->BeamTargetParticleSystemTemplate)
		{
			BeamEndDynamicComponent = Viewer->GetParticleSystemComponentFromPool();
			BeamEndDynamicComponent->SetTemplate(EmitterData->BeamTargetParticleSystemTemplate);
			BeamEndDynamicComponent->Scale = EmitterData->BeamTargetScale;
			BeamEndDynamicComponent->BeginDeferredUpdateTransform();
			ViewScene->AddComponent(BeamEndDynamicComponent, FMatrix::Identity);
		}
	}

	if (EmitterData->MyPointLightComponent)
	{
		PointLightComponent = ConstructObject<UPointLightComponent>(EmitterData->MyPointLightComponent->GetClass(), INVALID_OBJECT, NAME_None, 0, EmitterData->MyPointLightComponent);
		PointLightComponent->Brightness = 0.f;
		ViewScene->AddComponent(PointLightComponent, DynamicComponent->CachedParentToWorld);
	}
}

void EditorEmitterComponent::SetStartTime(FLOAT InStartTime)
{
	StartTime = InStartTime;
}

void EditorEmitterComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}
	check(EmitterData);
	check(DynamicComponent);

	FVector Dir(1, 0, 0);


	USkeletalMeshComponent* SrcPreviewSkelComp = NULL;
	USkeletalMeshComponent* DstPreviewSkelComp = NULL;
	if (EmitterData->bInHitPoint)
	{
		SrcPreviewSkelComp = Viewer->PreviewSkelCompTarget;
		DstPreviewSkelComp = Viewer->PreviewSkelComp;
	}
	else
	{
		SrcPreviewSkelComp = Viewer->PreviewSkelComp;
		DstPreviewSkelComp = Viewer->PreviewSkelCompTarget;
	}

	if (SrcPreviewSkelComp == NULL)
	{
		Viewer->AddErrorMessage(L"SrcPreviewSkelComp is null");
		return;
	}


	if (EmitterData->EmitterType == EMIT_Surround)
	{

	}
	else if (EmitterData->EmitterType == EMIT_Normal)
	{
		if (EmitterData->bAttach)
		{
			FMatrix CurrentMat = EmitterData->GetSocketMatrix(
				SrcPreviewSkelComp, 
				Viewer->PreviewWeaponList.Num() >= 1 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(0)) : NULL,
				Viewer->PreviewWeaponList.Num() >= 2 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(1)) : NULL,
				Viewer->TargetLocalToWorld.GetOrigin(),
				FALSE
				);

			if (EmitterData->bUseBaseRotation == FALSE)
			{
				CurrentMat = FScaleRotationTranslationMatrix(CurrentMat.GetScaleVector(), EmitterData->RelativeRotation, CurrentMat.GetOrigin());
			}

			DynamicComponent->ConditionalUpdateTransform(CurrentMat);
		}
	}
	else if (EmitterData->EmitterType == EMIT_FlyTo)
	{
		FVector DstPoint(0, 0, 100);
		FMatrix DstMat;
		if (OverwriteTargetType == EPTT_Actor)
		{
			DstMat = EmitterData->GetSocketMatrix(
				DstPreviewSkelComp, 
				Viewer->PreviewWeaponList.Num() >= 1 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(0)) : NULL,
				Viewer->PreviewWeaponList.Num() >= 2 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(1)) : NULL,
				Viewer->TargetLocalToWorld.GetOrigin(),
				FALSE
				);
			DstPoint = DstMat.GetOrigin();
		}
		else if (OverwriteTargetType == EPTT_TargetPoint)
		{
			DstPoint = Viewer->TargetLocalToWorld.GetOrigin();
		}
		else// if (EmitterData->ProjectileTargetType == EPTT_UseCasterDirection)
		{
			DstPoint = SpawnMat.GetOrigin() + SpawnMat.Rotator().Vector().SafeNormal() * AbilityData->SpellCommonAttribute.MaxDistance;
		}

		FVector SpawnPoint = SpawnMat.GetOrigin();
		FVector CurPoint = DynamicComponent->CachedParentToWorld.GetOrigin();
		FVector NewPoint = CurPoint;
		FRotator NewRotator = DynamicComponent->CachedParentToWorld.Rotator();
		FLOAT df = LastTime / LifeSpan;
		if (df > 1.0)
		{
			df = 1.0;
		}
		if (EmitterData->TrackLineType == ETLT_Line)
		{
			NewPoint = (DstPoint - SpawnPoint) * df + SpawnPoint;
			NewRotator = (NewPoint - CurPoint).Rotation();
		}
		else if (EmitterData->TrackLineType == ETLT_Bezier)
		{
			FVector BC1(0, 0, 0);
			FVector BC2(0, 0, 0);
			EmitterData->GetBezierControlPoint(BC1, BC2);
			FVector C1 = SpawnMat.TransformFVector(BC1);
			FVector C2 = SpawnMat.TransformFVector(DstPoint - SpawnPoint + BC2);
			NewPoint = CubicInterp<FVector, FLOAT>(SpawnPoint, C1, DstPoint, C2, df);
			NewRotator = (NewPoint - CurPoint).Rotation();
		}
		else if (EmitterData->TrackLineType == ETLT_Parabola)
		{
			NewPoint = (DstPoint - SpawnPoint) * df + SpawnPoint;
			// calculate height
			//y = -k*(x - t*l/2/c)*(x - t*l/2/c) + h; (x < t/c*l)

			FLOAT t = EmitterData->Duration;
			FLOAT c = EmitterData->LoopCount;
			FLOAT h = EmitterData->MaxHeight;
			FLOAT l = EmitterData->TimeToFinishFixRotation;
			FLOAT k = (t * l) / 2.0f / c;
			k = h / k / k;

			FLOAT CycleTime = t / c;
			INT PassedCount = LastTime / CycleTime;
			FLOAT x = LastTime - PassedCount * CycleTime;
			FLOAT z = 0.f;
			if (x <= t * l / c)
			{
				z = x - t * l / 2.f / c;
				z = -k * z * z + h;
			}
			else
			{
				z = 0.f;
			}
			NewPoint.Z += z;

			NewRotator = (NewPoint - CurPoint).Rotation();
		}

		FRotationTranslationMatrix tm(NewRotator, NewPoint);

		DynamicComponent->ConditionalUpdateTransform(tm);

	}
	else if (EmitterData->EmitterType == EMIT_ConnectTo)
	{
		if (EmitterData->bAttach)
		{
			FMatrix CurrentMat = EmitterData->GetSocketMatrix(
				SrcPreviewSkelComp, 
				Viewer->PreviewWeaponList.Num() >= 1 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(0)) : NULL,
				Viewer->PreviewWeaponList.Num() >= 2 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(1)) : NULL,
				Viewer->TargetLocalToWorld.GetOrigin(),
				FALSE
				);;
			DynamicComponent->ConditionalUpdateTransform(CurrentMat);
		}

		FLOAT Dis = 600;
		if (AbilityData && AbilityData->SpellCommonAttribute.MaxDistance > 0)
		{
			Dis = AbilityData->SpellCommonAttribute.MaxDistance;
		}

		FVector EndPoint = DynamicComponent->CachedParentToWorld.GetOrigin() + Dir * Dis;

		DynamicComponent->SetVectorParameter(FName(L"LinkBeamEnd"), EndPoint);

		if (BeamEndDynamicComponent)
		{
			FVector StartPoint = DynamicComponent->CachedParentToWorld.GetOrigin();
			FRotator NewRotator = (StartPoint - EndPoint).SafeNormal().Rotation();
			FRotationTranslationMatrix rtm2(NewRotator, EndPoint);
			BeamEndDynamicComponent->ConditionalUpdateTransform(rtm2);
		}
	}
	else if (EmitterData->EmitterType == EMIT_Rotate)
	{
		FVector Org;
		FMatrix BaseMat = EmitterData->GetSocketMatrix(
			SrcPreviewSkelComp, 
			Viewer->PreviewWeaponList.Num() >= 1 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(0)) : NULL,
			Viewer->PreviewWeaponList.Num() >= 2 ? Cast<USkeletalMeshComponent>(Viewer->PreviewWeaponList(1)) : NULL,
			Viewer->TargetLocalToWorld.GetOrigin(),
			FALSE
			);;

		FMatrix BToAMat = DynamicComponent->CachedParentToWorld * BaseMat.Inverse();
		FRotationMatrix frm(FRotator(EmitterData->RotateVelocity * DeltaTime));
		FMatrix NewMat = BToAMat * frm;
		NewMat = NewMat * BaseMat;

		DynamicComponent->ConditionalUpdateTransform(NewMat);
	}
	else if (EmitterData->EmitterType == EMIT_Surround)
	{

	}

	DynamicComponent->Tick(DeltaTime);

	if (BeamEndDynamicComponent)
	{
		BeamEndDynamicComponent->Tick(DeltaTime);
	}

	if (PointLightComponent)
	{
		FLOAT cb = LastTime / EmitterData->LightFadeUpTime;
		cb = Clamp(cb, 0.f, 1.f);
		if (LastTime <= EmitterData->LightLifeSpan && EmitterData->LightLifeSpan - LastTime <= EmitterData->LightFadeOutTime)
		{
			cb = (EmitterData->LightLifeSpan - LastTime) / EmitterData->LightFadeOutTime;
			cb = Clamp(cb, 0.f, 1.f);
		}
		PointLightComponent->Brightness = cb;
		PointLightComponent->ConditionalUpdateTransform(DynamicComponent->CachedParentToWorld);
	}

	LastTime += DeltaTime;

	if (EmitterData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime > LifeSpan)
		{
			OnStop();
		}
	}

}


void EditorTrailComponent::Tick(FLOAT DeltaTime)
{
	if (bEnd)
	{
		return;
	}

	if (!TrailNotify)
	{
		return;
	}

	UAnimNodeSequence * PreviewAnimNode = Viewer->GetMeshAnimNodeSequence(Viewer->PreviewSkelComp);

	TrailNotify->NotifyTick(PreviewAnimNode, PreviewAnimNode->CurrentTime, Min<FLOAT>(DeltaTime, TrailData->Duration), TrailData->Duration);

	LastTime += DeltaTime;
	if (TrailData->StopType == ACBT_StopWhenConstDurationPass)
	{
		if (LastTime > TrailData->Duration)
		{
			//bEnd = TRUE;
			OnStop();
		}
	}
}
void EditorTrailComponent::OnStateEnd()
{
	if (TrailData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorTrailComponent::OnAbilityEnd()
{
	if (TrailData->StopType == ACBT_StopWhenAbilityStateEnd)
	{
		OnStop();
	}
}

void EditorTrailComponent::OnStop()
{
	bEnd = TRUE;
	if (TrailNotify)
	{
		UAnimNodeSequence * PreviewAnimNode = Viewer->GetMeshAnimNodeSequence(Viewer->PreviewSkelComp);
		TrailNotify->NotifyEnd(PreviewAnimNode, PreviewAnimNode->CurrentTime);
	}
}


UParticleSystemComponent* WxAbilityViewer::GetParticleSystemComponentFromPool()
{
	UParticleSystemComponent* Ret = NULL;
	if (ParticleSystemComponentPool.Num() == 0)
	{
		Ret = ConstructObject<UParticleSystemComponent>(UParticleSystemComponent::StaticClass());
		Ret->AddToRoot();
	}
	else
	{
		Ret = ParticleSystemComponentPool(ParticleSystemComponentPool.Num() - 1);
		ParticleSystemComponentPool.Remove(ParticleSystemComponentPool.Num() - 1, 1);
	}
	return Ret;
}

void WxAbilityViewer::ReturnParticleSystemComponentToPool(UParticleSystemComponent* InComponent)
{
	ParticleSystemComponentPool.AddItem(InComponent);
}

void WxAbilityViewer::DeletePoolComponents()
{
	ClearAllComponents();
	for (INT i = 0; i < ParticleSystemComponentPool.Num(); ++i)
	{
		ParticleSystemComponentPool(i)->RemoveFromRoot();
	}
}

void WxAbilityViewer::OnStartComponentTrail(UAbilityTrailComponent* InComponent)
{
	if (!InComponent)
	{
		return;
	}
	UAnimNodeSequence * PreviewAnimNode = GetMeshAnimNodeSequence(PreviewSkelComp);
	if (PreviewAnimNode->AnimSeq == FALSE)
	{
		return;
	}

	if (PreviewAnimNode->bPlaying == FALSE)
	{
		return;
	}

	FLOAT StartTime = PreviewAnimNode->CurrentTime;
	FLOAT LastTime = StartTime + InComponent->Duration;

	if (bMeshChange)
	{
		InComponent->TrailList.Empty();
		bMeshChange = FALSE;
	}

	UAnimNotify_Trails* TrailNotifyTemplate = InComponent->GetTrailData(PreviewAnimNode);
	if (TrailNotifyTemplate == NULL)
	{
		InComponent->AddNewTrail(PreviewAnimNode, StartTime, LastTime);
		TrailNotifyTemplate = InComponent->GetTrailData(PreviewAnimNode);
	}
	else
	{
		InComponent->RebuildTrail(PreviewAnimNode, StartTime, LastTime);
	}

	if (TrailNotifyTemplate == NULL)
	{
		AddErrorMessage(L"Create TrailNotify Template error");
		return;
	}

	TrailNotifyTemplate->Notify(PreviewAnimNode);

	EditorTrailComponent* etc = new EditorTrailComponent;
	etc->AbilityData = AbilityObject;
	etc->TrailData = InComponent;
	etc->TrailNotify = TrailNotifyTemplate;
	etc->Viewer = this;
	EditorAbilityComponentList.AddItem(etc);
}


void WxAbilityViewer::OnStartComponentEmitter(UAbilityEmitterComponent* InComponent, WxAbilityViewer* InViewer, UBOOL bBullet, FLOAT CurrentTime)
{
	EditorEmitterComponent* EC = new EditorEmitterComponent;
	EC->Viewer = InViewer;
	EC->EmitterData = InComponent;
	EC->TemplateData = InComponent;
	EC->AbilityData = Cast<UMMOAbility>(InViewer->AbilityObject);
	EC->StartTime = CurrentTime;
	EC->bBullet = bBullet;
	EditorAbilityComponentList.AddItem(EC);
	EC->OnStart();
}

void WxAbilityViewer::OnStartComponent(UAbilityComponent* Component, FLOAT CurrentTime, UBOOL bTarget)
{
	if (Component == NULL)
	{
		return;
	}

	USkeletalMeshComponent* SelfMesh = NULL;
	USkeletalMeshComponent* OtherMesh = NULL;
	if (bTarget)
	{
		SelfMesh = PreviewSkelCompTarget;
		OtherMesh = PreviewSkelComp;
	}
	else
	{
		SelfMesh = PreviewSkelComp;
		OtherMesh = PreviewSkelCompTarget;
	}

	Component->bInHitPoint = bTarget;

	if (!SelfMesh)
	{
		return;
	}

	if (Component->IsA(UAbilityAnimationComponent::StaticClass()))
	{
		UAbilityAnimationComponent* comp = (UAbilityAnimationComponent*)(Component);
		UAnimSequence* Seq = NULL;
		for (INT i = 0; i < comp->Anims.Num(); ++i)
		{
			Seq = SelfMesh->FindAnimSequence(comp->Anims(i));
			if (Seq)
			{
				break;
			}
		}

		if (Seq)
		{
			UAnimNodeSequence* node = GetMeshAnimNodeSequence(SelfMesh);
			if (node)
			{
				node->SetAnim(Seq->SequenceName);
				FLOAT tt = FLOAT(comp->StartFrame) / 30.f;
				node->PlayAnim(comp->bLoop, comp->Rate, tt);
			}

			if (!bTarget)
			{
				// play weapon anim
				for (INT i = 0; i < PreviewWeaponList.Num(); ++i)
				{
					if (PreviewWeaponList(i))
					{
						node = GetMeshAnimNodeSequence(Cast<USkeletalMeshComponent>(PreviewWeaponList(i)));
						if (node)
						{
							node->SetAnim(Seq->SequenceName);
							FLOAT st = FLOAT(comp->StartFrame) / 30.f;
							node->PlayAnim(comp->bLoop, comp->Rate, st);
						}

					}
				}

				HideWeapon(comp->bHiddenWeapon);
			}
		}
	}
	else if (Component->IsA(UAbilityHitPointComponent::StaticClass()))
	{
		OnHitPoint(Component);
	}
	else if (Component->IsA(UAbilityDecalComponent::StaticClass()))
	{
		EditorDecalComponent* etc = new EditorDecalComponent;
		etc->DecalData = Cast<UAbilityDecalComponent>(Component);
		etc->Viewer = this;
		EditorAbilityComponentList.AddItem(etc);
		etc->OnStart();
	}
	else if (Component->IsA(UAbilityPostProcessComponent::StaticClass()))
	{
		EditorPostProcessComponent* etc = new EditorPostProcessComponent;
		etc->PostProcessData = Cast<UAbilityPostProcessComponent>(Component);
		etc->Viewer = this;
		EditorAbilityComponentList.AddItem(etc);
		etc->OnStart();
	}
	else if (Component->IsA(UAbilitySkeletalMeshComponent::StaticClass()))
	{
		EditorAbilityMeshComponent* node = new EditorAbilityMeshComponent;
		node->MeshData = Cast<UAbilitySkeletalMeshComponent>(Component);
		node->Viewer = this;
		EditorAbilityComponentList.AddItem(node);
		node->OnStart();
	}
	else if (Component->IsInA(UAbilityStaticMeshComponent::StaticClass()))
	{
		EditorStaticMeshComponent* node = new EditorStaticMeshComponent;
		node->Init(Component, this);
		EditorAbilityComponentList.AddItem(node);
		node->OnAction(TRUE);
	}
	else if (Component->IsA(UAbilityComp_MaterialEffect::StaticClass()))
	{
		UAbilityComp_MaterialEffect* InComponent = Cast<UAbilityComp_MaterialEffect>(Component);
		if (InComponent)
		{
			OnStartComponentMaterial(InComponent, bTarget);
		}
	}
	else if (Component->IsA(UAbilityTrailComponent::StaticClass()))
	{
		OnStartComponentTrail(Cast<UAbilityTrailComponent>(Component));
		//EditorTrailComponent* node = new EditorTrailComponent;
		//node->TrailData = Cast<UAbilityTrailComponent>(Component);
		//node->Viewer = this;
		//EditorAbilityComponentList.AddItem(node);
		//node->OnStart();
	}
	else if (Component->IsA(UAbilityActionsComponent::StaticClass()))
	{
		EditorActionsComponent* node = new EditorActionsComponent;
		node->ActionsData = Cast<UAbilityActionsComponent>(Component);
		node->Viewer = this;
		EditorAbilityComponentList.AddItem(node);
		node->OnStart();
	}
	else if (Component->IsA(UAbilityVisibleComponent::StaticClass()))
	{
		EditorVisibleComponent* node = new EditorVisibleComponent;
		node->VisibleData = Cast<UAbilityVisibleComponent>(Component);
		node->Viewer = this;
		EditorAbilityComponentList.AddItem(node);
		node->OnStart();
	}
	else if (Component->IsA(UAbilityScaleComponent::StaticClass()))
	{
		EditorScaleComponent* node = new EditorScaleComponent;
		node->ScaleData = Cast<UAbilityScaleComponent>(Component);
		node->Viewer = this;
		EditorAbilityComponentList.AddItem(node);
		node->OnStart();
	}
	else if (Component->IsA(UPositionTransformComponent::StaticClass()))
	{
		EditorPositionTransformComponent* node = new EditorPositionTransformComponent;
		node->PositionTransformData = Cast<UPositionTransformComponent>(Component);
		node->Viewer = this;
		EditorAbilityComponentList.AddItem(node);
		node->OnStart();
	}
	else if (Component->IsA(UAbilityPhysicSimulateComponent::StaticClass()))
	{
		EditorPhysicSimulateComponent* node = new EditorPhysicSimulateComponent;
		node->PhysicSimulateData = Cast<UAbilityPhysicSimulateComponent>(Component);
		node->Viewer = this;
		EditorAbilityComponentList.AddItem(node);
		node->OnStart();
	}
	else if (Component->IsA(UAbilityThrowComponent::StaticClass()))
	{
		EditorThrowComponent* node = new EditorThrowComponent;
		node->Init(Component, this);
		EditorAbilityComponentList.AddItem(node);
		node->OnAction(TRUE);
	}
	else if (Component->IsA(UAbilityMirrorComponent::StaticClass()))
	{
		EditorMirrorComponent* node = new EditorMirrorComponent;
		node->Init(Component, this);
		EditorAbilityComponentList.AddItem(node);
		node->OnAction(TRUE);
	}
	else if (Component->IsA(UAbilityEmitterComponent::StaticClass()))
	{
		UAbilityEmitterComponent* comp = Cast<UAbilityEmitterComponent>(Component);
		if (comp)
		{
			OnStartComponentEmitter(comp, this, FALSE, CurrentTime);
		}
	}
	else if (Component->IsA(UAbilitySoundCueComponent::StaticClass()))
	{
		UAbilitySoundCueComponent* comp = Cast<UAbilitySoundCueComponent>(Component);
		if (comp)
		{
			TArray<USoundCue*> ToPlaySounds;
			ToPlaySounds.Empty();
			if (comp->SoundPlayType == ESPT_WeaponUseSound || comp->SoundPlayType == ESPT_WeaponHitSound || comp->SoundPlayType == ESPT_ClothNoise) 
			{
				if (comp->PreviewSound)
				{
					ToPlaySounds.AddItem(comp->PreviewSound);
				}
			}
			else
			{
				BYTE EleType = 0;
				if (AbilityObject->PreviewResource)
				{
					EleType = AbilityObject->PreviewResource->OverriddenElementaryType;
				}
				comp->GetSoundCueByElementaryType(EleType, ToPlaySounds);
				//for (INT i =0; i < comp->Sounds.Num(); ++i)
				//{
				//	if (comp->Sounds(i))
				//	{
				//		ToPlaySounds.AddItem(comp->Sounds(i));
				//	}
				//}
			}

			for (INT i = 0; i < ToPlaySounds.Num(); ++i)
			{
				UAudioComponent* AudioComponent = UAudioDevice::CreateComponent( ToPlaySounds(i), SelfMesh->GetScene(), NULL, 0 );
				if( AudioComponent )
				{
					AudioComponent->bUseOwnerLocation	= 0;
					AudioComponent->Location			= SelfMesh->LocalToWorld.GetOrigin();


					AudioComponent->VolumeMultiplier		= 1.0;
					AudioComponent->PitchMultiplier			= 1.0;
					AudioComponent->bAllowSpatialization	&= GIsGame;
					AudioComponent->bIsUISound				= !GIsGame;
					AudioComponent->bAutoDestroy			= 1;
					AudioComponent->SubtitlePriority		= 10000;
					AudioComponent->Play();
				}
			}

		}
	}

}


/** Called to mark the beginning of a transaction entry for undo/redo 
 * Be sure to call Modify() on the UObject afterward
 * @param pcTransaction - unique transaction identifier
 */
bool WxAbilityViewer::BeginTransaction(const TCHAR* pcTransaction)
{
	if (TransactionInProgress())
	{
		FString kError(*LocalizeUnrealEd("Error_FailedToBeginTransaction"));
		kError += kTransactionName;
		checkf(0, TEXT("%s"), *kError);
		return FALSE;
	}

	GEditor->BeginTransaction(pcTransaction);
	kTransactionName = FString(pcTransaction);
	bTransactionInProgress = TRUE;

	return TRUE;
}

/** Called to mark the end of a transaction entry for undo/redo 
 * @param pcTransaction - unique transaction identifier (must match last call to BeginTransaction()
 */
bool WxAbilityViewer::EndTransaction(const TCHAR* pcTransaction)
{
	if (!TransactionInProgress())
	{
		FString kError(*LocalizeUnrealEd("Error_FailedToEndTransaction"));
		kError += kTransactionName;
		checkf(0, TEXT("%s"), *kError);
		return FALSE;
	}

	if (appStrcmp(*kTransactionName, pcTransaction) != 0)
	{
		debugf(TEXT("AnimSetViewer -   EndTransaction = %s --- Curr = %s"), 
			pcTransaction, *kTransactionName);
		return FALSE;
	}

	GEditor->EndTransaction();

	kTransactionName = TEXT("");
	bTransactionInProgress = FALSE;

	return TRUE;
}

/** Called to rollback previous begin/end transaction */
void WxAbilityViewer::UndoTransaction()
{
	GUnrealEd->Exec( TEXT("TRANSACTION UNDO") );
}

/** Called to redo the previously undone begin/end transaction */
void WxAbilityViewer::RedoTransaction()
{
	GUnrealEd->Exec( TEXT("TRANSACTION REDO") );
}

/** Have we called BeginTransaction() without an EndTransaction() */
bool WxAbilityViewer::TransactionInProgress()
{
	return bTransactionInProgress;
}

/**
 * Event wrapper around copying selected animseq to the clipboard
 */
void WxAbilityViewer::OnCopySelectedAnimSeqToClipboard(wxCommandEvent& In)
{
	//if( SelectedAnimSeq )
	//{
	//	appClipboardCopy(*SelectedAnimSeq->SequenceName.GetNameString());
	//}
}


// /**
//  * Open a PSA file with the given name, and returns how many animations this PSA has and list of names. 
//  * This is partially duplicated function with UEditorEngine::ImportPSAIntoAnimSet
//  * @param Filename - PSA file name
//  * @param ListOfAnimations - out list of animations this PSA has
//  * @return # of animations it found
//  */
// INT GetListOfAnimations(const TCHAR* Filename, TArray<FString> & ListOfAnimations )
// {
// 	// Open skeletal animation key file and read header.
// 	FArchive* AnimFile = GFileManager->CreateFileReader( Filename, 0, GLog );
// 	if( !AnimFile )
// 	{
// 		debugf( LocalizeSecure(LocalizeUnrealEd("Error_ErrorOpeningAnimFile"), Filename) );
// 		return 0;
// 	}
// 
// 	// Read main header
// 	VChunkHeader ChunkHeader;
// 	AnimFile->Serialize( &ChunkHeader, sizeof(VChunkHeader) );
// 	if( AnimFile->IsError() )
// 	{
// 		debugf( LocalizeSecure(LocalizeUnrealEd("Error_ErrorReadingAnimFile"), Filename) );
// 		delete AnimFile;
// 		return 0;
// 	}
// 
// 	// First I need to verify if curve data would exist
// 	// if version is higher than this, curve data is expected
// 	UBOOL bCheckCurveData = ChunkHeader.TypeFlag >= 20090127;
// 	
// 	/////// Read the bone names - convert into array of FNames.
// 	AnimFile->Serialize( &ChunkHeader, sizeof(VChunkHeader) );
// 
// 	INT NumPSATracks = ChunkHeader.DataCount; // Number of tracks of animation. One per bone.
// 
// 	struct FNamedBoneBinaryNoAlign
// 	{
// 		ANSICHAR   Name[64];	// Bone's name
// 		DWORD      Flags;		// reserved
// 		INT        NumChildren; //
// 		INT		   ParentIndex;	// 0/NULL if this is the root bone.  
// 		VJointPosNoAlign  BonePos;	    //
// 	};
// 
// 	/** This is due to keep the alignment working with ActorX format 
// 	 * FQuat in ActorX isn't 16 bit aligned, so when serialize using sizeof, this does not match up, 
// 	 */
// 	TArray<FNamedBoneBinaryNoAlign> RawBoneNamesBinNoAlign;
// 	RawBoneNamesBinNoAlign.Add(NumPSATracks);
// 	AnimFile->Serialize( &RawBoneNamesBinNoAlign(0), sizeof(FNamedBoneBinaryNoAlign) * ChunkHeader.DataCount);
// 	/////// Read the animation sequence infos
// 	AnimFile->Serialize( &ChunkHeader, sizeof(VChunkHeader) );
// 
// 	INT NumPSASequences = ChunkHeader.DataCount;
// 
// 	// emtpy list
// 	ListOfAnimations.Empty(NumPSASequences);
// 
// 	// read raw anim seq info
// 	TArray<AnimInfoBinary> RawAnimSeqInfo; // Array containing per-sequence information (sequence name, key range etc)
// 	RawAnimSeqInfo.Add(NumPSASequences);
// 	AnimFile->Serialize( &RawAnimSeqInfo(0), sizeof(AnimInfoBinary) * ChunkHeader.DataCount);
// 
// 	// iterate through and copy back to output data
// 	for ( INT I=0; I<RawAnimSeqInfo.Num(); ++I )
// 	{
// 		// add the list of anims to
// 		ListOfAnimations.AddItem(RawAnimSeqInfo(I).Name);
// 	}
// 
// 	// Have now read all information from file - can release handle.
// 	delete AnimFile;
// 	AnimFile = NULL;
// 
// 	return ListOfAnimations.Num();
// }
// 
// /* do I need to reimport from the file name given to current animset?
//  * @param Filename: PSA filename
//  * @param CurrentAnimSet : animset that I need to compare to
// */
// UBOOL NeedToReimportAnimations(const TCHAR* Filename, UAnimSet * CurrentAnimset)
// {
// 	TArray<FString> AnimList;
// 	// get list of animations in the file name
// 	if ( GetListOfAnimations(Filename, AnimList) > 0 )
// 	{
// 		// if so compare with CurrrentAnimSEt
// 		for ( INT I=0; I<AnimList.Num(); ++I )
// 		{
// 			// this is a bit tricky - if we don't want everything to be reimported?
// 			// for now we decide to import all of them if we find same one after talking to Jay H
// 			if ( CurrentAnimset->FindAnimSequence(FName(*AnimList(I))) )
// 			{
// 				return TRUE;
// 			}
// 		}
// 	}
// 
// 	return FALSE;
// }

/** Handle global file changed events. */
void WxAbilityViewer::Send( ECallbackEventType InType, const FString& InString, UObject* InObject)
{
//	if (bListenForFileChanges)
//	{
//		FFilename FileName = InString;
//		FString FileExtension = FileName.GetExtension();
//		//extra guard in case of other file listeners sending events
//		//if (!((FileExtension == "psk") || (FileExtension == "psa") || (FileExtension == "fbx")))
//		// for now we only support PSA. 
//		// PSK takes too long to import and FBX is still WIP
//		if (!(FileExtension == "psa"))
//		{
//			//not one of the file extensions to listen for.  Reject.
//			return;
//		}
//
//		if ( !SelectedAnimSet )
//		{
//			return;
//		}
//
//		FString AnimationName = FileName.GetBaseFilename();
//
//		//assume this isn't a file we care about
//// 		UBOOL bPerformImport = NeedToReimportAnimations(*FileName, SelectedAnimSet);
//// 
//// 		// if we need to reimport
//// 		if (bPerformImport)
//// 		{
//// 			//Actually perform the import of the animation
//// 			GApp->LastDir[LD_PSA] = FileName.GetPath(); // Save path as default for next time.
//// 
//// 			UEditorEngine::ImportPSAIntoAnimSet( SelectedAnimSet, *FileName, SelectedSkelMesh, TRUE );
//// 
//// 			SelectedAnimSet->MarkPackageDirty();
//// 
//// 			// balloon message
//// 			FString BalloonMessage = FString::Printf(TEXT("Animation %s Reimported: Check log for detail messages. "), *AnimationName);
//// 
//// 			// Display, via balloon, the results of the auto-import
//// 			FShowBalloonNotification::ShowNotification( LocalizeUnrealEd("AnimSetEditor_AutoReimportTitle"), BalloonMessage, ID_BALLOON_NOTIFY_ID );
//// 			// log out to log window - balloon message does not work on 64 bit
//// 			debugf(*BalloonMessage);
//// 
//// 			//cause a refresh of the available animation sets
//// 			UpdateAnimSeqList();
//// 			//reselect what was previously selected
//// 			SetSelectedAnimSequence( SelectedAnimSeq );
//// 		}
//	}
}

/**
 * This function is called when the window has been selected from within the ctrl + tab dialog.
 */
void WxAbilityViewer::OnSelected()
{
	Raise();
}

/**
* Called once to load AnimSetViewer settings, including window position.
*/

void WxAbilityViewer::LoadSettings()
{
	// Load Window Position.
	FWindowUtil::LoadPosSize(TEXT("AbilityEditor"), this, 256,256,1200,800);

	// Load the preview scene
	FEditorAbilityViewportClient* AbilityEditorClient = GetPreviewVC();
	check( AbilityEditorClient );
	AbilityEditorClient->PreviewScene.LoadSettings( TEXT( "AbilityEditor" ) );


	// loading sash position
	FLOAT PosBase = 0.7;
	FLOAT PosLeft = 0.7;
	FLOAT PosRight = 0.4;

	GConfig->GetFloat(TEXT("AbilityEditor"), TEXT("BaseSash"), PosBase, GEditorUserSettingsIni);
	GConfig->GetFloat(TEXT("AbilityEditor"), TEXT("LeftSash"), PosLeft, GEditorUserSettingsIni);
	GConfig->GetFloat(TEXT("AbilityEditor"), TEXT("RightSash"), PosRight, GEditorUserSettingsIni);

	INT ShowFlag = 0xffffffff;
	GConfig->GetInt(TEXT("AbilityEditor"), TEXT("ShowFlag"), ShowFlag, GEditorUserSettingsIni);
	DisplayFlag = ShowFlag;

	BaseWindow->SetSashPosition(BaseWindow->GetClientSize().GetWidth()*PosBase, true);
	LeftArea->SetSashPosition(LeftArea->GetClientSize().GetHeight()*PosLeft, true);
	RightArea->SetSashPosition(RightArea->GetClientSize().GetHeight()*PosRight, true);

}

/**
* Writes out values we want to save to the INI file.
*/

void WxAbilityViewer::SaveSettings()
{
	//SaveDockingLayout();

	// Save the preview scene
	FEditorAbilityViewportClient* AbilityEditorClient = GetPreviewVC();
	check( AbilityEditorClient );
	AbilityEditorClient->PreviewScene.SaveSettings( TEXT( "AbilityEditor" ) );

	//// Update the custom fov setting, so the last closed animset editor window is the saved setting
	//if ( GApp && GApp->EditorFrame && GApp->EditorFrame->ViewportConfigData && ToolBar && ToolBar->FOVSlider )
	//{
	//	GApp->EditorFrame->ViewportConfigData->SetCustomFOV( ToolBar->FOVSlider->GetValue() );
	//}

	// Save Window Position.
	FWindowUtil::SavePosSize(TEXT("AbilityEditor"), this);

	// save sash postion
	FLOAT PosBase = FLOAT(BaseWindow->GetSashPosition())/FLOAT(BaseWindow->GetClientSize().GetWidth());
	FLOAT PosLeft = FLOAT(LeftArea->GetSashPosition())/FLOAT(LeftArea->GetClientSize().GetHeight());
	FLOAT PosRight = FLOAT(RightArea->GetSashPosition())/FLOAT(RightArea->GetClientSize().GetHeight());
	GConfig->SetFloat(TEXT("AbilityEditor"), TEXT("BaseSash"), PosBase, GEditorUserSettingsIni);
	GConfig->SetFloat(TEXT("AbilityEditor"), TEXT("LeftSash"), PosLeft, GEditorUserSettingsIni);
	GConfig->SetFloat(TEXT("AbilityEditor"), TEXT("RightSash"), PosRight, GEditorUserSettingsIni);
	GConfig->SetInt(TEXT("AbilityEditor"), TEXT("ShowFlag"), DisplayFlag, GEditorUserSettingsIni);
}



/**
 * Returns a handle to the anim set viewer's preview viewport client.
 */
FEditorAbilityViewportClient* WxAbilityViewer::GetPreviewVC()
{
	checkSlow( PreviewWindow );
	return PreviewWindow->ASVPreviewVC;
}

/**
 * Serializes the preview scene so it isn't garbage collected
 *
 * @param Ar The archive to serialize with
 */
void WxAbilityViewer::Serialize(FArchive& Ar)
{
	FEditorAbilityViewportClient* ASVPreviewVC = GetPreviewVC();
	check( ASVPreviewVC );

	ASVPreviewVC->Serialize(Ar);
	//Ar << SocketPreviews;
}

///////////////////////////////////////////////////////////////////////////////////////
// Properties window NotifyHook stuff

void WxAbilityViewer::NotifyDestroy( void* Src )
{

}

void WxAbilityViewer::NotifyPreChange( void* Src, UProperty* PropertyAboutToChange )
{

//	if( PropertyAboutToChange )
//	{
//		// If it is the FaceFXAsset, NULL out the DefaultSkelMesh.
//		if( PropertyAboutToChange->GetFName() == FName(TEXT("FaceFXAsset")) )
//		{
//			if( SelectedSkelMesh && SelectedSkelMesh->FaceFXAsset )
//			{
//				SelectedSkelMesh->FaceFXAsset->DefaultSkelMesh = NULL;
//#if WITH_FACEFX
//				OC3Ent::Face::FxActor* Actor = SelectedSkelMesh->FaceFXAsset->GetFxActor();
//				if( Actor )
//				{
//					Actor->SetShouldClientRelink(FxTrue);
//				}
//#endif
//			}
//		}
//	}


	if (PropertyAboutToChange->GetFName() == FName(TEXT("PreviewResource")))
	{
		bMeshChange = TRUE;
	}
}

void WxAbilityViewer::NotifyPostChange( void* Src, UProperty* PropertyThatChanged )
{

	RefreshData();

	if (PropertyThatChanged)
	{
		if (AbilityObject)
		{
			AbilityObject->MarkPackageDirty();
		}
	}

	




//	if( PropertyThatChanged )
//	{
//		// If it is the FaceFXAsset, NULL out the DefaultSkelMesh.
//		if( PropertyThatChanged->GetFName() == FName(TEXT("FaceFXAsset")) )
//		{
//			if( SelectedSkelMesh && SelectedSkelMesh->FaceFXAsset )
//			{
//				SelectedSkelMesh->FaceFXAsset->DefaultSkelMesh = SelectedSkelMesh;
//#if WITH_FACEFX
//				OC3Ent::Face::FxActor* Actor = SelectedSkelMesh->FaceFXAsset->GetFxActor();
//				if( Actor )
//				{
//					Actor->SetShouldClientRelink(FxTrue);
//				}
//#endif
//				SelectedSkelMesh->FaceFXAsset->MarkPackageDirty();
//			}
//		}
//		else if(PropertyThatChanged->GetFName() == FName(TEXT("ClothingAssets")))
//		{
//			// If a property relating to the clothing asset assignment has changed, rebuild the apex clothing binding
//			if ( PreviewSkelComp )
//			{
//				PreviewSkelComp->ReleaseApexClothing();
//				PreviewSkelComp->InitApexClothing(RBPhysScene);
//			}
//		}
//		else if(PropertyThatChanged->GetFName() == FName(TEXT("bForceCPUSkinning")))
//		{
//			FComponentReattachContext ReattachContext(PreviewSkelComp);
//			FComponentReattachContext ReattachContextRaw(PreviewSkelCompRaw);
//			FComponentReattachContext ReattachContext1(PreviewSkelCompAux1);
//			FComponentReattachContext ReattachContext2(PreviewSkelCompAux2);
//			FComponentReattachContext ReattachContext3(PreviewSkelCompAux3);
//		}
//
//		//@todo. Do this the 'right' way...
//		bResampleAnimNotifyData = FALSE;
//		UObject* Outer = PropertyThatChanged->GetOuter();
//		UScriptStruct* ScriptStruct = Cast<UScriptStruct>(Outer);
//		if (ScriptStruct != NULL)
//		{
//			FAnimNotifyEvent* Event = (FAnimNotifyEvent*)ScriptStruct;
//			if (ScriptStruct->GetName() == TEXT("AnimNotifyEvent"))
//			{
//				bResampleAnimNotifyData = TRUE;
//			}
//		}
//		UAnimNotify_Trails* TrailsNotify = Cast<UAnimNotify_Trails>(Outer);
//		if (TrailsNotify)
//		{
//			bResampleAnimNotifyData = TRUE;
//		}
//
//		UClass* OuterClass = Cast<UClass>(Outer);
//		if (OuterClass)
//		{
//			if (OuterClass->GetName() == TEXT("AnimNotify_Trails"))
//			{
//				bResampleAnimNotifyData = TRUE;
//			}
//		}
//	}
//
//	// Might have changed UseTranslationBoneNames array. We have to fix any existing FAnimSetMeshLinkup objects in selected AnimSet.
//	// Would be nice to not do this all the time, but PropertyThatChanged seems flaky when editing arrays (sometimes NULL)
//	if( SelectedAnimSet)
//	{
//		SelectedAnimSet->BoneUseAnimTranslation.Empty();
//		SelectedAnimSet->ForceUseMeshTranslation.Empty();
//		SelectedAnimSet->LinkupCache.Empty();
//		SelectedAnimSet->SkelMesh2LinkupCache.Empty();
//
//		// We need to re-init any skeletal mesh components now, because they might still have references to linkups in this set.
//		for(TObjectIterator<USkeletalMeshComponent> It;It;++It)
//		{
//			USkeletalMeshComponent* SkelComp = *It;
//			if(!SkelComp->IsPendingKill() && !SkelComp->IsTemplate())
//			{
//				SkelComp->InitAnimTree();
//			}
//		}
//	}
//
//	// Might have changed ClothBones array - so need to rebuild cloth data.
//	// Would be nice to not do this all the time, but PropertyThatChanged seems flaky when editing arrays (sometimes NULL)
//	{
//		PreviewSkelComp->TermClothSim(NULL);
//		check(SelectedSkelMesh);
//		SelectedSkelMesh->ClearClothMeshCache();
//		SelectedSkelMesh->BuildClothMapping();
//		if(PreviewSkelComp->bEnableClothSimulation)
//		{
//			PreviewSkelComp->InitClothSim(RBPhysScene);
//		}
//		// Push any changes to cloth param to the running cloth sim.
//		PreviewSkelComp->UpdateClothParams();
//
//		PreviewSkelComp->TermSoftBodySim(NULL);
//		if(PreviewSkelComp->bEnableSoftBodySimulation)
//		{
//			PreviewSkelComp->InitSoftBodySim(RBPhysScene, TRUE);
//		}
//		PreviewSkelComp->UpdateSoftBodyParams();
//	}
//
//
//	// In case we change Origin/RotOrigin, update the components.
//	UpdateSkelComponents();
}

void WxAbilityViewer::NotifyExec( void* Src, const TCHAR* Cmd )
{
	GUnrealEd->NotifyExec(Src, Cmd);
}

/** Toggle the 'Socket Manager' window. */
void WxAbilityViewer::OnOpenSocketMgr(wxCommandEvent &In)
{
	if(!SocketMgr)
	{
// 		SocketMgr = new WxSocketManager( this, -1 );
// 		SocketMgr->Show();
// 
// 		ToolBar->ToggleTool(IDM_ANIMSET_OPENSOCKETMGR, true);
// 
// 		UpdateSocketList();
// 		SetSelectedSocket(NULL);
// 
// 		FEditorAbilityViewportClient* ASVPreviewVC = GetPreviewVC();
// 		check( ASVPreviewVC );
// 		ASVPreviewVC->Viewport->Invalidate();
	}
	else
	{
		SocketMgr->Close();

		ToolBar->ToggleTool(IDM_ANIMSET_OPENSOCKETMGR, false);
	}
}

/** Toggles the modal animation Animation compression dialog. */
void WxAbilityViewer::OnOpenAnimationCompressionDlg(wxCommandEvent &In)
{
// 	if ( !DlgAnimationCompression )
// 	{
// 		DlgAnimationCompression = new WxDlgAnimationCompression( this, -1 );
// 		DlgAnimationCompression->Show();
// 
// 		ToolBar->ToggleTool( IDM_ANIMSET_OpenAnimationCompressionDlg, true );
// 		MenuBar->Check( IDM_ANIMSET_OpenAnimationCompressionDlg, true );
// 	}
// 	else
// 	{
// 		DlgAnimationCompression->Close();
// 		DlgAnimationCompression = NULL;
// 		ToolBar->ToggleTool( IDM_ANIMSET_OpenAnimationCompressionDlg, false );
// 		MenuBar->Check( IDM_ANIMSET_OpenAnimationCompressionDlg, false );
// 	}
}

/** Updates the contents of the status bar, based on e.g. the selected set/sequence. */
void WxAbilityViewer::UpdateStatusBar()
{
	//StatusBar->UpdateStatusBar( this );
}

/** Updates the contents of the status bar, based on e.g. the selected set/sequence. */
void WxAbilityViewer::UpdateMaterialList()
{
	INT I=0;
	
 	if (PreviewSkelComp && PreviewSkelComp->SkeletalMesh)
 	{
		INT TotalNum = Min(PreviewSkelComp->SkeletalMesh->Materials.Num(), 5);

 		for (; I<TotalNum; ++I)
		{
			MenuBar->SectionSubMenu->Enable(IDM_ANIMSET_VERTEX_SECTION_0+I, true);
		}
	}

	for (; I<5; ++I)
	{
		MenuBar->SectionSubMenu->Enable(IDM_ANIMSET_VERTEX_SECTION_0+I, false);
	}

	// select section
	SelectSection(0);
}

/** Create a new Socket object and add it to the selected SkeletalMesh's Sockets array. */
void WxAbilityViewer::OnNewSocket( wxCommandEvent& In )
{
	//if(SocketMgr)
	//{
	//	// We only allow sockets to be created to bones that are in all LODs.

	//	// Get the bones we'll be updating if using the lowest LOD.
	//	FStaticLODModel& LODModel = SelectedSkelMesh->LODModels( SelectedSkelMesh->LODModels.Num()-1 );
	//	TArray<BYTE> LODBones = LODModel.RequiredBones;

	//	// Make list of bone names in lowest LOD.
	//	TArray<FString> BoneNames;
	//	BoneNames.AddZeroed( LODBones.Num() );
	//	for(INT i=0; i<LODBones.Num(); i++)
	//	{
	//		FName BoneName = SelectedSkelMesh->RefSkeleton( LODBones(i) ).Name;
	//		BoneNames(i) = BoneName.ToString();
	//	}

	//	// Display dialog and let user pick which bone to create a Socket for.
	//	WxDlgGenericComboEntry BoneDlg;
	//	if( BoneDlg.ShowModal( TEXT("NewSocket"), TEXT("BoneName"), BoneNames, 0, TRUE ) == wxID_OK )
	//	{
	//		FName BoneName = FName( *BoneDlg.GetSelectedString() );
	//		if(BoneName != NAME_None)
	//		{
	//			WxDlgGenericStringEntry NameDlg;
	//			UBOOL bLoopAddSocket = TRUE;

	//			while(bLoopAddSocket)
	//			{
	//				if( NameDlg.ShowModal( TEXT("NewSocket"), TEXT("SocketName"), TEXT("") ) == wxID_OK )
	//				{
	//					const FString& NewName = NameDlg.GetEnteredString();
	//					UBOOL bNameAlreadyExists = FALSE;
	//					for(INT SocketIndex = 0; SocketIndex < SelectedSkelMesh->Sockets.Num(); ++SocketIndex)
	//					{
	//						if(NewName == SelectedSkelMesh->Sockets(SocketIndex)->SocketName.GetNameString())
	//						{
	//							bNameAlreadyExists = TRUE;
	//							break;
	//						}
	//					}

	//					if(bNameAlreadyExists)
	//					{
	//						appMsgf(AMT_OK, LocalizeSecure(LocalizeUnrealEd("SocketAlreadyExists"), *NewName));
	//						continue;
	//					}

	//					FName NewSocketName = FName( *NewName );
	//					if(NewSocketName != NAME_None)
	//					{
	//						USkeletalMeshSocket* NewSocket = ConstructObject<USkeletalMeshSocket>( USkeletalMeshSocket::StaticClass(), SelectedSkelMesh );
	//						check(NewSocket);

	//						NewSocket->SocketName = NewSocketName;
	//						NewSocket->BoneName = BoneName;

	//						SelectedSkelMesh->Sockets.AddItem(NewSocket);
	//						SelectedSkelMesh->MarkPackageDirty();

	//						UpdateSocketList();
	//						SetSelectedSocket(NewSocket);
	//					}
	//				}

	//				// If a valid name wasn't entered then continue was called to loop over which means you can't get to this point
	//				bLoopAddSocket = FALSE;
	//			}
	//		}
	//	}
	//}
}

/** Delete the currently selected Socket. */
void WxAbilityViewer::OnDeleteSocket( wxCommandEvent& In )
{
	//if(SocketMgr)
	//{
	//	if(SelectedSocket)
	//	{
	//		SelectedSkelMesh->Sockets.RemoveItem(SelectedSocket);
	//		SelectedSkelMesh->MarkPackageDirty();

	//		UpdateSocketList();
	//		SetSelectedSocket(NULL);
	//		RecreateSocketPreviews();

	//		FEditorAbilityViewportClient* ASVPreviewVC = GetPreviewVC();
	//		check( ASVPreviewVC );
	//		ASVPreviewVC->Viewport->Invalidate();
	//	}
	//}
}

/** 
*	Handler called when clicking on a socket from the list. 
*	Sets the socket properties into the property window, and set the SelectedSocket variable.
*/
void WxAbilityViewer::OnClickSocket( wxCommandEvent& In )
{
	if(SocketMgr)
	{
		// Get the index of the item we clicked on.
		INT SocketIndex = SocketMgr->SocketList->GetSelection();
		if( SocketIndex != -1 )
		{
			USkeletalMeshSocket* SkelMeshSocket = ( USkeletalMeshSocket* )SocketMgr->SocketList->GetClientData( SocketIndex );
			if( SkelMeshSocket )
			{
				SetSelectedSocket( SkelMeshSocket );
			}
		}
	}
}

/** Called when clicking any of the socket movement mode buttons, to change the current mode. */
void WxAbilityViewer::OnSocketMoveMode( wxCommandEvent& In )
{
	if(SocketMgr)
	{
		INT Id = In.GetId();
		if(Id == IDM_ANIMSET_SOCKET_TRANSLATE)
		{
			SetSocketMoveMode(SMM_Translate);
		}
		else if(Id == IDM_ANIMSET_SOCKET_ROTATE)
		{
			SetSocketMoveMode(SMM_Rotate);
		}
	}
}

/**
 * Toggles whether or not active file listening should be engaged
 */
void WxAbilityViewer::OnToggleActiveFileListen( wxCommandEvent& In )
{
	bListenForFileChanges = !bListenForFileChanges;
	SetFileSystemNotifications(bListenForFileChanges);
}

/**
 * Helper function to enable and disable file listening that puts the file extensions local to one function
 */
void WxAbilityViewer::SetFileSystemNotifications(const UBOOL bListenOnOff)
{
#if WITH_MANAGED_CODE
	SetFileSystemNotification("psa", bListenOnOff);
	SetFileSystemNotification("psk", bListenOnOff);
	SetFileSystemNotification("fbx", bListenOnOff);
#endif
}


/**
 * Event that fires to update menu items for active file listening
 */
void WxAbilityViewer::UI_ActiveFileListen( wxUpdateUIEvent& In )
{
	In.Check( bListenForFileChanges == TRUE );
}


/** Iterate over all sockets clearing their preview skeletal/static mesh. */
void WxAbilityViewer::OnClearPreviewMeshes( wxCommandEvent& In )
{
	//ClearSocketPreviews();

	//for(INT i=0; i<SelectedSkelMesh->Sockets.Num(); i++)
	//{
	//	USkeletalMeshSocket* Socket = SelectedSkelMesh->Sockets(i);
	//	Socket->PreviewSkelMesh = NULL;
	//	Socket->PreviewStaticMesh = NULL;
	//	Socket->PreviewSkelComp = NULL;
	//	Socket->PreviewParticleSystem = NULL;
	//}

	//if(SocketMgr)
	//{
	//	SocketMgr->SocketProps->SetObject( SelectedSocket, EPropertyWindowFlags::NoFlags );
	//}
}

/** Copy socket info from mesh to clipboard. */
void WxAbilityViewer::OnCopySockets( wxCommandEvent& In )
{
	//// First, clear existing buffer
	//GUnrealEd->SkelSocketPasteBuffer.Empty();

	//// Then copy data into it.
	//GUnrealEd->SkelSocketPasteBuffer.AddZeroed(SelectedSkelMesh->Sockets.Num());
	//for(INT i=0; i<SelectedSkelMesh->Sockets.Num(); i++)
	//{
	//	USkeletalMeshSocket* Socket = SelectedSkelMesh->Sockets(i);
	//	GUnrealEd->SkelSocketPasteBuffer(i).SocketName			= Socket->SocketName;
	//	GUnrealEd->SkelSocketPasteBuffer(i).BoneName			= Socket->BoneName;
	//	GUnrealEd->SkelSocketPasteBuffer(i).RelativeLocation	= Socket->RelativeLocation;
	//	GUnrealEd->SkelSocketPasteBuffer(i).RelativeRotation	= Socket->RelativeRotation;
	//	GUnrealEd->SkelSocketPasteBuffer(i).RelativeScale		= Socket->RelativeScale;
	//}
}

/** Rename a socket specified by the user */
void WxAbilityViewer::OnRenameSockets( wxCommandEvent& In )
{
//// Get the item we clicked on.
//	INT SocketIndex = SocketMgr->SocketList->GetSelection();
//	USkeletalMeshSocket* SkelMeshSocket = ( USkeletalMeshSocket* )SocketMgr->SocketList->GetClientData( SocketIndex );
//
//	if (SkelMeshSocket)
//	{
//		WxDlgGenericStringEntry NameDlg;
//		UBOOL bLoopAddSocket = TRUE;
//		while(bLoopAddSocket)
//		{
//			//Present the rename dialog
//			if( NameDlg.ShowModal( TEXT("RenameSocket"), TEXT("SocketName"), *SkelMeshSocket->SocketName.ToString() ) == wxID_OK )
//			{
//				//Repeat until user cancels or gets a unique name
//				const FString& NewName = NameDlg.GetEnteredString();
//
//				UBOOL bNameAlreadyExists = FALSE;
//				UBOOL bSameSocket = FALSE;
//				for(INT AllSocketsIndex = 0; AllSocketsIndex < SelectedSkelMesh->Sockets.Num(); ++AllSocketsIndex)
//				{
//					if(NewName == SelectedSkelMesh->Sockets(AllSocketsIndex)->SocketName.GetNameString())
//					{
//						bNameAlreadyExists = TRUE;
//						if (SocketIndex == AllSocketsIndex)
//						{
//							bSameSocket = FALSE;
//						}
//						break;
//					}
//				}
//
//				if(bNameAlreadyExists)
//				{
//					if (bSameSocket)
//					{
//						//Just exit out without doing anything
//						bLoopAddSocket = FALSE;
//					}
//					appMsgf(AMT_OK, LocalizeSecure(LocalizeUnrealEd("SocketAlreadyExists"), *NewName));
//					continue;
//				}
//
//				//Change the name
//				FName NewSocketName = FName( *NewName );
//				if(NewSocketName != NAME_None)
//				{
//					SkelMeshSocket->SocketName = NewSocketName;
//					SelectedSkelMesh->MarkPackageDirty();
//
//					// Rename - in order to make sorting work we need to delete and add the item (rather than doing a SetText())
//					SocketMgr->SocketList->Delete( SocketIndex );
//					SocketMgr->SocketList->Append( *( SkelMeshSocket->SocketName.ToString() ), SkelMeshSocket );
//				}
//			}
//
//			// If a valid name wasn't entered then continue was called to loop over which means you can't get to this point
//			bLoopAddSocket = FALSE;
//		}
//	}
}

/** Paste socket info from clipboard to current mesh. */
void WxAbilityViewer::OnPasteSockets( wxCommandEvent& In )
{
	//// First check buffer is not empty.
	//if(GUnrealEd->SkelSocketPasteBuffer.Num() == 0)
	//{
	//	appMsgf( AMT_OK, *LocalizeUnrealEd("NoSocketsToPaste") );
	//	return;
	//}

	//// Then iterate over each one
	//for(INT i=0; i<GUnrealEd->SkelSocketPasteBuffer.Num(); i++)
	//{
	//	FName SocketName = GUnrealEd->SkelSocketPasteBuffer(i).SocketName;

	//	// See if we have a socket with this name.
	//	UBOOL bNameAlreadyExists = FALSE;
	//	for(INT SocketIndex = 0; SocketIndex < SelectedSkelMesh->Sockets.Num(); ++SocketIndex)
	//	{
	//		if(SocketName == SelectedSkelMesh->Sockets(SocketIndex)->SocketName)
	//		{
	//			bNameAlreadyExists = TRUE;
	//			break;
	//		}
	//	}

	//	if(bNameAlreadyExists)
	//	{
	//		appMsgf(AMT_OK, LocalizeSecure(LocalizeUnrealEd("SocketAlreadyExists"), *SocketName.ToString()));
	//		continue;
	//	}

	//	// Check there is a bone with the bone name.
	//	FName BoneName = GUnrealEd->SkelSocketPasteBuffer(i).BoneName;
	//	INT BoneIndex = SelectedSkelMesh->MatchRefBone( BoneName );
	//	if(BoneIndex == INDEX_NONE)
	//	{
	//		appMsgf(AMT_OK, LocalizeSecure(LocalizeUnrealEd("SocketBoneNotFound"), *BoneName.ToString(), *SocketName.ToString()));
	//		continue;
	//	}

	//	// Things look good - create socket, and add to skel mesh
	//	USkeletalMeshSocket* NewSocket = ConstructObject<USkeletalMeshSocket>( USkeletalMeshSocket::StaticClass(), SelectedSkelMesh );
	//	if(NewSocket)
	//	{
	//		NewSocket->SocketName = SocketName;
	//		NewSocket->BoneName = BoneName;

	//		NewSocket->RelativeLocation = GUnrealEd->SkelSocketPasteBuffer(i).RelativeLocation;
	//		NewSocket->RelativeRotation = GUnrealEd->SkelSocketPasteBuffer(i).RelativeRotation;
	//		NewSocket->RelativeScale = GUnrealEd->SkelSocketPasteBuffer(i).RelativeScale;

	//		SelectedSkelMesh->Sockets.AddItem(NewSocket);
	//	}
	//}

	//SelectedSkelMesh->MarkPackageDirty();
	//UpdateSocketList();
}

/** When the user slides the progressive drawing slider */
void WxAbilityViewer::OnProgressiveSliderChanged(wxScrollEvent& In)
{
	PreviewSkelComp->ProgressiveDrawingFraction = ((FLOAT)In.GetPosition()) / 10000.f;
}

/** Resets the FOV slider */
void WxAbilityViewer::OnFOVReset( wxCommandEvent& In )
{
	SetFOV( ( GEditor ? ((INT)GEditor->FOVAngle) : 90 ) );
}

/** When the user slides the FOV slider */
void WxAbilityViewer::OnFOVSliderChanged(wxScrollEvent& In)
{
	SetFOV( In.GetPosition() );
}

/** Sets the FOV and updated the icons */
void WxAbilityViewer::SetFOV(const INT iFOV)
{
	// Set the viewport to the parsed value
	if ( PreviewWindow && PreviewWindow->ASVPreviewVC )
	{
		PreviewWindow->ASVPreviewVC->ViewFOV = ((FLOAT)iFOV);
	}
	// Update the custom FOV so that any additional animset windows that open while this is still open have this value too
	if ( GApp && GApp->EditorFrame && GApp->EditorFrame->ViewportConfigData )
	{
		GApp->EditorFrame->ViewportConfigData->SetCustomFOV( iFOV );
	}
	//// Update the slider to also show the new value
	//if ( ToolBar && ToolBar->FOVSlider )
	//{
	//	ToolBar->FOVSlider->SetValue( iFOV );
	//}
}

//////////////////////////////////////////////////////////////////////////

/** Set the movement mode for the selected socket to the one supplied. */
void WxAbilityViewer::SetSocketMoveMode( EASVSocketMoveMode NewMode )
{
	if(SocketMgr)
	{
		SocketMgr->MoveMode = NewMode;

		if(SocketMgr->MoveMode == SMM_Rotate)
		{
			SocketMgr->ToolBar->ToggleTool( IDM_ANIMSET_SOCKET_TRANSLATE, false );
			SocketMgr->ToolBar->ToggleTool( IDM_ANIMSET_SOCKET_ROTATE, true );
		}
		else
		{
			SocketMgr->ToolBar->ToggleTool( IDM_ANIMSET_SOCKET_TRANSLATE, true );
			SocketMgr->ToolBar->ToggleTool( IDM_ANIMSET_SOCKET_ROTATE, false );
		}

		SocketMgr->UpdateTextEntry();

		// Flush hit proxies so we get them for the new widget.
		FEditorAbilityViewportClient* ASVPreviewVC = GetPreviewVC();
		check( ASVPreviewVC );
		ASVPreviewVC->Viewport->Invalidate();
	}
}

/** Set the currently selected socket to the supplied one */
void WxAbilityViewer::SetSelectedSocket(USkeletalMeshSocket* InSocket)
{
	//if (SelectedSocket != InSocket)
	//{
	//	SelectedSocket = InSocket;
	//	// Flush hit proxies so we get them for the new widget.
	//	FEditorAbilityViewportClient* ASVPreviewVC = GetPreviewVC();
	//	check( ASVPreviewVC );
	//	ASVPreviewVC->Viewport->Invalidate();

	//}
	//if( InSocket && SocketMgr )
	//{
	//	INT SocketIndex = INDEX_NONE;
	//	for( UINT iSocket = 0; iSocket < SocketMgr->SocketList->GetCount(); iSocket++ )
	//	{
	//		if( ( ( USkeletalMeshSocket* )SocketMgr->SocketList->GetClientData( iSocket ) ) == InSocket )
	//		{
	//			SocketIndex = iSocket;
	//		}
	//	}
	//	if( SocketIndex != INDEX_NONE )
	//	{
	//		SelectedSocket = InSocket;
	//		SocketMgr->SocketProps->SetObject( SelectedSocket, EPropertyWindowFlags::NoFlags );
	//		SocketMgr->SocketList->SetSelection( SocketIndex );

	//		// Flush hit proxies so we get them for the new widget.
	//		FEditorAbilityViewportClient* ASVPreviewVC = GetPreviewVC();
	//		check( ASVPreviewVC );
	//		ASVPreviewVC->Viewport->Invalidate();
	//		return;
	//	}
	//}

	//SelectedSocket = NULL;

	//if(SocketMgr)
	//{
	//	SocketMgr->SocketProps->SetObject(NULL, EPropertyWindowFlags::NoFlags );
	//	SocketMgr->SocketList->DeselectAll();
	//}
}

/** Update the list of sockets shown in the Socket Manager list box to match that of the selected SkeletalMesh. */
void WxAbilityViewer::UpdateSocketList()
{
	//if(SocketMgr)
	//{
	//	// Remember the current selection.
	//	UINT CurrentSelection = static_cast<UINT>(SocketMgr->SocketList->GetSelection());

	//	SocketMgr->SocketList->Clear();

	//	// Remove empty sockets
	//	for(INT i=0; i<SelectedSkelMesh->Sockets.Num(); i++)
	//	{
	//		USkeletalMeshSocket* Socket = SelectedSkelMesh->Sockets(i);
	//		if (NULL == Socket)
	//		{
	//			SelectedSkelMesh->Sockets.Remove(i);
	//			i--;
	//		}
	//	}

	//	for(INT i=0; i<SelectedSkelMesh->Sockets.Num(); i++)
	//	{
	//		USkeletalMeshSocket* Socket = SelectedSkelMesh->Sockets(i);
	//		SocketMgr->SocketList->Append( *(Socket->SocketName.ToString()), Socket );
	//	}

	//	// Restore the selection we had.
	//	if(CurrentSelection >= 0 && CurrentSelection < SocketMgr->SocketList->GetCount())
	//	{
	//		SocketMgr->SocketList->SetSelection(CurrentSelection);
	//	}
	//}
}

/** Destroy all components being used to preview sockets. */
void WxAbilityViewer::ClearSocketPreviews()
{
	//FEditorAbilityViewportClient* ASVPreviewVC = GetPreviewVC();
	//check( ASVPreviewVC );

	//for(INT i=0; i<SocketPreviews.Num(); i++)
	//{
	//	UPrimitiveComponent* PrimComp = SocketPreviews(i).PreviewComp;
	//	PreviewSkelComp->DetachComponent(PrimComp);
	//	ASVPreviewVC->PreviewScene.RemoveComponent(PrimComp);
	//}
	//SocketPreviews.Empty();
}


/** Update the Skeletal and StaticMesh Components used to preview attachments in the editor. */
void WxAbilityViewer::RecreateSocketPreviews()
{
	//ClearSocketPreviews();

	//if(SelectedSkelMesh)
	//{
	//	FEditorAbilityViewportClient* ASVPreviewVC = GetPreviewVC();
	//	check( ASVPreviewVC );

	//	// Then iterate over Sockets creating component for any preview skeletal/static meshes.
	//	for(INT i=0; i<SelectedSkelMesh->Sockets.Num(); i++)
	//	{
	//		USkeletalMeshSocket* Socket = SelectedSkelMesh->Sockets(i);
	//		
	//		if(Socket && Socket->PreviewSkelMesh)
	//		{
	//			// Create SkeletalMeshComponent and fill in mesh and scene.
	//			USkeletalMeshComponent* NewSkelComp = ConstructObject<USkeletalMeshComponent>(USkeletalMeshComponent::StaticClass());
	//			NewSkelComp->SkeletalMesh = Socket->PreviewSkelMesh;

	//			// Attach component to this socket.
	//			PreviewSkelComp->AttachComponentToSocket(NewSkelComp, Socket->SocketName);

	//			// Assign SkelComp, so we can edit its properties.
	//			Socket->PreviewSkelComp = NewSkelComp;
	//			// Create AnimNodeSequence for previewing animation playback
	//			Socket->PreviewSkelComp->Animations = ConstructObject<UAnimNodeSequence>(UAnimNodeSequence::StaticClass());

	//			// And keep track of it.
	//			new( SocketPreviews )FASVSocketPreview(Socket, NewSkelComp);
	//		}

	//		if(Socket && Socket->PreviewStaticMesh)
	//		{
	//			// Create StaticMeshComponent and fill in mesh and scene.
	//			UStaticMeshComponent* NewMeshComp = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass());
	//			NewMeshComp->StaticMesh = Socket->PreviewStaticMesh;

	//			// Attach component to this socket.
	//			PreviewSkelComp->AttachComponentToSocket(NewMeshComp, Socket->SocketName);

	//			// And keep track of it
	//			new( SocketPreviews )FASVSocketPreview(Socket, NewMeshComp);
	//		}


	//		if(Socket && Socket->PreviewParticleSystem)
	//		{
	//			// Create UParticleSystemComponent and fill in template and scene.
	//			UParticleSystemComponent* NewPSysComp = ConstructObject<UParticleSystemComponent>(UParticleSystemComponent::StaticClass());
	//			NewPSysComp->SetTemplate(Socket->PreviewParticleSystem);
	//			NewPSysComp->SetTickGroup(TG_PostUpdateWork);

	//			// Attach component to this socket.
	//			PreviewSkelComp->AttachComponentToSocket(NewPSysComp, Socket->SocketName);

	//			// And keep track of it
	//			new( SocketPreviews )FASVSocketPreview(Socket, NewPSysComp);
	//		}
	//	}
	//}
}

/** Update the components being used to preview sockets. Need to call this when you change a Socket to see its affect. */
void WxAbilityViewer::UpdateSocketPreviews()
{
	//for(INT i=0; i<SocketPreviews.Num(); i++)
	//{
	//	PreviewSkelComp->DetachComponent( SocketPreviews(i).PreviewComp );
	//	PreviewSkelComp->AttachComponentToSocket( SocketPreviews(i).PreviewComp, SocketPreviews(i).Socket->SocketName );
	//}
}


/**
 *	This function returns the name of the docking parent.  This name is used for saving and loading the layout files.
 *  @return A string representing a name to use for this docking parent.
 */
const TCHAR* WxAbilityViewer::GetDockingParentName() const
{
	return TEXT("AnimSetViewer");
}

/**
 * @return The current version of the docking parent, this value needs to be increased every time new docking windows are added or removed.
 */
const INT WxAbilityViewer::GetDockingParentVersion() const
{
	return 0;
}

/**
 *	Update the preview window.
 */
void WxAbilityViewer::UpdatePreviewWindow()
{
	if (PreviewWindow)
	{
		if (PreviewWindow->ASVPreviewVC)
		{
			if (PreviewWindow->ASVPreviewVC->Viewport)
			{
				PreviewWindow->ASVPreviewVC->Viewport->Invalidate();
			}
		}
	}
}

/** 
 * Update Vertex Color Render Mode : for tangents/normals
 */
void WxAbilityViewer::UpdateVertexMode()
{
	//if ( PreviewSkelComp )
	//{
	//	PreviewSkelComp->ApplyColorRenderMode(VertexInfo.ColorOption);
	//	FComponentReattachContext PreviewSkelReattach(PreviewSkelComp);
	//	PreviewWindow->ASVPreviewVC->Invalidate();
	//}
}

/** Returns the current LOD index being viewed. */
INT WxAbilityViewer::GetCurrentLODIndex() const
{
	INT LODIndex = 0;
	if ( PreviewSkelComp )
	{
		if ( PreviewSkelComp->ForcedLodModel > 0 )
		{
			LODIndex = PreviewSkelComp->ForcedLodModel - 1;
		}
	}
	return LODIndex;
}

void WxAbilityViewer::ConvertAllAbilityTime()
{
	TArray<FString> list, list1,list2;
	FString dir = TEXT("..\\..\\MMOGame\\Content\\MMOContent\\06_Ability");

	appFindFilesInDirectory(list1, *dir, TRUE, FALSE);

	dir = TEXT("..\\..\\MMOGame\\Content\\MMOContent\\05.Archetype");
	appFindFilesInDirectory(list2, *dir, TRUE, FALSE);

	list.Append(list1);
	list.Append(list2);


	//for (INT i = 0; i < list.Num(); ++i)
	//{
	//	const FString& PackageName = FPackageFileCache::PackageFromPath(*list(i));
	//	UPackage* Package = FindObject<UPackage>(NULL, *PackageName, TRUE);
	//	if ( Package != NULL )
	//	{
	//		Package->FullyLoad();
	//	}
	//	else
	//	{
	//		Package = UObject::LoadPackage( NULL, *list(i), LOAD_None );
	//	}

	//	if( Package == NULL )
	//	{
	//		continue;
	//	}

	//	
	//	for (INT j = 0; j < Package->NetObjects.Num(); ++j)
	//	{
	//		UObject* obj = Package->NetObjects(j);
	//		UMMOAbility* abi = Cast<UMMOAbility>(obj);
	//		if(abi)
	//		{
	//			Package->MarkPackageDirty();

	//			for (INT k = 0; k < abi->SpellComponentList.Num(); ++k)
	//			{
	//				/*
	//				FLOAT spellTime = abi->GetAbilitySpellTime();
	//				FLOAT newTime = spellTime * abi->SpellComponentList(k).StartTime;
	//				abi->SpellComponentList(k).StartTime = newTime;
	//				*/

	//				/*
	//				if (abi->SpellComponentList(k).Component && abi->SpellComponentList(k).Component->IsA(UAbilityAnimationComponent::StaticClass()))
	//				{
	//					UAnimSequence* Seq = NULL;
	//					UAbilityAnimationComponent* Comp = Cast<UAbilityAnimationComponent>(abi->SpellComponentList(k).Component);
	//					for (INT j = 0; j < Comp->Anims.Num(); ++j)
	//					{
	//						Seq = GetAnimSeq(Comp->Anims(j));
	//						if (Seq)
	//						{
	//							break;
	//						}
	//					}
	//					if (!Seq)
	//					{
	//						continue;
	//					}

	//					if (Comp->AnimStartPoint > 0.001)
	//					{
	//						Comp->StartFrame = Seq->SequenceLength * Comp->AnimStartPoint * 30.f;
	//					}
	//				}
	//				*/
	//			}

	//			for (INT k = 0; k < abi->CastComponentList.Num(); ++k)
	//			{
	//				/*
	//				FLOAT CastTime = abi->GetAbilityCastTime();
	//				FLOAT NewTime = CastTime * abi->CastComponentList(k).StartTime;
	//				abi->CastComponentList(k).StartTime = NewTime;
	//				*/

	//				/*
	//				if (abi->CastComponentList(k).Component && abi->CastComponentList(k).Component->IsA(UAbilityAnimationComponent::StaticClass()))
	//				{
	//					UAnimSequence* Seq = NULL;
	//					UAbilityAnimationComponent* Comp = Cast<UAbilityAnimationComponent>(abi->CastComponentList(k).Component);
	//					for (INT j = 0; j < Comp->Anims.Num(); ++j)
	//					{
	//						Seq = GetAnimSeq(Comp->Anims(j));
	//						if (Seq)
	//						{
	//							break;
	//						}
	//					}
	//					if (!Seq)
	//					{
	//						continue;
	//					}

	//					if (Comp->AnimStartPoint > 0.001)
	//					{
	//						Comp->StartFrame = Seq->SequenceLength * Comp->AnimStartPoint * 30.f;
	//					}
	//				}
	//				*/
	//			}
	//		}
	//	}

	//}
}


void WxAbilityViewer::OnShowEffectNode(wxCommandEvent& In)
{
	SetItemCheckValue(In.GetId(), In.IsChecked());
}

void WxAbilityViewer::OnShowCalcSimulate(wxCommandEvent& In)
{
	SetItemCheckValue(IDM_SHOW_CALC_SIMULATE, In.IsChecked());
}

FString WxAbilityViewer::GetWeaponTypeName(BYTE InType)
{
	UEnum* WeaponTypeObj = FindObject<UEnum>(UTypeDefine::StaticClass(), TEXT("EWeaponType"), TRUE);
	check(WeaponTypeObj);

	FString EnumValueName = WeaponTypeObj->GetEnum(InType).ToString();
	const FString& AlternateEnumValueName = WeaponTypeObj->GetMetaData( TEXT("DisplayName"), InType );
	if ( AlternateEnumValueName.Len() == 0 )
	{
		FString LocalizedName = LocalizeProperty( *(WeaponTypeObj->GetName()), *(EnumValueName) );	

		if(LocalizedName.Len() > 0)
		{
			return LocalizedName;
		}
		else
		{
			return EnumValueName;
		}
	}
	else
	{
		FString LocalizedName = LocalizeProperty( *(WeaponTypeObj->GetName()), *(AlternateEnumValueName) );
		if(LocalizedName.Len() > 0)
		{
			return LocalizedName;
		}
		else
		{
			return AlternateEnumValueName;
		}
	}
}

void WxAbilityViewer::OnSelectWeaponType(wxCommandEvent& In)
{
	if (AbilityObject == NULL)
	{
		return;
	}

	std::vector<INT> TypeList;
	wxArrayString ArrayString;
	INT SelectIdx = 0;

	TypeList.push_back(AbilityObject->DefaultWeaponType);
	FString NewNode = GetWeaponTypeName(AbilityObject->DefaultWeaponType);
	ArrayString.Add(wxString(*NewNode));
	if (CurrentWeaponType == AbilityObject->DefaultWeaponType)
	{
		SelectIdx = 0;
	}
	
	for (INT i = 0; i < AbilityObject->OverrideComponentList.Num(); ++i)
	{
		FString NewNode = GetWeaponTypeName(AbilityObject->OverrideComponentList(i).Type);
		ArrayString.Add(wxString(*NewNode));
		TypeList.push_back(AbilityObject->OverrideComponentList(i).Type);

		if (AbilityObject->OverrideComponentList(i).Type == CurrentWeaponType)
		{
			SelectIdx = ArrayString.GetCount() - 1;
		}
	}

	wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(this, wxString(TEXT("Select a type to edit")), wxString(), ArrayString);
	scd->SetSelection(SelectIdx);
	int Ret = scd->ShowModal();
	if (Ret == wxID_OK)
	{
		INT idx = scd->GetSelection();
		if (CurrentWeaponType != TypeList[idx])
		{
			CurrentWeaponType = TypeList[idx];
			RefreshData();
		}
	}
	delete scd;
}

void WxAbilityViewer::OnAddWeaponType(wxCommandEvent& In)
{
	if (AbilityObject == NULL)
	{
		return;
	}

	std::vector<INT> TypeList;
	wxArrayString ArrayString;
	for (INT i = 0; i < EWT_MAX; ++i)
	{
		UBOOL bFind = FALSE;
		for (INT j = 0; j < AbilityObject->OverrideComponentList.Num(); ++j)
		{
			if (i == AbilityObject->OverrideComponentList(j).Type)
			{
				bFind = TRUE;
				break;
			}
		}
		if (i == CurrentWeaponType)
		{
			bFind = TRUE;
		}
		if (bFind == FALSE)
		{
			FString NewNode = GetWeaponTypeName(i);
			ArrayString.Add(wxString(*NewNode));
			TypeList.push_back(i);
		}
	}

	wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(this, wxString(TEXT("Select a type to Add")), wxString(), ArrayString);
	int Ret = scd->ShowModal();
	if (Ret == wxID_OK)
	{
		INT idx = scd->GetSelection();
		CurrentWeaponType = TypeList[idx];
		idx = AbilityObject->OverrideComponentList.AddZeroed();
		AbilityObject->OverrideComponentList(idx).Type = CurrentWeaponType;
		RefreshData();
	}

	delete scd;

}

void WxAbilityViewer::OnDeleteWeaponType(wxCommandEvent& In)
{
	if (AbilityObject == NULL)
	{
		return;
	}

	if (AbilityObject->OverrideComponentList.Num() == 0)
	{
		return;
	}

	std::vector<INT> TypeList;
	wxArrayString ArrayString;
	INT SelectIdx = 0;

	for (INT i = 0; i < AbilityObject->OverrideComponentList.Num(); ++i)
	{
		FString NewNode = GetWeaponTypeName(AbilityObject->OverrideComponentList(i).Type);
		ArrayString.Add(wxString(*NewNode));
		TypeList.push_back(AbilityObject->OverrideComponentList(i).Type);
		if (CurrentWeaponType == AbilityObject->OverrideComponentList(i).Type)
		{
			SelectIdx = i;
		}
	}

	wxSingleChoiceDialog* scd = new wxSingleChoiceDialog(this, wxString(TEXT("Select a type to delete")), wxString(), ArrayString);
	int Ret = scd->ShowModal();
	if (Ret == wxID_OK)
	{
		INT idx = scd->GetSelection();
		if (CurrentWeaponType == TypeList[idx])
		{
			CurrentWeaponType = 0;
		}

		for (INT i = 0; i < AbilityObject->OverrideComponentList.Num(); ++i)
		{
			if (AbilityObject->OverrideComponentList(i).Type == TypeList[idx])
			{
				AbilityObject->OverrideComponentList.Remove(i);
				break;
			}
		}
		RefreshData();
	}

	delete scd;

}

void WxAbilityViewer::OnCopyDefaultComponentsToCurrent(wxCommandEvent& In)
{
	if (!AbilityObject)
	{
		return;
	}

	if (CurrentWeaponType != AbilityObject->DefaultWeaponType)
	{
		for (INT i = 0; i < AbilityObject->OverrideComponentList.Num(); ++i)
		{
			if (CurrentWeaponType == AbilityObject->OverrideComponentList(i).Type)
			{
				AbilityObject->OverrideComponentList(i).OverrideSpellComponentList.Empty();
				AbilityObject->OverrideComponentList(i).OverrideCastComponentList.Empty();

				for (INT j = 0; j < AbilityObject->SpellComponentList.Num(); ++j)
				{
					if (AbilityObject->SpellComponentList(j).Component)
					{
						UAbilityComponent* NewNode = ConstructObject<UAbilityComponent>(AbilityObject->SpellComponentList(j).Component->GetClass(), AbilityObject, NAME_None, 0, AbilityObject->SpellComponentList(j).Component);
						if (NewNode)
						{
							INT idx = AbilityObject->OverrideComponentList(i).OverrideSpellComponentList.AddZeroed();
							AbilityObject->OverrideComponentList(i).OverrideSpellComponentList(idx).StartTime = AbilityObject->SpellComponentList(j).StartTime;
							AbilityObject->OverrideComponentList(i).OverrideSpellComponentList(idx).Component = NewNode;

							AbilityObject->SpellComponentList(j).Component->CopyObjectProperties(NewNode);
						}
					}
				}

				for (INT j = 0; j < AbilityObject->CastComponentList.Num(); ++j)
				{
					if (AbilityObject->CastComponentList(j).Component)
					{
						UAbilityComponent* NewNode = ConstructObject<UAbilityComponent>(AbilityObject->CastComponentList(j).Component->GetClass(), AbilityObject, NAME_None, 0, AbilityObject->CastComponentList(j).Component);
						if (NewNode)
						{
							INT idx = AbilityObject->OverrideComponentList(i).OverrideCastComponentList.AddZeroed();
							AbilityObject->OverrideComponentList(i).OverrideCastComponentList(idx).StartTime = AbilityObject->CastComponentList(j).StartTime;
							AbilityObject->OverrideComponentList(i).OverrideCastComponentList(idx).Component = NewNode;

							AbilityObject->CastComponentList(j).Component->CopyObjectProperties(NewNode);
						}
					}
				}
			}
		}

		RefreshData();
	}

}

void WxAbilityViewer::OnCopyDefaultSoundComponentsToCurrent(wxCommandEvent& In)
{
	if (!AbilityObject)
	{
		return;
	}

	if (CurrentWeaponType != AbilityObject->DefaultWeaponType)
	{
		for (INT i = 0; i < AbilityObject->OverrideComponentList.Num(); ++i)
		{
			if (CurrentWeaponType == AbilityObject->OverrideComponentList(i).Type)
			{
				for (INT j = 0; j < AbilityObject->SpellComponentList.Num(); ++j)
				{
					if (AbilityObject->SpellComponentList(j).Component && AbilityObject->SpellComponentList(j).Component->IsA(UAbilitySoundCueComponent::StaticClass()))
					{
						UAbilityComponent* NewNode = ConstructObject<UAbilityComponent>(AbilityObject->SpellComponentList(j).Component->GetClass(), AbilityObject, NAME_None, 0, AbilityObject->SpellComponentList(j).Component);
						if (NewNode)
						{
							INT idx = AbilityObject->OverrideComponentList(i).OverrideSpellComponentList.AddZeroed();
							AbilityObject->OverrideComponentList(i).OverrideSpellComponentList(idx).StartTime = AbilityObject->SpellComponentList(j).StartTime;
							AbilityObject->OverrideComponentList(i).OverrideSpellComponentList(idx).Component = NewNode;

							AbilityObject->SpellComponentList(j).Component->CopyObjectProperties(NewNode);
						}
					}
				}

				for (INT j = 0; j < AbilityObject->CastComponentList.Num(); ++j)
				{
					if (AbilityObject->CastComponentList(j).Component && AbilityObject->CastComponentList(j).Component->IsA(UAbilitySoundCueComponent::StaticClass()))
					{
						UAbilityComponent* NewNode = ConstructObject<UAbilityComponent>(AbilityObject->CastComponentList(j).Component->GetClass(), AbilityObject, NAME_None, 0, AbilityObject->CastComponentList(j).Component);
						if (NewNode)
						{
							INT idx = AbilityObject->OverrideComponentList(i).OverrideCastComponentList.AddZeroed();
							AbilityObject->OverrideComponentList(i).OverrideCastComponentList(idx).StartTime = AbilityObject->CastComponentList(j).StartTime;
							AbilityObject->OverrideComponentList(i).OverrideCastComponentList(idx).Component = NewNode;

							AbilityObject->CastComponentList(j).Component->CopyObjectProperties(NewNode);
						}
					}
				}
			}
		}

		RefreshData();
	}

}

void WxAbilityViewer::OnCopyComponent(wxCommandEvent& In)
{
	CurrentCopyedComponent = CurrentSelectedComponent;
}

void WxAbilityViewer::OnPasteComponent(wxCommandEvent& In)
{
	check(AbilityObject);
	if (CurrentCopyedComponent)
	{
		AbilityObject->AddComponent(CurrentCopyedComponent->GetClass(), bEditSpellState, CurrentWeaponType, CurrentCopyedComponent);
		AbilityObject->MarkPackageDirty();
		AbilityObject->PostEditChange();
	}
}

UBOOL WxAbilityViewer::IsShowItemChecked(INT Idx)
{
	if (Idx == IDM_SHOW_CALC_SIMULATE)
	{
		Idx = 0;
	}
	else
	{
		Idx = Idx + 1;
	}
	return DisplayFlag & (1<<Idx);
}
void WxAbilityViewer::SetItemCheckValue(INT Idx, UBOOL bChecked)
{
	if (Idx == IDM_SHOW_CALC_SIMULATE)
	{
		Idx = 0;
	}
	else
	{
		Idx = Idx + 1;
	}

	UINT Key = 1 << Idx;
	if (bChecked)
	{
		DisplayFlag |= Key;
	}
	else
	{
		Key = ~Key;
		DisplayFlag &= Key;
	}
}

UBOOL WxAbilityViewer::IsComponentNeedToDraw(UAbilityComponent* InComponent)
{
	if (!InComponent)
	{
		return FALSE;
	}

	for (TMap<INT, UClass*>::TIterator it(ClassMap); it; ++it)
	{
		UClass* tc = it.Value();
		if (tc == InComponent->GetClass())
		{
			return IsShowItemChecked(it.Key() + IDM_EFFECT_NODE_SHOW_START - IDM_EFFECT_NODE_ADD_START);
		}
	}

	return TRUE;
}

UPostProcessChain* FEditorAbilityViewportClient::GetDefaultPostProcessChain()
{
	if (!PreviewPostProcessChain)
	{
		PreviewPostProcessChain = ConstructObject<UPostProcessChain>(UPostProcessChain::StaticClass());
	}
	return PreviewPostProcessChain;

}
