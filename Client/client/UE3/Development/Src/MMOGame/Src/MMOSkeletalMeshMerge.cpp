/*=============================================================================
	MMOSkeletalMeshMerge.cpp: Helper class for creating merged avatars.
	ming@snda
=============================================================================*/

#include "stdafx.h"
#include "UnSkeletalMeshMerge.h"
#include "UnTextureMerge.h"

IMPLEMENT_CLASS(UMMOSkeletalMeshMerge);

void UMMOSkeletalMeshMerge::Init( INT partNum )
{
	bDoneMerge = FALSE;
}

UBOOL UMMOSkeletalMeshMerge::DoMerge()
{
#if !FINAL_RELEASE
	// check sources
	if( !Validation() )
	{
		return FALSE;;
	}
#endif

	// merge textures
	//MergedTex = MergeTextures();

	// start texture merging thread
	if( !MergeTextures() )
	{
		debugf( TEXT( "Failed to start texture merging thread." ) );
	}

	// compile final mesh
	if( /*&& MergedTex &&*/ MergeMeshes() )
	{
		//for( INT idx = 0; idx < Parts.Num(); ++idx )
		//{
		//	Parts(idx).Mesh = NULL;
		//	Parts(idx).Textures.Empty();
		//}

		return TRUE;
	}

	return FALSE;
}

void UMMOSkeletalMeshMerge::CancelMerge()
{
	if( TexMergeUtil )
	{
		TexMergeUtil->CancelCurrentMerge();
	}
	bDoneMerge = FALSE;
}

void UMMOSkeletalMeshMerge::UpdateState()
{
	// update background thread state
	if( TexMergeUtil )
		TexMergeUtil->UpdateState();
}

UBOOL UMMOSkeletalMeshMerge::CheckFinish()
{
	//if( MergedTex && MergedTex->Resource && MergedTex->Resource->TextureRHI )
	//	return	TRUE;
	if( TexMergeUtil )
	{
		if( TexMergeUtil->MergeFinished() )
		{
			//PostMerge();
			delete TexMergeUtil;
			TexMergeUtil = NULL;

			bDoneMerge = TRUE;

			return TRUE;
		}
	}
	return FALSE;
}

UBOOL UMMOSkeletalMeshMerge::PrepareTextures()
{
	for( INT i = 0; i < Parts.Num(); ++i )
	{
		if( Parts(i).Textures.Num() != Parts(i).Textures_Base.Num() )
			return FALSE;
	}
	UTexture2D* tex;
	NumMipLevels = 5 - GSystemSettings.GetNumMipsToDrop();	// maximum mip levels we need
	for( INT i = 0; i < Parts.Num(); ++i )
	{
		Parts(i).Textures_N.Empty();
		Parts(i).Textures_S.Empty();

		for( INT j = 0; j < Parts(i).Textures_Base.Num(); ++j )
		{
			if( !Parts(i).Textures(j) || !Parts(i).Textures_Base(j) )
				return FALSE;	// diffuse texture must exist

			// force mipmaps to stream in
			Parts(i).Textures(j)->bForceMiplevelsToBeResident = TRUE;
			Parts(i).Textures(j)->SetForceMipLevelsToBeResident( 30.f );

			if( Parts(i).Textures(j)->ResidentMips < NumMipLevels )
				NumMipLevels = Parts(i).Textures(j)->ResidentMips;

			// normal
			FString name = Parts(i).Textures_Base(j)->GetName();
			FString newName = name.LeftChop(1) + TEXT("N");
			UObject* objTex;
			// first try to find the object, to prevent the render resource from been created again
			objTex = StaticFindObject( UTexture2D::StaticClass(), Parts(i).Textures(j)->GetOuter(), *newName, FALSE );
			if( objTex == NULL )
			{
				if (Parts(i).Textures(j)->IsInExportedPackage())
				{
					FString FullName = Parts(i).Textures(j)->GetOuter()->GetName();
					FullName = FullName.Replace(*name, *newName, TRUE);
					FullName = FullName + FString(TEXT(".")) + newName;
					objTex = StaticLoadObject(UTexture2D::StaticClass(), NULL, *FullName, 0, 0, 0);
				}
				else
				{
					objTex = StaticLoadObject( UTexture2D::StaticClass(), Parts(i).Textures(j)->GetOuter(), *newName, 0, 0, 0 , 0 );
				}
			}
			if( objTex == NULL )
			{
				debugf( TEXT( "Failed to load normal texture %s." ), *newName );
				return FALSE;
			}
			tex = CastChecked<UTexture2D>( objTex );
			Parts(i).Textures_N.AddItem( tex );

			tex->bForceMiplevelsToBeResident = TRUE;
			tex->SetForceMipLevelsToBeResident( 30.f );

			if( tex->ResidentMips < NumMipLevels )
				NumMipLevels = tex->ResidentMips;

			// specular
			newName = name.LeftChop(1) + TEXT("S");
			// first try to find the object, to prevent the render resource from been created again
			objTex = StaticFindObject( UTexture2D::StaticClass(), Parts(i).Textures(j)->GetOuter(), *newName, FALSE );
			if( objTex == NULL )
			{
				// not loaded yet, do it
				if (Parts(i).Textures(j)->IsInExportedPackage())
				{
					FString FullName = Parts(i).Textures(j)->GetOuter()->GetName();
					FullName = FullName.Replace(*name, *newName, TRUE);
					FullName = FullName + FString(TEXT(".")) + newName;
					objTex = StaticLoadObject(UTexture2D::StaticClass(), NULL, *FullName, 0, 0, 0);
				}
				else
				{
					objTex = StaticLoadObject( UTexture2D::StaticClass(), Parts(i).Textures(j)->GetOuter(), *newName, 0, 0, 0 , 0 );
				}

			}
			if( objTex == NULL )
			{
				debugf( TEXT( "Failed to load specular texture %s." ), *newName );
				return FALSE;
			}
			tex = CastChecked<UTexture2D>( objTex );
			Parts(i).Textures_S.AddItem( tex );

			tex->bForceMiplevelsToBeResident = TRUE;
			tex->SetForceMipLevelsToBeResident( 30.f );

			if( tex->ResidentMips < NumMipLevels )
				NumMipLevels = tex->ResidentMips;
		}
	}

	// create target textures
	MergedTex_D = CreateTargetTexture( TRUE, TC_Default );
	MergedTex_N = CreateTargetTexture( FALSE, TC_Normalmap );
	MergedTex_S = CreateTargetTexture( FALSE, TC_Default );

	//debugf( TEXT("NumMipLevels: %d"), NumMipLevels );

	// cancel last merge
	if( TexMergeUtil )
		TexMergeUtil->CancelCurrentMerge();

	return TRUE;
}

UTexture2DDynamic* UMMOSkeletalMeshMerge::CreateTargetTexture( UBOOL bSRGB, TextureCompressionSettings cmps )
{
	UTexture2DDynamic* NewTexture = Cast<UTexture2DDynamic>(StaticConstructObject(
		UTexture2DDynamic::StaticClass(), GetTransientPackage(), NAME_None, RF_Transient));
	if (NewTexture != NULL)
	{
		NewTexture->CompressionNone			= FALSE;
		NewTexture->CompressionSettings		= cmps;
		NewTexture->CompressionNoAlpha		= TRUE;
		NewTexture->DeferCompression		= FALSE;
		// Untiled format
		NewTexture->bNoTiling				= TRUE;
		NewTexture->SRGB					= bSRGB;
		NewTexture->LODGroup				= TEXTUREGROUP_Character;
		NewTexture->MipGenSettings			= TMGS_FromTextureGroup;//TMGS_NoMipmaps; // oscar@snda
		NewTexture->NumMips					= NumMipLevels;  // oscar@snda

		// drop highest mipmap levels according to system setting
		// must ensure the validation [0,2]!
		NewTexture->Init(
			TargetTexSizeX >> GSystemSettings.GetNumMipsToDrop(), 
			TargetTexSizeY >> GSystemSettings.GetNumMipsToDrop(), 
			PF_DXT1, FALSE, NumMipLevels);
	}
	return NewTexture;
}

UBOOL UMMOSkeletalMeshMerge::IsSourceTexturesFullyLoaded()
{
	for( INT i = 0; i < Parts.Num(); ++i )
	{
		for( INT j = 0; j < Parts(i).Textures.Num(); ++j )
		{
			if( !Parts(i).Textures(j) || !Parts(i).Textures(j)->IsFullyStreamedIn() )
			{
				return FALSE;
			}
		}

		for( INT j = 0; j < Parts(i).Textures_N.Num(); ++j )
		{	// normal texture may exist or not( 0 )
			if( !Parts(i).Textures_N(j) || !Parts(i).Textures_N(j)->IsFullyStreamedIn() )
			{
				return FALSE;
			}
		}

		for( INT j = 0; j < Parts(i).Textures_S.Num(); ++j )
		{	// specular texture must exist
			if( !Parts(i).Textures_S(j) || !Parts(i).Textures_S(j)->IsFullyStreamedIn() )
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

UBOOL UMMOSkeletalMeshMerge::LastMergeCancelled()
{
	if( TexMergeUtil && TexMergeUtil->IsDoingMerge() )
	{
		return FALSE;
	}
	return TRUE;
}

UBOOL UMMOSkeletalMeshMerge::Validation()
{
	if( Parts.Num() <= 1 )
		return FALSE;

	for( INT idx = 0; idx < Parts.Num(); ++idx )
	{
		if( Parts(idx).Mesh == NULL && idx != 5 && idx != 0 )
		{
			debugf( TEXT( "Error: Merge part %d is none." ), idx );
			return FALSE;
		}

		for( INT j = 0; j < Parts(idx).Textures.Num(); ++j )
		{
			if( Parts(idx).Textures(j)->SizeX != Parts(idx).SizeX ||
				Parts(idx).Textures(j)->SizeY != Parts(idx).SizeY )
			{
				//return FALSE;
				// acceptable error, resource not correct
				debugf( TEXT( "Merge texture source %s size incorrect, %d." ), 
					*Parts(idx).Textures(j)->GetName(), idx );
			}
		}
	}

	return IsSourceTexturesFullyLoaded();
}

UBOOL UMMOSkeletalMeshMerge::MergeMeshes()
{
	UBOOL dirty = FALSE;
	for( INT idx = 0; idx < Parts.Num(); ++idx )
	{
		//if( Parts(idx).Mesh != OldParts(idx).Mesh )
		//{
		//	dirty = TRUE;
		//	break;
		//}
	}
	//if( !dirty )
	//	return TRUE;

	TArray< USkeletalMesh* > SourceMeshList;
	TArray<FVector4> uvSections;
	for( INT idx = 0; idx < Parts.Num(); ++idx )
	{
		if( !Parts(idx).Mesh )
			continue;
		SourceMeshList.AddItem( Parts(idx).Mesh );

		FVector4 uv( Parts(idx).OrigX, Parts(idx).OrigY, Parts(idx).SizeX, Parts(idx).SizeY );
		uvSections.AddItem( uv );
	}

	TArray< FSkelMeshMergeSectionMapping > InForceSectionMapping;
	FSkeletalMeshMerge MeshMergeUtil( this, SourceMeshList, InForceSectionMapping, 0 );

	MeshMergeUtil.SetUVRemapping( TargetTexSizeX, TargetTexSizeY, uvSections );
	if( !MeshMergeUtil.DoMerge( FALSE ) )
	{
		return FALSE;
	}

	return TRUE;
}

UBOOL UMMOSkeletalMeshMerge::MergeTextures()
{
	for( INT idx = 0; idx < Parts.Num(); ++idx )
	{
		INT count = Parts(idx).Textures.Num();
		if( Parts(idx).Textures_N.Num() != count || Parts(idx).Textures_S.Num() != count )
			return FALSE;
		for( INT layer = 0; layer < count; ++layer )
		{
			if( !Parts(idx).Textures(layer) || !Parts(idx).Textures_N(layer) || !Parts(idx).Textures_S(layer) )
				return FALSE;
		}
	}
	//if( MergedTex == NULL )
	//{
	//	MergedTex = CastChecked<UTexture2DComposite>( 
	//		StaticConstructObject(UTexture2DComposite::StaticClass(), GetTransientPackage(), NAME_None, RF_Transient));
	//}
	//else
	//{
	//	MergedTex->SourceRegions.Empty();
	//}

	//MergedTex->DestSizeX = TargetTexSizeX;
	//MergedTex->DestSizeY = TargetTexSizeY;

	if( !TexMergeUtil )
	{
		TexMergeUtil = new FTextureMergeUtil();
	}
	else
	{
		TexMergeUtil->SourceRegions_D.Empty();
		TexMergeUtil->SourceRegions_N.Empty();
		TexMergeUtil->SourceRegions_S.Empty();
	}

	UBOOL dirty;
	for( INT idx = 0; idx < Parts.Num(); ++idx )
	{
		dirty = FALSE;

		FTextureMergeRegion srcReg_D;
		FTextureMergeRegion srcReg_N;
		FTextureMergeRegion srcReg_S;

		srcReg_D.OffsetX = 0;
		srcReg_D.OffsetY = 0;
		srcReg_D.SizeX = Parts(idx).SizeX;
		srcReg_D.SizeY = Parts(idx).SizeY;
		srcReg_D.DestOffsetX = Parts(idx).OrigX;
		srcReg_D.DestOffsetY = Parts(idx).OrigY;

		srcReg_N.OffsetX = 0;
		srcReg_N.OffsetY = 0;
		srcReg_N.SizeX = Parts(idx).SizeX;
		srcReg_N.SizeY = Parts(idx).SizeY;
		srcReg_N.DestOffsetX = Parts(idx).OrigX;
		srcReg_N.DestOffsetY = Parts(idx).OrigY;

		srcReg_S.OffsetX = 0;
		srcReg_S.OffsetY = 0;
		srcReg_S.SizeX = Parts(idx).SizeX;
		srcReg_S.SizeY = Parts(idx).SizeY;
		srcReg_S.DestOffsetX = Parts(idx).OrigX;
		srcReg_S.DestOffsetY = Parts(idx).OrigY;

		for( INT idx2 = 0; idx2 < Parts(idx).Textures.Num(); ++idx2 )
		{
			srcReg_D.Textures.AddItem( Parts(idx).Textures(idx2) );
			srcReg_N.Textures.AddItem( Parts(idx).Textures_N(idx2) );
			srcReg_S.Textures.AddItem( Parts(idx).Textures_S(idx2) );
		}
		TexMergeUtil->SourceRegions_D.AddItem( srcReg_D );
		TexMergeUtil->SourceRegions_N.AddItem( srcReg_N );
		TexMergeUtil->SourceRegions_S.AddItem( srcReg_S );
	}
	TexMergeUtil->TargetTexture_D = MergedTex_D;
	TexMergeUtil->TargetTexture_N = MergedTex_N;
	TexMergeUtil->TargetTexture_S = MergedTex_S;

	//MergedTex->UpdateCompositeTexture( 0 );
	//return MergedTex;

	return TexMergeUtil->DoMerge();
}

void UMMOSkeletalMeshMerge::PostMerge()
{
	//UMaterial* parent = CastChecked<UMaterial>( StaticLoadObject( 
	//			UMaterial::StaticClass(), 0, TEXT("MMO_MaterialTemplate.Character.MT_Opaque"), 0, 0, 0 ) );

	for( INT i = 0; i < Materials.Num(); ++i )
	{
		UMaterialInstance* pIns = (UMaterialInstance*)Materials(i);
		if( pIns && pIns->Parent == NewMaterialTemplate )
		{
			//pIns->SetParent( parent );
			//pIns->SetTextureParameterValue( "Diffuse_Map", MergedTex_D );
			//pIns->SetTextureParameterValue( "Normal_Map", MergedTex_N );
			//pIns->SetTextureParameterValue( "Specular_Map", MergedTex_S );
		}
		else
		{
			pIns = CastChecked<UMaterialInstanceConstant>( StaticConstructObject(
				UMaterialInstanceConstant::StaticClass(), GetOuter() ) );

			pIns->SetParent( NewMaterialTemplate );

			Materials(i) = pIns;
		}
		pIns->SetTextureParameterValue( "Diffuse_Map", MergedTex_D );
		pIns->SetTextureParameterValue( "Normal_Map", MergedTex_N );
		pIns->SetTextureParameterValue( "Specular_Map", MergedTex_S );

		//pIns->SetScalarParameterValue( "Specular_Strength", 0.f );
	}
}

void UMMOSkeletalMeshMerge::BeginDestroy()
{
	// delete texture merge utility here, which will cancel the possible merging
	if( TexMergeUtil )
	{
		delete TexMergeUtil;
	}
	Super::BeginDestroy();
}

UBOOL UMMOSkeletalMeshMerge::Equals( const UMMOSkeletalMeshMerge* other ) const
{
	if( other == NULL )
		return FALSE;

	if( Parts.Num() != other->Parts.Num() )
		return FALSE;
	for( INT idx = 0; idx < Parts.Num(); ++idx )
	{
		if( Parts(idx).Mesh != other->Parts(idx).Mesh )
			return FALSE;

		if( Parts(idx).Textures.Num() != other->Parts(idx).Textures.Num() )
			return FALSE;

		for( INT idx2 = 0; idx2 < Parts(idx).Textures.Num(); ++idx2 )
		{
			if( Parts(idx).Textures(idx2) != other->Parts(idx).Textures(idx2) )
				return FALSE;
		}
	}
	return TRUE;
}

void UMMOSkeletalMeshMerge::SetSkinColor( FLinearColor NewColor )
{
	if (Materials.Num() <= 0)
		return;

	UMaterialInstance* pIns = (UMaterialInstance*)Materials(0);
	if( pIns )
	{
		pIns->SetVectorParameterValue( "SkinColor", NewColor );
	}
}

void UMMOSkeletalMeshMerge::SetSkinSpecular( FLinearColor NewSpecular )
{
	if (Materials.Num() <= 0)
		return;

	UMaterialInstance* pIns = (UMaterialInstance*)Materials(0);
	if( pIns )
	{
		pIns->SetVectorParameterValue( "SkinSpecular", NewSpecular );
	}
}

void UMMOSkeletalMeshMerge::SetFakeSilhouetteColor( FLinearColor NewFakeSilhouette )
{
	if (Materials.Num() <= 0)
		return;

	UMaterialInstance* pIns = (UMaterialInstance*)Materials(0);
	if( pIns )
	{
		pIns->SetVectorParameterValue( "SilhouetteColor", NewFakeSilhouette );
	}
}