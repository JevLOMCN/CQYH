#include "stdafx.h"
#include "message_handler.h"
#include "DoodadManager.h"
#include "FSMIO.h"

void CMessageHandler::Parse_g2c_sync_doodad_list(const char* buf, UINT32 size)
{
	G2C_SYNC_DOODAD_LIST* msg = (G2C_SYNC_DOODAD_LIST*)buf;
	DOODAD_APPEAR_DATA *pDoodad;
	INT byDoodadCount = (size - sizeof(G2C_SYNC_DOODAD_LIST)) / sizeof(msg->astAppearData[0]);

	AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
	if(player==NULL)
	{
		warnf( TEXT("[CMessageHandler::Parse_g2c_sync_doodad_list] :GetAMMOPlayer failed !") );
		return;
	}

	for(int i = 0; i < byDoodadCount; ++i)
	{
		pDoodad = &msg->astAppearData[i];
		//Convert from server coordinates to local coordinates
		FVector vDoodadLocation( pDoodad->wCurrentX, pDoodad->wCurrentY, pDoodad->wCurrentZ );
		FRotator vDoodadRoyaw(0, ConvertToClientDegree(pDoodad->wFaceDirection), 0);

		ConvertCoordN2C( vDoodadLocation );
		const UDoodad* Doodad = DoodadManager::getSingleton().FindDoodadTemplate( pDoodad->nTemplateID );
		if(!Doodad)
		{
			warnf(TEXT("Doodad template no find [%id]"), pDoodad->nTemplateID);
			return;
		}

		if(Doodad->DoodadAvatar)
		{
			vDoodadLocation.Z += Doodad->DoodadAvatar->CollisionHeight*Doodad->DoodadAvatar->AvatarDrawScale;
		}

		//warnf(NAME_Log, TEXT("@@@@@@@@@@@ AMMODoodadActor::g2c_sync_doodad ID[%d] TID[%d]"), pDoodad->dwDoodadID, pDoodad->nTemplateID);
		DoodadManager::getSingleton().AddDoodadActor(
			vDoodadLocation, (UDoodad*)Doodad, vDoodadRoyaw, pDoodad->dwDoodadID, pDoodad->byRemainOpenCount);

		//ctrl->eventAddDoodad(/*true,*/ vDoodadLocation, Doodad,pDoodad->dwDoodadID);

		//MMOFacade::getSingleton().CreateDoodad(pDoodad->nTemplateID, vDoodadLocation, vDoodadRoyaw);
	}

	DoodadManager::getSingleton().RefreshDoodadEffects();
}

void CMessageHandler::Parse_g2c_doodad_appear(const char* buf, UINT32 size)
{

	G2C_DOODAD_APPEAR* msg = (G2C_DOODAD_APPEAR*)buf;

	AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
	if(player==NULL)
	{
		warnf( TEXT("[CMessageHandler::Parse_g2c_sync_doodad_list] :GetAMMOPlayer failed !") );
		return;
	}

	DOODAD_APPEAR_DATA *pDoodad = &msg->AppearData;
	//Convert from server coordinates to local coordinates
	FVector vDoodadLocation( pDoodad->wCurrentX, pDoodad->wCurrentY, pDoodad->wCurrentZ );
	FRotator vDoodadRoyaw(0, ConvertToClientDegree(pDoodad->wFaceDirection), 0);

	ConvertCoordN2C( vDoodadLocation );
	const UDoodad* Doodad = DoodadManager::getSingleton().FindDoodadTemplate( pDoodad->nTemplateID );
	if(!Doodad)
	{
		warnf(TEXT("Doodad template no find [%id]"), pDoodad->nTemplateID);
		return;
	}

	if(Doodad->DoodadAvatar)
	{
		vDoodadLocation.Z += Doodad->DoodadAvatar->CollisionHeight*Doodad->DoodadAvatar->AvatarDrawScale;
	}

	//warnf(NAME_Log, TEXT("@@@@@@@@@@@ AMMODoodadActor::g2c_sync_doodad ID[%d] TID[%d]"), pDoodad->dwDoodadID, pDoodad->nTemplateID);
	AMMODoodadActor* NewDoodad = DoodadManager::getSingleton().AddDoodadActor(vDoodadLocation, (UDoodad*)Doodad, vDoodadRoyaw, pDoodad->dwDoodadID, pDoodad->byRemainOpenCount);

	DoodadManager::getSingleton().RefreshDoodadEffects();

	if (NewDoodad && NewDoodad->DoodadTemplate && NewDoodad->DoodadTemplate->OnSpawnEffectList.Num() > 0)
	{
		NewDoodad->PlayActions(NewDoodad->DoodadTemplate->OnSpawnEffectList);
	}
}

void CMessageHandler::Parse_g2c_operate_doodad_begin(const char* buf, UINT32 size)
{
	G2C_OPERATE_DOODAD_BEGIN* msg = (G2C_OPERATE_DOODAD_BEGIN*)buf;
	//UINT32          dwDoodadID;
	//UINT16          wDuration;

	INT Duration = msg->wDuration/GAME_FPS * 1000;
	if (MMOFacade::getSingleton().GetMainPlayer()->ServerID == msg->dwCharID)
	{
		AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		check(pc);
		//pc->OnOperateDoodadBarStart(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Doodad.Doodading")),Duration );

		// rotate to the doodad actor
		AMMODoodadActor* findActor = DoodadManager::getSingleton().FindDoodadActor(msg->dwDoodadID);
		if (findActor)
		{
			if(findActor->DoodadTemplate->DoodadType != E_dt_Corpse)
			{
				pc->OnOperateDoodadBarStart(MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.Doodad.Doodading")),Duration );
			}

			FRotator NewRot = (findActor->Location - pc->Pawn->Location).Rotation();
			NewRot.Pitch = NewRot.Roll = 0;
			pc->Pawn->SetDesiredRotation(NewRot, FALSE, FALSE, -1.0, TRUE);
		}
	}

	DoodadManager::getSingleton().OnDoodadBeginOperate(msg->dwDoodadID, msg->dwCharID, Duration);

	//DoodadManager::getSingleton().PlayRoleAnimByDoodad(Duration);
}

void CMessageHandler::Parse_g2c_operate_doodad_end(const char* buf, UINT32 size)
{
	G2C_OPERATE_DOODAD_END* msg = (G2C_OPERATE_DOODAD_END*)buf;
	//UINT32          dwDoodadID;
	//UINT8           byIsBreak;
	if (MMOFacade::getSingleton().GetMainPlayer()->ServerID == msg->dwCharID)
	{
		AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
		check(pc);
		pc->OnOperateDoodadBarBarEnd(msg->byIsBreak);
	}

	DoodadManager::getSingleton().OnDoodadEndOperate(msg->dwDoodadID, msg->byIsBreak);

	//DoodadManager::getSingleton().StopRoleAnimByDoodad(msg->byIsBreak ? FALSE : TRUE);
}

void CMessageHandler::Parse_g2c_sync_doodad_open_count(const char* buf, UINT32 size)
{
	G2C_SYNC_DOODAD_OPEN_COUNT* msg = (G2C_SYNC_DOODAD_OPEN_COUNT*)buf;
	DoodadManager::getSingleton().UpdateDoodadOperateCount(msg->dwDoodadID, msg->byOpenCount, msg->dwCharID);
}

void CMessageHandler::Send_PickDoodadItem( INT DoodadID )
{
#if !SHIPPING_PC_GAME
	if(DoodadID < 0)
	{
		AMMODoodadActor* doodadActor = DoodadManager::getSingleton().FindDoodadActor(DoodadID);
		if(doodadActor)
		{
			INT remainOpenCnt = doodadActor->GetRemainOpenCount();
			if(remainOpenCnt > 0)
			{
				G2C_SYNC_DOODAD_OPEN_COUNT msg;
				msg.wMsg = g2c_sync_doodad_open_count;
				msg.byOpenCount = remainOpenCnt - 1;
				msg.dwDoodadID = DoodadID;
				Parse_g2c_sync_doodad_open_count((const char*)&msg, sizeof(G2C_SYNC_DOODAD_OPEN_COUNT));
			}
			else
			{
				class AMMODoodadActor* doodadActor = DoodadManager::getSingleton().FindDoodadActor(DoodadID);
				if(doodadActor)
				{
					//MMOTargetManager::getSingleton().ClearActor(doodadActor);
					FSMIO::getSingleton().Target.ClearActor(doodadActor);
					DoodadManager::getSingleton().DestroyDoodadActor(DoodadID, TRUE, sodtLeaveScene);
				}
			}
		}
		return;
	}
#endif

	C2G_OPERATE_DOODAD msg;
	msg.wMsg = c2g_operate_doodad;
	msg.dwDoodadID = DoodadID;
	_Send(&msg, sizeof(msg));
}
