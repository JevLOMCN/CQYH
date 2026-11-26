/*-----------------------------------------------------------------------------
* FTextureMerge
* ming@snda
-----------------------------------------------------------------------------*/

#include "EnginePrivate.h"
#include "UnTextureMerge.h"

#include "squish.h"

//#define OutputDebugInfo

TextureLockDataList FTextureMergeUtil::LockData_D;
TextureLockDataList FTextureMergeUtil::LockData_N;
TextureLockDataList FTextureMergeUtil::LockData_S;

UINT FTextureMergeUtil::CurrentRunningThreadNum = 0;

static UBOOL GetTextureMergeBufferLayout(INT Width, INT Height, UINT& RowBytes, SIZE_T& Rows)
{
	RowBytes = 0;
	Rows = 0;
	if( Width <= 0 || Height <= 0 )
		return FALSE;
	SIZE_T Blocks = (SIZE_T(Width) + 3) / 4;
	Rows = (SIZE_T(Height) + 3) / 4;
	if( Blocks > UINT(-1) / 8 || Blocks > SIZE_T(-1) / 8 )
		return FALSE;
	RowBytes = UINT(Blocks * 8);
	return RowBytes <= SIZE_T(-1) / Rows;
}

// Clip block-aligned copies to the space remaining after both offsets.
static void GetTextureMergeCopyBounds(const FTextureMergeRegion& InRegion, INT InMip,
	INT SrcWidth, INT SrcHeight, INT DstWidth, INT DstHeight, INT BlockX, INT BlockY,
	INT& SrcX, INT& SrcY, INT& DstX, INT& DstY, INT& Width, INT& Height)
{
	SrcX = SrcY = DstX = DstY = Width = Height = 0;
	if( InMip < 0 || InMip >= 31 || BlockX <= 0 || BlockY <= 0 ||
		SrcWidth < BlockX || SrcHeight < BlockY || DstWidth < BlockX || DstHeight < BlockY )
		return;

	SrcX = (Clamp(Max(0, InRegion.OffsetX) >> InMip, 0, SrcWidth - BlockX) / BlockX) * BlockX;
	SrcY = (Clamp(Max(0, InRegion.OffsetY) >> InMip, 0, SrcHeight - BlockY) / BlockY) * BlockY;
	INT RequestedDstX = InRegion.DestOffsetX < 0 ? SrcX : InRegion.DestOffsetX >> InMip;
	INT RequestedDstY = InRegion.DestOffsetY < 0 ? SrcY : InRegion.DestOffsetY >> InMip;
	DstX = (Clamp(RequestedDstX, 0, DstWidth - BlockX) / BlockX) * BlockX;
	DstY = (Clamp(RequestedDstY, 0, DstHeight - BlockY) / BlockY) * BlockY;

	INT AvailableX = Min(SrcWidth - SrcX, DstWidth - DstX) / BlockX;
	INT AvailableY = Min(SrcHeight - SrcY, DstHeight - DstY) / BlockY;
	if( InRegion.SizeX == -1 )
		Width = AvailableX * BlockX;
	else if( InRegion.SizeX > 0 )
	{
		INT Requested = Min(Max(1, InRegion.SizeX >> InMip), AvailableX * BlockX);
		Width = Min(AvailableX, Requested / BlockX + (Requested % BlockX != 0)) * BlockX;
	}
	if( InRegion.SizeY == -1 )
		Height = AvailableY * BlockY;
	else if( InRegion.SizeY > 0 )
	{
		INT Requested = Min(Max(1, InRegion.SizeY >> InMip), AvailableY * BlockY);
		Height = Min(AvailableY, Requested / BlockY + (Requested % BlockY != 0)) * BlockY;
	}
}

FTextureMergeUtil::FTextureMergeUtil() :
	TargetTexture_D(NULL),
	TargetTexture_N(NULL),
	TargetTexture_S(NULL),
	bFinished(FALSE),
	bWaiting(FALSE),
	bBackgroundMerging(FALSE),
	bDoingMerge(FALSE),
	bCancelCurrentMerge(FALSE),
	DstData_D(NULL),
	DstData_N(NULL),
	DstData_S(NULL),
	SrcData_D(NULL),
	SrcData_N(NULL),
	SrcData_S(NULL),
	CurrentTargetTexture_D(NULL),
	CurrentTargetTexture_N(NULL),
	CurrentTargetTexture_S(NULL),
	FirstMip(0),
	MipIdx(0),
	RegionIdx(0),
	LayerIdx(0),
	BlockSizeX(0),
	BlockSizeY(0),
	BlockBytes(0),
	DestMipSizeX(0),
	DestMipSizeY(0),
	SrcMipSizeX(0),
	SrcMipSizeY(0),
	SrcRegionOffsetX(0),
	SrcRegionOffsetY(0),
	DestRegionOffsetX(0),
	DestRegionOffsetY(0),
	RegionSizeX(0),
	RegionSizeY(0),
	SrcPitchBytes(0),
	SrcRowOffsetBytes(0),
	DestPitchBytes_D(0),
	DestPitchBytes_N(0),
	DestPitchBytes_S(0),
	DestRowOffsetBytes(0),
	BlockPerRow(0),
	Region(NULL),
	TexTargetD(NULL),
	TexTargetN(NULL),
	TexTargetS(NULL),
	SrcTex2DResource_D(NULL),
	SrcTex2DResource_N(NULL),
	SrcTex2DResource_S(NULL),
	SrcLockData_D(NULL),
	SrcLockData_N(NULL),
	SrcLockData_S(NULL)
{
	MergeFinishEvent.Create();
}

FTextureMergeUtil::~FTextureMergeUtil()
{
	if( bDoingMerge && bBackgroundMerging )
	{
		// free the thread resource
		--CurrentRunningThreadNum;

		if( MergeFinishEvent.Wait(-1) )
		{
			// clean up source textures
			if( SrcLockData_D && SrcTex2DResource_D )
			{
				CheckLockDataRefNum( SrcLockData_D, MipIdx, LockData_D );
// 				--SrcLockData_D->RefCount;
// 				if( SrcLockData_D->RefCount <= 0 )
// 				{
// 					// unlock textures...
// 					ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
// 						UnlockD,
// 						FTexture2DResource*, TextureResource, SrcTex2DResource_D,
// 						{
// 							if( TextureResource && TextureResource->GetTexture2DRHI() )
// 								RHIUnlockTexture2D( TextureResource->GetTexture2DRHI(), 0, FALSE );
// 						} );
// 					LockData_D.RemoveSingleItem( SrcLockData_D );
// 				}
			}

			if( SrcLockData_N && SrcTex2DResource_N )
			{
				CheckLockDataRefNum( SrcLockData_N, MipIdx, LockData_N );
// 				--SrcLockData_N->RefCount;
// 				if( SrcLockData_N->RefCount <= 0 )
// 				{
// 					// unlock textures...
// 					ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
// 						UnlockN,
// 						FTexture2DResource*, TextureResource, SrcTex2DResource_N,
// 						{
// 							if( TextureResource && TextureResource->GetTexture2DRHI() )
// 								RHIUnlockTexture2D( TextureResource->GetTexture2DRHI(), 0, FALSE );
// 						} );
// 					LockData_N.RemoveSingleItem( SrcLockData_N );
// 				}
			}

			if( SrcLockData_S && SrcTex2DResource_S )
			{
				CheckLockDataRefNum( SrcLockData_S, MipIdx, LockData_S );
// 				--SrcLockData_S->RefCount;
// 				if( SrcLockData_S->RefCount <= 0 )
// 				{
// 					// unlock textures...
// 					ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
// 						UnlockS,
// 						FTexture2DResource*, TextureResource, SrcTex2DResource_S,
// 						{
// 							if( TextureResource && TextureResource->GetTexture2DRHI() )
// 								RHIUnlockTexture2D( TextureResource->GetTexture2DRHI(), 0, FALSE );
// 						} );
// 					LockData_S.RemoveSingleItem( SrcLockData_S );
// 				}
			}

			// also release target textures
			//RHIUnlockTexture2D( TexTargetD, MipIdx, FALSE );
			//RHIUnlockTexture2D( TexTargetN, MipIdx, FALSE );
			//RHIUnlockTexture2D( TexTargetS, MipIdx, FALSE );
		}
	}
}

UBOOL FTextureMergeUtil::Validate()
{
	INT Drop = GSystemSettings.GetNumMipsToDrop();
	if( Drop < 0 || Drop >= 10 || !TargetTexture_D || !TargetTexture_N || !TargetTexture_S )
		return FALSE;
	UTexture2DDynamic* Targets[] = {TargetTexture_D, TargetTexture_N, TargetTexture_S};
	for( INT Channel = 0; Channel < ARRAY_COUNT(Targets); ++Channel )
	{
		UTexture2DDynamic* Target = Targets[Channel];
		UINT RowBytes;
		SIZE_T Rows;
		if( !Target->Resource || Target->Format != PF_DXT1 || Target->NumMips <= 0 ||
			Target->NumMips > 10 - Drop || Target->NumMips != TargetTexture_D->NumMips ||
			Target->SizeX != TargetTexture_D->SizeX || Target->SizeY != TargetTexture_D->SizeY ||
			!GetTextureMergeBufferLayout(Target->SizeX, Target->SizeY, RowBytes, Rows) )
			return FALSE;
	}
	if( SourceRegions_D.Num() == 0 || SourceRegions_D.Num() != SourceRegions_N.Num() ||
		SourceRegions_D.Num() != SourceRegions_S.Num() )
		return FALSE;
	UBOOL HasSource = FALSE;
	for( INT i = 0; i < SourceRegions_D.Num(); ++i )
	{
		INT Count = SourceRegions_D(i).Textures.Num();
		if( SourceRegions_N(i).Textures.Num() != Count || SourceRegions_S(i).Textures.Num() != Count )
			return FALSE;
		if( Count == 0 )
		{
			if( i == 0 || i == 5 )
				continue;
			return FALSE;
		}
		HasSource = TRUE;
		for( INT j = 0; j < Count; ++j )
		{
			UTexture2D* Textures[] = {SourceRegions_D(i).Textures(j),
				SourceRegions_N(i).Textures(j), SourceRegions_S(i).Textures(j)};
			for( INT Channel = 0; Channel < ARRAY_COUNT(Textures); ++Channel )
			{
				UTexture2D* Texture = Textures[Channel];
				UINT RowBytes;
				SIZE_T Rows;
				if( !Texture || !Texture->Resource || Texture->Format != PF_DXT1 ||
					!Texture->IsFullyStreamedIn() || Texture->ResidentMips <= 0 || Texture->ResidentMips > Texture->Mips.Num() ||
					Texture->Mips.Num() - Texture->ResidentMips > Drop ||
					Texture->Mips.Num() < Drop + TargetTexture_D->NumMips ||
					Texture->SizeX != Textures[0]->SizeX || Texture->SizeY != Textures[0]->SizeY ||
					!GetTextureMergeBufferLayout(Texture->SizeX, Texture->SizeY, RowBytes, Rows) )
					return FALSE;
			}
		}
	}
	return HasSource;
}

void FTextureMergeUtil::StartNewMerge( UBOOL UseBackgroundMerge )
{
	FirstMip = GSystemSettings.GetNumMipsToDrop();
	bDoingMerge = TRUE;
	bFinished = FALSE;
	bCancelCurrentMerge = FALSE;
	bWaiting = FALSE;

	CurrentSourceRegions_D = SourceRegions_D;
	CurrentSourceRegions_N = SourceRegions_N;
	CurrentSourceRegions_S = SourceRegions_S;
	CurrentTargetTexture_D = TargetTexture_D;
	CurrentTargetTexture_N = TargetTexture_N;
	CurrentTargetTexture_S = TargetTexture_S;

	if( UseBackgroundMerge )
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			MergeTextures_CPU,
			FTextureMergeUtil*, Texture, this,
			{
				Texture->RenderThread_StartMerge();
			} );
	}
	else
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			MergeTextures_GPU,
			FTextureMergeUtil*, Texture, this,
			{
				Texture->RenderThread_BlendRegions();
			} );
	}
}

void FTextureMergeUtil::CancelCurrentMerge()
{
	if( !bCancelCurrentMerge && bDoingMerge )
	{
		if( bWaiting )
		{	// haven't start the thread yet, cancel here
			// no need to wait for starting new thread
			bWaiting = FALSE;
			// let rendering thread to finish rest work
			MergeFinishEvent.Trigger();
		}
		else
		{
			bCancelCurrentMerge = TRUE;
		}
	}
}

UBOOL FTextureMergeUtil::DoMerge()
{
	if( !Validate() )
		return FALSE;

	if( !bDoingMerge )
	{
		StartNewMerge( TRUE );
	}
	else
	{	// stop last merge
		// todo: allow cancel here
		//if( !bCancelCurrentMerge )
		//	CancelCurrentMerge();
		return FALSE;
	}

	return TRUE;
}

struct A8R8G8B8
{
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
};

struct DXT1Tile
{
	WORD color1;
	WORD color2;
	WORD index1;
	WORD index2;
};

void DecompressDXT1Tile( const DXT1Tile* srcTile, A8R8G8B8* dstTile )
{
	squish::Decompress( (squish::u8*)dstTile, srcTile, squish::kDxt1 );
}

void CompressDXT1Tile( const A8R8G8B8* srcTile, DXT1Tile* dstTile )
{
	squish::Compress( (squish::u8*)srcTile, dstTile, squish::kDxt1 );
}

void BlendTile_CPU( 
	const DXT1Tile* srcTile_D, DXT1Tile* dstTile_D,
	const DXT1Tile* srcTile_N, DXT1Tile* dstTile_N,
	const DXT1Tile* srcTile_S, DXT1Tile* dstTile_S )
{
	A8R8G8B8 srcColor_D[16];
	A8R8G8B8 srcColor_N[16];
	A8R8G8B8 srcColor_S[16];

	A8R8G8B8 dstColor_D[16];
	A8R8G8B8 dstColor_N[16];
	A8R8G8B8 dstColor_S[16];

	DecompressDXT1Tile( srcTile_D, srcColor_D );
	DecompressDXT1Tile( srcTile_N, srcColor_N );
	DecompressDXT1Tile( srcTile_S, srcColor_S );

	DecompressDXT1Tile( dstTile_D, dstColor_D );
	DecompressDXT1Tile( dstTile_N, dstColor_N );
	DecompressDXT1Tile( dstTile_S, dstColor_S );

	for( UINT i = 0; i < 16; ++i )
	{
		float alpha = (float)srcColor_S[i].g / 255.f;

		if( srcColor_S[i].g == 255 )
		{
			dstColor_D[i].r = srcColor_D[i].r;
			dstColor_D[i].g = srcColor_D[i].g;
			dstColor_D[i].b = srcColor_D[i].b;

			dstColor_S[i].r = srcColor_S[i].r;
			dstColor_S[i].g = srcColor_S[i].g;
			dstColor_S[i].b = 0xFF;	// skin mask
		}
		else if( srcColor_S[i].g == 0 )
		{
			//dstColor_D[i].r = dstColor_D[i].r;
			//dstColor_D[i].g = dstColor_D[i].g;
			//dstColor_D[i].b = dstColor_D[i].b;

			//dstColor_S[i].r = dstColor_S[i].r;
			//dstColor_S[i].g = dstColor_S[i].g;
			//dstColor_S[i].b = 0x00;	// skin mask
		}
		else
		{
			float invAlpha = 1.f - alpha;

			dstColor_D[i].r = dstColor_D[i].r * invAlpha + srcColor_D[i].r * alpha;
			dstColor_D[i].g = dstColor_D[i].g * invAlpha + srcColor_D[i].g * alpha;
			dstColor_D[i].b = dstColor_D[i].b * invAlpha + srcColor_D[i].b * alpha;

			dstColor_S[i].r = dstColor_S[i].r * invAlpha + srcColor_S[i].r * alpha;
			dstColor_S[i].g = dstColor_S[i].g | srcColor_S[i].g;
			dstColor_S[i].b = srcColor_S[i].g; // skin mask
		}
		dstColor_D[i].a = 0xFF;
		dstColor_S[i].a = 0xFF;

		if( alpha > 0.5f )
		{
			dstColor_N[i].r = srcColor_N[i].r;
			dstColor_N[i].g = srcColor_N[i].g;
			dstColor_N[i].b = srcColor_N[i].b;
			dstColor_N[i].a = 0xFF;
		}
		else
		{
			dstColor_N[i].r = dstColor_N[i].r;
			dstColor_N[i].g = dstColor_N[i].g;
			dstColor_N[i].b = dstColor_N[i].b;
			dstColor_N[i].a = 0xFF;
		}
		/*dstColor_N[i].r = dstColor_N[i].r * invAlpha + srcColor_N[i].r * alpha;
		dstColor_N[i].g = dstColor_N[i].g * invAlpha + srcColor_N[i].g * alpha;
		dstColor_N[i].b = dstColor_N[i].b * invAlpha + srcColor_N[i].b * alpha;
		dstColor_N[i].a = 0xFF;*/
	}

	CompressDXT1Tile( dstColor_D, dstTile_D );
	CompressDXT1Tile( dstColor_N, dstTile_N );
	CompressDXT1Tile( dstColor_S, dstTile_S );
}

// gpu merge can't be cancelled
void FTextureMergeUtil::RenderThread_BlendRegions()
{
	TexTargetD = ((FTexture2DDynamicResource*)CurrentTargetTexture_D->Resource)->GetTexture2DRHI();
	TexTargetN = ((FTexture2DDynamicResource*)CurrentTargetTexture_N->Resource)->GetTexture2DRHI();
	TexTargetS = ((FTexture2DDynamicResource*)CurrentTargetTexture_S->Resource)->GetTexture2DRHI();

	INT NumMips = CurrentTargetTexture_D->NumMips;

	INT Format = PF_DXT1;
	BlockSizeX = GPixelFormats[Format].BlockSizeX;
	BlockSizeY = GPixelFormats[Format].BlockSizeY;
	BlockBytes = GPixelFormats[Format].BlockBytes;

	INT BaseSizeX = CurrentTargetTexture_D->SizeX;
	INT BaseSizeY = CurrentTargetTexture_D->SizeY;

	// process each mip level
	for( MipIdx=FirstMip; MipIdx < FirstMip + NumMips; MipIdx++ )
	{
		if( !LockTargetTextureData() )
			return;

		// list of source regions that need to be copied for this mip level
		TArray<FBlendTextureRegion2D*> BlendRegions;

		for( RegionIdx=0; RegionIdx < CurrentSourceRegions_D.Num(); ++RegionIdx )		
		{
			Region = &CurrentSourceRegions_D(RegionIdx);

			TextureRHIList texList;

			for( LayerIdx = 0; LayerIdx < Region->Textures.Num(); ++LayerIdx )
			{
				UTexture2D* SrcTexture = Region->Textures(LayerIdx);
				SrcTex2DResource_D = (FTexture2DResource*)SrcTexture->Resource;
				//if( SrcTex2DResource_D && 
				//	SrcTex2DResource_D->IsInitialized() &&
				//	Region->Textures(LayerIdx)->IsFullyStreamedIn() &&
				//	Region->Textures(LayerIdx)->ResidentMips == Region->Textures(LayerIdx)->RequestedMips &&
				//	Region->Textures(LayerIdx)->Mips.IsValidIndex(MipIdx) )
				//{
				//	texList.AddItem( SrcTex2DResource_D->GetTexture2DRHI() );
				//}

				SrcTex2DResource_N = (FTexture2DResource*)CurrentSourceRegions_N(RegionIdx).Textures(LayerIdx)->Resource;
				SrcTex2DResource_S = (FTexture2DResource*)CurrentSourceRegions_S(RegionIdx).Textures(LayerIdx)->Resource;

				UINT SrcStrideD = 0, SrcStrideN = 0, SrcStrideS = 0;
				INT SrcMipD = MipIdx - (SrcTexture->Mips.Num() - SrcTexture->ResidentMips);
				UTexture2D* SrcTextureN = CurrentSourceRegions_N(RegionIdx).Textures(LayerIdx);
				UTexture2D* SrcTextureS = CurrentSourceRegions_S(RegionIdx).Textures(LayerIdx);
				INT SrcMipN = MipIdx - (SrcTextureN->Mips.Num() - SrcTextureN->ResidentMips);
				INT SrcMipS = MipIdx - (SrcTextureS->Mips.Num() - SrcTextureS->ResidentMips);
				SrcData_D = SrcTex2DResource_D->GetTexture2DRHI() ? (BYTE*)RHILockTexture2D( SrcTex2DResource_D->GetTexture2DRHI(), SrcMipD, FALSE, SrcStrideD, FALSE ) : NULL;
				SrcData_N = SrcTex2DResource_N->GetTexture2DRHI() ? (BYTE*)RHILockTexture2D( SrcTex2DResource_N->GetTexture2DRHI(), SrcMipN, FALSE, SrcStrideN, FALSE ) : NULL;
				SrcData_S = SrcTex2DResource_S->GetTexture2DRHI() ? (BYTE*)RHILockTexture2D( SrcTex2DResource_S->GetTexture2DRHI(), SrcMipS, FALSE, SrcStrideS, FALSE ) : NULL;
				UINT RequiredPitch;
				SIZE_T RequiredRows;
				UBOOL LayoutValid = GetTextureMergeBufferLayout(Max(4, SrcTexture->SizeX >> MipIdx),
					Max(4, SrcTexture->SizeY >> MipIdx), RequiredPitch, RequiredRows);
				if( !LayoutValid || !SrcData_D || !SrcData_N || !SrcData_S || SrcStrideD < RequiredPitch ||
					SrcStrideN < RequiredPitch || SrcStrideS < RequiredPitch ||
					RequiredRows > SIZE_T(-1) / SrcStrideD || RequiredRows > SIZE_T(-1) / SrcStrideN ||
					RequiredRows > SIZE_T(-1) / SrcStrideS )
				{
					if( SrcData_D ) RHIUnlockTexture2D(SrcTex2DResource_D->GetTexture2DRHI(), SrcMipD, FALSE);
					if( SrcData_N ) RHIUnlockTexture2D(SrcTex2DResource_N->GetTexture2DRHI(), SrcMipN, FALSE);
					if( SrcData_S ) RHIUnlockTexture2D(SrcTex2DResource_S->GetTexture2DRHI(), SrcMipS, FALSE);
					bCancelCurrentMerge = TRUE;
					RenderThread_FinishMerge();
					return;
				}

				// Calculate source values
				INT SrcSizeX = SrcTexture->SizeX >> MipIdx;
				INT SrcSizeY = SrcTexture->SizeY >> MipIdx;
				SrcMipSizeX = Max((INT)GPixelFormats[Format].BlockSizeX,SrcSizeX);
				SrcMipSizeY = Max((INT)GPixelFormats[Format].BlockSizeY,SrcSizeY);

				GetTextureMergeCopyBounds(*Region, MipIdx, SrcMipSizeX, SrcMipSizeY,
					DestMipSizeX, DestMipSizeY, BlockSizeX, BlockSizeY,
					SrcRegionOffsetX, SrcRegionOffsetY, DestRegionOffsetX, DestRegionOffsetY, RegionSizeX, RegionSizeY);

				// size in bytes of an entire row for this mip
				SrcPitchBytes = UINT((SIZE_T(SrcMipSizeX) + BlockSizeX - 1) / BlockSizeX) * BlockBytes;

				// size in bytes of the offset to the starting part of the row to copy for this mip
				SrcRowOffsetBytes = UINT(SrcRegionOffsetX / BlockSizeX) * BlockBytes;
				DestRowOffsetBytes = UINT(DestRegionOffsetX / BlockSizeX) * BlockBytes;

				// block number of each row
				BlockPerRow = RegionSizeX / BlockSizeX;

				DXT1Tile* srcTile_D, * srcTile_N, * srcTile_S;
				DXT1Tile* dstTile_D, * dstTile_N, * dstTile_S;

				INT CurDestOffsetY = DestRegionOffsetY;
				if( LayerIdx == 0 )
				{	// copy by row
					DWORD RowSizeBytes = UINT(RegionSizeX / BlockSizeX) * BlockBytes;
					for( INT CurSrcOffsetY = SrcRegionOffsetY; CurSrcOffsetY < (SrcRegionOffsetY + RegionSizeY); CurSrcOffsetY += BlockSizeY )
					{
						INT CurSrcBlockOffsetY = CurSrcOffsetY / BlockSizeY;
						INT CurDestBlockOffsetY = CurDestOffsetY / BlockSizeY;

						BYTE* SrcOffset = SrcData_D + (SIZE_T(CurSrcBlockOffsetY) * SrcStrideD) + SrcRowOffsetBytes;
						BYTE* DstOffset = DstData_D + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_D) + DestRowOffsetBytes;
						appMemcpy( DstOffset, SrcOffset, RowSizeBytes );
						SrcOffset = SrcData_N + (SIZE_T(CurSrcBlockOffsetY) * SrcStrideN) + SrcRowOffsetBytes;
						DstOffset = DstData_N + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_N) + DestRowOffsetBytes;
						appMemcpy( DstOffset, SrcOffset, RowSizeBytes );
						SrcOffset = SrcData_S + (SIZE_T(CurSrcBlockOffsetY) * SrcStrideS) + SrcRowOffsetBytes;
						DstOffset = DstData_S + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_S) + DestRowOffsetBytes;
						appMemcpy( DstOffset, SrcOffset, RowSizeBytes );

						CurDestOffsetY += BlockSizeY;
					}
				}
				else
				{	// do blend per block
					for( INT CurSrcOffsetY=SrcRegionOffsetY; CurSrcOffsetY < (SrcRegionOffsetY+RegionSizeY); CurSrcOffsetY += BlockSizeY )
					{
						INT CurSrcBlockOffsetY = CurSrcOffsetY / BlockSizeY;
						INT CurDestBlockOffsetY = CurDestOffsetY / BlockSizeY;

						BYTE* SrcOffset_D = SrcData_D + (SIZE_T(CurSrcBlockOffsetY) * SrcStrideD) + SrcRowOffsetBytes;
						BYTE* DstOffset_D = DstData_D + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_D) + DestRowOffsetBytes;
						BYTE* SrcOffset_N = SrcData_N + (SIZE_T(CurSrcBlockOffsetY) * SrcStrideN) + SrcRowOffsetBytes;
						BYTE* DstOffset_N = DstData_N + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_N) + DestRowOffsetBytes;
						BYTE* SrcOffset_S = SrcData_S + (SIZE_T(CurSrcBlockOffsetY) * SrcStrideS) + SrcRowOffsetBytes;
						BYTE* DstOffset_S = DstData_S + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_S) + DestRowOffsetBytes;
						//appMemcpy( DstOffset, SrcOffset, RowSizeBytes );

						for( UINT blockIdx = 0; blockIdx < BlockPerRow; ++blockIdx )
						{
							srcTile_D = (DXT1Tile*)( SrcOffset_D + blockIdx * BlockBytes );
							srcTile_N = (DXT1Tile*)( SrcOffset_N + blockIdx * BlockBytes );
							srcTile_S = (DXT1Tile*)( SrcOffset_S + blockIdx * BlockBytes );
							dstTile_D = (DXT1Tile*)( DstOffset_D + blockIdx * BlockBytes );
							dstTile_N = (DXT1Tile*)( DstOffset_N + blockIdx * BlockBytes );
							dstTile_S = (DXT1Tile*)( DstOffset_S + blockIdx * BlockBytes );

							BlendTile_CPU( srcTile_D, dstTile_D, srcTile_N, dstTile_N, srcTile_S, dstTile_S );

							
						}
						CurDestOffsetY += BlockSizeY;
				}
			
			}

				RHIUnlockTexture2D( SrcTex2DResource_D->GetTexture2DRHI(), SrcMipD, FALSE );
				RHIUnlockTexture2D( SrcTex2DResource_N->GetTexture2DRHI(), SrcMipN, FALSE );
				RHIUnlockTexture2D( SrcTex2DResource_S->GetTexture2DRHI(), SrcMipS, FALSE );
		}

			//// scale regions for the current mip level. The regions are assumed to be sized w/ respect to the base (maybe not loaded) mip level
			//INT RegionOffsetX = Region->OffsetX >> MipIdx;
			//INT RegionOffsetY = Region->OffsetY >> MipIdx;
			//INT RegionDestOffsetX = Region->DestOffsetX >> MipIdx;
			//INT RegionDestOffsetY = Region->DestOffsetY >> MipIdx;
			//INT RegionSizeX = Max((INT)Region->SizeX >> MipIdx,1);
			//INT RegionSizeY = Max((INT)Region->SizeY >> MipIdx,1);

			//// scale create the new region for the RHI texture copy
			//FBlendTextureRegion2D* BlendRegion = new(BlendRegions) FBlendTextureRegion2D(
			//	RegionOffsetX,
			//	RegionOffsetY,
			//	RegionSizeX,
			//	RegionSizeY,
			//	RegionDestOffsetX,
			//	RegionDestOffsetY,
			//	texList
			//	);
		}

		RHIUnlockTexture2D( TexTargetD, MipIdx - FirstMip, FALSE );
		RHIUnlockTexture2D( TexTargetN, MipIdx - FirstMip, FALSE );
		RHIUnlockTexture2D( TexTargetS, MipIdx - FirstMip, FALSE );

		//// copy from all of the source regions for the current mip level to the temp texture
		//RHICopyTexture2D(
		//	CompositeTexture2D,
		//	MipIdx,
		//	CompositeResource->SizeX,
		//	CompositeResource->SizeY,
		//	CompositeResource->Format,
		//	CopyRegions
		//	);
	}

	bFinished = TRUE;
	bDoingMerge = FALSE;
}

void FTextureMergeUtil::RenderThread_StartMerge()
{
#ifdef OutputDebugInfo
	debugf(	TEXT("Start merge.") );
#endif
	MipIdx = FirstMip;
	RegionIdx = 0;
	LayerIdx = 0;

	TexTargetD = ((FTexture2DDynamicResource*)CurrentTargetTexture_D->Resource)->GetTexture2DRHI();
	TexTargetN = ((FTexture2DDynamicResource*)CurrentTargetTexture_N->Resource)->GetTexture2DRHI();
	TexTargetS = ((FTexture2DDynamicResource*)CurrentTargetTexture_S->Resource)->GetTexture2DRHI();

	//INT NumMips = CurrentTargetTexture_D->NumMips;//CurrentSourceRegions_D(0).Textures(0)->ResidentMips;

// 	INT Format = PF_DXT1;
// 	BlockSizeX = GPixelFormats[Format].BlockSizeX;
// 	BlockSizeY = GPixelFormats[Format].BlockSizeY;
// 	BlockBytes = GPixelFormats[Format].BlockBytes;
// 
// 	// lock dest surfaces
// 	UINT DstStride;
// 	DstData_D = (BYTE*)RHILockTexture2D( TexTargetD, MipIdx, TRUE, DstStride, FALSE );
// 	DstData_N = (BYTE*)RHILockTexture2D( TexTargetN, MipIdx, TRUE, DstStride, FALSE );
// 	DstData_S = (BYTE*)RHILockTexture2D( TexTargetS, MipIdx, TRUE, DstStride, FALSE );
// 
// 	INT BaseSizeX = CurrentTargetTexture_D->SizeX;
// 	INT BaseSizeY = CurrentTargetTexture_D->SizeY;
// 	DestMipSizeX = Max(BlockSizeX,BaseSizeX >> MipIdx);
// 	DestMipSizeY = Max(BlockSizeY,BaseSizeY >> MipIdx);

	if( !LockTargetTextureData() )
		return;

	RenderThread_MergeOneTexBegin();
}

UBOOL FTextureMergeUtil::LockTargetTextureData()
{
	INT Format = PF_DXT1;
	BlockSizeX = GPixelFormats[Format].BlockSizeX;
	BlockSizeY = GPixelFormats[Format].BlockSizeY;
	BlockBytes = GPixelFormats[Format].BlockBytes;

	//if ( MipIdx + RegionIdx + LayerIdx != 0 )
// 	{
// 		RHIUnlockTexture2D( TexTargetD, MipIdx, FALSE );
// 		RHIUnlockTexture2D( TexTargetN, MipIdx, FALSE );
// 		RHIUnlockTexture2D( TexTargetS, MipIdx, FALSE );
// 	}
	// lock dest surfaces
	INT TargetMip = MipIdx - FirstMip;
	DestPitchBytes_D = DestPitchBytes_N = DestPitchBytes_S = 0;
	DstData_D = TexTargetD ? (BYTE*)RHILockTexture2D( TexTargetD, TargetMip, TRUE, DestPitchBytes_D, FALSE ) : NULL;
	DstData_N = TexTargetN ? (BYTE*)RHILockTexture2D( TexTargetN, TargetMip, TRUE, DestPitchBytes_N, FALSE ) : NULL;
	DstData_S = TexTargetS ? (BYTE*)RHILockTexture2D( TexTargetS, TargetMip, TRUE, DestPitchBytes_S, FALSE ) : NULL;

	//debugf( TEXT("FTextureMergeUtil: Lock %d "), MipIdx );


	INT BaseSizeX = CurrentTargetTexture_D->SizeX;
	INT BaseSizeY = CurrentTargetTexture_D->SizeY;
	DestMipSizeX = Max(BlockSizeX,BaseSizeX >> TargetMip);
	DestMipSizeY = Max(BlockSizeY,BaseSizeY >> TargetMip);
	UINT RowBytes;
	SIZE_T Rows;
	UBOOL LayoutValid = GetTextureMergeBufferLayout(DestMipSizeX, DestMipSizeY, RowBytes, Rows);
	if( !LayoutValid || !DstData_D || !DstData_N || !DstData_S || DestPitchBytes_D < RowBytes ||
		DestPitchBytes_N < RowBytes || DestPitchBytes_S < RowBytes ||
		Rows > SIZE_T(-1) / DestPitchBytes_D || Rows > SIZE_T(-1) / DestPitchBytes_N ||
		Rows > SIZE_T(-1) / DestPitchBytes_S )
	{
		if( DstData_D ) RHIUnlockTexture2D(TexTargetD, TargetMip, FALSE);
		if( DstData_N ) RHIUnlockTexture2D(TexTargetN, TargetMip, FALSE);
		if( DstData_S ) RHIUnlockTexture2D(TexTargetS, TargetMip, FALSE);
		DstData_D = DstData_N = DstData_S = NULL;
		bCancelCurrentMerge = TRUE;
		bDoingMerge = FALSE;
		bFinished = TRUE;
		return FALSE;
	}
	// Uncovered atlas blocks must not retain uninitialized lock memory.
	for( SIZE_T Row = 0; Row < Rows; ++Row )
	{
		appMemzero(DstData_D + SIZE_T(Row) * DestPitchBytes_D, RowBytes);
		appMemzero(DstData_N + SIZE_T(Row) * DestPitchBytes_N, RowBytes);
		appMemzero(DstData_S + SIZE_T(Row) * DestPitchBytes_S, RowBytes);
	}
	return TRUE;
}

UBOOL FTextureMergeUtil::GetScrDataFromTex( FTextureLockData *&pTexLockData, FTexture2DResource *&tex2DRes, INT &mipID )
{
	if( !pTexLockData || !pTexLockData->TextureKey || !tex2DRes || mipID < 0 ||
		mipID >= ARRAY_COUNT(pTexLockData->Data) )
		return FALSE;
	UTexture2D* Texture = pTexLockData->TextureKey;
	INT FirstResidentMip = Texture->Mips.Num() - Texture->ResidentMips;
	if( Texture->SizeX <= 0 || Texture->SizeY <= 0 || FirstResidentMip < 0 ||
		mipID < FirstResidentMip || mipID >= Texture->Mips.Num() )
		return FALSE;
	INT Width = Max(4, Texture->SizeX >> mipID);
	INT Height = Max(4, Texture->SizeY >> mipID);
	UINT RowBytes;
	SIZE_T Rows;
	if( !GetTextureMergeBufferLayout(Width, Height, RowBytes, Rows) )
		return FALSE;
	if( !tex2DRes->GetTexture2DRHI() )
		return FALSE;
	UINT RowStride = 0;
	INT ResourceMip = mipID - FirstResidentMip;
	BYTE* Data = (BYTE*)RHILockTexture2D(tex2DRes->GetTexture2DRHI(), ResourceMip, FALSE, RowStride, FALSE);
	if( !Data )
		return FALSE;
	if( RowStride < RowBytes || Rows > SIZE_T(-1) / RowStride )
	{
		RHIUnlockTexture2D(tex2DRes->GetTexture2DRHI(), ResourceMip, FALSE);
		return FALSE;
	}
	// Cache the full source mip using a tight pitch, independent of region height
	// and of padding in the hardware lock's rows.
	BYTE* Copy = new BYTE[Rows * RowBytes];
	for( SIZE_T Row = 0; Row < Rows; ++Row )
		appMemcpy(Copy + Row * RowBytes, Data + Row * RowStride, RowBytes);
	pTexLockData->Data[mipID] = Copy;
	++pTexLockData->RefCount[mipID];
	RHIUnlockTexture2D(tex2DRes->GetTexture2DRHI(), ResourceMip, FALSE);
	return TRUE;
}

void FTextureMergeUtil::RenderThread_MergeOneTexBegin()
{
#ifdef OutputDebugInfo
	debugf( TEXT("Begin merge tex: region %d layer %d."), RegionIdx, LayerIdx );
#endif
	SrcLockData_D = SrcLockData_N = SrcLockData_S = NULL;
	if( !CurrentSourceRegions_D.IsValidIndex(RegionIdx) )
	{
		RenderThread_FinishMerge();
		return;
	}
	Region = &CurrentSourceRegions_D(RegionIdx);
	while( Region->Textures.Num() == 0 )
	{
		++RegionIdx;
		if( RegionIdx >= CurrentSourceRegions_D.Num() )
			break;
		Region = &CurrentSourceRegions_D(RegionIdx);
	}
	if( Region->Textures.Num() == 0 )
	{
		RenderThread_FinishMerge();
		return;
	}
	UTexture2D* SrcTexture = Region->Textures(LayerIdx);

	SrcTex2DResource_D = (FTexture2DResource*)SrcTexture->Resource;
	SrcTex2DResource_N = (FTexture2DResource*)CurrentSourceRegions_N(RegionIdx).Textures(LayerIdx)->Resource;
	SrcTex2DResource_S = (FTexture2DResource*)CurrentSourceRegions_S(RegionIdx).Textures(LayerIdx)->Resource;

	// lock source surfaces
	//UINT SrcStride;
	INT SrcLockIdx_D, SrcLockIdx_N, SrcLockIdx_S;

	SrcLockData_D = GetLockData_D( SrcTexture, SrcLockIdx_D );
	if( SrcLockData_D == NULL )
	{
		//SrcData_D = (BYTE*)RHILockTexture2D( SrcTex2DResource_D->GetTexture2DRHI(), MipIdx, FALSE, SrcStride, FALSE );
		SrcLockData_D = new FTextureLockData( SrcTexture );
		SrcLockIdx_D = LockData_D.AddItem( SrcLockData_D );
	}
	
	if ( SrcLockData_D->RefCount[MipIdx] == 0 )
	{
		if( !GetScrDataFromTex( SrcLockData_D, SrcTex2DResource_D, MipIdx ) )
		{
			if( SrcLockData_D ) CheckLockDataRefNum(SrcLockData_D, MipIdx, LockData_D);
			bCancelCurrentMerge = TRUE;
			RenderThread_FinishMerge();
			return;
		}
		SrcData_D = SrcLockData_D->Data[MipIdx];
	}
	else
	{
		SrcData_D = SrcLockData_D->Data[MipIdx];
		++SrcLockData_D->RefCount[MipIdx];
	}

	SrcLockData_N = GetLockData_N( CurrentSourceRegions_N(RegionIdx).Textures(LayerIdx), SrcLockIdx_N );
	if( SrcLockData_N == NULL )
	{
		//SrcData_N = (BYTE*)RHILockTexture2D( SrcTex2DResource_N->GetTexture2DRHI(), MipIdx, FALSE, SrcStride, FALSE );
		SrcLockData_N = new FTextureLockData( CurrentSourceRegions_N(RegionIdx).Textures(LayerIdx) );
		SrcLockIdx_N = LockData_N.AddItem( SrcLockData_N );
	}
	
	if ( SrcLockData_N->RefCount[MipIdx] == 0 )
	{
		if( !GetScrDataFromTex( SrcLockData_N, SrcTex2DResource_N, MipIdx ) )
		{
			if( SrcLockData_D ) CheckLockDataRefNum(SrcLockData_D, MipIdx, LockData_D);
			if( SrcLockData_N ) CheckLockDataRefNum(SrcLockData_N, MipIdx, LockData_N);
			bCancelCurrentMerge = TRUE;
			RenderThread_FinishMerge();
			return;
		}
		SrcData_N = SrcLockData_N->Data[MipIdx];
	}
	else
	{
		SrcData_N = SrcLockData_N->Data[MipIdx];
		++SrcLockData_N->RefCount[MipIdx];
	}

	SrcLockData_S = GetLockData_S( CurrentSourceRegions_S(RegionIdx).Textures(LayerIdx), SrcLockIdx_S );
	if( SrcLockData_S == NULL )
	{
		//SrcData_S = (BYTE*)RHILockTexture2D( SrcTex2DResource_S->GetTexture2DRHI(), MipIdx, FALSE, SrcStride, FALSE );
		SrcLockData_S = new FTextureLockData( CurrentSourceRegions_S(RegionIdx).Textures(LayerIdx) );
		SrcLockIdx_S = LockData_S.AddItem( SrcLockData_S );
	}

	if ( SrcLockData_S->RefCount[MipIdx] == 0 )
	{
		if( !GetScrDataFromTex( SrcLockData_S, SrcTex2DResource_S, MipIdx ) )
		{
			if( SrcLockData_D ) CheckLockDataRefNum(SrcLockData_D, MipIdx, LockData_D);
			if( SrcLockData_N ) CheckLockDataRefNum(SrcLockData_N, MipIdx, LockData_N);
			if( SrcLockData_S ) CheckLockDataRefNum(SrcLockData_S, MipIdx, LockData_S);
			bCancelCurrentMerge = TRUE;
			RenderThread_FinishMerge();
			return;
		}
		SrcData_S = SrcLockData_S->Data[MipIdx];
	}
	else
	{
		SrcData_S = SrcLockData_S->Data[MipIdx];
		++SrcLockData_S->RefCount[MipIdx];
	}

	// test code
	if( LockData_D.Num() != LockData_N.Num() || LockData_D.Num() != LockData_S.Num() )
	{
		debugf( TEXT("Lock data not good.") );
	}

	// Calculate source values
	INT SrcSizeX = SrcTexture->SizeX >> MipIdx;
	INT SrcSizeY = SrcTexture->SizeY >> MipIdx;
	SrcMipSizeX = Max(BlockSizeX,SrcSizeX);
	SrcMipSizeY = Max(BlockSizeY,SrcSizeY);

	GetTextureMergeCopyBounds(*Region, MipIdx, SrcMipSizeX, SrcMipSizeY,
		DestMipSizeX, DestMipSizeY, BlockSizeX, BlockSizeY,
		SrcRegionOffsetX, SrcRegionOffsetY, DestRegionOffsetX, DestRegionOffsetY, RegionSizeX, RegionSizeY);

	// size in bytes of an entire row for this mip
	SrcPitchBytes = UINT((SIZE_T(SrcMipSizeX) + BlockSizeX - 1) / BlockSizeX) * BlockBytes;

	// size in bytes of the offset to the starting part of the row to copy for this mip
	SrcRowOffsetBytes = UINT(SrcRegionOffsetX / BlockSizeX) * BlockBytes;
	DestRowOffsetBytes = UINT(DestRegionOffsetX / BlockSizeX) * BlockBytes;

	// block number of each row
	BlockPerRow = RegionSizeX / BlockSizeX;

	if( MaxMergeThreadNum != 0 && CurrentRunningThreadNum >= MaxMergeThreadNum )
	{
		// exceeds max thread number, wait other thread to finish
		bWaiting = TRUE;
	}
	else
	{
		// start merging thread right away
		StartMergingThread();
	}
}

void FTextureMergeUtil::StartMergingThread()
{
	// start merge task
	(new FAutoDeleteAsyncTask<FTextureMergeTask>(this))->StartBackgroundTask();

	// increment the thread count
	// this is safe since it's called in the same thread
	++CurrentRunningThreadNum;

	// mark the flag in case the object be destroyed during the merging thread is running
	bBackgroundMerging = TRUE;
}

void FTextureMergeUtil::CheckLockDataRefNum( FTextureLockData *&pTexLockData, INT &mipID, TextureLockDataList &datalist )
{
	if( !pTexLockData || mipID < 0 || mipID >= ARRAY_COUNT(pTexLockData->Data) )
		return;
	pTexLockData->RefCount[mipID] = Max( 0, pTexLockData->RefCount[mipID] -1 );
	if ( pTexLockData->RefCount[mipID] == 0 )
	{
		delete[] pTexLockData->Data[mipID];
		pTexLockData->Data[mipID] = NULL;
	}

	if ( appMemIsZero( pTexLockData->RefCount, sizeof( pTexLockData->RefCount ) ) )
	{
		datalist.RemoveSingleItemSwap( pTexLockData );
		delete pTexLockData;
		pTexLockData = NULL;
	}
}

void FTextureMergeUtil::RenderThread_MergeOneTexEnd()
{
#ifdef OutputDebugInfo
	debugf( TEXT("Finish merge tex.") );
#endif
	// release merged surfaces
// 	--SrcLockData_D->RefCount;
// 	if( SrcLockData_D->RefCount <= 0 )
// 	{
// 		RHIUnlockTexture2D( SrcTex2DResource_D->GetTexture2DRHI(), MipIdx, FALSE );
// 		LockData_D.RemoveSingleItem( SrcLockData_D );
// 	}

	CheckLockDataRefNum( SrcLockData_D, MipIdx, LockData_D );

// 	--SrcLockData_N->RefCount;
// 	if( SrcLockData_N->RefCount <= 0 )
// 	{
// 		RHIUnlockTexture2D( SrcTex2DResource_N->GetTexture2DRHI(), MipIdx, FALSE );
// 		LockData_N.RemoveSingleItem( SrcLockData_N );
// 	}

	CheckLockDataRefNum( SrcLockData_N, MipIdx, LockData_N );

// 	--SrcLockData_S->RefCount;
// 	if( SrcLockData_S->RefCount <= 0 )
// 	{
// 		RHIUnlockTexture2D( SrcTex2DResource_S->GetTexture2DRHI(), MipIdx, FALSE );
// 		LockData_S.RemoveSingleItem( SrcLockData_S );
// 	}

	CheckLockDataRefNum( SrcLockData_S, MipIdx, LockData_S );

	// test code
	if( LockData_D.Num() != LockData_N.Num() || LockData_D.Num() != LockData_S.Num() )
	{
		debugf( TEXT("Lock data not good.") );
	}

	
	
	++LayerIdx;
	if( LayerIdx == Region->Textures.Num() )
	{
		++RegionIdx;

		if( RegionIdx == CurrentSourceRegions_D.Num() || 
			CurrentSourceRegions_D(RegionIdx).Textures.Num() == 0 )
		{
			INT TargetMip = MipIdx - FirstMip;
			if ( TargetMip + 1 == CurrentTargetTexture_D->NumMips )
			{
				RenderThread_FinishMerge();
				return;
			}
			RHIUnlockTexture2D( TexTargetD, TargetMip, FALSE );
			RHIUnlockTexture2D( TexTargetN, TargetMip, FALSE );
			RHIUnlockTexture2D( TexTargetS, TargetMip, FALSE );
			//debugf( TEXT("FTextureMergeUtil: UnLock %d "), MipIdx );
			++MipIdx;
			if( !LockTargetTextureData() )
				return;
			RegionIdx = 0;
		}

		LayerIdx = 0;
	}
	
	//else
	{
		
	}
	

	// check cancel
	if( bCancelCurrentMerge )
	{
		RenderThread_FinishMerge();
	}
	else
	{
		
		RenderThread_MergeOneTexBegin();
	}
}

void FTextureMergeUtil::RenderThread_FinishMerge()
{
#ifdef OutputDebugInfo
	debugf( TEXT("Finish merge.") );
#endif

	// release dest surfaces // shit who commented this code out!
	INT TargetMip = MipIdx - FirstMip;
 	RHIUnlockTexture2D( TexTargetD, TargetMip, FALSE );
 	RHIUnlockTexture2D( TexTargetN, TargetMip, FALSE );
 	RHIUnlockTexture2D( TexTargetS, TargetMip, FALSE );

	MipIdx = 0;

	bDoingMerge = FALSE;
	bFinished = TRUE;
}

void FTextureMergeUtil::BackgroundThread_MergeTextures()
{
#ifdef OutputDebugInfo
	debugf( TEXT("Doing Merge.") );
#endif
	DXT1Tile* srcTile_D, * srcTile_N, * srcTile_S;
	DXT1Tile* dstTile_D, * dstTile_N, * dstTile_S;

	INT CurDestOffsetY = DestRegionOffsetY;
	if( LayerIdx == 0 )
	{	// copy by row
		DWORD RowSizeBytes = UINT(RegionSizeX / BlockSizeX) * BlockBytes;
		for( INT CurSrcOffsetY = SrcRegionOffsetY; CurSrcOffsetY < (SrcRegionOffsetY + RegionSizeY); CurSrcOffsetY += BlockSizeY )
		{
#ifdef OutputDebugInfo
			//debugf( TEXT("CurOffsetY: %d"), CurSrcOffsetY );
#endif
			INT CurSrcBlockOffsetY = CurSrcOffsetY / BlockSizeY;
			INT CurDestBlockOffsetY = CurDestOffsetY / BlockSizeY;

			BYTE* SrcOffset = SrcData_D + (SIZE_T(CurSrcBlockOffsetY) * SrcPitchBytes) + SrcRowOffsetBytes;
			BYTE* DstOffset = DstData_D + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_D) + DestRowOffsetBytes;
			appMemcpy( DstOffset, SrcOffset, RowSizeBytes );
			SrcOffset = SrcData_N + (SIZE_T(CurSrcBlockOffsetY) * SrcPitchBytes) + SrcRowOffsetBytes;
			DstOffset = DstData_N + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_N) + DestRowOffsetBytes;
			appMemcpy( DstOffset, SrcOffset, RowSizeBytes );
			SrcOffset = SrcData_S + (SIZE_T(CurSrcBlockOffsetY) * SrcPitchBytes) + SrcRowOffsetBytes;
			DstOffset = DstData_S + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_S) + DestRowOffsetBytes;
			appMemcpy( DstOffset, SrcOffset, RowSizeBytes );

			CurDestOffsetY += BlockSizeY;
		}
	}
	else
	{	// do blend per block(tile)
		for( INT CurSrcOffsetY=SrcRegionOffsetY; CurSrcOffsetY < (SrcRegionOffsetY+RegionSizeY); CurSrcOffsetY += BlockSizeY )
		{
#ifdef OutputDebugInfo
			//debugf( TEXT("CurOffsetY: %d"), CurSrcOffsetY );
#endif
			INT CurSrcBlockOffsetY = CurSrcOffsetY / BlockSizeY;
			INT CurDestBlockOffsetY = CurDestOffsetY / BlockSizeY;

			BYTE* SrcOffset_D = SrcData_D + (SIZE_T(CurSrcBlockOffsetY) * SrcPitchBytes) + SrcRowOffsetBytes;
			BYTE* DstOffset_D = DstData_D + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_D) + DestRowOffsetBytes;
			BYTE* SrcOffset_N = SrcData_N + (SIZE_T(CurSrcBlockOffsetY) * SrcPitchBytes) + SrcRowOffsetBytes;
			BYTE* DstOffset_N = DstData_N + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_N) + DestRowOffsetBytes;
			BYTE* SrcOffset_S = SrcData_S + (SIZE_T(CurSrcBlockOffsetY) * SrcPitchBytes) + SrcRowOffsetBytes;
			BYTE* DstOffset_S = DstData_S + (SIZE_T(CurDestBlockOffsetY) * DestPitchBytes_S) + DestRowOffsetBytes;
			//appMemcpy( DstOffset, SrcOffset, RowSizeBytes );

			for( UINT blockIdx = 0; blockIdx < BlockPerRow; ++blockIdx )
			{
				srcTile_D = (DXT1Tile*)( SrcOffset_D + blockIdx * BlockBytes );
				srcTile_N = (DXT1Tile*)( SrcOffset_N + blockIdx * BlockBytes );
				srcTile_S = (DXT1Tile*)( SrcOffset_S + blockIdx * BlockBytes );
				dstTile_D = (DXT1Tile*)( DstOffset_D + blockIdx * BlockBytes );
				dstTile_N = (DXT1Tile*)( DstOffset_N + blockIdx * BlockBytes );
				dstTile_S = (DXT1Tile*)( DstOffset_S + blockIdx * BlockBytes );

				BlendTile_CPU( srcTile_D, dstTile_D, srcTile_N, dstTile_N, srcTile_S, dstTile_S );

// 				A8R8G8B8 dstColor_D[16];
// 				switch( MipIdx )
// 				{
// 				case 1:
// 					{
// 						DecompressDXT1Tile( dstTile_D, dstColor_D );
// 						for( UINT i = 0; i < 16;  ++i )
// 						{
// 							dstColor_D[i].r = 255;
// 							dstColor_D[i].g = 0;
// 							dstColor_D[i].b = 0;
// 							dstColor_D[i].a = 255;
// 						}
// 						CompressDXT1Tile( dstColor_D, dstTile_D );
// 						break;
// 					}
// 				case 2:
// 					{
// 						DecompressDXT1Tile( dstTile_D, dstColor_D );
// 						for( UINT i = 0; i < 16;  ++i )
// 						{
// 							dstColor_D[i].r = 0;
// 							dstColor_D[i].g = 255;
// 							dstColor_D[i].b = 0;
// 							dstColor_D[i].a = 255;
// 						}
// 						CompressDXT1Tile( dstColor_D, dstTile_D );
// 						break;
// 					}
// 				case 3:
// 					{
// 						DecompressDXT1Tile( dstTile_D, dstColor_D );
// 						for( UINT i = 0; i < 16;  ++i )
// 						{
// 							dstColor_D[i].r = 0;
// 							dstColor_D[i].g = 0;
// 							dstColor_D[i].b = 255;
// 							dstColor_D[i].a = 255;
// 						}
// 						CompressDXT1Tile( dstColor_D, dstTile_D );
// 						break;
// 					}
// 				default:
// 					break;
// 				}
			}

			CurDestOffsetY += BlockSizeY;
		}
	}

#ifdef OutputDebugInfo
	debugf( TEXT("Merge done.") );
#endif
	//bMergeTaskFinished = TRUE;

	MergeFinishEvent.Trigger();
}

void FTextureMergeUtil::UpdateState()
{
	if( MergeFinished() )
		return;

	if( bWaiting )
	{
		// check if any thread has finished so we can start the new merging
		if( CurrentRunningThreadNum < MaxMergeThreadNum )
		{
			StartMergingThread();
		}
	}

	//if( bMergeTaskFinished )
	if( MergeFinishEvent.Wait(0) ) // not an efficient way
	{
#ifdef OutputDebugInfo
		debugf( TEXT("Enqueue MergeEnd command.") );
#endif
		// decrease thread count
		--CurrentRunningThreadNum;

		bBackgroundMerging = FALSE;


		// let render thread to coninue work
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			MergeTextureEnd,
			FTextureMergeUtil*, Texture, this,
			{
				Texture->RenderThread_MergeOneTexEnd();
			} );
		//bMergeTaskFinished = FALSE;
	}
}

FTextureLockData* FTextureMergeUtil::GetLockData_D( const UTexture2D* tex, INT& idx )
{
	FTextureLockData* temp;
	for( INT i = 0; i < LockData_D.Num(); ++i )
	{
		temp = LockData_D(i);
		// todo: operator() not efficient
		if( temp->TextureKey == tex )
		{
			idx = i;
			return temp;
		}
	}
	return NULL;
}

FTextureLockData* FTextureMergeUtil::GetLockData_N( const UTexture2D* tex, INT& idx )
{
	FTextureLockData* temp;
	for( INT i = 0; i < LockData_N.Num(); ++i )
	{
		temp = LockData_N(i);
		// todo: operator() not efficient
		if( temp->TextureKey == tex )
		{
			idx = i;
			return temp;
		}
	}
	return NULL;
}

FTextureLockData* FTextureMergeUtil::GetLockData_S( const UTexture2D* tex, INT& idx )
{
	FTextureLockData* temp;
	for( INT i = 0; i < LockData_S.Num(); ++i )
	{
		temp = LockData_S(i);
		// todo: operator() not efficient
		if( temp->TextureKey == tex )
		{
			idx = i;
			return temp;
		}
	}
	return NULL;
}

#ifdef OutputDebugInfo
#undef OutputDebugInfo
#endif