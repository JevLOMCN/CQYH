#include "stdafx.h"
#include "MMOFacade.h"
#include "TeamManager.h"
#include "FriendManager.h"
#include "MasterStudentManager.h"
#include "MMOGuildData.h"
#include "MMOMapDataManager.h"

FString AMMOPlayer::GetActorName()
{
	FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	if(curMapInfo.HideRoleName && IsMainPlayer()==FALSE)
	{
		return FString(TEXT(" "));
	}
	return ActorName;
}

bool AMMOPlayer::ValidateMoveDest(const FVector& dest) const
{
	if(IsMainPlayer())
	{
		INT64 llSrcX = 0;
		INT64 llSrcY = 0;
		INT64 llSrcZ = 0;
		ZONE_DATA* pDestZone = NULL;
		GRID_DATA* pDestGrid = NULL;

		gServerGridWorld.ConvertPosClient2Server(dest, llSrcX, llSrcY, llSrcZ);
		gServerGridWorld.GetZoneAndGrid(llSrcX, llSrcY, llSrcZ, pDestZone, pDestGrid);

		if(pDestGrid)
		{
			if(pDestGrid->bOutOfPoly)
			{
				SET_WARN_COLOR(COLOR_GREEN);
				warnf(NAME_Log, TEXT("Tried to move into a blocked cell: %s, move cancelled!"), *dest.ToString());
				CLEAR_WARN_COLOR();
				return false;
			}
		}
	}
	return true;
}

void AMMOPlayer::OnZoneOrGridChanged(void* pOldZonePter, void* pOldGridPter, void* pNewZonePter, void* pNewGridPter)	// zhaojun@snda
{
	__super::OnZoneOrGridChanged(pOldZonePter, pOldGridPter, pNewZonePter, pNewGridPter);
}


#ifdef _DEBUG
void AMMOPlayer::physWalking(FLOAT deltaTime, INT Iterations)
{
	Super::physWalking(deltaTime, Iterations);
}
#endif

void AMMOPlayer::moveDelta(FVector Delta)
{
#if M_DEBUG_CAMERA_MOVE
	static FColor clrList[] =
	{
		FColor(255, 0, 0),	
		FColor(255, 255, 0),
		FColor(0, 255, 0),
		FColor(0, 255, 255),
		FColor(0, 0, 255),
	};

	const UINT clrCnt = sizeof(clrList) / sizeof(FColor);
	static UINT debugClrIdx = 0;


	if(IsMainPlayer())
	{
		int i = 0;
		++i;
		UINT clrIdx = debugClrIdx % clrCnt;
		const FColor& clr = clrList[clrIdx];
		DrawDebugLine(Location, Location + Delta, clr.R, clr.G, clr.B, TRUE);

		ACamera* cam = MMOFacade::getSingleton().GetMainController()->PlayerCamera;
		static FVector lastCamPos = cam->CameraCache.POV.Location;

		FVector curCamPos = cam->CameraCache.POV.Location;

// 		FVector d1 = lastCamPos - Location;
// 		FVector d2 = curCamPos - Location;

		FVector d = curCamPos - lastCamPos;

		lastCamPos = curCamPos;

		DrawDebugLine(Location + FVector(100, 0, 0), Location + d + FVector(100, 0, 0), clr.R, clr.G, clr.B, TRUE);
	}

	++debugClrIdx;
#endif
	Super::moveDelta(Delta);

#if 0 && defined(_DEBUG) // oscar@snda
	INT64 llSrcX;
	INT64 llSrcY;
	INT64 llSrcZ;
	ZONE_DATA* pDestZone = NULL;
	GRID_DATA* pDestGrid = NULL;

	gServerGridWorld.ConvertPosClient2Server(Location, llSrcX, llSrcY, llSrcZ);
	gServerGridWorld.GetZoneAndGrid(llSrcX, llSrcY, llSrcZ, pDestZone, pDestGrid);

	if(pDestGrid)
	{
		if(pDestGrid->bOutOfPoly)
		{
			appDebugBreak();
		}
	}
#endif
}


void AMMOPlayer::performPhysics(FLOAT DeltaSeconds)
{
	if(IsInMatinee)
	{
		Physics = PHYS_Interpolating;
		physInterpolating(DeltaSeconds);
	}
	else
	{
		APawn::performPhysics(DeltaSeconds);
	}
}

FVector AMMOPlayer::GetInteractActorLocation()
{
	return Location;
}

FLOAT AMMOPlayer::GetMaxInteractRadius()
{
	return MMOFacade::getSingleton().GetMainController()->InteractDistance;
}

void AMMOPlayer::DoInteract()
{
	if(MoveType == EMT_InStall)
	{
		CMessageHandler::GetInstance().Send_ViewStallItem( ServerID );
	}
}

UBOOL AMMOPlayer::CanInteract() const
{
	return TRUE;
}

void AMMOPlayer::SetSilhouette( UBOOL show, FVector sc )
{
	Super::SetSilhouetteEx(show, sc);
}

//void AMMOPlayer::OnEnterSafeZone()
//{
//	// `log("Debug: +++++++++++++++++ Enter MMOVolume_SafeZone" @self);
//	//IsInSafeZone = TRUE;
//	//if(bHoldWeapon)
//	//{
//	//	bHoldWeapon = FALSE;
//		ReflashWeaponSocket();
//	//}
//}
//
//void AMMOPlayer::OnLeavingSafeZone()
//{
//	//IsInSafeZone = FALSE;
//	//if(!bHoldWeapon)
//	//{
//	//	bHoldWeapon = TRUE;
//		ReflashWeaponSocket();
//	//}
//}

BYTE AMMOPlayer::GetPickPriority()
{
	if(bDead)
	{
		return EPP_MAX;
	}

	if (IsMainPlayer())
	{
		return EPP_MAX;
	}
	else
	{
		return EPP_Normal;
	}

	//else if(ServerID == MMOFacade::getSingleton().GetMainPlayer()->ServerID && MoveType == EMT_InStall)
	//{
	//	return EPP_OtherPlayer;
	//}
	//
	//return PickPriority;
}

void AMMOPlayer::RefreshCombatPose()
{
	ReflashWeaponSocket();
	RefreshAnimSet();
}

void AMMOPlayer::ReflashWeaponSocket()
{
	bHoldWeapon = !ActorAttributes->byCombatPoseOn;
	HoldWeapon(!ActorAttributes->byCombatPoseOn);
}

void AMMOPlayer::PlayLevelUpEffect(FVector inLoc,FRotator inRot)
{
	if(!PlayerLevelUpEffect.IsEmpty())
	{
		UParticleSystem* ps = (UParticleSystem*)(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
			*PlayerLevelUpEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

		if(ps)
		{
			eventPlayParEffect(ps, inLoc, inRot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("PlayerLevelUpEffect config particlesystem is NULL!"));
	}

	MMOFacade::getSingleton().PlayUISound(TEXT("LevelUp"));
}

UBOOL AMMOPlayer::DrawMyHUD(class AMMOHud* H)
{
#if 0
		FVector screenPos;
		//local float healthPercent;
		//local LinearColor healthColor;
		//local Vector healthColorVec;
		FVector objectIconWorldPos;
		//local float cameraDistFromPos;
		//local float iconWidth, iconHeight;
		//local float IconDistanceScale;
		INT drawX, drawY;
		INT i, j;
		FVector drawStart, drawEnd;
		FLOAT radius;


		if(bHidden)
		{
			return true;
		}

		if(DrawDebugRange)
		{
			for(i = 0; i < 6; ++i)
			{
				radius = (i + 1) * 500;
				for(j = 1; j < 16; ++j)
				{
					drawStart.X = Location.X + cos( C_DegToRad * (j - 1) / 360 ) * radius;
					drawStart.Y = Location.X + sin( C_DegToRad * (j - 1) / 360 ) * radius;
					drawStart.Z = Location.Z;

					drawEnd.X = Location.X + cos( C_DegToRad * j / 360 ) * radius;
					drawEnd.Y = Location.X + sin( C_DegToRad * j / 360 ) * radius;
					drawEnd.Z = Location.Z;

					H->Draw3DLine( drawStart , drawEnd, FColor(255, 0, 0, 255) );
				}
			}
		}

		objectIconWorldPos = Location;//(HealthBarPositionOffset>>Rotation);//actual position
		objectIconWorldPos.Z += 120;
		screenPos = H->Canvas->Project(objectIconWorldPos);

		AMMOGame* game = Cast<AMMOGame>( GWorld->GetGameInfo() );
		if( game && game->DrawDebugActorState )
		{
			H->Canvas->SetDrawColor(255, 0, 0, 255);

			//drawY = screenPos.y - iconHeight/2;
			//drawX = screenPos.x - iconWidth/2;
			drawX = screenPos.X;
			drawY = screenPos.Y;

			H->Canvas->SetPos(drawX, drawY);
			FString str = FString::Printf( TEXT("[ %d] RoleName:[%s] UserID: [%s] %s") , 
				ServerID , *RoleName , *UserId , *Controller->GetStateName().ToString() );
			H->Canvas->DrawText( str );

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("[" $ GetPhysicsName() $ "]" $ "[" $ CollisionType $ "]" $ "[" $ Mesh.RBChannel $ "]");

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("BlockingVolume = [" $ Mesh.RBCollideWithChannels.BlockingVolume $ "] Cloth = [" $ Mesh.RBCollideWithChannels.Cloth $ "]");

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("Clothing = [" $ Mesh.RBCollideWithChannels.Clothing $ "] ClothingCollision = [" $ Mesh.RBCollideWithChannels.ClothingCollision $ "]");

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("DeadPawn = [" $ Mesh.RBCollideWithChannels.DeadPawn $ "] Default = [" $ Mesh.RBCollideWithChannels.Default $ "]");

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("EffectPhysics = [" $ Mesh.RBCollideWithChannels.EffectPhysics $ "] FluidDrain = [" $ Mesh.RBCollideWithChannels.FluidDrain $ "]");

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("FracturedMeshPart = [" $ Mesh.RBCollideWithChannels.FracturedMeshPart $ "] FluidDrain = [" $ Mesh.RBCollideWithChannels.GameplayPhysics $ "]");

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("Nothing = [" $ Mesh.RBCollideWithChannels.Nothing $ "] Pawn = [" $ Mesh.RBCollideWithChannels.Pawn $ "]");

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("Untitled1 = [" $ Mesh.RBCollideWithChannels.Untitled1 $ "] Untitled2 = [" $ Mesh.RBCollideWithChannels.Untitled2 $ "]");

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("Untitled3 = [" $ Mesh.RBCollideWithChannels.Untitled3 $ "] Untitled4 = [" $ Mesh.RBCollideWithChannels.Untitled4 $ "]");

			//drawY -= 20;
			//H.Canvas.SetPos(drawX, drawY);
			//H.Canvas.DrawText("Vehicle = [" $ Mesh.RBCollideWithChannels.Vehicle $ "] Water = [" $ Mesh.RBCollideWithChannels.Water $ "]");

		}

#endif
	return TRUE;
}

void AMMOPlayer::DrawHUDPostGFx( AHUD* H )
{
#if M_DEBUG_HUD_NAME
	FVector screenPos;
	FVector objectIconWorldPos;


	objectIconWorldPos = Location;//(HealthBarPositionOffset>>Rotation);//actual position
	objectIconWorldPos.Z += 120;
	screenPos = H->Canvas->Project(objectIconWorldPos);

	H->Canvas->SetDrawColor(255, 0, 0, 255);
	//H->Canvas->SetPos(drawX, drawY);
	FVector2D drawPos(screenPos.X, screenPos.Y);
	DrawBox2D(H->Canvas->Canvas, drawPos, drawPos + FVector2D(100, 20), FLinearColor(1, 0, 0));

#endif

	if(g_tickOrder == ETO_AfterDrawHUDPostGfxTick)
	{
		if(IsMainPlayer())
		{
			_OnMMOTick(GDeltaTime);
			logTickOrderf(TEXT("ETO_AfterDrawHUDPostGfxTick"));
		}
	}
}

void AMMOPlayer::PlayAchievementEffect(FVector inLoc,FRotator inRot)
{
	if (!PlayerAchievementEffect.IsEmpty())
	{
		UParticleSystem* ps = (UParticleSystem*)(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
			*PlayerAchievementEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

		if (ps)
		{
			eventPlayParEffect(ps, inLoc, inRot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("PlayerAchievementEffect config particlesystem is NULL!"));
	}
}

void AMMOPlayer::PlaySabacStatueEffect(FVector inLoc,FRotator inRot)
{
	if (!SabacStatueEffect.IsEmpty())
	{
		UParticleSystem* ps = (UParticleSystem*)(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
			*SabacStatueEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

		if (ps)
		{
			eventPlayParEffect(ps, inLoc, inRot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("SabacStatueEffect config particlesystem is NULL!"));
	}
}

//ability obtained effect------------------------------------------------------------------------
void AMMOPlayer::PlaySkillLearnedEffect(FVector inLoc,FRotator inRot)
{
	if(!PlayerSkillLearnedEffect.IsEmpty())
	{
		UParticleSystem* ps = (UParticleSystem*)(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
			*PlayerSkillLearnedEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

		if(ps)
		{
			eventPlayParEffect(ps, inLoc, inRot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("PlayerSkillLearnedEffect config particlesystem is NULL!"));
	}
}
//ability upgrade effect------------------------------------------------------------------------
void AMMOPlayer::PlaySkillLevelUpEffect(FVector inLoc,FRotator inRot)
{
	if(!PlayerSkillLevelUpEffect.IsEmpty())
	{
		UParticleSystem* ps = (UParticleSystem*)(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
			*PlayerSkillLevelUpEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

		if(ps)
		{
			eventPlayParEffect(ps, inLoc, inRot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("PlayerSkillLevelUpEffect config particlesystem is NULL!"));
	}

	MMOFacade::getSingleton().PlayUISound(TEXT("SkillLevelUp"));
}

//------------------------------------------------------------------------
void AMMOPlayer::PlayTeleportLeaveEffect(FVector inLoc,FRotator inRot)
{
	if(!PlayerTeleportLeaveEffect.IsEmpty())
	{
		UParticleSystem* ps = (UParticleSystem*)(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
			*PlayerTeleportLeaveEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

		if(ps)
		{
			eventPlayParEffect(ps, inLoc, inRot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("PlayerTeleportLeaveEffect config particlesystem is NULL!"));
	}
}
//------------------------------------------------------------------------
void AMMOPlayer::PlayTeleportEnterEffect(FVector inLoc,FRotator inRot)
{
	if(!PlayerTeleportEnterEffect.IsEmpty())
	{
		UParticleSystem* ps = (UParticleSystem*)(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
			*PlayerTeleportEnterEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

		if(ps)
		{
			eventPlayParEffect(ps, inLoc, inRot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("PlayerTeleportEnterEffect config particlesystem is NULL!"));
	}
}
//------------------------------------------------------------------------
void AMMOPlayer::PlayReviveEffect(FVector inLoc,FRotator inRot)
{
	if(!PlayerReviveEffect.IsEmpty())
	{
		UParticleSystem* ps = (UParticleSystem*)(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
			*PlayerReviveEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

		if(ps)
		{
			eventPlayParEffect(ps, inLoc, inRot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("PlayerReviveEffect config particlesystem is NULL!"));
	}
}
//------------------------------------------------------------------------
void AMMOPlayer::PlayGainExtraExpEffect(FVector inLoc,FRotator inRot)
{
	if(!PlayerGainExtraExpEffect.IsEmpty())
	{
		UParticleSystem* ps = (UParticleSystem*)(StaticLoadObject(UParticleSystem::StaticClass(), NULL,
			*PlayerGainExtraExpEffect, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));

		if(ps)
		{
			eventPlayParEffect(ps, inLoc, inRot);
		}
	}
	else
	{
		warnf(NAME_Error, TEXT("PlayGainExtraExpEffect config particlesystem is NULL!"));
	}
}
//------------------------------------------------------------------------
INT AMMOPlayer::GetMoney(BYTE MoneyType)
{
	return MMOFacade::getSingleton().GetPlayerMoney((EMONEY_TYPE)MoneyType);
}

//After re-login, reset data
void AMMOPlayer::ResetData()
{
	//don't lie dead after re-logging in
	bDead = FALSE;
	CloseStall();
}

void AMMOPlayer::OpenStall()
{
	// #24739
	if (IsMainPlayer())
	{
		if (MainController)
		{
			MainController->StopMove(TRUE);
		}
	}
	else
	{
		StopMovement();
	}

	// #20859
	RefreshVisible();
}

void AMMOPlayer::CloseStall()
{
	// return to normal state
	MoveType = EMT_Run;
	StallName = FString("");

	// #20859
	RefreshVisible();
}

void AMMOPlayer::ProcessDefaultDie(BYTE DieCause)
{
	AMMOActor::ProcessDefaultDie(DieCause);
	//if (IsMainPlayer())
	//{
	//	warnf(NAME_Warning, TEXT("ProcessDefaultDie1========================================================") );
	//	if (WorldInfo)
	//	{
	//		warnf(NAME_Warning, TEXT("ProcessDefaultDie2") );
	//		AMMOGame* Game = Cast<AMMOGame>(WorldInfo->Game);
	//		if (Game)
	//		{
	//			warnf(NAME_Warning, TEXT("ProcessDefaultDie3") );
	//			Game->EnableDeathEffects(TRUE);
	//		}
	//	}
	//}
}

//=======================================================================
void AMMOPlayer::SetInFighting( UBOOL isNowFighting )
{
	IsFighting = isNowFighting;
	//
#if !FINAL_RELEASE
	//debugf(NAME_Log, TEXT("%s Combat state Changed to %d"), *RoleName, isNowFighting );
#endif
	//
	if( IsMainPlayer() ){
		MMOGameEvent_OneInt evt( GameEvent_SelfCompatStateChange, IsFighting );
		MMOUI::PostGameEvent(&evt);
	}
}

UBOOL AMMOPlayer::IsInFighting()
{
	return IsFighting;
}
//=======================================================================
void AMMOPlayer::SetPrisonTerm( INT sec )
{
	INT originalTime = PrisonTerm;
	PrisonTerm = sec;

	//
	MMOGameEvent_OneInt evt( GameEvent_PrisonTermChanged, ServerID );
	MMOUI::PostGameEvent(&evt);
}

INT AMMOPlayer::GetPrisonTerm()
{
	return PrisonTerm;
}

UBOOL AMMOPlayer::IsInPrison()
{
	return ( PrisonTerm != 0 );
}
//=======================================================================
//Bruce@snda
void AMMOPlayer::RefreshCollision()
{
	float PlayerGlobalScale = GWorld->GetWorldInfo()->PlayerGlobalScale;
	UAvatarData* CurrentAV = UseingAvatar;
	if ( MeshChangeAvatarData )
	{
		CurrentAV = MeshChangeAvatarData;
	}

	if ( CurrentAV )
	{
		SetDrawScale( CurrentAV->MeshScale * CurrentAV->AvatarDrawScale * PlayerGlobalScale );
		SetCollisionSize( CurrentAV->CollisionRadius * CurrentAV->AvatarDrawScale * PlayerGlobalScale, 
			CurrentAV->CollisionHeight * CurrentAV->AvatarDrawScale * PlayerGlobalScale );
		UpdateLocationZ();
	}
}
//Bruce@snda


void AMMOPlayer::SetInTrading( UBOOL isNowTrading )
{
	IsTrading = isNowTrading;
}

UBOOL AMMOPlayer::IsInTrading()
{
	return IsTrading;
}

//=================================================================
void AMMOPlayer::SetVip(INT curVipType)
{
	VipType = curVipType;
}
//=================================================================
INT AMMOPlayer::GetVipType()
{
	return VipType;
}
//===========================================================
void AMMOPlayer::PlayRandomAnim()
{

	if (bDead)
	{
		return;
	}

	if (IsPlayingAnyAnimation())
	{
		return;
	}

	if(MMO::GetCurrentGameState() != EGameState_InGame){
		return;
	}

	if (isMoving)
	{
		return;
	}

	TArray<FName>* SeqList = &(PawnAnimInfo.FightIdleRestAnims);
	if (SeqList==NULL || SeqList->Num() == 0)
	{
		return;
	}

	INT idx = RandHelper(SeqList->Num());
	if ((*SeqList)(idx) == NAME_None)
	{
		return;
	}

	PlayAnimation((*SeqList)(idx));
}

void AMMOPlayer::TickSpecial(FLOAT Delta)
{
	AMMOActor::TickSpecial(Delta);

	LeftTimeToCheckAboutRestAnim -= Delta;
	if (LeftTimeToCheckAboutRestAnim <= 0)
	{
		LeftTimeToCheckAboutRestAnim = TimeToCheckAboutRestAnim;
		PlayRandomAnim();
	}

	if (IsMainPlayer())
	{
		if (CurrentAbilityState == EAS_Spell || CurrentAbilityState == EAS_Cast || CurrentAbilityState == EAS_Requested)
		{
			AbilityStateCD += Delta;
			if (AbilityStateCD > 5.0f)
			{
				AbilityStateCD = 0.0f;
				CurrentAbilityState = EAS_End;
			}
		}
	}
}

INT AMMOPlayer::GetRealMoney()
{
	return RealMoney;
}
void AMMOPlayer::SetRealMoney(INT money)
{
	RealMoney = money;
}

void AMMOPlayer::PlayOperationMaterialAni()
{
	if (NextOperationMaterialState == eOperationAnimationStateInvalid)
	{
		return ;
	}

	FString animateName;
	FString endAnimateName;
	if ( NextOperationMaterialState == eOperationAnimationStateStart)
	{
		GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationEnd"),endAnimateName,GDefaultGameIni);
		if (endAnimateName != TEXT("") && eventIsPlayingAnimationByName(FName(*endAnimateName)) == TRUE)
		{
			return;
		}
		GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationStart"),animateName,GDefaultGameIni);
		NextOperationMaterialState = eOperationAnimationStateLoop;
	}
	else if (NextOperationMaterialState == eOperationAnimationStateLoop)
	{
		GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationLoop"),animateName,GDefaultGameIni);
		NextOperationMaterialState = eOperationAnimationStateEnd;
	}
	else if (NextOperationMaterialState == eOperationAnimationStateEnd)
	{
		GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationEnd"),animateName,GDefaultGameIni);
		NextOperationMaterialState = eOperationAnimationStateInvalid;
	}
	else
	{
		return;
	}
	
	if (animateName != TEXT("") && eventIsPlayingAnimationByName(FName(*animateName)) == FALSE)
	{
		PlayAnimation(FName(*animateName));
	}
}

void AMMOPlayer::BeginOperateMaterial()
{
	FString startAnimateName;
	FString loopAnimateName;
	FString endAnimateName;
	FName   currentAniName;
	GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationStart"),startAnimateName,GDefaultGameIni);
	GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationLoop"),loopAnimateName,GDefaultGameIni);
	GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationEnd"),endAnimateName,GDefaultGameIni);
	GetCurrentAnimName(currentAniName);
	if (currentAniName.IsValid() == FALSE || ( currentAniName != *startAnimateName && currentAniName != *loopAnimateName && currentAniName != *endAnimateName) )
	{
		NextOperationMaterialState = eOperationAnimationStateInvalid;
	}	

	if (NextOperationMaterialState == eOperationAnimationStateInvalid )
	{
		NextOperationMaterialState = eOperationAnimationStateStart;
		PlayOperationMaterialAni();
	}
	else
	{
		NextOperationMaterialState = eOperationAnimationStateLoop;
	}	
}

void AMMOPlayer::OnAnimEndNative(class UAnimNodeSequence* SeqNode,FLOAT PlayedTime,FLOAT ExcessTime)
{
	Super::OnAnimEndNative(SeqNode,PlayedTime,ExcessTime);
	FString startAnimateName;
	FString loopAnimateName;
	FString endAnimateName;
	GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationStart"),startAnimateName,GDefaultGameIni);
	GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationLoop"),loopAnimateName,GDefaultGameIni);
	GConfig->GetString(TEXT("MineMaterial"),TEXT("OperationAnimationEnd"),endAnimateName,GDefaultGameIni);
	if (SeqNode->AnimSeqName == *startAnimateName || SeqNode->AnimSeqName == *loopAnimateName || SeqNode->AnimSeqName == *endAnimateName)
	{
		PlayOperationMaterialAni();
	}	
	else
	{
		NextOperationMaterialState = eOperationAnimationStateInvalid;
	}
}