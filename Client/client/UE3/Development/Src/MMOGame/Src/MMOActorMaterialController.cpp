/*=============================================================================
	MMOActorMaterialController.cpp: Manages the materials of actors
	ming@snda
=============================================================================*/

#include "stdafx.h"

void UMMOActorMaterialController::CopyMaterialInstanceParameters( UMaterial* DestMat, 
	UMaterialInstance* DestMatInst, UMaterialInstance* SrcMatInst )
{
	if( !DestMat || !DestMatInst || !SrcMatInst )
		return;

	TArray< FName > ParamNames;
	TArray< FGuid >	ParamIds;

	// vector params
	DestMat->GetAllVectorParameterNames( ParamNames, ParamIds );
	for( int i = 0; i < ParamNames.Num(); ++i )
	{
		FName paramName = ParamNames(i);
		FLinearColor vecValue;
		if( SrcMatInst->GetVectorParameterValue( paramName, vecValue ) )
		{
			DestMatInst->SetVectorParameterValue( paramName, vecValue );
		}
	}

	// scalar params
	DestMat->GetAllScalarParameterNames( ParamNames, ParamIds );
	for( int i = 0; i < ParamNames.Num(); ++i )
	{
		FName paramName = ParamNames(i);
		float fValue;
		if( SrcMatInst->GetScalarParameterValue( paramName, fValue ) )
		{
			DestMatInst->SetScalarParameterValue( paramName, fValue );
		}
	}
	
	// texture params
	DestMat->GetAllTextureParameterNames( ParamNames, ParamIds );
	for( int i = 0; i < ParamNames.Num(); ++i )
	{
		FName paramName = ParamNames(i);
		UTexture* texValue;
		if( SrcMatInst->GetTextureParameterValue( paramName, texValue ) )
		{
			DestMatInst->SetTextureParameterValue( paramName, texValue );
		}
	}
}

void UMMOActorMaterialController::SetTransparentMaterial( UBOOL bTransparent )
{
	if( Mesh != NULL && IsTransparent != bTransparent )
	{
		for( int i = 0; i < Mesh->GetNumElements(); ++i )
		{
			for( int j = 0; j < MeshMaterials.Num(); ++j )
			{
				if( MeshMaterials(j).Id == i )
				{
					if( bTransparent )
					{
						Mesh->SetMaterial( i, MeshMaterials(j).OpacityMaterial );
					}
					else
					{
						Mesh->SetMaterial( i, MeshMaterials(j).OriginalMaterial );
					}
					break;
				}
			}
		}
		IsTransparent = bTransparent;
	}
}

void UMMOActorMaterialController::SetOpacityPercent( float percent )
{
	if( !IsTransparent )
		return;
	for( int i = 0; i < MeshMaterials.Num(); ++i )
	{
		if(MeshMaterials(i).OpacityMaterial)
		{
			//warnf(NAME_Error, TEXT("___[%s]%f"), *MeshMaterials(i).OpacityMaterial->GetName(), percent);
			MeshMaterials(i).OpacityMaterial->SetScalarParameterValue( "OpacityPercent", percent );
		}
	}
	CurrentOpacity = percent;
}

void UMMOActorMaterialController::NativeTick( FLOAT deltaTime )
{
	if( IsFading )
	{
		if( CurrentOpacity < DestOpacity && FadeSpeed > 0.f )
		{
			CurrentOpacity += FadeSpeed * deltaTime;
			if( CurrentOpacity >= DestOpacity )
			{
				SetOpacity( DestOpacity );
				IsFading = false;
				//warnf(NAME_Error, TEXT("___ FadeIn OK %f"), DestOpacity);

				if( DestOpacity >= 0.999999f )
				{
					SetTransparentMaterial( false );
				}
			}
			else
			{
				SetOpacityPercent( CurrentOpacity );
			}
		}
		else if( CurrentOpacity > DestOpacity && FadeSpeed < 0.f )
		{
			CurrentOpacity += FadeSpeed * deltaTime;
			if( CurrentOpacity <= DestOpacity )
			{
				//warnf(NAME_Error, TEXT("___ FadeOut OK %f"), DestOpacity);
				SetOpacity( DestOpacity );
				IsFading = false;
			}
			else
			{
				SetOpacityPercent( CurrentOpacity );
			}
		}
	}
}

void UMMOActorMaterialController::SetOpacity(FLOAT percent)
{
	if( percent < 1.f )
	{
		//SetTransparentMaterial( true );
		SetOpacityPercent( percent );
	}
	else
	{
		SetTransparentMaterial( false );
	}
}

// Play translucent fade-in
void UMMOActorMaterialController::PlayOpacityFadeIn( FLOAT fadeInTime )
{
	// if one is fading out, start fading in from its current OpacityPercent
	if(!IsFading)
	{
		SetOpacityPercent( 0.0f );
	}

	IsFading = true;
	SetTransparentMaterial( true );

	DestOpacity = 1.f;
	
	if(fadeInTime > 0)
	{
		FadeSpeed = 1.0f / fadeInTime;
	}
	else
	{
		FadeSpeed = 5.0f;
	}

	if( FadeSpeed < 0.01f )
	{
		warnf(NAME_Error, TEXT("[MMO Debug] Incorrect fade in speed:%f FadeTime:%f"), FadeSpeed, fadeInTime );
		FadeSpeed = 5.0f;
	}
}

// Play translucent fade-out
void UMMOActorMaterialController::PlayOpacityFadeOut( FLOAT fadeOutTime )
{
	// if one is fading in, start fading out from its current OpacityPercent
	if(!IsFading)
	{
		SetOpacityPercent( 1.f );
	}

	IsFading = true;
	SetTransparentMaterial( true );
	DestOpacity = 0.f;

	if(fadeOutTime > 0)
	{
		FadeSpeed = -1.0f / fadeOutTime;
	}
	else
	{
		FadeSpeed = -5.0f;
	}

	if( FadeSpeed > -0.01f )
	{
		warnf(NAME_Error, TEXT("[MMO Debug] Incorrect fade out speed:%f FadeTime:%f"), FadeSpeed, fadeOutTime );
		FadeSpeed = -5.0f;
	}
}
