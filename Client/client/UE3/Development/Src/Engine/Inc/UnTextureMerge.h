/*-----------------------------------------------------------------------------
* FTextureMerge
* ming@snda
-----------------------------------------------------------------------------*/

#ifndef __UNTEXTUREMERGE_H__
#define	__UNTEXTUREMERGE_H__

#include "UnThreadingWindows.h"

struct FTextureMergeRegion
{
	INT OffsetX;
	INT OffsetY;
	INT SizeX;
	INT SizeY;
	INT DestOffsetX;
	INT DestOffsetY;
	TArray<UTexture2D*> Textures;	// ordered by priority(layers)
};

typedef TArray<FTextureMergeRegion> TextureMergeList;
typedef TArray<FTexture2DRHIParamRef> TextureRHIList;

struct FBlendTextureRegion2D
{
	UINT OffsetX;
	UINT OffsetY;
	UINT SizeX;
	UINT SizeY;
	UINT DestOffsetX;
	UINT DestOffsetY;
	TextureRHIList	Textures;

	FBlendTextureRegion2D( UINT offsetX, UINT offsetY, UINT sizeX, UINT sizeY,
		UINT destOffsetX, UINT destOffsetY, const TextureRHIList& texs ) :
		OffsetX(offsetX), OffsetY(offsetY), SizeX(sizeX), SizeY(sizeY),
		DestOffsetX(destOffsetX), DestOffsetY(destOffsetY), Textures(texs) {}
};

//struct FTextureMergeUnit
//{
//	const TextureMergeList&	SourceRegions;
//	UTexture2D*			TargetTexture;
//
//	FTextureMergeUnit( const TextureMergeList& regions, UTexture2D* tex ) :
//		SourceRegions( regions ), TargetTexture( tex ) {}
//};

struct FTextureLockData
{
	//FTexture2DResource* TextureResource;
	// use UTexture2D* as key, to prevent it from GC
	UTexture2D*	TextureKey;
	BYTE*	Data[10];
	INT	RefCount[10];

	FTextureLockData( UTexture2D* tex ) :
	TextureKey( tex )
	{
		appMemzero( Data, sizeof(Data) );
		appMemzero( RefCount, sizeof(RefCount) );
	}

	~FTextureLockData()
	{
		for( int i = 0 ; i < 10 ; ++i )
		{
			if ( Data[i] != NULL )
			{
				delete[] Data[i];
			}
		}
	}
};

typedef TArray< FTextureLockData* >	TextureLockDataList;

class FTextureMergeUtil
{
public:
	FTextureMergeUtil();
	~FTextureMergeUtil();

	UBOOL DoMerge();

	// hard coded, this class needs not to be used by other modules
	TextureMergeList	SourceRegions_D;	// diffuse source
	TextureMergeList	SourceRegions_N;	// normal source
	TextureMergeList	SourceRegions_S;	// r: specular, g: alpha, b: emmisive

	UTexture2DDynamic*	TargetTexture_D;
	UTexture2DDynamic*	TargetTexture_N;
	UTexture2DDynamic*	TargetTexture_S;

	//UINT AlphaIndex;	// the index in the texture array of alpha data
	//UINT AlphaChannel;	// 1:r 2:g 3:b

	void UpdateState();

	UBOOL MergeFinished()
	{
		return bFinished;
	}

	void RenderThread_BlendRegions();

	void RenderThread_StartMerge();
	void RenderThread_MergeOneTexBegin();
	void RenderThread_MergeOneTexEnd();
	void RenderThread_FinishMerge();

	void BackgroundThread_MergeTextures();

	UBOOL IsDoingMerge()
	{
		return bDoingMerge;
	}

	void CancelCurrentMerge();

private:
	UBOOL Validate();

	void StartNewMerge( UBOOL UseBackgroundMerge = TRUE );

	// start a background merging thread
	void StartMergingThread();

	UBOOL LockTargetTextureData();

	//void AllocSrcData();

	UBOOL GetScrDataFromTex( FTextureLockData *&pTexLockData, FTexture2DResource *&tex2DRes, INT &mipID );
	void CheckLockDataRefNum( FTextureLockData *&pTexLockData, INT &mipID, TextureLockDataList &datalist );

	//UBOOL bMergeTaskFinished;
	UBOOL bFinished;

	// can't start merging thread right away, waiting
	UBOOL bWaiting;

	// true if background merging thread is running for this util
	UBOOL bBackgroundMerging;

	FEventWin	MergeFinishEvent;

	UBOOL bDoingMerge;
	UBOOL bCancelCurrentMerge;

	BYTE*	DstData_D;
	BYTE*	DstData_N;
	BYTE*	DstData_S;
	BYTE*	SrcData_D;
	BYTE*	SrcData_N;
	BYTE*	SrcData_S;

	TextureMergeList	CurrentSourceRegions_D;
	TextureMergeList	CurrentSourceRegions_N;
	TextureMergeList	CurrentSourceRegions_S;

	UTexture2DDynamic*	CurrentTargetTexture_D;
	UTexture2DDynamic*	CurrentTargetTexture_N;
	UTexture2DDynamic*	CurrentTargetTexture_S;

	INT FirstMip;
	INT MipIdx;	// todo: merge all mip levels
	INT RegionIdx;
	INT	LayerIdx;
	INT BlockSizeX;
	INT BlockSizeY;
	INT BlockBytes;
	INT DestMipSizeX;
	INT DestMipSizeY;
	INT SrcMipSizeX;
	INT SrcMipSizeY;
	INT SrcRegionOffsetX;
	INT SrcRegionOffsetY;
	INT DestRegionOffsetX;
	INT DestRegionOffsetY;
	INT RegionSizeX;
	INT RegionSizeY;
	DWORD SrcPitchBytes;
	DWORD SrcRowOffsetBytes;
	UINT DestPitchBytes_D;
	UINT DestPitchBytes_N;
	UINT DestPitchBytes_S;
	DWORD DestRowOffsetBytes;
	DWORD BlockPerRow;
	FTextureMergeRegion* Region;
	FTexture2DRHIParamRef TexTargetD;
	FTexture2DRHIParamRef TexTargetN;
	FTexture2DRHIParamRef TexTargetS;
	FTexture2DResource* SrcTex2DResource_D;
	FTexture2DResource*	SrcTex2DResource_N;
	FTexture2DResource* SrcTex2DResource_S;

	// gpu merge don't need these since textures won't be locked twice
	//INT SrcLockIdx_D, SrcLockIdx_N, SrcLockIdx_S;
	FTextureLockData*	SrcLockData_D;
	FTextureLockData*	SrcLockData_N;
	FTextureLockData*	SrcLockData_S;

	// keep track of locked textures to avoid relock
	static TextureLockDataList	LockData_D;
	static TextureLockDataList	LockData_N;
	static TextureLockDataList	LockData_S;

	// the max # of merging threads allowed to run at the same time
	// set to 0 if don't want to restrict the max number
	static const UINT MaxMergeThreadNum = 0;	// not working very well, disabled for now
	// the number of merging thread that is currently running
	static UINT	CurrentRunningThreadNum;

	FTextureLockData* GetLockData_D( const UTexture2D* tr, INT& idx );
	FTextureLockData* GetLockData_N( const UTexture2D* tr, INT& idx );
	FTextureLockData* GetLockData_S( const UTexture2D* tr, INT& idx );

	//void RenderThread_BlendRegions( const FTextureMergeUnit& mergeD, 
	//	const FTextureMergeUnit& mergeN, const FTextureMergeUnit& mergeS );
};

class FTextureMergeTask : public FNonAbandonableTask
{
public:
	FTextureMergeTask( FTextureMergeUtil* InMergeUtil ) :
	  MergeUtil( InMergeUtil )
	{}


	void DoWork()
	{
		MergeUtil->BackgroundThread_MergeTextures();
	}

	static const TCHAR *Name()
	{
		return TEXT("FTextureMergeTask");
	}
 
private:
	FTextureMergeUtil* MergeUtil;
};

#endif // __UNTEXTUREMERGE_H__