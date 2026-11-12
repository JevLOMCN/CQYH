#include "stdafx.h"
#include "message_handler.h"
#include "MMOFacade.h"
#include "LinkManager.h"
#include "MMODropManager.h"
#include "MMOChatManager.h"

using namespace ExternalMessage;

//Start dropped item pickup related-----
void CMessageHandler::Send_PickDropItem(INT	DropID,UINT8 iMode )
{
    AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
    if( player && player->bDead )
    {
        //Cannot pick up after death
        return;
    }

    C2G_PICKUP_ITEM msg;
	msg.wMsg = c2g_pickup_item;
	msg.dwDropID = DropID;
	msg.byPickMode = iMode;

#if !SHIPPING_PC_GAME
	AMMODropActor* actor = MMODropManager::getSingleton().FindDropActor(DropID);
	if(actor)
	{
		FVector loc = actor->Location;
		ConvertCoordC2N(loc);

		SET_WARN_COLOR(COLOR_PURPLE);
		warnf(NAME_Log, TEXT("c2g_pickup_item ID:%d [%s] Client:%s Server:%s Player:%s"), DropID,
			(actor->ItemTemplate ? *actor->ItemTemplate->ItemName : TEXT("Unknown")),
			*actor->Location.ToString(),
			*loc.ToString(),
			*MMOFacade::getSingleton().GetMainActor()->Location.ToString());
		CLEAR_WARN_COLOR();
	}
#endif

	_Send(&msg, sizeof(msg));
}

void CMessageHandler::Parse_g2c_sync_drop_list(const char* buf, UINT32 size)
{
	G2C_SYNC_DROP_LIST* msg = (G2C_SYNC_DROP_LIST*)buf;
    INT byCount = (size - sizeof(G2C_SYNC_DROP_LIST)) / sizeof(msg->astDrops[0]);

	AMMOPlayerController* ctrl = MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
	if(player == NULL)
	{
		warnf( TEXT("[CMessageHandler::Parse_g2c_sync_drop_list] :GetAMMOPlayer failed !") );
		return;
	}

	for(INT i = 0; i < byCount; ++i)
	{
		DROP_INFO* pDropItem = &msg->astDrops[i];

		//Convert from server coordinates to local coordinates
		FVector vDropLocation(pDropItem->wDestX, pDropItem->wDestY, pDropItem->wDestZ);
		ConvertCoordN2C(vDropLocation);

		MMODropManager::getSingleton().AddDropActor(
			false, vDropLocation, vDropLocation, pDropItem->nTemplateID, pDropItem->nCount, pDropItem->dwDropID, true, i);
		
// 		UItem* item = MMOFacade::getSingleton().FindItemTemplate(pDropItem->nTemplateID);
// 		if(!item)
// 		{
// 			warnf(NAME_Error, TEXT("Item not find [%d]"), pDropItem->nTemplateID);
// 			return;
// 		}

		//ctrl->eventAddDropItem(msg->byIsNew, vDropLocation, item, pDropItem->dwDropID );
	}
		
}

void CMessageHandler::Parse_g2c_generate_drop_from_object(const char* buf, UINT32 size)
{
	G2C_GENERATE_DROP_FROM_OBJECT* msg = (G2C_GENERATE_DROP_FROM_OBJECT*)buf;
	DROP_INFO* pDropItem = &msg->drop;

	//Convert from server coordinates to local coordinates
	FVector vDropLocation(pDropItem->wDestX, pDropItem->wDestY, pDropItem->wDestZ);
	ConvertCoordN2C(vDropLocation);

	FVector spawnLoc = vDropLocation;
	AActor* actor = MMOFacade::getSingleton().GetActor(msg->dwSrcID);
	if(!actor)
	{
		warnf(NAME_Warning, TEXT("generate_drop_from_object not found src actor:%d"), msg->dwSrcID);
	}
	else
	{
		//spawnLoc = actor->Location;				 // todo do not comment out, there will be a BUG	#15007
	}

	MMODropManager::getSingleton().AddDropActor(
		true, spawnLoc, vDropLocation, pDropItem->nTemplateID, pDropItem->nCount, pDropItem->dwDropID, true, GetDropCounterFromID(msg->dwSrcID));
}

void CMessageHandler::TickDropAnim(FLOAT deltaTime)
{
	_tickDropAnimTime += deltaTime;
	if(_tickDropAnimTime > C_ClearDropAnimCounterInterval)
	{
		_tickDropAnimTime = 0;

		_dropIDCounterMap.clear();
	}
}

INT CMessageHandler::GetDropCounterFromID(INT srcID)
{
	DropActorSrcIDCounterMap::iterator it = _dropIDCounterMap.find(srcID);
	if(it != _dropIDCounterMap.end())
	{
		++it->second.Counter;
	}
	else
	{
		FDropIDCounter counterObj;
		counterObj.Counter = 0;
		counterObj.LifeSpan = GWorld->GetTimeSeconds();
		it = _dropIDCounterMap.insert(std::make_pair(srcID, counterObj)).first;
	}
	return it->second.Counter;
}

void CMessageHandler::Parse_g2c_pickup_item(const char* buf, UINT32 size)
{
	G2C_PICKUP_ITEM* msg = (G2C_PICKUP_ITEM*)buf;
	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* player = Cast<AMMOPlayer>( MMOFacade::getSingleton().GetCharacter( msg->dwRoleID ) );
	if( !player || !pc )
	{
		debugf(NAME_Error, TEXT("[CMessageHandler::Parse_g2c_pickup_item] can't find player"));
		return;
	}
	
	ItemInfo iteminfo;
    iteminfo.Load(msg->acItem);

	UItem* tItem = MMOFacade::getSingleton().FindItemTemplate( iteminfo.m_nTemplateID );
	if( !tItem )
	{
		debugf(NAME_Error, TEXT("[CMessageHandler::Parse_g2c_pickup_item] can't find item"));
		return;
	}

	FClientItemInfo clientItemInfo;
	memset(&clientItemInfo, 0, sizeof(FClientItemInfo));
	memcpy(&clientItemInfo, &iteminfo, sizeof(ItemInfo));

	 FString chatMsg;

	//Player's personal hint
	if( player->IsMainPlayer() )
	{
		/*
        //Bag shows a hint when obtaining new items
        FString sMsg ;
		sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.PickUpInfoText" )),
			*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.YouText" )),*tItem->GetDisplayName(),1);
		MMOChatManager::getSingleton().AddSystemMessage( sMsg );*/

		//Check quality to decide whether to pop up a small window
		/*if( tItem->ItemColor >= pc->ColorAboveAsTreasure )
		pHudMovie->AddPickItemInfo( itemInfo->nTemplateID );*/
		/*	chatMsg = FString::Printf(TEXT("%s%s x%d"),
		*MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.ItemTips.ObtainNew")),
		*LinkManager::HtmlLink_ColorItem(tItem, &clientItemInfo),
		1 );*/
	}
	else
	{
       
		chatMsg = FString::Printf( *MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.GamePlay.PickUpInfoText")),
			*player->ActorName,
			*LinkManager::HtmlLink_ColorItem(tItem, &clientItemInfo),
			1 );
		MMOChatManager::getSingleton().AddSystemMessage( chatMsg );
		/*FString sMsg ;
		sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.PickUpInfoText" )),
			*player->ActorName,*tItem->GetDisplayName(),1 );
		MMOChatManager::getSingleton().AddSystemMessage( sMsg ); */
		
	}
	
}

void CMessageHandler::Parse_g2c_pickup_money(const char* buf, UINT32 size)
{
	G2C_PICKUP_MONEY* msg = (G2C_PICKUP_MONEY*)buf;
	AMMOPlayerController* pc = MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* player = MMOFacade::getSingleton().GetMainPlayer();
	if( !player || !pc )
	{
		debugf(NAME_Error, TEXT("[CMessageHandler::Parse_g2c_pickup_item] can't find player"));
		return;
	}
	UItem* tItem = MMOFacade::getSingleton().FindItemTemplate( msg->nMoneyType );
	if( !tItem )
	{
		debugf(NAME_Error, TEXT("[CMessageHandler::Parse_g2c_pickup_item] can't find item"));
		return;
	}

	//Amount of money obtained, add Msg info
	AMMOHud* hud = MMOFacade::getSingleton().GetMyHud();
	if( !hud )
		return;
	UMMOUIGFXPlayer* pHudMovie = hud->HudMovie;
	if( !pHudMovie )
		return;

	//Player's personal hint
	FString sMsg ;
	sMsg = FString::Printf(*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.PickUpInfoText" )),
		*MMOUI::ParseLocalizedPropertyPath( TEXT("MMOGame.GamePlay.YouText" )), *tItem->GetDisplayName(), msg->nCount );
	MMOChatManager::getSingleton().AddSystemMessage( sMsg );

}

/*
void CMessageHandler::Parse_g2c_drop_disappear(const char* buf, UINT32 size)
{
	G2C_DROP_DISAPPEAR* msg = (G2C_DROP_DISAPPEAR*)buf;
	AMMOPlayerController* ctrl=MMOFacade::getSingleton().GetMainController();
	AMMOPlayer* player = Cast<AMMOPlayer>(ctrl->Pawn);
	if(player==NULL)
	{
		warnf( TEXT("[CMessageHandler::Parse_g2c_sync_drop_list] :GetAMMOPlayer failed !") );
		return;
	}
	player->eventDestroyDropItem(msg->dwDropID);

}

void CMessageHandler::Parse_g2c_drop_need_roll(const char* buf, UINT32 size)
{
	G2C_DROP_NEED_ROLL* msg = (G2C_DROP_NEED_ROLL*)buf;
}

void CMessageHandler::Parse_g2c_drop_roll_result(const char* buf, UINT32 size)
{
	G2C_DROP_ROLL_RESULT* msg = (G2C_DROP_ROLL_RESULT*)buf;
}*/
//End dropped item pickup related-----