/*
native functions of the MMOPlayerController class

*/

//#pragma once
#include "stdafx.h"
#include "MMOFacade.h"
#include "EngineInterpolationClasses.h"
#include "UnTerrain.h"
#include "net/message_handler.h"
#include "error_code_name.h"
#include "scene_def.h"
#include "ScaleformEngine.h"
#include "FSMIO.h"
#include "CommonGameEvents.h"
#include "MMOQuestManager.h"
#include "MMONavigationManager.h"
#include "MMODropManager.h"
#include "MMOWeatherManager.h"
#include "EquipDecomposeManager.h"
#include "DoodadManager.h"
#include "MMOSkillManager.h"
#include "MMOFreeTipsUIManager.h"
#include "MMOGuideManager.h"
#include "EngineAudioDeviceClasses.h"
#include "MMOBuffManager.h"
#include "FSMCtrl.h"
#include "LoginBase.h"
#include "AbilityProcessor.h"
#include "Mir2Attack.h"
#include "MMOChatManager.h"
#include "MMOMapDataManager.h"
#include "MMOMsgBoxManager.h"

extern UMMOGFXPlayer_RootBase* g_rootGFXPlayer;
INT g_Efficiency=0;
UBOOL _bReturnLogin = FALSE;
#if GRID_CTRL_MODE
	INT g_moveModGridSize = 128;
#else
	INT g_moveModGridSize = 1;
#endif

INT g_netLagToleranceDisatnce = 3; // 网络延迟的距离容差范围，多少个格子

FLOAT g_inMoveRateScale_1 = 0.85;
FLOAT g_inMoveRateScale_2 = 1.0;
FLOAT g_inMoveRateScale_3 = 1.2;
FLOAT g_inMoveRateScale_4 = 1.25;


FLOAT g_Run_AniRate[EPCT_MAX]			= {1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f};
FLOAT g_Walk_AniRate[EPCT_MAX]			= {1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f};
FLOAT g_Run_Oblique_AniRate[EPCT_MAX]	= {1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f};
FLOAT g_Walk_Oblique_AniRate[EPCT_MAX]	= {1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f,1.00f};

FLOAT g_Run_Phase[EPCT_MAX]				= {0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f};
FLOAT g_Walk_Phase[EPCT_MAX]			= {0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f};
FLOAT g_Run_Oblique_Phase[EPCT_MAX]		= {0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f};
FLOAT g_Walk_Oblique_Phase[EPCT_MAX]	= {0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f};
UBOOL g_ResetPhaseFlag					= FALSE;
UBOOL g_UniformSpeed					= FALSE;
INT   g_SerialID = 0;

#if !SHIPPING_PC_GAME
extern bool g_drawPathDebug;
#endif
extern UBOOL g_bGMBuff;
extern INT32 g_SendClearAbilityCacheTimes;
extern INT32 g_RecieveClearAbilityCacheTimes;
extern FLOAT g_UpdateClearAbilityCacheTime;
extern UBOOL g_PreMovetoPoint;
extern INT g_pickZOffset;
extern INT g_TotalFrameNum;
void AMMOPlayerController::ShowDebugPath()
{
#if !SHIPPING_PC_GAME
	g_drawPathDebug = !g_drawPathDebug;
#endif
}

#define SERVER_NAV_ALG
//#define GRID_CTRL_MODE

extern bool g_disableAllInput;
extern bool bUseGMCmd;

FString AMMOPlayerController::ConsoleCommand(const FString& Cmd,UBOOL bWriteToLog)
{
#if (!SHIPPING_PC_GAME)
	if(!bUseGMCmd)
	{
		return TEXT("");
	}
#endif

	return Super::ConsoleCommand(Cmd,bWriteToLog);
}

void AMMOPlayerController::DebugIdleBegin()
{
	//DebugBreak();
}

void AMMOPlayerController::SetSystemSettings(INT WorldLevel,INT TextureLevel,INT ShadowLevel,INT ResX,INT ResY,UBOOL fullscreen)
{
	// TODO: 暂时只设置分辨率
	
	ConsoleCommand(FString::Printf(TEXT("setres %dx%d %s"), ResX, ResY,
		fullscreen ? TEXT("F") : TEXT("W")));

	//ConsoleCommand(TEXT("SCALE HIGHEND"));

	//GSystemSettings.SetBucket( TextureLevel, WorldLevel, ShadowLevel );

	/*FSystemSettings Settings = GSystemSettings;

	Settings.bFullscreen = fullscreen;
	Settings.ResX = ResX;
	Settings.ResY = ResY;
	//GSystemSettings.SetResolution( ResX, ResY, fullscreen );
	GSystemSettings.ApplyNewSettings(Settings, TRUE );*/
	//GSystemSettings.ApplyFriendlySettings(TextureLevel+1, WorldLevel+1,ShadowLevel+1,ResX,ResY,fullscreen);
}
//------------------------------------------------------------------------
void AMMOPlayerController::GetSystemSettings(INT& WorldLevel,INT& TextureLevel,INT& ShadowLevel,INT& ResX,INT& ResY,INT&bFullScreen)
{
	FSystemSettings& Settings = GSystemSettings;

	//WorldLevel = Settings.WorldLevel;
	//TextureLevel = Settings.TextureLevel;
	//ShadowLevel = Settings.ShadowLevel;
	ResX = INT(Settings.ResX);
	ResY = INT(Settings.ResY);
	bFullScreen = INT(Settings.bFullscreen);
}
//----------------------
void AMMOPlayerController::SetTimeFactor(INT state, FLOAT value, FLOAT time)
{
	//便于恢复速度
	//if (GTimeFactorState == 0)
	{
		GTimeFactorState = state;
		GTimeFactorValue = value;
		GTimeFactorStateLastTime = time;
	}
}
//------------------------------------------------------------------------
void AMMOPlayerController::RegisterToNativeManager()
{
// 	if (GIsEditor)
// 	{
// 		return;
// 	}
// 	if(Facade::getSingletonPtr() == 0)
// 	{
// 		new Facade();
// 		MMOFacade::getSingleton().Init();	
// 	}

	MMOFacade::getSingleton().SetCurrentPlayerController(this);
}

void AMMOPlayerController::LoginGame(const FString& IP,INT Port, const FString& UserName,const FString& Password)
{	
    UBOOL connectToBS = FALSE;
    
	TCHAR ipFile[1024];
	appSprintf(ipFile, TEXT("%sIP.ini"), *appGameConfigDir());
	GConfig->GetBool(TEXT("LoginIP"), TEXT("ConnectBS"), connectToBS, ipFile);
	CMessageHandler::GetInstance().Login(*IP, Port, *UserName,*Password, connectToBS);
}

//整个程序周期只存在一个AMMOPlayerController，角色重新登录后，所有需要重置的数据放在这里进行
void AMMOPlayerController::ResetData( )
{
	//PCtrl中数据应是不需要其他玩家知晓，相对操作者的独立数据 
	RoleList.Empty();
	DeletingRoleList.Empty();
	AvatarInfoList.Empty();
	AMMOPlayer* player = Cast<AMMOPlayer>( Pawn );
	if( player )
		player->ResetData();
}

//------------------------------------------------------------------------
void AMMOPlayerController::SendRequest_SelectPlayer(INT nPlayerID)
{
	CMessageHandler::GetInstance().Send_CharactorSelect(nPlayerID);
}

void AMMOPlayerController::SendRequestSetFocus(INT InTargetId)
{
	CMessageHandler::GetInstance().Send_SelectFocus(InTargetId);
}


void AMMOPlayerController::OnHUDLoaded()
{
	if ( !GIsEditor )   // Bruce@snda
		MMOFacade::getSingleton().NotifyHUDLoaded();
}

#if M_ENABLE_NET_MSG_BUFF
extern FLOAT g_maxProcTime;
#endif

void AMMOPlayerController::DrawHUD( AHUD* H )
{
//	AActor* a;

	if(Pawn == NULL)
		return;

	//cancel our viewport's frame of blackness once we have a HUD
	//MMOGame(WorldInfo.Game).GetViewportClient().SetBlackOverlay(false);

	//Super::DrawHUD(H);


	// TODO: draw hud

	//get all of the Actors that implement the Drawable HUD interface, and tell them to draw their HUD for us
// 	foreach DynamicActors(class'Actor',a,class'IMMOActorHUD')
// 	{
// 		//if they only want the HUD drawn for their owner, see if us or our Pawn is the owner
// 		if(!IMMOActorHUD(a).OnlyDrawHUDForOwner() || self == a.Owner || (Pawn != none && Pawn == a.Owner))
// 		{	IMMOActorHUD(a).DrawMyHUD(MMOHud(H));
// 
// 
// 		}
// 	}

	if(bDebug)
	{
		H->Canvas->SetPos(300,300);
		H->Canvas->SetDrawColor(255,255,255,255);
		H->Canvas->DrawText(appItoa((INT)WorldInfo->TimeSeconds));
	}
}

extern FLOAT GAverageFPS;
void AMMOPlayerController::DrawHUDPostGFx( AHUD* H )
{
	/*local Actor a;
	foreach DynamicActors(class'Actor',a,class'IMMOActorHUD')
	{
		IMMOActorHUD(a).DrawHUDPostGFx(H);
	}
*/
#if 0
	TObjectIterator<UObject> It;
	for (; It; ++It)
	{
		UObject* obj = *It;
		if(obj)
		{
			TScriptInterface<IIMMOActorHUD> hudInterface = ScriptInterfaceCast<IIMMOActorHUD>(obj);
			if(hudInterface)
			{
				hudInterface->DrawHUDPostGFx(H);
			}
		}
	}
#endif
		

	if( bShowPlayerCount )
	{
		H->Canvas->SetDrawColor( 255, 0, 0, 255 );
		H->Canvas->SetPos( 100, 50 );

		INT maxPlayerCnt =  GSystemSettings.GetMaxShowPlayerCount();
		if(GetPlayer() && GetPlayer()->IsInSafeZone)
		{
			maxPlayerCnt =  GSystemSettings.MaxShowPlayerCount;
		}

		FString str = FString::Printf( TEXT("Player Count: %d/%d  NPC:%d FPSLv:%d AvgFPS:%.2f"), 
			MMOFacade::getSingleton().GetPlayerCount(), maxPlayerCnt,
			MMOFacade::getSingleton().GetNPCCount(), GSystemSettings.AutoFPSLevel, GAverageFPS);

		H->Canvas->DrawText( str );

		H->Canvas->SetDrawColor( 0, 255, 0, 255 );
		H->Canvas->SetPos( 100, 65 );

#if M_ENABLE_NET_MSG_BUFF
		if(!CMessageHandler::GetInstance()._fullSpeedProcNetMsg)
		{			
			H->Canvas->DrawText( FString::Printf(TEXT("Network Msg: Smooth - %.1f FPS"), 1.0f / g_maxProcTime));
		}
		else
		{
			H->Canvas->DrawText( TEXT("Network Msg: Full Speed") );
		}
#endif

		H->Canvas->SetPos( 100, 80 );
		if(GSystemSettings.bUseVSync)
		{
			H->Canvas->DrawText( TEXT("Vsync On") );
		}
		else
		{
			H->Canvas->DrawText( TEXT("Vsync Off") );
		}

		if(PlayerCamera && Cast<AMMOPlayerCamera>(PlayerCamera))
		{
			AMMOPlayerCamera* cam = Cast<AMMOPlayerCamera>(PlayerCamera);
			H->Canvas->SetPos( 100, 95 );
			if(cam->EnableSmoothCameraPos)
			{
				H->Canvas->DrawText( FString::Printf(TEXT("Cam Move: Smooth %.1f"), cam->DefaultMoveSpeed) );
			}
			else
			{
				H->Canvas->DrawText(  FString::Printf(TEXT("Cam Move: Hard Bind %.1f"), cam->DefaultMoveSpeed) );
			}
		}
	}
}

//------------------------------------------------------------------------
void AMMOPlayerController::SwitchUIPlayerByMapName(const FString& MapName)
{
	FString LocalMap = FFilename(GConfig->GetStr( TEXT("URL"), TEXT("LocalMap"),	GEngineIni )).GetBaseFilename();
	if( !myHUD || GIsEditor )
		return;

	// OSCARLOGIN:
	UMMOUIGFXPlayer*  HudMovie = Cast<AMMOHud>(myHUD)->HudMovie;

	LocalMap = LocalMap.ToLower();

	//if(  appStrcmp(*MapName,*LocalMap) == 0  ) //登录场景
	//{
	//	MMOFacade::getSingleton().NotifyEnterLogin();
	//}
	//else
	//{
	//	MMOFacade::getSingleton().NotifyEnterGame();
	//}
/*
#if 0 // OSCARLOGIN:
	if(  appStrcmp(*MapName,*LocalMap) == 0  ) //登录场景
	{
		MMOFacade::getSingleton().NotifyEnterLogin();
		if( SplashMovie &&  HudMovie)
		{ 
			//Login界面关闭其他无关界面 取消Focus
			MMOUI::CloseAllPanels( FALSE );
			HudMovie->eventSetUIPlayerVisible(false);
			HudMovie->Close(false);

			SplashMovie->Start();
			SplashMovie->Advance(0);
			SplashMovie->eventSetUIPlayerVisible(true);
		}
	}
	else
	{
		MMOFacade::getSingleton().NotifyEnterScene();
		if( SplashMovie &&  HudMovie)
		{
			SplashMovie->eventSetUIPlayerVisible(false);
			SplashMovie->Close(false);

			//GameHud会被移除 但也不至于把逻辑反过来写一遍吧
			HudMovie->Start();
			HudMovie->Advance(0);
			HudMovie->eventSetUIPlayerVisible(true);
			MMOUI::StartAllPanels( );
		}
	}
#endif*/

	//GConfig->GetString(TEXT("Configuration"), TEXT("GlobalMapInfoPath"),
	//globalMapInfoDir, GGameIni);
}
//------------------------------------------------------------------------
AMMOPlayer* AMMOPlayerController::GetPlayer()
{
	AMMOPlayer* player = Cast<AMMOPlayer>( Pawn);
	//check(player != 0);
	return player;
}
//------------------------------------------------------------------------
bool g_moveImm = true;
void AMMOPlayerController::SendRequest_MoveTo(FVector Position,UBOOL bRunOrWalk)
{	 
	if(GetPlayer() && !GIsEditor)
	{
			WaitingForTheMoveToRespond = 0.3f;
			CMessageHandler::GetInstance().Send_MoveTo_Mir2(
				GetPlayer()->ServerID, Pawn->Location, Position, bRunOrWalk);
		
			if(g_moveImm && g_PreMovetoPoint==TRUE)
			{
				FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
				AMMOPlayer* pSelfPlayer = GetPlayer();
				if(g_SendClearAbilityCacheTimes==g_RecieveClearAbilityCacheTimes && pSelfPlayer)
				{
					FVector OldPos = Pawn->Location;	
					FVector DisDir = Position-Pawn->Location;
					DisDir.Z = 0.0;
					FRotator NewRotation = DisDir.Rotation();
					pSelfPlayer->RotatoTo(NewRotation, 0);

					FLOAT L = DisDir.Size2D();
					FLOAT P = abs(DisDir.X)+abs(DisDir.Y);
					UBOOL bAxial = (P/L>1.2);	//轴向

					if(bRunOrWalk)
					{
						INT MoveFrames = GetPlayer()->ActorAttributes->GetAttribute(catRunSpeed);
						if(MoveFrames==0)MoveFrames=12;
						FLOAT MoveTimes = MoveFrames>0?ConvertToClientTime(MoveFrames):0.75;
						FLOAT Speed		= bAxial ? (256.0f/MoveTimes) : (256.0f*1.414213562373095f/MoveTimes);
						FLOAT inMoveRateScale = 0.75/MoveTimes;

						pSelfPlayer->SetMoveType(bAxial?EMT_Run:EMT_Run_Oblique);					

						//动画相位的智能复位，站立一段时候后，再次启动的时候，相位从初始值开始
						if(pSelfPlayer->isMoving==FALSE && pSelfPlayer->ResetMovePhaseTime<0.0f)
						{
							pSelfPlayer->SetMoveAnimPosition(bAxial?g_Run_Phase[pSelfPlayer->GetClassId()]:g_Run_Oblique_Phase[pSelfPlayer->GetClassId()]);	
						}
						pSelfPlayer->ResetMovePhaseTime = MoveTimes+0.5f;

						pSelfPlayer->eventScriptMoveTo(Position, Speed,inMoveRateScale);
					}
					else
					{
						INT MoveFrames = GetPlayer()->ActorAttributes->GetAttribute(catWalkSpeed);
						if(MoveFrames==0)MoveFrames=8;
						FLOAT MoveTimes = MoveFrames>0?ConvertToClientTime(MoveFrames):0.5;
						FLOAT Speed		= bAxial ? (128.0f/MoveTimes) : (128.0f*1.414213562373095f/MoveTimes);
						FLOAT inMoveRateScale = 0.5/MoveTimes;

						pSelfPlayer->SetMoveType(bAxial?EMT_Walk:EMT_Walk_Oblique);

						//动画相位的智能复位，站立一段时候后，再次启动的时候，相位从初始值开始
						if(pSelfPlayer->isMoving==FALSE && pSelfPlayer->ResetMovePhaseTime<0.0f)
						{
							pSelfPlayer->SetMoveAnimPosition(bAxial?g_Walk_Phase[pSelfPlayer->GetClassId()]:g_Walk_Oblique_Phase[pSelfPlayer->GetClassId()]);	
						}
						pSelfPlayer->ResetMovePhaseTime = MoveTimes+0.5f;

						pSelfPlayer->eventScriptMoveTo(Position, Speed,inMoveRateScale);
					}
				}
			}

			//		FVector dir = (Position - Pawn->Location).SafeNormal2D();
			//		dir.Z = 0;
			//		FRotator rot = dir.Rotation();
			//		GetPlayer()->RotatoTo(rot, 0);

			//		if(bRunOrWalk)
			//		{
			//			FVector dis = Position-Pawn->Location;
			//			FLOAT L = dis.Size2D();
			//			FLOAT P = abs(dis.X)+abs(dis.Y); //GRID_WIDTH / POINT_PER_CM * 2;
			//			INT RunFrames = GetPlayer()->ActorAttributes->GetAttribute(catRunSpeed);
			//			if(RunFrames==0)RunFrames=12;
			//			FLOAT RunTimes = ConvertToClientTime(RunFrames);
			//			FLOAT speedConfig = 256.0f/RunTimes;

			//			FLOAT speed = speedConfig;
			//			FLOAT inMoveRateScale = g_Run_Oblique_AniRate[GetPlayer()->GetClassId()];

			//			if(P/L>1.2)
			//			{	
			//				//speed*=0.7071f;
			//				speed = speedConfig;
			//				inMoveRateScale= g_Run_AniRate[GetPlayer()->GetClassId()]* 0.75f/RunTimes;
			//				//inMoveRateScale = g_Run_Oblique_AniRate[GetPlayer()->GetClassId()]*0.8;
			//				if(g_UniformSpeed==FALSE){
			//					GetPlayer()->SetMoveType(EMT_Run);
			//				}
			//				else
			//				{
			//					GetPlayer()->SetMoveType(EMT_Fly);
			//				}
			//			}
			//			else
			//			{
			//				speed = speedConfig*1.414213562373095f;
			//				inMoveRateScale = g_Run_Oblique_AniRate[GetPlayer()->GetClassId()]* 0.75f/RunTimes;
			//				if(g_UniformSpeed==FALSE){
			//					GetPlayer()->SetMoveType(EMT_Run_Oblique);
			//				}
			//				else
			//				{
			//					GetPlayer()->SetMoveType(EMT_Fly_Oblique);
			//				}
			//			}
			//			GetPlayer()->ResetMovePhaseTime = 0.75+0.5f;
			//			GetPlayer()->eventScriptMoveTo(Position, speed, inMoveRateScale);
			//		}
			//		else
			//		{
			//			FVector dis = Position-Pawn->Location;
			//			FLOAT L = dis.Size2D();
			//			FLOAT P = abs(dis.X)+abs(dis.Y); //GRID_WIDTH / POINT_PER_CM;
			//			INT WalkFrames = GetPlayer()->ActorAttributes->GetAttribute(catWalkSpeed);
			//			if(WalkFrames==0)WalkFrames=12;
			//			FLOAT WalkTimes = ConvertToClientTime(WalkFrames);
			//			FLOAT speedConfig = 128.0f/WalkTimes;

			//			FLOAT speed = speedConfig;
			//			FLOAT inMoveRateScale = g_Walk_Oblique_AniRate[GetPlayer()->GetClassId()]* 0.5f/WalkTimes;	
			//			if(P/L>1.2){
			//				speed = speedConfig;
			//				inMoveRateScale= g_Walk_AniRate[GetPlayer()->GetClassId()] * 0.5f/WalkTimes;	
			//				if(g_UniformSpeed==FALSE){
			//					GetPlayer()->SetMoveType(EMT_Walk);
			//				}
			//				else
			//				{
			//					GetPlayer()->SetMoveType(EMT_Walk);
			//				}
			//			}
			//			else{
			//				speed = speedConfig*1.414213562373095f;
			//				GetPlayer()->SetMoveType(EMT_Walk_Oblique);
			//			}
			//			GetPlayer()->ResetMovePhaseTime = 0.75+0.5f;
			//			GetPlayer()->eventScriptMoveTo(Position, speed, inMoveRateScale);

			//		}
			//	}
			//}
			////Pawn->DrawDebugPoint(Position, 3, FLinearColor(1.0, 0.f, 0.f), TRUE);

	}
	else
	{
		warnf(NAME_Error, TEXT("[AMMOPlayerController::SendRequest_MoveTo] Player is NULL???"));
	}
}

void AMMOPlayerController::SendRequestStop()
{
	if (!GIsEditor)
	{
		CMessageHandler::GetInstance().SendStopMove();
	}
}

void AMMOPlayerController::SendRequestAbility( INT nAbilityID )
{
	AMMOPlayer* player = Cast<AMMOPlayer>( Pawn );
	if(player)
		CMessageHandler::GetInstance().SendRequestReviveAbility(nAbilityID,player->ServerID,   player->Location);
}

void AMMOPlayerController::UpdateServerPosition()
{
	if(Pawn)
	{
		FVector NewLocation = Pawn->Location;
		if (Pawn->CylinderComponent)
		{
			NewLocation.Z -= Pawn->CylinderComponent->CollisionHeight;
		}
		CMessageHandler::GetInstance().UpdateServerPosition(NewLocation);
	}
}


//------------------------------------------------------------------------
void AMMOPlayerController::CreateRole(const FString& RoleName,INT classId,INT sex)
{
	//CMessageHandler::GetInstance().Send_CreateRole(*RoleName,classId,sex);
}


////------------------------------------------------------------------------
//void AMMOPlayerController::RequestUseAbility(INT TargetId,INT AbilityId,INT level, FVector Point, INT byRotateToTarget)
//{
//	CMessageHandler::GetInstance().Send_RequestUseAbility(TargetId, AbilityId, level, Point, byRotateToTarget);
//}



INT AMMOPlayerController::RequestUseAbilityEx(INT TargetId,INT SpellId,INT level, FVector Point, INT byRotateToTarget)
{
	//static INT SerialID=0;
	g_SerialID = (++g_SerialID) % 256;
	CMessageHandler::GetInstance().Send_RequestUseAbility_Ex(TargetId, SpellId, level, Point, byRotateToTarget, g_SerialID);
	FSMIO::getSingleton().Target.SetKeepRunning(FALSE);
	return g_SerialID;
}


void AMMOPlayerController::RequestSwitchSpell(INT SpellId)
{
	CMessageHandler::GetInstance().Send_RequestSwitchSpell(SpellId);
	FSMIO::getSingleton().CDTime.ResetCD_SwitchRespondTime(0.2f);
}


//void AMMOPlayerController::RequestUseAbility(INT InSlotId)
//{
//	CMessageHandler::GetInstance().SendRequestUseAbility(InSlotId);
//}

//------------------------------------------------------------------------------------------
void AMMOPlayerController::NativeTestAcceptQuest(INT Questid)
{
	MMOQuestManager::getSingleton().NotifyAcceptQuest(Questid);
}
//------------------------------------------------------------------------------------------
void AMMOPlayerController::NativeTestCompleteQuest(INT Questid,INT selectaward)
{
	MMOQuestManager::getSingleton().NotifyCompleteQuest(Questid);
}
//------------------------------------------------------------------------------------------
void AMMOPlayerController::NativeTestAbandonQuest(INT Questid)
{
	MMOQuestManager::getSingleton().NotifyAbandonQuest(Questid);
}
//------------------------------------------------------------------------
void AMMOPlayerController::OnQTEFinish( INT id )
{
	//CMessageHandler::GetInstance().Send_FinishQTE( id );
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
AActor* AMMOPlayerController::PickActor(FVector StartWorldPos,FVector EndWorldPos)
{
	//DrawDebugLine(StartWorldPos, EndWorldPos, 255, 0, 0, TRUE);
	//DrawDebugBox(StartWorldPos, FVector(10, 10, 10), 255, 0, 255, TRUE);
	//DrawDebugBox(EndWorldPos, FVector(10, 10, 10), 255, 255, 0, TRUE);


	FCheckResult* HitR = MMO::PickActor(StartWorldPos, EndWorldPos, /*this->Pawn*/NULL/*modified by zhaojun@snda*/);

	if(HitR)
	{
		MouseHitWorldNormal = HitR->Normal;

		APawn* PawnActor = Cast<APawn>(HitR->Actor);
		if(PawnActor)
		{	
			return HitR->Actor;
		}
	}

	return 0;
}
//--------add by JimZhang@snda.com----------------------------------------------------------------
AActor* AMMOPlayerController::PickPoint(FVector StartWorldPos,FVector EndWorldPos)
{
	//DrawDebugLine(StartWorldPos, EndWorldPos, 255, 0, 0, TRUE);
	//DrawDebugBox(StartWorldPos, FVector(10, 10, 10), 255, 0, 255, TRUE);
	//DrawDebugBox(EndWorldPos, FVector(10, 10, 10), 255, 255, 0, TRUE);


	FCheckResult* HitR = MMO::PickPoint(StartWorldPos, EndWorldPos, /*this->Pawn*/NULL/*modified by zhaojun@snda*/);

	if(HitR)
	{
		MouseHitWorldNormal = HitR->Normal;

		APawn* PawnActor = Cast<APawn>(HitR->Actor);
		if(PawnActor)
		{	
			// put the Z onto ground
			MouseHitWorldLocation = PawnActor->Location;
			MouseHitWorldLocation.Z -=
				PawnActor->CylinderComponent ? PawnActor->CylinderComponent->CollisionHeight : 0.0f;
		}
		else
		{
			MouseHitWorldLocation = HitR->Location;
		}

		return HitR->Actor;
	}
	else
	{

		MouseHitWorldLocation = EndWorldPos;
	}


	return 0;
}
//------------------------------------------------------------------------

QSORT_RETURN CDECL ComparePickedPointsActorPreferred( const FCheckResult** AA, const FCheckResult** AB )
{
	typedef TScriptInterface<class IIPickableActor> IPickableActor;
	IPickableActor A, B;

	A = ScriptInterfaceCast<IIPickableActor>((*AA)->Actor);
	B = ScriptInterfaceCast<IIPickableActor>((*AB)->Actor);

	//	if(bSortPickedActors)
	{
		if(A && B)
		{
			if(A->GetPickPriority() < B->GetPickPriority())
			{
				return -1;
			}
			else if(A->GetPickPriority() == B->GetPickPriority())
			{
				if((*AA)->Time < (*AB)->Time)
					return -1;
			}
		}
	}
	//else
	//{
	//	if((*AA)->Time < (*AB)->Time)
	//		return -1;
	//}
	return 1;
}

AActor* AMMOPlayerController::PickPointActorPreferred(FVector StartWorldPos,FVector EndWorldPos)
{
	//DrawDebugLine(StartWorldPos, EndWorldPos, 255, 0, 0, TRUE);
	//DrawDebugBox(StartWorldPos, FVector(10, 10, 10), 255, 0, 255, TRUE);
	//DrawDebugBox(EndWorldPos, FVector(10, 10, 10), 255, 255, 0, TRUE);

	// change (this->Pawn) to null for stall, we need pick the main player, zhaojun@snda
	//FCheckResult* HitR = MMO::PickPoint(StartWorldPos, EndWorldPos, NULL/*this->Pawn*/, TRUE, (QSORT_COMPARE)ComparePickedPointsActorPreferred);

	// zhaojun@snda, 2014-7-18
	FCheckResult* HitR = MMO::PickActor(StartWorldPos, EndWorldPos, NULL/*this->Pawn*/, TRUE, (QSORT_COMPARE)ComparePickedPointsActorPreferred, TRACE_Actors | TRACE_AllComponents);

	if(HitR)
	{
		if (HitR->Time < 1.0f)
			MouseHitWorldNormal = HitR->Normal;	// not only hit the aabb, but also hit the primitive

		APawn* PawnActor = Cast<APawn>(HitR->Actor);
		if(PawnActor)
		{	
			// put the Z onto ground
			MouseHitWorldLocation = PawnActor->Location;
			MouseHitWorldLocation.Z -=
				PawnActor->CylinderComponent ? PawnActor->CylinderComponent->CollisionHeight : 0.0f;
		}
		else
		{
			MouseHitWorldLocation = HitR->Location;
		}

		//const INT gridWidth = 512;

#if !SHIPPING_PC_GAME
		if(g_drawPathDebug)
		{
			FlushPersistentDebugLines();

			DrawDebugLine(StartWorldPos, EndWorldPos, 255, 255, 0, TRUE);
			if(PawnActor)
			{
				debugf(TEXT("======AMMOPlayerController::PickPointActorPreferred PawnActor:%s"), *HitR->Actor->GetFullName());

				DrawDebugSphere(MouseHitWorldLocation, 10, 255, 255, 0, TRUE);
			}
			else
			{
				debugf(TEXT("======AMMOPlayerController::PickPointActorPreferred:%s"), *HitR->Actor->GetFullName());

				DrawDebugSphere(MouseHitWorldLocation, 10, 0, 255, 0, TRUE);
			}
		}
#endif
		return HitR->Actor;
	}

	return 0;
}

UBOOL AMMOPlayerController::PickNavMesh(FVector StartWorldPos, FVector EndWorldPos)
{
	check(NavigationHandle);
	FVector vHitPos;
	FVector vHitNormal;
	//UBOOL bHitNavMesh = 
	//	NavigationHandle->LineCheck(StartWorldPos, EndTrace, FVector(0,0,0), &vHitPos, &vHitNormal);

	UBOOL bHitNavMesh = gServerGridWorld.Pick(vHitPos, StartWorldPos, EndWorldPos);

	if (bHitNavMesh)	// something hit
	{
		MouseHitNavMeshLocation = vHitPos;
	}

	return bHitNavMesh;
}

//------------------------------------------------------------------------


void AMMOPlayerController::OnLeaveScene()
{
	bNeedCommitPosition = FALSE;
	AMMOActor* mmo = Cast<AMMOActor>(Pawn);
	if (mmo)
	{
		mmo->bFreeze = FALSE;
	}

	ClearOldSceneFlags();
	FSMIO::getSingleton().Target.ClearDesireActor();

	////退出安全区检测
	//AMMOPlayer* player = Cast<AMMOPlayer>(Pawn);
	//if(player && player->IsInSafeZone)
	//{
	//	//player->IsInSafeZone = FALSE;

	//	//MMOFacade::getSingleton().GetMMOBuffManager()->DeleteBuff(player->ServerID, SERVERID_SAFEZONE);
	//}
}

void AMMOPlayerController::OnEnterScene()
{
	AMMOActor* mmo = Cast<AMMOActor>(Pawn);
	if (mmo)
	{
		//mmo->RefreshCollision();
	}
	bIsOperatingDoodad=FALSE;
	ClearOldSceneFlags();


	FExportMap &curMapInfo = MMOMapDataManager::getSingleton().GetCurMap();
	if (curMapInfo.OpenMapPkMode) // curMapInfo.MapPkMode >= E_rpm_Peace && curMapInfo.MapPkMode<E_rpm_Total) // curMapInfo.HideRoleName
	{
		INT _PkModeLevel = 8;
		GConfig->GetInt( TEXT("PkMode") , TEXT("PkModeLevel") , _PkModeLevel , GDefaultGameIni );

		AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
		if(player&&player->GetActorLevel()>=_PkModeLevel)
		{
			if (curMapInfo.MapPkMode >= E_rpm_Peace && curMapInfo.MapPkMode < E_rpm_Total)
			{
				CMessageHandler::GetInstance().Send_PkMode((UINT8)curMapInfo.MapPkMode);
			}
		}
	}
}

static FVector LastLocation;
void AMMOPlayerController::SyncPosition()
{
	//if(EGameState_InGame != MMO::GetCurrentGameState())
	//{
	//	return;
	//}

	Super::SyncPosition();
	if (bNeedCommitPosition && !GIsEditor)
	{
		LONG64 CurrentTime = MMOFacade::getSingleton().GetCurrentTime();
		LONG64 LastTime = MMOFacade::getSingleton().GetLastUpdateTime();
		LONG64 Delta = 1000/GAME_FPS;
		//for (LONG64 i = LastTime + Delta; i <= CurrentTime; i += Delta)
		//{
		//	MMOFacade::getSingleton().SetLastUpdateTime(i);
		//	UpdateServerPosition();
		//}
		if (CurrentTime - LastTime > 4000)
		{
			MMOFacade::getSingleton().SetLastUpdateTime(CurrentTime);
			UpdateServerPosition();

			FLOAT speed = (Pawn->Location - LastLocation).Size2D() / FLOAT((CurrentTime - LastTime) / 1000.f);
			//debugf(L"#### speed=%f", speed);
			LastLocation = Pawn->Location;
		}

		if (Pawn && (Cast<AMMOActor>(Pawn))->RotateToMousePointRefrence > 0)
		{
			FVector tv = GetFixedMouseHitWorldLocation();
			CMessageHandler::GetInstance().UpdateTargetPoint(tv);
			CMessageHandler::GetInstance().UpdateRotation(Pawn->Rotation);
		}
	}


	if (Pawn && (Cast<AMMOActor>(Pawn))->ShowNavmeshDistance)
	{
		FCheckResult HitNevmesh;
		FVector Start = Pawn->Location;
		FVector End = Start + FVector(0, 0, -500);
		UBOOL Ret1 = UNavigationHandle::StaticLineCheck(HitNevmesh, Start, End, FVector(0, 0, 0));
		//this->DrawDebugPoint(HitNevmesh.Location, 10, FLinearColor(1.0, 0, 0, 1.0), FALSE);

		FCheckResult HitWorld;
		GWorld->SingleLineCheck( HitWorld, Pawn, End, Start, TRACE_AllBlocking, FVector(0, 0, 0) );

		if (Ret1 == 0 && HitWorld.Actor)
		{
			FVector dir = HitNevmesh.Location - HitWorld.Location;
			FLOAT dis = dir.Size();
			FColor col;
			if (dis > 100)
			{
				col = FColor(255, 0, 0, 255);
			}
			else if (dis > 50)
			{
				col = FColor(255, 128, 0, 255);
			}
			else
			{
				col = FColor(0, 255, 0, 255);
			}
			FString text = FString::Printf(L"Dis(%f) World(%f,%f,%f) NavMesh(%f,%f,%f)", 
				dis, HitWorld.Location.X, HitWorld.Location.Y, HitWorld.Location.Z,
				HitNevmesh.Location.X, HitNevmesh.Location.Y, HitNevmesh.Location.Z
				);
			this->DrawDebugString(Pawn->Location, text, NULL, col, 0.05f);
		}
	}
}
//切换Avatar显示装备 ------------------------------------------------------------------------
void AMMOPlayerController::SetCharAvatart(int i)
{
	AMMOPlayer *pPlayer=GetPlayer();
	if( i < 0 || i >= RoleList.Num() )
		return;

	pPlayer->ClearEquipEffects();

	pPlayer->ActorAttributes->MountID = 0;
	//INT  MountID = pPlayer->ActorAttributes->MountID;
	pPlayer->eventSetMesh( RoleList(i).byCareer, RoleList(i).bySex );
	//pPlayer->ActorAttributes->MountID = MountID;

	if( i <  AvatarInfoList.Num() )
	{
		INT n = 0;
		for (; n < AvatarInfoList.Num(); ++n)
		{
			if(RoleList(i).dwRoleID == AvatarInfoList(n).RoleId)
			{
				break;
			}
		}
		if(n >= AvatarInfoList.Num())
			return;

		FAvatarInfo tAvatarInfo=AvatarInfoList(n);

		for (int j  =  0; j < NES_VISIBLE_TOTAL; ++j )
		{
			const UItem* item = MMOFacade::getSingleton().FindItemTemplate(tAvatarInfo.EquipList[j]);
			if(!item)
			{
				continue;
			}
			pPlayer->ItemManager->RequestTakeOn( -1, j, true, item );
		}
		//if( tAvatarInfo.MainHand != 0 )
		//{
		//	const UItem* pMainHand = MMOFacade::getSingleton().FindItemTemplate(tAvatarInfo.MainHand);
		//	if( pMainHand )
		//		pPlayer->ItemManager->RequestTakeOn( -1, E_NES_MAIN_HAND, true, pMainHand );
		//}
		//if( tAvatarInfo.OffHand != 0 )
		//{
		//	const UItem* pOffHand = MMOFacade::getSingleton().FindItemTemplate(tAvatarInfo.OffHand);
		//	if( pOffHand )
		//		pPlayer->ItemManager->RequestTakeOn( -1, ENES_OffHand, true, pOffHand );
		//}
	}
	pPlayer->ItemManager->UpdateOwnerData();
	pPlayer->NotifyRefreshEquipLevelByIndex(RoleList(i).WeaponEffectIndex);
}

void AMMOPlayerController::CallBack_PreLoad_ChangeScene()
{
	CMessageHandler::GetInstance().Do_g2c_changescene_enter();
}

//add by JimZhang@snda.com-------------------------------------------------
void AMMOPlayerController::RequestRevive(INT nType,INT itemID)
{
	ReviveType type = (ReviveType)nType;
	if(type==EPRT_ITEM)
	{
		CMessageHandler::GetInstance().Send_Revive( crtAtCurrentPos );
		//CMessageHandler::GetInstance().SendRequestReviveAbility(8002,GetPlayer()->ServerID,   GetPlayer()->Location);
		
	}
	else if(type==EPRT_COIN)
	{
		CMessageHandler::GetInstance().Send_Revive( crtInstant );
		//CMessageHandler::GetInstance().SendRequestReviveAbility(8001,GetPlayer()->ServerID,   GetPlayer()->Location);

	}
	else if(type==EPRT_FREE)
	{
		CMessageHandler::GetInstance().Send_Revive( crtFree );
		//CMessageHandler::GetInstance().SendRequestReviveAbility(8000,GetPlayer()->ServerID,   GetPlayer()->Location);
	}

	// to skip the next G2C_CHAR_STOP msg
	OnRevive = TRUE;

	//AMMOGame* game = Cast<AMMOGame>( GWorld->GetWorldInfo()->Game );
	//if( game )
	//	game->StartLongJump();
}

//------------------------------------------------------------------------
void AMMOPlayerController::DoMsgBoxOk( INT MsgBoxId, UBOOL agree)
{
	switch (MsgBoxId)
	{
	case 1://复活
		CMessageHandler::GetInstance().Send_Revive( 0 );
		break;
	default:
		break;
	}
}


void AMMOPlayerController::NativeTestResetDrop()
{
	MMOFacade::getSingleton().ReadTestDropTrace();
}


void AMMOPlayerController::TestStartRain()
{
	MMOWeatherManager::getSingleton().StartWeather(swtRain);
}

void AMMOPlayerController::TestStartSnow()
{
	MMOWeatherManager::getSingleton().StartWeather(swtSnow);
}

void AMMOPlayerController::TestStartStorm()
{
	MMOWeatherManager::getSingleton().StartWeather(swtStorm);
}

void AMMOPlayerController::TestStartLightning()
{
	MMOWeatherManager::getSingleton().StartWeather(swtThunder);
}

void AMMOPlayerController::TestStopRain()
{
	MMOWeatherManager::getSingleton().StopWeather(swtRain);
}

void AMMOPlayerController::TestStopSnow()
{
	MMOWeatherManager::getSingleton().StopWeather(swtSnow);
}

void AMMOPlayerController::TestStopStorm()
{
	MMOWeatherManager::getSingleton().StopWeather(swtStorm);
}

void AMMOPlayerController::TestStopLightning()
{
	MMOWeatherManager::getSingleton().StopWeather(swtThunder);
}

void AMMOPlayerController::TestEnableWeather( UBOOL bEnable )
{
	MMOWeatherManager::getSingleton().Enable( bEnable );
}

void AMMOPlayerController::SetFire()
{
	MMOWeatherManager::getSingleton().SetFire( Pawn->Location, 300 );
}

void AMMOPlayerController::RestrictDynamicShadow( UBOOL bRestrict, FLOAT dist )
{
	GSystemSettings.RestrictWholeSceneDynamicShadowParameters = bRestrict;
	GSystemSettings.RestrictWholeSceneDynamicShadowDistance = dist;
}

//------------------------------------------------------------------------
void AMMOPlayerController::DoMsgBoxCannel( INT MsgBoxId, UBOOL agree)
{
	//CMessageHandler::GetInstance().Send_FinishQTE( id );
}
//------------------------------------------------------------------------
void AMMOPlayerController::LogPickedActor( class AActor* actor)
{
#if !FINAL_RELEASE
	if(actor)
	{
		UPackage* outermost = actor->GetOutermost();
		warnf(NAME_Warning, TEXT("_________ PickActor:%s.%s"),
			outermost ? *outermost->GetName() : TEXT("NULL"), *actor->GetName());
	}
	else
	{
		warnf(NAME_Warning, TEXT("_________ PickActor: no actor picked"));
	}
#endif
}

AMMOActor* AMMOPlayerController::GetMMOActorByTempleteID(INT TemplateID)
{
	for (TObjectIterator<ANPC> it; it; ++it)
	{
		ANPC* npc = *it;
		UNPCTemplate* npcTemp = npc->Template;
		if(npcTemp && npcTemp->GetTemplateID() == TemplateID)
		{
			if((npc->Location-Pawn->Location).Size2D()<InteractDistance )
			{
				return Cast<AMMOActor>(npc);
			}
		}
	}
	return NULL;
}

AMMOActor* AMMOPlayerController::GetMMODoodadByTempleteID(INT TemplateID)
{
	DoodadActorMap doodadActorMap = DoodadManager::getSingleton().GetDoodadActorMap();
	DoodadActorMap::iterator iter = doodadActorMap.begin();
	for (; iter != doodadActorMap.end(); ++ iter)
	{
		AMMODoodadActor* doodadActor = iter->second;
		if (doodadActor && doodadActor->CanInteract() && doodadActor->DoodadTemplate && doodadActor->DoodadTemplate->ID == TemplateID)
		{
			if ((doodadActor->Location - Pawn->Location).Size2D() < doodadActor->GetMaxInteractRadius())
			{
				return doodadActor;
			}
		}
	}

	return NULL;
}

AMMOActor* AMMOPlayerController::GetClosedMMOActorByTempleteID(INT TemplateID)
{
	FLOAT MinDistance = 10000.0;
	FLOAT CurDistance = 0;
	AMMOActor* pClosedActor = NULL;

	for (TObjectIterator<ANPC> it; it; ++it)
	{
		ANPC* npc = *it;
		UNPCTemplate* npcTemp = npc->Template;
		if(npcTemp && npcTemp->GetTemplateID() == TemplateID)
		{
			CurDistance = (npc->Location-Pawn->Location).Size2D();
			if(CurDistance<MinDistance)
			{
				MinDistance = CurDistance;
				pClosedActor = npc;
			}
		}
	}

	return pClosedActor;
}

AMMOActor* AMMOPlayerController::GetClosedMMODoodadByTempleteID(INT TemplateID)
{
	FLOAT MinDistance = 10000.0;
	FLOAT CurDistance = 0;
	AMMOActor* pClosedActor = NULL;

	DoodadActorMap doodadActorMap = DoodadManager::getSingleton().GetDoodadActorMap();
	DoodadActorMap::iterator iter = doodadActorMap.begin();
	for (; iter != doodadActorMap.end(); ++ iter)
	{
		AMMODoodadActor* doodadActor = iter->second;
		if(doodadActor && doodadActor->CanInteract() && doodadActor->DoodadTemplate && doodadActor->DoodadTemplate->ID == TemplateID)
		{
			CurDistance = (doodadActor->Location - Pawn->Location).Size2D();
			if (CurDistance < MinDistance)
			{
				MinDistance = CurDistance;
				pClosedActor = doodadActor;
			}
		}
	}

	return pClosedActor;
}

FVector AMMOPlayerController::GetFixedMouseHitWorldLocation()
{
	FVector Ret = MouseHitWorldLocation;
	if (MouseProjectionDirection.Z == 0.0)
	{
		return Ret;
	}

	FLOAT Pz = Pawn->Location.Z-Pawn->GetCollisionHeight();
	Ret.X = (Pz - MouseProjectionOrigin.Z) / MouseProjectionDirection.Z * MouseProjectionDirection.X + MouseProjectionOrigin.X;
	Ret.Y = (Pz - MouseProjectionOrigin.Z) / MouseProjectionDirection.Z * MouseProjectionDirection.Y + MouseProjectionOrigin.Y;
	Ret.Z = Pz;
	return Ret;
}

//void AMMOPlayerController::AddCmdHuntToUseAbility(class UMMOAbility* InAbility,class AActor* Target,FVector Point)
//{
//	IIAbilityDamageable* enemyActor;
//	FLOAT /*MinDis, */MaxDis, CurrentDis;
//
//	if (InAbility == NULL)
//	{
//		warnf(L"AddCmdHuntToUseAbility, but the ability data is null");
//		return;
//	}
//
//	if(Target == NULL)
//	{
//		return;
//	}
//
//	AMMOActor* actor = Cast<AMMOActor>(Pawn);
//	if (actor == NULL)
//	{
//		return;
//	}
//
//	if (actor->CanUseAnyAbility() == FALSE || actor->CanMove() == FALSE)
//	{
//		debugf(L"can not use ability or can not move ,return!");
//		return;
//	}
//
//	////增加非锁定技能的补偿方式
//	//MaxDis = InAbility->eventGetMaxAttackDistance();
//
//	//if(InAbility->SpellCommonAttribute.LockType == ESL_Type1) //锁定自身技能
//	//{
//	//	eventAddCmdUseAbility(InAbility, Target, Point);
//	//}
//	//else if(InAbility->SpellCommonAttribute.LockType == ESL_Type2) //锁定角色技能
//	//{
//	//
//	//	eventAddCmdMoveToActor((Target, MaxDis * 0.75);
//	//	eventAddCmdUseAbility(InAbility, Target, Point, false);
//
//	//}
//	//else if(InAbility->SpellCommonAttribute.LockType == ESL_Type3) //鼠标指向
//	//{
//
//	//	eventAddCmdMoveToActor(Target, MaxDis * 0.75);
//	//	eventAddCmdUseAbility(InAbility, Target, Point, false);
//
//	//}
//
//
//	enemyActor = InterfaceCast<IIAbilityDamageable>(Target);
//	if((enemyActor != NULL))
//	{
//		if(!enemyActor->eventIsDead())
//		{
//			///*MinDis = */InAbility.GetMinAttackDistance();
//			MaxDis = InAbility->eventGetMaxAttackDistance();
//			CurrentDis = 0.f;
//			if(Cast<AMMOActor>(Target) != NULL)
//			{
//				CurrentDis = Cast<AMMOActor>(Target)->GetCollisionRadius();
//			}
//			CurrentDis = (Target->Location - actor->Location).Size() - CurrentDis - actor->GetCollisionRadius();
//			if(CurrentDis > MaxDis * 0.9)
//			{
//				if(actor->bFreeze)
//				{
//					return;
//				}
//				eventAddCmdMoveToActor(Target, MaxDis * 0.75);
//				eventAddCmdUseAbility(InAbility, Target, Point, false);
//			}
//			else
//			{
//				eventAddCmdUseAbility(InAbility, Target, Point);
//			}
//		}
//	}
//	else
//	{
//		eventAddCmdMoveToPoint(Point);
//	}
//
//}

UBOOL AMMOPlayerController::EnableMoving(FVector Des)
{
	FVector v1;
	FVector v2;
	FLOAT buf;
	FLOAT ActorDistance;
	UBOOL bTouch = FALSE;
	AMMOActor* Self = (AMMOActor*)Pawn;
	//add by jim ,用于actor接触阻挡的判定
	Self->onTouchActor2.Empty();
	for (TArrayNoInit<class AMMOActor*>::TIterator i(Self->onTouchActor); i; ++i)
	{
		AMMOActor *p = (*i);

		v1 = p->Location - Pawn->Location ;
		ActorDistance =  sqrt( v1.X*v1.X + v1.Y*v1.Y + v1.Z*v1.Z);
		//判定确实是相交的，容差系数0.8
		FLOAT d1 = Pawn->CylinderComponent ? Pawn->CylinderComponent->CollisionRadius : 0.0f;
		FLOAT d2 = p->CylinderComponent ? p->CylinderComponent->CollisionRadius : 0.0f;
		if( ActorDistance*0.8 < d1+d2 )
		{
			v1 = (Des-Pawn->Location);
			v2 = (p->Location - Pawn->Location );
			v1.Normalize();
			v2.Normalize();
			buf = v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z;
			if(buf>fPlayerColisionRatio)
			{
				Self->onTouchActor2.AddItem(p);
				bTouch = TRUE;
			}
		}
	}

	Self->onTouchActor.Empty();
	for (TArrayNoInit<class AMMOActor*>::TIterator i(Self->onTouchActor2); i; ++i)
	{
		AMMOActor *p = (*i);
		Self->onTouchActor.AddItem(p);
	}


	return !bTouch;

}
//------------------------------------------------------------------------
void AMMOPlayerController::OnQTEStart(class UBuff* BuffTemplate,INT objId, FLOAT LifeSpan)
{
	AMMOPlayer*		player		= Cast<AMMOPlayer>(Pawn);
	if(player)
	{
		UMMOAbility*	tPassive	= player->GetEquipedAbilityById( BuffTemplate->QTESpellId ); 
		if(tPassive)
		{	
			MMOUI::GetUIManager()->ShowProgressBarQTE(INT(LifeSpan*1000.0f),"Q",tPassive->GetAbilityIconName(),objId,BuffTemplate->QTESpellId);
			//MMOUI::GetUIManager()->delegateShowProgressBarQTE(INT(LifeSpan*1000.0f),"Q",tPassive->GetAbilityIconName(),objId,BuffTemplate->QTESpellId);//tPassive->eventGetAbilityId()
		}
	}
}

void AMMOPlayerController::OnQTEEnd(class UBuff* BuffTemplate, class AMMOActor* Target)
{
	MMOUI::GetUIManager()->CloseProgressBarQTE(0);
	//MMOUI::GetUIManager()->delegateCloseProgressBarQTE(0);
}

void AMMOPlayerController::OnAbilityTimerBarStart(class UMMOAbility* Skill, FLOAT Rate ,INT IntervalTime )//,  FLOAT TotalTime, FLOAT MinTime)
{
	UMMOUIGFXPlayer*  HudMovie = Cast<AMMOHud>(myHUD)->HudMovie;
	if(Skill)
	{
		FLOAT timeScale = 1.0/Rate;
		switch(Skill->SpellCommonAttribute.PrepareType)
		{
		case ES_PT_Type1: //引导
			{
				FLOAT TotalTime = FLOAT(Skill->SpellCommonAttribute.PrepareTimeMax - Skill->SpellCommonAttribute.PrepareTimeMin) / FLOAT(Skill->SpellCommonAttribute.Interval);
				TotalTime *= FLOAT(IntervalTime)/1000.f;
				MMOUI::GetUIManager()->ShowProgressBar(INT(TotalTime*1000.f),Skill->ClientCommonAttribute.AbilityName, 1);
				//MMOUI::GetUIManager()->delegateShowProgressBar(INT(TotalTime*1000.f),Skill->ClientCommonAttribute.AbilityName, 1);
				/////if(HudMovie)HudMovie->eventShowProgressBar(INT(TotalTime*1000.f),Skill->SpellCommonAttribute.AbilityName.ToString(), 1);
			}
			break;
		case ES_PT_Type2: //吟唱
			{
				FLOAT TotalTime = Skill->SpellCommonAttribute.PrepareTimeMin * timeScale;
				MMOUI::GetUIManager()->ShowProgressBar(INT(TotalTime*1000.f),Skill->ClientCommonAttribute.AbilityName, 0);
				//MMOUI::GetUIManager()->delegateShowProgressBar(INT(TotalTime*1000.f),Skill->ClientCommonAttribute.AbilityName, 0);
			}
			break;
		case ES_PT_Type5: //蓄力
			{
				FLOAT FullTime	= Skill->SpellCommonAttribute.AccumulateTimeMax * timeScale;
				MMOUI::GetUIManager()->ShowProgressBarStrength( INT(FullTime*1000.f) ,Skill->SpellCommonAttribute.AccumulateSeg );
				//MMOUI::GetUIManager()->delegateShowProgressBarStrength( INT(FullTime*1000.f) ,Skill->SpellCommonAttribute.AccumulateSeg );
			}
			break;
		default:
			break;
		}
	}
}
void AMMOPlayerController::OnAbilityTimerBarEnd(class UMMOAbility* Skill,UBOOL bBreak)
{
	if(Skill)
	{
		switch(Skill->SpellCommonAttribute.PrepareType)
		{
		case ES_PT_Type1: //引导
			if(bBreak){
				MMOUI::GetUIManager()->CloseProgressBar(INT(bBreak));
				//MMOUI::GetUIManager()->delegateCloseProgressBar(INT(bBreak));
			}
			break;
		case ES_PT_Type2: //吟唱
			MMOUI::GetUIManager()->CloseProgressBar(INT(bBreak));
			//MMOUI::GetUIManager()->delegateCloseProgressBar(INT(bBreak));
			break;
		case ES_PT_Type5: //蓄力
			MMOUI::GetUIManager()->CloseProgressBarStrength(INT(bBreak));
			//MMOUI::GetUIManager()->delegateCloseProgressBarStrength(INT(bBreak));
			break;
		default:
			break;
		}
	}
}
//-------------------------------------------------------------------------
INT AMMOPlayerController::GetQuadrant(FVector Des)
{
	FLOAT angleDir = TabToAngle(Pawn->Rotation.Yaw);
	FLOAT angleObj = HeadingAngle(Des);
	if(angleObj<0.f)angleObj = PI*2.f +angleObj;
	return Quadrant( FindDeltaAngle(   angleDir, angleObj+PI/4.f) );
}
//--------------------------------------------------------------------------
void AMMOPlayerController::ResetMoving()
{
	if(TargetLocationMarker)TargetLocationMarker->Hide();
	//TryingMoveDest =  Pawn->Location;// FVector(0,0,0);
	bHaveToGetNextWayPoint = FALSE;
}
//---------------------------------------------------------------------------

#if !SHIPPING_PC_GAME
void DrawLineSeg(const FVector& start, const FVector& end, BYTE R, BYTE G, BYTE B)
{
	MMOFacade::getSingleton().GetMainController()->DrawDebugSphere(start, 2, 16, R, G, B, TRUE);
	MMOFacade::getSingleton().GetMainController()->DrawDebugSphere(end, 2, 16, R, G, B, TRUE);
	MMOFacade::getSingleton().GetMainController()->DrawDebugLine(start, end, R, G, B, TRUE);
}
#endif


FVector AMMOPlayerController::StepSearchDest(FVector FootPos,FVector StartPos, FVector StepDir, FLOAT StepLength, INT StepNum)
{
	for(INT i=0;i<StepNum;i++)
	{
		FVector Pos = StartPos+StepDir*StepLength*FLOAT(i);
		UBOOL bFind = gServerGridWorld.GeneratePath(FootPos,Pos);

#if !SHIPPING_PC_GAME
		if(g_drawPathDebug)
		{
			DrawDebugSphere(Pos, 5, 16, 0, 255, 0, TRUE);
		}
#endif 

		if(bFind)
		{
			for(INT j=1;j<=4;j++)
			{
				Pos = StartPos+StepDir*StepLength*(FLOAT(i-1)+0.25f*FLOAT(j));
				bFind = gServerGridWorld.GeneratePath(FootPos,Pos);
				if(bFind)
				{
					return Pos;
				}
			}
		}
	}
	return FootPos;
}


FVector AMMOPlayerController::GetSafeDest(FVector DesPos,FLOAT SafeRadius)
{
	ZONE_DATA* pSrcZone = NULL;
	GRID_DATA* pSrcGrid = NULL;	
	gServerGridWorld.GetZoneAndGrid(DesPos, pSrcZone, pSrcGrid);
	if(pSrcGrid==NULL || pSrcGrid->bOutOfPoly || pSrcGrid->bBorder)
	{
		return DesPos;
	}

	FVector DeltaStep[8] = {FVector(1.0f,0.0,0.0),FVector(0.0f,1.0,0.0),FVector(-1.0f,0.0,0.0),FVector(0.0f,-1.0f,0.0),
							FVector(0.7071f,0.7071f,0.0),FVector(-0.7071f,0.7071,0.0),FVector(0.7071f,-0.7071f,0.0),FVector(-0.7071f,-0.7071f,0.0)};

	FVector DeltaStepOut = FVector(0.f,0.f,0.f);
	for(INT i=0;i<8;i++){
		gServerGridWorld.GetZoneAndGrid(DesPos+DeltaStep[i]*SafeRadius, pSrcZone, pSrcGrid);
	
		if(pSrcGrid==NULL || pSrcGrid->bOutOfPoly || pSrcGrid->bBorder)
		{
			DeltaStepOut += -DeltaStep[i];
		}
	}

	if(DeltaStepOut.X<-0.1)DeltaStepOut.X=-1.0f;
	else if(DeltaStepOut.X> 0.1)DeltaStepOut.X= 1.0f;

	if(DeltaStepOut.Y<-0.1)DeltaStepOut.Y=-1.0f;
	else if(DeltaStepOut.Y> 0.1)DeltaStepOut.Y= 1.0f;

	return DesPos+DeltaStepOut*SafeRadius;
}


FVector AMMOPlayerController::BisectionSearchDest(FVector FootPos,FVector StartPos, FVector EndPos, INT BisectionTimes)
{
	ZONE_DATA* pSrcZone = NULL;
	GRID_DATA* pSrcGrid = NULL;
	FVector BisectionS = StartPos;
	FVector BisectionD = EndPos;
	FVector BisectionM = (BisectionS+BisectionD)*0.5f;
	FVector OutPutPos  = FootPos;

	for(INT i=0;i<BisectionTimes;i++){
		gServerGridWorld.GetZoneAndGrid(BisectionM, pSrcZone, pSrcGrid);
		//UBOOL bFind = gServerGridWorld.GeneratePath(FootPos, BisectionM);
		if(pSrcGrid&&!pSrcGrid->bOutOfPoly&&!pSrcGrid->bBorder)	//   bFind)
		{
			OutPutPos  = BisectionM;
			BisectionS = BisectionM;
			BisectionM = (BisectionS+BisectionD)*0.5f;
		}
		else
		{
			BisectionD = BisectionM;
			BisectionM = (BisectionS+BisectionD)*0.5f;
		}
	}
	return GetSafeDest(OutPutPos,64.0f+16.0f);


	//FVector BisectionS = StartPos;
	//FVector BisectionD = EndPos;
	//FVector BisectionM = (BisectionS+BisectionD)*0.5f;
	//FVector OutPutPos  = FootPos;

	//for(INT i=0;i<BisectionTimes;i++){
	//	UBOOL bFind = gServerGridWorld.GeneratePath(FootPos, BisectionM);
	//	if(bFind)
	//	{
	//		OutPutPos  = BisectionM;
	//		BisectionS = BisectionM;
	//		BisectionM = (BisectionS+BisectionD)*0.5f;
	//	}
	//	else
	//	{
	//		BisectionD = BisectionM;
	//		BisectionM = (BisectionS+BisectionD)*0.5f;
	//	}
	//}
	//return OutPutPos;
}


//
//FVector AMMOPlayerController::OptimizeDest(FVector FootPos,FVector DesPos)
//{
//	FVector DeDir = FootPos-DesPos;
//	DeDir.Normalize();
//	FVector DeltaStep[4] = {FVector(1.0f,0.0,0.0),FVector(0.0f,1.0,0.0),FVector(-1.0f,0.0,0.0),FVector(0.0f,-1.0f,0.0)};
//
//	for(INT i=1;i<=4;i++)
//	{
//		for(INT j=0;j<4;j++)
//		{
//			FVector TestPos = DesPos + DeltaStep[j]*256.0f*FLOAT(i);
//			UBOOL bF = gServerGridWorld.GeneratePath(FootPos, TestPos);
//			if(bF)
//			{
//				FVector Output = BisectionSearchDest( FootPos, TestPos, TestPos-DeltaStep[j]*256.0f, 6);
//				Output = Output+DeltaStep[j]*64.0f;
//				return Output;
//			}
//		}
//	}
//	return FootPos;
//}



FVector AMMOPlayerController::OptimizeDest(FVector FootPos,FVector DesPos)
{
	ZONE_DATA* pSrcZone = NULL;
	GRID_DATA* pSrcGrid = NULL;

	FVector DeDir = FootPos-DesPos;
	DeDir.Normalize();
	FVector DeltaStep[8] = {FVector(1.0f,0.0,0.0),FVector(0.0f,1.0,0.0),FVector(-1.0f,0.0,0.0),FVector(0.0f,-1.0f,0.0),
							FVector(0.7071f,0.7071f,0.0),FVector(-0.7071f,0.7071,0.0),FVector(0.7071f,-0.7071f,0.0),FVector(-0.7071f,-0.7071f,0.0)};

	for(INT i=1;i<=16;i++)
	{
		for(INT j=0;j<8;j++)
		{
			FVector TestPos = DesPos + DeltaStep[j]*64.0f*FLOAT(i);
			gServerGridWorld.GetZoneAndGrid(TestPos, pSrcZone, pSrcGrid);
			if(pSrcGrid&&!pSrcGrid->bOutOfPoly&&!pSrcGrid->bBorder)
			{
				return GetSafeDest(TestPos,64.0f+16.0f);
			}
		}
	}
	return FootPos;
}




UBOOL AMMOPlayerController::ValidateMoveDest()
{
#ifdef SERVER_NAV_ALG

	TryingMoveDest.X = (INT)(TryingMoveDest.X + 0.5f);
	TryingMoveDest.Y = (INT)(TryingMoveDest.Y + 0.5f);
	TryingMoveDest.Z = (INT)(TryingMoveDest.Z + 0.5f);


#if 0 // .jump myd  .jump 15210 16534 19400 之后测试这段才OK
	TryingMoveDest = MouseHitWorldLocation;
	TryingMoveDest.X = 15213.110;
	TryingMoveDest.Y = 17302.793;
	TryingMoveDest.Z = 19415.904;
	debugf(TEXT("----%s -> %s"), *Pawn->Location.ToString(), *TryingMoveDest.ToString());
#endif

#if 0
	//  ----Move from: X=72300.000 Y=61700.000 Z=44198.000 -> X=72224.000 Y=60704.000 Z=44103.000
	TryingMoveDest.X = 72224;
	TryingMoveDest.Y = 60704;
	TryingMoveDest.Z = 44103;
#endif

#if GRID_CTRL_MODE
	TryingMoveDest.X = (INT)(TryingMoveDest.X / g_moveModGridSize) * g_moveModGridSize + 64 / 2;
	TryingMoveDest.Y = (INT)(TryingMoveDest.Y / g_moveModGridSize) * g_moveModGridSize + 64 / 2;
#endif



#if !SHIPPING_PC_GAME
	if(g_drawPathDebug)
	{
		DrawDebugStar(TryingMoveDest, 5, 0, 255, 0, TRUE);
//		DrawDebugBox(TryingMoveDest, FVector(g_moveModGridSize / 2, g_moveModGridSize / 2, g_moveModGridSize / 2), 0, 255, 0, TRUE);
		DrawDebugBox(TryingMoveDest, FVector(128.0f / 2, 128.0f / 2, 128.0f / 2), 0, 255, 0, TRUE);
		debugf(TEXT("----Move from: %s -> %s"), *Pawn->Location.ToString(), *TryingMoveDest.ToString());
	}
#endif

	UBOOL bOptimized = FALSE;
	TryingMoveDest.Z +=  g_pickZOffset;// TryingMoveDest +FVector(0.0,0.0,32.0f);
	TryingMoveDest =  gServerGridWorld.OptimizeDes(Pawn->Location, TryingMoveDest,bOptimized);		//  GetSafeDest(TryingMoveDest,64);
#if !SHIPPING_PC_GAME
	if(g_drawPathDebug)
	{
		DrawDebugSphere(TryingMoveDest, 8, 16, 255, 0, 0, TRUE);
	}
#endif	

	//判断有无必要寻路
	if(bOptimized && (Pawn->Location-TryingMoveDest).Size2D()<64.0f*3.0f )
	{
		return FALSE;
	}

	//判定可否直线直达
	UBOOL bFind = gServerGridWorld.GeneratePathLine(Pawn->Location, TryingMoveDest);
	if(bFind==FALSE){
		bFind = gServerGridWorld.GeneratePath(Pawn->Location, TryingMoveDest);
		gServerGridWorld.GeneratePathAnz();//路径优化
	}

#if !SHIPPING_PC_GAME
	FLinearColor navClr(0.0f, 1.0f, 1.0f);
	FVector drawDebugPathOffset(0, 0, 32.0f);
#endif


	INT32 pathLen = 0;
	const INT32* pathArr = gServerGridWorld.GetPath(pathLen);
	if(bFind)
	{
		if (pathLen <= 2 && bOptimized==FALSE)
		{
			bLinearMovement = true;
		}
		else
		{
			bLinearMovement = false;
		}
	}
	//else//寻路失败的时候
	//{
	//	//准备--------------------------------------------------------------------------------
	//	bLinearMovement = false;
	//	FLOAT   DeltaStep = 64.0f;
	//	FVector footLocation = Pawn->Location - FVector(0.0,0.0,Pawn->CylinderComponent->CollisionHeight * 0.5f);
	//	FLOAT deltaZ = footLocation.Z - MouseProjectionOrigin.Z;
	//	FLOAT DirectionZ = MouseProjectionDirection.Z;
	//	FVector fixedTargetPos = MouseProjectionOrigin + MouseProjectionDirection * (deltaZ / DirectionZ);
	//	FVector Dir = fixedTargetPos-footLocation;
	//	Dir.Normalize();

	//	//判定是否直线行走还是最近点行走
	//	UBOOL bF = gServerGridWorld.GeneratePath(footLocation, footLocation+Dir*DeltaStep*2.0f);
	//	if(bF)//直线行走
	//	{
	//		FVector TestPos_Target = BisectionSearchDest(footLocation,footLocation,fixedTargetPos,8);
	//		//TestPos_Target = TestPos_Target - Dir*DeltaStep;
	//		#if !SHIPPING_PC_GAME
	//			if(g_drawPathDebug)
	//			{
	//				DrawDebugSphere(TestPos_Target, 5, 16, 0, 255, 0, TRUE);
	//			}
	//		#endif 

	//		bFind = gServerGridWorld.GeneratePath(footLocation, TestPos_Target);
	//		if(bFind==FALSE)
	//		{
	//			debugf(TEXT("----Find == FALSE"));
	//		}
	//	}
	//	else
	//	{

	//		FVector TestPos_Target = OptimizeDest(footLocation,fixedTargetPos);
	//		#if !SHIPPING_PC_GAME
	//			if(g_drawPathDebug)
	//			{
	//				DrawDebugSphere(TestPos_Target, 5, 16, 0, 255, 0, TRUE);
	//			}
	//		#endif		

	//		if((TestPos_Target-footLocation).Size2D()>=DeltaStep*6.0f){
	//			bFind = gServerGridWorld.GeneratePath(footLocation, TestPos_Target);
	//		}

	//	}
	//}




//
//		FVector TestPos_Target = BisectionSearchDest(footLocation,footLocation,fixedTargetPos,8);
//		if((TestPos_Target-Pawn->Location).Size2D()>=64.0f+64.0f )
//		{
//			//离边缘有64距离
//			FVector dir = TestPos_Target - Pawn->Location;
//			dir.Normalize();
//			FVector des = TestPos_Target-dir*64.0f;
//
//#if !SHIPPING_PC_GAME
//			if(g_drawPathDebug)
//			{
//				DrawDebugSphere(des, 5, 16, 0, 255, 0, TRUE);
//			}
//#endif 
//			bFind = gServerGridWorld.GeneratePath(Pawn->Location, des);
//		}
//		else
//		{
//			FVector vec = footLocation-fixedTargetPos;
//
//			if(abs(vec.X)<=abs(vec.Y))
//			{
//				FVector vecX= vec;
//				vecX.Y = 0.0f;vecX.Z = 0.0f;
//				vecX.Normalize();
//
//				FVector D1 = StepSearchDest(footLocation,fixedTargetPos, vecX, 128.0f, 8);
//				
//
//
//			}
//
//
//
//
//			FVector vecX= vec;
//			vecX.Y = 0.0f;vecX.Z = 0.0f;
//			vecX.Normalize();
//
//			FVector vecY= vec;
//			vecY.X = 0.0f;vecY.Z = 0.0f;
//			vecY.Normalize();
//
//
//
//			FVector D1 = StepSearchDest(footLocation,fixedTargetPos, vecX, 128.0f, 8);
//			FVector D2 = StepSearchDest(footLocation,fixedTargetPos, vecY, 128.0f, 8);
//							
//			if((D1-fixedTargetPos).SizeSquared2D() < (D2-fixedTargetPos).SizeSquared2D())
//			{
//				TestPos_Target = D1;
//			}
//			else
//			{
//				TestPos_Target = D2;
//			}
//
//			if((TestPos_Target-footLocation).Size2D()>=64.0f+64.0f )
//			{
//				bFind = gServerGridWorld.GeneratePath(Pawn->Location, TestPos_Target);
//			}
//			else
//			{
//				bFind = FALSE;
//			}
//		}
//
//	}



//
//
//		bLinearMovement = false;
//		FVector footLocation = Pawn->Location - FVector(0.0,0.0,Pawn->CylinderComponent->CollisionHeight * 0.5f);
//		FLOAT deltaZ = footLocation.Z - MouseProjectionOrigin.Z;
//		FLOAT DirectionZ = MouseProjectionDirection.Z;
//		FVector fixedTargetPos = MouseProjectionOrigin + MouseProjectionDirection * (deltaZ / DirectionZ);
//
//		FVector TestPos2 = fixedTargetPos;
//		FVector TestPos1 = (footLocation+fixedTargetPos)*0.5f;
//		FVector TestPos0 = footLocation;
//
//		UBOOL	bCanMove = FALSE;
//		FVector TestPos_Target =footLocation;
//		for(INT i=0;i<8;i++){
//			UBOOL bFind_Sub = gServerGridWorld.GeneratePath(footLocation, TestPos1);
//			if(bFind_Sub)
//			{
//				bCanMove = TRUE;
//				TestPos_Target = TestPos1;
//
//				TestPos0 = TestPos1;
//				TestPos1 = (TestPos0+TestPos2)*0.5f;
//
//
//			}
//			else
//			{
//				TestPos2 = TestPos1;
//				TestPos1 = (TestPos0+TestPos2)*0.5f;
//			}
//		}
//
//		if(bCanMove && (TestPos_Target-Pawn->Location).Size2D()>=64.0f+64.0f )
//		{
//			//离边缘有64距离
//			FVector dir = TestPos_Target - Pawn->Location;
//			dir.Normalize();
//			FVector des = TestPos_Target-dir*64.0f;
//
//#if !SHIPPING_PC_GAME
//			if(g_drawPathDebug)
//			{
//				DrawDebugSphere(des, 5, 16, 0, 255, 0, TRUE);
//			}
//#endif 
//
//			bFind = gServerGridWorld.GeneratePath(Pawn->Location, des);
//		}
//		else
//		{
//			FVector vec = footLocation-fixedTargetPos;
//			
//			FVector vecX= vec;
//			vecX.Y = 0.0f;vecX.Z = 0.0f;
//			vecX.Normalize();
//
//			FVector vecY= vec;
//			vecY.X = 0.0f;vecY.Z = 0.0f;
//			vecY.Normalize();
//
//			FVector vecOptimize;
//
//			for(INT i=1;i<8;i++)
//			{
//				vecOptimize = fixedTargetPos+vecX*128.0f*FLOAT(i);
//				bFind = gServerGridWorld.GeneratePath(footLocation, vecOptimize );
//				if(bFind)
//				{
//					INT num=0;
//					for(INT j=1;j<=4+2;j++){
//						vecOptimize = fixedTargetPos+vecX*(128.0f*FLOAT(i-1)+32.0f*FLOAT(j));
//						bFind = gServerGridWorld.GeneratePath(footLocation, vecOptimize);
//						if(bFind)
//						{
//							num++;
//							if(num>2)break;
//						}
//					}
//#if !SHIPPING_PC_GAME
//					if(g_drawPathDebug)
//					{
//						DrawDebugSphere(vecOptimize, 5, 16, 0, 255, 0, TRUE);
//					}
//#endif 
//					break;
//				}
//
//				vecOptimize = fixedTargetPos+vecY*128.0f*FLOAT(i);
//				bFind = gServerGridWorld.GeneratePath(footLocation, vecOptimize );
//				if(bFind)
//				{
//					INT num=0;
//					for(INT j=1;j<=4+2;j++){
//						vecOptimize = fixedTargetPos+vecY*(128.0f*FLOAT(i-1)+32.0f*FLOAT(j));
//						bFind = gServerGridWorld.GeneratePath(footLocation, vecOptimize);
//						if(bFind)
//						{
//							num++;
//							if(num>2)break;
//						}
//					}
//
//#if !SHIPPING_PC_GAME
//					if(g_drawPathDebug)
//					{
//						DrawDebugSphere(vecOptimize, 5, 16, 0, 255, 0, TRUE);
//					}
//#endif 
//					break;
//				}
//			}
//
//			if(	bFind && (vecOptimize-Pawn->Location).Size2D()<64.0f+64.0f	)
//			{
//				bFind = FALSE;
//			}
//		}



////-------------------------------------------------------------------------------------------------------------------------
//		bLinearMovement = false;
//		FVector footLocation = Pawn->Location - FVector(0.0,0.0,Pawn->CylinderComponent->CollisionHeight * 0.5f);
//		FLOAT deltaZ = footLocation.Z - MouseProjectionOrigin.Z;
//		FLOAT DirectionZ = MouseProjectionDirection.Z;
//		FVector fixedTargetPos = MouseProjectionOrigin + MouseProjectionDirection * (deltaZ / DirectionZ);
//
//		//DrawDebugSphere(fixedTargetPos, 10, 16, 255, 0, 255, TRUE);
//
//		FVector dir = fixedTargetPos - footLocation;
//		dir = dir.SafeNormal2D();
//		dir.Z = 0.0f;
//		//Pawn->SetDesiredRotation(dir.Rotation(), FALSE, TRUE, -1.0f, TRUE);
//
//		INT k = 1;
//		for (k = 1; k <= 16; k+=1)
//		{
//			debugf(TEXT("=== Trying move forward delta: %d"), k);
//			FVector moveDist = Pawn->Location + dir * g_moveModGridSize * k;
//
//			moveDist.X = (INT)(moveDist.X / g_moveModGridSize) * g_moveModGridSize + 64 / 2;
//			moveDist.Y = (INT)(moveDist.Y / g_moveModGridSize) * g_moveModGridSize + 64 / 2;
//
//			bFind = gServerGridWorld.GeneratePath(Pawn->Location, moveDist);
//
//#if !SHIPPING_PC_GAME
//			if(g_drawPathDebug)
//			{
//				DrawDebugSphere(moveDist, 5, 16, 0, 255, 0, TRUE);
//			}
//#endif 
//
//			if(!bFind)
//			{
//				break;
//			}
//		}
//
//		if(bFind==FALSE && k>1)
//		{
//			// 找到后要退回1格，并取格子化坐标
//			FVector moveDist = Pawn->Location + dir * g_moveModGridSize * Max(0, (k - 1));
//			moveDist.X = (INT)(moveDist.X / g_moveModGridSize) * g_moveModGridSize + 64 / 2; // 只能按64的半格来走
//			moveDist.Y = (INT)(moveDist.Y / g_moveModGridSize) * g_moveModGridSize + 64 / 2;
//
//			bFind = gServerGridWorld.GeneratePath(Pawn->Location, moveDist);
//
//#if !SHIPPING_PC_GAME
//			if(g_drawPathDebug)
//			{
//				DrawDebugBox(moveDist, FVector(g_moveModGridSize / 2, g_moveModGridSize / 2, g_moveModGridSize / 2), 255, 255, 0, TRUE);
//				DrawDebugSphere(moveDist, 5, 16, 0, 255, 0, TRUE);
//			}
//#endif 
//		}
////-------------------------------------------------------------------------------------------------------------------------


//	} // 点到不可走的点时
	//----------------------------------------------------------------------------------------------------------------------------------------


//#if !SHIPPING_PC_GAME
//	if(g_drawPathDebug)
//	{
//		gServerGridWorld.DebugDrawPathResult(this);
//
//		if(pathLen > 1)
//		{
//			DrawDebugSphere(MouseHitWorldLocation, 15, 16, 0, 255, 0, TRUE);
//			DrawDebugSphere(TryingMoveDest, 10, 16, 255, 0, 255, TRUE);
//			DrawDebugSphere(Pawn->Location, 10, 16, 0, 255, 255, TRUE);
//
//			FColor drawClr = navClr.ToFColor(FALSE);
//			for (INT i = 1; i < pathLen; ++i)
//			{
//				FVector pt1, pt2;
//				gServerGridWorld.FetchPath(pt1, i - 1);
//				gServerGridWorld.FetchPath(pt2, i);
//				DrawDebugLine(pt1 + drawDebugPathOffset, pt2 + drawDebugPathOffset, drawClr.R, drawClr.G, drawClr.B, TRUE);
//				DrawDebugSphere(pt2 + drawDebugPathOffset, 5, 8, drawClr.R, drawClr.G, drawClr.B, TRUE);
//			}
//		}
//	}
//#endif

#endif
	

	//warnf(NAME_Error, TEXT("寻路开始----------------------------------------"));
#if !SHIPPING_PC_GAME
if(g_drawPathDebug)
{
	if(bFind)
	{
		FVector wayPoint;
		int     wayPointIndex=0;
		bool	bRes = true;
		warnf( TEXT("寻路算法开始----------from: %s -> %s"),*Pawn->Location.ToString(), *TryingMoveDest.ToString());
		while(bRes){
			bRes = gServerGridWorld.FetchPath(wayPoint, wayPointIndex++);
			if (bRes)
				warnf( TEXT("P%d: X:%f Y:%f Z:%f"),wayPointIndex,wayPoint.X,wayPoint.Y,wayPoint.Z);
		}
		warnf( TEXT("寻路算法结束------------------------------------------"));
	}
}
#endif
	return bFind;
}

void AMMOPlayerController::SetupPathfindingParams( FNavMeshPathParams& out_ParamCache )
{
	// it will be called by the NavigationHandle->GetValidatedAnchorPosition in (AMMOPlayerController::ValidateMoveDest)
	__super::SetupPathfindingParams(out_ParamCache);

	out_ParamCache.SearchExtent.X = PathFindCollisionRadius;
	out_ParamCache.SearchExtent.Y = PathFindCollisionRadius;
}


void AMMOPlayerController::AutoRotation()
{
	if(!GIsEditor && MMO::GetCurrentGameState() != EGameState_InGame)
	{	
		return;
	}

	if(Pawn && Pawn->isMoving)
	{
		return;
	}

	FVector footLocation = Pawn->Location - FVector(0.0,0.0,Pawn->CylinderComponent->CollisionHeight * 0.5f);
	FLOAT deltaZ = footLocation.Z - MouseProjectionOrigin.Z;
	FLOAT DirectionZ = MouseProjectionDirection.Z;
	FVector fixedTargetPos = MouseProjectionOrigin + MouseProjectionDirection * (deltaZ / DirectionZ);

	FVector dir = fixedTargetPos - footLocation;
	dir = dir.SafeNormal2D();
	dir.Z = 0.0f;
	FRotator Rot = dir.Rotation();

	if(Pawn)
	{
		//Pawn->LockDesiredRotation(false, true);
		//Pawn->SetDesiredRotation(Rot, FALSE, TRUE, 0.1f, TRUE);
		CMessageHandler::GetInstance().UpdateRotation(Rot);
	}
}


//FVector AMMOPlayerController::GetOptimizedDes(FVector Des)
//{
//	ZONE_DATA* pSrcZone = NULL;
//	GRID_DATA* pSrcGrid = NULL;
//	FVector    offset[4]= {FVector(-64.0f,-64.0f,0.0f),FVector(64.0f,64.0f,0.0f),FVector(-64.0f,64.0f,0.0f),FVector(64.0f,-64.0f,0.0f)};
//	//FVector    offset[8]= {	FVector(-64.0f,64.0f,0.0f),FVector(0.0f,64.0f,0.0f),FVector(64.0f,64.0f,0.0f),
//	//						FVector(-64.0f,0.0f,0.0f),							FVector(64.0f,0.0f,0.0f),
//	//						FVector(-64.0f,-64.0f,0.0f),FVector(0.0f,-64.0f,0.0f),FVector(64.0f,64.0f,0.0f)	};
//
//	FVector    offsetOut= FVector(0.0f,0.0f,0.0f);
//	for(INT j=0;j<4;j++){
//		gServerGridWorld.GetZoneAndGrid(Des+offset[j], pSrcZone, pSrcGrid);
//		if(pSrcGrid==NULL || pSrcGrid->bOutOfPoly)
//		{
//			offsetOut += -offset[j];
//		}
//	}	
//	return Des+offsetOut;
//}

void AMMOPlayerController::WantToMove(FVector Des,UBOOL bRunOrWalk,FLOAT minRadius)
{
	
	AMMOPlayer* pPlayer = GetPlayer();
	if(pPlayer==NULL)return;

	if(pPlayer->isMoving)return;

	if(pPlayer->CanMove()==FALSE)return;

	if(FSMIO::getSingleton().Trigger.GetWaitingSendMove()==TRUE)return;

	//warnf(TEXT("===============================WantToMove222 Des:x=%f,y=%f"),Des.X,Des.Y);
	NativeMoveToPoint(Des,bRunOrWalk,TRUE,minRadius);
}



void AMMOPlayerController::UpdateMoving()
{
	if(!GIsEditor && MMO::GetCurrentGameState() != EGameState_InGame)
	{	
		return;
	}
	//MMOGameEvent_Any evt1(GameEvent_UpdateMovePath);
	//MMOUI::PostGameEvent(&evt1);

	//FSMIO::getSingleton().Ctrl.UpdateMoving();
	return ;





//
//
//#ifdef SERVER_NAV_ALG
//
//	if(!GIsEditor && MMO::GetCurrentGameState() != EGameState_InGame)
//	{	
//		return;
//	}
//
//	///////////////////////是否在有效的区域内///////////////////////////////
//
//	//ZONE_DATA* pSrcZone = NULL;
//	//GRID_DATA* pSrcGrid = NULL;
//	//gServerGridWorld.GetZoneAndGrid(Pawn->Location, pSrcZone, pSrcGrid);
//
//	//if (!pSrcZone || !pSrcGrid)
//	//{
//	//	warnf(NAME_Error, TEXT("发现角色陷入障碍物深处，寻路失败，有可能地图的navMesh需要重新生成一下，和服务器差别过大，请查找原因！"));
//	//	return;
//	//}
//
//	//ZONE_DATA* pDestZone = NULL;
//	//GRID_DATA* pDestGrid = NULL;
//	//gServerGridWorld.GetZoneAndGrid(TryingMoveDest, pDestZone, pDestGrid);
//
//	//if (!pDestZone || !pDestGrid)
//	//{
//	//	warnf(NAME_Error, TEXT("目标点无效，请检查"));
//	//	return;
//	//}
//
//	MMOGameEvent_Any evt(GameEvent_UpdateMovePath);
//	MMOUI::PostGameEvent(&evt);
//
//	FVector NextPoint;
//	FVector MicroNextPoint;
//	//寻路过程中处理
//	UBOOL bRes = gServerGridWorld.FetchPath(NextPoint);
//	if(bRes)
//	{
//		while(bRes && (NextPoint-Pawn->Location).SizeSquared2D()<4.0f)
//		{
//			 bRes = gServerGridWorld.FetchPath(NextPoint);
//		}
//	}
//
//	if( bRes ) //gServerGridWorld.FetchPath(NextPoint) ) // Pawn->GetCollisionRadius()/5 ))
//	{
//		if(NextPoint.Z<0.01 && NextPoint.Z>-0.01)
//		{
//			NextPoint.Z = Pawn->Location.Z;
//		}
//
//		MicroNextPoint = Pawn->Location + (NextPoint-Pawn->Location)*0.0001f;
//		AMMOPlayer* player = Cast<AMMOPlayer>( Pawn );
//		if(MMOFacade::getSingleton().CheckPlayerBlock(Pawn->Location, MicroNextPoint,player)	)
//		{
//			bHaveToGetNextWayPoint = FALSE;
//			warnf(TEXT("行走被阻挡"));
//			FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
//			if (TargetLocationMarker)
//			{
//				TargetLocationMarker->Hide();
//			}
//		}
//		else
//		{
//			//SendRequest_MoveTo(	GetOptimizedDes(NextPoint)	);
//			//warnf(NAME_Error, TEXT("寻路中途点------UpdateMoving-------x:%f   y:%f"),NextPoint.X,NextPoint.Y );
//#ifdef DEBUG_SYNC_INFO
//			warnf(TEXT("行走过程中，中途折点，发送行走消息-----------X:%f Y:%f Z:%f"),NextPoint.X,NextPoint.Y,NextPoint.Z );
//#endif
//			SendRequest_MoveTo(	NextPoint	);
//		}
//	}
//	else//如果中途断掉，重新寻路
//	{
//		bHaveToGetNextWayPoint = FALSE;
//		//bLinearMovement=false;
//		warnf(TEXT("寻路结束"));
//		FSMIO::getSingleton().Trigger.SetWaitingSendMove(FALSE);
//		if (TargetLocationMarker)
//		{
//			TargetLocationMarker->Hide();
//		}
//	}
//
//#else	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	if(!GIsEditor && MMO::GetCurrentGameState() != EGameState_InGame)
//	{	
//		return;
//	}
//
//	FVector TempDes,NextPoint,ClosedPos;
//	UBOOL   bFoundPath = FALSE;
//	APylon *TestPylonS, *TestPylonD;
//	UBOOL   b2D = FALSE;
//	FNavMeshPolyBase* PolySrc = NULL;
//	FNavMeshPolyBase* PolyDest = NULL;
//
//	//1. 判断是不是走到目标点了
//	if( (TryingMoveDest - Pawn->Location).Size2D() < 10.0f )
//	{
//		if(TargetLocationMarker)TargetLocationMarker->Hide();
//		return;
//	}
//
//	TestPylonS = NavigationHandle->GetPylonFromPos(Pawn->Location, &PolySrc);
//	TestPylonD = NavigationHandle->GetPylonFromPos(TryingMoveDest, &PolyDest);
//
//	//人在navMesh之外，先直线走到navMesh上离他位置最近的点
//	//FLOAT fCollisionRadius = Pawn->GetCollisionRadius();
//	FLOAT fCollisionRadius = PathFindCollisionRadius;
//	if(TestPylonS == NULL)
//	{
//		bHaveToGetNextWayPoint = FALSE;
//		if( NavigationHandle->GetClosedPosInNavigationMash(Pawn->Location,
//			Pawn->WalkableFloorZ, 64.0f, fCollisionRadius, ClosedPos )   )
//		{
//			bLinearMovement=false;
//			//eventRequestAndDoMove(ClosedPos, Pawn->GroundSpeed, TRUE);
//			SendRequest_MoveTo(ClosedPos);
//			return;
//		}
//		else
//		{
//			warnf(NAME_Error, TEXT("发现角色陷入障碍物深处，寻路失败，有可能地图的navMesh需要重新生成一下，和服务器差别过大，请查找原因！"));
//			return;
//		}
//	}
//
//	//目标点在navMesh之外，先寻找离目标点最近的在navMesh上的点
//	if(TestPylonD == NULL)
//	{
//		bHaveToGetNextWayPoint = FALSE;
//		b2D = TRUE;
//
//		//预先寻找最近的导航点
//		if( NavigationHandle->GetClosedPosInNavigationMash(
//			TryingMoveDest, Pawn->WalkableFloorZ, FLOAT(64 * 16),
//			fCollisionRadius, ClosedPos )   )//先搜寻范围32倍格子
//		{
//			bOptimizeTargetPos = (TryingMoveDest - ClosedPos).Size2D() > fCollisionRadius;
//			TestPylonD = NavigationHandle->GetPylonFromPos(ClosedPos);
//			//if(TestPylonD){
//			TryingMoveDest = ClosedPos;
//			//}
//		}
//		else{
//
//			AMMOPlayerCamera* PCCam = Cast<AMMOPlayerCamera>(PlayerCamera);
//			if(PCCam)
//			{
//				FVector P= PCCam->CameraCache.POV.Location;
//				FVector D= TryingMoveDest-P;
//				FLOAT L = P.Z-TryingMoveDest.Z;
//				FLOAT S = Pawn->Location.Z-TryingMoveDest.Z;
//				TryingMoveDest = P + D*(L-S)/L;
//				warnf(NAME_Error, TEXT("在目标点没有找到navMesh，自动转成2d屏幕鼠标坐标"));
//			}
//
//
//			INT Loop = 1;
//			const INT LoopLimit = 8;
//			while(TestPylonD==NULL&&Loop<LoopLimit)
//			{
//
//				if( NavigationHandle->GetClosedPosInNavigationMash(
//					TryingMoveDest, Pawn->WalkableFloorZ, FLOAT(64 * 16),
//					fCollisionRadius, ClosedPos )   )//先搜寻范围32倍格子
//				{
//					//bOptimizeTargetPos = (TryingMoveDest - ClosedPos).Size2D() > fCollisionRadius;
//					TestPylonD = NavigationHandle->GetPylonFromPos(TryingMoveDest);
//
//					if(TestPylonD)
//					{
//						TryingMoveDest = ClosedPos;
//						TestPylonD = NavigationHandle->GetPylonFromPos(TryingMoveDest);
//						break;
//					}
//				}
//
//				TryingMoveDest = (TryingMoveDest+Pawn->Location)/2.0;
//				//TestPylonD = NavigationHandle->GetPylonFromPos(TryingMoveDest);
//				warnf(NAME_Error, TEXT("路径预折分！%d"), Loop );
//				Loop = Loop+1;
//			}
//		}
//	}
//
//
//	if(TestPylonD)
//	{
//		if(bHaveToGetNextWayPoint == FALSE)//寻路过程中不进行匹配
//		{
//			//如果点击在poly内，但是离边缘小于碰撞半径的情况下
//			FVector TryingMoveDest2 = TestPylonD->NavMeshPtr->W2LTransformFVector(TryingMoveDest);
//			if(TestPylonD->NavMeshPtr->ContainsPoint( TryingMoveDest2 ) == FALSE )
//			{
//				TryingMoveDest.Z += 64;
//			}
//
//			FNavMeshPolyBase* pPoly = TestPylonD->NavMeshPtr->GetPolyFromPoint(TryingMoveDest,Pawn->WalkableFloorZ);
//
//
//			////////////////////////added by zhaojun@snda begin////////////////////////
//
//			//FNavMeshPolyBase* pPoly = TestPylonD->NavMeshPtr->GetPolyFromPoint(TryingMoveDest,Pawn->WalkableFloorZ);
//
//			//if (!pPoly)
//			//{
//			//	warnf(NAME_Error, TEXT("終點無效"));
//			//	return;
//			//}
//
//			//FVector vOffset(FVector::ZeroVector);
//
//			//int NumEdges = pPoly->GetNumEdges();
//
//			//for (int i = 0; i < NumEdges; ++i)
//			//{
//			//	FNavMeshEdgeBase* pEdge = pPoly->GetEdgeFromIdx(i);
//
//			//	// if it is an one way edge, away from it
//			//	if (pEdge != NULL && pEdge->IsIslandEdge())
//			//	{
//			//		//float fDis = pEdge->PointDistToEdge(TryingMoveDest);
//			//		FVector ClosestPoint;
//
//			//		float fDis = PointDistToSegment(TryingMoveDest, 
//			//			pEdge->GetVertLocation(0), pEdge->GetVertLocation(1), 
//			//			ClosestPoint);
//
//			//		FVector tempOffset = TryingMoveDest - ClosestPoint;
//
//			//		FLOAT fDis2D = tempOffset.Size2D();
//			//		if (fDis2D < fCollisionRadius && fDis2D > 0)
//			//		{
//			//			tempOffset.Normalize();
//			//			vOffset += ((fCollisionRadius - fDis2D) * tempOffset);
//			//		}
//			//	}
//			//}
//
//			//if (vOffset != FVector::ZeroVector 
//			//	&& NavigationHandle->GetPylonFromPos(TryingMoveDest + vOffset))
//			//{
//			//	warnf(TEXT("============Offset============== %f, %f, %f"), vOffset.X, vOffset.Y, vOffset.Z);
//			//	TryingMoveDest += vOffset;
//			//}
//
//			////////////////////added by zhaojun@snda end//////////////////////
//
//			if(b2D)
//			{
//				bOptimizeTargetPos = TRUE;
//			}
//			else if(pPoly && pPoly->GetBestLocationForCyl(TryingMoveDest, 
//				fCollisionRadius, Pawn->WalkableFloorZ,ClosedPos) )
//			{
//				bOptimizeTargetPos = ((TryingMoveDest - ClosedPos).Size2D() > fCollisionRadius);
//				TryingMoveDest = ClosedPos;
//			}
//			else
//			{
//				bOptimizeTargetPos = FALSE;
//			}
//		}
//	}
//
//	//判断是否在寻路过程中
//	if(bHaveToGetNextWayPoint == FALSE)
//	{
//		bFoundPath = eventFindPath(TryingMoveDest);
//
//		if(bFoundPath==false)//如果寻路不到
//		{
//			warnf(NAME_Error, TEXT("路径不通！"));
//			//
//			FString errorStr = MMOUI::ParseLocalizedPropertyPath( TEXT("MMOErrorInfo.Navigation.NoPath") );
//			MMOUI::GetUIManager()->delegateAddScreenMessage( errorStr ,MsgInfoType_Table,TEXT("MMOErrorInfo.Navigation.NoPath"));
//			//MMOChatManager::getSingleton().AddSystemMessage( errorStr );
//
//			TryingMoveDest = Pawn->Location;
//			return;
//		}
//	}
//
//	//寻路过程中处理
//	if( m_NavHelper->GetNextMoveLocation( NextPoint, fCollisionRadius)) // Pawn->GetCollisionRadius()/5 ))
//	{
//		MMOGameEvent_Any evt(GameEvent_UpdateMovePath);
//		MMOUI::PostGameEvent(&evt);
//
//		bHaveToGetNextWayPoint = TRUE;
//
//		FVector OutClosestPoint;
//		FVector point = TryingMoveDest;
//		FVector startPoint = Pawn->Location;
//		FVector endPoint = NextPoint;
//		point.Z = 0;
//		startPoint.Z = 0;
//		endPoint.Z = 0;
//
//		//防止寻找失败，但是报正确，其实点在原点没动，navmesh连这种bug都有，要命咧
//		if((NextPoint-Pawn->Location).SizeSquared2D()<KINDA_SMALL_NUMBER)
//		{
//			endPoint = NextPoint = TryingMoveDest;
//			endPoint.Z = 0;
//		}
//
//		FLOAT dis = PointDistToSegment(point, startPoint, endPoint, OutClosestPoint); 
//
//		//因为有碰撞半径，有时候搜点的时候会先到一个远的点,（这个nevMesh有点傻，途经终点而不停，奔到远点后再回奔终点）
//		if(dis<fCollisionRadius && (NextPoint-Pawn->Location).SizeSquared2D()>=(TryingMoveDest-Pawn->Location).SizeSquared2D()   )
//		{
//			bLinearMovement=true;
//			//eventRequestAndDoMove(TryingMoveDest, Pawn->GroundSpeed,TRUE);
//			SendRequest_MoveTo(TryingMoveDest);
//			//GWorld->GetWorldInfo()->DrawDebugLine(TryingMoveDest+FVector(0.0f,0.0f,1000.f),TryingMoveDest+FVector(0.0f,0.0f,-1000.f),255,255,0,TRUE);
//			//FVector delta = TryingMoveDest - Pawn->Location;
//			//warnf(NAME_Error, TEXT("这是终点next : %f,%f pos:%f,%f"),delta.Size2D(),delta.SizeSquared2D(), TryingMoveDest.X,TryingMoveDest.Y );
//		}
//		else
//		{
//			if((NextPoint-Pawn->Location).Size2D()+(TryingMoveDest-NextPoint).Size2D() >  (TryingMoveDest-Pawn->Location).Size2D()*1.1  )
//			{
//				bLinearMovement=false;
//			}
//			else
//			{
//				bLinearMovement=true;
//			}
//			//eventRequestAndDoMove(NextPoint, Pawn->GroundSpeed,TRUE);
//			SendRequest_MoveTo(NextPoint);
//			//FVector delta = NextPoint - Pawn->Location;
//			//GWorld->GetWorldInfo()->DrawDebugLine(NextPoint+FVector(0.0f,0.0f,1000.f),NextPoint+FVector(0.0f,0.0f,-1000.f),0,255,255,TRUE);
//			//warnf(NAME_Error, TEXT("next : %f,%f pos:%f,%f"),delta.Size2D(),delta.SizeSquared2D(), NextPoint.X,NextPoint.Y );
//		}
//
//	}
//	else//如果中途断掉，重新寻路
//	{
//		bHaveToGetNextWayPoint = FALSE;
//		//eventMoveToPoint(TryingMoveDest);
//		bLinearMovement=false;
//		//eventRequestAndDoMove(TryingMoveDest, Pawn->GroundSpeed,TRUE);
//		SendRequest_MoveTo(TryingMoveDest);
//		warnf(NAME_Error, TEXT("寻路中途，找不到下一点了"));
//		//UpdateMoving();
//	}
//#endif
}

void AMMOPlayerController::StopMove(UBOOL NeedToNotifyServer)
{
	if (NeedToNotifyServer)
	{
		if (bNeedCommitPosition)
		{
			SendRequestStop();
		}
	}
	else{
		AMMOActor* mmoActor = Cast<AMMOActor>(Pawn);
		if (mmoActor)
		{
			mmoActor->StopMovement();
		}
		bHaveToGetNextWayPoint = FALSE;
		IsLeftMousePressed = FALSE;
	}

	if (TargetLocationMarker)
	{
		TargetLocationMarker->Hide();
	}
}

FVector2D AMMOPlayerController::GetMousePosition()
{
	ULocalPlayer* LP = Cast<ULocalPlayer>(Player);
	if(LP && LP->ViewportClient)
	{
		return LP->ViewportClient->GetMousePosition();
	}
	return FVector2D(-100.0f, -100.0f);
}

void AMMOPlayerController::NativeGeneratePickRay()
{
	ULocalPlayer* LP = Cast<ULocalPlayer>(Player);
	if(!LP)
	{
		return;
	}

	// Get mouse position
	FVector2D MousePos =  LP->ViewportClient->GetMousePosition();
	if((MousePos-LastScreenMousePos).IsNearlyZero() )
	{
		IsMouseMoving = FALSE;
	}
	else
	{
		IsMouseMoving = TRUE;
	}
	LastScreenMousePos = MousePos;

	FVector WorldOrigin, WorldDir;

	FVector2D ViewPortSize;
	LP->ViewportClient->GetViewportSize(ViewPortSize);
	if ( ViewPortSize.IsNearlyZero() )
		return;

	FVector2D RelativeMousePos = FVector2D(MousePos.X/ViewPortSize.X,MousePos.Y/ViewPortSize.Y );
	LP->DeProject(RelativeMousePos, WorldOrigin, WorldDir);

	// Do the Pick
	PlayerMouse = MousePos;
	MouseProjectionOrigin = WorldOrigin;
	MouseProjectionDirection = WorldDir;


	AMMOPlayerCamera* PCCam = Cast<AMMOPlayerCamera>(PlayerCamera);
	if(!PCCam)
	{
		return;
	}

	StartTrace = MouseProjectionOrigin;//PCCam->CameraCache.POV.Location;// PCCam->ViewTarget.POV.Location + WorldDir * 10.0f;
	EndTrace = StartTrace + WorldDir * 50000.0f;	// sometimes the obj is far away more than 5000, modify it to 50000

}

void AMMOPlayerController::NativeGetPickPoint()
{
	NativeGeneratePickRay();
	SetTraceActor(PickPoint(StartTrace, EndTrace));
}

void AMMOPlayerController::NativeGetPickPointActorPreferred()
{
	NativeGeneratePickRay();
	SetTraceActor(PickPointActorPreferred(StartTrace, EndTrace));
}

UBOOL AMMOPlayerController::NativeGetPickPointNavMesh()
{
	NativeGeneratePickRay();
	return PickNavMesh(StartTrace, EndTrace);
}


UBOOL AMMOPlayerController::CanUseAnyAbility(class UMMOAbility* InAbility)
{

	if( InAbility==NULL )
	{
		return FALSE;
	}
	
	if( InAbility->bCoolDown == TRUE )//&& InAbility->ClientCommonAttribute.ModifyCD <0.001f)
	{
		return FALSE; 
	}

	if( bIsOperatingDoodad )
	{
		return FALSE;
	}

	//if( InAbility->bCoolDown == TRUE && InAbility->ClientCommonAttribute.ModifyCD >0.0f &&  InAbility->SpellCommonAttribute.CancelType == E_spclt_None )
	//{
	//	return FALSE;
	//}

	//if(FSMIO::getSingleton().Target. ::Ability.UsableAbility && GetPlayer()->IsUsingAbility() &&  InAbility->SpellCommonAttribute.CancelType == E_spclt_DependOnValue && InAbility->SpellCommonAttribute.CancelLevel<=FSM::Ability.UsableAbility->SpellCommonAttribute.CancelLevel )
	//{
	//	return FALSE;
	//}
	
	//if( InAbility->bCoolDown == TRUE && InAbility->ClientCommonAttribute.ModifyCD >0.0f &&  InAbility->SpellCommonAttribute.CancelType == E_spclt_DependOnValue && InAbility->SpellCommonAttribute.Level >= InAbility->SpellCommonAttribute.CancelLevel)
	//{
	//	return FALSE;
	//}


	if(FSMIO::getSingleton().Ability.CanUseAnyAbility(InAbility)==FALSE)
	{
		return FALSE;
	}

	if( GetPlayer() && GetPlayer()->CanUseAnyAbility()==FALSE )
	{
		return FALSE;
	}

	//if( GetPlayer()->CanMove() ==FALSE)
	//{
	//	return FALSE;
	//}
	if( WaitingForTheAbilityBuf>0.0 )
	{
		return FALSE;
	}

	if( WaitingForTheAbilityRespond>0)
	{
		return FALSE;
	}

	if( WaitingForTheCombatPose>0.0)
	{
		return FALSE;
	}

	if( WaitingForEquipMount>0.0)
	{
		return FALSE;
	}

	return TRUE;
}

//===========================================================================
void AMMOPlayerController::NativeAddCmdUseAbility(UMMOAbility *InAbility, AActor* target/* = NULL*/, FVector Point/*=FVector(EC_EventParm)*/, UBOOL bPopFrontCmd/*=TRUE*/, INT TemplateID/*=-1*/ )
{
	if(InAbility == NULL )
	{
		warnf( NAME_Log, TEXT("can not find ability, or the ability is cooldown") );
		return;
	}

	AMMOActor *curActor = Cast<AMMOActor>(Pawn);
	if( curActor == NULL )
	{
		warnf( NAME_Error, TEXT("AMMOPlayerController::AddCmdUseAbility Pawn is NULL") );
		return;
	}

	if( curActor->CanUseAnyAbility() == false )
	{
		//上一个动作还未结束
		warnf( NAME_Log, TEXT("can not use ability ,return!") );
		return;
	}

	if( WaitingForTheCombatPose > 0 )
	{
		warnf( NAME_Log, TEXT("Waiting For The CombatPose ,return!") );
		return;
	}

	if( WaitingForEquipMount > 0 )
	{
		warnf( NAME_Log, TEXT("Waiting For The EquipMount ,return!") );
		return;
	}

	if( InAbility->bCoolDown == TRUE && InAbility->ClientCommonAttribute.ModifyCD < 0.01)
	{
		FString localMsg = TEXT("MMOErrorInfo.ERROR_CODE_DEF.errSpellNotReady");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( localMsg ), MsgInfoType_Table, localMsg );
		return;
	}

	//蓝不够
	UBOOL isManaOK = InAbility->CanUseManaCheck( curActor );
	if( isManaOK == FALSE )
	{
		FString localMsg = TEXT("MMOErrorInfo.ERROR_CODE_DEF.errSpellOutOfMana");
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath( localMsg ), MsgInfoType_Table, localMsg );
		return;
	}

	UBOOL bWaitingAction = GetWaitingAction();
	if(bWaitingAction)
	{
		UPlayerActionEx_Waiting *waitingAction = ConstructObject<UPlayerActionEx_Waiting>( UPlayerActionEx_Waiting::StaticClass() );
		if( waitingAction != NULL )
		{
			waitingAction->ActionOwner = this;
			waitingAction->LiftSpan = 5.0;
			waitingAction->bNeedReplaceTheFrontAction = bPopFrontCmd;
			PlayerActionQueue->Add( waitingAction );
		}
	}

	bRequestedAbility = true;

	UPlayerActionEx_UseAbility *useAbilityAction = ConstructObject<UPlayerActionEx_UseAbility>( UPlayerActionEx_UseAbility::StaticClass() );
	if( useAbilityAction != NULL )
	{
		useAbilityAction->bCancelAbility = false;
		useAbilityAction->ActionOwner = this;
		useAbilityAction->TemplateID = TemplateID;
		useAbilityAction->AbilityToUse = InAbility;
		useAbilityAction->TargetLocation = Point;//MouseHitWorldLocation;
		useAbilityAction->FixedLocation = Point;//GetFixedMouseHitWorldLocation();
		useAbilityAction->Target = Cast< AMMOActor >( target );
		useAbilityAction->bPrimAttackDynamicAbility = FALSE;

		//如果有target目标，则用目标的坐标作为target坐标，因为延时命令执行时，target目标会在移动，一般锁定点技能，target是为null jimzhang@snda.com
		if(useAbilityAction->Target != NULL )
		{
			useAbilityAction->TargetLocation = target->Location;
			useAbilityAction->FixedLocation  = target->Location;
		}

		if(bWaitingAction){
			useAbilityAction->bNeedReplaceTheFrontAction = false;
		}
		else{
			useAbilityAction->bNeedReplaceTheFrontAction = bPopFrontCmd;
		}

		if(!bPopFrontCmd)
		{
			useAbilityAction->LiftSpan = 5.f;
		}
		PlayerActionQueue->Add( useAbilityAction );
	}
}

//=====================================================================================================
INT AMMOPlayerController::GetCmdNum(BYTE Type)
{
	if(PlayerActionQueue)
	{
		return PlayerActionQueue->GetCmdNum(Type);
	}
	return 0;
}

void AMMOPlayerController::UseAbilityById( int InAbilityId, class AActor* Target, UBOOL bRelease, UBOOL bMouseLeftBtn)
{
    AMMOPlayer*		player		= Cast<AMMOPlayer>(Pawn);
    if(player)
    {
        UMMOAbility*	ability = player->GetEquipedAbilityById( InAbilityId ); 
        if( NULL != ability )
        {
            PressAbilityKey( ability, Target, bRelease, bMouseLeftBtn );
        }
    }
}

void AMMOPlayerController::PressAbilityKey(class UMMOAbility* InAbility,class AActor* Target, UBOOL bRelease, UBOOL bMouseLeftBtn)
{
	if(GIsEditor)
	{
		return;
	}

	if(bRelease==FALSE)
	{
		Mir2Attack::getSingleton().m_TryToMine = FALSE;
		if(bMouseLeftBtn==FALSE){
			FSMIO::getSingleton().Target.SetCacheAbility(InAbility);
		}
		else
		{
			FSMIO::getSingleton().Target.SetCacheAbility(NULL);
		}
		FSMIO::getSingleton().Target.SetKeepRunning(FALSE);
	}

	//开关类技能执行
	if(InAbility && InAbility->SpellCommonAttribute.CastType==E_spct_Switch)
	{
		if(bRelease==FALSE)
		{
			if(InAbility->bCoolDown)
			{
				UBOOL   Res=TRUE;
				FString cs=FString::Printf(TEXT("MMOGame.Ability.InCD_%d_%d"),InAbility->SpellCommonAttribute.Id,InAbility->SpellCommonAttribute.RuneSlotID );
				FString screenMsg = MMOUI::ParseLocalizedPropertyPath(cs, Res);
				if(Res){
					MMOUI::GetUIManager()->delegateAddScreenMessage( screenMsg , MsgInfoType_Warning , TEXT("MMOGame.Ability.InCD") );
					MMOChatManager::getSingleton().AddSystemMessage(screenMsg);
				}
				else
				{
					//FString screenMsg = MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.Ability.InCD_Default"));
					//MMOUI::GetUIManager()->delegateAddScreenMessage( screenMsg , MsgInfoType_Warning , TEXT("MMOGame.Ability.InCD") );
					//MMOChatManager::getSingleton().AddSystemMessage(screenMsg);
				}
			}

			if(InAbility->bCoolDown==FALSE||InAbility->ClientCommonAttribute.ModifyCD>0.001f)
			{
				RequestSwitchSpell(InAbility->SpellCommonAttribute.Id);
			}
		}
		else
		{
			FSMIO::getSingleton().Ctrl.AbilityKeyUp();
		}
		return;
	}

	//非开关类技能
	if(bRelease)
	{
		FSMIO::getSingleton().Ctrl.AbilityKeyUp();
	}

	// check whether need to switch input state
	if (!bRelease)
	{
		if (InAbility && InAbility->ClientCommonAttribute.TargetSelectType != TST_Default)
		{
			if (InAbility->CanUseSelfCheck(Cast<AMMOActor>(Pawn), TRUE))
			{
				SetGameKeyEventProcessState(FString(TEXT("KeyEventProcessStateAbilityTargetSelect")), InAbility);
			}
			else
			{
				//这个错误提示不用提示，所以杠掉..... 
				//FString enumVal(g_szErrorCodeNameList[errSpellConditionError]);
				//FString strString = FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal);
				//MMOUI::GetUIManager()->delegateAddScreenMessage(FString::Printf(TEXT("MMOErrorInfo.ERROR_CODE_DEF.%s"), *enumVal),MsgInfoType_Table,strString);
			}
			return;
		}
	}

	INT Index = 0;
	AMMOActor* pMMOActor = Cast<AMMOActor>(Target);
	if(pMMOActor)
	{
		Index = pMMOActor->GetIndex();// >ServerID;
	}

	if(bRelease==TRUE)
	{
		Index = 0;
	}
	//MMOFacade::getSingleton().GetMainControllerAI()->StartCmdAbility(InAbility,Index,bRelease,bMouseLeftBtn);
//	if(bRelease)	MMOTargetManager::getSingleton().ClearDesireTarget(InAbility);
//	else	MMOTargetManager::getSingleton().SetDesireTarget(InAbility, Target,bMouseLeftBtn);

	if(bRelease)
	{
		UMMOAbility* pDesireAbility = FSMIO::getSingleton().Target.GetDesireAbility();
		if(pDesireAbility && InAbility && pDesireAbility->SpellCommonAttribute.Id==InAbility->SpellCommonAttribute.Id)
		{
			FSMIO::getSingleton().Target.ClearDesireTarget(pDesireAbility);
		}

		//if(FSMIO::getSingleton().Target.GetDesireAbility() == InAbility && InAbility!=NULL){
		//	FSMIO::getSingleton().Target.ClearDesireTarget(InAbility);
		//}
	}
	else	
	{	
		if(InAbility){
			FSMIO::getSingleton().Target.SetDesireTarget(InAbility,NULL);
		}
	}

	if(!bRelease)
	{
		FSMIO::getSingleton().Ctrl.AbilityKeyDown();
	}
}

void AMMOPlayerController::PressMovingKey(class UMMOAbility* InAbility,class AActor* Target, UBOOL bRelease, UBOOL bMouseLeftBtn)
{
	if(GIsEditor)
	{
		return;
	}

	INT Index = 0;
	AMMOActor* pMMOActor = Cast<AMMOActor>(Target);
	if(pMMOActor)
	{
		Index = pMMOActor->GetIndex();// >ServerID;
	}

	if(bRelease==TRUE)
	{
		Index = 0;
	}

	//MMOTargetManager::getSingleton().SetDesireTarget(NULL,NULL);
	FSMIO::getSingleton().Target.SetDesireTarget(NULL,NULL);
	//MMOFacade::getSingleton().GetMainControllerAI()->StartCmdAbility(InAbility,Index,bRelease,bMouseLeftBtn,TRUE);
}

void AMMOPlayerController::Send_CombatPoseOn(UBOOL bOn ,UINT8 byType)
{
	WaitingForTheCombatPose = 0.5f;
	CMessageHandler::GetInstance().Send_CombatPoseOn(bOn,byType);
}

UBOOL AMMOPlayerController::GetWaitingAction()
{
	UBOOL bRes = FALSE;
	AMMOPlayer* player = Cast<AMMOPlayer>(Pawn);
	if(player && player->ActorAttributes){
		if(player->bHoldWeapon==FALSE)
		{
			if(player->ActorAttributes->byCombatPoseOn == TRUE ){
				Send_CombatPoseOn(FALSE,0);
				bRes = TRUE;
			}
		}
		if(player->ActorAttributes->MountID>0)
		{
			CMessageHandler::GetInstance().Send_EquipMount(0);
			bRes = TRUE;
		}
	}
	return bRes;
}

//Bruce@snda 
void AMMOPlayerController::AddAllAnimAndTime( AMMOActor* PNActor )
{
	for( INT WeaponIdx = 0; WeaponIdx < PNActor->UseingAvatar->WeaponAnimSetPairs.Num(); ++WeaponIdx )
	{
		for( INT Idx = 0; Idx < PNActor->UseingAvatar->WeaponAnimSetPairs(WeaponIdx).AnimSets.Num(); ++Idx )
		{
			for (INT i = 0; i < PNActor->UseingAvatar->WeaponAnimSetPairs(WeaponIdx).AnimSets(Idx)->Sequences.Num(); ++i)
			{
				FString str = PNActor->UseingAvatar->WeaponAnimSetPairs(WeaponIdx).AnimSets(Idx)->Sequences(i)->SequenceName.ToString();
				FLOAT LenES = PNActor->UseingAvatar->WeaponAnimSetPairs(WeaponIdx).AnimSets(Idx)->Sequences(i)->SequenceLength;
				AllAnimArr.AddItem( *str );
				AllDurTimeES.AddItem( LenES );
			}
		}		
	}
}

void AMMOPlayerController::PlayAllAnimByNPCID( INT npcID )
{
	nPlayType = 1;

	AllNPCActors.Empty();
	AllAnimArr.Empty();
	AllDurTimeES.Empty();

	for ( TObjectIterator<ANPC> it; it; ++it )
	{
		ANPC* npc = *it;
		if ( npc && npc->Template )
		{
			UNPCTemplate* npcTemp = npc->Template;
			if ( npcTemp && npcTemp->GetTemplateID() == npcID )
			{
				AllNPCActors.AddItem( Cast<AMMOActor>( npc ) );
			}
		}
	}

	if ( AllNPCActors.Num() && AllNPCActors(0) )
	{
		AddAllAnimAndTime( AllNPCActors(0) );

		if ( AllAnimArr.Num() )
		{
			for ( INT WIdx = 0; WIdx < AllNPCActors.Num(); ++WIdx )
			{
				AllNPCActors( WIdx )->PlayAnimation( FName( *AllAnimArr(0) ), 0, 1.0, FALSE );
			}
		}
	}
}

void AMMOPlayerController::PlayAllAnimByNPCIDTick( FLOAT delTime )
{
	static FLOAT NPCTotalSeqTime = 0.0f;
	static INT NPCSeq = 0;	

	if ( AllAnimArr.Num() && AllDurTimeES.Num() )
	{
		NPCTotalSeqTime += delTime;

		if ( NPCTotalSeqTime > ( AllDurTimeES( NPCSeq ) + 0.5f ) && NPCSeq < ( AllAnimArr.Num() - 1 ) )
		{
			++NPCSeq;

			for ( INT WIdx = 0; WIdx < AllNPCActors.Num(); ++WIdx )
			{
				AllNPCActors( WIdx )->PlayAnimation( FName( *AllAnimArr( NPCSeq ) ), 0, 1.0, FALSE );
			}

			NPCTotalSeqTime = 0.0f;
		}

		if( NPCTotalSeqTime > ( AllDurTimeES( NPCSeq ) + 0.5f ) && NPCSeq == ( AllAnimArr.Num() - 1 ) )
		{
			NPCSeq = 0;

			for ( INT WIdx = 0; WIdx < AllNPCActors.Num(); ++WIdx )
			{
				AllNPCActors( WIdx )->PlayAnimation( FName( *AllAnimArr( NPCSeq ) ), 0, 1.0, FALSE );
			}
		}					
	}
}

void AMMOPlayerController::PlayNPCAnimLoop( INT npcID, INT SeqLoop )
{
	nPlayType = 3;

	GlobalSeqLoop = SeqLoop - 1;

	AllNPCActors.Empty();
	AllAnimArr.Empty();
	AllDurTimeES.Empty();

	for ( TObjectIterator<ANPC> it; it; ++it )
	{
		ANPC* npc = *it;
		if ( npc && npc->Template )
		{
			UNPCTemplate* npcTemp = npc->Template;
			if(npcTemp && npcTemp->GetTemplateID() == npcID)
			{
				AllNPCActors.AddItem( Cast<AMMOActor>( npc ) );
			}
		}
	}

	if ( AllNPCActors.Num() && AllNPCActors(0) )
	{
		AddAllAnimAndTime( AllNPCActors(0) );
	}

	if ( GlobalSeqLoop < 0 || GlobalSeqLoop >= AllAnimArr.Num() )
	{
		return;
	}

	for ( INT WIdx = 0; WIdx < AllNPCActors.Num(); ++WIdx )
	{
		AllNPCActors( WIdx )->PlayAnimation( FName( *AllAnimArr( GlobalSeqLoop ) ), 0, 1.0, FALSE );
	}
}

void AMMOPlayerController::PlayNPCAnimLoopTick( FLOAT delTime )
{
	static FLOAT NPCTotalSeqTime = 0.0f;

	if( !AllAnimArr.Num() || !AllDurTimeES.Num() || !AllNPCActors.Num() )
	{
		return;
	}

	if ( GlobalSeqLoop < 0 || GlobalSeqLoop >= AllAnimArr.Num() )
	{
		return;
	}

	NPCTotalSeqTime += delTime;

	if ( NPCTotalSeqTime > ( AllDurTimeES( GlobalSeqLoop ) + 0.1f ) )
	{
		for( INT WIdx = 0; WIdx < AllNPCActors.Num(); ++WIdx )
		{
			AllNPCActors( WIdx )->PlayAnimation( FName( *AllAnimArr( GlobalSeqLoop ) ), 0, 1.0, FALSE );
		}

		NPCTotalSeqTime = 0.0f;
	}
}

void AMMOPlayerController::PlayPlayerAnimLoop( INT SeqLoop )
{
	nPlayType = 4;

	GlobalSeqLoop = SeqLoop - 1;

	AllAnimArr.Empty();
	AllDurTimeES.Empty();
	AllNPCActors.Empty();

	AllNPCActors.AddItem( MMOFacade::getSingleton().GetMainPlayer() );

	if ( AllNPCActors(0) )
	{
		AddAllAnimAndTime( AllNPCActors(0) );
	}

	if ( GlobalSeqLoop >= AllAnimArr.Num() || GlobalSeqLoop < 0 )
	{
		return;
	}	

	AllNPCActors(0)->PlayAnimation( FName( *AllAnimArr( GlobalSeqLoop ) ), 0, 1.0, FALSE );
}

void AMMOPlayerController::PlayPlayerAnimLoopTick( FLOAT delTime )
{
	static FLOAT NPCTotalSeqTime = 0.0f;

	if ( !AllAnimArr.Num() || !AllDurTimeES.Num() )
	{
		return;
	}

	if ( GlobalSeqLoop >= AllAnimArr.Num() || GlobalSeqLoop < 0 )
	{
		return;
	}	

	NPCTotalSeqTime += delTime;		

	if ( NPCTotalSeqTime > AllDurTimeES( GlobalSeqLoop ) + 0.1f )
	{
		AllNPCActors(0)->PlayAnimation( FName( *AllAnimArr( GlobalSeqLoop ) ), 0, 1.0, FALSE );
		NPCTotalSeqTime = 0.0f;
	}
}

void AMMOPlayerController::PlayMPlayerAnim()
{
	nPlayType = 2;

	AllAnimArr.Empty();
	AllDurTimeES.Empty();
	AllNPCActors.Empty();

	AllNPCActors.AddItem( MMOFacade::getSingleton().GetMainPlayer() );

	if ( AllNPCActors(0) )
	{
		AddAllAnimAndTime( AllNPCActors(0) );

		if ( AllAnimArr.Num() )
		{
			AllNPCActors(0)->PlayAnimation( FName( *AllAnimArr(0) ), 0, 1.0, FALSE );
		}
	}
}

void AMMOPlayerController::PlayMPlayerAnimTick( FLOAT delTime )
{
	static INT MPlayerSeq = 0;
	static FLOAT MPlayerTotalSeqTime = 0.0f;

	if ( AllAnimArr.Num() && AllDurTimeES.Num() )
	{
		MPlayerTotalSeqTime += delTime;		

		if ( MPlayerTotalSeqTime > ( AllDurTimeES( MPlayerSeq ) + 0.5f ) && MPlayerSeq < ( AllAnimArr.Num() - 1 ) )
		{
			AllNPCActors(0)->PlayAnimation( FName( *AllAnimArr( ++MPlayerSeq ) ), 0, 1.0, FALSE );
			MPlayerTotalSeqTime = 0.0f;
		}

		if ( MPlayerTotalSeqTime > ( AllDurTimeES( MPlayerSeq ) + 0.5f ) && MPlayerSeq == ( AllAnimArr.Num() - 1 ) )
		{
			MPlayerSeq = 0;
			AllNPCActors(0)->PlayAnimation( FName( *AllAnimArr( MPlayerSeq ) ), 0, 1.0, FALSE );
		}					
	}
}

void AMMOPlayerController::StopPlayPNAnim()
{
	nPlayType = 0;
}
//Bruce@snda 
extern bool g_needUpdateServerVars;
void AMMOPlayerController::NativePlayerTick( float DeltaTime )
{
	if(_bReturnLogin)
	{
		_bReturnLogin = FALSE;
		g_disableAllInput = false;
		OnLeaveScene();
		ReturnLogin();
		return;
	}


	//NativePickTick(DeltaTime);
	
	if(g_needUpdateServerVars && GWorld->GetGameSequence())
	{
		USequence* rootSeq = GWorld->GetGameSequence();
		TArray<USeqEvent_Server2Client_VarChange_Scene*> varEvents;
		rootSeq->FindSeqObjectsByClass(USeqEvent_Server2Client_VarChange_Scene::StaticClass(), (TArray<USequenceObject*>&)varEvents);

// 		SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_BLUE);
// 		warnf(NAME_Log, TEXT("USeqEvent_Server2Client_VarChange_Scene:%d"), varEvents.Num());
// 		CLEAR_WARN_COLOR();

		if(varEvents.Num() > 0)
		{
			 MMO::UpdateAllServerSceneVars();
		}
	}


	if(GIsEditor)
	{
		//Bruce@snda
		if ( bLeftMousePressed )
		{
			NativeGetPickPoint();

			if(GIsEditor)
			{
				if(Pawn)
				{
					Pawn->rotateToward( MouseHitWorldLocation );
					Cast<AMMOActor>( Pawn )->eventScriptMoveTo( MouseHitWorldLocation , 580.f);
				}
				return;
			}
		}

		switch ( nPlayType )
		{
		case 0:
			break;
		case 1:
			PlayAllAnimByNPCIDTick( DeltaTime );
			break;
		case 2:
			PlayMPlayerAnimTick( DeltaTime );
			break;
		case 3:
			PlayNPCAnimLoopTick( DeltaTime );
			break;
		case 4:
			PlayPlayerAnimLoopTick( DeltaTime );
			break;
		}
		//Bruce@snda 
		return;
	}



	if(MMO::GetCurrentGameState() == EGameState_InGame)
	{
		if (WorldInfo->TimeSeconds - LastUpdateFPSTime > 30.f)
		{
			CMessageHandler::GetInstance().Send_FPS();
			LastUpdateFPSTime = WorldInfo->TimeSeconds;
		}
		FSMCtrlManager::getSingleton().Update(DeltaTime);
		Mir2Attack::getSingleton().Update(DeltaTime);
		PlayerActionQueue->Update(DeltaTime);
	}
}

void AMMOPlayerController::UpdateGridFlag()
{
	if (!bShowGrid)
	{
		return;
	}
	AActor* CurrentTraceActor = GetTraceActor();
	if (!CurrentTraceActor)
	{
		return;
	}


	if (CurrentTraceActor->IsA(ALandscapeProxy::StaticClass()))
	{
	}
	else if (CurrentTraceActor->IsA(AStaticMeshActor::StaticClass()) && CurrentTraceActor->CollisionType == COLLIDE_BlockAll)
	{

	}
	else
	{
		return;
	}


	//FVector PointList[4];
	//gServerGridWorld.GetGridCorner(PointList, MouseHitWorldLocation);
	//PointList[0].Z = PointList[1].Z = PointList[2].Z = PointList[3].Z = MouseHitWorldLocation.Z + 1;
	//drawdeb

	ZONE_DATA* pDestZone = NULL;
	GRID_DATA* pDestGrid = NULL;
	if (gServerGridWorld.GetZoneAndGrid(MouseHitWorldLocation, pDestZone, pDestGrid))
	{
		//FVector CenterPos = MouseHitWorldLocation;
		//gServerGridWorld.GetGridCenter(CenterPos, MouseHitWorldLocation);

		FVector PointList[4];
		gServerGridWorld.GetGridCorner(PointList, MouseHitWorldLocation);
		PointList[0].Z = PointList[1].Z = PointList[2].Z = PointList[3].Z = MouseHitWorldLocation.Z + 20;

		BYTE R = 255, G = 255, B = 255;
		if (pDestGrid->bOutOfPoly == 0)
		{
			R = 0;
			G = 255;
			B = 0;
		}
		else
		{
			R = 255;
			G = 0;
			B = 0;
		}

		this->FlushPersistentDebugLines();
		DrawDebugLine(PointList[0], PointList[1], R, G, B, TRUE);
		DrawDebugLine(PointList[1], PointList[2], R, G, B, TRUE);
		DrawDebugLine(PointList[2], PointList[3], R, G, B, TRUE);
		DrawDebugLine(PointList[3], PointList[0], R, G, B, TRUE);
	}







}


class AActor* AMMOPlayerController::GetTraceActor() const
{
	return TraceActor;
}

void AMMOPlayerController::SetTraceActor(class AActor* inTraceActor)
{
	if(g_disableAllInput)
	{
		return;
	}

	if(TraceActor==inTraceActor)
	{
		return;
	}

	typedef TScriptInterface<IIInteractableActor> IInteractableActor;

	if(TraceActor)
	{

		IInteractableActor intActor = ScriptInterfaceCast<IIInteractableActor>(TraceActor);
		if(intActor)
		{
			intActor->SetSilhouette(false);
			//warnf( NAME_Error, TEXT("---------SetTraceActor 0") );
		}
	}

	TraceActor = inTraceActor;


	if(TraceActor)
	{
		IInteractableActor intActor = ScriptInterfaceCast<IIInteractableActor>(TraceActor);
		if(intActor)
		{
			if( TraceActor->IsA(AMMOActor::StaticClass()))
			{
				if( TraceActor == Pawn )		
				{
					Cast<AMMOActor>(TraceActor)->SetSilhouetteEx( TRUE, FVector( 0, 1, 0 ) );
					//warnf( NAME_Error, TEXT("---------SetSilhouetteEx 1") );
				}
				else 
				{
					Cast<AMMOActor>(TraceActor)->SetSilhouetteEx( TRUE, FVector( 1, 0, 0 ) );
					//warnf( NAME_Error, TEXT("---------SetSilhouetteEx 2") );
				}
			}
			else
			{
				IInteractableActor intActor = ScriptInterfaceCast<IIInteractableActor>(TraceActor);
				if(intActor)
				{
					intActor->SetSilhouette(true);
					//warnf( NAME_Error, TEXT("---------SetSilhouetteEx 3") );
				}
			}
		}
	}




//
//
//
//	typedef TScriptInterface<IIInteractableActor> IInteractableActor;
//	if(g_disableAllInput)
//	{
//		return;
//	}
//
//	if(TraceActor)
//	{
//#if 0
//		if(TraceActor->IsA(ANPC::StaticClass()))
//		{
//			ANPC* npc = Cast<ANPC>(TraceActor);
//			SET_WARN_COLOR_AND_BACKGROUND(COLOR_WHITE, COLOR_DARK_BLUE);
//			warnf(NAME_Log, TEXT("SetTraceActor:%s %s"), *npc->GetName(), *npc->Template->GetDisplayName());
//			CLEAR_WARN_COLOR();
//		}
//#endif
//
//		IInteractableActor intActor = ScriptInterfaceCast<IIInteractableActor>(TraceActor);
//		if(intActor)
//		{
//			//if(intActor->CanInteract())
//			{
//				intActor->SetSilhouette(false);
//			}
//		}
//
//// 		else if( TraceActor->IsA(AMMOActor::StaticClass()) )
//// 		{
//// 			Cast<AMMOActor>(TraceActor)->SetSilhouette( FALSE );
//// 		}
//// 		else if( TraceActor->IsA(AMMODropActor::StaticClass()) )		
//// 		{
//// 			//Cast<ADropItemActor>(TraceActor)->SetSilhouette( FALSE );			
//// 			AMMODropActor* dropActor = Cast<AMMODropActor>(TraceActor);
//// 			dropActor->Mesh->ShowSilhouette = FALSE;
//// 		}
//// 		else if( TraceActor->IsA(AMMODoodadActor::StaticClass()) )		
//// 		{
//// 			//Cast<ADropItemActor>(TraceActor)->SetSilhouette( FALSE );	
//// 			AMMODoodadActor* doodadActor = Cast<AMMODoodadActor>(TraceActor);
//// 			if(doodadActor->CanInteract())
//// 			{
//// 				doodadActor->SetSilhouette(false);
//// 			}
//// 		}
//// 		else if( TraceActor->IsA(ATeleportOrig::StaticClass()) )		
//// 		{
//// 			//Cast<ADropItemActor>(TraceActor)->SetSilhouette( FALSE );	
//// 			ATeleportOrig* teleportActor = Cast<ATeleportOrig>(TraceActor);
//// 			teleportActor->SetSilhouette(false);
//// 		}
//// 		else if( TraceActor->IsA(ATeleportTarget::StaticClass()) )		
//// 		{
//// 			//Cast<ADropItemActor>(TraceActor)->SetSilhouette( FALSE );	
//// 			ATeleportTarget* teleportActor = Cast<ATeleportTarget>(TraceActor);
//// 			teleportActor->SetSilhouette(false);
//// 		}
//	}
//
//	TraceActor = inTraceActor;
//
//	// draw silhouette
//	//`log("________ TraceActor" @IsoPlayerController.TraceActor);
//
//	if(TraceActor)
//	{
//		// temp color
//		if( TraceActor->IsA(AMMOActor::StaticClass()))
//		{
//			if( TraceActor == Pawn )		
//			{
//				Cast<AMMOActor>(TraceActor)->SetSilhouetteEx( TRUE, FVector( 0, 1, 0 ) );
//			}
//			else 
//			{
//				Cast<AMMOActor>(TraceActor)->SetSilhouetteEx( TRUE, FVector( 1, 0, 0 ) );
//			}
//		}
//		else
//		{
//			IInteractableActor intActor = ScriptInterfaceCast<IIInteractableActor>(TraceActor);
//			if(intActor)
//			{
//				//if(intActor->CanInteract())
//				{
//					intActor->SetSilhouette(true);
//				}
//			}
//		}
//		//else if( TraceActor->IsA(AMMODropActor::StaticClass()) )
//		//{
//		//	//黄色宝藏
//		//	AMMODropActor* dropActor = Cast<AMMODropActor>(TraceActor);
//		//	dropActor->Mesh->ShowSilhouette = TRUE;
//		//	dropActor->Mesh->SilhouetteColor = FVector( 1, 1, 0 );
//		//}
//		//else if( TraceActor->IsA(AMMODoodadActor::StaticClass()) )
//		//{
//		//	//黄色宝藏
//		//	AMMODoodadActor* doodadActor = Cast<AMMODoodadActor>(TraceActor);
//		//	if(doodadActor->CanInteract())
//		//	{
//		//		doodadActor->SetSilhouette(true);
//		//	}
//		//}
//		//else if( TraceActor->IsA(ATeleportOrig::StaticClass()) )		
//		//{
//		//	ATeleportOrig* teleportActor = Cast<ATeleportOrig>(TraceActor);
//		//	teleportActor->SetSilhouette(true);
//		//}
//		//else if( TraceActor->IsA(ATeleportTarget::StaticClass()) )		
//		//{
//		//	ATeleportTarget* teleportActor = Cast<ATeleportTarget>(TraceActor);
//		//	teleportActor->SetSilhouette(true);
//		//}
//	}
}

////用于Trace到非Actor，如UI等元素，但实现和Trace到actor一样结果的方法 
//void AMMOPlayerController::SetDesiredTraceActor(AActor* tActor )
//{
//	SetTraceActor(tActor);
//	if( TraceActor->IsA( ADropItemActor::StaticClass()) )
//	{
//		PickItemActor = Cast<ADropItemActor>( TraceActor ) ;
//		eventAddCmdMoveToActor( TraceActor );
//	}
//	else if( TraceActor->IsA( ANPC::StaticClass()) )
//	{
//		ANPC* desiredTarget = Cast<ANPC>( TraceActor );
//		MMO::TargetManager::getSingleton().SetDesiredTarget(desiredTarget, TargetManager::GetDefaultInteractRange());
//		if(!MMO::TargetManager::IsTargetInPlayerInteractRange(desiredTarget, TargetManager::GetDefaultInteractRange()))
//		{
//			eventAddCmdMoveToActor( desiredTarget );
//		}
//	}
//	else if( TraceActor->IsA( AMMOActor::StaticClass()) )
//	{ 
//		SelectedActor = Cast<AMMOActor>( TraceActor ) ;
//		MMO::TargetManager::getSingleton().SetDesiredTarget(SelectedActor, TargetManager::GetDefaultInteractRange());
//		if(!MMO::TargetManager::IsTargetInPlayerInteractRange(SelectedActor, TargetManager::GetDefaultInteractRange()))
//		{
//			eventAddCmdMoveToActor( SelectedActor );
//		}
//	}
//
//	//NativeOnLeftMouseKey( FALSE );
//}

void AMMOPlayerController::NativeOnRightMouseKey(UBOOL bRelease)
{
	// 点击到游戏中，即所有界面不响应鼠标右键，发出失去焦点，但其任然是焦点UI，zhaojun@snda-----------------------------------------------------------
	if (FGFxEngine::GetEngine()->GetFocusMovie(0) && FGFxEngine::GetEngine()->GetFocusMovie(0)->pUMovie)
	{
		if (InTextInput)
		{
			//UMMOUIBase* PanelChat = MMOUI::FindUIPanel(  TEXT("Chat"));
			UMMOUIChat* Chat = Cast<UMMOUIChat>(MMOUI::FindUIPanel(TEXT("Chat")));
			
			if(Chat)// && FGFxEngine::GetEngine()->GetFocusMovie(0)==Chat->GFXPlayer->pMovie )
			{
				Chat->ShowInputArea(FALSE);
				//MMOUI::RemoveTop(Chat); 
			}
			//FGFxEngine::GetEngine()->GetFocusMovie(0)->pUMovie->eventOnFocusLost(0);

			if(g_rootGFXPlayer)
			{
				g_rootGFXPlayer->eventUCClearFocus();
			}
		}
	}

	FGFxEngine::GetEngine()->PlayerControlRequireReleaseEvt = !bRelease;

	//---------------------------------------------------------------------------------------------------------------------------------------------------
			
	int Ret;
	//UIUICommand* cmd;
	UMMOAbility* ToUseAbility=NULL;
	//AActor*      TracePawnActor;
	Ret = eventCanProcessMouseKeyEvent(false, bRelease);
	if(Ret <= 0 )
	{
		return;
	}

	ToUseAbility = FSMIO::getSingleton().Ctrl.GetRightBtnBindAbility(bRelease);

	if(bRelease==false)
	{
		//右键关闭UI，鉴定，分解，等的特殊处理
		MMOUI::GetUIManager()->ClearCurrentItemSlotActionListener();
	}
	SetTraceActor(PickPointActorPreferred(StartTrace, EndTrace));

	if(bRelease)//松开
	{
		bRightMousePressed = !bRelease;
		PressAbilityKey(ToUseAbility,NULL, bRelease,FALSE);
	}
	else
	{
		bRightMousePressed = !bRelease;
		FSMIO::getSingleton().Ctrl.SetKeyType(KT_SECONDARY);
		PressAbilityKey(ToUseAbility,TraceActor, bRelease,FALSE);
	}
}

//
UBOOL AMMOPlayerController::IsTransformed()
{
	return IsInTransform;
}

void AMMOPlayerController::SetTransformState( UBOOL isTransformed )
{
	IsInTransform = isTransformed;
}

//确认Slot切换
void AMMOPlayerController::EquipNewSpellAndAutoUnequipOld( int slotId, int abilityId, int abilityLv, int runeSlot, UBOOL isLocked )
{
	MMOSkillManager &mmoSkillMgr = MMOSkillManager::getSingleton();
	if( IsInTransform == TRUE ){
		if( mmoSkillMgr.IsSkillChangeNeedCache( slotId ) ){
			mmoSkillMgr.InTransformStateSkillChanged( slotId, abilityId, abilityLv, runeSlot, isLocked );
			return;
		}
	}

	//
	if( abilityId <= 0 ){
        //空的Slot
		UnEquipSpell( slotId, isLocked );
	}
	else{
		UINT uniqueSkillID = MMOSkillManager::CombinateAbilityID( abilityId, abilityLv, runeSlot );
		UMMOAbility *newAbility = mmoSkillMgr.FindAbilityTemplate( uniqueSkillID );
		if( newAbility == NULL ){
			warnf( NAME_Error, TEXT("AMMOPlayerController::EquipSpell FindAbilityTemplate FAILED At skillID:%d,LV:%d,RuneID:%d"), abilityId, abilityLv, runeSlot );
		}
        
		//先卸下原装配的技能
		UnEquipSpell( slotId, isLocked );

		//装配新技能
		AMMOPlayer* mainPlayer = Cast<AMMOPlayer>( Pawn);
		mainPlayer->EquipSpell( slotId, newAbility );

		//Slot上有技能 则这个Slot肯定Locked为false
        KeyMapManager->BindingSlot( slotId, UICT_UseAbility, abilityId );

		FSMIO::getSingleton().Target.UpdateDesireAbility( slotId, abilityId);

	}	
}

//卸载装配的技能
void AMMOPlayerController::UnEquipSpell( INT slotId, UBOOL isLocked )
{
	AMMOPlayer* mainPlayer = Cast<AMMOPlayer>( Pawn);
	//先卸下原装配的技能
	INT oldEquipedSkillID = KeyMapManager->GetCmdIDBySlotID( slotId );
	UMMOAbility *pEquipedSkill = mainPlayer->GetEquipedAbilityById( oldEquipedSkillID );
	if( pEquipedSkill != NULL ){
		pEquipedSkill->SlotHotKeyID = -1;
	}
	//
	KeyMapManager->UnBindingSlot( slotId, isLocked );
}

//在场景中显示技能范围
void AMMOPlayerController::GuildAbilityInScene( UMMOAbility *toGuildAbility, UBOOL isToShow )
{
	//warnf(NAME_Warning, TEXT("AMMOPlayerController::GuildAbilityInScene %s %d") , *toGuildAbility->eventGetAbilityName(), isToShow);
	AMMOActor* mainActor = Cast<AMMOActor>(Pawn);
	check(mainActor);
	if (isToShow)
	{
		if (toGuildAbility->TipsComponentList.Num() > 0)
		{
			mainActor->AbilityShowEffectAreaIds = mainActor->PlayActions(toGuildAbility->TipsComponentList);
		}
	}
	else
	{
		for (INT i = 0; i < mainActor->AbilityShowEffectAreaIds.Num(); ++i)
		{
			EffectProcessor::GetSingleton().DeleteActiveComponent(mainActor->AbilityShowEffectAreaIds(i));
		}
		mainActor->AbilityShowEffectAreaIds.Empty();
	}

}

//=====================================================================================================================

extern FVector g_BlockPosition;

void AMMOPlayerController::NativeMoveToPoint(FVector Des, UBOOL bRunOrWalk,UBOOL bCrossJudge,FLOAT minRadius)
{
	UBOOL bRes=FALSE;
	UBOOL bOblique=FALSE;
	FVector DesEx = Des;

	if(bRunOrWalk==FALSE)
	{
		//warnf(NAME_Warning, TEXT("---------------NativeMoveToPoint"));
	}

	if(FSMIO::getSingleton().Ctrl.GetAlwaysWalk()||FSMIO::getSingleton().Ctrl.GetAlwaysWalkBase())
	{
		bRunOrWalk = FALSE;
	}

	AMMOActor* mainActor = Cast<AMMOActor>(Pawn);
	if(mainActor==NULL)
	{
		return;
	}

	if (g_bGMBuff)
	{
		TryingMoveDest = Des;
		bRes = TRUE;
	}
	else
	{
		TryingMoveDest = gServerGridWorld.OptimizeDes_Mir2(Pawn->Location, Des, bRes, bRunOrWalk, bOblique, bCrossJudge, minRadius, TRUE);
	}



	if(bRes)
	{
		if(bRunOrWalk==FALSE && FSM::CDTime.WalkDelayTime>0.0f)
		{
			return;
		}

		if(mainActor && mainActor->ResetMovePhaseTime<0.0f)
		{
			if(bRunOrWalk && bOblique){
				mainActor->SetMoveAnimPosition(g_Run_Oblique_Phase[mainActor->GetClassId()]);			
			}
			else if(bRunOrWalk && !bOblique){
				mainActor->SetMoveAnimPosition(g_Run_Phase[mainActor->GetClassId()]);
			}
			else if(!bRunOrWalk && bOblique){
				mainActor->SetMoveAnimPosition(g_Walk_Oblique_Phase[mainActor->GetClassId()]);
			}
			else if(!bRunOrWalk && !bOblique){
				mainActor->SetMoveAnimPosition(g_Walk_Phase[mainActor->GetClassId()]);
			}
		}
		SendRequest_MoveTo(	TryingMoveDest, bRunOrWalk);
		FSMIO::getSingleton().Trigger.SetWaitingSendMove(TRUE);
	}
	else if(MMODropManager::getSingleton().m_CanPickUpDropID>0 && MMODropManager::getSingleton().m_CanPickUpInfo)
	{
		MMODropManager::getSingleton().m_CanPickUpInfo->CannotPickCD = 0.2f;
		FSM::CDTime.MoveDelayTime = 0.3f;
		CMessageHandler::GetInstance().Send_PickDropItem(MMODropManager::getSingleton().m_CanPickUpDropID, 0);
	}
	else
	{
		//无法行走被阻挡，验证动态阻挡的正确性，主要是检查是否有格子上的actor是错误的
		static UBOOL bOpen = TRUE;
		if(bOpen)
		{
			MMOFacade::getSingleton().SetBlockCheck(g_BlockPosition);
		}

		if(MMODropManager::getSingleton().m_CanPickUpDropID==0)
		{
			FVector dir = Des - mainActor->ServerLocation;
			FLOAT   dis2D = dir.Size2D();
			dir.Z = 0.0f;	
			dir = dir.SafeNormal2D();

			if(dis2D<91.0f)
			{
				DesEx = mainActor->ServerLocation + dir*128.0f*1.20f;//系数1.2是为了均分八格子为22.5°
			}
		
			INT MoveDirIndex		= gServerGridWorld.OptimizeDir_Mir2(mainActor->ServerLocation,DesEx);
			INT LastMoveDirIndex	= gServerGridWorld.OptimizeDir_Mir2(mainActor->ServerLocation,LastMoveDest);
			if(MoveDirIndex!=LastMoveDirIndex)
			{

				if(MoveDirIndex>=0)
				{
					INT nYaw = gServerGridWorld.GetYawFromDirIndex((MoveDirIndex+7)%8);
					FRotator Rot = mainActor->Rotation;
					Rot.Yaw = nYaw;
					CMessageHandler::GetInstance().UpdateRotation(Rot,100);				
				}
				else{
					//FRotator Rot = dir.Rotation();
					INT nYaw = gServerGridWorld.GetYawFromDirIndex((MoveDirIndex+7+8)%8);
					FRotator Rot = mainActor->Rotation;
					Rot.Yaw = nYaw;
					CMessageHandler::GetInstance().UpdateRotation(Rot,100);
					//warnf(NAME_Warning, TEXT("---------------UpdateRotation"));
				}
			}

			if(dis2D>96.0f && dis2D<300.0f)
			{		
				Mir2Attack::getSingleton().TryToMine(mainActor->ServerLocation+dir*128.0f);
			}
		}
	}
	LastMoveDest = DesEx;

}

void AMMOPlayerController::SetTargetLocationMarker(FVector Pos, class AMMOActor* TargetActor )
{
	//if(bLeftMousePressed){
		TargetLocationActor = TargetActor;
		TargetLocationPos	= Pos; 
		TargetLocationMarkerCanShow = true;
	//}
}

void AMMOPlayerController::ShowTargetLocationMarker()
{
	// 过场播放时, 禁止显示
	AMMOPlayer* mainPlayer = MMOFacade::getSingleton().GetMainPlayer();
	if (mainPlayer && mainPlayer->IsInMatinee)
	{
		return;
	}

	if(TargetLocationActor==NULL && TargetLocationMarkerCanShow){
		if(TargetLocationMarker ) //&&  Pawn && Pawn->isMoving )
		{
			// navmesh较真实地形有一定的偏差，绘制行走点时pick到真实地形上，zhaojun@snda
			FCheckResult* HitR = MMO::PickPoint(TargetLocationPos + FVector(0,0,100), TargetLocationPos - FVector(0,0,100), this->Pawn);
			if(HitR)
			{
				TargetLocationMarker->SetLocation( HitR->Location );//   MouseHitNavMesh );
				TargetLocationMarker->Show();
			}
		}
	}
	TargetLocationMarkerCanShow = false;
}

//
void AMMOPlayerController::NativeOnLeftMouseKey(UBOOL bRelease)
{
	// 点击到游戏中，即所有界面不响应鼠标左键，发出失去焦点，但其任然是焦点UI，zhaojun@snda-----------------------------------------------------
// 	FGFxMovie* pFocus = FGFxEngine::GetEngine()->GetFocusMovie(0);
// 	if(pFocus && pFocus->pUMovie)
// 	{
// 		if (InTextInput)
// 		{
// 			UMMOUIBase* PanelChat = MMOUI::FindUIPanel(  TEXT("Chat"));
// 			UMMOUIChat* Chat = Cast<UMMOUIChat>(MMOUI::FindUIPanel(TEXT("Chat")));
// 			if(Chat && pFocus==PanelChat->GFXPlayer->pMovie ){
// 				Chat->ShowInputArea(FALSE);
// 				MMOUI::RemoveTop(PanelChat); 
// 			}
// 			pFocus->pUMovie->eventOnFocusLost(0);
// 		}
// 	}

	if (InTextInput)
	{
		UMMOUIChat* PanelChat = Cast<UMMOUIChat>(MMOUI::FindUIPanel(TEXT("Chat")));
		if(PanelChat)
		{
			PanelChat->ShowInputArea(FALSE);
			//MMOUI::RemoveTop(PanelChat); 
		}
	}


	if(g_rootGFXPlayer)
	{
		g_rootGFXPlayer->eventUCClearFocus();
	}
	
	FGFxEngine::GetEngine()->PlayerControlRequireReleaseEvt = !bRelease;

	// -------------------------------------------------------------------------------------------------------------------------------------------


	int Ret = 0;
	UIUICommand* cmd = NULL;
	AActor* TracePawnActor = NULL;
	AMMOActor* CP = NULL;

	UMMOAbility* ToUseAbility = FSMIO::getSingleton().Ctrl.GetLeftBtnBindAbility(bRelease);
	
	if(bRelease)//松开
	{
		bLeftMousePressed = false;
		PressAbilityKey(ToUseAbility,NULL, bRelease,TRUE);
	}
	else//按下
	{
		bLeftMousePressed = true;
//		NativeGetPickPointActorPreferred();
		FSMIO::getSingleton().Ctrl.SetKeyType(KT_PRIMARY);
		PressAbilityKey(ToUseAbility,TraceActor, bRelease,TRUE);
	}

}

AActor* AMMOPlayerController::GetSelectTarget()
{
	return SelectedActor;
}

void AMMOPlayerController::SetSelectTarget(AActor* Target)
{
	if(Target == NULL)
	{
		SelectedActor = NULL;
	}
	else
	{
		SelectedActor = Cast<AMMOActor>(Target);
	}

	//else 
	//{
	//	AMMOActor *tActor = Cast<AMMOActor>( Target ) ;
	//	if( tActor != NULL)
	//	{
	//		SelectedActor =	tActor;
	//		SendRequestSetFocus( SelectedActor->ServerID);
	//	}
	//	else
	//	{
	//		SelectedActor =	NULL;
	//	}
	//}

//	if(SelectedActor != NULL)
//	{
// 		if( !MyDecalManager )
// 		{
// 			MyDecalManager = Cast<AMMODecalManager>(GWorld->GetWorldInfo()->MyDecalManager);
// 		}
// 
// 		if( MyDecalManager )
// 		{
// 			AMMOActor* pMMOActor = Cast<AMMOActor>(Target);
// 			AMMOActor* player = Cast<AMMOActor>( Pawn );
// 			AMMODoodadActor* pDoodod = Cast<AMMODoodadActor>(pMMOActor);
// 			UBOOL bAttackable = MMOFacade::getSingleton().IsAttackable(player,pMMOActor);
// 			if(pDoodod && pDoodod->GetMouseCursor()==0)
// 			{
// 				//MyDecalManager->eventSelectActor(pMMOActor,ESDT_Default );
// 			}
// 			else if(bAttackable==FALSE)
// 			{
// 				MyDecalManager->eventSelectActor(pMMOActor,ESDT_Friend );
// 			}
// 			else
// 			{
// 				MyDecalManager->eventSelectActor(pMMOActor,ESDT_Enemy );
// 			}
// 		}
//	}
}


void AMMOPlayerController::SetGameKeyEventProcessState(const FString& InNewStateName,class UObject* InContextObject)
{
	check(GameKeyEventProcessor);
	GameKeyEventProcessor->GotoKeyProcessState(InNewStateName, InContextObject);
}

void AMMOPlayerController::SetCurrentMouseCursor(BYTE cursorType)
{
	MMOFacade::SetMouseCursor((EMMOCursorType)cursorType);
}

////技能升级功能开启
//void AMMOPlayerController::EnableSkillUp( UBOOL isEnable )
//{
//    UMMOUISkillPanel* skillPanel = ( UMMOUISkillPanel* )MMOUI::FindUIPanel ( "Skill" );
//    if( skillPanel )
//    {
//        skillPanel->EnableSkillUp( isEnable );
//    }
//}

//DumpMemoryUseStatus
void AMMOPlayerController::DumpMemoryUseStatus( )
{
#if STATS
    // Dumep it
    GStatManager.DumpMemoryUseStatus( );
#endif
}




void AMMOPlayerController::OnOperateDoodadBarStart(FString strTitle, INT IntervalTime)
{
	UMMOUIGFXPlayer*  HudMovie = Cast<AMMOHud>(myHUD)->HudMovie;
	FLOAT TotalTime = FLOAT(IntervalTime)/1000.f;
	MMOUI::GetUIManager()->ShowProgressBar(INT(TotalTime*1000.f), strTitle, 0);
	bIsOperatingDoodad=TRUE;
	//MMOUI::GetUIManager()->delegateShowProgressBar(INT(TotalTime*1000.f), strTitle, 0);
    //DoodadManager::getSingleton().PlayRoleAnimByDoodad(INT(TotalTime*1000.f));
}

void AMMOPlayerController::OnOperateDoodadBarBarEnd(UBOOL bBreak)
{
	UMMOUIGFXPlayer*  HudMovie = Cast<AMMOHud>(myHUD)->HudMovie;
	MMOUI::GetUIManager()->CloseProgressBar(INT(bBreak));
	bIsOperatingDoodad=FALSE;
	//MMOUI::GetUIManager()->delegateCloseProgressBar(INT(bBreak));
    //DoodadManager::getSingleton().StopRoleAnimByDoodad(bBreak ? FALSE : TRUE);
}

void AMMOPlayerController::NativeLogin(const FString& ip, INT port, const FString& account, const FString& pwd)
{
	CMessageHandler::GetInstance().Login(*ip, port, *account, *pwd);
}

void AMMOPlayerController::LoginSelectRole(INT roleid)
{
	CMessageHandler::GetInstance().Send_CharactorSelect(roleid);
}


void AMMOPlayerController::FocusOnInput(class UMMOUIBase* Panel)
{
	return;	// commented by zhaojun@snda

	/*
	if(Panel && Panel->GFXPlayer )
	{
		if (FGFxEngine::GetEngine()->GetFocusMovie(0) != Panel->GFXPlayer->pMovie)
		{
			FGFxEngine::GetEngine()->SetFocusMovie(, 0);

			if( InputPanel )
			{
				InputPanel->GFXPlayer->ClearCaptureKeys();
			}

			InputPanel = Panel;
		}
	}
	*/
}

void AMMOPlayerController::FocusOutInput( )
{
	return;
	/*
	UMMOUIBase* PanelSkilbar = MMOUI::FindUIPanel(TEXT("Skillbar"));
	if(PanelSkilbar && g_isInputChat)
	{
		//InputPanel->GFXPlayer->ClearCaptureKeys();
		InputPanel = NULL;

		FGFxEngine::GetEngine()->SetFocusMovie(PanelSkilbar->GFXPlayer->pMovie);
		g_isInputChat = FALSE;

		UMMOUIChat* Chat = Cast<UMMOUIChat>(MMOUI::FindUIPanel(TEXT("Chat")));
		
	}
	*/
}

void AMMOPlayerController::SetInTextInput( UBOOL InText )
{
	InTextInput = InText;
}

UBOOL AMMOPlayerController::GetInTextInput(  )
{
	return InTextInput;
}

void	AMMOPlayerController::FlushPressedKeys()
{
	PlayerInput->FlushPressedKeys();
}

void AMMOPlayerController::NativeOnEnterPressed()
{
	debugf(TEXT("---------------[AMMOPlayerController::NativeOnEnterPressed]"));

	// oscar@snda 移到KeyManager中
// 	// 判定当前是否是聊天输入模式，并进行切换，设置焦点
// 	UMMOUIBase* PanelChat = MMOUI::FindUIPanel(TEXT("Chat"));
// 	UMMOUIBase* PanelSkilbar = MMOUI::FindUIPanel(TEXT("Skillbar"));
// 	UMMOUIChat* Chat = Cast<UMMOUIChat>(MMOUI::FindUIPanel(TEXT("Chat")));
// 
// 	if(!PanelChat)
// 		return;
// 	//check(PanelChat && PanelChat->GFXPlayer && PanelChat->GFXPlayer->pMovie);
// 
// 	bool bFocusOnChat = (FGFxEngine::GetEngine()->GetFocusMovie(0) == PanelChat->GFXPlayer->pMovie);
// 
// 	if (InTextInput)
// 	{
// // 		if (bFocusOnChat)
// // 		{
// // 			TArray<struct FASValue> Args;
// // 			Chat->Invoke(TEXT("sendToGame"), Args);
// // 			Chat->ShowInputArea(FALSE);
// // 			//FGFxEngine::GetEngine()->SetFocusMovie(PanelSkilbar->GFXPlayer->pMovie);
// // 			MMOUI::RemoveTop(PanelChat); 
// // 			//PanelChat->GFXPlayer->ASFocusInTextInput(FALSE);	// 该句应有chat->OnFoucsLost()执行过来，这里暂时hack
// // 		}
// // 		else
// // 		{
// // 			//非聊天的输入框
// // 			// do nothing
// // 		}
// 	}
// 	else
// 	{
// 		TArray<struct FASValue> Args;
// 		Chat->ShowInputArea(TRUE);
// 		//FGFxEngine::GetEngine()->SetFocusMovie(PanelChat->GFXPlayer->pMovie);
// 		Chat->eventCallShowPrivateName();
// 		MMOUI::MoveToTop(PanelChat);
// 		Chat->Invoke(TEXT("SetTextInputFocus"),Args);	// 该句应有chat->OnFoucsGained()执行过来，这里暂时hack
// 	}
	
}

void AMMOPlayerController::NativePostBeginPlay()
{
    AMMOGame* game = Cast<AMMOGame>(GWorld->GetGameInfo());
    if( game )
    {
         MapName = game->DefaultLocalMapName;
    }
}

void AMMOPlayerController::ActiveRemoteEvent(FName eventName)
{
	MMO::ActiveKismetRemoteEvent(eventName);
}

void AMMOPlayerController::NativeTestMount(INT MountID)
{
	if(MountID>0)
	{
		CMessageHandler::GetInstance().Send_ActiveMount(MountID);
		CMessageHandler::GetInstance().Send_EquipMount(MountID);
	}
	else
	{
		CMessageHandler::GetInstance().Send_EquipMount(MountID);
	}
}

void AMMOPlayerController::NativeTestSomeThing()
{
	AMMOPlayer* player = Cast<AMMOPlayer>( Pawn );
	if(player){
		
		FVector vec = player->Rotation.Vector().SafeNormal();

		CMessageHandler::GetInstance().SendRoleMine(player->ServerLocation+vec*128.0f);
	}
}

extern void MMOUIWingPanel_Diag(UBOOL bFull);	// MMOUIWingPanel.cpp
void AMMOPlayerController::NativeTestShowPanelEx(const FString& strPanelName,UBOOL bShow)
{
	if(strPanelName == TEXT("UWDiag"))
	{
		// TestShowPanelEx UWDiag 1 : wing window diagnostics (tabs, IsPlayerHasWing, upgrade button); UWDiag 0 : additionally clicks the upgrade panel button
		MMOUIWingPanel_Diag(!bShow);
		return;
	}
	//MMOUI::GetUIManager()->ShowPanelEx(FName(*strPanelName),bShow,"");
	UMMOUIBase* UI = MMOUI::GetUIManager()->FindUIPanel(FName(*strPanelName));
	if(UI)
	{
		UI->SetVisible(bShow);// >Show(TRUE, displayPriority);
	}
	else if(bShow)
	{
		// panel not loaded yet (e.g. "TestShowPanelEx UnrealWing 1" for the wing panel): load its swf and show it
		MMOUI::GetUIManager()->ToggleShowPanelEx(FName(*strPanelName), "");
	}
}

void AMMOPlayerController::NativeTestCtrlType(int CTID)
{
	if(CTID>=GC_DIABLO3 && CTID<=GC_MIRFOREVER){
		MMOFacade::getSingleton().GCTType = GAME_CONTROL_TYPE(CTID);
	}
}

void AMMOPlayerController::NativeTestNeiGua(const FString& flagName,UBOOL bFlag)
{
	if(flagName==TEXT("MSFT"))
	{
		FSM::Ctrl.ctrlFlag_IgnoreShiftBtn = bFlag;
	}
	else if(flagName==TEXT("DDCS"))
	{
		FSM::Ctrl.ctrlFlag_DDCS = bFlag;
	}
	else if(flagName==TEXT("ZNBY"))
	{
		FSM::Ctrl.ctrlFlag_ZNBY = bFlag;
	}
	else if(flagName==TEXT("ZNSF"))
	{
		FSM::Ctrl.ctrlFlag_ZDZR = bFlag;
		FSM::Ctrl.ctrlFlag_ZDLH = bFlag;
		FSM::Ctrl.ctrlFlag_ZDYS = bFlag;
		FSM::Ctrl.ctrlFlag_ZDKD = bFlag;
	}
}

void AMMOPlayerController::NativeTestAttackAI(const FString& flagName,FLOAT fValue)
{
	if(flagName==TEXT("LAST"))
	{
		FSM::Ctrl.ctrlAttackAi_LastDelay = fValue;
	}
	if(flagName==TEXT("LOCK"))
	{
		FSM::Ctrl.ctrlAttackAi_LockTime = fValue;
	}
}

void AMMOPlayerController::NativeTestReputation()
{
	MMOFacade::getSingleton().ReadReputationTemp();

	MMOUI::GetUIManager()->delegateUpdateReputationData();

	//UMMOUICharacter* pCharactor = (UMMOUICharacter*)(MMOUI::GetUIManager()->FindUIPanel(MMOUI::GetUIManager()->Panel_Character));
	//if(pCharactor){
	//	pCharactor->InitReputationData();
	//	pCharactor->UpdateReputationData();
	//}
}

void AMMOPlayerController::NativeDeleteBagItem(INT slot1, INT slot2)
{
	for(INT i=slot1;i<=slot2;i++)
	{
		FClientItemInfo* item = MMOFacade::getSingleton().FindItem( E_CONSLOT_DEFAULT , i );
		if(item){
			UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate( item->m_nTemplateID );
			INT stack = item->GetCurStackCount(itemTemp);
			CMessageHandler::GetInstance().Send_DeleteItem( E_CONSLOT_DEFAULT , i , stack , item->m_nTemplateID );
		}
	}

}

void AMMOPlayerController::NativeTestDropIcon(INT TypeID)
{
	UGFxObject* dropIcon = NULL;
	if(MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer() != NULL){
		dropIcon = MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer()->eventCallGetDropIcon();//GetVariableObject(TEXT("root.DropIcon_mc"));
	}

	if(dropIcon)
	{
		dropIcon->SetInt(TEXT("IconType"),TypeID);
	}

}

void AMMOPlayerController::NativeTestEfficiency(INT nEfficiency)
{
	g_Efficiency = nEfficiency;
}

void AMMOPlayerController::NativeTestVersion()
{
	//MMOUI::GetUIManager()->delegateShowUnlockAOGString(TEXT("声望升级"));

	//MMOGameEvent_MarqueeNoticeInfo evt(  "1111111111111" , "#ffff00" , 3 , 20000 );
	//MMOUI::PostGameEvent( &evt);


	//MMOGameEvent_MarqueeNoticeInfo evt2( "2222222222222" , "#00ffff" , 3 , 20000 );
	//MMOUI::PostGameEvent( &evt2);	

	LoginBase::getSingleton().IsVersion2T = !LoginBase::getSingleton().IsVersion2T;
}

void AMMOPlayerController::NativeTestUniformSpeed()
{
	g_UniformSpeed = !g_UniformSpeed;
	if(g_UniformSpeed)
	{
		AnimFramesPerSecond = 30;
	}
	else
	{
		AnimFramesPerSecond = 30;
	}
}

void AMMOPlayerController::NativeTestBlock(INT BlockType, FLOAT BlockRadius)
{
	MMOFacade::getSingleton().BlockType = BlockType;
	MMOFacade::getSingleton().BlockRadius = BlockRadius;
}

//退出游戏 重新登录
extern void ShowSDOLoginDialogWnd(bool bForce);
void AMMOPlayerController::ReturnLogin()
{
    //直接加载地图 进入Loading状态
    if( EGameState_InGame == MMO::GetCurrentGameState() || EGameState_Loading == MMO::GetCurrentGameState())
    {
        MMO::ActiveGameState( EGameState_Loading );

        FString LocalMap = FFilename(GConfig->GetStr( TEXT("URL"), TEXT("LocalMap"),	GEngineIni )).GetBaseFilename();
        if(MMOFacade::getSingleton().GetMainController()->MapName != LocalMap )
        {
            MMOFacade::getSingleton().GetMainController()->eventTeleportToScene(
                LocalMap, FVector( 0, 0, 0 ), EGameState_Login );
        }
		else
		{
			MMO::ActiveGameState( EGameState_Login );
		}
    }
	else if(EGameState_Login == MMO::GetCurrentGameState())
	{
		if(MMOMsgBoxManager::getSingleton().GetMsgNumByType(EMsgboxType_Modal)==0)
		{
			ShowSDOLoginDialogWnd(true);
		}
	}
    else
    {
		FString LocalMap = FFilename(GConfig->GetStr(TEXT("URL"), TEXT("LocalMap"), GEngineIni)).GetBaseFilename();
		if (MMOFacade::getSingleton().GetMainController()->MapName != LocalMap)
		{
			MMOFacade::getSingleton().GetMainController()->eventTeleportToScene(
				LocalMap, FVector(0, 0, 0), EGameState_Login);
		}
		else
		{
			MMO::ActiveGameState(EGameState_Login);
		}
        //MMO::ActiveGameState( EGameState_Login );
    }
  
}

//退出游戏 重新选择人物 预留着 
void AMMOPlayerController::ReturnSelect()
{
    //直接加载地图 进入Loading状态
    MMO::ActiveGameState( EGameState_Loading );

    FString LocalMap = FFilename(GConfig->GetStr( TEXT("URL"), TEXT("LocalMap"),	GEngineIni )).GetBaseFilename();
    if(MMOFacade::getSingleton().GetMainController()->MapName != LocalMap )
    {
        MMOFacade::getSingleton().GetMainController()->eventTeleportToScene(
            LocalMap, FVector( 0, 0, 0 ), EGameState_Selecting );
    }

	MMOFacade::getSingleton().GetMMOBuffManager()->ClearAllBuff();
}


void AMMOPlayerController::TestDrop(INT itemTemplateID)
{
#if !SHIPPING_PC_GAME
	FVector dropPos = Pawn->Location;
	ConvertCoordC2N(dropPos);

	static INT g_dropID = -1;

	G2C_SYNC_DROP_LIST* dropMsg = (G2C_SYNC_DROP_LIST*)CMessageHandler::GetInstance().ms_MsgBuffer;
	dropMsg->byIsNew = true;
	dropMsg->astDrops[0].nTemplateID = itemTemplateID;
	dropMsg->astDrops[0].dwDropID = g_dropID--;
	dropMsg->astDrops[0].wDestX = dropPos.X;
	dropMsg->astDrops[0].wDestY = dropPos.Y;
	dropMsg->astDrops[0].wDestZ = dropPos.Z;
	dropMsg->wMsg = g2c_sync_drop_list;
	dropMsg->wSize = sizeof(G2C_SYNC_DROP_LIST) + sizeof(DROP_INFO);

	
	CMessageHandler::GetInstance().DebugParseMsg((const char*)dropMsg, dropMsg->wSize);
#endif
}

void AMMOPlayerController::TestDropMoney(INT moneyId, INT count)
{
#if !SHIPPING_PC_GAME
	FVector dropPos = Pawn->Location;
	ConvertCoordC2N(dropPos);

	static INT g_dropID = -1;

	G2C_SYNC_DROP_LIST* dropMsg = (G2C_SYNC_DROP_LIST*)CMessageHandler::GetInstance().ms_MsgBuffer;
	dropMsg->byIsNew = true;
	dropMsg->astDrops[0].nTemplateID = moneyId;
	dropMsg->astDrops[0].dwDropID = g_dropID--;
	dropMsg->astDrops[0].wDestX = dropPos.X;
	dropMsg->astDrops[0].wDestY = dropPos.Y;
	dropMsg->astDrops[0].wDestZ = dropPos.Z;
	dropMsg->astDrops[0].nCount = count;
	dropMsg->wMsg = g2c_sync_drop_list;
	dropMsg->wSize = sizeof(G2C_SYNC_DROP_LIST) + sizeof(DROP_INFO);


	CMessageHandler::GetInstance().DebugParseMsg((const char*)dropMsg, dropMsg->wSize);
#endif
}

void AMMOPlayerController::TestDropX(INT itemTemplateID,INT count,INT range)
{
	static INT g_dropID = -1000;

	G2C_SYNC_DROP_LIST* dropMsg = (G2C_SYNC_DROP_LIST*)CMessageHandler::GetInstance().ms_MsgBuffer;
	dropMsg->wMsg = g2c_sync_drop_list;
	dropMsg->wSize = sizeof(G2C_SYNC_DROP_LIST) + sizeof(DROP_INFO) * count;
	dropMsg->byIsNew = true;	

	for(INT i = 0; i < count; ++i)
	{
		INT xoff = RandRange(-range, range);
		INT yoff = RandRange(-range, range);
		FVector dropLoc = Pawn->Location;
		dropLoc.X += xoff;
		dropLoc.Y += yoff;

		ConvertCoordC2N(dropLoc);

		dropMsg->astDrops[i].nTemplateID = itemTemplateID;
		dropMsg->astDrops[i].dwDropID = g_dropID--;
		dropMsg->astDrops[i].wDestX = dropLoc.X;
		dropMsg->astDrops[i].wDestY = dropLoc.Y;
		dropMsg->astDrops[i].wDestZ = dropLoc.Z;
	}

	CMessageHandler::GetInstance().DebugParseMsg((const char*)dropMsg, dropMsg->wSize);

}



 void AMMOPlayerController::TestMatineeSubtitle(UBOOL show,const FString& subtitle)
 {
	 {
		 MMOGameEvent_MatineeSubtitle evts(subtitle, MMOGameEvent_MatineeSubtitle::EMSET_UpdateSubtitle);
		 MMOUI::PostGameEvent(&evts);
	 }

	 {
		 MMOGameEvent_MatineeSubtitle evt((bool)(show == TRUE), MMOGameEvent_MatineeSubtitle::EMSET_Show);
		 MMOUI::PostGameEvent(&evt);
	 }
 }

 void AMMOPlayerController::ReloadTween()
 {
	 MMOFacade::getSingleton().ReadTweenData();
 }

 void AMMOPlayerController::ReloadFreeTips()
 {
	 MMOFreeTipsUIManager::getSingleton().ReadFreeTipsConfig();
 }

 void AMMOPlayerController::UpdateCursor()
 {
	 UKeyEventProcess* CurrentState = GameKeyEventProcessor->GetActiveState();
	 if (CurrentState)
	 {
		 CurrentState->UpdateCursor();
	 }
 }

//#include "ClientSceneVariablesParse.h"
//#include "MMOEquipManager.h"
 void AMMOPlayerController::NativeTestFBTask( INT mapID, INT id, INT value )
 {
	 //G2C_SYNC_SCENE_VARIABLES::VARIABLE_DATA var;
	 //var.wVarIndex = (UINT16)id;
	 //var.nValue = (UINT32)value;
	 //CheckClientSceneVariableAction( mapID, var );
	 //MMOEquipManager::getSingleton().SetAllowedChangeSkills( value );
 }

 void AMMOPlayerController::TestGuideArrow(INT id, INT offsetX, INT offsetY)
 {
	 MMOGuideManager::getSingleton().TestGuideArrow(id, offsetX, offsetY);
 }

 void AMMOPlayerController::TestGuideBox(INT id, INT offsetX, INT offsetY, INT width, INT height)
 {
	 MMOGuideManager::getSingleton().TestGuideBox(id, offsetX, offsetY, width, height);
 }
 
 void AMMOPlayerController::TestEventSound()
 {
	 const UQuest* quest = MMOQuestData::getSingleton().GetQuestData(100);
	 if (quest)
	 {
		 USoundCue* questSoundSubmit = ((UQuest*)quest)->GetQuestSoundSubmit();
		 if (questSoundSubmit)
		 {
			 MMOFacade::getSingleton().GetMainActor()->PlayEventSound(questSoundSubmit);
		 }
	 }
 }

 void AMMOPlayerController::SetAnimReplayFPS(INT InFramesPerSecond)
 {
	 if (InFramesPerSecond <= 0)
	 {
		 warnf(TEXT("Can not set FramesPerSecond to <=0"));
	 }
	 else
	 {
		 AnimFramesPerSecond = InFramesPerSecond;
	 }

 }

UBOOL AMMOPlayerController::IsPlayingMatinee()
{
	TArray<USeqAct_Interp*> objList = MMO::FindObjectInKisemetByType<USeqAct_Interp>();

	for (INT i = 0; i < objList.Num(); ++i)
	{
		USeqAct_Interp* seqAct = objList(i);
		if (seqAct->bIsPlaying )
		{
			for (INT GroupIndex = 0; GroupIndex < seqAct->GroupInst.Num(); GroupIndex++)
			{
				// is the PC the group actor?
				if (seqAct->GroupInst(GroupIndex)->GetGroupActor() == this)
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

 UBOOL AMMOPlayerController::StopActiveMatinee()
 {
	 UBOOL bMatineeSkipped = FALSE;

	 TArray<USeqAct_Interp*> objList = MMO::FindObjectInKisemetByType<USeqAct_Interp>();

	// if so, look for all active matinees that has this Player in a director group
	 for (INT i = 0; i < objList.Num(); ++i)
	{
		 USeqAct_Interp* seqAct = objList(i);
		// isit currently playing (and skippable)?
		if (seqAct->bIsPlaying && seqAct->bIsSkippable)
		{
			USeqAct_Interp* sai = seqAct;
			for (INT GroupIndex = 0; GroupIndex < seqAct->GroupInst.Num(); GroupIndex++)
			{
				// is the PC the group actor?
				if (seqAct->GroupInst(GroupIndex)->GetGroupActor() == this)
				{
					const FLOAT RightBeforeEndTime = 0.1f;
					// make sure we aren';t already at the end (or before the allowed skip time)
					if ((seqAct->Position < seqAct->InterpData->InterpLength - RightBeforeEndTime) && (seqAct->Position >= 0.1))
					{
						// skip to end
						seqAct->SetPosition(seqAct->InterpData->InterpLength - RightBeforeEndTime, TRUE);

						seqAct->bCanceled = TRUE;	//fangruyi

						// send a callback that this was skipped
						GCallbackEvent->Send(CALLBACK_MatineeCanceled, seqAct);

						bMatineeSkipped = TRUE;

						extern FLOAT HACK_DelayAfterSkip;
						// for 2 seconds after actually skipping a matinee, don't allow savegame loadng
						HACK_DelayAfterSkip = 2.0f;
					}
				}
			}
		}
	}

	if(bMatineeSkipped && GWorld && GWorld->GetGameInfo())
	{
		GWorld->GetGameInfo()->eventMatineeCancelled();
	}

	MMOFacade::getSingleton().PauseBackSound(FALSE);
	return bMatineeSkipped;
 }

 void AMMOPlayerController::SetShowNpc(UBOOL bShow)
 {
	 bShowNpc = bShow;

	 for (TObjectIterator<ANPC> It; It; ++It)
	 {
		 //(*It)->SetHidden(!bShowNpc);
		 ANPC* npc = *It;
		 npc->SetVisibleByKismet(bShow);
	 }
 }
 void AMMOPlayerController::SetShowMainPlayer(UBOOL bShow)
 {
	 bShowMainPlayer = bShow;
	 //Pawn->SetHidden(!bShowMainPlayer);
	 AMMOPlayer* mmoPlayer = Cast<AMMOPlayer>(Pawn);
	 check(mmoPlayer);
	 mmoPlayer->SetVisibleByKismet(bShow);
 }
 void AMMOPlayerController::SetShowTheOtherPlayers(UBOOL bShow)
 {
	 bShowTheOtherPlayersByKismet = bShow;

	 //for (TObjectIterator<AMMOPlayer> It; It; ++It)
	 //{
		// if ((*It) != Pawn)
		// {
		//	 AMMOPlayer* player = *It;
		//	 player->SetVisibleByKismet(bShow);
		// }
	 //}
	 STLInt2IntMap* CharacterMap = MMOFacade::getSingleton().GetCharacterMap();
	 for (STLInt2IntMap::iterator pos = CharacterMap->begin(); pos != CharacterMap->end(); ++pos)
	 {
		 AMMOActor* pActor = MMOFacade::getSingleton().GetCharacter(pos->first);
		 if (pActor && pActor->IsA(AMMOPlayer::StaticClass()) && pActor != Pawn)
		 {
			 pActor->SetVisibleByKismet(bShow);
		 }
	 }
 }

 void AMMOPlayerController::SetShowTheOtherPlayersByHotKey(UBOOL bShow)
 {
	 

	 if(bShowTheOtherPlayersByHotKey!=bShow)
	 {
		 FString strKey;
		 UMMOKeyManager* keyManager = UMMOKeyManager::getSingleton();
		 if (!keyManager)
		 {
			 return;
		 }
		 UMMOKeyManager::KKeyworkd* pKKey = keyManager->FindKeyworld(UMMOKeyManager::KF_ShowHidePlayer, FALSE );
		 if(pKKey)
		 {
			 keyManager->KeyToString(pKKey,strKey);
		 }

		 if(!strKey.IsEmpty())
		 {
			 FString text = FString::Printf( TEXT("使用快捷键%s可以显示/隐藏其他玩家"), *(strKey) );
			 MMOChatManager::getSingleton().AddSystemMessage( text );
			 MMOUI::GetUIManager()->delegateAddScreenMessage( text ,MsgInfoType_Normal);
		 }
	 }

	 bShowTheOtherPlayersByHotKey = bShow;

	 STLInt2IntMap* CharacterMap = MMOFacade::getSingleton().GetCharacterMap();
	 for (STLInt2IntMap::iterator pos = CharacterMap->begin(); pos != CharacterMap->end(); ++pos)
	 {
		 AMMOActor* pActor = MMOFacade::getSingleton().GetCharacter(pos->first);
		 if (pActor && pActor->IsA(AMMOPlayer::StaticClass()) && pActor != Pawn)
		 {
			 pActor->SetVisibleByHotKey(bShow);
		 }
		 else if(pActor && pActor->GetActorType()==EACT_Pet)
		 {
			 pActor->SetVisibleByHotKey(bShow);
		 }
	 }
 }

 void AMMOPlayerController::SetShowDrops(UBOOL bShow)
 {
	 bShowDropsByKismet = bShow;

	 DropActorMap dropActorMap = MMODropManager::getSingleton().GetDropActorMap();
	 DropActorMap::iterator iter = dropActorMap.begin();
	 for (; iter != dropActorMap.end(); ++iter)
	 {
		 AMMODropActor* dropActor = iter->second.DropActor;
		 if (dropActor)
		 {
			 dropActor->SetHidden(!bShow);
		 }
	 }
 }

 void AMMOPlayerController::ClearOldSceneFlags()
 {
	 bShowMainPlayer = TRUE;
	 bShowNpc = TRUE;
	 bShowTheOtherPlayersByKismet = TRUE;
	 bShowDropsByKismet = TRUE;

	 SetShowMainPlayer(TRUE);
	 SetShowNpc(TRUE);
	 SetShowTheOtherPlayers(TRUE);
	 SetShowTheOtherPlayersByHotKey(TRUE);
	 SetShowDrops(TRUE);

	 g_disableAllInput = FALSE;

	 AMMOPlayer* mmoActor = Cast<AMMOPlayer>(Pawn);
	 check(mmoActor);
	 mmoActor->IsInMatinee = FALSE;

	 MMOUI::GetUIManager()->delegateHideTips();
 }

 void AMMOPlayerController::TestDropAll(INT dist)
 {
#if !SHIPPING_PC_GAME
	 MMODropManager::getSingleton().TestDropAll(dist);
#endif
 }
 
 extern FLOAT g_RPS;
 extern FLOAT g_RPSDeAcc;
 extern FLOAT g_ZGravity;
 extern FLOAT g_InitZSpeed;
 extern FLOAT g_SpawnDelay;

 void AMMOPlayerController::TestDropAnim(FLOAT g,FLOAT iniSpeed,FLOAT rps,FLOAT rpsAcc, FLOAT spawnDelay)
 {
#if !SHIPPING_PC_GAME
	 g_RPS = rps;
	 g_RPSDeAcc = rpsAcc;
	 g_ZGravity = g;
	 g_InitZSpeed = iniSpeed;
	 g_SpawnDelay = spawnDelay;
#endif
 }

 void AMMOPlayerController::TestDropSetParm(const FString& parName,const FString& parVal)
 {
#if !SHIPPING_PC_GAME
	 extern void ParseDropDbgParm(const FString& parName, const FString& parVal);
	 ParseDropDbgParm(parName, parVal);
#endif
 }


 void AMMOPlayerController::TestDoodad(INT doodadTemplateID,INT remainCnt)
 {
#if !SHIPPING_PC_GAME
	 static INT doodadActorID = -1;

	 const UDoodad* doodadTemp = DoodadManager::getSingleton().FindDoodadTemplate( doodadTemplateID );
	 if(!doodadTemp)
	 {
		 warnf(TEXT("Doodad template no find [%id]"), doodadTemplateID);
		 return;
	 }
	 
	 FVector doodadLocation = GetPlayer()->GetFootLocation();
	 if(doodadTemp->DoodadAvatar)
	 {
		 doodadLocation.Z += doodadTemp->DoodadAvatar->CollisionHeight * doodadTemp->DoodadAvatar->AvatarDrawScale;
	 }

	 DoodadManager::getSingleton().AddDoodadActor(doodadLocation, doodadTemp, FRotator(0,0,0), doodadActorID, remainCnt);
	 --doodadActorID;
#endif
 }



 void AMMOPlayerController::OnNavigationKey(BYTE KeyValue, UBOOL bRelease)
 {
	 if(GameKeyEventProcessor)
	 {
		 GameKeyEventProcessor->OnNavigationKey(KeyValue, bRelease);
	 }
 }

void AMMOPlayerController::SetHUDNameMoveSpeed(INT pix)
{
#if M_DEBUG_HUD_NAME_MOVE
	g_hudMoveSpeed = pix;
#endif
}

void AMMOPlayerController::NativePreRender(UCanvas* cav)
{
	if(g_tickOrder == ETO_BeforeCanvasRenderingTick)
	{
		_OnMMOTick(GDeltaTime);
		logTickOrderf(TEXT("ETO_BeforeCanvasRenderingTick"));
	}
}

void AMMOPlayerController::SetTickOrder(BYTE order)
{
	g_tickOrder = (ETickOrder)order;
}

void  AMMOPlayerController::NativeOnUp(UBOOL bReleased)
{
	if(bReleased)
	{

	}
}

void  AMMOPlayerController::NativeOnDown(UBOOL bReleased)
{
	if(bReleased)
	{

	}
}

//#include "MMOMapDataManager.h"
//#include "MMOArenaManager.h"
//#include "MMOChatManager.h"
void  AMMOPlayerController::NativeTest(INT testData)
{
	//FString MainPlayerName = MMOFacade::getSingleton().GetMainPlayer()->ActorName;
	//FString MsgContent = FString::Printf(TEXT("<#P0:<#PN:%s>><#R:%d><#T:MMOGame.DLG.ITEM.4>"),*MainPlayerName, testData);
	//MMOChatManager::getSingleton().ReceiveMessage(cltTvShow, 0, 0, MsgContent, TEXT(""), -1);

	//MMOGameEvent_OneInt syncGameLeftTimeEvt( GameEvent_SyncGameLeftTime, (INT32)testData );
	//MMOUI::PostGameEvent( &syncGameLeftTimeEvt );
	//CMessageHandler::GetInstance().Send_CombatPoseOn(bTest,0);
	//MMOGameEvent_OneInt evt( GameEvent_SceneFinished, MMOMapDataManager::getSingleton().GetCurMap().ServerID );
	//MMOUI::PostGameEvent(&evt);
	//MMOArenaManager::getSingleton().SetCurArenaState( (EArenaState)testData );
	//MMOMapDataManager::getSingleton().ActiveRevivePoint( GetPlayer()->Location );

	//MMOEquipManager &equipMgr = MMOEquipManager::getSingleton();
	//for( INT i = E_NES_MAIN_HAND; i < E_NES_TOTAL; ++i ){
	//	if( i == E_NES_AVATAR ){
	//		//暂时没有
	//		continue;
	//	}

	//	//其他
	//	FClientItemInfo* pItemInfo = MMOFacade::getSingleton().FindItem( E_CONSLOT_EQUIPPED , i );
	//	if( pItemInfo != NULL && testData == i ){
	//		pItemInfo->m_nCurDur = pItemInfo->m_nMaxDur * 0.2;
	//		pItemInfo->m_nMaxDur = 6;
	//		break;
	//	}
	//}
	//if( testData ){
	//	MMOMapDataManager::getSingleton().ActiveRevivePoint( GetPlayer()->Location );
	//}
	//else{
	//	MMOUI::GetUIManager()->delegateShowHintSceneWidget();
	//}
}

void AMMOPlayerController::NativeKismetRemoteEvent(const FString& eventName)
{
	MMO::ActiveKismetRemoteEvent(FName(*eventName));
}

void AMMOPlayerController::NativeAres(INT nIndex)
{
//	if(nIndex==0){
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 0 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 1 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 2 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 3 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 4 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 5 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 6 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 7 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 8 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 9 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 10 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 11 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 12 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 13 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 14 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 15 100000000") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddMoney 16 100000000") );
//
//		CMessageHandler::GetInstance().Send_BuyContainerSlot( E_CONSLOT_DEFAULT , 32 );
//		CMessageHandler::GetInstance().Send_BuyContainerSlot( E_CONSLOT_BANK , 56 );
//
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("Level 60") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("SetSpiritLevel 70") );
//
//	}
//	else if(nIndex==1){
//		MMOTargetManager::getSingleton().StartAres();
//	}
//	else if(nIndex==2){
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10109,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10119,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10129,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10209,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10219,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10229,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10309,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10319,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10329,60)") );
//
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10409,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10419,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10429,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10509,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10519,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10529,60)") );
//
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10909,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10919,60)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(10929,60)") );
//	}
//	else if(nIndex==3){
//
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(21001,297)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(21002,297)") );
//		CMessageHandler::GetInstance().Send_GMCommand( TEXT("AddItem(21003,297)") );
//	}
}
extern INT g_LastRequestAbilitySerialId;
extern INT g_LastRequestAbilityId;
void AMMOPlayerController::TestMyNewLogic()
{
	//MMOGameEvent_AddMsgBall evt( GameEvent_AddMsgBall , 1 , 2 );
	//MMOUI::PostGameEvent(&evt);
	static INT ID1=1035, Level1=3;
	static INT ID2=1431, Level2=3;


	if (FSM::Target.CurTargetMMOActor)
	{
		FSM::Target.AbilityTargetMMOActor = FSM::Target.CurTargetMMOActor;

		INT SerialID1 = RequestUseAbilityEx(0, ID1, Level1, FSM::Char.MainPlayer->ServerLocation);

		g_LastRequestAbilitySerialId = FSM::Char.MainPlayer->CurrentRequestAbilitySerialId;
		g_LastRequestAbilityId = FSM::Char.MainPlayer->CurrentRequestAbilityId;
		FSM::Char.MainPlayer->CurrentRequestAbilitySerialId = SerialID1;
		FSM::Char.MainPlayer->CurrentRequestAbilityId = ID1;

		INT SerialID2 = RequestUseAbilityEx(FSM::Target.CurTargetMMOActor->ServerID, ID2, Level2, FSM::Target.CurTargetMMOActor->ServerLocation);

		g_LastRequestAbilitySerialId = FSM::Char.MainPlayer->CurrentRequestAbilitySerialId;
		g_LastRequestAbilityId = FSM::Char.MainPlayer->CurrentRequestAbilityId;
		FSM::Char.MainPlayer->CurrentRequestAbilitySerialId = SerialID2;
		FSM::Char.MainPlayer->CurrentRequestAbilityId = ID2;

	}


}

void AMMOPlayerController::SetSmoothFrameRate(UBOOL on,FLOAT minFPS,FLOAT maxFPS)
{
	GEngine->bSmoothFrameRate = on;
	GEngine->MinSmoothedFrameRate = Clamp(minFPS, 5.0f, 200.0f);
	GEngine->MaxSmoothedFrameRate = Clamp(maxFPS, 5.0f, 200.0f);
	GEngine->SaveConfig();
}
#if !MMO_FINAL_SHIPPING

INT TheWarriorEquipForLevel40[] = {
	//50910141, //--剑盾
	50910241, //--巨剑
	//50910341, //--双斧
	50911141, //--头部
	50911241, //--肩部
	50911341, //--身体
	50911441, //--腿部
	50911541, //--腰部
	50911641, //--手腕
	50911741, //--脚
	50911841, //--披风
	//50952141, //--手镯
	//50952142, //--手镯
	//50952241, //--项链
	//50952341, //--戒指
	//50952342 //--戒指
};


INT TheAssassinEquipForLevel40[] = {
	//50920141,// --剑盾
	//50920241,// --巨剑
	50920341,// --双斧
	50921141,// --头部
	50921241,// --肩部
	50921341,// --身体
	50921441,// --腿部
	50921541,// --腰部
	50921641,// --手腕
	50921741,// --脚
	50921841,// --披风
	//50952141,// --手镯
	//50952142,// --手镯
	//50952241,// --项链
	//50952341,// --戒指
	//50952342 --戒指
};

INT TheArcherEquipForLevel40[] = {
	//50930141,// --双弩
	//50930241,// --重弩
	50930341,// --弓
	50931141,// --头部
	50931241,// --肩部
	50931341,// --身体
	50931441,// --腿部
	50931541,// --腰部
	50931641,// --手腕
	50931741,// --脚
	50931841,// --披风
	//50952141,// --手镯
	//50952142,// --手镯
	//50952241,// --项链
	//50952341,// --戒指
	//50952342 --戒指
};

INT TheMageEquipForLevel40[] = {
	//50940141,// --祭祀小刀
	//50940241,// --锤盾
	50940341,// --法杖
	50941141,// --头部
	50941241,// --肩部
	50941341,// --身体
	50941441,// --腿部
	50941541,// --腰部
	50941641,// --手腕
	50941741,// --脚
	50941841,// --披风
	//50952141,// --手镯
	//50952142,// --手镯
	//50952241,// --项链
	//50952341,// --戒指
	//50952342 --戒指
};

INT* testEquipArr[] =
{
	TheWarriorEquipForLevel40,
	TheMageEquipForLevel40,
	TheAssassinEquipForLevel40,
	TheArcherEquipForLevel40
};

#endif


void AMMOPlayerController::CreatePlayer(BYTE playerClass,BYTE gender,INT count,INT rangeOffset)
{
#if !MMO_FINAL_SHIPPING

	static INT playerID = 100;

	if(!(playerClass >= clsWarrior && playerClass <= clsArcher))
	{
		warnf(NAME_Warning, TEXT("playerClass must be one of 0, 1, 2, 3"));
		return;
	}

	if(gender != cstMale && gender != cstFemale)
	{
		warnf(NAME_Warning, TEXT("gender must be one of 1, 2"));
		return;
	}

	G2C_CHAR_APPEAR appear;
	G2C_SYNC_ROLE_DATA roleData;

	appMemzero(&appear, sizeof(G2C_CHAR_APPEAR));
	appMemzero(&roleData, sizeof(G2C_SYNC_ROLE_DATA));


	FVector playerPos = MMOFacade::getSingleton().GetMainActor()->Location;
	ConvertCoordC2N(playerPos);
	for (INT i = 0; i < count; ++i)
	{
		appear.wMsg = g2c_char_appear;
		appear.byAppearType = soatInRange;
		appear.stAppearData.dwCharID = MakeObjectID(otRole, playerID++);
		appear.stAppearData.wCurrentX = playerPos.X + RandRange(-rangeOffset, rangeOffset);
		appear.stAppearData.wCurrentY = playerPos.Y + RandRange(-rangeOffset, rangeOffset);
		appear.stAppearData.wCurrentZ = playerPos.Z;
		appear.stAppearData.wFaceDirection = RandRange(0, 0xFFFF);
		CMessageHandler::GetInstance().DebugParseMsg((const char*)&appear, sizeof(G2C_CHAR_APPEAR));


		roleData.wMsg = g2c_sync_role_data;
		roleData.dwRoleID = appear.stAppearData.dwCharID;
		roleData.bySex = gender;
		roleData.byClass = playerClass;
		roleData.nMaxHp = 1000;
		roleData.nMaxMp = 1000;
		roleData.byFaceType = 0;
		roleData.byHairColor = 0;
		roleData.byHairType = 0;
		roleData.bySkinColor = 0;

		FString playerName = FString::Printf(TEXT("client_%x"), roleData.dwRoleID);
		FTCHARToUTF8 szContent( *playerName);
		INT contentLen = appStrlen( szContent );
		strxcpy(roleData.szName, szContent, contentLen + 1);

		appMemcpy(roleData.anAvatar, testEquipArr[playerClass], NES_VISIBLE_TOTAL * sizeof(INT));			
		CMessageHandler::GetInstance().DebugParseMsg((const char*)&roleData, sizeof(G2C_SYNC_ROLE_DATA));
	}

#endif
}

void AMMOPlayerController::DrawNewGlobeFull()
{
	extern bool g_drawMaterialGlobe;
	g_drawMaterialGlobe = !g_drawMaterialGlobe;
}

void AMMOPlayerController::DrawNewGlobe()
{
	extern bool g_drawMaterialGlobeHalf;
	g_drawMaterialGlobeHalf = !g_drawMaterialGlobeHalf;

	MMOGameEvent_Any evt(GameEvent_ShowNewGlobe);
	MMOUI::PostGameEvent(&evt);
}

extern int g_pathRadiusLagMS;
extern int g_pickZOffset;

#if !SHIPPING_PC_GAME
extern bool g_drawDebugSMTrace;
#endif

void AMMOPlayerController::SetPathRadius(int lagMS)
{
	g_pathRadiusLagMS = lagMS;
}

void AMMOPlayerController::SetPathFindZOffset(int offset)
{
	g_pickZOffset = offset;
}

void AMMOPlayerController::ShowDebugPathTrace()
{
#if !SHIPPING_PC_GAME
	g_drawDebugSMTrace = !g_drawDebugSMTrace;
#endif
}

void AMMOPlayerController::SetMoveModGridSize(int sz)
{
	g_moveModGridSize = Max(1, sz);
}

void AMMOPlayerController::SetNetLagToleranceGridCount(int sz)
{
	g_netLagToleranceDisatnce = Max(1, sz);
}


void AMMOPlayerController::JumpProg()
{
	CMessageHandler::GetInstance().Send_GMCommand( TEXT("Jump 1 0 5400 5400") );
}

extern INT32 g_SendClearAbilityCacheTimes;
extern INT32 g_RecieveClearAbilityCacheTimes;
extern FLOAT g_UpdateClearAbilityCacheTime;
UBOOL AMMOPlayerController::IsCmdWaitingForClearCache()
{
	if(g_RecieveClearAbilityCacheTimes == g_SendClearAbilityCacheTimes)
	{
		return FALSE;
	}
	return TRUE;
}

void AMMOPlayerController::ExitGameWindow()
{
	ConsoleCommand(TEXT("exit"));
}

void AMMOPlayerController::OnReturnLoginConfirm()
{
	
	//_bReturnLogin = TRUE;
	INT GameState = MMO::GetCurrentGameState();
	if (GameState == EGameState_Selecting)
	{
		_bReturnLogin = FALSE;
		ReturnLogin();
	}
	else if(GameState == EGameState_Login)
	{
		_bReturnLogin = FALSE;
	}
	else
	{
		_bReturnLogin = TRUE;
	}

	//MMOMsgBoxManager::getSingleton().ClearAll();
	//g_disableAllInput = false;
	//OnLeaveScene();
	//ReturnLogin();
}

extern bool g_showTopmostElementPick;
void AMMOPlayerController::ShowTopmostPick()
{
#if !SHIPPING_PC_GAME
	g_showTopmostElementPick = !g_showTopmostElementPick;
#endif
}
extern FLOAT g_preLoadX;
extern FLOAT g_preLoadY;

extern bool g_enablePreLoad;

void AMMOPlayerController::PreJump(FLOAT x,FLOAT y)
{
	g_preLoadX = x;
	g_preLoadY = y;
}

void AMMOPlayerController::PreJumpNext()
{
	CMessageHandler::GetInstance().Send_GMCommand(
		*FString::Printf(TEXT("Jump %f %f"), g_preLoadX, g_preLoadY));
}

void AMMOPlayerController::PreJumpEnable()
{
	g_enablePreLoad = !g_enablePreLoad;
	debugf(TEXT("PreJumpEnable:"), (g_enablePreLoad ? TEXT("True") : TEXT("False")));
}

void AMMOPlayerController::TestDropIconPos(INT x,INT y)
{
	UGFxObject* obj = MMOUI::GetUIManager()->GetCommonUIComponentMoviePlayer()->GetDragIcon();
	if(obj)
	{
		obj->SetPosition(x, y);
	}
}

void AMMOPlayerController::DisWarn()
{
	GEngine->bSuppressMapWarnings = !GEngine->bSuppressMapWarnings;
}
void AMMOPlayerController::ZoomCameraIn()
{
	if(!M_MMO_HD)	
	{
		return;	
	}

	AMMOPlayerCamera* playerCam = Cast<AMMOPlayerCamera>(PlayerCamera);

	if(playerCam)
	{
		playerCam->ZoomIn();
	}
}


void AMMOPlayerController::ZoomCameraOut()
{
	if(!M_MMO_HD)
	{
		return;	
	}

	AMMOPlayerCamera* playerCam = Cast<AMMOPlayerCamera>(PlayerCamera);

	if(playerCam)
	{
		playerCam->ZoomOut();
	}
}

void AMMOPlayerController::Reload()
{
	MMOFacade::getSingleton().LoadStringMap();
}