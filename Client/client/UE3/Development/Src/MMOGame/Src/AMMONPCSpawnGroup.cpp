#include "stdafx.h"
//#include "MMOGameClasses.h"
#include "MMOUtil.h"
#include "MMOFacade.h"
#include "../../Engine/Src/SceneRenderTargets.h"

IMPLEMENT_CLASS(AMMONPCSpawnGroup)

#if WITH_EDITOR
void AMMONPCSpawnGroup::CheckForErrors()
{
	GWarn->MapCheck_Add(MCTYPE_ERROR, this, 
		*LocalizeUnrealEdMMO(TEXT("CheckError_DeprecatedClass")),
		NULL);
}
#endif
//------------------------------------------------------------------------
FVector AMMONPCSpawnGroup::GetObjectServerPosition()
{
	FVector result = Location;
	ConvertCoordC2S(result);
	result.X = (INT32)result.X / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	result.Y = (INT32)result.Y / GRID_WIDTH * GRID_WIDTH + GRID_WIDTH / 2;
	return result;
}
//------------------------------------------------------------------------
TArray<class ANPCSpawnPoint*> AMMONPCSpawnGroup::GetNPCSpawnPoints()
{
	return NPCSpawnPoints;
}
/** Static: Copies a texture to a render target texture */
void CopyTextureToRenderTargetTexture( UTexture* SourceTexture, UTextureRenderTarget2D* RenderTargetTexture )
{	
	check( SourceTexture != NULL );
	check( RenderTargetTexture != NULL );
	/*
	// Begin scene
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		BeginDrawingCommand,
		FViewport*,Viewport,Viewport,
	{
		Viewport->BeginRenderFrame();
	});*/

	// Grab the actual render target resource from the texture.  Note that we're absolutely NOT ALLOWED to
	// dereference this pointer.  We're just passing it along to other functions that will use it on the render
	// thread.  The only thing we're allowed to do is check to see if it's NULL or not.
	FTextureRenderTargetResource* RenderTargetResource = RenderTargetTexture->GameThread_GetRenderTargetResource();
	check( RenderTargetResource != NULL );
	{
		// Create a canvas for the render target and clear it to black
		FCanvas  Canvas( RenderTargetResource, NULL );

		const UINT Width = RenderTargetTexture->GetSurfaceWidth();
		const UINT Height = RenderTargetTexture->GetSurfaceHeight();

		// Grab the texture resource.  We only support 2D textures and render target textures here.
		FTexture* TextureResource = NULL;
		UTexture2D* Texture2D = Cast<UTexture2D>( SourceTexture );
		if( Texture2D != NULL )
		{
			TextureResource = Texture2D->Resource;
		}
		else
		{
			UTextureRenderTarget2D* TextureRenderTarget2D = Cast<UTextureRenderTarget2D>( SourceTexture );
			TextureResource = TextureRenderTarget2D->GameThread_GetRenderTargetResource();
		}
		check( TextureResource != NULL );
		// Draw a quad to copy the texture over to the render target
		{		
			const FLOAT MinU = 0.0f;
			const FLOAT MinV = 0.0f;
			const FLOAT MaxU = 1.0f;
			const FLOAT MaxV = 1.0f;
			const FLOAT MinX = 0.0f;
			const FLOAT MinY = 0.0f;
			const FLOAT MaxX = Width;
			const FLOAT MaxY = Height;

			DrawTriangle2D(
				&Canvas,
				FVector2D( MinX, MinY ),
				FVector2D( MinU, MinV ),
				FVector2D( MaxX, MinY ),
				FVector2D( MaxU, MinV ),
				FVector2D( MaxX, MaxY ),
				FVector2D( MaxU, MaxV ),
				FLinearColor::White,
				TextureResource,
				FALSE );		// Alpha blend?

			DrawTriangle2D(
				&Canvas,
				FVector2D( MaxX, MaxY ),
				FVector2D( MaxU, MaxV ),
				FVector2D( MinX, MaxY ),
				FVector2D( MinU, MaxV ),
				FVector2D( MinX, MinY ),
				FVector2D( MinU, MinV ),
				FLinearColor::White,
				TextureResource,
				FALSE );		// Alpha blend?
		}
		// Tell the rendering thread to draw any remaining batched elements
		Canvas.Flush(TRUE);
	}
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			UpdateMeshPaintRTCommand4,
			FTextureRenderTargetResource*, RenderTargetResource, RenderTargetResource,
		{
			// Copy (resolve) the rendered image from the frame buffer to its render target texture
			RHICopyToResolveTarget(
				RenderTargetResource->GetRenderTargetSurface(),		// Source texture
				TRUE,												// Do we need the source image content again?
				FResolveParams() );									// Resolve parameters
		});

	}
	  /*
	// End scene
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		EndDrawingCommand,
		FViewport*,Viewport,Viewport,
	{
		const UBOOL bShouldPresent = FALSE;
		const UBOOL bLockToVSync = FALSE;
		Viewport->EndRenderFrame( bShouldPresent, bLockToVSync );
	});
	*/
}

 void AMMONPCSpawnGroup::IniSpirite()
{
	/*USpriteComponent *Sprite =NULL ;
	//First take out the Sprite component
	for(UINT ComponentIndex = 0 ; ComponentIndex < (UINT)this->Components.Num() ; ++ ComponentIndex)
	{
		if( this->Components(ComponentIndex)->IsA( USpriteComponent::StaticClass() ) )
		{
			Sprite = Cast<USpriteComponent>(  this->Components(ComponentIndex) );
			break;
		}
	}
	if( Sprite !=NULL )
	{
		UTextureRenderTarget2D *rtTarget2D;
		rtTarget2D->Init( SpawnGroupSpriteTex->SizeX, SpawnGroupSpriteTex->SizeY, SpawnGroupSpriteTex->Format, this );	
		// Copy the normal Sprite image (created when initializing the component) onto the RT
		if( SpawnGroupSpriteTex!= NULL )
		{
			CopyTextureToRenderTargetTexture( SpawnGroupSpriteTex,rtTarget2D );
		}
		// Then draw the related GroupID via Canvas
		FCanvas Canvas( rtTarget2D, NULL );

		DrawString(&Canvas, 5, 0, *FString::Printf( TEXT("NPCSpawn Group ID:%d __NPC Count: %d"), AutoServerID , NPCSpawnPoints.Num() ), GEngine->SmallFont, FColor(255,0,0) );

		//Notify the rendering thread
		Canvas.Flush(TRUE);


		//UTexture2D* newTex = rtTarget2D;
		//Sprite->SetSprite ( newTex );
		//Done, submit
		FlushRenderingCommands();
	}
	*/
			//Add to render queue
	/*	{
			ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
				UpdateMeshPaintRTCommand3,
				FSceneRenderTargets*, rtTarget2D, rtTarget2D,
			{
				// Copy (resolve) the rendered image from the frame buffer to its render target texture
				RHICopyToResolveTarget(
					rtTarget2D->GetRenderTargetSurface(),		// Source texture
					TRUE,												// Do we need the source image content again?
					FResolveParams() );									// Resolve parameters
			});

		}*/
}

//------------------------------------------------------------------------
// ServerTriggerEditorComponent
//------------------------------------------------------------------------
IMPLEMENT_CLASS(UMMONpcSpawnGroupEditorComponent);

class MMONpcSpawnGroupSceneProxy : public FPrimitiveSceneProxy
{
public:
	MMONpcSpawnGroupSceneProxy(const UPrimitiveComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
	{
		MMONpcSpawnGroupActor =static_cast< AMMONPCSpawnGroup * >( InComponent->GetOwner() );
		check(MMONpcSpawnGroupActor);
	}

	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI,const FSceneView* View,UINT DPGIndex,DWORD Flags = 0)
	{
		//FCanvas Canvas(View->Family->RenderTarget,NULL);

		if(MMONpcSpawnGroupActor->IsSelected())
		{
			const TArray<class ANPCSpawnPoint*>& NPCSpawns = MMONpcSpawnGroupActor->GetNPCSpawnPoints();
			for (INT i = 0; i < NPCSpawns.Num(); ++i)
			{
				//A newly added SpawnPoint has empty content, drawing it directly would crash
				if( NPCSpawns(i) != NULL )
					PDI->DrawLine(MMONpcSpawnGroupActor->Location, NPCSpawns(i)->Location, FColor(255, 255, 0), SDPG_UnrealEdForeground, 3.0f );
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View)
	{
		FPrimitiveViewRelevance Result;
		Result.bDynamicRelevance = TRUE;
		Result.SetDPG( SDPG_UnrealEdForeground,TRUE );
		return Result;
	}

	virtual DWORD GetMemoryFootprint( void ) const { return( sizeof( *this ) + GetAllocatedSize() ); }
	DWORD GetAllocatedSize( void ) const { return( FPrimitiveSceneProxy::GetAllocatedSize() ); }

protected:

	AMMONPCSpawnGroup* MMONpcSpawnGroupActor;
};

FPrimitiveSceneProxy* UMMONpcSpawnGroupEditorComponent::CreateSceneProxy()
{
	return new MMONpcSpawnGroupSceneProxy(this);
}
//------------------------------------------------------------------------
void UMMONpcSpawnGroupEditorComponent::UpdateBounds()
{
	const FLOAT NewScale = (Owner ? Owner->DrawScale : 1.0f) * 512;
	Bounds = FBoxSphereBounds(GetOrigin(),FVector(NewScale,NewScale,NewScale),appSqrt(3.0f * Square(NewScale)));
}
//------------------------------------------------------------------------