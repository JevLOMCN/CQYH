#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "ICanvasRenderable.h"

IMPLEMENT_CLASS(AMMOHud)
IMPLEMENT_CLASS(UMMOGFXLoadPlayer)

typedef std::vector<class ICanvasRenderable*, M_UE3_STL_ALLOC(class ICanvasRenderable*) > CanvasRenderableList;
CanvasRenderableList g_canvasRenderable;

TArray<IIMMOActorHUD*> g_ActorHUDList;

extern void MMOUIWingPanel_DrawPreview(UCanvas* Canvas);	// MMOUIWingPanel.cpp

void AMMOHud::RegisterCanvasRenderable( class ICanvasRenderable* obj )
{
	g_canvasRenderable.push_back(obj);
}

void AMMOHud::UnRegisterCanvasRenderable( class ICanvasRenderable* obj )
{
	CanvasRenderableList::iterator i = std::find(g_canvasRenderable.begin(), g_canvasRenderable.end(), obj);
	if(i != g_canvasRenderable.end())
	{
		g_canvasRenderable.erase(i);
	}
}

void AMMOHud::AddDrawHUD(IIMMOActorHUD* h)
{
	g_ActorHUDList.AddUniqueItem(h);
}

void AMMOHud::RemoveDrawHUD(IIMMOActorHUD* h)
{
	g_ActorHUDList.RemoveItem(h);
}

void AMMOHud::DrawHUD()
{
	if(GIsEditor)
	{
		return;
	}

	MMOUI::GetUIManager()->SetSceneView(Canvas);

	for ( INT i = 0 ; i < MMOUI::GetUIManager()->UILevel.Num() ; i++ )
	{
		TScriptInterface<class IIMMOActorHUD> uiLevel = ( MMOUI::GetUIManager()->UILevel(i) );
		if( uiLevel )
		{
			uiLevel->DrawMyHUD(this);
		}
	}

	for (INT i = 0; i < g_ActorHUDList.Num(); ++i)
	{
		g_ActorHUDList(i)->DrawMyHUD(this);
	}


	for (size_t i = 0; i < g_canvasRenderable.size(); ++i)
	{
		g_canvasRenderable[i]->OnCanvasPostRender(Canvas, fDeltaTime);
	}

	MMOUIWingPanel_DrawPreview(Canvas);

	//AMMOPlayerController* pc = Cast<AMMOPlayerController>( PlayerOwner );
}

//DWORD g_moveCycles = 0;

void AMMOHud::DrawHUDPostGFx()
{
	Super::DrawHUDPostGFx();
	for (INT i = 0; i < g_ActorHUDList.Num(); ++i)
	{
		g_ActorHUDList(i)->DrawHUDPostGFx(this);
	}

	//DrawString(Canvas->Canvas, 500, 10, *appItoa(g_moveCycles), GEngine->SmallFont, FColor(255,255,0));
}


// todo: move uc to cpp
//void AMMOHud::ShowHitStates()
//{
//	FVector screenPos;
//	FVector2D screenSize;
//
//	for( int i = 0 ; i < MaxHitNumber ; ++i )
//	{
//		if(thisHit->Length() >0)
//		{
//			screenPos = Canvas->Project(thisHit(0)->HitLocation);
//		
//			MMOGame( WorldInfo.Game ).GetViewportClient().GetViewportSize(screenSize);
//		
//			screenPos.X = screenPos.X + Rand(DamageRandomRange) -DamageRandomRange/2;
//			screenPos.Y = screenPos.Y + Rand(DamageRandomRange) -DamageRandomRange/2;
//
//			if( thisHit(0)->dmg.HPChange > 0 )
//			{
//				class'MMOUIManager'.static.ShowHPRestore( screenPos.X , screenPos.y , thisHit[0].dmg.HPChange ,thisHit[0].HitLocation);
//			}
//			else
//			{
//				switch(thisHit[0].dmg.Type)
//				{
//					case DRT_Result7: //爆击
//						if( thisHit[0].dmg.MainRole )				
//						{
//							class'MMOUIManager'.static.ShowCriticaledNumber(  screenPos.X, screenPos.y , Abs( thisHit[0].dmg.HPChange ),thisHit[0].HitLocation );
//						}
//						else
//						{
//							class'MMOUIManager'.static.ShowCriticalNumber(  screenPos.X, screenPos.y , Abs( thisHit[0].dmg.HPChange ) ,thisHit[0].HitLocation);
//						}
//						break;
//					case DRT_Result2: //免疫
//						class'MMOUIManager'.static.ShowImmunity( screenPos.X, screenPos.y ,thisHit[0].HitLocation);
//						break;
//					case DRT_Result1: //未命中
//						class'MMOUIManager'.static.ShowMiss( screenPos.X, screenPos.y ,thisHit[0].HitLocation);
//						break;
//					case DRT_Result3: //格档
//						class'MMOUIManager'.static.ShowBlockNumber( screenPos.X, screenPos.y , Abs( thisHit[0].dmg.HPChange ) ,thisHit[0].HitLocation);
//						break;
//					case DRT_Result5: //闪避
//						class'MMOUIManager'.static.ShowDodge( screenPos.X, screenPos.y ,thisHit[0].HitLocation);
//						break;
//					case DRT_Result4: //招架
//						class'MMOUIManager'.static.ShowParry( screenPos.X, screenPos.y ,thisHit[0].HitLocation);
//						break;
//					case DRT_Result6:
//						if( thisHit[0].dmg.MainRole )				
//						{
//							class'MMOUIManager'.static.ShowAttackedNumber(  screenPos.X, screenPos.y , Abs( thisHit[0].dmg.HPChange ) ,thisHit[0].HitLocation);
//						}
//						else
//						{
//							class'MMOUIManager'.static.ShowAttackNumber(  screenPos.X , screenPos.y , Abs( thisHit[0].dmg.HPChange ) ,thisHit[0].HitLocation);
//						}
//						break;
//					case DRT_Result0:
//						//if( thisHit[0].dmg.HPChange > 0 )
//						//{
//						//	class'MMOUIManager'.static.ShowHPRestore( screenPos.X , screenPos.y , thisHit[0].dmg.HPChange );
//						//}
//						break;
//				}
//			}
//
//			thisHit.Remove(0,1);
//		}
//		else
//		{
//			break;
//		}
//	}
//}

void AMMOHud::NativePostBeginPlay(class ULocalPlayer* LocPlayer)
{
	//if(!GIsEditor)
	{
		MMOUI::LoadAllMoviePlayers(this, LocPlayer);
	}
}
//------------------------------------------------------------------------
AActor* AMMOHud::PickActors(FVector StartWorldPos,FVector EndWorldPos)
{
	FCheckResult* HitR = MMO::PickActor(StartWorldPos, EndWorldPos, this);

	if(HitR)
	{
		return HitR->Actor;
	}

	return 0;
}
//------------------------------------------------------------------------
void AMMOHud::ShowSilhouette(class AActor* Actor, UBOOL on, FVector Color)
{
	for(INT i = 0; i < Actor->Components.Num(); ++i)
	{
		UPrimitiveComponent* comp = Cast<UPrimitiveComponent>(Actor->Components(i));
		if(comp)
		{
			comp->UpdateSilhouetteState( on, Color );
			//comp->ShowSilhouette = on;
			//comp->SilhouetteColor = Color;
		}
	}				
}

UBOOL AMMOHud::IsOnCanvas(FVector screenPos,FLOAT width,FLOAT height)
{
	return !(screenPos.X < Canvas->OrgX-width/2 || screenPos.Y < Canvas->OrgY-height/2 || screenPos.X > Canvas->ClipX+width/2 || screenPos.Y > Canvas->ClipY+height/2);
}
//void AMMOHud::Tick(FLOAT DeltaTime)
//{
//	fDeltaTime = DeltaTime;
//}
//------------------------------------------------------------------------

void AMMOHud::AddPostRenderDelegate(FScriptDelegate fun)
{
	vecOnPostRender.AddItem( fun );
}

void AMMOHud::NativePostRender()
{
	AMMOPlayerController* PC = Cast<AMMOPlayerController>(PlayerOwner);
	if(!PC)
	{
		return;
	}

	ULocalPlayer* LP = Cast<ULocalPlayer>(PC->Player);
	if(!LP)
	{
		return;
	}

	// Get mouse position
	FVector2D MousePos = LP->ViewportClient->GetMousePosition();
	FVector WorldOrigin, WorldDir;
	if(Canvas && Canvas->SceneView)
	{
		Canvas->SceneView->DeprojectFVector2D(MousePos, WorldOrigin, WorldDir);
	}
	else
	{
		return;
	}

	// Do the Pick
	PC->PlayerMouse = MousePos;
	PC->MouseProjectionOrigin = WorldOrigin;
	PC->MouseProjectionDirection = WorldDir;

	AMMOPlayerCamera* PCCam = Cast<AMMOPlayerCamera>(PC->PlayerCamera);
	if(!PCCam)
	{
		return;
	}

	AActor* OldTraceActor = PC->GetTraceActor();

	PC->StartTrace = PCCam->CameraCache.POV.Location;// PCCam->ViewTarget.POV.Location + WorldDir * 10.0f;
	PC->EndTrace = PC->StartTrace + WorldDir * 50000.0f;	// sometimes the obj is far away more than 5000, modify it to 50000

	UBOOL bNeedSort = TRUE;
	AMMOActor* MainActor = Cast<AMMOActor>(PC->Pawn);
	if (PC->bInAutoFollowState || (MainActor && MainActor->RotateToMousePointRefrence > 0))
	{
		bNeedSort = FALSE;
	}
	
	FCheckResult* HitR = MMO::PickActor(PC->StartTrace, PC->EndTrace, this, TRUE);

	if(!HitR)
	{
		PC->UpdateCursor();
		return;
	}

	//typedef TScriptInterface<IIPickableActor> IPickableActor;
	//IPickableActor pickable = ScriptInterfaceCast<IIPickableActor>(HitR->Actor);
	//UBOOL CaptureInput = MMO::Facade::GetCaptureInput();
	//if(pickable && CaptureInput==FALSE)
	//{
	//	MMOFacade::getSingleton().SetMouseCursor((EMMOCursorType)pickable->GetMouseCursor());
	//}
	//else
	//{
	//	MMOFacade::getSingleton().SetMouseCursor(EMC_Arrow);
	//}

	PC->SetTraceActor(HitR->Actor);

	if(PC->GetTraceActor())
	{
		PC->MouseHitWorldNormal = HitR->Normal;

		APawn* PawnActor = Cast<APawn>(PC->GetTraceActor());
		if(PawnActor)
		{	
			// put the Z onto ground
			PC->MouseHitWorldLocation = PC->GetTraceActor()->Location;
			PC->MouseHitWorldLocation.Z -=
				PawnActor->CylinderComponent ? PawnActor->CylinderComponent->CollisionHeight : 0.0f;
		}
		else if (HitR->Time != 0)
		{
			// means actully picked, zhaojun@snda
			PC->MouseHitWorldLocation = HitR->Location;
		}
		else
		{
			// only hit aabb, do nothing, zhaojun@snda
			//PC->MouseHitWorldLocation = PC->GetTraceActor()->Location;
		}

		//PC->DrawDebugLine(PC->Pawn->Location, PC->MouseHitWorldLocation, 255, 0, 0, FALSE);

#if !FINAL_RELEASE
		//计算角色的视角位置为了调试光线和检查移动后的碰撞
		if(PlayerOwner->ViewTarget)
		{		
			APawn* ViewTar = Cast<APawn>(PlayerOwner->ViewTarget);

			if(ViewTar)
			{
				PC->PawnEyeLocation = ViewTar->Location + ViewTar->EyeHeight * FVector(0.0f, 0.0f, 1.0f);
			}
		}
#endif
	}

	PC->UpdateGridFlag();

	PC->UpdateCursor();
}
//------------------------------------------------------------------------
void AMMOHud::PostCreateUI()
{
}

//------------------------------------------------------------------------
void AMMOHud::AddGainInfo(BYTE Type,INT Value,FVector Location,INT subType)
{
	FGainInfo GI(EC_EventParm);
	GI.Type = Type;
	GI.Value = Value;
	GI.Location = Location;
	GI.SubType = subType;
	GainInfoList.AddItem( GI );
}

void AMMOHud::UpdateDamgeTextNode(FLOAT DeltaSeconds)
{
	Super::TickSpecial(DeltaSeconds);
	for (INT i = 0; i < DamageTextLifeSpan.Num(); ++i)
	{
		if (DamageTextLifeSpan(i) > 0.f)
		{
			DamageTextLifeSpan(i) -= DeltaSeconds;
		}
	}
}

UBOOL AMMOHud::AddNewDamgeTextNode()
{
	if (DamageTextLifeSpan.Num() < MaxHitNumber)
	{
		DamageTextLifeSpan.AddZeroed();
	}

	for (INT i = 0; i < DamageTextLifeSpan.Num(); ++i)
	{
		if (DamageTextLifeSpan(i) <= 0.f)
		{
			DamageTextLifeSpan(i) = DefualtDamageTextLifespan;
			return TRUE;
		}
	}

	return FALSE;
}

void AMMOHud::AddHitStats(struct FDamageResult dmg,FVector HitLocation)
{
	extern INT g_Efficiency;
	if(g_Efficiency>=1)return;

	if (AddNewDamgeTextNode())
	{

		INT idx = thisHit.AddZeroed();
		if(dmg.HPChange>0)
		{
			//warnf(NAME_Warning, TEXT("======!!!!==================================== AMMOHud::AddHitStats("));
		}
		thisHit(idx).dmg = dmg;
		thisHit(idx).HitLocation = HitLocation;
	}
}

void AMMOHud::SetWidgetInCenter( UGFxObject *Obj)
{
	//使控件居中
	//子级控件的得到的是相对位置 这里 还需要从头计算

	float posx,posy;
	UGFxObject *Parent;
	UGFxObject *Root;
	Obj->GetPosition(posx,posy);
	float pox,poy;
	pox = Obj->GetFloat("x");
	poy = Obj->GetFloat("y");
	posx = CenterX -Obj->GetFloat("width") / 2.0f ;
	posy = CenterY - Obj->GetFloat("height") / 2.0f;

	Parent = Obj->GetObject("parent");
	Root = Obj->GetObject("root");
	while ( Parent && Parent != Root )
	{
		pox = Parent->GetFloat("x");
		poy = Parent->GetFloat("y");
		//需要转化为本地的相对坐标
		posx -= pox;
		posy -=	poy;
		Parent = Parent->GetObject("parent");
	}
	Obj->SetPosition( posx, posy );
}