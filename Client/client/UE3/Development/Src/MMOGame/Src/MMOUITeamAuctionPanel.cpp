#include "stdafx.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"
#include "CommonGameEvents.h"
#include "TeamManager.h"
#include "MMOGuildManager.h"
#include "MMOGuildData.h"
#include "UITextParser.h"

IMPLEMENT_CLASS(UMMOUITeamAuctionPanel)

#define AuctionCDTime 120

void UMMOUITeamAuctionPanel::Init()
{
	Super::Init();

	check(EventAdapter == NULL);
	TArray<EGameEvent> evts;
	evts.AddItem(GameEvent_TeamAuctionNewAuction);
	evts.AddItem(GameEvent_TeamAuctionEnd);
	evts.AddItem(GameEvent_TeamAuctionOtherBid);
	evts.AddItem(GameEvent_TeamAuctionNewItem);
	evts.AddItem(GameEvent_TeamLeave);

	EventAdapter = new TGameEventAdapter<UMMOUITeamAuctionPanel>(this, evts);
	
}

void UMMOUITeamAuctionPanel::UnInit()
{
	Super::UnInit();
}

void UMMOUITeamAuctionPanel::OnNativeGameEvent(struct MMOGameEvent* evt)
{
	switch( evt->GetEventType() )
	{		
	case GameEvent_TeamAuctionNewAuction:
		{
			//New auction
			MMOGameEvent_OneInt* msg = (MMOGameEvent_OneInt*)(evt);

			if( !GFXPlayer || !msg )
				break;

			TArray<MMOTeamAuctionItem>& itemList = TeamManager::getSingleton().GetTeamAuctionItemList();

			UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );

			MMOTeamAuctionItem* curAuctionItem = TeamManager::getSingleton().GetCurAuctionItem();

			if( data && curAuctionItem )
			{
				UItem* itemTemp = MMOFacade::getSingleton().FindItemTemplate( curAuctionItem->Item.m_nTemplateID );
				if(!itemTemp)
				{
					break;
				}

				data->SetString( TEXT("numAcution") , appItoa( curAuctionItem->RewardMemberCount ) );
				data->SetString( TEXT("nameAcution") , TEXT("") );
				data->SetString( TEXT("itemName") , UITextParser::HTMLFormatItemName(itemTemp) );
				data->SetString( TEXT("maxPrice") , UITextParser::HTMLMoneyIconString( E_MONEYTYPE_02 , TeamManager::getSingleton().GetCurBidMoney())/*appItoa( curAuctionItem->RewardMemberCount ) */);
				INT idx = TeamManager::getSingleton().GetTeamAuctionItemIdx(curAuctionItem->Index);
				data->SetString( TEXT("itemIndex") , appItoa( idx ) );
				data->SetInt( TEXT("curTime") , AuctionCDTime );
				data->SetInt( TEXT("tolTime") , AuctionCDTime );

				//
				UGFxObject* slotData = GFXPlayer->CreateObject(TEXT("ActionScript3.Slot.ItemSlotData"));
				if(slotData)
				{
					slotData->SetString(TEXT("SlotRenderClassName"),  TEXT("NormalItemSlotRender"));
				}			

				if( slotData )
				{
					slotData->SetInt( TEXT("slotType"), ESPType_TeamAuctionPanel );
					slotData->SetInt( TEXT("slotIndex"), 0 );
					slotData->SetBool( TEXT("HaveData"), TRUE );
					slotData->SetInt( TEXT("EquipID"), ESBarSlotTp_Item );										
					slotData->SetInt( TEXT("quantity"), 1 );
					FString cs = FString(TEXT("img://")) + MMOFacade::getSingleton().GetItemIconNamePersonalized( itemTemp );
					slotData->SetString( TEXT("iconName"), cs );
					slotData->SetInt( TEXT("itemTemplateID"), itemTemp->GetTemplateID() );
					slotData->SetInt( TEXT("itemColor"), itemTemp->ItemColor );

					data->SetObject( TEXT("slotData") , slotData );
				}
				//

				eventCallUpdatePanel(data);
				eventCallUpdateItemNum( appItoa( TeamManager::getSingleton().GetCanBidAuctionItemNum() ) );
				eventCallSetInputPrice(TeamManager::getSingleton().GetCurBidMoney());
			}
			
		}
		break;
	case GameEvent_TeamAuctionOtherBid:
		{
			MMOGameEvent_TeamAuctionOtherBid* msg = (MMOGameEvent_TeamAuctionOtherBid*)(evt);
			if(GFXPlayer)
			{
				UGFxObject* data = GFXPlayer->CreateObject( TEXT("Object") );
				if(data)
				{
					MMOTeamMember* teamMember = TeamManager::getSingleton().GetMemberById(msg->RoleId);
					PlayerBaseInfo *playerInfo = MMOFacade::getSingleton().RequestPlayerBaseInfo( msg->RoleId);		
					if(playerInfo)
					{
						data->SetString( TEXT("nameAcution") , playerInfo->_RoleName );
					}
					else if(teamMember)
					{
						data->SetString( TEXT("nameAcution") , teamMember->ActorName );
					}
					else
					{
						data->SetString( TEXT("nameAcution") , TEXT("") );
					}
					
					data->SetString( TEXT("maxPrice") , UITextParser::HTMLMoneyIconString( E_MONEYTYPE_02 , msg->BidMoney)/*appItoa(msg->BidMoney) */);
					data->SetInt( TEXT("curTime") , AuctionCDTime );
					data->SetInt( TEXT("tolTime") , AuctionCDTime );

					eventCallUpdatePanelFast(data);
				}
			}
		}
		break;
	case GameEvent_TeamAuctionNewItem:
		{
			//Change the number on the button
			eventCallUpdateItemNum( appItoa( TeamManager::getSingleton().GetCanBidAuctionItemNum() ) );
		}
		break;
	case GameEvent_TeamAuctionEnd:
		{
			
		}
		break;
	case GameEvent_TeamLeave:
		{
			this->SetVisible(FALSE);
		}
		break;

	}
}

void UMMOUITeamAuctionPanel::AuctionBid( INT bidMoney )
{
	SET_WARN_COLOR(COLOR_CYAN);
	warnf( NAME_Log, TEXT("[Team AuctionBid Money:%d]"), bidMoney );
	CLEAR_WARN_COLOR();

	INT minBidMoney = TeamManager::getSingleton().GetMinBidMoney();
	MMOTeamAuctionItem* item = TeamManager::getSingleton().GetCurAuctionItem();
	if(!item)
	{
		return;
	}
	/*INT nCount = TeamManager::getSingleton().GetTeamMemberCount();
	if(nCount == 1)
	{
	MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamMessage.OneCannotAuctionTeam")),MsgInfoType_Table,TEXT("MMOGame.TeamMessage.OneCannotAuctionTeam"));
	return;
	}*/

	if( bidMoney < minBidMoney )
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamAuction.BidToLow")) , MsgInfoType_Table , TEXT("MMOGame.TeamAuction.BidToLow") );
		eventCallSetInputPrice(minBidMoney);
	}
	else
	{
		INT nGoldMoney = MMOFacade::getSingleton().GetPlayerMoney(E_MONEYTYPE_02);
		if(bidMoney > nGoldMoney)
		{
			MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamMessage.AuctionNoMoneyBid")) , MsgInfoType_Table , TEXT("MMOGame.TeamMessage.AuctionNoMoneyBid") );
		}
		else
			CMessageHandler::GetInstance().Send_TeamAuctionBid( item->Item.m_nTemplateID , bidMoney , item->Index );
	}
}

//void UMMOUITeamAuctionPanel::AuctionGiveupCurItem()
//{
//	MMOTeamAuctionItem* item = TeamManager::getSingleton().GetCurAuctionItem();
//	if(!item)
//	{
//		return;
//	}
//	CMessageHandler::GetInstance().Send_TeamAuctionAbandonBid( item->Item.m_nTemplateID , item->Index );
//}

void UMMOUITeamAuctionPanel::AuctionBidUp(INT bidMoney)
{
	INT minBidMoney = TeamManager::getSingleton().GetMinBidMoney();
	INT newBid = bidMoney + TeamManager::getSingleton().GetAddBidMoney();
	MMOTeamAuctionItem* item = TeamManager::getSingleton().GetCurAuctionItem();
	if(!item)
	{
		return;
	}

	if( newBid < minBidMoney )
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamAuction.BidToLow")) , MsgInfoType_Table , TEXT("MMOGame.TeamAuction.BidToLow") );
		eventCallSetInputPrice(minBidMoney);
	}
	else
	{
		eventCallSetInputPrice( newBid );
	}
}

void UMMOUITeamAuctionPanel::AuctionBidDown(INT bidMoney)
{
	INT minBidMoney = TeamManager::getSingleton().GetMinBidMoney();
	INT newBid = bidMoney - TeamManager::getSingleton().GetAddBidMoney();
	MMOTeamAuctionItem* item = TeamManager::getSingleton().GetCurAuctionItem();
	if(!item)
	{
		return;
	}

	if( newBid < minBidMoney )
	{
		MMOUI::GetUIManager()->delegateAddScreenMessage( MMOUI::ParseLocalizedPropertyPath(TEXT("MMOGame.TeamAuction.BidToLow")) , MsgInfoType_Table , TEXT("MMOGame.TeamAuction.BidToLow") );
		eventCallSetInputPrice(minBidMoney);
	}
	else
	{
		eventCallSetInputPrice( newBid );
	}
}

void UMMOUITeamAuctionPanel::ASTeamAuctionListGiveUp( INT index )
{
	TeamManager::getSingleton().RequestGiveUp( index );
}

void UMMOUITeamAuctionPanel::ASTeamAuctionBid( INT bidMoney )
{
	AuctionBid(bidMoney);
}
void UMMOUITeamAuctionPanel::ASTeamAuctionGiveup()
{
	TeamManager::getSingleton().RequestGiveUpCurrentItem();
}
void UMMOUITeamAuctionPanel::ASTeamAuctionClickItemList()
{
	MMOUI::GetUIManager()->ToggleShowPanelEx( MMOUI::GetUIManager()->Panel_TeamAuctionList , TEXT("") );
}

void UMMOUITeamAuctionPanel::ASTeamAuctionClickBidMoneyUp( INT bidMoney )
{
	AuctionBidUp(bidMoney);
}

void UMMOUITeamAuctionPanel::ASTeamAuctionClickBidMoneyDown( INT bidMoney )
{

	AuctionBidDown(bidMoney);
}